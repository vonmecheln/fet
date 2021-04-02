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
//                - span using
//                - times vertical
//                - table of content with hyperlinks
//                - css and JavaScript support
//                - rooms
//                - TIMETABLE_HTML_LEVEL


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


extern Timetable gt;
extern qint16 teachers_timetable_weekly[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern qint16 students_timetable_weekly[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern qint16 rooms_timetable_weekly[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];


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

void TimetableExport::writeSimulationResults(int n){
	QDir dir;

	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR))
		dir.mkdir(OUTPUT_DIR);

	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

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
	//students
	s=finalDestDir+STUDENTS_TIMETABLE_FILENAME_XML;
	writeStudentsTimetableXml(s);
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
	//students
	s=finalDestDir+STUDENTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeStudentsTimetableDaysHorizontalHtml(s, sTime, na);
	s=finalDestDir+STUDENTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeStudentsTimetableDaysVerticalHtml(s, sTime, na);
	s=finalDestDir+STUDENTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
	writeStudentsTimetableTimeHorizontalHtml(s, sTime, na);
	s=finalDestDir+STUDENTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
	writeStudentsTimetableTimeVerticalHtml(s, sTime, na);
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
		 QObject::tr("Cannot open file to write soft conflicts txt - please check you disk free space. Saving of soft conflicts aborted"));
		 
		return;
	
		assert(0);
	}
	QTextStream tos(&file);
	//tos.setCodec("UTF-8");
	//tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);
	
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
		 QObject::tr("Writing soft conflicts txt gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
	}
								
	file.close();
}

/**
Function writing the students' timetable in xml format to a file
*/
void TimetableExport::writeStudentsTimetableXml(const QString& xmlfilename)
{
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an XML file
	QFile file(xmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 QObject::tr("Cannot open file to write students timetable xml - please check you disk free space. Saving of xml students timetable aborted"));
		 
		return;
	
		assert(0);
	}
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
		 QObject::tr("Writing students timetable xml gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
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
		 QObject::tr("Cannot open file to write teachers timetable xml - please check you disk free space. Saving of xml teachers timetable aborted"));
		 
		return;
	
		assert(0);
	}
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
		 QObject::tr("Writing teachers timetable xml gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
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
		 QObject::tr("Cannot open file to write css style file - please check you disk free space. Saving of css style file aborted"));
		 
		return;
	
		assert(0);
	}
	QTextStream tos(&file);
	tos.setCodec("UTF-8");
	tos.setGenerateByteOrderMark(true);
	//tos.setEncoding(QTextStream::UnicodeUTF8);

	tos<<"/* "<<TimetableExport::tr("CSS Stylesheet of %1").arg(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev(FILE_SEP)-1))<<"\n";
	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"   "<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"\n";
	tos<<"   "<<TimetableExport::tr("Stylesheet generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<" */\n\n";

	tos<<"/* "<<TimetableExport::tr("To do a page-break only after every second timetable, cut line %1 and paste it into line %2.").arg(8).arg(14)<<" */\n";
	tos<<"/* "<<TimetableExport::tr("To hide an element just write the following phrase into the element")<<": display:none; */\n\n";
	tos<<"table {\n  page-break-before: always;\n  text-align: center;\n  border: 1px outset black;\n}\n\n";
	tos<<"table.modulo2 {\n\n}\n\n";
	tos<<"caption {\n\n}\n\n";
	tos<<"thead {\n\n}\n\n";
	tos<<"tfoot {\n\n}\n\n";
	tos<<"tbody {\n\n}\n\n";
	tos<<"th {\n  border: 1px inset black;\n}\n\n";
	tos<<"td {\n  border: 1px inset black;\n}\n\n";
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
		for(int i=0; i<gt.rules.yearsList.size(); i++){
			StudentsYear* sty=gt.rules.yearsList[i];
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
		tos<<"div.student {\n\n}\n\n";
		tos<<"div.teacher {\n\n}\n\n";
		tos<<"div.room {\n\n}\n\n";
		tos<<"div.line1 {\n\n}\n\n";
		tos<<"div.line2 {\n  font-size: smaller;\n  color: grey;\n}\n\n";
		tos<<"div.line3 {\n  font-size: smaller;\n  color: grey;\n}\n\n";
	}
	tos<<endl<<"/* "<<TimetableExport::tr("End of file.")<<" */\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 QObject::tr("Writing html css file gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
	}
						

	file.close();
}


//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the students' timetable in html format to a file.
Days horizontal
*/
void TimetableExport::writeStudentsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 QObject::tr("Cannot open file to write students timetable html days horizontal - please check you disk free space. Saving of html students timetable aborted"));
		 
		return;

		assert(0);
	}
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
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
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

		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
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
									default: tos<<protect2(act->subjectTagName); break;
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
		 QObject::tr("Writing students timetable html days horizontal gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
	}
								
	file.close();
}


