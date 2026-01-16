/*
File generate_pre.h
*/

/***************************************************************************
                          generate_pre.h  -  description
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

#ifndef GENERATE_PRE_H
#define GENERATE_PRE_H

#include "timetable_defs.h"
#include "timetable.h"

#include "matrix.h"

#include <QSet>
#include <QHash>
#include <QList>
#include <QPair>

#include <list>

#include <tuple>

#include <QTextStream>

#include <QtGlobal>

class QWidget;

const int MIN_HOURS_DAILY_INDEX_IN_ARRAY=1;

extern Matrix1D<QSet<int>> tmpPreferredRealRooms;
extern Matrix1D<bool> tmpFoundNonEmpty;

extern QSet<int> fixedVirtualSpaceNonZeroButNotTimeActivities;

//extern bool thereAreTeachersWithMaxHoursDailyWithUnder100Weight;
extern bool thereAreTeachersWithMaxHoursDailyOrPerRealDayWithUnder100Weight;
//extern bool thereAreSubgroupsWithMaxHoursDailyWithUnder100Weight;
extern bool thereAreSubgroupsWithMaxHoursDailyOrPerRealDayWithUnder100Weight;

extern Matrix1D<QList<SpaceConstraint*>> constraintsForActivity;

extern Matrix1D<bool> visitedActivityResultantRealRooms;
////////end rooms

extern Matrix1D<int> initialOrderOfActivitiesIndices;

extern Matrix1D<int> daysTeacherIsAvailable;
extern Matrix1D<int> daysSubgroupIsAvailable;
extern Matrix1D<int> requestedDaysForTeachers;
extern Matrix1D<int> requestedDaysForSubgroups;
extern Matrix1D<int> nReqForTeacher;
extern Matrix1D<int> nReqForSubgroup;

//used only in homeRoomsAreOk
extern Matrix1D<int> nHoursRequiredForRoom;
extern Matrix1D<int> nHoursAvailableForRoom;
/////////////////////////////

extern bool haveTeacherRoomNotAllowedTimesConstraints;
qint64 teacherRoomQInt64Combination(int teacher, int room);
qint64 dayHourQInt64Combination(int day, int hour);
extern QHash<QPair<qint64, qint64>, double> notAllowedTeacherRoomTimePercentages;
//first qint64 is (teacher, room), second qint64 is (day, hour)
bool computeNotAllowedTeacherRoomTimePercentages();

class PreferredRoomsItem{
public:
	double percentage;
	QSet<int> preferredRooms;
};

/////////used only in sortActivities
extern Matrix1D<int> nIncompatible;
extern Matrix1D<double> nMinDaysConstraintsBroken;
extern Matrix1D<int> nRoomsIncompat;
extern Matrix1D<double> nHoursForRoom;
extern Matrix1D<PreferredRoomsItem> maxPercentagePrefRooms;
extern Matrix1D<int> reprNInc;
////////////////////////////////////

extern Matrix1D<int> nIncompatibleFromFather;
extern Matrix1D<int> fatherActivityInInitialOrder;
////////////////////////////////////

inline bool findEquivalentSubgroupsCompareFunction(int i1, int i2);

inline bool compareFunctionGeneratePre(int i, int j);

inline bool compareFunctionGeneratePreWithGroupedActivities(int i, int j);

//extern Matrix1D<int> permutation;
extern Matrix1D<int> copyOfInitialPermutation;
//the permutation matrix to obtain activities in decreasing difficulty order

bool processTimeSpaceConstraints(QWidget* parent, QTextStream* initialOrderStream=nullptr);


////////BEGIN BASIC TIME CONSTRAINTS
//a value >=0 equal with the weight of the maximum weightPercentage of a basic time constraint
extern Matrix1D<QHash<int, int>> activitiesConflictingPercentage;

bool computeActivitiesConflictingPercentage(QWidget* parent);

void sortActivities(QWidget* parent, const QHash<int, int>& reprSameStartingTime, const QHash<int, QSet<int>>& reprSameActivitiesSet, QTextStream* initialOrderStream=nullptr);
////////END   BASIC TIME CONSTRAINTS


////////BEGIN MIN DAYS TIME CONSTRAINTS
extern Matrix1D<QList<int>> minDaysListOfActivities;
extern Matrix1D<QList<int>> minDaysListOfMinDays;
extern Matrix1D<QList<double>> minDaysListOfWeightPercentages;
extern Matrix1D<QList<bool>> minDaysListOfConsecutiveIfSameDay;
extern std::list<QList<int>> minDaysListOfActivitiesFromThisConstraint;
extern Matrix1D<QList<QList<int>*>> minDaysListOfActivitiesFromTheSameConstraint;

bool computeMinDays(QWidget* parent);
////////END   MIN DAYS TIME CONSTRAINTS


////////BEGIN MIN HALF DAYS TIME CONSTRAINTS - only for the Mornings-Afternoons mode
extern Matrix1D<QList<int>> minHalfDaysListOfActivities;
extern Matrix1D<QList<int>> minHalfDaysListOfMinDays;
extern Matrix1D<QList<double>> minHalfDaysListOfWeightPercentages;
extern Matrix1D<QList<bool>> minHalfDaysListOfConsecutiveIfSameDay;
extern std::list<QList<int>> minHalfDaysListOfActivitiesFromThisConstraint;
extern Matrix1D<QList<QList<int>*>> minHalfDaysListOfActivitiesFromTheSameConstraint;

bool computeMinHalfDays(QWidget* parent);
////////END   MIN HALF DAYS TIME CONSTRAINTS - only for the Mornings-Afternoons mode


////////BEGIN MAX DAYS TIME CONSTRAINTS
extern Matrix1D<QList<int>> maxDaysListOfActivities;
extern Matrix1D<QList<int>> maxDaysListOfMaxDays;
extern Matrix1D<QList<double>> maxDaysListOfWeightPercentages;

bool computeMaxDays(QWidget* parent);
////////END   MAX DAYS TIME CONSTRAINTS


////////BEGIN MAX HALF DAYS TIME CONSTRAINTS
extern Matrix1D<QList<int>> maxHalfDaysListOfActivities;
extern Matrix1D<QList<int>> maxHalfDaysListOfMaxDays;
extern Matrix1D<QList<double>> maxHalfDaysListOfWeightPercentages;

bool computeMaxHalfDays(QWidget* parent);
////////END   MAX HALF DAYS TIME CONSTRAINTS


////////BEGIN MAX HOURLY SPAN TIME CONSTRAINTS
extern Matrix1D<QList<int>> maxHourlySpanListOfActivities;
extern Matrix1D<QList<int>> maxHourlySpanListOfMaxSpan;
extern Matrix1D<QList<double>> maxHourlySpanListOfWeightPercentages;

bool computeMaxHourlySpan(QWidget* parent);
////////END   MAX HOURLY SPAN TIME CONSTRAINTS


////////BEGIN MAX TERMS TIME CONSTRAINTS
extern Matrix1D<QList<int>> maxTermsListOfActivities;
extern Matrix1D<QList<int>> maxTermsListOfMaxTerms;
extern Matrix1D<QList<double>> maxTermsListOfWeightPercentages;

bool computeMaxTerms(QWidget* parent);
////////END   MAX TERMS TIME CONSTRAINTS


////////BEGIN MIN GAPS between activities TIME CONSTRAINTS
extern Matrix1D<QList<int>> minGapsBetweenActivitiesListOfActivities;
extern Matrix1D<QList<int>> minGapsBetweenActivitiesListOfMinGaps;
extern Matrix1D<QList<double>> minGapsBetweenActivitiesListOfWeightPercentages;

bool computeMinGapsBetweenActivities(QWidget* parent);
////////END	 MIN GAPS between activities TIME CONSTRAINTS

////////BEGIN MAX GAPS between activities TIME CONSTRAINTS
extern Matrix1D<QList<int>> maxGapsBetweenActivitiesListOfActivities;
extern Matrix1D<QList<int>> maxGapsBetweenActivitiesListOfMaxGaps;
extern Matrix1D<QList<double>> maxGapsBetweenActivitiesListOfWeightPercentages;

bool computeMaxGapsBetweenActivities(QWidget* parent);
////////END	 MAX GAPS between activities TIME CONSTRAINTS


////////BEGIN students set not available, teacher not available, break, activity preferred time,
////////activity preferred times, activities preferred times
//percentage of allowed time, -1 if no restriction
extern Matrix2D<double> notAllowedTimesPercentages;

//break, which is not considered gap, false means no break, true means 100% break
//break can only be 100% or none
extern Matrix2D<bool> breakDayHour;

//students set not available, which is not considered gap, false means available, true means 100% not available
//students set not available can only be 100% or none
extern Matrix3D<bool> subgroupNotAvailableDayHour;

//used in students timetable view time horizontal dialog
extern QHash<QString, QSet<QPair<int, int>>> studentsSetNotAvailableDayHour;

//teacher not available, which is not considered gap, false means available, true means 100% not available
//teacher not available can only be 100% or none
extern Matrix3D<bool> teacherNotAvailableDayHour;

bool computeNotAllowedTimesPercentages(QWidget* parent);
////////END   students set not available, teacher not available, break, activity preferred time,
////////activity preferred time, activities preferred times


////////BEGIN students max gaps and early
bool computeNHoursPerSubgroup(QWidget* parent);
bool computeSubgroupsEarlyAndMaxGapsPercentages(QWidget* parent);

extern Matrix1D<double> subgroupsEarlyMaxBeginningsAtSecondHourPercentage;
extern Matrix1D<int> subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings;
extern Matrix1D<double> subgroupsMaxGapsPerWeekPercentage;
extern Matrix1D<int> subgroupsMaxGapsPerWeekMaxGaps;

extern Matrix1D<int> nHoursPerSubgroup; //used also for students min hours daily

//max gaps per day (not perfect!!!)
bool computeSubgroupsMaxGapsPerDayPercentages(QWidget* parent);

extern Matrix1D<double> subgroupsMaxGapsPerDayPercentage;
extern Matrix1D<int> subgroupsMaxGapsPerDayMaxGaps;
extern bool haveStudentsMaxGapsPerDay;

//max gaps per real day (not perfect!!!)
bool computeSubgroupsMaxGapsPerRealDayPercentages(QWidget* parent);

extern Matrix1D<double> subgroupsMaxGapsPerRealDayPercentage;
extern Matrix1D<int> subgroupsMaxGapsPerRealDayMaxGaps;
extern bool haveStudentsMaxGapsPerRealDay;

//2020-07-29
//max gaps per week for real days (not perfect!!!)
extern Matrix1D<double> subgroupsMaxGapsPerWeekForRealDaysPercentage;
extern Matrix1D<int> subgroupsMaxGapsPerWeekForRealDaysMaxGaps;
//extern bool haveStudentsMaxGapsPerRealDay;
////////END   students max gaps and early

////////BEGIN students max days per week
//activities indices (in 0..gt.rules.nInternalActivities-1) for each subgroup
extern Matrix1D<int> subgroupsMaxDaysPerWeekMaxDays; //-1 for not existing
extern Matrix1D<double> subgroupsMaxDaysPerWeekWeightPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
extern Matrix1D<QList<int>> subgroupsWithMaxDaysPerWeekForActivities;

//for mornings-afternoons
//activities indices (in 0..gt.rules.nInternalActivities-1) for each subgroup
extern Matrix1D<int> subgroupsMaxRealDaysPerWeekMaxDays; //-1 for not existing
extern Matrix1D<double> subgroupsMaxRealDaysPerWeekWeightPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
extern Matrix1D<QList<int>> subgroupsWithMaxRealDaysPerWeekForActivities;

////////BEGIN teachers max days per week
//activities indices (in 0..gt.rules.nInternalActivities-1) for each teacher
extern Matrix1D<int> teachersMaxDaysPerWeekMaxDays; //-1 for not existing
extern Matrix1D<double> teachersMaxDaysPerWeekWeightPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
extern Matrix1D<QList<int>> teachersWithMaxDaysPerWeekForActivities;

bool computeMaxDaysPerWeekForTeachers(QWidget* parent);

bool computeMaxDaysPerWeekForStudents(QWidget* parent);
////////END   teachers max days per week


//2025-05-30
extern Matrix1D<int> teachersMaxHoursPerTermMaxHours; //-1 for not existing
extern Matrix1D<double> teachersMaxHoursPerTermWeightPercentages; //-1 for not existing
extern Matrix1D<QList<int>> teachersWithMaxHoursPerTermForActivities;
bool computeMaxHoursPerTermForTeachers(QWidget* parent);


////////BEGIN teachers no two consecutive days
//activities indices (in 0..gt.rules.nInternalActivities-1) for each teacher
extern Matrix1D<double> teachersNoTwoConsecutiveDaysPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
extern Matrix1D<QList<int>> teachersWithNoTwoConsecutiveDaysForActivities;

bool computeNoTwoConsecutiveDaysForTeachers(QWidget* parent);
////////END   teachers no two consecutive days


////////BEGIN teachers max three consecutive days
//activities indices (in 0..gt.rules.nInternalActivities-1) for each teacher
extern Matrix1D<bool> teachersMaxThreeConsecutiveDaysAllowAMAMException;
extern Matrix1D<double> teachersMaxThreeConsecutiveDaysPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
extern Matrix1D<QList<int>> teachersWithMaxThreeConsecutiveDaysForActivities;

bool computeMaxThreeConsecutiveDaysForTeachers(QWidget* parent);
////////END   teachers max three consecutive days

////////BEGIN students max three consecutive days
//activities indices (in 0..gt.rules.nInternalActivities-1) for each subgroup
extern Matrix1D<bool> subgroupsMaxThreeConsecutiveDaysAllowAMAMException;
extern Matrix1D<double> subgroupsMaxThreeConsecutiveDaysPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
extern Matrix1D<QList<int>> subgroupsWithMaxThreeConsecutiveDaysForActivities;

bool computeMaxThreeConsecutiveDaysForStudents(QWidget* parent);
////////END   students max three consecutive days


//for mornings-afternoons
//activities indices (in 0..gt.rules.nInternalActivities-1) for each teacher
extern Matrix1D<int> teachersMaxRealDaysPerWeekMaxDays; //-1 for not existing
extern Matrix1D<double> teachersMaxRealDaysPerWeekWeightPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
extern Matrix1D<QList<int>> teachersWithMaxRealDaysPerWeekForActivities;

bool computeMaxRealDaysPerWeekForTeachers(QWidget* parent);
bool computeMaxAfternoonsPerWeekForTeachers(QWidget* parent);
bool computeMaxMorningsPerWeekForTeachers(QWidget* parent);

bool computeMaxRealDaysPerWeekForStudents(QWidget* parent);
bool computeMaxAfternoonsPerWeekForStudents(QWidget* parent);
bool computeMaxMorningsPerWeekForStudents(QWidget* parent);


////////BEGIN teachers max afternoons per week
//activities indices (in 0..gt.rules.nInternalActivities-1) for each teacher
extern Matrix1D<int> teachersMaxAfternoonsPerWeekMaxAfternoons; //-1 for not existing
extern Matrix1D<double> teachersMaxAfternoonsPerWeekWeightPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
extern Matrix1D<QList<int>> teachersWithMaxAfternoonsPerWeekForActivities;

////////BEGIN teachers max mornings per week
//activities indices (in 0..gt.rules.nInternalActivities-1) for each teacher
extern Matrix1D<int> teachersMaxMorningsPerWeekMaxMornings; //-1 for not existing
extern Matrix1D<double> teachersMaxMorningsPerWeekWeightPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
extern Matrix1D<QList<int>> teachersWithMaxMorningsPerWeekForActivities;

//bool computeMaxRealDaysPerWeekForTeachers(QWidget* parent);
////////END teachers max mornings per week


////////BEGIN students max afternoons per week
//activities indices (in 0..gt.rules.nInternalActivities-1) for each subgroup
extern Matrix1D<int> subgroupsMaxAfternoonsPerWeekMaxAfternoons; //-1 for not existing
extern Matrix1D<double> subgroupsMaxAfternoonsPerWeekWeightPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
extern Matrix1D<QList<int>> subgroupsWithMaxAfternoonsPerWeekForActivities;

////////BEGIN students max mornings per week
//activities indices (in 0..gt.rules.nInternalActivities-1) for each subgroup
extern Matrix1D<int> subgroupsMaxMorningsPerWeekMaxMornings; //-1 for not existing
extern Matrix1D<double> subgroupsMaxMorningsPerWeekWeightPercentages; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
extern Matrix1D<QList<int>> subgroupsWithMaxMorningsPerWeekForActivities;
////////END students max mornings per week


bool computeTeachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentages(QWidget* parent);
extern Matrix1D<double> teachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentage;
extern Matrix1D<int> teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings;

extern bool haveTeachersAfternoonsEarly;


bool computeSubgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentages(QWidget* parent);
extern Matrix1D<double> subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentage;
extern Matrix1D<int> subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings;

extern bool haveStudentsAfternoonsEarly;


bool computeTeachersMorningsEarlyMaxBeginningsAtSecondHourPercentages(QWidget* parent);
extern Matrix1D<double> teachersMorningsEarlyMaxBeginningsAtSecondHourPercentage;
extern Matrix1D<int> teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings;

extern bool haveTeachersMorningsEarly;


bool computeSubgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentages(QWidget* parent);
extern Matrix1D<double> subgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentage;
extern Matrix1D<int> subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings;

extern bool haveStudentsMorningsEarly;


////////BEGIN teachers max gaps per week and per day and per morning and afternoon
extern Matrix1D<double> teachersMaxGapsPerWeekPercentage;
extern Matrix1D<int> teachersMaxGapsPerWeekMaxGaps;

extern Matrix1D<double> teachersMaxGapsPerDayPercentage;
extern Matrix1D<int> teachersMaxGapsPerDayMaxGaps;

extern Matrix1D<double> teachersMaxGapsPerMorningAndAfternoonPercentage;
extern Matrix1D<int> teachersMaxGapsPerMorningAndAfternoonMaxGaps;

extern Matrix1D<int> nHoursPerTeacher;
bool computeNHoursPerTeacher(QWidget* parent);
bool computeTeachersMaxGapsPerWeekPercentage(QWidget* parent);
bool computeTeachersMaxGapsPerDayPercentage(QWidget* parent);
bool computeTeachersMaxGapsPerMorningAndAfternoonPercentage(QWidget* parent);
////////END   teachers max gaps per week and per day and per morning and afternoon


//max gaps per real day - not perfect!!!
extern Matrix1D<double> teachersMaxGapsPerRealDayPercentage;
extern Matrix1D<int> teachersMaxGapsPerRealDayMaxGaps;
extern Matrix1D<bool> teachersMaxGapsPerRealDayAllowException;
extern bool haveTeachersMaxGapsPerRealDay;

bool computeTeachersMaxGapsPerRealDayPercentage(QWidget* parent);

//2020-07-29
//max gaps per week for real days - not perfect!!!
extern Matrix1D<double> teachersMaxGapsPerWeekForRealDaysPercentage;
extern Matrix1D<int> teachersMaxGapsPerWeekForRealDaysMaxGaps;
//extern bool haveTeachersMaxGapsPerRealDay;

//bool computeTeachersMaxGapsPerWeekForRealDaysPercentage(QWidget* parent);
////////END   teachers max gaps

extern Matrix1D<bool> teacherConstrainedToZeroGapsPerAfternoon;
bool computeTeachersConstrainedToZeroGapsPerAfternoon(QWidget* parent);


////////BEGIN activities same starting time
extern Matrix1D<QList<int>> activitiesSameStartingTimeActivities;
extern Matrix1D<QList<double>> activitiesSameStartingTimePercentages;
bool computeActivitiesSameStartingTime(QWidget* parent, QHash<int, int> & reprSameStartingTime, QHash<int, QSet<int>> & reprSameActivitiesSet);
////////END   activities same starting time


////////BEGIN activities same starting hour
extern Matrix1D<QList<int>> activitiesSameStartingHourActivities;
extern Matrix1D<QList<double>> activitiesSameStartingHourPercentages;
void computeActivitiesSameStartingHour();
////////END   activities same starting hour


////////BEGIN activities same starting day
extern Matrix1D<QList<int>> activitiesSameStartingDayActivities;
extern Matrix1D<QList<double>> activitiesSameStartingDayPercentages;
void computeActivitiesSameStartingDay();
////////END   activities same starting hour


////////BEGIN activities not overlapping
extern Matrix1D<QList<int>> activitiesNotOverlappingActivities;
extern Matrix1D<QList<double>> activitiesNotOverlappingPercentages;
void computeActivitiesNotOverlapping();
////////END   activities not overlapping


//2025-09-22 - Constraint activities overlap completely or do not overlap
extern bool haveActivitiesOverlapCompletelyOrDoNotOverlap;

class ActivitiesOverlapCompletelyOrDoNotOverlap_item
{
public:
	//double weight; -> must be 100.0%
	QList<int> activitiesList;
	//QSet<int> activitiesSet;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<ActivitiesOverlapCompletelyOrDoNotOverlap_item> aocodnoList;
extern Matrix1D<QList<ActivitiesOverlapCompletelyOrDoNotOverlap_item*>> aocodnoListForActivity;

bool computeActivitiesOverlapCompletelyOrDoNotOverlap(QWidget* parent);


//2025-08-09 - Constraint activities pair of mutually exclusive sets of time slots
extern bool haveActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots;

class ActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots_item
{
public:
	//double weight; -> must be 100.0%
	QList<int> activitiesList;
	//QSet<int> activitiesSet;
	QSet<int> timeSlotsSet1;
	QList<int> timeSlotsList1;
	QSet<int> timeSlotsSet2;
	QList<int> timeSlotsList2;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<ActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots_item> apomesotsList;
extern Matrix1D<QList<ActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots_item*>> apomesotsListForActivity;

bool computeActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots(QWidget* parent);


//2025-08-09 - Constraint activities pair of mutually exclusive time slots
extern bool haveActivitiesPairOfMutuallyExclusiveTimeSlots;

class ActivitiesPairOfMutuallyExclusiveTimeSlots_item
{
public:
	//double weight; -> must be 100.0%
	QList<int> activitiesList;
	//QSet<int> activitiesSet;
	int day1;
	int hour1;
	int day2;
	int hour2;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<ActivitiesPairOfMutuallyExclusiveTimeSlots_item> apometsList;
extern Matrix1D<QList<ActivitiesPairOfMutuallyExclusiveTimeSlots_item*>> apometsListForActivity;

bool computeActivitiesPairOfMutuallyExclusiveTimeSlots(QWidget* parent);


//BEGIN activities occupy max sets of time slots from selection
extern bool haveActivitiesOccupyMaxSetsOfTimeSlotsFromSelection;

class ActivitiesOccupyMaxSetsOfTimeSlotsFromSelection_item
{
public:
	//weight is 100.0%
	QList<int> listOfActivities;
	//QSet<int> activitiesSet;
	int maxOccupiedSets;
	QList<QList<int>> listOfLists;
	//Matrix1D<int> timeToListIndex;
	QList<int> timeToListIndex;
};

extern std::list<ActivitiesOccupyMaxSetsOfTimeSlotsFromSelection_item> aomsotsfsList;
extern Matrix1D<QList<ActivitiesOccupyMaxSetsOfTimeSlotsFromSelection_item*>> aomsotsfsListForActivity;

bool computeActivitiesOccupyMaxSetsOfTimeSlotsFromSelection(QWidget* parent);
//END   activities occupy max sets of time slots from selection

//BEGIN teacher(s) occupy max sets of time slots from selection
extern bool haveTeachersOccupyMaxSetsOfTimeSlotsFromSelection;

class TeachersOccupyMaxSetsOfTimeSlotsFromSelection_item
{
public:
	//weight is 100.0%
	int maxOccupiedSets;
	QList<QList<int>> listOfLists;
	//Matrix1D<int> timeToListIndex;
	QList<int> timeToListIndex;
};

/*extern Matrix1D<QList<double>> teachersOccupyMaxSetsOfTimeSlotsFromSelectionPercentages;
extern Matrix1D<QList<int>> teachersOccupyMaxSetsOfTimeSlotsFromSelectionMaxOccupiedSets;
extern Matrix1D<QList<QList<QSet<int>>>> teachersOccupyMaxSetsOfTimeSlotsFromSelectionListOfSets;
extern Matrix1D<QList<QList<QList<int>>>> teachersOccupyMaxSetsOfTimeSlotsFromSelectionListOfLists;*/

