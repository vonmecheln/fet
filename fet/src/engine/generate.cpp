/*
File generate.cpp
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

#include <iomanip>
#include <iostream>
using namespace std;

#include "timetable_defs.h"
#include "timetable.h"
#include "generate.h"
#include "rules.h"

#include "generate_pre.h"

#include <QMutex>

extern QMutex mutex; //timetablegenerateform.cpp

#include <QList>
#include <QSet>

#include <QSemaphore>

extern QSemaphore semaphorePlacedActivity;
extern QSemaphore finishedSemaphore;

extern Timetable gt;

bool swappedActivities[MAX_ACTIVITIES];

bool foundGoodSwap;

//static QSet<int> tlistSet[MAX_HOURS_PER_WEEK];

//not sure, it might be necessary 2*... or even more
int restoreActIndex[10*MAX_ACTIVITIES]; //the index of the act. to restore
int restoreTime[10*MAX_ACTIVITIES]; //the time when to restore
int restoreRoom[10*MAX_ACTIVITIES]; //the time when to restore
int nRestore;

//static clock_t start_search_ticks;

int limitcallsrandomswap;

//static double time_limit;

const int MAX_LEVEL=31;

int level_limit;

int ncallsrandomswap;
int maxncallsrandomswap;

//for checking the no gaps and early for students
int nHoursScheduledForSubgroup[MAX_TOTAL_SUBGROUPS];
QList<int> activitiesForSubgroup[MAX_TOTAL_SUBGROUPS][MAX_HOURS_PER_WEEK];

//for checking the max gaps for teachers
int nHoursScheduledForTeacher[MAX_TEACHERS];
QList<int> activitiesForTeacher[MAX_TEACHERS][MAX_HOURS_PER_WEEK];



//for checking the students (set) min hours daily
int nHoursScheduledForSubgroupForDay[MAX_TOTAL_SUBGROUPS][MAX_DAYS_PER_WEEK];



//if level==0, choose best position with lowest number
//of conflicting activities
QList<int> conflActivitiesTimeSlot;
int timeSlot;
int roomSlot;


int triedRemovals[MAX_ACTIVITIES][MAX_HOURS_PER_WEEK];

int impossibleActivity;

int invPermutation[MAX_ACTIVITIES];

const int INF=2000000000;


////////tabu list of tried removals (circular)
const int MAX_TABU=MAX_ACTIVITIES*MAX_HOURS_PER_WEEK; //20000; //400
int tabu_size;
int crt_tabu_index;
int tabu_activities[MAX_TABU];
int tabu_times[MAX_TABU];
////////////


inline bool skipRandom(double weightPercentage)
{
	if(weightPercentage<0)
		return true; //non-existing constraint

	double t=weightPercentage/100.0;
	assert(t>=0 && t<=1);
		
	t*=MM;
	int tt=int(floor(t+0.5));
	assert(tt>=0 && tt<=MM);
						
	int r=randomKnuth();
	assert(r>0 && r<MM); //r cannot be 0
	if(tt<=r)
		return true;
	else
		return false;
}

/*inline bool skipRandom(int weightPercentage)
{
	if(weightPercentage==-1)
		return true; //non-existing constraint
		
	bool rnd100=rand()%100;

	if(weightPercentage<=rnd100)
		return true;
	else
		return false;
}*/


Generate::Generate()
{
}

Generate::~Generate()
{
}

bool Generate::precompute()
{
	return processTimeConstraints();
}

