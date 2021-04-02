/*
File timespacechromosome.cpp
*/

/*
Copyright 2005 Lalescu Liviu.

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

#include "genetictimetable_defs.h"
#include "timespacechromosome.h"
#include "rules.h"
#include "timeconstraint.h"
#include "spaceconstraint.h"

#include <iostream>
//Added by qt3to4:
#include <QTextStream>
using namespace std;

int better(Rules& r, TimeSpaceChromosome& c1, TimeSpaceChromosome& c2){ //returns true if c1 is better than c2
	//Here the order is important, you have to compute firstly the hard fitness, then the soft fitness
	int hf1=c1.hardFitness(r);
	int sf1=c1.softFitness(r);
	int hf2=c2.hardFitness(r);
	int sf2=c2.softFitness(r);

	return better(hf1, sf1, hf2, sf2);
}

//defined in timechromosome.cpp
/*int better(int hf1, int sf1, int hf2, int sf2){
	return hf1<hf2 || hf1==hf2 && sf1<sf2;
}*/

//critical function here - must be optimized for speed
void TimeSpaceChromosome::copy(Rules& r, TimeSpaceChromosome& c){
	this->_hardFitness=c._hardFitness;
	this->_softFitness=c._softFitness;

	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++){
		this->times[i] = c.times[i];
		this->rooms[i] = c.rooms[i];
	}
	//memcpy(times, c.times, r.nActivities * sizeof(times[0]));
	
	this->timeChangedForMatrixCalculation=c.timeChangedForMatrixCalculation;
	this->spaceChangedForMatrixCalculation=c.spaceChangedForMatrixCalculation;	
}

void TimeSpaceChromosome::init(Rules& r){
	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++){
		this->times[i]=UNALLOCATED_TIME;
		this->rooms[i]=UNALLOCATED_SPACE;
	}

	this->_hardFitness=this->_softFitness=-1;

	this->timeChangedForMatrixCalculation=true;
	this->spaceChangedForMatrixCalculation=true;	
}

bool TimeSpaceChromosome::read(Rules& r, const QString& filename){
	assert(r.initialized);

	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly))
		assert(0);
	QTextStream tis(&file);
	this->read(r, tis);
	file.close();

	return true;
}

bool TimeSpaceChromosome::read(Rules &r, QTextStream &tis){
	assert(r.initialized);
	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++){
		tis>>this->times[i];
		tis>>this->rooms[i];
		if(tis.atEnd()){
			//The rules and the solution do not match (1)
			return false;
		}

		if(this->times[i]>=r.nHoursPerWeek && this->times[i]!=UNALLOCATED_TIME){
			//The rules and the solution do not match (2)
			return false;
		}
		if(this->rooms[i]>=r.nInternalRooms && this->rooms[i]!=UNALLOCATED_SPACE){
			//The rules and the solution do not match (3)
			return false;
		}	
	}
	
	this->_hardFitness=this->_softFitness=-1;

	this->timeChangedForMatrixCalculation=true;
	this->spaceChangedForMatrixCalculation=true;	

	return true;
}

void TimeSpaceChromosome::write(Rules& r, const QString &filename){
	assert(r.initialized);

	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	this->write(r, tos);
	file.close();
}

void TimeSpaceChromosome::write(Rules& r, QTextStream &tos){
	assert(r.initialized);
	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++){
		tos<<this->times[i]<<endl;
		tos<<this->rooms[i]<<endl;
	}
}

void TimeSpaceChromosome::makeTimesRoomsUnallocated(Rules& r){
	assert(r.initialized);
	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++){
		this->times[i]=UNALLOCATED_TIME;
		this->rooms[i]=UNALLOCATED_SPACE;
	}

	this->_hardFitness=this->_softFitness=-1;

	this->timeChangedForMatrixCalculation=true;
	this->spaceChangedForMatrixCalculation=true;	
}

