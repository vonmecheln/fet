/*
File optimizetime_data.cpp
*/

/*
Copyright 2006 Lalescu Liviu.

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

#include "genetictimetable_defs.h"
#include "rules.h"
#include "genetictimetable.h"

#include "optimizetime_data.h"

#include <string.h>

#include <iostream>
#include <fstream>
//Added by qt3to4:
//#include <QTextStream>
using namespace std;

//#include <qdom.h>
//#include <qstring.h>
//#include <qtranslator.h>
//#include <qmessagebox.h>

#include <QMessageBox>

extern GeneticTimetable gt;


const int PERCENTAGE_THRESHOLD=50; //the threshold of weight percentage for which two activities become conflicting

int permutation[MAX_ACTIVITIES]; //the permutation matrix to obtain activities in
//(in-de)creasing order of number of compatible activities


//BASIC TIME
qint8 activitiesConflictingPercentage[MAX_ACTIVITIES][MAX_ACTIVITIES];

//MIN N DAYS BETWEEN ACTIVITIES
QList<int> minNDaysListOfActivities[MAX_ACTIVITIES];
QList<int> minNDaysListOfMinDays[MAX_ACTIVITIES];
QList<int> minNDaysListOfWeightPercentages[MAX_ACTIVITIES];
QList<bool> minNDaysListOfAdjIfBroken[MAX_ACTIVITIES];

//TCH & ST NOT AVAIL, BREAK, ACT(S) PREFERRED TIME(S)
qint8 allowedTimesPercentages[MAX_ACTIVITIES][MAX_HOURS_PER_WEEK];

//STUDENTS NO GAPS & EARLY
int nHoursPerSubgroup[MAX_TOTAL_SUBGROUPS];
qint8 subgroupsEarlyPercentage[MAX_TOTAL_SUBGROUPS];
qint8 subgroupsNoGapsPercentage[MAX_TOTAL_SUBGROUPS];

//TEACHERS MAX DAYS PER WEEK
//activities indices (in 0..gt.rules.nInternalActivities-1) for each teacher
QList<int> activitiesForTeachers[MAX_TEACHERS];
//int maxDaysPerWeekForTeachers[MAX_TEACHERS]; //-1 is not existent
int teachersMaxDaysPerWeekMaxDays[MAX_TEACHERS];
int teachersMaxDaysPerWeekWeightPercentages[MAX_TEACHERS];
QList<int> teacherActivitiesOfTheDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
QList<int> teachersWithMaxDaysPerWeekForActivities[MAX_ACTIVITIES];

/////////////////care for teachers max gaps
int nHoursPerTeacher[MAX_TEACHERS];
int teachersMaxGapsPercentage[MAX_TEACHERS];
int teachersMaxGapsMaxGaps[MAX_TEACHERS];

//students (set) n hours daily
/*int studentsNHoursDailyMinHours[MAX_TOTAL_SUBGROUPS];
int studentsNHoursDailyMaxHours[MAX_TOTAL_SUBGROUPS];
qint8 studentsNHoursDailyPercentage[MAX_TOTAL_SUBGROUPS];
QList<int> studentsSubgroupsWithNHoursDaily;*/

//activities same starting time
QList<int> activitiesSameStartingTimeActivities[MAX_ACTIVITIES];
QList<int> activitiesSameStartingTimePercentages[MAX_ACTIVITIES];

//activities same starting hour
QList<int> activitiesSameStartingHourActivities[MAX_ACTIVITIES];
QList<int> activitiesSameStartingHourPercentages[MAX_ACTIVITIES];

//activities not overlapping
QList<int> activitiesNotOverlappingActivities[MAX_ACTIVITIES];
QList<int> activitiesNotOverlappingPercentages[MAX_ACTIVITIES];

//teacher(s) max hours daily
int teachersMaxHoursDailyPercentages[MAX_TEACHERS];
int teachersMaxHoursDailyMaxHours[MAX_TEACHERS];
int teachersGapsPerDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
int teachersRealGapsPerDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
int teachersNHoursPerDay[MAX_TEACHERS][MAX_DAYS_PER_WEEK];
qint16 teachersTimetable[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];

