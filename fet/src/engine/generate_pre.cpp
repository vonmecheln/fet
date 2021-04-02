/*
File generate_pre.cpp
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

#include "timetable_defs.h"
#include "rules.h"
#include "timetable.h"

#include "generate_pre.h"

#include <string.h>

#include <iostream>
#include <fstream>
using namespace std;

#include <QMessageBox>

extern Timetable gt;

#include <QApplication>
#include <QProgressDialog>

extern QApplication* pqapplication;


int permutation[MAX_ACTIVITIES]; //the permutation matrix to obtain activities in
//decreasing difficulty order


qint16 teachersTimetable[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
qint16 subgroupsTimetable[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
qint16 roomsTimetable[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];


//BASIC TIME
qint8 activitiesConflictingPercentage[MAX_ACTIVITIES][MAX_ACTIVITIES];

//MIN N DAYS BETWEEN ACTIVITIES
QList<int> minNDaysListOfActivities[MAX_ACTIVITIES];
QList<int> minNDaysListOfMinDays[MAX_ACTIVITIES];
QList<double> minNDaysListOfWeightPercentages[MAX_ACTIVITIES];
QList<bool> minNDaysListOfConsecutiveIfSameDay[MAX_ACTIVITIES];

//TCH & ST NOT AVAIL, BREAK, ACT(S) PREFERRED TIME(S)
double allowedTimesPercentages[MAX_ACTIVITIES][MAX_HOURS_PER_WEEK];

//breaks are no gaps - I keep track of this
bool breakTime[MAX_HOURS_PER_WEEK];
bool breakDayHour[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

bool subgroupNotAvailableTime[MAX_TOTAL_SUBGROUPS][MAX_HOURS_PER_WEEK];
bool subgroupNotAvailableDayHour[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

bool teacherNotAvailableTime[MAX_TEACHERS][MAX_HOURS_PER_WEEK];
bool teacherNotAvailableDayHour[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

//STUDENTS NO GAPS & EARLY
int nHoursPerSubgroup[MAX_TOTAL_SUBGROUPS];
double subgroupsEarlyPercentage[MAX_TOTAL_SUBGROUPS];
double subgroupsNoGapsPercentage[MAX_TOTAL_SUBGROUPS];

//TEACHERS MAX DAYS PER WEEK
int teachersMaxDaysPerWeekMaxDays[MAX_TEACHERS];
double teachersMaxDaysPerWeekWeightPercentages[MAX_TEACHERS];
QList<int> teacherActivitiesOfTheDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
QList<int> teachersWithMaxDaysPerWeekForActivities[MAX_ACTIVITIES];

/////////////////care for teachers max gaps
int nHoursPerTeacher[MAX_TEACHERS];
double teachersMaxGapsPercentage[MAX_TEACHERS];
int teachersMaxGapsMaxGaps[MAX_TEACHERS];

//activities same starting time
QList<int> activitiesSameStartingTimeActivities[MAX_ACTIVITIES];
QList<double> activitiesSameStartingTimePercentages[MAX_ACTIVITIES];

//activities same starting hour
QList<int> activitiesSameStartingHourActivities[MAX_ACTIVITIES];
QList<double> activitiesSameStartingHourPercentages[MAX_ACTIVITIES];

//activities not overlapping
QList<int> activitiesNotOverlappingActivities[MAX_ACTIVITIES];
QList<double> activitiesNotOverlappingPercentages[MAX_ACTIVITIES];

//teacher(s) max hours daily
int teachersGapsPerDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
int teachersNHoursPerDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];

double teachersMaxHoursDailyPercentages1[MAX_TEACHERS];
int teachersMaxHoursDailyMaxHours1[MAX_TEACHERS];
int teachersRealGapsPerDay1[MAX_TEACHERS][MAX_DAYS_PER_WEEK];

double teachersMaxHoursDailyPercentages2[MAX_TEACHERS];
int teachersMaxHoursDailyMaxHours2[MAX_TEACHERS];
int teachersRealGapsPerDay2[MAX_TEACHERS][MAX_DAYS_PER_WEEK];

//teacher(s) min hours daily
double teachersMinHoursDailyPercentages[MAX_TEACHERS];
int teachersMinHoursDailyMinHours[MAX_TEACHERS];

//students (set) max hours daily
double subgroupsMaxHoursDailyPercentages1[MAX_TOTAL_SUBGROUPS];
int subgroupsMaxHoursDailyMaxHours1[MAX_TOTAL_SUBGROUPS];

double subgroupsMaxHoursDailyPercentages2[MAX_TOTAL_SUBGROUPS];
int subgroupsMaxHoursDailyMaxHours2[MAX_TOTAL_SUBGROUPS];

//students (set) min hours daily
double subgroupsMinHoursDailyPercentages[MAX_TOTAL_SUBGROUPS];
int subgroupsMinHoursDailyMinHours[MAX_TOTAL_SUBGROUPS];

// 2 activities consecutive
//index represents the first activity, value in array represents the second activity
QList<double> constr2ActivitiesConsecutivePercentages[MAX_ACTIVITIES];
QList<int> constr2ActivitiesConsecutiveActivities[MAX_ACTIVITIES];

//index represents the second activity, value in array represents the first activity
QList<double> inverseConstr2ActivitiesConsecutivePercentages[MAX_ACTIVITIES];
QList<int> inverseConstr2ActivitiesConsecutiveActivities[MAX_ACTIVITIES];
// 2 activities consecutive

double activityEndsStudentsDayPercentages[MAX_ACTIVITIES];

////////rooms
double allowedRoomTimePercentages[MAX_ROOMS][MAX_HOURS_PER_WEEK]; //-1 for available

QList<int> activitiesPreferredRoomsPreferredRooms[MAX_ACTIVITIES];
double activitiesPreferredRoomsPercentage[MAX_ACTIVITIES];
bool unspecifiedRoom[MAX_ACTIVITIES];
////////rooms

#define max(x,y)		((x)>=(y)?(x):(y))


bool processTimeConstraints()
{
	assert(gt.rules.internalStructureComputed);

	/////1. BASIC TIME CONSTRAINTS
	bool t=computeActivitiesConflictingPercentage();
	if(!t)
		return false;
	//sortActivities(); - done last time, because taking care of other constraints
	//////////////////////////////
	
	/////2. min n days between activities
	computeMinNDays();
	/////////////////////////////////////
	
	/////3. st not avail, tch not avail, break, activity pref time,
	/////   activity preferred times, activities preferred times
	t=computeAllowedTimesPercentages();
	if(!t)
		return false;
	///////////////////////////////////////////////////////////////
	
	/////4. students no gaps and early
	t=computeNHoursPerSubgroup();
	if(!t)
		return false;
	t=computeSubgroupsEarlyAndNoGapsPercentages();
	if(!t)
		return false;
	//////////////////////////////////
	
	/////5. TEACHER MAX DAYS PER WEEK
	t=computeMaxDaysPerWeekForTeachers();
	if(!t)
		return false;
	//////////////////////////////////
	
	
	/////6. TEACHERS MAX GAPS PER WEEK
	t=computeNHoursPerTeacher();
	if(!t)
		return false;
	t=computeTeachersMaxGapsPercentage();
	if(!t)
		return false;
	//////////////////////////////////
	
	computeActivitiesSameStartingTime();

	computeActivitiesSameStartingHour();
	
	computeActivitiesNotOverlapping();

	//must be after allowed times, after n hours per teacher and after max days per week
	t=computeTeachersMaxHoursDaily();
	if(!t)
		return false;

	//must be after n hours per teacher
	t=computeTeachersMinHoursDaily();
	if(!t)
		return false;
	
	//must be after allowed times and after n hours per subgroup
	t=computeSubgroupsMaxHoursDaily();
	if(!t)
		return false;
	
	t=computeSubgroupsMinHoursDaily();
	if(!t)
		return false;
		
	computeConstr2ActivitiesConsecutive();
	
	t=computeActivityEndsStudentsDayPercentages();
	if(!t)
		return false;
		
	//check for impossible min n days
	t=checkMinNDays100Percent();
	if(!t)
		return false;
	t=checkMinNDaysConsecutiveIfSameDay();
	if(!t)
		return false;
		
	/////////////rooms	
	t=computeBasicSpace();
	if(!t)
		return false;
	t=computeAllowedRoomTimePercentages();
	if(!t)
		return false;
	t=computeActivitiesRoomsPreferences();
	if(!t)
		return false;
	//////////////////
	
	sortActivities();
	
	bool ok=true;
	
	return ok;
}

//must be after allowed times and after n hours per subgroup
bool computeSubgroupsMaxHoursDaily()
{
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

			//////////
			/*if(smd->weightPercentage!=100){
				ok=false;

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize, because you have constraint students max hours daily with"
				 " weight (percentage) below 100. Starting with FET version 5.3.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again"),
				 QObject::tr("Skip rest of max hours problems"), QObject::tr("See next incompatibility max hours"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}*/
			//////////

			for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMaxHoursDailyMaxHours1[sb]==-1 ||
				 subgroupsMaxHoursDailyMaxHours1[sb] >= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyPercentages1[sb] <= smd->weightPercentage){
				 	subgroupsMaxHoursDailyMaxHours1[sb] = smd->maxHoursDaily;
					subgroupsMaxHoursDailyPercentages1[sb] = smd->weightPercentage;
					}
				else if(subgroupsMaxHoursDailyMaxHours1[sb] <= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyPercentages1[sb] >= smd->weightPercentage){
				 	//nothing
				}
				else{
					if(subgroupsMaxHoursDailyMaxHours2[sb]==-1 ||
					 subgroupsMaxHoursDailyMaxHours2[sb] >= smd->maxHoursDaily &&
					 subgroupsMaxHoursDailyPercentages2[sb] <= smd->weightPercentage){
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
		
						int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
						 QObject::tr("Cannot optimize for subgroup %1, because there are too many constraints"
						 " of type max hours daily relating to him, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a subgroup, but their resultant must"
						 " be maximum 2 constraints of type max hours daily.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again.\n\n"
						 " For more details, join the mailing list or email the author")
						 .arg(gt.rules.internalSubgroupsList[sb]->name),
						 QObject::tr("Skip rest of max hours problems"), QObject::tr("See next incompatibility max hours"), QString(),
						 1, 0 );
				 	
						if(t==0)
							return false;
					}
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
			ConstraintStudentsSetMaxHoursDaily* smd=(ConstraintStudentsSetMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			//////////
			/*if(smd->weightPercentage!=100){
				ok=false;

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize, because you have constraint students set max hours daily for students set %1 with"
				 " weight (percentage) below 100. Starting with FET version 5.3.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(smd->students),
				 QObject::tr("Skip rest of max hours problems"), QObject::tr("See next incompatibility max hours"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}*/
			//////////

			for(int q=0; q<smd->iSubgroupsList.count(); q++){
				int sb=smd->iSubgroupsList.at(q);
			//for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMaxHoursDailyMaxHours1[sb]==-1 ||
				 subgroupsMaxHoursDailyMaxHours1[sb] >= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyPercentages1[sb] <= smd->weightPercentage){
				 	subgroupsMaxHoursDailyMaxHours1[sb] = smd->maxHoursDaily;
					subgroupsMaxHoursDailyPercentages1[sb] = smd->weightPercentage;
					}
				else if(subgroupsMaxHoursDailyMaxHours1[sb] <= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyPercentages1[sb] >= smd->weightPercentage){
				 	//nothing
				}
				else{
					if(subgroupsMaxHoursDailyMaxHours2[sb]==-1 ||
					 subgroupsMaxHoursDailyMaxHours2[sb] >= smd->maxHoursDaily &&
					 subgroupsMaxHoursDailyPercentages2[sb] <= smd->weightPercentage){
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
	
						int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
						 QObject::tr("Cannot optimize for subgroup %1, because there are too many constraints"
						 " of type max hours daily relating to him, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a subgroup, but their resultant must"
						 " be maximum 2 constraints of type max hours daily.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again."
						 " For more details, join the mailing list or email the author")
						 .arg(gt.rules.internalSubgroupsList[sb]->name),
						 QObject::tr("Skip rest of max hours problems"), QObject::tr("See next incompatibility max hours"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}
				}
			}
		}
	}
	
	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		if(subgroupsMaxHoursDailyPercentages1[sb]==100){
			int nAllowedSlotsPerDay[MAX_DAYS_PER_WEEK];
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[sb][d][h])
						nAllowedSlotsPerDay[d]++;
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],subgroupsMaxHoursDailyMaxHours1[sb]);
			}
			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerSubgroup[sb]){
				ok=false;
				
				QString s;
				s=QObject::tr("Cannot optimize for subgroup %1, because there is a constraint of type"
				 " max %2 hours daily with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, students set not available and/or breaks. The number of total hours for this subgroup is"
				 " %3 and the number of available slots is, considering max hours daily and all other constraints, %4.")
				 .arg(gt.rules.internalSubgroupsList[sb]->name)
				 .arg(subgroupsMaxHoursDailyMaxHours1[sb])
				 .arg(nHoursPerSubgroup[sb])
				 .arg(total);
				s+="\n\n";
				s+=QObject::tr("Please modify your data accordingly and try again"
				 ". For more details, join the mailing list or email the author");
	
				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"), s,
				 QObject::tr("Skip rest of max hours problems"), QObject::tr("See next incompatibility max hours"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}

	for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
		if(subgroupsMaxHoursDailyPercentages2[sb]==100){
			int nAllowedSlotsPerDay[MAX_DAYS_PER_WEEK];
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !subgroupNotAvailableDayHour[sb][d][h])
						nAllowedSlotsPerDay[d]++;
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],subgroupsMaxHoursDailyMaxHours2[sb]);
			}
			int total=0;
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				total+=nAllowedSlotsPerDay[d];
			if(total<nHoursPerSubgroup[sb]){
				ok=false;
				
				QString s;
				s=QObject::tr("Cannot optimize for subgroup %1, because there is a constraint of type"
				 " max %2 hours daily with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, students set not available and/or breaks. The number of total hours for this subgroup is"
				 " %3 and the number of available slots is, considering max hours daily and all other constraints, %4.")
				 .arg(gt.rules.internalSubgroupsList[sb]->name)
				 .arg(subgroupsMaxHoursDailyMaxHours2[sb])
				 .arg(nHoursPerSubgroup[sb])
				 .arg(total);
				s+="\n\n";
				s+=QObject::tr("Please modify your data accordingly and try again"
				 ". For more details, join the mailing list or email the author");
	
				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"), s,
				 QObject::tr("Skip rest of max hours problems"), QObject::tr("See next incompatibility max hours"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}
	
	return ok;
}
	
