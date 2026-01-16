/***************************************************************************
                          timetablegenerateform.cpp  -  description
                             -------------------
    begin                : Tue Apr 22 2003
    copyright            : (C) 2003 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "generate.h"
#include "generate_pre.h"

#include "timetablegenerateform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "timetableexport.h"

#include "rules.h"

#include <QString>
#include <QStringList>

#include <QTextEdit>

//#include <QDateTime>
//#include <QLocale>

#include <QMessageBox>

//#include <QMutex>

#include <QDir>

#include <QApplication>
#include <QCoreApplication>
#include <QtGlobal>

#include <QProcess>
//#include <QTimer>

#include <QFileInfo>

#include <QFont>

#include "longtextmessagebox.h"

#include <thread>
#include <mutex>
//#include <condition_variable>

//#include <QSemaphore>

//QMutex myMutex;

//It might be safer to not make this variable static - see the comment for this in timetablegeneratemultipleform.cpp
//static GenerateThread generateThread;

//QSemaphore semaphorePlacedActivity; //used to update when an activity is placed

//QSemaphore finishedSemaphore;

//Represents the current status of the generation - running or stopped.
extern bool generation_running;

//extern bool students_schedule_ready;
//extern bool teachers_schedule_ready;
//extern bool rooms_buildings_schedule_ready;

//extern Solution best_solution;

//extern Solution highestStageSolution;

extern QString conflictsStringTitle;
extern QString conflictsString;

Generate gen;

QString initialOrderOfActivities;

QString getActivityDetailedDescription(Rules& r, int id);

const QString settingsName=QString("TimetableGenerateUnsuccessfulForm");

//static std::thread singleThread;

/*void GenerateThread::run()
{
	const int INF=2000000000;
	bool impossible, timeExceeded;

	gen.generate(INF, impossible, timeExceeded, true); //true means threaded
}*/

void runSingle()
{
	const int INF=2000000000;
	bool restarted, impossible, timeExceeded;

	gen.semaphorePlacedActivity.tryAcquire(); //this has effect after stopping the generation or if the timetable is impossible to solve
	assert(gen.semaphorePlacedActivity.available()==0);
	assert(gen.semaphoreFinished.available()==0);
	gen.generateWithSemaphore(INF, restarted, impossible, timeExceeded, true); //true means threaded
}

TimetableGenerateForm::TimetableGenerateForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	QFont font(pausedLabel->font());
	font.setBold(true);
	pausedLabel->setFont(font);

	currentResultsTextEdit->setReadOnly(true);

	connect(startPushButton, &QPushButton::clicked, this, &TimetableGenerateForm::start);
	connect(stopPushButton, &QPushButton::clicked, this, &TimetableGenerateForm::stop);
	connect(pausePushButton, &QPushButton::clicked, this, &TimetableGenerateForm::pause);
	connect(writeResultsPushButton, &QPushButton::clicked, this, &TimetableGenerateForm::write);
	connect(closePushButton, &QPushButton::clicked, this, &TimetableGenerateForm::closePressed);
	connect(helpPushButton, &QPushButton::clicked, this, &TimetableGenerateForm::help);
	connect(seeImpossiblePushButton, &QPushButton::clicked, this, &TimetableGenerateForm::seeImpossible);
	connect(seeInitialOrderPushButton, &QPushButton::clicked, this, &TimetableGenerateForm::seeInitialOrder);
	connect(writeHighestStagePushButton, &QPushButton::clicked, this, &TimetableGenerateForm::writeHighestStage);
	connect(stopHighestPushButton, &QPushButton::clicked, this, &TimetableGenerateForm::stopHighest);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	generation_running=false;

	startPushButton->setDefault(true);

	pausedLabel->setVisible(false);
	
	currentResultsTextEdit->setEnabled(true);
	
	startPushButton->setEnabled(true);
	stopPushButton->setDisabled(true);
	pausePushButton->setDisabled(true);
	pausePushButton->setText(tr("Pause", "Pause the generation"));
	stopHighestPushButton->setDisabled(true);
	closePushButton->setEnabled(true);
	writeResultsPushButton->setDisabled(true);
	writeHighestStagePushButton->setDisabled(true);
	seeImpossiblePushButton->setDisabled(true);
	seeInitialOrderPushButton->setDisabled(true);

	connect(&gen, &Generate::activityPlaced, this, &TimetableGenerateForm::activityPlaced);
	connect(&gen, &Generate::generationFinished, this, &TimetableGenerateForm::generationFinished);
	connect(&gen, &Generate::impossibleToSolve, this, &TimetableGenerateForm::impossibleToSolve);
}

TimetableGenerateForm::~TimetableGenerateForm()
{
	saveFETDialogGeometry(this);
	if(generation_running)
		this->stop();
		
	/*for(QProcess* myProcess : std::as_const(commandProcesses))
		//if(myProcess->state()!=QProcess::NotRunning)
			myProcess->close();*/
	
	/*generateThread.quit();
	generateThread.wait();*/
}

