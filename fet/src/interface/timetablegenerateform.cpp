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

#include "generate.h"

#include "timetablegenerateform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "timetableexport.h"

#include <qpushbutton.h>
#include <qstring.h>

#include <QDesktopWidget>

#include <QTextEdit>

#include <iostream>
#include <fstream>
using namespace std;

#include <QMessageBox>

#include <QMutex>

QMutex mutex;

static GenerateThread generateThread;

#include <QSemaphore>

QSemaphore semaphorePlacedActivity; //used to update when an activity is placed

QSemaphore finishedSemaphore;

//Represents the current status of the simulation - running or stopped.
extern bool simulation_running;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

extern Solution best_solution;

extern QString conflictsString;


Generate gen;


void GenerateThread::run()
{
	const int INF=2000000000;
	bool impossible, timeExceeded;

	gen.generate(INF, impossible, timeExceeded);
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

	connect(&gen, SIGNAL(activityPlaced(int)),
	 this, SLOT(activityPlaced(int)));
	connect(&gen, SIGNAL(simulationFinished()),
	 this, SLOT(simulationFinished()));
	connect(&gen, SIGNAL(impossibleToSolve()),
	 this, SLOT(impossibleToSolve()));

	this->setAttribute(Qt::WA_DeleteOnClose);
}

TimetableGenerateForm::~TimetableGenerateForm()
{
	if(simulation_running)
		this->stop();
}