//students (set) max hours daily
int subgroupsMaxHoursDailyPercentages[MAX_TOTAL_SUBGROUPS];
int subgroupsMaxHoursDailyMaxHours[MAX_TOTAL_SUBGROUPS];
//int subgroupsGapsPerDay[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];
//int subgroupsNHoursPerDay[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];
qint16 subgroupsTimetable[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];


bool processTimeConstraints()
{
	assert(gt.rules.internalStructureComputed);

	/////1. BASIC TIME CONSTRAINTS
	bool t=computeActivitiesConflictingPercentage();
	if(!t)
		return false;
	sortActivities();
	//////////////////////////////
	
	/////2. min n days between activities
	computeMinNDays();
	/////////////////////////////////////
	
	/////3. st not avail, tch not avail, break, activity pref time,
	/////   activity preferred times, activities preferred times
	computeAllowedTimesPercentages();
	///////////////////////////////////////////////////////////////
	
	/////4. students no gaps and early
	computeNHoursPerSubgroup();
	t=computeSubgroupsEarlyAndNoGapsPercentages();
	if(!t)
		return false;
	//////////////////////////////////
	
	/////5. TEACHER MAX DAYS PER WEEK
	computeActivitiesForTeachers();
	t=computeMaxDaysPerWeekForTeachers();
	if(!t)
		return false;
	//////////////////////////////////
	
	
	/////6. TEACHERS MAX GAPS PER WEEK
	computeNHoursPerTeacher();
	t=computeTeachersMaxGapsPercentage();
	if(!t)
		return false;
	//////////////////////////////////
	
	//students (set) n hours daily
	/*t=computeStudentsNHoursDaily();
	if(!t)
		return false;*/
		
	computeActivitiesSameStartingTime();

	computeActivitiesSameStartingHour();
	
	computeActivitiesNotOverlapping();
	
	t=computeTeachersMaxHoursDaily();
	if(!t)
		return false;
	
	t=computeSubgroupsMaxHoursDaily();
	if(!t)
		return false;
	
	//check for not implemented constraints in FET 5
	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		TimeConstraint* tc=gt.rules.internalTimeConstraintsList[i];
		
		if(tc->type==CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY){
			ok=false;
			
			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize, because you have constraints of type "
			 "teachers max hours continuously. To run with this preview version, you need to remove such constraints"),
			 "Skip rest of constraints problems", "See next not-implemented yet constraint", QString(),
			 1, 0 );

			if(t==0)
				break;
		}
		else if(tc->type==CONSTRAINT_TEACHERS_SUBGROUPS_MAX_HOURS_DAILY){
			ok=false;
			
			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize, because you have constraints of type "
			 "teachers subgroups max hours daily. To run with this preview version, you need to remove such constraints"),
			 "Skip rest of constraints problems", "See next not-implemented yet constraint", QString(),
			 1, 0 );

			if(t==0)
				break;
		}
		/*else if(tc->type==CONSTRAINT_TEACHERS_MAX_HOURS_DAILY){
			ok=false;
			
			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize, because you have constraints of type\n"
			 "teachers max hours daily. To run with this preview version, you need to remove such constraints"),
			 "Skip rest of constraints problems", "See next not-implemented yet constraint", QString(),
			 1, 0 );

			if(t==0)
				break;
		}*/
		else if(tc->type==CONSTRAINT_TEACHERS_MIN_HOURS_DAILY){
			ok=false;
			
			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize, because you have constraints of type "
			 "teachers min hours daily. To run with this preview version, you need to remove such constraints"),
			 "Skip rest of constraints problems", "See next not-implemented yet constraint", QString(),
			 1, 0 );

			if(t==0)
				break;
		}
		else if(tc->type==CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK){
			ok=false;
			
			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize, because you have constraints of type "
			 "teachers max days per week. To run with this preview version, you need to remove such constraints"),
			 "Skip rest of constraints problems", "See next not-implemented yet constraint", QString(),
			 1, 0 );

			if(t==0)
				break;
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK){
			ok=false;
			
			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize, because you have constraints of type "
			 "students set interval max days per week. To run with this preview version, you need to remove such constraints"),
			 "Skip rest of constraints problems", "See next not-implemented yet constraint", QString(),
			 1, 0 );

			if(t==0)
				break;
		}
		/*
		else if(tc->type==CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING){
			ok=false;
			
			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize, because you have constraints of type\n"
			 "activities not overlapping. To run with this preview version, you need to remove such constraints"),
			 "Skip rest of constraints problems", "See next not-implemented yet constraint", QString(),
			 1, 0 );

			if(t==0)
				break;
		}*/
		else if(tc->type==CONSTRAINT_ACTIVITY_ENDS_DAY){
			ok=false;
			
			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize, because you have constraints of type "
			 "activity ends day. To run with this preview version, you need to remove such constraints"),
			 "Skip rest of constraints problems", "See next not-implemented yet constraint", QString(),
			 1, 0 );

			if(t==0)
				break;
		}
		else if(tc->type==CONSTRAINT_2_ACTIVITIES_CONSECUTIVE){
			ok=false;
			
			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize, because you have constraints of type "
			 "2 activities consecutive. To run with this preview version, you need to remove such constraints"),
			 "Skip rest of constraints problems", "See next not-implemented yet constraint", QString(),
			 1, 0 );

			if(t==0)
				break;
		}
		else if(tc->type==CONSTRAINT_2_ACTIVITIES_ORDERED){
			ok=false;
			
			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize, because you have constraints of type "
			 "2 activities ordered. To run with this preview version, you need to remove such constraints"),
			 "Skip rest of constraints problems", "See next not-implemented yet constraint", QString(),
			 1, 0 );

			if(t==0)
				break;
		}
		else if(tc->type==CONSTRAINT_2_ACTIVITIES_GROUPED){
			ok=false;
			
			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize, because you have constraints of type "
			 "2 activities grouped. To run with this preview version, you need to remove such constraints"),
			 "Skip rest of constraints problems", "See next not-implemented yet constraint", QString(),
			 1, 0 );

			if(t==0)
				break;
		}
		else if(tc->type==CONSTRAINT_TEACHERS_SUBJECT_TAGS_MAX_HOURS_CONTINUOUSLY){
			ok=false;
			
			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize, because you have constraints of type "
			 "teachers subject tags max hours continuously. To run with this preview version, you need to remove such constraints"),
			 "Skip rest of constraints problems", "See next not-implemented yet constraint", QString(),
			 1, 0 );

			if(t==0)
				break;
		}
		else if(tc->type==CONSTRAINT_TEACHERS_SUBJECT_TAG_MAX_HOURS_CONTINUOUSLY){
			ok=false;
			
			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize, because you have constraints of type "
			 "teachers subject tag max hours continuously. To run with this preview version, you need to remove such constraints"),
			 "Skip rest of constraints problems", "See next not-implemented yet constraint", QString(),
			 1, 0 );

			if(t==0)
				break;
		}
		else if(tc->type==CONSTRAINT_STUDENTS_SET_N_HOURS_DAILY){
			ok=false;
			
			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize, because you have constraints of type "
			 "students set n hours daily. To run with this preview version, you need to remove such constraints"),
			 "Skip rest of constraints problems", "See next not-implemented yet constraint", QString(),
			 1, 0 );

			if(t==0)
				break;
		}
		else if(tc->type==CONSTRAINT_STUDENTS_N_HOURS_DAILY){
			ok=false;
			
			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize, because you have constraints of type "
			 "students n hours daily. To run with this preview version, you need to remove such constraints"),
			 "Skip rest of constraints problems", "See next not-implemented yet constraint", QString(),
			 1, 0 );

			if(t==0)
				break;
		}
	}
	
	return ok;
}

