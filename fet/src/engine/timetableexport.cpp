/***************************************************************************
                          timetableexport.cpp  -  description
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

//**********************************************************************************************************************/
//August 2007
//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
//added features: - xhtml 1.0 strict valide
//                - colspan and rowspan using
//                - times vertical
//                - table of content with hyperlinks
//                - css and JavaScript support
//                - print rooms timetable
//                - TIMETABLE_HTML_LEVEL
//                - print groups and years timetable
//                - print subjects timetable
//                - print teachers free periods timetable
//                - print all activities timetable
//                - index html file


#include "timetable_defs.h"
#include "timetable.h"
#include "timetableexport.h"
#include "solution.h"

#include <qstring.h>
#include <qtextstream.h>
#include <qfile.h>

#include <QList>

#include <QDesktopWidget>

#include <iostream>
#include <fstream>
using namespace std;

#include <QMessageBox>

#include <QLocale>
#include <QTime>
#include <QDate>

//Represents the current status of the simulation - running or stopped.
extern bool simulation_running;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

extern Solution best_solution;
extern bool LANGUAGE_STYLE_RIGHT_TO_LEFT;
extern QString LANGUAGE_FOR_HTML;

extern Timetable gt;
extern qint16 teachers_timetable_weekly[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern qint16 students_timetable_weekly[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern qint16 rooms_timetable_weekly[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

extern QList<qint16> teachers_free_periods_timetable_weekly[TEACHERS_FREE_PERIODS_N_CATEGORIES][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

extern bool breakDayHour[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern bool teacherNotAvailableDayHour[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern double notAllowedRoomTimePercentages[MAX_ROOMS][MAX_HOURS_PER_WEEK];
extern bool subgroupNotAvailableDayHour[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

QList<qint16> activitiesForCurrentSubject[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

QList<qint16> activitiesAtTime[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

extern Rules rules2;


const QString STRING_EMPTY_SLOT="---";

const QString STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES="???";

const QString STRING_NOT_AVAILABLE_SLOT="-x-";

//#define STRING_NOT_AVAILABLE_SLOT		( (PRINT_NOT_AVAILABLE_TIME_SLOTS) ? QString("-x-") : QString("---") )

//QString STRING_NOT_AVAILABLE_SLOT="-x-";				//TODO: do this extern
//QString STRING_EMPTY_SLOT="---";					//TODO: do this extern
//QString STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES="???";	//TODO: do this extern


TimetableExport::TimetableExport()
{
}

TimetableExport::~TimetableExport()
{
}


void TimetableExport::getStudentsTimetable(Solution &c){
	//assert(gt.timePopulation.initialized);
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);

	//assert(c.HFitness()==0); - for perfect solutions
	c.getSubgroupsTimetable(gt.rules, students_timetable_weekly);
	best_solution.copy(gt.rules, c);
	students_schedule_ready=true;
}

void TimetableExport::getTeachersTimetable(Solution &c){
	//assert(gt.timePopulation.initialized);
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);

	//assert(c.HFitness()==0); - for perfect solutions
	//c.getTeachersTimetable(gt.rules, teachers_timetable_weekly);
	c.getTeachersTimetable(gt.rules, teachers_timetable_weekly, teachers_free_periods_timetable_weekly);
	best_solution.copy(gt.rules, c);
	teachers_schedule_ready=true;
}

void TimetableExport::getRoomsTimetable(Solution &c){
	//assert(gt.timePopulation.initialized);
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);

	//assert(c.HFitness()==0); - for perfect solutions
	c.getRoomsTimetable(gt.rules, rooms_timetable_weekly);
	best_solution.copy(gt.rules, c);
	rooms_schedule_ready=true;
}


void TimetableExport::writeSimulationResults(){
	QDir dir;

	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR))
		dir.mkdir(OUTPUT_DIR);

	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);
	assert(TIMETABLE_HTML_LEVEL>=0);
	assert(TIMETABLE_HTML_LEVEL<=5);

	QString s;
	QString bar;
	if(INPUT_FILENAME_XML=="")
		bar="";
	else
		bar="_";
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);

	//now write the solution in xml files
	//subgroups
	s=OUTPUT_DIR+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_FILENAME_XML;
	writeSubgroupsTimetableXml(s);
	//teachers
	s=OUTPUT_DIR+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_FILENAME_XML;
	writeTeachersTimetableXml(s);

	//now get the time. TODO: maybe write it in xml too? so do it a few lines earlier!
	/*time_t ltime;
	tzset();
	time(&ltime);
	QString sTime=ctime(&ltime);
	//remove the endl, because it looks awful in html and css file(by Volker Dirr)
	int sTs=sTime.size();sTs--;
	if(sTime[sTs]=='\n')
		sTime.remove(sTs,1);*/
	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);

	//now get the number of placed activities. TODO: maybe write it in xml too? so do it a few lines earlier!
	int na=0;
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		if(best_solution.times[i]!=UNALLOCATED_TIME)
			na++;
	
	//write the conflicts in txt mode
	s=OUTPUT_DIR+FILE_SEP+s2+bar+CONFLICTS_FILENAME;
	writeConflictsTxt(s, sTime, na);
	
	//now write the solution in html files
	if(TIMETABLE_HTML_LEVEL>=1){
		s=OUTPUT_DIR+FILE_SEP+s2+"_"+STYLESHEET_CSS;
		writeStylesheetCss(s, sTime, na);
	}
	//indexHtml
	s=OUTPUT_DIR+FILE_SEP+s2+bar+INDEX_HTML;
	writeIndexHtml(s, sTime, na);
	//subgroups
	s=OUTPUT_DIR+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysVerticalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeSubgroupsTimetableTimeHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeSubgroupsTimetableTimeVerticalHtml(s, sTime, na);
	//groups
	s=OUTPUT_DIR+FILE_SEP+s2+bar+GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeGroupsTimetableDaysHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeGroupsTimetableDaysVerticalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeGroupsTimetableTimeHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeGroupsTimetableTimeVerticalHtml(s, sTime, na);
	//years
	s=OUTPUT_DIR+FILE_SEP+s2+bar+YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeYearsTimetableDaysHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeYearsTimetableDaysVerticalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeYearsTimetableTimeHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeYearsTimetableTimeVerticalHtml(s, sTime, na);
	//teachers
	s=OUTPUT_DIR+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersTimetableDaysHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersTimetableDaysVerticalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeTeachersTimetableTimeHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeTeachersTimetableTimeVerticalHtml(s, sTime, na);
	//rooms
	s=OUTPUT_DIR+FILE_SEP+s2+bar+ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeRoomsTimetableDaysHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeRoomsTimetableDaysVerticalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeRoomsTimetableTimeHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeRoomsTimetableTimeVerticalHtml(s, sTime, na);
	//subjects
	s=OUTPUT_DIR+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubjectsTimetableDaysHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubjectsTimetableDaysVerticalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeSubjectsTimetableTimeHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeSubjectsTimetableTimeVerticalHtml(s, sTime, na);
	//all activities
	s=OUTPUT_DIR+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeAllActivitiesTimetableDaysHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeAllActivitiesTimetableDaysVerticalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeAllActivitiesTimetableTimeHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeAllActivitiesTimetableTimeVerticalHtml(s, sTime, na);
	//teachers free periods
	s=OUTPUT_DIR+FILE_SEP+s2+bar+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersFreePeriodsTimetableDaysHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+bar+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersFreePeriodsTimetableDaysVerticalHtml(s, sTime, na);

	cout<<"Writing simulation results to disk completed successfully"<<endl;
}


void TimetableExport::writeTimetableDataFile(const QString& filename)
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_schedule_ready){
		QMessageBox::critical(NULL, tr("FET - Critical"), tr("Timetable not generated - cannot save it - this should not happen (please report bug)"));
		return;	
	}

	Solution* tc=&best_solution;

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		//Activity* act=&gt.rules.internalActivitiesList[ai];
		int time=tc->times[ai];
		if(time==UNALLOCATED_TIME){
			QMessageBox::critical(NULL, tr("FET - Critical"), tr("Incomplete timetable - this should not happen - please report bug"));
			return;	
		}
		
		int ri=tc->rooms[ai];
		if(ri==UNALLOCATED_SPACE){
			QMessageBox::critical(NULL, tr("FET - Critical"), tr("Incomplete timetable - this should not happen - please report bug"));
			return;	
		}
	}
	
	rules2.initialized=true;
	
	rules2.institutionName=gt.rules.institutionName;
	rules2.comments=gt.rules.comments;
	
	rules2.nHoursPerDay=gt.rules.nHoursPerDay;
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		rules2.hoursOfTheDay[i]=gt.rules.hoursOfTheDay[i];

	rules2.nDaysPerWeek=gt.rules.nDaysPerWeek;
	for(int i=0; i<gt.rules.nDaysPerWeek; i++)
		rules2.daysOfTheWeek[i]=gt.rules.daysOfTheWeek[i];
		
	rules2.yearsList=gt.rules.yearsList;
	
	rules2.teachersList=gt.rules.teachersList;
	
	rules2.subjectsList=gt.rules.subjectsList;
	
	rules2.activityTagsList=gt.rules.activityTagsList;

	rules2.activitiesList=gt.rules.activitiesList;

	rules2.buildingsList=gt.rules.buildingsList;

	rules2.roomsList=gt.rules.roomsList;

	rules2.timeConstraintsList=gt.rules.timeConstraintsList;
	
	rules2.spaceConstraintsList=gt.rules.spaceConstraintsList;


	//add locking constraints
	TimeConstraintsList lockTimeConstraintsList;
	SpaceConstraintsList lockSpaceConstraintsList;



	bool report=false;
	
	int addedTime=0, duplicatesTime=0;
	int addedSpace=0, duplicatesSpace=0;

	//lock selected activities
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		Activity* act=&gt.rules.internalActivitiesList[ai];
		int time=tc->times[ai];
		if(time>=0 && time<gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay){
			int hour=time/gt.rules.nDaysPerWeek;
			int day=time%gt.rules.nDaysPerWeek;

			ConstraintActivityPreferredStartingTime* ctr=new ConstraintActivityPreferredStartingTime(100.0, act->id, day, hour);
			bool t=rules2.addTimeConstraint(ctr);
						
			if(t){
				addedTime++;
				lockTimeConstraintsList.append(ctr);
			}
			else
				duplicatesTime++;

			QString s;
						
			if(t)
				s=tr("Added the following constraint to saved file:")+"\n"+ctr->getDetailedDescription(gt.rules);
			else{
				s=tr("Constraint\n%1 NOT added to saved file - duplicate").arg(ctr->getDetailedDescription(gt.rules));
				delete ctr;
			}
						
			if(report){
				int k;
				if(t)
					k=QMessageBox::information(NULL, tr("FET information"), s,
				 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
				else
					k=QMessageBox::warning(NULL, tr("FET warning"), s,
				 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
		 		if(k==0)
					report=false;
			}
		}
					
		int ri=tc->rooms[ai];
		if(ri!=UNALLOCATED_SPACE && ri!=UNSPECIFIED_ROOM && ri>=0 && ri<gt.rules.nInternalRooms){
			ConstraintActivityPreferredRoom* ctr=new ConstraintActivityPreferredRoom(100, act->id, (gt.rules.internalRoomsList[ri])->name);
			bool t=rules2.addSpaceConstraint(ctr);

			QString s;
						
			if(t){
				addedSpace++;
				lockSpaceConstraintsList.append(ctr);
			}
			else
				duplicatesSpace++;

			if(t)
				s=tr("Added the following constraint to saved file:")+"\n"+ctr->getDetailedDescription(gt.rules);
			else{
				s=tr("Constraint\n%1 NOT added to saved file - duplicate").arg(ctr->getDetailedDescription(gt.rules));
				delete ctr;
			}
						
			if(report){
				int k;
				if(t)
					k=QMessageBox::information(NULL, tr("FET information"), s,
				 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
				else
					k=QMessageBox::warning(NULL, tr("FET warning"), s,
					 tr("Skip information"), tr("See next"), QString(), 1, 0 );
																			 				 	
				if(k==0)
					report=false;
			}
		}
	}

	//QMessageBox::information(NULL, tr("FET information"), tr("Added %1 locking time constraints and %2 locking space constraints to saved file,"
	// " ignored %3 activities which were already fixed in time and %4 activities which were already fixed in space").arg(addedTime).arg(addedSpace).arg(duplicatesTime).arg(duplicatesSpace));
		
	bool result=rules2.write(filename);
	
	while(!lockTimeConstraintsList.isEmpty())
		delete lockTimeConstraintsList.takeFirst();
	while(!lockSpaceConstraintsList.isEmpty())
		delete lockSpaceConstraintsList.takeFirst();

	//if(result)	
	//	QMessageBox::information(NULL, tr("FET information"),
	//		tr("File saved successfully. You can see it on the hard disk. Current data file remained untouched (of locking constraints),"
	//		" so you can save it also, or generate different timetables."));

	rules2.nHoursPerDay=0;
	rules2.nDaysPerWeek=0;

	rules2.yearsList.clear();
	
	rules2.teachersList.clear();
	
	rules2.subjectsList.clear();
	
	rules2.activityTagsList.clear();

	rules2.activitiesList.clear();

	rules2.buildingsList.clear();

	rules2.roomsList.clear();

	rules2.timeConstraintsList.clear();
	
	rules2.spaceConstraintsList.clear();
	
	if(!result){
		QMessageBox::critical(NULL, tr("FET critical"), tr("Could not save the data + timetable file on the hard disk - maybe hard disk is full"));
	}
}


void TimetableExport::writeSimulationResults(int n){
	QDir dir;

	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR))
		dir.mkdir(OUTPUT_DIR);

	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);
	assert(TIMETABLE_HTML_LEVEL>=0);
	assert(TIMETABLE_HTML_LEVEL<=5);

	QString s;
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	
	QString destDir=OUTPUT_DIR+FILE_SEP+s2;
	
	if(!dir.exists(destDir))
		dir.mkdir(destDir);
		
	QString finalDestDir=destDir+FILE_SEP+QString::number(n);

	if(!dir.exists(finalDestDir))
		dir.mkdir(finalDestDir);
		
	finalDestDir+=FILE_SEP;

	QString s3=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	finalDestDir+=s3+"_";
	
	//write data+timetable in .fet format
	writeTimetableDataFile(finalDestDir+MULTIPLE_TIMETABLE_DATA_RESULTS_FILE);

	//now write the solution in xml files
	//subgroups
	s=finalDestDir+SUBGROUPS_TIMETABLE_FILENAME_XML;
	writeSubgroupsTimetableXml(s);
	//teachers
	s=finalDestDir+TEACHERS_TIMETABLE_FILENAME_XML;
	writeTeachersTimetableXml(s);

	//now get the time. TODO: maybe write it in xml too? so do it a few lines earlier!
	/*time_t ltime;
	tzset();
	time(&ltime);
	QString sTime=ctime(&ltime);
	//remove the endl, because it looks awful in html and css file (by Volker Dirr)
	int sTs=sTime.size();sTs--;
	if(sTime[sTs]=='\n')
		sTime.remove(sTs,1);*/
	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);

	//now get the number of placed activities. TODO: maybe write it in xml too? so do it a few lines earlier!
	int na=0;
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		if(best_solution.times[i]!=UNALLOCATED_TIME)
			na++;
	
	//write the conflicts in txt mode
	s=finalDestDir+CONFLICTS_FILENAME;
	writeConflictsTxt(s, sTime, na);
	
	//now write the solution in html files
	if(TIMETABLE_HTML_LEVEL>=1){
		s=finalDestDir+STYLESHEET_CSS;
		writeStylesheetCss(s, sTime, na);
	}
	//indexHtml
	s=finalDestDir+INDEX_HTML;
	writeIndexHtml(s, sTime, na);
	//subgroups
	s=finalDestDir+SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysHorizontalHtml(s, sTime, na);
	s=finalDestDir+SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysVerticalHtml(s, sTime, na);
	s=finalDestDir+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeSubgroupsTimetableTimeHorizontalHtml(s, sTime, na);
	s=finalDestDir+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeSubgroupsTimetableTimeVerticalHtml(s, sTime, na);
	//groups
	s=finalDestDir+GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeGroupsTimetableDaysHorizontalHtml(s, sTime, na);
	s=finalDestDir+GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeGroupsTimetableDaysVerticalHtml(s, sTime, na);
	s=finalDestDir+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeGroupsTimetableTimeHorizontalHtml(s, sTime, na);
	s=finalDestDir+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeGroupsTimetableTimeVerticalHtml(s, sTime, na);
	//years
	s=finalDestDir+YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeYearsTimetableDaysHorizontalHtml(s, sTime, na);
	s=finalDestDir+YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeYearsTimetableDaysVerticalHtml(s, sTime, na);
	s=finalDestDir+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeYearsTimetableTimeHorizontalHtml(s, sTime, na);
	s=finalDestDir+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeYearsTimetableTimeVerticalHtml(s, sTime, na);
	//teachers
	s=finalDestDir+TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersTimetableDaysHorizontalHtml(s, sTime, na);
	s=finalDestDir+TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersTimetableDaysVerticalHtml(s, sTime, na);
	s=finalDestDir+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeTeachersTimetableTimeHorizontalHtml(s, sTime, na);
	s=finalDestDir+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeTeachersTimetableTimeVerticalHtml(s, sTime, na);
	//rooms
	s=finalDestDir+ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeRoomsTimetableDaysHorizontalHtml(s, sTime, na);
	s=finalDestDir+ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeRoomsTimetableDaysVerticalHtml(s, sTime, na);
	s=finalDestDir+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeRoomsTimetableTimeHorizontalHtml(s, sTime, na);
	s=finalDestDir+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeRoomsTimetableTimeVerticalHtml(s, sTime, na);
	//subjects
	s=finalDestDir+SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubjectsTimetableDaysHorizontalHtml(s, sTime, na);
	s=finalDestDir+SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubjectsTimetableDaysVerticalHtml(s, sTime, na);
	s=finalDestDir+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeSubjectsTimetableTimeHorizontalHtml(s, sTime, na);
	s=finalDestDir+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeSubjectsTimetableTimeVerticalHtml(s, sTime, na);
	//all activities
	s=finalDestDir+ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeAllActivitiesTimetableDaysHorizontalHtml(s, sTime, na);
	s=finalDestDir+ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeAllActivitiesTimetableDaysVerticalHtml(s, sTime, na);
	s=finalDestDir+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeAllActivitiesTimetableTimeHorizontalHtml(s, sTime, na);
	s=finalDestDir+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeAllActivitiesTimetableTimeVerticalHtml(s, sTime, na);
	//teachers free periods
	s=finalDestDir+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersFreePeriodsTimetableDaysHorizontalHtml(s, sTime, na);
	s=finalDestDir+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersFreePeriodsTimetableDaysVerticalHtml(s, sTime, na);
					
	cout<<"Writing multiple simulation results to disk completed successfully"<<endl;
}

