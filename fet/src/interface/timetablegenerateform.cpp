/***************************************************************************
                          timetablegenerateform.cpp  -  description
                             -------------------
    begin                : Tue Apr 22 2003
    copyright            : (C) 2003 by Lalescu Liviu
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

//xhtml saving timetables code contributed by Volker Dirr (timetabling.de)

#include "generate.h"

#include "timetablegenerateform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <q3combobox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3table.h>
#include <qapplication.h>
#include <q3textedit.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qfile.h>

#include <QList>
#include <QSet>

#include <QDesktopWidget>

#include <QTextEdit>

#include <iostream>
#include <fstream>
using namespace std;

#include <QMessageBox>

QMutex mutex;

static TimeSolvingThread timeSolvingThread;

#include <QSemaphore>

QSemaphore semaphoreTime; //used to update when an activity is placed
//QSemaphore semaphoreTime2; //used when simulation finished successfully

//QSemaphore stopSemaphore;
QSemaphore finishedSemaphore;

//Represents the current status of the simulation - running or stopped.
extern bool simulation_running;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

extern Solution best_solution;

extern QString conflictsString;


Generate ot;


void TimeSolvingThread::run()
{
	ot.optimize();
}

TimetableGenerateForm::TimetableGenerateForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	simulation_running=false;

	startPushButton->setEnabled(TRUE);
	stopPushButton->setDisabled(TRUE);
	closePushButton->setEnabled(TRUE);
	writeResultsPushButton->setDisabled(TRUE);

	connect(&ot, SIGNAL(activityPlaced(int)),
	 this, SLOT(activityPlaced(int)));
	connect(&ot, SIGNAL(simulationFinished()),
	 this, SLOT(simulationFinished()));
	connect(&ot, SIGNAL(impossibleToSolve()),
	 this, SLOT(impossibleToSolve()));

	this->setAttribute(Qt::WA_DeleteOnClose);
}

TimetableGenerateForm::~TimetableGenerateForm()
{
	//timeSolvingThread.quit();

	if(simulation_running)
		this->stop();

/*	mutex.lock();
	ot.abortOptimization=true;
	mutex.unlock();

	simulation_running=false;*/
}

void TimetableGenerateForm::start(){
	if(!gt.rules.internalStructureComputed){
		if(!gt.rules.computeInternalStructure()){
			QMessageBox::warning(this, TimetableGenerateForm::tr("FET warning"), TimetableGenerateForm::tr("Data is wrong. Please correct and try again"));
			//assert(0);
			return;
		}
	}

	if(!gt.rules.initialized || gt.rules.activitiesList.isEmpty()){
		QMessageBox::critical(this, TimetableGenerateForm::tr("FET information"),
			TimetableGenerateForm::tr("You have entered simulation with uninitialized rules or 0 activities...aborting"));
		assert(0);
		exit(1);
		return;
	}

	/*if(!gt.timePopulation.initialized){
		QMessageBox::warning(this, TimetableGenerateForm::tr("FET information"),
			TimetableGenerateForm::tr("You didn't initialize or load the initial state"));
		return;
	}*/

    //QLabel* label = new QLabel(TimetableGenerateForm::tr("Entering simulation....precomputing"));
    //label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	//label->setText(TimetableGenerateForm::tr("Entering simulation....precomputing"));
	//label->setAlignment(Qt::AlignBottom | Qt::AlignRight);
	//label->show();

	//QMessageBox::information(this, TimetableGenerateForm::tr("FET information"),
	// TimetableGenerateForm::tr("Entering simulation"));

	currentResultsTextEdit->setText(TimetableGenerateForm::tr("Entering simulation....precomputing"));
	currentResultsTextEdit->repaint();

	//QMessageBox::information(this, TimetableGenerateForm::tr("FET information"),
	// TimetableGenerateForm::tr("Entering simulation"));

	bool ok=ot.precompute();

	//cout<<"ok=="<<ok<<endl;
	//assert(0);

	if(!ok){
		//delete label;
		currentResultsTextEdit->setText(TimetableGenerateForm::tr("Cannot optimize - please modify your data"));
		currentResultsTextEdit->repaint();

		QMessageBox::information(this, TimetableGenerateForm::tr("FET information"),
		 TimetableGenerateForm::tr("Your data cannot be processed - please modify it as instructed"
		 "\nFor more information you can join the mailing list or write to author"));

		return;
	}

	//delete label;

	startPushButton->setDisabled(TRUE);
	stopPushButton->setEnabled(TRUE);
	closePushButton->setDisabled(TRUE);
	writeResultsPushButton->setEnabled(TRUE);

	/*QMessageBox::information(this, TimetableGenerateForm::tr("FET information"),
	 TimetableGenerateForm::tr("Entering simulation..."));*/

	simulation_running=true;

	timeSolvingThread.start();
}