extern std::list<TeachersOccupyMaxSetsOfTimeSlotsFromSelection_item> tomsotsfsList;
extern Matrix1D<QList<TeachersOccupyMaxSetsOfTimeSlotsFromSelection_item*>> tomsotsfsListForTeacher;

bool computeTeachersOccupyMaxSetsOfTimeSlotsFromSelection(QWidget* parent);
//END   teacher(s) occupy max sets of time slots from selection


//BEGIN students (set) occupy max sets of time slots from selection
extern bool haveStudentsOccupyMaxSetsOfTimeSlotsFromSelection;

class StudentsOccupyMaxSetsOfTimeSlotsFromSelection_item
{
public:
	//weight percentage is 100.0%
	int maxOccupiedSets;
	QList<QList<int>> listOfLists;
	//Matrix1D<int> timeToListIndex;
	QList<int> timeToListIndex;
};

/*extern Matrix1D<QList<double>> subgroupsOccupyMaxSetsOfTimeSlotsFromSelectionPercentages;
extern Matrix1D<QList<int>> subgroupsOccupyMaxSetsOfTimeSlotsFromSelectionMaxOccupiedSets;
extern Matrix1D<QList<QList<QSet<int>>>> subgroupsOccupyMaxSetsOfTimeSlotsFromSelectionListOfSets;
extern Matrix1D<QList<QList<QList<int>>>> subgroupsOccupyMaxSetsOfTimeSlotsFromSelectionListOfLists;*/

