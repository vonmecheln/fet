/*File generate_pre.cpp
*/

/***************************************************************************
                          generate_pre.cpp  -  description
                             -------------------
    begin                : 2002
    copyright            : (C) 2002 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "rules.h"
#include "timetable.h"

#include "generate_pre.h"

#include "matrix.h"

#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

#include "messageboxes.h"

#include <QtAlgorithms>
#include <QtGlobal>

#include <QPair>
#include <QSet>
#include <QHash>
#include <QMultiHash>
#include <QQueue>
#include <QList>

#include <list>

extern Timetable gt;

//#include <QApplication>
#ifndef FET_COMMAND_LINE
#include <QProgressDialog>
#include <QMessageBox>
#endif


#ifndef FET_COMMAND_LINE
extern QString initialOrderOfActivities;
#else
QString initialOrderOfActivities;
#endif


bool haveStudentsMinHoursDailyAllowEmptyDays;

Matrix1D<QSet<int>> tmpPreferredRealRooms;
Matrix1D<bool> tmpFoundNonEmpty;

QSet<int> fixedVirtualSpaceNonZeroButNotTimeActivities;

//bool thereAreTeachersWithMaxHoursDailyWithUnder100Weight;
bool thereAreTeachersWithMaxHoursDailyOrPerRealDayWithUnder100Weight;
//bool thereAreSubgroupsWithMaxHoursDailyWithUnder100Weight;
bool thereAreSubgroupsWithMaxHoursDailyOrPerRealDayWithUnder100Weight;

Matrix1D<QList<SpaceConstraint*>> constraintsForActivity;

Matrix1D<bool> visitedActivityResultantRealRooms;
////////end rooms

//extern Matrix1D<int> initialOrderOfActivitiesIndices;
Matrix1D<int> initialOrderOfActivitiesIndices;

Matrix1D<int> daysTeacherIsAvailable;
Matrix1D<int> daysSubgroupIsAvailable;
Matrix1D<int> requestedDaysForTeachers;
Matrix1D<int> requestedDaysForSubgroups;
Matrix1D<int> nReqForTeacher;
Matrix1D<int> nReqForSubgroup;

//used only in homeRoomsAreOk
Matrix1D<int> nHoursRequiredForRoom;
Matrix1D<int> nHoursAvailableForRoom;
/////////////////////////////

/////////used only in sortActivities
Matrix1D<int> nIncompatible;
Matrix1D<double> nMinDaysConstraintsBroken;
Matrix1D<int> nRoomsIncompat;
Matrix1D<double> nHoursForRoom;
Matrix1D<PreferredRoomsItem> maxPercentagePrefRooms;
Matrix1D<int> reprNInc;
////////////////////////////////////

Matrix1D<int> nIncompatibleFromFather;
Matrix1D<int> fatherActivityInInitialOrder;
////////////////////////////////////

//Matrix1D<int> permutation;
Matrix1D<int> copyOfInitialPermutation;
//the permutation matrix to obtain activities in decreasing difficulty order

//bool processTimeSpaceConstraints(QWidget* parent, QTextStream* initialOrderStream=nullptr);


////////BEGIN BASIC TIME CONSTRAINTS
//a value >=0 equal with the weight of the maximum weightPercentage of a basic time constraint
Matrix1D<QHash<int, int>> activitiesConflictingPercentage;

//bool computeActivitiesConflictingPercentage(QWidget* parent);

//void sortActivities(QWidget* parent, const QHash<int, int>& reprSameStartingTime, const QHash<int, QSet<int>>& reprSameActivitiesSet, QTextStream* initialOrderStream=nullptr);
////////END   BASIC TIME CONSTRAINTS


////////BEGIN MIN DAYS TIME CONSTRAINTS
Matrix1D<QList<int>> minDaysListOfActivities;
Matrix1D<QList<int>> minDaysListOfMinDays;
Matrix1D<QList<double>> minDaysListOfWeightPercentages;
Matrix1D<QList<bool>> minDaysListOfConsecutiveIfSameDay;
std::list<QList<int>> minDaysListOfActivitiesFromThisConstraint;
Matrix1D<QList<QList<int>*>> minDaysListOfActivitiesFromTheSameConstraint;

//bool computeMinDays(QWidget* parent);
////////END   MIN DAYS TIME CONSTRAINTS


////////BEGIN MIN HALF DAYS TIME CONSTRAINTS - only for the Mornings-Afternoons mode
Matrix1D<QList<int>> minHalfDaysListOfActivities;
Matrix1D<QList<int>> minHalfDaysListOfMinDays;
Matrix1D<QList<double>> minHalfDaysListOfWeightPercentages;
Matrix1D<QList<bool>> minHalfDaysListOfConsecutiveIfSameDay;
std::list<QList<int>> minHalfDaysListOfActivitiesFromThisConstraint;
Matrix1D<QList<QList<int>*>> minHalfDaysListOfActivitiesFromTheSameConstraint;

//bool computeMinHalfDays(QWidget* parent);
////////END   MIN HALF DAYS TIME CONSTRAINTS - only for the Mornings-Afternoons mode


////////BEGIN MAX DAYS TIME CONSTRAINTS
Matrix1D<QList<int>> maxDaysListOfActivities;
Matrix1D<QList<int>> maxDaysListOfMaxDays;
Matrix1D<QList<double>> maxDaysListOfWeightPercentages;

//bool computeMaxDays(QWidget* parent);
////////END   MAX DAYS TIME CONSTRAINTS


////////BEGIN MAX HALF DAYS TIME CONSTRAINTS
Matrix1D<QList<int>> maxHalfDaysListOfActivities;
Matrix1D<QList<int>> maxHalfDaysListOfMaxDays;
Matrix1D<QList<double>> maxHalfDaysListOfWeightPercentages;

//bool computeMaxHalfDays(QWidget* parent);
////////END   MAX HALF DAYS TIME CONSTRAINTS


////////BEGIN MAX TERMS TIME CONSTRAINTS
Matrix1D<QList<int>> maxTermsListOfActivities;
Matrix1D<QList<int>> maxTermsListOfMaxTerms;
Matrix1D<QList<double>> maxTermsListOfWeightPercentages;

//bool computeMaxTerms(QWidget* parent);
////////END   MAX TERMS TIME CONSTRAINTS


////////BEGIN MIN GAPS between activities TIME CONSTRAINTS
Matrix1D<QList<int>> minGapsBetweenActivitiesListOfActivities;
Matrix1D<QList<int>> minGapsBetweenActivitiesListOfMinGaps;
Matrix1D<QList<double>> minGapsBetweenActivitiesListOfWeightPercentages;

//bool computeMinGapsBetweenActivities(QWidget* parent);
////////END	 MIN GAPS between activities TIME CONSTRAINTS

//MAX GAPS BETWEEN ACTIVITIES
Matrix1D<QList<int> > maxGapsBetweenActivitiesListOfActivities;
Matrix1D<QList<int> > maxGapsBetweenActivitiesListOfMaxGaps;
Matrix1D<QList<double> > maxGapsBetweenActivitiesListOfWeightPercentages;

////////BEGIN st. not available, tch not avail., break, activity preferred time,
////////activity preferred times, activities preferred times
//percentage of allowed time, -1 if no restriction
Matrix2D<double> notAllowedTimesPercentages;

//break, which is not considered gap, false means no break, true means 100% break
//break can only be 100% or none
Matrix2D<bool> breakDayHour;

//students set not available, which is not considered gap, false means available, true means 100% not available
//students set not available can only be 100% or none
Matrix3D<bool> subgroupNotAvailableDayHour;

//used in students timetable view time horizontal dialog
QHash<QString, QSet<QPair<int, int>>> studentsSetNotAvailableDayHour;

//teacher not available, which is not considered gap, false means available, true means 100% not available
//teacher not available can only be 100% or none
Matrix3D<bool> teacherNotAvailableDayHour;

//bool computeNotAllowedTimesPercentages(QWidget* parent);
////////END   st. not available, tch not avail., break, activity preferred time,
////////activity preferred time, activities preferred times


////////BEGIN students max gaps and early
//bool computeNHoursPerSubgroup(QWidget* parent);
//bool computeSubgroupsEarlyAndMaxGapsPercentages(QWidget* parent);

Matrix1D<double> subgroupsEarlyMaxBeginningsAtSecondHourPercentage;
Matrix1D<int> subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings;
Matrix1D<double> subgroupsMaxGapsPerWeekPercentage;
Matrix1D<int> subgroupsMaxGapsPerWeekMaxGaps;

Matrix1D<int> nHoursPerSubgroup; //used also for students min hours daily

//max gaps per day (not perfect!!!)
//bool computeSubgroupsMaxGapsPerDayPercentages(QWidget* parent);

Matrix1D<double> subgroupsMaxGapsPerDayPercentage;
Matrix1D<int> subgroupsMaxGapsPerDayMaxGaps;
bool haveStudentsMaxGapsPerDay;


Matrix1D<double> subgroupsMaxGapsPerRealDayPercentage;
Matrix1D<int> subgroupsMaxGapsPerRealDayMaxGaps;
bool haveStudentsMaxGapsPerRealDay;

//2020-07-29
Matrix1D<double> subgroupsMaxGapsPerWeekForRealDaysPercentage;
Matrix1D<int> subgroupsMaxGapsPerWeekForRealDaysMaxGaps;
//bool haveStudentsMaxGapsPerRealDay;

////////END   students max gaps and early

//STUDENTS MAX REAL DAYS PER WEEK
Matrix1D<int> subgroupsMaxRealDaysPerWeekMaxDays;
Matrix1D<double> subgroupsMaxRealDaysPerWeekWeightPercentages;
Matrix1D<QList<int>> subgroupsWithMaxRealDaysPerWeekForActivities;


//TEACHERS MAX REAL DAYS PER WEEK
Matrix1D<int> teachersMaxRealDaysPerWeekMaxDays;
Matrix1D<double> teachersMaxRealDaysPerWeekWeightPercentages;
Matrix1D<QList<int>> teachersWithMaxRealDaysPerWeekForActivities;

//TEACHERS MAX AFTERNOONS PER WEEK
Matrix1D<int> teachersMaxAfternoonsPerWeekMaxAfternoons;
Matrix1D<double> teachersMaxAfternoonsPerWeekWeightPercentages;
Matrix1D<QList<int>> teachersWithMaxAfternoonsPerWeekForActivities;

//TEACHERS MAX MORNINGS PER WEEK
Matrix1D<int> teachersMaxMorningsPerWeekMaxMornings;
Matrix1D<double> teachersMaxMorningsPerWeekWeightPercentages;
Matrix1D<QList<int>> teachersWithMaxMorningsPerWeekForActivities;


//2020-06-25
////////BEGIN students max afternoons per week
//activities indices (in 0..gt.rules.nInternalActivities-1) for each subgroup
Matrix1D<int> subgroupsMaxAfternoonsPerWeekMaxAfternoons; //-1 for not existing
Matrix1D<double> subgroupsMaxAfternoonsPerWeekWeightPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
Matrix1D<QList<int>> subgroupsWithMaxAfternoonsPerWeekForActivities;

////////BEGIN students max mornings per week
//activities indices (in 0..gt.rules.nInternalActivities-1) for each subgroup
Matrix1D<int> subgroupsMaxMorningsPerWeekMaxMornings; //-1 for not existing
Matrix1D<double> subgroupsMaxMorningsPerWeekWeightPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
Matrix1D<QList<int>> subgroupsWithMaxMorningsPerWeekForActivities;
////////END students max mornings per week


////////BEGIN students max days per week
//activities indices (in 0..gt.rules.nInternalActivities-1) for each subgroup
Matrix1D<int> subgroupsMaxDaysPerWeekMaxDays; //-1 for not existing
Matrix1D<double> subgroupsMaxDaysPerWeekWeightPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
Matrix1D<QList<int>> subgroupsWithMaxDaysPerWeekForActivities;

////////BEGIN teachers max days per week
//activities indices (in 0..gt.rules.nInternalActivities-1) for each teacher
Matrix1D<int> teachersMaxDaysPerWeekMaxDays; //-1 for not existing
Matrix1D<double> teachersMaxDaysPerWeekWeightPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
Matrix1D<QList<int>> teachersWithMaxDaysPerWeekForActivities;

//bool computeMaxDaysPerWeekForTeachers(QWidget* parent);

//bool computeMaxDaysPerWeekForStudents(QWidget* parent);
////////END   teachers max days per week


////////BEGIN teachers max three consecutive days
//activities indices (in 0..gt.rules.nInternalActivities-1) for each teacher
Matrix1D<bool> teachersMaxThreeConsecutiveDaysAllowAMAMException;
Matrix1D<double> teachersMaxThreeConsecutiveDaysPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
Matrix1D<QList<int>> teachersWithMaxThreeConsecutiveDaysForActivities;

//bool computeMaxThreeDaysConsecutiveForTeachers(QWidget* parent);
////////END   teachers max three consecutive days

////////BEGIN students max three consecutive days
//activities indices (in 0..gt.rules.nInternalActivities-1) for each subgroup
Matrix1D<bool> subgroupsMaxThreeConsecutiveDaysAllowAMAMException;
Matrix1D<double> subgroupsMaxThreeConsecutiveDaysPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
Matrix1D<QList<int>> subgroupsWithMaxThreeConsecutiveDaysForActivities;

//bool computeMaxThreeDaysConsecutiveForStudents(QWidget* parent);
////////END   students max three consecutive days


Matrix1D<double> teachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentage;
Matrix1D<int> teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings;

Matrix1D<double> subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentage;
Matrix1D<int> subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings;


Matrix1D<double> teachersMorningsEarlyMaxBeginningsAtSecondHourPercentage;
Matrix1D<int> teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings;

Matrix1D<double> subgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentage;
Matrix1D<int> subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings;


////////BEGIN teachers max gaps per week and per day and per morning and afternoon
Matrix1D<double> teachersMaxGapsPerWeekPercentage;
Matrix1D<int> teachersMaxGapsPerWeekMaxGaps;

Matrix1D<double> teachersMaxGapsPerDayPercentage;
Matrix1D<int> teachersMaxGapsPerDayMaxGaps;

Matrix1D<double> teachersMaxGapsPerMorningAndAfternoonPercentage;
Matrix1D<int> teachersMaxGapsPerMorningAndAfternoonMaxGaps;

Matrix1D<int> nHoursPerTeacher;
//bool computeNHoursPerTeacher(QWidget* parent);
//bool computeTeachersMaxGapsPerWeekPercentage(QWidget* parent);
//bool computeTeachersMaxGapsPerDayPercentage(QWidget* parent);
//bool computeTeachersMaxGapsPerMorningAndAfternoonPercentage(QWidget* parent);


Matrix1D<double> teachersMaxGapsPerRealDayPercentage;
Matrix1D<int> teachersMaxGapsPerRealDayMaxGaps;
Matrix1D<bool> teachersMaxGapsPerRealDayAllowException;
bool haveTeachersMaxGapsPerRealDay;

//2020-07-29
Matrix1D<double> teachersMaxGapsPerWeekForRealDaysPercentage;
Matrix1D<int> teachersMaxGapsPerWeekForRealDaysMaxGaps;
//bool haveTeachersMaxGapsPerRealDay;

////////END   teachers max gaps per week and per day and per morning and afternoon

bool haveTeachersAfternoonsEarly;
bool haveStudentsAfternoonsEarly;

bool haveTeachersMorningsEarly;
bool haveStudentsMorningsEarly;

Matrix1D<bool> teacherConstrainedToZeroGapsPerAfternoon;


////////BEGIN activities same starting time
Matrix1D<QList<int>> activitiesSameStartingTimeActivities;
Matrix1D<QList<double>> activitiesSameStartingTimePercentages;
//bool computeActivitiesSameStartingTime(QWidget* parent, QHash<int, int> & reprSameStartingTime, QHash<int, QSet<int>> & reprSameActivitiesSet);
////////END   activities same starting time


////////BEGIN activities same starting hour
Matrix1D<QList<int>> activitiesSameStartingHourActivities;
Matrix1D<QList<double>> activitiesSameStartingHourPercentages;
//void computeActivitiesSameStartingHour();
////////END   activities same starting hour


////////BEGIN activities same starting day
Matrix1D<QList<int>> activitiesSameStartingDayActivities;
Matrix1D<QList<double>> activitiesSameStartingDayPercentages;
//void computeActivitiesSameStartingDay();
////////END   activities same starting hour


////////BEGIN activities not overlapping
Matrix1D<QList<int>> activitiesNotOverlappingActivities;
Matrix1D<QList<double>> activitiesNotOverlappingPercentages;
//void computeActivitiesNotOverlapping();
////////END   activities not overlapping


////////BEGIN teacher(s) max hours daily
Matrix1D<double> teachersMaxHoursDailyPercentages1;
Matrix1D<int> teachersMaxHoursDailyMaxHours1;

Matrix1D<double> teachersMaxHoursDailyPercentages2;
Matrix1D<int> teachersMaxHoursDailyMaxHours2;

//bool computeTeachersMaxHoursDaily(QWidget* parent);
////////END   teacher(s) max hours daily


Matrix1D<double> teachersMaxHoursDailyRealDaysPercentages1;
Matrix1D<int> teachersMaxHoursDailyRealDaysMaxHours1;

Matrix1D<double> teachersMaxHoursDailyRealDaysPercentages2;
Matrix1D<int> teachersMaxHoursDailyRealDaysMaxHours2;


////////BEGIN teacher(s) max hours continuously
Matrix1D<double> teachersMaxHoursContinuouslyPercentages1;
Matrix1D<int> teachersMaxHoursContinuouslyMaxHours1;

Matrix1D<double> teachersMaxHoursContinuouslyPercentages2;
Matrix1D<int> teachersMaxHoursContinuouslyMaxHours2;

//bool computeTeachersMaxHoursContinuously(QWidget* parent);
////////END   teacher(s) max hours continuously


///////BEGIN teacher(s) activity tag max hours daily
bool haveTeachersActivityTagMaxHoursDaily;

Matrix1D<QList<int>> teachersActivityTagMaxHoursDailyMaxHours;
Matrix1D<QList<int>> teachersActivityTagMaxHoursDailyActivityTag;
Matrix1D<QList<double>> teachersActivityTagMaxHoursDailyPercentage;

//bool computeTeachersActivityTagMaxHoursDaily(QWidget* parent);
///////END   teacher(s) activity tag max hours daily


//teacher(s) activity tag max hours daily real days
bool haveTeachersActivityTagMaxHoursDailyRealDays;

Matrix1D<QList<int>> teachersActivityTagMaxHoursDailyRealDaysMaxHours;
Matrix1D<QList<int>> teachersActivityTagMaxHoursDailyRealDaysActivityTag;
Matrix1D<QList<double>> teachersActivityTagMaxHoursDailyRealDaysPercentage;


///////BEGIN teacher(s) activity tag max hours continuously
bool haveTeachersActivityTagMaxHoursContinuously;

Matrix1D<QList<int>> teachersActivityTagMaxHoursContinuouslyMaxHours;
Matrix1D<QList<int>> teachersActivityTagMaxHoursContinuouslyActivityTag;
Matrix1D<QList<double>> teachersActivityTagMaxHoursContinuouslyPercentage;

//bool computeTeachersActivityTagMaxHoursContinuously(QWidget* parent);
///////END   teacher(s) activity tag max hours continuously


////////BEGIN teacher(s) min hours daily
Matrix2D<double> teachersMinHoursDailyPercentages;
Matrix2D<int> teachersMinHoursDailyMinHours;

Matrix1D<double> teachersMinHoursPerAfternoonPercentages;
Matrix1D<int> teachersMinHoursPerAfternoonMinHours;

//bool computeTeachersMinHoursDaily(QWidget* parent);
////////END   teacher(s) min hours daily


////////BEGIN teacher(s) min days per week
Matrix1D<double> teachersMinDaysPerWeekPercentages;
Matrix1D<int> teachersMinDaysPerWeekMinDays;

//bool computeTeachersMinDaysPerWeek(QWidget* parent);
////////END   teacher(s) min days per week


////////BEGIN students (set) max hours daily
Matrix1D<double> subgroupsMaxHoursDailyPercentages1;
Matrix1D<int> subgroupsMaxHoursDailyMaxHours1;

Matrix1D<double> subgroupsMaxHoursDailyPercentages2;
Matrix1D<int> subgroupsMaxHoursDailyMaxHours2;

//bool computeSubgroupsMaxHoursDaily(QWidget* parent);
////////END   students (set) max hours daily


//2020-06-28
//teacher(s) max hours per all afternoons
Matrix1D<double> teachersMaxHoursPerAllAfternoonsPercentages;
Matrix1D<int> teachersMaxHoursPerAllAfternoonsMaxHours;

//2020-06-28
//students (set) max hours per all afternoons
Matrix1D<double> subgroupsMaxHoursPerAllAfternoonsPercentages;
Matrix1D<int> subgroupsMaxHoursPerAllAfternoonsMaxHours;

//real day
Matrix1D<double> teachersMinHoursDailyRealDaysPercentages;
Matrix1D<int> teachersMinHoursDailyRealDaysMinHours;

//teacher(s) min days per week
Matrix1D<double> teachersMinRealDaysPerWeekPercentages;
Matrix1D<int> teachersMinRealDaysPerWeekMinDays;

//teacher(s) min mornings and afternoons per week
Matrix1D<double> teachersMinMorningsPerWeekPercentages;
Matrix1D<int> teachersMinMorningsPerWeekMinMornings;
Matrix1D<double> teachersMinAfternoonsPerWeekPercentages;
Matrix1D<int> teachersMinAfternoonsPerWeekMinAfternoons;

//students min mornings and afternoons per week
Matrix1D<double> subgroupsMinMorningsPerWeekPercentages;
Matrix1D<int> subgroupsMinMorningsPerWeekMinMornings;
Matrix1D<double> subgroupsMinAfternoonsPerWeekPercentages;
Matrix1D<int> subgroupsMinAfternoonsPerWeekMinAfternoons;


////////BEGIN students (set) max hours continuously
Matrix1D<double> subgroupsMaxHoursContinuouslyPercentages1;
Matrix1D<int> subgroupsMaxHoursContinuouslyMaxHours1;

Matrix1D<double> subgroupsMaxHoursContinuouslyPercentages2;
Matrix1D<int> subgroupsMaxHoursContinuouslyMaxHours2;

//bool computeStudentsMaxHoursContinuously(QWidget* parent);
////////END   students (set) max hours continuously


///////BEGIN students (set) activity tag max hours daily
bool haveStudentsActivityTagMaxHoursDaily;

Matrix1D<QList<int>> subgroupsActivityTagMaxHoursDailyMaxHours;
Matrix1D<QList<int>> subgroupsActivityTagMaxHoursDailyActivityTag;
Matrix1D<QList<double>> subgroupsActivityTagMaxHoursDailyPercentage;

//bool computeStudentsActivityTagMaxHoursDaily(QWidget* parent);
///////END   students (set) activity tag max hours daily


//students (set) activity tag max hours daily real days
bool haveStudentsActivityTagMaxHoursDailyRealDays;

Matrix1D<QList<int>> subgroupsActivityTagMaxHoursDailyRealDaysMaxHours;
Matrix1D<QList<int>> subgroupsActivityTagMaxHoursDailyRealDaysActivityTag;
Matrix1D<QList<double>> subgroupsActivityTagMaxHoursDailyRealDaysPercentage;


///////BEGIN students (set) activity tag max hours continuously
bool haveStudentsActivityTagMaxHoursContinuously;

Matrix1D<QList<int>> subgroupsActivityTagMaxHoursContinuouslyMaxHours;
Matrix1D<QList<int>> subgroupsActivityTagMaxHoursContinuouslyActivityTag;
Matrix1D<QList<double>> subgroupsActivityTagMaxHoursContinuouslyPercentage;

//bool computeStudentsActivityTagMaxHoursContinuously(QWidget* parent);
///////END   students (set) activity tag max hours continuously


//students (set) max hours daily real days
Matrix1D<double> subgroupsMaxHoursDailyRealDaysPercentages1;
Matrix1D<int> subgroupsMaxHoursDailyRealDaysMaxHours1;

Matrix1D<double> subgroupsMaxHoursDailyRealDaysPercentages2;
Matrix1D<int> subgroupsMaxHoursDailyRealDaysMaxHours2;


////////BEGIN students (set) min hours daily
Matrix2D<double> subgroupsMinHoursDailyPercentages;
Matrix2D<int> subgroupsMinHoursDailyMinHours;
Matrix1D<bool> subgroupsMinHoursDailyAllowEmptyDays;
Matrix1D<bool> subgroupsMinHoursPerMorningAllowEmptyMornings;

Matrix1D<double> subgroupsMinHoursPerAfternoonPercentages;
Matrix1D<int> subgroupsMinHoursPerAfternoonMinHours;
Matrix1D<bool> subgroupsMinHoursPerAfternoonAllowEmptyAfternoons;

bool haveStudentsMinHoursDailyMorningsAfternoonsAllowEmptyDays;
//bool computeSubgroupsMinHoursDaily(QWidget* parent);
////////END   students (set) min hours daily


//////////////BEGIN 2 activities consecutive
//index represents the first activity, value in array represents the second activity
Matrix1D<QList<double>> constrTwoActivitiesConsecutivePercentages;
Matrix1D<QList<int>> constrTwoActivitiesConsecutiveActivities;
//void computeConstrTwoActivitiesConsecutive();

//index represents the second activity, value in array represents the first activity
Matrix1D<QList<double>> inverseConstrTwoActivitiesConsecutivePercentages;
Matrix1D<QList<int>> inverseConstrTwoActivitiesConsecutiveActivities;
//////////////END   2 activities consecutive


//////////////BEGIN 2 activities grouped
//index represents the first activity, value in array represents the second activity
Matrix1D<QList<double>> constrTwoActivitiesGroupedPercentages;
Matrix1D<QList<int>> constrTwoActivitiesGroupedActivities;
//void computeConstrTwoActivitiesGrouped();


//////////////BEGIN 3 activities grouped
//index represents the first activity, value in array represents the second and third activities
Matrix1D<QList<double>> constrThreeActivitiesGroupedPercentages;
Matrix1D<QList<QPair<int, int>>> constrThreeActivitiesGroupedActivities;
//void computeConstrThreeActivitiesGrouped();


//////////////BEGIN 2 activities ordered
//index represents the first activity, value in array represents the second activity
Matrix1D<QList<double>> constrTwoActivitiesOrderedPercentages;
Matrix1D<QList<int>> constrTwoActivitiesOrderedActivities;
//void computeConstrTwoActivitiesOrdered();

//index represents the second activity, value in array represents the first activity
Matrix1D<QList<double>> inverseConstrTwoActivitiesOrderedPercentages;
Matrix1D<QList<int>> inverseConstrTwoActivitiesOrderedActivities;
//////////////END   2 activities ordered

//////////////BEGIN 2 activities ordered if same day
//index represents the first activity, value in array represents the second activity
Matrix1D<QList<double>> constrTwoActivitiesOrderedIfSameDayPercentages;
Matrix1D<QList<int>> constrTwoActivitiesOrderedIfSameDayActivities;
//void computeConstrTwoActivitiesOrderedIfSameDay();

//index represents the second activity, value in array represents the first activity
Matrix1D<QList<double>> inverseConstrTwoActivitiesOrderedIfSameDayPercentages;
Matrix1D<QList<int>> inverseConstrTwoActivitiesOrderedIfSameDayActivities;
//////////////END   2 activities ordered if same day


////////////BEGIN activity ends students day
Matrix1D<double> activityEndsStudentsDayPercentages; //-1 for not existing
//bool computeActivityEndsStudentsDayPercentages(QWidget* parent);
bool haveActivityEndsStudentsDay;
////////////END   activity ends students day

////////////BEGIN activity ends teachers day
Matrix1D<double> activityEndsTeachersDayPercentages; //-1 for not existing
//bool computeActivityEndsTeachersDayPercentages(QWidget* parent);
bool haveActivityEndsTeachersDay;
////////////END   activity ends teachers day


////////////BEGIN activity begins students day
Matrix1D<double> activityBeginsStudentsDayPercentages; //-1 for not existing
//bool computeActivityBeginsStudentsDayPercentages(QWidget* parent);
bool haveActivityBeginsStudentsDay;
////////////END   activity begins students day

////////////BEGIN activity begins teachers day
Matrix1D<double> activityBeginsTeachersDayPercentages; //-1 for not existing
//bool computeActivityBeginsTeachersDayPercentages(QWidget* parent);
bool haveActivityBeginsTeachersDay;
////////////END   activity begins teachers day


bool checkMinDays100Percent(QWidget* parent);
bool checkMinDaysMaxTwoOnSameDay(QWidget* parent);

bool checkMinHalfDays100Percent(QWidget* parent);
bool checkMinHalfDaysMaxTwoOnSameDay(QWidget* parent);


bool checkMaxHoursForActivityDuration(QWidget* parent);


///////BEGIN teachers interval max days per week
Matrix1D<QList<double>> teachersIntervalMaxDaysPerWeekPercentages;
Matrix1D<QList<int>> teachersIntervalMaxDaysPerWeekMaxDays;
Matrix1D<QList<int>> teachersIntervalMaxDaysPerWeekIntervalStart;
Matrix1D<QList<int>> teachersIntervalMaxDaysPerWeekIntervalEnd;

//bool computeTeachersIntervalMaxDaysPerWeek(QWidget* parent);


//morning
Matrix1D<QList<double>> teachersMorningIntervalMaxDaysPerWeekPercentages;
Matrix1D<QList<int>> teachersMorningIntervalMaxDaysPerWeekMaxDays;
Matrix1D<QList<int>> teachersMorningIntervalMaxDaysPerWeekIntervalStart;
Matrix1D<QList<int>> teachersMorningIntervalMaxDaysPerWeekIntervalEnd;

//bool computeTeachersMorningIntervalMaxDaysPerWeek();

//afternoon
Matrix1D<QList<double>> teachersAfternoonIntervalMaxDaysPerWeekPercentages;
Matrix1D<QList<int>> teachersAfternoonIntervalMaxDaysPerWeekMaxDays;
Matrix1D<QList<int>> teachersAfternoonIntervalMaxDaysPerWeekIntervalStart;
Matrix1D<QList<int>> teachersAfternoonIntervalMaxDaysPerWeekIntervalEnd;

//bool computeTeachersAfternoonIntervalMaxDaysPerWeek();

///////END   teachers interval max days per week


///////BEGIN subgroups interval max days per week
Matrix1D<QList<double>> subgroupsIntervalMaxDaysPerWeekPercentages;
Matrix1D<QList<int>> subgroupsIntervalMaxDaysPerWeekMaxDays;
Matrix1D<QList<int>> subgroupsIntervalMaxDaysPerWeekIntervalStart;
Matrix1D<QList<int>> subgroupsIntervalMaxDaysPerWeekIntervalEnd;

//bool computeSubgroupsIntervalMaxDaysPerWeek(QWidget* parent);
///////END   subgroups interval max days per week



///////BEGIN students morning interval max days per week
Matrix1D<QList<double>> subgroupsMorningIntervalMaxDaysPerWeekPercentages;
Matrix1D<QList<int>> subgroupsMorningIntervalMaxDaysPerWeekMaxDays;
Matrix1D<QList<int>> subgroupsMorningIntervalMaxDaysPerWeekIntervalStart;
Matrix1D<QList<int>> subgroupsMorningIntervalMaxDaysPerWeekIntervalEnd;

//bool computeSubgroupsMorningIntervalMaxDaysPerWeek();
///////END   subgroups morning interval max days per week

///////BEGIN students afternoon interval max days per week
Matrix1D<QList<double>> subgroupsAfternoonIntervalMaxDaysPerWeekPercentages;
Matrix1D<QList<int>> subgroupsAfternoonIntervalMaxDaysPerWeekMaxDays;
Matrix1D<QList<int>> subgroupsAfternoonIntervalMaxDaysPerWeekIntervalStart;
Matrix1D<QList<int>> subgroupsAfternoonIntervalMaxDaysPerWeekIntervalEnd;

//bool computeSubgroupsAfternoonIntervalMaxDaysPerWeek();
///////END   subgroups afternoon interval max days per week


//2017-02-06
Matrix1D<int> teachersMaxSpanPerDayMaxSpan; //-1 for not existing
Matrix1D<double> teachersMaxSpanPerDayPercentages; //-1 for not existing
Matrix1D<bool> teachersMaxSpanPerDayAllowOneDayExceptionPlusOne;


////////////
Matrix1D<int> teachersMaxSpanPerRealDayMaxSpan; //-1 for not existing
Matrix1D<double> teachersMaxSpanPerRealDayPercentages; //-1 for not existing
Matrix1D<bool> teachersMaxSpanPerRealDayAllowOneDayExceptionPlusOne; //for Luca

Matrix1D<int> subgroupsMaxSpanPerRealDayMaxSpan; //-1 for not existing
Matrix1D<double> subgroupsMaxSpanPerRealDayPercentages; //-1 for not existing
////////////


Matrix1D<int> teachersMinRestingHoursCircularMinHours; //-1 for not existing
Matrix1D<double> teachersMinRestingHoursCircularPercentages; //-1 for not existing
Matrix1D<int> teachersMinRestingHoursNotCircularMinHours; //-1 for not existing
Matrix1D<double> teachersMinRestingHoursNotCircularPercentages; //-1 for not existing

Matrix1D<int> subgroupsMaxSpanPerDayMaxSpan; //-1 for not existing
Matrix1D<double> subgroupsMaxSpanPerDayPercentages; //-1 for not existing

Matrix1D<int> subgroupsMinRestingHoursCircularMinHours; //-1 for not existing
Matrix1D<double> subgroupsMinRestingHoursCircularPercentages; //-1 for not existing
Matrix1D<int> subgroupsMinRestingHoursNotCircularMinHours; //-1 for not existing
Matrix1D<double> subgroupsMinRestingHoursNotCircularPercentages; //-1 for not existing

//bool computeTeachersMaxSpanPerDay(QWidget* parent);
//bool computeTeachersMinRestingHours(QWidget* parent);
//bool computeSubgroupsMaxSpanPerDay(QWidget* parent);
//bool computeSubgroupsMinRestingHours(QWidget* parent);
////////////


//2020-07-20
///////////////
Matrix1D<int> teachersMinRestingHoursBetweenMorningAndAfternoonMinHours; //-1 for not existing
Matrix1D<double> teachersMinRestingHoursBetweenMorningAndAfternoonPercentages; //-1 for not existing

Matrix1D<int> subgroupsMinRestingHoursBetweenMorningAndAfternoonMinHours; //-1 for not existing
Matrix1D<double> subgroupsMinRestingHoursBetweenMorningAndAfternoonPercentages; //-1 for not existing
//bool computeTeachersMinRestingHoursBetweenMorningAndAfternoon(QWidget* parent);
//bool computeSubgroupsMinRestingHoursBetweenMorningAndAfternoon(QWidget* parent);
///////////////


Matrix1D<double> teachersMaxTwoConsecutiveMorningsPercentage;
Matrix1D<double> teachersMaxTwoConsecutiveAfternoonsPercentage;


Matrix1D<double> teachersMaxTwoActivityTagsPerDayFromN1N2N3Percentages;
Matrix1D<int> activityTagN1N2N3;
Matrix1D<QList<int>> teachersWithN1N2N3ForActivities;

Matrix1D<double> subgroupsMaxTwoActivityTagsPerDayFromN1N2N3Percentages;
Matrix1D<QList<int>> subgroupsWithN1N2N3ForActivities;

Matrix1D<double> teachersMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages;
Matrix1D<double> subgroupsMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages;


////////BEGIN rooms
//bool computeBasicSpace(QWidget* parent);

Matrix2D<double> notAllowedRoomTimePercentages; //-1 for available
//bool computeNotAllowedRoomTimePercentages();

bool haveTeacherRoomNotAllowedTimesConstraints;
QHash<QPair<qint64, qint64>, double> notAllowedTeacherRoomTimePercentages;
//first qint64 is (teacher, room), second qint64 is (day, hour)
//bool computeNotAllowedTeacherRoomTimePercentages();

Matrix1D<QList<PreferredRoomsItem>> activitiesPreferredRoomsList;
Matrix1D<bool> unspecifiedPreferredRoom;

Matrix1D<QList<int>> activitiesHomeRoomsHomeRooms;
Matrix1D<double> activitiesHomeRoomsPercentage;
Matrix1D<bool> unspecifiedHomeRoom;

Matrix1D<QSet<int>> preferredRealRooms;

//bool computeActivitiesRoomsPreferences(QWidget* parent);
////////END   rooms


////////BEGIN building changes
Matrix1D<double> maxBuildingChangesPerWeekForStudentsPercentages;
Matrix1D<int> maxBuildingChangesPerWeekForStudentsMaxChanges;
//bool computeMaxBuildingChangesPerWeekForStudents(QWidget* parent);

Matrix1D<double> maxBuildingChangesPerDayForStudentsPercentages;
Matrix1D<int> maxBuildingChangesPerDayForStudentsMaxChanges;
//bool computeMaxBuildingChangesPerDayForStudents(QWidget* parent);

Matrix1D<double> minGapsBetweenBuildingChangesForStudentsPercentages;
Matrix1D<int> minGapsBetweenBuildingChangesForStudentsMinGaps;
//bool computeMinGapsBetweenBuildingChangesForStudents(QWidget* parent);

Matrix1D<double> maxBuildingChangesPerWeekForTeachersPercentages;
Matrix1D<int> maxBuildingChangesPerWeekForTeachersMaxChanges;
//bool computeMaxBuildingChangesPerWeekForTeachers(QWidget* parent);

Matrix1D<double> maxBuildingChangesPerDayForTeachersPercentages;
Matrix1D<int> maxBuildingChangesPerDayForTeachersMaxChanges;
//bool computeMaxBuildingChangesPerDayForTeachers(QWidget* parent);

Matrix1D<double> minGapsBetweenBuildingChangesForTeachersPercentages;
Matrix1D<int> minGapsBetweenBuildingChangesForTeachersMinGaps;
//bool computeMinGapsBetweenBuildingChangesForTeachers(QWidget* parent);

//BEGIN building changes per real day for teachers
Matrix1D<double> maxBuildingChangesPerRealDayForTeachersPercentages;
Matrix1D<int> maxBuildingChangesPerRealDayForTeachersMaxChanges;
//END   building changes per real day for teachers

//BEGIN building changes per real day for students
Matrix1D<double> maxBuildingChangesPerRealDayForSubgroupsPercentages;
Matrix1D<int> maxBuildingChangesPerRealDayForSubgroupsMaxChanges;
//END   building changes per real day for students
////////END   building changes


////////BEGIN room changes
Matrix1D<double> maxRoomChangesPerWeekForStudentsPercentages;
Matrix1D<int> maxRoomChangesPerWeekForStudentsMaxChanges;
//bool computeMaxRoomChangesPerWeekForStudents(QWidget* parent);

Matrix1D<double> maxRoomChangesPerDayForStudentsPercentages;
Matrix1D<int> maxRoomChangesPerDayForStudentsMaxChanges;
//bool computeMaxRoomChangesPerDayForStudents(QWidget* parent);

Matrix1D<double> minGapsBetweenRoomChangesForStudentsPercentages;
Matrix1D<int> minGapsBetweenRoomChangesForStudentsMinGaps;
//bool computeMinGapsBetweenRoomChangesForStudents(QWidget* parent);

Matrix1D<double> maxRoomChangesPerWeekForTeachersPercentages;
Matrix1D<int> maxRoomChangesPerWeekForTeachersMaxChanges;
//bool computeMaxRoomChangesPerWeekForTeachers(QWidget* parent);

Matrix1D<double> maxRoomChangesPerDayForTeachersPercentages;
Matrix1D<int> maxRoomChangesPerDayForTeachersMaxChanges;
//bool computeMaxRoomChangesPerDayForTeachers(QWidget* parent);

Matrix1D<double> minGapsBetweenRoomChangesForTeachersPercentages;
Matrix1D<int> minGapsBetweenRoomChangesForTeachersMinGaps;
//bool computeMinGapsBetweenRoomChangesForTeachers(QWidget* parent);


//BEGIN room changes per real day for teachers
Matrix1D<double> maxRoomChangesPerRealDayForTeachersPercentages;
Matrix1D<int> maxRoomChangesPerRealDayForTeachersMaxChanges;
//END   room changes per real day for teachers

//BEGIN room changes per real day for students
Matrix1D<double> maxRoomChangesPerRealDayForSubgroupsPercentages;
Matrix1D<int> maxRoomChangesPerRealDayForSubgroupsMaxChanges;
//END   room changes per real day for students

////////END   room changes


Matrix1D<QList<int>> mustComputeTimetableSubgroups;
Matrix1D<QList<int>> mustComputeTimetableTeachers;
Matrix1D<bool> mustComputeTimetableSubgroup;
Matrix1D<bool> mustComputeTimetableTeacher;

//void computeMustComputeTimetableSubgroups();
//void computeMustComputeTimetableTeachers();


//bool homeRoomsAreOk(QWidget* parent);


//2011-09-25 - Constraint activities occupy max time slots from selection

//We need the references to the elements to be valid, so we need this to be a std::list
std::list<ActivitiesOccupyMaxTimeSlotsFromSelection_item> aomtsList;
Matrix1D<QList<ActivitiesOccupyMaxTimeSlotsFromSelection_item*>> aomtsListForActivity;

//bool computeActivitiesOccupyMaxTimeSlotsFromSelection(QWidget* parent);

//2019-11-16 - Constraint activities occupy min time slots from selection

//We need the references to the elements to be valid, so we need this to be a std::list
std::list<ActivitiesOccupyMinTimeSlotsFromSelection_item> aomintsList;
Matrix1D<QList<ActivitiesOccupyMinTimeSlotsFromSelection_item*>> aomintsListForActivity;

//bool computeActivitiesOccupyMinTimeSlotsFromSelection(QWidget* parent);

//2011-09-30 - Constraint activities max simultaneous in selected time slots

//We need the references to the elements to be valid, so we need this to be a std::list
std::list<ActivitiesMaxSimultaneousInSelectedTimeSlots_item> amsistsList;
Matrix1D<QList<ActivitiesMaxSimultaneousInSelectedTimeSlots_item*>> amsistsListForActivity;

//bool computeActivitiesMaxSimultaneousInSelectedTimeSlots(QWidget* parent);

//2019-11-16 - Constraint activities min simultaneous in selected time slots

//We need the references to the elements to be valid, so we need this to be a std::list
std::list<ActivitiesMinSimultaneousInSelectedTimeSlots_item> aminsistsList;
Matrix1D<QList<ActivitiesMinSimultaneousInSelectedTimeSlots_item*>> aminsistsListForActivity;

//bool computeActivitiesMinSimultaneousInSelectedTimeSlots(QWidget* parent);

bool haveActivitiesOccupyMaxConstraints;
Matrix1D<bool> activityHasOccupyMaxConstraints;

bool haveActivitiesMaxSimultaneousConstraints;
Matrix1D<bool> activityHasMaxSimultaneousConstraints;

//2020-05-01 - Constraint max total activities from set in selected time slots
std::list<ActivitiesMaxTotalFromSetInSelectedTimeSlots_item> amtfsistsList;
Matrix1D<QList<ActivitiesMaxTotalFromSetInSelectedTimeSlots_item*>> amtfsistsListForActivity;
//bool computeActivitiesMaxTotalFromSetInSelectedTimeSlots(QWidget* parent);

//for the terms mode
//2020-01-14 - Constraint activities max in a term
std::list<ActivitiesMaxInATerm_item> amiatList;
Matrix1D<QList<ActivitiesMaxInATerm_item*>> amiatListForActivity;
//bool computeActivitiesMaxInATerm(QWidget* parent);

//2022-05-19 - Constraint activities min in a term
std::list<ActivitiesMinInATerm_item> aminiatList;
Matrix1D<QList<ActivitiesMinInATerm_item*>> aminiatListForActivity;
//bool computeActivitiesMinInATerm(QWidget* parent);

//2020-01-14 - Constraint activities occupy max terms
std::list<ActivitiesOccupyMaxTerms_item> aomtList;
Matrix1D<QList<ActivitiesOccupyMaxTerms_item*>> aomtListForActivity;
//bool computeActivitiesOccupyMaxTerms(QWidget* parent);

//2019-06-08 - Constraint students (set) min gaps between ordered pair of activity tags

//We need the references to the elements to be valid, so we need this to be a std::list
std::list<StudentsMinGapsBetweenOrderedPairOfActivityTags_item> smgbopoatList;
Matrix1D<QList<StudentsMinGapsBetweenOrderedPairOfActivityTags_item*>> smgbopoatListForActivity;

//bool computeStudentsMinGapsBetweenOrderedPairOfActivityTags(QWidget* parent);

//2019-06-08 - Constraint teacher(s) min gaps between ordered pair of activity tags

//We need the references to the elements to be valid, so we need this to be a std::list
std::list<TeachersMinGapsBetweenOrderedPairOfActivityTags_item> tmgbopoatList;
Matrix1D<QList<TeachersMinGapsBetweenOrderedPairOfActivityTags_item*>> tmgbopoatListForActivity;

//bool computeTeachersMinGapsBetweenOrderedPairOfActivityTags(QWidget* parent);

//2021-12-15 - Constraint students (set) min gaps between activity tag

//We need the references to the elements to be valid, so we need this to be a std::list
std::list<StudentsMinGapsBetweenActivityTag_item> smgbatList;
Matrix1D<QList<StudentsMinGapsBetweenActivityTag_item*>> smgbatListForActivity;

//bool computeStudentsMinGapsBetweenActivityTag(QWidget* parent);

//2021-12-15 - Constraint teacher(s) min gaps between activity tag

//We need the references to the elements to be valid, so we need this to be a std::list
std::list<TeachersMinGapsBetweenActivityTag_item> tmgbatList;
Matrix1D<QList<TeachersMinGapsBetweenActivityTag_item*>> tmgbatListForActivity;

//bool computeTeachersMinGapsBetweenActivityTag(QWidget* parent);

//2012-04-29 - Constraint activities occupy max different rooms

//We need the references to the elements to be valid, so we need this to be a std::list
std::list<ActivitiesOccupyMaxDifferentRooms_item> aomdrList;
Matrix1D<QList<ActivitiesOccupyMaxDifferentRooms_item*>> aomdrListForActivity;

//bool computeActivitiesOccupyMaxDifferentRooms(QWidget* parent);

//2013-09-14 - Constraint activities same room if consecutive

//We need the references to the elements to be valid, so we need this to be a std::list
std::list<ActivitiesSameRoomIfConsecutive_item> asricList;
Matrix1D<QList<ActivitiesSameRoomIfConsecutive_item*>> asricListForActivity;

//bool computeActivitiesSameRoomIfConsecutive(QWidget* parent);

/////////////////////////////////////////////////////////////////////////

//2019-11-20
//We need the references to the elements to be valid, so we need this to be a std::list
std::list<SubgroupActivityTagMinHoursDaily_item> satmhdList;
Matrix1D<QList<SubgroupActivityTagMinHoursDaily_item*>> satmhdListForSubgroup;
bool haveStudentsActivityTagMinHoursDaily;

//bool computeStudentsActivityTagMinHoursDaily(QWidget* parent);

//2019-11-20

//We need the references to the elements to be valid, so we need this to be a std::list
std::list<TeacherActivityTagMinHoursDaily_item> tatmhdList;
Matrix1D<QList<TeacherActivityTagMinHoursDaily_item*>> tatmhdListForTeacher;
bool haveTeachersActivityTagMinHoursDaily;

//bool computeTeachersActivityTagMinHoursDaily(QWidget* parent);

//2022-02-16 - speed improvement in the Mornings-Afternoons mode
Matrix1D<QList<int>> subgroupsForActivitiesOfTheDayMornings;
Matrix1D<QList<int>> subgroupsForActivitiesOfTheDayAfternoons;
////
Matrix1D<QList<int>> teachersForActivitiesOfTheDayMornings;
Matrix1D<QList<int>> teachersForActivitiesOfTheDayAfternoons;
////
//void computeSubgroupsTeachersForActivitiesOfTheDay();

/////////////////////////////////////////////////////////////////////////

Matrix1D<bool> fixedTimeActivity;
Matrix1D<bool> fixedSpaceActivity;
//bool computeFixedActivities(QWidget* parent);

//The following two functions are used in teacher room not available time constraints.
qint64 teacherRoomQInt64Combination(int teacher, int room)
{
	assert(teacher>=0);
	assert(teacher<gt.rules.nInternalTeachers);
	assert(room>=0);
	assert(room<gt.rules.nInternalRooms);

	return qint64(teacher)*qint64(gt.rules.nInternalRooms)+qint64(room);
}

qint64 dayHourQInt64Combination(int day, int hour)
{
	assert(hour>=0);
	assert(hour<gt.rules.nHoursPerDay);
	assert(day>=0);
	assert(day<gt.rules.nDaysPerWeek);

	return qint64(hour)*qint64(gt.rules.nDaysPerWeek)+qint64(day);
}

inline bool findEquivalentSubgroupsCompareFunction(int i1, int i2)
{
	const QList<int>& a1=gt.rules.internalSubgroupsList[i1]->activitiesForSubgroup;
	const QList<int>& a2=gt.rules.internalSubgroupsList[i2]->activitiesForSubgroup;
	
	if(a1.count()<a2.count()){
		return true;
	}
	else if(a1.count()>a2.count()){
		return false;
	}
	else{
		assert(a1.count()==a2.count());
		for(int i=0; i<a1.count(); i++){
			if(a1.at(i)<a2.at(i)){
				return true;
			}
			else if(a1.at(i)>a2.at(i)){
				return false;
			}
		}
	}
	return false;
}

////////////////////////////////////

inline bool compareFunctionGeneratePre(int i, int j)
{
	if(nIncompatible[i]>nIncompatible[j] || (nIncompatible[i]==nIncompatible[j] && nMinDaysConstraintsBroken[i]>nMinDaysConstraintsBroken[j]))
		return true;
	
	return false;
}

inline bool compareFunctionGeneratePreWithGroupedActivities(int i, int j)
{
	//nMinDaysBroken is different from 0.0 only if the activity is fixed
	if( nIncompatibleFromFather[i]>nIncompatibleFromFather[j]
	 || (nIncompatibleFromFather[i]==nIncompatibleFromFather[j] && nMinDaysConstraintsBroken[i]>nMinDaysConstraintsBroken[j])
	 || (nIncompatibleFromFather[i]==nIncompatibleFromFather[j] && nMinDaysConstraintsBroken[i]==nMinDaysConstraintsBroken[j] && nIncompatible[i]>nIncompatible[j] ) )
		return true;

	return false;
}


bool processTimeSpaceConstraints(QWidget* parent, QTextStream* initialOrderStream)
{
	assert(gt.rules.internalStructureComputed);

	if(gt.rules.mode==MORNINGS_AFTERNOONS && gt.rules.nDaysPerWeek%2 != 0){
		GeneratePreIrreconcilableMessage::mediumInformation(parent, GeneratePreTranslate::tr("FET warning"),
		 GeneratePreTranslate::tr("Cannot generate the timetable in the mornings-afternoons mode if the number of FET days per week is odd. "
		 "Please make it an even number and try again."));
		return false;
	}
	else if(gt.rules.mode==TERMS && gt.rules.nDaysPerWeek != gt.rules.nTerms * gt.rules.nDaysPerTerm){
		GeneratePreIrreconcilableMessage::mediumInformation(parent, GeneratePreTranslate::tr("FET warning"),
		 GeneratePreTranslate::tr("You are working in the Terms mode and the number of"
		 " days per week (%1) is not equal to the number of terms (%2) x the number of days per term (%3) - please correct this before generating.")
		 .arg(gt.rules.nDaysPerWeek).arg(gt.rules.nTerms).arg(gt.rules.nDaysPerTerm));
		return false;
	}

	QList<TimeConstraint*> incompatibleModeTimeConstraints;
	for(TimeConstraint* tc : qAsConst(gt.rules.timeConstraintsList))
		if((gt.rules.mode==OFFICIAL && !tc->canBeUsedInOfficialMode()) ||
		 (gt.rules.mode==MORNINGS_AFTERNOONS && !tc->canBeUsedInMorningsAfternoonsMode()) ||
		 (gt.rules.mode==BLOCK_PLANNING && !tc->canBeUsedInBlockPlanningMode()) ||
		 (gt.rules.mode==TERMS && !tc->canBeUsedInTermsMode()))
			incompatibleModeTimeConstraints.append(tc);

	QList<SpaceConstraint*> incompatibleModeSpaceConstraints;
	for(SpaceConstraint* sc : qAsConst(gt.rules.spaceConstraintsList))
		if((gt.rules.mode==OFFICIAL && !sc->canBeUsedInOfficialMode()) ||
		 (gt.rules.mode==MORNINGS_AFTERNOONS && !sc->canBeUsedInMorningsAfternoonsMode()) ||
		 (gt.rules.mode==BLOCK_PLANNING && !sc->canBeUsedInBlockPlanningMode()) ||
		 (gt.rules.mode==TERMS && !sc->canBeUsedInTermsMode()))
			incompatibleModeSpaceConstraints.append(sc);
	
	if(!incompatibleModeTimeConstraints.isEmpty() || !incompatibleModeSpaceConstraints.isEmpty()){
		QString s=GeneratePreTranslate::tr("You have constraints which are incompatible with the current mode. Please remove them"
		 " from the all time/space constraint dialogs or change the mode."
		 " If this is a FET bug, please report it. These constraints are listed below:");
		s+="\n\n";
		for(TimeConstraint* tc : qAsConst(incompatibleModeTimeConstraints)){
			s+=tc->getDetailedDescription(gt.rules);
			s+="\n";
		}
		for(SpaceConstraint* sc : qAsConst(incompatibleModeSpaceConstraints)){
			s+=sc->getDetailedDescription(gt.rules);
			s+="\n";
		}

		GeneratePreIrreconcilableMessage::mediumInformation(parent, GeneratePreTranslate::tr("FET warning"), s);
		return false;
	}

	////////////Compute equivalent subgroups
	if(SHOW_WARNING_FOR_SUBGROUPS_WITH_THE_SAME_ACTIVITIES){
		QList<int> tmpSortedSubgroupsList;
		Matrix1D<bool> isSignificantSubgroup;
		
		isSignificantSubgroup.resize(gt.rules.nInternalSubgroups);
		
		tmpSortedSubgroupsList.clear();
		for(int i=0; i<gt.rules.nInternalSubgroups; i++)
			tmpSortedSubgroupsList.append(i);
		
		std::stable_sort(tmpSortedSubgroupsList.begin(), tmpSortedSubgroupsList.end(), findEquivalentSubgroupsCompareFunction);
		
		QStringList s;
		
		if(gt.rules.nInternalSubgroups>0)
			isSignificantSubgroup[tmpSortedSubgroupsList.at(0)]=true;
		for(int i=1; i<gt.rules.nInternalSubgroups; i++){
			int s1=tmpSortedSubgroupsList.at(i-1);
			int s2=tmpSortedSubgroupsList.at(i);
			
			isSignificantSubgroup[s2]=true;
		
			const QList<int>& l1=gt.rules.internalSubgroupsList[s1]->activitiesForSubgroup;
			const QList<int>& l2=gt.rules.internalSubgroupsList[s2]->activitiesForSubgroup;
			if(l1.count()==l2.count()){
				int i;
				for(i=0; i<l1.count(); i++)
					if(l1.at(i)!=l2.at(i))
						break;
				if(i==l1.count()){
					isSignificantSubgroup[s2]=false;
					
					s.append(GeneratePreTranslate::tr("Subgroup %1 has the same activities as subgroup %2.").arg(gt.rules.internalSubgroupsList[s2]->name).arg(gt.rules.internalSubgroupsList[s1]->name));
				}
			}
		}
		
		int cnt=0;
		for(int i=0; i<gt.rules.nInternalSubgroups; i++)
			if(!isSignificantSubgroup[i])
				cnt++;
		
		if(cnt>0){
			QString s0=GeneratePreTranslate::tr("Optimization tip:");
			s0+=" ";
			s0+=GeneratePreTranslate::tr("There are %1 subgroups (from the total of %2 subgroups) which have the same activities as other subgroups. They are listed below."
				" If the constraints relating to these subgroups are also the same, you can make the generation (directly proportional) faster by completely removing the subgroups"
				" which are equivalent to other subgroups (leaving only one representative for each equivalence set). (The generation algorithm will not completely remove the equivalent"
				" subgroups automatically.)").arg(cnt).arg(gt.rules.nInternalSubgroups);
			s0+="\n\n";
			s0+=GeneratePreTranslate::tr("If you did not add all the activities yet or if the number of equivalent subgroups compared to the total number of subgroups"
				" is small, probably you can safely ignore this message.");
			s0+="\n\n";
			s0+=GeneratePreTranslate::tr("You can deactivate this message from the 'Settings' menu.");
			
			GeneratePreReconcilableMessage::largeInformation(parent, GeneratePreTranslate::tr("FET warning"), s0+QString("\n\n")+s.join("\n"));
		}
	}
	//////////////////////////////////

	//////////////////begin resizing

	//2021-03-18
	//permutation.resize(gt.rules.nInternalActivities);
	copyOfInitialPermutation.resize(gt.rules.nInternalActivities);
	//
	subgroupsEarlyMaxBeginningsAtSecondHourPercentage.resize(gt.rules.nInternalSubgroups);
	subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxGapsPerWeekPercentage.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxGapsPerWeekMaxGaps.resize(gt.rules.nInternalSubgroups);
	//
	nHoursPerSubgroup.resize(gt.rules.nInternalSubgroups);
	//
	subgroupsMaxGapsPerDayPercentage.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxGapsPerDayMaxGaps.resize(gt.rules.nInternalSubgroups);
	//
	subgroupsMaxDaysPerWeekMaxDays.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxDaysPerWeekWeightPercentages.resize(gt.rules.nInternalSubgroups);
	//
	teachersMaxDaysPerWeekMaxDays.resize(gt.rules.nInternalTeachers);
	teachersMaxDaysPerWeekWeightPercentages.resize(gt.rules.nInternalTeachers);
	//
	teachersMaxThreeConsecutiveDaysAllowAMAMException.resize(gt.rules.nInternalTeachers);
	teachersMaxThreeConsecutiveDaysPercentages.resize(gt.rules.nInternalTeachers); //-1 for not existing
	//
	subgroupsMaxThreeConsecutiveDaysAllowAMAMException.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxThreeConsecutiveDaysPercentages.resize(gt.rules.nInternalSubgroups); //-1 for not existing
	//
	teachersMaxGapsPerWeekPercentage.resize(gt.rules.nInternalTeachers);
	teachersMaxGapsPerWeekMaxGaps.resize(gt.rules.nInternalTeachers);
	teachersMaxGapsPerDayPercentage.resize(gt.rules.nInternalTeachers);
	teachersMaxGapsPerDayMaxGaps.resize(gt.rules.nInternalTeachers);
	teachersMaxGapsPerMorningAndAfternoonPercentage.resize(gt.rules.nInternalTeachers);
	teachersMaxGapsPerMorningAndAfternoonMaxGaps.resize(gt.rules.nInternalTeachers);
	nHoursPerTeacher.resize(gt.rules.nInternalTeachers);
	//
	teachersMaxHoursDailyPercentages1.resize(gt.rules.nInternalTeachers);
	teachersMaxHoursDailyMaxHours1.resize(gt.rules.nInternalTeachers);
	teachersMaxHoursDailyPercentages2.resize(gt.rules.nInternalTeachers);
	teachersMaxHoursDailyMaxHours2.resize(gt.rules.nInternalTeachers);
	//
	teachersMaxHoursContinuouslyPercentages1.resize(gt.rules.nInternalTeachers);
	teachersMaxHoursContinuouslyMaxHours1.resize(gt.rules.nInternalTeachers);
	teachersMaxHoursContinuouslyPercentages2.resize(gt.rules.nInternalTeachers);
	teachersMaxHoursContinuouslyMaxHours2.resize(gt.rules.nInternalTeachers);
	//
	teachersMinHoursDailyPercentages.resize(gt.rules.nInternalTeachers, 2);
	teachersMinHoursDailyMinHours.resize(gt.rules.nInternalTeachers, 2);
	teachersMinHoursDailyRealDaysPercentages.resize(gt.rules.nInternalTeachers);
	teachersMinHoursDailyRealDaysMinHours.resize(gt.rules.nInternalTeachers);

	teachersMinHoursPerAfternoonPercentages.resize(gt.rules.nInternalTeachers);
	teachersMinHoursPerAfternoonMinHours.resize(gt.rules.nInternalTeachers);
	//
	teachersMinDaysPerWeekPercentages.resize(gt.rules.nInternalTeachers);
	teachersMinDaysPerWeekMinDays.resize(gt.rules.nInternalTeachers);
	//
	teachersMaxRealDaysPerWeekMaxDays.resize(gt.rules.nInternalTeachers);
	teachersMaxRealDaysPerWeekWeightPercentages.resize(gt.rules.nInternalTeachers);
	//
	teachersMaxGapsPerRealDayPercentage.resize(gt.rules.nInternalTeachers);
	teachersMaxGapsPerRealDayMaxGaps.resize(gt.rules.nInternalTeachers);
	teachersMaxGapsPerRealDayAllowException.resize(gt.rules.nInternalTeachers);
	teachersMaxGapsPerWeekForRealDaysPercentage.resize(gt.rules.nInternalTeachers);
	teachersMaxGapsPerWeekForRealDaysMaxGaps.resize(gt.rules.nInternalTeachers);
	teacherConstrainedToZeroGapsPerAfternoon.resize(gt.rules.nInternalTeachers);
	//
	teachersMaxAfternoonsPerWeekMaxAfternoons.resize(gt.rules.nInternalTeachers);
	teachersMaxAfternoonsPerWeekWeightPercentages.resize(gt.rules.nInternalTeachers);
	teachersMaxMorningsPerWeekMaxMornings.resize(gt.rules.nInternalTeachers);
	teachersMaxMorningsPerWeekWeightPercentages.resize(gt.rules.nInternalTeachers);
	//
	teachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentage.resize(gt.rules.nInternalTeachers);
	teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings.resize(gt.rules.nInternalTeachers);
	//
	teachersMorningsEarlyMaxBeginningsAtSecondHourPercentage.resize(gt.rules.nInternalTeachers);
	teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings.resize(gt.rules.nInternalTeachers);
	//
	teachersMaxHoursDailyRealDaysPercentages1.resize(gt.rules.nInternalTeachers);
	teachersMaxHoursDailyRealDaysMaxHours1.resize(gt.rules.nInternalTeachers);
	teachersMaxHoursDailyRealDaysPercentages2.resize(gt.rules.nInternalTeachers);
	teachersMaxHoursDailyRealDaysMaxHours2.resize(gt.rules.nInternalTeachers);
	//
	teachersMinRealDaysPerWeekPercentages.resize(gt.rules.nInternalTeachers);
	teachersMinRealDaysPerWeekMinDays.resize(gt.rules.nInternalTeachers);
	teachersMinMorningsPerWeekPercentages.resize(gt.rules.nInternalTeachers);
	teachersMinMorningsPerWeekMinMornings.resize(gt.rules.nInternalTeachers);
	teachersMinAfternoonsPerWeekPercentages.resize(gt.rules.nInternalTeachers);
	teachersMinAfternoonsPerWeekMinAfternoons.resize(gt.rules.nInternalTeachers);
	teachersMaxHoursPerAllAfternoonsPercentages.resize(gt.rules.nInternalTeachers);
	teachersMaxHoursPerAllAfternoonsMaxHours.resize(gt.rules.nInternalTeachers);
	//
	subgroupsMaxAfternoonsPerWeekMaxAfternoons.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxAfternoonsPerWeekWeightPercentages.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxMorningsPerWeekMaxMornings.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxMorningsPerWeekWeightPercentages.resize(gt.rules.nInternalSubgroups);
	//
	subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentage.resize(gt.rules.nInternalSubgroups);
	subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings.resize(gt.rules.nInternalSubgroups);
	//
	subgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentage.resize(gt.rules.nInternalSubgroups);
	subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings.resize(gt.rules.nInternalSubgroups);
	//
	subgroupsMaxHoursDailyPercentages1.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxHoursDailyMaxHours1.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxHoursDailyPercentages2.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxHoursDailyMaxHours2.resize(gt.rules.nInternalSubgroups);
	//
	subgroupsMaxHoursContinuouslyPercentages1.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxHoursContinuouslyMaxHours1.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxHoursContinuouslyPercentages2.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxHoursContinuouslyMaxHours2.resize(gt.rules.nInternalSubgroups);
	//
	subgroupsMinHoursDailyPercentages.resize(gt.rules.nInternalSubgroups, 2);
	subgroupsMinHoursDailyMinHours.resize(gt.rules.nInternalSubgroups, 2);
	subgroupsMinHoursDailyAllowEmptyDays.resize(gt.rules.nInternalSubgroups);
	subgroupsMinHoursPerMorningAllowEmptyMornings.resize(gt.rules.nInternalSubgroups);

	subgroupsMinHoursPerAfternoonPercentages.resize(gt.rules.nInternalSubgroups);
	subgroupsMinHoursPerAfternoonMinHours.resize(gt.rules.nInternalSubgroups);
	subgroupsMinHoursPerAfternoonAllowEmptyAfternoons.resize(gt.rules.nInternalSubgroups);
	//
	subgroupsMaxGapsPerRealDayPercentage.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxGapsPerRealDayMaxGaps.resize(gt.rules.nInternalSubgroups);
	//
	subgroupsMaxGapsPerWeekForRealDaysPercentage.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxGapsPerWeekForRealDaysMaxGaps.resize(gt.rules.nInternalSubgroups);
	//
	subgroupsMaxRealDaysPerWeekMaxDays.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxRealDaysPerWeekWeightPercentages.resize(gt.rules.nInternalSubgroups);
	//
	subgroupsMaxHoursPerAllAfternoonsPercentages.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxHoursPerAllAfternoonsMaxHours.resize(gt.rules.nInternalSubgroups);
	subgroupsMinMorningsPerWeekPercentages.resize(gt.rules.nInternalSubgroups);
	subgroupsMinMorningsPerWeekMinMornings.resize(gt.rules.nInternalSubgroups);
	subgroupsMinAfternoonsPerWeekPercentages.resize(gt.rules.nInternalSubgroups);
	subgroupsMinAfternoonsPerWeekMinAfternoons.resize(gt.rules.nInternalSubgroups);
	//
	subgroupsMaxHoursDailyRealDaysPercentages1.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxHoursDailyRealDaysMaxHours1.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxHoursDailyRealDaysPercentages2.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxHoursDailyRealDaysMaxHours2.resize(gt.rules.nInternalSubgroups);
	//
	activityEndsStudentsDayPercentages.resize(gt.rules.nInternalActivities);
	activityEndsTeachersDayPercentages.resize(gt.rules.nInternalActivities);
	//
	activityBeginsStudentsDayPercentages.resize(gt.rules.nInternalActivities);
	activityBeginsTeachersDayPercentages.resize(gt.rules.nInternalActivities);
	//
	teachersMaxSpanPerDayMaxSpan.resize(gt.rules.nInternalTeachers);
	teachersMaxSpanPerDayPercentages.resize(gt.rules.nInternalTeachers);
	teachersMaxSpanPerDayAllowOneDayExceptionPlusOne.resize(gt.rules.nInternalTeachers);
	teachersMinRestingHoursCircularMinHours.resize(gt.rules.nInternalTeachers);
	teachersMinRestingHoursCircularPercentages.resize(gt.rules.nInternalTeachers);
	teachersMinRestingHoursNotCircularMinHours.resize(gt.rules.nInternalTeachers);
	teachersMinRestingHoursNotCircularPercentages.resize(gt.rules.nInternalTeachers);
	//
	teachersMaxTwoConsecutiveMorningsPercentage.resize(gt.rules.nInternalTeachers);
	teachersMaxTwoConsecutiveAfternoonsPercentage.resize(gt.rules.nInternalTeachers);
	//
	teachersMaxSpanPerRealDayMaxSpan.resize(gt.rules.nInternalTeachers);
	teachersMaxSpanPerRealDayPercentages.resize(gt.rules.nInternalTeachers);
	teachersMaxSpanPerRealDayAllowOneDayExceptionPlusOne.resize(gt.rules.nInternalTeachers);
	//
	teachersMinRestingHoursBetweenMorningAndAfternoonMinHours.resize(gt.rules.nInternalTeachers);
	teachersMinRestingHoursBetweenMorningAndAfternoonPercentages.resize(gt.rules.nInternalTeachers);
	//
	teachersMaxTwoActivityTagsPerDayFromN1N2N3Percentages.resize(gt.rules.nInternalTeachers);
	subgroupsMaxTwoActivityTagsPerDayFromN1N2N3Percentages.resize(gt.rules.nInternalSubgroups);
	//
	teachersMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages.resize(gt.rules.nInternalTeachers);
	subgroupsMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages.resize(gt.rules.nInternalSubgroups);
	//
	subgroupsMinRestingHoursBetweenMorningAndAfternoonMinHours.resize(gt.rules.nInternalSubgroups);
	subgroupsMinRestingHoursBetweenMorningAndAfternoonPercentages.resize(gt.rules.nInternalSubgroups);
	//
	subgroupsMaxSpanPerDayMaxSpan.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxSpanPerDayPercentages.resize(gt.rules.nInternalSubgroups);
	subgroupsMinRestingHoursCircularMinHours.resize(gt.rules.nInternalSubgroups);
	subgroupsMinRestingHoursCircularPercentages.resize(gt.rules.nInternalSubgroups);
	subgroupsMinRestingHoursNotCircularMinHours.resize(gt.rules.nInternalSubgroups);
	subgroupsMinRestingHoursNotCircularPercentages.resize(gt.rules.nInternalSubgroups);
	//
	subgroupsMaxSpanPerRealDayMaxSpan.resize(gt.rules.nInternalSubgroups);
	subgroupsMaxSpanPerRealDayPercentages.resize(gt.rules.nInternalSubgroups);
	//
	unspecifiedPreferredRoom.resize(gt.rules.nInternalActivities);
	activitiesHomeRoomsPercentage.resize(gt.rules.nInternalActivities);
	unspecifiedHomeRoom.resize(gt.rules.nInternalActivities);
	//
	maxBuildingChangesPerWeekForStudentsPercentages.resize(gt.rules.nInternalSubgroups);
	maxBuildingChangesPerWeekForStudentsMaxChanges.resize(gt.rules.nInternalSubgroups);
	maxBuildingChangesPerDayForStudentsPercentages.resize(gt.rules.nInternalSubgroups);
	maxBuildingChangesPerDayForStudentsMaxChanges.resize(gt.rules.nInternalSubgroups);
	minGapsBetweenBuildingChangesForStudentsPercentages.resize(gt.rules.nInternalSubgroups);
	minGapsBetweenBuildingChangesForStudentsMinGaps.resize(gt.rules.nInternalSubgroups);
	//
	maxBuildingChangesPerWeekForTeachersPercentages.resize(gt.rules.nInternalTeachers);
	maxBuildingChangesPerWeekForTeachersMaxChanges.resize(gt.rules.nInternalTeachers);
	maxBuildingChangesPerDayForTeachersPercentages.resize(gt.rules.nInternalTeachers);
	maxBuildingChangesPerDayForTeachersMaxChanges.resize(gt.rules.nInternalTeachers);
	minGapsBetweenBuildingChangesForTeachersPercentages.resize(gt.rules.nInternalTeachers);
	minGapsBetweenBuildingChangesForTeachersMinGaps.resize(gt.rules.nInternalTeachers);
	//
	maxBuildingChangesPerRealDayForTeachersPercentages.resize(gt.rules.nInternalTeachers);
	maxBuildingChangesPerRealDayForTeachersMaxChanges.resize(gt.rules.nInternalTeachers);
	//
	maxRoomChangesPerRealDayForTeachersPercentages.resize(gt.rules.nInternalTeachers);
	maxRoomChangesPerRealDayForTeachersMaxChanges.resize(gt.rules.nInternalTeachers);
	//
	maxRoomChangesPerWeekForStudentsPercentages.resize(gt.rules.nInternalSubgroups);
	maxRoomChangesPerWeekForStudentsMaxChanges.resize(gt.rules.nInternalSubgroups);
	maxRoomChangesPerDayForStudentsPercentages.resize(gt.rules.nInternalSubgroups);
	maxRoomChangesPerDayForStudentsMaxChanges.resize(gt.rules.nInternalSubgroups);
	minGapsBetweenRoomChangesForStudentsPercentages.resize(gt.rules.nInternalSubgroups);
	minGapsBetweenRoomChangesForStudentsMinGaps.resize(gt.rules.nInternalSubgroups);
	//
	maxBuildingChangesPerRealDayForSubgroupsPercentages.resize(gt.rules.nInternalSubgroups);
	maxBuildingChangesPerRealDayForSubgroupsMaxChanges.resize(gt.rules.nInternalSubgroups);
	//
	maxRoomChangesPerRealDayForSubgroupsPercentages.resize(gt.rules.nInternalSubgroups);
	maxRoomChangesPerRealDayForSubgroupsMaxChanges.resize(gt.rules.nInternalSubgroups);
	//
	maxRoomChangesPerWeekForTeachersPercentages.resize(gt.rules.nInternalTeachers);
	maxRoomChangesPerWeekForTeachersMaxChanges.resize(gt.rules.nInternalTeachers);
	maxRoomChangesPerDayForTeachersPercentages.resize(gt.rules.nInternalTeachers);
	maxRoomChangesPerDayForTeachersMaxChanges.resize(gt.rules.nInternalTeachers);
	minGapsBetweenRoomChangesForTeachersPercentages.resize(gt.rules.nInternalTeachers);
	minGapsBetweenRoomChangesForTeachersMinGaps.resize(gt.rules.nInternalTeachers);
	//
	mustComputeTimetableSubgroup.resize(gt.rules.nInternalSubgroups);
	mustComputeTimetableTeacher.resize(gt.rules.nInternalTeachers);
	//
	activityTagN1N2N3.resize(gt.rules.nInternalActivities);
	//
	activityHasOccupyMaxConstraints.resize(gt.rules.nInternalActivities);
	activityHasMaxSimultaneousConstraints.resize(gt.rules.nInternalActivities);
	//
	fixedTimeActivity.resize(gt.rules.nInternalActivities);
	fixedSpaceActivity.resize(gt.rules.nInternalActivities);
	//
	initialOrderOfActivitiesIndices.resize(gt.rules.nInternalActivities);
	//
	daysTeacherIsAvailable.resize(gt.rules.nInternalTeachers);
	daysSubgroupIsAvailable.resize(gt.rules.nInternalSubgroups);
	requestedDaysForTeachers.resize(gt.rules.nInternalTeachers);
	requestedDaysForSubgroups.resize(gt.rules.nInternalSubgroups);
	nReqForTeacher.resize(gt.rules.nInternalTeachers);
	nReqForSubgroup.resize(gt.rules.nInternalSubgroups);
	//
	nHoursRequiredForRoom.resize(gt.rules.nInternalRooms);
	nHoursAvailableForRoom.resize(gt.rules.nInternalRooms);
	//
	nIncompatible.resize(gt.rules.nInternalActivities);
	nMinDaysConstraintsBroken.resize(gt.rules.nInternalActivities);
	nRoomsIncompat.resize(gt.rules.nInternalRooms);
	nHoursForRoom.resize(gt.rules.nInternalRooms);
	maxPercentagePrefRooms.resize(gt.rules.nInternalActivities);
	reprNInc.resize(gt.rules.nInternalActivities);
	//
	nIncompatibleFromFather.resize(gt.rules.nInternalActivities);
	fatherActivityInInitialOrder.resize(gt.rules.nInternalActivities);

	//MIN DAYS BETWEEN ACTIVITIES
	minDaysListOfActivities.resize(gt.rules.nInternalActivities);
	minDaysListOfMinDays.resize(gt.rules.nInternalActivities);
	minDaysListOfWeightPercentages.resize(gt.rules.nInternalActivities);
	minDaysListOfConsecutiveIfSameDay.resize(gt.rules.nInternalActivities);
	minDaysListOfActivitiesFromTheSameConstraint.resize(gt.rules.nInternalActivities);
	
	//MIN HALF DAYS BETWEEN ACTIVITIES
	minHalfDaysListOfActivities.resize(gt.rules.nInternalActivities);
	minHalfDaysListOfMinDays.resize(gt.rules.nInternalActivities);
	minHalfDaysListOfWeightPercentages.resize(gt.rules.nInternalActivities);
	minHalfDaysListOfConsecutiveIfSameDay.resize(gt.rules.nInternalActivities);
	minHalfDaysListOfActivitiesFromTheSameConstraint.resize(gt.rules.nInternalActivities);

	//MAX DAYS BETWEEN ACTIVITIES
	maxDaysListOfActivities.resize(gt.rules.nInternalActivities);
	maxDaysListOfMaxDays.resize(gt.rules.nInternalActivities);
	maxDaysListOfWeightPercentages.resize(gt.rules.nInternalActivities);

	//MAX HALF DAYS BETWEEN ACTIVITIES
	maxHalfDaysListOfActivities.resize(gt.rules.nInternalActivities);
	maxHalfDaysListOfMaxDays.resize(gt.rules.nInternalActivities);
	maxHalfDaysListOfWeightPercentages.resize(gt.rules.nInternalActivities);

	//MAX TERMS BETWEEN ACTIVITIES
	maxTermsListOfActivities.resize(gt.rules.nInternalActivities);
	maxTermsListOfMaxTerms.resize(gt.rules.nInternalActivities);
	maxTermsListOfWeightPercentages.resize(gt.rules.nInternalActivities);

	//MIN GAPS BETWEEN ACTIVITIES
	minGapsBetweenActivitiesListOfActivities.resize(gt.rules.nInternalActivities);
	minGapsBetweenActivitiesListOfMinGaps.resize(gt.rules.nInternalActivities);
	minGapsBetweenActivitiesListOfWeightPercentages.resize(gt.rules.nInternalActivities);

	//MAX GAPS BETWEEN ACTIVITIES
	maxGapsBetweenActivitiesListOfActivities.resize(gt.rules.nInternalActivities);
	maxGapsBetweenActivitiesListOfMaxGaps.resize(gt.rules.nInternalActivities);
	maxGapsBetweenActivitiesListOfWeightPercentages.resize(gt.rules.nInternalActivities);

	teachersWithMaxDaysPerWeekForActivities.resize(gt.rules.nInternalActivities);
	subgroupsWithMaxDaysPerWeekForActivities.resize(gt.rules.nInternalActivities);

	teachersWithMaxThreeConsecutiveDaysForActivities.resize(gt.rules.nInternalActivities);
	subgroupsWithMaxThreeConsecutiveDaysForActivities.resize(gt.rules.nInternalActivities);

	teachersWithMaxRealDaysPerWeekForActivities.resize(gt.rules.nInternalActivities);

	teachersWithMaxAfternoonsPerWeekForActivities.resize(gt.rules.nInternalActivities);
	teachersWithMaxMorningsPerWeekForActivities.resize(gt.rules.nInternalActivities);

	subgroupsWithMaxAfternoonsPerWeekForActivities.resize(gt.rules.nInternalActivities);
	subgroupsWithMaxMorningsPerWeekForActivities.resize(gt.rules.nInternalActivities);

	subgroupsWithMaxRealDaysPerWeekForActivities.resize(gt.rules.nInternalActivities);

	//activities same starting time
	activitiesSameStartingTimeActivities.resize(gt.rules.nInternalActivities);
	activitiesSameStartingTimePercentages.resize(gt.rules.nInternalActivities);

	//activities same starting hour
	activitiesSameStartingHourActivities.resize(gt.rules.nInternalActivities);
	activitiesSameStartingHourPercentages.resize(gt.rules.nInternalActivities);

	//activities same starting day
	activitiesSameStartingDayActivities.resize(gt.rules.nInternalActivities);
	activitiesSameStartingDayPercentages.resize(gt.rules.nInternalActivities);

	//activities not overlapping
	activitiesNotOverlappingActivities.resize(gt.rules.nInternalActivities);
	activitiesNotOverlappingPercentages.resize(gt.rules.nInternalActivities);

	// 2 activities consecutive
	//index represents the first activity, value in array represents the second activity
	constrTwoActivitiesConsecutivePercentages.resize(gt.rules.nInternalActivities);
	constrTwoActivitiesConsecutiveActivities.resize(gt.rules.nInternalActivities);

	//index represents the second activity, value in array represents the first activity
	inverseConstrTwoActivitiesConsecutivePercentages.resize(gt.rules.nInternalActivities);
	inverseConstrTwoActivitiesConsecutiveActivities.resize(gt.rules.nInternalActivities);
	// 2 activities consecutive

	// 2 activities grouped
	//index represents the first activity, value in array represents the second activity
	constrTwoActivitiesGroupedPercentages.resize(gt.rules.nInternalActivities);
	constrTwoActivitiesGroupedActivities.resize(gt.rules.nInternalActivities);

	// 3 activities grouped
	//index represents the first activity, value in array represents the second activity
	constrThreeActivitiesGroupedPercentages.resize(gt.rules.nInternalActivities);
	constrThreeActivitiesGroupedActivities.resize(gt.rules.nInternalActivities);

	// 2 activities ordered
	//index represents the first activity, value in array represents the second activity
	constrTwoActivitiesOrderedPercentages.resize(gt.rules.nInternalActivities);
	constrTwoActivitiesOrderedActivities.resize(gt.rules.nInternalActivities);

	//index represents the second activity, value in array represents the first activity
	inverseConstrTwoActivitiesOrderedPercentages.resize(gt.rules.nInternalActivities);
	inverseConstrTwoActivitiesOrderedActivities.resize(gt.rules.nInternalActivities);
	// 2 activities ordered

	// 2 activities ordered if same day
	//index represents the first activity, value in array represents the second activity
	constrTwoActivitiesOrderedIfSameDayPercentages.resize(gt.rules.nInternalActivities);
	constrTwoActivitiesOrderedIfSameDayActivities.resize(gt.rules.nInternalActivities);

	//index represents the second activity, value in array represents the first activity
	inverseConstrTwoActivitiesOrderedIfSameDayPercentages.resize(gt.rules.nInternalActivities);
	inverseConstrTwoActivitiesOrderedIfSameDayActivities.resize(gt.rules.nInternalActivities);
	// 2 activities ordered if same day

	//rooms
	activitiesPreferredRoomsList.resize(gt.rules.nInternalActivities);

	activitiesHomeRoomsHomeRooms.resize(gt.rules.nInternalActivities);
	////////rooms

	mustComputeTimetableSubgroups.resize(gt.rules.nInternalActivities);
	mustComputeTimetableTeachers.resize(gt.rules.nInternalActivities);

	//2021-02-20
	teachersIntervalMaxDaysPerWeekPercentages.resize(gt.rules.nInternalTeachers);
	teachersIntervalMaxDaysPerWeekMaxDays.resize(gt.rules.nInternalTeachers);
	teachersIntervalMaxDaysPerWeekIntervalStart.resize(gt.rules.nInternalTeachers);
	teachersIntervalMaxDaysPerWeekIntervalEnd.resize(gt.rules.nInternalTeachers);

	teachersMorningIntervalMaxDaysPerWeekPercentages.resize(gt.rules.nInternalTeachers);
	teachersMorningIntervalMaxDaysPerWeekMaxDays.resize(gt.rules.nInternalTeachers);
	teachersMorningIntervalMaxDaysPerWeekIntervalStart.resize(gt.rules.nInternalTeachers);
	teachersMorningIntervalMaxDaysPerWeekIntervalEnd.resize(gt.rules.nInternalTeachers);

	teachersAfternoonIntervalMaxDaysPerWeekPercentages.resize(gt.rules.nInternalTeachers);
	teachersAfternoonIntervalMaxDaysPerWeekMaxDays.resize(gt.rules.nInternalTeachers);
	teachersAfternoonIntervalMaxDaysPerWeekIntervalStart.resize(gt.rules.nInternalTeachers);
	teachersAfternoonIntervalMaxDaysPerWeekIntervalEnd.resize(gt.rules.nInternalTeachers);

	subgroupsIntervalMaxDaysPerWeekPercentages.resize(gt.rules.nInternalSubgroups);
	subgroupsIntervalMaxDaysPerWeekMaxDays.resize(gt.rules.nInternalSubgroups);
	subgroupsIntervalMaxDaysPerWeekIntervalStart.resize(gt.rules.nInternalSubgroups);
	subgroupsIntervalMaxDaysPerWeekIntervalEnd.resize(gt.rules.nInternalSubgroups);

	subgroupsMorningIntervalMaxDaysPerWeekPercentages.resize(gt.rules.nInternalSubgroups);
	subgroupsMorningIntervalMaxDaysPerWeekMaxDays.resize(gt.rules.nInternalSubgroups);
	subgroupsMorningIntervalMaxDaysPerWeekIntervalStart.resize(gt.rules.nInternalSubgroups);
	subgroupsMorningIntervalMaxDaysPerWeekIntervalEnd.resize(gt.rules.nInternalSubgroups);

	subgroupsAfternoonIntervalMaxDaysPerWeekPercentages.resize(gt.rules.nInternalSubgroups);
	subgroupsAfternoonIntervalMaxDaysPerWeekMaxDays.resize(gt.rules.nInternalSubgroups);
	subgroupsAfternoonIntervalMaxDaysPerWeekIntervalStart.resize(gt.rules.nInternalSubgroups);
	subgroupsAfternoonIntervalMaxDaysPerWeekIntervalEnd.resize(gt.rules.nInternalSubgroups);

	//////teachers and subgroups activity tag max hours daily and continuously
	teachersActivityTagMaxHoursDailyMaxHours.resize(gt.rules.nInternalTeachers);
	teachersActivityTagMaxHoursDailyActivityTag.resize(gt.rules.nInternalTeachers);
	teachersActivityTagMaxHoursDailyPercentage.resize(gt.rules.nInternalTeachers);

	teachersActivityTagMaxHoursDailyRealDaysMaxHours.resize(gt.rules.nInternalTeachers);
	teachersActivityTagMaxHoursDailyRealDaysActivityTag.resize(gt.rules.nInternalTeachers);
	teachersActivityTagMaxHoursDailyRealDaysPercentage.resize(gt.rules.nInternalTeachers);

	teachersActivityTagMaxHoursContinuouslyMaxHours.resize(gt.rules.nInternalTeachers);
	teachersActivityTagMaxHoursContinuouslyActivityTag.resize(gt.rules.nInternalTeachers);
	teachersActivityTagMaxHoursContinuouslyPercentage.resize(gt.rules.nInternalTeachers);

	teachersWithN1N2N3ForActivities.resize(gt.rules.nInternalActivities);
	subgroupsWithN1N2N3ForActivities.resize(gt.rules.nInternalActivities);

	subgroupsActivityTagMaxHoursDailyMaxHours.resize(gt.rules.nInternalSubgroups);
	subgroupsActivityTagMaxHoursDailyActivityTag.resize(gt.rules.nInternalSubgroups);
	subgroupsActivityTagMaxHoursDailyPercentage.resize(gt.rules.nInternalSubgroups);

	subgroupsActivityTagMaxHoursDailyRealDaysMaxHours.resize(gt.rules.nInternalSubgroups);
	subgroupsActivityTagMaxHoursDailyRealDaysActivityTag.resize(gt.rules.nInternalSubgroups);
	subgroupsActivityTagMaxHoursDailyRealDaysPercentage.resize(gt.rules.nInternalSubgroups);

	subgroupsActivityTagMaxHoursContinuouslyMaxHours.resize(gt.rules.nInternalSubgroups);
	subgroupsActivityTagMaxHoursContinuouslyActivityTag.resize(gt.rules.nInternalSubgroups);
	subgroupsActivityTagMaxHoursContinuouslyPercentage.resize(gt.rules.nInternalSubgroups);
	
	satmhdListForSubgroup.resize(gt.rules.nInternalSubgroups);
	tatmhdListForTeacher.resize(gt.rules.nInternalTeachers);
	
	//2011-09-25
	aomtsListForActivity.resize(gt.rules.nInternalActivities);
	//2019-11-16
	aomintsListForActivity.resize(gt.rules.nInternalActivities);
	//2011-09-30
	amsistsListForActivity.resize(gt.rules.nInternalActivities);
	//2019-11-16
	aminsistsListForActivity.resize(gt.rules.nInternalActivities);

	//2020-05-02
	amtfsistsListForActivity.resize(gt.rules.nInternalActivities);

	//for terms
	//2020-01-14
	amiatListForActivity.resize(gt.rules.nInternalActivities);
	//2022-05-19
	aminiatListForActivity.resize(gt.rules.nInternalActivities);
	//2020-01-14
	aomtListForActivity.resize(gt.rules.nInternalActivities);

	//2019-06-08
	smgbopoatListForActivity.resize(gt.rules.nInternalActivities);
	//2019-06-08
	tmgbopoatListForActivity.resize(gt.rules.nInternalActivities);

	//2021-12-15
	smgbatListForActivity.resize(gt.rules.nInternalActivities);
	//2021-12-15
	tmgbatListForActivity.resize(gt.rules.nInternalActivities);

	//2012-04-29
	aomdrListForActivity.resize(gt.rules.nInternalActivities);

	//2013-09-14
	asricListForActivity.resize(gt.rules.nInternalActivities);
	
	//2022-02-16
	subgroupsForActivitiesOfTheDayMornings.resize(gt.rules.nInternalActivities);
	subgroupsForActivitiesOfTheDayAfternoons.resize(gt.rules.nInternalActivities);
	////
	teachersForActivitiesOfTheDayMornings.resize(gt.rules.nInternalActivities);
	teachersForActivitiesOfTheDayAfternoons.resize(gt.rules.nInternalActivities);

	//////////////////end resizing - new feature
	
	QHash<int, int> reprSameStartingTime;
	QHash<int, QSet<int>> reprSameActivitiesSet;

	/////1. BASIC TIME CONSTRAINTS
	bool t=computeActivitiesConflictingPercentage(parent);
	if(!t)
		return false;
	//////////////////////////////
	
	/////2. min days between activities
	t=computeMinDays(parent);
	if(!t)
		return false;
	/////////////////////////////////////
	
	/////2.1 min half days between activities
	t=computeMinHalfDays(parent);
	if(!t)
		return false;
	/////////////////////////////////////
	
	/////2.3. max days between activities
	t=computeMaxDays(parent);
	if(!t)
		return false;
	/////////////////////////////////////
	
	/////2.35. max half days between activities
	t=computeMaxHalfDays(parent);
	if(!t)
		return false;
	/////////////////////////////////////
	
	/////2.4. max terms between activities
	t=computeMaxTerms(parent);
	if(!t)
		return false;
	/////////////////////////////////////
	
	/////2.5. min gaps between activities
	t=computeMinGapsBetweenActivities(parent);
	if(!t)
		return false;
	/////////////////////////////////////
	
	/////2.6. max gaps between activities
	t=computeMaxGapsBetweenActivities(parent);
	if(!t)
		return false;
	/////////////////////////////////////

	/////3. students not available, teachers not available, break, activity preferred time,
	/////   activity preferred times, activities preferred times
	t=computeNotAllowedTimesPercentages(parent);
	if(!t)
		return false;
	///////////////////////////////////////////////////////////////
	
	/////3.5. STUDENTS MAX DAYS PER WEEK
	t=computeMaxDaysPerWeekForStudents(parent);
	if(!t)
		return false;

	t=computeMaxThreeConsecutiveDaysForStudents(parent);
	if(!t)
		return false;

	t=computeMaxRealDaysPerWeekForStudents(parent);
	if(!t)
		return false;
	//////////////////////////////////

	//3.6. students max afternoons/mornings per week
	t=computeMaxAfternoonsPerWeekForStudents(parent);
	if(!t)
		return false;
	t=computeMaxMorningsPerWeekForStudents(parent);
	if(!t)
		return false;
	//////////////////////////////////

	/////4. students max gaps and early
	t=computeNHoursPerSubgroup(parent);
	if(!t)
		return false;
	t=computeSubgroupsEarlyAndMaxGapsPercentages(parent);
	if(!t)
		return false;
	t=computeSubgroupsMaxGapsPerDayPercentages(parent); //!!!after max gaps per week
	if(!t)
		return false;
	t=computeSubgroupsMaxGapsPerRealDayPercentages(parent); //Old useless comment? !!!after max gaps per week
	if(!t)
		return false;

	//!!!After subgroups early
	t=computeSubgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentages(parent);
	if(!t)
		return false;
	//!!!After subgroups early and subgroups afternoons early
	t=computeSubgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentages(parent);
	if(!t)
		return false;
	//////////////////////////////////
	
	/////5. TEACHERS MAX DAYS PER WEEK (and max three days consecutive)
	t=computeMaxDaysPerWeekForTeachers(parent);
	if(!t)
		return false;
	t=computeMaxThreeConsecutiveDaysForTeachers(parent);
	if(!t)
		return false;
	t=computeMaxRealDaysPerWeekForTeachers(parent);
	if(!t)
		return false;

	t=computeMaxAfternoonsPerWeekForTeachers(parent);
	if(!t)
		return false;
	t=computeMaxMorningsPerWeekForTeachers(parent);
	if(!t)
		return false;
	//////////////////////////////////
	
	
	/////6. TEACHERS MAX GAPS PER WEEK/DAY
	t=computeNHoursPerTeacher(parent);
	if(!t)
		return false;
	t=computeTeachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentages(parent);
	if(!t)
		return false;
	t=computeTeachersMorningsEarlyMaxBeginningsAtSecondHourPercentages(parent);
	if(!t)
		return false;
	t=computeTeachersMaxGapsPerWeekPercentage(parent);
	if(!t)
		return false;
	t=computeTeachersMaxGapsPerDayPercentage(parent);
	if(!t)
		return false;
	//!!!After max gaps per week and per day
	t=computeTeachersMaxGapsPerMorningAndAfternoonPercentage(parent);
	if(!t)
		return false;
	t=computeTeachersMaxGapsPerRealDayPercentage(parent);
	if(!t)
		return false;
	t=computeTeachersConstrainedToZeroGapsPerAfternoon(parent);
	if(!t)
		return false;
	//////////////////////////////////
	
	//must be AFTER basic time constraints (computeActivitiesConflictingPercentage)
	t=computeActivitiesSameStartingTime(parent, reprSameStartingTime, reprSameActivitiesSet);
	if(!t)
		return false;

	computeActivitiesSameStartingHour();
	
	computeActivitiesSameStartingDay();
	
	computeActivitiesNotOverlapping();

	//must be after allowed times, after n hours per teacher and after max days per week for teachers
	thereAreTeachersWithMaxHoursDailyOrPerRealDayWithUnder100Weight=false;
	//deprecated comment below
	//after teachers max hours daily, because of initializing with false in mhd of the variable which represents true/false <100.0% constraints of this type.
	t=computeTeachersMaxHoursDaily(parent);
	if(!t)
		return false;
	t=computeTeachersMaxHoursDailyRealDays(parent);
	if(!t)
		return false;

	t=computeTeachersMaxHoursContinuously(parent);
	if(!t)
		return false;

	t=computeTeachersActivityTagMaxHoursDaily(parent);
	if(!t)
		return false;
	t=computeTeachersActivityTagMaxHoursDailyRealDays(parent);
	if(!t)
		return false;


	t=computeTeachersActivityTagMaxHoursContinuously(parent);
	if(!t)
		return false;

	//must be after n hours per teacher
	t=computeTeachersMinHoursDaily(parent);
	if(!t)
		return false;
	t=computeTeachersMinHoursDailyRealDays(parent);
	if(!t)
		return false;

	//must be after teachers min hours daily and min hours daily real days
	t=computeTeachersMinDaysPerWeek(parent);
	if(!t)
		return false;
	t=computeTeachersMinRealDaysPerWeek(parent);
	if(!t)
		return false;

	//must be after teachers min hours daily (I think not), teachers max gaps per day and teachers max gaps per week
	t=computeTeachersMinMorningsAfternoonsPerWeek(parent);
	if(!t)
		return false;

	//2020-06-28
	//must be after n hours per teacher
	t=computeTeachersMaxHoursPerAllAfternoons(parent);
	if(!t)
		return false;

	//must be after allowed times, after n hours per subgroup and after max days per week for subgroups
	//thereAreSubgroupsWithMaxHoursDailyWithUnder100Weight=false;
	thereAreSubgroupsWithMaxHoursDailyOrPerRealDayWithUnder100Weight=false;
	t=computeSubgroupsMaxHoursDaily(parent);
	if(!t)
		return false;
	//must be after allowed times and after n hours per subgroup
	t=computeSubgroupsMaxHoursDailyRealDays(parent);
	if(!t)
		return false;

	t=computeStudentsMaxHoursContinuously(parent);
	if(!t)
		return false;

	t=computeStudentsMaxHoursContinuously(parent);
	if(!t)
		return false;

	t=computeStudentsActivityTagMaxHoursDaily(parent);
	if(!t)
		return false;

	t=computeStudentsActivityTagMaxHoursDailyRealDays(parent);
	if(!t)
		return false;

	t=computeStudentsActivityTagMaxHoursContinuously(parent);
	if(!t)
		return false;
		
	//after max hours daily/continuously without/with an activity tag
	t=checkMaxHoursForActivityDuration(parent);
	if(!t)
		return false;

	t=computeSubgroupsMinHoursDaily(parent);
	if(!t)
		return false;

	//must be after students min hours daily (I think not), students max gaps per day and students max gaps per week (I think not)
	t=computeStudentsMinMorningsAfternoonsPerWeek(parent);
	if(!t)
		return false;

	//2020-06-28
	//must be after n hours per subgroup
	t=computeStudentsMaxHoursPerAllAfternoons(parent);
	if(!t)
		return false;

	t=computeStudentsActivityTagMinHoursDaily(parent);
	if(!t)
		return false;
		
	t=computeTeachersActivityTagMinHoursDaily(parent);
	if(!t)
		return false;
		
	computeConstrTwoActivitiesConsecutive();
	
	computeConstrTwoActivitiesGrouped();
	
	computeConstrThreeActivitiesGrouped();
	
	computeConstrTwoActivitiesOrdered();
	
	computeConstrTwoActivitiesOrderedIfSameDay();
	
	t=computeActivityEndsStudentsDayPercentages(parent);
	if(!t)
		return false;

	t=computeActivityEndsTeachersDayPercentages(parent);
	if(!t)
		return false;
		
	t=computeActivityBeginsStudentsDayPercentages(parent);
	if(!t)
		return false;

	t=computeActivityBeginsTeachersDayPercentages(parent);
	if(!t)
		return false;
		
	//check for impossible min days
	t=checkMinDays100Percent(parent);
	if(!t)
		return false;
	t=checkMinDaysMaxTwoOnSameDay(parent);
	if(!t)
		return false;
	
	//check for impossible min half days
	t=checkMinHalfDays100Percent(parent);
	if(!t)
		return false;
	t=checkMinHalfDaysMaxTwoOnSameDay(parent);
	if(!t)
		return false;
	
	//check teachers interval max days per week
	t=computeTeachersIntervalMaxDaysPerWeek(parent);
	if(!t)
		return false;
	
	//check teachers morning interval max days per week
	t=computeTeachersMorningIntervalMaxDaysPerWeek(parent);
	if(!t)
		return false;

	//check teachers afternoon interval max days per week
	t=computeTeachersAfternoonIntervalMaxDaysPerWeek(parent);
	if(!t)
		return false;

	t=computeTeachersMaxTwoConsecutiveMornings(parent);
	if(!t)
		return false;
	t=computeTeachersMaxTwoConsecutiveAfternoons(parent);
	if(!t)
		return false;

	//2017-02-06
	t=computeTeachersMaxSpanPerDay(parent);
	if(!t)
		return false;

	//2017-02-06
	t=computeTeachersMaxSpanPerRealDay(parent);
	if(!t)
		return false;

	//check subgroups interval max days per week
	t=computeSubgroupsIntervalMaxDaysPerWeek(parent);
	if(!t)
		return false;

	t=computeSubgroupsMorningIntervalMaxDaysPerWeek(parent);
	if(!t)
		return false;
	t=computeSubgroupsAfternoonIntervalMaxDaysPerWeek(parent);
	if(!t)
		return false;

	/////////
	t=computeSubgroupsMaxSpanPerDay(parent);
	if(!t)
		return false;

	t=computeSubgroupsMaxSpanPerRealDay(parent);
	if(!t)
		return false;
	/////////

	t=computeTeachersMinRestingHours(parent);
	if(!t)
		return false;

	t=computeSubgroupsMinRestingHours(parent);
	if(!t)
		return false;

	t=computeTeachersMinRestingHoursBetweenMorningAndAfternoon(parent);
	if(!t)
		return false;

	t=computeSubgroupsMinRestingHoursBetweenMorningAndAfternoon(parent);
	if(!t)
		return false;

	t=computeN1N2N3(parent);
	if(!t)
		return false;

	////////////////
	//2011-09-25
	t=computeActivitiesOccupyMaxTimeSlotsFromSelection(parent);
	if(!t)
		return false;
	
	////////////////
	//2019-11-16
	t=computeActivitiesOccupyMinTimeSlotsFromSelection(parent);
	if(!t)
		return false;
	
	//2011-09-30
	t=computeActivitiesMaxSimultaneousInSelectedTimeSlots(parent);
	if(!t)
		return false;
	////////////////

	//2019-11-16
	t=computeActivitiesMinSimultaneousInSelectedTimeSlots(parent);
	if(!t)
		return false;
	////////////////

	//2020-05-02
	t=computeActivitiesMaxTotalFromSetInSelectedTimeSlots(parent);
	if(!t)
		return false;
	////////////////

	//for terms
	////////////////
	//2020-01-14
	t=computeActivitiesMaxInATerm(parent);
	if(!t)
		return false;

	//2022-05-19
	t=computeActivitiesMinInATerm(parent);
	if(!t)
		return false;

	////////////////
	//2020-01-14
	t=computeActivitiesOccupyMaxTerms(parent);
	if(!t)
		return false;

	//2019-06-08
	t=computeStudentsMinGapsBetweenOrderedPairOfActivityTags(parent);
	if(!t)
		return false;
	////////////////

	//2019-06-08
	t=computeTeachersMinGapsBetweenOrderedPairOfActivityTags(parent);
	if(!t)
		return false;

	//2021-12-15
	t=computeStudentsMinGapsBetweenActivityTag(parent);
	if(!t)
		return false;
	////////////////

	//2021-12-15
	t=computeTeachersMinGapsBetweenActivityTag(parent);
	if(!t)
		return false;
	////////////////

	//2012-04-29
	t=computeActivitiesOccupyMaxDifferentRooms(parent);
	if(!t)
		return false;
	
	////////////////
	
	//2013-09-14
	t=computeActivitiesSameRoomIfConsecutive(parent);
	if(!t)
		return false;
	
	////////////////
	
	/////////////rooms
	t=computeBasicSpace(parent);
	if(!t)
		return false;
	t=computeNotAllowedRoomTimePercentages();
	if(!t)
		return false;
		
	t=computeNotAllowedTeacherRoomTimePercentages();
	if(!t)
		return false;
	//QHash<QPair<QPair<int, int>, QPair<int, int>>, double > notAllowedTeacherRoomTimePercentages;
	
	//compute below !before computeFixedActivities!
	t=computeActivitiesRoomsPreferences(parent);
	if(!t)
		return false;
	//////////////////
	
	/////////buildings
	t=computeMaxBuildingChangesPerWeekForStudents(parent);
	if(!t)
		return false;
	t=computeMaxBuildingChangesPerDayForStudents(parent);
	if(!t)
		return false;
	t=computeMinGapsBetweenBuildingChangesForStudents(parent);
	if(!t)
		return false;

	t=computeMaxBuildingChangesPerWeekForTeachers(parent);
	if(!t)
		return false;
	t=computeMaxBuildingChangesPerDayForTeachers(parent);
	if(!t)
		return false;
	t=computeMinGapsBetweenBuildingChangesForTeachers(parent);
	if(!t)
		return false;
	//////////////////
	
	/////////rooms
	t=computeMaxRoomChangesPerWeekForStudents(parent);
	if(!t)
		return false;
	t=computeMaxRoomChangesPerDayForStudents(parent);
	if(!t)
		return false;
	t=computeMinGapsBetweenRoomChangesForStudents(parent);
	if(!t)
		return false;

	t=computeMaxRoomChangesPerWeekForTeachers(parent);
	if(!t)
		return false;
	t=computeMaxRoomChangesPerDayForTeachers(parent);
	if(!t)
		return false;
	t=computeMinGapsBetweenRoomChangesForTeachers(parent);
	if(!t)
		return false;
	//////////////////

	t=computeMaxRoomChangesPerRealDayForTeachers(parent);
	if(!t)
		return false;
	t=computeMaxRoomChangesPerRealDayForStudents(parent);
	if(!t)
		return false;
	//////////////////

	t=computeMaxBuildingChangesPerRealDayForTeachers(parent);
	if(!t)
		return false;
	t=computeMaxBuildingChangesPerRealDayForStudents(parent);
	if(!t)
		return false;
	//////////////////

	t=homeRoomsAreOk(parent);
	if(!t)
		return false;
	
	computeMustComputeTimetableSubgroups();
	computeMustComputeTimetableTeachers();
	
	computeSubgroupsTeachersForActivitiesOfTheDay();
	
	//!after computeActivitiesRoomsPreferences!
	t=computeFixedActivities(parent);
	if(!t)
		return false;
	
	//must have here repr computed correctly
	sortActivities(parent, reprSameStartingTime, reprSameActivitiesSet, initialOrderStream);
	
	if(SHOW_WARNING_FOR_MAX_HOURS_DAILY_WITH_UNDER_100_WEIGHT && (thereAreTeachersWithMaxHoursDailyOrPerRealDayWithUnder100Weight || thereAreSubgroupsWithMaxHoursDailyOrPerRealDayWithUnder100Weight)){
		QString s=GeneratePreTranslate::tr("Your file contains constraints of type teacher(s)/students (set) max hours daily or per real day (where applicable)"
		 " with a weight less than 100%. This is not recommended, because in this case the algorithm is not implemented perfectly"
		 " (even if it might work well in practice). You are advised to use constraints of this type only with weight 100%.");
		s+="\n\n";
		s+=GeneratePreTranslate::tr("Are you sure you want to continue?");
		s+="\n\n";
		s+=GeneratePreTranslate::tr("Note: You can deactivate this warning from the Settings menu.");

		int t=GeneratePreReconcilableMessage::largeConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
		 GeneratePreTranslate::tr("Continue"), GeneratePreTranslate::tr("Cancel"), QString(), 0, 1);

		if(t!=0)
			return false;
	}

	if(SHOW_WARNING_FOR_NOT_PERFECT_CONSTRAINTS){
		if(haveStudentsMaxGapsPerDay || haveTeachersActivityTagMaxHoursDailyRealDays || haveStudentsActivityTagMaxHoursDailyRealDays
		 || haveStudentsMaxGapsPerRealDay || haveTeachersMaxGapsPerRealDay /* || haveStudentsMaxGapsPerWeekForRealDays || haveTeachersMaxGapsPerWeekForRealDays */
		 || haveTeachersActivityTagMinHoursDaily || haveStudentsActivityTagMinHoursDaily
		 || haveTeachersActivityTagMaxHoursDaily || haveStudentsActivityTagMaxHoursDaily){
			QString s=GeneratePreTranslate::tr("Your data contains constraints students max gaps per day"
			 " and/or students max gaps per real day and/or teachers max gaps per real day"
			 " and/or students max gaps per week for real days and/or teachers max gaps per week for real days"
			 " and/or activity tag max/min hours daily/per real day (where applicable).");
			s+="\n\n";
			s+=GeneratePreTranslate::tr("These constraints are good, but they are not perfectly optimized for speed. You may obtain a long generation time or even impossible timetables.");
			s+=" ";
			s+=GeneratePreTranslate::tr("It is recommended to use such constraints with caution.");
			s+="\n\n";
			s+=GeneratePreTranslate::tr("Are you sure you want to continue?");

#ifdef FET_COMMAND_LINE
			int b=GeneratePreReconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s, GeneratePreTranslate::tr("Yes"), GeneratePreTranslate::tr("No"), QString(), 0, 1);
			if(b!=0)
				return false;
#else
			QMessageBox::StandardButton b=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"), s, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			if(b!=QMessageBox::Yes)
				return false;
#endif
		}
	}

	if(gt.rules.mode!=MORNINGS_AFTERNOONS && SHOW_WARNING_FOR_STUDENTS_MIN_HOURS_DAILY_WITH_ALLOW_EMPTY_DAYS){
		if(haveStudentsMinHoursDailyAllowEmptyDays || haveStudentsMinHoursDailyMorningsAfternoonsAllowEmptyDays){
			QString s=GeneratePreTranslate::tr("Your data contains constraints students min hours daily/per morning/per afternoon which allow empty days/mornings/afternoons.");
			s+="\n\n";
			s+=GeneratePreTranslate::tr("These constraints are nonstandard. They are recommended only if the students can have free days and a solution with free days for students exists."
			 " Otherwise the solution might be impossible for FET to find.");
			s+=" ";
			s+=GeneratePreTranslate::tr("It is recommended to use such constraints with caution.");
			s+="\n\n";
			s+=GeneratePreTranslate::tr("Are you sure you want to continue?");

#ifdef FET_COMMAND_LINE
			int b=GeneratePreReconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s, GeneratePreTranslate::tr("Yes"), GeneratePreTranslate::tr("No"), QString(), 0, 1);
			if(b!=0)
				return false;
#else
			QMessageBox::StandardButton b=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"), s, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			if(b!=QMessageBox::Yes)
				return false;
#endif
		}
	}

	if(SHOW_WARNING_FOR_GROUP_ACTIVITIES_IN_INITIAL_ORDER){
		if(gt.rules.groupActivitiesInInitialOrderList.count()>0){
			QString s=GeneratePreTranslate::tr("Your data contains the option to group activities in the initial order.");
			s+="\n\n";
			s+=GeneratePreTranslate::tr("This option is nonstandard. It is recommended only if you know what you are doing,"
				" otherwise the solution might be impossible for FET to find.");
			s+=" ";
			s+=GeneratePreTranslate::tr("Use with caution.");
			s+="\n\n";
			s+=GeneratePreTranslate::tr("Are you sure you want to continue?");

#ifdef FET_COMMAND_LINE
			int b=GeneratePreReconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s, GeneratePreTranslate::tr("Yes"), GeneratePreTranslate::tr("No"), QString(), 0, 1);
			if(b!=0)
				return false;
#else
			QMessageBox::StandardButton b=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"), s, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
			if(b!=QMessageBox::Yes)
				return false;
#endif
		}
	}

	if(SHOW_WARNING_FOR_ACTIVITIES_FIXED_SPACE_VIRTUAL_REAL_ROOMS_BUT_NOT_FIXED_TIME && !fixedVirtualSpaceNonZeroButNotTimeActivities.isEmpty()){
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
		QList<int> tl=QList<int>(fixedVirtualSpaceNonZeroButNotTimeActivities.constBegin(), fixedVirtualSpaceNonZeroButNotTimeActivities.constEnd());
#else
		QList<int> tl=fixedVirtualSpaceNonZeroButNotTimeActivities.toList();
#endif
		std::stable_sort(tl.begin(), tl.end());

		QStringList tlids;
		for(int i : qAsConst(tl))
			tlids.append(CustomFETString::number(gt.rules.internalActivitiesList[i].id));

		QString s=GeneratePreTranslate::tr("You have a number of %1 activities (listed below) which are not locked in time, but are locked in space"
		 " into a virtual room (having specified also a nonempty list of selected real rooms). This might lead to impossible, cycling timetables (FET might"
		 " not be able to find a timetable, even if one exists). It is recommended either to lock these activities also in time, or to"
		 " remove the specified real rooms and leave only the preferred virtual room. This problematic situation was discovered when"
		 " generating on a crafted file derived from a German example (the exact file name is %2). If after an initial successful generation"
		 " you only lock the activities in space (but not also in time), FET seems to cycle indefinitely when trying to generate again on the newly obtained file.")
		 .arg(tl.count())
		 .arg("fet-v.v.v/examples/Germany/secondary-school-1/test-virtual-rooms-1/German-test-virtual-rooms-1.fet");
		s+="\n\n";
		s+=GeneratePreTranslate::tr("Are you sure you want to continue?");
		s+="\n\n";
		s+=GeneratePreTranslate::tr("Note: If an activity is fixed in a virtual room with a constraint activity preferred room with a 100% weight,"
		 " it is considered locked in space only if the constraint also specifies a nonempty list of real rooms assigned for this activity"
		 " (chosen from the list of sets of real rooms of the virtual room).");
		s+="\n\n";
		s+=GeneratePreTranslate::tr("Note: You can deactivate this warning from the Settings menu.");
		s+="\n\n";
		s+=GeneratePreTranslate::tr("The ids of the activities are: %1.")
		 .arg(tlids.join(", "));

		int t=GeneratePreReconcilableMessage::largeConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
		 GeneratePreTranslate::tr("Continue"), GeneratePreTranslate::tr("Cancel"), QString(), 0, 1);

		if(t!=0)
			return false;
	}

	bool ok=true;

	return ok;
}

//must be after allowed times, after n hours per subgroup and after max days per week for subgroups
bool computeSubgroupsMaxHoursDaily(QWidget* parent)
{
	thereAreSubgroupsWithMaxHoursDailyOrPerRealDayWithUnder100Weight=false;

	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMaxHoursDailyMaxHours1[i]=-1;
		subgroupsMaxHoursDailyPercentages1[i]=-1;

		subgroupsMaxHoursDailyMaxHours2[i]=-1;
		subgroupsMaxHoursDailyPercentages2[i]=-1;
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_HOURS_DAILY){
			ConstraintStudentsMaxHoursDaily* smd=(ConstraintStudentsMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			if(smd->weightPercentage<100.0)
				thereAreSubgroupsWithMaxHoursDailyOrPerRealDayWithUnder100Weight=true;

			for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMaxHoursDailyMaxHours1[sb]==-1 ||
				 (subgroupsMaxHoursDailyMaxHours1[sb] >= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyPercentages1[sb] <= smd->weightPercentage)){
				 	subgroupsMaxHoursDailyMaxHours1[sb] = smd->maxHoursDaily;
					subgroupsMaxHoursDailyPercentages1[sb] = smd->weightPercentage;
					}
				else if(subgroupsMaxHoursDailyMaxHours1[sb] <= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyPercentages1[sb] >= smd->weightPercentage){
				 	//nothing
				}
				else{
					if(subgroupsMaxHoursDailyMaxHours2[sb]==-1 ||
					 (subgroupsMaxHoursDailyMaxHours2[sb] >= smd->maxHoursDaily &&
					 subgroupsMaxHoursDailyPercentages2[sb] <= smd->weightPercentage)){
					 	subgroupsMaxHoursDailyMaxHours2[sb] = smd->maxHoursDaily;
						subgroupsMaxHoursDailyPercentages2[sb] = smd->weightPercentage;
						}
					else if(subgroupsMaxHoursDailyMaxHours2[sb] <= smd->maxHoursDaily &&
					 subgroupsMaxHoursDailyPercentages2[sb] >= smd->weightPercentage){
					 	//nothing
					}
					else{
						 //cannot proceed
						ok=false;
		
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are too many constraints"
						 " of type max hours daily relating to it, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a subgroup, but their resultant must"
						 " be maximum 2 constraints of type max hours daily.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again.")
						 .arg(gt.rules.internalSubgroupsList[sb]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							return false;
					}
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
			ConstraintStudentsSetMaxHoursDaily* smd=(ConstraintStudentsSetMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			if(smd->weightPercentage<100.0)
				thereAreSubgroupsWithMaxHoursDailyOrPerRealDayWithUnder100Weight=true;

			for(int q=0; q<smd->iSubgroupsList.count(); q++){
				int sb=smd->iSubgroupsList.at(q);
			//for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMaxHoursDailyMaxHours1[sb]==-1 ||
				 (subgroupsMaxHoursDailyMaxHours1[sb] >= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyPercentages1[sb] <= smd->weightPercentage)){
				 	subgroupsMaxHoursDailyMaxHours1[sb] = smd->maxHoursDaily;
					subgroupsMaxHoursDailyPercentages1[sb] = smd->weightPercentage;
					}
				else if(subgroupsMaxHoursDailyMaxHours1[sb] <= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyPercentages1[sb] >= smd->weightPercentage){
				 	//nothing
				}
				else{
					if(subgroupsMaxHoursDailyMaxHours2[sb]==-1 ||
					 (subgroupsMaxHoursDailyMaxHours2[sb] >= smd->maxHoursDaily &&
					 subgroupsMaxHoursDailyPercentages2[sb] <= smd->weightPercentage)){
					 	subgroupsMaxHoursDailyMaxHours2[sb] = smd->maxHoursDaily;
						subgroupsMaxHoursDailyPercentages2[sb] = smd->weightPercentage;
						}
					else if(subgroupsMaxHoursDailyMaxHours2[sb] <= smd->maxHoursDaily &&
					 subgroupsMaxHoursDailyPercentages2[sb] >= smd->weightPercentage){
					 	//nothing
					}
					else{
						//cannot proceed
						ok=false;
	
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are too many constraints"
						 " of type max hours daily relating to it, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a subgroup, but their resultant must"
						 " be maximum 2 constraints of type max hours daily.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again.")
						 .arg(gt.rules.internalSubgroupsList[sb]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}
				}
			}
		}
	}
	
	Matrix1D<int> nAllowedSlotsPerDay;
	nAllowedSlotsPerDay.resize(gt.rules.nDaysPerWeek);
	
	Matrix1D<int> dayAvailable;
	dayAvailable.resize(gt.rules.nDaysPerWeek);
	
	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		if(subgroupsMaxHoursDailyPercentages1[sb]==100){
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[sb][d][h])
						nAllowedSlotsPerDay[d]++;
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],subgroupsMaxHoursDailyMaxHours1[sb]);
			}
			
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				dayAvailable[d]=1;
			if(subgroupsMaxDaysPerWeekMaxDays[sb]>=0){
				//max days per week has 100% weight
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					dayAvailable[d]=0;
				assert(subgroupsMaxDaysPerWeekMaxDays[sb]<=gt.rules.nDaysPerWeek);
				for(int k=0; k<subgroupsMaxDaysPerWeekMaxDays[sb]; k++){
					int maxPos=-1, maxVal=-1;
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						if(dayAvailable[d]==0)
							if(maxVal<nAllowedSlotsPerDay[d]){
								maxVal=nAllowedSlotsPerDay[d];
								maxPos=d;
							}
					assert(maxPos>=0);
					assert(dayAvailable[maxPos]==0);
					dayAvailable[maxPos]=1;
				}
			}
			
			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				if(dayAvailable[d]==1)
					total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerSubgroup[sb]){
				ok=false;
				
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there is a constraint of type"
				 " max %2 hours daily with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, students (set) max days per week, students set not available and/or breaks."
				 " The number of total hours for this subgroup is"
				 " %3 and the number of available slots is, considering max hours daily and all other constraints, %4.")
				 .arg(gt.rules.internalSubgroupsList[sb]->name)
				 .arg(subgroupsMaxHoursDailyMaxHours1[sb])
				 .arg(nHoursPerSubgroup[sb])
				 .arg(total);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}

	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		if(subgroupsMaxHoursDailyPercentages2[sb]==100){
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[sb][d][h])
						nAllowedSlotsPerDay[d]++;
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],subgroupsMaxHoursDailyMaxHours2[sb]);
			}
			
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				dayAvailable[d]=1;
			if(subgroupsMaxDaysPerWeekMaxDays[sb]>=0){
				//max days per week has 100% weight
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					dayAvailable[d]=0;
				assert(subgroupsMaxDaysPerWeekMaxDays[sb]<=gt.rules.nDaysPerWeek);
				for(int k=0; k<subgroupsMaxDaysPerWeekMaxDays[sb]; k++){
					int maxPos=-1, maxVal=-1;
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						if(dayAvailable[d]==0)
							if(maxVal<nAllowedSlotsPerDay[d]){
								maxVal=nAllowedSlotsPerDay[d];
								maxPos=d;
							}
					assert(maxPos>=0);
					assert(dayAvailable[maxPos]==0);
					dayAvailable[maxPos]=1;
				}
			}
			
			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				if(dayAvailable[d]==1)
					total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerSubgroup[sb]){
				ok=false;
				
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there is a constraint of type"
				 " max %2 hours daily with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, students (set) max days per week, students set not available and/or breaks."
				 " The number of total hours for this subgroup is"
				 " %3 and the number of available slots is, considering max hours daily and all other constraints, %4.")
				 .arg(gt.rules.internalSubgroupsList[sb]->name)
				 .arg(subgroupsMaxHoursDailyMaxHours2[sb])
				 .arg(nHoursPerSubgroup[sb])
				 .arg(total);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}

	return ok;
}

//must be after allowed times and after n hours per subgroup
bool computeSubgroupsMaxHoursDailyRealDays(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMaxHoursDailyRealDaysMaxHours1[i]=-1;
		subgroupsMaxHoursDailyRealDaysPercentages1[i]=-1;

		subgroupsMaxHoursDailyRealDaysMaxHours2[i]=-1;
		subgroupsMaxHoursDailyRealDaysPercentages2[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_REAL_DAYS){
			ConstraintStudentsMaxHoursDailyRealDays* smd=(ConstraintStudentsMaxHoursDailyRealDays*)gt.rules.internalTimeConstraintsList[i];

			if(smd->weightPercentage<100.0)
				thereAreSubgroupsWithMaxHoursDailyOrPerRealDayWithUnder100Weight=true;

			for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMaxHoursDailyRealDaysMaxHours1[sb]==-1 ||
				 (subgroupsMaxHoursDailyRealDaysMaxHours1[sb] >= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyRealDaysPercentages1[sb] <= smd->weightPercentage)){
					subgroupsMaxHoursDailyRealDaysMaxHours1[sb] = smd->maxHoursDaily;
					subgroupsMaxHoursDailyRealDaysPercentages1[sb] = smd->weightPercentage;
					}
				else if(subgroupsMaxHoursDailyRealDaysMaxHours1[sb] <= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyRealDaysPercentages1[sb] >= smd->weightPercentage){
					//nothing
				}
				else{
					if(subgroupsMaxHoursDailyRealDaysMaxHours2[sb]==-1 ||
					 (subgroupsMaxHoursDailyRealDaysMaxHours2[sb] >= smd->maxHoursDaily &&
					 subgroupsMaxHoursDailyRealDaysPercentages2[sb] <= smd->weightPercentage)){
						subgroupsMaxHoursDailyRealDaysMaxHours2[sb] = smd->maxHoursDaily;
						subgroupsMaxHoursDailyRealDaysPercentages2[sb] = smd->weightPercentage;
						}
					else if(subgroupsMaxHoursDailyRealDaysMaxHours2[sb] <= smd->maxHoursDaily &&
					 subgroupsMaxHoursDailyRealDaysPercentages2[sb] >= smd->weightPercentage){
						//nothing
					}
					else{
						 //cannot proceed
						ok=false;

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are too many constraints"
						 " of type max hours daily for real days relating to it, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a subgroup, but their resultant must"
						 " be maximum 2 constraints of type max hours daily for real days.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again.")
						 .arg(gt.rules.internalSubgroupsList[sb]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_REAL_DAYS){
			ConstraintStudentsSetMaxHoursDailyRealDays* smd=(ConstraintStudentsSetMaxHoursDailyRealDays*)gt.rules.internalTimeConstraintsList[i];

			if(smd->weightPercentage<100.0)
				thereAreSubgroupsWithMaxHoursDailyOrPerRealDayWithUnder100Weight=true;

			for(int q=0; q<smd->iSubgroupsList.count(); q++){
				int sb=smd->iSubgroupsList.at(q);
			//for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMaxHoursDailyRealDaysMaxHours1[sb]==-1 ||
				 (subgroupsMaxHoursDailyRealDaysMaxHours1[sb] >= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyRealDaysPercentages1[sb] <= smd->weightPercentage)){
					subgroupsMaxHoursDailyRealDaysMaxHours1[sb] = smd->maxHoursDaily;
					subgroupsMaxHoursDailyRealDaysPercentages1[sb] = smd->weightPercentage;
					}
				else if(subgroupsMaxHoursDailyRealDaysMaxHours1[sb] <= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyRealDaysPercentages1[sb] >= smd->weightPercentage){
					//nothing
				}
				else{
					if(subgroupsMaxHoursDailyRealDaysMaxHours2[sb]==-1 ||
					 (subgroupsMaxHoursDailyRealDaysMaxHours2[sb] >= smd->maxHoursDaily &&
					 subgroupsMaxHoursDailyRealDaysPercentages2[sb] <= smd->weightPercentage)){
						subgroupsMaxHoursDailyRealDaysMaxHours2[sb] = smd->maxHoursDaily;
						subgroupsMaxHoursDailyRealDaysPercentages2[sb] = smd->weightPercentage;
						}
					else if(subgroupsMaxHoursDailyRealDaysMaxHours2[sb] <= smd->maxHoursDaily &&
					 subgroupsMaxHoursDailyRealDaysPercentages2[sb] >= smd->weightPercentage){
						//nothing
					}
					else{
						//cannot proceed
						ok=false;

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are too many constraints"
						 " of type max hours daily for real days relating to it, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a subgroup, but their resultant must"
						 " be maximum 2 constraints of type max hours daily for real days.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again.")
						 .arg(gt.rules.internalSubgroupsList[sb]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}
				}
			}
		}
	}

	if(gt.rules.mode==MORNINGS_AFTERNOONS)
		assert(gt.rules.nDaysPerWeek%2==0); //this is taken care of previously - the generation cannot begin otherwise

	Matrix1D<int> nAllowedSlotsPerDay;
	nAllowedSlotsPerDay.resize(gt.rules.nDaysPerWeek/2);
	Matrix1D<int> dayAvailable;
	dayAvailable.resize(gt.rules.nDaysPerWeek/2);
	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		if(subgroupsMaxHoursDailyRealDaysPercentages1[sb]==100){
			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++){
					if(!breakDayHour[2*d][h] && !subgroupNotAvailableDayHour[sb][2*d][h])
						nAllowedSlotsPerDay[d]++;
					if(!breakDayHour[2*d+1][h] && !subgroupNotAvailableDayHour[sb][2*d+1][h])
						nAllowedSlotsPerDay[d]++;
				}
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],subgroupsMaxHoursDailyRealDaysMaxHours1[sb]);
			}

			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
				dayAvailable[d]=1;
			if(subgroupsMaxRealDaysPerWeekMaxDays[sb]>=0){
				//max days per week has 100% weight
				for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
					dayAvailable[d]=0;
				assert(subgroupsMaxRealDaysPerWeekMaxDays[sb]<=gt.rules.nDaysPerWeek/2);
				for(int k=0; k<subgroupsMaxRealDaysPerWeekMaxDays[sb]; k++){
					int maxPos=-1, maxVal=-1;
					for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
						if(dayAvailable[d]==0)
							if(maxVal<nAllowedSlotsPerDay[d]){
								maxVal=nAllowedSlotsPerDay[d];
								maxPos=d;
							}
					assert(maxPos>=0);
					assert(dayAvailable[maxPos]==0);
					dayAvailable[maxPos]=1;
				}
			}

			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
				if(dayAvailable[d]==1)
					total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerSubgroup[sb]){
				ok=false;

				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there is a constraint of type"
				 " max %2 hours daily for real days with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, students (set) max real days per week, students set not available and/or breaks."
				 " The number of total hours for this subgroup is"
				 " %3 and the number of available slots is, considering max hours daily for real days and all other constraints, %4.")
				 .arg(gt.rules.internalSubgroupsList[sb]->name)
				 .arg(subgroupsMaxHoursDailyRealDaysMaxHours1[sb])
				 .arg(nHoursPerSubgroup[sb])
				 .arg(total);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
	}

	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		if(subgroupsMaxHoursDailyRealDaysPercentages2[sb]==100){
			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++){
					if(!breakDayHour[2*d][h] && !subgroupNotAvailableDayHour[sb][2*d][h])
						nAllowedSlotsPerDay[d]++;
					if(!breakDayHour[2*d+1][h] && !subgroupNotAvailableDayHour[sb][2*d+1][h])
						nAllowedSlotsPerDay[d]++;
				}
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],subgroupsMaxHoursDailyRealDaysMaxHours1[sb]);
			}

			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
				dayAvailable[d]=1;
			if(subgroupsMaxRealDaysPerWeekMaxDays[sb]>=0){
				//max days per week has 100% weight
				for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
					dayAvailable[d]=0;
				assert(subgroupsMaxRealDaysPerWeekMaxDays[sb]<=gt.rules.nDaysPerWeek/2);
				for(int k=0; k<subgroupsMaxRealDaysPerWeekMaxDays[sb]; k++){
					int maxPos=-1, maxVal=-1;
					for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
						if(dayAvailable[d]==0)
							if(maxVal<nAllowedSlotsPerDay[d]){
								maxVal=nAllowedSlotsPerDay[d];
								maxPos=d;
							}
					assert(maxPos>=0);
					assert(dayAvailable[maxPos]==0);
					dayAvailable[maxPos]=1;
				}
			}

			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
				if(dayAvailable[d]==1)
					total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerSubgroup[sb]){
				ok=false;

				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there is a constraint of type"
				 " max %2 hours daily for real days with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, students (set) max real days per week, students set not available and/or breaks."
				 " The number of total hours for this subgroup is"
				 " %3 and the number of available slots is, considering max hours daily for real days and all other constraints, %4.")
				 .arg(gt.rules.internalSubgroupsList[sb]->name)
				 .arg(subgroupsMaxHoursDailyRealDaysMaxHours2[sb])
				 .arg(nHoursPerSubgroup[sb])
				 .arg(total);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
	}

	return ok;
}

//must be after allowed times, after n hours per subgroup and after max days per week for subgroups
bool computeSubgroupsMaxSpanPerDay(QWidget* parent)
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMaxSpanPerDayMaxSpan[i]=-1;
		subgroupsMaxSpanPerDayPercentages[i]=-1;
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_DAY){
			ConstraintStudentsSetMaxSpanPerDay* ssmsd=(ConstraintStudentsSetMaxSpanPerDay*)gt.rules.internalTimeConstraintsList[i];

			if(ssmsd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set max span per day for students %1"
				 " with weight (percentage) below 100. Please make weight 100% and try again").arg(ssmsd->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_SPAN_PER_DAY){
			ConstraintStudentsMaxSpanPerDay* smsd=(ConstraintStudentsMaxSpanPerDay*)gt.rules.internalTimeConstraintsList[i];

			if(smsd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students max span per day"
				 " with weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_DAY){
			ConstraintStudentsSetMaxSpanPerDay* ssmsd=(ConstraintStudentsSetMaxSpanPerDay*)gt.rules.internalTimeConstraintsList[i];
			
			for(int sbg : qAsConst(ssmsd->iSubgroupsList))
				if(subgroupsMaxSpanPerDayPercentages[sbg]==-1
				 || (subgroupsMaxSpanPerDayPercentages[sbg]>=0 && subgroupsMaxSpanPerDayMaxSpan[sbg]>ssmsd->maxSpanPerDay)){
					subgroupsMaxSpanPerDayPercentages[sbg]=100.0;
					subgroupsMaxSpanPerDayMaxSpan[sbg]=ssmsd->maxSpanPerDay;
				}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_SPAN_PER_DAY){
			ConstraintStudentsMaxSpanPerDay* smsd=(ConstraintStudentsMaxSpanPerDay*)gt.rules.internalTimeConstraintsList[i];
			
			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				if(subgroupsMaxSpanPerDayPercentages[sbg]==-1
				 || (subgroupsMaxSpanPerDayPercentages[sbg]>=0 && subgroupsMaxSpanPerDayMaxSpan[sbg]>smsd->maxSpanPerDay)){
					subgroupsMaxSpanPerDayPercentages[sbg]=100.0;
					subgroupsMaxSpanPerDayMaxSpan[sbg]=smsd->maxSpanPerDay;
				}
			}
		}
	}
	
	//This is similar to subgroups max hours daily checking. It is not a very useful test, but does not hurt.
	
	Matrix1D<int> nAllowedSlotsPerDay;
	nAllowedSlotsPerDay.resize(gt.rules.nDaysPerWeek);
	
	Matrix1D<int> dayAvailable;
	dayAvailable.resize(gt.rules.nDaysPerWeek);
	
	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		if(subgroupsMaxSpanPerDayPercentages[sb]==100){
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[sb][d][h])
						nAllowedSlotsPerDay[d]++;
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],subgroupsMaxSpanPerDayMaxSpan[sb]);
			}
			
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				dayAvailable[d]=1;
			if(subgroupsMaxDaysPerWeekMaxDays[sb]>=0){
				//max days per week has 100% weight
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					dayAvailable[d]=0;
				assert(subgroupsMaxDaysPerWeekMaxDays[sb]<=gt.rules.nDaysPerWeek);
				for(int k=0; k<subgroupsMaxDaysPerWeekMaxDays[sb]; k++){
					int maxPos=-1, maxVal=-1;
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						if(dayAvailable[d]==0)
							if(maxVal<nAllowedSlotsPerDay[d]){
								maxVal=nAllowedSlotsPerDay[d];
								maxPos=d;
							}
					assert(maxPos>=0);
					assert(dayAvailable[maxPos]==0);
					dayAvailable[maxPos]=1;
				}
			}
			
			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				if(dayAvailable[d]==1)
					total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerSubgroup[sb]){
				ok=false;
				
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there is a constraint of type"
				 " max %2 span per day with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, students (set) max days per week, students set not available and/or breaks."
				 " The number of total hours for this subgroup is"
				 " %3 and the number of available slots is, considering max span per day and all other constraints, %4.")
				 .arg(gt.rules.internalSubgroupsList[sb]->name)
				 .arg(subgroupsMaxSpanPerDayMaxSpan[sb])
				 .arg(nHoursPerSubgroup[sb])
				 .arg(total);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}

	return ok;
}

//must be after allowed times, after n hours per subgroup and after max days per week for subgroups
bool computeSubgroupsMaxSpanPerRealDay(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMaxSpanPerRealDayMaxSpan[i]=-1;
		subgroupsMaxSpanPerRealDayPercentages[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_REAL_DAY){
			ConstraintStudentsSetMaxSpanPerRealDay* ssmsd=(ConstraintStudentsSetMaxSpanPerRealDay*)gt.rules.internalTimeConstraintsList[i];

			if(ssmsd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set max span per real day for students %1"
				 " with weight (percentage) below 100. Please make weight 100% and try again").arg(ssmsd->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_SPAN_PER_REAL_DAY){
			ConstraintStudentsMaxSpanPerRealDay* smsd=(ConstraintStudentsMaxSpanPerRealDay*)gt.rules.internalTimeConstraintsList[i];

			if(smsd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students max span per real day"
				 " with weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_REAL_DAY){
			ConstraintStudentsSetMaxSpanPerRealDay* ssmsd=(ConstraintStudentsSetMaxSpanPerRealDay*)gt.rules.internalTimeConstraintsList[i];

			for(int sbg : qAsConst(ssmsd->iSubgroupsList))
				if(subgroupsMaxSpanPerRealDayPercentages[sbg]==-1
				 || (subgroupsMaxSpanPerRealDayPercentages[sbg]>=0 && subgroupsMaxSpanPerRealDayMaxSpan[sbg]>ssmsd->maxSpanPerDay)){
					subgroupsMaxSpanPerRealDayPercentages[sbg]=100.0;
					subgroupsMaxSpanPerRealDayMaxSpan[sbg]=ssmsd->maxSpanPerDay;
				}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_SPAN_PER_REAL_DAY){
			ConstraintStudentsMaxSpanPerRealDay* smsd=(ConstraintStudentsMaxSpanPerRealDay*)gt.rules.internalTimeConstraintsList[i];

			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				if(subgroupsMaxSpanPerRealDayPercentages[sbg]==-1
				 || (subgroupsMaxSpanPerRealDayPercentages[sbg]>=0 && subgroupsMaxSpanPerRealDayMaxSpan[sbg]>smsd->maxSpanPerDay)){
					subgroupsMaxSpanPerRealDayPercentages[sbg]=100.0;
					subgroupsMaxSpanPerRealDayMaxSpan[sbg]=smsd->maxSpanPerDay;
				}
			}
		}
	}

	if(gt.rules.mode==MORNINGS_AFTERNOONS)
		assert(gt.rules.nDaysPerWeek%2==0); //this is taken care of previously - the generation cannot begin otherwise

	//This is similar to subgroups max hours daily checking. It is not a very useful test, but does not hurt.
	Matrix1D<int> nAllowedSlotsPerDay;
	nAllowedSlotsPerDay.resize(gt.rules.nDaysPerWeek/2);
	Matrix1D<int> dayAvailable;
	dayAvailable.resize(gt.rules.nDaysPerWeek/2);
	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		if(subgroupsMaxSpanPerRealDayPercentages[sb]==100){
			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++){
					if(!breakDayHour[2*d][h] && !subgroupNotAvailableDayHour[sb][2*d][h])
						nAllowedSlotsPerDay[d]++;
					if(!breakDayHour[2*d+1][h] && !subgroupNotAvailableDayHour[sb][2*d+1][h])
						nAllowedSlotsPerDay[d]++;
				}
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],subgroupsMaxSpanPerRealDayMaxSpan[sb]);
			}

			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
				dayAvailable[d]=1;
			if(subgroupsMaxRealDaysPerWeekMaxDays[sb]>=0){
				//max days per week has 100% weight
				for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
					dayAvailable[d]=0;
				assert(subgroupsMaxRealDaysPerWeekMaxDays[sb]<=gt.rules.nDaysPerWeek/2);
				for(int k=0; k<subgroupsMaxRealDaysPerWeekMaxDays[sb]; k++){
					int maxPos=-1, maxVal=-1;
					for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
						if(dayAvailable[d]==0)
							if(maxVal<nAllowedSlotsPerDay[d]){
								maxVal=nAllowedSlotsPerDay[d];
								maxPos=d;
							}
					assert(maxPos>=0);
					assert(dayAvailable[maxPos]==0);
					dayAvailable[maxPos]=1;
				}
			}

			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
				if(dayAvailable[d]==1)
					total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerSubgroup[sb]){
				ok=false;

				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there is a constraint of type"
				 " max %2 span per real day with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, students (set) max real days per week, students set not available and/or breaks."
				 " The number of total hours for this subgroup is"
				 " %3 and the number of available slots is, considering max span per real day and all other constraints, %4.")
				 .arg(gt.rules.internalSubgroupsList[sb]->name)
				 .arg(subgroupsMaxSpanPerRealDayMaxSpan[sb])
				 .arg(nHoursPerSubgroup[sb])
				 .arg(total);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
	}

	return ok;
}

bool computeStudentsMaxHoursContinuously(QWidget* parent)
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMaxHoursContinuouslyMaxHours1[i]=-1;
		subgroupsMaxHoursContinuouslyPercentages1[i]=-1;

		subgroupsMaxHoursContinuouslyMaxHours2[i]=-1;
		subgroupsMaxHoursContinuouslyPercentages2[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY){
			ConstraintStudentsMaxHoursContinuously* smd=(ConstraintStudentsMaxHoursContinuously*)gt.rules.internalTimeConstraintsList[i];

			for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMaxHoursContinuouslyMaxHours1[sb]==-1 ||
				 (subgroupsMaxHoursContinuouslyMaxHours1[sb] >= smd->maxHoursContinuously &&
				 subgroupsMaxHoursContinuouslyPercentages1[sb] <= smd->weightPercentage)){
				 	subgroupsMaxHoursContinuouslyMaxHours1[sb] = smd->maxHoursContinuously;
					subgroupsMaxHoursContinuouslyPercentages1[sb] = smd->weightPercentage;
					}
				else if(subgroupsMaxHoursContinuouslyMaxHours1[sb] <= smd->maxHoursContinuously &&
				 subgroupsMaxHoursContinuouslyPercentages1[sb] >= smd->weightPercentage){
				 	//nothing
				}
				else{
					if(subgroupsMaxHoursContinuouslyMaxHours2[sb]==-1 ||
					 (subgroupsMaxHoursContinuouslyMaxHours2[sb] >= smd->maxHoursContinuously &&
					 subgroupsMaxHoursContinuouslyPercentages2[sb] <= smd->weightPercentage)){
					 	subgroupsMaxHoursContinuouslyMaxHours2[sb] = smd->maxHoursContinuously;
						subgroupsMaxHoursContinuouslyPercentages2[sb] = smd->weightPercentage;
						}
					else if(subgroupsMaxHoursContinuouslyMaxHours2[sb] <= smd->maxHoursContinuously &&
					 subgroupsMaxHoursContinuouslyPercentages2[sb] >= smd->weightPercentage){
					 	//nothing
					}
					else{
						 //cannot proceed
						ok=false;
		
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are too many constraints"
						 " of type max hours continuously relating to it, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a subgroup, but their resultant must"
						 " be maximum 2 constraints of type max hours continuously.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again.")
						 .arg(gt.rules.internalSubgroupsList[sb]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							return false;
					}
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY){
			ConstraintStudentsSetMaxHoursContinuously* smd=(ConstraintStudentsSetMaxHoursContinuously*)gt.rules.internalTimeConstraintsList[i];

			for(int q=0; q<smd->iSubgroupsList.count(); q++){
				int sb=smd->iSubgroupsList.at(q);
			//for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMaxHoursContinuouslyMaxHours1[sb]==-1 ||
				 (subgroupsMaxHoursContinuouslyMaxHours1[sb] >= smd->maxHoursContinuously &&
				 subgroupsMaxHoursContinuouslyPercentages1[sb] <= smd->weightPercentage)){
				 	subgroupsMaxHoursContinuouslyMaxHours1[sb] = smd->maxHoursContinuously;
					subgroupsMaxHoursContinuouslyPercentages1[sb] = smd->weightPercentage;
					}
				else if(subgroupsMaxHoursContinuouslyMaxHours1[sb] <= smd->maxHoursContinuously &&
				 subgroupsMaxHoursContinuouslyPercentages1[sb] >= smd->weightPercentage){
				 	//nothing
				}
				else{
					if(subgroupsMaxHoursContinuouslyMaxHours2[sb]==-1 ||
					 (subgroupsMaxHoursContinuouslyMaxHours2[sb] >= smd->maxHoursContinuously &&
					 subgroupsMaxHoursContinuouslyPercentages2[sb] <= smd->weightPercentage)){
					 	subgroupsMaxHoursContinuouslyMaxHours2[sb] = smd->maxHoursContinuously;
						subgroupsMaxHoursContinuouslyPercentages2[sb] = smd->weightPercentage;
						}
					else if(subgroupsMaxHoursContinuouslyMaxHours2[sb] <= smd->maxHoursContinuously &&
					 subgroupsMaxHoursContinuouslyPercentages2[sb] >= smd->weightPercentage){
					 	//nothing
					}
					else{
						//cannot proceed
						ok=false;
	
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are too many constraints"
						 " of type max hours continuously relating to it, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a subgroup, but their resultant must"
						 " be maximum 2 constraints of type max hours continuously.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again.")
						 .arg(gt.rules.internalSubgroupsList[sb]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}
				}
			}
		}
	}
	
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		for(int sbg : qAsConst(gt.rules.internalActivitiesList[ai].iSubgroupsList)){
			if(subgroupsMaxHoursContinuouslyPercentages1[sbg]>=0 && gt.rules.internalActivitiesList[ai].duration > subgroupsMaxHoursContinuouslyMaxHours1[sbg]){
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there is a constraint of type"
				 " max %2 hours continuously which cannot be respected because of activity with id %3 (which has duration %4).")
				 .arg(gt.rules.internalSubgroupsList[sbg]->name)
				 .arg(subgroupsMaxHoursContinuouslyMaxHours1[sbg])
				 .arg(gt.rules.internalActivitiesList[ai].id)
				 .arg(gt.rules.internalActivitiesList[ai].duration);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			if(subgroupsMaxHoursContinuouslyPercentages2[sbg]>=0 && gt.rules.internalActivitiesList[ai].duration > subgroupsMaxHoursContinuouslyMaxHours2[sbg]){
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there is a constraint of type"
				 " max %2 hours continuously which cannot be respected because of activity with id %3 (which has duration %4).")
				 .arg(gt.rules.internalSubgroupsList[sbg]->name)
				 .arg(subgroupsMaxHoursContinuouslyMaxHours2[sbg])
				 .arg(gt.rules.internalActivitiesList[ai].id)
				 .arg(gt.rules.internalActivitiesList[ai].duration);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}
	
	return ok;
}

bool computeStudentsActivityTagMaxHoursDaily(QWidget* parent)
{
	haveStudentsActivityTagMaxHoursDaily=false;
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsActivityTagMaxHoursDailyMaxHours[i].clear();
		subgroupsActivityTagMaxHoursDailyPercentage[i].clear();
		subgroupsActivityTagMaxHoursDailyActivityTag[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY){
			haveStudentsActivityTagMaxHoursDaily=true;

			ConstraintStudentsActivityTagMaxHoursDaily* samd=(ConstraintStudentsActivityTagMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			for(int sb : qAsConst(samd->canonicalSubgroupsList)){
				int pos1=-1, pos2=-1;
				
				for(int j=0; j<subgroupsActivityTagMaxHoursDailyMaxHours[sb].count(); j++){
					if(subgroupsActivityTagMaxHoursDailyActivityTag[sb].at(j)==samd->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}
				
				if(pos1==-1){
					subgroupsActivityTagMaxHoursDailyActivityTag[sb].append(samd->activityTagIndex);
					subgroupsActivityTagMaxHoursDailyMaxHours[sb].append(samd->maxHoursDaily);
					subgroupsActivityTagMaxHoursDailyPercentage[sb].append(samd->weightPercentage);
				}
				else{
					if(subgroupsActivityTagMaxHoursDailyMaxHours[sb].at(pos1) <= samd->maxHoursDaily
					 && subgroupsActivityTagMaxHoursDailyPercentage[sb].at(pos1) >= samd->weightPercentage){
					 	//do nothing
					}
					else if(subgroupsActivityTagMaxHoursDailyMaxHours[sb].at(pos1) >= samd->maxHoursDaily
					 && subgroupsActivityTagMaxHoursDailyPercentage[sb].at(pos1) <= samd->weightPercentage){
					
						subgroupsActivityTagMaxHoursDailyActivityTag[sb][pos1]=samd->activityTagIndex;
						subgroupsActivityTagMaxHoursDailyMaxHours[sb][pos1]=samd->maxHoursDaily;
						subgroupsActivityTagMaxHoursDailyPercentage[sb][pos1]=samd->weightPercentage;
					}
					else{
						if(pos2==-1){
							subgroupsActivityTagMaxHoursDailyActivityTag[sb].append(samd->activityTagIndex);
							subgroupsActivityTagMaxHoursDailyMaxHours[sb].append(samd->maxHoursDaily);
							subgroupsActivityTagMaxHoursDailyPercentage[sb].append(samd->weightPercentage);
						}
						else{

							if(subgroupsActivityTagMaxHoursDailyMaxHours[sb].at(pos2) <= samd->maxHoursDaily
							 && subgroupsActivityTagMaxHoursDailyPercentage[sb].at(pos2) >= samd->weightPercentage){
							 	//do nothing
							}
							else if(subgroupsActivityTagMaxHoursDailyMaxHours[sb].at(pos2) >= samd->maxHoursDaily
							 && subgroupsActivityTagMaxHoursDailyPercentage[sb].at(pos2) <= samd->weightPercentage){
							
								subgroupsActivityTagMaxHoursDailyActivityTag[sb][pos2]=samd->activityTagIndex;
								subgroupsActivityTagMaxHoursDailyMaxHours[sb][pos2]=samd->maxHoursDaily;
								subgroupsActivityTagMaxHoursDailyPercentage[sb][pos2]=samd->weightPercentage;
							}
							else{
								ok=false;
	
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for subgroup %1 and activity tag %2, because there are too many constraints"
								 " of type activity tag max hours daily relating to them, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a subgroup and an activity tag, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours daily.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalSubgroupsList[sb]->name)
								 .arg(samd->activityTagName),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY){
			haveStudentsActivityTagMaxHoursDaily=true;

			ConstraintStudentsSetActivityTagMaxHoursDaily* samd=(ConstraintStudentsSetActivityTagMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			for(int sb : qAsConst(samd->canonicalSubgroupsList)){
				int pos1=-1, pos2=-1;
				
				for(int j=0; j<subgroupsActivityTagMaxHoursDailyMaxHours[sb].count(); j++){
					if(subgroupsActivityTagMaxHoursDailyActivityTag[sb].at(j)==samd->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}
				
				if(pos1==-1){
					subgroupsActivityTagMaxHoursDailyActivityTag[sb].append(samd->activityTagIndex);
					subgroupsActivityTagMaxHoursDailyMaxHours[sb].append(samd->maxHoursDaily);
					subgroupsActivityTagMaxHoursDailyPercentage[sb].append(samd->weightPercentage);
				}
				else{
					if(subgroupsActivityTagMaxHoursDailyMaxHours[sb].at(pos1) <= samd->maxHoursDaily
					 && subgroupsActivityTagMaxHoursDailyPercentage[sb].at(pos1) >= samd->weightPercentage){
					 	//do nothing
					}
					else if(subgroupsActivityTagMaxHoursDailyMaxHours[sb].at(pos1) >= samd->maxHoursDaily
					 && subgroupsActivityTagMaxHoursDailyPercentage[sb].at(pos1) <= samd->weightPercentage){
					
						subgroupsActivityTagMaxHoursDailyActivityTag[sb][pos1]=samd->activityTagIndex;
						subgroupsActivityTagMaxHoursDailyMaxHours[sb][pos1]=samd->maxHoursDaily;
						subgroupsActivityTagMaxHoursDailyPercentage[sb][pos1]=samd->weightPercentage;
					}
					else{
						if(pos2==-1){
							subgroupsActivityTagMaxHoursDailyActivityTag[sb].append(samd->activityTagIndex);
							subgroupsActivityTagMaxHoursDailyMaxHours[sb].append(samd->maxHoursDaily);
							subgroupsActivityTagMaxHoursDailyPercentage[sb].append(samd->weightPercentage);
						}
						else{

							if(subgroupsActivityTagMaxHoursDailyMaxHours[sb].at(pos2) <= samd->maxHoursDaily
							 && subgroupsActivityTagMaxHoursDailyPercentage[sb].at(pos2) >= samd->weightPercentage){
							 	//do nothing
							}
							else if(subgroupsActivityTagMaxHoursDailyMaxHours[sb].at(pos2) >= samd->maxHoursDaily
							 && subgroupsActivityTagMaxHoursDailyPercentage[sb].at(pos2) <= samd->weightPercentage){
							
								subgroupsActivityTagMaxHoursDailyActivityTag[sb][pos2]=samd->activityTagIndex;
								subgroupsActivityTagMaxHoursDailyMaxHours[sb][pos2]=samd->maxHoursDaily;
								subgroupsActivityTagMaxHoursDailyPercentage[sb][pos2]=samd->weightPercentage;
							}
							else{
								ok=false;
	
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for subgroup %1 and activity tag %2, because there are too many constraints"
								 " of type activity tag max hours daily relating to them, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a subgroup and an activity tag, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours daily.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalSubgroupsList[sb]->name)
								 .arg(samd->activityTagName),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
	}

	Matrix1D<int> navd;
	navd.resize(gt.rules.nDaysPerWeek);
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			navd[d]=0;
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[i][d][h])
					navd[d]++;
			}
		}
	
		for(int j=0; j<subgroupsActivityTagMaxHoursDailyMaxHours[i].count(); j++){
			int mh=subgroupsActivityTagMaxHoursDailyMaxHours[i].at(j);
			double perc=subgroupsActivityTagMaxHoursDailyPercentage[i].at(j);
			int at=subgroupsActivityTagMaxHoursDailyActivityTag[i].at(j);
			if(perc==100.0){
				int totalAt=0;
				for(int ai : qAsConst(gt.rules.internalSubgroupsList[i]->activitiesForSubgroup))
					if(gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(at))
						totalAt+=gt.rules.internalActivitiesList[ai].duration;
						
				int ava=0;
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					ava+=min(navd[d], mh);
					
				if(ava<totalAt){
					ok=false;
					
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there is a constraint activity tag %2 max %3 hours daily for it with weight 100%"
					 " which cannot be satisfied, considering the number of available slots (%4) and total duration of activities with this activity tag (%5)"
					 ". Please correct and try again.", "%2 is the activity tag for this constraint, %3 is the max number of hours daily for this constraint")
					 .arg(gt.rules.internalSubgroupsList[i]->name).arg(gt.rules.activityTagsList.at(at)->name).arg(mh).arg(ava).arg(totalAt),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
		}
	}
	
	return ok;
}

bool computeStudentsActivityTagMaxHoursDailyRealDays(QWidget* parent)
{
	haveStudentsActivityTagMaxHoursDailyRealDays=false;

	bool ok=true;

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[i].clear();
		subgroupsActivityTagMaxHoursDailyRealDaysPercentage[i].clear();
		subgroupsActivityTagMaxHoursDailyRealDaysActivityTag[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS){
			haveStudentsActivityTagMaxHoursDailyRealDays=true;

			ConstraintStudentsActivityTagMaxHoursDailyRealDays* samd=(ConstraintStudentsActivityTagMaxHoursDailyRealDays*)gt.rules.internalTimeConstraintsList[i];

			for(int sb : qAsConst(samd->canonicalSubgroupsList)){
				int pos1=-1, pos2=-1;

				for(int j=0; j<subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb].count(); j++){
					if(subgroupsActivityTagMaxHoursDailyRealDaysActivityTag[sb].at(j)==samd->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}

				if(pos1==-1){
					subgroupsActivityTagMaxHoursDailyRealDaysActivityTag[sb].append(samd->activityTagIndex);
					subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb].append(samd->maxHoursDaily);
					subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sb].append(samd->weightPercentage);
				}
				else{
					if(subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb].at(pos1) <= samd->maxHoursDaily
					 && subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sb].at(pos1) >= samd->weightPercentage){
						//do nothing
					}
					else if(subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb].at(pos1) >= samd->maxHoursDaily
					 && subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sb].at(pos1) <= samd->weightPercentage){

						subgroupsActivityTagMaxHoursDailyRealDaysActivityTag[sb][pos1]=samd->activityTagIndex;
						subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb][pos1]=samd->maxHoursDaily;
						subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sb][pos1]=samd->weightPercentage;
					}
					else{
						if(pos2==-1){
							subgroupsActivityTagMaxHoursDailyRealDaysActivityTag[sb].append(samd->activityTagIndex);
							subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb].append(samd->maxHoursDaily);
							subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sb].append(samd->weightPercentage);
						}
						else{

							if(subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb].at(pos2) <= samd->maxHoursDaily
							 && subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sb].at(pos2) >= samd->weightPercentage){
								//do nothing
							}
							else if(subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb].at(pos2) >= samd->maxHoursDaily
							 && subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sb].at(pos2) <= samd->weightPercentage){

								subgroupsActivityTagMaxHoursDailyRealDaysActivityTag[sb][pos2]=samd->activityTagIndex;
								subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb][pos2]=samd->maxHoursDaily;
								subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sb][pos2]=samd->weightPercentage;
							}
							else{
								ok=false;

								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for subgroup %1 and activity tag %2, because there are too many constraints"
								 " of type activity tag max hours daily per real day relating to them, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a subgroup and an activity tag, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours daily per real day.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalSubgroupsList[sb]->name)
								 .arg(samd->activityTagName),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS){
			haveStudentsActivityTagMaxHoursDailyRealDays=true;

			ConstraintStudentsSetActivityTagMaxHoursDailyRealDays* samd=(ConstraintStudentsSetActivityTagMaxHoursDailyRealDays*)gt.rules.internalTimeConstraintsList[i];

			for(int sb : qAsConst(samd->canonicalSubgroupsList)){
				int pos1=-1, pos2=-1;

				for(int j=0; j<subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb].count(); j++){
					if(subgroupsActivityTagMaxHoursDailyRealDaysActivityTag[sb].at(j)==samd->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}

				if(pos1==-1){
					subgroupsActivityTagMaxHoursDailyRealDaysActivityTag[sb].append(samd->activityTagIndex);
					subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb].append(samd->maxHoursDaily);
					subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sb].append(samd->weightPercentage);
				}
				else{
					if(subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb].at(pos1) <= samd->maxHoursDaily
					 && subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sb].at(pos1) >= samd->weightPercentage){
						//do nothing
					}
					else if(subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb].at(pos1) >= samd->maxHoursDaily
					 && subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sb].at(pos1) <= samd->weightPercentage){

						subgroupsActivityTagMaxHoursDailyRealDaysActivityTag[sb][pos1]=samd->activityTagIndex;
						subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb][pos1]=samd->maxHoursDaily;
						subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sb][pos1]=samd->weightPercentage;
					}
					else{
						if(pos2==-1){
							subgroupsActivityTagMaxHoursDailyRealDaysActivityTag[sb].append(samd->activityTagIndex);
							subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb].append(samd->maxHoursDaily);
							subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sb].append(samd->weightPercentage);
						}
						else{

							if(subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb].at(pos2) <= samd->maxHoursDaily
							 && subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sb].at(pos2) >= samd->weightPercentage){
								//do nothing
							}
							else if(subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb].at(pos2) >= samd->maxHoursDaily
							 && subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sb].at(pos2) <= samd->weightPercentage){

								subgroupsActivityTagMaxHoursDailyRealDaysActivityTag[sb][pos2]=samd->activityTagIndex;
								subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sb][pos2]=samd->maxHoursDaily;
								subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sb][pos2]=samd->weightPercentage;
							}
							else{
								ok=false;

								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for subgroup %1 and activity tag %2, because there are too many constraints"
								 " of type activity tag max hours daily per real day relating to them, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a subgroup and an activity tag, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours daily per real day.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalSubgroupsList[sb]->name)
								 .arg(samd->activityTagName),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
	}

	Matrix1D<int> navd;
	navd.resize(gt.rules.nDaysPerWeek/2);

	if(gt.rules.mode==MORNINGS_AFTERNOONS)
		assert(gt.rules.nDaysPerWeek%2==0);

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		for(int d=0; d<gt.rules.nDaysPerWeek/2; d++){
			navd[d]=0;
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				if(!breakDayHour[2*d][h] && !subgroupNotAvailableDayHour[i][2*d][h])
					navd[d]++;
				if(!breakDayHour[2*d+1][h] && !subgroupNotAvailableDayHour[i][2*d+1][h])
					navd[d]++;
			}
		}

		for(int j=0; j<subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[i].count(); j++){
			int mh=subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[i].at(j);
			double perc=subgroupsActivityTagMaxHoursDailyRealDaysPercentage[i].at(j);
			int at=subgroupsActivityTagMaxHoursDailyRealDaysActivityTag[i].at(j);
			if(perc==100.0){
				int totalAt=0;
				for(int ai : qAsConst(gt.rules.internalSubgroupsList[i]->activitiesForSubgroup))
					if(gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(at))
						totalAt+=gt.rules.internalActivitiesList[ai].duration;

				int ava=0;
				for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
					ava+=min(navd[d], mh);

				if(ava<totalAt){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there is a constraint activity tag %2 max %3 hours daily per real day for it with weight 100%"
					 " which cannot be satisfied, considering the number of available slots (%4) and total duration of activities with this activity tag (%5)"
					 ". Please correct and try again.", "%2 is the activity tag for this constraint, %3 is the max number of hours daily for this constraint")
					 .arg(gt.rules.internalSubgroupsList[i]->name).arg(gt.rules.activityTagsList.at(at)->name).arg(mh).arg(ava).arg(totalAt),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
		}
	}

	return ok;
}

bool computeStudentsActivityTagMaxHoursContinuously(QWidget* parent)
{
	haveStudentsActivityTagMaxHoursContinuously=false;
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsActivityTagMaxHoursContinuouslyMaxHours[i].clear();
		subgroupsActivityTagMaxHoursContinuouslyPercentage[i].clear();
		subgroupsActivityTagMaxHoursContinuouslyActivityTag[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			haveStudentsActivityTagMaxHoursContinuously=true;

			ConstraintStudentsActivityTagMaxHoursContinuously* samc=(ConstraintStudentsActivityTagMaxHoursContinuously*)gt.rules.internalTimeConstraintsList[i];
			
			for(int sb : qAsConst(samc->canonicalSubgroupsList)){
				int pos1=-1, pos2=-1;
				
				for(int j=0; j<subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].count(); j++){
					if(subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb].at(j)==samc->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}
				
				if(pos1==-1){
					subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb].append(samc->activityTagIndex);
					subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].append(samc->maxHoursContinuously);
					subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].append(samc->weightPercentage);
				}
				else{
					if(subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].at(pos1) <= samc->maxHoursContinuously
					 && subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].at(pos1) >= samc->weightPercentage){
					 	//do nothing
					}
					else if(subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].at(pos1) >= samc->maxHoursContinuously
					 && subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].at(pos1) <= samc->weightPercentage){
					
						subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb][pos1]=samc->activityTagIndex;
						subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb][pos1]=samc->maxHoursContinuously;
						subgroupsActivityTagMaxHoursContinuouslyPercentage[sb][pos1]=samc->weightPercentage;
					}
					else{
						if(pos2==-1){
							subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb].append(samc->activityTagIndex);
							subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].append(samc->maxHoursContinuously);
							subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].append(samc->weightPercentage);
						}
						else{

							if(subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].at(pos2) <= samc->maxHoursContinuously
							 && subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].at(pos2) >= samc->weightPercentage){
							 	//do nothing
							}
							else if(subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].at(pos2) >= samc->maxHoursContinuously
							 && subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].at(pos2) <= samc->weightPercentage){
							
								subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb][pos2]=samc->activityTagIndex;
								subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb][pos2]=samc->maxHoursContinuously;
								subgroupsActivityTagMaxHoursContinuouslyPercentage[sb][pos2]=samc->weightPercentage;
							}
							else{
								ok=false;
	
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for subgroup %1 and activity tag %2, because there are too many constraints"
								 " of type activity tag max hours continuously relating to them, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a subgroup and an activity tag, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours continuously.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalSubgroupsList[sb]->name)
								 .arg(samc->activityTagName),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			haveStudentsActivityTagMaxHoursContinuously=true;

			ConstraintStudentsSetActivityTagMaxHoursContinuously* samc=(ConstraintStudentsSetActivityTagMaxHoursContinuously*)gt.rules.internalTimeConstraintsList[i];
			
			for(int sb : qAsConst(samc->canonicalSubgroupsList)){
				int pos1=-1, pos2=-1;
				
				for(int j=0; j<subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].count(); j++){
					if(subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb].at(j)==samc->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}
				
				if(pos1==-1){
					subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb].append(samc->activityTagIndex);
					subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].append(samc->maxHoursContinuously);
					subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].append(samc->weightPercentage);
				}
				else{
					if(subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].at(pos1) <= samc->maxHoursContinuously
					 && subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].at(pos1) >= samc->weightPercentage){
					 	//do nothing
					}
					else if(subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].at(pos1) >= samc->maxHoursContinuously
					 && subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].at(pos1) <= samc->weightPercentage){
					
						subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb][pos1]=samc->activityTagIndex;
						subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb][pos1]=samc->maxHoursContinuously;
						subgroupsActivityTagMaxHoursContinuouslyPercentage[sb][pos1]=samc->weightPercentage;
					}
					else{
						if(pos2==-1){
							subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb].append(samc->activityTagIndex);
							subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].append(samc->maxHoursContinuously);
							subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].append(samc->weightPercentage);
						}
						else{

							if(subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].at(pos2) <= samc->maxHoursContinuously
							 && subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].at(pos2) >= samc->weightPercentage){
							 	//do nothing
							}
							else if(subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb].at(pos2) >= samc->maxHoursContinuously
							 && subgroupsActivityTagMaxHoursContinuouslyPercentage[sb].at(pos2) <= samc->weightPercentage){
							
								subgroupsActivityTagMaxHoursContinuouslyActivityTag[sb][pos2]=samc->activityTagIndex;
								subgroupsActivityTagMaxHoursContinuouslyMaxHours[sb][pos2]=samc->maxHoursContinuously;
								subgroupsActivityTagMaxHoursContinuouslyPercentage[sb][pos2]=samc->weightPercentage;
							}
							else{
								ok=false;
	
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for subgroup %1 and activity tag %2, because there are too many constraints"
								 " of type activity tag max hours continuously relating to them, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a subgroup and an activity tag, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours continuously.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalSubgroupsList[sb]->name)
								 .arg(samc->activityTagName),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
	}
	
	return ok;
}

bool checkMaxHoursForActivityDuration(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		Activity* act=&gt.rules.internalActivitiesList[i];
		
		//teachers
		for(int tch : qAsConst(act->iTeachersList)){
			if(teachersMaxHoursDailyPercentages1[tch]==100.0){
				int m=teachersMaxHoursDailyMaxHours1[tch];
				if(act->duration > m){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
					 " a constraint teacher(s) max %3 hours daily with weight = 100% for the teacher %4. The activity's duration is"
					 " higher than the teacher's max hours daily. Please correct and try again.")
					 .arg(act->id)
					 .arg(act->duration)
					 .arg(m)
					 .arg(gt.rules.internalTeachersList[tch]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
			if(teachersMaxHoursDailyPercentages2[tch]==100.0){
				int m=teachersMaxHoursDailyMaxHours2[tch];
				if(act->duration > m){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
					 " a constraint teacher(s) max %3 hours daily with weight = 100% for the teacher %4. The activity's duration is"
					 " higher than the teacher's max hours daily. Please correct and try again.")
					 .arg(act->id)
					 .arg(act->duration)
					 .arg(m)
					 .arg(gt.rules.internalTeachersList[tch]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}

			if(teachersMaxHoursContinuouslyPercentages1[tch]==100.0){
				int m=teachersMaxHoursContinuouslyMaxHours1[tch];
				if(act->duration > m){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
					 " a constraint teacher(s) max %3 hours continuously with weight = 100% for the teacher %4. The activity's duration is"
					 " higher than the teacher's max hours continuously. Please correct and try again.")
					 .arg(act->id)
					 .arg(act->duration)
					 .arg(m)
					 .arg(gt.rules.internalTeachersList[tch]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
			if(teachersMaxHoursContinuouslyPercentages2[tch]==100.0){
				int m=teachersMaxHoursContinuouslyMaxHours2[tch];
				if(act->duration > m){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
					 " a constraint teacher(s) max %3 hours continuously with weight = 100% for the teacher %4. The activity's duration is"
					 " higher than the teacher's max hours continuously. Please correct and try again.")
					 .arg(act->id)
					 .arg(act->duration)
					 .arg(m)
					 .arg(gt.rules.internalTeachersList[tch]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}

			for(int j=0; j<teachersActivityTagMaxHoursDailyMaxHours[tch].count(); j++){
				if(teachersActivityTagMaxHoursDailyPercentage[tch].at(j)==100.0){
					int m=teachersActivityTagMaxHoursDailyMaxHours[tch].at(j);
					int at=teachersActivityTagMaxHoursDailyActivityTag[tch].at(j);
					if(act->iActivityTagsSet.contains(at) && act->duration > m){
						ok=false;
	
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
						 " a constraint teacher(s) activity tag max %3 hours daily with weight = 100% for the teacher %4 and activity tag %5."
						 " The activity's duration is higher than the teacher's max hours daily with this activity tag (which the activity contains)."
						 " Please correct and try again.")
						 .arg(act->id)
						 .arg(act->duration)
						 .arg(m)
						 .arg(gt.rules.internalTeachersList[tch]->name)
						 .arg(gt.rules.activityTagsList[at]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							return false;
					}
				}
			}

			for(int j=0; j<teachersActivityTagMaxHoursContinuouslyMaxHours[tch].count(); j++){
				if(teachersActivityTagMaxHoursContinuouslyPercentage[tch].at(j)==100.0){
					int m=teachersActivityTagMaxHoursContinuouslyMaxHours[tch].at(j);
					int at=teachersActivityTagMaxHoursContinuouslyActivityTag[tch].at(j);
					if(act->iActivityTagsSet.contains(at) && act->duration > m){
						ok=false;
	
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
						 " a constraint teacher(s) activity tag max %3 hours continuously with weight = 100% for the teacher %4 and activity tag %5."
						 " The activity's duration is higher than the teacher's max hours continuously with this activity tag (which the activity contains)."
						 " Please correct and try again.")
						 .arg(act->id)
						 .arg(act->duration)
						 .arg(m)
						 .arg(gt.rules.internalTeachersList[tch]->name)
						 .arg(gt.rules.activityTagsList[at]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							return false;
					}
				}
			}
		}

		//students
		for(int sbg : qAsConst(act->iSubgroupsList)){
			if(subgroupsMaxHoursDailyPercentages1[sbg]==100.0){
				int m=subgroupsMaxHoursDailyMaxHours1[sbg];
				if(act->duration > m){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
					 " a constraint students (set) max %3 hours daily with weight = 100% for the subgroup %4. The activity's duration is"
					 " higher than the subgroup's max hours daily. Please correct and try again.")
					 .arg(act->id)
					 .arg(act->duration)
					 .arg(m)
					 .arg(gt.rules.internalSubgroupsList[sbg]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}

			if(subgroupsMaxHoursDailyPercentages2[sbg]==100.0){
				int m=subgroupsMaxHoursDailyMaxHours2[sbg];
				if(act->duration > m){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
					 " a constraint students (set) max %3 hours daily with weight = 100% for the subgroup %4. The activity's duration is"
					 " higher than the subgroup's max hours daily. Please correct and try again.")
					 .arg(act->id)
					 .arg(act->duration)
					 .arg(m)
					 .arg(gt.rules.internalSubgroupsList[sbg]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
		
			if(subgroupsMaxHoursContinuouslyPercentages1[sbg]==100.0){
				int m=subgroupsMaxHoursContinuouslyMaxHours1[sbg];
				if(act->duration > m){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
					 " a constraint students (set) max %3 hours continuously with weight = 100% for the subgroup %4. The activity's duration is"
					 " higher than the subgroup's max hours continuously. Please correct and try again.")
					 .arg(act->id)
					 .arg(act->duration)
					 .arg(m)
					 .arg(gt.rules.internalSubgroupsList[sbg]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
			if(subgroupsMaxHoursContinuouslyPercentages2[sbg]==100.0){
				int m=subgroupsMaxHoursContinuouslyMaxHours2[sbg];
				if(act->duration > m){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
					 " a constraint students (set) max %3 hours continuously with weight = 100% for the subgroup %4. The activity's duration is"
					 " higher than the subgroup's max hours continuously. Please correct and try again.")
					 .arg(act->id)
					 .arg(act->duration)
					 .arg(m)
					 .arg(gt.rules.internalSubgroupsList[sbg]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
		
			for(int j=0; j<subgroupsActivityTagMaxHoursDailyMaxHours[sbg].count(); j++){
				if(subgroupsActivityTagMaxHoursDailyPercentage[sbg].at(j)==100.0){
					int m=subgroupsActivityTagMaxHoursDailyMaxHours[sbg].at(j);
					int at=subgroupsActivityTagMaxHoursDailyActivityTag[sbg].at(j);
					if(act->iActivityTagsSet.contains(at) && act->duration > m){
						ok=false;
	
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
						 " a constraint students (set) activity tag max %3 hours daily with weight = 100% for the subgroup %4 and activity tag %5."
						 " The activity's duration is higher than the subgroup's max hours daily with this activity tag (which the activity contains)."
						 " Please correct and try again.")
						 .arg(act->id)
						 .arg(act->duration)
						 .arg(m)
						 .arg(gt.rules.internalSubgroupsList[sbg]->name)
						 .arg(gt.rules.activityTagsList[at]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							return false;
					}
				}
			}

			for(int j=0; j<subgroupsActivityTagMaxHoursContinuouslyMaxHours[sbg].count(); j++){
				if(subgroupsActivityTagMaxHoursContinuouslyPercentage[sbg].at(j)==100.0){
					int m=subgroupsActivityTagMaxHoursContinuouslyMaxHours[sbg].at(j);
					int at=subgroupsActivityTagMaxHoursContinuouslyActivityTag[sbg].at(j);
					if(act->iActivityTagsSet.contains(at) && act->duration > m){
						ok=false;
	
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize, because you have activity id = %1 with duration = %2 and"
						 " a constraint students (set) activity tag max %3 hours continuously with weight = 100% for the subgroup %4 and activity tag %5."
						 " The activity's duration is higher than the subgroup's max hours continuously with this activity tag (which the activity contains)."
						 " Please correct and try again.")
						 .arg(act->id)
						 .arg(act->duration)
						 .arg(m)
						 .arg(gt.rules.internalSubgroupsList[sbg]->name)
						 .arg(gt.rules.activityTagsList[at]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							return false;
					}
				}
			}
		}
	}
	
	return ok;
}

bool computeSubgroupsMinHoursDaily(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMinHoursDailyMinHours[i][1]=-1;
		subgroupsMinHoursDailyPercentages[i][1]=-1;
		subgroupsMinHoursDailyAllowEmptyDays[i]=true;
		subgroupsMinHoursPerMorningAllowEmptyMornings[i]=true;

		subgroupsMinHoursPerAfternoonMinHours[i]=-1;
		subgroupsMinHoursPerAfternoonPercentages[i]=-1;
		subgroupsMinHoursPerAfternoonAllowEmptyAfternoons[i]=true;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_HOURS_DAILY){
			ConstraintStudentsMinHoursDaily* smd=(ConstraintStudentsMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			if(smd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for students, because the constraint of type min hours daily relating to students"
				 " has no 100% weight"
				 ". Please modify your data accordingly and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			//////////
			if(smd->minHoursDaily>gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min hours daily with"
				 " %1 min hours daily, and the number of working hours per day is only %2. Please correct and try again")
				 .arg(smd->minHoursDaily)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
			ConstraintStudentsSetMinHoursDaily* smd=(ConstraintStudentsSetMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			if(smd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for students set %1, because the constraint of type min hours daily relating to it"
				 " has no 100% weight"
				 ". Please modify your data accordingly and try again")
				 .arg(smd->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			//////////
			if(smd->minHoursDaily>gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min hours daily for students set %1 with"
				 " %2 min hours daily, and the number of working hours per day is only %3. Please correct and try again")
				 .arg(smd->students)
				 .arg(smd->minHoursDaily)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////
		}
	}

	if(!ok)
		return ok;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_HOURS_DAILY){
			ConstraintStudentsMinHoursDaily* smd=(ConstraintStudentsMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMinHoursDailyMinHours[sb][MIN_HOURS_DAILY_INDEX_IN_ARRAY]==-1 ||
				 (subgroupsMinHoursDailyMinHours[sb][MIN_HOURS_DAILY_INDEX_IN_ARRAY] <= smd->minHoursDaily &&
				 subgroupsMinHoursDailyPercentages[sb][MIN_HOURS_DAILY_INDEX_IN_ARRAY] <= smd->weightPercentage)){
					subgroupsMinHoursDailyMinHours[sb][MIN_HOURS_DAILY_INDEX_IN_ARRAY] = smd->minHoursDaily;
					subgroupsMinHoursDailyPercentages[sb][MIN_HOURS_DAILY_INDEX_IN_ARRAY] = smd->weightPercentage;
					}
				else if(subgroupsMinHoursDailyMinHours[sb][MIN_HOURS_DAILY_INDEX_IN_ARRAY] >= smd->minHoursDaily &&
				 subgroupsMinHoursDailyPercentages[sb][MIN_HOURS_DAILY_INDEX_IN_ARRAY] >= smd->weightPercentage){
					//nothing
				}
				else{ //cannot proceed
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are two constraints"
					 " of type min hours daily relating to it, and the weight percentage is higher on the constraint"
					 " with less minimum hours. You are allowed only to have for each subgroup"
					 " the most important constraint with maximum weight percentage and largest minimum hours daily allowed"
					 ". Please modify your data accordingly and try again")
					 .arg(gt.rules.internalSubgroupsList[sb]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}

				if(smd->allowEmptyDays==false)
					subgroupsMinHoursDailyAllowEmptyDays[sb]=false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
			ConstraintStudentsSetMinHoursDaily* smd=(ConstraintStudentsSetMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			for(int q=0; q<smd->iSubgroupsList.count(); q++){
				int sb=smd->iSubgroupsList.at(q);
				if(subgroupsMinHoursDailyMinHours[sb][MIN_HOURS_DAILY_INDEX_IN_ARRAY]==-1 ||
				 (subgroupsMinHoursDailyMinHours[sb][MIN_HOURS_DAILY_INDEX_IN_ARRAY] <= smd->minHoursDaily &&
				 subgroupsMinHoursDailyPercentages[sb][MIN_HOURS_DAILY_INDEX_IN_ARRAY] <= smd->weightPercentage)){
					subgroupsMinHoursDailyMinHours[sb][MIN_HOURS_DAILY_INDEX_IN_ARRAY] = smd->minHoursDaily;
					subgroupsMinHoursDailyPercentages[sb][MIN_HOURS_DAILY_INDEX_IN_ARRAY] = smd->weightPercentage;
					}
				else if(subgroupsMinHoursDailyMinHours[sb][MIN_HOURS_DAILY_INDEX_IN_ARRAY] >= smd->minHoursDaily &&
				 subgroupsMinHoursDailyPercentages[sb][MIN_HOURS_DAILY_INDEX_IN_ARRAY] >= smd->weightPercentage){
					//nothing
				}
				else{ //cannot proceed
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because there are two constraints"
					 " of type min hours daily relating to it, and the weight percentage is higher on the constraint"
					 " with less minimum hours. You are allowed only to have for each subgroup"
					 " the most important constraint with maximum weight percentage and largest minimum hours daily allowed"
					 ". Please modify your data accordingly and try again")
					 .arg(gt.rules.internalSubgroupsList[sb]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
				if(smd->allowEmptyDays==false)
					subgroupsMinHoursDailyAllowEmptyDays[sb]=false;
			}
		}
	}

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMinHoursDailyMinHours[i][0]=subgroupsMinHoursDailyMinHours[i][1];
		subgroupsMinHoursDailyPercentages[i][0]=subgroupsMinHoursDailyPercentages[i][1];
	}


	//2020-06-14
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_MORNING){
			ConstraintStudentsSetMinHoursPerMorning* smd=(ConstraintStudentsSetMinHoursPerMorning*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(smd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min hours per morning for students set %1 with"
				 " weight (percentage) below 100. Starting with FET version 5.4.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(smd->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(smd->minHoursPerMorning>gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min hours per morning for students set %1 with"
				 " %2 min hours per morning, and the number of working hours per day is only %3. Please correct and try again")
				 .arg(smd->students)
				 .arg(smd->minHoursPerMorning)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int sbg : qAsConst(smd->iSubgroupsList)){
				if(subgroupsMinHoursDailyMinHours[sbg][1]==-1){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min hours per morning for subgroup %1 but not also"
					 " min hours daily for him. Please add a constraint students (set) min hours daily affecting this subgroup.")
					 .arg(gt.rules.internalSubgroupsList[sbg]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}

				if(subgroupsMinHoursDailyMinHours[sbg][0]==-1 || subgroupsMinHoursDailyMinHours[sbg][0]<smd->minHoursPerMorning){
					subgroupsMinHoursDailyMinHours[sbg][0]=smd->minHoursPerMorning;
					subgroupsMinHoursDailyPercentages[sbg][0]=100;
				}

				if(subgroupsMinHoursPerMorningAllowEmptyMornings[sbg]==true && smd->allowEmptyMornings==false)
					subgroupsMinHoursPerMorningAllowEmptyMornings[sbg]=false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_HOURS_PER_MORNING){
			ConstraintStudentsMinHoursPerMorning* smd=(ConstraintStudentsMinHoursPerMorning*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(smd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min hours per morning with"
				 " weight (percentage) below 100. Starting with FET version 5.4.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(smd->minHoursPerMorning>gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min hours per morning with"
				 " %1 min hours per morning, and the number of working hours per day is only %2. Please correct and try again")
				 .arg(smd->minHoursPerMorning)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				if(subgroupsMinHoursDailyMinHours[sbg][1]==-1){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min hours per morning for subgroup %1 but not also"
					 " min hours daily for him. Please add a constraint students (set) min hours daily affecting this subgroup.")
					 .arg(gt.rules.internalSubgroupsList[sbg]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}

				if(subgroupsMinHoursDailyMinHours[sbg][0]==-1 || subgroupsMinHoursDailyMinHours[sbg][0]<smd->minHoursPerMorning){
					subgroupsMinHoursDailyMinHours[sbg][0]=smd->minHoursPerMorning;
					subgroupsMinHoursDailyPercentages[sbg][0]=100;
				}

				if(subgroupsMinHoursPerMorningAllowEmptyMornings[sbg]==true && smd->allowEmptyMornings==false)
					subgroupsMinHoursPerMorningAllowEmptyMornings[sbg]=false;
			}
		}
	}


	//2022-09-10
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_AFTERNOON){
			ConstraintStudentsSetMinHoursPerAfternoon* smd=(ConstraintStudentsSetMinHoursPerAfternoon*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(smd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min hours per afternoon for students set %1 with"
				 " weight (percentage) below 100. Starting with FET version 5.4.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(smd->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(smd->minHoursPerAfternoon>gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min hours per afternoon for students set %1 with"
				 " %2 min hours per afternoon, and the number of working hours per day is only %3. Please correct and try again")
				 .arg(smd->students)
				 .arg(smd->minHoursPerAfternoon)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int sbg : qAsConst(smd->iSubgroupsList)){
				if(subgroupsMinHoursDailyMinHours[sbg][1]==-1){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min hours per afternoon for subgroup %1 but not also"
					 " min hours daily for him. Please add a constraint students (set) min hours daily affecting this subgroup.")
					 .arg(gt.rules.internalSubgroupsList[sbg]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}

				if(subgroupsMinHoursPerAfternoonMinHours[sbg]==-1 || subgroupsMinHoursPerAfternoonMinHours[sbg]<smd->minHoursPerAfternoon){
					subgroupsMinHoursPerAfternoonMinHours[sbg]=smd->minHoursPerAfternoon;
					subgroupsMinHoursPerAfternoonPercentages[sbg]=100;
				}

				if(subgroupsMinHoursPerAfternoonAllowEmptyAfternoons[sbg]==true && smd->allowEmptyAfternoons==false)
					subgroupsMinHoursPerAfternoonAllowEmptyAfternoons[sbg]=false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_HOURS_PER_AFTERNOON){
			ConstraintStudentsMinHoursPerAfternoon* smd=(ConstraintStudentsMinHoursPerAfternoon*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(smd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min hours per afternoon with"
				 " weight (percentage) below 100. Starting with FET version 5.4.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(smd->minHoursPerAfternoon>gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min hours per afternoon with"
				 " %1 min hours per afternoon, and the number of working hours per day is only %2. Please correct and try again")
				 .arg(smd->minHoursPerAfternoon)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				if(subgroupsMinHoursDailyMinHours[sbg][1]==-1){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min hours per afternoon for subgroup %1 but not also"
					 " min hours daily for him. Please add a constraint students (set) min hours daily affecting this subgroup.")
					 .arg(gt.rules.internalSubgroupsList[sbg]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}

				if(subgroupsMinHoursPerAfternoonMinHours[sbg]==-1 || subgroupsMinHoursPerAfternoonMinHours[sbg]<smd->minHoursPerAfternoon){
					subgroupsMinHoursPerAfternoonMinHours[sbg]=smd->minHoursPerAfternoon;
					subgroupsMinHoursPerAfternoonPercentages[sbg]=100;
				}

				if(subgroupsMinHoursPerAfternoonAllowEmptyAfternoons[sbg]==true && smd->allowEmptyAfternoons==false)
					subgroupsMinHoursPerAfternoonAllowEmptyAfternoons[sbg]=false;
			}
		}
	}

	haveStudentsMinHoursDailyMorningsAfternoonsAllowEmptyDays=false;

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		for(int i=0; i<gt.rules.nInternalSubgroups; i++){
			if(subgroupsMinHoursDailyMinHours[i][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0 && subgroupsMinHoursDailyAllowEmptyDays[i]==true && !haveStudentsMinHoursDailyMorningsAfternoonsAllowEmptyDays)
				haveStudentsMinHoursDailyMorningsAfternoonsAllowEmptyDays=true;

			if(subgroupsMinHoursDailyMinHours[i][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0 && subgroupsMinHoursDailyAllowEmptyDays[i]==false){
				if(gt.rules.nDaysPerWeek*subgroupsMinHoursDailyMinHours[i][MIN_HOURS_DAILY_INDEX_IN_ARRAY] > nHoursPerSubgroup[i]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("For subgroup %1 you have too little activities to respect the constraint(s)"
					 " of type min hours daily (the constraint(s) do not allow empty days). Please modify your data accordingly and try again.")
					 .arg(gt.rules.internalSubgroupsList[i]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}

				for(int j=0; j<gt.rules.nDaysPerWeek; j++){
					int freeSlots=0;
					for(int k=0; k<gt.rules.nHoursPerDay; k++)
						if(!subgroupNotAvailableDayHour[i][j][k] && !breakDayHour[j][k])
							freeSlots++;
					if(subgroupsMinHoursDailyMinHours[i][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>freeSlots){
						ok=false;

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("For subgroup %1 cannot respect the constraint(s)"
						 " of type min hours daily (the constraint(s) do not allow empty days) on day %2, because of students set not available and/or break."
						 " Please modify your data accordingly and try again")
						 .arg(gt.rules.internalSubgroupsList[i]->name)
						 .arg(gt.rules.daysOfTheWeek[j]),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}
				}
			}
			else if(subgroupsMinHoursDailyMinHours[i][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0 && subgroupsMinHoursDailyAllowEmptyDays[i]==true){

				if(nHoursPerSubgroup[i]>0 && subgroupsMinHoursDailyMinHours[i][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>nHoursPerSubgroup[i]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min %1 hours daily for subgroup"
					 " %2 (the constraint allows empty days). This subgroup has in total only %3 hours per week, so impossible constraint."
					 " Please correct and try again")
					 .arg(subgroupsMinHoursDailyMinHours[i][MIN_HOURS_DAILY_INDEX_IN_ARRAY])
					 .arg(gt.rules.internalSubgroupsList[i]->name)
					 .arg(nHoursPerSubgroup[i])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}

				if(subgroupsMinHoursDailyMinHours[i][MIN_HOURS_DAILY_INDEX_IN_ARRAY]<2){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min %1 hours daily for subgroup"
					 " %2 and the constraint allows empty days. The number of min hours daily should be at least 2, to make a non-trivial constraint. Please correct and try again")
					 .arg(subgroupsMinHoursDailyMinHours[i][MIN_HOURS_DAILY_INDEX_IN_ARRAY])
					 .arg(gt.rules.internalSubgroupsList[i]->name)
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
		}
	}
	else{
		for(int i=0; i<gt.rules.nInternalSubgroups; i++){
			if(subgroupsMinHoursDailyMinHours[i][1]>=0 && (subgroupsMinHoursDailyAllowEmptyDays[i]==true
			 || subgroupsMinHoursPerMorningAllowEmptyMornings[i]==true || subgroupsMinHoursPerAfternoonAllowEmptyAfternoons[i]==true) && !haveStudentsMinHoursDailyMorningsAfternoonsAllowEmptyDays)
				haveStudentsMinHoursDailyMorningsAfternoonsAllowEmptyDays=true;

			if(subgroupsMinHoursDailyMinHours[i][1]>=0 && subgroupsMinHoursDailyAllowEmptyDays[i]==false){
				if(gt.rules.nDaysPerWeek/2*subgroupsMinHoursDailyMinHours[i][1] > nHoursPerSubgroup[i]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("For subgroup %1 you have too little activities to respect the constraint(s)"
					 " of type min hours daily (the constraint(s) do not allow empty days). Please modify your data accordingly and try again.")
					 .arg(gt.rules.internalSubgroupsList[i]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}

				/*for(int j=0; j<gt.rules.nDaysPerWeek; j++){
					int freeSlots=0;
					for(int k=0; k<gt.rules.nHoursPerDay; k++)
						if(!subgroupNotAvailableDayHour[i][j][k] && !breakDayHour[j][k])
							freeSlots++;
					if(subgroupsMinHoursDailyMinHours[i]>freeSlots){
						ok=false;

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("For subgroup %1 cannot respect the constraint(s)"
						 " of type min hours daily (the constraint(s) do not allow empty days) on day %2, because of students set not available and/or break."
						 " Please modify your data accordingly and try again")
						 .arg(gt.rules.internalSubgroupsList[i]->name)
						 .arg(gt.rules.daysOfTheWeek[j]),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}
				}*/
			}
			else if(subgroupsMinHoursDailyMinHours[i][1]>=0 && subgroupsMinHoursDailyAllowEmptyDays[i]==true){

				if(nHoursPerSubgroup[i]>0 && subgroupsMinHoursDailyMinHours[i][1]>nHoursPerSubgroup[i]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min %1 hours daily for subgroup"
					 " %2 (the constraint allows empty days). This subgroup has in total only %3 hours per week, so impossible constraint."
					 " Please correct and try again")
					 .arg(subgroupsMinHoursDailyMinHours[i][1])
					 .arg(gt.rules.internalSubgroupsList[i]->name)
					 .arg(nHoursPerSubgroup[i])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}

				/*if(subgroupsMinHoursDailyMinHours[i][1]<2){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min %1 hours daily for subgroup"
					 " %2 and the constraint allows empty days. The number of min hours daily should be at least 2, to make a non-trivial constraint. Please correct and try again")
					 .arg(subgroupsMinHoursDailyMinHours[i])
					 .arg(gt.rules.internalSubgroupsList[i]->name)
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}*/
			}
			if(subgroupsMinHoursDailyMinHours[i][0]>=0 && subgroupsMinHoursPerMorningAllowEmptyMornings[i]==false){
				if(gt.rules.nDaysPerWeek/2*subgroupsMinHoursDailyMinHours[i][0] > nHoursPerSubgroup[i]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("For subgroup %1 you have too little activities to respect the constraint(s)"
					 " of type min hours per morning (the constraint(s) do not allow empty mornings). Please modify your data accordingly and try again.")
					 .arg(gt.rules.internalSubgroupsList[i]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}

			if(subgroupsMinHoursPerAfternoonMinHours[i]>=0 && subgroupsMinHoursPerAfternoonAllowEmptyAfternoons[i]==false){
				if(gt.rules.nDaysPerWeek/2*subgroupsMinHoursPerAfternoonMinHours[i] > nHoursPerSubgroup[i]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("For subgroup %1 you have too little activities to respect the constraint(s)"
					 " of type min hours per afternoon (the constraint(s) do not allow empty afternoons). Please modify your data accordingly and try again.")
					 .arg(gt.rules.internalSubgroupsList[i]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
		}
	}

	return ok;
}

bool computeStudentsActivityTagMinHoursDaily(QWidget* parent)
{
	haveStudentsActivityTagMinHoursDaily=false;
	
	bool ok=true;
	
	satmhdList.clear();
	for(int i=0; i<gt.rules.nInternalSubgroups; i++)
		satmhdListForSubgroup[i].clear();
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MIN_HOURS_DAILY){
			haveStudentsActivityTagMinHoursDaily=true;
			ConstraintStudentsActivityTagMinHoursDaily* smd=(ConstraintStudentsActivityTagMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			if(smd->weightPercentage!=100){
				ok=false;
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for students, because the constraint of type activity tag min hours daily relating to students"
				 " has no 100% weight"
				 ". Please modify your data accordingly and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MIN_HOURS_DAILY){
			haveStudentsActivityTagMinHoursDaily=true;
			ConstraintStudentsSetActivityTagMinHoursDaily* smd=(ConstraintStudentsSetActivityTagMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			if(smd->weightPercentage!=100){
				ok=false;
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for students, because the constraint of type activity tag min hours daily relating to students set %1"
				 " has no 100% weight"
				 ". Please modify your data accordingly and try again").arg(smd->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}

	if(haveStudentsActivityTagMinHoursDaily){
		for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MIN_HOURS_DAILY){
				ConstraintStudentsActivityTagMinHoursDaily* smd=(ConstraintStudentsActivityTagMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];
				
				for(int sbg : qAsConst(smd->canonicalSubgroupsList)){
					SubgroupActivityTagMinHoursDaily_item item;
					item.durationOfActivitiesWithActivityTagForSubgroup=0;
					
					for(int ai : qAsConst(gt.rules.internalSubgroupsList[sbg]->activitiesForSubgroup)){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(act->iActivityTagsSet.contains(smd->activityTagIndex))
							item.durationOfActivitiesWithActivityTagForSubgroup+=act->duration;
					}
					
					if(item.durationOfActivitiesWithActivityTagForSubgroup>0){
						item.activityTag=smd->activityTagIndex;
						item.minHoursDaily=smd->minHoursDaily;
						item.allowEmptyDays=smd->allowEmptyDays;
						
						satmhdList.push_back(item);
						//satmhdListForSubgroup[sbg].append(&satmhdList[satmhdList.count()-1]);
						satmhdListForSubgroup[sbg].append(&satmhdList.back());
						
						if(!item.allowEmptyDays && item.durationOfActivitiesWithActivityTagForSubgroup<gt.rules.nDaysPerWeek*item.minHoursDaily){
							ok=false;
				
							int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
							 GeneratePreTranslate::tr("Cannot optimize, because the constraint of type activity tag %1 min %2 hours daily relating to subgroup %3"
							 " requires at least %4 hours of work per week, but the activities of this subgroup with this activity tag sum to only %5 hours"
							 " per week (the constraint does not allow empty days). Please correct and try again")
							 .arg(smd->activityTagName).arg(smd->minHoursDaily).arg(gt.rules.internalSubgroupsList[sbg]->name)
							 .arg(gt.rules.nDaysPerWeek*item.minHoursDaily).arg(item.durationOfActivitiesWithActivityTagForSubgroup),
							 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
							 1, 0 );
							 	
							if(t==0)
								return false;
						}
					}
					else{
						assert(0);
					}
				}
			}
			else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MIN_HOURS_DAILY){
				ConstraintStudentsSetActivityTagMinHoursDaily* smd=(ConstraintStudentsSetActivityTagMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];
				
				for(int sbg : qAsConst(smd->canonicalSubgroupsList)){
					SubgroupActivityTagMinHoursDaily_item item;
					item.durationOfActivitiesWithActivityTagForSubgroup=0;
					
					for(int ai : qAsConst(gt.rules.internalSubgroupsList[sbg]->activitiesForSubgroup)){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(act->iActivityTagsSet.contains(smd->activityTagIndex))
							item.durationOfActivitiesWithActivityTagForSubgroup+=act->duration;
					}
					
					if(item.durationOfActivitiesWithActivityTagForSubgroup>0){
						item.activityTag=smd->activityTagIndex;
						item.minHoursDaily=smd->minHoursDaily;
						item.allowEmptyDays=smd->allowEmptyDays;
						
						satmhdList.push_back(item);
						//satmhdListForSubgroup[sbg].append(&satmhdList[satmhdList.count()-1]);
						satmhdListForSubgroup[sbg].append(&satmhdList.back());
						
						if(!item.allowEmptyDays && item.durationOfActivitiesWithActivityTagForSubgroup<gt.rules.nDaysPerWeek*item.minHoursDaily){
							ok=false;
				
							int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
							 GeneratePreTranslate::tr("Cannot optimize, because the constraint of type activity tag %1 min %2 hours daily relating to subgroup %3"
							 " requires at least %4 hours of work per week, but the activities of this subgroup with this activity tag sum to only %5 hours"
							 " per week (the constraint does not allow empty days). Please correct and try again")
							 .arg(smd->activityTagName).arg(smd->minHoursDaily).arg(gt.rules.internalSubgroupsList[sbg]->name)
							 .arg(gt.rules.nDaysPerWeek*item.minHoursDaily).arg(item.durationOfActivitiesWithActivityTagForSubgroup),
							 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
							 1, 0 );
							 	
							if(t==0)
								return false;
						}
					}
					else{
						assert(0);
					}
				}
			}
		}
	}
	
	return ok;
}

//must be after allowed times, after n hours per teacher and after max days per week for teachers
bool computeTeachersMaxHoursDaily(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMaxHoursDailyMaxHours1[i]=-1;
		teachersMaxHoursDailyPercentages1[i]=-1;

		teachersMaxHoursDailyMaxHours2[i]=-1;
		teachersMaxHoursDailyPercentages2[i]=-1;
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY){
			ConstraintTeacherMaxHoursDaily* tmd=(ConstraintTeacherMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			if(tmd->weightPercentage<100.0)
				thereAreTeachersWithMaxHoursDailyOrPerRealDayWithUnder100Weight=true;

			//////////
			/*if(tmd->weightPercentage!=100){
				ok=false;

				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max hours daily for teacher %1 with"
				 " weight (percentage) below 100. Starting with FET version 5.3.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tmd->teacherName),
				 GeneratePreTranslate::tr("Skip rest of max hours problems"), GeneratePreTranslate::tr("See next incompatibility max hours"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}*/
			//////////

			if(teachersMaxHoursDailyMaxHours1[tmd->teacher_ID]==-1 ||
			 (teachersMaxHoursDailyMaxHours1[tmd->teacher_ID] >= tmd->maxHoursDaily &&
			 teachersMaxHoursDailyPercentages1[tmd->teacher_ID] <= tmd->weightPercentage)){
			 	teachersMaxHoursDailyMaxHours1[tmd->teacher_ID] = tmd->maxHoursDaily;
				teachersMaxHoursDailyPercentages1[tmd->teacher_ID] = tmd->weightPercentage;
			}
			else if(teachersMaxHoursDailyMaxHours1[tmd->teacher_ID] <= tmd->maxHoursDaily &&
			 teachersMaxHoursDailyPercentages1[tmd->teacher_ID] >= tmd->weightPercentage){
			 	//nothing
			}
			else{
				if(teachersMaxHoursDailyMaxHours2[tmd->teacher_ID]==-1 ||
				 (teachersMaxHoursDailyMaxHours2[tmd->teacher_ID] >= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyPercentages2[tmd->teacher_ID] <= tmd->weightPercentage)){
				 	teachersMaxHoursDailyMaxHours2[tmd->teacher_ID] = tmd->maxHoursDaily;
					teachersMaxHoursDailyPercentages2[tmd->teacher_ID] = tmd->weightPercentage;
				}
				else if(teachersMaxHoursDailyMaxHours2[tmd->teacher_ID] <= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyPercentages2[tmd->teacher_ID] >= tmd->weightPercentage){
				 	//nothing
				}
				else{ //cannot proceed
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are too many constraints"
					 " of type max hours daily relating to him, which cannot be compressed in 2 constraints of this type."
					 " Two constraints max hours can be compressed into a single one if the max hours are lower"
					 " in the first one and the weight percentage is higher on the first one."
					 " It is possible to use any number of such constraints for a teacher, but their resultant must"
					 " be maximum 2 constraints of type max hours daily.\n\n"
					 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
					 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
					 " Please modify your data accordingly and try again.")
					 .arg(gt.rules.internalTeachersList[tmd->teacher_ID]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_HOURS_DAILY){
			ConstraintTeachersMaxHoursDaily* tmd=(ConstraintTeachersMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			if(tmd->weightPercentage<100.0)
				thereAreTeachersWithMaxHoursDailyOrPerRealDayWithUnder100Weight=true;

			//////////
			/*if(tmd->weightPercentage!=100){
				ok=false;

				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max hours daily with"
				 " weight (percentage) below 100. Starting with FET version 5.3.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest of max hours problems"), GeneratePreTranslate::tr("See next incompatibility max hours"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}*/
			//////////

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMaxHoursDailyMaxHours1[tch]==-1 ||
				 (teachersMaxHoursDailyMaxHours1[tch] >= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyPercentages1[tch] <= tmd->weightPercentage)){
				 	teachersMaxHoursDailyMaxHours1[tch] = tmd->maxHoursDaily;
					teachersMaxHoursDailyPercentages1[tch] = tmd->weightPercentage;
					}
				else if(teachersMaxHoursDailyMaxHours1[tch] <= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyPercentages1[tch] >= tmd->weightPercentage){
				 	//nothing
				}
				else{
					if(teachersMaxHoursDailyMaxHours2[tch]==-1 ||
					 (teachersMaxHoursDailyMaxHours2[tch] >= tmd->maxHoursDaily &&
					 teachersMaxHoursDailyPercentages2[tch] <= tmd->weightPercentage)){
					 	teachersMaxHoursDailyMaxHours2[tch] = tmd->maxHoursDaily;
						teachersMaxHoursDailyPercentages2[tch] = tmd->weightPercentage;
						}
					else if(teachersMaxHoursDailyMaxHours2[tch] <= tmd->maxHoursDaily &&
					 teachersMaxHoursDailyPercentages2[tch] >= tmd->weightPercentage){
				 	//nothing
					}
					else{ //cannot proceed
						ok=false;

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are too many constraints"
						 " of type max hours daily relating to him, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a teacher, but their resultant must"
						 " be maximum 2 constraints of type max hours daily.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again.")
						 .arg(gt.rules.internalTeachersList[tch]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}
				}
			}
		}
	}
	
	Matrix1D<int> nAllowedSlotsPerDay;
	nAllowedSlotsPerDay.resize(gt.rules.nDaysPerWeek);
	
	Matrix1D<int> dayAvailable;
	dayAvailable.resize(gt.rules.nDaysPerWeek);

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMaxHoursDailyPercentages1[tc]==100){
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h])
						nAllowedSlotsPerDay[d]++;
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],teachersMaxHoursDailyMaxHours1[tc]);
			}
			
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				dayAvailable[d]=1;
			if(teachersMaxDaysPerWeekMaxDays[tc]>=0){
				//max days per week has 100% weight
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					dayAvailable[d]=0;
				assert(teachersMaxDaysPerWeekMaxDays[tc]<=gt.rules.nDaysPerWeek);
				for(int k=0; k<teachersMaxDaysPerWeekMaxDays[tc]; k++){
					int maxPos=-1, maxVal=-1;
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						if(dayAvailable[d]==0)
							if(maxVal<nAllowedSlotsPerDay[d]){
								maxVal=nAllowedSlotsPerDay[d];
								maxPos=d;
							}
					assert(maxPos>=0);
					assert(dayAvailable[maxPos]==0);
					dayAvailable[maxPos]=1;
				}
			}
			
			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				if(dayAvailable[d]==1)
					total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerTeacher[tc]){
				ok=false;
				
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint of type"
				 " max %2 hours daily with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, teacher max days per week, teacher not available and/or breaks."
				 " The number of total hours for this teacher is"
				 " %3 and the number of available slots is, considering max hours daily and all other constraints, %4.")
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(teachersMaxHoursDailyMaxHours1[tc])
				 .arg(nHoursPerTeacher[tc])
				 .arg(total);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMaxHoursDailyPercentages2[tc]==100){
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h])
						nAllowedSlotsPerDay[d]++;
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],teachersMaxHoursDailyMaxHours2[tc]);
			}
			
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				dayAvailable[d]=1;
			if(teachersMaxDaysPerWeekMaxDays[tc]>=0){
				//max days per week has 100% weight
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					dayAvailable[d]=0;
				assert(teachersMaxDaysPerWeekMaxDays[tc]<=gt.rules.nDaysPerWeek);
				for(int k=0; k<teachersMaxDaysPerWeekMaxDays[tc]; k++){
					int maxPos=-1, maxVal=-1;
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						if(dayAvailable[d]==0)
							if(maxVal<nAllowedSlotsPerDay[d]){
								maxVal=nAllowedSlotsPerDay[d];
								maxPos=d;
							}
					assert(maxPos>=0);
					assert(dayAvailable[maxPos]==0);
					dayAvailable[maxPos]=1;
				}
			}
			
			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				if(dayAvailable[d]==1)
					total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerTeacher[tc]){
				ok=false;
				
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint of type"
				 " max %2 hours daily with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, teacher max days per week, teacher not available and/or breaks."
				 " The number of total hours for this teacher is"
				 " %3 and the number of available slots is, considering max hours daily and all other constraints, %4.")
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(teachersMaxHoursDailyMaxHours2[tc])
				 .arg(nHoursPerTeacher[tc])
				 .arg(total);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}

	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
			if(teachersMaxHoursDailyPercentages1[tc]==100){
				for(int d=0; d<gt.rules.nDaysPerWeek; d++){
					nAllowedSlotsPerDay[d]=0;
					for(int h=0; h<gt.rules.nHoursPerDay; h++)
						if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h])
							nAllowedSlotsPerDay[d]++;
					nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],teachersMaxHoursDailyMaxHours1[tc]);
				}

				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					dayAvailable[d]=1;
				if(teachersMaxRealDaysPerWeekMaxDays[tc]>=0){
					//max days per week has 100% weight
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						dayAvailable[d]=0;
					assert(teachersMaxRealDaysPerWeekMaxDays[tc]<=gt.rules.nDaysPerWeek/2);
					for(int k=0; k<2*teachersMaxRealDaysPerWeekMaxDays[tc]; k++){
						int maxPos=-1, maxVal=-1;
						for(int d=0; d<gt.rules.nDaysPerWeek; d++)
							if(dayAvailable[d]==0)
								if(maxVal<nAllowedSlotsPerDay[d]){
									maxVal=nAllowedSlotsPerDay[d];
									maxPos=d;
								}
						assert(maxPos>=0);
						assert(dayAvailable[maxPos]==0);
						dayAvailable[maxPos]=1;
					}
				}

				int total=0;
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					if(dayAvailable[d]==1)
						total+=nAllowedSlotsPerDay[d];
				if(total<nHoursPerTeacher[tc]){
					ok=false;

					QString s;
					s=GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint of type"
					 " max %2 hours daily with 100% weight which cannot be respected because of number of days per week,"
					 " number of hours per day, teacher max days per week, teacher not available and/or breaks."
					 " The number of total hours for this teacher is"
					 " %3 and the number of available slots is, considering max hours daily and all other constraints, %4.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(teachersMaxHoursDailyMaxHours1[tc])
					 .arg(nHoursPerTeacher[tc])
					 .arg(total);
					s+="\n\n";
					s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
		}

		Matrix1D<int> nAllowedSlotsPerDay;
		nAllowedSlotsPerDay.resize(gt.rules.nDaysPerWeek);
		Matrix1D<int> dayAvailable;
		dayAvailable.resize(gt.rules.nDaysPerWeek);

		for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
			if(teachersMaxHoursDailyPercentages2[tc]==100){
				for(int d=0; d<gt.rules.nDaysPerWeek; d++){
					nAllowedSlotsPerDay[d]=0;
					for(int h=0; h<gt.rules.nHoursPerDay; h++)
						if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h])
							nAllowedSlotsPerDay[d]++;
					nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],teachersMaxHoursDailyMaxHours2[tc]);
				}

				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					dayAvailable[d]=1;
				if(teachersMaxRealDaysPerWeekMaxDays[tc]>=0){
					//max days per week has 100% weight
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						dayAvailable[d]=0;
					assert(teachersMaxRealDaysPerWeekMaxDays[tc]<=gt.rules.nDaysPerWeek/2);
					for(int k=0; k<2*teachersMaxRealDaysPerWeekMaxDays[tc]; k++){
						int maxPos=-1, maxVal=-1;
						for(int d=0; d<gt.rules.nDaysPerWeek; d++)
							if(dayAvailable[d]==0)
								if(maxVal<nAllowedSlotsPerDay[d]){
									maxVal=nAllowedSlotsPerDay[d];
									maxPos=d;
								}
						assert(maxPos>=0);
						assert(dayAvailable[maxPos]==0);
						dayAvailable[maxPos]=1;
					}
				}

				int total=0;
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					if(dayAvailable[d]==1)
						total+=nAllowedSlotsPerDay[d];
				if(total<nHoursPerTeacher[tc]){
					ok=false;

					QString s;
					s=GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint of type"
					 " max %2 hours daily with 100% weight which cannot be respected because of number of days per week,"
					 " number of hours per day, teacher max days per week, teacher not available and/or breaks."
					 " The number of total hours for this teacher is"
					 " %3 and the number of available slots is, considering max hours daily and all other constraints, %4.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(teachersMaxHoursDailyMaxHours2[tc])
					 .arg(nHoursPerTeacher[tc])
					 .arg(total);
					s+="\n\n";
					s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
		}
	}

	return ok;
}

//must be after allowed times, after n hours per teacher and after max days per week
bool computeTeachersMaxHoursDailyRealDays(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMaxHoursDailyRealDaysMaxHours1[i]=-1;
		teachersMaxHoursDailyRealDaysPercentages1[i]=-1;

		teachersMaxHoursDailyRealDaysMaxHours2[i]=-1;
		teachersMaxHoursDailyRealDaysPercentages2[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS){
			ConstraintTeacherMaxHoursDailyRealDays* tmd=(ConstraintTeacherMaxHoursDailyRealDays*)gt.rules.internalTimeConstraintsList[i];

			if(tmd->weightPercentage<100.0)
				thereAreTeachersWithMaxHoursDailyOrPerRealDayWithUnder100Weight=true;

			//////////
			/*if(tmd->weightPercentage!=100){
				ok=false;

				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max hours daily for teacher %1 with"
				 " weight (percentage) below 100. Starting with FET version 5.3.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tmd->teacherName),
				 GeneratePreTranslate::tr("Skip rest of max hours problems"), GeneratePreTranslate::tr("See next incompatibility max hours"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}*/
			//////////

			if(teachersMaxHoursDailyRealDaysMaxHours1[tmd->teacher_ID]==-1 ||
			 (teachersMaxHoursDailyRealDaysMaxHours1[tmd->teacher_ID] >= tmd->maxHoursDaily &&
			 teachersMaxHoursDailyRealDaysPercentages1[tmd->teacher_ID] <= tmd->weightPercentage)){
				teachersMaxHoursDailyRealDaysMaxHours1[tmd->teacher_ID] = tmd->maxHoursDaily;
				teachersMaxHoursDailyRealDaysPercentages1[tmd->teacher_ID] = tmd->weightPercentage;
			}
			else if(teachersMaxHoursDailyRealDaysMaxHours1[tmd->teacher_ID] <= tmd->maxHoursDaily &&
			 teachersMaxHoursDailyRealDaysPercentages1[tmd->teacher_ID] >= tmd->weightPercentage){
				//nothing
			}
			else{
				if(teachersMaxHoursDailyRealDaysMaxHours2[tmd->teacher_ID]==-1 ||
				 (teachersMaxHoursDailyRealDaysMaxHours2[tmd->teacher_ID] >= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyRealDaysPercentages2[tmd->teacher_ID] <= tmd->weightPercentage)){
					teachersMaxHoursDailyRealDaysMaxHours2[tmd->teacher_ID] = tmd->maxHoursDaily;
					teachersMaxHoursDailyRealDaysPercentages2[tmd->teacher_ID] = tmd->weightPercentage;
				}
				else if(teachersMaxHoursDailyRealDaysMaxHours2[tmd->teacher_ID] <= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyRealDaysPercentages2[tmd->teacher_ID] >= tmd->weightPercentage){
					//nothing
				}
				else{ //cannot proceed
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are too many constraints"
					 " of type max hours daily per real day relating to him, which cannot be compressed in 2 constraints of this type."
					 " Two constraints max hours can be compressed into a single one if the max hours are lower"
					 " in the first one and the weight percentage is higher on the first one."
					 " It is possible to use any number of such constraints for a teacher, but their resultant must"
					 " be maximum 2 constraints of type max hours daily per real day.\n\n"
					 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
					 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
					 " Please modify your data accordingly and try again.")
					 .arg(gt.rules.internalTeachersList[tmd->teacher_ID]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_REAL_DAYS){
			ConstraintTeachersMaxHoursDailyRealDays* tmd=(ConstraintTeachersMaxHoursDailyRealDays*)gt.rules.internalTimeConstraintsList[i];

			if(tmd->weightPercentage<100.0)
				thereAreTeachersWithMaxHoursDailyOrPerRealDayWithUnder100Weight=true;

			//////////
			/*if(tmd->weightPercentage!=100){
				ok=false;

				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max hours daily with"
				 " weight (percentage) below 100. Starting with FET version 5.3.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest of max hours problems"), GeneratePreTranslate::tr("See next incompatibility max hours"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}*/
			//////////

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMaxHoursDailyRealDaysMaxHours1[tch]==-1 ||
				 (teachersMaxHoursDailyRealDaysMaxHours1[tch] >= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyRealDaysPercentages1[tch] <= tmd->weightPercentage)){
					teachersMaxHoursDailyRealDaysMaxHours1[tch] = tmd->maxHoursDaily;
					teachersMaxHoursDailyRealDaysPercentages1[tch] = tmd->weightPercentage;
					}
				else if(teachersMaxHoursDailyRealDaysMaxHours1[tch] <= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyRealDaysPercentages1[tch] >= tmd->weightPercentage){
					//nothing
				}
				else{
					if(teachersMaxHoursDailyRealDaysMaxHours2[tch]==-1 ||
					 (teachersMaxHoursDailyRealDaysMaxHours2[tch] >= tmd->maxHoursDaily &&
					 teachersMaxHoursDailyRealDaysPercentages2[tch] <= tmd->weightPercentage)){
						teachersMaxHoursDailyRealDaysMaxHours2[tch] = tmd->maxHoursDaily;
						teachersMaxHoursDailyRealDaysPercentages2[tch] = tmd->weightPercentage;
						}
					else if(teachersMaxHoursDailyRealDaysMaxHours2[tch] <= tmd->maxHoursDaily &&
					 teachersMaxHoursDailyRealDaysPercentages2[tch] >= tmd->weightPercentage){
					//nothing
					}
					else{ //cannot proceed
						ok=false;

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are too many constraints"
						 " of type max hours daily per real day relating to him, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a teacher, but their resultant must"
						 " be maximum 2 constraints of type max hours daily per real day.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again.")
						 .arg(gt.rules.internalTeachersList[tch]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}
				}
			}
		}
	}

	if(gt.rules.mode==MORNINGS_AFTERNOONS)
		assert(gt.rules.nDaysPerWeek%2==0); //this is taken care of previously - the generation cannot begin otherwise

	Matrix1D<int> nAllowedSlotsPerDay;
	nAllowedSlotsPerDay.resize(gt.rules.nDaysPerWeek/2);
	Matrix1D<int> dayAvailable;
	dayAvailable.resize(gt.rules.nDaysPerWeek/2);

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMaxHoursDailyRealDaysPercentages1[tc]==100){
			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++){
					if(!breakDayHour[2*d][h] && !teacherNotAvailableDayHour[tc][2*d][h])
						nAllowedSlotsPerDay[d]++;
					if(!breakDayHour[2*d+1][h] && !teacherNotAvailableDayHour[tc][2*d+1][h])
						nAllowedSlotsPerDay[d]++;
				}
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],teachersMaxHoursDailyRealDaysMaxHours1[tc]);
			}

			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
				dayAvailable[d]=1;
			if(teachersMaxRealDaysPerWeekMaxDays[tc]>=0){
				//max days per week has 100% weight
				for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
					dayAvailable[d]=0;
				assert(teachersMaxRealDaysPerWeekMaxDays[tc]<=gt.rules.nDaysPerWeek/2);
				for(int k=0; k<teachersMaxRealDaysPerWeekMaxDays[tc]; k++){
					int maxPos=-1, maxVal=-1;
					for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
						if(dayAvailable[d]==0)
							if(maxVal<nAllowedSlotsPerDay[d]){
								maxVal=nAllowedSlotsPerDay[d];
								maxPos=d;
							}
					assert(maxPos>=0);
					assert(dayAvailable[maxPos]==0);
					dayAvailable[maxPos]=1;
				}
			}

			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
				if(dayAvailable[d]==1)
					total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerTeacher[tc]){
				ok=false;

				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint of type"
				 " max %2 hours daily per real day with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, teacher(s) max real days per week, teacher not available and/or breaks."
				 " The number of total hours for this teacher is"
				 " %3 and the number of available slots is, considering max hours daily per real day and all other constraints, %4.")
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(teachersMaxHoursDailyRealDaysMaxHours1[tc])
				 .arg(nHoursPerTeacher[tc])
				 .arg(total);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
	}

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMaxHoursDailyRealDaysPercentages2[tc]==100){
			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++){
					if(!breakDayHour[2*d][h] && !teacherNotAvailableDayHour[tc][2*d][h])
						nAllowedSlotsPerDay[d]++;
					if(!breakDayHour[2*d+1][h] && !teacherNotAvailableDayHour[tc][2*d+1][h])
						nAllowedSlotsPerDay[d]++;
				}
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],teachersMaxHoursDailyRealDaysMaxHours1[tc]);
			}

			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
				dayAvailable[d]=1;
			if(teachersMaxRealDaysPerWeekMaxDays[tc]>=0){
				//max days per week has 100% weight
				for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
					dayAvailable[d]=0;
				assert(teachersMaxRealDaysPerWeekMaxDays[tc]<=gt.rules.nDaysPerWeek/2);
				for(int k=0; k<teachersMaxRealDaysPerWeekMaxDays[tc]; k++){
					int maxPos=-1, maxVal=-1;
					for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
						if(dayAvailable[d]==0)
							if(maxVal<nAllowedSlotsPerDay[d]){
								maxVal=nAllowedSlotsPerDay[d];
								maxPos=d;
							}
					assert(maxPos>=0);
					assert(dayAvailable[maxPos]==0);
					dayAvailable[maxPos]=1;
				}
			}

			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
				if(dayAvailable[d]==1)
					total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerTeacher[tc]){
				ok=false;

				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint of type"
				 " max %2 hours daily per real day with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, teacher(s) max real days per week, teacher not available and/or breaks."
				 " The number of total hours for this teacher is"
				 " %3 and the number of available slots is, considering max hours daily per real day and all other constraints, %4.")
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(teachersMaxHoursDailyRealDaysMaxHours2[tc])
				 .arg(nHoursPerTeacher[tc])
				 .arg(total);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
	}

	return ok;
}

//must be after allowed times, after n hours per teacher and after max days per week for teachers
bool computeTeachersMaxSpanPerDay(QWidget* parent)
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMaxSpanPerDayMaxSpan[i]=-1;
		teachersMaxSpanPerDayPercentages[i]=-1;
		teachersMaxSpanPerDayAllowOneDayExceptionPlusOne[i]=true;
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_SPAN_PER_DAY){
			ConstraintTeacherMaxSpanPerDay* tmsd=(ConstraintTeacherMaxSpanPerDay*)gt.rules.internalTimeConstraintsList[i];

			if(tmsd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max span per day for teacher %1"
				 " with weight (percentage) below 100. Please make weight 100% and try again").arg(tmsd->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_SPAN_PER_DAY){
			ConstraintTeachersMaxSpanPerDay* tmsd=(ConstraintTeachersMaxSpanPerDay*)gt.rules.internalTimeConstraintsList[i];

			if(tmsd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max span per day"
				 " with weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_SPAN_PER_DAY){
			ConstraintTeacherMaxSpanPerDay* tmsd=(ConstraintTeacherMaxSpanPerDay*)gt.rules.internalTimeConstraintsList[i];
			
			if(teachersMaxSpanPerDayPercentages[tmsd->teacher_ID]==-1){
				teachersMaxSpanPerDayPercentages[tmsd->teacher_ID]=100.0;
				teachersMaxSpanPerDayMaxSpan[tmsd->teacher_ID]=tmsd->maxSpanPerDay;
				teachersMaxSpanPerDayAllowOneDayExceptionPlusOne[tmsd->teacher_ID]=tmsd->allowOneDayExceptionPlusOne;
			}
			else if(teachersMaxSpanPerDayMaxSpan[tmsd->teacher_ID] > tmsd->maxSpanPerDay){
				assert(teachersMaxSpanPerDayPercentages[tmsd->teacher_ID]==100.0);
				teachersMaxSpanPerDayMaxSpan[tmsd->teacher_ID]=tmsd->maxSpanPerDay;
				teachersMaxSpanPerDayAllowOneDayExceptionPlusOne[tmsd->teacher_ID]=tmsd->allowOneDayExceptionPlusOne;
			}
			else if(teachersMaxSpanPerDayMaxSpan[tmsd->teacher_ID]==tmsd->maxSpanPerDay){
				assert(teachersMaxSpanPerDayPercentages[tmsd->teacher_ID]==100.0);
				assert(teachersMaxSpanPerDayMaxSpan[tmsd->teacher_ID]==tmsd->maxSpanPerDay);
				if(!tmsd->allowOneDayExceptionPlusOne && teachersMaxSpanPerDayAllowOneDayExceptionPlusOne[tmsd->teacher_ID])
					teachersMaxSpanPerDayAllowOneDayExceptionPlusOne[tmsd->teacher_ID]=false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_SPAN_PER_DAY){
			ConstraintTeachersMaxSpanPerDay* tmsd=(ConstraintTeachersMaxSpanPerDay*)gt.rules.internalTimeConstraintsList[i];
			
			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMaxSpanPerDayPercentages[tch]==-1){
					teachersMaxSpanPerDayPercentages[tch]=100.0;
					teachersMaxSpanPerDayMaxSpan[tch]=tmsd->maxSpanPerDay;
					teachersMaxSpanPerDayAllowOneDayExceptionPlusOne[tch]=tmsd->allowOneDayExceptionPlusOne;
				}
				else if(teachersMaxSpanPerDayMaxSpan[tch] > tmsd->maxSpanPerDay){
					assert(teachersMaxSpanPerDayPercentages[tch]==100.0);
					teachersMaxSpanPerDayMaxSpan[tch]=tmsd->maxSpanPerDay;
					teachersMaxSpanPerDayAllowOneDayExceptionPlusOne[tch]=tmsd->allowOneDayExceptionPlusOne;
				}
				else if(teachersMaxSpanPerDayMaxSpan[tch]==tmsd->maxSpanPerDay){
					assert(teachersMaxSpanPerDayPercentages[tch]==100.0);
					assert(teachersMaxSpanPerDayMaxSpan[tch]==tmsd->maxSpanPerDay);
					if(!tmsd->allowOneDayExceptionPlusOne && teachersMaxSpanPerDayAllowOneDayExceptionPlusOne[tch])
						teachersMaxSpanPerDayAllowOneDayExceptionPlusOne[tch]=false;
				}
			}
		}
	}
	
	Matrix1D<int> nAllowedSlotsPerDay;
	nAllowedSlotsPerDay.resize(gt.rules.nDaysPerWeek);
	
	Matrix1D<int> dayAvailable;
	dayAvailable.resize(gt.rules.nDaysPerWeek);

	//This is similar to teachers max hours daily checking. It is not a very useful test, but does not hurt.
	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		bool exceptionUsed=false;
		if(!teachersMaxSpanPerDayAllowOneDayExceptionPlusOne[tc])
			exceptionUsed=true;
		
		if(teachersMaxSpanPerDayPercentages[tc]==100){
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h])
						nAllowedSlotsPerDay[d]++;
				
				if(exceptionUsed==false && nAllowedSlotsPerDay[d]>=teachersMaxSpanPerDayMaxSpan[tc]+1){
					nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],teachersMaxSpanPerDayMaxSpan[tc]+1);
					exceptionUsed=true;
				}
				else{
					nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],teachersMaxSpanPerDayMaxSpan[tc]);
				}
			}
			
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				dayAvailable[d]=1;
			if(teachersMaxDaysPerWeekMaxDays[tc]>=0){
				//max days per week has 100% weight
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					dayAvailable[d]=0;
				assert(teachersMaxDaysPerWeekMaxDays[tc]<=gt.rules.nDaysPerWeek);
				for(int k=0; k<teachersMaxDaysPerWeekMaxDays[tc]; k++){
					int maxPos=-1, maxVal=-1;
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						if(dayAvailable[d]==0)
							if(maxVal<nAllowedSlotsPerDay[d]){
								maxVal=nAllowedSlotsPerDay[d];
								maxPos=d;
							}
					assert(maxPos>=0);
					assert(dayAvailable[maxPos]==0);
					dayAvailable[maxPos]=1;
				}
			}
			
			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				if(dayAvailable[d]==1)
					total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerTeacher[tc]){
				ok=false;
				
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint of type"
				 " max %2 span per day with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, teacher max days per week, teacher not available and/or breaks."
				 " The number of total hours for this teacher is"
				 " %3 and the number of available slots is, considering max span per day and all other constraints, %4.")
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(teachersMaxSpanPerDayMaxSpan[tc])
				 .arg(nHoursPerTeacher[tc])
				 .arg(total);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}

	return ok;
}

//must be after allowed times, after n hours per teacher and after max days per week for teachers
bool computeTeachersMaxSpanPerRealDay(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMaxSpanPerRealDayMaxSpan[i]=-1;
		teachersMaxSpanPerRealDayPercentages[i]=-1;
		teachersMaxSpanPerRealDayAllowOneDayExceptionPlusOne[i]=true;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_SPAN_PER_REAL_DAY){
			ConstraintTeacherMaxSpanPerRealDay* tmsd=(ConstraintTeacherMaxSpanPerRealDay*)gt.rules.internalTimeConstraintsList[i];

			if(tmsd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max span per real day for teacher %1"
				 " with weight (percentage) below 100. Please make weight 100% and try again").arg(tmsd->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_SPAN_PER_REAL_DAY){
			ConstraintTeachersMaxSpanPerRealDay* tmsd=(ConstraintTeachersMaxSpanPerRealDay*)gt.rules.internalTimeConstraintsList[i];

			if(tmsd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max span per real day"
				 " with weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_SPAN_PER_REAL_DAY){
			ConstraintTeacherMaxSpanPerRealDay* tmsd=(ConstraintTeacherMaxSpanPerRealDay*)gt.rules.internalTimeConstraintsList[i];

			if(teachersMaxSpanPerRealDayPercentages[tmsd->teacher_ID]==-1){
				teachersMaxSpanPerRealDayPercentages[tmsd->teacher_ID]=100.0;
				teachersMaxSpanPerRealDayMaxSpan[tmsd->teacher_ID]=tmsd->maxSpanPerDay;
				teachersMaxSpanPerRealDayAllowOneDayExceptionPlusOne[tmsd->teacher_ID]=tmsd->allowOneDayExceptionPlusOne;
			}
			else if(teachersMaxSpanPerRealDayMaxSpan[tmsd->teacher_ID] > tmsd->maxSpanPerDay){
				assert(teachersMaxSpanPerRealDayPercentages[tmsd->teacher_ID]==100.0);
				teachersMaxSpanPerRealDayMaxSpan[tmsd->teacher_ID]=tmsd->maxSpanPerDay;
				teachersMaxSpanPerRealDayAllowOneDayExceptionPlusOne[tmsd->teacher_ID]=tmsd->allowOneDayExceptionPlusOne;
			}
			else if(teachersMaxSpanPerRealDayMaxSpan[tmsd->teacher_ID]==tmsd->maxSpanPerDay){
				assert(teachersMaxSpanPerRealDayPercentages[tmsd->teacher_ID]==100.0);
				assert(teachersMaxSpanPerRealDayMaxSpan[tmsd->teacher_ID]==tmsd->maxSpanPerDay);
				if(!tmsd->allowOneDayExceptionPlusOne && teachersMaxSpanPerRealDayAllowOneDayExceptionPlusOne[tmsd->teacher_ID])
					teachersMaxSpanPerRealDayAllowOneDayExceptionPlusOne[tmsd->teacher_ID]=false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_SPAN_PER_REAL_DAY){
			ConstraintTeachersMaxSpanPerRealDay* tmsd=(ConstraintTeachersMaxSpanPerRealDay*)gt.rules.internalTimeConstraintsList[i];

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMaxSpanPerRealDayPercentages[tch]==-1){
					teachersMaxSpanPerRealDayPercentages[tch]=100.0;
					teachersMaxSpanPerRealDayMaxSpan[tch]=tmsd->maxSpanPerDay;
					teachersMaxSpanPerRealDayAllowOneDayExceptionPlusOne[tch]=tmsd->allowOneDayExceptionPlusOne;
				}
				else if(teachersMaxSpanPerRealDayMaxSpan[tch] > tmsd->maxSpanPerDay){
					assert(teachersMaxSpanPerRealDayPercentages[tch]==100.0);
					teachersMaxSpanPerRealDayMaxSpan[tch]=tmsd->maxSpanPerDay;
					teachersMaxSpanPerRealDayAllowOneDayExceptionPlusOne[tch]=tmsd->allowOneDayExceptionPlusOne;
				}
				else if(teachersMaxSpanPerRealDayMaxSpan[tch]==tmsd->maxSpanPerDay){
					assert(teachersMaxSpanPerRealDayPercentages[tch]==100.0);
					assert(teachersMaxSpanPerRealDayMaxSpan[tch]==tmsd->maxSpanPerDay);
					if(!tmsd->allowOneDayExceptionPlusOne && teachersMaxSpanPerRealDayAllowOneDayExceptionPlusOne[tch])
						teachersMaxSpanPerRealDayAllowOneDayExceptionPlusOne[tch]=false;
				}
			}
		}
	}

	if(gt.rules.mode==MORNINGS_AFTERNOONS)
		assert(gt.rules.nDaysPerWeek%2==0); //this is taken care of previously - the generation cannot begin otherwise

	//This is similar to teachers max hours daily checking. It is not a very useful test, but does not hurt.
	Matrix1D<int> nAllowedSlotsPerDay;
	nAllowedSlotsPerDay.resize(gt.rules.nDaysPerWeek/2);
	Matrix1D<int> dayAvailable;
	dayAvailable.resize(gt.rules.nDaysPerWeek/2);

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		bool exceptionUsed=false;
		if(!teachersMaxSpanPerRealDayAllowOneDayExceptionPlusOne[tc])
			exceptionUsed=true;

		if(teachersMaxSpanPerRealDayPercentages[tc]==100){
			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++){
					if(!breakDayHour[2*d][h] && !teacherNotAvailableDayHour[tc][2*d][h])
						nAllowedSlotsPerDay[d]++;
					if(!breakDayHour[2*d+1][h] && !teacherNotAvailableDayHour[tc][2*d+1][h])
						nAllowedSlotsPerDay[d]++;
				}

				if(exceptionUsed==false && nAllowedSlotsPerDay[d]>=teachersMaxSpanPerRealDayMaxSpan[tc]+1){
					nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],teachersMaxSpanPerRealDayMaxSpan[tc]+1);
					exceptionUsed=true;
				}
				else{
					nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],teachersMaxSpanPerRealDayMaxSpan[tc]);
				}
			}

			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
				dayAvailable[d]=1;
			if(teachersMaxRealDaysPerWeekMaxDays[tc]>=0){
				//max days per week has 100% weight
				for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
					dayAvailable[d]=0;
				assert(teachersMaxRealDaysPerWeekMaxDays[tc]<=gt.rules.nDaysPerWeek/2);
				for(int k=0; k<teachersMaxRealDaysPerWeekMaxDays[tc]; k++){
					int maxPos=-1, maxVal=-1;
					for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
						if(dayAvailable[d]==0)
							if(maxVal<nAllowedSlotsPerDay[d]){
								maxVal=nAllowedSlotsPerDay[d];
								maxPos=d;
							}
					assert(maxPos>=0);
					assert(dayAvailable[maxPos]==0);
					dayAvailable[maxPos]=1;
				}
			}

			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
				if(dayAvailable[d]==1)
					total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerTeacher[tc]){
				ok=false;

				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint of type"
				 " max %2 span per real day with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, teacher(s) max real days per week, teacher not available and/or breaks."
				 " The number of total hours for this teacher is"
				 " %3 and the number of available slots is, considering max span per real day and all other constraints, %4.")
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(teachersMaxSpanPerRealDayMaxSpan[tc])
				 .arg(nHoursPerTeacher[tc])
				 .arg(total);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
	}

	return ok;
}

bool computeTeachersMaxHoursContinuously(QWidget* parent)
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMaxHoursContinuouslyMaxHours1[i]=-1;
		teachersMaxHoursContinuouslyPercentages1[i]=-1;

		teachersMaxHoursContinuouslyMaxHours2[i]=-1;
		teachersMaxHoursContinuouslyPercentages2[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY){
			ConstraintTeacherMaxHoursContinuously* tmd=(ConstraintTeacherMaxHoursContinuously*)gt.rules.internalTimeConstraintsList[i];

			if(teachersMaxHoursContinuouslyMaxHours1[tmd->teacher_ID]==-1 ||
			 (teachersMaxHoursContinuouslyMaxHours1[tmd->teacher_ID] >= tmd->maxHoursContinuously &&
			 teachersMaxHoursContinuouslyPercentages1[tmd->teacher_ID] <= tmd->weightPercentage)){
			 	teachersMaxHoursContinuouslyMaxHours1[tmd->teacher_ID] = tmd->maxHoursContinuously;
				teachersMaxHoursContinuouslyPercentages1[tmd->teacher_ID] = tmd->weightPercentage;
			}
			else if(teachersMaxHoursContinuouslyMaxHours1[tmd->teacher_ID] <= tmd->maxHoursContinuously &&
			 teachersMaxHoursContinuouslyPercentages1[tmd->teacher_ID] >= tmd->weightPercentage){
			 	//nothing
			}
			else{
				if(teachersMaxHoursContinuouslyMaxHours2[tmd->teacher_ID]==-1 ||
				 (teachersMaxHoursContinuouslyMaxHours2[tmd->teacher_ID] >= tmd->maxHoursContinuously &&
				 teachersMaxHoursContinuouslyPercentages2[tmd->teacher_ID] <= tmd->weightPercentage)){
				 	teachersMaxHoursContinuouslyMaxHours2[tmd->teacher_ID] = tmd->maxHoursContinuously;
					teachersMaxHoursContinuouslyPercentages2[tmd->teacher_ID] = tmd->weightPercentage;
				}
				else if(teachersMaxHoursContinuouslyMaxHours2[tmd->teacher_ID] <= tmd->maxHoursContinuously &&
				 teachersMaxHoursContinuouslyPercentages2[tmd->teacher_ID] >= tmd->weightPercentage){
				 	//nothing
				}
				else{ //cannot proceed
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are too many constraints"
					 " of type max hours continuously relating to him, which cannot be compressed in 2 constraints of this type."
					 " Two constraints max hours can be compressed into a single one if the max hours are lower"
					 " in the first one and the weight percentage is higher on the first one."
					 " It is possible to use any number of such constraints for a teacher, but their resultant must"
					 " be maximum 2 constraints of type max hours continuously.\n\n"
					 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
					 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
					 " Please modify your data accordingly and try again.")
					 .arg(gt.rules.internalTeachersList[tmd->teacher_ID]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY){
			ConstraintTeachersMaxHoursContinuously* tmd=(ConstraintTeachersMaxHoursContinuously*)gt.rules.internalTimeConstraintsList[i];

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMaxHoursContinuouslyMaxHours1[tch]==-1 ||
				 (teachersMaxHoursContinuouslyMaxHours1[tch] >= tmd->maxHoursContinuously &&
				 teachersMaxHoursContinuouslyPercentages1[tch] <= tmd->weightPercentage)){
				 	teachersMaxHoursContinuouslyMaxHours1[tch] = tmd->maxHoursContinuously;
					teachersMaxHoursContinuouslyPercentages1[tch] = tmd->weightPercentage;
					}
				else if(teachersMaxHoursContinuouslyMaxHours1[tch] <= tmd->maxHoursContinuously &&
				 teachersMaxHoursContinuouslyPercentages1[tch] >= tmd->weightPercentage){
				 	//nothing
				}
				else{
					if(teachersMaxHoursContinuouslyMaxHours2[tch]==-1 ||
					 (teachersMaxHoursContinuouslyMaxHours2[tch] >= tmd->maxHoursContinuously &&
					 teachersMaxHoursContinuouslyPercentages2[tch] <= tmd->weightPercentage)){
					 	teachersMaxHoursContinuouslyMaxHours2[tch] = tmd->maxHoursContinuously;
						teachersMaxHoursContinuouslyPercentages2[tch] = tmd->weightPercentage;
						}
					else if(teachersMaxHoursContinuouslyMaxHours2[tch] <= tmd->maxHoursContinuously &&
					 teachersMaxHoursContinuouslyPercentages2[tch] >= tmd->weightPercentage){
				 	//nothing
					}
					else{ //cannot proceed
						ok=false;

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are too many constraints"
						 " of type max hours continuously relating to him, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a teacher, but their resultant must"
						 " be maximum 2 constraints of type max hours continuously.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again.")
						 .arg(gt.rules.internalTeachersList[tch]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}
				}
			}
		}
	}
	
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		for(int tch : qAsConst(gt.rules.internalActivitiesList[ai].iTeachersList)){
			if(teachersMaxHoursContinuouslyPercentages1[tch]>=0 && gt.rules.internalActivitiesList[ai].duration > teachersMaxHoursContinuouslyMaxHours1[tch]){
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint of type"
				 " max %2 hours continuously which cannot be respected because of activity with id %3 (which has duration %4).")
				 .arg(gt.rules.internalTeachersList[tch]->name)
				 .arg(teachersMaxHoursContinuouslyMaxHours1[tch])
				 .arg(gt.rules.internalActivitiesList[ai].id)
				 .arg(gt.rules.internalActivitiesList[ai].duration);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			if(teachersMaxHoursContinuouslyPercentages2[tch]>=0 && gt.rules.internalActivitiesList[ai].duration > teachersMaxHoursContinuouslyMaxHours2[tch]){
				QString s;
				s=GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint of type"
				 " max %2 hours continuously which cannot be respected because of activity with id %3 (which has duration %4).")
				 .arg(gt.rules.internalTeachersList[tch]->name)
				 .arg(teachersMaxHoursContinuouslyMaxHours2[tch])
				 .arg(gt.rules.internalActivitiesList[ai].id)
				 .arg(gt.rules.internalActivitiesList[ai].duration);
				s+="\n\n";
				s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}
	
	return ok;
}

bool computeTeachersActivityTagMaxHoursDaily(QWidget* parent)
{
	haveTeachersActivityTagMaxHoursDaily=false;
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersActivityTagMaxHoursDailyMaxHours[i].clear();
		teachersActivityTagMaxHoursDailyPercentage[i].clear();
		teachersActivityTagMaxHoursDailyActivityTag[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY){
			haveTeachersActivityTagMaxHoursDaily=true;

			ConstraintTeachersActivityTagMaxHoursDaily* tamd=(ConstraintTeachersActivityTagMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			for(int tc : qAsConst(tamd->canonicalTeachersList)){
				int pos1=-1, pos2=-1;
				
				for(int j=0; j<teachersActivityTagMaxHoursDailyMaxHours[tc].count(); j++){
					if(teachersActivityTagMaxHoursDailyActivityTag[tc].at(j)==tamd->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}
				
				if(pos1==-1){
					teachersActivityTagMaxHoursDailyActivityTag[tc].append(tamd->activityTagIndex);
					teachersActivityTagMaxHoursDailyMaxHours[tc].append(tamd->maxHoursDaily);
					teachersActivityTagMaxHoursDailyPercentage[tc].append(tamd->weightPercentage);
				}
				else{
					if(teachersActivityTagMaxHoursDailyMaxHours[tc].at(pos1) <= tamd->maxHoursDaily
					 && teachersActivityTagMaxHoursDailyPercentage[tc].at(pos1) >= tamd->weightPercentage){
					 	//do nothing
					}
					else if(teachersActivityTagMaxHoursDailyMaxHours[tc].at(pos1) >= tamd->maxHoursDaily
					 && teachersActivityTagMaxHoursDailyPercentage[tc].at(pos1) <= tamd->weightPercentage){
					
						teachersActivityTagMaxHoursDailyActivityTag[tc][pos1]=tamd->activityTagIndex;
						teachersActivityTagMaxHoursDailyMaxHours[tc][pos1]=tamd->maxHoursDaily;
						teachersActivityTagMaxHoursDailyPercentage[tc][pos1]=tamd->weightPercentage;
					}
					else{
						if(pos2==-1){
							teachersActivityTagMaxHoursDailyActivityTag[tc].append(tamd->activityTagIndex);
							teachersActivityTagMaxHoursDailyMaxHours[tc].append(tamd->maxHoursDaily);
							teachersActivityTagMaxHoursDailyPercentage[tc].append(tamd->weightPercentage);
						}
						else{

							if(teachersActivityTagMaxHoursDailyMaxHours[tc].at(pos2) <= tamd->maxHoursDaily
							 && teachersActivityTagMaxHoursDailyPercentage[tc].at(pos2) >= tamd->weightPercentage){
							 	//do nothing
							}
							else if(teachersActivityTagMaxHoursDailyMaxHours[tc].at(pos2) >= tamd->maxHoursDaily
							 && teachersActivityTagMaxHoursDailyPercentage[tc].at(pos2) <= tamd->weightPercentage){
							
								teachersActivityTagMaxHoursDailyActivityTag[tc][pos2]=tamd->activityTagIndex;
								teachersActivityTagMaxHoursDailyMaxHours[tc][pos2]=tamd->maxHoursDaily;
								teachersActivityTagMaxHoursDailyPercentage[tc][pos2]=tamd->weightPercentage;
							}
							else{
								ok=false;
	
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for teacher %1 and activity tag %2, because there are too many constraints"
								 " of type activity tag max hours daily relating to them, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a teacher and an activity tag, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours daily.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalTeachersList[tc]->name)
								 .arg(tamd->activityTagName),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY){
			haveTeachersActivityTagMaxHoursDaily=true;

			ConstraintTeacherActivityTagMaxHoursDaily* tamd=(ConstraintTeacherActivityTagMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			for(int tc : qAsConst(tamd->canonicalTeachersList)){
				int pos1=-1, pos2=-1;
				
				for(int j=0; j<teachersActivityTagMaxHoursDailyMaxHours[tc].count(); j++){
					if(teachersActivityTagMaxHoursDailyActivityTag[tc].at(j)==tamd->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}
				
				if(pos1==-1){
					teachersActivityTagMaxHoursDailyActivityTag[tc].append(tamd->activityTagIndex);
					teachersActivityTagMaxHoursDailyMaxHours[tc].append(tamd->maxHoursDaily);
					teachersActivityTagMaxHoursDailyPercentage[tc].append(tamd->weightPercentage);
				}
				else{
					if(teachersActivityTagMaxHoursDailyMaxHours[tc].at(pos1) <= tamd->maxHoursDaily
					 && teachersActivityTagMaxHoursDailyPercentage[tc].at(pos1) >= tamd->weightPercentage){
					 	//do nothing
					}
					else if(teachersActivityTagMaxHoursDailyMaxHours[tc].at(pos1) >= tamd->maxHoursDaily
					 && teachersActivityTagMaxHoursDailyPercentage[tc].at(pos1) <= tamd->weightPercentage){
					
						teachersActivityTagMaxHoursDailyActivityTag[tc][pos1]=tamd->activityTagIndex;
						teachersActivityTagMaxHoursDailyMaxHours[tc][pos1]=tamd->maxHoursDaily;
						teachersActivityTagMaxHoursDailyPercentage[tc][pos1]=tamd->weightPercentage;
					}
					else{
						if(pos2==-1){
							teachersActivityTagMaxHoursDailyActivityTag[tc].append(tamd->activityTagIndex);
							teachersActivityTagMaxHoursDailyMaxHours[tc].append(tamd->maxHoursDaily);
							teachersActivityTagMaxHoursDailyPercentage[tc].append(tamd->weightPercentage);
						}
						else{

							if(teachersActivityTagMaxHoursDailyMaxHours[tc].at(pos2) <= tamd->maxHoursDaily
							 && teachersActivityTagMaxHoursDailyPercentage[tc].at(pos2) >= tamd->weightPercentage){
							 	//do nothing
							}
							else if(teachersActivityTagMaxHoursDailyMaxHours[tc].at(pos2) >= tamd->maxHoursDaily
							 && teachersActivityTagMaxHoursDailyPercentage[tc].at(pos2) <= tamd->weightPercentage){
							
								teachersActivityTagMaxHoursDailyActivityTag[tc][pos2]=tamd->activityTagIndex;
								teachersActivityTagMaxHoursDailyMaxHours[tc][pos2]=tamd->maxHoursDaily;
								teachersActivityTagMaxHoursDailyPercentage[tc][pos2]=tamd->weightPercentage;
							}
							else{
								ok=false;
	
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for teacher %1 and activity tag %2, because there are too many constraints"
								 " of type activity tag max hours daily relating to them, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a teacher and an activity tag, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours daily.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalTeachersList[tc]->name)
								 .arg(tamd->activityTagName),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
	}

	Matrix1D<int> navd;
	navd.resize(gt.rules.nDaysPerWeek);
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			navd[d]=0;
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[i][d][h])
					navd[d]++;
			}
		}
	
		for(int j=0; j<teachersActivityTagMaxHoursDailyMaxHours[i].count(); j++){
			int mh=teachersActivityTagMaxHoursDailyMaxHours[i].at(j);
			double perc=teachersActivityTagMaxHoursDailyPercentage[i].at(j);
			int at=teachersActivityTagMaxHoursDailyActivityTag[i].at(j);
			if(perc==100.0){
				int totalAt=0;
				for(int ai : qAsConst(gt.rules.internalTeachersList[i]->activitiesForTeacher))
					if(gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(at))
						totalAt+=gt.rules.internalActivitiesList[ai].duration;
						
				int ava=0;
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					ava+=min(navd[d], mh);
					
				if(ava<totalAt){
					ok=false;
					
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint activity tag %2 max %3 hours daily for it with weight 100%"
					 " which cannot be satisfied, considering the number of available slots (%4) and total duration of activities with this activity tag (%5)"
					 ". Please correct and try again.", "%2 is the activity tag for this constraint, %3 is the max number of hours daily for this constraint")
					 .arg(gt.rules.internalTeachersList[i]->name).arg(gt.rules.activityTagsList.at(at)->name).arg(mh).arg(ava).arg(totalAt),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
		}
	}
	
	return ok;
}


bool computeTeachersActivityTagMaxHoursDailyRealDays(QWidget* parent)
{
	haveTeachersActivityTagMaxHoursDailyRealDays=false;

	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersActivityTagMaxHoursDailyRealDaysMaxHours[i].clear();
		teachersActivityTagMaxHoursDailyRealDaysPercentage[i].clear();
		teachersActivityTagMaxHoursDailyRealDaysActivityTag[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS){
			haveTeachersActivityTagMaxHoursDailyRealDays=true;

			ConstraintTeachersActivityTagMaxHoursDailyRealDays* tamd=(ConstraintTeachersActivityTagMaxHoursDailyRealDays*)gt.rules.internalTimeConstraintsList[i];

			for(int tc : qAsConst(tamd->canonicalTeachersList)){
				int pos1=-1, pos2=-1;

				for(int j=0; j<teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc].count(); j++){
					if(teachersActivityTagMaxHoursDailyRealDaysActivityTag[tc].at(j)==tamd->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}

				if(pos1==-1){
					teachersActivityTagMaxHoursDailyRealDaysActivityTag[tc].append(tamd->activityTagIndex);
					teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc].append(tamd->maxHoursDaily);
					teachersActivityTagMaxHoursDailyRealDaysPercentage[tc].append(tamd->weightPercentage);
				}
				else{
					if(teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc].at(pos1) <= tamd->maxHoursDaily
					 && teachersActivityTagMaxHoursDailyRealDaysPercentage[tc].at(pos1) >= tamd->weightPercentage){
						//do nothing
					}
					else if(teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc].at(pos1) >= tamd->maxHoursDaily
					 && teachersActivityTagMaxHoursDailyRealDaysPercentage[tc].at(pos1) <= tamd->weightPercentage){

						teachersActivityTagMaxHoursDailyRealDaysActivityTag[tc][pos1]=tamd->activityTagIndex;
						teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc][pos1]=tamd->maxHoursDaily;
						teachersActivityTagMaxHoursDailyRealDaysPercentage[tc][pos1]=tamd->weightPercentage;
					}
					else{
						if(pos2==-1){
							teachersActivityTagMaxHoursDailyRealDaysActivityTag[tc].append(tamd->activityTagIndex);
							teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc].append(tamd->maxHoursDaily);
							teachersActivityTagMaxHoursDailyRealDaysPercentage[tc].append(tamd->weightPercentage);
						}
						else{

							if(teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc].at(pos2) <= tamd->maxHoursDaily
							 && teachersActivityTagMaxHoursDailyRealDaysPercentage[tc].at(pos2) >= tamd->weightPercentage){
								//do nothing
							}
							else if(teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc].at(pos2) >= tamd->maxHoursDaily
							 && teachersActivityTagMaxHoursDailyRealDaysPercentage[tc].at(pos2) <= tamd->weightPercentage){

								teachersActivityTagMaxHoursDailyRealDaysActivityTag[tc][pos2]=tamd->activityTagIndex;
								teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc][pos2]=tamd->maxHoursDaily;
								teachersActivityTagMaxHoursDailyRealDaysPercentage[tc][pos2]=tamd->weightPercentage;
							}
							else{
								ok=false;

								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for teacher %1 and activity tag %2, because there are too many constraints"
								 " of type activity tag max hours daily per real day relating to them, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a teacher and an activity tag, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours daily per real day.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalTeachersList[tc]->name)
								 .arg(tamd->activityTagName),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS){
			haveTeachersActivityTagMaxHoursDailyRealDays=true;

			ConstraintTeacherActivityTagMaxHoursDailyRealDays* tamd=(ConstraintTeacherActivityTagMaxHoursDailyRealDays*)gt.rules.internalTimeConstraintsList[i];

			for(int tc : qAsConst(tamd->canonicalTeachersList)){
				int pos1=-1, pos2=-1;

				for(int j=0; j<teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc].count(); j++){
					if(teachersActivityTagMaxHoursDailyRealDaysActivityTag[tc].at(j)==tamd->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}

				if(pos1==-1){
					teachersActivityTagMaxHoursDailyRealDaysActivityTag[tc].append(tamd->activityTagIndex);
					teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc].append(tamd->maxHoursDaily);
					teachersActivityTagMaxHoursDailyRealDaysPercentage[tc].append(tamd->weightPercentage);
				}
				else{
					if(teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc].at(pos1) <= tamd->maxHoursDaily
					 && teachersActivityTagMaxHoursDailyRealDaysPercentage[tc].at(pos1) >= tamd->weightPercentage){
						//do nothing
					}
					else if(teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc].at(pos1) >= tamd->maxHoursDaily
					 && teachersActivityTagMaxHoursDailyRealDaysPercentage[tc].at(pos1) <= tamd->weightPercentage){

						teachersActivityTagMaxHoursDailyRealDaysActivityTag[tc][pos1]=tamd->activityTagIndex;
						teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc][pos1]=tamd->maxHoursDaily;
						teachersActivityTagMaxHoursDailyRealDaysPercentage[tc][pos1]=tamd->weightPercentage;
					}
					else{
						if(pos2==-1){
							teachersActivityTagMaxHoursDailyRealDaysActivityTag[tc].append(tamd->activityTagIndex);
							teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc].append(tamd->maxHoursDaily);
							teachersActivityTagMaxHoursDailyRealDaysPercentage[tc].append(tamd->weightPercentage);
						}
						else{

							if(teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc].at(pos2) <= tamd->maxHoursDaily
							 && teachersActivityTagMaxHoursDailyRealDaysPercentage[tc].at(pos2) >= tamd->weightPercentage){
								//do nothing
							}
							else if(teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc].at(pos2) >= tamd->maxHoursDaily
							 && teachersActivityTagMaxHoursDailyRealDaysPercentage[tc].at(pos2) <= tamd->weightPercentage){

								teachersActivityTagMaxHoursDailyRealDaysActivityTag[tc][pos2]=tamd->activityTagIndex;
								teachersActivityTagMaxHoursDailyRealDaysMaxHours[tc][pos2]=tamd->maxHoursDaily;
								teachersActivityTagMaxHoursDailyRealDaysPercentage[tc][pos2]=tamd->weightPercentage;
							}
							else{
								ok=false;

								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for teacher %1 and activity tag %2, because there are too many constraints"
								 " of type activity tag max hours daily per real day relating to them, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a teacher and an activity tag, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours daily per real day.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalTeachersList[tc]->name)
								 .arg(tamd->activityTagName),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
	}
	Matrix1D<int> navd;
	navd.resize(gt.rules.nDaysPerWeek/2);

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		for(int d=0; d<gt.rules.nDaysPerWeek/2; d++){
			navd[d]=0;
			for(int h=0; h<gt.rules.nHoursPerDay; h++){
				if(!breakDayHour[2*d][h] && !teacherNotAvailableDayHour[i][2*d][h])
					navd[d]++;
				if(!breakDayHour[2*d+1][h] && !teacherNotAvailableDayHour[i][2*d+1][h])
					navd[d]++;
			}
		}

		for(int j=0; j<teachersActivityTagMaxHoursDailyRealDaysMaxHours[i].count(); j++){
			int mh=teachersActivityTagMaxHoursDailyRealDaysMaxHours[i].at(j);
			double perc=teachersActivityTagMaxHoursDailyRealDaysPercentage[i].at(j);
			int at=teachersActivityTagMaxHoursDailyRealDaysActivityTag[i].at(j);
			if(perc==100.0){
				int totalAt=0;
				for(int ai : qAsConst(gt.rules.internalTeachersList[i]->activitiesForTeacher))
					if(gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(at))
						totalAt+=gt.rules.internalActivitiesList[ai].duration;

				int ava=0;
				for(int d=0; d<gt.rules.nDaysPerWeek/2; d++)
					ava+=min(navd[d], mh);

				if(ava<totalAt){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there is a constraint activity tag %2 max %3 hours daily per real day for it with weight 100%"
					 " which cannot be satisfied, considering the number of available slots (%4) and total duration of activities with this activity tag (%5)"
					 ". Please correct and try again.", "%2 is the activity tag for this constraint, %3 is the max number of hours daily for this constraint")
					 .arg(gt.rules.internalTeachersList[i]->name).arg(gt.rules.activityTagsList.at(at)->name).arg(mh).arg(ava).arg(totalAt),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
		}
	}

	return ok;
}

bool computeTeachersActivityTagMaxHoursContinuously(QWidget* parent)
{
	haveTeachersActivityTagMaxHoursContinuously=false;
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersActivityTagMaxHoursContinuouslyMaxHours[i].clear();
		teachersActivityTagMaxHoursContinuouslyPercentage[i].clear();
		teachersActivityTagMaxHoursContinuouslyActivityTag[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			haveTeachersActivityTagMaxHoursContinuously=true;

			ConstraintTeachersActivityTagMaxHoursContinuously* tamc=(ConstraintTeachersActivityTagMaxHoursContinuously*)gt.rules.internalTimeConstraintsList[i];
			
			for(int tc : qAsConst(tamc->canonicalTeachersList)){
				int pos1=-1, pos2=-1;
				
				for(int j=0; j<teachersActivityTagMaxHoursContinuouslyMaxHours[tc].count(); j++){
					if(teachersActivityTagMaxHoursContinuouslyActivityTag[tc].at(j)==tamc->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}
				
				if(pos1==-1){
					teachersActivityTagMaxHoursContinuouslyActivityTag[tc].append(tamc->activityTagIndex);
					teachersActivityTagMaxHoursContinuouslyMaxHours[tc].append(tamc->maxHoursContinuously);
					teachersActivityTagMaxHoursContinuouslyPercentage[tc].append(tamc->weightPercentage);
				}
				else{
					if(teachersActivityTagMaxHoursContinuouslyMaxHours[tc].at(pos1) <= tamc->maxHoursContinuously
					 && teachersActivityTagMaxHoursContinuouslyPercentage[tc].at(pos1) >= tamc->weightPercentage){
					 	//do nothing
					}
					else if(teachersActivityTagMaxHoursContinuouslyMaxHours[tc].at(pos1) >= tamc->maxHoursContinuously
					 && teachersActivityTagMaxHoursContinuouslyPercentage[tc].at(pos1) <= tamc->weightPercentage){
					
						teachersActivityTagMaxHoursContinuouslyActivityTag[tc][pos1]=tamc->activityTagIndex;
						teachersActivityTagMaxHoursContinuouslyMaxHours[tc][pos1]=tamc->maxHoursContinuously;
						teachersActivityTagMaxHoursContinuouslyPercentage[tc][pos1]=tamc->weightPercentage;
					}
					else{
						if(pos2==-1){
							teachersActivityTagMaxHoursContinuouslyActivityTag[tc].append(tamc->activityTagIndex);
							teachersActivityTagMaxHoursContinuouslyMaxHours[tc].append(tamc->maxHoursContinuously);
							teachersActivityTagMaxHoursContinuouslyPercentage[tc].append(tamc->weightPercentage);
						}
						else{

							if(teachersActivityTagMaxHoursContinuouslyMaxHours[tc].at(pos2) <= tamc->maxHoursContinuously
							 && teachersActivityTagMaxHoursContinuouslyPercentage[tc].at(pos2) >= tamc->weightPercentage){
							 	//do nothing
							}
							else if(teachersActivityTagMaxHoursContinuouslyMaxHours[tc].at(pos2) >= tamc->maxHoursContinuously
							 && teachersActivityTagMaxHoursContinuouslyPercentage[tc].at(pos2) <= tamc->weightPercentage){
							
								teachersActivityTagMaxHoursContinuouslyActivityTag[tc][pos2]=tamc->activityTagIndex;
								teachersActivityTagMaxHoursContinuouslyMaxHours[tc][pos2]=tamc->maxHoursContinuously;
								teachersActivityTagMaxHoursContinuouslyPercentage[tc][pos2]=tamc->weightPercentage;
							}
							else{
								ok=false;
	
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for teacher %1 and activity tag %2, because there are too many constraints"
								 " of type activity tag max hours continuously relating to them, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a teacher and an activity tag, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours continuously.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalTeachersList[tc]->name)
								 .arg(tamc->activityTagName),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
			haveTeachersActivityTagMaxHoursContinuously=true;

			ConstraintTeacherActivityTagMaxHoursContinuously* tamc=(ConstraintTeacherActivityTagMaxHoursContinuously*)gt.rules.internalTimeConstraintsList[i];
			
			for(int tc : qAsConst(tamc->canonicalTeachersList)){
				int pos1=-1, pos2=-1;
				
				for(int j=0; j<teachersActivityTagMaxHoursContinuouslyMaxHours[tc].count(); j++){
					if(teachersActivityTagMaxHoursContinuouslyActivityTag[tc].at(j)==tamc->activityTagIndex){
						if(pos1==-1){
							pos1=j;
						}
						else{
							assert(pos2==-1);
							pos2=j;
						}
					}
				}
				
				if(pos1==-1){
					teachersActivityTagMaxHoursContinuouslyActivityTag[tc].append(tamc->activityTagIndex);
					teachersActivityTagMaxHoursContinuouslyMaxHours[tc].append(tamc->maxHoursContinuously);
					teachersActivityTagMaxHoursContinuouslyPercentage[tc].append(tamc->weightPercentage);
				}
				else{
					if(teachersActivityTagMaxHoursContinuouslyMaxHours[tc].at(pos1) <= tamc->maxHoursContinuously
					 && teachersActivityTagMaxHoursContinuouslyPercentage[tc].at(pos1) >= tamc->weightPercentage){
					 	//do nothing
					}
					else if(teachersActivityTagMaxHoursContinuouslyMaxHours[tc].at(pos1) >= tamc->maxHoursContinuously
					 && teachersActivityTagMaxHoursContinuouslyPercentage[tc].at(pos1) <= tamc->weightPercentage){
					
						teachersActivityTagMaxHoursContinuouslyActivityTag[tc][pos1]=tamc->activityTagIndex;
						teachersActivityTagMaxHoursContinuouslyMaxHours[tc][pos1]=tamc->maxHoursContinuously;
						teachersActivityTagMaxHoursContinuouslyPercentage[tc][pos1]=tamc->weightPercentage;
					}
					else{
						if(pos2==-1){
							teachersActivityTagMaxHoursContinuouslyActivityTag[tc].append(tamc->activityTagIndex);
							teachersActivityTagMaxHoursContinuouslyMaxHours[tc].append(tamc->maxHoursContinuously);
							teachersActivityTagMaxHoursContinuouslyPercentage[tc].append(tamc->weightPercentage);
						}
						else{

							if(teachersActivityTagMaxHoursContinuouslyMaxHours[tc].at(pos2) <= tamc->maxHoursContinuously
							 && teachersActivityTagMaxHoursContinuouslyPercentage[tc].at(pos2) >= tamc->weightPercentage){
							 	//do nothing
							}
							else if(teachersActivityTagMaxHoursContinuouslyMaxHours[tc].at(pos2) >= tamc->maxHoursContinuously
							 && teachersActivityTagMaxHoursContinuouslyPercentage[tc].at(pos2) <= tamc->weightPercentage){
							
								teachersActivityTagMaxHoursContinuouslyActivityTag[tc][pos2]=tamc->activityTagIndex;
								teachersActivityTagMaxHoursContinuouslyMaxHours[tc][pos2]=tamc->maxHoursContinuously;
								teachersActivityTagMaxHoursContinuouslyPercentage[tc][pos2]=tamc->weightPercentage;
							}
							else{
								ok=false;
	
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize for teacher %1 and activity tag %2, because there are too many constraints"
								 " of type activity tag max hours continuously relating to them, which cannot be compressed in 2 constraints of this type."
								 " Two constraints max hours can be compressed into a single one if the max hours are lower"
								 " in the first one and the weight percentage is higher on the first one."
								 " It is possible to use any number of such constraints for a teacher and an activity tag, but their resultant must"
								 " be maximum 2 constraints of type activity tag max hours continuously.\n\n"
								 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
								 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
								 " Please modify your data accordingly and try again.")
								 .arg(gt.rules.internalTeachersList[tc]->name)
								 .arg(tamc->activityTagName),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return false;
							}
						}
					}
				}
			}
		}
	}
	
	return ok;
}

//2020-06-28
//must be after n hours per teacher
bool computeTeachersMaxHoursPerAllAfternoons(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMaxHoursPerAllAfternoonsMaxHours[i]=-1;
		teachersMaxHoursPerAllAfternoonsPercentages[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_HOURS_PER_ALL_AFTERNOONS){
			ConstraintTeacherMaxHoursPerAllAfternoons* tmd=(ConstraintTeacherMaxHoursPerAllAfternoons*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max hours per all afternoons for teacher %1 with"
				 " weight (percentage) below 100. Please make weight 100% and try again")
				 .arg(tmd->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			if(teachersMaxHoursPerAllAfternoonsMaxHours[tmd->teacher_ID]==-1 || teachersMaxHoursPerAllAfternoonsMaxHours[tmd->teacher_ID]>tmd->maxHoursPerAllAfternoons){
				teachersMaxHoursPerAllAfternoonsMaxHours[tmd->teacher_ID]=tmd->maxHoursPerAllAfternoons;
				teachersMaxHoursPerAllAfternoonsPercentages[tmd->teacher_ID]=100;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_HOURS_PER_ALL_AFTERNOONS){
			ConstraintTeachersMaxHoursPerAllAfternoons* tmd=(ConstraintTeachersMaxHoursPerAllAfternoons*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max hours per all afternoons with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMaxHoursPerAllAfternoonsMaxHours[tch]==-1 || teachersMaxHoursPerAllAfternoonsMaxHours[tch]>tmd->maxHoursPerAllAfternoons){
					teachersMaxHoursPerAllAfternoonsMaxHours[tch]=tmd->maxHoursPerAllAfternoons;
					teachersMaxHoursPerAllAfternoonsPercentages[tch]=100;
				}
			}
		}
	}

	for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
		if(teachersMaxHoursPerAllAfternoonsMaxHours[tch]>=0){
			int nAvailableMorningHours=0;
			for(int d=0; d<gt.rules.nDaysPerWeek; d+=2) //morning
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tch][d][h])
						nAvailableMorningHours++;

			if(teachersMaxHoursPerAllAfternoonsMaxHours[tch]+nAvailableMorningHours<nHoursPerTeacher[tch]){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher(s) max hours per all afternoons for teacher %1 which is impossible"
				 " to respect (allowed afternoon = %2, available morning = %3, total hours for teacher = %4)")
				 .arg(gt.rules.internalTeachersList[tch]->name)
				 .arg(teachersMaxHoursPerAllAfternoonsMaxHours[tch])
				 .arg(nAvailableMorningHours)
				 .arg(nHoursPerTeacher[tch]),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
	}

	return ok;
}

//2020-06-28
//must be after n hours per subgroup
bool computeStudentsMaxHoursPerAllAfternoons(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMaxHoursPerAllAfternoonsMaxHours[i]=-1;
		subgroupsMaxHoursPerAllAfternoonsPercentages[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_PER_ALL_AFTERNOONS){
			ConstraintStudentsSetMaxHoursPerAllAfternoons* tmd=(ConstraintStudentsSetMaxHoursPerAllAfternoons*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set max hours per all afternoons for students set %1 with"
				 " weight (percentage) below 100. Please make weight 100% and try again")
				 .arg(tmd->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int sbg : qAsConst(tmd->iSubgroupsList))
				if(subgroupsMaxHoursPerAllAfternoonsMaxHours[sbg]==-1 || subgroupsMaxHoursPerAllAfternoonsMaxHours[sbg]>tmd->maxHoursPerAllAfternoons){
					subgroupsMaxHoursPerAllAfternoonsMaxHours[sbg]=tmd->maxHoursPerAllAfternoons;
					subgroupsMaxHoursPerAllAfternoonsPercentages[sbg]=100;
				}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_HOURS_PER_ALL_AFTERNOONS){
			ConstraintStudentsMaxHoursPerAllAfternoons* tmd=(ConstraintStudentsMaxHoursPerAllAfternoons*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students max hours per all afternoons with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				if(subgroupsMaxHoursPerAllAfternoonsMaxHours[sbg]==-1 || subgroupsMaxHoursPerAllAfternoonsMaxHours[sbg]>tmd->maxHoursPerAllAfternoons){
					subgroupsMaxHoursPerAllAfternoonsMaxHours[sbg]=tmd->maxHoursPerAllAfternoons;
					subgroupsMaxHoursPerAllAfternoonsPercentages[sbg]=100;
				}
			}
		}
	}

	for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
		if(subgroupsMaxHoursPerAllAfternoonsMaxHours[sbg]>=0){
			int nAvailableMorningHours=0;
			for(int d=0; d<gt.rules.nDaysPerWeek; d+=2) //morning
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[sbg][d][h])
						nAvailableMorningHours++;

			if(subgroupsMaxHoursPerAllAfternoonsMaxHours[sbg]+nAvailableMorningHours<nHoursPerSubgroup[sbg]){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students (set) max hours per all afternoons for subgroup %1 which is impossible"
				 " to respect (allowed afternoon = %2, available morning = %3, total hours for subgroup = %4)")
				 .arg(gt.rules.internalSubgroupsList[sbg]->name)
				 .arg(subgroupsMaxHoursPerAllAfternoonsMaxHours[sbg])
				 .arg(nAvailableMorningHours)
				 .arg(nHoursPerSubgroup[sbg]),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
	}

	return ok;
}

//must be after n hours per teacher
bool computeTeachersMinHoursDaily(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMinHoursDailyMinHours[i][1]=-1;
		teachersMinHoursDailyPercentages[i][1]=-1;

		teachersMinHoursPerAfternoonMinHours[i]=-1;
		teachersMinHoursPerAfternoonPercentages[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_HOURS_DAILY){
			ConstraintTeacherMinHoursDaily* tmd=(ConstraintTeacherMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min hours daily for teacher %1 with"
				 " weight (percentage) below 100. Starting with FET version 5.4.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tmd->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minHoursDaily>gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min hours daily for teacher %1 with"
				 " %2 min hours daily, and the number of working hours per day is only %3. Please correct and try again")
				 .arg(tmd->teacherName)
				 .arg(tmd->minHoursDaily)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			if(teachersMinHoursDailyMinHours[tmd->teacher_ID][MIN_HOURS_DAILY_INDEX_IN_ARRAY]==-1 || teachersMinHoursDailyMinHours[tmd->teacher_ID][MIN_HOURS_DAILY_INDEX_IN_ARRAY]<tmd->minHoursDaily){
				teachersMinHoursDailyMinHours[tmd->teacher_ID][MIN_HOURS_DAILY_INDEX_IN_ARRAY]=tmd->minHoursDaily;
				teachersMinHoursDailyPercentages[tmd->teacher_ID][MIN_HOURS_DAILY_INDEX_IN_ARRAY]=100;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_HOURS_DAILY){
			ConstraintTeachersMinHoursDaily* tmd=(ConstraintTeachersMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min hours daily with"
				 " weight (percentage) below 100. Starting with FET version 5.4.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minHoursDaily>gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min hours daily with"
				 " %1 min hours daily, and the number of working hours per day is only %2. Please correct and try again")
				 .arg(tmd->minHoursDaily)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////
			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMinHoursDailyMinHours[tch][MIN_HOURS_DAILY_INDEX_IN_ARRAY]==-1 || teachersMinHoursDailyMinHours[tch][MIN_HOURS_DAILY_INDEX_IN_ARRAY]<tmd->minHoursDaily){
					teachersMinHoursDailyMinHours[tch][MIN_HOURS_DAILY_INDEX_IN_ARRAY]=tmd->minHoursDaily;
					teachersMinHoursDailyPercentages[tch][MIN_HOURS_DAILY_INDEX_IN_ARRAY]=100;
				}
			}
		}
	}

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMinHoursDailyMinHours[i][0]=teachersMinHoursDailyMinHours[i][1];
		teachersMinHoursDailyPercentages[i][0]=teachersMinHoursDailyPercentages[i][1];
	}

	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING){
				ConstraintTeacherMinHoursPerMorning* tmd=(ConstraintTeacherMinHoursPerMorning*)gt.rules.internalTimeConstraintsList[i];

				//////////
				if(tmd->weightPercentage!=100){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min hours per morning for teacher %1 with"
					 " weight (percentage) below 100. Starting with FET version 5.4.0 it is only possible"
					 " to use 100% weight for such constraints. Please make weight 100% and try again")
					 .arg(tmd->teacherName),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
				//////////

				//////////
				if(tmd->minHoursPerMorning>gt.rules.nHoursPerDay){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min hours per morning for teacher %1 with"
					 " %2 min hours per morning, and the number of working hours per day is only %3. Please correct and try again")
					 .arg(tmd->teacherName)
					 .arg(tmd->minHoursPerMorning)
					 .arg(gt.rules.nHoursPerDay),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
				//////////

				if(teachersMinHoursDailyMinHours[tmd->teacher_ID][1]==-1){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min hours per morning for teacher %1 but not also"
					 " min hours daily for him. Please add a constraint teacher(s) min hours daily affecting this teacher.")
					 .arg(tmd->teacherName),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}

				if(teachersMinHoursDailyMinHours[tmd->teacher_ID][0]==-1 || teachersMinHoursDailyMinHours[tmd->teacher_ID][0]<tmd->minHoursPerMorning){
					teachersMinHoursDailyMinHours[tmd->teacher_ID][0]=tmd->minHoursPerMorning;
					teachersMinHoursDailyPercentages[tmd->teacher_ID][0]=100;
				}
			}
			else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_HOURS_PER_MORNING){
				ConstraintTeachersMinHoursPerMorning* tmd=(ConstraintTeachersMinHoursPerMorning*)gt.rules.internalTimeConstraintsList[i];

				//////////
				if(tmd->weightPercentage!=100){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min hours per morning with"
					 " weight (percentage) below 100. Starting with FET version 5.4.0 it is only possible"
					 " to use 100% weight for such constraints. Please make weight 100% and try again"),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
				//////////

				//////////
				if(tmd->minHoursPerMorning>gt.rules.nHoursPerDay){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min hours per morning with"
					 " %1 min hours per morning, and the number of working hours per day is only %2. Please correct and try again")
					 .arg(tmd->minHoursPerMorning)
					 .arg(gt.rules.nHoursPerDay),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
				//////////
				for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
					if(teachersMinHoursDailyMinHours[tch][1]==-1){
						ok=false;

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min hours per morning for teacher %1 but not also"
						 " min hours daily for him. Please add a constraint teacher(s) min hours daily affecting this teacher.")
						 .arg(gt.rules.internalTeachersList[tch]->name),
						/*int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min hours per morning for all the teachers but not also"
						 " min hours daily for them. Please add one or more constraints teacher(s) min hours daily."),*/
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}

					if(teachersMinHoursDailyMinHours[tch][0]==-1 || teachersMinHoursDailyMinHours[tch][0]<tmd->minHoursPerMorning){
						teachersMinHoursDailyMinHours[tch][0]=tmd->minHoursPerMorning;
						teachersMinHoursDailyPercentages[tch][0]=100;
					}
				}
			}
		}

		//2022-09-10
		for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON){
				ConstraintTeacherMinHoursPerAfternoon* tmd=(ConstraintTeacherMinHoursPerAfternoon*)gt.rules.internalTimeConstraintsList[i];

				//////////
				if(tmd->weightPercentage!=100){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min hours per afternoon for teacher %1 with"
					 " weight (percentage) below 100. Starting with FET version 5.4.0 it is only possible"
					 " to use 100% weight for such constraints. Please make weight 100% and try again")
					 .arg(tmd->teacherName),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
				//////////

				//////////
				if(tmd->minHoursPerAfternoon>gt.rules.nHoursPerDay){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min hours per afternoon for teacher %1 with"
					 " %2 min hours per afternoon, and the number of working hours per day is only %3. Please correct and try again")
					 .arg(tmd->teacherName)
					 .arg(tmd->minHoursPerAfternoon)
					 .arg(gt.rules.nHoursPerDay),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
				//////////

				if(teachersMinHoursDailyMinHours[tmd->teacher_ID][1]==-1){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min hours per afternoon for teacher %1 but not also"
					 " min hours daily for him. Please add a constraint teacher(s) min hours daily affecting this teacher.")
					 .arg(tmd->teacherName),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}

				if(teachersMinHoursPerAfternoonMinHours[tmd->teacher_ID]==-1 || teachersMinHoursPerAfternoonMinHours[tmd->teacher_ID]<tmd->minHoursPerAfternoon){
					teachersMinHoursPerAfternoonMinHours[tmd->teacher_ID]=tmd->minHoursPerAfternoon;
					teachersMinHoursPerAfternoonPercentages[tmd->teacher_ID]=100;
				}
			}
			else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_HOURS_PER_AFTERNOON){
				ConstraintTeachersMinHoursPerAfternoon* tmd=(ConstraintTeachersMinHoursPerAfternoon*)gt.rules.internalTimeConstraintsList[i];

				//////////
				if(tmd->weightPercentage!=100){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min hours per afternoon with"
					 " weight (percentage) below 100. Starting with FET version 5.4.0 it is only possible"
					 " to use 100% weight for such constraints. Please make weight 100% and try again"),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
				//////////

				//////////
				if(tmd->minHoursPerAfternoon>gt.rules.nHoursPerDay){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min hours per afternoon with"
					 " %1 min hours per afternoon, and the number of working hours per day is only %2. Please correct and try again")
					 .arg(tmd->minHoursPerAfternoon)
					 .arg(gt.rules.nHoursPerDay),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
				//////////
				for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
					if(teachersMinHoursDailyMinHours[tch][1]==-1){
						ok=false;

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min hours per afternoon for teacher %1 but not also"
						 " min hours daily for him. Please add a constraint teacher(s) min hours daily affecting this teacher.")
						 .arg(gt.rules.internalTeachersList[tch]->name),
						/*int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min hours per afternoon for all the teachers but not also"
						 " min hours daily for them. Please add one or more constraints teacher(s) min hours daily."),*/
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}

					if(teachersMinHoursPerAfternoonMinHours[tch]==-1 || teachersMinHoursPerAfternoonMinHours[tch]<tmd->minHoursPerAfternoon){
						teachersMinHoursPerAfternoonMinHours[tch]=tmd->minHoursPerAfternoon;
						teachersMinHoursPerAfternoonPercentages[tch]=100;
					}
				}
			}
		}
	}

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMinHoursDailyPercentages[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY]==100){
			assert(teachersMinHoursDailyMinHours[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0);
			if(nHoursPerTeacher[tc]>0 && teachersMinHoursDailyMinHours[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>nHoursPerTeacher[tc]){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min %1 hours daily for teacher"
				 " %2 (the constraint allows empty days). This teacher has in total only %3 hours per week, so impossible constraint."
				 " Please correct and try again")
				 .arg(teachersMinHoursDailyMinHours[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY])
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(nHoursPerTeacher[tc])
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			if(teachersMinHoursDailyMinHours[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY]<2){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min %1 hours daily for teacher"
				 " %2 (the constraint allows empty days). The number of min hours daily should be at least 2, to make a non-trivial constraint. Please correct and try again")
				 .arg(teachersMinHoursDailyMinHours[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY])
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			/*if(teachersMinHoursDailyMinHours[tc][0]>=0 && teachersMinHoursPerMorningAllowEmptyMornings[tc]==false){
				if(gt.rules.nDaysPerWeek/2*teachersMinHoursDailyMinHours[tc][0] > nHoursPerTeacher[tc]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("For teacher %1 you have too little activities to respect the constraint(s)"
					 " of type min hours per morning (the constraint(s) do not allow empty mornings). Please modify your data accordingly and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}

			if(teachersMinHoursPerAfternoonMinHours[tc]>=0 && teachersMinHoursPerAfternoonAllowEmptyAfternoons[tc]==false){
				if(gt.rules.nDaysPerWeek/2*teachersMinHoursPerAfternoonMinHours[tc] > nHoursPerTeacher[tc]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("For teacher %1 you have too little activities to respect the constraint(s)"
					 " of type min hours per afternoon (the constraint(s) do not allow empty afternoons). Please modify your data accordingly and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}*/
		}
	}

	return ok;
}

//must be after n hours per teacher
bool computeTeachersMinHoursDailyRealDays(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMinHoursDailyRealDaysMinHours[i]=-1;
		teachersMinHoursDailyRealDaysPercentages[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS){
			ConstraintTeacherMinHoursDailyRealDays* tmd=(ConstraintTeacherMinHoursDailyRealDays*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min hours daily for real days for teacher %1 with"
				 " weight (percentage) below 100. Starting with FET version 5.4.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tmd->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minHoursDaily>2*gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min hours daily for real days for teacher %1 with"
				 " %2 min hours daily, and the number of working hours per real day is only %3. Please correct and try again")
				 .arg(tmd->teacherName)
				 .arg(tmd->minHoursDaily)
				 .arg(2*gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			if(teachersMinHoursDailyRealDaysMinHours[tmd->teacher_ID]==-1 || teachersMinHoursDailyRealDaysMinHours[tmd->teacher_ID]<tmd->minHoursDaily){
				teachersMinHoursDailyRealDaysMinHours[tmd->teacher_ID]=tmd->minHoursDaily;
				teachersMinHoursDailyRealDaysPercentages[tmd->teacher_ID]=100;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_HOURS_DAILY_REAL_DAYS){
			ConstraintTeachersMinHoursDailyRealDays* tmd=(ConstraintTeachersMinHoursDailyRealDays*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min hours daily for real days with"
				 " weight (percentage) below 100. Starting with FET version 5.4.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minHoursDaily>2*gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min hours daily for real days with"
				 " %1 min hours daily, and the number of working hours per real day is only %2. Please correct and try again")
				 .arg(tmd->minHoursDaily)
				 .arg(2*gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////
			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMinHoursDailyRealDaysMinHours[tch]==-1 || teachersMinHoursDailyRealDaysMinHours[tch]<tmd->minHoursDaily){
					teachersMinHoursDailyRealDaysMinHours[tch]=tmd->minHoursDaily;
					teachersMinHoursDailyRealDaysPercentages[tch]=100;
				}
			}
		}
	}

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMinHoursDailyRealDaysPercentages[tc]==100){
			assert(teachersMinHoursDailyRealDaysMinHours[tc]>=0);
			if(nHoursPerTeacher[tc]>0 && teachersMinHoursDailyRealDaysMinHours[tc]>nHoursPerTeacher[tc]){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min %1 hours daily for real days for teacher"
				 " %2 (the constraint allows empty days). This teacher has in total only %3 hours per week, so impossible constraint."
				 " Please correct and try again")
				 .arg(teachersMinHoursDailyRealDaysMinHours[tc])
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(nHoursPerTeacher[tc])
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			if(teachersMinHoursDailyRealDaysMinHours[tc]<2){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min %1 hours daily for real days for teacher"
				 " %2 (the constraint allows empty days). The number of min hours daily for real days should be at least 2, to make a non-trivial constraint. Please correct and try again")
				 .arg(teachersMinHoursDailyRealDaysMinHours[tc])
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
	}

	return ok;
}

bool computeTeachersActivityTagMinHoursDaily(QWidget* parent)
{
	haveTeachersActivityTagMinHoursDaily=false;
	
	bool ok=true;
	
	tatmhdList.clear();
	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		tatmhdListForTeacher[i].clear();
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MIN_HOURS_DAILY){
			haveTeachersActivityTagMinHoursDaily=true;
			ConstraintTeachersActivityTagMinHoursDaily* tmd=(ConstraintTeachersActivityTagMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			if(tmd->weightPercentage!=100){
				ok=false;
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teachers, because the constraint of type activity tag min hours daily relating to teachers"
				 " has no 100% weight"
				 ". Please modify your data accordingly and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MIN_HOURS_DAILY){
			haveTeachersActivityTagMinHoursDaily=true;
			ConstraintTeacherActivityTagMinHoursDaily* tmd=(ConstraintTeacherActivityTagMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			if(tmd->weightPercentage!=100){
				ok=false;
	
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teachers, because the constraint of type activity tag min hours daily relating to teacher %1"
				 " has no 100% weight"
				 ". Please modify your data accordingly and try again").arg(tmd->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}

	if(haveTeachersActivityTagMinHoursDaily){
		for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_ACTIVITY_TAG_MIN_HOURS_DAILY){
				ConstraintTeachersActivityTagMinHoursDaily* tmd=(ConstraintTeachersActivityTagMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];
				
				for(int tch : qAsConst(tmd->canonicalTeachersList)){
					TeacherActivityTagMinHoursDaily_item item;
					item.durationOfActivitiesWithActivityTagForTeacher=0;
					
					for(int ai : qAsConst(gt.rules.internalTeachersList[tch]->activitiesForTeacher)){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(act->iActivityTagsSet.contains(tmd->activityTagIndex))
							item.durationOfActivitiesWithActivityTagForTeacher+=act->duration;
					}
					
					if(item.durationOfActivitiesWithActivityTagForTeacher>0){
						item.activityTag=tmd->activityTagIndex;
						item.minHoursDaily=tmd->minHoursDaily;
						item.allowEmptyDays=tmd->allowEmptyDays;
						
						tatmhdList.push_back(item);
						//tatmhdListForTeacher[tch].append(&tatmhdList[tatmhdList.count()-1]);
						tatmhdListForTeacher[tch].append(&tatmhdList.back());
						
						if(!item.allowEmptyDays && item.durationOfActivitiesWithActivityTagForTeacher<gt.rules.nDaysPerWeek*item.minHoursDaily){
							ok=false;
				
							int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
							 GeneratePreTranslate::tr("Cannot optimize, because the constraint of type activity tag %1 min %2 hours daily relating to teacher %3"
							 " requires at least %4 hours of work per week, but the activities of this teacher with this activity tag sum to only %5 hours"
							 " per week (the constraint does not allow empty days). Please correct and try again")
							 .arg(tmd->activityTagName).arg(tmd->minHoursDaily).arg(gt.rules.internalTeachersList[tch]->name)
							 .arg(gt.rules.nDaysPerWeek*item.minHoursDaily).arg(item.durationOfActivitiesWithActivityTagForTeacher),
							 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
							 1, 0 );
							 	
							if(t==0)
								return false;
						}
					}
					else{
						assert(0);
					}
				}
			}
			else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_ACTIVITY_TAG_MIN_HOURS_DAILY){
				ConstraintTeacherActivityTagMinHoursDaily* tmd=(ConstraintTeacherActivityTagMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];
				
				for(int tch : qAsConst(tmd->canonicalTeachersList)){
					TeacherActivityTagMinHoursDaily_item item;
					item.durationOfActivitiesWithActivityTagForTeacher=0;
					
					for(int ai : qAsConst(gt.rules.internalTeachersList[tch]->activitiesForTeacher)){
						Activity* act=&gt.rules.internalActivitiesList[ai];
						if(act->iActivityTagsSet.contains(tmd->activityTagIndex))
							item.durationOfActivitiesWithActivityTagForTeacher+=act->duration;
					}
					
					if(item.durationOfActivitiesWithActivityTagForTeacher>0){
						item.activityTag=tmd->activityTagIndex;
						item.minHoursDaily=tmd->minHoursDaily;
						item.allowEmptyDays=tmd->allowEmptyDays;
						
						tatmhdList.push_back(item);
						//tatmhdListForTeacher[tch].append(&tatmhdList[tatmhdList.count()-1]);
						tatmhdListForTeacher[tch].append(&tatmhdList.back());
						
						if(!item.allowEmptyDays && item.durationOfActivitiesWithActivityTagForTeacher<gt.rules.nDaysPerWeek*item.minHoursDaily){
							ok=false;
				
							int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
							 GeneratePreTranslate::tr("Cannot optimize, because the constraint of type activity tag %1 min %2 hours daily relating to teacher %3"
							 " requires at least %4 hours of work per week, but the activities of this teacher with this activity tag sum to only %5 hours"
							 " per week (the constraint does not allow empty days). Please correct and try again")
							 .arg(tmd->activityTagName).arg(tmd->minHoursDaily).arg(gt.rules.internalTeachersList[tch]->name)
							 .arg(gt.rules.nDaysPerWeek*item.minHoursDaily).arg(item.durationOfActivitiesWithActivityTagForTeacher),
							 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
							 1, 0 );
							 	
							if(t==0)
								return false;
						}
					}
					else{
						assert(0);
					}
				}
			}
		}
	}
	
	return ok;
}

//must be after min hours for teachers (and min hours daily real days)
bool computeTeachersMinDaysPerWeek(QWidget* parent)
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMinDaysPerWeekMinDays[i]=-1;
		teachersMinDaysPerWeekPercentages[i]=-1;
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK){
			ConstraintTeacherMinDaysPerWeek* tmd=(ConstraintTeacherMinDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min days per week for teacher %1 with"
				 " weight (percentage) below 100. Please make weight 100% and try again")
				 .arg(tmd->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minDaysPerWeek>gt.rules.nDaysPerWeek){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min days per week for teacher %1 with"
				 " %2 min days per week, and the number of working days per week is only %3. Please correct and try again")
				 .arg(tmd->teacherName)
				 .arg(tmd->minDaysPerWeek)
				 .arg(gt.rules.nDaysPerWeek),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////

			if(teachersMinDaysPerWeekMinDays[tmd->teacher_ID]==-1 || teachersMinDaysPerWeekMinDays[tmd->teacher_ID]<tmd->minDaysPerWeek){
				teachersMinDaysPerWeekMinDays[tmd->teacher_ID]=tmd->minDaysPerWeek;
				teachersMinDaysPerWeekPercentages[tmd->teacher_ID]=100;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK){
			ConstraintTeachersMinDaysPerWeek* tmd=(ConstraintTeachersMinDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min days per week with weight"
				 " (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minDaysPerWeek>gt.rules.nDaysPerWeek){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min days per week with"
				 " %1 min days per week, and the number of working days per week is only %2. Please correct and try again")
				 .arg(tmd->minDaysPerWeek)
				 .arg(gt.rules.nDaysPerWeek),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////
			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMinDaysPerWeekMinDays[tch]==-1 || teachersMinDaysPerWeekMinDays[tch]<tmd->minDaysPerWeek){
					teachersMinDaysPerWeekMinDays[tch]=tmd->minDaysPerWeek;
					teachersMinDaysPerWeekPercentages[tch]=100;
				}
			}
		}
	}
	
	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMinDaysPerWeekMinDays[tc]>=0){
			int md=teachersMinDaysPerWeekMinDays[tc];
			if(md>gt.rules.internalTeachersList[tc]->activitiesForTeacher.count()){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min days per week %2 and he has only %3 activities - impossible."
				 " Please correct and try again.")
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(md)
				 .arg(gt.rules.internalTeachersList[tc]->activitiesForTeacher.count())
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			if(teachersMinHoursDailyMinHours[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0){
				int mh=teachersMinHoursDailyMinHours[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY];
				
				if(md*mh>nHoursPerTeacher[tc]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min days per week %2 and min hours daily %3"
					 " and he has only %4 working hours - impossible. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(md)
					 .arg(mh)
					 .arg(nHoursPerTeacher[tc])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		}
	}

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMinDaysPerWeekMinDays[tc]>=0){
			if(teachersMaxDaysPerWeekMaxDays[tc]>=0){
				if(teachersMaxDaysPerWeekMaxDays[tc]<teachersMinDaysPerWeekMinDays[tc]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min days per week %2 > max days per week %3"
					 " - impossible (min days must be <= max days). Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(teachersMinDaysPerWeekMinDays[tc])
					 .arg(teachersMaxDaysPerWeekMaxDays[tc])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		
			int med=1; //minimum each day = 1 hour
			if(teachersMinHoursDailyMinHours[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0)
				med=teachersMinHoursDailyMinHours[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY];
				
			int navdays=0;
			
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				int navhours=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h])
						navhours++;
				if(navhours>=med)
					navdays++;
			}
			
			if(navdays<teachersMinDaysPerWeekMinDays[tc]){
				ok=false;
				
				QString s;
				
				if(teachersMinHoursDailyMinHours[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0){
					s=GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min days per week %2 and only %3"
					 " available days considering breaks and not available and min hours daily for this teacher. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(teachersMinDaysPerWeekMinDays[tc])
					 .arg(navdays);
				}
				else{
					s=GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min days per week %2 and only %3"
					 " available days considering breaks and not available for this teacher. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(teachersMinDaysPerWeekMinDays[tc])
					 .arg(navdays);
				}

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
		 	
				if(t==0)
					return false;
			}
		}
	}

	
	return ok;
}

//must be after min hours for teachers
bool computeTeachersMinRealDaysPerWeek(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMinRealDaysPerWeekMinDays[i]=-1;
		teachersMinRealDaysPerWeekPercentages[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_REAL_DAYS_PER_WEEK){
			ConstraintTeacherMinRealDaysPerWeek* tmd=(ConstraintTeacherMinRealDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min real days per week for teacher %1 with"
				 " weight (percentage) below 100. Please make weight 100% and try again")
				 .arg(tmd->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minDaysPerWeek>gt.rules.nDaysPerWeek/2){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min real days per week for teacher %1 with"
				 " %2 min days per week, and the number of working real days per week is only %3. Please correct and try again")
				 .arg(tmd->teacherName)
				 .arg(tmd->minDaysPerWeek)
				 .arg(gt.rules.nDaysPerWeek/2),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			if(teachersMinRealDaysPerWeekMinDays[tmd->teacher_ID]==-1 || teachersMinRealDaysPerWeekMinDays[tmd->teacher_ID]<tmd->minDaysPerWeek){
				teachersMinRealDaysPerWeekMinDays[tmd->teacher_ID]=tmd->minDaysPerWeek;
				teachersMinRealDaysPerWeekPercentages[tmd->teacher_ID]=100;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_REAL_DAYS_PER_WEEK){
			ConstraintTeachersMinRealDaysPerWeek* tmd=(ConstraintTeachersMinRealDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min real days per week with weight"
				 " (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minDaysPerWeek>gt.rules.nDaysPerWeek/2){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min real days per week with"
				 " %1 min days per week, and the number of working real days per week is only %2. Please correct and try again")
				 .arg(tmd->minDaysPerWeek)
				 .arg(gt.rules.nDaysPerWeek/2),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////
			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMinRealDaysPerWeekMinDays[tch]==-1 || teachersMinRealDaysPerWeekMinDays[tch]<tmd->minDaysPerWeek){
					teachersMinRealDaysPerWeekMinDays[tch]=tmd->minDaysPerWeek;
					teachersMinRealDaysPerWeekPercentages[tch]=100;
				}
			}
		}
	}

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMinRealDaysPerWeekMinDays[tc]>=0){
			int md=teachersMinRealDaysPerWeekMinDays[tc];
			if(md>gt.rules.internalTeachersList[tc]->activitiesForTeacher.count()){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min real days per week %2 and he has only %3 activities - impossible."
				 " Please correct and try again.")
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(md)
				 .arg(gt.rules.internalTeachersList[tc]->activitiesForTeacher.count())
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			if(teachersMinHoursDailyMinHours[tc][1]>=0){
				int mh=teachersMinHoursDailyMinHours[tc][1];

				if(md*mh>nHoursPerTeacher[tc]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min real days per week %2 and min hours daily %3"
					 " and he has only %4 working hours - impossible. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(md)
					 .arg(mh)
					 .arg(nHoursPerTeacher[tc])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
			if(teachersMinHoursDailyRealDaysMinHours[tc]>=0){
				int mh=teachersMinHoursDailyRealDaysMinHours[tc];

				if(md*mh>nHoursPerTeacher[tc]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min real days per week %2 and min hours daily per real day %3"
					 " and he has only %4 working hours - impossible. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(md)
					 .arg(mh)
					 .arg(nHoursPerTeacher[tc])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
		}
	}

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMinRealDaysPerWeekMinDays[tc]>=0){
			if(teachersMaxRealDaysPerWeekMaxDays[tc]>=0){
				if(teachersMaxRealDaysPerWeekMaxDays[tc]<teachersMinRealDaysPerWeekMinDays[tc]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min real days per week %2 > max real days per week %3"
					 " - impossible (min days must be <= max days). Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(teachersMinRealDaysPerWeekMinDays[tc])
					 .arg(teachersMaxRealDaysPerWeekMaxDays[tc])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}

			int med[2]; //minimum each day = 1 hour
			med[1]=1; //afternoon
			med[0]=1; //morning
			if(teachersMinHoursDailyMinHours[tc][1]>=0)
				med[1]=teachersMinHoursDailyMinHours[tc][1];
			if(teachersMinHoursDailyMinHours[tc][0]>=0)
				med[0]=teachersMinHoursDailyMinHours[tc][0];

			int navdays=0;

			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				int navhours=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h])
						navhours++;
				if(navhours>=med[d%2])
					navdays++;
			}

			if(navdays<teachersMinRealDaysPerWeekMinDays[tc]){
				ok=false;

				QString s;

				if(teachersMinHoursDailyMinHours[tc][1]>=0){
					s=GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min real days per week %2 and only %3"
					 " available days considering breaks and not available and min hours daily for this teacher. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(teachersMinRealDaysPerWeekMinDays[tc])
					 .arg(navdays);
				}
				else{
					s=GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min real days per week %2 and only %3"
					 " available days considering breaks and not available for this teacher. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(teachersMinRealDaysPerWeekMinDays[tc])
					 .arg(navdays);
				}

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			int med_wd=1; //minimum each day = 1 hour, whole day
			if(teachersMinHoursDailyRealDaysMinHours[tc]>=0)
				med_wd=teachersMinHoursDailyRealDaysMinHours[tc];

			navdays=0;

			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++){
				int navhours=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++){
					if(!breakDayHour[2*d][h] && !teacherNotAvailableDayHour[tc][2*d][h])
						navhours++;
					if(!breakDayHour[2*d+1][h] && !teacherNotAvailableDayHour[tc][2*d+1][h])
						navhours++;
				}
				if(navhours>=med_wd)
					navdays++;
			}

			if(navdays<teachersMinRealDaysPerWeekMinDays[tc]){
				ok=false;

				QString s;

				if(teachersMinHoursDailyRealDaysMinHours[tc]>=0){
					s=GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min real days per week %2 and only %3"
					 " available days considering breaks and not available and min hours daily for real days for this teacher. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(teachersMinRealDaysPerWeekMinDays[tc])
					 .arg(navdays);
				}
				else{
					s=GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min real days per week %2 and only %3"
					 " available days considering breaks and not available for this teacher. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(teachersMinRealDaysPerWeekMinDays[tc])
					 .arg(navdays);
				}

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
	}

	return ok;
}

bool computeTeachersMinRestingHours(QWidget* parent)
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMinRestingHoursCircularMinHours[i]=-1;
		teachersMinRestingHoursCircularPercentages[i]=-1;
		teachersMinRestingHoursNotCircularMinHours[i]=-1;
		teachersMinRestingHoursNotCircularPercentages[i]=-1;
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_RESTING_HOURS){
			ConstraintTeacherMinRestingHours* tmrh=(ConstraintTeacherMinRestingHours*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmrh->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min resting hours for teacher %1 with"
				 " weight (percentage) below 100. It is only possible to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tmrh->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////
			if(tmrh->minRestingHours>gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min resting hours for teacher %1 with"
				 " %2 min resting hours, and the number of working hours per day is only %3. Please correct and try again")
				 .arg(tmrh->teacherName)
				 .arg(tmrh->minRestingHours)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////

			if(tmrh->circular==true){
				if(teachersMinRestingHoursCircularMinHours[tmrh->teacher_ID]==-1 ||
				 (teachersMinRestingHoursCircularMinHours[tmrh->teacher_ID]>=0 && teachersMinRestingHoursCircularMinHours[tmrh->teacher_ID]<tmrh->minRestingHours)){
					teachersMinRestingHoursCircularMinHours[tmrh->teacher_ID]=tmrh->minRestingHours;
					teachersMinRestingHoursCircularPercentages[tmrh->teacher_ID]=100;
				}
			}
			else{
				if(teachersMinRestingHoursNotCircularMinHours[tmrh->teacher_ID]==-1 ||
				 (teachersMinRestingHoursNotCircularMinHours[tmrh->teacher_ID]>=0 && teachersMinRestingHoursNotCircularMinHours[tmrh->teacher_ID]<tmrh->minRestingHours)){
					teachersMinRestingHoursNotCircularMinHours[tmrh->teacher_ID]=tmrh->minRestingHours;
					teachersMinRestingHoursNotCircularPercentages[tmrh->teacher_ID]=100;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_RESTING_HOURS){
			ConstraintTeachersMinRestingHours* tmrh=(ConstraintTeachersMinRestingHours*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmrh->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min resting hours with"
				 " weight (percentage) below 100. It is only possible to use 100% weight for such constraints. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////
			if(tmrh->minRestingHours>gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min resting hours with"
				 " %1 min resting hours, and the number of working hours per day is only %2. Please correct and try again")
				 .arg(tmrh->minRestingHours)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(tmrh->circular==true){
					if(teachersMinRestingHoursCircularMinHours[tch]==-1 ||
					 (teachersMinRestingHoursCircularMinHours[tch]>=0 && teachersMinRestingHoursCircularMinHours[tch]<tmrh->minRestingHours)){
						teachersMinRestingHoursCircularMinHours[tch]=tmrh->minRestingHours;
						teachersMinRestingHoursCircularPercentages[tch]=100;
					}
				}
				else{
					if(teachersMinRestingHoursNotCircularMinHours[tch]==-1 ||
					 (teachersMinRestingHoursNotCircularMinHours[tch]>=0 && teachersMinRestingHoursNotCircularMinHours[tch]<tmrh->minRestingHours)){
						teachersMinRestingHoursNotCircularMinHours[tch]=tmrh->minRestingHours;
						teachersMinRestingHoursNotCircularPercentages[tch]=100;
					}
				}
			}
		}
	}

	//small possible speedup
	for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
		if(teachersMinRestingHoursCircularMinHours[tch]>=0 && teachersMinRestingHoursNotCircularMinHours[tch]>=0 &&
		 teachersMinRestingHoursCircularMinHours[tch]>=teachersMinRestingHoursNotCircularMinHours[tch]){
			teachersMinRestingHoursNotCircularMinHours[tch]=-1;
			teachersMinRestingHoursNotCircularPercentages[tch]=-1;
		}
	}

	return ok;
}

bool computeSubgroupsMinRestingHours(QWidget* parent)
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMinRestingHoursCircularMinHours[i]=-1;
		subgroupsMinRestingHoursCircularPercentages[i]=-1;
		subgroupsMinRestingHoursNotCircularMinHours[i]=-1;
		subgroupsMinRestingHoursNotCircularPercentages[i]=-1;
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS){
			ConstraintStudentsSetMinRestingHours* smrh=(ConstraintStudentsSetMinRestingHours*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(smrh->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min resting hours for students set %1 with"
				 " weight (percentage) below 100. It is only possible to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(smrh->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////
			if(smrh->minRestingHours>gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min resting hours for students set %1 with"
				 " %2 min resting hours, and the number of working hours per day is only %3. Please correct and try again")
				 .arg(smrh->students)
				 .arg(smrh->minRestingHours)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////

			for(int sbg : qAsConst(smrh->iSubgroupsList)){
				if(smrh->circular==true){
					if(subgroupsMinRestingHoursCircularMinHours[sbg]==-1 ||
					 (subgroupsMinRestingHoursCircularMinHours[sbg]>=0 && subgroupsMinRestingHoursCircularMinHours[sbg]<smrh->minRestingHours)){
						subgroupsMinRestingHoursCircularMinHours[sbg]=smrh->minRestingHours;
						subgroupsMinRestingHoursCircularPercentages[sbg]=100;
					}
				}
				else{
					if(subgroupsMinRestingHoursNotCircularMinHours[sbg]==-1 ||
					 (subgroupsMinRestingHoursNotCircularMinHours[sbg]>=0 && subgroupsMinRestingHoursNotCircularMinHours[sbg]<smrh->minRestingHours)){
						subgroupsMinRestingHoursNotCircularMinHours[sbg]=smrh->minRestingHours;
						subgroupsMinRestingHoursNotCircularPercentages[sbg]=100;
					}
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_RESTING_HOURS){
			ConstraintStudentsMinRestingHours* smrh=(ConstraintStudentsMinRestingHours*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(smrh->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min resting hours with"
				 " weight (percentage) below 100. It is only possible to use 100% weight for such constraints. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////
			if(smrh->minRestingHours>gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min resting hours with"
				 " %1 min resting hours, and the number of working hours per day is only %2. Please correct and try again")
				 .arg(smrh->minRestingHours)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////

			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				if(smrh->circular==true){
					if(subgroupsMinRestingHoursCircularMinHours[sbg]==-1 ||
					 (subgroupsMinRestingHoursCircularMinHours[sbg]>=0 && subgroupsMinRestingHoursCircularMinHours[sbg]<smrh->minRestingHours)){
						subgroupsMinRestingHoursCircularMinHours[sbg]=smrh->minRestingHours;
						subgroupsMinRestingHoursCircularPercentages[sbg]=100;
					}
				}
				else{
					if(subgroupsMinRestingHoursNotCircularMinHours[sbg]==-1 ||
					 (subgroupsMinRestingHoursNotCircularMinHours[sbg]>=0 && subgroupsMinRestingHoursNotCircularMinHours[sbg]<smrh->minRestingHours)){
						subgroupsMinRestingHoursNotCircularMinHours[sbg]=smrh->minRestingHours;
						subgroupsMinRestingHoursNotCircularPercentages[sbg]=100;
					}
				}
			}
		}
	}

	//small possible speedup
	for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
		if(subgroupsMinRestingHoursCircularMinHours[sbg]>=0 && subgroupsMinRestingHoursNotCircularMinHours[sbg]>=0 &&
		 subgroupsMinRestingHoursCircularMinHours[sbg]>=subgroupsMinRestingHoursNotCircularMinHours[sbg]){
			subgroupsMinRestingHoursNotCircularMinHours[sbg]=-1;
			subgroupsMinRestingHoursNotCircularPercentages[sbg]=-1;
		}
	}

	return ok;
}

//must be after min hours for teachers (I think not), teachers max gaps per day and teachers max gaps per week
bool computeTeachersMinMorningsAfternoonsPerWeek(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMinMorningsPerWeekMinMornings[i]=-1;
		teachersMinMorningsPerWeekPercentages[i]=-1;

		teachersMinAfternoonsPerWeekMinAfternoons[i]=-1;
		teachersMinAfternoonsPerWeekPercentages[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_MORNINGS_PER_WEEK){
			ConstraintTeacherMinMorningsPerWeek* tmd=(ConstraintTeacherMinMorningsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min mornings per week for teacher %1 with"
				 " weight (percentage) below 100. Please make weight 100% and try again")
				 .arg(tmd->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minMorningsPerWeek>gt.rules.nDaysPerWeek/2){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min mornings per week for teacher %1 with"
				 " %2 min mornings per week, and the number of working mornings per week is only %3. Please correct and try again")
				 .arg(tmd->teacherName)
				 .arg(tmd->minMorningsPerWeek)
				 .arg(gt.rules.nDaysPerWeek/2),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			if(teachersMinMorningsPerWeekMinMornings[tmd->teacher_ID]==-1 || teachersMinMorningsPerWeekMinMornings[tmd->teacher_ID]<tmd->minMorningsPerWeek){
				teachersMinMorningsPerWeekMinMornings[tmd->teacher_ID]=tmd->minMorningsPerWeek;
				teachersMinMorningsPerWeekPercentages[tmd->teacher_ID]=100;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_MORNINGS_PER_WEEK){
			ConstraintTeachersMinMorningsPerWeek* tmd=(ConstraintTeachersMinMorningsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min mornings per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minMorningsPerWeek>gt.rules.nDaysPerWeek/2){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min mornings per week with"
				 " %1 min mornings per week, and the number of working mornings per week is only %2. Please correct and try again")
				 .arg(tmd->minMorningsPerWeek)
				 .arg(gt.rules.nDaysPerWeek/2),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMinMorningsPerWeekMinMornings[tch]==-1 || teachersMinMorningsPerWeekMinMornings[tch]<tmd->minMorningsPerWeek){
					teachersMinMorningsPerWeekMinMornings[tch]=tmd->minMorningsPerWeek;
					teachersMinMorningsPerWeekPercentages[tch]=100;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_AFTERNOONS_PER_WEEK){
			ConstraintTeacherMinAfternoonsPerWeek* tmd=(ConstraintTeacherMinAfternoonsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min afternoons per week for teacher %1 with"
				 " weight (percentage) below 100. Please make weight 100% and try again")
				 .arg(tmd->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minAfternoonsPerWeek>gt.rules.nDaysPerWeek/2){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min afternoons per week for teacher %1 with"
				 " %2 min afternoons per week, and the number of working afternoons per week is only %3. Please correct and try again")
				 .arg(tmd->teacherName)
				 .arg(tmd->minAfternoonsPerWeek)
				 .arg(gt.rules.nDaysPerWeek/2),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			if(teachersMinAfternoonsPerWeekMinAfternoons[tmd->teacher_ID]==-1 || teachersMinAfternoonsPerWeekMinAfternoons[tmd->teacher_ID]<tmd->minAfternoonsPerWeek){
				teachersMinAfternoonsPerWeekMinAfternoons[tmd->teacher_ID]=tmd->minAfternoonsPerWeek;
				teachersMinAfternoonsPerWeekPercentages[tmd->teacher_ID]=100;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_AFTERNOONS_PER_WEEK){
			ConstraintTeachersMinAfternoonsPerWeek* tmd=(ConstraintTeachersMinAfternoonsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min afternoons per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minAfternoonsPerWeek>gt.rules.nDaysPerWeek/2){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min afternoons per week with"
				 " %1 min afternoons per week, and the number of working afternoons per week is only %2. Please correct and try again")
				 .arg(tmd->minAfternoonsPerWeek)
				 .arg(gt.rules.nDaysPerWeek/2),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMinAfternoonsPerWeekMinAfternoons[tch]==-1 || teachersMinAfternoonsPerWeekMinAfternoons[tch]<tmd->minAfternoonsPerWeek){
					teachersMinAfternoonsPerWeekMinAfternoons[tch]=tmd->minAfternoonsPerWeek;
					teachersMinAfternoonsPerWeekPercentages[tch]=100;
				}
			}
		}
	}

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMinMorningsPerWeekMinMornings[tc]>=0){
			int md=teachersMinMorningsPerWeekMinMornings[tc];
			if(md>gt.rules.internalTeachersList[tc]->activitiesForTeacher.count()){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min mornings per week %2 and he has only %3 activities - impossible."
				 " Please correct and try again.")
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(md)
				 .arg(gt.rules.internalTeachersList[tc]->activitiesForTeacher.count())
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			if(teachersMinHoursDailyMinHours[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0){
				int mh=teachersMinHoursDailyMinHours[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY];
				
				if(md*mh>nHoursPerTeacher[tc]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min mornings per week %2 and min hours daily %3"
					 " and he has only %4 working hours - impossible. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(md)
					 .arg(mh)
					 .arg(nHoursPerTeacher[tc])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}

			if(teachersMinHoursDailyMinHours[tc][0]>=0 && teachersMinHoursDailyMinHours[tc][0]>teachersMinHoursDailyMinHours[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY]){
				int mh=teachersMinHoursDailyMinHours[tc][0];
				
				if(md*mh>nHoursPerTeacher[tc]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min mornings per week %2 and min hours per morning %3"
					 " and he has only %4 working hours - impossible. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(md)
					 .arg(mh)
					 .arg(nHoursPerTeacher[tc])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		}

		if(teachersMinAfternoonsPerWeekMinAfternoons[tc]>=0){
			int md=teachersMinAfternoonsPerWeekMinAfternoons[tc];
			if(md>gt.rules.internalTeachersList[tc]->activitiesForTeacher.count()){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min afternoons per week %2 and he has only %3 activities - impossible."
				 " Please correct and try again.")
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(md)
				 .arg(gt.rules.internalTeachersList[tc]->activitiesForTeacher.count())
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			if(teachersMinHoursDailyMinHours[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0){
				int mh=teachersMinHoursDailyMinHours[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY];
				
				if(md*mh>nHoursPerTeacher[tc]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min afternoons per week %2 and min hours daily %3"
					 " and he has only %4 working hours - impossible. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(md)
					 .arg(mh)
					 .arg(nHoursPerTeacher[tc])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}

			if(teachersMinHoursPerAfternoonMinHours[tc]>=0 && teachersMinHoursPerAfternoonMinHours[tc]>teachersMinHoursDailyMinHours[tc][MIN_HOURS_DAILY_INDEX_IN_ARRAY]){
				int mh=teachersMinHoursPerAfternoonMinHours[tc];
				
				if(md*mh>nHoursPerTeacher[tc]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min afternoons per week %2 and min hours per afternoon %3"
					 " and he has only %4 working hours - impossible. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(md)
					 .arg(mh)
					 .arg(nHoursPerTeacher[tc])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		}
	}

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMinMorningsPerWeekMinMornings[tc]>=0){
			if(teachersMaxMorningsPerWeekMaxMornings[tc]>=0){
				if(teachersMaxMorningsPerWeekMaxMornings[tc]<teachersMinMorningsPerWeekMinMornings[tc]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min mornings per week %2 > max mornings per week %3"
					 " - impossible (min mornings must be <= max mornings). Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(teachersMinMorningsPerWeekMinMornings[tc])
					 .arg(teachersMaxMorningsPerWeekMaxMornings[tc])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
		}
	}

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMinAfternoonsPerWeekMinAfternoons[tc]>=0){
			if(teachersMaxAfternoonsPerWeekMaxAfternoons[tc]>=0){
				if(teachersMaxAfternoonsPerWeekMaxAfternoons[tc]<teachersMinAfternoonsPerWeekMinAfternoons[tc]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min afternoons per week %2 > max afternoons per week %3"
					 " - impossible (min afternoons must be <= max afternoons). Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tc]->name)
					 .arg(teachersMinAfternoonsPerWeekMinAfternoons[tc])
					 .arg(teachersMaxAfternoonsPerWeekMaxAfternoons[tc])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
		}
	}

	/*for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
		if(teachersMinMorningsPerWeekMinMornings[tch]>=0){
			int md=teachersMinMorningsPerWeekMinMornings[tch];
			if(md>gt.rules.internalTeachersList[tch]->activitiesForTeacher.count()){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min mornings per week %2 and it has only %3 activities - impossible."
				 " Please correct and try again.")
				 .arg(gt.rules.internalTeachersList[tch]->name)
				 .arg(md)
				 .arg(gt.rules.internalTeachersList[tch]->activitiesForTeacher.count())
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			if(teachersMinHoursDailyMinHours[tch][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0){
				int mh=teachersMinHoursDailyMinHours[tch][MIN_HOURS_DAILY_INDEX_IN_ARRAY];
				
				if(md*mh>nHoursPerTeacher[tch]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min mornings per week %2 and min hours daily %3"
					 " and it has only %4 working hours - impossible. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tch]->name)
					 .arg(md)
					 .arg(mh)
					 .arg(nHoursPerTeacher[tch])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}

			if(teachersMinHoursDailyMinHours[tch][0]>=0 && teachersMinHoursDailyMinHours[tch][0]>teachersMinHoursDailyMinHours[tch][MIN_HOURS_DAILY_INDEX_IN_ARRAY]){
				int mh=teachersMinHoursDailyMinHours[tch][0];
				
				if(md*mh>nHoursPerTeacher[tch]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min mornings per week %2 and min hours per morning %3"
					 " and it has only %4 working hours - impossible. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tch]->name)
					 .arg(md)
					 .arg(mh)
					 .arg(nHoursPerTeacher[tch])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		}

		if(teachersMinAfternoonsPerWeekMinAfternoons[tch]>=0){
			int md=teachersMinAfternoonsPerWeekMinAfternoons[tch];
			if(md>gt.rules.internalTeachersList[tch]->activitiesForTeacher.count()){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min afternoons per week %2 and it has only %3 activities - impossible."
				 " Please correct and try again.")
				 .arg(gt.rules.internalTeachersList[tch]->name)
				 .arg(md)
				 .arg(gt.rules.internalTeachersList[tch]->activitiesForTeacher.count())
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			if(teachersMinHoursDailyMinHours[tch][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0){
				int mh=teachersMinHoursDailyMinHours[tch][MIN_HOURS_DAILY_INDEX_IN_ARRAY];
				
				if(md*mh>nHoursPerTeacher[tch]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min afternoons per week %2 and min hours daily %3"
					 " and it has only %4 working hours - impossible. Please correct and try again.")
					 .arg(gt.rules.internalTeachersList[tch]->name)
					 .arg(md)
					 .arg(mh)
					 .arg(nHoursPerTeacher[tch])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		}
	}*/

	/*for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
		if(teachersMinMorningsPerWeekMinMornings[tch]>=0 || teachersMinAfternoonsPerWeekMinAfternoons[tch]>=0){
			if(teachersMaxGapsPerDayMaxGaps[tch]!=0 && teachersMaxGapsPerWeekMaxGaps[tch]!=0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min mornings or min afternoons per week constraints,"
				 " but not also max 0 gaps per day or per week. Please add max 0 gaps per day or per week for this teacher (or for all teachers) and generate again.")
				 .arg(gt.rules.internalTeachersList[tch]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
	}*/

	return ok;
}


//must be after min hours for students (I think not), students max gaps per day and students max gaps per week (I think not)
bool computeStudentsMinMorningsAfternoonsPerWeek(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMinMorningsPerWeekMinMornings[i]=-1;
		subgroupsMinMorningsPerWeekPercentages[i]=-1;

		subgroupsMinAfternoonsPerWeekMinAfternoons[i]=-1;
		subgroupsMinAfternoonsPerWeekPercentages[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_MORNINGS_PER_WEEK){
			ConstraintStudentsSetMinMorningsPerWeek* tmd=(ConstraintStudentsSetMinMorningsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min mornings per week for students set %1 with"
				 " weight (percentage) below 100. Please make weight 100% and try again")
				 .arg(tmd->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minMorningsPerWeek>gt.rules.nDaysPerWeek/2){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min mornings per week for students set %1 with"
				 " %2 min mornings per week, and the number of working mornings per week is only %3. Please correct and try again")
				 .arg(tmd->students)
				 .arg(tmd->minMorningsPerWeek)
				 .arg(gt.rules.nDaysPerWeek/2),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int sbg : tmd->iSubgroupsList)
				if(subgroupsMinMorningsPerWeekMinMornings[sbg]==-1 || subgroupsMinMorningsPerWeekMinMornings[sbg]<tmd->minMorningsPerWeek){
					subgroupsMinMorningsPerWeekMinMornings[sbg]=tmd->minMorningsPerWeek;
					subgroupsMinMorningsPerWeekPercentages[sbg]=100;
				}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_MORNINGS_PER_WEEK){
			ConstraintStudentsMinMorningsPerWeek* tmd=(ConstraintStudentsMinMorningsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min mornings per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minMorningsPerWeek>gt.rules.nDaysPerWeek/2){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min mornings per week with"
				 " %1 min mornings per week, and the number of working mornings per week is only %2. Please correct and try again")
				 .arg(tmd->minMorningsPerWeek)
				 .arg(gt.rules.nDaysPerWeek/2),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				if(subgroupsMinMorningsPerWeekMinMornings[sbg]==-1 || subgroupsMinMorningsPerWeekMinMornings[sbg]<tmd->minMorningsPerWeek){
					subgroupsMinMorningsPerWeekMinMornings[sbg]=tmd->minMorningsPerWeek;
					subgroupsMinMorningsPerWeekPercentages[sbg]=100;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_AFTERNOONS_PER_WEEK){
			ConstraintStudentsSetMinAfternoonsPerWeek* tmd=(ConstraintStudentsSetMinAfternoonsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min afternoons per week for students set %1 with"
				 " weight (percentage) below 100. Please make weight 100% and try again")
				 .arg(tmd->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minAfternoonsPerWeek>gt.rules.nDaysPerWeek/2){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min afternoons per week for students set %1 with"
				 " %2 min afternoons per week, and the number of working afternoons per week is only %3. Please correct and try again")
				 .arg(tmd->students)
				 .arg(tmd->minAfternoonsPerWeek)
				 .arg(gt.rules.nDaysPerWeek/2),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int sbg : tmd->iSubgroupsList)
				if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]==-1 || subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]<tmd->minAfternoonsPerWeek){
					subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]=tmd->minAfternoonsPerWeek;
					subgroupsMinAfternoonsPerWeekPercentages[sbg]=100;
				}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_AFTERNOONS_PER_WEEK){
			ConstraintStudentsMinAfternoonsPerWeek* tmd=(ConstraintStudentsMinAfternoonsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min afternoons per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minAfternoonsPerWeek>gt.rules.nDaysPerWeek/2){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min afternoons per week with"
				 " %1 min afternoons per week, and the number of working afternoons per week is only %2. Please correct and try again")
				 .arg(tmd->minAfternoonsPerWeek)
				 .arg(gt.rules.nDaysPerWeek/2),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]==-1 || subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]<tmd->minAfternoonsPerWeek){
					subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]=tmd->minAfternoonsPerWeek;
					subgroupsMinAfternoonsPerWeekPercentages[sbg]=100;
				}
			}
		}
	}

	for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
		if(subgroupsMinMorningsPerWeekMinMornings[sbg]>=0){
			int md=subgroupsMinMorningsPerWeekMinMornings[sbg];
			if(md>gt.rules.internalSubgroupsList[sbg]->activitiesForSubgroup.count()){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize because for subgroup %1 you have min mornings per week %2 and it has only %3 activities - impossible."
				 " Please correct and try again.")
				 .arg(gt.rules.internalSubgroupsList[sbg]->name)
				 .arg(md)
				 .arg(gt.rules.internalSubgroupsList[sbg]->activitiesForSubgroup.count())
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			if(subgroupsMinHoursDailyMinHours[sbg][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0){
				int mh=subgroupsMinHoursDailyMinHours[sbg][MIN_HOURS_DAILY_INDEX_IN_ARRAY];
				
				if(md*mh>nHoursPerSubgroup[sbg]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for subgroup %1 you have min mornings per week %2 and min hours daily %3"
					 " and it has only %4 working hours - impossible. Please correct and try again.")
					 .arg(gt.rules.internalSubgroupsList[sbg]->name)
					 .arg(md)
					 .arg(mh)
					 .arg(nHoursPerSubgroup[sbg])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}

			if(subgroupsMinHoursDailyMinHours[sbg][0]>=0 && subgroupsMinHoursDailyMinHours[sbg][0]>subgroupsMinHoursDailyMinHours[sbg][MIN_HOURS_DAILY_INDEX_IN_ARRAY]){
				int mh=subgroupsMinHoursDailyMinHours[sbg][0];
				
				if(md*mh>nHoursPerSubgroup[sbg]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for subgroup %1 you have min mornings per week %2 and min hours per morning %3"
					 " and it has only %4 working hours - impossible. Please correct and try again.")
					 .arg(gt.rules.internalSubgroupsList[sbg]->name)
					 .arg(md)
					 .arg(mh)
					 .arg(nHoursPerSubgroup[sbg])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}

		}

		if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>=0){
			int md=subgroupsMinAfternoonsPerWeekMinAfternoons[sbg];
			if(md>gt.rules.internalSubgroupsList[sbg]->activitiesForSubgroup.count()){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize because for subgroup %1 you have min afternoons per week %2 and it has only %3 activities - impossible."
				 " Please correct and try again.")
				 .arg(gt.rules.internalSubgroupsList[sbg]->name)
				 .arg(md)
				 .arg(gt.rules.internalSubgroupsList[sbg]->activitiesForSubgroup.count())
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			if(subgroupsMinHoursDailyMinHours[sbg][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0){
				int mh=subgroupsMinHoursDailyMinHours[sbg][MIN_HOURS_DAILY_INDEX_IN_ARRAY];
				
				if(md*mh>nHoursPerSubgroup[sbg]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for subgroup %1 you have min afternoons per week %2 and min hours daily %3"
					 " and it has only %4 working hours - impossible. Please correct and try again.")
					 .arg(gt.rules.internalSubgroupsList[sbg]->name)
					 .arg(md)
					 .arg(mh)
					 .arg(nHoursPerSubgroup[sbg])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}

			if(subgroupsMinHoursPerAfternoonMinHours[sbg]>=0 && subgroupsMinHoursPerAfternoonMinHours[sbg]>subgroupsMinHoursDailyMinHours[sbg][MIN_HOURS_DAILY_INDEX_IN_ARRAY]){
				int mh=subgroupsMinHoursPerAfternoonMinHours[sbg];
				
				if(md*mh>nHoursPerSubgroup[sbg]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for subgroup %1 you have min afternoons per week %2 and min hours per afternoon %3"
					 " and it has only %4 working hours - impossible. Please correct and try again.")
					 .arg(gt.rules.internalSubgroupsList[sbg]->name)
					 .arg(md)
					 .arg(mh)
					 .arg(nHoursPerSubgroup[sbg])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		}
	}

	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		if(subgroupsMinMorningsPerWeekMinMornings[sb]>=0){
			if(subgroupsMaxMorningsPerWeekMaxMornings[sb]>=0){
				if(subgroupsMaxMorningsPerWeekMaxMornings[sb]<subgroupsMinMorningsPerWeekMinMornings[sb]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for subgroup %1 you have min mornings per week %2 > max mornings per week %3"
					 " - impossible (min mornings must be <= max mornings). Please correct and try again.")
					 .arg(gt.rules.internalSubgroupsList[sb]->name)
					 .arg(subgroupsMinMorningsPerWeekMinMornings[sb])
					 .arg(subgroupsMaxMorningsPerWeekMaxMornings[sb])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
		}
	}

	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		if(subgroupsMinAfternoonsPerWeekMinAfternoons[sb]>=0){
			if(subgroupsMaxAfternoonsPerWeekMaxAfternoons[sb]>=0){
				if(subgroupsMaxAfternoonsPerWeekMaxAfternoons[sb]<subgroupsMinAfternoonsPerWeekMinAfternoons[sb]){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize because for subgroup %1 you have min afternoons per week %2 > max afternoons per week %3"
					 " - impossible (min afternoons must be <= max afternoons). Please correct and try again.")
					 .arg(gt.rules.internalSubgroupsList[sb]->name)
					 .arg(subgroupsMinAfternoonsPerWeekMinAfternoons[sb])
					 .arg(subgroupsMaxAfternoonsPerWeekMaxAfternoons[sb])
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return false;
				}
			}
		}
	}

	/*for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
		if(teachersMinMorningsPerWeekMinMornings[tch]>=0 || teachersMinAfternoonsPerWeekMinAfternoons[tch]>=0){
			if(teachersMaxGapsPerDayMaxGaps[tch]!=0 && teachersMaxGapsPerWeekMaxGaps[tch]!=0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize because for teacher %1 you have min mornings or min afternoons per week constraints,"
				 " but not also max 0 gaps per day or per week. Please add max 0 gaps per day or per week for this teacher (or for all teachers) and generate again.")
				 .arg(gt.rules.internalTeachersList[tch]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
	}*/

	return ok;
}


bool computeTeachersMaxTwoConsecutiveMornings(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		teachersMaxTwoConsecutiveMorningsPercentage[i]=-1;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_MORNINGS){
			ConstraintTeacherMaxTwoConsecutiveMornings* tmd=(ConstraintTeacherMaxTwoConsecutiveMornings*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max two consecutive mornings for teacher %1 with"
				 " weight (percentage) below 100. Please make weight 100% and try again")
				 .arg(tmd->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			if(teachersMaxTwoConsecutiveMorningsPercentage[tmd->teacher_ID]==-1)
				teachersMaxTwoConsecutiveMorningsPercentage[tmd->teacher_ID]=100;
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_MORNINGS){
			ConstraintTeachersMaxTwoConsecutiveMornings* tmd=(ConstraintTeachersMaxTwoConsecutiveMornings*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max two consecutive mornings with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int tc=0; tc<gt.rules.nInternalTeachers; tc++)
				if(teachersMaxTwoConsecutiveMorningsPercentage[tc]==-1)
					teachersMaxTwoConsecutiveMorningsPercentage[tc]=100;
		}
	}

	return ok;
}

bool computeTeachersMaxTwoConsecutiveAfternoons(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		teachersMaxTwoConsecutiveAfternoonsPercentage[i]=-1;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_AFTERNOONS){
			ConstraintTeacherMaxTwoConsecutiveAfternoons* tmd=(ConstraintTeacherMaxTwoConsecutiveAfternoons*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max two consecutive afternoons for teacher %1 with"
				 " weight (percentage) below 100. Please make weight 100% and try again")
				 .arg(tmd->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			if(teachersMaxTwoConsecutiveAfternoonsPercentage[tmd->teacher_ID]==-1)
				teachersMaxTwoConsecutiveAfternoonsPercentage[tmd->teacher_ID]=100;
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_AFTERNOONS){
			ConstraintTeachersMaxTwoConsecutiveAfternoons* tmd=(ConstraintTeachersMaxTwoConsecutiveAfternoons*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max two consecutive afternoons with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int tc=0; tc<gt.rules.nInternalTeachers; tc++)
				if(teachersMaxTwoConsecutiveAfternoonsPercentage[tc]==-1)
					teachersMaxTwoConsecutiveAfternoonsPercentage[tc]=100;
		}
	}

	return ok;
}

bool computeTeachersMinRestingHoursBetweenMorningAndAfternoon(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMinRestingHoursBetweenMorningAndAfternoonMinHours[i]=-1;
		teachersMinRestingHoursBetweenMorningAndAfternoonPercentages[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON){
			ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon* tmrh=(ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmrh->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min resting hours between morning and afternoon for teacher %1 with"
				 " weight (percentage) below 100. It is only possible to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tmrh->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////
			if(tmrh->minRestingHours>2*gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher min resting hours between morning and afternoon for teacher %1 with"
				 " %2 min resting hours, and the number of working hours per day is only %3. Min resting hours should be at most two times the number of hours per day."
				 " Please correct and try again")
				 .arg(tmrh->teacherName)
				 .arg(tmrh->minRestingHours)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			if(teachersMinRestingHoursBetweenMorningAndAfternoonMinHours[tmrh->teacher_ID]==-1 ||
			 (teachersMinRestingHoursBetweenMorningAndAfternoonMinHours[tmrh->teacher_ID]>=0 && teachersMinRestingHoursBetweenMorningAndAfternoonMinHours[tmrh->teacher_ID]<tmrh->minRestingHours)){
				teachersMinRestingHoursBetweenMorningAndAfternoonMinHours[tmrh->teacher_ID]=tmrh->minRestingHours;
				teachersMinRestingHoursBetweenMorningAndAfternoonPercentages[tmrh->teacher_ID]=100;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON){
			ConstraintTeachersMinRestingHoursBetweenMorningAndAfternoon* tmrh=(ConstraintTeachersMinRestingHoursBetweenMorningAndAfternoon*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmrh->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min resting hours between morning and afternoon with"
				 " weight (percentage) below 100. It is only possible to use 100% weight for such constraints. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////
			if(tmrh->minRestingHours>2*gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers min resting hours between morning and afternoon with"
				 " %1 min resting hours, and the number of working hours per day is only %2. Min resting hours should be at most two times the number of hours per day."
				 " Please correct and try again")
				 .arg(tmrh->minRestingHours)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMinRestingHoursBetweenMorningAndAfternoonMinHours[tch]==-1 ||
				 (teachersMinRestingHoursBetweenMorningAndAfternoonMinHours[tch]>=0 && teachersMinRestingHoursBetweenMorningAndAfternoonMinHours[tch]<tmrh->minRestingHours)){
					teachersMinRestingHoursBetweenMorningAndAfternoonMinHours[tch]=tmrh->minRestingHours;
					teachersMinRestingHoursBetweenMorningAndAfternoonPercentages[tch]=100;
				}
			}
		}
	}

	return ok;
}

bool computeSubgroupsMinRestingHoursBetweenMorningAndAfternoon(QWidget* parent)
{
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMinRestingHoursBetweenMorningAndAfternoonMinHours[i]=-1;
		subgroupsMinRestingHoursBetweenMorningAndAfternoonPercentages[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON){
			ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon* smrh=(ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(smrh->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min resting hours between morning and afternoon for students set %1 with"
				 " weight (percentage) below 100. It is only possible to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(smrh->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////
			if(smrh->minRestingHours>2*gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set min resting hours between morning and afternoon for students set %1 with"
				 " %2 min resting hours, and the number of working hours per day is only %3. Min resting hours should be at most two times the number of hours per day."
				 " Please correct and try again")
				 .arg(smrh->students)
				 .arg(smrh->minRestingHours)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int sbg : qAsConst(smrh->iSubgroupsList)){
				if(subgroupsMinRestingHoursBetweenMorningAndAfternoonMinHours[sbg]==-1 ||
				 (subgroupsMinRestingHoursBetweenMorningAndAfternoonMinHours[sbg]>=0 && subgroupsMinRestingHoursBetweenMorningAndAfternoonMinHours[sbg]<smrh->minRestingHours)){
					subgroupsMinRestingHoursBetweenMorningAndAfternoonMinHours[sbg]=smrh->minRestingHours;
					subgroupsMinRestingHoursBetweenMorningAndAfternoonPercentages[sbg]=100;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON){
			ConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon* smrh=(ConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(smrh->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min resting hours with"
				 " weight (percentage) below 100. It is only possible to use 100% weight for such constraints. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////
			if(smrh->minRestingHours>2*gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students min resting hours between morning and afternoon with"
				 " %1 min resting hours, and the number of working hours per day is only %2. Min resting hours should be at most two times the number of hours per day."
				 " Please correct and try again")
				 .arg(smrh->minRestingHours)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
			//////////

			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				if(subgroupsMinRestingHoursBetweenMorningAndAfternoonMinHours[sbg]==-1 ||
				 (subgroupsMinRestingHoursBetweenMorningAndAfternoonMinHours[sbg]>=0 && subgroupsMinRestingHoursBetweenMorningAndAfternoonMinHours[sbg]<smrh->minRestingHours)){
					subgroupsMinRestingHoursBetweenMorningAndAfternoonMinHours[sbg]=smrh->minRestingHours;
					subgroupsMinRestingHoursBetweenMorningAndAfternoonPercentages[sbg]=100;
				}
			}
		}
	}

	return ok;
}

void computeActivitiesNotOverlapping()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		activitiesNotOverlappingActivities[i].clear();
		activitiesNotOverlappingPercentages[i].clear();
	}
	
	QHash<QPair<int, int>, int> pos;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING){
			ConstraintActivitiesNotOverlapping* no=(ConstraintActivitiesNotOverlapping*)gt.rules.internalTimeConstraintsList[i];

			for(int j=0; j<no->_n_activities; j++){
				int ai1=no->_activities[j];
				for(int k=0; k<no->_n_activities; k++){
					int ai2=no->_activities[k];
					if(ai1!=ai2){
						int t=pos.value(QPair<int, int>(ai1, ai2), -1);
						//int t=activitiesNotOverlappingActivities[ai1].indexOf(ai2);
						if(t>=0){
							assert(t<activitiesNotOverlappingPercentages[ai1].count());
							assert(activitiesNotOverlappingActivities[ai1].at(t)==ai2);
							if(activitiesNotOverlappingPercentages[ai1].at(t) < no->weightPercentage){
								activitiesNotOverlappingPercentages[ai1][t]=no->weightPercentage;
							}
						}
						else{
							pos.insert(QPair<int, int>(ai1, ai2), activitiesNotOverlappingPercentages[ai1].count());
							activitiesNotOverlappingPercentages[ai1].append(no->weightPercentage);
							activitiesNotOverlappingActivities[ai1].append(ai2);
						}
					}
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITY_TAGS_NOT_OVERLAPPING){
			ConstraintActivityTagsNotOverlapping* catno=(ConstraintActivityTagsNotOverlapping*)gt.rules.internalTimeConstraintsList[i];

			for(int i=0; i<catno->activitiesIndicesLists.count(); i++){
				const QList<int>& l1=catno->activitiesIndicesLists.at(i);
				for(int j=0; j<catno->activitiesIndicesLists.count(); j++){
					if(i!=j){
						const QList<int>& l2=catno->activitiesIndicesLists.at(j);
						
						for(int a : qAsConst(l1))
							for(int b : qAsConst(l2)){
								assert(a!=b);
								
								int p=pos.value(QPair<int, int>(a, b), -1);
								if(p>=0){
									assert(p<activitiesNotOverlappingPercentages[a].count());
									assert(activitiesNotOverlappingActivities[a].at(p)==b);
									if(activitiesNotOverlappingPercentages[a].at(p) < catno->weightPercentage){
										activitiesNotOverlappingPercentages[a][p]=catno->weightPercentage;
									}
								}
								else{
									pos.insert(QPair<int, int>(a, b), activitiesNotOverlappingPercentages[a].count());
									activitiesNotOverlappingPercentages[a].append(catno->weightPercentage);
									activitiesNotOverlappingActivities[a].append(b);
								}
							}
					}
				}
			}
		}
	}
}

bool computeActivitiesSameStartingTime(QWidget* parent, QHash<int, int>& reprSameStartingTime, QHash<int, QSet<int>>& reprSameActivitiesSet)
{
	bool reportunder100=true;
	bool report100=true;
	
	bool oktocontinue=true;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		activitiesSameStartingTimeActivities[i].clear();
		activitiesSameStartingTimePercentages[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME){
			ConstraintActivitiesSameStartingTime* sst=(ConstraintActivitiesSameStartingTime*)gt.rules.internalTimeConstraintsList[i];

			for(int j=0; j<sst->_n_activities; j++){
				int ai1=sst->_activities[j];
				for(int k=j+1; k<sst->_n_activities; k++){
					int ai2=sst->_activities[k];
					
					if(sst->weightPercentage==100.0 && activitiesConflictingPercentage[ai1].value(ai2, -1)==100)
						oktocontinue=false;
					
					if(sst->weightPercentage<100.0 && reportunder100 && activitiesConflictingPercentage[ai1].value(ai2, -1)==100){
						QString s;
						
						s+=sst->getDetailedDescription(gt.rules);
						s+="\n";
						s+=GeneratePreTranslate::tr("The constraint is impossible to respect, because there are the activities with id-s %1 and %2 which "
						 "conflict one with another, because they have common students sets or teachers or must be not overlapping. FET will allow you to continue, "
						 "because the weight of this constraint is below 100.0%, "
						 "but anyway most probably you have made a mistake in this constraint, "
						 "so it is recommended to modify it.")
						 .arg(gt.rules.internalActivitiesList[ai1].id)
						 .arg(gt.rules.internalActivitiesList[ai2].id);
					
						int t=GeneratePreReconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 s, GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							reportunder100=false;
					}
					else if(sst->weightPercentage==100.0 && report100 && activitiesConflictingPercentage[ai1].value(ai2, -1)==100){
						QString s;
						
						s+=sst->getDetailedDescription(gt.rules);
						s+="\n";
						s+=GeneratePreTranslate::tr("The constraint is impossible to respect, because there are the activities with id-s %1 and %2 which "
						 "conflict one with another, because they have common students sets or teachers or must be not overlapping. The weight of this constraint is 100.0%, "
						 "so your timetable is impossible. Please correct this constraint.")
						 .arg(gt.rules.internalActivitiesList[ai1].id)
						 .arg(gt.rules.internalActivitiesList[ai2].id);
					
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 s, GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							report100=false;
					}
				}
			}
			
			for(int j=0; j<sst->_n_activities; j++){
				int ai1=sst->_activities[j];
				for(int k=0; k<sst->_n_activities; k++){
					int ai2=sst->_activities[k];
					if(ai1!=ai2){
						int t=activitiesSameStartingTimeActivities[ai1].indexOf(ai2);
						double perc=-1;
						if(t!=-1){
							perc=activitiesSameStartingTimePercentages[ai1].at(t);
							assert(perc>=0 && perc<=100);
						}
						
						if(t==-1 /*|| perc<sst->weightPercentage*/){
							activitiesSameStartingTimeActivities[ai1].append(ai2);
							activitiesSameStartingTimePercentages[ai1].append(sst->weightPercentage);
							assert(activitiesSameStartingTimeActivities[ai1].count()==activitiesSameStartingTimePercentages[ai1].count());
						}
						else if(t>=0 && perc<sst->weightPercentage){
							activitiesSameStartingTimePercentages[ai1][t]=sst->weightPercentage;
						}
					}
				}
			}
		}
		
		
	//added June 2009, FET-5.10.0
	bool reportIndirect=true;
	
	QMultiHash<int, int> adjMatrix;
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME
		 &&gt.rules.internalTimeConstraintsList[i]->weightPercentage==100.0){
			ConstraintActivitiesSameStartingTime* sst=(ConstraintActivitiesSameStartingTime*)gt.rules.internalTimeConstraintsList[i];
			
			for(int i=1; i<sst->_n_activities; i++){
				adjMatrix.insert(sst->_activities[0], sst->_activities[i]);
				adjMatrix.insert(sst->_activities[i], sst->_activities[0]);
			}
		}
	}
	
	QHash<int, int> repr;
	//repr.clear();
	
	QQueue<int> queue;
	
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		int start=i;
		
		if(repr.value(start, -1)==-1){ //not visited
			repr.insert(start, start);
			queue.enqueue(start);
			while(!queue.isEmpty()){
				int crtHead=queue.dequeue();
				assert(repr.value(crtHead, -1)==start);
				QList<int> neighList=adjMatrix.values(crtHead);
				for(int neigh : qAsConst(neighList)){
					if(repr.value(neigh, -1)==-1){
						queue.enqueue(neigh);
						repr.insert(neigh, start);
					}
					else{
						assert(repr.value(neigh, -1)==start);
					}
				}
			}
		}
	}
	
	//faster than below
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		QHash<int, int>& hashConfl=activitiesConflictingPercentage[i];

		QHash<int, int>::const_iterator it=hashConfl.constBegin();
		while(it!=hashConfl.constEnd()){
			//cout<<it.key()<<": "<<it.value()<<endl;
			int j=it.key();
			if(i!=j){
				if(it.value()==100){
					if(repr.value(i)==repr.value(j)){
						oktocontinue=false;
					
						if(reportIndirect){
							QString s=GeneratePreTranslate::tr("You have a set of impossible constraints activities same starting time, considering all the indirect links between"
							 " activities same starting time constraints");
							s+="\n\n";
							s+=GeneratePreTranslate::tr("The activities with ids %1 and %2 must be simultaneous (request determined indirectly), but they have common teachers and/or students sets or must be not overlapping")
							 .arg(gt.rules.internalActivitiesList[i].id).arg(gt.rules.internalActivitiesList[j].id);
						
							int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
							 s, GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
							 1, 0 );
				 	
							if(t==0)
								reportIndirect=false;
						}
					}
				}
			}
			it++;
		}
	}

/*
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		for(int j=i+1; j<gt.rules.nInternalActivities; j++)
			if(repr.value(i) == repr.value(j)){
				if(activitiesConflictingPercentage[i].value(j, -1)==100){
					oktocontinue=false;
					
					if(reportIndirect){
						QString s=GeneratePreTranslate::tr("You have a set of impossible constraints activities same starting time, considering all the indirect links between"
						 " activities same starting time constraints");
						s+="\n\n";
						s+=GeneratePreTranslate::tr("The activities with ids %1 and %2 must be simultaneous (request determined indirectly), but they have common teachers and/or students sets or must be not overlapping")
						 .arg(gt.rules.internalActivitiesList[i].id).arg(gt.rules.internalActivitiesList[j].id);
					
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 s, GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
				 	
						if(t==0)
							reportIndirect=false;
					}
				}
			}*/
	///////////end added 5.10.0, June 2009
	
	QHash<int, QSet<int>> hashSet;
	
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		assert(repr.contains(i));
		int r=repr.value(i);
		hashSet[r].insert(i); //faster
		/*QSet<int> s;
		if(hashSet.contains(r))
			s=hashSet.value(r);
		s.insert(i);
		hashSet.insert(r, s);*/
	}
	
	reprSameStartingTime=repr;
	reprSameActivitiesSet=hashSet;

	return oktocontinue;
}

void computeActivitiesSameStartingHour()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		activitiesSameStartingHourActivities[i].clear();
		activitiesSameStartingHourPercentages[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR){
			ConstraintActivitiesSameStartingHour* sst=(ConstraintActivitiesSameStartingHour*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<sst->_n_activities; j++){
				int ai1=sst->_activities[j];
				for(int k=0; k<sst->_n_activities; k++){
					int ai2=sst->_activities[k];
					if(ai1!=ai2){
						int t=activitiesSameStartingHourActivities[ai1].indexOf(ai2);
						double perc=-1;
						if(t!=-1){
							perc=activitiesSameStartingHourPercentages[ai1].at(t);
							assert(perc>=0 && perc<=100);
						}
						
						if(t==-1 /*|| perc<sst->weightPercentage*/){
							activitiesSameStartingHourActivities[ai1].append(ai2);
							activitiesSameStartingHourPercentages[ai1].append(sst->weightPercentage);
							assert(activitiesSameStartingHourActivities[ai1].count()==activitiesSameStartingHourPercentages[ai1].count());
						}
						else if(t>=0 && perc<sst->weightPercentage){
							activitiesSameStartingHourPercentages[ai1][t]=sst->weightPercentage;
						}
					}
				}
			}
		}
}

void computeActivitiesSameStartingDay()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		activitiesSameStartingDayActivities[i].clear();
		activitiesSameStartingDayPercentages[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY){
			ConstraintActivitiesSameStartingDay* sst=(ConstraintActivitiesSameStartingDay*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<sst->_n_activities; j++){
				int ai1=sst->_activities[j];
				for(int k=0; k<sst->_n_activities; k++){
					int ai2=sst->_activities[k];
					if(ai1!=ai2){
						int t=activitiesSameStartingDayActivities[ai1].indexOf(ai2);
						double perc=-1;
						if(t!=-1){
							perc=activitiesSameStartingDayPercentages[ai1].at(t);
							assert(perc>=0 && perc<=100);
						}
						
						if(t==-1 /*|| perc<sst->weightPercentage*/){
							activitiesSameStartingDayActivities[ai1].append(ai2);
							activitiesSameStartingDayPercentages[ai1].append(sst->weightPercentage);
							assert(activitiesSameStartingDayActivities[ai1].count()==activitiesSameStartingDayPercentages[ai1].count());
						}
						else if(t>=0 && perc<sst->weightPercentage){
							activitiesSameStartingDayPercentages[ai1][t]=sst->weightPercentage;
						}
					}
				}
			}
		}
}

////////////teachers' max gaps
//important also for other purposes
bool computeNHoursPerTeacher(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		nHoursPerTeacher[i]=0;
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		Activity* act=&gt.rules.internalActivitiesList[i];
		for(int j=0; j<act->iTeachersList.count(); j++){
			int tc=act->iTeachersList.at(j);
			nHoursPerTeacher[tc]+=act->duration;
		}
	}
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		if(nHoursPerTeacher[i]>gt.rules.nHoursPerWeek){
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because the number of hours for teacher is %2"
			  " and you have only %3 days x %4 hours in a week.")
			 .arg(gt.rules.internalTeachersList[i]->name)
			 .arg(nHoursPerTeacher[i])
			 .arg(gt.rules.nDaysPerWeek)
			 .arg(gt.rules.nHoursPerDay),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
		 	
			if(t==0)
				return ok;
		}
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		int freeSlots=0;
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			for(int k=0; k<gt.rules.nHoursPerDay; k++)
				if(!teacherNotAvailableDayHour[i][j][k] && !breakDayHour[j][k])
					freeSlots++;
		if(nHoursPerTeacher[i]>freeSlots){
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because the number of hours for teacher is %2"
			  " and you have only %3 free slots from constraints teacher not available and/or break.")
			 .arg(gt.rules.internalTeachersList[i]->name)
			 .arg(nHoursPerTeacher[i])
			 .arg(freeSlots),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
		 	
			if(t==0)
				return ok;
		}
	}
	
	//max days per week has 100% weight
	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		if(teachersMaxDaysPerWeekMaxDays[i]>=0){
			int nd=teachersMaxDaysPerWeekMaxDays[i];
			if(nHoursPerTeacher[i] > nd*gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because the number of hours for teacher is %2"
				  " and you have only %3 allowed days from constraint teacher max days per week x %4 hours in a day."
				  " Probably there is an error in your data")
				 .arg(gt.rules.internalTeachersList[i]->name)
				 .arg(nHoursPerTeacher[i])
				 .arg(nd)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
		 	
				if(t==0)
					return ok;
			}
		}

	//max days per week has 100% weight
	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		for(int i=0; i<gt.rules.nInternalTeachers; i++)
			if(teachersMaxRealDaysPerWeekMaxDays[i]>=0){
				int nd=teachersMaxRealDaysPerWeekMaxDays[i];
				if(nHoursPerTeacher[i] > 2*nd*gt.rules.nHoursPerDay){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because the number of hours for teacher is %2"
					  " and you have only %3 allowed real days from constraint teacher max real days per week x %4 hours in a real day."
					  " Probably there is an error in your data")
					 .arg(gt.rules.internalTeachersList[i]->name)
					 .arg(nHoursPerTeacher[i])
					 .arg(nd)
					 .arg(2*gt.rules.nHoursPerDay),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return ok;
				}
			}
	}

	//max days per week has 100% weight
	//check max days per week together with not available and breaks

	Matrix1D<int> nAllowedSlotsPerDay;
	nAllowedSlotsPerDay.resize(gt.rules.nDaysPerWeek);
	
	Matrix1D<int> dayAvailable;
	dayAvailable.resize(gt.rules.nDaysPerWeek);

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			nAllowedSlotsPerDay[d]=0;
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h])
					nAllowedSlotsPerDay[d]++;
		}

		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			dayAvailable[d]=1;
		if(teachersMaxDaysPerWeekMaxDays[tc]>=0){
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				dayAvailable[d]=0;
		
			assert(teachersMaxDaysPerWeekMaxDays[tc]<=gt.rules.nDaysPerWeek);
			for(int k=0; k<teachersMaxDaysPerWeekMaxDays[tc]; k++){
				int maxPos=-1, maxVal=-1;
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					if(dayAvailable[d]==0)
						if(maxVal<nAllowedSlotsPerDay[d]){
							maxVal=nAllowedSlotsPerDay[d];
							maxPos=d;
						}
				assert(maxPos>=0);
				assert(dayAvailable[maxPos]==0);
				dayAvailable[maxPos]=1;
			}
		}
			
		int total=0;
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			if(dayAvailable[d]==1)
				total+=nAllowedSlotsPerDay[d];
		if(total<nHoursPerTeacher[tc]){
			ok=false;
			
			QString s;
			s=GeneratePreTranslate::tr("Cannot optimize for teacher %1, because of too constrained"
			 " teacher max days per week, teacher not available and/or breaks."
			 " The number of total hours for this teacher is"
			 " %2 and the number of available slots is, considering max days per week and all other constraints, %3.")
			 .arg(gt.rules.internalTeachersList[tc]->name)
			 .arg(nHoursPerTeacher[tc])
			 .arg(total);
			s+="\n\n";
			s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
			
			if(t==0)
				return false;
		}
	}
	
	return ok;
}

bool computeTeachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentages(QWidget* parent)
{
	haveTeachersAfternoonsEarly=false;

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[i]=-1;
		teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		//teachers early
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
			haveTeachersAfternoonsEarly=true;
			ConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHour* se=(ConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHour*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<gt.rules.nInternalTeachers; j++){
				if(teachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[j] < se->weightPercentage)
					teachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[j] = se->weightPercentage;
				if(teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j]==-1 || teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] > se->maxBeginningsAtSecondHour)
					teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] = se->maxBeginningsAtSecondHour;
			}
		}

		//teacher early
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
			haveTeachersAfternoonsEarly=true;
			ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour* se=(ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour*) gt.rules.internalTimeConstraintsList[i];
			int j=se->teacherIndex;
			if(teachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[j] < se->weightPercentage)
				teachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[j] = se->weightPercentage;
			if(teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j]==-1 || teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] > se->maxBeginningsAtSecondHour)
				teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] = se->maxBeginningsAtSecondHour;
		}
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		assert((teachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[i]==-1 &&
		 teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]==-1) ||
		 (teachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0 &&
		 teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0));

		bool okteacher=true;

		if(teachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0 && teachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[i]!=100){
			okteacher=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because you have an afternoons early max beginnings at second hour constraint"
			 " with weight percentage less than 100%. Currently, the algorithm can only"
			 " optimize with not existing constraint early m.b.a.s.h. or existing with 100% weight for it"
			 ". Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalTeachersList[i]->name),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );

			if(t==0)
				break;
		}

		if(teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0
		 && teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>gt.rules.nDaysPerWeek/2){
			okteacher=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because afternoons early max beginnings at second hour constraint"
			 " has max beginnings at second hour %2, and the number of afternoons per week is %3, which is less. It must be that the number of"
			 " afternoons per week must be greater or equal with the max beginnings at second hour\n"
			 "Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalTeachersList[i]->name)
			 .arg(teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i])
			 .arg(gt.rules.nDaysPerWeek/2),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );

			if(t==0)
				break;
		}

		if(!okteacher)
			ok=false;
	}

	return ok;
}

bool computeTeachersMorningsEarlyMaxBeginningsAtSecondHourPercentages(QWidget* parent)
{
	haveTeachersMorningsEarly=false;

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMorningsEarlyMaxBeginningsAtSecondHourPercentage[i]=-1;
		teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		//teachers early
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
			haveTeachersMorningsEarly=true;
			ConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHour* se=(ConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHour*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<gt.rules.nInternalTeachers; j++){
				if(teachersMorningsEarlyMaxBeginningsAtSecondHourPercentage[j] < se->weightPercentage)
					teachersMorningsEarlyMaxBeginningsAtSecondHourPercentage[j] = se->weightPercentage;
				if(teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j]==-1 || teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] > se->maxBeginningsAtSecondHour)
					teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] = se->maxBeginningsAtSecondHour;
			}
		}

		//teacher early
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
			haveTeachersMorningsEarly=true;
			ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour* se=(ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour*) gt.rules.internalTimeConstraintsList[i];
			int j=se->teacherIndex;
			if(teachersMorningsEarlyMaxBeginningsAtSecondHourPercentage[j] < se->weightPercentage)
				teachersMorningsEarlyMaxBeginningsAtSecondHourPercentage[j] = se->weightPercentage;
			if(teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j]==-1 || teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] > se->maxBeginningsAtSecondHour)
				teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] = se->maxBeginningsAtSecondHour;
		}
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		assert((teachersMorningsEarlyMaxBeginningsAtSecondHourPercentage[i]==-1 &&
		 teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]==-1) ||
		 (teachersMorningsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0 &&
		 teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0));

		bool okteacher=true;

		if(teachersMorningsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0 && teachersMorningsEarlyMaxBeginningsAtSecondHourPercentage[i]!=100){
			okteacher=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because you have an mornings early max beginnings at second hour constraint"
			 " with weight percentage less than 100%. Currently, the algorithm can only"
			 " optimize with not existing constraint early m.b.a.s.h. or existing with 100% weight for it"
			 ". Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalTeachersList[i]->name),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );

			if(t==0)
				break;
		}

		if(teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0
		 && teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>gt.rules.nDaysPerWeek/2){
			okteacher=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because mornings early max beginnings at second hour constraint"
			 " has max beginnings at second hour %2, and the number of mornings per week is %3, which is less. It must be that the number of"
			 " mornings per week must be greater or equal with the max beginnings at second hour\n"
			 "Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalTeachersList[i]->name)
			 .arg(teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i])
			 .arg(gt.rules.nDaysPerWeek/2),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );

			if(t==0)
				break;
		}

		if(!okteacher)
			ok=false;
	}

	return ok;
}

bool computeSubgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentages(QWidget* parent)
{
	haveStudentsAfternoonsEarly=false;

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[i]=-1;
		subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		//subgroups early
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
			haveStudentsAfternoonsEarly=true;
			ConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHour* se=(ConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHour*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<gt.rules.nInternalSubgroups; j++){
				if(subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[j] < se->weightPercentage)
					subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[j] = se->weightPercentage;
				if(subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j]==-1 || subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] > se->maxBeginningsAtSecondHour)
					subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] = se->maxBeginningsAtSecondHour;
			}
		}

		//subgroup early
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
			haveStudentsAfternoonsEarly=true;
			ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour* se=(ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour*) gt.rules.internalTimeConstraintsList[i];
			for(int j : se->iSubgroupsList){
				if(subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[j] < se->weightPercentage)
					subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[j] = se->weightPercentage;
				if(subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j]==-1 || subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] > se->maxBeginningsAtSecondHour)
					subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] = se->maxBeginningsAtSecondHour;
			}
		}
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		assert((subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[i]==-1 &&
		 subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]==-1) ||
		 (subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0 &&
		 subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0));

		bool oksubgroup=true;

		if(subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0 && subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[i]!=100){
			oksubgroup=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because you have an afternoons early max beginnings at second hour constraint"
			 " with weight percentage less than 100%. Currently, the algorithm can only"
			 " optimize with not existing constraint early m.b.a.s.h. or existing with 100% weight for it"
			 ". Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );

			if(t==0)
				break;
		}

		if(subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0
		 && subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>gt.rules.nDaysPerWeek/2){
			oksubgroup=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because afternoons early max beginnings at second hour constraint"
			 " has max beginnings at second hour %2, and the number of afternoons per week is %3, which is less. It must be that the number of"
			 " afternoons per week must be greater or equal with the max beginnings at second hour\n"
			 "Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name)
			 .arg(subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i])
			 .arg(gt.rules.nDaysPerWeek/2),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );

			if(t==0)
				break;
		}

		if(!oksubgroup)
			ok=false;
	}

	if(ok){
		for(int i=0; i<gt.rules.nInternalSubgroups; i++){
			if(subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0
			 && subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0
			 && subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i])
				subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i];
		}
	}

	return ok;
}

bool computeSubgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentages(QWidget* parent)
{
	haveStudentsMorningsEarly=false;

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentage[i]=-1;
		subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		//subgroups early
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
			haveStudentsMorningsEarly=true;
			ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour* se=(ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<gt.rules.nInternalSubgroups; j++){
				if(subgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentage[j] < se->weightPercentage)
					subgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentage[j] = se->weightPercentage;
				if(subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j]==-1 || subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] > se->maxBeginningsAtSecondHour)
					subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] = se->maxBeginningsAtSecondHour;
			}
		}

		//subgroup early
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
			haveStudentsMorningsEarly=true;
			ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour* se=(ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour*) gt.rules.internalTimeConstraintsList[i];
			for(int j : se->iSubgroupsList){
				if(subgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentage[j] < se->weightPercentage)
					subgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentage[j] = se->weightPercentage;
				if(subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j]==-1 || subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] > se->maxBeginningsAtSecondHour)
					subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] = se->maxBeginningsAtSecondHour;
			}
		}
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		assert((subgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentage[i]==-1 &&
		 subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]==-1) ||
		 (subgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0 &&
		 subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0));

		bool oksubgroup=true;

		if(subgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0 && subgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentage[i]!=100){
			oksubgroup=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because you have an mornings early max beginnings at second hour constraint"
			 " with weight percentage less than 100%. Currently, the algorithm can only"
			 " optimize with not existing constraint early m.b.a.s.h. or existing with 100% weight for it"
			 ". Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );

			if(t==0)
				break;
		}

		if(subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0
		 && subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>gt.rules.nDaysPerWeek/2){
			oksubgroup=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because mornings early max beginnings at second hour constraint"
			 " has max beginnings at second hour %2, and the number of mornings per week is %3, which is less. It must be that the number of"
			 " mornings per week must be greater or equal with the max beginnings at second hour\n"
			 "Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name)
			 .arg(subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i])
			 .arg(gt.rules.nDaysPerWeek/2),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );

			if(t==0)
				break;
		}

		if(!oksubgroup)
			ok=false;
	}

	if(ok){
		for(int i=0; i<gt.rules.nInternalSubgroups; i++){
			if(subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0
			 && subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0
			 && subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i])
				subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i];

			if(subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0
			 && subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0
			 && subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0
			 && subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]+subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]<subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i])
				subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]=subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]+subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i];
			else if(subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0
			 && subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0
			 && subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]==-1){
				subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]=subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]+subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i];
				subgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentage[i]=100;
			}
		}
	}

	return ok;
}

bool computeTeachersMaxGapsPerWeekPercentage(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalTeachers; j++){
		teachersMaxGapsPerWeekMaxGaps[j]=-1;
		teachersMaxGapsPerWeekPercentage[j]=-1;
	}
	
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK){
			ConstraintTeachersMaxGapsPerWeek* tg=(ConstraintTeachersMaxGapsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max gaps per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK){
			ConstraintTeacherMaxGapsPerWeek* tg=(ConstraintTeacherMaxGapsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max gaps per week with"
				 " weight (percentage) below 100 for teacher %1. Please make weight 100% and try again")
				 .arg(tg->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK){
			ConstraintTeachersMaxGapsPerWeek* tg=(ConstraintTeachersMaxGapsPerWeek*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<gt.rules.nInternalTeachers; j++){
				if(teachersMaxGapsPerWeekMaxGaps[j]==-1
				 ||(teachersMaxGapsPerWeekMaxGaps[j]>=0 && teachersMaxGapsPerWeekMaxGaps[j]>=tg->maxGaps && teachersMaxGapsPerWeekPercentage[j]<=tg->weightPercentage)){
					teachersMaxGapsPerWeekMaxGaps[j]=tg->maxGaps;
					teachersMaxGapsPerWeekPercentage[j]=tg->weightPercentage;
				}
				else if(teachersMaxGapsPerWeekMaxGaps[j]>=0 && teachersMaxGapsPerWeekMaxGaps[j]<=tg->maxGaps && teachersMaxGapsPerWeekPercentage[j]>=tg->weightPercentage){
					//do nothing
				}
				else{
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are two constraints"
					 " of type max gaps per week relating to him, and the weight percentage is higher on the constraint"
					 " with more gaps allowed. You are allowed only to have for each teacher"
					 " the most important constraint with maximum weight percentage and minimum gaps allowed"
					 ". Please modify your data accordingly and try again")
					 .arg(gt.rules.internalTeachersList[j]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK){
			ConstraintTeacherMaxGapsPerWeek* tg=(ConstraintTeacherMaxGapsPerWeek*)gt.rules.internalTimeConstraintsList[i];
		
			int j=tg->teacherIndex;
			if(teachersMaxGapsPerWeekMaxGaps[j]==-1
			 ||(teachersMaxGapsPerWeekMaxGaps[j]>=0 && teachersMaxGapsPerWeekMaxGaps[j]>=tg->maxGaps && teachersMaxGapsPerWeekPercentage[j]<=tg->weightPercentage)){
				teachersMaxGapsPerWeekMaxGaps[j]=tg->maxGaps;
				teachersMaxGapsPerWeekPercentage[j]=tg->weightPercentage;
			}
			else if(teachersMaxGapsPerWeekMaxGaps[j]>=0 && teachersMaxGapsPerWeekMaxGaps[j]<=tg->maxGaps && teachersMaxGapsPerWeekPercentage[j]>=tg->weightPercentage){
				//do nothing
			}
			else{
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are two constraints"
				 " of type max gaps per week relating to him, and the weight percentage is higher on the constraint"
				 " with more gaps allowed. You are allowed only to have for each teacher"
				 " the most important constraint with maximum weight percentage and minimum gaps allowed"
				 ". Please modify your data accordingly and try again")
				 .arg(gt.rules.internalTeachersList[j]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
		 	
				if(t==0)
					return false;
			}
		}
	}
	
	return ok;
}

bool computeTeachersMaxGapsPerDayPercentage(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalTeachers; j++){
		teachersMaxGapsPerDayMaxGaps[j]=-1;
		teachersMaxGapsPerDayPercentage[j]=-1;
	}
	
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY){
			ConstraintTeachersMaxGapsPerDay* tg=(ConstraintTeachersMaxGapsPerDay*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max gaps per day with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY){
			ConstraintTeacherMaxGapsPerDay* tg=(ConstraintTeacherMaxGapsPerDay*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max gaps per day with"
				 " weight (percentage) below 100 for teacher %1. Please make weight 100% and try again")
				 .arg(tg->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY){
			ConstraintTeachersMaxGapsPerDay* tg=(ConstraintTeachersMaxGapsPerDay*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<gt.rules.nInternalTeachers; j++){
				if(teachersMaxGapsPerDayMaxGaps[j]==-1
				 ||(teachersMaxGapsPerDayMaxGaps[j]>=0 && teachersMaxGapsPerDayMaxGaps[j]>=tg->maxGaps && teachersMaxGapsPerDayPercentage[j]<=tg->weightPercentage)){
					teachersMaxGapsPerDayMaxGaps[j]=tg->maxGaps;
					teachersMaxGapsPerDayPercentage[j]=tg->weightPercentage;
				}
				else if(teachersMaxGapsPerDayMaxGaps[j]>=0 && teachersMaxGapsPerDayMaxGaps[j]<=tg->maxGaps && teachersMaxGapsPerDayPercentage[j]>=tg->weightPercentage){
					//do nothing
				}
				else{
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are two constraints"
					 " of type max gaps per day relating to him, and the weight percentage is higher on the constraint"
					 " with more gaps allowed. You are allowed only to have for each teacher"
					 " the most important constraint with maximum weight percentage and minimum gaps allowed"
					 ". Please modify your data accordingly and try again")
					 .arg(gt.rules.internalTeachersList[j]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY){
			ConstraintTeacherMaxGapsPerDay* tg=(ConstraintTeacherMaxGapsPerDay*)gt.rules.internalTimeConstraintsList[i];
		
			int j=tg->teacherIndex;
			if(teachersMaxGapsPerDayMaxGaps[j]==-1
			 ||(teachersMaxGapsPerDayMaxGaps[j]>=0 && teachersMaxGapsPerDayMaxGaps[j]>=tg->maxGaps && teachersMaxGapsPerDayPercentage[j]<=tg->weightPercentage)){
				teachersMaxGapsPerDayMaxGaps[j]=tg->maxGaps;
				teachersMaxGapsPerDayPercentage[j]=tg->weightPercentage;
			}
			else if(teachersMaxGapsPerDayMaxGaps[j]>=0 && teachersMaxGapsPerDayMaxGaps[j]<=tg->maxGaps && teachersMaxGapsPerDayPercentage[j]>=tg->weightPercentage){
				//do nothing
			}
			else{
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are two constraints"
				 " of type max gaps per day relating to him, and the weight percentage is higher on the constraint"
				 " with more gaps allowed. You are allowed only to have for each teacher"
				 " the most important constraint with maximum weight percentage and minimum gaps allowed"
				 ". Please modify your data accordingly and try again")
				 .arg(gt.rules.internalTeachersList[j]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
		 	
				if(t==0)
					return false;
			}
		}
	}
	
	return ok;
}

bool computeTeachersMaxGapsPerMorningAndAfternoonPercentage(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalTeachers; j++){
		teachersMaxGapsPerMorningAndAfternoonMaxGaps[j]=-1;
		teachersMaxGapsPerMorningAndAfternoonPercentage[j]=-1;
	}
	
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_MORNING_AND_AFTERNOON){
			ConstraintTeachersMaxGapsPerMorningAndAfternoon* tg=(ConstraintTeachersMaxGapsPerMorningAndAfternoon*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max gaps per morning and afternoon with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_MORNING_AND_AFTERNOON){
			ConstraintTeacherMaxGapsPerMorningAndAfternoon* tg=(ConstraintTeacherMaxGapsPerMorningAndAfternoon*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max gaps per morning and afternoon with"
				 " weight (percentage) below 100 for teacher %1. Please make weight 100% and try again")
				 .arg(tg->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_MORNING_AND_AFTERNOON){
			ConstraintTeachersMaxGapsPerMorningAndAfternoon* tg=(ConstraintTeachersMaxGapsPerMorningAndAfternoon*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<gt.rules.nInternalTeachers; j++){
				if(teachersMaxGapsPerMorningAndAfternoonMaxGaps[j]==-1
				 ||(teachersMaxGapsPerMorningAndAfternoonMaxGaps[j]>=0 && teachersMaxGapsPerMorningAndAfternoonMaxGaps[j]>=tg->maxGaps && teachersMaxGapsPerMorningAndAfternoonPercentage[j]<=tg->weightPercentage)){
					teachersMaxGapsPerMorningAndAfternoonMaxGaps[j]=tg->maxGaps;
					teachersMaxGapsPerMorningAndAfternoonPercentage[j]=tg->weightPercentage;
				}
				else if(teachersMaxGapsPerMorningAndAfternoonMaxGaps[j]>=0 && teachersMaxGapsPerMorningAndAfternoonMaxGaps[j]<=tg->maxGaps && teachersMaxGapsPerMorningAndAfternoonPercentage[j]>=tg->weightPercentage){
					//do nothing
				}
				else{
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are two constraints"
					 " of type max gaps per morning and afternoon relating to him, and the weight percentage is higher on the constraint"
					 " with more gaps allowed. You are allowed only to have for each teacher"
					 " the most important constraint with maximum weight percentage and minimum gaps allowed"
					 ". Please modify your data accordingly and try again")
					 .arg(gt.rules.internalTeachersList[j]->name),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_MORNING_AND_AFTERNOON){
			ConstraintTeacherMaxGapsPerMorningAndAfternoon* tg=(ConstraintTeacherMaxGapsPerMorningAndAfternoon*)gt.rules.internalTimeConstraintsList[i];
		
			int j=tg->teacherIndex;
			if(teachersMaxGapsPerMorningAndAfternoonMaxGaps[j]==-1
			 ||(teachersMaxGapsPerMorningAndAfternoonMaxGaps[j]>=0 && teachersMaxGapsPerMorningAndAfternoonMaxGaps[j]>=tg->maxGaps && teachersMaxGapsPerMorningAndAfternoonPercentage[j]<=tg->weightPercentage)){
				teachersMaxGapsPerMorningAndAfternoonMaxGaps[j]=tg->maxGaps;
				teachersMaxGapsPerMorningAndAfternoonPercentage[j]=tg->weightPercentage;
			}
			else if(teachersMaxGapsPerMorningAndAfternoonMaxGaps[j]>=0 && teachersMaxGapsPerMorningAndAfternoonMaxGaps[j]<=tg->maxGaps && teachersMaxGapsPerMorningAndAfternoonPercentage[j]>=tg->weightPercentage){
				//do nothing
			}
			else{
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because there are two constraints"
				 " of type max gaps per morning and afternoon relating to him, and the weight percentage is higher on the constraint"
				 " with more gaps allowed. You are allowed only to have for each teacher"
				 " the most important constraint with maximum weight percentage and minimum gaps allowed"
				 ". Please modify your data accordingly and try again")
				 .arg(gt.rules.internalTeachersList[j]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
		 	
				if(t==0)
					return false;
			}
		}
	}
	
	if(ok){
		for(int j=0; j<gt.rules.nInternalTeachers; j++)
			if(teachersMaxGapsPerMorningAndAfternoonMaxGaps[j]>=0){
				if(teachersMaxGapsPerDayMaxGaps[j]==-1 || (teachersMaxGapsPerDayMaxGaps[j]>=0
				 && teachersMaxGapsPerDayMaxGaps[j]>teachersMaxGapsPerMorningAndAfternoonMaxGaps[j])){
					teachersMaxGapsPerDayPercentage[j]=100;
					teachersMaxGapsPerDayMaxGaps[j]=teachersMaxGapsPerMorningAndAfternoonMaxGaps[j];
				}

				if(teachersMaxGapsPerWeekMaxGaps[j]==-1 || (teachersMaxGapsPerWeekMaxGaps[j]>=0
				 && teachersMaxGapsPerWeekMaxGaps[j]>teachersMaxGapsPerMorningAndAfternoonMaxGaps[j]*gt.rules.nDaysPerWeek/2)){
					teachersMaxGapsPerWeekPercentage[j]=100;
					teachersMaxGapsPerWeekMaxGaps[j]=teachersMaxGapsPerMorningAndAfternoonMaxGaps[j]*gt.rules.nDaysPerWeek/2;
				}
			}
	}

	return ok;
}

bool computeTeachersMaxGapsPerRealDayPercentage(QWidget* parent)
{
	bool ok=true;

	haveTeachersMaxGapsPerRealDay=false;

	for(int j=0; j<gt.rules.nInternalTeachers; j++){
		teachersMaxGapsPerRealDayMaxGaps[j]=-1;
		teachersMaxGapsPerRealDayAllowException[j]=true;
		teachersMaxGapsPerRealDayPercentage[j]=-1;

		teachersMaxGapsPerWeekForRealDaysMaxGaps[j]=-1;
		teachersMaxGapsPerWeekForRealDaysPercentage[j]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_REAL_DAY){
			haveTeachersMaxGapsPerRealDay=true;
			ConstraintTeachersMaxGapsPerRealDay* tg=(ConstraintTeachersMaxGapsPerRealDay*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max gaps per real day with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}

		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY){
			haveTeachersMaxGapsPerRealDay=true;
			ConstraintTeacherMaxGapsPerRealDay* tg=(ConstraintTeacherMaxGapsPerRealDay*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max gaps per real day with"
				 " weight (percentage) below 100 for teacher %1. Please make weight 100% and try again")
				 .arg(tg->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}

		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS){
			haveTeachersMaxGapsPerRealDay=true;
			ConstraintTeachersMaxGapsPerWeekForRealDays* tg=(ConstraintTeachersMaxGapsPerWeekForRealDays*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max gaps per week for real days with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}

		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS){
			haveTeachersMaxGapsPerRealDay=true;
			ConstraintTeacherMaxGapsPerWeekForRealDays* tg=(ConstraintTeacherMaxGapsPerWeekForRealDays*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max gaps per week for real days with"
				 " weight (percentage) below 100 for teacher %1. Please make weight 100% and try again")
				 .arg(tg->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_REAL_DAY){
			ConstraintTeachersMaxGapsPerRealDay* tg=(ConstraintTeachersMaxGapsPerRealDay*)gt.rules.internalTimeConstraintsList[i];

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMaxGapsPerRealDayPercentage[tch]==-1){
					teachersMaxGapsPerRealDayPercentage[tch]=100.0;
					teachersMaxGapsPerRealDayMaxGaps[tch]=tg->maxGaps;
					teachersMaxGapsPerRealDayAllowException[tch]=tg->allowOneDayExceptionPlusOne;
				}
				else if(teachersMaxGapsPerRealDayMaxGaps[tch]>tg->maxGaps){
					assert(teachersMaxGapsPerRealDayPercentage[tch]==100.0);
					teachersMaxGapsPerRealDayMaxGaps[tch]=tg->maxGaps;
					teachersMaxGapsPerRealDayAllowException[tch]=tg->allowOneDayExceptionPlusOne;
				}
				else if(teachersMaxGapsPerRealDayMaxGaps[tch]==tg->maxGaps){
					assert(teachersMaxGapsPerRealDayPercentage[tch]==100.0);
					assert(teachersMaxGapsPerRealDayMaxGaps[tch]==tg->maxGaps);
					if(!tg->allowOneDayExceptionPlusOne && teachersMaxGapsPerRealDayAllowException[tch])
						teachersMaxGapsPerRealDayAllowException[tch]=false;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY){
			ConstraintTeacherMaxGapsPerRealDay* tg=(ConstraintTeacherMaxGapsPerRealDay*)gt.rules.internalTimeConstraintsList[i];

			int tch=tg->teacherIndex;

			if(teachersMaxGapsPerRealDayPercentage[tch]==-1){
				teachersMaxGapsPerRealDayPercentage[tch]=100.0;
				teachersMaxGapsPerRealDayMaxGaps[tch]=tg->maxGaps;
				teachersMaxGapsPerRealDayAllowException[tch]=tg->allowOneDayExceptionPlusOne;
			}
			else if(teachersMaxGapsPerRealDayMaxGaps[tch]>tg->maxGaps){
				assert(teachersMaxGapsPerRealDayPercentage[tch]==100.0);
				teachersMaxGapsPerRealDayMaxGaps[tch]=tg->maxGaps;
				teachersMaxGapsPerRealDayAllowException[tch]=tg->allowOneDayExceptionPlusOne;
			}
			else if(teachersMaxGapsPerRealDayMaxGaps[tch]==tg->maxGaps){
				assert(teachersMaxGapsPerRealDayPercentage[tch]==100.0);
				assert(teachersMaxGapsPerRealDayMaxGaps[tch]==tg->maxGaps);
				if(!tg->allowOneDayExceptionPlusOne && teachersMaxGapsPerRealDayAllowException[tch])
					teachersMaxGapsPerRealDayAllowException[tch]=false;
			}
		}

		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS){
			ConstraintTeachersMaxGapsPerWeekForRealDays* tg=(ConstraintTeachersMaxGapsPerWeekForRealDays*)gt.rules.internalTimeConstraintsList[i];

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMaxGapsPerWeekForRealDaysPercentage[tch]==-1){
					teachersMaxGapsPerWeekForRealDaysPercentage[tch]=100.0;
					teachersMaxGapsPerWeekForRealDaysMaxGaps[tch]=tg->maxGaps;
				}
				else if(teachersMaxGapsPerWeekForRealDaysMaxGaps[tch]>tg->maxGaps){
					assert(teachersMaxGapsPerWeekForRealDaysPercentage[tch]==100.0);
					teachersMaxGapsPerWeekForRealDaysMaxGaps[tch]=tg->maxGaps;
				}
			}
		}

		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS){
			ConstraintTeacherMaxGapsPerWeekForRealDays* tg=(ConstraintTeacherMaxGapsPerWeekForRealDays*)gt.rules.internalTimeConstraintsList[i];

			int tch=tg->teacherIndex;

			if(teachersMaxGapsPerWeekForRealDaysPercentage[tch]==-1){
				teachersMaxGapsPerWeekForRealDaysPercentage[tch]=100.0;
				teachersMaxGapsPerWeekForRealDaysMaxGaps[tch]=tg->maxGaps;
			}
			else if(teachersMaxGapsPerWeekForRealDaysMaxGaps[tch]>tg->maxGaps){
				assert(teachersMaxGapsPerWeekForRealDaysPercentage[tch]==100.0);
				teachersMaxGapsPerWeekForRealDaysMaxGaps[tch]=tg->maxGaps;
			}
		}
	}

	for(int j=0; j<gt.rules.nInternalTeachers; j++){
		if(teachersMaxGapsPerRealDayMaxGaps[j]==0 || teachersMaxGapsPerWeekForRealDaysMaxGaps[j]==0){
			teachersMaxGapsPerRealDayMaxGaps[j]=0;
			teachersMaxGapsPerRealDayPercentage[j]=100.0;

			teachersMaxGapsPerWeekForRealDaysMaxGaps[j]=0;
			teachersMaxGapsPerWeekForRealDaysPercentage[j]=100.0;
		}
		else if(teachersMaxGapsPerRealDayMaxGaps[j]>=0 && (teachersMaxGapsPerWeekForRealDaysMaxGaps[j]<0 ||
		 (teachersMaxGapsPerWeekForRealDaysMaxGaps[j]>=0 &&
		 teachersMaxGapsPerWeekForRealDaysMaxGaps[j] > teachersMaxGapsPerRealDayMaxGaps[j]*(gt.rules.nDaysPerWeek/2)))){
			teachersMaxGapsPerWeekForRealDaysMaxGaps[j] = teachersMaxGapsPerRealDayMaxGaps[j]*(gt.rules.nDaysPerWeek/2);
			teachersMaxGapsPerWeekForRealDaysPercentage[j]=100.0;
		}
	}

	return ok;
}

/////////////////

bool computeTeachersConstrainedToZeroGapsPerAfternoon(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalTeachers; j++)
		teacherConstrainedToZeroGapsPerAfternoon[j]=false;

	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_ZERO_GAPS_PER_AFTERNOON){
			ConstraintTeachersMaxZeroGapsPerAfternoon* tg=(ConstraintTeachersMaxZeroGapsPerAfternoon*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max zero gaps per afternoon with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}

		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_ZERO_GAPS_PER_AFTERNOON){
			ConstraintTeacherMaxZeroGapsPerAfternoon* tg=(ConstraintTeacherMaxZeroGapsPerAfternoon*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max zero gaps per afternoon with"
				 " weight (percentage) below 100 for teacher %1. Please make weight 100% and try again")
				 .arg(tg->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}
		}
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_ZERO_GAPS_PER_AFTERNOON){
			ConstraintTeachersMaxZeroGapsPerAfternoon* tg=(ConstraintTeachersMaxZeroGapsPerAfternoon*)gt.rules.internalTimeConstraintsList[i];
			assert(tg->weightPercentage==100);

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++)
				teacherConstrainedToZeroGapsPerAfternoon[tch]=true;
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_ZERO_GAPS_PER_AFTERNOON){
			ConstraintTeacherMaxZeroGapsPerAfternoon* tg=(ConstraintTeacherMaxZeroGapsPerAfternoon*)gt.rules.internalTimeConstraintsList[i];
			assert(tg->weightPercentage==100);

			teacherConstrainedToZeroGapsPerAfternoon[tg->teacherIndex]=true;
		}
	}

	//cout<<"teacherConstrainedToZeroGapsPerAfternoon[0]="<<teacherConstrainedToZeroGapsPerAfternoon[0]<<endl;

	return ok;
}
/////////////////

///////students' max gaps and early (part 1)
//important also for other purposes
bool computeNHoursPerSubgroup(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++)
		nHoursPerSubgroup[i]=0;
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		Activity* act=&gt.rules.internalActivitiesList[i];
		for(int j=0; j<act->iSubgroupsList.count(); j++){
			int sb=act->iSubgroupsList.at(j);
			nHoursPerSubgroup[sb]+=act->duration;
		}
	}
	
	bool ok=true;
	for(int i=0; i<gt.rules.nInternalSubgroups; i++)
		if(nHoursPerSubgroup[i]>gt.rules.nHoursPerWeek){
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because the number of hours for subgroup is %2"
			  " and you have only %3 days x %4 hours in a week.")
			 .arg(gt.rules.internalSubgroupsList[i]->name)
			 .arg(nHoursPerSubgroup[i])
			 .arg(gt.rules.nDaysPerWeek)
			 .arg(gt.rules.nHoursPerDay),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
		 	
			if(t==0)
				return ok;
		}
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		int freeSlots=0;
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			for(int k=0; k<gt.rules.nHoursPerDay; k++)
				if(!subgroupNotAvailableDayHour[i][j][k] && !breakDayHour[j][k])
					freeSlots++;
		if(nHoursPerSubgroup[i]>freeSlots){
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because the number of hours for subgroup is %2"
			  " and you have only %3 free slots from constraints students set not available and/or break.")
			 .arg(gt.rules.internalSubgroupsList[i]->name)
			 .arg(nHoursPerSubgroup[i])
			 .arg(freeSlots),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
		 	
			if(t==0)
				return ok;
		}
	}
	
	//max days per week has 100% weight
	for(int i=0; i<gt.rules.nInternalSubgroups; i++)
		if(subgroupsMaxDaysPerWeekMaxDays[i]>=0){
			int nd=subgroupsMaxDaysPerWeekMaxDays[i];
			if(nHoursPerSubgroup[i] > nd*gt.rules.nHoursPerDay){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because the number of hours for subgroup is %2"
				  " and you have only %3 allowed days from constraint students (set) max days per week x %4 hours in a day."
				  " Probably there is an error in your data")
				 .arg(gt.rules.internalSubgroupsList[i]->name)
				 .arg(nHoursPerSubgroup[i])
				 .arg(nd)
				 .arg(gt.rules.nHoursPerDay),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
		 	
				if(t==0)
					return ok;
			}
		}

	//max days per week has 100% weight
	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		for(int i=0; i<gt.rules.nInternalSubgroups; i++)
			if(subgroupsMaxRealDaysPerWeekMaxDays[i]>=0){
				int nd=subgroupsMaxRealDaysPerWeekMaxDays[i];
				if(nHoursPerSubgroup[i] > 2*nd*gt.rules.nHoursPerDay){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because the number of hours for subgroup is %2"
					  " and you have only %3 allowed real days from constraint students (set) max real days per week x %4 hours in a real day."
					  " Probably there is an error in your data")
					 .arg(gt.rules.internalSubgroupsList[i]->name)
					 .arg(nHoursPerSubgroup[i])
					 .arg(nd)
					 .arg(2*gt.rules.nHoursPerDay),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );

					if(t==0)
						return ok;
				}
			}
	}
	//max days per week has 100% weight
	//check max days per week together with not available and breaks

	Matrix1D<int> nAllowedSlotsPerDay;
	nAllowedSlotsPerDay.resize(gt.rules.nDaysPerWeek);
	
	Matrix1D<int> dayAvailable;
	dayAvailable.resize(gt.rules.nDaysPerWeek);

	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			nAllowedSlotsPerDay[d]=0;
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[sb][d][h])
					nAllowedSlotsPerDay[d]++;
		}

		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			dayAvailable[d]=1;
		if(subgroupsMaxDaysPerWeekMaxDays[sb]>=0){
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				dayAvailable[d]=0;
		
			assert(subgroupsMaxDaysPerWeekMaxDays[sb]<=gt.rules.nDaysPerWeek);
			for(int k=0; k<subgroupsMaxDaysPerWeekMaxDays[sb]; k++){
				int maxPos=-1, maxVal=-1;
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					if(dayAvailable[d]==0)
						if(maxVal<nAllowedSlotsPerDay[d]){
							maxVal=nAllowedSlotsPerDay[d];
							maxPos=d;
						}
				assert(maxPos>=0);
				assert(dayAvailable[maxPos]==0);
				dayAvailable[maxPos]=1;
			}
		}
			
		int total=0;
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			if(dayAvailable[d]==1)
				total+=nAllowedSlotsPerDay[d];
		if(total<nHoursPerSubgroup[sb]){
			ok=false;
				
			QString s;
			s=GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because of too constrained"
			 " students (set) max days per week, students set not available and/or breaks."
			 " The number of total hours for this subgroup is"
			 " %2 and the number of available slots is, considering max days per week and all other constraints, %3.")
			 .arg(gt.rules.internalSubgroupsList[sb]->name)
			 .arg(nHoursPerSubgroup[sb])
			 .arg(total);
			s+="\n\n";
			s+=GeneratePreTranslate::tr("Please modify your data accordingly and try again");
	
			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
				 	
			if(t==0)
				return false;
		}
	}
	
	return ok;
}

bool computeMaxDaysPerWeekForTeachers(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalTeachers; j++){
		teachersMaxDaysPerWeekMaxDays[j]=-1;
		teachersMaxDaysPerWeekWeightPercentages[j]=-1;
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK){
			ConstraintTeacherMaxDaysPerWeek* tn=(ConstraintTeacherMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max days per week with"
				 " weight (percentage) below 100 for teacher %1. Starting with FET version 5.2.17 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tn->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}

			if(teachersMaxDaysPerWeekMaxDays[tn->teacher_ID]==-1 ||
			 (teachersMaxDaysPerWeekMaxDays[tn->teacher_ID]>=0 && teachersMaxDaysPerWeekMaxDays[tn->teacher_ID] > tn->maxDaysPerWeek)){
				teachersMaxDaysPerWeekMaxDays[tn->teacher_ID]=tn->maxDaysPerWeek;
				teachersMaxDaysPerWeekWeightPercentages[tn->teacher_ID]=tn->weightPercentage;
			}
			/*else{
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because it has at least two constraints max days per week"
				 ". Please modify your data correspondingly (leave maximum one constraint of type"
				 " constraint teacher max days per week for each teacher) and try again")
				 .arg(gt.rules.internalTeachersList[tn->teacher_ID]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			
				if(t==0)
					break;
			}*/
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_DAYS_PER_WEEK){
			ConstraintTeachersMaxDaysPerWeek* tn=(ConstraintTeachersMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max days per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			for(int t=0; t<gt.rules.nInternalTeachers; t++){
				if(teachersMaxDaysPerWeekMaxDays[t]==-1 ||
				 (teachersMaxDaysPerWeekMaxDays[t]>=0 && teachersMaxDaysPerWeekMaxDays[t] > tn->maxDaysPerWeek)){
					teachersMaxDaysPerWeekMaxDays[t]=tn->maxDaysPerWeek;
					teachersMaxDaysPerWeekWeightPercentages[t]=tn->weightPercentage;
				}
			}
			/*else{
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because it has at least two constraints max days per week"
				 ". Please modify your data correspondingly (leave maximum one constraint of type"
				 " constraint teacher max days per week for each teacher) and try again")
				 .arg(gt.rules.internalTeachersList[tn->teacher_ID]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			
				if(t==0)
					break;
			}*/
		}
	}
	
	if(ok){
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			teachersWithMaxDaysPerWeekForActivities[i].clear();
		
			Activity* act=&gt.rules.internalActivitiesList[i];
			for(int j=0; j<act->iTeachersList.count(); j++){
				int tch=act->iTeachersList.at(j);
				
				if(teachersMaxDaysPerWeekMaxDays[tch]>=0){
					assert(teachersWithMaxDaysPerWeekForActivities[i].indexOf(tch)==-1);
					teachersWithMaxDaysPerWeekForActivities[i].append(tch);
				}
			}
		}
	}
	
	return ok;
}

bool computeMaxThreeConsecutiveDaysForTeachers(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalTeachers; j++){
		teachersMaxThreeConsecutiveDaysAllowAMAMException[j]=true;
		teachersMaxThreeConsecutiveDaysPercentages[j]=-1;
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_THREE_CONSECUTIVE_DAYS){
			ConstraintTeacherMaxThreeConsecutiveDays* tn=(ConstraintTeacherMaxThreeConsecutiveDays*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max three consecutive days with"
				 " weight (percentage) below 100% for teacher %1. It is only possible to use 100% weight for such constraints."
				 " Please make weight 100% and try again.")
				 .arg(tn->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}

			if(teachersMaxThreeConsecutiveDaysPercentages[tn->teacher_ID]==-1 ||
			 (teachersMaxThreeConsecutiveDaysPercentages[tn->teacher_ID]>=0 &&
			 teachersMaxThreeConsecutiveDaysAllowAMAMException[tn->teacher_ID]==true)){
				teachersMaxThreeConsecutiveDaysPercentages[tn->teacher_ID]=tn->weightPercentage;
				teachersMaxThreeConsecutiveDaysAllowAMAMException[tn->teacher_ID]=tn->allowAMAMException;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_THREE_CONSECUTIVE_DAYS){
			ConstraintTeachersMaxThreeConsecutiveDays* tn=(ConstraintTeachersMaxThreeConsecutiveDays*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max three consecutive days with"
				 " weight (percentage) below 100%. It is only possible to use 100% weight for such constraints. Please make weight 100% and try again."),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}

			for(int t=0; t<gt.rules.nInternalTeachers; t++){
				if(teachersMaxThreeConsecutiveDaysPercentages[t]==-1 ||
				 (teachersMaxThreeConsecutiveDaysPercentages[t]>=0 &&
				 teachersMaxThreeConsecutiveDaysAllowAMAMException[t]==true)){
					teachersMaxThreeConsecutiveDaysPercentages[t]=tn->weightPercentage;
					teachersMaxThreeConsecutiveDaysAllowAMAMException[t]=tn->allowAMAMException;
				}
			}
		}
	}
	
	if(ok){
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			teachersWithMaxThreeConsecutiveDaysForActivities[i].clear();
		
			Activity* act=&gt.rules.internalActivitiesList[i];
			for(int j=0; j<act->iTeachersList.count(); j++){
				int tch=act->iTeachersList.at(j);
				
				if(teachersMaxThreeConsecutiveDaysPercentages[tch]>=0){
					assert(teachersWithMaxThreeConsecutiveDaysForActivities[i].indexOf(tch)==-1);
					teachersWithMaxThreeConsecutiveDaysForActivities[i].append(tch);
				}
			}
		}
	}
	
	return ok;
}

bool computeMaxThreeConsecutiveDaysForStudents(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalSubgroups; j++){
		subgroupsMaxThreeConsecutiveDaysAllowAMAMException[j]=true;
		subgroupsMaxThreeConsecutiveDaysPercentages[j]=-1;
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_THREE_CONSECUTIVE_DAYS){
			ConstraintStudentsSetMaxThreeConsecutiveDays* cn=(ConstraintStudentsSetMaxThreeConsecutiveDays*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set max three consecutive days with"
				 " weight (percentage) below 100% for students set %1. It is only possible to use 100% weight for such constraints."
				 " Please make weight 100% and try again.")
				 .arg(cn->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}

			for(int q=0; q<cn->iSubgroupsList.count(); q++){
				int sbg=cn->iSubgroupsList.at(q);
				if(subgroupsMaxThreeConsecutiveDaysPercentages[sbg]==-1 ||
				 (subgroupsMaxThreeConsecutiveDaysPercentages[sbg]>=0 &&
				 subgroupsMaxThreeConsecutiveDaysAllowAMAMException[sbg]==true)){
					subgroupsMaxThreeConsecutiveDaysPercentages[sbg]=cn->weightPercentage;
					subgroupsMaxThreeConsecutiveDaysAllowAMAMException[sbg]=cn->allowAMAMException;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_THREE_CONSECUTIVE_DAYS){
			ConstraintStudentsMaxThreeConsecutiveDays* cn=(ConstraintStudentsMaxThreeConsecutiveDays*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students max three consecutive days with"
				 " weight (percentage) below 100%. It is only possible to use 100% weight for such constraints. Please make weight 100% and try again."),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}

			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				if(subgroupsMaxThreeConsecutiveDaysPercentages[sbg]==-1 ||
				 (subgroupsMaxThreeConsecutiveDaysPercentages[sbg]>=0 &&
				 subgroupsMaxThreeConsecutiveDaysAllowAMAMException[sbg]==true)){
					subgroupsMaxThreeConsecutiveDaysPercentages[sbg]=cn->weightPercentage;
					subgroupsMaxThreeConsecutiveDaysAllowAMAMException[sbg]=cn->allowAMAMException;
				}
			}
		}
	}
	
	if(ok){
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			subgroupsWithMaxThreeConsecutiveDaysForActivities[i].clear();
		
			Activity* act=&gt.rules.internalActivitiesList[i];
			for(int j=0; j<act->iSubgroupsList.count(); j++){
				int sbg=act->iSubgroupsList.at(j);
				
				if(subgroupsMaxThreeConsecutiveDaysPercentages[sbg]>=0){
					assert(subgroupsWithMaxThreeConsecutiveDaysForActivities[i].indexOf(sbg)==-1);
					subgroupsWithMaxThreeConsecutiveDaysForActivities[i].append(sbg);
				}
			}
		}
	}
	
	return ok;
}

bool computeMaxDaysPerWeekForStudents(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalSubgroups; j++){
		subgroupsMaxDaysPerWeekMaxDays[j]=-1;
		subgroupsMaxDaysPerWeekWeightPercentages[j]=-1;
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK){
			ConstraintStudentsSetMaxDaysPerWeek* cn=(ConstraintStudentsSetMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];
			if(cn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set max days per week with"
				 " weight (percentage) below 100 for students set %1. It is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(cn->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}

			for(int sb : qAsConst(cn->iSubgroupsList)){
				if(subgroupsMaxDaysPerWeekMaxDays[sb]==-1 ||
				 (subgroupsMaxDaysPerWeekMaxDays[sb]>=0 && subgroupsMaxDaysPerWeekMaxDays[sb] > cn->maxDaysPerWeek)){
					subgroupsMaxDaysPerWeekMaxDays[sb]=cn->maxDaysPerWeek;
					subgroupsMaxDaysPerWeekWeightPercentages[sb]=cn->weightPercentage;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_DAYS_PER_WEEK){
			ConstraintStudentsMaxDaysPerWeek* cn=(ConstraintStudentsMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students max days per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				
				if(t==0)
					return false;
			}
			
			for(int s=0; s<gt.rules.nInternalSubgroups; s++){
				if(subgroupsMaxDaysPerWeekMaxDays[s]==-1 ||
				 (subgroupsMaxDaysPerWeekMaxDays[s]>=0 && subgroupsMaxDaysPerWeekMaxDays[s] > cn->maxDaysPerWeek)){
					subgroupsMaxDaysPerWeekMaxDays[s]=cn->maxDaysPerWeek;
					subgroupsMaxDaysPerWeekWeightPercentages[s]=cn->weightPercentage;
				}
			}
		}
	}
	
	if(ok){
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			subgroupsWithMaxDaysPerWeekForActivities[i].clear();
		
			Activity* act=&gt.rules.internalActivitiesList[i];
			for(int j=0; j<act->iSubgroupsList.count(); j++){
				int sb=act->iSubgroupsList.at(j);
				
				if(subgroupsMaxDaysPerWeekMaxDays[sb]>=0){
					assert(subgroupsWithMaxDaysPerWeekForActivities[i].indexOf(sb)==-1);
					subgroupsWithMaxDaysPerWeekForActivities[i].append(sb);
				}
			}
		}
	}
	
	return ok;
}

bool computeMaxRealDaysPerWeekForTeachers(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalTeachers; j++){
		teachersMaxRealDaysPerWeekMaxDays[j]=-1;
		teachersMaxRealDaysPerWeekWeightPercentages[j]=-1;
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_REAL_DAYS_PER_WEEK){
			ConstraintTeacherMaxRealDaysPerWeek* tn=(ConstraintTeacherMaxRealDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];
			
			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max real days per week with"
				 " weight (percentage) below 100 for teacher %1. Starting with FET version 5.2.17 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tn->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			if(teachersMaxRealDaysPerWeekMaxDays[tn->teacher_ID]==-1 ||
			 (teachersMaxRealDaysPerWeekMaxDays[tn->teacher_ID]>=0 && teachersMaxRealDaysPerWeekMaxDays[tn->teacher_ID] > tn->maxDaysPerWeek)){
				teachersMaxRealDaysPerWeekMaxDays[tn->teacher_ID]=tn->maxDaysPerWeek;
				teachersMaxRealDaysPerWeekWeightPercentages[tn->teacher_ID]=tn->weightPercentage;
			}
			/*else{
				ok=false;

				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because it has at least two constraints max days per week"
				 ". Please modify your data correspondingly (leave maximum one constraint of type"
				 " constraint teacher max days per week for each teacher) and try again")
				 .arg(gt.rules.internalTeachersList[tn->teacher_ID]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					break;
			}*/
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_REAL_DAYS_PER_WEEK){
			ConstraintTeachersMaxRealDaysPerWeek* tn=(ConstraintTeachersMaxRealDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max real days per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int t=0; t<gt.rules.nInternalTeachers; t++){
				if(teachersMaxRealDaysPerWeekMaxDays[t]==-1 ||
				 (teachersMaxRealDaysPerWeekMaxDays[t]>=0 && teachersMaxRealDaysPerWeekMaxDays[t] > tn->maxDaysPerWeek)){
					teachersMaxRealDaysPerWeekMaxDays[t]=tn->maxDaysPerWeek;
					teachersMaxRealDaysPerWeekWeightPercentages[t]=tn->weightPercentage;
				}
			}
			/*else{
				ok=false;

				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because it has at least two constraints max days per week"
				 ". Please modify your data correspondingly (leave maximum one constraint of type"
				 " constraint teacher max days per week for each teacher) and try again")
				 .arg(gt.rules.internalTeachersList[tn->teacher_ID]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					break;
			}*/
		}
	}

	if(ok){
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			teachersWithMaxRealDaysPerWeekForActivities[i].clear();

			Activity* act=&gt.rules.internalActivitiesList[i];
			for(int j=0; j<act->iTeachersList.count(); j++){
				int tch=act->iTeachersList.at(j);

				if(teachersMaxRealDaysPerWeekMaxDays[tch]>=0){
					assert(teachersWithMaxRealDaysPerWeekForActivities[i].indexOf(tch)==-1);
					teachersWithMaxRealDaysPerWeekForActivities[i].append(tch);
				}
			}
		}
	}

	return ok;
}

bool computeMaxAfternoonsPerWeekForTeachers(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalTeachers; j++){
		teachersMaxAfternoonsPerWeekMaxAfternoons[j]=-1;
		teachersMaxAfternoonsPerWeekWeightPercentages[j]=-1;
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_AFTERNOONS_PER_WEEK){
			ConstraintTeacherMaxAfternoonsPerWeek* tn=(ConstraintTeacherMaxAfternoonsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max afternoons per week with"
				 " weight (percentage) below 100 for teacher %1. Starting with FET version 5.2.17 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tn->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			if(teachersMaxAfternoonsPerWeekMaxAfternoons[tn->teacher_ID]==-1 ||
			 (teachersMaxAfternoonsPerWeekMaxAfternoons[tn->teacher_ID]>=0 && teachersMaxAfternoonsPerWeekMaxAfternoons[tn->teacher_ID] > tn->maxAfternoonsPerWeek)){
				teachersMaxAfternoonsPerWeekMaxAfternoons[tn->teacher_ID]=tn->maxAfternoonsPerWeek;
				teachersMaxAfternoonsPerWeekWeightPercentages[tn->teacher_ID]=tn->weightPercentage;
			}
			/*else{
				ok=false;

				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because it has at least two constraints max days per week"
				 ". Please modify your data correspondingly (leave maximum one constraint of type"
				 " constraint teacher max days per week for each teacher) and try again")
				 .arg(gt.rules.internalTeachersList[tn->teacher_ID]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					break;
			}*/
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_AFTERNOONS_PER_WEEK){
			ConstraintTeachersMaxAfternoonsPerWeek* tn=(ConstraintTeachersMaxAfternoonsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max afternoons per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int t=0; t<gt.rules.nInternalTeachers; t++){
				if(teachersMaxAfternoonsPerWeekMaxAfternoons[t]==-1 ||
				 (teachersMaxAfternoonsPerWeekMaxAfternoons[t]>=0 && teachersMaxAfternoonsPerWeekMaxAfternoons[t] > tn->maxAfternoonsPerWeek)){
					teachersMaxAfternoonsPerWeekMaxAfternoons[t]=tn->maxAfternoonsPerWeek;
					teachersMaxAfternoonsPerWeekWeightPercentages[t]=tn->weightPercentage;
				}
			}
			/*else{
				ok=false;

				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because it has at least two constraints max days per week"
				 ". Please modify your data correspondingly (leave maximum one constraint of type"
				 " constraint teacher max days per week for each teacher) and try again")
				 .arg(gt.rules.internalTeachersList[tn->teacher_ID]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					break;
			}*/
		}
	}

	if(ok){
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			teachersWithMaxAfternoonsPerWeekForActivities[i].clear();

			Activity* act=&gt.rules.internalActivitiesList[i];
			for(int j=0; j<act->iTeachersList.count(); j++){
				int tch=act->iTeachersList.at(j);

				if(teachersMaxAfternoonsPerWeekMaxAfternoons[tch]>=0){
					assert(teachersWithMaxAfternoonsPerWeekForActivities[i].indexOf(tch)==-1);
					teachersWithMaxAfternoonsPerWeekForActivities[i].append(tch);
				}
			}
		}
	}

	return ok;
}

bool computeMaxMorningsPerWeekForTeachers(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalTeachers; j++){
		teachersMaxMorningsPerWeekMaxMornings[j]=-1;
		teachersMaxMorningsPerWeekWeightPercentages[j]=-1;
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_MORNINGS_PER_WEEK){
			ConstraintTeacherMaxMorningsPerWeek* tn=(ConstraintTeacherMaxMorningsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher max mornings per week with"
				 " weight (percentage) below 100 for teacher %1. Starting with FET version 5.2.17 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tn->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			if(teachersMaxMorningsPerWeekMaxMornings[tn->teacher_ID]==-1 ||
			 (teachersMaxMorningsPerWeekMaxMornings[tn->teacher_ID]>=0 && teachersMaxMorningsPerWeekMaxMornings[tn->teacher_ID] > tn->maxMorningsPerWeek)){
				teachersMaxMorningsPerWeekMaxMornings[tn->teacher_ID]=tn->maxMorningsPerWeek;
				teachersMaxMorningsPerWeekWeightPercentages[tn->teacher_ID]=tn->weightPercentage;
			}
			/*else{
				ok=false;

				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because it has at least two constraints max days per week"
				 ". Please modify your data correspondingly (leave maximum one constraint of type"
				 " constraint teacher max days per week for each teacher) and try again")
				 .arg(gt.rules.internalTeachersList[tn->teacher_ID]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					break;
			}*/
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_MORNINGS_PER_WEEK){
			ConstraintTeachersMaxMorningsPerWeek* tn=(ConstraintTeachersMaxMorningsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers max mornings per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int t=0; t<gt.rules.nInternalTeachers; t++){
				if(teachersMaxMorningsPerWeekMaxMornings[t]==-1 ||
				 (teachersMaxMorningsPerWeekMaxMornings[t]>=0 && teachersMaxMorningsPerWeekMaxMornings[t] > tn->maxMorningsPerWeek)){
					teachersMaxMorningsPerWeekMaxMornings[t]=tn->maxMorningsPerWeek;
					teachersMaxMorningsPerWeekWeightPercentages[t]=tn->weightPercentage;
				}
			}
			/*else{
				ok=false;

				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize for teacher %1, because it has at least two constraints max days per week"
				 ". Please modify your data correspondingly (leave maximum one constraint of type"
				 " constraint teacher max days per week for each teacher) and try again")
				 .arg(gt.rules.internalTeachersList[tn->teacher_ID]->name),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					break;
			}*/
		}
	}

	if(ok){
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			teachersWithMaxMorningsPerWeekForActivities[i].clear();

			Activity* act=&gt.rules.internalActivitiesList[i];
			for(int j=0; j<act->iTeachersList.count(); j++){
				int tch=act->iTeachersList.at(j);

				if(teachersMaxMorningsPerWeekMaxMornings[tch]>=0){
					assert(teachersWithMaxMorningsPerWeekForActivities[i].indexOf(tch)==-1);
					teachersWithMaxMorningsPerWeekForActivities[i].append(tch);
				}
			}
		}
	}

	return ok;
}

bool computeMaxRealDaysPerWeekForStudents(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalSubgroups; j++){
		subgroupsMaxRealDaysPerWeekMaxDays[j]=-1;
		subgroupsMaxRealDaysPerWeekWeightPercentages[j]=-1;
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_REAL_DAYS_PER_WEEK){
			ConstraintStudentsSetMaxRealDaysPerWeek* cn=(ConstraintStudentsSetMaxRealDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];
			if(cn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set max real days per week with"
				 " weight (percentage) below 100 for students set %1. It is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(cn->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int sb : qAsConst(cn->iSubgroupsList)){
				if(subgroupsMaxRealDaysPerWeekMaxDays[sb]==-1 ||
				 (subgroupsMaxRealDaysPerWeekMaxDays[sb]>=0 && subgroupsMaxRealDaysPerWeekMaxDays[sb] > cn->maxDaysPerWeek)){
					subgroupsMaxRealDaysPerWeekMaxDays[sb]=cn->maxDaysPerWeek;
					subgroupsMaxRealDaysPerWeekWeightPercentages[sb]=cn->weightPercentage;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_REAL_DAYS_PER_WEEK){
			ConstraintStudentsMaxRealDaysPerWeek* cn=(ConstraintStudentsMaxRealDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students max real days per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int s=0; s<gt.rules.nInternalSubgroups; s++){
				if(subgroupsMaxRealDaysPerWeekMaxDays[s]==-1 ||
				 (subgroupsMaxRealDaysPerWeekMaxDays[s]>=0 && subgroupsMaxRealDaysPerWeekMaxDays[s] > cn->maxDaysPerWeek)){
					subgroupsMaxRealDaysPerWeekMaxDays[s]=cn->maxDaysPerWeek;
					subgroupsMaxRealDaysPerWeekWeightPercentages[s]=cn->weightPercentage;
				}
			}
		}
	}

	if(ok){
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			subgroupsWithMaxRealDaysPerWeekForActivities[i].clear();

			Activity* act=&gt.rules.internalActivitiesList[i];
			for(int j=0; j<act->iSubgroupsList.count(); j++){
				int sb=act->iSubgroupsList.at(j);

				if(subgroupsMaxRealDaysPerWeekMaxDays[sb]>=0){
					assert(subgroupsWithMaxRealDaysPerWeekForActivities[i].indexOf(sb)==-1);
					subgroupsWithMaxRealDaysPerWeekForActivities[i].append(sb);
				}
			}
		}
	}

	return ok;
}

//2020-06-25
bool computeMaxAfternoonsPerWeekForStudents(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalSubgroups; j++){
		subgroupsMaxAfternoonsPerWeekMaxAfternoons[j]=-1;
		subgroupsMaxAfternoonsPerWeekWeightPercentages[j]=-1;
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_AFTERNOONS_PER_WEEK){
			ConstraintStudentsSetMaxAfternoonsPerWeek* tn=(ConstraintStudentsSetMaxAfternoonsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set max afternoons per week with"
				 " weight (percentage) below 100 for students set %1. Please make weight 100% and try again")
				 .arg(tn->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int sbg : tn->iSubgroupsList)
				if(subgroupsMaxAfternoonsPerWeekMaxAfternoons[sbg]==-1 ||
				 (subgroupsMaxAfternoonsPerWeekMaxAfternoons[sbg]>=0 && subgroupsMaxAfternoonsPerWeekMaxAfternoons[sbg] > tn->maxAfternoonsPerWeek)){
					subgroupsMaxAfternoonsPerWeekMaxAfternoons[sbg]=tn->maxAfternoonsPerWeek;
					subgroupsMaxAfternoonsPerWeekWeightPercentages[sbg]=tn->weightPercentage;
				}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_AFTERNOONS_PER_WEEK){
			ConstraintStudentsMaxAfternoonsPerWeek* tn=(ConstraintStudentsMaxAfternoonsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students max afternoons per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				if(subgroupsMaxAfternoonsPerWeekMaxAfternoons[sbg]==-1 ||
				 (subgroupsMaxAfternoonsPerWeekMaxAfternoons[sbg]>=0 && subgroupsMaxAfternoonsPerWeekMaxAfternoons[sbg] > tn->maxAfternoonsPerWeek)){
					subgroupsMaxAfternoonsPerWeekMaxAfternoons[sbg]=tn->maxAfternoonsPerWeek;
					subgroupsMaxAfternoonsPerWeekWeightPercentages[sbg]=tn->weightPercentage;
				}
			}
		}
	}

	if(ok){
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			subgroupsWithMaxAfternoonsPerWeekForActivities[i].clear();

			Activity* act=&gt.rules.internalActivitiesList[i];
			for(int j=0; j<act->iSubgroupsList.count(); j++){
				int sbg=act->iSubgroupsList.at(j);

				if(subgroupsMaxAfternoonsPerWeekMaxAfternoons[sbg]>=0){
					assert(subgroupsWithMaxAfternoonsPerWeekForActivities[i].indexOf(sbg)==-1);
					subgroupsWithMaxAfternoonsPerWeekForActivities[i].append(sbg);
				}
			}
		}
	}

	return ok;
}

bool computeMaxMorningsPerWeekForStudents(QWidget* parent)
{
	for(int j=0; j<gt.rules.nInternalSubgroups; j++){
		subgroupsMaxMorningsPerWeekMaxMornings[j]=-1;
		subgroupsMaxMorningsPerWeekWeightPercentages[j]=-1;
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_MORNINGS_PER_WEEK){
			ConstraintStudentsSetMaxMorningsPerWeek* tn=(ConstraintStudentsSetMaxMorningsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set max mornings per week with"
				 " weight (percentage) below 100 for students set %1. Please make weight 100% and try again")
				 .arg(tn->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int sbg : tn->iSubgroupsList)
				if(subgroupsMaxMorningsPerWeekMaxMornings[sbg]==-1 ||
				 (subgroupsMaxMorningsPerWeekMaxMornings[sbg]>=0 && subgroupsMaxMorningsPerWeekMaxMornings[sbg] > tn->maxMorningsPerWeek)){
					subgroupsMaxMorningsPerWeekMaxMornings[sbg]=tn->maxMorningsPerWeek;
					subgroupsMaxMorningsPerWeekWeightPercentages[sbg]=tn->weightPercentage;
				}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_MORNINGS_PER_WEEK){
			ConstraintStudentsMaxMorningsPerWeek* tn=(ConstraintStudentsMaxMorningsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students max mornings per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				if(subgroupsMaxMorningsPerWeekMaxMornings[sbg]==-1 ||
				 (subgroupsMaxMorningsPerWeekMaxMornings[sbg]>=0 && subgroupsMaxMorningsPerWeekMaxMornings[sbg] > tn->maxMorningsPerWeek)){
					subgroupsMaxMorningsPerWeekMaxMornings[sbg]=tn->maxMorningsPerWeek;
					subgroupsMaxMorningsPerWeekWeightPercentages[sbg]=tn->weightPercentage;
				}
			}
		}
	}

	if(ok){
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			subgroupsWithMaxMorningsPerWeekForActivities[i].clear();

			Activity* act=&gt.rules.internalActivitiesList[i];
			for(int j=0; j<act->iSubgroupsList.count(); j++){
				int sbg=act->iSubgroupsList.at(j);

				if(subgroupsMaxMorningsPerWeekMaxMornings[sbg]>=0){
					assert(subgroupsWithMaxMorningsPerWeekForActivities[i].indexOf(sbg)==-1);
					subgroupsWithMaxMorningsPerWeekForActivities[i].append(sbg);
				}
			}
		}
	}

	return ok;
}

bool computeSubgroupsEarlyAndMaxGapsPercentages(QWidget* parent) //st max gaps & early - part 2
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]=-1;
		subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]=-1;
		subgroupsMaxGapsPerWeekPercentage[i]=-1;
		subgroupsMaxGapsPerWeekMaxGaps[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		//students early
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
			ConstraintStudentsEarlyMaxBeginningsAtSecondHour* se=(ConstraintStudentsEarlyMaxBeginningsAtSecondHour*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<gt.rules.nInternalSubgroups; j++){
				if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[j] < se->weightPercentage)
					subgroupsEarlyMaxBeginningsAtSecondHourPercentage[j] = se->weightPercentage;
				if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j]==-1 || subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] > se->maxBeginningsAtSecondHour)
					subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] = se->maxBeginningsAtSecondHour;
			}
		}

		//students set early
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR){
			ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour* se=(ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour*) gt.rules.internalTimeConstraintsList[i];
			for(int q=0; q<se->iSubgroupsList.count(); q++){
				int j=se->iSubgroupsList.at(q);
				if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[j] < se->weightPercentage)
					subgroupsEarlyMaxBeginningsAtSecondHourPercentage[j] = se->weightPercentage;
				if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j]==-1 || subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] > se->maxBeginningsAtSecondHour)
					subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[j] = se->maxBeginningsAtSecondHour;
			}
		}

		//students max gaps
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK){
			ConstraintStudentsMaxGapsPerWeek* sg=(ConstraintStudentsMaxGapsPerWeek*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<gt.rules.nInternalSubgroups; j++){ //weight is 100% for all of them
				if(subgroupsMaxGapsPerWeekPercentage[j] < sg->weightPercentage)
					subgroupsMaxGapsPerWeekPercentage[j] = sg->weightPercentage;
				if(subgroupsMaxGapsPerWeekMaxGaps[j]==-1 || subgroupsMaxGapsPerWeekMaxGaps[j] > sg->maxGaps){
					subgroupsMaxGapsPerWeekMaxGaps[j] = sg->maxGaps;
				}
			}
		}

		//students set max gaps
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK){
			ConstraintStudentsSetMaxGapsPerWeek* sg=(ConstraintStudentsSetMaxGapsPerWeek*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<sg->iSubgroupsList.count(); j++){
				int s=sg->iSubgroupsList.at(j);
				
				if(subgroupsMaxGapsPerWeekPercentage[s] < sg->weightPercentage)
					subgroupsMaxGapsPerWeekPercentage[s] = sg->weightPercentage;
				if(subgroupsMaxGapsPerWeekMaxGaps[s]==-1 || subgroupsMaxGapsPerWeekMaxGaps[s] > sg->maxGaps){
					subgroupsMaxGapsPerWeekMaxGaps[s] = sg->maxGaps;
				}
			}
		}
	}
	
	bool ok=true;
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		assert((subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]==-1 &&
		 subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]==-1) ||
		 (subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0 &&
		 subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0));

		assert((subgroupsMaxGapsPerWeekPercentage[i]==-1 &&
		 subgroupsMaxGapsPerWeekMaxGaps[i]==-1) ||
		 (subgroupsMaxGapsPerWeekPercentage[i]>=0 &&
		 subgroupsMaxGapsPerWeekMaxGaps[i]>=0));
	
		bool oksubgroup=true;
		/*if(subgroupsNoGapsPercentage[i]== -1 && subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]==-1 ||
			subgroupsNoGapsPercentage[i]>=0 && subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]==-1 ||
			subgroupsNoGapsPercentage[i]>=0 && subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0
			&& subgroupsNoGapsPercentage[i]==subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i])
				oksubgroup=true;
		else
			oksubgroup=false;*/
			
		if(subgroupsMaxGapsPerWeekPercentage[i]>=0 && subgroupsMaxGapsPerWeekPercentage[i]!=100){
			oksubgroup=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because you have a max gaps constraint"
			 " with weight percentage less than 100%. Currently, the algorithm can only"
			 " optimize with not existing constraint max gaps or existing with 100% weight for it"
			 ". Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
			
			if(t==0)
				break;
		}
		if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0 && subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]!=100){
			oksubgroup=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because you have an early max beginnings at second hour constraint"
			 " with weight percentage less than 100%. Currently, the algorithm can only"
			 " optimize with not existing constraint early m.b.a.s.h. or existing with 100% weight for it"
			 ". Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
			
			if(t==0)
				break;
		}
		/*if(subgroupsNoGapsPercentage[i]== -1 && subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0){
			oksubgroup=false;
			ok=false;

			int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because a students early max beginnings at second hour constraint"
			 " exists for this subgroup, and you have not 'max gaps' requirements for this subgroup. "
			 "The algorithm can 1. optimize with 'early' and 'max gaps'"
			 " having the same weight percentage or 2. only 'max gaps' optimization"
			 " without 'early'. Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name),
			 GeneratePreTranslate::tr("Skip rest of early - max gaps problems"), GeneratePreTranslate::tr("See next incompatibility max gaps - early"), QString(),
			 1, 0 );
			
			if(t==0)
				break;
		}
		if(subgroupsNoGapsPercentage[i]>=0 && subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]>=0
		 && subgroupsNoGapsPercentage[i]!=subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]){
		 	oksubgroup=false;
			ok=false;

			int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because early max beginnings at second hour constraint"
			 " has weight percentage %2, and 'max gaps' constraint has weight percentage %3."
			 ". The algorithm can:"
			 "\n1: Optimize with 'early' and 'max gaps' having the same weight percentage or"
			 "\n2. Only 'max gaps' optimization without 'early'."
			 "\nPlease modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name)
			 .arg(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[i]).
			 arg(subgroupsNoGapsPercentage[i]),
			 GeneratePreTranslate::tr("Skip rest of early - max gaps problems"), GeneratePreTranslate::tr("See next incompatibility max gaps - early"), QString(),
			 1, 0 );
			
			if(t==0)
				break;
		}*/

		if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>=0
		 && subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]>gt.rules.nDaysPerWeek){
		 	oksubgroup=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because early max beginnings at second hour constraint"
			 " has max beginnings at second hour %2, and the number of days per week is %3, which is less. It must be that the number of"
			 " days per week must be greater or equal with the max beginnings at second hour\n"
			 "Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name)
			 .arg(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[i]).
			 arg(gt.rules.nDaysPerWeek),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
			
			if(t==0)
				break;
		}
		
		if(!oksubgroup)
			ok=false;
	}
	
	return ok;
}

bool computeSubgroupsMaxGapsPerDayPercentages(QWidget* parent)
{
	haveStudentsMaxGapsPerDay=false;

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMaxGapsPerDayPercentage[i]=-1;
		subgroupsMaxGapsPerDayMaxGaps[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_GAPS_PER_DAY){
			haveStudentsMaxGapsPerDay=true;
			ConstraintStudentsMaxGapsPerDay* sg=(ConstraintStudentsMaxGapsPerDay*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<gt.rules.nInternalSubgroups; j++){ //weight is 100% for all of them
				if(subgroupsMaxGapsPerDayPercentage[j] < sg->weightPercentage)
					subgroupsMaxGapsPerDayPercentage[j] = sg->weightPercentage;
				if(subgroupsMaxGapsPerDayMaxGaps[j]==-1 || subgroupsMaxGapsPerDayMaxGaps[j] > sg->maxGaps){
					subgroupsMaxGapsPerDayMaxGaps[j] = sg->maxGaps;
				}
			}
		}

		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY){
			haveStudentsMaxGapsPerDay=true;
			ConstraintStudentsSetMaxGapsPerDay* sg=(ConstraintStudentsSetMaxGapsPerDay*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<sg->iSubgroupsList.count(); j++){
				int s=sg->iSubgroupsList.at(j);
				
				if(subgroupsMaxGapsPerDayPercentage[s] < sg->weightPercentage)
					subgroupsMaxGapsPerDayPercentage[s] = sg->weightPercentage;
				if(subgroupsMaxGapsPerDayMaxGaps[s]==-1 || subgroupsMaxGapsPerDayMaxGaps[s] > sg->maxGaps){
					subgroupsMaxGapsPerDayMaxGaps[s] = sg->maxGaps;
				}
			}
		}
	}
	
	bool ok=true;
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		assert((subgroupsMaxGapsPerDayPercentage[i]==-1 &&
		 subgroupsMaxGapsPerDayMaxGaps[i]==-1) ||
		 (subgroupsMaxGapsPerDayPercentage[i]>=0 &&
		 subgroupsMaxGapsPerDayMaxGaps[i]>=0));
	
		bool oksubgroup=true;

		if(subgroupsMaxGapsPerDayPercentage[i]>=0 && subgroupsMaxGapsPerDayPercentage[i]!=100){
			oksubgroup=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because you have a max gaps constraint"
			 " with weight percentage less than 100%. Currently, the algorithm can only"
			 " optimize with not existing constraint max gaps or existing with 100% weight for it"
			 ". Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
			
			if(t==0)
				break;
		}

		if(!oksubgroup)
			ok=false;
	}

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		if(subgroupsMaxGapsPerDayMaxGaps[i]>=0){
			int mgw=gt.rules.nDaysPerWeek*subgroupsMaxGapsPerDayMaxGaps[i];
			assert(mgw>=0);
			if(subgroupsMaxGapsPerWeekMaxGaps[i]==-1 || (subgroupsMaxGapsPerWeekMaxGaps[i]>=0 && subgroupsMaxGapsPerWeekMaxGaps[i]>mgw)){
				subgroupsMaxGapsPerWeekMaxGaps[i]=mgw;
				subgroupsMaxGapsPerWeekPercentage[i]=100.0;
			}
		}
	}
	
	return ok;
}

bool computeSubgroupsMaxGapsPerRealDayPercentages(QWidget* parent)
{
	haveStudentsMaxGapsPerRealDay=false;

	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMaxGapsPerRealDayPercentage[i]=-1;
		subgroupsMaxGapsPerRealDayMaxGaps[i]=-1;

		subgroupsMaxGapsPerWeekForRealDaysPercentage[i]=-1;
		subgroupsMaxGapsPerWeekForRealDaysMaxGaps[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_GAPS_PER_REAL_DAY){
			haveStudentsMaxGapsPerRealDay=true;
			ConstraintStudentsMaxGapsPerRealDay* sg=(ConstraintStudentsMaxGapsPerRealDay*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<gt.rules.nInternalSubgroups; j++){ //weight is 100% for all of them
				if(subgroupsMaxGapsPerRealDayPercentage[j] < sg->weightPercentage)
					subgroupsMaxGapsPerRealDayPercentage[j] = sg->weightPercentage;
				if(subgroupsMaxGapsPerRealDayMaxGaps[j]==-1 || subgroupsMaxGapsPerRealDayMaxGaps[j] > sg->maxGaps){
					subgroupsMaxGapsPerRealDayMaxGaps[j] = sg->maxGaps;
				}
			}
		}

		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_REAL_DAY){
			haveStudentsMaxGapsPerRealDay=true;
			ConstraintStudentsSetMaxGapsPerRealDay* sg=(ConstraintStudentsSetMaxGapsPerRealDay*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<sg->iSubgroupsList.count(); j++){
				int s=sg->iSubgroupsList.at(j);

				if(subgroupsMaxGapsPerRealDayPercentage[s] < sg->weightPercentage)
					subgroupsMaxGapsPerRealDayPercentage[s] = sg->weightPercentage;
				if(subgroupsMaxGapsPerRealDayMaxGaps[s]==-1 || subgroupsMaxGapsPerRealDayMaxGaps[s] > sg->maxGaps){
					subgroupsMaxGapsPerRealDayMaxGaps[s] = sg->maxGaps;
				}
			}
		}

		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS){
			haveStudentsMaxGapsPerRealDay=true;
			ConstraintStudentsMaxGapsPerWeekForRealDays* sg=(ConstraintStudentsMaxGapsPerWeekForRealDays*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<gt.rules.nInternalSubgroups; j++){ //weight is 100% for all of them
				if(subgroupsMaxGapsPerWeekForRealDaysPercentage[j] < sg->weightPercentage)
					subgroupsMaxGapsPerWeekForRealDaysPercentage[j] = sg->weightPercentage;
				if(subgroupsMaxGapsPerWeekForRealDaysMaxGaps[j]==-1 || subgroupsMaxGapsPerWeekForRealDaysMaxGaps[j] > sg->maxGaps){
					subgroupsMaxGapsPerWeekForRealDaysMaxGaps[j] = sg->maxGaps;
				}
			}
		}

		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS){
			haveStudentsMaxGapsPerRealDay=true;
			ConstraintStudentsSetMaxGapsPerWeekForRealDays* sg=(ConstraintStudentsSetMaxGapsPerWeekForRealDays*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<sg->iSubgroupsList.count(); j++){
				int s=sg->iSubgroupsList.at(j);

				if(subgroupsMaxGapsPerWeekForRealDaysPercentage[s] < sg->weightPercentage)
					subgroupsMaxGapsPerWeekForRealDaysPercentage[s] = sg->weightPercentage;
				if(subgroupsMaxGapsPerWeekForRealDaysMaxGaps[s]==-1 || subgroupsMaxGapsPerWeekForRealDaysMaxGaps[s] > sg->maxGaps){
					subgroupsMaxGapsPerWeekForRealDaysMaxGaps[s] = sg->maxGaps;
				}
			}
		}
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		assert((subgroupsMaxGapsPerRealDayPercentage[i]==-1 && subgroupsMaxGapsPerRealDayMaxGaps[i]==-1) ||
		 (subgroupsMaxGapsPerRealDayPercentage[i]>=0 && subgroupsMaxGapsPerRealDayMaxGaps[i]>=0));

		assert((subgroupsMaxGapsPerWeekForRealDaysPercentage[i]==-1 && subgroupsMaxGapsPerWeekForRealDaysMaxGaps[i]==-1) ||
		 (subgroupsMaxGapsPerWeekForRealDaysPercentage[i]>=0 && subgroupsMaxGapsPerWeekForRealDaysMaxGaps[i]>=0));

		bool oksubgroup=true;

		if((subgroupsMaxGapsPerRealDayPercentage[i]>=0 && subgroupsMaxGapsPerRealDayPercentage[i]!=100) ||
		 (subgroupsMaxGapsPerWeekForRealDaysPercentage[i]>=0 && subgroupsMaxGapsPerWeekForRealDaysPercentage[i]!=100)){
			oksubgroup=false;
			ok=false;

			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
			 GeneratePreTranslate::tr("Cannot optimize for subgroup %1, because you have a max gaps constraint"
			 " with weight percentage less than 100%. Currently, the algorithm can only"
			 " optimize with not existing constraint max gaps or existing with 100% weight for it"
			 ". Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name),
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );

			if(t==0)
				break;
		}

		if(!oksubgroup)
			ok=false;
	}

	for(int j=0; j<gt.rules.nInternalSubgroups; j++){
		if(subgroupsMaxGapsPerRealDayMaxGaps[j]==0 || subgroupsMaxGapsPerWeekForRealDaysMaxGaps[j]==0){
			subgroupsMaxGapsPerRealDayMaxGaps[j]=0;
			subgroupsMaxGapsPerRealDayPercentage[j]=100.0;

			subgroupsMaxGapsPerWeekForRealDaysMaxGaps[j]=0;
			subgroupsMaxGapsPerWeekForRealDaysPercentage[j]=100.0;
		}
		else if(subgroupsMaxGapsPerRealDayMaxGaps[j]>=0 && (subgroupsMaxGapsPerWeekForRealDaysMaxGaps[j]<0 ||
		 (subgroupsMaxGapsPerWeekForRealDaysMaxGaps[j]>=0 &&
		 subgroupsMaxGapsPerWeekForRealDaysMaxGaps[j] > subgroupsMaxGapsPerRealDayMaxGaps[j]*(gt.rules.nDaysPerWeek/2)))){
			subgroupsMaxGapsPerWeekForRealDaysMaxGaps[j] = subgroupsMaxGapsPerRealDayMaxGaps[j]*(gt.rules.nDaysPerWeek/2);
			subgroupsMaxGapsPerWeekForRealDaysPercentage[j]=100.0;
		}
	}

	return ok;
}

bool computeNotAllowedTimesPercentages(QWidget* parent)
{
	Matrix1D<bool> allowed;
	allowed.resize(gt.rules.nHoursPerWeek);

	bool ok=true;

	assert(gt.rules.internalStructureComputed);

	breakDayHour.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	//BREAK
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int k=0; k<gt.rules.nHoursPerDay; k++)
			breakDayHour[j][k]=false;
	
	subgroupNotAvailableDayHour.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	//STUDENTS SET NOT AVAILABLE
	for(int i=0; i<gt.rules.nInternalSubgroups; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			for(int k=0; k<gt.rules.nHoursPerDay; k++)
				subgroupNotAvailableDayHour[i][j][k]=false;
	
	//used in students timetable view time horizontal dialog
	studentsSetNotAvailableDayHour.clear();
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		TimeConstraint* ctr=gt.rules.internalTimeConstraintsList[i];
		if(ctr->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES){
			ConstraintStudentsSetNotAvailableTimes* csna=(ConstraintStudentsSetNotAvailableTimes*)ctr;
			assert(csna->active);
			
			assert(gt.rules.studentsHash.contains(csna->students));
			
			assert(!studentsSetNotAvailableDayHour.contains(csna->students));
			QSet<QPair<int, int>> mySet;
			for(int j=0; j<csna->days.count(); j++){
				int d=csna->days.at(j);
				int h=csna->hours.at(j);
				assert(!mySet.contains(QPair<int, int>(d,h)));
				mySet.insert(QPair<int, int>(d,h));
			}
			studentsSetNotAvailableDayHour.insert(csna->students, mySet);
		}
	}
	
	teacherNotAvailableDayHour.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	//TEACHER NOT AVAILABLE
	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			for(int k=0; k<gt.rules.nHoursPerDay; k++)
				teacherNotAvailableDayHour[i][j][k]=false;
	
	notAllowedTimesPercentages.resize(gt.rules.nInternalActivities, gt.rules.nHoursPerWeek);
	//improvement by Volker Dirr (late activities):
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		Activity* act=&gt.rules.internalActivitiesList[i];
		for(int j=0; j<gt.rules.nHoursPerWeek; j++){
			int h=j/gt.rules.nDaysPerWeek;
			if(h+act->duration <= gt.rules.nHoursPerDay)
				notAllowedTimesPercentages[i][j]=-1;
			else
				notAllowedTimesPercentages[i][j]=100;
		}
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		//TEACHER not available
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES){
			ConstraintTeacherNotAvailableTimes* tn=(ConstraintTeacherNotAvailableTimes*)gt.rules.internalTimeConstraintsList[i];
			for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				for(int ti=0; ti<act->iTeachersList.count(); ti++)
					if(act->iTeachersList.at(ti)==tn->teacher_ID){
						assert(tn->days.count()==tn->hours.count());
						for(int kk=0; kk<tn->days.count(); kk++){
							int d=tn->days.at(kk);
							int h=tn->hours.at(kk);
							
							for(int hh=max(0, h-act->duration+1); hh<=h; hh++)
								if(notAllowedTimesPercentages[ai][d+hh*gt.rules.nDaysPerWeek]<tn->weightPercentage)
									notAllowedTimesPercentages[ai][d+hh*gt.rules.nDaysPerWeek]=tn->weightPercentage;
						}
						//break; //search no more for teacher -- careful with that
					}
			}

			if(tn->weightPercentage!=100){
				ok=false;

				GeneratePreIrreconcilableMessage::mediumInformation(parent, GeneratePreTranslate::tr("FET warning"),
					GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
					"teacher not available with weight percentage less than 100% for teacher %1. Currently, FET can only optimize with "
					"constraints teacher not available with 100% weight (or no constraint). Please "
					"modify your data accordingly and try again.").arg(tn->teacher));
		
				return ok;
			}
			else{
				assert(tn->weightPercentage==100);
				assert(tn->days.count()==tn->hours.count());
				for(int kk=0; kk<tn->days.count(); kk++){
					int d=tn->days.at(kk);
					int h=tn->hours.at(kk);

					teacherNotAvailableDayHour[tn->teacher_ID][d][h]=true;
				}
			}
		}

		//STUDENTS SET not available
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES){
			ConstraintStudentsSetNotAvailableTimes* sn=(ConstraintStudentsSetNotAvailableTimes*)gt.rules.internalTimeConstraintsList[i];
			for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				for(int sg=0; sg<act->iSubgroupsList.count(); sg++)
					for(int j=0; j<sn->iSubgroupsList.count(); j++){
						if(act->iSubgroupsList.at(sg)==sn->iSubgroupsList.at(j)){
							assert(sn->days.count()==sn->hours.count());
							for(int kk=0; kk<sn->days.count(); kk++){
								int d=sn->days.at(kk);
								int h=sn->hours.at(kk);

								for(int hh=max(0, h-act->duration+1); hh<=h; hh++)
									if(notAllowedTimesPercentages[ai][d+hh*gt.rules.nDaysPerWeek]<sn->weightPercentage)
										notAllowedTimesPercentages[ai][d+hh*gt.rules.nDaysPerWeek]=sn->weightPercentage;
							}
							//break; //search no more for subgroup - this can bring an improvement in precalculation
							//but needs attention
						}
					}
			}

			if(sn->weightPercentage!=100){
				ok=false;

				GeneratePreIrreconcilableMessage::mediumInformation(parent, GeneratePreTranslate::tr("FET warning"),
					GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
					"students set not available with weight percentage less than 100% for students set %1. Currently, FET can only optimize with "
					"constraints students set not available with 100% weight (or no constraint). Please "
					"modify your data accordingly and try again.").arg(sn->students));
		
				return ok;
			}
			else{
				assert(sn->weightPercentage==100);
				for(int q=0; q<sn->iSubgroupsList.count(); q++){
					int ss=sn->iSubgroupsList.at(q);
					assert(sn->days.count()==sn->hours.count());
					for(int kk=0; kk<sn->days.count(); kk++){
						int d=sn->days.at(kk);
						int h=sn->hours.at(kk);

						subgroupNotAvailableDayHour[ss][d][h]=true;
					}
				}
			}
		}
		
		//BREAK
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_BREAK_TIMES){
			ConstraintBreakTimes* br=(ConstraintBreakTimes*)gt.rules.internalTimeConstraintsList[i];
			for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
				Activity* act=&gt.rules.internalActivitiesList[ai];

				assert(br->days.count()==br->hours.count());
				for(int kk=0; kk<br->days.count(); kk++){
					int d=br->days.at(kk);
					int h=br->hours.at(kk);

					for(int hh=max(0, h-act->duration+1); hh<=h; hh++)
						if(notAllowedTimesPercentages[ai][d+hh*gt.rules.nDaysPerWeek]<br->weightPercentage)
							notAllowedTimesPercentages[ai][d+hh*gt.rules.nDaysPerWeek]=br->weightPercentage;
				}
			}

			if(br->weightPercentage!=100){
				ok=false;

				GeneratePreIrreconcilableMessage::mediumInformation(parent, GeneratePreTranslate::tr("FET warning"),
					GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
					"break with weight percentage less than 100%. Currently, FET can only optimize with "
					"constraints break with 100% weight (or no constraint). Please "
					"modify your data accordingly and try again."));
		
				return ok;
			}
			else{
				assert(br->weightPercentage==100);

				assert(br->days.count()==br->hours.count());
				for(int kk=0; kk<br->days.count(); kk++){
					int d=br->days.at(kk);
					int h=br->hours.at(kk);

					breakDayHour[d][h]=true;
				}
			}
		}

		//ACTIVITY preferred starting time
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
			ConstraintActivityPreferredStartingTime* ap=(ConstraintActivityPreferredStartingTime*)gt.rules.internalTimeConstraintsList[i];
			
			if(ap->day>=0 && ap->hour>=0){
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					for(int h=0; h<gt.rules.nHoursPerDay; h++)
						if(d!=ap->day || h!=ap->hour)
							if(notAllowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]<ap->weightPercentage)
								notAllowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]=ap->weightPercentage;
			}
			else if(ap->day>=0){
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					for(int h=0; h<gt.rules.nHoursPerDay; h++)
						if(d!=ap->day)
							if(notAllowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]<ap->weightPercentage)
								notAllowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]=ap->weightPercentage;
			}
			else if(ap->hour>=0){
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					for(int h=0; h<gt.rules.nHoursPerDay; h++)
						if(h!=ap->hour)
							if(notAllowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]<ap->weightPercentage)
								notAllowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]=ap->weightPercentage;
			}
			else{
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
					"activity preferred starting time with no day nor hour selected (for activity with id=%1). "
					"Please modify your data accordingly (remove or edit constraint) and try again.")
					.arg(gt.rules.internalActivitiesList[ap->activityIndex].id),
					GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				1, 0 );

				if(t==0)
					break;
				//assert(0);
			}
		}

		//ACTIVITY preferred day
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITY_PREFERRED_DAY){
			ConstraintActivityPreferredDay* ap=(ConstraintActivityPreferredDay*)gt.rules.internalTimeConstraintsList[i];
			
			if(ap->day>=0){
				for(int d=0; d<gt.rules.nDaysPerWeek; d++)
					for(int h=0; h<gt.rules.nHoursPerDay; h++)
						if(d!=ap->day)
							if(notAllowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]<ap->weightPercentage)
								notAllowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]=ap->weightPercentage;
			}
			else{
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
					"activity preferred day with no day selected (for activity with id=%1). "
					"Please modify your data accordingly (remove or edit constraint) and try again.")
					.arg(gt.rules.internalActivitiesList[ap->activityIndex].id),
					GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				1, 0 );

				if(t==0)
					break;
				//assert(0);
			}
		}

		//ACTIVITY preferred starting times
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES){
			ConstraintActivityPreferredStartingTimes* ap=(ConstraintActivityPreferredStartingTimes*)gt.rules.internalTimeConstraintsList[i];
			
			int ai=ap->activityIndex;
			
			for(int k=0; k<gt.rules.nHoursPerWeek; k++)
				allowed[k]=false;
			
			for(int m=0; m<ap->nPreferredStartingTimes_L; m++){
				int d=ap->days_L[m];
				int h=ap->hours_L[m];
				
				if(d>=0 && h>=0){
					assert(d>=0 && h>=0);
					allowed[d+h*gt.rules.nDaysPerWeek]=true;
				}
				else if(d>=0){
					for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
						allowed[d+hh*gt.rules.nDaysPerWeek]=true;
				}
				else if(h>=0){
					for(int dd=0; dd<gt.rules.nDaysPerWeek; dd++)
						allowed[dd+h*gt.rules.nDaysPerWeek]=true;
				}
			}
			
			for(int k=0; k<gt.rules.nHoursPerWeek; k++)
				if(!allowed[k])
					if(notAllowedTimesPercentages[ai][k] < ap->weightPercentage)
						notAllowedTimesPercentages[ai][k] = ap->weightPercentage;
		}
		
		//ACTIVITIES preferred starting times
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES){
			ConstraintActivitiesPreferredStartingTimes* ap=(ConstraintActivitiesPreferredStartingTimes*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<ap->nActivities; j++){
				int ai=ap->activitiesIndices[j];
				
				for(int k=0; k<gt.rules.nHoursPerWeek; k++)
					allowed[k]=false;
					
				for(int m=0; m<ap->nPreferredStartingTimes_L; m++){
					int d=ap->days_L[m];
					int h=ap->hours_L[m];
					assert(d>=0 && h>=0);
					allowed[d+h*gt.rules.nDaysPerWeek]=true;
				}
				
				for(int k=0; k<gt.rules.nHoursPerWeek; k++)
					if(!allowed[k])
						if(notAllowedTimesPercentages[ai][k] < ap->weightPercentage)
							notAllowedTimesPercentages[ai][k] = ap->weightPercentage;
			}
		}
		
		//subactivities preferred starting times
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES){
			ConstraintSubactivitiesPreferredStartingTimes* ap=(ConstraintSubactivitiesPreferredStartingTimes*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<ap->nActivities; j++){
				int ai=ap->activitiesIndices[j];
				
				for(int k=0; k<gt.rules.nHoursPerWeek; k++)
					allowed[k]=false;
					
				for(int m=0; m<ap->nPreferredStartingTimes_L; m++){
					int d=ap->days_L[m];
					int h=ap->hours_L[m];
					assert(d>=0 && h>=0);
					allowed[d+h*gt.rules.nDaysPerWeek]=true;
				}
				
				for(int k=0; k<gt.rules.nHoursPerWeek; k++)
					if(!allowed[k])
						if(notAllowedTimesPercentages[ai][k] < ap->weightPercentage)
							notAllowedTimesPercentages[ai][k] = ap->weightPercentage;
			}
		}
		
		//ACTIVITY preferred time slots
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS){
			ConstraintActivityPreferredTimeSlots* ap=(ConstraintActivityPreferredTimeSlots*)gt.rules.internalTimeConstraintsList[i];
			
			int ai=ap->p_activityIndex;
			
			for(int k=0; k<gt.rules.nHoursPerWeek; k++)
				allowed[k]=false;
					
			for(int m=0; m<ap->p_nPreferredTimeSlots_L; m++){
				int d=ap->p_days_L[m];
				int h=ap->p_hours_L[m];
				
				if(d>=0 && h>=0){
					assert(d>=0 && h>=0);
					allowed[d+h*gt.rules.nDaysPerWeek]=true;
				}
				else if(d>=0){
					for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
						allowed[d+hh*gt.rules.nDaysPerWeek]=true;
				}
				else if(h>=0){
					for(int dd=0; dd<gt.rules.nDaysPerWeek; dd++)
						allowed[dd+h*gt.rules.nDaysPerWeek]=true;
				}
			}
			
			for(int k=0; k<gt.rules.nHoursPerWeek; k++){
				int d=k%gt.rules.nDaysPerWeek;
				int h=k/gt.rules.nDaysPerWeek;
				
				bool ok=true;
				
				for(int dur=0; dur<gt.rules.internalActivitiesList[ai].duration && h+dur<gt.rules.nHoursPerDay; dur++){
					assert(d+(h+dur)*gt.rules.nDaysPerWeek<gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
					if(!allowed[d+(h+dur)*gt.rules.nDaysPerWeek]){
						ok=false;
						break;
					}
				}
			
				if(!ok)
					if(notAllowedTimesPercentages[ai][k] < ap->weightPercentage)
						notAllowedTimesPercentages[ai][k] = ap->weightPercentage;
			}
		}
		
		//ACTIVITIES preferred time slots
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS){
			ConstraintActivitiesPreferredTimeSlots* ap=(ConstraintActivitiesPreferredTimeSlots*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<ap->p_nActivities; j++){
				int ai=ap->p_activitiesIndices[j];
				
				for(int k=0; k<gt.rules.nHoursPerWeek; k++)
					allowed[k]=false;
					
				for(int m=0; m<ap->p_nPreferredTimeSlots_L; m++){
					int d=ap->p_days_L[m];
					int h=ap->p_hours_L[m];
					assert(d>=0 && h>=0);
					allowed[d+h*gt.rules.nDaysPerWeek]=true;
				}
				
				for(int k=0; k<gt.rules.nHoursPerWeek; k++){
					int d=k%gt.rules.nDaysPerWeek;
					int h=k/gt.rules.nDaysPerWeek;
					
					bool ok=true;
					
					for(int dur=0; dur<gt.rules.internalActivitiesList[ai].duration && h+dur<gt.rules.nHoursPerDay; dur++){
						assert(d+(h+dur)*gt.rules.nDaysPerWeek<gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
						if(!allowed[d+(h+dur)*gt.rules.nDaysPerWeek]){
							ok=false;
							break;
						}
					}
			
					if(!ok)
						if(notAllowedTimesPercentages[ai][k] < ap->weightPercentage)
							notAllowedTimesPercentages[ai][k] = ap->weightPercentage;
				}
			}
		}

		//subactivities preferred time slots
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS){
			ConstraintSubactivitiesPreferredTimeSlots* ap=(ConstraintSubactivitiesPreferredTimeSlots*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<ap->p_nActivities; j++){
				int ai=ap->p_activitiesIndices[j];
				
				for(int k=0; k<gt.rules.nHoursPerWeek; k++)
					allowed[k]=false;
					
				for(int m=0; m<ap->p_nPreferredTimeSlots_L; m++){
					int d=ap->p_days_L[m];
					int h=ap->p_hours_L[m];
					assert(d>=0 && h>=0);
					allowed[d+h*gt.rules.nDaysPerWeek]=true;
				}
				
				for(int k=0; k<gt.rules.nHoursPerWeek; k++){
					int d=k%gt.rules.nDaysPerWeek;
					int h=k/gt.rules.nDaysPerWeek;
					
					bool ok=true;
					
					for(int dur=0; dur<gt.rules.internalActivitiesList[ai].duration && h+dur<gt.rules.nHoursPerDay; dur++){
						assert(d+(h+dur)*gt.rules.nDaysPerWeek<gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
						if(!allowed[d+(h+dur)*gt.rules.nDaysPerWeek]){
							ok=false;
							break;
						}
					}
			
					if(!ok)
						if(notAllowedTimesPercentages[ai][k] < ap->weightPercentage)
							notAllowedTimesPercentages[ai][k] = ap->weightPercentage;
				}
			}
		}
	}
	
	return ok;
}

bool computeMinDays(QWidget* parent)
{
	QSet<ConstraintMinDaysBetweenActivities*> mdset;

	bool ok=true;

	minDaysListOfActivitiesFromThisConstraint.clear();
	for(int j=0; j<gt.rules.nInternalActivities; j++){
		minDaysListOfActivities[j].clear();
		minDaysListOfMinDays[j].clear();
		minDaysListOfConsecutiveIfSameDay[j].clear();
		minDaysListOfWeightPercentages[j].clear();
		minDaysListOfActivitiesFromTheSameConstraint[j].clear();
		
		//for(int k=0; k<gt.rules.nInternalActivities; k++)
		//	minDays[j][k]=0;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES
		 /*&&gt.rules.internalTimeConstraintsList[i]->compulsory==true*/){
			ConstraintMinDaysBetweenActivities* md=
			 (ConstraintMinDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
			
			minDaysListOfActivitiesFromThisConstraint.push_back(md->_activities);
			for(int j=0; j<md->_n_activities; j++){
				int ai1=md->_activities[j];
				for(int k=0; k<md->_n_activities; k++)
					if(j!=k){
						int ai2=md->_activities[k];
						if(ai1==ai2){
							ok=false;
							
							if(!mdset.contains(md)){
								mdset.insert(md);
						
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize, because you have a constraint min days between activities with duplicate activities. The constraint "
								 "is: %1. Please correct that.").arg(md->getDetailedDescription(gt.rules)),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );
					
								if(t==0)
									return ok;
							}
						}
						int m=md->minDays;
						/*if(m>minDays[ai1][ai2])
							minDays[ai1][ai2]=minDays[ai2][ai1]=m;*/
						
						minDaysListOfActivities[ai1].append(ai2);
						minDaysListOfMinDays[ai1].append(m);
						assert(md->weightPercentage>=0 && md->weightPercentage<=100);
						minDaysListOfWeightPercentages[ai1].append(md->weightPercentage);
						minDaysListOfConsecutiveIfSameDay[ai1].append(md->consecutiveIfSameDay);
						minDaysListOfActivitiesFromTheSameConstraint[ai1].append(&minDaysListOfActivitiesFromThisConstraint.back());
					}
			}
		}

	/*for(int j=0; j<gt.rules.nInternalActivities; j++)
		for(int k=0; k<gt.rules.nInternalActivities; k++)
			if(minDays[j][k]>0){
				assert(j!=k);
				minDaysListOfActivities[j].append(k);
				minDaysListOfMinDays[j].append(minDays[j][k]);
			}*/
			
	return ok;
}

bool computeMinHalfDays(QWidget* parent)
{
	QSet<ConstraintMinHalfDaysBetweenActivities*> mdset;

	bool ok=true;

	minHalfDaysListOfActivitiesFromThisConstraint.clear();
	for(int j=0; j<gt.rules.nInternalActivities; j++){
		minHalfDaysListOfActivities[j].clear();
		minHalfDaysListOfMinDays[j].clear();
		minHalfDaysListOfConsecutiveIfSameDay[j].clear();
		minHalfDaysListOfWeightPercentages[j].clear();
		minHalfDaysListOfActivitiesFromTheSameConstraint[j].clear();
		
		//for(int k=0; k<gt.rules.nInternalActivities; k++)
		//	minDays[j][k]=0;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES
		 /*&&gt.rules.internalTimeConstraintsList[i]->compulsory==true*/){
			ConstraintMinHalfDaysBetweenActivities* md=
			 (ConstraintMinHalfDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
			
			minHalfDaysListOfActivitiesFromThisConstraint.push_back(md->_activities);
			for(int j=0; j<md->_n_activities; j++){
				int ai1=md->_activities[j];
				for(int k=0; k<md->_n_activities; k++)
					if(j!=k){
						int ai2=md->_activities[k];
						if(ai1==ai2){
							ok=false;
							
							if(!mdset.contains(md)){
								mdset.insert(md);
						
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize, because you have a constraint min half days between activities with duplicate activities. The constraint "
								 "is: %1. Please correct that.").arg(md->getDetailedDescription(gt.rules)),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );
					
								if(t==0)
									return ok;
							}
						}
						int m=md->minDays;
						/*if(m>minDays[ai1][ai2])
							minDays[ai1][ai2]=minDays[ai2][ai1]=m;*/
						
						minHalfDaysListOfActivities[ai1].append(ai2);
						minHalfDaysListOfMinDays[ai1].append(m);
						assert(md->weightPercentage>=0 && md->weightPercentage<=100);
						minHalfDaysListOfWeightPercentages[ai1].append(md->weightPercentage);
						minHalfDaysListOfConsecutiveIfSameDay[ai1].append(md->consecutiveIfSameDay);
						minHalfDaysListOfActivitiesFromTheSameConstraint[ai1].append(&minHalfDaysListOfActivitiesFromThisConstraint.back());
					}
			}
		}

	/*for(int j=0; j<gt.rules.nInternalActivities; j++)
		for(int k=0; k<gt.rules.nInternalActivities; k++)
			if(minDays[j][k]>0){
				assert(j!=k);
				minHalfDaysListOfActivities[j].append(k);
				minHalfDaysListOfMinDays[j].append(minDays[j][k]);
			}*/
			
	return ok;
}

bool computeMaxDays(QWidget* parent)
{
	QSet<ConstraintMaxDaysBetweenActivities*> mdset;

	bool ok=true;

	for(int j=0; j<gt.rules.nInternalActivities; j++){
		maxDaysListOfActivities[j].clear();
		maxDaysListOfMaxDays[j].clear();
		maxDaysListOfWeightPercentages[j].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES
		 /*&&gt.rules.internalTimeConstraintsList[i]->compulsory==true*/){
			ConstraintMaxDaysBetweenActivities* md=
			 (ConstraintMaxDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<md->_n_activities; j++){
				int ai1=md->_activities[j];
				for(int k=0; k<md->_n_activities; k++)
					if(j!=k){
						int ai2=md->_activities[k];
						if(ai1==ai2){
							ok=false;
							
							if(!mdset.contains(md)){
								mdset.insert(md);
						
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize, because you have a constraint max days between activities with duplicate activities. The constraint "
								 "is: %1. Please correct that.").arg(md->getDetailedDescription(gt.rules)),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );
					
								if(t==0)
									return ok;
							}
						}
						int m=md->maxDays;
						/*if(m>minDays[ai1][ai2])
							minDays[ai1][ai2]=minDays[ai2][ai1]=m;*/
						
						maxDaysListOfActivities[ai1].append(ai2);
						maxDaysListOfMaxDays[ai1].append(m);
						assert(md->weightPercentage >=0 && md->weightPercentage<=100);
						maxDaysListOfWeightPercentages[ai1].append(md->weightPercentage);
						//maxDaysListOfConsecutiveIfSameDay[ai1].append(md->consecutiveIfSameDay);
					}
			}
		}

	/*for(int j=0; j<gt.rules.nInternalActivities; j++)
		for(int k=0; k<gt.rules.nInternalActivities; k++)
			if(minDays[j][k]>0){
				assert(j!=k);
				minDaysListOfActivities[j].append(k);
				minDaysListOfMinDays[j].append(minDays[j][k]);
			}*/
			
	return ok;
}

bool computeMaxHalfDays(QWidget* parent)
{
	QSet<ConstraintMaxHalfDaysBetweenActivities*> mdset;

	bool ok=true;

	for(int j=0; j<gt.rules.nInternalActivities; j++){
		maxHalfDaysListOfActivities[j].clear();
		maxHalfDaysListOfMaxDays[j].clear();
		maxHalfDaysListOfWeightPercentages[j].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MAX_HALF_DAYS_BETWEEN_ACTIVITIES
		 /*&&gt.rules.internalTimeConstraintsList[i]->compulsory==true*/){
			ConstraintMaxHalfDaysBetweenActivities* md=
			 (ConstraintMaxHalfDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<md->_n_activities; j++){
				int ai1=md->_activities[j];
				for(int k=0; k<md->_n_activities; k++)
					if(j!=k){
						int ai2=md->_activities[k];
						if(ai1==ai2){
							ok=false;
							
							if(!mdset.contains(md)){
								mdset.insert(md);
						
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize, because you have a constraint max half days between activities with duplicate activities. The constraint "
								 "is: %1. Please correct that.").arg(md->getDetailedDescription(gt.rules)),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );
					
								if(t==0)
									return ok;
							}
						}
						int m=md->maxDays;
						/*if(m>minDays[ai1][ai2])
							minDays[ai1][ai2]=minDays[ai2][ai1]=m;*/
						
						maxHalfDaysListOfActivities[ai1].append(ai2);
						maxHalfDaysListOfMaxDays[ai1].append(m);
						assert(md->weightPercentage >=0 && md->weightPercentage<=100);
						maxHalfDaysListOfWeightPercentages[ai1].append(md->weightPercentage);
						//maxDaysListOfConsecutiveIfSameDay[ai1].append(md->consecutiveIfSameDay);
					}
			}
		}

	/*for(int j=0; j<gt.rules.nInternalActivities; j++)
		for(int k=0; k<gt.rules.nInternalActivities; k++)
			if(minDays[j][k]>0){
				assert(j!=k);
				minDaysListOfActivities[j].append(k);
				minDaysListOfMinDays[j].append(minDays[j][k]);
			}*/
			
	return ok;
}

bool computeMaxTerms(QWidget* parent)
{
	QSet<ConstraintMaxTermsBetweenActivities*> mtset;

	bool ok=true;

	for(int j=0; j<gt.rules.nInternalActivities; j++){
		maxTermsListOfActivities[j].clear();
		maxTermsListOfMaxTerms[j].clear();
		maxTermsListOfWeightPercentages[j].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MAX_TERMS_BETWEEN_ACTIVITIES
		 /*&&gt.rules.internalTimeConstraintsList[i]->compulsory==true*/){
			ConstraintMaxTermsBetweenActivities* mt=
			 (ConstraintMaxTermsBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<mt->_n_activities; j++){
				int ai1=mt->_activities[j];
				for(int k=0; k<mt->_n_activities; k++)
					if(j!=k){
						int ai2=mt->_activities[k];
						if(ai1==ai2){
							ok=false;
							
							if(!mtset.contains(mt)){
								mtset.insert(mt);
						
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize, because you have a constraint max terms between activities with duplicate activities. The constraint "
								 "is: %1. Please correct that.").arg(mt->getDetailedDescription(gt.rules)),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );
					
								if(t==0)
									return ok;
							}
						}
						int m=mt->maxTerms;
						/*if(m>minDays[ai1][ai2])
							minDays[ai1][ai2]=minDays[ai2][ai1]=m;*/
						
						maxTermsListOfActivities[ai1].append(ai2);
						maxTermsListOfMaxTerms[ai1].append(m);
						assert(mt->weightPercentage >=0 && mt->weightPercentage<=100);
						maxTermsListOfWeightPercentages[ai1].append(mt->weightPercentage);
						//maxDaysListOfConsecutiveIfSameDay[ai1].append(md->consecutiveIfSameDay);
					}
			}
		}

	/*for(int j=0; j<gt.rules.nInternalActivities; j++)
		for(int k=0; k<gt.rules.nInternalActivities; k++)
			if(minDays[j][k]>0){
				assert(j!=k);
				minDaysListOfActivities[j].append(k);
				minDaysListOfMinDays[j].append(minDays[j][k]);
			}*/
			
	return ok;
}

bool computeMinGapsBetweenActivities(QWidget* parent)
{
	QSet<ConstraintMinGapsBetweenActivities*> mgset;

	bool ok=true;

	for(int j=0; j<gt.rules.nInternalActivities; j++){
		minGapsBetweenActivitiesListOfActivities[j].clear();
		minGapsBetweenActivitiesListOfMinGaps[j].clear();
		minGapsBetweenActivitiesListOfWeightPercentages[j].clear();
		
		//for(int k=0; k<gt.rules.nInternalActivities; k++)
		//	minDays[j][k]=0;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES
		 /*&&gt.rules.internalTimeConstraintsList[i]->compulsory==true*/){
			ConstraintMinGapsBetweenActivities* mg=
			 (ConstraintMinGapsBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
			
			assert(mg->_n_activities==mg->_activities.count());
			
			for(int j=0; j<mg->_n_activities; j++){
				int ai1=mg->_activities[j];
				for(int k=0; k<mg->_n_activities; k++)
					if(j!=k){
						int ai2=mg->_activities[k];
						if(ai1==ai2){
							ok=false;
							
							if(!mgset.contains(mg)){
								mgset.insert(mg);
						
								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize, because you have a constraint min gaps between activities with duplicate activities. The constraint "
								 "is: %1. Please correct that.").arg(mg->getDetailedDescription(gt.rules)),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );
					
								if(t==0)
									return ok;
							}
						}
						int m=mg->minGaps;
						
						minGapsBetweenActivitiesListOfActivities[ai1].append(ai2);
						minGapsBetweenActivitiesListOfMinGaps[ai1].append(m);
						assert(mg->weightPercentage >=0 && mg->weightPercentage<=100);
						minGapsBetweenActivitiesListOfWeightPercentages[ai1].append(mg->weightPercentage);
					}
			}
		}

	return ok;
}

bool computeMaxGapsBetweenActivities(QWidget* parent)
{
	QSet<ConstraintMaxGapsBetweenActivities*> mgset;

	bool ok=true;

	for(int j=0; j<gt.rules.nInternalActivities; j++){
		maxGapsBetweenActivitiesListOfActivities[j].clear();
		maxGapsBetweenActivitiesListOfMaxGaps[j].clear();
		maxGapsBetweenActivitiesListOfWeightPercentages[j].clear();

		//for(int k=0; k<gt.rules.nInternalActivities; k++)
		//	minDays[j][k]=0;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MAX_GAPS_BETWEEN_ACTIVITIES
		 /*&&gt.rules.internalTimeConstraintsList[i]->compulsory==true*/){
			ConstraintMaxGapsBetweenActivities* mg=
			 (ConstraintMaxGapsBetweenActivities*)gt.rules.internalTimeConstraintsList[i];

			assert(mg->_n_activities==mg->_activities.count());

			for(int j=0; j<mg->_n_activities; j++){
				int ai1=mg->_activities[j];
				for(int k=0; k<mg->_n_activities; k++)
					if(j!=k){
						int ai2=mg->_activities[k];
						if(ai1==ai2){
							ok=false;

							if(!mgset.contains(mg)){
								mgset.insert(mg);

								int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
								 GeneratePreTranslate::tr("Cannot optimize, because you have a constraint max gaps between activities with duplicate activities. The constraint "
								 "is: %1. Please correct that.").arg(mg->getDetailedDescription(gt.rules)),
								 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
								 1, 0 );

								if(t==0)
									return ok;
							}
						}
						int m=mg->maxGaps;

						maxGapsBetweenActivitiesListOfActivities[ai1].append(ai2);
						maxGapsBetweenActivitiesListOfMaxGaps[ai1].append(m);
						assert(mg->weightPercentage >=0 && mg->weightPercentage<=100);
						maxGapsBetweenActivitiesListOfWeightPercentages[ai1].append(mg->weightPercentage);
					}
			}
		}

	return ok;
}

bool computeActivitiesConflictingPercentage(QWidget* parent)
{
	//get maximum weight percent of a basic time constraint
	double m=-1;
	
	bool ok=false;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_BASIC_COMPULSORY_TIME){
			ok=true;
			if(gt.rules.internalTimeConstraintsList[i]->weightPercentage>m)
				m=gt.rules.internalTimeConstraintsList[i]->weightPercentage;
		}
	
	if(m<100)
		ok=false;
	
	if(!ok || m<100){
		GeneratePreIrreconcilableMessage::mediumInformation(parent, GeneratePreTranslate::tr("FET warning"),
		 GeneratePreTranslate::tr("Cannot generate, because you do not have a constraint of type basic compulsory time or its weight is lower than 100.0%.")
		 +" "+
		 GeneratePreTranslate::tr("Please add a constraint of this type with weight 100%.")
		 +" "+
		 GeneratePreTranslate::tr("You can add this constraint from the menu Data -> Time constraints -> Miscellaneous -> Basic compulsory time constraints.")
		 +"\n\n"+
		 GeneratePreTranslate::tr("Explanation:")
		 +" "+
		 GeneratePreTranslate::tr("Each time you create a new file, it contains an automatically added constraint of this type.")
		 +" "+
		 GeneratePreTranslate::tr("For complete flexibility, you are allowed to remove it (even if this is a wrong idea).")
		 +" "+
		 GeneratePreTranslate::tr("Maybe you removed it by mistake from your file.")
		 +" "+
		 GeneratePreTranslate::tr("By adding it again, everything should be all right.")
		 );
		return false;
	}
	
	assert(m>=0 && m<=100);
	assert(m==100);

	//compute conflicting
	activitiesConflictingPercentage.resize(gt.rules.nInternalActivities);
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		activitiesConflictingPercentage[i].clear();
		
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		activitiesConflictingPercentage[i].insert(i, 100);

	QProgressDialog progress(parent);
	progress.setWindowTitle(GeneratePreTranslate::tr("Precomputing", "Title of a progress dialog"));
	progress.setLabelText(GeneratePreTranslate::tr("Precomputing ... please wait"));
	progress.setRange(0, qMax(gt.rules.nInternalTeachers+gt.rules.nInternalSubgroups, 1));
	progress.setModal(true);
	
	int ttt=0;
	
	for(int t=0; t<gt.rules.nInternalTeachers; t++){
		progress.setValue(ttt);
		//pqapplication->processEvents();
		if(progress.wasCanceled()){
			GeneratePreIrreconcilableMessage::information(parent, GeneratePreTranslate::tr("FET information"), GeneratePreTranslate::tr("Canceled"));
			return false;
		}
		
		ttt++;
		
		for(int i : qAsConst(gt.rules.internalTeachersList[t]->activitiesForTeacher))
			for(int j : qAsConst(gt.rules.internalTeachersList[t]->activitiesForTeacher))
				activitiesConflictingPercentage[i].insert(j, 100);
	}
	
	for(int s=0; s<gt.rules.nInternalSubgroups; s++){
		progress.setValue(ttt);
		//pqapplication->processEvents();
		if(progress.wasCanceled()){
			GeneratePreIrreconcilableMessage::information(parent, GeneratePreTranslate::tr("FET information"), GeneratePreTranslate::tr("Canceled"));
			return false;
		}
		
		ttt++;
		
		for(int i : qAsConst(gt.rules.internalSubgroupsList[s]->activitiesForSubgroup))
			for(int j : qAsConst(gt.rules.internalSubgroupsList[s]->activitiesForSubgroup))
				activitiesConflictingPercentage[i].insert(j, 100);
	}

	progress.setValue(qMax(gt.rules.nInternalTeachers+gt.rules.nInternalSubgroups, 1));
	
	//new Volker (start)
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		TimeConstraint* tc=gt.rules.internalTimeConstraintsList[i];
		if(tc->type==CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING){
			if(tc->weightPercentage==100.0){
				ConstraintActivitiesNotOverlapping* cno=(ConstraintActivitiesNotOverlapping*) tc;
				
				for(int a=0; a<cno->_n_activities; a++){
					for(int b=0; b<cno->_n_activities; b++){
						if(cno->_activities[a]!=cno->_activities[b]){
							activitiesConflictingPercentage[cno->_activities[a]].insert(cno->_activities[b], 100);
						}
					}
				}
			}
			else{
				ConstraintActivitiesNotOverlapping* cno=(ConstraintActivitiesNotOverlapping*) tc;
				
				int ww=int(cno->weightPercentage);
				if(ww>100)
					ww=100;
			
				for(int a=0; a<cno->_n_activities; a++){
					for(int b=0; b<cno->_n_activities; b++){
						if(cno->_activities[a]!=cno->_activities[b]){
							if(activitiesConflictingPercentage[cno->_activities[a]].value(cno->_activities[b], -1) < ww)
								activitiesConflictingPercentage[cno->_activities[a]].insert(cno->_activities[b], ww);
						}
					}
				}
			}
		}
		else if(tc->type==CONSTRAINT_ACTIVITY_TAGS_NOT_OVERLAPPING){
			ConstraintActivityTagsNotOverlapping* catno=(ConstraintActivityTagsNotOverlapping*) tc;
			assert(catno->weightPercentage>=0.0 && catno->weightPercentage<=100.0);
			
			for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				
				int cnt=0;
				
				for(int at : qAsConst(catno->activityTagsIndices)){
					if(act->iActivityTagsSet.contains(at))
						cnt++;
				}
				
				if(cnt>=2){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("%1 cannot be respected because"
					 " the activity with id=%2 refers to %3 activity tags from this constraint. Each activity"
					 " should refer to at most one activity tag from each separated constraint of this type.")
					 .arg(catno->getDetailedDescription(gt.rules)).arg(act->id).arg(cnt),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return ok;
				}
			}
			
			if(!ok)
				return false;
		
			for(int i=0; i<catno->activitiesIndicesLists.count(); i++){
				const QList<int>& l1=catno->activitiesIndicesLists.at(i);
				for(int j=0; j<catno->activitiesIndicesLists.count(); j++){
					if(i!=j){
						const QList<int>& l2=catno->activitiesIndicesLists.at(j);
						
						for(int a : qAsConst(l1))
							for(int b : qAsConst(l2)){
								if(a!=b){
									if(activitiesConflictingPercentage[a].value(b, -1) < catno->weightPercentage){
										activitiesConflictingPercentage[a].insert(b, catno->weightPercentage);
									}
								}
								else{
									assert(0);
								}
							}
					}
				}
			}
		}
	}
	//new Volker (end)

	return true;
}

void computeConstrTwoActivitiesConsecutive()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		constrTwoActivitiesConsecutivePercentages[i].clear();
		constrTwoActivitiesConsecutiveActivities[i].clear();

		inverseConstrTwoActivitiesConsecutivePercentages[i].clear();
		inverseConstrTwoActivitiesConsecutiveActivities[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE){
			ConstraintTwoActivitiesConsecutive* c2=(ConstraintTwoActivitiesConsecutive*)gt.rules.internalTimeConstraintsList[i];
			
			int fai=c2->firstActivityIndex;
			int sai=c2->secondActivityIndex;
			
			//direct
			int j=constrTwoActivitiesConsecutiveActivities[fai].indexOf(sai);
			if(j==-1){
				constrTwoActivitiesConsecutiveActivities[fai].append(sai);
				constrTwoActivitiesConsecutivePercentages[fai].append(c2->weightPercentage);
			}
			else if(j>=0 && constrTwoActivitiesConsecutivePercentages[fai].at(j)<c2->weightPercentage){
				constrTwoActivitiesConsecutivePercentages[fai][j]=c2->weightPercentage;
			}

			//inverse
			j=inverseConstrTwoActivitiesConsecutiveActivities[sai].indexOf(fai);
			if(j==-1){
				inverseConstrTwoActivitiesConsecutiveActivities[sai].append(fai);
				inverseConstrTwoActivitiesConsecutivePercentages[sai].append(c2->weightPercentage);
			}
			else if(j>=0 && inverseConstrTwoActivitiesConsecutivePercentages[sai].at(j)<c2->weightPercentage){
				inverseConstrTwoActivitiesConsecutivePercentages[sai][j]=c2->weightPercentage;
			}
		}
}

void computeConstrTwoActivitiesGrouped()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		constrTwoActivitiesGroupedPercentages[i].clear();
		constrTwoActivitiesGroupedActivities[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TWO_ACTIVITIES_GROUPED){
			ConstraintTwoActivitiesGrouped* c2=(ConstraintTwoActivitiesGrouped*)gt.rules.internalTimeConstraintsList[i];
			
			int fai=c2->firstActivityIndex;
			int sai=c2->secondActivityIndex;
			
			//direct
			int j=constrTwoActivitiesGroupedActivities[fai].indexOf(sai);
			if(j==-1){
				constrTwoActivitiesGroupedActivities[fai].append(sai);
				constrTwoActivitiesGroupedPercentages[fai].append(c2->weightPercentage);
			}
			else if(j>=0 && constrTwoActivitiesGroupedPercentages[fai].at(j)<c2->weightPercentage){
				constrTwoActivitiesGroupedPercentages[fai][j]=c2->weightPercentage;
			}

			//inverse
			j=constrTwoActivitiesGroupedActivities[sai].indexOf(fai);
			if(j==-1){
				constrTwoActivitiesGroupedActivities[sai].append(fai);
				constrTwoActivitiesGroupedPercentages[sai].append(c2->weightPercentage);
			}
			else if(j>=0 && constrTwoActivitiesGroupedPercentages[sai].at(j)<c2->weightPercentage){
				constrTwoActivitiesGroupedPercentages[sai][j]=c2->weightPercentage;
			}
		}
}

void computeConstrThreeActivitiesGrouped()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		constrThreeActivitiesGroupedPercentages[i].clear();
		constrThreeActivitiesGroupedActivities[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_THREE_ACTIVITIES_GROUPED){
			ConstraintThreeActivitiesGrouped* c3=(ConstraintThreeActivitiesGrouped*)gt.rules.internalTimeConstraintsList[i];
			
			int fai=c3->firstActivityIndex;
			int sai=c3->secondActivityIndex;
			int tai=c3->thirdActivityIndex;

			QPair<int, int> p23(sai, tai);
			int j=constrThreeActivitiesGroupedActivities[fai].indexOf(p23);
			if(j==-1){
				constrThreeActivitiesGroupedActivities[fai].append(p23);
				constrThreeActivitiesGroupedPercentages[fai].append(c3->weightPercentage);
			}
			else if(j>=0 && constrThreeActivitiesGroupedPercentages[fai].at(j)<c3->weightPercentage){
				constrThreeActivitiesGroupedPercentages[fai][j]=c3->weightPercentage;
			}

			QPair<int, int> p13(fai, tai);
			j=constrThreeActivitiesGroupedActivities[sai].indexOf(p13);
			if(j==-1){
				constrThreeActivitiesGroupedActivities[sai].append(p13);
				constrThreeActivitiesGroupedPercentages[sai].append(c3->weightPercentage);
			}
			else if(j>=0 && constrThreeActivitiesGroupedPercentages[sai].at(j)<c3->weightPercentage){
				constrThreeActivitiesGroupedPercentages[sai][j]=c3->weightPercentage;
			}

			QPair<int, int> p12(fai, sai);
			j=constrThreeActivitiesGroupedActivities[tai].indexOf(p12);
			if(j==-1){
				constrThreeActivitiesGroupedActivities[tai].append(p12);
				constrThreeActivitiesGroupedPercentages[tai].append(c3->weightPercentage);
			}
			else if(j>=0 && constrThreeActivitiesGroupedPercentages[tai].at(j)<c3->weightPercentage){
				constrThreeActivitiesGroupedPercentages[tai][j]=c3->weightPercentage;
			}
		}
}

void computeConstrTwoActivitiesOrdered()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		constrTwoActivitiesOrderedPercentages[i].clear();
		constrTwoActivitiesOrderedActivities[i].clear();

		inverseConstrTwoActivitiesOrderedPercentages[i].clear();
		inverseConstrTwoActivitiesOrderedActivities[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TWO_ACTIVITIES_ORDERED){
			ConstraintTwoActivitiesOrdered* c2=(ConstraintTwoActivitiesOrdered*)gt.rules.internalTimeConstraintsList[i];
			
			int fai=c2->firstActivityIndex;
			int sai=c2->secondActivityIndex;
			
			//direct
			int j=constrTwoActivitiesOrderedActivities[fai].indexOf(sai);
			if(j==-1){
				constrTwoActivitiesOrderedActivities[fai].append(sai);
				constrTwoActivitiesOrderedPercentages[fai].append(c2->weightPercentage);
			}
			else if(j>=0 && constrTwoActivitiesOrderedPercentages[fai].at(j)<c2->weightPercentage){
				constrTwoActivitiesOrderedPercentages[fai][j]=c2->weightPercentage;
			}

			//inverse
			j=inverseConstrTwoActivitiesOrderedActivities[sai].indexOf(fai);
			if(j==-1){
				inverseConstrTwoActivitiesOrderedActivities[sai].append(fai);
				inverseConstrTwoActivitiesOrderedPercentages[sai].append(c2->weightPercentage);
			}
			else if(j>=0 && inverseConstrTwoActivitiesOrderedPercentages[sai].at(j)<c2->weightPercentage){
				inverseConstrTwoActivitiesOrderedPercentages[sai][j]=c2->weightPercentage;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TWO_SETS_OF_ACTIVITIES_ORDERED){
			ConstraintTwoSetsOfActivitiesOrdered* c2=(ConstraintTwoSetsOfActivitiesOrdered*)gt.rules.internalTimeConstraintsList[i];
			
			for(int fai : qAsConst(c2->firstActivitiesIndicesList)){
				for(int sai : qAsConst(c2->secondActivitiesIndicesList)){
					//direct
					int j=constrTwoActivitiesOrderedActivities[fai].indexOf(sai);
					if(j==-1){
						constrTwoActivitiesOrderedActivities[fai].append(sai);
						constrTwoActivitiesOrderedPercentages[fai].append(c2->weightPercentage);
					}
					else if(j>=0 && constrTwoActivitiesOrderedPercentages[fai].at(j)<c2->weightPercentage){
						constrTwoActivitiesOrderedPercentages[fai][j]=c2->weightPercentage;
					}

					//inverse
					j=inverseConstrTwoActivitiesOrderedActivities[sai].indexOf(fai);
					if(j==-1){
						inverseConstrTwoActivitiesOrderedActivities[sai].append(fai);
						inverseConstrTwoActivitiesOrderedPercentages[sai].append(c2->weightPercentage);
					}
					else if(j>=0 && inverseConstrTwoActivitiesOrderedPercentages[sai].at(j)<c2->weightPercentage){
						inverseConstrTwoActivitiesOrderedPercentages[sai][j]=c2->weightPercentage;
					}
				}
			}
		}
	}
}

void computeConstrTwoActivitiesOrderedIfSameDay()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		constrTwoActivitiesOrderedIfSameDayPercentages[i].clear();
		constrTwoActivitiesOrderedIfSameDayActivities[i].clear();

		inverseConstrTwoActivitiesOrderedIfSameDayPercentages[i].clear();
		inverseConstrTwoActivitiesOrderedIfSameDayActivities[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TWO_ACTIVITIES_ORDERED_IF_SAME_DAY){
			ConstraintTwoActivitiesOrderedIfSameDay* c2=(ConstraintTwoActivitiesOrderedIfSameDay*)gt.rules.internalTimeConstraintsList[i];
			
			int fai=c2->firstActivityIndex;
			int sai=c2->secondActivityIndex;
			
			//direct
			int j=constrTwoActivitiesOrderedIfSameDayActivities[fai].indexOf(sai);
			if(j==-1){
				constrTwoActivitiesOrderedIfSameDayActivities[fai].append(sai);
				constrTwoActivitiesOrderedIfSameDayPercentages[fai].append(c2->weightPercentage);
			}
			else if(j>=0 && constrTwoActivitiesOrderedIfSameDayPercentages[fai].at(j)<c2->weightPercentage){
				constrTwoActivitiesOrderedIfSameDayPercentages[fai][j]=c2->weightPercentage;
			}

			//inverse
			j=inverseConstrTwoActivitiesOrderedIfSameDayActivities[sai].indexOf(fai);
			if(j==-1){
				inverseConstrTwoActivitiesOrderedIfSameDayActivities[sai].append(fai);
				inverseConstrTwoActivitiesOrderedIfSameDayPercentages[sai].append(c2->weightPercentage);
			}
			else if(j>=0 && inverseConstrTwoActivitiesOrderedIfSameDayPercentages[sai].at(j)<c2->weightPercentage){
				inverseConstrTwoActivitiesOrderedIfSameDayPercentages[sai][j]=c2->weightPercentage;
			}
		}
}

bool computeActivityEndsStudentsDayPercentages(QWidget* parent)
{
	bool ok=true;

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++)
		activityEndsStudentsDayPercentages[ai]=-1;
	
	haveActivityEndsStudentsDay=false;
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY){
			haveActivityEndsStudentsDay=true;
		
			ConstraintActivityEndsStudentsDay* cae=(ConstraintActivityEndsStudentsDay*)gt.rules.internalTimeConstraintsList[i];
			
			if(cae->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
				 "activity ends students day for activity with id=%1 with weight percentage under 100%. "
				 "Constraint activity ends students day can only have weight percentage 100%. "
				 "Please modify your data accordingly (remove or edit constraint) and try again.")
				 .arg(gt.rules.internalActivitiesList[cae->activityIndex].id),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				1, 0 );
				
				if(t==0)
					break;
			}
			
			int ai=cae->activityIndex;
			if(activityEndsStudentsDayPercentages[ai] < cae->weightPercentage)
				activityEndsStudentsDayPercentages[ai] = cae->weightPercentage;
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY){
			haveActivityEndsStudentsDay=true;
		
			ConstraintActivitiesEndStudentsDay* cae=(ConstraintActivitiesEndStudentsDay*)gt.rules.internalTimeConstraintsList[i];
			
			if(cae->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
				 "activities end students day with weight percentage under 100%. "
				 "Constraint activities end students day can only have weight percentage 100%. "
				 "Please modify your data accordingly (remove or edit constraint) and try again."),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				1, 0 );
				
				if(t==0)
					break;
			}
			
			for(int i=0; i<cae->nActivities; i++){
				int ai=cae->activitiesIndices[i];
				if(activityEndsStudentsDayPercentages[ai] < cae->weightPercentage)
					activityEndsStudentsDayPercentages[ai] = cae->weightPercentage;
			}
		}
	}
	
	return ok;
}

bool computeActivityEndsTeachersDayPercentages(QWidget* parent)
{
	bool ok=true;

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++)
		activityEndsTeachersDayPercentages[ai]=-1;
	
	haveActivityEndsTeachersDay=false;
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITY_ENDS_TEACHERS_DAY){
			haveActivityEndsTeachersDay=true;
		
			ConstraintActivityEndsTeachersDay* cae=(ConstraintActivityEndsTeachersDay*)gt.rules.internalTimeConstraintsList[i];
			
			if(cae->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
				 "activity ends teachers day for activity with id==%1 with weight percentage under 100%. "
				 "Constraint activity ends teachers day can only have weight percentage 100%. "
				 "Please modify your data accordingly (remove or edit constraint) and try again.")
				 .arg(gt.rules.internalActivitiesList[cae->activityIndex].id),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				1, 0 );
				
				if(t==0)
					break;
			}
			
			int ai=cae->activityIndex;
			if(activityEndsTeachersDayPercentages[ai] < cae->weightPercentage)
				activityEndsTeachersDayPercentages[ai] = cae->weightPercentage;
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_END_TEACHERS_DAY){
			haveActivityEndsTeachersDay=true;
		
			ConstraintActivitiesEndTeachersDay* cae=(ConstraintActivitiesEndTeachersDay*)gt.rules.internalTimeConstraintsList[i];
			
			if(cae->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
				 "activities end teachers day with weight percentage under 100%. "
				 "Constraint activities end teachers day can only have weight percentage 100%. "
				 "Please modify your data accordingly (remove or edit constraint) and try again."),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				1, 0 );
				
				if(t==0)
					break;
			}
			
			for(int i=0; i<cae->nActivities; i++){
				int ai=cae->activitiesIndices[i];
				if(activityEndsTeachersDayPercentages[ai] < cae->weightPercentage)
					activityEndsTeachersDayPercentages[ai] = cae->weightPercentage;
			}
		}
	}
		
	return ok;
}

bool computeActivityBeginsStudentsDayPercentages(QWidget* parent)
{
	bool ok=true;

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++)
		activityBeginsStudentsDayPercentages[ai]=-1;
	
	haveActivityBeginsStudentsDay=false;
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITY_BEGINS_STUDENTS_DAY){
			haveActivityBeginsStudentsDay=true;
		
			ConstraintActivityBeginsStudentsDay* cae=(ConstraintActivityBeginsStudentsDay*)gt.rules.internalTimeConstraintsList[i];
			
			if(cae->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
				 "activity begins students day for activity with id=%1 with weight percentage under 100%. "
				 "Constraint activity begins students day can only have weight percentage 100%. "
				 "Please modify your data accordingly (remove or edit constraint) and try again.")
				 .arg(gt.rules.internalActivitiesList[cae->activityIndex].id),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				1, 0 );
				
				if(t==0)
					break;
			}
			
			int ai=cae->activityIndex;
			if(activityBeginsStudentsDayPercentages[ai] < cae->weightPercentage)
				activityBeginsStudentsDayPercentages[ai] = cae->weightPercentage;
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_BEGIN_STUDENTS_DAY){
			haveActivityBeginsStudentsDay=true;
		
			ConstraintActivitiesBeginStudentsDay* cae=(ConstraintActivitiesBeginStudentsDay*)gt.rules.internalTimeConstraintsList[i];
			
			if(cae->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
				 "activities begin students day with weight percentage under 100%. "
				 "Constraint activities begin students day can only have weight percentage 100%. "
				 "Please modify your data accordingly (remove or edit constraint) and try again."),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				1, 0 );
				
				if(t==0)
					break;
			}
			
			for(int i=0; i<cae->nActivities; i++){
				int ai=cae->activitiesIndices[i];
				if(activityBeginsStudentsDayPercentages[ai] < cae->weightPercentage)
					activityBeginsStudentsDayPercentages[ai] = cae->weightPercentage;
			}
		}
	}
	
	return ok;
}

bool computeActivityBeginsTeachersDayPercentages(QWidget* parent)
{
	bool ok=true;

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++)
		activityBeginsTeachersDayPercentages[ai]=-1;
	
	haveActivityBeginsTeachersDay=false;
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITY_BEGINS_TEACHERS_DAY){
			haveActivityBeginsTeachersDay=true;
		
			ConstraintActivityBeginsTeachersDay* cae=(ConstraintActivityBeginsTeachersDay*)gt.rules.internalTimeConstraintsList[i];
			
			if(cae->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
				 "activity begins teachers day for activity with id==%1 with weight percentage under 100%. "
				 "Constraint activity begins teachers day can only have weight percentage 100%. "
				 "Please modify your data accordingly (remove or edit constraint) and try again.")
				 .arg(gt.rules.internalActivitiesList[cae->activityIndex].id),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				1, 0 );
				
				if(t==0)
					break;
			}
			
			int ai=cae->activityIndex;
			if(activityBeginsTeachersDayPercentages[ai] < cae->weightPercentage)
				activityBeginsTeachersDayPercentages[ai] = cae->weightPercentage;
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_BEGIN_TEACHERS_DAY){
			haveActivityBeginsTeachersDay=true;
		
			ConstraintActivitiesBeginTeachersDay* cae=(ConstraintActivitiesBeginTeachersDay*)gt.rules.internalTimeConstraintsList[i];
			
			if(cae->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraints of type "
				 "activities begin teachers day with weight percentage under 100%. "
				 "Constraint activities begin teachers day can only have weight percentage 100%. "
				 "Please modify your data accordingly (remove or edit constraint) and try again."),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				1, 0 );
				
				if(t==0)
					break;
			}
			
			for(int i=0; i<cae->nActivities; i++){
				int ai=cae->activitiesIndices[i];
				if(activityBeginsTeachersDayPercentages[ai] < cae->weightPercentage)
					activityBeginsTeachersDayPercentages[ai] = cae->weightPercentage;
			}
		}
	}
		
	return ok;
}

bool checkMinDays100Percent(QWidget* parent)
{
	bool ok=true;
	
	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		daysTeacherIsAvailable[tc]=0;

		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			bool dayAvailable=false;
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h]){
					dayAvailable=true;
					break;
				}
				
			if(dayAvailable)
				daysTeacherIsAvailable[tc]++;
		}

		if(teachersMaxDaysPerWeekMaxDays[tc]>=0){ //it has compulsory 100% weight
			assert(teachersMaxDaysPerWeekWeightPercentages[tc]==100);
			daysTeacherIsAvailable[tc]=min(daysTeacherIsAvailable[tc], teachersMaxDaysPerWeekMaxDays[tc]);
		}

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(teachersMaxRealDaysPerWeekMaxDays[tc]>=0){ //it has compulsory 100% weight
				assert(teachersMaxRealDaysPerWeekWeightPercentages[tc]==100);
				daysTeacherIsAvailable[tc]=min(daysTeacherIsAvailable[tc], teachersMaxRealDaysPerWeekMaxDays[tc]);
			}
		}
	}

	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		daysSubgroupIsAvailable[sb]=0;

		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			bool dayAvailable=false;
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[sb][d][h]){
					dayAvailable=true;
					break;
				}
				
			if(dayAvailable)
				daysSubgroupIsAvailable[sb]++;
		}

		if(subgroupsMaxDaysPerWeekMaxDays[sb]>=0){ //it has compulsory 100% weight
			assert(subgroupsMaxDaysPerWeekWeightPercentages[sb]==100);
			daysSubgroupIsAvailable[sb]=min(daysSubgroupIsAvailable[sb], subgroupsMaxDaysPerWeekMaxDays[sb]);
		}

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(subgroupsMaxRealDaysPerWeekMaxDays[sb]>=0){ //it has compulsory 100% weight
				assert(subgroupsMaxRealDaysPerWeekWeightPercentages[sb]==100);
				daysSubgroupIsAvailable[sb]=min(daysSubgroupIsAvailable[sb], subgroupsMaxRealDaysPerWeekMaxDays[sb]);
			}
		}
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES
		 &&gt.rules.internalTimeConstraintsList[i]->weightPercentage==100.0){
			ConstraintMinDaysBetweenActivities* md=(ConstraintMinDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
			
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				if(md->minDays>=1){
					int na=md->_n_activities;
					int nd=md->minDays;
					if((na-1)*nd+1 > gt.rules.nDaysPerWeek){
						ok=false;
						
						QString s=GeneratePreTranslate::tr("%1 cannot be respected because it contains %2 activities,"
						 " has weight 100% and has min number of days between activities=%3. The minimum required number of days per week for"
						 " that would be (nactivities-1)*mindays+1=%4, and you have only %5 days per week - impossible. Please correct this constraint.", "%1 is the detailed description of a constraint"
						)
						 .arg(md->getDetailedDescription(gt.rules))
						 .arg(na)
						 .arg(nd)
						 .arg((na-1)*nd+1)
						 .arg(gt.rules.nDaysPerWeek)
						 ;

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
						
						if(t==0)
							return ok;
					}
				}
			}
			else{
				if(md->minDays>=1){
					int na=md->_n_activities;
					int nd=md->minDays;
					if((na-1)*nd+1 > gt.rules.nDaysPerWeek/2){
						ok=false;
						
						QString s=GeneratePreTranslate::tr("%1 cannot be respected because it contains %2 activities,"
						 " has weight 100% and has min number of real days between activities=%3. The minimum required number of real days per week for"
						 " that would be (nactivities-1)*mindays+1=%4, and you have only %5 real days per week - impossible. Please correct this constraint.", "%1 is the detailed description of a constraint"
						)
						 .arg(md->getDetailedDescription(gt.rules))
						 .arg(na)
						 .arg(nd)
						 .arg((na-1)*nd+1)
						 .arg(gt.rules.nDaysPerWeek/2)
						 ;

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
						
						if(t==0)
							return ok;
					}
				}
			}
			
			if(md->minDays>=1){
				for(int tc=0; tc<gt.rules.nInternalTeachers; tc++)
					requestedDaysForTeachers[tc]=0;
				for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++)
					requestedDaysForSubgroups[sb]=0;
			
				for(int j=0; j<md->_n_activities; j++){
					int ai=md->_activities[j];
					for(int k=0; k<gt.rules.internalActivitiesList[ai].iTeachersList.count(); k++){
						int tc=gt.rules.internalActivitiesList[ai].iTeachersList.at(k);
						requestedDaysForTeachers[tc]++;
					}
					for(int k=0; k<gt.rules.internalActivitiesList[ai].iSubgroupsList.count(); k++){
						int sb=gt.rules.internalActivitiesList[ai].iSubgroupsList.at(k);
						requestedDaysForSubgroups[sb]++;
					}
				}
				for(int tc=0; tc<gt.rules.nInternalTeachers; tc++)
					if(requestedDaysForTeachers[tc]>daysTeacherIsAvailable[tc]){
						ok=false;
						
						QString s=GeneratePreTranslate::tr("%1 cannot be respected because teacher %2 has at most"
						 " %3 available days from teacher not available, breaks and teacher max days per week."
						 " Please lower the weight of this constraint to a value below 100% (it depends"
						 " on your situation, if 0% is too little, make it 90%, 95% or even 99.75%."
						 " Even a large weight should not slow down much the program."
						 " A situation where you may need to make it larger than 0% is for instance if you have 5 activities with 4"
						 " possible days. You want to spread them 1, 1, 1 and 2, not 2, 2 and 1)", "%1 is the detailed description of a constraint"
						)
						 .arg(md->getDetailedDescription(gt.rules))
						 .arg(gt.rules.internalTeachersList[tc]->name)
						 .arg(daysTeacherIsAvailable[tc]);

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
					
						if(t==0)
							return ok;
					}
				for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++)
					if(requestedDaysForSubgroups[sb]>daysSubgroupIsAvailable[sb]){
						ok=false;
						
						QString s=GeneratePreTranslate::tr("%1 cannot be respected because subgroup %2 has at most"
						 " %3 available days from students set not available and breaks."
						 " Please lower the weight of this constraint to a value below 100% (it depends"
						 " on your situation, if 0% is too little, make it 90%, 95% or even 99.75%."
						 " Even a large weight should not slow down much the program."
						 " A situation where you may need to make it larger than 0% is for instance if you have 5 activities with 4"
						 " possible days. You want to spread them 1, 1, 1 and 2, not 2, 2 and 1)", "%1 is the detailed description of a constraint"
						 )
						 .arg(md->getDetailedDescription(gt.rules))
						 .arg(gt.rules.internalSubgroupsList[sb]->name)
						 .arg(daysSubgroupIsAvailable[sb]);

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
					
						if(t==0)
							return ok;
					}
			}
		}
	}
	
	return ok;
}

bool checkMinDaysMaxTwoOnSameDay(QWidget* parent)
{
	bool ok=true;

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
			daysTeacherIsAvailable[tc]=0;

			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				bool dayAvailable=false;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h]){
						dayAvailable=true;
						break;
					}

				if(dayAvailable)
					daysTeacherIsAvailable[tc]++;
			}

			if(teachersMaxDaysPerWeekMaxDays[tc]>=0){ //it has compulsory 100% weight
				assert(teachersMaxDaysPerWeekWeightPercentages[tc]==100);
				daysTeacherIsAvailable[tc]=min(daysTeacherIsAvailable[tc], teachersMaxDaysPerWeekMaxDays[tc]);
			}

			/*if(gt.rules.mode==MORNINGS_AFTERNOONS){
				if(teachersMaxRealDaysPerWeekMaxDays[tc]>=0){ //it has compulsory 100% weight
					assert(teachersMaxRealDaysPerWeekWeightPercentages[tc]==100);
					daysTeacherIsAvailable[tc]=min(daysTeacherIsAvailable[tc], teachersMaxRealDaysPerWeekMaxDays[tc]);
				}
			}*/
		}

		for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
			daysSubgroupIsAvailable[sb]=0;

			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				bool dayAvailable=false;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[sb][d][h]){
						dayAvailable=true;
						break;
					}

				if(dayAvailable)
					daysSubgroupIsAvailable[sb]++;
			}

			if(subgroupsMaxDaysPerWeekMaxDays[sb]>=0){ //it has compulsory 100% weight
				assert(subgroupsMaxDaysPerWeekWeightPercentages[sb]==100);
				daysSubgroupIsAvailable[sb]=min(daysSubgroupIsAvailable[sb], subgroupsMaxDaysPerWeekMaxDays[sb]);
			}

			/*if(gt.rules.mode==MORNINGS_AFTERNOONS){
				if(subgroupsMaxRealDaysPerWeekMaxDays[sb]>=0){ //it has compulsory 100% weight
					assert(subgroupsMaxRealDaysPerWeekWeightPercentages[sb]==100);
					daysSubgroupIsAvailable[sb]=min(daysSubgroupIsAvailable[sb], subgroupsMaxRealDaysPerWeekMaxDays[sb]);
				}
			}*/
		}

		for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
				ConstraintMinDaysBetweenActivities* md=(ConstraintMinDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
				if(1/*md->consecutiveIfSameDay*/){
					for(int tc=0; tc<gt.rules.nInternalTeachers; tc++)
						nReqForTeacher[tc]=0;
					for(int j=0; j<md->_n_activities; j++){
						int ai=md->_activities[j];
						for(int k=0; k<gt.rules.internalActivitiesList[ai].iTeachersList.count(); k++){
							int tc=gt.rules.internalActivitiesList[ai].iTeachersList.at(k);
							nReqForTeacher[tc]++;
						}
					}

					for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
						if(2*daysTeacherIsAvailable[tc] < nReqForTeacher[tc]){
							ok=false;

							QString s=GeneratePreTranslate::tr("%1 cannot be respected because teacher %2 has at most"
							" %3 available days. Currently FET cannot put more than 2 activities on the same day."
							" You have 2*available days<number of activities in this constraint.",
							"%1 is the detailed description of a constraint"
							)
							.arg(md->getDetailedDescription(gt.rules))
							.arg(gt.rules.internalTeachersList[tc]->name)
							.arg(daysTeacherIsAvailable[tc]);

							int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
							GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
							1, 0 );

							if(t==0)
								return ok;
						}
					}

					for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++)
						nReqForSubgroup[sb]=0;
					for(int j=0; j<md->_n_activities; j++){
						int ai=md->_activities[j];
						for(int k=0; k<gt.rules.internalActivitiesList[ai].iSubgroupsList.count(); k++){
							int sb=gt.rules.internalActivitiesList[ai].iSubgroupsList.at(k);
							nReqForSubgroup[sb]++;
						}
					}

					for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
						if(2*daysSubgroupIsAvailable[sb] < nReqForSubgroup[sb]){
							ok=false;

							QString s=GeneratePreTranslate::tr("%1 cannot be respected because subgroup %2 has at most"
							" %3 available days. Currently FET cannot put more than 2 activities on the same day."
							" You have 2*available days<number of activities in this constraint.",
							"%1 is the detailed description of a constraint"
							)
							.arg(md->getDetailedDescription(gt.rules))
							.arg(gt.rules.internalSubgroupsList[sb]->name)
							.arg(daysSubgroupIsAvailable[sb]);

							int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
							GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
							1, 0 );

							if(t==0)
								return ok;
						}
					}
				}
			}
		}
	}
	else{
		for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
			daysTeacherIsAvailable[tc]=0;

			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++){
				bool dayAvailable=false;
				for(int h=0; h<gt.rules.nHoursPerDay; h++){
					if(!breakDayHour[2*d][h] && !teacherNotAvailableDayHour[tc][2*d][h]){
						dayAvailable=true;
						break;
					}
					if(!breakDayHour[2*d+1][h] && !teacherNotAvailableDayHour[tc][2*d+1][h]){
						dayAvailable=true;
						break;
					}
				}

				if(dayAvailable)
					daysTeacherIsAvailable[tc]++;
			}

			//Careful: here we cannot divide by 2.
			if(teachersMaxDaysPerWeekMaxDays[tc]>=0){ //it has compulsory 100% weight
				assert(teachersMaxDaysPerWeekWeightPercentages[tc]==100);
				daysTeacherIsAvailable[tc]=min(daysTeacherIsAvailable[tc], teachersMaxDaysPerWeekMaxDays[tc]);
			}

			if(teachersMaxRealDaysPerWeekMaxDays[tc]>=0){ //it has compulsory 100% weight
				assert(teachersMaxRealDaysPerWeekWeightPercentages[tc]==100);
				daysTeacherIsAvailable[tc]=min(daysTeacherIsAvailable[tc], teachersMaxRealDaysPerWeekMaxDays[tc]);
			}
		}

		for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
			daysSubgroupIsAvailable[sb]=0;

			for(int d=0; d<gt.rules.nDaysPerWeek/2; d++){
				bool dayAvailable=false;
				for(int h=0; h<gt.rules.nHoursPerDay; h++){
					if(!breakDayHour[2*d][h] && !subgroupNotAvailableDayHour[sb][2*d][h]){
						dayAvailable=true;
						break;
					}
					if(!breakDayHour[2*d+1][h] && !subgroupNotAvailableDayHour[sb][2*d+1][h]){
						dayAvailable=true;
						break;
					}
				}

				if(dayAvailable)
					daysSubgroupIsAvailable[sb]++;
			}

			//Careful: here we cannot divide by 2.
			if(subgroupsMaxDaysPerWeekMaxDays[sb]>=0){ //it has compulsory 100% weight
				assert(subgroupsMaxDaysPerWeekWeightPercentages[sb]==100);
				daysSubgroupIsAvailable[sb]=min(daysSubgroupIsAvailable[sb], subgroupsMaxDaysPerWeekMaxDays[sb]);
			}

			if(subgroupsMaxRealDaysPerWeekMaxDays[sb]>=0){ //it has compulsory 100% weight
				assert(subgroupsMaxRealDaysPerWeekWeightPercentages[sb]==100);
				daysSubgroupIsAvailable[sb]=min(daysSubgroupIsAvailable[sb], subgroupsMaxRealDaysPerWeekMaxDays[sb]);
			}
		}

		for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
				ConstraintMinDaysBetweenActivities* md=(ConstraintMinDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
				if(1/*md->consecutiveIfSameDay*/){
					for(int tc=0; tc<gt.rules.nInternalTeachers; tc++)
						nReqForTeacher[tc]=0;
					for(int j=0; j<md->_n_activities; j++){
						int ai=md->_activities[j];
						for(int k=0; k<gt.rules.internalActivitiesList[ai].iTeachersList.count(); k++){
							int tc=gt.rules.internalActivitiesList[ai].iTeachersList.at(k);
							nReqForTeacher[tc]++;
						}
					}

					for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
						if(2*daysTeacherIsAvailable[tc] < nReqForTeacher[tc]){
							ok=false;

							QString s=GeneratePreTranslate::tr("%1 cannot be respected because teacher %2 has at most"
							" %3 available real days. Currently FET cannot put more than 2 activities in the same real day."
							" You have 2*available days<number of activities in this constraint.",
							"%1 is the detailed description of a constraint"
							)
							.arg(md->getDetailedDescription(gt.rules))
							.arg(gt.rules.internalTeachersList[tc]->name)
							.arg(daysTeacherIsAvailable[tc]);

							int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
							GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
							1, 0 );

							if(t==0)
								return ok;
						}
					}

					for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++)
						nReqForSubgroup[sb]=0;
					for(int j=0; j<md->_n_activities; j++){
						int ai=md->_activities[j];
						for(int k=0; k<gt.rules.internalActivitiesList[ai].iSubgroupsList.count(); k++){
							int sb=gt.rules.internalActivitiesList[ai].iSubgroupsList.at(k);
							nReqForSubgroup[sb]++;
						}
					}

					for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
						if(2*daysSubgroupIsAvailable[sb] < nReqForSubgroup[sb]){
							ok=false;

							QString s=GeneratePreTranslate::tr("%1 cannot be respected because subgroup %2 has at most"
							" %3 available real days. Currently FET cannot put more than 2 activities in the same real day."
							" You have 2*available days<number of activities in this constraint.",
							"%1 is the detailed description of a constraint"
							)
							.arg(md->getDetailedDescription(gt.rules))
							.arg(gt.rules.internalSubgroupsList[sb]->name)
							.arg(daysSubgroupIsAvailable[sb]);

							int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
							GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
							1, 0 );

							if(t==0)
								return ok;
						}
					}
				}
			}
		}
	}
	
	return ok;
}

bool checkMinHalfDays100Percent(QWidget* parent)
{
	bool ok=true;
	
	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		return ok;
	
	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		daysTeacherIsAvailable[tc]=0;

		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			bool dayAvailable=false;
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h]){
					dayAvailable=true;
					break;
				}
				
			if(dayAvailable)
				daysTeacherIsAvailable[tc]++;
		}

		if(teachersMaxDaysPerWeekMaxDays[tc]>=0){ //it has compulsory 100% weight
			assert(teachersMaxDaysPerWeekWeightPercentages[tc]==100);
			daysTeacherIsAvailable[tc]=min(daysTeacherIsAvailable[tc], teachersMaxDaysPerWeekMaxDays[tc]);
		}

		/*if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(teachersMaxRealDaysPerWeekMaxDays[tc]>=0){ //it has compulsory 100% weight
				assert(teachersMaxRealDaysPerWeekWeightPercentages[tc]==100);
				daysTeacherIsAvailable[tc]=min(daysTeacherIsAvailable[tc], teachersMaxRealDaysPerWeekMaxDays[tc]);
			}
		}*/
	}

	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		daysSubgroupIsAvailable[sb]=0;

		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			bool dayAvailable=false;
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[sb][d][h]){
					dayAvailable=true;
					break;
				}
				
			if(dayAvailable)
				daysSubgroupIsAvailable[sb]++;
		}

		if(subgroupsMaxDaysPerWeekMaxDays[sb]>=0){ //it has compulsory 100% weight
			assert(subgroupsMaxDaysPerWeekWeightPercentages[sb]==100);
			daysSubgroupIsAvailable[sb]=min(daysSubgroupIsAvailable[sb], subgroupsMaxDaysPerWeekMaxDays[sb]);
		}

		/*if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(subgroupsMaxRealDaysPerWeekMaxDays[sb]>=0){ //it has compulsory 100% weight
				assert(subgroupsMaxRealDaysPerWeekWeightPercentages[sb]==100);
				daysSubgroupIsAvailable[sb]=min(daysSubgroupIsAvailable[sb], subgroupsMaxRealDaysPerWeekMaxDays[sb]);
			}
		}*/
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES
		 &&gt.rules.internalTimeConstraintsList[i]->weightPercentage==100.0){
			ConstraintMinHalfDaysBetweenActivities* md=(ConstraintMinHalfDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
			
			if(md->minDays>=1){
				int na=md->_n_activities;
				int nd=md->minDays;
				if((na-1)*nd+1 > gt.rules.nDaysPerWeek){
					ok=false;
					
					QString s=GeneratePreTranslate::tr("%1 cannot be respected because it contains %2 activities,"
					 " has weight 100% and has min number of half days between activities=%3. The minimum required number of days per week for"
					 " that would be (nactivities-1)*mindays+1=%4, and you have only %5 days per week - impossible. Please correct this constraint.", "%1 is the detailed description of a constraint"
					)
					 .arg(md->getDetailedDescription(gt.rules))
					 .arg(na)
					 .arg(nd)
					 .arg((na-1)*nd+1)
					 .arg(gt.rules.nDaysPerWeek)
					 ;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
					
					if(t==0)
						return ok;
				}
			}
			
			if(md->minDays>=1){
				for(int tc=0; tc<gt.rules.nInternalTeachers; tc++)
					requestedDaysForTeachers[tc]=0;
				for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++)
					requestedDaysForSubgroups[sb]=0;
			
				for(int j=0; j<md->_n_activities; j++){
					int ai=md->_activities[j];
					for(int k=0; k<gt.rules.internalActivitiesList[ai].iTeachersList.count(); k++){
						int tc=gt.rules.internalActivitiesList[ai].iTeachersList.at(k);
						requestedDaysForTeachers[tc]++;
					}
					for(int k=0; k<gt.rules.internalActivitiesList[ai].iSubgroupsList.count(); k++){
						int sb=gt.rules.internalActivitiesList[ai].iSubgroupsList.at(k);
						requestedDaysForSubgroups[sb]++;
					}
				}
				for(int tc=0; tc<gt.rules.nInternalTeachers; tc++)
					if(requestedDaysForTeachers[tc]>daysTeacherIsAvailable[tc]){
						ok=false;
						
						QString s=GeneratePreTranslate::tr("%1 cannot be respected because teacher %2 has at most"
						 " %3 available half days from teacher not available, breaks and teacher max days per week."
						 " Please lower the weight of this constraint to a value below 100% (it depends"
						 " on your situation, if 0% is too little, make it 90%, 95% or even 99.75%."
						 " Even a large weight should not slow down much the program."
						 " A situation where you may need to make it larger than 0% is for instance if you have 5 activities with 4"
						 " possible days. You want to spread them 1, 1, 1 and 2, not 2, 2 and 1)", "%1 is the detailed description of a constraint"
						)
						 .arg(md->getDetailedDescription(gt.rules))
						 .arg(gt.rules.internalTeachersList[tc]->name)
						 .arg(daysTeacherIsAvailable[tc]);

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
					
						if(t==0)
							return ok;
					}
				for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++)
					if(requestedDaysForSubgroups[sb]>daysSubgroupIsAvailable[sb]){
						ok=false;
						
						QString s=GeneratePreTranslate::tr("%1 cannot be respected because subgroup %2 has at most"
						 " %3 available half days from students set not available and breaks."
						 " Please lower the weight of this constraint to a value below 100% (it depends"
						 " on your situation, if 0% is too little, make it 90%, 95% or even 99.75%."
						 " Even a large weight should not slow down much the program."
						 " A situation where you may need to make it larger than 0% is for instance if you have 5 activities with 4"
						 " possible days. You want to spread them 1, 1, 1 and 2, not 2, 2 and 1)", "%1 is the detailed description of a constraint"
						 )
						 .arg(md->getDetailedDescription(gt.rules))
						 .arg(gt.rules.internalSubgroupsList[sb]->name)
						 .arg(daysSubgroupIsAvailable[sb]);

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
					
						if(t==0)
							return ok;
					}
			}
		}
	}
	
	return ok;
}

bool checkMinHalfDaysMaxTwoOnSameDay(QWidget* parent)
{
	bool ok=true;

	if(gt.rules.mode!=MORNINGS_AFTERNOONS)
		return ok;

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		daysTeacherIsAvailable[tc]=0;

		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			bool dayAvailable=false;
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h]){
					dayAvailable=true;
					break;
				}
				
			if(dayAvailable)
				daysTeacherIsAvailable[tc]++;
		}

		if(teachersMaxDaysPerWeekMaxDays[tc]>=0){ //it has compulsory 100% weight
			assert(teachersMaxDaysPerWeekWeightPercentages[tc]==100);
			daysTeacherIsAvailable[tc]=min(daysTeacherIsAvailable[tc], teachersMaxDaysPerWeekMaxDays[tc]);
		}

		/*if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(teachersMaxRealDaysPerWeekMaxDays[tc]>=0){ //it has compulsory 100% weight
				assert(teachersMaxRealDaysPerWeekWeightPercentages[tc]==100);
				daysTeacherIsAvailable[tc]=min(daysTeacherIsAvailable[tc], teachersMaxRealDaysPerWeekMaxDays[tc]);
			}
		}*/
	}

	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		daysSubgroupIsAvailable[sb]=0;

		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			bool dayAvailable=false;
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[sb][d][h]){
					dayAvailable=true;
					break;
				}
				
			if(dayAvailable)
				daysSubgroupIsAvailable[sb]++;
		}

		if(subgroupsMaxDaysPerWeekMaxDays[sb]>=0){ //it has compulsory 100% weight
			assert(subgroupsMaxDaysPerWeekWeightPercentages[sb]==100);
			daysSubgroupIsAvailable[sb]=min(daysSubgroupIsAvailable[sb], subgroupsMaxDaysPerWeekMaxDays[sb]);
		}

		/*if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(subgroupsMaxRealDaysPerWeekMaxDays[sb]>=0){ //it has compulsory 100% weight
				assert(subgroupsMaxRealDaysPerWeekWeightPercentages[sb]==100);
				daysSubgroupIsAvailable[sb]=min(daysSubgroupIsAvailable[sb], subgroupsMaxRealDaysPerWeekMaxDays[sb]);
			}
		}*/
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES){
			ConstraintMinHalfDaysBetweenActivities* md=(ConstraintMinHalfDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
			if(1/*md->consecutiveIfSameDay*/){
				for(int tc=0; tc<gt.rules.nInternalTeachers; tc++)
					nReqForTeacher[tc]=0;
				for(int j=0; j<md->_n_activities; j++){
					int ai=md->_activities[j];
					for(int k=0; k<gt.rules.internalActivitiesList[ai].iTeachersList.count(); k++){
						int tc=gt.rules.internalActivitiesList[ai].iTeachersList.at(k);
						nReqForTeacher[tc]++;
					}
				}
			
				for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
					if(2*daysTeacherIsAvailable[tc] < nReqForTeacher[tc]){
						ok=false;
						
						QString s=GeneratePreTranslate::tr("%1 cannot be respected because teacher %2 has at most"
						 " %3 available half days. Currently FET cannot put more than 2 activities on the same day."
						 " You have 2*available days<number of activities in this constraint.",
						 "%1 is the detailed description of a constraint"
						)
						 .arg(md->getDetailedDescription(gt.rules))
						 .arg(gt.rules.internalTeachersList[tc]->name)
						 .arg(daysTeacherIsAvailable[tc]);
	
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
						
						if(t==0)
							return ok;
					}
				}

				for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++)
					nReqForSubgroup[sb]=0;
				for(int j=0; j<md->_n_activities; j++){
					int ai=md->_activities[j];
					for(int k=0; k<gt.rules.internalActivitiesList[ai].iSubgroupsList.count(); k++){
						int sb=gt.rules.internalActivitiesList[ai].iSubgroupsList.at(k);
						nReqForSubgroup[sb]++;
					}
				}
			
				for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
					if(2*daysSubgroupIsAvailable[sb] < nReqForSubgroup[sb]){
						ok=false;
						
						QString s=GeneratePreTranslate::tr("%1 cannot be respected because subgroup %2 has at most"
						 " %3 available half days. Currently FET cannot put more than 2 activities on the same day."
						 " You have 2*available days<number of activities in this constraint.",
						 "%1 is the detailed description of a constraint"
						)
						 .arg(md->getDetailedDescription(gt.rules))
						 .arg(gt.rules.internalSubgroupsList[sb]->name)
						 .arg(daysSubgroupIsAvailable[sb]);

						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
					
						if(t==0)
							return ok;
					}
				}
			}
		}
	}
	
	return ok;
}

bool computeTeachersIntervalMaxDaysPerWeek(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersIntervalMaxDaysPerWeekPercentages[i].clear();
		teachersIntervalMaxDaysPerWeekMaxDays[i].clear();
		teachersIntervalMaxDaysPerWeekIntervalStart[i].clear();
		teachersIntervalMaxDaysPerWeekIntervalEnd[i].clear();
	}
	
	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintTeacherIntervalMaxDaysPerWeek* tn=(ConstraintTeacherIntervalMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher interval max days per week with"
				 " weight (percentage) below 100 for teacher %1. Starting with FET version 5.6.2 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tn->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				
				if(t==0)
					return false;
			}

			bool exists=false;
			for(int j=0; j<teachersIntervalMaxDaysPerWeekPercentages[tn->teacher_ID].count(); j++)
				if(teachersIntervalMaxDaysPerWeekPercentages[tn->teacher_ID].at(j)==tn->weightPercentage &&
				 teachersIntervalMaxDaysPerWeekMaxDays[tn->teacher_ID].at(j)==tn->maxDaysPerWeek &&
				 teachersIntervalMaxDaysPerWeekIntervalStart[tn->teacher_ID].at(j)==tn->startHour &&
				 teachersIntervalMaxDaysPerWeekIntervalEnd[tn->teacher_ID].at(j)==tn->endHour){
					exists=true;
					break;
				}
			
			if(!exists){
				teachersIntervalMaxDaysPerWeekPercentages[tn->teacher_ID].append(tn->weightPercentage);
				teachersIntervalMaxDaysPerWeekMaxDays[tn->teacher_ID].append(tn->maxDaysPerWeek);
				teachersIntervalMaxDaysPerWeekIntervalStart[tn->teacher_ID].append(tn->startHour);
				teachersIntervalMaxDaysPerWeekIntervalEnd[tn->teacher_ID].append(tn->endHour);
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintTeachersIntervalMaxDaysPerWeek* tn=(ConstraintTeachersIntervalMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers interval max days per week with"
				 " weight (percentage) below 100. Starting with FET version 5.6.2 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				
				if(t==0)
					return false;
			}

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				bool exists=false;
				for(int j=0; j<teachersIntervalMaxDaysPerWeekPercentages[tch].count(); j++)
					if(teachersIntervalMaxDaysPerWeekPercentages[tch].at(j)==tn->weightPercentage &&
					 teachersIntervalMaxDaysPerWeekMaxDays[tch].at(j)==tn->maxDaysPerWeek &&
					 teachersIntervalMaxDaysPerWeekIntervalStart[tch].at(j)==tn->startHour &&
					 teachersIntervalMaxDaysPerWeekIntervalEnd[tch].at(j)==tn->endHour){
						exists=true;
						break;
					}
			
				if(!exists){
					teachersIntervalMaxDaysPerWeekPercentages[tch].append(tn->weightPercentage);
					teachersIntervalMaxDaysPerWeekMaxDays[tch].append(tn->maxDaysPerWeek);
					teachersIntervalMaxDaysPerWeekIntervalStart[tch].append(tn->startHour);
					teachersIntervalMaxDaysPerWeekIntervalEnd[tch].append(tn->endHour);
				}
			}
		}
	}
	
	return ok;
}

//morning
bool computeTeachersMorningIntervalMaxDaysPerWeek(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMorningIntervalMaxDaysPerWeekPercentages[i].clear();
		teachersMorningIntervalMaxDaysPerWeekMaxDays[i].clear();
		teachersMorningIntervalMaxDaysPerWeekIntervalStart[i].clear();
		teachersMorningIntervalMaxDaysPerWeekIntervalEnd[i].clear();
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MORNING_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintTeacherMorningIntervalMaxDaysPerWeek* tn=(ConstraintTeacherMorningIntervalMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher morning interval max days per week with"
				 " weight (percentage) below 100 for teacher %1. Starting with FET version 5.6.2 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tn->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			bool exists=false;
			for(int j=0; j<teachersMorningIntervalMaxDaysPerWeekPercentages[tn->teacher_ID].count(); j++)
				if(teachersMorningIntervalMaxDaysPerWeekPercentages[tn->teacher_ID].at(j)==tn->weightPercentage &&
				 teachersMorningIntervalMaxDaysPerWeekMaxDays[tn->teacher_ID].at(j)==tn->maxDaysPerWeek &&
				 teachersMorningIntervalMaxDaysPerWeekIntervalStart[tn->teacher_ID].at(j)==tn->startHour &&
				 teachersMorningIntervalMaxDaysPerWeekIntervalEnd[tn->teacher_ID].at(j)==tn->endHour){
					exists=true;
					break;
				}

			if(!exists){
				teachersMorningIntervalMaxDaysPerWeekPercentages[tn->teacher_ID].append(tn->weightPercentage);
				teachersMorningIntervalMaxDaysPerWeekMaxDays[tn->teacher_ID].append(tn->maxDaysPerWeek);
				teachersMorningIntervalMaxDaysPerWeekIntervalStart[tn->teacher_ID].append(tn->startHour);
				teachersMorningIntervalMaxDaysPerWeekIntervalEnd[tn->teacher_ID].append(tn->endHour);
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintTeachersMorningIntervalMaxDaysPerWeek* tn=(ConstraintTeachersMorningIntervalMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers morning interval max days per week with"
				 " weight (percentage) below 100. Starting with FET version 5.6.2 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				bool exists=false;
				for(int j=0; j<teachersMorningIntervalMaxDaysPerWeekPercentages[tch].count(); j++)
					if(teachersMorningIntervalMaxDaysPerWeekPercentages[tch].at(j)==tn->weightPercentage &&
					 teachersMorningIntervalMaxDaysPerWeekMaxDays[tch].at(j)==tn->maxDaysPerWeek &&
					 teachersMorningIntervalMaxDaysPerWeekIntervalStart[tch].at(j)==tn->startHour &&
					 teachersMorningIntervalMaxDaysPerWeekIntervalEnd[tch].at(j)==tn->endHour){
						exists=true;
						break;
					}

				if(!exists){
					teachersMorningIntervalMaxDaysPerWeekPercentages[tch].append(tn->weightPercentage);
					teachersMorningIntervalMaxDaysPerWeekMaxDays[tch].append(tn->maxDaysPerWeek);
					teachersMorningIntervalMaxDaysPerWeekIntervalStart[tch].append(tn->startHour);
					teachersMorningIntervalMaxDaysPerWeekIntervalEnd[tch].append(tn->endHour);
				}
			}
		}
	}

	return ok;
}

//afternoon
bool computeTeachersAfternoonIntervalMaxDaysPerWeek(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersAfternoonIntervalMaxDaysPerWeekPercentages[i].clear();
		teachersAfternoonIntervalMaxDaysPerWeekMaxDays[i].clear();
		teachersAfternoonIntervalMaxDaysPerWeekIntervalStart[i].clear();
		teachersAfternoonIntervalMaxDaysPerWeekIntervalEnd[i].clear();
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintTeacherAfternoonIntervalMaxDaysPerWeek* tn=(ConstraintTeacherAfternoonIntervalMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teacher afternoon interval max days per week with"
				 " weight (percentage) below 100 for teacher %1. Starting with FET version 5.6.2 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tn->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			bool exists=false;
			for(int j=0; j<teachersAfternoonIntervalMaxDaysPerWeekPercentages[tn->teacher_ID].count(); j++)
				if(teachersAfternoonIntervalMaxDaysPerWeekPercentages[tn->teacher_ID].at(j)==tn->weightPercentage &&
				 teachersAfternoonIntervalMaxDaysPerWeekMaxDays[tn->teacher_ID].at(j)==tn->maxDaysPerWeek &&
				 teachersAfternoonIntervalMaxDaysPerWeekIntervalStart[tn->teacher_ID].at(j)==tn->startHour &&
				 teachersAfternoonIntervalMaxDaysPerWeekIntervalEnd[tn->teacher_ID].at(j)==tn->endHour){
					exists=true;
					break;
				}

			if(!exists){
				teachersAfternoonIntervalMaxDaysPerWeekPercentages[tn->teacher_ID].append(tn->weightPercentage);
				teachersAfternoonIntervalMaxDaysPerWeekMaxDays[tn->teacher_ID].append(tn->maxDaysPerWeek);
				teachersAfternoonIntervalMaxDaysPerWeekIntervalStart[tn->teacher_ID].append(tn->startHour);
				teachersAfternoonIntervalMaxDaysPerWeekIntervalEnd[tn->teacher_ID].append(tn->endHour);
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintTeachersAfternoonIntervalMaxDaysPerWeek* tn=(ConstraintTeachersAfternoonIntervalMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint teachers afternoon interval max days per week with"
				 " weight (percentage) below 100. Starting with FET version 5.6.2 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				bool exists=false;
				for(int j=0; j<teachersAfternoonIntervalMaxDaysPerWeekPercentages[tch].count(); j++)
					if(teachersAfternoonIntervalMaxDaysPerWeekPercentages[tch].at(j)==tn->weightPercentage &&
					 teachersAfternoonIntervalMaxDaysPerWeekMaxDays[tch].at(j)==tn->maxDaysPerWeek &&
					 teachersAfternoonIntervalMaxDaysPerWeekIntervalStart[tch].at(j)==tn->startHour &&
					 teachersAfternoonIntervalMaxDaysPerWeekIntervalEnd[tch].at(j)==tn->endHour){
						exists=true;
						break;
					}

				if(!exists){
					teachersAfternoonIntervalMaxDaysPerWeekPercentages[tch].append(tn->weightPercentage);
					teachersAfternoonIntervalMaxDaysPerWeekMaxDays[tch].append(tn->maxDaysPerWeek);
					teachersAfternoonIntervalMaxDaysPerWeekIntervalStart[tch].append(tn->startHour);
					teachersAfternoonIntervalMaxDaysPerWeekIntervalEnd[tch].append(tn->endHour);
				}
			}
		}
	}

	return ok;
}

bool computeSubgroupsIntervalMaxDaysPerWeek(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsIntervalMaxDaysPerWeekPercentages[i].clear();
		subgroupsIntervalMaxDaysPerWeekMaxDays[i].clear();
		subgroupsIntervalMaxDaysPerWeekIntervalStart[i].clear();
		subgroupsIntervalMaxDaysPerWeekIntervalEnd[i].clear();
	}
	
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintStudentsSetIntervalMaxDaysPerWeek* cn=(ConstraintStudentsSetIntervalMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set interval max days per week with"
				 " weight (percentage) below 100 for students set %1. Starting with FET version 5.6.2 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(cn->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				
				if(t==0)
					return false;
			}
			
			for(int sbg : qAsConst(cn->iSubgroupsList)){
				bool exists=false;
				for(int j=0; j<subgroupsIntervalMaxDaysPerWeekPercentages[sbg].count(); j++)
					if(subgroupsIntervalMaxDaysPerWeekPercentages[sbg].at(j)==cn->weightPercentage &&
					 subgroupsIntervalMaxDaysPerWeekMaxDays[sbg].at(j)==cn->maxDaysPerWeek &&
					 subgroupsIntervalMaxDaysPerWeekIntervalStart[sbg].at(j)==cn->startHour &&
					 subgroupsIntervalMaxDaysPerWeekIntervalEnd[sbg].at(j)==cn->endHour){
						exists=true;
						break;
					}
				
				if(!exists){
					subgroupsIntervalMaxDaysPerWeekPercentages[sbg].append(cn->weightPercentage);
					subgroupsIntervalMaxDaysPerWeekMaxDays[sbg].append(cn->maxDaysPerWeek);
					subgroupsIntervalMaxDaysPerWeekIntervalStart[sbg].append(cn->startHour);
					subgroupsIntervalMaxDaysPerWeekIntervalEnd[sbg].append(cn->endHour);
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintStudentsIntervalMaxDaysPerWeek* cn=(ConstraintStudentsIntervalMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students interval max days per week with"
				 " weight (percentage) below 100. Starting with FET version 5.6.2 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 //.arg(cn->students),
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				
				if(t==0)
					return false;
			}
			
			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				bool exists=false;
				for(int j=0; j<subgroupsIntervalMaxDaysPerWeekPercentages[sbg].count(); j++)
					if(subgroupsIntervalMaxDaysPerWeekPercentages[sbg].at(j)==cn->weightPercentage &&
					 subgroupsIntervalMaxDaysPerWeekMaxDays[sbg].at(j)==cn->maxDaysPerWeek &&
					 subgroupsIntervalMaxDaysPerWeekIntervalStart[sbg].at(j)==cn->startHour &&
					 subgroupsIntervalMaxDaysPerWeekIntervalEnd[sbg].at(j)==cn->endHour){
						exists=true;
						break;
					}
				
				if(!exists){
					subgroupsIntervalMaxDaysPerWeekPercentages[sbg].append(cn->weightPercentage);
					subgroupsIntervalMaxDaysPerWeekMaxDays[sbg].append(cn->maxDaysPerWeek);
					subgroupsIntervalMaxDaysPerWeekIntervalStart[sbg].append(cn->startHour);
					subgroupsIntervalMaxDaysPerWeekIntervalEnd[sbg].append(cn->endHour);
				}
			}
		}
	}

	return ok;
}

bool computeSubgroupsMorningIntervalMaxDaysPerWeek(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMorningIntervalMaxDaysPerWeekPercentages[i].clear();
		subgroupsMorningIntervalMaxDaysPerWeekMaxDays[i].clear();
		subgroupsMorningIntervalMaxDaysPerWeekIntervalStart[i].clear();
		subgroupsMorningIntervalMaxDaysPerWeekIntervalEnd[i].clear();
	}

	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MORNING_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintStudentsSetMorningIntervalMaxDaysPerWeek* cn=(ConstraintStudentsSetMorningIntervalMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set morning interval max days per week with"
				 " weight (percentage) below 100 for students set %1. Please make weight 100% and try again")
				 .arg(cn->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int sbg : qAsConst(cn->iSubgroupsList)){
				bool exists=false;
				for(int j=0; j<subgroupsMorningIntervalMaxDaysPerWeekPercentages[sbg].count(); j++)
					if(subgroupsMorningIntervalMaxDaysPerWeekPercentages[sbg].at(j)==cn->weightPercentage &&
					 subgroupsMorningIntervalMaxDaysPerWeekMaxDays[sbg].at(j)==cn->maxDaysPerWeek &&
					 subgroupsMorningIntervalMaxDaysPerWeekIntervalStart[sbg].at(j)==cn->startHour &&
					 subgroupsMorningIntervalMaxDaysPerWeekIntervalEnd[sbg].at(j)==cn->endHour){
						exists=true;
						break;
					}

				if(!exists){
					subgroupsMorningIntervalMaxDaysPerWeekPercentages[sbg].append(cn->weightPercentage);
					subgroupsMorningIntervalMaxDaysPerWeekMaxDays[sbg].append(cn->maxDaysPerWeek);
					subgroupsMorningIntervalMaxDaysPerWeekIntervalStart[sbg].append(cn->startHour);
					subgroupsMorningIntervalMaxDaysPerWeekIntervalEnd[sbg].append(cn->endHour);
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintStudentsMorningIntervalMaxDaysPerWeek* cn=(ConstraintStudentsMorningIntervalMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students morning interval max days per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again")
				 //.arg(cn->students),
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				bool exists=false;
				for(int j=0; j<subgroupsMorningIntervalMaxDaysPerWeekPercentages[sbg].count(); j++)
					if(subgroupsMorningIntervalMaxDaysPerWeekPercentages[sbg].at(j)==cn->weightPercentage &&
					 subgroupsMorningIntervalMaxDaysPerWeekMaxDays[sbg].at(j)==cn->maxDaysPerWeek &&
					 subgroupsMorningIntervalMaxDaysPerWeekIntervalStart[sbg].at(j)==cn->startHour &&
					 subgroupsMorningIntervalMaxDaysPerWeekIntervalEnd[sbg].at(j)==cn->endHour){
						exists=true;
						break;
					}

				if(!exists){
					subgroupsMorningIntervalMaxDaysPerWeekPercentages[sbg].append(cn->weightPercentage);
					subgroupsMorningIntervalMaxDaysPerWeekMaxDays[sbg].append(cn->maxDaysPerWeek);
					subgroupsMorningIntervalMaxDaysPerWeekIntervalStart[sbg].append(cn->startHour);
					subgroupsMorningIntervalMaxDaysPerWeekIntervalEnd[sbg].append(cn->endHour);
				}
			}
		}
	}

	return ok;
}

bool computeSubgroupsAfternoonIntervalMaxDaysPerWeek(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsAfternoonIntervalMaxDaysPerWeekPercentages[i].clear();
		subgroupsAfternoonIntervalMaxDaysPerWeekMaxDays[i].clear();
		subgroupsAfternoonIntervalMaxDaysPerWeekIntervalStart[i].clear();
		subgroupsAfternoonIntervalMaxDaysPerWeekIntervalEnd[i].clear();
	}

	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek* cn=(ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students set afternoon interval max days per week with"
				 " weight (percentage) below 100 for students set %1. Please make weight 100% and try again")
				 .arg(cn->students),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int sbg : qAsConst(cn->iSubgroupsList)){
				bool exists=false;
				for(int j=0; j<subgroupsAfternoonIntervalMaxDaysPerWeekPercentages[sbg].count(); j++)
					if(subgroupsAfternoonIntervalMaxDaysPerWeekPercentages[sbg].at(j)==cn->weightPercentage &&
					 subgroupsAfternoonIntervalMaxDaysPerWeekMaxDays[sbg].at(j)==cn->maxDaysPerWeek &&
					 subgroupsAfternoonIntervalMaxDaysPerWeekIntervalStart[sbg].at(j)==cn->startHour &&
					 subgroupsAfternoonIntervalMaxDaysPerWeekIntervalEnd[sbg].at(j)==cn->endHour){
						exists=true;
						break;
					}

				if(!exists){
					subgroupsAfternoonIntervalMaxDaysPerWeekPercentages[sbg].append(cn->weightPercentage);
					subgroupsAfternoonIntervalMaxDaysPerWeekMaxDays[sbg].append(cn->maxDaysPerWeek);
					subgroupsAfternoonIntervalMaxDaysPerWeekIntervalStart[sbg].append(cn->startHour);
					subgroupsAfternoonIntervalMaxDaysPerWeekIntervalEnd[sbg].append(cn->endHour);
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK){
			ConstraintStudentsAfternoonIntervalMaxDaysPerWeek* cn=(ConstraintStudentsAfternoonIntervalMaxDaysPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint students afternoon interval max days per week with"
				 " weight (percentage) below 100. Please make weight 100% and try again")
				 //.arg(cn->students),
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				bool exists=false;
				for(int j=0; j<subgroupsAfternoonIntervalMaxDaysPerWeekPercentages[sbg].count(); j++)
					if(subgroupsAfternoonIntervalMaxDaysPerWeekPercentages[sbg].at(j)==cn->weightPercentage &&
					 subgroupsAfternoonIntervalMaxDaysPerWeekMaxDays[sbg].at(j)==cn->maxDaysPerWeek &&
					 subgroupsAfternoonIntervalMaxDaysPerWeekIntervalStart[sbg].at(j)==cn->startHour &&
					 subgroupsAfternoonIntervalMaxDaysPerWeekIntervalEnd[sbg].at(j)==cn->endHour){
						exists=true;
						break;
					}

				if(!exists){
					subgroupsAfternoonIntervalMaxDaysPerWeekPercentages[sbg].append(cn->weightPercentage);
					subgroupsAfternoonIntervalMaxDaysPerWeekMaxDays[sbg].append(cn->maxDaysPerWeek);
					subgroupsAfternoonIntervalMaxDaysPerWeekIntervalStart[sbg].append(cn->startHour);
					subgroupsAfternoonIntervalMaxDaysPerWeekIntervalEnd[sbg].append(cn->endHour);
				}
			}
		}
	}

	return ok;
}

//2011-09-25
bool computeActivitiesOccupyMaxTimeSlotsFromSelection(QWidget* parent)
{
	haveActivitiesOccupyMaxConstraints=false;

	bool ok=true;
	
	aomtsList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		aomtsListForActivity[i].clear();
		activityHasOccupyMaxConstraints[i]=false;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TIME_SLOTS_FROM_SELECTION){
			if(!haveActivitiesOccupyMaxConstraints)
				haveActivitiesOccupyMaxConstraints=true;

			ConstraintActivitiesOccupyMaxTimeSlotsFromSelection* cn=(ConstraintActivitiesOccupyMaxTimeSlotsFromSelection*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'activities occupy max time slots from selection'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			ActivitiesOccupyMaxTimeSlotsFromSelection_item item;
			item.activitiesList=cn->_activitiesIndices;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.activitiesSet=QSet<int>(item.activitiesList.constBegin(), item.activitiesList.constEnd());
#else
			item.activitiesSet=item.activitiesList.toSet();
#endif
			item.maxOccupiedTimeSlots=cn->maxOccupiedTimeSlots;
			for(int t=0; t < cn->selectedDays.count(); t++)
				item.selectedTimeSlotsList.append(cn->selectedDays.at(t)+cn->selectedHours.at(t)*gt.rules.nDaysPerWeek);
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.selectedTimeSlotsSet=QSet<int>(item.selectedTimeSlotsList.constBegin(), item.selectedTimeSlotsList.constEnd());
#else
			item.selectedTimeSlotsSet=item.selectedTimeSlotsList.toSet();
#endif
			
			aomtsList.push_back(item);
			//ActivitiesOccupyMaxTimeSlotsFromSelection_item* p_item=&aomtsList[aomtsList.count()-1];
			ActivitiesOccupyMaxTimeSlotsFromSelection_item* p_item=&aomtsList.back();
			for(int ai : qAsConst(cn->_activitiesIndices)){
				aomtsListForActivity[ai].append(p_item);
				
				if(activityHasOccupyMaxConstraints[ai]==false)
					activityHasOccupyMaxConstraints[ai]=true;
			}
		}
	}
	
	return ok;
}

//2019-11-16
bool computeActivitiesOccupyMinTimeSlotsFromSelection(QWidget* parent)
{
	bool ok=true;
	
	aomintsList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		aomintsListForActivity[i].clear();

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_OCCUPY_MIN_TIME_SLOTS_FROM_SELECTION){
			ConstraintActivitiesOccupyMinTimeSlotsFromSelection* cn=(ConstraintActivitiesOccupyMinTimeSlotsFromSelection*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'activities occupy min time slots from selection'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			if(cn->selectedDays.count() < cn->minOccupiedTimeSlots){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have a constraint of type 'activities occupy min time slots from selection'"
				 " with the number of selected slots being %1, but the number of requested minimum slots is %2, which is greater - impossible. The constraint is:\n"
				 "%3\nPlease correct and try again.")
				 .arg(cn->selectedDays.count())
				 .arg(cn->minOccupiedTimeSlots)
				 .arg(cn->getDetailedDescription(gt.rules))
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			int totalDuration=0;
			for(int ai : qAsConst(cn->_activitiesIndices))
				totalDuration+=gt.rules.internalActivitiesList[ai].duration;
			if(totalDuration < cn->minOccupiedTimeSlots){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have a constraint of type 'activities occupy min time slots from selection'"
				 " with the total duration of the selected activities being %1, but the number of requested minimum slots is %2, which is greater - impossible."
				 " The constraint is:\n%3\nPlease correct and try again.")
				 .arg(totalDuration)
				 .arg(cn->minOccupiedTimeSlots)
				 .arg(cn->getDetailedDescription(gt.rules))
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			ActivitiesOccupyMinTimeSlotsFromSelection_item item;
			item.activitiesList=cn->_activitiesIndices;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.activitiesSet=QSet<int>(item.activitiesList.constBegin(), item.activitiesList.constEnd());
#else
			item.activitiesSet=item.activitiesList.toSet();
#endif
			item.minOccupiedTimeSlots=cn->minOccupiedTimeSlots;
			for(int t=0; t < cn->selectedDays.count(); t++)
				item.selectedTimeSlotsList.append(cn->selectedDays.at(t)+cn->selectedHours.at(t)*gt.rules.nDaysPerWeek);
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.selectedTimeSlotsSet=QSet<int>(item.selectedTimeSlotsList.constBegin(), item.selectedTimeSlotsList.constEnd());
#else
			item.selectedTimeSlotsSet=item.selectedTimeSlotsList.toSet();
#endif
			
			aomintsList.push_back(item);
			//ActivitiesOccupyMinTimeSlotsFromSelection_item* p_item=&aomintsList[aomintsList.count()-1];
			ActivitiesOccupyMinTimeSlotsFromSelection_item* p_item=&aomintsList.back();
			for(int ai : qAsConst(cn->_activitiesIndices))
				aomintsListForActivity[ai].append(p_item);
		}
	}
	
	return ok;
}

//2011-09-30
bool computeActivitiesMaxSimultaneousInSelectedTimeSlots(QWidget* parent)
{
	haveActivitiesMaxSimultaneousConstraints=false;

	bool ok=true;
	
	amsistsList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		amsistsListForActivity[i].clear();
		activityHasMaxSimultaneousConstraints[i]=false;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS){
			if(!haveActivitiesMaxSimultaneousConstraints)
				haveActivitiesMaxSimultaneousConstraints=true;
			
			ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots* cn=(ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'activities max simultaneous in selected time slots'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			ActivitiesMaxSimultaneousInSelectedTimeSlots_item item;
			item.activitiesList=cn->_activitiesIndices;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.activitiesSet=QSet<int>(item.activitiesList.constBegin(), item.activitiesList.constEnd());
#else
			item.activitiesSet=item.activitiesList.toSet();
#endif
			item.maxSimultaneous=cn->maxSimultaneous;
			for(int t=0; t < cn->selectedDays.count(); t++)
				item.selectedTimeSlotsList.append(cn->selectedDays.at(t)+cn->selectedHours.at(t)*gt.rules.nDaysPerWeek);
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.selectedTimeSlotsSet=QSet<int>(item.selectedTimeSlotsList.constBegin(), item.selectedTimeSlotsList.constEnd());
#else
			item.selectedTimeSlotsSet=item.selectedTimeSlotsList.toSet();
#endif
			
			amsistsList.push_back(item);
			//ActivitiesMaxSimultaneousInSelectedTimeSlots_item* p_item=&amsistsList[amsistsList.count()-1];
			ActivitiesMaxSimultaneousInSelectedTimeSlots_item* p_item=&amsistsList.back();
			for(int ai : qAsConst(cn->_activitiesIndices)){
				amsistsListForActivity[ai].append(p_item);
				
				if(activityHasMaxSimultaneousConstraints[ai]==false)
					activityHasMaxSimultaneousConstraints[ai]=true;
			}
		}
	}
	
	return ok;
}

//2019-11-16
bool computeActivitiesMinSimultaneousInSelectedTimeSlots(QWidget* parent)
{
	bool ok=true;
	
	aminsistsList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		aminsistsListForActivity[i].clear();

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_MIN_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS){
			ConstraintActivitiesMinSimultaneousInSelectedTimeSlots* cn=(ConstraintActivitiesMinSimultaneousInSelectedTimeSlots*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'activities min simultaneous in selected time slots'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			if(!cn->allowEmptySlots){
				int totalDuration=0;
				for(int ai : qAsConst(cn->_activitiesIndices))
					totalDuration+=gt.rules.internalActivitiesList[ai].duration;
				if(totalDuration < cn->minSimultaneous*cn->selectedDays.count()){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because you have a constraint of type 'activities min simultaneous in selected time slots'"
					 " with the total duration of the selected activities being %1, but the required number of occupying slots is"
					 " %2 (minimum simultaneous) x %3 (selected slots) = %4, which is greater - impossible (the constraint does not allow empty slots)."
					 " The constraint is:\n%5\nPlease correct and try again.")
					 .arg(totalDuration)
					 .arg(cn->minSimultaneous)
					 .arg(cn->selectedDays.count())
					 .arg(cn->minSimultaneous*cn->selectedDays.count())
					 .arg(cn->getDetailedDescription(gt.rules))
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
			
			ActivitiesMinSimultaneousInSelectedTimeSlots_item item;
			item.activitiesList=cn->_activitiesIndices;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.activitiesSet=QSet<int>(item.activitiesList.constBegin(), item.activitiesList.constEnd());
#else
			item.activitiesSet=item.activitiesList.toSet();
#endif
			item.minSimultaneous=cn->minSimultaneous;
			for(int t=0; t < cn->selectedDays.count(); t++)
				item.selectedTimeSlotsList.append(cn->selectedDays.at(t)+cn->selectedHours.at(t)*gt.rules.nDaysPerWeek);
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.selectedTimeSlotsSet=QSet<int>(item.selectedTimeSlotsList.constBegin(), item.selectedTimeSlotsList.constEnd());
#else
			item.selectedTimeSlotsSet=item.selectedTimeSlotsList.toSet();
#endif
			item.allowEmptySlots=cn->allowEmptySlots;
			
			aminsistsList.push_back(item);
			//ActivitiesMinSimultaneousInSelectedTimeSlots_item* p_item=&aminsistsList[aminsistsList.count()-1];
			ActivitiesMinSimultaneousInSelectedTimeSlots_item* p_item=&aminsistsList.back();
			for(int ai : qAsConst(cn->_activitiesIndices))
				aminsistsListForActivity[ai].append(p_item);
		}
	}
	
	return ok;
}

//2020-05-02
bool computeActivitiesMaxTotalFromSetInSelectedTimeSlots(QWidget* parent)
{
	bool ok=true;

	amtfsistsList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		amtfsistsListForActivity[i].clear();

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MAX_TOTAL_ACTIVITIES_FROM_SET_IN_SELECTED_TIME_SLOTS){
			ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots* cn=(ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'max total activities from set in selected time slots'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			ActivitiesMaxTotalFromSetInSelectedTimeSlots_item item;
			item.activitiesList=cn->_activitiesIndices;
/*#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.activitiesSet=QSet<int>(item.activitiesList.begin(), item.activitiesList.end());
#else
			item.activitiesSet=item.activitiesList.toSet();
#endif*/
			item.maxActivities=cn->maxActivities;
			for(int t=0; t < cn->selectedDays.count(); t++)
//				item.selectedTimeSlotsList.append(cn->selectedDays.at(t)+cn->selectedHours.at(t)*gt.rules.nDaysPerWeek);
				item.selectedTimeSlotsSet.insert(cn->selectedDays.at(t)+cn->selectedHours.at(t)*gt.rules.nDaysPerWeek);
/*#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.selectedTimeSlotsSet=QSet<int>(item.selectedTimeSlotsList.begin(), item.selectedTimeSlotsList.end());
#else
			item.selectedTimeSlotsSet=item.selectedTimeSlotsList.toSet();
#endif*/
//			item.allowEmptySlots=cn->allowEmptySlots;

			amtfsistsList.push_back(item);
			//ActivitiesMinSimultaneousInSelectedTimeSlots_item* p_item=&aminsistsList[aminsistsList.count()-1];
			ActivitiesMaxTotalFromSetInSelectedTimeSlots_item * p_item=&amtfsistsList.back();
			for(int ai : qAsConst(cn->_activitiesIndices))
				amtfsistsListForActivity[ai].append(p_item);
		}
	}

	return ok;
}

//for terms
//2020-01-14
bool computeActivitiesMaxInATerm(QWidget* parent)
{
	bool ok=true;

	amiatList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		amiatListForActivity[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_MAX_IN_A_TERM){

			ConstraintActivitiesMaxInATerm* cn=(ConstraintActivitiesMaxInATerm*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'activities max in a term'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			ActivitiesMaxInATerm_item item;
			item.activitiesList=cn->_activitiesIndices;
			item.maxActivitiesInATerm=cn->maxActivitiesInATerm;

			amiatList.push_back(item);
			ActivitiesMaxInATerm_item* p_item=&amiatList.back();
			for(int ai : qAsConst(cn->_activitiesIndices)){
				amiatListForActivity[ai].append(p_item);
			}
		}
	}

	return ok;
}

//for terms
//20220-05-19
bool computeActivitiesMinInATerm(QWidget* parent)
{
	bool ok=true;

	aminiatList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		aminiatListForActivity[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_MIN_IN_A_TERM){

			ConstraintActivitiesMinInATerm* cn=(ConstraintActivitiesMinInATerm*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'activities min in a term'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			if(cn->_activitiesIndices.count()>0 && cn->_activitiesIndices.count()<cn->minActivitiesInATerm){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have a constraint of type 'activities min in a term'"
				 " which is impossible to satisfy. The number of activities in the constraint is greater than 0 and less than the"
				 " minimum number of activities allowed in a term. The constraint is:\n%1\nPlease correct and try again.")
				 .arg(cn->getDetailedDescription(gt.rules))
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			if(!cn->allowEmptyTerms && cn->_activitiesIndices.count()<cn->minActivitiesInATerm*gt.rules.nTerms){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have a constraint of type 'activities min in a term'"
				 " which is impossible to satisfy. The number of activities in the constraint is less than the minimum number of"
				 " activities allowed in a term x the number of terms (the constraint does not allow empty terms)."
				 " The constraint is:\n%1\nPlease correct and try again.")
				 .arg(cn->getDetailedDescription(gt.rules))
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			ActivitiesMinInATerm_item item;
			item.activitiesList=cn->_activitiesIndices;
			item.minActivitiesInATerm=cn->minActivitiesInATerm;
			item.allowEmptyTerms=cn->allowEmptyTerms;

			aminiatList.push_back(item);
			ActivitiesMinInATerm_item* p_item=&aminiatList.back();
			for(int ai : qAsConst(cn->_activitiesIndices)){
				aminiatListForActivity[ai].append(p_item);
			}
		}
	}

	return ok;
}

//2020-01-14
bool computeActivitiesOccupyMaxTerms(QWidget* parent)
{
	bool ok=true;

	aomtList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		aomtListForActivity[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TERMS){

			ConstraintActivitiesOccupyMaxTerms* cn=(ConstraintActivitiesOccupyMaxTerms*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'activities occupy max terms'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			ActivitiesOccupyMaxTerms_item item;
			item.activitiesList=cn->_activitiesIndices;
			item.maxOccupiedTerms=cn->maxOccupiedTerms;

			aomtList.push_back(item);
			ActivitiesOccupyMaxTerms_item* p_item=&aomtList.back();
			for(int ai : qAsConst(cn->_activitiesIndices)){
				aomtListForActivity[ai].append(p_item);
			}
		}
	}

	return ok;
}

//2019-06-08
bool computeStudentsMinGapsBetweenOrderedPairOfActivityTags(QWidget* parent)
{
	bool ok=true;
	
	smgbopoatList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		smgbopoatListForActivity[i].clear();

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS){
			ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags* cn=(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'students set min gaps between ordered pair of activity tags'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			if(cn->canonicalSubgroupsList.isEmpty())
				continue;
			
			StudentsMinGapsBetweenOrderedPairOfActivityTags_item item;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.canonicalSetOfSubgroups=QSet<int>(cn->canonicalSubgroupsList.constBegin(), cn->canonicalSubgroupsList.constEnd());
#else
			item.canonicalSetOfSubgroups=cn->canonicalSubgroupsList.toSet();
#endif
			item.minGaps=cn->minGaps;
			item.firstActivityTag=cn->_firstActivityTagIndex;
			item.secondActivityTag=cn->_secondActivityTagIndex;
			
			smgbopoatList.push_back(item);
			//StudentsMinGapsBetweenOrderedPairOfActivityTags_item* p_item=&smgbopoatList[smgbopoatList.count()-1];
			StudentsMinGapsBetweenOrderedPairOfActivityTags_item* p_item=&smgbopoatList.back();
			
			for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
				Activity* act=&gt.rules.internalActivitiesList[ai];

#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
				QSet<int> studentsSet(act->iSubgroupsList.constBegin(), act->iSubgroupsList.constEnd());
#else
				QSet<int> studentsSet=act->iSubgroupsList.toSet();
#endif
				studentsSet.intersect(item.canonicalSetOfSubgroups);
				if(studentsSet.isEmpty())
					continue;
			
				bool first, second;
			
				if(act->iActivityTagsSet.contains(cn->_firstActivityTagIndex))
					first=true;
				else
					first=false;

				if(act->iActivityTagsSet.contains(cn->_secondActivityTagIndex))
					second=true;
				else
					second=false;
					
				if(first && second){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because the activity with id %1 has both the first and the second activity tags"
					 " of the constraint of type 'students set min %2 gaps between ordered pair of activity tags %3 and %4'. Please"
					 " correct and try again.").arg(act->id).arg(cn->minGaps).arg(cn->firstActivityTag).arg(cn->secondActivityTag)
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
					
					if(t==0)
						return false;
				}
				else if(first || second){
					smgbopoatListForActivity[ai].append(p_item);
				}
				else{
					//do nothing
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS){
			ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags* cn=(ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'students min gaps between ordered pair of activity tags'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			if(cn->canonicalSubgroupsList.isEmpty())
				continue;

			StudentsMinGapsBetweenOrderedPairOfActivityTags_item item;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.canonicalSetOfSubgroups=QSet<int>(cn->canonicalSubgroupsList.constBegin(), cn->canonicalSubgroupsList.constEnd());
#else
			item.canonicalSetOfSubgroups=cn->canonicalSubgroupsList.toSet();
#endif
			item.minGaps=cn->minGaps;
			item.firstActivityTag=cn->_firstActivityTagIndex;
			item.secondActivityTag=cn->_secondActivityTagIndex;
			
			smgbopoatList.push_back(item);
			//StudentsMinGapsBetweenOrderedPairOfActivityTags_item* p_item=&smgbopoatList[smgbopoatList.count()-1];
			StudentsMinGapsBetweenOrderedPairOfActivityTags_item* p_item=&smgbopoatList.back();
			
			for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
				Activity* act=&gt.rules.internalActivitiesList[ai];

#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
				QSet<int> studentsSet(act->iSubgroupsList.constBegin(), act->iSubgroupsList.constEnd());
#else
				QSet<int> studentsSet=act->iSubgroupsList.toSet();
#endif
				studentsSet.intersect(item.canonicalSetOfSubgroups);
				if(studentsSet.isEmpty())
					continue;

				bool first, second;
			
				if(act->iActivityTagsSet.contains(cn->_firstActivityTagIndex))
					first=true;
				else
					first=false;

				if(act->iActivityTagsSet.contains(cn->_secondActivityTagIndex))
					second=true;
				else
					second=false;
					
				if(first && second){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because the activity with id %1 has both the first and the second activity tags"
					 " of the constraint of type 'students min %2 gaps between ordered pair of activity tags %3 and %4'. Please"
					 " correct and try again.").arg(act->id).arg(cn->minGaps).arg(cn->firstActivityTag).arg(cn->secondActivityTag)
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
					
					if(t==0)
						return false;
				}
				else if(first || second){
					smgbopoatListForActivity[ai].append(p_item);
				}
				else{
					//do nothing
				}
			}
		}
	}
	
	return ok;
}

//2021-12-15
bool computeStudentsMinGapsBetweenActivityTag(QWidget* parent)
{
	bool ok=true;
	
	smgbatList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		smgbatListForActivity[i].clear();

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG){
			ConstraintStudentsSetMinGapsBetweenActivityTag* cn=(ConstraintStudentsSetMinGapsBetweenActivityTag*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'students set min gaps between activity tag'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			if(cn->canonicalSubgroupsList.isEmpty())
				continue;
			
			StudentsMinGapsBetweenActivityTag_item item;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.canonicalSetOfSubgroups=QSet<int>(cn->canonicalSubgroupsList.constBegin(), cn->canonicalSubgroupsList.constEnd());
#else
			item.canonicalSetOfSubgroups=cn->canonicalSubgroupsList.toSet();
#endif
			item.minGaps=cn->minGaps;
			item.activityTag=cn->_activityTagIndex;
			
			smgbatList.push_back(item);
			StudentsMinGapsBetweenActivityTag_item* p_item=&smgbatList.back();
			
			for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
				Activity* act=&gt.rules.internalActivitiesList[ai];

#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
				QSet<int> studentsSet(act->iSubgroupsList.constBegin(), act->iSubgroupsList.constEnd());
#else
				QSet<int> studentsSet=act->iSubgroupsList.toSet();
#endif
				studentsSet.intersect(item.canonicalSetOfSubgroups);
				if(studentsSet.isEmpty())
					continue;
			
				bool hasTag;
			
				if(act->iActivityTagsSet.contains(cn->_activityTagIndex))
					hasTag=true;
				else
					hasTag=false;

				if(hasTag){
					smgbatListForActivity[ai].append(p_item);
				}
				else{
					//do nothing
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG){
			ConstraintStudentsMinGapsBetweenActivityTag* cn=(ConstraintStudentsMinGapsBetweenActivityTag*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'students min gaps between activity tag'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			if(cn->canonicalSubgroupsList.isEmpty())
				continue;

			StudentsMinGapsBetweenActivityTag_item item;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.canonicalSetOfSubgroups=QSet<int>(cn->canonicalSubgroupsList.constBegin(), cn->canonicalSubgroupsList.constEnd());
#else
			item.canonicalSetOfSubgroups=cn->canonicalSubgroupsList.toSet();
#endif
			item.minGaps=cn->minGaps;
			item.activityTag=cn->_activityTagIndex;
			
			smgbatList.push_back(item);
			StudentsMinGapsBetweenActivityTag_item* p_item=&smgbatList.back();
			
			for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
				Activity* act=&gt.rules.internalActivitiesList[ai];

#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
				QSet<int> studentsSet(act->iSubgroupsList.constBegin(), act->iSubgroupsList.constEnd());
#else
				QSet<int> studentsSet=act->iSubgroupsList.toSet();
#endif
				studentsSet.intersect(item.canonicalSetOfSubgroups);
				if(studentsSet.isEmpty())
					continue;

				bool hasTag;
			
				if(act->iActivityTagsSet.contains(cn->_activityTagIndex))
					hasTag=true;
				else
					hasTag=false;

				if(hasTag){
					smgbatListForActivity[ai].append(p_item);
				}
				else{
					//do nothing
				}
			}
		}
	}
	
	return ok;
}

//2019-06-08
bool computeTeachersMinGapsBetweenOrderedPairOfActivityTags(QWidget* parent)
{
	bool ok=true;
	
	tmgbopoatList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		tmgbopoatListForActivity[i].clear();

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS){
			ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags* cn=(ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'teacher min gaps between ordered pair of activity tags'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}

			if(cn->canonicalTeachersList.isEmpty())
				continue;
			assert(cn->canonicalTeachersList.count()==1);
			
			TeachersMinGapsBetweenOrderedPairOfActivityTags_item item;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.canonicalSetOfTeachers=QSet<int>(cn->canonicalTeachersList.constBegin(), cn->canonicalTeachersList.constEnd());
#else
			item.canonicalSetOfTeachers=cn->canonicalTeachersList.toSet();
#endif
			item.minGaps=cn->minGaps;
			item.firstActivityTag=cn->_firstActivityTagIndex;
			item.secondActivityTag=cn->_secondActivityTagIndex;
			
			tmgbopoatList.push_back(item);
			//TeachersMinGapsBetweenOrderedPairOfActivityTags_item* p_item=&tmgbopoatList[tmgbopoatList.count()-1];
			TeachersMinGapsBetweenOrderedPairOfActivityTags_item* p_item=&tmgbopoatList.back();
			
			for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
				Activity* act=&gt.rules.internalActivitiesList[ai];

				if(!act->iTeachersList.contains(cn->canonicalTeachersList.at(0)))
					continue;
			
				bool first, second;
			
				if(act->iActivityTagsSet.contains(cn->_firstActivityTagIndex))
					first=true;
				else
					first=false;

				if(act->iActivityTagsSet.contains(cn->_secondActivityTagIndex))
					second=true;
				else
					second=false;
					
				if(first && second){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because the activity with id %1 has both the first and the second activity tags"
					 " of the constraint of type 'teacher min %2 gaps between ordered pair of activity tags %3 and %4'. Please"
					 " correct and try again.").arg(act->id).arg(cn->minGaps).arg(cn->firstActivityTag).arg(cn->secondActivityTag)
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
					
					if(t==0)
						return false;
				}
				else if(first || second){
					tmgbopoatListForActivity[ai].append(p_item);
				}
				else{
					//do nothing
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS){
			ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags* cn=(ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'teachers min gaps between ordered pair of activity tags'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			if(cn->canonicalTeachersList.isEmpty())
				continue;

			TeachersMinGapsBetweenOrderedPairOfActivityTags_item item;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.canonicalSetOfTeachers=QSet<int>(cn->canonicalTeachersList.constBegin(), cn->canonicalTeachersList.constEnd());
#else
			item.canonicalSetOfTeachers=cn->canonicalTeachersList.toSet();
#endif
			item.minGaps=cn->minGaps;
			item.firstActivityTag=cn->_firstActivityTagIndex;
			item.secondActivityTag=cn->_secondActivityTagIndex;
			
			tmgbopoatList.push_back(item);
			//TeachersMinGapsBetweenOrderedPairOfActivityTags_item* p_item=&tmgbopoatList[tmgbopoatList.count()-1];
			TeachersMinGapsBetweenOrderedPairOfActivityTags_item* p_item=&tmgbopoatList.back();
			
			for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
				Activity* act=&gt.rules.internalActivitiesList[ai];

#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
				QSet<int> teachersSet(act->iTeachersList.constBegin(), act->iTeachersList.constEnd());
#else
				QSet<int> teachersSet=act->iTeachersList.toSet();
#endif
				teachersSet.intersect(item.canonicalSetOfTeachers);
				if(teachersSet.isEmpty())
					continue;

				bool first, second;
			
				if(act->iActivityTagsSet.contains(cn->_firstActivityTagIndex))
					first=true;
				else
					first=false;

				if(act->iActivityTagsSet.contains(cn->_secondActivityTagIndex))
					second=true;
				else
					second=false;
					
				if(first && second){
					ok=false;

					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot optimize, because the activity with id %1 has both the first and the second activity tags"
					 " of the constraint of type 'teachers min %2 gaps between ordered pair of activity tags %3 and %4'. Please"
					 " correct and try again.").arg(act->id).arg(cn->minGaps).arg(cn->firstActivityTag).arg(cn->secondActivityTag)
					 ,
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
					
					if(t==0)
						return false;
				}
				else if(first || second){
					tmgbopoatListForActivity[ai].append(p_item);
				}
				else{
					//do nothing
				}
			}
		}
	}
	
	return ok;
}

//2021-12-15
bool computeTeachersMinGapsBetweenActivityTag(QWidget* parent)
{
	bool ok=true;
	
	tmgbatList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		tmgbatListForActivity[i].clear();

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG){
			ConstraintTeacherMinGapsBetweenActivityTag* cn=(ConstraintTeacherMinGapsBetweenActivityTag*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'teacher min gaps between activity tag'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}

			if(cn->canonicalTeachersList.isEmpty())
				continue;
			assert(cn->canonicalTeachersList.count()==1);
			
			TeachersMinGapsBetweenActivityTag_item item;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.canonicalSetOfTeachers=QSet<int>(cn->canonicalTeachersList.constBegin(), cn->canonicalTeachersList.constEnd());
#else
			item.canonicalSetOfTeachers=cn->canonicalTeachersList.toSet();
#endif
			item.minGaps=cn->minGaps;
			item.activityTag=cn->_activityTagIndex;
			
			tmgbatList.push_back(item);
			TeachersMinGapsBetweenActivityTag_item* p_item=&tmgbatList.back();
			
			for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
				Activity* act=&gt.rules.internalActivitiesList[ai];

				if(!act->iTeachersList.contains(cn->canonicalTeachersList.at(0)))
					continue;
			
				bool hasTag;
			
				if(act->iActivityTagsSet.contains(cn->_activityTagIndex))
					hasTag=true;
				else
					hasTag=false;

				if(hasTag){
					tmgbatListForActivity[ai].append(p_item);
				}
				else{
					//do nothing
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG){
			ConstraintTeachersMinGapsBetweenActivityTag* cn=(ConstraintTeachersMinGapsBetweenActivityTag*)gt.rules.internalTimeConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'teachers min gaps between activity tag'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			if(cn->canonicalTeachersList.isEmpty())
				continue;

			TeachersMinGapsBetweenActivityTag_item item;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.canonicalSetOfTeachers=QSet<int>(cn->canonicalTeachersList.constBegin(), cn->canonicalTeachersList.constEnd());
#else
			item.canonicalSetOfTeachers=cn->canonicalTeachersList.toSet();
#endif
			item.minGaps=cn->minGaps;
			item.activityTag=cn->_activityTagIndex;
			
			tmgbatList.push_back(item);
			TeachersMinGapsBetweenActivityTag_item* p_item=&tmgbatList.back();
			
			for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
				Activity* act=&gt.rules.internalActivitiesList[ai];

#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
				QSet<int> teachersSet(act->iTeachersList.constBegin(), act->iTeachersList.constEnd());
#else
				QSet<int> teachersSet=act->iTeachersList.toSet();
#endif
				teachersSet.intersect(item.canonicalSetOfTeachers);
				if(teachersSet.isEmpty())
					continue;

				bool hasTag;
			
				if(act->iActivityTagsSet.contains(cn->_activityTagIndex))
					hasTag=true;
				else
					hasTag=false;

				if(hasTag){
					tmgbatListForActivity[ai].append(p_item);
				}
				else{
					//do nothing
				}
			}
		}
	}
	
	return ok;
}

//2012-04-29
bool computeActivitiesOccupyMaxDifferentRooms(QWidget* parent)
{
	bool ok=true;
	
	aomdrList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		aomdrListForActivity[i].clear();

	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS){
			ConstraintActivitiesOccupyMaxDifferentRooms* cn=(ConstraintActivitiesOccupyMaxDifferentRooms*)gt.rules.internalSpaceConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'activities occupy max different rooms'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			ActivitiesOccupyMaxDifferentRooms_item item;
			item.activitiesList=cn->_activitiesIndices;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.activitiesSet=QSet<int>(item.activitiesList.constBegin(), item.activitiesList.constEnd());
#else
			item.activitiesSet=item.activitiesList.toSet();
#endif
			item.maxDifferentRooms=cn->maxDifferentRooms;
			
			aomdrList.push_back(item);
			//ActivitiesOccupyMaxDifferentRooms_item* p_item=&aomdrList[aomdrList.count()-1];
			ActivitiesOccupyMaxDifferentRooms_item* p_item=&aomdrList.back();
			for(int ai : qAsConst(cn->_activitiesIndices))
				aomdrListForActivity[ai].append(p_item);
		}
	}
	
	return ok;
}

//2013-09-14
bool computeActivitiesSameRoomIfConsecutive(QWidget* parent)
{
	bool ok=true;
	
	asricList.clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		asricListForActivity[i].clear();

	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE){
			ConstraintActivitiesSameRoomIfConsecutive* cn=(ConstraintActivitiesSameRoomIfConsecutive*)gt.rules.internalSpaceConstraintsList[i];

			if(cn->weightPercentage!=100.0){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because you have constraint(s) of type 'activities same room if consecutive'"
				 " with weight (percentage) below 100.0%. Please make the weight 100.0% and try again")
				 ,
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			
			ActivitiesSameRoomIfConsecutive_item item;
			item.activitiesList=cn->_activitiesIndices;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			item.activitiesSet=QSet<int>(item.activitiesList.constBegin(), item.activitiesList.constEnd());
#else
			item.activitiesSet=item.activitiesList.toSet();
#endif
			
			asricList.push_back(item);
			//ActivitiesSameRoomIfConsecutive_item* p_item=&asricList[asricList.count()-1];
			ActivitiesSameRoomIfConsecutive_item* p_item=&asricList.back();
			for(int ai : qAsConst(cn->_activitiesIndices))
				asricListForActivity[ai].append(p_item);
		}
	}
	
	return ok;
}

bool computeBasicSpace(QWidget* parent)
{
	double m=-1;
	bool ok=false;
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++)
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_BASIC_COMPULSORY_SPACE){
			ok=true;
			if(gt.rules.internalSpaceConstraintsList[i]->weightPercentage>m)
				m=gt.rules.internalSpaceConstraintsList[i]->weightPercentage;
		}
		
	if(m<100)
		ok=false;
		
	if(!ok || m<100){
		GeneratePreIrreconcilableMessage::mediumInformation(parent, GeneratePreTranslate::tr("FET warning"),
		 GeneratePreTranslate::tr("Cannot generate, because you do not have a constraint of type basic compulsory space or its weight is lower than 100.0%.")
		 +" "+
		 GeneratePreTranslate::tr("Please add a constraint of this type with weight 100%.")
		 +" "+
		 GeneratePreTranslate::tr("You can add this constraint from the menu Data -> Space constraints -> Miscellaneous -> Basic compulsory space constraints.")
		 +"\n\n"+
		 GeneratePreTranslate::tr("Explanation:")
		 +" "+
		 GeneratePreTranslate::tr("Each time you create a new file, it contains an automatically added constraint of this type.")
		 +" "+
		 GeneratePreTranslate::tr("For complete flexibility, you are allowed to remove it (even if this is a wrong idea).")
		 +" "+
		 GeneratePreTranslate::tr("Maybe you removed it by mistake from your file.")
		 +" "+
		 GeneratePreTranslate::tr("By adding it again, everything should be all right.")
		 );
		return false;
	}
	return ok;
}

bool computeNotAllowedRoomTimePercentages()
{
	notAllowedRoomTimePercentages.resize(gt.rules.nInternalRooms, gt.rules.nHoursPerWeek);

	for(int i=0; i<gt.rules.nInternalRooms; i++)
		for(int j=0; j<gt.rules.nHoursPerWeek; j++)
			notAllowedRoomTimePercentages[i][j]=-1;

	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES){
			ConstraintRoomNotAvailableTimes* rn=(ConstraintRoomNotAvailableTimes*)gt.rules.internalSpaceConstraintsList[i];
			
			assert(rn->days.count()==rn->hours.count());
			for(int kk=0; kk<rn->days.count(); kk++){
				int d=rn->days.at(kk);
				int h=rn->hours.at(kk);

				if(notAllowedRoomTimePercentages[rn->room_ID][d+h*gt.rules.nDaysPerWeek]<rn->weightPercentage)
					notAllowedRoomTimePercentages[rn->room_ID][d+h*gt.rules.nDaysPerWeek]=rn->weightPercentage;
			}
		}
	}
	
	return true;
}

bool computeNotAllowedTeacherRoomTimePercentages()
{
	haveTeacherRoomNotAllowedTimesConstraints=false;
	//QHash<QPair<qint64, qint64>, double > notAllowedTeacherRoomTimePercentages;
	//first qint64 is (teacher, room), second qint64 is (day, hour)
	notAllowedTeacherRoomTimePercentages.clear();

	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES){
			haveTeacherRoomNotAllowedTimesConstraints=true;
			ConstraintTeacherRoomNotAvailableTimes* rn=(ConstraintTeacherRoomNotAvailableTimes*)gt.rules.internalSpaceConstraintsList[i];

			assert(rn->days.count()==rn->hours.count());
			for(int kk=0; kk<rn->days.count(); kk++){
				int d=rn->days.at(kk);
				int h=rn->hours.at(kk);

				int tch=rn->teacher_ID;
				int rm=rn->room_ID;

				double perc=notAllowedTeacherRoomTimePercentages.value(QPair<qint64, qint64>(teacherRoomQInt64Combination(tch, rm), dayHourQInt64Combination(d, h)), -1);
				if(perc<rn->weightPercentage)
					notAllowedTeacherRoomTimePercentages.insert(QPair<qint64, qint64>(teacherRoomQInt64Combination(tch, rm), dayHourQInt64Combination(d, h)), rn->weightPercentage);
			}
		}
	}
	
	return true;
}

bool computeActivitiesRoomsPreferences(QWidget* parent)
{
	constraintsForActivity.resize(gt.rules.nInternalActivities);

	preferredRealRooms.resize(gt.rules.nInternalActivities);

	tmpPreferredRealRooms.resize(gt.rules.nInternalActivities);
	tmpFoundNonEmpty.resize(gt.rules.nInternalActivities);

	//to disallow duplicates
	QSet<QString> studentsSetHomeRoom;
	QSet<QString> teachersHomeRoom;
	/*QSet<QString> subjectsPreferredRoom;
	QSet<QPair<QString, QString>> subjectsActivityTagsPreferredRoom;*/
	//QSet<int> activitiesPreferredRoom;

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		constraintsForActivity[i].clear();
	
		unspecifiedPreferredRoom[i]=true;
		activitiesPreferredRoomsList[i].clear();
		//activitiesPreferredRoomsPercentage[i]=-1;

		unspecifiedHomeRoom[i]=true;
		activitiesHomeRoomsHomeRooms[i].clear();
		activitiesHomeRoomsPercentage[i]=-1;
		
		preferredRealRooms[i].clear();
	}
	
	bool ok=true;
	
	visitedActivityResultantRealRooms.resize(gt.rules.nInternalActivities);
	for(int a=0; a<gt.rules.nInternalActivities; a++)
		visitedActivityResultantRealRooms[a]=false;

	for(int a=0; a<gt.rules.nInternalActivities; a++){
		tmpPreferredRealRooms[a].clear();
		tmpFoundNonEmpty[a]=false;
	}
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
			ConstraintActivityPreferredRoom* apr=(ConstraintActivityPreferredRoom*)gt.rules.internalSpaceConstraintsList[i];
			
			int a=apr->_activity;

			if(apr->weightPercentage==100.0){
				if(!apr->preferredRealRooms.isEmpty()){
					if(tmpFoundNonEmpty[a]){
						tmpPreferredRealRooms[a].intersect(apr->preferredRealRooms);
					}
					else{
						tmpPreferredRealRooms[a]=apr->preferredRealRooms;
						tmpFoundNonEmpty[a]=true;
					}
				}
			}
		}
	}

	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_HOME_ROOM){
			ConstraintStudentsSetHomeRoom* spr=(ConstraintStudentsSetHomeRoom*)gt.rules.internalSpaceConstraintsList[i];
			
			if(studentsSetHomeRoom.contains(spr->studentsName)){
				ok=false;
				
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "students set home room(s) for students set %1. Please leave only one of them")
				 .arg(spr->studentsName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			studentsSetHomeRoom.insert(spr->studentsName);
		
			for(int a : qAsConst(spr->_activities)){
				if(unspecifiedHomeRoom[a]){
					unspecifiedHomeRoom[a]=false;
					activitiesHomeRoomsPercentage[a]=spr->weightPercentage;
					assert(activitiesHomeRoomsHomeRooms[a].count()==0);
					activitiesHomeRoomsHomeRooms[a].append(spr->_room);
				}
				else{
					int t=activitiesHomeRoomsHomeRooms[a].indexOf(spr->_room);
					activitiesHomeRoomsHomeRooms[a].clear();
					activitiesHomeRoomsPercentage[a]=max(activitiesHomeRoomsPercentage[a], spr->weightPercentage);
					if(t!=-1){
						activitiesHomeRoomsHomeRooms[a].append(spr->_room);
					}
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_HOME_ROOMS){
			ConstraintStudentsSetHomeRooms* spr=(ConstraintStudentsSetHomeRooms*)gt.rules.internalSpaceConstraintsList[i];
		
			if(studentsSetHomeRoom.contains(spr->studentsName)){
				ok=false;
				
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "students set home room(s) for students set %1. Please leave only one of them")
				 .arg(spr->studentsName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			studentsSetHomeRoom.insert(spr->studentsName);
		
			for(int a : qAsConst(spr->_activities)){
				if(unspecifiedHomeRoom[a]){
					unspecifiedHomeRoom[a]=false;
					activitiesHomeRoomsPercentage[a]=spr->weightPercentage;
					assert(activitiesHomeRoomsHomeRooms[a].count()==0);
					for(int rm : qAsConst(spr->_rooms)){
						activitiesHomeRoomsHomeRooms[a].append(rm);
					}
				}
				else{
					QList<int> shared;
					for(int rm : qAsConst(spr->_rooms)){
						if(activitiesHomeRoomsHomeRooms[a].indexOf(rm)!=-1)
							shared.append(rm);
					}
					activitiesHomeRoomsPercentage[a]=max(activitiesHomeRoomsPercentage[a], spr->weightPercentage);
					activitiesHomeRoomsHomeRooms[a]=shared;
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHER_HOME_ROOM){
			ConstraintTeacherHomeRoom* spr=(ConstraintTeacherHomeRoom*)gt.rules.internalSpaceConstraintsList[i];
			
			if(teachersHomeRoom.contains(spr->teacherName)){
				ok=false;
				
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "teacher home room(s) for teacher %1. Please leave only one of them")
				 .arg(spr->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			teachersHomeRoom.insert(spr->teacherName);
		
			for(int a : qAsConst(spr->_activities)){
				if(unspecifiedHomeRoom[a]){
					unspecifiedHomeRoom[a]=false;
					activitiesHomeRoomsPercentage[a]=spr->weightPercentage;
					assert(activitiesHomeRoomsHomeRooms[a].count()==0);
					activitiesHomeRoomsHomeRooms[a].append(spr->_room);
				}
				else{
					int t=activitiesHomeRoomsHomeRooms[a].indexOf(spr->_room);
					activitiesHomeRoomsHomeRooms[a].clear();
					activitiesHomeRoomsPercentage[a]=max(activitiesHomeRoomsPercentage[a], spr->weightPercentage);
					if(t!=-1){
						activitiesHomeRoomsHomeRooms[a].append(spr->_room);
					}
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHER_HOME_ROOMS){
			ConstraintTeacherHomeRooms* spr=(ConstraintTeacherHomeRooms*)gt.rules.internalSpaceConstraintsList[i];
		
			if(teachersHomeRoom.contains(spr->teacherName)){
				ok=false;
				
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "teacher home room(s) for teacher %1. Please leave only one of them")
				 .arg(spr->teacherName),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			teachersHomeRoom.insert(spr->teacherName);
		
			for(int a : qAsConst(spr->_activities)){
				if(unspecifiedHomeRoom[a]){
					unspecifiedHomeRoom[a]=false;
					activitiesHomeRoomsPercentage[a]=spr->weightPercentage;
					assert(activitiesHomeRoomsHomeRooms[a].count()==0);
					for(int rm : qAsConst(spr->_rooms)){
						activitiesHomeRoomsHomeRooms[a].append(rm);
					}
				}
				else{
					QList<int> shared;
					for(int rm : qAsConst(spr->_rooms)){
						if(activitiesHomeRoomsHomeRooms[a].indexOf(rm)!=-1)
							shared.append(rm);
					}
					activitiesHomeRoomsPercentage[a]=max(activitiesHomeRoomsPercentage[a], spr->weightPercentage);
					activitiesHomeRoomsHomeRooms[a]=shared;
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
			ConstraintSubjectPreferredRoom* spr=(ConstraintSubjectPreferredRoom*)gt.rules.internalSpaceConstraintsList[i];
			
			/*if(subjectsPreferredRoom.contains(spr->subjectName)){
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "subject preferred room(s) for subject %1. Please leave only one of them")
				 .arg(spr->subjectName),
				 GeneratePreTranslate::tr("Skip rest of such problems"), GeneratePreTranslate::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			subjectsPreferredRoom.insert(spr->subjectName);*/
		
			for(int a : qAsConst(spr->_activities)){
				if(spr->weightPercentage==100.0){
					constraintsForActivity[a].append(gt.rules.internalSpaceConstraintsList[i]);
				}
				else{
					PreferredRoomsItem it;
				
					it.percentage=spr->weightPercentage;
					it.preferredRooms.insert(spr->_room);
				
					if(unspecifiedPreferredRoom[a]){
						unspecifiedPreferredRoom[a]=false;
						//activitiesPreferredRoomsPercentage[a]=spr->weightPercentage;
						//assert(activitiesPreferredRoomsPreferredRooms[a].count()==0);
						//activitiesPreferredRoomsPreferredRooms[a].append(spr->_room);
					}
					else{
						//int t=activitiesPreferredRoomsPreferredRooms[a].indexOf(spr->_room);
						//activitiesPreferredRoomsPreferredRooms[a].clear();
						//activitiesPreferredRoomsPercentage[a]=max(activitiesPreferredRoomsPercentage[a], spr->weightPercentage);
						//if(t!=-1){
						//	activitiesPreferredRoomsPreferredRooms[a].append(spr->_room);
						//}
					}
				
					activitiesPreferredRoomsList[a].append(it);
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
			ConstraintSubjectPreferredRooms* spr=(ConstraintSubjectPreferredRooms*)gt.rules.internalSpaceConstraintsList[i];
			
			/*if(subjectsPreferredRoom.contains(spr->subjectName)){
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "subject preferred room(s) for subject %1. Please leave only one of them")
				 .arg(spr->subjectName),
				 GeneratePreTranslate::tr("Skip rest of such problems"), GeneratePreTranslate::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			subjectsPreferredRoom.insert(spr->subjectName);*/
		
			for(int a : qAsConst(spr->_activities)){
				if(spr->weightPercentage==100.0){
					constraintsForActivity[a].append(gt.rules.internalSpaceConstraintsList[i]);
				}
				else{
					PreferredRoomsItem it;
				
					it.percentage=spr->weightPercentage;
					for(int k : qAsConst(spr->_rooms))
						it.preferredRooms.insert(k);
				
					if(unspecifiedPreferredRoom[a])
						unspecifiedPreferredRoom[a]=false;
				
					activitiesPreferredRoomsList[a].append(it);
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM){
			ConstraintSubjectActivityTagPreferredRoom* spr=(ConstraintSubjectActivityTagPreferredRoom*)gt.rules.internalSpaceConstraintsList[i];
			
			/*QPair<QString, QString> pair=qMakePair(spr->subjectName, spr->activityTagName);
			if(subjectsActivityTagsPreferredRoom.contains(pair)){
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "subject activity tag preferred room(s) for subject %1, activity tag %2. Please leave only one of them")
				 .arg(spr->subjectName)
				 .arg(spr->activityTagName),
				 GeneratePreTranslate::tr("Skip rest of such problems"), GeneratePreTranslate::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			subjectsActivityTagsPreferredRoom.insert(pair);*/
		
			//for(int j=0; j<spr->_nActivities; j++){
			//	int a=spr->_activities[j];
			for(int a : qAsConst(spr->_activities)){
				if(spr->weightPercentage==100.0){
					constraintsForActivity[a].append(gt.rules.internalSpaceConstraintsList[i]);
				}
				else{
					PreferredRoomsItem it;
					
					it.percentage=spr->weightPercentage;
					it.preferredRooms.insert(spr->_room);
				
					if(unspecifiedPreferredRoom[a])
						unspecifiedPreferredRoom[a]=false;
					
					activitiesPreferredRoomsList[a].append(it);
				}

				/*if(unspecifiedPreferredRoom[a]){
					unspecifiedPreferredRoom[a]=false;
					activitiesPreferredRoomsPercentage[a]=spr->weightPercentage;
					assert(activitiesPreferredRoomsPreferredRooms[a].count()==0);
					activitiesPreferredRoomsPreferredRooms[a].append(spr->_room);
				}
				else{
					int t=activitiesPreferredRoomsPreferredRooms[a].indexOf(spr->_room);
					activitiesPreferredRoomsPreferredRooms[a].clear();
					activitiesPreferredRoomsPercentage[a]=max(activitiesPreferredRoomsPercentage[a], spr->weightPercentage);
					if(t!=-1){
						activitiesPreferredRoomsPreferredRooms[a].append(spr->_room);
					}
				}*/
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS){
			ConstraintSubjectActivityTagPreferredRooms* spr=(ConstraintSubjectActivityTagPreferredRooms*)gt.rules.internalSpaceConstraintsList[i];
			
			/*QPair<QString, QString> pair=qMakePair(spr->subjectName, spr->activityTagName);
			if(subjectsActivityTagsPreferredRoom.contains(pair)){
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "subject activity tag preferred room(s) for subject %1, activity tag %2. Please leave only one of them")
				 .arg(spr->subjectName)
				 .arg(spr->activityTagName),
				 GeneratePreTranslate::tr("Skip rest of such problems"), GeneratePreTranslate::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			subjectsActivityTagsPreferredRoom.insert(pair);*/
		
			for(int a : qAsConst(spr->_activities)){
				if(spr->weightPercentage==100.0){
					constraintsForActivity[a].append(gt.rules.internalSpaceConstraintsList[i]);
				}
				else{
					PreferredRoomsItem it;
					
					it.percentage=spr->weightPercentage;
					for(int k : qAsConst(spr->_rooms))
						it.preferredRooms.insert(k);
				
					if(unspecifiedPreferredRoom[a])
						unspecifiedPreferredRoom[a]=false;
					
					activitiesPreferredRoomsList[a].append(it);
				}
			}
		}

		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM){
			ConstraintActivityTagPreferredRoom* spr=(ConstraintActivityTagPreferredRoom*)gt.rules.internalSpaceConstraintsList[i];

			for(int a : qAsConst(spr->_activities)){
				if(spr->weightPercentage==100.0){
					constraintsForActivity[a].append(gt.rules.internalSpaceConstraintsList[i]);
				}
				else{
					PreferredRoomsItem it;
					
					it.percentage=spr->weightPercentage;
					it.preferredRooms.insert(spr->_room);
			
					if(unspecifiedPreferredRoom[a])
						unspecifiedPreferredRoom[a]=false;
					
					activitiesPreferredRoomsList[a].append(it);
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS){
			ConstraintActivityTagPreferredRooms* spr=(ConstraintActivityTagPreferredRooms*)gt.rules.internalSpaceConstraintsList[i];

			for(int a : qAsConst(spr->_activities)){
				if(spr->weightPercentage==100.0){
					constraintsForActivity[a].append(gt.rules.internalSpaceConstraintsList[i]);
				}
				else{
					PreferredRoomsItem it;
				
					it.percentage=spr->weightPercentage;
					for(int k : qAsConst(spr->_rooms))
						it.preferredRooms.insert(k);
			
					if(unspecifiedPreferredRoom[a])
						unspecifiedPreferredRoom[a]=false;
				
					activitiesPreferredRoomsList[a].append(it);
				}
			}
		}

		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
			ConstraintActivityPreferredRoom* apr=(ConstraintActivityPreferredRoom*)gt.rules.internalSpaceConstraintsList[i];
			
			/*if(activitiesPreferredRoom.contains(apr->activityId)){
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "activity preferred room(s) for activity with id %1. Please leave only one of them")
				 .arg(apr->activityId),
				 GeneratePreTranslate::tr("Skip rest of such problems"), GeneratePreTranslate::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			activitiesPreferredRoom.insert(apr->activityId);*/
		
			int a=apr->_activity;

			if(!apr->preferredRealRooms.isEmpty()){
				if(gt.rules.internalRoomsList[apr->_room]->isVirtual==false){
					ok=false;
					
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot generate the timetable, because you have a constraint activity preferred room for the activity"
					 " with id=%1 which specifies a list of real rooms, but the preferred room is not virtual. Please correct.")
					 .arg(gt.rules.internalActivitiesList[a].id),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
		
					if(t==0)
						break;
				}
				
				int t2=-1;
				for(int rr : qAsConst(apr->preferredRealRooms))
					if(gt.rules.internalRoomsList[rr]->isVirtual==true){
						ok=false;
					
						t2=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot generate the timetable, because you have a constraint activity preferred room for the activity"
						 " with id=%1 which specifies a list of real rooms, but the room %2 from this list is virtual. Please correct.")
						 .arg(gt.rules.internalActivitiesList[a].id).arg(gt.rules.internalRoomsList[rr]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
		
						if(t2==0)
							break;
					}
				if(t2==0){
					assert(!ok);
					break;
				}
			}
				
			if(apr->weightPercentage==100.0){
				constraintsForActivity[a].append(gt.rules.internalSpaceConstraintsList[i]);
				
				if(!apr->preferredRealRooms.isEmpty()){
					if(apr->preferredRealRooms.count()!=gt.rules.internalRoomsList[apr->_room]->rrsl.count()){
						ok=false;
					
						int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
						 GeneratePreTranslate::tr("Cannot generate the timetable, because you have a preferred room constraint for the activity"
						 " with id=%1 which specifies a real rooms list which does not have the same"
						 " number of elements as the number of sets of real rooms for the preferred virtual room (%2)."
						 " Please correct this.").arg(gt.rules.internalActivitiesList[a].id).arg(gt.rules.internalRoomsList[apr->_room]->name),
						 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
						 1, 0 );
			
						if(t==0)
							break;
					}

					assert(tmpFoundNonEmpty[a]==true);
					if(tmpPreferredRealRooms[a].count()!=gt.rules.internalRoomsList[apr->_room]->rrsl.count()){
						if(visitedActivityResultantRealRooms[a]){
							assert(ok==false);
						}
						else{
							visitedActivityResultantRealRooms[a]=true;
						
							ok=false;
					
							int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
							 GeneratePreTranslate::tr("Cannot generate the timetable, because you have more preferred room constraints for the activity"
							 " with id=%1 which specify one or more lists of real rooms whose resultant real rooms list does not have the same"
							 " number of elements as the number of sets of real rooms for the preferred virtual room (%2)."
							 " Please correct this.").arg(gt.rules.internalActivitiesList[a].id).arg(gt.rules.internalRoomsList[apr->_room]->name),
							 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
							 1, 0 );
				
							if(t==0)
								break;
						}
					}
					
					preferredRealRooms[a]=tmpPreferredRealRooms[a];
				}
			}
			else{
				if(!apr->preferredRealRooms.isEmpty()){
					ok=false;
					
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot generate the timetable, because you have a constraint activity preferred room for the activity"
					 " with id=%1 which specifies a nonempty list of real rooms and has weight <100.0%. It is necessary that the weight is exactly 100.0% or the list"
					 " of real rooms to be empty in this case. Please correct.").arg(gt.rules.internalActivitiesList[a].id),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
		
					if(t==0)
						break;
				}
			
				PreferredRoomsItem it;
					
				it.percentage=apr->weightPercentage;
				it.preferredRooms.insert(apr->_room);
				
				if(unspecifiedPreferredRoom[a])
					unspecifiedPreferredRoom[a]=false;
			
				activitiesPreferredRoomsList[a].append(it);
			}
			/*if(unspecifiedPreferredRoom[a]){
				unspecifiedPreferredRoom[a]=false;
				activitiesPreferredRoomsPercentage[a]=apr->weightPercentage;
				assert(activitiesPreferredRoomsPreferredRooms[a].count()==0);
				activitiesPreferredRoomsPreferredRooms[a].append(apr->_room);
			}
			else{
				int t=activitiesPreferredRoomsPreferredRooms[a].indexOf(apr->_room);
				activitiesPreferredRoomsPreferredRooms[a].clear();
				activitiesPreferredRoomsPercentage[a]=max(activitiesPreferredRoomsPercentage[a], apr->weightPercentage);
				if(t!=-1){
					activitiesPreferredRoomsPreferredRooms[a].append(apr->_room);
				}
			}*/
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOMS){
			ConstraintActivityPreferredRooms* apr=(ConstraintActivityPreferredRooms*)gt.rules.internalSpaceConstraintsList[i];
			
			/*if(activitiesPreferredRoom.contains(apr->activityId)){
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because you have more than one constraint of type "
				 "activity preferred room(s) for activity with id %1. Please leave only one of them")
				 .arg(apr->activityId),
				 GeneratePreTranslate::tr("Skip rest of such problems"), GeneratePreTranslate::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
			activitiesPreferredRoom.insert(apr->activityId);*/
		
			int a=apr->_activity;

			if(apr->weightPercentage==100.0){
				constraintsForActivity[a].append(gt.rules.internalSpaceConstraintsList[i]);
			}
			else{
				PreferredRoomsItem it;
					
				it.percentage=apr->weightPercentage;
				for(int k : qAsConst(apr->_rooms))
					it.preferredRooms.insert(k);
				
				if(unspecifiedPreferredRoom[a])
					unspecifiedPreferredRoom[a]=false;
					
				activitiesPreferredRoomsList[a].append(it);
			}
		}
	}
	
	for(int a=0; a<gt.rules.nInternalActivities; a++){
		QList<SpaceConstraint*> scl=constraintsForActivity[a];
		
		if(scl.count()==0){
			//nothing
		}
		else{
			if(unspecifiedPreferredRoom[a])
				unspecifiedPreferredRoom[a]=false;

			PreferredRoomsItem it;
			it.percentage=100.0;
			it.preferredRooms.clear();
	
			bool begin=true;
			for(SpaceConstraint* ctr : qAsConst(scl)){
				if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
					ConstraintSubjectPreferredRoom* spr=(ConstraintSubjectPreferredRoom*)ctr;
					
					if(begin){
						it.preferredRooms.insert(spr->_room);
						begin=false;
					}
					else{
						QSet<int> set;
						set.insert(spr->_room);
						it.preferredRooms.intersect(set);
					}
				}
				else if(ctr->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
					ConstraintSubjectPreferredRooms* spr=(ConstraintSubjectPreferredRooms*)ctr;
					
					if(begin){
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
						it.preferredRooms=QSet<int>(spr->_rooms.constBegin(), spr->_rooms.constEnd());
#else
						it.preferredRooms=spr->_rooms.toSet();
#endif
						begin=false;
					}
					else{
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
						it.preferredRooms.intersect(QSet<int>(spr->_rooms.constBegin(), spr->_rooms.constEnd()));
#else
						it.preferredRooms.intersect(spr->_rooms.toSet());
#endif
					}
				}
				else if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM){
					ConstraintSubjectActivityTagPreferredRoom* spr=(ConstraintSubjectActivityTagPreferredRoom*)ctr;
					
					if(begin){
						it.preferredRooms.insert(spr->_room);
						begin=false;
					}
					else{
						QSet<int> set;
						set.insert(spr->_room);
						it.preferredRooms.intersect(set);
					}
				}
				else if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS){
					ConstraintSubjectActivityTagPreferredRooms* spr=(ConstraintSubjectActivityTagPreferredRooms*)ctr;
					
					if(begin){
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
						it.preferredRooms=QSet<int>(spr->_rooms.constBegin(), spr->_rooms.constEnd());
#else
						it.preferredRooms=spr->_rooms.toSet();
#endif
						begin=false;
					}
					else{
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
						it.preferredRooms.intersect(QSet<int>(spr->_rooms.constBegin(), spr->_rooms.constEnd()));
#else
						it.preferredRooms.intersect(spr->_rooms.toSet());
#endif
					}
				}
				else if(ctr->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM){
					ConstraintActivityTagPreferredRoom* spr=(ConstraintActivityTagPreferredRoom*)ctr;
					
					if(begin){
						it.preferredRooms.insert(spr->_room);
						begin=false;
					}
					else{
						QSet<int> set;
						set.insert(spr->_room);
						it.preferredRooms.intersect(set);
					}
				}
				else if(ctr->type==CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS){
					ConstraintActivityTagPreferredRooms* spr=(ConstraintActivityTagPreferredRooms*)ctr;
					
					if(begin){
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
						it.preferredRooms=QSet<int>(spr->_rooms.constBegin(), spr->_rooms.constEnd());
#else
						it.preferredRooms=spr->_rooms.toSet();
#endif
						begin=false;
					}
					else{
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
						it.preferredRooms.intersect(QSet<int>(spr->_rooms.constBegin(), spr->_rooms.constEnd()));
#else
						it.preferredRooms.intersect(spr->_rooms.toSet());
#endif
					}
				}
				else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
					ConstraintActivityPreferredRoom* spr=(ConstraintActivityPreferredRoom*)ctr;
					
					if(begin){
						it.preferredRooms.insert(spr->_room);
						begin=false;
					}
					else{
						QSet<int> set;
						set.insert(spr->_room);
						it.preferredRooms.intersect(set);
					}
				}
				else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOMS){
					ConstraintActivityPreferredRooms* spr=(ConstraintActivityPreferredRooms*)ctr;
					
					if(begin){
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
						it.preferredRooms=QSet<int>(spr->_rooms.constBegin(), spr->_rooms.constEnd());
#else
						it.preferredRooms=spr->_rooms.toSet();
#endif
						begin=false;
					}
					else{
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
						it.preferredRooms.intersect(QSet<int>(spr->_rooms.constBegin(), spr->_rooms.constEnd()));
#else
						it.preferredRooms.intersect(spr->_rooms.toSet());
#endif
					}
				}
			}
			
			activitiesPreferredRoomsList[a].append(it);
		}
	}
	
	/*for(int i=0; i<gt.rules.nInternalActivities; i++)
		if(!unspecifiedPreferredRoom[i])
			if(activitiesPreferredRoomsPreferredRooms[i].count()==0){
				ok=false;
				
				int t=QMessageBox::warning(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because for activity with id=%1 "
				 "you have no allowed room (from constraints activity preferred room(s) and subject (activity tag) preferred room(s) )")
				 .arg(gt.rules.internalActivitiesList[i].id),
				 GeneratePreTranslate::tr("Skip rest of activities without rooms"), GeneratePreTranslate::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}*/

	for(int i=0; i<gt.rules.nInternalActivities; i++)
		if(!unspecifiedHomeRoom[i])
			if(activitiesHomeRoomsHomeRooms[i].count()==0){
				ok=false;
				
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because for activity with id=%1 "
				 "you have no allowed home room (from constraints students set home room(s) and teacher home room(s))")
				 .arg(gt.rules.internalActivitiesList[i].id),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(!unspecifiedPreferredRoom[i]){
			for(int kk=0; kk<activitiesPreferredRoomsList[i].count(); kk++){
				PreferredRoomsItem& it=activitiesPreferredRoomsList[i][kk];
		
				bool okinitial=true;
				if(it.preferredRooms.count()==0){
					okinitial=false;

					ok=false;
					
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot generate timetable, because for activity with id=%1 "
					 "you have no allowed preferred room (from preferred room(s) constraints).")
					 .arg(gt.rules.internalActivitiesList[i].id),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
	
					if(t==0)
						goto jumpOverPrefRoomsNStudents;

					//assert(0);
				}
				QSet<int> tmp=it.preferredRooms;
				for(int r : qAsConst(tmp)){
					if(gt.rules.internalRoomsList[r]->capacity < gt.rules.internalActivitiesList[i].nTotalStudents){
						it.preferredRooms.remove(r);
					}
				}
				if(okinitial && it.preferredRooms.count()==0){
					ok=false;
					
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot generate timetable, because for activity with id=%1 "
					 "you have no allowed preferred room (from the allowed number of students and preferred room(s) constraints)")
					 .arg(gt.rules.internalActivitiesList[i].id),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
	
					if(t==0)
						goto jumpOverPrefRoomsNStudents;
				}		
			}
		}
	}
jumpOverPrefRoomsNStudents:
	
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(!unspecifiedHomeRoom[i]){
			bool okinitial=true;
			if(activitiesHomeRoomsHomeRooms[i].count()==0)
				okinitial=false;
/*			QList<int> tmp=activitiesHomeRoomsHomeRooms[i];
			for(int r : qAsConst(tmp)){
				if(gt.rules.internalRoomsList[r]->capacity < gt.rules.internalActivitiesList[i].nTotalStudents){
					int t=activitiesHomeRoomsHomeRooms[i].removeAll(r);
					assert(t==1);
				}
			}*/
			//Better:
			QList<int> tmp;
			for(int r : qAsConst(activitiesHomeRoomsHomeRooms[i]))
				if(gt.rules.internalRoomsList[r]->capacity >= gt.rules.internalActivitiesList[i].nTotalStudents)
					tmp.append(r);
			activitiesHomeRoomsHomeRooms[i]=tmp;
			
			if(okinitial && activitiesHomeRoomsHomeRooms[i].count()==0){
				ok=false;
				
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot generate timetable, because for activity with id=%1 "
				 "you have no allowed home room (from the allowed number of students)")
				 .arg(gt.rules.internalActivitiesList[i].id),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
		}
	}
	
	
	//////////new test, 9 Sept. 2009
	if(ok){
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			if(!unspecifiedPreferredRoom[i]){
				bool begin=true;
				QSet<int> allowedRooms;
				for(int kk=0; kk<activitiesPreferredRoomsList[i].count(); kk++){
					PreferredRoomsItem& it=activitiesPreferredRoomsList[i][kk];
					if(it.percentage==100.0){
						if(begin){
							allowedRooms=it.preferredRooms;
							begin=false;
						}
						else
							allowedRooms.intersect(it.preferredRooms);
					}
				}
				if(!begin && allowedRooms.count()==0){
					ok=false;
				
					int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 GeneratePreTranslate::tr("Cannot generate the timetable, because for activity with id=%1 "
					 "you have no allowed preferred room (considering rooms' capacities and constraints preferred"
					 " room(s) with 100.0% weight percentage)")
					 .arg(gt.rules.internalActivitiesList[i].id),
					 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
		
					if(t==0)
						break;
				}
			}
		}
	}
	//////////
	
	return ok;
}

bool computeMaxBuildingChangesPerDayForStudents(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		maxBuildingChangesPerDayForStudentsPercentages[i]=-1;
		maxBuildingChangesPerDayForStudentsMaxChanges[i]=-1;
	}
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY){
			ConstraintStudentsSetMaxBuildingChangesPerDay* spr=(ConstraintStudentsSetMaxBuildingChangesPerDay*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students set max building changes per day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int sbg : qAsConst(spr->iSubgroupsList)){
				maxBuildingChangesPerDayForStudentsPercentages[sbg]=100;
				if(maxBuildingChangesPerDayForStudentsMaxChanges[sbg]<0)
					maxBuildingChangesPerDayForStudentsMaxChanges[sbg]=spr->maxBuildingChangesPerDay;
				else
					maxBuildingChangesPerDayForStudentsMaxChanges[sbg]=min(maxBuildingChangesPerDayForStudentsMaxChanges[sbg], spr->maxBuildingChangesPerDay);
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY){
			ConstraintStudentsMaxBuildingChangesPerDay* spr=(ConstraintStudentsMaxBuildingChangesPerDay*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students max building changes per day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				maxBuildingChangesPerDayForStudentsPercentages[sbg]=100;
				if(maxBuildingChangesPerDayForStudentsMaxChanges[sbg]<0)
					maxBuildingChangesPerDayForStudentsMaxChanges[sbg]=spr->maxBuildingChangesPerDay;
				else
					maxBuildingChangesPerDayForStudentsMaxChanges[sbg]=min(maxBuildingChangesPerDayForStudentsMaxChanges[sbg], spr->maxBuildingChangesPerDay);
			}
		}
	}

	return ok;
}

bool computeMaxBuildingChangesPerWeekForStudents(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		maxBuildingChangesPerWeekForStudentsPercentages[i]=-1;
		maxBuildingChangesPerWeekForStudentsMaxChanges[i]=-1;
	}
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK){
			ConstraintStudentsSetMaxBuildingChangesPerWeek* spr=(ConstraintStudentsSetMaxBuildingChangesPerWeek*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students set max building changes per week"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int sbg : qAsConst(spr->iSubgroupsList)){
				maxBuildingChangesPerWeekForStudentsPercentages[sbg]=100;
				if(maxBuildingChangesPerWeekForStudentsMaxChanges[sbg]<0)
					maxBuildingChangesPerWeekForStudentsMaxChanges[sbg]=spr->maxBuildingChangesPerWeek;
				else
					maxBuildingChangesPerWeekForStudentsMaxChanges[sbg]=min(maxBuildingChangesPerWeekForStudentsMaxChanges[sbg], spr->maxBuildingChangesPerWeek);
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK){
			ConstraintStudentsMaxBuildingChangesPerWeek* spr=(ConstraintStudentsMaxBuildingChangesPerWeek*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students max building changes per week"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				maxBuildingChangesPerWeekForStudentsPercentages[sbg]=100;
				if(maxBuildingChangesPerWeekForStudentsMaxChanges[sbg]<0)
					maxBuildingChangesPerWeekForStudentsMaxChanges[sbg]=spr->maxBuildingChangesPerWeek;
				else
					maxBuildingChangesPerWeekForStudentsMaxChanges[sbg]=min(maxBuildingChangesPerWeekForStudentsMaxChanges[sbg], spr->maxBuildingChangesPerWeek);
			}
		}
	}
	
	return ok;
}

bool computeMinGapsBetweenBuildingChangesForStudents(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		minGapsBetweenBuildingChangesForStudentsPercentages[i]=-1;
		minGapsBetweenBuildingChangesForStudentsMinGaps[i]=-1;
	}

	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
			ConstraintStudentsSetMinGapsBetweenBuildingChanges* spr=(ConstraintStudentsSetMinGapsBetweenBuildingChanges*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students set min gaps between building changes"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int sbg : qAsConst(spr->iSubgroupsList)){
				minGapsBetweenBuildingChangesForStudentsPercentages[sbg]=100;
				if(minGapsBetweenBuildingChangesForStudentsMinGaps[sbg]<0)
					minGapsBetweenBuildingChangesForStudentsMinGaps[sbg]=spr->minGapsBetweenBuildingChanges;
				else
					minGapsBetweenBuildingChangesForStudentsMinGaps[sbg]=max(minGapsBetweenBuildingChangesForStudentsMinGaps[sbg], spr->minGapsBetweenBuildingChanges);
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
			ConstraintStudentsMinGapsBetweenBuildingChanges* spr=(ConstraintStudentsMinGapsBetweenBuildingChanges*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students min gaps between building changes"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				minGapsBetweenBuildingChangesForStudentsPercentages[sbg]=100;
				if(minGapsBetweenBuildingChangesForStudentsMinGaps[sbg]<0)
					minGapsBetweenBuildingChangesForStudentsMinGaps[sbg]=spr->minGapsBetweenBuildingChanges;
				else
					minGapsBetweenBuildingChangesForStudentsMinGaps[sbg]=max(minGapsBetweenBuildingChangesForStudentsMinGaps[sbg], spr->minGapsBetweenBuildingChanges);
			}
		}
	}
	
	return ok;
}

bool computeMaxBuildingChangesPerDayForTeachers(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		maxBuildingChangesPerDayForTeachersPercentages[i]=-1;
		maxBuildingChangesPerDayForTeachersMaxChanges[i]=-1;
	}
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY){
			ConstraintTeacherMaxBuildingChangesPerDay* spr=(ConstraintTeacherMaxBuildingChangesPerDay*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teacher max building changes per day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			maxBuildingChangesPerDayForTeachersPercentages[spr->teacher_ID]=100;
			if(maxBuildingChangesPerDayForTeachersMaxChanges[spr->teacher_ID]<0)
				maxBuildingChangesPerDayForTeachersMaxChanges[spr->teacher_ID]=spr->maxBuildingChangesPerDay;
			else
				maxBuildingChangesPerDayForTeachersMaxChanges[spr->teacher_ID]=min(maxBuildingChangesPerDayForTeachersMaxChanges[spr->teacher_ID], spr->maxBuildingChangesPerDay);
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY){
			ConstraintTeachersMaxBuildingChangesPerDay* spr=(ConstraintTeachersMaxBuildingChangesPerDay*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teachers max building changes per day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				maxBuildingChangesPerDayForTeachersPercentages[tch]=100;
				if(maxBuildingChangesPerDayForTeachersMaxChanges[tch]<0)
					maxBuildingChangesPerDayForTeachersMaxChanges[tch]=spr->maxBuildingChangesPerDay;
				else
					maxBuildingChangesPerDayForTeachersMaxChanges[tch]=min(maxBuildingChangesPerDayForTeachersMaxChanges[tch], spr->maxBuildingChangesPerDay);
			}
		}
	}

	return ok;
}

bool computeMaxBuildingChangesPerWeekForTeachers(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		maxBuildingChangesPerWeekForTeachersPercentages[i]=-1;
		maxBuildingChangesPerWeekForTeachersMaxChanges[i]=-1;
	}
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK){
			ConstraintTeacherMaxBuildingChangesPerWeek* spr=(ConstraintTeacherMaxBuildingChangesPerWeek*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teacher max building changes per week"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			maxBuildingChangesPerWeekForTeachersPercentages[spr->teacher_ID]=100;
			if(maxBuildingChangesPerWeekForTeachersMaxChanges[spr->teacher_ID]<0)
				maxBuildingChangesPerWeekForTeachersMaxChanges[spr->teacher_ID]=spr->maxBuildingChangesPerWeek;
			else
				maxBuildingChangesPerWeekForTeachersMaxChanges[spr->teacher_ID]=min(maxBuildingChangesPerWeekForTeachersMaxChanges[spr->teacher_ID], spr->maxBuildingChangesPerWeek);
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK){
			ConstraintTeachersMaxBuildingChangesPerWeek* spr=(ConstraintTeachersMaxBuildingChangesPerWeek*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teachers max building changes per week"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){			
				maxBuildingChangesPerWeekForTeachersPercentages[tch]=100;
				if(maxBuildingChangesPerWeekForTeachersMaxChanges[tch]<0)
					maxBuildingChangesPerWeekForTeachersMaxChanges[tch]=spr->maxBuildingChangesPerWeek;
				else
					maxBuildingChangesPerWeekForTeachersMaxChanges[tch]=min(maxBuildingChangesPerWeekForTeachersMaxChanges[tch], spr->maxBuildingChangesPerWeek);
			}
		}
	}

	return ok;
}

bool computeMinGapsBetweenBuildingChangesForTeachers(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		minGapsBetweenBuildingChangesForTeachersPercentages[i]=-1;
		minGapsBetweenBuildingChangesForTeachersMinGaps[i]=-1;
	}

	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
			ConstraintTeacherMinGapsBetweenBuildingChanges* spr=(ConstraintTeacherMinGapsBetweenBuildingChanges*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teacher min gaps between building changes"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			minGapsBetweenBuildingChangesForTeachersPercentages[spr->teacher_ID]=100;
			if(minGapsBetweenBuildingChangesForTeachersMinGaps[spr->teacher_ID]<0)
				minGapsBetweenBuildingChangesForTeachersMinGaps[spr->teacher_ID]=spr->minGapsBetweenBuildingChanges;
			else
				minGapsBetweenBuildingChangesForTeachersMinGaps[spr->teacher_ID]=max(minGapsBetweenBuildingChangesForTeachersMinGaps[spr->teacher_ID], spr->minGapsBetweenBuildingChanges);
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
			ConstraintTeachersMinGapsBetweenBuildingChanges* spr=(ConstraintTeachersMinGapsBetweenBuildingChanges*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teachers min gaps between building changes"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				minGapsBetweenBuildingChangesForTeachersPercentages[tch]=100;
				if(minGapsBetweenBuildingChangesForTeachersMinGaps[tch]<0)
					minGapsBetweenBuildingChangesForTeachersMinGaps[tch]=spr->minGapsBetweenBuildingChanges;
				else
					minGapsBetweenBuildingChangesForTeachersMinGaps[tch]=max(minGapsBetweenBuildingChangesForTeachersMinGaps[tch], spr->minGapsBetweenBuildingChanges);
			}
		}
	}
	
	return ok;
}

bool computeMaxRoomChangesPerDayForStudents(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		maxRoomChangesPerDayForStudentsPercentages[i]=-1;
		maxRoomChangesPerDayForStudentsMaxChanges[i]=-1;
	}
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY){
			ConstraintStudentsSetMaxRoomChangesPerDay* spr=(ConstraintStudentsSetMaxRoomChangesPerDay*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students set max room changes per day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int sbg : qAsConst(spr->iSubgroupsList)){
				maxRoomChangesPerDayForStudentsPercentages[sbg]=100;
				if(maxRoomChangesPerDayForStudentsMaxChanges[sbg]<0)
					maxRoomChangesPerDayForStudentsMaxChanges[sbg]=spr->maxRoomChangesPerDay;
				else
					maxRoomChangesPerDayForStudentsMaxChanges[sbg]=min(maxRoomChangesPerDayForStudentsMaxChanges[sbg], spr->maxRoomChangesPerDay);
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY){
			ConstraintStudentsMaxRoomChangesPerDay* spr=(ConstraintStudentsMaxRoomChangesPerDay*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students max room changes per day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				maxRoomChangesPerDayForStudentsPercentages[sbg]=100;
				if(maxRoomChangesPerDayForStudentsMaxChanges[sbg]<0)
					maxRoomChangesPerDayForStudentsMaxChanges[sbg]=spr->maxRoomChangesPerDay;
				else
					maxRoomChangesPerDayForStudentsMaxChanges[sbg]=min(maxRoomChangesPerDayForStudentsMaxChanges[sbg], spr->maxRoomChangesPerDay);
			}
		}
	}

	return ok;
}

bool computeMaxRoomChangesPerWeekForStudents(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		maxRoomChangesPerWeekForStudentsPercentages[i]=-1;
		maxRoomChangesPerWeekForStudentsMaxChanges[i]=-1;
	}
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK){
			ConstraintStudentsSetMaxRoomChangesPerWeek* spr=(ConstraintStudentsSetMaxRoomChangesPerWeek*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students set max room changes per week"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int sbg : qAsConst(spr->iSubgroupsList)){
				maxRoomChangesPerWeekForStudentsPercentages[sbg]=100;
				if(maxRoomChangesPerWeekForStudentsMaxChanges[sbg]<0)
					maxRoomChangesPerWeekForStudentsMaxChanges[sbg]=spr->maxRoomChangesPerWeek;
				else
					maxRoomChangesPerWeekForStudentsMaxChanges[sbg]=min(maxRoomChangesPerWeekForStudentsMaxChanges[sbg], spr->maxRoomChangesPerWeek);
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_WEEK){
			ConstraintStudentsMaxRoomChangesPerWeek* spr=(ConstraintStudentsMaxRoomChangesPerWeek*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students max room changes per week"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				maxRoomChangesPerWeekForStudentsPercentages[sbg]=100;
				if(maxRoomChangesPerWeekForStudentsMaxChanges[sbg]<0)
					maxRoomChangesPerWeekForStudentsMaxChanges[sbg]=spr->maxRoomChangesPerWeek;
				else
					maxRoomChangesPerWeekForStudentsMaxChanges[sbg]=min(maxRoomChangesPerWeekForStudentsMaxChanges[sbg], spr->maxRoomChangesPerWeek);
			}
		}
	}
	
	return ok;
}

bool computeMinGapsBetweenRoomChangesForStudents(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		minGapsBetweenRoomChangesForStudentsPercentages[i]=-1;
		minGapsBetweenRoomChangesForStudentsMinGaps[i]=-1;
	}

	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES){
			ConstraintStudentsSetMinGapsBetweenRoomChanges* spr=(ConstraintStudentsSetMinGapsBetweenRoomChanges*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students set min gaps between room changes"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int sbg : qAsConst(spr->iSubgroupsList)){
				minGapsBetweenRoomChangesForStudentsPercentages[sbg]=100;
				if(minGapsBetweenRoomChangesForStudentsMinGaps[sbg]<0)
					minGapsBetweenRoomChangesForStudentsMinGaps[sbg]=spr->minGapsBetweenRoomChanges;
				else
					minGapsBetweenRoomChangesForStudentsMinGaps[sbg]=max(minGapsBetweenRoomChangesForStudentsMinGaps[sbg], spr->minGapsBetweenRoomChanges);
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ROOM_CHANGES){
			ConstraintStudentsMinGapsBetweenRoomChanges* spr=(ConstraintStudentsMinGapsBetweenRoomChanges*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students min gaps between room changes"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				minGapsBetweenRoomChangesForStudentsPercentages[sbg]=100;
				if(minGapsBetweenRoomChangesForStudentsMinGaps[sbg]<0)
					minGapsBetweenRoomChangesForStudentsMinGaps[sbg]=spr->minGapsBetweenRoomChanges;
				else
					minGapsBetweenRoomChangesForStudentsMinGaps[sbg]=max(minGapsBetweenRoomChangesForStudentsMinGaps[sbg], spr->minGapsBetweenRoomChanges);
			}
		}
	}
	
	return ok;
}

bool computeMaxRoomChangesPerDayForTeachers(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		maxRoomChangesPerDayForTeachersPercentages[i]=-1;
		maxRoomChangesPerDayForTeachersMaxChanges[i]=-1;
	}
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY){
			ConstraintTeacherMaxRoomChangesPerDay* spr=(ConstraintTeacherMaxRoomChangesPerDay*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teacher max room changes per day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			maxRoomChangesPerDayForTeachersPercentages[spr->teacher_ID]=100;
			if(maxRoomChangesPerDayForTeachersMaxChanges[spr->teacher_ID]<0)
				maxRoomChangesPerDayForTeachersMaxChanges[spr->teacher_ID]=spr->maxRoomChangesPerDay;
			else
				maxRoomChangesPerDayForTeachersMaxChanges[spr->teacher_ID]=min(maxRoomChangesPerDayForTeachersMaxChanges[spr->teacher_ID], spr->maxRoomChangesPerDay);
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY){
			ConstraintTeachersMaxRoomChangesPerDay* spr=(ConstraintTeachersMaxRoomChangesPerDay*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teachers max room changes per day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				maxRoomChangesPerDayForTeachersPercentages[tch]=100;
				if(maxRoomChangesPerDayForTeachersMaxChanges[tch]<0)
					maxRoomChangesPerDayForTeachersMaxChanges[tch]=spr->maxRoomChangesPerDay;
				else
					maxRoomChangesPerDayForTeachersMaxChanges[tch]=min(maxRoomChangesPerDayForTeachersMaxChanges[tch], spr->maxRoomChangesPerDay);
			}
		}
	}

	return ok;
}

bool computeMaxRoomChangesPerWeekForTeachers(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		maxRoomChangesPerWeekForTeachersPercentages[i]=-1;
		maxRoomChangesPerWeekForTeachersMaxChanges[i]=-1;
	}
	
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK){
			ConstraintTeacherMaxRoomChangesPerWeek* spr=(ConstraintTeacherMaxRoomChangesPerWeek*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teacher max room changes per week"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			maxRoomChangesPerWeekForTeachersPercentages[spr->teacher_ID]=100;
			if(maxRoomChangesPerWeekForTeachersMaxChanges[spr->teacher_ID]<0)
				maxRoomChangesPerWeekForTeachersMaxChanges[spr->teacher_ID]=spr->maxRoomChangesPerWeek;
			else
				maxRoomChangesPerWeekForTeachersMaxChanges[spr->teacher_ID]=min(maxRoomChangesPerWeekForTeachersMaxChanges[spr->teacher_ID], spr->maxRoomChangesPerWeek);
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK){
			ConstraintTeachersMaxRoomChangesPerWeek* spr=(ConstraintTeachersMaxRoomChangesPerWeek*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teachers max room changes per week"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){			
				maxRoomChangesPerWeekForTeachersPercentages[tch]=100;
				if(maxRoomChangesPerWeekForTeachersMaxChanges[tch]<0)
					maxRoomChangesPerWeekForTeachersMaxChanges[tch]=spr->maxRoomChangesPerWeek;
				else
					maxRoomChangesPerWeekForTeachersMaxChanges[tch]=min(maxRoomChangesPerWeekForTeachersMaxChanges[tch], spr->maxRoomChangesPerWeek);
			}
		}
	}

	return ok;
}

bool computeMinGapsBetweenRoomChangesForTeachers(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		minGapsBetweenRoomChangesForTeachersPercentages[i]=-1;
		minGapsBetweenRoomChangesForTeachersMinGaps[i]=-1;
	}

	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES){
			ConstraintTeacherMinGapsBetweenRoomChanges* spr=(ConstraintTeacherMinGapsBetweenRoomChanges*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teacher min gaps between room changes"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			minGapsBetweenRoomChangesForTeachersPercentages[spr->teacher_ID]=100;
			if(minGapsBetweenRoomChangesForTeachersMinGaps[spr->teacher_ID]<0)
				minGapsBetweenRoomChangesForTeachersMinGaps[spr->teacher_ID]=spr->minGapsBetweenRoomChanges;
			else
				minGapsBetweenRoomChangesForTeachersMinGaps[spr->teacher_ID]=max(minGapsBetweenRoomChangesForTeachersMinGaps[spr->teacher_ID], spr->minGapsBetweenRoomChanges);
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES){
			ConstraintTeachersMinGapsBetweenRoomChanges* spr=(ConstraintTeachersMinGapsBetweenRoomChanges*)gt.rules.internalSpaceConstraintsList[i];
			
			if(spr->weightPercentage!=100){
				ok=false;
		
				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teachers min gaps between room changes"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
			
			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				minGapsBetweenRoomChangesForTeachersPercentages[tch]=100;
				if(minGapsBetweenRoomChangesForTeachersMinGaps[tch]<0)
					minGapsBetweenRoomChangesForTeachersMinGaps[tch]=spr->minGapsBetweenRoomChanges;
				else
					minGapsBetweenRoomChangesForTeachersMinGaps[tch]=max(minGapsBetweenRoomChangesForTeachersMinGaps[tch], spr->minGapsBetweenRoomChanges);
			}
		}
	}
	
	return ok;
}

bool computeMaxRoomChangesPerRealDayForTeachers(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		maxRoomChangesPerRealDayForTeachersPercentages[i]=-1;
		maxRoomChangesPerRealDayForTeachersMaxChanges[i]=-1;
	}

	bool ok=true;

	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY){
			ConstraintTeacherMaxRoomChangesPerRealDay* spr=(ConstraintTeacherMaxRoomChangesPerRealDay*)gt.rules.internalSpaceConstraintsList[i];

			if(spr->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teacher max room changes per real day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			maxRoomChangesPerRealDayForTeachersPercentages[spr->teacher_ID]=100;
			if(maxRoomChangesPerRealDayForTeachersMaxChanges[spr->teacher_ID]<0)
				maxRoomChangesPerRealDayForTeachersMaxChanges[spr->teacher_ID]=spr->maxRoomChangesPerDay;
			else
				maxRoomChangesPerRealDayForTeachersMaxChanges[spr->teacher_ID]=min(maxRoomChangesPerRealDayForTeachersMaxChanges[spr->teacher_ID], spr->maxRoomChangesPerDay);
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY){
			ConstraintTeachersMaxRoomChangesPerRealDay* spr=(ConstraintTeachersMaxRoomChangesPerRealDay*)gt.rules.internalSpaceConstraintsList[i];

			if(spr->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teachers max room changes per real day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				maxRoomChangesPerRealDayForTeachersPercentages[tch]=100;
				if(maxRoomChangesPerRealDayForTeachersMaxChanges[tch]<0)
					maxRoomChangesPerRealDayForTeachersMaxChanges[tch]=spr->maxRoomChangesPerDay;
				else
					maxRoomChangesPerRealDayForTeachersMaxChanges[tch]=min(maxRoomChangesPerRealDayForTeachersMaxChanges[tch], spr->maxRoomChangesPerDay);
			}
		}
	}

	return ok;
}

bool computeMaxRoomChangesPerRealDayForStudents(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		maxRoomChangesPerRealDayForSubgroupsPercentages[i]=-1;
		maxRoomChangesPerRealDayForSubgroupsMaxChanges[i]=-1;
	}

	bool ok=true;

	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY){
			ConstraintStudentsSetMaxRoomChangesPerRealDay* spr=(ConstraintStudentsSetMaxRoomChangesPerRealDay*)gt.rules.internalSpaceConstraintsList[i];

			if(spr->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students set max room changes per real day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int sbg : spr->iSubgroupsList){
				maxRoomChangesPerRealDayForSubgroupsPercentages[sbg]=100;
				if(maxRoomChangesPerRealDayForSubgroupsMaxChanges[sbg]<0)
					maxRoomChangesPerRealDayForSubgroupsMaxChanges[sbg]=spr->maxRoomChangesPerDay;
				else
					maxRoomChangesPerRealDayForSubgroupsMaxChanges[sbg]=min(maxRoomChangesPerRealDayForSubgroupsMaxChanges[sbg], spr->maxRoomChangesPerDay);
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY){
			ConstraintStudentsMaxRoomChangesPerRealDay* spr=(ConstraintStudentsMaxRoomChangesPerRealDay*)gt.rules.internalSpaceConstraintsList[i];

			if(spr->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students max room changes per real day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				maxRoomChangesPerRealDayForSubgroupsPercentages[sbg]=100;
				if(maxRoomChangesPerRealDayForSubgroupsMaxChanges[sbg]<0)
					maxRoomChangesPerRealDayForSubgroupsMaxChanges[sbg]=spr->maxRoomChangesPerDay;
				else
					maxRoomChangesPerRealDayForSubgroupsMaxChanges[sbg]=min(maxRoomChangesPerRealDayForSubgroupsMaxChanges[sbg], spr->maxRoomChangesPerDay);
			}
		}
	}

	return ok;
}

bool computeMaxBuildingChangesPerRealDayForTeachers(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		maxBuildingChangesPerRealDayForTeachersPercentages[i]=-1;
		maxBuildingChangesPerRealDayForTeachersMaxChanges[i]=-1;
	}

	bool ok=true;

	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY){
			ConstraintTeacherMaxBuildingChangesPerRealDay* spr=(ConstraintTeacherMaxBuildingChangesPerRealDay*)gt.rules.internalSpaceConstraintsList[i];

			if(spr->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teacher max building changes per real day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			maxBuildingChangesPerRealDayForTeachersPercentages[spr->teacher_ID]=100;
			if(maxBuildingChangesPerRealDayForTeachersMaxChanges[spr->teacher_ID]<0)
				maxBuildingChangesPerRealDayForTeachersMaxChanges[spr->teacher_ID]=spr->maxBuildingChangesPerDay;
			else
				maxBuildingChangesPerRealDayForTeachersMaxChanges[spr->teacher_ID]=min(maxBuildingChangesPerRealDayForTeachersMaxChanges[spr->teacher_ID], spr->maxBuildingChangesPerDay);
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY){
			ConstraintTeachersMaxBuildingChangesPerRealDay* spr=(ConstraintTeachersMaxBuildingChangesPerRealDay*)gt.rules.internalSpaceConstraintsList[i];

			if(spr->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint teachers max building changes per real day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				maxBuildingChangesPerRealDayForTeachersPercentages[tch]=100;
				if(maxBuildingChangesPerRealDayForTeachersMaxChanges[tch]<0)
					maxBuildingChangesPerRealDayForTeachersMaxChanges[tch]=spr->maxBuildingChangesPerDay;
				else
					maxBuildingChangesPerRealDayForTeachersMaxChanges[tch]=min(maxBuildingChangesPerRealDayForTeachersMaxChanges[tch], spr->maxBuildingChangesPerDay);
			}
		}
	}

	return ok;
}

bool computeMaxBuildingChangesPerRealDayForStudents(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		maxBuildingChangesPerRealDayForSubgroupsPercentages[i]=-1;
		maxBuildingChangesPerRealDayForSubgroupsMaxChanges[i]=-1;
	}

	bool ok=true;

	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY){
			ConstraintStudentsSetMaxBuildingChangesPerRealDay* spr=(ConstraintStudentsSetMaxBuildingChangesPerRealDay*)gt.rules.internalSpaceConstraintsList[i];

			if(spr->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students set max building changes per real day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int sbg : spr->iSubgroupsList){
				maxBuildingChangesPerRealDayForSubgroupsPercentages[sbg]=100;
				if(maxBuildingChangesPerRealDayForSubgroupsMaxChanges[sbg]<0)
					maxBuildingChangesPerRealDayForSubgroupsMaxChanges[sbg]=spr->maxBuildingChangesPerDay;
				else
					maxBuildingChangesPerRealDayForSubgroupsMaxChanges[sbg]=min(maxBuildingChangesPerRealDayForSubgroupsMaxChanges[sbg], spr->maxBuildingChangesPerDay);
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY){
			ConstraintStudentsMaxBuildingChangesPerRealDay* spr=(ConstraintStudentsMaxBuildingChangesPerRealDay*)gt.rules.internalSpaceConstraintsList[i];

			if(spr->weightPercentage!=100){
				ok=false;

				int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
				 GeneratePreTranslate::tr("Cannot optimize, because there is a space constraint students max building changes per real day"
				 " with weight under 100%. Please correct and try again"),
				 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
				 1, 0 );

				if(t==0)
					return false;
			}

			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++){
				maxBuildingChangesPerRealDayForSubgroupsPercentages[sbg]=100;
				if(maxBuildingChangesPerRealDayForSubgroupsMaxChanges[sbg]<0)
					maxBuildingChangesPerRealDayForSubgroupsMaxChanges[sbg]=spr->maxBuildingChangesPerDay;
				else
					maxBuildingChangesPerRealDayForSubgroupsMaxChanges[sbg]=min(maxBuildingChangesPerRealDayForSubgroupsMaxChanges[sbg], spr->maxBuildingChangesPerDay);
			}
		}
	}

	return ok;
}

void computeMustComputeTimetableSubgroups()
{
	for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++)
		mustComputeTimetableSubgroup[sbg]=false;

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		Activity* act=&gt.rules.internalActivitiesList[ai];
	
		mustComputeTimetableSubgroups[ai].clear();
		
		for(int sbg : qAsConst(act->iSubgroupsList))
			if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0 ||
			  subgroupsMaxGapsPerDayPercentage[sbg]>=0 ||
			  subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0 ||
			  subgroupsMaxHoursDailyPercentages1[sbg]>=0 ||
			  subgroupsMaxHoursDailyPercentages2[sbg]>=0 ||
			  subgroupsMaxHoursContinuouslyPercentages1[sbg]>=0 ||
			  subgroupsMaxHoursContinuouslyPercentages2[sbg]>=0 ||
			  subgroupsMinHoursDailyPercentages[sbg][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0 || //daily for official, afternoon for mornings-afternoons
			
			  subgroupsMaxSpanPerDayPercentages[sbg]>=0 ||
			  subgroupsMinRestingHoursCircularPercentages[sbg]>=0 ||
			  subgroupsMinRestingHoursNotCircularPercentages[sbg]>=0 ||
			
			  subgroupsIntervalMaxDaysPerWeekPercentages[sbg].count()>0 ||
			
			  maxBuildingChangesPerDayForStudentsPercentages[sbg]>=0 ||
			  maxBuildingChangesPerWeekForStudentsPercentages[sbg]>=0 ||
			  minGapsBetweenBuildingChangesForStudentsPercentages[sbg]>=0 ||
			
			  maxRoomChangesPerDayForStudentsPercentages[sbg]>=0 ||
			  maxRoomChangesPerWeekForStudentsPercentages[sbg]>=0 ||
			  minGapsBetweenRoomChangesForStudentsPercentages[sbg]>=0 ||
			
			  subgroupsActivityTagMaxHoursContinuouslyPercentage[sbg].count()>0 ||
			  subgroupsActivityTagMaxHoursDailyPercentage[sbg].count()>0 ||
			  satmhdListForSubgroup[sbg].count()>0 ||
			  //no need to consider constraints students (set) min gaps between ordered pair of activity tags
			  //or min gaps between activity tag

			  //mornings-afternoons
			  subgroupsMaxGapsPerRealDayPercentage[sbg]>=0 ||
			  subgroupsMaxGapsPerWeekForRealDaysPercentage[sbg]>=0 ||
			  subgroupsMaxHoursDailyRealDaysPercentages1[sbg]>=0 ||
			  subgroupsMaxHoursDailyRealDaysPercentages2[sbg]>=0 ||
			  subgroupsMinHoursDailyPercentages[sbg][0]>=0 || //mornings-afternoons: morning (daily or morning constraint)
			  subgroupsMinHoursPerAfternoonPercentages[sbg]>=0 ||

			  subgroupsMaxHoursPerAllAfternoonsPercentages[sbg]>=0 ||

			  subgroupsMinMorningsPerWeekPercentages[sbg]>=0 ||
			  subgroupsMinAfternoonsPerWeekPercentages[sbg]>=0 ||

			  subgroupsMaxSpanPerRealDayPercentages[sbg]>=0 ||
			  subgroupsMinRestingHoursBetweenMorningAndAfternoonPercentages[sbg]>=0 ||

			  subgroupsMorningIntervalMaxDaysPerWeekPercentages[sbg].count()>0 ||
			  subgroupsAfternoonIntervalMaxDaysPerWeekPercentages[sbg].count()>0 ||

			  maxRoomChangesPerRealDayForSubgroupsPercentages[sbg]>=0 ||
			  maxBuildingChangesPerRealDayForSubgroupsPercentages[sbg]>=0 ||

			  subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sbg].count()>0 ||

			  subgroupsMaxTwoActivityTagsPerDayFromN1N2N3Percentages[sbg]>=0 ||
			  subgroupsMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages[sbg]>=0 ||

			  subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0 ||
			  subgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0
			  ){
			
				mustComputeTimetableSubgroups[ai].append(sbg);
				mustComputeTimetableSubgroup[sbg]=true;
			}
	}
}

void computeMustComputeTimetableTeachers()
{
	for(int tch=0; tch<gt.rules.nInternalTeachers; tch++)
		mustComputeTimetableTeacher[tch]=false;

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		Activity* act=&gt.rules.internalActivitiesList[ai];
	
		mustComputeTimetableTeachers[ai].clear();
		
		for(int tch : qAsConst(act->iTeachersList))
			if(teachersMaxGapsPerWeekPercentage[tch]>=0 ||
			  teachersMaxGapsPerDayPercentage[tch]>=0 ||
			  teachersMaxHoursDailyPercentages1[tch]>=0 ||
			  teachersMaxHoursDailyPercentages2[tch]>=0 ||
			  teachersMaxHoursContinuouslyPercentages1[tch]>=0 ||
			  teachersMaxHoursContinuouslyPercentages2[tch]>=0 ||
			  teachersMinHoursDailyPercentages[tch][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0 || //daily for official, afternoon for mornings-afternoons
			
			  teachersMinDaysPerWeekPercentages[tch]>=0 ||
			
			  teachersMaxSpanPerDayPercentages[tch]>=0 ||
			  teachersMinRestingHoursCircularPercentages[tch]>=0 ||
			  teachersMinRestingHoursNotCircularPercentages[tch]>=0 ||

			  teachersIntervalMaxDaysPerWeekPercentages[tch].count()>0 ||
			
			  maxBuildingChangesPerDayForTeachersPercentages[tch]>=0 ||
			  maxBuildingChangesPerWeekForTeachersPercentages[tch]>=0 ||
			  minGapsBetweenBuildingChangesForTeachersPercentages[tch]>=0 ||
			
			  maxRoomChangesPerDayForTeachersPercentages[tch]>=0 ||
			  maxRoomChangesPerWeekForTeachersPercentages[tch]>=0 ||
			  minGapsBetweenRoomChangesForTeachersPercentages[tch]>=0 ||
			
			  teachersActivityTagMaxHoursContinuouslyPercentage[tch].count()>0 ||
			  teachersActivityTagMaxHoursDailyPercentage[tch].count()>0 ||
			  tatmhdListForTeacher[tch].count()>0 ||
			  //no need to consider constraints teacher(s) min gaps between ordered pair of activity tags
			  //or min gaps between activity tag

			  //mornings-afternoons
			  teachersMaxGapsPerRealDayPercentage[tch]>=0 ||
			  teachersMaxGapsPerWeekForRealDaysPercentage[tch]>=0 ||
			  teachersMaxHoursDailyRealDaysPercentages1[tch]>=0 ||
			  teachersMaxHoursDailyRealDaysPercentages2[tch]>=0 ||

			  teachersMinHoursDailyPercentages[tch][0]>=0 || //mornings-afternoons: morning (daily or morning constraint)
			  teachersMinHoursPerAfternoonPercentages[tch]>=0 ||
			  teachersMinHoursDailyRealDaysPercentages[tch]>=0 ||

			  teachersMaxHoursPerAllAfternoonsPercentages[tch]>=0 ||

			  teachersMinRealDaysPerWeekPercentages[tch]>=0 ||
			  teachersMinMorningsPerWeekPercentages[tch]>=0 ||
			  teachersMinAfternoonsPerWeekPercentages[tch]>=0 ||

			  teachersMaxSpanPerRealDayPercentages[tch]>=0 ||
			  teachersMinRestingHoursBetweenMorningAndAfternoonPercentages[tch]>=0 ||

			  teachersMorningIntervalMaxDaysPerWeekPercentages[tch].count()>0 ||
			  teachersAfternoonIntervalMaxDaysPerWeekPercentages[tch].count()>0 ||

			  maxRoomChangesPerRealDayForTeachersPercentages[tch]>=0 ||
			  maxBuildingChangesPerRealDayForTeachersPercentages[tch]>=0 ||

			  teachersActivityTagMaxHoursDailyRealDaysPercentage[tch].count()>0 ||

			  teachersMaxTwoActivityTagsPerDayFromN1N2N3Percentages[tch]>=0 ||
			  teachersMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages[tch]>=0 ||

			  teachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[tch]>=0 ||
			  teachersMorningsEarlyMaxBeginningsAtSecondHourPercentage[tch]>=0 ||

			  teachersMaxGapsPerMorningAndAfternoonPercentage[tch]>=0 ||

			  teacherConstrainedToZeroGapsPerAfternoon[tch]==true
			  ){
			
				mustComputeTimetableTeachers[ai].append(tch);
				mustComputeTimetableTeacher[tch]=true;
			}
	}
}

//2022-02-16
void computeSubgroupsTeachersForActivitiesOfTheDay()
{
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		//students
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
		QSet<int> st_smhd=QSet<int>(subgroupsWithMaxDaysPerWeekForActivities[ai].constBegin(), subgroupsWithMaxDaysPerWeekForActivities[ai].constEnd());
		QSet<int> st_smtd=QSet<int>(subgroupsWithMaxThreeConsecutiveDaysForActivities[ai].constBegin(), subgroupsWithMaxThreeConsecutiveDaysForActivities[ai].constEnd());
		QSet<int> st_smd=QSet<int>(subgroupsWithMaxRealDaysPerWeekForActivities[ai].constBegin(), subgroupsWithMaxRealDaysPerWeekForActivities[ai].constEnd());
		QSet<int> st_sma=QSet<int>(subgroupsWithMaxAfternoonsPerWeekForActivities[ai].constBegin(), subgroupsWithMaxAfternoonsPerWeekForActivities[ai].constEnd());
		QSet<int> st_smn1n2n3=QSet<int>(subgroupsWithN1N2N3ForActivities[ai].constBegin(), subgroupsWithN1N2N3ForActivities[ai].constEnd());
		QSet<int> st_smm=QSet<int>(subgroupsWithMaxMorningsPerWeekForActivities[ai].constBegin(), subgroupsWithMaxMorningsPerWeekForActivities[ai].constEnd());
#else
		QSet<int> st_smhd=subgroupsWithMaxDaysPerWeekForActivities[ai].toSet();
		QSet<int> st_smtd=subgroupsWithMaxThreeConsecutiveDaysForActivities[ai].toSet();
		QSet<int> st_smd=subgroupsWithMaxRealDaysPerWeekForActivities[ai].toSet();
		QSet<int> st_smn1n2n3=subgroupsWithN1N2N3ForActivities[ai].toSet();
		QSet<int> st_sma=subgroupsWithMaxAfternoonsPerWeekForActivities[ai].toSet();
		QSet<int> st_smm=subgroupsWithMaxMorningsPerWeekForActivities[ai].toSet();
#endif
		QSet<int> st_smda=st_smhd+st_smtd+st_smd+st_sma+st_smn1n2n3;
		QSet<int> st_smdm=st_smhd+st_smtd+st_smd+st_smm+st_smn1n2n3;
		
		QList<int> st_lmda;
		QList<int> st_lmdm;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
		st_lmda=QList<int>(st_smda.constBegin(), st_smda.constEnd());
		st_lmdm=QList<int>(st_smdm.constBegin(), st_smdm.constEnd());
#else
		st_lmda=st_smda.toList();
		st_lmdm=st_smdm.toList();
#endif
		std::stable_sort(st_lmda.begin(), st_lmda.end()); //not needed, the generation behavior (random seed) stays the same with any order of st_lmda - see the corresponding code in generate.cpp
		std::stable_sort(st_lmdm.begin(), st_lmdm.end()); //not needed, the generation behavior (random seed) stays the same with any order of st_lmdm - see the corresponding code in generate.cpp

		subgroupsForActivitiesOfTheDayAfternoons[ai]=st_lmda;
		subgroupsForActivitiesOfTheDayMornings[ai]=st_lmdm;
	}
	
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		//teachers
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
		QSet<int> smhd=QSet<int>(teachersWithMaxDaysPerWeekForActivities[ai].constBegin(), teachersWithMaxDaysPerWeekForActivities[ai].constEnd());
		QSet<int> smtd=QSet<int>(teachersWithMaxThreeConsecutiveDaysForActivities[ai].constBegin(), teachersWithMaxThreeConsecutiveDaysForActivities[ai].constEnd());
		QSet<int> smd=QSet<int>(teachersWithMaxRealDaysPerWeekForActivities[ai].constBegin(), teachersWithMaxRealDaysPerWeekForActivities[ai].constEnd());
		QSet<int> smn1n2n3=QSet<int>(teachersWithN1N2N3ForActivities[ai].constBegin(), teachersWithN1N2N3ForActivities[ai].constEnd());
		QSet<int> sma=QSet<int>(teachersWithMaxAfternoonsPerWeekForActivities[ai].constBegin(), teachersWithMaxAfternoonsPerWeekForActivities[ai].constEnd());
		QSet<int> smm=QSet<int>(teachersWithMaxMorningsPerWeekForActivities[ai].constBegin(), teachersWithMaxMorningsPerWeekForActivities[ai].constEnd());
#else
		QSet<int> smhd=teachersWithMaxDaysPerWeekForActivities[ai].toSet();
		QSet<int> smtd=teachersWithMaxThreeConsecutiveDaysForActivities[ai].toSet();
		QSet<int> smd=teachersWithMaxRealDaysPerWeekForActivities[ai].toSet();
		QSet<int> smn1n2n3=teachersWithN1N2N3ForActivities[ai].toSet();
		QSet<int> sma=teachersWithMaxAfternoonsPerWeekForActivities[ai].toSet();
		QSet<int> smm=teachersWithMaxMorningsPerWeekForActivities[ai].toSet();
#endif
		QSet<int> smda=smhd+smtd+smd+sma+smn1n2n3;
		QSet<int> smdm=smhd+smtd+smd+smm+smn1n2n3;
		
		QList<int> lmda;
		QList<int> lmdm;

#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
		lmda=QList<int>(smda.constBegin(), smda.constEnd());
		lmdm=QList<int>(smdm.constBegin(), smdm.constEnd());
#else
		lmda=smda.toList();
		lmdm=smdm.toList();
#endif
		std::stable_sort(lmda.begin(), lmda.end()); //not needed, the generation behavior (random seed) stays the same with any order of lmda - see the corresponding code in generate.cpp
		std::stable_sort(lmdm.begin(), lmdm.end()); //not needed, the generation behavior (random seed) stays the same with any order of lmdm - see the corresponding code in generate.cpp

		teachersForActivitiesOfTheDayAfternoons[ai]=lmda;
		teachersForActivitiesOfTheDayMornings[ai]=lmdm;
	}
}

bool computeN1N2N3(QWidget* parent)
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMaxTwoActivityTagsPerDayFromN1N2N3Percentages[i]=-1.0;
		teachersMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages[i]=-1.0;
	}
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMaxTwoActivityTagsPerDayFromN1N2N3Percentages[i]=-1.0;
		subgroupsMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages[i]=-1.0;
	}
	
	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_TWO_ACTIVITY_TAGS_PER_DAY_FROM_N1N2N3){
			ConstraintTeacherMaxTwoActivityTagsPerDayFromN1N2N3* tn=(ConstraintTeacherMaxTwoActivityTagsPerDayFromN1N2N3*)gt.rules.internalTimeConstraintsList[i];
			teachersMaxTwoActivityTagsPerDayFromN1N2N3Percentages[tn->teacher_ID]=100.0;
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_TWO_ACTIVITY_TAGS_PER_DAY_FROM_N1N2N3){
			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++)
				teachersMaxTwoActivityTagsPerDayFromN1N2N3Percentages[tch]=100.0;
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_TWO_ACTIVITY_TAGS_PER_DAY_FROM_N1N2N3){
			ConstraintStudentsSetMaxTwoActivityTagsPerDayFromN1N2N3* sn=(ConstraintStudentsSetMaxTwoActivityTagsPerDayFromN1N2N3*)gt.rules.internalTimeConstraintsList[i];
			for(int sbg : qAsConst(sn->iSubgroupsList))
				subgroupsMaxTwoActivityTagsPerDayFromN1N2N3Percentages[sbg]=100.0;
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_TWO_ACTIVITY_TAGS_PER_DAY_FROM_N1N2N3){
			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++)
				subgroupsMaxTwoActivityTagsPerDayFromN1N2N3Percentages[sbg]=100.0;
		}

		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_TWO_ACTIVITY_TAGS_PER_REAL_DAY_FROM_N1N2N3){
			ConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3* tn=(ConstraintTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3*)gt.rules.internalTimeConstraintsList[i];
			teachersMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages[tn->teacher_ID]=100.0;
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_TWO_ACTIVITY_TAGS_PER_REAL_DAY_FROM_N1N2N3){
			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++)
				teachersMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages[tch]=100.0;
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_TWO_ACTIVITY_TAGS_PER_REAL_DAY_FROM_N1N2N3){
			ConstraintStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3* sn=(ConstraintStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3*)gt.rules.internalTimeConstraintsList[i];
			for(int sbg : qAsConst(sn->iSubgroupsList))
				subgroupsMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages[sbg]=100.0;
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_TWO_ACTIVITY_TAGS_PER_REAL_DAY_FROM_N1N2N3){
			for(int sbg=0; sbg<gt.rules.nInternalSubgroups; sbg++)
				subgroupsMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages[sbg]=100.0;
		}
	}

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		teachersWithN1N2N3ForActivities[i].clear();

		Activity* act=&gt.rules.internalActivitiesList[i];
		for(int j=0; j<act->iTeachersList.count(); j++){
			int tch=act->iTeachersList.at(j);

			if(teachersMaxTwoActivityTagsPerDayFromN1N2N3Percentages[tch]>=0 || teachersMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages[tch]>=0){
				assert(teachersWithN1N2N3ForActivities[i].indexOf(tch)==-1);
				teachersWithN1N2N3ForActivities[i].append(tch);
			}
		}
	}

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		subgroupsWithN1N2N3ForActivities[i].clear();

		Activity* act=&gt.rules.internalActivitiesList[i];
		for(int j=0; j<act->iSubgroupsList.count(); j++){
			int sbg=act->iSubgroupsList.at(j);

			if(subgroupsMaxTwoActivityTagsPerDayFromN1N2N3Percentages[sbg]>=0 || subgroupsMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages[sbg]>=0){
				assert(subgroupsWithN1N2N3ForActivities[i].indexOf(sbg)==-1);
				subgroupsWithN1N2N3ForActivities[i].append(sbg);
			}
		}
	}

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		Activity* act=&gt.rules.internalActivitiesList[ai];
		int cnt=0;
		for(const QString& tag : qAsConst(act->activityTagsNames)){
			if(tag=="N1"){
				activityTagN1N2N3[ai]=0;
				cnt++;
			}
			else if(tag=="N2"){
				activityTagN1N2N3[ai]=1;
				cnt++;
			}
			else if(tag=="N3"){
				activityTagN1N2N3[ai]=2;
				cnt++;
			}
		}
		if(cnt==0){
			activityTagN1N2N3[ai]=3; //none
		}
		else if((teachersWithN1N2N3ForActivities[ai].count()>=1 || subgroupsWithN1N2N3ForActivities[ai].count()>=1) && cnt>=2){
			ok=false;
		
			QString s=GeneratePreTranslate::tr("Activity with id=%1 has more than one activity tag from N1, N2, and N3, and you have"
			 " at least a constraint of type teacher(s)/students (set) max two activity tags from N1, N2, and N3 per (real) day for the teacher(s)/students of this activity - please correct that.")
			 .arg(gt.rules.internalActivitiesList[ai].id);
			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
			
			if(t==0)
				return false;
		}
	}

	return ok;
}

bool computeFixedActivities(QWidget* parent)
{
	bool ok=true;
	
	fixedVirtualSpaceNonZeroButNotTimeActivities.clear();

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		int notAllowedSlots=0;
		for(int tim=0; tim<gt.rules.nHoursPerWeek; tim++)
			if(notAllowedTimesPercentages[ai][tim]==100)
				notAllowedSlots++;
		
		if(notAllowedSlots==gt.rules.nHoursPerWeek){
			ok=false;
		
			QString s=GeneratePreTranslate::tr("Activity with id=%1 has no allowed slot - please correct that.").arg(gt.rules.internalActivitiesList[ai].id);
			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
					
			if(t==0)
				return false;
		}
		else if(notAllowedSlots==gt.rules.nHoursPerWeek-1)
			fixedTimeActivity[ai]=true;
		else
			fixedTimeActivity[ai]=false;
			
		//space
		//2019-09-22: after computeActivitiesRoomsPreferences
		QSet<int> srrs; //single real rooms set
		for(int r=0; r<gt.rules.nInternalRooms; r++){
			Room* rm=gt.rules.internalRoomsList[r];
			
			if(rm->isVirtual==false)
				continue;
			
			bool allSetsAreSingle=true;
			for(const QList<int>& tl : qAsConst(rm->rrsl))
				if(tl.count()!=1){
					assert(tl.count()>=2);
					allSetsAreSingle=false;
					break;
				}
				
			if(allSetsAreSingle)
				srrs.insert(r);
		}
		
		fixedSpaceActivity[ai]=false;
		for(const PreferredRoomsItem& it : qAsConst(activitiesPreferredRoomsList[ai]))
			if(it.percentage==100.0 && it.preferredRooms.count()==1){
				//int rm=it.preferredRooms.toList().at(0);
				int rm=*(it.preferredRooms.constBegin());
				assert(rm>=0 && rm<gt.rules.nInternalRooms);
				
				/*
				if(gt.rules.internalRoomsList[rm]->isVirtual==false ||
				 //2019-09-22: the test below is not too good for the initial order, but otherwise the partially locked timetables become impossible
				 //(it's more like a hack).
				 (gt.rules.internalRoomsList[rm]->isVirtual==true && preferredRealRooms[ai].count()!=0) ||
				 (gt.rules.internalRoomsList[rm]->isVirtual==true && preferredRealRooms[ai].count()==0 && srrs.contains(rm))){
					fixedSpaceActivity[ai]=true;
					break;
				}*/

				//If the preferred real rooms are not fixed (preferredRealRooms[ai].count()==0) for a virtual room, we cannot
				//make fixedSpaceActivity[ai]=true, because otherwise we might get an impossible timetable for fixed timetables.
				if(gt.rules.internalRoomsList[rm]->isVirtual==false){
					fixedSpaceActivity[ai]=true;
					break;
				}
				else if(gt.rules.internalRoomsList[rm]->isVirtual==true && preferredRealRooms[ai].count()==0 && srrs.contains(rm)){
					fixedSpaceActivity[ai]=true;
					break;
				}
				else if(gt.rules.internalRoomsList[rm]->isVirtual==true && preferredRealRooms[ai].count()!=0){
					fixedSpaceActivity[ai]=true;
					
					if(fixedTimeActivity[ai]==false && !fixedVirtualSpaceNonZeroButNotTimeActivities.contains(ai))
						fixedVirtualSpaceNonZeroButNotTimeActivities.insert(ai);
						
					break;
				}
			}
		/////////////////////////////////////////////////////
	}
	
	return ok;
}

bool homeRoomsAreOk(QWidget* parent)
{
	for(int r=0; r<gt.rules.nInternalRooms; r++)
		nHoursRequiredForRoom[r]=0;
		
	for(int a=0; a<gt.rules.nInternalActivities; a++)
		if(unspecifiedPreferredRoom[a] && !unspecifiedHomeRoom[a]
		  && activitiesHomeRoomsHomeRooms[a].count()==1 && activitiesHomeRoomsPercentage[a]==100.0){
		  	int r=activitiesHomeRoomsHomeRooms[a].at(0);
		  	nHoursRequiredForRoom[r]+=gt.rules.internalActivitiesList[a].duration;
		}
		
	for(int r=0; r<gt.rules.nInternalRooms; r++){
		nHoursAvailableForRoom[r]=0;
		for(int d=0; d<gt.rules.nDaysPerWeek; d++)
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && notAllowedRoomTimePercentages[r][d+h*gt.rules.nDaysPerWeek]<100.0)
					nHoursAvailableForRoom[r]++;
	}
	
	bool ok=true;

	for(int r=0; r<gt.rules.nInternalRooms; r++)
		if(nHoursRequiredForRoom[r]>nHoursAvailableForRoom[r]){
			ok=false;

			QString s=GeneratePreTranslate::tr("Room %1 has not enough slots for home rooms constraints (requested %2, available %3) - please correct that.")
			  .arg(gt.rules.internalRoomsList[r]->name).arg(nHoursRequiredForRoom[r]).arg(nHoursAvailableForRoom[r]);
			int t=GeneratePreIrreconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"), s,
			 GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
			 1, 0 );
					
			if(t==0)
				return false;
		}
		
	return ok;
}

void sortActivities(QWidget* parent, const QHash<int, int>& reprSameStartingTime, const QHash<int, QSet<int>>& reprSameActivitiesSet, QTextStream* initialOrderStream)
{
	//I should take care of home rooms, but I don't want to change the routine below which works well

	//	const int INF  = 2000000000;
	const int INF  = 1500000000; //INF and INF2 values of variables may be increased, so they should be INF2>>INF and INF2<<MAXINT(2147.........)
	//	const int INF2 = 2000000001;
	const int INF2 = 2000000000;

	const double THRESHOLD=80.0;
	
	//rooms init
	for(int j=0; j<gt.rules.nInternalRooms; j++){
		nRoomsIncompat[j]=0;
		if(gt.rules.internalRoomsList[j]->isVirtual==false){
			for(int k=0; k<gt.rules.nHoursPerWeek; k++){
				if(notAllowedRoomTimePercentages[j][k]>=THRESHOLD){
					nRoomsIncompat[j]++;
				}
			}
		}
		else{
			for(int k=0; k<gt.rules.nHoursPerWeek; k++){
				bool available=true;
				if(notAllowedRoomTimePercentages[j][k]>=THRESHOLD){
					available=false;
				}
				if(available){
					for(const QList<int>& tl : qAsConst(gt.rules.internalRoomsList[j]->rrsl)){
						bool ok=false;
						for(int r2 : qAsConst(tl)){
							if(notAllowedRoomTimePercentages[r2][k]<THRESHOLD){
								ok=true;
								break;
							}
						}
						if(!ok){
							available=false;
							break;
						}
					}
				}
				if(!available){
					nRoomsIncompat[j]++;
				}
			}
		}
	}

	for(int j=0; j<gt.rules.nInternalRooms; j++)
		nHoursForRoom[j]=0.0;

	//only consider for each activity the constraint preferred room(s) with highest percentage (and then lowest number of rooms)
	for(int j=0; j<gt.rules.nInternalActivities; j++){
		maxPercentagePrefRooms[j].percentage=-1;
		maxPercentagePrefRooms[j].preferredRooms.clear();
	
		double maxPercentage=-1;
		double minNRooms=INF;
		for(const PreferredRoomsItem& it : qAsConst(activitiesPreferredRoomsList[j]))
			if(maxPercentage<it.percentage || (maxPercentage==it.percentage && minNRooms>it.preferredRooms.count())){
				maxPercentage=it.percentage;
				minNRooms=it.preferredRooms.count();
				maxPercentagePrefRooms[j]=it;
			}
	}

	for(int j=0; j<gt.rules.nInternalActivities; j++){
		PreferredRoomsItem it=maxPercentagePrefRooms[j];
		if(it.percentage>=THRESHOLD){
			assert(!unspecifiedPreferredRoom[j]);
			for(int rm : qAsConst(it.preferredRooms))
				nHoursForRoom[rm]+=double(gt.rules.internalActivitiesList[j].duration)/double(it.preferredRooms.count());
		}
	}
	
	//make those with same starting time have conflicts
	//for instance, if A1 simultaneous with A2 and A2 conflicts with A3, then A1 conflicts with A3
	//also, A1 will conflict with A2, but this conflict is not counted
	//idea of Volker Dirr, implementation of Liviu Lalescu
	
	QSet<int> allRepresentatives;
	for(int r : qAsConst(reprSameStartingTime)) //only values, which are representatives
		allRepresentatives.insert(r);
	
	for(int r : qAsConst(allRepresentatives)){
		assert(reprSameActivitiesSet.contains(r));
		QSet<int> s=reprSameActivitiesSet.value(r);
		
		assert(s.count()>=1);
		if(s.count()>=2){
			//Faster
			QHash<int, int> conflHash;
			
			for(int i : qAsConst(s)){
				QHash<int, int>::const_iterator it=activitiesConflictingPercentage[i].constBegin();
				while(it!=activitiesConflictingPercentage[i].constEnd()){
					int j=it.key();
					int weight=it.value();
					if(conflHash.value(j, -1)<weight)
						conflHash.insert(j, weight);
					it++;
				}
			}
			
			for(int i : qAsConst(s)){
				QHash<int, int>::const_iterator it=conflHash.constBegin();
				while(it!=conflHash.constEnd()){
					int j=it.key();
					int weight=it.value();
					if(activitiesConflictingPercentage[i].value(j, -1)<weight)
						activitiesConflictingPercentage[i].insert(j, weight);

					if(activitiesConflictingPercentage[j].value(i, -1)<weight)
						activitiesConflictingPercentage[j].insert(i, weight);
						
					it++;
				}
			}
		}
	}
	//end same starting time

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		nIncompatible[i]=0;
		
		assert(reprSameStartingTime.contains(i));
		
		//basic
		QHash<int, int>& hashConfl=activitiesConflictingPercentage[i];

		QHash<int, int>::const_iterator iter=hashConfl.constBegin();
		while(iter!=hashConfl.constEnd()){
			int j=iter.key();
			assert(reprSameStartingTime.contains(j));

			if(reprSameStartingTime.value(i)!=reprSameStartingTime.value(j)){
				if(i!=j && iter.value()>=THRESHOLD){
					nIncompatible[i]+=gt.rules.internalActivitiesList[j].duration;
				}
			}
			iter++;
		}

		//not available, break, preferred time(s)
		for(int j=0; j<gt.rules.nHoursPerWeek; j++)
			if(notAllowedTimesPercentages[i][j]>=THRESHOLD)
				nIncompatible[i]++;
		
		//min days between activities - no
		
		if(gt.rules.mode!=MORNINGS_AFTERNOONS){
			//students max days per week
			for(int s : qAsConst(gt.rules.internalActivitiesList[i].iSubgroupsList)){
				if(subgroupsMaxDaysPerWeekWeightPercentages[s]>=THRESHOLD){
					assert(gt.rules.nDaysPerWeek-subgroupsMaxDaysPerWeekMaxDays[s] >=0 );
					nIncompatible[i]+=(gt.rules.nDaysPerWeek-subgroupsMaxDaysPerWeekMaxDays[s])*gt.rules.nHoursPerDay;
				}
			}

			//teachers max days per week
			for(int t : qAsConst(gt.rules.internalActivitiesList[i].iTeachersList)){
				if(teachersMaxDaysPerWeekWeightPercentages[t]>=THRESHOLD){
					assert(gt.rules.nDaysPerWeek-teachersMaxDaysPerWeekMaxDays[t] >=0 );
					nIncompatible[i]+=(gt.rules.nDaysPerWeek-teachersMaxDaysPerWeekMaxDays[t])*gt.rules.nHoursPerDay;
				}
			}
		}
		else{
			assert(gt.rules.nDaysPerWeek%2==0);
		
			//students max real days per week
			for(int s : qAsConst(gt.rules.internalActivitiesList[i].iSubgroupsList)){
				int nIncompat=0;
				if(subgroupsMaxDaysPerWeekWeightPercentages[s]>=THRESHOLD){
					assert(gt.rules.nDaysPerWeek-subgroupsMaxDaysPerWeekMaxDays[s] >=0 );
					nIncompat=(gt.rules.nDaysPerWeek-subgroupsMaxDaysPerWeekMaxDays[s])*gt.rules.nHoursPerDay;
				}

				int nIncompatReal=0;
				if(subgroupsMaxRealDaysPerWeekWeightPercentages[s]>=THRESHOLD){
					assert(gt.rules.nDaysPerWeek/2-subgroupsMaxRealDaysPerWeekMaxDays[s] >=0 );
					nIncompatReal=(gt.rules.nDaysPerWeek/2-subgroupsMaxRealDaysPerWeekMaxDays[s])*2*gt.rules.nHoursPerDay;
				}
				
				nIncompatible[i]+=max(nIncompat, nIncompatReal);
			}

			//teachers max real days per week
			for(int t : qAsConst(gt.rules.internalActivitiesList[i].iTeachersList)){
				int nIncompat=0;
				if(teachersMaxDaysPerWeekWeightPercentages[t]>=THRESHOLD){
					assert(gt.rules.nDaysPerWeek-teachersMaxDaysPerWeekMaxDays[t] >=0 );
					nIncompat=(gt.rules.nDaysPerWeek-teachersMaxDaysPerWeekMaxDays[t])*gt.rules.nHoursPerDay;
				}
			
				int nIncompatReal=0;
				if(teachersMaxRealDaysPerWeekWeightPercentages[t]>=THRESHOLD){
					assert(gt.rules.nDaysPerWeek/2-teachersMaxRealDaysPerWeekMaxDays[t] >=0 );
					nIncompatReal=(gt.rules.nDaysPerWeek/2-teachersMaxRealDaysPerWeekMaxDays[t])*2*gt.rules.nHoursPerDay;
				}
				
				nIncompatible[i]+=max(nIncompat, nIncompatReal);
			}
		}

		
		//rooms
		PreferredRoomsItem it=maxPercentagePrefRooms[i];
		if(it.percentage>=THRESHOLD){
			double cnt=0.0;
			assert(!unspecifiedPreferredRoom[i]);
			for(int rm : qAsConst(it.preferredRooms))
				cnt+=double(nRoomsIncompat[rm])+nHoursForRoom[rm]-(double(gt.rules.internalActivitiesList[i].duration)/double(it.preferredRooms.count()));
				 //-duration because we considered also current activity
			
			if(cnt<0.0){
				//assert(cnt+0.01>=0.0); //maybe rouding error, but not too high
				if(!(cnt+0.01>=0.0)){
					cout<<"Probable ERROR in file "<<__FILE__<<", line "<<__LINE__<<", cnt should be >=0.0"<<endl;
				}
				cnt=0.0;
			}
			
			assert(cnt>=0.0);
			
			//2013-01-08 - So that generation is identical on all computers
			double t = cnt / double(it.preferredRooms.count()); //average for all the rooms
			
			t*=100000.0;
			t=floor(t+0.5);
			t/=100000.0;

			nIncompatible[i] += int(t); //average for all the rooms.
		}
		
		nIncompatible[i]*=gt.rules.internalActivitiesList[i].duration;
		
		assert(nIncompatible[i]<INF);
		
		assert(nIncompatible[i]>=0);
		
		if(fixedTimeActivity[i]){
			nIncompatible[i]=INF;
			
			if(fixedSpaceActivity[i])
				nIncompatible[i]=INF2;
			
			//this is to avoid an "impossible to generate" bug in fixed timetables - does not eliminate completely the bug, unfortunately
			nIncompatible[i]+=gt.rules.internalActivitiesList[i].iSubgroupsList.count()+
			 gt.rules.internalActivitiesList[i].iTeachersList.count();
			
			assert(nIncompatible[i]>=INF);
		}
	}

	//DEPRECATED COMMENT
	//same starting time - not computing, the algo takes care even without correct sorting
	//it is difficult to sort about same starting time
	
	//IT IS POSSIBLE TO SORT ABOUT SAME STARTING TIME, see below, idea of Volker Dirr. It is much faster for some data sets.
	//(for some data sets, it is 7 times faster)
	
	//compute repr:
	/////////////////
	QMultiHash<int, int> adjMatrix;
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME
		 &&gt.rules.internalTimeConstraintsList[i]->weightPercentage==100.0){
			ConstraintActivitiesSameStartingTime* sst=(ConstraintActivitiesSameStartingTime*)gt.rules.internalTimeConstraintsList[i];
			
			for(int i=1; i<sst->_n_activities; i++){
				adjMatrix.insert(sst->_activities[0], sst->_activities[i]);
				adjMatrix.insert(sst->_activities[i], sst->_activities[0]);
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE
		 && gt.rules.internalTimeConstraintsList[i]->weightPercentage==100.0){
			ConstraintTwoActivitiesConsecutive* c2c=(ConstraintTwoActivitiesConsecutive*)gt.rules.internalTimeConstraintsList[i];
			
			adjMatrix.insert(c2c->firstActivityIndex, c2c->secondActivityIndex);
			adjMatrix.insert(c2c->secondActivityIndex, c2c->firstActivityIndex);
		}
	}
		
	QHash<int, int> repr;
	//repr.clear();
	
	QQueue<int> queue;
	
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		int start=i;
		
		if(repr.value(start, -1)==-1){ //not visited
			repr.insert(start, start);
			queue.enqueue(start);
			while(!queue.isEmpty()){
				int crtHead=queue.dequeue();
				assert(repr.value(crtHead, -1)==start);
				QList<int> neighList=adjMatrix.values(crtHead);
				for(int neigh : qAsConst(neighList)){
					if(repr.value(neigh, -1)==-1){
						queue.enqueue(neigh);
						repr.insert(neigh, start);
					}
					else{
						assert(repr.value(neigh, -1)==start);
					}
				}
			}
		}
	}

	/////////////////
	
	//take care of chains of constraints with 100% weight
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		reprNInc[i]=-1;
		
	//repr contains the representative of each activity
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		//int MM=nIncompatible[i];
		assert(repr.contains(i));
		int r=repr.value(i);
		
		int xx=nIncompatible[i];
		if(fixedTimeActivity[i] && fixedSpaceActivity[i]){
			assert(xx>=INF2);
			xx-=(INF2-INF);
		}
		
		assert(xx<INF2);
		assert(xx>=0);
		
		if(reprNInc[r]<xx)
			reprNInc[r]=xx;
	}

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		int r=repr.value(i);
		assert(reprNInc[r]>=0);
		if(nIncompatible[i]<reprNInc[r])
			nIncompatible[i]=reprNInc[r];
	}
	
	//new Volker (start), modified by Liviu
	//takes care of 100% weight constraints (not necessary - already took care above) and also <100% and >=80% weight constraints
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		TimeConstraint* tc=gt.rules.internalTimeConstraintsList[i];
		if(tc->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME && tc->weightPercentage>=THRESHOLD){
			ConstraintActivitiesSameStartingTime* c=(ConstraintActivitiesSameStartingTime*) tc;
			
			int xx=nIncompatible[c->_activities[0]];
			if(fixedTimeActivity[c->_activities[0]] && fixedSpaceActivity[c->_activities[0]]){
				assert(xx>=INF2);
				xx-=(INF2-INF);
			}
			assert(xx>=0);
			assert(xx<INF2);

			int MM=xx;
			
			for(int a=1; a<c->_n_activities; a++){
				int yy=nIncompatible[c->_activities[a]];
				if(fixedTimeActivity[c->_activities[a]] && fixedSpaceActivity[c->_activities[a]]){
					assert(yy>=INF2);
					yy-=(INF2-INF);
				}
				assert(yy>=0);
				assert(yy<INF2);

				if(MM<yy)
					MM=yy;
			}
			
			for(int a=0; a<c->_n_activities; a++)
				if(nIncompatible[c->_activities[a]]<MM)
					nIncompatible[c->_activities[a]]=MM;
		}
		else if(tc->type==CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE && tc->weightPercentage>=THRESHOLD){
			ConstraintTwoActivitiesConsecutive* c=(ConstraintTwoActivitiesConsecutive*) tc;
			
			int xx=nIncompatible[c->firstActivityIndex];
			if(fixedTimeActivity[c->firstActivityIndex] && fixedSpaceActivity[c->firstActivityIndex]){
				assert(xx>=INF2);
				xx-=(INF2-INF);
			}
			assert(xx>=0);
			assert(xx<INF2);
			
			int MM=xx;
			
			int yy=nIncompatible[c->secondActivityIndex];
			if(fixedTimeActivity[c->secondActivityIndex] && fixedSpaceActivity[c->secondActivityIndex]){
				assert(yy>=INF2);
				yy-=(INF2-INF);
			}
			assert(yy>=0);
			assert(yy<INF2);
			
			if(MM<yy)
				MM=yy;
				
			if(nIncompatible[c->firstActivityIndex] < MM)
				nIncompatible[c->firstActivityIndex] = MM;

			if(nIncompatible[c->secondActivityIndex] < MM)
				nIncompatible[c->secondActivityIndex] = MM;
		}
		else if(tc->type==CONSTRAINT_TWO_ACTIVITIES_GROUPED && tc->weightPercentage>=THRESHOLD){
			ConstraintTwoActivitiesGrouped* c=(ConstraintTwoActivitiesGrouped*) tc;
			
			int xx=nIncompatible[c->firstActivityIndex];
			if(fixedTimeActivity[c->firstActivityIndex] && fixedSpaceActivity[c->firstActivityIndex]){
				assert(xx>=INF2);
				xx-=(INF2-INF);
			}
			assert(xx>=0);
			assert(xx<INF2);
			
			int MM=xx;
			
			int yy=nIncompatible[c->secondActivityIndex];
			if(fixedTimeActivity[c->secondActivityIndex] && fixedSpaceActivity[c->secondActivityIndex]){
				assert(yy>=INF2);
				yy-=(INF2-INF);
			}
			assert(yy>=0);
			assert(yy<INF2);
			
			if(MM<yy)
				MM=yy;
				
			if(nIncompatible[c->firstActivityIndex] < MM)
				nIncompatible[c->firstActivityIndex] = MM;

			if(nIncompatible[c->secondActivityIndex] < MM)
				nIncompatible[c->secondActivityIndex] = MM;
		}
	}
	//new Volker (end)
	
	//2014-06-30 - group activities in initial order
	if(gt.rules.groupActivitiesInInitialOrderList.count()>0){
		for(int i=0; i<gt.rules.nInternalActivities; i++)
			fatherActivityInInitialOrder[i]=-1;
		
		bool report=true;
		int j=0;
		
		for(GroupActivitiesInInitialOrderItem* item : qAsConst(gt.rules.groupActivitiesInInitialOrderList)){
			j++;
			
			if(!item->active)
				continue;
			
			if(item->indices.count()<2){
				if(report){
					QString s=GeneratePreTranslate::tr("Group activities in initial order item number %1 is ignored, because it contains less than"
					 " two active activities").arg(j);
				
					int t=GeneratePreReconcilableMessage::mediumConfirmation(parent, GeneratePreTranslate::tr("FET warning"),
					 s, GeneratePreTranslate::tr("Skip rest"), GeneratePreTranslate::tr("See next"), QString(),
					 1, 0 );
				
					if(t==0)
						report=false;
				}
			}
			else{
				int index0=item->indices.at(0);
				int xx=nIncompatible[index0];
				/*if(fixedTimeActivity[index0] && fixedSpaceActivity[index0]){
					assert(xx>=INF2);
					xx-=(INF2-INF);
				}
				assert(xx>=0);
				assert(xx<INF2);*/
				
				int MM=xx;
				int father=0;
				
				for(int i=1; i<item->indices.count(); i++){
					int indexi=item->indices.at(i);

					int yy=nIncompatible[indexi];
					/*if(fixedTimeActivity[indexi] && fixedSpaceActivity[indexi]){
						assert(yy>=INF2);
						yy-=(INF2-INF);
					}
					assert(yy>=0);
					assert(yy<INF2);*/
					
					if(MM<yy){
						MM=yy;
						father=i;
					}
				}
				
				//if(MM>=INF)
				//	MM=INF-1;

				for(int i=0; i<item->indices.count(); i++)
					if(i!=father){
						assert(fatherActivityInInitialOrder[item->indices.at(i)]==-1);
						fatherActivityInInitialOrder[item->indices.at(i)]=item->indices.at(father);
					}
					
				/*for(int i=0; i<item.indices.count(); i++){
					int ai=item.indices.at(i);
					if(nIncompatible[ai]<MM)
						nIncompatible[ai]=MM;
				}*/
			}
		}
	}
	
	//this is to avoid an "impossible to generate" bug in fixed timetables - does not eliminate completely the bug, unfortunately
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(nIncompatible[i]>=INF && nIncompatible[i]<INF2){
			if(fixedTimeActivity[i]){
				nIncompatible[i]=INF;
		
				nIncompatible[i]+=gt.rules.internalActivitiesList[i].iSubgroupsList.count()+
				 gt.rules.internalActivitiesList[i].iTeachersList.count();
			
				assert(nIncompatible[i]>=INF);
			}
			else{
				nIncompatible[i]=INF;
			}
		}
		else if(nIncompatible[i]>=INF2){
			assert(fixedTimeActivity[i] && fixedSpaceActivity[i]);
		
			int xx=nIncompatible[i]-INF2;
			assert(xx==gt.rules.internalActivitiesList[i].iSubgroupsList.count()+
			 gt.rules.internalActivitiesList[i].iTeachersList.count());
		}
	}
	
	QHash<int, int> allowedSlotForFixedActivity;
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(fixedTimeActivity[i]){
			int cnt=0;
			int allowed=-1;
			for(int s=0; s<gt.rules.nHoursPerWeek; s++){
				if(notAllowedTimesPercentages[i][s]<100.0){
					allowed=s;
					cnt++;
				}
			}
			assert(cnt==1);
			allowedSlotForFixedActivity.insert(i, allowed);
		}
	}
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		nMinDaysConstraintsBroken[i]=1.0;
		
		if(fixedTimeActivity[i]){
			assert(allowedSlotForFixedActivity.contains(i));
			int si=allowedSlotForFixedActivity.value(i);
			int di=si%gt.rules.nDaysPerWeek;
			
			for(int d=0; d<minDaysListOfActivities[i].count(); d++){
				int j=minDaysListOfActivities[i].at(d);
				if(!fixedTimeActivity[j])
					continue;
				int m=minDaysListOfMinDays[i].at(d);
				double w=minDaysListOfWeightPercentages[i].at(d)/100.0;
				
				assert(allowedSlotForFixedActivity.contains(j));
				int sj=allowedSlotForFixedActivity.value(j);
				int dj=sj%gt.rules.nDaysPerWeek;
				
				int dist;
				if(gt.rules.mode!=MORNINGS_AFTERNOONS)
					dist=abs(di-dj);
				else
					dist=abs(di/2-dj/2);
				if(dist<m){
					//careful, don't assert weight is <100.0, because the data may be impossible and we get assert failed
					nMinDaysConstraintsBroken[i]*=(1.0-w);
				}
			}
			
			for(int d=0; d<minHalfDaysListOfActivities[i].count(); d++){
				int j=minHalfDaysListOfActivities[i].at(d);
				if(!fixedTimeActivity[j])
					continue;
				int m=minHalfDaysListOfMinDays[i].at(d);
				double w=minHalfDaysListOfWeightPercentages[i].at(d)/100.0;
				
				assert(allowedSlotForFixedActivity.contains(j));
				int sj=allowedSlotForFixedActivity.value(j);
				int dj=sj%gt.rules.nDaysPerWeek;
				
				int dist;
				assert(gt.rules.mode==MORNINGS_AFTERNOONS);
				dist=abs(di-dj);
				if(dist<m){
					//careful, don't assert weight is <100.0, because the data may be impossible and we get assert failed
					nMinDaysConstraintsBroken[i]*=(1.0-w);
				}
			}
		}
		
		nMinDaysConstraintsBroken[i]=1.0-nMinDaysConstraintsBroken[i];
		//the resultant weight of all broken constraints
		//(the probability that after one try, we are not OK)
	}
	
	//DEPRECATED
	//Sort activities in increasing order of number of the other activities with which
	//this activity does not conflict
	//Selection sort, based on a permutation
	
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		copyOfInitialPermutation[i]=i;
		
	/*for(int i=0; i<gt.rules.nInternalActivities; i++){
		for(int j=i+1; j<gt.rules.nInternalActivities; j++){
			if(nIncompatible[copyOfInitialPermutation[i]]<nIncompatible[copyOfInitialPermutation[j]] ||
			 (nIncompatible[copyOfInitialPermutation[i]]==nIncompatible[copyOfInitialPermutation[j]] && nMinDaysConstraintsBroken[copyOfInitialPermutation[i]]<nMinDaysConstraintsBroken[copyOfInitialPermutation[j]])){
				int t=copyOfInitialPermutation[i];
				copyOfInitialPermutation[i]=copyOfInitialPermutation[j];
				copyOfInitialPermutation[j]=t;
			}
		}
	}*/
	
	//descending by nIncompatible, then by nMinDaysConstraintsBroken
	//(by nMinDaysConstraintsBroken to alleviate an 'impossible to generate' bug if generating for a fixed timetable).
	if(gt.rules.groupActivitiesInInitialOrderList.count()==0){
		//qStableSort(copyOfInitialPermutation+0, copyOfInitialPermutation+gt.rules.nInternalActivities, compareFunctionGeneratePre);
		//std::stable_sort(copyOfInitialPermutation+0, copyOfInitialPermutation+gt.rules.nInternalActivities, compareFunctionGeneratePre);
		std::stable_sort(&copyOfInitialPermutation[0], &copyOfInitialPermutation[gt.rules.nInternalActivities], compareFunctionGeneratePre);

		for(int q=1; q<gt.rules.nInternalActivities; q++){
			int i=copyOfInitialPermutation[q-1];
			int j=copyOfInitialPermutation[q];
			//DEPRECATED COMMENT: don't assert nMinDaysConstraintsBroken is descending (if nIncompatible is equal), because it is a double number and we may get rounding errors
			//assert(nIncompatible[copyOfInitialPermutation[i-1]]>=nIncompatible[copyOfInitialPermutation[i]]);
			assert(nIncompatible[i]>nIncompatible[j] || (nIncompatible[i]==nIncompatible[j] && nMinDaysConstraintsBroken[i]>=nMinDaysConstraintsBroken[j]));
		}
	}
	else{
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			int nInc_i;
			
			if(fatherActivityInInitialOrder[i]==-1)
				nInc_i=nIncompatible[i];
			else
				nInc_i=nIncompatible[fatherActivityInInitialOrder[i]];
			
			if(nInc_i>=INF2){
				if(fixedTimeActivity[i] && fixedSpaceActivity[i]){
					nInc_i=INF2;
			
					nInc_i+=gt.rules.internalActivitiesList[i].iSubgroupsList.count()+
					gt.rules.internalActivitiesList[i].iTeachersList.count();
				
					assert(nInc_i>=INF2);
				}
				else if(fixedTimeActivity[i]){
					nInc_i=INF;
			
					nInc_i+=gt.rules.internalActivitiesList[i].iSubgroupsList.count()+
					gt.rules.internalActivitiesList[i].iTeachersList.count();
				
					assert(nInc_i>=INF);
				}
				else{
					nInc_i=INF;
				}
			}
			else if(nInc_i>=INF && nInc_i<INF2){
				if(fixedTimeActivity[i]){
					nInc_i=INF;
			
					nInc_i+=gt.rules.internalActivitiesList[i].iSubgroupsList.count()+
					gt.rules.internalActivitiesList[i].iTeachersList.count();
				
					assert(nInc_i>=INF);
				}
				else{
					nInc_i=INF;
				}
			}
			
			nIncompatibleFromFather[i]=nInc_i;
		}

		//qStableSort(copyOfInitialPermutation+0, copyOfInitialPermutation+gt.rules.nInternalActivities, compareFunctionGeneratePreWithGroupedActivities);
		//std::stable_sort(copyOfInitialPermutation+0, copyOfInitialPermutation+gt.rules.nInternalActivities, compareFunctionGeneratePreWithGroupedActivities);
		std::stable_sort(&copyOfInitialPermutation[0], &copyOfInitialPermutation[gt.rules.nInternalActivities], compareFunctionGeneratePreWithGroupedActivities);

		for(int q=1; q<gt.rules.nInternalActivities; q++){
			int i=copyOfInitialPermutation[q-1];
			int j=copyOfInitialPermutation[q];
			assert( nIncompatibleFromFather[i]>nIncompatibleFromFather[j]
			 || (nIncompatibleFromFather[i]==nIncompatibleFromFather[j] && nMinDaysConstraintsBroken[i]>nMinDaysConstraintsBroken[j])
			 || (nIncompatibleFromFather[i]==nIncompatibleFromFather[j] && nMinDaysConstraintsBroken[i]==nMinDaysConstraintsBroken[j] && nIncompatible[i]>=nIncompatible[j] ) );
		}
	}
	
	if(VERBOSE){
		cout<<"The order of activities (id-s):"<<endl;
		for(int i=0; i<gt.rules.nInternalActivities; i++){
			cout<<"No: "<<i+1;
		
			Activity* act=&gt.rules.internalActivitiesList[copyOfInitialPermutation[i]];
			cout<<", Id="<<act->id;
			
			bool spacerBefore=false, spacerAfter=false;
			if(act->isSplit()){
				if(act->id==act->activityGroupId){
					spacerBefore=false;
					spacerAfter=true;
				}
				else{
					spacerBefore=true;
					spacerAfter=false;
				}
			}
			cout<<", ";
			if(spacerBefore)
				cout<<"    ";
			if(act->isSplit())
				cout<<"Duration: "<<act->duration<<"/"<<act->totalDuration;
			else
				cout<<"Duration: "<<act->duration;
			cout<<", ";
			if(spacerAfter)
				cout<<"    ";
			
			cout<<"Teachers: ";
			QString tj=act->teachersNames.join(" ");
			cout<<qPrintable(tj);
			cout<<", Subject: "<<qPrintable(act->subjectName);
			if(act->activityTagsNames.count()>0){
				QString atj=act->activityTagsNames.join(" ");
				cout<<", Activity tags: "<<qPrintable(atj);
			}
			cout<<", Students: ";
			QString sj=act->studentsNames.join(" ");
			cout<<qPrintable(sj);
			
			cout<<", nIncompatible[copyOfInitialPermutation[i]]="<<nIncompatible[copyOfInitialPermutation[i]];
			if(nMinDaysConstraintsBroken[copyOfInitialPermutation[i]]>0.0)
				cout<<", nMinDaysConstraintsBroken[copyOfInitialPermutation[i]]="<<nMinDaysConstraintsBroken[copyOfInitialPermutation[i]];
				
			if(gt.rules.groupActivitiesInInitialOrderList.count()>0)
				if(fatherActivityInInitialOrder[copyOfInitialPermutation[i]]>=0)
					cout<<" (grouped with id "<<gt.rules.internalActivitiesList[fatherActivityInInitialOrder[copyOfInitialPermutation[i]]].id<<")";
			
			cout<<endl;
		}
		cout<<"End - the order of activities (id-s):"<<endl;
	}

	QString s="";
	s+=GeneratePreTranslate::tr("This is the initial evaluation order of activities computed by FET."
	 " If the generation gets stuck after a certain number of activities, please check the activity"
	 " corresponding to this number+1. You might find errors in your input.");
	s+="\n\n";
	s+=GeneratePreTranslate::tr("The initial order of activities (id-s):");
	s+="\n\n";
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		initialOrderOfActivitiesIndices[i]=copyOfInitialPermutation[i];
	
		s+=GeneratePreTranslate::tr("No: %1", "Number").arg(i+1);
		s+=", ";
	
		Activity* act=&gt.rules.internalActivitiesList[copyOfInitialPermutation[i]];
		s+=GeneratePreTranslate::tr("Id: %1", "Id of activity").arg(act->id);
		s+=", ";

		bool spacerBefore=false, spacerAfter=false;
		if(act->isSplit()){
			if(act->id==act->activityGroupId){
				spacerBefore=false;
				spacerAfter=true;
			}
			else{
				spacerBefore=true;
				spacerAfter=false;
			}
		}
		if(spacerBefore)
			s+="    ";
		if(act->isSplit())
			s+=GeneratePreTranslate::tr("Duration: %1").arg(CustomFETString::number(act->duration)+QString("/")+CustomFETString::number(act->totalDuration));
		else
			s+=GeneratePreTranslate::tr("Duration: %1").arg(act->duration);
		s+=", ";
		if(spacerAfter)
			s+="    ";
		
		s+=GeneratePreTranslate::tr("Teachers: %1").arg(act->teachersNames.join(", "));
		s+=", ";
		s+=GeneratePreTranslate::tr("Subject: %1").arg(act->subjectName);
		s+=", ";
		//if(act->activityTagsNames.count()>0)
		s+=GeneratePreTranslate::tr("Activity tags: %1").arg(act->activityTagsNames.join(", "));
		s+=", ";
		s+=GeneratePreTranslate::tr("Students: %1").arg(act->studentsNames.join(", "));

		if(gt.rules.groupActivitiesInInitialOrderList.count()>0){
			s+=", ";
			s+=GeneratePreTranslate::tr("nIncompatible: %1").arg(nIncompatible[copyOfInitialPermutation[i]]);
			if(nMinDaysConstraintsBroken[copyOfInitialPermutation[i]]>0.0){
				s+=", ";
				s+=GeneratePreTranslate::tr("nMinDaysConstraintsBroken: %1").arg(nMinDaysConstraintsBroken[copyOfInitialPermutation[i]]);
			}
			
			if(fatherActivityInInitialOrder[copyOfInitialPermutation[i]]>=0){
				s+=" ";
				s+=GeneratePreTranslate::tr("(grouped with id %1)").arg(gt.rules.internalActivitiesList[fatherActivityInInitialOrder[copyOfInitialPermutation[i]]].id);
			}
		}
		
		s+="\n";
	}
	s+=GeneratePreTranslate::tr("End - the order of activities (id-s)");
	s+="\n";
	initialOrderOfActivities=s;
	
	if(initialOrderStream!=nullptr){
		(*initialOrderStream)<<s;
		(*initialOrderStream).flush();
	}
}