void TimetableGenerateForm::stop()
{
	if(!simulation_running){
		QMessageBox::critical(this, TimetableGenerateForm::tr("FET information"),
		 TimetableGenerateForm::tr("Simulation stopped but the simulation is not running."
		 " This should not happen. Maybe you aborted simulation previously. Please report possible bug to author"));

		return;
	}

	mutex.lock();
	ot.abortOptimization=true;
	mutex.unlock();

	//cout<<"trying to acquire stopsemaphore - timetable allocatehoursform.cpp line 181"<<endl;
	//stopSemaphore.acquire();
	//cout<<"ok, passed this line - timetable allocatehoursform.cpp line 183\n";

	simulation_running=false;

	mutex.lock();

	Solution& c=ot.c;

	getStudentsTimetable(c);
	getTeachersTimetable(c);
	getRoomsTimetable(c);

	//needed to find the conflicts strings
	QString tmp;
	c.fitness(gt.rules, &tmp);

	//update the string representing the conflicts
	conflictsString = "";
	conflictsString+="Total conflicts: ";
	conflictsString+=QString::number(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString += TimetableGenerateForm::tr("Conflicts listing (in decreasing order):\n");

	foreach(QString t, c.conflictsDescriptionList)
		conflictsString+=t+"\n";

	writeSimulationResults(c);

	QString s=TimetableGenerateForm::tr("Simulation interrupted. FET could not find a perfect timetable. "
	 "Maybe you can consider lowering the constraints.");

	s+=" ";

	s+=TimetableGenerateForm::tr("The partial results are saved in the directory %1 in html and xml mode"
	 " and the conflicts in txt mode").arg(OUTPUT_DIR);

	s+="\n\n";

	s+=TimetableGenerateForm::tr("Additional information relating impossible to schedule activities:\n\n");
	s+=TimetableGenerateForm::tr("Please check the constraints related to the last "
	 "activities in the list below, which might be impossible to schedule:\n\n");
	s+=TimetableGenerateForm::tr("Here are the placed activities which lead to an inconsistency, "
	 "in order from the first one to the last (the last one FET failed to schedule "
	 "and the last ones are most likely impossible):\n\n");
	for(int i=0; i<ot.nDifficultActivities; i++){
		int ai=ot.difficultActivities[i];

		s+=TimetableGenerateForm::tr("No: %1").arg(i+1);

		s+=", ";

		s+=TimetableGenerateForm::tr("Id: %1").arg(gt.rules.internalActivitiesList[ai].id);
		s+=TimetableGenerateForm::tr(" TN: ");
		bool first=true;
		foreach(QString tn, gt.rules.internalActivitiesList[ai].teachersNames){
			if(!first)
				s+=", ";
			first=false;
			s+=tn;
		}
		s+=", ";
		s+=TimetableGenerateForm::tr("SN: %1").arg(gt.rules.internalActivitiesList[ai].subjectName);
		s+=", ";
		first=true;
		s+=TimetableGenerateForm::tr(" StN: ");
		foreach(QString sn, gt.rules.internalActivitiesList[ai].studentsNames){
			if(!first)
				s+=", ";
			first=false;
			s+=sn;
		}

		s+="\n";
	}

	mutex.unlock();

	//show the message in a dialog
	QDialog* dialog=new QDialog();

	QVBoxLayout* vl=new QVBoxLayout(dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(TimetableGenerateForm::tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout(0);
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, SIGNAL(clicked()), dialog, SLOT(close()));

	dialog->setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - 350;
	int yy=desktop->height()/2 - 250;
	dialog->setGeometry(xx, yy, 700, 500);

	dialog->exec();
	//QTextEdit* te=new QTextEdit(s, this);
	//te->show();
	//QMessageBox::information(this, TimetableGenerateForm::tr("FET information"), s);

	startPushButton->setEnabled(TRUE);
	stopPushButton->setDisabled(TRUE);
	closePushButton->setEnabled(TRUE);
	writeResultsPushButton->setDisabled(TRUE);
}

void TimetableGenerateForm::impossibleToSolve()
{
	if(!simulation_running){
		QMessageBox::critical(this, TimetableGenerateForm::tr("FET information"),
		 TimetableGenerateForm::tr("Simulation impossible to solve, but the simulation is not running."
		 " This should not happen. Maybe you aborted simulation previously. Please report possible bug to author"));

		return;
	}

	mutex.lock();
	ot.abortOptimization=true;
	mutex.unlock();

	simulation_running=false;

	mutex.lock();



	Solution& c=ot.c;

	getStudentsTimetable(c);
	getTeachersTimetable(c);
	getRoomsTimetable(c);

	//needed to find the conflicts strings
	QString tmp;
	c.fitness(gt.rules, &tmp);

	//update the string representing the conflicts
	conflictsString = "";
	conflictsString+="Total conflicts: ";
	conflictsString+=QString::number(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString += TimetableGenerateForm::tr("Conflicts listing (in decreasing order):\n");

	foreach(QString t, c.conflictsDescriptionList)
		conflictsString+=t+"\n";

	writeSimulationResults(c);



	QString s=TimetableGenerateForm::tr("FET could not find a timetable. "
	 "Maybe you can consider lowering the constraints.");

	s+=" ";

	s+=TimetableGenerateForm::tr("The partial results are saved in the directory %1 in html and xml mode"
	 " and the conflicts in txt mode").arg(OUTPUT_DIR);

	s+="\n\n";

	s+=TimetableGenerateForm::tr("Additional information relating impossible to schedule activities:\n\n");
	/*s+=TimetableGenerateForm::tr("Maybe your constraints are too high. Especially check your students (set) early constraint, "
	 "because if you enter partial data there might be impossible to respect early, so you might "
	 "firstly generate without this constraint and see if it works\n\n");*/
	s+=TimetableGenerateForm::tr("Please check the constraints related to the "
	 "activity below, which might be impossible to schedule:\n\n");
	for(int i=0; i<ot.nDifficultActivities; i++){
		int ai=ot.difficultActivities[i];

		s+=TimetableGenerateForm::tr("No: %1").arg(i+1);

		s+=", ";

		s+=TimetableGenerateForm::tr("Id: %1").arg(gt.rules.internalActivitiesList[ai].id);
		s+=TimetableGenerateForm::tr(" TN: ");
		bool first=true;
		foreach(QString tn, gt.rules.internalActivitiesList[ai].teachersNames){
			if(!first)
				s+=", ";
			first=false;
			s+=tn;
		}
		s+=", ";
		s+=TimetableGenerateForm::tr("SN: %1").arg(gt.rules.internalActivitiesList[ai].subjectName);
		s+=", ";
		first=true;
		s+=TimetableGenerateForm::tr(" StN: ");
		foreach(QString sn, gt.rules.internalActivitiesList[ai].studentsNames){
			if(!first)
				s+=", ";
			first=false;
			s+=sn;
		}

		s+="\n";
	}

	mutex.unlock();

	//show the message in a dialog
	QDialog* dialog=new QDialog();

	QVBoxLayout* vl=new QVBoxLayout(dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(TimetableGenerateForm::tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout(0);
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, SIGNAL(clicked()), dialog, SLOT(close()));

	dialog->setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - 350;
	int yy=desktop->height()/2 - 250;
	dialog->setGeometry(xx, yy, 700, 500);

	dialog->exec();

	startPushButton->setEnabled(TRUE);
	stopPushButton->setDisabled(TRUE);
	closePushButton->setEnabled(TRUE);
	writeResultsPushButton->setDisabled(TRUE);
}

void TimetableGenerateForm::simulationFinished()
{
	if(!simulation_running){
		QMessageBox::critical(this, TimetableGenerateForm::tr("FET information"),
		 TimetableGenerateForm::tr("Simulation finished but the simulation is not running."
		 " This should not happen. Maybe you aborted simulation previously. Please report possible bug to author"));

		return;
	}

	assert(simulation_running);

	finishedSemaphore.acquire();

	//mutex.lock();

	Solution& c=ot.c;

	getStudentsTimetable(c);
	getTeachersTimetable(c);
	getRoomsTimetable(c);

	//needed to find the conflicts strings
	QString tmp;
	c.fitness(gt.rules, &tmp);

	//update the string representing the conflicts
	conflictsString = "";
	conflictsString+="Total conflicts: ";
	conflictsString+=QString::number(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString += TimetableGenerateForm::tr("Conflicts listing (in decreasing order):\n");

	foreach(QString t, c.conflictsDescriptionList)
		conflictsString+=t+"\n";

	writeSimulationResults(c);

	//mutex.unlock();

	QMessageBox::information(this, TimetableGenerateForm::tr("FET information"),
		TimetableGenerateForm::tr("Allocation terminated successfully, remaining %1 weighted"
		" conflicts from constraints with weight percentage lower than 100%"
		" (see menu Timetable/Show conflicts (time) or the text file in"
		" the output directory for details)."
		"\nSimulation results should be now written. You may check now Timetable/View."
		" The results are also saved in the directory %2 in"
		" html and xml mode and the conflicts in txt mode").arg(c.conflictsTotal).arg(OUTPUT_DIR));

	simulation_running=false;

	startPushButton->setEnabled(TRUE);
	stopPushButton->setDisabled(TRUE);
	closePushButton->setEnabled(TRUE);
	writeResultsPushButton->setEnabled(TRUE);
}

void TimetableGenerateForm::activityPlaced(int na){
	/*if(!simulation_running){
		assert(0);

		QMessageBox::critical(this, TimetableGenerateForm::tr("FET information"),
		 TimetableGenerateForm::tr("Activity placed but the simulation is not running."
		 " Maybe you aborted simulation previously. Please report possible bug to author"));

		return;
	}*/

	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);

	mutex.lock();

	//Solution& c=gt.timePopulation.bestChromosome(gt.rules);
	//Solution& c=ot.c;

	//write to the Qt interface
	QString s;
	//s = TimetableGenerateForm::tr("Population number:"); s+=QString::number(gt.timePopulation.n); s+="\n";
	//s += TimetableGenerateForm::tr("Generation:"); s+=QString::number(generation+1)+"\n";
	s+=TimetableGenerateForm::tr("%1 out of %2 activities placed").arg(na).arg(gt.rules.nInternalActivities)+"\n";

	/*double d1=c.fitness(gt.rules);
	double d2 = d1 - floor(d1/10000)*10000;
	s+=TimetableGenerateForm::tr("Constraints conflicts (without unallocated):"); s+=QString::number(d2)+"\n";*/
	//s+=TimetableGenerateForm::tr("Non-compulsory constraints conflicts:"); s+=QString::number(c.softFitness(gt.rules))+"\n";

	//s+=TimetableGenerateForm::tr("Elapsed time: %1 seconds").arg(ot.searchTime); s+="\n";
	s+=TimetableGenerateForm::tr("Elapsed time:");
	int t=ot.searchTime; //seconds
	int h=t/3600;
	if(h>0)
		s+=TimetableGenerateForm::tr(" %1 h").arg(h);
	t=t%3600;
	int m=t/60;
	if(m>0)
		s+=TimetableGenerateForm::tr(" %1 m").arg(m);
	t=t%60;
	if(t>0)
		s+=TimetableGenerateForm::tr(" %1 s").arg(t);
	s+="\n";

	s+="\n";
	s+=TimetableGenerateForm::tr("Please wait. It might take 5 to 20 minutes or even more for very difficult timetables")+"\n";
	/*s+=TimetableGenerateForm::tr("Activities are placed in order, firstly the most difficult ones, "
	 "backtracking and swapping order when stucked. When trying to place a new activity, possible swaps of already placed"
	 " activities are analysed to try to make space for the new activity")+"\n";*/
	s+=TimetableGenerateForm::tr("The process of searching is semi-randomized, which means that "
	 "you will get different timetables and running times each time. You can choose the best timetable from several runs");
	s+="\n";
	s+=TimetableGenerateForm::tr("Usually, there is no need to stop and restart the search, even if the algorithm seems stucked."
	 " Please report to author contrary cases");
	s+="\n";
	s+=TimetableGenerateForm::tr("It is recommended to strengthen the constraints step by step (for"
	 " intance min n days or teachers max gaps), as you obtain feasible timetables.");
	s+="\n";
	s+=TimetableGenerateForm::tr("For nearly impossible timetables, the time of generation might be larger."
	 " I am not sure if stop+restart might help sometimes for such timetables");

	mutex.unlock();

	currentResultsTextEdit->setText(s);

	semaphoreTime.release();
}

void TimetableGenerateForm::write(){
	mutex.lock();

	//Solution& c=gt.timePopulation.bestChromosome(gt.rules);
	Solution& c=ot.c;

	getStudentsTimetable(c);
	getTeachersTimetable(c);
	getRoomsTimetable(c);

	//needed to find the conflicts strings
	QString tmp;
	c.fitness(gt.rules, &tmp);

	//update the string representing the conflicts
	conflictsString = "";
	conflictsString+="Total conflicts: ";
	conflictsString+=QString::number(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString += TimetableGenerateForm::tr("Conflicts listing (in decreasing order):\n");

	foreach(QString t, c.conflictsDescriptionList)
		conflictsString+=t+"\n";

	writeSimulationResults(c);

	mutex.unlock();

	QMessageBox::information(this, TimetableGenerateForm::tr("FET information"),
		TimetableGenerateForm::tr("Simulation results should be now written. You may check now Timetable/View. "
		"The results are also saved in the directory %1 in html and xml mode"
		" and the conflicts in txt mode").arg(OUTPUT_DIR));
}

void TimetableGenerateForm::closePressed()
{
	if(!timeSolvingThread.isRunning())
		this->close();
}

void TimetableGenerateForm::writeSimulationResults(Solution &c){
	if(&c!=NULL)
		;

	QDir dir;

	//make sure that the input directory exists - only for GNU/Linux
	//For Windows, I make a "fet.ini" in the current working directory
/*#ifndef WIN32
	if(!dir.exists(QDir::homeDirPath()+"/.fet"))
		dir.mkdir(QDir::homeDirPath()+"/.fet");
#endif*/

	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR))
		dir.mkdir(OUTPUT_DIR);

	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	QString s;
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);

	//now write the solution in xml files
	//students
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+STUDENTS_TIMETABLE_FILENAME_XML;
	writeStudentsTimetableXml(s);
	//teachers
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+TEACHERS_TIMETABLE_FILENAME_XML;
	writeTeachersTimetableXml(s);

	//now get the time. TODO: maybe write it in xml too? so do it a few lines earlier!
	time_t ltime;
	tzset();
	time(&ltime);
	QString sTime=ctime(&ltime);

	//now get the number of placed activities. TODO: maybe write it in xml too? so do it a few lines earlier!
	int na=0;
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		if(best_solution.times[i]!=UNALLOCATED_TIME)
		na++;
	
	//write the conflicts in txt mode
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+CONFLICTS_FILENAME;
	writeConflictsTxt(s, sTime, na);
	
	//now write the solution in html files
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+STYLESHEET_CSS;
	writeStylesheetCss(s, sTime, na);
	//students
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+STUDENTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeStudentsTimetableDaysHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+STUDENTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeStudentsTimetableDaysVerticalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+STUDENTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeStudentsTimetableTimeHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+STUDENTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeStudentsTimetableTimeVerticalHtml(s, sTime, na);
	//teachers
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersTimetableDaysHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersTimetableDaysVerticalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeTeachersTimetableTimeHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeTeachersTimetableTimeVerticalHtml(s, sTime, na);
	//rooms
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeRoomsTimetableDaysHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeRoomsTimetableDaysVerticalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeRoomsTimetableTimeHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeRoomsTimetableTimeVerticalHtml(s, sTime, na);
	
	cout<<"Writing simulation results to disk completed successfully"<<endl;
}

void TimetableGenerateForm::getStudentsTimetable(Solution &c){
	//assert(gt.timePopulation.initialized);
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);

	//assert(c.HFitness()==0); - for perfect solutions
	c.getSubgroupsTimetable(gt.rules, students_timetable_weekly);
	best_solution.copy(gt.rules, c);
	students_schedule_ready=true;
}

void TimetableGenerateForm::getTeachersTimetable(Solution &c){
	//assert(gt.timePopulation.initialized);
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);

	//assert(c.HFitness()==0); - for perfect solutions
	c.getTeachersTimetable(gt.rules, teachers_timetable_weekly);
	best_solution.copy(gt.rules, c);
	teachers_schedule_ready=true;
}

