/*
File timepopulation.cpp
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

#include <iomanip>
#include <iostream>
using namespace std;

#include "genetictimetable_defs.h"
#include "genetictimetable.h"
#include "optimizetime.h"
#include "rules.h"

#include "optimizetime_data.h"

#include <QMutex>

extern QMutex mutex; //timetableallocatehoursform.cpp

#include <QList>
#include <QSet>

#include <QSemaphore>

extern QSemaphore semaphoreTime;
//extern QSemaphore stopSemaphore;
extern QSemaphore finishedSemaphore;

extern GeneticTimetable gt;

static bool swappedActivities[MAX_ACTIVITIES];

static bool foundGoodSwap;

static QSet<int> tlistSet[MAX_HOURS_PER_WEEK];

//not sure, it might be necessary 10*...
static int restoreActIndex[10*MAX_ACTIVITIES]; //the index of the act. to restore
static int restoreTime[10*MAX_ACTIVITIES]; //the time when to restore
static int nRestore;

//static int pred[MAX_ACTIVITIES];

static clock_t start_search_ticks;

static double time_limit;

static int level_limit;

bool randomswapfixedactivity(int ai, int timeslot);
void randomswap(int ai, int level);

//int minNDaysBrokenAllowancePercentage;

//for checking the no gaps and early for students
int nHoursScheduledForSubgroup[MAX_TOTAL_SUBGROUPS];
QList<int> activitiesForSubgroup[MAX_TOTAL_SUBGROUPS][MAX_HOURS_PER_WEEK];

//for checking the max gaps for teachers
int nHoursScheduledForTeacher[MAX_TEACHERS];
QList<int> activitiesForTeacher[MAX_TEACHERS][MAX_HOURS_PER_WEEK];


//if level==0, choose best position with lowest number
//of conflicting activities
static QList<int> conflActivitiesTimeSlot;
static int timeSlot;

//static bool fixedActivity[MAX_ACTIVITIES];

//static QList<int> triedTimes[MAX_ACTIVITIES];

//static QSet<int> triedRemovals[MAX_ACTIVITIES];
static int triedRemovals[MAX_ACTIVITIES][MAX_HOURS_PER_WEEK];

/*const int TABU=1000;
static int tabuActivities[TABU];
static int tabuTimes[TABU];
int crtTabuPos;*/

static int impossibleActivity;

static int invPermutation[MAX_ACTIVITIES];

const int INF=2000000000;

bool skipRandom(int weightPercentage)
{
	if(weightPercentage==-1)
		return true; //non-existing constraint

	if(weightPercentage<=rand()%100)
		return true;
	else
		return false;
}


OptimizeTime::OptimizeTime()
{
}

OptimizeTime::~OptimizeTime()
{
}

/*bool OptimizeTime::timesUp()
{
	mutex.lock();
	
	if(abortOptimization){
		mutex.unlock();
		return true;
	}
	
	mutex.unlock();
	return false;
}*/

bool OptimizeTime::precompute()
{
	return processTimeConstraints();
}

void OptimizeTime::optimize()
{
	impossibleActivity=false;

	//for(int i=0; i<gt.rules.nInternalActivities; i++)
	//	triedRemovals[i].clear();
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		for(int j=0; j<gt.rules.nHoursPerWeek; j++)
			triedRemovals[i][j]=0;

	//for(int i=0; i<gt.rules.nInternalActivities; i++)
	//	impossibleTimes[i].clear();

	/*for(int i=0; i<TABU; i++)
		tabuActivities[i]=tabuTimes[i]=-1;
	crtTabuPos=0;*/

	abortOptimization=false;

	c.makeTimesUnallocated(gt.rules);

	/*for(int i=0; i<gt.rules.nInternalActivities; i++)
		for(int j=0; j<gt.rules.nInternalActivities; j++)
			if(gt.rules.activitiesSimilar[i][j])
				assert(gt.rules.activityContained[i][j] && gt.rules.activityContained[j][i]);*/

//	for(int i=0; i<gt.rules.nInternalActivities; i++)
//		pred[i]=-1;
		
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		invPermutation[permutation[i]]=i;

	for(int i=0; i<gt.rules.nInternalActivities; i++)
		swappedActivities[permutation[i]]=false;

	tzset();
	time_t start_time;
	time(&start_time);
	
	time_limit=0.25;
	//time_limit=0.1;
	
	level_limit=14; //20; //16
	
	//ATENTION TO REPAIRING IN HARD FITNESS ROUTINE
	
	//int act_max=0, act_max_pos=-1;
	
	//for(int i=0; i<gt.rules.nInternalActivities; i++)
	//	fixedActivity[i]=false;
	
	for(int added_act=0; added_act<gt.rules.nInternalActivities; added_act++){
		prevvalue:
		
		mutex.lock();
		//cout<<"mutex locked - optimizetime 151"<<endl;
		if(abortOptimization){
			mutex.unlock();
			//cout<<"mutex unlocked - optimizetime 154"<<endl;
			//cout<<"stopsemaphore will be released - optimizetime.cpp line 153\n";
			//stopSemaphore.release();
			//cout<<"stopsemaphore released - optimizetime.cpp line 155\n";
			return;
		}
		time_t crt_time;
		time(&crt_time);		
		searchTime=crt_time-start_time;

		start_search_ticks=clock();

		/*double ftn;
		ftn=c.fitness(gt.rules);
		cout<<"line 168 - fitness=="<<ftn<<endl;*/

		for(int i=0; i<=added_act; i++)
		//for(int i=0; i<gt.rules.nInternalActivities; i++)
			swappedActivities[permutation[i]]=false;
		for(int i=added_act+1; i<gt.rules.nInternalActivities; i++)
			assert(!swappedActivities[permutation[i]]);

		cout<<endl<<"Trying to place activity number added_act=="<<added_act<<
		 "\nwith id=="<<gt.rules.internalActivitiesList[permutation[added_act]].id<<
		 ", from nInternalActivities=="<<gt.rules.nInternalActivities<<endl;
	 
		assert(c.times[permutation[added_act]]==UNALLOCATED_TIME);

		for(int i=0; i<gt.rules.nHoursPerWeek; i++)
			tlistSet[i].clear();
		for(int i=0; i<added_act; i++){
			if(c.times[permutation[i]]==UNALLOCATED_TIME)
				cout<<"ERROR: act with id=="<<gt.rules.internalActivitiesList[permutation[i]].id<<" has time unallocated"<<endl;
			assert(c.times[permutation[i]]!=UNALLOCATED_TIME);
			for(int j=0; j<gt.rules.internalActivitiesList[permutation[i]].duration; j++)
				tlistSet[c.times[permutation[i]]+j*gt.rules.nDaysPerWeek].insert(permutation[i]);
		}
				
		///////////////care for student (set) max hours daily
		for(int i=0; i<gt.rules.nInternalSubgroups; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				for(int k=0; k<gt.rules.nHoursPerDay; k++)
					subgroupsTimetable[i][j][k]=-1;
		for(int j=0; j<added_act; j++){
			int i=permutation[j];
			assert(c.times[i]!=UNALLOCATED_TIME);
			Activity* act=&gt.rules.internalActivitiesList[i];
			int hour=c.times[i]/gt.rules.nDaysPerWeek;
			int day=c.times[i]%gt.rules.nDaysPerWeek;
			for(int j=0; j<act->nSubgroups; j++){
				int sb=act->subgroups[j];
				for(int dd=0; dd<act->duration && hour+dd<gt.rules.nHoursPerDay; dd++){
					assert(subgroupsTimetable[sb][day][hour+dd]==-1);
					subgroupsTimetable[sb][day][hour+dd]=i;
				}
			}
		}

		///////////////care for teacher(s) max hours daily
		for(int i=0; i<gt.rules.nInternalTeachers; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				for(int k=0; k<gt.rules.nHoursPerDay; k++)
					teachersTimetable[i][j][k]=-1;
		for(int j=0; j<added_act; j++){
			int i=permutation[j];
			assert(c.times[i]!=UNALLOCATED_TIME);
			Activity* act=&gt.rules.internalActivitiesList[i];
			int hour=c.times[i]/gt.rules.nDaysPerWeek;
			int day=c.times[i]%gt.rules.nDaysPerWeek;
			for(int j=0; j<act->nTeachers; j++){
				int tc=act->teachers[j];
				for(int dd=0; dd<act->duration && hour+dd<gt.rules.nHoursPerDay; dd++){
					assert(teachersTimetable[tc][day][hour+dd]==-1);
					teachersTimetable[tc][day][hour+dd]=i;
				}
			}
		}
		for(int i=0; i<gt.rules.nInternalTeachers; i++){
			if(teachersMaxHoursDailyMaxHours[i]>=0){
				for(int d=0; d<gt.rules.nDaysPerWeek; d++){
					teachersNHoursPerDay[i][d]=0;
					teachersGapsPerDay[i][d]=0;
					int h;

					for(h=gt.rules.nHoursPerDay-1; h>=0; h--)
						if(teachersTimetable[i][d][h]>=0){
							assert(!breakDayHour[d][h]);
							break;
						}
							
					int cnt=0;
					for( ; h>=0; h--) if(!breakDayHour[d][h]){ //break is no gap, is not considered
						if(teachersTimetable[i][d][h]>=0){
							teachersNHoursPerDay[i][d]++;
							teachersGapsPerDay[i][d]+=cnt;
							cnt=0;
						}
						else
							cnt++;
					}
					
					teachersRealGapsPerDay[i][d]=0;
					if(teachersGapsPerDay[i][d]+teachersNHoursPerDay[i][d]>teachersMaxHoursDailyMaxHours[i])
						teachersRealGapsPerDay[i][d] = teachersGapsPerDay[i][d] + teachersNHoursPerDay[i][d] - teachersMaxHoursDailyMaxHours[i];
				}
			}
		}
		
		///////////////care for teachers information (to help with gaps)
		if(true /*teachersMaxGapsMaxGaps>=0*/){
			for(int i=0; i<gt.rules.nInternalTeachers; i++){
				nHoursScheduledForTeacher[i]=0;
				for(int j=0; j<gt.rules.nHoursPerWeek; j++)
					activitiesForTeacher[i][j].clear();
			}
			for(int i=0; i<added_act; i++){
				Activity* act=&gt.rules.internalActivitiesList[permutation[i]];
				for(int j=0; j<act->nTeachers; j++){
					int tc=act->teachers[j];
			
					for(int t=c.times[permutation[i]]; t<c.times[permutation[i]]+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
						assert(activitiesForTeacher[tc][t].indexOf(permutation[i])==-1);
						activitiesForTeacher[tc][t].append(permutation[i]);
					}
				}
			}
			for(int i=0; i<gt.rules.nInternalTeachers; i++){
				for(int day=0; day<gt.rules.nDaysPerWeek; day++){
					int t;
					int cnt=0;
					for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+day; t>=0; t-=gt.rules.nDaysPerWeek)
						if(activitiesForTeacher[i][t].count()>0){
							assert(!breakTime[t]);
							break;
						}
						
					for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){ //break is no gap
						if(activitiesForTeacher[i][t].count()>0){
							nHoursScheduledForTeacher[i]+=cnt+1;
							cnt=0;
						}
						else
							cnt++;
					}
				}
			}
		}
		///////////////
		
		///////////////care for subgroups information (to help with students no gaps and possibly early)
		for(int i=0; i<gt.rules.nInternalSubgroups; i++){
			nHoursScheduledForSubgroup[i]=0;
			for(int j=0; j<gt.rules.nHoursPerWeek; j++)
				activitiesForSubgroup[i][j].clear();
		}
		for(int i=0; i<added_act; i++){
			Activity* act=&gt.rules.internalActivitiesList[permutation[i]];
			for(int j=0; j<act->nSubgroups; j++){
				int isg=act->subgroups[j];
			
				for(int t=c.times[permutation[i]]; t<c.times[permutation[i]]+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
					assert(activitiesForSubgroup[isg][t].indexOf(permutation[i])==-1);
					activitiesForSubgroup[isg][t].append(permutation[i]);
				}
			}
		}
		for(int i=0; i<gt.rules.nInternalSubgroups; i++){
			//no gaps together with early
			if(subgroupsEarlyPercentage[i]>=0 && subgroupsNoGapsPercentage[i]>=0){
				int cnt=0;
				for(int day=0; day<gt.rules.nDaysPerWeek; day++){
					bool count=false;
					for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+day; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){ //break is no gap
						if(activitiesForSubgroup[i][t].count()>0)
							count=true;
						if(count)
							cnt++;
					}
				}
				nHoursScheduledForSubgroup[i]=cnt;
			}
			//only no gaps
			else if(subgroupsNoGapsPercentage[i]>=0){
				int t;
				int cntfinal=0;
				for(int day=0; day<gt.rules.nDaysPerWeek; day++){
					int cnt=0;
					for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+day; t>=0; t-=gt.rules.nDaysPerWeek){
						if(activitiesForSubgroup[i][t].count()>0){
							assert(!breakTime[t]);
							break;
						}
					}

					for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){ //break is no gap
						if(activitiesForSubgroup[i][t].count()>0){
							cntfinal+=cnt+1;
							cnt=0;
						}
						else
							cnt++;
					}
				}
				
				nHoursScheduledForSubgroup[i]=cntfinal;
			}
		}
		///////////////
		
		//////////////care for teachers max days per week
		for(int i=0; i<gt.rules.nInternalTeachers; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				teacherActivitiesOfTheDay[i][j].clear();
				
		for(int i=0; i<added_act; i++){
			//Activity* act=&gt.rules.internalActivitiesList[permutation[i]];
			int d=c.times[permutation[i]]%gt.rules.nDaysPerWeek;
			
			foreach(int j, teachersWithMaxDaysPerWeekForActivities[permutation[i]]){
				assert(teacherActivitiesOfTheDay[j][d].indexOf(permutation[i])==-1);
				teacherActivitiesOfTheDay[j][d].append(permutation[i]);
			}
		}
		//////////////
		
		foundGoodSwap=false;
	
		assert(!swappedActivities[permutation[added_act]]);
		swappedActivities[permutation[added_act]]=true;

		nRestore=0;
		randomswap(permutation[added_act], 0);
			
		//assert(!swappedActivities[permutation[added_act]]);
		//assert(swappedActivities[permutation[added_act]]);
		//swappedActivities[permutation[added_act]]=false;
				
		if(!foundGoodSwap){
			if(impossibleActivity){
				mutex.unlock();
				nDifficultActivities=1;
				difficultActivities[0]=permutation[added_act];
				emit(impossibleToSolve());
				
				return;
			}
		
			//update difficult activities (activities which are placed correctly so far, together with added_act
			nDifficultActivities=added_act+1;
			cout<<"nDifficultActivities=="<<nDifficultActivities<<endl;
			for(int j=0; j<=added_act; j++)
				difficultActivities[j]=permutation[j];
			
//////////////////////	
			assert(conflActivitiesTimeSlot.count()>0);
			
			cout<<"conflActivitiesTimeSlot.count()=="<<conflActivitiesTimeSlot.count()<<endl;
			foreach(int i, conflActivitiesTimeSlot){
				cout<<"Confl activity id:"<<gt.rules.internalActivitiesList[i].id;
				cout<<" time of this activity:"<<c.times[i]<<endl;
			}
			//cout<<endl;
			cout<<"timeSlot=="<<timeSlot<<endl;

			QList<int> ok;
			QList<int> confl;
			for(int j=0; j<added_act; j++)
				if(conflActivitiesTimeSlot.indexOf(permutation[j])!=-1){
					if(triedRemovals[permutation[j]][c.times[permutation[j]]]>0){
						cout<<"Warning - explored removal: id=="<<
						 gt.rules.internalActivitiesList[permutation[j]].id<<", time=="<<c.times[permutation[j]]
						 <<", times=="<<triedRemovals[permutation[j]][c.times[permutation[j]]]<<endl;
					}
					triedRemovals[permutation[j]][c.times[permutation[j]]]++;
				
					confl.append(permutation[j]);
				}
				else
					ok.append(permutation[j]);
				
			assert(confl.count()==conflActivitiesTimeSlot.count());
			
			int j=0;
			int tmp=permutation[added_act];
			foreach(int k, ok){
				permutation[j]=k;
				invPermutation[k]=j;
				j++;
			}
			int q=j;
			cout<<"q=="<<q<<endl;
			permutation[j]=tmp;
			invPermutation[tmp]=j;
			j++;
			cout<<"id of permutation[j=="<<j-1<<"]=="<<gt.rules.internalActivitiesList[permutation[j-1]].id<<endl;
			cout<<"conflicting:"<<endl;
			foreach(int k, confl){
				permutation[j]=k;
				invPermutation[k]=j;
				j++;
				cout<<"id of permutation[j=="<<j-1<<"]=="<<gt.rules.internalActivitiesList[permutation[j-1]].id<<endl;
			}
			assert(j==added_act+1);
			
			//check
			/*int pv[MAX_ACTIVITIES];
			for(int i=0; i<gt.rules.nInternalActivities; i++)
				pv[i]=0;
			for(int i=0; i<gt.rules.nInternalActivities; i++)
				pv[permutation[i]]++;
			for(int i=0; i<gt.rules.nInternalActivities; i++)
				assert(pv[i]==1);*/
			//

			cout<<"tmp represents activity with id=="<<gt.rules.internalActivitiesList[tmp].id;
			cout<<" initial time: "<<c.times[tmp];
			cout<<" final time: "<<timeSlot<<endl;
			c.times[tmp]=timeSlot;
			
			for(int i=q+1; i<=added_act; i++){
				c.times[permutation[i]]=UNALLOCATED_TIME;
			}
			c._fitness=-1;
			c.timeChangedForMatrixCalculation=true;			
				
			added_act=q+1;
			mutex.unlock();
	
			emit(activityPlaced(q+1));
			semaphoreTime.acquire();

			goto prevvalue;
//////////////////////
		}			
		else{ //if foundGoodSwap==true
			nPlacedActivities=added_act+1;
	
			mutex.unlock();
			//cout<<"mutex unlocked - optimizetime 382"<<endl;
			emit(activityPlaced(added_act+1));
			semaphoreTime.acquire();
			mutex.lock();
			//cout<<"mutex locked - optimizetime 386"<<endl;
			
			/*cout<<"Found good swap:"<<endl;*/
			
			/*QString str;
			cout<<"Fitness of current chromosome=="<<(ftn=c.fitness(gt.rules, &str))<<endl;*/
			//cout<<"str==------------------------"<<endl<<qPrintable(str)<<endl;
			/*if(ftn==0 || added_act==gt.rules.nInternalActivities && foundGoodSwap){*/
			if(added_act==gt.rules.nInternalActivities && foundGoodSwap){
				mutex.unlock();
				//cout<<"mutex unlocked - optimizetime 397"<<endl;
				break;
			}
			
			bool ok=true;
			for(int i=0; i<=added_act; i++){
				if(c.times[permutation[i]]==UNALLOCATED_TIME){
					cout<<"ERROR: act with id=="<<gt.rules.internalActivitiesList[permutation[i]].id<<" has time unallocated"<<endl;
					ok=false;
				}
				//assert(c.times[permutation[i]]!=UNALLOCATED_TIME);
			}
			assert(ok);
		}

		mutex.unlock();	
		//cout<<"mutex unlocked - optimizetime 403"<<endl;
	}

	mutex.lock();
	
	//c.write(gt.rules, "chromo.dat");
	
	mutex.unlock();

	time_t end_time;
	time(&end_time);
	searchTime=end_time-start_time;
	cout<<"Total searching time (seconds): "<<end_time-start_time<<endl;
	
	emit(simulationFinished());
	
	finishedSemaphore.release();
}