bool computeSubgroupsMinHoursDaily()
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMinHoursDailyMinHours[i]=-1;
		subgroupsMinHoursDailyPercentages[i]=-1;
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_HOURS_DAILY){
			ConstraintStudentsMinHoursDaily* smd=(ConstraintStudentsMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			if(smd->weightPercentage!=100){
				ok=false;
	
				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize for students, because the constraint of type min hours daily relating to students"
				 " has no 100% weight"
				 ". Please modify your data accordingly and try again"
				 ". For more details, join the mailing list or email the author"),
				 QObject::tr("Skip rest of min hours problems"), QObject::tr("See next incompatibility min hours"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
			ConstraintStudentsSetMinHoursDaily* smd=(ConstraintStudentsSetMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			
			if(smd->weightPercentage!=100){
				ok=false;
	
				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize for students set %1, because the constraint of type min hours daily relating to him"
				 " has no 100% weight"
				 ". Please modify your data accordingly and try again"
				 ". For more details, join the mailing list or email the author")
				 .arg(smd->students),
				 QObject::tr("Skip rest of min hours problems"), QObject::tr("See next incompatibility min hours"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}

	if(!ok)
		return ok;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MIN_HOURS_DAILY){
			ConstraintStudentsMinHoursDaily* smd=(ConstraintStudentsMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMinHoursDailyMinHours[sb]==-1 ||
				 subgroupsMinHoursDailyMinHours[sb] <= smd->minHoursDaily &&
				 subgroupsMinHoursDailyPercentages[sb] <= smd->weightPercentage){
				 	subgroupsMinHoursDailyMinHours[sb] = smd->minHoursDaily;
					subgroupsMinHoursDailyPercentages[sb] = smd->weightPercentage;
					}
				else if(subgroupsMinHoursDailyMinHours[sb] >= smd->minHoursDaily &&
				 subgroupsMinHoursDailyPercentages[sb] >= smd->weightPercentage){
				 	//nothing
				}
				else{ //cannot proceed
					ok=false;
	
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("Cannot optimize for subgroup %1, because there are two constraints"
					 " of type min hours daily relating to him, and the weight percentage is higher on the constraint"
					 " with less minimum hours. You are allowed only to have for each subgroup"
					 " the most important constraint with maximum weight percentage and largest minimum hours daily allowed"
					 ". Please modify your data accordingly and try again"
					 ". For more details, join the mailing list or email the author")
					 .arg(gt.rules.internalSubgroupsList[sb]->name),
					 QObject::tr("Skip rest of min hours problems"), QObject::tr("See next incompatibility min hours"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY){
			ConstraintStudentsSetMinHoursDaily* smd=(ConstraintStudentsSetMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			for(int q=0; q<smd->iSubgroupsList.count(); q++){
				int sb=smd->iSubgroupsList.at(q);
				if(subgroupsMinHoursDailyMinHours[sb]==-1 ||
				 subgroupsMinHoursDailyMinHours[sb] <= smd->minHoursDaily &&
				 subgroupsMinHoursDailyPercentages[sb] <= smd->weightPercentage){
				 	subgroupsMinHoursDailyMinHours[sb] = smd->minHoursDaily;
					subgroupsMinHoursDailyPercentages[sb] = smd->weightPercentage;
					}
				else if(subgroupsMinHoursDailyMinHours[sb] >= smd->minHoursDaily &&
				 subgroupsMinHoursDailyPercentages[sb] >= smd->weightPercentage){
				 	//nothing
				}
				else{ //cannot proceed
					ok=false;
	
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("Cannot optimize for subgroup %1, because there are two constraints"
					 " of type min hours daily relating to him, and the weight percentage is higher on the constraint"
					 " with less minimum hours. You are allowed only to have for each subgroup"
					 " the most important constraint with maximum weight percentage and largest minimum hours daily allowed"
					 ". Please modify your data accordingly and try again"
					 ". For more details, join the mailing list or email the author")
					 .arg(gt.rules.internalSubgroupsList[sb]->name),
					 QObject::tr("Skip rest of min hours problems"), QObject::tr("See next incompatibility min hours"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
		}
	}

	for(int i=0; i<gt.rules.nInternalSubgroups; i++)
		if(subgroupsMinHoursDailyMinHours[i]>=0){
			if(gt.rules.nDaysPerWeek*subgroupsMinHoursDailyMinHours[i] > nHoursPerSubgroup[i]){
				ok=false;
			
				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("For subgroup %1 you have too little activities to respect the constraint"
				 " of type min hours daily. Please modify your data accordingly and try again"
				 ". A possible situation is that you have unneeded groups like 'year1 WHOLE YEAR' and subgroups with name like 'year1 WHOLE YEAR WHOLE GROUP'"
				 ". You might need to remove such dummy groups and subgroups (they are generated if you start allocation"
				 " with incomplete students data). FET cannot automatically remove such dummy groups and subgroups"
				 ". For more details, join the mailing list or email the author")
				 .arg(gt.rules.internalSubgroupsList[i]->name),
				 QObject::tr("Skip rest of min hours problems"), QObject::tr("See next incompatibility min hours"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}

			for(int j=0; j<gt.rules.nDaysPerWeek; j++){
				int freeSlots=0;
				for(int k=0; k<gt.rules.nHoursPerDay; k++)
					if(!subgroupNotAvailableDayHour[i][j][k] && !breakDayHour[j][k])
						freeSlots++;
				if(subgroupsMinHoursDailyMinHours[i]>freeSlots){
					ok=false;
			
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("For subgroup %1 cannot respect the constraint"
					 " of type min hours daily on day %2, because of students set not available and/or break."
					 " Probably you have a day off for this subgroup. The workaround is described in the Help/Frequently Asked Questions menu"
					 " (for short: keep constraint min hours daily for this students set, remove corresponding constraint students set not available (or break)"
					 " for this students set, add a dummy activity for this students set with dummy subject and duration the number of hours per day"
					 " and add a constraint activity preferred time for this dummy activity in the corresponding day, first hour)\n\n"
					 "Please modify your data accordingly and try again")
					 .arg(gt.rules.internalSubgroupsList[i]->name)
					 .arg(gt.rules.daysOfTheWeek[j]),
					 QObject::tr("Skip rest of min hours problems"), QObject::tr("See next incompatibility min hours"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
		}
	
	return ok;
}
	
//must be after allowed times, after n hours per teacher and after max days per week
bool computeTeachersMaxHoursDaily()
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

			//////////
			/*if(tmd->weightPercentage!=100){
				ok=false;

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize, because you have constraint teacher max hours daily for teacher %1 with"
				 " weight (percentage) below 100. Starting with FET version 5.3.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tmd->teacherName),
				 QObject::tr("Skip rest of max hours problems"), QObject::tr("See next incompatibility max hours"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}*/
			//////////

			if(teachersMaxHoursDailyMaxHours1[tmd->teacher_ID]==-1 ||
			 teachersMaxHoursDailyMaxHours1[tmd->teacher_ID] >= tmd->maxHoursDaily &&
			 teachersMaxHoursDailyPercentages1[tmd->teacher_ID] <= tmd->weightPercentage){
			 	teachersMaxHoursDailyMaxHours1[tmd->teacher_ID] = tmd->maxHoursDaily;
				teachersMaxHoursDailyPercentages1[tmd->teacher_ID] = tmd->weightPercentage;
			}
			else if(teachersMaxHoursDailyMaxHours1[tmd->teacher_ID] <= tmd->maxHoursDaily &&
			 teachersMaxHoursDailyPercentages1[tmd->teacher_ID] >= tmd->weightPercentage){
			 	//nothing
			}
			else{
				if(teachersMaxHoursDailyMaxHours2[tmd->teacher_ID]==-1 ||
				 teachersMaxHoursDailyMaxHours2[tmd->teacher_ID] >= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyPercentages2[tmd->teacher_ID] <= tmd->weightPercentage){
				 	teachersMaxHoursDailyMaxHours2[tmd->teacher_ID] = tmd->maxHoursDaily;
					teachersMaxHoursDailyPercentages2[tmd->teacher_ID] = tmd->weightPercentage;
				}
				else if(teachersMaxHoursDailyMaxHours2[tmd->teacher_ID] <= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyPercentages2[tmd->teacher_ID] >= tmd->weightPercentage){
				 	//nothing
				}
				else{ //cannot proceed
					ok=false;

					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("Cannot optimize for teacher %1, because there are too many constraints"
					 " of type max hours daily relating to him, which cannot be compressed in 2 constraints of this type."
					 " Two constraints max hours can be compressed into a single one if the max hours are lower"
					 " in the first one and the weight percentage is higher on the first one."
					 " It is possible to use any number of such constraints for a teacher, but their resultant must"
					 " be maximum 2 constraints of type max hours daily.\n\n"
					 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
					 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
					 " Please modify your data accordingly and try again."
					 " For more details, join the mailing list or email the author")
					 .arg(gt.rules.internalTeachersList[tmd->teacher_ID]->name),
					 QObject::tr("Skip rest of max hours problems"), QObject::tr("See next incompatibility max hours"), QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_HOURS_DAILY){
			ConstraintTeachersMaxHoursDaily* tmd=(ConstraintTeachersMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			//////////
			/*if(tmd->weightPercentage!=100){
				ok=false;

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize, because you have constraint teachers max hours daily with"
				 " weight (percentage) below 100. Starting with FET version 5.3.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again"),
				 QObject::tr("Skip rest of max hours problems"), QObject::tr("See next incompatibility max hours"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}*/
			//////////

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMaxHoursDailyMaxHours1[tch]==-1 ||
				 teachersMaxHoursDailyMaxHours1[tch] >= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyPercentages1[tch] <= tmd->weightPercentage){
				 	teachersMaxHoursDailyMaxHours1[tch] = tmd->maxHoursDaily;
					teachersMaxHoursDailyPercentages1[tch] = tmd->weightPercentage;
					}
				else if(teachersMaxHoursDailyMaxHours1[tch] <= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyPercentages1[tch] >= tmd->weightPercentage){
				 	//nothing
				}
				else{
					if(teachersMaxHoursDailyMaxHours2[tch]==-1 ||
					 teachersMaxHoursDailyMaxHours2[tch] >= tmd->maxHoursDaily &&
					 teachersMaxHoursDailyPercentages2[tch] <= tmd->weightPercentage){
					 	teachersMaxHoursDailyMaxHours2[tch] = tmd->maxHoursDaily;
						teachersMaxHoursDailyPercentages2[tch] = tmd->weightPercentage;
						}
					else if(teachersMaxHoursDailyMaxHours2[tch] <= tmd->maxHoursDaily &&
					 teachersMaxHoursDailyPercentages2[tch] >= tmd->weightPercentage){
				 	//nothing
					}
					else{ //cannot proceed
						ok=false;

						int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
						 QObject::tr("Cannot optimize for teacher %1, because there are too many constraints"
						 " of type max hours daily relating to him, which cannot be compressed in 2 constraints of this type."
						 " Two constraints max hours can be compressed into a single one if the max hours are lower"
						 " in the first one and the weight percentage is higher on the first one."
						 " It is possible to use any number of such constraints for a teacher, but their resultant must"
						 " be maximum 2 constraints of type max hours daily.\n\n"
						 " Example: you are allowed to use 3 constraints: 6 hours 95%, 7 hours 100% and 8 hours 100%,"
						 " which can be compressed into 2 constraints: 6 hours 95%, 7 hours 100%\n\n"
						 " Please modify your data accordingly and try again."
						 " For more details, join the mailing list or email the author")
						 .arg(gt.rules.internalTeachersList[tch]->name),
						 QObject::tr("Skip rest of max hours problems"), QObject::tr("See next incompatibility max hours"), QString(),
						 1, 0 );

						if(t==0)
							return false;
					}
				}
			}
		}
	}
	
	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMaxHoursDailyPercentages1[tc]==100){
			int nAllowedSlotsPerDay[MAX_DAYS_PER_WEEK];
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h])
						nAllowedSlotsPerDay[d]++;
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],teachersMaxHoursDailyMaxHours1[tc]);
			}
			
			int dayAvailable[MAX_DAYS_PER_WEEK];
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				dayAvailable[d]=1;
			if(teachersMaxDaysPerWeekMaxDays[tc]>=0){
				//n days per week has 100% weight
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
				s=QObject::tr("Cannot optimize for teacher %1, because there is a constraint of type"
				 " max %2 hours daily with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, teacher max days per week, teacher not available and/or breaks."
				 " The number of total hours for this teacher is"
				 " %3 and the number of available slots is, considering max hours daily and all other constraints, %4.")
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(teachersMaxHoursDailyMaxHours1[tc])
				 .arg(nHoursPerTeacher[tc])
				 .arg(total);
				s+="\n\n";
				s+=QObject::tr("Please modify your data accordingly and try again"
				 ". For more details, join the mailing list or email the author");
	
				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"), s,
				 QObject::tr("Skip rest of max hours problems"), QObject::tr("See next incompatibility max hours"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}

	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMaxHoursDailyPercentages2[tc]==100){
			int nAllowedSlotsPerDay[MAX_DAYS_PER_WEEK];
			for(int d=0; d<gt.rules.nDaysPerWeek; d++){
				nAllowedSlotsPerDay[d]=0;
				for(int h=0; h<gt.rules.nHoursPerDay; h++)
					if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h])
						nAllowedSlotsPerDay[d]++;
				nAllowedSlotsPerDay[d]=min(nAllowedSlotsPerDay[d],teachersMaxHoursDailyMaxHours2[tc]);
			}
			
			int dayAvailable[MAX_DAYS_PER_WEEK];
			for(int d=0; d<gt.rules.nDaysPerWeek; d++)
				dayAvailable[d]=1;
			if(teachersMaxDaysPerWeekMaxDays[tc]>=0){
				//n days per week has 100% weight
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
				s=QObject::tr("Cannot optimize for teacher %1, because there is a constraint of type"
				 " max %2 hours daily with 100% weight which cannot be respected because of number of days per week,"
				 " number of hours per day, teacher max days per week, teacher not available and/or breaks."
				 " The number of total hours for this teacher is"
				 " %3 and the number of available slots is, considering max hours daily and all other constraints, %4.")
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(teachersMaxHoursDailyMaxHours2[tc])
				 .arg(nHoursPerTeacher[tc])
				 .arg(total);
				s+="\n\n";
				s+=QObject::tr("Please modify your data accordingly and try again"
				 ". For more details, join the mailing list or email the author");
	
				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"), s,
				 QObject::tr("Skip rest of max hours problems"), QObject::tr("See next incompatibility max hours"), QString(),
				 1, 0 );
				 	
				if(t==0)
					return false;
			}
		}
	}

	return ok;
}