void TimetableGenerateForm::getRoomsTimetable(Solution &c){
	//assert(gt.timePopulation.initialized);
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);

	//assert(c.HFitness()==0); - for perfect solutions
	c.getRoomsTimetable(gt.rules, rooms_timetable_weekly);
	best_solution.copy(gt.rules, c);
	rooms_schedule_ready=true;
}

/**
Function writing the conflicts to txt file
modified by Volker Dirr (timetabling.de) from old code by Liviu Lalescu
*/

void TimetableGenerateForm::writeConflictsTxt(const QString& filename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an CSS file
	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	//tos.setCodec("UTF-8");
	//tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);

	tos<<TimetableGenerateForm::tr("Conflicts of %1").arg(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1))<<"\n";
	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<TimetableGenerateForm::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"\n";
	tos<<TimetableGenerateForm::tr("Generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"\n\n";

	tos<<"Total conflicts: "<<ot.c.conflictsTotal<<endl<<endl;
	tos<<"Conflicts list (in decreasing order):"<<endl<<endl;
	foreach(QString t, ot.c.conflictsDescriptionList)
		tos<<t<<endl;
	tos<<endl<<TimetableGenerateForm::tr("End of file.");
	file.close();
}

/**
Function writing the students' timetable in xml format to a file
*/
void TimetableGenerateForm::writeStudentsTimetableXml(const QString& xmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an XML file
	QFile file(xmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos<<"<"<<protect(STUDENTS_TIMETABLE_TAG)<<">\n";

	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
		tos<<"\n";
		tos<< "  <Subgroup name=\"";
		QString subgroup_name = gt.rules.internalSubgroupsList[subgroup]->name;
		tos<< protect(subgroup_name) << "\">\n";

		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			tos<<"   <Day name=\""<<protect(gt.rules.daysOfTheWeek[k])<<"\">\n";
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos << "    <Hour name=\"" << protect(gt.rules.hoursOfTheDay[j]) << "\">\n";
				tos<<"     ";
				int ai=students_timetable_weekly[subgroup][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					//Activity* act=gt.rules.activitiesList.at(ai);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos<<" <Teacher name=\""<<protect(*it)<<"\"></Teacher>";
					tos<<"<Subject name=\""<<protect(act->subjectName)<<"\"></Subject>";
					tos<<"<Subject_Tag name=\""<<protect(act->subjectTagName)<<"\"></Subject_Tag>";
				}
				tos<<"\n";

				//tos<<"     <Week2>";
				//ai=students_timetable_week2[subgroup][k][j]; //activity index
				ai=UNALLOCATED_ACTIVITY;
				if(ai!=UNALLOCATED_ACTIVITY){
					//Activity* act=gt.rules.activitiesList.at(ai);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos<<" <Teacher name=\""<<protect(*it)<<"\"></Teacher>";
					tos<<"<Subject name=\""<<protect(act->subjectName)<<"\"></Subject>";
					tos<<"<Subject_Tag name=\""<<protect(act->subjectTagName)<<"\"></Subject_Tag>";
				}

				//tos<<"</Week2>\n";
				tos << "    </Hour>\n";
			}
			tos<<"   </Day>\n";
		}
		tos<<"  </Subgroup>\n";
	}

	tos<<"\n";
	tos << "</" << protect(STUDENTS_TIMETABLE_TAG) << ">\n";

	file.close();
}