void OptimizeTime::moveActivity(int ai, int fromslot, int toslot)
{
	Activity* act=&gt.rules.internalActivitiesList[ai];

	//cout<<"here: id of act=="<<act->id<<", fromslot=="<<fromslot<<", toslot=="<<toslot<<endl;

	assert(fromslot==c.times[ai]);
	if(fromslot!=UNALLOCATED_TIME){
		int d=fromslot%gt.rules.nDaysPerWeek;
		int h=fromslot/gt.rules.nDaysPerWeek;
		
		for(int t=fromslot; t<fromslot+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
			int tt=tlistSet[t].remove(ai);
			assert(tt==1);
		}

		//update for students (set) max hours daily
		for(int q=0; q<act->nSubgroups; q++){
			int sb=act->subgroups[q];	
			//if(1 || subgroupsMaxHoursDailyMaxHours[sb]>=0){
			if(1){
			//the timetable must be computed also for other purposes
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
					assert(subgroupsTimetable[sb][d][h+dd]==ai);
					subgroupsTimetable[sb][d][h+dd]=-1;
				}
			
				/*subgroupsNHoursPerDay[sb][d]=0;
				//subgroupsGapsPerDay[sb][d]=0;

				int hh;

				for(hh=gt.rules.nHoursPerDay-1; hh>=0; hh--)
					if(subgroupsTimetable[sb][d][hh]>=0)
						break;
						
				int cnt=0;
				for( ; hh>=0; hh--){
					if(subgroupsTimetable[sb][d][hh]>=0){
						subgroupsNHoursPerDay[sb][d]++;
						//subgroupsGapsPerDay[sb][d]+=cnt;
						cnt=0;
					}
					else
						cnt++;
				}*/
				
				//if(subgroupsEarlyPercentage[sb]>=0)
				//	subgroupsGapsPerDay[sb][d]+=cnt;
			}
		}

		//update for teacher(s) max hours daily
		for(int q=0; q<act->nTeachers; q++){
			int tch=act->teachers[q];	
			//the timetable must be computed also for other purposes
			//if(1 || teachersMaxHoursDailyMaxHours[tch]>=0){
			if(1){
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
					assert(teachersTimetable[tch][d][h+dd]==ai);
					teachersTimetable[tch][d][h+dd]=-1;
				}
			
				teachersNHoursPerDay[tch][d]=0;
				teachersGapsPerDay[tch][d]=0;

				int hh;

				for(hh=gt.rules.nHoursPerDay-1; hh>=0; hh--)
					if(teachersTimetable[tch][d][hh]>=0){
						assert(!breakDayHour[d][hh]);
						break;
					}
						
				int cnt=0;
				for( ; hh>=0; hh--) if(!breakDayHour[d][hh]) { //break is no gap, no activity, nothing
					if(teachersTimetable[tch][d][hh]>=0){
						teachersNHoursPerDay[tch][d]++;
						teachersGapsPerDay[tch][d]+=cnt;
						cnt=0;
					}
					else
						cnt++;
				}

				teachersRealGapsPerDay[tch][d]=0;
				if(teachersGapsPerDay[tch][d]+teachersNHoursPerDay[tch][d]>teachersMaxHoursDailyMaxHours[tch])
					teachersRealGapsPerDay[tch][d] = teachersGapsPerDay[tch][d] + teachersNHoursPerDay[tch][d] - teachersMaxHoursDailyMaxHours[tch];
			}
		}

		//update teachers' list of activities for each day
		/////////////////
		foreach(int tch, teachersWithMaxDaysPerWeekForActivities[ai]){
			int tt=teacherActivitiesOfTheDay[tch][d].removeAll(ai);
			assert(tt==1);
		}
		/////////////////
		
		//update teachers' count of occupied activities
		/////////////////
		if(true /*teachersMaxGapsMaxGaps>=0*/){
			for(int i=0; i<act->nTeachers; i++){
				int tc=act->teachers[i];
	
				int t;
				int cntfinal=0;
				int cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForTeacher[tc][t].count()>0){
						assert(!breakTime[t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]) { //break is no gap
					if(activitiesForTeacher[tc][t].count()>0){
						cntfinal+=cnt+1;
						cnt=0;
					}
					else
						cnt++;
				}

				for(t=fromslot; t<fromslot+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
					int tt=activitiesForTeacher[tc][t].removeAll(ai);
					assert(tt==1);
				}

				cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForTeacher[tc][t].count()>0){
						assert(!breakTime[t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){  //break is no gap
					if(activitiesForTeacher[tc][t].count()>0){
						cntfinal-=cnt+1;
						cnt=0;
					}
					else
						cnt++;
				}

				assert(cntfinal>=0);
			
				nHoursScheduledForTeacher[tc]-=cntfinal;
				assert(nHoursScheduledForTeacher[tc]>=0);
			}
		}
		/////////////////////
		
		//update subgroups count of occupied activities
		/////////////////
		//int d=fromslot%gt.rules.nDaysPerWeek;
		//int h=fromslot/gt.rules.nDaysPerWeek;
		for(int i=0; i<act->nSubgroups; i++){
			int isg=act->subgroups[i];
			
			if(subgroupsEarlyPercentage[isg]>=0 && subgroupsNoGapsPercentage[isg]>=0){
			//students early+no gaps
				assert(subgroupsEarlyPercentage[isg]==subgroupsNoGapsPercentage[isg]);
			
				bool count=false;
				int cnt=0;
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){ //break is no gap
					if(activitiesForSubgroup[isg][t].count()>0)
						count=true;
					if(count)
						cnt++;
				}

				for(int t=fromslot; t<fromslot+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
					int tt=activitiesForSubgroup[isg][t].removeAll(ai);
					assert(tt==1);
				}
		
				count=false;
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){
					if(activitiesForSubgroup[isg][t].count()>0)
						count=true;
					if(count)
						cnt--;
				}
				
				assert(cnt>=0);
				
				nHoursScheduledForSubgroup[isg]-=cnt;
				assert(nHoursScheduledForSubgroup[isg]>=0);
			}
			else if(subgroupsEarlyPercentage[isg]==-1 && subgroupsNoGapsPercentage[isg]>=0){
			//only students no gaps
				int t;
				int cntfinal=0;
				int cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForSubgroup[isg][t].count()>0){
						assert(!breakTime[t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){ //break is no gap
					if(activitiesForSubgroup[isg][t].count()>0){
						cntfinal+=cnt+1;
						cnt=0;
					}
					else
						cnt++;
				}

				for(t=fromslot; t<fromslot+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
					int tt=activitiesForSubgroup[isg][t].removeAll(ai);
					assert(tt==1);
				}

				cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForSubgroup[isg][t].count()>0){
						assert(!breakTime[t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){
					if(activitiesForSubgroup[isg][t].count()>0){
						cntfinal-=cnt+1;
						cnt=0;
					}
					else
						cnt++;
				}

				assert(cntfinal>=0);
			
				nHoursScheduledForSubgroup[isg]-=cntfinal;
				assert(nHoursScheduledForSubgroup[isg]>=0);
			}
			else
				assert(subgroupsEarlyPercentage[isg]==-1 && subgroupsNoGapsPercentage[isg]==-1);
		/////////////////////
		}
	}
	
	c.times[ai]=toslot;
	c._fitness=-1;
	c.timeChangedForMatrixCalculation=true;
	
	if(toslot!=UNALLOCATED_TIME){
		int d=toslot%gt.rules.nDaysPerWeek;
		int h=toslot/gt.rules.nDaysPerWeek;
		
		for(int t=toslot; t<toslot+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
			assert(!tlistSet[t].contains(ai));
			tlistSet[t].insert(ai);
		}

		//update for students (set) max hours daily
		for(int q=0; q<act->nSubgroups; q++){
			int sb=act->subgroups[q];
			//if(1 || subgroupsMaxHoursDailyMaxHours[sb]>=0){
			//the timetable must be computed also for other purposes
			if(1){
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
					assert(subgroupsTimetable[sb][d][h+dd]==-1);
					subgroupsTimetable[sb][d][h+dd]=ai;
				}
			}
		}

		//update for teacher(s) max hours daily
		for(int q=0; q<act->nTeachers; q++){
			int tch=act->teachers[q];
			//the timetable must be computed also for other purposes
			//if(1 || teachersMaxHoursDailyMaxHours[tch]>=0){
			if(1){
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
					assert(teachersTimetable[tch][d][h+dd]==-1);
					teachersTimetable[tch][d][h+dd]=ai;
				}
			
				teachersNHoursPerDay[tch][d]=0;
				teachersGapsPerDay[tch][d]=0;

				int hh;

				for(hh=gt.rules.nHoursPerDay-1; hh>=0; hh--)
					if(teachersTimetable[tch][d][hh]>=0){
						assert(!breakDayHour[d][hh]);
						break;
					}
						
				int cnt=0;
				for( ; hh>=0; hh--) if(!breakDayHour[d][hh]) { //break is no gap
					if(teachersTimetable[tch][d][hh]>=0){
						teachersNHoursPerDay[tch][d]++;
						teachersGapsPerDay[tch][d]+=cnt;
						cnt=0;
					}
					else
						cnt++;
				}

				teachersRealGapsPerDay[tch][d]=0;
				if(teachersGapsPerDay[tch][d]+teachersNHoursPerDay[tch][d]>teachersMaxHoursDailyMaxHours[tch])
					teachersRealGapsPerDay[tch][d] = teachersGapsPerDay[tch][d] + teachersNHoursPerDay[tch][d] - teachersMaxHoursDailyMaxHours[tch];
			}
		}

		//update teachers' list of activities for each day
		/////////////////
		foreach(int tch, teachersWithMaxDaysPerWeekForActivities[ai]){
			assert(teacherActivitiesOfTheDay[tch][d].indexOf(ai)==-1);
			teacherActivitiesOfTheDay[tch][d].append(ai);
		}
		/////////////////
		
		//update teachers' count of occupied activities
		/////////////////
		if(true /*teachersMaxGapsMaxGaps>=0*/){
			for(int i=0; i<act->nTeachers; i++){
				int tc=act->teachers[i];
	
				int t;
				int cntfinal=0;
				int cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForTeacher[tc][t].count()>0){
						assert(!breakTime[t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){ //break is no gap
					if(activitiesForTeacher[tc][t].count()>0){
						cntfinal-=cnt+1;
						cnt=0;
					}
					else
						cnt++;
				}
	
				for(int t=toslot; t<toslot+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
					int tt=activitiesForTeacher[tc][t].indexOf(ai);
					assert(tt==-1);
					activitiesForTeacher[tc][t].append(ai);
				}
	
				cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForTeacher[tc][t].count()>0){
						assert(!breakTime[t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){
					if(activitiesForTeacher[tc][t].count()>0){
						cntfinal+=cnt+1;
						cnt=0;
					}
					else
						cnt++;
				}
	
				assert(cntfinal>=0);
				
				nHoursScheduledForTeacher[tc]+=cntfinal;
			
				if(teachersMaxGapsPercentage[tc]==100)
					assert(nHoursScheduledForTeacher[tc] <= nHoursPerTeacher[tc] + teachersMaxGapsMaxGaps[tc]);
			}
		}
		/////////////////////

		//update subgroups count of occupied activities
		/////////////////
		//int d=toslot%gt.rules.nDaysPerWeek;
		//int h=toslot/gt.rules.nDaysPerWeek;
		for(int i=0; i<act->nSubgroups; i++){
			int isg=act->subgroups[i];

			if(subgroupsEarlyPercentage[isg]>=0 && subgroupsNoGapsPercentage[isg]>=0){
			//students early+no gaps
				bool count=false;
				int cnt=0;
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){ //break is no gap
					if(activitiesForSubgroup[isg][t].count()>0)
						count=true;
					if(count)
						cnt--;
				}

				for(int t=toslot; t<toslot+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
					int tt=activitiesForSubgroup[isg][t].indexOf(ai);
					assert(tt==-1);
					activitiesForSubgroup[isg][t].append(ai);
				}
		
				count=false;
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){
					if(activitiesForSubgroup[isg][t].count()>0)
						count=true;
					if(count)
						cnt++;
				}
			
				assert(cnt>=0);
			
				nHoursScheduledForSubgroup[isg]+=cnt;
				/*if(!(nHoursScheduledForSubgroup[isg]<=nHoursPerSubgroup[isg])){
					cout<<"isg=="<<isg<<endl;
					cout<<"nHoursScheduledForSubgroup[isg]=="<<nHoursScheduledForSubgroup[isg]<<endl;
					cout<<"nHoursPerSubgroup[isg]=="<<nHoursPerSubgroup[isg]<<endl;
				}*/
			}
			else if(subgroupsEarlyPercentage[isg]==-1 && subgroupsNoGapsPercentage[isg]>=0){
			//only students no gaps
				int t;
				int cntfinal=0;
				int cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForSubgroup[isg][t].count()>0){
						assert(!breakTime[t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){ //break is no gap
					if(activitiesForSubgroup[isg][t].count()>0){
						cntfinal-=cnt+1;
						cnt=0;
					}
					else
						cnt++;
				}

				for(int t=toslot; t<toslot+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
					int tt=activitiesForSubgroup[isg][t].indexOf(ai);
					assert(tt==-1);
					activitiesForSubgroup[isg][t].append(ai);
				}

				cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForSubgroup[isg][t].count()>0){
						assert(!breakTime[t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){
					if(activitiesForSubgroup[isg][t].count()>0){
						cntfinal+=cnt+1;
						cnt=0;
					}
					else
						cnt++;
				}

				assert(cntfinal>=0);
			
				nHoursScheduledForSubgroup[isg]+=cntfinal;
			}
			else
				assert(subgroupsEarlyPercentage[isg]==-1 && subgroupsNoGapsPercentage[isg]==-1);
			
			if(subgroupsNoGapsPercentage[isg]==100)
				assert(nHoursScheduledForSubgroup[isg]<=nHoursPerSubgroup[isg]);
		}
		/////////////////////		
	}
}