//must be after n hours per teacher
bool computeTeachersMinHoursDaily()
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMinHoursDailyMinHours[i]=-1;
		teachersMinHoursDailyPercentages[i]=-1;
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MIN_HOURS_DAILY){
			ConstraintTeacherMinHoursDaily* tmd=(ConstraintTeacherMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize, because you have constraint teacher min hours daily for teacher %1 with"
				 " weight (percentage) below 100. Starting with FET version 5.4.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tmd->teacherName),
				 QObject::tr("Skip rest of min hours problems"), QObject::tr("See next incompatibility min hours"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minHoursDaily>gt.rules.nHoursPerDay){
				ok=false;

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize, because you have constraint teacher min hours daily for teacher %1 with"
				 " %2 min hours daily, and the number of working hours per day is only %3. Please correct and try again")
				 .arg(tmd->teacherName)
				 .arg(tmd->minHoursDaily)
				 .arg(gt.rules.nHoursPerDay),
				 QObject::tr("Skip rest of min hours problems"), QObject::tr("See next incompatibility min hours"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////

			if(teachersMinHoursDailyMinHours[tmd->teacher_ID]==-1 || teachersMinHoursDailyMinHours[tmd->teacher_ID]<tmd->minHoursDaily){
				teachersMinHoursDailyMinHours[tmd->teacher_ID]=tmd->minHoursDaily;
				teachersMinHoursDailyPercentages[tmd->teacher_ID]=100;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MIN_HOURS_DAILY){
			ConstraintTeachersMinHoursDaily* tmd=(ConstraintTeachersMinHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			//////////
			if(tmd->weightPercentage!=100){
				ok=false;

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize, because you have constraint teachers min hours daily with"
				 " weight (percentage) below 100. Starting with FET version 5.4.0 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again"),
				 QObject::tr("Skip rest of min hours problems"), QObject::tr("See next incompatibility min hours"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////

			//////////
			if(tmd->minHoursDaily>gt.rules.nHoursPerDay){
				ok=false;

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize, because you have constraint teachers min hours daily with"
				 " %1 min hours daily, and the number of working hours per day is only %2. Please correct and try again")
				 .arg(tmd->minHoursDaily)
				 .arg(gt.rules.nHoursPerDay),
				 QObject::tr("Skip rest of min hours problems"), QObject::tr("See next incompatibility min hours"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
			//////////
			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMinHoursDailyMinHours[tch]==-1 || teachersMinHoursDailyMinHours[tch]<tmd->minHoursDaily)
					teachersMinHoursDailyMinHours[tch]=tmd->minHoursDaily;
					teachersMinHoursDailyPercentages[tch]=100;
			}
		}
	}
	
	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		if(teachersMinHoursDailyPercentages[tc]==100){
			assert(teachersMinHoursDailyMinHours[tc]>=0);
			if(nHoursPerTeacher[tc]>0 && teachersMinHoursDailyMinHours[tc]>nHoursPerTeacher[tc]){
				ok=false;

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize, because you have constraint teacher min %1 hours daily for teacher"
				 " %2. This teacher has in total only %3 hours per week, so impossible constraint."
				 " Please correct and try again")
				 .arg(teachersMinHoursDailyMinHours[tc])
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 .arg(nHoursPerTeacher[tc])
				 ,
				 QObject::tr("Skip rest of min hours problems"), QObject::tr("See next incompatibility min hours"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}

			if(teachersMinHoursDailyMinHours[tc]<2){
				ok=false;

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize, because you have constraint teacher min %1 hours daily for teacher"
				 " %2. The number of min hours daily should be at least 2, to make a non-trivial constraint. Please correct and try again")
				 .arg(teachersMinHoursDailyMinHours[tc])
				 .arg(gt.rules.internalTeachersList[tc]->name)
				 ,
				 QObject::tr("Skip rest of min hours problems"), QObject::tr("See next incompatibility min hours"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
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

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING){
			ConstraintActivitiesNotOverlapping* no=(ConstraintActivitiesNotOverlapping*)gt.rules.internalTimeConstraintsList[i];

			for(int j=0; j<no->_n_activities; j++){
				int ai1=no->_activities[j];
				for(int k=0; k<no->_n_activities; k++){
					int ai2=no->_activities[k];
					if(ai1!=ai2){
						int t=activitiesNotOverlappingActivities[ai1].indexOf(ai2);
						if(t>=0){
							if(activitiesNotOverlappingPercentages[ai1].at(t) < no->weightPercentage){
								activitiesNotOverlappingPercentages[ai1][t]=no->weightPercentage;
							}
						}
						else{
							activitiesNotOverlappingPercentages[ai1].append(no->weightPercentage);
							activitiesNotOverlappingActivities[ai1].append(ai2);
						}
					}
				}
			}
		}
}

void computeActivitiesSameStartingTime()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		activitiesSameStartingTimeActivities[i].clear();
		activitiesSameStartingTimePercentages[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME){
			ConstraintActivitiesSameStartingTime* sst=(ConstraintActivitiesSameStartingTime*)gt.rules.internalTimeConstraintsList[i];
			
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
						
						if(t==-1 || perc<sst->weightPercentage){
							activitiesSameStartingTimeActivities[ai1].append(ai2);
							activitiesSameStartingTimePercentages[ai1].append(sst->weightPercentage);
							assert(activitiesSameStartingTimeActivities[ai1].count()==activitiesSameStartingTimePercentages[ai1].count());
						}
					}
				}
			}
		}
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
						
						if(t==-1 || perc<sst->weightPercentage){
							activitiesSameStartingHourActivities[ai1].append(ai2);
							activitiesSameStartingHourPercentages[ai1].append(sst->weightPercentage);
							assert(activitiesSameStartingHourActivities[ai1].count()==activitiesSameStartingHourPercentages[ai1].count());
						}
					}
				}
			}
		}
}

