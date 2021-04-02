/***************************************************************************
                          timetable_defs.h  -  description
                             -------------------
    begin                : Sat Mar 15 2003
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


#ifndef TIMETABLE_DEFS
#define TIMETABLE_DEFS
//#define WIN32
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>

/*#ifdef WIN32
#define for		if(0);else for
#endif*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#include <qstring.h>
#include <qtranslator.h>
#include <qdir.h>

/**
The version number
*/
extern const QString FET_VERSION;

/**
The language
*/
extern QString FET_LANGUAGE;

//English has to be counted also
extern const int NUMBER_OF_LANGUAGES;

extern bool LANGUAGE_STYLE_RIGHT_TO_LEFT;

extern QString LANGUAGE_FOR_HTML;

/**
Timetable html css javaScript Level, added by Volker Dirr
*/
extern int TIMETABLE_HTML_LEVEL;

extern bool PRINT_NOT_AVAILABLE_TIME_SLOTS;

extern bool DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS;

/**
The maximum number of different years of students
*/
//const int MAX_YEARS=15000;

/**
Maximum number of allowed groups per year of students
*/
//const int MAX_GROUPS_PER_YEAR=15000;

/**
Maximum number of allowed subgroups per group of students
*/
//const int MAX_SUBGROUPS_PER_GROUP=15000;

/**
The maximum total number of different groups of students
*/
//const int MAX_TOTAL_GROUPS=15000;

/**
The maximum total number of different subgroups of students
*/
const int MAX_TOTAL_SUBGROUPS=15000;//MAX_YEARS*MAX_GROUPS_PER_YEAR*MAX_SUBGROUPS_PER_GROUP;

const int MAX_ROOM_CAPACITY=15000;


/**
The maximum number of different teachers
*/
const int MAX_TEACHERS=700;

/**
The maximum number of different subjects
*/
const int MAX_SUBJECTS=1000;

/**
The maximum number of activities
*/
const int MAX_ACTIVITIES=5000;

/**
The maximum number of rooms
*/
const int MAX_ROOMS=1000;

/**
The maximum number of buildings
*/
const int MAX_BUILDINGS=100;

/**
This constant represents an unallocated activity
*/
const qint16 UNALLOCATED_ACTIVITY = MAX_ACTIVITIES;

/**
The maximum number of working hours per day.
*/
const int MAX_HOURS_PER_DAY=60;

/**
The maximum number of working days per week.
*/
const int MAX_DAYS_PER_WEEK=28;

/**
The predefined names of the days of the week
*/
const QString PREDEFINED_DAYS_OF_THE_WEEK[]={"Monday", "Tuesday", "Wednesday",
	"Thursday", "Friday", "Saturday", "Sunday", "Monday2",
	"Tuesday2", "Wednesday2", "Thursday2", "Friday2", "Saturday2", "Sunday2",
	"Monday3", "Tuesday3", "Wednesday3",
	"Thursday3", "Friday3", "Saturday3", "Sunday3", "Monday4",
	"Tuesday4", "Wednesday4", "Thursday4", "Friday4", "Saturday4", "Sunday4"};

/**
The maximum number of working hours in a week.
<p>
hours in a week are arranged like this:
           Mo Tu We Th Fr
0-1        0  1  2  3  4
1-2        5  6  7  8  9
2-3        10 11 12 13 14
3-4        15 16 17 18 19
4-5        20 21 22 23 24
5-6        25 26 27 28 29 etc.
*/
const int MAX_HOURS_PER_WEEK = MAX_HOURS_PER_DAY * MAX_DAYS_PER_WEEK;

/**
This constant represents unallocated time for an activity
*/
const qint16 UNALLOCATED_TIME = MAX_HOURS_PER_WEEK;

/**
This constant represents unallocated space for an activity
*/
const qint16 UNALLOCATED_SPACE = MAX_ROOMS;