/**
Function writing the teachers' timetable xml format to a file
*/
void TimetableGenerateForm::writeTeachersTimetableXml(const QString& xmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Writing the timetable in xml format
	QFile file(xmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos << "<" << protect(TEACHERS_TIMETABLE_TAG) << ">\n";

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		tos<<"\n";
		tos << "  <Teacher name=\"" << protect(gt.rules.internalTeachersList[i]->name) << "\">\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			tos << "   <Day name=\"" << protect(gt.rules.daysOfTheWeek[k]) << "\">\n";
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos << "    <Hour name=\"" << protect(gt.rules.hoursOfTheDay[j]) << "\">\n";

				tos<<"     ";
				int ai=teachers_timetable_weekly[i][k][j]; //activity index
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tos<<"<Subject name=\""<<protect(act->subjectName)<<"\"></Subject>";
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << "<Students name=\"" << protect(*it) << "\"></Students>";
				}
				tos<<"\n";

				//tos<<"     <Week2>";
				//ai=teachers_timetable_week2[i][k][j]; //activity index
				ai=UNALLOCATED_ACTIVITY;
				//act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tos<<"<Subject name=\""<<protect(act->subjectName)<<"\"></Subject>";
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << "<Students name=\"" << protect(*it) << "\"></Students>";
				}
				//tos<<"</Week2>\n";

				tos << "    </Hour>\n";
			}
			tos << "   </Day>\n";
		}
		tos<<"  </Teacher>\n";
	}

	tos<<"\n";
	tos << "</" << protect(TEACHERS_TIMETABLE_TAG) << ">\n";

	file.close();
}

//**********************************************************************************************************************/
//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
//added features: - xhtml 1.0 strict valide
//                - span using
//                - times vertical
//                - table of content with hyperlinks
//                - css support
//                - rooms


/**
Function writing the stylesheet in css format to a file by Volker Dirr.
*/
void TimetableGenerateForm::writeStylesheetCss(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an CSS file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);

	tos<<"/* "<<TimetableGenerateForm::tr("CSS Stylesheet of %1").arg(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1))<<"\n";
	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"   "<<TimetableGenerateForm::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"\n";
	tos<<"   "<<TimetableGenerateForm::tr("Stylesheet generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<" */\n\n";

	tos<<"/* "<<TimetableGenerateForm::tr("To do a page-break only after every second timetiable, cut line %1 and paste it into line %2.").arg(7).arg(14)<<" */\n";
	tos<<"table {\n  page-break-before: always;\n  text-align: center;\n  border: 1px outset black;\n}\n\n\n";
	tos<<"table.modulo2 {\n\n}\n\n\n";
	tos<<"caption {\n\n}\n\n\n";
	tos<<"thead {\n\n}\n\n\n";
	tos<<"tfoot {\n\n}\n\n\n";
	tos<<"tbody {\n\n}\n\n\n";
	tos<<"th {\n  border: 1px inset black;\n}\n\n\n";
	tos<<"td {\n  border: 1px inset black;\n}\n\n\n";

	file.close();
}