void TimetableGenerateForm::start(){
	//closeAllTimetableViewDialogs();

	if(!gt.rules.internalStructureComputed){
		if(!gt.rules.computeInternalStructure(this)){
			QMessageBox::warning(this, TimetableGenerateForm::tr("FET warning"), TimetableGenerateForm::tr("Data is wrong. Please correct and try again"));
			return;
		}
	}

	if(!gt.rules.initialized || gt.rules.activitiesList.isEmpty()){
		QMessageBox::critical(this, TimetableGenerateForm::tr("FET information"),
			TimetableGenerateForm::tr("You have entered the generation with uninitialized rules or 0 activities...aborting"));
		assert(0);
		exit(1);
		return;
	}

	currentResultsTextEdit->setPlainText(TimetableGenerateForm::tr("Entering generation ... precomputing, please be patient"));

	gen.abortOptimization=false;
	gen.restart=false;
	gen.paused=false;
	gen.pausedTime=0;
	bool ok=gen.precompute(this);
	
	if(!ok){
		currentResultsTextEdit->setPlainText(TimetableGenerateForm::tr("Cannot generate - please modify your data"));
		currentResultsTextEdit->update();

		QMessageBox::information(this, TimetableGenerateForm::tr("FET information"),
		 TimetableGenerateForm::tr("Your data cannot be processed - please modify it as instructed"));

		return;
	}

	pausedLabel->setVisible(false);

	currentResultsTextEdit->setEnabled(true);

	startPushButton->setDisabled(true);
	stopPushButton->setEnabled(true);
	pausePushButton->setEnabled(true);
	pausePushButton->setText(tr("Pause", "Pause the generation"));
	stopHighestPushButton->setEnabled(true);
	closePushButton->setDisabled(true);
	writeResultsPushButton->setEnabled(true);
	writeHighestStagePushButton->setEnabled(true);
	seeImpossiblePushButton->setEnabled(true);
	seeInitialOrderPushButton->setEnabled(true);

	generation_running=true;
	
	//2024-10-07
	QString kk;
	kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	//kk.append("-single");

	initialOutputDirectory=OUTPUT_DIR+FILE_SEP+"timetables"+kk;

	if(!OVERWRITE_SINGLE_GENERATION_FILES && QFileInfo::exists(initialOutputDirectory)){
		int i=2;
		for(;;){
			QString CODN=initialOutputDirectory+"-"+QString::number(i);
			if(!QFileInfo::exists(CODN)){
				initialOutputDirectory=CODN;
				break;
			}
			i++;
		}
	}
	///////////

	gen.c.makeUnallocated(gt.rules);
	
	CURRENT_OUTPUT_DIRECTORY=initialOutputDirectory;
	
	TimetableExport::writeRandomSeed(this, gen.rng, true); //true represents 'before' state

	//generateThread.start();
	/*singleThread=std::thread(runSingle);
	singleThread.detach();*/
	std::thread(runSingle).detach();
}