////////////teachers' no gaps
//important also for other purposes
bool computeNHoursPerTeacher()
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

			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize for teacher %1, because the number of hours for teacher is %2 "
			  " and you have only %3 days x %4 hours in a week. Probably you need to increase with 1 the number"
			  " of hours per day (probably you misunderstood FET hours notation)")
			 .arg(gt.rules.internalTeachersList[i]->name)
			 .arg(nHoursPerTeacher[i])
			 .arg(gt.rules.nDaysPerWeek)
			 .arg(gt.rules.nHoursPerDay),
			 QObject::tr("Skip rest of teachers problems"), QObject::tr("See next teacher problem"), QString(),
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

			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize for teacher %1, because the number of hours for teacher is %2 "
			  " and you have only %3 free slots from constraints teacher not available and/or break. Maybe you inputted wrong constraints teacher"
			  " not available or break or the number of hours per day is less with 1, because of a misunderstanding")
			 .arg(gt.rules.internalTeachersList[i]->name)
			 .arg(nHoursPerTeacher[i])
			 .arg(freeSlots),
			 QObject::tr("Skip rest of teach. not avail. problems"), QObject::tr("See next teach. not avail. problem"), QString(),
			 1, 0 );
		 	
			if(t==0)
				return ok;
		}
	}
	
	//n days per week has 100% weight
	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		if(teachersMaxDaysPerWeekMaxDays[i]>=0){
			int nd=teachersMaxDaysPerWeekMaxDays[i];
			if(nHoursPerTeacher[i] > nd*gt.rules.nHoursPerDay){
				ok=false;

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize for teacher %1, because the number of hours for teacher is %2"
				  " and you have only %3 allowed days from constraint teacher max days per week x %4 hours in a day."
				  " Probably there is an error in your data")
				 .arg(gt.rules.internalTeachersList[i]->name)
				 .arg(nHoursPerTeacher[i])
				 .arg(nd)
				 .arg(gt.rules.nHoursPerDay),
				 QObject::tr("Skip rest of teachers problems"), QObject::tr("See next teacher problem"), QString(),
				 1, 0 );
		 	
				if(t==0)
					return ok;
			}
		}
		
	//n days per week has 100% weight
	//check n days per week together with not available and breaks
	for(int tc=0; tc<gt.rules.nInternalTeachers; tc++){
		int nAllowedSlotsPerDay[MAX_DAYS_PER_WEEK];
		for(int d=0; d<gt.rules.nDaysPerWeek; d++){
			nAllowedSlotsPerDay[d]=0;
			for(int h=0; h<gt.rules.nHoursPerDay; h++)
				if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[tc][d][h])
					nAllowedSlotsPerDay[d]++;
		}

		int dayAvailable[MAX_DAYS_PER_WEEK];
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
			s=QObject::tr("Cannot optimize for teacher %1, because of too constrained"
			 " teacher max days per week, teacher not available and/or breaks."
			 " The number of total hours for this teacher is"
			 " %2 and the number of available slots is, considering max days per week and all other constraints, %3.")
			 .arg(gt.rules.internalTeachersList[tc]->name)
			 .arg(nHoursPerTeacher[tc])
			 .arg(total);
			s+="\n\n";
			s+=QObject::tr("Please modify your data accordingly and try again"
			 ". For more details, join the mailing list or email the author");
	
			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"), s,
			 QObject::tr("Skip rest of max hours problems"), QObject::tr("See next incompatibility max hours"), QString(),
			 1, 0 );
				 	
			if(t==0)
				return false;
		}
	}
	
	return ok;
}

bool computeTeachersMaxGapsPercentage()
{
	for(int j=0; j<gt.rules.nInternalTeachers; j++){
		teachersMaxGapsMaxGaps[j]=-1;
		teachersMaxGapsPercentage[j]=-1;
	}
	
	bool ok=true;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK){
			ConstraintTeachersMaxGapsPerWeek* tg=(ConstraintTeachersMaxGapsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize, because you have constraint teachers max gaps with"
				 " weight (percentage) below 100. Please make weight 100% and try again"),
				 QObject::tr("Skip rest of max gaps problems"), QObject::tr("See next incompatibility max gaps"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}

		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK){
			ConstraintTeacherMaxGapsPerWeek* tg=(ConstraintTeacherMaxGapsPerWeek*)gt.rules.internalTimeConstraintsList[i];

			if(tg->weightPercentage!=100){
				ok=false;

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize, because you have constraint teacher max gaps with"
				 " weight (percentage) below 100 for teacher %1. Please make weight 100% and try again")
				 .arg(tg->teacherName),
				 QObject::tr("Skip rest of max gaps problems"), QObject::tr("See next incompatibility max gaps"), QString(),
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
				if(teachersMaxGapsMaxGaps[j]==-1 
				 ||	teachersMaxGapsMaxGaps[j]>=0 && teachersMaxGapsMaxGaps[j]>=tg->maxGaps && teachersMaxGapsPercentage[j]<=tg->weightPercentage){
					teachersMaxGapsMaxGaps[j]=tg->maxGaps;
					teachersMaxGapsPercentage[j]=tg->weightPercentage;
				}
				else if(teachersMaxGapsMaxGaps[j]>=0 && teachersMaxGapsMaxGaps[j]<=tg->maxGaps && teachersMaxGapsPercentage[j]>=tg->weightPercentage){
					//do nothing
				}
				else{
					ok=false;

					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("Cannot optimize for teacher %1, because there are two constraints"
					 " of type max gaps relating to him, and the weight percentage is higher on the constraint"
					 " with more gaps allowed. You are allowed only to have for each teacher"
					 " the most important constraint with maximum weight percentage and minimum gaps allowed"
					 ". Please modify your data accordingly and try again"
					 ". For more details, join the mailing list or email the author")
					 .arg(gt.rules.internalTeachersList[j]->name),
					 QObject::tr("Skip rest of max gaps problems"), QObject::tr("See next incompatibility max gaps"), QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		}
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK){
			ConstraintTeacherMaxGapsPerWeek* tg=(ConstraintTeacherMaxGapsPerWeek*)gt.rules.internalTimeConstraintsList[i];
		
			int j=tg->teacherIndex;
			if(teachersMaxGapsMaxGaps[j]==-1 
			 ||	teachersMaxGapsMaxGaps[j]>=0 && teachersMaxGapsMaxGaps[j]>=tg->maxGaps && teachersMaxGapsPercentage[j]<=tg->weightPercentage){
				teachersMaxGapsMaxGaps[j]=tg->maxGaps;
				teachersMaxGapsPercentage[j]=tg->weightPercentage;
			}
			else if(teachersMaxGapsMaxGaps[j]>=0 && teachersMaxGapsMaxGaps[j]<=tg->maxGaps && teachersMaxGapsPercentage[j]>=tg->weightPercentage){
				//do nothing
			}
			else{
				ok=false;

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize for teacher %1, because there are two constraints"
				 " of type max gaps relating to him, and the weight percentage is higher on the constraint"
				 " with more gaps allowed. You are allowed only to have for each teacher"
				 " the most important constraint with maximum weight percentage and minimum gaps allowed"
				 ". Please modify your data accordingly and try again"
				 ". For more details, join the mailing list or email the author")
				 .arg(gt.rules.internalTeachersList[j]->name),
				 QObject::tr("Skip rest of max gaps problems"), QObject::tr("See next incompatibility max gaps"), QString(),
				 1, 0 );
		 	
				if(t==0)
					return false;
			}
		}
	}
	
	return ok;
}
/////////////////


///////students' no gaps and early (part 1)
//important also for other purposes
bool computeNHoursPerSubgroup()
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++)
		nHoursPerSubgroup[i]=0;
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		Activity* act=&gt.rules.internalActivitiesList[i];
		for(int j=0; j<act->iSubgroupsList.count(); j++){
			int isg=act->iSubgroupsList.at(j);
			nHoursPerSubgroup[isg]+=act->duration;
		}
	}
	
	bool ok=true;
	for(int i=0; i<gt.rules.nInternalSubgroups; i++)
		if(nHoursPerSubgroup[i]>gt.rules.nHoursPerWeek){
			ok=false;

			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize for subgroup %1, because the number of hours for subgroup is %2 "
			  " and you have only %3 days x %4 hours in a week. Probably you need to increase with 1 the number"
			  " of hours per day (probably you misunderstood FET hours notation)")
			 .arg(gt.rules.internalSubgroupsList[i]->name)
			 .arg(nHoursPerSubgroup[i])
			 .arg(gt.rules.nDaysPerWeek)
			 .arg(gt.rules.nHoursPerDay),
			 QObject::tr("Skip rest of students problems"), QObject::tr("See next students problem"), QString(),
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

			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize for subgroup %1, because the number of hours for subgroup is %2 "
			  " and you have only %3 free slots from constraints students set not available and/or break. Maybe you inputted wrong constraints students set"
			  " not available or break or the number of hours per day is less with 1, because of a misunderstanding")
			 .arg(gt.rules.internalSubgroupsList[i]->name)
			 .arg(nHoursPerSubgroup[i])
			 .arg(freeSlots),
			 QObject::tr("Skip rest of stud. not avail. problems"), QObject::tr("See next stud. not avail. problem"), QString(),
			 1, 0 );
		 	
			if(t==0)
				return ok;
		}
	}
		
	return ok;
}


/*void computeSharedSubgroups()
{
	assert(gt.rules.internalStructureComputed);
	
	for(int ai1=0; ai1<gt.rules.nInternalActivities; ai1++){
		Activity* act1=&gt.rules.internalActivitiesList[ai1];
		for(int ai2=0; ai2<gt.rules.nInternalActivities; ai2++){
			Activity* act2=&gt.rules.internalActivitiesList[ai2];
	
			sharedSubgroups[ai1][ai2].clear();
			
			for(int i1=0; i1<act1->nSubgroups; i1++){
				int isg1=act1->subgroups[i1];
				
				for(int i2=0; i2<act2->nSubgroups; i2++){
					int isg2=act1->subgroups[i2];
				
					if(isg1==isg2)
						sharedSubgroups[ai1][ai2].insert(isg1);
				}
			}
		}
	}
}*/


