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

extern int permutation[MAX_ACTIVITIES]; //the permutation matrix to obtain activities in
//decreasing difficulty order

bool processTimeConstraints();


extern qint16 subgroupsTimetable[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern qint16 teachersTimetable[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
extern qint16 roomsTimetable[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];


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

void computeMinNDays();
////////END   MIN N DAYS TIME CONSTRAINTS


////////BEGIN st. not available, tch not avail., break, activity preferred time,
////////activity preferred times, activities preferred times
//percentage of allowed time, -1 if no restriction
extern double allowedTimesPercentages[MAX_ACTIVITIES][MAX_HOURS_PER_WEEK];

//break, which is not considered gap, false means no break, true means 100% break
//break can only be 100% or none
extern bool breakTime[MAX_HOURS_PER_WEEK];
extern bool breakDayHour[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

//students set not available, which is not considered gap, false means available, true means 100% not available
//students set not available can only be 100% or none
extern bool subgroupNotAvailableTime[MAX_TOTAL_SUBGROUPS][MAX_HOURS_PER_WEEK];
extern bool subgroupNotAvailableDayHour[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

//teacher not available, which is not considered gap, false means available, true means 100% not available
//teacher not available can only be 100% or none
extern bool teacherNotAvailableTime[MAX_TEACHERS][MAX_HOURS_PER_WEEK];
extern bool teacherNotAvailableDayHour[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

bool computeAllowedTimesPercentages();
////////END   st. not available, tch not avail., break, activity preferred time,
////////activity preferred time, activities preferred times


////////BEGIN students no gaps and early
bool computeNHoursPerSubgroup();
bool computeSubgroupsEarlyAndNoGapsPercentages();

extern double subgroupsEarlyPercentage[MAX_TOTAL_SUBGROUPS];
extern double subgroupsNoGapsPercentage[MAX_TOTAL_SUBGROUPS];

extern int nHoursPerSubgroup[MAX_TOTAL_SUBGROUPS]; //used also for students min hours daily
////////END   students no gaps and early


////////BEGIN teachers max days per week
//activities indices (in 0..gt.rules.nInternalActivities-1) for each teacher
extern int teachersMaxDaysPerWeekMaxDays[MAX_TEACHERS]; //-1 for not existing
extern double teachersMaxDaysPerWeekWeightPercentages[MAX_TEACHERS]; //-1 for not existing
extern QList<int> teacherActivitiesOfTheDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];

extern QList<int> teachersWithMaxDaysPerWeekForActivities[MAX_ACTIVITIES];

bool computeMaxDaysPerWeekForTeachers();
////////END   teachers max days per week


////////BEGIN teachers max gaps per week
extern QList<int> activitiesForTeachers[MAX_TEACHERS];
extern double teachersMaxGapsPercentage[MAX_TEACHERS];
extern int teachersMaxGapsMaxGaps[MAX_TEACHERS];
extern int nHoursPerTeacher[MAX_TEACHERS];
bool computeNHoursPerTeacher();
bool computeTeachersMaxGapsPercentage();
////////END   teachers max gaps per week


////////BEGIN activities same starting time
extern QList<int> activitiesSameStartingTimeActivities[MAX_ACTIVITIES];
extern QList<double> activitiesSameStartingTimePercentages[MAX_ACTIVITIES];
void computeActivitiesSameStartingTime();
////////END   activities same starting time


////////BEGIN activities same starting hour
extern QList<int> activitiesSameStartingHourActivities[MAX_ACTIVITIES];
extern QList<double> activitiesSameStartingHourPercentages[MAX_ACTIVITIES];
void computeActivitiesSameStartingHour();
////////END   activities same starting hour


////////BEGIN activities not overlapping
extern QList<int> activitiesNotOverlappingActivities[MAX_ACTIVITIES];
extern QList<double> activitiesNotOverlappingPercentages[MAX_ACTIVITIES];
void computeActivitiesNotOverlapping();
////////END   activities not overlapping


////////BEGIN teacher(s) max hours daily
extern double teachersMaxHoursDailyPercentages[MAX_TEACHERS];
extern int teachersMaxHoursDailyMaxHours[MAX_TEACHERS];
extern int teachersGapsPerDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
extern int teachersRealGapsPerDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
extern int teachersNHoursPerDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
bool computeTeachersMaxHoursDaily();
////////END   teacher(s) max hours daily


////////BEGIN students (set) max hours daily
extern double subgroupsMaxHoursDailyPercentages[MAX_TOTAL_SUBGROUPS];
extern int subgroupsMaxHoursDailyMaxHours[MAX_TOTAL_SUBGROUPS];
bool computeSubgroupsMaxHoursDaily();
////////END   students (set) max hours daily


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


////////BEGIN rooms
bool computeBasicSpace();

extern double allowedRoomTimePercentages[MAX_ROOMS][MAX_HOURS_PER_WEEK]; //-1 for available
bool computeAllowedRoomTimePercentages();

extern QList<int> activitiesPreferredRoomsPreferredRooms[MAX_ACTIVITIES];
extern double activitiesPreferredRoomsPercentage[MAX_ACTIVITIES];
extern bool unspecifiedRoom[MAX_ACTIVITIES];
bool computeActivitiesRoomsPreferences();
////////END   rooms


//QSet<int> sharedSubgroups[MAX_ACTIVITIES][MAX_ACTIVITIES];

//void computeSharedSubgroups();

//void Rules::computeActivitiesSimilar();
	
//void Rules::computeActivitiesContained()


#endif