void TimeSpaceChromosome::makeTimesRoomsRandom(Rules& r){
	assert(r.initialized);
	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++){
		this->times[i] = rand()%r.nHoursPerWeek;
		this->rooms[i]=rand()%r.nInternalRooms;
	}

	this->_hardFitness = this->_softFitness = -1;

	this->timeChangedForMatrixCalculation=true;
	this->spaceChangedForMatrixCalculation=true;	
}


int TimeSpaceChromosome::hardFitness(Rules& r, QString* conflictsString){
	assert(r.initialized);
	assert(r.internalStructureComputed);

	/*if(this->_hardFitness>=0 && (this->timeChangedForMatrixCalculation || this->spaceChangedForMatrixCalculation)){
		cout<<"this->_hardFitness=="<<this->_hardFitness<<endl;
		cout<<"this->timeChangedForMatrixCalculation=="<<this->timeChangedForMatrixCalculation<<endl;
		cout<<"this->spaceChangedForMatrixCalculation=="<<this->spaceChangedForMatrixCalculation<<endl;
	}*/
	
	if(this->_hardFitness>=0)
		assert(this->timeChangedForMatrixCalculation==false && this->spaceChangedForMatrixCalculation==false);

	if(this->_hardFitness>=0 && conflictsString==NULL)
	//If you want to see the log, you have to recompute the fitness, even if it is
	//already computed
		return this->_hardFitness;
		
	//Repair the chromosome - we enter here with the assumption that
	//the time constraints of type ConstraintActivityPreferredTime,
	//ConstraintActivitiesSameTime and Constraint2ActivitiesConsecutive
	//do not contradict one with each other.
	//I had good reasons here not to repair activities that are scheduled too late
	//(that is, have the hour+duration>nHoursPerDay.
	//The reason is that there might be a mutation by swapping 2 activities,
	//and I want it to consider all the variants.
	//I might be wrong :-)
	
	//1)preferred times
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
	}
	
	this->timeChangedForMatrixCalculation=true;
	
	//repairing - space constraints
	for(int i=0; i<r.nInternalActivities; i++)
		if(r.fixedRoom[i]>=0)
			this->rooms[i]=r.fixedRoom[i];
			
	for(int i=0; i<r.nInternalActivities; i++)
		if(r.sameRoom[i]>=0 && this->rooms[r.sameRoom[i]]!=UNALLOCATED_SPACE){
			this->rooms[i]=this->rooms[r.sameRoom[i]];
			if(r.fixedRoom[i]>=0)
				assert(r.fixedRoom[i]==this->rooms[i]);
		}
		
	this->spaceChangedForMatrixCalculation=true;
	
	this->_hardFitness=0;
	//here we must not have compulsory activity preferred time nor 
	//compulsory activities same time
	//Also, here I compute soft fitness (for faster results,
	//I do not want to pass again through the constraints)
	this->_softFitness=0;
	for(int i=0; i<r.nInternalTimeConstraints; i++)
		if(r.internalTimeConstraintsList[i]->compulsory==true)
			this->_hardFitness += r.internalTimeConstraintsList[i]->fitness(*this, r, conflictsString);
		else
			//not logged here
			this->_softFitness += r.internalTimeConstraintsList[i]->fitness(*this, r, NULL);
			
	int days[MAX_ACTIVITIES], hours[MAX_ACTIVITIES];
	for(int i=0; i<r.nInternalActivities; i++)
		if(this->times[i]!=UNALLOCATED_TIME)
		{
			days[i]=this->times[i]%r.nDaysPerWeek;
			hours[i]=this->times[i]/r.nDaysPerWeek;
		}
		else{
			days[i]=UNALLOCATED_TIME;
			hours[i]=UNALLOCATED_TIME;	
		}
			
	for(int i=0; i<r.nInternalSpaceConstraints; i++)
		if(r.internalSpaceConstraintsList[i]->compulsory==true)
			this->_hardFitness += r.internalSpaceConstraintsList[i]->fitness(*this, r, days, hours, conflictsString);
		else
			//not logged here
			this->_softFitness += r.internalSpaceConstraintsList[i]->fitness(*this, r, days, hours, NULL);
		
	/*cout<<"after:"<<endl;
	cout<<"this->timeChangedForMatrixCalculation=="<<this->timeChangedForMatrixCalculation<<endl;
	cout<<"this->spaceChangedForMatrixCalculation=="<<this->spaceChangedForMatrixCalculation<<endl;*/
			
	return this->_hardFitness;
}

