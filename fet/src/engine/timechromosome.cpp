/*
File timechromosome.cpp
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

#include "genetictimetable_defs.h"
#include "timechromosome.h"
#include "rules.h"
#include "timeconstraint.h"

int better(Rules& r, TimeChromosome& c1, TimeChromosome& c2){ //returns true if c1 is better than c2
	//Here the order is important, you have to compute firstly the hard fitness, then the soft fitness
	int hf1=c1.hardFitness(r);
	int sf1=c1.softFitness(r);
	int hf2=c2.hardFitness(r);
	int sf2=c2.softFitness(r);

	return better(hf1, sf1, hf2, sf2);
}

int better(int hf1, int sf1, int hf2, int sf2){
	return hf1<hf2 || hf1==hf2 && sf1<sf2;
}

//critical function here - must be optimized for speed
void TimeChromosome::copy(Rules& r, TimeChromosome& c){
	this->_hardFitness=c._hardFitness;
	this->_softFitness=c._softFitness;

	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++)
		this->times[i] = c.times[i];
	//memcpy(times, c.times, r.nActivities * sizeof(times[0]));
	
	this->timeChangedForMatrixCalculation=c.timeChangedForMatrixCalculation;
}

void TimeChromosome::init(Rules& r){
	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++)
			this->times[i]=UNALLOCATED_TIME;

	this->_hardFitness=this->_softFitness=-1;
	
	this->timeChangedForMatrixCalculation=true;
}

bool TimeChromosome::read(Rules& r, const QString& filename){
	assert(r.initialized);

	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly))
		assert(0);
	QTextStream tis(&file);
	this->read(r, tis);
	file.close();

	return true;
}

bool TimeChromosome::read(Rules &r, QTextStream &tis){
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
	
	this->_hardFitness=this->_softFitness=-1;
	
	this->timeChangedForMatrixCalculation=true;

	return true;
}

void TimeChromosome::write(Rules& r, const QString &filename){
	assert(r.initialized);

	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	this->write(r, tos);
	file.close();
}

void TimeChromosome::write(Rules& r, QTextStream &tos){
	assert(r.initialized);
	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++){
		tos<<this->times[i]<<endl;
	}
}

void TimeChromosome::makeTimesUnallocated(Rules& r){
	assert(r.initialized);
	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++)
			this->times[i]=UNALLOCATED_TIME;

	this->_hardFitness=this->_softFitness=-1;

	this->timeChangedForMatrixCalculation=true;
}

void TimeChromosome::makeTimesRandom(Rules& r){
	assert(r.initialized);
	assert(r.internalStructureComputed);

	for(int i=0; i<r.nInternalActivities; i++)
			this->times[i] = rand()%r.nHoursPerWeek;

	this->_hardFitness = this->_softFitness = -1;

	this->timeChangedForMatrixCalculation=true;
}


int TimeChromosome::hardFitness(Rules& r, QString* conflictsString){
	assert(r.initialized);
	assert(r.internalStructureComputed);

	/*if(this->timeChangedForMatrixCalculation && this->_hardFitness>=0){
		cout<<"this->_hardFitness=="<<this->_hardFitness<<endl;
		cout<<"this->timeChangedForMatrixCalculation=="<<this->timeChangedForMatrixCalculation<<endl;		
	}*/
	
	if(this->_hardFitness>=0)
		assert(this->timeChangedForMatrixCalculation==false);
		
	if(this->_hardFitness>=0 && conflictsString==NULL)
	//If you want to see the log, you have to recompute the fitness, even if it is
	//already computed
		return this->_hardFitness;
		
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
	
	this->_hardFitness=0;
	//here we must not have compulsory activity preferred time nor 
	//compulsory activities same time and/or hour
	//Also, here I compute soft fitness (for faster results,
	//I do not want to pass again through the constraints)
	this->_softFitness=0;
	for(int i=0; i<r.nInternalTimeConstraints; i++)
		if(r.internalTimeConstraintsList[i]->compulsory==true)
			this->_hardFitness += r.internalTimeConstraintsList[i]->fitness(*this, r, conflictsString);
		else
			//not logged here
			this->_softFitness += r.internalTimeConstraintsList[i]->fitness(*this, r, NULL);
			
	return this->_hardFitness;
}