extern std::list<StudentsOccupyMaxSetsOfTimeSlotsFromSelection_item> somsotsfsList;
extern Matrix1D<QList<StudentsOccupyMaxSetsOfTimeSlotsFromSelection_item*>> somsotsfsListForSubgroup;

bool computeSubgroupsOccupyMaxSetsOfTimeSlotsFromSelection(QWidget* parent);
//END   students (set) occupy max sets of time slots from selection


//BEGIN teacher(s) pair of mutually exclusive time slots
extern bool haveTeachersPairOfMutualExclusiveTimeSlots;

extern Matrix1D<QList<double>> teachersPairOfMutuallyExclusiveTimeSlotsPercentages;
extern Matrix1D<QList<int>> teachersPairOfMutuallyExclusiveTimeSlotsDay1;
extern Matrix1D<QList<int>> teachersPairOfMutuallyExclusiveTimeSlotsHour1;
extern Matrix1D<QList<int>> teachersPairOfMutuallyExclusiveTimeSlotsDay2;
extern Matrix1D<QList<int>> teachersPairOfMutuallyExclusiveTimeSlotsHour2;

bool computeTeachersPairOfMutuallyExclusiveTimeSlots(QWidget* parent);
//END   teacher(s) pair of mutually exclusive time slots

//BEGIN students(s) pair of mutually exclusive time slots
extern bool haveStudentsPairOfMutualExclusiveTimeSlots;

extern Matrix1D<QList<double>> subgroupsPairOfMutuallyExclusiveTimeSlotsPercentages;
extern Matrix1D<QList<int>> subgroupsPairOfMutuallyExclusiveTimeSlotsDay1;
extern Matrix1D<QList<int>> subgroupsPairOfMutuallyExclusiveTimeSlotsHour1;
extern Matrix1D<QList<int>> subgroupsPairOfMutuallyExclusiveTimeSlotsDay2;
extern Matrix1D<QList<int>> subgroupsPairOfMutuallyExclusiveTimeSlotsHour2;

bool computeSubgroupsPairOfMutuallyExclusiveTimeSlots(QWidget* parent);
//END   students(s) pair of mutually exclusive time slots


//BEGIN teacher(s) pair of mutually exclusive sets of time slots
extern bool haveTeachersPairOfMutualExclusiveSetsOfTimeSlots;

class TeachersPairOfMutuallyExclusiveSetsOfTimeSlots_item
{
public:
	//weight percentage is 100.0%
	QSet<int> set1;
	QList<int> list1;
	QSet<int> set2;
	QList<int> list2;
};

/*extern Matrix1D<QList<double>> teachersPairOfMutuallyExclusiveSetsOfTimeSlotsPercentages;
extern Matrix1D<QList<QSet<int>>> teachersPairOfMutuallyExclusiveSetsOfTimeSlotsTimeSlotsSet1;
extern Matrix1D<QList<QList<int>>> teachersPairOfMutuallyExclusiveSetsOfTimeSlotsTimeSlotsList1;
extern Matrix1D<QList<QSet<int>>> teachersPairOfMutuallyExclusiveSetsOfTimeSlotsTimeSlotsSet2;
extern Matrix1D<QList<QList<int>>> teachersPairOfMutuallyExclusiveSetsOfTimeSlotsTimeSlotsList2;*/

extern std::list<TeachersPairOfMutuallyExclusiveSetsOfTimeSlots_item> tpomesotsList;
extern Matrix1D<QList<TeachersPairOfMutuallyExclusiveSetsOfTimeSlots_item*>> tpomesotsListForTeacher;

bool computeTeachersPairOfMutuallyExclusiveSetsOfTimeSlots(QWidget* parent);
//END   teacher(s) pair of mutually exclusive sets of time slots

//BEGIN students(s) pair of mutually exclusive sets of time slots
extern bool haveStudentsPairOfMutualExclusiveSetsOfTimeSlots;

class StudentsPairOfMutuallyExclusiveSetsOfTimeSlots_item
{
public:
	//weight percentage is 100.0%
	QSet<int> set1;
	QList<int> list1;
	QSet<int> set2;
	QList<int> list2;
};

/*extern Matrix1D<QList<double>> subgroupsPairOfMutuallyExclusiveSetsOfTimeSlotsPercentages;
extern Matrix1D<QList<QSet<int>>> subgroupsPairOfMutuallyExclusiveSetsOfTimeSlotsTimeSlotsSet1;
extern Matrix1D<QList<QList<int>>> subgroupsPairOfMutuallyExclusiveSetsOfTimeSlotsTimeSlotsList1;
extern Matrix1D<QList<QSet<int>>> subgroupsPairOfMutuallyExclusiveSetsOfTimeSlotsTimeSlotsSet2;
extern Matrix1D<QList<QList<int>>> subgroupsPairOfMutuallyExclusiveSetsOfTimeSlotsTimeSlotsList2;*/

extern std::list<StudentsPairOfMutuallyExclusiveSetsOfTimeSlots_item> spomesotsList;
extern Matrix1D<QList<StudentsPairOfMutuallyExclusiveSetsOfTimeSlots_item*>> spomesotsListForSubgroup;

bool computeSubgroupsPairOfMutuallyExclusiveSetsOfTimeSlots(QWidget* parent);
//END   students(s) pair of mutually exclusive sets of time slots


////////BEGIN teacher(s) max hours daily
extern Matrix1D<double> teachersMaxHoursDailyPercentages1;
extern Matrix1D<int> teachersMaxHoursDailyMaxHours1;

extern Matrix1D<double> teachersMaxHoursDailyPercentages2;
extern Matrix1D<int> teachersMaxHoursDailyMaxHours2;

bool computeTeachersMaxHoursDaily(QWidget* parent);
////////END   teacher(s) max hours daily


//BEGIN teacher(s) max hours daily in hourly interval
extern Matrix1D<QList<double>> teachersMaxHoursDailyInIntervalPercentages;
extern Matrix1D<QList<int>> teachersMaxHoursDailyInIntervalMaxHours;
extern Matrix1D<QList<int>> teachersMaxHoursDailyInIntervalStartHour;
extern Matrix1D<QList<int>> teachersMaxHoursDailyInIntervalEndHour;

bool computeTeachersMaxHoursDailyInInterval(QWidget* parent);
//END   teacher(s) max hours daily in hourly interval


////////BEGIN teacher(s) max hours daily for real days
extern Matrix1D<double> teachersMaxHoursDailyRealDaysPercentages1;
extern Matrix1D<int> teachersMaxHoursDailyRealDaysMaxHours1;

