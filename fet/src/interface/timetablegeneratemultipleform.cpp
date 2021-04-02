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

extern QMutex mutex;

static GenerateMultipleThread generateMultipleThread;

#include <QSemaphore>

static QSemaphore semaphoreTimetableFinished; //used to update when an activity is placed

//Represents the current status of the simulation - running or stopped.
extern bool simulation_running_multi;

extern QSemaphore semaphorePlacedActivity;

Generate genMulti;

static int nTimetables;
static int timeLimit;

extern Solution best_solution;

extern QString conflictsString;

time_t start_time;

void GenerateMultipleThread::run()
{
	genMulti.abortOptimization=false;

	for(int i=0; i<nTimetables; i++){
		time(&start_time);
	
		bool impossible;
		bool timeExceeded;
		bool ok=genMulti.precompute(); //here I can speed up, precomputation is only needed to restore permutation of activities
		assert(ok);
		//cout<<"timeLimit=="<<timeLimit<<endl;

		genMulti.generate(timeLimit, impossible, timeExceeded);

		QString s;

		mutex.lock();
		if(genMulti.abortOptimization){
			mutex.unlock();
			return;
		}
		else if(impossible){
			s=tr("Timetable impossible to generate");
		}
		else if(timeExceeded){
			s=tr("Time exceeded for current timetable");
		}
		else{
			time_t finish_time;
			time(&finish_time);
			int seconds=finish_time-start_time;
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
		
		emit(timetableGenerated(i+1, s));
		semaphoreTimetableFinished.acquire();
	}
	
	emit(finished());
}

TimetableGenerateMultipleForm::TimetableGenerateMultipleForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	simulation_running_multi=false;

	startPushButton->setEnabled(TRUE);
	stopPushButton->setDisabled(TRUE);
	closePushButton->setEnabled(TRUE);
	minutesGroupBox->setEnabled(TRUE);
	timetablesGroupBox->setEnabled(TRUE);

	connect(&generateMultipleThread, SIGNAL(timetableGenerated(int, const QString&)),
		this, SLOT(timetableGenerated(int, const QString&)));

	connect(&generateMultipleThread, SIGNAL(finished()),
		this, SLOT(finished()));

	connect(&genMulti, SIGNAL(activityPlaced(int)),
		this, SLOT(activityPlaced(int)));

	this->setAttribute(Qt::WA_DeleteOnClose);
}

TimetableGenerateMultipleForm::~TimetableGenerateMultipleForm()
{
	if(simulation_running_multi)
		this->stop();
}

void TimetableGenerateMultipleForm::help()
{
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	QString destDir=OUTPUT_DIR+FILE_SEP+s2;

	QMessageBox::information(this, tr("FET information"), tr("Notice: you can only see generated timetables on the hard disk,"
	 " in html and xml formats and soft conflicts in txt format, or latest timetable in the FET Timetable/View menu."
	 " It is needed that the directory"
	 " %1 to be emptied+deleted before proceeeding.\n\nPlease note that, for large data, each timetable might occupy more"
	 " megabytes of hard disk space,"
	 " so make sure you have enough space (you can check the dimension of a single timetable as a precaution).")
	 .arg(destDir));
}

void TimetableGenerateMultipleForm::start(){
	nTimetables=timetablesSpinBox->value();
	assert(nTimetables>0);
	timeLimit=60*minutesSpinBox->value(); //seconds
	assert(timeLimit>0);

	QDir dir;
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	QString destDir=OUTPUT_DIR+FILE_SEP+s2;
	if(dir.exists(destDir)){
		QMessageBox::warning(this, tr("FET information"), tr("Cannot proceeed, directory %1 exists and might not be empty,"
		 " (it might contain old files). You need to manually remove all contents of this directory AND the directory itself (or rename it)")
		 .arg(destDir));
		 
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

	generateMultipleThread.start();
}

void TimetableGenerateMultipleForm::timetableGenerated(int timetable, const QString& description)
{
	QString s=currentResultsTextEdit->text();
	s+=tr("Timetable no: %1 => %2").arg(timetable).arg(description);
	s+="\n";
	currentResultsTextEdit->setText(s);

	//needed to get the conflicts string
	QString tmp;
	genMulti.c.fitness(gt.rules, &tmp);
	
	TimetableExport::getStudentsTimetable(genMulti.c);
	TimetableExport::getTeachersTimetable(genMulti.c);
	TimetableExport::getRoomsTimetable(genMulti.c);

	TimetableExport::writeSimulationResults(timetable);

	//update the string representing the conflicts
	conflictsString = "";
	conflictsString+="Total soft conflicts: ";
	conflictsString+=QString::number(best_solution.conflictsTotal);
	conflictsString+="\n";
	conflictsString += tr("Soft conflicts listing (in decreasing order):\n");

	foreach(QString t, best_solution.conflictsDescriptionList)
		conflictsString+=t+"\n";


	semaphoreTimetableFinished.release();
}

void TimetableGenerateMultipleForm::stop()
{
	if(!simulation_running_multi){
		QMessageBox::critical(this, TimetableGenerateMultipleForm::tr("FET information"),
		 TimetableGenerateMultipleForm::tr("Simulation stopped but the simulation is not running."
		 " This should not happen. Maybe you aborted simulation previously. Please report possible bug to author"));

		return;
	}

	mutex.lock();
	genMulti.abortOptimization=true;
	mutex.unlock();

	simulation_running_multi=false;

	QString s=TimetableGenerateMultipleForm::tr("Simulation interrupted.");
	s+=" ";

	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	QString destDir=OUTPUT_DIR+FILE_SEP+s2;

	s+=TimetableGenerateMultipleForm::tr("The results for the generated timetables are saved in the directory %1 in html and xml mode"
	 " and the soft conflicts in txt mode").arg(destDir);
	 
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
		QMessageBox::critical(this, TimetableGenerateMultipleForm::tr("FET information"),
		 TimetableGenerateMultipleForm::tr("Simulation finished but the simulation is not running."
		 " This should not happen. Maybe you aborted simulation previously. Please report possible bug to author"));

		return;
	}

	assert(simulation_running_multi);

	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	QString destDir=OUTPUT_DIR+FILE_SEP+s2;

	QMessageBox::information(this, TimetableGenerateMultipleForm::tr("FET information"),
		TimetableGenerateMultipleForm::tr("Simulation terminated successfully. The results are saved in directory %1 in html"
		" and xml mode and the soft conflicts in txt mode.").arg(destDir));

	simulation_running_multi=false;

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
	int seconds=finish_time-start_time;
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
