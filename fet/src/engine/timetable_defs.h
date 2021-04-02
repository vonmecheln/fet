/*
File timetable_defs.h
*/

/***************************************************************************
                          timetable_defs.h  -  description
                             -------------------
    begin                : Sat Mar 15 2003
    copyright            : (C) 2003 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef TIMETABLE_DEFS_H
#define TIMETABLE_DEFS_H

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

#include <QString>

#include <QtGlobal>

#include "centerwidgetonscreen.h"

#if QT_VERSION < QT_VERSION_CHECK(5,7,0)
#include <type_traits>
namespace std
{
	template<class T> using add_const_t = typename add_const<T>::type;
}
template<class T> constexpr std::add_const_t<T>& qAsConst(T& t) noexcept
{
	return t;
}
template<class T> void qAsConst(const T&&) = delete;
#endif

class QWidget;

/**
The version number
*/
extern const QString FET_VERSION;

/**
The language
*/
extern QString FET_LANGUAGE;

extern bool LANGUAGE_STYLE_RIGHT_TO_LEFT;

extern QString LANGUAGE_FOR_HTML;

/**
Timetable html css javaScript Level, added by Volker Dirr
*/
extern int TIMETABLE_HTML_LEVEL;

extern bool TIMETABLE_HTML_PRINT_ACTIVITY_TAGS;

extern bool PRINT_DETAILED_HTML_TIMETABLES;

extern bool PRINT_DETAILED_HTML_TEACHERS_FREE_PERIODS;

extern bool PRINT_NOT_AVAILABLE_TIME_SLOTS;

extern bool PRINT_BREAK_TIME_SLOTS;

extern bool DIVIDE_HTML_TIMETABLES_WITH_TIME_AXIS_BY_DAYS;

extern bool TIMETABLE_HTML_REPEAT_NAMES;

extern bool PRINT_ACTIVITIES_WITH_SAME_STARTING_TIME;

extern bool USE_GUI_COLORS;

extern bool SHOW_SUBGROUPS_IN_COMBO_BOXES;
extern bool SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING;

extern bool WRITE_TIMETABLE_CONFLICTS;

extern bool WRITE_TIMETABLES_STATISTICS;
extern bool WRITE_TIMETABLES_XML;
extern bool WRITE_TIMETABLES_DAYS_HORIZONTAL;
extern bool WRITE_TIMETABLES_DAYS_VERTICAL;
extern bool WRITE_TIMETABLES_TIME_HORIZONTAL;
extern bool WRITE_TIMETABLES_TIME_VERTICAL;

extern bool WRITE_TIMETABLES_SUBGROUPS;
extern bool WRITE_TIMETABLES_GROUPS;
extern bool WRITE_TIMETABLES_YEARS;
extern bool WRITE_TIMETABLES_TEACHERS;
extern bool WRITE_TIMETABLES_TEACHERS_FREE_PERIODS;
extern bool WRITE_TIMETABLES_ROOMS;
extern bool WRITE_TIMETABLES_SUBJECTS;
extern bool WRITE_TIMETABLES_ACTIVITY_TAGS;
extern bool WRITE_TIMETABLES_ACTIVITIES;

extern bool SHOW_VIRTUAL_ROOMS_IN_TIMETABLES;

extern bool SHOW_SHORTCUTS_ON_MAIN_WINDOW;

extern bool SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES;

extern bool ENABLE_ACTIVITY_TAG_MAX_HOURS_DAILY;
extern bool ENABLE_ACTIVITY_TAG_MIN_HOURS_DAILY;
extern bool ENABLE_STUDENTS_MAX_GAPS_PER_DAY;

extern bool SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS;

extern bool SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES;

extern bool SHOW_WARNING_FOR_ACTIVITIES_FIXED_SPACE_VIRTUAL_REAL_ROOMS_BUT_NOT_FIXED_TIME;

extern bool SHOW_WARNING_FOR_MAX_HOURS_DAILY_WITH_UNDER_100_WEIGHT;

extern bool ENABLE_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS;
extern bool SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS;

extern bool ENABLE_GROUP_ACTIVITIES_IN_INITIAL_ORDER;
extern bool SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER;

extern bool CONFIRM_ACTIVITY_PLANNING;
extern bool CONFIRM_SPREAD_ACTIVITIES;
extern bool CONFIRM_REMOVE_REDUNDANT;
extern bool CONFIRM_SAVE_TIMETABLE;

extern bool VERBOSE;

/**
The maximum total number of different subgroups of students
*/
const int MAX_TOTAL_SUBGROUPS=30000;//MAX_YEARS*MAX_GROUPS_PER_YEAR*MAX_SUBGROUPS_PER_GROUP;

const int MAX_ROOM_CAPACITY=30000;

/**
The maximum number of different teachers
*/
const int MAX_TEACHERS=6000;

