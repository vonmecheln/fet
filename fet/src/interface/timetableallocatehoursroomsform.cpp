/***************************************************************************
                          timetableallocatehoursroomsform.cpp  -  description
                             -------------------
    begin                : Feb 14, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#include "timetableallocatehoursroomsform.h"
#include "fetmainform.h"
#include "genetictimetable_defs.h"
#include "genetictimetable.h"
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

#include <QDesktopWidget>

#include <QSemaphore>

static QSemaphore semaphore;

#include <iostream>
#include <fstream>
using namespace std;

static QMutex mutex;
static TimeSpaceSolvingThread timeSpaceSolvingThread;

//Represents the current state - simulation running or stopped.
extern bool simulation_running;

extern bool students_schedule_ready2;
extern bool teachers_schedule_ready2;
extern bool rooms_schedule_ready2;

extern TimeSpaceChromosome best_time_space_chromosome;

extern QString timeSpaceConflictsString;

void TimeSpaceSolvingThread::run()
{
	time_t ltime;
	tzset();
	/* Get UNIX-style time and display as number and string. */
	time( &ltime );

	for(int i=0; i<max_generations; i++){
		mutex.lock();
	
		if(evolution_method==1){
			gt.timeSpacePopulation.evolve1(gt.rules);
		}
		else if(evolution_method==2){
			gt.timeSpacePopulation.evolve2(gt.rules);
		}
		else{
			assert(0);
		}
		
		mutex.unlock();
		
		emit(generationComputed(i));
		
		semaphore.acquire();

		time_t tmp;
		time( &tmp );

		if(tmp-ltime>=timelimit){
			break;
		}
		
		if(!simulation_running)
			break;
	}
}

TimetableAllocateHoursRoomsForm::TimetableAllocateHoursRoomsForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	simulation_running=false;
	
	connect(&timeSpaceSolvingThread, SIGNAL(generationComputed(int)),
	 this, SLOT(updateGeneration(int)));
}

TimetableAllocateHoursRoomsForm::~TimetableAllocateHoursRoomsForm()
{
}

void TimetableAllocateHoursRoomsForm::start(){
	if(!gt.rules.initialized || gt.rules.activitiesList.isEmpty()){
		QMessageBox::critical(this, QObject::tr("FET information"),
			QObject::tr("You have entered simulation with uninitialized rules or 0 activities...aborting"));
		assert(0);
		exit(1);
		return;
	}
	//if(!gt.rules.internalStructureComputed){
	if(!gt.timeSpacePopulation.initialized){
		QMessageBox::information(this, QObject::tr("FET information"), 
			QObject::tr("You didn't initialize or load the initial state"));
		return;
	}
	
	startPushButton->setDisabled(TRUE);
	stopPushButton->setEnabled(TRUE);
	savePositionPushButton->setEnabled(TRUE);
	loadPositionPushButton->setDisabled(TRUE);
	initializeUnallocatedPushButton->setDisabled(TRUE);
	initializeRandomlyPushButton->setDisabled(TRUE);
	closePushButton->setDisabled(TRUE);

	QMessageBox::information(this, QObject::tr("FET information"),
	 QObject::tr("Entering simulation..."));
	 
	simulation_running=true;
	
	timeSpaceSolvingThread.start();
}

void TimetableAllocateHoursRoomsForm::stop(){
	simulation_running=false;

	mutex.lock();

	TimeSpaceChromosome &c=gt.timeSpacePopulation.bestChromosome(gt.rules);

	getTeachersTimetable(c);
	getStudentsTimetable(c);
	getRoomsTimetable(c);
	
	//update the string representing the conflicts
	timeSpaceConflictsString = "";
	timeSpaceConflictsString += "COMPULSORY CONSTRAINTS CONFLICTS (more important):\n";
	c.hardFitness(gt.rules, &timeSpaceConflictsString);
	timeSpaceConflictsString += "\n--------------------------\n\n";
	timeSpaceConflictsString += "NON-COMPULSORY CONSTRAINTS CONFLICTS (less important):\n";
	c.softFitness(gt.rules, &timeSpaceConflictsString);

	writeSimulationResults(c);

	mutex.unlock();

	QMessageBox::information(this, QObject::tr("FET information"),
		QObject::tr("Simulation completed successfully"));

	startPushButton->setEnabled(TRUE);
	stopPushButton->setDisabled(TRUE);
	savePositionPushButton->setDisabled(TRUE);
	loadPositionPushButton->setEnabled(TRUE);
	initializeUnallocatedPushButton->setEnabled(TRUE);
	initializeRandomlyPushButton->setEnabled(TRUE);
	closePushButton->setEnabled(TRUE);
}


void TimetableAllocateHoursRoomsForm::updateGeneration(int generation){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timeSpacePopulation.initialized);
	
	mutex.lock();

	TimeSpaceChromosome& c=gt.timeSpacePopulation.bestChromosome(gt.rules);

	//write to the Qt interface
	QString s;
	s = "Generation:";
	s+=QString::number(generation+1)+"\n";
	s+="Compulsory constraints conflicts:";
	s+=QString::number(c.hardFitness(gt.rules))+"\n";
	s+="Non-compulsory constraints conflicts:";
	s+=QString::number(c.softFitness(gt.rules));
	
	mutex.unlock();

	currentResultsTextEdit->setText(s);
	
	semaphore.release();
}