//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the students' timetable in html format to a file.
Days vertical
*/
void TimetableExport::writeStudentsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 QObject::tr("Cannot open file to write students timetable html days vertical - please check you disk free space. Saving of html students timetable aborted"));
		 
		return;

		assert(0);
	}
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
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
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

		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
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
									default: tos<<protect2(act->subjectTagName); break;
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
		 QObject::tr("Writing students timetable html days vertical gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
	}
								
	file.close();
}


//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the students' timetable html format to a file
Time vertical
*/

void TimetableExport::writeStudentsTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 QObject::tr("Cannot open file to write students timetable html time vertical - please check you disk free space. Saving of html students timetable aborted"));
		 
		return;

		assert(0);
	}
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

	tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalSubgroups<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
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
									default: tos<<protect2(act->subjectTagName); break;
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
	tos << "      </tbody>\n    </table>\n";
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 QObject::tr("Writing students timetable html time vertical gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
	}
								
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code by Liviu Lalescu)
/**
Function writing the students' timetable in html format to a file.
Time horizontal
*/
void TimetableExport::writeStudentsTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	//assert(gt.timePopulation.initialized);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready);

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
	if(!file.open(QIODevice::WriteOnly)){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 QObject::tr("Cannot open file to write students timetable html time horizontal - please check you disk free space. Saving of html students timetable aborted"));
		 
		return;

		assert(0);
	}
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

	tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
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
									default: tos<<protect2(act->subjectTagName); break;
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
	tos << "      </tbody>\n    </table>\n";
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 QObject::tr("Writing students timetable html time horizontal gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
	}
								
	file.close();
}



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
		 QObject::tr("Cannot open file to write teachers timetable html days horizontal - please check you disk free space. Saving of html teachers timetable aborted"));
		 
		return;

		assert(0);
	}
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

		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
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
									default: tos<<protect2(act->subjectTagName); break;
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
		 QObject::tr("Writing teachers timetable html days horizontal gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
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
		 QObject::tr("Cannot open file to write teachers timetable html days vertical - please check you disk free space. Saving of html teachers timetable aborted"));
		 
		return;

		assert(0);
	}
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

		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
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
									default: tos<<protect2(act->subjectTagName); break;
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
		 QObject::tr("Writing teachers timetable html days vertical gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
	}
								
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
//(old code was done by Liviu Lalescu)
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
		 QObject::tr("Cannot open file to write teachers timetable html time vertical - please check you disk free space. Saving of html teachers timetable aborted"));
		 
		return;

		assert(0);
	}
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

	tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalTeachers<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
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
									default: tos<<protect2(act->subjectTagName); break;
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
	tos << "      </tbody>\n    </table>\n";
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 QObject::tr("Writing teachers timetable html time vertical gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
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
		 QObject::tr("Cannot open file to write teachers timetable html time horizontal - please check you disk free space. Saving of html teachers timetable aborted"));
		 
		return;

		assert(0);
	}
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

	tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
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
									default: tos<<protect2(act->subjectTagName); break;
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
	tos << "      </tbody>\n    </table>\n";
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 QObject::tr("Writing teachers timetable html time horizontal gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
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
		 QObject::tr("Cannot open file to write rooms timetable html days horizontal - please check you disk free space. Saving of html rooms timetable aborted"));
		 
		return;

		assert(0);
	}
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

			tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
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
										default: tos<<protect2(act->subjectTagName); break;
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
		 QObject::tr("Writing rooms timetable html days horizontal gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
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
		 QObject::tr("Cannot open file to write rooms timetable html days vertical - please check you disk free space. Saving of html rooms timetable aborted"));
		 
		return;

		assert(0);
	}
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

			tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
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
										default: tos<<protect2(act->subjectTagName); break;
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
		 QObject::tr("Writing rooms timetable html days vertical gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
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
		 QObject::tr("Cannot open file to write rooms timetable html time vertical - please check you disk free space. Saving of html rooms timetable aborted"));
		 
		return;

		assert(0);
	}
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

		tos<<"      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""<<gt.rules.nInternalRooms<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
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
										default: tos<<protect2(act->subjectTagName); break;
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
		tos << "      </tbody>\n    </table>\n";
	}
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 QObject::tr("Writing rooms timetable html time vertical gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
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
		 QObject::tr("Cannot open file to write rooms timetable html time horizontal - please check you disk free space. Saving of html rooms timetable aborted"));
		 
		return;

		assert(0);
	}
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

		tos<<"      <tfoot><tr><td></td><td colspan=\""<<gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek<<"\">"<<TimetableExport::tr("Timetable generated with FET %1 on %2").arg(FET_VERSION).arg(saveTime)<<"</td></tr></tfoot>\n";
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
										default: tos<<protect2(act->subjectTagName); break;
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
		tos << "      </tbody>\n    </table>\n";
	}
	tos << "  </body>\n</html>\n\n";

	if(file.error()>0){
		QMessageBox::critical(NULL, QObject::tr("FET critical"),
		 QObject::tr("Writing rooms timetable html time horizontal gave error code %1, which means saving is compromised. Please check your disk free space")
		 .arg(file.error()));
	}
								
	file.close();
}