bool computeSubgroupsMaxHoursDaily()
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalSubgroups; i++){
		subgroupsMaxHoursDailyMaxHours[i]=-1;
		subgroupsMaxHoursDailyPercentages[i]=-1;
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_MAX_HOURS_DAILY){
			ConstraintStudentsMaxHoursDaily* smd=(ConstraintStudentsMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMaxHoursDailyMaxHours[sb]==-1 ||
				 subgroupsMaxHoursDailyMaxHours[sb] >= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyPercentages[sb] <= smd->weightPercentage){
				 	subgroupsMaxHoursDailyMaxHours[sb] = smd->maxHoursDaily;
					subgroupsMaxHoursDailyPercentages[sb] = int(smd->weightPercentage);
					}
				else if(subgroupsMaxHoursDailyMaxHours[sb] <= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyPercentages[sb] >= smd->weightPercentage){
				 	//nothing
				}
				else{ //cannot proceed
					ok=false;
	
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("Cannot optimize for subgroup %1, because there are two constraints"
					 " of type max hours daily relating to him, and the weight percentage is higher on the constraint"
					 " with more hours daily allowed. You are allowed only to have for each subgroup"
					 " the most important constraint with maximum weight percentage and minimum hours daily allowed allowed"
					 ". Please modify your data accordingly and try again"
					 ". For more details, join the mailing list or email the author")
					 .arg(gt.rules.internalSubgroupsList[sb]->name),
					 "Skip rest of max hours problems", "See next incompatibility max hours", QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY){
			ConstraintStudentsSetMaxHoursDaily* smd=(ConstraintStudentsSetMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			for(int q=0; q<smd->nSubgroups; q++){
				int sb=smd->subgroups[q];
			//for(int sb=0; sb<gt.rules.nInternalSubgroups; sb++){
				if(subgroupsMaxHoursDailyMaxHours[sb]==-1 ||
				 subgroupsMaxHoursDailyMaxHours[sb] >= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyPercentages[sb] <= smd->weightPercentage){
				 	subgroupsMaxHoursDailyMaxHours[sb] = smd->maxHoursDaily;
					subgroupsMaxHoursDailyPercentages[sb] = int(smd->weightPercentage);
					}
				else if(subgroupsMaxHoursDailyMaxHours[sb] <= smd->maxHoursDaily &&
				 subgroupsMaxHoursDailyPercentages[sb] >= smd->weightPercentage){
				 	//nothing
				}
				else{ //cannot proceed
					ok=false;
	
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("Cannot optimize for subgroup %1, because there are two constraints"
					 " of type max hours daily relating to him, and the weight percentage is higher on the constraint"
					 " with more hours daily allowed. You are allowed only to have for each subgroup"
					 " the most important constraint with maximum weight percentage and minimum hours daily allowed allowed"
					 ". Please modify your data accordingly and try again"
					 ". For more details, join the mailing list or email the author")
					 .arg(gt.rules.internalSubgroupsList[sb]->name),
					 "Skip rest of max hours problems", "See next incompatibility max hours", QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
				}
			}
		}
	}
	
	return ok;
}
	