void TimetableAllocateHoursRoomsForm::write(){
	mutex.lock();
	
	TimeSpaceChromosome &c=gt.timeSpacePopulation.bestChromosome(gt.rules);

	getTeachersTimetable(c);
	getStudentsTimetable(c);
	getRoomsTimetable(c);

	//update the string representing the conflicts
	timeSpaceConflictsString = "";
	timeSpaceConflictsString += "COMPULSORY CONSTRAINTS CONFLICTS (more important):\n";
	c.hardFitness(gt.rules, &timeSpaceConflictsString);
	timeSpaceConflictsString += "\n--------------------------\n\n";
	timeSpaceConflictsString += "NON-COMPULSORY CONSTRAINTS CONFLICTS (less important):\n";
	c.softFitness(gt.rules, &timeSpaceConflictsString);

	writeSimulationResults(c);
	
	mutex.unlock();

	QMessageBox::information(this, QObject::tr("FET information"),
	 QObject::tr("Simulation results should be successfully written. You may check now Timetable/View"));
}

void TimetableAllocateHoursRoomsForm::savePosition()
{
	mutex.lock();

	QString s=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	gt.timeSpacePopulation.write(gt.rules, OUTPUT_DIR+FILE_SEP+s+"_time_space_population_state.txt");
	
	mutex.unlock();
	
	QMessageBox::information(this, QObject::tr("FET information"),
	 QObject::tr("Simulation position saved to hard disk.\nPress button to continue."));
}

void TimetableAllocateHoursRoomsForm::loadPosition()
{
	if(!gt.rules.initialized || gt.rules.activitiesList.isEmpty()){
		QMessageBox::critical(this, QObject::tr("FET information"),
			QObject::tr("You have entered simulation with uninitialized rules or 0 activities...aborting"));
		assert(0);
		exit(1);
		return;
	}
	
	if(!gt.rules.computeInternalStructure())
		return;

	bool prev_state=gt.timeSpacePopulation.initialized;
	gt.timeSpacePopulation.init(gt.rules, population_number);
	QString s=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	bool existing_file=gt.timeSpacePopulation.read(gt.rules, OUTPUT_DIR+FILE_SEP+s+"_time_space_population_state.txt");
	if(existing_file==false){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("You did not save any internal state yet - aborting operation"));
		gt.timeSpacePopulation.initialized=prev_state;
		return;
	}
	QMessageBox::information(this, QObject::tr("FET information"),
		QObject::tr("Simulation position restored from hard disk. You may now continue the simulation"));
	this->writeResultsPushButton->setEnabled(TRUE);
}

void TimetableAllocateHoursRoomsForm::initializeUnallocated()
{
	if(!gt.rules.initialized){
		QMessageBox::critical(this, QObject::tr("FET information"),
			QObject::tr("You have entered simulation with uninitialized rules...aborting"));
		assert(0);
		exit(1);
		return;
	}
	if(gt.rules.activitiesList.isEmpty()){
		QMessageBox::critical(this, QObject::tr("FET information"),
			QObject::tr("You have entered simulation with 0 activities...aborting"));
		assert(0);
		exit(1);
		return;
	}
	if(gt.rules.roomsList.isEmpty()){
		QMessageBox::critical(this, QObject::tr("FET information"),
			QObject::tr("You have entered simulation with 0 rooms...aborting"));
		assert(0);
		exit(1);
		return;
	}

	if(!gt.rules.internalStructureComputed)
		if(!gt.rules.computeInternalStructure())
			return;

	gt.timeSpacePopulation.init(gt.rules, population_number);
	gt.timeSpacePopulation.makeTimesRoomsUnallocated(gt.rules);
	QMessageBox::information(this, QObject::tr("FET information"),
		QObject::tr("Initialized with unallocated data - now you can start simulation"));
	this->writeResultsPushButton->setEnabled(TRUE);
}

void TimetableAllocateHoursRoomsForm::initializeRandomly()
{
	if(!gt.rules.initialized){
		QMessageBox::critical(this, QObject::tr("FET information"),
			QObject::tr("You have entered simulation with uninitialized rules...aborting"));
		assert(0);
		exit(1);
		return;
	}
	if(gt.rules.activitiesList.isEmpty()){
		QMessageBox::critical(this, QObject::tr("FET information"),
			QObject::tr("You have entered simulation with 0 activities...aborting"));
		assert(0);
		exit(1);
		return;
	}
	if(gt.rules.roomsList.isEmpty()){
		QMessageBox::critical(this, QObject::tr("FET information"),
			QObject::tr("You have entered simulation with 0 rooms...aborting"));
		assert(0);
		exit(1);
		return;
	}

	if(!gt.rules.internalStructureComputed)
		if(!gt.rules.computeInternalStructure())
			return;

	gt.timeSpacePopulation.init(gt.rules, population_number);
	gt.timeSpacePopulation.makeTimesRoomsRandom(gt.rules);
	QMessageBox::information(this, QObject::tr("FET information"),
		QObject::tr("Initialized with random data - now you can start simulation"));
	this->writeResultsPushButton->setEnabled(TRUE);
}