void TimetableGenerateForm::stop()
{
	if(!generation_running){
		return;
	}

	generation_running=false;

	//gen.myMutex.lock();
	gen.abortOptimization=true;
	//gen.myMutex.unlock();
	
	//QCoreApplication::sendPostedEvents();
	gen.semaphorePlacedActivity.release();
	
	gen.semaphoreFinished.acquire();

	//gen.semaphorePlacedActivity.tryAcquire();

	//gen.myMutex.lock();

	Solution& c=gen.c;

	//needed to find the conflicts strings
	FakeString tmp;
	c.fitness(gt.rules, &tmp);

	/*TimetableExport::getStudentsTimetable(c);
	TimetableExport::getTeachersTimetable(c);
	TimetableExport::getRoomsTimetable(c);*/
	TimetableExport::getStudentsTeachersRoomsBuildingsTimetable(c);

	//update the string representing the conflicts
	conflictsStringTitle=TimetableGenerateForm::tr("Conflicts", "Title of dialog");
	conflictsString="";
	conflictsString+=tr("Number of broken constraints: %1").arg(c.conflictsWeightList.count());
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Total conflicts:");
	conflictsString+=" ";
	conflictsString+=CustomFETString::numberPlusTwoDigitsPrecision(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Conflicts listing (in decreasing order):");
	conflictsString+="\n";

	for(const QString& t : std::as_const(c.conflictsDescriptionList))
		conflictsString+=t+"\n";
	
	updateAllTimetableViewDialogs();

	QString kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-current");

	CURRENT_OUTPUT_DIRECTORY=OUTPUT_DIR+FILE_SEP+"timetables"+kk;
	
	if(!OVERWRITE_SINGLE_GENERATION_FILES && QFileInfo::exists(CURRENT_OUTPUT_DIRECTORY)){
		int i=2;
		for(;;){
			QString CODN=CURRENT_OUTPUT_DIRECTORY+"-"+QString::number(i);
			if(!QFileInfo::exists(CODN)){
				CURRENT_OUTPUT_DIRECTORY=CODN;
				break;
			}
			i++;
		}
	}
	
	TimetableExport::writeGenerationResults(this);

	QString s=TimetableGenerateForm::tr("Generation interrupted! FET could not find a timetable."
	 " Maybe you can consider relaxing the constraints.");

	s+=" ";
	
	/*QString kk;
	kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-single");*/
	
	s+=TimetableGenerateForm::tr("The starting random seed was saved in the directory %1.").arg(QDir::toNativeSeparators(initialOutputDirectory));
	s+=" ";
	s+=TimetableGenerateForm::tr("The partial results were saved in the directory %1.").arg(QDir::toNativeSeparators(CURRENT_OUTPUT_DIRECTORY));

	s+="\n\n";

	s+=TimetableGenerateForm::tr("Additional information relating to the impossible to schedule activities:");
	s+="\n\n";

	s+=tr("FET managed to schedule correctly the first %1 most difficult activities."
	 " You can see the initial order of placing the activities in the generate dialog. The activity which might cause problems"
	 " might be the next activity in the initial order of evaluation. This activity is listed below:")
	 .arg(gen.maxActivitiesPlaced);
	
	s+="\n\n";
	
	s+=tr("Please check the constraints related to following possibly problematic activity (or teacher(s), or students set(s)):");
	s+="\n";
	s+="-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ";
	s+="\n";
	
	if(gen.maxActivitiesPlaced>=0 && gen.maxActivitiesPlaced<gt.rules.nInternalActivities
	 && initialOrderOfActivitiesIndices[gen.maxActivitiesPlaced]>=0 && initialOrderOfActivitiesIndices[gen.maxActivitiesPlaced]<gt.rules.nInternalActivities){
		int ai=initialOrderOfActivitiesIndices[gen.maxActivitiesPlaced];

		s+=TimetableGenerateForm::tr("Id: %1 (%2)", "%1 is id of activity, %2 is detailed description of activity")
			.arg(gt.rules.internalActivitiesList[ai].id)
			.arg(getActivityDetailedDescription(gt.rules, gt.rules.internalActivitiesList[ai].id));
	}
	else
		s+=tr("Difficult activity cannot be computed - please report possible bug");

	s+="\n";
	s+="-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ";

	s+="\n\n";

	s+=TimetableGenerateForm::tr("Please check the constraints related to the last "
	 "activities in the list below, which might be impossible to schedule:");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("Here are the placed activities which led to an inconsistency, "
	 "in order, from the first one to the last one (the last one FET failed to schedule "
	 "and the last ones are most likely the difficult ones):");
	s+="\n\n";
	for(int i=0; i<gen.nDifficultActivities; i++){
		int ai=gen.difficultActivities[i];

		s+=TimetableGenerateForm::tr("No: %1").arg(i+1);

		s+=translatedCommaSpace();

		s+=TimetableGenerateForm::tr("Id: %1 (%2)", "%1 is id of activity, %2 is detailed description of activity")
			.arg(gt.rules.internalActivitiesList[ai].id)
			.arg(getActivityDetailedDescription(gt.rules, gt.rules.internalActivitiesList[ai].id));

		s+="\n";
	}

	//gen.myMutex.unlock();

	//show the message in a dialog
	QDialog dialog(this);
	
	dialog.setWindowTitle(TimetableGenerateForm::tr("Generation stopped", "The title of a dialog, meaning that the generation of the timetable was stopped."));

	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(TimetableGenerateForm::tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, &QPushButton::clicked, &dialog, &QDialog::close);
	
	dialog.resize(700,500);
	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, settingsName);
	
	setParentAndOtherThings(&dialog, this);
	dialog.exec();
	saveFETDialogGeometry(&dialog, settingsName);

	pausedLabel->setVisible(false);

	currentResultsTextEdit->setEnabled(true);

	startPushButton->setEnabled(true);
	stopPushButton->setDisabled(true);
	pausePushButton->setDisabled(true);
	pausePushButton->setText(tr("Pause", "Pause the generation"));
	stopHighestPushButton->setDisabled(true);
	closePushButton->setEnabled(true);
	writeResultsPushButton->setDisabled(true);
	writeHighestStagePushButton->setDisabled(true);
	seeImpossiblePushButton->setDisabled(true);

	//generateThread.quit();
	//generateThread.wait();
	//if(singleThread.joinable())
	//	singleThread.join();
}