const qint16 UNSPECIFIED_ROOM = MAX_ROOMS+1;


/**
The maximum number of time constraints
*/
const int MAX_TIME_CONSTRAINTS = 10000;

/**
The maximum number of space constraints
*/
const int MAX_SPACE_CONSTRAINTS = 10000;

/**
The maximum number of preferred times that can be considered
in this kind of constraint
*/
const int MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS = MAX_HOURS_PER_WEEK;

/**
The maximum number of preferred times that can be considered
in this kind of constraint
*/
const int MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS = MAX_HOURS_PER_WEEK;

const int MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES = MAX_HOURS_PER_WEEK;
const int MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES = MAX_HOURS_PER_WEEK;

const int MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS = MAX_HOURS_PER_WEEK;
const int MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES = MAX_HOURS_PER_WEEK;

/**
The maximum number of activities that can be put in
a constraint of type ConstraintActivitiesMinNDaysBetweenActivities
I guess this variable must disappear and the
restriction modified to allocate dynamically the
necessary memory.
*/
const int MAX_CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES = 100;

/**
The maximum number of activities for a single
constraint of type
ConstraintActivitiesSameStartingTime
*/
const int MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME=100;

/**
The maximum number of activities for a single
constraint of type
ConstraintActivitiesSameStartingHour
*/
const int MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR=100;

/**
The maximum number of activities for a single
constraint of type
ConstraintActivitiesSameStartingHour
*/
const int MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY=100;

/**
The maximum number of activities for a single
constraint of type
ConstraintActivitiesNotOverlapping
*/
const int MAX_CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING=100;


/**
File and directory separator
*/
const QString FILE_SEP="/";


/**
The XML tag used for identification of the input file (old)
*/
const QString INPUT_FILE_TAG_3_6_1="FET_VERSION_3_6_1_AND_ABOVE_TIMETABLE_DATA_FILE";

/**
The XML tag used for identification of the output file
"Subgroup's Timetable"
*/
const QString STUDENTS_TIMETABLE_TAG="Students_Timetable";

/**
The XML tag used for identification of the output file
"Teacher's Timetable"
*/
const QString TEACHERS_TIMETABLE_TAG="Teachers_Timetable";

const QString ACTIVITIES_TIMETABLE_TAG="Activities_Timetable";

/**
The XML tag used for identification of the output file
"Rooms's Timetable"
*/
const QString ROOMS_TIMETABLE_TAG="Rooms_Timetable";

/**
The timetable's rules input file name
*/
extern QString INPUT_FILENAME_XML;

/**
The working directory
*/
extern QString WORKING_DIRECTORY;

/**
The import directory
*/
extern QString IMPORT_DIRECTORY;

/**
The initialization file for timetable program
*/
/*#ifdef WIN32
const QString INI_FILENAME="fet.ini";
#else
const QString INI_FILENAME=QDir::homeDirPath()+"/.fet/fet.ini";
#endif*/

//OUTPUT FILES

/**
The output directory. Please be careful when editing it,
because the functions add a FILE_SEP sign at the end of it
and then the name of a file. If you make OUTPUT_DIR="",
there will be problems.
*/
extern const QString OUTPUT_DIR;
/*#ifdef Q_OS_WIN
const QString OUTPUT_DIR="results";
#else
const QString OUTPUT_DIR=QDir::homeDirPath()+"/fet-results";
#endif*/

/**
An output file containing the timetable for each subgroup,
arranged in xml format
*/
const QString SUBGROUPS_TIMETABLE_FILENAME_XML="subgroups_timetable.xml";

/**
An output file containing the timetable for each teacher,
arranged in xml format
*/
const QString TEACHERS_TIMETABLE_FILENAME_XML="teachers_timetable.xml";

const QString ACTIVITIES_TIMETABLE_FILENAME_XML="activities_timetable.xml";