//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the students' timetable in html format to a file.
Days horizontal
*/
void TimetableGenerateForm::writeStudentsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
	tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableGenerateForm::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>"<<TimetableGenerateForm::tr("Table of content")<<"</p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		tos<<"      <li>\n        "<<TimetableGenerateForm::tr("Year")<<" "<<protect2(sty->name)<<"\n        <ul>\n";
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			tos<<"          <li>\n            "<<TimetableGenerateForm::tr("Group")<<" "<<protect2(stg->name)<<": \n";
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				tos<<"              <a href=\""<<"#table_"<<protect2id(sts->name)<<"\">"<<protect2(sts->name)<<"</a>\n";
			}
			tos<<"          </li>\n";
		}
		tos<<"        </ul>\n      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
		QString subgroup_name = gt.rules.internalSubgroupsList[subgroup]->name;
		tos<<"    <table id=\"table_"<<protect2id(subgroup_name)<<"\" border=\"1\"";
		if(subgroup%2==0) tos<<" class=\"modulo2\"";
		tos<<">\n";
				
		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<protect2(subgroup_name)<<"</th></tr>\n";
		tos<<"        <tr>\n          <!-- span -->\n";
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			tos<<"          <th>"<<protect2(gt.rules.daysOfTheWeek[j])<<"</th>\n";
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";

		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableGenerateForm::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		tos<<"      <tbody>\n";

		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			tos<<"        <tr>\n";
			tos<<"          <th>"<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";

			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				int ai=students_timetable_weekly[subgroup][k][j]; //activity index
				int currentTime=k+gt.rules.nDaysPerWeek*j;
				if(ai!=UNALLOCATED_ACTIVITY){
					if(best_solution.times[ai]==currentTime){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						tos<<"          <td";
						if(act->duration==1)
							tos<<">";
						else
							tos<<" rowspan=\""<<act->duration<<"\">";
						tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br />";
						for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
							tos<<protect2(*it);
							if(it!=act->teachersNames.end()-1)
								tos<<", ";
						}
						int r=best_solution.rooms[ai];
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							tos<<"<br />"<<gt.rules.internalRoomsList[r]->name;
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else
					tos<<"          <td>&nbsp;<br />---<br />&nbsp;</td>\n";
			}
			tos<<"        </tr>\n";
		}
		tos<<"      </tbody>\n";
		if(subgroup!=gt.rules.nInternalSubgroups-1){
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableGenerateForm::tr("back to the top")<<"</a></p>\n";
			tos<<"    <p>&nbsp;</p>\n\n";
		} else {
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableGenerateForm::tr("back to the top")<<"</a></p>\n";
		}
	}
	tos<<"  </body>\n</html>\n";
	file.close();
}


//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the students' timetable in html format to a file.
Days vertical
*/
void TimetableGenerateForm::writeStudentsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
	tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableGenerateForm::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>"<<TimetableGenerateForm::tr("Table of content")<<"</p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		tos<<"      <li>\n        "<<TimetableGenerateForm::tr("Year")<<" "<<protect2(sty->name)<<"\n        <ul>\n";
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			tos<<"          <li>\n            "<<TimetableGenerateForm::tr("Group")<<" "<<protect2(stg->name)<<": \n";
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				tos<<"              <a href=\""<<"#table_"<<protect2id(sts->name)<<"\">"<<protect2(sts->name)<<"</a>\n";
			}
			tos<<"          </li>\n";
		}
		tos<<"        </ul>\n      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n";

	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
		QString subgroup_name = gt.rules.internalSubgroupsList[subgroup]->name;
		tos<<"    <table id=\"table_"<<protect2id(subgroup_name)<<"\" border=\"1\"";
		if(subgroup%2==0) tos<<" class=\"modulo2\"";
		tos<<">\n";
		
		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<protect2(subgroup_name)<<"</th></tr>\n";
		tos<<"        <tr>\n          <!-- span -->\n";
		for(int j=0; j<gt.rules.nHoursPerDay; j++)
			tos<<"          <th>"<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";

		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableGenerateForm::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		tos<<"      <tbody>\n";

		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			tos<<"        <tr>\n";
			tos<<"          <th>"<<protect2(gt.rules.daysOfTheWeek[k])<<"</th>\n";

			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				int ai=students_timetable_weekly[subgroup][k][j]; //activity index
				int currentTime=k+gt.rules.nDaysPerWeek*j;
				if(ai!=UNALLOCATED_ACTIVITY){
					if(best_solution.times[ai]==currentTime){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						tos<<"          <td";
						if(act->duration==1)
							tos<<">";
						else
							tos<<" colspan=\""<<act->duration<<"\">";
						tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br />";
						for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
							tos<<protect2(*it);
							if(it!=act->teachersNames.end()-1)
								tos<<", ";
						}
						int r=best_solution.rooms[ai];
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							tos<<"<br />"<<gt.rules.internalRoomsList[r]->name;
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				}
				else
					tos<<"          <td>&nbsp;<br />---<br />&nbsp;</td>\n";
			}
			tos<<"        </tr>\n";
		}
		tos<<"      </tbody>\n";
		if(subgroup!=gt.rules.nInternalSubgroups-1){
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableGenerateForm::tr("back to the top")<<"</a></p>\n";
			tos<<"    <p>&nbsp;</p>\n\n";
		} else {
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableGenerateForm::tr("back to the top")<<"</a></p>\n";
		}
	}

	tos<<"  </body>\n</html>\n";

	file.close();
}


//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the students' timetable html format to a file
Time vertical
*/

void TimetableGenerateForm::writeStudentsTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
	tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableGenerateForm::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <table border=\"1\">\n";
	tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

	tos<<"      <thead>\n        <tr><td colspan=\"2\"></td>";
	for(int i=0; i<gt.rules.nInternalSubgroups; i++)
		tos << "<th>" << gt.rules.internalSubgroupsList[i]->name << "</th>";
	tos<<"</tr>\n      </thead>\n";

	tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalSubgroups<<"\">"<<TimetableGenerateForm::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
	tos<<"      <tbody>\n";

	for(int k=0; k<gt.rules.nDaysPerWeek; k++){
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			tos << "        <tr>\n";
			if(j==0)
				tos << "        <th rowspan=\"" << gt.rules.nHoursPerDay  << "\">" << protect2vert(gt.rules.daysOfTheWeek[k]) << "</th>\n";
			else tos <<"          <!-- span -->\n";

			tos << "          <th>" << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
			for(int i=0; i<gt.rules.nInternalSubgroups; i++){
				int currentTime=k+gt.rules.nDaysPerWeek*j;
				int ai=students_timetable_weekly[i][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					if(best_solution.times[ai]==currentTime){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						tos<<"          <td";
						if(act->duration==1)
							tos<<">";
						else
							tos<<" rowspan=\""<<act->duration<<"\">";
						tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br />";
						for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
							tos<<protect2(*it);
							if(it!=act->teachersNames.end()-1)
								tos<<", ";
						}
						int r=best_solution.rooms[ai];
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							tos<<"<br />"<<gt.rules.internalRoomsList[r]->name;
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else
					tos<<"          <td>&nbsp;<br />---<br />&nbsp;</td>\n";
			}
			tos<<"        </tr>\n";
		}
	}
	tos << "      </tbody>\n    </table>\n";
	tos << "  </body>\n</html>\n";
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the students' timetable in html format to a file.
Time horizontal
*/
void TimetableGenerateForm::writeStudentsTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
	tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableGenerateForm::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <table border=\"1\">\n";
	tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

	tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td>";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		tos << "<th colspan=\"" << gt.rules.nHoursPerDay <<"\">" << protect2(gt.rules.daysOfTheWeek[k]) << "</th>";
	tos<<"        </tr>\n";
	tos<<"        <tr>\n          <!-- span -->\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		for(int j=0; j<gt.rules.nHoursPerDay; j++)
			tos << "          <th>" << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
	tos<<"        </tr>\n";
	tos<<"      </thead>\n";

	tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek<<"\">"<<TimetableGenerateForm::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
	tos<<"      <tbody>\n";

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		tos << "        <tr>\n";
		tos << "          <th>" << gt.rules.internalSubgroupsList[i]->name << "</th>\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				int currentTime=k+gt.rules.nDaysPerWeek*j;
				int ai=students_timetable_weekly[i][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					if(best_solution.times[ai]==currentTime){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						tos<<"          <td";
						if(act->duration==1)
							tos<<">";
						else
							tos<<" colspan=\""<<act->duration<<"\">";
						tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br />";
						for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
							tos<<protect2(*it);
							if(it!=act->teachersNames.end()-1)
								tos<<", ";
						}
						int r=best_solution.rooms[ai];
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							tos<<"<br />"<<gt.rules.internalRoomsList[r]->name;
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else
					tos<<"          <td>&nbsp;<br />---<br />&nbsp;</td>\n";
			}
		}
		tos<<"        </tr>\n";
	}
	tos << "      </tbody>\n    </table>\n";
	tos << "  </body>\n</html>\n";
	file.close();
}



