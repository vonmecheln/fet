/*
File timeconstraint.h
*/

/***************************************************************************
                          timeconstraint.h  -  description
                             -------------------
    begin                : 2002
    copyright            : (C) 2002 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef TIMECONSTRAINT_H
#define TIMECONSTRAINT_H

#include <QCoreApplication>

#include "timetable_defs.h"

#include <QString>
#include <QList>
#include <QStringList>

#include <QSet>

class Rules;
class Solution;
class TimeConstraint;
class Activity;
class Teacher;
class Subject;
class ActivityTag;
class StudentsSet;

class FakeString;

class QWidget;

typedef QList<TimeConstraint*> TimeConstraintsList;

//If you change any of these const int-s, you need to update the const QString FET_DATA_FORMAT_VERSION from timetable_defs.cpp to a new value,
//because of the disk history feature.
const int CONSTRAINT_GENERIC_TIME										=0;

const int CONSTRAINT_BASIC_COMPULSORY_TIME								=1;
const int CONSTRAINT_BREAK_TIMES										=2;

const int CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES						=3;
const int CONSTRAINT_TEACHERS_MAX_HOURS_DAILY							=4;
const int CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK							=5;
const int CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK							=6;
const int CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK							=7;
const int CONSTRAINT_TEACHER_MAX_HOURS_DAILY							=8;
const int CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY					=9;
const int CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY						=10;

const int CONSTRAINT_TEACHERS_MIN_HOURS_DAILY							=11;
const int CONSTRAINT_TEACHER_MIN_HOURS_DAILY							=12;
const int CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY							=13;
const int CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY							=14;

const int CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR		=15;
const int CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR	=16;
const int CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES					=17;
const int CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK							=18;
const int CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK						=19;

const int CONSTRAINT_STUDENTS_MAX_HOURS_DAILY							=20;
const int CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY						=21;
const int CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY					=22;
const int CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY				=23;

const int CONSTRAINT_STUDENTS_MIN_HOURS_DAILY							=24;
const int CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY						=25;

const int CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY							=26;
const int CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME					=27;
const int CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME						=28;
const int CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING							=29;
const int CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES						=30;
const int CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS						=31;
const int CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS					=32;
const int CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES					=33;
const int CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES				=34;
const int CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR						=35;
const int CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY						=36;
const int CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE							=37;
const int CONSTRAINT_TWO_ACTIVITIES_ORDERED								=38;
const int CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES						=39;
const int CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS					=40;
const int CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES				=41;

const int CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK					=42;
const int CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK				=43;
const int CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK			=44;
const int CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK				=45;

const int CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY						=46;

const int CONSTRAINT_TWO_ACTIVITIES_GROUPED								=47;

const int CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY		=48;
const int CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY		=49;
const int CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY		=50;
const int CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY	=51;

const int CONSTRAINT_TEACHERS_MAX_DAYS_PER_WEEK							=52;

const int CONSTRAINT_THREE_ACTIVITIES_GROUPED							=53;
const int CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES						=54;

const int CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK							=55;
const int CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK							=56;

const int CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY				=57;
const int CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY				=58;
const int CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY				=59;
const int CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY			=60;

const int CONSTRAINT_STUDENTS_MAX_GAPS_PER_DAY							=61;
const int CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY						=62;

const int CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TIME_SLOTS_FROM_SELECTION	=63;
const int CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS	=64;

const int CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK						=65;
const int CONSTRAINT_STUDENTS_MAX_DAYS_PER_WEEK							=66;

//2017-02-06
const int CONSTRAINT_TEACHER_MAX_SPAN_PER_DAY							=67;
const int CONSTRAINT_TEACHERS_MAX_SPAN_PER_DAY							=68;
const int CONSTRAINT_TEACHER_MIN_RESTING_HOURS							=69;
const int CONSTRAINT_TEACHERS_MIN_RESTING_HOURS							=70;
const int CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_DAY						=71;
const int CONSTRAINT_STUDENTS_MAX_SPAN_PER_DAY							=72;
const int CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS						=73;
const int CONSTRAINT_STUDENTS_MIN_RESTING_HOURS							=74;

//2018-06-13
const int CONSTRAINT_TWO_ACTIVITIES_ORDERED_IF_SAME_DAY					=75;

//2019-06-08
const int CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS	=76;
const int CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS		=77;
const int CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS			=78;
const int CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS		=79;

const int CONSTRAINT_ACTIVITY_TAGS_NOT_OVERLAPPING						=80;

const int CONSTRAINT_ACTIVITIES_OCCUPY_MIN_TIME_SLOTS_FROM_SELECTION	=81;
const int CONSTRAINT_ACTIVITIES_MIN_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS	=82;

const int CONSTRAINT_TEACHERS_ACTIVITY_TAG_MIN_HOURS_DAILY				=83;
const int CONSTRAINT_TEACHER_ACTIVITY_TAG_MIN_HOURS_DAILY				=84;
const int CONSTRAINT_STUDENTS_ACTIVITY_TAG_MIN_HOURS_DAILY				=85;
const int CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MIN_HOURS_DAILY			=86;

const int CONSTRAINT_ACTIVITY_ENDS_TEACHERS_DAY							=87;
const int CONSTRAINT_ACTIVITIES_END_TEACHERS_DAY						=88;

//Begin for mornings-afternoons
const int CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_REAL_DAYS					=89;
const int CONSTRAINT_TEACHER_MAX_REAL_DAYS_PER_WEEK						=90;
const int CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS					=91;

const int CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_REAL_DAYS					=92;
const int CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_REAL_DAYS				=93;

const int CONSTRAINT_TEACHERS_MAX_REAL_DAYS_PER_WEEK					=94;

const int CONSTRAINT_TEACHERS_MIN_REAL_DAYS_PER_WEEK					=95;
const int CONSTRAINT_TEACHER_MIN_REAL_DAYS_PER_WEEK						=96;

const int CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS	=97;
const int CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS		=98;
const int CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS	=99;
const int CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS=100;

const int CONSTRAINT_TEACHER_MAX_AFTERNOONS_PER_WEEK					=101;
const int CONSTRAINT_TEACHERS_MAX_AFTERNOONS_PER_WEEK					=102;
const int CONSTRAINT_TEACHER_MAX_MORNINGS_PER_WEEK						=103;
const int CONSTRAINT_TEACHERS_MAX_MORNINGS_PER_WEEK						=104;

const int CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET			=105;
const int CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET		=106;

const int CONSTRAINT_TEACHERS_MIN_MORNINGS_PER_WEEK						=107;
const int CONSTRAINT_TEACHER_MIN_MORNINGS_PER_WEEK						=108;
const int CONSTRAINT_TEACHERS_MIN_AFTERNOONS_PER_WEEK					=109;
const int CONSTRAINT_TEACHER_MIN_AFTERNOONS_PER_WEEK					=110;

const int CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_MORNINGS				=111;
const int CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_MORNINGS				=112;
const int CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_AFTERNOONS				=113;
const int CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_AFTERNOONS			=114;

//Added in FET Algeria and Morocco on 2018-11-02
const int CONSTRAINT_TEACHERS_MAX_GAPS_PER_REAL_DAY						=115;
const int CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY						=116;
const int CONSTRAINT_STUDENTS_MAX_GAPS_PER_REAL_DAY						=117;
const int CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_REAL_DAY					=118;

//2019-07-03
const int CONSTRAINT_TEACHERS_MIN_HOURS_DAILY_REAL_DAYS					=119;
const int CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS					=120;

//2019-08-18 - for Said213
const int CONSTRAINT_TEACHERS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR		=121;
const int CONSTRAINT_TEACHER_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR			=122;

const int CONSTRAINT_TEACHERS_MIN_HOURS_PER_MORNING						=123;
const int CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING						=124;

const int CONSTRAINT_TEACHER_MAX_SPAN_PER_REAL_DAY						=125;
const int CONSTRAINT_TEACHERS_MAX_SPAN_PER_REAL_DAY						=126;
const int CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_REAL_DAY					=127;
const int CONSTRAINT_STUDENTS_MAX_SPAN_PER_REAL_DAY						=128;

const int CONSTRAINT_TEACHER_MORNING_INTERVAL_MAX_DAYS_PER_WEEK			=129;
const int CONSTRAINT_TEACHERS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK		=130;

const int CONSTRAINT_TEACHER_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK		=131;
const int CONSTRAINT_TEACHERS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK		=132;

const int CONSTRAINT_STUDENTS_MIN_HOURS_PER_MORNING						=133;
const int CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_MORNING					=134;

const int CONSTRAINT_TEACHER_MAX_ZERO_GAPS_PER_AFTERNOON				=135;
const int CONSTRAINT_TEACHERS_MAX_ZERO_GAPS_PER_AFTERNOON				=136;

const int CONSTRAINT_STUDENTS_SET_MAX_AFTERNOONS_PER_WEEK				=137;
const int CONSTRAINT_STUDENTS_MAX_AFTERNOONS_PER_WEEK					=138;
const int CONSTRAINT_STUDENTS_SET_MAX_MORNINGS_PER_WEEK					=139;
const int CONSTRAINT_STUDENTS_MAX_MORNINGS_PER_WEEK						=140;

const int CONSTRAINT_STUDENTS_MIN_MORNINGS_PER_WEEK						=141;
const int CONSTRAINT_STUDENTS_SET_MIN_MORNINGS_PER_WEEK					=142;
const int CONSTRAINT_STUDENTS_MIN_AFTERNOONS_PER_WEEK					=143;
const int CONSTRAINT_STUDENTS_SET_MIN_AFTERNOONS_PER_WEEK				=144;

const int CONSTRAINT_STUDENTS_SET_MORNING_INTERVAL_MAX_DAYS_PER_WEEK	=145;
const int CONSTRAINT_STUDENTS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK		=146;
const int CONSTRAINT_STUDENTS_SET_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK	=147;
const int CONSTRAINT_STUDENTS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK		=148;

const int CONSTRAINT_TEACHER_MAX_HOURS_PER_ALL_AFTERNOONS				=149;
const int CONSTRAINT_TEACHERS_MAX_HOURS_PER_ALL_AFTERNOONS				=150;

const int CONSTRAINT_STUDENTS_SET_MAX_HOURS_PER_ALL_AFTERNOONS			=151;
const int CONSTRAINT_STUDENTS_MAX_HOURS_PER_ALL_AFTERNOONS				=152;

const int CONSTRAINT_TEACHER_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON		=153;
const int CONSTRAINT_TEACHERS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON		=154;
const int CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON	=155;
const int CONSTRAINT_STUDENTS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON		=156;

//2020-07-24 - for lakhdar bezzit
const int CONSTRAINT_STUDENTS_SET_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR	=157;
const int CONSTRAINT_STUDENTS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR		=158;

//Added in FET Algeria and Morocco on 2020-07-29
const int CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS						=159;
const int CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS						=160;
const int CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS						=161;
const int CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS					=162;

const int CONSTRAINT_STUDENTS_SET_MAX_REAL_DAYS_PER_WEEK				=163;
const int CONSTRAINT_STUDENTS_MAX_REAL_DAYS_PER_WEEK					=164;
//End for mornings-afternoons

//For block-planning mode
const int CONSTRAINT_MAX_TOTAL_ACTIVITIES_FROM_SET_IN_SELECTED_TIME_SLOTS			=165;

const int CONSTRAINT_MAX_GAPS_BETWEEN_ACTIVITIES									=166;

//For the terms mode
const int CONSTRAINT_ACTIVITIES_MAX_IN_A_TERM							=167;
const int CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TERMS						=168;

//Begin for mornings-afternoons - 2021-08-12
const int CONSTRAINT_TEACHERS_MAX_GAPS_PER_MORNING_AND_AFTERNOON					=169;
const int CONSTRAINT_TEACHER_MAX_GAPS_PER_MORNING_AND_AFTERNOON						=170;

const int CONSTRAINT_TEACHERS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR			=171;
const int CONSTRAINT_TEACHER_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR			=172;
const int CONSTRAINT_STUDENTS_SET_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR		=173;
const int CONSTRAINT_STUDENTS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR			=174;
//End   for mornings-afternoons

const int CONSTRAINT_TWO_SETS_OF_ACTIVITIES_ORDERED									=175;

//Begin for mornings-afternoons - 2021-09-26
const int CONSTRAINT_TEACHERS_MAX_THREE_CONSECUTIVE_DAYS							=176;
const int CONSTRAINT_TEACHER_MAX_THREE_CONSECUTIVE_DAYS								=177;
//End   for mornings-afternoons - 2021-09-26

//2021-12-15
const int CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG						=178;
const int CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG							=179;
const int CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG							=180;
const int CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG							=181;

//Begin for mornings-afternoons - 2022-02-15
const int CONSTRAINT_STUDENTS_MAX_THREE_CONSECUTIVE_DAYS							=182;
const int CONSTRAINT_STUDENTS_SET_MAX_THREE_CONSECUTIVE_DAYS						=183;
//End   for mornings-afternoons - 2021-02-15

//Begin for mornings-afternoons - 2022-05-14
const int CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES								=184;
//End   for mornings-afternoons - 2022-05-14

const int CONSTRAINT_ACTIVITY_PREFERRED_DAY											=185;

//Begin for terms - 2022-05-20
const int CONSTRAINT_ACTIVITIES_MIN_IN_A_TERM										=186;
const int CONSTRAINT_MAX_TERMS_BETWEEN_ACTIVITIES									=187;
//End   for terms - 2022-05-20

const int CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET				=188;
const int CONSTRAINT_STUDENTS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET					=189;

const int CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET				=190;
const int CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET				=191;
const int CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET			=192;
const int CONSTRAINT_STUDENTS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET				=193;

//Begin for mornings-afternoons - 2022-07-31
const int CONSTRAINT_MAX_HALF_DAYS_BETWEEN_ACTIVITIES								=194;
//End   for mornings-afternoons - 2022-07-31

const int CONSTRAINT_ACTIVITY_BEGINS_STUDENTS_DAY									=195;
const int CONSTRAINT_ACTIVITIES_BEGIN_STUDENTS_DAY									=196;
const int CONSTRAINT_ACTIVITY_BEGINS_TEACHERS_DAY									=197;
const int CONSTRAINT_ACTIVITIES_BEGIN_TEACHERS_DAY									=198;

const int CONSTRAINT_TEACHERS_MIN_HOURS_PER_AFTERNOON								=199;
const int CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON								=200;
const int CONSTRAINT_STUDENTS_MIN_HOURS_PER_AFTERNOON								=201;
const int CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_AFTERNOON							=202;

const int CONSTRAINT_ACTIVITIES_MAX_HOURLY_SPAN										=203;

const int CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_IN_INTERVAL							=204;
const int CONSTRAINT_TEACHER_MAX_HOURS_DAILY_IN_INTERVAL							=205;
const int CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_IN_INTERVAL							=206;
const int CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_IN_INTERVAL						=207;

//2024-03-15
const int CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY	=208;
const int CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY		=209;
const int CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY		=210;
const int CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY		=211;

const int CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY					=212;
const int CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY						=213;
const int CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY							=214;
const int CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY						=215;

//2024-05-18
const int CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON	=216;
const int CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON		=217;
const int CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON		=218;
const int CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON		=219;

const int CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON					=220;
const int CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON						=221;
const int CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON						=222;
const int CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON						=223;

//2024-09-03
const int CONSTRAINT_TEACHERS_NO_TWO_CONSECUTIVE_DAYS															=224;
const int CONSTRAINT_TEACHER_NO_TWO_CONSECUTIVE_DAYS															=225;

//2025-03-15
const int CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS												=226;
const int CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS												=227;
const int CONSTRAINT_STUDENTS_SET_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS											=228;
const int CONSTRAINT_STUDENTS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS												=229;

const int CONSTRAINT_TWO_SETS_OF_ACTIVITIES_SAME_SECTIONS														=230;

//2025-05-15 - suggested by Yush Yuen
const int CONSTRAINT_STUDENTS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS											=231;
const int CONSTRAINT_STUDENTS_SET_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS										=232;
//2025-05-17
const int CONSTRAINT_TEACHERS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS											=233;
const int CONSTRAINT_TEACHER_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS												=234;

const int CONSTRAINT_TEACHER_MAX_HOURS_PER_TERM																	=235;
const int CONSTRAINT_TEACHERS_MAX_HOURS_PER_TERM																=236;

//2025-08-04
const int CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS										=237;
const int CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS										=238;
const int CONSTRAINT_STUDENTS_SET_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS									=239;
const int CONSTRAINT_STUDENTS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS										=240;

const int CONSTRAINT_ACTIVITIES_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS									=241;
const int CONSTRAINT_ACTIVITIES_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS											=242;

//2025-08-17
const int CONSTRAINT_TEACHER_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION										=243;
const int CONSTRAINT_TEACHERS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION										=244;

const int CONSTRAINT_STUDENTS_SET_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION								=245;
const int CONSTRAINT_STUDENTS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION										=246;

const int CONSTRAINT_ACTIVITIES_OVERLAP_COMPLETELY_OR_DO_NOT_OVERLAP											=247;

const int CONSTRAINT_ACTIVITIES_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION									=248;

const int CONSTRAINT_ACTIVITY_BEGINS_OR_ENDS_STUDENTS_DAY														=249;
const int CONSTRAINT_ACTIVITIES_BEGIN_OR_END_STUDENTS_DAY														=250;
const int CONSTRAINT_ACTIVITY_BEGINS_OR_ENDS_TEACHERS_DAY														=251;
const int CONSTRAINT_ACTIVITIES_BEGIN_OR_END_TEACHERS_DAY														=252;

const int CONSTRAINT_ACTIVITIES_MAX_TOTAL_NUMBER_OF_STUDENTS_IN_SELECTED_TIME_SLOTS								=253;

class QDataStream;

QString getActivityDescription(Rules& r, int id);
QString getActivityDetailedDescription(Rules& r, int id);

bool timeConstraintCanHaveAnyWeight(int type);

/**
This class represents a time constraint
*/
class TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(TimeConstraint)