/**
An output file containing the timetable for each room,
arranged in xml format
*/
const QString ROOMS_TIMETABLE_FILENAME_XML="rooms_timetable.xml";

/**
An output file containing the css stylesheet for each html-file
*/
const QString STYLESHEET_CSS="stylesheet.css";

/**
An output file containing the index html file
*/
const QString INDEX_HTML="index.html";

/**
An output file containing the timetable for each subgroup, arranged in html format.
Days horizontal version.
*/
const QString SUBGROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="subgroups_timetable_days_horizontal.html";

/**
An output file containing the timetable for each subgroup, arranged in html format.
Days vertical version.
*/
const QString SUBGROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="subgroups_timetable_days_vertical.html";

/**
An output file containing the timetable for each subgroup, arranged in html format.
Time horizontal version.
*/
const QString SUBGROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="subgroups_timetable_time_horizontal.html";

/**
An output file containing the timetable for each subgroup, arranged in html format.
Time vertical version.
*/
const QString SUBGROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="subgroups_timetable_time_vertical.html";

/**
An output file containing the timetable for each group, arranged in html format.
Days horizontal version.
*/
const QString GROUPS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="groups_timetable_days_horizontal.html";

/**
An output file containing the timetable for each group, arranged in html format.
Days vertical version.
*/
const QString GROUPS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="groups_timetable_days_vertical.html";

/**
An output file containing the timetable for each group, arranged in html format.
Time horizontal version.
*/
const QString GROUPS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="groups_timetable_time_horizontal.html";

/**
An output file containing the timetable for each group, arranged in html format.
Time vertical version.
*/
const QString GROUPS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="groups_timetable_time_vertical.html";

/**
An output file containing the timetable for each year, arranged in html format.
Days horizontal version.
*/
const QString YEARS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="years_timetable_days_horizontal.html";

/**
An output file containing the timetable for each year, arranged in html format.
Days vertical version.
*/
const QString YEARS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="years_timetable_days_vertical.html";

/**
An output file containing the timetable for each year, arranged in html format.
Time horizontal version.
*/
const QString YEARS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="years_timetable_time_horizontal.html";

/**
An output file containing the timetable for each year, arranged in html format.
Time vertical version.
*/
const QString YEARS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="years_timetable_time_vertical.html";

/**
An output file containing the timetable for each teacher, arranged in html format.
Days horizontal version.
*/
const QString TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="teachers_timetable_days_horizontal.html";

/**
An output file containing the timetable for each teacher, arranged in html format.
Days vertical version.
*/
const QString TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="teachers_timetable_days_vertical.html";

/**
An output file containing the timetable for each teacher, arranged in html format.
Time horizontal version.
*/
const QString TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="teachers_timetable_time_horizontal.html";

/**
An output file containing the timetable for each teacher, arranged in html format.
Time vertical version.
*/
const QString TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="teachers_timetable_time_vertical.html";

/**
An output file containing the timetable for each room, arranged in html format.
Days horizontal version.
*/
const QString ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="rooms_timetable_days_horizontal.html";

/**
An output file containing the timetable for each room, arranged in html format.
Days vertical version.
*/
const QString ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="rooms_timetable_days_vertical.html";

/**
An output file containing the timetable for each room, arranged in html format.
Time horizontal version.
*/
const QString ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="rooms_timetable_time_horizontal.html";

/**
An output file containing the timetable for each room, arranged in html format.
Time vertical version.
*/
const QString ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="rooms_timetable_time_vertical.html";

/**
An output file containing the timetable for each subject, arranged in html format.
Days horizontal version.
*/
const QString SUBJECTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="subjects_timetable_days_horizontal.html";

/**
An output file containing the timetable for each subject, arranged in html format.
Days vertical version.
*/
const QString SUBJECTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="subjects_timetable_days_vertical.html";

/**
An output file containing the timetable for each subject, arranged in html format.
Time horizontal version.
*/
const QString SUBJECTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="subjects_timetable_time_horizontal.html";