void TimetableExport::writeSimulationResultsCommandLine()
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);
	assert(TIMETABLE_HTML_LEVEL>=0);
	assert(TIMETABLE_HTML_LEVEL<=5);

	TimetableExport::writeSubgroupsTimetableXml(SUBGROUPS_TIMETABLE_FILENAME_XML);
	TimetableExport::writeTeachersTimetableXml(TEACHERS_TIMETABLE_FILENAME_XML);
			
	//get the time
	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
							
	//now get the number of placed activities. TODO: maybe write it in xml too? so do it a few lines earlier!
	int na=0;
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		if(best_solution.times[i]!=UNALLOCATED_TIME)
			na++;
														//write the conflicts in txt mode
	QString s=CONFLICTS_FILENAME;
	TimetableExport::writeConflictsTxt(s, sTime, na);
	
	//now write the solution in html files
	if(TIMETABLE_HTML_LEVEL>=1){
		s="_"+STYLESHEET_CSS;
		TimetableExport::writeStylesheetCss(s, sTime, na);
	}
	//indexHtml
	s=INDEX_HTML;
	writeIndexHtml(s, sTime, na);
	//subgroups
	s=SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	TimetableExport::writeSubgroupsTimetableDaysHorizontalHtml(s, sTime, na);
	s=SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	TimetableExport::writeSubgroupsTimetableDaysVerticalHtml(s, sTime, na);
	s=SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	TimetableExport::writeSubgroupsTimetableTimeHorizontalHtml(s, sTime, na);
	s=SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	TimetableExport::writeSubgroupsTimetableTimeVerticalHtml(s, sTime, na);
	//groups
	s=GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	TimetableExport::writeGroupsTimetableDaysHorizontalHtml(s, sTime, na);
	s=GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	TimetableExport::writeGroupsTimetableDaysVerticalHtml(s, sTime, na);
	s=GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	TimetableExport::writeGroupsTimetableTimeHorizontalHtml(s, sTime, na);
	s=GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	TimetableExport::writeGroupsTimetableTimeVerticalHtml(s, sTime, na);
	//years
	s=YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	TimetableExport::writeYearsTimetableDaysHorizontalHtml(s, sTime, na);
	s=YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	TimetableExport::writeYearsTimetableDaysVerticalHtml(s, sTime, na);
	s=YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	TimetableExport::writeYearsTimetableTimeHorizontalHtml(s, sTime, na);
	s=YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	TimetableExport::writeYearsTimetableTimeVerticalHtml(s, sTime, na);
	//teachers
	s=TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	TimetableExport::writeTeachersTimetableDaysHorizontalHtml(s, sTime, na);
	s=TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	TimetableExport::writeTeachersTimetableDaysVerticalHtml(s, sTime, na);
	s=TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	TimetableExport::writeTeachersTimetableTimeHorizontalHtml(s, sTime, na);
	s=TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	TimetableExport::writeTeachersTimetableTimeVerticalHtml(s, sTime, na);
	//rooms
	s=ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	TimetableExport::writeRoomsTimetableDaysHorizontalHtml(s, sTime, na);
	s=ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	TimetableExport::writeRoomsTimetableDaysVerticalHtml(s, sTime, na);
	s=ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	TimetableExport::writeRoomsTimetableTimeHorizontalHtml(s, sTime, na);
	s=ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	TimetableExport::writeRoomsTimetableTimeVerticalHtml(s, sTime, na);
	//subjects
	s=SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	TimetableExport::writeSubjectsTimetableDaysHorizontalHtml(s, sTime, na);
	s=SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	TimetableExport::writeSubjectsTimetableDaysVerticalHtml(s, sTime, na);
	s=SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	TimetableExport::writeSubjectsTimetableTimeHorizontalHtml(s, sTime, na);
	s=SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	TimetableExport::writeSubjectsTimetableTimeVerticalHtml(s, sTime, na);
	//all activities
	s=ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	TimetableExport::writeAllActivitiesTimetableDaysHorizontalHtml(s, sTime, na);
	s=ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	TimetableExport::writeAllActivitiesTimetableDaysVerticalHtml(s, sTime, na);
	s=ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	TimetableExport::writeAllActivitiesTimetableTimeHorizontalHtml(s, sTime, na);
	s=ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	TimetableExport::writeAllActivitiesTimetableTimeVerticalHtml(s, sTime, na);
	//teachers free periods
	s=TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	TimetableExport::writeTeachersFreePeriodsTimetableDaysHorizontalHtml(s, sTime, na);
	s=TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	TimetableExport::writeTeachersFreePeriodsTimetableDaysVerticalHtml(s, sTime, na);
}


/**
Function writing the conflicts to txt file
modified by Volker Dirr (timetabling.de) from old code by Liviu Lalescu
*/
void TimetableExport::writeConflictsTxt(const QString& filename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(filename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	
	if(placedActivities==gt.rules.nInternalActivities){
		tos<<TimetableExport::tr("Soft conflicts of %1").arg(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1))<<"\n";
		tos<<TimetableExport::tr("Generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"\n\n";

		tos<<TimetableExport::tr("Total soft conflicts: ")<<best_solution.conflictsTotal<<endl<<endl;
		tos<<TimetableExport::tr("Soft conflicts list (in decreasing order):")<<endl<<endl;
		foreach(QString t, best_solution.conflictsDescriptionList)
			tos<<t<<endl;
		tos<<endl<<TimetableExport::tr("End of file.")<<"\n\n";
	}
	else{
		tos<<TimetableExport::tr("Conflicts of %1").arg(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1))<<"\n";
		tos<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"\n";
		tos<<TimetableExport::tr("Generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"\n\n";

		tos<<TimetableExport::tr("Total conflicts: ")<<best_solution.conflictsTotal<<endl<<endl;
		tos<<TimetableExport::tr("Conflicts list (in decreasing order):")<<endl<<endl;
		foreach(QString t, best_solution.conflictsDescriptionList)
			tos<<t<<endl;
		tos<<endl<<TimetableExport::tr("End of file.")<<"\n\n";
	}
	
	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(filename).arg(file.error()));
	}
	file.close();
}

/**
Function writing the subgroups' timetable in xml format to a file
*/
void TimetableExport::writeSubgroupsTimetableXml(const QString& xmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an XML file
	QFile file(xmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(xmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
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
					tos<<"<Activity_Tag name=\""<<protect(act->activityTagName)<<"\"></Activity_Tag>";

					int r=best_solution.rooms[ai];
					if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
						tos<<"<Room name=\""<<protect(gt.rules.internalRoomsList[r]->name)<<"\"></Room>";
					}
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
					tos<<"<Activity_Tag name=\""<<protect(act->activityTagName)<<"\"></Activity_Tag>";
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

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(xmlfilename).arg(file.error()));
	}
	file.close();
}

/**
Function writing the teachers' timetable xml format to a file
*/
void TimetableExport::writeTeachersTimetableXml(const QString& xmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Writing the timetable in xml format
	QFile file(xmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(xmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
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
					tos<<"<Activity_Tag name=\""<<protect(act->activityTagName)<<"\"></Activity_Tag>";
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						tos << "<Students name=\"" << protect(*it) << "\"></Students>";

					int r=best_solution.rooms[ai];
					if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
						tos<<"<Room name=\""<<protect(gt.rules.internalRoomsList[r]->name)<<"\"></Room>";
					}
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

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(xmlfilename).arg(file.error()));
	}
	file.close();
}

/**
Function writing the index html file by Volker Dirr.
*/
void TimetableExport::writeIndexHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n"; 

	QString bar;
	QString s2="";
	if(INPUT_FILENAME_XML=="")
		bar="";
	else{
		bar="_";
		s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);
	}
	tos<<"    <p>\n";

	tos<<"      <a href=\""<<s2+bar+CONFLICTS_FILENAME<<"\" target=\"_new\">"<<tr("View the soft conflicts list.")<<"</a><br />\n";
	tos<<"      <a href=\""<<s2+bar+TEACHERS_TIMETABLE_FILENAME_XML<<"\" target=\"_new\">"<<tr("View the teachers xml.")<<"</a><br />\n";
	tos<<"      <a href=\""<<s2+bar+SUBGROUPS_TIMETABLE_FILENAME_XML<<"\" target=\"_new\">"<<tr("View the subgroups xml.")<<"</a>\n";
	tos<<"    </p>\n\n";

	tos<<"    <table border=\"1\">\n";

	tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

	tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\"4\">"+tr("Timetables")+"</th></tr>\n";
	tos<<"        <tr>\n          <!-- span -->\n";
	tos<<"          <th>"+tr("Days Horizontal")+"</th><th>"+tr("Days Vertical")+"</th><th>"+tr("Time Horizontal")+"</th><th>"+tr("Time Vertical")+"</th>";
	tos<<"        </tr>\n";
	tos<<"      </thead>\n";
	tos<<"      <tbody>\n";

	tos<<"        <tr>\n";
	tos<<"          <th>"+tr("Subgroups")+"</th>\n";
	tos<<"          <td><a href=\""<<s2+bar+SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"        </tr>\n";
	tos<<"        <tr>\n";
	tos<<"          <th>"+tr("Groups")+"</th>\n";
	tos<<"          <td><a href=\""<<s2+bar+GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"        </tr>\n";
	tos<<"        <tr>\n";
	tos<<"          <th>"+tr("Years")+"</th>\n";
	tos<<"          <td><a href=\""<<s2+bar+YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"        </tr>\n";
	tos<<"        <tr>\n";
	tos<<"          <th>"+tr("Teachers")+"</th>\n";
	tos<<"          <td><a href=\""<<s2+bar+TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"        </tr>\n";
	tos<<"        <tr>\n";
	tos<<"          <th>"+tr("Teachers Free Periods")+"</th>\n";
	tos<<"          <td><a href=\""<<s2+bar+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
	tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
	tos<<"        </tr>\n";
	tos<<"        <tr>\n";
	tos<<"          <th>"+tr("Rooms")+"</th>\n";
	tos<<"          <td><a href=\""<<s2+bar+ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"        </tr>\n";
	tos<<"        <tr>\n";
	tos<<"          <th>"+tr("Subjects")+"</th>\n";
	tos<<"          <td><a href=\""<<s2+bar+SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"        </tr>\n";
	tos<<"        <tr>\n";
	tos<<"          <th>"+tr("Activities")+"</th>\n";
	tos<<"          <td><a href=\""<<s2+bar+ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
	tos<<"          <td><a href=\""<<s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\" target=\"_new\">"+tr("view")+"</a></td>\n";
//	tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
//	tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
	tos<<"        </tr>\n";
	//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tr class=\"foot\"><td></td><td colspan=\"4\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
	//workaround end.
	tos<<"      </tbody>\n";
	tos<<"    </table>\n";

	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

/**
Function writing the stylesheet in css format to a file by Volker Dirr.
*/
void TimetableExport::writeStylesheetCss(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an CSS file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"/* "<<TimetableExport::tr("CSS Stylesheet of %1").arg(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1))<<"\n";
	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"   "<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"\n";
	tos<<"   "<<TimetableExport::tr("Stylesheet generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<" */\n\n";

	tos<<"/* "<<TimetableExport::tr("To do a page-break only after every second timetable, cut line %1 and paste it into line %2.").arg(8).arg(14)<<" */\n";
	tos<<"/* "<<TimetableExport::tr("To hide an element just write the following phrase into the element")<<": display:none; */\n\n";
	tos<<"table {\n  page-break-before: always;\n  text-align: center;\n}\n\n";
	tos<<"table.modulo2 {\n\n}\n\n";
	tos<<"table.detailed {\n  margin-left:auto; margin-right:auto;\n  text-align: center;\n  border: 0px;\n  border-spacing: 0;\n  border-collapse: collapse;\n}\n\n";
	tos<<"caption {\n\n}\n\n";
	tos<<"thead {\n\n}\n\n";
	//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"/* OpenOffice import the tfoot incorrect. So I used tr.foot instead of tfoot.\n";
	tos<<"   compare http://www.openoffice.org/issues/show_bug.cgi?id=82600\n";
	tos<<"tfoot {\n\n}*/\n\n";
	tos<<"tr.foot {\n\n}\n\n";
	//workaround end
	tos<<"tbody {\n\n}\n\n";
	tos<<"th {\n\n}\n\n";
	tos<<"td {\n\n}\n\n";
	tos<<"td.detailed {\n  border: 1px dashed silver;\n  border-bottom: 0;\n  border-top: 0;\n}\n\n";
	if(TIMETABLE_HTML_LEVEL>=2){
		tos<<"th.xAxis {\n/*width: 8em; */\n}\n\n";
		tos<<"th.yAxis {\n  height: 8ex;\n}\n\n";
	}
	if(TIMETABLE_HTML_LEVEL>=4){ // must be written before LEVEL 3, because LEVEL 3 should have higher priority
		for(int i=0; i<gt.rules.subjectsList.size(); i++){
			tos << "span.subject_"<<protect2id(gt.rules.subjectsList[i]->name)<<" {\n\n}\n\n";
		}
		for(int i=0; i<gt.rules.activityTagsList.size(); i++){
			tos << "span.activitytag_"<<protect2id(gt.rules.activityTagsList[i]->name)<<" {\n\n}\n\n";
		}
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			tos << "span.student_"<<protect2id(sty->name)<<" {\n\n}\n\n";
			for(int j=0; j<sty->groupsList.size(); j++){
				StudentsGroup* stg=sty->groupsList[j];
				tos << "span.student_"<<protect2id(stg->name)<<" {\n\n}\n\n";
			}
		}
		for(int i=0; i<gt.rules.nInternalTeachers; i++){
			tos << "span.teacher_"<<protect2id(gt.rules.internalTeachersList[i]->name)<<" {\n\n}\n\n";
		}
		for(int room=0; room<gt.rules.nInternalRooms; room++){
			tos << "span.room_"<<protect2id(gt.rules.internalRoomsList[room]->name)<<" {\n\n}\n\n";
		}
	}
	if(TIMETABLE_HTML_LEVEL>=3){
		tos<<"span.subject {\n\n}\n\n";
		tos<<"span.activitytag {\n\n}\n\n";
		tos<<"span.empty {\n  color: gray;\n}\n\n";
		tos<<"td.empty {\n  border-color:silver;\n  border-right-style:none;\n  border-bottom-style:none;\n  border-left-style:dotted;\n  border-top-style:dotted;\n}\n\n";
		tos<<"span.notAvailable {\n  color: gray;\n}\n\n";
		tos<<"td.notAvailable {\n  border-color:silver;\n  border-right-style:none;\n  border-bottom-style:none;\n  border-left-style:dotted;\n  border-top-style:dotted;\n}\n\n";
		tos<<"td.student, div.student {\n\n}\n\n";
		tos<<"td.teacher, div.teacher {\n\n}\n\n";
		tos<<"td.room, div.room {\n\n}\n\n";
		tos<<"tr.line0 {\n  font-size: smaller;\n}\n\n";
		tos<<"tr.line1, div.line1 {\n\n}\n\n";
		tos<<"tr.line2, div.line2 {\n  font-size: smaller;\n  color: gray;\n}\n\n";
		tos<<"tr.line3, div.line3 {\n  font-size: smaller;\n  color: silver;\n}\n\n";
	}
	
	tos<<endl<<"/* "<<TimetableExport::tr("Style the teachers free periods")<<" */\n\n";
	if(TIMETABLE_HTML_LEVEL>=2){
		tos<<"div.DESCRIPTION {\n  text-align: left;\n  font-size: smaller;\n}\n\n";
	}
	if(TIMETABLE_HTML_LEVEL>=3){
		tos<<"div.TEACHER_HAS_SINGLE_GAP {\n  color: black;\n}\n\n";
		tos<<"div.TEACHER_HAS_BORDER_GAP {\n  color: gray;\n}\n\n";
		tos<<"div.TEACHER_HAS_BIG_GAP {\n  color: silver;\n}\n\n";
		tos<<"div.TEACHER_MUST_COME_EARLIER {\n  color: purple;\n}\n\n";
		tos<<"div.TEACHER_MUST_COME_MUCH_EARLIER {\n  font-size: smaller;\n  color: fuchsia;\n}\n\n";
		tos<<"div.TEACHER_MUST_STAY_LONGER {\n  color: teal;\n}\n\n";
		tos<<"div.TEACHER_MUST_STAY_MUCH_LONGER {\n  font-size: smaller;\n  color: aqua;\n}\n\n";
		tos<<"div.TEACHER_HAS_A_FREE_DAY {\n  font-size: smaller;\n  color: red;\n}\n\n";
		tos<<"div.TEACHER_IS_NOT_AVAILABLE {\n  font-size: smaller;\n  color: olive;\n}\n\n";
	}
	tos<<endl<<"/* "<<TimetableExport::tr("End of file.")<<" */\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}


//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the subgroups' timetable in html format to a file.
Days horizontal
*/
void TimetableExport::writeSubgroupsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n"; 

	tos<<"    <p><strong>"<<TimetableExport::tr("Table of content")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		tos<<"      <li>\n        "<<TimetableExport::tr("Year")<<" "<<protect2(sty->name)<<"\n        <ul>\n";
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			tos<<"          <li>\n            "<<TimetableExport::tr("Group")<<" "<<protect2(stg->name)<<": \n";
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
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"xAxis\">";
			else
				tos<<"          <th>";
			tos<<protect2(gt.rules.daysOfTheWeek[j])<<"</th>\n";
		}
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";
		/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		*/
		tos<<"      <tbody>\n";
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			tos<<"        <tr>\n";
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"yAxis\">";
			else
				tos<<"          <th>";
			tos<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				int ai=students_timetable_weekly[subgroup][k][j]; //activity index
				int currentTime=k+gt.rules.nDaysPerWeek*j;
				if(ai!=UNALLOCATED_ACTIVITY){
					if(best_solution.times[ai]==currentTime){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(act->duration==1)
							tos<<"          <td>";
						else
							tos<<"          <td rowspan=\""<<act->duration<<"\">";
						if(act->subjectName.size()>0||act->activityTagName.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"line1\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->activityTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->activityTagName); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						if(act->teachersNames.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"teacher line2\">";
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
									default: tos<<protect2(*it); break;
								}
								if(it!=act->teachersNames.end()-1)
									tos<<", ";
							}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						int r=best_solution.rooms[ai];
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"room line3\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else {
					if((subgroupNotAvailableDayHour[subgroup][k][j] || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
						switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
							case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
							case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
							default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
						}
					} else {
						switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
							case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
							case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
							default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
						}
					}
				}
			}
			tos<<"        </tr>\n";
		}
		//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
		//workaround end.
		tos<<"      </tbody>\n";
		if(subgroup!=gt.rules.nInternalSubgroups-1){
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
			tos<<"    <p>&nbsp;</p>\n\n";
		} else {
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
		}
	}
	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}


//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the subgroups' timetable in html format to a file.
Days vertical
*/
void TimetableExport::writeSubgroupsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n";
		
	tos<<"    <p><strong>"<<TimetableExport::tr("Table of content")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		tos<<"      <li>\n        "<<TimetableExport::tr("Year")<<" "<<protect2(sty->name)<<"\n        <ul>\n";
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			tos<<"          <li>\n            "<<TimetableExport::tr("Group")<<" "<<protect2(stg->name)<<": \n";
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
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"xAxis\">";
			else
				tos<<"          <th>";
			tos<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
		}
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";
		/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		*/
		tos<<"      <tbody>\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			tos<<"        <tr>\n";
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"yAxis\">";
			else
				tos<<"          <th>";
			tos<<protect2(gt.rules.daysOfTheWeek[k])<<"</th>\n";

			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				int ai=students_timetable_weekly[subgroup][k][j]; //activity index
				int currentTime=k+gt.rules.nDaysPerWeek*j;
				if(ai!=UNALLOCATED_ACTIVITY){
					if(best_solution.times[ai]==currentTime){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(act->duration==1)
							tos<<"          <td>";
						else
							tos<<"          <td colspan=\""<<act->duration<<"\">";
						if(act->subjectName.size()>0||act->activityTagName.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"line1\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->activityTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->activityTagName); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						if(act->teachersNames.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"teacher line2\">";
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
									default: tos<<protect2(*it); break;
								}
								if(it!=act->teachersNames.end()-1)
									tos<<", ";
							}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						int r=best_solution.rooms[ai];
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"room line3\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else {
					if((subgroupNotAvailableDayHour[subgroup][k][j] || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
						switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
							case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
							case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
							default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
						}
					} else {
						switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
							case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
							case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
							default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
						}
					}
				}
			}
			tos<<"        </tr>\n";
		}
		//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
		//workaround end.
		tos<<"      </tbody>\n";
		if(subgroup!=gt.rules.nInternalSubgroups-1){
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
			tos<<"    <p>&nbsp;</p>\n\n";
		} else {
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
		}
	}

	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}


//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing the subgroups' timetable html format to a file
Time vertical
*/

void TimetableExport::writeSubgroupsTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <table border=\"1\">\n";
	tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

	tos<<"      <thead>\n        <tr><td colspan=\"2\"></td>";
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		if(TIMETABLE_HTML_LEVEL>=2)
			tos<<"          <th class=\"xAxis\">";
		else
			tos<<"          <th>";
		tos << gt.rules.internalSubgroupsList[i]->name << "</th>";
	}
	tos<<"</tr>\n      </thead>\n";
	/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalSubgroups<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
	*/
	tos<<"      <tbody>\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++){
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			tos << "        <tr>\n";
			if(j==0)
				tos << "        <th rowspan=\"" << gt.rules.nHoursPerDay  << "\">" << protect2vert(gt.rules.daysOfTheWeek[k]) << "</th>\n";
			else tos <<"          <!-- span -->\n";

			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"yAxis\">";
			else
				tos<<"          <th>";
			tos << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
			for(int i=0; i<gt.rules.nInternalSubgroups; i++){
				int currentTime=k+gt.rules.nDaysPerWeek*j;
				int ai=students_timetable_weekly[i][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					if(best_solution.times[ai]==currentTime){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(act->duration==1)
							tos<<"          <td>";
						else
							tos<<"          <td rowspan=\""<<act->duration<<"\">";
						if(act->subjectName.size()>0||act->activityTagName.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"line1\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->activityTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->activityTagName); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						if(act->teachersNames.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"teacher line2\">";
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
									default: tos<<protect2(*it); break;
								}
								if(it!=act->teachersNames.end()-1)
									tos<<", ";
							}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						int r=best_solution.rooms[ai];
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"room line3\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else {
					if((subgroupNotAvailableDayHour[i][k][j] || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
						switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
							case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
							case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
							default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
						}
					} else {
						switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
							case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
							case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
							default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
						}
					}
				}
			}
			tos<<"        </tr>\n";
		}
	}
	//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalSubgroups<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
	//workaround end.
	tos << "      </tbody>\n    </table>\n";
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}	
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the subgroups' timetable in html format to a file.
Time horizontal
*/
void TimetableExport::writeSubgroupsTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <table border=\"1\">\n";
	tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

	tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td>";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		tos << "<th colspan=\"" << gt.rules.nHoursPerDay <<"\">" << protect2(gt.rules.daysOfTheWeek[k]) << "</th>";
	tos<<"        </tr>\n";
	tos<<"        <tr>\n          <!-- span -->\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"xAxis\">";
			else
				tos<<"          <th>";
			tos << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
		}
	tos<<"        </tr>\n";
	tos<<"      </thead>\n";
	/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
	*/
	tos<<"      <tbody>\n";
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		tos << "        <tr>\n";
		if(TIMETABLE_HTML_LEVEL>=2)
			tos<<"          <th class=\"yAxis\">";
		else
			tos<<"          <th>";
		tos << gt.rules.internalSubgroupsList[i]->name << "</th>\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				int currentTime=k+gt.rules.nDaysPerWeek*j;
				int ai=students_timetable_weekly[i][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					if(best_solution.times[ai]==currentTime){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(act->duration==1)
							tos<<"          <td>";
						else
							tos<<"          <td colspan=\""<<act->duration<<"\">";
						if(act->subjectName.size()>0||act->activityTagName.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"line1\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->activityTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->activityTagName); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						if(act->teachersNames.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"teacher line2\">";
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
									default: tos<<protect2(*it); break;
								}
								if(it!=act->teachersNames.end()-1)
									tos<<", ";
							}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						int r=best_solution.rooms[ai];
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"room line3\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else {
					if((subgroupNotAvailableDayHour[i][k][j] || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
						switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
							case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
							case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
							default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
						}
					} else {
						switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
							case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
							case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
							default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
						}
					}
				}
			}
		}
		tos<<"        </tr>\n";
	}
	//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
	//workaround end.
	tos << "      </tbody>\n    </table>\n";
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//Now print the groups