bool computeTeachersMaxHoursDaily()
{
	bool ok=true;
	
	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		teachersMaxHoursDailyMaxHours[i]=-1;
		teachersMaxHoursDailyPercentages[i]=-1;
	}
	
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY){
			ConstraintTeacherMaxHoursDaily* tmd=(ConstraintTeacherMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			if(teachersMaxHoursDailyMaxHours[tmd->teacher_ID]==-1 ||
			 teachersMaxHoursDailyMaxHours[tmd->teacher_ID] >= tmd->maxHoursDaily &&
			 teachersMaxHoursDailyPercentages[tmd->teacher_ID] <= tmd->weightPercentage){
			 	teachersMaxHoursDailyMaxHours[tmd->teacher_ID] = tmd->maxHoursDaily;
				teachersMaxHoursDailyPercentages[tmd->teacher_ID] = int(tmd->weightPercentage);
			}
			else if(teachersMaxHoursDailyMaxHours[tmd->teacher_ID] <= tmd->maxHoursDaily &&
			 teachersMaxHoursDailyPercentages[tmd->teacher_ID] >= tmd->weightPercentage){
			 	//nothing
			}
			else{ //cannot proceed
				ok=false;

				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize for teacher %1, because there are two constraints"
				 " of type max hours daily relating to him, and the weight percentage is higher on the constraint"
				 " with more hours daily allowed. You are allowed only to have for each teacher"
				 " the most important constraint with maximum weight percentage and minimum hours daily allowed allowed"
				 ". Please modify your data accordingly and try again"
				 ". For more details, join the mailing list or email the author")
				 .arg(gt.rules.internalTeachersList[tmd->teacher_ID]->name),
				 "Skip rest of max hours problems", "See next incompatibility max hours", QString(),
				 1, 0 );
			 	
				if(t==0)
					return false;
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHERS_MAX_HOURS_DAILY){
			ConstraintTeachersMaxHoursDaily* tmd=(ConstraintTeachersMaxHoursDaily*)gt.rules.internalTimeConstraintsList[i];

			for(int tch=0; tch<gt.rules.nInternalTeachers; tch++){
				if(teachersMaxHoursDailyMaxHours[tch]==-1 ||
				 teachersMaxHoursDailyMaxHours[tch] >= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyPercentages[tch] <= tmd->weightPercentage){
				 	teachersMaxHoursDailyMaxHours[tch] = tmd->maxHoursDaily;
					teachersMaxHoursDailyPercentages[tch] = int(tmd->weightPercentage);
					}
				else if(teachersMaxHoursDailyMaxHours[tch] <= tmd->maxHoursDaily &&
				 teachersMaxHoursDailyPercentages[tch] >= tmd->weightPercentage){
				 	//nothing
				}
				else{ //cannot proceed
					ok=false;
	
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("Cannot optimize for teacher %1, because there are two constraints"
					 " of type max hours daily relating to him, and the weight percentage is higher on the constraint"
					 " with more hours daily allowed. You are allowed only to have for each teacher"
					 " the most important constraint with maximum weight percentage and minimum hours daily allowed allowed"
					 ". Please modify your data accordingly and try again"
					 ". For more details, join the mailing list or email the author")
					 .arg(gt.rules.internalTeachersList[tch]->name),
					 "Skip rest of max hours problems", "See next incompatibility max hours", QString(),
					 1, 0 );
				 	
					if(t==0)
						return false;
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
								activitiesNotOverlappingPercentages[ai1][t]=int(no->weightPercentage);
							}
						}
						else{
							activitiesNotOverlappingPercentages[ai1].append(int(no->weightPercentage));
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
						int perc=-1;
						if(t!=-1){
							perc=activitiesSameStartingTimePercentages[ai1].at(t);
							assert(perc>=0 && perc<=100);
						}
						
						if(t==-1 || perc<sst->weightPercentage){
							activitiesSameStartingTimeActivities[ai1].append(ai2);
							activitiesSameStartingTimePercentages[ai1].append(int(sst->weightPercentage));
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
						int perc=-1;
						if(t!=-1){
							perc=activitiesSameStartingHourPercentages[ai1].at(t);
							assert(perc>=0 && perc<=100);
						}
						
						if(t==-1 || perc<sst->weightPercentage){
							activitiesSameStartingHourActivities[ai1].append(ai2);
							activitiesSameStartingHourPercentages[ai1].append(int(sst->weightPercentage));
							assert(activitiesSameStartingHourActivities[ai1].count()==activitiesSameStartingHourPercentages[ai1].count());
						}
					}
				}
			}
		}
}