extern Matrix1D<double> teachersMaxHoursDailyRealDaysPercentages2;
extern Matrix1D<int> teachersMaxHoursDailyRealDaysMaxHours2;

bool computeTeachersMaxHoursDailyRealDays(QWidget* parent);
////////END   teacher(s) max hours daily for real days


////////BEGIN teacher(s) max hours continuously
extern Matrix1D<double> teachersMaxHoursContinuouslyPercentages1;
extern Matrix1D<int> teachersMaxHoursContinuouslyMaxHours1;

extern Matrix1D<double> teachersMaxHoursContinuouslyPercentages2;
extern Matrix1D<int> teachersMaxHoursContinuouslyMaxHours2;

bool computeTeachersMaxHoursContinuously(QWidget* parent);
////////END   teacher(s) max hours continuously


///////BEGIN teacher(s) activity tag max hours daily
extern bool haveTeachersActivityTagMaxHoursDaily;

extern Matrix1D<QList<int>> teachersActivityTagMaxHoursDailyMaxHours;
extern Matrix1D<QList<int>> teachersActivityTagMaxHoursDailyActivityTag;
extern Matrix1D<QList<double>> teachersActivityTagMaxHoursDailyPercentage;

bool computeTeachersActivityTagMaxHoursDaily(QWidget* parent);
///////END   teacher(s) activity tag max hours daily


///////BEGIN teacher(s) activity tag max hours daily real days
extern bool haveTeachersActivityTagMaxHoursDailyRealDays;

extern Matrix1D<QList<int>> teachersActivityTagMaxHoursDailyRealDaysMaxHours;
extern Matrix1D<QList<int>> teachersActivityTagMaxHoursDailyRealDaysActivityTag;
extern Matrix1D<QList<double>> teachersActivityTagMaxHoursDailyRealDaysPercentage;

bool computeTeachersActivityTagMaxHoursDailyRealDays(QWidget* parent);
///////END   teacher(s) activity tag max hours daily real days


///////BEGIN teacher(s) activity tag max hours continuously
extern bool haveTeachersActivityTagMaxHoursContinuously;

extern Matrix1D<QList<int>> teachersActivityTagMaxHoursContinuouslyMaxHours;
extern Matrix1D<QList<int>> teachersActivityTagMaxHoursContinuouslyActivityTag;
extern Matrix1D<QList<double>> teachersActivityTagMaxHoursContinuouslyPercentage;

bool computeTeachersActivityTagMaxHoursContinuously(QWidget* parent);
///////END   teacher(s) activity tag max hours continuously


////////BEGIN teacher(s) min hours daily
extern Matrix2D<double> teachersMinHoursDailyPercentages; //If having mornings-afternoons: 0 is for morning, 1 is for afternoon
extern Matrix2D<int> teachersMinHoursDailyMinHours; //If having official: both should be equal, but we use 1.

extern Matrix1D<double> teachersMinHoursPerAfternoonPercentages;
extern Matrix1D<int> teachersMinHoursPerAfternoonMinHours;

bool computeTeachersMinHoursDaily(QWidget* parent);

//real days
extern Matrix1D<double> teachersMinHoursDailyRealDaysPercentages;
extern Matrix1D<int> teachersMinHoursDailyRealDaysMinHours;

bool computeTeachersMinHoursDailyRealDays(QWidget* parent);
////////END   teacher(s) min hours daily


////////BEGIN teacher(s) min days per week
extern Matrix1D<double> teachersMinDaysPerWeekPercentages;
extern Matrix1D<int> teachersMinDaysPerWeekMinDays;

bool computeTeachersMinDaysPerWeek(QWidget* parent);
////////END   teacher(s) min days per week


////////BEGIN teacher(s) min real days per week
extern Matrix1D<double> teachersMinRealDaysPerWeekPercentages;
extern Matrix1D<int> teachersMinRealDaysPerWeekMinDays;

bool computeTeachersMinRealDaysPerWeek(QWidget* parent);
////////END   teacher(s) min real days per week



////////BEGIN teacher(s) min mornings and afternoons per week
extern Matrix1D<double> teachersMinMorningsPerWeekPercentages;
extern Matrix1D<int> teachersMinMorningsPerWeekMinMornings;
extern Matrix1D<double> teachersMinAfternoonsPerWeekPercentages;
extern Matrix1D<int> teachersMinAfternoonsPerWeekMinAfternoons;

bool computeTeachersMinMorningsAfternoonsPerWeek(QWidget* parent);
////////END   teacher(s) min mornings and afternoons per week


//2020-06-28
extern Matrix1D<double> teachersMaxHoursPerAllAfternoonsPercentages;
extern Matrix1D<int> teachersMaxHoursPerAllAfternoonsMaxHours;

bool computeTeachersMaxHoursPerAllAfternoons(QWidget* parent);
////////////


//2020-06-28
extern Matrix1D<double> subgroupsMaxHoursPerAllAfternoonsPercentages;
extern Matrix1D<int> subgroupsMaxHoursPerAllAfternoonsMaxHours;

bool computeStudentsMaxHoursPerAllAfternoons(QWidget* parent);
////////////


////////BEGIN students min mornings and afternoons per week
extern Matrix1D<double> subgroupsMinMorningsPerWeekPercentages;
extern Matrix1D<int> subgroupsMinMorningsPerWeekMinMornings;
extern Matrix1D<double> subgroupsMinAfternoonsPerWeekPercentages;
extern Matrix1D<int> subgroupsMinAfternoonsPerWeekMinAfternoons;

bool computeStudentsMinMorningsAfternoonsPerWeek(QWidget* parent);
////////END   students min mornings and afternoons per week


////////BEGIN students (set) max hours daily
extern Matrix1D<double> subgroupsMaxHoursDailyPercentages1;
extern Matrix1D<int> subgroupsMaxHoursDailyMaxHours1;

extern Matrix1D<double> subgroupsMaxHoursDailyPercentages2;
extern Matrix1D<int> subgroupsMaxHoursDailyMaxHours2;

bool computeSubgroupsMaxHoursDaily(QWidget* parent);
////////END   students (set) max hours daily


//BEGIN students (set) max hours daily in hourly interval
extern Matrix1D<QList<double>> subgroupsMaxHoursDailyInIntervalPercentages;
extern Matrix1D<QList<int>> subgroupsMaxHoursDailyInIntervalMaxHours;
extern Matrix1D<QList<int>> subgroupsMaxHoursDailyInIntervalStartHour;
extern Matrix1D<QList<int>> subgroupsMaxHoursDailyInIntervalEndHour;

bool computeSubgroupsMaxHoursDailyInInterval(QWidget* parent);
//END   students (set) max hours daily in hourly interval


////////BEGIN students (set) max hours daily real days
extern Matrix1D<double> subgroupsMaxHoursDailyRealDaysPercentages1;
extern Matrix1D<int> subgroupsMaxHoursDailyRealDaysMaxHours1;

extern Matrix1D<double> subgroupsMaxHoursDailyRealDaysPercentages2;
extern Matrix1D<int> subgroupsMaxHoursDailyRealDaysMaxHours2;

bool computeSubgroupsMaxHoursDailyRealDays(QWidget* parent);
////////END   students (set) max hours daily real days


////////BEGIN students (set) max hours continuously
extern Matrix1D<double> subgroupsMaxHoursContinuouslyPercentages1;
extern Matrix1D<int> subgroupsMaxHoursContinuouslyMaxHours1;

extern Matrix1D<double> subgroupsMaxHoursContinuouslyPercentages2;
extern Matrix1D<int> subgroupsMaxHoursContinuouslyMaxHours2;

bool computeStudentsMaxHoursContinuously(QWidget* parent);
////////END   students (set) max hours continuously


///////BEGIN students (set) activity tag max hours daily
extern bool haveStudentsActivityTagMaxHoursDaily;

extern Matrix1D<QList<int>> subgroupsActivityTagMaxHoursDailyMaxHours;
extern Matrix1D<QList<int>> subgroupsActivityTagMaxHoursDailyActivityTag;
extern Matrix1D<QList<double>> subgroupsActivityTagMaxHoursDailyPercentage;

bool computeStudentsActivityTagMaxHoursDaily(QWidget* parent);
///////END   students (set) activity tag max hours daily


///////BEGIN students (set) activity tag max hours daily real days
extern bool haveStudentsActivityTagMaxHoursDailyRealDays;

extern Matrix1D<QList<int>> subgroupsActivityTagMaxHoursDailyRealDaysMaxHours;
extern Matrix1D<QList<int>> subgroupsActivityTagMaxHoursDailyRealDaysActivityTag;
extern Matrix1D<QList<double>> subgroupsActivityTagMaxHoursDailyRealDaysPercentage;

bool computeStudentsActivityTagMaxHoursDailyRealDays(QWidget* parent);
///////END   students (set) activity tag max hours daily real days


///////BEGIN students (set) activity tag max hours continuously
extern bool haveStudentsActivityTagMaxHoursContinuously;

extern Matrix1D<QList<int>> subgroupsActivityTagMaxHoursContinuouslyMaxHours;
extern Matrix1D<QList<int>> subgroupsActivityTagMaxHoursContinuouslyActivityTag;
extern Matrix1D<QList<double>> subgroupsActivityTagMaxHoursContinuouslyPercentage;

bool computeStudentsActivityTagMaxHoursContinuously(QWidget* parent);
///////END   students (set) activity tag max hours continuously


////////BEGIN students (set) min hours daily
extern Matrix2D<double> subgroupsMinHoursDailyPercentages; //If having mornings-afternoons: 0 is for morning, 1 is for afternoon
extern Matrix2D<int> subgroupsMinHoursDailyMinHours; //If having official: both should be equal, but we use 1.
extern Matrix1D<bool> subgroupsMinHoursDailyAllowEmptyDays;
extern Matrix1D<bool> subgroupsMinHoursPerMorningAllowEmptyMornings;

extern Matrix1D<double> subgroupsMinHoursPerAfternoonPercentages;
extern Matrix1D<int> subgroupsMinHoursPerAfternoonMinHours;
extern Matrix1D<bool> subgroupsMinHoursPerAfternoonAllowEmptyAfternoons;

bool computeSubgroupsMinHoursDaily(QWidget* parent);
////////END   students (set) min hours daily


//////////////BEGIN 2 activities consecutive
//index represents the first activity, value in array represents the second activity
extern Matrix1D<QList<double>> constrTwoActivitiesConsecutivePercentages;
extern Matrix1D<QList<int>> constrTwoActivitiesConsecutiveActivities;
void computeConstrTwoActivitiesConsecutive();

//index represents the second activity, value in array represents the first activity
extern Matrix1D<QList<double>> inverseConstrTwoActivitiesConsecutivePercentages;
extern Matrix1D<QList<int>> inverseConstrTwoActivitiesConsecutiveActivities;
//////////////END   2 activities consecutive