public:
	/**
	The percentage weight of this constraint, 100% compulsory, 0% non-compulsory
	*/
	double weightPercentage;
	
	bool active;
	
	QString comments;

	/**
	Specifies the type of this constraint (using the above constants).
	*/
	int type;

	/**
	True for mandatory constraints, false for non-mandatory constraints.
	*/
	//bool compulsory;

	/**
	Dummy constructor - needed for the static array of constraints.
	Any other use should be avoided.
	*/
	TimeConstraint();
	
	virtual ~TimeConstraint()=0;

	/**
	DEPRECATED COMMENT BELOW:
	Constructor - please note that the maximum allowed weight is 100.0
	The reason: unallocated activities must have very big conflict weight,
	and any other constraints must have much more lower weight,
	so that the timetable can evolve when starting with uninitialized activities.
	*/
	TimeConstraint(double wp);
	
	bool canHaveAnyWeight(); //false if it can have the weight only 100%.

	bool canBeUsedInOfficialMode();
	bool canBeUsedInMorningsAfternoonsMode();
	bool canBeUsedInBlockPlanningMode();
	bool canBeUsedInTermsMode();

	/**
	The function that calculates the fitness of a solution, according to this
	constraint. We need the solution to compute this fitness factor.
	If conflictsString!=nullptr, it will be initialized with a text explaining
	where this constraint is broken.
	*/
	virtual double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr)=0;

	/**
	Returns an XML description of this constraint
	*/
	virtual QString getXmlDescription(Rules& r)=0;

	/**
	Computes the internal structure for this constraint.
	
	It returns false if the constraint is an activity related
	one and it depends on only inactive activities.
	*/
	virtual bool computeInternalStructure(QWidget* parent, Rules& r)=0;
	
	virtual bool hasInactiveActivities(Rules& r)=0;

	/**
	Returns a small description string for this constraint
	*/
	virtual QString getDescription(Rules& r)=0;

	/**
	Returns a detailed description string for this constraint
	*/
	virtual QString getDetailedDescription(Rules& r)=0;
	
	/**
	Returns true if this constraint is related to this activity
	*/
	virtual bool isRelatedToActivity(Rules& r, Activity* a)=0;

	/**
	Returns true if this constraint is related to this teacher
	*/
	virtual bool isRelatedToTeacher(Teacher* t)=0;

	/**
	Returns true if this constraint is related to this subject
	*/
	virtual bool isRelatedToSubject(Subject* s)=0;

	/**
	Returns true if this constraint is related to this activity tag
	*/
	virtual bool isRelatedToActivityTag(ActivityTag* s)=0;

	/**
	Returns true if this constraint is related to this students set
	*/
	virtual bool isRelatedToStudentsSet(Rules& r, StudentsSet* s)=0;
	
	virtual bool hasWrongDayOrHour(Rules& r)=0;
	virtual bool canRepairWrongDayOrHour(Rules& r)=0;
	virtual bool repairWrongDayOrHour(Rules& r)=0;
};

/**
This class comprises all the basic compulsory constraints (constraints
which must be fulfilled for any timetable) - the time allocation part
*/
class ConstraintBasicCompulsoryTime: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintBasicCompulsoryTime)

public:
	ConstraintBasicCompulsoryTime();

	ConstraintBasicCompulsoryTime(double wp);

	bool computeInternalStructure(QWidget* parent, Rules& r);
	
	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherNotAvailableTimes: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherNotAvailableTimes)

public:
	QList<int> days;
	QList<int> hours;

	/**
	The teacher's name
	*/
	QString teacher;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherNotAvailableTimes();

	ConstraintTeacherNotAvailableTimes(double wp, const QString& tn, const QList<int>& d, const QList<int>& h);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetNotAvailableTimes: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetNotAvailableTimes)

public:
	QList<int> days;
	QList<int> hours;

	/**
	The name of the students
	*/
	QString students;

	/**
	The subgroups involved in this constraint
	*/
	QList<int> iSubgroupsList;

	ConstraintStudentsSetNotAvailableTimes();

	ConstraintStudentsSetNotAvailableTimes(double wp, const QString& sn, const QList<int>& d, const QList<int>& h);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesSameStartingTime: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesSameStartingTime)

public:
	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id-s)
	*/
	QList<int> activitiesIds;
	//int activitiesIds[MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME];

	QSet<int> activitiesIdsSet;

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (indexes in the rules) - internal structure
	*/
	//int _activities[MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME];
	QList<int> _activities;

	ConstraintActivitiesSameStartingTime();

	/**
	Constructor, using:
	the weight, the number of activities and the list of activities' id-s.
	*/
	//ConstraintActivitiesSameStartingTime(double wp, int n_act, const int act[]);
	ConstraintActivitiesSameStartingTime(double wp, int n_act, const QList<int>& act);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);
	
	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint.
It aims at scheduling a set of activities so that they do not overlap.
The number of conflicts is considered the number of overlapping
hours.
*/
class ConstraintActivitiesNotOverlapping: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesNotOverlapping)

public:
	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id-s)
	*/
	QList<int> activitiesIds;
	//int activitiesIds[MAX_CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING];

	QSet<int> activitiesIdsSet;

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (index in the rules) - internal structure
	*/
	//int _activities[MAX_CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING];
	QList<int> _activities;

	ConstraintActivitiesNotOverlapping();

	/**
	Constructor, using:
	the weight, the number of activities and the list of activities.
	*/
	ConstraintActivitiesNotOverlapping(double wp, int n_act, const QList<int>& act);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules &r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivityTagsNotOverlapping: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityTagsNotOverlapping)

public:
	QStringList activityTagsNames;
	
	//internal
	QList<int> activityTagsIndices;
	
	QList<QList<int>> activitiesIndicesLists;

	ConstraintActivityTagsNotOverlapping();

	ConstraintActivityTagsNotOverlapping(double wp, const QStringList& atl);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint.
It aims at scheduling a set of activities so that they
have a minimum of N days between any two of them.
*/
class ConstraintMinDaysBetweenActivities: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintMinDaysBetweenActivities)

public:
	bool consecutiveIfSameDay;

	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id-s)
	*/
	QList<int> activitiesIds;
	//int activitiesIds[MAX_CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES];

	QSet<int> activitiesIdsSet;

	/**
	The number of minimum days between each 2 activities
	*/
	int minDays;

	//internal structure (redundant)

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (index in the rules) - internal structure
	*/
	QList<int> _activities;
	//int _activities[MAX_CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES];

	ConstraintMinDaysBetweenActivities();

	/**
	Constructor, using:
	the weight, consecutive if on the same day, the number of activities, the list of activities, and the min number of days.
	*/
	ConstraintMinDaysBetweenActivities(double wp, bool cisd, int n_act, const QList<int>& act, int _minDays);

	/**
	Comparison operator - to be sure that we do not introduce duplicates
	*/
	bool operator==(ConstraintMinDaysBetweenActivities& c);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules &r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintMaxDaysBetweenActivities: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintMaxDaysBetweenActivities)

public:
	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id-s)
	*/
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;

	/**
	The number of maximum days between each 2 activities
	*/
	int maxDays;

	//internal structure (redundant)

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (index in the rules) - internal structure
	*/
	QList<int> _activities;

	ConstraintMaxDaysBetweenActivities();

	/**
	Constructor, using:
	the weight, the number of activities, the list of activities, and the max number of days.
	*/
	ConstraintMaxDaysBetweenActivities(double wp, int n_act, const QList<int>& act, int _maxDays);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules &r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesMaxHourlySpan: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesMaxHourlySpan)

public:
	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id-s)
	*/
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;

	/**
	The number of maximum hourly span of each pair of 2 activities
	*/
	int maxHourlySpan;

	//internal structure (redundant)

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (index in the rules) - internal structure
	*/
	QList<int> _activities;

	ConstraintActivitiesMaxHourlySpan();

	/**
	Constructor, using:
	the weight, the number of activities, the list of activities, and the max hourly span.
	*/
	ConstraintActivitiesMaxHourlySpan(double wp, int n_act, const QList<int>& act, int _maxHourlySpan);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules &r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintMinGapsBetweenActivities: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintMinGapsBetweenActivities)

public:
	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id-s)
	*/
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;

	/**
	The number of minimum gaps between each 2 activities, if on the same day
	*/
	int minGaps;

	//internal structure (redundant)

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (index in the rules) - internal structure
	*/
	QList<int> _activities;

	ConstraintMinGapsBetweenActivities();

	/**
	Constructor, using:
	the weight, the number of activities, the list of activities, and the min number of gaps.
	*/
	//ConstraintMinGapsBetweenActivities(double wp, int n_act, const int act[], int ngaps);
	ConstraintMinGapsBetweenActivities(double wp, int n_act, const QList<int>& actList, int ngaps);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules &r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintMaxGapsBetweenActivities: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintMaxGapsBetweenActivities)

public:
	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id-s)
	*/
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;

	/**
	The number of maximum gaps between each 2 activities, if on the same day
	*/
	int maxGaps;

	//internal structure (redundant)

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (index in the rules) - internal structure
	*/
	QList<int> _activities;

	ConstraintMaxGapsBetweenActivities();

	/**
	Constructor, using:
	the weight, the number of activities, the list of activities, and the max number of gaps.
	*/
	//ConstraintMaxGapsBetweenActivities(double wp, int n_act, const int act[], int ngaps);
	ConstraintMaxGapsBetweenActivities(double wp, int n_act, const QList<int>& actList, int ngaps);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules &r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxHoursDaily: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxHoursDaily)

public:
	/**
	The maximum hours daily
	*/
	int maxHoursDaily;

	ConstraintTeachersMaxHoursDaily();

	ConstraintTeachersMaxHoursDaily(double wp, int maxhours);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxHoursDaily: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxHoursDaily)

public:
	/**
	The maximum hours daily
	*/
	int maxHoursDaily;
	
	QString teacherName;
	
	int teacher_ID;

	ConstraintTeacherMaxHoursDaily();

	ConstraintTeacherMaxHoursDaily(double wp, int maxhours, const QString& teacher);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxHoursDailyInInterval: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxHoursDailyInInterval)

public:
	/**
	The maximum hours daily
	*/
	int maxHoursDaily;
	
	int startHour;
	int endHour;
	
	QString teacherName;
	
	int teacher_ID;

	ConstraintTeacherMaxHoursDailyInInterval();

	ConstraintTeacherMaxHoursDailyInInterval(double wp, int maxhours, const QString& teacher, int sh, int eh);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxHoursDailyInInterval: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxHoursDailyInInterval)

public:
	/**
	The maximum hours daily
	*/
	int maxHoursDaily;
	
	int startHour;
	int endHour;
	
	ConstraintTeachersMaxHoursDailyInInterval();

	ConstraintTeachersMaxHoursDailyInInterval(double wp, int maxhours, int sh, int eh);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxHoursDailyInInterval: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxHoursDailyInInterval)

public:
	int maxHoursDaily;

	/**
	The students set name
	*/
	QString students;

	//internal variables

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;

	int startHour;
	int endHour;

	ConstraintStudentsSetMaxHoursDailyInInterval();

	ConstraintStudentsSetMaxHoursDailyInInterval(double wp, int maxnh, const QString& s, int sh, int eh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxHoursDailyInInterval: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxHoursDailyInInterval)

public:
	int maxHoursDaily;

	int startHour;
	int endHour;

	ConstraintStudentsMaxHoursDailyInInterval();

	ConstraintStudentsMaxHoursDailyInInterval(double wp, int maxnh, int sh, int eh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint, aimed at obtaining timetables
which do not allow more than X hours in a row for any teacher
*/
class ConstraintTeachersMaxHoursContinuously: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxHoursContinuously)

public:
	/**
	The maximum hours continuously
	*/
	int maxHoursContinuously;

	ConstraintTeachersMaxHoursContinuously();

	ConstraintTeachersMaxHoursContinuously(double wp, int maxhours);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxHoursContinuously: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxHoursContinuously)

public:
	/**
	The maximum hours continuously
	*/
	int maxHoursContinuously;
	
	QString teacherName;
	
	int teacher_ID;

	ConstraintTeacherMaxHoursContinuously();

	ConstraintTeacherMaxHoursContinuously(double wp, int maxhours, const QString& teacher);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersActivityTagMaxHoursContinuously: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersActivityTagMaxHoursContinuously)

public:
	/**
	The maximum hours continuously
	*/
	int maxHoursContinuously;
	
	QString activityTagName;
	
	int activityTagIndex;
	
	QList<int> canonicalTeachersList;

	ConstraintTeachersActivityTagMaxHoursContinuously();

	ConstraintTeachersActivityTagMaxHoursContinuously(double wp, int maxhours, const QString& activityTag);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherActivityTagMaxHoursContinuously: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherActivityTagMaxHoursContinuously)

public:
	/**
	The maximum hours continuously
	*/
	int maxHoursContinuously;
	
	QString teacherName;
	
	QString activityTagName;
	
	int teacher_ID;

	int activityTagIndex;
	
	QList<int> canonicalTeachersList;

	ConstraintTeacherActivityTagMaxHoursContinuously();

	ConstraintTeacherActivityTagMaxHoursContinuously(double wp, int maxhours, const QString& teacher, const QString& activityTag);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint.
The resulting timetable must respect the requirement
that this teacher must not have too much working
days per week.
*/
class ConstraintTeacherMaxDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week (-1 for don't care)
	*/
	int maxDaysPerWeek;

	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherMaxDaysPerWeek();

	ConstraintTeacherMaxDaysPerWeek(double wp, int maxnd, const QString& t);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week (-1 for don't care)
	*/
	int maxDaysPerWeek;

	ConstraintTeachersMaxDaysPerWeek();

	ConstraintTeachersMaxDaysPerWeek(double wp, int maxnd);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinDaysPerWeek)

public:
	int minDaysPerWeek;

	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherMinDaysPerWeek();

	ConstraintTeacherMinDaysPerWeek(double wp, int mindays, const QString& t);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinDaysPerWeek)

public:
	int minDaysPerWeek;

	ConstraintTeachersMinDaysPerWeek();

	ConstraintTeachersMinDaysPerWeek(double wp, int mindays);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint.
It constrains the timetable to not schedule any activity
in the specified day, during the start hour and end hour.
*/
class ConstraintBreakTimes: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintBreakTimes)

public:
	QList<int> days;
	QList<int> hours;

	ConstraintBreakTimes();

	ConstraintBreakTimes(double wp, const QList<int>& d, const QList<int>& h);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint. It adds, to the fitness of
the solution, a conflicts factor computed from the gaps
existing in the timetable (regarding the students).
The overall result is a timetable having less gaps for the students.
*/
class ConstraintStudentsMaxGapsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxGapsPerWeek)

public:
	int maxGaps;

	ConstraintStudentsMaxGapsPerWeek();

	ConstraintStudentsMaxGapsPerWeek(double wp, int mg);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint. It adds, to the fitness of
the solution, a conflicts factor computed from the gaps
existing in the timetable (regarding the specified students set).
*/
class ConstraintStudentsSetMaxGapsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxGapsPerWeek)

public:
	int maxGaps;

	/**
	The name of the students set for this constraint
	*/
	QString students;

	//internal redundant data

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMaxGapsPerWeek();

	ConstraintStudentsSetMaxGapsPerWeek(double wp, int mg, const QString& st);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxGapsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxGapsPerWeek)

public:
	int maxGaps;

	ConstraintTeachersMaxGapsPerWeek();

	ConstraintTeachersMaxGapsPerWeek(double wp, int maxGaps);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxGapsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxGapsPerWeek)

public:
	int maxGaps;
	
	QString teacherName;
	
	int teacherIndex;

	ConstraintTeacherMaxGapsPerWeek();

	ConstraintTeacherMaxGapsPerWeek(double wp, const QString& tn, int maxGaps);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxGapsPerDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxGapsPerDay)

public:
	int maxGaps;

	ConstraintTeachersMaxGapsPerDay();

	ConstraintTeachersMaxGapsPerDay(double wp, int maxGaps);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxGapsPerDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxGapsPerDay)

public:
	int maxGaps;
	
	QString teacherName;
	
	int teacherIndex;

	ConstraintTeacherMaxGapsPerDay();

	ConstraintTeacherMaxGapsPerDay(double wp, const QString& tn, int maxGaps);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxGapsPerMorningAndAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxGapsPerMorningAndAfternoon)

public:
	int maxGaps;

	ConstraintTeachersMaxGapsPerMorningAndAfternoon();

	ConstraintTeachersMaxGapsPerMorningAndAfternoon(double wp, int maxGaps);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxGapsPerMorningAndAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxGapsPerMorningAndAfternoon)

public:
	int maxGaps;
	
	QString teacherName;
	
	int teacherIndex;

	ConstraintTeacherMaxGapsPerMorningAndAfternoon();

	ConstraintTeacherMaxGapsPerMorningAndAfternoon(double wp, const QString& tn, int maxGaps);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint. It adds, to the fitness of
the solution, a fitness factor that is related to how early
the students begin their courses. The result is a timetable
having more activities scheduled at the beginning of the day.
IMPORTANT: fortnightly activities are treated as weekly ones,
for speed and because in normal situations this does not matter.
*/
class ConstraintStudentsEarlyMaxBeginningsAtSecondHour: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsEarlyMaxBeginningsAtSecondHour)

public:

	int maxBeginningsAtSecondHour;

	ConstraintStudentsEarlyMaxBeginningsAtSecondHour();

	ConstraintStudentsEarlyMaxBeginningsAtSecondHour(double wp, int mBSH);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour)

public:
	int maxBeginningsAtSecondHour;

	/**
	The name of the students
	*/
	QString students;

	/**
	The number of subgroups involved in this constraint
	*/
	//int nSubgroups;

	/**
	The subgroups involved in this constraint
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;

	ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour();

	ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour(double wp, int mBSH, const QString& students);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxHoursDaily: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxHoursDaily)

public:
	int maxHoursDaily;

	ConstraintStudentsMaxHoursDaily();

	ConstraintStudentsMaxHoursDaily(double wp, int maxnh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxHoursDaily: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxHoursDaily)

public:
	int maxHoursDaily;

	/**
	The students set name
	*/
	QString students;

	//internal variables

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMaxHoursDaily();

	ConstraintStudentsSetMaxHoursDaily(double wp, int maxnh, const QString& s);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxHoursContinuously: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxHoursContinuously)

public:
	int maxHoursContinuously;

	ConstraintStudentsMaxHoursContinuously();

	ConstraintStudentsMaxHoursContinuously(double wp, int maxnh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxHoursContinuously: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxHoursContinuously)

public:
	int maxHoursContinuously;

	/**
	The students set name
	*/
	QString students;

	//internal variables

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMaxHoursContinuously();

	ConstraintStudentsSetMaxHoursContinuously(double wp, int maxnh, const QString& s);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsActivityTagMaxHoursContinuously: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsActivityTagMaxHoursContinuously)

public:
	int maxHoursContinuously;
	
	QString activityTagName;
	
	int activityTagIndex;
	
	QList<int> canonicalSubgroupsList;

	ConstraintStudentsActivityTagMaxHoursContinuously();

	ConstraintStudentsActivityTagMaxHoursContinuously(double wp, int maxnh, const QString& activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetActivityTagMaxHoursContinuously: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetActivityTagMaxHoursContinuously)

public:
	int maxHoursContinuously;

	/**
	The students set name
	*/
	QString students;
	
	QString activityTagName;

	//internal variables
	
	int activityTagIndex;

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;
	
	QList<int> canonicalSubgroupsList;

	ConstraintStudentsSetActivityTagMaxHoursContinuously();

	ConstraintStudentsSetActivityTagMaxHoursContinuously(double wp, int maxnh, const QString& s, const QString& activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMinHoursDaily: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMinHoursDaily)

public:
	int minHoursDaily;
	
	bool allowEmptyDays;

	ConstraintStudentsMinHoursDaily();

	ConstraintStudentsMinHoursDaily(double wp, int minnh, bool _allowEmptyDays);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMinHoursDaily: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMinHoursDaily)

public:
	int minHoursDaily;

	/**
	The students set name
	*/
	QString students;
	
	bool allowEmptyDays;

	//internal variables

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMinHoursDaily();

	ConstraintStudentsSetMinHoursDaily(double wp, int minnh, const QString& s, bool _allowEmptyDays);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint.
It adds, to the fitness of the solution, a fitness factor that
grows as the activity is scheduled farther from the wanted time
For the moment, fitness factor increases with one unit for every hour
and one unit for every day.
*/
class ConstraintActivityPreferredStartingTime: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityPreferredStartingTime)