//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing the groups' timetable in html format to a file.
Days horizontal
*/
void TimetableExport::writeGroupsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n"; 
		
	tos<<"    <p><strong>"<<TimetableExport::tr("Table of content")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		tos<<"      <li>\n        "<<TimetableExport::tr("Year")<<" "<<protect2(sty->name)<<"\n        <ul>\n";
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			tos<<"          <li>\n            "<<TimetableExport::tr("Group");
			tos<<" <a href=\""<<"#table_"<<protect2id(stg->name)<<"_DETAILED\">"<<protect2(stg->name)<<" ("<<tr("Detailed")<<")</a> /";
			tos<<" <a href=\""<<"#table_"<<protect2id(stg->name)<<"\">"<<protect2(stg->name)<<" ("<<tr("Less detailed")<<")</a>\n";
			tos<<"          </li>\n";
		}
		tos<<"        </ul>\n      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	bool PRINT_DETAILED=true;
	do{
		int group=0;
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			for(int g=0; g<sty->groupsList.size(); g++){
				StudentsGroup* stg=sty->groupsList[g];
				tos<<"    <table id=\"table_"<<protect2id(stg->name);
				if(PRINT_DETAILED==true) tos<<"_DETAILED";
				tos<<"\" border=\"1\"";
				if(group%2==0) tos<<" class=\"modulo2\"";
				tos<<">\n";
						
				tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

				tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<protect2(stg->name)<<"</th></tr>\n";
				tos<<"        <tr>\n          <!-- span -->\n";
				for(int j=0; j<gt.rules.nDaysPerWeek; j++){
					if(TIMETABLE_HTML_LEVEL>=2)
						tos<<"          <th class=\"xAxis\">";
					else
						tos<<"          <th>";
					tos<<protect2(gt.rules.daysOfTheWeek[j])<<"</th>\n";
				}
				tos<<"        </tr>\n";
				tos<<"      </thead>\n";
				/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
				tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
				*/
				tos<<"      <tbody>\n";
				for(int j=0; j<gt.rules.nHoursPerDay; j++){
					tos<<"        <tr>\n";
					if(TIMETABLE_HTML_LEVEL>=2)
						tos<<"          <th class=\"yAxis\">";
					else
						tos<<"          <th>";
					tos<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
					for(int k=0; k<gt.rules.nDaysPerWeek; k++){
						QList<qint16> allActivities;
						allActivities.clear();
						bool isNotAvailable=true;
						for(int sg=0; sg<stg->subgroupsList.size(); sg++){
							StudentsSubgroup* sts=stg->subgroupsList[sg];
							//old code by Volker
							/*
							for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
								QString subgroup_name = gt.rules.internalSubgroupsList[subgroup]->name;
								if(subgroup_name==sts->name)
							*/
							//speed improvement by Liviu
							int subgroup=sts->indexInInternalSubgroupsList;
									if(!(allActivities.contains(students_timetable_weekly[subgroup][k][j])))
												allActivities<<students_timetable_weekly[subgroup][k][j];
							//}
							if(!subgroupNotAvailableDayHour[subgroup][k][j])
								isNotAvailable=false;
						}
						assert(!allActivities.isEmpty());
						int ai=allActivities[0]; //activity index
						int currentTime=k+gt.rules.nDaysPerWeek*j;
						if(allActivities.size()==1){  // because i am using colspan or rowspan!!!
							if(ai!=UNALLOCATED_ACTIVITY){
								if(best_solution.times[ai]==currentTime){
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(act->duration==1)
										tos<<"          <td>";
									else
										tos<<"          <td rowspan=\""<<act->duration<<"\">";
									if(act->subjectName.size()>0||act->activityTagName.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"line1\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
												case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												default: tos<<protect2(act->subjectName); break;
											}
										if(act->activityTagName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
												case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
												case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
												default: tos<<" "<<protect2(act->activityTagName); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									if(act->teachersNames.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"teacher line2\">";
										for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
											switch(TIMETABLE_HTML_LEVEL){
												case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
												case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
												default: tos<<protect2(*it); break;
											}
											if(it!=act->teachersNames.end()-1)
												tos<<", ";
										}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									int r=best_solution.rooms[ai];
									if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"room line3\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									tos<<"</td>\n";
								} else
									tos<<"          <!-- span -->\n";
							} else {
								if((isNotAvailable || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : ;
										case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
										case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
										default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
									}
								} else {
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : ;
										case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
										case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
										default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
									}
								}
							}
						} else{
							if(PRINT_DETAILED==false) tos<<"          <td>"<<protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)<<"</td>\n";
							else{
								if(TIMETABLE_HTML_LEVEL>=1)
									tos<<"          <td><table class=\"detailed\">";
								else
									tos<<"          <td><table>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"student line0\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->studentsNames.size()>0){
											for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
													case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
													default: tos<<protect2(*st); break;
													}
												if(st!=act->studentsNames.end()-1)
													tos<<", ";
											}
										}
										tos<<"</td>";	
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"line1\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->subjectName.size()>0||act->activityTagName.size()>0){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
													case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													default: tos<<protect2(act->subjectName); break;
												}
											if(act->activityTagName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
													case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
													case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
													default: tos<<" "<<protect2(act->activityTagName); break;
												}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"teacher line2\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->teachersNames.size()>0){
											for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
													case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
													default: tos<<protect2(*it); break;
												}
												if(it!=act->teachersNames.end()-1)
													tos<<", ";
											}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"room line3\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										int r=best_solution.rooms[ai];
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
													case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
													default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
												}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								tos<<"</table></td>\n";
							}
						}
					}
					tos<<"        </tr>\n";
				}
				//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
				tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
				//workaround end.
				tos<<"      </tbody>\n";
				if(((i!=gt.rules.augmentedYearsList.size()-1) && (g!=sty->groupsList.size())) || (PRINT_DETAILED==true)){
					tos<<"    </table>\n    <p>&nbsp;</p>\n";
					tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
					tos<<"    <p>&nbsp;</p>\n\n";
				} else {
					tos<<"    </table>\n    <p>&nbsp;</p>\n";
					tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
				}
				group++;
			}
		}
		if(PRINT_DETAILED==true) PRINT_DETAILED=false;
		else PRINT_DETAILED=true;
	} while(PRINT_DETAILED!=true);
	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}



//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing the groups' timetable in html format to a file.
Days vertical
*/
void TimetableExport::writeGroupsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n"; 
		
	tos<<"    <p><strong>"<<TimetableExport::tr("Table of content")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		tos<<"      <li>\n        "<<TimetableExport::tr("Year")<<" "<<protect2(sty->name)<<"\n        <ul>\n";
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			tos<<"          <li>\n            "<<TimetableExport::tr("Group");
			tos<<" <a href=\""<<"#table_"<<protect2id(stg->name)<<"_DETAILED\">"<<protect2(stg->name)<<" ("<<tr("Detailed")<<")</a> /";
			tos<<" <a href=\""<<"#table_"<<protect2id(stg->name)<<"\">"<<protect2(stg->name)<<" ("<<tr("Less detailed")<<")</a>\n";
			tos<<"          </li>\n";
		}
		tos<<"        </ul>\n      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n";

	bool PRINT_DETAILED=true;
	do{
		int group=0;
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			for(int g=0; g<sty->groupsList.size(); g++){
				StudentsGroup* stg=sty->groupsList[g];
				tos<<"    <table id=\"table_"<<protect2id(stg->name);
				if(PRINT_DETAILED==true) tos<<"_DETAILED";
				tos<<"\" border=\"1\"";
				if(group%2==0) tos<<" class=\"modulo2\"";
				tos<<">\n";
						
				tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

				tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<protect2(stg->name)<<"</th></tr>\n";
				tos<<"        <tr>\n          <!-- span -->\n";
				for(int j=0; j<gt.rules.nHoursPerDay; j++){
					if(TIMETABLE_HTML_LEVEL>=2)
						tos<<"          <th class=\"xAxis\">";
					else
						tos<<"          <th>";
					tos<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
				}
				tos<<"        </tr>\n";
				tos<<"      </thead>\n";
				/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
				tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
				*/
				tos<<"      <tbody>\n";
				for(int k=0; k<gt.rules.nDaysPerWeek; k++){
					tos<<"        <tr>\n";
					if(TIMETABLE_HTML_LEVEL>=2)
						tos<<"          <th class=\"yAxis\">";
					else
						tos<<"          <th>";
					tos<<protect2(gt.rules.daysOfTheWeek[k])<<"</th>\n";
					for(int j=0; j<gt.rules.nHoursPerDay; j++){
						QList<qint16> allActivities;
						allActivities.clear();
						bool isNotAvailable=true;
						for(int sg=0; sg<stg->subgroupsList.size(); sg++){
							StudentsSubgroup* sts=stg->subgroupsList[sg];
							//old code by Volker
							/*
							for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
								QString subgroup_name = gt.rules.internalSubgroupsList[subgroup]->name;
								if(subgroup_name==sts->name)
							*/
							//speed improvement by Liviu
							int subgroup=sts->indexInInternalSubgroupsList;
									if(!(allActivities.contains(students_timetable_weekly[subgroup][k][j])))
												allActivities<<students_timetable_weekly[subgroup][k][j];
							//}
							if(!subgroupNotAvailableDayHour[subgroup][k][j])
								isNotAvailable=false;
						}
						assert(!allActivities.isEmpty());
						int ai=allActivities[0]; //activity index
						int currentTime=k+gt.rules.nDaysPerWeek*j;
						if(allActivities.size()==1){  // because i am using colspan or rowspan!!!
							if(ai!=UNALLOCATED_ACTIVITY){
								if(best_solution.times[ai]==currentTime){
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(act->duration==1)
										tos<<"          <td>";
									else
										tos<<"          <td colspan=\""<<act->duration<<"\">";
									if(act->subjectName.size()>0||act->activityTagName.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"line1\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
												case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												default: tos<<protect2(act->subjectName); break;
											}
										if(act->activityTagName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
												case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
												case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
												default: tos<<" "<<protect2(act->activityTagName); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									if(act->teachersNames.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"teacher line2\">";
										for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
											switch(TIMETABLE_HTML_LEVEL){
												case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
												case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
												default: tos<<protect2(*it); break;
											}
											if(it!=act->teachersNames.end()-1)
												tos<<", ";
										}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									int r=best_solution.rooms[ai];
									if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"room line3\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									tos<<"</td>\n";
								} else
									tos<<"          <!-- span -->\n";
							} else {
								if((isNotAvailable || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : ;
										case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
										case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
										default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
									}
								} else {
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : ;
										case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
										case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
										default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
									}
								}
							}
						} else{
							if(PRINT_DETAILED==false) tos<<"          <td>"<<protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)<<"</td>\n";
							else{
								if(TIMETABLE_HTML_LEVEL>=1)
									tos<<"          <td><table class=\"detailed\">";
								else
									tos<<"          <td><table>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"student line0\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->studentsNames.size()>0){
											for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
													case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
													default: tos<<protect2(*st); break;
													}
												if(st!=act->studentsNames.end()-1)
													tos<<", ";
											}
										}
										tos<<"</td>";	
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"line1\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->subjectName.size()>0||act->activityTagName.size()>0){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
													case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													default: tos<<protect2(act->subjectName); break;
												}
											if(act->activityTagName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
													case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
													case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
													default: tos<<" "<<protect2(act->activityTagName); break;
												}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"teacher line2\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->teachersNames.size()>0){
											for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
													case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
													default: tos<<protect2(*it); break;
												}
												if(it!=act->teachersNames.end()-1)
													tos<<", ";
											}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"room line3\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										int r=best_solution.rooms[ai];
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
													case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
													default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
												}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								tos<<"</table></td>\n";
							}
						}
					}
					tos<<"        </tr>\n";
				}
				//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
				tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
				//workaround end.
				tos<<"      </tbody>\n";
				if(((i!=gt.rules.augmentedYearsList.size()-1) && (g!=sty->groupsList.size())) || (PRINT_DETAILED==true)){
					tos<<"    </table>\n    <p>&nbsp;</p>\n";
					tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
					tos<<"    <p>&nbsp;</p>\n\n";
				} else {
					tos<<"    </table>\n    <p>&nbsp;</p>\n";
					tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
				}
				group++;
			}
		}
		if(PRINT_DETAILED==true) PRINT_DETAILED=false;
		else PRINT_DETAILED=true;
	} while(PRINT_DETAILED!=true);
	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}


//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing the groups' timetable html format to a file
Time vertical
*/

void TimetableExport::writeGroupsTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	bool PRINT_DETAILED=true;
	do{
		tos<<"    <table id=\"table_"<<protect2id(gt.rules.institutionName);
		if(PRINT_DETAILED==true) tos<<"_DETAILED";
		tos<<"\" border=\"1\">\n";

		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		tos<<"      <thead>\n        <tr><td colspan=\"2\"></td>";
		int group=0;
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			for(int g=0; g<sty->groupsList.size(); g++){
				group++;
				StudentsGroup* stg=sty->groupsList[g];
				if(TIMETABLE_HTML_LEVEL>=2)
					tos<<"          <th class=\"xAxis\">";
				else
					tos<<"          <th>";
				tos << stg->name << "</th>";
			}
		}
		
		tos<<"</tr>\n      </thead>\n";
		/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<group<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		*/
		tos<<"      <tbody>\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos << "        <tr>\n";
				if(j==0)
					tos << "        <th rowspan=\"" << gt.rules.nHoursPerDay  << "\">" << protect2vert(gt.rules.daysOfTheWeek[k]) << "</th>\n";
				else tos <<"          <!-- span -->\n";
				if(TIMETABLE_HTML_LEVEL>=2)
					tos<<"          <th class=\"yAxis\">";
				else
					tos<<"          <th>";
				tos << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
				for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
					StudentsYear* sty=gt.rules.augmentedYearsList[i];
					for(int g=0; g<sty->groupsList.size(); g++){
						StudentsGroup* stg=sty->groupsList[g];
						QList<qint16> allActivities;
						allActivities.clear();
						bool isNotAvailable=true;
						for(int sg=0; sg<stg->subgroupsList.size(); sg++){
							StudentsSubgroup* sts=stg->subgroupsList[sg];
							//old code by Volker
							/*
							for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
								QString subgroup_name = gt.rules.internalSubgroupsList[subgroup]->name;
								if(subgroup_name==sts->name)
							*/
							//speed improvement by Liviu
							int subgroup=sts->indexInInternalSubgroupsList;
									if(!(allActivities.contains(students_timetable_weekly[subgroup][k][j])))
												allActivities<<students_timetable_weekly[subgroup][k][j];
							//}
							if(!subgroupNotAvailableDayHour[subgroup][k][j])
								isNotAvailable=false;
						}
						assert(!allActivities.isEmpty());
						int ai=allActivities[0]; //activity index
						int currentTime=k+gt.rules.nDaysPerWeek*j;
						if(allActivities.size()==1){  // because i am using colspan or rowspan!!!
							if(ai!=UNALLOCATED_ACTIVITY){
								if(best_solution.times[ai]==currentTime){
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(act->duration==1)
										tos<<"          <td>";
									else
										tos<<"          <td rowspan=\""<<act->duration<<"\">";
									if(act->subjectName.size()>0||act->activityTagName.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"line1\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
												case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												default: tos<<protect2(act->subjectName); break;
											}
										if(act->activityTagName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
												case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
												case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
												default: tos<<" "<<protect2(act->activityTagName); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									if(act->teachersNames.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"teacher line2\">";
										for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
											switch(TIMETABLE_HTML_LEVEL){
												case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
												case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
												default: tos<<protect2(*it); break;
											}
											if(it!=act->teachersNames.end()-1)
												tos<<", ";
										}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									int r=best_solution.rooms[ai];
									if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"room line3\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									tos<<"</td>\n";
								} else
									tos<<"          <!-- span -->\n";
							} else {
								if((isNotAvailable || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : ;
										case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
										case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
										default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
									}
								} else {
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : ;
										case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
										case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
										default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
									}
								}
							}
						} else{
							if(PRINT_DETAILED==false) tos<<"          <td>"<<protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)<<"</td>\n";
							else{
								if(TIMETABLE_HTML_LEVEL>=1)
									tos<<"          <td><table class=\"detailed\">";
								else
									tos<<"          <td><table>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"student line0\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->studentsNames.size()>0){
											for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
													case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
													default: tos<<protect2(*st); break;
													}
												if(st!=act->studentsNames.end()-1)
													tos<<", ";
											}
										}
										tos<<"</td>";	
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"line1\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->subjectName.size()>0||act->activityTagName.size()>0){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
													case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													default: tos<<protect2(act->subjectName); break;
												}
											if(act->activityTagName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
													case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
													case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
													default: tos<<" "<<protect2(act->activityTagName); break;
												}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"teacher line2\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->teachersNames.size()>0){
											for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
													case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
													default: tos<<protect2(*it); break;
												}
												if(it!=act->teachersNames.end()-1)
													tos<<", ";
											}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"room line3\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										int r=best_solution.rooms[ai];
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
													case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
													default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
												}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								tos<<"</table></td>\n";
							}
						}
					}
				}
				tos<<"        </tr>\n";
			}
		}
		//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""<<group<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
		//workaround end.
		tos<<"      </tbody>\n";
		if(PRINT_DETAILED==true){
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
			tos<<"    <p>&nbsp;</p>\n\n";
		} else {
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
		}
		if(PRINT_DETAILED==true) PRINT_DETAILED=false;
		else PRINT_DETAILED=true;
	} while(PRINT_DETAILED!=true);
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing the groups' timetable in html format to a file.
Time horizontal
*/
void TimetableExport::writeGroupsTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";
	
	bool PRINT_DETAILED=true;
	do{
		tos<<"    <table id=\"table_"<<protect2id(gt.rules.institutionName);
		if(PRINT_DETAILED==true) tos<<"_DETAILED";
		tos<<"\" border=\"1\">\n";

		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td>";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++)
			tos << "<th colspan=\"" << gt.rules.nHoursPerDay <<"\">" << protect2(gt.rules.daysOfTheWeek[k]) << "</th>";
		tos <<"</tr>\n";
		tos<<"        <tr>\n          <!-- span -->\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++)
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				if(TIMETABLE_HTML_LEVEL>=2)
					tos<<"          <th class=\"xAxis\">";
				else
					tos<<"          <th>";
				tos << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
			}
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";
		/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		*/
		tos<<"      <tbody>\n";
		
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			for(int g=0; g<sty->groupsList.size(); g++){
				StudentsGroup* stg=sty->groupsList[g];
				tos << "        <tr>\n";
				if(TIMETABLE_HTML_LEVEL>=2)
					tos<<"          <th class=\"yAxis\">";
				else
					tos<<"          <th>";
				tos << protect2(stg->name) << "</th>\n";
				for(int k=0; k<gt.rules.nDaysPerWeek; k++){
					for(int j=0; j<gt.rules.nHoursPerDay; j++){
						QList<qint16> allActivities;
						allActivities.clear();
						bool isNotAvailable=true;
						for(int sg=0; sg<stg->subgroupsList.size(); sg++){
							StudentsSubgroup* sts=stg->subgroupsList[sg];
							//old code by Volker
							/*
							for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
								QString subgroup_name = gt.rules.internalSubgroupsList[subgroup]->name;
								if(subgroup_name==sts->name)
							*/
							//speed improvement by Liviu
							int subgroup=sts->indexInInternalSubgroupsList;
									if(!(allActivities.contains(students_timetable_weekly[subgroup][k][j])))
												allActivities<<students_timetable_weekly[subgroup][k][j];
							//}
							if(!subgroupNotAvailableDayHour[subgroup][k][j])
								isNotAvailable=false;
						}
						assert(!allActivities.isEmpty());
						int ai=allActivities[0]; //activity index
						int currentTime=k+gt.rules.nDaysPerWeek*j;
						if(allActivities.size()==1){  // because i am using colspan or rowspan!!!
							if(ai!=UNALLOCATED_ACTIVITY){
								if(best_solution.times[ai]==currentTime){
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(act->duration==1)
										tos<<"          <td>";
									else
										tos<<"          <td colspan=\""<<act->duration<<"\">";
									if(act->subjectName.size()>0||act->activityTagName.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"line1\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
												case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												default: tos<<protect2(act->subjectName); break;
											}
										if(act->activityTagName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
												case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
												case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
												default: tos<<" "<<protect2(act->activityTagName); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									if(act->teachersNames.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"teacher line2\">";
										for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
											switch(TIMETABLE_HTML_LEVEL){
												case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
												case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
												default: tos<<protect2(*it); break;
											}
											if(it!=act->teachersNames.end()-1)
												tos<<", ";
										}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									int r=best_solution.rooms[ai];
									if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"room line3\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									tos<<"</td>\n";
								} else
									tos<<"          <!-- span -->\n";
							} else {
								if((isNotAvailable || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : ;
										case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
										case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
										default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
									}
								} else {
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : ;
										case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
										case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
										default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
									}
								}
							}
						} else{
							if(PRINT_DETAILED==false) tos<<"          <td>"<<protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)<<"</td>\n";
							else{
								if(TIMETABLE_HTML_LEVEL>=1)
									tos<<"          <td><table class=\"detailed\">";
								else
									tos<<"          <td><table>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"student line0\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->studentsNames.size()>0){
											for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
													case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
													default: tos<<protect2(*st); break;
													}
												if(st!=act->studentsNames.end()-1)
													tos<<", ";
											}
										}
										tos<<"</td>";	
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"line1\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->subjectName.size()>0||act->activityTagName.size()>0){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
													case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													default: tos<<protect2(act->subjectName); break;
												}
											if(act->activityTagName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
													case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
													case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
													default: tos<<" "<<protect2(act->activityTagName); break;
												}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"teacher line2\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->teachersNames.size()>0){
											for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
													case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
													default: tos<<protect2(*it); break;
												}
												if(it!=act->teachersNames.end()-1)
													tos<<", ";
											}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"room line3\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										int r=best_solution.rooms[ai];
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
													case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
													default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
												}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								tos<<"</table></td>\n";
							}
						}
					}
				}
				tos<<"        </tr>\n";
			}
		}
		//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
		//workaround end.
		tos<<"      </tbody>\n";
		if(PRINT_DETAILED==true){
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
			tos<<"    <p>&nbsp;</p>\n\n";
		} else {
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
		}
		if(PRINT_DETAILED==true) PRINT_DETAILED=false;
		else PRINT_DETAILED=true;
	} while(PRINT_DETAILED!=true);
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//Now print the years