//////////////BEGIN 2 activities grouped
//index represents the first activity, value in array represents the second activity
extern Matrix1D<QList<double>> constrTwoActivitiesGroupedPercentages;
extern Matrix1D<QList<int>> constrTwoActivitiesGroupedActivities;
void computeConstrTwoActivitiesGrouped();


//////////////BEGIN 3 activities grouped
//index represents the first activity, value in array represents the second and third activities
extern Matrix1D<QList<double>> constrThreeActivitiesGroupedPercentages;
extern Matrix1D<QList<QPair<int, int>>> constrThreeActivitiesGroupedActivities;
void computeConstrThreeActivitiesGrouped();


//////////////BEGIN 2 activities ordered
//index represents the first activity, value in array represents the second activity
extern Matrix1D<QList<double>> constrTwoActivitiesOrderedPercentages;
extern Matrix1D<QList<int>> constrTwoActivitiesOrderedActivities;
void computeConstrTwoActivitiesOrdered();

//index represents the second activity, value in array represents the first activity
extern Matrix1D<QList<double>> inverseConstrTwoActivitiesOrderedPercentages;
extern Matrix1D<QList<int>> inverseConstrTwoActivitiesOrderedActivities;
//////////////END   2 activities ordered

//////////////BEGIN 2 activities ordered if same day
//index represents the first activity, value in array represents the second activity
extern Matrix1D<QList<double>> constrTwoActivitiesOrderedIfSameDayPercentages;
extern Matrix1D<QList<int>> constrTwoActivitiesOrderedIfSameDayActivities;
void computeConstrTwoActivitiesOrderedIfSameDay();

//index represents the second activity, value in array represents the first activity
extern Matrix1D<QList<double>> inverseConstrTwoActivitiesOrderedIfSameDayPercentages;
extern Matrix1D<QList<int>> inverseConstrTwoActivitiesOrderedIfSameDayActivities;
//////////////END   2 activities ordered if same day


////////////BEGIN activity ends students day
extern Matrix1D<double> activityEndsStudentsDayPercentages; //-1 for not existing
bool computeActivityEndsStudentsDayPercentages(QWidget* parent);
extern bool haveActivityEndsStudentsDay;
////////////END   activity ends students day

////////////BEGIN activity ends teachers day
extern Matrix1D<double> activityEndsTeachersDayPercentages; //-1 for not existing
bool computeActivityEndsTeachersDayPercentages(QWidget* parent);
extern bool haveActivityEndsTeachersDay;
////////////END   activity ends teachers day


////////////BEGIN activity begins students day
extern Matrix1D<double> activityBeginsStudentsDayPercentages; //-1 for not existing
bool computeActivityBeginsStudentsDayPercentages(QWidget* parent);
extern bool haveActivityBeginsStudentsDay;
////////////END   activity begins students day

////////////BEGIN activity begins teachers day
extern Matrix1D<double> activityBeginsTeachersDayPercentages; //-1 for not existing
bool computeActivityBeginsTeachersDayPercentages(QWidget* parent);
extern bool haveActivityBeginsTeachersDay;
////////////END   activity begins teachers day


////////////BEGIN activity begins or ends students day
extern Matrix1D<double> activityBeginsOrEndsStudentsDayPercentages; //-1 for not existing
bool computeActivityBeginsOrEndsStudentsDayPercentages(QWidget* parent);
extern bool haveActivityBeginsOrEndsStudentsDay;
////////////END   activity begins or ends students day

////////////BEGIN activity begins or ends teachers day
extern Matrix1D<double> activityBeginsOrEndsTeachersDayPercentages; //-1 for not existing
bool computeActivityBeginsOrEndsTeachersDayPercentages(QWidget* parent);
extern bool haveActivityBeginsOrEndsTeachersDay;
////////////END   activity begins or ends teachers day


bool checkMinDays100Percent(QWidget* parent);
bool checkMinDaysMaxTwoOnSameDay(QWidget* parent);

bool checkMinHalfDays100Percent(QWidget* parent);
bool checkMinHalfDaysMaxTwoOnSameDay(QWidget* parent);


bool checkMaxHoursForActivityDuration(QWidget* parent);


///////BEGIN teachers interval max days per week
extern Matrix1D<QList<double>> teachersIntervalMaxDaysPerWeekPercentages;
extern Matrix1D<QList<int>> teachersIntervalMaxDaysPerWeekMaxDays;
extern Matrix1D<QList<int>> teachersIntervalMaxDaysPerWeekIntervalStart;
extern Matrix1D<QList<int>> teachersIntervalMaxDaysPerWeekIntervalEnd;

bool computeTeachersIntervalMaxDaysPerWeek(QWidget* parent);
///////END   teachers interval max days per week

//morning
extern Matrix1D<QList<double>> teachersMorningIntervalMaxDaysPerWeekPercentages;
extern Matrix1D<QList<int>> teachersMorningIntervalMaxDaysPerWeekMaxDays;
extern Matrix1D<QList<int>> teachersMorningIntervalMaxDaysPerWeekIntervalStart;
extern Matrix1D<QList<int>> teachersMorningIntervalMaxDaysPerWeekIntervalEnd;

bool computeTeachersMorningIntervalMaxDaysPerWeek(QWidget* parent);

//afternoon
extern Matrix1D<QList<double>> teachersAfternoonIntervalMaxDaysPerWeekPercentages;
extern Matrix1D<QList<int>> teachersAfternoonIntervalMaxDaysPerWeekMaxDays;
extern Matrix1D<QList<int>> teachersAfternoonIntervalMaxDaysPerWeekIntervalStart;
extern Matrix1D<QList<int>> teachersAfternoonIntervalMaxDaysPerWeekIntervalEnd;

bool computeTeachersAfternoonIntervalMaxDaysPerWeek(QWidget* parent);


extern Matrix1D<double> teachersMaxTwoConsecutiveMorningsPercentage;
extern Matrix1D<double> teachersMaxTwoConsecutiveAfternoonsPercentage;

bool computeTeachersMaxTwoConsecutiveMornings(QWidget* parent);
bool computeTeachersMaxTwoConsecutiveAfternoons(QWidget* parent);


///////BEGIN subgroups interval max days per week
extern Matrix1D<QList<double>> subgroupsIntervalMaxDaysPerWeekPercentages;
extern Matrix1D<QList<int>> subgroupsIntervalMaxDaysPerWeekMaxDays;
extern Matrix1D<QList<int>> subgroupsIntervalMaxDaysPerWeekIntervalStart;
extern Matrix1D<QList<int>> subgroupsIntervalMaxDaysPerWeekIntervalEnd;

bool computeSubgroupsIntervalMaxDaysPerWeek(QWidget* parent);
///////END   subgroups interval max days per week


//2020-06-26
///////BEGIN subgroups morning interval max days per week
extern Matrix1D<QList<double>> subgroupsMorningIntervalMaxDaysPerWeekPercentages;
extern Matrix1D<QList<int>> subgroupsMorningIntervalMaxDaysPerWeekMaxDays;
extern Matrix1D<QList<int>> subgroupsMorningIntervalMaxDaysPerWeekIntervalStart;
extern Matrix1D<QList<int>> subgroupsMorningIntervalMaxDaysPerWeekIntervalEnd;

bool computeSubgroupsMorningIntervalMaxDaysPerWeek(QWidget* parent);
///////END   subgroups morning interval max days per week

///////BEGIN subgroups afternoon interval max days per week
extern Matrix1D<QList<double>> subgroupsAfternoonIntervalMaxDaysPerWeekPercentages;
extern Matrix1D<QList<int>> subgroupsAfternoonIntervalMaxDaysPerWeekMaxDays;
extern Matrix1D<QList<int>> subgroupsAfternoonIntervalMaxDaysPerWeekIntervalStart;
extern Matrix1D<QList<int>> subgroupsAfternoonIntervalMaxDaysPerWeekIntervalEnd;

bool computeSubgroupsAfternoonIntervalMaxDaysPerWeek(QWidget* parent);
///////END   subgroups afternoon interval max days per week


//2017-02-06
extern Matrix1D<int> teachersMaxSpanPerDayMaxSpan; //-1 for not existing
extern Matrix1D<double> teachersMaxSpanPerDayPercentages; //-1 for not existing
extern Matrix1D<bool> teachersMaxSpanPerDayAllowOneDayExceptionPlusOne;


///////////////
extern Matrix1D<int> teachersMaxSpanPerRealDayMaxSpan; //-1 for not existing
extern Matrix1D<double> teachersMaxSpanPerRealDayPercentages; //-1 for not existing
extern Matrix1D<bool> teachersMaxSpanPerRealDayAllowOneDayExceptionPlusOne;

extern Matrix1D<int> subgroupsMaxSpanPerRealDayMaxSpan; //-1 for not existing
extern Matrix1D<double> subgroupsMaxSpanPerRealDayPercentages; //-1 for not existing

bool computeTeachersMaxSpanPerRealDay(QWidget* parent);
bool computeSubgroupsMaxSpanPerRealDay(QWidget* parent);
///////////////


extern Matrix1D<int> teachersMinRestingHoursCircularMinHours; //-1 for not existing
extern Matrix1D<double> teachersMinRestingHoursCircularPercentages; //-1 for not existing
extern Matrix1D<int> teachersMinRestingHoursNotCircularMinHours; //-1 for not existing
extern Matrix1D<double> teachersMinRestingHoursNotCircularPercentages; //-1 for not existing

extern Matrix1D<int> subgroupsMaxSpanPerDayMaxSpan; //-1 for not existing
extern Matrix1D<double> subgroupsMaxSpanPerDayPercentages; //-1 for not existing

extern Matrix1D<int> subgroupsMinRestingHoursCircularMinHours; //-1 for not existing
extern Matrix1D<double> subgroupsMinRestingHoursCircularPercentages; //-1 for not existing
extern Matrix1D<int> subgroupsMinRestingHoursNotCircularMinHours; //-1 for not existing
extern Matrix1D<double> subgroupsMinRestingHoursNotCircularPercentages; //-1 for not existing

bool computeTeachersMaxSpanPerDay(QWidget* parent);
bool computeTeachersMinRestingHours(QWidget* parent);
bool computeSubgroupsMaxSpanPerDay(QWidget* parent);
bool computeSubgroupsMinRestingHours(QWidget* parent);
////////////


//2020-07-20
///////////////
extern Matrix1D<int> teachersMinRestingHoursBetweenMorningAndAfternoonMinHours; //-1 for not existing
extern Matrix1D<double> teachersMinRestingHoursBetweenMorningAndAfternoonPercentages; //-1 for not existing