int TimeSpaceChromosome::softFitness(Rules& r, QString* conflictsString){
	assert(r.initialized);
	assert(r.internalStructureComputed);

	if(this->_softFitness>=0 && conflictsString==NULL)
	//If you want to see the log, you have to recompute the fitness, even if it is 
	//already computed
		return this->_softFitness;
	
	assert(this->_softFitness>=0); //I must have calculated this before - this is just a check
	//I prefer to compute the soft fitness also in the hard fitness calculation,
	//to avoid double passing through the constraints

	this->_softFitness=0;
	for(int i=0; i<r.nInternalTimeConstraints; i++)
		if(r.internalTimeConstraintsList[i]->compulsory==false)
			this->_softFitness += r.internalTimeConstraintsList[i]->fitness(*this, r, conflictsString);

	int days[MAX_ACTIVITIES], hours[MAX_ACTIVITIES];
	for(int i=0; i<r.nInternalActivities; i++)
		if(this->times[i]!=UNALLOCATED_TIME)
		{
			days[i]=this->times[i]%r.nDaysPerWeek;
			hours[i]=this->times[i]/r.nDaysPerWeek;
		}
		else{
			days[i]=UNALLOCATED_TIME;
			hours[i]=UNALLOCATED_TIME;	
		}
			
	for(int i=0; i<r.nInternalSpaceConstraints; i++)
		if(r.internalSpaceConstraintsList[i]->compulsory==false)
			this->_softFitness += r.internalSpaceConstraintsList[i]->fitness(*this, r, days, hours, conflictsString);

	return this->_softFitness;
}

//critical function here - must be optimized for speed
void TimeSpaceChromosome::crossover(Rules& r, TimeSpaceChromosome& c1, TimeSpaceChromosome& c2)
{
	this->twoPointCrossover(r, c1, c2);
}

//critical function here - must be optimized for speed
void TimeSpaceChromosome::onePointCrossover(Rules& r, TimeSpaceChromosome& c1, TimeSpaceChromosome& c2){
	assert(r.internalStructureComputed);

	int q=rand()%(r.nInternalActivities+1);
	int i;
	for(i=0; i<q; i++)
		this->times[i]=c1.times[i];
	//memcpy(times, c1.times, q*sizeof(times[0]));
	for(; i<r.nInternalActivities; i++)
		this->times[i]=c2.times[i];
	//memcpy(times+q, c2.times+q, (r.nActivities-q)*sizeof(times[0]));
	
	q=rand()%(r.nInternalActivities+1);
	for(i=0; i<q; i++)
		this->rooms[i]=c1.rooms[i];
	for(; i<r.nInternalActivities; i++)
		this->rooms[i]=c2.rooms[i];

	this->_hardFitness = this->_softFitness = -1;

	this->timeChangedForMatrixCalculation=true;
	this->spaceChangedForMatrixCalculation=true;	
}