//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing the years' timetable in html format to a file.
Days horizontal
*/
void TimetableExport::writeYearsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n"; 
		
	tos<<"    <p><strong>"<<TimetableExport::tr("Table of content")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		tos<<"      <li>\n        "<<TimetableExport::tr("Year");
		tos<<" <a href=\""<<"#table_"<<protect2id(sty->name)<<"_DETAILED\">"<<protect2(sty->name)<<" ("<<tr("Detailed")<<")</a> /";
		tos<<" <a href=\""<<"#table_"<<protect2id(sty->name)<<"\">"<<protect2(sty->name)<<" ("<<tr("Less detailed")<<")</a>\n";
		tos<<"      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	bool PRINT_DETAILED=true;
	do{
		for(int year=0; year<gt.rules.augmentedYearsList.size(); year++){
			StudentsYear* sty=gt.rules.augmentedYearsList[year];
				tos<<"    <table id=\"table_"<<protect2id(sty->name);
				if(PRINT_DETAILED==true) tos<<"_DETAILED";
				tos<<"\" border=\"1\"";
				if(year%2==0) tos<<" class=\"modulo2\"";
				tos<<">\n";
						
				tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

				tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<protect2(sty->name)<<"</th></tr>\n";
				tos<<"        <tr>\n          <!-- span -->\n";
				for(int j=0; j<gt.rules.nDaysPerWeek; j++){
					if(TIMETABLE_HTML_LEVEL>=2)
						tos<<"          <th class=\"xAxis\">";
					else
						tos<<"          <th>";
					tos<<protect2(gt.rules.daysOfTheWeek[j])<<"</th>\n";
				}
				tos<<"        </tr>\n";
				tos<<"      </thead>\n";
				/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
				tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
				*/
				tos<<"      <tbody>\n";
				for(int j=0; j<gt.rules.nHoursPerDay; j++){
					tos<<"        <tr>\n";
					if(TIMETABLE_HTML_LEVEL>=2)
						tos<<"          <th class=\"yAxis\">";
					else
						tos<<"          <th>";
					tos<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
					for(int k=0; k<gt.rules.nDaysPerWeek; k++){
						QList<qint16> allActivities;
						allActivities.clear();
						bool isNotAvailable=true;
						for(int g=0; g<sty->groupsList.size(); g++){
							StudentsGroup* stg=sty->groupsList[g];
							for(int sg=0; sg<stg->subgroupsList.size(); sg++){
								StudentsSubgroup* sts=stg->subgroupsList[sg];
								//old code by Volker
								/*
								for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
									QString subgroup_name = gt.rules.internalSubgroupsList[subgroup]->name;
									if(subgroup_name==sts->name)
								*/
								//speed improvement by Liviu
								int subgroup=sts->indexInInternalSubgroupsList;
										if(!(allActivities.contains(students_timetable_weekly[subgroup][k][j])))
													allActivities<<students_timetable_weekly[subgroup][k][j];
								//}
								if(!subgroupNotAvailableDayHour[subgroup][k][j])
									isNotAvailable=false;
							}
						}
						assert(!allActivities.isEmpty());
						int ai=allActivities[0]; //activity index
						int currentTime=k+gt.rules.nDaysPerWeek*j;
						if(allActivities.size()==1){  // because i am using colspan or rowspan!!!
							if(ai!=UNALLOCATED_ACTIVITY){
								if(best_solution.times[ai]==currentTime){
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(act->duration==1)
										tos<<"          <td>";
									else
										tos<<"          <td rowspan=\""<<act->duration<<"\">";
									if(act->subjectName.size()>0||act->activityTagName.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"line1\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
												case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												default: tos<<protect2(act->subjectName); break;
											}
										if(act->activityTagName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
												case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
												case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
												default: tos<<" "<<protect2(act->activityTagName); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									if(act->teachersNames.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"teacher line2\">";
										for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
											switch(TIMETABLE_HTML_LEVEL){
												case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
												case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
												default: tos<<protect2(*it); break;
											}
											if(it!=act->teachersNames.end()-1)
												tos<<", ";
										}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									int r=best_solution.rooms[ai];
									if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"room line3\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									tos<<"</td>\n";
								} else
									tos<<"          <!-- span -->\n";
							} else {
								if((isNotAvailable || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : ;
										case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
										case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
										default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
									}
								} else {
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : ;
										case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
										case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
										default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
									}
								}
							}
						} else{
							if(PRINT_DETAILED==false) tos<<"          <td>"<<protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)<<"</td>\n";
							else{
								if(TIMETABLE_HTML_LEVEL>=1)
									tos<<"          <td><table class=\"detailed\">";
								else
									tos<<"          <td><table>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"student line0\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->studentsNames.size()>0){
											for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
													case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
													default: tos<<protect2(*st); break;
													}
												if(st!=act->studentsNames.end()-1)
													tos<<", ";
											}
										}
										tos<<"</td>";	
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"line1\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->subjectName.size()>0||act->activityTagName.size()>0){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
													case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													default: tos<<protect2(act->subjectName); break;
												}
											if(act->activityTagName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
													case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
													case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
													default: tos<<" "<<protect2(act->activityTagName); break;
												}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"teacher line2\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->teachersNames.size()>0){
											for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
													case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
													default: tos<<protect2(*it); break;
												}
												if(it!=act->teachersNames.end()-1)
													tos<<", ";
											}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"room line3\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										int r=best_solution.rooms[ai];
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
													case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
													default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
												}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								tos<<"</table></td>\n";
							}
						}
					}
					tos<<"        </tr>\n";
				}
				//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
				tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
				//workaround end.
				tos<<"      </tbody>\n";
				if((year!=gt.rules.augmentedYearsList.size()-1) || (PRINT_DETAILED==true)){
					tos<<"    </table>\n    <p>&nbsp;</p>\n";
					tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
					tos<<"    <p>&nbsp;</p>\n\n";
				} else {
					tos<<"    </table>\n    <p>&nbsp;</p>\n";
					tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
				}
			
		}
		if(PRINT_DETAILED==true) PRINT_DETAILED=false;
		else PRINT_DETAILED=true;
	} while(PRINT_DETAILED!=true);
	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}


//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing the years' timetable in html format to a file.
Days vertical
*/
void TimetableExport::writeYearsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n"; 
		
	tos<<"    <p><strong>"<<TimetableExport::tr("Table of content")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		tos<<"      <li>\n        "<<TimetableExport::tr("Year");
		tos<<" <a href=\""<<"#table_"<<protect2id(sty->name)<<"_DETAILED\">"<<protect2(sty->name)<<" ("<<tr("Detailed")<<")</a> /";
		tos<<" <a href=\""<<"#table_"<<protect2id(sty->name)<<"\">"<<protect2(sty->name)<<" ("<<tr("Less detailed")<<")</a>\n";
		tos<<"      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	bool PRINT_DETAILED=true;
	do{
		for(int year=0; year<gt.rules.augmentedYearsList.size(); year++){
				StudentsYear* sty=gt.rules.augmentedYearsList[year];
				tos<<"    <table id=\"table_"<<protect2id(sty->name);
				if(PRINT_DETAILED==true) tos<<"_DETAILED";
				tos<<"\" border=\"1\"";
				if(year%2==0) tos<<" class=\"modulo2\"";
				tos<<">\n";
						
				tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

				tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<protect2(sty->name)<<"</th></tr>\n";
				tos<<"        <tr>\n          <!-- span -->\n";
				for(int j=0; j<gt.rules.nHoursPerDay; j++){
					if(TIMETABLE_HTML_LEVEL>=2)
						tos<<"          <th class=\"xAxis\">";
					else
						tos<<"          <th>";
					tos<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
				}
				tos<<"        </tr>\n";
				tos<<"      </thead>\n";
				/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
				tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
				*/
				tos<<"      <tbody>\n";
				for(int k=0; k<gt.rules.nDaysPerWeek; k++){
					tos<<"        <tr>\n";
					if(TIMETABLE_HTML_LEVEL>=2)
						tos<<"          <th class=\"yAxis\">";
					else
						tos<<"          <th>";
					tos<<protect2(gt.rules.daysOfTheWeek[k])<<"</th>\n";

					for(int j=0; j<gt.rules.nHoursPerDay; j++){
						QList<qint16> allActivities;
						allActivities.clear();
						bool isNotAvailable=true;
						for(int g=0; g<sty->groupsList.size(); g++){
							StudentsGroup* stg=sty->groupsList[g];
							for(int sg=0; sg<stg->subgroupsList.size(); sg++){
								StudentsSubgroup* sts=stg->subgroupsList[sg];
								//old code by Volker
								/*
								for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
									QString subgroup_name = gt.rules.internalSubgroupsList[subgroup]->name;
									if(subgroup_name==sts->name)
								*/
								//speed improvement by Liviu
								int subgroup=sts->indexInInternalSubgroupsList;
										if(!(allActivities.contains(students_timetable_weekly[subgroup][k][j])))
													allActivities<<students_timetable_weekly[subgroup][k][j];
								//}
								if(!subgroupNotAvailableDayHour[subgroup][k][j])
									isNotAvailable=false;
							}
						}
						assert(!allActivities.isEmpty());
						int ai=allActivities[0]; //activity index
						int currentTime=k+gt.rules.nDaysPerWeek*j;
						if(allActivities.size()==1){  // because i am using colspan or rowspan!!!
							if(ai!=UNALLOCATED_ACTIVITY){
								if(best_solution.times[ai]==currentTime){
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(act->duration==1)
										tos<<"          <td>";
									else
										tos<<"          <td colspan=\""<<act->duration<<"\">";
									if(act->subjectName.size()>0||act->activityTagName.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"line1\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
												case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												default: tos<<protect2(act->subjectName); break;
											}
										if(act->activityTagName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
												case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
												case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
												default: tos<<" "<<protect2(act->activityTagName); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									if(act->teachersNames.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"teacher line2\">";
										for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
											switch(TIMETABLE_HTML_LEVEL){
												case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
												case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
												default: tos<<protect2(*it); break;
											}
											if(it!=act->teachersNames.end()-1)
												tos<<", ";
										}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									int r=best_solution.rooms[ai];
									if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"room line3\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									tos<<"</td>\n";
								} else
									tos<<"          <!-- span -->\n";
							} else {
								if((isNotAvailable || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : ;
										case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
										case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
										default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
									}
								} else {
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : ;
										case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
										case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
										default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
									}
								}
							}
						} else{
							if(PRINT_DETAILED==false) tos<<"          <td>"<<protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)<<"</td>\n";
							else{
								if(TIMETABLE_HTML_LEVEL>=1)
									tos<<"          <td><table class=\"detailed\">";
								else
									tos<<"          <td><table>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"student line0\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->studentsNames.size()>0){
											for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
													case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
													default: tos<<protect2(*st); break;
													}
												if(st!=act->studentsNames.end()-1)
													tos<<", ";
											}
										}
										tos<<"</td>";	
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"line1\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->subjectName.size()>0||act->activityTagName.size()>0){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
													case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													default: tos<<protect2(act->subjectName); break;
												}
											if(act->activityTagName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
													case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
													case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
													default: tos<<" "<<protect2(act->activityTagName); break;
												}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"teacher line2\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->teachersNames.size()>0){
											for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
													case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
													default: tos<<protect2(*it); break;
												}
												if(it!=act->teachersNames.end()-1)
													tos<<", ";
											}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"room line3\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										int r=best_solution.rooms[ai];
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
													case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
													default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
												}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								tos<<"</table></td>\n";
							}
						}
					}
					tos<<"        </tr>\n";
				}
				//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
				tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
				//workaround end.
				tos<<"      </tbody>\n";
				if((year!=gt.rules.augmentedYearsList.size()-1) &&  (PRINT_DETAILED==true)){
					tos<<"    </table>\n    <p>&nbsp;</p>\n";
					tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
					tos<<"    <p>&nbsp;</p>\n\n";
				} else {
					tos<<"    </table>\n    <p>&nbsp;</p>\n";
					tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
				}
			
		}
		if(PRINT_DETAILED==true) PRINT_DETAILED=false;
		else PRINT_DETAILED=true;
	} while(PRINT_DETAILED!=true);
	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}


//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing the years' timetable html format to a file
Time vertical
*/