public:
	/**
	Activity id
	*/
	int activityId;

	/**
	The preferred day. If -1, then the user does not care about the day.
	*/
	int day;

	/**
	The preferred hour. If -1, then the user does not care about the hour.
	*/
	int hour;

	bool permanentlyLocked; //if this is true, then this activity cannot be unlocked from the timetable view form

	//internal variables
	/**
	The index of the activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int activityIndex;
	
	ConstraintActivityPreferredStartingTime();

	ConstraintActivityPreferredStartingTime(double wp, int actId, int d, int h, bool perm);

	/**
	Comparison operator - to be sure that we do not introduce duplicates
	*/
	bool operator==(const ConstraintActivityPreferredStartingTime& c);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint.
It returns conflicts if the activity is scheduled in another interval
than the preferred set of times.
*/
class ConstraintActivityPreferredTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityPreferredTimeSlots)

public:
	/**
	Activity id
	*/
	int p_activityId;

	/**
	The number of preferred times
	*/
	int p_nPreferredTimeSlots_L;

	/**
	The preferred days. If -1, then the user does not care about the day.
	*/
	//int p_days[MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS];
	QList<int> p_days_L;

	/**
	The preferred hour. If -1, then the user does not care about the hour.
	*/
	//int p_hours[MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS];
	QList<int> p_hours_L;

	//internal variables
	/**
	The index of the activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int p_activityIndex;

	ConstraintActivityPreferredTimeSlots();

	//ConstraintActivityPreferredTimeSlots(double wp, int actId, int nPT, int d[], int h[]);
	ConstraintActivityPreferredTimeSlots(double wp, int actId, int nPT_L, const QList<int>& d_L, const QList<int>& h_L);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivityPreferredStartingTimes: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityPreferredStartingTimes)

public:
	/**
	Activity id
	*/
	int activityId;

	/**
	The number of preferred times
	*/
	int nPreferredStartingTimes_L;

	/**
	The preferred days. If -1, then the user does not care about the day.
	*/
	//int days[MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES];
	QList<int> days_L;

	/**
	The preferred hour. If -1, then the user does not care about the hour.
	*/
	//int hours[MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES];
	QList<int> hours_L;

	//internal variables
	/**
	The index of the activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int activityIndex;

	ConstraintActivityPreferredStartingTimes();

	//ConstraintActivityPreferredStartingTimes(double wp, int actId, int nPT, int d[], int h[]);
	ConstraintActivityPreferredStartingTimes(double wp, int actId, int nPT_L, const QList<int>& d_L, const QList<int>& h_L);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint.
It returns conflicts if a set of activities is scheduled in another interval
than the preferred set of times.
The set of activities is specified by a subject, teacher, students or a combination
of these.
*/
class ConstraintActivitiesPreferredTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesPreferredTimeSlots)

public:
	/**
	The teacher. If void, all teachers.
	*/
	QString p_teacherName;

	/**
	The students. If void, all students.
	*/
	QString p_studentsName;

	/**
	The subject. If void, all subjects.
	*/
	QString p_subjectName;

	/**
	The activity tag. If void, all activity tags.
	*/
	QString p_activityTagName;

	int duration; //if -1, neglected. Otherwise, >=1.

	/**
	The number of preferred times
	*/
	int p_nPreferredTimeSlots_L;

	/**
	The preferred days. If -1, then the user does not care about the day.
	*/
	//int p_days[MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS];
	QList<int> p_days_L;

	/**
	The preferred hours. If -1, then the user does not care about the hour.
	*/
	//int p_hours[MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS];
	QList<int> p_hours_L;

	//internal variables
	
	/**
	The number of activities which are represented by the subject, teacher and students requirements.
	*/
	int p_nActivities;
	
	/**
	The indices of the activities in the rules (from 0 to rules.nActivities-1)
	These are indices in the internal list -> Rules::internalActivitiesList
	*/
	QList<int> p_activitiesIndices;

	ConstraintActivitiesPreferredTimeSlots();

	//ConstraintActivitiesPreferredTimeSlots(double wp, const QString& te,
	//	const QString& st, const QString& su, const QString& sut, int nPT, int d[], int h[]);
	ConstraintActivitiesPreferredTimeSlots(double wp, const QString& te,
		const QString& st, const QString& su, const QString& sut, int dur, int nPT_L, QList<int> d_L, QList<int> h_L);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintSubactivitiesPreferredTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintSubactivitiesPreferredTimeSlots)

public:
	int componentNumber;

	/**
	The teacher. If void, all teachers.
	*/
	QString p_teacherName;

	/**
	The students. If void, all students.
	*/
	QString p_studentsName;

	/**
	The subject. If void, all subjects.
	*/
	QString p_subjectName;

	/**
	The activity tag. If void, all activity tags.
	*/
	QString p_activityTagName;

	int duration; //if -1, neglected. Otherwise, >=1.

	/**
	The number of preferred times
	*/
	int p_nPreferredTimeSlots_L;

	/**
	The preferred days. If -1, then the user does not care about the day.
	*/
	//int p_days[MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS];
	QList<int> p_days_L;

	/**
	The preferred hours. If -1, then the user does not care about the hour.
	*/
	//int p_hours[MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS];
	QList<int> p_hours_L;

	//internal variables
	
	/**
	The number of activities which are represented by the subject, teacher and students requirements.
	*/
	int p_nActivities;
	
	/**
	The indices of the activities in the rules (from 0 to rules.nActivities-1)
	These are indices in the internal list -> Rules::internalActivitiesList
	*/
	QList<int> p_activitiesIndices;

	ConstraintSubactivitiesPreferredTimeSlots();

	//ConstraintSubactivitiesPreferredTimeSlots(double wp, int compNo, const QString& te,
	//	const QString& st, const QString& su, const QString& sut, int nPT, int d[], int h[]);
	ConstraintSubactivitiesPreferredTimeSlots(double wp, int compNo, const QString& te,
		const QString& st, const QString& su, const QString& sut, int dur, int nPT_L, QList<int> d_L, QList<int> h_L);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesPreferredStartingTimes: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesPreferredStartingTimes)

public:
	/**
	The teacher. If void, all teachers.
	*/
	QString teacherName;

	/**
	The students. If void, all students.
	*/
	QString studentsName;

	/**
	The subject. If void, all subjects.
	*/
	QString subjectName;

	/**
	The activity tag. If void, all activity tags.
	*/
	QString activityTagName;

	int duration; //if -1, neglected. Otherwise, >=1.

	/**
	The number of preferred times
	*/
	int nPreferredStartingTimes_L;

	/**
	The preferred days. If -1, then the user does not care about the day.
	*/
	//int days[MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES];
	QList<int> days_L;

	/**
	The preferred hours. If -1, then the user does not care about the hour.
	*/
	//int hours[MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES];
	QList<int> hours_L;

	//internal variables
	
	/**
	The number of activities which are represented by the subject, teacher and students requirements.
	*/
	int nActivities;
	
	/**
	The indices of the activities in the rules (from 0 to rules.nActivities-1)
	These are indices in the internal list -> Rules::internalActivitiesList
	*/
	QList<int> activitiesIndices;

	ConstraintActivitiesPreferredStartingTimes();

	//ConstraintActivitiesPreferredStartingTimes(double wp, const QString& te,
	//	const QString& st, const QString& su, const QString& sut, int nPT, int d[], int h[]);
	ConstraintActivitiesPreferredStartingTimes(double wp, const QString& te,
		const QString& st, const QString& su, const QString& sut, int dur, int nPT_L, QList<int> d_L, QList<int> h_L);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintSubactivitiesPreferredStartingTimes: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintSubactivitiesPreferredStartingTimes)

public:
	int componentNumber;

	/**
	The teacher. If void, all teachers.
	*/
	QString teacherName;

	/**
	The students. If void, all students.
	*/
	QString studentsName;

	/**
	The subject. If void, all subjects.
	*/
	QString subjectName;

	/**
	The activity tag. If void, all activity tags.
	*/
	QString activityTagName;

	int duration; //if -1, neglected. Otherwise, >=1.

	/**
	The number of preferred times
	*/
	int nPreferredStartingTimes_L;

	/**
	The preferred days. If -1, then the user does not care about the day.
	*/
	//int days[MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES];
	QList<int> days_L;

	/**
	The preferred hours. If -1, then the user does not care about the hour.
	*/
	//int hours[MAX_N_CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES];
	QList<int> hours_L;

	//internal variables
	
	/**
	The number of activities which are represented by the subject, teacher and students requirements.
	*/
	int nActivities;
	
	/**
	The indices of the activities in the rules (from 0 to rules.nActivities-1)
	These are indices in the internal list -> Rules::internalActivitiesList
	*/
	QList<int> activitiesIndices;

	ConstraintSubactivitiesPreferredStartingTimes();

	ConstraintSubactivitiesPreferredStartingTimes(double wp, int compNo, const QString& te,
		const QString& st, const QString& su, const QString& sut, int dur, int nPT_L, QList<int> d_L, QList<int> h_L);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesSameStartingHour: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesSameStartingHour)

public:
	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id-s)
	*/
	QList<int> activitiesIds;
	//int activitiesIds[MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR];

	QSet<int> activitiesIdsSet;

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (index in the rules) - internal structure
	*/
	QList<int> _activities;
	//int _activities[MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR];

	ConstraintActivitiesSameStartingHour();

	/**
	Constructor, using:
	the weight, the number of activities and the list of activities' id-s.
	*/
	//ConstraintActivitiesSameStartingHour(double wp, int n_act, const int act[]);
	ConstraintActivitiesSameStartingHour(double wp, int n_act, const QList<int>& act);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesSameStartingDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesSameStartingDay)

public:
	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id-s)
	*/
	QList<int> activitiesIds;
	//int activitiesIds[MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY];

	QSet<int> activitiesIdsSet;

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (index in the rules) - internal structure
	*/
	//int _activities[MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY];
	QList<int> _activities;

	ConstraintActivitiesSameStartingDay();

	/**
	Constructor, using:
	the weight, the number of activities and the list of activities' id-s.
	*/
	//ConstraintActivitiesSameStartingDay(double wp, int n_act, const int act[]);
	ConstraintActivitiesSameStartingDay(double wp, int n_act, const QList<int>& act);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTwoActivitiesConsecutive: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTwoActivitiesConsecutive)

public:
	/**
	First activity id
	*/
	int firstActivityId;

	/**
	Second activity id
	*/
	int secondActivityId;

	//internal variables
	/**
	The index of the first activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int firstActivityIndex;

	/**
	The index of the second activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int secondActivityIndex;

	ConstraintTwoActivitiesConsecutive();

	ConstraintTwoActivitiesConsecutive(double wp, int firstActId, int secondActId);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTwoActivitiesGrouped: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTwoActivitiesGrouped)

public:
	/**
	First activity id
	*/
	int firstActivityId;

	/**
	Second activity id
	*/
	int secondActivityId;

	//internal variables
	/**
	The index of the first activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int firstActivityIndex;

	/**
	The index of the second activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int secondActivityIndex;

	ConstraintTwoActivitiesGrouped();

	ConstraintTwoActivitiesGrouped(double wp, int firstActId, int secondActId);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintThreeActivitiesGrouped: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintThreeActivitiesGrouped)

public:
	/**
	First activity id
	*/
	int firstActivityId;

	/**
	Second activity id
	*/
	int secondActivityId;

	int thirdActivityId;

	//internal variables
	/**
	The index of the first activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int firstActivityIndex;

	/**
	The index of the second activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int secondActivityIndex;

	int thirdActivityIndex;

	ConstraintThreeActivitiesGrouped();

	ConstraintThreeActivitiesGrouped(double wp, int firstActId, int secondActId, int thirdActId);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTwoActivitiesOrdered: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTwoActivitiesOrdered)

public:
	/**
	First activity id
	*/
	int firstActivityId;

	/**
	Second activity id
	*/
	int secondActivityId;

	//internal variables
	/**
	The index of the first activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int firstActivityIndex;

	/**
	The index of the second activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int secondActivityIndex;

	ConstraintTwoActivitiesOrdered();

	ConstraintTwoActivitiesOrdered(double wp, int firstActId, int secondActId);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTwoSetsOfActivitiesOrdered: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTwoSetsOfActivitiesOrdered)

public:
	QList<int> firstActivitiesIdsList;

	QList<int> secondActivitiesIdsList;

	QSet<int> firstActivitiesIdsSet;

	QSet<int> secondActivitiesIdsSet;

	//internal variables
	QList<int> firstActivitiesIndicesList;

	QList<int> secondActivitiesIndicesList;

	ConstraintTwoSetsOfActivitiesOrdered();

	ConstraintTwoSetsOfActivitiesOrdered(double wp, const QList<int>& firstActsIds, const QList<int>& secondActsIds);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules &r);

	void recomputeActivitiesSets();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTwoActivitiesOrderedIfSameDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTwoActivitiesOrderedIfSameDay)

public:
	/**
	First activity id
	*/
	int firstActivityId;

	/**
	Second activity id
	*/
	int secondActivityId;

	//internal variables
	/**
	The index of the first activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int firstActivityIndex;

	/**
	The index of the second activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int secondActivityIndex;

	ConstraintTwoActivitiesOrderedIfSameDay();

	ConstraintTwoActivitiesOrderedIfSameDay(double wp, int firstActId, int secondActId);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivityEndsStudentsDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityEndsStudentsDay)

public:
	/**
	Activity id
	*/
	int activityId;

	//internal variables
	/**
	The index of the activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int activityIndex;

	ConstraintActivityEndsStudentsDay();

	ConstraintActivityEndsStudentsDay(double wp, int actId);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinHoursDaily: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinHoursDaily)

public:
	/**
	The minimum hours daily
	*/
	int minHoursDaily;
	
	bool allowEmptyDays;

	ConstraintTeachersMinHoursDaily();

	ConstraintTeachersMinHoursDaily(double wp, int minhours, bool _allowEmptyDays);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinHoursDaily: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinHoursDaily)

public:
	/**
	The minimum hours daily
	*/
	int minHoursDaily;
	
	QString teacherName;
	
	int teacher_ID;
	
	bool allowEmptyDays;

	ConstraintTeacherMinHoursDaily();

	ConstraintTeacherMinHoursDaily(double wp, int minhours, const QString& teacher, bool _allowEmptyDays);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherIntervalMaxDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherIntervalMaxDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week
	*/
	int maxDaysPerWeek;
	
	int startHour;

	int endHour; //might be = to gt.rules.nHoursPerDay

	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherIntervalMaxDaysPerWeek();

	ConstraintTeacherIntervalMaxDaysPerWeek(double wp, int maxnd, const QString& tn, int sh, int eh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersIntervalMaxDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersIntervalMaxDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week
	*/
	int maxDaysPerWeek;
	
	int startHour;

	int endHour; //might be = to gt.rules.nHoursPerDay

	ConstraintTeachersIntervalMaxDaysPerWeek();

	ConstraintTeachersIntervalMaxDaysPerWeek(double wp, int maxnd, int sh, int eh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetIntervalMaxDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetIntervalMaxDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week
	*/
	int maxDaysPerWeek;
	
	int startHour;

	int endHour; //might be = to gt.rules.nHoursPerDay

	/**
	The name of the students set for this constraint
	*/
	QString students;

	//internal redundant data

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	QList<int> iSubgroupsList;

	ConstraintStudentsSetIntervalMaxDaysPerWeek();

	ConstraintStudentsSetIntervalMaxDaysPerWeek(double wp, int maxnd, const QString& sn, int sh, int eh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsIntervalMaxDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsIntervalMaxDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week
	*/
	int maxDaysPerWeek;
	
	int startHour;

	int endHour; //might be = to gt.rules.nHoursPerDay

	ConstraintStudentsIntervalMaxDaysPerWeek();

	ConstraintStudentsIntervalMaxDaysPerWeek(double wp, int maxnd, int sh, int eh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesEndStudentsDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesEndStudentsDay)

public:
	/**
	The teacher. If void, all teachers.
	*/
	QString teacherName;

	/**
	The students. If void, all students.
	*/
	QString studentsName;

	/**
	The subject. If void, all subjects.
	*/
	QString subjectName;

	/**
	The activity tag. If void, all activity tags.
	*/
	QString activityTagName;
	
	
	//internal data

	/**
	The number of activities which are represented by the subject, teacher and students requirements.
	*/
	int nActivities;
	
	/**
	The indices of the activities in the rules (from 0 to rules.nActivities-1)
	These are indices in the internal list -> Rules::internalActivitiesList
	*/
	QList<int> activitiesIndices;

	ConstraintActivitiesEndStudentsDay();

	ConstraintActivitiesEndStudentsDay(double wp, const QString& te, const QString& st, const QString& su, const QString& sut);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivityEndsTeachersDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityEndsTeachersDay)

public:
	/**
	Activity id
	*/
	int activityId;

	//internal variables
	/**
	The index of the activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int activityIndex;

	ConstraintActivityEndsTeachersDay();

	ConstraintActivityEndsTeachersDay(double wp, int actId);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesEndTeachersDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesEndTeachersDay)

public:
	/**
	The teacher. If void, all teachers.
	*/
	QString teacherName;

	/**
	The students. If void, all students.
	*/
	QString studentsName;

	/**
	The subject. If void, all subjects.
	*/
	QString subjectName;

	/**
	The activity tag. If void, all activity tags.
	*/
	QString activityTagName;
	
	
	//internal data

	/**
	The number of activities which are represented by the subject, teacher and students requirements.
	*/
	int nActivities;
	
	/**
	The indices of the activities in the rules (from 0 to rules.nActivities-1)
	These are indices in the internal list -> Rules::internalActivitiesList
	*/
	//int activitiesIndices[MAX_ACTIVITIES];
	QList<int> activitiesIndices;

	ConstraintActivitiesEndTeachersDay();

	ConstraintActivitiesEndTeachersDay(double wp, const QString& te, const QString& st, const QString& su, const QString& sut);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersActivityTagMaxHoursDaily: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersActivityTagMaxHoursDaily)

public:
	/**
	The maximum hours daily
	*/
	int maxHoursDaily;
	
	QString activityTagName;
	
	int activityTagIndex;
	
	QList<int> canonicalTeachersList;

	ConstraintTeachersActivityTagMaxHoursDaily();

	ConstraintTeachersActivityTagMaxHoursDaily(double wp, int maxhours, const QString& activityTag);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherActivityTagMaxHoursDaily: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherActivityTagMaxHoursDaily)

public:
	/**
	The maximum hours daily
	*/
	int maxHoursDaily;
	
	QString teacherName;
	
	QString activityTagName;
	
	int teacher_ID;

	int activityTagIndex;
	
	QList<int> canonicalTeachersList;

	ConstraintTeacherActivityTagMaxHoursDaily();

	ConstraintTeacherActivityTagMaxHoursDaily(double wp, int maxhours, const QString& teacher, const QString& activityTag);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsActivityTagMaxHoursDaily: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsActivityTagMaxHoursDaily)

public:
	int maxHoursDaily;
	
	QString activityTagName;
	
	int activityTagIndex;
	
	QList<int> canonicalSubgroupsList;

	ConstraintStudentsActivityTagMaxHoursDaily();

	ConstraintStudentsActivityTagMaxHoursDaily(double wp, int maxnh, const QString& activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetActivityTagMaxHoursDaily: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetActivityTagMaxHoursDaily)

public:
	int maxHoursDaily;

	/**
	The students set name
	*/
	QString students;
	
	QString activityTagName;

	//internal variables
	
	int activityTagIndex;

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;
	
	QList<int> canonicalSubgroupsList;

	ConstraintStudentsSetActivityTagMaxHoursDaily();

	ConstraintStudentsSetActivityTagMaxHoursDaily(double wp, int maxnh, const QString& s, const QString& activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersActivityTagMinHoursDaily: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersActivityTagMinHoursDaily)

