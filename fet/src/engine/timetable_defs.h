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

#ifdef WIN32
#define for		if(0);else for
#endif

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

/**
The maximum number of different years of students
*/
const int MAX_YEARS=100;

/**
Maximum number of allowed groups per year of students
*/
const int MAX_GROUPS_PER_YEAR=100;

/**
Maximum number of allowed subgroups per group of students
*/
const int MAX_SUBGROUPS_PER_GROUP=100;

/**
The maximum total number of different groups of students
*/
const int MAX_TOTAL_GROUPS=MAX_YEARS*MAX_GROUPS_PER_YEAR;

/**
The maximum total number of different subgroups of students
*/
const int MAX_TOTAL_SUBGROUPS=5000;//MAX_YEARS*MAX_GROUPS_PER_YEAR*MAX_SUBGROUPS_PER_GROUP;

/**
This constant represents the maximum number of total allowed subgroups
for any activity.
*/
const int MAX_SUBGROUPS_PER_ACTIVITY=400;//MAX_SUBGROUPS_PER_GROUP * MAX_GROUPS_PER_YEAR;

/**
This constant represents the maximum number of teachers
for any activity.
*/
const int MAX_TEACHERS_PER_ACTIVITY = 100;

const int MAX_SUBGROUPS_PER_CONSTRAINT=400;//MAX_SUBGROUPS_PER_GROUP * MAX_GROUPS_PER_YEAR;

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
const int MAX_ACTIVITIES=2000;

/**
The maximum number of rooms
*/
const int MAX_ROOMS=300;

/**
This constant represents an unallocated activity
*/
const qint16 UNALLOCATED_ACTIVITY = MAX_ACTIVITIES;

/**
The maximum number of working hours per day.
*/
const int MAX_HOURS_PER_DAY=30;

/**
The maximum number of working days per week.
*/
const int MAX_DAYS_PER_WEEK=14;

/**
The predefined names of the days of the week
*/
const QString PREDEFINED_DAYS_OF_THE_WEEK[]={"Monday", "Tuesday", "Wednesday",
	"Thursday", "Friday", "Saturday", "Sunday", "Monday2",
	"Tuesday2", "Wednesday2", "Thursday2", "Friday2", "Saturday2", "Sunday2"};

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
const int MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_TIMES = 100;

/**
The maximum number of preferred times that can be considered
in this kind of constraint
*/
const int MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIMES = 100;

/**
The maximum number of activities that can be put in
a constraint of type ConstraintActivitiesMinNDaysBetweenActivities
I guess this variable must disappear and the
restriction modified to allocate dynamically the
necessary memory.
*/
const int MAX_CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES = 20;

/**
The maximum number of activities for a single
constraint of type
ConstraintActivitiesSameStartingTime
*/
const int MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME=50;

/**
The maximum number of activities for a single
constraint of type
ConstraintActivitiesSameStartingHour
*/
const int MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR=50;

/**
The maximum number of activities for a single
constraint of type
ConstraintActivitiesNotOverlapping
*/
const int MAX_CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING=100;

/**
The maximum number of rooms for a single
constraint of type
ConstraintActivityPreferredRooms
*/
const int MAX_CONSTRAINT_ACTIVITY_PREFERRED_ROOMS=50;

/**
The maximum number of rooms for a single
constraint of type
ConstraintSubjectPreferredRooms
*/
const int MAX_CONSTRAINT_SUBJECT_PREFERRED_ROOMS=50;

/**
The maximum number of rooms for a single
constraint of type
ConstraintSubjectSubjectTagPreferredRooms
*/
const int MAX_CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS=50;

/**
The maximum number of activities for a single
constraint of type
ConstraintActivitiesSameRoom
*/
const int MAX_CONSTRAINT_ACTIVITIES_SAME_ROOM=20;

/**
The maximum number of activities which share the same subject name.
*/
const int MAX_ACTIVITIES_FOR_A_SUBJECT=200;

/**
The maximum number of activities which share the same subject tag name.
*/
const int MAX_ACTIVITIES_FOR_A_SUBJECT_TAG=1000;

/**
The maximum number of activities which share the same subject+subject tag name.
*/
const int MAX_ACTIVITIES_FOR_A_SUBJECT_SUBJECT_TAG=200;

