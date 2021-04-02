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


//Represents the current status of the simulation - running or stopped.
extern bool simulation_running;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_schedule_ready;

extern Solution best_solution;
extern bool LANGUAGE_STYLE_RIGHT_TO_LEFT;
extern QString LANGUAGE_FOR_HTML;

extern Timetable gt;
extern qint16 teachers_timetable_weekly[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];		//the getXXXmatrix have only qint 8
extern qint16 students_timetable_weekly[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];	//the getXXXmatrix have only qint 8
extern qint16 rooms_timetable_weekly[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];			//the getXXXmatrix have only qint 8


QList<qint16> activitiesForCurrentSubject[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

QList<qint16> activitiesAtTime[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

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
	c.getTeachersTimetable(gt.rules, teachers_timetable_weekly);
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
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1);

	//now write the solution in xml files
	//subgroups
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+SUBGROUPS_TIMETABLE_FILENAME_XML;
	writeSubgroupsTimetableXml(s);
	//teachers
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+TEACHERS_TIMETABLE_FILENAME_XML;
	writeTeachersTimetableXml(s);

	//now get the time. TODO: maybe write it in xml too? so do it a few lines earlier!
	time_t ltime;
	tzset();
	time(&ltime);
	QString sTime=ctime(&ltime);
	//remove the endl, because it looks awful in html and css file(by Volker Dirr)
	int sTs=sTime.size();sTs--;
	if(sTime[sTs]=='\n')
		sTime.remove(sTs,1);

	//now get the number of placed activities. TODO: maybe write it in xml too? so do it a few lines earlier!
	int na=0;
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		if(best_solution.times[i]!=UNALLOCATED_TIME)
			na++;
	
	//write the conflicts in txt mode
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+CONFLICTS_FILENAME;
	writeConflictsTxt(s, sTime, na);
	
	//now write the solution in html files
	if(TIMETABLE_HTML_LEVEL>=1){
		s=OUTPUT_DIR+FILE_SEP+s2+"_"+STYLESHEET_CSS;
		writeStylesheetCss(s, sTime, na);
	}
	//subgroups
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysVerticalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeSubgroupsTimetableTimeHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeSubgroupsTimetableTimeVerticalHtml(s, sTime, na);
	//groups
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeGroupsTimetableDaysHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeGroupsTimetableDaysVerticalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeGroupsTimetableTimeHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeGroupsTimetableTimeVerticalHtml(s, sTime, na);
	//years
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeYearsTimetableDaysHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeYearsTimetableDaysVerticalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeYearsTimetableTimeHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeYearsTimetableTimeVerticalHtml(s, sTime, na);
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
	//subjects
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubjectsTimetableDaysHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubjectsTimetableDaysVerticalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeSubjectsTimetableTimeHorizontalHtml(s, sTime, na);
	s=OUTPUT_DIR+FILE_SEP+s2+"_"+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeSubjectsTimetableTimeVerticalHtml(s, sTime, na);
	
	cout<<"Writing simulation results to disk completed successfully"<<endl;
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

	//now write the solution in xml files
	//subgroups
	s=finalDestDir+SUBGROUPS_TIMETABLE_FILENAME_XML;
	writeSubgroupsTimetableXml(s);
	//teachers
	s=finalDestDir+TEACHERS_TIMETABLE_FILENAME_XML;
	writeTeachersTimetableXml(s);

	//now get the time. TODO: maybe write it in xml too? so do it a few lines earlier!
	time_t ltime;
	tzset();
	time(&ltime);
	QString sTime=ctime(&ltime);
	//remove the endl, because it looks awful in html and css file (by Volker Dirr)
	int sTs=sTime.size();sTs--;
	if(sTime[sTs]=='\n')
		sTime.remove(sTs,1);

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
	
	cout<<"Writing multiple simulation results to disk completed successfully"<<endl;
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

		tos<<"Total soft conflicts: "<<best_solution.conflictsTotal<<endl<<endl;
		tos<<"Soft conflicts list (in decreasing order):"<<endl<<endl;
		foreach(QString t, best_solution.conflictsDescriptionList)
			tos<<t<<endl;
		tos<<endl<<TimetableExport::tr("End of file.")<<"\n\n";
	}
	else{
		tos<<TimetableExport::tr("Conflicts of %1").arg(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1))<<"\n";
		tos<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"\n";
		tos<<TimetableExport::tr("Generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"\n\n";

		tos<<"Total conflicts: "<<best_solution.conflictsTotal<<endl<<endl;
		tos<<"Conflicts list (in decreasing order):"<<endl<<endl;
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
					tos<<"<Subject_Tag name=\""<<protect(act->subjectTagName)<<"\"></Subject_Tag>";

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
					tos<<"<Subject_Tag name=\""<<protect(act->subjectTagName)<<"\"></Subject_Tag>";
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
		for(int i=0; i<gt.rules.subjectTagsList.size(); i++){
			tos << "span.subjecttag_"<<protect2id(gt.rules.subjectTagsList[i]->name)<<" {\n\n}\n\n";
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
		tos<<"span.subjecttag {\n\n}\n\n";
		tos<<"td.student, div.student {\n\n}\n\n";
		tos<<"td.teacher, div.teacher {\n\n}\n\n";
		tos<<"td.room, div.room {\n\n}\n\n";
		tos<<"tr.line0 {\n  font-size: smaller;\n}\n\n";
		tos<<"tr.line1, div.line1 {\n\n}\n\n";
		tos<<"tr.line2, div.line2 {\n  font-size: smaller;\n  color: grey;\n}\n\n";
		tos<<"tr.line3, div.line3 {\n  font-size: smaller;\n  color: grey;\n}\n\n";
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
						if(act->subjectName.size()>0||act->subjectTagName.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"line1\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->subjectTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->subjectTagName); break;
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
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else
					tos<<"          <td>---</td>\n";
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
						if(act->subjectName.size()>0||act->subjectTagName.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"line1\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->subjectTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->subjectTagName); break;
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
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				}
				else
					tos<<"          <td>---</td>\n";
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
						if(act->subjectName.size()>0||act->subjectTagName.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"line1\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->subjectTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->subjectTagName); break;
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
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else
					tos<<"          <td>---</td>\n";
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
						if(act->subjectName.size()>0||act->subjectTagName.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"line1\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->subjectTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->subjectTagName); break;
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
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else
					tos<<"          <td>---</td>\n";
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
									if(act->subjectName.size()>0||act->subjectTagName.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"line1\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
												case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												default: tos<<protect2(act->subjectName); break;
											}
										if(act->subjectTagName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
												case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
												case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
												default: tos<<" "<<protect2(act->subjectTagName); break;
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
												case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
												case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									tos<<"</td>\n";
								} else
									tos<<"          <!-- span -->\n";
							} else
								tos<<"          <td>---</td>\n";
						} else{
							if(PRINT_DETAILED==false) tos<<"          <td>\?\?\?</td>\n";
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
													case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
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
										if(act->subjectName.size()>0||act->subjectTagName.size()>0){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
													case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													default: tos<<protect2(act->subjectName); break;
												}
											if(act->subjectTagName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
													case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
													case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
													default: tos<<" "<<protect2(act->subjectTagName); break;
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
													case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
													case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
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
									if(act->subjectName.size()>0||act->subjectTagName.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"line1\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
												case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												default: tos<<protect2(act->subjectName); break;
											}
										if(act->subjectTagName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
												case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
												case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
												default: tos<<" "<<protect2(act->subjectTagName); break;
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
												case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
												case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									tos<<"</td>\n";
								} else
									tos<<"          <!-- span -->\n";
							} else
								tos<<"          <td>---</td>\n";
						} else{
							if(PRINT_DETAILED==false) tos<<"          <td>\?\?\?</td>\n";
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
													case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
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
										if(act->subjectName.size()>0||act->subjectTagName.size()>0){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
													case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													default: tos<<protect2(act->subjectName); break;
												}
											if(act->subjectTagName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
													case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
													case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
													default: tos<<" "<<protect2(act->subjectTagName); break;
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
													case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
													case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
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
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			for(int g=0; g<sty->groupsList.size(); g++){
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
				int group=0;
				for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
					StudentsYear* sty=gt.rules.augmentedYearsList[i];
					for(int g=0; g<sty->groupsList.size(); g++){
						StudentsGroup* stg=sty->groupsList[g];
						QList<qint16> allActivities;
						allActivities.clear();
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
									if(act->subjectName.size()>0||act->subjectTagName.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"line1\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
												case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												default: tos<<protect2(act->subjectName); break;
											}
										if(act->subjectTagName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
												case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
												case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
												default: tos<<" "<<protect2(act->subjectTagName); break;
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
												case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
												case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									tos<<"</td>\n";
								} else
									tos<<"          <!-- span -->\n";
							} else
								tos<<"          <td>---</td>\n";
						} else{
							if(PRINT_DETAILED==false) tos<<"          <td>\?\?\?</td>\n";
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
													case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
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
										if(act->subjectName.size()>0||act->subjectTagName.size()>0){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
													case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													default: tos<<protect2(act->subjectName); break;
												}
											if(act->subjectTagName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
													case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
													case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
													default: tos<<" "<<protect2(act->subjectTagName); break;
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
													case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
													case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
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
						group++;
					}
				}
				tos<<"        </tr>\n";
			}
		}
		//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalSubgroups<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
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
		tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalSubgroups<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
		*/
		tos<<"      <tbody>\n";
		
		int group=0;
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
									if(act->subjectName.size()>0||act->subjectTagName.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"line1\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
												case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												default: tos<<protect2(act->subjectName); break;
											}
										if(act->subjectTagName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
												case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
												case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
												default: tos<<" "<<protect2(act->subjectTagName); break;
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
												case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
												case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									tos<<"</td>\n";
								} else
									tos<<"          <!-- span -->\n";
							} else
								tos<<"          <td>---</td>\n";
						} else{
							if(PRINT_DETAILED==false) tos<<"          <td>\?\?\?</td>\n";
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
													case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
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
										if(act->subjectName.size()>0||act->subjectTagName.size()>0){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
													case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													default: tos<<protect2(act->subjectName); break;
												}
											if(act->subjectTagName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
													case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
													case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
													default: tos<<" "<<protect2(act->subjectTagName); break;
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
													case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
													case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
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
				group++;
			}
		}
		//workaround begin. compare http://www.openoffice.org/issues/show_bug.cgi?id=82600
		tos<<"      <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalSubgroups<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
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
									if(act->subjectName.size()>0||act->subjectTagName.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"line1\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
												case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												default: tos<<protect2(act->subjectName); break;
											}
										if(act->subjectTagName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
												case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
												case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
												default: tos<<" "<<protect2(act->subjectTagName); break;
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
												case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
												case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									tos<<"</td>\n";
								} else
									tos<<"          <!-- span -->\n";
							} else
								tos<<"          <td>---</td>\n";
						} else{
							if(PRINT_DETAILED==false) tos<<"          <td>\?\?\?</td>\n";
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
													case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
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
										if(act->subjectName.size()>0||act->subjectTagName.size()>0){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
													case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													default: tos<<protect2(act->subjectName); break;
												}
											if(act->subjectTagName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
													case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
													case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
													default: tos<<" "<<protect2(act->subjectTagName); break;
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
													case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
													case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
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
									if(act->subjectName.size()>0||act->subjectTagName.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"line1\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
												case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												default: tos<<protect2(act->subjectName); break;
											}
										if(act->subjectTagName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
												case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
												case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
												default: tos<<" "<<protect2(act->subjectTagName); break;
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
												case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
												case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									tos<<"</td>\n";
								} else
									tos<<"          <!-- span -->\n";
							} else
								tos<<"          <td>---</td>\n";
						} else{
							if(PRINT_DETAILED==false) tos<<"          <td>\?\?\?</td>\n";
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
													case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
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
										if(act->subjectName.size()>0||act->subjectTagName.size()>0){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
													case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													default: tos<<protect2(act->subjectName); break;
												}
											if(act->subjectTagName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
													case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
													case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
													default: tos<<" "<<protect2(act->subjectTagName); break;
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
													case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
													case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
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
				for(int year=0; year<gt.rules.augmentedYearsList.size(); year++){
						StudentsYear* sty=gt.rules.augmentedYearsList[year];
						QList<qint16> allActivities;
						allActivities.clear();
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
									if(act->subjectName.size()>0||act->subjectTagName.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"line1\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
												case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												default: tos<<protect2(act->subjectName); break;
											}
										if(act->subjectTagName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
												case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
												case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
												default: tos<<" "<<protect2(act->subjectTagName); break;
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
												case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
												case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									tos<<"</td>\n";
								} else
									tos<<"          <!-- span -->\n";
							} else
								tos<<"          <td>---</td>\n";
						} else{
							if(PRINT_DETAILED==false) tos<<"          <td>\?\?\?</td>\n";
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
													case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
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
										if(act->subjectName.size()>0||act->subjectTagName.size()>0){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
													case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													default: tos<<protect2(act->subjectName); break;
												}
											if(act->subjectTagName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
													case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
													case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
													default: tos<<" "<<protect2(act->subjectTagName); break;
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
													case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
													case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
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
		tos<<"      <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalSubgroups<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
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
									if(act->subjectName.size()>0||act->subjectTagName.size()>0){
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"<div class=\"line1\">";
										if(act->subjectName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
												case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
												default: tos<<protect2(act->subjectName); break;
											}
										if(act->subjectTagName.size()>0)
											switch(TIMETABLE_HTML_LEVEL){
												case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
												case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
												case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
												default: tos<<" "<<protect2(act->subjectTagName); break;
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
												case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
												case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
												default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
											}
										if(TIMETABLE_HTML_LEVEL>=3)
											tos<<"</div>";
										else tos<<"<br />";
									}
									tos<<"</td>\n";
								} else
									tos<<"          <!-- span -->\n";
							} else
								tos<<"          <td>---</td>\n";
						} else{
							if(PRINT_DETAILED==false) tos<<"          <td>\?\?\?</td>\n";
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
													case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
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
										if(act->subjectName.size()>0||act->subjectTagName.size()>0){
											if(act->subjectName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
													case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
													default: tos<<protect2(act->subjectName); break;
												}
											if(act->subjectTagName.size()>0)
												switch(TIMETABLE_HTML_LEVEL){
													case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
													case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
													case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
													default: tos<<" "<<protect2(act->subjectTagName); break;
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
													case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
													case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
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
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
									default: tos<<protect2(*st); break;
								}
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						if(act->subjectName.size()>0||act->subjectTagName.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"line2\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->subjectTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->subjectTagName); break;
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
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else
					tos<<"          <td>---</td>\n";
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
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
									default: tos<<protect2(*st); break;
								}
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						if(act->subjectName.size()>0||act->subjectTagName.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"line2\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->subjectTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->subjectTagName); break;
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
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				}
				else
					tos<<"          <td>---</td>\n";
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
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
									default: tos<<protect2(*st); break;
								}
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						if(act->subjectName.size()>0||act->subjectTagName.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"line2\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->subjectTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->subjectTagName); break;
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
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else
					tos<<"          <td>---</td>\n";
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
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
									default: tos<<protect2(*st); break;
								}
								if(st!=act->studentsNames.end()-1)
									tos<<", ";
							}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						if(act->subjectName.size()>0||act->subjectTagName.size()>0){
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"<div class=\"line2\">";
							if(act->subjectName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
									case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
									default: tos<<protect2(act->subjectName); break;
								}
							if(act->subjectTagName.size()>0)
								switch(TIMETABLE_HTML_LEVEL){
									case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
									case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
									case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
									default: tos<<" "<<protect2(act->subjectTagName); break;
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
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
									default: tos<<protect2(gt.rules.internalRoomsList[r]->name); break;
								}
							if(TIMETABLE_HTML_LEVEL>=3)
								tos<<"</div>";
							else tos<<"<br />";
						}
						tos<<"</td>\n";
					} else
						tos<<"          <!-- span -->\n";
				} else
					tos<<"          <td>---</td>\n";
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
										case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
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
										case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
										default: tos<<protect2(*it); break;
									}
									if(it!=act->teachersNames.end()-1)
										tos<<", ";
								}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							if(act->subjectName.size()>0||act->subjectTagName.size()>0){
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<div class=\"line3\">";
								if(act->subjectName.size()>0)
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
										case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
										case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
										default: tos<<protect2(act->subjectName); break;
									}
								if(act->subjectTagName.size()>0)
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
										case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
										case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
										default: tos<<" "<<protect2(act->subjectTagName); break;
									}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							tos<<"</td>\n";								
						} else
							tos<<"          <!-- span -->\n";
					} else
						tos<<"          <td>---</td>\n";
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
										case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
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
										case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
										default: tos<<protect2(*it); break;
									}
									if(it!=act->teachersNames.end()-1)
										tos<<", ";
								}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							if(act->subjectName.size()>0||act->subjectTagName.size()>0){
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<div class=\"line3\">";
								if(act->subjectName.size()>0)
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
										case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
										case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
										default: tos<<protect2(act->subjectName); break;
									}
								if(act->subjectTagName.size()>0)
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
										case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
										case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
										default: tos<<" "<<protect2(act->subjectTagName); break;
									}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							tos<<"</td>\n";	
						} else
							tos<<"          <!-- span -->\n";
					}
					else
						tos<<"          <td>---</td>\n";
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
										case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
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
										case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
										default: tos<<protect2(*it); break;
									}
									if(it!=act->teachersNames.end()-1)
										tos<<", ";
								}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							if(act->subjectName.size()>0||act->subjectTagName.size()>0){
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<div class=\"line3\">";
								if(act->subjectName.size()>0)
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
										case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
										case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
										default: tos<<protect2(act->subjectName); break;
									}
								if(act->subjectTagName.size()>0)
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
										case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
										case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
										default: tos<<" "<<protect2(act->subjectTagName); break;
									}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							tos<<"</td>\n";	
						} else
							tos<<"          <!-- span -->\n";
					} else
						tos<<"          <td>---</td>\n";
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
										case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
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
										case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
										default: tos<<protect2(*it); break;
									}
									if(it!=act->teachersNames.end()-1)
										tos<<", ";
								}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							if(act->subjectName.size()>0||act->subjectTagName.size()>0){
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"<div class=\"line3\">";
								if(act->subjectName.size()>0)
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : tos<<"<span class=\"subject\">"<<protect2(act->subjectName)<<"</span>"; break;
										case 4 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\">"<<protect2(act->subjectName)<<"</span></span>"; break;
										case 5 : tos<<"<span class=\"subject\"><span class=\"subject_"<<protect2id(act->subjectName)<<"\" onmouseover=\"highlight('subject_"<<protect2id(act->subjectName)<<"')\">"<<protect2(act->subjectName)<<"</span></span>"; break;
										default: tos<<protect2(act->subjectName); break;
									}
								if(act->subjectTagName.size()>0)
									switch(TIMETABLE_HTML_LEVEL){
										case 3 : tos<<" <span class=\"subjecttag\">"<<protect2(act->subjectTagName)<<"</span>"; break;
										case 4 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
										case 5 : tos<<" <span class=\"subjecttag\"><span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span></span>"; break;
										default: tos<<" "<<protect2(act->subjectTagName); break;
									}
								if(TIMETABLE_HTML_LEVEL>=3)
									tos<<"</div>";
								else tos<<"<br />";
							}
							tos<<"</td>\n";	
						} else
							tos<<"          <!-- span -->\n";
					} else
						tos<<"          <td>---</td>\n";
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
				if(allActivities.isEmpty())
					tos<<"          <td>---</td>\n";
				else {
					int ai=allActivities[0]; //activity index
					if(TIMETABLE_HTML_LEVEL>=1)
						tos<<"          <td><table class=\"detailed\">";
					else
						tos<<"          <td><table>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"line0 subjecttag\">";
					else	tos<<"<tr>";
					for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->subjectTagName.size()>0)
							switch(TIMETABLE_HTML_LEVEL){
								case 4 : tos<<"<span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span>"; break;
								case 5 : tos<<"<span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span>"; break;
								default: tos<<protect2(act->subjectTagName); break;
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
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
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
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
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
				
				if(allActivities.isEmpty())
					tos<<"          <td>---</td>\n";
				else {
					int ai=allActivities[0]; //activity index
					if(TIMETABLE_HTML_LEVEL>=1)
						tos<<"          <td><table class=\"detailed\">";
					else
						tos<<"          <td><table>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"line0 subjecttag\">";
					else	tos<<"<tr>";
					for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->subjectTagName.size()>0)
							switch(TIMETABLE_HTML_LEVEL){
								case 4 : tos<<"<span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span>"; break;
								case 5 : tos<<"<span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span>"; break;
								default: tos<<protect2(act->subjectTagName); break;
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
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
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
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
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
	tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalSubgroups<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
	*/
	tos<<"      <tbody>\n";


	//LIVIU
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
				if(allActivities.isEmpty())
					tos<<"          <td>---</td>\n";
				else {
					int ai=allActivities[0]; //activity index
					if(TIMETABLE_HTML_LEVEL>=1)
						tos<<"          <td><table class=\"detailed\">";
					else
						tos<<"          <td><table>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"line0 subjecttag\">";
					else	tos<<"<tr>";
					for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->subjectTagName.size()>0)
							switch(TIMETABLE_HTML_LEVEL){
								case 4 : tos<<"<span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span>"; break;
								case 5 : tos<<"<span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span>"; break;
								default: tos<<protect2(act->subjectTagName); break;
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
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
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
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
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
	tos<<"      <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalSubgroups<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr>\n";
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
				if(allActivities.isEmpty())
					tos<<"          <td>---</td>\n";
				else {
					int ai=allActivities[0]; //activity index
					if(TIMETABLE_HTML_LEVEL>=1)
						tos<<"          <td><table class=\"detailed\">";
					else
						tos<<"          <td><table>";
					if(TIMETABLE_HTML_LEVEL>=3)
						tos<<"<tr class=\"line0 subjecttag\">";
					else	tos<<"<tr>";
					for(int a=0; a<allActivities.size(); a++){
						ai=allActivities[a];
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(TIMETABLE_HTML_LEVEL>=1)
							tos<<"<td class=\"detailed\">";
						else
							tos<<"<td>";
						if(act->subjectTagName.size()>0)
							switch(TIMETABLE_HTML_LEVEL){
								case 4 : tos<<"<span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\">"<<protect2(act->subjectTagName)<<"</span>"; break;
								case 5 : tos<<"<span class=\"subjecttag_"<<protect2id(act->subjectTagName)<<"\" onmouseover=\"highlight('subjecttag_"<<protect2id(act->subjectTagName)<<"')\">"<<protect2(act->subjectTagName)<<"</span>"; break;
								default: tos<<protect2(act->subjectTagName); break;
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
									case 5 : tos<<"<span class=\"student_"<<protect2id(*st)<<"\" onmouseover=\"highlight('student_"<<protect2id(*st)<<"')\">"<<protect2(*st)<<"</span>"; break;
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
									case 5 : tos<<"<span class=\"teacher_"<<protect2id(*it)<<"\" onmouseover=\"highlight('teacher_"<<protect2id(*it)<<"')\">"<<protect2(*it)<<"</span>"; break;
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
									case 5 : tos<<"<span class=\"room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"\" onmouseover=\"highlight('room_"<<protect2id(gt.rules.internalRoomsList[r]->name)<<"')\">"<<protect2(gt.rules.internalRoomsList[r]->name)<<"</span>"; break;
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