void OptimizeTime::randomswap(int ai, int level){
	//cout<<"level=="<<level<<endl;

	if(level==0){
		conflActivitiesTimeSlot.clear();
		timeSlot=-1;
	}

	if(level>=level_limit){
		return;
	}
	
	clock_t crt_search_ticks=clock();
	if(double(crt_search_ticks-start_search_ticks)/CLOCKS_PER_SEC >= time_limit)
		return;
		
	Activity* act=&gt.rules.internalActivitiesList[ai];
		
	//cout<<"ai=="<<ai<<", corresponding to id=="<<gt.rules.internalActivitiesList[ai].id<<", level=="<<level<<endl;

	//generate random permutation like in CLR second edition
	//this is used to scan times in random order
	int perm[MAX_HOURS_PER_WEEK];
	
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		perm[i]=i;
	for(int i=0; i<gt.rules.nHoursPerWeek; i++){
		int t=perm[i];
		int r=rand()%(gt.rules.nHoursPerWeek-i);
		perm[i]=perm[i+r];
		perm[i+r]=t;
	}
	
	/*int checkPerm[MAX_HOURS_PER_WEEK];
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		checkPerm[i]=false;
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		checkPerm[perm[i]]=true;
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		assert(checkPerm[i]==true);*/
	
	/*
	cout<<"Perm: ";
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		cout<<", perm["<<i<<"]="<<perm[i];
	cout<<endl;
	*/
	
	//record the conflicting activities for each timeslot
	QList<int> conflActivities[MAX_HOURS_PER_WEEK];
	int conflPerm[MAX_HOURS_PER_WEEK]; //the permutation in increasing order of number of conflicting activities
	int nConflActivities[MAX_HOURS_PER_WEEK];
	
	for(int n=0; n<gt.rules.nHoursPerWeek; n++){
		int newtime=perm[n];
		
		nConflActivities[newtime]=0;

		int d=newtime%gt.rules.nDaysPerWeek;
		int h=newtime/gt.rules.nDaysPerWeek;

/////////////////////////////////////////////////////////////////////////////////////////////

		//not too late
		if(h+act->duration>gt.rules.nHoursPerDay){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed (tch&st not available, break, act(s) preferred time(s))
		if(!skipRandom(allowedTimesPercentages[ai][newtime])){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//care about basic time constraints
		bool okbasictime=true;
		for(int t1=newtime; t1<newtime+act->duration*gt.rules.nDaysPerWeek; t1+=gt.rules.nDaysPerWeek)
			foreach(int ai2, tlistSet[t1]){
				if(ai!=ai2 && !skipRandom(activitiesConflictingPercentage[ai][ai2])){
					if(swappedActivities[ai2]){
						okbasictime=false;
						goto impossiblebasictime;
					}
				
					if(conflActivities[newtime].indexOf(ai2)==-1){
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(nConflActivities[newtime]==conflActivities[newtime].count());
					}
				}
			}
impossiblebasictime:
		if(!okbasictime){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
				
		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//care about min n days
		bool okmindays=true;
		for(int i=0; i<minNDaysListOfActivities[ai].count(); i++){
			int ai2=minNDaysListOfActivities[ai].at(i);
			int md=minNDaysListOfMinDays[ai].at(i);
			int ai2time=c.times[ai2];
			if(ai2time!=UNALLOCATED_TIME){
				int d2=ai2time%gt.rules.nDaysPerWeek;
				int h2=ai2time/gt.rules.nDaysPerWeek;
				if(md>abs(d-d2)){
					bool okrand=skipRandom(minNDaysListOfWeightPercentages[ai].at(i));
				
					//broken min n days - there is a minNDaysBrokenAllowancePercentage% chance to place them adjacent
					
					if(minNDaysListOfConsecutiveIfSameDay[ai].at(i)==true){ //must place them adjacent if on same day
						if(okrand && 
						 ( (d==d2 && (h+act->duration==h2 || h2+gt.rules.internalActivitiesList[ai2].duration==h)) || d!=d2 )
						 ){
						}
						else{
							if(swappedActivities[ai2]){
								okmindays=false;
								goto impossiblemindays;
							}
							
							if(conflActivities[newtime].indexOf(ai2)==-1){
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(nConflActivities[newtime]==conflActivities[newtime].count());
							}
						}
					}					
					else{ //can place them anywhere
						if(okrand){
						}
						else{
							if(swappedActivities[ai2]){
								okmindays=false;
								goto impossiblemindays;
							}
							
							if(conflActivities[newtime].indexOf(ai2)==-1){
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(nConflActivities[newtime]==conflActivities[newtime].count());
							}
						}
					}					
				}
			}
		}
impossiblemindays:
		if(!okmindays){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from same starting time
		bool oksamestartingtime=true;
		for(int i=0; i<activitiesSameStartingTimeActivities[ai].count(); i++){
			int ai2=activitiesSameStartingTimeActivities[ai].at(i);
			int perc=activitiesSameStartingTimePercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				if(newtime!=c.times[ai2] && !skipRandom(perc)){
					assert(ai2!=ai);
					
					if(swappedActivities[ai2]){
						oksamestartingtime=false;
						goto impossiblesamestartingtime;
					}
					
					if(conflActivities[newtime].indexOf(ai2)==-1){
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
					}
				}
			}
		}
impossiblesamestartingtime:
		if(!oksamestartingtime){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from same starting hour
		bool oksamestartinghour=true;
		for(int i=0; i<activitiesSameStartingHourActivities[ai].count(); i++){
			int ai2=activitiesSameStartingHourActivities[ai].at(i);
			int perc=activitiesSameStartingHourPercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				if((newtime/gt.rules.nDaysPerWeek)!=(c.times[ai2]/gt.rules.nDaysPerWeek) && !skipRandom(perc)){
					if(swappedActivities[ai2]){
						oksamestartinghour=false;
						goto impossiblesamestartinghour;
					}
				
					if(conflActivities[newtime].indexOf(ai2)==-1){
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
					}
				}
			}
		}
impossiblesamestartinghour:
		if(!oksamestartinghour){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from not overlapping
		bool oknotoverlapping=true;
		for(int i=0; i<activitiesNotOverlappingActivities[ai].count(); i++){
			int ai2=activitiesNotOverlappingActivities[ai].at(i);
			int perc=activitiesNotOverlappingPercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
				//int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
				if(d==d2){
					int st=newtime;
					int en=st+gt.rules.nDaysPerWeek*act->duration;
					int st2=c.times[ai2];
					int en2=st2+gt.rules.nDaysPerWeek*gt.rules.internalActivitiesList[ai2].duration;
					if(!(en<=st2 || en2<=st) && !skipRandom(perc)){
						assert(ai2!=ai);
						
						if(swappedActivities[ai2]){
							oknotoverlapping=false;
							goto impossiblenotoverlapping;
						}
						
						if(conflActivities[newtime].indexOf(ai2)==-1){
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						}
					}
				}
			}
		}
impossiblenotoverlapping:
		if(!oknotoverlapping){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from students (set) max hours daily
		bool okstudentsmaxhoursdaily=true;
		for(int q=0; q<act->nSubgroups; q++){
			int sb=act->subgroups[q];
			if(subgroupsMaxHoursDailyMaxHours[sb]>=0){
				int initialGaps=0;
				int initialNHours=0;
				int finalGaps=0;
				int finalNHours=0;
				
				int j;
				for(j=gt.rules.nHoursPerDay-1; j>=0; j--)
					if(subgroupsTimetable[sb][d][j]>=0){
						assert(!breakDayHour[d][j]);
						break;
					}
				
				int cnt=0;
				
				for( ; j>=0; j--) if(!breakDayHour[d][j]){ //break is no gap, no activity
					if(subgroupsTimetable[sb][d][j]>=0){
						initialNHours++;
						initialGaps+=cnt;
						cnt=0;
					}
					else{
						cnt++;
					}
				}
				if(subgroupsEarlyPercentage[sb]>=0)
					initialGaps+=cnt;

				//
				for(j=gt.rules.nHoursPerDay-1; j>=0; j--)
					if(subgroupsTimetable[sb][d][j]>=0 || j>=h && j<h+act->duration){
						//the slot is possible, so no break here. If there was a break, we would have
						//failed the test 40 lines above (line 1128 currently)
						assert(!breakDayHour[d][j]);
						break;
					}
				
				cnt=0;
				
				for( ; j>=0; j--) if(!breakDayHour[d][j]){
					if(subgroupsTimetable[sb][d][j]>=0 || j>=h && j<h+act->duration){
						finalNHours++;
						finalGaps+=cnt;
						cnt=0;
					}
					else{
						cnt++;
					}
				}				
				if(subgroupsEarlyPercentage[sb]>=0)
					finalGaps+=cnt;
			
				if(subgroupsNoGapsPercentage[sb]==100){ //no gaps or early+no gaps 100%
					if(finalNHours+finalGaps>subgroupsMaxHoursDailyMaxHours[sb]){
						bool skip=skipRandom(subgroupsMaxHoursDailyPercentages[sb]);
						if(!skip){
							if(subgroupsMaxHoursDailyMaxHours[sb]<act->duration){
								okstudentsmaxhoursdaily=false;
								goto impossiblestudentsmaxhoursdaily;
							}
						
							for(int j=0; j<gt.rules.nHoursPerDay; j++){
								int ai2=subgroupsTimetable[sb][d][j];
							
								if(ai2>=0){
									if(swappedActivities[ai2]){
										okstudentsmaxhoursdaily=false;
										goto impossiblestudentsmaxhoursdaily;
									}
								
									if(conflActivities[newtime].indexOf(ai2)==-1){
										conflActivities[newtime].append(ai2);
										nConflActivities[newtime]++;
										assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									}
								}
							}
						}
					}
				}
				else{
					if(finalNHours>subgroupsMaxHoursDailyMaxHours[sb]){
						bool skip=skipRandom(subgroupsMaxHoursDailyPercentages[sb]);
						if(!skip){
							if(subgroupsMaxHoursDailyMaxHours[sb]<act->duration){
								okstudentsmaxhoursdaily=false;
								goto impossiblestudentsmaxhoursdaily;
							}
						
							for(int j=0; j<gt.rules.nHoursPerDay; j++){
								int ai2=subgroupsTimetable[sb][d][j];
							
								if(ai2>=0){
									if(swappedActivities[ai2]){
										okstudentsmaxhoursdaily=false;
										goto impossiblestudentsmaxhoursdaily;
									}
								
									if(conflActivities[newtime].indexOf(ai2)==-1){
										conflActivities[newtime].append(ai2);
										nConflActivities[newtime]++;
										assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									}
								}
							}
						}
					}
				}
			}
		}
impossiblestudentsmaxhoursdaily:
		if(!okstudentsmaxhoursdaily){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from teachers max hours daily
		bool okteachersmaxhoursdaily=true;
		for(int q=0; q<act->nTeachers; q++){
			int i=act->teachers[q];
			if(teachersMaxHoursDailyMaxHours[i]>=0){
				int initialGaps=0;
				int initialNHours=0;
				int finalGaps=0;
				int finalNHours=0;
				
				int j;
				for(j=gt.rules.nHoursPerDay-1; j>=0; j--)
					if(teachersTimetable[i][d][j]>=0){
						assert(!breakDayHour[d][j]);
						break;
					}
				
				int cnt=0;
				
				for( ; j>=0; j--) if(!breakDayHour[d][j]){ //break is no gap, no activity
					if(teachersTimetable[i][d][j]>=0){
						initialNHours++;
						initialGaps+=cnt;
						cnt=0;
					}
					else{
						cnt++;
					}
				}

				//
				for(j=gt.rules.nHoursPerDay-1; j>=0; j--)
					if(teachersTimetable[i][d][j]>=0 || j>=h && j<h+act->duration){
						assert(!breakDayHour[d][j]);
						break;
					}
				
				cnt=0;
				
				for( ; j>=0; j--) if(!breakDayHour[d][j]){
					if(teachersTimetable[i][d][j]>=0 || j>=h && j<h+act->duration){
						finalNHours++;
						finalGaps+=cnt;
						cnt=0;
					}
					else{
						cnt++;
					}
				}
				
				if(teachersMaxGapsPercentage[i]==-1){
					//no max gaps constraint
					if(finalNHours>teachersMaxHoursDailyMaxHours[i]){
						bool skip=skipRandom(teachersMaxHoursDailyPercentages[i]);
						if(!skip){
							if(teachersMaxHoursDailyMaxHours[i]<act->duration){
								okteachersmaxhoursdaily=false;
								goto impossibleteachersmaxhoursdaily;
							}

							for(int j=0; j<gt.rules.nHoursPerDay; j++){
								int ai2=teachersTimetable[i][d][j];
							
								if(ai2>=0){
									if(swappedActivities[ai2]){
										okteachersmaxhoursdaily=false;
										goto impossibleteachersmaxhoursdaily;
									}
								
									if(conflActivities[newtime].indexOf(ai2)==-1){
										conflActivities[newtime].append(ai2);
										nConflActivities[newtime]++;
										assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									}
								}
							}
						}
					}
				}
				else{
					int nrg=teachersMaxGapsMaxGaps[i]; //number remaining gaps
					for(int q=0; q<gt.rules.nDaysPerWeek; q++)
						if(q!=d)
							nrg-=teachersRealGapsPerDay[i][q];
					
					if(teachersMaxGapsPercentage[i]==100 && teachersMaxHoursDailyPercentages[i]==100)
						assert(nrg>=0);
						
					if(nrg<0)
						nrg=0;
						
					if(finalNHours>teachersMaxHoursDailyMaxHours[i] || finalNHours+finalGaps>teachersMaxHoursDailyMaxHours[i]+nrg){
						bool skip=skipRandom(teachersMaxHoursDailyPercentages[i]);
						if(!skip){
							if(teachersMaxHoursDailyMaxHours[i]<act->duration){
								okteachersmaxhoursdaily=false;
								goto impossibleteachersmaxhoursdaily;
							}

							for(int j=0; j<gt.rules.nHoursPerDay; j++){
								int ai2=teachersTimetable[i][d][j];
							
								if(ai2>=0){
									if(swappedActivities[ai2]){
										okteachersmaxhoursdaily=false;
										goto impossibleteachersmaxhoursdaily;
									}
								
									if(conflActivities[newtime].indexOf(ai2)==-1){
										conflActivities[newtime].append(ai2);
										nConflActivities[newtime]++;
										assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									}
								}
							}
						}
					}
				}
			}
		}
impossibleteachersmaxhoursdaily:
		if(!okteachersmaxhoursdaily){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
				
		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//not causing more than teachersMaxGapsPerWeek teachers gaps
		/////////////////
		if(true /*teachersMaxGapsMaxGaps>=0*/){
			int k;
			for(k=0; k<act->nTeachers; k++) if(teachersMaxGapsPercentage[k]>=0){
				int tc=act->teachers[k];
	
				int t;
				int cntfinal=0;
				int cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForTeacher[tc][t].count()>0){
						assert(!breakTime[t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){ //break is no gap, no activity
					if(activitiesForTeacher[tc][t].count()>0){
						cntfinal-=cnt+1;
						cnt=0;
					}
					else
						cnt++;
				}

				cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForTeacher[tc][t].count()>0 || (t>=newtime && t<newtime+gt.rules.nDaysPerWeek*act->duration)){
						assert(!breakTime[t]); //this slot has no break
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){
					if(activitiesForTeacher[tc][t].count()>0 || (t>=newtime && t<newtime+gt.rules.nDaysPerWeek*act->duration)){
						cntfinal+=cnt+1;
						cnt=0;
					}
					else
						cnt++;
				}

				assert(cntfinal>=0);
	
				if(cntfinal==0){
				}
				else if(nHoursScheduledForTeacher[tc]+cntfinal<=nHoursPerTeacher[tc]+teachersMaxGapsMaxGaps[tc]){
				}
				else if(skipRandom(teachersMaxGapsPercentage[tc])){
				}
				else{ //illegal


					/////////////////////////////////////////////////////////////////////////////////////////
					//added in 5.0.0-preview22
					int excessHours=nHoursScheduledForTeacher[tc]+cntfinal-nHoursPerTeacher[tc]-teachersMaxGapsMaxGaps[tc];
					assert(excessHours>0);
							
					//cout<<"excessHours=="<<excessHours<<endl;
							
					int optNWrong=INF;
					int optMinWrong=INF;
					int optNConflActs=gt.rules.nInternalActivities;
					
					//remove at the end
					int nWrong1[MAX_DAYS_PER_WEEK];
					int minWrong1[MAX_DAYS_PER_WEEK];

					QList<int> conflActs1[MAX_DAYS_PER_WEEK];
			
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						nWrong1[d2]=0;
						minWrong1[d2]=INF;

						int excess=excessHours;
								
						conflActs1[d2].clear();
								
						if(d2!=d){
							int h2;
							for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
								if(teachersTimetable[tc][d2][h2]>=0){
									assert(!breakDayHour[d2][h2]);
									break;
								}
							}
							bool okswapped=true;
							for(; h2>=0; h2--) if(!breakDayHour[d2][h2]) {
								int ttt=teachersTimetable[tc][d2][h2];
								assert(ttt!=ai);
								if(ttt>=0){
									if(swappedActivities[ttt]){
										okswapped=false;
									}
									
									if(conflActs1[d2].indexOf(ttt)==-1)
										conflActs1[d2].append(ttt);
												
										nWrong1[d2]+=triedRemovals[ttt][c.times[ttt]];

										if(minWrong1[d2]>triedRemovals[ttt][c.times[ttt]])
											minWrong1[d2]=triedRemovals[ttt][c.times[ttt]];
								}
								excess--;
								if(excess==0)
									break;
							}
							bool okhere=false;
							for(h2--; h2>=0; h2--) if(!breakDayHour[d2][h2])
								if(teachersTimetable[tc][d2][h2]>=0){
									okhere=true;
									break;
								}
							if(excess>0 || !okhere || !okswapped){
								nWrong1[d2]=-1;
								minWrong1[d2]=-1;
							}
							else{
								if(minWrong1[d2]<optMinWrong
								 || optNWrong>nWrong1[d2] && minWrong1[d2]==optMinWrong
								 || optNWrong==nWrong1[d2] && minWrong1[d2]==optMinWrong && optNConflActs>conflActs1[d2].count()){
									optNWrong=nWrong1[d2];
									optMinWrong=minWrong1[d2];
									optNConflActs=conflActs1[d2].count();
								}
							}
						}
						else{ //if d2==d
							int h2;
							for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
								if(teachersTimetable[tc][d2][h2]>=0){
									assert(!breakDayHour[d2][h2]);
									break;
								}
							}
							bool okswapped=true;
							for(; h2>=0; h2--) if(!breakDayHour[d2][h2]) {
								int ttt=teachersTimetable[tc][d2][h2];
								assert(ttt!=ai);
								if(ttt>=0){
									if(swappedActivities[ttt])
										okswapped=false;
								
									if(conflActs1[d2].indexOf(ttt)==-1)
										conflActs1[d2].append(ttt);
												
										nWrong1[d2]+=triedRemovals[ttt][c.times[ttt]];

										if(minWrong1[d2]>triedRemovals[ttt][c.times[ttt]])
											minWrong1[d2]=triedRemovals[ttt][c.times[ttt]];
								}
								excess--;
								if(excess==0)
									break;
							}
							bool okhere=false;
							for(int h3=h2-1; h3>=0; h3--) if(!breakDayHour[d2][h3])
								if(teachersTimetable[tc][d2][h3]>=0 || h3>=h && h3<h+act->duration){
									okhere=true;
									break;
								}
							if(excess>0 || h2<h+act->duration || !okhere || !okswapped){
								nWrong1[d2]=-1;
								minWrong1[d2]=-1;
							}
							else{										
								if(minWrong1[d2]<optMinWrong
								 || optNWrong>nWrong1[d2] && minWrong1[d2]==optMinWrong
								 || optNWrong==nWrong1[d2] && minWrong1[d2]==optMinWrong && optNConflActs>conflActs1[d2].count()){
									optNWrong=nWrong1[d2];
									optMinWrong=minWrong1[d2];
									optNConflActs=conflActs1[d2].count();
								}
							}
						}
					}
							
					//or remove at the beginning
					int nWrong2[MAX_DAYS_PER_WEEK];
					int minWrong2[MAX_DAYS_PER_WEEK];

					QList<int> conflActs2[MAX_DAYS_PER_WEEK];
			
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						nWrong2[d2]=0;
						minWrong2[d2]=INF;

						int excess=excessHours;
						
						conflActs2[d2].clear();
								
						if(d2!=d){
							int h2;
							for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
								if(teachersTimetable[tc][d2][h2]>=0){
									assert(!breakDayHour[d2][h2]);
									break;
								}
							}
							bool okswapped=true;
							for(; h2<gt.rules.nHoursPerDay; h2++) if(!breakDayHour[d2][h2]) {
								int ttt=teachersTimetable[tc][d2][h2];
								assert(ttt!=ai);
								if(ttt>=0){
									if(swappedActivities[ttt])
										okswapped=false;
								
									if(conflActs2[d2].indexOf(ttt)==-1)
										conflActs2[d2].append(ttt);
												
										nWrong2[d2]+=triedRemovals[ttt][c.times[ttt]];

										if(minWrong2[d2]>triedRemovals[ttt][c.times[ttt]])
											minWrong2[d2]=triedRemovals[ttt][c.times[ttt]];
								}
								excess--;
								if(excess==0)
									break;
							}
							bool okhere=false;
							for(h2++; h2<gt.rules.nHoursPerDay; h2++) if(!breakDayHour[d2][h2])
								if(teachersTimetable[tc][d2][h2]>=0){
									okhere=true;
									break;
								}
							if(excess>0 || !okhere || !okswapped){
								nWrong2[d2]=-1;
								minWrong2[d2]=-1;
							}
							else{										
								if(minWrong2[d2]<optMinWrong
								 || optNWrong>nWrong2[d2] && minWrong2[d2]==optMinWrong
								 || optNWrong==nWrong2[d2] && minWrong2[d2]==optMinWrong && optNConflActs>conflActs2[d2].count()){
									optNWrong=nWrong2[d2];
									optMinWrong=minWrong2[d2];
									optNConflActs=conflActs2[d2].count();
								}
							}
						}
						else{ //if d2==d
							int h2;
							for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
								if(teachersTimetable[tc][d2][h2]>=0){
									assert(!breakDayHour[d2][h2]);
									break;
								}
							}
							bool okswapped=true;
							for(; h2<gt.rules.nHoursPerDay; h2++) if(!breakDayHour[d2][h2]) {
								int ttt=teachersTimetable[tc][d2][h2];
								assert(ttt!=ai);
								if(ttt>=0){
									if(swappedActivities[ttt])
										okswapped=false;
									
									if(conflActs2[d2].indexOf(ttt)==-1)
										conflActs2[d2].append(ttt);
												
										nWrong2[d2]+=triedRemovals[ttt][c.times[ttt]];

										if(minWrong2[d2]>triedRemovals[ttt][c.times[ttt]])
											minWrong2[d2]=triedRemovals[ttt][c.times[ttt]];
								}
								excess--;
								if(excess==0)
									break;
							}
							bool okhere=false;
							for(int h3=h2+1; h3<gt.rules.nHoursPerDay; h3++) if(!breakDayHour[d2][h3])
								if(teachersTimetable[tc][d2][h3]>=0 || h3>=h && h3<h+act->duration){
									okhere=true;
									break;
								}
							if(excess>0 || h2>=h || !okhere || !okswapped){
								nWrong2[d2]=-1;
								minWrong2[d2]=-1;
							}
							else{
								if(minWrong2[d2]<optMinWrong
								 || optNWrong>nWrong2[d2] && minWrong2[d2]==optMinWrong
								 || optNWrong==nWrong2[d2] && minWrong2[d2]==optMinWrong && optNConflActs>conflActs2[d2].count()){
									optNWrong=nWrong2[d2];
									optMinWrong=minWrong2[d2];
									optNConflActs=conflActs2[d2].count();
								}
							}
						}
					}

					if(optNWrong==INF || optNWrong==-1){
						if(level==0){
							cout<<"WARNING: extreme case, where I have to remove all teacher's activities "
							 "because of max gaps constraints, teacher=="<<qPrintable(gt.rules.internalTeachersList[tc]->name)<<endl;
							cout<<"ai is activity with id=="<<gt.rules.internalActivitiesList[ai].id<<endl;
							cout<<"explored time: day=="<<qPrintable(gt.rules.daysOfTheWeek[d])<<", hour=="<<qPrintable(gt.rules.hoursOfTheDay[h])<<endl;
							cout<<"excessHours=="<<excessHours<<endl;
						}
						
						bool okswapped=true;
			
						//try to remove all other activities of this teacher
						for(int i=0; i<gt.rules.nHoursPerWeek; i++){
							assert(activitiesForTeacher[tc][i].count()<=1);

							if(activitiesForTeacher[tc][i].count()==0)
								continue;

							int ai2=activitiesForTeacher[tc][i].at(0);
							
							if(swappedActivities[ai])
								okswapped=false;
						
							if(conflActivities[newtime].indexOf(ai2)==-1){
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							}
						}
						if(!okswapped)
							break; //impossible slot
					}
					else{
						assert(optNWrong<INF);
						assert(optNWrong>=0);

						QList<int> optDays;
						QList<int> where; //at the end (1) or at the begin (2)
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
							if(nWrong1[d2]>=0)
								if(optNWrong==nWrong1[d2] && minWrong1[d2]==optMinWrong && optNConflActs==conflActs1[d2].count()){
									optDays.append(d2);
									where.append(1);
								}
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
							if(nWrong2[d2]>=0)
								if(optNWrong==nWrong2[d2] && minWrong2[d2]==optMinWrong && optNConflActs==conflActs2[d2].count()){
									optDays.append(d2);
									where.append(2);
								}
						assert(optDays.count()>0);
						int rnd=rand()%optDays.count();
						int d2=optDays.at(rnd);
						int w=where.at(rnd);
						assert(d2>=0);
								
						bool ok=false;
						if(w==1)
							foreach(int ai2, conflActs1[d2]){
								ok=true;
								assert(ai2!=ai);
								assert(!swappedActivities[ai2]);
								if(conflActivities[newtime].indexOf(ai2)==-1){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}
							}
						else{
							assert(w==2);
							foreach(int ai2, conflActs2[d2]){
								ok=true;
								assert(ai2!=ai);
								assert(!swappedActivities[ai2]);
								if(conflActivities[newtime].indexOf(ai2)==-1){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}
							}
						}
						assert(ok);
					}
					/////////////////////////////////////////////////////////////////////////////////////////


					/*for(int i=0; i<gt.rules.nHoursPerWeek; i++){
						assert(activitiesForTeacher[tc][i].count()<=1);

						if(activitiesForTeacher[tc][i].count()==0)
							continue;

						int ai2=activitiesForTeacher[tc][i].at(0);
						
						if(conflActivities[newtime].indexOf(ai2)==-1){
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						}
					}*/
				}
			}
			if(k<act->nTeachers){
				//assert(0);
				nConflActivities[newtime]=MAX_ACTIVITIES;
				continue;
			}
		}
		/////////////////////

		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/

/////////////////////////////////////////////////////////////////////////////////////////////

		//not causing students' gaps (final gaps)
		//////////////////
		int k;
		for(k=0; k<act->nSubgroups; k++){
			int isg=act->subgroups[k];

			if(subgroupsEarlyPercentage[isg]>=0 && subgroupsNoGapsPercentage[isg]>=0){
			//students early+no gaps
				bool count=false;
				int cnt=0;
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){ //break is no gap, no activity
					if(activitiesForSubgroup[isg][t].count()>0)
						count=true;
					if(count)
						cnt--;
				}

				count=false;
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){
					if( (t>=newtime && t<newtime+gt.rules.nDaysPerWeek*act->duration) ||
					 (activitiesForSubgroup[isg][t].count()>0) )
						count=true;
					if(count)
						cnt++;
				}
			
				assert(cnt>=0);
				
				bool ok;
				if(cnt==0){
					ok=true;
				}
				else{
					if(nHoursScheduledForSubgroup[isg]+cnt>nHoursPerSubgroup[isg]){
						ok=skipRandom(subgroupsNoGapsPercentage[isg]);
						
						if(!ok){
							/////////////////////////////////////////////////////////////////////////////////////////
							//added in 5.0.0-preview22
							int excessHours=nHoursScheduledForSubgroup[isg]+cnt-nHoursPerSubgroup[isg];
							
							//cout<<"excessHours=="<<excessHours<<endl;
							
							int nWrong[MAX_DAYS_PER_WEEK];
							int minWrong[MAX_DAYS_PER_WEEK];

							int optNWrong=INF;
							int optMinWrong=INF;
							int optNConflActs=gt.rules.nInternalActivities;
					
							QList<int> conflActs[MAX_DAYS_PER_WEEK];
			
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								nWrong[d2]=0;
								minWrong[d2]=INF;

								//nWrong[d2]=-1;
								//minWrong[d2]=-1;
								
								int excess=excessHours;
								
								conflActs[d2].clear();
								
								if(d2!=d){
									int h2;
									for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
										if(subgroupsTimetable[isg][d2][h2]>=0){
											assert(!breakDayHour[d2][h2]);
											break;
										}
									}
									bool okswapped=true;
									for(; h2>=0; h2--) if(!breakDayHour[d2][h2]) {
										int ttt=subgroupsTimetable[isg][d2][h2];
										assert(ttt!=ai);
										if(ttt>=0){
											if(swappedActivities[ttt])
												okswapped=false;
											if(conflActs[d2].indexOf(ttt)==-1)
												conflActs[d2].append(ttt);
												
												nWrong[d2]+=triedRemovals[ttt][c.times[ttt]];

												if(minWrong[d2]>triedRemovals[ttt][c.times[ttt]])
													minWrong[d2]=triedRemovals[ttt][c.times[ttt]];
										}
										excess--;
										if(excess==0)
											break;
									}
									if(excess>0 || !okswapped){
										nWrong[d2]=-1;
										minWrong[d2]=-1;
									}
									else{										
										if(minWrong[d2]<optMinWrong
										 || optNWrong>nWrong[d2] && minWrong[d2]==optMinWrong
										 || optNWrong==nWrong[d2] && minWrong[d2]==optMinWrong && optNConflActs>conflActs[d2].count()){
											optNWrong=nWrong[d2];
											optMinWrong=minWrong[d2];
											optNConflActs=conflActs[d2].count();
										}
									}
								}
								else{ //if d2==d
									int h2;
									for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
										if(subgroupsTimetable[isg][d2][h2]>=0){
											assert(!breakDayHour[d2][h2]);
											break;
										}
									}
									bool okswapped=true;
									for(; h2>=0; h2--) if(!breakDayHour[d2][h2]) {
										int ttt=subgroupsTimetable[isg][d2][h2];
										assert(ttt!=ai);
										if(ttt>=0){
											if(swappedActivities[ttt])
												okswapped=false;
											if(conflActs[d2].indexOf(ttt)==-1)
												conflActs[d2].append(ttt);
												
												nWrong[d2]+=triedRemovals[ttt][c.times[ttt]];

												if(minWrong[d2]>triedRemovals[ttt][c.times[ttt]])
													minWrong[d2]=triedRemovals[ttt][c.times[ttt]];
										}
										excess--;
										if(excess==0)
											break;
									}
									if(excess>0 || h2<h+act->duration || !okswapped){
										nWrong[d2]=-1;
										minWrong[d2]=-1;
									}
									else{										
										if(minWrong[d2]<optMinWrong
										 || optNWrong>nWrong[d2] && minWrong[d2]==optMinWrong
										 || optNWrong==nWrong[d2] && minWrong[d2]==optMinWrong && optNConflActs>conflActs[d2].count()){
											optNWrong=nWrong[d2];
											optMinWrong=minWrong[d2];
											optNConflActs=conflActs[d2].count();
										}
									}
								}
							}
		
							if(optNWrong==INF || optNWrong==-1){
								if(level==0)
									cout<<"WARNING - unlikely case, optimizetime.cpp line 2153"<<endl;
								break; //impossible slot
							}
							assert(optNWrong<INF);
							assert(optNWrong>=0);

							QList<int> optDays;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
								if(nWrong[d2]>=0)
									if(optNWrong==nWrong[d2] && minWrong[d2]==optMinWrong && optNConflActs==conflActs[d2].count())
										optDays.append(d2);
							assert(optDays.count()>0);
							int rnd=rand()%optDays.count();
							int d2=optDays.at(rnd);
							assert(d2>=0);

							bool ok=false;
							foreach(int ai2, conflActs[d2]){
								ok=true;
								assert(ai2!=ai);
								assert(!swappedActivities[ai2]);
								if(conflActivities[newtime].indexOf(ai2)==-1){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}
							}
							assert(ok);
							/////////////////////////////////////////////////////////////////////////////////////////
												
							/*if(h!=0){
								break; //impossible slot
							}
						
							for(int i=0; i<gt.rules.nHoursPerWeek; i++){
								assert(activitiesForSubgroup[isg][i].count()<=1);
								
								if(activitiesForSubgroup[isg][i].count()==0)
									continue;

								int ai2=activitiesForSubgroup[isg][i].at(0);
								
								if(conflActivities[newtime].indexOf(ai2)==-1){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}
							}*/
						}
						/////////////////////////////
					}
					else
						ok=true;
				}
			}
			else if(subgroupsEarlyPercentage[isg]==-1 && subgroupsNoGapsPercentage[isg]>=0){
			//only students no gaps
				int t;
				int cntfinal=0;
				int cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForSubgroup[isg][t].count()>0){
						assert(!breakTime[t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){ //break is no gap, no activity
					if(activitiesForSubgroup[isg][t].count()>0){
						cntfinal-=cnt+1;
						cnt=0;
					}
					else
						cnt++;
				}

				cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForSubgroup[isg][t].count()>0 || (t>=newtime && t<newtime+gt.rules.nDaysPerWeek*act->duration)){
						assert(!breakTime[t]); //current slot is not break
						break;
					}
				}
	
				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t]){
					if(activitiesForSubgroup[isg][t].count()>0 || (t>=newtime && t<newtime+gt.rules.nDaysPerWeek*act->duration)){
						cntfinal+=cnt+1;
						cnt=0;
					}
					else
						cnt++;
				}

				assert(cntfinal>=0);
				
				bool ok;
				if(cntfinal==0){
					ok=true;
				}
				else{
					//cout<<"cntfinal=="<<cntfinal<<endl;
				
					if(nHoursScheduledForSubgroup[isg]+cntfinal>nHoursPerSubgroup[isg]){
						ok=skipRandom(subgroupsNoGapsPercentage[isg]);

						if(!ok){
							/////////////////////////////////////////////////////////////////////////////////////////
							//added in 5.0.0-preview22
							int excessHours=nHoursScheduledForSubgroup[isg]+cntfinal-nHoursPerSubgroup[isg];
							assert(excessHours>0);
							
							//cout<<"excessHours=="<<excessHours<<endl;
							
							int optNWrong=INF;
							int optMinWrong=INF;
							int optNConflActs=gt.rules.nInternalActivities;
					
							//remove at the end
							int nWrong1[MAX_DAYS_PER_WEEK];
							int minWrong1[MAX_DAYS_PER_WEEK];

							QList<int> conflActs1[MAX_DAYS_PER_WEEK];
			
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								nWrong1[d2]=0;
								minWrong1[d2]=INF;

								int excess=excessHours;
								
								conflActs1[d2].clear();
								
								if(d2!=d){
									int h2;
									for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
										if(subgroupsTimetable[isg][d2][h2]>=0){
											assert(!breakDayHour[d2][h2]);
											break;
										}
									}
									bool okswapped=true;
									for(; h2>=0; h2--) if(!breakDayHour[d2][h2]) {
										int ttt=subgroupsTimetable[isg][d2][h2];
										assert(ttt!=ai);
										if(ttt>=0){
											if(swappedActivities[ttt])
												okswapped=false;
											if(conflActs1[d2].indexOf(ttt)==-1)
												conflActs1[d2].append(ttt);
												
												nWrong1[d2]+=triedRemovals[ttt][c.times[ttt]];

												if(minWrong1[d2]>triedRemovals[ttt][c.times[ttt]])
													minWrong1[d2]=triedRemovals[ttt][c.times[ttt]];
										}
										excess--;
										if(excess==0)
											break;
									}
									bool okhere=false;
									for(h2--; h2>=0; h2--) if(!breakDayHour[d2][h2])
										if(subgroupsTimetable[isg][d2][h2]>=0){
											okhere=true;
											break;
										}
									if(excess>0 || !okhere || !okswapped){
										nWrong1[d2]=-1;
										minWrong1[d2]=-1;
									}
									else{										
										if(minWrong1[d2]<optMinWrong
										 || optNWrong>nWrong1[d2] && minWrong1[d2]==optMinWrong
										 || optNWrong==nWrong1[d2] && minWrong1[d2]==optMinWrong && optNConflActs>conflActs1[d2].count()){
											optNWrong=nWrong1[d2];
											optMinWrong=minWrong1[d2];
											optNConflActs=conflActs1[d2].count();
										}
									}
								}
								else{ //if d2==d
									int h2;
									for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
										if(subgroupsTimetable[isg][d2][h2]>=0){
											assert(!breakDayHour[d2][h2]);
											break;
										}
									}
									bool okswapped=true;
									for(; h2>=0; h2--) if(!breakDayHour[d2][h2]) {
										int ttt=subgroupsTimetable[isg][d2][h2];
										assert(ttt!=ai);
										if(ttt>=0){
											if(swappedActivities[ttt])
												okswapped=false;
											if(conflActs1[d2].indexOf(ttt)==-1)
												conflActs1[d2].append(ttt);
												
												nWrong1[d2]+=triedRemovals[ttt][c.times[ttt]];

												if(minWrong1[d2]>triedRemovals[ttt][c.times[ttt]])
													minWrong1[d2]=triedRemovals[ttt][c.times[ttt]];
										}
										excess--;
										if(excess==0)
											break;
									}
									bool okhere=false;
									for(int h3=h2-1; h3>=0; h3--) if(!breakDayHour[d2][h3])
										if(subgroupsTimetable[isg][d2][h3]>=0 || h3>=h && h3<h+act->duration){
											okhere=true;
											break;
										}
									if(excess>0 || h2<h+act->duration || !okhere || !okswapped){
										nWrong1[d2]=-1;
										minWrong1[d2]=-1;
									}
									else{										
										if(minWrong1[d2]<optMinWrong
										 || optNWrong>nWrong1[d2] && minWrong1[d2]==optMinWrong
										 || optNWrong==nWrong1[d2] && minWrong1[d2]==optMinWrong && optNConflActs>conflActs1[d2].count()){
											optNWrong=nWrong1[d2];
											optMinWrong=minWrong1[d2];
											optNConflActs=conflActs1[d2].count();
										}
									}
								}
							}
							
							//or remove at the beginning
							int nWrong2[MAX_DAYS_PER_WEEK];
							int minWrong2[MAX_DAYS_PER_WEEK];

							QList<int> conflActs2[MAX_DAYS_PER_WEEK];
			
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								nWrong2[d2]=0;
								minWrong2[d2]=INF;

								int excess=excessHours;
								
								conflActs2[d2].clear();
								
								if(d2!=d){
									int h2;
									for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
										if(subgroupsTimetable[isg][d2][h2]>=0){
											assert(!breakDayHour[d2][h2]);
											break;
										}
									}
									bool okswapped=true;
									for(; h2<gt.rules.nHoursPerDay; h2++) if(!breakDayHour[d2][h2]) {
										int ttt=subgroupsTimetable[isg][d2][h2];
										assert(ttt!=ai);
										if(ttt>=0){
											if(swappedActivities[ttt])
												okswapped=false;
											if(conflActs2[d2].indexOf(ttt)==-1)
												conflActs2[d2].append(ttt);
												
												nWrong2[d2]+=triedRemovals[ttt][c.times[ttt]];

												if(minWrong2[d2]>triedRemovals[ttt][c.times[ttt]])
													minWrong2[d2]=triedRemovals[ttt][c.times[ttt]];
										}
										excess--;
										if(excess==0)
											break;
									}
									bool okhere=false;
									for(h2++; h2<gt.rules.nHoursPerDay; h2++) if(!breakDayHour[d2][h2])
										if(subgroupsTimetable[isg][d2][h2]>=0){
											okhere=true;
											break;
										}
									if(excess>0 || !okhere || !okswapped){
										nWrong2[d2]=-1;
										minWrong2[d2]=-1;
									}
									else{										
										if(minWrong2[d2]<optMinWrong
										 || optNWrong>nWrong2[d2] && minWrong2[d2]==optMinWrong
										 || optNWrong==nWrong2[d2] && minWrong2[d2]==optMinWrong && optNConflActs>conflActs2[d2].count()){
											optNWrong=nWrong2[d2];
											optMinWrong=minWrong2[d2];
											optNConflActs=conflActs2[d2].count();
										}
									}
								}
								else{ //if d2==d
									int h2;
									for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
										if(subgroupsTimetable[isg][d2][h2]>=0){
											assert(!breakDayHour[d2][h2]);
											break;
										}
									}
									bool okswapped=true;
									for(; h2<gt.rules.nHoursPerDay; h2++) if(!breakDayHour[d2][h2]) {
										int ttt=subgroupsTimetable[isg][d2][h2];
										assert(ttt!=ai);
										if(ttt>=0){
											if(swappedActivities[ttt])
												okswapped=false;
											if(conflActs2[d2].indexOf(ttt)==-1)
												conflActs2[d2].append(ttt);
												
												nWrong2[d2]+=triedRemovals[ttt][c.times[ttt]];

												if(minWrong2[d2]>triedRemovals[ttt][c.times[ttt]])
													minWrong2[d2]=triedRemovals[ttt][c.times[ttt]];
										}
										excess--;
										if(excess==0)
											break;
									}
									bool okhere=false;
									for(int h3=h2+1; h3<gt.rules.nHoursPerDay; h3++) if(!breakDayHour[d2][h3])
										if(subgroupsTimetable[isg][d2][h3]>=0 || h3>=h && h3<h+act->duration){
											okhere=true;
											break;
										}
									if(excess>0 || h2>=h || !okhere || !okswapped){
										nWrong2[d2]=-1;
										minWrong2[d2]=-1;
									}
									else{										
										if(minWrong2[d2]<optMinWrong
										 || optNWrong>nWrong2[d2] && minWrong2[d2]==optMinWrong
										 || optNWrong==nWrong2[d2] && minWrong2[d2]==optMinWrong && optNConflActs>conflActs2[d2].count()){
											optNWrong=nWrong2[d2];
											optMinWrong=minWrong2[d2];
											optNConflActs=conflActs2[d2].count();
										}
									}
								}
							}

							if(optNWrong==INF || optNWrong==-1){
								if(level==0){
									cout<<"WARNING - unlikely case, optimizetime.cpp line 2492"<<endl;
									cout<<"excessHours=="<<excessHours<<endl;
									cout<<"Students set name: "<<qPrintable(gt.rules.internalSubgroupsList[isg]->name)<<endl;
									cout<<"newtime=="<<newtime<<" (d=="<<d<<", h=="<<h<<")"<<endl;
									for(int hh=0; hh<gt.rules.nHoursPerDay; hh++){
										for(int dd=0; dd<gt.rules.nDaysPerWeek; dd++)
											cout<<setw(5)<<subgroupsTimetable[isg][dd][hh];
										cout<<endl;
									}
								}
								break; //impossible slot
							}
							assert(optNWrong<INF);
							assert(optNWrong>=0);

							QList<int> optDays;
							QList<int> where; //at the end (1) or at the begin (2)
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
								if(nWrong1[d2]>=0)
									if(optNWrong==nWrong1[d2] && minWrong1[d2]==optMinWrong && optNConflActs==conflActs1[d2].count()){
										optDays.append(d2);
										where.append(1);
									}
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
								if(nWrong2[d2]>=0)
									if(optNWrong==nWrong2[d2] && minWrong2[d2]==optMinWrong && optNConflActs==conflActs2[d2].count()){
										optDays.append(d2);
										where.append(2);
									}
							assert(optDays.count()>0);
							int rnd=rand()%optDays.count();
							int d2=optDays.at(rnd);
							int w=where.at(rnd);
							assert(d2>=0);
							
							bool ok=false;
							if(w==1)
								foreach(int ai2, conflActs1[d2]){
									ok=true;
									assert(ai2!=ai);
									assert(!swappedActivities[ai2]);
									if(conflActivities[newtime].indexOf(ai2)==-1){
										conflActivities[newtime].append(ai2);
										nConflActivities[newtime]++;
										assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									}
								}
							else{
								assert(w==2);
								foreach(int ai2, conflActs2[d2]){
									ok=true;
									assert(ai2!=ai);
									assert(!swappedActivities[ai2]);
									if(conflActivities[newtime].indexOf(ai2)==-1){
										conflActivities[newtime].append(ai2);
										nConflActivities[newtime]++;
										assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									}
								}
							}
							assert(ok);
							/////////////////////////////////////////////////////////////////////////////////////////

							/*for(int i=0; i<gt.rules.nHoursPerWeek; i++){
								assert(activitiesForSubgroup[isg][i].count()<=1);

								if(activitiesForSubgroup[isg][i].count()==0)
									continue;

								int ai2=activitiesForSubgroup[isg][i].at(0);
								
								if(conflActivities[newtime].indexOf(ai2)==-1){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}
							}*/
						}
						/////////////////////////////
					}
					else
						ok=true;
				}
			}
			else
				assert(subgroupsEarlyPercentage[isg]==-1 && subgroupsNoGapsPercentage[isg]==-1);
		}
		if(k<act->nSubgroups){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking the teacher max days per week constraints
		////////////////////////////BEGIN max days per week for teachers
		bool okteachermaxdaysperweek=true;
		foreach(int tc, teachersWithMaxDaysPerWeekForActivities[ai]){
		//for(int i=0; i<act->nTeachers; i++){
			//int t=act->teachers[i];
			int maxDays=teachersMaxDaysPerWeekMaxDays[tc];
			assert(maxDays>=0); //the list contains real information
			if(maxDays>=0){
				assert(maxDays>0);

				int ndinitial=0;
				for(int j=0; j<gt.rules.nDaysPerWeek; j++)
					if(teacherActivitiesOfTheDay[tc][j].count()>0)
						ndinitial++;
				
				int ndfinal=0;
				for(int j=0; j<gt.rules.nDaysPerWeek; j++)
					if(teacherActivitiesOfTheDay[tc][j].count()>0 || j==d)
						ndfinal++;
				
				assert(ndfinal>=ndinitial);
				if(ndfinal==ndinitial){
				}
				else if(ndfinal<=maxDays){
				}
				else if(skipRandom(teachersMaxDaysPerWeekWeightPercentages[tc])){
				}
				else{
					int nWrong[MAX_DAYS_PER_WEEK];
					int minWrong[MAX_DAYS_PER_WEEK];

					int optNWrong=INF;
					int optMinWrong=INF;
					int optNConflActs=gt.rules.nInternalActivities;
					
					QList<int> conflActs[MAX_DAYS_PER_WEEK];
			
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						nWrong[d2]=-1;
						minWrong[d2]=-1;
						bool okswapped=true;
						if(d2!=d && teacherActivitiesOfTheDay[tc][d2].count()>0){
							conflActs[d2].clear();
							nWrong[d2]=0;
							minWrong[d2]=INF;
							foreach(int a, teacherActivitiesOfTheDay[tc][d2])
								if(conflActs[d2].indexOf(a)==-1){
									if(swappedActivities[a])
										okswapped=false;
								
									conflActs[d2].append(a);
									nWrong[d2]+=triedRemovals[a][c.times[a]];
									
									if(minWrong[d2]>triedRemovals[a][c.times[a]])
										minWrong[d2]=triedRemovals[a][c.times[a]];
								}
								
							if(!okswapped){
								nWrong[d2]=-1;
								minWrong[d2]=-1;
							}
							else{
								if(minWrong[d2]<optMinWrong
								 || optNWrong>nWrong[d2] && minWrong[d2]==optMinWrong
								 || optNWrong==nWrong[d2] && minWrong[d2]==optMinWrong && optNConflActs>conflActs[d2].count()){
									optNWrong=nWrong[d2];
									optMinWrong=minWrong[d2];
									optNConflActs=conflActs[d2].count();
								}
							}
						}
					}
		
					if(optNWrong>=INF || optNWrong<0){
						okteachermaxdaysperweek=false;
						goto impossibleteachermaxdaysperweek;
					}
					
					assert(optNWrong<INF);
					assert(optNWrong>=0);

					QList<int> optDays;
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
						if(nWrong[d2]>=0)
							if(optNWrong==nWrong[d2] && minWrong[d2]==optMinWrong && optNConflActs==conflActs[d2].count())
								optDays.append(d2);
					assert(optDays.count()>0);
					int rnd=rand()%optDays.count();
					int d2=optDays.at(rnd);
					assert(d2>=0);

					bool ok=false;
					foreach(int ai2, conflActs[d2]){
						ok=true;
						assert(ai2!=ai);
						assert(!swappedActivities[ai2]);
						if(conflActivities[newtime].indexOf(ai2)==-1){
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						}
					}
					assert(ok);
				}
			}
		}