/**
The maximum number of activities which share the same teacher name.
This constant is used (for now) in ConstraintTeacherRequiresRoom.
*/
const int MAX_ACTIVITIES_FOR_A_TEACHER=200;


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
#ifdef WIN32
const QString OUTPUT_DIR="results";
#else
const QString OUTPUT_DIR=QDir::homeDirPath()+"/fet-results";
#endif

/**
An output file containing the timetable for each subgroup,
arranged in xml format
*/
const QString STUDENTS_TIMETABLE_FILENAME_XML="studentstimetable.xml";

/**
An output file containing the timetable for each teacher,
arranged in xml format
*/
const QString TEACHERS_TIMETABLE_FILENAME_XML="teacherstimetable.xml";

/**
An output file containing the timetable for each room,
arranged in xml format
*/
const QString ROOMS_TIMETABLE_FILENAME_XML="roomstimetable.xml";

/**
An output file containing the css stylesheet for each html-file
*/
const QString STYLESHEET_CSS="stylesheet.css";

/**
An output file containing the timetable for each subgroup, arranged in html format.
Days horizontal version.
*/
const QString STUDENTS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="studentstimetabledayshorizontal.html";

/**
An output file containing the timetable for each subgroup, arranged in html format.
Days vertical version.
*/
const QString STUDENTS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="studentstimetabledaysvertical.html";

/**
An output file containing the timetable for each subgroup, arranged in html format.
Time horizontal version.
*/
const QString STUDENTS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="studentstimetabletimehorizontal.html";

/**
An output file containing the timetable for each subgroup, arranged in html format.
Time vertical version.
*/
const QString STUDENTS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="studentstimetabletimevertical.html";

/**
An output file containing the timetable for each teacher, arranged in html format.
Days horizontal version.
*/
const QString TEACHERS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="teacherstimetabledayshorizontal.html";

/**
An output file containing the timetable for each teacher, arranged in html format.
Days vertical version.
*/
const QString TEACHERS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="teacherstimetabledaysvertical.html";

/**
An output file containing the timetable for each teacher, arranged in html format.
Time horizontal version.
*/
const QString TEACHERS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="teacherstimetabletimehorzontal.html";

/**
An output file containing the timetable for each teacher, arranged in html format.
Time vertical version.
*/
const QString TEACHERS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="teacherstimetabletimevertical.html";

/**
An output file containing the timetable for each room,
arranged in html format.
Days horizontal version.
*/
const QString ROOMS_TIMETABLE_DAYS_HORIZONTAL_FILENAME_HTML="roomstimetabledayshorizontal.html";

/**
An output file containing the timetable for each room,
arranged in html format.
Days vertical version.
*/
const QString ROOMS_TIMETABLE_DAYS_VERTICAL_FILENAME_HTML="roomstimetabledaysvertical.html";

/**
An output file containing the timetable for each room,
arranged in html format.
Time horizontal version.
*/
const QString ROOMS_TIMETABLE_TIME_HORIZONTAL_FILENAME_HTML="roomstimetabletimehorizontal.html";

/**
An output file containing the timetable for each room,
arranged in html format.
Time vertical version.
*/
const QString ROOMS_TIMETABLE_TIME_VERTICAL_FILENAME_HTML="roomstimetabletimevertical.html";

/**
A log file explaining how the xml input file was parsed
*/
const QString XML_PARSING_LOG_FILENAME="xmlreadinglog.txt";

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
A function used in iCal saving
*/
QString protect3(const QString& str);

//functions below are used in iCal exporting functions
bool isLeapYear(int year);

bool isCorrectDay(const QString day);

bool isCorrectHour(const QString hour);

QString nextDay(const QString day);

bool sumHours(const QString hour1, const QString hour2, QString& result);

QString iCalFolding(const QString s);

extern int checkForUpdates;

extern QString internetVersion;


//for random Knuth - from Knuth TAOCP Vol. 2 Seminumerical Algorithms section 3.6
//these numbers are really important - please do not change them, NEVER!!!
//if you want, write a new random number generator routine, with other name
extern int X;
const int MM=2147483647;
const int AA=48271;
const int QQ=44488;
const int RR=3399;
void initRandomKnuth();
int randomKnuth();
	

#endif