extern Matrix1D<int> subgroupsMinRestingHoursBetweenMorningAndAfternoonMinHours; //-1 for not existing
extern Matrix1D<double> subgroupsMinRestingHoursBetweenMorningAndAfternoonPercentages; //-1 for not existing
bool computeTeachersMinRestingHoursBetweenMorningAndAfternoon(QWidget* parent);
bool computeSubgroupsMinRestingHoursBetweenMorningAndAfternoon(QWidget* parent);
///////////////

/////////
extern Matrix1D<QList<double>> subgroupsMaxActivityTagsPerDayFromSetPercentages;
extern Matrix1D<QList<int>> subgroupsMaxActivityTagsPerDayFromSetMaxTags;
extern Matrix1D<QList<QSet<int>>> subgroupsMaxActivityTagsPerDayFromSetTagsSet;
extern bool haveStudentsMaxActivityTagsPerDayFromSet;
extern Matrix1D<QList<std::tuple<int, int, int>>> subgroupsMaxActivityTagsPerDayForActivity;

extern Matrix1D<QList<double>> subgroupsMaxActivityTagsPerRealDayFromSetPercentages;
extern Matrix1D<QList<int>> subgroupsMaxActivityTagsPerRealDayFromSetMaxTags;
extern Matrix1D<QList<QSet<int>>> subgroupsMaxActivityTagsPerRealDayFromSetTagsSet;
extern bool haveStudentsMaxActivityTagsPerRealDayFromSet;
extern Matrix1D<QList<std::tuple<int, int, int>>> subgroupsMaxActivityTagsPerRealDayForActivity;
/////////
extern Matrix1D<QList<double>> teachersMaxActivityTagsPerDayFromSetPercentages;
extern Matrix1D<QList<int>> teachersMaxActivityTagsPerDayFromSetMaxTags;
extern Matrix1D<QList<QSet<int>>> teachersMaxActivityTagsPerDayFromSetTagsSet;
extern bool haveTeachersMaxActivityTagsPerDayFromSet;
extern Matrix1D<QList<std::tuple<int, int, int>>> teachersMaxActivityTagsPerDayForActivity;

extern Matrix1D<QList<double>> teachersMaxActivityTagsPerRealDayFromSetPercentages;
extern Matrix1D<QList<int>> teachersMaxActivityTagsPerRealDayFromSetMaxTags;
extern Matrix1D<QList<QSet<int>>> teachersMaxActivityTagsPerRealDayFromSetTagsSet;
extern bool haveTeachersMaxActivityTagsPerRealDayFromSet;
extern Matrix1D<QList<std::tuple<int, int, int>>> teachersMaxActivityTagsPerRealDayForActivity;

extern Matrix1D<QList<int>> teachersWithTagsForActivities;
extern Matrix1D<QList<int>> subgroupsWithTagsForActivities;

bool computeTeachersStudentsMaxActivityTagsFromSet(QWidget* parent);
/////////

////////BEGIN rooms
bool computeBasicSpace(QWidget* parent);

extern Matrix2D<double> notAllowedRoomTimePercentages; //-1 for available
bool computeNotAllowedRoomTimePercentages();

extern Matrix1D<QList<PreferredRoomsItem>> activitiesPreferredRoomsList;
extern Matrix1D<bool> unspecifiedPreferredRoom;

extern Matrix1D<QList<int>> activitiesHomeRoomsHomeRooms;
extern Matrix1D<double> activitiesHomeRoomsPercentage;
extern Matrix1D<bool> unspecifiedHomeRoom;

extern Matrix1D<QSet<int>> preferredRealRooms;

bool computeActivitiesRoomsPreferences(QWidget* parent);

//2024-09-15
extern Matrix1D<QList<double>> roomsMaxActivityTagsPerDayFromSetPercentages;
extern Matrix1D<QList<int>> roomsMaxActivityTagsPerDayFromSetMaxTags;
extern Matrix1D<QList<QSet<int>>> roomsMaxActivityTagsPerDayFromSetTagsSet;
extern bool haveRoomsMaxActivityTagsPerDayFromSet;

extern Matrix1D<QList<double>> roomsMaxActivityTagsPerRealDayFromSetPercentages;
extern Matrix1D<QList<int>> roomsMaxActivityTagsPerRealDayFromSetMaxTags;
extern Matrix1D<QList<QSet<int>>> roomsMaxActivityTagsPerRealDayFromSetTagsSet;
extern bool haveRoomsMaxActivityTagsPerRealDayFromSet;

//2024-09-23
extern Matrix1D<QList<double>> roomsMaxActivityTagsPerWeekFromSetPercentages;
extern Matrix1D<QList<int>> roomsMaxActivityTagsPerWeekFromSetMaxTags;
extern Matrix1D<QList<QSet<int>>> roomsMaxActivityTagsPerWeekFromSetTagsSet;
extern bool haveRoomsMaxActivityTagsPerWeekFromSet;

bool computeRoomsMaxActivityTagsFromSet(QWidget* parent);
////////END   rooms


////////BEGIN building changes
extern Matrix1D<double> maxBuildingChangesPerWeekForStudentsPercentages;
extern Matrix1D<int> maxBuildingChangesPerWeekForStudentsMaxChanges;
bool computeMaxBuildingChangesPerWeekForStudents(QWidget* parent);

extern Matrix1D<double> maxBuildingChangesPerDayForStudentsPercentages;
extern Matrix1D<int> maxBuildingChangesPerDayForStudentsMaxChanges;
bool computeMaxBuildingChangesPerDayForStudents(QWidget* parent);

extern Matrix1D<QList<double>> maxBuildingChangesPerDayInIntervalForStudentsPercentages;
extern Matrix1D<QList<int>> maxBuildingChangesPerDayInIntervalForStudentsMaxChanges;
extern Matrix1D<QList<int>> maxBuildingChangesPerDayInIntervalForStudentsIntervalStart;
extern Matrix1D<QList<int>> maxBuildingChangesPerDayInIntervalForStudentsIntervalEnd;
bool computeMaxBuildingChangesPerDayInIntervalForStudents(QWidget* parent);

extern Matrix1D<double> minGapsBetweenBuildingChangesForStudentsPercentages;
extern Matrix1D<int> minGapsBetweenBuildingChangesForStudentsMinGaps;
bool computeMinGapsBetweenBuildingChangesForStudents(QWidget* parent);

extern Matrix1D<double> maxBuildingChangesPerWeekForTeachersPercentages;
extern Matrix1D<int> maxBuildingChangesPerWeekForTeachersMaxChanges;
bool computeMaxBuildingChangesPerWeekForTeachers(QWidget* parent);

extern Matrix1D<double> maxBuildingChangesPerDayForTeachersPercentages;
extern Matrix1D<int> maxBuildingChangesPerDayForTeachersMaxChanges;
bool computeMaxBuildingChangesPerDayForTeachers(QWidget* parent);

extern Matrix1D<QList<double>> maxBuildingChangesPerDayInIntervalForTeachersPercentages;
extern Matrix1D<QList<int>> maxBuildingChangesPerDayInIntervalForTeachersMaxChanges;
extern Matrix1D<QList<int>> maxBuildingChangesPerDayInIntervalForTeachersIntervalStart;
extern Matrix1D<QList<int>> maxBuildingChangesPerDayInIntervalForTeachersIntervalEnd;
bool computeMaxBuildingChangesPerDayInIntervalForTeachers(QWidget* parent);

extern Matrix1D<double> minGapsBetweenBuildingChangesForTeachersPercentages;
extern Matrix1D<int> minGapsBetweenBuildingChangesForTeachersMinGaps;
bool computeMinGapsBetweenBuildingChangesForTeachers(QWidget* parent);

//BEGIN building changes per real day for teachers
extern Matrix1D<double> maxBuildingChangesPerRealDayForTeachersPercentages;
extern Matrix1D<int> maxBuildingChangesPerRealDayForTeachersMaxChanges;
bool computeMaxBuildingChangesPerRealDayForTeachers(QWidget* parent);
//END   building changes per real day for teachers

extern Matrix1D<QList<double>> maxBuildingChangesPerRealDayInIntervalForTeachersPercentages;
extern Matrix1D<QList<int>> maxBuildingChangesPerRealDayInIntervalForTeachersMaxChanges;
extern Matrix1D<QList<int>> maxBuildingChangesPerRealDayInIntervalForTeachersIntervalStart;
extern Matrix1D<QList<int>> maxBuildingChangesPerRealDayInIntervalForTeachersIntervalEnd;
bool computeMaxBuildingChangesPerRealDayInIntervalForTeachers(QWidget* parent);

//BEGIN building changes per real day for students
extern Matrix1D<double> maxBuildingChangesPerRealDayForSubgroupsPercentages;
extern Matrix1D<int> maxBuildingChangesPerRealDayForSubgroupsMaxChanges;
bool computeMaxBuildingChangesPerRealDayForStudents(QWidget* parent);
//END   building changes per real day for students

extern Matrix1D<QList<double>> maxBuildingChangesPerRealDayInIntervalForStudentsPercentages;
extern Matrix1D<QList<int>> maxBuildingChangesPerRealDayInIntervalForStudentsMaxChanges;
extern Matrix1D<QList<int>> maxBuildingChangesPerRealDayInIntervalForStudentsIntervalStart;
extern Matrix1D<QList<int>> maxBuildingChangesPerRealDayInIntervalForStudentsIntervalEnd;
bool computeMaxBuildingChangesPerRealDayInIntervalForStudents(QWidget* parent);

////////END   building changes


////////BEGIN room changes
extern Matrix1D<double> maxRoomChangesPerWeekForStudentsPercentages;
extern Matrix1D<int> maxRoomChangesPerWeekForStudentsMaxChanges;
bool computeMaxRoomChangesPerWeekForStudents(QWidget* parent);

extern Matrix1D<double> maxRoomChangesPerDayForStudentsPercentages;
extern Matrix1D<int> maxRoomChangesPerDayForStudentsMaxChanges;
bool computeMaxRoomChangesPerDayForStudents(QWidget* parent);

extern Matrix1D<double> minGapsBetweenRoomChangesForStudentsPercentages;
extern Matrix1D<int> minGapsBetweenRoomChangesForStudentsMinGaps;
bool computeMinGapsBetweenRoomChangesForStudents(QWidget* parent);

extern Matrix1D<double> maxRoomChangesPerWeekForTeachersPercentages;
extern Matrix1D<int> maxRoomChangesPerWeekForTeachersMaxChanges;
bool computeMaxRoomChangesPerWeekForTeachers(QWidget* parent);

extern Matrix1D<double> maxRoomChangesPerDayForTeachersPercentages;
extern Matrix1D<int> maxRoomChangesPerDayForTeachersMaxChanges;
bool computeMaxRoomChangesPerDayForTeachers(QWidget* parent);

extern Matrix1D<double> minGapsBetweenRoomChangesForTeachersPercentages;
extern Matrix1D<int> minGapsBetweenRoomChangesForTeachersMinGaps;
bool computeMinGapsBetweenRoomChangesForTeachers(QWidget* parent);