void TimetableAllocateHoursRoomsForm::writeSimulationResults(TimeSpaceChromosome &c){
	if(&c!=NULL)
		;

	QDir dir;
	
	//make sure that the input directory exists - only for GNU/Linux
	//For Windows, I make a "fet.ini" in the current working directory
#ifndef WIN32
	if(!dir.exists(QDir::homeDirPath()+"/.fet"))
		dir.mkdir(QDir::homeDirPath()+"/.fet");
#endif
						
	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR))
		dir.mkdir(OUTPUT_DIR);

	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timeSpacePopulation.initialized);
	assert(students_schedule_ready2);
	assert(teachers_schedule_ready2);
	assert(rooms_schedule_ready2);

	QString s;
	
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);

	//write the space conflicts - in txt mode
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+TIME_SPACE_CONFLICTS_FILENAME;
	QFile file(s);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setEncoding(QTextStream::UnicodeUTF8);
	tos<<timeSpaceConflictsString<<endl;
	file.close();
	//now write the solution in xml and html
	//students - xml
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+STUDENTS_TIMETABLE_FILENAME_XML;
	writeStudentsTimetableXml(s);
	//teachers - xml
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+TEACHERS_TIMETABLE_FILENAME_XML;
	writeTeachersTimetableXml(s);
	//students - html
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+STUDENTS_TIMETABLE_WITH_ROOMS_1_DAYS_HORIZONTAL_FILENAME_HTML;
	writeStudentsTimetableWithRooms1DaysHorizontalHtml(s);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+STUDENTS_TIMETABLE_WITH_ROOMS_1_DAYS_VERTICAL_FILENAME_HTML;
	writeStudentsTimetableWithRooms1DaysVerticalHtml(s);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+STUDENTS_TIMETABLE_WITH_ROOMS_2_FILENAME_HTML;
	writeStudentsTimetableWithRooms2Html(s);
	//teachers - html
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+TEACHERS_TIMETABLE_WITH_ROOMS_1_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersTimetableWithRooms1DaysHorizontalHtml(s);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+TEACHERS_TIMETABLE_WITH_ROOMS_1_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersTimetableWithRooms1DaysVerticalHtml(s);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+TEACHERS_TIMETABLE_WITH_ROOMS_2_FILENAME_HTML;
	writeTeachersTimetableWithRooms2Html(s);
	//rooms - xml
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+ROOMS_TIMETABLE_FILENAME_XML;
	writeRoomsTimetableXml(s);
	//rooms - html
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+ROOMS_TIMETABLE_1_DAYS_HORIZONTAL_FILENAME_HTML;
	writeRoomsTimetable1DaysHorizontalHtml(s);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+ROOMS_TIMETABLE_1_DAYS_VERTICAL_FILENAME_HTML;
	writeRoomsTimetable1DaysVerticalHtml(s);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+ROOMS_TIMETABLE_2_FILENAME_HTML;
	writeRoomsTimetable2Html(s);

	cout<<"Writing simulation results to disk completed successfully"<<endl;
}

void TimetableAllocateHoursRoomsForm::getTeachersTimetable(TimeSpaceChromosome &c){
	assert(gt.timeSpacePopulation.initialized);
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);

	//assert(c.HFitness()==0); - for perfect solutions
	c.getTeachersTimetable(gt.rules, teachers_timetable_week1, teachers_timetable_week2);
	teachers_schedule_ready2=true;
}

void TimetableAllocateHoursRoomsForm::getStudentsTimetable(TimeSpaceChromosome &c){
	assert(gt.timeSpacePopulation.initialized);
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);

	//assert(c.HFitness()==0); - for perfect solutions
	c.getSubgroupsTimetable(gt.rules, students_timetable_week1, students_timetable_week2);
	students_schedule_ready2=true;
}

void TimetableAllocateHoursRoomsForm::getRoomsTimetable(TimeSpaceChromosome &c){
	assert(gt.timeSpacePopulation.initialized);
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);

	//assert(c.HFitness()==0); - for perfect solutions
	c.getRoomsTimetable(gt.rules, rooms_timetable_week1, rooms_timetable_week2);
	best_time_space_chromosome.copy(gt.rules, c);
	rooms_schedule_ready2=true;
}

/**
Function writing the students' timetable in xml format to a file
*/
void TimetableAllocateHoursRoomsForm::writeStudentsTimetableXml(const QString& xmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timeSpacePopulation.initialized);
	assert(students_schedule_ready2 && teachers_schedule_ready2);

	//Now we print the results to an XML file
	QFile file(xmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setEncoding(QTextStream::UnicodeUTF8);
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
				tos<<"     <Week1>";
				int ai=students_timetable_week1[subgroup][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					//Activity* act=gt.rules.activitiesList.at(ai);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos<<" <Teacher name=\""<<protect(*it)<<"\"></Teacher>";
					tos<<"<Subject name=\""<<protect(act->subjectName)<<"\"></Subject>";
					tos<<"<Subject_Tag name=\""<<protect(act->subjectTagName)<<"\"></Subject_Tag>";
				}
				tos<<"</Week1>\n";
				tos<<"     <Week2>";
				ai=students_timetable_week2[subgroup][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					//Activity* act=gt.rules.activitiesList.at(ai);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos<<" <Teacher name=\""<<protect(*it)<<"\"></Teacher>";
					tos<<"<Subject name=\""<<protect(act->subjectName)<<"\"></Subject>";
					tos<<"<Subject_Tag name=\""<<protect(act->subjectTagName)<<"\"></Subject_Tag>";
				}

				tos<<"</Week2>\n";
				tos << "    </Hour>\n";
			}
			tos<<"   </Day>\n";
		}
		tos<<"  </Subgroup>\n";
	}

	tos<<"\n";
	tos << "</" << STUDENTS_TIMETABLE_TAG << ">\n";

	file.close();
}