//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the teachers' timetable html format to a file.
Days horizontal.
*/
void TimetableGenerateForm::writeTeachersTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
	tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableGenerateForm::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>"<<TimetableGenerateForm::tr("Table of content")<<"</p>\n";
	tos<<"    <ul>\n";
	for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
		QString teacher_name = gt.rules.internalTeachersList[teacher]->name;
		tos<<"      <li><a href=\""<<"#table_"<<protect2id(teacher_name)<<"\">"<<protect2(teacher_name)<<"</a></li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
		QString teacher_name = gt.rules.internalTeachersList[teacher]->name;
		tos<<"    <table id=\"table_"<<protect2id(teacher_name)<<"\" border=\"1\"";
		if(teacher%2==0) tos<<" class=\"modulo2\"";
		tos<<">\n";
		
		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<protect2(teacher_name)<<"</th></tr>\n";
		tos<<"        <tr>\n          <!-- span -->\n";
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			tos<<"          <th>"<<protect2(gt.rules.daysOfTheWeek[j])<<"</th>\n";
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";

		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableGenerateForm::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		tos<<"      <tbody>\n";

		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			tos<<"        <tr>\n";
			tos<<"          <th>"<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";

			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				int ai=teachers_timetable_weekly[teacher][k][j]; //activity index
				int currentTime=k+gt.rules.nDaysPerWeek*j;
				if(ai!=UNALLOCATED_ACTIVITY){
					if(best_solution.times[ai]==currentTime){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						tos<<"          <td";
						if(act->duration==1)
							tos<<">";
						else
							tos<<" rowspan=\""<<act->duration<<"\">";
						for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
							tos<<protect2(*st);
							if(st!=act->studentsNames.end()-1)
								tos<<", ";
						}
						tos<<"<br />";
						tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName);
						int r=best_solution.rooms[ai];
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							tos<<"<br />"<<gt.rules.internalRoomsList[r]->name;
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else
					tos<<"          <td>&nbsp;<br />---<br />&nbsp;</td>\n";
			}
			tos<<"        </tr>\n";
		}
		tos<<"      </tbody>\n";
		if(teacher!=gt.rules.nInternalTeachers-1){
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableGenerateForm::tr("back to the top")<<"</a></p>\n";
			tos<<"    <p>&nbsp;</p>\n\n";
		} else {
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableGenerateForm::tr("back to the top")<<"</a></p>\n";
		}
	}
	tos<<"  </body>\n</html>\n";
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the teachers' timetable html format to a file.
Days vertical.
*/
void TimetableGenerateForm::writeTeachersTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
	tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1> "<<TimetableGenerateForm::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>"<<TimetableGenerateForm::tr("Table of content")<<"</p>\n";
	tos<<"    <ul>\n";
	for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
		QString teacher_name = gt.rules.internalTeachersList[teacher]->name;
		tos<<"      <li><a href=\""<<"#table_"<<protect2id(teacher_name)<<"\">"<<protect2(teacher_name)<<"</a></li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
		QString teacher_name = gt.rules.internalTeachersList[teacher]->name;
		tos<<"    <table id=\"table_"<<protect2id(teacher_name)<<"\" border=\"1\"";
		if(teacher%2==0) tos<<" class=\"modulo2\"";
		tos<<">\n";
		
		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		tos<<"      <thead>\n";
		tos<<"        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<protect2(teacher_name)<<"</th></tr>\n";
		tos<<"        <tr>\n          <!-- span -->\n";
		for(int j=0; j<gt.rules.nHoursPerDay; j++)
			tos<<"          <th>"<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";

		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableGenerateForm::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		tos<<"      <tbody>\n";

		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			tos<<"        <tr>\n";
			tos<<"          <th>"<<protect2(gt.rules.daysOfTheWeek[k])<<"</th>\n";

			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				int ai=teachers_timetable_weekly[teacher][k][j]; //activity index
				int currentTime=k+gt.rules.nDaysPerWeek*j;
				if(ai!=UNALLOCATED_ACTIVITY){
					if(best_solution.times[ai]==currentTime){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						tos<<"          <td";
						if(act->duration==1)
							tos<<">";
						else
							tos<<" colspan=\""<<act->duration<<"\">";
						for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
							tos<<protect2(*st);
							if(st!=act->studentsNames.end()-1)
								tos<<", ";
						}
						tos<<"<br />";
						tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName);
						int r=best_solution.rooms[ai];
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							tos<<"<br />"<<gt.rules.internalRoomsList[r]->name;
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				}
				else
					tos<<"          <td>&nbsp;<br />---<br />&nbsp;</td>\n";
			}
			tos<<"        </tr>\n";
		}
		tos<<"      </tbody>\n";
		if(teacher!=gt.rules.nInternalTeachers-1){
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableGenerateForm::tr("back to the top")<<"</a></p>\n";
			tos<<"    <p>&nbsp;</p>\n\n";
		} else {
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableGenerateForm::tr("back to the top")<<"</a></p>\n";
		}
	}
	tos<<"  </body>\n</html>\n";
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code was done by Liviu Lalescu)
/**
Function writing the teachers' timetable html format to a file
Time vertical
*/
void TimetableGenerateForm::writeTeachersTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
	tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableGenerateForm::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <table border=\"1\">\n";
	tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

	tos<<"      <thead>\n        <tr><td colspan=\"2\"></td>";
	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		tos << "<th>" << gt.rules.internalTeachersList[i]->name << "</th>";
	tos<<"</tr>\n      </thead>\n";

	tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalTeachers<<"\">"<<TimetableGenerateForm::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
	tos<<"      <tbody>\n";

	for(int k=0; k<gt.rules.nDaysPerWeek; k++){
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			tos << "        <tr>\n";
			if(j==0)
				tos << "        <th rowspan=\"" << gt.rules.nHoursPerDay  << "\">" << protect2vert(gt.rules.daysOfTheWeek[k]) << "</th>\n";
			else tos <<"          <!-- span -->\n";
				tos << "          <th>" << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
			for(int i=0; i<gt.rules.nInternalTeachers; i++){
				int currentTime=k+gt.rules.nDaysPerWeek*j;
				int ai=teachers_timetable_weekly[i][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					if(best_solution.times[ai]==currentTime){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						tos<<"          <td";
						if(act->duration==1)
							tos<<">";
						else
							tos<<" rowspan=\""<<act->duration<<"\">";
						for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
							tos<<protect2(*st);
							if(st!=act->studentsNames.end()-1)
								tos<<", ";
						}
						tos<<"<br />";
						tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName);
						int r=best_solution.rooms[ai];
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							tos<<"<br />"<<gt.rules.internalRoomsList[r]->name;
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else
					tos<<"          <td>&nbsp;<br />---<br />&nbsp;</td>\n";
			}
			tos<<"        </tr>\n";
		}
	}
	tos << "      </tbody>\n    </table>\n";
	tos << "  </body>\n</html>\n";
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the teachers' timetable html format to a file.
Time horizontal
*/
void TimetableGenerateForm::writeTeachersTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
	tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableGenerateForm::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <table border=\"1\">\n";
	tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

	tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td>";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		tos << "<th colspan=\"" << gt.rules.nHoursPerDay <<"\">" << protect2(gt.rules.daysOfTheWeek[k]) << "</th>";
	tos <<"</tr>\n";
	tos<<"        <tr>\n          <!-- span -->\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		for(int j=0; j<gt.rules.nHoursPerDay; j++)
			tos << "          <th>" << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
	tos<<"        </tr>\n";
	tos<<"      </thead>\n";

	tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek<<"\">"<<TimetableGenerateForm::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
	tos<<"      <tbody>\n";

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		tos << "        <tr>\n";
		tos << "          <th>" << gt.rules.internalTeachersList[i]->name << "</th>\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				int currentTime=k+gt.rules.nDaysPerWeek*j;
				int ai=teachers_timetable_weekly[i][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					if(best_solution.times[ai]==currentTime){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						tos<<"          <td";
						if(act->duration==1)
							tos<<">";
						else
							tos<<" colspan=\""<<act->duration<<"\">";
						for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
							tos<<protect2(*st);
							if(st!=act->studentsNames.end()-1)
								tos<<", ";
						}
						tos<<"<br />";
						tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName);
						int r=best_solution.rooms[ai];
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							tos<<"<br />"<<gt.rules.internalRoomsList[r]->name;
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else
					tos<<"          <td>&nbsp;<br />---<br />&nbsp;</td>\n";
			}
		}
		tos<<"        </tr>\n";
	}
	tos << "      </tbody>\n    </table>\n";
	tos << "  </body>\n</html>\n";
	file.close();
}

