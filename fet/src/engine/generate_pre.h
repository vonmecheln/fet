/*
File generate_pre.h
*/

/*
Copyright 2007 Lalescu Liviu.

This file is part of FET.

FET is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

FET is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with FET; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef GENERATE_PRE_H
#define GENERATE_PRE_H

#include "timetable_defs.h"
#include "timetable.h"

#include <QSet>

extern int permutation[MAX_ACTIVITIES]; //the permutation matrix to obtain activities in
//decreasing difficulty order

bool processTimeConstraints();


////////BEGIN BASIC TIME CONSTRAINTS
//extern bool activitiesConflicting[MAX_ACTIVITIES][MAX_ACTIVITIES];
extern qint8 activitiesConflictingPercentage[MAX_ACTIVITIES][MAX_ACTIVITIES]; //-1 for not conflicting,
//a value >=0 equal with the weight of the maximum weightPercentage of a basic time constraint

bool computeActivitiesConflictingPercentage();
//void computeActivitiesConflicting();

void sortActivities();
////////END   BASIC TIME CONSTRAINTS


////////BEGIN MIN N DAYS TIME CONSTRAINTS
extern QList<int> minNDaysListOfActivities[MAX_ACTIVITIES];
extern QList<int> minNDaysListOfMinDays[MAX_ACTIVITIES];
extern QList<double> minNDaysListOfWeightPercentages[MAX_ACTIVITIES];
extern QList<bool> minNDaysListOfConsecutiveIfSameDay[MAX_ACTIVITIES];

bool computeMinNDays();
////////END   MIN N DAYS TIME CONSTRAINTS


////////BEGIN MIN GAPS between activities TIME CONSTRAINTS
extern QList<int> minGapsBetweenActivitiesListOfActivities[MAX_ACTIVITIES];
extern QList<int> minGapsBetweenActivitiesListOfMinGaps[MAX_ACTIVITIES];
extern QList<double> minGapsBetweenActivitiesListOfWeightPercentages[MAX_ACTIVITIES];

bool computeMinGapsBetweenActivities();
////////END	 MIN GAPS between activities TIME CONSTRAINTS


////////BEGIN st. not available, tch not avail., break, activity preferred time,
////////activity preferred times, activities preferred times
//percentage of allowed time, -1 if no restriction
extern double notAllowedTimesPercentages[MAX_ACTIVITIES][MAX_HOURS_PER_WEEK];

//break, which is not considered gap, false means no break, true means 100% break
//break can only be 100% or none
extern bool breakDayHour[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

//students set not available, which is not considered gap, false means available, true means 100% not available
//students set not available can only be 100% or none
//extern bool subgroupNotAvailableTime[MAX_TOTAL_SUBGROUPS][MAX_HOURS_PER_WEEK];
extern bool subgroupNotAvailableDayHour[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
//#define subgroupNotAvailableTime(s,i)	(subgroupNotAvailableDayHour[(s)][((i)%gt.rules.nDaysPerWeek)][((i)/gt.rules.nDaysPerWeek)])

//teacher not available, which is not considered gap, false means available, true means 100% not available
//teacher not available can only be 100% or none
//extern bool teacherNotAvailableTime[MAX_TEACHERS][MAX_HOURS_PER_WEEK];
extern bool teacherNotAvailableDayHour[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
//#define teacherNotAvailableTime(t,i)	(teacherNotAvailableDayHour[(t)][((i)%gt.rules.nDaysPerWeek)][((i)/gt.rules.nDaysPerWeek)])

bool computeNotAllowedTimesPercentages();
////////END   st. not available, tch not avail., break, activity preferred time,
////////activity preferred time, activities preferred times


////////BEGIN students no gaps and early
bool computeNHoursPerSubgroup();
bool computeSubgroupsEarlyAndMaxGapsPercentages();

extern double subgroupsEarlyMaxBeginningsAtSecondHourPercentage[MAX_TOTAL_SUBGROUPS];
extern int subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[MAX_TOTAL_SUBGROUPS];
extern double subgroupsMaxGapsPerWeekPercentage[MAX_TOTAL_SUBGROUPS];
extern int subgroupsMaxGapsPerWeekMaxGaps[MAX_TOTAL_SUBGROUPS];

extern int nHoursPerSubgroup[MAX_TOTAL_SUBGROUPS]; //used also for students min hours daily
////////END   students no gaps and early


////////BEGIN teachers max days per week
//activities indices (in 0..gt.rules.nInternalActivities-1) for each teacher
extern int teachersMaxDaysPerWeekMaxDays[MAX_TEACHERS]; //-1 for not existing
extern double teachersMaxDaysPerWeekWeightPercentages[MAX_TEACHERS]; //-1 for not existing
//it is practically better to use the variable below and to put it exactly like in generate.cpp,
//the order of activities changes
extern QList<int> teacherActivitiesOfTheDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
extern QList<int> teachersWithMaxDaysPerWeekForActivities[MAX_ACTIVITIES];

bool computeMaxDaysPerWeekForTeachers();
////////END   teachers max days per week


////////BEGIN teachers max gaps per week and per day
//extern QList<int> activitiesForTeachers[MAX_TEACHERS];
extern double teachersMaxGapsPerWeekPercentage[MAX_TEACHERS];
extern int teachersMaxGapsPerWeekMaxGaps[MAX_TEACHERS];

extern double teachersMaxGapsPerDayPercentage[MAX_TEACHERS];
extern int teachersMaxGapsPerDayMaxGaps[MAX_TEACHERS];

extern int nHoursPerTeacher[MAX_TEACHERS];
bool computeNHoursPerTeacher();
bool computeTeachersMaxGapsPerWeekPercentage();
bool computeTeachersMaxGapsPerDayPercentage();
////////END   teachers max gaps per week and per day


////////BEGIN activities same starting time
extern QList<int> activitiesSameStartingTimeActivities[MAX_ACTIVITIES];
extern QList<double> activitiesSameStartingTimePercentages[MAX_ACTIVITIES];
bool computeActivitiesSameStartingTime();
////////END   activities same starting time


////////BEGIN activities same starting hour
extern QList<int> activitiesSameStartingHourActivities[MAX_ACTIVITIES];
extern QList<double> activitiesSameStartingHourPercentages[MAX_ACTIVITIES];
void computeActivitiesSameStartingHour();
////////END   activities same starting hour


////////BEGIN activities same starting day
extern QList<int> activitiesSameStartingDayActivities[MAX_ACTIVITIES];
extern QList<double> activitiesSameStartingDayPercentages[MAX_ACTIVITIES];
void computeActivitiesSameStartingDay();
////////END   activities same starting hour


////////BEGIN activities not overlapping
extern QList<int> activitiesNotOverlappingActivities[MAX_ACTIVITIES];
extern QList<double> activitiesNotOverlappingPercentages[MAX_ACTIVITIES];
void computeActivitiesNotOverlapping();
////////END   activities not overlapping


////////BEGIN teacher(s) max hours daily
extern int teachersGapsPerDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
extern int teachersNHoursPerDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];

extern double teachersMaxHoursDailyPercentages1[MAX_TEACHERS];
extern int teachersMaxHoursDailyMaxHours1[MAX_TEACHERS];
extern int teachersRealGapsPerDay1[MAX_TEACHERS][MAX_DAYS_PER_WEEK];

extern double teachersMaxHoursDailyPercentages2[MAX_TEACHERS];
extern int teachersMaxHoursDailyMaxHours2[MAX_TEACHERS];
extern int teachersRealGapsPerDay2[MAX_TEACHERS][MAX_DAYS_PER_WEEK];

bool computeTeachersMaxHoursDaily();
////////END   teacher(s) max hours daily


////////BEGIN teacher(s) max hours continuously
extern double teachersMaxHoursContinuouslyPercentages1[MAX_TEACHERS];
extern int teachersMaxHoursContinuouslyMaxHours1[MAX_TEACHERS];

extern double teachersMaxHoursContinuouslyPercentages2[MAX_TEACHERS];
extern int teachersMaxHoursContinuouslyMaxHours2[MAX_TEACHERS];

bool computeTeachersMaxHoursContinuously();
////////END   teacher(s) max hours continuously


///////BEGIN teacher(s) activity tag max hours continuously
extern bool haveTeachersActivityTagMaxHoursContinuously;

extern QList<int> teachersActivityTagMaxHoursContinuouslyMaxHours[MAX_TEACHERS];
extern QList<int> teachersActivityTagMaxHoursContinuouslyActivityTag[MAX_TEACHERS];
extern QList<double> teachersActivityTagMaxHoursContinuouslyPercentage[MAX_TEACHERS];

bool computeTeachersActivityTagMaxHoursContinuously();
///////END   teacher(s) activity tag max hours continuously


////////BEGIN teacher(s) min hours daily
extern double teachersMinHoursDailyPercentages[MAX_TEACHERS];
extern int teachersMinHoursDailyMinHours[MAX_TEACHERS];

bool computeTeachersMinHoursDaily();
////////END   teacher(s) min hours daily


////////BEGIN students (set) max hours daily
extern double subgroupsMaxHoursDailyPercentages1[MAX_TOTAL_SUBGROUPS];
extern int subgroupsMaxHoursDailyMaxHours1[MAX_TOTAL_SUBGROUPS];

extern double subgroupsMaxHoursDailyPercentages2[MAX_TOTAL_SUBGROUPS];
extern int subgroupsMaxHoursDailyMaxHours2[MAX_TOTAL_SUBGROUPS];

bool computeSubgroupsMaxHoursDaily();
////////END   students (set) max hours daily


////////BEGIN students (set) max hours continuously
extern double subgroupsMaxHoursContinuouslyPercentages1[MAX_TOTAL_SUBGROUPS];
extern int subgroupsMaxHoursContinuouslyMaxHours1[MAX_TOTAL_SUBGROUPS];

extern double subgroupsMaxHoursContinuouslyPercentages2[MAX_TOTAL_SUBGROUPS];
extern int subgroupsMaxHoursContinuouslyMaxHours2[MAX_TOTAL_SUBGROUPS];

bool computeStudentsMaxHoursContinuously();
////////END   students (set) max hours continuously


///////BEGIN students (set) activity tag max hours continuously
extern bool haveStudentsActivityTagMaxHoursContinuously;

extern QList<int> subgroupsActivityTagMaxHoursContinuouslyMaxHours[MAX_TOTAL_SUBGROUPS];
extern QList<int> subgroupsActivityTagMaxHoursContinuouslyActivityTag[MAX_TOTAL_SUBGROUPS];
extern QList<double> subgroupsActivityTagMaxHoursContinuouslyPercentage[MAX_TOTAL_SUBGROUPS];

bool computeStudentsActivityTagMaxHoursContinuously();
///////END   students (set) activity tag max hours continuously


////////BEGIN students (set) min hours daily
extern double subgroupsMinHoursDailyPercentages[MAX_TOTAL_SUBGROUPS];
extern int subgroupsMinHoursDailyMinHours[MAX_TOTAL_SUBGROUPS];
bool computeSubgroupsMinHoursDaily();
////////END   students (set) min hours daily


//////////////BEGIN 2 activities consecutive
//index represents the first activity, value in array represents the second activity
extern QList<double> constr2ActivitiesConsecutivePercentages[MAX_ACTIVITIES];
extern QList<int> constr2ActivitiesConsecutiveActivities[MAX_ACTIVITIES];
void computeConstr2ActivitiesConsecutive();

//index represents the second activity, value in array represents the first activity
extern QList<double> inverseConstr2ActivitiesConsecutivePercentages[MAX_ACTIVITIES];
extern QList<int> inverseConstr2ActivitiesConsecutiveActivities[MAX_ACTIVITIES];
//////////////END   2 activities consecutive


//////////////BEGIN 2 activities grouped
//index represents the first activity, value in array represents the second activity
extern QList<double> constr2ActivitiesGroupedPercentages[MAX_ACTIVITIES];
extern QList<int> constr2ActivitiesGroupedActivities[MAX_ACTIVITIES];
void computeConstr2ActivitiesGrouped();


//////////////BEGIN 2 activities ordered
//index represents the first activity, value in array represents the second activity
extern QList<double> constr2ActivitiesOrderedPercentages[MAX_ACTIVITIES];
extern QList<int> constr2ActivitiesOrderedActivities[MAX_ACTIVITIES];
void computeConstr2ActivitiesOrdered();

//index represents the second activity, value in array represents the first activity
extern QList<double> inverseConstr2ActivitiesOrderedPercentages[MAX_ACTIVITIES];
extern QList<int> inverseConstr2ActivitiesOrderedActivities[MAX_ACTIVITIES];
//////////////END   2 activities ordered


////////////BEGIN activity ends students day
extern double activityEndsStudentsDayPercentages[MAX_ACTIVITIES]; //-1 for not existing
bool computeActivityEndsStudentsDayPercentages();
extern bool haveActivityEndsStudentsDay;
////////////END   activity ends students day


bool checkMinNDays100Percent();
bool checkMinNDaysConsecutiveIfSameDay();


///////BEGIN teachers interval max days per week
extern double teachersIntervalMaxDaysPerWeekPercentages1[MAX_TEACHERS];
extern int teachersIntervalMaxDaysPerWeekMaxDays1[MAX_TEACHERS];
extern int teachersIntervalMaxDaysPerWeekIntervalStart1[MAX_TEACHERS];
extern int teachersIntervalMaxDaysPerWeekIntervalEnd1[MAX_TEACHERS];

extern double teachersIntervalMaxDaysPerWeekPercentages2[MAX_TEACHERS];
extern int teachersIntervalMaxDaysPerWeekMaxDays2[MAX_TEACHERS];
extern int teachersIntervalMaxDaysPerWeekIntervalStart2[MAX_TEACHERS];
extern int teachersIntervalMaxDaysPerWeekIntervalEnd2[MAX_TEACHERS];

bool computeTeachersIntervalMaxDaysPerWeek();
///////END   teachers interval max days per week


///////BEGIN subgroups interval max days per week
extern double subgroupsIntervalMaxDaysPerWeekPercentages1[MAX_TOTAL_SUBGROUPS];
extern int subgroupsIntervalMaxDaysPerWeekMaxDays1[MAX_TOTAL_SUBGROUPS];
extern int subgroupsIntervalMaxDaysPerWeekIntervalStart1[MAX_TOTAL_SUBGROUPS];
extern int subgroupsIntervalMaxDaysPerWeekIntervalEnd1[MAX_TOTAL_SUBGROUPS];

extern double subgroupsIntervalMaxDaysPerWeekPercentages2[MAX_TOTAL_SUBGROUPS];
extern int subgroupsIntervalMaxDaysPerWeekMaxDays2[MAX_TOTAL_SUBGROUPS];
extern int subgroupsIntervalMaxDaysPerWeekIntervalStart2[MAX_TOTAL_SUBGROUPS];
extern int subgroupsIntervalMaxDaysPerWeekIntervalEnd2[MAX_TOTAL_SUBGROUPS];

bool computeSubgroupsIntervalMaxDaysPerWeek();
///////END   subgroups interval max days per week


////////BEGIN rooms
bool computeBasicSpace();

extern double notAllowedRoomTimePercentages[MAX_ROOMS][MAX_HOURS_PER_WEEK]; //-1 for available
bool computeNotAllowedRoomTimePercentages();

class PreferredRoomsItem{
public:
	double percentage;
	QSet<int> preferredRooms;
};

extern QList<PreferredRoomsItem> activitiesPreferredRoomsList[MAX_ACTIVITIES];
extern bool unspecifiedPreferredRoom[MAX_ACTIVITIES];

extern QList<int> activitiesHomeRoomsHomeRooms[MAX_ACTIVITIES];
extern double activitiesHomeRoomsPercentage[MAX_ACTIVITIES];
extern bool unspecifiedHomeRoom[MAX_ACTIVITIES];

bool computeActivitiesRoomsPreferences();
////////END   rooms


////////BEGIN buildings
extern double maxBuildingChangesPerDayForStudentsPercentages[MAX_TOTAL_SUBGROUPS];
extern int maxBuildingChangesPerDayForStudentsMaxChanges[MAX_TOTAL_SUBGROUPS];
bool computeMaxBuildingChangesPerDayForStudents();

extern double minGapsBetweenBuildingChangesForStudentsPercentages[MAX_TOTAL_SUBGROUPS];
extern int minGapsBetweenBuildingChangesForStudentsMinGaps[MAX_TOTAL_SUBGROUPS];
bool computeMinGapsBetweenBuildingChangesForStudents();

extern double maxBuildingChangesPerDayForTeachersPercentages[MAX_TEACHERS];
extern int maxBuildingChangesPerDayForTeachersMaxChanges[MAX_TEACHERS];
bool computeMaxBuildingChangesPerDayForTeachers();

extern double minGapsBetweenBuildingChangesForTeachersPercentages[MAX_TEACHERS];
extern int minGapsBetweenBuildingChangesForTeachersMinGaps[MAX_TEACHERS];
bool computeMinGapsBetweenBuildingChangesForTeachers();

extern double maxBuildingChangesPerWeekForStudentsPercentages[MAX_TOTAL_SUBGROUPS];
extern int maxBuildingChangesPerWeekForStudentsMaxChanges[MAX_TOTAL_SUBGROUPS];
bool computeMaxBuildingChangesPerWeekForStudents();

extern double maxBuildingChangesPerWeekForTeachersPercentages[MAX_TEACHERS];
extern int maxBuildingChangesPerWeekForTeachersMaxChanges[MAX_TEACHERS];
bool computeMaxBuildingChangesPerWeekForTeachers();
////////END   buildings


extern QList<int> mustComputeTimetableSubgroups[MAX_ACTIVITIES];
extern QList<int> mustComputeTimetableTeachers[MAX_ACTIVITIES];
extern bool mustComputeTimetableSubgroup[MAX_TOTAL_SUBGROUPS];
extern bool mustComputeTimetableTeacher[MAX_TEACHERS];


bool homeRoomsAreOk();


extern bool fixedTimeActivity[MAX_ACTIVITIES]; //this is in fact time fixed, not both time and space
extern bool fixedSpaceActivity[MAX_ACTIVITIES];
bool computeFixedActivities();


#endif