/**
An output file containing the timetable for each subject, arranged in html format.
Time vertical version.
*/
const QString SUBJECTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="subjects_timetable_time_vertical.html";

/**
An output file containing the timetable for all activities, arranged in html format.
Days horizontal version.
*/
const QString ALL_ACTIVITIES_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="all_activities_timetable_days_horizontal.html";

/**
An output file containing the timetable for all activities, arranged in html format.
Days vertical version.
*/
const QString ALL_ACTIVITIES_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="all_activities_timetable_days_vertical.html";


/**
An output file containing the timetable for all activities, arranged in html format.
Time horizontal version.
*/
const QString ALL_ACTIVITIES_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="all_activities_timetable_time_horizontal.html";

/**
An output file containing the timetable for all activities, arranged in html format.
Time vertical version.
*/
const QString ALL_ACTIVITIES_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="all_activities_timetable_time_vertical.html";



/**
A log file explaining how the xml input file was parsed
*/
const QString XML_PARSING_LOG_FILENAME="xml_reading_log.txt";

/**
A file containing the time conflicts
*/
const QString CONFLICTS_FILENAME="soft_conflicts.txt";


/**
A function used in xml saving
*/
QString protect(const QString& str);

/**
A function used in html saving
*/
QString protect2(const QString& str);

/**
A function used in html saving
*/
QString protect2vert(const QString& str);

/**
A function used in html saving
*/
QString protect2id(const QString& str);

/**
A function used in html saving
*/
QString protect2java(const QString& str);

/**
A function used in iCal saving
*/
QString protect3(const QString& str);

/**
This constants represents free periods of a teacher in the teachers free periods timetable
*/
const qint16 TEACHER_HAS_SINGLE_GAP =0;
const qint16 TEACHER_HAS_BORDER_GAP =1;
const qint16 TEACHER_HAS_BIG_GAP =2;

const qint16 TEACHER_MUST_COME_EARLIER =4;
const qint16 TEACHER_MUST_COME_MUCH_EARLIER =6;

const qint16 TEACHER_MUST_STAY_LONGER =3;
const qint16 TEACHER_MUST_STAY_MUCH_LONGER =5;		// BE CAREFULL, I just print into LESS_DETAILED timetable, if it's smaller then TEACHER_MUST_STAY_MUCH_LONGER

const qint16 TEACHER_HAS_A_FREE_DAY =7;

const qint16 TEACHER_IS_NOT_AVAILABLE =8;

const int TEACHERS_FREE_PERIODS_N_CATEGORIES=9;

/**
An output file containing the timetable for free periods of teachers, arranged in html format.
Days horizontal version.
*/
const QString TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="teachers_free_periods_timetable_days_horizontal.html";

/**
An output file containing the timetable for free periods of teachers, arranged in html format.
Days vertical version.
*/
const QString TEACHERS_FREE_PERIODS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="teachers_free_periods_timetable_days_vertical.html";

const QString MULTIPLE_TIMETABLE_DATA_RESULTS_FILE="data_and_timetable.fet";


//functions below are used in iCal exporting functions
bool isLeapYear(int year);

bool isCorrectDay(const QString day);

bool isCorrectHour(const QString hour);

QString nextDay(const QString day);

bool sumHours(const QString hour1, const QString hour2, QString& result);

QString iCalFolding(const QString s);

extern int checkForUpdates;

extern QString internetVersion;

//class Widget;
void centerWidgetOnScreen(QWidget* widget);


//for random Knuth - from Knuth TAOCP Vol. 2 Seminumerical Algorithms section 3.6
//these numbers are really important - please do not change them, NEVER!!!
//if you want, write a new random number generator routine, with other name
//extern int X;
const int MM=2147483647;
const int AA=48271;
const int QQ=44488;
const int RR=3399;
void initRandomKnuth();
int randomKnuth();
	

#endif