//BEGIN room changes per real day for teachers
extern Matrix1D<double> maxRoomChangesPerRealDayForTeachersPercentages;
extern Matrix1D<int> maxRoomChangesPerRealDayForTeachersMaxChanges;
bool computeMaxRoomChangesPerRealDayForTeachers(QWidget* parent);
//END   room changes per real day for teachers

//BEGIN room changes per real day for students
extern Matrix1D<double> maxRoomChangesPerRealDayForSubgroupsPercentages;
extern Matrix1D<int> maxRoomChangesPerRealDayForSubgroupsMaxChanges;
bool computeMaxRoomChangesPerRealDayForStudents(QWidget* parent);
//END   room changes per real day for students

extern Matrix1D<QList<double>> maxRoomChangesPerDayInIntervalForStudentsPercentages;
extern Matrix1D<QList<int>> maxRoomChangesPerDayInIntervalForStudentsMaxChanges;
extern Matrix1D<QList<int>> maxRoomChangesPerDayInIntervalForStudentsIntervalStart;
extern Matrix1D<QList<int>> maxRoomChangesPerDayInIntervalForStudentsIntervalEnd;
bool computeMaxRoomChangesPerDayInIntervalForStudents(QWidget* parent);

extern Matrix1D<QList<double>> maxRoomChangesPerDayInIntervalForTeachersPercentages;
extern Matrix1D<QList<int>> maxRoomChangesPerDayInIntervalForTeachersMaxChanges;
extern Matrix1D<QList<int>> maxRoomChangesPerDayInIntervalForTeachersIntervalStart;
extern Matrix1D<QList<int>> maxRoomChangesPerDayInIntervalForTeachersIntervalEnd;
bool computeMaxRoomChangesPerDayInIntervalForTeachers(QWidget* parent);

extern Matrix1D<QList<double>> maxRoomChangesPerRealDayInIntervalForTeachersPercentages;
extern Matrix1D<QList<int>> maxRoomChangesPerRealDayInIntervalForTeachersMaxChanges;
extern Matrix1D<QList<int>> maxRoomChangesPerRealDayInIntervalForTeachersIntervalStart;
extern Matrix1D<QList<int>> maxRoomChangesPerRealDayInIntervalForTeachersIntervalEnd;
bool computeMaxRoomChangesPerRealDayInIntervalForTeachers(QWidget* parent);

extern Matrix1D<QList<double>> maxRoomChangesPerRealDayInIntervalForStudentsPercentages;
extern Matrix1D<QList<int>> maxRoomChangesPerRealDayInIntervalForStudentsMaxChanges;
extern Matrix1D<QList<int>> maxRoomChangesPerRealDayInIntervalForStudentsIntervalStart;
extern Matrix1D<QList<int>> maxRoomChangesPerRealDayInIntervalForStudentsIntervalEnd;
bool computeMaxRoomChangesPerRealDayInIntervalForStudents(QWidget* parent);

////////END   room changes


extern Matrix1D<QList<int>> mustComputeTimetableSubgroups;
extern Matrix1D<QList<int>> mustComputeTimetableTeachers;
extern Matrix1D<bool> mustComputeTimetableSubgroup;
extern Matrix1D<bool> mustComputeTimetableTeacher;

void computeMustComputeTimetableSubgroups();
void computeMustComputeTimetableTeachers();


bool homeRoomsAreOk(QWidget* parent);


//2025-04-02
class TwoSetsOfActivitiesSameSections_item
{
public:
	//double weight; -> must be 100.0%
	QList<int> activitiesAList;
	QHash<int, int> activitiesAHash;

	QList<int> activitiesBList;
	QHash<int, int> activitiesBHash;

	//not really needed
	/*QList<int> aTimeSlotsList;
	QSet<int> aTimeSlotsSet;

	QList<int> bTimeSlotsList;
	QSet<int> bTimeSlotsSet;*/

	//QList<int> oTimeSlotsList;
	QSet<int> oTimeSlotsSet; //exception slots
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<TwoSetsOfActivitiesSameSections_item> assabList;
extern Matrix1D<QList<TwoSetsOfActivitiesSameSections_item*>> assabListForActivity;

bool computeTwoSetsOfActivitiesSameSections(QWidget* parent);


//2011-09-25 - Constraint activities occupy max time slots from selection
class ActivitiesOccupyMaxTimeSlotsFromSelection_item
{
public:
	//double weight; -> must be 100.0%
	QList<int> activitiesList;
	QSet<int> activitiesSet;
	QList<int> selectedTimeSlotsList;
	QSet<int> selectedTimeSlotsSet;
	int maxOccupiedTimeSlots;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<ActivitiesOccupyMaxTimeSlotsFromSelection_item> aomtsList;
extern Matrix1D<QList<ActivitiesOccupyMaxTimeSlotsFromSelection_item*>> aomtsListForActivity;

bool computeActivitiesOccupyMaxTimeSlotsFromSelection(QWidget* parent);

//2019-11-16 - Constraint activities occupy min time slots from selection
class ActivitiesOccupyMinTimeSlotsFromSelection_item
{
public:
	//double weight; -> must be 100.0%
	QList<int> activitiesList;
	QSet<int> activitiesSet;
	QList<int> selectedTimeSlotsList;
	QSet<int> selectedTimeSlotsSet;
	int minOccupiedTimeSlots;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<ActivitiesOccupyMinTimeSlotsFromSelection_item> aomintsList;
extern Matrix1D<QList<ActivitiesOccupyMinTimeSlotsFromSelection_item*>> aomintsListForActivity;

bool computeActivitiesOccupyMinTimeSlotsFromSelection(QWidget* parent);

//2011-09-30 - Constraint activities max simultaneous in selected time slots
class ActivitiesMaxSimultaneousInSelectedTimeSlots_item
{
public:
	//double weight; -> must be 100.0%
	QList<int> activitiesList;
	QSet<int> activitiesSet;
	QList<int> selectedTimeSlotsList;
	QSet<int> selectedTimeSlotsSet;
	int maxSimultaneous;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<ActivitiesMaxSimultaneousInSelectedTimeSlots_item> amsistsList;
extern Matrix1D<QList<ActivitiesMaxSimultaneousInSelectedTimeSlots_item*>> amsistsListForActivity;

bool computeActivitiesMaxSimultaneousInSelectedTimeSlots(QWidget* parent);

//2019-11-16 - Constraint activities min simultaneous in selected time slots
class ActivitiesMinSimultaneousInSelectedTimeSlots_item
{
public:
	//double weight; -> must be 100.0%
	QList<int> activitiesList;
	QSet<int> activitiesSet;
	QList<int> selectedTimeSlotsList;
	QSet<int> selectedTimeSlotsSet;
	int minSimultaneous;