bool computeMaxDaysPerWeekForTeachers()
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

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize, because you have constraint teacher max days per week with"
				 " weight (percentage) below 100 for teacher %1. Starting with FET version 5.2.17 it is only possible"
				 " to use 100% weight for such constraints. Please make weight 100% and try again")
				 .arg(tn->teacherName),
				 QObject::tr("Skip rest of max days problems"), QObject::tr("See next incompatibility max days"), QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}

			if(teachersMaxDaysPerWeekMaxDays[tn->teacher_ID]==-1){
				teachersMaxDaysPerWeekMaxDays[tn->teacher_ID]=tn->maxDaysPerWeek;
				teachersMaxDaysPerWeekWeightPercentages[tn->teacher_ID]=tn->weightPercentage;
			}
			else{
				ok=false;
				
				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize for teacher %1, because it has at least two constraints max days per week"
				 ". Please modify your data correspondingly (leave maximum one constraint of type"
				 " constraint teacher max days per week for each teacher) and try again")
				 .arg(gt.rules.internalTeachersList[tn->teacher_ID]->name),
				 QObject::tr("Skip rest of max days per week problems"), QObject::tr("See next incompatibility max days per week"), QString(),
				 1, 0 );
			 
				if(t==0)
					break;
			}
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

bool computeSubgroupsEarlyAndNoGapsPercentages() //st no gaps & early - part 2
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsEarlyPercentage[i]=-1;
		subgroupsNoGapsPercentage[i]=-1;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		//students early
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_EARLY){
			ConstraintStudentsEarly* se=(ConstraintStudentsEarly*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<gt.rules.nInternalSubgroups; j++)
				if(subgroupsEarlyPercentage[j] < se->weightPercentage)
					subgroupsEarlyPercentage[j] = se->weightPercentage;
		}

		//students set early
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_EARLY){
			ConstraintStudentsSetEarly* se=(ConstraintStudentsSetEarly*) gt.rules.internalTimeConstraintsList[i];
			for(int q=0; q<se->iSubgroupsList.count(); q++){
			//for(int j=0; j<gt.rules.nInternalSubgroups; j++)
				int j=se->iSubgroupsList.at(q);
				if(subgroupsEarlyPercentage[j] < se->weightPercentage)
					subgroupsEarlyPercentage[j] = se->weightPercentage;
			}
		}

		//students no gaps
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_NO_GAPS){
			ConstraintStudentsNoGaps* sg=(ConstraintStudentsNoGaps*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<gt.rules.nInternalSubgroups; j++)
				if(subgroupsNoGapsPercentage[j] < sg->weightPercentage)
					subgroupsNoGapsPercentage[j] = sg->weightPercentage;
		}

		//students set no gaps
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_NO_GAPS){
			ConstraintStudentsSetNoGaps* sg=(ConstraintStudentsSetNoGaps*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<sg->iSubgroupsList.count(); j++){
				int s=sg->iSubgroupsList.at(j);
				if(subgroupsNoGapsPercentage[s] < sg->weightPercentage)
					subgroupsNoGapsPercentage[s] = sg->weightPercentage;
			}
		}
	}
	
	bool ok=true;
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		bool oksubgroup;
		if(subgroupsNoGapsPercentage[i]== -1 && subgroupsEarlyPercentage[i]==-1 ||
			subgroupsNoGapsPercentage[i]>=0 && subgroupsEarlyPercentage[i]==-1 ||
			subgroupsNoGapsPercentage[i]>=0 && subgroupsEarlyPercentage[i]>=0 && subgroupsNoGapsPercentage[i]==subgroupsEarlyPercentage[i])
				oksubgroup=true;
		else
			oksubgroup=false;
			
		if(subgroupsNoGapsPercentage[i]>=0 && subgroupsNoGapsPercentage[i]!=100){
			oksubgroup=false;
			ok=false;

			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize for subgroup %1, because you have a no gaps constraint"
			 " with weight percentage less than 100%. Currently, the algorithm can only"
			 " optimize with not existing constraint no gaps or existing with 100% weight for it"
			 ". Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name),
			 QObject::tr("Skip rest of early - no gaps problems"), QObject::tr("See next incompatibility no gaps - early"), QString(),
			 1, 0 );
			 
			if(t==0)
				break;
		}
		if(subgroupsEarlyPercentage[i]>=0 && subgroupsEarlyPercentage[i]!=100){
			oksubgroup=false;
			ok=false;

			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize for subgroup %1, because you have an early constraint"
			 " with weight percentage less than 100%. Currently, the algorithm can only"
			 " optimize with not existing constraint early or existing with 100% weight for it"
			 ". Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name),
			 QObject::tr("Skip rest of early - no gaps problems"), QObject::tr("See next incompatibility no gaps - early"), QString(),
			 1, 0 );
			 
			if(t==0)
				break;
		}
		if(subgroupsNoGapsPercentage[i]== -1 && subgroupsEarlyPercentage[i]>=0){
			oksubgroup=false;
			ok=false;

			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize for subgroup %1, because a students early constraint"
			 " exists for this subgroup, and you have not 'no gaps' requirements for this subgroup. "
			 "The algorithm can 1. optimize with 'early' and 'no gaps'"
			 " having the same weight percentage or 2. only 'no gaps' optimization"
			 " without 'early'. Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name),
			 QObject::tr("Skip rest of early - no gaps problems"), QObject::tr("See next incompatibility no gaps - early"), QString(),
			 1, 0 );
			 
			if(t==0)
				break;
		}
		else if(subgroupsNoGapsPercentage[i]>=0 && subgroupsEarlyPercentage[i]>=0 
		 && subgroupsNoGapsPercentage[i]!=subgroupsEarlyPercentage[i]){
		 	oksubgroup=false;
			ok=false;

			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize for subgroup %1, because early constraint"
			 " has weight percentage %2, and 'no gaps' constraint has weight percentage %3."
			 ". The algorithm can:"
			 "\n1: Optimize with 'early' and 'no gaps' having the same weight percentage or"
			 "\n2. Only 'no gaps' optimization without 'early'."
			 "\nPlease modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name)
			 .arg(subgroupsEarlyPercentage[i]).
			 arg(subgroupsNoGapsPercentage[i]),
			 QObject::tr("Skip rest of early - no gaps problems"), QObject::tr("See next incompatibility no gaps - early"), QString(),
			 1, 0 );
			 
			if(t==0)
				break;
		}
		else{
			oksubgroup=true;
		}
		
		if(!oksubgroup)
			ok=false;
	}
	
	return ok;
}

bool computeAllowedTimesPercentages()
{
	bool ok=true;

	assert(gt.rules.internalStructureComputed);

	//BREAK
	for(int j=0; j<gt.rules.nHoursPerWeek; j++)
		breakTime[j]=false;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int k=0; k<gt.rules.nHoursPerDay; k++)
			breakDayHour[j][k]=false;

	//STUDENTS SET NOT AVAILABLE
	for(int i=0; i<gt.rules.nInternalSubgroups; i++)
		for(int j=0; j<gt.rules.nHoursPerWeek; j++)
			subgroupNotAvailableTime[i][j]=false;
	for(int i=0; i<gt.rules.nInternalSubgroups; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			for(int k=0; k<gt.rules.nHoursPerDay; k++)
				subgroupNotAvailableDayHour[i][j][k]=false;	
	
	//TEACHER NOT AVAILABLE
	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		for(int j=0; j<gt.rules.nHoursPerWeek; j++)
			teacherNotAvailableTime[i][j]=false;
	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			for(int k=0; k<gt.rules.nHoursPerDay; k++)
				teacherNotAvailableDayHour[i][j][k]=false;	
	
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		for(int j=0; j<gt.rules.nHoursPerWeek; j++)
			allowedTimesPercentages[i][j]=-1;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
			//TEACHER not available
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_NOT_AVAILABLE){
				ConstraintTeacherNotAvailable* tn=(ConstraintTeacherNotAvailable*)gt.rules.internalTimeConstraintsList[i];
				for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(int ti=0; ti<act->iTeachersList.count(); ti++)
						if(act->iTeachersList.at(ti)==tn->teacher_ID){
							for(int h=max(0, tn->h1 - act->duration + 1); h<tn->h2; h++){
								if(allowedTimesPercentages[ai][tn->d+h*gt.rules.nDaysPerWeek]<tn->weightPercentage)
									allowedTimesPercentages[ai][tn->d+h*gt.rules.nDaysPerWeek]=tn->weightPercentage;
							}
							break; //search no more for teacher
						}
				}

				if(tn->weightPercentage!=100){
					ok=false;

					QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("Cannot optimize, because you have constraints of type "
					 "teacher not available with weight percentage less than 100\% for teacher %1. Currently, FET can only optimize with "
					 "constraints teacher not available with 100\% weight (or no constraint). Please "
					 "modify your data accordingly and try again.").arg(tn->teacherName));
			
					return ok;
				}
				else{				
					assert(tn->weightPercentage==100);
					for(int h=tn->h1; h<tn->h2; h++){
						teacherNotAvailableTime[tn->teacher_ID][tn->d+h*gt.rules.nDaysPerWeek]=true;
						teacherNotAvailableDayHour[tn->teacher_ID][tn->d][h]=true;
					}
				}
			}

			//STUDENTS SET not available		
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE){
				ConstraintStudentsSetNotAvailable* sn=(ConstraintStudentsSetNotAvailable*)gt.rules.internalTimeConstraintsList[i];
				for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(int sg=0; sg<act->iSubgroupsList.count(); sg++)
						for(int j=0; j<sn->iSubgroupsList.count(); j++){
							if(act->iSubgroupsList.at(sg)==sn->iSubgroupsList.at(j)){
								for(int h=max(0, sn->h1 - act->duration + 1); h<sn->h2; h++){
									if(allowedTimesPercentages[ai][sn->d+h*gt.rules.nDaysPerWeek]<sn->weightPercentage)
										allowedTimesPercentages[ai][sn->d+h*gt.rules.nDaysPerWeek]=sn->weightPercentage;
								}
								//break; //search no more for subgroup - this can bring an improvement in precalculation
								//but needs attention
							}
						}
				}

				if(sn->weightPercentage!=100){
					ok=false;

					QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("Cannot optimize, because you have constraints of type "
					 "students set not available with weight percentage less than 100\% for students set %1. Currently, FET can only optimize with "
					 "constraints students set not available with 100\% weight (or no constraint). Please "
					 "modify your data accordingly and try again.").arg(sn->students));
			
					return ok;
				}
				else{				
					assert(sn->weightPercentage==100);
					for(int q=0; q<sn->iSubgroupsList.count(); q++){
						int ss=sn->iSubgroupsList.at(q);
						for(int h=sn->h1; h<sn->h2; h++){
							subgroupNotAvailableTime[ss][sn->d+h*gt.rules.nDaysPerWeek]=true;
							subgroupNotAvailableDayHour[ss][sn->d][h]=true;
						}
					}
				}
			}
			
			//BREAK
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_BREAK){
				ConstraintBreak* br=(ConstraintBreak*)gt.rules.internalTimeConstraintsList[i];
				for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(int h=max(0, br->h1 - act->duration + 1); h<br->h2; h++){
						if(allowedTimesPercentages[ai][br->d+h*gt.rules.nDaysPerWeek]<br->weightPercentage)
							allowedTimesPercentages[ai][br->d+h*gt.rules.nDaysPerWeek]=br->weightPercentage;
					}
				}

				if(br->weightPercentage!=100){
					ok=false;

					QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("Cannot optimize, because you have constraints of type "
					 "break with weight percentage less than 100\%. Currently, FET can only optimize with "
					 "constraints break with 100\% weight (or no constraint). Please "
					 "modify your data accordingly and try again."));
			
					return ok;
				}
				else{				
					assert(br->weightPercentage==100);
					for(int h=br->h1; h<br->h2; h++){
						breakTime[br->d+h*gt.rules.nDaysPerWeek]=true;
						breakDayHour[br->d][h]=true;
					}
				}
			}

			//ACTIVITY preferred time
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITY_PREFERRED_TIME){
				ConstraintActivityPreferredTime* ap=(ConstraintActivityPreferredTime*)gt.rules.internalTimeConstraintsList[i];
				
				if(ap->day>=0 && ap->hour>=0){
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						for(int h=0; h<gt.rules.nHoursPerDay; h++)
							if(d!=ap->day || h!=ap->hour)
								if(allowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]<ap->weightPercentage)
									allowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]=ap->weightPercentage;
				}
				else if(ap->day>=0){
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						for(int h=0; h<gt.rules.nHoursPerDay; h++)
							if(d!=ap->day)
								if(allowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]<ap->weightPercentage)
									allowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]=ap->weightPercentage;
				}
				else if(ap->hour>=0){
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						for(int h=0; h<gt.rules.nHoursPerDay; h++)
							if(h!=ap->hour)
								if(allowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]<ap->weightPercentage)
									allowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]=ap->weightPercentage;
				}
				else{
					ok=false;

					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("Cannot optimize, because you have constraints of type "
					 "activity preferred time with no day nor hour selected (for activity with id==%1). "
					 "Please modify your data accordingly (remove or edit constraint) and try again.")
					 .arg(gt.rules.internalActivitiesList[ap->activityIndex].id),
					 QObject::tr("Skip rest of constraints problems"), QObject::tr("See next incorrect constraint"), QString(),
					1, 0 );

					if(t==0)
						break;
					//assert(0);
				}
			}	

			//ACTIVITY preferred times
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITY_PREFERRED_TIMES){
				ConstraintActivityPreferredTimes* ap=(ConstraintActivityPreferredTimes*)gt.rules.internalTimeConstraintsList[i];
				
				int ai=ap->activityIndex;
				
				bool allowed[MAX_HOURS_PER_WEEK];
				for(int k=0; k<gt.rules.nHoursPerWeek; k++)
					allowed[k]=false;
						
				for(int m=0; m<ap->nPreferredTimes; m++){
					int d=ap->days[m];
					int h=ap->hours[m];
					
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
						if(allowedTimesPercentages[ai][k] < ap->weightPercentage)
							allowedTimesPercentages[ai][k] = ap->weightPercentage;
			}
			
			//ACTIVITIES preferred times
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITIES_PREFERRED_TIMES){
				ConstraintActivitiesPreferredTimes* ap=(ConstraintActivitiesPreferredTimes*)gt.rules.internalTimeConstraintsList[i];
				
				for(int j=0; j<ap->nActivities; j++){
					int ai=ap->activitiesIndices[j];
					
					bool allowed[MAX_HOURS_PER_WEEK];
					for(int k=0; k<gt.rules.nHoursPerWeek; k++)
						allowed[k]=false;
						
					for(int m=0; m<ap->nPreferredTimes; m++){
						int d=ap->days[m];
						int h=ap->hours[m];
						assert(d>=0 && h>=0);
						allowed[d+h*gt.rules.nDaysPerWeek]=true;
					}
					
					for(int k=0; k<gt.rules.nHoursPerWeek; k++)
						if(!allowed[k])
							if(allowedTimesPercentages[ai][k] < ap->weightPercentage)
								allowedTimesPercentages[ai][k] = ap->weightPercentage;
				}
			}
	}
	
	return ok;
}