void TimetableExport::writeYearsTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	bool PRINT_DETAILED=true;
	do{
		tos<<"    <table id=\"table_"<<protect2id(gt.rules.institutionName);
		if(PRINT_DETAILED==true) tos<<"_DETAILED";
		tos<<"\" border=\"1\">\n";

		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		tos<<"      <thead>\n        <tr><td colspan=\"2\"></td>";
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
				if(TIMETABLE_HTML_LEVEL>=2)
					tos<<"          <th class=\"xAxis\">";
				else
					tos<<"          <th>";
				tos << sty->name << "</th>";
		}
		
		tos<<"</tr>\n      </thead>\n";
		/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.augmentedYearsList.size()<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		*/
		tos<<"      <tbody>\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos << "        <tr>\n";
				if(j==0)
					tos << "        <th rowspan=\"" << gt.rules.nHoursPerDay  << "\">" << protect2vert(gt.rules.daysOfTheWeek[k]) << "</th>\n";
				else tos <<"          <!-- span -->\n";
				if(TIMETABLE_HTML_LEVEL>=2)
					tos<<"          <th class=\"yAxis\">";
				else
					tos<<"          <th>";
				tos << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
				for(int year=0; year<gt.rules.augmentedYearsList.size(); year++){
						StudentsYear* sty=gt.rules.augmentedYearsList[year];
						QList<qint16> allActivities;
						allActivities.clear();
						bool isNotAvailable=true;
						for(int g=0; g<sty->groupsList.size(); g++){
							StudentsGroup* stg=sty->groupsList[g];
							for(int sg=0; sg<stg->subgroupsList.size(); sg++){
								StudentsSubgroup* sts=stg->subgroupsList[sg];
								//old code by Volker
								/*
								for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
									QString subgroup_name = gt.rules.internalSubgroupsList[subgroup]->name;
									if(subgroup_name==sts->name)
								*/
								//speed improvement by Liviu
								int subgroup=sts->indexInInternalSubgroupsList;
										if(!(allActivities.contains(students_timetable_weekly[subgroup][k][j])))
													allActivities<<students_timetable_weekly[subgroup][k][j];
								//}
								if(!subgroupNotAvailableDayHour[subgroup][k][j])
									isNotAvailable=false;
							}
						}
						assert(!allActivities.isEmpty());
						int ai=allActivities[0]; //activity index
						int currentTime=k+gt.rules.nDaysPerWeek*j;
						if(allActivities.size()==1){  // because i am using colspan or rowspan!!!
							if(ai!=UNALLOCATED_ACTIVITY){
								if(best_solution.times[ai]==currentTime){
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(act->duration==1)
										tos<<"          <td>";
									else
										tos<<"          <td rowspan=\""<<act->duration<<"\">";
									if(act->subjectName.size()>0||act->activityTagName.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"line1\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
												case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												default: tos<<protect2(act->subjectName); break;
											}
										if(act->activityTagName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
												case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
												case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
												default: tos<<" "<<protect2(act->activityTagName); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									if(act->teachersNames.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"teacher line2\">";
										for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
											switch(TIMETABLE_HTML_LEVEL){
												case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
												case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
												default: tos<<protect2(*it); break;
											}
											if(it!=act->teachersNames.end()-1)
												tos<<", ";
										}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									int r=best_solution.rooms[ai];
									if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"room line3\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									tos<<"</td>\n";
								} else
									tos<<"          <!-- span -->\n";
							} else {
								if((isNotAvailable || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : ;
										case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
										case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
										default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
									}
								} else {
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : ;
										case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
										case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
										default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
									}
								}
							}
						} else{
							if(PRINT_DETAILED==false) tos<<"          <td>"<<protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)<<"</td>\n";
							else{
								if(TIMETABLE_HTML_LEVEL>=1)
									tos<<"          <td><table class=\"detailed\">";
								else
									tos<<"          <td><table>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"student line0\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->studentsNames.size()>0){
											for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
													case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
													default: tos<<protect2(*st); break;
													}
												if(st!=act->studentsNames.end()-1)
													tos<<", ";
											}
										}
										tos<<"</td>";	
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"line1\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->subjectName.size()>0||act->activityTagName.size()>0){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
													case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													default: tos<<protect2(act->subjectName); break;
												}
											if(act->activityTagName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
													case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
													case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
													default: tos<<" "<<protect2(act->activityTagName); break;
												}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"teacher line2\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->teachersNames.size()>0){
											for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
													case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
													default: tos<<protect2(*it); break;
												}
												if(it!=act->teachersNames.end()-1)
													tos<<", ";
											}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"room line3\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										int r=best_solution.rooms[ai];
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
													case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
													default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
												}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								tos<<"</table></td>\n";
							}
					}
				}
				tos<<"        </tr>\n";
			}
		}
		//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""<<gt.rules.augmentedYearsList.size()<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
		//workaround end.
		tos<<"      </tbody>\n";
		if(PRINT_DETAILED==true){
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
			tos<<"    <p>&nbsp;</p>\n\n";
		} else {
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
		}
		if(PRINT_DETAILED==true) PRINT_DETAILED=false;
		else PRINT_DETAILED=true;
	} while(PRINT_DETAILED!=true);
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing the years' timetable in html format to a file.
Time horizontal
*/
void TimetableExport::writeYearsTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";
	
	bool PRINT_DETAILED=true;
	do{
		tos<<"    <table id=\"table_"<<protect2id(gt.rules.institutionName);
		if(PRINT_DETAILED==true) tos<<"_DETAILED";
		tos<<"\" border=\"1\">\n";

		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td>";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++)
			tos << "<th colspan=\"" << gt.rules.nHoursPerDay <<"\">" << protect2(gt.rules.daysOfTheWeek[k]) << "</th>";
		tos <<"</tr>\n";
		tos<<"        <tr>\n          <!-- span -->\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++)
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				if(TIMETABLE_HTML_LEVEL>=2)
					tos<<"          <th class=\"xAxis\">";
				else
					tos<<"          <th>";
				tos << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
			}
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";
		/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalSubgroups<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		*/
		tos<<"      <tbody>\n";
		
		for(int year=0; year<gt.rules.augmentedYearsList.size(); year++){
			StudentsYear* sty=gt.rules.augmentedYearsList[year];
				tos << "        <tr>\n";
				if(TIMETABLE_HTML_LEVEL>=2)
					tos<<"          <th class=\"yAxis\">";
				else
					tos<<"          <th>";
				tos << protect2(sty->name) << "</th>\n";
				for(int k=0; k<gt.rules.nDaysPerWeek; k++){
					for(int j=0; j<gt.rules.nHoursPerDay; j++){
						QList<qint16> allActivities;
						allActivities.clear();
						bool isNotAvailable=true;
						for(int g=0; g<sty->groupsList.size(); g++){
							StudentsGroup* stg=sty->groupsList[g];
							for(int sg=0; sg<stg->subgroupsList.size(); sg++){
								StudentsSubgroup* sts=stg->subgroupsList[sg];
								//old code by Volker
								/*
								for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
									QString subgroup_name = gt.rules.internalSubgroupsList[subgroup]->name;
									if(subgroup_name==sts->name)
								*/
								//speed improvement by Liviu
								int subgroup=sts->indexInInternalSubgroupsList;
										if(!(allActivities.contains(students_timetable_weekly[subgroup][k][j])))
													allActivities<<students_timetable_weekly[subgroup][k][j];
								//}
								if(!subgroupNotAvailableDayHour[subgroup][k][j])
									isNotAvailable=false;
							}
						}
						assert(!allActivities.isEmpty());
						int ai=allActivities[0]; //activity index
						int currentTime=k+gt.rules.nDaysPerWeek*j;
						if(allActivities.size()==1){  // because i am using colspan or rowspan!!!
							if(ai!=UNALLOCATED_ACTIVITY){
								if(best_solution.times[ai]==currentTime){
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(act->duration==1)
										tos<<"          <td>";
									else
										tos<<"          <td colspan=\""<<act->duration<<"\">";
									if(act->subjectName.size()>0||act->activityTagName.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"line1\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
												case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												default: tos<<protect2(act->subjectName); break;
											}
										if(act->activityTagName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
												case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
												case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
												default: tos<<" "<<protect2(act->activityTagName); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									if(act->teachersNames.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"teacher line2\">";
										for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
											switch(TIMETABLE_HTML_LEVEL){
												case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
												case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
												default: tos<<protect2(*it); break;
											}
											if(it!=act->teachersNames.end()-1)
												tos<<", ";
										}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									int r=best_solution.rooms[ai];
									if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"room line3\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									tos<<"</td>\n";
								} else
									tos<<"          <!-- span -->\n";
							} else {
								if((isNotAvailable || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : ;
										case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
										case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
										default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
									}
								} else {
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : ;
										case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
										case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
										default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
									}
								}
							}
						} else{
							if(PRINT_DETAILED==false) tos<<"          <td>"<<protect2(STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES)<<"</td>\n";
							else{
								if(TIMETABLE_HTML_LEVEL>=1)
									tos<<"          <td><table class=\"detailed\">";
								else
									tos<<"          <td><table>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"student line0\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->studentsNames.size()>0){
											for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
													case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
													default: tos<<protect2(*st); break;
													}
												if(st!=act->studentsNames.end()-1)
													tos<<", ";
											}
										}
										tos<<"</td>";	
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"line1\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->subjectName.size()>0||act->activityTagName.size()>0){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
													case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													default: tos<<protect2(act->subjectName); break;
												}
											if(act->activityTagName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
													case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
													case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
													default: tos<<" "<<protect2(act->activityTagName); break;
												}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"teacher line2\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(act->teachersNames.size()>0){
											for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
													case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
													default: tos<<protect2(*it); break;
												}
												if(it!=act->teachersNames.end()-1)
													tos<<", ";
											}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<tr class=\"room line3\">";
								else	tos<<"<tr>";
								for(int a=0; a<allActivities.size(); a++){
									ai=allActivities[a];
									Activity* act=&gt.rules.internalActivitiesList[ai];
									if(ai!=UNALLOCATED_ACTIVITY){
										int r=best_solution.rooms[ai];
										if(TIMETABLE_HTML_LEVEL>=1)
											tos<<"<td class=\"detailed\">";
										else
											tos<<"<td>";
										if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
													case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
													default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
												}
										}
										tos<<"</td>";
									}
								}
								tos<<"</tr>";
								tos<<"</table></td>\n";
							}
						}
					}
					
				}
				tos<<"        </tr>\n";
		}
		//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
		//workaround end.
		tos<<"      </tbody>\n";
		if(PRINT_DETAILED==true){
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
			tos<<"    <p>&nbsp;</p>\n\n";
		} else {
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
		}
		if(PRINT_DETAILED==true) PRINT_DETAILED=false;
		else PRINT_DETAILED=true;
	} while(PRINT_DETAILED!=true);
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}




//***********************************************************************************************************************************************************************************************

//Now print all activities

//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing all activities' timetable in html format to a file.
Days horizontal
*/
void TimetableExport::writeAllActivitiesTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n"; 
	
	// code by Liviu (start)					//TODO: do this only one time? Maybe in solution.cpp or timetableexport.cpp as a function
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		for(int j=0; j<gt.rules.nHoursPerDay; j++)
			activitiesAtTime[k][j].clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++) {		//maybe TODO: maybe it is better to do this sorted by studnets or teachers?
		Activity* act=&gt.rules.internalActivitiesList[i];
		if(best_solution.times[i]!=UNALLOCATED_TIME) {
			int hour=best_solution.times[i]/gt.rules.nDaysPerWeek;
			int day=best_solution.times[i]%gt.rules.nDaysPerWeek;
			for(int dd=0; dd < act->duration && hour+dd < gt.rules.nHoursPerDay; dd++)
				activitiesAtTime[day][hour+dd].append(i);
		}
	}
	// code by Liviu (end)

	tos<<"    <table border=\"1\">\n";	
	tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";
	tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<tr("All Activities")<<"</th></tr>\n";
	tos<<"        <tr>\n          <!-- span -->\n";
	for(int j=0; j<gt.rules.nDaysPerWeek; j++){
		if(TIMETABLE_HTML_LEVEL>=2)
			tos<<"          <th class=\"xAxis\">";
		else
			tos<<"          <th>";
		tos<<protect2(gt.rules.daysOfTheWeek[j])<<"</th>\n";
	}
	tos<<"        </tr>\n";
	tos<<"      </thead>\n";
	/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
	*/
	tos<<"      <tbody>\n";
	for(int j=0; j<gt.rules.nHoursPerDay; j++){
		tos<<"        <tr>\n";
		if(TIMETABLE_HTML_LEVEL>=2)
			tos<<"          <th class=\"yAxis\">";
		else
			tos<<"          <th>";
		tos<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			if(activitiesAtTime[k][j].isEmpty()){
				if((breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
					switch(TIMETABLE_HTML_LEVEL){
						case 3 : ;
						case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
						case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
						default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
					}
				} else {
					switch(TIMETABLE_HTML_LEVEL){
						case 3 : ;
						case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
						case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
						default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
					}
				}
			} else {
				if(TIMETABLE_HTML_LEVEL>=1)
					tos<<"          <td><table class=\"detailed\">";
				else
					tos<<"          <td><table>";
				if(TIMETABLE_HTML_LEVEL>=3)
					tos<<"<tr class=\"student line0\">";
				else	tos<<"<tr>";
				for(int a=0; a<activitiesAtTime[k][j].size(); a++){
					int ai=activitiesAtTime[k][j].at(a);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(ai!=UNALLOCATED_ACTIVITY){
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->studentsNames.size()>0){
							for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
									default: tos<<protect2(*st); break;
									}
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
						}
						tos<<"</td>";	
					}
				}
				tos<<"</tr>";
				if(TIMETABLE_HTML_LEVEL>=3)
					tos<<"<tr class=\"line1\">";
				else	tos<<"<tr>";
				for(int a=0; a<activitiesAtTime[k][j].size(); a++){
					int ai=activitiesAtTime[k][j].at(a);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(ai!=UNALLOCATED_ACTIVITY){
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->subjectName.size()>0||act->activityTagName.size()>0){
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->activityTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->activityTagName); break;
								}
						}
						tos<<"</td>";
					}
				}
				tos<<"</tr>";
				if(TIMETABLE_HTML_LEVEL>=3)
					tos<<"<tr class=\"teacher line2\">";
				else	tos<<"<tr>";
				for(int a=0; a<activitiesAtTime[k][j].size(); a++){
					int ai=activitiesAtTime[k][j].at(a);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(ai!=UNALLOCATED_ACTIVITY){
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->teachersNames.size()>0){
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
									default: tos<<protect2(*it); break;
								}
								if(it!=act->teachersNames.end()-1)
									tos<<", ";
							}
						}
						tos<<"</td>";
					}
				}
				tos<<"</tr>";
				if(TIMETABLE_HTML_LEVEL>=3)
					tos<<"<tr class=\"room line3\">";
				else	tos<<"<tr>";
				for(int a=0; a<activitiesAtTime[k][j].size(); a++){
					int ai=activitiesAtTime[k][j].at(a);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(ai!=UNALLOCATED_ACTIVITY){
						int r=best_solution.rooms[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
						}
						tos<<"</td>";
					}
				}
				tos<<"</tr>";
				tos<<"</table></td>\n";
			}
		}
		tos<<"        </tr>\n";
	}
	//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
	//workaround end.
	tos<<"      </tbody>\n";
	tos<<"    </table>\n";
	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}


//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing all activities' timetable in html format to a file.
Days vertical
*/
void TimetableExport::writeAllActivitiesTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n"; 
		
	// code by Liviu (start)					//TODO: do this only one time? Maybe in solution.cpp or timetableexport.cpp as a function
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		for(int j=0; j<gt.rules.nHoursPerDay; j++)
			activitiesAtTime[k][j].clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++) {		//maybe TODO: maybe it is better to do this sorted by studnets or teachers?
		Activity* act=&gt.rules.internalActivitiesList[i];
		if(best_solution.times[i]!=UNALLOCATED_TIME) {
			int hour=best_solution.times[i]/gt.rules.nDaysPerWeek;
			int day=best_solution.times[i]%gt.rules.nDaysPerWeek;
			for(int dd=0; dd < act->duration && hour+dd < gt.rules.nHoursPerDay; dd++)
				activitiesAtTime[day][hour+dd].append(i);
		}
	}
	// code by Liviu (end)

	tos<<"    <table border=\"1\">\n";
	tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";
	tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<tr("All Activities")<<"</th></tr>\n";
	tos<<"        <tr>\n          <!-- span -->\n";
	for(int j=0; j<gt.rules.nHoursPerDay; j++){
		if(TIMETABLE_HTML_LEVEL>=2)
			tos<<"          <th class=\"xAxis\">";
		else
			tos<<"          <th>";
		tos<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
	}
	tos<<"        </tr>\n";
	tos<<"      </thead>\n";
	/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
	*/
	tos<<"      <tbody>\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++){
		tos<<"        <tr>\n";
		if(TIMETABLE_HTML_LEVEL>=2)
			tos<<"          <th class=\"yAxis\">";
		else
			tos<<"          <th>";
		tos<<protect2(gt.rules.daysOfTheWeek[k])<<"</th>\n";
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			if(activitiesAtTime[k][j].isEmpty()){
				if((breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
					switch(TIMETABLE_HTML_LEVEL){
						case 3 : ;
						case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
						case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
						default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
					}
				} else {
					switch(TIMETABLE_HTML_LEVEL){
						case 3 : ;
						case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
						case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
						default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
					}
				}
			} else {
				if(TIMETABLE_HTML_LEVEL>=1)
					tos<<"          <td><table class=\"detailed\">";
				else
					tos<<"          <td><table>";
				if(TIMETABLE_HTML_LEVEL>=3)
					tos<<"<tr class=\"student line0\">";
				else	tos<<"<tr>";
				for(int a=0; a<activitiesAtTime[k][j].size(); a++){
					int ai=activitiesAtTime[k][j].at(a);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(ai!=UNALLOCATED_ACTIVITY){
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->studentsNames.size()>0){
							for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
									default: tos<<protect2(*st); break;
								}
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
						}
						tos<<"</td>";	
					}
				}
				tos<<"</tr>";
				if(TIMETABLE_HTML_LEVEL>=3)
					tos<<"<tr class=\"line1\">";
				else	tos<<"<tr>";
				for(int a=0; a<activitiesAtTime[k][j].size(); a++){
					int ai=activitiesAtTime[k][j].at(a);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(ai!=UNALLOCATED_ACTIVITY){
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->subjectName.size()>0||act->activityTagName.size()>0){
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->activityTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->activityTagName); break;
								}
						}
						tos<<"</td>";
					}
				}
				tos<<"</tr>";
				if(TIMETABLE_HTML_LEVEL>=3)
					tos<<"<tr class=\"teacher line2\">";
				else	tos<<"<tr>";
				for(int a=0; a<activitiesAtTime[k][j].size(); a++){
					int ai=activitiesAtTime[k][j].at(a);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(ai!=UNALLOCATED_ACTIVITY){
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->teachersNames.size()>0){
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
									default: tos<<protect2(*it); break;
								}
								if(it!=act->teachersNames.end()-1)
									tos<<", ";
							}
						}
						tos<<"</td>";
					}
				}
				tos<<"</tr>";
				if(TIMETABLE_HTML_LEVEL>=3)
					tos<<"<tr class=\"room line3\">";
				else	tos<<"<tr>";
				for(int a=0; a<activitiesAtTime[k][j].size(); a++){
					int ai=activitiesAtTime[k][j].at(a);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(ai!=UNALLOCATED_ACTIVITY){
						int r=best_solution.rooms[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
						}
						tos<<"</td>";
					}
				}
				tos<<"</tr>";
				tos<<"</table></td>\n";
			}
		}
		tos<<"        </tr>\n";
	}
	//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
	//workaround end.
	tos<<"      </tbody>\n";
	tos<<"    </table>\n";
	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}




//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing all activities' timetable html format to a file
Time vertical
*/

void TimetableExport::writeAllActivitiesTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	// code by Liviu (start)					//TODO: do this only one time? Maybe in solution.cpp or timetableexport.cpp as a function
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		for(int j=0; j<gt.rules.nHoursPerDay; j++)
			activitiesAtTime[k][j].clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++) {		//maybe TODO: maybe it is better to do this sorted by studnets or teachers?
		Activity* act=&gt.rules.internalActivitiesList[i];
		if(best_solution.times[i]!=UNALLOCATED_TIME) {
			int hour=best_solution.times[i]/gt.rules.nDaysPerWeek;
			int day=best_solution.times[i]%gt.rules.nDaysPerWeek;
			for(int dd=0; dd < act->duration && hour+dd < gt.rules.nHoursPerDay; dd++)
				activitiesAtTime[day][hour+dd].append(i);
		}
	}
	// code by Liviu (end)

	tos<<"    <table border=\"1\">\n";
	tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";
	tos<<"      <thead>\n        <tr><td colspan=\"2\"></td>";
	if(TIMETABLE_HTML_LEVEL>=2)
		tos<<"          <th class=\"xAxis\">";
	else
		tos<<"          <th>";
	tos << tr("All Activities"); //Liviu
	tos<<"</th></tr>\n      </thead>\n";
	/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tfoot><tr><td colspan=\"2\"></td><td>"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
	*/
	tos<<"      <tbody>\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++){
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			tos << "        <tr>\n";
			if(j==0)
				tos << "        <th rowspan=\"" << gt.rules.nHoursPerDay  << "\">" << protect2vert(gt.rules.daysOfTheWeek[k]) << "</th>\n";
			else tos <<"          <!-- span -->\n";
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"yAxis\">";
			else
				tos<<"          <th>";
			tos << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
			if(activitiesAtTime[k][j].isEmpty()){
				if((breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
					switch(TIMETABLE_HTML_LEVEL){
						case 3 : ;
						case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
						case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
						default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
					}
				} else {
				switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
					case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
						case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
						default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
					}
				}
			} else {
				if(TIMETABLE_HTML_LEVEL>=1)
					tos<<"          <td><table class=\"detailed\">";
				else
					tos<<"          <td><table>";
				if(TIMETABLE_HTML_LEVEL>=3)
					tos<<"<tr class=\"student line0\">";
				else	tos<<"<tr>";
				for(int a=0; a<activitiesAtTime[k][j].size(); a++){
					int ai=activitiesAtTime[k][j].at(a);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(ai!=UNALLOCATED_ACTIVITY){
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->studentsNames.size()>0){
							for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
									default: tos<<protect2(*st); break;
								}
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
						}
						tos<<"</td>";	
					}
				}
				tos<<"</tr>";
				if(TIMETABLE_HTML_LEVEL>=3)
					tos<<"<tr class=\"line1\">";
				else	tos<<"<tr>";
				for(int a=0; a<activitiesAtTime[k][j].size(); a++){
					int ai=activitiesAtTime[k][j].at(a);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(ai!=UNALLOCATED_ACTIVITY){
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->subjectName.size()>0||act->activityTagName.size()>0){
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->activityTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->activityTagName); break;
								}
						}
						tos<<"</td>";
					}
				}
				tos<<"</tr>";
				if(TIMETABLE_HTML_LEVEL>=3)
					tos<<"<tr class=\"teacher line2\">";
				else	tos<<"<tr>";
				for(int a=0; a<activitiesAtTime[k][j].size(); a++){
					int ai=activitiesAtTime[k][j].at(a);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(ai!=UNALLOCATED_ACTIVITY){
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->teachersNames.size()>0){
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
									default: tos<<protect2(*it); break;
								}
								if(it!=act->teachersNames.end()-1)
									tos<<", ";
							}
						}
						tos<<"</td>";
					}
				}
				tos<<"</tr>";
				if(TIMETABLE_HTML_LEVEL>=3)
					tos<<"<tr class=\"room line3\">";
				else	tos<<"<tr>";
				for(int a=0; a<activitiesAtTime[k][j].size(); a++){
					int ai=activitiesAtTime[k][j].at(a);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(ai!=UNALLOCATED_ACTIVITY){
						int r=best_solution.rooms[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
						}
						tos<<"</td>";
					}
				}
				tos<<"</tr>";
				tos<<"</table></td>\n";
			}
			tos<<"        </tr>\n";
		}
	}
	//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tr class=\"foot\"><td colspan=\"2\"></td><td>"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
	//workaround end.
	tos<<"      </tbody>\n";
	tos<<"    </table>\n";
	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing all activities' timetable in html format to a file.
