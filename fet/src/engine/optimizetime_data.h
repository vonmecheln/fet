/*
File optimizetime_data.h
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

#ifndef OPTIMIZETIME_DATA_H
#define OPTIMIZETIME_DATA_H

#include "genetictimetable_defs.h"
#include "genetictimetable.h"

extern int permutation[MAX_ACTIVITIES]; //the permutation matrix to obtain activities in
//(in-de)creasing order of number of compatible activities

bool processTimeConstraints();

extern const int PERCENTAGE_THRESHOLD;


////////BEGIN BASIC TIME CONSTRAINTS
//extern bool activitiesConflicting[MAX_ACTIVITIES][MAX_ACTIVITIES];
extern qint8 activitiesConflictingPercentage[MAX_ACTIVITIES][MAX_ACTIVITIES]; //-1 for not conflicting,
//a value >=0 equal with the weight of the maximum weightPercentage of a basic time constraint

void computeActivitiesConflictingPercentage();
//void computeActivitiesConflicting();

void sortActivities();
////////END   BASIC TIME CONSTRAINTS


////////BEGIN MIN N DAYS TIME CONSTRAINTS
extern QList<int> minNDaysListOfActivities[MAX_ACTIVITIES];
extern QList<int> minNDaysListOfMinDays[MAX_ACTIVITIES];
extern QList<int> minNDaysListOfWeightPercentages[MAX_ACTIVITIES];
extern QList<bool> minNDaysListOfAdjIfBroken[MAX_ACTIVITIES];

void computeMinNDays();
////////END   MIN N DAYS TIME CONSTRAINTS


////////BEGIN st. not available, tch not avail., break, activity preferred time,
////////activity preferred times, activities preferred times
//percentage of allowed time, -1 if no restriction
extern qint8 allowedTimesPercentages[MAX_ACTIVITIES][MAX_HOURS_PER_WEEK];

void computeAllowedTimesPercentages();
////////END   st. not available, tch not avail., break, activity preferred time,
////////activity preferred time, activities preferred times


////////BEGIN students no gaps and early
void computeNHoursPerSubgroup();
bool computeSubgroupsEarlyAndNoGapsPercentages();

extern qint8 subgroupsEarlyPercentage[MAX_TOTAL_SUBGROUPS];
extern qint8 subgroupsNoGapsPercentage[MAX_TOTAL_SUBGROUPS];

extern int nHoursPerSubgroup[MAX_TOTAL_SUBGROUPS];
////////END   students no gaps and early


////////BEGIN teachers max days per week
//activities indices (in 0..gt.rules.nInternalActivities-1) for each teacher
extern int teachersMaxDaysPerWeekMaxDays[MAX_TEACHERS]; //-1 for not existing
extern int teachersMaxDaysPerWeekWeightPercentages[MAX_TEACHERS]; //-1 for not existing
extern QList<int> teacherActivitiesOfTheDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];

extern QList<int> teachersWithMaxDaysPerWeekForActivities[MAX_ACTIVITIES];

void computeActivitiesForTeachers();
bool computeMaxDaysPerWeekForTeachers();
////////END   teachers max days per week


////////BEGIN teachers max gaps per week
extern QList<int> activitiesForTeachers[MAX_TEACHERS];
extern int teachersMaxGapsPercentage[MAX_TEACHERS];
extern int teachersMaxGapsMaxGaps[MAX_TEACHERS];
extern int nHoursPerTeacher[MAX_TEACHERS];
void computeNHoursPerTeacher();
bool computeTeachersMaxGapsPercentage();
////////END   teachers max gaps per week


////////BEGIN students (set) n hours daily
extern int studentsNHoursDailyMinHours[MAX_TOTAL_SUBGROUPS];
extern int studentsNHoursDailyMaxHours[MAX_TOTAL_SUBGROUPS];
extern qint8 studentsNHoursDailyPercentage[MAX_TOTAL_SUBGROUPS];
extern QList<int> studentsSubgroupsWithNHoursDaily;
bool computeStudentsNHoursDaily();
////////END   students (set) n hours daily


////////BEGIN activities same starting time
extern QList<int> activitiesSameStartingTimeActivities[MAX_ACTIVITIES];
extern QList<int> activitiesSameStartingTimePercentages[MAX_ACTIVITIES];
void computeActivitiesSameStartingTime();
////////END   activities same starting time


////////BEGIN activities same starting hour
extern QList<int> activitiesSameStartingHourActivities[MAX_ACTIVITIES];
extern QList<int> activitiesSameStartingHourPercentages[MAX_ACTIVITIES];
void computeActivitiesSameStartingHour();
////////END   activities same starting hour


////////BEGIN activities not overlapping
extern QList<int> activitiesNotOverlappingActivities[MAX_ACTIVITIES];
extern QList<int> activitiesNotOverlappingPercentages[MAX_ACTIVITIES];
void computeActivitiesNotOverlapping();
////////END   activities not overlapping


//QSet<int> sharedSubgroups[MAX_ACTIVITIES][MAX_ACTIVITIES];

//void computeSharedSubgroups();

//void Rules::computeActivitiesSimilar();
	
//void Rules::computeActivitiesContained()


#endif
