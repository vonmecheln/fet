/*
File solution.cpp
*/

/*
Copyright 2002, 2003 Lalescu Liviu.

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

/*
2-point and uniform crossover code by Volker Dirr, Volker at dirr-computer dot de
*/

#include <iostream>
//Added by qt3to4:
#include <QTextStream>
using namespace std;

#include "timetable_defs.h"
#include "solution.h"
#include "rules.h"
#include "timeconstraint.h"


//critical function here - must be optimized for speed
void Solution::copy(Rules& r, Solution& c){
	this->_fitness=c._fitness;

	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++){
		this->times[i] = c.times[i];
		this->rooms[i]=c.rooms[i];
	}
	//memcpy(times, c.times, r.nActivities * sizeof(times[0]));
	
	this->changedForMatrixCalculation=c.changedForMatrixCalculation;

	//added in version 5.2.0
	conflictsWeightList=c.conflictsWeightList;
	conflictsDescriptionList=c.conflictsDescriptionList;
	conflictsTotal=c.conflictsTotal;
	
	teachersMatrixReady=c.teachersMatrixReady;
	subgroupsMatrixReady=c.subgroupsMatrixReady;
	roomsMatrixReady=c.roomsMatrixReady;
}

void Solution::init(Rules& r){
	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++){
		this->times[i]=UNALLOCATED_TIME;
		this->rooms[i]=UNALLOCATED_SPACE;
	}

	this->_fitness=-1;
	
	this->changedForMatrixCalculation=true;
}

bool Solution::read(Rules& r, const QString& filename){
	assert(0);

	assert(r.initialized);

	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly))
		assert(0);
	QTextStream tis(&file);
	this->read(r, tis);
	file.close();

	return true;
}

bool Solution::read(Rules &r, QTextStream &tis){
	assert(0);

	assert(r.initialized);
	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++){
		tis>>this->times[i];
		if(tis.atEnd()){
			//The rules and the solution do not match (1)
			return false;
		}

		if(this->times[i]>=r.nHoursPerWeek && this->times[i]!=UNALLOCATED_TIME){
			//The rules and the solution do not match (2)
			return false;
		}
	}
	
	this->_fitness=-1;
	
	this->changedForMatrixCalculation=true;

	return true;
}

void Solution::write(Rules& r, const QString &filename){
	assert(0);

	assert(r.initialized);

	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	this->write(r, tos);
	file.close();
}

void Solution::write(Rules& r, QTextStream &tos){
	assert(0);

	assert(r.initialized);
	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++){
		tos<<this->times[i]<<endl;
	}
}

void Solution::makeUnallocated(Rules& r){
	assert(r.initialized);
	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++){
		this->times[i]=UNALLOCATED_TIME;
		this->rooms[i]=UNALLOCATED_SPACE;
	}

	this->_fitness=-1;

	this->changedForMatrixCalculation=true;
}

void Solution::makeRandom(Rules& r){
	assert(0);

	assert(r.initialized);
	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++){
		this->times[i] = randomKnuth()%r.nHoursPerWeek;
		//don't forget about rooms
	}

	this->_fitness = -1;

	this->changedForMatrixCalculation=true;
}