/**
Function writing the teachers' timetable xml format to a file
*/
void TimetableAllocateHoursRoomsForm::writeTeachersTimetableXml(const QString& xmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timeSpacePopulation.initialized);
	assert(students_schedule_ready2 && teachers_schedule_ready2);

	//Writing the timetable in xml format
	QFile file(xmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setEncoding(QTextStream::UnicodeUTF8);
	tos << "<" << protect(TEACHERS_TIMETABLE_TAG) << ">\n";

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		tos<<"\n";
		tos << "  <Teacher name=\"" << protect(gt.rules.internalTeachersList[i]->name) << "\">\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			tos << "   <Day name=\"" << protect(gt.rules.daysOfTheWeek[k]) << "\">\n";
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos << "    <Hour name=\"" << protect(gt.rules.hoursOfTheDay[j]) << "\">\n";

				tos<<"     <Week1>";
				int ai=teachers_timetable_week1[i][k][j]; //activity index
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tos<<"<Subject name=\""<<protect(act->subjectName)<<"\"></Subject>";
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << "<Students name=\"" << protect(*it) << "\"></Students>";
				}
				tos<<"</Week1>\n";

				tos<<"     <Week2>";
				ai=teachers_timetable_week2[i][k][j]; //activity index
				//act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tos<<"<Subject name=\""<<protect(act->subjectName)<<"\"></Subject>";
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << "<Students name=\"" << protect(*it) << "\"></Students>";
				}
				tos<<"</Week2>\n";

				tos << "    </Hour>\n";
			}
			tos << "   </Day>\n";
		}
		tos<<"  </Teacher>\n";
	}

	tos<<"\n";
	tos << "</" << TEACHERS_TIMETABLE_TAG << ">\n";

	file.close();
}

/**
Function writing the rooms' timetable in xml format to a file
*/
void TimetableAllocateHoursRoomsForm::writeRoomsTimetableXml(const QString& xmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timeSpacePopulation.initialized);
	assert(students_schedule_ready2 && teachers_schedule_ready2);
	assert(rooms_schedule_ready2);

	//Writing the timetable in xml format
	QFile file(xmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setEncoding(QTextStream::UnicodeUTF8);
	tos << "<" << protect(ROOMS_TIMETABLE_TAG) << ">\n";

	for(int i=0; i<gt.rules.nInternalRooms; i++){
		tos<<"\n";
		tos << "  <Room name=\"" << protect(gt.rules.internalRoomsList[i]->name) << "\">\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			tos << "   <Day name=\"" << protect(gt.rules.daysOfTheWeek[k]) << "\">\n";
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos << "    <Hour name=\"" << protect(gt.rules.hoursOfTheDay[j]) << "\">\n";

				tos<<"     <Week1>";
				int ai=rooms_timetable_week1[i][k][j]; //activity index
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << "<Students name=\"" << protect(*it) << "\"></Students>";
				}
				tos<<"</Week1>\n";

				tos<<"     <Week2>";
				ai=rooms_timetable_week2[i][k][j]; //activity index
				//act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << "<Students name=\"" << protect(*it) << "\"></Students>";
				}
				tos<<"</Week2>\n";

				tos << "    </Hour>\n";
			}
			tos << "   </Day>\n";
		}
		tos<<"  </Room>\n";
	}

	tos<<"\n";
	tos << "</" << ROOMS_TIMETABLE_TAG << ">\n";

	file.close();
}