Time horizontal
*/
void TimetableExport::writeAllActivitiesTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	// code by Liviu (start)					//TODO: do this only one time? Maybe in solution.cpp or timetableexport.cpp as a function
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		for(int j=0; j<gt.rules.nHoursPerDay; j++)
			activitiesAtTime[k][j].clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++) {		//maybe TODO: maybe it is better to do this sorted by studnets or teachers?
		Activity* act=&gt.rules.internalActivitiesList[i];
		if(best_solution.times[i]!=UNALLOCATED_TIME) {
			int hour=best_solution.times[i]/gt.rules.nDaysPerWeek;
			int day=best_solution.times[i]%gt.rules.nDaysPerWeek;
			for(int dd=0; dd < act->duration && hour+dd < gt.rules.nHoursPerDay; dd++)
				activitiesAtTime[day][hour+dd].append(i);
		}
	}
	// code by Liviu (end)


	tos<<"    <table border=\"1\">\n";
	tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";
	tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td>";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		tos << "<th colspan=\"" << gt.rules.nHoursPerDay <<"\">" << protect2(gt.rules.daysOfTheWeek[k]) << "</th>";
	tos <<"</tr>\n";
	tos<<"        <tr>\n          <!-- span -->\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"xAxis\">";
			else
				tos<<"          <th>";
			tos << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
		}
	tos<<"        </tr>\n";
	tos<<"      </thead>\n";
	/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
	*/
	tos<<"      <tbody>\n";
		
	tos << "        <tr>\n";
	if(TIMETABLE_HTML_LEVEL>=2)
		tos<<"          <th class=\"yAxis\">";
	else
		tos<<"          <th>";
	tos << tr("All Activities") << "</th>\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++){
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			if(activitiesAtTime[k][j].isEmpty()){
				if((breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
					switch(TIMETABLE_HTML_LEVEL){
						case 3 : ;
						case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
						case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
						default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
					}
				} else {
					switch(TIMETABLE_HTML_LEVEL){
						case 3 : ;
						case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
						case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
						default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
					}
				}
			} else {
				if(TIMETABLE_HTML_LEVEL>=1)
					tos<<"          <td><table class=\"detailed\">";
				else
					tos<<"          <td><table>";
				if(TIMETABLE_HTML_LEVEL>=3)
					tos<<"<tr class=\"student line0\">";
				else	tos<<"<tr>";
				for(int a=0; a<activitiesAtTime[k][j].size(); a++){
					int ai=activitiesAtTime[k][j].at(a);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(ai!=UNALLOCATED_ACTIVITY){
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->studentsNames.size()>0){
							for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
									default: tos<<protect2(*st); break;
								}
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
						}
						tos<<"</td>";	
					}
				}
				tos<<"</tr>";
				if(TIMETABLE_HTML_LEVEL>=3)
					tos<<"<tr class=\"line1\">";
				else	tos<<"<tr>";
				for(int a=0; a<activitiesAtTime[k][j].size(); a++){
					int ai=activitiesAtTime[k][j].at(a);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(ai!=UNALLOCATED_ACTIVITY){
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->subjectName.size()>0||act->activityTagName.size()>0){
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->activityTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->activityTagName); break;
								}
						}
						tos<<"</td>";
					}
				}
				tos<<"</tr>";
				if(TIMETABLE_HTML_LEVEL>=3)
					tos<<"<tr class=\"teacher line2\">";
				else	tos<<"<tr>";
				for(int a=0; a<activitiesAtTime[k][j].size(); a++){
					int ai=activitiesAtTime[k][j].at(a);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(ai!=UNALLOCATED_ACTIVITY){
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->teachersNames.size()>0){
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
									default: tos<<protect2(*it); break;
								}
								if(it!=act->teachersNames.end()-1)
									tos<<", ";
							}
						}
						tos<<"</td>";
					}
				}
				tos<<"</tr>";
				if(TIMETABLE_HTML_LEVEL>=3)
					tos<<"<tr class=\"room line3\">";
				else	tos<<"<tr>";
				for(int a=0; a<activitiesAtTime[k][j].size(); a++){
					int ai=activitiesAtTime[k][j].at(a);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(ai!=UNALLOCATED_ACTIVITY){
						int r=best_solution.rooms[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
						}
						tos<<"</td>";
					}
				}
				tos<<"</tr>";
				tos<<"</table></td>\n";
			}
		}
	}
	tos<<"        </tr>\n";
	//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
	//workaround end.
	tos<<"      </tbody>\n";
	tos<<"    </table>\n";
	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}





//************************************************************************************************************************************************************************************************

// Now print the teachers

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the teachers' timetable html format to a file.
Days horizontal.
*/
void TimetableExport::writeTeachersTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n"; 
		
	tos<<"    <p><strong>"<<TimetableExport::tr("Table of content")<<"</strong></p>\n";
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
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"xAxis\">";
			else
				tos<<"          <th>";
			tos<<protect2(gt.rules.daysOfTheWeek[j])<<"</th>\n";
		}
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";
		/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		*/
		tos<<"      <tbody>\n";
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			tos<<"        <tr>\n";
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"yAxis\">";
			else
				tos<<"          <th>";
			tos<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				int ai=teachers_timetable_weekly[teacher][k][j]; //activity index
				int currentTime=k+gt.rules.nDaysPerWeek*j;
				if(ai!=UNALLOCATED_ACTIVITY){
					if(best_solution.times[ai]==currentTime){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(act->duration==1)
							tos<<"          <td>";
						else
							tos<<"          <td rowspan=\""<<act->duration<<"\">";
						if(act->studentsNames.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"student line1\">";
							for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
									default: tos<<protect2(*st); break;
								}
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						if(act->subjectName.size()>0||act->activityTagName.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"line2\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->activityTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->activityTagName); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						int r=best_solution.rooms[ai];
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"room line3\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else {
					if((teacherNotAvailableDayHour[teacher][k][j] || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
						switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
							case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
							case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
							default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
						}
					} else {
						switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
							case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
							case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
							default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
						}
					}

				}
			}
			tos<<"        </tr>\n";
		}
		//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
		//workaround end.
		tos<<"      </tbody>\n";
		if(teacher!=gt.rules.nInternalTeachers-1){
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
			tos<<"    <p>&nbsp;</p>\n\n";
		} else {
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
		}
	}
	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the teachers' timetable html format to a file.
Days vertical.
*/
void TimetableExport::writeTeachersTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1> "<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n"; 
		
	tos<<"    <p><strong>"<<TimetableExport::tr("Table of content")<<"</strong></p>\n";
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
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"xAxis\">";
			else
				tos<<"          <th>";
			tos<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
		}
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";
		/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		*/
		tos<<"      <tbody>\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			tos<<"        <tr>\n";
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"yAxis\">";
			else
				tos<<"          <th>";
			tos<<protect2(gt.rules.daysOfTheWeek[k])<<"</th>\n";
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				int ai=teachers_timetable_weekly[teacher][k][j]; //activity index
				int currentTime=k+gt.rules.nDaysPerWeek*j;
				if(ai!=UNALLOCATED_ACTIVITY){
					if(best_solution.times[ai]==currentTime){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(act->duration==1)
							tos<<"          <td>";
						else
							tos<<"          <td colspan=\""<<act->duration<<"\">";
						if(act->studentsNames.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"student line1\">";
							for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
									default: tos<<protect2(*st); break;
								}
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						if(act->subjectName.size()>0||act->activityTagName.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"line2\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->activityTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->activityTagName); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						int r=best_solution.rooms[ai];
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"room line3\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else {
					if((teacherNotAvailableDayHour[teacher][k][j] || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
						switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
							case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
							case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
							default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
						}
					} else {
						switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
							case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
							case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
							default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
						}
					}
				}
			}
			tos<<"        </tr>\n";
		}
		//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
		//workaround end.
		tos<<"      </tbody>\n";
		if(teacher!=gt.rules.nInternalTeachers-1){
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
			tos<<"    <p>&nbsp;</p>\n\n";
		} else {
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
		}
	}
	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing the teachers' timetable html format to a file
Time vertical
*/
void TimetableExport::writeTeachersTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <table border=\"1\">\n";
	tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

	tos<<"      <thead>\n        <tr><td colspan=\"2\"></td>";
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		if(TIMETABLE_HTML_LEVEL>=2)
			tos<<"          <th class=\"xAxis\">";
		else
			tos<<"          <th>";
		tos << gt.rules.internalTeachersList[i]->name << "</th>";
	}
	tos<<"</tr>\n      </thead>\n";
	/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalTeachers<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
	*/
	tos<<"      <tbody>\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++){
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			tos << "        <tr>\n";
			if(j==0)
				tos << "        <th rowspan=\"" << gt.rules.nHoursPerDay  << "\">" << protect2vert(gt.rules.daysOfTheWeek[k]) << "</th>\n";
			else tos <<"          <!-- span -->\n";
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"yAxis\">";
			else
				tos<<"          <th>";
			tos << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
			for(int i=0; i<gt.rules.nInternalTeachers; i++){
				int currentTime=k+gt.rules.nDaysPerWeek*j;
				int ai=teachers_timetable_weekly[i][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					if(best_solution.times[ai]==currentTime){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(act->duration==1)
							tos<<"          <td>";
						else
							tos<<"          <td rowspan=\""<<act->duration<<"\">";
						if(act->studentsNames.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"student line1\">";
							for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
									default: tos<<protect2(*st); break;
								}
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						if(act->subjectName.size()>0||act->activityTagName.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"line2\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->activityTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->activityTagName); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						int r=best_solution.rooms[ai];
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"room line3\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else {
					if((teacherNotAvailableDayHour[i][k][j] || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
						switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
							case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
							case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
							default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
						}
					} else {
						switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
							case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
							case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
							default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
						}
					}
				}
			}
			tos<<"        </tr>\n";
		}
	}
	//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalTeachers<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
	//workaround end.
	tos << "      </tbody>\n    </table>\n";
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the teachers' timetable html format to a file.
Time horizontal
*/
void TimetableExport::writeTeachersTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <table border=\"1\">\n";
	tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

	tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td>";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		tos << "<th colspan=\"" << gt.rules.nHoursPerDay <<"\">" << protect2(gt.rules.daysOfTheWeek[k]) << "</th>";
	tos <<"</tr>\n";
	tos<<"        <tr>\n          <!-- span -->\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"xAxis\">";
			else
				tos<<"          <th>";
			tos << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
		}
	tos<<"        </tr>\n";
	tos<<"      </thead>\n";
	/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
	*/
	tos<<"      <tbody>\n";
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		tos << "        <tr>\n";
		if(TIMETABLE_HTML_LEVEL>=2)
			tos<<"          <th class=\"yAxis\">";
		else
			tos<<"          <th>";
		tos << gt.rules.internalTeachersList[i]->name << "</th>\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				int currentTime=k+gt.rules.nDaysPerWeek*j;
				int ai=teachers_timetable_weekly[i][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					if(best_solution.times[ai]==currentTime){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(act->duration==1)
							tos<<"          <td>";
						else
							tos<<"          <td colspan=\""<<act->duration<<"\">";
						if(act->studentsNames.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"student line1\">";
							for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
									default: tos<<protect2(*st); break;
								}
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						if(act->subjectName.size()>0||act->activityTagName.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"line2\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->activityTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->activityTagName); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						int r=best_solution.rooms[ai];
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"room line3\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else {
					if((teacherNotAvailableDayHour[i][k][j] || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
						switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
							case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
							case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</span></td>\n"; break;
							default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<"</td>\n";
						}
					} else {
						switch(TIMETABLE_HTML_LEVEL){
							case 3 : ;
							case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
							case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
							default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
						}
					}
				}
			}
		}
		tos<<"        </tr>\n";
	}
	//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
	//workaround end.
	tos << "      </tbody>\n    </table>\n";
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

/**
Function writing the rooms' timetable html format to a file by Volker Dirr
Days horizontal.
*/
void TimetableExport::writeRoomsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n"; 	
	
	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<TimetableExport::tr("No rooms recorded in fet for %1.").arg(protect2(gt.rules.institutionName))<<"</h1>\n";
	else {
		tos<<"    <p><strong>"<<TimetableExport::tr("Table of content")<<"</strong></p>\n";
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
			for(int j=0; j<gt.rules.nDaysPerWeek; j++){
				if(TIMETABLE_HTML_LEVEL>=2)
					tos<<"          <th class=\"xAxis\">";
				else
					tos<<"          <th>";
				tos<<protect2(gt.rules.daysOfTheWeek[j])<<"</th>\n";
			}
			tos<<"        </tr>\n";
			tos<<"      </thead>\n";
			/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
			tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
			*/
			tos<<"      <tbody>\n";
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos<<"        <tr>\n";
				if(TIMETABLE_HTML_LEVEL>=2)
					tos<<"          <th class=\"yAxis\">";
				else
					tos<<"          <th>";
				tos<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
				for(int k=0; k<gt.rules.nDaysPerWeek; k++){
					int ai=rooms_timetable_weekly[room][k][j]; //activity index
					int currentTime=k+gt.rules.nDaysPerWeek*j;
					if(ai!=UNALLOCATED_ACTIVITY){
						if(best_solution.times[ai]==currentTime){
							Activity* act=&gt.rules.internalActivitiesList[ai];
							if(act->duration==1)
								tos<<"          <td>";
							else
								tos<<"          <td rowspan=\""<<act->duration<<"\">";
							if(act->studentsNames.size()>0){
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<div class=\"student line1\">";
								for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
									switch(TIMETABLE_HTML_LEVEL){
										case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
										case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
										default: tos<<protect2(*st); break;
									}
									if(st!=act->studentsNames.end()-1)
										tos<<", ";
								}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							if(act->teachersNames.size()>0){
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<div class=\"teacher line2\">";
								for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
									switch(TIMETABLE_HTML_LEVEL){
										case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
										case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
										default: tos<<protect2(*it); break;
									}
									if(it!=act->teachersNames.end()-1)
										tos<<", ";
								}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							if(act->subjectName.size()>0||act->activityTagName.size()>0){
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<div class=\"line3\">";
								if(act->subjectName.size()>0)
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
										case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
										case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
										default: tos<<protect2(act->subjectName); break;
									}
								if(act->activityTagName.size()>0)
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
										case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
										case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
										default: tos<<" "<<protect2(act->activityTagName); break;
									}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							tos<<"</td>\n";								
						} else
							tos<<"          <!-- span -->\n";
					} else {
						if((notAllowedRoomTimePercentages[room][k+j*gt.rules.nDaysPerWeek]>=0 || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
							QString weight="";
							if(notAllowedRoomTimePercentages[room][k+j*gt.rules.nDaysPerWeek]>=0 && 
							notAllowedRoomTimePercentages[room][k+j*gt.rules.nDaysPerWeek]<100.0 && !breakDayHour[k][j]){
								weight="<br />"+QString::number(notAllowedRoomTimePercentages[room][k+j*gt.rules.nDaysPerWeek])+" %";
							}
							weight=""; //not printing the weight
							switch(TIMETABLE_HTML_LEVEL){
								case 3 : ;
								case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<""<<weight<<"</span></td>\n"; break;
								case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<""<<weight<<"</span></td>\n"; break;
								default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<""<<weight<<"</td>\n";
							}
						} else {
							switch(TIMETABLE_HTML_LEVEL){
								case 3 : ;
								case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
								case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
								default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
							}
						}
					}
				}
				tos<<"        </tr>\n";
			}
			//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
			tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
			//workaround end.
			tos<<"      </tbody>\n";
			if(room!=gt.rules.nInternalRooms-1){
				tos<<"    </table>\n    <p>&nbsp;</p>\n";
				tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
				tos<<"    <p>&nbsp;</p>\n\n";
			} else {
				tos<<"    </table>\n    <p>&nbsp;</p>\n";
				tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
			}
		}
	}
	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

/**
Function writing the rooms' timetable html format to a file by Volker Dirr
Days vertical.
*/
void TimetableExport::writeRoomsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;

		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1> "<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n"; 
		
	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<TimetableExport::tr("No rooms recorded in fet for %1.").arg(protect2(gt.rules.institutionName))<<"</h1>\n";
	else {
		tos<<"    <p><strong>"<<TimetableExport::tr("Table of content")<<"</strong></p>\n";
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
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				if(TIMETABLE_HTML_LEVEL>=2)
					tos<<"          <th class=\"xAxis\">";
					else
						tos<<"          <th>";
					tos<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
			}
			tos<<"        </tr>\n";
			tos<<"      </thead>\n";
			/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
			tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
			*/
			tos<<"      <tbody>\n";
			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				tos<<"        <tr>\n";
				if(TIMETABLE_HTML_LEVEL>=2)
					tos<<"          <th class=\"yAxis\">";
				else
					tos<<"          <th>";
				tos<<protect2(gt.rules.daysOfTheWeek[k])<<"</th>\n";
				for(int j=0; j<gt.rules.nHoursPerDay; j++){
					int ai=rooms_timetable_weekly[room][k][j]; //activity index
					int currentTime=k+gt.rules.nDaysPerWeek*j;
					if(ai!=UNALLOCATED_ACTIVITY){
						if(best_solution.times[ai]==currentTime){
							Activity* act=&gt.rules.internalActivitiesList[ai];
							if(act->duration==1)
								tos<<"          <td>";
							else
								tos<<"          <td colspan=\""<<act->duration<<"\">";
							if(act->studentsNames.size()>0){
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<div class=\"student line1\">";
								for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
									switch(TIMETABLE_HTML_LEVEL){
										case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
										case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
										default: tos<<protect2(*st); break;
									}
									if(st!=act->studentsNames.end()-1)
										tos<<", ";
								}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							if(act->teachersNames.size()>0){
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<div class=\"teacher line2\">";
								for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
									switch(TIMETABLE_HTML_LEVEL){
										case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
										case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
										default: tos<<protect2(*it); break;
									}
									if(it!=act->teachersNames.end()-1)
										tos<<", ";
								}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							if(act->subjectName.size()>0||act->activityTagName.size()>0){
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<div class=\"line3\">";
								if(act->subjectName.size()>0)
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
										case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
										case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
										default: tos<<protect2(act->subjectName); break;
									}
								if(act->activityTagName.size()>0)
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
										case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
										case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
										default: tos<<" "<<protect2(act->activityTagName); break;
									}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							tos<<"</td>\n";	
						} else
							tos<<"          <!-- span -->\n";
					} else {
						if((notAllowedRoomTimePercentages[room][k+j*gt.rules.nDaysPerWeek]>=0 || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
							QString weight="";
							if(notAllowedRoomTimePercentages[room][k+j*gt.rules.nDaysPerWeek]>=0 && 
							 notAllowedRoomTimePercentages[room][k+j*gt.rules.nDaysPerWeek]<100.0 && !breakDayHour[k][j]){
								weight="<br />"+QString::number(notAllowedRoomTimePercentages[room][k+j*gt.rules.nDaysPerWeek])+" %";
							}
							weight=""; //not printing the weight
							switch(TIMETABLE_HTML_LEVEL){
								case 3 : ;
								case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<""<<weight<<"</span></td>\n"; break;
								case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<""<<weight<<"</span></td>\n"; break;
								default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<""<<weight<<"</td>\n";
							}
						} else {
							switch(TIMETABLE_HTML_LEVEL){
								case 3 : ;
								case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
								case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
								default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
							}
						}
					}
				}
				tos<<"        </tr>\n";
			}
			//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
			tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
			//workaround end.
			tos<<"      </tbody>\n";
			if(room!=gt.rules.nInternalRooms-1){
				tos<<"    </table>\n    <p>&nbsp;</p>\n";
				tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
				tos<<"    <p>&nbsp;</p>\n\n";
			} else {
				tos<<"    </table>\n    <p>&nbsp;</p>\n";
				tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
			}
		}
	}
	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}


/**
Function writing the rooms' timetable html format to a file by Volker Dirr
Time vertical.
*/
void TimetableExport::writeRoomsTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";
	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<TimetableExport::tr("No rooms recorded in fet for %1.").arg(protect2(gt.rules.institutionName))<<"</h1>\n";
	else {
		tos<<"    <table border=\"1\">\n";
		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		tos<<"      <thead>\n        <tr><td colspan=\"2\"></td>";
		for(int i=0; i<gt.rules.nInternalRooms; i++){
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"xAxis\">";
			else
				tos<<"          <th>";
			tos << gt.rules.internalRoomsList[i]->name << "</th>";
		}
		tos<<"</tr>\n      </thead>\n";
		/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalRooms<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		*/
		tos<<"      <tbody>\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos << "        <tr>\n";
				if(j==0)
					tos << "        <th rowspan=\"" << gt.rules.nHoursPerDay  << "\">" << protect2vert(gt.rules.daysOfTheWeek[k]) << "</th>\n";
				else tos <<"          <!-- span -->\n";
				if(TIMETABLE_HTML_LEVEL>=2)
					tos<<"          <th class=\"yAxis\">";
				else
					tos<<"          <th>";
				tos << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
				for(int i=0; i<gt.rules.nInternalRooms; i++){
					int currentTime=k+gt.rules.nDaysPerWeek*j;
					int ai=rooms_timetable_weekly[i][k][j]; //activity index
					if(ai!=UNALLOCATED_ACTIVITY){
						if(best_solution.times[ai]==currentTime){
							Activity* act=&gt.rules.internalActivitiesList[ai];
							if(act->duration==1)
								tos<<"          <td>";
							else
								tos<<"          <td rowspan=\""<<act->duration<<"\">";
							if(act->studentsNames.size()>0){
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<div class=\"student line1\">";
								for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
									switch(TIMETABLE_HTML_LEVEL){
										case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
										case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
										default: tos<<protect2(*st); break;
									}
									if(st!=act->studentsNames.end()-1)
										tos<<", ";
								}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							if(act->teachersNames.size()>0){
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<div class=\"teacher line2\">";
								for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
									switch(TIMETABLE_HTML_LEVEL){
										case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
										case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
										default: tos<<protect2(*it); break;
									}
									if(it!=act->teachersNames.end()-1)
										tos<<", ";
								}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							if(act->subjectName.size()>0||act->activityTagName.size()>0){
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<div class=\"line3\">";
								if(act->subjectName.size()>0)
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
										case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
										case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
										default: tos<<protect2(act->subjectName); break;
									}
								if(act->activityTagName.size()>0)
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
										case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
										case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
										default: tos<<" "<<protect2(act->activityTagName); break;
									}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							tos<<"</td>\n";	
						} else
							tos<<"          <!-- span -->\n";
					} else {
						if((notAllowedRoomTimePercentages[i][k+j*gt.rules.nDaysPerWeek]>=0 || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
							QString weight="";
							if(notAllowedRoomTimePercentages[i][k+j*gt.rules.nDaysPerWeek]>=0 && 
							 notAllowedRoomTimePercentages[i][k+j*gt.rules.nDaysPerWeek]<100.0 && !breakDayHour[k][j]){
								weight="<br />"+QString::number(notAllowedRoomTimePercentages[i][k+j*gt.rules.nDaysPerWeek])+" %";
							}
							weight=""; //not printing the weight
							switch(TIMETABLE_HTML_LEVEL){
								case 3 : ;
								case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<""<<weight<<"</span></td>\n"; break;
								case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<""<<weight<<"</span></td>\n"; break;
								default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<""<<weight<<"</td>\n";
							}
						} else {
							switch(TIMETABLE_HTML_LEVEL){
								case 3 : ;
								case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
								case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
								default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
							}
						}
					}
				}
				tos<<"        </tr>\n";
			}
		}
		//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalRooms<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
		//workaround end.
		tos << "      </tbody>\n    </table>\n";
	}
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}


