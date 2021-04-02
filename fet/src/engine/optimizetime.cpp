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

static int restoreActIndex[MAX_ACTIVITIES]; //the index of the act. to restore
static int restoreTime[MAX_ACTIVITIES]; //the time when to restore
static int nRestore;

static int pred[MAX_ACTIVITIES];

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

//for students (set) n hours daily
QList<int> studentsActivitiesForDay[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];


//QList<qint16> impossibleTimes[MAX_ACTIVITIES];
QSet<int> impossibleTimes[MAX_ACTIVITIES];


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
	abortOptimization=false;

	c.makeTimesUnallocated(gt.rules);

	/*for(int i=0; i<gt.rules.nInternalActivities; i++)
		for(int j=0; j<gt.rules.nInternalActivities; j++)
			if(gt.rules.activitiesSimilar[i][j])
				assert(gt.rules.activityContained[i][j] && gt.rules.activityContained[j][i]);*/

	for(int i=0; i<gt.rules.nInternalActivities; i++)
		pred[i]=-1;

	tzset();
	time_t start_time;
	time(&start_time);
	
	time_limit=0.25;
	
	level_limit=10;
	
	//ATENTION TO REPAIRING IN HARD FITNESS ROUTINE
	
	//int act_max=0, act_max_pos=-1;
	
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
			swappedActivities[permutation[i]]=false;

		cout<<endl<<"Trying to place activity number added_act=="<<added_act<<
		 "\nwith id=="<<gt.rules.internalActivitiesList[permutation[added_act]].id<<
		 ", from nInternalActivities=="<<gt.rules.nInternalActivities<<endl;
	 
		assert(c.times[permutation[added_act]]==UNALLOCATED_TIME);

		for(int i=0; i<gt.rules.nHoursPerWeek; i++)
			tlistSet[i].clear();
		for(int i=0; i<added_act; i++)
			for(int j=0; j<gt.rules.internalActivitiesList[permutation[i]].duration; j++)
				tlistSet[c.times[permutation[i]]+j*gt.rules.nDaysPerWeek].insert(permutation[i]);
				
		//care for students (set) n hours daily
		/*foreach(int sg, studentsSubgroupsWithNHoursDaily){
			assert(studentsNHoursDailyMaxHours[sg]>=0);
			for(int dd=0; dd<gt.rules.nDaysPerWeek; dd++)
				studentsActivitiesForDay[sg][dd].clear();
		}
		for(int i=0; i<added_act; i++){
			Activity* act=&gt.rules.internalActivitiesList[permutation[i]];
			for(int j=0; j<act->nSubgroups; j++){
				int sg=act->subgroups[j];
				if(studentsNHoursDailyMaxHours[sg]>=0){
					int dd=c.times[permutation[i]]%gt.rules.nDaysPerWeek;
					studentsActivitiesForDay[sg][dd].append(permutation[i]);
				}
			}
		}*/
		
		///////////////care for teachers information (to help with gaps)
		if(teachersMaxGapsMaxGaps>=0){
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
						if(activitiesForTeacher[i][t].count()>0)
							break;
						
					for( ; t>=0; t-=gt.rules.nDaysPerWeek){
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
					for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+day; t>=0; t-=gt.rules.nDaysPerWeek){
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
						if(activitiesForSubgroup[i][t].count()>0)
							break;
					}

					for( ; t>=0; t-=gt.rules.nDaysPerWeek){
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
			
		assert(swappedActivities[permutation[added_act]]);
		swappedActivities[permutation[added_act]]=false;
				
		/*if(!foundGoodSwap){
			if(added_act>act_max)
				act_max_pos=-1;
		
			//if current activity does not conflict with act_max_pos, update act_max_pos and act_max
			if(act_max_pos==-1 || (!activitiesConflicting[permutation[act_max_pos]][permutation[added_act]] &&
			 minNDays[permutation[act_max_pos]][permutation[added_act]]==0)){
			 	cout<<"here"<<endl;
			 	
				act_max=added_act;
				act_max_pos=added_act;
			}
			
			int j;
			for(j=act_max_pos-1; j>=0; j--)
				if(activitiesConflicting[permutation[act_max_pos]][permutation[j]] ||
				 minNDays[permutation[act_max_pos]][permutation[j]]>0)
					break;
					
			assert(j>=0);
					
			int tmp=permutation[act_max_pos];
			for(int k=act_max_pos-1; k>=j; k--)
				permutation[k+1]=permutation[k];
			permutation[j]=tmp;
		
			act_max_pos=j;

			for(int k=act_max_pos; k<=added_act; k++)
				c.times[permutation[k]]=UNALLOCATED_TIME;
			c._fitness=-1;
			c.timeChangedForMatrixCalculation=true;			
				
			added_act=act_max_pos;
			mutex.unlock();
			goto prevvalue;
		}*/

		if(!foundGoodSwap){
			//update difficult activities (activities which are placed correctly so far, together with added_act
			nDifficultActivities=added_act+1;
			cout<<"nDifficultActivities=="<<nDifficultActivities<<endl;
			for(int j=0; j<=added_act; j++)
				difficultActivities[j]=permutation[j];
		
			//compute pred list and find the point to backtrack
			//jump one time to pred
			int pred_pos=-1;
			if(pred[permutation[added_act]]>=0){
				for(int j=added_act-1; j>=0; j--)
					if(pred[permutation[added_act]]==permutation[j]){
						pred_pos=j;
						break;
					}
			}
				
			if(pred_pos==-1){
				//cout<<"here here here here here here here here here here here here here here here here "<<endl;
			
				//////////added in version 5.0.0-preview5
				for(int k=0; k<gt.rules.nInternalActivities; k++){
					pred[permutation[k]]=-1;
					impossibleTimes[permutation[k]].clear();
				}
				/////////////////////////////////////////
			
				//compute pred
				int j=added_act;
				for(;;){
					int k=j-1;
					while(k>=0){
						//OLDER VARIANT:
						//if(!activitiesConflicting[permutation[j]][permutation[k]]
						// && minNDays[permutation[j]][permutation[k]]==0)
						
						bool conflmind=false;
						for(int ii=0; ii<minNDaysListOfActivities[permutation[added_act]].count(); ii++)
							if(minNDaysListOfActivities[permutation[added_act]].at(ii)==permutation[k])
								if(minNDaysListOfWeightPercentages[permutation[added_act]].at(ii)>=PERCENTAGE_THRESHOLD)
									conflmind=true;

						if(activitiesConflictingPercentage[permutation[added_act]][permutation[k]]<PERCENTAGE_THRESHOLD
						 && !conflmind)
							k--;
						else
							break;
					}
					pred[permutation[j]]=-1;
					if(k<0)
						break;
					pred[permutation[j]]=permutation[k];

					//////////////////
					if(j==added_act){
						//assert(impossibleTimes[permutation[k]].indexOf(c.times[permutation[k]])==-1);
						//impossibleTimes[permutation[k]].append(c.times[permutation[k]]);
						assert(!impossibleTimes[permutation[k]].contains(c.times[permutation[k]]));
						impossibleTimes[permutation[k]].insert(c.times[permutation[k]]);
					}
					//////////////////

					j=k;
				}
			}
			else{
				for(int k=pred_pos; k<added_act; k++){
					assert(c.times[permutation[k]]!=UNALLOCATED_TIME);
					if(pred[permutation[k]]>=0){
						//assert(impossibleTimes[permutation[k]].indexOf(c.times[permutation[k]])==-1);
						//impossibleTimes[permutation[k]].append(c.times[permutation[k]]);
						assert(!impossibleTimes[permutation[k]].contains(c.times[permutation[k]]));
						impossibleTimes[permutation[k]].insert(c.times[permutation[k]]);
					}
				}
			}

			//backtrack
			int j;
			for(j=added_act-1; j>=0; j--)
				if(pred[permutation[added_act]]==permutation[j])
					break;
					
			/*for(int k=j; k<=added_act; k++){
				cout<<"Act id=="<<gt.rules.internalActivitiesList[permutation[k]].id<<" ";
				cout<<"predId="<<gt.rules.internalActivitiesList[pred[permutation[k]]].id<<" ";
				cout<<"impossible times: ";
				foreach(qint16 it, impossibleTimes[permutation[k]])
					cout<<it<<" ";
				cout<<endl;
			}*/
					
			if(j<0){
				//????? invalid timetable data?
				assert(0);
		
				//no predecessor to this activity - try again with another random value
				mutex.unlock();
				//cout<<"mutex unlocked - optimizetime 358"<<endl;
				goto prevvalue;
			}
					
			assert(j>=0);
		
			int t=permutation[added_act];
			for(int i=added_act-1; i>=j; i--)
				permutation[i+1]=permutation[i];
			permutation[j]=t;
		
			for(int i=j; i<=added_act; i++)
				c.times[permutation[i]]=UNALLOCATED_TIME;
			c._fitness=-1;
			c.timeChangedForMatrixCalculation=true;			
				
			added_act=j;
			mutex.unlock();
			//cout<<"mutex unlocked - optimizetime 376"<<endl;
			goto prevvalue;
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
		}

		mutex.unlock();	
		//cout<<"mutex unlocked - optimizetime 403"<<endl;
	}

	mutex.lock();
	
	//c.write(gt.rules, "chromo.dat");
	
	/////////////////a check for min n days
	/*for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		assert(c.times[ai]!=UNALLOCATED_TIME);
		int d=c.times[ai]%gt.rules.nDaysPerWeek;
		int h=c.times[ai]/gt.rules.nDaysPerWeek;
		for(int i=0; i<minNDaysListOfActivities[ai].count(); i++){
			int ai2=minNDaysListOfActivities[ai].at(i);
			int md=minNDaysListOfMinDays[ai].at(i);
			assert(c.times[ai2]!=UNALLOCATED_TIME);
			int ai2time=c.times[ai2];

			int d2=ai2time%gt.rules.nDaysPerWeek;
			int h2=ai2time/gt.rules.nDaysPerWeek;
			//broken min n days - have to be adjacent?
			
			if(minNDaysListOfWeightPercentages[ai].at(i)==100)
				assert(md<=abs(d-d2));
			
			if(md>abs(d-d2) && minNDaysListOfAdjIfBroken[ai].at(i)==true)
				assert(d==d2 && (h+gt.rules.internalActivitiesList[ai].duration==h2 || h2+gt.rules.internalActivitiesList[ai2].duration==h));
		}
	}*/
	/////////////////
	
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
		//int h=fromslot/gt.rules.nDaysPerWeek;
		
		for(int t=fromslot; t<fromslot+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
			int tt=tlistSet[t].remove(ai);
			assert(tt==1);
		}

		//update for students (set) n hours daily
		/*
		for(int i=0; i<act->nSubgroups; i++){
			int isg=act->subgroups[i];
			if(studentsNHoursDailyMaxHours[isg]>=0){
				int tt=studentsActivitiesForDay[isg][d].removeAll(ai);
				assert(tt==1);
			}
		}*/

		//update teachers' list of activities for each day
		/////////////////
		foreach(int tch, teachersWithMaxDaysPerWeekForActivities[ai]){
			int tt=teacherActivitiesOfTheDay[tch][d].removeAll(ai);
			assert(tt==1);
		}
		/////////////////
		
		//update teachers' count of occupied activities
		/////////////////
		if(teachersMaxGapsMaxGaps>=0){
			for(int i=0; i<act->nTeachers; i++){
				int tc=act->teachers[i];
	
				int t;
				int cntfinal=0;
				int cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForTeacher[tc][t].count()>0)
						break;
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek){
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
					if(activitiesForTeacher[tc][t].count()>0)
						break;
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek){
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
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
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
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
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
					if(activitiesForSubgroup[isg][t].count()>0)
						break;
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek){
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
					if(activitiesForSubgroup[isg][t].count()>0)
						break;
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek){
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
		//int h=fromslot/gt.rules.nDaysPerWeek;
		
		for(int t=toslot; t<toslot+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
			assert(!tlistSet[t].contains(ai));
			tlistSet[t].insert(ai);
		}

		//update for students (set) n hours daily
		/*
		for(int i=0; i<act->nSubgroups; i++){
			int isg=act->subgroups[i];
			if(studentsNHoursDailyMaxHours[isg]>=0){
				assert(studentsActivitiesForDay[isg][d].indexOf(ai)==-1);
				studentsActivitiesForDay[isg][d].append(ai);
			}
		}*/

		//update teachers' list of activities for each day
		/////////////////
		foreach(int tch, teachersWithMaxDaysPerWeekForActivities[ai]){
			assert(teacherActivitiesOfTheDay[tch][d].indexOf(ai)==-1);
			teacherActivitiesOfTheDay[tch][d].append(ai);
		}
		/////////////////
		
		//update teachers' count of occupied activities
		/////////////////
		if(teachersMaxGapsMaxGaps>=0){
			for(int i=0; i<act->nTeachers; i++){
				int tc=act->teachers[i];
	
				int t;
				int cntfinal=0;
				int cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForTeacher[tc][t].count()>0)
						break;
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek){
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
					if(activitiesForTeacher[tc][t].count()>0)
						break;
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForTeacher[tc][t].count()>0){
						cntfinal+=cnt+1;
						cnt=0;
					}
					else
						cnt++;
				}
	
				assert(cntfinal>=0);
				
				nHoursScheduledForTeacher[tc]+=cntfinal;
			
				if(teachersMaxGapsPercentage==100)
					assert(nHoursScheduledForTeacher[tc] <= nHoursPerTeacher[tc] + teachersMaxGapsMaxGaps);
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
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
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
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
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
					if(activitiesForSubgroup[isg][t].count()>0)
						break;
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek){
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
					if(activitiesForSubgroup[isg][t].count()>0)
						break;
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek){
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
	if(level>=level_limit)
		return;
	
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

		///////////////////
		if(impossibleTimes[ai].contains(newtime)){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		///////////////////

		//not too late
		int d=newtime%gt.rules.nDaysPerWeek;
		int h=newtime/gt.rules.nDaysPerWeek;

		//not too late
		if(h+act->duration>gt.rules.nHoursPerDay){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		//allowed (tch&st not available, break, act(s) preferred time(s))
		if(!skipRandom(allowedTimesPercentages[ai][newtime])){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		//allowed from same starting time
		bool oksamestartingtime=true;
		for(int i=0; i<activitiesSameStartingTimeActivities[ai].count(); i++){
			int ai2=activitiesSameStartingTimeActivities[ai].at(i);
			int perc=activitiesSameStartingTimePercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				if(newtime!=c.times[ai2] && !skipRandom(perc)){
					oksamestartingtime=false;
					break;
				}
			}
		}
		if(!oksamestartingtime){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		//allowed from same starting hour
		bool oksamestartinghour=true;
		for(int i=0; i<activitiesSameStartingHourActivities[ai].count(); i++){
			int ai2=activitiesSameStartingHourActivities[ai].at(i);
			int perc=activitiesSameStartingHourPercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				if((newtime/gt.rules.nDaysPerWeek)!=(c.times[ai2]/gt.rules.nDaysPerWeek) && !skipRandom(perc)){
					oksamestartinghour=false;
					break;
				}
			}
		}
		if(!oksamestartinghour){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		//allowed from not overlapping
		bool oknotoverlapping=true;
		for(int i=0; i<activitiesNotOverlappingActivities[ai].count(); i++){
		//foreach(int ai2, activitiesNotOverlapping[ai])
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
						oknotoverlapping=false;
						break;
					}
				}
			}
		}
		if(!oknotoverlapping){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		//allowed for students (set) n hours daily
		int k;
		/*for(k=0; k<act->nSubgroups; k++){
			int isg=act->subgroups[k];
			if(studentsNHoursDailyMaxHours[isg]>=0){
				int nh=0;
				foreach(int ai2, studentsActivitiesForDay[isg][d])
					nh+=gt.rules.internalActivitiesList[ai2].duration;
				
				if(nh+act->duration > studentsNHoursDailyMaxHours[isg] && !skipRandom(studentsNHoursDailyPercentage[isg])){
					//cout<<"nh=="<<nh<<", act->duration=="<<act->duration<<", studentsNHoursDailyMaxHours[isg]=="<<studentsNHoursDailyMaxHours[isg]<<endl;
					break;
				}
			}
		}
		if(k<act->nSubgroups){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}*/

		//not causing more than teachersMaxGapsPerWeek teachers gaps
		/////////////////
		//int d=fromslot%gt.rules.nDaysPerWeek;
		//int h=fromslot/gt.rules.nDaysPerWeek;
		if(teachersMaxGapsMaxGaps>=0){
			int k;
			for(k=0; k<act->nTeachers; k++){
				int tc=act->teachers[k];
	
				int t;
				int cntfinal=0;
				int cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForTeacher[tc][t].count()>0)
						break;
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForTeacher[tc][t].count()>0){
						cntfinal-=cnt+1;
						cnt=0;
					}
					else
						cnt++;
				}

				cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForTeacher[tc][t].count()>0 || (t>=newtime && t<newtime+gt.rules.nDaysPerWeek*act->duration))
						break;
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek){
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
				else if(nHoursScheduledForTeacher[tc]+cntfinal<=nHoursPerTeacher[tc]+teachersMaxGapsMaxGaps){
				//if(nHoursScheduledForTeacher[tc]+cntfinal<=nHoursPerTeacher[tc]+teachersMaxGapsMaxGaps){
				}
				else if(skipRandom(teachersMaxGapsPercentage)){
				}
				else //illegal
					break;
				//if(nHoursScheduledForTeacher[tc]+cntfinal>nHoursPerTeacher[tc]+MAX_GAPS_PER_TEACHER)
				//	break;
			}
			if(k<act->nTeachers){
				nConflActivities[newtime]=MAX_ACTIVITIES;
				continue;
			}
		}
		/////////////////////
		
		//not causing students gaps (final gaps)
		//////////////////
		for(k=0; k<act->nSubgroups; k++){
			int isg=act->subgroups[k];

			if(subgroupsEarlyPercentage[isg]>=0 && subgroupsNoGapsPercentage[isg]>=0){
			//students early+no gaps
				bool count=false;
				int cnt=0;
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForSubgroup[isg][t].count()>0)
						count=true;
					if(count)
						cnt--;
				}

				count=false;
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
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
					if(nHoursScheduledForSubgroup[isg]+cnt>nHoursPerSubgroup[isg])
						ok=skipRandom(subgroupsNoGapsPercentage[isg]);
					else
						ok=true;
				}
				
				if(!ok)
					break;
			}
			else if(subgroupsEarlyPercentage[isg]==-1 && subgroupsNoGapsPercentage[isg]>=0){
			//only students no gaps
				int t;
				int cntfinal=0;
				int cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForSubgroup[isg][t].count()>0)
						break;
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForSubgroup[isg][t].count()>0){
						cntfinal-=cnt+1;
						cnt=0;
					}
					else
						cnt++;
				}

				cnt=0;
				for(t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek){
					if(activitiesForSubgroup[isg][t].count()>0 || (t>=newtime && t<newtime+gt.rules.nDaysPerWeek*act->duration))
						break;
				}
	
				for( ; t>=0; t-=gt.rules.nDaysPerWeek){
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
					if(nHoursScheduledForSubgroup[isg]+cntfinal>nHoursPerSubgroup[isg])
						ok=skipRandom(subgroupsNoGapsPercentage[isg]);
					else
						ok=true;
				}
				
				if(!ok)
					break;
			}
			else
				assert(subgroupsEarlyPercentage[isg]==-1 && subgroupsNoGapsPercentage[isg]==-1);
		}
		if(k<act->nSubgroups){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		//not breaking the teacher max days per week constraints
		////////////////////////////BEGIN max days per week for teachers
		bool ok=true;
		foreach(int t, teachersWithMaxDaysPerWeekForActivities[ai]){
		//for(int i=0; i<act->nTeachers; i++){
			//int t=act->teachers[i];
			int maxDays=teachersMaxDaysPerWeekMaxDays[t];
			assert(maxDays>=0); //the list contains real information
			if(maxDays>=0){
				assert(maxDays>0);
				
				int ndinitial=0;
				for(int j=0; j<gt.rules.nDaysPerWeek; j++)
					if(teacherActivitiesOfTheDay[t][j].count()>0)
						ndinitial++;
				
				int ndfinal=0;
				for(int j=0; j<gt.rules.nDaysPerWeek; j++)
					if(teacherActivitiesOfTheDay[t][j].count()>0 || j==d)
						ndfinal++;
				
				assert(ndfinal>=ndinitial);
				if(ndfinal==ndinitial){
				}
				else if(ndfinal<=maxDays){
				}
				else if(skipRandom(teachersMaxDaysPerWeekWeightPercentages[t])){
				}
				else
					ok=false;
				
				/*bool dayOccupied[MAX_DAYS_PER_WEEK];
				for(int j=0; j<gt.rules.nDaysPerWeek; j++)
					dayOccupied[j]=false;
				
				foreach(int ai2, activitiesForTeachers[t])
					if(ai2!=ai && c.times[ai2]!=UNALLOCATED_TIME)
						dayOccupied[c.times[ai2]%gt.rules.nDaysPerWeek]=true;
					
				int nd=0;	
				for(int j=0; j<gt.rules.nDaysPerWeek; j++)
					if(dayOccupied[j])
						nd++;
				
				assert(nd<=maxDays);
				
				if(!(nd==maxDays && dayOccupied[d] || nd<maxDays))
					ok=false;*/
					
				if(!ok)
					break;
			}
		}
		if(!ok){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		////////////////////////////END max days per week
		
		//care about basic time constraints
		for(int t1=newtime; t1<newtime+act->duration*gt.rules.nDaysPerWeek; t1+=gt.rules.nDaysPerWeek)
			foreach(int ai2, tlistSet[t1]){
				//if(ai!=ai2 && activitiesConflicting[ai][ai2]){
				if(ai!=ai2 && !skipRandom(activitiesConflictingPercentage[ai][ai2])){
					conflActivities[newtime].append(ai2);
				}
			}
				
		//care about min n days
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
					//int rn=rand()%100;
					//bool okrand = rn < minNDaysBrokenAllowancePercentage;
					
					if(minNDaysListOfAdjIfBroken[ai].at(i)==true){ //must place them adjacent if break min n days
						if(okrand && (d==d2 && (h+act->duration==h2 || h2+gt.rules.internalActivitiesList[ai2].duration==h))){
						}
						else{
							if(conflActivities[newtime].indexOf(ai2)==-1)
								conflActivities[newtime].append(ai2);
						}
					}					
					else{ //can place them anywhere
						if(okrand){
						}
						else{
							if(conflActivities[newtime].indexOf(ai2)==-1)
								conflActivities[newtime].append(ai2);
						}
					}					
				}
			}
		}
					
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
	
	for(int i=0; i<gt.rules.nHoursPerWeek; i++){
		int newtime=conflPerm[perm[i]]; //the considered time
		if(nConflActivities[newtime]>=MAX_ACTIVITIES)
			break;
		
		//no conflicting activities for this timeslot - place the activity and return
		if(nConflActivities[newtime]==0){
			/*
			for(int j=0; j<gt.rules.nInternalActivities; j++)
				if(c.times[j]!=UNALLOCATED_TIME && j!=ai)
					if(gt.rules.activitiesConflicting[ai][j])
						for(int d=0; d<gt.rules.internalActivitiesList[j].duration; d++)
							assert(c.times[j]+d*gt.rules.nDaysPerWeek!=newtime);*/
		
			assert(conflActivities[newtime].count()==0);
		
			restoreActIndex[nRestore]=ai;
			restoreTime[nRestore]=c.times[ai];
			nRestore++;
			
			moveActivity(ai, c.times[ai], newtime);
			
			foundGoodSwap=true;
			return;
		}
		else{
			int ok=true;
			foreach(int a, conflActivities[newtime])
				if(swappedActivities[a] /*|| gt.rules.activitiesSimilar[ai][a] || gt.rules.activityContained[ai][a]*/){
					ok=false;
					break;
				}
			
			if(!ok)
				continue;

			//////////////place it at a new time
			int oldNRestore=nRestore;
			
			restoreActIndex[nRestore]=ai;
			int oldtime=c.times[ai];
			restoreTime[nRestore]=oldtime;
			nRestore++;

			moveActivity(ai, oldtime, newtime);
			//////////////////
			
			bool all_not_visited=true;
			foreach(int a, conflActivities[newtime])
				if(swappedActivities[a]){
					all_not_visited=false;
					break;
				}
				
			if(all_not_visited)
				foreach(int a, conflActivities[newtime])
					swappedActivities[a]=true;

			foundGoodSwap=false;
			
			ok=false;
			if(all_not_visited){
				assert(conflActivities[newtime].size()>0);
				ok=true;
			
				foreach(int a, conflActivities[newtime]){
					randomswap(a, level+1);
					if(!foundGoodSwap){
						ok=false;
						break;
					}
					assert(foundGoodSwap);
					foundGoodSwap=false;
				}
			}

			if(ok){
				foundGoodSwap=true;
				return;
			}

			if(all_not_visited)
				foreach(int a, conflActivities[newtime])
					swappedActivities[a]=false;
			
			//////////////restore times from the restore list
			for(int j=nRestore-1; j>=oldNRestore; j--){
				assert(c.times[ai]!=UNALLOCATED_TIME);
				
				ai=restoreActIndex[j];
				oldtime=restoreTime[j];
				
				moveActivity(ai, c.times[ai], oldtime);
			}
			nRestore=oldNRestore;
			
			assert(!foundGoodSwap);
			//the return below means no backtracking, if executed
			//return;
		}
	}
}