/*
bool computeStudentsNHoursDaily()
{
	for(int j=0; j<gt.rules.nInternalSubgroups; j++){
		studentsNHoursDailyMinHours[j]=-1;
		studentsNHoursDailyMaxHours[j]=-1;
		studentsNHoursDailyPercentage[j]=-1;
	}

	bool ok=true;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_N_HOURS_DAILY){
			ConstraintStudentsNHoursDaily* sn=(ConstraintStudentsNHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<gt.rules.nInternalSubgroups; j++){
				if(studentsNHoursDailyMinHours[j]==-1){
					studentsNHoursDailyMinHours[j]=sn->minHoursDaily;
					studentsNHoursDailyMaxHours[j]=sn->maxHoursDaily;
					studentsNHoursDailyPercentage[j]=int(sn->weightPercentage);
				}
				else{
					ok=false;
				
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("Cannot optimize for subgroup %1, because it has at least two constraints n hours daily"
					 "\nPlease modify your data correspondingly (leave maximum one constraint of type"
					 "\nconstraint students (set) nhours daily for each subgroup) and try again")
					 .arg(gt.rules.internalSubgroupsList[j]->name),
					 "Skip rest of students n hours daily problems", "See next incompatibility n hours daily", QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		}
		else if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_N_HOURS_DAILY){
			ConstraintStudentsSetNHoursDaily* sn=(ConstraintStudentsSetNHoursDaily*)gt.rules.internalTimeConstraintsList[i];
			for(int k=0; k<sn->nSubgroups; k++){
				int j=sn->subgroups[k];
				if(studentsNHoursDailyMinHours[j]==-1){
					studentsNHoursDailyMinHours[j]=sn->minHoursDaily;
					studentsNHoursDailyMaxHours[j]=sn->maxHoursDaily;
					studentsNHoursDailyPercentage[j]=int(sn->weightPercentage);
				}
				else{
					ok=false;
				
					int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
					 QObject::tr("Cannot optimize for subgroup %1, because it has at least two constraints n hours daily"
					 "\nPlease modify your data correspondingly (leave maximum one constraint of type"
					 "\nconstraint students (set) nhours daily for each subgroup) and try again")
					 .arg(gt.rules.internalSubgroupsList[j]->name),
					 "Skip rest of students n hours daily problems", "See next incompatibility n hours daily", QString(),
					 1, 0 );
			 	
					if(t==0)
						return false;
				}
			}
		}
	}
	
	if(ok){
		studentsSubgroupsWithNHoursDaily.clear();
		for(int j=0; j<gt.rules.nInternalSubgroups; j++){
			if(studentsNHoursDailyMaxHours[j]>=0){
				studentsSubgroupsWithNHoursDaily.append(j);
			}
		}
	}
	
	return ok;
}*/