void TimetableGenerateForm::stopHighest()
{
	if(!generation_running){
		return;
	}

	generation_running=false;

	//gen.myMutex.lock();
	gen.abortOptimization=true;
	//gen.myMutex.unlock();
	
	//QCoreApplication::sendPostedEvents();
	gen.semaphorePlacedActivity.release();

	gen.semaphoreFinished.acquire();

	//gen.semaphorePlacedActivity.tryAcquire();

	//gen.myMutex.lock();

	Solution& c=gen.highestStageSolution;

	//needed to find the conflicts strings
	FakeString tmp;
	c.fitness(gt.rules, &tmp);

	/*TimetableExport::getStudentsTimetable(c);
	TimetableExport::getTeachersTimetable(c);
	TimetableExport::getRoomsTimetable(c);*/
	TimetableExport::getStudentsTeachersRoomsBuildingsTimetable(c);

	//update the string representing the conflicts
	conflictsStringTitle=TimetableGenerateForm::tr("Conflicts", "Title of dialog");
	conflictsString="";
	conflictsString+=tr("Number of broken constraints: %1").arg(c.conflictsWeightList.count());
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Total conflicts:");
	conflictsString+=" ";
	conflictsString+=CustomFETString::numberPlusTwoDigitsPrecision(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Conflicts listing (in decreasing order):");
	conflictsString+="\n";

	for(const QString& t : std::as_const(c.conflictsDescriptionList))
		conflictsString+=t+"\n";

	updateAllTimetableViewDialogs();

	QString kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-highest");

	HIGHEST_OUTPUT_DIRECTORY=OUTPUT_DIR+FILE_SEP+"timetables"+kk;
	
	if(!OVERWRITE_SINGLE_GENERATION_FILES && QFileInfo::exists(HIGHEST_OUTPUT_DIRECTORY)){
		int i=2;
		for(;;){
			QString CODN=HIGHEST_OUTPUT_DIRECTORY+"-"+QString::number(i);
			if(!QFileInfo::exists(CODN)){
				HIGHEST_OUTPUT_DIRECTORY=CODN;
				break;
			}
			i++;
		}
	}
	
	TimetableExport::writeHighestStageResults(this);

	QString s=TimetableGenerateForm::tr("Generation interrupted! FET could not find a timetable."
	 " Maybe you can consider relaxing the constraints.");

	s+=" ";
	
	/*QString kk;
	kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-highest");*/

	s+=TimetableGenerateForm::tr("The starting random seed was saved in the directory %1.").arg(QDir::toNativeSeparators(initialOutputDirectory));
	s+=" ";
	s+=TimetableGenerateForm::tr("The partial highest-stage results were saved in the directory %1.").arg(QDir::toNativeSeparators(HIGHEST_OUTPUT_DIRECTORY));

	s+="\n\n";

	s+=TimetableGenerateForm::tr("Additional information relating to the impossible to schedule activities:");
	s+="\n\n";

	s+=tr("FET managed to schedule correctly the first %1 most difficult activities."
	 " You can see the initial order of placing the activities in the generate dialog. The activity which might cause problems"
	 " might be the next activity in the initial order of evaluation. This activity is listed below:")
	 .arg(gen.maxActivitiesPlaced);
	
	s+="\n\n";
	
	s+=tr("Please check the constraints related to following possibly problematic activity (or teacher(s), or students set(s)):");
	s+="\n";
	s+="-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ";
	s+="\n";
	
	if(gen.maxActivitiesPlaced>=0 && gen.maxActivitiesPlaced<gt.rules.nInternalActivities
	 && initialOrderOfActivitiesIndices[gen.maxActivitiesPlaced]>=0 && initialOrderOfActivitiesIndices[gen.maxActivitiesPlaced]<gt.rules.nInternalActivities){
		int ai=initialOrderOfActivitiesIndices[gen.maxActivitiesPlaced];

		s+=TimetableGenerateForm::tr("Id: %1 (%2)", "%1 is id of activity, %2 is detailed description of activity")
			.arg(gt.rules.internalActivitiesList[ai].id)
			.arg(getActivityDetailedDescription(gt.rules, gt.rules.internalActivitiesList[ai].id));
	}
	else
		s+=tr("Difficult activity cannot be computed - please report possible bug");

	s+="\n";

	//gen.myMutex.unlock();

	//show the message in a dialog
	QDialog dialog(this);

	dialog.setWindowTitle(TimetableGenerateForm::tr("Generation stopped (highest stage)", "The title of a dialog, meaning that the generation of the timetable was stopped "
		"and highest stage timetable written."));

	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(TimetableGenerateForm::tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, &QPushButton::clicked, &dialog, &QDialog::close);

	dialog.resize(700,500);
	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, settingsName);
	
	setParentAndOtherThings(&dialog, this);
	dialog.exec();
	saveFETDialogGeometry(&dialog, settingsName);

	pausedLabel->setVisible(false);

	currentResultsTextEdit->setEnabled(true);

	startPushButton->setEnabled(true);
	stopPushButton->setDisabled(true);
	pausePushButton->setDisabled(true);
	pausePushButton->setText(tr("Pause", "Pause the generation"));
	stopHighestPushButton->setDisabled(true);
	closePushButton->setEnabled(true);
	writeResultsPushButton->setDisabled(true);
	writeHighestStagePushButton->setDisabled(true);
	seeImpossiblePushButton->setDisabled(true);

	//generateThread.quit();
	//generateThread.wait();
	//if(singleThread.joinable())
	//	singleThread.join();
}