/**
Function writing the students' timetable (with rooms) in html format to a file.
Days horizontal.
*/
void TimetableAllocateHoursRoomsForm::writeStudentsTimetableWithRooms1DaysHorizontalHtml(const QString& htmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timeSpacePopulation.initialized);
	assert(students_schedule_ready2 && teachers_schedule_ready2 && rooms_schedule_ready2);

	SpaceChromosome* c=(SpaceChromosome*)(&gt.timeSpacePopulation.bestChromosome(gt.rules));

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setEncoding(QTextStream::UnicodeUTF8);
	
	tos<<"<html>\n";
	tos<<"<meta content=\"text/html; charset=utf-8\">\n";
	tos<<"<title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"<body>\n";
	
	tos<<"<center><h3>"<<protect2(gt.rules.institutionName)<<"</h3></center><br>\n";

	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
		QString subgroup_name = gt.rules.internalSubgroupsList[subgroup]->name;
		tos<<"<p align=\"center\">"<<protect2(subgroup_name)<<"</p>\n";		
		tos<<"<table border=\"1\" cellpadding=\"6\">"<<endl;

		tos<<"<tr>\n<td></td>\n";
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			tos<<"<td>"<<protect2(gt.rules.daysOfTheWeek[j])<<"</td>\n";		
		tos<<"</tr>\n";

		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			tos<<"<tr>\n";
			tos<<"<td>"<<protect2(gt.rules.hoursOfTheDay[j])<<"</td>\n";
			
			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				tos<<"<td style=\"width:14em;\">\n";
				
				int ai=students_timetable_week1[subgroup][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					//Activity* act=gt.rules.activitiesList.at(ai);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos<<protect2(*it)<<"<br/>";
					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";
					
					int ri=c->rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=gt.rules.roomsList.at(ri)->name;
					tos<<protect2(roomName)<<"<br/>";
				}
				else
					tos<<"&nbsp;";
				ai=students_timetable_week2[subgroup][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					tos<<"/<br/>";
					//Activity* act=gt.rules.activitiesList.at(ai);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos<<protect2(*it)<<"<br/>";
					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";

					int ri=c->rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=gt.rules.roomsList.at(ri)->name;
					tos<<protect2(roomName)<<"<br/>";
				}
				tos<<"</td>\n";
			}			
			tos<<"</tr>\n";
		}		
		tos<<"</table>\n";
	}

	tos<<"<p/>"<<endl;
	time_t ltime;
	tzset();
	time(&ltime);
	tos<<QObject::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(ctime(&ltime));
					
	tos<<"</body>\n</html>\n";

	file.close();
}

/**
Function writing the students' timetable (with rooms) in html format to a file.
Days vertical.
*/
void TimetableAllocateHoursRoomsForm::writeStudentsTimetableWithRooms1DaysVerticalHtml(const QString& htmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timeSpacePopulation.initialized);
	assert(students_schedule_ready2 && teachers_schedule_ready2 && rooms_schedule_ready2);

	SpaceChromosome* c=(SpaceChromosome*)(&gt.timeSpacePopulation.bestChromosome(gt.rules));

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setEncoding(QTextStream::UnicodeUTF8);
	
	tos<<"<html>\n";
	tos<<"<meta content=\"text/html; charset=utf-8\">\n";
	tos<<"<title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"<body>\n";
	
	tos<<"<center><h3>"<<protect2(gt.rules.institutionName)<<"</h3></center><br>\n";

	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
		QString subgroup_name = gt.rules.internalSubgroupsList[subgroup]->name;
		tos<<"<p align=\"center\">"<<protect2(subgroup_name)<<"</p>\n";		
		tos<<"<table border=\"1\" cellpadding=\"6\">"<<endl;

		tos<<"<tr>\n<td></td>\n";
		for(int j=0; j<gt.rules.nHoursPerDay; j++)
			tos<<"<td>"<<protect2(gt.rules.hoursOfTheDay[j])<<"</td>\n";		
		tos<<"</tr>\n";

		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			tos<<"<tr>\n";
			tos<<"<td>"<<protect2(gt.rules.daysOfTheWeek[k])<<"</td>\n";
			
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos<<"<td style=\"width:14em;\">\n";
				
				int ai=students_timetable_week1[subgroup][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					//Activity* act=gt.rules.activitiesList.at(ai);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos<<protect2(*it)<<"<br/>";
					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";
					
					int ri=c->rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=gt.rules.roomsList.at(ri)->name;
					tos<<protect2(roomName)<<"<br/>";
				}
				else
					tos<<"&nbsp;";
				ai=students_timetable_week2[subgroup][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					tos<<"/<br/>";
					//Activity* act=gt.rules.activitiesList.at(ai);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos<<protect2(*it)<<"<br/>";
					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";

					int ri=c->rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=gt.rules.roomsList.at(ri)->name;
					tos<<protect2(roomName)<<"<br/>";
				}
				tos<<"</td>\n";
			}			
			tos<<"</tr>\n";
		}		
		tos<<"</table>\n";
	}

	tos<<"<p/>"<<endl;
	time_t ltime;
	tzset();
	time(&ltime);
	tos<<QObject::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(ctime(&ltime));
					
	tos<<"</body>\n</html>\n";

	file.close();
}