////////////teachers' no gaps
void computeNHoursPerTeacher()
{
	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		nHoursPerTeacher[i]=0;
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		Activity* act=&gt.rules.internalActivitiesList[i];
		for(int j=0; j<act->nTeachers; j++){
			int tc=act->teachers[j];
			nHoursPerTeacher[tc]+=act->duration;
		}
	}
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
			
			for(int j=0; j<gt.rules.nInternalTeachers; j++){
				if(teachersMaxGapsMaxGaps[j]==-1 
				 ||	teachersMaxGapsMaxGaps[j]>=0 && teachersMaxGapsMaxGaps[j]>=tg->maxGaps && teachersMaxGapsPercentage[j]<=tg->weightPercentage){
					teachersMaxGapsMaxGaps[j]=tg->maxGaps;
					teachersMaxGapsPercentage[j]=int(tg->weightPercentage);
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
					 "Skip rest of max gaps problems", "See next incompatibility max gaps", QString(),
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
				teachersMaxGapsPercentage[j]=int(tg->weightPercentage);
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
				 "Skip rest of max gaps problems", "See next incompatibility max gaps", QString(),
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
void computeNHoursPerSubgroup()
{
	for(int i=0; i<gt.rules.nInternalSubgroups; i++)
		nHoursPerSubgroup[i]=0;
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		Activity* act=&gt.rules.internalActivitiesList[i];
		for(int j=0; j<act->nSubgroups; j++){
			int isg=act->subgroups[j];
			nHoursPerSubgroup[isg]+=act->duration;
		}
	}
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

void computeActivitiesForTeachers()
{
	assert(gt.rules.internalStructureComputed);
	
	for(int j=0; j<gt.rules.nInternalTeachers; j++)
		activitiesForTeachers[j].clear();

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		Activity* act=&gt.rules.internalActivitiesList[i];
		for(int j=0; j<act->nTeachers; j++){
			int t=act->teachers[j];
			if(activitiesForTeachers[t].indexOf(i)==-1)
				activitiesForTeachers[t].append(i);
		}
	}
}

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
			if(teachersMaxDaysPerWeekMaxDays[tn->teacher_ID]==-1){
				teachersMaxDaysPerWeekMaxDays[tn->teacher_ID]=tn->maxDaysPerWeek;
				teachersMaxDaysPerWeekWeightPercentages[tn->teacher_ID]=int(tn->weightPercentage);
			}
			else{
				ok=false;
				
				int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
				 QObject::tr("Cannot optimize for teacher %1, because it has at least two constraints max days per week"
				 ". Please modify your data correspondingly (leave maximum one constraint of type"
				 " constraint teacher max days per week for each teacher) and try again")
				 .arg(gt.rules.internalTeachersList[tn->teacher_ID]->name),
				 "Skip rest of max days per week problems", "See next incompatibility max days per week", QString(),
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
			for(int j=0; j<act->nTeachers; j++){
				int tch=act->teachers[j];
				
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
					subgroupsEarlyPercentage[j] = int(se->weightPercentage);
		}

		//students set early
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_EARLY){
			ConstraintStudentsSetEarly* se=(ConstraintStudentsSetEarly*) gt.rules.internalTimeConstraintsList[i];
			for(int q=0; q<se->nSubgroups; q++){
			//for(int j=0; j<gt.rules.nInternalSubgroups; j++)
				int j=se->subgroups[q];
				if(subgroupsEarlyPercentage[j] < se->weightPercentage)
					subgroupsEarlyPercentage[j] = int(se->weightPercentage);
			}
		}

		//students no gaps
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_NO_GAPS){
			ConstraintStudentsNoGaps* sg=(ConstraintStudentsNoGaps*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<gt.rules.nInternalSubgroups; j++)
				if(subgroupsNoGapsPercentage[j] < sg->weightPercentage)
					subgroupsNoGapsPercentage[j] = int(sg->weightPercentage);
		}

		//students set no gaps
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_NO_GAPS){
			ConstraintStudentsSetNoGaps* sg=(ConstraintStudentsSetNoGaps*) gt.rules.internalTimeConstraintsList[i];
			for(int j=0; j<sg->nSubgroups; j++){
				int s=sg->subgroups[j];
				if(subgroupsNoGapsPercentage[s] < sg->weightPercentage)
					subgroupsNoGapsPercentage[s] = int(sg->weightPercentage);
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
			 "Abort", "See next incompatibility no gaps - early", QString(),
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
			 "Abort", "See next incompatibility no gaps - early", QString(),
			 1, 0 );
			 
			if(t==0)
				break;
		}
		if(subgroupsNoGapsPercentage[i]== -1 && subgroupsEarlyPercentage[i]>=0){
			oksubgroup=false;
			ok=false;

			int t=QMessageBox::warning(NULL, QObject::tr("FET warning"),
			 QObject::tr("Cannot optimize for subgroup %1, because a students early constraint"
			 " exists for this subgroup, and you have not 'no gaps' requirements for this subgroup."
			 ". The algorithm can 1. optimize with 'early' and 'no gaps'"
			 " having the same weight percentage or 2. only 'no gaps' optimization"
			 " without 'early'. Please modify your data correspondingly and try again")
			 .arg(gt.rules.internalSubgroupsList[i]->name)
			 .arg(subgroupsEarlyPercentage[i]),
			 "Abort", "See next incompatibility no gaps - early", QString(),
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
			 "Skip rest of early - no gaps problems", "See next incompatibility no gaps - early", QString(),
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

void computeAllowedTimesPercentages()
{
	assert(gt.rules.internalStructureComputed);

	for(int i=0; i<gt.rules.nInternalActivities; i++)
		for(int j=0; j<gt.rules.nHoursPerWeek; j++)
			allowedTimesPercentages[i][j]=-1;

	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++){
			//TEACHER not available
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_TEACHER_NOT_AVAILABLE){
				ConstraintTeacherNotAvailable* tn=(ConstraintTeacherNotAvailable*)gt.rules.internalTimeConstraintsList[i];
				for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(int ti=0; ti<act->nTeachers; ti++)
						if(act->teachers[ti]==tn->teacher_ID){
							for(int h=max(0, tn->h1 - act->duration + 1); h<tn->h2; h++){
								if(allowedTimesPercentages[ai][tn->d+h*gt.rules.nDaysPerWeek]<tn->weightPercentage)
									allowedTimesPercentages[ai][tn->d+h*gt.rules.nDaysPerWeek]=int(tn->weightPercentage);
							}
							break; //search no more for teacher
						}
				}
			}

			//STUDENTS SET not available		
			if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE){
				ConstraintStudentsSetNotAvailable* sn=(ConstraintStudentsSetNotAvailable*)gt.rules.internalTimeConstraintsList[i];
				for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					for(int sg=0; sg<act->nSubgroups; sg++)
						for(int j=0; j<sn->nSubgroups; j++){
							if(act->subgroups[sg]==sn->subgroups[j]){
								for(int h=max(0, sn->h1 - act->duration + 1); h<sn->h2; h++){
									if(allowedTimesPercentages[ai][sn->d+h*gt.rules.nDaysPerWeek]<sn->weightPercentage)
										allowedTimesPercentages[ai][sn->d+h*gt.rules.nDaysPerWeek]=int(sn->weightPercentage);
								}
								//break; //search no more for subgroup - this can bring an improvement in precalculation
								//but needs attention
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
							allowedTimesPercentages[ai][br->d+h*gt.rules.nDaysPerWeek]=int(br->weightPercentage);
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
									allowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]=int(ap->weightPercentage);
				}
				else if(ap->day>=0){
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						for(int h=0; h<gt.rules.nHoursPerDay; h++)
							if(d!=ap->day)
								if(allowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]<ap->weightPercentage)
									allowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]=int(ap->weightPercentage);
				}
				else if(ap->hour>=0){
					for(int d=0; d<gt.rules.nDaysPerWeek; d++)
						for(int h=0; h<gt.rules.nHoursPerDay; h++)
							if(h!=ap->hour)
								if(allowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]<ap->weightPercentage)
									allowedTimesPercentages[ap->activityIndex][d+h*gt.rules.nDaysPerWeek]=int(ap->weightPercentage);
				}
				else
					assert(0);
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
							allowedTimesPercentages[ai][k] = int(ap->weightPercentage);
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
								allowedTimesPercentages[ai][k] = int(ap->weightPercentage);
				}
			}
	}
}

