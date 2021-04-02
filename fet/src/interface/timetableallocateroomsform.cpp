/***************************************************************************
                          timetableallocateroomsform.cpp  -  description
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

#include "timetableallocateroomsform.h"
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

QSemaphore semaphore;

#include <iostream>
#include <fstream>
using namespace std;

static QMutex mutex;
static SpaceSolvingThread spaceSolvingThread;

//Represents the current state - simulation running or stopped.
extern bool simulation_running;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

extern SpaceChromosome best_space_chromosome;

extern QString spaceConflictsString;

void SpaceSolvingThread::run()
{
	//To print current time (starting time)
	time_t ltime;
	tzset();
	/* Get UNIX-style time and display as number and string. */
	time( &ltime );

	for(int i=0; i<max_generations; i++){
		mutex.lock();
	
		if(evolution_method==1){
			gt.spacePopulation.evolve1(gt.rules);
		}
		else if(evolution_method==2){
			gt.spacePopulation.evolve2(gt.rules);
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

TimetableAllocateRoomsForm::TimetableAllocateRoomsForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	simulation_running=false;
	
	connect(&spaceSolvingThread, SIGNAL(generationComputed(int)),
	 this, SLOT(updateGeneration(int)));
}

TimetableAllocateRoomsForm::~TimetableAllocateRoomsForm()
{
}

void TimetableAllocateRoomsForm::start(){
	if(!gt.rules.initialized || gt.rules.activitiesList.isEmpty()){
		QMessageBox::critical(this, QObject::tr("FET information"),
			QObject::tr("You have entered simulation with uninitialized rules or 0 activities...aborting"));
		assert(0);
		exit(1);
		return;
	}
	//if(!gt.rules.internalStructureComputed){
	if(!gt.spacePopulation.initialized){
		QMessageBox::information(this, QObject::tr("FET information"), 
			QObject::tr("You didn't initialize or load the initial state"));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::information(this, QObject::tr("FET information"), 
			QObject::tr("You didn't allocate the hours prior to allocating the rooms"));
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
	
	spaceSolvingThread.start();
}

void TimetableAllocateRoomsForm::stop(){
	simulation_running=false;
	
	mutex.lock();

	SpaceChromosome &c=gt.spacePopulation.bestChromosome(gt.rules);

	getRoomsTimetable(c);
	
	//update the string representing the conflicts
	spaceConflictsString = "";
	spaceConflictsString += "COMPULSORY CONSTRAINTS CONFLICTS (more important):\n";
	c.hardFitness(gt.rules, gt.spacePopulation.days, gt.spacePopulation.hours, &spaceConflictsString);
	spaceConflictsString += "\n--------------------------\n\n";
	spaceConflictsString += "NON-COMPULSORY CONSTRAINTS CONFLICTS (less important):\n";
	c.softFitness(gt.rules, gt.spacePopulation.days, gt.spacePopulation.hours, &spaceConflictsString);

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

void TimetableAllocateRoomsForm::updateGeneration(int generation){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.spacePopulation.initialized);
	
	mutex.lock();

	SpaceChromosome& c=gt.spacePopulation.bestChromosome(gt.rules);

	//write to the Qt interface
	QString s;
	s = "Generation:";
	s+=QString::number(generation+1)+"\n";
	s+="Compulsory constraints conflicts:";
	s+=QString::number(c.hardFitness(gt.rules, gt.spacePopulation.days, gt.spacePopulation.hours))+"\n";
	s+="Non-compulsory constraints conflicts:";
	s+=QString::number(c.softFitness(gt.rules, gt.spacePopulation.days, gt.spacePopulation.hours));

	mutex.unlock();

	currentResultsTextEdit->setText(s);
	
	semaphore.release();
}

void TimetableAllocateRoomsForm::write()
{
	mutex.lock();

	SpaceChromosome &c=gt.spacePopulation.bestChromosome(gt.rules);
	
	getRoomsTimetable(c);
	
	//update the string representing the conflicts
	spaceConflictsString = "";
	spaceConflictsString += "COMPULSORY CONSTRAINTS CONFLICTS (more important):\n";
	c.hardFitness(gt.rules, gt.spacePopulation.days, gt.spacePopulation.hours, &spaceConflictsString);
	spaceConflictsString += "\n--------------------------\n\n";
	spaceConflictsString += "NON-COMPULSORY CONSTRAINTS CONFLICTS (less important):\n";
	c.softFitness(gt.rules, gt.spacePopulation.days, gt.spacePopulation.hours, &spaceConflictsString);
	
	writeSimulationResults(c);

	mutex.unlock();

	QMessageBox::information(this, QObject::tr("FET information"),
		QObject::tr("Simulation results should be successfully written. You may check now Timetable/View"));
}

void TimetableAllocateRoomsForm::savePosition()
{
	mutex.lock();

	QString s=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	gt.spacePopulation.write(gt.rules, OUTPUT_DIR+FILE_SEP+s+"_space_population_state.txt");
	
	mutex.unlock();
	
	QMessageBox::information(this, QObject::tr("FET information"),
	 QObject::tr("Simulation position saved to hard disk.\nPress button to continue."));																										
}

void TimetableAllocateRoomsForm::loadPosition()
{
	if(!gt.rules.initialized || gt.rules.activitiesList.isEmpty()){
		QMessageBox::critical(this, QObject::tr("FET information"),
			QObject::tr("You have entered simulation with uninitialized rules or 0 activities...aborting"));
		assert(0);
		exit(1);
		return;
	}

	bool prev_state=gt.spacePopulation.initialized;
	gt.spacePopulation.init(gt.rules, population_number, gt.timePopulation.bestChromosome(gt.rules));
	QString s=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	bool existing_file=gt.spacePopulation.read(gt.rules, OUTPUT_DIR+FILE_SEP+s+"_space_population_state.txt");
	if(existing_file==false) //for versions older or equal to 3.9.21
		existing_file=gt.spacePopulation.read(gt.rules, OUTPUT_DIR+FILE_SEP+"space_population_state.txt");
	if(existing_file==false){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("You did not save any internal state yet - aborting operation"));
		gt.spacePopulation.initialized=prev_state;
		return;
	}
	QMessageBox::information(this, QObject::tr("FET information"),
		QObject::tr("Simulation position restored from hard disk. You may now continue the simulation"));
	this->writeResultsPushButton->setEnabled(TRUE);
}

void TimetableAllocateRoomsForm::initializeUnallocated()
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

	assert(gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready);

	gt.spacePopulation.init(gt.rules, population_number, gt.timePopulation.bestChromosome(gt.rules));
	gt.spacePopulation.makeRoomsUnallocated(gt.rules);
	QMessageBox::information(this, QObject::tr("FET information"),
		QObject::tr("Initialized with unallocated data - now you can start simulation"));
	this->writeResultsPushButton->setEnabled(TRUE);
}