/**
Function writing the rooms' timetable html format to a file by Volker Dirr
Time vertical.
*/
void TimetableExport::writeRoomsTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<TimetableExport::tr("No rooms recorded in fet for %1.").arg(protect2(gt.rules.institutionName))<<"</h1>\n";
	else {
		tos<<"    <table border=\"1\">\n";
		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td>";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++)
			tos << "<th colspan=\"" << gt.rules.nHoursPerDay <<"\">" << protect2(gt.rules.daysOfTheWeek[k]) << "</th>";
		tos <<"</tr>\n";
		tos<<"        <tr>\n          <!-- span -->\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++)
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				if(TIMETABLE_HTML_LEVEL>=2)
					tos<<"          <th class=\"xAxis\">";
				else
					tos<<"          <th>";
				tos << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
			}
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";
		/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		*/
		tos<<"      <tbody>\n";
		for(int i=0; i<gt.rules.nInternalRooms; i++){
			tos << "        <tr>\n";
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"yAxis\">";
			else
				tos<<"          <th>";
			tos << gt.rules.internalRoomsList[i]->name << "</th>\n";
			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				for(int j=0; j<gt.rules.nHoursPerDay; j++){
					int currentTime=k+gt.rules.nDaysPerWeek*j;
					int ai=rooms_timetable_weekly[i][k][j]; //activity index
					if(ai!=UNALLOCATED_ACTIVITY){
						if(best_solution.times[ai]==currentTime){
							Activity* act=&gt.rules.internalActivitiesList[ai];
							if(act->duration==1)
								tos<<"          <td>";
							else
								tos<<"          <td colspan=\""<<act->duration<<"\">";
							if(act->studentsNames.size()>0){
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<div class=\"student line1\">";
								for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
									switch(TIMETABLE_HTML_LEVEL){
										case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
										case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
										default: tos<<protect2(*st); break;
									}
									if(st!=act->studentsNames.end()-1)
										tos<<", ";
								}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							if(act->teachersNames.size()>0){
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<div class=\"teacher line2\">";
								for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
									switch(TIMETABLE_HTML_LEVEL){
										case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
										case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
										default: tos<<protect2(*it); break;
									}
									if(it!=act->teachersNames.end()-1)
										tos<<", ";
								}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							if(act->subjectName.size()>0||act->activityTagName.size()>0){
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<div class=\"line3\">";
								if(act->subjectName.size()>0)
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
										case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
										case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2java(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
										default: tos<<protect2(act->subjectName); break;
									}
								if(act->activityTagName.size()>0)
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : tos<<" <span class=\"activitytag\">"<<protect2(act->activityTagName)<<"</span>"; break;
										case 4 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
										case 5 : tos<<" <span class=\"activitytag\"><span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span></span>"; break;
										default: tos<<" "<<protect2(act->activityTagName); break;
									}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							tos<<"</td>\n";	
						} else
							tos<<"          <!-- span -->\n";
					} else {
						if((notAllowedRoomTimePercentages[i][k+j*gt.rules.nDaysPerWeek]>=0 || breakDayHour[k][j]) && PRINT_NOT_AVAILABLE_TIME_SLOTS){
							QString weight="";
							if(notAllowedRoomTimePercentages[i][k+j*gt.rules.nDaysPerWeek]>=0 && 
							 notAllowedRoomTimePercentages[i][k+j*gt.rules.nDaysPerWeek]<100.0 && !breakDayHour[k][j]){
								weight="<br />"+QString::number(notAllowedRoomTimePercentages[i][k+j*gt.rules.nDaysPerWeek])+" %";
							}
							weight=""; //not printing the weight
							switch(TIMETABLE_HTML_LEVEL){
								case 3 : ;
								case 4 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<""<<weight<<"</span></td>\n"; break;
								case 5 : tos<<"          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<""<<weight<<"</span></td>\n"; break;
								default: tos<<"          <td>"<<protect2(STRING_NOT_AVAILABLE_SLOT)<<""<<weight<<"</td>\n";
							}
						} else {
							switch(TIMETABLE_HTML_LEVEL){
								case 3 : ;
								case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
								case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
								default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
							}
						}
					}
				}
			}
			tos<<"        </tr>\n";
		}
		//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
		//workaround end.
		tos << "      </tbody>\n    </table>\n";
	}
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}


//Now print the subjects

//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing the subjects' timetable in html format to a file.
Days horizontal
*/
void TimetableExport::writeSubjectsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n"; 

	tos<<"    <p><strong>"<<TimetableExport::tr("Table of content")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		tos<<"      <li>\n        "<<TimetableExport::tr("Subject");
		tos<<" <a href=\""<<"#table_"<<protect2id(gt.rules.subjectsList[i]->name)<<"\">"<<gt.rules.subjectsList[i]->name<<"</a>\n";
		tos<<"      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";


	for(int subject=0; subject<gt.rules.subjectsList.size(); subject++){
		///////Liviu here
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				activitiesForCurrentSubject[d][h].clear();
		foreach(int ai, gt.rules.activitiesForSubject[subject])
			if(best_solution.times[ai]!=UNALLOCATED_TIME){
				int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
				int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
				Activity* act=&gt.rules.internalActivitiesList[ai];
				for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
					activitiesForCurrentSubject[d][h+dd].append(ai);
			}
		///////end Liviu
	
		tos<<"    <table id=\"table_"<<protect2id(gt.rules.subjectsList[subject]->name);
		tos<<"\" border=\"1\"";
		if(subject%2==0) tos<<" class=\"modulo2\"";
		tos<<">\n";

		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<protect2(gt.rules.subjectsList[subject]->name)<<"</th></tr>\n";
		tos<<"        <tr>\n          <!-- span -->\n";
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"xAxis\">";
			else
				tos<<"          <th>";
			tos<<protect2(gt.rules.daysOfTheWeek[j])<<"</th>\n";
		}
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";
		/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		*/
		tos<<"      <tbody>\n";
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			tos<<"        <tr>\n";
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"yAxis\">";
			else
				tos<<"          <th>";
			tos<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				QList<qint16> allActivities;
				
				//Liviu:
				allActivities=activitiesForCurrentSubject[k][j];
				
				/*
				allActivities.clear();
				//Now get the activitiy ids. I don't run through the InternalActivitiesList, even that is faster. I run through subgroupsList, because by that the activites are sorted by that in the html-table.
				for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
					if(students_timetable_weekly[subgroup][k][j]!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[students_timetable_weekly[subgroup][k][j]];
						if(act->subjectName==gt.rules.subjectsList[subject]->name)
							if(!(allActivities.contains(students_timetable_weekly[subgroup][k][j]))){
								allActivities<<students_timetable_weekly[subgroup][k][j];
							}
					}
				}
				//Now run through the teachers timetable, because activities without a students set are still missing.
				for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
					if(teachers_timetable_weekly[teacher][k][j]!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[teachers_timetable_weekly[teacher][k][j]];
						if(act->subjectName==gt.rules.subjectsList[subject]->name)
							if(!(allActivities.contains(teachers_timetable_weekly[teacher][k][j]))){
								assert(act->studentsNames.isEmpty());
								allActivities<<teachers_timetable_weekly[teacher][k][j];
							}
					}
				}*/
				if(allActivities.isEmpty()){
					switch(TIMETABLE_HTML_LEVEL){
						case 3 : ;
						case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
						case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
						default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
					}
				} else {
					int ai=allActivities[0]; //activity index
					if(TIMETABLE_HTML_LEVEL>=1)
						tos<<"          <td><table class=\"detailed\">";
					else
						tos<<"          <td><table>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"line0 activitytag\">";
					else	tos<<"<tr>";
					for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->activityTagName.size()>0)
							switch(TIMETABLE_HTML_LEVEL){
								case 4 : tos<<"<span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span>"; break;
								case 5 : tos<<"<span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span>"; break;
								default: tos<<protect2(act->activityTagName); break;
							}
						tos<<"</td>";
					}
					tos<<"</tr>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"student line1\">";
					else	tos<<"<tr>";
					for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->studentsNames.size()>0){
							for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
									default: tos<<protect2(*st); break;
								}
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
						}
						tos<<"</td>";	
					}
					tos<<"</tr>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"teacher line2\">";
						else	tos<<"<tr>";
						for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->teachersNames.size()>0){
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
									default: tos<<protect2(*it); break;
								}
								if(it!=act->teachersNames.end()-1)
									tos<<", ";
							}
						}
						tos<<"</td>";
					}
					tos<<"</tr>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"room line3\">";
					else	tos<<"<tr>";
					for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						int r=best_solution.rooms[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
						}
						tos<<"</td>";
					}
					tos<<"</tr>";
					tos<<"</table></td>\n";
				}
			}
			tos<<"        </tr>\n";
		}
		//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
		//workaround end.
		tos<<"      </tbody>\n";
		if((subject!=gt.rules.subjectsList.size()-1)){
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
			tos<<"    <p>&nbsp;</p>\n\n";
		} else {
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
		}
	}
	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}


//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing the subjects' timetable in html format to a file.
Days vertical
*/
void TimetableExport::writeSubjectsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n"; 

	tos<<"    <p><strong>"<<TimetableExport::tr("Table of content")<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		tos<<"      <li>\n        "<<TimetableExport::tr("Subject");
		tos<<" <a href=\""<<"#table_"<<protect2id(gt.rules.subjectsList[i]->name)<<"\">"<<gt.rules.subjectsList[i]->name<<"</a>\n";
		tos<<"      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	for(int subject=0; subject<gt.rules.subjectsList.size(); subject++){
		///////Liviu here
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				activitiesForCurrentSubject[d][h].clear();
		foreach(int ai, gt.rules.activitiesForSubject[subject])
			if(best_solution.times[ai]!=UNALLOCATED_TIME){
				int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
				int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
				Activity* act=&gt.rules.internalActivitiesList[ai];
				for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
					activitiesForCurrentSubject[d][h+dd].append(ai);
			}
		///////end Liviu

		tos<<"    <table id=\"table_"<<protect2id(gt.rules.subjectsList[subject]->name);
		tos<<"\" border=\"1\"";
		if(subject%2==0) tos<<" class=\"modulo2\"";
		tos<<">\n";

		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<protect2(gt.rules.subjectsList[subject]->name)<<"</th></tr>\n";
		tos<<"        <tr>\n          <!-- span -->\n";
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"xAxis\">";
			else
				tos<<"          <th>";
			tos<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
		}
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";
		/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		*/
		tos<<"      <tbody>\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			tos<<"        <tr>\n";
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"yAxis\">";
			else
				tos<<"          <th>";
			tos<<protect2(gt.rules.daysOfTheWeek[k])<<"</th>\n";
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				QList<qint16> allActivities;

				//Liviu:
				allActivities=activitiesForCurrentSubject[k][j];

				/*
				allActivities.clear();
				//Now get the activitiy ids. I don't run through the InternalActivitiesList, even that is faster. I run through subgroupsList, because by that the activites are sorted by that in the html-table.
				for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
					if(students_timetable_weekly[subgroup][k][j]!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[students_timetable_weekly[subgroup][k][j]];
						if(act->subjectName==gt.rules.subjectsList[subject]->name)
							if(!(allActivities.contains(students_timetable_weekly[subgroup][k][j]))){
								allActivities<<students_timetable_weekly[subgroup][k][j];
							}
					}
				}
				//Now run through the teachers timetable, because activities without a students set are still missing.
				for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
					if(teachers_timetable_weekly[teacher][k][j]!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[teachers_timetable_weekly[teacher][k][j]];
						if(act->subjectName==gt.rules.subjectsList[subject]->name)
							if(!(allActivities.contains(teachers_timetable_weekly[teacher][k][j]))){
								assert(act->studentsNames.isEmpty());
								allActivities<<teachers_timetable_weekly[teacher][k][j];
							}
					}
				}
				*/
				
				if(allActivities.isEmpty()){
					switch(TIMETABLE_HTML_LEVEL){
						case 3 : ;
						case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
						case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
						default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
					}
				} else {
					int ai=allActivities[0]; //activity index
					if(TIMETABLE_HTML_LEVEL>=1)
						tos<<"          <td><table class=\"detailed\">";
					else
						tos<<"          <td><table>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"line0 activitytag\">";
					else	tos<<"<tr>";
					for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->activityTagName.size()>0)
							switch(TIMETABLE_HTML_LEVEL){
								case 4 : tos<<"<span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span>"; break;
								case 5 : tos<<"<span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span>"; break;
								default: tos<<protect2(act->activityTagName); break;
							}
						tos<<"</td>";
					}
					tos<<"</tr>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"student line1\">";
					else	tos<<"<tr>";
					for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->studentsNames.size()>0){
							for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
									default: tos<<protect2(*st); break;
								}
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
						}
						tos<<"</td>";	
					}
					tos<<"</tr>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"teacher line2\">";
						else	tos<<"<tr>";
						for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->teachersNames.size()>0){
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
									default: tos<<protect2(*it); break;
								}
								if(it!=act->teachersNames.end()-1)
									tos<<", ";
							}
						}
						tos<<"</td>";
					}
					tos<<"</tr>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"room line3\">";
					else	tos<<"<tr>";
					for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						int r=best_solution.rooms[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
						}
						tos<<"</td>";
					}
					tos<<"</tr>";
					tos<<"</table></td>\n";
				}
			}
		tos<<"        </tr>\n";
		}
		//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
		//workaround end.
		tos<<"      </tbody>\n";
		if((subject!=gt.rules.subjectsList.size()-1)){
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
			tos<<"    <p>&nbsp;</p>\n\n";
		} else {
			tos<<"    </table>\n    <p>&nbsp;</p>\n";
			tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
		}
	}
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}


//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing the subjects' timetable html format to a file
Time vertical
*/

void TimetableExport::writeSubjectsTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";


	tos<<"    <table id=\"table_"<<protect2id(gt.rules.institutionName)<<"\" border=\"1\">\n";

	tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

	tos<<"      <thead>\n        <tr><td colspan=\"2\"></td>";
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"xAxis\">";
			else
				tos<<"          <th>";
			tos << gt.rules.subjectsList[i]->name << "</th>";
	}
		
	tos<<"</tr>\n      </thead>\n";
	/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.subjectsList.size()<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
	*/
	tos<<"      <tbody>\n";


	//LIVIU										//maybe TODO: write a function for this
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		for(int j=0; j<gt.rules.nHoursPerDay; j++)
			activitiesAtTime[k][j].clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++) {
		Activity* act=&gt.rules.internalActivitiesList[i];
		if(best_solution.times[i]!=UNALLOCATED_TIME) {
			int hour=best_solution.times[i]/gt.rules.nDaysPerWeek;
			int day=best_solution.times[i]%gt.rules.nDaysPerWeek;
			for(int dd=0; dd < act->duration && hour+dd < gt.rules.nHoursPerDay; dd++)
				activitiesAtTime[day][hour+dd].append(i);
		}
	}
	///////


	for(int k=0; k<gt.rules.nDaysPerWeek; k++){
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			tos << "        <tr>\n";
			if(j==0)
				tos << "        <th rowspan=\"" << gt.rules.nHoursPerDay  << "\">" << protect2vert(gt.rules.daysOfTheWeek[k]) << "</th>\n";
			else tos <<"          <!-- span -->\n";
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"yAxis\">";
			else
				tos<<"          <th>";
			tos << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";


			for(int subject=0; subject<gt.rules.subjectsList.size(); subject++){
				QList<qint16> allActivities;
				allActivities.clear();
				
				foreach(int ai, gt.rules.activitiesForSubject[subject])
					if(activitiesAtTime[k][j].contains(ai)){
						assert(!allActivities.contains(ai));
						allActivities.append(ai);
					}
				
				/*//Now get the activitiy ids. I don't run through the InternalActivitiesList, even that is faster. I run through subgroupsList, because by that the activites are sorted by that in the html-table.
				for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
					if(students_timetable_weekly[subgroup][k][j]!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[students_timetable_weekly[subgroup][k][j]];
						if(act->subjectName==gt.rules.subjectsList[subject]->name)
							if(!(allActivities.contains(students_timetable_weekly[subgroup][k][j]))){
								allActivities<<students_timetable_weekly[subgroup][k][j];
							}
					}
				}
				//Now run through the teachers timetable, because activities without a students set are still missing.
				for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
					if(teachers_timetable_weekly[teacher][k][j]!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[teachers_timetable_weekly[teacher][k][j]];
						if(act->subjectName==gt.rules.subjectsList[subject]->name)
							if(!(allActivities.contains(teachers_timetable_weekly[teacher][k][j]))){
								assert(act->studentsNames.isEmpty());
								allActivities<<teachers_timetable_weekly[teacher][k][j];
							}
					}
				}*/
				if(allActivities.isEmpty()){
					switch(TIMETABLE_HTML_LEVEL){
						case 3 : ;
						case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
						case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
						default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
					}
				} else {
					int ai=allActivities[0]; //activity index
					if(TIMETABLE_HTML_LEVEL>=1)
						tos<<"          <td><table class=\"detailed\">";
					else
						tos<<"          <td><table>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"line0 activitytag\">";
					else	tos<<"<tr>";
					for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->activityTagName.size()>0)
							switch(TIMETABLE_HTML_LEVEL){
								case 4 : tos<<"<span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span>"; break;
								case 5 : tos<<"<span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span>"; break;
								default: tos<<protect2(act->activityTagName); break;
							}
						tos<<"</td>";
					}
					tos<<"</tr>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"student line1\">";
					else	tos<<"<tr>";
					for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->studentsNames.size()>0){
							for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
									default: tos<<protect2(*st); break;
								}
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
						}
						tos<<"</td>";	
					}
					tos<<"</tr>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"teacher line2\">";
						else	tos<<"<tr>";
						for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->teachersNames.size()>0){
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
									default: tos<<protect2(*it); break;
								}
								if(it!=act->teachersNames.end()-1)
									tos<<", ";
							}
						}
						tos<<"</td>";
					}
					tos<<"</tr>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"room line3\">";
					else	tos<<"<tr>";
					for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						int r=best_solution.rooms[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
						}
						tos<<"</td>";
					}
					tos<<"</tr>";
					tos<<"</table></td>\n";
				}
			}
			tos<<"        </tr>\n";
		}
	}
	//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""<<gt.rules.subjectsList.size()<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
	//workaround end.
	tos<<"      </tbody>\n    </table>\n";
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing the subjects' timetable in html format to a file.
Time horizontal
*/
void TimetableExport::writeSubjectsTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body>\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";
	tos<<"    <table id=\"table_"<<protect2id(gt.rules.institutionName)<<"\" border=\"1\">\n";

	tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

	tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td>";

	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		tos << "<th colspan=\"" << gt.rules.nHoursPerDay <<"\">" << protect2(gt.rules.daysOfTheWeek[k]) << "</th>";
	tos <<"</tr>\n";
	tos<<"        <tr>\n          <!-- span -->\n";
	for(int k=0; k<gt.rules.nDaysPerWeek; k++)
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"xAxis\">";
			else
				tos<<"          <th>";
			tos << protect2(gt.rules.hoursOfTheDay[j]) << "</th>\n";
		}
	tos<<"        </tr>\n";
	tos<<"      </thead>\n";
	/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalSubgroups<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
	*/
	tos<<"      <tbody>\n";
	for(int subject=0; subject<gt.rules.subjectsList.size(); subject++){
		tos << "        <tr>\n";
		if(TIMETABLE_HTML_LEVEL>=2)
			tos << "        <th class=\"yAxis\">" << protect2(gt.rules.subjectsList[subject]->name) << "</th>\n";
		else
			tos << "        <th>" << protect2(gt.rules.subjectsList[subject]->name) << "</th>\n";

		///////Liviu here
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				activitiesForCurrentSubject[d][h].clear();
		foreach(int ai, gt.rules.activitiesForSubject[subject])
			if(best_solution.times[ai]!=UNALLOCATED_TIME){
				int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
				int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
				Activity* act=&gt.rules.internalActivitiesList[ai];
				for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
					activitiesForCurrentSubject[d][h+dd].append(ai);
			}
		///////end Liviu

		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				QList<qint16> allActivities;

				allActivities=activitiesForCurrentSubject[k][j];


				/*allActivities.clear();
				//Now get the activitiy ids. I don't run through the InternalActivitiesList, even that is faster. I run through subgroupsList, because by that the activites are sorted by that in the html-table.
				for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
					if(students_timetable_weekly[subgroup][k][j]!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[students_timetable_weekly[subgroup][k][j]];
						if(act->subjectName==gt.rules.subjectsList[subject]->name)
							if(!(allActivities.contains(students_timetable_weekly[subgroup][k][j]))){
								allActivities<<students_timetable_weekly[subgroup][k][j];
							}
					}
				}
				//Now run through the teachers timetable, because activities without a students set are still missing.
				for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
					if(teachers_timetable_weekly[teacher][k][j]!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[teachers_timetable_weekly[teacher][k][j]];
						if(act->subjectName==gt.rules.subjectsList[subject]->name)
							if(!(allActivities.contains(teachers_timetable_weekly[teacher][k][j]))){
								assert(act->studentsNames.isEmpty());
								allActivities<<teachers_timetable_weekly[teacher][k][j];
							}
					}
				}*/
				if(allActivities.isEmpty()){
					switch(TIMETABLE_HTML_LEVEL){
						case 3 : ;
						case 4 : tos<<"          <td class=\"empty\"><span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
						case 5 : tos<<"          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span></td>\n"; break;
						default: tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
					}
				} else {
					int ai=allActivities[0]; //activity index
					if(TIMETABLE_HTML_LEVEL>=1)
						tos<<"          <td><table class=\"detailed\">";
					else
						tos<<"          <td><table>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"line0 activitytag\">";
					else	tos<<"<tr>";
					for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->activityTagName.size()>0)
							switch(TIMETABLE_HTML_LEVEL){
								case 4 : tos<<"<span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\">"<<protect2(act->activityTagName)<<"</span>"; break;
								case 5 : tos<<"<span class=\"activitytag_"<<protect2id(act->activityTagName)<<"\" onmouseover=\"highlight('activitytag_"<<protect2java(act->activityTagName)<<"')\">"<<protect2(act->activityTagName)<<"</span>"; break;
								default: tos<<protect2(act->activityTagName); break;
							}
						tos<<"</td>";
					}
					tos<<"</tr>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"student line1\">";
					else	tos<<"<tr>";
					for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->studentsNames.size()>0){
							for(QStringList::Iterator st=act->studentsNames.begin(); st!=act->studentsNames.end(); st++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\">"<<protect2(*st)<<"</span>"; break;
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2java(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
									default: tos<<protect2(*st); break;
								}
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
						}
						tos<<"</td>";	
					}
					tos<<"</tr>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"teacher line2\">";
						else	tos<<"<tr>";
						for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->teachersNames.size()>0){
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\">"<<protect2(*it)<<"</span>"; break;
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
									default: tos<<protect2(*it); break;
								}
								if(it!=act->teachersNames.end()-1)
									tos<<", ";
							}
						}
						tos<<"</td>";
					}
					tos<<"</tr>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"room line3\">";
					else	tos<<"<tr>";
					for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						int r=best_solution.rooms[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2java(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
						}
						tos<<"</td>";
					}
					tos<<"</tr>";
					tos<<"</table></td>\n";
				}
			}
		}
		tos<<"        </tr>\n";
	}
	//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
	tos<<"      <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
	//workaround end.
	tos<<"      </tbody>\n    </table>\n";
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