/**
The maximum number of activities
DEPRECATED COMMENT BELOW
IMPORTANT: must be qint16 (max 32767), because we are using qint16 for each activity index and for
unallocated activity = max_activities
*/
const int MAX_ACTIVITIES=30000;

//if you want to increase this, you also need to modify the add/modify activity dialogs, to permit larger values
//(add more pages in the subactivities tab).
const int MAX_SPLIT_OF_AN_ACTIVITY=35;

/**
The maximum number of rooms
DEPRECATED COMMENT BELOW
IMPORTANT: max_rooms+1 must be qint16 (max 32766 for max_rooms), because we are using qint16 for each room index and
for unallocated space = max_rooms and for unspecified room = max_rooms+1
*/
const int MAX_ROOMS=6000;

/**
The maximum number of buildings
*/
const int MAX_BUILDINGS=6000;

/**
This constant represents an unallocated activity
*/
const int UNALLOCATED_ACTIVITY = MAX_ACTIVITIES;

/**
The maximum number of working hours per day.
DEPRECATED COMMENT BELOW
IMPORTANT: max hours per day * max days per week = max hours per week must be qint16 (max 32767),
because each time is qint16 and unallocated time is qint16
*/
const int MAX_HOURS_PER_DAY=72;

/**
The maximum number of working days per week.
DEPRECATED COMMENT BELOW
IMPORTANT: max hours per day * max days per week = max hours per week must be qint16 (max 32767)
because each time is qint16 and unallocated time is qint16
*/
const int MAX_DAYS_PER_WEEK=35;

/**
The predefined names of the days of the week
*/
extern const QString PREDEFINED_DAYS_OF_THE_WEEK[];

/**
The maximum number of working hours in a week.

Hours in a week are arranged like this:
         Mo Tu We Th Fr
1        0  1  2  3  4
2        5  6  7  8  9
3        10 11 12 13 14
4        15 16 17 18 19
5        20 21 22 23 24
6        25 26 27 28 29 etc.

DEPRECATED COMMENT BELOW
IMPORTANT: MAX_HOURS_PER_DAY * MAX_DAYS_PER_WEEK == MAX_HOURS_PER_WEEK must be qint16 (max 32767)
because each time is qint16 and unallocated time is qint16
*/
const int MAX_HOURS_PER_WEEK = MAX_HOURS_PER_DAY * MAX_DAYS_PER_WEEK;

/**
This constant represents unallocated time for an activity
*/
const int UNALLOCATED_TIME = MAX_HOURS_PER_WEEK;

/**
This constant represents unallocated space for an activity
*/
const int UNALLOCATED_SPACE = MAX_ROOMS;

const int UNSPECIFIED_ROOM = MAX_ROOMS+1;

/**
The maximum number of time constraints
*/
//const int MAX_TIME_CONSTRAINTS = 60000;

/**
The maximum number of space constraints
*/
//const int MAX_SPACE_CONSTRAINTS = 60000;

/**
File and directory separator
*/
extern const QString FILE_SEP;

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

//OUTPUT FILES

/**
The output directory. Please be careful when editing it,
because the functions add a FILE_SEP sign at the end of it
and then the name of a file. If you make OUTPUT_DIR="",
there might be problems.
*/
extern QString OUTPUT_DIR;

/**
A log file explaining how the xml input file was parsed
*/
extern const QString XML_PARSING_LOG_FILENAME;

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
//QString protect2id(const QString& str);

/**
A function used in html saving
*/
//QString protect2java(const QString& str);

/**
This constants represents free periods of a teacher in the teachers free periods timetable
*/
const int TEACHER_HAS_SINGLE_GAP = 0;
const int TEACHER_HAS_BORDER_GAP = 1;
const int TEACHER_HAS_BIG_GAP = 2;

const int TEACHER_MUST_COME_EARLIER = 4;
const int TEACHER_MUST_COME_MUCH_EARLIER = 6;

const int TEACHER_MUST_STAY_LONGER = 3;
const int TEACHER_MUST_STAY_MUCH_LONGER = 5;		// BE CAREFUL, I just print into LESS_DETAILED timetable, if it's smaller than TEACHER_MUST_STAY_MUCH_LONGER

const int TEACHER_HAS_A_FREE_DAY = 7;

const int TEACHER_IS_NOT_AVAILABLE = 8;

const int TEACHERS_FREE_PERIODS_N_CATEGORIES = 9;

extern bool checkForUpdates;

extern QString internetVersion;

const int STUDENTS_COMBO_BOXES_STYLE_SIMPLE=0;
const int STUDENTS_COMBO_BOXES_STYLE_ICONS=1;
const int STUDENTS_COMBO_BOXES_STYLE_CATEGORIZED=2;
extern int STUDENTS_COMBO_BOXES_STYLE;