void TimetableAllocateRoomsForm::initializeRandomly()
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

	assert(gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready);

	gt.spacePopulation.init(gt.rules, population_number, gt.timePopulation.bestChromosome(gt.rules));
	gt.spacePopulation.makeRoomsRandom(gt.rules);
	QMessageBox::information(this, QObject::tr("FET information"),
		QObject::tr("Initialized with random data - now you can start simulation"));
	this->writeResultsPushButton->setEnabled(TRUE);
}

void TimetableAllocateRoomsForm::writeSimulationResults(SpaceChromosome &c){
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
	assert(gt.spacePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready);
	assert(rooms_schedule_ready);

	QString s;
	
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);

	//write the space conflicts - in txt mode
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+SPACE_CONFLICTS_FILENAME;
	QFile file(s);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	tos.setEncoding(QTextStream::UnicodeUTF8);
	tos<<spaceConflictsString<<endl;
	file.close();
	//now write the solution in xml and html
	//students - html
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+STUDENTS_TIMETABLE_WITH_ROOMS_DAYS_HORIZONTAL_FILENAME_HTML;
	writeStudentsTimetableWithRoomsDaysHorizontalHtml(s);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+STUDENTS_TIMETABLE_WITH_ROOMS_DAYS_VERTICAL_FILENAME_HTML;
	writeStudentsTimetableWithRoomsDaysVerticalHtml(s);
	//teachers - html
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+TEACHERS_TIMETABLE_WITH_ROOMS_1_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersTimetableWithRooms1DaysHorizontalHtml(s);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+TEACHERS_TIMETABLE_WITH_ROOMS_1_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersTimetableWithRooms1DaysVerticalHtml(s);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+TEACHERS_TIMETABLE_WITH_ROOMS_2_FILENAME_HTML;
	writeTeachersTimetableWithRooms2Html(s);
	//rooms - xml and html
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+ROOMS_TIMETABLE_FILENAME_XML;
	writeRoomsTimetableXml(s);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeRoomsTimetableDaysHorizontalHtml(s);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeRoomsTimetableDaysVerticalHtml(s);

	cout<<"Writing simulation results to disk completed successfully"<<endl;
}

void TimetableAllocateRoomsForm::getRoomsTimetable(SpaceChromosome &c){
	assert(gt.spacePopulation.initialized);
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);

	//assert(c.HFitness()==0); - for perfect solutions
	c.getRoomsTimetable(gt.rules, gt.spacePopulation.days, gt.spacePopulation.hours, rooms_timetable_week1, rooms_timetable_week2);
	best_space_chromosome.copy(gt.rules, c);
	rooms_schedule_ready=true;
}