// Now print the teachers free periods. Code by Volker Dirr (http://timetabling.de/)
/**
Function writing the teachers' free periods timetable html format to a file.
Days horizontal.
*/
void TimetableExport::writeTeachersFreePeriodsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1>"<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n"; 

	tos<<"    <ul>\n";
	tos<<"      <li><a href=\""<<"#table_DETAILED\">"<<TimetableExport::tr("Teachers' Free Periods")<<" ("<<tr("Detailed")<<")</a></li>\n";
	tos<<"      <li><a href=\""<<"#table_LESS_DETAILED\">"<<TimetableExport::tr("Teachers' Free Periods")<<" ("<<tr("Less detailed")<<")</a></li>\n";
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	tos<<"    <div class=\"TEACHER_HAS_SINGLE_GAP\">"<<TimetableExport::tr("Teacher has a single gap")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_BORDER_GAP\">"<<TimetableExport::tr("Teacher has a border gap")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_BIG_GAP\">"<<TimetableExport::tr("Teacher has a big gap")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_COME_EARLIER\">"<<TimetableExport::tr("Teacher must come earlier")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_COME_MUCH_EARLIER\">"<<TimetableExport::tr("Teacher must come much earlier")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_STAY_LONGER\">"<<TimetableExport::tr("Teacher must stay longer")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_STAY_MUCH_LONGER\">"<<TimetableExport::tr("Teacher must stay much longer")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_A_FREE_DAY\">"<<TimetableExport::tr("Teacher has a free day")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_IS_NOT_AVAILABLE\">"<<TimetableExport::tr("Teacher is not available")<<"</div>\n";

	tos<<"    <p>&nbsp;</p>\n\n";
	
	bool PRINT_DETAILED=true;
	do{
		if(PRINT_DETAILED==true)
			tos<<"    <table id=\"table_DETAILED\" border=\"1\">\n";
		else
			tos<<"    <table id=\"table_LESS_DETAILED\" border=\"1\">\n";
		
		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		if(PRINT_DETAILED==true)
			tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Teachers' Free Periods")<<" ("<<tr("Detailed")<<")</th></tr>\n";
		else	tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Teachers' Free Periods")<<" ("<<tr("Less detailed")<<")</th></tr>\n";

		tos<<"        <tr>\n          <!-- span -->\n";
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"xAxis\">";
			else
				tos<<"          <th>";
			tos<<protect2(gt.rules.daysOfTheWeek[j])<<"</th>\n";
		}
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";
		/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		*/
		tos<<"      <tbody>\n";
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			tos<<"        <tr>\n";
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"yAxis\">";
			else
				tos<<"          <th>";
			tos<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				tos<<"          <td>";
				bool empty_slot=true;
				for(int tfp=0; tfp<TEACHERS_FREE_PERIODS_N_CATEGORIES; tfp++){
					if(teachers_free_periods_timetable_weekly[tfp][k][j].size()>0){
						empty_slot=false;
						if(TIMETABLE_HTML_LEVEL>=2)
							tos<<"<div class=\"DESCRIPTION\">";
						switch(tfp){
							case TEACHER_HAS_SINGLE_GAP		: tos<<TimetableExport::tr("Single gap"); break;
							case TEACHER_HAS_BORDER_GAP		: tos<<TimetableExport::tr("Border gap"); break;
							case TEACHER_HAS_BIG_GAP		: tos<<TimetableExport::tr("Big gap"); break;
							case TEACHER_MUST_COME_EARLIER		: tos<<TimetableExport::tr("Must come earlier"); break;
							case TEACHER_MUST_STAY_LONGER		: tos<<TimetableExport::tr("Must stay longer"); break;
							case TEACHER_MUST_COME_MUCH_EARLIER	: tos<<TimetableExport::tr("Must come much earlier"); break;
							case TEACHER_MUST_STAY_MUCH_LONGER	: tos<<TimetableExport::tr("Must stay much longer"); break;
							case TEACHER_HAS_A_FREE_DAY		: tos<<TimetableExport::tr("Free day"); break;
							case TEACHER_IS_NOT_AVAILABLE		: tos<<TimetableExport::tr("Not available"); break;
							default: assert(0==1); break;
						}
						if(TIMETABLE_HTML_LEVEL>=2)
							tos<<":</div>";
						else tos<<":<br />";
						if(TIMETABLE_HTML_LEVEL>=3)
							switch(tfp){
								case TEACHER_HAS_SINGLE_GAP		: tos<<"<div class=\"TEACHER_HAS_SINGLE_GAP\">"; break;
								case TEACHER_HAS_BORDER_GAP		: tos<<"<div class=\"TEACHER_HAS_BORDER_GAP\">"; break;
								case TEACHER_HAS_BIG_GAP		: tos<<"<div class=\"TEACHER_HAS_BIG_GAP\">"; break;
								case TEACHER_MUST_COME_EARLIER		: tos<<"<div class=\"TEACHER_MUST_COME_EARLIER\">"; break;
								case TEACHER_MUST_STAY_LONGER		: tos<<"<div class=\"TEACHER_MUST_STAY_LONGER\">"; break;
								case TEACHER_MUST_COME_MUCH_EARLIER	: tos<<"<div class=\"TEACHER_MUST_COME_MUCH_EARLIER\">"; break;
								case TEACHER_MUST_STAY_MUCH_LONGER	: tos<<"<div class=\"TEACHER_MUST_STAY_MUCH_LONGER\">"; break;
								case TEACHER_HAS_A_FREE_DAY		: tos<<"<div class=\"TEACHER_HAS_A_FREE_DAY\">"; break;
								case TEACHER_IS_NOT_AVAILABLE		: tos<<"<div class=\"TEACHER_IS_NOT_AVAILABLE\">"; break;
								default: assert(0==1); break;
							}
						for(int t=0; t<teachers_free_periods_timetable_weekly[tfp][k][j].size(); t++){
							QString teacher_name = gt.rules.internalTeachersList[teachers_free_periods_timetable_weekly[tfp][k][j].at(t)]->name;
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"teacher_"<<protect2id(teacher_name)<<"\">"<<protect2(teacher_name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(teacher_name)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(teacher_name)<<"')\">"<<protect2(teacher_name)<<"</span>"; break;
									default: tos<<protect2(teacher_name); break;
								}
							tos<<"<br />";
						}
						if(TIMETABLE_HTML_LEVEL>=3)
							tos<<"</div>";
					}
					if(PRINT_DETAILED==false&&tfp>=TEACHER_MUST_COME_EARLIER) tfp=TEACHER_IS_NOT_AVAILABLE+1;
				}
				if(empty_slot){
					switch(TIMETABLE_HTML_LEVEL){
						case 3 : ;
						case 4 : tos<<"<span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span>"; break;
						case 5 : tos<<"<span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span>"; break;
						default: tos<<protect2(STRING_EMPTY_SLOT);
					}
				}
				tos<<"</td>\n";
			}
			tos<<"        </tr>\n";
		}
		//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"        <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
		//workaround end.
		tos<<"      </tbody>\n    </table>\n    <p>&nbsp;</p>\n";
		tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
		tos<<"    <p>&nbsp;</p>\n\n";
		if(PRINT_DETAILED==true) PRINT_DETAILED=false;
		else PRINT_DETAILED=true;
	} while(PRINT_DETAILED!=true);
	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (http://timetabling.de/)
/**
Function writing the teachers' free periods timetable html format to a file.
Days vertical.
*/
void TimetableExport::writeTeachersFreePeriodsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);

	tos<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tos<<"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\n";

	if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false)
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\">\n";
	else
		tos<<"<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""<<LANGUAGE_FOR_HTML<<"\" xml:lang=\""<<LANGUAGE_FOR_HTML<<"\" dir=\"rtl\">\n";
	tos<<"  <head>\n";
	tos<<"    <title>"<<protect2(gt.rules.institutionName)<<"</title>\n";
	tos<<"    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1)+"_"+STYLESHEET_CSS;
		tos<<"    <link rel=\"stylesheet\" media=\"all\" href=\""<<cssfilename<<"\" type=\"text/css\" />\n";
	}
	if(TIMETABLE_HTML_LEVEL>=5){  // the following JavaScript code is pretty similar to an example of Les Richardson ( http://richtech.ca/openadmin/index.html )
		tos<<"    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
		tos<<"    <script type=\"text/javascript\">\n";
		tos<<"      function highlight(classval) {\n";
		tos<<"        var spans = document.getElementsByTagName('span');\n";
		tos<<"        for(var i=0;spans.length>i;i++) {\n";
		tos<<"          if (spans[i].className == classval) {\n";
		tos<<"            spans[i].style.backgroundColor = 'lime';\n";
		tos<<"          } else {\n";
		tos<<"            spans[i].style.backgroundColor = 'white';\n";
		tos<<"          }\n";
		tos<<"        }\n";
		tos<<"      }\n";
		tos<<"    </script>\n";
	}
	tos<<"  </head>\n\n";

	tos<<"  <body id=\"top\">\n";

	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"    <h1> "<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"</h1>\n";

	tos<<"    <p>\n      <strong>"<<TimetableExport::tr("Institution name")<<":</strong> "<<protect2(gt.rules.institutionName)<<"<br />\n";
	tos<<"      <strong>"<<TimetableExport::tr("Comments")<<":</strong> "<<protect2(gt.rules.comments)<<"\n    </p>\n"; 

	tos<<"    <ul>\n";
	tos<<"      <li><a href=\""<<"#table_DETAILED\">"<<TimetableExport::tr("Teachers' Free Periods")<<" ("<<tr("Detailed")<<")</a></li>\n";
	tos<<"      <li><a href=\""<<"#table_LESS_DETAILED\">"<<TimetableExport::tr("Teachers' Free Periods")<<" ("<<tr("Less detailed")<<")</a></li>\n";
	tos<<"    </ul>\n    <p>&nbsp;</p>\n\n";

	tos<<"    <div class=\"TEACHER_HAS_SINGLE_GAP\">"<<TimetableExport::tr("Teacher has a single gap")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_BORDER_GAP\">"<<TimetableExport::tr("Teacher has a border gap")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_BIG_GAP\">"<<TimetableExport::tr("Teacher has a big gap")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_COME_EARLIER\">"<<TimetableExport::tr("Teacher must come earlier")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_COME_MUCH_EARLIER\">"<<TimetableExport::tr("Teacher must come much earlier")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_STAY_LONGER\">"<<TimetableExport::tr("Teacher must stay longer")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_STAY_MUCH_LONGER\">"<<TimetableExport::tr("Teacher must stay much longer")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_A_FREE_DAY\">"<<TimetableExport::tr("Teacher has a free day")<<"</div>\n";
	tos<<"    <div class=\"TEACHER_IS_NOT_AVAILABLE\">"<<TimetableExport::tr("Teacher is not available")<<"</div>\n";

	tos<<"    <p>&nbsp;</p>\n\n";

	bool PRINT_DETAILED=true;
	do{
		if(PRINT_DETAILED==true)
			tos<<"    <table id=\"table_DETAILED\" border=\"1\">\n";
		else
			tos<<"    <table id=\"table_LESS_DETAILED\" border=\"1\">\n";
		
		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		if(PRINT_DETAILED==true)
			tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Teachers' Free Periods")<<" ("<<tr("Detailed")<<")</th></tr>\n";
		else	tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Teachers' Free Periods")<<" ("<<tr("Less detailed")<<")</th></tr>\n";

		tos<<"        <tr>\n          <!-- span -->\n";
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"xAxis\">";
			else
				tos<<"          <th>";
			tos<<protect2(gt.rules.hoursOfTheDay[j])<<"</th>\n";
		}
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";
		/*workaround. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		*/
		tos<<"      <tbody>\n";
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			tos<<"        <tr>\n";
			if(TIMETABLE_HTML_LEVEL>=2)
				tos<<"          <th class=\"yAxis\">";
			else
				tos<<"          <th>";
			tos<<protect2(gt.rules.daysOfTheWeek[k])<<"</th>\n";
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				tos<<"          <td>";
				bool empty_slot=true;
				for(int tfp=0; tfp<TEACHERS_FREE_PERIODS_N_CATEGORIES; tfp++){
					if(teachers_free_periods_timetable_weekly[tfp][k][j].size()>0){
						empty_slot=false;
						if(TIMETABLE_HTML_LEVEL>=2)
							tos<<"<div class=\"DESCRIPTION\">";
						switch(tfp){
							case TEACHER_HAS_SINGLE_GAP		: tos<<TimetableExport::tr("Single gap"); break;
							case TEACHER_HAS_BORDER_GAP		: tos<<TimetableExport::tr("Border gap"); break;
							case TEACHER_HAS_BIG_GAP		: tos<<TimetableExport::tr("Big gap"); break;
							case TEACHER_MUST_COME_EARLIER		: tos<<TimetableExport::tr("Must come earlier"); break;
							case TEACHER_MUST_STAY_LONGER		: tos<<TimetableExport::tr("Must stay longer"); break;
							case TEACHER_MUST_COME_MUCH_EARLIER	: tos<<TimetableExport::tr("Must come much earlier"); break;
							case TEACHER_MUST_STAY_MUCH_LONGER	: tos<<TimetableExport::tr("Must stay much longer"); break;
							case TEACHER_HAS_A_FREE_DAY		: tos<<TimetableExport::tr("Free day"); break;
							case TEACHER_IS_NOT_AVAILABLE		: tos<<TimetableExport::tr("Not available"); break;
							default: assert(0==1); break;
						}
						if(TIMETABLE_HTML_LEVEL>=2)
							tos<<":</div>";
						else tos<<":<br />";
						if(TIMETABLE_HTML_LEVEL>=3)
							switch(tfp){
								case TEACHER_HAS_SINGLE_GAP		: tos<<"<div class=\"TEACHER_HAS_SINGLE_GAP\">"; break;
								case TEACHER_HAS_BORDER_GAP		: tos<<"<div class=\"TEACHER_HAS_BORDER_GAP\">"; break;
								case TEACHER_HAS_BIG_GAP		: tos<<"<div class=\"TEACHER_HAS_BIG_GAP\">"; break;
								case TEACHER_MUST_COME_EARLIER		: tos<<"<div class=\"TEACHER_MUST_COME_EARLIER\">"; break;
								case TEACHER_MUST_STAY_LONGER		: tos<<"<div class=\"TEACHER_MUST_STAY_LONGER\">"; break;
								case TEACHER_MUST_COME_MUCH_EARLIER	: tos<<"<div class=\"TEACHER_MUST_COME_MUCH_EARLIER\">"; break;
								case TEACHER_MUST_STAY_MUCH_LONGER	: tos<<"<div class=\"TEACHER_MUST_STAY_MUCH_LONGER\">"; break;
								case TEACHER_HAS_A_FREE_DAY		: tos<<"<div class=\"TEACHER_HAS_A_FREE_DAY\">"; break;
								case TEACHER_IS_NOT_AVAILABLE		: tos<<"<div class=\"TEACHER_IS_NOT_AVAILABLE\">"; break;
								default: assert(0==1); break;
							}
						for(int t=0; t<teachers_free_periods_timetable_weekly[tfp][k][j].size(); t++){
							QString teacher_name = gt.rules.internalTeachersList[teachers_free_periods_timetable_weekly[tfp][k][j].at(t)]->name;
								switch(TIMETABLE_HTML_LEVEL){
									case 4 : tos<<"<span class=\"teacher_"<<protect2id(teacher_name)<<"\">"<<protect2(teacher_name)<<"</span>"; break;
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(teacher_name)<<"\" onmouseover=\"highlight('teacher_"<<protect2java(teacher_name)<<"')\">"<<protect2(teacher_name)<<"</span>"; break;
									default: tos<<protect2(teacher_name); break;
								}
							tos<<"<br />";
						}
						if(TIMETABLE_HTML_LEVEL>=3)
							tos<<"</div>";
					}
					if(PRINT_DETAILED==false&&tfp>=TEACHER_MUST_COME_EARLIER) tfp=TEACHER_IS_NOT_AVAILABLE+1;
				}
				if(empty_slot){
					switch(TIMETABLE_HTML_LEVEL){
						case 3 : ;
						case 4 : tos<<"<span class=\"empty\">"<<protect2(STRING_EMPTY_SLOT)<<"</span>"; break;
						case 5 : tos<<"<span class=\"empty\" onmouseover=\"highlight('empty')\">"<<protect2(STRING_EMPTY_SLOT)<<"</span>"; break;
						default: tos<<protect2(STRING_EMPTY_SLOT);
					}
				}
				tos<<"</td>\n";
			}
			tos<<"        </tr>\n";
		}
		//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"        <tr class=\"foot\"><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
		//workaround end.
		tos<<"      </tbody>\n    </table>\n    <p>&nbsp;</p>\n";
		tos<<"    <p><a href=\""<<"#top\">"<<TimetableExport::tr("back to the top")<<"</a></p>\n";
		tos<<"    <p>&nbsp;</p>\n\n";
		if(PRINT_DETAILED==true) PRINT_DETAILED=false;
		else PRINT_DETAILED=true;
	} while(PRINT_DETAILED!=true);
	tos<<"  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 TimetableExport::tr("Writing %1 gave error code %2, which means saving is compromised. Please check your disk's free space.").arg(htmlfilename).arg(file.error()));
	}
	file.close();
}