/**
Function writing the rooms' timetable html format to a file by Volker Dirr
Days horizontal.
*/
void TimetableGenerateForm::writeRoomsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
	tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableGenerateForm::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<TimetableGenerateForm::tr("No rooms recorded in fet for %1.").arg(protect2(gt.rules.institutionName))<<"</h1>\n";
	else {
		tos<<"    <p>"<<TimetableGenerateForm::tr("Table of content")<<"</p>\n";
		tos<<"    <ul>\n";
		for(int room=0; room<gt.rules.nInternalRooms; room++){
			QString room_name = gt.rules.internalRoomsList[room]->name;
			tos<<"      <li><a href=\""<<"#table_"<<protect2id(room_name)<<"\">"<<protect2(room_name)<<"</a></li>\n";
		}
		tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

		for(int room=0; room<gt.rules.nInternalRooms; room++){
			QString room_name = gt.rules.internalRoomsList[room]->name;
			tos<<"    <table id=\"table_"<<protect2id(room_name)<<"\" border=\"1\"";
			if(room%2==0) tos<<" class=\"modulo2\"";
			tos<<">\n";
		
			tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

			tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<protect2(room_name)<<"</th></tr>\n";
			tos<<"        <tr>\n          <!-- span -->\n";
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				tos<<"          <th>"<<protect2(gt.rules.daysOfTheWeek[j])<<"</th>\n";
			tos<<"        </tr>\n";
			tos<<"      </thead>\n";

			tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableGenerateForm::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
			tos<<"      <tbody>\n";

			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos<<"        <tr>\n";
				tos<<"          <th>"<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
				for(int k=0; k<gt.rules.nDaysPerWeek; k++){
					int ai=rooms_timetable_weekly[room][k][j]; //activity index
					int currentTime=k+gt.rules.nDaysPerWeek*j;
					if(ai!=UNALLOCATED_ACTIVITY){
						if(best_solution.times[ai]==currentTime){
							Activity* act=&gt.rules.internalActivitiesList[ai];
							tos<<"          <td";
							if(act->duration==1)
								tos<<">";
							else
								tos<<" rowspan=\""<<act->duration<<"\">";
							for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
								tos<<protect2(*st);
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
							tos<<"<br />";
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								tos<<protect2(*it);
								if(it!=act->teachersNames.end()-1)
									tos<<", ";
							}
							tos<<"<br />";
							tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName);
							tos<<"</td>\n";
						} else
							tos<<"          <!-- span -->\n";
					} else
						tos<<"          <td>&nbsp;<br />---<br />&nbsp;</td>\n";
				}
				tos<<"        </tr>\n";
			}
			tos<<"      </tbody>\n";
			if(room!=gt.rules.nInternalRooms-1){
				tos<<"    </table>\n    <p>&nbsp;</p>\n";
				tos<<"    <p><a href=\""<<"#top\">"<<TimetableGenerateForm::tr("back to the top")<<"</a></p>\n";
				tos<<"    <p>&nbsp;</p>\n\n";
			} else {
				tos<<"    </table>\n    <p>&nbsp;</p>\n";
				tos<<"    <p><a href=\""<<"#top\">"<<TimetableGenerateForm::tr("back to the top")<<"</a></p>\n";
			}
		}
	}
	tos<<"  </body>\n</html>\n";
	file.close();
}