///////tricks to save work to reconvert old code
const int CUSTOM_DOUBLE_PRECISION=6; //the maximum number of significant digits after the decimal dot for the weights
const int MULTIPLICANT_DOUBLE_PRECISION=100000000; //10^(2+CUSTOM_DOUBLE_PRECISION)
const int HALF_MULTIPLICANT_DOUBLE_PRECISION=MULTIPLICANT_DOUBLE_PRECISION/2;

void weight_sscanf(const QString& str, const char* fmt, double* result);

class CustomFETString{
public:
	static QString number(int n);
	static QString number(double x);
	static QString numberPlusTwoDigitsPrecision(double x);
};

double customFETStrToDouble(const QString& str, bool* ok=0);
////////////////////////////////////////////////

//Trick found on http://www.qtcentre.org/threads/53066-QMap-sorting-according-to-QLocale
//to create a QMap<QString, ...> ordered by locale order.
//Third post, by ChrisW67.
class LocaleString: public QString{
public:
	LocaleString(const QString& s): QString(s){}
};

inline bool operator<(const LocaleString& lhs, const LocaleString& rhs)
{
	return QString::localeAwareCompare(lhs, rhs)<0;
}
/////////////////////////////////////////////////////

extern bool BEEP_AT_END_OF_GENERATION;
extern bool ENABLE_COMMAND_AT_END_OF_GENERATION;
extern QString commandAtEndOfGeneration;
//extern bool DETACHED_NOTIFICATION;
//extern int terminateCommandAfterSeconds;
//extern int killCommandAfterSeconds;

//Old RNG below
//for random Knuth - from Knuth TAOCP Vol. 2 Seminumerical Algorithms section 3.6
//these numbers are really important - please do not change them, NEVER!!!
//if you want, write a new random number generator routine, with other name
//I think I found a minor possible improvement, the author said: if(Z<=0) then Z+=MM,
//but I think this would be better: if(Z<=0) then Z+=MM-1. - Yes, the author confirmed.
//The period of this RNG is ~74 quadrilions, according to Knuth ( (2^31-2)*(2^31-250)/62 ~= 7.4*10^16 ).
//Unfortunately, this period is not too long. For the moment I prefer to keep this RNG,
//since changing it is highly critical. But we need to consider the change for the future.
//Changing it will also mean that the method of saving/restoring the seed will need to change.
//extern int XX;
//extern int YY;
/*const int MM=2147483647;
const int AA=48271;
const int QQ=44488;
const int RR=3399;

const int MMM=2147483399;
const int AAA=40692;
const int QQQ=52774;
const int RRR=3791;

void initRandomKnuth();
int randomKnuth1MM1(); //a random between 1 and MM-1
int randomKnuth(int k); //a random between 0 and k-1
*/

//32-bits Random number generator U(0,1): MRG32k3a
//Author: Pierre L'Ecuyer
//Modified: U[0,1), as instructed by the author in his papers.

//Reference for the MRG32k3a random number generator code:

//P. L'Ecuyer, ``Good Parameter Sets for Combined Multiple Recursive
//Random Number Generators'', Shorter version in Operations Research, 47,
//1 (1999), 159--164.
//<http://pubsonline.informs.org/doi/abs/10.1287/opre.47.1.159>

//Includes ideas and code from the files: http://simul.iro.umontreal.ca/rng/MRG32k3a.h, http://simul.iro.umontreal.ca/rng/MRG32k3a.c,
//and/or https://www.iro.umontreal.ca/~lecuyer/myftp/papers/combmrg2.c

//Used with permission from the author, Pierre L'Ecuyer (9 March 2020)

//Modified so that it uses 64-bit integer number operations instead of the not-so-reliable 'double' type.

//The period of this RNG is ~2^191 or ~3*10^57. It has an internal state of 6 32-bit integers.

class MRG32k3a{
public:
	qint64 s10, s11, s12, s20, s21, s22;

	static const qint64 m1;
	static const qint64 m2;
	static const qint64 a12;
	static const qint64 a13n;
	static const qint64 a21;
	static const qint64 a23n;
	
	MRG32k3a();
	~MRG32k3a();
	
	/*
	The seeds for s10, s11, s12 must be integers in [0, m1 - 1] and not all 0.
	The seeds for s20, s21, s22 must be integers in [0, m2 - 1] and not all 0.
	*/
	void initializeMRG32k3a(qint64 _s10, qint64 _s11, qint64 _s12,
	 qint64 _s20, qint64 _s21, qint64 _s22);

	void initializeMRG32k3a();

	unsigned int uiMRG32k3a(); //returns a random number >=0 and <m1

	//double dMRG32k3a(); //returns a uniform real number (double) in the interval [0,1)

	int intMRG32k3a(int k); //returns a random integer >=0 and <k, using the multiplication method recommended in Knuth TAOCP Vol. 2, Section 3.4.1,
		//but implemented using 64-bit numbers. (although I think that we could use the integer division/modulo method, as Knuth additionally notes,
		//since m1 and m2 are prime and so the last bits are also random - but he added that note for a single modulo operation, not for the difference.)
};

#endif