public:
	/**
	The minimum hours daily
	*/
	int minHoursDaily;
	
	int minDaysWithTag;
	
	QString activityTagName;
	
	int activityTagIndex;
	
	QList<int> canonicalTeachersList;

	ConstraintTeachersActivityTagMinHoursDaily();

	ConstraintTeachersActivityTagMinHoursDaily(double wp, int minhours, int mindays, const QString& activityTag);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherActivityTagMinHoursDaily: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherActivityTagMinHoursDaily)

public:
	/**
	The minimum hours daily
	*/
	int minHoursDaily;
	
	int minDaysWithTag;
	
	QString teacherName;
	
	QString activityTagName;
	
	int teacher_ID;

	int activityTagIndex;
	
	QList<int> canonicalTeachersList;

	ConstraintTeacherActivityTagMinHoursDaily();

	ConstraintTeacherActivityTagMinHoursDaily(double wp, int minhours, int mindays, const QString& teacher, const QString& activityTag);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsActivityTagMinHoursDaily: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsActivityTagMinHoursDaily)

public:
	int minHoursDaily;
	
	int minDaysWithTag;
	
	QString activityTagName;
	
	int activityTagIndex;
	
	QList<int> canonicalSubgroupsList;

	ConstraintStudentsActivityTagMinHoursDaily();

	ConstraintStudentsActivityTagMinHoursDaily(double wp, int maxnh, int mindays, const QString& activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetActivityTagMinHoursDaily: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetActivityTagMinHoursDaily)

public:
	int minHoursDaily;
	
	int minDaysWithTag;

	/**
	The students set name
	*/
	QString students;
	
	QString activityTagName;

	//internal variables
	
	int activityTagIndex;

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;
	
	QList<int> canonicalSubgroupsList;

	ConstraintStudentsSetActivityTagMinHoursDaily();

	ConstraintStudentsSetActivityTagMinHoursDaily(double wp, int minnh, int mindays, const QString& s, const QString& activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxGapsPerDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxGapsPerDay)

public:
	int maxGaps;

	ConstraintStudentsMaxGapsPerDay();

	ConstraintStudentsMaxGapsPerDay(double wp, int mg);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxGapsPerDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxGapsPerDay)

public:
	int maxGaps;

	/**
	The name of the students set for this constraint
	*/
	QString students;

	//internal redundant data

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMaxGapsPerDay();

	ConstraintStudentsSetMaxGapsPerDay(double wp, int mg, const QString& st);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesOccupyMaxTimeSlotsFromSelection: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesOccupyMaxTimeSlotsFromSelection)

public:
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;
	
	QList<int> selectedDays;
	QList<int> selectedHours;
	
	int maxOccupiedTimeSlots;

	//internal variables
	QList<int> _activitiesIndices;

	ConstraintActivitiesOccupyMaxTimeSlotsFromSelection();

	ConstraintActivitiesOccupyMaxTimeSlotsFromSelection(double wp, const QList<int>& a_L, const QList<int>& d_L, const QList<int>& h_L, int max_slots);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesOccupyMinTimeSlotsFromSelection: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesOccupyMinTimeSlotsFromSelection)

public:
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;
	
	QList<int> selectedDays;
	QList<int> selectedHours;
	
	int minOccupiedTimeSlots;

	//internal variables
	QList<int> _activitiesIndices;

	ConstraintActivitiesOccupyMinTimeSlotsFromSelection();

	ConstraintActivitiesOccupyMinTimeSlotsFromSelection(double wp, const QList<int>& a_L, const QList<int>& d_L, const QList<int>& h_L, int min_slots);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots)

public:
	QList<int> activitiesIds;
	
	QSet<int> activitiesIdsSet;
	
	QList<int> selectedDays;
	QList<int> selectedHours;
	
	int maxSimultaneous;

	//internal variables
	QList<int> _activitiesIndices;

	ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots();

	ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots(double wp, const QList<int>& a_L, const QList<int>& d_L, const QList<int>& h_L, int max_simultaneous);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesMinSimultaneousInSelectedTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesMinSimultaneousInSelectedTimeSlots)

public:
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;
	
	QList<int> selectedDays;
	QList<int> selectedHours;
	
	int minSimultaneous;
	
	bool allowEmptySlots;

	//internal variables
	QList<int> _activitiesIndices;

	ConstraintActivitiesMinSimultaneousInSelectedTimeSlots();

	ConstraintActivitiesMinSimultaneousInSelectedTimeSlots(double wp, const QList<int>& a_L, const QList<int>& d_L, const QList<int>& h_L, int min_simultaneous, bool allow_empty_slots);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots)

public:
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;

	QList<int> selectedDays;
	QList<int> selectedHours;

	int maxActivities;

	//internal variables
	QList<int> _activitiesIndices;

	ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots();

	ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots(double wp, const QList<int>& a_L, const QList<int>& d_L, const QList<int>& h_L, int max_activities);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesMaxInATerm: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesMaxInATerm)

public:
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;

	int maxActivitiesInATerm;

	//internal variables
	QList<int> _activitiesIndices;

	ConstraintActivitiesMaxInATerm();

	ConstraintActivitiesMaxInATerm(double wp, const QList<int>& a_L, int max_acts);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesOccupyMaxTerms: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesOccupyMaxTerms)

public:
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;

	int maxOccupiedTerms;

	//internal variables
	QList<int> _activitiesIndices;

	ConstraintActivitiesOccupyMaxTerms();

	ConstraintActivitiesOccupyMaxTerms(double wp, const QList<int>& a_L, int max_occupied);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week
	*/
	int maxDaysPerWeek;
	
	/**
	The name of the students set for this constraint
	*/
	QString students;

	//internal redundant data

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMaxDaysPerWeek();

	ConstraintStudentsSetMaxDaysPerWeek(double wp, int maxnd, const QString& sn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week
	*/
	int maxDaysPerWeek;
	
	ConstraintStudentsMaxDaysPerWeek();

	ConstraintStudentsMaxDaysPerWeek(double wp, int maxnd);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxSpanPerDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxSpanPerDay)

public:
	/**
	The maximum span daily
	*/
	int maxSpanPerDay;
	
	bool allowOneDayExceptionPlusOne;
	
	QString teacherName;
	
	int teacher_ID;

	ConstraintTeacherMaxSpanPerDay();

	ConstraintTeacherMaxSpanPerDay(double wp, int maxspan, bool except, const QString& teacher);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxSpanPerDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxSpanPerDay)

public:
	/**
	The maximum span daily
	*/
	int maxSpanPerDay;
	
	bool allowOneDayExceptionPlusOne;

	ConstraintTeachersMaxSpanPerDay();

	ConstraintTeachersMaxSpanPerDay(double wp, int maxspan, bool except);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinRestingHours: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinRestingHours)

public:
	int minRestingHours;
	
	bool circular;
	
	QString teacherName;
	
	int teacher_ID;

	ConstraintTeacherMinRestingHours();

	ConstraintTeacherMinRestingHours(double wp, int minrestinghours, bool circ, const QString& teacher);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinRestingHours: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinRestingHours)

public:
	int minRestingHours;
	
	bool circular;
	
	ConstraintTeachersMinRestingHours();

	ConstraintTeachersMinRestingHours(double wp, int minrestinghours, bool circ);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxSpanPerDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxSpanPerDay)

public:
	int maxSpanPerDay;
	
	/**
	The name of the students set for this constraint
	*/
	QString students;

	//internal redundant data

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMaxSpanPerDay();

	ConstraintStudentsSetMaxSpanPerDay(double wp, int maxspan, const QString& sn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxSpanPerDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxSpanPerDay)

public:
	int maxSpanPerDay;
	
	ConstraintStudentsMaxSpanPerDay();

	ConstraintStudentsMaxSpanPerDay(double wp, int maxspan);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMinRestingHours: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMinRestingHours)

public:
	int minRestingHours;
	
	bool circular;
	
	/**
	The name of the students set for this constraint
	*/
	QString students;

	//internal redundant data

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMinRestingHours();

	ConstraintStudentsSetMinRestingHours(double wp, int minrestinghours, bool circ, const QString& sn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMinRestingHours: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMinRestingHours)

public:
	int minRestingHours;
	
	bool circular;
	
	ConstraintStudentsMinRestingHours();

	ConstraintStudentsMinRestingHours(double wp, int minrestinghours, bool circ);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags)

public:
	int minGaps;

	QString students;
	
	QString firstActivityTag;
	QString secondActivityTag;

	//internal variables

	QList<int> canonicalSubgroupsList;
	
	int _firstActivityTagIndex;
	int _secondActivityTagIndex;

	ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags();

	ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags(double wp, const QString& _students, int _minGaps, const QString& _firstActivityTag, const QString& _secondActivityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags)

public:
	int minGaps;

	QString firstActivityTag;
	QString secondActivityTag;

	//internal variables

	QList<int> canonicalSubgroupsList;

	int _firstActivityTagIndex;
	int _secondActivityTagIndex;

	ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags();

	ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags(double wp, int _minGaps, const QString& _firstActivityTag, const QString& _secondActivityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags)

public:
	int minGaps;

	QString teacher;
	
	QString firstActivityTag;
	QString secondActivityTag;

	//internal variables
	
	QList<int> canonicalTeachersList;

	int _firstActivityTagIndex;
	int _secondActivityTagIndex;

	ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags();

	ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags(double wp, const QString& _teacher, int _minGaps, const QString& _firstActivityTag, const QString& _secondActivityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags)

public:
	int minGaps;

	QString firstActivityTag;
	QString secondActivityTag;

	//internal variables

	QList<int> canonicalTeachersList;

	int _firstActivityTagIndex;
	int _secondActivityTagIndex;

	ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags();

	ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags(double wp, int _minGaps, const QString& _firstActivityTag, const QString& _secondActivityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMinGapsBetweenActivityTag: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMinGapsBetweenActivityTag)

public:
	int minGaps;

	QString students;
	
	QString activityTag;

	//internal variables

	QList<int> canonicalSubgroupsList;
	
	int _activityTagIndex;

	ConstraintStudentsSetMinGapsBetweenActivityTag();

	ConstraintStudentsSetMinGapsBetweenActivityTag(double wp, const QString& _students, int _minGaps, const QString& _activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMinGapsBetweenActivityTag: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMinGapsBetweenActivityTag)

public:
	int minGaps;

	QString activityTag;

	//internal variables

	QList<int> canonicalSubgroupsList;

	int _activityTagIndex;

	ConstraintStudentsMinGapsBetweenActivityTag();

	ConstraintStudentsMinGapsBetweenActivityTag(double wp, int _minGaps, const QString& _activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinGapsBetweenActivityTag: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinGapsBetweenActivityTag)

public:
	int minGaps;

	QString teacher;
	
	QString activityTag;

	//internal variables
	
	QList<int> canonicalTeachersList;

	int _activityTagIndex;

	ConstraintTeacherMinGapsBetweenActivityTag();

	ConstraintTeacherMinGapsBetweenActivityTag(double wp, const QString& _teacher, int _minGaps, const QString& _activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinGapsBetweenActivityTag: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinGapsBetweenActivityTag)

public:
	int minGaps;

	QString activityTag;

	//internal variables

	QList<int> canonicalTeachersList;

	int _activityTagIndex;

	ConstraintTeachersMinGapsBetweenActivityTag();

	ConstraintTeachersMinGapsBetweenActivityTag(double wp, int _minGaps, const QString& _activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay)

public:
	int minGaps;

	QString students;
	
	QString firstActivityTag;
	QString secondActivityTag;

	//internal variables

	QList<int> canonicalSubgroupsList;
	
	int _firstActivityTagIndex;
	int _secondActivityTagIndex;

	ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay();

	ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay(double wp, const QString& _students, int _minGaps, const QString& _firstActivityTag, const QString& _secondActivityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay)

public:
	int minGaps;

	QString firstActivityTag;
	QString secondActivityTag;

	//internal variables

	QList<int> canonicalSubgroupsList;

	int _firstActivityTagIndex;
	int _secondActivityTagIndex;

	ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay();

	ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay(double wp, int _minGaps, const QString& _firstActivityTag, const QString& _secondActivityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay)

public:
	int minGaps;

	QString teacher;
	
	QString firstActivityTag;
	QString secondActivityTag;

	//internal variables
	
	QList<int> canonicalTeachersList;

	int _firstActivityTagIndex;
	int _secondActivityTagIndex;

	ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay();

	ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay(double wp, const QString& _teacher, int _minGaps, const QString& _firstActivityTag, const QString& _secondActivityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay)

public:
	int minGaps;

	QString firstActivityTag;
	QString secondActivityTag;

	//internal variables

	QList<int> canonicalTeachersList;

	int _firstActivityTagIndex;
	int _secondActivityTagIndex;

	ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay();

	ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay(double wp, int _minGaps, const QString& _firstActivityTag, const QString& _secondActivityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMinGapsBetweenActivityTagPerRealDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMinGapsBetweenActivityTagPerRealDay)

public:
	int minGaps;

	QString students;
	
	QString activityTag;

	//internal variables

	QList<int> canonicalSubgroupsList;
	
	int _activityTagIndex;

	ConstraintStudentsSetMinGapsBetweenActivityTagPerRealDay();

	ConstraintStudentsSetMinGapsBetweenActivityTagPerRealDay(double wp, const QString& _students, int _minGaps, const QString& _activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMinGapsBetweenActivityTagPerRealDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMinGapsBetweenActivityTagPerRealDay)

public:
	int minGaps;

	QString activityTag;

	//internal variables

	QList<int> canonicalSubgroupsList;

	int _activityTagIndex;

	ConstraintStudentsMinGapsBetweenActivityTagPerRealDay();

	ConstraintStudentsMinGapsBetweenActivityTagPerRealDay(double wp, int _minGaps, const QString& _activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinGapsBetweenActivityTagPerRealDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinGapsBetweenActivityTagPerRealDay)

public:
	int minGaps;

	QString teacher;
	
	QString activityTag;

	//internal variables
	
	QList<int> canonicalTeachersList;

	int _activityTagIndex;

	ConstraintTeacherMinGapsBetweenActivityTagPerRealDay();

	ConstraintTeacherMinGapsBetweenActivityTagPerRealDay(double wp, const QString& _teacher, int _minGaps, const QString& _activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinGapsBetweenActivityTagPerRealDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinGapsBetweenActivityTagPerRealDay)

public:
	int minGaps;

	QString activityTag;

	//internal variables

	QList<int> canonicalTeachersList;

	int _activityTagIndex;

	ConstraintTeachersMinGapsBetweenActivityTagPerRealDay();

	ConstraintTeachersMinGapsBetweenActivityTagPerRealDay(double wp, int _minGaps, const QString& _activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon)

public:
	int minGaps;

	QString students;
	
	QString firstActivityTag;
	QString secondActivityTag;

	//internal variables

	QList<int> canonicalSubgroupsList;
	
	int _firstActivityTagIndex;
	int _secondActivityTagIndex;

	ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon();

	ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(double wp, const QString& _students, int _minGaps, const QString& _firstActivityTag, const QString& _secondActivityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon)

public:
	int minGaps;

	QString firstActivityTag;
	QString secondActivityTag;

	//internal variables

	QList<int> canonicalSubgroupsList;

	int _firstActivityTagIndex;
	int _secondActivityTagIndex;

	ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon();

	ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(double wp, int _minGaps, const QString& _firstActivityTag, const QString& _secondActivityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon)

public:
	int minGaps;

	QString teacher;
	
	QString firstActivityTag;
	QString secondActivityTag;

	//internal variables
	
	QList<int> canonicalTeachersList;

	int _firstActivityTagIndex;
	int _secondActivityTagIndex;

	ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon();

	ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(double wp, const QString& _teacher, int _minGaps, const QString& _firstActivityTag, const QString& _secondActivityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon)

public:
	int minGaps;

	QString firstActivityTag;
	QString secondActivityTag;

	//internal variables

	QList<int> canonicalTeachersList;

	int _firstActivityTagIndex;
	int _secondActivityTagIndex;

	ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon();

	ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(double wp, int _minGaps, const QString& _firstActivityTag, const QString& _secondActivityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon)

public:
	int minGaps;

	QString students;
	
	QString activityTag;

	//internal variables

	QList<int> canonicalSubgroupsList;
	
	int _activityTagIndex;

	ConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon();

	ConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon(double wp, const QString& _students, int _minGaps, const QString& _activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon)

public:
	int minGaps;

	QString activityTag;

	//internal variables

	QList<int> canonicalSubgroupsList;

	int _activityTagIndex;

	ConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon();

	ConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon(double wp, int _minGaps, const QString& _activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon)

public:
	int minGaps;

	QString teacher;
	
	QString activityTag;

	//internal variables
	
	QList<int> canonicalTeachersList;

	int _activityTagIndex;

	ConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon();

	ConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon(double wp, const QString& _teacher, int _minGaps, const QString& _activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon)

public:
	int minGaps;

	QString activityTag;

	//internal variables

	QList<int> canonicalTeachersList;

	int _activityTagIndex;

	ConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon();

	ConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon(double wp, int _minGaps, const QString& _activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//Begin for mornings-afternoons
class ConstraintTeachersMaxHoursDailyRealDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxHoursDailyRealDays)

public:
	/**
	The maximum hours daily
	*/
	int maxHoursDaily;

	ConstraintTeachersMaxHoursDailyRealDays();

	ConstraintTeachersMaxHoursDailyRealDays(double wp, int maxhours);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxHoursDailyRealDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxHoursDailyRealDays)

public:
	/**
	The maximum hours daily
	*/
	int maxHoursDaily;

	QString teacherName;

	int teacher_ID;

	ConstraintTeacherMaxHoursDailyRealDays();

	ConstraintTeacherMaxHoursDailyRealDays(double wp, int maxhours, const QString& teacher);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint.
The resulting timetable must respect the requirement
that this teacher must not have too much working
days per week.
*/
class ConstraintTeacherMaxRealDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxRealDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week (-1 for don't care)
	*/
	int maxDaysPerWeek;

	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherMaxRealDaysPerWeek();

	ConstraintTeacherMaxRealDaysPerWeek(double wp, int maxnd, const QString& tn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxRealDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxRealDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week (-1 for don't care)
	*/
	int maxDaysPerWeek;

	ConstraintTeachersMaxRealDaysPerWeek();

	ConstraintTeachersMaxRealDaysPerWeek(double wp, int maxnd);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinRealDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinRealDaysPerWeek)

public:
	int minDaysPerWeek;

	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherMinRealDaysPerWeek();

	ConstraintTeacherMinRealDaysPerWeek(double wp, int mindays, const QString& t);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinRealDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinRealDaysPerWeek)

public:
	int minDaysPerWeek;

	ConstraintTeachersMinRealDaysPerWeek();

	ConstraintTeachersMinRealDaysPerWeek(double wp, int mindays);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxGapsPerRealDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxGapsPerRealDay)

public:
	int maxGaps;

	bool allowOneDayExceptionPlusOne;

	ConstraintTeachersMaxGapsPerRealDay();

	ConstraintTeachersMaxGapsPerRealDay(double wp, int maxGaps, bool except);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxGapsPerRealDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxGapsPerRealDay)