double Solution::fitness(Rules& r, QString* conflictsString){
	assert(r.initialized);
	assert(r.internalStructureComputed);

	/*if(this->changedForMatrixCalculation && this->_hardFitness>=0){
		cout<<"this->_hardFitness=="<<this->_hardFitness<<endl;
		cout<<"this->changedForMatrixCalculation=="<<this->changedForMatrixCalculation<<endl;		
	}*/
	
	if(this->_fitness>=0)
		assert(this->changedForMatrixCalculation==false);
		
	if(this->_fitness>=0 && conflictsString==NULL)
	//If you want to see the log, you have to recompute the fitness, even if it is
	//already computed
		return this->_fitness;
		
	//Repair the chromosome - we enter here with the assumption that
	//the time constraints of type ConstraintActivityPreferredTime,
	//ConstraintActivitiesSameTime and ConstraintActivitiesSameStartingHour
	//do not contradict one with each other.
	//I had good reasons here not to repair activities that are scheduled too late
	//(that is, have the hour+duration>nHoursPerDay.
	//The reason is that there might be a mutation by swapping 2 activities,
	//and I want it to consider all the variants.
	//I might be wrong :-)
	
	//1)preferred times
	/*not repairing anymore - 27 June 2007
	for(int i=0; i<r.nInternalActivities; i++){
		if(r.fixedDay[i]>=0 && r.fixedHour[i]>=0){
			this->times[i] = r.fixedDay[i] + r.fixedHour[i] * r.nDaysPerWeek;
		}
		else if(r.fixedDay[i]>=0 && this->times[i]!=UNALLOCATED_TIME){
			this->times[i] = r.fixedDay[i] + (this->times[i]/r.nDaysPerWeek)*r.nDaysPerWeek;
		}
		else if(r.fixedHour[i]>=0 && this->times[i]!=UNALLOCATED_TIME){
			this->times[i] = (this->times[i]%r.nDaysPerWeek) + r.fixedHour[i]*r.nDaysPerWeek;
		}
	}
	
	//2)same starting day and/or hour
	for(int i=0; i<r.nInternalActivities; i++){
		if(r.sameDay[i]>=0 && r.sameHour[i]>=0 && this->times[r.sameDay[i]]!=UNALLOCATED_TIME && this->times[r.sameHour[i]]!=UNALLOCATED_TIME){
			int d = this->times[r.sameDay[i]] % r.nDaysPerWeek;
			int h = this->times[r.sameHour[i]] / r.nDaysPerWeek;
			this->times[i] = d + h * r.nDaysPerWeek;
			if(r.fixedDay[i]>=0)
				assert(r.fixedDay[i]==d);
			if(r.fixedHour[i]>=0)
				assert(r.fixedHour[i]==h);
		}
		if(r.sameDay[i]>=0 && this->times[i]!=UNALLOCATED_SPACE && this->times[r.sameDay[i]]!=UNALLOCATED_TIME){
			int d = this->times[r.sameDay[i]] % r.nDaysPerWeek;
			int h = this->times[i] / r.nDaysPerWeek;
			this->times[i] = d + h * r.nDaysPerWeek;
			if(r.fixedDay[i]>=0)
				assert(r.fixedDay[i]==d);
		}
		if(r.sameHour[i]>=0 && this->times[i]!=UNALLOCATED_SPACE && this->times[r.sameHour[i]]!=UNALLOCATED_TIME){
			int d = this->times[i] % r.nDaysPerWeek;
			int h = this->times[r.sameHour[i]] / r.nDaysPerWeek;
			this->times[i] = d + h * r.nDaysPerWeek;
			if(r.fixedHour[i]>=0)
				assert(r.fixedHour[i]==h);
		}
	}*/
	
	this->changedForMatrixCalculation=true;
	
	this->_fitness=0;
	//here we must not have compulsory activity preferred time nor 
	//compulsory activities same time and/or hour
	//Also, here I compute soft fitness (for faster results,
	//I do not want to pass again through the constraints)
	
	this->conflictsDescriptionList.clear();
	this->conflictsWeightList.clear();
	
	this->teachersMatrixReady=false;
	this->subgroupsMatrixReady=false;
	this->roomsMatrixReady=false;
	
	for(int i=0; i<r.nInternalTimeConstraints; i++){
		QList<QString> sl;
		QList<double> cl;
		this->_fitness += r.internalTimeConstraintsList[i]->fitness(*this, r, cl, sl, conflictsString);
		conflictsWeightList+=cl;
		conflictsDescriptionList+=sl;
	}	
	for(int i=0; i<r.nInternalSpaceConstraints; i++){
		QList<QString> sl;
		QList<double> cl;
		this->_fitness += r.internalSpaceConstraintsList[i]->fitness(*this, r, cl, sl, conflictsString);
		conflictsWeightList+=cl;
		conflictsDescriptionList+=sl;
	}
		
	this->conflictsTotal=0;
	foreach(double cn, conflictsWeightList)
		conflictsTotal+=cn;
		
	//sort descending according to conflicts
	for(int i=0; i<conflictsWeightList.size(); i++)
		for(int j=0; j<i; j++)
			if(conflictsWeightList[i]>conflictsWeightList[j]){
				double t=conflictsWeightList[i];
				conflictsWeightList[i]=conflictsWeightList[j];
				conflictsWeightList[j]=t;
				
				QString s=conflictsDescriptionList[i];
				conflictsDescriptionList[i]=conflictsDescriptionList[j];
				conflictsDescriptionList[j]=s;
			}
			
	this->changedForMatrixCalculation=false;

	return this->_fitness;
}