/**
Function writing the students' timetable (with rooms) html format to a file (var. 2)
*/
void TimetableAllocateHoursRoomsForm::writeStudentsTimetableWithRooms2Html(const QString& htmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timeSpacePopulation.initialized);
	assert(students_schedule_ready2 && teachers_schedule_ready2 && rooms_schedule_ready2);

	SpaceChromosome* c=(SpaceChromosome*)(&gt.timeSpacePopulation.bestChromosome(gt.rules));

	//Writing the timetable in xml format
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setEncoding(QTextStream::UnicodeUTF8);
	tos << "<html>\n";
	tos<<"<meta content=\"text/html; charset=utf-8\">\n";
	tos<<"<body>\n<table border=\"1\">\n";
	
	tos<<"<tr><td></td>\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		tos << "<td align=\"center\" colspan=\"" << gt.rules.nHoursPerDay <<"\">" << protect2(gt.rules.daysOfTheWeek[k]) << "</td>\n";
	tos<<"</tr>\n";

	tos<<"<tr>\n";
	tos<<"<td></td>\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		for(int j=0; j<gt.rules.nHoursPerDay; j++)
			tos << "<td>" << protect2(gt.rules.hoursOfTheDay[j]) << "</td>\n";

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		tos<<"<tr>\n";
		tos << "<td>" << gt.rules.internalSubgroupsList[i]->name << "</td>\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos<<"<td>";
				int ai=students_timetable_week1[i][k][j]; //activity index
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos<<protect2(*it)<<"<br/>";

					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";
					
					int ri=c->rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=gt.rules.roomsList.at(ri)->name;
					tos<<protect2(roomName)<<"<br/>";
				}
				else
					tos<<"&nbsp;";

				ai=students_timetable_week2[i][k][j]; //activity index
				//act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tos<<"/<br/>";
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos << protect2(*it) <<"<br/>";

					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";
					
					int ri=c->rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=gt.rules.roomsList.at(ri)->name;
					tos<<protect2(roomName)<<"<br/>";
				}
				tos<<"</td>\n";
			}
		}
		tos<<"</tr>\n";
	}

	tos<<"</table>\n";
	
	tos<<"<p/>"<<endl;
	time_t ltime;
	tzset();
	time(&ltime);
	tos<<QObject::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(ctime(&ltime));
	
	tos<<"</body>\n</html>\n";

	file.close();
}

/**
Function writing the teachers' timetable (with rooms) html format to a file (var. 1).
Days horizontal.
*/
void TimetableAllocateHoursRoomsForm::writeTeachersTimetableWithRooms1DaysHorizontalHtml(const QString& htmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timeSpacePopulation.initialized);
	assert(students_schedule_ready2 && teachers_schedule_ready2 && rooms_schedule_ready2);

	SpaceChromosome* c=(SpaceChromosome*)(&gt.timeSpacePopulation.bestChromosome(gt.rules));

	//Writing the timetable in xml format
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setEncoding(QTextStream::UnicodeUTF8);
	tos<<"<html>\n";
	tos<<"<meta content=\"text/html; charset=utf-8\">\n";
	tos<<"<title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"<body>\n";
	tos<<"<center><h3>"<<protect2(gt.rules.institutionName)<<"</h3></center><br>\n";

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		tos<<"<p align=\"center\">"<<protect2(gt.rules.internalTeachersList[i]->name)<<"</p>\n";
		tos<<"<table width=\"100%\" border=\"1\" cellpadding=\"6\">\n";

		tos<<"<tr>\n<td></td>\n";
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			tos << "<td>" << protect2(gt.rules.daysOfTheWeek[j]) << "</td>\n";
		tos<<"</tr>\n";
		
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			tos<<"<tr>\n";
			
			tos<<"<td>"<<protect2(gt.rules.hoursOfTheDay[j])<<"</td>\n";
			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				tos<<"<td style=\"width:14em;\">";

				int ai=teachers_timetable_week1[i][k][j]; //activity index
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << protect2(*it) << "<br/>";
						
					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";
					
					int ri=c->rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=gt.rules.roomsList.at(ri)->name;
					tos<<protect2(roomName)<<"<br/>";
				}
				else
					tos<<"&nbsp;";

				ai=teachers_timetable_week2[i][k][j]; //activity index
				//act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tos<<"/<br/>\n";
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << protect2(*it) << "<br/>";
					
					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";
					
					int ri=c->rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=gt.rules.roomsList.at(ri)->name;
					tos<<protect2(roomName)<<"<br/>";
				}
				tos<<"</td>\n";
			}
			tos << "</tr>\n";
		}
		tos<<"</table>\n";
	}
	
	tos<<"<p/>"<<endl;
	time_t ltime;
	tzset();
	time(&ltime);
	tos<<QObject::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(ctime(&ltime));

	tos<<"</body>\n</html>\n";

	file.close();
}