//critical function here - must be optimized for speed
//code contributed by Volker Dirr
void TimeSpaceChromosome::twoPointCrossover(Rules& r, TimeSpaceChromosome& c1, TimeSpaceChromosome& c2)
{
	assert(r.internalStructureComputed);
	
	int p=rand()%(r.nInternalActivities);
	int q;
	
	do{
		q=rand()%(r.nInternalActivities);
	}while(p==q);
	 
	if(p>q){
		int w=q;
		q=p;
		p=w;
	}
	int z=rand()%2;
	int i;
	
	if(z==0){
		for(i=0; i<p; i++)
			this->times[i]=c1.times[i];
		for(; i<q; i++)
			this->times[i]=c2.times[i];
		for(; i<r.nInternalActivities; i++)
			this->times[i]=c1.times[i];
	}
	else{
		for(i=0; i<p; i++)
			this->times[i]=c2.times[i];
		for(; i<q; i++)
			this->times[i]=c1.times[i];
		for(; i<r.nInternalActivities; i++)
			this->times[i]=c2.times[i];
	}
	 
	this->_hardFitness = this->_softFitness = -1;
	this->timeChangedForMatrixCalculation=true;

	p=rand()%(r.nInternalActivities);
	
	do{
		q=rand()%(r.nInternalActivities);
	}while(p==q);
	 
	if(p>q){
		int w=q;
		q=p;
		p=w;
	}
	z=rand()%2;
	
	if(z==0){
		for(i=0; i<p; i++)
			this->rooms[i]=c1.rooms[i];
		for(; i<q; i++)
			this->rooms[i]=c2.rooms[i];
		for(; i<r.nInternalActivities; i++)
			this->rooms[i]=c1.rooms[i];
	}
	else{
		for(i=0; i<p; i++)
			this->rooms[i]=c2.rooms[i];
		for(; i<q; i++)
			this->rooms[i]=c1.rooms[i];
		for(; i<r.nInternalActivities; i++)
			this->rooms[i]=c2.rooms[i];
	}
	 
	//this->_hardFitness = this->_softFitness = -1;
	this->spaceChangedForMatrixCalculation=true;
}

//critical function here - must be optimized for speed
//code contributed by Volker Dirr
void TimeSpaceChromosome::uniformCrossover(Rules& r, TimeSpaceChromosome& c1, TimeSpaceChromosome& c2)
{
	assert(r.internalStructureComputed);
	
	int z;
	int dom=(rand()%40)+30;
	for(int i=0; i<r.nInternalActivities; i++){
		if(c1.times[i]==c2.times[i])
			this->times[i]=c1.times[i];
		else{
			z=rand()%100;
			if(z<dom)
				this->times[i]=c1.times[i];
			else
				this->times[i]=c2.times[i];
		}
	}
	
	this->_hardFitness = this->_softFitness = -1;
	this->timeChangedForMatrixCalculation=true;

	//int z;
	dom=(rand()%70)+30;
	for(int i=0; i<r.nInternalActivities; i++){
		if(c1.rooms[i]==c2.rooms[i])
			this->rooms[i]=c1.rooms[i];
		else{
			z=rand()%100;
			if(z<dom)
				this->rooms[i]=c1.rooms[i];
			else
				this->rooms[i]=c2.rooms[i];
		}
	}
	
	//this->_hardFitness = this->_softFitness = -1;
	this->spaceChangedForMatrixCalculation=true;
}

//critical function here - must be optimized for speed
void TimeSpaceChromosome::mutate1(Rules& r){
	int p,q,k;
	
	assert(r.internalStructureComputed);

	p=rand()%r.nInternalActivities;

	do{
		q=rand()%r.nInternalActivities;
	}while(p==q); //I think this is the fastest solution
		//because, suppose we have 10% of allowed values for q. Then, the probability 
		//that this step is performed 10 times is (0.9)^10=0.34...
		//obviusly, 10% is very little, generally we deal with more than 90% allowed values
	int m=rand()%100;
	if(m<50){
		k=this->times[p]; 
		this->times[p]=this->times[q]; 
		this->times[q]=k; //exchange the values
	}
	else{
		k=this->rooms[p];
		this->rooms[p]=this->rooms[q];
		this->rooms[q]=k;
	}

	this->_hardFitness = this->_softFitness = -1;

	this->timeChangedForMatrixCalculation=true;
	this->spaceChangedForMatrixCalculation=true;	
}