void TimetableGenerateForm::impossibleToSolve()
{
	if(!generation_running){
		return;
	}

	generation_running=false;

	//gen.myMutex.lock();
	gen.abortOptimization=true;
	//gen.myMutex.unlock();

	//QCoreApplication::sendPostedEvents();
	gen.semaphorePlacedActivity.release();

	gen.semaphoreFinished.acquire();

	//gen.semaphorePlacedActivity.tryAcquire();

	//gen.myMutex.lock();

	Solution& c=gen.c;

	//needed to find the conflicts strings
	FakeString tmp;
	c.fitness(gt.rules, &tmp);

	/*TimetableExport::getStudentsTimetable(c);
	TimetableExport::getTeachersTimetable(c);
	TimetableExport::getRoomsTimetable(c);*/
	TimetableExport::getStudentsTeachersRoomsBuildingsTimetable(c);

	//update the string representing the conflicts
	conflictsStringTitle=TimetableGenerateForm::tr("Conflicts", "Title of dialog");
	conflictsString="";
	conflictsString+=TimetableGenerateForm::tr("Total conflicts:");
	conflictsString+=tr("Number of broken constraints: %1").arg(c.conflictsWeightList.count());
	conflictsString+="\n";
	conflictsString+=" ";
	conflictsString+=CustomFETString::numberPlusTwoDigitsPrecision(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Conflicts listing (in decreasing order):");
	conflictsString+="\n";

	for(const QString& t : std::as_const(c.conflictsDescriptionList))
		conflictsString+=t+"\n";

	updateAllTimetableViewDialogs();

	QString kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-current");

	CURRENT_OUTPUT_DIRECTORY=OUTPUT_DIR+FILE_SEP+"timetables"+kk;
	
	if(!OVERWRITE_SINGLE_GENERATION_FILES && QFileInfo::exists(CURRENT_OUTPUT_DIRECTORY)){
		int i=2;
		for(;;){
			QString CODN=CURRENT_OUTPUT_DIRECTORY+"-"+QString::number(i);
			if(!QFileInfo::exists(CODN)){
				CURRENT_OUTPUT_DIRECTORY=CODN;
				break;
			}
			i++;
		}
	}

	TimetableExport::writeGenerationResults(this);

	QString s=TimetableGenerateForm::tr("Generation impossible! Maybe you can consider relaxing the constraints.");

	s+=" ";

	/*QString kk;
	kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-single");*/

	s+=TimetableGenerateForm::tr("The starting random seed was saved in the directory %1.").arg(QDir::toNativeSeparators(initialOutputDirectory));
	s+=" ";
	s+=TimetableGenerateForm::tr("The partial results were saved in the directory %1.").arg(QDir::toNativeSeparators(CURRENT_OUTPUT_DIRECTORY));

	s+="\n\n";

	s+=TimetableGenerateForm::tr("Additional information relating to the impossible to schedule activities:");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("Please check the constraints related to the "
	 "activity below, which might be impossible to schedule:");
	s+="\n\n";
	for(int i=0; i<gen.nDifficultActivities; i++){
		int ai=gen.difficultActivities[i];

		s+=TimetableGenerateForm::tr("No: %1").arg(i+1);

		s+=translatedCommaSpace();

		s+=TimetableGenerateForm::tr("Id: %1 (%2)", "%1 is id of activity, %2 is detailed description of activity")
			.arg(gt.rules.internalActivitiesList[ai].id)
			.arg(getActivityDetailedDescription(gt.rules, gt.rules.internalActivitiesList[ai].id));

		s+="\n";
	}

	//gen.myMutex.unlock();

	//show the message in a dialog
	QDialog dialog(this);

	dialog.setWindowTitle(TimetableGenerateForm::tr("Generation impossible", "The title of a dialog, meaning that the generation of the timetable is impossible."));

	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(TimetableGenerateForm::tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, &QPushButton::clicked, &dialog, &QDialog::close);

	dialog.resize(700,500);
	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, settingsName);

	setParentAndOtherThings(&dialog, this);

	if(BEEP_AT_END_OF_GENERATION)
		QApplication::beep();

	if(ENABLE_COMMAND_AT_END_OF_GENERATION){
		QString s=commandAtEndOfGeneration.simplified();
		if(!s.isEmpty()){
			QStringList sl=s.split(" ");
			assert(sl.count()>=1);
			QString command=sl.at(0);
			QStringList arguments;
			for(int i=1; i<sl.count(); i++)
				arguments.append(sl.at(i));
			
			/*if(DETACHED_NOTIFICATION==false){
				QProcess* myProcess=new QProcess();
				if(terminateCommandAfterSeconds>0)
					QTimer::singleShot(terminateCommandAfterSeconds*1000, myProcess, SL OT(terminate()));
				if(killCommandAfterSeconds>0)
					QTimer::singleShot(killCommandAfterSeconds*1000, myProcess, SL OT(kill()));
				
				//https://www.qtcentre.org/threads/43083-Freeing-a-QProcess-after-it-has-finished-using-deleteLater()
				connect(myProcess, SIG NAL(finished(int)), myProcess, SL OT(deleteLater()));
				myProcess->start(command, arguments);
			}*/
			//else{
			QProcess::startDetached(command, arguments);
			//}
		}
	}

	dialog.exec();
	saveFETDialogGeometry(&dialog, settingsName);

	pausedLabel->setVisible(false);

	currentResultsTextEdit->setEnabled(true);

	startPushButton->setEnabled(true);
	stopPushButton->setDisabled(true);
	pausePushButton->setDisabled(true);
	pausePushButton->setText(tr("Pause", "Pause the generation"));
	stopHighestPushButton->setDisabled(true);
	closePushButton->setEnabled(true);
	writeResultsPushButton->setDisabled(true);
	writeHighestStagePushButton->setDisabled(true);
	seeImpossiblePushButton->setDisabled(true);

	//generateThread.quit();
	//generateThread.wait();
	//if(singleThread.joinable())
	//	singleThread.join();
}