	bool allowEmptySlots;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<ActivitiesMinSimultaneousInSelectedTimeSlots_item> aminsistsList;
extern Matrix1D<QList<ActivitiesMinSimultaneousInSelectedTimeSlots_item*>> aminsistsListForActivity;

bool computeActivitiesMinSimultaneousInSelectedTimeSlots(QWidget* parent);


//2025-10-18 - Constraint activities max total number of students in selected time slots
class ActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots_item
{
public:
	//double weight; -> must be 100.0%
	QList<int> activitiesList;
	QSet<int> activitiesSet;
	QList<int> selectedTimeSlotsList;
	QSet<int> selectedTimeSlotsSet;
	int maxNumberOfStudents;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<ActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots_item> amtnosistsList;
extern Matrix1D<QList<ActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots_item*>> amtnosistsListForActivity;

bool computeActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots(QWidget* parent);


extern bool haveTwoSetsOfActivitiesSameSections;
extern Matrix1D<bool> activityHasTwoSetsOfActivitiesSameSections;

extern bool haveActivitiesOccupyMaxConstraints;
extern Matrix1D<bool> activityHasOccupyMaxConstraints;

extern bool haveActivitiesMaxSimultaneousConstraints;
extern Matrix1D<bool> activityHasMaxSimultaneousConstraints;

extern bool haveActivitiesMaxNumberOfStudentsConstraints;
extern Matrix1D<bool> activityHasMaxNumberOfStudentsConstraints;

//2020-05-01 - Constraint max total activities from set in selected time slots
class ActivitiesMaxTotalFromSetInSelectedTimeSlots_item
{
public:
	//double weight; -> must be 100.0%
	QList<int> activitiesList;
	//QSet<int> activitiesSet;
	//QList<int> selectedTimeSlotsList;
	QSet<int> selectedTimeSlotsSet;
	int maxActivities;
};

//The next variable should be a std::list!!!
//We need the references to the elements to be valid
extern std::list<ActivitiesMaxTotalFromSetInSelectedTimeSlots_item> amtfsistsList;
extern Matrix1D<QList<ActivitiesMaxTotalFromSetInSelectedTimeSlots_item*>> amtfsistsListForActivity;

bool computeActivitiesMaxTotalFromSetInSelectedTimeSlots(QWidget* parent);

//for terms
//2020-01-14 - Constraint activities max in a term

class ActivitiesMaxInATerm_item
{
public:
	//double weight; -> must be 100.0%
	QList<int> activitiesList;
	int maxActivitiesInATerm;
};

//The next variable should be a std::list!!!
//We need the references to the elements to be valid
extern std::list<ActivitiesMaxInATerm_item> amiatList;
extern Matrix1D<QList<ActivitiesMaxInATerm_item*>> amiatListForActivity;

bool computeActivitiesMaxInATerm(QWidget* parent);

//2022-05-19 - Constraint activities min in a term

class ActivitiesMinInATerm_item
{
public:
	//double weight; -> must be 100.0%
	QList<int> activitiesList;
	int minActivitiesInATerm;
	bool allowEmptyTerms;
};

//The next variable should be a std::list!!!
//We need the references to the elements to be valid
extern std::list<ActivitiesMinInATerm_item> aminiatList;
extern Matrix1D<QList<ActivitiesMinInATerm_item*>> aminiatListForActivity;

bool computeActivitiesMinInATerm(QWidget* parent);

//2020-01-14 - Constraint activities occupy max terms

class ActivitiesOccupyMaxTerms_item
{
public:
	//double weight; -> must be 100.0%
	QList<int> activitiesList;
	int maxOccupiedTerms;
};

//The next variable should be a std::list!!!
//We need the references to the elements to be valid
extern std::list<ActivitiesOccupyMaxTerms_item> aomtList;
extern Matrix1D<QList<ActivitiesOccupyMaxTerms_item*>> aomtListForActivity;

bool computeActivitiesOccupyMaxTerms(QWidget* parent);

//2019-06-08 - Constraint students (set) min gaps between ordered pair of activity tags
class StudentsMinGapsBetweenOrderedPairOfActivityTags_item{
public:
	//double weight; -> must be 100.0%
	QSet<int> canonicalSetOfSubgroups;
	int minGaps;
	int firstActivityTag;
	int secondActivityTag;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<StudentsMinGapsBetweenOrderedPairOfActivityTags_item> smgbopoatList;
extern Matrix1D<QList<StudentsMinGapsBetweenOrderedPairOfActivityTags_item*>> smgbopoatListForActivity;

bool computeStudentsMinGapsBetweenOrderedPairOfActivityTags(QWidget* parent);

//2019-06-08 - Constraint teacher(s) min gaps between ordered pair of activity tags
class TeachersMinGapsBetweenOrderedPairOfActivityTags_item{
public:
	//double weight; -> must be 100.0%
	QSet<int> canonicalSetOfTeachers;
	int minGaps;
	int firstActivityTag;
	int secondActivityTag;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<TeachersMinGapsBetweenOrderedPairOfActivityTags_item> tmgbopoatList;
extern Matrix1D<QList<TeachersMinGapsBetweenOrderedPairOfActivityTags_item*>> tmgbopoatListForActivity;

bool computeTeachersMinGapsBetweenOrderedPairOfActivityTags(QWidget* parent);

//2021-12-15 - Constraint students (set) min gaps between activity tag
class StudentsMinGapsBetweenActivityTag_item{
public:
	//double weight; -> must be 100.0%
	QSet<int> canonicalSetOfSubgroups;
	int minGaps;
	int activityTag;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<StudentsMinGapsBetweenActivityTag_item> smgbatList;
extern Matrix1D<QList<StudentsMinGapsBetweenActivityTag_item*>> smgbatListForActivity;

bool computeStudentsMinGapsBetweenActivityTag(QWidget* parent);

//2021-12-15 - Constraint teacher(s) min gaps between activity tag
class TeachersMinGapsBetweenActivityTag_item{
public:
	//double weight; -> must be 100.0%
	QSet<int> canonicalSetOfTeachers;
	int minGaps;
	int activityTag;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<TeachersMinGapsBetweenActivityTag_item> tmgbatList;
extern Matrix1D<QList<TeachersMinGapsBetweenActivityTag_item*>> tmgbatListForActivity;

bool computeTeachersMinGapsBetweenActivityTag(QWidget* parent);


//2024-03-15 - Constraint students (set) min gaps between ordered pair of activity tags per real day
class StudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay_item{
public:
	//double weight; -> must be 100.0%
	QSet<int> canonicalSetOfSubgroups;
	int minGaps;
	int firstActivityTag;
	int secondActivityTag;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<StudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay_item> smgbopoatprdList;
extern Matrix1D<QList<StudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay_item*>> smgbopoatprdListForActivity;

bool computeStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay(QWidget* parent);

//2024-03-15 - Constraint teacher(s) min gaps between ordered pair of activity tags per real day
class TeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay_item{
public:
	//double weight; -> must be 100.0%
	QSet<int> canonicalSetOfTeachers;
	int minGaps;
	int firstActivityTag;
	int secondActivityTag;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<TeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay_item> tmgbopoatprdList;
extern Matrix1D<QList<TeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay_item*>> tmgbopoatprdListForActivity;

bool computeTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay(QWidget* parent);

//2024-03-15 - Constraint students (set) min gaps between activity tag per real day
class StudentsMinGapsBetweenActivityTagPerRealDay_item{
public:
	//double weight; -> must be 100.0%
	QSet<int> canonicalSetOfSubgroups;
	int minGaps;
	int activityTag;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<StudentsMinGapsBetweenActivityTagPerRealDay_item> smgbatprdList;
extern Matrix1D<QList<StudentsMinGapsBetweenActivityTagPerRealDay_item*>> smgbatprdListForActivity;

bool computeStudentsMinGapsBetweenActivityTagPerRealDay(QWidget* parent);

//2024-03-15 - Constraint teacher(s) min gaps between activity tag per real day
class TeachersMinGapsBetweenActivityTagPerRealDay_item{
public:
	//double weight; -> must be 100.0%
	QSet<int> canonicalSetOfTeachers;
	int minGaps;
	int activityTag;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<TeachersMinGapsBetweenActivityTagPerRealDay_item> tmgbatprdList;
extern Matrix1D<QList<TeachersMinGapsBetweenActivityTagPerRealDay_item*>> tmgbatprdListForActivity;

bool computeTeachersMinGapsBetweenActivityTagPerRealDay(QWidget* parent);


//2024-05-18 - Constraint students (set) min gaps between ordered pair of activity tags between morning and afternoon
class StudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon_item{
public:
	//double weight; -> must be 100.0%
	QSet<int> canonicalSetOfSubgroups;
	int minGaps;
	int firstActivityTag;
	int secondActivityTag;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<StudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon_item> smgbopoatbmaaList;
extern Matrix1D<QList<StudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon_item*>> smgbopoatbmaaListForActivity;

bool computeStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(QWidget* parent);

//2024-05-18 - Constraint teacher(s) min gaps between ordered pair of activity tags between morning and afternoon
class TeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon_item{
public:
	//double weight; -> must be 100.0%
	QSet<int> canonicalSetOfTeachers;
	int minGaps;
	int firstActivityTag;
	int secondActivityTag;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<TeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon_item> tmgbopoatbmaaList;
extern Matrix1D<QList<TeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon_item*>> tmgbopoatbmaaListForActivity;

bool computeTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(QWidget* parent);

//2024-05-18 - Constraint students (set) min gaps between activity tag between morning and afternoon
class StudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon_item{
public:
	//double weight; -> must be 100.0%
	QSet<int> canonicalSetOfSubgroups;
	int minGaps;
	int activityTag;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<StudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon_item> smgbatbmaaList;
extern Matrix1D<QList<StudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon_item*>> smgbatbmaaListForActivity;

bool computeStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon(QWidget* parent);

//2024-05-18 - Constraint teacher(s) min gaps between activity tag between morning and afternoon
class TeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon_item{
public:
	//double weight; -> must be 100.0%
	QSet<int> canonicalSetOfTeachers;
	int minGaps;
	int activityTag;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<TeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon_item> tmgbatbmaaList;
extern Matrix1D<QList<TeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon_item*>> tmgbatbmaaListForActivity;

bool computeTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon(QWidget* parent);


//2012-04-29 - Constraint activities occupy max different rooms
class ActivitiesOccupyMaxDifferentRooms_item
{
public:
	//double weight; -> must be 100.0%
	QList<int> activitiesList;
	QSet<int> activitiesSet;
	int maxDifferentRooms;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<ActivitiesOccupyMaxDifferentRooms_item> aomdrList;
extern Matrix1D<QList<ActivitiesOccupyMaxDifferentRooms_item*>> aomdrListForActivity;

bool computeActivitiesOccupyMaxDifferentRooms(QWidget* parent);

//2013-09-14 - Constraint activities same room if consecutive
class ActivitiesSameRoomIfConsecutive_item
{
public:
	//double weight; -> must be 100.0%
	QList<int> activitiesList;
	QSet<int> activitiesSet;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<ActivitiesSameRoomIfConsecutive_item> asricList;
extern Matrix1D<QList<ActivitiesSameRoomIfConsecutive_item*>> asricListForActivity;

bool computeActivitiesSameRoomIfConsecutive(QWidget* parent);

/////////////////////////////////////////////////////////////////////////

//2025-05-15
class SubgroupMaxSingleGapsInSelectedTimeSlots_item
{
public:
	int maxSingleGaps;
	QList<int> selectedTimeSlotsList;
	QSet<int> selectedTimeSlotsSet;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<SubgroupMaxSingleGapsInSelectedTimeSlots_item> smsgistsList;
extern Matrix1D<QList<SubgroupMaxSingleGapsInSelectedTimeSlots_item*>> smsgistsListForSubgroup;
extern bool haveStudentsMaxSingleGapsInSelectedTimeSlots;

extern Matrix1D<QList<int>> activitiesWithDuration1ForSubgroup;

bool computeStudentsMaxSingleGapsInSelectedTimeSlots(QWidget* parent);

/////////////////////////////////////////////////////////////////////////

//2025-05-17
class TeacherMaxSingleGapsInSelectedTimeSlots_item
{
public:
	int maxSingleGaps;
	QList<int> selectedTimeSlotsList;
	QSet<int> selectedTimeSlotsSet;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<TeacherMaxSingleGapsInSelectedTimeSlots_item> tmsgistsList;
extern Matrix1D<QList<TeacherMaxSingleGapsInSelectedTimeSlots_item*>> tmsgistsListForTeacher;
extern bool haveTeachersMaxSingleGapsInSelectedTimeSlots;

extern Matrix1D<QList<int>> activitiesWithDuration1ForTeacher;

bool computeTeachersMaxSingleGapsInSelectedTimeSlots(QWidget* parent);

/////////////////////////////////////////////////////////////////////////

//2019-11-20
class SubgroupActivityTagMinHoursDaily_item
{
public:
	int activityTag;
	int minHoursDaily;
	int minDaysWithTag;
	int durationOfActivitiesWithActivityTagForSubgroup;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<SubgroupActivityTagMinHoursDaily_item> satmhdList;
extern Matrix1D<QList<SubgroupActivityTagMinHoursDaily_item*>> satmhdListForSubgroup;
extern bool haveStudentsActivityTagMinHoursDaily;

bool computeStudentsActivityTagMinHoursDaily(QWidget* parent);

//2019-11-20
class TeacherActivityTagMinHoursDaily_item
{
public:
	int activityTag;
	int minHoursDaily;
	int minDaysWithTag;
	int durationOfActivitiesWithActivityTagForTeacher;
};

//We need the references to the elements to be valid, so we need this to be a std::list
extern std::list<TeacherActivityTagMinHoursDaily_item> tatmhdList;
extern Matrix1D<QList<TeacherActivityTagMinHoursDaily_item*>> tatmhdListForTeacher;
extern bool haveTeachersActivityTagMinHoursDaily;

bool computeTeachersActivityTagMinHoursDaily(QWidget* parent);

//2022-02-16 - speed improvements in the Mornings-Afternoons mode
extern Matrix1D<QList<int>> subgroupsForActivitiesOfTheDay;
extern Matrix1D<QList<int>> subgroupsForActivitiesOfTheDayMornings;
extern Matrix1D<QList<int>> subgroupsForActivitiesOfTheDayAfternoons;
////
extern Matrix1D<QList<int>> teachersForActivitiesOfTheDay;
extern Matrix1D<QList<int>> teachersForActivitiesOfTheDayMornings;
extern Matrix1D<QList<int>> teachersForActivitiesOfTheDayAfternoons;
////
void computeSubgroupsTeachersForActivitiesOfTheDay();

/////////////////////////////////////////////////////////////////////////

extern Matrix1D<bool> fixedTimeActivity;
extern Matrix1D<bool> fixedSpaceActivity;
bool computeFixedActivities(QWidget* parent);

class GeneratePreTranslate: public QObject{
	Q_OBJECT
};

#endif