int TimeChromosome::softFitness(Rules& r, QString* conflictsString){
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

	return this->_softFitness;
}

//critical function here - must be optimized for speed
void TimeChromosome::crossover(Rules& r, TimeChromosome& c1, TimeChromosome& c2)
{
	this->twoPointCrossover(r, c1, c2);
}

//critical function here - must be optimized for speed
void TimeChromosome::onePointCrossover(Rules& r, TimeChromosome& c1, TimeChromosome& c2)
{
	assert(r.internalStructureComputed);

	int q=rand()%(r.nInternalActivities+1);
	int i;
	for(i=0; i<q; i++)
		this->times[i]=c1.times[i];
	//memcpy(times, c1.times, q*sizeof(times[0]));
	for(; i<r.nInternalActivities; i++)
		this->times[i]=c2.times[i];
	//memcpy(times+q, c2.times+q, (r.nActivities-q)*sizeof(times[0]));

	this->_hardFitness = this->_softFitness = -1;

	this->timeChangedForMatrixCalculation=true;
}

//critical function here - must be optimized for speed
//code contributed by Volker Dirr
void TimeChromosome::twoPointCrossover(Rules& r, TimeChromosome& c1, TimeChromosome& c2)
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
}

//critical function here - must be optimized for speed
//code contributed by Volker Dirr
void TimeChromosome::uniformCrossover(Rules& r, TimeChromosome& c1, TimeChromosome& c2)
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
}

//critical function here - must be optimized for speed
void TimeChromosome::mutate1(Rules& r)
{
	int p,q,k;
	
	assert(r.internalStructureComputed);

	p=rand()%r.nInternalActivities;

	do{
		q=rand()%r.nInternalActivities;
	}while(p==q); //I think this is the fastest solution
		//because, suppose we have 10% of allowed values for q. Then, the probability 
		//that this step is performed 10 times is (0.9)^10=0.34...
		//obviusly, 10% is very little, generally we deal with more than 90% allowed values
	k=this->times[p]; 
	this->times[p]=this->times[q]; 
	this->times[q]=k; //exchange the values

	this->_hardFitness = this->_softFitness = -1;

	this->timeChangedForMatrixCalculation=true;
}

//critical function here - must be optimized for speed
void TimeChromosome::mutate2(Rules& r){
	assert(r.internalStructureComputed);
	
	int p,k;
	p=rand()%r.nInternalActivities;
	k=rand()%r.nHoursPerWeek;
	this->times[p]=k;

	this->_hardFitness = this->_softFitness = -1;

	this->timeChangedForMatrixCalculation=true;
}

//critical function here - must be optimized for speed
int TimeChromosome::getTeachersMatrix(Rules& r, int16 a[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY]){
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
int TimeChromosome::getSubgroupsMatrix(Rules& r, int16 a[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY]){
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
void TimeChromosome::getTeachersTimetable(Rules& r, int16 a1[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY],int16 a2[MAX_TEACHERS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY]){
	//assert(HFitness()==0); //This is only for perfect solutions, that do not have any non-satisfied hard constrains

	assert(r.initialized);
	assert(r.internalStructureComputed);

	int i, j, k;
	for(i=0; i<r.nInternalTeachers; i++)
		for(j=0; j<r.nDaysPerWeek; j++)
			for(k=0; k<r.nHoursPerDay; k++)
				a1[i][j][k]=a2[i][j][k]=UNALLOCATED_ACTIVITY;

	Activity *act;
	for(i=0; i<r.nInternalActivities; i++) 
		if(this->times[i]!=UNALLOCATED_TIME) {
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

void TimeChromosome::getSubgroupsTimetable(Rules& r, int16 a1[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY],int16 a2[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY]){
	//assert(HFitness()==0);	//This is only for perfect solutions, that do not have any non-satisfied hard constrains

	assert(r.initialized);
	assert(r.internalStructureComputed);

	int i, j, k;
	for(i=0; i<r.nInternalSubgroups; i++)
		for(j=0; j<r.nDaysPerWeek; j++)
			for(k=0; k<r.nHoursPerDay; k++)
				a1[i][j][k]=a2[i][j][k]=UNALLOCATED_ACTIVITY;

	Activity *act;
	for(i=0; i<r.nInternalActivities; i++)
		if(this->times[i]!=UNALLOCATED_TIME) {
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