/**
Function writing the rooms' timetable in xml format to a file
*/
void TimetableAllocateRoomsForm::writeRoomsTimetableXml(const QString& xmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.spacePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready);
	assert(rooms_schedule_ready);

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
	tos << "</" << protect(ROOMS_TIMETABLE_TAG) << ">\n";

	file.close();
}

/**
Function writing the students' timetable (with rooms) in html format to a file.
Days horizontal.
*/
void TimetableAllocateRoomsForm::writeStudentsTimetableWithRoomsDaysHorizontalHtml(const QString& htmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	SpaceChromosome& c=gt.spacePopulation.bestChromosome(gt.rules);

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
					
					int ri=c.rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=protect2(gt.rules.roomsList.at(ri)->name);
					tos<<roomName<<"<br/>";
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

					int ri=c.rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=protect2(gt.rules.roomsList.at(ri)->name);
					tos<<roomName<<"<br/>";
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
void TimetableAllocateRoomsForm::writeStudentsTimetableWithRoomsDaysVerticalHtml(const QString& htmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	SpaceChromosome& c=gt.spacePopulation.bestChromosome(gt.rules);

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
					
					int ri=c.rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=protect2(gt.rules.roomsList.at(ri)->name);
					tos<<roomName<<"<br/>";
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

					int ri=c.rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=protect2(gt.rules.roomsList.at(ri)->name);
					tos<<roomName<<"<br/>";
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
Function writing the teachers' timetable (with rooms) html format to a file (var. 1).
Days horizontal.
*/
void TimetableAllocateRoomsForm::writeTeachersTimetableWithRooms1DaysHorizontalHtml(const QString& htmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	SpaceChromosome& c=gt.spacePopulation.bestChromosome(gt.rules);

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
			
			tos<<"<td>"<<gt.rules.hoursOfTheDay[j]<<"</td>\n";
			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				tos<<"<td style=\"width:14em;\">";

				int ai=teachers_timetable_week1[i][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << protect2(*it) << "<br/>";
						
					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";
					
					int ri=c.rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=protect2(gt.rules.roomsList.at(ri)->name);
					tos<<roomName<<"<br/>";
				}
				else
					tos<<"&nbsp;";

				ai=teachers_timetable_week2[i][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tos<<"/<br/>\n";
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << protect2(*it) << "<br/>";
					
					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";

					int ri=c.rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=protect2(gt.rules.roomsList.at(ri)->name);
					tos<<roomName<<"<br/>";
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
void TimetableAllocateRoomsForm::writeTeachersTimetableWithRooms1DaysVerticalHtml(const QString& htmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	SpaceChromosome& c=gt.spacePopulation.bestChromosome(gt.rules);

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
			
			tos<<"<td>"<<gt.rules.daysOfTheWeek[k]<<"</td>\n";
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos<<"<td style=\"width:14em;\">";

				int ai=teachers_timetable_week1[i][k][j]; //activity index
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << protect2(*it) << "<br/>";
						
					tos<<protect2(act->subjectName)<<" "<<protect2(act->subjectTagName)<<"<br/>";
					
					int ri=c.rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=protect2(gt.rules.roomsList.at(ri)->name);
					tos<<roomName<<"<br/>";
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

					int ri=c.rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=protect2(gt.rules.roomsList.at(ri)->name);
					tos<<roomName<<"<br/>";
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
void TimetableAllocateRoomsForm::writeTeachersTimetableWithRooms2Html(const QString& htmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	SpaceChromosome& c=gt.spacePopulation.bestChromosome(gt.rules);

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
		tos << "<td>" << protect2(gt.rules.internalTeachersList[i]->name) << "</td>\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos<<"<td>";
				int ai=teachers_timetable_week1[i][k][j]; //activity index
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos<<protect2(*it)<<"<br/>";

					int ri=c.rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=protect2(gt.rules.roomsList.at(ri)->name);
					tos<<roomName<<"<br/>";
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

					int ri=c.rooms[ai]; //room index
					QString roomName;
					if(ri==UNALLOCATED_SPACE)
						roomName="UNALLOCATED_SPACE";
					else
						roomName=protect2(gt.rules.roomsList.at(ri)->name);
					tos<<roomName<<"<br/>";
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
void TimetableAllocateRoomsForm::writeRoomsTimetableDaysHorizontalHtml(const QString& htmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.spacePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready);
	assert(rooms_schedule_ready);

	//Writing the timetable in xml format
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
void TimetableAllocateRoomsForm::writeRoomsTimetableDaysVerticalHtml(const QString& htmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(gt.spacePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready);
	assert(rooms_schedule_ready);

	//Writing the timetable in xml format
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