public:
	int maxGaps;

	bool allowOneDayExceptionPlusOne;

	QString teacherName;

	int teacherIndex;

	ConstraintTeacherMaxGapsPerRealDay();

	ConstraintTeacherMaxGapsPerRealDay(double wp, const QString& tn, int maxGaps, bool except);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxHoursDailyRealDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxHoursDailyRealDays)

public:
	int maxHoursDaily;

	ConstraintStudentsMaxHoursDailyRealDays();

	ConstraintStudentsMaxHoursDailyRealDays(double wp, int maxnh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxHoursDailyRealDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxHoursDailyRealDays)

public:
	int maxHoursDaily;

	/**
	The students set name
	*/
	QString students;

	//internal variables

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMaxHoursDailyRealDays();

	ConstraintStudentsSetMaxHoursDailyRealDays(double wp, int maxnh, const QString& s);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinHoursPerMorning: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinHoursPerMorning)

public:
	/**
	The minimum hours per morning
	*/
	int minHoursPerMorning;

	bool allowEmptyMornings;

	ConstraintTeachersMinHoursPerMorning();

	ConstraintTeachersMinHoursPerMorning(double wp, int minhours, bool _allowEmptyMornings);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinHoursPerMorning: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinHoursPerMorning)

public:
	/**
	The minimum hours per morning
	*/
	int minHoursPerMorning;

	QString teacherName;

	int teacher_ID;

	bool allowEmptyMornings;

	ConstraintTeacherMinHoursPerMorning();

	ConstraintTeacherMinHoursPerMorning(double wp, int minhours, const QString& teacher, bool _allowEmptyMornings);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinHoursDailyRealDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinHoursDailyRealDays)

public:
	/**
	The minimum hours daily
	*/
	int minHoursDaily;

	bool allowEmptyDays;

	ConstraintTeachersMinHoursDailyRealDays();

	ConstraintTeachersMinHoursDailyRealDays(double wp, int minhours, bool _allowEmptyDays);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinHoursDailyRealDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinHoursDailyRealDays)

public:
	/**
	The minimum hours daily
	*/
	int minHoursDaily;

	QString teacherName;

	int teacher_ID;

	bool allowEmptyDays;

	ConstraintTeacherMinHoursDailyRealDays();

	ConstraintTeacherMinHoursDailyRealDays(double wp, int minhours, const QString& teacher, bool _allowEmptyDays);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//morning
class ConstraintTeacherMorningIntervalMaxDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMorningIntervalMaxDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week (mornings)
	*/
	int maxDaysPerWeek;

	int startHour;

	int endHour; //might be = to gt.rules.nHoursPerDay

	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherMorningIntervalMaxDaysPerWeek();

	ConstraintTeacherMorningIntervalMaxDaysPerWeek(double wp, int maxnd, const QString& tn, int sh, int eh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMorningIntervalMaxDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMorningIntervalMaxDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week (mornings)
	*/
	int maxDaysPerWeek;

	int startHour;

	int endHour; //might be = to gt.rules.nHoursPerDay

	ConstraintTeachersMorningIntervalMaxDaysPerWeek();

	ConstraintTeachersMorningIntervalMaxDaysPerWeek(double wp, int maxnd, int sh, int eh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//afternoon
class ConstraintTeacherAfternoonIntervalMaxDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherAfternoonIntervalMaxDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week (afternoons)
	*/
	int maxDaysPerWeek;

	int startHour;

	int endHour; //might be = to gt.rules.nHoursPerDay

	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherAfternoonIntervalMaxDaysPerWeek();

	ConstraintTeacherAfternoonIntervalMaxDaysPerWeek(double wp, int maxnd, const QString& tn, int sh, int eh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersAfternoonIntervalMaxDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersAfternoonIntervalMaxDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week (afternoons)
	*/
	int maxDaysPerWeek;

	int startHour;

	int endHour; //might be = to gt.rules.nHoursPerDay

	ConstraintTeachersAfternoonIntervalMaxDaysPerWeek();

	ConstraintTeachersAfternoonIntervalMaxDaysPerWeek(double wp, int maxnd, int sh, int eh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersActivityTagMaxHoursDailyRealDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersActivityTagMaxHoursDailyRealDays)

public:
	/**
	The maximum hours daily
	*/
	int maxHoursDaily;

	QString activityTagName;

	int activityTagIndex;

	QList<int> canonicalTeachersList;

	ConstraintTeachersActivityTagMaxHoursDailyRealDays();

	ConstraintTeachersActivityTagMaxHoursDailyRealDays(double wp, int maxhours, const QString& activityTag);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherActivityTagMaxHoursDailyRealDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherActivityTagMaxHoursDailyRealDays)

public:
	/**
	The maximum hours daily
	*/
	int maxHoursDaily;

	QString teacherName;

	QString activityTagName;

	int teacher_ID;

	int activityTagIndex;

	QList<int> canonicalTeachersList;

	ConstraintTeacherActivityTagMaxHoursDailyRealDays();

	ConstraintTeacherActivityTagMaxHoursDailyRealDays(double wp, int maxhours, const QString& teacher, const QString& activityTag);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsActivityTagMaxHoursDailyRealDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsActivityTagMaxHoursDailyRealDays)

public:
	int maxHoursDaily;

	QString activityTagName;

	int activityTagIndex;

	QList<int> canonicalSubgroupsList;

	ConstraintStudentsActivityTagMaxHoursDailyRealDays();

	ConstraintStudentsActivityTagMaxHoursDailyRealDays(double wp, int maxnh, const QString& activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetActivityTagMaxHoursDailyRealDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetActivityTagMaxHoursDailyRealDays)

public:
	int maxHoursDaily;

	/**
	The students set name
	*/
	QString students;

	QString activityTagName;

	//internal variables

	int activityTagIndex;

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;

	QList<int> canonicalSubgroupsList;

	ConstraintStudentsSetActivityTagMaxHoursDailyRealDays();

	ConstraintStudentsSetActivityTagMaxHoursDailyRealDays(double wp, int maxnh, const QString& s, const QString& activityTag);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxGapsPerRealDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxGapsPerRealDay)

public:
	int maxGaps;

	ConstraintStudentsMaxGapsPerRealDay();

	ConstraintStudentsMaxGapsPerRealDay(double wp, int mg);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxGapsPerRealDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxGapsPerRealDay)

public:
	int maxGaps;

	/**
	The name of the students set for this constraint
	*/
	QString students;

	//internal redundant data

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMaxGapsPerRealDay();

	ConstraintStudentsSetMaxGapsPerRealDay(double wp, int mg, const QString& st );

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxRealDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxRealDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week
	*/
	int maxDaysPerWeek;

	/**
	The name of the students set for this constraint
	*/
	QString students;

	//internal redundant data

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMaxRealDaysPerWeek();

	ConstraintStudentsSetMaxRealDaysPerWeek(double wp, int maxnd, const QString& sn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxRealDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxRealDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week
	*/
	int maxDaysPerWeek;

	ConstraintStudentsMaxRealDaysPerWeek();

	ConstraintStudentsMaxRealDaysPerWeek(double wp, int maxnd);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxSpanPerRealDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxSpanPerRealDay)

public:
	/**
	The maximum span daily
	*/
	int maxSpanPerDay;

	bool allowOneDayExceptionPlusOne;

	QString teacherName;

	int teacher_ID;

	ConstraintTeacherMaxSpanPerRealDay();

	ConstraintTeacherMaxSpanPerRealDay(double wp, int maxspan, bool except, const QString& teacher);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxSpanPerRealDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxSpanPerRealDay)

public:
	/**
	The maximum span daily
	*/
	int maxSpanPerDay;

	bool allowOneDayExceptionPlusOne;

	ConstraintTeachersMaxSpanPerRealDay();

	ConstraintTeachersMaxSpanPerRealDay(double wp, int maxspan, bool except);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxSpanPerRealDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxSpanPerRealDay)

public:
	int maxSpanPerDay;

	/**
	The name of the students set for this constraint
	*/
	QString students;

	//internal redundant data

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMaxSpanPerRealDay();

	ConstraintStudentsSetMaxSpanPerRealDay(double wp, int maxspan, const QString& sn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxSpanPerRealDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxSpanPerRealDay)

public:
	int maxSpanPerDay;

	ConstraintStudentsMaxSpanPerRealDay();

	ConstraintStudentsMaxSpanPerRealDay(double wp, int maxspan);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxAfternoonsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxAfternoonsPerWeek)

public:
	/**
	The number of maximum allowed working days per week (-1 for don't care)
	*/
	int maxAfternoonsPerWeek;

	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherMaxAfternoonsPerWeek();

	ConstraintTeacherMaxAfternoonsPerWeek(double wp, int maxnd, const QString& tn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxAfternoonsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxAfternoonsPerWeek)

public:
	/**
	The number of maximum allowed working days per week (-1 for don't care)
	*/
	int maxAfternoonsPerWeek;

	ConstraintTeachersMaxAfternoonsPerWeek();

	ConstraintTeachersMaxAfternoonsPerWeek(double wp, int maxnd);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxMorningsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxMorningsPerWeek)

public:
	/**
	The number of maximum allowed working days per week (-1 for don't care)
	*/
	int maxMorningsPerWeek;

	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherMaxMorningsPerWeek();

	ConstraintTeacherMaxMorningsPerWeek(double wp, int maxnd, const QString& tn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxMorningsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxMorningsPerWeek)

public:
	/**
	The number of maximum allowed working days per week (-1 for don't care)
	*/
	int maxMorningsPerWeek;

	ConstraintTeachersMaxMorningsPerWeek();

	ConstraintTeachersMaxMorningsPerWeek(double wp, int maxnd);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxActivityTagsPerDayFromSet: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxActivityTagsPerDayFromSet)

public:

	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;
	
	int maxTags;
	
	QList<QString> tagsList;
	
	QSet<int> internalTagsSet;

	ConstraintTeacherMaxActivityTagsPerDayFromSet();

	ConstraintTeacherMaxActivityTagsPerDayFromSet(double wp, const QString& tn, int mtg, const QList<QString>& tgl);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxActivityTagsPerDayFromSet: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxActivityTagsPerDayFromSet)

public:

	int maxTags;
	
	QList<QString> tagsList;
	
	QSet<int> internalTagsSet;

	ConstraintTeachersMaxActivityTagsPerDayFromSet();

	ConstraintTeachersMaxActivityTagsPerDayFromSet(double wp, int mtg, const QList<QString>& tgl);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinMorningsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinMorningsPerWeek)

public:
	int minMorningsPerWeek;

	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherMinMorningsPerWeek();

	ConstraintTeacherMinMorningsPerWeek(double wp, int minmornings, const QString& teacher);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinMorningsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinMorningsPerWeek)

public:
	int minMorningsPerWeek;

	ConstraintTeachersMinMorningsPerWeek();

	ConstraintTeachersMinMorningsPerWeek(double wp, int minmornings);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinAfternoonsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinAfternoonsPerWeek)

public:
	int minAfternoonsPerWeek;

	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherMinAfternoonsPerWeek();

	ConstraintTeacherMinAfternoonsPerWeek(double wp, int minafternoons, const QString& teacher);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinAfternoonsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinAfternoonsPerWeek)

public:
	int minAfternoonsPerWeek;

	ConstraintTeachersMinAfternoonsPerWeek();

	ConstraintTeachersMinAfternoonsPerWeek(double wp, int minafternoons);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxTwoConsecutiveMornings: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxTwoConsecutiveMornings)

public:
	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherMaxTwoConsecutiveMornings();

	ConstraintTeacherMaxTwoConsecutiveMornings(double wp, const QString& tn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxTwoConsecutiveMornings: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxTwoConsecutiveMornings)

public:
	ConstraintTeachersMaxTwoConsecutiveMornings();

	ConstraintTeachersMaxTwoConsecutiveMornings(double wp);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxTwoConsecutiveAfternoons: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxTwoConsecutiveAfternoons)

public:
	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherMaxTwoConsecutiveAfternoons();

	ConstraintTeacherMaxTwoConsecutiveAfternoons(double wp, const QString& tn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxTwoConsecutiveAfternoons: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxTwoConsecutiveAfternoons)

public:
	ConstraintTeachersMaxTwoConsecutiveAfternoons();

	ConstraintTeachersMaxTwoConsecutiveAfternoons(double wp);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHour: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHour)

public:

	int maxBeginningsAtSecondHour;

	ConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHour();

	ConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHour(double wp, int mBSH);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour)

public:
	int maxBeginningsAtSecondHour;

	QString teacherName;

	//internal
	int teacherIndex;

	ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour();

	ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour(double wp, int mBSH, const QString& teacher);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHour: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHour)

public:

	int maxBeginningsAtSecondHour;

	ConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHour();

	ConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHour(double wp, int mBSH);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour)

public:
	int maxBeginningsAtSecondHour;

	QString teacherName;

	//internal
	int teacherIndex;

	ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour();

	ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour(double wp, int mBSH, const QString& teacher);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//2020-06-14
class ConstraintStudentsMinHoursPerMorning: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMinHoursPerMorning)

public:
	int minHoursPerMorning;

	bool allowEmptyMornings;

	ConstraintStudentsMinHoursPerMorning();

	ConstraintStudentsMinHoursPerMorning(double wp, int minnh, bool _allowEmptyMornings);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMinHoursPerMorning: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMinHoursPerMorning)

public:
	int minHoursPerMorning;

	/**
	The students set name
	*/
	QString students;

	bool allowEmptyMornings;

	//internal variables

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMinHoursPerMorning();

	ConstraintStudentsSetMinHoursPerMorning(double wp, int minnh, const QString& s, bool _allowEmptyMornings);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxZeroGapsPerAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxZeroGapsPerAfternoon)

public:
	QString teacherName;

	int teacherIndex;

	ConstraintTeacherMaxZeroGapsPerAfternoon();

	ConstraintTeacherMaxZeroGapsPerAfternoon(double wp, const QString& tn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxZeroGapsPerAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxZeroGapsPerAfternoon)

public:
	ConstraintTeachersMaxZeroGapsPerAfternoon();

	ConstraintTeachersMaxZeroGapsPerAfternoon(double wp);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//2020-06-25
class ConstraintStudentsMaxAfternoonsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxAfternoonsPerWeek)

public:
	int maxAfternoonsPerWeek;

	ConstraintStudentsMaxAfternoonsPerWeek();

	ConstraintStudentsMaxAfternoonsPerWeek(double wp, int maxnd);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxAfternoonsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxAfternoonsPerWeek)

public:
	int maxAfternoonsPerWeek;

	QString students;

	QList<int> iSubgroupsList;

	ConstraintStudentsSetMaxAfternoonsPerWeek();

	ConstraintStudentsSetMaxAfternoonsPerWeek(double wp, int maxnd, const QString& sn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxMorningsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxMorningsPerWeek)

public:
	int maxMorningsPerWeek;

	ConstraintStudentsMaxMorningsPerWeek();

	ConstraintStudentsMaxMorningsPerWeek(double wp, int maxnd);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxMorningsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxMorningsPerWeek)

public:
	int maxMorningsPerWeek;

	QString students;

	QList<int> iSubgroupsList;

	ConstraintStudentsSetMaxMorningsPerWeek();

	ConstraintStudentsSetMaxMorningsPerWeek(double wp, int maxnd, const QString& sn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//2020-06-26
class ConstraintStudentsMinAfternoonsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMinAfternoonsPerWeek)

public:
	int minAfternoonsPerWeek;

	ConstraintStudentsMinAfternoonsPerWeek();

	ConstraintStudentsMinAfternoonsPerWeek(double wp, int minnd);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMinAfternoonsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMinAfternoonsPerWeek)

public:
	int minAfternoonsPerWeek;

	QString students;

	QList<int> iSubgroupsList;

	ConstraintStudentsSetMinAfternoonsPerWeek();

	ConstraintStudentsSetMinAfternoonsPerWeek(double wp, int minnd, const QString& sn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMinMorningsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMinMorningsPerWeek)

public:
	int minMorningsPerWeek;

	ConstraintStudentsMinMorningsPerWeek();

	ConstraintStudentsMinMorningsPerWeek(double wp, int minnd);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMinMorningsPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMinMorningsPerWeek)

public:
	int minMorningsPerWeek;

	QString students;

	QList<int> iSubgroupsList;

	ConstraintStudentsSetMinMorningsPerWeek();

	ConstraintStudentsSetMinMorningsPerWeek(double wp, int minnd, const QString& sn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//2020-06-26
class ConstraintStudentsSetMorningIntervalMaxDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMorningIntervalMaxDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week
	*/
	int maxDaysPerWeek;

	int startHour;

	int endHour; //might be = to gt.rules.nHoursPerDay

	/**
	The name of the students set for this constraint
	*/
	QString students;

	//internal redundant data

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMorningIntervalMaxDaysPerWeek();

	ConstraintStudentsSetMorningIntervalMaxDaysPerWeek(double wp, int maxnd, const QString& sn, int sh, int eh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMorningIntervalMaxDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMorningIntervalMaxDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week
	*/
	int maxDaysPerWeek;

	int startHour;

	int endHour; //might be = to gt.rules.nHoursPerDay


	ConstraintStudentsMorningIntervalMaxDaysPerWeek();

	ConstraintStudentsMorningIntervalMaxDaysPerWeek(double wp, int maxnd, int sh, int eh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week
	*/
	int maxDaysPerWeek;

	int startHour;

	int endHour; //might be = to gt.rules.nHoursPerDay

	/**
	The name of the students set for this constraint
	*/
	QString students;

	//internal redundant data

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	QList<int> iSubgroupsList;

	ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek();

	ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek(double wp, int maxnd, const QString& sn, int sh, int eh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsAfternoonIntervalMaxDaysPerWeek: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsAfternoonIntervalMaxDaysPerWeek)

public:
	/**
	The number of maximum allowed working days per week
	*/
	int maxDaysPerWeek;

	int startHour;

	int endHour; //might be = to gt.rules.nHoursPerDay


	ConstraintStudentsAfternoonIntervalMaxDaysPerWeek();

	ConstraintStudentsAfternoonIntervalMaxDaysPerWeek(double wp, int maxnd, int sh, int eh);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//2020-06-28
class ConstraintTeacherMaxHoursPerAllAfternoons: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxHoursPerAllAfternoons)

public:
	int maxHoursPerAllAfternoons;

	QString teacherName;

	int teacher_ID;

	ConstraintTeacherMaxHoursPerAllAfternoons();

	ConstraintTeacherMaxHoursPerAllAfternoons(double wp, int maxhours, const QString& teacher);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxHoursPerAllAfternoons: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxHoursPerAllAfternoons)

public:
	int maxHoursPerAllAfternoons;

	ConstraintTeachersMaxHoursPerAllAfternoons();

	ConstraintTeachersMaxHoursPerAllAfternoons(double wp, int maxhours);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//2020-06-28
class ConstraintStudentsSetMaxHoursPerAllAfternoons: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxHoursPerAllAfternoons)

public:
	int maxHoursPerAllAfternoons;

	QString students;

	QList<int> iSubgroupsList;

	ConstraintStudentsSetMaxHoursPerAllAfternoons();

	ConstraintStudentsSetMaxHoursPerAllAfternoons(double wp, int maxhours, const QString& st);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxHoursPerAllAfternoons: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxHoursPerAllAfternoons)

public:
	int maxHoursPerAllAfternoons;

	ConstraintStudentsMaxHoursPerAllAfternoons();

	ConstraintStudentsMaxHoursPerAllAfternoons(double wp, int maxhours);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon)