//critical function here - must be optimized for speed
int Solution::getTeachersMatrix(Rules& r, qint16 a[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY]){
	assert(r.initialized);
	assert(r.internalStructureComputed);
	
	int conflicts=0;

	int i;
	for(i=0; i<r.nInternalTeachers; i++)
		for(int j=0; j<r.nDaysPerWeek; j++)
			for(int k=0; k<r.nHoursPerDay; k++)
				a[i][j][k]=0;

	for(i=0; i<r.nInternalActivities; i++)
		if(this->times[i]!=UNALLOCATED_TIME) {
			int hour = this->times[i] / r.nDaysPerWeek;
			int day = this->times[i] % r.nDaysPerWeek;
			Activity* act=&r.internalActivitiesList[i];
			for(int dd=0; dd<act->duration && hour+dd<r.nHoursPerDay; dd++)
				for(int it=0; it<act->nTeachers; it++){
					int tch=act->teachers[it];
					int tmp=a[tch][day][hour+dd];
					/*if(act->parity==PARITY_WEEKLY){
						conflicts += tmp<2 ? tmp : 2;
						a[tch][day][hour+dd]+=2;
					}
					else{
						assert(act->parity==PARITY_FORTNIGHTLY);
						conflicts += tmp<2 ? 0 : 1;
						a[tch][day][hour+dd]++;
					}*/
					conflicts += tmp==0 ? 0 : 1;
					a[tch][day][hour+dd]++;
				}
		}

	this->changedForMatrixCalculation=false;
		
	return conflicts;
}

//critical function here - must be optimized for speed
int Solution::getSubgroupsMatrix(Rules& r, qint16 a[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY]){
	assert(r.initialized);
	assert(r.internalStructureComputed);
	
	int conflicts=0;

	int i;
	for(i=0; i<r.nInternalSubgroups; i++)
		for(int j=0; j<r.nDaysPerWeek; j++)
			for(int k=0; k<r.nHoursPerDay; k++)
				a[i][j][k]=0;

	for(i=0; i<r.nInternalActivities; i++)
		if(this->times[i]!=UNALLOCATED_TIME){
			int hour=this->times[i]/r.nDaysPerWeek;
			int day=this->times[i]%r.nDaysPerWeek;
			Activity* act = &r.internalActivitiesList[i];
			for(int dd=0; dd < act->duration && hour+dd < r.nHoursPerDay; dd++)
				for(int isg=0; isg < act->nSubgroups; isg++){ //isg => index subgroup
					int sg = act->subgroups[isg]; //sg => subgroup
					int tmp=a[sg][day][hour+dd];
					/*if(act->parity == PARITY_WEEKLY){
						conflicts += tmp<2 ? tmp : 2;
						a[sg][day][hour+dd]+=2;
					}
					else{
						assert(act->parity == PARITY_FORTNIGHTLY);
						conflicts += tmp<2 ? 0 : 1;
						a[sg][day][hour+dd]++;
					}*/
					conflicts += tmp==0 ? 0 : 1;
					a[sg][day][hour+dd]++;
				}
		}
		
	this->changedForMatrixCalculation=false;
		
	return conflicts;
}

//The following 2 functions (GetTeachersTimetable & GetSubgroupsTimetable)
//are very similar to the above 2 ones (GetTeachersMatrix & GetSubgroupsMatrix)
void Solution::getTeachersTimetable(Rules& r, qint16 a[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY]){
	//assert(HFitness()==0); //This is only for perfect solutions, that do not have any non-satisfied hard constrains

	assert(r.initialized);
	assert(r.internalStructureComputed);

	int i, j, k;
	for(i=0; i<r.nInternalTeachers; i++)
		for(j=0; j<r.nDaysPerWeek; j++)
			for(k=0; k<r.nHoursPerDay; k++)
				//a1[i][j][k]=a2[i][j][k]=UNALLOCATED_ACTIVITY;
				a[i][j][k]=UNALLOCATED_ACTIVITY;

	Activity *act;
	for(i=0; i<r.nInternalActivities; i++) 
		if(this->times[i]!=UNALLOCATED_TIME) {
			act=&r.internalActivitiesList[i];
			int hour=this->times[i]/r.nDaysPerWeek;
			int day=this->times[i]%r.nDaysPerWeek;
			for(int dd=0; dd < act->duration && hour+dd < r.nHoursPerDay; dd++)
				for(int ti=0; ti<act->nTeachers; ti++){
					int tch = act->teachers[ti]; //teacher index
					/*if(a1[tch][day][hour+dd]==UNALLOCATED_ACTIVITY)
						a1[tch][day][hour+dd]=i;
					else
						a2[tch][day][hour+dd]=i;*/
					assert(a[tch][day][hour+dd]==UNALLOCATED_ACTIVITY);
					a[tch][day][hour+dd]=i;
				}
		}
}