//critical function here - must be optimized for speed
void TimeSpaceChromosome::mutate2(Rules& r){
	assert(r.internalStructureComputed);
	
	int p,k;
	int m=rand()%100;
	if(m<50){
		p=rand()%r.nInternalActivities;
		k=rand()%r.nHoursPerWeek;
		this->times[p]=k;
	}
	else{
		p=rand()%r.nInternalActivities;
		k=rand()%r.nInternalRooms;
		this->rooms[p]=k;
	}

	this->_hardFitness = this->_softFitness = -1;

	this->timeChangedForMatrixCalculation=true;
	this->spaceChangedForMatrixCalculation=true;	
}

//critical function here - must be optimized for speed
int TimeSpaceChromosome::getTeachersMatrix(Rules& r, int16 a[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY]){
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
					if(act->parity==PARITY_WEEKLY){
						conflicts += tmp<2 ? tmp : 2;
						a[tch][day][hour+dd]+=2;
					}
					else{
						assert(act->parity==PARITY_FORTNIGHTLY);
						conflicts += tmp<2 ? 0 : 1;
						a[tch][day][hour+dd]++;
					}
				}
		}
		
	this->timeChangedForMatrixCalculation=false;
		
	return conflicts;
}

//critical function here - must be optimized for speed
int TimeSpaceChromosome::getSubgroupsMatrix(Rules& r, int16 a[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY]){
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
					if(act->parity == PARITY_WEEKLY){
						conflicts += tmp<2 ? tmp : 2;
						a[sg][day][hour+dd]+=2;
					}
					else{
						assert(act->parity == PARITY_FORTNIGHTLY);
						conflicts += tmp<2 ? 0 : 1;
						a[sg][day][hour+dd]++;
					}
				}
		}
		
	this->timeChangedForMatrixCalculation=false;
		
	return conflicts;
}

//The following 2 functions (GetTeachersTimetable & GetSubgroupsTimetable)
//are very similar to the above 2 ones (GetTeachersMatrix & GetSubgroupsMatrix)
void TimeSpaceChromosome::getTeachersTimetable(Rules& r, int16 a1[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY],int16 a2[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY]){
	//assert(HFitness()==0); //This is only for perfect solutions, that do not have any non-satisfied hard constrains

	assert(r.initialized);
	assert(r.internalStructureComputed);

	int i, j, k;
	for(i=0; i<r.nInternalTeachers; i++)
		for(j=0; j<r.nDaysPerWeek; j++)
			for(k=0; k<r.nHoursPerDay; k++)
				a1[i][j][k]=a2[i][j][k]=UNALLOCATED_ACTIVITY;

	Activity *act;
	for(i=0; i<r.nInternalActivities; i++) if(this->times[i]!=UNALLOCATED_TIME) {
		act=&r.internalActivitiesList[i];
		int hour=this->times[i]/r.nDaysPerWeek;
		int day=this->times[i]%r.nDaysPerWeek;
		for(int dd=0; dd < act->duration && hour+dd < r.nHoursPerDay; dd++)
			for(int ti=0; ti<act->nTeachers; ti++){
				int tch = act->teachers[ti]; //teacher index
				if(a1[tch][day][hour+dd]==UNALLOCATED_ACTIVITY)
					a1[tch][day][hour+dd]=i;
				else
					a2[tch][day][hour+dd]=i;
			}
	}
}

void TimeSpaceChromosome::getSubgroupsTimetable(Rules& r, int16 a1[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY],int16 a2[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY]){
	//assert(HFitness()==0);	//This is only for perfect solutions, that do not have any non-satisfied hard constrains

	assert(r.initialized);
	assert(r.internalStructureComputed);

	int i, j, k;
	for(i=0; i<r.nInternalSubgroups; i++)
		for(j=0; j<r.nDaysPerWeek; j++)
			for(k=0; k<r.nHoursPerDay; k++)
				a1[i][j][k]=a2[i][j][k]=UNALLOCATED_ACTIVITY;

	Activity *act;
	for(i=0; i<r.nInternalActivities; i++) if(this->times[i]!=UNALLOCATED_TIME) {
		act=&r.internalActivitiesList[i];
		int hour=this->times[i]/r.nDaysPerWeek;
		int day=this->times[i]%r.nDaysPerWeek;
		for(int dd=0; dd < act->duration && hour+dd < r.nHoursPerDay; dd++){
			for(int isg=0; isg < act->nSubgroups; isg++){ //isg -> index subgroup
				int sg = act->subgroups[isg]; //sg -> subgroup
				if(a1[sg][day][hour+dd]==UNALLOCATED_ACTIVITY)
					a1[sg][day][hour+dd]=i;
				else
					a2[sg][day][hour+dd]=i;
			}
		}
	}
}