public:
	int minRestingHours;

	QString teacherName;

	int teacher_ID;

	ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon();

	ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon(double wp, int minrestinghours, const QString& teacher);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinRestingHoursBetweenMorningAndAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinRestingHoursBetweenMorningAndAfternoon)

public:
	int minRestingHours;

	ConstraintTeachersMinRestingHoursBetweenMorningAndAfternoon();

	ConstraintTeachersMinRestingHoursBetweenMorningAndAfternoon(double wp, int minrestinghours);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon)

public:
	int minRestingHours;

	/**
	The name of the students set for this constraint
	*/
	QString students;

	//internal redundant data

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon();

	ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon(double wp, int minrestinghours, const QString& sn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon)

public:
	int minRestingHours;

	ConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon();

	ConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon(double wp, int minrestinghours);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHour: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHour)

public:

	int maxBeginningsAtSecondHour;

	ConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHour();

	ConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHour(double wp, int mBSH);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour)

public:
	int maxBeginningsAtSecondHour;

	/**
	The name of the students
	*/
	QString students;

	/**
	The number of subgroups involved in this constraint
	*/
	//int nSubgroups;

	/**
	The subgroups involved in this constraint
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;

	ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour();

	ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour(double wp, int mBSH, const QString& students);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour)

public:

	int maxBeginningsAtSecondHour;

	ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour();

	ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour(double wp, int mBSH);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour)

public:
	int maxBeginningsAtSecondHour;

	/**
	The name of the students
	*/
	QString students;

	/**
	The number of subgroups involved in this constraint
	*/
	//int nSubgroups;

	/**
	The subgroups involved in this constraint
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour();

	ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour(double wp, int mBSH, const QString& students);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//2020-07-29
class ConstraintTeachersMaxGapsPerWeekForRealDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxGapsPerWeekForRealDays)

public:
	int maxGaps;

	ConstraintTeachersMaxGapsPerWeekForRealDays();

	ConstraintTeachersMaxGapsPerWeekForRealDays(double wp, int maxGaps);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxGapsPerWeekForRealDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxGapsPerWeekForRealDays)

public:
	int maxGaps;

	QString teacherName;

	int teacherIndex;

	ConstraintTeacherMaxGapsPerWeekForRealDays();

	ConstraintTeacherMaxGapsPerWeekForRealDays(double wp, const QString& tn, int maxGaps);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxGapsPerWeekForRealDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxGapsPerWeekForRealDays)

public:
	int maxGaps;

	ConstraintStudentsMaxGapsPerWeekForRealDays();

	ConstraintStudentsMaxGapsPerWeekForRealDays(double wp, int mg);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxGapsPerWeekForRealDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxGapsPerWeekForRealDays)

public:
	int maxGaps;

	/**
	The name of the students set for this constraint
	*/
	QString students;

	//internal redundant data

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMaxGapsPerWeekForRealDays();

	ConstraintStudentsSetMaxGapsPerWeekForRealDays(double wp, int mg, const QString& st );

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};
//End for mornings-afternoons

//Begin mornings-afternoons 2021-09-26
class ConstraintTeacherMaxThreeConsecutiveDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxThreeConsecutiveDays)

public:
	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	bool allowAMAMException; //AMAM=afternoon morning afternoon morning.

	ConstraintTeacherMaxThreeConsecutiveDays();

	ConstraintTeacherMaxThreeConsecutiveDays(double wp, bool ae, const QString& tn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxThreeConsecutiveDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxThreeConsecutiveDays)

public:
	bool allowAMAMException; //AMAM=afternoon morning afternoon morning.

	ConstraintTeachersMaxThreeConsecutiveDays();

	ConstraintTeachersMaxThreeConsecutiveDays(double wp, bool ae);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};
//End   mornings-afternoons 2021-09-26

//Begin mornings-afternoons 2022-02-15
class ConstraintStudentsSetMaxThreeConsecutiveDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxThreeConsecutiveDays)

public:
	/**
	The name of the students set for this constraint
	*/
	QString students;

	//internal redundant data
	/**
	The subgroups
	*/
	QList<int> iSubgroupsList;

	bool allowAMAMException; //AMAM=afternoon morning afternoon morning.

	ConstraintStudentsSetMaxThreeConsecutiveDays();

	ConstraintStudentsSetMaxThreeConsecutiveDays(double wp, bool ae, const QString& sn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxThreeConsecutiveDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxThreeConsecutiveDays)

public:
	bool allowAMAMException; //AMAM=afternoon morning afternoon morning.

	ConstraintStudentsMaxThreeConsecutiveDays();

	ConstraintStudentsMaxThreeConsecutiveDays(double wp, bool ae);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};
//End   mornings-afternoons 2022-02-15

//Begin mornings-afternoons 2022-05-14
class ConstraintMinHalfDaysBetweenActivities: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintMinHalfDaysBetweenActivities)

public:
	bool consecutiveIfSameDay;

	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id-s)
	*/
	QList<int> activitiesIds;
	//int activitiesIds[MAX_CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES];

	QSet<int> activitiesIdsSet;

	/**
	The number of minimum days between each 2 activities
	*/
	int minDays;

	//internal structure (redundant)

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (index in the rules) - internal structure
	*/
	QList<int> _activities;
	//int _activities[MAX_CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES];

	ConstraintMinHalfDaysBetweenActivities();

	/**
	Constructor, using:
	the weight, the number of activities and the list of activities.
	*/
	ConstraintMinHalfDaysBetweenActivities(double wp, bool cisd, int n_act, const QList<int>& act, int _minHalfDays);

	/**
	Comparison operator - to be sure that we do not introduce duplicates
	*/
	bool operator==(ConstraintMinHalfDaysBetweenActivities& c);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules &r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};
//End   mornings-afternoons 2022-05-14

class ConstraintActivityPreferredDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityPreferredDay)

public:
	/**
	Activity id
	*/
	int activityId;

	/**
	The preferred day.
	*/
	int day;

	//bool permanentlyLocked; //if this is true, then this activity cannot be unlocked from the timetable view form

	//internal variables
	/**
	The index of the activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int activityIndex;
	
	ConstraintActivityPreferredDay();

	ConstraintActivityPreferredDay(double wp, int actId, int d/*, bool perm*/);

	/**
	Comparison operator - to be sure that we do not introduce duplicates
	*/
	bool operator==(const ConstraintActivityPreferredDay& c);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesMinInATerm: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesMinInATerm)

public:
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;

	int minActivitiesInATerm;
	
	bool allowEmptyTerms;

	//internal variables
	QList<int> _activitiesIndices;

	ConstraintActivitiesMinInATerm();

	ConstraintActivitiesMinInATerm(double wp, const QList<int>& a_L, int min_acts, bool allow_empty_terms);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintMaxTermsBetweenActivities: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintMaxTermsBetweenActivities)

public:
	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id-s)
	*/
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;

	/**
	The number of maximum terms between each 2 activities
	*/
	int maxTerms;

	//internal structure (redundant)

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (index in the rules) - internal structure
	*/
	QList<int> _activities;

	ConstraintMaxTermsBetweenActivities();

	/**
	Constructor, using:
	the weight, the number of activities, the list of activities, and the max number of terms.
	*/
	ConstraintMaxTermsBetweenActivities(double wp, int n_act, const QList<int>& act, int _maxTerms);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules &r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxActivityTagsPerDayFromSet: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxActivityTagsPerDayFromSet)

public:

	/**
	The name of the students
	*/
	QString students;

	/**
	The subgroups involved in this constraint
	*/
	QList<int> iSubgroupsList;

	int maxTags;
	
	QList<QString> tagsList;
	
	QSet<int> internalTagsSet;

	ConstraintStudentsSetMaxActivityTagsPerDayFromSet();

	ConstraintStudentsSetMaxActivityTagsPerDayFromSet(double wp, const QString& sn, int mtg, const QList<QString>& tgl);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxActivityTagsPerDayFromSet: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxActivityTagsPerDayFromSet)

public:

	int maxTags;
	
	QList<QString> tagsList;
	
	QSet<int> internalTagsSet;

	ConstraintStudentsMaxActivityTagsPerDayFromSet();

	ConstraintStudentsMaxActivityTagsPerDayFromSet(double wp, int mtg, const QList<QString>& tgl);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxActivityTagsPerRealDayFromSet: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxActivityTagsPerRealDayFromSet)

public:

	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	int maxTags;
	
	QList<QString> tagsList;
	
	QSet<int> internalTagsSet;

	ConstraintTeacherMaxActivityTagsPerRealDayFromSet();

	ConstraintTeacherMaxActivityTagsPerRealDayFromSet(double wp, const QString& tn, int mtg, const QList<QString>& tgl);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxActivityTagsPerRealDayFromSet: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxActivityTagsPerRealDayFromSet)

public:

	int maxTags;
	
	QList<QString> tagsList;
	
	QSet<int> internalTagsSet;

	ConstraintTeachersMaxActivityTagsPerRealDayFromSet();

	ConstraintTeachersMaxActivityTagsPerRealDayFromSet(double wp, int mtg, const QList<QString>& tgl);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxActivityTagsPerRealDayFromSet: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxActivityTagsPerRealDayFromSet)

public:

	/**
	The name of the students
	*/
	QString students;

	/**
	The subgroups involved in this constraint
	*/
	QList<int> iSubgroupsList;

	int maxTags;
	
	QList<QString> tagsList;
	
	QSet<int> internalTagsSet;

	ConstraintStudentsSetMaxActivityTagsPerRealDayFromSet();

	ConstraintStudentsSetMaxActivityTagsPerRealDayFromSet(double wp, const QString& sn, int mtg, const QList<QString>& tgl);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxActivityTagsPerRealDayFromSet: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxActivityTagsPerRealDayFromSet)

public:

	int maxTags;
	
	QList<QString> tagsList;
	
	QSet<int> internalTagsSet;

	ConstraintStudentsMaxActivityTagsPerRealDayFromSet();

	ConstraintStudentsMaxActivityTagsPerRealDayFromSet(double wp, int mtg, const QList<QString>& tgl);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintMaxHalfDaysBetweenActivities: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintMaxHalfDaysBetweenActivities)

public:
	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id-s)
	*/
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;

	/**
	The number of maximum days between each 2 activities
	*/
	int maxDays;

	//internal structure (redundant)

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (index in the rules) - internal structure
	*/
	QList<int> _activities;

	ConstraintMaxHalfDaysBetweenActivities();

	/**
	Constructor, using:
	the weight, the number of activities, the list of activities, and the max number of days.
	*/
	ConstraintMaxHalfDaysBetweenActivities(double wp, int n_act, const QList<int>& act, int _maxHalfDays);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules &r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivityBeginsStudentsDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityBeginsStudentsDay)

public:
	/**
	Activity id
	*/
	int activityId;

	//internal variables
	/**
	The index of the activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int activityIndex;

	ConstraintActivityBeginsStudentsDay();

	ConstraintActivityBeginsStudentsDay(double wp, int actId);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesBeginStudentsDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesBeginStudentsDay)

public:
	/**
	The teacher. If void, all teachers.
	*/
	QString teacherName;

	/**
	The students. If void, all students.
	*/
	QString studentsName;

	/**
	The subject. If void, all subjects.
	*/
	QString subjectName;

	/**
	The activity tag. If void, all activity tags.
	*/
	QString activityTagName;
	
	
	//internal data

	/**
	The number of activities which are represented by the subject, teacher and students requirements.
	*/
	int nActivities;
	
	/**
	The indices of the activities in the rules (from 0 to rules.nActivities-1)
	These are indices in the internal list -> Rules::internalActivitiesList
	*/
	QList<int> activitiesIndices;

	ConstraintActivitiesBeginStudentsDay();

	ConstraintActivitiesBeginStudentsDay(double wp, const QString& te, const QString& st, const QString& su, const QString& sut);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivityBeginsTeachersDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityBeginsTeachersDay)

public:
	/**
	Activity id
	*/
	int activityId;

	//internal variables
	/**
	The index of the activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int activityIndex;

	ConstraintActivityBeginsTeachersDay();

	ConstraintActivityBeginsTeachersDay(double wp, int actId);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesBeginTeachersDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesBeginTeachersDay)

public:
	/**
	The teacher. If void, all teachers.
	*/
	QString teacherName;

	/**
	The students. If void, all students.
	*/
	QString studentsName;

	/**
	The subject. If void, all subjects.
	*/
	QString subjectName;

	/**
	The activity tag. If void, all activity tags.
	*/
	QString activityTagName;
	
	
	//internal data

	/**
	The number of activities which are represented by the subject, teacher and students requirements.
	*/
	int nActivities;
	
	/**
	The indices of the activities in the rules (from 0 to rules.nActivities-1)
	These are indices in the internal list -> Rules::internalActivitiesList
	*/
	//int activitiesIndices[MAX_ACTIVITIES];
	QList<int> activitiesIndices;

	ConstraintActivitiesBeginTeachersDay();

	ConstraintActivitiesBeginTeachersDay(double wp, const QString& te, const QString& st, const QString& su, const QString& sut);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//2022-09-10
class ConstraintTeachersMinHoursPerAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinHoursPerAfternoon)

public:
	/**
	The minimum hours per morning
	*/
	int minHoursPerAfternoon;

	bool allowEmptyAfternoons;

	ConstraintTeachersMinHoursPerAfternoon();

	ConstraintTeachersMinHoursPerAfternoon(double wp, int minhours, bool _allowEmptyAfternoons);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinHoursPerAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinHoursPerAfternoon)

public:
	/**
	The minimum hours per morning
	*/
	int minHoursPerAfternoon;

	QString teacherName;

	int teacher_ID;

	bool allowEmptyAfternoons;

	ConstraintTeacherMinHoursPerAfternoon();

	ConstraintTeacherMinHoursPerAfternoon(double wp, int minhours, const QString& teacher, bool _allowEmptyAfternoons);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMinHoursPerAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMinHoursPerAfternoon)

public:
	int minHoursPerAfternoon;

	bool allowEmptyAfternoons;

	ConstraintStudentsMinHoursPerAfternoon();

	ConstraintStudentsMinHoursPerAfternoon(double wp, int minnh, bool _allowEmptyAfternoons);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMinHoursPerAfternoon: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMinHoursPerAfternoon)