void computeMinNDays()
{
	for(int j=0; j<gt.rules.nInternalActivities; j++){
		minNDaysListOfActivities[j].clear();
		minNDaysListOfMinDays[j].clear();
		minNDaysListOfConsecutiveIfSameDay[j].clear();
		minNDaysListOfWeightPercentages[j].clear();
				
		//for(int k=0; k<gt.rules.nInternalActivities; k++)
		//	minNDays[j][k]=0;
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES
		 /*&&gt.rules.internalTimeConstraintsList[i]->compulsory==true*/){
			ConstraintMinNDaysBetweenActivities* md=
			 (ConstraintMinNDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
			
			for(int j=0; j<md->_n_activities; j++){
				int ai1=md->_activities[j];
				for(int k=0; k<md->_n_activities; k++)
					if(j!=k){
						int ai2=md->_activities[k];
						int m=md->minDays;
						/*if(m>minNDays[ai1][ai2])
							minNDays[ai1][ai2]=minNDays[ai2][ai1]=m;*/
						
						minNDaysListOfActivities[ai1].append(ai2);
						minNDaysListOfMinDays[ai1].append(m);
						assert(md->weightPercentage >=0 && md->weightPercentage<=100);
						minNDaysListOfWeightPercentages[ai1].append(md->weightPercentage);
						minNDaysListOfConsecutiveIfSameDay[ai1].append(md->consecutiveIfSameDay);
					}
			}
		}

	/*for(int j=0; j<gt.rules.nInternalActivities; j++)
		for(int k=0; k<gt.rules.nInternalActivities; k++)
			if(minNDays[j][k]>0){
				assert(j!=k);
				minNDaysListOfActivities[j].append(k);
				minNDaysListOfMinDays[j].append(minNDays[j][k]);
			}*/
}

bool computeActivitiesConflictingPercentage()
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
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Cannot optimize, because you have no basic time constraints or its weight is lower than 100.0%. "
		 "Please add a basic time constraint (100% weight)"));
		return false;
	}
	
	assert(m>=0 && m<=100);
	assert(m==100);

	//compute conflictig
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		for(int j=0; j<gt.rules.nInternalActivities; j++)
			activitiesConflictingPercentage[i][j]=-1;
		
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		activitiesConflictingPercentage[i][i]=100;

	QProgressDialog progress(NULL);
	progress.setLabelText(QObject::tr("Precomputing ... please wait"));
	progress.setRange(0, gt.rules.nInternalTeachers+gt.rules.nInternalSubgroups);
	progress.setModal(true);
	
	int ttt=0;
	
	for(int t=0; t<gt.rules.nInternalTeachers; t++){
		progress.setValue(ttt);
		pqapplication->processEvents();
		if(progress.wasCanceled()){
			QMessageBox::information(NULL, QObject::tr("FET information"), QObject::tr("Canceled"));
			return false;
		}
		
		ttt++;
		
		foreach(int i, gt.rules.internalTeachersList[t]->activitiesForTeacher)
			foreach(int j, gt.rules.internalTeachersList[t]->activitiesForTeacher)
				activitiesConflictingPercentage[i][j]=100;
	}
	
	for(int s=0; s<gt.rules.nInternalSubgroups; s++){
		progress.setValue(ttt);
		pqapplication->processEvents();
		if(progress.wasCanceled()){
			QMessageBox::information(NULL, QObject::tr("FET information"), QObject::tr("Canceled"));
			return false;
		}
		
		ttt++;
		
		foreach(int i, gt.rules.internalSubgroupsList[s]->activitiesForSubgroup)
			foreach(int j, gt.rules.internalSubgroupsList[s]->activitiesForSubgroup)
				activitiesConflictingPercentage[i][j]=100;
	}
	
	return true;
}

//old
#if 0
bool computeActivitiesConflictingPercentage()
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
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Cannot optimize, because you have no basic time constraints or its weight is lower than 100.0%. "
		 "Please add a basic time constraint (100% weight)"));
		return false;
	}
	
	assert(m>=0 && m<=100);
	assert(m==100);

	//compute conflictig
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		activitiesConflictingPercentage[i][i]=100;
		
	QProgressDialog progress(NULL);
	progress.setLabelText(QObject::tr("Precomputing ... please wait"));
	progress.setRange(0, gt.rules.nInternalActivities*(gt.rules.nInternalActivities-1)/2);
	progress.setModal(true);
	
	int ttt=0;
		
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		progress.setValue(ttt);
		pqapplication->processEvents();
		if(progress.wasCanceled()){
			QMessageBox::information(NULL, QObject::tr("FET information"), QObject::tr("Canceled"));
			return false;
		}
			
		for(int j=i+1; j<gt.rules.nInternalActivities; j++){
			ttt++;
		
			//see if they share a teacher
			bool shareTeacher=false;
			foreach(QString tni, gt.rules.internalActivitiesList[i].teachersNames){
				foreach(QString tnj, gt.rules.internalActivitiesList[j].teachersNames){
					if(tni==tnj){
						shareTeacher=true;
						break;
					}
				}
				if(shareTeacher)
					break;
			}
			
			bool shareStudentsSet=false;
			foreach(QString ssi, gt.rules.internalActivitiesList[i].studentsNames){
				foreach(QString ssj, gt.rules.internalActivitiesList[j].studentsNames){
					if(gt.rules.studentsSetsRelated(ssi, ssj)){
						shareStudentsSet=true;
						break;
					}
				}
				if(shareStudentsSet)
					break;
			}
			
			if(shareTeacher||shareStudentsSet)
				activitiesConflictingPercentage[i][j]=activitiesConflictingPercentage[j][i]=100;
			else
				activitiesConflictingPercentage[i][j]=activitiesConflictingPercentage[j][i]=-1;
		}
	}
		
	return true;
}
#endif
//endif 0

void computeConstr2ActivitiesConsecutive()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		constr2ActivitiesConsecutivePercentages[i].clear();
		constr2ActivitiesConsecutiveActivities[i].clear();

		inverseConstr2ActivitiesConsecutivePercentages[i].clear();
		inverseConstr2ActivitiesConsecutiveActivities[i].clear();
	}

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_2_ACTIVITIES_CONSECUTIVE){
			Constraint2ActivitiesConsecutive* c2=(Constraint2ActivitiesConsecutive*)gt.rules.internalTimeConstraintsList[i];
			
			int fai=c2->firstActivityIndex;
			int sai=c2->secondActivityIndex;
			
			//direct
			int j=constr2ActivitiesConsecutiveActivities[fai].indexOf(sai); 
			if(j==-1){
				constr2ActivitiesConsecutiveActivities[fai].append(sai);
				constr2ActivitiesConsecutivePercentages[fai].append(c2->weightPercentage);
			}
			else if(j>=0 && constr2ActivitiesConsecutivePercentages[fai].at(j)<c2->weightPercentage){
				constr2ActivitiesConsecutivePercentages[fai][j]=c2->weightPercentage;
			}

			//inverse
			j=inverseConstr2ActivitiesConsecutiveActivities[sai].indexOf(fai); 
			if(j==-1){
				inverseConstr2ActivitiesConsecutiveActivities[sai].append(fai);
				inverseConstr2ActivitiesConsecutivePercentages[sai].append(c2->weightPercentage);
			}
			else if(j>=0 && inverseConstr2ActivitiesConsecutivePercentages[sai].at(j)<c2->weightPercentage){
				inverseConstr2ActivitiesConsecutivePercentages[sai][j]=c2->weightPercentage;
			}
		}
}

bool computeActivityEndsStudentsDayPercentages()
{
	bool ok=true;

	for(int ai=0; ai<gt.rules.nInternalActivities; ai++)
		activityEndsStudentsDayPercentages[ai]=-1;
		
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY){
			ConstraintActivityEndsStudentsDay* cae=(ConstraintActivityEndsStudentsDay*)gt.rules.internalTimeConstraintsList[i];
			
			if(cae->weightPercentage!=100){
				ok=false;

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize, because you have constraints of type "
				 "activity activity ends students day for activity with id==%1 with weight percentage under 100%. "
				 "Constraint activity ends students day can only have weight percentage 100%. "
				 "Please modify your data accordingly (remove or edit constraint) and try again.")
				 .arg(gt.rules.internalActivitiesList[cae->activityIndex].id),
				 QObject::tr("Skip rest of constraints problems"), QObject::tr("See next incorrect constraint"), QString(),
				1, 0 );
				
				if(t==0)
					break;
			}
			
			int ai=cae->activityIndex;
			if(activityEndsStudentsDayPercentages[ai] < cae->weightPercentage)
				activityEndsStudentsDayPercentages[ai] = cae->weightPercentage;
		}
		
	return ok;
}