/**
Function writing the teachers' timetable (with rooms) html format to a file (var. 1).
Days vertical.
*/
void TimetableAllocateHoursRoomsForm::writeTeachersTimetableWithRooms1DaysVerticalHtml(const QString& htmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timeSpacePopulation.initialized);
	assert(students_schedule_ready2 && teachers_schedule_ready2 && rooms_schedule_ready2);

	SpaceChromosome* c=(SpaceChromosome*)(&gt.timeSpacePopulation.bestChromosome(gt.rules));

	//Writing the timetable in xml format
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setEncoding(QTextStream::UnicodeUTF8);
	tos<<"<html>\n";
	tos<<"<meta content=\"text/html; charset=utf-8\">\n";
	tos<<"<title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"<body>\n";
	tos<<"<center><h3>"<<protect2(gt.rules.institutionName)<<"</h3></center><br>\n";

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		tos<<"<p align=\"center\">"<<protect2(gt.rules.internalTeachersList[i]->name)<<"</p>\n";
		tos<<"<table width=\"100%\" border=\"1\" cellpadding=\"6\">\n";

		tos<<"<tr>\n<td></td>\n";
		for(int j=0; j<gt.rules.nHoursPerDay; j++)
			tos << "<td>" << protect2(gt.rules.hoursOfTheDay[j]) << "</td>\n";
		tos<<"</tr>\n";
		
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			tos<<"<tr>\n";
			
			tos<<"<td>"<<protect2(gt.rules.daysOfTheWeek[k])<<"</td>\n";
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos<<"<td style=\"width:14em;\">";

				int ai=teachers_timetable_week1[i][k][j]; //activity index
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << protect2(*it) << "<br/>";
						
					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";
					
					int ri=c->rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=gt.rules.roomsList.at(ri)->name;
					tos<<protect2(roomName)<<"<br/>";
				}
				else
					tos<<"&nbsp;";

				ai=teachers_timetable_week2[i][k][j]; //activity index
				//act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tos<<"/<br/>\n";
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << protect2(*it) << "<br/>";
					
					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";
					
					int ri=c->rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=gt.rules.roomsList.at(ri)->name;
					tos<<protect2(roomName)<<"<br/>";
				}
				tos<<"</td>\n";
			}
			tos << "</tr>\n";
		}
		tos<<"</table>\n";
	}
	
	tos<<"<p/>"<<endl;
	time_t ltime;
	tzset();
	time(&ltime);
	tos<<QObject::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(ctime(&ltime));

	tos<<"</body>\n</html>\n";

	file.close();
}

/**
Function writing the teachers' timetable (with rooms) html format to a file (var. 2)
*/
void TimetableAllocateHoursRoomsForm::writeTeachersTimetableWithRooms2Html(const QString& htmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timeSpacePopulation.initialized);
	assert(students_schedule_ready2 && teachers_schedule_ready2 && rooms_schedule_ready2);

	SpaceChromosome* c=(SpaceChromosome*)(&gt.timeSpacePopulation.bestChromosome(gt.rules));

	//Writing the timetable in xml format
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setEncoding(QTextStream::UnicodeUTF8);
	tos << "<html>\n";
	tos<<"<meta content=\"text/html; charset=utf-8\">\n";
	tos<<"<body>\n<table border=\"1\">\n";
	
	tos<<"<tr><td></td>\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		tos << "<td align=\"center\" colspan=\"" << gt.rules.nHoursPerDay <<"\">" << protect2(gt.rules.daysOfTheWeek[k]) << "</td>\n";
	tos<<"</tr>\n";

	tos<<"<tr>\n";
	tos<<"<td></td>\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		for(int j=0; j<gt.rules.nHoursPerDay; j++)
			tos << "<td>" << protect2(gt.rules.hoursOfTheDay[j]) << "</td>\n";

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		tos<<"<tr>\n";
		tos << "<td>" << gt.rules.internalTeachersList[i]->name << "</td>\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos<<"<td>";
				int ai=teachers_timetable_week1[i][k][j]; //activity index
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos<<protect2(*it)<<"<br/>";

					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";
					
					int ri=c->rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=gt.rules.roomsList.at(ri)->name;
					tos<<protect2(roomName)<<"<br/>";
				}
				else
					tos<<"&nbsp;";

				ai=teachers_timetable_week2[i][k][j]; //activity index
				//act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tos<<"/<br/>";
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << protect2(*it) <<"<br/>";

					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";
					
					int ri=c->rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=gt.rules.roomsList.at(ri)->name;
					tos<<protect2(roomName)<<"<br/>";
				}
				tos<<"</td>\n";
			}
		}
		tos<<"</tr>\n";
	}

	tos<<"</table>\n";
	
	tos<<"<p/>"<<endl;
	time_t ltime;
	tzset();
	time(&ltime);
	tos<<QObject::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(ctime(&ltime));
	
	tos<<"</body>\n</html>\n";

	file.close();
}

/**
Function writing the rooms' timetable in html format to a file.
Days horizontal.
*/
void TimetableAllocateHoursRoomsForm::writeRoomsTimetable1DaysHorizontalHtml(const QString& htmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timeSpacePopulation.initialized);
	assert(students_schedule_ready2 && teachers_schedule_ready2);
	assert(rooms_schedule_ready2);

	//Writing the timetable in html format
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setEncoding(QTextStream::UnicodeUTF8);
	tos<<"<html>\n";
	tos<<"<meta content=\"text/html; charset=utf-8\">\n";
	tos<<"<title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"<body>";
	tos<<"<center><h3>"<<protect2(gt.rules.institutionName)<<"</h3></center><br/>\n";

	for(int i=0; i<gt.rules.nInternalRooms; i++){
		tos<<"<p align=\"center\">"<<protect2(gt.rules.internalRoomsList[i]->name)<<"</p>\n";
		tos<<"<table border=\"1\" cellpadding=\"6\">\n";

		tos<<"<tr>\n<td>&nbsp;</td>\n";
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			tos << "<td>" << protect2(gt.rules.daysOfTheWeek[j]) << "</td>\n";
		tos<<"</tr>\n";

		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			tos<<"<tr>\n";
			tos << "<td>" << protect2(gt.rules.hoursOfTheDay[j]) << "</td>\n";
			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				tos << "<td>";

				int ai=rooms_timetable_week1[i][k][j]; //activity index
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << protect2(*it) << "<br/>";
					tos<<protect2(act->subjectName)<<"<br/>";
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos << protect2(*it) << "<br/>";
				}
				else
					tos<<"&nbsp;";

				ai=rooms_timetable_week2[i][k][j]; //activity index
				//act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tos<<"/<br/>";
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << protect2(*it) << "<br/>";
					tos<<protect2(act->subjectName)<<"<br/>";
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos << protect2(*it) << "<br/>";
				}

				tos << "</td>\n";
			}
			tos << "</tr>\n";
		}
		tos<<"</table>\n";
	}

	tos<<"<p/>"<<endl;
	time_t ltime;
	tzset();
	time(&ltime);
	tos<<QObject::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(ctime(&ltime));

	tos<<"</body>\n</html>\n";

	file.close();
}