public:
	int minHoursPerAfternoon;

	/**
	The students set name
	*/
	QString students;

	bool allowEmptyAfternoons;

	//internal variables

	/**
	The number of subgroups
	*/
	//int nSubgroups;

	/**
	The subgroups
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMinHoursPerAfternoon();

	ConstraintStudentsSetMinHoursPerAfternoon(double wp, int minnh, const QString& s, bool _allowEmptyAfternoons);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//2024-09-04
class ConstraintTeacherNoTwoConsecutiveDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherNoTwoConsecutiveDays)

public:
	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherNoTwoConsecutiveDays();

	ConstraintTeacherNoTwoConsecutiveDays(double wp, const QString& tn);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersNoTwoConsecutiveDays: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersNoTwoConsecutiveDays)

public:
	ConstraintTeachersNoTwoConsecutiveDays();

	ConstraintTeachersNoTwoConsecutiveDays(double wp);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherPairOfMutuallyExclusiveTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherPairOfMutuallyExclusiveTimeSlots)

public:
	int day1;
	int hour1;
	int day2;
	int hour2;
	
	QString teacherName;
	
	int teacher_ID;

	ConstraintTeacherPairOfMutuallyExclusiveTimeSlots();

	ConstraintTeacherPairOfMutuallyExclusiveTimeSlots(double wp, const QString& teacher, int d1, int h1, int d2, int h2);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersPairOfMutuallyExclusiveTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersPairOfMutuallyExclusiveTimeSlots)

public:
	int day1;
	int hour1;
	int day2;
	int hour2;
	
	ConstraintTeachersPairOfMutuallyExclusiveTimeSlots();

	ConstraintTeachersPairOfMutuallyExclusiveTimeSlots(double wp, int d1, int h1, int d2, int h2);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetPairOfMutuallyExclusiveTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetPairOfMutuallyExclusiveTimeSlots)

public:
	int day1;
	int hour1;
	int day2;
	int hour2;
	
	QString students;
	
	QList<int> iSubgroupsList;
	
	ConstraintStudentsSetPairOfMutuallyExclusiveTimeSlots();

	ConstraintStudentsSetPairOfMutuallyExclusiveTimeSlots(double wp, const QString& s, int d1, int h1, int d2, int h2);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsPairOfMutuallyExclusiveTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsPairOfMutuallyExclusiveTimeSlots)

public:
	int day1;
	int hour1;
	int day2;
	int hour2;
	
	ConstraintStudentsPairOfMutuallyExclusiveTimeSlots();

	ConstraintStudentsPairOfMutuallyExclusiveTimeSlots(double wp, int d1, int h1, int d2, int h2);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTwoSetsOfActivitiesSameSections: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTwoSetsOfActivitiesSameSections)

public:
	QList<int> activitiesAIds;
	QSet<int> activitiesAIdsSet;

	QList<int> activitiesBIds;
	QSet<int> activitiesBIdsSet;

	QList<int> oDays; //exception slots (previously overflow slots).
	QList<int> oHours;

	//internal variables
	QList<int> _activitiesAIndices;
	QList<int> _activitiesBIndices;
	QSet<int> _oSlotsSet;

	ConstraintTwoSetsOfActivitiesSameSections();

	ConstraintTwoSetsOfActivitiesSameSections(double wp, const QList<int>& aA_L, const QList<int>& aB_L, const QList<int>& d_L, const QList<int>& h_L);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);

	void recomputeActivitiesSets();

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxSingleGapsInSelectedTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxSingleGapsInSelectedTimeSlots)

public:
	int maxSingleGaps;

	QList<int> selectedDays;
	QList<int> selectedHours;

	ConstraintStudentsMaxSingleGapsInSelectedTimeSlots();

	ConstraintStudentsMaxSingleGapsInSelectedTimeSlots(double wp, int maxsg, const QList<int>& d_L, const QList<int>& h_L);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxSingleGapsInSelectedTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxSingleGapsInSelectedTimeSlots)

public:
	QString students;

	int maxSingleGaps;

	QList<int> selectedDays;
	QList<int> selectedHours;

	QList<int> iSubgroupsList;

	ConstraintStudentsSetMaxSingleGapsInSelectedTimeSlots();

	ConstraintStudentsSetMaxSingleGapsInSelectedTimeSlots(double wp, const QString& st, int maxsg, const QList<int>& d_L, const QList<int>& h_L);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxSingleGapsInSelectedTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxSingleGapsInSelectedTimeSlots)

public:
	QString teacher;

	int maxSingleGaps;

	QList<int> selectedDays;
	QList<int> selectedHours;

	//internal
	int teacher_ID;

	ConstraintTeacherMaxSingleGapsInSelectedTimeSlots();

	ConstraintTeacherMaxSingleGapsInSelectedTimeSlots(double wp, const QString& tn, int maxsg, const QList<int>& d_L, const QList<int>& h_L);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxSingleGapsInSelectedTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxSingleGapsInSelectedTimeSlots)

public:
	int maxSingleGaps;

	QList<int> selectedDays;
	QList<int> selectedHours;

	ConstraintTeachersMaxSingleGapsInSelectedTimeSlots();

	ConstraintTeachersMaxSingleGapsInSelectedTimeSlots(double wp, int maxsg, const QList<int>& d_L, const QList<int>& h_L);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxHoursPerTerm: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxHoursPerTerm)

public:
	int maxHoursPerTerm;

	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherMaxHoursPerTerm();

	ConstraintTeacherMaxHoursPerTerm(double wp, int maxht, const QString& t);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxHoursPerTerm: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxHoursPerTerm)

public:
	int maxHoursPerTerm;

	ConstraintTeachersMaxHoursPerTerm();

	ConstraintTeachersMaxHoursPerTerm(double wp, int maxht);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherPairOfMutuallyExclusiveSetsOfTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherPairOfMutuallyExclusiveSetsOfTimeSlots)

public:
	QList<int> selectedDays1;
	QList<int> selectedHours1;
	QList<int> selectedDays2;
	QList<int> selectedHours2;
	
	QString teacherName;
	
	//internal
	QSet<int> set1;
	QList<int> list1;
	QSet<int> set2;
	QList<int> list2;
	
	int teacher_ID;

	ConstraintTeacherPairOfMutuallyExclusiveSetsOfTimeSlots();

	ConstraintTeacherPairOfMutuallyExclusiveSetsOfTimeSlots(double wp, const QString& teacher,
	 const QList<int>& sd1, const QList<int>& sh1, const QList<int>& sd2, const QList<int>& sh2);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersPairOfMutuallyExclusiveSetsOfTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersPairOfMutuallyExclusiveSetsOfTimeSlots)

public:
	QList<int> selectedDays1;
	QList<int> selectedHours1;
	QList<int> selectedDays2;
	QList<int> selectedHours2;
	
	//internal
	QSet<int> set1;
	QList<int> list1;
	QSet<int> set2;
	QList<int> list2;
	
	ConstraintTeachersPairOfMutuallyExclusiveSetsOfTimeSlots();

	ConstraintTeachersPairOfMutuallyExclusiveSetsOfTimeSlots(double wp,
	 const QList<int>& sd1, const QList<int>& sh1, const QList<int>& sd2, const QList<int>& sh2);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots)

public:
	QList<int> selectedDays1;
	QList<int> selectedHours1;
	QList<int> selectedDays2;
	QList<int> selectedHours2;
	
	QString students;
	
	//internal
	QSet<int> set1;
	QList<int> list1;
	QSet<int> set2;
	QList<int> list2;
	
	QList<int> iSubgroupsList;
	
	ConstraintStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots();

	ConstraintStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots(double wp, const QString& s,
	 const QList<int>& sd1, const QList<int>& sh1, const QList<int>& sd2, const QList<int>& sh2);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsPairOfMutuallyExclusiveSetsOfTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsPairOfMutuallyExclusiveSetsOfTimeSlots)

public:
	QList<int> selectedDays1;
	QList<int> selectedHours1;
	QList<int> selectedDays2;
	QList<int> selectedHours2;
	
	//internal
	QSet<int> set1;
	QList<int> list1;
	QSet<int> set2;
	QList<int> list2;

	ConstraintStudentsPairOfMutuallyExclusiveSetsOfTimeSlots();

	ConstraintStudentsPairOfMutuallyExclusiveSetsOfTimeSlots(double wp,
	 const QList<int>& sd1, const QList<int>& sh1, const QList<int>& sd2, const QList<int>& sh2);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots)

public:
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;

	QList<int> selectedDays1;
	QList<int> selectedHours1;
	QList<int> selectedDays2;
	QList<int> selectedHours2;
	
	//internal variables
	QList<int> _activitiesIndices;

	QSet<int> timeSlotsSet1;
	QList<int> timeSlotsList1;
	QSet<int> timeSlotsSet2;
	QList<int> timeSlotsList2;

	ConstraintActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots();

	ConstraintActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots(double wp, const QList<int>& a_L,
	 const QList<int>& sd1, const QList<int>& sh1, const QList<int>& sd2, const QList<int>& sh2);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesPairOfMutuallyExclusiveTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesPairOfMutuallyExclusiveTimeSlots)

public:
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;

	int day1;
	int hour1;
	int day2;
	int hour2;
	
	//internal variables
	QList<int> _activitiesIndices;

	ConstraintActivitiesPairOfMutuallyExclusiveTimeSlots();
	ConstraintActivitiesPairOfMutuallyExclusiveTimeSlots(double wp, const QList<int>& a_L, int d1, int h1, int d2, int h2);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherOccupiesMaxSetsOfTimeSlotsFromSelection: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherOccupiesMaxSetsOfTimeSlotsFromSelection)

public:
	int maxOccupiedSets;
	
	QList<QList<int>> selectedDays;
	QList<QList<int>> selectedHours;
	
	QString teacherName;
	
	//internal
	QList<QSet<int>> listOfSets;
	QList<QList<int>> listOfLists;
	
	int teacher_ID;

	ConstraintTeacherOccupiesMaxSetsOfTimeSlotsFromSelection();

	ConstraintTeacherOccupiesMaxSetsOfTimeSlotsFromSelection(double wp, const QString& teacher, int mos, const QList<QList<int>>& sd, const QList<QList<int>>& sh);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersOccupyMaxSetsOfTimeSlotsFromSelection: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersOccupyMaxSetsOfTimeSlotsFromSelection)

public:
	int maxOccupiedSets;
	
	QList<QList<int>> selectedDays;
	QList<QList<int>> selectedHours;
	
	//internal
	QList<QSet<int>> listOfSets;
	QList<QList<int>> listOfLists;
	
	ConstraintTeachersOccupyMaxSetsOfTimeSlotsFromSelection();

	ConstraintTeachersOccupyMaxSetsOfTimeSlotsFromSelection(double wp, int mos, const QList<QList<int>>& sd, const QList<QList<int>>& sh);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection)

public:
	int maxOccupiedSets;
	
	QList<QList<int>> selectedDays;
	QList<QList<int>> selectedHours;
	
	QString students;
	
	//internal
	QList<QSet<int>> listOfSets;
	QList<QList<int>> listOfLists;
	
	QList<int> iSubgroupsList;

	ConstraintStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection();

	ConstraintStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection(double wp, const QString& s, int mos, const QList<QList<int>>& sd, const QList<QList<int>>& sh);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsOccupyMaxSetsOfTimeSlotsFromSelection: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsOccupyMaxSetsOfTimeSlotsFromSelection)

public:
	int maxOccupiedSets;
	
	QList<QList<int>> selectedDays;
	QList<QList<int>> selectedHours;
	
	//internal
	QList<QSet<int>> listOfSets;
	QList<QList<int>> listOfLists;
	
	ConstraintStudentsOccupyMaxSetsOfTimeSlotsFromSelection();

	ConstraintStudentsOccupyMaxSetsOfTimeSlotsFromSelection(double wp, int mos, const QList<QList<int>>& sd, const QList<QList<int>>& sh);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesOverlapCompletelyOrDoNotOverlap: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesOverlapCompletelyOrDoNotOverlap)

public:
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;

	//internal variables
	QList<int> _activitiesIndices;

	ConstraintActivitiesOverlapCompletelyOrDoNotOverlap();
	ConstraintActivitiesOverlapCompletelyOrDoNotOverlap(double wp, const QList<int>& a_L);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesOccupyMaxSetsOfTimeSlotsFromSelection: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesOccupyMaxSetsOfTimeSlotsFromSelection)

public:
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;

	int maxOccupiedSets;
	
	QList<QList<int>> selectedDays;
	QList<QList<int>> selectedHours;
	
	//internal variables
	QList<int> _activitiesIndices;

	QList<QSet<int>> listOfSets;
	QList<QList<int>> listOfLists;
	
	ConstraintActivitiesOccupyMaxSetsOfTimeSlotsFromSelection();

	ConstraintActivitiesOccupyMaxSetsOfTimeSlotsFromSelection(double wp, const QList<int>& a_L, int mos, const QList<QList<int>>& sd, const QList<QList<int>>& sh);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);
	
	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivityBeginsOrEndsStudentsDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityBeginsOrEndsStudentsDay)

public:
	/**
	Activity id
	*/
	int activityId;

	//internal variables
	/**
	The index of the activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int activityIndex;

	ConstraintActivityBeginsOrEndsStudentsDay();

	ConstraintActivityBeginsOrEndsStudentsDay(double wp, int actId);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesBeginOrEndStudentsDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesBeginOrEndStudentsDay)

public:
	/**
	The teacher. If void, all teachers.
	*/
	QString teacherName;

	/**
	The students. If void, all students.
	*/
	QString studentsName;

	/**
	The subject. If void, all subjects.
	*/
	QString subjectName;

	/**
	The activity tag. If void, all activity tags.
	*/
	QString activityTagName;
	
	
	//internal data

	/**
	The number of activities which are represented by the subject, teacher and students requirements.
	*/
	int nActivities;
	
	/**
	The indices of the activities in the rules (from 0 to rules.nActivities-1)
	These are indices in the internal list -> Rules::internalActivitiesList
	*/
	QList<int> activitiesIndices;

	ConstraintActivitiesBeginOrEndStudentsDay();

	ConstraintActivitiesBeginOrEndStudentsDay(double wp, const QString& te, const QString& st, const QString& su, const QString& sut);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivityBeginsOrEndsTeachersDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityBeginsOrEndsTeachersDay)

public:
	/**
	Activity id
	*/
	int activityId;

	//internal variables
	/**
	The index of the activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int activityIndex;

	ConstraintActivityBeginsOrEndsTeachersDay();

	ConstraintActivityBeginsOrEndsTeachersDay(double wp, int actId);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesBeginOrEndTeachersDay: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesBeginOrEndTeachersDay)

public:
	/**
	The teacher. If void, all teachers.
	*/
	QString teacherName;

	/**
	The students. If void, all students.
	*/
	QString studentsName;

	/**
	The subject. If void, all subjects.
	*/
	QString subjectName;

	/**
	The activity tag. If void, all activity tags.
	*/
	QString activityTagName;
	
	
	//internal data

	/**
	The number of activities which are represented by the subject, teacher and students requirements.
	*/
	int nActivities;
	
	/**
	The indices of the activities in the rules (from 0 to rules.nActivities-1)
	These are indices in the internal list -> Rules::internalActivitiesList
	*/
	//int activitiesIndices[MAX_ACTIVITIES];
	QList<int> activitiesIndices;

	ConstraintActivitiesBeginOrEndTeachersDay();

	ConstraintActivitiesBeginOrEndTeachersDay(double wp, const QString& te, const QString& st, const QString& su, const QString& sut);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots: public TimeConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots)

public:
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;
	
	QList<int> selectedDays;
	QList<int> selectedHours;
	
	int maxNumberOfStudents;

	//internal variables
	QList<int> _activitiesIndices;

	ConstraintActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots();

	ConstraintActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots(double wp, const QList<int>& a_L, const QList<int>& d_L, const QList<int>& h_L, int max_n_st);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Rules& r, Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//1
QDataStream& operator<<(QDataStream& stream, const ConstraintBasicCompulsoryTime& tc);
//2
QDataStream& operator<<(QDataStream& stream, const ConstraintBreakTimes& tc);
//3
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherNotAvailableTimes& tc);
//4
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxHoursDaily& tc);
//5
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxDaysPerWeek& tc);
//6
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxGapsPerWeek& tc);
//7
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxGapsPerWeek& tc);
//8
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxHoursDaily& tc);
//9
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxHoursContinuously& tc);
//10
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxHoursContinuously& tc);
//11
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinHoursDaily& tc);
//12
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinHoursDaily& tc);
//13
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxGapsPerDay& tc);
//14
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxGapsPerDay& tc);
//15
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsEarlyMaxBeginningsAtSecondHour& tc);
//16
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour& tc);
//17
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetNotAvailableTimes& tc);
//18
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxGapsPerWeek& tc);
//19
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxGapsPerWeek& tc);
//20
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxHoursDaily& tc);
//21
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxHoursDaily& tc);
//22
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxHoursContinuously& tc);
//23
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxHoursContinuously& tc);
//24
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinHoursDaily& tc);
//25
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinHoursDaily& tc);
//26
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityEndsStudentsDay& tc);
//27
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityPreferredStartingTime& tc);
//28
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesSameStartingTime& tc);
//29
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesNotOverlapping& tc);
//30
QDataStream& operator<<(QDataStream& stream, const ConstraintMinDaysBetweenActivities& tc);
//31
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityPreferredTimeSlots& tc);
//32
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesPreferredTimeSlots& tc);
//33
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityPreferredStartingTimes& tc);
//34
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesPreferredStartingTimes& tc);
//35
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesSameStartingHour& tc);
//36
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesSameStartingDay& tc);
//37
QDataStream& operator<<(QDataStream& stream, const ConstraintTwoActivitiesConsecutive& tc);
//38
QDataStream& operator<<(QDataStream& stream, const ConstraintTwoActivitiesOrdered& tc);
//39
QDataStream& operator<<(QDataStream& stream, const ConstraintMinGapsBetweenActivities& tc);
//40
QDataStream& operator<<(QDataStream& stream, const ConstraintSubactivitiesPreferredTimeSlots& tc);
//41
QDataStream& operator<<(QDataStream& stream, const ConstraintSubactivitiesPreferredStartingTimes& tc);
//42
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherIntervalMaxDaysPerWeek& tc);
//43
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersIntervalMaxDaysPerWeek& tc);
//44
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetIntervalMaxDaysPerWeek& tc);
//45
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsIntervalMaxDaysPerWeek& tc);
//46
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesEndStudentsDay& tc);
//47
QDataStream& operator<<(QDataStream& stream, const ConstraintTwoActivitiesGrouped& tc);
//48
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersActivityTagMaxHoursContinuously& tc);
//49
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherActivityTagMaxHoursContinuously& tc);
//50
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsActivityTagMaxHoursContinuously& tc);
//51
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetActivityTagMaxHoursContinuously& tc);
//52
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxDaysPerWeek& tc);
//53
QDataStream& operator<<(QDataStream& stream, const ConstraintThreeActivitiesGrouped& tc);
//54
QDataStream& operator<<(QDataStream& stream, const ConstraintMaxDaysBetweenActivities& tc);
//55
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinDaysPerWeek& tc);
//56
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinDaysPerWeek& tc);
//57
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersActivityTagMaxHoursDaily& tc);
//58
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherActivityTagMaxHoursDaily& tc);
//59
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsActivityTagMaxHoursDaily& tc);
//60
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetActivityTagMaxHoursDaily& tc);
//61
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxGapsPerDay& tc);
//62
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxGapsPerDay& tc);
//63
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesOccupyMaxTimeSlotsFromSelection& tc);
//64
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots& tc);
//65
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxDaysPerWeek& tc);
//66
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxDaysPerWeek& tc);
//67
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxSpanPerDay& tc);
//68
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxSpanPerDay& tc);
//69
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinRestingHours& tc);
//70
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinRestingHours& tc);
//71
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxSpanPerDay& tc);
//72
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxSpanPerDay& tc);
//73
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinRestingHours& tc);
//74
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinRestingHours& tc);
//75
QDataStream& operator<<(QDataStream& stream, const ConstraintTwoActivitiesOrderedIfSameDay& tc);
//76
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags& tc);
//77
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags& tc);
//78
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags& tc);
//79
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags& tc);
//80
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityTagsNotOverlapping& tc);
//81
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesOccupyMinTimeSlotsFromSelection& tc);
//82
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesMinSimultaneousInSelectedTimeSlots& tc);
//83
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersActivityTagMinHoursDaily& tc);
//84
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherActivityTagMinHoursDaily& tc);
//85
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsActivityTagMinHoursDaily& tc);
//86
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetActivityTagMinHoursDaily& tc);
//87
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityEndsTeachersDay& tc);
//88
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesEndTeachersDay& tc);
//89
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxHoursDailyRealDays& tc);
//90
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxRealDaysPerWeek& tc);
//91
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxHoursDailyRealDays& tc);
//92
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxHoursDailyRealDays& tc);
//93
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxHoursDailyRealDays& tc);
//94
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxRealDaysPerWeek& tc);
//95
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinRealDaysPerWeek& tc);
//96
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinRealDaysPerWeek& tc);
//97
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersActivityTagMaxHoursDailyRealDays& tc);
//98
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherActivityTagMaxHoursDailyRealDays& tc);
//99
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsActivityTagMaxHoursDailyRealDays& tc);
//100
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetActivityTagMaxHoursDailyRealDays& tc);
//101
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxAfternoonsPerWeek& tc);
//102
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxAfternoonsPerWeek& tc);
//103
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxMorningsPerWeek& tc);
//104
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxMorningsPerWeek& tc);
//105
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxActivityTagsPerDayFromSet& tc);
//106
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxActivityTagsPerDayFromSet& tc);
//107
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinMorningsPerWeek& tc);
//108
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinMorningsPerWeek& tc);
//109
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinAfternoonsPerWeek& tc);
//110
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinAfternoonsPerWeek& tc);
//111
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxTwoConsecutiveMornings& tc);
//112
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxTwoConsecutiveMornings& tc);
//113
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxTwoConsecutiveAfternoons& tc);
//114
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxTwoConsecutiveAfternoons& tc);
//115
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxGapsPerRealDay& tc);
//116
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxGapsPerRealDay& tc);
//117
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxGapsPerRealDay& tc);
//118
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxGapsPerRealDay& tc);
//119
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinHoursDailyRealDays& tc);
//120
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinHoursDailyRealDays& tc);
//121
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHour& tc);
//122
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour& tc);
//123
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinHoursPerMorning& tc);
//124
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinHoursPerMorning& tc);
//125
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxSpanPerRealDay& tc);
//126
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxSpanPerRealDay& tc);
//127
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxSpanPerRealDay& tc);
//128
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxSpanPerRealDay& tc);
//129
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMorningIntervalMaxDaysPerWeek& tc);
//130
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMorningIntervalMaxDaysPerWeek& tc);
//131
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherAfternoonIntervalMaxDaysPerWeek& tc);
//132
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersAfternoonIntervalMaxDaysPerWeek& tc);
//133
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinHoursPerMorning& tc);
//134
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinHoursPerMorning& tc);
//135
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxZeroGapsPerAfternoon& tc);
//136
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxZeroGapsPerAfternoon& tc);
//137
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxAfternoonsPerWeek& tc);
//138
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxAfternoonsPerWeek& tc);
//139
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxMorningsPerWeek& tc);
//140
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxMorningsPerWeek& tc);
//141
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinMorningsPerWeek& tc);
//142
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinMorningsPerWeek& tc);
//143
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinAfternoonsPerWeek& tc);
//144
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinAfternoonsPerWeek& tc);
//145
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMorningIntervalMaxDaysPerWeek& tc);
//146
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMorningIntervalMaxDaysPerWeek& tc);
//147
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek& tc);
//148
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsAfternoonIntervalMaxDaysPerWeek& tc);
//149
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxHoursPerAllAfternoons& tc);
//150
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxHoursPerAllAfternoons& tc);
//151
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxHoursPerAllAfternoons& tc);
//152
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxHoursPerAllAfternoons& tc);
//153
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon& tc);
//154
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinRestingHoursBetweenMorningAndAfternoon& tc);
//155
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon& tc);
//156
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon& tc);
//157
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour& tc);
//158
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHour& tc);
//159
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxGapsPerWeekForRealDays& tc);
//160
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxGapsPerWeekForRealDays& tc);
//161
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxGapsPerWeekForRealDays& tc);
//162
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxGapsPerWeekForRealDays& tc);
//163
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxRealDaysPerWeek& tc);
//164
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxRealDaysPerWeek& tc);
//165
QDataStream& operator<<(QDataStream& stream, const ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots& tc);
//166
QDataStream& operator<<(QDataStream& stream, const ConstraintMaxGapsBetweenActivities& tc);
//167
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesMaxInATerm& tc);
//168
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesOccupyMaxTerms& tc);
//169
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxGapsPerMorningAndAfternoon& tc);
//170
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxGapsPerMorningAndAfternoon& tc);
//171
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHour& tc);
//172
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour& tc);
//173
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour& tc);
//174
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour& tc);
//175
QDataStream& operator<<(QDataStream& stream, const ConstraintTwoSetsOfActivitiesOrdered& tc);
//176
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxThreeConsecutiveDays& tc);
//177
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxThreeConsecutiveDays& tc);
//178
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinGapsBetweenActivityTag& tc);
//179
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinGapsBetweenActivityTag& tc);
//180
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinGapsBetweenActivityTag& tc);
//181
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinGapsBetweenActivityTag& tc);
//182
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxThreeConsecutiveDays& tc);
//183
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxThreeConsecutiveDays& tc);
//184
QDataStream& operator<<(QDataStream& stream, const ConstraintMinHalfDaysBetweenActivities& tc);
//185
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityPreferredDay& tc);
//186
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesMinInATerm& tc);
//187
QDataStream& operator<<(QDataStream& stream, const ConstraintMaxTermsBetweenActivities& tc);
//188
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxActivityTagsPerDayFromSet& tc);
//189
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxActivityTagsPerDayFromSet& tc);
//190
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxActivityTagsPerRealDayFromSet& tc);
//191
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxActivityTagsPerRealDayFromSet& tc);
//192
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxActivityTagsPerRealDayFromSet& tc);
//193
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxActivityTagsPerRealDayFromSet& tc);
//194
QDataStream& operator<<(QDataStream& stream, const ConstraintMaxHalfDaysBetweenActivities& tc);
//195
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityBeginsStudentsDay& tc);
//196
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesBeginStudentsDay& tc);
//197
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityBeginsTeachersDay& tc);
//198
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesBeginTeachersDay& tc);
//199
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinHoursPerAfternoon& tc);
//200
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinHoursPerAfternoon& tc);
//201
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinHoursPerAfternoon& tc);
//202
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinHoursPerAfternoon& tc);
//203
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesMaxHourlySpan& tc);
//204
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxHoursDailyInInterval& tc);
//205
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxHoursDailyInInterval& tc);
//206
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxHoursDailyInInterval& tc);
//207
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxHoursDailyInInterval& tc);
//208
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay& tc);
//209
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay& tc);
//210
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay& tc);
//211
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay& tc);
//212
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinGapsBetweenActivityTagPerRealDay& tc);
//213
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinGapsBetweenActivityTagPerRealDay& tc);
//214
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinGapsBetweenActivityTagPerRealDay& tc);
//215
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinGapsBetweenActivityTagPerRealDay& tc);
//216
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon& tc);
//217
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon& tc);
//218
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon& tc);
//219
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon& tc);
//220
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon& tc);
//221
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon& tc);
//222
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon& tc);
//223
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon& tc);
//224
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersNoTwoConsecutiveDays& tc);
//225
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherNoTwoConsecutiveDays& tc);
//226
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherPairOfMutuallyExclusiveTimeSlots& tc);
//227
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersPairOfMutuallyExclusiveTimeSlots& tc);
//228
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetPairOfMutuallyExclusiveTimeSlots& tc);
//229
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsPairOfMutuallyExclusiveTimeSlots& tc);
//230
QDataStream& operator<<(QDataStream& stream, const ConstraintTwoSetsOfActivitiesSameSections& tc);
//231
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxSingleGapsInSelectedTimeSlots& tc);
//232
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxSingleGapsInSelectedTimeSlots& tc);
//233
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxSingleGapsInSelectedTimeSlots& tc);
//234
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxSingleGapsInSelectedTimeSlots& tc);
//235
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxHoursPerTerm& tc);
//236
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxHoursPerTerm& tc);
//237
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherPairOfMutuallyExclusiveSetsOfTimeSlots& tc);
//238
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersPairOfMutuallyExclusiveSetsOfTimeSlots& tc);
//239
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots& tc);
//240
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsPairOfMutuallyExclusiveSetsOfTimeSlots& tc);
//241
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots& tc);
//242
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesPairOfMutuallyExclusiveTimeSlots& tc);
//243
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherOccupiesMaxSetsOfTimeSlotsFromSelection& tc);
//244
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersOccupyMaxSetsOfTimeSlotsFromSelection& tc);
//245
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection& tc);
//246
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsOccupyMaxSetsOfTimeSlotsFromSelection& tc);
//247
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesOverlapCompletelyOrDoNotOverlap& tc);
//248
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesOccupyMaxSetsOfTimeSlotsFromSelection& tc);
//249
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityBeginsOrEndsStudentsDay& tc);
//250
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesBeginOrEndStudentsDay& tc);
//251
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityBeginsOrEndsTeachersDay& tc);
//252
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesBeginOrEndTeachersDay& tc);
//253
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots& tc);