bool checkMinNDays100Percent()
{
	bool ok=true;
	
	int daysTeacherIsAvailable[MAX_TEACHERS];

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
	}

	int daysSubgroupIsAvailable[MAX_TOTAL_SUBGROUPS];

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
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES
		 &&gt.rules.internalTimeConstraintsList[i]->weightPercentage==100.0){
			ConstraintMinNDaysBetweenActivities* md=(ConstraintMinNDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];			
			if(md->minDays>=1){
				int requestedDaysForTeachers[MAX_TEACHERS];
				for(int tc=0; tc<gt.rules.nInternalTeachers; tc++)
					requestedDaysForTeachers[tc]=0;
				int requestedDaysForSubgroups[MAX_TOTAL_SUBGROUPS];
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
						
						QString s=QObject::tr("Constraint %1 cannot be respected because teacher %2 has at most"
						 " %3 available days from teacher not available, breaks and teacher max days per week."
						 " Please lower the weight of this constraint to a value below 100% (it depends"
						 " on your situation, if 0% is too little, make it 90%, 95% or even 99.75%."
						 " Even a large weight should not slow down much the program."
						 " A situation where you may need to make it larger than 0% is for instance if you have 5 activities with 4"
						 " possible days. You want to spread them 1, 1, 1 and 2, not 2, 2 and 1)"
						)
						 .arg(md->getDetailedDescription(gt.rules))
						 .arg(gt.rules.internalTeachersList[tc]->name)
						 .arg(daysTeacherIsAvailable[tc]);

						int t=QMessageBox::warning(NULL, QObject::tr("FET warning"), s, 
						 QObject::tr("Skip rest of min n days problems"), QObject::tr("See next incorrect constraint"), QString(),
						 1, 0 );
					
						if(t==0)
							return ok;
					}
				for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++)
					if(requestedDaysForSubgroups[sb]>daysSubgroupIsAvailable[sb]){
						ok=false;
						
						QString s=QObject::tr("Constraint %1 cannot be respected because subgroup %2 has at most"
						 " %3 available days from students set not available and breaks."
						 " Please lower the weight of this constraint to a value below 100% (it depends"
						 " on your situation, if 0% is too little, make it 90%, 95% or even 99.75%."
						 " Even a large weight should not slow down much the program."
						 " A situation where you may need to make it larger than 0% is for instance if you have 5 activities with 4"
						 " possible days. You want to spread them 1, 1, 1 and 2, not 2, 2 and 1)"
						 )
						 .arg(md->getDetailedDescription(gt.rules))
						 .arg(gt.rules.internalSubgroupsList[sb]->name)
						 .arg(daysSubgroupIsAvailable[sb]);

						int t=QMessageBox::warning(NULL, QObject::tr("FET warning"), s, 
						 QObject::tr("Skip rest of min n days problems"), QObject::tr("See next incorrect constraint"), QString(),
						 1, 0 );
					
						if(t==0)
							return ok;
					}
			}
		}
	}
	
	return ok;
}
	
bool checkMinNDaysConsecutiveIfSameDay()
{
	bool ok=true;
	
	int daysTeacherIsAvailable[MAX_TEACHERS];

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
	}

	int daysSubgroupIsAvailable[MAX_TOTAL_SUBGROUPS];

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
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES){
			ConstraintMinNDaysBetweenActivities* md=(ConstraintMinNDaysBetweenActivities*)gt.rules.internalTimeConstraintsList[i];
			if(md->consecutiveIfSameDay){
				int nReqForTeacher[MAX_TEACHERS];				
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
						
						QString s=QObject::tr("Constraint %1 cannot be respected because teacher %2 has at most"
						 " %3 available days. You specified for this constraint consecutive if same day=true."
						 " Currently FET cannot put more than 2 activities in the same day"
						 " if consecutive if same day is true. You have 2*available days<number of activities in this constraint."
						 " This is a very unlikely situation, that is why I didn't care too much about it."
						 " If you encounter it, please please modify your file (uncheck consecutive if same day"
						 " or add other activities with larger duration) or contact author/mailing list."
						)
						 .arg(md->getDetailedDescription(gt.rules))
						 .arg(gt.rules.internalTeachersList[tc]->name)
						 .arg(daysTeacherIsAvailable[tc]);
	
						int t=QMessageBox::warning(NULL, QObject::tr("FET warning"), s, 
						 QObject::tr("Skip rest of min n days problems"), QObject::tr("See next incorrect constraint"), QString(),
						 1, 0 );
						
						if(t==0)
							return ok;
					}	
				}

				int nReqForSubgroup[MAX_TOTAL_SUBGROUPS];				
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
						
						QString s=QObject::tr("Constraint %1 cannot be respected because subgroup %2 has at most"
						 " %3 available days. You specified for this constraint consecutive if same day=true."
						 " Currently FET cannot put more than 2 activities in the same day"
						 " if consecutive if same day is true. You have 2*available days<number of activities in this constraint."
						 " This is a very unlikely situation, that is why I didn't care too much about it."
						 " If you encounter it, please modify your file (uncheck consecutive if same day"
						 " or add other activities with larger duration) or contact author/mailing list."
						)
						 .arg(md->getDetailedDescription(gt.rules))
						 .arg(gt.rules.internalSubgroupsList[sb]->name)
						 .arg(daysSubgroupIsAvailable[sb]);

						int t=QMessageBox::warning(NULL, QObject::tr("FET warning"), s, 
						 QObject::tr("Skip rest of min n days problems"), QObject::tr("See next incorrect constraint"), QString(),
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
	
/*void Rules::computeActivitiesSimilar()
{
	for(int i=0; i<this->nInternalActivities; i++)
		this->activitiesSimilar[i][i]=true;
	for(int i=0; i<this->nInternalActivities; i++)
		for(int j=i+1; j<this->nInternalActivities; j++){
			bool sameDuration=false;
			if(this->internalActivitiesList[i].duration==this->internalActivitiesList[j].duration)
				sameDuration=true;

			if(!sameDuration)
				continue;
	
			//see if they share all teachers
			bool sameTeachers=true;
			foreach(QString tni, this->internalActivitiesList[i].teachersNames){
				bool shareTeacher=false;
				foreach(QString tnj, this->internalActivitiesList[j].teachersNames){
					if(tni==tnj){
						shareTeacher=true;
						break;
					}
				}
				if(!shareTeacher){
					sameTeachers=false;
					break;
				}
			}
			if(sameTeachers)
				foreach(QString tnj, this->internalActivitiesList[j].teachersNames){
					bool shareTeacher=false;
					foreach(QString tni, this->internalActivitiesList[i].teachersNames){
						if(tni==tnj){
							shareTeacher=true;
							break;
						}
					}
					if(!shareTeacher){
						sameTeachers=false;
						break;
					}
				}
			
			if(!sameTeachers)
				continue;
			
			bool sameStudentsSets=true;
			foreach(QString ssi, this->internalActivitiesList[i].studentsNames){
				bool shareStudentsSet=false;
				foreach(QString ssj, this->internalActivitiesList[j].studentsNames){
					if(studentsSetsRelated(ssi, ssj)){
						shareStudentsSet=true;
						break;
					}
				}
				if(!shareStudentsSet){
					sameStudentsSets=false;
					break;
				}
			}
			if(sameStudentsSets)
				foreach(QString ssj, this->internalActivitiesList[j].studentsNames){
					bool shareStudentsSet=false;
					foreach(QString ssi, this->internalActivitiesList[i].studentsNames){
						if(studentsSetsRelated(ssi, ssj)){
							shareStudentsSet=true;
							break;
						}
					}
					if(!shareStudentsSet){
						sameStudentsSets=false;
						break;
					}
				}
			
			if(sameTeachers && sameStudentsSets && sameDuration)
				this->activitiesSimilar[i][j]=this->activitiesSimilar[j][i]=true;
			else
				this->activitiesSimilar[i][j]=this->activitiesSimilar[j][i]=false;
		}
		
	bool visited[MAX_ACTIVITIES];
	for(int i=0; i<nInternalActivities; i++)
		visited[i]=false;
	for(int i=0; i<nInternalActivities; i++){
		if(visited[i])
			continue;
		visited[i]=true;
		cout<<"Similar activities for activity with index "<<i<<": ";
		for(int j=0; j<nInternalActivities; j++)
			if(i!=j && activitiesSimilar[i][j]){
				cout<<j<<" ";
				visited[j]=true;
			}
		cout<<endl;
	}
}*/
	
/*void Rules::computeActivitiesContained()
{
	for(int i=0; i<this->nInternalActivities; i++)
		this->activityContained[i][i]=true;
	for(int i=0; i<this->nInternalActivities; i++)
		for(int j=0; j<this->nInternalActivities; j++){
			if(i==j)
				continue;
				
			bool containedDuration=false;
			if(this->internalActivitiesList[j].duration>=this->internalActivitiesList[i].duration)
				containedDuration=true;

			if(!containedDuration)
				continue;
	
			//see if j has at least all the teachers of i
			bool containedTeachers=true;
			foreach(QString tni, this->internalActivitiesList[i].teachersNames){
				bool shareTeacher=false;
				foreach(QString tnj, this->internalActivitiesList[j].teachersNames){
					if(tni==tnj){
						shareTeacher=true;
						break;
					}
				}
				if(!shareTeacher){
					containedTeachers=false;
					break;
				}
			}
			
			if(!containedTeachers)
				continue;
			
			bool containedStudentsSets=true;
			foreach(QString ssi, this->internalActivitiesList[i].studentsNames){
				bool shareStudentsSet=false;
				foreach(QString ssj, this->internalActivitiesList[j].studentsNames){
					if(studentsSetsRelated(ssi, ssj)){
						shareStudentsSet=true;
						break;
					}
				}
				if(!shareStudentsSet){
					containedStudentsSets=false;
					break;
				}
			}
			
			if(containedTeachers && containedStudentsSets && containedDuration)
				this->activityContained[i][j]=true;
			else
				this->activityContained[i][j]=false;
		}
		
	for(int i=0; i<nInternalActivities; i++){
		cout<<"Contained activities for activity with index "<<i<<": ";
		for(int j=0; j<nInternalActivities; j++)
			if(i!=j && activityContained[j][i]){
				cout<<j<<" ";
			}
		cout<<endl;
	}
}*/

bool computeBasicSpace()
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
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Cannot optimize, because you have no basic space constraints or its weight percentage is lower than 100.0%. "
		 "Please add a basic space constraint with 100% weight"));
		return false;
	}
	return ok;
}