void TimetableGenerateForm::start(){
	if(!gt.rules.internalStructureComputed){
		if(!gt.rules.computeInternalStructure()){
			QMessageBox::warning(this, TimetableGenerateForm::tr("FET warning"), TimetableGenerateForm::tr("Data is wrong. Please correct and try again"));
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

	currentResultsTextEdit->setText(TimetableGenerateForm::tr("Entering simulation....precomputing"));
	currentResultsTextEdit->repaint();

	gen.abortOptimization=false;
	bool ok=gen.precompute();

	if(!ok){
		currentResultsTextEdit->setText(TimetableGenerateForm::tr("Cannot generate - please modify your data"));
		currentResultsTextEdit->repaint();

		QMessageBox::information(this, TimetableGenerateForm::tr("FET information"),
		 TimetableGenerateForm::tr("Your data cannot be processed - please modify it as instructed"
		 "\nFor more information you can join the mailing list or write to author"));

		return;
	}

	startPushButton->setDisabled(TRUE);
	stopPushButton->setEnabled(TRUE);
	closePushButton->setDisabled(TRUE);
	writeResultsPushButton->setEnabled(TRUE);

	simulation_running=true;

	generateThread.start();
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
	gen.abortOptimization=true;
	mutex.unlock();

	simulation_running=false;

	mutex.lock();

	Solution& c=gen.c;

	//needed to find the conflicts strings
	QString tmp;
	c.fitness(gt.rules, &tmp);

	TimetableExport::getStudentsTimetable(c);
	TimetableExport::getTeachersTimetable(c);
	TimetableExport::getRoomsTimetable(c);

	//update the string representing the conflicts
	conflictsString = "";
	conflictsString+="Total conflicts: ";
	conflictsString+=QString::number(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString += TimetableGenerateForm::tr("Conflicts listing (in decreasing order):\n");

	foreach(QString t, c.conflictsDescriptionList)
		conflictsString+=t+"\n";

	TimetableExport::writeSimulationResults();

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
	for(int i=0; i<gen.nDifficultActivities; i++){
		int ai=gen.difficultActivities[i];

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

void TimetableGenerateForm::impossibleToSolve()
{
	if(!simulation_running){
		QMessageBox::critical(this, TimetableGenerateForm::tr("FET information"),
		 TimetableGenerateForm::tr("Simulation impossible to solve, but the simulation is not running."
		 " This should not happen. Maybe you aborted simulation previously. Please report possible bug to author"));

		return;
	}

	mutex.lock();
	gen.abortOptimization=true;
	mutex.unlock();

	simulation_running=false;

	mutex.lock();



	Solution& c=gen.c;

	//needed to find the conflicts strings
	QString tmp;
	c.fitness(gt.rules, &tmp);

	TimetableExport::getStudentsTimetable(c);
	TimetableExport::getTeachersTimetable(c);
	TimetableExport::getRoomsTimetable(c);

	//update the string representing the conflicts
	conflictsString = "";
	conflictsString+="Total conflicts: ";
	conflictsString+=QString::number(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString += TimetableGenerateForm::tr("Conflicts listing (in decreasing order):\n");

	foreach(QString t, c.conflictsDescriptionList)
		conflictsString+=t+"\n";

	TimetableExport::writeSimulationResults();


	QString s=TimetableGenerateForm::tr("FET could not find a timetable. "
	 "Maybe you can consider lowering the constraints.");

	s+=" ";

	s+=TimetableGenerateForm::tr("The partial results are saved in the directory %1 in html and xml mode"
	 " and the conflicts in txt mode").arg(OUTPUT_DIR);

	s+="\n\n";

	s+=TimetableGenerateForm::tr("Additional information relating impossible to schedule activities:\n\n");
	s+=TimetableGenerateForm::tr("Please check the constraints related to the "
	 "activity below, which might be impossible to schedule:\n\n");
	for(int i=0; i<gen.nDifficultActivities; i++){
		int ai=gen.difficultActivities[i];

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

	Solution& c=gen.c;

	//needed to find the conflicts strings
	QString tmp;
	c.fitness(gt.rules, &tmp);

	TimetableExport::getStudentsTimetable(c);
	TimetableExport::getTeachersTimetable(c);
	TimetableExport::getRoomsTimetable(c);

	//update the string representing the conflicts
	conflictsString = "";
	conflictsString+="Total soft conflicts: ";
	conflictsString+=QString::number(c.conflictsTotal);
	conflictsString+="\n";
	conflictsString += TimetableGenerateForm::tr("Soft conflicts listing (in decreasing order):\n");

	foreach(QString t, c.conflictsDescriptionList)
		conflictsString+=t+"\n";

	TimetableExport::writeSimulationResults();

	//mutex.unlock();

	QMessageBox::information(this, TimetableGenerateForm::tr("FET information"),
		TimetableGenerateForm::tr("Allocation terminated successfully, remaining %1 weighted"
		" soft conflicts from constraints with weight percentage lower than 100%"
		" (see menu Timetable/Show soft conflicts or the text file in"
		" the output directory for details)."
		"\n\nSimulation results should be now written. You may check now Timetable/View."
		" The results are also saved in the directory %2 in"
		" html and xml mode and the soft conflicts in txt mode").arg(c.conflictsTotal).arg(OUTPUT_DIR));

	simulation_running=false;

	startPushButton->setEnabled(TRUE);
	stopPushButton->setDisabled(TRUE);
	closePushButton->setEnabled(TRUE);
	writeResultsPushButton->setEnabled(TRUE);
}

void TimetableGenerateForm::activityPlaced(int na){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);

	mutex.lock();

	//write to the Qt interface
	QString s;
	s+=TimetableGenerateForm::tr("%1 out of %2 activities placed").arg(na).arg(gt.rules.nInternalActivities)+"\n";

	s+=TimetableGenerateForm::tr("Elapsed time:");
	int t=gen.searchTime; //seconds
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

	mutex.unlock();

	currentResultsTextEdit->setText(s);

	semaphorePlacedActivity.release();
}

void TimetableGenerateForm::help()
{
	QString s="";
	
	s+=TimetableGenerateForm::tr("Please wait. It might take 5 to 20 minutes or even more for very difficult timetables")+"\n";
	s+=TimetableGenerateForm::tr("Activities are placed in order, most difficult ones first"); s+="\n";
	s+=TimetableGenerateForm::tr("The process of searching is semi-randomized, which means that "
	 "you will get different timetables and running times each time. You can choose the best timetable from several runs");
	s+="\n";
	s+=TimetableGenerateForm::tr("Usually, there is no need to stop and restart the search, even if the algorithm seems stucked."
	 " Please report to author contrary cases");
	s+="\n";
	s+=TimetableGenerateForm::tr("It is recommended to strengthen the constraints step by step (for"
	 " intance min n days or teachers max gaps), as you obtain feasible timetables.");
	 
	QMessageBox::information(this, tr("FET help"), s);
}

void TimetableGenerateForm::write(){
	mutex.lock();

	Solution& c=gen.c;

	TimetableExport::getStudentsTimetable(c);
	TimetableExport::getTeachersTimetable(c);
	TimetableExport::getRoomsTimetable(c);

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

	TimetableExport::writeSimulationResults();

	mutex.unlock();

	QMessageBox::information(this, TimetableGenerateForm::tr("FET information"),
		TimetableGenerateForm::tr("Simulation results should now be written in the directory %1 in html and xml mode"
		" and the conflicts in txt mode").arg(OUTPUT_DIR));
}

void TimetableGenerateForm::closePressed()
{
	if(!generateThread.isRunning())
		this->close();
}