impossibleteachermaxdaysperweek:
		if(!okteachermaxdaysperweek){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		/*foreach(int ai2, conflActivities[newtime])
			assert(!swappedActivities[ai2]);*/

		////////////////////////////END max days per week
		
/////////////////////////////////////////////////////////////////////////////////////////////

		
		///////////////////////////////
		//5.0.0-preview28
		//no conflicting activities for this timeslot - place the activity and return
		if(nConflActivities[newtime]==0){
			assert(c.times[ai]==UNALLOCATED_TIME);
			
			assert(conflActivities[newtime].count()==0);
		
			restoreActIndex[nRestore]=ai;
			restoreTime[nRestore]=c.times[ai];
			nRestore++;
			
			//5.0.0-preview25
			assert(swappedActivities[ai]);			
			//cout<<"level=="<<level<<", activity with id=="<<gt.rules.internalActivitiesList[ai].id<<
			// " goes free from time: "<<c.times[ai]<<" to time: "<<newtime<<", swapped act is true"<<endl;
			
			moveActivity(ai, c.times[ai], newtime);
			
			foundGoodSwap=true;
			return;
		}
		///////////////////////////////
		


		nConflActivities[newtime]=conflActivities[newtime].count();
	}
	

	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		conflPerm[perm[i]]=perm[i];
		
	//sorting - stable (not needed?) - O(n^2) - should be improved?
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		for(int j=i+1; j<gt.rules.nHoursPerWeek; j++)
			if(nConflActivities[conflPerm[perm[i]]]>nConflActivities[conflPerm[perm[j]]]){
				int t=conflPerm[perm[i]];
				conflPerm[perm[i]]=conflPerm[perm[j]];
				conflPerm[perm[j]]=t;
			}
			
	/*cout<<"perm[i]: ";
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		cout<<perm[i]<<" ";
	cout<<endl;
	cout<<"conflPerm[perm[i]]: ";
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		cout<<conflPerm[perm[i]]<<" ";
	cout<<endl;
	cout<<"nConflActivities[i]: ";
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		cout<<nConflActivities[i]<<" ";
	cout<<endl;
	assert(0);*/
			
	//for(int i=1; i<gt.rules.nHoursPerWeek; i++)
	//	assert(nConflActivities[conflPerm[perm[i-1]]]<=nConflActivities[conflPerm[perm[i]]]);

	/*for(int i=0; i<gt.rules.nHoursPerWeek; i++){
		int newtime=conflPerm[perm[i]];
		if(nConflActivities[newtime]!=MAX_ACTIVITIES)
			foreach(int ai2, conflActivities[newtime])
				assert(!swappedActivities[ai2]);
	}*/
	
	if(level==0){
		int minIndexAct[MAX_HOURS_PER_WEEK];
		int nWrong[MAX_HOURS_PER_WEEK];
		int minWrong[MAX_HOURS_PER_WEEK];
		for(int i=0; i<gt.rules.nHoursPerWeek; i++){
			minIndexAct[i]=gt.rules.nInternalActivities;
			nWrong[i]=INF;
			minWrong[i]=INF;
		}
		
		QList<int> tim;
		for(int i=0; i<gt.rules.nHoursPerWeek; i++)
			if(nConflActivities[conflPerm[perm[i]]]>0 && nConflActivities[conflPerm[perm[i]]]<MAX_ACTIVITIES)
				tim.append(conflPerm[perm[i]]);
		if(tim.count()==0 && nConflActivities[conflPerm[perm[0]]]==MAX_ACTIVITIES){
			cout<<"optimizetime.cpp line 2712 - WARNING - no possible timeslots for activity with id=="<<
			 gt.rules.internalActivitiesList[ai].id<<endl;
			 
			impossibleActivity=true;
		}
		if(tim.count()>0){
			foreach(int i, tim){
				int cnt=0;
				int m=gt.rules.nInternalActivities;
				foreach(int aii, conflActivities[i]){
					if(triedRemovals[aii][c.times[aii]]>0)
						cnt+=triedRemovals[aii][c.times[aii]];
						
					if(minWrong[i]>triedRemovals[aii][c.times[aii]])
						minWrong[i]=triedRemovals[aii][c.times[aii]];
						
					int j=invPermutation[aii];
					assert(j<gt.rules.nInternalActivities);
					if(m>j)
						m=j;
				}
				minIndexAct[i]=m;
				nWrong[i]=cnt;
			}
			
			int optNWrong=INF;
			int optMinWrong=INF;
			int optNConflActs=gt.rules.nInternalActivities;
			int optIndex=-1;
			int j=-1;
			foreach(int i, tim)
				//choose a random time out of these with minimum number of wrongly replaced activities
				if(minWrong[i]<optMinWrong || minWrong[i]==optMinWrong && optIndex<minIndexAct[i]
				 || optNWrong>nWrong[i] && minWrong[i]==optMinWrong && optIndex==minIndexAct[i]
				 || optIndex==minIndexAct[i] && optNWrong==nWrong[i] && minWrong[i]==optMinWrong && optNConflActs>nConflActivities[i]){
					optNWrong=nWrong[i];
					optMinWrong=minWrong[i];
					optIndex=minIndexAct[i];
					optNConflActs=nConflActivities[i];
					j=i;
				}
			assert(j>=0);
			QList<int> tim2;
			foreach(int i, tim)
				if(optNWrong==nWrong[i] && optIndex==minIndexAct[i] && minWrong[i]==optMinWrong && optNConflActs==nConflActivities[i])
					tim2.append(i);
			assert(tim2.count()>0);
			int rnd=rand()%tim2.count();
			j=tim2.at(rnd);

			assert(j>=0);
			timeSlot=j;
			conflActivitiesTimeSlot=conflActivities[timeSlot];
		}
	}
	
	for(int i=0; i<gt.rules.nHoursPerWeek; i++){
		int newtime=conflPerm[perm[i]]; //the considered time
		if(nConflActivities[newtime]>=MAX_ACTIVITIES)
			break;
		
		assert(c.times[ai]==UNALLOCATED_TIME);
			
		//no conflicting activities for this timeslot - place the activity and return
		if(nConflActivities[newtime]==0){
			assert(0); //done earlier
		
			assert(conflActivities[newtime].count()==0);
		
			restoreActIndex[nRestore]=ai;
			restoreTime[nRestore]=c.times[ai];
			nRestore++;
			
			//5.0.0-preview25
			assert(swappedActivities[ai]);			
			//cout<<"level=="<<level<<", activity with id=="<<gt.rules.internalActivitiesList[ai].id<<
			// " goes free from time: "<<c.times[ai]<<" to time: "<<newtime<<", swapped act is true"<<endl;
			
			moveActivity(ai, c.times[ai], newtime);
			
			foundGoodSwap=true;
			return;
		}
		else{
			/*foreach(int ai2, conflActivities[newtime])
				assert(!swappedActivities[ai2]);*/
				
			if(level==level_limit-1){
				//cout<<"level_limit-1==level=="<<level<<", for activity with id "<<gt.rules.internalActivitiesList[ai].id<<" returning"<<endl;
				foundGoodSwap=false;
				break;
			}
		
			//sort activities in decreasing order of difficulty.
			//if the index of the activity in "permutation" is smaller, the act. is more difficult
			QList<int> sorted;
			QList<int> conflActs=conflActivities[newtime];
			while(conflActs.count()>0){
				int m=gt.rules.nInternalActivities;
				int j=-1;
				for(int k=0; k<conflActs.count(); k++){
					int a=conflActs.at(k);
					if(invPermutation[a]<m){
						m=invPermutation[a];
						j=k;
					}
				}
				assert(j>=0);
				sorted.append(conflActs.at(j));
				int a=conflActs.at(j);
				int t=conflActs.removeAll(a);
				assert(t==1);
			}
			assert(sorted.count()==conflActivities[newtime].count());
			conflActivities[newtime]=sorted;
		
			int ok=true;
			//cout<<"LEVEL=="<<level<<", for activity ai with id=="<<gt.rules.internalActivitiesList[ai].id<<", list of conflActivities ids: ";
			foreach(int a, conflActivities[newtime]){
				//cout<<gt.rules.internalActivitiesList[a].id<<" ";
				if(swappedActivities[a]){
					assert(0);
					//cout<<"here ";
					ok=false;
					break;
				}
			}
			//cout<<endl;
			
			if(!ok){
				assert(0);
				continue;
			}
				
			//////////////place it at a new time
			
			int oldNRestore=nRestore;
			
			////////////////
			QList<int> oldacts;
			QList<int> oldtimes;
			
			if(1 /*ok*/){
				assert(conflActivities[newtime].size()>0);
				
				foreach(int a, conflActivities[newtime]){
					//cout<<"Level=="<<level<<", conflicting act. id=="<<gt.rules.internalActivitiesList[a].id<<", old time=="<<c.times[a]<<endl;
					
					restoreActIndex[nRestore]=a;
					restoreTime[nRestore]=c.times[a];
					nRestore++;
					
					oldacts.append(a);
					oldtimes.append(c.times[a]);
					assert(c.times[a]!=UNALLOCATED_TIME);
					//cout<<"level=="<<level<<", unallocating activity with id=="<<gt.rules.internalActivitiesList[a].id<<endl;
					moveActivity(a, c.times[a], UNALLOCATED_TIME);
					
					//swappedActivities[a]=true;
				}
			}
			assert(oldacts.count()==conflActivities[newtime].count());
			assert(oldtimes.count()==conflActivities[newtime].count());
			////////////////
			
			int oldtime=c.times[ai];
			//if(c.times[ai]!=UNALLOCATED_TIME){
				restoreActIndex[nRestore]=ai;
				restoreTime[nRestore]=oldtime;
				nRestore++;
			//}
			
			//cout<<"Level=="<<level<<", act. id=="<<gt.rules.internalActivitiesList[ai].id<<", old time=="<<c.times[ai]<<endl;

			moveActivity(ai, oldtime, newtime);
			//cout<<"level=="<<level<<", activity with id=="<<gt.rules.internalActivitiesList[ai].id<<
			// " goes from time: "<<oldtime<<" to time: "<<newtime<<endl;
			//////////////////
			
			if(1)
				foreach(int a, conflActivities[newtime])
					swappedActivities[a]=true;

			foundGoodSwap=false;
			
			ok=false;
			if(1){
				assert(conflActivities[newtime].size()>0);
				ok=true;
				
				foreach(int a, conflActivities[newtime]){
					randomswap(a, level+1);
					if(!foundGoodSwap){
						ok=false;
						break;
					}
					assert(c.times[a]!=UNALLOCATED_TIME);
					assert(foundGoodSwap);
					foundGoodSwap=false;
				}
			}
			
			if(ok){
				foreach(int a, conflActivities[newtime])
					assert(c.times[a]!=UNALLOCATED_TIME);
				assert(c.times[ai]!=UNALLOCATED_TIME);
			
				foundGoodSwap=true;
				return;
			}

			/*if(1)
				foreach(int a, conflActivities[newtime])
					swappedActivities[a]=false;*/
			
			//////////////restore times from the restore list
			for(int j=nRestore-1; j>=oldNRestore; j--){
				//assert(c.times[ai]!=UNALLOCATED_TIME);
				
				int aii=restoreActIndex[j];
				oldtime=restoreTime[j];
				
				/*if(aii!=ai)
					cout<<"Level=="<<level<<", activity with id=="<<gt.rules.internalActivitiesList[aii].id<<" should change swapped state from true to false"<<endl;
				else
					cout<<"Level=="<<level<<", activity with id=="<<gt.rules.internalActivitiesList[aii].id<<" should remain swapped==true"<<endl;
				*/
				
				if(aii!=ai){
					//assert(swappedActivities[aii]);
					swappedActivities[aii]=false;
				}
				else{
					assert(swappedActivities[aii]);
					//swappedActivities[aii]=false;
				}
				
				//assert(oldtime!=UNALLOCATED_TIME);
				
				//cout<<"level=="<<level<<", activity with id=="<<gt.rules.internalActivitiesList[aii].id<<
				// " restored from time: "<<c.times[aii]<<" to time: "<<oldtime<<endl;
				moveActivity(aii, c.times[aii], oldtime);

				//cout<<"Level=="<<level<<", act. id=="<<gt.rules.internalActivitiesList[ai].id<<", restoring old time=="<<c.times[ai]<<endl;
				
				//assert(oldtime!=UNALLOCATED_TIME);
			}
			nRestore=oldNRestore;

			//////////////////////////////
			foreach(int a, conflActivities[newtime]){
				assert(c.times[a]!=UNALLOCATED_TIME);
				assert(!swappedActivities[a]);
			}
			//////////////////////////////
			
			assert(!foundGoodSwap);
		}
	}
}
