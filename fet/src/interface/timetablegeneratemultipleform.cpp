/***************************************************************************
                          timetablegeneratemultipleform.cpp  -  description
                             -------------------
    begin                : Aug 20, 2007
    copyright            : (C) 2007 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "longtextmessagebox.h"

#include "generate.h"

#include "timetablegeneratemultipleform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "timetableexport.h"

#include <qpushbutton.h>

#include <QDesktopWidget>

#include <QTextEdit>

#include <iostream>
#include <fstream>
using namespace std;

#include <QMessageBox>

#include <QMutex>

#include <QDir>

extern QMutex mutex;

static GenerateMultipleThread generateMultipleThread;

#include <QSemaphore>

static QSemaphore semaphoreTimetableFinished; 

static QSemaphore semaphoreTimetableStarted;

//Represents the current status of the simulation - running or stopped.
extern bool simulation_running_multi;

extern QSemaphore semaphorePlacedActivity;

Generate genMulti;

static int nTimetables;
static int timeLimit;

extern Solution best_solution;

extern QString conflictsString;

static time_t start_time;

static time_t initial_time;

extern int permutation[MAX_ACTIVITIES];
int savedPermutation[MAX_ACTIVITIES];

void GenerateMultipleThread::run()
{
	genMulti.abortOptimization=false;
	
	time(&initial_time);

	for(int i=0; i<nTimetables; i++){
		time(&start_time);
	
		bool impossible;
		bool timeExceeded;
		
		//bool ok=genMulti.precompute(); //here I can speed up, precomputation is only needed to restore permutation of activities
		//assert(ok);
		for(int qq=0; qq<gt.rules.nInternalActivities; qq++)
			permutation[qq]=savedPermutation[qq];
			
		emit(timetableStarted(i+1));
		semaphoreTimetableStarted.acquire();

		genMulti.generate(timeLimit, impossible, timeExceeded, true); //true means threaded

		QString s;
		
		bool ok;

		mutex.lock();
		if(genMulti.abortOptimization){
			mutex.unlock();
			return;
		}
		else if(impossible){
			s=tr("Timetable impossible to generate");
			ok=false;
		}
		else if(timeExceeded){
			s=tr("Time exceeded for current timetable");
			ok=false;
		}
		else{
			ok=true;
			
			time_t finish_time;
			time(&finish_time);
			int seconds=int(finish_time-start_time);
			int hours=seconds/3600;
			seconds%=3600;
			int minutes=seconds/60;
			seconds%=60;
			
			QString tmp;
			genMulti.c.fitness(gt.rules, &tmp);
			
			s=tr("Timetable has %1 soft conflicts factor and was generated in %2 hours, %3 minutes and %4 seconds")
			 .arg(genMulti.c.conflictsTotal)
			 .arg(hours)
			 .arg(minutes)
			 .arg(seconds);
		}
		mutex.unlock();
		
		emit(timetableGenerated(i+1, s, ok));
		semaphoreTimetableFinished.acquire();
	}
	
	emit(finished());
}

TimetableGenerateMultipleForm::TimetableGenerateMultipleForm()
{
    setupUi(this);

    connect(startPushButton, SIGNAL(clicked()), this /*TimetableGenerateMultipleForm_template*/, SLOT(start()));
    connect(stopPushButton, SIGNAL(clicked()), this /*TimetableGenerateMultipleForm_template*/, SLOT(stop()));
    connect(closePushButton, SIGNAL(clicked()), this /*TimetableGenerateMultipleForm_template*/, SLOT(closePressed()));
    connect(pushButton4, SIGNAL(clicked()), this /*TimetableGenerateMultipleForm_template*/, SLOT(help()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	simulation_running_multi=false;

	startPushButton->setEnabled(TRUE);
	stopPushButton->setDisabled(TRUE);
	closePushButton->setEnabled(TRUE);
	minutesGroupBox->setEnabled(TRUE);
	timetablesGroupBox->setEnabled(TRUE);

	connect(&generateMultipleThread, SIGNAL(timetableGenerated(int, const QString&, bool)),
		this, SLOT(timetableGenerated(int, const QString&, bool)));

	connect(&generateMultipleThread, SIGNAL(timetableStarted(int)),
		this, SLOT(timetableStarted(int)));

	connect(&generateMultipleThread, SIGNAL(finished()),
		this, SLOT(finished()));

	connect(&genMulti, SIGNAL(activityPlaced(int)),
		this, SLOT(activityPlaced(int)));

	//this->setAttribute(Qt::WA_DeleteOnClose);
}

TimetableGenerateMultipleForm::~TimetableGenerateMultipleForm()
{
	if(simulation_running_multi)
		this->stop();
}

void TimetableGenerateMultipleForm::help()
{
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	
	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	
	QString destDir=OUTPUT_DIR+FILE_SEP+"timetables"+FILE_SEP+s2+"-multi";

	QString s=tr("You can only see generated timetables on the hard disk,"
	 " in html and xml formats and soft conflicts in txt format, or latest timetable in the Timetable/View menu."
	 " It is needed that the directory"
	 " %1 to be emptied+deleted before proceeding.").arg(QDir::toNativeSeparators(destDir))
	 +"\n\n"
	 +tr("Note that, for large data, each timetable might occupy more"
	 " megabytes of hard disk space,"
	 " so make sure you have enough space (you can check the dimension of a single timetable as a precaution).")
	 +"\n\n"
	 +tr("There are also saved the timetables in .fet format (data + constraints to lock the timetable), so that you can open each of them later")
	 +"\n\n"
	 +tr("If you get impossible timetable, please enter menu Generate (single) and see the initial order of evaluation of activities,"
	 " this might help.")
	 +"\n\n"
	 +tr("You can limit the search time, by specifying the maximum number of minutes allowed to spend for each timetable (option %1).").arg("'"+tr("Limit for each timetable")+"'")
	 +" "+tr("The maximum and also the predefined value is %1 minutes, which means %2 hours, so virtually unlimited.").arg(1200).arg(20)
	 ;
	 
	 LongTextMessageBox::largeInformation(this, tr("FET information"), s);

/*
	QMessageBox::information(this, tr("FET information"), tr("Notice: you can only see generated timetables on the hard disk,"
	 " in html and xml formats and soft conflicts in txt format, or latest timetable in the FET Timetable/View menu."
	 " It is needed that the directory"
	 " %1 to be emptied+deleted before proceeding.\n\nPlease note that, for large data, each timetable might occupy more"
	 " megabytes of hard disk space,"
	 " so make sure you have enough space (you can check the dimension of a single timetable as a precaution).")
	 .arg(QDir::toNativeSeparators(destDir))
	 +"\n\n"
	 +tr("NEW: There are also saved the timetables in .fet format (data + constraints to lock the timetable), so that you can open each of them later")
	 +"\n\n"
	 +tr("If you get impossible timetable, please enter menu Generate (single) and see the initial order of evaluation of activities,"
	 " this might help."));*/
}

void TimetableGenerateMultipleForm::start(){
	nTimetables=timetablesSpinBox->value();
	assert(nTimetables>0);
	timeLimit=60*minutesSpinBox->value(); //seconds
	assert(timeLimit>0);

	QDir dir;
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
		//cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	
	QString destDir=OUTPUT_DIR+FILE_SEP+"timetables"+FILE_SEP+s2+"-multi";
	if(dir.exists(destDir)){
		QMessageBox::warning(this, tr("FET information"), tr("Directory %1 exists and might not be empty,"
		 " (it might contain old files). You need to manually remove all contents of this directory AND the directory itself (or rename it)"
		 " and then you can generate multiple timetables")
		 .arg(QDir::toNativeSeparators(destDir)));
		 
		return;
	}

	if(!gt.rules.internalStructureComputed){
		if(!gt.rules.computeInternalStructure()){
			QMessageBox::warning(this, TimetableGenerateMultipleForm::tr("FET warning"), TimetableGenerateMultipleForm::tr("Data is wrong. Please correct and try again"));
			return;
		}
	}

	if(!gt.rules.initialized || gt.rules.activitiesList.isEmpty()){
		QMessageBox::critical(this, TimetableGenerateMultipleForm::tr("FET information"),
			TimetableGenerateMultipleForm::tr("You have entered simulation with uninitialized rules or 0 activities...aborting"));
		assert(0);
		exit(1);
		return;
	}

	currentResultsTextEdit->setText("");
	//currentResultsTextEdit->repaint();

	bool ok=genMulti.precompute();
	if(!ok){
		currentResultsTextEdit->setText(TimetableGenerateMultipleForm::tr("Cannot optimize - please modify your data"));
		currentResultsTextEdit->repaint();

		QMessageBox::information(this, TimetableGenerateMultipleForm::tr("FET information"),
		 TimetableGenerateMultipleForm::tr("Your data cannot be processed - please modify it as instructed"
		 "\nFor more information you can join the mailing list or write to author"));

		return;
	}

	startPushButton->setDisabled(TRUE);
	stopPushButton->setEnabled(TRUE);
	minutesGroupBox->setDisabled(TRUE);
	timetablesGroupBox->setDisabled(TRUE);
	closePushButton->setDisabled(TRUE);

	simulation_running_multi=true;

	for(int qq=0; qq<gt.rules.nInternalActivities; qq++)
		savedPermutation[qq]=permutation[qq];
		
	genMulti.c.makeUnallocated(gt.rules);

	generateMultipleThread.start();
}

void TimetableGenerateMultipleForm::timetableStarted(int timetable)
{
	TimetableExport::writeRandomSeed(timetable);
	
	semaphoreTimetableStarted.release();
}

void TimetableGenerateMultipleForm::timetableGenerated(int timetable, const QString& description, bool ok)
{
	QString s=currentResultsTextEdit->text();
	s+=tr("Timetable no: %1 => %2").arg(timetable).arg(description);
	s+="\n";
	currentResultsTextEdit->setText(s);

	if(ok){
		//needed to get the conflicts string
		QString tmp;
		//cout<<"tmp=="<<qPrintable(tmp)<<endl;
		//cout<<"&tmp=="<<&tmp<<endl;
		genMulti.c.fitness(gt.rules, &tmp);
	
		TimetableExport::getStudentsTimetable(genMulti.c);
		TimetableExport::getTeachersTimetable(genMulti.c);
		TimetableExport::getRoomsTimetable(genMulti.c);

		TimetableExport::writeSimulationResults(timetable);

		//update the string representing the conflicts
		conflictsString = "";
		conflictsString+=tr("Total soft conflicts:");
		conflictsString+=" ";
		conflictsString+=QString::number(best_solution.conflictsTotal);
		conflictsString+="\n";
		conflictsString += tr("Soft conflicts listing (in decreasing order):\n");

		foreach(QString t, best_solution.conflictsDescriptionList)
			conflictsString+=t+"\n";
	}

	semaphoreTimetableFinished.release();
}

void TimetableGenerateMultipleForm::stop()
{
	if(!simulation_running_multi){
		/*QMessageBox::critical(this, TimetableGenerateMultipleForm::tr("FET information"),
		 TimetableGenerateMultipleForm::tr("Simulation stopped but the simulation is not running."
		 " This should not happen. Maybe you aborted simulation previously. Please report possible bug to author"));*/

		return;
	}

	simulation_running_multi=false;

	mutex.lock();
	genMulti.abortOptimization=true;
	mutex.unlock();

	QString s=TimetableGenerateMultipleForm::tr("Simulation interrupted.");
	s+=" ";

	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
		//cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	
	QString destDir=OUTPUT_DIR+FILE_SEP+"timetables"+FILE_SEP+s2+"-multi";

	time_t final_time;
	time(&final_time);
	int sec=int(final_time-initial_time);
	int h=sec/3600;
	sec%=3600;
	int m=sec/60;
	sec%=60;

	s+=TimetableGenerateMultipleForm::tr("The results for the generated timetables are saved in the directory %1 in html and xml mode"
	 " and the soft conflicts in txt mode").arg(QDir::toNativeSeparators(destDir));
	 
	s+="\n\n"+tr("The data+timetables are also saved as .fet files (data+constraints to lock the timetable), so you can open-modify-regenerate the same timetables after that");
	 
	s+="\n\n"+tr("Total searching time was %1h %2m %3s").arg(h).arg(m).arg(sec);
	 
	QMessageBox::information(this, tr("FET information"), s);

	startPushButton->setEnabled(TRUE);
	stopPushButton->setDisabled(TRUE);
	minutesGroupBox->setEnabled(TRUE);
	timetablesGroupBox->setEnabled(TRUE);
	closePushButton->setEnabled(TRUE);
}

void TimetableGenerateMultipleForm::finished()
{
	simulationFinished();
}

void TimetableGenerateMultipleForm::simulationFinished()
{
	if(!simulation_running_multi){
		/*QMessageBox::critical(this, TimetableGenerateMultipleForm::tr("FET information"),
		 TimetableGenerateMultipleForm::tr("Simulation finished but the simulation is not running."
		 " This should not happen. Maybe you aborted simulation previously. Please report possible bug to author"));*/

		return;
	}

	simulation_running_multi=false;

	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);

	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
		//cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	
	QString destDir=OUTPUT_DIR+FILE_SEP+"timetables"+FILE_SEP+s2+"-multi";
	
	time_t final_time;
	time(&final_time);
	int s=int(final_time-initial_time);
	int h=s/3600;
	s%=3600;
	int m=s/60;
	s%=60;

	QMessageBox::information(this, TimetableGenerateMultipleForm::tr("FET information"),
		TimetableGenerateMultipleForm::tr("Simulation terminated successfully. The results are saved in directory %1 in html"
		" and xml mode and the soft conflicts in txt mode.").arg(QDir::toNativeSeparators(destDir))+"\n\n"
		+tr("The data+timetables are also saved as .fet files (data+constraints to lock the timetable), so you can open-modify-regenerate the same timetables after that")
		+"\n\n"+tr("Total searching time was %1h %2m %3s").arg(h).arg(m).arg(s));

	startPushButton->setEnabled(TRUE);
	stopPushButton->setDisabled(TRUE);
	minutesGroupBox->setEnabled(TRUE);
	timetablesGroupBox->setEnabled(TRUE);
	closePushButton->setEnabled(TRUE);
}

void TimetableGenerateMultipleForm::activityPlaced(int na)
{
	time_t finish_time;
	time(&finish_time);
	int seconds=int(finish_time-start_time);
	int hours=seconds/3600;
	seconds%=3600;
	int minutes=seconds/60;
	seconds%=60;
			
	textLabel->setText(tr("Current timetable: %1 out of %2 activities placed, %3h %4m %5s")
	 .arg(na)
	 .arg(gt.rules.nInternalActivities)
	 .arg(hours)
	 .arg(minutes)
	 .arg(seconds));
	 
	semaphorePlacedActivity.release();
}

void TimetableGenerateMultipleForm::closePressed()
{
	if(!generateMultipleThread.isRunning())
		this->close();
}
