/*
File timetableexport.cpp
*/

/***************************************************************************
                          timetableexport.cpp  -  description
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

//**********************************************************************************************************************/
//August 2007
//XHTML generation code by Volker Dirr (timetabling.de)
//Features:   - XHTML 1.0 strict valid
//            - using colspan and rowspan
//            - table of contents with hyperlinks
//            - CSS and JavaScript support
//            - index HTML file
//            - TIMETABLE_HTML_LEVEL
//            - days/time horizontal/vertical
//            - subgroups, groups, years, teachers, rooms, subjects, activities, activity tags timetable
//            - teachers free periods
//            - daily timetable
//            - activities with same starting time
//            - reorganized functions. now they can be also used for printing
//            - split times tables after X names (TIMETABLE_HTML_SPLIT?) and choose if subjects, activity tags, teachers, students and/or rooms should be printed
//            - (TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS)
//            - teachers and students statistics (gaps, free days, hours)

//TODO: all must be internal here. so maybe also do daysOfTheWeek and hoursPerDay also internal
//maybe TODO: use back_odd and back_even (or back0 and back1, because easier to code!) like in printing. so don't use the table_odd and table_even anymore
//maybe TODO: make TIMETABLE_HTML_SPLIT? (similar to TIMETABLE_HTML_LEVEL)
//maybe TODO: rename augmentedYearsList into internalYearsList to have it similar to others?
//maybe TODO: some "stg" stuff can be replaced by gt.rules.internalGroupsList. I don't want to do that now, because id-s will change. That is not critical, but I want to diff tables with old release.

#include "timetable_defs.h"
#include "timetable.h"
#include "timetableexport.h"
#include "solution.h"

#include "matrix.h"

#include <iostream>
using namespace std;

#include <Qt>

#include <QString>
#include <QTextStream>
#include <QFile>

#include <QList>

#include <QHash>

#include "messageboxes.h"

#include <QLocale>
#include <QTime>
#include <QDate>

#include <QDir>

//std::stable_sort and std::reverse
#include <algorithm>

static const bool removeOldFiles=false;

//Represents the current status of the generation - running or stopped.
//extern bool generation_running;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_buildings_schedule_ready;

extern Solution best_solution;
extern bool LANGUAGE_STYLE_RIGHT_TO_LEFT;
extern QString LANGUAGE_FOR_HTML;

extern Timetable gt;
extern Matrix3D<int> teachers_timetable_weekly;
extern Matrix3D<int> students_timetable_weekly;
extern Matrix3D<int> rooms_timetable_weekly;
extern Matrix3D<QList<int>> virtual_rooms_timetable_weekly;

extern Matrix3D<QList<int>> buildings_timetable_weekly;

extern Matrix3D<QList<int>> teachers_free_periods_timetable_weekly;

extern Matrix2D<bool> breakDayHour;
extern Matrix3D<bool> teacherNotAvailableDayHour;
extern Matrix2D<double> notAllowedRoomTimePercentages;
extern Matrix3D<bool> subgroupNotAvailableDayHour;

static Matrix2D<QList<int>> activitiesForCurrentSubject;
static Matrix2D<QList<int>> activitiesForCurrentActivityTag;

static Matrix2D<QList<int>> activitiesAtTimeList;
static Matrix2D<QSet<int>> activitiesAtTimeSet;

extern Rules rules2;

const QString STRING_EMPTY_SLOT="---";

//const QString STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES="???";

const QString STRING_NOT_AVAILABLE_TIME_SLOT="-x-";

const QString STRING_BREAK_SLOT="-X-";

//these hashes are needed to get the IDs for HTML and CSS in timetableexport and statistics
static QHash<QString, QString> hashSubjectIDsTimetable;
static QHash<QString, QString> hashActivityTagIDsTimetable;
static QHash<QString, QString> hashStudentIDsTimetable;
static QHash<QString, QString> hashTeacherIDsTimetable;
static QHash<QString, QString> hashBuildingIDsTimetable;
static QHash<QString, QString> hashRoomIDsTimetable;
static QHash<QString, QString> hashDayIDsTimetable;

//static QHash<QString, QString> hashColorStringIDsTimetable;
QHash<int, int> hashActivityColorBySubject;
QList<int> activeHashActivityColorBySubject;
QHash<int, int> hashActivityColorBySubjectAndStudents;
QList<int> activeHashActivityColorBySubjectAndStudents;
const int COLOR_BY_SUBJECT=1;
const int COLOR_BY_SUBJECT_STUDENTS=2;

//these hashes are needed to care about activities with same starting time
static QHash<int, QList<int>> activitiesWithSameStartingTimeList;
static QHash<int, QSet<int>> activitiesWithSameStartingTimeSet;

//Now the filenames of the output files are following (for XML and all HTML tables)
const QString SUBGROUPS_TIMETABLE_FILENAME_XML="subgroups.xml";
const QString TEACHERS_TIMETABLE_FILENAME_XML="teachers.xml";
const QString ACTIVITIES_TIMETABLE_FILENAME_XML="activities.xml";
//const QString ROOMS_TIMETABLE_FILENAME_XML="rooms.xml";

const QString CONFLICTS_FILENAME="soft_conflicts.txt";
const QString INDEX_HTML="index.html";
const QString STYLESHEET_CSS="stylesheet.css";

const QString SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="subgroups_days_horizontal.html";
const QString SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="subgroups_days_vertical.html";
const QString SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="subgroups_time_horizontal.html";
const QString SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="subgroups_time_vertical.html";

const QString GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="groups_days_horizontal.html";
const QString GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="groups_days_vertical.html";
const QString GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="groups_time_horizontal.html";
const QString GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="groups_time_vertical.html";

const QString YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="years_days_horizontal.html";
const QString YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="years_days_vertical.html";
const QString YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="years_time_horizontal.html";
const QString YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="years_time_vertical.html";

const QString TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="teachers_days_horizontal.html";
const QString TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="teachers_days_vertical.html";
const QString TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="teachers_time_horizontal.html";
const QString TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="teachers_time_vertical.html";

const QString BUILDINGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="buildings_days_horizontal.html";
const QString BUILDINGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="buildings_days_vertical.html";
const QString BUILDINGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="buildings_time_horizontal.html";
const QString BUILDINGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="buildings_time_vertical.html";

const QString ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="rooms_days_horizontal.html";
const QString ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="rooms_days_vertical.html";
const QString ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="rooms_time_horizontal.html";
const QString ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="rooms_time_vertical.html";

const QString SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="subjects_days_horizontal.html";
const QString SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="subjects_days_vertical.html";
const QString SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="subjects_time_horizontal.html";
const QString SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="subjects_time_vertical.html";

const QString ACTIVITY_TAGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="activity_tags_days_horizontal.html";
const QString ACTIVITY_TAGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="activity_tags_days_vertical.html";
const QString ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="activity_tags_time_horizontal.html";
const QString ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="activity_tags_time_vertical.html";

const QString ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="activities_days_horizontal.html";
const QString ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="activities_days_vertical.html";
const QString ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="activities_time_horizontal.html";
const QString ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="activities_time_vertical.html";

const QString TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="teachers_free_periods_days_horizontal.html";
const QString TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="teachers_free_periods_days_vertical.html";

const QString TEACHERS_STATISTICS_FILENAME_HTML="teachers_statistics.html";
const QString STUDENTS_STATISTICS_FILENAME_HTML="students_statistics.html";

const QString MULTIPLE_TIMETABLE_DATA_RESULTS_FILE="data_and_timetable.fet";

//now the XML tags used for identification of the output file (is that comment correct? it's the old comment)
const QString STUDENTS_TIMETABLE_TAG="Students_Timetable";
const QString TEACHERS_TIMETABLE_TAG="Teachers_Timetable";
const QString ACTIVITIES_TIMETABLE_TAG="Activities_Timetable";
const QString ROOMS_TIMETABLE_TAG="Rooms_Timetable";

const QString RANDOM_SEED_FILENAME_BEFORE="random_seed_before.txt";
const QString RANDOM_SEED_FILENAME_AFTER="random_seed_after.txt";

const bool USE_DUMMY_TH_TR_TD=true;

//extern int XX;
//extern int YY;
//extern MRG32k3a rng;

QString generationLocalizedTime=QString(""); //to be used in timetableprintform.cpp

QString TimetableExport::dummyTh(int nSpacesBefore)
{
	return QString(nSpacesBefore, ' ')+QString("<th></th>")+QString("\n");
}

QString TimetableExport::dummyTr(int nSpacesBeforeTr, int nSpacesBeforeTd, int colspan)
{
	return QString(nSpacesBeforeTr, ' ')+QString("<tr>\n")+
	 QString(nSpacesBeforeTd, ' ')+QString("<td colspan=\"")+CustomFETString::number(colspan)+QString("\"></td>\n")+
	 QString(nSpacesBeforeTr, ' ')+QString("</tr>\n");
}

QString TimetableExport::dummyTd(int nSpacesBefore)
{
	return QString(nSpacesBefore, ' ')+QString("<td></td>")+QString("\n");
}

//similar with the code from Marco Vassura, modified by Volker Dirr to avoid usage of QColor and QBrush, since these need QtGui.
//(the command-line version does not have access to QtGui.)
//slightly modified by Liviu Lalescu on 2021-03-01
void TimetableExport::stringToColor(const QString& s, int& r, int& g, int& b)
{
	// CRC-24 based on RFC 2440 Section 6.1
	unsigned long int crc = 0xB704CEUL;
	QByteArray ba=s.toUtf8();
	for(char c : std::as_const(ba)){
		unsigned char uc=(unsigned char)(c);
		crc ^= (uc & 0xFF) << 16;
		for (int i = 0; i < 8; i++) {
			crc <<= 1;
			if (crc & 0x1000000UL)
				crc ^= 0x1864CFBUL;
		}
	}

	r = int((crc>>16) & 0xFF);
	g = int((crc>>8) & 0xFF);
	b = int(crc & 0xFF);
}
//similar with the code from Marco Vassura, modified by Volker Dirr

bool writeAtLeastATimetable()
{
	bool t = WRITE_TIMETABLE_CONFLICTS ||
	
	 (WRITE_TIMETABLES_STATISTICS &&
	 (WRITE_TIMETABLES_SUBGROUPS ||
	 WRITE_TIMETABLES_GROUPS ||
	 WRITE_TIMETABLES_YEARS ||
	 WRITE_TIMETABLES_TEACHERS)) ||
	
	 (WRITE_TIMETABLES_XML &&
	 (WRITE_TIMETABLES_SUBGROUPS ||
	 WRITE_TIMETABLES_TEACHERS ||
	 WRITE_TIMETABLES_ACTIVITIES)) ||
	
	 ((WRITE_TIMETABLES_DAYS_HORIZONTAL ||
	 WRITE_TIMETABLES_DAYS_VERTICAL ||
	 WRITE_TIMETABLES_TIME_HORIZONTAL ||
	 WRITE_TIMETABLES_TIME_VERTICAL) &&
	 (WRITE_TIMETABLES_SUBGROUPS ||
	 WRITE_TIMETABLES_GROUPS ||
	 WRITE_TIMETABLES_YEARS ||
	 WRITE_TIMETABLES_TEACHERS ||
	 WRITE_TIMETABLES_BUILDINGS ||
	 WRITE_TIMETABLES_ROOMS ||
	 WRITE_TIMETABLES_SUBJECTS ||
	 WRITE_TIMETABLES_ACTIVITY_TAGS ||
	 WRITE_TIMETABLES_ACTIVITIES)) ||
	
	 ((WRITE_TIMETABLES_DAYS_HORIZONTAL ||
	 WRITE_TIMETABLES_DAYS_VERTICAL) &&
	 WRITE_TIMETABLES_TEACHERS_FREE_PERIODS);
	
	 return t;
}

TimetableExport::TimetableExport()
{
}

TimetableExport::~TimetableExport()
{
}

void TimetableExport::getStudentsTeachersRoomsBuildingsTimetable(Solution &c){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);

	c.getSubgroupsTimetable(gt.rules, students_timetable_weekly);
	c.getTeachersTimetable(gt.rules, teachers_timetable_weekly, teachers_free_periods_timetable_weekly);
	c.getRoomsTimetable(gt.rules, rooms_timetable_weekly, virtual_rooms_timetable_weekly);
	c.getBuildingsTimetable(gt.rules, buildings_timetable_weekly);

	best_solution.copy(gt.rules, c);

	students_schedule_ready=true;
	teachers_schedule_ready=true;
	rooms_buildings_schedule_ready=true;
}

void TimetableExport::getNumberOfPlacedActivities(int& number1, int& number2)
{
	number1=0;
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		if(best_solution.times[i]!=UNALLOCATED_TIME)
			number1++;

	number2=0;
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		if(best_solution.rooms[i]!=UNALLOCATED_SPACE)
			number2++;
}

void TimetableExport::writeGenerationResults(QWidget* parent){
	QList<int> subgroupsSortedOrder;
	QList<StudentsSubgroup*> lst;
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++)
		lst.append(gt.rules.internalSubgroupsList[subgroup]);
	if(TIMETABLES_SUBGROUPS_SORTED)
		std::stable_sort(lst.begin(), lst.end(), subgroupsAscending);
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++)
		subgroupsSortedOrder.append(lst.at(subgroup)->indexInInternalSubgroupsList);

	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);
	assert(TIMETABLE_HTML_LEVEL>=0);
	assert(TIMETABLE_HTML_LEVEL<=7);

	computeHashForIDsTimetable();
	computeActivitiesAtTime();
	computeActivitiesWithSameStartingTime();

	QDir dir;
	
	/*QString OUTPUT_DIR_TIMETABLES=OUTPUT_DIR+FILE_SEP+"timetables";
	
	OUTPUT_DIR_TIMETABLES.append(FILE_SEP);
	if(INPUT_FILENAME_XML=="")
		OUTPUT_DIR_TIMETABLES.append("unnamed");
	else{
		OUTPUT_DIR_TIMETABLES.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));
		if(OUTPUT_DIR_TIMETABLES.right(4)==".fet")
			OUTPUT_DIR_TIMETABLES=OUTPUT_DIR_TIMETABLES.left(OUTPUT_DIR_TIMETABLES.length()-4);
		//else if(INPUT_FILENAME_XML!="")
		//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	}
	OUTPUT_DIR_TIMETABLES.append("-single");*/
	
	QString OUTPUT_DIR_TIMETABLES=CURRENT_OUTPUT_DIRECTORY;
	
	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR_TIMETABLES))
		dir.mkpath(OUTPUT_DIR_TIMETABLES);

	QString s;
	QString bar;
	if(INPUT_FILENAME_XML=="")
		bar="";
	else
		bar="_";
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	
	//now write the solution in XML files
	//subgroups
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_FILENAME_XML;
	writeSubgroupsTimetableXml(parent, s, subgroupsSortedOrder);
	//teachers
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_FILENAME_XML;
	writeTeachersTimetableXml(parent, s);
	//activities
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITIES_TIMETABLE_FILENAME_XML;
	writeActivitiesTimetableXml(parent, s);

	//now get the time. TODO: maybe write it in XML too? so do it a few lines earlier!
	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE_WITH_LOCALE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
	generationLocalizedTime=sTime;
	
	//now get the number of placed activities. TODO: maybe write it in XML too? so do it a few lines earlier!
	int na=0;
	int na2=0;
	getNumberOfPlacedActivities(na, na2);
	
	if(na==gt.rules.nInternalActivities && na==na2){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+MULTIPLE_TIMETABLE_DATA_RESULTS_FILE;
		if(VERBOSE){
			cout<<"Since the generation is complete, FET will write also the timetable data file"<<endl;
		}
		writeTimetableDataFile(parent, s);
	}
	
	//write the conflicts in txt mode
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+CONFLICTS_FILENAME;
	writeConflictsTxt(parent, s, sTime, na);
	
	//now write the solution in html files
	if(TIMETABLE_HTML_LEVEL>=1){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+STYLESHEET_CSS;
		writeStylesheetCss(parent, s, sTime, na);
	}
	
	//indexHtml
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+INDEX_HTML;
	writeIndexHtml(parent, s, sTime, na);
	
	//subgroups
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysHorizontalHtml(parent, s, sTime, na, subgroupsSortedOrder);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysVerticalHtml(parent, s, sTime, na, subgroupsSortedOrder);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeHorizontalHtml(parent, s, sTime, na, subgroupsSortedOrder);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeVerticalHtml(parent, s, sTime, na, subgroupsSortedOrder);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na, subgroupsSortedOrder);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeVerticalDailyHtml(parent, s, sTime, na, subgroupsSortedOrder);
	}
	//groups
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeGroupsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeGroupsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeGroupsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeGroupsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeGroupsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeGroupsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//years
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeYearsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeYearsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeYearsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeYearsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeYearsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeYearsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//teachers
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeTeachersTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeTeachersTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeTeachersTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeTeachersTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//rooms
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeRoomsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeRoomsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeRoomsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeRoomsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeRoomsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeRoomsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//buildings
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+BUILDINGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeBuildingsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+BUILDINGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeBuildingsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+BUILDINGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeBuildingsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+BUILDINGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeBuildingsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+BUILDINGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeBuildingsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+BUILDINGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeBuildingsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//subjects
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubjectsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubjectsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubjectsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubjectsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubjectsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubjectsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//activty_tags
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeActivityTagsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeActivityTagsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//all activities
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeAllActivitiesTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeAllActivitiesTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//teachers free periods
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersFreePeriodsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersFreePeriodsTimetableDaysVerticalHtml(parent, s, sTime, na);
	//statistics
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_STATISTICS_FILENAME_HTML;
	writeTeachersStatisticsHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+STUDENTS_STATISTICS_FILENAME_HTML;
	writeStudentsStatisticsHtml(parent, s, sTime, na);

/*
	//needed for printing from the interface, so don't clear them!
	hashSubjectIDsTimetable.clear();
	hashActivityTagIDsTimetable.clear();
	hashStudentIDsTimetable.clear();
	hashTeacherIDsTimetable.clear();
	hashRoomIDsTimetable.clear();
	hashDayIDsTimetable.clear();
	hashActivityColorBySubject.clear();
	hashActivityColorBySubjectAndStudents.clear();
	activeHashActivityColorBySubject.clear();
	activeHashActivityColorBySubjectAndStudents.clear();
*/
	if(VERBOSE){
		cout<<"Writing the generation results to disk completed successfully"<<endl;
	}
}

void TimetableExport::writeHighestStageResults(QWidget* parent){
	QList<int> subgroupsSortedOrder;
	QList<StudentsSubgroup*> lst;
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++)
		lst.append(gt.rules.internalSubgroupsList[subgroup]);
	if(TIMETABLES_SUBGROUPS_SORTED)
		std::stable_sort(lst.begin(), lst.end(), subgroupsAscending);
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++)
		subgroupsSortedOrder.append(lst.at(subgroup)->indexInInternalSubgroupsList);
	QDir dir;
	
	/*QString OUTPUT_DIR_TIMETABLES=OUTPUT_DIR+FILE_SEP+"timetables";
	
	OUTPUT_DIR_TIMETABLES.append(FILE_SEP);
	if(INPUT_FILENAME_XML=="")
		OUTPUT_DIR_TIMETABLES.append("unnamed");
	else{
		OUTPUT_DIR_TIMETABLES.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));
		if(OUTPUT_DIR_TIMETABLES.right(4)==".fet")
			OUTPUT_DIR_TIMETABLES=OUTPUT_DIR_TIMETABLES.left(OUTPUT_DIR_TIMETABLES.length()-4);
		//else if(INPUT_FILENAME_XML!="")
		//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	}
	OUTPUT_DIR_TIMETABLES.append("-highest");*/
	
	QString OUTPUT_DIR_TIMETABLES=HIGHEST_OUTPUT_DIRECTORY;
	
	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR_TIMETABLES))
		dir.mkpath(OUTPUT_DIR_TIMETABLES);

	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);
	assert(TIMETABLE_HTML_LEVEL>=0);
	assert(TIMETABLE_HTML_LEVEL<=7);

	computeHashForIDsTimetable();
	computeActivitiesAtTime();
	computeActivitiesWithSameStartingTime();

	QString s;
	QString bar;
	if(INPUT_FILENAME_XML=="")
		bar="";
	else
		bar="_";
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	
	//now write the solution in XML files
	//subgroups
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_FILENAME_XML;
	writeSubgroupsTimetableXml(parent, s, subgroupsSortedOrder);
	//teachers
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_FILENAME_XML;
	writeTeachersTimetableXml(parent, s);
	//activities
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITIES_TIMETABLE_FILENAME_XML;
	writeActivitiesTimetableXml(parent, s);

	//now get the time. TODO: maybe write it in XML too? so do it a few lines earlier!
	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE_WITH_LOCALE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
	generationLocalizedTime=sTime;
	
	//now get the number of placed activities. TODO: maybe write it in XML too? so do it a few lines earlier!
	int na=0;
	int na2=0;
	getNumberOfPlacedActivities(na, na2);
	
	if(na==gt.rules.nInternalActivities && na==na2){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+MULTIPLE_TIMETABLE_DATA_RESULTS_FILE;
		if(VERBOSE){
			cout<<"Since the generation is complete, FET will write also the timetable data file"<<endl;
		}
		writeTimetableDataFile(parent, s);
	}
	
	//write the conflicts in txt mode
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+CONFLICTS_FILENAME;
	writeConflictsTxt(parent, s, sTime, na);
	
	//now write the solution in html files
	if(TIMETABLE_HTML_LEVEL>=1){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+STYLESHEET_CSS;
		writeStylesheetCss(parent, s, sTime, na);
	}
	
	//indexHtml
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+INDEX_HTML;
	writeIndexHtml(parent, s, sTime, na);
	
	//subgroups
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysHorizontalHtml(parent, s, sTime, na, subgroupsSortedOrder);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysVerticalHtml(parent, s, sTime, na, subgroupsSortedOrder);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeHorizontalHtml(parent, s, sTime, na, subgroupsSortedOrder);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeVerticalHtml(parent, s, sTime, na, subgroupsSortedOrder);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na, subgroupsSortedOrder);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeVerticalDailyHtml(parent, s, sTime, na, subgroupsSortedOrder);
	}
	//groups
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeGroupsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeGroupsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeGroupsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeGroupsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeGroupsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeGroupsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//years
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeYearsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeYearsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeYearsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeYearsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeYearsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeYearsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//teachers
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeTeachersTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeTeachersTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeTeachersTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeTeachersTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//rooms
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeRoomsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeRoomsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeRoomsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeRoomsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeRoomsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeRoomsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//buildings
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+BUILDINGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeBuildingsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+BUILDINGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeBuildingsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+BUILDINGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeBuildingsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+BUILDINGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeBuildingsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+BUILDINGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeBuildingsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+BUILDINGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeBuildingsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//subjects
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubjectsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubjectsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubjectsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubjectsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubjectsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubjectsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//activity_tags
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeActivityTagsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeActivityTagsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//all activities
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeAllActivitiesTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeAllActivitiesTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//teachers free periods
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersFreePeriodsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersFreePeriodsTimetableDaysVerticalHtml(parent, s, sTime, na);
	//statistics
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+TEACHERS_STATISTICS_FILENAME_HTML;
	writeTeachersStatisticsHtml(parent, s, sTime, na);
	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+STUDENTS_STATISTICS_FILENAME_HTML;
	writeStudentsStatisticsHtml(parent, s, sTime, na);

	//needed for printing from the interface, so don't clear them!
/*	hashSubjectIDsTimetable.clear();
	hashActivityTagIDsTimetable.clear();
	hashStudentIDsTimetable.clear();
	hashTeacherIDsTimetable.clear();
	hashRoomIDsTimetable.clear();
	hashDayIDsTimetable.clear();
	hashActivityColorBySubject.clear();
	hashActivityColorBySubjectAndStudents.clear();
	activeHashActivityColorBySubject.clear();
	activeHashActivityColorBySubjectAndStudents.clear();
*/
	if(VERBOSE){
		cout<<"Writing highest stage results to disk completed successfully"<<endl;
	}
}

void TimetableExport::writeRandomSeed(QWidget* parent, const MRG32k3a& rng, bool before)
{
	QString RANDOM_SEED_FILENAME;
	if(before)
		RANDOM_SEED_FILENAME=RANDOM_SEED_FILENAME_BEFORE;
	else
		RANDOM_SEED_FILENAME=RANDOM_SEED_FILENAME_AFTER;

	QDir dir;
	
	/*QString OUTPUT_DIR_TIMETABLES=OUTPUT_DIR+FILE_SEP+"timetables";
	
	OUTPUT_DIR_TIMETABLES.append(FILE_SEP);
	if(INPUT_FILENAME_XML=="")
		OUTPUT_DIR_TIMETABLES.append("unnamed");
	else{
		OUTPUT_DIR_TIMETABLES.append(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1));
		if(OUTPUT_DIR_TIMETABLES.right(4)==".fet")
			OUTPUT_DIR_TIMETABLES=OUTPUT_DIR_TIMETABLES.left(OUTPUT_DIR_TIMETABLES.length()-4);
		//else if(INPUT_FILENAME_XML!="")
		//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	}
	OUTPUT_DIR_TIMETABLES.append("-single");*/
	
	QString OUTPUT_DIR_TIMETABLES=CURRENT_OUTPUT_DIRECTORY;
	
	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR_TIMETABLES))
		dir.mkpath(OUTPUT_DIR_TIMETABLES);

	QString s;
	QString bar;
	if(INPUT_FILENAME_XML=="")
		bar="";
	else
		bar="_";
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);

	s=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+bar+RANDOM_SEED_FILENAME;
	
	writeRandomSeedFile(parent, rng, s, before);
}

void TimetableExport::writeRandomSeedFile(QWidget* parent, const MRG32k3a& rng, const QString& filename, bool before)
{
	QString s=filename;

	QFile file(s);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(s));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE_WITH_LOCALE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
	
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
	if(before){
		tos<<tr("Generation started on: %1", "%1 is the time").arg(sTime);
		tos<<Qt::endl<<Qt::endl;
		tos<<tr("The random seed at the start of generation is:", "The random seed has 6 components, to follow on the next 2 lines (3 values on each line).");
		tos<<Qt::endl;
		tos<<QString("\ts10=%1, s11=%2, s12=%3,").arg(rng.s10).arg(rng.s11).arg(rng.s12);
		tos<<Qt::endl;
		tos<<QString("\ts20=%1, s21=%2, s22=%3.").arg(rng.s20).arg(rng.s21).arg(rng.s22);
		tos<<Qt::endl<<Qt::endl;
		tos<<tr("This file was automatically generated by FET %1.", "%1 is the FET version number").arg(FET_VERSION);
		tos<<Qt::endl;
	}
	else{
		tos<<tr("Generation ended on: %1", "%1 is the time").arg(sTime);
		tos<<Qt::endl<<Qt::endl;
		tos<<tr("The random seed at the end of generation is:", "The random seed has 6 components, to follow on the next 2 lines (3 values on each line).");
		tos<<Qt::endl;
		tos<<QString("\ts10=%1, s11=%2, s12=%3,").arg(rng.s10).arg(rng.s11).arg(rng.s12);
		tos<<Qt::endl;
		tos<<QString("\ts20=%1, s21=%2, s22=%3.").arg(rng.s20).arg(rng.s21).arg(rng.s22);
		tos<<Qt::endl<<Qt::endl;
		tos<<tr("This file was automatically generated by FET %1.", "%1 is the FET version number").arg(FET_VERSION);
		tos<<Qt::endl;
	}
#else
	if(before){
		tos<<tr("Generation started on: %1", "%1 is the time").arg(sTime);
		tos<<endl<<endl;
		tos<<tr("The random seed at the start of generation is:", "The random seed has 6 components, to follow on the next 2 lines (3 values on each line).");
		tos<<endl;
		tos<<QString("\ts10=%1, s11=%2, s12=%3,").arg(rng.s10).arg(rng.s11).arg(rng.s12);
		tos<<endl;
		tos<<QString("\ts20=%1, s21=%2, s22=%3.").arg(rng.s20).arg(rng.s21).arg(rng.s22);
		tos<<endl<<endl;
		tos<<tr("This file was automatically generated by FET %1.", "%1 is the FET version number").arg(FET_VERSION);
		tos<<endl;
	}
	else{
		tos<<tr("Generation ended on: %1", "%1 is the time").arg(sTime);
		tos<<endl<<endl;
		tos<<tr("The random seed at the end of generation is:", "The random seed has 6 components, to follow on the next 2 lines (3 values on each line).");
		tos<<endl;
		tos<<QString("\ts10=%1, s11=%2, s12=%3,").arg(rng.s10).arg(rng.s11).arg(rng.s12);
		tos<<endl;
		tos<<QString("\ts20=%1, s21=%2, s22=%3.").arg(rng.s20).arg(rng.s21).arg(rng.s22);
		tos<<endl<<endl;
		tos<<tr("This file was automatically generated by FET %1.", "%1 is the FET version number").arg(FET_VERSION);
		tos<<endl;
	}
#endif
	
	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(s).arg(file.errorString()));
	}
	file.close();
}

void TimetableExport::writeTimetableDataFile(QWidget* parent, const QString& filename){
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_buildings_schedule_ready){
		IrreconcilableCriticalMessage::critical(parent, tr("FET - Critical"), tr("Timetable not generated - cannot save it - this should not happen (please report bug)"));
		return;
	}

	Solution* tc=&best_solution;

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		//Activity* act=&gt.rules.internalActivitiesList[ai];
		int time=tc->times[ai];
		if(time==UNALLOCATED_TIME){
			IrreconcilableCriticalMessage::critical(parent, tr("FET - Critical"), tr("Incomplete timetable - this should not happen - please report bug"));
			return;
		}
		
		int ri=tc->rooms[ai];
		if(ri==UNALLOCATED_SPACE){
			IrreconcilableCriticalMessage::critical(parent, tr("FET - Critical"), tr("Incomplete timetable - this should not happen - please report bug"));
			return;
		}
	}
	
	rules2.initialized=true;
	
	rules2.mode=gt.rules.mode;
	
	rules2.institutionName=gt.rules.institutionName;
	rules2.comments=gt.rules.comments;
	
	rules2.nTerms=gt.rules.nTerms;
	rules2.nDaysPerTerm=gt.rules.nDaysPerTerm;
	
	rules2.nDaysPerWeek=gt.rules.nDaysPerWeek;
	rules2.daysOfTheWeek=gt.rules.daysOfTheWeek;
	rules2.daysOfTheWeek_longNames=gt.rules.daysOfTheWeek_longNames;
	
	rules2.nRealDaysPerWeek=gt.rules.nRealDaysPerWeek;
	rules2.realDaysOfTheWeek=gt.rules.realDaysOfTheWeek;
	rules2.realDaysOfTheWeek_longNames=gt.rules.realDaysOfTheWeek_longNames;
	
	rules2.nHoursPerDay=gt.rules.nHoursPerDay;
	rules2.hoursOfTheDay=gt.rules.hoursOfTheDay;
	rules2.hoursOfTheDay_longNames=gt.rules.hoursOfTheDay_longNames;

	rules2.nRealHoursPerDay=gt.rules.nRealHoursPerDay;
	rules2.realHoursOfTheDay=gt.rules.realHoursOfTheDay;
	rules2.realHoursOfTheDay_longNames=gt.rules.realHoursOfTheDay_longNames;

	rules2.subjectsList=gt.rules.subjectsList;
	
	rules2.activityTagsList=gt.rules.activityTagsList;

	rules2.teachersList=gt.rules.teachersList;
	
	rules2.yearsList=gt.rules.yearsList;
	
	rules2.activitiesList=gt.rules.activitiesList;

	rules2.buildingsList=gt.rules.buildingsList;

	rules2.roomsList=gt.rules.roomsList;

	rules2.timeConstraintsList=gt.rules.timeConstraintsList;
	
	rules2.spaceConstraintsList=gt.rules.spaceConstraintsList;

	rules2.apstHash=gt.rules.apstHash;
	rules2.aprHash=gt.rules.aprHash;
	
	rules2.groupActivitiesInInitialOrderList=gt.rules.groupActivitiesInInitialOrderList;

	//add locking constraints
	TimeConstraintsList lockTimeConstraintsList;
	SpaceConstraintsList lockSpaceConstraintsList;

	//bool report=false;
	
	//int addedTime=0, duplicatesTime=0;
	//int addedSpace=0, duplicatesSpace=0;

	//lock selected activities
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		Activity* act=&gt.rules.internalActivitiesList[ai];
		int time=tc->times[ai];
		if(time>=0 && time<gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay){
			int hour=time/gt.rules.nDaysPerWeek;
			int day=time%gt.rules.nDaysPerWeek;

			ConstraintActivityPreferredStartingTime* ctr=new ConstraintActivityPreferredStartingTime(100.0, act->id, day, hour, false); //permanently locked is false
			bool t=rules2.addTimeConstraint(ctr);
			
			if(t){
				//addedTime++;
				lockTimeConstraintsList.append(ctr);
			}
			//else
			//	duplicatesTime++;

			QString s;
			
			if(t)
				s=tr("Added the following constraint to saved file:")+"\n"+ctr->getDetailedDescription(gt.rules);
			else{
				s=tr("Constraint\n%1 NOT added to saved file - duplicate").arg(ctr->getDetailedDescription(gt.rules));
				delete ctr;
			}
			
			/*if(report){
				int k;
				if(t)
					k=TimetableExportMessage::information(parent, tr("FET information"), s,
				 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
				else
					k=TimetableExportMessage::warning(parent, tr("FET warning"), s,
				 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
		 		if(k==0)
					report=false;
			}*/
		}
		
		int ri=tc->rooms[ai];
		if(ri!=UNALLOCATED_SPACE && ri!=UNSPECIFIED_ROOM && ri>=0 && ri<gt.rules.nInternalRooms){
			QStringList tl;
			if(gt.rules.internalRoomsList[ri]->isVirtual==false)
				assert(tc->realRoomsList[ai].isEmpty());
			else
				for(int rr : std::as_const(tc->realRoomsList[ai]))
					tl.append(gt.rules.internalRoomsList[rr]->name);
			
			ConstraintActivityPreferredRoom* ctr=new ConstraintActivityPreferredRoom(100, act->id, (gt.rules.internalRoomsList[ri])->name, tl, false); //permanently locked is false
			bool t=rules2.addSpaceConstraint(ctr);

			QString s;
			
			if(t){
				//addedSpace++;
				lockSpaceConstraintsList.append(ctr);
			}
			//else
			//	duplicatesSpace++;

			if(t)
				s=tr("Added the following constraint to saved file:")+"\n"+ctr->getDetailedDescription(gt.rules);
			else{
				s=tr("Constraint\n%1 NOT added to saved file - duplicate").arg(ctr->getDetailedDescription(gt.rules));
				delete ctr;
			}
			
			/*if(report){
				int k;
				if(t)
					k=TimetableExportMessage::information(parent, tr("FET information"), s,
				 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
				else
					k=TimetableExportMessage::warning(parent, tr("FET warning"), s,
					 tr("Skip information"), tr("See next"), QString(), 1, 0 );
				if(k==0)
					report=false;
			}*/
		}
	}

	//QMessageBox::information(parent, tr("FET information"), tr("Added %1 locking time constraints and %2 locking space constraints to saved file,"
	// " ignored %3 activities which were already fixed in time and %4 activities which were already fixed in space").arg(addedTime).arg(addedSpace).arg(duplicatesTime).arg(duplicatesSpace));
	
	bool result=rules2.write(parent, filename);
	
	for(TimeConstraint* tc : std::as_const(lockTimeConstraintsList))
		delete tc;
	lockTimeConstraintsList.clear();
	for(SpaceConstraint* sc : std::as_const(lockSpaceConstraintsList))
		delete sc;
	lockSpaceConstraintsList.clear();
	//while(!lockTimeConstraintsList.isEmpty())
	//	delete lockTimeConstraintsList.takeFirst();
	//while(!lockSpaceConstraintsList.isEmpty())
	//	delete lockSpaceConstraintsList.takeFirst();

	//if(result)
	//	QMessageBox::information(parent, tr("FET information"),
	//		tr("File saved successfully. You can see it on the hard disk. The current data file remained untouched (of locking constraints),"
	//		" so you can save it also, or generate different timetables."));

	rules2.nDaysPerWeek=0;
	rules2.daysOfTheWeek.clear();
	rules2.daysOfTheWeek_longNames.clear();

	rules2.nRealDaysPerWeek=0;
	rules2.realDaysOfTheWeek.clear();
	rules2.realDaysOfTheWeek_longNames.clear();

	rules2.nHoursPerDay=0;
	rules2.hoursOfTheDay.clear();
	rules2.hoursOfTheDay_longNames.clear();

	rules2.nRealHoursPerDay=0;
	rules2.realHoursOfTheDay.clear();
	rules2.realHoursOfTheDay_longNames.clear();

	rules2.subjectsList.clear();
	
	rules2.activityTagsList.clear();

	rules2.teachersList.clear();
	
	rules2.yearsList.clear();
	
	rules2.activitiesList.clear();

	rules2.buildingsList.clear();

	rules2.roomsList.clear();

	rules2.timeConstraintsList.clear();
	
	rules2.spaceConstraintsList.clear();

	rules2.apstHash.clear();
	rules2.aprHash.clear();
	
	rules2.groupActivitiesInInitialOrderList.clear();

	if(!result){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"), tr("Could not save the data + timetable file on the hard disk - maybe hard disk is full"));
	}
}

void TimetableExport::writeGenerationResults(QWidget* parent, int n, bool highest){
	QList<int> subgroupsSortedOrder;
	QList<StudentsSubgroup*> lst;
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++)
		lst.append(gt.rules.internalSubgroupsList[subgroup]);
	if(TIMETABLES_SUBGROUPS_SORTED)
		std::stable_sort(lst.begin(), lst.end(), subgroupsAscending);
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++)
		subgroupsSortedOrder.append(lst.at(subgroup)->indexInInternalSubgroupsList);

	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);
	assert(TIMETABLE_HTML_LEVEL>=0);
	assert(TIMETABLE_HTML_LEVEL<=7);

	computeHashForIDsTimetable();
	computeActivitiesAtTime();
	computeActivitiesWithSameStartingTime();

	QDir dir;
	
	/*QString OUTPUT_DIR_TIMETABLES=OUTPUT_DIR+FILE_SEP+"timetables";

	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR_TIMETABLES))
		dir.mkpath(OUTPUT_DIR_TIMETABLES);

	QString s;
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	
	QString destDir=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+"-multi";*/
	
	QString destDir=MULTIPLE_OUTPUT_DIRECTORY;
	
	if(!dir.exists(destDir))
		dir.mkpath(destDir);
	
	QString finalDestDir=destDir+FILE_SEP+CustomFETString::number(n);
	
	if(highest)
		finalDestDir+=QString("-highest");

	if(!dir.exists(finalDestDir))
		dir.mkpath(finalDestDir);
	
	finalDestDir+=FILE_SEP;

	QString s3=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);

	if(s3.right(4)==".fet")
		s3=s3.left(s3.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	finalDestDir+=s3+"_";
	
	//write data+timetable in .fet format
	if(!highest)
		writeTimetableDataFile(parent, finalDestDir+MULTIPLE_TIMETABLE_DATA_RESULTS_FILE);

	//now write the solution in XML files
	//subgroups
	QString s=finalDestDir+SUBGROUPS_TIMETABLE_FILENAME_XML;
	writeSubgroupsTimetableXml(parent, s, subgroupsSortedOrder);
	//teachers
	s=finalDestDir+TEACHERS_TIMETABLE_FILENAME_XML;
	writeTeachersTimetableXml(parent, s);
	//activities
	s=finalDestDir+ACTIVITIES_TIMETABLE_FILENAME_XML;
	writeActivitiesTimetableXml(parent, s);

	//now get the time. TODO: maybe write it in XML too? so do it a few lines earlier!
	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE_WITH_LOCALE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
	generationLocalizedTime=sTime;

	//now get the number of placed activities. TODO: maybe write it in XML too? so do it a few lines earlier!
	int na=0;
	int na2=0;
	getNumberOfPlacedActivities(na, na2);
	
	//write the conflicts in txt mode
	s=finalDestDir+CONFLICTS_FILENAME;
	writeConflictsTxt(parent, s, sTime, na);
	
	//now write the solution in html files
	if(TIMETABLE_HTML_LEVEL>=1){
		s=finalDestDir+STYLESHEET_CSS;
		writeStylesheetCss(parent, s, sTime, na);
	}
	//indexHtml
	s=finalDestDir+INDEX_HTML;
	writeIndexHtml(parent, s, sTime, na);
	//subgroups
	s=finalDestDir+SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysHorizontalHtml(parent, s, sTime, na, subgroupsSortedOrder);
	s=finalDestDir+SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubgroupsTimetableDaysVerticalHtml(parent, s, sTime, na, subgroupsSortedOrder);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeHorizontalHtml(parent, s, sTime, na, subgroupsSortedOrder);
		s=finalDestDir+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeVerticalHtml(parent, s, sTime, na, subgroupsSortedOrder);
	} else {
		s=finalDestDir+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na, subgroupsSortedOrder);
		s=finalDestDir+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubgroupsTimetableTimeVerticalDailyHtml(parent, s, sTime, na, subgroupsSortedOrder);
	}
	//groups
	s=finalDestDir+GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeGroupsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeGroupsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeGroupsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=finalDestDir+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeGroupsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=finalDestDir+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeGroupsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=finalDestDir+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeGroupsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//years
	s=finalDestDir+YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeYearsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeYearsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeYearsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=finalDestDir+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeYearsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=finalDestDir+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeYearsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=finalDestDir+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeYearsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//teachers
	s=finalDestDir+TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeTeachersTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=finalDestDir+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeTeachersTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=finalDestDir+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeTeachersTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=finalDestDir+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeTeachersTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//rooms
	s=finalDestDir+ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeRoomsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeRoomsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeRoomsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=finalDestDir+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeRoomsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=finalDestDir+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeRoomsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=finalDestDir+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeRoomsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//buildings
	s=finalDestDir+BUILDINGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeBuildingsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+BUILDINGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeBuildingsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+BUILDINGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeBuildingsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=finalDestDir+BUILDINGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeBuildingsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=finalDestDir+BUILDINGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeBuildingsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=finalDestDir+BUILDINGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeBuildingsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//subjects
	s=finalDestDir+SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeSubjectsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeSubjectsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubjectsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=finalDestDir+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubjectsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=finalDestDir+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeSubjectsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=finalDestDir+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeSubjectsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//activity_tags
	s=finalDestDir+ACTIVITY_TAGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeActivityTagsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+ACTIVITY_TAGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeActivityTagsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=finalDestDir+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=finalDestDir+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=finalDestDir+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeActivityTagsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//all activities
	s=finalDestDir+ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeAllActivitiesTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeAllActivitiesTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=finalDestDir+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=finalDestDir+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=finalDestDir+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=finalDestDir+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		writeAllActivitiesTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//teachers free periods
	s=finalDestDir+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	writeTeachersFreePeriodsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=finalDestDir+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	writeTeachersFreePeriodsTimetableDaysVerticalHtml(parent, s, sTime, na);
	//statistics
	s=finalDestDir+TEACHERS_STATISTICS_FILENAME_HTML;
	writeTeachersStatisticsHtml(parent, s, sTime, na);
	s=finalDestDir+STUDENTS_STATISTICS_FILENAME_HTML;
	writeStudentsStatisticsHtml(parent, s, sTime, na);

	//needed for printing from the interface, so don't clear them!
/*	hashSubjectIDsTimetable.clear();
	hashActivityTagIDsTimetable.clear();
	hashStudentIDsTimetable.clear();
	hashTeacherIDsTimetable.clear();
	hashRoomIDsTimetable.clear();
	hashDayIDsTimetable.clear();
	hashActivityColorBySubject.clear();
	hashActivityColorBySubjectAndStudents.clear();
	activeHashActivityColorBySubject.clear();
	activeHashActivityColorBySubjectAndStudents.clear();
*/
	if(VERBOSE){
		cout<<"Writing the multiple generation results to disk completed successfully"<<endl;
	}
}

void TimetableExport::writeRandomSeed(QWidget* parent, const MRG32k3a& rng, int n, bool before){
	QString RANDOM_SEED_FILENAME;
	if(before)
		RANDOM_SEED_FILENAME=RANDOM_SEED_FILENAME_BEFORE;
	else
		RANDOM_SEED_FILENAME=RANDOM_SEED_FILENAME_AFTER;

	QDir dir;
	
	/*QString OUTPUT_DIR_TIMETABLES=OUTPUT_DIR+FILE_SEP+"timetables";

	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR_TIMETABLES))
		dir.mkpath(OUTPUT_DIR_TIMETABLES);

	QString s;
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	
	QString destDir=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+"-multi";*/
	
	QString destDir=MULTIPLE_OUTPUT_DIRECTORY;
	
	if(!dir.exists(destDir))
		dir.mkpath(destDir);
		
	QString finalDestDir=destDir+FILE_SEP+CustomFETString::number(n);

	if(!dir.exists(finalDestDir))
		dir.mkpath(finalDestDir);
		
	finalDestDir+=FILE_SEP;

	QString s3=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);

	if(s3.right(4)==".fet")
		s3=s3.left(s3.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	finalDestDir+=s3+"_";
	
	QString s=finalDestDir+RANDOM_SEED_FILENAME;

	writeRandomSeedFile(parent, rng, s, before);
}

void TimetableExport::writeReportForMultiple(QWidget* parent, const QString& description, bool begin)
{
	QDir dir;
	
	/*QString OUTPUT_DIR_TIMETABLES=OUTPUT_DIR+FILE_SEP+"timetables";

	//make sure that the output directory exists
	if(!dir.exists(OUTPUT_DIR_TIMETABLES))
		dir.mkpath(OUTPUT_DIR_TIMETABLES);

	QString s;
	QString s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(s2.right(4)==".fet")
		s2=s2.left(s2.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	
	QString destDir=OUTPUT_DIR_TIMETABLES+FILE_SEP+s2+"-multi";*/
	
	QString destDir=MULTIPLE_OUTPUT_DIRECTORY;
	
	if(!dir.exists(destDir))
		dir.mkpath(destDir);
		
	QString filename=destDir+FILE_SEP+QString("report.txt");

	QFile file(filename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::Append)){
#else
	if(!file.open(QIODevice::Append)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(filename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	if(begin){
		tos.setGenerateByteOrderMark(true);
	}
	
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
	tos<<description<<Qt::endl;
#else
	tos<<description<<endl;
#endif
	
	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(filename).arg(file.errorString()));
	}
	file.close();
}

void TimetableExport::writeGenerationResultsCommandLine(QWidget* parent, const QString& outputDirectory){ //outputDirectory contains trailing FILE_SEP
	QList<int> subgroupsSortedOrder;
	QList<StudentsSubgroup*> lst;
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++)
		lst.append(gt.rules.internalSubgroupsList[subgroup]);
	if(TIMETABLES_SUBGROUPS_SORTED)
		std::stable_sort(lst.begin(), lst.end(), subgroupsAscending);
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++)
		subgroupsSortedOrder.append(lst.at(subgroup)->indexInInternalSubgroupsList);
	QString add=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(add.right(4)==".fet")
		add=add.left(add.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	if(add!="")
		add.append("_");

	/////////

	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);
	assert(TIMETABLE_HTML_LEVEL>=0);
	assert(TIMETABLE_HTML_LEVEL<=7);

	computeHashForIDsTimetable();
	computeActivitiesAtTime();
	computeActivitiesWithSameStartingTime();

	TimetableExport::writeSubgroupsTimetableXml(parent, outputDirectory+add+SUBGROUPS_TIMETABLE_FILENAME_XML, subgroupsSortedOrder);
	TimetableExport::writeTeachersTimetableXml(parent, outputDirectory+add+TEACHERS_TIMETABLE_FILENAME_XML);
	TimetableExport::writeActivitiesTimetableXml(parent, outputDirectory+add+ACTIVITIES_TIMETABLE_FILENAME_XML);
	
	//get the time
	QDate dat=QDate::currentDate();
	QTime tim=QTime::currentTime();
	QLocale loc(FET_LANGUAGE_WITH_LOCALE);
	QString sTime=loc.toString(dat, QLocale::ShortFormat)+" "+loc.toString(tim, QLocale::ShortFormat);
	generationLocalizedTime=sTime; //really unneeded, but just to be similar to the other parts
	
	//now get the number of placed activities. TODO: maybe write it in XML too? so do it a few lines earlier!
	int na=0;
	int na2=0;
	getNumberOfPlacedActivities(na, na2);
	
	if(na==gt.rules.nInternalActivities && na==na2){
		QString s=outputDirectory+add+MULTIPLE_TIMETABLE_DATA_RESULTS_FILE;
		if(VERBOSE){
			cout<<"Since the generation is complete, FET will write also the timetable data file"<<endl;
		}
		writeTimetableDataFile(parent, s);
	}

	//write the conflicts in txt mode
	QString s=add+CONFLICTS_FILENAME;
	s.prepend(outputDirectory);
	TimetableExport::writeConflictsTxt(parent, s, sTime, na);
	
	//now write the solution in html files
	if(TIMETABLE_HTML_LEVEL>=1){
		s=add+STYLESHEET_CSS;
		s.prepend(outputDirectory);
		TimetableExport::writeStylesheetCss(parent, s, sTime, na);
	}
	//indexHtml
	s=add+INDEX_HTML;
	s.prepend(outputDirectory);
	writeIndexHtml(parent, s, sTime, na);
	//subgroups
	s=add+SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeSubgroupsTimetableDaysHorizontalHtml(parent, s, sTime, na, subgroupsSortedOrder);
	s=add+SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeSubgroupsTimetableDaysVerticalHtml(parent, s, sTime, na, subgroupsSortedOrder);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeSubgroupsTimetableTimeHorizontalHtml(parent, s, sTime, na, subgroupsSortedOrder);
		s=add+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeSubgroupsTimetableTimeVerticalHtml(parent, s, sTime, na, subgroupsSortedOrder);
	} else {
		s=add+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeSubgroupsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na, subgroupsSortedOrder);
		s=add+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeSubgroupsTimetableTimeVerticalDailyHtml(parent, s, sTime, na, subgroupsSortedOrder);
	}
	//groups
	s=add+GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeGroupsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeGroupsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeGroupsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=add+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeGroupsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=add+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeGroupsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=add+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeGroupsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//years
	s=add+YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeYearsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeYearsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeYearsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=add+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeYearsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=add+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeYearsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=add+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeYearsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//teachers
	s=add+TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeTeachersTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeTeachersTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeTeachersTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=add+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeTeachersTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=add+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeTeachersTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=add+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeTeachersTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//rooms
	s=add+ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeRoomsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeRoomsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeRoomsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=add+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeRoomsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=add+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeRoomsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=add+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeRoomsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//buildings
	s=add+BUILDINGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeBuildingsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+BUILDINGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeBuildingsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+BUILDINGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeBuildingsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=add+BUILDINGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeBuildingsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=add+BUILDINGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeBuildingsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=add+BUILDINGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeBuildingsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//subjects
	s=add+SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeSubjectsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeSubjectsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeSubjectsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=add+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeSubjectsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=add+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeSubjectsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=add+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeSubjectsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//activity_tags
	s=add+ACTIVITY_TAGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeActivityTagsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+ACTIVITY_TAGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeActivityTagsTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeActivityTagsTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=add+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeActivityTagsTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=add+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeActivityTagsTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=add+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeActivityTagsTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//all activities
	s=add+ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeAllActivitiesTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeAllActivitiesTimetableDaysVerticalHtml(parent, s, sTime, na);
	if(!DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS){
		s=add+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeAllActivitiesTimetableTimeHorizontalHtml(parent, s, sTime, na);
		s=add+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeAllActivitiesTimetableTimeVerticalHtml(parent, s, sTime, na);
	} else {
		s=add+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeAllActivitiesTimetableTimeHorizontalDailyHtml(parent, s, sTime, na);
		s=add+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML;
		s.prepend(outputDirectory);
		TimetableExport::writeAllActivitiesTimetableTimeVerticalDailyHtml(parent, s, sTime, na);
	}
	//teachers free periods
	s=add+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeTeachersFreePeriodsTimetableDaysHorizontalHtml(parent, s, sTime, na);
	s=add+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeTeachersFreePeriodsTimetableDaysVerticalHtml(parent, s, sTime, na);
	//statistics
	s=add+TEACHERS_STATISTICS_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeTeachersStatisticsHtml(parent, s, sTime, na);
	s=add+STUDENTS_STATISTICS_FILENAME_HTML;
	s.prepend(outputDirectory);
	TimetableExport::writeStudentsStatisticsHtml(parent, s, sTime, na);

	//we can keep it, since it is for the command line version (but in fact we can also clear or delete these lines, since command line doesn't need interface printing)
	/*hashSubjectIDsTimetable.clear();
	hashActivityTagIDsTimetable.clear();
	hashStudentIDsTimetable.clear();
	hashTeacherIDsTimetable.clear();
	hashRoomIDsTimetable.clear();
	hashDayIDsTimetable.clear();
	hashActivityColorBySubject.clear();
	hashActivityColorBySubjectAndStudents.clear();
	activeHashActivityColorBySubject.clear();
	activeHashActivityColorBySubjectAndStudents.clear();*/
}

void TimetableExport::writeRandomSeedCommandLine(QWidget* parent, const MRG32k3a& rng, const QString& outputDirectory, bool before){ //outputDirectory contains trailing FILE_SEP
	QString RANDOM_SEED_FILENAME;
	if(before)
		RANDOM_SEED_FILENAME=RANDOM_SEED_FILENAME_BEFORE;
	else
		RANDOM_SEED_FILENAME=RANDOM_SEED_FILENAME_AFTER;

	QString add=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(add.right(4)==".fet")
		add=add.left(add.length()-4);
	//else if(INPUT_FILENAME_XML!="")
	//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;

	if(add!="")
		add.append("_");

	QString s=add+RANDOM_SEED_FILENAME;
	s.prepend(outputDirectory);
	
	writeRandomSeedFile(parent, rng, s, before);
}

//by Volker Dirr (timetabling.de)
void TimetableExport::writeConflictsTxt(QWidget* parent, const QString& filename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLE_CONFLICTS){
		if(removeOldFiles)
			if(QFile::exists(filename))
				QFile::remove(filename);
	
		return;
	}

	QFile file(filename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(filename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);
	
	if(placedActivities==gt.rules.nInternalActivities){
		QString tt=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
		if(INPUT_FILENAME_XML=="")
			tt=tr("unnamed");
		tos<<TimetableExport::tr("Soft conflicts of %1", "%1 is the file name").arg(tt);
		tos<<"\n";
		tos<<TimetableExport::tr("Generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)<<"\n\n";

		tos<<TimetableExport::tr("Number of broken soft constraints: %1").arg(best_solution.conflictsWeightList.count())<<"\n";
		tos<<TimetableExport::tr("Total soft conflicts: %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(best_solution.conflictsTotal))<<"\n\n";
		tos<<TimetableExport::tr("Soft conflicts list (in decreasing order):")<<"\n\n";
		for(const QString& t : std::as_const(best_solution.conflictsDescriptionList))
			tos<<t<<"\n";
		tos<<"\n"<<TimetableExport::tr("End of file.")<<"\n";
	}
	else{
		QString tt=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
		if(INPUT_FILENAME_XML=="")
			tt=tr("unnamed");
		tos<<TimetableExport::tr("Conflicts of %1").arg(tt);
		tos<<"\n";
		tos<<TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities)<<"\n";
		tos<<TimetableExport::tr("Generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime)<<"\n\n";

		tos<<TimetableExport::tr("Number of broken constraints: %1").arg(best_solution.conflictsWeightList.count())<<"\n";
		tos<<TimetableExport::tr("Total conflicts: %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(best_solution.conflictsTotal))<<"\n\n";
		tos<<TimetableExport::tr("Conflicts list (in decreasing order):")<<"\n\n";
		for(const QString& t : std::as_const(best_solution.conflictsDescriptionList))
			tos<<t<<"\n";
		tos<<"\n"<<TimetableExport::tr("End of file.")<<"\n";
	}
	
	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(filename).arg(file.errorString()));
	}
	file.close();
}

void TimetableExport::writeSubgroupsTimetableXml(QWidget* parent, const QString& xmlfilename, const QList<int>& subgroupsSortedOrder){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_XML || !WRITE_TIMETABLES_SUBGROUPS){
		if(removeOldFiles)
			if(QFile::exists(xmlfilename))
				QFile::remove(xmlfilename);
	
		return;
	}

	//Now we print the results to an XML file
	QFile file(xmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(xmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);
	tos<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	tos<<"<"<<protect(STUDENTS_TIMETABLE_TAG)<<">\n";

	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
		int realSubgroup;
		if(subgroupsSortedOrder!=QList<int>())
			realSubgroup=subgroupsSortedOrder.at(subgroup);
		else
			realSubgroup=subgroup;

		tos<< "  <Subgroup name=\"";
		QString subgroup_name = gt.rules.internalSubgroupsList[realSubgroup]->name;
		tos<< protect(subgroup_name) << "\">\n";

		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			tos<<"    <Day name=\""<<protect(gt.rules.daysOfTheWeek[day])<<"\">\n";
			for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
				tos << "      <Hour name=\"" << protect(gt.rules.hoursOfTheDay[hour]) << "\">\n";
				//tos<<"        ";
				int ai=students_timetable_weekly[realSubgroup][day][hour]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					//Activity* act=gt.rules.activitiesList.at(ai);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tos<<"        <Activity id=\""<<CustomFETString::number(act->id)<<"\"></Activity>\n";
					for(QStringList::const_iterator it=act->teachersNames.constBegin(); it!=act->teachersNames.constEnd(); it++)
						tos<<"        <Teacher name=\""<<protect(*it)<<"\"></Teacher>\n";
					tos<<"        <Subject name=\""<<protect(act->subjectName)<<"\"></Subject>\n";
					for(const QString& atn : std::as_const(act->activityTagsNames))
						tos<<"        <Activity_Tag name=\""<<protect(atn)<<"\"></Activity_Tag>\n";

					int r=best_solution.rooms[ai];
					if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
						tos<<"        <Room name=\""<<protect(gt.rules.internalRoomsList[r]->name)<<"\"></Room>\n";
						if(gt.rules.internalRoomsList[r]->isVirtual==true)
							for(int rr : std::as_const(best_solution.realRoomsList[ai]))
								tos<<"        <Real_Room name=\""<<protect(gt.rules.internalRoomsList[rr]->name)<<"\"></Real_Room>\n";
					}
				}
				//tos<<"\n";
				tos << "      </Hour>\n";
			}
			tos<<"    </Day>\n";
		}
		tos<<"  </Subgroup>\n";
	}

	tos << "</" << protect(STUDENTS_TIMETABLE_TAG) << ">\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(xmlfilename).arg(file.errorString()));
	}
	file.close();
}

void TimetableExport::writeTeachersTimetableXml(QWidget* parent, const QString& xmlfilename){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_XML || !WRITE_TIMETABLES_TEACHERS){
		if(removeOldFiles)
			if(QFile::exists(xmlfilename))
				QFile::remove(xmlfilename);
	
		return;
	}

	//Writing the timetable in XML format
	QFile file(xmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(xmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);
	tos<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	tos << "<" << protect(TEACHERS_TIMETABLE_TAG) << ">\n";

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		tos << "  <Teacher name=\"" << protect(gt.rules.internalTeachersList[i]->name) << "\">\n";
		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			tos << "    <Day name=\"" << protect(gt.rules.daysOfTheWeek[day]) << "\">\n";
			for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
				tos << "      <Hour name=\"" << protect(gt.rules.hoursOfTheDay[hour]) << "\">\n";

				//tos<<"        ";
				int ai=teachers_timetable_weekly[i][day][hour]; //activity index
				//Activity* act=gt.rules.activitiesList.at(ai);
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tos<<"        <Activity id=\""<<CustomFETString::number(act->id)<<"\"></Activity>\n";
					tos<<"        <Subject name=\""<<protect(act->subjectName)<<"\"></Subject>\n";
					for(const QString& atn : std::as_const(act->activityTagsNames))
						tos<<"        <Activity_Tag name=\""<<protect(atn)<<"\"></Activity_Tag>\n";
					for(QStringList::const_iterator it=act->studentsNames.constBegin(); it!=act->studentsNames.constEnd(); it++)
						tos << "        <Students name=\"" << protect(*it) << "\"></Students>\n";

					int r=best_solution.rooms[ai];
					if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
						tos<<"        <Room name=\""<<protect(gt.rules.internalRoomsList[r]->name)<<"\"></Room>\n";
						if(gt.rules.internalRoomsList[r]->isVirtual==true)
							for(int rr : std::as_const(best_solution.realRoomsList[ai]))
								tos<<"        <Real_Room name=\""<<protect(gt.rules.internalRoomsList[rr]->name)<<"\"></Real_Room>\n";
					}
				}
				//tos<<"\n";
				tos << "      </Hour>\n";
			}
			tos << "    </Day>\n";
		}
		tos<<"  </Teacher>\n";
	}

	tos << "</" << protect(TEACHERS_TIMETABLE_TAG) << ">\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(xmlfilename).arg(file.errorString()));
	}
	file.close();
}

void TimetableExport::writeActivitiesTimetableXml(QWidget* parent, const QString& xmlfilename){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_XML || !WRITE_TIMETABLES_ACTIVITIES){
		if(removeOldFiles)
			if(QFile::exists(xmlfilename))
				QFile::remove(xmlfilename);

		return;
	}

	//Writing the timetable in XML format
	QFile file(xmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(xmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);
	tos<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	tos << "<" << protect(ACTIVITIES_TIMETABLE_TAG) << ">\n";
	
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		tos<<"  <Activity>\n";
		
		tos<<"    <Id>"<<gt.rules.internalActivitiesList[i].id<<"</Id>\n";
		
		QString day="";
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			int d=best_solution.times[i]%gt.rules.nDaysPerWeek;
			day=gt.rules.daysOfTheWeek[d];
		}
		tos<<"    <Day>"<<protect(day)<<"</Day>\n";
		
		QString hour="";
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			int h=best_solution.times[i]/gt.rules.nDaysPerWeek;
			hour=gt.rules.hoursOfTheDay[h];
		}
		tos<<"    <Hour>"<<protect(hour)<<"</Hour>\n";
		
		QString room="";
		if(best_solution.rooms[i]!=UNALLOCATED_SPACE && best_solution.rooms[i]!=UNSPECIFIED_ROOM){
			int r=best_solution.rooms[i];
			room=gt.rules.internalRoomsList[r]->name;
		}
		tos<<"    <Room>"<<protect(room)<<"</Room>\n";
		if(best_solution.rooms[i]!=UNALLOCATED_SPACE && best_solution.rooms[i]!=UNSPECIFIED_ROOM){
			int r=best_solution.rooms[i];
			if(gt.rules.internalRoomsList[r]->isVirtual==true){
				assert(gt.rules.internalRoomsList[r]->rrsl.count()==best_solution.realRoomsList[i].count());
				for(int rr : std::as_const(best_solution.realRoomsList[i])){
					assert(rr>=0 && rr<gt.rules.nInternalRooms);
					tos<<"    <Real_Room>"<<protect(gt.rules.internalRoomsList[rr]->name)<<"</Real_Room>\n";
				}
			}
		}
		
		tos<<"  </Activity>\n";
	}

	tos << "</" << protect(ACTIVITIES_TIMETABLE_TAG) << ">\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(xmlfilename).arg(file.errorString()));
	}
	file.close();
}

// writing the index html file by Volker Dirr.
void TimetableExport::writeIndexHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	bool _writeAtLeastATimetable = writeAtLeastATimetable();

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(false, placedActivities, true);

	QString bar;
	QString s2="";
	if(INPUT_FILENAME_XML=="")
		bar="";
	else{
		bar="_";
		s2=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);

		if(s2.right(4)==".fet")
			s2=s2.left(s2.length()-4);
		//else if(INPUT_FILENAME_XML!="")
		//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
	}
	tos<<"    <p>\n";
	
	if(!_writeAtLeastATimetable){
		tos<<"      "<<protect2(TimetableExport::tr("No timetable was written, because from the settings you disabled writing any timetable."))<<"\n";
		tos<<"      "<<protect2(TimetableExport::tr("The exception is that after each successful (complete) timetable generation the %1 file"
		 " will be written.").arg("data_and_timetable.fet"))<<"\n";
		tos<<"    </p>\n";
		tos<<"    <p>\n";
		tos<<"      "<<protect2(TimetableExport::tr("File generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))<<"\n";
		tos<<"    </p>\n";
	}
	else{
		if(WRITE_TIMETABLE_CONFLICTS)
			tos<<"      <a href=\""<<s2+bar+CONFLICTS_FILENAME<<"\">"<<protect2(tr("View the soft conflicts list."))<<"</a><br />\n";
		else
			tos<<"      "<<protect2(TimetableExport::tr("Soft conflicts list - disabled."))<<"<br />\n";

		///////////////////////////

		QString tmps1, tmps2;
		if(WRITE_TIMETABLES_STATISTICS && (WRITE_TIMETABLES_YEARS || WRITE_TIMETABLES_GROUPS || WRITE_TIMETABLES_SUBGROUPS) )
			tmps1="      <a href=\""+s2+bar+STUDENTS_STATISTICS_FILENAME_HTML+"\">"+protect2(tr("students"))+"</a>";
		else
			tmps1=protect2(tr("students - disabled"));
		
		if(WRITE_TIMETABLES_STATISTICS && WRITE_TIMETABLES_TEACHERS)
			tmps2="<a href=\""+s2+bar+TEACHERS_STATISTICS_FILENAME_HTML+"\">"+protect2(tr("teachers"))+"</a>";
		else
			tmps2=protect2(tr("teachers - disabled"));
			
		QString tmps3=protect2(tr("View statistics: %1, %2.", "%1 and %2 are two files in HTML format, to show statistics for students and teachers. The user can click on one file to view it"))
		 .arg(tmps1).arg(tmps2);

		tos<<"      "<<tmps3<<"<br />\n";
		
		///////////////////////////
		
		QString tmp1, tmp2, tmp3;
		if(WRITE_TIMETABLES_XML && WRITE_TIMETABLES_SUBGROUPS)
			tmp1="<a href=\""+s2+bar+SUBGROUPS_TIMETABLE_FILENAME_XML+"\">"+protect2(tr("subgroups"))+"</a>";
		else
			tmp1=protect2(tr("subgroups - disabled", "It means the subgroups XML timetables are disabled"));
		if(WRITE_TIMETABLES_XML && WRITE_TIMETABLES_TEACHERS)
			tmp2="<a href=\""+s2+bar+TEACHERS_TIMETABLE_FILENAME_XML+"\">"+protect2(tr("teachers"))+"</a>";
		else
			tmp2=protect2(tr("teachers - disabled", "It means the teachers XML timetables are disabled"));
		if(WRITE_TIMETABLES_XML && WRITE_TIMETABLES_ACTIVITIES)
			tmp3="<a href=\""+s2+bar+ACTIVITIES_TIMETABLE_FILENAME_XML+"\">"+protect2(tr("activities"))+"</a>";
		else
			tmp3=protect2(tr("activities - disabled", "It means the activities XML timetables are disabled"));
		QString tmp4=protect2(TimetableExport::tr("View XML: %1, %2, %3.", "%1, %2, and %3 are three files in XML format, subgroups, teachers and activities timetables. The user can click on one file to view it")).arg(tmp1).arg(tmp2).arg(tmp3);
		tos<<"      "<<tmp4<<"\n";

		tos<<"    </p>\n";

		tos<<"    <table border=\"1\">\n";

		tos<<"      <caption>"<<protect2(gt.rules.institutionName)<<"</caption>\n";

		tos<<"      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\"4\">"+protect2(tr("Timetables"))+"</th></tr>\n";
		tos<<"        <tr>\n          <!-- span -->\n";
		tos<<"          <th>"+protect2(tr("Days Horizontal"))+"</th><th>"+protect2(tr("Days Vertical"))+"</th><th>"+protect2(tr("Time Horizontal"))+"</th><th>"+protect2(tr("Time Vertical"))+"</th>\n";
		tos<<"        </tr>\n";
		tos<<"      </thead>\n";
		tos<<"      <tbody>\n";
		
		/* workaround
		tos<<"      <tfoot><tr><td></td><td colspan=\"4\">"<<protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))<<"</td></tr></tfoot>\n";
		*/

		tos<<"        <tr>\n";
		tos<<"          <th>"+protect2(tr("Subgroups"))+"</th>\n";
		if(WRITE_TIMETABLES_SUBGROUPS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		} else {
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		}
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+protect2(tr("Groups"))+"</th>\n";
		if(WRITE_TIMETABLES_GROUPS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		} else {
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		}
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+protect2(tr("Years"))+"</th>\n";
		if(WRITE_TIMETABLES_YEARS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		} else {
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		}
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+protect2(tr("Teachers"))+"</th>\n";
		if(WRITE_TIMETABLES_TEACHERS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		} else {
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		}
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+protect2(tr("Teachers' Free Periods"))+"</th>\n";
		if(WRITE_TIMETABLES_TEACHERS_FREE_PERIODS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		} else {
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		}
		tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
		tos<<"          <td>"<<protect2(STRING_EMPTY_SLOT)<<"</td>\n";
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+protect2(tr("Rooms"))+"</th>\n";
		if(WRITE_TIMETABLES_ROOMS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		} else {
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		}
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+protect2(tr("Buildings"))+"</th>\n";
		if(WRITE_TIMETABLES_BUILDINGS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+BUILDINGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+BUILDINGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+BUILDINGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+BUILDINGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		} else {
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		}
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+protect2(tr("Subjects"))+"</th>\n";
		if(WRITE_TIMETABLES_SUBJECTS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		} else {
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		}
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+protect2(tr("Activity Tags"))+"</th>\n";
		if(WRITE_TIMETABLES_ACTIVITY_TAGS){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+ACTIVITY_TAGS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+ACTIVITY_TAGS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+ACTIVITY_TAGS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		} else {
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		}
		tos<<"        </tr>\n";
		tos<<"        <tr>\n";
		tos<<"          <th>"+protect2(tr("Activities"))+"</th>\n";
		if(WRITE_TIMETABLES_ACTIVITIES){
			if(WRITE_TIMETABLES_DAYS_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_DAYS_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_HORIZONTAL)
				tos<<"          <td><a href=\""<<s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			if(WRITE_TIMETABLES_TIME_VERTICAL)
				tos<<"          <td><a href=\""<<s2+bar+ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML<<"\">"+protect2(tr("view"))+"</a></td>\n";
			else
				tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		} else {
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
			tos<<"          <td>"+protect2(tr("disabled"))+"</td>\n";
		}
		tos<<"        </tr>\n";
		//workaround begin.
		tos<<"        <tr class=\"foot\"><td></td><td colspan=\"4\">"<<protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))<<"</td></tr>\n";
		//workaround end.
		tos<<"      </tbody>\n";
		tos<<"    </table>\n";
	}

	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

// writing the style sheet in CSS format to a file by Volker Dirr.
void TimetableExport::writeStylesheetCss(QWidget* parent, const QString& cssfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);
	
	bool _writeAtLeastATimetable = writeAtLeastATimetable();

	if(!_writeAtLeastATimetable){
		if(removeOldFiles)
			if(QFile::exists(cssfilename))
				QFile::remove(cssfilename);

		return;
	}

	//get used students	//TODO: do it the same way in statistics.cpp
	QSet<QString> usedStudents;
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		for(const QString& st : std::as_const(gt.rules.internalActivitiesList[i].studentsNames)){
			if(!usedStudents.contains(st))
				usedStudents<<st;
		}
	}

	//Now we print the results to a CSS file
	QFile file(cssfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(cssfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);
	
	tos<<"@charset \"UTF-8\";"<<"\n\n";

	QString tt=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
	if(INPUT_FILENAME_XML=="")
		tt=tr("unnamed");
	tos<<"/* "<<protect3(TimetableExport::tr("CSS Style sheet of %1", "%1 is the file name").arg(tt));
	tos<<"\n";
	if(placedActivities!=gt.rules.nInternalActivities)
		tos<<"   "<<protect3(TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities))<<"\n";
	tos<<"   "<<protect3(TimetableExport::tr("Style sheet generated with FET %1 on %2", "%1 is FET version, %2 is date and time").arg(FET_VERSION).arg(saveTime))<<" */\n\n";

	tos<<"/* "<<protect3(TimetableExport::tr("To do a page-break only after every second timetable, delete \"page-break-before: always;\" in \"table.even_table\".",
		"Please keep fields in quotes as they are, untranslated."))<<" */\n";
	tos<<"/* "<<protect3(TimetableExport::tr("Delete \"page-break-before: always;\" in \"table.even_table\" and in \"table.odd_table\" to skip page-breaks.",
		"Please keep fields in quotes as they are, untranslated."))<<" */\n";
	tos<<"/* "<<protect3(TimetableExport::tr("To hide an element just write the following phrase into the element: %1 (without quotes).",
		"%1 is a short phrase beginning and ending with quotes, and we want the user to be able to add it, but without quotes").arg("\"display: none;\""))<<" */\n\n";
	tos<<"p.back {\n  margin-top: 4ex;\n  margin-bottom: 5ex;\n}\n\n";
	tos<<"table {\n  text-align: center;\n  page-break-inside: avoid;\n}\n\n";
	tos<<"table.odd_table {\n  page-break-before: always;\n}\n\n";
	tos<<"table.even_table {\n  page-break-before: always;\n}\n\n";
	tos<<"table.detailed {\n  margin-left: auto; margin-right: auto;\n  text-align: center;\n  border: 0px;\n  border-spacing: 0;\n  border-collapse: collapse;\n}\n\n";
	tos<<"caption {\n\n}\n\n";
	tos<<"span.institution {\n  font-weight: bold;\n}\n\n";
	tos<<"span.name {\n\n}\n\n";
	tos<<"span.comment {\n  /*font-style: italic;*/\n}\n\n";
	tos<<"span.legend_title {\n  font-weight: bold;\n}\n\n";
	tos<<"thead {\n\n}\n\n";
	
	//workaround begin.
	tos<<"/* "<<protect3(TimetableExport::tr("Some programs import \"tfoot\" incorrectly. So we use \"tr.foot\" instead of \"tfoot\".",
	 "Please keep tfoot and tr.foot untranslated, as they are in the original English phrase"))<<" */\n\n";
	//tos<<"tfoot {\n\n}\n\n";
	tos<<"tr.foot {\n\n}\n\n";
	//workaround end

	tos<<"tbody {\n\n}\n\n";
	tos<<"th {\n\n}\n\n";
	tos<<"td {\n\n}\n\n";
	tos<<"td.detailed {\n  border: 1px dashed silver;\n  border-bottom: 0;\n  border-top: 0;\n}\n\n";
	if(TIMETABLE_HTML_LEVEL>=2){
		tos<<"th.xAxis {\n/* width: 8em; */\n}\n\n";
		tos<<"th.yAxis {\n  height: 8ex;\n}\n\n";
	}
	
	//By Liviu, with ideas from Volker
	if(TIMETABLE_HTML_LEVEL==7){ //must be written before LEVEL 3, because LEVEL 3 should have higher priority
		int cnt=0;
		for(int i : std::as_const(activeHashActivityColorBySubject)){
			Activity* act=&gt.rules.internalActivitiesList[i];
			
			QString tmpString=act->subjectName;
			
			//similar to the coloring by Marco Vassura (start)
			int r,g,b;
			stringToColor(tmpString, r, g, b);
			tos << "td.c_"<<QString::number(cnt+1)<<" { /* Activity id: "<<QString::number(act->id)<<" (subject) */\n ";
			tos<<"background-color: rgb("<<QString::number(r)<<", "<<QString::number(g)<<", "<<QString::number(b)<<");\n";
			double brightness = double(r)*0.299 + double(g)*0.587 + double(b)*0.114;
			if (brightness<127.5)
				tos<<" color: white;\n";
			else
				tos<<" color: black;\n";
			tos<<"}\n\n";
			//similar to the coloring by Marco Vassura (end)
			cnt++;
		}
		for(int i : std::as_const(activeHashActivityColorBySubjectAndStudents)){
			Activity* act=&gt.rules.internalActivitiesList[i];
			
			QString tmpString=act->subjectName+" "+act->studentsNames.join(", ");
			
			//similar to the coloring by Marco Vassura (start)
			int r,g,b;
			stringToColor(tmpString, r, g, b);
			tos << "td.c_"<<QString::number(cnt+1)<<" { /* Activity id: "<<QString::number(act->id)<<" (subject+students) */\n ";
			tos<<"background-color: rgb("<<QString::number(r)<<", "<<QString::number(g)<<", "<<QString::number(b)<<");\n";
			double brightness = double(r)*0.299 + double(g)*0.587 + double(b)*0.114;
			if (brightness<127.5)
				tos<<" color: white;\n";
			else
				tos<<" color: black;\n";
			tos<<"}\n\n";
			//similar to the coloring by Marco Vassura (end)
			cnt++;
		}
	}
//	if(TIMETABLE_HTML_LEVEL==7){ // must be written before LEVEL 3, because LEVEL 3 should have higher priority
//		QHashIterator<QString, QString> i(hashColorStringIDsTimetable);
//		while(i.hasNext()) {
//			i.next();
//			tos << "td.c_"<<i.value()<<" { /* "<<i.key()<<" */\n ";
//			
//			//similar to the coloring by Marco Vassura (start)
//			int r, g, b;
//			stringToColor(i.key(), r, g, b);
//			tos<<"background-color: rgb("<<QString::number(r)<<", "<<QString::number(g)<<", "<<QString::number(b)<<");\n";
//			double brightness = double(r)*0.299 + double(g)*0.587 + double(b)*0.114;
//			if (brightness<127.5)
//				tos<<" color: white;\n";
//			else
//				tos<<" color: black;\n";
//			//similar to the coloring by Marco Vassura (end)
//			tos<<"}\n\n";
//		}
//	}
	else if(TIMETABLE_HTML_LEVEL>=4){ // must be written before LEVEL 3, because LEVEL 3 should have higher priority
		for(int i=0; i<gt.rules.nInternalSubjects; i++){
			tos << "span.s_"<<hashSubjectIDsTimetable.value(gt.rules.internalSubjectsList[i]->name)<<" { /* subject "<<protect3(gt.rules.internalSubjectsList[i]->name)<<" */\n\n}\n\n";
		}
		for(int i=0; i<gt.rules.nInternalActivityTags; i++){
			if(gt.rules.internalActivityTagsList[i]->printable){
				tos << "span.at_"<<hashActivityTagIDsTimetable.value(gt.rules.internalActivityTagsList[i]->name)<<" { /* activity tag "<<protect3(gt.rules.internalActivityTagsList[i]->name)<<" */\n\n}\n\n";
			}
		}
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			if(usedStudents.contains(sty->name))
				tos << "span.ss_"<<hashStudentIDsTimetable.value(sty->name)<<" { /* students set "<<protect3(sty->name)<<" */\n\n}\n\n";
			for(int j=0; j<sty->groupsList.size(); j++){
				StudentsGroup* stg=sty->groupsList[j];
				if(usedStudents.contains(stg->name))
					tos << "span.ss_"<<hashStudentIDsTimetable.value(stg->name)<<" { /* students set "<<protect3(stg->name)<<" */\n\n}\n\n";
				for(int k=0; k<stg->subgroupsList.size(); k++){
					StudentsSubgroup* sts=stg->subgroupsList[k];
					if(usedStudents.contains(sts->name))
						tos << "span.ss_"<<hashStudentIDsTimetable.value(sts->name)<<" { /* students set "<<protect3(sts->name)<<" */\n\n}\n\n";
				}
			}
		}
		for(int i=0; i<gt.rules.nInternalTeachers; i++){
			tos << "span.t_"<<hashTeacherIDsTimetable.value(gt.rules.internalTeachersList[i]->name)<<" { /* teacher "<<protect3(gt.rules.internalTeachersList[i]->name)<<" */\n\n}\n\n";
		}
		for(int room=0; room<gt.rules.nInternalRooms; room++){
			tos << "span.r_"<<hashRoomIDsTimetable.value(gt.rules.internalRoomsList[room]->name)<<" { /* room "<<protect3(gt.rules.internalRoomsList[room]->name)<<" */\n\n}\n\n";
		}
	}
	if(TIMETABLE_HTML_LEVEL>=3){
		tos<<"span.subject {\n\n}\n\n";
		tos<<"span.activitytag {\n\n}\n\n";

		tos<<"span.empty {\n  color: gray;\n}\n\n";
		tos<<"td.empty {\n  border-color: silver;\n  border-right-style: none;\n  border-bottom-style: none;\n  border-left-style: dotted;\n  border-top-style: dotted;\n}\n\n";

		tos<<"span.notAvailable {\n  color: gray;\n}\n\n";
		tos<<"td.notAvailable {\n  border-color: silver;\n  border-right-style: none;\n  border-bottom-style: none;\n  border-left-style: dotted;\n  border-top-style: dotted;\n}\n\n";

		tos<<"span.break {\n  color: gray;\n}\n\n";
		tos<<"td.break {\n  border-color: silver;\n  border-right-style: none;\n  border-bottom-style: none;\n  border-left-style: dotted;\n  border-top-style: dotted;\n}\n\n";

		tos<<"tr.studentsset, div.studentsset {\n\n}\n\n";
		tos<<"tr.teacher, div.teacher {\n\n}\n\n";
		tos<<"tr.room, div.room {\n\n}\n\n";
		if(TIMETABLE_HTML_LEVEL!=7){
			tos<<"tr.line0, div.line0 {\n  font-size: smaller;\n}\n\n";
			tos<<"tr.line1, div.line1 {\n\n}\n\n";
			tos<<"tr.line2, div.line2 {\n  font-size: smaller;\n  color: gray;\n}\n\n";
			tos<<"tr.line3, div.line3 {\n  font-size: smaller;\n  color: silver;\n}\n\n";
		} else {
			tos<<"tr.line0, div.line0 {\n  font-size: smaller;\n}\n\n";
			tos<<"tr.line1, div.line1 {\n\n}\n\n";
			tos<<"tr.line2, div.line2 {\n  font-size: smaller;\n}\n\n";
			tos<<"tr.line3, div.line3 {\n  font-size: smaller;\n}\n\n";
		}
	}
	if(TIMETABLE_HTML_LEVEL==6){
		tos<<"/* "<<protect3(TimetableExport::tr("Be careful. You might get mutual and ambiguous styles. CSS means that the last definition will be used."))<<" */\n\n";
		for(int i=0; i<gt.rules.nInternalSubjects; i++){
			tos << "td.s_"<<hashSubjectIDsTimetable.value(gt.rules.internalSubjectsList[i]->name)<<" { /* subject "<<protect3(gt.rules.internalSubjectsList[i]->name)<<" */\n\n}\n\n";
		}
		for(int i=0; i<gt.rules.nInternalActivityTags; i++){
			if(gt.rules.internalActivityTagsList[i]->printable){
				tos << "td.at_"<<hashActivityTagIDsTimetable.value(gt.rules.internalActivityTagsList[i]->name)<<" { /* activity tag "<<protect3(gt.rules.internalActivityTagsList[i]->name)<<" */\n\n}\n\n";
			}
		}
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			if(usedStudents.contains(sty->name))
				tos << "td.ss_"<<hashStudentIDsTimetable.value(sty->name)<<" { /* students set "<<protect3(sty->name)<<" */\n\n}\n\n";
			for(int j=0; j<sty->groupsList.size(); j++){
				StudentsGroup* stg=sty->groupsList[j];
				if(usedStudents.contains(stg->name))
					tos << "td.ss_"<<hashStudentIDsTimetable.value(stg->name)<<" { /* students set "<<protect3(stg->name)<<" */\n\n}\n\n";
				for(int k=0; k<stg->subgroupsList.size(); k++){
					StudentsSubgroup* sts=stg->subgroupsList[k];
					if(usedStudents.contains(sts->name))
						tos << "td.ss_"<<hashStudentIDsTimetable.value(sts->name)<<" { /* students set "<<protect3(sts->name)<<" */\n\n}\n\n";
				}
			}
		}
		for(int i=0; i<gt.rules.nInternalTeachers; i++){
			tos << "td.t_"<<hashTeacherIDsTimetable.value(gt.rules.internalTeachersList[i]->name)<<" { /* teacher "<<protect3(gt.rules.internalTeachersList[i]->name)<<" */\n\n}\n\n";
		}

		//not included yet
		//for(int room=0; room<gt.rules.nInternalRooms; room++){
		//	tos << "span.r_"<<hashRoomIDsTimetable.value(gt.rules.internalRoomsList[room]->name)<<" { /* room "<<gt.rules.internalRoomsList[room]->name<<" */\n\n}\n\n";
		//}
	}
	tos<<"/* "<<protect3(TimetableExport::tr("Style the teachers' free periods"))<<" */\n\n";
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
	tos<<"/* "<<protect3(TimetableExport::tr("End of file."))<<" */\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(cssfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
void TimetableExport::writeSubgroupsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities, const QList<int>& subgroupsSortedOrder){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_SUBGROUPS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	
	tos<<"    <p><strong>"<<protect2(TimetableExport::tr("Table of contents"))<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		tos<<"      <li>\n        "<<protect2(TimetableExport::tr("Year"))<<" "<<getStudentsSetString(sty, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_CODES)<<"\n        <ul>\n";
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			tos<<"          <li>\n            "<<protect2(TimetableExport::tr("Group"))<<" "<<getStudentsSetString(stg, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_CODES)<<":\n";
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				tos<<"              <a href=\""<<"#table_"<<hashStudentIDsTimetable.value(sts->name)<<"\">"<<getStudentsSetString(sts, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_CODES)<<"</a>\n";
			}
			tos<<"          </li>\n";
		}
		tos<<"        </ul>\n      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n";

	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
		tos<<singleSubgroupsTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, subgroup, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES, subgroupsSortedOrder);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
void TimetableExport::writeSubgroupsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities, const QList<int>& subgroupsSortedOrder){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_SUBGROUPS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	
	tos<<"    <p><strong>"<<protect2(TimetableExport::tr("Table of contents"))<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		tos<<"      <li>\n        "<<protect2(TimetableExport::tr("Year"))<<" "<<getStudentsSetString(sty, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_CODES)<<"\n        <ul>\n";
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			tos<<"          <li>\n            "<<protect2(TimetableExport::tr("Group"))<<" "<<getStudentsSetString(stg, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_CODES)<<":\n";
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				tos<<"              <a href=\""<<"#table_"<<hashStudentIDsTimetable.value(sts->name)<<"\">"<<getStudentsSetString(sts, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_CODES)<<"</a>\n";
			}
			tos<<"          </li>\n";
		}
		tos<<"        </ul>\n      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n";

	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
		tos<<singleSubgroupsTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, subgroup, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES, subgroupsSortedOrder);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}

	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeSubgroupsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities, const QList<int>& subgroupsSortedOrder){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_SUBGROUPS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	QSet<int> tmp;
	tos << singleSubgroupsTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalSubgroups, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES, subgroupsSortedOrder);

	tos << printCompleteLegend(SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
void TimetableExport::writeSubgroupsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities, const QList<int>& subgroupsSortedOrder){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_SUBGROUPS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	QSet<int> tmp;
	tos << singleSubgroupsTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalSubgroups, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES, subgroupsSortedOrder);

	tos << printCompleteLegend(SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

// by Volker Dirr
void TimetableExport::writeSubgroupsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities, const QList<int>& subgroupsSortedOrder){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_SUBGROUPS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
		QSet<int> tmp;
		tos<<singleSubgroupsTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalSubgroups, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES, subgroupsSortedOrder);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

// by Volker Dirr
void TimetableExport::writeSubgroupsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities, const QList<int>& subgroupsSortedOrder){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_SUBGROUPS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES);
	
	for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
		QSet<int> tmp;
		tos<<singleSubgroupsTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalSubgroups, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES, subgroupsSortedOrder);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LEGEND,
		 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//Now print the groups
//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeGroupsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_GROUPS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	
	tos<<"    <p><strong>"<<protect2(TimetableExport::tr("Table of contents"))<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		tos<<"      <li>\n        "<<protect2(TimetableExport::tr("Year"))<<" "<<getStudentsSetString(sty, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_CODES)<<"\n        <ul>\n";
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			tos<<"          <li>\n            "<<protect2(TimetableExport::tr("Group"));
			tos<<" <a href=\""<<"#table_"<<hashStudentIDsTimetable.value(stg->name)<<"\">"<<getStudentsSetString(stg, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_CODES)<<"</a>\n";
			tos<<"          </li>\n";
		}
		tos<<"        </ul>\n      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n";

	for(int group=0; group<gt.rules.internalGroupsList.size(); group++){
		tos << singleGroupsTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, group, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}
	
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeGroupsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_GROUPS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	
	tos<<"    <p><strong>"<<protect2(TimetableExport::tr("Table of contents"))<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		tos<<"      <li>\n        "<<protect2(TimetableExport::tr("Year"))<<" "<<getStudentsSetString(sty, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_CODES)<<"\n        <ul>\n";
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			tos<<"          <li>\n            "<<protect2(TimetableExport::tr("Group"));
			tos<<" <a href=\""<<"#table_"<<hashStudentIDsTimetable.value(stg->name)<<"\">"<<getStudentsSetString(stg, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_CODES)<<"</a>\n";
			tos<<"          </li>\n";
		}
		tos<<"        </ul>\n      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n";

	for(int group=0; group<gt.rules.internalGroupsList.size(); group++){
		tos<<singleGroupsTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, group, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}
	
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeGroupsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_GROUPS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	QSet<int> tmp;
	tos<<singleGroupsTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, gt.rules.internalGroupsList.size(), tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);

	tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";

	tos << printCompleteLegend(SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_LEGEND_CODES_FIRST);
	
	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeGroupsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_GROUPS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	QSet<int> tmp;
	tos<<singleGroupsTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, gt.rules.internalGroupsList.size(), tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
	tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	
	tos << printCompleteLegend(SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeGroupsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_GROUPS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
		QSet<int> tmp;
		tos<<singleGroupsTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.internalGroupsList.size(), tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}
	
	tos << printCompleteLegend(SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeGroupsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_GROUPS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES);
	
	for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
		QSet<int> tmp;
		tos<<singleGroupsTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.internalGroupsList.size(), tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//Now print the years

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeYearsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_YEARS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	
	tos<<"    <p><strong>"<<protect2(TimetableExport::tr("Table of contents"))<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int year=0; year<gt.rules.augmentedYearsList.size(); year++){
		StudentsYear* sty=gt.rules.augmentedYearsList[year];
		tos<<"      <li>\n        "<<protect2(TimetableExport::tr("Year"));
		tos<<" <a href=\""<<"#table_"<<hashStudentIDsTimetable.value(sty->name)<<"\">"<<getStudentsSetString(sty, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_CODES)<<"</a>\n";
		tos<<"      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n";

	for(int year=0; year<gt.rules.augmentedYearsList.size(); year++){
		tos << singleYearsTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, year, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}
	
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeYearsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_YEARS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	
	tos<<"    <p><strong>"<<protect2(TimetableExport::tr("Table of contents"))<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int year=0; year<gt.rules.augmentedYearsList.size(); year++){
		StudentsYear* sty=gt.rules.augmentedYearsList[year];
		tos<<"      <li>\n        "<<protect2(TimetableExport::tr("Year"));
		tos<<" <a href=\""<<"#table_"<<hashStudentIDsTimetable.value(sty->name)<<"\">"<<getStudentsSetString(sty, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_CODES)<<"</a>\n";
		tos<<"      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n";

	for(int year=0; year<gt.rules.augmentedYearsList.size(); year++){
		tos << singleYearsTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, year, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}
	
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeYearsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_YEARS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	QSet<int> tmp;
	tos<<singleYearsTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, gt.rules.augmentedYearsList.size(), tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);

	tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";

	tos << printCompleteLegend(SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_LEGEND_CODES_FIRST);
	
	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeYearsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_YEARS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	QSet<int> tmp;
	tos<<singleYearsTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, gt.rules.augmentedYearsList.size(), tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);

	tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	
	tos << printCompleteLegend(SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeYearsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_YEARS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
		QSet<int> tmp;
		tos<<singleYearsTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.augmentedYearsList.size(), tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}
	
	tos << printCompleteLegend(SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeYearsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_YEARS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES);
	
	for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
		QSet<int> tmp;
		tos<<singleYearsTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.augmentedYearsList.size(), tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 PRINT_DETAILED_HTML_TIMETABLES, TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}
	
	tos << printCompleteLegend(SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//Print all activities

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeAllActivitiesTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_ACTIVITIES){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<singleAllActivitiesTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeAllActivitiesTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_ACTIVITIES){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<singleAllActivitiesTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeAllActivitiesTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_ACTIVITIES){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);

	tos<<singleAllActivitiesTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

	tos << printCompleteLegend(SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeAllActivitiesTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_ACTIVITIES){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);

	tos<<singleAllActivitiesTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

	tos << printCompleteLegend(SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeAllActivitiesTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_ACTIVITIES){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES);

	for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
		tos<<singleAllActivitiesTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeAllActivitiesTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_ACTIVITIES){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES);

	for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
		tos<<singleAllActivitiesTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//Print the teachers

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
void TimetableExport::writeTeachersTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_TEACHERS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);
	
	tos<<writeHead(true, placedActivities, true);
	
	tos<<"    <p><strong>"<<protect2(TimetableExport::tr("Table of contents"))<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
		QString teacher_name = gt.rules.internalTeachersList[teacher]->name;
		tos<<"      <li><a href=\""<<"#table_"<<hashTeacherIDsTimetable.value(teacher_name)<<"\">"<<getTeacherString(gt.rules.internalTeachersList[teacher], SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_CODES)<<"</a></li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n";

	for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
		tos<<singleTeachersTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, teacher, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
void TimetableExport::writeTeachersTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_TEACHERS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	
	tos<<"    <p><strong>"<<protect2(TimetableExport::tr("Table of contents"))<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
		QString teacher_name = gt.rules.internalTeachersList[teacher]->name;
		tos<<"      <li><a href=\""<<"#table_"<<hashTeacherIDsTimetable.value(teacher_name)<<"\">"<<getTeacherString(gt.rules.internalTeachersList[teacher], SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_CODES)<<"</a></li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n";

	for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
		tos<<singleTeachersTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, teacher, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeTeachersTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_TEACHERS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	QSet<int> tmp;
	tos<<singleTeachersTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalTeachers, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

	tos << printCompleteLegend(SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code modified by Volker Dirr (timetabling.de) from old html generation code
void TimetableExport::writeTeachersTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_TEACHERS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	QSet<int> tmp;
	tos<<singleTeachersTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalTeachers, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

	tos << printCompleteLegend(SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//by Volker Dirr
void TimetableExport::writeTeachersTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_TEACHERS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
		QSet<int> tmp;
		tos<<singleTeachersTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalTeachers, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//by Volker Dirr
void TimetableExport::writeTeachersTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_TEACHERS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
		QSet<int> tmp;
		tos<<singleTeachersTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalTeachers, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//writing the buildings' timetable html format to a file by Volker Dirr
void TimetableExport::writeBuildingsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_BUILDINGS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	
	if(gt.rules.nInternalBuildings==0)
		tos<<"    <h1>"<<protect2(TimetableExport::tr("No buildings recorded in FET for %1.", "%1 is the institution name").arg(gt.rules.institutionName))<<"</h1>\n";
	else {
		tos<<"    <p><strong>"<<protect2(TimetableExport::tr("Table of contents"))<<"</strong></p>\n";
		tos<<"    <ul>\n";
		for(int building=0; building<gt.rules.nInternalBuildings; building++){
			QString building_name = gt.rules.internalBuildingsList[building]->name;
			tos<<"      <li><a href=\""<<"#table_"<<hashBuildingIDsTimetable.value(building_name)<<"\">"<<protect2(building_name)<<"</a></li>\n";
		}
		tos<<"    </ul>\n    <p>&nbsp;</p>\n";

		for(int building=0; building<gt.rules.nInternalBuildings; building++){
			tos<<singleBuildingsTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, building, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
			tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
		}
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//writing the buildings' timetable html format to a file by Volker Dirr
void TimetableExport::writeBuildingsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_BUILDINGS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;

		assert(0);
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	
	if(gt.rules.nInternalBuildings==0)
		tos<<"    <h1>"<<protect2(TimetableExport::tr("No buildings recorded in FET for %1.", "%1 is the institution name").arg(gt.rules.institutionName))<<"</h1>\n";
	else {
		tos<<"    <p><strong>"<<protect2(TimetableExport::tr("Table of contents"))<<"</strong></p>\n";
		tos<<"    <ul>\n";
		for(int building=0; building<gt.rules.nInternalBuildings; building++){
			QString building_name = gt.rules.internalBuildingsList[building]->name;
			tos<<"      <li><a href=\""<<"#table_"<<hashBuildingIDsTimetable.value(building_name)<<"\">"<<protect2(building_name)<<"</a></li>\n";
		}
		tos<<"    </ul>\n    <p>&nbsp;</p>\n";

		for(int building=0; building<gt.rules.nInternalBuildings; building++){
			tos<<singleBuildingsTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, building, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
			tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
		}
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//writing the rooms' timetable html format to a file by Volker Dirr
void TimetableExport::writeBuildingsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_BUILDINGS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	if(gt.rules.nInternalBuildings==0)
		tos<<"    <h1>"<<protect2(TimetableExport::tr("No buildings recorded in FET for %1.", "%1 is the institution name").arg(gt.rules.institutionName))<<"</h1>\n";
	else {
		QSet<int> tmp;
		tos<<singleBuildingsTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalBuildings, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

// writing the buildings' timetable html format to a file by Volker Dirr
void TimetableExport::writeBuildingsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_BUILDINGS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	if(gt.rules.nInternalBuildings==0)
		tos<<"    <h1>"<<protect2(TimetableExport::tr("No buildings recorded in FET for %1.", "%1 is the institution name").arg(gt.rules.institutionName))<<"</h1>\n";
	else {
		QSet<int> tmp;
		tos<<singleBuildingsTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalBuildings, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//by Volker Dirr
void TimetableExport::writeBuildingsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_BUILDINGS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES);
	
	if(gt.rules.nInternalBuildings==0)
		tos<<"    <h1>"<<protect2(TimetableExport::tr("No buildings recorded in FET for %1.", "%1 is the institution name").arg(gt.rules.institutionName))<<"</h1>\n";
	else {
		for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
			QSet<int> tmp;
			tos<<singleBuildingsTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalBuildings, tmp, saveTime,
				 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
				 TIMETABLE_HTML_REPEAT_NAMES);
			
			tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
		}
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//by Volker Dirr
void TimetableExport::writeBuildingsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_BUILDINGS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES);
	
	if(gt.rules.nInternalBuildings==0)
		tos<<"    <h1>"<<protect2(TimetableExport::tr("No buildings recorded in FET for %1.", "%1 is the institution name").arg(gt.rules.institutionName))<<"</h1>\n";
	else {
		for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
			QSet<int> tmp;
			tos<<singleBuildingsTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalBuildings, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

			tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
		}
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//writing the rooms' timetable html format to a file by Volker Dirr
void TimetableExport::writeRoomsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_ROOMS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	
	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<protect2(TimetableExport::tr("No rooms recorded in FET for %1.", "%1 is the institution name").arg(gt.rules.institutionName))<<"</h1>\n";
	else {
		tos<<"    <p><strong>"<<protect2(TimetableExport::tr("Table of contents"))<<"</strong></p>\n";
		tos<<"    <ul>\n";
		for(int room=0; room<gt.rules.nInternalRooms; room++){
			QString room_name = gt.rules.internalRoomsList[room]->name;
			tos<<"      <li><a href=\""<<"#table_"<<hashRoomIDsTimetable.value(room_name)<<"\">"<<getRoomString(gt.rules.internalRoomsList[room], SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_CODES)<<"</a></li>\n";
		}
		tos<<"    </ul>\n    <p>&nbsp;</p>\n";

		for(int room=0; room<gt.rules.nInternalRooms; room++){
			tos<<singleRoomsTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, room, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
			tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
		}
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//writing the rooms' timetable html format to a file by Volker Dirr
void TimetableExport::writeRoomsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_ROOMS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;

		assert(0);
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	
	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<protect2(TimetableExport::tr("No rooms recorded in FET for %1.", "%1 is the institution name").arg(gt.rules.institutionName))<<"</h1>\n";
	else {
		tos<<"    <p><strong>"<<protect2(TimetableExport::tr("Table of contents"))<<"</strong></p>\n";
		tos<<"    <ul>\n";
		for(int room=0; room<gt.rules.nInternalRooms; room++){
			QString room_name = gt.rules.internalRoomsList[room]->name;
			tos<<"      <li><a href=\""<<"#table_"<<hashRoomIDsTimetable.value(room_name)<<"\">"<<getRoomString(gt.rules.internalRoomsList[room], SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_CODES)<<"</a></li>\n";
		}
		tos<<"    </ul>\n    <p>&nbsp;</p>\n";

		for(int room=0; room<gt.rules.nInternalRooms; room++){
			tos<<singleRoomsTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, room, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
			tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
		}
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//writing the rooms' timetable html format to a file by Volker Dirr
void TimetableExport::writeRoomsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_ROOMS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<protect2(TimetableExport::tr("No rooms recorded in FET for %1.", "%1 is the institution name").arg(gt.rules.institutionName))<<"</h1>\n";
	else {
		QSet<int> tmp;
		tos<<singleRoomsTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalRooms, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

// writing the rooms' timetable html format to a file by Volker Dirr
void TimetableExport::writeRoomsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_ROOMS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);
	
	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<protect2(TimetableExport::tr("No rooms recorded in FET for %1.", "%1 is the institution name").arg(gt.rules.institutionName))<<"</h1>\n";
	else {
		QSet<int> tmp;
		tos<<singleRoomsTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalRooms, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//by Volker Dirr
void TimetableExport::writeRoomsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_ROOMS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES);
	
	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<protect2(TimetableExport::tr("No rooms recorded in FET for %1.", "%1 is the institution name").arg(gt.rules.institutionName))<<"</h1>\n";
	else {
		for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
			QSet<int> tmp;
			tos<<singleRoomsTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalRooms, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

			tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
		}
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//by Volker Dirr
void TimetableExport::writeRoomsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_ROOMS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES);
	
	if(gt.rules.nInternalRooms==0)
		tos<<"    <h1>"<<protect2(TimetableExport::tr("No rooms recorded in FET for %1.", "%1 is the institution name").arg(gt.rules.institutionName))<<"</h1>\n";
	else {
		for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
			QSet<int> tmp;
			tos<<singleRoomsTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalRooms, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

			tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
		}
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//Print the subjects

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeSubjectsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_SUBJECTS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);

	tos<<"    <p><strong>"<<protect2(TimetableExport::tr("Table of contents"))<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.nInternalSubjects; i++){
		tos<<"      <li>\n        "<<protect2(TimetableExport::tr("Subject"));
		tos<<" <a href=\""<<"#table_"<<hashSubjectIDsTimetable.value(gt.rules.internalSubjectsList[i]->name)<<"\">"<<getSubjectString(gt.rules.internalSubjectsList[i], SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_CODES)<<"</a>\n";
		tos<<"      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n";


	for(int subject=0; subject<gt.rules.nInternalSubjects; subject++){
		tos<<singleSubjectsTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, subject, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeSubjectsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_SUBJECTS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);

	tos<<"    <p><strong>"<<protect2(TimetableExport::tr("Table of contents"))<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.nInternalSubjects; i++){
		tos<<"      <li>\n        "<<protect2(TimetableExport::tr("Subject"));
		tos<<" <a href=\""<<"#table_"<<hashSubjectIDsTimetable.value(gt.rules.internalSubjectsList[i]->name)<<"\">"<<getSubjectString(gt.rules.internalSubjectsList[i], SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_CODES)<<"</a>\n";
		tos<<"      </li>\n";
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n";

	for(int subject=0; subject<gt.rules.nInternalSubjects; subject++){
		tos<<singleSubjectsTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, subject, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}
	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeSubjectsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_SUBJECTS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);

	QSet<int> tmp;
	tos<<singleSubjectsTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalSubjects, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

	tos << printCompleteLegend(SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeSubjectsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_SUBJECTS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);

	QSet<int> tmp;
	tos<<singleSubjectsTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalSubjects, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

	tos << printCompleteLegend(SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeSubjectsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_SUBJECTS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
		QSet<int> tmp;
		tos<<singleSubjectsTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalSubjects, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeSubjectsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_SUBJECTS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
		QSet<int> tmp;
		tos<<singleSubjectsTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalSubjects, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//Print the activity tags

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeActivityTagsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_ACTIVITY_TAGS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);

	tos<<"    <p><strong>"<<protect2(TimetableExport::tr("Table of contents"))<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.nInternalActivityTags; i++){
		if(gt.rules.internalActivityTagsList[i]->printable){
			tos<<"      <li>\n        "<<protect2(TimetableExport::tr("Activity Tag"));
			tos<<" <a href=\""<<"#table_"<<hashActivityTagIDsTimetable.value(gt.rules.internalActivityTagsList[i]->name)<<"\">"<<getActivityTagString(gt.rules.internalActivityTagsList[i], SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES)<<"</a>\n";
			tos<<"      </li>\n";
		}
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n";

	for(int activityTag=0; activityTag<gt.rules.nInternalActivityTags; activityTag++){
		if(gt.rules.internalActivityTagsList[activityTag]->printable){
			tos<<singleActivityTagsTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, activityTag, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
			tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
		}
	}
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeActivityTagsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_ACTIVITY_TAGS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);

	tos<<"    <p><strong>"<<protect2(TimetableExport::tr("Table of contents"))<<"</strong></p>\n";
	tos<<"    <ul>\n";
	for(int i=0; i<gt.rules.nInternalActivityTags; i++){
		if(gt.rules.internalActivityTagsList[i]->printable){
			tos<<"      <li>\n        "<<protect2(TimetableExport::tr("Activity Tag"));
			tos<<" <a href=\""<<"#table_"<<hashActivityTagIDsTimetable.value(gt.rules.internalActivityTagsList[i]->name)<<"\">"<<getActivityTagString(gt.rules.internalActivityTagsList[i], SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES)<<"</a>\n";
			tos<<"      </li>\n";
		}
	}
	tos<<"    </ul>\n    <p>&nbsp;</p>\n";

	for(int activityTag=0; activityTag<gt.rules.nInternalActivityTags; activityTag++){
		if(gt.rules.internalActivityTagsList[activityTag]->printable){
			tos<<singleActivityTagsTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, activityTag, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);
			tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
		}
	}
	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeActivityTagsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_ACTIVITY_TAGS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);

	QSet<int> tmp;
	tos<<singleActivityTagsTimetableTimeVerticalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalActivityTags, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

	tos << printCompleteLegend(SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeActivityTagsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_ACTIVITY_TAGS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, false);

	QSet<int> tmp;
	tos<<singleActivityTagsTimetableTimeHorizontalHtml(TIMETABLE_HTML_LEVEL, gt.rules.nInternalActivityTags, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

	tos << printCompleteLegend(SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeActivityTagsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_VERTICAL || !WRITE_TIMETABLES_ACTIVITY_TAGS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
		QSet<int> tmp;
		tos<<singleActivityTagsTimetableTimeVerticalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalActivityTags, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeActivityTagsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_TIME_HORIZONTAL || !WRITE_TIMETABLES_ACTIVITY_TAGS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	tos<<writeTOCDays(SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES);

	for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); day++){
		QSet<int> tmp;
		tos<<singleActivityTagsTimetableTimeHorizontalDailyHtml(TIMETABLE_HTML_LEVEL, day, gt.rules.nInternalActivityTags, tmp, saveTime,
		 TIMETABLE_HTML_PRINT_SUBJECTS, TIMETABLE_HTML_PRINT_ACTIVITY_TAGS, TIMETABLE_HTML_PRINT_TEACHERS, TIMETABLE_HTML_PRINT_STUDENTS, TIMETABLE_HTML_PRINT_ROOMS,
		 TIMETABLE_HTML_REPEAT_NAMES);

		tos<<"    <p class=\"back\"><a href=\""<<"#top\">"<<protect2(TimetableExport::tr("back to the top"))<<"</a></p>\n";
	}

	tos << printCompleteLegend(SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_LEGEND_CODES_FIRST);

	tos << "  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//Print the teachers free periods. Code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeTeachersFreePeriodsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_HORIZONTAL || !WRITE_TIMETABLES_TEACHERS_FREE_PERIODS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);

	tos<<"    <div class=\"TEACHER_HAS_SINGLE_GAP\">"<<protect2(TimetableExport::tr("Teacher has a single gap"))<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_BORDER_GAP\">"<<protect2(TimetableExport::tr("Teacher has a border gap"))<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_BIG_GAP\">"<<protect2(TimetableExport::tr("Teacher has a big gap"))<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_COME_EARLIER\">"<<protect2(TimetableExport::tr("Teacher must come earlier"))<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_COME_MUCH_EARLIER\">"<<protect2(TimetableExport::tr("Teacher must come much earlier"))<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_STAY_LONGER\">"<<protect2(TimetableExport::tr("Teacher must stay longer"))<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_STAY_MUCH_LONGER\">"<<protect2(TimetableExport::tr("Teacher must stay much longer"))<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_A_FREE_DAY\">"<<protect2(TimetableExport::tr("Teacher has a free day"))<<"</div>\n";
	tos<<"    <div class=\"TEACHER_IS_NOT_AVAILABLE\">"<<protect2(TimetableExport::tr("Teacher is not available"))<<"</div>\n";

	tos<<"    <p>&nbsp;</p>\n";
	
	tos<<singleTeachersFreePeriodsTimetableDaysHorizontalHtml(TIMETABLE_HTML_LEVEL, saveTime, PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS, TIMETABLE_HTML_REPEAT_NAMES);
	
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//XHTML generation code by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeTeachersFreePeriodsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_DAYS_VERTICAL || !WRITE_TIMETABLES_TEACHERS_FREE_PERIODS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);

	tos<<"    <div class=\"TEACHER_HAS_SINGLE_GAP\">"<<protect2(TimetableExport::tr("Teacher has a single gap"))<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_BORDER_GAP\">"<<protect2(TimetableExport::tr("Teacher has a border gap"))<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_BIG_GAP\">"<<protect2(TimetableExport::tr("Teacher has a big gap"))<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_COME_EARLIER\">"<<protect2(TimetableExport::tr("Teacher must come earlier"))<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_COME_MUCH_EARLIER\">"<<protect2(TimetableExport::tr("Teacher must come much earlier"))<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_STAY_LONGER\">"<<protect2(TimetableExport::tr("Teacher must stay longer"))<<"</div>\n";
	tos<<"    <div class=\"TEACHER_MUST_STAY_MUCH_LONGER\">"<<protect2(TimetableExport::tr("Teacher must stay much longer"))<<"</div>\n";
	tos<<"    <div class=\"TEACHER_HAS_A_FREE_DAY\">"<<protect2(TimetableExport::tr("Teacher has a free day"))<<"</div>\n";
	tos<<"    <div class=\"TEACHER_IS_NOT_AVAILABLE\">"<<protect2(TimetableExport::tr("Teacher is not available"))<<"</div>\n";

	tos<<"    <p>&nbsp;</p>\n";

	tos<<singleTeachersFreePeriodsTimetableDaysVerticalHtml(TIMETABLE_HTML_LEVEL, saveTime, PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS, TIMETABLE_HTML_REPEAT_NAMES);

	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//Code contributed by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeTeachersStatisticsHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);
	
	if(!WRITE_TIMETABLES_STATISTICS || !WRITE_TIMETABLES_TEACHERS){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	
	bool PRINT_DETAILED=true;
	tos<<singleTeachersStatisticsHtml(TIMETABLE_HTML_LEVEL, saveTime, PRINT_DETAILED, TIMETABLE_HTML_REPEAT_NAMES, true);
	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//Code contributed by Volker Dirr (https://timetabling.de/)
void TimetableExport::writeStudentsStatisticsHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities){
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	if(!WRITE_TIMETABLES_STATISTICS || !(WRITE_TIMETABLES_YEARS || WRITE_TIMETABLES_GROUPS || WRITE_TIMETABLES_SUBGROUPS) ){
		if(removeOldFiles)
			if(QFile::exists(htmlfilename))
				QFile::remove(htmlfilename);

		return;
	}

	//Now we print the results to an HTML file
	QFile file(htmlfilename);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(htmlfilename));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);

	tos<<writeHead(true, placedActivities, true);
	bool PRINT_DETAILED=true;
	tos<<singleStudentsStatisticsHtml(TIMETABLE_HTML_LEVEL, saveTime, PRINT_DETAILED, TIMETABLE_HTML_REPEAT_NAMES, true);

	tos<<"  </body>\n</html>\n";

	if(file.error()!=QFileDevice::NoError){
		IrreconcilableCriticalMessage::critical(parent, tr("FET critical"),
		 TimetableExport::tr("Writing '%1' gave the error message '%2', which means the writing is compromised. Please check your disk's free space.",
		 "%1 is the name of a file").arg(htmlfilename).arg(file.errorString()));
	}
	file.close();
}

//------------------------------------------------------------------
//------------------------------------------------------------------

void TimetableExport::computeHashForIDsTimetable(){
// by Volker Dirr

//TODO This is unneeded if you use a relational data base, because we can use the primary key id of the database.
//This is very similar to statistics compute hash. So always check it if you change something here!

	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);
	hashStudentIDsTimetable.clear();
	int cnt=1;
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		if(!hashStudentIDsTimetable.contains(sty->name)){
			hashStudentIDsTimetable.insert(sty->name, CustomFETString::number(cnt));
			cnt++;
		}
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			if(!hashStudentIDsTimetable.contains(stg->name)){
				hashStudentIDsTimetable.insert(stg->name, CustomFETString::number(cnt));
				cnt++;
			}
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				if(!hashStudentIDsTimetable.contains(sts->name)){
					hashStudentIDsTimetable.insert(sts->name, CustomFETString::number(cnt));
					cnt++;
				}
			}
		}
	}

	hashSubjectIDsTimetable.clear();
	for(int i=0; i<gt.rules.nInternalSubjects; i++){
		hashSubjectIDsTimetable.insert(gt.rules.internalSubjectsList[i]->name, CustomFETString::number(i+1));
	}
	hashActivityTagIDsTimetable.clear();
	for(int i=0; i<gt.rules.nInternalActivityTags; i++){
	//	if(gt.rules.internalActivityTagsList[i]->printable){
			hashActivityTagIDsTimetable.insert(gt.rules.internalActivityTagsList[i]->name, CustomFETString::number(i+1));
	//	}
	}
	hashTeacherIDsTimetable.clear();
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		hashTeacherIDsTimetable.insert(gt.rules.internalTeachersList[i]->name, CustomFETString::number(i+1));
	}
	hashBuildingIDsTimetable.clear();
	for(int building=0; building<gt.rules.nInternalBuildings; building++){
		hashBuildingIDsTimetable.insert(gt.rules.internalBuildingsList[building]->name, CustomFETString::number(building+1));
	}
	hashRoomIDsTimetable.clear();
	for(int room=0; room<gt.rules.nInternalRooms; room++){
		hashRoomIDsTimetable.insert(gt.rules.internalRoomsList[room]->name, CustomFETString::number(room+1));
	}
	hashDayIDsTimetable.clear();
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		for(int day=0; day<gt.rules.nDaysPerWeek; day++){
			hashDayIDsTimetable.insert(gt.rules.daysOfTheWeek[day], CustomFETString::number(day+1));
		}
	}
	else{
		for(int day=0; day<gt.rules.nRealDaysPerWeek; day++){
			hashDayIDsTimetable.insert(gt.rules.realDaysOfTheWeek[day], CustomFETString::number(day+1));
		}
	}
	if(TIMETABLE_HTML_LEVEL==7){
		computeHashActivityColorBySubject();
		computeHashActivityColorBySubjectAndStudents();
	}
}

//By Liviu, with ideas from Volker
void TimetableExport::computeHashActivityColorBySubject(){
	QHash<QString, int> tmpHash;

	hashActivityColorBySubject.clear();
	activeHashActivityColorBySubject.clear();
	
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	QSet<QString> alreadyAdded;
	
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			Activity* act=&gt.rules.internalActivitiesList[i];
			QString tmpString=act->subjectName;
			if(!alreadyAdded.contains(tmpString)){
				alreadyAdded.insert(tmpString);
				hashActivityColorBySubject.insert(i, alreadyAdded.count());
				activeHashActivityColorBySubject.append(i);
				tmpHash.insert(tmpString, alreadyAdded.count());
			}
			else{
				assert(tmpHash.contains(tmpString));
				int k=tmpHash.value(tmpString);
				hashActivityColorBySubject.insert(i, k);
			}
		}
	}
	
	//cout<<"hashActivityColorBySubject.count()=="<<hashActivityColorBySubject.count()<<endl;
}

//By Liviu, with ideas from Volker
void TimetableExport::computeHashActivityColorBySubjectAndStudents(){
	QHash<QString, int> tmpHash;

	hashActivityColorBySubjectAndStudents.clear();
	activeHashActivityColorBySubjectAndStudents.clear();
	
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);

	QSet<QString> alreadyAdded;
	
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(best_solution.times[i]!=UNALLOCATED_TIME){
			Activity* act=&gt.rules.internalActivitiesList[i];
			
			QString tmpString=act->subjectName+" "+act->studentsNames.join(", ");
			if(!alreadyAdded.contains(tmpString)){
				alreadyAdded.insert(tmpString);
				hashActivityColorBySubjectAndStudents.insert(i, alreadyAdded.count());
				activeHashActivityColorBySubjectAndStudents.append(i);
				tmpHash.insert(tmpString, alreadyAdded.count());
			}
			else{
				assert(tmpHash.contains(tmpString));
				int k=tmpHash.value(tmpString);
				hashActivityColorBySubjectAndStudents.insert(i, k);
			}
		}
	}
	
	//cout<<"hashActivityColorBySubjectAndStudents.count()=="<<hashActivityColorBySubjectAndStudents.count()<<endl;
}

/*void TimetableExport::computeHashForColors(QHash<QString, QString>& hashColorStringIDsTimetable){
// by Volker Dirr
	qWarning("compute hash for colors");
	hashColorStringIDsTimetable.clear();
	assert(gt.rules.initialized && gt.rules.internalStructureComputed);
	assert(students_schedule_ready && teachers_schedule_ready && rooms_buildings_schedule_ready);
	QSet<QString> alreadyAddedString;
	for(int i=0; i<gt.rules.nInternalActivities; i++) {
		Activity* act=&gt.rules.internalActivitiesList[i];
		if(best_solution.times[i]!=UNALLOCATED_TIME) {
			qWarning("add a hash");
			//coloring for students
			QString tmpString=act->subjectName;
			if(!alreadyAddedString.contains(tmpString)){
				alreadyAddedString<<tmpString;
				hashColorStringIDsTimetable.insert(tmpString, CustomFETString::number(alreadyAddedString.count()));
			}
			//coloring for teachers
			tmpString=act->subjectName+" "+act->studentsNames.join(protect2(translatedCommaSpace()));
			if(!alreadyAddedString.contains(tmpString)){
				alreadyAddedString<<tmpString;
				hashColorStringIDsTimetable.insert(tmpString, CustomFETString::number(alreadyAddedString.count()));
			}
			//coloring for rooms
//			it is similar to students
//			tmpString=act->subjectName+" "+act->studentsNames.join(protect2(translatedCommaSpace()));
//			if(!alreadyAddedString.contains(tmpString)){
//				alreadyAddedString<<tmpString;
//				hashColorStringIDsTimetable.insert(tmpString, CustomFETString::number(alreadyAddedString.count()));
//			}
		}
	}
}*/

void TimetableExport::computeActivitiesAtTime(){		// by Liviu Lalescu
	activitiesAtTimeList.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	activitiesAtTimeSet.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			activitiesAtTimeList[day][hour].clear();
			activitiesAtTimeSet[day][hour].clear();
		}
	}
	for(int i=0; i<gt.rules.nInternalActivities; i++) {		//maybe TODO: maybe it is better to do this sorted by students or teachers?
		Activity* act=&gt.rules.internalActivitiesList[i];
		if(best_solution.times[i]!=UNALLOCATED_TIME) {
			int hour=best_solution.times[i]/gt.rules.nDaysPerWeek;
			int day=best_solution.times[i]%gt.rules.nDaysPerWeek;
			for(int dd=0; dd < act->duration && hour+dd < gt.rules.nHoursPerDay; dd++){
				activitiesAtTimeList[day][hour+dd].append(i);
				activitiesAtTimeSet[day][hour+dd].insert(i);
			}
		}
	}
}

void TimetableExport::computeActivitiesWithSameStartingTime(){
// by Volker Dirr
	activitiesWithSameStartingTimeList.clear();
	activitiesWithSameStartingTimeSet.clear();

	if(PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME){
		for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
			TimeConstraint* tc=gt.rules.internalTimeConstraintsList[i];
			if(tc->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME){ //not needed anymore:  && tc->weightPercentage==100
				ConstraintActivitiesSameStartingTime* c=(ConstraintActivitiesSameStartingTime*) tc;
				for(int a=0; a<c->_n_activities; a++){
					//speed improvement
					QList<int> tmpList=activitiesWithSameStartingTimeList.value(c->_activities[a], QList<int>());
					QSet<int> tmpSet=activitiesWithSameStartingTimeSet.value(c->_activities[a], QSet<int>());
					for(int b=0; b<c->_n_activities; b++){
						if(a!=b){
							if(best_solution.times[c->_activities[a]]==best_solution.times[c->_activities[b]]){ //because the constraint might not have been with 100% weight and might have failed
								if(!tmpSet.contains(c->_activities[b])){
									tmpList.append(c->_activities[b]);
									tmpSet.insert(c->_activities[b]);
								}
							}
						}
					}
					activitiesWithSameStartingTimeList.insert(c->_activities[a], tmpList);
					activitiesWithSameStartingTimeSet.insert(c->_activities[a], tmpSet);
					/*
					QList<int> tmpList;
					if(activitiesWithSameStartingTime.contains(c->_activities[a]))
						tmpList=activitiesWithSameStartingTime.value(c->_activities[a]);
					for(int b=0; b<c->_n_activities; b++){
						if(a!=b){
							if(best_solution.times[c->_activities[a]]==best_solution.times[c->_activities[b]]){ 	//because constraint is maybe not with 100% weight and failed
								if(!tmpList.contains(c->_activities[b])){
									tmpList<<c->_activities[b];
								}
							}
						}
					}
					activitiesWithSameStartingTime.insert(c->_activities[a], tmpList);
					*/
				}
			}
		}
	}
}

bool TimetableExport::addActivitiesWithSameStartingTime(QList<int>& allActivitiesList, QSet<int>& allActivitiesSet, int hour){
// by Volker Dirr
	if(PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME){
		bool haveActivitiesWithSameStartingTime=false;
		QList<int> allActivitiesListNew;
		QSet<int> allActivitiesSetNew;
		for(int tmpAct : std::as_const(allActivitiesList)){
			allActivitiesListNew.append(tmpAct);
			allActivitiesSetNew.insert(tmpAct);
			if(activitiesWithSameStartingTimeSet.contains(tmpAct)){
				assert(activitiesWithSameStartingTimeList.contains(tmpAct));
				QList<int> sameTimeList=activitiesWithSameStartingTimeList.value(tmpAct);
				QSet<int> sameTimeSet=activitiesWithSameStartingTimeSet.value(tmpAct);
				for(int sameTimeAct : std::as_const(sameTimeList)){
					if(!allActivitiesSetNew.contains(sameTimeAct) && !allActivitiesSet.contains(sameTimeAct)){
						if(best_solution.times[sameTimeAct]!=UNALLOCATED_TIME){
							Activity* act=&gt.rules.internalActivitiesList[sameTimeAct];
							assert(best_solution.times[tmpAct]==best_solution.times[sameTimeAct]);//{
							if((best_solution.times[sameTimeAct]/gt.rules.nDaysPerWeek+(act->duration-1))>=hour){
								allActivitiesListNew.append(sameTimeAct);
								allActivitiesSetNew.insert(sameTimeAct);
							}
							haveActivitiesWithSameStartingTime=true; //don't add this line in the previous 'if' command because of activities with different duration!
							//}
						}
					}
				}
			}
		}
		//allActivities.clear();
		allActivitiesList=allActivitiesListNew;
		allActivitiesSet=allActivitiesSetNew;
		//allActivitiesNew.clear();
		return haveActivitiesWithSameStartingTime;
	}
	else
		return false;
}

// by Volker Dirr
QString TimetableExport::writeHead(bool java, int placedActivities, bool printInstitution){
	QString tmp;
	tmp+="<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	tmp+="  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n";

	if(!LANGUAGE_STYLE_RIGHT_TO_LEFT)
		tmp+="<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""+LANGUAGE_FOR_HTML+"\" xml:lang=\""+LANGUAGE_FOR_HTML+"\">\n";
	else
		tmp+="<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""+LANGUAGE_FOR_HTML+"\" xml:lang=\""+LANGUAGE_FOR_HTML+"\" dir=\"rtl\">\n";
	tmp+="  <head>\n";
	tmp+="    <title>"+protect2(gt.rules.institutionName)+"</title>\n";
	tmp+="    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n";
	if(TIMETABLE_HTML_LEVEL>=1){
		QString cssfilename=INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.lastIndexOf(FILE_SEP)-1);
		
		if(cssfilename.right(4)==".fet")
			cssfilename=cssfilename.left(cssfilename.length()-4);
		//else if(INPUT_FILENAME_XML!="")
		//	cout<<"Minor problem - input file does not end in .fet extension - might be a problem when saving the timetables"<<" (file:"<<__FILE__<<", line:"<<__LINE__<<")"<<endl;
		
		cssfilename+="_"+STYLESHEET_CSS;
		if(INPUT_FILENAME_XML=="")
			cssfilename=STYLESHEET_CSS;
		tmp+="    <link rel=\"stylesheet\" media=\"all\" href=\""+cssfilename+"\" type=\"text/css\" />\n";
	}
	if(java){
		if(TIMETABLE_HTML_LEVEL>=5 && TIMETABLE_HTML_LEVEL!=7){  // the following JavaScript code is pretty similar to an example of Les Richardson
			tmp+="    <meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\" />\n";
			tmp+="    <script type=\"text/javascript\">\n";
			tmp+="      function highlight(classval) {\n";
			tmp+="        var spans = document.getElementsByTagName('span');\n";
			tmp+="        for(var i=0; spans.length>i; i++) {\n";
			tmp+="          if (spans[i].className == classval) {\n";
			tmp+="            spans[i].style.backgroundColor = 'lime';\n";
			tmp+="          } else {\n";
			tmp+="            spans[i].style.backgroundColor = 'white';\n";
			tmp+="          }\n";
			tmp+="        }\n";
			tmp+="      }\n";
			tmp+="    </script>\n";
		}
	}
	tmp+="  </head>\n";
	tmp+="  <body id=\"top\">\n";
	if(placedActivities!=gt.rules.nInternalActivities)
		tmp+="    <h1>"+protect2(TimetableExport::tr("Warning! Only %1 out of %2 activities placed!").arg(placedActivities).arg(gt.rules.nInternalActivities))+"</h1>\n";
	if(printInstitution){
		tmp+="    <table>\n      <tr align=\"left\" valign=\"top\">\n        <th>"+protect2(TimetableExport::tr("Institution name"))+":</th>\n        <td>"+protect2(gt.rules.institutionName)+"</td>\n      </tr>\n    </table>\n";
		tmp+="    <table>\n      <tr align=\"left\" valign=\"top\">\n        <th>"+protect2(TimetableExport::tr("Comments"))+":</th>\n        <td>"+protect2(gt.rules.comments).replace("\n", "<br />\n")+"</td>\n      </tr>\n    </table>\n";
	}
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeTOCDays(bool writeNames, bool writeLongNames){
	QString tmp;
	tmp+="    <p><strong>"+protect2(TimetableExport::tr("Table of contents"))+"</strong></p>\n";
	tmp+="    <ul>\n";
	for(int day=0; day<(gt.rules.mode!=MORNINGS_AFTERNOONS?gt.rules.nDaysPerWeek:gt.rules.nRealDaysPerWeek); day++){
		tmp+="      <li>\n";
		tmp+="        <a href=\"#table_"+hashDayIDsTimetable.value(gt.rules.mode!=MORNINGS_AFTERNOONS?gt.rules.daysOfTheWeek[day]:gt.rules.realDaysOfTheWeek[day])+"\">"+getDayOrRealDayString(day, writeNames, writeLongNames)+"</a>\n";
		tmp+="      </li>\n";
	}
	tmp+="    </ul>\n    <p>&nbsp;</p>\n";
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeStartTagTDofActivities(int htmlLevel, const Activity* act, bool detailed, bool colspan, bool rowspan, int colorBy,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
 bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
 bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
 bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
 bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes){
	Q_UNUSED(printRooms);

	Q_UNUSED(printRoomsNames);
	Q_UNUSED(printRoomsLongNames);
	Q_UNUSED(printRoomsCodes);
	
	bool localPrintSubjects = printSubjects && (printSubjectsNames || printSubjectsLongNames || printSubjectsCodes);
	bool localPrintActivityTags = printActivityTags && (printActivityTagsNames || printActivityTagsLongNames || printActivityTagsCodes);
	bool localPrintTeachers = printTeachers && (printTeachersNames || printTeachersLongNames || printTeachersCodes);
	bool localPrintStudents = printStudents && (printStudentsNames || printStudentsLongNames || printStudentsCodes);
	
	QString tmp;
	assert(!(colspan && rowspan));
	if(detailed)
		assert(!colspan && !rowspan);
	else
		tmp+="          ";
	tmp+="<td";
	if(rowspan && act->duration>1)
		tmp+=" rowspan=\""+CustomFETString::number(act->duration)+"\"";
	if(colspan && act->duration>1)
		tmp+=" colspan=\""+CustomFETString::number(act->duration)+"\"";
	if(htmlLevel==6 &&
	 (localPrintSubjects
	 || (localPrintActivityTags && !act->activityTagsNames.isEmpty())
	 || (localPrintTeachers && !act->teachersNames.isEmpty())
	 || (localPrintStudents && !act->studentsNames.isEmpty()))){
		tmp+=" class=\"";
		int chopSize=0;
		if(localPrintSubjects && act->subjectName.size()>0){
			tmp+="s_"+hashSubjectIDsTimetable.value(act->subjectName);
			tmp+=" ";
			chopSize=1;
		}
		if(localPrintActivityTags && act->activityTagsNames.size()>0){
			for(const QString& atn : std::as_const(act->activityTagsNames)){
				assert(hashActivityTagIDsTimetable.contains(atn));
				int id=hashActivityTagIDsTimetable.value(atn, "0").toInt()-1;
				assert(id>=0);
				assert(id<gt.rules.nInternalActivityTags);
				if(gt.rules.internalActivityTagsList[id]->printable){
					tmp+="at_"+hashActivityTagIDsTimetable.value(atn);
					tmp+=" ";
					chopSize=1;
				}
			}
		}
		if(localPrintStudents && act->studentsNames.size()>0){
			for(const QString& st : std::as_const(act->studentsNames)){
				tmp+="ss_"+hashStudentIDsTimetable.value(st);
				tmp+=" ";
				chopSize=1;
			}
		}
		if(localPrintTeachers && act->teachersNames.size()>0){
			for(const QString& t : std::as_const(act->teachersNames)){
				tmp+="t_"+hashTeacherIDsTimetable.value(t);
				tmp+=" ";
				chopSize=1;
			}
		}
		//I need ai for this!!! So I need a parameter ai?! //TODO
		/*int r=best_solution.rooms[ai];
		if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
			tmp+=" room_"+protect2id(gt.rules.internalRoomsList[r]->name);
		}*/
		if(chopSize>=1)
			tmp.chop(chopSize);
		if(detailed)
			tmp+=" detailed";
		tmp+="\"";
	}
	if(htmlLevel==7){
		assert(gt.rules.activitiesHash.contains(act->id));
		int index=gt.rules.activitiesHash.value(act->id);
		switch(colorBy){
			case COLOR_BY_SUBJECT_STUDENTS: tmp+=" class=\"c_"+QString::number(activeHashActivityColorBySubject.count()+hashActivityColorBySubjectAndStudents.value(index)); break;
			case COLOR_BY_SUBJECT: tmp+=" class=\"c_"+QString::number(hashActivityColorBySubject.value(index)); break;
			default: assert(0==1);
		}
		
		if(detailed)
			tmp+=" detailed";
		tmp+="\"";
	}
	if(detailed && htmlLevel>=1 && htmlLevel<=5)
		tmp+=" class=\"detailed\"";
	tmp+=">";
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeSubjectAndActivityTags(int htmlLevel, const Activity* act, const QString& startTag, const QString& startTagAttribute, bool activityTagsOnly, bool printActivityTags,
 bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes, bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes){
	QString tmp;
	if(act->subjectName.size()>0||act->activityTagsNames.size()>0){
		if(startTag=="div" && htmlLevel>=3)
			tmp+="<"+startTag+startTagAttribute+">";
		QString oldtt=QString("");
		if(act->subjectName.size()>0 && !activityTagsOnly && (printSubjectsNames || printSubjectsLongNames || printSubjectsCodes)){
			QString tt=getSubjectString(gt.rules.internalSubjectsList[gt.rules.subjectsHash.value(act->subjectName)], printSubjectsNames, printSubjectsLongNames, printSubjectsCodes);
			switch(htmlLevel){
				case 3 : tmp+="<span class=\"subject\">"+tt+"</span>"; break;
				case 4 : tmp+="<span class=\"subject\"><span class=\"s_"+hashSubjectIDsTimetable.value(act->subjectName)+"\">"+tt+"</span></span>"; break;
				case 5 : [[fallthrough]];
				case 6 : tmp+="<span class=\"subject\"><span class=\"s_"+hashSubjectIDsTimetable.value(act->subjectName)+"\" onmouseover=\"highlight('s_"+hashSubjectIDsTimetable.value(act->subjectName)+"')\">"+tt+"</span></span>"; break;
				case 7 : tmp+="<span class=\"subject\">"+tt+"</span>"; break;
				default: tmp+=tt; break;
			}
			oldtt=tt;
		}
		if(act->activityTagsNames.size()>0 && printActivityTags && (printActivityTagsNames || printActivityTagsLongNames || printActivityTagsCodes)){
			bool writeTags=false;
			for(const QString& atn : std::as_const(act->activityTagsNames)){
				assert(hashActivityTagIDsTimetable.contains(atn));
				int id=hashActivityTagIDsTimetable.value(atn, "0").toInt()-1;
				assert(id>=0);
				assert(id<gt.rules.nInternalActivityTags);
				if(gt.rules.internalActivityTagsList[id]->printable){
					writeTags=true;
					break;
				}
			}
			if(writeTags){
				if(true/*!activityTagsOnly*/){
					if(htmlLevel>=3){
						tmp+="<span class=\"activitytag\">";
					}
				}
				bool firstTag=true;
				for(const QString& atn : std::as_const(act->activityTagsNames)){
					assert(hashActivityTagIDsTimetable.contains(atn));
					int id=hashActivityTagIDsTimetable.value(atn, "0").toInt()-1;
					assert(id>=0);
					assert(id<gt.rules.nInternalActivityTags);
					if(gt.rules.internalActivityTagsList[id]->printable){
						QString tt=getActivityTagString(gt.rules.internalActivityTagsList[gt.rules.activityTagsHash.value(atn)], printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes);

						if(!oldtt.isEmpty()){
							bool ml = SETTINGS_TIMETABLES_SEPARATE_ACTIVITY_TAGS_NAME_LONG_NAME_CODE_BY_BREAK && (oldtt.contains("<br />") || tt.contains("<br />"));
							tmp += ml ? QString("<br />") : (firstTag ? QString(" ") : protect2(translatedCommaSpace()));
						}
						
						oldtt=tt;

						switch(htmlLevel){
							case 3 : tmp+=tt; break;
							case 4 : tmp+="<span class=\"at_"+hashActivityTagIDsTimetable.value(atn)+"\">"+tt+"</span>"; break;
							case 5 : [[fallthrough]];
							case 6 : tmp+="<span class=\"at_"+hashActivityTagIDsTimetable.value(atn)+"\" onmouseover=\"highlight('at_"+hashActivityTagIDsTimetable.value(atn)+"')\">"+tt+"</span>"; break;
							default: tmp+=tt; break;
						}
						
						if(firstTag)
							firstTag=false;
					}
				}
				if(true/* || !activityTagsOnly*/){
					if(htmlLevel>=3){
						tmp+="</span>";
					}
				}
			}
		}
		if(startTag=="div"){
			if(htmlLevel>=3)
				tmp+="</div>";
			else tmp+="<br />";
		}
	}
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeStudents(int htmlLevel, const Activity* act, const QString& startTag, const QString& startTagAttribute,
 bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes){
	QString tmp;
	if(act->studentsNames.size()>0 && (printStudentsNames || printStudentsLongNames || printStudentsCodes)){
		if(startTag=="div" && htmlLevel>=3)
			tmp+="<"+startTag+startTagAttribute+">";
		QString oldtt=QString("");
		for(const QString& st : std::as_const(act->studentsNames)){
			StudentsSet* ss=gt.rules.studentsHash.value(st);
			assert(ss!=nullptr);

			QString tt=getStudentsSetString(ss, printStudentsNames, printStudentsLongNames, printStudentsCodes);
			
			if(!oldtt.isEmpty()){
				bool ml = SETTINGS_TIMETABLES_SEPARATE_STUDENTS_NAME_LONG_NAME_CODE_BY_BREAK && (oldtt.contains("<br />") || tt.contains("<br />"));
				tmp += ml ? QString("<br />") : protect2(translatedCommaSpace());
			}
			
			oldtt=tt;

			switch(htmlLevel){
				case 4 : tmp+="<span class=\"ss_"+hashStudentIDsTimetable.value(st)+"\">"+tt+"</span>"; break;
				case 5 : [[fallthrough]];
				case 6 : tmp+="<span class=\"ss_"+hashStudentIDsTimetable.value(st)+"\" onmouseover=\"highlight('ss_"+hashStudentIDsTimetable.value(st)+"')\">"+tt+"</span>"; break;
				default: tmp+=tt; break;
			}
		}
		if(startTag=="div"){
			if(htmlLevel>=3)
				tmp+="</div>";
			else tmp+="<br />";
		}
	}
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeTeachers(int htmlLevel, const Activity* act, const QString& startTag, const QString& startTagAttribute,
 bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes){
	QString tmp;
	if(act->teachersNames.size()>0 && (printTeachersNames || printTeachersLongNames || printTeachersCodes)){
		if(startTag=="div" && htmlLevel>=3)
			tmp+="<"+startTag+startTagAttribute+">";
		QString oldtt=QString("");
		for(const QString& t : std::as_const(act->teachersNames)){
			QString tt=getTeacherString(gt.rules.internalTeachersList[gt.rules.teachersHash.value(t)], printTeachersNames, printTeachersLongNames, printTeachersCodes);

			if(!oldtt.isEmpty()){
				bool ml = SETTINGS_TIMETABLES_SEPARATE_TEACHERS_NAME_LONG_NAME_CODE_BY_BREAK && (oldtt.contains("<br />") || tt.contains("<br />"));
				tmp += ml ? QString("<br />") : protect2(translatedCommaSpace());
			}
			
			oldtt=tt;

			switch(htmlLevel){
				case 4 : tmp+="<span class=\"t_"+hashTeacherIDsTimetable.value(t)+"\">"+tt+"</span>"; break;
				case 5 : [[fallthrough]];
				case 6 : tmp+="<span class=\"t_"+hashTeacherIDsTimetable.value(t)+"\" onmouseover=\"highlight('t_"+hashTeacherIDsTimetable.value(t)+"')\">"+tt+"</span>"; break;
				default: tmp+=tt; break;
			}
		}
		if(startTag=="div"){
			if(htmlLevel>=3)
				tmp+="</div>";
			else tmp+="<br />";
		}
	}
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeRoom(int htmlLevel, int ai, const QString& startTag, const QString& startTagAttribute,
 bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes){
	QString tmp;
	int r=best_solution.rooms[ai];
	if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM && (printRoomsNames || printRoomsLongNames || printRoomsCodes)){
		if(startTag=="div" && htmlLevel>=3)
			tmp+="<"+startTag+startTagAttribute+">";
		
		if(gt.rules.internalRoomsList[r]->isVirtual==false){
			Room* rm=gt.rules.internalRoomsList[r];

			QString tt=getRoomString(rm, printRoomsNames, printRoomsLongNames, printRoomsCodes);

			switch(htmlLevel){
				case 4 : tmp+="<span class=\"r_"+hashRoomIDsTimetable.value(gt.rules.internalRoomsList[r]->name)+"\">"+tt+"</span>"; break;
				case 5 : [[fallthrough]];
				case 6 : tmp+="<span class=\"r_"+hashRoomIDsTimetable.value(gt.rules.internalRoomsList[r]->name)+"\" onmouseover=\"highlight('r_"+hashRoomIDsTimetable.value(gt.rules.internalRoomsList[r]->name)+"')\">"+tt+"</span>"; break;
				default: tmp+=tt; break;
			}
		} else {
			if(SHOW_VIRTUAL_ROOMS_IN_TIMETABLES && (printRoomsNames || printRoomsLongNames || printRoomsCodes)){
				Room* rm=gt.rules.internalRoomsList[r];

				QString tt=getRoomString(rm, printRoomsNames, printRoomsLongNames, printRoomsCodes);

				switch(htmlLevel){
					case 4 : tmp+="<span class=\"r_"+hashRoomIDsTimetable.value(gt.rules.internalRoomsList[r]->name)+"\">"+tt+"</span>"; break;
					case 5 : [[fallthrough]];
					case 6 : tmp+="<span class=\"r_"+hashRoomIDsTimetable.value(gt.rules.internalRoomsList[r]->name)+"\" onmouseover=\"highlight('r_"+hashRoomIDsTimetable.value(gt.rules.internalRoomsList[r]->name)+"')\">"+tt+"</span>"; break;
					default: tmp+=tt; break;
				}
				//tmp+="Virtual Room X";
				tmp+=" (";
			}
			QStringList rooms;
			for(int rr : std::as_const(best_solution.realRoomsList[ai])){
				Room* rrm=gt.rules.internalRoomsList[rr];

				QString tt=getRoomString(rrm, printRoomsNames, printRoomsLongNames, printRoomsCodes);

				QString room;
				switch(htmlLevel){
					case 4 : room+="<span class=\"r_"+hashRoomIDsTimetable.value(gt.rules.internalRoomsList[rr]->name)+"\">"+tt+"</span>"; break;
					case 5 : [[fallthrough]];
					case 6 : room+="<span class=\"r_"+hashRoomIDsTimetable.value(gt.rules.internalRoomsList[rr]->name)+"\" onmouseover=\"highlight('r_"+hashRoomIDsTimetable.value(gt.rules.internalRoomsList[rr]->name)+"')\">"+tt+"</span>"; break;
					default: room+=tt; break;
				}
				rooms.append(room);
			}
			tmp+=rooms.join(protect2(translatedCommaSpace()));
			if(SHOW_VIRTUAL_ROOMS_IN_TIMETABLES){
				tmp+=")";
			}
		}

		if(startTag=="div"){
			if(htmlLevel>=3)
				tmp+="</div>";
			else tmp+="<br />";
		}
	}
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeNotAvailableSlot(int htmlLevel, const QString& weight){
	QString tmp;
	//weight=" "+weight;
	switch(htmlLevel){
		case 3 : [[fallthrough]];
		case 4 : tmp="          <td class=\"notAvailable\"><span class=\"notAvailable\">"+protect2(STRING_NOT_AVAILABLE_TIME_SLOT)+weight+"</span></td>\n"; break;
		case 5 : [[fallthrough]];
		case 6 : tmp="          <td class=\"notAvailable\"><span class=\"notAvailable\" onmouseover=\"highlight('notAvailable')\">"+protect2(STRING_NOT_AVAILABLE_TIME_SLOT)+weight+"</span></td>\n"; break;
		case 7 : tmp="          <td class=\"notAvailable\"><span class=\"notAvailable\">"+protect2(STRING_NOT_AVAILABLE_TIME_SLOT)+weight+"</span></td>\n"; break;
		default: tmp="          <td>"+protect2(STRING_NOT_AVAILABLE_TIME_SLOT)+weight+"</td>\n";
	}
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeBreakSlot(int htmlLevel, const QString& weight){
	QString tmp;
	//weight=" "+weight;
	switch(htmlLevel){
		case 3 : [[fallthrough]];
		case 4 : tmp="          <td class=\"break\"><span class=\"break\">"+protect2(STRING_BREAK_SLOT)+weight+"</span></td>\n"; break;
		case 5 : [[fallthrough]];
		case 6 : tmp="          <td class=\"break\"><span class=\"break\" onmouseover=\"highlight('break')\">"+protect2(STRING_BREAK_SLOT)+weight+"</span></td>\n"; break;
		case 7 : tmp="          <td class=\"break\"><span class=\"break\">"+protect2(STRING_BREAK_SLOT)+weight+"</span></td>\n"; break;
		default: tmp="          <td>"+protect2(STRING_BREAK_SLOT)+weight+"</td>\n";
	}
	return tmp;
}

// by Volker Dirr
QString TimetableExport::writeEmpty(int htmlLevel){
	QString tmp;
	switch(htmlLevel){
		case 3 : [[fallthrough]];
		case 4 : tmp="          <td class=\"empty\"><span class=\"empty\">"+protect2(STRING_EMPTY_SLOT)+"</span></td>\n"; break;
		case 5 : [[fallthrough]];
		case 6 : tmp="          <td class=\"empty\"><span class=\"empty\" onmouseover=\"highlight('empty')\">"+protect2(STRING_EMPTY_SLOT)+"</span></td>\n"; break;
		case 7 : tmp="          <td class=\"empty\"><span class=\"empty\">"+protect2(STRING_EMPTY_SLOT)+"</span></td>\n"; break;
		default: tmp="          <td>"+protect2(STRING_EMPTY_SLOT)+"</td>\n";
	}
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivityStudents(int htmlLevel, int ai, int day, int hour, bool notAvailable, bool colspan, bool rowspan,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
 bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
 bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
 bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
 bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes,
 const QString& skipStudentsSet){
	QString tmp;
	int currentTime=day+gt.rules.nDaysPerWeek*hour;
	if(ai!=UNALLOCATED_ACTIVITY){
		if(best_solution.times[ai]==currentTime){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, false, colspan, rowspan, COLOR_BY_SUBJECT, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
			 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
			 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
			 printTeachersNames, printTeachersLongNames, printTeachersCodes,
			 printStudentsNames, printStudentsLongNames, printStudentsCodes,
			 printRoomsNames, printRoomsLongNames, printRoomsCodes
			 );
			//TODO line0
			bool skipLine0=false;
			if(act->studentsNames.size()==1){
				if(act->studentsNames.at(0)==skipStudentsSet){
					skipLine0=true;
				}
			}
			if(printStudents){
				if(!skipLine0){
					tmp+=writeStudents(htmlLevel, act, "div", " class=\"studentsset line0\"", printStudentsNames, printStudentsLongNames, printStudentsCodes);
				}
			}
			if(printSubjects || (printActivityTags && !act->activityTagsNames.isEmpty()))
				tmp+=writeSubjectAndActivityTags(htmlLevel, act, "div", " class=\"line1\"", !printSubjects, printActivityTags,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes);
			if(printTeachers)
				tmp+=writeTeachers(htmlLevel, act, "div", " class=\"teacher line2\"", printTeachersNames, printTeachersLongNames, printTeachersCodes);
			if(printRooms)
				tmp+=writeRoom(htmlLevel, ai, "div", " class=\"room line3\"", printRoomsNames, printRoomsLongNames, printRoomsCodes);
			tmp+="</td>\n";
		} else
			tmp+="          <!-- span -->\n";
	} else {
		if(notAvailable && PRINT_NOT_AVAILABLE_TIME_SLOTS){
			tmp+=writeNotAvailableSlot(htmlLevel, "");
		}
		else if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
			tmp+=writeBreakSlot(htmlLevel, "");
		}
		else{
			tmp+=writeEmpty(htmlLevel);
		}
	}
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivitiesStudents(int htmlLevel, const QList<int>& allActivitiesOriginal,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
 bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
 bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
 bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
 bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes){
	QList<int> allActivities=allActivitiesOriginal;
	if(PRINT_RTL)
		std::reverse(allActivities.begin(), allActivities.end());

	bool localPrintTeachers=false;
	if(printTeachers){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				if(!act->teachersNames.isEmpty()){
					localPrintTeachers=true;
					break;
				}
			}
		}
	}
	bool localPrintStudents=false;
	if(printStudents){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				if(!act->studentsNames.isEmpty()){
					localPrintStudents=true;
					break;
				}
			}
		}
	}
	bool localPrintRooms=false;
	if(printRooms){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				int r=best_solution.rooms[ai];
				if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
					localPrintRooms=true;
					break;
				}
			}
		}
	}

	QString tmp;

	if(htmlLevel>=1)
		tmp+="          <td><table class=\"detailed\">";
	else
		tmp+="          <td><table>";
	if(localPrintStudents){
		if(htmlLevel>=3)
			tmp+="<tr class=\"studentsset line0\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeStudents(htmlLevel, act, "", "", printStudentsNames, printStudentsLongNames, printStudentsCodes)+"</td>";
			}
		}
		tmp+="</tr>";
	}
	if(printSubjects || printActivityTags){
		bool ok=true;
		if(!printSubjects){
			ok=false;
			for(int a=0; a<allActivities.size(); a++){
				int ai=allActivities[a];
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(!act->activityTagsNames.isEmpty()){
						ok=true;
						break;
					}
				}
			}
		}
		if(ok){
			if(htmlLevel>=3)
				tmp+="<tr class=\"line1\">";
			else	tmp+="<tr>";
			for(int a=0; a<allActivities.size(); a++){
				int ai=allActivities[a];
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
					 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
					 printTeachersNames, printTeachersLongNames, printTeachersCodes,
					 printStudentsNames, printStudentsLongNames, printStudentsCodes,
					 printRoomsNames, printRoomsLongNames, printRoomsCodes
					 )
					 +writeSubjectAndActivityTags(htmlLevel, act, "", "", !printSubjects, printActivityTags, printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
					 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes)+"</td>";
				}
			}
			tmp+="</tr>";
		}
	}
	if(localPrintTeachers){
		if(htmlLevel>=3)
			tmp+="<tr class=\"teacher line2\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeTeachers(htmlLevel, act, "", "", printTeachersNames, printTeachersLongNames, printTeachersCodes)+"</td>";
			}
		}
		tmp+="</tr>";
	}
	if(localPrintRooms){
		if(htmlLevel>=3)
			tmp+="<tr class=\"room line3\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeRoom(htmlLevel, ai, "", "", printRoomsNames, printRoomsLongNames, printRoomsCodes)+"</td>";
			}
		}
		tmp+="</tr>";
	}
	tmp+="</table></td>\n";
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivityTeacher(int htmlLevel, int teacher, int day, int hour, bool colspan, bool rowspan,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
 bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
 bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
 bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
 bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes,
 const QString& skipTeacher){
	QString tmp;
	int ai=teachers_timetable_weekly[teacher][day][hour];
	int currentTime=day+gt.rules.nDaysPerWeek*hour;
	if(ai!=UNALLOCATED_ACTIVITY){
		if(best_solution.times[ai]==currentTime){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, false, colspan, rowspan, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
			 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
			 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
			 printTeachersNames, printTeachersLongNames, printTeachersCodes,
			 printStudentsNames, printStudentsLongNames, printStudentsCodes,
			 printRoomsNames, printRoomsLongNames, printRoomsCodes
			 );
			//TODO line0
			bool skipLine0=false;
			if(act->teachersNames.size()==1){
				if(act->teachersNames.at(0)==skipTeacher){
					skipLine0=true;
				}
			}
			if(printTeachers){
				if(!skipLine0){
					tmp+=writeTeachers(htmlLevel, act, "div", " class=\"teacher line0\"", printTeachersNames, printTeachersLongNames, printTeachersCodes);
				}
			}
			if(printStudents)
				tmp+=writeStudents(htmlLevel, act, "div", " class=\"studentsset line1\"", printStudentsNames, printStudentsLongNames, printStudentsCodes);
			if(printSubjects || (printActivityTags && !act->activityTagsNames.isEmpty()))
				tmp+=writeSubjectAndActivityTags(htmlLevel, act, "div", " class=\"line2\"", !printSubjects, printActivityTags, printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes);
			if(printRooms)
				tmp+=writeRoom(htmlLevel, ai, "div", " class=\"room line3\"", printRoomsNames, printRoomsLongNames, printRoomsCodes);
			tmp+="</td>\n";
		} else
			tmp+="          <!-- span -->\n";
	} else {
		if(teacherNotAvailableDayHour[teacher][day][hour] && PRINT_NOT_AVAILABLE_TIME_SLOTS){
			tmp+=writeNotAvailableSlot(htmlLevel, "");
		}
		else if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
			tmp+=writeBreakSlot(htmlLevel, "");
		}
		else{
			tmp+=writeEmpty(htmlLevel);
		}
	}
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivitiesTeachers(int htmlLevel, const QList<int>& allActivitiesOriginal,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
 bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
 bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
 bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
 bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes){
	QList<int> allActivities=allActivitiesOriginal;
	if(PRINT_RTL)
		std::reverse(allActivities.begin(), allActivities.end());

	bool localPrintTeachers=false;
	if(printTeachers){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				if(!act->teachersNames.isEmpty()){
					localPrintTeachers=true;
					break;
				}
			}
		}
	}
	bool localPrintStudents=false;
	if(printStudents){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				if(!act->studentsNames.isEmpty()){
					localPrintStudents=true;
					break;
				}
			}
		}
	}
	bool localPrintRooms=false;
	if(printRooms){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				int r=best_solution.rooms[ai];
				if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
					localPrintRooms=true;
					break;
				}
			}
		}
	}

	QString tmp;
	if(htmlLevel>=1)
		tmp+="          <td><table class=\"detailed\">";
	else
		tmp+="          <td><table>";
	
	if(localPrintTeachers){
		if(htmlLevel>=3)
			tmp+="<tr class=\"teacher line0\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeTeachers(htmlLevel, act, "", "", printTeachersNames, printTeachersLongNames, printTeachersCodes)+"</td>";
			}
		}
		tmp+="</tr>";
	}
	if(localPrintStudents){
		if(htmlLevel>=3)
			tmp+="<tr class=\"studentsset line1\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeStudents(htmlLevel, act, "", "", printStudentsNames, printStudentsLongNames, printStudentsCodes)+"</td>";
			}
		}
		tmp+="</tr>";
	}
	if(printSubjects || printActivityTags){
		bool ok=true;
		if(!printSubjects){
			ok=false;
			for(int a=0; a<allActivities.size(); a++){
				int ai=allActivities[a];
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(!act->activityTagsNames.isEmpty()){
						ok=true;
						break;
					}
				}
			}
		}
		if(ok){
			if(htmlLevel>=3)
				tmp+="<tr class=\"line2\">";
			else	tmp+="<tr>";
			for(int a=0; a<allActivities.size(); a++){
				int ai=allActivities[a];
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
					 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
					 printTeachersNames, printTeachersLongNames, printTeachersCodes,
					 printStudentsNames, printStudentsLongNames, printStudentsCodes,
					 printRoomsNames, printRoomsLongNames, printRoomsCodes
					 )
					 +writeSubjectAndActivityTags(htmlLevel, act, "", "", !printSubjects, printActivityTags, printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
					 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes)+"</td>";
				}
			}
			tmp+="</tr>";
		}
	}
	if(localPrintRooms){
		if(htmlLevel>=3)
			tmp+="<tr class=\"room line3\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeRoom(htmlLevel, ai, "", "", printRoomsNames, printRoomsLongNames, printRoomsCodes)+"</td>";
			}
		}
		tmp+="</tr>";
	}
	tmp+="</table></td>\n";
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivityBuilding(int htmlLevel, int building, int day, int hour, bool colspan, bool rowspan,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
 bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
 bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
 bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
 bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes){
	/*Q_UNUSED(printRooms);

	Q_UNUSED(printRoomsNames);
	Q_UNUSED(printRoomsLongNames);
	Q_UNUSED(printRoomsCodes);*/

	QString tmp;
	assert(buildings_timetable_weekly[building][day][hour].count()==1);
	int ai=buildings_timetable_weekly[building][day][hour].at(0);
	int currentTime=day+gt.rules.nDaysPerWeek*hour;
	if(ai!=UNALLOCATED_ACTIVITY){
		if(best_solution.times[ai]==currentTime){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, false, colspan, rowspan, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
			 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
			 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
			 printTeachersNames, printTeachersLongNames, printTeachersCodes,
			 printStudentsNames, printStudentsLongNames, printStudentsCodes,
			 printRoomsNames, printRoomsLongNames, printRoomsCodes
			 );
			//Each activity has only a single room. So there is no need for line0. Modify this as soon as FET supports multiple rooms per activity.
			if(printStudents)
				tmp+=writeStudents(htmlLevel, act, "div", " class=\"studentsset line1\"", printStudentsNames, printStudentsLongNames, printStudentsCodes);
			if(printTeachers)
				tmp+=writeTeachers(htmlLevel, act, "div", " class=\"teacher line2\"", printTeachersNames, printTeachersLongNames, printTeachersCodes);
			if(printSubjects || (printActivityTags && !act->activityTagsNames.isEmpty()))
				tmp+=writeSubjectAndActivityTags(htmlLevel, act, "div", " class=\"line3\"", !printSubjects, printActivityTags, printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes);
			tmp+="</td>\n";
		} else
			tmp+="          <!-- span -->\n";
	} else {
		if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
			tmp+=writeBreakSlot(htmlLevel, "");
		}
		else{
			tmp+=writeEmpty(htmlLevel);
		}
	}
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivitiesBuildings(int htmlLevel, const QList<int>& allActivitiesOriginal,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
 bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
 bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
 bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
 bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes){
	QList<int> allActivities=allActivitiesOriginal;
	if(PRINT_RTL)
		std::reverse(allActivities.begin(), allActivities.end());

	bool localPrintTeachers=false;
	if(printTeachers){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				if(!act->teachersNames.isEmpty()){
					localPrintTeachers=true;
					break;
				}
			}
		}
	}
	bool localPrintStudents=false;
	if(printStudents){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				if(!act->studentsNames.isEmpty()){
					localPrintStudents=true;
					break;
				}
			}
		}
	}
	bool localPrintRooms=false;
	if(printRooms){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				int r=best_solution.rooms[ai];
				if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
					localPrintRooms=true;
					break;
				}
			}
		}
	}

	QString tmp;
	if(htmlLevel>=1)
		tmp+="          <td><table class=\"detailed\">";
	else
		tmp+="          <td><table>";
	
	if(localPrintRooms){
		if(htmlLevel>=3)
			tmp+="<tr class=\"room line0\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeRoom(htmlLevel, ai, "", "", printRoomsNames, printRoomsLongNames, printRoomsCodes)+"</td>";
			}
		}
		tmp+="</tr>";
	}
	if(localPrintStudents){
		if(htmlLevel>=3)
			tmp+="<tr class=\"studentsset line1\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeStudents(htmlLevel, act, "", "", printStudentsNames, printStudentsLongNames, printStudentsCodes)+"</td>";
			}
		}
		tmp+="</tr>";
	}
	if(localPrintTeachers){
		if(htmlLevel>=3)
			tmp+="<tr class=\"teacher line2\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeTeachers(htmlLevel, act, "", "", printTeachersNames, printTeachersLongNames, printTeachersCodes)+"</td>";
			}
		}
		tmp+="</tr>";
	}
	if(printSubjects || printActivityTags){
		bool ok=true;
		if(!printSubjects){
			ok=false;
			for(int a=0; a<allActivities.size(); a++){
				int ai=allActivities[a];
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(!act->activityTagsNames.isEmpty()){
						ok=true;
						break;
					}
				}
			}
		}
		if(ok){
			if(htmlLevel>=3)
				tmp+="<tr class=\"line3\">";
			else	tmp+="<tr>";
			for(int a=0; a<allActivities.size(); a++){
				int ai=allActivities[a];
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
					 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
					 printTeachersNames, printTeachersLongNames, printTeachersCodes,
					 printStudentsNames, printStudentsLongNames, printStudentsCodes,
					 printRoomsNames, printRoomsLongNames, printRoomsCodes
					 )
					 +writeSubjectAndActivityTags(htmlLevel, act, "", "", !printSubjects, printActivityTags, printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
					 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes)+"</td>";
				}
			}
			tmp+="</tr>";
		}
	}
	
	tmp+="</table></td>\n";
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivityRoom(int htmlLevel, int room, int day, int hour, bool colspan, bool rowspan,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
 bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
 bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
 bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
 bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes){
	/*Q_UNUSED(printRooms);

	Q_UNUSED(printRoomsNames);
	Q_UNUSED(printRoomsLongNames);
	Q_UNUSED(printRoomsCodes);*/

	QString tmp;
	int ai=rooms_timetable_weekly[room][day][hour];
	int currentTime=day+gt.rules.nDaysPerWeek*hour;
	if(ai!=UNALLOCATED_ACTIVITY){
		if(best_solution.times[ai]==currentTime){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			tmp+=writeStartTagTDofActivities(htmlLevel, act, false, colspan, rowspan, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
			 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
			 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
			 printTeachersNames, printTeachersLongNames, printTeachersCodes,
			 printStudentsNames, printStudentsLongNames, printStudentsCodes,
			 printRoomsNames, printRoomsLongNames, printRoomsCodes
			 );
			//Each activity has only a single room. So there is no need for line0. Modify this as soon as FET supports multiple rooms per activity.
			if(printStudents)
				tmp+=writeStudents(htmlLevel, act, "div", " class=\"studentsset line1\"", printStudentsNames, printStudentsLongNames, printStudentsCodes);
			if(printTeachers)
				tmp+=writeTeachers(htmlLevel, act, "div", " class=\"teacher line2\"", printTeachersNames, printTeachersLongNames, printTeachersCodes);
			if(printSubjects || (printActivityTags && !act->activityTagsNames.isEmpty()))
				tmp+=writeSubjectAndActivityTags(htmlLevel, act, "div", " class=\"line3\"", !printSubjects, printActivityTags, printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes);
			tmp+="</td>\n";
		} else
			tmp+="          <!-- span -->\n";
	} else {
		if(notAllowedRoomTimePercentages[room][day+hour*gt.rules.nDaysPerWeek]>=0 && PRINT_NOT_AVAILABLE_TIME_SLOTS){
			tmp+=writeNotAvailableSlot(htmlLevel, "");
		}
		else if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
			tmp+=writeBreakSlot(htmlLevel, "");
		}
		else{
			tmp+=writeEmpty(htmlLevel);
		}
	}
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivitiesRooms(int htmlLevel, const QList<int>& allActivitiesOriginal,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
 bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
 bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
 bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
 bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes){
	QList<int> allActivities=allActivitiesOriginal;
	if(PRINT_RTL)
		std::reverse(allActivities.begin(), allActivities.end());

	bool localPrintTeachers=false;
	if(printTeachers){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				if(!act->teachersNames.isEmpty()){
					localPrintTeachers=true;
					break;
				}
			}
		}
	}
	bool localPrintStudents=false;
	if(printStudents){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				if(!act->studentsNames.isEmpty()){
					localPrintStudents=true;
					break;
				}
			}
		}
	}
	bool localPrintRooms=false;
	if(printRooms){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				int r=best_solution.rooms[ai];
				if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
					localPrintRooms=true;
					break;
				}
			}
		}
	}

	QString tmp;
	if(htmlLevel>=1)
		tmp+="          <td><table class=\"detailed\">";
	else
		tmp+="          <td><table>";
	
	if(localPrintRooms){
		if(htmlLevel>=3)
			tmp+="<tr class=\"room line0\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeRoom(htmlLevel, ai, "", "", printRoomsNames, printRoomsLongNames, printRoomsCodes)+"</td>";
			}
		}
		tmp+="</tr>";
	}
	if(localPrintStudents){
		if(htmlLevel>=3)
			tmp+="<tr class=\"studentsset line1\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeStudents(htmlLevel, act, "", "", printStudentsNames, printStudentsLongNames, printStudentsCodes)+"</td>";
			}
		}
		tmp+="</tr>";
	}
	if(localPrintTeachers){
		if(htmlLevel>=3)
			tmp+="<tr class=\"teacher line2\">";
		else	tmp+="<tr>";
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeTeachers(htmlLevel, act, "", "", printTeachersNames, printTeachersLongNames, printTeachersCodes)+"</td>";
			}
		}
		tmp+="</tr>";
	}
	if(printSubjects || printActivityTags){
		bool ok=true;
		if(!printSubjects){
			ok=false;
			for(int a=0; a<allActivities.size(); a++){
				int ai=allActivities[a];
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(!act->activityTagsNames.isEmpty()){
						ok=true;
						break;
					}
				}
			}
		}
		if(ok){
			if(htmlLevel>=3)
				tmp+="<tr class=\"line3\">";
			else	tmp+="<tr>";
			for(int a=0; a<allActivities.size(); a++){
				int ai=allActivities[a];
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
					 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
					 printTeachersNames, printTeachersLongNames, printTeachersCodes,
					 printStudentsNames, printStudentsLongNames, printStudentsCodes,
					 printRoomsNames, printRoomsLongNames, printRoomsCodes
					 )
					 +writeSubjectAndActivityTags(htmlLevel, act, "", "", !printSubjects, printActivityTags, printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
					 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes)+"</td>";
				}
			}
			tmp+="</tr>";
		}
	}
	
	tmp+="</table></td>\n";
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivitiesSubjects(int htmlLevel, const QList<int>& allActivitiesOriginal,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
 bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
 bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
 bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
 bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes){
	QList<int> allActivities=allActivitiesOriginal;
	if(PRINT_RTL)
		std::reverse(allActivities.begin(), allActivities.end());

	bool localPrintTeachers=false;
	if(printTeachers){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				if(!act->teachersNames.isEmpty()){
					localPrintTeachers=true;
					break;
				}
			}
		}
	}
	bool localPrintStudents=false;
	if(printStudents){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				if(!act->studentsNames.isEmpty()){
					localPrintStudents=true;
					break;
				}
			}
		}
	}
	bool localPrintRooms=false;
	if(printRooms){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				int r=best_solution.rooms[ai];
				if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
					localPrintRooms=true;
					break;
				}
			}
		}
	}

	QString tmp;
	if(allActivities.isEmpty()){
		tmp+=writeEmpty(htmlLevel);
	} else {
		if(htmlLevel>=1)
			tmp+="          <td><table class=\"detailed\">";
		else
			tmp+="          <td><table>";
		//Each activity has only a single subject. So there is no need for subjects in line0. Modify this as soon as FET supports multiple subjects per activity.
		if(printSubjects || printActivityTags){
			bool ok=true;
			if(!printSubjects && printActivityTags){
				ok=false;
				for(int a=0; a<allActivities.size(); a++){
					int ai=allActivities[a];
					if(ai!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(!act->activityTagsNames.isEmpty()){
							ok=true;
							break;
						}
					}
				}
			}
			else if(printSubjects && !printActivityTags){
				ok=false;
				for(int a=0; a<allActivities.size(); a++){
					Activity* act=&gt.rules.internalActivitiesList[allActivities[a]];
					Activity* act0=&gt.rules.internalActivitiesList[allActivities[0]];	//Because this is always the original subject. We don't need to repeat it, because it is displayed in the tables head
					if(act->subjectName==act0->subjectName){
						ok=true;
						break;
					}
				}
			}
			else if(printSubjects && printActivityTags){
				bool oks=false;
				for(int a=0; a<allActivities.size(); a++){
					Activity* act=&gt.rules.internalActivitiesList[allActivities[a]];
					Activity* act0=&gt.rules.internalActivitiesList[allActivities[0]];	//Because this is always the original subject. We don't need to repeat it, because it is displayed in the tables head
					if(act->subjectName==act0->subjectName){
						oks=true;
						break;
					}
				}
				
				bool okat=false;
				for(int a=0; a<allActivities.size(); a++){
					int ai=allActivities[a];
					if(ai!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(!act->activityTagsNames.isEmpty()){
							okat=true;
							break;
						}
					}
				}
				
				ok=oks || okat;
			}
			if(ok){
				if(htmlLevel>=3)
					tmp+="<tr class=\"line0\">";
				else	tmp+="<tr>";
				for(int a=0; a<allActivities.size(); a++){
					Activity* act=&gt.rules.internalActivitiesList[allActivities[a]];
					Activity* act0=&gt.rules.internalActivitiesList[allActivities[0]];	//Because this is always the original subject. We don't need to repeat it, because it is displayed in the tables head
					if(act->subjectName==act0->subjectName){
						tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
						 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
						 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
						 printTeachersNames, printTeachersLongNames, printTeachersCodes,
						 printStudentsNames, printStudentsLongNames, printStudentsCodes,
						 printRoomsNames, printRoomsLongNames, printRoomsCodes
						 )
						 +writeSubjectAndActivityTags(htmlLevel, act, "", "", true, printActivityTags, printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
						 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes)+"</td>";
					} else {
						tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
						 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
						 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
						 printTeachersNames, printTeachersLongNames, printTeachersCodes,
						 printStudentsNames, printStudentsLongNames, printStudentsCodes,
						 printRoomsNames, printRoomsLongNames, printRoomsCodes
						 )
						 +writeSubjectAndActivityTags(htmlLevel, act, "", "", !printSubjects, printActivityTags, printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
						 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes)+"</td>";
					}
				}
				tmp+="</tr>";
			}
		}
		if(localPrintStudents){
			if(htmlLevel>=3)
				tmp+="<tr class=\"studentsset line1\">";
			else	tmp+="<tr>";
			for(int a=0; a<allActivities.size(); a++){
				Activity* act=&gt.rules.internalActivitiesList[allActivities[a]];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeStudents(htmlLevel, act, "", "", printStudentsNames, printStudentsLongNames, printStudentsCodes)+"</td>";
			}
			tmp+="</tr>";
		}
		if(localPrintTeachers){
			if(htmlLevel>=3)
				tmp+="<tr class=\"teacher line2\">";
			else	tmp+="<tr>";
			for(int a=0; a<allActivities.size(); a++){
				Activity* act=&gt.rules.internalActivitiesList[allActivities[a]];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeTeachers(htmlLevel, act, "", "", printTeachersNames, printTeachersLongNames, printTeachersCodes)+"</td>";
			}
			tmp+="</tr>";
		}
		if(localPrintRooms){
			if(htmlLevel>=3)
				tmp+="<tr class=\"room line3\">";
			else	tmp+="<tr>";
			for(int a=0; a<allActivities.size(); a++){
				int ai=allActivities[a];
				Activity* act=&gt.rules.internalActivitiesList[ai];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeRoom(htmlLevel, ai, "", "", printRoomsNames, printRoomsLongNames, printRoomsCodes)+"</td>";
			}
			tmp+="</tr>";
		}
		tmp+="</table></td>\n";
	}
	return tmp;
}

//by Volker Dirr
QString TimetableExport::writeActivitiesActivityTags(int htmlLevel, const QList<int>& allActivitiesOriginal,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
 bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
 bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
 bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
 bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes){
	QList<int> allActivities=allActivitiesOriginal;
	if(PRINT_RTL)
		std::reverse(allActivities.begin(), allActivities.end());

	bool localPrintTeachers=false;
	if(printTeachers){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				if(!act->teachersNames.isEmpty()){
					localPrintTeachers=true;
					break;
				}
			}
		}
	}
	bool localPrintStudents=false;
	if(printStudents){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				if(!act->studentsNames.isEmpty()){
					localPrintStudents=true;
					break;
				}
			}
		}
	}
	bool localPrintRooms=false;
	if(printRooms){
		for(int a=0; a<allActivities.size(); a++){
			int ai=allActivities[a];
			if(ai!=UNALLOCATED_ACTIVITY){
				int r=best_solution.rooms[ai];
				if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
					localPrintRooms=true;
					break;
				}
			}
		}
	}

	QString tmp;
	if(allActivities.isEmpty()){
		tmp+=writeEmpty(htmlLevel);
	} else {
		if(htmlLevel>=1)
			tmp+="          <td><table class=\"detailed\">";
		else
			tmp+="          <td><table>";
		
		bool ok=true;
		if(!printSubjects){
			ok=false;
			for(int a=0; a<allActivities.size(); a++){
				int ai=allActivities[a];
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					if(!act->activityTagsNames.isEmpty()){
						ok=true;
						break;
					}
				}
			}
		}
		if(ok){
			if(printSubjects || printActivityTags){
				if(htmlLevel>=3)
					tmp+="<tr class=\"line0\">";
				else	tmp+="<tr>";
				for(int a=0; a<allActivities.size(); a++){
					int ai=allActivities[a];
					if(ai!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
						 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
						 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
						 printTeachersNames, printTeachersLongNames, printTeachersCodes,
						 printStudentsNames, printStudentsLongNames, printStudentsCodes,
						 printRoomsNames, printRoomsLongNames, printRoomsCodes
						 )
						 +writeSubjectAndActivityTags(htmlLevel, act, "", "", !printSubjects, printActivityTags, printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
						 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes)+"</td>";
					}
				}
				tmp+="</tr>";
			}
		}
		if(localPrintStudents){
			if(htmlLevel>=3)
				tmp+="<tr class=\"studentsset line1\">";
			else	tmp+="<tr>";
			for(int a=0; a<allActivities.size(); a++){
				Activity* act=&gt.rules.internalActivitiesList[allActivities[a]];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeStudents(htmlLevel, act, "", "", printStudentsNames, printStudentsLongNames, printStudentsCodes)+"</td>";
			}
			tmp+="</tr>";
		}
		if(localPrintTeachers){
			if(htmlLevel>=3)
				tmp+="<tr class=\"teacher line2\">";
			else	tmp+="<tr>";
			for(int a=0; a<allActivities.size(); a++){
				Activity* act=&gt.rules.internalActivitiesList[allActivities[a]];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeTeachers(htmlLevel, act, "", "", printTeachersNames, printTeachersLongNames, printTeachersCodes)+"</td>";
			}
			tmp+="</tr>";
		}
		if(localPrintRooms){
			if(htmlLevel>=3)
				tmp+="<tr class=\"room line3\">";
			else	tmp+="<tr>";
			for(int a=0; a<allActivities.size(); a++){
				int ai=allActivities[a];
				Activity* act=&gt.rules.internalActivitiesList[ai];
				tmp+=writeStartTagTDofActivities(htmlLevel, act, true, false, false, COLOR_BY_SUBJECT_STUDENTS, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 printSubjectsNames, printSubjectsLongNames, printSubjectsCodes,
				 printActivityTagsNames, printActivityTagsLongNames, printActivityTagsCodes,
				 printTeachersNames, printTeachersLongNames, printTeachersCodes,
				 printStudentsNames, printStudentsLongNames, printStudentsCodes,
				 printRoomsNames, printRoomsLongNames, printRoomsCodes
				 )
				 +writeRoom(htmlLevel, ai, "", "", printRoomsNames, printRoomsLongNames, printRoomsCodes)+"</td>";
			}
			tmp+="</tr>";
		}
		tmp+="</table></td>\n";
	}
	return tmp;
}

//the following functions return a single html table (needed for html file export and printing)
//by Volker Dirr
QString TimetableExport::singleSubgroupsTimetableDaysHorizontalHtml(int htmlLevel, int subgroup, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames, const QList<int>& subgroupsSortedOrder){
	int realSubgroup;
	if(subgroupsSortedOrder!=QList<int>())
		realSubgroup=subgroupsSortedOrder.at(subgroup);
	else
		realSubgroup=subgroup;
	
	assert(realSubgroup>=0);
	assert(realSubgroup<gt.rules.nInternalSubgroups);
	QString tmpString;
	QString subgroup_name = gt.rules.internalSubgroupsList[realSubgroup]->name;
	
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table_"+hashStudentIDsTimetable.value(subgroup_name)+"\" border=\"1\"";
	else
		tmpString+="    <table align=\"center\" id=\"table_"+hashStudentIDsTimetable.value(subgroup_name)+"\" border=\"1\"";
	if(subgroup%2==0) tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";
	
	QStringList tl;
	if(!gt.rules.institutionName.isEmpty())
		tl.append("<span class=\"institution\">"+protect2(gt.rules.institutionName)+"</span>");
	QString nm=getStudentsSetString(gt.rules.internalSubgroupsList[realSubgroup], SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_CODES);
	if(!nm.isEmpty())
		tl.append("<span class=\"name\">"+nm+"</span>");
	QString c1=gt.rules.internalSubgroupsList[realSubgroup]->comments;
	if(SETTINGS_TIMETABLES_PRINT_SUBGROUPS_COMMENTS && !c1.isEmpty()){
		QString c2=protect2(c1);
		c2.replace("\n", "<br />\n");
		tl.append("<span class=\"comment\">"+c2+"</span>");
	}
	
	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;
	
	tmpString+="      <caption>"+tl.join("<br />")+"</caption>\n";
	tmpString+="      <thead>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
		int td;
		if(!PRINT_RTL)
			td=rtd;
		else
			td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		
		tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int th=0; th<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); th++){
		int hour;
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			hour=th;
		else
			hour=th%gt.rules.nHoursPerDay;
		
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
			int td;
			if(!PRINT_RTL)
				td=rtd;
			else
				td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
			
			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			allActivitiesList.append(students_timetable_weekly[realSubgroup][day][hour]);
			allActivitiesSet.insert(students_timetable_weekly[realSubgroup][day][hour]);
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, students_timetable_weekly[realSubgroup][day][hour], day, hour, subgroupNotAvailableDayHour[realSubgroup][day][hour], false, true,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_CODES,
				 subgroup_name
				 );
			} else{
				tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_CODES
				 );
			}
			
			addActivitiesLegend(allActivitiesList, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
			 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
			 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
			 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_LEGEND,
			 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LEGEND,
			 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_LEGEND);
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	
	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);
	
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleSubgroupsTimetableDaysVerticalHtml(int htmlLevel, int subgroup, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames, const QList<int>& subgroupsSortedOrder){
	int realSubgroup;
	if(subgroupsSortedOrder!=QList<int>())
		realSubgroup=subgroupsSortedOrder.at(subgroup);
	else
		realSubgroup=subgroup;

	assert(realSubgroup>=0);
	assert(realSubgroup<gt.rules.nInternalSubgroups);
	QString tmpString;
	QString subgroup_name = gt.rules.internalSubgroupsList[realSubgroup]->name;

	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table_"+hashStudentIDsTimetable.value(subgroup_name)+"\" border=\"1\"";
	else
		tmpString+="    <table align=\"center\" id=\"table_"+hashStudentIDsTimetable.value(subgroup_name)+"\" border=\"1\"";
	if(subgroup%2==0) tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	QStringList tl;
	if(!gt.rules.institutionName.isEmpty())
		tl.append("<span class=\"institution\">"+protect2(gt.rules.institutionName)+"</span>");
	QString nm=getStudentsSetString(gt.rules.internalSubgroupsList[realSubgroup], SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_CODES);
	if(!nm.isEmpty())
		tl.append("<span class=\"name\">"+nm+"</span>");
	QString c1=gt.rules.internalSubgroupsList[realSubgroup]->comments;
	if(SETTINGS_TIMETABLES_PRINT_SUBGROUPS_COMMENTS && !c1.isEmpty()){
		QString c2=protect2(c1);
		c2.replace("\n", "<br />\n");
		tl.append("<span class=\"comment\">"+c2+"</span>");
	}

	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;
	
	tmpString+="      <caption>"+tl.join("<br />")+"</caption>\n";
	tmpString+="      <thead>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
		int th;
		if(!PRINT_RTL)
			th=rth;
		else
			th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";

		tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int td=0; td<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); td++){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
			int th;
			if(!PRINT_RTL)
				th=rth;
			else
				th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;

			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			int hour;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				hour=th;
			else
				hour=th%gt.rules.nHoursPerDay;
			
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			allActivitiesList.append(students_timetable_weekly[realSubgroup][day][hour]);
			allActivitiesSet.insert(students_timetable_weekly[realSubgroup][day][hour]);
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, students_timetable_weekly[realSubgroup][day][hour], day, hour, subgroupNotAvailableDayHour[realSubgroup][day][hour], true, false,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_CODES,
				 subgroup_name
				 );
			} else{
				tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_CODES
				 );
			}

			addActivitiesLegend(allActivitiesList, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
			 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
			 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
			 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_LEGEND,
			 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LEGEND,
			 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_LEGEND);
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_SUBGROUPS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleSubgroupsTimetableTimeVerticalHtml(int htmlLevel, int maxSubgroups, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames, const QList<int>& subgroupsSortedOrder){
	QList<int> realSubgroupsList;
	int currentCount=0;
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups && currentCount<maxSubgroups; subgroup++){
		int realSubgroup;
		if(subgroupsSortedOrder!=QList<int>())
			realSubgroup=subgroupsSortedOrder.at(subgroup);
		else
			realSubgroup=subgroup;

		if(!excludedNames.contains(subgroup)){
			currentCount++;
			realSubgroupsList.append(realSubgroup);
			excludedNames.insert(subgroup);
		}
	}
	if(PRINT_RTL)
		std::reverse(realSubgroupsList.begin(), realSubgroupsList.end());

	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";
	
	for(int realSubgroup : std::as_const(realSubgroupsList)){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=getStudentsSetString(gt.rules.internalSubgroupsList[realSubgroup], SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			tmpString+="        <tr>\n";
			if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}

				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				if(gt.rules.mode!=MORNINGS_AFTERNOONS)
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				else
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			for(int realSubgroup : std::as_const(realSubgroupsList)){
				QList<int> allActivitiesList;
				QSet<int> allActivitiesSet;
				//allActivities.clear();
				allActivitiesList.append(students_timetable_weekly[realSubgroup][day][hour]);
				allActivitiesSet.insert(students_timetable_weekly[realSubgroup][day][hour]);
				bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
				if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
					tmpString+=writeActivityStudents(htmlLevel, students_timetable_weekly[realSubgroup][day][hour], day, hour, subgroupNotAvailableDayHour[realSubgroup][day][hour], false, true,
					 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_CODES,
					 gt.rules.internalSubgroupsList[realSubgroup]->name
					 );
				} else{
					tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_CODES
					);
				}
			}
			if((!PRINT_RTL && repeatNames) || PRINT_RTL){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
			}
			tmpString+="        </tr>\n";
		}

		if(USE_DUMMY_TH_TR_TD){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(day!=gt.rules.nDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, currentCount+2+(repeatNames?2:0));
			}
			else{
				if(day%2==1 && day/2!=gt.rules.nRealDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, currentCount+2+(repeatNames?2:0));
			}
		}
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleSubgroupsTimetableTimeHorizontalHtml(int htmlLevel, int maxSubgroups, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames, const QList<int>& subgroupsSortedOrder){
	QList<int> realSubgroupsList;
	int currentCount=0;
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups && currentCount<maxSubgroups; subgroup++){
		int realSubgroup;
		if(subgroupsSortedOrder!=QList<int>())
			realSubgroup=subgroupsSortedOrder.at(subgroup);
		else
			realSubgroup=subgroup;

		if(!excludedNames.contains(subgroup)){
			currentCount++;
			realSubgroupsList.append(realSubgroup);
			excludedNames.insert(subgroup);
		}
	}

	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(day, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(day, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;
				
				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nRealHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nRealHoursPerDay-1-rhour;

				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getRealHourString(hour, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	
	for(int realSubgroup : std::as_const(realSubgroupsList)){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getStudentsSetString(gt.rules.internalSubgroupsList[realSubgroup], SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
		}
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTd(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;

				QList<int> allActivitiesList;
				QSet<int> allActivitiesSet;
				//allActivities.clear();
				allActivitiesList.append(students_timetable_weekly[realSubgroup][day][hour]);
				allActivitiesSet.insert(students_timetable_weekly[realSubgroup][day][hour]);
				bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
				if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
					tmpString+=writeActivityStudents(htmlLevel, students_timetable_weekly[realSubgroup][day][hour], day, hour, subgroupNotAvailableDayHour[realSubgroup][day][hour], true, false,
					 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_CODES,
					 gt.rules.internalSubgroupsList[realSubgroup]->name
					 );
				} else{
					tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_CODES
					);
				}
			}

			if(USE_DUMMY_TH_TR_TD){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=dummyTd(10);
				}
				else{
					if(rday%2==1)
						tmpString+=dummyTd(10);
				}
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getStudentsSetString(gt.rules.internalSubgroupsList[realSubgroup], SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(USE_DUMMY_TH_TR_TD){
		int additionalSpan = (gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek-1 : gt.rules.nRealDaysPerWeek-1);
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		else
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	else{
		int additionalSpan = 0;
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		else
			tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleSubgroupsTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, int maxSubgroups, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames, const QList<int>& subgroupsSortedOrder){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));

	QList<int> realSubgroupsList;
	int currentCount=0;
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups && currentCount<maxSubgroups; subgroup++){
		int realSubgroup;
		if(subgroupsSortedOrder!=QList<int>())
			realSubgroup=subgroupsSortedOrder.at(subgroup);
		else
			realSubgroup=subgroup;

		if(!excludedNames.contains(subgroup)){
			currentCount++;
			realSubgroupsList.append(realSubgroup);
			excludedNames.insert(subgroup);
		}
	}
	if(PRINT_RTL)
		std::reverse(realSubgroupsList.begin(), realSubgroupsList.end());

	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	for(int realSubgroup : std::as_const(realSubgroupsList)){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=getStudentsSetString(gt.rules.internalSubgroupsList[realSubgroup], SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int realHour=0; realHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); realHour++){
		int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
		int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);
		
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}
			else{
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}

			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			else
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}

		for(int realSubgroup : std::as_const(realSubgroupsList)){
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			allActivitiesList.append(students_timetable_weekly[realSubgroup][day][hour]);
			allActivitiesSet.insert(students_timetable_weekly[realSubgroup][day][hour]);
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, students_timetable_weekly[realSubgroup][day][hour], day, hour, subgroupNotAvailableDayHour[realSubgroup][day][hour], false, true,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_CODES,
				 gt.rules.internalSubgroupsList[realSubgroup]->name);
			} else{
				tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_CODES
				 );
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			else{
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleSubgroupsTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, int maxSubgroups, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames, const QList<int>& subgroupsSortedOrder){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));

	QList<int> realSubgroupsList;
	int currentCount=0;
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups && currentCount<maxSubgroups; subgroup++){
		int realSubgroup;
		if(subgroupsSortedOrder!=QList<int>())
			realSubgroup=subgroupsSortedOrder.at(subgroup);
		else
			realSubgroup=subgroup;

		if(!excludedNames.contains(subgroup)){
			currentCount++;
			realSubgroupsList.append(realSubgroup);
			excludedNames.insert(subgroup);
		}
	}

	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(realDay, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	else
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(realDay, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
		int realHour;
		if(!PRINT_RTL)
			realHour=rrealHour;
		else
			realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+=getHourString(realHour, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		else
			tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int realSubgroup : std::as_const(realSubgroupsList)){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getStudentsSetString(gt.rules.internalSubgroupsList[realSubgroup], SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
		}
		for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
			int realHour;
			if(!PRINT_RTL)
				realHour=rrealHour;
			else
				realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;

			int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
			int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);

			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			allActivitiesList.append(students_timetable_weekly[realSubgroup][day][hour]);
			allActivitiesSet.insert(students_timetable_weekly[realSubgroup][day][hour]);
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, students_timetable_weekly[realSubgroup][day][hour], day, hour, subgroupNotAvailableDayHour[realSubgroup][day][hour], true, false,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_CODES,
				 gt.rules.internalSubgroupsList[realSubgroup]->name
				 );
			} else{
				tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_ROOMS_CODES
				 );
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getStudentsSetString(gt.rules.internalSubgroupsList[realSubgroup], SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBGROUPS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleGroupsTimetableDaysHorizontalHtml(int htmlLevel, int group, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool detailed, bool repeatNames){
	assert(group>=0);
	assert(group<gt.rules.internalGroupsList.size());
	QString tmpString;

	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table_"+hashStudentIDsTimetable.value(gt.rules.internalGroupsList.at(group)->name);
	else
		tmpString+="    <table align=\"center\" id=\"table_"+hashStudentIDsTimetable.value(gt.rules.internalGroupsList.at(group)->name);
	tmpString+="\" border=\"1\"";
	if(group%2==0) tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";
	
	QStringList tl;
	if(!gt.rules.institutionName.isEmpty())
		tl.append("<span class=\"institution\">"+protect2(gt.rules.institutionName)+"</span>");
	QString nm=getStudentsSetString(gt.rules.internalGroupsList.at(group), SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_CODES);
	if(!nm.isEmpty())
		tl.append("<span class=\"name\">"+nm+"</span>");
	QString c1=gt.rules.internalGroupsList.at(group)->comments;
	if(SETTINGS_TIMETABLES_PRINT_GROUPS_COMMENTS && !c1.isEmpty()){
		QString c2=protect2(c1);
		c2.replace("\n", "<br />\n");
		tl.append("<span class=\"comment\">"+c2+"</span>");
	}
	
	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;

	tmpString+="      <caption>"+tl.join("<br />")+"</caption>\n";
	tmpString+="      <thead>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";
	
	for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
		int td;
		if(!PRINT_RTL)
			td=rtd;
		else
			td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";

		tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int th=0; th<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); th++){
		int hour;
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			hour=th;
		else
			hour=th%gt.rules.nHoursPerDay;
		
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
			int td;
			if(!PRINT_RTL)
				td=rtd;
			else
				td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
			
			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			bool isNotAvailable=true;
			for(int sg=0; sg<gt.rules.internalGroupsList[group]->subgroupsList.size(); sg++){
				StudentsSubgroup* sts=gt.rules.internalGroupsList[group]->subgroupsList[sg];
				int subgroup=sts->indexInInternalSubgroupsList;
				if(!(allActivitiesSet.contains(students_timetable_weekly[subgroup][day][hour]))){
					allActivitiesList.append(students_timetable_weekly[subgroup][day][hour]);
					allActivitiesSet.insert(students_timetable_weekly[subgroup][day][hour]);
				}
				if(!subgroupNotAvailableDayHour[subgroup][day][hour])
					isNotAvailable=false;
			}
			assert(!allActivitiesList.isEmpty());
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, allActivitiesList[0], day, hour, isNotAvailable, false, true,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_CODES,
				 gt.rules.internalGroupsList[group]->name
				 );
			} else{
				if(!detailed) tmpString+="          <td>"+protect2(/*STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES*/translatedThreeQuestionMarks())+"</td>\n";
				else{
					tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_CODES
					);
				}
			}

			addActivitiesLegend(allActivitiesList, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
			 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
			 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
			 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_LEGEND,
			 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LEGEND,
			 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_LEGEND);
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleGroupsTimetableDaysVerticalHtml(int htmlLevel, int group, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool detailed, bool repeatNames){
	assert(group>=0);
	assert(group<gt.rules.internalGroupsList.size());
	QString tmpString;

	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table_"+hashStudentIDsTimetable.value(gt.rules.internalGroupsList.at(group)->name);
	else
		tmpString+="    <table align=\"center\" id=\"table_"+hashStudentIDsTimetable.value(gt.rules.internalGroupsList.at(group)->name);
	tmpString+="\" border=\"1\"";
	if(group%2==0) tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	QStringList tl;
	if(!gt.rules.institutionName.isEmpty())
		tl.append("<span class=\"institution\">"+protect2(gt.rules.institutionName)+"</span>");
	QString nm=getStudentsSetString(gt.rules.internalGroupsList.at(group), SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_CODES);
	if(!nm.isEmpty())
		tl.append("<span class=\"name\">"+nm+"</span>");
	QString c1=gt.rules.internalGroupsList.at(group)->comments;
	if(SETTINGS_TIMETABLES_PRINT_GROUPS_COMMENTS && !c1.isEmpty()){
		QString c2=protect2(c1);
		c2.replace("\n", "<br />\n");
		tl.append("<span class=\"comment\">"+c2+"</span>");
	}

	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;
	
	tmpString+="      <caption>"+tl.join("<br />")+"</caption>\n";
	tmpString+="      <thead>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
		int th;
		if(!PRINT_RTL)
			th=rth;
		else
			th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";

		tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int td=0; td<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); td++){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
			int th;
			if(!PRINT_RTL)
				th=rth;
			else
				th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;

			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			int hour;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				hour=th;
			else
				hour=th%gt.rules.nHoursPerDay;
			
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			bool isNotAvailable=true;
			for(int sg=0; sg<gt.rules.internalGroupsList.at(group)->subgroupsList.size(); sg++){
				StudentsSubgroup* sts=gt.rules.internalGroupsList.at(group)->subgroupsList[sg];
				int subgroup=sts->indexInInternalSubgroupsList;
				if(!(allActivitiesSet.contains(students_timetable_weekly[subgroup][day][hour]))){
					allActivitiesList.append(students_timetable_weekly[subgroup][day][hour]);
					allActivitiesSet.insert(students_timetable_weekly[subgroup][day][hour]);
				}
				if(!subgroupNotAvailableDayHour[subgroup][day][hour])
					isNotAvailable=false;
			}
			assert(!allActivitiesList.isEmpty());
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, allActivitiesList[0], day, hour, isNotAvailable, true, false,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_CODES,
				 gt.rules.internalGroupsList[group]->name
				 );
			} else{
				if(!detailed) tmpString+="          <td>"+protect2(/*STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES*/translatedThreeQuestionMarks())+"</td>\n";
				else{
					tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_CODES
					 );
				}
			}

			addActivitiesLegend(allActivitiesList, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
			 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
			 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
			 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_LEGEND,
			 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LEGEND,
			 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_LEGEND);
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_GROUPS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleGroupsTimetableTimeVerticalHtml(int htmlLevel, int maxGroups, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool detailed, bool repeatNames){
	QList<int> groupsList;

	int currentCount=0;
	for(int group=0; group<gt.rules.internalGroupsList.size() && currentCount<maxGroups; group++){
		if(!excludedNames.contains(group)){
			currentCount++;
			groupsList.append(group);
			excludedNames.insert(group);
		}
	}
	if(PRINT_RTL)
		std::reverse(groupsList.begin(), groupsList.end());

	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table";
	else
		tmpString+="    <table align=\"center\" id=\"table";
	tmpString+="\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	for(int group : std::as_const(groupsList)){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=getStudentsSetString(gt.rules.internalGroupsList.at(group), SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			tmpString+="        <tr>\n";
			
			if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}

				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				if(gt.rules.mode!=MORNINGS_AFTERNOONS)
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				else
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}
			
			for(int group : std::as_const(groupsList)){
				QList<int> allActivitiesList;
				QSet<int> allActivitiesSet;
				//allActivities.clear();
				bool isNotAvailable=true;
				for(int sg=0; sg<gt.rules.internalGroupsList.at(group)->subgroupsList.size(); sg++){
					StudentsSubgroup* sts=gt.rules.internalGroupsList.at(group)->subgroupsList[sg];
					int subgroup=sts->indexInInternalSubgroupsList;
					if(!(allActivitiesSet.contains(students_timetable_weekly[subgroup][day][hour]))){
						allActivitiesList.append(students_timetable_weekly[subgroup][day][hour]);
						allActivitiesSet.insert(students_timetable_weekly[subgroup][day][hour]);
					}
					if(!subgroupNotAvailableDayHour[subgroup][day][hour])
						isNotAvailable=false;
				}
				assert(!allActivitiesList.isEmpty());
				bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
				if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
					tmpString+=writeActivityStudents(htmlLevel, allActivitiesList[0], day, hour, isNotAvailable, false, true,
					 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_CODES,
					 gt.rules.internalGroupsList[group]->name
					 );
				} else{
					if(!detailed) tmpString+="          <td>"+protect2(/*STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES*/translatedThreeQuestionMarks())+"</td>\n";
					else{
						tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
						 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_CODES,
						 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
						 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_CODES,
						 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES,
						 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_CODES
						 );
					}
				}
			}
			if((!PRINT_RTL && repeatNames) || PRINT_RTL){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
			}
			tmpString+="        </tr>\n";
		}

		if(USE_DUMMY_TH_TR_TD){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(day!=gt.rules.nDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, currentCount+2+(repeatNames?2:0));
			}
			else{
				if(day%2==1 && day/2!=gt.rules.nRealDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, currentCount+2+(repeatNames?2:0));
			}
		}
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleGroupsTimetableTimeHorizontalHtml(int htmlLevel, int maxGroups, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool detailed, bool repeatNames){
	QList<int> groupsList;
	int currentCount=0;
	for(int group=0; group<gt.rules.internalGroupsList.size() && currentCount<maxGroups; group++){
		if(!excludedNames.contains(group)){
			currentCount++;
			groupsList.append(group);
			excludedNames.insert(group);
		}
	}

	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table";
	else
		tmpString+="    <table align=\"center\" id=\"table";
	tmpString+="\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(day, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(day, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;
				
				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nRealHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nRealHoursPerDay-1-rhour;

				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getRealHourString(hour, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int group : std::as_const(groupsList)){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getStudentsSetString(gt.rules.internalGroupsList.at(group), SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
		}

		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTd(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;

				QList<int> allActivitiesList;
				QSet<int> allActivitiesSet;
				//allActivities.clear();
				bool isNotAvailable=true;
				for(int sg=0; sg<gt.rules.internalGroupsList.at(group)->subgroupsList.size(); sg++){
					StudentsSubgroup* sts=gt.rules.internalGroupsList.at(group)->subgroupsList[sg];
					int subgroup=sts->indexInInternalSubgroupsList;
					if(!(allActivitiesSet.contains(students_timetable_weekly[subgroup][day][hour]))){
						allActivitiesList.append(students_timetable_weekly[subgroup][day][hour]);
						allActivitiesSet.insert(students_timetable_weekly[subgroup][day][hour]);
					}
					if(!subgroupNotAvailableDayHour[subgroup][day][hour])
						isNotAvailable=false;
				}
				assert(!allActivitiesList.isEmpty());
				bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
				if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
					tmpString+=writeActivityStudents(htmlLevel, allActivitiesList[0], day, hour, isNotAvailable, true, false,
					 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_CODES,
					 gt.rules.internalGroupsList[group]->name
					 );
				} else{
					if(!detailed) tmpString+="          <td>"+protect2(/*STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES*/translatedThreeQuestionMarks())+"</td>\n";
					else{
						tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
						 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_CODES,
						 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
						 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_CODES,
						 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES,
						 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_CODES
						);
					}
				}
			}

			if(USE_DUMMY_TH_TR_TD){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=dummyTd(10);
				}
				else{
					if(rday%2==1)
						tmpString+=dummyTd(10);
				}
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getStudentsSetString(gt.rules.internalGroupsList.at(group), SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(USE_DUMMY_TH_TR_TD){
		int additionalSpan = (gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek-1 : gt.rules.nRealDaysPerWeek-1);
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		else
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	else{
		int additionalSpan = 0;
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		else
			tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleGroupsTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, int maxGroups, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool detailed, bool repeatNames){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));

	QList<int> groupsList;
	int currentCount=0;
	for(int group=0; group<gt.rules.internalGroupsList.size() && currentCount<maxGroups; group++){
		if(!excludedNames.contains(group)){
			currentCount++;
			groupsList.append(group);
			excludedNames.insert(group);
		}
	}
	if(PRINT_RTL)
		std::reverse(groupsList.begin(), groupsList.end());
	
	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	for(int group : std::as_const(groupsList)){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=getStudentsSetString(gt.rules.internalGroupsList.at(group), SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
	}
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int realHour=0; realHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); realHour++){
		int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
		int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);

		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}
			else{
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}

			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			else
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}

		for(int group : std::as_const(groupsList)){
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			bool isNotAvailable=true;
			for(int sg=0; sg<gt.rules.internalGroupsList.at(group)->subgroupsList.size(); sg++){
				StudentsSubgroup* sts=gt.rules.internalGroupsList.at(group)->subgroupsList[sg];
				int subgroup=sts->indexInInternalSubgroupsList;
				if(!(allActivitiesSet.contains(students_timetable_weekly[subgroup][day][hour]))){
					allActivitiesList.append(students_timetable_weekly[subgroup][day][hour]);
					allActivitiesSet.insert(students_timetable_weekly[subgroup][day][hour]);
				}
				if(!subgroupNotAvailableDayHour[subgroup][day][hour])
					isNotAvailable=false;
			}
			assert(!allActivitiesList.isEmpty());
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, allActivitiesList[0], day, hour, isNotAvailable, false, true,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_CODES,
				 gt.rules.internalGroupsList[group]->name
				 );
			} else{
				if(!detailed) tmpString+="          <td>"+protect2(/*STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES*/translatedThreeQuestionMarks())+"</td>\n";
				else{
					tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_CODES
					 );
				}
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			else{
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleGroupsTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, int maxGroups, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool detailed, bool repeatNames){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));

	QList<int> groupsList;
	int currentCount=0;
	for(int group=0; group<gt.rules.internalGroupsList.size() && currentCount<maxGroups; group++){
		if(!excludedNames.contains(group)){
			currentCount++;
			groupsList.append(group);
			excludedNames.insert(group);
		}
	}
	
	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(realDay, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	else
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(realDay, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
		int realHour;
		if(!PRINT_RTL)
			realHour=rrealHour;
		else
			realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+=getHourString(realHour, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		else
			tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int group : std::as_const(groupsList)){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getStudentsSetString(gt.rules.internalGroupsList.at(group), SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
		}
		for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
			int realHour;
			if(!PRINT_RTL)
				realHour=rrealHour;
			else
				realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;

			int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
			int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);

			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			bool isNotAvailable=true;
			for(int sg=0; sg<gt.rules.internalGroupsList.at(group)->subgroupsList.size(); sg++){
				StudentsSubgroup* sts=gt.rules.internalGroupsList.at(group)->subgroupsList[sg];
				int subgroup=sts->indexInInternalSubgroupsList;
				if(!(allActivitiesSet.contains(students_timetable_weekly[subgroup][day][hour]))){
					allActivitiesList.append(students_timetable_weekly[subgroup][day][hour]);
					allActivitiesSet.insert(students_timetable_weekly[subgroup][day][hour]);
				}
				if(!subgroupNotAvailableDayHour[subgroup][day][hour])
					isNotAvailable=false;
			}
			assert(!allActivitiesList.isEmpty());
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, allActivitiesList[0], day, hour, isNotAvailable, true, false,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_CODES,
				 gt.rules.internalGroupsList[group]->name);
			} else{
				if(!detailed) tmpString+="          <td>"+protect2(/*STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES*/translatedThreeQuestionMarks())+"</td>\n";
				else{
					tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_ROOMS_CODES
					 );
				}
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getStudentsSetString(gt.rules.internalGroupsList.at(group), SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_GROUPS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleYearsTimetableDaysHorizontalHtml(int htmlLevel, int year, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool detailed, bool repeatNames){
	assert(year>=0);
	assert(year<gt.rules.augmentedYearsList.size());
	QString tmpString;

	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table_"+hashStudentIDsTimetable.value(gt.rules.augmentedYearsList.at(year)->name);
	else
		tmpString+="    <table align=\"center\" id=\"table_"+hashStudentIDsTimetable.value(gt.rules.augmentedYearsList.at(year)->name);
	tmpString+="\" border=\"1\"";
	if(year%2==0)  tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	QStringList tl;
	if(!gt.rules.institutionName.isEmpty())
		tl.append("<span class=\"institution\">"+protect2(gt.rules.institutionName)+"</span>");
	QString nm=getStudentsSetString(gt.rules.augmentedYearsList.at(year), SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_CODES);
	if(!nm.isEmpty())
		tl.append("<span class=\"name\">"+nm+"</span>");
	QString c1=gt.rules.augmentedYearsList.at(year)->comments;
	if(SETTINGS_TIMETABLES_PRINT_YEARS_COMMENTS && !c1.isEmpty()){
		QString c2=protect2(c1);
		c2.replace("\n", "<br />\n");
		tl.append("<span class=\"comment\">"+c2+"</span>");
	}
	
	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;

	tmpString+="      <caption>"+tl.join("<br />")+"</caption>\n";
	tmpString+="      <thead>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
		int td;
		if(!PRINT_RTL)
			td=rtd;
		else
			td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		
		tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int th=0; th<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); th++){
		int hour;
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			hour=th;
		else
			hour=th%gt.rules.nHoursPerDay;
		
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
			int td;
			if(!PRINT_RTL)
				td=rtd;
			else
				td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
			
			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			bool isNotAvailable=true;
			for(int g=0; g<gt.rules.augmentedYearsList.at(year)->groupsList.size(); g++){
				StudentsGroup* stg=gt.rules.augmentedYearsList.at(year)->groupsList[g];
				for(int sg=0; sg<stg->subgroupsList.size(); sg++){
					StudentsSubgroup* sts=stg->subgroupsList[sg];
					int subgroup=sts->indexInInternalSubgroupsList;
					if(!(allActivitiesSet.contains(students_timetable_weekly[subgroup][day][hour]))){
						allActivitiesList.append(students_timetable_weekly[subgroup][day][hour]);
						allActivitiesSet.insert(students_timetable_weekly[subgroup][day][hour]);
					}
					if(!subgroupNotAvailableDayHour[subgroup][day][hour])
						isNotAvailable=false;
				}
			}
			assert(!allActivitiesList.isEmpty());
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, allActivitiesList[0], day, hour, isNotAvailable, false, true,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_CODES,
				 gt.rules.augmentedYearsList.at(year)->name
				 );
			} else{
				if(!detailed) tmpString+="          <td>"+protect2(/*STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES*/translatedThreeQuestionMarks())+"</td>\n";
				else{
					tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_CODES
					 );
				}
			}

			addActivitiesLegend(allActivitiesList, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
			 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
			 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
			 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_LEGEND,
			 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LEGEND,
			 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_LEGEND);
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleYearsTimetableDaysVerticalHtml(int htmlLevel, int year, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool detailed, bool repeatNames){
	assert(year>=0);
	assert(year<gt.rules.augmentedYearsList.size());
	QString tmpString;
	
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table_"+hashStudentIDsTimetable.value(gt.rules.augmentedYearsList.at(year)->name);
	else
		tmpString+="    <table align=\"center\" id=\"table_"+hashStudentIDsTimetable.value(gt.rules.augmentedYearsList.at(year)->name);
	tmpString+="\" border=\"1\"";
	if(year%2==0)  tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	QStringList tl;
	if(!gt.rules.institutionName.isEmpty())
		tl.append("<span class=\"institution\">"+protect2(gt.rules.institutionName)+"</span>");
	QString nm=getStudentsSetString(gt.rules.augmentedYearsList.at(year), SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_CODES);
	if(!nm.isEmpty())
		tl.append("<span class=\"name\">"+nm+"</span>");
	QString c1=gt.rules.augmentedYearsList.at(year)->comments;
	if(SETTINGS_TIMETABLES_PRINT_YEARS_COMMENTS && !c1.isEmpty()){
		QString c2=protect2(c1);
		c2.replace("\n", "<br />\n");
		tl.append("<span class=\"comment\">"+c2+"</span>");
	}

	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;
	
	tmpString+="      <caption>"+tl.join("<br />")+"</caption>\n";
	tmpString+="      <thead>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
		int th;
		if(!PRINT_RTL)
			th=rth;
		else
			th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";

		tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int td=0; td<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); td++){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
			int th;
			if(!PRINT_RTL)
				th=rth;
			else
				th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;

			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			int hour;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				hour=th;
			else
				hour=th%gt.rules.nHoursPerDay;
			
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			bool isNotAvailable=true;
			for(int g=0; g<gt.rules.augmentedYearsList.at(year)->groupsList.size(); g++){
				StudentsGroup* stg=gt.rules.augmentedYearsList.at(year)->groupsList[g];
				for(int sg=0; sg<stg->subgroupsList.size(); sg++){
					StudentsSubgroup* sts=stg->subgroupsList[sg];
					int subgroup=sts->indexInInternalSubgroupsList;
					if(!(allActivitiesSet.contains(students_timetable_weekly[subgroup][day][hour]))){
						allActivitiesList.append(students_timetable_weekly[subgroup][day][hour]);
						allActivitiesSet.insert(students_timetable_weekly[subgroup][day][hour]);
					}
					if(!subgroupNotAvailableDayHour[subgroup][day][hour])
						isNotAvailable=false;
				}
			}
			assert(!allActivitiesList.isEmpty());
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, allActivitiesList[0], day, hour, isNotAvailable, true, false,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_CODES,
				 gt.rules.augmentedYearsList.at(year)->name
				 );
			} else{
				if(!detailed) tmpString+="          <td>"+protect2(/*STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES*/translatedThreeQuestionMarks())+"</td>\n";
				else{
					tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_CODES
					 );
				}
			}

			addActivitiesLegend(allActivitiesList, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
			 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
			 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
			 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_LEGEND,
			 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LEGEND,
			 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_LEGEND);
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_YEARS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleYearsTimetableTimeVerticalHtml(int htmlLevel, int maxYears, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool detailed, bool repeatNames){
	QList<int> yearsList;
	
	int currentCount=0;
	for(int year=0; year<gt.rules.augmentedYearsList.size() && currentCount<maxYears; year++){
		if(!excludedNames.contains(year)){
			currentCount++;
			yearsList.append(year);
			excludedNames.insert(year);
		}
	}
	if(PRINT_RTL)
		std::reverse(yearsList.begin(), yearsList.end());

	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table";
	else
		tmpString+="    <table align=\"center\" id=\"table";
	tmpString+="\" border=\"1\">\n";
	
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	for(int year : std::as_const(yearsList)){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=getStudentsSetString(gt.rules.augmentedYearsList.at(year), SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
	}
	
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			tmpString+="        <tr>\n";

			if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}

				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				if(gt.rules.mode!=MORNINGS_AFTERNOONS)
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				else
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			for(int year : std::as_const(yearsList)){
				QList<int> allActivitiesList;
				QSet<int> allActivitiesSet;
				//allActivities.clear();
				bool isNotAvailable=true;
				StudentsYear* sty=gt.rules.augmentedYearsList[year];
				for(int g=0; g<sty->groupsList.size(); g++){
					StudentsGroup* stg=sty->groupsList[g];
					for(int sg=0; sg<stg->subgroupsList.size(); sg++){
						StudentsSubgroup* sts=stg->subgroupsList[sg];
						int subgroup=sts->indexInInternalSubgroupsList;
						if(!(allActivitiesSet.contains(students_timetable_weekly[subgroup][day][hour]))){
							allActivitiesList.append(students_timetable_weekly[subgroup][day][hour]);
							allActivitiesSet.insert(students_timetable_weekly[subgroup][day][hour]);
						}
						if(!subgroupNotAvailableDayHour[subgroup][day][hour])
							isNotAvailable=false;
					}
				}
				assert(!allActivitiesList.isEmpty());
				bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
				if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
					tmpString+=writeActivityStudents(htmlLevel, allActivitiesList[0], day, hour, isNotAvailable, false, true,
					 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_CODES,
					 gt.rules.augmentedYearsList.at(year)->name
					 );
				} else{
					if(!detailed) tmpString+="          <td>"+protect2(/*STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES*/translatedThreeQuestionMarks())+"</td>\n";
					else{
						tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
						 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_CODES,
						 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
						 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_CODES,
						 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES,
						 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_CODES
						 );
					}
				}
			}
			if((!PRINT_RTL && repeatNames) || PRINT_RTL){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
			}
			tmpString+="        </tr>\n";
		}

		if(USE_DUMMY_TH_TR_TD){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(day!=gt.rules.nDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, currentCount+2+(repeatNames?2:0));
			}
			else{
				if(day%2==1 && day/2!=gt.rules.nRealDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, currentCount+2+(repeatNames?2:0));
			}
		}
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleYearsTimetableTimeHorizontalHtml(int htmlLevel, int maxYears, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool detailed, bool repeatNames){
	QList<int> yearsList;
	int currentCount=0;
	for(int year=0; year<gt.rules.augmentedYearsList.size() && currentCount<maxYears; year++){
		if(!excludedNames.contains(year)){
			currentCount++;
			yearsList.append(year);
			excludedNames.insert(year);
		}
	}

	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table";
	else
		tmpString+="    <table align=\"center\" id=\"table";
	tmpString+="\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(day, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(day, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;
				
				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nRealHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nRealHoursPerDay-1-rhour;

				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getRealHourString(hour, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int year : std::as_const(yearsList)){
		StudentsYear* sty=gt.rules.augmentedYearsList[year];
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getStudentsSetString(sty, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
		}

		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTd(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;

				QList<int> allActivitiesList;
				QSet<int> allActivitiesSet;
				//allActivities.clear();
				bool isNotAvailable=true;
				for(int g=0; g<sty->groupsList.size(); g++){
					StudentsGroup* stg=sty->groupsList[g];
					for(int sg=0; sg<stg->subgroupsList.size(); sg++){
						StudentsSubgroup* sts=stg->subgroupsList[sg];
						int subgroup=sts->indexInInternalSubgroupsList;
						if(!(allActivitiesSet.contains(students_timetable_weekly[subgroup][day][hour]))){
							allActivitiesList.append(students_timetable_weekly[subgroup][day][hour]);
							allActivitiesSet.insert(students_timetable_weekly[subgroup][day][hour]);
						}
						if(!subgroupNotAvailableDayHour[subgroup][day][hour])
							isNotAvailable=false;
					}
				}
				assert(!allActivitiesList.isEmpty());
				bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
				if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
					tmpString+=writeActivityStudents(htmlLevel, allActivitiesList[0], day, hour, isNotAvailable, true, false,
					 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_CODES,
					 gt.rules.augmentedYearsList.at(year)->name
					 );
				} else{
					if(!detailed) tmpString+="          <td>"+protect2(/*STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES*/translatedThreeQuestionMarks())+"</td>\n";
					else{
						tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
						 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_CODES,
						 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
						 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_CODES,
						 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES,
						 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_CODES
						 );
					}
				}
			}

			if(USE_DUMMY_TH_TR_TD){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=dummyTd(10);
				}
				else{
					if(rday%2==1)
						tmpString+=dummyTd(10);
				}
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getStudentsSetString(sty, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(USE_DUMMY_TH_TR_TD){
		int additionalSpan = (gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek-1 : gt.rules.nRealDaysPerWeek-1);
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		else
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	else{
		int additionalSpan = 0;
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		else
			tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleYearsTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, int maxYears, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool detailed, bool repeatNames){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));

	QList<int> yearsList;
	int currentCount=0;
	for(int year=0; year<gt.rules.augmentedYearsList.size() && currentCount<maxYears; year++){
		if(!excludedNames.contains(year)){
			currentCount++;
			yearsList.append(year);
			excludedNames.insert(year);
		}
	}
	if(PRINT_RTL)
		std::reverse(yearsList.begin(), yearsList.end());

	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	for(int year : std::as_const(yearsList)){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=getStudentsSetString(gt.rules.augmentedYearsList.at(year), SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
	}
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int realHour=0; realHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); realHour++){
		int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
		int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);

		tmpString+="        <tr>\n";

		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}
			else{
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}

			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			else
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}

		for(int year : std::as_const(yearsList)){
			StudentsYear* sty=gt.rules.augmentedYearsList[year];
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			bool isNotAvailable=true;
			for(int g=0; g<sty->groupsList.size(); g++){
				StudentsGroup* stg=sty->groupsList[g];
				for(int sg=0; sg<stg->subgroupsList.size(); sg++){
					StudentsSubgroup* sts=stg->subgroupsList[sg];
					int subgroup=sts->indexInInternalSubgroupsList;
					if(!(allActivitiesSet.contains(students_timetable_weekly[subgroup][day][hour]))){
						allActivitiesList.append(students_timetable_weekly[subgroup][day][hour]);
						allActivitiesSet.insert(students_timetable_weekly[subgroup][day][hour]);
					}
					if(!subgroupNotAvailableDayHour[subgroup][day][hour])
						isNotAvailable=false;
				}
			}
			assert(!allActivitiesList.isEmpty());
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, allActivitiesList[0], day, hour, isNotAvailable, false, true,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_CODES,
				 gt.rules.augmentedYearsList.at(year)->name
				 );
			} else{
				if(!detailed) tmpString+="          <td>"+protect2(/*STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES*/translatedThreeQuestionMarks())+"</td>\n";
				else{
					tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_CODES
					 );
				}
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			else{
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleYearsTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, int maxYears, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool detailed, bool repeatNames){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));

	QList<int> yearsList;
	int currentCount=0;
	for(int year=0; year<gt.rules.augmentedYearsList.size() && currentCount<maxYears; year++){
		if(!excludedNames.contains(year)){
			currentCount++;
			yearsList.append(year);
			excludedNames.insert(year);
		}
	}

	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(realDay, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	else
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(realDay, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
		int realHour;
		if(!PRINT_RTL)
			realHour=rrealHour;
		else
			realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+=getHourString(realHour, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		else
			tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int year : std::as_const(yearsList)){
		StudentsYear* sty=gt.rules.augmentedYearsList[year];
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getStudentsSetString(sty, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
		}
		for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
			int realHour;
			if(!PRINT_RTL)
				realHour=rrealHour;
			else
				realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;

			int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
			int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);

			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			bool isNotAvailable=true;
			for(int g=0; g<sty->groupsList.size(); g++){
				StudentsGroup* stg=sty->groupsList[g];
				for(int sg=0; sg<stg->subgroupsList.size(); sg++){
					StudentsSubgroup* sts=stg->subgroupsList[sg];
					int subgroup=sts->indexInInternalSubgroupsList;
					if(!(allActivitiesSet.contains(students_timetable_weekly[subgroup][day][hour]))){
						allActivitiesList.append(students_timetable_weekly[subgroup][day][hour]);
						allActivitiesSet.insert(students_timetable_weekly[subgroup][day][hour]);
					}
					if(!subgroupNotAvailableDayHour[subgroup][day][hour])
						isNotAvailable=false;
				}
			}
			assert(!allActivitiesList.isEmpty());
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityStudents(htmlLevel, allActivitiesList[0], day, hour, isNotAvailable, true, false,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_CODES,
				 gt.rules.augmentedYearsList.at(year)->name
				 );
			} else{
				if(!detailed) tmpString+="          <td>"+protect2(/*STRING_SEVERAL_ACTIVITIES_IN_LESS_DETAILED_TABLES*/translatedThreeQuestionMarks())+"</td>\n";
				else{
					tmpString+=writeActivitiesStudents(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_ROOMS_CODES
					 );
				}
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getStudentsSetString(sty, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_YEARS_TIME_HV_PRINT_STUDENTS_CODES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleAllActivitiesTimetableDaysHorizontalHtml(int htmlLevel, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;

	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"<br />\n"+protect2(tr("All Activities"))+"</caption>\n";
	tmpString+="      <thead>\n";
	/*tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+tr("All Activities")+"</th>\n";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";*/
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
		int td;
		if(!PRINT_RTL)
			td=rtd;
		else
			td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";

		tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int th=0; th<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); th++){
		int hour;
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			hour=th;
		else
			hour=th%gt.rules.nHoursPerDay;
		
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
			int td;
			if(!PRINT_RTL)
				td=rtd;
			else
				td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
			
			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			if(activitiesAtTimeList[day][hour].isEmpty()){
				if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
					tmpString+=writeBreakSlot(htmlLevel, "");
				} else {
					tmpString+=writeEmpty(htmlLevel);
				}
			} else {
				tmpString+=writeActivitiesStudents(htmlLevel, activitiesAtTimeList[day][hour],
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_CODES
				 );

				addActivitiesLegend(activitiesAtTimeList[day][hour], usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_LEGEND,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_LEGEND,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_LEGEND,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_LEGEND);
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleAllActivitiesTimetableDaysVerticalHtml(int htmlLevel, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;

	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"<br />\n"+protect2(tr("All Activities"))+"</caption>\n";
	tmpString+="      <thead>\n";
	/*tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+tr("All Activities")+"</th>\n";
	if(repeatNames){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";*/
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
		int th;
		if(!PRINT_RTL)
			th=rth;
		else
			th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";

		tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int td=0; td<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); td++){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
			int th;
			if(!PRINT_RTL)
				th=rth;
			else
				th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;

			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			int hour;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				hour=th;
			else
				hour=th%gt.rules.nHoursPerDay;
			
			if(activitiesAtTimeList[day][hour].isEmpty()){
				if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
					tmpString+=writeBreakSlot(htmlLevel, "");
				} else {
					tmpString+=writeEmpty(htmlLevel);
				}
			} else {
				tmpString+=writeActivitiesStudents(htmlLevel, activitiesAtTimeList[day][hour],
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_CODES
				 );

				addActivitiesLegend(activitiesAtTimeList[day][hour], usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_LEGEND,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_LEGEND,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_LEGEND,
				 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_LEGEND);
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITIES_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleAllActivitiesTimetableTimeVerticalHtml(int htmlLevel, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(htmlLevel>=2)
		tmpString+="          <th class=\"xAxis\">";
	else
		tmpString+="          <th>";
	tmpString+=protect2(tr("All Activities"));
	tmpString+="</th>\n";

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td>"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			tmpString+="        <tr>\n";

			if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}

				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				if(gt.rules.mode!=MORNINGS_AFTERNOONS)
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				else
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(activitiesAtTimeList[day][hour].isEmpty()){
				if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
					tmpString+=writeBreakSlot(htmlLevel, "");
				} else {
					tmpString+=writeEmpty(htmlLevel);
				}
			} else {
				tmpString+=writeActivitiesStudents(htmlLevel, activitiesAtTimeList[day][hour],
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_CODES
				 );
			}
			if((!PRINT_RTL && repeatNames) || PRINT_RTL){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
			}
			tmpString+="        </tr>\n";
		}

		if(USE_DUMMY_TH_TR_TD){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(day!=gt.rules.nDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, 1+2+(repeatNames?2:0));
			}
			else{
				if(day%2==1 && day/2!=gt.rules.nRealDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, 1+2+(repeatNames?2:0));
			}
		}
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td>"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td>"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleAllActivitiesTimetableTimeHorizontalHtml(int htmlLevel, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(day, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(day, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;
				
				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nRealHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nRealHoursPerDay-1-rhour;

				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getRealHourString(hour, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	tmpString+="        <tr>\n";
	if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(tr("All Activities"))+"</th>\n";
	}

	if(USE_DUMMY_TH_TR_TD)
		tmpString+=dummyTd(10);

	for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
		int day;
		if(!PRINT_RTL)
			day=rday;
		else
			day=gt.rules.nDaysPerWeek-1-rday;

		for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
			int hour;
			if(!PRINT_RTL)
				hour=rhour;
			else
				hour=gt.rules.nHoursPerDay-1-rhour;

			if(activitiesAtTimeList[day][hour].isEmpty()){
				if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
					tmpString+=writeBreakSlot(htmlLevel, "");
				} else {
					tmpString+=writeEmpty(htmlLevel);
				}
			} else {
				tmpString+=writeActivitiesStudents(htmlLevel, activitiesAtTimeList[day][hour],
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_CODES
				 );
			}
		}

		if(USE_DUMMY_TH_TR_TD){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				tmpString+=dummyTd(10);
			}
			else{
				if(rday%2==1)
					tmpString+=dummyTd(10);
			}
		}
	}
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(tr("All Activities"))+"</th>\n";
	}
	tmpString+="        </tr>\n";
	//workaround begin.
	if(USE_DUMMY_TH_TR_TD){
		int additionalSpan = (gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek-1 : gt.rules.nRealDaysPerWeek-1);
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		else
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	else{
		int additionalSpan = 0;
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		else
			tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleAllActivitiesTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));
	
	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(htmlLevel>=2)
		tmpString+="          <th class=\"xAxis\">";
	else
		tmpString+="          <th>";
	tmpString+=protect2(tr("All Activities"));
	tmpString+="</th>\n";

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td>"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int realHour=0; realHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); realHour++){
		int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
		int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);

		tmpString+="        <tr>\n";

		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}
			else{
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}

			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			else
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}

		if(activitiesAtTimeList[day][hour].isEmpty()){
			if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
				tmpString+=writeBreakSlot(htmlLevel, "");
			} else {
				tmpString+=writeEmpty(htmlLevel);
			}
		} else {
			tmpString+=writeActivitiesStudents(htmlLevel, activitiesAtTimeList[day][hour],
			 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
			 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_CODES
			 );
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			else{
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td>"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td>"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleAllActivitiesTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));

	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(realDay, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	else
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(realDay, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
		int realHour;
		if(!PRINT_RTL)
			realHour=rrealHour;
		else
			realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+=getHourString(realHour, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		else
			tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	tmpString+="        <tr>\n";
	if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(tr("All Activities"))+"</th>\n";
	}
	for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
		int realHour;
		if(!PRINT_RTL)
			realHour=rrealHour;
		else
			realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;

		int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
		int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);

		if(activitiesAtTimeList[day][hour].isEmpty()){
			if(breakDayHour[day][hour] && PRINT_BREAK_TIME_SLOTS){
				tmpString+=writeBreakSlot(htmlLevel, "");
			} else {
				tmpString+=writeEmpty(htmlLevel);
			}
		} else {
			tmpString+=writeActivitiesStudents(htmlLevel, activitiesAtTimeList[day][hour],
			 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
			 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_SUBJECTS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_TEACHERS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_STUDENTS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITIES_TIME_HV_PRINT_ROOMS_CODES
			 );
		}
	}
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"yAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(tr("All Activities"))+"</th>\n";
	}
	tmpString+="        </tr>\n";
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleTeachersTimetableDaysHorizontalHtml(int htmlLevel, int teacher, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(teacher>=0);
	assert(teacher<gt.rules.nInternalTeachers);
	QString tmpString;
	QString teacher_name = gt.rules.internalTeachersList[teacher]->name;
	
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table_"+hashTeacherIDsTimetable.value(teacher_name)+"\" border=\"1\"";
	else
		tmpString+="    <table align=\"center\" id=\"table_"+hashTeacherIDsTimetable.value(teacher_name)+"\" border=\"1\"";
	if(teacher%2==0)  tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";
	
	QStringList tl;
	if(!gt.rules.institutionName.isEmpty())
		tl.append("<span class=\"institution\">"+protect2(gt.rules.institutionName)+"</span>");
	QString nm=getTeacherString(gt.rules.internalTeachersList[teacher], SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_CODES);
	if(!nm.isEmpty())
		tl.append("<span class=\"name\">"+nm+"</span>");
	QString c1=gt.rules.internalTeachersList[teacher]->comments;
	if(SETTINGS_TIMETABLES_PRINT_TEACHERS_COMMENTS && !c1.isEmpty()){
		QString c2=protect2(c1);
		c2.replace("\n", "<br />\n");
		tl.append("<span class=\"comment\">"+c2+"</span>");
	}
	
	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;

	tmpString+="      <caption>"+tl.join("<br />")+"</caption>\n";
	tmpString+="      <thead>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
		int td;
		if(!PRINT_RTL)
			td=rtd;
		else
			td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";

		tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int th=0; th<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); th++){
		int hour;
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			hour=th;
		else
			hour=th%gt.rules.nHoursPerDay;
		
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
			int td;
			if(!PRINT_RTL)
				td=rtd;
			else
				td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
			
			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			allActivitiesList.append(teachers_timetable_weekly[teacher][day][hour]);
			allActivitiesSet.insert(teachers_timetable_weekly[teacher][day][hour]);
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityTeacher(htmlLevel, teacher, day, hour, false, true,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_CODES,
				 teacher_name);
			} else{
				tmpString+=writeActivitiesTeachers(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_CODES
				 );
			}

			addActivitiesLegend(allActivitiesList, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
			 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
			 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
			 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LEGEND,
			 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_LEGEND,
			 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_LEGEND);
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleTeachersTimetableDaysVerticalHtml(int htmlLevel, int teacher, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(teacher>=0);
	assert(teacher<gt.rules.nInternalTeachers);
	QString tmpString;
	QString teacher_name = gt.rules.internalTeachersList[teacher]->name;

	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table_"+hashTeacherIDsTimetable.value(teacher_name)+"\" border=\"1\"";
	else
		tmpString+="    <table align=\"center\" id=\"table_"+hashTeacherIDsTimetable.value(teacher_name)+"\" border=\"1\"";
	if(teacher%2==0) tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";
	
	QStringList tl;
	if(!gt.rules.institutionName.isEmpty())
		tl.append("<span class=\"institution\">"+protect2(gt.rules.institutionName)+"</span>");
	QString nm=getTeacherString(gt.rules.internalTeachersList[teacher], SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_CODES);
	if(!nm.isEmpty())
		tl.append("<span class=\"name\">"+nm+"</span>");
	QString c1=gt.rules.internalTeachersList[teacher]->comments;
	if(SETTINGS_TIMETABLES_PRINT_TEACHERS_COMMENTS && !c1.isEmpty()){
		QString c2=protect2(c1);
		c2.replace("\n", "<br />\n");
		tl.append("<span class=\"comment\">"+c2+"</span>");
	}
	
	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;

	tmpString+="      <caption>"+tl.join("<br />")+"</caption>\n";
	tmpString+="      <thead>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
		int th;
		if(!PRINT_RTL)
			th=rth;
		else
			th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";

		tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int td=0; td<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); td++){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
			int th;
			if(!PRINT_RTL)
				th=rth;
			else
				th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;

			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			int hour;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				hour=th;
			else
				hour=th%gt.rules.nHoursPerDay;
			
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			allActivitiesList.append(teachers_timetable_weekly[teacher][day][hour]);
			allActivitiesSet.insert(teachers_timetable_weekly[teacher][day][hour]);
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityTeacher(htmlLevel, teacher, day, hour, true, false,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_CODES,
				 teacher_name);
			} else{
				tmpString+=writeActivitiesTeachers(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_CODES
				 );
			}

			addActivitiesLegend(allActivitiesList, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
			 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
			 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
			 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LEGEND,
			 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_LEGEND,
			 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_LEGEND);
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_TEACHERS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleTeachersTimetableTimeVerticalHtml(int htmlLevel, int maxTeachers, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	QList<int> teachersList;
	int currentCount=0;
	for(int teacher=0; teacher<gt.rules.nInternalTeachers && currentCount<maxTeachers; teacher++){
		if(!excludedNames.contains(teacher)){
			currentCount++;
			teachersList.append(teacher);
			excludedNames.insert(teacher);
		}
	}
	if(PRINT_RTL)
		std::reverse(teachersList.begin(), teachersList.end());

	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	for(int teacher : std::as_const(teachersList)){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=getTeacherString(gt.rules.internalTeachersList[teacher], SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			tmpString+="        <tr>\n";

			if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}

				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				if(gt.rules.mode!=MORNINGS_AFTERNOONS)
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				else
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			for(int teacher : std::as_const(teachersList)){
				QList<int> allActivitiesList;
				QSet<int> allActivitiesSet;
				//allActivities.clear();
				allActivitiesList.append(teachers_timetable_weekly[teacher][day][hour]);
				allActivitiesSet.insert(teachers_timetable_weekly[teacher][day][hour]);
				bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
				if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
					tmpString+=writeActivityTeacher(htmlLevel, teacher, day, hour, false, true,
					 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_CODES,
					 gt.rules.internalTeachersList[teacher]->name);
				} else {
					tmpString+=writeActivitiesTeachers(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_CODES
					);
				}
			}
			if((!PRINT_RTL && repeatNames) || PRINT_RTL){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
			}
			tmpString+="        </tr>\n";
		}

		if(USE_DUMMY_TH_TR_TD){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(day!=gt.rules.nDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, currentCount+2+(repeatNames?2:0));
			}
			else{
				if(day%2==1 && day/2!=gt.rules.nRealDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, currentCount+2+(repeatNames?2:0));
			}
		}
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleTeachersTimetableTimeHorizontalHtml(int htmlLevel, int maxTeachers, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	QList<int> teachersList;
	int currentCount=0;
	for(int teacher=0; teacher<gt.rules.nInternalTeachers && currentCount<maxTeachers; teacher++){
		if(!excludedNames.contains(teacher)){
			currentCount++;
			teachersList.append(teacher);
			excludedNames.insert(teacher);
		}
	}
	
	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(day, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(day, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;
				
				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nRealHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nRealHoursPerDay-1-rhour;

				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getRealHourString(hour, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int teacher : std::as_const(teachersList)){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getTeacherString(gt.rules.internalTeachersList[teacher], SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES)+"</th>\n";
		}

		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTd(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;

				QList<int> allActivitiesList;
				QSet<int> allActivitiesSet;
				//allActivities.clear();
				allActivitiesList.append(teachers_timetable_weekly[teacher][day][hour]);
				allActivitiesSet.insert(teachers_timetable_weekly[teacher][day][hour]);
				bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
				if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
					tmpString+=writeActivityTeacher(htmlLevel, teacher, day, hour, true, false,
					 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_CODES,
					 gt.rules.internalTeachersList[teacher]->name);
				} else {
					tmpString+=writeActivitiesTeachers(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_CODES
					 );
				}
			}

			if(USE_DUMMY_TH_TR_TD){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=dummyTd(10);
				}
				else{
					if(rday%2==1)
						tmpString+=dummyTd(10);
				}
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getTeacherString(gt.rules.internalTeachersList[teacher], SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(USE_DUMMY_TH_TR_TD){
		int additionalSpan = (gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek-1 : gt.rules.nRealDaysPerWeek-1);
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		else
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	else{
		int additionalSpan = 0;
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		else
			tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleTeachersTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, int maxTeachers, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));
	
	QList<int> teachersList;
	int currentCount=0;
	for(int teacher=0; teacher<gt.rules.nInternalTeachers && currentCount<maxTeachers; teacher++){
		if(!excludedNames.contains(teacher)){
			currentCount++;
			teachersList.append(teacher);
			excludedNames.insert(teacher);
		}
	}
	if(PRINT_RTL)
		std::reverse(teachersList.begin(), teachersList.end());
	
	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	for(int teacher : std::as_const(teachersList)){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=getTeacherString(gt.rules.internalTeachersList[teacher], SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES)+"</th>\n";
	}
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int realHour=0; realHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); realHour++){
		int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
		int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);

		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}
			else{
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}

			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			else
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}

		for(int teacher : std::as_const(teachersList)){
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			allActivitiesList.append(teachers_timetable_weekly[teacher][day][hour]);
			allActivitiesSet.insert(teachers_timetable_weekly[teacher][day][hour]);
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityTeacher(htmlLevel, teacher, day, hour, false, true,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_CODES,
				 gt.rules.internalTeachersList[teacher]->name);
			} else {
				tmpString+=writeActivitiesTeachers(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_CODES
				 );
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			else{
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleTeachersTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, int maxTeachers, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));

	QList<int> teachersList;
	int currentCount=0;
	for(int teacher=0; teacher<gt.rules.nInternalTeachers && currentCount<maxTeachers; teacher++){
		if(!excludedNames.contains(teacher)){
			currentCount++;
			teachersList.append(teacher);
			excludedNames.insert(teacher);
		}
	}
	
	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(realDay, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	else
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(realDay, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
		int realHour;
		if(!PRINT_RTL)
			realHour=rrealHour;
		else
			realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+=getHourString(realHour, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		else
			tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int teacher : std::as_const(teachersList)){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getTeacherString(gt.rules.internalTeachersList[teacher], SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES)+"</th>\n";
		}
		
		for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
			int realHour;
			if(!PRINT_RTL)
				realHour=rrealHour;
			else
				realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;

			int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
			int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);

			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			allActivitiesList.append(teachers_timetable_weekly[teacher][day][hour]);
			allActivitiesSet.insert(teachers_timetable_weekly[teacher][day][hour]);
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityTeacher(htmlLevel, teacher, day, hour, true, false,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_CODES,
				 gt.rules.internalTeachersList[teacher]->name);
			} else {
				tmpString+=writeActivitiesTeachers(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_ROOMS_CODES
				 );
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getTeacherString(gt.rules.internalTeachersList[teacher], SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_TEACHERS_TIME_HV_PRINT_TEACHERS_CODES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleBuildingsTimetableDaysHorizontalHtml(int htmlLevel, int building, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(building>=0);
	assert(building<gt.rules.nInternalBuildings);
	QString tmpString;
	QString building_name = gt.rules.internalBuildingsList[building]->name;

	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table_"+hashBuildingIDsTimetable.value(building_name)+"\" border=\"1\"";
	else
		tmpString+="    <table align=\"center\" id=\"table_"+hashBuildingIDsTimetable.value(building_name)+"\" border=\"1\"";
	if(building%2==0)  tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	QStringList tl;
	if(!gt.rules.institutionName.isEmpty())
		tl.append("<span class=\"institution\">"+protect2(gt.rules.institutionName)+"</span>");
	QString nm=getBuildingString(gt.rules.internalBuildingsList[building], true, true, true);
	if(!nm.isEmpty())
		tl.append("<span class=\"name\">"+nm+"</span>");
	QString c1=gt.rules.internalBuildingsList[building]->comments;
	if(SETTINGS_TIMETABLES_PRINT_BUILDINGS_COMMENTS && !c1.isEmpty()){
		QString c2=protect2(c1);
		c2.replace("\n", "<br />\n");
		tl.append("<span class=\"comment\">"+c2+"</span>");
	}
	
	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;

	tmpString+="      <caption>"+tl.join("<br />")+"</caption>\n";
	tmpString+="      <thead>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
		int td;
		if(!PRINT_RTL)
			td=rtd;
		else
			td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";

		tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int th=0; th<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); th++){
		int hour;
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			hour=th;
		else
			hour=th%gt.rules.nHoursPerDay;
		
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
			int td;
			if(!PRINT_RTL)
				td=rtd;
			else
				td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
			
			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			
			allActivitiesList.append(buildings_timetable_weekly[building][day][hour]);
			for(int ai : std::as_const(buildings_timetable_weekly[building][day][hour]))
				allActivitiesSet.insert(ai);
			
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			//2024-08-16: 0 && ..., because otherwise we get a bug with the span if in the building there are two activities,
			//starting at the same time, with different durations.
			if(false && allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityBuilding(htmlLevel, building, day, hour, false, true,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_CODES
				 );
			} else {
				tmpString+=writeActivitiesBuildings(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_CODES
				 );
			}

			addActivitiesLegend(allActivitiesList, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
			 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
			 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
			 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_LEGEND,
			 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_LEGEND,
			 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_LEGEND);
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleBuildingsTimetableDaysVerticalHtml(int htmlLevel, int building, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(building>=0);
	assert(building<gt.rules.nInternalBuildings);
	QString tmpString;
	QString building_name = gt.rules.internalBuildingsList[building]->name;

	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table_"+hashBuildingIDsTimetable.value(building_name)+"\" border=\"1\"";
	else
		tmpString+="    <table align=\"center\" id=\"table_"+hashBuildingIDsTimetable.value(building_name)+"\" border=\"1\"";
	if(building%2==0)  tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	QStringList tl;
	if(!gt.rules.institutionName.isEmpty())
		tl.append("<span class=\"institution\">"+protect2(gt.rules.institutionName)+"</span>");
	QString nm=getBuildingString(gt.rules.internalBuildingsList[building], true, true, true);
	if(!nm.isEmpty())
		tl.append("<span class=\"name\">"+nm+"</span>");
	QString c1=gt.rules.internalBuildingsList[building]->comments;
	if(SETTINGS_TIMETABLES_PRINT_BUILDINGS_COMMENTS && !c1.isEmpty()){
		QString c2=protect2(c1);
		c2.replace("\n", "<br />\n");
		tl.append("<span class=\"comment\">"+c2+"</span>");
	}
	
	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;

	tmpString+="      <caption>"+tl.join("<br />")+"</caption>\n";
	tmpString+="      <thead>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
		int th;
		if(!PRINT_RTL)
			th=rth;
		else
			th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";

		tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int td=0; td<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); td++){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
			int th;
			if(!PRINT_RTL)
				th=rth;
			else
				th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;

			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			int hour;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				hour=th;
			else
				hour=th%gt.rules.nHoursPerDay;

			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			
			allActivitiesList.append(buildings_timetable_weekly[building][day][hour]);
			for(int ai : std::as_const(buildings_timetable_weekly[building][day][hour]))
				allActivitiesSet.insert(ai);
			
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			//2024-08-16: 0 && ..., because otherwise we get a bug with the span if in the building there are two activities,
			//starting at the same time, with different durations.
			if(false && allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityBuilding(htmlLevel, building, day, hour, true, false,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_CODES
				 );
			} else {
				tmpString+=writeActivitiesBuildings(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_CODES
				 );
			}

			addActivitiesLegend(allActivitiesList, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
			 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
			 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
			 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_LEGEND,
			 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_LEGEND,
			 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_LEGEND);
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_BUILDINGS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleBuildingsTimetableTimeVerticalHtml(int htmlLevel, int maxBuildings, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	QList<int> buildingsList;
	int currentCount=0;
	for(int building=0; building<gt.rules.nInternalBuildings && currentCount<maxBuildings; building++){
		if(!excludedNames.contains(building)){
			currentCount++;
			buildingsList.append(building);
			excludedNames.insert(building);
		}
	}
	if(PRINT_RTL)
		std::reverse(buildingsList.begin(), buildingsList.end());

	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	for(int building : buildingsList){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.internalBuildingsList[building]->name)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			tmpString+="        <tr>\n";

			if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}

				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				if(gt.rules.mode!=MORNINGS_AFTERNOONS)
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				else
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			for(int building : std::as_const(buildingsList)){
				QList<int> allActivitiesList;
				QSet<int> allActivitiesSet;

				allActivitiesList.append(buildings_timetable_weekly[building][day][hour]);
				for(int ai : std::as_const(buildings_timetable_weekly[building][day][hour]))
					allActivitiesSet.insert(ai);

				bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
				//2024-08-16: 0 && ..., because otherwise we get a bug with the span if in the building there are two activities,
				//starting at the same time, with different durations.
				if(false && allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
					tmpString+=writeActivityBuilding(htmlLevel, building, day, hour, false, true,
					 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_CODES
					 );
				} else {
					tmpString+=writeActivitiesBuildings(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_CODES
					 );
				}
			}
			if((!PRINT_RTL && repeatNames) || PRINT_RTL){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
			}
			tmpString+="        </tr>\n";
		}

		if(USE_DUMMY_TH_TR_TD){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(day!=gt.rules.nDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, currentCount+2+(repeatNames?2:0));
			}
			else{
				if(day%2==1 && day/2!=gt.rules.nRealDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, currentCount+2+(repeatNames?2:0));
			}
		}
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleBuildingsTimetableTimeHorizontalHtml(int htmlLevel, int maxBuildings, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	QList<int> buildingsList;
	int currentCount=0;
	for(int building=0; building<gt.rules.nInternalBuildings && currentCount<maxBuildings; building++){
		if(!excludedNames.contains(building)){
			currentCount++;
			buildingsList.append(building);
			excludedNames.insert(building);
		}
	}
	
	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(day, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(day, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;
				
				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nRealHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nRealHoursPerDay-1-rhour;

				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getRealHourString(hour, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int building : std::as_const(buildingsList)){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.internalBuildingsList[building]->name)+"</th>\n";
		}

		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTd(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;

				QList<int> allActivitiesList;
				QSet<int> allActivitiesSet;

				allActivitiesList.append(buildings_timetable_weekly[building][day][hour]);
				for(int ai : std::as_const(buildings_timetable_weekly[building][day][hour]))
					allActivitiesSet.insert(ai);

				bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
				//2024-08-16: 0 && ..., because otherwise we get a bug with the span if in the building there are two activities,
				//starting at the same time, with different durations.
				if(false && allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
					tmpString+=writeActivityBuilding(htmlLevel, building, day, hour, true, false,
					 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_CODES
					 );
				} else {
					tmpString+=writeActivitiesBuildings(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_CODES
					 );
				}
			}

			if(USE_DUMMY_TH_TR_TD){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=dummyTd(10);
				}
				else{
					if(rday%2==1)
						tmpString+=dummyTd(10);
				}
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.internalBuildingsList[building]->name)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(USE_DUMMY_TH_TR_TD){
		int additionalSpan = (gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek-1 : gt.rules.nRealDaysPerWeek-1);
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		else
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	else{
		int additionalSpan = 0;
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		else
			tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleBuildingsTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, int maxBuildings, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));
	
	QList<int> buildingsList;
	int currentCount=0;
	for(int building=0; building<gt.rules.nInternalBuildings && currentCount<maxBuildings; building++){
		if(!excludedNames.contains(building)){
			currentCount++;
			buildingsList.append(building);
			excludedNames.insert(building);
		}
	}
	if(PRINT_RTL)
		std::reverse(buildingsList.begin(), buildingsList.end());
	
	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	for(int building : std::as_const(buildingsList)){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.internalBuildingsList[building]->name)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	
	for(int realHour=0; realHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); realHour++){
		int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
		int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);
		
		tmpString+="        <tr>\n";

		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}
			else{
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}

			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			else
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}

		for(int building : std::as_const(buildingsList)){
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;

			allActivitiesList.append(buildings_timetable_weekly[building][day][hour]);
			for(int ai : std::as_const(buildings_timetable_weekly[building][day][hour]))
				allActivitiesSet.insert(ai);
			
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			//2024-08-16: 0 && ..., because otherwise we get a bug with the span if in the building there are two activities,
			//starting at the same time, with different durations.
			if(false && allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityBuilding(htmlLevel, building, day, hour, false, true,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_CODES
				 );
			} else {
				tmpString+=writeActivitiesBuildings(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_CODES
				 );
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			else{
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleBuildingsTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, int maxBuildings, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));

	QList<int> buildingsList;
	int currentCount=0;
	for(int building=0; building<gt.rules.nInternalBuildings && currentCount<maxBuildings; building++){
		if(!excludedNames.contains(building)){
			currentCount++;
			buildingsList.append(building);
			excludedNames.insert(building);
		}
	}

	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(realDay, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	else
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(realDay, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
		int realHour;
		if(!PRINT_RTL)
			realHour=rrealHour;
		else
			realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+=getHourString(realHour, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		else
			tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int building : std::as_const(buildingsList)){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.internalBuildingsList[building]->name)+"</th>\n";
		}
		for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
			int realHour;
			if(!PRINT_RTL)
				realHour=rrealHour;
			else
				realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;

			int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
			int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);

			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;

			allActivitiesList.append(buildings_timetable_weekly[building][day][hour]);
			for(int ai : std::as_const(buildings_timetable_weekly[building][day][hour]))
				allActivitiesSet.insert(ai);
			
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			//2024-08-16: 0 && ..., because otherwise we get a bug with the span if in the building there are two activities,
			//starting at the same time, with different durations.
			if(false && allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityBuilding(htmlLevel, building, day, hour, true, false,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_CODES
				 );
			} else {
				tmpString+=writeActivitiesBuildings(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_BUILDINGS_TIME_HV_PRINT_ROOMS_CODES
				 );
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.internalBuildingsList[building]->name)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleRoomsTimetableDaysHorizontalHtml(int htmlLevel, int room, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(room>=0);
	assert(room<gt.rules.nInternalRooms);
	QString tmpString;
	QString room_name = gt.rules.internalRoomsList[room]->name;

	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table_"+hashRoomIDsTimetable.value(room_name)+"\" border=\"1\"";
	else
		tmpString+="    <table align=\"center\" id=\"table_"+hashRoomIDsTimetable.value(room_name)+"\" border=\"1\"";
	if(room%2==0)  tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	QStringList tl;
	if(!gt.rules.institutionName.isEmpty())
		tl.append("<span class=\"institution\">"+protect2(gt.rules.institutionName)+"</span>");
	QString nm=getRoomString(gt.rules.internalRoomsList[room], SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_CODES);
	if(!nm.isEmpty())
		tl.append("<span class=\"name\">"+nm+"</span>");
	QString c1=gt.rules.internalRoomsList[room]->comments;
	if(SETTINGS_TIMETABLES_PRINT_ROOMS_COMMENTS && !c1.isEmpty()){
		QString c2=protect2(c1);
		c2.replace("\n", "<br />\n");
		tl.append("<span class=\"comment\">"+c2+"</span>");
	}
	
	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;

	tmpString+="      <caption>"+tl.join("<br />")+"</caption>\n";
	tmpString+="      <thead>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
		int td;
		if(!PRINT_RTL)
			td=rtd;
		else
			td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";

		tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int th=0; th<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); th++){
		int hour;
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			hour=th;
		else
			hour=th%gt.rules.nHoursPerDay;
		
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			tmpString+="        <tr>\n";
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
			int td;
			if(!PRINT_RTL)
				td=rtd;
			else
				td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
			
			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			
			if(gt.rules.internalRoomsList[room]->isVirtual==false){
				allActivitiesList.append(rooms_timetable_weekly[room][day][hour]);
				allActivitiesSet.insert(rooms_timetable_weekly[room][day][hour]);
			}
			else{
				allActivitiesList.append(virtual_rooms_timetable_weekly[room][day][hour]);
				for(int ai : std::as_const(virtual_rooms_timetable_weekly[room][day][hour]))
					allActivitiesSet.insert(ai);
				if(allActivitiesList.isEmpty()){
					allActivitiesList.append(UNALLOCATED_ACTIVITY);
					allActivitiesSet.insert(UNALLOCATED_ACTIVITY);
				}
			}

			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityRoom(htmlLevel, room, day, hour, false, true,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_CODES
				 );
			} else {
				tmpString+=writeActivitiesRooms(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_CODES
				 );
			}

			addActivitiesLegend(allActivitiesList, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
			 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
			 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
			 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_LEGEND,
			 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_LEGEND,
			 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LEGEND);
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleRoomsTimetableDaysVerticalHtml(int htmlLevel, int room, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(room>=0);
	assert(room<gt.rules.nInternalRooms);
	QString tmpString;
	QString room_name = gt.rules.internalRoomsList[room]->name;

	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table_"+hashRoomIDsTimetable.value(room_name)+"\" border=\"1\"";
	else
		tmpString+="    <table align=\"center\" id=\"table_"+hashRoomIDsTimetable.value(room_name)+"\" border=\"1\"";
	if(room%2==0)  tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	QStringList tl;
	if(!gt.rules.institutionName.isEmpty())
		tl.append("<span class=\"institution\">"+protect2(gt.rules.institutionName)+"</span>");
	QString nm=getRoomString(gt.rules.internalRoomsList[room], SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_CODES);
	if(!nm.isEmpty())
		tl.append("<span class=\"name\">"+nm+"</span>");
	QString c1=gt.rules.internalRoomsList[room]->comments;
	if(SETTINGS_TIMETABLES_PRINT_ROOMS_COMMENTS && !c1.isEmpty()){
		QString c2=protect2(c1);
		c2.replace("\n", "<br />\n");
		tl.append("<span class=\"comment\">"+c2+"</span>");
	}
	
	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;

	tmpString+="      <caption>"+tl.join("<br />")+"</caption>\n";
	tmpString+="      <thead>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
		int th;
		if(!PRINT_RTL)
			th=rth;
		else
			th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
			else
				tmpString+="          <th>";

		tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int td=0; td<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); td++){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
			int th;
			if(!PRINT_RTL)
				th=rth;
			else
				th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;

			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			int hour;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				hour=th;
			else
				hour=th%gt.rules.nHoursPerDay;
			
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			
			if(gt.rules.internalRoomsList[room]->isVirtual==false){
				allActivitiesList.append(rooms_timetable_weekly[room][day][hour]);
				allActivitiesSet.insert(rooms_timetable_weekly[room][day][hour]);
			}
			else{
				allActivitiesList.append(virtual_rooms_timetable_weekly[room][day][hour]);
				for(int ai : std::as_const(virtual_rooms_timetable_weekly[room][day][hour]))
					allActivitiesSet.insert(ai);
				if(allActivitiesList.isEmpty()){
					allActivitiesList.append(UNALLOCATED_ACTIVITY);
					allActivitiesSet.insert(UNALLOCATED_ACTIVITY);
				}
			}
			
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityRoom(htmlLevel, room, day, hour, true, false,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_CODES
				 );
			} else {
				tmpString+=writeActivitiesRooms(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_CODES
				 );
			}

			addActivitiesLegend(allActivitiesList, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
			 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
			 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
			 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_LEGEND,
			 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_LEGEND,
			 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LEGEND);
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ROOMS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleRoomsTimetableTimeVerticalHtml(int htmlLevel, int maxRooms, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	QList<int> roomsList;
	int currentCount=0;
	for(int room=0; room<gt.rules.nInternalRooms && currentCount<maxRooms; room++){
		if(!excludedNames.contains(room)){
			currentCount++;
			roomsList.append(room);
			excludedNames.insert(room);
		}
	}
	if(PRINT_RTL)
		std::reverse(roomsList.begin(), roomsList.end());

	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	for(int room : std::as_const(roomsList)){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=getRoomString(gt.rules.internalRoomsList[room], SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			tmpString+="        <tr>\n";

			if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}

				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				if(gt.rules.mode!=MORNINGS_AFTERNOONS)
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				else
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			for(int room : std::as_const(roomsList)){
				QList<int> allActivitiesList;
				QSet<int> allActivitiesSet;
				
				if(gt.rules.internalRoomsList[room]->isVirtual==false){
					allActivitiesList.append(rooms_timetable_weekly[room][day][hour]);
					allActivitiesSet.insert(rooms_timetable_weekly[room][day][hour]);
				}
				else{
					allActivitiesList.append(virtual_rooms_timetable_weekly[room][day][hour]);
					for(int ai : std::as_const(virtual_rooms_timetable_weekly[room][day][hour]))
						allActivitiesSet.insert(ai);
					if(allActivitiesList.isEmpty()){
						allActivitiesList.append(UNALLOCATED_ACTIVITY);
						allActivitiesSet.insert(UNALLOCATED_ACTIVITY);
					}
				}

				bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
				if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
					tmpString+=writeActivityRoom(htmlLevel, room, day, hour, false, true,
					 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES
					 );
				} else {
					tmpString+=writeActivitiesRooms(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES
					 );
				}
			}
			if((!PRINT_RTL && repeatNames) || PRINT_RTL){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
			}
			tmpString+="        </tr>\n";
		}

		if(USE_DUMMY_TH_TR_TD){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(day!=gt.rules.nDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, currentCount+2+(repeatNames?2:0));
			}
			else{
				if(day%2==1 && day/2!=gt.rules.nRealDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, currentCount+2+(repeatNames?2:0));
			}
		}
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleRoomsTimetableTimeHorizontalHtml(int htmlLevel, int maxRooms, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	QList<int> roomsList;
	int currentCount=0;
	for(int room=0; room<gt.rules.nInternalRooms && currentCount<maxRooms; room++){
		if(!excludedNames.contains(room)){
			currentCount++;
			roomsList.append(room);
			excludedNames.insert(room);
		}
	}
	
	
	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(day, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(day, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;
				
				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nRealHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nRealHoursPerDay-1-rhour;

				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getRealHourString(hour, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int room : std::as_const(roomsList)){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getRoomString(gt.rules.internalRoomsList[room], SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES)+"</th>\n";
		}

		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTd(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;

				QList<int> allActivitiesList;
				QSet<int> allActivitiesSet;
				
				if(gt.rules.internalRoomsList[room]->isVirtual==false){
					allActivitiesList.append(rooms_timetable_weekly[room][day][hour]);
					allActivitiesSet.insert(rooms_timetable_weekly[room][day][hour]);
				}
				else{
					allActivitiesList.append(virtual_rooms_timetable_weekly[room][day][hour]);
					for(int ai : std::as_const(virtual_rooms_timetable_weekly[room][day][hour]))
						allActivitiesSet.insert(ai);
					if(allActivitiesList.isEmpty()){
						allActivitiesList.append(UNALLOCATED_ACTIVITY);
						allActivitiesSet.insert(UNALLOCATED_ACTIVITY);
					}
				}

				bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
				if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
					tmpString+=writeActivityRoom(htmlLevel, room, day, hour, true, false,
					 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES
					 );
				} else {
					tmpString+=writeActivitiesRooms(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_CODES,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_CODES,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_CODES,
					 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES
					);
				}
			}

			if(USE_DUMMY_TH_TR_TD){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=dummyTd(10);
				}
				else{
					if(rday%2==1)
						tmpString+=dummyTd(10);
				}
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getRoomString(gt.rules.internalRoomsList[room], SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(USE_DUMMY_TH_TR_TD){
		int additionalSpan = (gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek-1 : gt.rules.nRealDaysPerWeek-1);
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		else
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	else{
		int additionalSpan = 0;
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		else
			tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleRoomsTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, int maxRooms, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));
	
	QList<int> roomsList;
	int currentCount=0;
	for(int room=0; room<gt.rules.nInternalRooms && currentCount<maxRooms; room++){
		if(!excludedNames.contains(room)){
			currentCount++;
			roomsList.append(room);
			excludedNames.insert(room);
		}
	}
	if(PRINT_RTL)
		std::reverse(roomsList.begin(), roomsList.end());
	
	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	for(int room : std::as_const(roomsList)){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=getRoomString(gt.rules.internalRoomsList[room], SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	
	for(int realHour=0; realHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); realHour++){
		int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
		int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);
		
		tmpString+="        <tr>\n";

		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}
			else{
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}

			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			else
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		
		for(int room : std::as_const(roomsList)){
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			
			if(gt.rules.internalRoomsList[room]->isVirtual==false){
				allActivitiesList.append(rooms_timetable_weekly[room][day][hour]);
				allActivitiesSet.insert(rooms_timetable_weekly[room][day][hour]);
			}
			else{
				allActivitiesList.append(virtual_rooms_timetable_weekly[room][day][hour]);
				for(int ai : std::as_const(virtual_rooms_timetable_weekly[room][day][hour]))
					allActivitiesSet.insert(ai);
				if(allActivitiesList.isEmpty()){
					allActivitiesList.append(UNALLOCATED_ACTIVITY);
					allActivitiesSet.insert(UNALLOCATED_ACTIVITY);
				}
			}
			
			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityRoom(htmlLevel, room, day, hour, false, true,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES
				 );
			} else {
				tmpString+=writeActivitiesRooms(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES
				);
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			else{
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleRoomsTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, int maxRooms, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));

	QList<int> roomsList;
	int currentCount=0;
	for(int room=0; room<gt.rules.nInternalRooms && currentCount<maxRooms; room++){
		if(!excludedNames.contains(room)){
			currentCount++;
			roomsList.append(room);
			excludedNames.insert(room);
		}
	}
	
	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(realDay, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	else
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(realDay, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
		int realHour;
		if(!PRINT_RTL)
			realHour=rrealHour;
		else
			realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+=getHourString(realHour, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		else
			tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int room : std::as_const(roomsList)){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getRoomString(gt.rules.internalRoomsList[room], SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES)+"</th>\n";
		}
		for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
			int realHour;
			if(!PRINT_RTL)
				realHour=rrealHour;
			else
				realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;

			int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
			int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);

			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			
			if(gt.rules.internalRoomsList[room]->isVirtual==false){
				allActivitiesList.append(rooms_timetable_weekly[room][day][hour]);
				allActivitiesSet.insert(rooms_timetable_weekly[room][day][hour]);
			}
			else{
				allActivitiesList.append(virtual_rooms_timetable_weekly[room][day][hour]);
				for(int ai : std::as_const(virtual_rooms_timetable_weekly[room][day][hour]))
					allActivitiesSet.insert(ai);
				if(allActivitiesList.isEmpty()){
					allActivitiesList.append(UNALLOCATED_ACTIVITY);
					allActivitiesSet.insert(UNALLOCATED_ACTIVITY);
				}
			}

			bool haveActivitiesWithSameStartingTime=addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			if(allActivitiesList.size()==1 && !haveActivitiesWithSameStartingTime){  // because I am using colspan or rowspan!!!
				tmpString+=writeActivityRoom(htmlLevel, room, day, hour, true, false,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES
				 );
			} else {
				tmpString+=writeActivitiesRooms(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES
				);
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getRoomString(gt.rules.internalRoomsList[room], SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ROOMS_TIME_HV_PRINT_ROOMS_CODES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleSubjectsTimetableDaysHorizontalHtml(int htmlLevel, int subject, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(subject>=0);
	assert(subject<gt.rules.nInternalSubjects);
	QString tmpString;
	///////by Liviu Lalescu
	activitiesForCurrentSubject.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	for(int d=0; d<gt.rules.nDaysPerWeek; d++)
		for(int h=0; h<gt.rules.nHoursPerDay; h++)
			activitiesForCurrentSubject[d][h].clear();
	for(int ai : std::as_const(gt.rules.activitiesForSubjectList[subject]))
		if(best_solution.times[ai]!=UNALLOCATED_TIME){
			int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
			int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
			Activity* act=&gt.rules.internalActivitiesList[ai];
			for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
				activitiesForCurrentSubject[d][h+dd].append(ai);
		}
	///////end Liviu Lalescu

	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table_"+hashSubjectIDsTimetable.value(gt.rules.internalSubjectsList[subject]->name);
	else
		tmpString+="    <table align=\"center\" id=\"table_"+hashSubjectIDsTimetable.value(gt.rules.internalSubjectsList[subject]->name);
	tmpString+="\" border=\"1\"";
	if(subject%2==0)  tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	QStringList tl;
	if(!gt.rules.institutionName.isEmpty())
		tl.append("<span class=\"institution\">"+protect2(gt.rules.institutionName)+"</span>");
	QString nm=getSubjectString(gt.rules.internalSubjectsList[subject], SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_CODES);
	if(!nm.isEmpty())
		tl.append("<span class=\"name\">"+nm+"</span>");
	QString c1=gt.rules.internalSubjectsList[subject]->comments;
	if(SETTINGS_TIMETABLES_PRINT_SUBJECTS_COMMENTS && !c1.isEmpty()){
		QString c2=protect2(c1);
		c2.replace("\n", "<br />\n");
		tl.append("<span class=\"comment\">"+c2+"</span>");
	}
	
	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;

	tmpString+="      <caption>"+tl.join("<br />")+"</caption>\n";
	tmpString+="      <thead>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
		int td;
		if(!PRINT_RTL)
			td=rtd;
		else
			td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";

		tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int th=0; th<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); th++){
		int hour;
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			hour=th;
		else
			hour=th%gt.rules.nHoursPerDay;
		
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
			int td;
			if(!PRINT_RTL)
				td=rtd;
			else
				td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
			
			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			
			allActivitiesList=activitiesForCurrentSubject[day][hour];
			for(int ai : std::as_const(activitiesForCurrentSubject[day][hour]))
				allActivitiesSet.insert(ai);
			
			addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			tmpString+=writeActivitiesSubjects(htmlLevel, allActivitiesList, printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_CODES,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_CODES,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_CODES,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_CODES
			);

			addActivitiesLegend(allActivitiesList, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_LEGEND,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_LEGEND,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_LEGEND);
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleSubjectsTimetableDaysVerticalHtml(int htmlLevel, int subject, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(subject>=0);
	assert(subject<gt.rules.nInternalSubjects);
	QString tmpString;
	///////by Liviu Lalescu
	activitiesForCurrentSubject.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	for(int d=0; d<gt.rules.nDaysPerWeek; d++)
		for(int h=0; h<gt.rules.nHoursPerDay; h++)
			activitiesForCurrentSubject[d][h].clear();
	for(int ai : std::as_const(gt.rules.activitiesForSubjectList[subject]))
		if(best_solution.times[ai]!=UNALLOCATED_TIME){
			int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
			int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
			Activity* act=&gt.rules.internalActivitiesList[ai];
			for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
				activitiesForCurrentSubject[d][h+dd].append(ai);
		}
	///////end Liviu Lalescu

	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table_"+hashSubjectIDsTimetable.value(gt.rules.internalSubjectsList[subject]->name);
	else
		tmpString+="    <table align=\"center\" id=\"table_"+hashSubjectIDsTimetable.value(gt.rules.internalSubjectsList[subject]->name);
	tmpString+="\" border=\"1\"";
	if(subject%2==0) tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	QStringList tl;
	if(!gt.rules.institutionName.isEmpty())
		tl.append("<span class=\"institution\">"+protect2(gt.rules.institutionName)+"</span>");
	QString nm=getSubjectString(gt.rules.internalSubjectsList[subject], SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_CODES);
	if(!nm.isEmpty())
		tl.append("<span class=\"name\">"+nm+"</span>");
	QString c1=gt.rules.internalSubjectsList[subject]->comments;
	if(SETTINGS_TIMETABLES_PRINT_SUBJECTS_COMMENTS && !c1.isEmpty()){
		QString c2=protect2(c1);
		c2.replace("\n", "<br />\n");
		tl.append("<span class=\"comment\">"+c2+"</span>");
	}
	
	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;

	tmpString+="      <caption>"+tl.join("<br />")+"</caption>\n";
	tmpString+="      <thead>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
		int th;
		if(!PRINT_RTL)
			th=rth;
		else
			th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";

		tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int td=0; td<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); td++){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
			int th;
			if(!PRINT_RTL)
				th=rth;
			else
				th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;

			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			int hour;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				hour=th;
			else
				hour=th%gt.rules.nHoursPerDay;
			
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			
			allActivitiesList=activitiesForCurrentSubject[day][hour];
			for(int ai : std::as_const(activitiesForCurrentSubject[day][hour]))
				allActivitiesSet.insert(ai);

			addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			tmpString+=writeActivitiesSubjects(htmlLevel, allActivitiesList,
			 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_CODES,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_CODES,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_CODES,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_CODES
			 );

			addActivitiesLegend(allActivitiesList, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_LEGEND,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_LEGEND,
			 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_LEGEND);
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_SUBJECTS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleSubjectsTimetableTimeVerticalHtml(int htmlLevel, int maxSubjects, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	QList<int> subjectsList;
	int currentCount=0;
	for(int subject=0; subject<gt.rules.nInternalSubjects && currentCount<maxSubjects; subject++){
		if(!excludedNames.contains(subject)){
			currentCount++;
			subjectsList.append(subject);
			excludedNames.insert(subject);
		}
	}
	if(PRINT_RTL)
		std::reverse(subjectsList.begin(), subjectsList.end());

	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table\" border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" id=\"table\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	for(int subject : std::as_const(subjectsList)){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=getSubjectString(gt.rules.internalSubjectsList[subject], SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			tmpString+="        <tr>\n";

			if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}

				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				if(gt.rules.mode!=MORNINGS_AFTERNOONS)
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				else
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			for(int subject : std::as_const(subjectsList)){
				QList<int> allActivitiesList;
				QSet<int> allActivitiesSet;
				//allActivities.clear();
				
				for(int ai : std::as_const(gt.rules.activitiesForSubjectList[subject]))
					if(activitiesAtTimeSet[day][hour].contains(ai)){
						assert(!allActivitiesSet.contains(ai));
						allActivitiesList.append(ai);
						allActivitiesSet.insert(ai);
					}
				
				/* //Now get the activities ids. I don't run through the internalActivitiesList, even if that is faster. I run through subgroupsList, because by that the activities are sorted by that in the html-table.
				for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
					if(students_timetable_weekly[subgroup][day][hour]!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[students_timetable_weekly[subgroup][day][hour]];
						if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
							if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour]))){
								allActivities+students_timetable_weekly[subgroup][day][hour];
							}
					}
				}
				//Now run through the teachers timetable, because activities without a students set are still missing.
				for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
					if(teachers_timetable_weekly[teacher][day][hour]!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[teachers_timetable_weekly[teacher][day][hour]];
						if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
							if(!(allActivities.contains(teachers_timetable_weekly[teacher][day][hour]))){
								assert(act->studentsNames.isEmpty());
								allActivities+teachers_timetable_weekly[teacher][day][hour];
							}
					}
				}*/
				addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
				tmpString+=writeActivitiesSubjects(htmlLevel, allActivitiesList,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_CODES
				 );
			}
			if((!PRINT_RTL && repeatNames) || PRINT_RTL){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
			}
			tmpString+="        </tr>\n";
		}

		if(USE_DUMMY_TH_TR_TD){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(day!=gt.rules.nDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, currentCount+2+(repeatNames?2:0));
			}
			else{
				if(day%2==1 && day/2!=gt.rules.nRealDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, currentCount+2+(repeatNames?2:0));
			}
		}
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleSubjectsTimetableTimeHorizontalHtml(int htmlLevel, int maxSubjects, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	QList<int> subjectsList;
	int currentCount=0;
	for(int subject=0; subject<gt.rules.nInternalSubjects && currentCount<maxSubjects; subject++){
		if(!excludedNames.contains(subject)){
			currentCount++;
			subjectsList.append(subject);
			excludedNames.insert(subject);
		}
	}

	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table\" border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" id=\"table\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(day, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(day, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;
				
				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nRealHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nRealHoursPerDay-1-rhour;

				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getRealHourString(hour, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int subject : std::as_const(subjectsList)){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">"+getSubjectString(gt.rules.internalSubjectsList[subject], SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES)+"</th>\n";
			else
				tmpString+="          <th>"+getSubjectString(gt.rules.internalSubjectsList[subject], SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES)+"</th>\n";
		}

		///////by Liviu Lalescu
		activitiesForCurrentSubject.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				activitiesForCurrentSubject[d][h].clear();
		for(int ai : std::as_const(gt.rules.activitiesForSubjectList[subject]))
			if(best_solution.times[ai]!=UNALLOCATED_TIME){
				int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
				int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
				Activity* act=&gt.rules.internalActivitiesList[ai];
				for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
					activitiesForCurrentSubject[d][h+dd].append(ai);
			}
		///////end Liviu Lalescu

		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTd(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;

				QList<int> allActivitiesList;
				QSet<int> allActivitiesSet;
				
				allActivitiesList=activitiesForCurrentSubject[day][hour];
				for(int ai : std::as_const(activitiesForCurrentSubject[day][hour]))
					allActivitiesSet.insert(ai);

				/*allActivities.clear();
				//Now get the activities ids. I don't run through the internalActivitiesList, even that is faster. I run through subgroupsList, because by that the activities are sorted by that in the html-table.
				for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
					if(students_timetable_weekly[subgroup][day][hour]!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[students_timetable_weekly[subgroup][day][hour]];
						if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
							if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour]))){
								allActivities+students_timetable_weekly[subgroup][day][hour];
							}
					}
				}
				//Now run through the teachers timetable, because activities without a students set are still missing.
				for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
					if(teachers_timetable_weekly[teacher][day][hour]!=UNALLOCATED_ACTIVITY){
						Activity* act=&gt.rules.internalActivitiesList[teachers_timetable_weekly[teacher][day][hour]];
						if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
							if(!(allActivities.contains(teachers_timetable_weekly[teacher][day][hour]))){
								assert(act->studentsNames.isEmpty());
								allActivities+teachers_timetable_weekly[teacher][day][hour];
							}
					}
				}*/
				addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
				tmpString+=writeActivitiesSubjects(htmlLevel, allActivitiesList,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_CODES
				 );
			}

			if(USE_DUMMY_TH_TR_TD){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=dummyTd(10);
				}
				else{
					if(rday%2==1)
						tmpString+=dummyTd(10);
				}
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">"+getSubjectString(gt.rules.internalSubjectsList[subject], SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES)+"</th>\n";
			else
				tmpString+="          <th>"+getSubjectString(gt.rules.internalSubjectsList[subject], SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(USE_DUMMY_TH_TR_TD){
		int additionalSpan = (gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek-1 : gt.rules.nRealDaysPerWeek-1);
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		else
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	else{
		int additionalSpan = 0;
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		else
			tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleSubjectsTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, int maxSubjects, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));
	
	QList<int> subjectsList;
	int currentCount=0;
	for(int subject=0; subject<gt.rules.nInternalSubjects && currentCount<maxSubjects; subject++){
		if(!excludedNames.contains(subject)){
			currentCount++;
			subjectsList.append(subject);
			excludedNames.insert(subject);
		}
	}
	if(PRINT_RTL)
		std::reverse(subjectsList.begin(), subjectsList.end());
	
	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	for(int subject : std::as_const(subjectsList)){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=getSubjectString(gt.rules.internalSubjectsList[subject], SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int realHour=0; realHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); realHour++){
		int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
		int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);
		
		tmpString+="        <tr>\n";

		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}
			else{
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}

			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			else
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}

		for(int subject : std::as_const(subjectsList)){
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			
			for(int ai : std::as_const(gt.rules.activitiesForSubjectList[subject]))
				if(activitiesAtTimeSet[day][hour].contains(ai)){
					assert(!allActivitiesSet.contains(ai));
					allActivitiesList.append(ai);
					allActivitiesSet.insert(ai);
				}
			
			/*//Now get the activities ids. I don't run through the internalActivitiesList, even that is faster. I run through subgroupsList, because by that the activities are sorted by that in the html-table.
			for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
				if(students_timetable_weekly[subgroup][day][hour]!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[students_timetable_weekly[subgroup][day][hour]];
					if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
						if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour]))){
							allActivities+students_timetable_weekly[subgroup][day][hour];
						}
				}
			}
			//Now run through the teachers timetable, because activities without a students set are still missing.
			for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
				if(teachers_timetable_weekly[teacher][day][hour]!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[teachers_timetable_weekly[teacher][day][hour]];
					if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
						if(!(allActivities.contains(teachers_timetable_weekly[teacher][day][hour]))){
							assert(act->studentsNames.isEmpty());
							allActivities+teachers_timetable_weekly[teacher][day][hour];
						}
				}
			}*/
			addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			tmpString+=writeActivitiesSubjects(htmlLevel, allActivitiesList,
			 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
			 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES,
			 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
			 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_CODES,
			 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_CODES,
			 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_CODES
			 );
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			else{
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleSubjectsTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, int maxSubjects, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));

	QList<int> subjectsList;
	int currentCount=0;
	for(int subject=0; subject<gt.rules.nInternalSubjects && currentCount<maxSubjects; subject++){
		if(!excludedNames.contains(subject)){
			currentCount++;
			subjectsList.append(subject);
			excludedNames.insert(subject);
		}
	}
	
	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(realDay, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	else
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(realDay, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
		int realHour;
		if(!PRINT_RTL)
			realHour=rrealHour;
		else
			realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+=getHourString(realHour, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		else
			tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int subject : std::as_const(subjectsList)){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">"+getSubjectString(gt.rules.internalSubjectsList[subject], SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES)+"</th>\n";
			else
				tmpString+="          <th>"+getSubjectString(gt.rules.internalSubjectsList[subject], SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES)+"</th>\n";
		}
		
		///////by Liviu Lalescu
		activitiesForCurrentSubject.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				activitiesForCurrentSubject[d][h].clear();
		for(int ai : std::as_const(gt.rules.activitiesForSubjectList[subject]))
			if(best_solution.times[ai]!=UNALLOCATED_TIME){
				int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
				int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
				Activity* act=&gt.rules.internalActivitiesList[ai];
				for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
					activitiesForCurrentSubject[d][h+dd].append(ai);
			}
		///////end Liviu Lalescu

		for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
			int realHour;
			if(!PRINT_RTL)
				realHour=rrealHour;
			else
				realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;

			int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
			int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);

			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;

			allActivitiesList=activitiesForCurrentSubject[day][hour];
			for(int ai : std::as_const(activitiesForCurrentSubject[day][hour]))
				allActivitiesSet.insert(ai);

			/*allActivities.clear();
			//Now get the activities ids. I don't run through the internalActivitiesList, even that is faster. I run through subgroupsList, because by that the activities are sorted by that in the html-table.
			for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
				if(students_timetable_weekly[subgroup][day][hour]!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[students_timetable_weekly[subgroup][day][hour]];
					if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
						if(!(allActivities.contains(students_timetable_weekly[subgroup][day][hour]))){
							allActivities+students_timetable_weekly[subgroup][day][hour];
						}
				}
			}
			//Now run through the teachers timetable, because activities without a students set are still missing.
			for(int teacher=0; teacher<gt.rules.nInternalTeachers; teacher++){
				if(teachers_timetable_weekly[teacher][day][hour]!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[teachers_timetable_weekly[teacher][day][hour]];
					if(act->subjectName==gt.rules.internalSubjectsList[subject]->name)
						if(!(allActivities.contains(teachers_timetable_weekly[teacher][day][hour]))){
							assert(act->studentsNames.isEmpty());
							allActivities+teachers_timetable_weekly[teacher][day][hour];
						}
				}
			}*/
			addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			tmpString+=writeActivitiesSubjects(htmlLevel, allActivitiesList,
			 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
			 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES,
			 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
			 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_TEACHERS_CODES,
			 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_STUDENTS_CODES,
			 SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_ROOMS_CODES
			 );
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">"+getSubjectString(gt.rules.internalSubjectsList[subject], SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES)+"</th>\n";
			else
				tmpString+="          <th>"+getSubjectString(gt.rules.internalSubjectsList[subject], SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_SUBJECTS_TIME_HV_PRINT_SUBJECTS_CODES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleActivityTagsTimetableDaysHorizontalHtml(int htmlLevel, int activityTag, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(activityTag>=0);
	assert(activityTag<gt.rules.nInternalActivityTags);
	QString tmpString;
	///////by Liviu Lalescu
	activitiesForCurrentActivityTag.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	for(int d=0; d<gt.rules.nDaysPerWeek; d++)
		for(int h=0; h<gt.rules.nHoursPerDay; h++)
			activitiesForCurrentActivityTag[d][h].clear();
	for(int ai : std::as_const(gt.rules.activitiesForActivityTagList[activityTag]))
		if(best_solution.times[ai]!=UNALLOCATED_TIME){
			int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
			int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
			Activity* act=&gt.rules.internalActivitiesList[ai];
			for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
				activitiesForCurrentActivityTag[d][h+dd].append(ai);
		}
	///////end Liviu Lalescu

	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table_"+hashActivityTagIDsTimetable.value(gt.rules.internalActivityTagsList[activityTag]->name);
	else
		tmpString+="    <table align=\"center\" id=\"table_"+hashActivityTagIDsTimetable.value(gt.rules.internalActivityTagsList[activityTag]->name);
	tmpString+="\" border=\"1\"";
	if(activityTag%2==0)  tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	QStringList tl;
	if(!gt.rules.institutionName.isEmpty())
		tl.append("<span class=\"institution\">"+protect2(gt.rules.institutionName)+"</span>");
	QString nm=getActivityTagString(gt.rules.internalActivityTagsList[activityTag], SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES);
	if(!nm.isEmpty())
		tl.append("<span class=\"name\">"+nm+"</span>");
	QString c1=gt.rules.internalActivityTagsList[activityTag]->comments;
	if(SETTINGS_TIMETABLES_PRINT_ACTIVITY_TAGS_COMMENTS && !c1.isEmpty()){
		QString c2=protect2(c1);
		c2.replace("\n", "<br />\n");
		tl.append("<span class=\"comment\">"+c2+"</span>");
	}
	
	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;

	tmpString+="      <caption>"+tl.join("<br />")+"</caption>\n";
	tmpString+="      <thead>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
		int td;
		if(!PRINT_RTL)
			td=rtd;
		else
			td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";

		tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int th=0; th<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); th++){
		int hour;
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			hour=th;
		else
			hour=th%gt.rules.nHoursPerDay;
		
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		for(int rtd=0; rtd<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); rtd++){
			int td;
			if(!PRINT_RTL)
				td=rtd;
			else
				td=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)-1-rtd;
			
			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			
			allActivitiesList=activitiesForCurrentActivityTag[day][hour];
			for(int ai : std::as_const(activitiesForCurrentActivityTag[day][hour]))
				allActivitiesSet.insert(ai);
			
			addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			tmpString+=writeActivitiesActivityTags(htmlLevel, allActivitiesList,
			 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_CODES
			 );

			addActivitiesLegend(allActivitiesList, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_LEGEND,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_LEGEND,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_LEGEND);
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleActivityTagsTimetableDaysVerticalHtml(int htmlLevel, int activityTag, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(activityTag>=0);
	assert(activityTag<gt.rules.nInternalActivityTags);
	QString tmpString;
	///////by Liviu Lalescu
	activitiesForCurrentActivityTag.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	for(int d=0; d<gt.rules.nDaysPerWeek; d++)
		for(int h=0; h<gt.rules.nHoursPerDay; h++)
			activitiesForCurrentActivityTag[d][h].clear();
	for(int ai : std::as_const(gt.rules.activitiesForActivityTagList[activityTag]))
		if(best_solution.times[ai]!=UNALLOCATED_TIME){
			int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
			int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
			Activity* act=&gt.rules.internalActivitiesList[ai];
			for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
				activitiesForCurrentActivityTag[d][h+dd].append(ai);
		}
	///////end Liviu Lalescu

	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table_"+hashActivityTagIDsTimetable.value(gt.rules.internalActivityTagsList[activityTag]->name);
	else
		tmpString+="    <table align=\"center\" id=\"table_"+hashActivityTagIDsTimetable.value(gt.rules.internalActivityTagsList[activityTag]->name);
	tmpString+="\" border=\"1\"";
	if(activityTag%2==0) tmpString+=" class=\"odd_table\"";
	else tmpString+=" class=\"even_table\"";
	tmpString+=">\n";

	QStringList tl;
	if(!gt.rules.institutionName.isEmpty())
		tl.append("<span class=\"institution\">"+protect2(gt.rules.institutionName)+"</span>");
	QString nm=getActivityTagString(gt.rules.internalActivityTagsList[activityTag], SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES);
	if(!nm.isEmpty())
		tl.append("<span class=\"name\">"+nm+"</span>");
	QString c1=gt.rules.internalActivityTagsList[activityTag]->comments;
	if(SETTINGS_TIMETABLES_PRINT_ACTIVITY_TAGS_COMMENTS && !c1.isEmpty()){
		QString c2=protect2(c1);
		c2.replace("\n", "<br />\n");
		tl.append("<span class=\"comment\">"+c2+"</span>");
	}
	
	QSet<Subject*> usedSubjectsSet;
	QSet<ActivityTag*> usedActivityTagsSet;
	QSet<Teacher*> usedTeachersSet;
	QSet<StudentsSet*> usedStudentsSet;
	QSet<Room*> usedRoomsSet;

	tmpString+="      <caption>"+tl.join("<br />")+"</caption>\n";
	tmpString+="      <thead>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <td></td>\n";
	else
		tmpString+="        <tr>\n";

	for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
		int th;
		if(!PRINT_RTL)
			th=rth;
		else
			th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";

		tmpString+=getHourOrRealHourString(th, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if((!PRINT_RTL && repeatNames) || PRINT_RTL)
		tmpString+="          <td></td>\n";

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int td=0; td<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek); td++){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		for(int rth=0; rth<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rth++){
			int th;
			if(!PRINT_RTL)
				th=rth;
			else
				th=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rth;

			int day;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				day=td;
			else
				day=2*td+th/gt.rules.nHoursPerDay;
			
			int hour;
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				hour=th;
			else
				hour=th%gt.rules.nHoursPerDay;
			
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			
			allActivitiesList=activitiesForCurrentActivityTag[day][hour];
			for(int ai : std::as_const(activitiesForCurrentActivityTag[day][hour]))
				allActivitiesSet.insert(ai);

			addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			tmpString+=writeActivitiesActivityTags(htmlLevel, allActivitiesList,
			 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_CODES
			 );

			addActivitiesLegend(allActivitiesList, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_LEGEND,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_LEGEND,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_LEGEND);
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=getDayOrRealDayString(td, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";

	printLegend(tmpString, usedSubjectsSet, usedActivityTagsSet, usedTeachersSet, usedStudentsSet, usedRoomsSet,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_SUBJECTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ACTIVITY_TAGS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_TEACHERS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_STUDENTS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_ROOMS_LEGEND,
	 SETTINGS_TIMETABLES_ACTIVITY_TAGS_DAYS_HV_PRINT_LEGEND_CODES_FIRST);

	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleActivityTagsTimetableTimeVerticalHtml(int htmlLevel, int maxActivityTag, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	QList<int> activityTagsList;
	int currentCount=0;
	for(int activityTag=0; activityTag<gt.rules.nInternalActivityTags && currentCount<maxActivityTag; activityTag++){
		if(gt.rules.internalActivityTagsList[activityTag]->printable){
			if(!excludedNames.contains(activityTag)){
				currentCount++;
				activityTagsList.append(activityTag);
				excludedNames.insert(activityTag);
			}
		}
	}
	if(PRINT_RTL)
		std::reverse(activityTagsList.begin(), activityTagsList.end());

	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table\" border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" id=\"table\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	for(int activityTag : std::as_const(activityTagsList)){
		currentCount++;
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=getActivityTagString(gt.rules.internalActivityTagsList[activityTag], SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
			tmpString+="        <tr>\n";

			if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}

				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				if(gt.rules.mode!=MORNINGS_AFTERNOONS)
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				else
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			for(int activityTag : std::as_const(activityTagsList)){
				QList<int> allActivitiesList;
				QSet<int> allActivitiesSet;
				//allActivities.clear();
				
				for(int ai : std::as_const(gt.rules.activitiesForActivityTagList[activityTag]))
					if(activitiesAtTimeSet[day][hour].contains(ai)){
						assert(!allActivitiesSet.contains(ai));
						allActivitiesList.append(ai);
						allActivitiesSet.insert(ai);
					}
				
				addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
				tmpString+=writeActivitiesActivityTags(htmlLevel, allActivitiesList,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_CODES
				 );
			}
			if((!PRINT_RTL && repeatNames) || PRINT_RTL){
				if(htmlLevel>=2)
					tmpString+="          <th class=\"yAxis\">";
				else
					tmpString+="          <th>";
				
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=getHourString(hour, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
				else{
					tmpString+=getRealHourString((day%2)*gt.rules.nHoursPerDay+hour, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";

					if(day%2==0 && hour==0)
						tmpString+="          <th rowspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(day/2, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
					else tmpString+="          <!-- span -->\n";
				}
			}
			tmpString+="        </tr>\n";
		}

		if(USE_DUMMY_TH_TR_TD){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(day!=gt.rules.nDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, currentCount+2+(repeatNames?2:0));
			}
			else{
				if(day%2==1 && day/2!=gt.rules.nRealDaysPerWeek-1)
					tmpString+=dummyTr(8, 10, currentCount+2+(repeatNames?2:0));
			}
		}
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleActivityTagsTimetableTimeHorizontalHtml(int htmlLevel, int maxActivityTag, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	QList<int> activityTagsList;
	int currentCount=0;
	for(int activityTag=0; activityTag<gt.rules.nInternalActivityTags && currentCount<maxActivityTag; activityTag++){
		if(gt.rules.internalActivityTagsList[activityTag]->printable){
			if(!excludedNames.contains(activityTag)){
				currentCount++;
				activityTagsList.append(activityTag);
				excludedNames.insert(activityTag);
			}
		}
	}
	
	QString tmpString;
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table\" border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" id=\"table\" border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(day, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;
			tmpString+="          <th colspan=\"" +QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(day, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
			
			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;
				
				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}
	else{
		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTh(10);

		for(int rday=0; rday<gt.rules.nRealDaysPerWeek; rday++){
			/*int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nRealDaysPerWeek-1-rday;*/

			for(int rhour=0; rhour<gt.rules.nRealHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nRealHoursPerDay-1-rhour;

				if(htmlLevel>=2)
					tmpString+="          <th class=\"xAxis\">";
				else
					tmpString+="          <th>";

				tmpString+=getRealHourString(hour, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			}

			if(USE_DUMMY_TH_TR_TD)
				tmpString+=dummyTh(10);
		}
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int activityTag : std::as_const(activityTagsList)){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">"+getActivityTagString(gt.rules.internalActivityTagsList[activityTag], SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES)+"</th>\n";
			else
				tmpString+="          <th>"+getActivityTagString(gt.rules.internalActivityTagsList[activityTag], SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES)+"</th>\n";
		}

		///////by Liviu Lalescu
		activitiesForCurrentActivityTag.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				activitiesForCurrentActivityTag[d][h].clear();
		for(int ai : std::as_const(gt.rules.activitiesForActivityTagList[activityTag]))
			if(best_solution.times[ai]!=UNALLOCATED_TIME){
				int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
				int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
				Activity* act=&gt.rules.internalActivitiesList[ai];
				for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
					activitiesForCurrentActivityTag[d][h+dd].append(ai);
			}
		///////end Liviu Lalescu

		if(USE_DUMMY_TH_TR_TD)
			tmpString+=dummyTd(10);

		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;

			for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
				int hour;
				if(!PRINT_RTL)
					hour=rhour;
				else
					hour=gt.rules.nHoursPerDay-1-rhour;

				QList<int> allActivitiesList;
				QSet<int> allActivitiesSet;
				
				allActivitiesList=activitiesForCurrentActivityTag[day][hour];
				for(int ai : std::as_const(activitiesForCurrentActivityTag[day][hour]))
					allActivitiesSet.insert(ai);

				addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
				tmpString+=writeActivitiesActivityTags(htmlLevel, allActivitiesList,
				 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
				 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_CODES,
				 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_CODES
				 );
			}

			if(USE_DUMMY_TH_TR_TD){
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					tmpString+=dummyTd(10);
				}
				else{
					if(rday%2==1)
						tmpString+=dummyTd(10);
				}
			}
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">"+getActivityTagString(gt.rules.internalActivityTagsList[activityTag], SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES)+"</th>\n";
			else
				tmpString+="          <th>"+getActivityTagString(gt.rules.internalActivityTagsList[activityTag], SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(USE_DUMMY_TH_TR_TD){
		int additionalSpan = (gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek-1 : gt.rules.nRealDaysPerWeek-1);
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		else
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td><td></td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	else{
		int additionalSpan = 0;
		if(!PRINT_RTL || (PRINT_RTL && repeatNames))
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		else
			tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek+additionalSpan)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
	}
	//workaround end.
	tmpString+="      </tbody>\n    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleActivityTagsTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, int maxActivityTag, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));
	
	QList<int> activityTagsList;
	int currentCount=0;
	for(int activityTag=0; activityTag<gt.rules.nInternalActivityTags && currentCount<maxActivityTag; activityTag++){
		if(gt.rules.internalActivityTagsList[activityTag]->printable){
			if(!excludedNames.contains(activityTag)){
				currentCount++;
				activityTagsList.append(activityTag);
				excludedNames.insert(activityTag);
			}
		}
	}
	if(PRINT_RTL)
		std::reverse(activityTagsList.begin(), activityTagsList.end());
	
	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td colspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	for(int activityTag : std::as_const(activityTagsList)){
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=getActivityTagString(gt.rules.internalActivityTagsList[activityTag], SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td colspan=\"2\"></td>\n";
	}
	
	tmpString+="        </tr>\n      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int realHour=0; realHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); realHour++){
		int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
		int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);
		
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}
			else{
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else
					tmpString+="          <!-- span -->\n";
			}

			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
			else
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		}
		
		for(int activityTag : std::as_const(activityTagsList)){
			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			//allActivities.clear();
			
			for(int ai : std::as_const(gt.rules.activitiesForActivityTagList[activityTag]))
				if(activitiesAtTimeSet[day][hour].contains(ai)){
					assert(!allActivitiesSet.contains(ai));
					allActivitiesList.append(ai);
					allActivitiesSet.insert(ai);
				}
			
			addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			tmpString+=writeActivitiesActivityTags(htmlLevel, allActivitiesList,
			 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_CODES
			 );
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";

			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				tmpString+=getHourString(hour, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(hour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getVerticalDayString(day, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
			else{
				tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
				
				if(realHour==0)
					tmpString+="          <th rowspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getVerticalRealDayString(realDay, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
				else tmpString+="          <!-- span -->\n";
			}
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td colspan=\"2\"></td><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(currentCount)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td colspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleActivityTagsTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, int maxActivityTag, QSet<int>& excludedNames, const QString& saveTime,
 bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
 bool repeatNames){
	assert(realDay>=0);
	assert(realDay<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nDaysPerWeek : gt.rules.nRealDaysPerWeek));

	QList<int> activityTagsList;
	int currentCount=0;
	for(int activityTag=0; activityTag<gt.rules.nInternalActivityTags && currentCount<maxActivityTag; activityTag++){
		if(gt.rules.internalActivityTagsList[activityTag]->printable){
			if(!excludedNames.contains(activityTag)){
				currentCount++;
				activityTagsList.append(activityTag);
				excludedNames.insert(activityTag);
			}
		}
	}

	QString tmpString;
	if(!PRINT_FROM_INTERFACE){
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	else{
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.daysOfTheWeek[realDay])+"\" border=\"1\">\n";
		else
			tmpString+="    <table align=\"center\" id=\"table_"+hashDayIDsTimetable.value(gt.rules.realDaysOfTheWeek[realDay])+"\" border=\"1\">\n";
	}
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr>\n          <td rowspan=\"2\"></td>\n";
	else
		tmpString+="      <thead>\n        <tr>\n";

	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+getDayString(realDay, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	else
		tmpString+="          <th colspan=\""+QString::number(gt.rules.nRealHoursPerDay)+"\">"+getRealDayString(realDay, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_DAYS_LONG_NAMES)+"</th>\n";
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <td rowspan=\"2\"></td>\n";
	}
	tmpString+="        </tr>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr>\n          <!-- span -->\n";
	else
		tmpString+="        <tr>\n";

	for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
		int realHour;
		if(!PRINT_RTL)
			realHour=rrealHour;
		else
			realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		if(gt.rules.mode!=MORNINGS_AFTERNOONS)
			tmpString+=getHourString(realHour, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
		else
			tmpString+=getRealHourString(realHour, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_HOURS_LONG_NAMES)+"</th>\n";
	}

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="          <!-- span -->\n";
	}

	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";

	for(int activityTag : std::as_const(activityTagsList)){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">"+getActivityTagString(gt.rules.internalActivityTagsList[activityTag], SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES)+"</th>\n";
			else
				tmpString+="          <th>"+getActivityTagString(gt.rules.internalActivityTagsList[activityTag], SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES)+"</th>\n";
		}

		///////by Liviu Lalescu
		activitiesForCurrentActivityTag.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				activitiesForCurrentActivityTag[d][h].clear();
		for(int ai : std::as_const(gt.rules.activitiesForActivityTagList[activityTag]))
			if(best_solution.times[ai]!=UNALLOCATED_TIME){
				int d=best_solution.times[ai]%gt.rules.nDaysPerWeek;
				int h=best_solution.times[ai]/gt.rules.nDaysPerWeek;
				Activity* act=&gt.rules.internalActivitiesList[ai];
				for(int dd=0; dd < act->duration && h+dd < gt.rules.nHoursPerDay; dd++)
					activitiesForCurrentActivityTag[d][h+dd].append(ai);
			}
		///////end Liviu Lalescu

		for(int rrealHour=0; rrealHour<(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay); rrealHour++){
			int realHour;
			if(!PRINT_RTL)
				realHour=rrealHour;
			else
				realHour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)-1-rrealHour;

			int day=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realDay : realDay*2+realHour/gt.rules.nHoursPerDay);
			int hour=(gt.rules.mode!=MORNINGS_AFTERNOONS ? realHour : realHour%gt.rules.nHoursPerDay);

			QList<int> allActivitiesList;
			QSet<int> allActivitiesSet;
			
			allActivitiesList=activitiesForCurrentActivityTag[day][hour];
			for(int ai : std::as_const(activitiesForCurrentActivityTag[day][hour]))
				allActivitiesSet.insert(ai);

			addActivitiesWithSameStartingTime(allActivitiesList, allActivitiesSet, hour);
			tmpString+=writeActivitiesActivityTags(htmlLevel, allActivitiesList,
			 printSubjects, printActivityTags, printTeachers, printStudents, printRooms,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_SUBJECTS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_TEACHERS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_STUDENTS_CODES,
			 SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ROOMS_CODES
			 );
		}
		if((!PRINT_RTL && repeatNames) || PRINT_RTL){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">"+getActivityTagString(gt.rules.internalActivityTagsList[activityTag], SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES)+"</th>\n";
			else
				tmpString+="          <th>"+getActivityTagString(gt.rules.internalActivityTagsList[activityTag], SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_LONG_NAMES, SETTINGS_TIMETABLES_ACTIVITY_TAGS_TIME_HV_PRINT_ACTIVITY_TAGS_CODES)+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.nHoursPerDay : gt.rules.nRealHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if((!PRINT_RTL && repeatNames) || PRINT_RTL){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleTeachersFreePeriodsTimetableDaysHorizontalHtml(int htmlLevel, const QString& saveTime, bool detailed, bool repeatNames){
	QString tmpString;
	
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table\" border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" id=\"table\" border=\"1\">\n";
	
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Teachers' Free Periods"))+"</th>";
	else
		tmpString+="      <thead>\n        <tr><th colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Teachers' Free Periods"))+"</th>";
	
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	
	tmpString+="</tr>\n";
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
		int day;
		if(!PRINT_RTL)
			day=rday;
		else
			day=gt.rules.nDaysPerWeek-1-rday;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nDaysPerWeek+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int hour=0; hour<gt.rules.nHoursPerDay; hour++){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
		for(int rday=0; rday<gt.rules.nDaysPerWeek; rday++){
			int day;
			if(!PRINT_RTL)
				day=rday;
			else
				day=gt.rules.nDaysPerWeek-1-rday;

			bool empty_slot;
			empty_slot=true;
			for(int tfp=0; tfp<TEACHERS_FREE_PERIODS_N_CATEGORIES; tfp++){
				if(teachers_free_periods_timetable_weekly[tfp][day][hour].size()>0){
					empty_slot=false;
				}
				if(!detailed&&tfp>=TEACHER_MUST_COME_EARLIER) break;
			}
			if(!empty_slot) tmpString+="          <td>";
			for(int tfp=0; tfp<TEACHERS_FREE_PERIODS_N_CATEGORIES; tfp++){
				if(teachers_free_periods_timetable_weekly[tfp][day][hour].size()>0){
					if(htmlLevel>=2)
						tmpString+="<div class=\"DESCRIPTION\">";
					switch(tfp){
						case TEACHER_HAS_SINGLE_GAP		: tmpString+=protect2(TimetableExport::tr("Single gap")); break;
						case TEACHER_HAS_BORDER_GAP		: tmpString+=protect2(TimetableExport::tr("Border gap")); break;
						case TEACHER_HAS_BIG_GAP		: tmpString+=protect2(TimetableExport::tr("Big gap")); break;
						case TEACHER_MUST_COME_EARLIER		: tmpString+=protect2(TimetableExport::tr("Must come earlier")); break;
						case TEACHER_MUST_STAY_LONGER		: tmpString+=protect2(TimetableExport::tr("Must stay longer")); break;
						case TEACHER_MUST_COME_MUCH_EARLIER	: tmpString+=protect2(TimetableExport::tr("Must come much earlier")); break;
						case TEACHER_MUST_STAY_MUCH_LONGER	: tmpString+=protect2(TimetableExport::tr("Must stay much longer")); break;
						case TEACHER_HAS_A_FREE_DAY		: tmpString+=protect2(TimetableExport::tr("Free day")); break;
						case TEACHER_IS_NOT_AVAILABLE		: tmpString+=protect2(TimetableExport::tr("Not available", "It refers to a teacher")); break;
						default: assert(0==1); break;
					}
					if(htmlLevel>=2)
						tmpString+=":</div>";
					else tmpString+=":<br />";
					if(htmlLevel>=3)
						switch(tfp){
							case TEACHER_HAS_SINGLE_GAP		: tmpString+="<div class=\"TEACHER_HAS_SINGLE_GAP\">"; break;
							case TEACHER_HAS_BORDER_GAP		: tmpString+="<div class=\"TEACHER_HAS_BORDER_GAP\">"; break;
							case TEACHER_HAS_BIG_GAP		: tmpString+="<div class=\"TEACHER_HAS_BIG_GAP\">"; break;
							case TEACHER_MUST_COME_EARLIER		: tmpString+="<div class=\"TEACHER_MUST_COME_EARLIER\">"; break;
							case TEACHER_MUST_STAY_LONGER		: tmpString+="<div class=\"TEACHER_MUST_STAY_LONGER\">"; break;
							case TEACHER_MUST_COME_MUCH_EARLIER	: tmpString+="<div class=\"TEACHER_MUST_COME_MUCH_EARLIER\">"; break;
							case TEACHER_MUST_STAY_MUCH_LONGER	: tmpString+="<div class=\"TEACHER_MUST_STAY_MUCH_LONGER\">"; break;
							case TEACHER_HAS_A_FREE_DAY		: tmpString+="<div class=\"TEACHER_HAS_A_FREE_DAY\">"; break;
							case TEACHER_IS_NOT_AVAILABLE		: tmpString+="<div class=\"TEACHER_IS_NOT_AVAILABLE\">"; break;
							default: assert(0==1); break;
						}
					for(int t=0; t<teachers_free_periods_timetable_weekly[tfp][day][hour].size(); t++){
						QString teacher_name = gt.rules.internalTeachersList[teachers_free_periods_timetable_weekly[tfp][day][hour].at(t)]->name;
							switch(htmlLevel){
								case 4 : tmpString+="<span class=\"t_"+hashTeacherIDsTimetable.value(teacher_name)+"\">"+protect2(teacher_name)+"</span>"; break;
								case 5 : [[fallthrough]];
								case 6 : tmpString+="<span class=\"t_"+hashTeacherIDsTimetable.value(teacher_name)+"\" onmouseover=\"highlight('t_"+hashTeacherIDsTimetable.value(teacher_name)+"')\">"+protect2(teacher_name)+"</span>"; break;
								default: tmpString+=protect2(teacher_name); break;
							}
						tmpString+="<br />";
					}
					if(htmlLevel>=3)
						tmpString+="</div>";
				}
				if(!detailed&&tfp>=TEACHER_MUST_COME_EARLIER) break;
			}
			if(!empty_slot){
				tmpString+="</td>\n";
			} else {
				tmpString+=writeEmpty(htmlLevel);
			}
		}
		if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.nDaysPerWeek)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleTeachersFreePeriodsTimetableDaysVerticalHtml(int htmlLevel, const QString& saveTime, bool detailed, bool repeatNames){
	QString tmpString;
	
	if(!PRINT_FROM_INTERFACE)
		tmpString+="    <table id=\"table\" border=\"1\">\n";
	else
		tmpString+="    <table align=\"center\" id=\"table\" border=\"1\">\n";
	
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";

	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="      <thead>\n        <tr><td rowspan=\"2\"></td><th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(TimetableExport::tr("Teachers' Free Periods"))+"</th>";
	else
		tmpString+="      <thead>\n        <tr><th colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(TimetableExport::tr("Teachers' Free Periods"))+"</th>";

	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="<td rowspan=\"2\"></td>";
	}
	tmpString+="</tr>\n";
	
	tmpString+="        <tr>\n          <!-- span -->\n";
	for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
		int hour;
		if(!PRINT_RTL)
			hour=rhour;
		else
			hour=gt.rules.nHoursPerDay-1-rhour;
		
		if(htmlLevel>=2)
			tmpString+="          <th class=\"xAxis\">";
		else
			tmpString+="          <th>";
		tmpString+=protect2(gt.rules.hoursOfTheDay[hour])+"</th>\n";
	}
	tmpString+="        </tr>\n";
	tmpString+="      </thead>\n";
	/*workaround
	tmpString+="      <tfoot><tr><td></td><td colspan=\""+gt.rules.nHoursPerDay+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr></tfoot>\n";
	*/
	tmpString+="      <tbody>\n";
	for(int day=0; day<gt.rules.nDaysPerWeek; day++){
		tmpString+="        <tr>\n";
		if(!PRINT_RTL || (PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		}
		for(int rhour=0; rhour<gt.rules.nHoursPerDay; rhour++){
			int hour;
			if(!PRINT_RTL)
				hour=rhour;
			else
				hour=gt.rules.nHoursPerDay-1-rhour;

			bool empty_slot;
			empty_slot=true;
			for(int tfp=0; tfp<TEACHERS_FREE_PERIODS_N_CATEGORIES; tfp++){
				if(teachers_free_periods_timetable_weekly[tfp][day][hour].size()>0){
					empty_slot=false;
				}
				if(!detailed&&tfp>=TEACHER_MUST_COME_EARLIER) break;
			}
			if(!empty_slot) tmpString+="          <td>";
			for(int tfp=0; tfp<TEACHERS_FREE_PERIODS_N_CATEGORIES; tfp++){
				if(teachers_free_periods_timetable_weekly[tfp][day][hour].size()>0){
					if(htmlLevel>=2)
						tmpString+="<div class=\"DESCRIPTION\">";
					switch(tfp){
						case TEACHER_HAS_SINGLE_GAP		: tmpString+=protect2(TimetableExport::tr("Single gap")); break;
						case TEACHER_HAS_BORDER_GAP		: tmpString+=protect2(TimetableExport::tr("Border gap")); break;
						case TEACHER_HAS_BIG_GAP		: tmpString+=protect2(TimetableExport::tr("Big gap")); break;
						case TEACHER_MUST_COME_EARLIER		: tmpString+=protect2(TimetableExport::tr("Must come earlier")); break;
						case TEACHER_MUST_STAY_LONGER		: tmpString+=protect2(TimetableExport::tr("Must stay longer")); break;
						case TEACHER_MUST_COME_MUCH_EARLIER	: tmpString+=protect2(TimetableExport::tr("Must come much earlier")); break;
						case TEACHER_MUST_STAY_MUCH_LONGER	: tmpString+=protect2(TimetableExport::tr("Must stay much longer")); break;
						case TEACHER_HAS_A_FREE_DAY		: tmpString+=protect2(TimetableExport::tr("Free day")); break;
						case TEACHER_IS_NOT_AVAILABLE		: tmpString+=protect2(TimetableExport::tr("Not available")); break;
						default: assert(0==1); break;
					}
					if(htmlLevel>=2)
						tmpString+=":</div>";
					else tmpString+=":<br />";
					if(htmlLevel>=3)
						switch(tfp){
							case TEACHER_HAS_SINGLE_GAP		: tmpString+="<div class=\"TEACHER_HAS_SINGLE_GAP\">"; break;
							case TEACHER_HAS_BORDER_GAP		: tmpString+="<div class=\"TEACHER_HAS_BORDER_GAP\">"; break;
							case TEACHER_HAS_BIG_GAP		: tmpString+="<div class=\"TEACHER_HAS_BIG_GAP\">"; break;
							case TEACHER_MUST_COME_EARLIER		: tmpString+="<div class=\"TEACHER_MUST_COME_EARLIER\">"; break;
							case TEACHER_MUST_STAY_LONGER		: tmpString+="<div class=\"TEACHER_MUST_STAY_LONGER\">"; break;
							case TEACHER_MUST_COME_MUCH_EARLIER	: tmpString+="<div class=\"TEACHER_MUST_COME_MUCH_EARLIER\">"; break;
							case TEACHER_MUST_STAY_MUCH_LONGER	: tmpString+="<div class=\"TEACHER_MUST_STAY_MUCH_LONGER\">"; break;
							case TEACHER_HAS_A_FREE_DAY		: tmpString+="<div class=\"TEACHER_HAS_A_FREE_DAY\">"; break;
							case TEACHER_IS_NOT_AVAILABLE		: tmpString+="<div class=\"TEACHER_IS_NOT_AVAILABLE\">"; break;
							default: assert(0==1); break;
						}
					for(int t=0; t<teachers_free_periods_timetable_weekly[tfp][day][hour].size(); t++){
						QString teacher_name = gt.rules.internalTeachersList[teachers_free_periods_timetable_weekly[tfp][day][hour].at(t)]->name;
							switch(htmlLevel){
								case 4 : tmpString+="<span class=\"t_"+hashTeacherIDsTimetable.value(teacher_name)+"\">"+protect2(teacher_name)+"</span>"; break;
								case 5 : [[fallthrough]];
								case 6 : tmpString+="<span class=\"t_"+hashTeacherIDsTimetable.value(teacher_name)+"\" onmouseover=\"highlight('t_"+hashTeacherIDsTimetable.value(teacher_name)+"')\">"+protect2(teacher_name)+"</span>"; break;
								default: tmpString+=protect2(teacher_name); break;
							}
						tmpString+="<br />";
					}
					if(htmlLevel>=3)
						tmpString+="</div>";
				}
				if(!detailed&&tfp>=TEACHER_MUST_COME_EARLIER) break;
			}
			if(!empty_slot){
				tmpString+="</td>\n";
			} else
				tmpString+=writeEmpty(htmlLevel);
		}
		if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
			if(htmlLevel>=2)
				tmpString+="          <th class=\"yAxis\">";
			else
				tmpString+="          <th>";
			tmpString+=protect2(gt.rules.daysOfTheWeek[day])+"</th>\n";
		}
		tmpString+="        </tr>\n";
	}
	//workaround begin.
	if(!PRINT_RTL || (PRINT_RTL && repeatNames))
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	else
		tmpString+="        <tr class=\"foot\"><td colspan=\""+QString::number(gt.rules.nHoursPerDay)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
	if(PRINT_RTL || (!PRINT_RTL && repeatNames)){
		tmpString+="<td></td>";
	}
	tmpString+="</tr>\n";
	//workaround end.
	tmpString+="      </tbody>\n";
	tmpString+="    </table>\n";
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleTeachersStatisticsHtml(int htmlLevel, const QString& saveTime, bool detailed, bool repeatNames, bool printAll){
	Q_UNUSED(htmlLevel);
	QString tmpString;
	if(!printAll){
		tmpString+="    <p>\n";
		tmpString+="      <strong>"+protect2(tr("This is a teaser only. Values are not correct!"))+"</strong>\n";
		tmpString+="    </p>\n";
	}
	tmpString+="    <p>\n";
	tmpString+="      "+protect2(tr("This file doesn't list limits that are set by constraints. It contains statistics about the minimum and the maximum values of the currently generated solution."))+"\n";
	tmpString+="    </p>\n";
	
	QString teachersString="";
	int freeDaysAllTeachers=0;
	int minFreeDaysAllTeachers=gt.rules.nDaysPerWeek;
	int maxFreeDaysAllTeachers=0;
	int gapsAllTeachers=0;
	int minGapsPerDayAllTeachers=gt.rules.nHoursPerDay;
	int maxGapsPerDayAllTeachers=0;
	int minGapsPerWeekAllTeachers=gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek;
	int maxGapsPerWeekAllTeachers=0;
	int minHoursPerDayAllTeachers=gt.rules.nHoursPerDay;
	int maxHoursPerDayAllTeachers=0;
	int minHoursPerWeekAllTeachers=gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek;
	int maxHoursPerWeekAllTeachers=0;
	int totalNumberOfHours=0;
	for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
		int freeDaysSingleTeacher=0;
		int gapsSingleTeacher=0;
		int minGapsPerDaySingleTeacher=gt.rules.nHoursPerDay;
		int maxGapsPerDaySingleTeacher=0;
		int minHoursPerDaySingleTeacher=gt.rules.nHoursPerDay;
		int maxHoursPerDaySingleTeacher=0;
		int hoursPerWeekSingleTeacher=0;
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			int firstPeriod=-1;
			int lastPeriod=-1;
			int gapsPerDaySingleTeacher=0;
			int hoursPerDaySingleTeacher=0;
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				if(teachers_timetable_weekly[tch][d][h]!=UNALLOCATED_ACTIVITY){
					if(firstPeriod==-1)
						firstPeriod=h;
					lastPeriod=h;
					hoursPerDaySingleTeacher++;
				}
			}
			if(firstPeriod==-1){
				freeDaysSingleTeacher++;
			} else {
				for(int h=firstPeriod; h<lastPeriod; h++){
					if(teachers_timetable_weekly[tch][d][h]==UNALLOCATED_ACTIVITY && teacherNotAvailableDayHour[tch][d][h]==false && breakDayHour[d][h]==false){
						gapsPerDaySingleTeacher++;
					}
				}
			}
			gapsSingleTeacher+=gapsPerDaySingleTeacher;
			if(minGapsPerDaySingleTeacher>gapsPerDaySingleTeacher)
				minGapsPerDaySingleTeacher=gapsPerDaySingleTeacher;
			if(maxGapsPerDaySingleTeacher<gapsPerDaySingleTeacher)
				maxGapsPerDaySingleTeacher=gapsPerDaySingleTeacher;
			if(hoursPerDaySingleTeacher>0){
				if(minHoursPerDaySingleTeacher>hoursPerDaySingleTeacher)
					minHoursPerDaySingleTeacher=hoursPerDaySingleTeacher;
				if(maxHoursPerDaySingleTeacher<hoursPerDaySingleTeacher)
					maxHoursPerDaySingleTeacher=hoursPerDaySingleTeacher;
			}
			hoursPerWeekSingleTeacher+=hoursPerDaySingleTeacher;
		}
		totalNumberOfHours+=hoursPerWeekSingleTeacher;
		if(minHoursPerWeekAllTeachers>hoursPerWeekSingleTeacher)
			minHoursPerWeekAllTeachers=hoursPerWeekSingleTeacher;
		if(maxHoursPerWeekAllTeachers<hoursPerWeekSingleTeacher)
			maxHoursPerWeekAllTeachers=hoursPerWeekSingleTeacher;
		
		if(minFreeDaysAllTeachers>freeDaysSingleTeacher)
			minFreeDaysAllTeachers=freeDaysSingleTeacher;
		if(maxFreeDaysAllTeachers<freeDaysSingleTeacher)
			maxFreeDaysAllTeachers=freeDaysSingleTeacher;
		
		if(minGapsPerDayAllTeachers>minGapsPerDaySingleTeacher)
			minGapsPerDayAllTeachers=minGapsPerDaySingleTeacher;
		if(maxGapsPerDayAllTeachers<maxGapsPerDaySingleTeacher)
			maxGapsPerDayAllTeachers=maxGapsPerDaySingleTeacher;
			
		if(minGapsPerWeekAllTeachers>gapsSingleTeacher)
			minGapsPerWeekAllTeachers=gapsSingleTeacher;
		if(maxGapsPerWeekAllTeachers<gapsSingleTeacher)
			maxGapsPerWeekAllTeachers=gapsSingleTeacher;
		
		if(minHoursPerDayAllTeachers>minHoursPerDaySingleTeacher)
			minHoursPerDayAllTeachers=minHoursPerDaySingleTeacher;
		if(maxHoursPerDayAllTeachers<maxHoursPerDaySingleTeacher)
			maxHoursPerDayAllTeachers=maxHoursPerDaySingleTeacher;

		gapsAllTeachers+=gapsSingleTeacher;
		freeDaysAllTeachers+=freeDaysSingleTeacher;
		
		if(detailed){
			if(freeDaysSingleTeacher==gt.rules.nDaysPerWeek)
				minHoursPerDaySingleTeacher=0;
			teachersString+="      <tr><th>"+protect2(gt.rules.internalTeachersList[tch]->name)
								+"</th><td>"+QString::number(hoursPerWeekSingleTeacher)
								+"</td><td>"+QString::number(freeDaysSingleTeacher)
								+"</td><td>"+QString::number(gapsSingleTeacher)
								+"</td><td>"+QString::number(minGapsPerDaySingleTeacher)
								+"</td><td>"+QString::number(maxGapsPerDaySingleTeacher)
								+"</td><td>"+QString::number(minHoursPerDaySingleTeacher)
								+"</td><td>"+QString::number(maxHoursPerDaySingleTeacher)
								+"</td>";
			
			if(repeatNames){
				teachersString+="<th>"+protect2(gt.rules.internalTeachersList[tch]->name)+"</th>";
			}
			teachersString+="</tr>\n";
		}
		if(!printAll && tch>10){
			break;
		}
	}
	
	tmpString+="    <table border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n";
	tmpString+="        <tr><th>"+protect2(tr("All teachers"))
				+"</th><th>"+protect2(tr("Hours per week"))
				+"</th><th>"+protect2(tr("Free days"))
				+"</th><th>"+protect2(tr("Gaps"))
				+"</th><th>"+protect2(tr("Gaps per day"))
				+"</th><th>"+protect2(tr("Hours per day"))
				+"</th></tr>\n";
	tmpString+="      </thead>\n";
	tmpString+="      <tr><th>"+protect2(tr("Sum"))+"</th>";
	tmpString+="<td>"+QString::number(totalNumberOfHours)+"</td>";
	tmpString+="<td>"+QString::number(freeDaysAllTeachers)+"</td>";
	tmpString+="<td>"+QString::number(gapsAllTeachers)+"</td>";
	tmpString+="<td>---</td>";
	tmpString+="<td>---</td>";
	tmpString+="</tr>\n";
	tmpString+="      <tr><th>"+protect2(tr("Average"))+"</th>";
	tmpString+="<td>"+QString::number(double(totalNumberOfHours)/gt.rules.nInternalTeachers,'f',2)+"</td>";
	tmpString+="<td>"+QString::number(double(freeDaysAllTeachers)/gt.rules.nInternalTeachers,'f',2)+"</td>";
	tmpString+="<td>"+QString::number(double(gapsAllTeachers)/gt.rules.nInternalTeachers,'f',2)+"</td>";
	tmpString+="<td>---</td>";
	tmpString+="<td>---</td>";
	tmpString+="</tr>\n";
	tmpString+="      <tr><th>"+protect2(tr("Min"))+"</th>";
	tmpString+="<td>"+QString::number(minHoursPerWeekAllTeachers)+"</td>";
	tmpString+="<td>"+QString::number(minFreeDaysAllTeachers)+"</td>";
	tmpString+="<td>"+QString::number(minGapsPerWeekAllTeachers)+"</td>";
	tmpString+="<td>"+QString::number(minGapsPerDayAllTeachers)+"</td>";
	tmpString+="<td>"+QString::number(minHoursPerDayAllTeachers)+"</td>";
	tmpString+="</tr>\n";
	tmpString+="      <tr><th>"+protect2(tr("Max"))+"</th>";
	tmpString+="<td>"+QString::number(maxHoursPerWeekAllTeachers)+"</td>";
	tmpString+="<td>"+QString::number(maxFreeDaysAllTeachers)+"</td>";
	tmpString+="<td>"+QString::number(maxGapsPerWeekAllTeachers)+"</td>";
	tmpString+="<td>"+QString::number(maxGapsPerDayAllTeachers)+"</td>";
	tmpString+="<td>"+QString::number(maxHoursPerDayAllTeachers)+"</td>";
	tmpString+="</tr>\n";
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(5)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr>\n";
	//workaround end.
	tmpString+="    </table>\n";
	
	if(detailed){
		tmpString+="    <p class=\"back0\"><br /></p>\n";
		
		tmpString+="    <table border=\"1\">\n";
		tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
		tmpString+="      <thead>\n";
		tmpString+="        <tr><th>"+protect2(tr("Teacher"))
			+"</th><th>"+protect2(tr("Hours per week"))
			+"</th><th>"+protect2(tr("Free days"))
			+"</th><th>"+protect2(tr("Total gaps"))
			+"</th><th>"+protect2(tr("Min gaps per day"))
			+"</th><th>"+protect2(tr("Max gaps per day"))
			+"</th><th>"+protect2(tr("Min hours per day"))
			+"</th><th>"+protect2(tr("Max hours per day"))
			+"</th>";
		if(repeatNames){
			tmpString+="<th>"+protect2(tr("Teacher"))+"</th>";
		}
		tmpString+="</tr>\n";
		tmpString+="      </thead>\n";
		tmpString+=teachersString;
		//workaround begin.
		tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(7)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
		if(repeatNames){
			tmpString+="<td></td>";
		}
		tmpString+="</tr>\n";
		//workaround end.
		tmpString+="    </table>\n";
	}
	return tmpString;
}

//by Volker Dirr
QString TimetableExport::singleStudentsStatisticsHtml(int htmlLevel, const QString& saveTime, bool detailed, bool repeatNames, bool printAll){
	Q_UNUSED(htmlLevel);
	QString tmpString;
	if(!printAll){
		tmpString+="    <p>\n";
		tmpString+="      <strong>"+protect2(tr("This is a teaser only. Values are not correct!"))+"</strong>\n";
		tmpString+="    </p>\n";
	}
	tmpString+="    <p>\n";
	tmpString+="      "+protect2(tr("This file doesn't list limits that are set by constraints. It contains statistics about the minimum and the maximum values of the currently generated solution."))+"\n";
	tmpString+="    </p>\n";
	
	//subgroups statistics (start)
	QString subgroupsString="";
	int freeDaysAllSubgroups=0;
	int minFreeDaysAllSubgroups=gt.rules.nDaysPerWeek;
	int maxFreeDaysAllSubgroups=0;
	int gapsAllSubgroups=0;
	int minGapsPerDayAllSubgroups=gt.rules.nHoursPerDay;
	int maxGapsPerDayAllSubgroups=0;
	int minGapsPerWeekAllSubgroups=gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek;
	int maxGapsPerWeekAllSubgroups=0;
	int minHoursPerDayAllSubgroups=gt.rules.nHoursPerDay;
	int maxHoursPerDayAllSubgroups=0;
	int minHoursPerWeekAllSubgroups=gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek;
	int maxHoursPerWeekAllSubgroups=0;
	int totalNumberOfHours=0;
	QList<int> freeDaysPerWeekSubgroupList;
	QList<int> gapsPerWeekSubgroupList;
	QList<int> minGapsPerDaySubgroupList;
	QList<int> maxGapsPerDaySubgroupList;
	QList<int> minHoursPerDaySubgroupList;
	QList<int> maxHoursPerDaySubgroupList;
	QList<int> hoursPerWeekSubgroupList;
	for(int subgroup=0; subgroup<gt.rules.nInternalSubgroups; subgroup++){
		int freeDaysSingleSubgroup=0;
		int gapsSingleSubgroup=0;
		int minGapsPerDaySingleSubgroup=gt.rules.nHoursPerDay;
		int maxGapsPerDaySingleSubgroup=0;
		int minHoursPerDaySingleSubgroup=gt.rules.nHoursPerDay;
		int maxHoursPerDaySingleSubgroup=0;
		int hoursPerWeekSingleSubgroup=0;
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			int firstPeriod=-1;
			int lastPeriod=-1;
			int gapsPerDaySingleSubgroup=0;
			int hoursPerDaySingleSubgroup=0;
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				if(students_timetable_weekly[subgroup][d][h]!=UNALLOCATED_ACTIVITY){
					if(firstPeriod==-1)
						firstPeriod=h;
					lastPeriod=h;
					hoursPerDaySingleSubgroup++;
				}
			}
			if(firstPeriod==-1){
				freeDaysSingleSubgroup++;
			} else {
				for(int h=firstPeriod; h<lastPeriod; h++){
					if(students_timetable_weekly[subgroup][d][h]==UNALLOCATED_ACTIVITY && subgroupNotAvailableDayHour[subgroup][d][h]==false && breakDayHour[d][h]==false){
						gapsPerDaySingleSubgroup++;
					}
				}
			}
			gapsSingleSubgroup+=gapsPerDaySingleSubgroup;
			if(minGapsPerDaySingleSubgroup>gapsPerDaySingleSubgroup)
				minGapsPerDaySingleSubgroup=gapsPerDaySingleSubgroup;
			if(maxGapsPerDaySingleSubgroup<gapsPerDaySingleSubgroup)
				maxGapsPerDaySingleSubgroup=gapsPerDaySingleSubgroup;
			if(hoursPerDaySingleSubgroup>0){
				if(minHoursPerDaySingleSubgroup>hoursPerDaySingleSubgroup)
					minHoursPerDaySingleSubgroup=hoursPerDaySingleSubgroup;
				if(maxHoursPerDaySingleSubgroup<hoursPerDaySingleSubgroup)
					maxHoursPerDaySingleSubgroup=hoursPerDaySingleSubgroup;
			}
			hoursPerWeekSingleSubgroup+=hoursPerDaySingleSubgroup;
		}
		totalNumberOfHours+=hoursPerWeekSingleSubgroup;
		if(minHoursPerWeekAllSubgroups>hoursPerWeekSingleSubgroup)
			minHoursPerWeekAllSubgroups=hoursPerWeekSingleSubgroup;
		if(maxHoursPerWeekAllSubgroups<hoursPerWeekSingleSubgroup)
			maxHoursPerWeekAllSubgroups=hoursPerWeekSingleSubgroup;
		
		if(minFreeDaysAllSubgroups>freeDaysSingleSubgroup)
			minFreeDaysAllSubgroups=freeDaysSingleSubgroup;
		if(maxFreeDaysAllSubgroups<freeDaysSingleSubgroup)
			maxFreeDaysAllSubgroups=freeDaysSingleSubgroup;
		
		if(minGapsPerDayAllSubgroups>minGapsPerDaySingleSubgroup)
			minGapsPerDayAllSubgroups=minGapsPerDaySingleSubgroup;
		if(maxGapsPerDayAllSubgroups<maxGapsPerDaySingleSubgroup)
			maxGapsPerDayAllSubgroups=maxGapsPerDaySingleSubgroup;
			
		if(minGapsPerWeekAllSubgroups>gapsSingleSubgroup)
			minGapsPerWeekAllSubgroups=gapsSingleSubgroup;
		if(maxGapsPerWeekAllSubgroups<gapsSingleSubgroup)
			maxGapsPerWeekAllSubgroups=gapsSingleSubgroup;
		
		if(minHoursPerDayAllSubgroups>minHoursPerDaySingleSubgroup)
			minHoursPerDayAllSubgroups=minHoursPerDaySingleSubgroup;
		if(maxHoursPerDayAllSubgroups<maxHoursPerDaySingleSubgroup)
			maxHoursPerDayAllSubgroups=maxHoursPerDaySingleSubgroup;

		gapsAllSubgroups+=gapsSingleSubgroup;
		freeDaysAllSubgroups+=freeDaysSingleSubgroup;
		
		if(freeDaysSingleSubgroup==gt.rules.nDaysPerWeek)
			minHoursPerDaySingleSubgroup=0;
		if(detailed){
			subgroupsString+="      <tr><th>"+protect2(gt.rules.internalSubgroupsList[subgroup]->name)
							+"</th><td>"+QString::number(hoursPerWeekSingleSubgroup)
							+"</td><td>"+QString::number(freeDaysSingleSubgroup)
							+"</td><td>"+QString::number(gapsSingleSubgroup)
							+"</td><td>"+QString::number(minGapsPerDaySingleSubgroup)
							+"</td><td>"+QString::number(maxGapsPerDaySingleSubgroup)
							+"</td><td>"+QString::number(minHoursPerDaySingleSubgroup)
							+"</td><td>"+QString::number(maxHoursPerDaySingleSubgroup)
							+"</td>";
			if(repeatNames){
				subgroupsString+="<th>"+protect2(gt.rules.internalSubgroupsList[subgroup]->name)+"</th>";
			}
			subgroupsString+="</tr>\n";
			hoursPerWeekSubgroupList<<hoursPerWeekSingleSubgroup;
			freeDaysPerWeekSubgroupList<<freeDaysSingleSubgroup;
			gapsPerWeekSubgroupList<<gapsSingleSubgroup;
			minGapsPerDaySubgroupList<<minGapsPerDaySingleSubgroup;
			maxGapsPerDaySubgroupList<<maxGapsPerDaySingleSubgroup;
			minHoursPerDaySubgroupList<<minHoursPerDaySingleSubgroup;
			maxHoursPerDaySubgroupList<<maxHoursPerDaySingleSubgroup;
		}
		if(!printAll && subgroup>10){
			break;
		}
	}

	tmpString+="    <table border=\"1\">\n";
	tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
	tmpString+="      <thead>\n";
	tmpString+="        <tr><th>"+protect2(tr("All students"))
				+"</th><th>"+protect2(tr("Hours per week"))
				+"</th><th>"+protect2(tr("Free days"))
				+"</th><th>"+protect2(tr("Gaps"))
				+"</th><th>"+protect2(tr("Gaps per day"))
				+"</th><th>"+protect2(tr("Hours per day"))
				+"</th></tr>\n";
	tmpString+="      </thead>\n";
	tmpString+="      <tr><th>"+protect2(tr("Sum"))+"</th>";
	tmpString+="<td>"+QString::number(totalNumberOfHours)+"</td>";
	tmpString+="<td>"+QString::number(freeDaysAllSubgroups)+"</td>";
	tmpString+="<td>"+QString::number(gapsAllSubgroups)+"</td>";
	tmpString+="<td>---</td>";
	tmpString+="<td>---</td>";
	tmpString+="</tr>\n";
	tmpString+="      <tr><th>"+protect2(tr("Average"))+"</th>";
	tmpString+="<td>"+QString::number(double(totalNumberOfHours)/gt.rules.nInternalSubgroups,'f',2)+"</td>";
	tmpString+="<td>"+QString::number(double(freeDaysAllSubgroups)/gt.rules.nInternalSubgroups,'f',2)+"</td>";
	tmpString+="<td>"+QString::number(double(gapsAllSubgroups)/gt.rules.nInternalSubgroups,'f',2)+"</td>";
	tmpString+="<td>---</td>";
	tmpString+="<td>---</td>";
	tmpString+="</tr>\n";
	tmpString+="      <tr><th>"+protect2(tr("Min"))+"</th>";
	tmpString+="<td>"+QString::number(minHoursPerWeekAllSubgroups)+"</td>";
	tmpString+="<td>"+QString::number(minFreeDaysAllSubgroups)+"</td>";
	tmpString+="<td>"+QString::number(minGapsPerWeekAllSubgroups)+"</td>";
	tmpString+="<td>"+QString::number(minGapsPerDayAllSubgroups)+"</td>";
	tmpString+="<td>"+QString::number(minHoursPerDayAllSubgroups)+"</td>";
	tmpString+="</tr>\n";
	tmpString+="      <tr><th>"+protect2(tr("Max"))+"</th>";
	tmpString+="<td>"+QString::number(maxHoursPerWeekAllSubgroups)+"</td>";
	tmpString+="<td>"+QString::number(maxFreeDaysAllSubgroups)+"</td>";
	tmpString+="<td>"+QString::number(maxGapsPerWeekAllSubgroups)+"</td>";
	tmpString+="<td>"+QString::number(maxGapsPerDayAllSubgroups)+"</td>";
	tmpString+="<td>"+QString::number(maxHoursPerDayAllSubgroups)+"</td>";
	tmpString+="</tr>\n";
	//workaround begin.
	tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(5)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td></tr>\n";
	//workaround end.
	tmpString+="    </table>\n";
	
	tmpString+="    <p class=\"back0\"><br /></p>\n";
	
	//subgroups statistics (end)
	
	if(detailed){
		if(!printAll){
			//similar to source in else part (start)
			tmpString+="    <p></p>\n";
			tmpString+="    <table border=\"1\">\n";
			tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
			tmpString+="      <thead>\n";
			tmpString+="      <tr><th>"+protect2(tr("Subgroup"))
					+"</th><th>"+protect2(tr("Hours per week"))
					+"</th><th>"+protect2(tr("Free days"))
					+"</th><th>"+protect2(tr("Total gaps"))
					+"</th><th>"+protect2(tr("Min gaps per day"))
					+"</th><th>"+protect2(tr("Max gaps per day"))
					+"</th><th>"+protect2(tr("Min hours per day"))
					+"</th><th>"+protect2(tr("Max hours per day"))
					+"</th>";
			if(repeatNames){
				tmpString+="<td>"+protect2(tr("Subgroup"))+"</td>";
			}
			tmpString+="</tr>\n";
			tmpString+="      </thead>\n";
			tmpString+=subgroupsString;
			//workaround begin.
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(6)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
			if(repeatNames){
				tmpString+="<td></td>";
			}
			tmpString+="</tr>\n";
			//workaround end.
			tmpString+="    </table>\n";
			//similar to source in else part (end)
		} else {
			//groups and years statistics (start)
			QString yearsString="    <table border=\"1\">\n";
			yearsString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
			yearsString+="      <thead>\n";
			yearsString+="      <tr><th>"+protect2(tr("Year"))
							+"</th><th>"+protect2(tr("Min hours per week"))
							+"</th><th>"+protect2(tr("Max hours per week"))
							+"</th><th>"+protect2(tr("Min free days"))
							+"</th><th>"+protect2(tr("Max free days"))
							+"</th><th>"+protect2(tr("Min hours per day"))
							+"</th><th>"+protect2(tr("Max hours per day"))
							+"</th><th>"+protect2(tr("Min gaps per week"))
							+"</th><th>"+protect2(tr("Max gaps per week"))
							+"</th><th>"+protect2(tr("Min gaps per day"))
							+"</th><th>"+protect2(tr("Max gaps per day"))
							+"</th>";
			if(repeatNames){
					yearsString+="<th>"+protect2(tr("Year"))+"</th>";
			}
			yearsString+="</tr>\n";
			yearsString+="      </thead>\n";
			QString groupsString="    <table border=\"1\">\n";
			groupsString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
			groupsString+="      <thead>\n";
			groupsString+="      <tr><th>"+protect2(tr("Group"))
							+"</th><th>"+protect2(tr("Min hours per week"))
							+"</th><th>"+protect2(tr("Max hours per week"))
							+"</th><th>"+protect2(tr("Min free days"))
							+"</th><th>"+protect2(tr("Max free days"))
							+"</th><th>"+protect2(tr("Min hours per day"))
							+"</th><th>"+protect2(tr("Max hours per day"))
							+"</th><th>"+protect2(tr("Min gaps per week"))
							+"</th><th>"+protect2(tr("Max gaps per week"))
							+"</th><th>"+protect2(tr("Min gaps per day"))
							+"</th><th>"+protect2(tr("Max gaps per day"))
							+"</th>";
			if(repeatNames){
				groupsString+="<th>"+protect2(tr("Group"))+"</th>";
			}
			groupsString+="</tr>\n";
			groupsString+="      </thead>\n";
			for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
				StudentsYear* sty=gt.rules.augmentedYearsList[i];
				int minNumberOfHoursYear=gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek;
				int maxNumberOfHoursYear=0;
				int minFreeDaysPerWeekYear=gt.rules.nDaysPerWeek;
				int maxFreeDaysPerWeekYear=0;
				int minGapsPerDayYear=gt.rules.nHoursPerDay;
				int maxGapsPerDayYear=0;
				int minGapsPerWeekYear=gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek;
				int maxGapsPerWeekYear=0;
				int minHoursPerDayYear=gt.rules.nHoursPerDay;
				int maxHoursPerDayYear=0;
				for(int g=0; g<sty->groupsList.size(); g++){
					StudentsGroup* stg=sty->groupsList[g];
					int minNumberOfHoursGroup=gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek;
					int maxNumberOfHoursGroup=0;
					int minFreeDaysPerWeekGroup=gt.rules.nDaysPerWeek;
					int maxFreeDaysPerWeekGroup=0;
					int minGapsPerDayGroup=gt.rules.nHoursPerDay;
					int maxGapsPerDayGroup=0;
					int minGapsPerWeekGroup=gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek;
					int maxGapsPerWeekGroup=0;
					int minHoursPerDayGroup=gt.rules.nHoursPerDay;
					int maxHoursPerDayGroup=0;
					for(int sg=0; sg<stg->subgroupsList.size(); sg++){
						StudentsSubgroup* sts=stg->subgroupsList[sg];
						int subgroup=sts->indexInInternalSubgroupsList;

						if(minNumberOfHoursGroup>hoursPerWeekSubgroupList.at(subgroup))
							minNumberOfHoursGroup=hoursPerWeekSubgroupList.at(subgroup);
						if(maxNumberOfHoursGroup<hoursPerWeekSubgroupList.at(subgroup))
							maxNumberOfHoursGroup=hoursPerWeekSubgroupList.at(subgroup);
						
						if(minFreeDaysPerWeekGroup>freeDaysPerWeekSubgroupList.at(subgroup))
							minFreeDaysPerWeekGroup=freeDaysPerWeekSubgroupList.at(subgroup);
						if(maxFreeDaysPerWeekGroup<freeDaysPerWeekSubgroupList.at(subgroup))
							maxFreeDaysPerWeekGroup=freeDaysPerWeekSubgroupList.at(subgroup);
						
						if(minHoursPerDayGroup>minHoursPerDaySubgroupList.at(subgroup))
							minHoursPerDayGroup=minHoursPerDaySubgroupList.at(subgroup);
						if(maxHoursPerDayGroup<maxHoursPerDaySubgroupList.at(subgroup))
							maxHoursPerDayGroup=maxHoursPerDaySubgroupList.at(subgroup);
						
						if(minGapsPerWeekGroup>gapsPerWeekSubgroupList.at(subgroup))
							minGapsPerWeekGroup=gapsPerWeekSubgroupList.at(subgroup);
						if(maxGapsPerWeekGroup<gapsPerWeekSubgroupList.at(subgroup))
							maxGapsPerWeekGroup=gapsPerWeekSubgroupList.at(subgroup);
						
						if(minGapsPerDayGroup>minGapsPerDaySubgroupList.at(subgroup))
							minGapsPerDayGroup=minGapsPerDaySubgroupList.at(subgroup);
						if(maxGapsPerDayGroup<maxGapsPerDaySubgroupList.at(subgroup))
							maxGapsPerDayGroup=maxGapsPerDaySubgroupList.at(subgroup);
					}
					//print groups
					groupsString+="      <tr><th>"+protect2(stg->name)+"</th><td>"
					+QString::number(minNumberOfHoursGroup)+"</td><td>"+QString::number(maxNumberOfHoursGroup)+"</td><td>"
					+QString::number(minFreeDaysPerWeekGroup)+"</td><td>"+QString::number(maxFreeDaysPerWeekGroup)+"</td><td>"
					+QString::number(minHoursPerDayGroup)+"</td><td>"+QString::number(maxHoursPerDayGroup)+"</td><td>"
					+QString::number(minGapsPerWeekGroup)+"</td><td>"+QString::number(maxGapsPerWeekGroup)+"</td><td>"
					+QString::number(minGapsPerDayGroup)+"</td><td>"+QString::number(maxGapsPerDayGroup)+"</td>";
					if(repeatNames){
						groupsString+="<th>"+protect2(stg->name)+"</th>";
					}
					groupsString+="</tr>\n";

					//check years
					if(minNumberOfHoursYear>minNumberOfHoursGroup)
						minNumberOfHoursYear=minNumberOfHoursGroup;
					if(maxNumberOfHoursYear<maxNumberOfHoursGroup)
						maxNumberOfHoursYear=maxNumberOfHoursGroup;
					
					if(minFreeDaysPerWeekYear>minFreeDaysPerWeekGroup)
						minFreeDaysPerWeekYear=minFreeDaysPerWeekGroup;
					if(maxFreeDaysPerWeekYear<maxFreeDaysPerWeekGroup)
						maxFreeDaysPerWeekYear=maxFreeDaysPerWeekGroup;
					
					if(minHoursPerDayYear>minHoursPerDayGroup)
						minHoursPerDayYear=minHoursPerDayGroup;
					if(maxHoursPerDayYear<maxHoursPerDayGroup)
						maxHoursPerDayYear=maxHoursPerDayGroup;
						
					if(minGapsPerWeekYear>minGapsPerWeekGroup)
						minGapsPerWeekYear=minGapsPerWeekGroup;
					if(maxGapsPerWeekYear<maxGapsPerWeekGroup)
						maxGapsPerWeekYear=maxGapsPerWeekGroup;
						
					if(minGapsPerDayYear>minGapsPerDayGroup)
						minGapsPerDayYear=minGapsPerDayGroup;
					if(maxGapsPerDayYear<maxGapsPerDayGroup)
						maxGapsPerDayYear=maxGapsPerDayGroup;
				}
				//print years
					yearsString+="      <tr><th>"+protect2(sty->name)+"</th><td>"
					+QString::number(minNumberOfHoursYear)+"</td><td>"+QString::number(maxNumberOfHoursYear)+"</td><td>"
					+QString::number(minFreeDaysPerWeekYear)+"</td><td>"+QString::number(maxFreeDaysPerWeekYear)+"</td><td>"
					+QString::number(minHoursPerDayYear)+"</td><td>"+QString::number(maxHoursPerDayYear)+"</td><td>"
					+QString::number(minGapsPerWeekYear)+"</td><td>"+QString::number(maxGapsPerWeekYear)+"</td><td>"
					+QString::number(minGapsPerDayYear)+"</td><td>"+QString::number(maxGapsPerDayYear)+"</td>";
					if(repeatNames){
						yearsString+="<th>"+protect2(sty->name)+"</th>";
					}
					yearsString+="</tr>\n";
			}
			//workaround begin.
			groupsString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(10)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
			if(repeatNames){
				groupsString+="<td></td>";
			}
			groupsString+="</tr>\n";
			//workaround end.
			groupsString+="    </table>\n";
			groupsString+="    <p class=\"back0\"><br /></p>\n";
			//workaround begin.
			yearsString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(10)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
			if(repeatNames){
				yearsString+="<td></td>";
			}
			yearsString+="</tr>\n";
			//workaround end.
			yearsString+="    </table>\n";
			yearsString+="    <p class=\"back0\"><br /></p>\n";
			tmpString+=yearsString;
			tmpString+=groupsString;
			//similar to source in if part (start)
			tmpString+="    <p></p>\n";
			tmpString+="    <table border=\"1\">\n";
			tmpString+="      <caption>"+protect2(gt.rules.institutionName)+"</caption>\n";
			tmpString+="      <thead>\n";
			tmpString+="      <tr><th>"+protect2(tr("Subgroup"))
					+"</th><th>"+protect2(tr("Hours per week"))
					+"</th><th>"+protect2(tr("Free days"))
					+"</th><th>"+protect2(tr("Total gaps"))
					+"</th><th>"+protect2(tr("Min gaps per day"))
					+"</th><th>"+protect2(tr("Max gaps per day"))
					+"</th><th>"+protect2(tr("Min hours per day"))
					+"</th><th>"+protect2(tr("Max hours per day"))
					+"</th>";
			if(repeatNames){
				tmpString+="<td>"+protect2(tr("Subgroup"))+"</td>";
			}
			tmpString+="</tr>\n";
			tmpString+="      </thead>\n";
			tmpString+=subgroupsString;
			//workaround begin.
			tmpString+="        <tr class=\"foot\"><td></td><td colspan=\""+QString::number(7)+"\">"+protect2(TimetableExport::tr("Timetable generated with FET %1 on %2", "%1 is FET version, %2 is the date and time of generation").arg(FET_VERSION).arg(saveTime))+"</td>";
			if(repeatNames){
				tmpString+="<td></td>";
			}
			tmpString+="</tr>\n";
			//workaround end.
			tmpString+="    </table>\n";
			//similar to source in if part (end)
		}
	}
	//groups and years statistics (end)
	return tmpString;
}

QString TimetableExport::getSubjectString(Subject* sbj, bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes)
{
	QStringList tl;
	
	if(printSubjectsNames && !sbj->name.isEmpty())
		tl.append(protect2(sbj->name));
	if(printSubjectsLongNames && !sbj->longName.isEmpty())
		tl.append(protect2(sbj->longName));
	if(printSubjectsCodes && !sbj->code.isEmpty())
		tl.append(protect2(sbj->code));
	
	return tl.join(SETTINGS_TIMETABLES_SEPARATE_SUBJECTS_NAME_LONG_NAME_CODE_BY_BREAK?QString("<br />"):QString(" "));
}

QString TimetableExport::getActivityTagString(ActivityTag* at, bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes)
{
	QStringList tl;
	
	if(printActivityTagsNames && !at->name.isEmpty())
		tl.append(protect2(at->name));
	if(printActivityTagsLongNames && !at->longName.isEmpty())
		tl.append(protect2(at->longName));
	if(printActivityTagsCodes && !at->code.isEmpty())
		tl.append(protect2(at->code));
	
	return tl.join(SETTINGS_TIMETABLES_SEPARATE_ACTIVITY_TAGS_NAME_LONG_NAME_CODE_BY_BREAK?QString("<br />"):QString(" "));
}

QString TimetableExport::getTeacherString(Teacher* tch, bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes)
{
	QStringList tl;
	
	if(printTeachersNames && !tch->name.isEmpty())
		tl.append(protect2(tch->name));
	if(printTeachersLongNames && !tch->longName.isEmpty())
		tl.append(protect2(tch->longName));
	if(printTeachersCodes && !tch->code.isEmpty())
		tl.append(protect2(tch->code));
	
	return tl.join(SETTINGS_TIMETABLES_SEPARATE_TEACHERS_NAME_LONG_NAME_CODE_BY_BREAK?QString("<br />"):QString(" "));
}

QString TimetableExport::getStudentsSetString(StudentsSet* ss, bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes)
{
	QStringList tl;
	
	if(printStudentsNames && !ss->name.isEmpty())
		tl.append(protect2(ss->name));
	if(printStudentsLongNames && !ss->longName.isEmpty())
		tl.append(protect2(ss->longName));
	if(printStudentsCodes && !ss->code.isEmpty())
		tl.append(protect2(ss->code));
	
	return tl.join(SETTINGS_TIMETABLES_SEPARATE_STUDENTS_NAME_LONG_NAME_CODE_BY_BREAK?QString("<br />"):QString(" "));
}

QString TimetableExport::getBuildingString(Building* bu, bool printBuildingsNames, bool printBuildingsLongNames, bool printBuildingsCodes)
{
	QStringList tl;
	
	if(printBuildingsNames && !bu->name.isEmpty())
		tl.append(protect2(bu->name));
	if(printBuildingsLongNames && !bu->longName.isEmpty())
		tl.append(protect2(bu->longName));
	if(printBuildingsCodes && !bu->code.isEmpty())
		tl.append(protect2(bu->code));
	
	return tl.join(SETTINGS_TIMETABLES_SEPARATE_BUILDINGS_NAME_LONG_NAME_CODE_BY_BREAK?QString("<br />"):QString(" "));
}

QString TimetableExport::getRoomString(Room* rm, bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes)
{
	QStringList tl;
	
	if(printRoomsNames && !rm->name.isEmpty())
		tl.append(protect2(rm->name));
	if(printRoomsLongNames && !rm->longName.isEmpty())
		tl.append(protect2(rm->longName));
	if(printRoomsCodes && !rm->code.isEmpty())
		tl.append(protect2(rm->code));
	
	return tl.join(SETTINGS_TIMETABLES_SEPARATE_ROOMS_NAME_LONG_NAME_CODE_BY_BREAK?QString("<br />"):QString(" "));
}

QString TimetableExport::getDayOrRealDayString(int d, bool printDaysNames, bool printDaysLongNames)
{
	QStringList tl;

	QString dn = (gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.daysOfTheWeek[d] : gt.rules.realDaysOfTheWeek[d]);
	if(printDaysNames && !dn.isEmpty())
		tl.append(protect2(dn));
	QString dln = (gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.daysOfTheWeek_longNames[d] : gt.rules.realDaysOfTheWeek_longNames[d]);
	if(printDaysLongNames && !dln.isEmpty())
		tl.append(protect2(dln));

	return tl.join(SETTINGS_TIMETABLES_SEPARATE_DAYS_NAME_LONG_NAME_BY_BREAK?QString("<br />"):QString(" "));
}

QString TimetableExport::getDayString(int d, bool printDaysNames, bool printDaysLongNames)
{
	QStringList tl;

	QString dn = gt.rules.daysOfTheWeek[d];
	if(printDaysNames && !dn.isEmpty())
		tl.append(protect2(dn));
	QString dln = gt.rules.daysOfTheWeek_longNames[d];
	if(printDaysLongNames && !dln.isEmpty())
		tl.append(protect2(dln));

	return tl.join(SETTINGS_TIMETABLES_SEPARATE_DAYS_NAME_LONG_NAME_BY_BREAK?QString("<br />"):QString(" "));
}

QString TimetableExport::getRealDayString(int d, bool printDaysNames, bool printDaysLongNames)
{
	QStringList tl;

	QString dn = gt.rules.realDaysOfTheWeek[d];
	if(printDaysNames && !dn.isEmpty())
		tl.append(protect2(dn));
	QString dln = gt.rules.realDaysOfTheWeek_longNames[d];
	if(printDaysLongNames && !dln.isEmpty())
		tl.append(protect2(dln));

	return tl.join(SETTINGS_TIMETABLES_SEPARATE_DAYS_NAME_LONG_NAME_BY_BREAK?QString("<br />"):QString(" "));
}

QString TimetableExport::getHourOrRealHourString(int h, bool printHoursNames, bool printHoursLongNames)
{
	QStringList tl;

	QString hn = (gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.hoursOfTheDay[h] : gt.rules.realHoursOfTheDay[h]);
	if(printHoursNames && !hn.isEmpty())
		tl.append(protect2(hn));
	QString hln = (gt.rules.mode!=MORNINGS_AFTERNOONS ? gt.rules.hoursOfTheDay_longNames[h] : gt.rules.realHoursOfTheDay_longNames[h]);
	if(printHoursLongNames && !hln.isEmpty())
		tl.append(protect2(hln));

	return tl.join(SETTINGS_TIMETABLES_SEPARATE_HOURS_NAME_LONG_NAME_BY_BREAK?QString("<br />"):QString(" "));
}

QString TimetableExport::getHourString(int h, bool printHoursNames, bool printHoursLongNames)
{
	QStringList tl;

	QString hn = gt.rules.hoursOfTheDay[h];
	if(printHoursNames && !hn.isEmpty())
		tl.append(protect2(hn));
	QString hln = gt.rules.hoursOfTheDay_longNames[h];
	if(printHoursLongNames && !hln.isEmpty())
		tl.append(protect2(hln));

	return tl.join(SETTINGS_TIMETABLES_SEPARATE_HOURS_NAME_LONG_NAME_BY_BREAK?QString("<br />"):QString(" "));
}

QString TimetableExport::getRealHourString(int h, bool printHoursNames, bool printHoursLongNames)
{
	QStringList tl;

	QString hn = gt.rules.realHoursOfTheDay[h];
	if(printHoursNames && !hn.isEmpty())
		tl.append(protect2(hn));
	QString hln = gt.rules.realHoursOfTheDay_longNames[h];
	if(printHoursLongNames && !hln.isEmpty())
		tl.append(protect2(hln));

	return tl.join(SETTINGS_TIMETABLES_SEPARATE_HOURS_NAME_LONG_NAME_BY_BREAK?QString("<br />"):QString(" "));
}

QString TimetableExport::getVerticalDayString(int d, bool printDaysNames, bool printDaysLongNames)
{
	QStringList tl;

	QString dn = gt.rules.daysOfTheWeek[d];
	if(printDaysNames && !dn.isEmpty())
		tl.append(dn);
	QString dln = gt.rules.daysOfTheWeek_longNames[d];
	if(printDaysLongNames && !dln.isEmpty())
		tl.append(dln);

	return protect2vert(tl.join(QString(" ")));
}

QString TimetableExport::getVerticalRealDayString(int d, bool printDaysNames, bool printDaysLongNames)
{
	QStringList tl;

	QString dn = gt.rules.realDaysOfTheWeek[d];
	if(printDaysNames && !dn.isEmpty())
		tl.append(dn);
	QString dln = gt.rules.realDaysOfTheWeek_longNames[d];
	if(printDaysLongNames && !dln.isEmpty())
		tl.append(dln);

	return protect2vert(tl.join(QString(" ")));
}

void TimetableExport::addActivitiesLegend(const QList<int>& allActivitiesList, QSet<Subject*>& usedSubjectsSet, QSet<ActivityTag*>& usedActivityTagsSet, QSet<Teacher*>& usedTeachersSet, QSet<StudentsSet*>& usedStudentsSet, QSet<Room*>& usedRoomsSet,
 bool printSubjectsLegend, bool printActivityTagsLegend, bool printTeachersLegend, bool printStudentsLegend, bool printRoomsLegend)
{
	if(printSubjectsLegend){
		for(int ai : std::as_const(allActivitiesList)){
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				int sbji=gt.rules.subjectsHash.value(act->subjectName, -1);
				assert(sbji>=0);
				usedSubjectsSet.insert(gt.rules.internalSubjectsList[sbji]);
			}
		}
	}

	if(printActivityTagsLegend){
		for(int ai : std::as_const(allActivitiesList)){
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				for(const QString& at : act->activityTagsNames){
					int ati=gt.rules.activityTagsHash.value(at, -1);
					assert(ati>=0);
					usedActivityTagsSet.insert(gt.rules.internalActivityTagsList[ati]);
				}
			}
		}
	}

	if(printTeachersLegend){
		for(int ai : std::as_const(allActivitiesList)){
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				for(const QString& tch : act->teachersNames){
					int tchi=gt.rules.teachersHash.value(tch, -1);
					assert(tchi>=0);
					usedTeachersSet.insert(gt.rules.internalTeachersList[tchi]);
				}
			}
		}
	}

	if(printStudentsLegend){
		for(int ai : std::as_const(allActivitiesList)){
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				for(const QString& st : act->studentsNames){
					StudentsSet* stp=gt.rules.studentsHash.value(st, nullptr);
					assert(stp!=nullptr);
					usedStudentsSet.insert(stp);
				}
			}
		}
	}

	if(printRoomsLegend){
		for(int ai : std::as_const(allActivitiesList)){
			if(ai!=UNALLOCATED_ACTIVITY){
				int r=best_solution.rooms[ai];
				if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
					usedRoomsSet.insert(gt.rules.internalRoomsList[r]);
					if(gt.rules.internalRoomsList[r]->isVirtual==true)
						for(int rr : std::as_const(best_solution.realRoomsList[ai]))
							usedRoomsSet.insert(gt.rules.internalRoomsList[rr]);
				}
			}
		}
	}
}

void TimetableExport::printLegend(QString& tmpString, QSet<Subject*>& usedSubjectsSet, QSet<ActivityTag*>& usedActivityTagsSet, QSet<Teacher*>& usedTeachersSet, QSet<StudentsSet*>& usedStudentsSet, QSet<Room*>& usedRoomsSet,
 bool printSubjectsLegend, bool printActivityTagsLegend, bool printTeachersLegend, bool printStudentsLegend, bool printRoomsLegend,
 bool printCodesFirst)
{
	QString indent=QString(4, ' ');
	
	if(!printCodesFirst){
		if(printSubjectsLegend && !usedSubjectsSet.isEmpty()){
			QList<Subject*> usedSubjectsList=QList<Subject*>(usedSubjectsSet.constBegin(), usedSubjectsSet.constEnd());
			std::stable_sort(usedSubjectsList.begin(), usedSubjectsList.end(), subjectsAscending);
			tmpString+=indent+"<p>\n";
			tmpString+=indent+"  <span class=\"legend_title\">"+protect2(tr("Subjects legend:", "It means the list of the complete information for the subjects: name, long name, code, and comments"))+"</span><br />\n";

			bool first=true;
			for(Subject* sbj : std::as_const(usedSubjectsList)){
				if(!first)
					tmpString+="<br />\n";
				else
					first=false;

				tmpString+=indent+"  &nbsp;&nbsp;&nbsp;&nbsp; "+protect2(sbj->name);
				if(!sbj->longName.isEmpty())
					tmpString+=QString(" - ")+protect2(sbj->longName);
				if(!sbj->code.isEmpty())
					tmpString+=QString(" - ")+protect2(sbj->code);
				if(!sbj->comments.isEmpty())
					tmpString+=QString(" - ")+protect2(sbj->comments).replace("\n", "<br />\n");
			}

			tmpString+="\n"+indent+"</p>\n";
		}

		if(printActivityTagsLegend && !usedActivityTagsSet.isEmpty()){
			QList<ActivityTag*> usedActivityTagsList=QList<ActivityTag*>(usedActivityTagsSet.constBegin(), usedActivityTagsSet.constEnd());
			std::stable_sort(usedActivityTagsList.begin(), usedActivityTagsList.end(), activityTagsAscending);
			tmpString+=indent+"<p>\n";
			tmpString+=indent+"  <span class=\"legend_title\">"+protect2(tr("Activity tags legend:", "It means the list of the complete information for the activity tag: name, long name, code, and comments"))+"</span><br />\n";

			bool first=true;
			for(ActivityTag* at : std::as_const(usedActivityTagsList)){
				if(!first)
					tmpString+="<br />\n";
				else
					first=false;

				tmpString+=indent+"  &nbsp;&nbsp;&nbsp;&nbsp; "+protect2(at->name);
				if(!at->longName.isEmpty())
					tmpString+=QString(" - ")+protect2(at->longName);
				if(!at->code.isEmpty())
					tmpString+=QString(" - ")+protect2(at->code);
				if(!at->comments.isEmpty())
					tmpString+=QString(" - ")+protect2(at->comments).replace("\n", "<br />\n");
			}

			tmpString+="\n"+indent+"</p>\n";
		}

		if(printTeachersLegend && !usedTeachersSet.isEmpty()){
			QList<Teacher*> usedTeachersList=QList<Teacher*>(usedTeachersSet.constBegin(), usedTeachersSet.constEnd());
			std::stable_sort(usedTeachersList.begin(), usedTeachersList.end(), teachersAscending);
			tmpString+=indent+"<p>\n";
			tmpString+=indent+"  <span class=\"legend_title\">"+protect2(tr("Teachers legend:", "It means the list of the complete information for the teachers: name, long name, code, and comments"))+"</span><br />\n";

			bool first=true;
			for(Teacher* tch : std::as_const(usedTeachersList)){
				if(!first)
					tmpString+="<br />\n";
				else
					first=false;

				tmpString+=indent+"  &nbsp;&nbsp;&nbsp;&nbsp; "+protect2(tch->name);
				if(!tch->longName.isEmpty())
					tmpString+=QString(" - ")+protect2(tch->longName);
				if(!tch->code.isEmpty())
					tmpString+=QString(" - ")+protect2(tch->code);
				if(!tch->comments.isEmpty())
					tmpString+=QString(" - ")+protect2(tch->comments).replace("\n", "<br />\n");
			}

			tmpString+="\n"+indent+"</p>\n";
		}

		if(printStudentsLegend && !usedStudentsSet.isEmpty()){
			QList<StudentsSet*> usedStudentsList=QList<StudentsSet*>(usedStudentsSet.constBegin(), usedStudentsSet.constEnd());
			std::stable_sort(usedStudentsList.begin(), usedStudentsList.end(), studentsSetsAscending);
			tmpString+=indent+"<p>\n";
			tmpString+=indent+"  <span class=\"legend_title\">"+protect2(tr("Students legend:", "It means the list of the complete information for the students: name, long name, code, and comments"))+"</span><br />\n";

			bool first=true;
			for(StudentsSet* st : std::as_const(usedStudentsList)){
				if(!first)
					tmpString+="<br />\n";
				else
					first=false;

				tmpString+=indent+"  &nbsp;&nbsp;&nbsp;&nbsp; "+protect2(st->name);
				if(!st->longName.isEmpty())
					tmpString+=QString(" - ")+protect2(st->longName);
				if(!st->code.isEmpty())
					tmpString+=QString(" - ")+protect2(st->code);
				if(!st->comments.isEmpty())
					tmpString+=QString(" - ")+protect2(st->comments).replace("\n", "<br />\n");
			}

			tmpString+="\n"+indent+"</p>\n";
		}

		if(printRoomsLegend && !usedRoomsSet.isEmpty()){
			QList<Room*> usedRoomsList=QList<Room*>(usedRoomsSet.constBegin(), usedRoomsSet.constEnd());
			std::stable_sort(usedRoomsList.begin(), usedRoomsList.end(), roomsAscending);
			tmpString+=indent+"<p>\n";
			tmpString+=indent+"  <span class=\"legend_title\">"+protect2(tr("Rooms legend:", "It means the list of the complete information for the rooms: name, long name, code, and comments"))+"</span><br />\n";

			bool first=true;
			for(Room* rm : std::as_const(usedRoomsList)){
				if(!first)
					tmpString+="<br />\n";
				else
					first=false;

				tmpString+=indent+"  &nbsp;&nbsp;&nbsp;&nbsp; "+protect2(rm->name);
				if(!rm->longName.isEmpty())
					tmpString+=QString(" - ")+protect2(rm->longName);
				if(!rm->code.isEmpty())
					tmpString+=QString(" - ")+protect2(rm->code);
				if(!rm->comments.isEmpty())
					tmpString+=QString(" - ")+protect2(rm->comments).replace("\n", "<br />\n");
			}

			tmpString+="\n"+indent+"</p>\n";
		}
	}
	else{ //if print codes first
		if(printSubjectsLegend && !usedSubjectsSet.isEmpty()){
			QList<Subject*> usedSubjectsList=QList<Subject*>(usedSubjectsSet.constBegin(), usedSubjectsSet.constEnd());
			std::stable_sort(usedSubjectsList.begin(), usedSubjectsList.end(), subjectsCodesAscending);
			tmpString+=indent+"<p>\n";
			tmpString+=indent+"  <span class=\"legend_title\">"+protect2(tr("Subjects legend:", "It means the list of the complete information for the subjects: name, long name, code, and comments"))+"</span><br />\n";

			bool first=true;
			for(Subject* sbj : std::as_const(usedSubjectsList)){
				if(!first)
					tmpString+="<br />\n";
				else
					first=false;

				tmpString+=indent+"  &nbsp;&nbsp;&nbsp;&nbsp; ";
				if(!sbj->code.isEmpty())
					tmpString+=protect2(sbj->code)+QString(" - ");
				tmpString+=protect2(sbj->name);
				if(!sbj->longName.isEmpty())
					tmpString+=QString(" - ")+protect2(sbj->longName);
				if(!sbj->comments.isEmpty())
					tmpString+=QString(" - ")+protect2(sbj->comments).replace("\n", "<br />\n");
			}

			tmpString+="\n"+indent+"</p>\n";
		}

		if(printActivityTagsLegend && !usedActivityTagsSet.isEmpty()){
			QList<ActivityTag*> usedActivityTagsList=QList<ActivityTag*>(usedActivityTagsSet.constBegin(), usedActivityTagsSet.constEnd());
			std::stable_sort(usedActivityTagsList.begin(), usedActivityTagsList.end(), activityTagsCodesAscending);
			tmpString+=indent+"<p>\n";
			tmpString+=indent+"  <span class=\"legend_title\">"+protect2(tr("Activity tags legend:", "It means the list of the complete information for the activity tag: name, long name, code, and comments"))+"</span><br />\n";

			bool first=true;
			for(ActivityTag* at : std::as_const(usedActivityTagsList)){
				if(!first)
					tmpString+="<br />\n";
				else
					first=false;

				tmpString+=indent+"  &nbsp;&nbsp;&nbsp;&nbsp; ";
				if(!at->code.isEmpty())
					tmpString+=protect2(at->code)+QString(" - ");
				tmpString+=protect2(at->name);
				if(!at->longName.isEmpty())
					tmpString+=QString(" - ")+protect2(at->longName);
				if(!at->comments.isEmpty())
					tmpString+=QString(" - ")+protect2(at->comments).replace("\n", "<br />\n");
			}

			tmpString+="\n"+indent+"</p>\n";
		}

		if(printTeachersLegend && !usedTeachersSet.isEmpty()){
			QList<Teacher*> usedTeachersList=QList<Teacher*>(usedTeachersSet.constBegin(), usedTeachersSet.constEnd());
			std::stable_sort(usedTeachersList.begin(), usedTeachersList.end(), teachersCodesAscending);
			tmpString+=indent+"<p>\n";
			tmpString+=indent+"  <span class=\"legend_title\">"+protect2(tr("Teachers legend:", "It means the list of the complete information for the teachers: name, long name, code, and comments"))+"</span><br />\n";

			bool first=true;
			for(Teacher* tch : std::as_const(usedTeachersList)){
				if(!first)
					tmpString+="<br />\n";
				else
					first=false;

				tmpString+=indent+"  &nbsp;&nbsp;&nbsp;&nbsp; ";
				if(!tch->code.isEmpty())
					tmpString+=protect2(tch->code)+QString(" - ");
				tmpString+=protect2(tch->name);
				if(!tch->longName.isEmpty())
					tmpString+=QString(" - ")+protect2(tch->longName);
				if(!tch->comments.isEmpty())
					tmpString+=QString(" - ")+protect2(tch->comments).replace("\n", "<br />\n");
			}

			tmpString+="\n"+indent+"</p>\n";
		}

		if(printStudentsLegend && !usedStudentsSet.isEmpty()){
			QList<StudentsSet*> usedStudentsList=QList<StudentsSet*>(usedStudentsSet.constBegin(), usedStudentsSet.constEnd());
			std::stable_sort(usedStudentsList.begin(), usedStudentsList.end(), studentsSetsCodesAscending);
			tmpString+=indent+"<p>\n";
			tmpString+=indent+"  <span class=\"legend_title\">"+protect2(tr("Students legend:", "It means the list of the complete information for the students: name, long name, code, and comments"))+"</span><br />\n";

			bool first=true;
			for(StudentsSet* st : std::as_const(usedStudentsList)){
				if(!first)
					tmpString+="<br />\n";
				else
					first=false;

				tmpString+=indent+"  &nbsp;&nbsp;&nbsp;&nbsp; ";
				if(!st->code.isEmpty())
					tmpString+=protect2(st->code)+QString(" - ");
				tmpString+=protect2(st->name);
				if(!st->longName.isEmpty())
					tmpString+=QString(" - ")+protect2(st->longName);
				if(!st->comments.isEmpty())
					tmpString+=QString(" - ")+protect2(st->comments).replace("\n", "<br />\n");
			}

			tmpString+="\n"+indent+"</p>\n";
		}

		if(printRoomsLegend && !usedRoomsSet.isEmpty()){
			QList<Room*> usedRoomsList=QList<Room*>(usedRoomsSet.constBegin(), usedRoomsSet.constEnd());
			std::stable_sort(usedRoomsList.begin(), usedRoomsList.end(), roomsCodesAscending);
			tmpString+=indent+"<p>\n";
			tmpString+=indent+"  <span class=\"legend_title\">"+protect2(tr("Rooms legend:", "It means the list of the complete information for the rooms: name, long name, code, and comments"))+"</span><br />\n";

			bool first=true;
			for(Room* rm : std::as_const(usedRoomsList)){
				if(!first)
					tmpString+="<br />\n";
				else
					first=false;

				tmpString+=indent+"  &nbsp;&nbsp;&nbsp;&nbsp; ";
				if(!rm->code.isEmpty())
					tmpString+=protect2(rm->code)+QString(" - ");
				tmpString+=protect2(rm->name);
				if(!rm->longName.isEmpty())
					tmpString+=QString(" - ")+protect2(rm->longName);
				if(!rm->comments.isEmpty())
					tmpString+=QString(" - ")+protect2(rm->comments).replace("\n", "<br />\n");
			}

			tmpString+="\n"+indent+"</p>\n";
		}
	}
}

QString TimetableExport::printCompleteLegend(bool printSubjectsLegend, bool printActivityTagsLegend, bool printTeachersLegend, bool printStudentsLegend, bool printRoomsLegend,
	bool printCodesFirst)
{
	QSet<Subject*> allSubjects;
	QSet<ActivityTag*> allActivityTags;
	QSet<Teacher*> allTeachers;
	QSet<StudentsSet*> allStudents;
	QSet<Room*> allRooms;
	
	QList<int> allActivitiesList;

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++)
		allActivitiesList.append(ai);
	
	QString str;
	
	addActivitiesLegend(allActivitiesList, allSubjects, allActivityTags, allTeachers, allStudents, allRooms,
	 printSubjectsLegend, printActivityTagsLegend, printTeachersLegend, printStudentsLegend, printRoomsLegend);
	printLegend(str, allSubjects, allActivityTags, allTeachers, allStudents, allRooms,
	 printSubjectsLegend, printActivityTagsLegend, printTeachersLegend, printStudentsLegend, printRoomsLegend,
	 printCodesFirst);
	
	return str;
}