//1
QDataStream& operator>>(QDataStream& stream, ConstraintBasicCompulsoryTime& tc);
//2
QDataStream& operator>>(QDataStream& stream, ConstraintBreakTimes& tc);
//3
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherNotAvailableTimes& tc);
//4
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxHoursDaily& tc);
//5
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxDaysPerWeek& tc);
//6
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxGapsPerWeek& tc);
//7
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxGapsPerWeek& tc);
//8
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxHoursDaily& tc);
//9
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxHoursContinuously& tc);
//10
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxHoursContinuously& tc);
//11
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinHoursDaily& tc);
//12
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinHoursDaily& tc);
//13
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxGapsPerDay& tc);
//14
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxGapsPerDay& tc);
//15
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsEarlyMaxBeginningsAtSecondHour& tc);
//16
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour& tc);
//17
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetNotAvailableTimes& tc);
//18
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxGapsPerWeek& tc);
//19
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxGapsPerWeek& tc);
//20
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxHoursDaily& tc);
//21
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxHoursDaily& tc);
//22
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxHoursContinuously& tc);
//23
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxHoursContinuously& tc);
//24
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinHoursDaily& tc);
//25
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinHoursDaily& tc);
//26
QDataStream& operator>>(QDataStream& stream, ConstraintActivityEndsStudentsDay& tc);
//27
QDataStream& operator>>(QDataStream& stream, ConstraintActivityPreferredStartingTime& tc);
//28
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesSameStartingTime& tc);
//29
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesNotOverlapping& tc);
//30
QDataStream& operator>>(QDataStream& stream, ConstraintMinDaysBetweenActivities& tc);
//31
QDataStream& operator>>(QDataStream& stream, ConstraintActivityPreferredTimeSlots& tc);
//32
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesPreferredTimeSlots& tc);
//33
QDataStream& operator>>(QDataStream& stream, ConstraintActivityPreferredStartingTimes& tc);
//34
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesPreferredStartingTimes& tc);
//35
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesSameStartingHour& tc);
//36
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesSameStartingDay& tc);
//37
QDataStream& operator>>(QDataStream& stream, ConstraintTwoActivitiesConsecutive& tc);
//38
QDataStream& operator>>(QDataStream& stream, ConstraintTwoActivitiesOrdered& tc);
//39
QDataStream& operator>>(QDataStream& stream, ConstraintMinGapsBetweenActivities& tc);
//40
QDataStream& operator>>(QDataStream& stream, ConstraintSubactivitiesPreferredTimeSlots& tc);
//41
QDataStream& operator>>(QDataStream& stream, ConstraintSubactivitiesPreferredStartingTimes& tc);
//42
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherIntervalMaxDaysPerWeek& tc);
//43
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersIntervalMaxDaysPerWeek& tc);
//44
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetIntervalMaxDaysPerWeek& tc);
//45
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsIntervalMaxDaysPerWeek& tc);
//46
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesEndStudentsDay& tc);
//47
QDataStream& operator>>(QDataStream& stream, ConstraintTwoActivitiesGrouped& tc);
//48
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersActivityTagMaxHoursContinuously& tc);
//49
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherActivityTagMaxHoursContinuously& tc);
//50
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsActivityTagMaxHoursContinuously& tc);
//51
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetActivityTagMaxHoursContinuously& tc);
//52
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxDaysPerWeek& tc);
//53
QDataStream& operator>>(QDataStream& stream, ConstraintThreeActivitiesGrouped& tc);
//54
QDataStream& operator>>(QDataStream& stream, ConstraintMaxDaysBetweenActivities& tc);
//55
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinDaysPerWeek& tc);
//56
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinDaysPerWeek& tc);
//57
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersActivityTagMaxHoursDaily& tc);
//58
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherActivityTagMaxHoursDaily& tc);
//59
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsActivityTagMaxHoursDaily& tc);
//60
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetActivityTagMaxHoursDaily& tc);
//61
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxGapsPerDay& tc);
//62
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxGapsPerDay& tc);
//63
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesOccupyMaxTimeSlotsFromSelection& tc);
//64
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots& tc);
//65
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxDaysPerWeek& tc);
//66
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxDaysPerWeek& tc);
//67
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxSpanPerDay& tc);
//68
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxSpanPerDay& tc);
//69
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinRestingHours& tc);
//70
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinRestingHours& tc);
//71
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxSpanPerDay& tc);
//72
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxSpanPerDay& tc);
//73
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinRestingHours& tc);
//74
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinRestingHours& tc);
//75
QDataStream& operator>>(QDataStream& stream, ConstraintTwoActivitiesOrderedIfSameDay& tc);
//76
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags& tc);
//77
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags& tc);
//78
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags& tc);
//79
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags& tc);
//80
QDataStream& operator>>(QDataStream& stream, ConstraintActivityTagsNotOverlapping& tc);
//81
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesOccupyMinTimeSlotsFromSelection& tc);
//82
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesMinSimultaneousInSelectedTimeSlots& tc);
//83
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersActivityTagMinHoursDaily& tc);
//84
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherActivityTagMinHoursDaily& tc);
//85
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsActivityTagMinHoursDaily& tc);
//86
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetActivityTagMinHoursDaily& tc);
//87
QDataStream& operator>>(QDataStream& stream, ConstraintActivityEndsTeachersDay& tc);
//88
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesEndTeachersDay& tc);
//89
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxHoursDailyRealDays& tc);
//90
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxRealDaysPerWeek& tc);
//91
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxHoursDailyRealDays& tc);
//92
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxHoursDailyRealDays& tc);
//93
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxHoursDailyRealDays& tc);
//94
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxRealDaysPerWeek& tc);
//95
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinRealDaysPerWeek& tc);
//96
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinRealDaysPerWeek& tc);
//97
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersActivityTagMaxHoursDailyRealDays& tc);
//98
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherActivityTagMaxHoursDailyRealDays& tc);
//99
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsActivityTagMaxHoursDailyRealDays& tc);
//100
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetActivityTagMaxHoursDailyRealDays& tc);
//101
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxAfternoonsPerWeek& tc);
//102
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxAfternoonsPerWeek& tc);
//103
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxMorningsPerWeek& tc);
//104
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxMorningsPerWeek& tc);
//105
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxActivityTagsPerDayFromSet& tc);
//106
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxActivityTagsPerDayFromSet& tc);
//107
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinMorningsPerWeek& tc);
//108
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinMorningsPerWeek& tc);
//109
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinAfternoonsPerWeek& tc);
//110
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinAfternoonsPerWeek& tc);
//111
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxTwoConsecutiveMornings& tc);
//112
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxTwoConsecutiveMornings& tc);
//113
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxTwoConsecutiveAfternoons& tc);
//114
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxTwoConsecutiveAfternoons& tc);
//115
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxGapsPerRealDay& tc);
//116
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxGapsPerRealDay& tc);
//117
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxGapsPerRealDay& tc);
//118
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxGapsPerRealDay& tc);
//119
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinHoursDailyRealDays& tc);
//120
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinHoursDailyRealDays& tc);
//121
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHour& tc);
//122
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour& tc);
//123
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinHoursPerMorning& tc);
//124
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinHoursPerMorning& tc);
//125
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxSpanPerRealDay& tc);
//126
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxSpanPerRealDay& tc);
//127
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxSpanPerRealDay& tc);
//128
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxSpanPerRealDay& tc);
//129
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMorningIntervalMaxDaysPerWeek& tc);
//130
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMorningIntervalMaxDaysPerWeek& tc);
//131
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherAfternoonIntervalMaxDaysPerWeek& tc);
//132
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersAfternoonIntervalMaxDaysPerWeek& tc);
//133
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinHoursPerMorning& tc);
//134
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinHoursPerMorning& tc);
//135
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxZeroGapsPerAfternoon& tc);
//136
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxZeroGapsPerAfternoon& tc);
//137
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxAfternoonsPerWeek& tc);
//138
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxAfternoonsPerWeek& tc);
//139
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxMorningsPerWeek& tc);
//140
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxMorningsPerWeek& tc);
//141
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinMorningsPerWeek& tc);
//142
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinMorningsPerWeek& tc);
//143
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinAfternoonsPerWeek& tc);
//144
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinAfternoonsPerWeek& tc);
//145
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMorningIntervalMaxDaysPerWeek& tc);
//146
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMorningIntervalMaxDaysPerWeek& tc);
//147
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek& tc);
//148
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsAfternoonIntervalMaxDaysPerWeek& tc);
//149
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxHoursPerAllAfternoons& tc);
//150
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxHoursPerAllAfternoons& tc);
//151
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxHoursPerAllAfternoons& tc);
//152
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxHoursPerAllAfternoons& tc);
//153
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon& tc);
//154
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinRestingHoursBetweenMorningAndAfternoon& tc);
//155
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon& tc);
//156
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon& tc);
//157
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour& tc);
//158
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHour& tc);
//159
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxGapsPerWeekForRealDays& tc);
//160
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxGapsPerWeekForRealDays& tc);
//161
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxGapsPerWeekForRealDays& tc);
//162
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxGapsPerWeekForRealDays& tc);
//163
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxRealDaysPerWeek& tc);
//164
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxRealDaysPerWeek& tc);
//165
QDataStream& operator>>(QDataStream& stream, ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots& tc);
//166
QDataStream& operator>>(QDataStream& stream, ConstraintMaxGapsBetweenActivities& tc);
//167
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesMaxInATerm& tc);
//168
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesOccupyMaxTerms& tc);
//169
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxGapsPerMorningAndAfternoon& tc);
//170
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxGapsPerMorningAndAfternoon& tc);
//171
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHour& tc);
//172
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour& tc);
//173
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour& tc);
//174
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour& tc);
//175
QDataStream& operator>>(QDataStream& stream, ConstraintTwoSetsOfActivitiesOrdered& tc);
//176
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxThreeConsecutiveDays& tc);
//177
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxThreeConsecutiveDays& tc);
//178
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinGapsBetweenActivityTag& tc);
//179
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinGapsBetweenActivityTag& tc);
//180
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinGapsBetweenActivityTag& tc);
//181
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinGapsBetweenActivityTag& tc);
//182
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxThreeConsecutiveDays& tc);
//183
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxThreeConsecutiveDays& tc);
//184
QDataStream& operator>>(QDataStream& stream, ConstraintMinHalfDaysBetweenActivities& tc);
//185
QDataStream& operator>>(QDataStream& stream, ConstraintActivityPreferredDay& tc);
//186
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesMinInATerm& tc);
//187
QDataStream& operator>>(QDataStream& stream, ConstraintMaxTermsBetweenActivities& tc);
//188
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxActivityTagsPerDayFromSet& tc);
//189
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxActivityTagsPerDayFromSet& tc);
//190
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxActivityTagsPerRealDayFromSet& tc);
//191
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxActivityTagsPerRealDayFromSet& tc);
//192
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxActivityTagsPerRealDayFromSet& tc);
//193
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxActivityTagsPerRealDayFromSet& tc);
//194
QDataStream& operator>>(QDataStream& stream, ConstraintMaxHalfDaysBetweenActivities& tc);
//195
QDataStream& operator>>(QDataStream& stream, ConstraintActivityBeginsStudentsDay& tc);
//196
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesBeginStudentsDay& tc);
//197
QDataStream& operator>>(QDataStream& stream, ConstraintActivityBeginsTeachersDay& tc);
//198
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesBeginTeachersDay& tc);
//199
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinHoursPerAfternoon& tc);
//200
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinHoursPerAfternoon& tc);
//201
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinHoursPerAfternoon& tc);
//202
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinHoursPerAfternoon& tc);
//203
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesMaxHourlySpan& tc);
//204
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxHoursDailyInInterval& tc);
//205
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxHoursDailyInInterval& tc);
//206
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxHoursDailyInInterval& tc);
//207
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxHoursDailyInInterval& tc);
//208
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay& tc);
//209
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay& tc);
//210
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay& tc);
//211
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay& tc);
//212
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinGapsBetweenActivityTagPerRealDay& tc);
//213
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinGapsBetweenActivityTagPerRealDay& tc);
//214
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinGapsBetweenActivityTagPerRealDay& tc);
//215
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinGapsBetweenActivityTagPerRealDay& tc);
//216
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon& tc);
//217
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon& tc);
//218
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon& tc);
//219
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon& tc);
//220
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon& tc);
//221
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon& tc);
//222
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon& tc);
//223
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon& tc);
//224
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersNoTwoConsecutiveDays& tc);
//225
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherNoTwoConsecutiveDays& tc);
//226
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherPairOfMutuallyExclusiveTimeSlots& tc);
//227
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersPairOfMutuallyExclusiveTimeSlots& tc);
//228
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetPairOfMutuallyExclusiveTimeSlots& tc);
//229
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsPairOfMutuallyExclusiveTimeSlots& tc);
//230
QDataStream& operator>>(QDataStream& stream, ConstraintTwoSetsOfActivitiesSameSections& tc);
//231
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxSingleGapsInSelectedTimeSlots& tc);
//232
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxSingleGapsInSelectedTimeSlots& tc);
//233
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxSingleGapsInSelectedTimeSlots& tc);
//234
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxSingleGapsInSelectedTimeSlots& tc);
//235
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxHoursPerTerm& tc);
//236
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxHoursPerTerm& tc);
//237
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherPairOfMutuallyExclusiveSetsOfTimeSlots& tc);
//238
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersPairOfMutuallyExclusiveSetsOfTimeSlots& tc);
//239
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots& tc);
//240
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsPairOfMutuallyExclusiveSetsOfTimeSlots& tc);
//241
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots& tc);
//242
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesPairOfMutuallyExclusiveTimeSlots& tc);
//243
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherOccupiesMaxSetsOfTimeSlotsFromSelection& tc);
//244
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersOccupyMaxSetsOfTimeSlotsFromSelection& tc);
//245
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection& tc);
//246
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsOccupyMaxSetsOfTimeSlotsFromSelection& tc);
//247
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesOverlapCompletelyOrDoNotOverlap& tc);
//248
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesOccupyMaxSetsOfTimeSlotsFromSelection& tc);
//249
QDataStream& operator>>(QDataStream& stream, ConstraintActivityBeginsOrEndsStudentsDay& tc);
//250
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesBeginOrEndStudentsDay& tc);
//251
QDataStream& operator>>(QDataStream& stream, ConstraintActivityBeginsOrEndsTeachersDay& tc);
//252
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesBeginOrEndTeachersDay& tc);
//253
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots& tc);

#endif
