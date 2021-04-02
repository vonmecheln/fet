Pseudocode of generate.cpp

by Liviu Lalescu, on 26 August 2007

Copyright 2007 Lalescu Liviu

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



There are 3 main functions:

void Generate::generate(int maxSeconds, bool& impossible, bool& timeExceeded)

void Generate::moveActivity(int ai, int fromslot, int toslot, int fromroom, int toroom)

void Generate::randomswap(int ai, int level) 
 Try to place activity with index ai recursively placing conflicting activities at level+1



#include <QList>
#include <QSet>

#include <QSemaphore>

extern QSemaphore semaphorePlacedActivity;
extern QSemaphore finishedSemaphore;

extern Timetable gt;

bool swappedActivities[MAX_ACTIVITIES];

bool foundGoodSwap;

//not sure, it might be necessary 2*... or even more
int restoreActIndex[10*MAX_ACTIVITIES]; //the index of the act. to restore
int restoreTime[10*MAX_ACTIVITIES]; //the time when to restore
int restoreRoom[10*MAX_ACTIVITIES]; //the time when to restore
int nRestore;

int limitcallsrandomswap;

const int MAX_LEVEL=31;

int level_limit;

int ncallsrandomswap;
int maxncallsrandomswap;


//if level==0, choose best position with lowest number
//of conflicting activities
QList<int> conflActivitiesTimeSlot;
int timeSlot;
int roomSlot;


int triedRemovals[MAX_ACTIVITIES][MAX_HOURS_PER_WEEK];  Tabu list

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
	return false with probability weightPercentage or true with probability 100.0-weightPercentage
}

void Generate::generate(int maxSeconds, bool& impossible, bool& timeExceeded)
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
	
	for(int added_act=0; added_act<gt.rules.nInternalActivities; added_act++){
		prevvalue:
	
		if(abortOptimization){
			return;
		}
		time_t crt_time;
		time(&crt_time);		
		searchTime=crt_time-start_time;
		
		if(searchTime>=maxSeconds){
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

		reset variables which keep timetable and recompute it for placed activities
		(from permutation[0] to permutation[added_act-1])
		
		foundGoodSwap=false;
	
		assert(!swappedActivities[permutation[added_act]]);
		swappedActivities[permutation[added_act]]=true;

		nRestore=0;
		//start_search_ticks=clock();
		ncallsrandomswap=0;
		randomswap(permutation[added_act], 0);
		
		if(!foundGoodSwap){
			if(impossibleActivity){
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
			
			We chosed a timeslot which is best,
			 put activity permutation[added_act] into this timeslot,
			 and take all activities which conflict with permutation[added_act]
			 and unallocate them. Array permutation[] is updated: permutation[added_act]
			 is moved towards front and conflicting activities towards the end.
			 After that, we begin trying to place these conflicting activities
			For each confl. act. which becomes unallocated, we place it in the tabu list
			
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

			emit(activityPlaced(q+1));
			semaphorePlacedActivity.acquire();

			goto prevvalue;
//////////////////////
		}			
		else{ //if foundGoodSwap==true
			We could place the activity - no backtracking necessary
		
			nPlacedActivities=added_act+1;
			
			emit(activityPlaced(added_act+1));
			semaphorePlacedActivity.acquire();

			if(added_act==gt.rules.nInternalActivities && foundGoodSwap){   We found solution
				break;
			}
		}
	}

	time_t end_time;
	time(&end_time);
	searchTime=end_time-start_time;
	cout<<"Total searching time (seconds): "<<end_time-start_time<<endl;
	
	emit(simulationFinished());
	
	finishedSemaphore.release();
}

void Generate::moveActivity(int ai, int fromslot, int toslot, int fromroom, int toroom)
{
	Simple to understand here - just move activity with index ai from
	 initial slot (possibly unallocated) and initial room (possibly unallocated)
	 to final slot (possibly unallocated) and final room (possibly unallocated)
	Update timetable variables

	if(fromslot!=UNALLOCATED_TIME){
		int d=fromslot%gt.rules.nDaysPerWeek;
		int h=fromslot/gt.rules.nDaysPerWeek;
		
		.............
	}
	
	c.times[ai]=toslot;
	c.rooms[ai]=toroom;
	c._fitness=-1;
	c.changedForMatrixCalculation=true;
	
	if(toslot!=UNALLOCATED_TIME){
		int d=toslot%gt.rules.nDaysPerWeek;
		int h=toslot/gt.rules.nDaysPerWeek;
		
		.......................
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
	Try to place activity with index ai recursively

	if(level==0){
		Need to find best slot to unallocate in case we cannot place this activity
		 (This slot is used in function generate, when foundGoodSwap==false)
	
		conflActivitiesTimeSlot.clear();
		timeSlot=-1;
	}

	if(level>=level_limit){
		return;
	}
	
	if(ncallsrandomswap>=limitcallsrandomswap)
		return;
		
	ncallsrandomswap++;
	
	Activity* act=&gt.rules.internalActivitiesList[ai];
	
#if 0
	int nMinDaysBroken[MAX_HOURS_PER_WEEK]; //to count for broken min n days between activities constraints
	
	int selectedRoom[MAX_HOURS_PER_WEEK];
#endif
		
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
		
		Find now all conflicting activities for this slot (newtime)
		 If a conflicting activity is already swapped, this slot (newtime) is impossible
		If slot newtime has no min n days constraints broken and zero conflicting activities,
		 place act. at this slot and return.
		Otherwise, sort all slots, firstly the ones with lowest activities conflicting with ai
		 and minimum constraints min n days broken
	}

	if(level==0){
		Choose time slot with lowest number of already tried removals from tabu list (from this slot we will
		 remove activities conflicting with ai), in case of ties choose lowest number of conflicting activities,
		 in case of ties choose timeslot with earliest placed activity, to allow renewal of timetable,
		 so that no restart is necessary
	}

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
			
			assert(swappedActivities[ai]);			
			
			moveActivity(ai, c.times[ai], newtime, c.rooms[ai], selectedRoom[newtime]);
			
			foundGoodSwap=true;
			return;
		}
		else{
			if(level==level_limit-1){
				foundGoodSwap=false;
				break;
			}
			
			if(ncallsrandomswap>=limitcallsrandomswap){
				foundGoodSwap=false;
				break;
			}
		
			int ok=true;
			foreach(int a, conflActivities[newtime]){
				if(swappedActivities[a]){
					assert(0);
					ok=false;
					break;
				}
			}
			
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

			//////////////restore times from the restore list
			for(int j=nRestore-1; j>=oldNRestore; j--){
				int aii=restoreActIndex[j];
				oldtime=restoreTime[j];
				oldroom=restoreRoom[j];
				
				if(aii!=ai){
					swappedActivities[aii]=false;
				}
				else{
					assert(swappedActivities[aii]);
				}
				
				moveActivity(aii, c.times[aii], oldtime, c.rooms[aii], oldroom);
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
			
			if(level>=5)         We do not search all alternatives if level>=5, only the first best
				return;
		}
	}
}