void Generate::generate(int maxSeconds, bool& impossible, bool& timeExceeded/*, const bool semaphorePlacedActivity*/)
{
	impossible=false;
	timeExceeded=false;

	maxncallsrandomswap=-1;

	impossibleActivity=false;

	for(int i=0; i<gt.rules.nInternalActivities; i++)
		for(int j=0; j<gt.rules.nHoursPerWeek; j++)
			triedRemovals[i][j]=0;
			
	////////init tabu
	tabu_size=gt.rules.nInternalActivities*gt.rules.nHoursPerWeek;
	assert(tabu_size<=MAX_TABU);
	crt_tabu_index=0;
	for(int i=0; i<tabu_size; i++)
		tabu_activities[i]=tabu_times[i]=-1;
	/////////////////

	//abortOptimization=false; you have to take care of this before calling this function

	c.makeUnallocated(gt.rules);

	for(int i=0; i<gt.rules.nInternalActivities; i++)
		invPermutation[permutation[i]]=i;

	for(int i=0; i<gt.rules.nInternalActivities; i++)
		swappedActivities[permutation[i]]=false;

	tzset();
	time_t start_time;
	time(&start_time);
	
	//time_limit=0.25; //obsolete
	
	//2000 was before
	//limitcallsrandomswap=1000; //1600, 1500 also good values, 1000 too low???
	limitcallsrandomswap=2*gt.rules.nInternalActivities; //???? value found practically
	
	//14 was before
	level_limit=14; //20; //16
	
	assert(level_limit<MAX_LEVEL);
	
	int maxPlacedActivities=-1;
	
	//int nCallsRandomSwapLevel0=0;
	
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
		
		if(searchTime>=maxSeconds){
			mutex.unlock();
			
			timeExceeded=true;
			
			return;
		}

		for(int i=0; i<=added_act; i++)
			swappedActivities[permutation[i]]=false;
		for(int i=added_act+1; i<gt.rules.nInternalActivities; i++)
			assert(!swappedActivities[permutation[i]]);

		cout<<endl<<"Trying to place activity number added_act=="<<added_act<<
		 "\nwith id=="<<gt.rules.internalActivitiesList[permutation[added_act]].id<<
		 ", from nInternalActivities=="<<gt.rules.nInternalActivities<<endl;
	 
		assert(c.times[permutation[added_act]]==UNALLOCATED_TIME);
		assert(c.rooms[permutation[added_act]]==UNALLOCATED_SPACE);

		/*for(int i=0; i<gt.rules.nHoursPerWeek; i++)
			tlistSet[i].clear();*/
		for(int i=0; i<added_act; i++){
			if(c.times[permutation[i]]==UNALLOCATED_TIME)
				cout<<"ERROR: act with id=="<<gt.rules.internalActivitiesList[permutation[i]].id<<" has time unallocated"<<endl;
			assert(c.times[permutation[i]]!=UNALLOCATED_TIME);
			/*for(int j=0; j<gt.rules.internalActivitiesList[permutation[i]].duration; j++)
				tlistSet[c.times[permutation[i]]+j*gt.rules.nDaysPerWeek].insert(permutation[i]);*/

			if(c.rooms[permutation[i]]==UNALLOCATED_SPACE)
				cout<<"ERROR: act with id=="<<gt.rules.internalActivitiesList[permutation[i]].id<<" has room unallocated"<<endl;
			assert(c.rooms[permutation[i]]!=UNALLOCATED_SPACE);
		}

		///////////////rooms' timetable
		for(int i=0; i<gt.rules.nInternalRooms; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				for(int k=0; k<gt.rules.nHoursPerDay; k++)
					roomsTimetable[i][j][k]=-1;
		for(int j=0; j<added_act; j++){
			int i=permutation[j];
			assert(c.rooms[i]!=UNALLOCATED_SPACE);
			if(c.rooms[i]!=UNSPECIFIED_ROOM){
				int rm=c.rooms[i];
			
				Activity* act=&gt.rules.internalActivitiesList[i];
				int hour=c.times[i]/gt.rules.nDaysPerWeek;
				int day=c.times[i]%gt.rules.nDaysPerWeek;
				for(int dd=0; dd<act->duration && hour+dd<gt.rules.nHoursPerDay; dd++){
					assert(roomsTimetable[rm][day][hour+dd]==-1);
					roomsTimetable[rm][day][hour+dd]=i;
				}
			}
		}
		///////////////////////////////
				
		///////////////care for student (set) max hours daily
		//timetable is also important for other purposes
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

		//care for students (set) min hours daily
		for(int sg=0; sg<gt.rules.nInternalSubgroups; sg++){
			if(subgroupsMinHoursDailyMinHours[sg]>=0){
				for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
					nHoursScheduledForSubgroupForDay[sg][d2]=0;
				
					if(subgroupsEarlyPercentage[sg]>=0 && subgroupsNoGapsPercentage[sg]>=0){ //early+no gaps
						int h2;
						for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
							if(subgroupsTimetable[sg][d2][h2]>=0){
								assert(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[sg][d2][h2]);
								break;
							}
						for(; h2>=0; h2--)
							if(!breakDayHour[d2][h2] && ! subgroupNotAvailableDayHour[sg][d2][h2]) //breaks and not available are not counted as gaps or lessons
								nHoursScheduledForSubgroupForDay[sg][d2]++;
					}
					else if(subgroupsNoGapsPercentage[sg]>=0){ //only no gaps
						int h2;
						for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
							if(subgroupsTimetable[sg][d2][h2]>=0){
								assert(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[sg][d2][h2]);
								break;
							}
						int cnt=0;
						for(; h2>=0; h2--)
							if(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[sg][d2][h2]){
								if(subgroupsTimetable[sg][d2][h2]>=0){
									nHoursScheduledForSubgroupForDay[sg][d2]+=cnt+1;
									cnt=0;
								}
								else
									cnt++;
							}
					}
					else{ //students have no "no gaps" or "early" constraints
						for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
							if(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[sg][d2][h2])
								if(subgroupsTimetable[sg][d2][h2]>=0)
									nHoursScheduledForSubgroupForDay[sg][d2]++;
					}
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
							assert(!breakDayHour[d][h] && !teacherNotAvailableDayHour[i][d][h]);
							break;
						}
							
					int cnt=0;
					for( ; h>=0; h--) if(!breakDayHour[d][h] && !teacherNotAvailableDayHour[i][d][h]){ //break and not available is no gap, is not considered
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
							assert(!breakTime[t] && !teacherNotAvailableTime[i][t]);
							break;
						}
						
					for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !teacherNotAvailableTime[i][t]){ //break and not available is no gap
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
					for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+day; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !subgroupNotAvailableTime[i][t]){ //break and not available is no gap
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
							assert(!breakTime[t] && !subgroupNotAvailableTime[i][t]);
							break;
						}
					}

					for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !subgroupNotAvailableTime[i][t]){ //break and not available is no gap
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
		//start_search_ticks=clock();
		ncallsrandomswap=0;
		randomswap(permutation[added_act], 0);
		
		/*nCallsRandomSwapLevel0++;
		if(nCallsRandomSwapLevel0%300==0){ //forget tried removals
			for(int i=0; i<gt.rules.nInternalActivities; i++)
				for(int j=0; j<gt.rules.nHoursPerWeek; j++)
					triedRemovals[i][j]=0;
		}*/
		
		/*cout<<"ncallsrandomswap=="<<ncallsrandomswap<<endl;
		if(maxncallsrandomswap<ncallsrandomswap)
			maxncallsrandomswap=ncallsrandomswap;
		cout<<"maxncallsrandomswap=="<<maxncallsrandomswap<<endl;*/
			
		//assert(!swappedActivities[permutation[added_act]]);
		//assert(swappedActivities[permutation[added_act]]);
		//swappedActivities[permutation[added_act]]=false;
				
		if(!foundGoodSwap){
			if(impossibleActivity){
				mutex.unlock();
				nDifficultActivities=1;
				difficultActivities[0]=permutation[added_act];
				
				impossible=true;
				
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
				cout<<" time of this activity:"<<c.times[i];
				if(c.rooms[i]!=UNSPECIFIED_ROOM)
					cout<<" room of this activity:"<<qPrintable(gt.rules.internalRoomsList[c.rooms[i]]->name)<<endl;
				else
					cout<<" room of this activity: UNSPECIFIED_ROOM"<<endl;
			}
			//cout<<endl;
			cout<<"timeSlot=="<<timeSlot<<endl;
			if(roomSlot!=UNSPECIFIED_ROOM)
				cout<<"roomSlot=="<<qPrintable(gt.rules.internalRoomsList[roomSlot]->name)<<endl;
			else
				cout<<"roomSlot==UNSPECIFIED_ROOM"<<endl;

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
					
					/////update tabu
					int a=tabu_activities[crt_tabu_index];
					int t=tabu_times[crt_tabu_index];
					if(a>=0 && t>=0){
						assert(triedRemovals[a][t]>0);
						triedRemovals[a][t]--;
						//cout<<"Removing activity with id=="<<gt.rules.internalActivitiesList[a].id<<", time=="<<t<<endl;
					}
					tabu_activities[crt_tabu_index]=permutation[j];
					tabu_times[crt_tabu_index]=c.times[permutation[j]];
					//cout<<"Inserting activity with id=="<<gt.rules.internalActivitiesList[permutation[j]].id<<", time=="<<c.times[permutation[j]]<<endl;
					crt_tabu_index=(crt_tabu_index+1)%tabu_size;
					////////////////
				
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
			//cout<<"q=="<<q<<endl;
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
			c.rooms[tmp]=roomSlot;
			
			for(int i=q+1; i<=added_act; i++){
				c.times[permutation[i]]=UNALLOCATED_TIME;
				c.rooms[permutation[i]]=UNALLOCATED_SPACE;
			}
			c._fitness=-1;
			c.changedForMatrixCalculation=true;
				
			added_act=q+1;
			mutex.unlock();
	
			//if(semaphorePlacedActivity){
				emit(activityPlaced(q+1));
				semaphorePlacedActivity.acquire();
			//}

			goto prevvalue;
//////////////////////
		}			
		else{ //if foundGoodSwap==true
			nPlacedActivities=added_act+1;
			
			if(maxPlacedActivities<nPlacedActivities){
				//forget the old tried removals
				/*for(int i=0; i<gt.rules.nInternalActivities; i++)
					for(int j=0; j<gt.rules.nHoursPerWeek; j++)
						triedRemovals[i][j]=0;*/
				maxPlacedActivities=nPlacedActivities;
			}

			mutex.unlock();
			//cout<<"mutex unlocked - optimizetime 382"<<endl;
			//if(semaphorePlacedActivity){
				emit(activityPlaced(added_act+1));
				semaphorePlacedActivity.acquire();
			//}
			mutex.lock();
			//cout<<"mutex locked - optimizetime 386"<<endl;
			
			/*cout<<"Found good swap:"<<endl;*/
			
			/*QString str;
			cout<<"Fitness of current chromosome=="<<(ftn=c.fitness(gt.rules, &str))<<endl;*/
			//cout<<"str==------------------------"<<endl<<qPrintable(str)<<endl;
			/*if(ftn==0 || added_act==gt.rules.nInternalActivities && foundGoodSwap){*/
			if(added_act==gt.rules.nInternalActivities && foundGoodSwap){
				mutex.unlock();
				break;
			}
			
			bool ok=true;
			for(int i=0; i<=added_act; i++){
				if(c.times[permutation[i]]==UNALLOCATED_TIME){
					cout<<"ERROR: act with id=="<<gt.rules.internalActivitiesList[permutation[i]].id<<" has time unallocated"<<endl;
					ok=false;
				}
				if(c.rooms[permutation[i]]==UNALLOCATED_SPACE){
					cout<<"ERROR: act with id=="<<gt.rules.internalActivitiesList[permutation[i]].id<<" has room unallocated"<<endl;
					ok=false;
				}
			}
			assert(ok);
		}

		mutex.unlock();	
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

void Generate::moveActivity(int ai, int fromslot, int toslot, int fromroom, int toroom)
{
	Activity* act=&gt.rules.internalActivitiesList[ai];

	//cout<<"here: id of act=="<<act->id<<", fromslot=="<<fromslot<<", toslot=="<<toslot<<endl;

	assert(fromslot==c.times[ai]);
	assert(fromroom==c.rooms[ai]);
	
	if(fromslot==UNALLOCATED_TIME || fromroom==UNALLOCATED_SPACE)
		assert(fromslot==UNALLOCATED_TIME && fromroom==UNALLOCATED_SPACE);
	if(toslot==UNALLOCATED_TIME || toroom==UNALLOCATED_SPACE)
		assert(toslot==UNALLOCATED_TIME && toroom==UNALLOCATED_SPACE);
	
	if(fromslot!=UNALLOCATED_TIME){
		int d=fromslot%gt.rules.nDaysPerWeek;
		int h=fromslot/gt.rules.nDaysPerWeek;
		
		////////////////rooms
		int rm=c.rooms[ai];
		if(rm!=UNSPECIFIED_ROOM)
			for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
				assert(dd+h<gt.rules.nHoursPerDay);
				if(roomsTimetable[rm][d][h+dd]==ai)
					roomsTimetable[rm][d][h+dd]=-1;
			}
		/////////////////////
		
		/*for(int t=fromslot; t<fromslot+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
			int tt=tlistSet[t].remove(ai);
			assert(tt==1);
		}*/

		//update for students (set) max hours daily
		//careful: timetable is also used for other purposes
		for(int q=0; q<act->nSubgroups; q++){
			int sb=act->subgroups[q];	
			if(1){
			//the timetable must be computed also for other purposes
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
					assert(subgroupsTimetable[sb][d][h+dd]==ai);
					subgroupsTimetable[sb][d][h+dd]=-1;
				}
			}
		}

		//update for students (set) min hours daily
		for(int q=0; q<act->nSubgroups; q++){
			int sg=act->subgroups[q];
			if(subgroupsMinHoursDailyMinHours[sg]>=0){
				int d2=d;

				nHoursScheduledForSubgroupForDay[sg][d2]=0;
				
				if(subgroupsEarlyPercentage[sg]>=0 && subgroupsNoGapsPercentage[sg]>=0){ //early+no gaps
					int h2;
					for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
						if(subgroupsTimetable[sg][d2][h2]>=0){
							assert(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[sg][d2][h2]);
							break;
						}
					for(; h2>=0; h2--)
						if(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[sg][d2][h2]) //breaks are not counted as gaps or lessons
							nHoursScheduledForSubgroupForDay[sg][d2]++;
				}
				else if(subgroupsNoGapsPercentage[sg]>=0){ //only no gaps
					int h2;
					for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
						if(subgroupsTimetable[sg][d2][h2]>=0){
							assert(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[sg][d2][h2]);
							break;
						}
					int cnt=0;
					for(; h2>=0; h2--)
						if(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[sg][d2][h2]){
							if(subgroupsTimetable[sg][d2][h2]>=0){
								nHoursScheduledForSubgroupForDay[sg][d2]+=cnt+1;
								cnt=0;
							}
							else
								cnt++;
						}
				}
				else{ //students have no "no gaps" or "early" constraints
					for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
						if(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[sg][d2][h2])
							if(subgroupsTimetable[sg][d2][h2]>=0)
								nHoursScheduledForSubgroupForDay[sg][d2]++;
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
					assert(teachersTimetable[tch][d][h+dd]==ai);
					teachersTimetable[tch][d][h+dd]=-1;
				}
			
				teachersNHoursPerDay[tch][d]=0;
				teachersGapsPerDay[tch][d]=0;

				int hh;

				for(hh=gt.rules.nHoursPerDay-1; hh>=0; hh--)
					if(teachersTimetable[tch][d][hh]>=0){
						assert(!breakDayHour[d][hh] && !teacherNotAvailableDayHour[tch][d][hh]);
						break;
					}
						
				int cnt=0;
				for( ; hh>=0; hh--) if(!breakDayHour[d][hh] && !teacherNotAvailableDayHour[tch][d][hh]) { //break and not available is no gap, no activity, nothing
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
						assert(!breakTime[t] && !teacherNotAvailableTime[tc][t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !teacherNotAvailableTime[tc][t]) { //break and not available is no gap
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
						assert(!breakTime[t] && !teacherNotAvailableTime[tc][t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !teacherNotAvailableTime[tc][t]){  //break and not available is no gap
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
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !subgroupNotAvailableTime[isg][t]){ //break and not available is no gap
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
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !subgroupNotAvailableTime[isg][t]){
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
						assert(!breakTime[t] && !subgroupNotAvailableTime[isg][t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !subgroupNotAvailableTime[isg][t]){ //break is no gap
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
						assert(!breakTime[t] && !subgroupNotAvailableTime[isg][t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !subgroupNotAvailableTime[isg][t]){
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
	c.rooms[ai]=toroom;
	c._fitness=-1;
	c.changedForMatrixCalculation=true;
	
	if(toslot!=UNALLOCATED_TIME){
		int d=toslot%gt.rules.nDaysPerWeek;
		int h=toslot/gt.rules.nDaysPerWeek;
		
		////////////////rooms
		int rm=c.rooms[ai];
		if(rm!=UNSPECIFIED_ROOM)
			for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
				assert(dd+h<gt.rules.nHoursPerDay);
				assert(roomsTimetable[rm][d][h+dd]==-1);
				roomsTimetable[rm][d][h+dd]=ai;
			}
		/////////////////////
		
		/*for(int t=toslot; t<toslot+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
			assert(!tlistSet[t].contains(ai));
			tlistSet[t].insert(ai);
		}*/

		//update for students (set) max hours daily
		for(int q=0; q<act->nSubgroups; q++){
			int sb=act->subgroups[q];
			//the timetable must be computed also for other purposes
			if(1){
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
					assert(subgroupsTimetable[sb][d][h+dd]==-1);
					subgroupsTimetable[sb][d][h+dd]=ai;
				}
			}
		}

		//update for students (set) min hours daily
		for(int q=0; q<act->nSubgroups; q++){
			int sg=act->subgroups[q];
			if(subgroupsMinHoursDailyMinHours[sg]>=0){
				int d2=d;

				nHoursScheduledForSubgroupForDay[sg][d2]=0;
				
				if(subgroupsEarlyPercentage[sg]>=0 && subgroupsNoGapsPercentage[sg]>=0){ //early+no gaps
					int h2;
					for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
						if(subgroupsTimetable[sg][d2][h2]>=0){
							assert(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[sg][d2][h2]);
							break;
						}
					for(; h2>=0; h2--)
						if(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[sg][d2][h2]) //breaks and not available are not counted as gaps or lessons
							nHoursScheduledForSubgroupForDay[sg][d2]++;
				}
				else if(subgroupsNoGapsPercentage[sg]>=0){ //only no gaps
					int h2;
					for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
						if(subgroupsTimetable[sg][d2][h2]>=0){
							assert(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[sg][d2][h2]);
							break;
						}
					int cnt=0;
					for(; h2>=0; h2--)
						if(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[sg][d2][h2]){
							if(subgroupsTimetable[sg][d2][h2]>=0){
								nHoursScheduledForSubgroupForDay[sg][d2]+=cnt+1;
								cnt=0;
							}
							else
								cnt++;
						}
				}
				else{ //students have no "no gaps" or "early" constraints
					for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
						if(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[sg][d2][h2])
							if(subgroupsTimetable[sg][d2][h2]>=0)
								nHoursScheduledForSubgroupForDay[sg][d2]++;
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
						assert(!breakDayHour[d][hh] && !teacherNotAvailableDayHour[tch][d][hh]);
						break;
					}
						
				int cnt=0;
				for( ; hh>=0; hh--) if(!breakDayHour[d][hh] && !teacherNotAvailableDayHour[tch][d][hh]) { //break and not available is no gap
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
						assert(!breakTime[t] && !teacherNotAvailableTime[tc][t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !teacherNotAvailableTime[tc][t]){ //break and not available is no gap
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
						assert(!breakTime[t] && !teacherNotAvailableTime[tc][t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !teacherNotAvailableTime[tc][t]){
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
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !subgroupNotAvailableTime[isg][t]){ //break and not available is no gap
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
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !subgroupNotAvailableTime[isg][t]){
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
						assert(!breakTime[t] && !subgroupNotAvailableTime[isg][t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !subgroupNotAvailableTime[isg][t]){ //break and not available is no gap
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
						assert(!breakTime[t] && !subgroupNotAvailableTime[isg][t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !subgroupNotAvailableTime[isg][t]){
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

//faster: (to avoid allocating memory at each call
#if 1

static int nMinDaysBrokenL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
static int selectedRoomL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
static int permL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
static QList<int> conflActivitiesL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
static int conflPermL[MAX_LEVEL][MAX_HOURS_PER_WEEK]; //the permutation in increasing order of number of conflicting activities
static int nConflActivitiesL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
static int roomSlotsL[MAX_LEVEL][MAX_HOURS_PER_WEEK];

#define nMinDaysBroken			(nMinDaysBrokenL[level])
#define selectedRoom			(selectedRoomL[level])
#define perm					(permL[level])
#define conflActivities			(conflActivitiesL[level])
#define conflPerm				(conflPermL[level])
#define nConflActivities		(nConflActivitiesL[level])
#define roomSlots				(roomSlotsL[level])

#endif

void Generate::randomswap(int ai, int level){
	//cout<<"level=="<<level<<endl;

	if(level==0){
		conflActivitiesTimeSlot.clear();
		timeSlot=-1;

		/*for(int l=0; l<level_limit; l++)
			for(int i=0; i<gt.rules.nHoursPerWeek; i++){
				nMinDaysBrokenL[l][i]=0;
				selectedRoomL[l][i]=-1;
				permL[l][i]=-1;
				conflActivitiesL[l][i].clear();
				conflPermL[l][i]=-1;
				nConflActivitiesL[l][i]=0;
				roomSlotsL[l][i]=-1;
			}*/
	}

	if(level>=level_limit){
		return;
	}
	
	/*clock_t crt_search_ticks=clock();
	if(double((double)(crt_search_ticks)-(double)(start_search_ticks))> (double)(CLOCKS_PER_SEC) * (double)(time_limit))
		return;*/
		
	if(ncallsrandomswap>=limitcallsrandomswap)
		return;
		
	/*for(int i=0; i<gt.rules.nHoursPerWeek; i++){
		nMinDaysBroken[i]=0;
		selectedRoom[i]=-1;
		perm[i]=-1;
		conflActivities[i].clear();
		conflPerm[i]=-1;
		nConflActivities[i]=0;
		roomSlots[i]=-1;
	}*/
		
	ncallsrandomswap++;
	
	Activity* act=&gt.rules.internalActivitiesList[ai];
	
#if 0
	int nMinDaysBroken[MAX_HOURS_PER_WEEK]; //to count for broken min n days between activities constraints
	
	int selectedRoom[MAX_HOURS_PER_WEEK];
#endif
		
	//cout<<"ai=="<<ai<<", corresponding to id=="<<gt.rules.internalActivitiesList[ai].id<<", level=="<<level<<endl;

	//generate random permutation like in CLR second edition
	//this is used to scan times in random order
#if 0
	int perm[MAX_HOURS_PER_WEEK];
#endif
	
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		perm[i]=i;
	for(int i=0; i<gt.rules.nHoursPerWeek; i++){
		int t=perm[i];
		int r=randomKnuth()%(gt.rules.nHoursPerWeek-i);
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
#if 0
	QList<int> conflActivities[MAX_HOURS_PER_WEEK];
	int conflPerm[MAX_HOURS_PER_WEEK]; //the permutation in increasing order of number of conflicting activities
	int nConflActivities[MAX_HOURS_PER_WEEK];
	
	int roomSlots[MAX_HOURS_PER_WEEK];
#endif
	
	for(int n=0; n<gt.rules.nHoursPerWeek; n++){
		int newtime=perm[n];
		
		nConflActivities[newtime]=0;
		conflActivities[newtime].clear();

		int d=newtime%gt.rules.nDaysPerWeek;
		int h=newtime/gt.rules.nDaysPerWeek;
		
		nMinDaysBroken[newtime]=0;

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

		///////////////////////////////////
		//added in 5.0.0-preview30
		//same teacher?
		for(int dur=0; dur<act->duration; dur++){
			assert(h+dur<gt.rules.nHoursPerDay);
			for(int q=0; q<act->nTeachers; q++){
				int tch=act->teachers[q];
				if(teachersTimetable[tch][d][h+dur]>=0){
					int ai2=teachersTimetable[tch][d][h+dur];
					assert(ai2!=ai);
				
					if(!skipRandom(activitiesConflictingPercentage[ai][ai2])){
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
			}
		}
		//same subgroup?
		for(int dur=0; dur<act->duration; dur++){
			assert(h+dur<gt.rules.nHoursPerDay);
			for(int q=0; q<act->nSubgroups; q++){
				int sbg=act->subgroups[q];
				if(subgroupsTimetable[sbg][d][h+dur]>=0){
					int ai2=subgroupsTimetable[sbg][d][h+dur];
					assert(ai2!=ai);
			
					if(!skipRandom(activitiesConflictingPercentage[ai][ai2])){
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
			}
		}
		///////////////////////////////////
				
/*		for(int t1=newtime; t1<newtime+act->duration*gt.rules.nDaysPerWeek; t1+=gt.rules.nDaysPerWeek)
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
			}*/
			
			
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
						 ( (d==d2 && (h+act->duration==h2 || h2+gt.rules.internalActivitiesList[ai2].duration==h)) || d!=d2 ) ){
						 	nMinDaysBroken[newtime]++;
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
						 	nMinDaysBroken[newtime]++;
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
			double perc=activitiesSameStartingTimePercentages[ai].at(i);
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
			double perc=activitiesSameStartingHourPercentages[ai].at(i);
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
			double perc=activitiesNotOverlappingPercentages[ai].at(i);
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

		//allowed from 2 activities consecutive
		bool ok2activitiesconsecutive=true;
		
		for(int i=0; i<constr2ActivitiesConsecutiveActivities[ai].count(); i++){
			//direct
			int ai2=constr2ActivitiesConsecutiveActivities[ai].at(i);
			double perc=constr2ActivitiesConsecutivePercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
				int h2=c.times[ai2]/gt.rules.nDaysPerWeek;				
				bool ok=true;
				
				if(d2!=d)
					ok=false;
				else if(h+act->duration > h2)
					ok=false;
				else if(d==d2){
					int kk;
					for(kk=h+act->duration; kk<gt.rules.nHoursPerDay; kk++)
						if(!breakDayHour[d][kk])
							break;
					assert(kk<=h2);
					if(kk!=h2)
						ok=false;
				}
				
				if(!ok && !skipRandom(perc)){
					assert(ai2!=ai);
					
					if(swappedActivities[ai2]){
						ok2activitiesconsecutive=false;
						goto impossible2activitiesconsecutive;
					}
					
					if(conflActivities[newtime].indexOf(ai2)==-1){
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
					}
				}
			}
		}

		for(int i=0; i<inverseConstr2ActivitiesConsecutiveActivities[ai].count(); i++){
			//inverse
			int ai2=inverseConstr2ActivitiesConsecutiveActivities[ai].at(i);
			double perc=inverseConstr2ActivitiesConsecutivePercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
				int h2=c.times[ai2]/gt.rules.nDaysPerWeek;				
				bool ok=true;
				
				if(d2!=d)
					ok=false;
				else if(h2+gt.rules.internalActivitiesList[ai2].duration > h)
					ok=false;
				else if(d==d2){
					int kk;
					for(kk=h2+gt.rules.internalActivitiesList[ai2].duration; kk<gt.rules.nHoursPerDay; kk++)
						if(!breakDayHour[d][kk])
							break;
					assert(kk<=h);
					if(kk!=h)
						ok=false;
				}
			
				if(!ok && !skipRandom(perc)){
					assert(ai2!=ai);
					
					if(swappedActivities[ai2]){
						ok2activitiesconsecutive=false;
						goto impossible2activitiesconsecutive;
					}
					
					if(conflActivities[newtime].indexOf(ai2)==-1){
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
					}
				}
			}
		}
		
impossible2activitiesconsecutive:
		if(!ok2activitiesconsecutive){
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
						assert(!breakDayHour[d][j] && !subgroupNotAvailableDayHour[sb][d][j]);
						break;
					}
				
				int cnt=0;
				
				for( ; j>=0; j--) if(!breakDayHour[d][j] && !subgroupNotAvailableDayHour[sb][d][j]){ //break and not available is no gap, no activity
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
						//failed the test in lines above (line 1396 now)
						assert(!breakDayHour[d][j] && !subgroupNotAvailableDayHour[sb][d][j]);
						break;
					}
				
				cnt=0;
				
				for( ; j>=0; j--) if(!breakDayHour[d][j] && !subgroupNotAvailableDayHour[sb][d][j]){
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

		//allowed from students (set) min hours daily
		bool okstudentsminhoursdaily=true;
		for(int q=0; q<act->nSubgroups; q++){
			int sb=act->subgroups[q];
			if(subgroupsMinHoursDailyMinHours[sb]>=0){
				/*int nhinit=0;
				for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
					nhinit+=nHoursScheduledForSubgroupForDay[sb][d2];*/
					
				int nhday=0;

				if(subgroupsEarlyPercentage[sb]>=0 && subgroupsNoGapsPercentage[sb]>=0){ //early+no gaps
					int h2;
					for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
						if(subgroupsTimetable[sb][d][h2]>=0 || h2>=h && h2<h+act->duration){
							assert(!breakDayHour[d][h2] && !subgroupNotAvailableDayHour[sb][d][h2]);
							break;
						}
					for(; h2>=0; h2--)
						if(!breakDayHour[d][h2] && !subgroupNotAvailableDayHour[sb][d][h2]) //breaks and not available are not counted as gaps or lessons
							nhday++;
				}
				else if(subgroupsNoGapsPercentage[sb]>=0){ //only no gaps
					int h2;
					for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
						if(subgroupsTimetable[sb][d][h2]>=0 || h2>=h && h2<h+act->duration){
							assert(!breakDayHour[d][h2] && !subgroupNotAvailableDayHour[sb][d][h2]);
							break;
						}
					int cnt=0;
					for(; h2>=0; h2--)
						if(!breakDayHour[d][h2] && !subgroupNotAvailableDayHour[sb][d][h2]){
							if(subgroupsTimetable[sb][d][h2]>=0 || h2>=h && h2<h+act->duration){
								nhday+=cnt+1;
								cnt=0;
							}
							else
								cnt++;
						}
				}
				else{ //students have no "no gaps" or "early" constraints
					for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
						if(!breakDayHour[d][h2] && !subgroupNotAvailableDayHour[sb][d][h2])
							if(subgroupsTimetable[sb][d][h2]>=0 || h2>=h && h2<h+act->duration)
								nhday++;
				}
				
				int nrequested=0;
				int nold=0;
				for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
					if(d2!=d)
						nrequested+=max(subgroupsMinHoursDailyMinHours[sb], nHoursScheduledForSubgroupForDay[sb][d2]);
					else
						nrequested+=max(subgroupsMinHoursDailyMinHours[sb], nhday);
						
					nold+=max(subgroupsMinHoursDailyMinHours[sb], nHoursScheduledForSubgroupForDay[sb][d2]);
				}
				
				assert(subgroupsMinHoursDailyPercentages[sb]==100);
						
				if(subgroupsMinHoursDailyPercentages[sb]==100)
					assert(nold<=nHoursPerSubgroup[sb]);
				
				if(nrequested>nHoursPerSubgroup[sb]){
					assert(subgroupsMinHoursDailyPercentages[sb]==100);
					if(subgroupsMinHoursDailyPercentages[sb]==100)
						assert(nhday>subgroupsMinHoursDailyMinHours[sb] && nhday>nHoursScheduledForSubgroupForDay[sb][d]);
						
					if(!skipRandom(subgroupsMinHoursDailyPercentages[sb])){
						/////////////EARLY+NO GAPS
						if(subgroupsEarlyPercentage[sb]>=0 && subgroupsNoGapsPercentage[sb]>=0){ //early+no gaps
							int dayEnd[MAX_DAYS_PER_WEEK];
							int nRem[MAX_DAYS_PER_WEEK];
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								if(d2!=d){
									dayEnd[d2]=gt.rules.nHoursPerDay-1;
									while(dayEnd[d2]>=0 && (breakDayHour[d2][dayEnd[d2]] || subgroupNotAvailableDayHour[sb][d2][dayEnd[d2]] || subgroupsTimetable[sb][d2][dayEnd[d2]]==-1))
										dayEnd[d2]--;
								}
								else
									dayEnd[d2]=-1;
									
								nRem[d2]=nHoursScheduledForSubgroupForDay[sb][d2]-subgroupsMinHoursDailyMinHours[sb];
							}
							
							int excess=nrequested-nHoursPerSubgroup[sb];
							
							QList<int> conflActs;
							
							bool ok=false;
							
							for(;;){
								int d2;
								bool found=false;
								int conflAct[MAX_DAYS_PER_WEEK];
								for(d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									conflAct[d2]=-2; //impossible
									while(dayEnd[d2]>=0 && (breakDayHour[d2][dayEnd[d2]] || subgroupNotAvailableDayHour[sb][d2][dayEnd[d2]]))
										dayEnd[d2]--;
									if(dayEnd[d2]>=0 && nRem[d2]>0 && 
									 (subgroupsTimetable[sb][d2][dayEnd[d2]]==-1
									 || subgroupsTimetable[sb][d2][dayEnd[d2]]>=0 && !swappedActivities[subgroupsTimetable[sb][d2][dayEnd[d2]]])){
										found=true;
										if(conflActivities[newtime].indexOf(subgroupsTimetable[sb][d2][dayEnd[d2]])==-1
										 && conflActs.indexOf(subgroupsTimetable[sb][d2][dayEnd[d2]])==-1)
											conflAct[d2]=subgroupsTimetable[sb][d2][dayEnd[d2]];
										else
											conflAct[d2]=-1;
									}
								}
								if(!found){
									ok=false;
									break;
								}
								
								//chose earliest placed activity, to avoid cycling
								int m=gt.rules.nInternalActivities;
								int mpos=-1;
								for(d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(nRem[d2]>0){
										if(conflAct[d2]==-1)
											break;
										else if(conflAct[d2]>=0 && m>invPermutation[conflAct[d2]]){
											m=invPermutation[conflAct[d2]];
											mpos=d2;
										}
									}
								}
								assert(m>=0 && m<gt.rules.nInternalActivities && mpos>=0 || d2<gt.rules.nDaysPerWeek);
								if(d2==gt.rules.nDaysPerWeek)
									d2=mpos;
								
								assert(nRem[d2]>0);
								nRem[d2]--;
								excess--;
								dayEnd[d2]--;
								
								assert(conflAct[d2]>=-1);
								if(conflActs.indexOf(conflAct[d2])==-1 && conflAct[d2]>=0)
									conflActs.append(conflAct[d2]);
								
								if(excess==0){
									ok=true;
									break;
								}
							}
							
							if(ok){
								foreach(int ai2, conflActs){
									assert(conflActivities[newtime].indexOf(ai2)==-1);
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
								}
							}
							else{
								/*cout<<"generate.cpp line 1933 - no slots found"<<endl;
								cout<<"d=="<<d<<", h=="<<h<<", newtime=="<<newtime<<endl;
								cout<<"nhday=="<<nhday<<", subgroupsMinHoursDailyMinHours[sb]=="<<subgroupsMinHoursDailyMinHours[sb]<<endl;*/
		
								okstudentsminhoursdaily=false;
								goto impossiblestudentsminhoursdaily;
							}
						}
						/////////////ONLY NO GAPS
						else if(subgroupsNoGapsPercentage[sb]>=0){ //only no gaps
							int dayEnd[MAX_DAYS_PER_WEEK];
							int dayStart[MAX_DAYS_PER_WEEK];
							int nRem[MAX_DAYS_PER_WEEK];
							
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								if(d2!=d){
									dayEnd[d2]=gt.rules.nHoursPerDay-1;
									while(dayEnd[d2]>=0 && (breakDayHour[d2][dayEnd[d2]] || subgroupNotAvailableDayHour[sb][d2][dayEnd[d2]] || subgroupsTimetable[sb][d2][dayEnd[d2]]==-1))
										dayEnd[d2]--;
								}
								else{
									dayEnd[d2]=gt.rules.nHoursPerDay-1;
									while(dayEnd[d2]>=0 && 
									 (breakDayHour[d2][dayEnd[d2]] || subgroupNotAvailableDayHour[sb][d2][dayEnd[d2]] || (subgroupsTimetable[sb][d2][dayEnd[d2]]==-1 && !(dayEnd[d2]>=h && dayEnd[d2]<h+act->duration))))
										dayEnd[d2]--;
								}
								
								if(d2!=d){
									dayStart[d2]=0;
									while(dayStart[d2]<gt.rules.nHoursPerDay && (breakDayHour[d2][dayStart[d2]] || subgroupNotAvailableDayHour[sb][d2][dayStart[d2]] || subgroupsTimetable[sb][d2][dayStart[d2]]==-1))
										dayStart[d2]++;
								}
								else{
									dayStart[d2]=0;
									while(dayStart[d2]<gt.rules.nHoursPerDay && 
									 (breakDayHour[d2][dayStart[d2]] || subgroupNotAvailableDayHour[sb][d2][dayStart[d2]] || (subgroupsTimetable[sb][d2][dayStart[d2]]==-1 && !(dayStart[d2]>=h && dayStart[d2]<h+act->duration))))
										dayStart[d2]++;
								}
									
								if(d2!=d)
									nRem[d2]=nHoursScheduledForSubgroupForDay[sb][d2]-subgroupsMinHoursDailyMinHours[sb];
								else{
									nRem[d2]=nhday-subgroupsMinHoursDailyMinHours[sb]/*-act->duration*/;
									assert(nRem[d2]==dayEnd[d2]-dayStart[d2]+1-subgroupsMinHoursDailyMinHours[sb]/*-act->duration*/);
									//cout<<"nRem[d]=="<<nRem[d]<<endl;
								}
							}
							
							int excess=nrequested-nHoursPerSubgroup[sb];
							
							QList<int> conflActs;
							
							bool ok=false;
							
							for(;;){
								int d2;
								bool foundEnd=false;
								bool foundStart=false;
								int conflActEnd[MAX_DAYS_PER_WEEK];
								int conflActStart[MAX_DAYS_PER_WEEK];
								for(d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									conflActEnd[d2]=-2; //impossible
									while(dayEnd[d2]>=0 && (breakDayHour[d2][dayEnd[d2]] || subgroupNotAvailableDayHour[sb][d2][dayEnd[d2]]))
										dayEnd[d2]--;

									conflActStart[d2]=-2; //impossible
									while(dayStart[d2]<gt.rules.nHoursPerDay && (breakDayHour[d2][dayStart[d2]] || subgroupNotAvailableDayHour[sb][d2][dayStart[d2]]))
										dayStart[d2]++;

									if(d2!=d){
										if(dayEnd[d2]>=0 && dayEnd[d2]>=dayStart[d2] && nRem[d2]>0 && 
										 (subgroupsTimetable[sb][d2][dayEnd[d2]]==-1
										 || subgroupsTimetable[sb][d2][dayEnd[d2]]>=0 && !swappedActivities[subgroupsTimetable[sb][d2][dayEnd[d2]]])){
											foundEnd=true;
											if(conflActivities[newtime].indexOf(subgroupsTimetable[sb][d2][dayEnd[d2]])==-1
											 && conflActs.indexOf(subgroupsTimetable[sb][d2][dayEnd[d2]])==-1)
												conflActEnd[d2]=subgroupsTimetable[sb][d2][dayEnd[d2]];
											else
												conflActEnd[d2]=-1;
										}
									}
									else{
										if(dayEnd[d2]>=0 && dayEnd[d2]>=dayStart[d2] && nRem[d2]>0
										 && !(dayEnd[d2]>=h && dayEnd[d2]<h+act->duration)
										 && (subgroupsTimetable[sb][d2][dayEnd[d2]]==-1
										 || subgroupsTimetable[sb][d2][dayEnd[d2]]>=0 && !swappedActivities[subgroupsTimetable[sb][d2][dayEnd[d2]]])){
											foundEnd=true;
											if(conflActivities[newtime].indexOf(subgroupsTimetable[sb][d2][dayEnd[d2]])==-1
											 && conflActs.indexOf(subgroupsTimetable[sb][d2][dayEnd[d2]])==-1)
												conflActEnd[d2]=subgroupsTimetable[sb][d2][dayEnd[d2]];
											else
												conflActEnd[d2]=-1;
										}
									}

									if(d2!=d){
										if(dayStart[d2]<gt.rules.nHoursPerDay && dayEnd[d2]>=dayStart[d2] && nRem[d2]>0 && 
										 (subgroupsTimetable[sb][d2][dayStart[d2]]==-1
										 || subgroupsTimetable[sb][d2][dayStart[d2]]>=0 && !swappedActivities[subgroupsTimetable[sb][d2][dayStart[d2]]])){
											foundStart=true;
											if(conflActivities[newtime].indexOf(subgroupsTimetable[sb][d2][dayStart[d2]])==-1
											 && conflActs.indexOf(subgroupsTimetable[sb][d2][dayStart[d2]])==-1)
												conflActStart[d2]=subgroupsTimetable[sb][d2][dayStart[d2]];
											else
												conflActStart[d2]=-1;
										}
									}
									else{
										if(dayStart[d2]<gt.rules.nHoursPerDay && dayEnd[d2]>=dayStart[d2] && nRem[d2]>0
										 && !(dayStart[d2]>=h && dayStart[d2]<h+act->duration)
										 && (subgroupsTimetable[sb][d2][dayStart[d2]]==-1
										 || subgroupsTimetable[sb][d2][dayStart[d2]]>=0 && !swappedActivities[subgroupsTimetable[sb][d2][dayStart[d2]]])){
											foundStart=true;
											if(conflActivities[newtime].indexOf(subgroupsTimetable[sb][d2][dayStart[d2]])==-1
											 && conflActs.indexOf(subgroupsTimetable[sb][d2][dayStart[d2]])==-1)
												conflActStart[d2]=subgroupsTimetable[sb][d2][dayStart[d2]];
											else
												conflActStart[d2]=-1;
										}
									}
								}
								if(!foundEnd && !foundStart){
									ok=false;
									break;
								}
								
								int chooseStart=-1;
								int bestD=-1;
								//chose earliest placed activity, to avoid cycling
								int m=gt.rules.nInternalActivities;
								int mpos=-1;
								if(foundEnd){
									for(d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										if(nRem[d2]>0){
											if(conflActEnd[d2]==-1)
												break;
											else if(conflActEnd[d2]>=0 && m>invPermutation[conflActEnd[d2]]){
												m=invPermutation[conflActEnd[d2]];
												mpos=d2;
											}
										}
									}
									assert(m>=0 && m<gt.rules.nInternalActivities && mpos>=0 || d2<gt.rules.nDaysPerWeek);
									if(d2==gt.rules.nDaysPerWeek)
										d2=mpos;
									chooseStart=false;
									bestD=d2;
								}
								if(foundStart){
									int oldm=m;
									for(d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										if(nRem[d2]>0){
											if(conflActStart[d2]==-1)
												break;
											else if(conflActStart[d2]>=0 && m>invPermutation[conflActStart[d2]]){
												m=invPermutation[conflActStart[d2]];
												mpos=d2;
											}
										}
									}
									assert(m>=0 && m<gt.rules.nInternalActivities && mpos>=0 || d2<gt.rules.nDaysPerWeek);
									if(d2<gt.rules.nDaysPerWeek){
										bestD=d2;
										chooseStart=true;
									}
									else if(oldm!=m){
										assert(oldm>m);
									
										d2=mpos;
										bestD=d2;
										chooseStart=true;
									}
								}
								
								assert(chooseStart>=0);
									
								assert(bestD>=0);
								d2=bestD;
								
								assert(nRem[d2]>0);
								nRem[d2]--;
								excess--;
								
								if(chooseStart){
									dayStart[d2]++;

									assert(conflActStart[d2]>=-1);
									if(conflActs.indexOf(conflActStart[d2])==-1 && conflActStart[d2]>=0)
										conflActs.append(conflActStart[d2]);
								}
								else{
									dayEnd[d2]--;
			
									assert(conflActEnd[d2]>=-1);
									if(conflActs.indexOf(conflActEnd[d2])==-1 && conflActEnd[d2]>=0)
										conflActs.append(conflActEnd[d2]);								
								}
								
								if(excess==0){
									ok=true;
									break;
								}
							}
							
							if(ok){
								foreach(int ai2, conflActs){
									assert(conflActivities[newtime].indexOf(ai2)==-1);
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
								}
							}
							else{
								if(level==0){
									cout<<"WARNING: level==0 and: "<<endl;
									cout<<"generate.cpp line 2152 - no slots found"<<endl;
									cout<<"d=="<<d<<", h=="<<h<<", newtime=="<<newtime<<endl;
									cout<<"nhday=="<<nhday<<", subgroupsMinHoursDailyMinHours[sb]=="<<subgroupsMinHoursDailyMinHours[sb]<<endl;
									cout<<endl;
								}
		
								okstudentsminhoursdaily=false;
								goto impossiblestudentsminhoursdaily;
							}
						}
						/////////////NOTHING (early and no gaps not present)
						else{
							int nRem[MAX_DAYS_PER_WEEK];
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								if(d2!=d)
									nRem[d2]=nHoursScheduledForSubgroupForDay[sb][d2]-subgroupsMinHoursDailyMinHours[sb];
								else{
									nRem[d2]=nhday-subgroupsMinHoursDailyMinHours[sb];
									//NOT ALWAYS TRUE: assert(nRem[d2]==nHoursScheduledForSubgroupForDay[sb][d2]-subgroupsMinHoursDailyMinHours[sb]+act->duration);
									//(for superimposed act over old activities)
								}
							}
							
							int excess=nrequested-nHoursPerSubgroup[sb];
							
							QList<int> conflActs;
							
							bool ok=false;
							
							QList<int> triedDays;
							QList<int> triedHours;
							
							for(;;){
								int d2;
								bool found=false;
								int conflAct=-2;
								int selDay=-1;
								int selHour=-1;
								
								for(d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(nRem[d2]>0){
										for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
											bool alreadyTried=false;
											assert(triedDays.count()==triedHours.count());
											for(int k=0; k<triedDays.count(); k++)
												if(triedDays.at(k)==d2 && triedHours.at(k)==h2){
													alreadyTried=true;
													break;
												}
												
											if(alreadyTried)
												continue;
										
											if(!breakDayHour[d2][h2] &&!subgroupNotAvailableDayHour[sb][d2][h2] && !(d2==d && h2>=h && h2<h+act->duration) ){
												if(subgroupsTimetable[sb][d2][h2]>=0 && !swappedActivities[subgroupsTimetable[sb][d2][h2]]){
													//choose conflAct as earlier placed as possible, to avoid cycles
													if(conflAct==-2 || conflAct>=0 && invPermutation[conflAct]>invPermutation[subgroupsTimetable[sb][d2][h2]]){
														found=true;
														conflAct=subgroupsTimetable[sb][d2][h2];
														selDay=d2;
														selHour=h2;
														
														if(conflActs.indexOf(conflAct)>=0 || conflActivities[newtime].indexOf(conflAct)>=0){ //already in, do not count it again
															conflAct=-1;
															goto alreadyIn;
														}
													}
												}
											}
										}
									}
								}
								
alreadyIn:
								
								if(!found){
									ok=false;
									break;
								}
								
								assert(selDay>=0);
								assert(selDay<gt.rules.nDaysPerWeek);
								assert(selHour>=0);
								assert(selHour<gt.rules.nHoursPerDay);
								
								triedDays.append(selDay);
								triedHours.append(selHour);
								
								d2=selDay;
								assert(nRem[d2]>0);
								nRem[d2]--;
								excess--;
								
								assert(conflAct>=-1);
								if(conflAct!=-1 && conflActs.indexOf(conflAct)==-1)
									conflActs.append(conflAct);
								
								if(excess==0){
									ok=true;
									break;
								}
							}
							
							if(ok){
								foreach(int ai2, conflActs){
									assert(conflActivities[newtime].indexOf(ai2)==-1);
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
								}
							}
							else{
								if(level==0){
									cout<<"generate.cpp line 2227 - no slots found"<<endl;
									cout<<"d=="<<d<<", h=="<<h<<", newtime=="<<newtime<<endl;
									cout<<"nhday=="<<nhday<<", subgroupsMinHoursDailyMinHours[sb]=="<<subgroupsMinHoursDailyMinHours[sb]<<endl;
								}
		
								okstudentsminhoursdaily=false;
								goto impossiblestudentsminhoursdaily;
							}
						}
					}
				}
			}
		}
impossiblestudentsminhoursdaily:
		if(!okstudentsminhoursdaily){
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
						assert(!breakDayHour[d][j] && !teacherNotAvailableDayHour[i][d][j]);
						break;
					}
				
				int cnt=0;
				
				for( ; j>=0; j--) if(!breakDayHour[d][j] && !teacherNotAvailableDayHour[i][d][j]){ //break and not available is no gap, no activity
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
						assert(!breakDayHour[d][j] && !teacherNotAvailableDayHour[i][d][j]);
						break;
					}
				
				cnt=0;
				
				for( ; j>=0; j--) if(!breakDayHour[d][j] && !teacherNotAvailableDayHour[i][d][j]){
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
						assert(!breakTime[t] && !teacherNotAvailableTime[tc][t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !teacherNotAvailableTime[tc][t]){ //break and not available is no gap, no activity
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
						assert(!breakTime[t] && !teacherNotAvailableTime[tc][t]); //this slot has no break
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !teacherNotAvailableTime[tc][t]){
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
									assert(!breakDayHour[d2][h2] && !teacherNotAvailableDayHour[tc][d2][h2]);
									break;
								}
							}
							bool okswapped=true;
							for(; h2>=0; h2--) if(!breakDayHour[d2][h2] && !teacherNotAvailableDayHour[tc][d2][h2]) {
								int ttt=teachersTimetable[tc][d2][h2];
								assert(ttt!=ai);
								if(ttt>=0){
									if(swappedActivities[ttt]){
										okswapped=false;
									}
									
									if(conflActs1[d2].indexOf(ttt)==-1 && conflActivities[newtime].indexOf(ttt)==-1)
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
							for(h2--; h2>=0; h2--) if(!breakDayHour[d2][h2] && !teacherNotAvailableDayHour[tc][d2][h2])
								if(teachersTimetable[tc][d2][h2]>=0){
									okhere=true;
									break;
								}
							if(excess>0 || !okhere || !okswapped){
								nWrong1[d2]=-1;
								minWrong1[d2]=-1;
							}
							else{
/*#if 1
								if(optNConflActs>conflActs1[d2].count()){
#endif*/
/*#if 1
								if(optNWrong>nWrong1[d2]
								 || optNWrong==nWrong1[d2] && optNConflActs>conflActs1[d2].count()){
#endif*/
#if 1
								if((level==0 && (optNWrong>nWrong1[d2] || optNWrong==nWrong1[d2] && optNConflActs>conflActs1[d2].count()))
								 || (level>0 && optNConflActs>conflActs1[d2].count())){
#endif
/*#if 1&0
								if(optNWrong>nWrong1[d2]
								 || optNWrong==nWrong1[d2] && minWrong1[d2]<optMinWrong
								 || optNWrong==nWrong1[d2] && minWrong1[d2]==optMinWrong && optNConflActs>conflActs1[d2].count()){
#endif*/
/*#if 0
								if(minWrong1[d2]<optMinWrong
								 || optNWrong>nWrong1[d2] && minWrong1[d2]==optMinWrong
								 || optNWrong==nWrong1[d2] && minWrong1[d2]==optMinWrong && optNConflActs>conflActs1[d2].count()){
#endif*/
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
									assert(!breakDayHour[d2][h2] && !teacherNotAvailableDayHour[tc][d2][h2]);
									break;
								}
							}
							bool okswapped=true;
							for(; h2>=0; h2--) if(!breakDayHour[d2][h2] && !teacherNotAvailableDayHour[tc][d2][h2]) {
								int ttt=teachersTimetable[tc][d2][h2];
								assert(ttt!=ai);
								if(ttt>=0){
									if(swappedActivities[ttt])
										okswapped=false;
								
									if(conflActs1[d2].indexOf(ttt)==-1 && conflActivities[newtime].indexOf(ttt)==-1)
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
							for(int h3=h2-1; h3>=0; h3--) if(!breakDayHour[d2][h3] && !teacherNotAvailableDayHour[tc][d2][h3])
								if(teachersTimetable[tc][d2][h3]>=0 || h3>=h && h3<h+act->duration){
									okhere=true;
									break;
								}
							if(excess>0 || h2<h+act->duration || !okhere || !okswapped){
								nWrong1[d2]=-1;
								minWrong1[d2]=-1;
							}
							else{										
/*#if 1
								if(optNConflActs>conflActs1[d2].count()){
#endif*/
/*#if 1
								if(optNWrong>nWrong1[d2]
								 || optNWrong==nWrong1[d2] && optNConflActs>conflActs1[d2].count()){
#endif*/
#if 1
								if((level==0 && (optNWrong>nWrong1[d2] || optNWrong==nWrong1[d2] && optNConflActs>conflActs1[d2].count()))
								 || (level>0 && optNConflActs>conflActs1[d2].count())){
#endif
/*#if 1&0
								if(optNWrong>nWrong1[d2]
								 || optNWrong==nWrong1[d2] && minWrong1[d2]<optMinWrong
								 || optNWrong==nWrong1[d2] && minWrong1[d2]==optMinWrong && optNConflActs>conflActs1[d2].count()){
#endif*/
/*#if 0
								if(minWrong1[d2]<optMinWrong
								 || optNWrong>nWrong1[d2] && minWrong1[d2]==optMinWrong
								 || optNWrong==nWrong1[d2] && minWrong1[d2]==optMinWrong && optNConflActs>conflActs1[d2].count()){
#endif*/
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
									assert(!breakDayHour[d2][h2] && !teacherNotAvailableDayHour[tc][d2][h2]);
									break;
								}
							}
							bool okswapped=true;
							for(; h2<gt.rules.nHoursPerDay; h2++) if(!breakDayHour[d2][h2] && !teacherNotAvailableDayHour[tc][d2][h2]) {
								int ttt=teachersTimetable[tc][d2][h2];
								assert(ttt!=ai);
								if(ttt>=0){
									if(swappedActivities[ttt])
										okswapped=false;
								
									if(conflActs2[d2].indexOf(ttt)==-1 && conflActivities[newtime].indexOf(ttt)==-1)
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
							for(h2++; h2<gt.rules.nHoursPerDay; h2++) if(!breakDayHour[d2][h2] && !teacherNotAvailableDayHour[tc][d2][h2])
								if(teachersTimetable[tc][d2][h2]>=0){
									okhere=true;
									break;
								}
							if(excess>0 || !okhere || !okswapped){
								nWrong2[d2]=-1;
								minWrong2[d2]=-1;
							}
							else{										
/*#if 1
								if(optNConflActs>conflActs2[d2].count()){
#endif*/
/*#if 1
								if(optNWrong>nWrong2[d2]
								 || optNWrong==nWrong2[d2] && optNConflActs>conflActs2[d2].count()){
#endif*/
#if 1
								if((level==0 && (optNWrong>nWrong2[d2] || optNWrong==nWrong2[d2] && optNConflActs>conflActs2[d2].count()))
								 || (level>0 && optNConflActs>conflActs2[d2].count())){
#endif
/*#if 1&0
								if(optNWrong>nWrong2[d2]
								 || optNWrong==nWrong2[d2] && minWrong2[d2]<optMinWrong
								 || optNWrong==nWrong2[d2] && minWrong2[d2]==optMinWrong && optNConflActs>conflActs2[d2].count()){
#endif*/
/*#if 0
								if(minWrong2[d2]<optMinWrong
								 || optNWrong>nWrong2[d2] && minWrong2[d2]==optMinWrong
								 || optNWrong==nWrong2[d2] && minWrong2[d2]==optMinWrong && optNConflActs>conflActs2[d2].count()){
#endif*/
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
									assert(!breakDayHour[d2][h2] && !teacherNotAvailableDayHour[tc][d2][h2]);
									break;
								}
							}
							bool okswapped=true;
							for(; h2<gt.rules.nHoursPerDay; h2++) if(!breakDayHour[d2][h2] && !teacherNotAvailableDayHour[tc][d2][h2]) {
								int ttt=teachersTimetable[tc][d2][h2];
								assert(ttt!=ai);
								if(ttt>=0){
									if(swappedActivities[ttt])
										okswapped=false;
									
									if(conflActs2[d2].indexOf(ttt)==-1 && conflActivities[newtime].indexOf(ttt)==-1)
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
							for(int h3=h2+1; h3<gt.rules.nHoursPerDay; h3++) if(!breakDayHour[d2][h3] && !teacherNotAvailableDayHour[tc][d2][h3])
								if(teachersTimetable[tc][d2][h3]>=0 || h3>=h && h3<h+act->duration){
									okhere=true;
									break;
								}
							if(excess>0 || h2>=h || !okhere || !okswapped){
								nWrong2[d2]=-1;
								minWrong2[d2]=-1;
							}
							else{
/*#if 1
								if(optNConflActs>conflActs2[d2].count()){
#endif*/
/*#if 1
								if(optNWrong>nWrong2[d2]
								 || optNWrong==nWrong2[d2] && optNConflActs>conflActs2[d2].count()){
#endif*/
#if 1
								if((level==0 && (optNWrong>nWrong2[d2] || optNWrong==nWrong2[d2] && optNConflActs>conflActs2[d2].count()))
								 || (level>0 && optNConflActs>conflActs2[d2].count())){
#endif
/*#if 1&0
								if(optNWrong>nWrong2[d2]
								 || optNWrong==nWrong2[d2] && minWrong2[d2]<optMinWrong
								 || optNWrong==nWrong2[d2] && minWrong2[d2]==optMinWrong && optNConflActs>conflActs2[d2].count()){
#endif*/
/*#if 0
								if(minWrong2[d2]<optMinWrong
								 || optNWrong>nWrong2[d2] && minWrong2[d2]==optMinWrong
								 || optNWrong==nWrong2[d2] && minWrong2[d2]==optMinWrong && optNConflActs>conflActs2[d2].count()){
#endif*/
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
/*#if 1
								if(optNConflActs==conflActs1[d2].count()){
#endif*/
/*#if 1
								if(optNWrong==nWrong1[d2] && optNConflActs==conflActs1[d2].count()){
#endif*/
#if 1
								if(level==0 && optNWrong==nWrong1[d2] && optNConflActs==conflActs1[d2].count()
								 || level>0 && optNConflActs==conflActs1[d2].count()){
#endif
/*#if 0
								if(optNWrong==nWrong1[d2] && minWrong1[d2]==optMinWrong && optNConflActs==conflActs1[d2].count()){
#endif*/
									optDays.append(d2);
									where.append(1);
								}
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
							if(nWrong2[d2]>=0)
/*#if 1
								if(optNConflActs==conflActs2[d2].count()){
#endif*/
/*#if 1
								if(optNWrong==nWrong2[d2] && optNConflActs==conflActs2[d2].count()){
#endif*/
#if 1
								if(level==0 && optNWrong==nWrong2[d2] && optNConflActs==conflActs2[d2].count()
								 || level>0 && optNConflActs==conflActs2[d2].count()){
#endif
/*#if 0
								if(optNWrong==nWrong2[d2] && minWrong2[d2]==optMinWrong && optNConflActs==conflActs2[d2].count()){
#endif*/
									optDays.append(d2);
									where.append(2);
								}
						assert(optDays.count()>0);
						int rnd=randomKnuth()%optDays.count();
						int d2=optDays.at(rnd);
						int w=where.at(rnd);
						assert(d2>=0);
								
						bool ok=false; //TODO: remove this variable
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
								else
									assert(0);
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
								else
									assert(0);
							}
						}
						//assert(ok);
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
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !subgroupNotAvailableTime[isg][t]){ //break and not available is no gap, no activity
					if(activitiesForSubgroup[isg][t].count()>0)
						count=true;
					if(count)
						cnt--;
				}

				count=false;
				for(int t=(gt.rules.nHoursPerDay-1)*gt.rules.nDaysPerWeek+d; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !subgroupNotAvailableTime[isg][t]){
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
											assert(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[isg][d2][h2]);
											break;
										}
									}
									bool okswapped=true;
									for(; h2>=0; h2--) if(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[isg][d2][h2]) {
										int ttt=subgroupsTimetable[isg][d2][h2];
										assert(ttt!=ai);
										if(ttt>=0){
											if(swappedActivities[ttt])
												okswapped=false;
											if(conflActs[d2].indexOf(ttt)==-1 && conflActivities[newtime].indexOf(ttt)==-1)
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
/*#if 1
										if(optNConflActs>conflActs[d2].count()){
#endif*/
/*#if 1
										if(optNWrong>nWrong[d2]
										 || optNWrong==nWrong[d2] && optNConflActs>conflActs[d2].count()){
#endif*/
#if 1
										if(level==0 && (optNWrong>nWrong[d2] || optNWrong==nWrong[d2] && optNConflActs>conflActs[d2].count())
										 || level>0 && optNConflActs>conflActs[d2].count()){
#endif
/*#if 1&0
										if(optNWrong>nWrong[d2]
										 || optNWrong==nWrong[d2] && minWrong[d2]<optMinWrong
										 || optNWrong==nWrong[d2] && minWrong[d2]==optMinWrong && optNConflActs>conflActs[d2].count()){
#endif*/
/*#if 0
										if(minWrong[d2]<optMinWrong
										 || optNWrong>nWrong[d2] && minWrong[d2]==optMinWrong
										 || optNWrong==nWrong[d2] && minWrong[d2]==optMinWrong && optNConflActs>conflActs[d2].count()){
#endif*/
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
											assert(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[isg][d2][h2]);
											break;
										}
									}
									bool okswapped=true;
									for(; h2>=0; h2--) if(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[isg][d2][h2]) {
										int ttt=subgroupsTimetable[isg][d2][h2];
										assert(ttt!=ai);
										if(ttt>=0){
											if(swappedActivities[ttt])
												okswapped=false;
											if(conflActs[d2].indexOf(ttt)==-1 && conflActivities[newtime].indexOf(ttt)==-1)
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
/*#if 1
										if(optNConflActs>conflActs[d2].count()){
#endif*/
/*#if 1
										if(optNWrong>nWrong[d2]
										 || optNWrong==nWrong[d2] && optNConflActs>conflActs[d2].count()){
#endif*/
#if 1
										if(level==0 && (optNWrong>nWrong[d2] || optNWrong==nWrong[d2] && optNConflActs>conflActs[d2].count())
										 || level>0 && optNConflActs>conflActs[d2].count()){
#endif
/*#if 1&0
										if(optNWrong>nWrong[d2]
										 || optNWrong==nWrong[d2] && minWrong[d2]<optMinWrong
										 || optNWrong==nWrong[d2] && minWrong[d2]==optMinWrong && optNConflActs>conflActs[d2].count()){
#endif*/
/*#if 0
										if(minWrong[d2]<optMinWrong
										 || optNWrong>nWrong[d2] && minWrong[d2]==optMinWrong
										 || optNWrong==nWrong[d2] && minWrong[d2]==optMinWrong && optNConflActs>conflActs[d2].count()){
#endif*/
											optNWrong=nWrong[d2];
											optMinWrong=minWrong[d2];
											optNConflActs=conflActs[d2].count();
										}
									}
								}
							}
		
							if(optNWrong==INF || optNWrong==-1){
								if(level==0)
									cout<<"WARNING - unlikely case, optimizetime.cpp line 3070"<<endl;
								break; //impossible slot
							}
							assert(optNWrong<INF);
							assert(optNWrong>=0);

							QList<int> optDays;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
								if(nWrong[d2]>=0)
/*#if 1
									if(optNConflActs==conflActs[d2].count())
#endif*/
/*#if 1
									if(optNWrong==nWrong[d2] && optNConflActs==conflActs[d2].count())
#endif*/
#if 1
									if(level==0 && optNWrong==nWrong[d2] && optNConflActs==conflActs[d2].count()
									 || level>0 && optNConflActs==conflActs[d2].count())
#endif
/*#if 0
									if(optNWrong==nWrong[d2] && minWrong[d2]==optMinWrong && optNConflActs==conflActs[d2].count())
#endif*/
										optDays.append(d2);
							assert(optDays.count()>0);
							int rnd=randomKnuth()%optDays.count();
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
								else
									assert(0);
							}
							//assert(ok);
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
						assert(!breakTime[t] && !subgroupNotAvailableTime[isg][t]);
						break;
					}
				}

				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !subgroupNotAvailableTime[isg][t]){ //break and not available is no gap, no activity
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
						assert(!breakTime[t] && !subgroupNotAvailableTime[isg][t]); //current slot is not break
						break;
					}
				}
	
				for( ; t>=0; t-=gt.rules.nDaysPerWeek) if(!breakTime[t] && !subgroupNotAvailableTime[isg][t]){
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
											assert(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[isg][d2][h2]);
											break;
										}
									}
									bool okswapped=true;
									for(; h2>=0; h2--) if(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[isg][d2][h2]) {
										int ttt=subgroupsTimetable[isg][d2][h2];
										assert(ttt!=ai);
										if(ttt>=0){
											if(swappedActivities[ttt])
												okswapped=false;
											if(conflActs1[d2].indexOf(ttt)==-1 && conflActivities[newtime].indexOf(ttt)==-1)
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
									for(h2--; h2>=0; h2--) if(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[isg][d2][h2])
										if(subgroupsTimetable[isg][d2][h2]>=0){
											okhere=true;
											break;
										}
									if(excess>0 || !okhere || !okswapped){
										nWrong1[d2]=-1;
										minWrong1[d2]=-1;
									}
									else{										
/*#if 1
										if(optNConflActs>conflActs1[d2].count()){
#endif*/
/*#if 1
										if(optNWrong>nWrong1[d2]
										 || optNWrong==nWrong1[d2] && optNConflActs>conflActs1[d2].count()){
#endif*/
#if 1
										if(level==0 && (optNWrong>nWrong1[d2] || optNWrong==nWrong1[d2] && optNConflActs>conflActs1[d2].count())
										 || level>0 && optNConflActs>conflActs1[d2].count()){
#endif
/*#if 1&0
										if(optNWrong>nWrong1[d2]
										 || optNWrong==nWrong1[d2] && minWrong1[d2]<optMinWrong
										 || optNWrong==nWrong1[d2] && minWrong1[d2]==optMinWrong && optNConflActs>conflActs1[d2].count()){
#endif*/
/*#if 0
										if(minWrong1[d2]<optMinWrong
										 || optNWrong>nWrong1[d2] && minWrong1[d2]==optMinWrong
										 || optNWrong==nWrong1[d2] && minWrong1[d2]==optMinWrong && optNConflActs>conflActs1[d2].count()){
#endif*/
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
											assert(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[isg][d2][h2]);
											break;
										}
									}
									bool okswapped=true;
									for(; h2>=0; h2--) if(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[isg][d2][h2]) {
										int ttt=subgroupsTimetable[isg][d2][h2];
										assert(ttt!=ai);
										if(ttt>=0){
											if(swappedActivities[ttt])
												okswapped=false;
											if(conflActs1[d2].indexOf(ttt)==-1 && conflActivities[newtime].indexOf(ttt)==-1)
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
									for(int h3=h2-1; h3>=0; h3--) if(!breakDayHour[d2][h3] && !subgroupNotAvailableDayHour[isg][d2][h3])
										if(subgroupsTimetable[isg][d2][h3]>=0 || h3>=h && h3<h+act->duration){
											okhere=true;
											break;
										}
									if(excess>0 || h2<h+act->duration || !okhere || !okswapped){
										nWrong1[d2]=-1;
										minWrong1[d2]=-1;
									}
									else{										
/*#if 1
										if(optNConflActs>conflActs1[d2].count()){
#endif*/
/*#if 1
										if(optNWrong>nWrong1[d2]
										 || optNWrong==nWrong1[d2] && optNConflActs>conflActs1[d2].count()){
#endif*/
#if 1
										if(level==0 && (optNWrong>nWrong1[d2] || optNWrong==nWrong1[d2] && optNConflActs>conflActs1[d2].count())
										 || level>0 && optNConflActs>conflActs1[d2].count()){
#endif
/*#if 1&0
										if(optNWrong>nWrong1[d2]
										 || optNWrong==nWrong1[d2] && minWrong1[d2]<optMinWrong
										 || optNWrong==nWrong1[d2] && minWrong1[d2]==optMinWrong && optNConflActs>conflActs1[d2].count()){
#endif*/
/*#if 0
										if(minWrong1[d2]<optMinWrong
										 || optNWrong>nWrong1[d2] && minWrong1[d2]==optMinWrong
										 || optNWrong==nWrong1[d2] && minWrong1[d2]==optMinWrong && optNConflActs>conflActs1[d2].count()){
#endif*/
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
											assert(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[isg][d2][h2]);
											break;
										}
									}
									bool okswapped=true;
									for(; h2<gt.rules.nHoursPerDay; h2++) if(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[isg][d2][h2]) {
										int ttt=subgroupsTimetable[isg][d2][h2];
										assert(ttt!=ai);
										if(ttt>=0){
											if(swappedActivities[ttt])
												okswapped=false;
											if(conflActs2[d2].indexOf(ttt)==-1 && conflActivities[newtime].indexOf(ttt)==-1)
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
									for(h2++; h2<gt.rules.nHoursPerDay; h2++) if(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[isg][d2][h2])
										if(subgroupsTimetable[isg][d2][h2]>=0){
											okhere=true;
											break;
										}
									if(excess>0 || !okhere || !okswapped){
										nWrong2[d2]=-1;
										minWrong2[d2]=-1;
									}
									else{										
/*#if 1
										if(optNConflActs>conflActs2[d2].count()){
#endif*/
/*#if 1
										if(optNWrong>nWrong2[d2]
										 || optNWrong==nWrong2[d2] && optNConflActs>conflActs2[d2].count()){
#endif*/
#if 1
										if(level==0 && (optNWrong>nWrong2[d2] || optNWrong==nWrong2[d2] && optNConflActs>conflActs2[d2].count())
										 || level>0 && optNConflActs>conflActs2[d2].count()){
#endif
/*#if 1&0
										if(optNWrong>nWrong2[d2]
										 || optNWrong==nWrong2[d2] && minWrong2[d2]<optMinWrong
										 || optNWrong==nWrong2[d2] && minWrong2[d2]==optMinWrong && optNConflActs>conflActs2[d2].count()){
#endif*/
/*#if 0
										if(minWrong2[d2]<optMinWrong
										 || optNWrong>nWrong2[d2] && minWrong2[d2]==optMinWrong
										 || optNWrong==nWrong2[d2] && minWrong2[d2]==optMinWrong && optNConflActs>conflActs2[d2].count()){
#endif*/
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
											assert(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[isg][d2][h2]);
											break;
										}
									}
									bool okswapped=true;
									for(; h2<gt.rules.nHoursPerDay; h2++) if(!breakDayHour[d2][h2] && !subgroupNotAvailableDayHour[isg][d2][h2]) {
										int ttt=subgroupsTimetable[isg][d2][h2];
										assert(ttt!=ai);
										if(ttt>=0){
											if(swappedActivities[ttt])
												okswapped=false;
											if(conflActs2[d2].indexOf(ttt)==-1 && conflActivities[newtime].indexOf(ttt)==-1)
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
									for(int h3=h2+1; h3<gt.rules.nHoursPerDay; h3++) if(!breakDayHour[d2][h3] && !subgroupNotAvailableDayHour[isg][d2][h3])
										if(subgroupsTimetable[isg][d2][h3]>=0 || h3>=h && h3<h+act->duration){
											okhere=true;
											break;
										}
									if(excess>0 || h2>=h || !okhere || !okswapped){
										nWrong2[d2]=-1;
										minWrong2[d2]=-1;
									}
									else{										
/*#if 1
										if(optNConflActs>conflActs2[d2].count()){
#endif*/
/*#if 1
										if(optNWrong>nWrong2[d2]
										 || optNWrong==nWrong2[d2] && optNConflActs>conflActs2[d2].count()){
#endif*/
#if 1
										if(level==0 && (optNWrong>nWrong2[d2] || optNWrong==nWrong2[d2] && optNConflActs>conflActs2[d2].count())
										 || level>0 && optNConflActs>conflActs2[d2].count()){
#endif
/*#if 1&0
										if(optNWrong>nWrong2[d2]
										 || optNWrong==nWrong2[d2] && minWrong2[d2]<optMinWrong
										 || optNWrong==nWrong2[d2] && minWrong2[d2]==optMinWrong && optNConflActs>conflActs2[d2].count()){
#endif*/
/*#if 0
										if(minWrong2[d2]<optMinWrong
										 || optNWrong>nWrong2[d2] && minWrong2[d2]==optMinWrong
										 || optNWrong==nWrong2[d2] && minWrong2[d2]==optMinWrong && optNConflActs>conflActs2[d2].count()){
#endif*/
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
									/*cout<<"excessHours=="<<excessHours<<endl;
									cout<<"Students set name: "<<qPrintable(gt.rules.internalSubgroupsList[isg]->name)<<endl;
									cout<<"newtime=="<<newtime<<" (d=="<<d<<", h=="<<h<<")"<<endl;
									for(int hh=0; hh<gt.rules.nHoursPerDay; hh++){
										for(int dd=0; dd<gt.rules.nDaysPerWeek; dd++)
											cout<<setw(5)<<subgroupsTimetable[isg][dd][hh];
										cout<<endl;
									}*/
								}
								break; //impossible slot
							}
							assert(optNWrong<INF);
							assert(optNWrong>=0);

							QList<int> optDays;
							QList<int> where; //at the end (1) or at the begin (2)
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
								if(nWrong1[d2]>=0)
/*#if 1
									if(optNConflActs==conflActs1[d2].count()){
#endif*/
/*#if 1
									if(optNWrong==nWrong1[d2] && optNConflActs==conflActs1[d2].count()){
#endif*/
#if 1
									if(level==0 && optNWrong==nWrong1[d2] && optNConflActs==conflActs1[d2].count()
									 || level>0 && optNConflActs==conflActs1[d2].count()){
#endif
/*#if 0
									if(optNWrong==nWrong1[d2] && minWrong1[d2]==optMinWrong && optNConflActs==conflActs1[d2].count()){
#endif*/
										optDays.append(d2);
										where.append(1);
									}
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
								if(nWrong2[d2]>=0)
/*#if 1
									if(optNConflActs==conflActs2[d2].count()){
#endif*/
/*#if 1
									if(optNWrong==nWrong2[d2] && optNConflActs==conflActs2[d2].count()){
#endif*/
#if 1
									if(level==0 && optNWrong==nWrong2[d2] && optNConflActs==conflActs2[d2].count()
									 || level>0 && optNConflActs==conflActs2[d2].count()){
#endif
/*#if 0
									if(optNWrong==nWrong2[d2] && minWrong2[d2]==optMinWrong && optNConflActs==conflActs2[d2].count()){
#endif*/
										optDays.append(d2);
										where.append(2);
									}
							assert(optDays.count()>0);
							int rnd=randomKnuth()%optDays.count();
							int d2=optDays.at(rnd);
							int w=where.at(rnd);
							assert(d2>=0);
							
							bool ok=false; //TODO: remove this variable
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
									else
										assert(0);
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
									else
										assert(0);
								}
							}
							//assert(ok);
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
								if(conflActs[d2].indexOf(a)==-1 && conflActivities[newtime].indexOf(a)==-1){
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
/*#if 1
								if(optNConflActs>conflActs[d2].count()){
#endif*/
/*#if 1
								if(optNWrong>nWrong[d2]
								 || optNWrong==nWrong[d2] && optNConflActs>conflActs[d2].count()){
#endif*/
#if 1
								if(level==0 && (optNWrong>nWrong[d2] || optNWrong==nWrong[d2] && optNConflActs>conflActs[d2].count())
								 || level>0 && optNConflActs>conflActs[d2].count()){
#endif
/*#if 1&0
								if(optNWrong>nWrong[d2]
								 || optNWrong==nWrong[d2] && minWrong[d2]<optMinWrong
								 || optNWrong==nWrong[d2] && minWrong[d2]==optMinWrong && optNConflActs>conflActs[d2].count()){
#endif*/
/*#if 0
								if(minWrong[d2]<optMinWrong
								 || optNWrong>nWrong[d2] && minWrong[d2]==optMinWrong
								 || optNWrong==nWrong[d2] && minWrong[d2]==optMinWrong && optNConflActs>conflActs[d2].count()){
#endif*/
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
/*#if 1
							if(optNConflActs==conflActs[d2].count())
#endif*/
/*#if 1
							if(optNWrong==nWrong[d2] && optNConflActs==conflActs[d2].count())
#endif*/
#if 1
							if(level==0 && optNWrong==nWrong[d2] && optNConflActs==conflActs[d2].count()
							 || level>0 && optNConflActs==conflActs[d2].count())
#endif
/*#if 0
							if(optNWrong==nWrong[d2] && minWrong[d2]==optMinWrong && optNConflActs==conflActs[d2].count())
#endif*/
								optDays.append(d2);
					assert(optDays.count()>0);
					int rnd=randomKnuth()%optDays.count();
					int d2=optDays.at(rnd);
					assert(d2>=0);

					bool ok=false; //TODO: remove this variable
					foreach(int ai2, conflActs[d2]){
						ok=true;
						assert(ai2!=ai);
						assert(!swappedActivities[ai2]);
						if(conflActivities[newtime].indexOf(ai2)==-1){
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						}
						else
							assert(0);
					}
					//assert(ok);
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


		//////////////////rooms
		roomSlots[newtime]=UNALLOCATED_SPACE;
		if(unspecifiedRoom[ai])
			roomSlots[newtime]=UNSPECIFIED_ROOM;
		bool okroomnotavailable=true;
		selectedRoom[newtime]=UNSPECIFIED_ROOM;
		if(!unspecifiedRoom[ai]){
			int optConflActivities=MAX_ACTIVITIES;
	
			//QList<int> conflActivitiesRooms[MAX_ROOMS];
			//int nConflActivitiesRooms[MAX_ROOMS];
			
			QList<QList<int> > conflActivitiesRooms;
			QList<int> nConflActivitiesRooms;
			QList<int> listedRooms;
			
			bool unspecified_room=false;
			
			QList<int> tmp_list;
			int tmp_n_confl_acts;
			
			foreach(int rm, activitiesPreferredRoomsPreferredRooms[ai]){
				//if(unspecified_room)
				//	break;
			
				int dur;
				for(dur=0; dur<act->duration; dur++)
					if(allowedRoomTimePercentages[rm][newtime+dur*gt.rules.nDaysPerWeek]>=0 &&
					 !skipRandom(allowedRoomTimePercentages[rm][newtime+dur*gt.rules.nDaysPerWeek]))
					 	break;
						
				if(dur==act->duration){
					//nConflActivitiesRooms[rm]=0;
					//conflActivitiesRooms[rm].clear();
					tmp_n_confl_acts=0;
					tmp_list.clear();
					
					int dur2;
					bool canSkip=skipRandom(activitiesPreferredRoomsPercentage[ai]);
					for(dur2=0; dur2<act->duration; dur2++){
						int ai2=roomsTimetable[rm][d][h+dur2];
						if(ai2>=0){
							if(conflActivities[newtime].indexOf(ai2)==-1){
								if(canSkip){
									unspecified_room=true;
									break;
								}
								
								if(swappedActivities[ai2]){
									//nConflActivitiesRooms[rm]=MAX_ACTIVITIES;
									tmp_n_confl_acts=MAX_ACTIVITIES;
									break;
								}
								else{
									if(tmp_list.indexOf(ai2)==-1){
										tmp_list.append(ai2);
										tmp_n_confl_acts++;
									}
									/*if(conflActivitiesRooms[rm].indexOf(ai2)==-1){
										conflActivitiesRooms[rm].append(ai2);
										nConflActivitiesRooms[rm]++;
									}*/
								}
							}
						}
					}
					if(dur2==act->duration){
						listedRooms.append(rm);
						nConflActivitiesRooms.append(tmp_n_confl_acts);
						conflActivitiesRooms.append(tmp_list);
						
						/*cout<<"conflActivitiesRooms.count()=="<<conflActivitiesRooms.count()<<endl;
						cout<<"tmp_list.count()=="<<tmp_list.count()<<endl;
						cout<<"conflActivitiesRooms.last.count()=="<<conflActivitiesRooms.at(conflActivitiesRooms.count()-1).count()<<endl;*/

						if(tmp_n_confl_acts<optConflActivities)
							optConflActivities=tmp_n_confl_acts;
					}
				}
				else
					//nConflActivitiesRooms[rm]=MAX_ACTIVITIES;
					tmp_n_confl_acts=MAX_ACTIVITIES;
			}
			
			if(!unspecified_room || optConflActivities==0){
				if(optConflActivities==MAX_ACTIVITIES){
					okroomnotavailable=false;
					goto impossibleroomnotavailable;
				}
				
				assert(optConflActivities<MAX_ACTIVITIES);
	
				QList<int> allowedRoomsIndex;
				/*foreach(int rm, activitiesPreferredRoomsPreferredRooms[ai]){
					if(optConflActivities==nConflActivitiesRooms[rm])
						allowedRooms.append(rm);
				}*/
				
				assert(listedRooms.count()==nConflActivitiesRooms.count());
				assert(listedRooms.count()==conflActivitiesRooms.count());
				
				for(int q=0; q<listedRooms.count(); q++){
					if(nConflActivitiesRooms.at(q)==optConflActivities)
						allowedRoomsIndex.append(q);
				}
					
				assert(allowedRoomsIndex.count()>0);
				int q=randomKnuth()%allowedRoomsIndex.count();
				int t=allowedRoomsIndex.at(q);
				assert(t>=0 && t<listedRooms.count());
				int r=listedRooms.at(t);
				assert(r>=0 && r<gt.rules.nInternalRooms);
				selectedRoom[newtime]=r;
				roomSlots[newtime]=r;
				
				assert(nConflActivitiesRooms.at(t)==conflActivitiesRooms.at(t).count());
				
				//int cnt=0;
				
				//cout<<"q=="<<q<<", t=="<<t<<", r=="<<r<<endl;
				
				//cout<<"Initially, cnt=="<<cnt<<endl;
				foreach(int a, conflActivitiesRooms.at(t)){
					//cnt++;
					//cout<<"cnt=="<<cnt<<" ";
					assert(conflActivities[newtime].indexOf(a)==-1);
					conflActivities[newtime].append(a);
					nConflActivities[newtime]++;
				}
			}
			else
				selectedRoom[newtime]=UNSPECIFIED_ROOM;
		}

impossibleroomnotavailable:
		if(!okroomnotavailable){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		///////////////////////

		
		///////////////////////////////
		//5.0.0-preview28
		//no conflicting activities for this timeslot - place the activity and return
		if(nConflActivities[newtime]==0 && nMinDaysBroken[newtime]==0){
			assert(c.times[ai]==UNALLOCATED_TIME);
			
			assert(conflActivities[newtime].count()==0);
		
			restoreActIndex[nRestore]=ai;
			restoreTime[nRestore]=c.times[ai];
			restoreRoom[nRestore]=c.rooms[ai];
			nRestore++;
			
			//5.0.0-preview25
			assert(swappedActivities[ai]);			
			
			moveActivity(ai, c.times[ai], newtime, c.rooms[ai], selectedRoom[newtime]);
			
			foundGoodSwap=true;
			return;
		}
		///////////////////////////////
		


		nConflActivities[newtime]=conflActivities[newtime].count();
	}
	

	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		conflPerm[perm[i]]=perm[i];
		
	//sorting - O(n^2) - should be improved?
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		for(int j=i+1; j<gt.rules.nHoursPerWeek; j++)
#if 1
			if(nConflActivities[conflPerm[perm[i]]]>nConflActivities[conflPerm[perm[j]]]
			 || nConflActivities[conflPerm[perm[i]]]==nConflActivities[conflPerm[perm[j]]] 
			 && nMinDaysBroken[conflPerm[perm[i]]]>nMinDaysBroken[conflPerm[perm[j]]] ){
#endif
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
			if(nConflActivities[conflPerm[perm[i]]]>0 && nConflActivities[conflPerm[perm[i]]]<MAX_ACTIVITIES
			 && roomSlots[conflPerm[perm[i]]]!=UNALLOCATED_SPACE)
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
			
			//int optMinIndex=-1;
			int optMinIndex=gt.rules.nInternalActivities;
			int optNWrong=INF;
			int optMinWrong=INF;
			int optNConflActs=gt.rules.nInternalActivities;
			int j=-1;
			
			//bool chooseRandom = (randomKnuth()%20 == 0);
			
			foreach(int i, tim){
				//choose a random time out of these with minimum number of wrongly replaced activities
/*#if 1
				if(optNWrong>nWrong[i]
				 || optNWrong==nWrong[i] && optNConflActs>nConflActivities[i]
				 || optNWrong==nWrong[i] && minIndexAct[i]<optMinIndex && optNConflActs==nConflActivities[i]){
#endif*/
#if 1
				if(optMinWrong>minWrong[i]
				 || optMinWrong==minWrong[i] && optNWrong>nWrong[i]
				 || optMinWrong==minWrong[i] && optNWrong==nWrong[i] && optNConflActs>nConflActivities[i]
				 || optMinWrong==minWrong[i] && optNWrong==nWrong[i] && minIndexAct[i]<optMinIndex && optNConflActs==nConflActivities[i]){
#endif
/*#if 1
				if(optNWrong>nWrong[i]){
#endif*/
					optNWrong=nWrong[i];
					optMinWrong=minWrong[i];
					optMinIndex=minIndexAct[i];
					optNConflActs=nConflActivities[i];
					j=i;
				}
			}
			
			assert(j>=0);
			QList<int> tim2;
			foreach(int i, tim)
				if(optNWrong==nWrong[i] && minWrong[i]==optMinWrong && minIndexAct[i]==optMinIndex && optNConflActs==nConflActivities[i])
					tim2.append(i);
			assert(tim2.count()>0);
			int rnd=randomKnuth()%tim2.count();
			j=tim2.at(rnd);

			assert(j>=0);
			timeSlot=j;
			assert(roomSlots[j]!=UNALLOCATED_SPACE);
			roomSlot=roomSlots[j];
			conflActivitiesTimeSlot=conflActivities[timeSlot];
		}
	}

	//int nExplored=0;
	
	for(int i=0; i<gt.rules.nHoursPerWeek; i++){
		int newtime=conflPerm[perm[i]]; //the considered time
		if(nConflActivities[newtime]>=MAX_ACTIVITIES)
			break;
		
		assert(c.times[ai]==UNALLOCATED_TIME);
			
		//no conflicting activities for this timeslot - place the activity and return
		if(nConflActivities[newtime]==0){
			assert(c.times[ai]==UNALLOCATED_TIME);
			
			assert(conflActivities[newtime].count()==0);
		
			restoreActIndex[nRestore]=ai;
			restoreTime[nRestore]=c.times[ai];
			restoreRoom[nRestore]=c.rooms[ai];
			nRestore++;
			
			//5.0.0-preview25
			assert(swappedActivities[ai]);			
			
			moveActivity(ai, c.times[ai], newtime, c.rooms[ai], selectedRoom[newtime]);
			
			foundGoodSwap=true;
			return;
		}
		else{
			/*foreach(int ai2, conflActivities[newtime])
				assert(!swappedActivities[ai2]);*/
				
			//nExplored++;
			
			//if(nExplored>=4)
			//	return;
				
			if(level==level_limit-1){
				//cout<<"level_limit-1==level=="<<level<<", for activity with id "<<gt.rules.internalActivitiesList[ai].id<<" returning"<<endl;
				foundGoodSwap=false;
				break;
			}
			
			if(ncallsrandomswap>=limitcallsrandomswap){
				foundGoodSwap=false;
				break;
			}
		
			//sort activities in decreasing order of difficulty.
			//if the index of the activity in "permutation" is smaller, the act. is more difficult
			/*QList<int> sorted;
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
				
				j=0; //no more considering permutation order
				
				sorted.append(conflActs.at(j));
				int a=conflActs.at(j);
				int t=conflActs.removeAll(a);
				assert(t==1);
			}
			assert(sorted.count()==conflActivities[newtime].count());
			conflActivities[newtime]=sorted;*/
		
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
			QList<int> oldrooms;
			
			if(1 /*ok*/){
				assert(conflActivities[newtime].size()>0);
				
				foreach(int a, conflActivities[newtime]){
					//cout<<"Level=="<<level<<", conflicting act. id=="<<gt.rules.internalActivitiesList[a].id<<", old time=="<<c.times[a]<<endl;
					
					restoreActIndex[nRestore]=a;
					restoreTime[nRestore]=c.times[a];
					restoreRoom[nRestore]=c.rooms[a];
					nRestore++;
					
					oldacts.append(a);
					oldtimes.append(c.times[a]);
					oldrooms.append(c.rooms[a]);
					assert(c.times[a]!=UNALLOCATED_TIME);
					assert(c.rooms[a]!=UNALLOCATED_SPACE);
					//cout<<"level=="<<level<<", unallocating activity with id=="<<gt.rules.internalActivitiesList[a].id<<endl;
					moveActivity(a, c.times[a], UNALLOCATED_TIME, c.rooms[a], UNALLOCATED_SPACE);
					
					//swappedActivities[a]=true;
				}
			}
			assert(oldacts.count()==conflActivities[newtime].count());
			assert(oldtimes.count()==conflActivities[newtime].count());
			assert(oldrooms.count()==conflActivities[newtime].count());
			////////////////
			
			int oldtime=c.times[ai];
			int oldroom=c.rooms[ai];
			//if(c.times[ai]!=UNALLOCATED_TIME){
				restoreActIndex[nRestore]=ai;
				restoreTime[nRestore]=oldtime;
				restoreRoom[nRestore]=oldroom;
				nRestore++;
			//}
			
			//cout<<"Level=="<<level<<", act. id=="<<gt.rules.internalActivitiesList[ai].id<<", old time=="<<c.times[ai]<<endl;

			moveActivity(ai, oldtime, newtime, oldroom, selectedRoom[newtime]);
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
				oldroom=restoreRoom[j];
				
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
				moveActivity(aii, c.times[aii], oldtime, c.rooms[aii], oldroom);
				
				//cout<<"Level=="<<level<<", act. id=="<<gt.rules.internalActivitiesList[ai].id<<", restoring old time=="<<c.times[ai]<<endl;
				
				//assert(oldtime!=UNALLOCATED_TIME);
			}
			nRestore=oldNRestore;

			//////////////////////////////
			foreach(int a, conflActivities[newtime]){
				assert(c.times[a]!=UNALLOCATED_TIME);
				assert(c.rooms[a]!=UNALLOCATED_SPACE);
				assert(!swappedActivities[a]);
			}
			//////////////////////////////
			
			assert(!foundGoodSwap);
			
			if(level>=5) //7 originally
				return;
		}
	}
}