void computeMinNDays()
{
	for(int j=0; j<gt.rules.nInternalActivities; j++){
		minNDaysListOfActivities[j].clear();
		minNDaysListOfMinDays[j].clear();
		minNDaysListOfAdjIfBroken[j].clear();
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
						assert(int(md->weightPercentage) >=0 && int(md->weightPercentage)<=100);
						minNDaysListOfWeightPercentages[ai1].append(int(md->weightPercentage));
						minNDaysListOfAdjIfBroken[ai1].append(md->adjacentIfBroken);
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
	int m=-1;
	
	bool ok=false;
	for(int i=0; i<gt.rules.nInternalTimeConstraints; i++)
		if(gt.rules.internalTimeConstraintsList[i]->type==CONSTRAINT_BASIC_COMPULSORY_TIME){
			ok=true;
			if(gt.rules.internalTimeConstraintsList[i]->weightPercentage>m)
				m=int(gt.rules.internalTimeConstraintsList[i]->weightPercentage);
		}
		
	if(!ok){
		QMessageBox::warning(NULL, QObject::tr("FET warning"),
		 QObject::tr("Cannot optimize, because you have no basic time constraints. "
		 "Please add a basic time constraint (recommended with 100% weight)"));
		return false;
	}
	
	assert(m>=0 && m<=100);

	//compute conflictig
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		activitiesConflictingPercentage[i][i]=m;
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		for(int j=i+1; j<gt.rules.nInternalActivities; j++){
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
				activitiesConflictingPercentage[i][j]=activitiesConflictingPercentage[j][i]=m;
			else
				activitiesConflictingPercentage[i][j]=activitiesConflictingPercentage[j][i]=-1;
		}
		
	return true;
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

void sortActivities()
{
	//computeMinNDays();
	
	static int nCompatible[MAX_ACTIVITIES];

	for(int i=0; i<gt.rules.nInternalActivities; i++){
		nCompatible[i]=0;
		
		for(int j=0; j<gt.rules.nInternalActivities; j++)
			if(i!=j && activitiesConflictingPercentage[i][j]==-1)
				nCompatible[i]++;
				
	/*	nCompatible[i] -= 5
		 *minNDaysListOfActivities[i].count()
		 *minNDaysListOfActivities[i].count()
		 *minNDaysListOfActivities[i].count()
		 *minNDaysListOfActivities[i].count();*/
	}
	
	//Sort activities in in-creasing order of number of the other activities with which
	//this activity does not conflict
	//Selection sort, based on a permutation
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		permutation[i]=i;
		
	for(int i=0; i<gt.rules.nInternalActivities; i++){
		for(int j=i+1; j<gt.rules.nInternalActivities; j++){
			if(nCompatible[permutation[i]]>nCompatible[permutation[j]]){
				int t=permutation[i];
				permutation[i]=permutation[j];
				permutation[j]=t;
			}
		}
	}
	
	/*cout<<"The order of activities (id-s):"<<endl;
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		cout<<"id=="<<gt.rules.internalActivitiesList[permutation[i]].id<<endl;
	cout<<"End - the order of activities (id-s):"<<endl;*/

	//RANDOM ORDER
/*	for(int i=0; i<gt.rules.nInternalActivities; i++)
		permutation[i]=i;
	for(int i=0; i<gt.rules.nInternalActivities*gt.rules.nInternalActivities/2; i++){
		int j=rand()%gt.rules.nInternalActivities;
		int k=rand()%gt.rules.nInternalActivities;
		int tmp;
		tmp=permutation[j];
		permutation[j]=permutation[k];
		permutation[k]=tmp;
	}*/
}