int TimeSpaceChromosome::getRoomsMatrix(Rules& r, int16 a[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY])
{
	assert(r.initialized);
	assert(r.internalStructureComputed);

	int days[MAX_ACTIVITIES], hours[MAX_ACTIVITIES];
	for(int i=0; i<r.nInternalActivities; i++)
		if(this->times[i]!=UNALLOCATED_TIME)
		{
			days[i]=this->times[i]%r.nDaysPerWeek;
			hours[i]=this->times[i]/r.nDaysPerWeek;
		}
		else{
			days[i]=UNALLOCATED_TIME;
			hours[i]=UNALLOCATED_TIME;	
		}

	int conflicts=0;

	int i;
	for(i=0; i<r.nInternalRooms; i++)
		for(int j=0; j<r.nDaysPerWeek; j++)
			for(int k=0; k<r.nHoursPerDay; k++)
				a[i][j][k]=0;

	for(i=0; i<r.nInternalActivities; i++){
		int room=this->rooms[i];
		int hour = hours[i];
		int day = days[i];
		if(room!=UNALLOCATED_SPACE && hour!=UNALLOCATED_TIME && day!=UNALLOCATED_TIME) {
			Activity* act=&r.internalActivitiesList[i];
			for(int dd=0; dd<act->duration && hour+dd<r.nHoursPerDay; dd++){
				int tmp=a[room][day][hour+dd];
				if(act->parity==PARITY_WEEKLY){
					conflicts += tmp<2 ? tmp : 2;
					a[room][day][hour+dd]+=2;
				}
				else{
					assert(act->parity==PARITY_FORTNIGHTLY);
					conflicts += tmp<2 ? 0 : 1;
					a[room][day][hour+dd]++;
				}
			}
		}
	}

	this->spaceChangedForMatrixCalculation=false;

	return conflicts;
}

void TimeSpaceChromosome::getRoomsTimetable(Rules& r,int16 a1[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY],int16 a2[MAX_ROOMS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY])
{
	assert(r.initialized);
	assert(r.internalStructureComputed);

	int days[MAX_ACTIVITIES], hours[MAX_ACTIVITIES];
	for(int i=0; i<r.nInternalActivities; i++)
		if(this->times[i]!=UNALLOCATED_TIME)
		{
			days[i]=this->times[i]%r.nDaysPerWeek;
			hours[i]=this->times[i]/r.nDaysPerWeek;
		}
		else{
			days[i]=UNALLOCATED_TIME;
			hours[i]=UNALLOCATED_TIME;	
		}

	int i, j, k;
	for(i=0; i<r.nInternalRooms; i++)
		for(j=0; j<r.nDaysPerWeek; j++)
			for(k=0; k<r.nHoursPerDay; k++)
				a1[i][j][k]=a2[i][j][k]=UNALLOCATED_ACTIVITY;

	Activity *act;
	for(i=0; i<r.nInternalActivities; i++){
		act=&r.internalActivitiesList[i];
		int room=this->rooms[i];
		int day=days[i];
		int hour=hours[i];
		if(room!=UNALLOCATED_SPACE && day!=UNALLOCATED_TIME && hour!=UNALLOCATED_TIME){
			for(int dd=0; dd < act->duration && hour+dd < r.nHoursPerDay; dd++){
				if(a1[room][day][hour+dd]==UNALLOCATED_ACTIVITY)
					a1[room][day][hour+dd]=i;
				else
					a2[room][day][hour+dd]=i;
			}
		}
	}
}