void Solution::getSubgroupsTimetable(Rules& r, qint16 a[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY]){
	//assert(HFitness()==0);	//This is only for perfect solutions, that do not have any non-satisfied hard constrains

	assert(r.initialized);
	assert(r.internalStructureComputed);

	int i, j, k;
	for(i=0; i<r.nInternalSubgroups; i++)
		for(j=0; j<r.nDaysPerWeek; j++)
			for(k=0; k<r.nHoursPerDay; k++)
				//a1[i][j][k]=a2[i][j][k]=UNALLOCATED_ACTIVITY;
				a[i][j][k]=UNALLOCATED_ACTIVITY;

	Activity *act;
	for(i=0; i<r.nInternalActivities; i++)
		if(this->times[i]!=UNALLOCATED_TIME) {
			act=&r.internalActivitiesList[i];
			int hour=this->times[i]/r.nDaysPerWeek;
			int day=this->times[i]%r.nDaysPerWeek;
			for(int dd=0; dd < act->duration && hour+dd < r.nHoursPerDay; dd++){
				for(int isg=0; isg < act->nSubgroups; isg++){ //isg -> index subgroup
					int sg = act->subgroups[isg]; //sg -> subgroup
					/*if(a1[sg][day][hour+dd]==UNALLOCATED_ACTIVITY)
						a1[sg][day][hour+dd]=i;
					else
						a2[sg][day][hour+dd]=i;*/
					assert(a[sg][day][hour+dd]==UNALLOCATED_ACTIVITY);
					a[sg][day][hour+dd]=i;
				}
			}
		}
}

int Solution::getRoomsMatrix(
	Rules& r, 
	qint16 a[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY])
{
	assert(r.initialized);
	assert(r.internalStructureComputed);

	int conflicts=0;

	int i;
	for(i=0; i<r.nInternalRooms; i++)
		for(int j=0; j<r.nDaysPerWeek; j++)
			for(int k=0; k<r.nHoursPerDay; k++)
				a[i][j][k]=0;

	for(i=0; i<r.nInternalActivities; i++){
		int room=this->rooms[i];
		int hour=times[i]/r.nDaysPerWeek;
		int day=times[i]%r.nDaysPerWeek;
		//int hour = hours[i];
		//int day = days[i];
		if(room!=UNALLOCATED_SPACE && room!=UNSPECIFIED_ROOM && hour!=UNALLOCATED_TIME && day!=UNALLOCATED_TIME) {
			Activity* act=&r.internalActivitiesList[i];
			for(int dd=0; dd<act->duration && hour+dd<r.nHoursPerDay; dd++){
				int tmp=a[room][day][hour+dd];
				/*if(act->parity==PARITY_WEEKLY){
					conflicts += tmp<2 ? tmp : 2;
					a[room][day][hour+dd]+=2;
				}
				else{
					assert(act->parity==PARITY_FORTNIGHTLY);
					conflicts += tmp<2 ? 0 : 1;
					a[room][day][hour+dd]++;
				}*/
				conflicts += tmp==0 ? 0 : 1;
				a[room][day][hour+dd]++;
			}
		}
	}
	
	this->changedForMatrixCalculation=false;
	
	return conflicts;
}

void Solution::getRoomsTimetable(
	Rules& r,
	qint16 a[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY])
{
	assert(r.initialized);
	assert(r.internalStructureComputed);

	int i, j, k;
	for(i=0; i<r.nInternalRooms; i++)
		for(j=0; j<r.nDaysPerWeek; j++)
			for(k=0; k<r.nHoursPerDay; k++)
				//a1[i][j][k]=a2[i][j][k]=UNALLOCATED_ACTIVITY;
				a[i][j][k]=UNALLOCATED_ACTIVITY;

	Activity *act;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		int room=this->rooms[i];
		int hour=times[i]/r.nDaysPerWeek;
		int day=times[i]%r.nDaysPerWeek;
		//int day=days[i];
		//int hour=hours[i];
		if(room!=UNALLOCATED_SPACE && room!=UNSPECIFIED_ROOM && day!=UNALLOCATED_TIME && hour!=UNALLOCATED_TIME){
			for(int dd=0; dd < act->duration && hour+dd < r.nHoursPerDay; dd++){
				/*if(a1[room][day][hour+dd]==UNALLOCATED_ACTIVITY)
					a1[room][day][hour+dd]=i;
				else
					a2[room][day][hour+dd]=i;*/
				assert(a[room][day][hour+dd]==UNALLOCATED_ACTIVITY);
				a[room][day][hour+dd]=i;
			}
		}
	}
}