/**
Function writing the rooms' timetable in html format to a file.
Days vertical.
*/
void TimetableAllocateHoursRoomsForm::writeRoomsTimetable1DaysVerticalHtml(const QString& htmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timeSpacePopulation.initialized);
	assert(students_schedule_ready2 && teachers_schedule_ready2);
	assert(rooms_schedule_ready2);

	//Writing the timetable in html format
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setEncoding(QTextStream::UnicodeUTF8);
	tos<<"<html>\n";
	tos<<"<meta content=\"text/html; charset=utf-8\">\n";
	tos<<"<title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"<body>";
	tos<<"<center><h3>"<<protect2(gt.rules.institutionName)<<"</h3></center><br/>\n";

	for(int i=0; i<gt.rules.nInternalRooms; i++){
		tos<<"<p align=\"center\">"<<protect2(gt.rules.internalRoomsList[i]->name)<<"</p>\n";
		tos<<"<table border=\"1\" cellpadding=\"6\">\n";

		tos<<"<tr>\n<td>&nbsp;</td>\n";
		for(int j=0; j<gt.rules.nHoursPerDay; j++)
			tos << "<td>" << protect2(gt.rules.hoursOfTheDay[j]) << "</td>\n";
		tos<<"</tr>\n";

		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			tos<<"<tr>\n";
			tos << "<td>" << protect2(gt.rules.daysOfTheWeek[k]) << "</td>\n";
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos << "<td>";

				int ai=rooms_timetable_week1[i][k][j]; //activity index
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << protect2(*it) << "<br/>";
					tos<<protect2(act->subjectName)<<"<br/>";
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos << protect2(*it) << "<br/>";
				}
				else
					tos<<"&nbsp;";

				ai=rooms_timetable_week2[i][k][j]; //activity index
				//act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tos<<"/<br/>";
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << protect2(*it) << "<br/>";
					tos<<protect2(act->subjectName)<<"<br/>";
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos << protect2(*it) << "<br/>";
				}

				tos << "</td>\n";
			}
			tos << "</tr>\n";
		}
		tos<<"</table>\n";
	}

	tos<<"<p/>"<<endl;
	time_t ltime;
	tzset();
	time(&ltime);
	tos<<QObject::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(ctime(&ltime));

	tos<<"</body>\n</html>\n";

	file.close();
}

/**
Function writing the teachers' timetable (with rooms) html format to a file (var. 2)
*/
void TimetableAllocateHoursRoomsForm::writeRoomsTimetable2Html(const QString& htmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timeSpacePopulation.initialized);
	assert(students_schedule_ready2 && teachers_schedule_ready2 && rooms_schedule_ready2);

	//Writing the timetable in html format
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setEncoding(QTextStream::UnicodeUTF8);
	tos << "<html>\n";
	tos<<"<meta content=\"text/html; charset=utf-8\">\n";
	tos<<"<body>\n<table border=\"1\">\n";
	
	tos<<"<tr><td></td>\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		tos << "<td align=\"center\" colspan=\"" << gt.rules.nHoursPerDay <<"\">" << protect2(gt.rules.daysOfTheWeek[k]) << "</td>\n";
	tos<<"</tr>\n";

	tos<<"<tr>\n";
	tos<<"<td></td>\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		for(int j=0; j<gt.rules.nHoursPerDay; j++)
			tos << "<td>" << protect2(gt.rules.hoursOfTheDay[j]) << "</td>\n";

	for(int i=0; i<gt.rules.nInternalRooms; i++){
		tos<<"<tr>\n";
		tos << "<td>" << gt.rules.internalRoomsList[i]->name << "</td>\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos<<"<td>";
				int ai=rooms_timetable_week1[i][k][j]; //activity index
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos<<protect2(*it)<<"<br/>";

					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";
					
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos<<protect2(*it)<<"<br/>";
				}
				else
					tos<<"&nbsp;";

				ai=rooms_timetable_week2[i][k][j]; //activity index
				//act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos<<protect2(*it)<<"<br/>";

					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";
					
					for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
						tos<<protect2(*it)<<"<br/>";
				}
				tos<<"</td>\n";
			}
		}
		tos<<"</tr>\n";
	}

	tos<<"</table>\n";
	
	tos<<"<p/>"<<endl;
	time_t ltime;
	tzset();
	time(&ltime);
	tos<<QObject::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(ctime(&ltime));
	
	tos<<"</body>\n</html>\n";

	file.close();
}