void TimetableGenerateForm::generationFinished()
{
	if(!generation_running){
		return;
	}

	generation_running=false;

	//QCoreApplication::sendPostedEvents();
	//gen.semaphorePlacedActivity.release();

	gen.semaphoreFinished.acquire();

	//gen.semaphorePlacedActivity.tryAcquire();

	//gen.finishedSemaphore.acquire();

	//gen.myMutex.lock();

	CURRENT_OUTPUT_DIRECTORY=initialOutputDirectory;

	TimetableExport::writeRandomSeed(this, gen.rng, false); //false represents 'before' state

	Solution& c=gen.c;

	//needed to find the conflicts strings
	FakeString tmp;
	c.fitness(gt.rules, &tmp);

	/*TimetableExport::getStudentsTimetable(c);
	TimetableExport::getTeachersTimetable(c);
	TimetableExport::getRoomsTimetable(c);*/
	TimetableExport::getStudentsTeachersRoomsBuildingsTimetable(c);

	//update the string representing the conflicts
	conflictsStringTitle=TimetableGenerateForm::tr("Soft conflicts", "Title of dialog");
	conflictsString="";

	conflictsString+=tr("Number of broken soft constraints: %1").arg(c.conflictsWeightList.count());

	conflictsString+="\n";

	conflictsString+=tr("Total soft conflicts:");
	conflictsString+=" ";
	conflictsString+=CustomFETString::numberPlusTwoDigitsPrecision(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Soft conflicts listing (in decreasing order):");
	conflictsString+="\n";

	for(const QString& t : std::as_const(c.conflictsDescriptionList))
		conflictsString+=t+"\n";

	updateAllTimetableViewDialogs();

	TimetableExport::writeGenerationResults(this);

	/*QString kk;
	kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-single");*/

	QString s=QString("");
	s+=tr("Generation successful!");
	s+=QString("\n\n");
	s+=tr("Number of broken soft constraints: %1").arg(c.conflictsWeightList.count());
	s+=QString("\n");
	s+=tr("Weighted soft conflicts: %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(c.conflictsTotal));
	s+=QString("\n\n");
	s+=tr("The results were saved in the directory %1.").arg(QDir::toNativeSeparators(CURRENT_OUTPUT_DIRECTORY));

	//gen.myMutex.unlock();

//Old comment below (2020-08-14).
//On Windows we do not beep for Qt >= 5.14.1, because the QMessageBox below beeps itself.
//It would be better to test at runtime, not at compile time, but it is easier/safer this way.
//(The alternative would be to develop a parser for the function qVersion(), but I am not sure it will always respect the exact format "vM.vm.vp".)
//We test the macro Q_OS_WIN32 because on the old Qt 4 it is the only available macro from these three below,
//Q_OS_WIN, Q_OS_WIN32, and Q_OS_WIN64 (which are available on Qt 5.14.1). Yes, the test is redundant (because if QT_VERSION < 5.14.1
//the condition is true and if QT_VERSION >= 5.14.1 then all these three macros are available), but this doesn't hurt.
//#if (!defined(Q_OS_WIN) && !defined(Q_OS_WIN32) && !defined(Q_OS_WIN64)) || (QT_VERSION < QT_VERSION_CHECK(5,14,1))
	if(BEEP_AT_END_OF_GENERATION)
		QApplication::beep();
//#endif

	if(ENABLE_COMMAND_AT_END_OF_GENERATION){
		QString s=commandAtEndOfGeneration.simplified();
		if(!s.isEmpty()){
			QStringList sl=s.split(" ");
			assert(sl.count()>=1);
			QString command=sl.at(0);
			QStringList arguments;
			for(int i=1; i<sl.count(); i++)
				arguments.append(sl.at(i));
			
			/*if(DETACHED_NOTIFICATION==false){
				QProcess* myProcess=new QProcess();
				if(terminateCommandAfterSeconds>0)
					QTimer::singleShot(terminateCommandAfterSeconds*1000, myProcess, SL OT(terminate()));
				if(killCommandAfterSeconds>0)
					QTimer::singleShot(killCommandAfterSeconds*1000, myProcess, SL OT(kill()));
				
				//https://www.qtcentre.org/threads/43083-Freeing-a-QProcess-after-it-has-finished-using-deleteLater()
				connect(myProcess, SIG NAL(finished(int)), myProcess, SL OT(deleteLater()));
				myProcess->start(command, arguments);
			}*/
			//else{
			QProcess::startDetached(command, arguments);
			//}
		}
	}
	
	//Trick so that the message box will be silent (the only sound is thus the beep above).
	QMessageBox msgBox(this);
	msgBox.setWindowTitle(TimetableGenerateForm::tr("FET information"));
	msgBox.setText(s);
	msgBox.exec();
	//QMessageBox::information(this, TimetableGenerateForm::tr("FET information"), s);

	pausedLabel->setVisible(false);

	currentResultsTextEdit->setEnabled(true);

	startPushButton->setEnabled(true);
	stopPushButton->setDisabled(true);
	pausePushButton->setDisabled(true);
	pausePushButton->setText(tr("Pause", "Pause the generation"));
	stopHighestPushButton->setDisabled(true);
	closePushButton->setEnabled(true);
	writeResultsPushButton->setDisabled(true);
	writeHighestStagePushButton->setDisabled(true);
	seeImpossiblePushButton->setDisabled(true);

	//generateThread.quit();
	//generateThread.wait();
	//if(singleThread.joinable())
	//	singleThread.join();
}

void TimetableGenerateForm::activityPlaced(int nThread, int na){
	Q_UNUSED(nThread);
	
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	
	//if(gen.paused) -> DON'T DO THIS!!! We need to release the semaphore below.
	//	return;

	gen.myMutex.lock();
	int t=gen.searchTime; //seconds
	int mact=gen.maxActivitiesPlaced;
	int seconds=gen.timeToHighestStage;
	gen.myMutex.unlock();

	gen.semaphorePlacedActivity.release();

	//gen.cvForPlacedActivity.notify_one();

	//write to the Qt interface
	QString s;
	s+=TimetableGenerateForm::tr("%1 out of %2 activities placed").arg(na).arg(gt.rules.nInternalActivities)+"\n";

	s+=TimetableGenerateForm::tr("Elapsed time:");
	int h=t/3600;
	if(h>0){
		s+=" ";
		s+=TimetableGenerateForm::tr("%1 h", "hours").arg(h);
	}
	t=t%3600;
	int m=t/60;
	if(m>0){
		s+=" ";
		s+=TimetableGenerateForm::tr("%1 m", "minutes").arg(m);
	}
	t=t%60;
	if(t>0){
		s+=" ";
		s+=TimetableGenerateForm::tr("%1 s", "seconds").arg(t);
	}
	
	bool zero=false;
	if(seconds==0)
		zero=true;
	int hh=seconds/3600;
	seconds%=3600;
	int mm=seconds/60;
	seconds%=60;
	int ss=seconds;

	QString tim;
	if(hh>0){
		tim+=" ";
		tim+=tr("%1 h", "hours").arg(hh);
	}
	if(mm>0){
		tim+=" ";
		tim+=tr("%1 m", "minutes").arg(mm);
	}
	if(ss>0 || zero){
		tim+=" ";
		tim+=tr("%1 s", "seconds").arg(ss);
	}
	tim.remove(0, 1);
	s+="\n\n";
	s+=tr("Max placed activities: %1 (at %2)", "%1 represents the maximum number of activities placed, %2 is a time interval").arg(mact).arg(tim);

	//if(gen.paused)
	//	s.prepend(tr("[PAUSED]", "Generation is paused")+QString("\n"));

	currentResultsTextEdit->setPlainText(s);
}

void TimetableGenerateForm::help()
{
	QString s;
	
	s+=TimetableGenerateForm::tr("Please wait. It might take 5 to 20 minutes or even more for very difficult timetables");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("Activities are placed in order, most difficult ones first");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("The process of searching is semi-randomized, which means that "
	 "you will get different timetables and running times each time. You can choose the best timetable from several runs");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("Usually, there is no need to stop and restart the search."
	 " But for very difficult timetables this can help. Sometimes in such cases FET can become stuck and cycle forever,"
	 " and restarting might produce a very fast solution.");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("It is recommended to strengthen the constraints step by step (for"
	 " instance min days between activities weight or teacher(s) max gaps), as you obtain feasible timetables.");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("If your timetable gets stuck on a certain activity number k (and then"
	 " begins going back), please check the initial evaluation order and see activity number k+1 in this list. You may find"
	 " errors this way.");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("If the generation is successful, you cannot have hard conflicts. You can have only soft conflicts,"
	 " corresponding to constraints with weight lower than 100.0%, which are reported in detail.");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("After the generation (successful or interrupted), you can view the current (complete or incomplete) timetable"
	 " in the corresponding view timetable dialogs, and the list of conflicts in the view conflicts dialog.");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("The results are saved in your selected results directory in HTML and XML mode and the soft conflicts"
	 " in text mode, along with the current data and timetable, saved as a .fet data file (with activities locked by constraints), so"
	 " that you can open, modify and regenerate the current timetable later");
	
	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}

void TimetableGenerateForm::write(){
	if(gen.paused) //should not happen
		assert(0);

	gen.myMutex.lock();

	Solution& c=gen.c;

	//needed to find the conflicts strings
	FakeString tmp;
	c.fitness(gt.rules, &tmp);

	/*TimetableExport::getStudentsTimetable(c);
	TimetableExport::getTeachersTimetable(c);
	TimetableExport::getRoomsTimetable(c);*/
	TimetableExport::getStudentsTeachersRoomsBuildingsTimetable(c);

	//update the string representing the conflicts
	conflictsStringTitle=TimetableGenerateForm::tr("Conflicts", "Title of dialog");
	conflictsString="";
	conflictsString+=tr("Number of broken constraints: %1").arg(c.conflictsWeightList.count());
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Total conflicts:");
	conflictsString+=" ";
	conflictsString+=CustomFETString::numberPlusTwoDigitsPrecision(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Conflicts listing (in decreasing order):");
	conflictsString+="\n";

	for(const QString& t : std::as_const(c.conflictsDescriptionList))
		conflictsString+=t+"\n";

	updateAllTimetableViewDialogs();

	QString kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-current");

	CURRENT_OUTPUT_DIRECTORY=OUTPUT_DIR+FILE_SEP+"timetables"+kk;
	
	if(!OVERWRITE_SINGLE_GENERATION_FILES && QFileInfo::exists(CURRENT_OUTPUT_DIRECTORY)){
		int i=2;
		for(;;){
			QString CODN=CURRENT_OUTPUT_DIRECTORY+"-"+QString::number(i);
			if(!QFileInfo::exists(CODN)){
				CURRENT_OUTPUT_DIRECTORY=CODN;
				break;
			}
			i++;
		}
	}

	TimetableExport::writeGenerationResults(this);

	gen.myMutex.unlock();

	/*QString kk;
	kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-single");*/

	QMessageBox::information(this, TimetableGenerateForm::tr("FET information"),
		TimetableGenerateForm::tr("The generation results should now be written in the directory %1 in HTML and XML mode"
		" and the conflicts in txt mode").arg(QDir::toNativeSeparators(CURRENT_OUTPUT_DIRECTORY)));
}

void TimetableGenerateForm::writeHighestStage(){
	if(gen.paused) //should not happen
		assert(0);

	gen.myMutex.lock();

	Solution& c=gen.highestStageSolution;

	//needed to find the conflicts strings
	FakeString tmp;
	c.fitness(gt.rules, &tmp);

	/*TimetableExport::getStudentsTimetable(c);
	TimetableExport::getTeachersTimetable(c);
	TimetableExport::getRoomsTimetable(c);*/
	TimetableExport::getStudentsTeachersRoomsBuildingsTimetable(c);

	//update the string representing the conflicts
	conflictsStringTitle=TimetableGenerateForm::tr("Conflicts", "Title of dialog");
	conflictsString="";
	conflictsString+=tr("Number of broken constraints: %1").arg(c.conflictsWeightList.count());
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Total conflicts:");
	conflictsString+=" ";
	conflictsString+=CustomFETString::numberPlusTwoDigitsPrecision(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString+=TimetableGenerateForm::tr("Conflicts listing (in decreasing order):");
	conflictsString+="\n";

	for(const QString& t : std::as_const(c.conflictsDescriptionList))
		conflictsString+=t+"\n";

	updateAllTimetableViewDialogs();

	QString kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-highest");

	HIGHEST_OUTPUT_DIRECTORY=OUTPUT_DIR+FILE_SEP+"timetables"+kk;
	
	if(!OVERWRITE_SINGLE_GENERATION_FILES && QFileInfo::exists(HIGHEST_OUTPUT_DIRECTORY)){
		int i=2;
		for(;;){
			QString CODN=HIGHEST_OUTPUT_DIRECTORY+"-"+QString::number(i);
			if(!QFileInfo::exists(CODN)){
				HIGHEST_OUTPUT_DIRECTORY=CODN;
				break;
			}
			i++;
		}
	}

	TimetableExport::writeHighestStageResults(this);

	gen.myMutex.unlock();

	/*QString kk;
	kk=FILE_SEP;
	if(INPUT_FILENAME_XML=="")
		kk.append("unnamed");
	else{
		kk.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));

		if(kk.right(4)==".fet")
			kk=kk.left(kk.length()-4);
	}
	kk.append("-highest");*/

	QMessageBox::information(this, TimetableGenerateForm::tr("FET information"),
		TimetableGenerateForm::tr("Highest stage results should now be written in the directory %1 in HTML and XML mode"
		" and the conflicts in txt mode").arg(QDir::toNativeSeparators(HIGHEST_OUTPUT_DIRECTORY)));
}

void TimetableGenerateForm::closePressed()
{
	//if(singleThread.joinable())
	//	singleThread.join();
	//if(!generateThread.isRunning())

	if(!generation_running)
	//if(!singleThread.joinable())
		this->close();
}

void TimetableGenerateForm::seeImpossible()
{
	if(gen.paused) //should not happen
		assert(0);

	QString s;

	gen.myMutex.lock();

	s+=TimetableGenerateForm::tr("Information relating difficult to schedule activities:");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("Please check the constraints related to the last "
	 "activities in the list below, which might be difficult to schedule:");
	s+="\n\n";
	s+=TimetableGenerateForm::tr("Here are the placed activities which led to a difficulty, "
	 "in order, from the first one to the last one (the last one FET failed to schedule "
	 "and the last ones are probably difficult):");
	s+="\n\n";
	for(int i=0; i<gen.nDifficultActivities; i++){
		int ai=gen.difficultActivities[i];

		s+=TimetableGenerateForm::tr("No: %1").arg(i+1);

		s+=translatedCommaSpace();

		s+=TimetableGenerateForm::tr("Id: %1 (%2)", "%1 is id of activity, %2 is detailed description of activity")
			.arg(gt.rules.internalActivitiesList[ai].id)
			.arg(getActivityDetailedDescription(gt.rules, gt.rules.internalActivitiesList[ai].id));

		s+="\n";
	}

	gen.myMutex.unlock();
	
	//show the message in a dialog
	QDialog dialog(this);
	
	dialog.setWindowTitle(tr("FET - information about difficult activities"));

	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, &QPushButton::clicked, &dialog, &QDialog::close);

	dialog.resize(700,500);
	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, settingsName);

	setParentAndOtherThings(&dialog, this);
	dialog.exec();
	saveFETDialogGeometry(&dialog, settingsName);
}

void TimetableGenerateForm::pause()
{
	if(!gen.paused){
		writeResultsPushButton->setDisabled(true);
		writeHighestStagePushButton->setDisabled(true);
		seeImpossiblePushButton->setDisabled(true);

		pausePushButton->setText(tr("Continue", "Continue the generation (it was previously paused, and now it will continue from where it was paused). "
		 "Please make a clear distinction between 'Continue the generation' and 'Restart the generation'."));
		gen.paused=true;
		
		currentResultsTextEdit->setDisabled(true);
		
		pausedLabel->setVisible(true);

		//QString s=tr("[PAUSED]", "Generation is paused")+QString("\n")+currentResultsTextEdit->toPlainText();
		//currentResultsTextEdit->setPlainText(s);
	}
	else{
		pausedLabel->setVisible(false);

		currentResultsTextEdit->setEnabled(true);

		pausePushButton->setText(tr("Pause", "Pause the generation"));
		gen.paused=false;

		writeResultsPushButton->setEnabled(true);
		writeHighestStagePushButton->setEnabled(true);
		seeImpossiblePushButton->setEnabled(true);
	}
}

void TimetableGenerateForm::seeInitialOrder()
{
	QString s=initialOrderOfActivities;

	//show the message in a dialog
	QDialog dialog(this);
	
	dialog.setWindowTitle(tr("FET - information about initial order of evaluation of activities"));

	QVBoxLayout* vl=new QVBoxLayout(&dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, &QPushButton::clicked, &dialog, &QDialog::close);

	dialog.resize(700,500);
	centerWidgetOnScreen(&dialog);
	restoreFETDialogGeometry(&dialog, settingsName);

	setParentAndOtherThings(&dialog, this);
	dialog.exec();
	saveFETDialogGeometry(&dialog, settingsName);
}