/**
Function writing the rooms' timetable html format to a file by Volker Dirr
Days vertical.
*/
void TimetableGenerateForm::writeRoomsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
	tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1> "<<TimetableGenerateForm::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<TimetableGenerateForm::tr("No rooms recorded in fet for %1.").arg(protect2(gt.rules.institutionName))<<"</h1>\n";
	else {
		tos<<"    <p>"<<TimetableGenerateForm::tr("Table of content")<<"</p>\n";
		tos<<"    <ul>\n";
		for(int room=0; room<gt.rules.nInternalRooms; room++){
			QString room_name = gt.rules.internalRoomsList[room]->name;
			tos<<"      <li><a href=\""<<"#table_"<<protect2id(room_name)<<"\">"<<protect2(room_name)<<"</a></li>\n";
		}
		tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

		for(int room=0; room<gt.rules.nInternalRooms; room++){
			QString room_name = gt.rules.internalRoomsList[room]->name;
			tos<<"    <table id=\"table_"<<protect2id(room_name)<<"\" border=\"1\"";
			if(room%2==0) tos<<" class=\"modulo2\"";
			tos<<">\n";
			
			tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

			tos<<"      <thead>\n";
			tos<<"        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<protect2(room_name)<<"</th></tr>\n";
			tos<<"        <tr>\n          <!-- span -->\n";
			for(int j=0; j<gt.rules.nHoursPerDay; j++)
				tos<<"          <th>"<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
			tos<<"        </tr>\n";
			tos<<"      </thead>\n";

			tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableGenerateForm::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
			tos<<"      <tbody>\n";

			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				tos<<"        <tr>\n";
				tos<<"          <th>"<<protect2(gt.rules.daysOfTheWeek[k])<<"</th>\n";
				for(int j=0; j<gt.rules.nHoursPerDay; j++){
					int ai=rooms_timetable_weekly[room][k][j]; //activity index
					int currentTime=k+gt.rules.nDaysPerWeek*j;
					if(ai!=UNALLOCATED_ACTIVITY){
						if(best_solution.times[ai]==currentTime){
							Activity* act=&gt.rules.internalActivitiesList[ai];
							tos<<"          <td";
							if(act->duration==1)
								tos<<">";
							else
								tos<<" colspan=\""<<act->duration<<"\">";
							for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
								tos<<protect2(*st);
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
							tos<<"<br />";
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								tos<<protect2(*it);
								if(it!=act->teachersNames.end()-1)
									tos<<", ";
							}
							tos<<"<br />";
							tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName);
							tos<<"</td>\n";
						} else
							tos<<"          <!-- span -->\n";
					}
					else
						tos<<"          <td>&nbsp;<br />---<br />&nbsp;</td>\n";
				}
				tos<<"        </tr>\n";
			}
			tos<<"      </tbody>\n";
			if(room!=gt.rules.nInternalRooms-1){
				tos<<"    </table>\n    <p>&nbsp;</p>\n";
				tos<<"    <p><a href=\""<<"#top\">"<<TimetableGenerateForm::tr("back to the top")<<"</a></p>\n";
				tos<<"    <p>&nbsp;</p>\n\n";
			} else {
				tos<<"    </table>\n    <p>&nbsp;</p>\n";
				tos<<"    <p><a href=\""<<"#top\">"<<TimetableGenerateForm::tr("back to the top")<<"</a></p>\n";
			}
		}
	}
	tos<<"  </body>\n</html>\n";
	file.close();
}


/**
Function writing the rooms' timetable html format to a file by Volker Dirr
Time vertical.
*/
void TimetableGenerateForm::writeRoomsTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
	tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableGenerateForm::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";
	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<TimetableGenerateForm::tr("No rooms recorded in fet for %1.").arg(protect2(gt.rules.institutionName))<<"</h1>\n";
	else {
		tos<<"    <table border=\"1\">\n";
		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		tos<<"      <thead>\n        <tr><td colspan=\"2\"></td>";
		for(int i=0; i<gt.rules.nInternalRooms; i++)
			tos << "<th>" << gt.rules.internalRoomsList[i]->name << "</th>";
		tos<<"</tr>\n      </thead>\n";

		tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalRooms<<"\">"<<TimetableGenerateForm::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		tos<<"      <tbody>\n";

		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos << "        <tr>\n";
				if(j==0)
					tos << "        <th rowspan=\"" << gt.rules.nHoursPerDay  << "\">" << protect2vert(gt.rules.daysOfTheWeek[k]) << "</th>\n";
				else tos <<"          <!-- span -->\n";
					tos << "          <th>" << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
				for(int i=0; i<gt.rules.nInternalRooms; i++){
					int currentTime=k+gt.rules.nDaysPerWeek*j;
					int ai=rooms_timetable_weekly[i][k][j]; //activity index
					if(ai!=UNALLOCATED_ACTIVITY){
						if(best_solution.times[ai]==currentTime){
							Activity* act=&gt.rules.internalActivitiesList[ai];
							tos<<"          <td";
							if(act->duration==1)
								tos<<">";
							else
								tos<<" rowspan=\""<<act->duration<<"\">";
							for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
								tos<<protect2(*st);
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
							tos<<"<br />";
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								tos<<protect2(*it);
								if(it!=act->teachersNames.end()-1)
									tos<<", ";
							}
							tos<<"<br />";
							tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName);
							tos<<"</td>\n";
						} else
							tos<<"          <!-- span -->\n";
					} else
						tos<<"          <td>&nbsp;<br />---<br />&nbsp;</td>\n";
				}
				tos<<"        </tr>\n";
			}
		}
		tos << "      </tbody>\n    </table>\n";
	}
	tos << "  </body>\n</html>\n";
	file.close();
}


/**
Function writing the rooms' timetable html format to a file by Volker Dirr
Time vertical.
*/
void TimetableGenerateForm::writeRoomsTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
	tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableGenerateForm::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<TimetableGenerateForm::tr("No rooms recorded in fet for %1.").arg(protect2(gt.rules.institutionName))<<"</h1>\n";
	else {
		tos<<"    <table border=\"1\">\n";
		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td>";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++)
			tos << "<th colspan=\"" << gt.rules.nHoursPerDay <<"\">" << protect2(gt.rules.daysOfTheWeek[k]) << "</th>";
		tos <<"</tr>\n";
		tos<<"        <tr>\n          <!-- span -->\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++)
			for(int j=0; j<gt.rules.nHoursPerDay; j++)
				tos << "          <th>" << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";

		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek<<"\">"<<TimetableGenerateForm::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		tos<<"      <tbody>\n";

		for(int i=0; i<gt.rules.nInternalRooms; i++){
			tos << "        <tr>\n";
			tos << "          <th>" << gt.rules.internalRoomsList[i]->name << "</th>\n";
			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				for(int j=0; j<gt.rules.nHoursPerDay; j++){
					int currentTime=k+gt.rules.nDaysPerWeek*j;
					int ai=rooms_timetable_weekly[i][k][j]; //activity index
					if(ai!=UNALLOCATED_ACTIVITY){
						if(best_solution.times[ai]==currentTime){
							Activity* act=&gt.rules.internalActivitiesList[ai];
							tos<<"          <td";
							if(act->duration==1)
								tos<<">";
							else
								tos<<" colspan=\""<<act->duration<<"\">";
							for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
								tos<<protect2(*st);
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
							tos<<"<br />";
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								tos<<protect2(*it);
								if(it!=act->teachersNames.end()-1)
									tos<<", ";
							}
							tos<<"<br />";
							tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName);
							tos<<"</td>\n";
						} else
							tos<<"          <!-- span -->\n";
					} else
						tos<<"          <td>&nbsp;<br />---<br />&nbsp;</td>\n";
				}
			}
			tos<<"        </tr>\n";
		}
		tos << "      </tbody>\n    </table>\n";
	}
	tos << "  </body>\n</html>\n";
	file.close();
}