bool computeAllowedRoomTimePercentages()
{
	for(int i=0; i<gt.rules.nInternalRooms; i++)
		for(int j=0; j<gt.rules.nHoursPerWeek; j++)
			allowedRoomTimePercentages[i][j]=-1;

	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ROOM_NOT_AVAILABLE){
			ConstraintRoomNotAvailable* rn=(ConstraintRoomNotAvailable*)gt.rules.internalSpaceConstraintsList[i];
			for(int h=rn->h1; h<rn->h2; h++){
				if(allowedRoomTimePercentages[rn->room_ID][rn->d+h*gt.rules.nDaysPerWeek]<rn->weightPercentage)
					allowedRoomTimePercentages[rn->room_ID][rn->d+h*gt.rules.nDaysPerWeek]=rn->weightPercentage;
			}
		}
	}
	
	return true;
}

bool computeActivitiesRoomsPreferences()
{
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		unspecifiedRoom[i]=true;
		activitiesPreferredRoomsPreferredRooms[i].clear();
		activitiesPreferredRoomsPercentage[i]=-1;
	}
	
	for(int i=0; i<gt.rules.nInternalSpaceConstraints; i++){
		if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_SUBJECT_PREFERRED_ROOM){
			ConstraintSubjectPreferredRoom* spr=(ConstraintSubjectPreferredRoom*)gt.rules.internalSpaceConstraintsList[i];
			
			for(int j=0; j<spr->_nActivities; j++){
				int a=spr->_activities[j];
				
				if(unspecifiedRoom[a]){
					unspecifiedRoom[a]=false;
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
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_SUBJECT_PREFERRED_ROOMS){
			ConstraintSubjectPreferredRooms* spr=(ConstraintSubjectPreferredRooms*)gt.rules.internalSpaceConstraintsList[i];
			
			for(int j=0; j<spr->_nActivities; j++){
				int a=spr->_activities[j];
				
				if(unspecifiedRoom[a]){
					unspecifiedRoom[a]=false;
					activitiesPreferredRoomsPercentage[a]=spr->weightPercentage;
					assert(activitiesPreferredRoomsPreferredRooms[a].count()==0);
					for(int k=0; k<spr->_n_preferred_rooms; k++){
						int rm=spr->_rooms[k];
						activitiesPreferredRoomsPreferredRooms[a].append(rm);
					}
				}
				else{
					QList<int> shared;
					for(int k=0; k<spr->_n_preferred_rooms; k++){
						int rm=spr->_rooms[k];
						if(activitiesPreferredRoomsPreferredRooms[a].indexOf(rm)!=-1)
							shared.append(rm);
					}
					activitiesPreferredRoomsPercentage[a]=max(activitiesPreferredRoomsPercentage[a], spr->weightPercentage);
					activitiesPreferredRoomsPreferredRooms[a]=shared;
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOM){
			ConstraintSubjectSubjectTagPreferredRoom* spr=(ConstraintSubjectSubjectTagPreferredRoom*)gt.rules.internalSpaceConstraintsList[i];
			
			for(int j=0; j<spr->_nActivities; j++){
				int a=spr->_activities[j];
				
				if(unspecifiedRoom[a]){
					unspecifiedRoom[a]=false;
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
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS){
			ConstraintSubjectSubjectTagPreferredRooms* spr=(ConstraintSubjectSubjectTagPreferredRooms*)gt.rules.internalSpaceConstraintsList[i];
			
			for(int j=0; j<spr->_nActivities; j++){
				int a=spr->_activities[j];
				
				if(unspecifiedRoom[a]){
					unspecifiedRoom[a]=false;
					activitiesPreferredRoomsPercentage[a]=spr->weightPercentage;
					assert(activitiesPreferredRoomsPreferredRooms[a].count()==0);
					for(int k=0; k<spr->_n_preferred_rooms; k++){
						int rm=spr->_rooms[k];
						activitiesPreferredRoomsPreferredRooms[a].append(rm);
					}
				}
				else{
					QList<int> shared;
					for(int k=0; k<spr->_n_preferred_rooms; k++){
						int rm=spr->_rooms[k];
						if(activitiesPreferredRoomsPreferredRooms[a].indexOf(rm)!=-1)
							shared.append(rm);
					}
					activitiesPreferredRoomsPercentage[a]=max(activitiesPreferredRoomsPercentage[a], spr->weightPercentage);
					activitiesPreferredRoomsPreferredRooms[a]=shared;
				}
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
			ConstraintActivityPreferredRoom* apr=(ConstraintActivityPreferredRoom*)gt.rules.internalSpaceConstraintsList[i];
			
			int a=apr->_activity;
				
			if(unspecifiedRoom[a]){
				unspecifiedRoom[a]=false;
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
			}
		}
		else if(gt.rules.internalSpaceConstraintsList[i]->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOMS){
			ConstraintActivityPreferredRooms* apr=(ConstraintActivityPreferredRooms*)gt.rules.internalSpaceConstraintsList[i];
			
			int a=apr->_activity;
				
			if(unspecifiedRoom[a]){
				unspecifiedRoom[a]=false;
				activitiesPreferredRoomsPercentage[a]=apr->weightPercentage;
				assert(activitiesPreferredRoomsPreferredRooms[a].count()==0);
				for(int k=0; k<apr->_n_preferred_rooms; k++){
					int rm=apr->_rooms[k];
					activitiesPreferredRoomsPreferredRooms[a].append(rm);
				}
			}
			else{
				QList<int> shared;
				for(int k=0; k<apr->_n_preferred_rooms; k++){
					int rm=apr->_rooms[k];
					if(activitiesPreferredRoomsPreferredRooms[a].indexOf(rm)!=-1)
						shared.append(rm);
				}
				activitiesPreferredRoomsPercentage[a]=max(activitiesPreferredRoomsPercentage[a], apr->weightPercentage);
				activitiesPreferredRoomsPreferredRooms[a]=shared;
			}
		}
	}
	
	bool ok=true;
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		if(!unspecifiedRoom[i])
			if(activitiesPreferredRoomsPreferredRooms[i].count()==0){
				ok=false;
				
				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot generate timetable, because for activity with id==%1 "
				 "you have no allowed room (from constraints subject (subject tag) preferred room(s) and activity preferred room(s) )")
				 .arg(gt.rules.internalActivitiesList[i].id),
				 QObject::tr("Skip rest of activities without rooms"), QObject::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		if(!unspecifiedRoom[i]){
			bool okinitial=true;
			if(activitiesPreferredRoomsPreferredRooms[i].count()==0)
				okinitial=false;
			foreach(int r, activitiesPreferredRoomsPreferredRooms[i]){
				if(gt.rules.internalRoomsList[r]->capacity < gt.rules.internalActivitiesList[i].nTotalStudents){
					activitiesPreferredRoomsPreferredRooms[i].removeAll(r);
				}
			}
			if(okinitial && activitiesPreferredRoomsPreferredRooms[i].count()==0){
				ok=false;
				
				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot generate timetable, because for activity with id==%1 "
				 "you have no allowed room (from the allowed number of students)")
				 .arg(gt.rules.internalActivitiesList[i].id),
				 QObject::tr("Skip rest of activities without rooms"), QObject::tr("See next problem"), QString(),
				 1, 0 );
	
				if(t==0)
					break;
			}
		}
	}
	
	return ok;
}

void sortActivities()
{
	const double THRESHOLD=80.0;
	
	int nIncompatible[MAX_ACTIVITIES];
	
	
	
	//rooms init
	int nRoomsIncompat[MAX_ROOMS];
	for(int j=0; j<gt.rules.nInternalRooms; j++){
		nRoomsIncompat[j]=0;
		for(int k=0; k<gt.rules.nHoursPerWeek; k++)
			if(allowedRoomTimePercentages[j][k]>=THRESHOLD)
				nRoomsIncompat[j]++;
	}
	int nHoursForRoom[MAX_ROOMS];	

	for(int j=0; j<gt.rules.nInternalRooms; j++)
		nHoursForRoom[j]=0;

	for(int j=0; j<gt.rules.nInternalActivities; j++)
		if(activitiesPreferredRoomsPercentage[j]>=THRESHOLD){
			assert(!unspecifiedRoom[j]);
			foreach(int rm, activitiesPreferredRoomsPreferredRooms[j])
				nHoursForRoom[rm]+=gt.rules.internalActivitiesList[j].duration;
		}

	

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		nIncompatible[i]=0;
		
		//basic
		for(int j=0; j<gt.rules.nInternalActivities; j++)
			if(i!=j && activitiesConflictingPercentage[i][j]>=THRESHOLD){
				assert(activitiesConflictingPercentage[i][j]==100.0);
				nIncompatible[i]+=gt.rules.internalActivitiesList[j].duration;
			}
				
		//not available, break, preferred time(s)
		for(int j=0; j<gt.rules.nHoursPerWeek; j++)
			if(allowedTimesPercentages[i][j]>=THRESHOLD)
				nIncompatible[i]++;
		
		//min n days - no
		

		//teachers max days per week
		foreach(int t, teachersWithMaxDaysPerWeekForActivities[i]){
			if(teachersMaxDaysPerWeekWeightPercentages[t]>=THRESHOLD)
				assert(gt.rules.nDaysPerWeek-teachersMaxDaysPerWeekMaxDays[t] >=0 );
				nIncompatible[i]+=(gt.rules.nDaysPerWeek-teachersMaxDaysPerWeekMaxDays[t])*gt.rules.nHoursPerDay;
		}

		
		//rooms
		if(activitiesPreferredRoomsPercentage[i]>=THRESHOLD){
			int cnt=0;
			assert(!unspecifiedRoom[i]);
			foreach(int rm, activitiesPreferredRoomsPreferredRooms[i])
				cnt+=nRoomsIncompat[rm]+nHoursForRoom[rm]-1; //-1 because we considered also current activity
				
			nIncompatible[i] += cnt / activitiesPreferredRoomsPreferredRooms[i].count(); //average for all the rooms
		}
				
		

		nIncompatible[i]*=gt.rules.internalActivitiesList[i].duration;
	}

	//same starting time - not computing, the algo takes care even without correct sorting
	//it is difficult to sort about same starting time
	
	//Sort activities in in-creasing order of number of the other activities with which
	//this activity does not conflict
	//Selection sort, based on a permutation
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		permutation[i]=i;
		
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		for(int j=i+1; j<gt.rules.nInternalActivities; j++){
			if(nIncompatible[permutation[i]]<nIncompatible[permutation[j]]){
				int t=permutation[i];
				permutation[i]=permutation[j];
				permutation[j]=t;
			}
		}
	}
	
	cout<<"The order of activities (id-s):"<<endl;
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		cout<<"No: "<<i+1<<", nIncompatible[permutation[i]]=="<<nIncompatible[permutation[i]]<<", ";
	
		Activity* act=&gt.rules.internalActivitiesList[permutation[i]];
		cout<<"id=="<<act->id;
		cout<<", teachers: ";
		foreach(QString s, act->teachersNames)
			cout<<qPrintable(s)<<" ";
		cout<<", subj=="<<qPrintable(act->subjectName);
		cout<<", students: ";
		foreach(QString s, act->studentsNames)
			cout<<qPrintable(s)<<" ";
		cout<<endl;
	}
	cout<<"End - the order of activities (id-s):"<<endl;
	//assert(0);

	//RANDOM ORDER
	/*for(int i=0; i<gt.rules.nInternalActivities; i++)
		permutation[i]=i;
	for(int i=0; i<gt.rules.nInternalActivities*gt.rules.nInternalActivities/2; i++){
		int j=randomKnuth()%gt.rules.nInternalActivities;
		int k=randomKnuth()%gt.rules.nInternalActivities;
		int tmp;
		tmp=permutation[j];
		permutation[j]=permutation[k];
		permutation[k]=tmp;
	}*/
}

