/*
File generate.cpp
*/

/***************************************************************************
                          generate.cpp  -  description
                             -------------------
    begin                : 2002
    copyright            : (C) 2002 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

//Note 2022-08-16: The code in the constraints of type activity(ies) start students/teachers day and students/teachers max span per real day
//is not perfect, but cannot be written perfectly. It might take care of students/teachers begins mornings/afternoons early max beginnings at second hour,
//but it is very risky and too complicated.

//Note 2022-05-22: In FET version 6.5.0 the code for the constraints students max span per (real) day was improved.
/*Note 2018-07-28: The code for students max span per day, students early max beginnings at second hour, students/teachers max gaps per day/week,
students/teachers min/max hours daily can and should be theoretically corrected. But it is very risky. Many examples and variants should be tested.
See the directory doc/algorithm/2018-07-28-should-improve-theoretically for a better generate file, but which behaves much worse on at least a file,
examples/Romania/Pedagogic-High-School-Tg-Mures/2007-2008_sem1-d-test-students-max-span-per-day.fet (because of the new code in
students max span per day).*/

/*
2020-12-05:
Old comment below, because Qt 6 combined QVector and QList into QList and the resulting class is working at least as fast as a Qt 5 QList,
in many cases ~10% faster or even more.

Note: Since FET-5.44.0 the random number generator changed, and the seed has 2x3=6 components instead of only 2. But probably
the problem below still remains.
Note: TODO item #398
(In fact this is an anti-TODO):

Qt documentation recommends the use of QVector instead of QList. I tried on many files and indeed on many of them it improves the speed with even 10%.
But for some files, among which some from the Economics Faculty of Timisoara, it is much slower, even with 20% slower.

The file examples/Romania/Faculty-Econ-Timisoara-difficult/2007-2008-sem-2/Econ-Timisoara.fet :
	starting with random seed X=1234, Y=1234 (ending with random seed X=2061125487, Y=844641195 for FET-5.42.0 official)
		it takes 18 minutes 51 seconds with QList
		it takes 23 minutes 53 seconds with QVector
The file examples/Romania/Faculty-Econ-Timisoara-difficult/2009-2010-sem-1/Econ-v0.8.fet :
	starting with random seed X=1234, Y=1234 (ending with random seed X=48863282, Y=367996316 for FET-5.42.0 official)
		it takes 11 minutes 52 seconds with QList
		it takes 13 minutes 17 seconds with QVector

So at least for now FET will use QList.*/

//The sorting with the compare function as a class member using lambdas was inspired by this page:
//https://stackoverflow.com/questions/37767847/stdsort-function-with-custom-compare-function-results-error-reference-to-non

//Note about using constBegin() and constEnd() when converting from QSet to QList and viceversa: In Qt >= 5.14.0 and < 6.0.0 it is necessary
//to use 'const' for the global variables which are accessed in more than one thread concurrently. It seems that in Qt 6 this does not matter,
//but it is better to use it. We are using the 'const' variant for the begin() and end() iterators in the whole generate.cpp file, for safety.

#include <ctime>

#include <Qt>
#include <QtGlobal>
#include <QtAlgorithms>

#include <iostream>
#include <algorithm>
#include <cstdlib>

using namespace std;

#include "timetable_defs.h"
#include "timetable.h"
#include "generate.h"
#include "rules.h"

#include "generate_pre.h"

#include "matrix.h"

#include <QList>
#include <QSet>
#include <QHash>
//#include <QQueue>

//#include <condition_variable>

//extern QMutex myMutex; //timetablegenerateform.cpp

/*
#ifndef FET_COMMAND_LINE
extern QSemaphore semaphorePlacedActivity;
extern QSemaphore finishedSemaphore;
#else
QSemaphore semaphorePlacedActivity;
QSemaphore finishedSemaphore;
#endif
*/

//extern MRG32k3a rng;

extern Timetable gt;

const int MAX_LEVEL=14;

const int LEVEL_STOP_CONFLICTS_CALCULATION=MAX_LEVEL;

const int INF=2000000000;

//const int MAX_RETRIES_FOR_AN_ACTIVITY_AT_LEVEL_0=400000;
const int MAX_RETRIES_FOR_AN_ACTIVITY_AT_LEVEL_0=2000000000;

bool Generate::compareConflictsIncreasing(int a, int b)
{
	return nConflictingActivitiesBipartiteMatching[a]<nConflictingActivitiesBipartiteMatching[b];
}

bool Generate::compareConflictsIncreasingAtLevel0(int a, int b)
{
	if(nConflictingActivitiesBipartiteMatching[a]==0 || nConflictingActivitiesBipartiteMatching[b]==0)
		return nConflictingActivitiesBipartiteMatching[a]<nConflictingActivitiesBipartiteMatching[b];
		
	assert(conflictingActivitiesBipartiteMatching[a].count()>0);
	assert(conflictingActivitiesBipartiteMatching[b].count()>0);
	
	int minWrongA=INF;
	int nWrongA=0;
	int minIndexActA=gt.rules.nInternalActivities;
	for(int ai2 : qAsConst(conflictingActivitiesBipartiteMatching[a])){
		minWrongA=min(minWrongA, triedRemovals(ai2, tmpGlobalSolutionCompareLevel0->times[ai2]));
		nWrongA+=triedRemovals(ai2, tmpGlobalSolutionCompareLevel0->times[ai2]);
		minIndexActA=min(minIndexActA, invPermutation[ai2]);
	}
	
	int minWrongB=INF;
	int nWrongB=0;
	int minIndexActB=gt.rules.nInternalActivities;
	for(int ai2 : qAsConst(conflictingActivitiesBipartiteMatching[b])){
		minWrongB=min(minWrongB, triedRemovals(ai2, tmpGlobalSolutionCompareLevel0->times[ai2]));
		nWrongB+=triedRemovals(ai2, tmpGlobalSolutionCompareLevel0->times[ai2]);
		minIndexActB=min(minIndexActB, invPermutation[ai2]);
	}
	
	if(minWrongA!=minWrongB)
		return minWrongA<minWrongB;
		
	if(nWrongA!=nWrongB)
		return nWrongA<nWrongB;
		
	if(nConflictingActivitiesBipartiteMatching[a]!=nConflictingActivitiesBipartiteMatching[b])
		return nConflictingActivitiesBipartiteMatching[a]<nConflictingActivitiesBipartiteMatching[b];
		
	return minIndexActA<minIndexActB;
}

///////////////////////////////////////////////////////////begin Hopcroft-Karp

/*bool breadthFirstSearch()
{
	QQueue<int> q;

	for(int i=0; i<nRealRooms; i++){
		int u=randomPermutation[i];
		
		//if(globalLevel>0)
		//	cout<<"i=="<<i<<", u=="<<u<<", nCA[u]=="<<nConflictingActivities[u]<<" ";
		
		if(pairNode[u]==NIL_NODE){
			dist[u]=0;
			q.enqueue(u);
		}
		else{
			dist[u]=INF_DIST;
		}
	}
	
	//if(globalLevel>0)
	//	cout<<endl<<endl;
	
	dist[NIL_NODE]=INF_DIST;
	while(!q.isEmpty()){
		int u=q.dequeue();
		if(dist[u]<dist[NIL_NODE]){
			for(int v : qAsConst(adj[u])){
				if(dist[pairNode[v]]==INF_DIST){
					dist[pairNode[v]]=dist[u]+1;
					q.enqueue(pairNode[v]);
				}
			}
		}
	}
	
	return dist[NIL_NODE]!=INF_DIST;
}

bool depthFirstSearch(int u)
{
	if(u!=NIL_NODE){
		for(int v : qAsConst(adj[u])){
			if(dist[pairNode[v]]==dist[u]+1){
				if(depthFirstSearch(pairNode[v])==true){
					pairNode[v]=u;
					pairNode[u]=v;
					return true;
				}
			}
		}
		dist[u]=INF_DIST;
		return false;
	}
	
	return true;
}

int hopcroftKarp()
{
	for(int i=0; i<nRealRooms+nSets; i++)
		pairNode[i]=NIL_NODE;
		
	int matching=0;
	
	while(breadthFirstSearch()==true){
		for(int i=0; i<nRealRooms; i++){
			int u=randomPermutation[i];
			if(pairNode[u]==NIL_NODE){
				if(depthFirstSearch(u)==true){
					matching++;
				}
			}
		}
	}
	
	return matching;
}*/

///////////////////////////////////////////////////////////end Hopcroft-Karp

////////////////////////begin maximum bipartite matching by depth first search

bool Generate::depthFirstSearch(int u)
{
	if(u!=NIL_NODE){
		if(visited[u])
			return false;
	
		visited[u]=true;
		for(int v : qAsConst(adj[u])){
			if(depthFirstSearch(pairNode[v])==true){
				pairNode[v]=u;
				pairNode[u]=v; //useless assignment
				return true;
			}
		}
		return false;
	}
	
	return true;
}

int Generate::maximumBipartiteMatching()
{
	for(int i=0; i<nRealRooms+nSets; i++) //i could start from nRealRooms, if we also don't check pairNode[u] below
		pairNode[i]=NIL_NODE;
		
	int matching=0;
	
	for(int i=0; i<nRealRooms; i++){
		int u=semiRandomPermutation[i];
		if(pairNode[u]==NIL_NODE){ //useless test (see comment above)
			for(int j=0; j<nRealRooms; j++)
				visited[j]=false;
			if(depthFirstSearch(u)==true)
				matching++;
		}
		else{
			assert(0);
		}
		if(matching>=nSets)
			break;
	}
	
	return matching;
}

////////////////////////end maximum bipartite matching by depth first search

//For mornings-afternoons
inline bool teacherNoGapsPerAfternoon(int tch)
{
	return teacherConstrainedToZeroGapsPerAfternoon[tch];
}

inline void Generate::addAiToNewTimetable(int ai, const Activity* act, int d, int h)
{
	for(int tch : qAsConst(mustComputeTimetableTeachers[ai])){
		for(int dur=0; dur<act->duration; dur++){
			oldTeachersTimetable(tch,d,h+dur)=newTeachersTimetable(tch,d,h+dur);
			newTeachersTimetable(tch,d,h+dur)=ai;
		}
		oldTeachersDayNHours(tch,d)=newTeachersDayNHours(tch,d);
		oldTeachersDayNGaps(tch,d)=newTeachersDayNGaps(tch,d);
	}

	for(int sbg : qAsConst(mustComputeTimetableSubgroups[ai])){
		for(int dur=0; dur<act->duration; dur++){
			oldSubgroupsTimetable(sbg,d,h+dur)=newSubgroupsTimetable(sbg,d,h+dur);
			newSubgroupsTimetable(sbg,d,h+dur)=ai;
		}
		oldSubgroupsDayNHours(sbg,d)=newSubgroupsDayNHours(sbg,d);
		oldSubgroupsDayNGaps(sbg,d)=newSubgroupsDayNGaps(sbg,d);
		oldSubgroupsDayNFirstGaps(sbg,d)=newSubgroupsDayNFirstGaps(sbg,d);
	}

	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		if(haveTeachersAfternoonsEarly && (d%2)==1)
			for(int tch : qAsConst(mustComputeTimetableTeachers[ai]))
				oldTeachersDayNFirstGaps(tch,d)=newTeachersDayNFirstGaps(tch,d);
		else if(haveTeachersMorningsEarly && (d%2)==0)
			for(int tch : qAsConst(mustComputeTimetableTeachers[ai]))
				oldTeachersDayNFirstGaps(tch,d)=newTeachersDayNFirstGaps(tch,d);

		if(haveTeachersMaxGapsPerRealDay){
			for(int tch : qAsConst(mustComputeTimetableTeachers[ai])){
				oldTeachersRealDayNHours(tch,d/2)=newTeachersRealDayNHours(tch,d/2);
				oldTeachersRealDayNGaps(tch,d/2)=newTeachersRealDayNGaps(tch,d/2);
			}
		}

		if(haveStudentsMaxGapsPerRealDay){
			for(int sbg : qAsConst(mustComputeTimetableSubgroups[ai])){
				oldSubgroupsRealDayNHours(sbg,d/2)=newSubgroupsRealDayNHours(sbg,d/2);
				oldSubgroupsRealDayNGaps(sbg,d/2)=newSubgroupsRealDayNGaps(sbg,d/2);
				oldSubgroupsRealDayNFirstGaps(sbg,d/2)=newSubgroupsRealDayNFirstGaps(sbg,d/2);
			}
		}
	}
}

inline void Generate::removeAiFromNewTimetable(int ai, const Activity* act, int d, int h)
{
	for(int tch : qAsConst(mustComputeTimetableTeachers[ai])){
		for(int dur=0; dur<act->duration; dur++){
			assert(newTeachersTimetable(tch,d,h+dur)==ai);
			newTeachersTimetable(tch,d,h+dur)=oldTeachersTimetable(tch,d,h+dur);
		}
		newTeachersDayNHours(tch,d)=oldTeachersDayNHours(tch,d);
		newTeachersDayNGaps(tch,d)=oldTeachersDayNGaps(tch,d);
	}

	for(int sbg : qAsConst(mustComputeTimetableSubgroups[ai])){
		for(int dur=0; dur<act->duration; dur++){
			assert(newSubgroupsTimetable(sbg,d,h+dur)==ai);
			newSubgroupsTimetable(sbg,d,h+dur)=oldSubgroupsTimetable(sbg,d,h+dur);
		}
		newSubgroupsDayNHours(sbg,d)=oldSubgroupsDayNHours(sbg,d);
		newSubgroupsDayNGaps(sbg,d)=oldSubgroupsDayNGaps(sbg,d);
		newSubgroupsDayNFirstGaps(sbg,d)=oldSubgroupsDayNFirstGaps(sbg,d);
	}

	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		if(haveTeachersAfternoonsEarly && (d%2)==1)
			for(int tch : qAsConst(mustComputeTimetableTeachers[ai]))
				newTeachersDayNFirstGaps(tch,d)=oldTeachersDayNFirstGaps(tch,d);
		else if(haveTeachersMorningsEarly && (d%2)==0)
			for(int tch : qAsConst(mustComputeTimetableTeachers[ai]))
				newTeachersDayNFirstGaps(tch,d)=oldTeachersDayNFirstGaps(tch,d);

		if(haveTeachersMaxGapsPerRealDay){
			for(int tch : qAsConst(mustComputeTimetableTeachers[ai])){
				newTeachersRealDayNHours(tch,d/2)=oldTeachersRealDayNHours(tch,d/2);
				newTeachersRealDayNGaps(tch,d/2)=oldTeachersRealDayNGaps(tch,d/2);
			}
		}

		if(haveStudentsMaxGapsPerRealDay){
			for(int sbg : qAsConst(mustComputeTimetableSubgroups[ai])){
				newSubgroupsRealDayNHours(sbg,d/2)=oldSubgroupsRealDayNHours(sbg,d/2);
				newSubgroupsRealDayNGaps(sbg,d/2)=oldSubgroupsRealDayNGaps(sbg,d/2);
				newSubgroupsRealDayNFirstGaps(sbg,d/2)=oldSubgroupsRealDayNFirstGaps(sbg,d/2);
			}
		}
	}
}

inline void Generate::removeAi2FromTchTimetable(int ai2)
{
	Activity* act2=&gt.rules.internalActivitiesList[ai2];
	int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
	int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
	
	for(int dur2=0; dur2<act2->duration; dur2++){
		assert(tchTimetable(d2,h2+dur2)==ai2);
		if(tchTimetable(d2,h2+dur2)==ai2)
			tchTimetable(d2,h2+dur2)=-1;
	}
}

inline void Generate::removeAi2FromSbgTimetable(int ai2)
{
	Activity* act2=&gt.rules.internalActivitiesList[ai2];
	int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
	int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
	
	for(int dur2=0; dur2<act2->duration; dur2++){
		assert(sbgTimetable(d2,h2+dur2)==ai2);
		if(sbgTimetable(d2,h2+dur2)==ai2)
			sbgTimetable(d2,h2+dur2)=-1;
	}
}

inline void Generate::getTchTimetable(int tch, const QList<int>& conflActivities)
{
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			int ai2=newTeachersTimetable(tch,d2,h2);
			if(ai2>=0 && !conflActivities.contains(ai2))
				tchTimetable(d2,h2)=ai2;
			else
				tchTimetable(d2,h2)=-1;
		}
		
	/*for(int dur=0; dur<act->duration; dur++){
		assert(tchTimetable(d,h+dur)==-1);
		tchTimetable(d,h+dur)=ai;
	}*/
}

inline void Generate::getSbgTimetable(int sbg, const QList<int>& conflActivities)
{
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			int ai2=newSubgroupsTimetable(sbg,d2,h2);
			if(ai2>=0 && !conflActivities.contains(ai2))
				sbgTimetable(d2,h2)=ai2;
			else
				sbgTimetable(d2,h2)=-1;
		}
		
	/*for(int dur=0; dur<act->duration; dur++){
		assert(sbgTimetable(d,h+dur)==-1);
		sbgTimetable(d,h+dur)=ai;
	}*/
}

inline void Generate::updateTchNHoursGaps(int tch, int d)
{
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		int hours=0, gaps=0;

		int h;
		for(h=0; h<gt.rules.nHoursPerDay; h++)
			if(tchTimetable(d,h)>=0)
				break;
		int ng=0;
		for(; h<gt.rules.nHoursPerDay; h++){
			if(tchTimetable(d,h)>=0){
				hours++;
				gaps+=ng;
				ng=0;
			}
			else if(!breakDayHour(d,h) && !teacherNotAvailableDayHour(tch,d,h))
				ng++;
		}
		tchDayNGaps[d]=gaps;
		tchDayNHours[d]=hours;
	}
	else{
		int hours=0, gaps=0, nfirstgaps=0;

		int h;
		for(h=0; h<gt.rules.nHoursPerDay; h++){
			if(tchTimetable(d,h)>=0)
				break;
			else if(!breakDayHour(d,h) && !teacherNotAvailableDayHour(tch,d,h))
				nfirstgaps++;
		}
		int ng=0;
		for(; h<gt.rules.nHoursPerDay; h++){
			if(tchTimetable(d,h)>=0){
				hours++;
				gaps+=ng;
				ng=0;
			}
			else if(!breakDayHour(d,h) && !teacherNotAvailableDayHour(tch,d,h))
				ng++;
		}
		tchDayNGaps[d]=gaps;
		tchDayNHours[d]=hours;

		if(haveTeachersAfternoonsEarly && (d%2)==1){
			if(hours>0){
				tchDayNFirstGaps[d]=nfirstgaps;
			}
			else{
				tchDayNFirstGaps[d]=0;
			}
		}
		else if(haveTeachersMorningsEarly && (d%2)==0){
			if(hours>0){
				tchDayNFirstGaps[d]=nfirstgaps;
			}
			else{
				tchDayNFirstGaps[d]=0;
			}
		}
	}
}

inline void Generate::updateTchNHoursGapsRealDay(int tch, int real_d)
{
	assert(gt.rules.mode==MORNINGS_AFTERNOONS);

	int hours=0, gaps=0;

	int h;
	int d;
	int double_h;
	for(double_h=0; double_h<2*gt.rules.nHoursPerDay; double_h++){
		if(double_h<gt.rules.nHoursPerDay)
			d=real_d*2;
		else
			d=real_d*2+1;
		h=double_h%gt.rules.nHoursPerDay;
		if(tchTimetable(d,h)>=0)
			break;
	}
	int ng=0;
	for(; double_h<2*gt.rules.nHoursPerDay; double_h++){
		if(double_h<gt.rules.nHoursPerDay)
			d=real_d*2;
		else
			d=real_d*2+1;
		h=double_h%gt.rules.nHoursPerDay;
		if(tchTimetable(d,h)>=0){
			hours++;
			gaps+=ng;
			ng=0;
		}
		else if(!breakDayHour(d,h) && !teacherNotAvailableDayHour(tch,d,h))
			ng++;
	}
	tchRealDayNGaps[real_d]=gaps;
	tchRealDayNHours[real_d]=hours;
}

inline void Generate::updateSbgNHoursGaps(int sbg, int d)
{
	int hours=0, gaps=0, nfirstgaps=0;

	int h;
	for(h=0; h<gt.rules.nHoursPerDay; h++){
		if(sbgTimetable(d,h)>=0)
			break;
		else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
			nfirstgaps++;
	}
	int ng=0;
	for(; h<gt.rules.nHoursPerDay; h++){
		if(sbgTimetable(d,h)>=0){
			hours++;
			gaps+=ng;
			ng=0;
		}
		else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
			ng++;
	}
	sbgDayNGaps[d]=gaps;
	sbgDayNHours[d]=hours;
	if(sbgDayNHours[d]>0)
		sbgDayNFirstGaps[d]=nfirstgaps;
	else
		sbgDayNFirstGaps[d]=0;
}

inline void Generate::updateSbgNHoursGapsRealDay(int sbg, int real_d)
{
	assert(gt.rules.mode==MORNINGS_AFTERNOONS);

	int hours=0, gaps=0, nfirstgaps=0;
	int hours_first_half=0;
	int nfirstgaps_first_half=0;

	int h;
	int d;
	int double_h;
	for(double_h=0; double_h<2*gt.rules.nHoursPerDay; double_h++){
		if(double_h<gt.rules.nHoursPerDay)
			d=real_d*2;
		else
			d=real_d*2+1;
		h=double_h%gt.rules.nHoursPerDay;
		if(sbgTimetable(d,h)>=0)
			break;
		else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h)){
			nfirstgaps++;
			if(d%2==0)
				nfirstgaps_first_half++;
		}
	}
	int ng=0;
	for(; double_h<2*gt.rules.nHoursPerDay; double_h++){
		if(double_h<gt.rules.nHoursPerDay)
			d=real_d*2;
		else
			d=real_d*2+1;
		h=double_h%gt.rules.nHoursPerDay;
		if(sbgTimetable(d,h)>=0){
			hours++;
			if(d%2==0)
				hours_first_half++;
			gaps+=ng;
			ng=0;
		}
		else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
			ng++;
	}
	sbgRealDayNGaps[real_d]=gaps;
	sbgRealDayNHours[real_d]=hours;
	if(hours_first_half>0)
		sbgRealDayNFirstGaps[real_d]=nfirstgaps;
	else if(hours>0)
		sbgRealDayNFirstGaps[real_d]=nfirstgaps-nfirstgaps_first_half;
	else
		sbgRealDayNFirstGaps[real_d]=0;
}

inline void Generate::updateTeachersNHoursGaps(int ai, int d)
{
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		for(int tch : qAsConst(mustComputeTimetableTeachers[ai])){
			int hours=0, gaps=0;

			int h;
			for(h=0; h<gt.rules.nHoursPerDay; h++)
				if(newTeachersTimetable(tch,d,h)>=0)
					break;
			int ng=0;
			for(; h<gt.rules.nHoursPerDay; h++){
				if(newTeachersTimetable(tch,d,h)>=0){
					hours++;
					gaps+=ng;
					ng=0;
				}
				else if(!breakDayHour(d,h) && !teacherNotAvailableDayHour(tch,d,h))
					ng++;
			}
			newTeachersDayNGaps(tch,d)=gaps;
			newTeachersDayNHours(tch,d)=hours;
		}
	}
	else{
		for(int tch : qAsConst(mustComputeTimetableTeachers[ai])){
			int hours=0, gaps=0, nfirstgaps=0;

			int h;
			for(h=0; h<gt.rules.nHoursPerDay; h++){
				if(newTeachersTimetable(tch,d,h)>=0)
					break;
				else if(!breakDayHour(d,h) && !teacherNotAvailableDayHour(tch,d,h))
					nfirstgaps++;
			}
			int ng=0;
			for(; h<gt.rules.nHoursPerDay; h++){
				if(newTeachersTimetable(tch,d,h)>=0){
					hours++;
					gaps+=ng;
					ng=0;
				}
				else if(!breakDayHour(d,h) && !teacherNotAvailableDayHour(tch,d,h))
					ng++;
			}
			newTeachersDayNGaps(tch,d)=gaps;
			newTeachersDayNHours(tch,d)=hours;

			if(haveTeachersAfternoonsEarly && (d%2)==1){
				if(hours>0){
					newTeachersDayNFirstGaps(tch,d)=nfirstgaps;
				}
				else{
					newTeachersDayNFirstGaps(tch,d)=0;
				}
			}
			else if(haveTeachersMorningsEarly && (d%2)==0){
				if(hours>0){
					newTeachersDayNFirstGaps(tch,d)=nfirstgaps;
				}
				else{
					newTeachersDayNFirstGaps(tch,d)=0;
				}
			}
		}
	}
}

inline void Generate::updateTeachersNHoursGapsRealDay(int ai, int real_d)
{
	assert(gt.rules.mode==MORNINGS_AFTERNOONS);

	for(int tch : qAsConst(mustComputeTimetableTeachers[ai])){
		int hours=0, gaps=0;

		int h;
		int d;
		int double_h;
		for(double_h=0; double_h<2*gt.rules.nHoursPerDay; double_h++){
			if(double_h<gt.rules.nHoursPerDay)
				d=real_d*2;
			else
				d=real_d*2+1;
			h=double_h%gt.rules.nHoursPerDay;
			if(newTeachersTimetable(tch,d,h)>=0)
				break;
		}
		int ng=0;
		for(; double_h<2*gt.rules.nHoursPerDay; double_h++){
			if(double_h<gt.rules.nHoursPerDay)
				d=real_d*2;
			else
				d=real_d*2+1;
			h=double_h%gt.rules.nHoursPerDay;
			if(newTeachersTimetable(tch,d,h)>=0){
				hours++;
				gaps+=ng;
				ng=0;
			}
			else if(!breakDayHour(d,h) && !teacherNotAvailableDayHour(tch,d,h))
				ng++;
		}
		newTeachersRealDayNGaps(tch,real_d)=gaps;
		newTeachersRealDayNHours(tch,real_d)=hours;
	}
}

inline void Generate::updateSubgroupsNHoursGaps(int ai, int d)
{
	for(int sbg : qAsConst(mustComputeTimetableSubgroups[ai])){
		int hours=0, gaps=0, nfirstgaps=0;

		int h;
		for(h=0; h<gt.rules.nHoursPerDay; h++){
			if(newSubgroupsTimetable(sbg,d,h)>=0)
				break;
			else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
				nfirstgaps++;
		}
		int ng=0;
		for(; h<gt.rules.nHoursPerDay; h++){
			if(newSubgroupsTimetable(sbg,d,h)>=0){
				hours++;
				gaps+=ng;
				ng=0;
			}
			else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
				ng++;
		}
		newSubgroupsDayNGaps(sbg,d)=gaps;
		newSubgroupsDayNHours(sbg,d)=hours;
		if(hours>0)
			newSubgroupsDayNFirstGaps(sbg,d)=nfirstgaps;
		else
			newSubgroupsDayNFirstGaps(sbg,d)=0;
	}
}

inline void Generate::updateSubgroupsNHoursGapsRealDay(int ai, int real_d)
{
	assert(gt.rules.mode==MORNINGS_AFTERNOONS);

	for(int sbg : qAsConst(mustComputeTimetableSubgroups[ai])){
		int hours=0, gaps=0, nfirstgaps=0;
		int hours_first_half=0;
		int nfirstgaps_first_half=0;

		int h;
		int d;
		int double_h;
		for(double_h=0; double_h<2*gt.rules.nHoursPerDay; double_h++){
			if(double_h<gt.rules.nHoursPerDay)
				d=real_d*2;
			else
				d=real_d*2+1;
			h=double_h%gt.rules.nHoursPerDay;
			if(newSubgroupsTimetable(sbg,d,h)>=0)
				break;
			else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h)){
				nfirstgaps++;
				if(d%2==0)
					nfirstgaps_first_half++;
			}
		}
		int ng=0;
		for(; double_h<2*gt.rules.nHoursPerDay; double_h++){
			if(double_h<gt.rules.nHoursPerDay)
				d=real_d*2;
			else
				d=real_d*2+1;
			h=double_h%gt.rules.nHoursPerDay;
			if(newSubgroupsTimetable(sbg,d,h)>=0){
				hours++;
				if(d%2==0)
					hours_first_half++;
				gaps+=ng;
				ng=0;
			}
			else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
				ng++;
		}
		newSubgroupsRealDayNGaps(sbg,real_d)=gaps;
		newSubgroupsRealDayNHours(sbg,real_d)=hours;
		if(hours_first_half>0)
			newSubgroupsRealDayNFirstGaps(sbg,real_d)=nfirstgaps;
		else if(hours>0)
			newSubgroupsRealDayNFirstGaps(sbg,real_d)=nfirstgaps-nfirstgaps_first_half;
		else
			newSubgroupsRealDayNFirstGaps(sbg,real_d)=0;
	}
}

inline void Generate::teacherGetNHoursGaps(int tch)
{
	if(!mustComputeTimetableTeacher[tch])
		return;

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
			newTeachersDayNHours(tch,d2)=0;
			newTeachersDayNGaps(tch,d2)=0;
			//useless code commented on 2021-08-11 (we are not in the MORNINGS_AFTERNOONS mode).
			//if(haveTeachersAfternoonsEarly && d2%2==1)
			//	newTeachersDayNFirstGaps(tch,d2)=0;
		}
		for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
			bool countGaps=false;
			int ng=0;
			for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(newTeachersTimetable(tch,d2,h2)>=0){
					newTeachersDayNHours(tch,d2)++;
					if(countGaps)
						newTeachersDayNGaps(tch,d2)+=ng;
					else
						countGaps=true;
					ng=0;
				}
				else if(!breakDayHour(d2,h2) && !teacherNotAvailableDayHour(tch,d2,h2))
					ng++;
			}
		}
	}
	else{
		for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
			newTeachersDayNHours(tch,d2)=0;
			newTeachersDayNGaps(tch,d2)=0;
			if(haveTeachersAfternoonsEarly && d2%2==1)
				newTeachersDayNFirstGaps(tch,d2)=0;
			else if(haveTeachersMorningsEarly && d2%2==0)
				newTeachersDayNFirstGaps(tch,d2)=0;
		}
		for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
			bool countGaps=false;
			int ng=0;
			int h2;
			int nfirstgaps=0;

			for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(newTeachersTimetable(tch,d2,h2)>=0)
					break;
				else if(!breakDayHour(d2,h2) && !teacherNotAvailableDayHour(tch,d2,h2))
					nfirstgaps++;
			}
			for(; h2<gt.rules.nHoursPerDay; h2++){
				if(newTeachersTimetable(tch,d2,h2)>=0){
					newTeachersDayNHours(tch,d2)++;
					if(countGaps)
						newTeachersDayNGaps(tch,d2)+=ng;
					else
						countGaps=true;
					ng=0;
				}
				else if(!breakDayHour(d2,h2) && !teacherNotAvailableDayHour(tch,d2,h2))
					ng++;
			}

			if(haveTeachersAfternoonsEarly && d2%2==1 && newTeachersDayNHours(tch,d2)>0)
				newTeachersDayNFirstGaps(tch,d2)=nfirstgaps;
			if(haveTeachersMorningsEarly && d2%2==0 && newTeachersDayNHours(tch,d2)>0)
				newTeachersDayNFirstGaps(tch,d2)=nfirstgaps;
		}

		if(haveTeachersMaxGapsPerRealDay){
			for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
				newTeachersRealDayNHours(tch,d2)=0;
				newTeachersRealDayNGaps(tch,d2)=0;
			}
			for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
				bool countGaps=false;
				int ng=0;
				for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
					if(newTeachersTimetable(tch,2*d2,h2)>=0){
						newTeachersRealDayNHours(tch,d2)++;
						if(countGaps)
							newTeachersRealDayNGaps(tch,d2)+=ng;
						else
							countGaps=true;
						ng=0;
					}
					else if(!breakDayHour(2*d2,h2) && !teacherNotAvailableDayHour(tch,2*d2,h2))
						ng++;
				}
				for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
					if(newTeachersTimetable(tch,2*d2+1,h2)>=0){
						newTeachersRealDayNHours(tch,d2)++;
						if(countGaps)
							newTeachersRealDayNGaps(tch,d2)+=ng;
						else
							countGaps=true;
						ng=0;
					}
					else if(!breakDayHour(2*d2+1,h2) && !teacherNotAvailableDayHour(tch,2*d2+1,h2))
						ng++;
				}
			}
		}
	}
}

inline void Generate::tchGetNHoursGaps(int tch)
{
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
			tchDayNHours[d2]=0;
			tchDayNGaps[d2]=0;
		}
		for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
			bool countGaps=false;
			int ng=0;
			for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(tchTimetable(d2,h2)>=0){
					tchDayNHours[d2]++;
					if(countGaps)
						tchDayNGaps[d2]+=ng;
					else
						countGaps=true;
					ng=0;
				}
				else if(!breakDayHour(d2,h2) && !teacherNotAvailableDayHour(tch,d2,h2))
					ng++;
			}
		}
	}
	else{
		for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
			tchDayNHours[d2]=0;
			tchDayNGaps[d2]=0;

			if(haveTeachersAfternoonsEarly && d2%2==1)
				tchDayNFirstGaps[d2]=0;
			else if(haveTeachersMorningsEarly && d2%2==0)
				tchDayNFirstGaps[d2]=0;
		}
		for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
			bool countGaps=false;
			int ng=0;
			int nfirstgaps=0;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(tchTimetable(d2,h2)>=0)
					break;
				else if(!breakDayHour(d2,h2) && !teacherNotAvailableDayHour(tch,d2,h2))
					nfirstgaps++;
			}
			for(; h2<gt.rules.nHoursPerDay; h2++){
				if(tchTimetable(d2,h2)>=0){
					tchDayNHours[d2]++;
					if(countGaps)
						tchDayNGaps[d2]+=ng;
					else
						countGaps=true;
					ng=0;
				}
				else if(!breakDayHour(d2,h2) && !teacherNotAvailableDayHour(tch,d2,h2))
					ng++;
			}

			if(haveTeachersAfternoonsEarly && d2%2==1 && tchDayNHours[d2]>0)
				tchDayNFirstGaps[d2]=nfirstgaps;
			else if(haveTeachersMorningsEarly && d2%2==0 && tchDayNHours[d2]>0)
				tchDayNFirstGaps[d2]=nfirstgaps;
		}
	}
}

inline void Generate::tchGetNHoursGapsRealDays(int tch)
{
	assert(gt.rules.mode==MORNINGS_AFTERNOONS);

	for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
		tchRealDayNHours[d2]=0;
		tchRealDayNGaps[d2]=0;
	}
	for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
		bool countGaps=false;
		int ng=0;
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(tchTimetable(2*d2,h2)>=0){
				tchRealDayNHours[d2]++;
				if(countGaps)
					tchRealDayNGaps[d2]+=ng;
				else
					countGaps=true;
				ng=0;
			}
			else if(!breakDayHour(2*d2,h2) && !teacherNotAvailableDayHour(tch,2*d2,h2))
				ng++;
		}
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(tchTimetable(2*d2+1,h2)>=0){
				tchRealDayNHours[d2]++;
				if(countGaps)
					tchRealDayNGaps[d2]+=ng;
				else
					countGaps=true;
				ng=0;
			}
			else if(!breakDayHour(2*d2+1,h2) && !teacherNotAvailableDayHour(tch,2*d2+1,h2))
				ng++;
		}
	}
}

inline bool Generate::teacherRemoveAnActivityFromBeginOrEnd(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	//Teacher: remove an activity from the beginning or from the end of any day
	QList<int> possibleDays;
	QList<bool> atBeginning;
	QList<int> acts;
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(tchDayNHours[d2]>0){
			int actIndexBegin=-1, actIndexEnd=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(tchTimetable(d2,h2)>=0){
					actIndexBegin=tchTimetable(d2,h2);
					break;
				}
			}
			if(actIndexBegin>=0)
				if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
					actIndexBegin=-1;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
				if(tchTimetable(d2,h2)>=0){
					actIndexEnd=tchTimetable(d2,h2);
					break;
				}
			}
			if(actIndexEnd>=0)
				if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai || actIndexEnd==actIndexBegin)
					actIndexEnd=-1;

			if(actIndexBegin>=0){
				assert(!acts.contains(actIndexBegin));
				possibleDays.append(d2);
				atBeginning.append(true);
				acts.append(actIndexBegin);
			}
			if(actIndexEnd>=0){
				assert(!acts.contains(actIndexEnd));
				possibleDays.append(d2);
				atBeginning.append(false);
				acts.append(actIndexEnd);
			}
		}
	}
	
	bool possibleBeginOrEnd=true;
	if(possibleDays.count()==0)
		possibleBeginOrEnd=false;
	
	if(possibleBeginOrEnd){
		int t;
		
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(possibleDays.count());
		}
		
		assert(t>=0 && t<possibleDays.count());
		
		int d2=possibleDays.at(t);
		bool begin=atBeginning.at(t);
		int ai2=acts.at(t);
		
		removedActivity=ai2;
		
		if(begin){
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(tchTimetable(d2,h2)>=0)
					break;
			assert(h2<gt.rules.nHoursPerDay);
			
			assert(tchTimetable(d2,h2)==ai2);
			
			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}
		else{
			int h2;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
				if(tchTimetable(d2,h2)>=0)
					break;
			assert(h2>=0);
			
			assert(tchTimetable(d2,h2)==ai2);
			
			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}
		
		return true;
	}
	else
		return false;
}

inline bool Generate::teacherRemoveAnActivityFromEnd(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	//Teacher: remove an activity from the end of any day
	QList<int> possibleDays;
	QList<int> acts;
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(tchDayNHours[d2]>0){
			int actIndexEnd=-1;
			int h2;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
				if(tchTimetable(d2,h2)>=0){
					actIndexEnd=tchTimetable(d2,h2);
					break;
				}
			}
			if(actIndexEnd>=0)
				if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai)
					actIndexEnd=-1;

			if(actIndexEnd>=0){
				assert(!acts.contains(actIndexEnd));
				possibleDays.append(d2);
				acts.append(actIndexEnd);
			}
		}
	}
	
	bool possibleEnd=true;
	if(possibleDays.count()==0)
		possibleEnd=false;
	
	if(possibleEnd){
		int t;
		
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(possibleDays.count());
		}
		
		assert(t>=0 && t<possibleDays.count());
		
		int d2=possibleDays.at(t);
		int ai2=acts.at(t);
		
		removedActivity=ai2;
		
		int h2;
		for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
			if(tchTimetable(d2,h2)>=0)
				break;
		assert(h2>=0);
		
		assert(tchTimetable(d2,h2)==ai2);
		
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::teacherRemoveAnActivityFromBeginMorningOrEndAfternoon(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	QList<int> possibleDays;
	QList<bool> atBeginning;
	QList<int> acts;
	for(int real_d=0; real_d<gt.rules.nDaysPerWeek/2; real_d++){
		int d2_morning=real_d*2;
		int d2_afternoon=real_d*2+1;

		if(tchDayNHours[d2_morning]>0){
			int actIndexBegin=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(tchTimetable(d2_morning,h2)>=0){
					actIndexBegin=tchTimetable(d2_morning,h2);
					break;
				}
			}
			if(actIndexBegin>=0)
				if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
					actIndexBegin=-1;

			if(actIndexBegin>=0){
				assert(!acts.contains(actIndexBegin));
				possibleDays.append(d2_morning);
				atBeginning.append(true);
				acts.append(actIndexBegin);
			}
		}

		if(tchDayNHours[d2_afternoon]>0){
			int actIndexEnd=-1;
			int h2;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
				if(tchTimetable(d2_afternoon,h2)>=0){
					actIndexEnd=tchTimetable(d2_afternoon,h2);
					break;
				}
			}
			if(actIndexEnd>=0)
				if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai /* || actIndexEnd==actIndexBegin */)
					actIndexEnd=-1;

			if(actIndexEnd>=0){
				assert(!acts.contains(actIndexEnd));
				possibleDays.append(d2_afternoon);
				atBeginning.append(false);
				acts.append(actIndexEnd);
			}
		}
	}

	bool possibleBeginOrEnd=true;
	if(possibleDays.count()==0)
		possibleBeginOrEnd=false;

	if(possibleBeginOrEnd){
		int t;

		if(level==0){
			int optMinWrong=INF;

			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
					optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}

			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));

			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(possibleDays.count());
		}

		assert(t>=0 && t<possibleDays.count());

		int d2=possibleDays.at(t);
		bool begin=atBeginning.at(t);
		int ai2=acts.at(t);

		removedActivity=ai2;

		if(begin){
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(tchTimetable(d2,h2)>=0)
					break;
			assert(h2<gt.rules.nHoursPerDay);

			assert(tchTimetable(d2,h2)==ai2);

			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}
		else{
			int h2;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
				if(tchTimetable(d2,h2)>=0)
					break;
			assert(h2>=0);

			assert(tchTimetable(d2,h2)==ai2);

			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}

		return true;
	}
	else
		return false;
}

inline bool Generate::teacherRemoveAnActivityFromBeginOrEndCertainRealDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	QList<int> possibleDays;
	QList<bool> atBeginning;
	QList<int> acts;
	int real_d=d2;

	int d2_morning=real_d*2;
	int d2_afternoon=real_d*2+1;

	int actIndexBegin=-1;
	if(tchDayNHours[d2_morning]>0){
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(tchTimetable(d2_morning,h2)>=0){
				actIndexBegin=tchTimetable(d2_morning,h2);
				break;
			}
		}
		if(actIndexBegin>=0)
			if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
				actIndexBegin=-1;

		if(actIndexBegin>=0){
			assert(!acts.contains(actIndexBegin));
			possibleDays.append(d2_morning);
			atBeginning.append(true);
			acts.append(actIndexBegin);
		}
	}
	else if(tchDayNHours[d2_afternoon]>0){
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(tchTimetable(d2_afternoon,h2)>=0){
				actIndexBegin=tchTimetable(d2_afternoon,h2);
				break;
			}
		}
		if(actIndexBegin>=0)
			if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
				actIndexBegin=-1;

		if(actIndexBegin>=0){
			assert(!acts.contains(actIndexBegin));
			possibleDays.append(d2_afternoon);
			atBeginning.append(true);
			acts.append(actIndexBegin);
		}
	}

	if(tchDayNHours[d2_afternoon]>0){
		int actIndexEnd=-1;
		int h2;
		for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
			if(tchTimetable(d2_afternoon,h2)>=0){
				actIndexEnd=tchTimetable(d2_afternoon,h2);
				break;
			}
		}
		if(actIndexEnd>=0)
			if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai || actIndexEnd==actIndexBegin)
				actIndexEnd=-1;

		if(actIndexEnd>=0){
			assert(!acts.contains(actIndexEnd));
			possibleDays.append(d2_afternoon);
			atBeginning.append(false);
			acts.append(actIndexEnd);
		}
	}
	else if(tchDayNHours[d2_morning]>0){
		int actIndexEnd=-1;
		int h2;
		for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
			if(tchTimetable(d2_morning, h2)>=0){
				actIndexEnd=tchTimetable(d2_morning,h2);
				break;
			}
		}
		if(actIndexEnd>=0)
			if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai || actIndexEnd==actIndexBegin)
				actIndexEnd=-1;

		if(actIndexEnd>=0){
			assert(!acts.contains(actIndexEnd));
			possibleDays.append(d2_morning);
			atBeginning.append(false);
			acts.append(actIndexEnd);
		}
	}

	bool possibleBeginOrEnd=true;
	if(possibleDays.count()==0)
		possibleBeginOrEnd=false;

	if(possibleBeginOrEnd){
		int t;

		if(level==0){
			int optMinWrong=INF;

			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
					optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}

			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));

			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(possibleDays.count());
		}

		assert(t>=0 && t<possibleDays.count());

		int d2=possibleDays.at(t);
		bool begin=atBeginning.at(t);
		int ai2=acts.at(t);

		removedActivity=ai2;

		if(begin){
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(tchTimetable(d2,h2)>=0)
					break;
			assert(h2<gt.rules.nHoursPerDay);

			assert(tchTimetable(d2,h2)==ai2);

			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}
		else{
			int h2;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
				if(tchTimetable(d2,h2)>=0)
					break;
			assert(h2>=0);

			assert(tchTimetable(d2,h2)==ai2);

			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}

		return true;
	}
	else
		return false;
}

inline bool Generate::teacherRemoveAnActivityFromBeginOrEndCertainDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	//Teacher: remove an activity from the beginning or from the end of a certain day
	int actIndexBegin=-1, actIndexEnd=-1;
	
	if(tchDayNHours[d2]>0){
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(tchTimetable(d2,h2)>=0){
				actIndexBegin=tchTimetable(d2,h2);
				break;
			}
		}
		if(actIndexBegin>=0)
			if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
				actIndexBegin=-1;
		for(int h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
			if(tchTimetable(d2,h2)>=0){
				actIndexEnd=tchTimetable(d2,h2);
				break;
			}
		}
		if(actIndexEnd>=0)
			if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai || actIndexEnd==actIndexBegin)
				actIndexEnd=-1;
	}
	
	if(actIndexEnd>=0 || actIndexBegin>=0){
		int ai2=-1;
		if(level==0){
			int optMinWrong=INF;

			if(actIndexBegin>=0){
				if(optMinWrong>triedRemovals(actIndexBegin,c.times[actIndexBegin])){
					optMinWrong=triedRemovals(actIndexBegin,c.times[actIndexBegin]);
				}
				ai2=actIndexBegin;
			}

			if(actIndexEnd>=0){
				if(optMinWrong>triedRemovals(actIndexEnd,c.times[actIndexEnd])){
					optMinWrong=triedRemovals(actIndexEnd,c.times[actIndexEnd]);
				}
				ai2=actIndexEnd;
			}
			
			if(actIndexBegin>=0 && actIndexEnd>=0 && optMinWrong==triedRemovals(actIndexEnd,c.times[actIndexEnd]) &&
			  optMinWrong==triedRemovals(actIndexBegin,c.times[actIndexBegin])){
				if(rng.intMRG32k3a(2)==0)
					ai2=actIndexBegin;
				else
					ai2=actIndexEnd;
			}
		}
		else{
			if(actIndexBegin>=0 && actIndexEnd<0)
				ai2=actIndexBegin;
			else if(actIndexEnd>=0 && actIndexBegin<0)
				ai2=actIndexEnd;
			else{
				if(rng.intMRG32k3a(2)==0)
					ai2=actIndexBegin;
				else
					ai2=actIndexEnd;
			}
		}
		assert(ai2>=0);
		
		removedActivity=ai2;
		
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::teacherRemoveAnActivityFromBeginOrEndCertainTwoDays(int d2, int d4, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	int actIndexBegin2=-1, actIndexEnd2=-1;
	int actIndexBegin4=-1, actIndexEnd4=-1;

	if(tchDayNHours[d2]>0){
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(tchTimetable(d2,h2)>=0){
				actIndexBegin2=tchTimetable(d2,h2);
				break;
			}
		}
		if(actIndexBegin2>=0)
			if(fixedTimeActivity[actIndexBegin2] || swappedActivities[actIndexBegin2] || actIndexBegin2==ai)
				actIndexBegin2=-1;
		for(int h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
			if(tchTimetable(d2,h2)>=0){
				actIndexEnd2=tchTimetable(d2,h2);
				break;
			}
		}
		if(actIndexEnd2>=0)
			if(fixedTimeActivity[actIndexEnd2] || swappedActivities[actIndexEnd2] || actIndexEnd2==ai || actIndexEnd2==actIndexBegin2)
				actIndexEnd2=-1;
	}
	if(tchDayNHours[d4]>0){
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(tchTimetable(d4,h2)>=0){
				actIndexBegin4=tchTimetable(d4,h2);
				break;
			}
		}
		if(actIndexBegin4>=0)
			if(fixedTimeActivity[actIndexBegin4] || swappedActivities[actIndexBegin4] || actIndexBegin4==ai)
				actIndexBegin4=-1;
		for(int h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
			if(tchTimetable(d4,h2)>=0){
				actIndexEnd4=tchTimetable(d4,h2);
				break;
			}
		}
		if(actIndexEnd4>=0)
			if(fixedTimeActivity[actIndexEnd4] || swappedActivities[actIndexEnd4] || actIndexEnd4==ai || actIndexEnd4==actIndexBegin4)
				actIndexEnd4=-1;
	}

	if(actIndexEnd2>=0 || actIndexBegin2>=0 || actIndexEnd4>=0 || actIndexBegin4>=0){
		int ai2=-1;
		if(level==0){
			int optMinWrong=INF;

			if(actIndexBegin2>=0){
				if(optMinWrong>triedRemovals(actIndexBegin2,c.times[actIndexBegin2])){
					optMinWrong=triedRemovals(actIndexBegin2,c.times[actIndexBegin2]);
				}
				ai2=actIndexBegin2;
			}

			if(actIndexEnd2>=0){
				if(optMinWrong>triedRemovals(actIndexEnd2,c.times[actIndexEnd2])){
					optMinWrong=triedRemovals(actIndexEnd2,c.times[actIndexEnd2]);
				}
				ai2=actIndexEnd2;
			}

			if(actIndexBegin4>=0){
				if(optMinWrong>triedRemovals(actIndexBegin4,c.times[actIndexBegin4])){
					optMinWrong=triedRemovals(actIndexBegin4,c.times[actIndexBegin4]);
				}
				ai2=actIndexBegin4;
			}

			if(actIndexEnd4>=0){
				if(optMinWrong>triedRemovals(actIndexEnd4,c.times[actIndexEnd4])){
					optMinWrong=triedRemovals(actIndexEnd4,c.times[actIndexEnd4]);
				}
				ai2=actIndexEnd4;
			}

			QList<int> tl;

			if(actIndexBegin2>=0)
				if(optMinWrong==triedRemovals(actIndexBegin2,c.times[actIndexBegin2]))
					tl.append(0);
			if(actIndexEnd2>=0)
				if(optMinWrong==triedRemovals(actIndexEnd2,c.times[actIndexEnd2]))
					tl.append(1);
			if(actIndexBegin4>=0)
				if(optMinWrong==triedRemovals(actIndexBegin4,c.times[actIndexBegin4]))
					tl.append(2);
			if(actIndexEnd4>=0)
				if(optMinWrong==triedRemovals(actIndexEnd4,c.times[actIndexEnd4]))
					tl.append(3);

			int ii=tl.at(rng.intMRG32k3a(tl.count()));

			if(ii==0)
				ai2=actIndexBegin2;
			else if(ii==1)
				ai2=actIndexEnd2;
			else if(ii==2)
				ai2=actIndexBegin4;
			else if(ii==3)
				ai2=actIndexEnd4;

			/*
			if(actIndexBegin>=0 && actIndexEnd>=0 && optMinWrong==triedRemovals[actIndexEnd][c.times[actIndexEnd]] &&
			  optMinWrong==triedRemovals[actIndexBegin][c.times[actIndexBegin]]){
				if(rng.intMRG32k3a()%2==0)
					ai2=actIndexBegin;
				else
					ai2=actIndexEnd;
			}*/
		}
		else{
			QList<int> tl;

			if(actIndexBegin2>=0)
				tl.append(0);
			if(actIndexEnd2>=0)
				tl.append(1);
			if(actIndexBegin4>=0)
				tl.append(2);
			if(actIndexEnd4>=0)
				tl.append(3);

			int ii=tl.at(rng.intMRG32k3a(tl.count()));

			if(ii==0)
				ai2=actIndexBegin2;
			else if(ii==1)
				ai2=actIndexEnd2;
			else if(ii==2)
				ai2=actIndexBegin4;
			else if(ii==3)
				ai2=actIndexEnd4;
/*

			if(actIndexBegin>=0 && actIndexEnd<0)
				ai2=actIndexBegin;
			else if(actIndexEnd>=0 && actIndexBegin<0)
				ai2=actIndexEnd;
			else{
				if(rng.intMRG32k3a()%2==0)
					ai2=actIndexBegin;
				else
					ai2=actIndexEnd;
			}*/
		}
		assert(ai2>=0);

		removedActivity=ai2;

		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());

		return true;
	}
	else
		return false;
}

inline bool Generate::teacherRemoveAnActivityFromAnywhere(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	//Teacher: remove an activity from anywhere
	QList<int> acts;
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(tchDayNHours[d2]>0){
			int actIndex=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(tchTimetable(d2,h2)>=0){
					actIndex=tchTimetable(d2,h2);
					
					if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex))
						actIndex=-1;

					if(actIndex>=0){
						assert(!acts.contains(actIndex));
						acts.append(actIndex);
					}
				}
		}
	}
	
	if(acts.count()>0){
		int t;
		
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(acts.count());
		}
		
		int ai2=acts.at(t);
		
		removedActivity=ai2;
		
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::teacherRemoveAnActivityFromAnywhereCertainDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	//Teacher: remove an activity from anywhere certain day
	QList<int> acts;
	if(tchDayNHours[d2]>0){
		int actIndex=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(tchTimetable(d2,h2)>=0){
				actIndex=tchTimetable(d2,h2);
				
				if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex))
					actIndex=-1;

				if(actIndex>=0){
					assert(!acts.contains(actIndex));
					acts.append(actIndex);
				}
			}
	}
	
	if(acts.count()>0){
		int t;
		
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(acts.count());
		}
		
		int ai2=acts.at(t);
		
		removedActivity=ai2;
		
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::teacherRemoveAnActivityFromAnywhereCertainDayCertainActivityTag(int d2, int actTag, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	//Teacher: remove an activity from anywhere certain day certain activity tag
	QList<int> acts;
	if(tchDayNHours[d2]>0){
		int actIndex=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(tchTimetable(d2,h2)>=0){
				actIndex=tchTimetable(d2,h2);
				
				if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex) || !gt.rules.internalActivitiesList[actIndex].iActivityTagsSet.contains(actTag))
					actIndex=-1;

				if(actIndex>=0){
					assert(!acts.contains(actIndex));
					acts.append(actIndex);
				}
			}
	}
	
	if(acts.count()>0){
		int t;
		
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
					optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(acts.count());
		}
		
		int ai2=acts.at(t);
		
		removedActivity=ai2;
		
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::teacherRemoveAnActivityFromAnywhereCertainDayDayPairCertainActivityTag(int d2, int dpair2, int actTag, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	QList<int> acts;
	if(tchDayNHours[d2]>0){
		int actIndex=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(tchTimetable(d2,h2)>=0){
				actIndex=tchTimetable(d2,h2);

				if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex) || !gt.rules.internalActivitiesList[actIndex].iActivityTagsSet.contains(actTag))
					actIndex=-1;

				if(actIndex>=0){
					assert(!acts.contains(actIndex));
					acts.append(actIndex);
				}
			}
	}
	if(tchDayNHours[dpair2]>0){
		int actIndex=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(tchTimetable(dpair2,h2)>=0){
				actIndex=tchTimetable(dpair2,h2);

				assert(actIndex!=ai);

				if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || acts.contains(actIndex) || !gt.rules.internalActivitiesList[actIndex].iActivityTagsSet.contains(actTag))
					actIndex=-1;

				if(actIndex>=0){
					assert(!acts.contains(actIndex));
					acts.append(actIndex);
				}
			}
	}

	if(acts.count()>0){
		int t;

		if(level==0){
			int optMinWrong=INF;

			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
					optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}

			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));

			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(acts.count());
		}

		int ai2=acts.at(t);

		removedActivity=ai2;

		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());

		return true;
	}
	else
		return false;
}

inline bool Generate::teacherRemoveAnActivityFromAnywhereCertainTwoDays(int d2, int d4, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	QList<int> acts;
	if(tchDayNHours[d2]>0){
		int actIndex=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(tchTimetable(d2,h2)>=0){
				actIndex=tchTimetable(d2,h2);

				if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex))
					actIndex=-1;

				if(actIndex>=0){
					assert(!acts.contains(actIndex));
					acts.append(actIndex);
				}
			}
	}

	if(tchDayNHours[d4]>0){
		int actIndex=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(tchTimetable(d4,h2)>=0){
				actIndex=tchTimetable(d4,h2);

				if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex))
					actIndex=-1;

				if(actIndex>=0){
					assert(!acts.contains(actIndex));
					acts.append(actIndex);
				}
			}
	}

	if(acts.count()>0){
		int t;

		if(level==0){
			int optMinWrong=INF;

			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
					optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}

			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));

			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(acts.count());
		}

		int ai2=acts.at(t);

		removedActivity=ai2;

		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());

		return true;
	}
	else
		return false;
}

//students
inline void Generate::subgroupGetNHoursGaps(int sbg)
{
	if(!mustComputeTimetableSubgroup[sbg])
		return;

	for(int d=0; d<gt.rules.nDaysPerWeek; d++){
		int hours=0, gaps=0, nfirstgaps=0;

		int h;
		for(h=0; h<gt.rules.nHoursPerDay; h++){
			if(newSubgroupsTimetable(sbg,d,h)>=0)
				break;
			else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
				nfirstgaps++;
		}
		int ng=0;
		for(; h<gt.rules.nHoursPerDay; h++){
			if(newSubgroupsTimetable(sbg,d,h)>=0){
				hours++;
				gaps+=ng;
				ng=0;
			}
			else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
				ng++;
		}
		newSubgroupsDayNGaps(sbg,d)=gaps;
		newSubgroupsDayNHours(sbg,d)=hours;
		if(hours>0)
			newSubgroupsDayNFirstGaps(sbg,d)=nfirstgaps;
		else
			newSubgroupsDayNFirstGaps(sbg,d)=0;
	}

/*	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		newSubgroupsDayNHours(sbg,d2)=0;
		newSubgroupsDayNGaps(sbg,d2)=0;
	}
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		bool countGaps=false;
		int ng=0;
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(newSubgroupsTimetable(sbg,d2,h2)>=0){
				newSubgroupsDayNHours(sbg,d2)++;
				if(countGaps)
					newSubgroupsDayNGaps(sbg,d2)+=ng;
				else
					countGaps=true;
				ng=0;
			}
			else if(!breakDayHour(d2,h2) && !subgroupNotAvailableDayHour(sbg,d2,h2))
				ng++;
		}
	}*/

	if(gt.rules.mode==MORNINGS_AFTERNOONS && haveStudentsMaxGapsPerRealDay){
		for(int real_d=0; real_d<gt.rules.nDaysPerWeek/2; real_d++){
			int hours=0, gaps=0, nfirstgaps=0;
			int hours_first_half=0;
			int nfirstgaps_first_half=0;

			int h;
			int d;
			int double_h;
			assert(gt.rules.nHoursPerDay>0);
			for(double_h=0; double_h<2*gt.rules.nHoursPerDay; double_h++){
				if(double_h<gt.rules.nHoursPerDay)
					d=real_d*2;
				else
					d=real_d*2+1;
				h=double_h%gt.rules.nHoursPerDay;
				if(newSubgroupsTimetable(sbg,d,h)>=0)
					break;
				else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h)){
					nfirstgaps++;
					if(d%2==0)
						nfirstgaps_first_half++;
				}
			}
			int ng=0;
			for(; double_h<2*gt.rules.nHoursPerDay; double_h++){
				if(double_h<gt.rules.nHoursPerDay)
					d=real_d*2;
				else
					d=real_d*2+1;
				h=double_h%gt.rules.nHoursPerDay;
				if(newSubgroupsTimetable(sbg,d,h)>=0){
					hours++;
					if(d%2==0)
						hours_first_half++;
					gaps+=ng;
					ng=0;
				}
				else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
					ng++;
			}
			newSubgroupsRealDayNGaps(sbg,real_d)=gaps;
			newSubgroupsRealDayNHours(sbg,real_d)=hours;
			if(hours_first_half>0)
				newSubgroupsRealDayNFirstGaps(sbg,real_d)=nfirstgaps;
			else if(hours>0)
				newSubgroupsRealDayNFirstGaps(sbg,real_d)=nfirstgaps-nfirstgaps_first_half;
			else
				newSubgroupsRealDayNFirstGaps(sbg,real_d)=0;
		}
	}
}

inline void Generate::sbgGetNHoursGaps(int sbg)
{
	for(int d=0; d<gt.rules.nDaysPerWeek; d++){
		int hours=0, gaps=0, nfirstgaps=0;

		int h;
		for(h=0; h<gt.rules.nHoursPerDay; h++){
			if(sbgTimetable(d,h)>=0)
				break;
			else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
				nfirstgaps++;
		}
		int ng=0;
		for(; h<gt.rules.nHoursPerDay; h++){
			if(sbgTimetable(d,h)>=0){
				hours++;
				gaps+=ng;
				ng=0;
			}
			else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
				ng++;
		}
	
		sbgDayNGaps[d]=gaps;
		sbgDayNHours[d]=hours;
		if(sbgDayNHours[d]>0)
			sbgDayNFirstGaps[d]=nfirstgaps;
		else
			sbgDayNFirstGaps[d]=0;
	}
	
	/*
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		sbgDayNHours[d2]=0;
		sbgDayNGaps[d2]=0;
		sbgDayNFirstGaps[d2]=0;
	}
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		bool countGaps=false;
		int ng=0;
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
		if(sbgTimetable(d2,h2)>=0){
				sbgDayNHours[d2]++;
				if(countGaps)
					sbgDayNGaps[d2]+=ng;
				else
					countGaps=true;
				ng=0;
			}
			else if(!breakDayHour(d2,h2) && !subgroupNotAvailableDayHour(sbg,d2,h2))
				ng++;
		}
	}*/
}

inline void Generate::sbgGetNHoursGapsRealDays(int sbg)
{
	assert(gt.rules.mode==MORNINGS_AFTERNOONS);
	for(int real_d=0; real_d<gt.rules.nDaysPerWeek/2; real_d++){
		int hours=0, gaps=0, nfirstgaps=0;
		int hours_first_half=0;
		int nfirstgaps_first_half=0;

		int h;
		int d;
		int double_h;
		assert(gt.rules.nHoursPerDay>0);
		for(double_h=0; double_h<2*gt.rules.nHoursPerDay; double_h++){
			if(double_h<gt.rules.nHoursPerDay)
				d=real_d*2;
			else
				d=real_d*2+1;
			h=double_h%gt.rules.nHoursPerDay;
			if(sbgTimetable(d,h)>=0)
				break;
			else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h)){
				nfirstgaps++;
				if(d%2==0)
					nfirstgaps_first_half++;
			}
		}
		int ng=0;
		for(; double_h<2*gt.rules.nHoursPerDay; double_h++){
			if(double_h<gt.rules.nHoursPerDay)
				d=real_d*2;
			else
				d=real_d*2+1;
			h=double_h%gt.rules.nHoursPerDay;
			if(sbgTimetable(d,h)>=0){
				hours++;
				if(d%2==0)
					hours_first_half++;
				gaps+=ng;
				ng=0;
			}
			else if(!breakDayHour(d,h) && !subgroupNotAvailableDayHour(sbg,d,h))
				ng++;
		}

		sbgRealDayNGaps[real_d]=gaps;
		sbgRealDayNHours[real_d]=hours;
		if(hours_first_half>0)
			sbgRealDayNFirstGaps[real_d]=nfirstgaps;
		else if(hours>0)
			sbgRealDayNFirstGaps[real_d]=nfirstgaps-nfirstgaps_first_half;
		else
			sbgRealDayNFirstGaps[real_d]=0;
	}
}

inline bool Generate::subgroupRemoveAnActivityFromBeginOrEnd(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	//Subgroup: remove an activity from the beginning or from the end of any day
	QList<int> possibleDays;
	QList<bool> atBeginning;
	QList<int> acts;
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(sbgDayNHours[d2]>0){
			int actIndexBegin=-1, actIndexEnd=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(sbgTimetable(d2,h2)>=0){
					actIndexBegin=sbgTimetable(d2,h2);
					break;
				}
			}
			if(actIndexBegin>=0)
				if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
					actIndexBegin=-1;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
				if(sbgTimetable(d2,h2)>=0){
					actIndexEnd=sbgTimetable(d2,h2);
					break;
				}
			}
			if(actIndexEnd>=0)
				if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai || actIndexEnd==actIndexBegin)
					actIndexEnd=-1;

			if(actIndexBegin>=0){
				assert(!acts.contains(actIndexBegin));
				possibleDays.append(d2);
				atBeginning.append(true);
				acts.append(actIndexBegin);
			}
			if(actIndexEnd>=0){
				assert(!acts.contains(actIndexEnd));
				possibleDays.append(d2);
				atBeginning.append(false);
				acts.append(actIndexEnd);
			}
		}
	}
	
	bool possibleBeginOrEnd=true;
	if(possibleDays.count()==0)
		possibleBeginOrEnd=false;
	
	if(possibleBeginOrEnd){
		int t;
		
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(possibleDays.count());
		}
		
		assert(t>=0 && t<possibleDays.count());
		
		int d2=possibleDays.at(t);
		bool begin=atBeginning.at(t);
		int ai2=acts.at(t);
		
		removedActivity=ai2;
		
		if(begin){
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(sbgTimetable(d2,h2)>=0)
					break;
			assert(h2<gt.rules.nHoursPerDay);
		
			assert(sbgTimetable(d2,h2)==ai2);
			
			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}
		else{
			int h2;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
				if(sbgTimetable(d2,h2)>=0)
					break;
			assert(h2>=0);
			
			assert(sbgTimetable(d2,h2)==ai2);
			
			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}
		
		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromBeginMorningOrEndAfternoon(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	QList<int> possibleDays;
	QList<bool> atBeginning;
	QList<int> acts;
	for(int real_d=0; real_d<gt.rules.nDaysPerWeek/2; real_d++){
		int d2_morning=real_d*2;
		int d2_afternoon=real_d*2+1;

		if(sbgDayNHours[d2_morning]>0){
			int actIndexBegin=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(sbgTimetable(d2_morning,h2)>=0){
					actIndexBegin=sbgTimetable(d2_morning,h2);
					break;
				}
			}
			if(actIndexBegin>=0)
				if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
					actIndexBegin=-1;

			if(actIndexBegin>=0){
				assert(!acts.contains(actIndexBegin));
				possibleDays.append(d2_morning);
				atBeginning.append(true);
				acts.append(actIndexBegin);
			}
		}

		if(sbgDayNHours[d2_afternoon]>0){
			int actIndexEnd=-1;
			int h2;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
				if(sbgTimetable(d2_afternoon,h2)>=0){
					actIndexEnd=sbgTimetable(d2_afternoon,h2);
					break;
				}
			}
			if(actIndexEnd>=0)
				if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai /* || actIndexEnd==actIndexBegin */)
					actIndexEnd=-1;

			if(actIndexEnd>=0){
				assert(!acts.contains(actIndexEnd));
				possibleDays.append(d2_afternoon);
				atBeginning.append(false);
				acts.append(actIndexEnd);
			}
		}
	}

	bool possibleBeginOrEnd=true;
	if(possibleDays.count()==0)
		possibleBeginOrEnd=false;

	if(possibleBeginOrEnd){
		int t;

		if(level==0){
			int optMinWrong=INF;

			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
					optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}

			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));

			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(possibleDays.count());
		}

		assert(t>=0 && t<possibleDays.count());

		int d2=possibleDays.at(t);
		bool begin=atBeginning.at(t);
		int ai2=acts.at(t);

		removedActivity=ai2;

		if(begin){
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(sbgTimetable(d2,h2)>=0)
					break;
			assert(h2<gt.rules.nHoursPerDay);

			assert(sbgTimetable(d2,h2)==ai2);

			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}
		else{
			int h2;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
				if(sbgTimetable(d2,h2)>=0)
					break;
			assert(h2>=0);

			assert(sbgTimetable(d2,h2)==ai2);

			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}

		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromBeginOrEndCertainRealDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	QList<int> possibleDays;
	QList<bool> atBeginning;
	QList<int> acts;
	int real_d=d2;

	int d2_morning=real_d*2;
	int d2_afternoon=real_d*2+1;

	int actIndexBegin=-1;
	if(sbgDayNHours[d2_morning]>0){
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(sbgTimetable(d2_morning,h2)>=0){
				actIndexBegin=sbgTimetable(d2_morning,h2);
				break;
			}
		}
		if(actIndexBegin>=0)
			if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
				actIndexBegin=-1;

		if(actIndexBegin>=0){
			assert(!acts.contains(actIndexBegin));
			possibleDays.append(d2_morning);
			atBeginning.append(true);
			acts.append(actIndexBegin);
		}
	}
	else if(sbgDayNHours[d2_afternoon]>0){
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(sbgTimetable(d2_afternoon,h2)>=0){
				actIndexBegin=sbgTimetable(d2_afternoon,h2);
				break;
			}
		}
		if(actIndexBegin>=0)
			if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
				actIndexBegin=-1;

		if(actIndexBegin>=0){
			assert(!acts.contains(actIndexBegin));
			possibleDays.append(d2_afternoon);
			atBeginning.append(true);
			acts.append(actIndexBegin);
		}
	}

	if(sbgDayNHours[d2_afternoon]>0){
		int actIndexEnd=-1;
		int h2;
		for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
			if(sbgTimetable(d2_afternoon,h2)>=0){
				actIndexEnd=sbgTimetable(d2_afternoon,h2);
				break;
			}
		}
		if(actIndexEnd>=0)
			if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai || actIndexEnd==actIndexBegin)
				actIndexEnd=-1;

		if(actIndexEnd>=0){
			assert(!acts.contains(actIndexEnd));
			possibleDays.append(d2_afternoon);
			atBeginning.append(false);
			acts.append(actIndexEnd);
		}
	}
	else if(sbgDayNHours[d2_morning]>0){
		int actIndexEnd=-1;
		int h2;
		for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
			if(sbgTimetable(d2_morning,h2)>=0){
				actIndexEnd=sbgTimetable(d2_morning,h2);
				break;
			}
		}
		if(actIndexEnd>=0)
			if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai || actIndexEnd==actIndexBegin)
				actIndexEnd=-1;

		if(actIndexEnd>=0){
			assert(!acts.contains(actIndexEnd));
			possibleDays.append(d2_morning);
			atBeginning.append(false);
			acts.append(actIndexEnd);
		}
	}

	bool possibleBeginOrEnd=true;
	if(possibleDays.count()==0)
		possibleBeginOrEnd=false;

	if(possibleBeginOrEnd){
		int t;

		if(level==0){
			int optMinWrong=INF;

			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
					optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}

			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));

			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(possibleDays.count());
		}

		assert(t>=0 && t<possibleDays.count());

		int d2=possibleDays.at(t);
		bool begin=atBeginning.at(t);
		int ai2=acts.at(t);

		removedActivity=ai2;

		if(begin){
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(sbgTimetable(d2,h2)>=0)
					break;
			assert(h2<gt.rules.nHoursPerDay);

			assert(sbgTimetable(d2,h2)==ai2);

			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}
		else{
			int h2;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
				if(sbgTimetable(d2,h2)>=0)
					break;
			assert(h2>=0);

			assert(sbgTimetable(d2,h2)==ai2);

			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}

		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromEndCertainRealDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	QList<int> possibleDays;
	QList<bool> atBeginning;
	QList<int> acts;
	int real_d=d2;

	int d2_morning=real_d*2;
	int d2_afternoon=real_d*2+1;

/*
	if(sbgDayNHours[d2_morning]>0){
		int actIndexBegin=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(sbgTimetable(d2_morning,h2)>=0){
				actIndexBegin=sbgTimetable(d2_morning,h2);
				break;
			}
		}
		if(actIndexBegin>=0)
			if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
				actIndexBegin=-1;

		if(actIndexBegin>=0){
			assert(!acts.contains(actIndexBegin));
			possibleDays.append(d2_morning);
			atBeginning.append(true);
			acts.append(actIndexBegin);
		}
	}
*/

	if(sbgDayNHours[d2_afternoon]>0){
		int actIndexEnd=-1;
		int h2;
		for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
			if(sbgTimetable(d2_afternoon,h2)>=0){
				actIndexEnd=sbgTimetable(d2_afternoon,h2);
				break;
			}
		}
		if(actIndexEnd>=0)
			if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai) // || actIndexEnd==actIndexBegin
				actIndexEnd=-1;

		if(actIndexEnd>=0){
			assert(!acts.contains(actIndexEnd));
			possibleDays.append(d2_afternoon);
			atBeginning.append(false);
			acts.append(actIndexEnd);
		}
	}
	else if(sbgDayNHours[d2_morning]>0){
		int actIndexEnd=-1;
		int h2;
		for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
			if(sbgTimetable(d2_morning,h2)>=0){
				actIndexEnd=sbgTimetable(d2_morning,h2);
				break;
			}
		}
		if(actIndexEnd>=0)
			if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai ) // || actIndexEnd==actIndexBegin
				actIndexEnd=-1;

		if(actIndexEnd>=0){
			assert(!acts.contains(actIndexEnd));
			possibleDays.append(d2_morning);
			atBeginning.append(false);
			acts.append(actIndexEnd);
		}
	}

	bool possibleBeginOrEnd=true;
	if(possibleDays.count()==0)
		possibleBeginOrEnd=false;

	if(possibleBeginOrEnd){
		int t;

		if(level==0){
			int optMinWrong=INF;

			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
					optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}

			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));

			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(possibleDays.count());
		}

		assert(t>=0 && t<possibleDays.count());

		int d2=possibleDays.at(t);
		bool begin=atBeginning.at(t);
		int ai2=acts.at(t);

		removedActivity=ai2;

		if(begin){
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(sbgTimetable(d2,h2)>=0)
					break;
			assert(h2<gt.rules.nHoursPerDay);

			assert(sbgTimetable(d2,h2)==ai2);

			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}
		else{
			int h2;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
				if(sbgTimetable(d2,h2)>=0)
					break;
			assert(h2>=0);

			assert(sbgTimetable(d2,h2)==ai2);

			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}

		return true;
	}
	else
		return false;
}

/*inline bool Generate::subgroupRemoveAnActivityFromBeginCertainRealDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	QList<int> possibleDays;
	QList<bool> atBeginning;
	QList<int> acts;
	int real_d=d2;

	int d2_morning=real_d*2;
	int d2_afternoon=real_d*2+1;

	if(sbgDayNHours[d2_morning]>0){
		int actIndexBegin=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(sbgTimetable(d2_morning,h2)>=0){
				actIndexBegin=sbgTimetable(d2_morning,h2);
				break;
			}
		}
		if(actIndexBegin>=0)
			if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
				actIndexBegin=-1;

		if(actIndexBegin>=0){
			assert(!acts.contains(actIndexBegin));
			possibleDays.append(d2_morning);
			atBeginning.append(true);
			acts.append(actIndexBegin);
		}
	}
	else if(sbgDayNHours[d2_afternoon]>0){
		int actIndexBegin=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(sbgTimetable(d2_afternoon,h2)>=0){
				actIndexBegin=sbgTimetable(d2_afternoon,h2);
				break;
			}
		}
		if(actIndexBegin>=0)
			if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
				actIndexBegin=-1;

		if(actIndexBegin>=0){
			assert(!acts.contains(actIndexBegin));
			possibleDays.append(d2_afternoon);
			atBeginning.append(true);
			acts.append(actIndexBegin);
		}
	}

	bool possibleBeginOrEnd=true;
	if(possibleDays.count()==0)
		possibleBeginOrEnd=false;

	if(possibleBeginOrEnd){
		int t;

		if(level==0){
			int optMinWrong=INF;

			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
					optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}

			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));

			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(possibleDays.count());
		}

		assert(t>=0 && t<possibleDays.count());

		int d2=possibleDays.at(t);
		bool begin=atBeginning.at(t);
		int ai2=acts.at(t);

		removedActivity=ai2;

		if(begin){
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(sbgTimetable(d2,h2)>=0)
					break;
			assert(h2<gt.rules.nHoursPerDay);

			assert(sbgTimetable(d2,h2)==ai2);

			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}
		else{
			int h2;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
				if(sbgTimetable(d2,h2)>=0)
					break;
			assert(h2>=0);

			assert(sbgTimetable(d2,h2)==ai2);

			assert(!conflActivities.contains(ai2));
			conflActivities.append(ai2);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		}

		return true;
	}
	else
		return false;
}
*/

inline bool Generate::subgroupRemoveAnActivityFromBegin(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	//Subgroup: remove an activity from the beginning of any day
	QList<int> possibleDays;
	QList<int> acts;
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(sbgDayNHours[d2]>0){
			int actIndexBegin=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(sbgTimetable(d2,h2)>=0){
					actIndexBegin=sbgTimetable(d2,h2);
					break;
				}
			}
			if(actIndexBegin>=0)
				if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
					actIndexBegin=-1;

			if(actIndexBegin>=0){
				assert(!acts.contains(actIndexBegin));
				possibleDays.append(d2);
				acts.append(actIndexBegin);
			}
		}
	}
	
	bool possibleBegin=true;
	if(possibleDays.count()==0)
		possibleBegin=false;
	
	if(possibleBegin){
		int t;
		
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(possibleDays.count());
		}
		
		assert(t>=0 && t<possibleDays.count());
		
		int d2=possibleDays.at(t);
		int ai2=acts.at(t);
		
		removedActivity=ai2;
		
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(sbgTimetable(d2,h2)>=0)
				break;
		assert(h2<gt.rules.nHoursPerDay);
	
		assert(sbgTimetable(d2,h2)==ai2);
		
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromBeginMorning(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	QList<int> possibleDays;
	QList<int> acts;
	for(int real_d=0; real_d<gt.rules.nDaysPerWeek/2; real_d++){
		int d2=real_d*2;
		if(sbgDayNHours[d2]>0){
			int actIndexBegin=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(sbgTimetable(d2,h2)>=0){
					actIndexBegin=sbgTimetable(d2,h2);
					break;
				}
			}
			if(actIndexBegin>=0)
				if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
					actIndexBegin=-1;

			if(actIndexBegin>=0){
				assert(!acts.contains(actIndexBegin));
				possibleDays.append(d2);
				acts.append(actIndexBegin);
			}
		}
	}

	bool possibleBegin=true;
	if(possibleDays.count()==0)
		possibleBegin=false;

	if(possibleBegin){
		int t;

		if(level==0){
			int optMinWrong=INF;

			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
					optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}

			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));

			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(possibleDays.count());
		}

		assert(t>=0 && t<possibleDays.count());

		int d2=possibleDays.at(t);
		int ai2=acts.at(t);

		removedActivity=ai2;

		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(sbgTimetable(d2,h2)>=0)
				break;
		assert(h2<gt.rules.nHoursPerDay);

		assert(sbgTimetable(d2,h2)==ai2);

		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());

		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromBeginCertainDay(int d2, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	//Subgroup: remove an activity from the beginning of a certain day
	if(sbgDayNHours[d2]>0){
		int actIndexBegin=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(sbgTimetable(d2,h2)>=0){
				actIndexBegin=sbgTimetable(d2,h2);
				break;
			}
		}
		if(actIndexBegin>=0)
			if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
				actIndexBegin=-1;

		if(actIndexBegin>=0){
			removedActivity=actIndexBegin;
	
			assert(!conflActivities.contains(actIndexBegin));
			conflActivities.append(actIndexBegin);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromBeginCertainTwoDays(int d2, int d4, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	int actIndexBegin2=-1;
	if(sbgDayNHours[d2]>0){
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(sbgTimetable(d2,h2)>=0){
				actIndexBegin2=sbgTimetable(d2,h2);
				break;
			}
		}
		if(actIndexBegin2>=0)
			if(fixedTimeActivity[actIndexBegin2] || swappedActivities[actIndexBegin2] || actIndexBegin2==ai)
				actIndexBegin2=-1;
	}
	int actIndexBegin4=-1;
	if(sbgDayNHours[d4]>0){
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(sbgTimetable(d4,h2)>=0){
				actIndexBegin4=sbgTimetable(d4,h2);
				break;
			}
		}
		if(actIndexBegin4>=0)
			if(fixedTimeActivity[actIndexBegin4] || swappedActivities[actIndexBegin4] || actIndexBegin4==ai)
				actIndexBegin4=-1;
	}

	int actIndexBegin=-1;

	if(actIndexBegin2==-1)
		actIndexBegin=actIndexBegin4;
	else if(actIndexBegin4==-1)
		actIndexBegin=actIndexBegin2;
	else{
		if(level==0){
			int optMinWrong=INF;

			if(optMinWrong>triedRemovals(actIndexBegin2,c.times[actIndexBegin2]))
				optMinWrong=triedRemovals(actIndexBegin2,c.times[actIndexBegin2]);
			if(optMinWrong>triedRemovals(actIndexBegin4,c.times[actIndexBegin4]))
				optMinWrong=triedRemovals(actIndexBegin4,c.times[actIndexBegin4]);

			QList<int> tl;

			if(optMinWrong==triedRemovals(actIndexBegin2,c.times[actIndexBegin2]))
				tl.append(0);
			if(optMinWrong==triedRemovals(actIndexBegin4,c.times[actIndexBegin4]))
				tl.append(1);

			assert(tl.count()>=1);
			int ii=tl.at(rng.intMRG32k3a(tl.count()));

			if(ii==0)
				actIndexBegin=actIndexBegin2;
			else if(ii==1)
				actIndexBegin=actIndexBegin4;
		}
		else{
			if(rng.intMRG32k3a(2)==0)
				actIndexBegin=actIndexBegin2;
			else
				actIndexBegin=actIndexBegin4;
		}
	}

	if(actIndexBegin>=0){
		removedActivity=actIndexBegin;

		assert(!conflActivities.contains(actIndexBegin));
		conflActivities.append(actIndexBegin);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());

		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromEnd(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	//Subgroup: remove an activity from the end of any day
	QList<int> possibleDays;
	QList<int> acts;
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(sbgDayNHours[d2]>0){
			int actIndexEnd=-1;
			int h2;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
				if(sbgTimetable(d2,h2)>=0){
					actIndexEnd=sbgTimetable(d2,h2);
					break;
				}
			}
			if(actIndexEnd>=0)
				if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai)
					actIndexEnd=-1;

			if(actIndexEnd>=0){
				assert(!acts.contains(actIndexEnd));
				possibleDays.append(d2);
				acts.append(actIndexEnd);
			}
		}
	}
	
	bool possibleEnd=true;
	if(possibleDays.count()==0)
		possibleEnd=false;
	
	if(possibleEnd){
		int t;
		
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(possibleDays.count());
		}
		
		assert(t>=0 && t<possibleDays.count());
		
		int d2=possibleDays.at(t);
		int ai2=acts.at(t);
		
		removedActivity=ai2;
		
		int h2;
		for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
			if(sbgTimetable(d2,h2)>=0)
				break;
		assert(h2>=0);
		
		assert(sbgTimetable(d2,h2)==ai2);
		
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromEndAfternoon(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	QList<int> possibleDays;
	QList<int> acts;
	for(int real_d=0; real_d<gt.rules.nDaysPerWeek/2; real_d++){
		int d2=2*real_d+1;
		if(sbgDayNHours[d2]>0){
			int actIndexEnd=-1;
			int h2;
			for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
				if(sbgTimetable(d2,h2)>=0){
					actIndexEnd=sbgTimetable(d2,h2);
					break;
				}
			}
			if(actIndexEnd>=0)
				if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai)
					actIndexEnd=-1;

			if(actIndexEnd>=0){
				assert(!acts.contains(actIndexEnd));
				possibleDays.append(d2);
				acts.append(actIndexEnd);
			}
		}
	}

	bool possibleEnd=true;
	if(possibleDays.count()==0)
		possibleEnd=false;

	if(possibleEnd){
		int t;

		if(level==0){
			int optMinWrong=INF;

			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
					optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}

			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));

			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(possibleDays.count());
		}

		assert(t>=0 && t<possibleDays.count());

		int d2=possibleDays.at(t);
		int ai2=acts.at(t);

		removedActivity=ai2;

		int h2;
		for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
			if(sbgTimetable(d2,h2)>=0)
				break;
		assert(h2>=0);

		assert(sbgTimetable(d2,h2)==ai2);

		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());

		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromEndCertainDay(int d2, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	//Subgroup: remove an activity from the end of a certain day
	if(sbgDayNHours[d2]>0){
		int actIndexEnd=-1;
		int h2;
		for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
			if(sbgTimetable(d2,h2)>=0){
				actIndexEnd=sbgTimetable(d2,h2);
				break;
			}
		}
		if(actIndexEnd>=0)
			if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai)
				actIndexEnd=-1;

		if(actIndexEnd>=0){
			removedActivity=actIndexEnd;
	
			assert(!conflActivities.contains(actIndexEnd));
			conflActivities.append(actIndexEnd);
			nConflActivities++;
			assert(nConflActivities==conflActivities.count());
		
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromEndCertainTwoDays(int d2, int d4, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	int actIndexEnd2=-1;
	if(sbgDayNHours[d2]>0){
		int h2;
		for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
			if(sbgTimetable(d2,h2)>=0){
				actIndexEnd2=sbgTimetable(d2,h2);
				break;
			}
		}
		if(actIndexEnd2>=0)
			if(fixedTimeActivity[actIndexEnd2] || swappedActivities[actIndexEnd2] || actIndexEnd2==ai)
				actIndexEnd2=-1;
	}
	int actIndexEnd4=-1;
	if(sbgDayNHours[d4]>0){
		int h2;
		for(h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
			if(sbgTimetable(d4,h2)>=0){
				actIndexEnd4=sbgTimetable(d4,h2);
				break;
			}
		}
		if(actIndexEnd4>=0)
			if(fixedTimeActivity[actIndexEnd4] || swappedActivities[actIndexEnd4] || actIndexEnd4==ai)
				actIndexEnd4=-1;
	}

	int actIndexEnd=-1;

	if(actIndexEnd2==-1)
		actIndexEnd=actIndexEnd4;
	else if(actIndexEnd4==-1)
		actIndexEnd=actIndexEnd2;
	else{
		if(level==0){
			int optMinWrong=INF;

			if(optMinWrong>triedRemovals(actIndexEnd2,c.times[actIndexEnd2]))
				optMinWrong=triedRemovals(actIndexEnd2,c.times[actIndexEnd2]);
			if(optMinWrong>triedRemovals(actIndexEnd4,c.times[actIndexEnd4]))
				optMinWrong=triedRemovals(actIndexEnd4,c.times[actIndexEnd4]);

			QList<int> tl;

			if(optMinWrong==triedRemovals(actIndexEnd2,c.times[actIndexEnd2]))
				tl.append(0);
			if(optMinWrong==triedRemovals(actIndexEnd4,c.times[actIndexEnd4]))
				tl.append(1);

			assert(tl.count()>=1);
			int ii=tl.at(rng.intMRG32k3a(tl.count()));

			if(ii==0)
				actIndexEnd=actIndexEnd2;
			else if(ii==1)
				actIndexEnd=actIndexEnd4;
		}
		else{
			if(rng.intMRG32k3a(2)==0)
				actIndexEnd=actIndexEnd2;
			else
				actIndexEnd=actIndexEnd4;
		}
	}

	if(actIndexEnd>=0){
		removedActivity=actIndexEnd;

		assert(!conflActivities.contains(actIndexEnd));
		conflActivities.append(actIndexEnd);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());

		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromAnywhere(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	//Subgroup: remove an activity from anywhere
	QList<int> acts;
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		if(sbgDayNHours[d2]>0){
			int actIndex=-1;
			int h2;
			for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
				if(sbgTimetable(d2,h2)>=0){
					actIndex=sbgTimetable(d2,h2);
					
					if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex))
						actIndex=-1;

					if(actIndex>=0){
						assert(!acts.contains(actIndex));
						acts.append(actIndex);
					}
				}
		}
	}
	
	if(acts.count()>0){
		int t;
		
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(acts.count());
		}
		
		int ai2=acts.at(t);
		
		removedActivity=ai2;
		
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromAnywhereCertainDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	//Subgroup: remove an activity from anywhere certain day
	QList<int> acts;
	if(sbgDayNHours[d2]>0){
		int actIndex=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(sbgTimetable(d2,h2)>=0){
				actIndex=sbgTimetable(d2,h2);
				
				if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex))
					actIndex=-1;

				if(actIndex>=0){
					assert(!acts.contains(actIndex));
					acts.append(actIndex);
				}
			}
	}
	
	if(acts.count()>0){
		int t;
		
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
				 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(acts.count());
		}
		
		int ai2=acts.at(t);
		
		removedActivity=ai2;
		
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromBeginOrEndCertainDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	//Subgroup: remove an activity from the beginning or from the end of a certain day
	int actIndexBegin=-1, actIndexEnd=-1;
	
	if(sbgDayNHours[d2]>0){
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			if(sbgTimetable(d2,h2)>=0){
				actIndexBegin=sbgTimetable(d2,h2);
				break;
			}
		}
		if(actIndexBegin>=0)
			if(fixedTimeActivity[actIndexBegin] || swappedActivities[actIndexBegin] || actIndexBegin==ai)
				actIndexBegin=-1;
		for(int h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
			if(sbgTimetable(d2,h2)>=0){
				actIndexEnd=sbgTimetable(d2,h2);
				break;
			}
		}
		if(actIndexEnd>=0)
			if(fixedTimeActivity[actIndexEnd] || swappedActivities[actIndexEnd] || actIndexEnd==ai || actIndexEnd==actIndexBegin)
				actIndexEnd=-1;
	}
	
	if(actIndexEnd>=0 || actIndexBegin>=0){
		int ai2=-1;
		if(level==0){
			int optMinWrong=INF;

			if(actIndexBegin>=0){
				if(optMinWrong>triedRemovals(actIndexBegin,c.times[actIndexBegin])){
				 	optMinWrong=triedRemovals(actIndexBegin,c.times[actIndexBegin]);
				}
				ai2=actIndexBegin;
			}

			if(actIndexEnd>=0){
				if(optMinWrong>triedRemovals(actIndexEnd,c.times[actIndexEnd])){
				 	optMinWrong=triedRemovals(actIndexEnd,c.times[actIndexEnd]);
				}
				ai2=actIndexEnd;
			}
			
			if(actIndexBegin>=0 && actIndexEnd>=0 && optMinWrong==triedRemovals(actIndexEnd,c.times[actIndexEnd]) &&
			  optMinWrong==triedRemovals(actIndexBegin,c.times[actIndexBegin])){
				if(rng.intMRG32k3a(2)==0)
					ai2=actIndexBegin;
				else
					ai2=actIndexEnd;
			}
		}
		else{
			if(actIndexBegin>=0 && actIndexEnd<0)
				ai2=actIndexBegin;
			else if(actIndexEnd>=0 && actIndexBegin<0)
				ai2=actIndexEnd;
			else{
				if(rng.intMRG32k3a(2)==0)
					ai2=actIndexBegin;
				else
					ai2=actIndexEnd;
			}
		}
		assert(ai2>=0);
		
		removedActivity=ai2;
		
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromAnywhereCertainDayCertainActivityTag(int d2, int actTag, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	//Subgroup: remove an activity from anywhere certain day certain activity tag
	QList<int> acts;
	if(sbgDayNHours[d2]>0){
		int actIndex=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(sbgTimetable(d2,h2)>=0){
				actIndex=sbgTimetable(d2,h2);
				
				if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex) || !gt.rules.internalActivitiesList[actIndex].iActivityTagsSet.contains(actTag))
					actIndex=-1;

				if(actIndex>=0){
					assert(!acts.contains(actIndex));
					acts.append(actIndex);
				}
			}
	}
	
	if(acts.count()>0){
		int t;
		
		if(level==0){
			int optMinWrong=INF;
			
			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
					optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}
			
			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}
			
			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));
			
			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(acts.count());
		}
		
		int ai2=acts.at(t);
		
		removedActivity=ai2;
		
		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());
		
		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromAnywhereCertainDayDayPairCertainActivityTag(int d2, int dpair2, int actTag, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	QList<int> acts;
	if(sbgDayNHours[d2]>0){
		int actIndex=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(sbgTimetable(d2,h2)>=0){
				actIndex=sbgTimetable(d2,h2);

				if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex) || !gt.rules.internalActivitiesList[actIndex].iActivityTagsSet.contains(actTag))
					actIndex=-1;

				if(actIndex>=0){
					assert(!acts.contains(actIndex));
					acts.append(actIndex);
				}
			}
	}
	if(sbgDayNHours[dpair2]>0){
		int actIndex=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(sbgTimetable(dpair2,h2)>=0){
				actIndex=sbgTimetable(dpair2,h2);

				assert(actIndex!=ai);

				if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || acts.contains(actIndex) || !gt.rules.internalActivitiesList[actIndex].iActivityTagsSet.contains(actTag))
					actIndex=-1;

				if(actIndex>=0){
					assert(!acts.contains(actIndex));
					acts.append(actIndex);
				}
			}
	}

	if(acts.count()>0){
		int t;

		if(level==0){
			int optMinWrong=INF;

			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
					optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}

			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));

			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(acts.count());
		}

		int ai2=acts.at(t);

		removedActivity=ai2;

		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());

		return true;
	}
	else
		return false;
}

inline bool Generate::subgroupRemoveAnActivityFromAnywhereCertainTwoDays(int d2, int d4, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity) //returns true if successful, false if impossible
{
	QList<int> acts;
	if(sbgDayNHours[d2]>0){
		int actIndex=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(sbgTimetable(d2,h2)>=0){
				actIndex=sbgTimetable(d2,h2);

				if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex))
					actIndex=-1;

				if(actIndex>=0){
					assert(!acts.contains(actIndex));
					acts.append(actIndex);
				}
			}
	}
	if(sbgDayNHours[d4]>0){
		int actIndex=-1;
		int h2;
		for(h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(sbgTimetable(d4,h2)>=0){
				actIndex=sbgTimetable(d4,h2);

				if(fixedTimeActivity[actIndex] || swappedActivities[actIndex] || actIndex==ai || acts.contains(actIndex))
					actIndex=-1;

				if(actIndex>=0){
					assert(!acts.contains(actIndex));
					acts.append(actIndex);
				}
			}
	}

	if(acts.count()>0){
		int t;

		if(level==0){
			int optMinWrong=INF;

			QList<int> tl;

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
					optMinWrong=triedRemovals(ai2,c.times[ai2]);
				}
			}

			for(int q=0; q<acts.count(); q++){
				int ai2=acts.at(q);
				if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
					tl.append(q);
			}

			assert(tl.count()>=1);
			int mpos=tl.at(rng.intMRG32k3a(tl.count()));

			assert(mpos>=0 && mpos<acts.count());
			t=mpos;
		}
		else{
			t=rng.intMRG32k3a(acts.count());
		}

		int ai2=acts.at(t);

		removedActivity=ai2;

		assert(!conflActivities.contains(ai2));
		conflActivities.append(ai2);
		nConflActivities++;
		assert(nConflActivities==conflActivities.count());

		return true;
	}
	else
		return false;
}

bool Generate::skipRandom(double weightPercentage)
{
	if(weightPercentage<0.0)
		return true; //non-existing constraint
		
	if(weightPercentage>=100.0)
		return false;

	double t=weightPercentage/100.0;
	assert(t>=0.0 && t<1.0);
	
	qint64 tt=qint64(t*double(MULTIPLICANT_DOUBLE_PRECISION));
	tt*=rng.m1;
	tt+=HALF_MULTIPLICANT_DOUBLE_PRECISION; //round the result of the division below
	tt/=MULTIPLICANT_DOUBLE_PRECISION;
	assert(tt>=0 && tt<rng.m1);
	unsigned int ttu=(unsigned int)(tt);
	
	unsigned int r=rng.uiMRG32k3a();
	
	if(ttu<=r)
		return true;
	else
		return false;
	
	//old code below
	//t*=double(MM);
	//int tt=int(t+0.5);
	//assert(tt>=0 /* && tt<=MM */); //the second condition is always true, because MM >= any int value
	
	//int r=randomKnuth1MM1();
	//assert(r>0 && r<MM); //r cannot be 0
	//if(tt<=r)
	//	return true;
	//else
	//	return false;
}


Generate::Generate()
{
	this->nThread=0; //only used in multiple generation on multiple threads.
}

Generate::~Generate()
{
}

bool Generate::precompute(QWidget* parent, QTextStream* initialOrderStream)
{
	return processTimeSpaceConstraints(parent, initialOrderStream);
}

inline bool Generate::checkBuildingChanges(int sbg, int tch, const QList<int>& globalConflActivities, int rm, int level, const Activity* act, int ai, int d, int h, QList<int>& tmp_list)
{
	assert((sbg==-1 && tch>=0) || (sbg>=0 && tch==-1));
	if(sbg>=0)
		assert(sbg<gt.rules.nInternalSubgroups);
	if(tch>=0)
		assert(tch<gt.rules.nInternalTeachers);

	if(sbg>=0)
		assert(minGapsBetweenBuildingChangesForStudentsPercentages[sbg]>=0 || maxBuildingChangesPerDayForStudentsPercentages[sbg]>=0
		   || maxBuildingChangesPerWeekForStudentsPercentages[sbg]>=0);
	if(tch>=0)
		assert(minGapsBetweenBuildingChangesForTeachersPercentages[tch]>=0 || maxBuildingChangesPerDayForTeachersPercentages[tch]>=0
		   || maxBuildingChangesPerWeekForTeachersPercentages[tch]>=0);
		
	//int buildings[MAX_HOURS_PER_DAY], activities[MAX_HOURS_PER_DAY];
	for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
		int ai2;
		if(sbg>=0)
			ai2=newSubgroupsTimetable(sbg,d,h2);
		else
			ai2=newTeachersTimetable(tch,d,h2);

		if(ai2>=0 && !globalConflActivities.contains(ai2) && !tmp_list.contains(ai2)){
			int rm2;
			if(h2>=h && h2<h+act->duration){
				assert(ai2==ai);
				rm2=rm;
			}
			else
				rm2=c.rooms[ai2];
			if(rm2!=UNALLOCATED_SPACE && rm2!=UNSPECIFIED_ROOM){
				assert(rm2>=0);
				activities[h2]=ai2;
				buildings[h2]=gt.rules.internalRoomsList[rm2]->buildingIndex;
			}
			else{
				activities[h2]=ai2;
				buildings[h2]=-1;
			}
		}
		else{
			buildings[h2]=-1;
			activities[h2]=-1;
		}
	}
	
	assert(buildings[h]!=-1); //because we checked this before calling the function checkBuildingChanges(...)
	//if(buildings[h]==-1) //no problem
	//	return true;
	
	//min gaps
	double perc;
	int mg;
	if(sbg>=0){
		perc=minGapsBetweenBuildingChangesForStudentsPercentages[sbg];
		mg=minGapsBetweenBuildingChangesForStudentsMinGaps[sbg];
	}
	else{
		perc=minGapsBetweenBuildingChangesForTeachersPercentages[tch];
		mg=minGapsBetweenBuildingChangesForTeachersMinGaps[tch];
	}
	if(perc>=0){
		for(int h2=max(0, h-mg); h2<=min(h+act->duration-1+mg, gt.rules.nHoursPerDay-1); h2++)
			if(!(h2>=h && h2<h+act->duration))
				if(buildings[h2]!=buildings[h] && buildings[h2]!=-1){
					int ai2=activities[h2];
					assert(ai2>=0);
					if(!swappedActivities[ai2] && !(fixedTimeActivity[ai2] && fixedSpaceActivity[ai2])){
						if(!tmp_list.contains(ai2)){
							tmp_list.append(ai2);
							
							int ha=c.times[ai2]/gt.rules.nDaysPerWeek;
							int dura=gt.rules.internalActivitiesList[ai2].duration;
							for(int h3=ha; h3<ha+dura; h3++){
								assert(activities[h3]==ai2);
								assert(buildings[h3]!=-1);
								buildings[h3]=-1;
								activities[h3]=-1;
							}
						}
					}
					else{
						return false;
					}
				}
	}

	//max changes per day
	int mc;
	if(sbg>=0){
		perc=maxBuildingChangesPerDayForStudentsPercentages[sbg];
		mc=maxBuildingChangesPerDayForStudentsMaxChanges[sbg];
	}
	else{
		perc=maxBuildingChangesPerDayForTeachersPercentages[tch];
		mc=maxBuildingChangesPerDayForTeachersMaxChanges[tch];
	}
	
	if(perc>=0){
		int crt_building=-1;
		int n_changes=0;
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(buildings[h2]!=-1){
				if(crt_building!=buildings[h2]){
					if(crt_building!=-1)
						n_changes++;
					crt_building=buildings[h2];
				}
			}
			
		if(n_changes>mc){ //not OK
			if(level>=LEVEL_STOP_CONFLICTS_CALCULATION)
				return false;
			
			QList<int> removableActsList;
			for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(!(h2>=h && h2<h+act->duration))
					if(buildings[h2]!=-1 && activities[h2]>=0 && !swappedActivities[activities[h2]] && !(fixedTimeActivity[activities[h2]] && fixedSpaceActivity[activities[h2]]))
						if(!removableActsList.contains(activities[h2])){
							removableActsList.append(activities[h2]);
							assert(!globalConflActivities.contains(activities[h2]));
							assert(!tmp_list.contains(activities[h2]));
						}
			}
			
			for(;;){
				int ai2=-1;
				QList<int> optimalRemovableActs;
				if(level==0){
					int nWrong=INF;
					for(int a : qAsConst(removableActsList))
						if(nWrong>triedRemovals(a,c.times[a])){
							nWrong=triedRemovals(a,c.times[a]);
						}
					for(int a : qAsConst(removableActsList))
						if(nWrong==triedRemovals(a,c.times[a]))
							optimalRemovableActs.append(a);
				}
				else
					optimalRemovableActs=removableActsList;
					
				if(removableActsList.count()>0)
					assert(optimalRemovableActs.count()>0);
					
				if(optimalRemovableActs.count()==0)
					return false;
					
				ai2=optimalRemovableActs.at(rng.intMRG32k3a(optimalRemovableActs.count()));
				
				assert(!swappedActivities[ai2]);
				assert(!(fixedTimeActivity[ai2] && fixedSpaceActivity[ai2]));
				assert(!globalConflActivities.contains(ai2));
				assert(!tmp_list.contains(ai2));
				assert(ai2>=0);

				tmp_list.append(ai2);
				
				int t=removableActsList.removeAll(ai2);
				assert(t==1);
				
				int ha=c.times[ai2]/gt.rules.nDaysPerWeek;
				int da=gt.rules.internalActivitiesList[ai2].duration;
				for(int h2=ha; h2<ha+da; h2++){
					assert(activities[h2]==ai2);
					assert(buildings[h2]!=-1);
					buildings[h2]=-1;
					activities[h2]=-1;
				}
				
				int crt_building=-1;
				int n_changes=0;
				for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
					if(buildings[h2]!=-1){
						if(crt_building!=buildings[h2]){
							if(crt_building!=-1)
								n_changes++;
							crt_building=buildings[h2];
						}
					}
			
				if(n_changes<=mc){ //OK
					break;
				}
			}
		}
	}
	
	//max changes per week
	if(sbg>=0){
		perc=maxBuildingChangesPerWeekForStudentsPercentages[sbg];
		mc=maxBuildingChangesPerWeekForStudentsMaxChanges[sbg];
	}
	else{
		perc=maxBuildingChangesPerWeekForTeachersPercentages[tch];
		mc=maxBuildingChangesPerWeekForTeachersMaxChanges[tch];
	}
	if(perc==-1){
		assert(mc==-1);
		return true;
	}
	
	//Old comment below
	//I would like to get rid of these large static variables, but making them dynamic slows down ~33% for a sample from Timisoara Economics
	//static int weekBuildings[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	//static int weekActivities[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			int ai2;
			if(sbg>=0)
				ai2=newSubgroupsTimetable(sbg,d2,h2);
			else
				ai2=newTeachersTimetable(tch,d2,h2);
	
			if(ai2>=0 && !globalConflActivities.contains(ai2) && !tmp_list.contains(ai2)){
				int rm2;
				if(d==d2 && h2>=h && h2<h+act->duration){
					assert(ai2==ai);
					rm2=rm;
				}
				else
					rm2=c.rooms[ai2];
				if(rm2!=UNALLOCATED_SPACE && rm2!=UNSPECIFIED_ROOM){
					assert(rm2>=0);
					weekActivities[d2][h2]=ai2;
					weekBuildings[d2][h2]=gt.rules.internalRoomsList[rm2]->buildingIndex;
				}
				else{
					weekActivities[d2][h2]=ai2;
					weekBuildings[d2][h2]=-1;
				}
			}
			else{
				weekBuildings[d2][h2]=-1;
				weekActivities[d2][h2]=-1;
			}
		}
	}

	int n_changes=0;
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		int crt_building=-1;
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(weekBuildings[d2][h2]!=-1){
				if(crt_building!=weekBuildings[d2][h2]){
					if(crt_building!=-1)
						n_changes++;
					crt_building=weekBuildings[d2][h2];
				}
			}
	}
	
	if(n_changes>mc){ //not OK
		if(level>=LEVEL_STOP_CONFLICTS_CALCULATION)
			return false;
		
		QList<int> removableActsList;
		for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
			for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(!(d2==d && h2>=h && h2<h+act->duration))
					if(weekBuildings[d2][h2]!=-1 && weekActivities[d2][h2]>=0 && !swappedActivities[weekActivities[d2][h2]] && !(fixedTimeActivity[weekActivities[d2][h2]] && fixedSpaceActivity[weekActivities[d2][h2]]))
						if(!removableActsList.contains(weekActivities[d2][h2])){
							removableActsList.append(weekActivities[d2][h2]);
							assert(!globalConflActivities.contains(weekActivities[d2][h2]));
							assert(!tmp_list.contains(weekActivities[d2][h2]));
						}
			}
		}
		
		for(;;){
			int ai2=-1;
			QList<int> optimalRemovableActs;
			if(level==0){
				int nWrong=INF;
				for(int a : qAsConst(removableActsList))
					if(nWrong>triedRemovals(a,c.times[a])){
						nWrong=triedRemovals(a,c.times[a]);
					}
				for(int a : qAsConst(removableActsList))
					if(nWrong==triedRemovals(a,c.times[a]))
						optimalRemovableActs.append(a);
			}
			else
				optimalRemovableActs=removableActsList;
			
			if(removableActsList.count()>0)
				assert(optimalRemovableActs.count()>0);
			
			if(optimalRemovableActs.count()==0)
				return false;
			
			ai2=optimalRemovableActs.at(rng.intMRG32k3a(optimalRemovableActs.count()));
			
			assert(!swappedActivities[ai2]);
			assert(!(fixedTimeActivity[ai2] && fixedSpaceActivity[ai2]));
			assert(!globalConflActivities.contains(ai2));
			assert(!tmp_list.contains(ai2));
			assert(ai2>=0);

			tmp_list.append(ai2);
			
			int t=removableActsList.removeAll(ai2);
			assert(t==1);
			
			int ha=c.times[ai2]/gt.rules.nDaysPerWeek;
			int da=c.times[ai2]%gt.rules.nDaysPerWeek;
			int dura=gt.rules.internalActivitiesList[ai2].duration;
			for(int h2=ha; h2<ha+dura; h2++){
				assert(weekActivities[da][h2]==ai2);
				assert(weekBuildings[da][h2]!=-1);
				weekBuildings[da][h2]=-1;
				weekActivities[da][h2]=-1;
			}
			
			int n_changes=0;
			for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
				int crt_building=-1;
				for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
					if(weekBuildings[d2][h2]!=-1){
						if(crt_building!=weekBuildings[d2][h2]){
							if(crt_building!=-1)
								n_changes++;
							crt_building=weekBuildings[d2][h2];
						}
					}
			}
		
			if(n_changes<=mc){ //OK
				break;
			}
		}
	}
	
	return true;
}

inline bool Generate::checkRoomChanges(int sbg, int tch, const QList<int>& globalConflActivities, int rm, int level, const Activity* act, int ai, int d, int h, QList<int>& tmp_list)
{
	assert((sbg==-1 && tch>=0) || (sbg>=0 && tch==-1));
	if(sbg>=0)
		assert(sbg<gt.rules.nInternalSubgroups);
	if(tch>=0)
		assert(tch<gt.rules.nInternalTeachers);

	if(sbg>=0)
		assert(minGapsBetweenRoomChangesForStudentsPercentages[sbg]>=0 || maxRoomChangesPerDayForStudentsPercentages[sbg]>=0
		   || maxRoomChangesPerWeekForStudentsPercentages[sbg]>=0);
	if(tch>=0)
		assert(minGapsBetweenRoomChangesForTeachersPercentages[tch]>=0 || maxRoomChangesPerDayForTeachersPercentages[tch]>=0
		   || maxRoomChangesPerWeekForTeachersPercentages[tch]>=0);
	
	//int rooms[MAX_HOURS_PER_DAY], activities[MAX_HOURS_PER_DAY];
	for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
		int ai2;
		if(sbg>=0)
			ai2=newSubgroupsTimetable(sbg,d,h2);
		else
			ai2=newTeachersTimetable(tch,d,h2);

		if(ai2>=0 && !globalConflActivities.contains(ai2) && !tmp_list.contains(ai2)){
			int rm2;
			if(h2>=h && h2<h+act->duration){
				assert(ai2==ai);
				rm2=rm;
			}
			else
				rm2=c.rooms[ai2];
			if(rm2!=UNALLOCATED_SPACE && rm2!=UNSPECIFIED_ROOM){
				assert(rm2>=0);
				activities[h2]=ai2;
				rooms[h2]=rm2;
			}
			else{
				activities[h2]=ai2;
				rooms[h2]=-1;
			}
		}
		else{
			rooms[h2]=-1;
			activities[h2]=-1;
		}
	}
	
	assert(rooms[h]!=-1); //because we checked this before calling the function checkRoomChanges(...)
	//if(rooms[h]==-1) //no problem
	//	return true;
	
	//min gaps
	double perc;
	int mg;
	if(sbg>=0){
		perc=minGapsBetweenRoomChangesForStudentsPercentages[sbg];
		mg=minGapsBetweenRoomChangesForStudentsMinGaps[sbg];
	}
	else{
		perc=minGapsBetweenRoomChangesForTeachersPercentages[tch];
		mg=minGapsBetweenRoomChangesForTeachersMinGaps[tch];
	}
	if(perc>=0){
		for(int h2=max(0, h-mg); h2<=min(h+act->duration-1+mg, gt.rules.nHoursPerDay-1); h2++)
			if(!(h2>=h && h2<h+act->duration))
				if(rooms[h2]!=rooms[h] && rooms[h2]!=-1){
					int ai2=activities[h2];
					assert(ai2>=0);
					if(!swappedActivities[ai2] && !(fixedTimeActivity[ai2] && fixedSpaceActivity[ai2])){
						if(!tmp_list.contains(ai2)){
							tmp_list.append(ai2);
							
							int ha=c.times[ai2]/gt.rules.nDaysPerWeek;
							int dura=gt.rules.internalActivitiesList[ai2].duration;
							for(int h3=ha; h3<ha+dura; h3++){
								assert(activities[h3]==ai2);
								assert(rooms[h3]!=-1);
								rooms[h3]=-1;
								activities[h3]=-1;
							}
						}
					}
					else{
						return false;
					}
				}
	}

	//max changes per day
	int mc;
	if(sbg>=0){
		perc=maxRoomChangesPerDayForStudentsPercentages[sbg];
		mc=maxRoomChangesPerDayForStudentsMaxChanges[sbg];
	}
	else{
		perc=maxRoomChangesPerDayForTeachersPercentages[tch];
		mc=maxRoomChangesPerDayForTeachersMaxChanges[tch];
	}
	
	if(perc>=0){
		int crt_room=-1;
		int n_changes=0;
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(rooms[h2]!=-1){
				if(crt_room!=rooms[h2]){
					if(crt_room!=-1)
						n_changes++;
					crt_room=rooms[h2];
				}
			}
			
		if(n_changes>mc){ //not OK
			if(level>=LEVEL_STOP_CONFLICTS_CALCULATION)
				return false;
			
			QList<int> removableActsList;
			for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(!(h2>=h && h2<h+act->duration))
					if(rooms[h2]!=-1 && activities[h2]>=0 && !swappedActivities[activities[h2]] && !(fixedTimeActivity[activities[h2]] && fixedSpaceActivity[activities[h2]]))
						if(!removableActsList.contains(activities[h2])){
							removableActsList.append(activities[h2]);
							assert(!globalConflActivities.contains(activities[h2]));
							assert(!tmp_list.contains(activities[h2]));
						}
			}
			
			for(;;){
				int ai2=-1;
				QList<int> optimalRemovableActs;
				if(level==0){
					int nWrong=INF;
					for(int a : qAsConst(removableActsList))
						if(nWrong>triedRemovals(a,c.times[a])){
							nWrong=triedRemovals(a,c.times[a]);
						}
					for(int a : qAsConst(removableActsList))
						if(nWrong==triedRemovals(a,c.times[a]))
							optimalRemovableActs.append(a);
				}
				else
					optimalRemovableActs=removableActsList;
				
				if(removableActsList.count()>0)
					assert(optimalRemovableActs.count()>0);
				
				if(optimalRemovableActs.count()==0)
					return false;
				
				ai2=optimalRemovableActs.at(rng.intMRG32k3a(optimalRemovableActs.count()));
				
				assert(!swappedActivities[ai2]);
				assert(!(fixedTimeActivity[ai2] && fixedSpaceActivity[ai2]));
				assert(!globalConflActivities.contains(ai2));
				assert(!tmp_list.contains(ai2));
				assert(ai2>=0);

				tmp_list.append(ai2);
				
				int t=removableActsList.removeAll(ai2);
				assert(t==1);
				
				int ha=c.times[ai2]/gt.rules.nDaysPerWeek;
				int da=gt.rules.internalActivitiesList[ai2].duration;
				for(int h2=ha; h2<ha+da; h2++){
					assert(activities[h2]==ai2);
					assert(rooms[h2]!=-1);
					rooms[h2]=-1;
					activities[h2]=-1;
				}
				
				int crt_room=-1;
				int n_changes=0;
				for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
					if(rooms[h2]!=-1){
						if(crt_room!=rooms[h2]){
							if(crt_room!=-1)
								n_changes++;
							crt_room=rooms[h2];
						}
					}
			
				if(n_changes<=mc){ //OK
					break;
				}
			}
		}
	}
	
	//max changes per week
	if(sbg>=0){
		perc=maxRoomChangesPerWeekForStudentsPercentages[sbg];
		mc=maxRoomChangesPerWeekForStudentsMaxChanges[sbg];
	}
	else{
		perc=maxRoomChangesPerWeekForTeachersPercentages[tch];
		mc=maxRoomChangesPerWeekForTeachersMaxChanges[tch];
	}
	if(perc==-1){
		assert(mc==-1);
		return true;
	}
	
	//Old comment below
	//I would like to get rid of these large static variables, but making them dynamic slows down ~33% for a sample from Timisoara Economics
	//static int weekRooms[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	//static int weekActivities[MAX_DAYS_PER_WEEK][MAX_HOURS_PER_DAY];
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			int ai2;
			if(sbg>=0)
				ai2=newSubgroupsTimetable(sbg,d2,h2);
			else
				ai2=newTeachersTimetable(tch,d2,h2);
	
			if(ai2>=0 && !globalConflActivities.contains(ai2) && !tmp_list.contains(ai2)){
				int rm2;
				if(d==d2 && h2>=h && h2<h+act->duration){
					assert(ai2==ai);
					rm2=rm;
				}
				else
					rm2=c.rooms[ai2];
				if(rm2!=UNALLOCATED_SPACE && rm2!=UNSPECIFIED_ROOM){
					assert(rm2>=0);
					weekActivities[d2][h2]=ai2;
					weekRooms[d2][h2]=rm2;
				}
				else{
					weekActivities[d2][h2]=ai2;
					weekRooms[d2][h2]=-1;
				}
			}
			else{
				weekRooms[d2][h2]=-1;
				weekActivities[d2][h2]=-1;
			}
		}
	}

	int n_changes=0;
	for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
		int crt_room=-1;
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
			if(weekRooms[d2][h2]!=-1){
				if(crt_room!=weekRooms[d2][h2]){
					if(crt_room!=-1)
						n_changes++;
					crt_room=weekRooms[d2][h2];
				}
			}
	}
	
	if(n_changes>mc){ //not OK
		if(level>=LEVEL_STOP_CONFLICTS_CALCULATION)
			return false;
		
		QList<int> removableActsList;
		for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
			for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
				if(!(d2==d && h2>=h && h2<h+act->duration))
					if(weekRooms[d2][h2]!=-1 && weekActivities[d2][h2]>=0 && !swappedActivities[weekActivities[d2][h2]] && !(fixedTimeActivity[weekActivities[d2][h2]] && fixedSpaceActivity[weekActivities[d2][h2]]))
						if(!removableActsList.contains(weekActivities[d2][h2])){
							removableActsList.append(weekActivities[d2][h2]);
							assert(!globalConflActivities.contains(weekActivities[d2][h2]));
							assert(!tmp_list.contains(weekActivities[d2][h2]));
						}
			}
		}
		
		for(;;){
			int ai2=-1;
			QList<int> optimalRemovableActs;
			if(level==0){
				int nWrong=INF;
				for(int a : qAsConst(removableActsList))
					if(nWrong>triedRemovals(a,c.times[a])){
						nWrong=triedRemovals(a,c.times[a]);
					}
				for(int a : qAsConst(removableActsList))
					if(nWrong==triedRemovals(a,c.times[a]))
						optimalRemovableActs.append(a);
			}
			else
				optimalRemovableActs=removableActsList;
			
			if(removableActsList.count()>0)
				assert(optimalRemovableActs.count()>0);
			
			if(optimalRemovableActs.count()==0)
				return false;
			
			ai2=optimalRemovableActs.at(rng.intMRG32k3a(optimalRemovableActs.count()));
			
			assert(!swappedActivities[ai2]);
			assert(!(fixedTimeActivity[ai2] && fixedSpaceActivity[ai2]));
			assert(!globalConflActivities.contains(ai2));
			assert(!tmp_list.contains(ai2));
			assert(ai2>=0);

			tmp_list.append(ai2);
			
			int t=removableActsList.removeAll(ai2);
			assert(t==1);
			
			int ha=c.times[ai2]/gt.rules.nDaysPerWeek;
			int da=c.times[ai2]%gt.rules.nDaysPerWeek;
			int dura=gt.rules.internalActivitiesList[ai2].duration;
			for(int h2=ha; h2<ha+dura; h2++){
				assert(weekActivities[da][h2]==ai2);
				assert(weekRooms[da][h2]!=-1);
				weekRooms[da][h2]=-1;
				weekActivities[da][h2]=-1;
			}
			
			int n_changes=0;
			for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
				int crt_room=-1;
				for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
					if(weekRooms[d2][h2]!=-1){
						if(crt_room!=weekRooms[d2][h2]){
							if(crt_room!=-1)
								n_changes++;
							crt_room=weekRooms[d2][h2];
						}
					}
			}
		
			if(n_changes<=mc){ //OK
				break;
			}
		}
	}
	
	return true;
}

inline bool Generate::checkBuildingChangesPerRealDay(int sbg, int tch, const QList<int>& globalConflActivities, int rm, int level, const Activity* act, int ai, int d, int h, QList<int>& tmp_list)
{
	assert((sbg==-1 && tch>=0) || (sbg>=0 && tch==-1));
	if(sbg>=0){
		assert(sbg<gt.rules.nInternalSubgroups);
		assert(maxBuildingChangesPerRealDayForSubgroupsPercentages[sbg]>=0);
	}
	else if(tch>=0){
		assert(tch<gt.rules.nInternalTeachers);
		assert(maxBuildingChangesPerRealDayForTeachersPercentages[tch]>=0);
	}

	//int rooms[2*MAX_HOURS_PER_DAY], activities[2*MAX_HOURS_PER_DAY];
	for(int q=0; q<2; q++){
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			int ai2;
			if(tch>=0)
				ai2=newTeachersTimetable(tch,(d/2)*2+q,h2);
			else
				ai2=newSubgroupsTimetable(sbg,(d/2)*2+q,h2);

			if(ai2>=0 && !globalConflActivities.contains(ai2) && !tmp_list.contains(ai2)){
				int rm2;
				if((d==(d/2)*2+q) && (h2>=h && h2<h+act->duration)){
					assert(ai2==ai);
					rm2=rm;
				}
				else
					rm2=c.rooms[ai2];
				if(rm2!=UNALLOCATED_SPACE && rm2!=UNSPECIFIED_ROOM){
					assert(rm2>=0);
					activitiesx2[h2+q*gt.rules.nHoursPerDay]=ai2;
					buildingsx2[h2+q*gt.rules.nHoursPerDay]=gt.rules.internalRoomsList[rm2]->buildingIndex;
				}
				else{
					activitiesx2[h2+q*gt.rules.nHoursPerDay]=ai2;
					buildingsx2[h2+q*gt.rules.nHoursPerDay]=-1;
				}
			}
			else{
				buildingsx2[h2+q*gt.rules.nHoursPerDay]=-1;
				activitiesx2[h2+q*gt.rules.nHoursPerDay]=-1;
			}
		}
	}

	if(buildingsx2[h+(d%2)*gt.rules.nHoursPerDay]==-1){ //we entered here assuming rm is a valid room
		assert(0);
		return true;
	}

	double perc;
	int mc;
	if(tch>=0){
		perc=maxBuildingChangesPerRealDayForTeachersPercentages[tch];
		mc=maxBuildingChangesPerRealDayForTeachersMaxChanges[tch];
	}
	else{
		perc=maxBuildingChangesPerRealDayForSubgroupsPercentages[sbg];
		mc=maxBuildingChangesPerRealDayForSubgroupsMaxChanges[sbg];
	}

	if(perc>=0){
		int crt_building=-1;
		int n_changes=0;
		for(int h2=0; h2<2*gt.rules.nHoursPerDay; h2++)
			if(buildingsx2[h2]!=-1){
				if(crt_building!=buildingsx2[h2]){
					if(crt_building!=-1)
						n_changes++;
					crt_building=buildingsx2[h2];
				}
			}

		if(n_changes>mc){ //not OK
			if(level>=LEVEL_STOP_CONFLICTS_CALCULATION)
				return false;

			QList<int> removableActsList;
			for(int h2=0; h2<2*gt.rules.nHoursPerDay; h2++){
				if(!(h2>=h+(d%2)*gt.rules.nHoursPerDay && h2<h+act->duration+(d%2)*gt.rules.nHoursPerDay))
					if(buildingsx2[h2]!=-1 && activitiesx2[h2]>=0 && !swappedActivities[activitiesx2[h2]] && !(fixedTimeActivity[activitiesx2[h2]]&&fixedSpaceActivity[activitiesx2[h2]]))
						if(!removableActsList.contains(activitiesx2[h2])){
							removableActsList.append(activitiesx2[h2]);
							assert(!globalConflActivities.contains(activitiesx2[h2]));
							assert(!tmp_list.contains(activitiesx2[h2]));
						}
			}

			for(;;){
				int ai2=-1;
				QList<int> optimalRemovableActs;
				if(level==0){
					int nWrong=INF;
					for(int a : qAsConst(removableActsList))
						if(nWrong>triedRemovals(a,c.times[a]) ){
							nWrong=triedRemovals(a,c.times[a]);
						}
					for(int a : qAsConst(removableActsList))
						if(nWrong==triedRemovals(a,c.times[a]))
							optimalRemovableActs.append(a);
				}
				else
					optimalRemovableActs=removableActsList;

				if(removableActsList.count()>0)
					assert(optimalRemovableActs.count()>0);

				if(optimalRemovableActs.count()==0)
					return false;

				ai2=optimalRemovableActs.at(rng.intMRG32k3a(optimalRemovableActs.count()));

				assert(!swappedActivities[ai2]);
				assert(!(fixedTimeActivity[ai2]&&fixedSpaceActivity[ai2]));
				assert(!globalConflActivities.contains(ai2));
				assert(!tmp_list.contains(ai2));
				assert(ai2>=0);

				tmp_list.append(ai2);

				int t=removableActsList.removeAll(ai2);
				assert(t==1);

				int ha=c.times[ai2]/gt.rules.nDaysPerWeek;
				int da=c.times[ai2]%gt.rules.nDaysPerWeek;
				int dura=gt.rules.internalActivitiesList[ai2].duration;
				for(int h2=ha; h2<ha+dura; h2++){
					assert(activitiesx2[h2+(da%2)*gt.rules.nHoursPerDay]==ai2);
					assert(buildingsx2[h2+(da%2)*gt.rules.nHoursPerDay]!=-1);
					buildingsx2[h2+(da%2)*gt.rules.nHoursPerDay]=-1;
					activitiesx2[h2+(da%2)*gt.rules.nHoursPerDay]=-1;
				}

				int crt_building=-1;
				int n_changes=0;
				for(int h2=0; h2<2*gt.rules.nHoursPerDay; h2++)
					if(buildingsx2[h2]!=-1){
						if(crt_building!=buildingsx2[h2]){
							if(crt_building!=-1)
								n_changes++;
							crt_building=buildingsx2[h2];
						}
					}

				if(n_changes<=mc){ //OK
					break;
				}
			}
		}
	}

	return true;
}

inline bool Generate::checkRoomChangesPerRealDay(int sbg, int tch, const QList<int>& globalConflActivities, int rm, int level, const Activity* act, int ai, int d, int h, QList<int>& tmp_list)
{
	assert((sbg==-1 && tch>=0) || (sbg>=0 && tch==-1));
	if(sbg>=0){
		assert(sbg<gt.rules.nInternalSubgroups);
		assert(maxRoomChangesPerRealDayForSubgroupsPercentages[sbg]>=0);
	}
	else if(tch>=0){
		assert(tch<gt.rules.nInternalTeachers);
		assert(maxRoomChangesPerRealDayForTeachersPercentages[tch]>=0);
	}

	//int rooms[2*MAX_HOURS_PER_DAY], activities[2*MAX_HOURS_PER_DAY];
	for(int q=0; q<2; q++){
		for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
			int ai2;
			if(tch>=0)
				ai2=newTeachersTimetable(tch,(d/2)*2+q,h2);
			else
				ai2=newSubgroupsTimetable(sbg,(d/2)*2+q,h2);

			if(ai2>=0 && !globalConflActivities.contains(ai2) && !tmp_list.contains(ai2)){
				int rm2;
				if((d==(d/2)*2+q) && (h2>=h && h2<h+act->duration)){
					assert(ai2==ai);
					rm2=rm;
				}
				else
					rm2=c.rooms[ai2];
				if(rm2!=UNALLOCATED_SPACE && rm2!=UNSPECIFIED_ROOM){
					assert(rm2>=0);
					activitiesx2[h2+q*gt.rules.nHoursPerDay]=ai2;
					roomsx2[h2+q*gt.rules.nHoursPerDay]=rm2;
				}
				else{
					activitiesx2[h2+q*gt.rules.nHoursPerDay]=ai2;
					roomsx2[h2+q*gt.rules.nHoursPerDay]=-1;
				}
			}
			else{
				roomsx2[h2+q*gt.rules.nHoursPerDay]=-1;
				activitiesx2[h2+q*gt.rules.nHoursPerDay]=-1;
			}
		}
	}

	if(roomsx2[h+(d%2)*gt.rules.nHoursPerDay]==-1){ //we entered here assuming rm is a valid room
		assert(0);
		return true;
	}

	double perc;
	int mc;
	if(tch>=0){
		perc=maxRoomChangesPerRealDayForTeachersPercentages[tch];
		mc=maxRoomChangesPerRealDayForTeachersMaxChanges[tch];
	}
	else{
		perc=maxRoomChangesPerRealDayForSubgroupsPercentages[sbg];
		mc=maxRoomChangesPerRealDayForSubgroupsMaxChanges[sbg];
	}

	if(perc>=0){
		int crt_room=-1;
		int n_changes=0;
		for(int h2=0; h2<2*gt.rules.nHoursPerDay; h2++)
			if(roomsx2[h2]!=-1){
				if(crt_room!=roomsx2[h2]){
					if(crt_room!=-1)
						n_changes++;
					crt_room=roomsx2[h2];
				}
			}

		if(n_changes>mc){ //not OK
			if(level>=LEVEL_STOP_CONFLICTS_CALCULATION)
				return false;

			QList<int> removableActsList;
			for(int h2=0; h2<2*gt.rules.nHoursPerDay; h2++){
				if(!(h2>=h+(d%2)*gt.rules.nHoursPerDay && h2<h+act->duration+(d%2)*gt.rules.nHoursPerDay))
					if(roomsx2[h2]!=-1 && activitiesx2[h2]>=0 && !swappedActivities[activitiesx2[h2]] && !(fixedTimeActivity[activitiesx2[h2]]&&fixedSpaceActivity[activitiesx2[h2]]))
						if(!removableActsList.contains(activitiesx2[h2])){
							removableActsList.append(activitiesx2[h2]);
							assert(!globalConflActivities.contains(activitiesx2[h2]));
							assert(!tmp_list.contains(activitiesx2[h2]));
						}
			}

			for(;;){
				int ai2=-1;
				QList<int> optimalRemovableActs;
				if(level==0){
					int nWrong=INF;
					for(int a : qAsConst(removableActsList))
						if(nWrong>triedRemovals(a,c.times[a]) ){
							nWrong=triedRemovals(a,c.times[a]);
						}
					for(int a : qAsConst(removableActsList))
						if(nWrong==triedRemovals(a,c.times[a]))
							optimalRemovableActs.append(a);
				}
				else
					optimalRemovableActs=removableActsList;

				if(removableActsList.count()>0)
					assert(optimalRemovableActs.count()>0);

				if(optimalRemovableActs.count()==0)
					return false;

				ai2=optimalRemovableActs.at(rng.intMRG32k3a(optimalRemovableActs.count()));

				assert(!swappedActivities[ai2]);
				assert(!(fixedTimeActivity[ai2]&&fixedSpaceActivity[ai2]));
				assert(!globalConflActivities.contains(ai2));
				assert(!tmp_list.contains(ai2));
				assert(ai2>=0);

				tmp_list.append(ai2);

				int t=removableActsList.removeAll(ai2);
				assert(t==1);

				int ha=c.times[ai2]/gt.rules.nDaysPerWeek;
				int da=c.times[ai2]%gt.rules.nDaysPerWeek;
				int dura=gt.rules.internalActivitiesList[ai2].duration;
				for(int h2=ha; h2<ha+dura; h2++){
					assert(activitiesx2[h2+(da%2)*gt.rules.nHoursPerDay]==ai2);
					assert(roomsx2[h2+(da%2)*gt.rules.nHoursPerDay]!=-1);
					roomsx2[h2+(da%2)*gt.rules.nHoursPerDay]=-1;
					activitiesx2[h2+(da%2)*gt.rules.nHoursPerDay]=-1;
				}

				int crt_room=-1;
				int n_changes=0;
				for(int h2=0; h2<2*gt.rules.nHoursPerDay; h2++)
					if(roomsx2[h2]!=-1){
						if(crt_room!=roomsx2[h2]){
							if(crt_room!=-1)
								n_changes++;
							crt_room=roomsx2[h2];
						}
					}

				if(n_changes<=mc){ //OK
					break;
				}
			}
		}
	}

	return true;
}

//2012-04-29
inline bool Generate::checkActivitiesOccupyMaxDifferentRooms(const QList<int>& globalConflActivities, int rm, int level, int ai, QList<int>& tmp_list)
{
	for(ActivitiesOccupyMaxDifferentRooms_item* item : qAsConst(aomdrListForActivity[ai])){
		//preliminary
		QSet<int> occupiedRoomsSet;
		for(int ai2 : qAsConst(item->activitiesList))
			if(ai2!=ai && !globalConflActivities.contains(ai2) && !tmp_list.contains(ai2)){
				int rm2=c.rooms[ai2];
				if(rm2!=UNALLOCATED_SPACE && rm2!=UNSPECIFIED_ROOM)
					if(!occupiedRoomsSet.contains(rm2)){
						occupiedRoomsSet.insert(rm2);
						if(occupiedRoomsSet.count()==item->maxDifferentRooms) //no further testing needed
							break;
					}
			}
			
		if(!globalConflActivities.contains(ai) && !tmp_list.contains(ai)) //should be always true
			if(rm!=UNALLOCATED_SPACE && rm!=UNSPECIFIED_ROOM) //should be always true
				if(!occupiedRoomsSet.contains(rm))
					occupiedRoomsSet.insert(rm);
		
		if(occupiedRoomsSet.count()<=item->maxDifferentRooms)
			continue;

		//correction needed
		QList<QSet<int>> activitiesInRoom;
		occupiedRoomsSet.clear();
		QList<int> occupiedRoomsList;
		QHash<int, int> roomIndexInOccupiedRoomsList;
		QList<bool> canEmptyRoom;
		
		for(int ai2 : qAsConst(item->activitiesList))
			if(!globalConflActivities.contains(ai2) && !tmp_list.contains(ai2)){
				int rm2;
				if(ai2==ai)
					rm2=rm;
				else
					rm2=c.rooms[ai2];
				if(rm2!=UNALLOCATED_SPACE && rm2!=UNSPECIFIED_ROOM){
					int ind;
					if(!occupiedRoomsSet.contains(rm2)){
						occupiedRoomsSet.insert(rm2);
						occupiedRoomsList.append(rm2);
						canEmptyRoom.append(true);
						
						QSet<int> tl;
						tl.insert(ai2);
						activitiesInRoom.append(tl);
						
						ind=activitiesInRoom.count()-1;
						roomIndexInOccupiedRoomsList.insert(rm2, ind);
					}
					else{
						assert(roomIndexInOccupiedRoomsList.contains(rm2));
						ind=roomIndexInOccupiedRoomsList.value(rm2);
						assert(ind>=0 && ind<activitiesInRoom.count());
						activitiesInRoom[ind].insert(ai2);
					}
					
					if(ai2==ai || (fixedTimeActivity[ai2] && fixedSpaceActivity[ai2]) || swappedActivities[ai2])
						if(canEmptyRoom[ind]==true)
							canEmptyRoom[ind]=false;
				}
			}
			
		assert(occupiedRoomsSet.count()==item->maxDifferentRooms+1);
		
		QList<int> candidates;
		for(int rm2 : qAsConst(occupiedRoomsList)){
			assert(roomIndexInOccupiedRoomsList.contains(rm2));
			int ind=roomIndexInOccupiedRoomsList.value(rm2);
			if(canEmptyRoom.at(ind)==true)
				candidates.append(ind);
		}

		if(level==0){
			QList<int> finalCandidates;
			
			int optConflActivities=MAX_ACTIVITIES;
			int optMinWrong=INF;
			int optNWrong=INF;
			int optMinIndexAct=gt.rules.nInternalActivities;

			//phase 1
			for(int ind : qAsConst(candidates)){
				const QSet<int>& activitiesForCandidate=activitiesInRoom.at(ind);
				
				int tmp_n_confl_acts=activitiesForCandidate.count();
				int tmp_minWrong=INF;
				int tmp_nWrong=0;
				int tmp_minIndexAct=gt.rules.nInternalActivities;
				
				if(activitiesForCandidate.count()>0){
					for(int ai2 : qAsConst(activitiesForCandidate)){
						tmp_minWrong=min(tmp_minWrong, triedRemovals(ai2,c.times[ai2]));
						tmp_nWrong+=triedRemovals(ai2,c.times[ai2]);
						tmp_minIndexAct=min(tmp_minIndexAct, invPermutation[ai2]);
					}
				}
				else{
					assert(0);
					tmp_minWrong=0;
					tmp_nWrong=0;
					tmp_minIndexAct=-1;
				}
		
				if(optMinWrong>tmp_minWrong ||
				  (optMinWrong==tmp_minWrong && optNWrong>tmp_nWrong) ||
				  (optMinWrong==tmp_minWrong && optNWrong==tmp_nWrong && optConflActivities>tmp_n_confl_acts) ||
				  (optMinWrong==tmp_minWrong && optNWrong==tmp_nWrong && optConflActivities==tmp_n_confl_acts && optMinIndexAct>tmp_minIndexAct)){
					optConflActivities=tmp_n_confl_acts;
					optMinWrong=tmp_minWrong;
					optNWrong=tmp_nWrong;
					optMinIndexAct=tmp_minIndexAct;
				}
			}

			//phase 2
			for(int ind : qAsConst(candidates)){
				const QSet<int>& activitiesForCandidate=activitiesInRoom.at(ind);
				
				int tmp_n_confl_acts=activitiesForCandidate.count();
				int tmp_minWrong=INF;
				int tmp_nWrong=0;
				int tmp_minIndexAct=gt.rules.nInternalActivities;
				
				if(activitiesForCandidate.count()>0){
					for(int ai2 : qAsConst(activitiesForCandidate)){
						tmp_minWrong=min(tmp_minWrong, triedRemovals(ai2,c.times[ai2]));
						tmp_nWrong+=triedRemovals(ai2,c.times[ai2]);
						tmp_minIndexAct=min(tmp_minIndexAct, invPermutation[ai2]);
					}
				}
				else{
					assert(0);
					tmp_minWrong=0;
					tmp_nWrong=0;
					tmp_minIndexAct=-1;
				}
		
				if(optMinWrong==tmp_minWrong && optNWrong==tmp_nWrong && optConflActivities==tmp_n_confl_acts && optMinIndexAct==tmp_minIndexAct){
					finalCandidates.append(ind);
				}
			}
			
			//phase 3
			if(candidates.count()>0)
				assert(finalCandidates.count()>0);
			candidates=finalCandidates;
		}
		else{ //if(level>0)
			QList<int> finalCandidates;
			
			int optConflActivities=MAX_ACTIVITIES;

			for(int ind : qAsConst(candidates)){
				if(activitiesInRoom.at(ind).count()<optConflActivities){
					optConflActivities=activitiesInRoom.at(ind).count();
					finalCandidates.clear();
					finalCandidates.append(ind);
				}
				else if(activitiesInRoom.at(ind).count()==optConflActivities){
					finalCandidates.append(ind);
				}
			}
			
			if(candidates.count()>0)
				assert(finalCandidates.count()>0);
			candidates=finalCandidates;
		}
		
		if(candidates.count()==0)
			return false;
		
		int indexToRemove=candidates.at(rng.intMRG32k3a(candidates.count()));
		assert(canEmptyRoom.at(indexToRemove)==true);
		
		//To keep the generation identical on all computers - 2013-01-03
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
		QList<int> tmpListFromSet=QList<int>(activitiesInRoom.at(indexToRemove).constBegin(), activitiesInRoom.at(indexToRemove).constEnd());
#else
		QList<int> tmpListFromSet=activitiesInRoom.at(indexToRemove).toList();
#endif
		std::stable_sort(tmpListFromSet.begin(), tmpListFromSet.end());
		//Randomize list
		for(int i=0; i<tmpListFromSet.count(); i++){
			int t=tmpListFromSet.at(i);
			int r=rng.intMRG32k3a(tmpListFromSet.count()-i);
			tmpListFromSet[i]=tmpListFromSet[i+r];
			tmpListFromSet[i+r]=t;
		}
		
		for(int ai2 : qAsConst(tmpListFromSet)){
			assert(!globalConflActivities.contains(ai2) && !tmp_list.contains(ai2));
			assert(ai2!=ai && !(fixedTimeActivity[ai2] && fixedSpaceActivity[ai2]) && !swappedActivities[ai2]);
			tmp_list.append(ai2);
		}
	}
	
	return true;
}

//2013-09-14
inline bool Generate::checkActivitiesSameRoomIfConsecutive(const QList<int>& globalConflActivities, int rm, int ai, int d, int h, QList<int>& tmp_list)
{
	for(ActivitiesSameRoomIfConsecutive_item* item : qAsConst(asricListForActivity[ai])){
		for(int ai2 : qAsConst(item->activitiesList)){
			if(ai2!=ai && !globalConflActivities.contains(ai2) && !tmp_list.contains(ai2)){
				int rm2=c.rooms[ai2];
				if(rm2!=UNALLOCATED_SPACE && rm2!=UNSPECIFIED_ROOM){
					if(c.times[ai2]!=UNALLOCATED_TIME){
						int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
						int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
						
						if( d==d2 && (h2==h+gt.rules.internalActivitiesList[ai].duration || h==h2+gt.rules.internalActivitiesList[ai2].duration) ){
							if(rm!=rm2){ //not OK
								if(!(fixedTimeActivity[ai2] && fixedSpaceActivity[ai2]) && !swappedActivities[ai2])
									tmp_list.append(ai2);
								else
									return false;
							}
						}
					}
				}
			}
		}
	}
	
	return true;
}

inline bool Generate::chooseRoom(const QList<int>& listOfRooms, const QList<int>& globalConflActivities, int level, const Activity* act, int ai, int d, int h,
 int& roomSlot, int& selectedSlot, QList<int>& localConflActivities, QList<int>& realRoomsList /*if the selected room is virtual, these are the real rooms for sets 0..nsets-1*/)
{
	roomSlot=selectedSlot=UNSPECIFIED_ROOM; //if we don't find a room, return these values

	int optConflActivities=MAX_ACTIVITIES;
	int optMinWrong=INF;
	int optNWrong=INF;
	int optMinIndexAct=gt.rules.nInternalActivities;
	
	QList<QList<int>> conflActivitiesRooms;
	QList<int> nConflActivitiesRooms;
	QList<int> listedRooms;
	
	QList<int> minWrong;
	QList<int> nWrong;
	QList<int> minIndexAct;
	
	QList<int> tmp_list;
	int tmp_n_confl_acts;
	int tmp_minWrong;
	int tmp_nWrong;
	int tmp_minIndexAct;
	
	int newtime=d+h*gt.rules.nDaysPerWeek;
	
	QHash<int, QList<int>> realRoomsHash;
	
	for(int rm : qAsConst(listOfRooms)){
		int dur;
		for(dur=0; dur<act->duration; dur++){
			if(notAllowedRoomTimePercentages[rm][newtime+dur*gt.rules.nDaysPerWeek]>=0 &&
			 !skipRandom(notAllowedRoomTimePercentages[rm][newtime+dur*gt.rules.nDaysPerWeek]))
				break;
			
			if(haveTeacherRoomNotAllowedTimesConstraints){
				bool ok=true;
				for(int tch : qAsConst(act->iTeachersList)){
					double wp=notAllowedTeacherRoomTimePercentages.value(QPair<qint64, qint64>(teacherRoomQInt64Combination(tch, rm), dayHourQInt64Combination(d, h+dur)), -1);
					if(wp>=0 && !skipRandom(wp)){
						ok=false;
						break;
					}
				}
				if(!ok)
					break;
			}
		}
		
		if(dur==act->duration){
			tmp_list.clear();
			
			int dur2;
			if(gt.rules.internalRoomsList[rm]->isVirtual==false){
				if(gt.rules.mode!=BLOCK_PLANNING){
					for(dur2=0; dur2<act->duration; dur2++){
						int ai2=roomsTimetable(rm,d,h+dur2);
						if(ai2>=0){
							if(!globalConflActivities.contains(ai2)){
								if(swappedActivities[ai2] || (fixedTimeActivity[ai2] && fixedSpaceActivity[ai2])){
									tmp_n_confl_acts=MAX_ACTIVITIES; //not really needed
									break;
								}
								else{
									if(!tmp_list.contains(ai2)){
										tmp_list.append(ai2);
									}
								}
							}
						}
					}
				}
				else{
					//same room in real life time slots?
					for(dur2=0; dur2<act->duration; dur2++){
						int day_tch;
						for(day_tch=0; day_tch<gt.rules.nDaysPerWeek; day_tch++){
							int ai2=roomsTimetable(rm,day_tch,h+dur2);
							if(ai2>=0){
								if(!globalConflActivities.contains(ai2)){
									if(swappedActivities[ai2] || (fixedTimeActivity[ai2] && fixedSpaceActivity[ai2])){
										tmp_n_confl_acts=MAX_ACTIVITIES; //not really needed
										break;
									}
									else{
										if(!tmp_list.contains(ai2)){
											tmp_list.append(ai2);
										}
									}
								}
							}
						}
						if(day_tch<gt.rules.nDaysPerWeek)
							break;
					}
					////////////////////
				}
			}
			else{
				dur2=act->duration;
			}

			//2019-09-14, suggested by math - virtual room?
			if(dur2==act->duration){
				if(gt.rules.internalRoomsList[rm]->isVirtual==true){
					assert(tmp_list.isEmpty());
				
					const QList<QList<int>>& rrsl=gt.rules.internalRoomsList[rm]->rrsl; //real rooms sets list.
					QList<QList<int>> nrrsl; //new rooms sets list, meaning the accepted ones after checking for not available and not swapped and not fixed.
												//2019-09-22: and after checking for preferred real rooms of this activity.
					QSet<int> acceptedRoomsSet;
					QList<int> acceptedRoomsList;
					QHash<int, int> nConflictingActivitiesHash;
					QHash<int, QSet<int>> conflictingActivitiesHash;
					for(const QList<int>& tl : qAsConst(rrsl)){
						QList<int> nrs; //new rooms set
						for(int rr : qAsConst(tl)){ //real room
							if(acceptedRoomsSet.contains(rr)){
								nrs.append(rr);
								continue;
							}
						
							if(!preferredRealRooms[ai].isEmpty())
								if(!preferredRealRooms[ai].contains(rr))
									continue;
							int dur3;
							for(dur3=0; dur3<act->duration; dur3++){
								if(notAllowedRoomTimePercentages[rr][newtime+dur3*gt.rules.nDaysPerWeek]>=0 &&
								 !skipRandom(notAllowedRoomTimePercentages[rr][newtime+dur3*gt.rules.nDaysPerWeek]))
									break;

								if(haveTeacherRoomNotAllowedTimesConstraints){
									bool ok=true;
									for(int tch : qAsConst(act->iTeachersList)){
										double wp=notAllowedTeacherRoomTimePercentages.value(QPair<qint64, qint64>(teacherRoomQInt64Combination(tch, rr), dayHourQInt64Combination(d, h+dur3)), -1);
										if(wp>=0 && !skipRandom(wp)){
											ok=false;
											break;
										}
									}
									if(!ok)
										break;
								}
							}

							if(dur3==act->duration){
								QSet<int> conflActivitiesForCurrentRoom;
								int dur4;
								if(gt.rules.mode!=BLOCK_PLANNING){
									for(dur4=0; dur4<act->duration; dur4++){
										int ai2=roomsTimetable(rr,d,h+dur4);
										if(ai2>=0){
											if(!globalConflActivities.contains(ai2)){
												if(swappedActivities[ai2] || (fixedTimeActivity[ai2] && fixedSpaceActivity[ai2])){
													break;
												}
												else if(!conflActivitiesForCurrentRoom.contains(ai2)){
													conflActivitiesForCurrentRoom.insert(ai2);
												}
											}
										}
									}
								}
								else{
									//same room in real life time slots?
									for(dur4=0; dur4<act->duration; dur4++){
										int day_tch;
										for(day_tch=0; day_tch<gt.rules.nDaysPerWeek; day_tch++){
											int ai2=roomsTimetable(rr,day_tch,h+dur4);
											if(ai2>=0){
												if(!globalConflActivities.contains(ai2)){
													if(swappedActivities[ai2] || (fixedTimeActivity[ai2] && fixedSpaceActivity[ai2])){
														break;
													}
													else if(!conflActivitiesForCurrentRoom.contains(ai2)){
														conflActivitiesForCurrentRoom.insert(ai2);
													}
												}
											}
										}
										if(day_tch<gt.rules.nDaysPerWeek)
											break;
									}
									///////////////
								}
								if(dur4==act->duration){
									nrs.append(rr);
									if(!acceptedRoomsSet.contains(rr)){
										acceptedRoomsSet.insert(rr);
										
										assert(!nConflictingActivitiesHash.contains(rr));
										nConflictingActivitiesHash.insert(rr, conflActivitiesForCurrentRoom.count());
										
										if(level==0){
											assert(!conflictingActivitiesHash.contains(rr));
											conflictingActivitiesHash.insert(rr, conflActivitiesForCurrentRoom);
										}
									}
									else{
										assert(0);
									}
								}
							}
						}
						if(nrs.isEmpty()){
							dur2=0;
							break;
						}
						else{
							nrrsl.append(nrs);
						}
					}
					if(dur2==act->duration){
						assert(rrsl.count()==nrrsl.count());
						if(acceptedRoomsSet.count()<rrsl.count())
							dur2=0;
					}
					if(dur2==act->duration){
						assert(rrsl.count()==nrrsl.count());

						//Old comment (but some parts of it remain true):
						//Apply a maximum bipartite matching (Hopcroft-Karp) on the randomized bipartite graph, to get a real room from each set.
						//We have a solution if the maximum bipartite matching value == nrrsl.count() (the number of sets).
						//The graph: on the left we have the rooms, on the right we have the sets.
						//There is a connection from a room to a set if this room belongs to this set.
						//The randomization makes the order of the left side vertices random; also their adjacency lists
						//to the nodes on the right will be randomly ordered.
						//This random order of the nodes on the left and of the adjancencies will be considered when running the Hopcroft-Karp
						//maximum bipartite matching algorithm, hoping that it will return a somewhat random maximum bipartite matching out of the
						//multitude of possible solutions (a hopefully random set of chosen rooms, one room from each set, the rooms being different).
						//I could not find a proof that this should work perfectly/acceptably, but practical tests showed a good behavior.
						
						//New comment (in addition to the old one above, parts of which remain correct): we find a maximum bipartite matching
						//so that the preferred rooms are those with lowest conflicts. This is possible in O(VE) with depth first search.
					
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
						acceptedRoomsList=QList<int>(acceptedRoomsSet.constBegin(), acceptedRoomsSet.constEnd());
#else
						acceptedRoomsList=acceptedRoomsSet.toList();
#endif
						nRealRooms=acceptedRoomsList.count();
						nSets=nrrsl.count();
						NIL_NODE=nRealRooms+nSets;
						
						std::stable_sort(acceptedRoomsList.begin(), acceptedRoomsList.end()); //To keep the generation identical for the same random seed.

						for(int i=0; i<nRealRooms; i++){
							nConflictingActivitiesBipartiteMatching[i]=nConflictingActivitiesHash.value(acceptedRoomsList.at(i), -1);
							assert(nConflictingActivitiesBipartiteMatching[i]>=0);
							
							if(level==0){
								assert(conflictingActivitiesHash.contains(acceptedRoomsList.at(i)));
								conflictingActivitiesBipartiteMatching[i]=conflictingActivitiesHash.value(acceptedRoomsList.at(i), QSet<int>());
							}
						}

						//semi-randomize the order of the rooms
						QList<int> tl;
						for(int i=0; i<nRealRooms; i++)
							tl.append(i);
						for(int i=0; i<nRealRooms; i++){
							int t=tl.at(i);
							int r=rng.intMRG32k3a(nRealRooms-i);
							tl[i]=tl[i+r];
							tl[i+r]=t;
						}
						if(level>0){
							std::stable_sort(tl.begin(), tl.end(), [this](int a, int b){return compareConflictsIncreasing(a, b);});
						}
						else{
							assert(level==0);
							tmpGlobalSolutionCompareLevel0=&c;
							std::stable_sort(tl.begin(), tl.end(), [this](int a, int b){return compareConflictsIncreasingAtLevel0(a, b);});
						}
						for(int i=0; i<nRealRooms; i++)
							semiRandomPermutation[i]=tl.at(i);

						QHash<int, int> realRoomsHash;
						
						int q=0;
						for(int t : qAsConst(acceptedRoomsList)){
							realRoomsHash.insert(t, q);
							adj[q].clear();
							q++;
						}
						
						q=0;
						for(const QList<int>& tl2 : qAsConst(nrrsl)){
							for(int rr : qAsConst(tl2)){
								int tr=realRoomsHash.value(rr, -1);
								assert(tr>=0);
								
								//now add adjacency from tr (on the left, in U) to q (on the right, in V), as integers
								adj[tr].append(q+nRealRooms);
							}
							q++;
						}
						
						//randomize the adjancency of the rooms - this is not a critical step
						//(the chosen real rooms will be the same, only the order of the repartition of these real rooms
						//to the sets might change).
						for(int i=0; i<nRealRooms; i++){
							for(int j=0; j<adj[i].count()-1; j++)
								assert(adj[i].at(j)<adj[i].at(j+1));
						
							for(int j=0; j<adj[i].count(); j++){
								int t=adj[i].at(j);
								int r=rng.intMRG32k3a(adj[i].count()-j);
								adj[i][j]=adj[i][j+r];
								adj[i][j+r]=t;
							}
						}
			
						//globalLevel=level;
						int m=maximumBipartiteMatching();
						if(m!=nrrsl.count()){
							assert(m<nrrsl.count());
							dur2=0;
						}
					}
					if(dur2==act->duration){
						for(int i=nRealRooms; i<nRealRooms+nSets; i++){
							//int s=i-nRealRooms;
							int rr=acceptedRoomsList.at(pairNode[i]);
							
							//for set s we selected real room rr.
							int dur4;
							for(dur4=0; dur4<act->duration; dur4++){
								int ai2=roomsTimetable(rr,d,h+dur4);
								if(ai2>=0){
									if(!globalConflActivities.contains(ai2)){
										if(swappedActivities[ai2] || (fixedTimeActivity[ai2] && fixedSpaceActivity[ai2])){
											assert(0);
											break;
										}
										else{
											if(!tmp_list.contains(ai2)){
												tmp_list.append(ai2);
											}
										}
									}
								}
							}
							assert(dur4==act->duration);
							
							QList<int> tl=realRoomsHash.value(rm, QList<int>());
							tl.append(rr);
							realRoomsHash.insert(rm, tl);
						}
					}
				}
			}

			if(dur2==act->duration){
				bool ok=true;

				if(rm!=UNALLOCATED_SPACE && rm!=UNSPECIFIED_ROOM){
					//2019-11-14
					//check the room changes for the students and for the teachers
				
					//room changes for students
					for(int sbg : qAsConst(act->iSubgroupsList)){
						if(minGapsBetweenRoomChangesForStudentsPercentages[sbg]>=0 || maxRoomChangesPerDayForStudentsPercentages[sbg]>=0
						  || maxRoomChangesPerWeekForStudentsPercentages[sbg]>=0){
							ok=checkRoomChanges(sbg, -1, globalConflActivities, rm, level, act, ai, d, h, tmp_list);
							if(!ok)
								break;
						}
					}

					if(!ok)
						continue;
			
					//room changes for teachers
					for(int tch : qAsConst(act->iTeachersList)){
						if(minGapsBetweenRoomChangesForTeachersPercentages[tch]>=0 || maxRoomChangesPerDayForTeachersPercentages[tch]>=0
						  || maxRoomChangesPerWeekForTeachersPercentages[tch]>=0){
							ok=checkRoomChanges(-1, tch, globalConflActivities, rm, level, act, ai, d, h, tmp_list);
							if(!ok)
								break;
						}
					}

					if(!ok)
						continue;

					if(gt.rules.mode==MORNINGS_AFTERNOONS){
						//room changes per day for students
						for(int sbg : qAsConst(act->iSubgroupsList)){
							if(maxRoomChangesPerRealDayForSubgroupsPercentages[sbg]>=0){
								ok=checkRoomChangesPerRealDay(sbg, -1, globalConflActivities, rm, level, act, ai, d, h, tmp_list);
								if(!ok)
									break;
							}
						}

						if(!ok)
							continue;

						//room changes per real for teachers
						for(int tch : qAsConst(act->iTeachersList)){
							if(maxRoomChangesPerRealDayForTeachersPercentages[tch]>=0){
								ok=checkRoomChangesPerRealDay(-1, tch, globalConflActivities, rm, level, act, ai, d, h, tmp_list);
								if(!ok)
									break;
							}
						}

						if(!ok)
							continue;
					}

					assert(rm!=UNALLOCATED_SPACE && rm!=UNSPECIFIED_ROOM);
					if(gt.rules.internalRoomsList[rm]->buildingIndex!=-1){
						//check the building changes for the students and for the teachers
						
						//building changes for students
						for(int sbg : qAsConst(act->iSubgroupsList)){
							if(minGapsBetweenBuildingChangesForStudentsPercentages[sbg]>=0 || maxBuildingChangesPerDayForStudentsPercentages[sbg]>=0
							  || maxBuildingChangesPerWeekForStudentsPercentages[sbg]>=0){
								ok=checkBuildingChanges(sbg, -1, globalConflActivities, rm, level, act, ai, d, h, tmp_list);
								if(!ok)
									break;
							}
						}

						if(!ok)
							continue;
			
						//building changes for teachers
						for(int tch : qAsConst(act->iTeachersList)){
							if(minGapsBetweenBuildingChangesForTeachersPercentages[tch]>=0 || maxBuildingChangesPerDayForTeachersPercentages[tch]>=0
							  || maxBuildingChangesPerWeekForTeachersPercentages[tch]>=0){
								ok=checkBuildingChanges(-1, tch, globalConflActivities, rm, level, act, ai, d, h, tmp_list);
								if(!ok)
									break;
							}
						}

						if(!ok)
							continue;

						if(gt.rules.mode==MORNINGS_AFTERNOONS){
							//building changes per real day for students
							for(int sbg : qAsConst(act->iSubgroupsList)){
								if(maxBuildingChangesPerRealDayForSubgroupsPercentages[sbg]>=0){
									ok=checkBuildingChangesPerRealDay(sbg, -1, globalConflActivities, rm, level, act, ai, d, h, tmp_list);
									if(!ok)
										break;
								}
							}
	
							if(!ok)
								continue;
	
							//building changes per real day for teachers
							for(int tch : qAsConst(act->iTeachersList)){
								if(maxBuildingChangesPerRealDayForTeachersPercentages[tch]>=0){
									ok=checkBuildingChangesPerRealDay(-1, tch, globalConflActivities, rm, level, act, ai, d, h, tmp_list);
									if(!ok)
										break;
								}
							}
	
							if(!ok)
								continue;
						}
					}
				}

				//max occupied rooms for a set of activities - 2012-04-29
				if(aomdrListForActivity[ai].count()>0)
					ok=checkActivitiesOccupyMaxDifferentRooms(globalConflActivities, rm, level, ai, tmp_list);
				
				if(!ok)
					continue;
				
				//activities same room if consecutive - 2013-09-14
				if(asricListForActivity[ai].count()>0)
					ok=checkActivitiesSameRoomIfConsecutive(globalConflActivities, rm, ai, d, h, tmp_list);
				
				if(!ok)
					continue;
				
				tmp_minWrong=INF;
				tmp_nWrong=0;
				
				tmp_minIndexAct=gt.rules.nInternalActivities;
				
				tmp_n_confl_acts=tmp_list.count();
				
				if(level==0){
					if(tmp_list.count()>0){ //serious bug corrected on 2012-05-02, but it seems that it didn't affect the users until now
						for(int ai2 : qAsConst(tmp_list)){
							tmp_minWrong=min(tmp_minWrong, triedRemovals(ai2,c.times[ai2]));
							tmp_nWrong+=triedRemovals(ai2,c.times[ai2]);
							tmp_minIndexAct=min(tmp_minIndexAct, invPermutation[ai2]);
						}
					}
					else{
						tmp_minWrong=0;
						tmp_nWrong=0;
						tmp_minIndexAct=-1;
					}
				}
				
				listedRooms.append(rm);
				nConflActivitiesRooms.append(tmp_n_confl_acts);
				conflActivitiesRooms.append(tmp_list);
				
				if(level>0){
					if(tmp_n_confl_acts<optConflActivities)
						optConflActivities=tmp_n_confl_acts;
				}
				else{ // if(level==0)
					minWrong.append(tmp_minWrong);
					nWrong.append(tmp_nWrong);
					minIndexAct.append(tmp_minIndexAct);
	
					if(optMinWrong>tmp_minWrong ||
					  (optMinWrong==tmp_minWrong && optNWrong>tmp_nWrong) ||
					  (optMinWrong==tmp_minWrong && optNWrong==tmp_nWrong && optConflActivities>tmp_n_confl_acts) ||
					  (optMinWrong==tmp_minWrong && optNWrong==tmp_nWrong && optConflActivities==tmp_n_confl_acts && optMinIndexAct>tmp_minIndexAct)){
						optConflActivities=tmp_n_confl_acts;
						optMinWrong=tmp_minWrong;
						optNWrong=tmp_nWrong;
						optMinIndexAct=tmp_minIndexAct;
					}
				}
			}
		}
		else //not really needed
			tmp_n_confl_acts=MAX_ACTIVITIES;
	}
	
	if(optConflActivities==MAX_ACTIVITIES) //roomSlot == selectedSlot == UNSPECIFIED_ROOM
		return false;
	
	assert(optConflActivities<MAX_ACTIVITIES);
	
	QList<int> allowedRoomsIndex;

	assert(listedRooms.count()==nConflActivitiesRooms.count());
	assert(listedRooms.count()==conflActivitiesRooms.count());
	
	if(level>0){
		for(int q=0; q<listedRooms.count(); q++){
			//The 'mapr' custom version disables the test below and always does an .append(q). But this
			//gives a very bad behavior for the file examples/Egypt/Dakahlia/institution-2012-2013.fet
			//(usually, the file seems not to solve anymore, and one time it solved very slowly).
			//Update 2016-08-26: Also mapr for zt3's input is working much better with a code _with_ the test below
			//(which does not always .append(q) ).
			if(nConflActivitiesRooms.at(q)==optConflActivities){
				allowedRoomsIndex.append(q);
			}
		}
	}
	else{
		for(int q=0; q<listedRooms.count(); q++){
			if(optMinWrong==minWrong.at(q) && optNWrong==nWrong.at(q) && optConflActivities==nConflActivitiesRooms.at(q) && optMinIndexAct==minIndexAct.at(q)){
				allowedRoomsIndex.append(q);
			}
		}
		/*if(allowedRoomsIndex.count()!=1)
			cout<<"allowedRoomsIndex.count()=="<<allowedRoomsIndex.count()<<endl;
		assert(allowedRoomsIndex.count()==1);*/
	}
	
	assert(allowedRoomsIndex.count()>0);
	int q=rng.intMRG32k3a(allowedRoomsIndex.count());
	int t=allowedRoomsIndex.at(q);
	assert(t>=0 && t<listedRooms.count());
	int r=listedRooms.at(t);
	assert(r>=0 && r<gt.rules.nInternalRooms);
	selectedSlot=r;
	roomSlot=r;
	realRoomsList=realRoomsHash.value(r, QList<int>());
	
	assert(nConflActivitiesRooms.at(t)==conflActivitiesRooms.at(t).count());
	
	localConflActivities.clear(); /////Liviu: added 22 August 2008 (modified 22 May 2020): bug fix, which might have been a crash bug or a non-observable bug.
									//Liviu, 19 May 2020: If I remember correctly, the explanation was: because in the function getRoom() below we might call
									//the function getPreferredRoom() and after that the function getHomeRoom() without clearing the list localConflActivities
									//in between the calls, more precisely if okp is true and localConflActivities.count()>0 after calling getPreferredRoom().
									//22 May 2020: If localConflActivities is not emptied between the call to getPreferredRoom() and getHomeRoom(),
									//we would get a crash bug (if localConflActivities would contain duplicates) or a non-observable bug (otherwise).
	
	for(int a : qAsConst(conflActivitiesRooms.at(t))){
		assert(!globalConflActivities.contains(a));
		assert(!localConflActivities.contains(a)); ///////////Liviu: added 22 August 2008.
		localConflActivities.append(a);
	}
	
	return true;
}

inline bool Generate::getHomeRoom(const QList<int>& globalConflActivities, int level, const Activity* act, int ai, int d, int h,
 int& roomSlot, int& selectedSlot, QList<int>& localConflActivities, QList<int>& realRoomsList)
{
	assert(!unspecifiedHomeRoom[ai]);

	return chooseRoom(activitiesHomeRoomsHomeRooms[ai], globalConflActivities, level, act, ai, d, h, roomSlot, selectedSlot, localConflActivities, realRoomsList);
}

inline bool Generate::getPreferredRoom(const QList<int>& globalConflActivities, int level, const Activity* act, int ai, int d, int h,
 int& roomSlot, int& selectedSlot, QList<int>& localConflActivities, bool& canBeUnspecifiedPreferredRoom, QList<int>& realRoomsList)
{
	assert(!unspecifiedPreferredRoom[ai]);
	
	bool unspecifiedRoom=true;
	QSet<int> allowedRooms;
	for(const PreferredRoomsItem& it : qAsConst(activitiesPreferredRoomsList[ai])){
		bool skip=skipRandom(it.percentage);
		
		if(!skip){
			if(unspecifiedRoom){
				unspecifiedRoom=false;
				allowedRooms=it.preferredRooms;
			}
			else{
				allowedRooms.intersect(it.preferredRooms);
			}
		}
		else{
			if(unspecifiedRoom){
				allowedRooms.unite(it.preferredRooms);
			}
			else{
				//do nothing
			}
		}
	}
	
	QList<int> allowedRoomsList;
	for(int rm : qAsConst(allowedRooms))
		allowedRoomsList.append(rm);
	std::stable_sort(allowedRoomsList.begin(), allowedRoomsList.end()); //To keep the generation identical on all computers - 2013-01-03
	//Randomize list
	for(int i=0; i<allowedRoomsList.count(); i++){
		int t=allowedRoomsList.at(i);
		int r=rng.intMRG32k3a(allowedRoomsList.count()-i);
		allowedRoomsList[i]=allowedRoomsList[i+r];
		allowedRoomsList[i+r]=t;
	}
	
	canBeUnspecifiedPreferredRoom=unspecifiedRoom;

	return chooseRoom(allowedRoomsList, globalConflActivities, level, act, ai, d, h, roomSlot, selectedSlot, localConflActivities, realRoomsList);
}

inline bool Generate::getRoom(int level, const Activity* act, int ai, int d, int h,
 int& roomSlot, int& selectedSlot, QList<int>& conflActivities, int& nConflActivities, QList<int>& realRoomsList)
{
	bool okh;
	
	QList<int> localConflActivities;
	
	if(unspecifiedPreferredRoom[ai]){
		if(unspecifiedHomeRoom[ai]){
			roomSlot=UNSPECIFIED_ROOM;
			selectedSlot=UNSPECIFIED_ROOM;
			return true;
		}
		else{
			okh=getHomeRoom(conflActivities, level, act, ai, d, h, roomSlot, selectedSlot, localConflActivities, realRoomsList);
			if(okh){
				for(int t : qAsConst(localConflActivities)){
					conflActivities.append(t);
					nConflActivities++;
				}
				return okh;
			}
			else{
				okh=skipRandom(activitiesHomeRoomsPercentage[ai]);
				return okh;
			}
		}
	}
	else{
		bool okp;
		bool canBeUnspecifiedPreferredRoom;
	
		okp=getPreferredRoom(conflActivities, level, act, ai, d, h, roomSlot, selectedSlot, localConflActivities, canBeUnspecifiedPreferredRoom, realRoomsList);
		if(okp && localConflActivities.count()==0){
			return okp;
		}
		else if(okp){
			if(canBeUnspecifiedPreferredRoom){ //skipRandom(activitiesPreferredRoomsPercentage[ai])){
				//get a home room
				if(unspecifiedHomeRoom[ai]){
					roomSlot=UNSPECIFIED_ROOM;
					selectedSlot=UNSPECIFIED_ROOM;
					return true;
				}
				
				okh=getHomeRoom(conflActivities, level, act, ai, d, h, roomSlot, selectedSlot, localConflActivities, realRoomsList);
				if(okh){
					for(int t : qAsConst(localConflActivities)){
						conflActivities.append(t);
						nConflActivities++;
					}
					return okh;
				}
				else{
					okh=skipRandom(activitiesHomeRoomsPercentage[ai]);
					return okh;
				}
			}
			else{
				for(int t : qAsConst(localConflActivities)){
					conflActivities.append(t);
					nConflActivities++;
				}
				assert(nConflActivities==conflActivities.count());
				assert(okp==true);
				return okp;
				//get this preferred room
			}
		}
		else{ //!ok from preferred room, search a home room
			if(canBeUnspecifiedPreferredRoom){ //skipRandom(activitiesPreferredRoomsPercentage[ai])){
				//get a home room
				if(unspecifiedHomeRoom[ai]){
					roomSlot=UNSPECIFIED_ROOM;
					selectedSlot=UNSPECIFIED_ROOM;
					return true;
				}
				
				okh=getHomeRoom(conflActivities, level, act, ai, d, h, roomSlot, selectedSlot, localConflActivities, realRoomsList);
				if(okh){
					for(int t : qAsConst(localConflActivities)){
						conflActivities.append(t);
						nConflActivities++;
					}
					return okh;
				}
				else{
					okh=skipRandom(activitiesHomeRoomsPercentage[ai]);
					return okh;
				}
			}
			else{
				assert(okp==false);
				return okp;
			}
		}
	}
}

void Generate::generateWithSemaphore(int maxSeconds, bool& impossible, bool& timeExceeded, bool threaded, QTextStream* maxPlacedActivityStream)
{
	isRunning=true;
	generate(maxSeconds, impossible, timeExceeded, threaded, maxPlacedActivityStream);
	semaphoreFinished.release();
	isRunning=false;
}

void Generate::generate(int maxSeconds, bool& impossible, bool& timeExceeded, bool threaded, QTextStream* maxPlacedActivityStream)
{
	permutation.resize(gt.rules.nInternalActivities);
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		permutation[i]=copyOfInitialPermutation[i];

	this->isThreaded=threaded;
	
	//2019-09-14 - begin for the maximum bipartite matching algorithm
	int j=0; //the number of real rooms
	int k=0; //the maximum number of sets for a virtual room
	for(int i=0; i<gt.rules.nInternalRooms; i++){
		if(gt.rules.internalRoomsList[i]->isVirtual==false){
			j++;
		}
		else{
			if(k<gt.rules.internalRoomsList[i]->rrsl.count()){
				k=gt.rules.internalRoomsList[i]->rrsl.count();
			}
		}
	}
	if(k>0)
		assert(j>0);
	if(j<gt.rules.nInternalRooms)
		assert(k>0);
	if(j>0 && k>0){
		adj.resize(j);
		visited.resize(j);
		//dist.resize(j+k+1); //+1 for the NIL_NODE, +k even if dist for them is not used, but so that dist[NIL_NODE] is accessible, since NIL_NODE can be maximum j+k.
		semiRandomPermutation.resize(j);
		pairNode.resize(j+k); //be careful: even if we would get rid of the assignment and checking of pairNode[u], we need to keep this array's size = j+k
		nConflictingActivitiesBipartiteMatching.resize(j);
		conflictingActivitiesBipartiteMatching.resize(j);
	}
	//end - for the maximum bipartite matching algorithm

	//2021-03-17
	difficultActivities.resize(gt.rules.nInternalActivities);
	//
	activities.resize(gt.rules.nHoursPerDay);
	rooms.resize(gt.rules.nHoursPerDay);
	buildings.resize(gt.rules.nHoursPerDay);
	activitiesx2.resize(2*gt.rules.nHoursPerDay);
	roomsx2.resize(2*gt.rules.nHoursPerDay);
	buildingsx2.resize(2*gt.rules.nHoursPerDay);
	weekBuildings.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	weekActivities.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	weekRooms.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	aminoCnt.resize(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	aminsCnt.resize(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	possibleToEmptySlot.resize(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	//
	occupiedDay.resize(gt.rules.nDaysPerWeek);
	canEmptyDay.resize(gt.rules.nDaysPerWeek);
	_nConflActivities.resize(gt.rules.nDaysPerWeek);
	_activitiesForDay.resize(gt.rules.nDaysPerWeek);
	_minWrong.resize(gt.rules.nDaysPerWeek);
	_nWrong.resize(gt.rules.nDaysPerWeek);
	_minIndexAct.resize(gt.rules.nDaysPerWeek);
	occupiedIntervalDay.resize(gt.rules.nDaysPerWeek);
	canEmptyIntervalDay.resize(gt.rules.nDaysPerWeek);
	_activitiesForIntervalDay.resize(gt.rules.nDaysPerWeek);
	sbgDayNHoursWithTag.resize(gt.rules.nDaysPerWeek);
	possibleToEmptyDay.resize(gt.rules.nDaysPerWeek);
	tchDayNHoursWithTag.resize(gt.rules.nDaysPerWeek);
	//
	assert(gt.rules.nTerms>=1);
	assert(gt.rules.nTerms<=MAX_DAYS_PER_WEEK);
	canEmptyTerm.resize(gt.rules.nTerms);
	termActivities.resize(gt.rules.nTerms);
	//
	swappedActivities.resize(gt.rules.nInternalActivities);
	restoreActIndex.resize(2*gt.rules.nInternalActivities);
	restoreTime.resize(2*gt.rules.nInternalActivities);
	restoreRoom.resize(2*gt.rules.nInternalActivities);
	restoreRealRoomsList.resize(2*gt.rules.nInternalActivities);
	invPermutation.resize(gt.rules.nInternalActivities);
	//
	nMinDaysBrokenL.resize(MAX_LEVEL, gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	selectedRoomL.resize(MAX_LEVEL, gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	permL.resize(MAX_LEVEL, gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	conflActivitiesL.resize(MAX_LEVEL, gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	nConflActivitiesL.resize(MAX_LEVEL, gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	roomSlotsL.resize(MAX_LEVEL, gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	realRoomsListL.resize(MAX_LEVEL, gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	//
	slotActivity.resize(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	////////////

	l0nWrong.resize(gt.rules.nHoursPerWeek);
	l0minWrong.resize(gt.rules.nHoursPerWeek);
	l0minIndexAct.resize(gt.rules.nHoursPerWeek);

	teachersTimetable.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	subgroupsTimetable.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	roomsTimetable.resize(gt.rules.nInternalRooms, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);

	newTeachersTimetable.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	newSubgroupsTimetable.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	newTeachersDayNHours.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek);
	newTeachersDayNGaps.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek);
	newTeachersDayNFirstGaps.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek);
	newSubgroupsDayNHours.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek);
	newSubgroupsDayNGaps.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek);
	newSubgroupsDayNFirstGaps.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek);

	newTeachersRealDayNHours.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek/2);
	newTeachersRealDayNGaps.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek/2);

	newSubgroupsRealDayNHours.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek/2);
	newSubgroupsRealDayNGaps.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek/2);
	newSubgroupsRealDayNFirstGaps.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek/2);

	oldTeachersTimetable.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	oldSubgroupsTimetable.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	oldTeachersDayNHours.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek);
	oldTeachersDayNGaps.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek);
	oldTeachersDayNFirstGaps.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek);
	oldSubgroupsDayNHours.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek);
	oldSubgroupsDayNGaps.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek);
	oldSubgroupsDayNFirstGaps.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek);

	oldTeachersRealDayNHours.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek/2);
	oldTeachersRealDayNGaps.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek/2);

	oldSubgroupsRealDayNHours.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek/2);
	oldSubgroupsRealDayNGaps.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek/2);
	oldSubgroupsRealDayNFirstGaps.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek/2);

	tchTimetable.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	tchDayNHours.resize(gt.rules.nDaysPerWeek);
	tchDayNGaps.resize(gt.rules.nDaysPerWeek);
	tchDayNFirstGaps.resize(gt.rules.nDaysPerWeek);

	tchRealDayNHours.resize(gt.rules.nDaysPerWeek/2);
	tchRealDayNGaps.resize(gt.rules.nDaysPerWeek/2);

	sbgTimetable.resize(gt.rules.nDaysPerWeek, gt.rules.nHoursPerDay);
	sbgDayNHours.resize(gt.rules.nDaysPerWeek);
	sbgDayNGaps.resize(gt.rules.nDaysPerWeek);
	sbgDayNFirstGaps.resize(gt.rules.nDaysPerWeek);

	sbgRealDayNHours.resize(gt.rules.nDaysPerWeek/2);
	sbgRealDayNGaps.resize(gt.rules.nDaysPerWeek/2);
	sbgRealDayNFirstGaps.resize(gt.rules.nDaysPerWeek/2);

	teacherActivitiesOfTheDay.resize(gt.rules.nInternalTeachers, gt.rules.nDaysPerWeek);
	subgroupActivitiesOfTheDay.resize(gt.rules.nInternalSubgroups, gt.rules.nDaysPerWeek);
	
	//2011-09-30
	if(haveActivitiesOccupyMaxConstraints || haveActivitiesMaxSimultaneousConstraints){
		activitiesAtTime.resize(gt.rules.nHoursPerWeek);

		slotSetOfActivities.resize(gt.rules.nHoursPerWeek);
		slotCanEmpty.resize(gt.rules.nHoursPerWeek);
	}

	if(threaded){
		myMutex.lock();
	}
	//isRunning=true;
	c.makeUnallocated(gt.rules);
	
	nDifficultActivities=0;

	impossible=false;
	timeExceeded=false;

	impossibleActivity=false;
	
	maxActivitiesPlaced=0;

	if(threaded){
		myMutex.unlock();
	}

	triedRemovals.resize(gt.rules.nInternalActivities, gt.rules.nHoursPerWeek);
	for(int i=0; i<gt.rules.nInternalActivities; i++)
		for(int j=0; j<gt.rules.nHoursPerWeek; j++)
			triedRemovals(i,j)=0;
			
	////////init tabu
	tabu_size=gt.rules.nInternalActivities*gt.rules.nHoursPerWeek;
	//assert(tabu_size<=MAX_TABU);
	crt_tabu_index=0;
	/*qint16 tabu_activities[MAX_TABU];
	qint16 tabu_times[MAX_TABU];*/
	tabu_activities.resize(tabu_size);
	tabu_times.resize(tabu_size);
	for(int i=0; i<tabu_size; i++)
		tabu_activities[i]=tabu_times[i]=-1;
	/////////////////

	//abortOptimization=false; you have to take care of this before calling this function

	for(int i=0; i<gt.rules.nInternalActivities; i++)
		invPermutation[permutation[i]]=i;

	for(int i=0; i<gt.rules.nInternalActivities; i++)
		swappedActivities[permutation[i]]=false;

	//time_t starting_time;
	time(&starting_time);
	
	if(threaded){
		myMutex.lock();
	}
	timeToHighestStage=0;
	searchTime=0;
	if(threaded){
		myMutex.unlock();
	}
	
	limitcallsrandomswap=2*gt.rules.nInternalActivities; //Value found practically
	
	level_limit=14;
	
	assert(level_limit<=MAX_LEVEL);
	
	for(int added_act=0; added_act<gt.rules.nInternalActivities; added_act++){
		prevvalue:
		
		if(abortOptimization)
			return;

		if(threaded){
			myMutex.lock();
		}
		/*if(abortOptimization){
			//isRunning=false;
			
			if(threaded){
				myMutex.unlock();
			}
			return;
		}*/
		time_t crt_time;
		time(&crt_time);
		searchTime=int(difftime(crt_time, starting_time));
		
		if(searchTime>=maxSeconds){
			//isRunning=false;

			timeExceeded=true;
			
			if(threaded){
				myMutex.unlock();
			}
			
			return;
		}

		for(int i=0; i<=added_act; i++)
			swappedActivities[permutation[i]]=false;
		for(int i=added_act+1; i<gt.rules.nInternalActivities; i++)
			assert(!swappedActivities[permutation[i]]);

		if(VERBOSE){
			cout<<endl<<"Trying to place activity number added_act=="<<added_act<<
			 "\nwith id=="<<gt.rules.internalActivitiesList[permutation[added_act]].id<<
			 ", from nInternalActivities=="<<gt.rules.nInternalActivities<<endl;
		}
		//verifyUnallocated(permutation[added_act]]);
		//assert(c.times[permutation[added_act]]==UNALLOCATED_TIME);
		//assert(c.rooms[permutation[added_act]]==UNALLOCATED_SPACE);
		if(fixedTimeActivity[permutation[added_act]] && fixedSpaceActivity[permutation[added_act]]){
			assert(c.times[permutation[added_act]]==UNALLOCATED_TIME);
			assert(c.rooms[permutation[added_act]]==UNALLOCATED_SPACE);
		}
		else if(fixedTimeActivity[permutation[added_act]] && !fixedSpaceActivity[permutation[added_act]]){
			assert(c.rooms[permutation[added_act]]==UNALLOCATED_SPACE);
		}
		else if(!fixedTimeActivity[permutation[added_act]]){
			assert(c.times[permutation[added_act]]==UNALLOCATED_TIME);
			assert(c.rooms[permutation[added_act]]==UNALLOCATED_SPACE);
		}
		else
			assert(0);

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
					roomsTimetable(i,j,k)=-1;
		for(int j=0; j<added_act; j++){
			int i=permutation[j];
			assert(c.rooms[i]!=UNALLOCATED_SPACE);
			if(c.rooms[i]!=UNSPECIFIED_ROOM){
				int rm=c.rooms[i];
			
				Activity* act=&gt.rules.internalActivitiesList[i];
				int hour=c.times[i]/gt.rules.nDaysPerWeek;
				int day=c.times[i]%gt.rules.nDaysPerWeek;
				for(int dd=0; dd<act->duration && hour+dd<gt.rules.nHoursPerDay; dd++){
					assert(roomsTimetable(rm,day,hour+dd)==-1);
					if(gt.rules.internalRoomsList[rm]->isVirtual==false)
						roomsTimetable(rm,day,hour+dd)=i;
				}

				if(gt.rules.internalRoomsList[rm]->isVirtual){
					assert(gt.rules.internalRoomsList[rm]->rrsl.count()==c.realRoomsList[i].count());
					
					int k=0;
					for(int rr : qAsConst(c.realRoomsList[i])){
						assert(gt.rules.internalRoomsList[rm]->rrsl.at(k).contains(rr));
						
						for(int dd=0; dd<act->duration; dd++){
							assert(hour+dd<gt.rules.nHoursPerDay);
	
							assert(rr!=UNALLOCATED_SPACE);
							assert(roomsTimetable(rr,day,hour+dd)==-1);
							roomsTimetable(rr,day,hour+dd)=i;
						}
						
						k++;
					}
				}
			}
		}
		///////////////////////////////
		
		//subgroups' timetable
		for(int i=0; i<gt.rules.nInternalSubgroups; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				for(int k=0; k<gt.rules.nHoursPerDay; k++){
					subgroupsTimetable(i,j,k)=-1;
				}
		for(int j=0; j<gt.rules.nInternalActivities/*added_act*/; j++){
			int i=permutation[j];
			if(j<added_act){
				assert(c.times[i]!=UNALLOCATED_TIME);
			}
			else{
				if(c.times[i]==UNALLOCATED_TIME)
					continue;
			}
			assert(c.times[i]!=UNALLOCATED_TIME);
			Activity* act=&gt.rules.internalActivitiesList[i];
			int hour=c.times[i]/gt.rules.nDaysPerWeek;
			int day=c.times[i]%gt.rules.nDaysPerWeek;
			for(int sb : qAsConst(act->iSubgroupsList)){
				for(int dd=0; dd<act->duration && hour+dd<gt.rules.nHoursPerDay; dd++){
					assert(subgroupsTimetable(sb,day,hour+dd)==-1);
					subgroupsTimetable(sb,day,hour+dd)=i;
				}
			}
		}

		//new
		for(int i=0; i<gt.rules.nInternalSubgroups; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				for(int k=0; k<gt.rules.nHoursPerDay; k++){
					newSubgroupsTimetable(i,j,k)=-1;
				}
		for(int j=0; j<gt.rules.nInternalActivities/*added_act*/; j++){
			int i=permutation[j];
			if(j<added_act){
				assert(c.times[i]!=UNALLOCATED_TIME);
			}
			else{
				if(c.times[i]==UNALLOCATED_TIME)
					continue;
			}
			assert(c.times[i]!=UNALLOCATED_TIME);
			Activity* act=&gt.rules.internalActivitiesList[i];
			int hour=c.times[i]/gt.rules.nDaysPerWeek;
			int day=c.times[i]%gt.rules.nDaysPerWeek;
			for(int sb : qAsConst(act->iSubgroupsList)){
				for(int dd=0; dd<act->duration && hour+dd<gt.rules.nHoursPerDay; dd++){
					assert(newSubgroupsTimetable(sb,day,hour+dd)==-1);
					newSubgroupsTimetable(sb,day,hour+dd)=i;
				}
			}
		}

		for(int i=0; i<gt.rules.nInternalSubgroups; i++)
			subgroupGetNHoursGaps(i);

		//////////////care for students max days/mornings/afternoons per week
		for(int i=0; i<gt.rules.nInternalSubgroups; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				subgroupActivitiesOfTheDay(i,j).clear();
		
		for(int i=0; i<gt.rules.nInternalActivities/*added_act*/; i++){
			if(i<added_act){
			}
			else{
				if(c.times[permutation[i]]==UNALLOCATED_TIME)
					continue;
			}
			//Activity* act=&gt.rules.internalActivitiesList[permutation[i]];
			int d=c.times[permutation[i]]%gt.rules.nDaysPerWeek;
			
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				for(int j : qAsConst(subgroupsWithMaxDaysPerWeekForActivities[permutation[i]])){
					assert(subgroupActivitiesOfTheDay(j,d).indexOf(permutation[i])==-1);
					subgroupActivitiesOfTheDay(j,d).append(permutation[i]);
				}
			}
			else{
				/*
				Consider also N1N2N3, as for teachers
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
				QSet<int> st_smhd=QSet<int>(subgroupsWithMaxDaysPerWeekForActivities[permutation[i]].constBegin(), subgroupsWithMaxDaysPerWeekForActivities[permutation[i]].constEnd());
				QSet<int> st_smtd=QSet<int>(subgroupsWithMaxThreeConsecutiveDaysForActivities[permutation[i]].constBegin(), subgroupsWithMaxThreeConsecutiveDaysForActivities[permutation[i]].constEnd());
				QSet<int> st_smd=QSet<int>(subgroupsWithMaxRealDaysPerWeekForActivities[permutation[i]].constBegin(), subgroupsWithMaxRealDaysPerWeekForActivities[permutation[i]].constEnd());
				QSet<int> st_sma=QSet<int>(subgroupsWithMaxAfternoonsPerWeekForActivities[permutation[i]].constBegin(), subgroupsWithMaxAfternoonsPerWeekForActivities[permutation[i]].constEnd());
				QSet<int> st_smm=QSet<int>(subgroupsWithMaxMorningsPerWeekForActivities[permutation[i]].constBegin(), subgroupsWithMaxMorningsPerWeekForActivities[permutation[i]].constEnd());
#else
				QSet<int> st_smhd=subgroupsWithMaxDaysPerWeekForActivities[permutation[i]].toSet();
				QSet<int> st_smtd=subgroupsWithMaxThreeConsecutiveDaysForActivities[permutation[i]].toSet();
				QSet<int> st_smd=subgroupsWithMaxRealDaysPerWeekForActivities[permutation[i]].toSet();
				QSet<int> st_sma=subgroupsWithMaxAfternoonsPerWeekForActivities[permutation[i]].toSet();
				QSet<int> st_smm=subgroupsWithMaxMorningsPerWeekForActivities[permutation[i]].toSet();
#endif
				QSet<int> st_smda=st_smhd+st_smtd+st_smd+st_sma;
				QSet<int> st_smdm=st_smhd+st_smtd+st_smd+st_smm;
				*/

				//speed improvement on 2022-02-16
				if(d%2==1){
					for(int j : qAsConst(/*st_smda*/ subgroupsForActivitiesOfTheDayAfternoons[permutation[i]])){
						assert(subgroupActivitiesOfTheDay(j,d).indexOf(permutation[i])==-1);
						subgroupActivitiesOfTheDay(j,d).append(permutation[i]);
					}
				}
				else{
					for(int j : qAsConst(/*st_smdm*/ subgroupsForActivitiesOfTheDayMornings[permutation[i]])){
						assert(subgroupActivitiesOfTheDay(j,d).indexOf(permutation[i])==-1);
						subgroupActivitiesOfTheDay(j,d).append(permutation[i]);
					}
				}
			}
		}
		//////////////

		//teachers' timetable
		for(int i=0; i<gt.rules.nInternalTeachers; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				for(int k=0; k<gt.rules.nHoursPerDay; k++){
					teachersTimetable(i,j,k)=-1;
				}
		for(int j=0; j<gt.rules.nInternalActivities/*added_act*/; j++){
			int i=permutation[j];
			if(j<added_act){
				assert(c.times[i]!=UNALLOCATED_TIME);
			}
			else{
				if(c.times[i]==UNALLOCATED_TIME)
					continue;
			}
			assert(c.times[i]!=UNALLOCATED_TIME);
			Activity* act=&gt.rules.internalActivitiesList[i];
			int hour=c.times[i]/gt.rules.nDaysPerWeek;
			int day=c.times[i]%gt.rules.nDaysPerWeek;
			for(int tc : qAsConst(act->iTeachersList)){
				for(int dd=0; dd<act->duration && hour+dd<gt.rules.nHoursPerDay; dd++){
					assert(teachersTimetable(tc,day,hour+dd)==-1);
					teachersTimetable(tc,day,hour+dd)=i;
				}
			}
		}

		//new
		for(int i=0; i<gt.rules.nInternalTeachers; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				for(int k=0; k<gt.rules.nHoursPerDay; k++){
					newTeachersTimetable(i,j,k)=-1;
				}
		for(int j=0; j<gt.rules.nInternalActivities/*added_act*/; j++){
			int i=permutation[j];
			if(j<added_act){
				assert(c.times[i]!=UNALLOCATED_TIME);
			}
			else{
				if(c.times[i]==UNALLOCATED_TIME)
					continue;
			}
			assert(c.times[i]!=UNALLOCATED_TIME);
			Activity* act=&gt.rules.internalActivitiesList[i];
			int hour=c.times[i]/gt.rules.nDaysPerWeek;
			int day=c.times[i]%gt.rules.nDaysPerWeek;
			for(int tc : qAsConst(act->iTeachersList)){
				for(int dd=0; dd<act->duration && hour+dd<gt.rules.nHoursPerDay; dd++){
					assert(newTeachersTimetable(tc,day,hour+dd)==-1);
					newTeachersTimetable(tc,day,hour+dd)=i;
				}
			}
		}

		for(int i=0; i<gt.rules.nInternalTeachers; i++)
			teacherGetNHoursGaps(i);
		
		//////////////care for teachers max days/afternoons/mornings per week
		for(int i=0; i<gt.rules.nInternalTeachers; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				teacherActivitiesOfTheDay(i,j).clear();
		
		for(int i=0; i<gt.rules.nInternalActivities/*added_act*/; i++){
			if(i<added_act){
			}
			else{
				if(c.times[permutation[i]]==UNALLOCATED_TIME)
					continue;
			}
			//Activity* act=&gt.rules.internalActivitiesList[permutation[i]];
			int d=c.times[permutation[i]]%gt.rules.nDaysPerWeek;
			
			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				for(int j : qAsConst(teachersWithMaxDaysPerWeekForActivities[permutation[i]])){
					assert(teacherActivitiesOfTheDay(j,d).indexOf(permutation[i])==-1);
					teacherActivitiesOfTheDay(j,d).append(permutation[i]);
				}
			}
			else{
				/*
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
				QSet<int> smhd=QSet<int>(teachersWithMaxDaysPerWeekForActivities[permutation[i]].constBegin(), teachersWithMaxDaysPerWeekForActivities[permutation[i]].constEnd());
				QSet<int> smtd=QSet<int>(teachersWithMaxThreeConsecutiveDaysForActivities[permutation[i]].constBegin(), teachersWithMaxThreeConsecutiveDaysForActivities[permutation[i]].constEnd());
				QSet<int> smd=QSet<int>(teachersWithMaxRealDaysPerWeekForActivities[permutation[i]].constBegin(), teachersWithMaxRealDaysPerWeekForActivities[permutation[i]].constEnd());
				QSet<int> smn1n2n3=QSet<int>(teachersWithN1N2N3ForActivities[permutation[i]].constBegin(), teachersWithN1N2N3ForActivities[permutation[i]].constEnd());
				QSet<int> sma=QSet<int>(teachersWithMaxAfternoonsPerWeekForActivities[permutation[i]].constBegin(), teachersWithMaxAfternoonsPerWeekForActivities[permutation[i]].constEnd());
				QSet<int> smm=QSet<int>(teachersWithMaxMorningsPerWeekForActivities[permutation[i]].constBegin(), teachersWithMaxMorningsPerWeekForActivities[permutation[i]].constEnd());
#else
				QSet<int> smhd=teachersWithMaxDaysPerWeekForActivities[permutation[i]].toSet();
				QSet<int> smtd=teachersWithMaxThreeConsecutiveDaysForActivities[permutation[i]].toSet();
				QSet<int> smd=teachersWithMaxRealDaysPerWeekForActivities[permutation[i]].toSet();
				QSet<int> smn1n2n3=teachersWithN1N2N3ForActivities[permutation[i]].toSet();
				QSet<int> sma=teachersWithMaxAfternoonsPerWeekForActivities[permutation[i]].toSet();
				QSet<int> smm=teachersWithMaxMorningsPerWeekForActivities[permutation[i]].toSet();
#endif
				QSet<int> smda=smhd+smtd+smd+sma+smn1n2n3;
				QSet<int> smdm=smhd+smtd+smd+smm+smn1n2n3;
				*/

				//speed improvement on 2022-02-16
				if(d%2==1){
					for(int j : qAsConst(/*smda*/ teachersForActivitiesOfTheDayAfternoons[permutation[i]])){
						assert(teacherActivitiesOfTheDay(j,d).indexOf(permutation[i])==-1);
						teacherActivitiesOfTheDay(j,d).append(permutation[i]);
					}
				}
				else{
					for(int j : qAsConst(/*smdm*/ teachersForActivitiesOfTheDayMornings[permutation[i]])){
						assert(teacherActivitiesOfTheDay(j,d).indexOf(permutation[i])==-1);
						teacherActivitiesOfTheDay(j,d).append(permutation[i]);
					}
				}
			}
		}
		//////////////
		
		//2011-09-30
		if(haveActivitiesOccupyMaxConstraints || haveActivitiesMaxSimultaneousConstraints){
			for(int t=0; t<gt.rules.nHoursPerWeek; t++)
				activitiesAtTime[t].clear();
	
			for(int j=0; j<gt.rules.nInternalActivities/*added_act*/; j++){
				int i=permutation[j];
				
				if(!activityHasOccupyMaxConstraints[i] && !activityHasMaxSimultaneousConstraints[i])
					continue;
				
				if(j<added_act){
					assert(c.times[i]!=UNALLOCATED_TIME);
				}
				else{
					if(c.times[i]==UNALLOCATED_TIME)
						continue;
				}
				assert(c.times[i]!=UNALLOCATED_TIME);
				
				Activity* act=&gt.rules.internalActivitiesList[i];
			
				for(int t=c.times[i]; t<c.times[i]+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
					assert(!activitiesAtTime[t].contains(i));
					activitiesAtTime[t].insert(i);
				}
			}
		}
		//////////////
		
		foundGoodSwap=false;
	
		assert(!swappedActivities[permutation[added_act]]);
		swappedActivities[permutation[added_act]]=true;

		nRestore=0;
		ncallsrandomswap=0;
		currentlyNPlacedActivities=added_act;
		randomSwap(permutation[added_act], 0);
		
		if(!foundGoodSwap){
			if(abortOptimization){
				if(threaded){
					myMutex.unlock();
				}
				
				return;
			}
			if(impossibleActivity){
				//isRunning=false;

				nDifficultActivities=1;
				difficultActivities[0]=permutation[added_act];
				
				impossible=true;
				
				if(threaded){
					myMutex.unlock();
				}

				emit(impossibleToSolve());
				
				return;
			}
		
			//update difficult activities (activities which are placed correctly so far, together with added_act)
			nDifficultActivities=added_act+1;
			if(VERBOSE){
				cout<<"nDifficultActivities=="<<nDifficultActivities<<endl;
			}
			for(int j=0; j<=added_act; j++)
				difficultActivities[j]=permutation[j];
			
//////////////////////
			assert(conflActivitiesTimeSlot.count()>0);
			
			if(VERBOSE){
				cout<<"conflActivitiesTimeSlot.count()=="<<conflActivitiesTimeSlot.count()<<endl;
				for(int i : qAsConst(conflActivitiesTimeSlot)){
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
			}

			QList<int> ok;
			QList<int> confl;
			for(int j=0; j<added_act; j++){
				if(conflActivitiesTimeSlot.indexOf(permutation[j])!=-1){
					if(VERBOSE){
						if(triedRemovals(permutation[j],c.times[permutation[j]])>0){
							cout<<"Warning - explored removal: id=="<<
							 gt.rules.internalActivitiesList[permutation[j]].id<<", time=="<<c.times[permutation[j]]
							 <<", times=="<<triedRemovals(permutation[j],c.times[permutation[j]])<<endl;
						}
					}
					triedRemovals(permutation[j],c.times[permutation[j]])++;
					
					/////update tabu
					int a=tabu_activities[crt_tabu_index];
					int t=tabu_times[crt_tabu_index];
					if(a>=0 && t>=0){
						assert(triedRemovals(a,t)>0);
						triedRemovals(a,t)--;
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
			}
			
			assert(confl.count()==conflActivitiesTimeSlot.count());
			
			int j=0;
			int tmp=permutation[added_act];
			for(int k : qAsConst(ok)){
				permutation[j]=k;
				invPermutation[k]=j;
				j++;
			}
			int q=j;
			permutation[j]=tmp;
			invPermutation[tmp]=j;
			j++;
			if(VERBOSE){
				cout<<"id of permutation[j=="<<j-1<<"]=="<<gt.rules.internalActivitiesList[permutation[j-1]].id<<endl;
				cout<<"conflicting:"<<endl;
			}
			for(int k : qAsConst(confl)){
				permutation[j]=k;
				invPermutation[k]=j;
				j++;
				if(VERBOSE){
					cout<<"id of permutation[j=="<<j-1<<"]=="<<gt.rules.internalActivitiesList[permutation[j-1]].id<<endl;
				}
			}
			assert(j==added_act+1);
			
			if(VERBOSE){
				cout<<"tmp represents activity with id=="<<gt.rules.internalActivitiesList[tmp].id;
				cout<<" initial time: "<<c.times[tmp];
				cout<<" final time: "<<timeSlot<<endl;
			}
			c.times[tmp]=timeSlot;
			c.rooms[tmp]=roomSlot;
			c.realRoomsList[tmp]=realRoomsSlot;
			
			for(int i=q+1; i<=added_act; i++){
				if(!fixedTimeActivity[permutation[i]])
					c.times[permutation[i]]=UNALLOCATED_TIME;
				c.rooms[permutation[i]]=UNALLOCATED_SPACE;
			}
			c._fitness=-1;
			c.changedForMatrixCalculation=true;
			
			added_act=q+1;
			/*if(threaded){
				myMutex.unlock();
			}*/
	
			//if(semaphorePlacedActivity){
			emit(activityPlaced(nThread, q+1));
			if(threaded){
				//std::mutex mtx;
				//std::unique_lock<std::mutex> lck(mtx);
				//semaphorePlacedActivity.acquire();
				//cvForPlacedActivity.wait(lck);
				myMutex.unlock();
				
				semaphorePlacedActivity.acquire();
			}
			//}

			goto prevvalue;
//////////////////////
		}
		else{ //if foundGoodSwap==true
			nPlacedActivities=added_act+1;
			
			if(maxActivitiesPlaced<added_act+1){
				time_t tmp;
				time(&tmp);
				timeToHighestStage=int(difftime(tmp, starting_time));
				
				highestStageSolution.copy(gt.rules, c);

				maxActivitiesPlaced=added_act+1;
				
				if(maxPlacedActivityStream!=nullptr){
					int sec=timeToHighestStage;
					int hh=sec/3600;
					sec%=3600;
					int mm=sec/60;
					sec%=60;
					QString s=tr("At time %1 h %2 m %3 s, FET reached %4 activities placed", "h=hours, m=minutes, s=seconds. Please leave spaces between 'time', %1, h, %2, m, %3, s, so they are visible")
						.arg(hh).arg(mm).arg(sec).arg(maxActivitiesPlaced);
					
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
					(*maxPlacedActivityStream)<<s<<Qt::endl;
#else
					(*maxPlacedActivityStream)<<s<<endl;
#endif
				}
			}
			
			/*if(threaded){
				myMutex.unlock();
			}*/
			emit(activityPlaced(nThread, added_act+1));
			if(threaded){
				//std::mutex mtx;
				//std::unique_lock<std::mutex> lck(mtx);
				//semaphorePlacedActivity.acquire();
				//cvForPlacedActivity.wait(lck);
				myMutex.unlock();

				semaphorePlacedActivity.acquire();

				//semaphorePlacedActivity.acquire();
				myMutex.lock();
			}
			/*if(added_act==gt.rules.nInternalActivities && foundGoodSwap){ //Should be added_act+1==...
				//isRunning=false;

				if(threaded){
					myMutex.unlock();
				}
				break;
			}*/
			
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

		if(threaded){
			myMutex.unlock();
		}
	}

	time_t end_time;
	time(&end_time);
	searchTime=int(difftime(end_time, starting_time));
#ifdef FET_COMMAND_LINE
	cout<<"Total searching time (seconds): "<<int(difftime(end_time, starting_time))<<endl;
#else
	if(VERBOSE){
		cout<<"Total searching time (seconds): "<<int(difftime(end_time, starting_time))<<endl;
	}
#endif

	emit(simulationFinished());
	
	//if(threaded)
	//	finishedSemaphore.release();
}

void Generate::moveActivity(int ai, int fromslot, int toslot, int fromroom, int toroom, const QList<int>& fromRealRoomsList, const QList<int>& toRealRoomsList)
{
	Activity* act=&gt.rules.internalActivitiesList[ai];

	assert(fromslot==c.times[ai]);
	assert(fromroom==c.rooms[ai]);
	assert(fromRealRoomsList==c.realRoomsList[ai]);
	
	if(!fixedTimeActivity[ai] && (fromslot==UNALLOCATED_TIME || fromroom==UNALLOCATED_SPACE))
		assert(fromslot==UNALLOCATED_TIME && fromroom==UNALLOCATED_SPACE);
	if(!fixedTimeActivity[ai] && (toslot==UNALLOCATED_TIME || toroom==UNALLOCATED_SPACE))
		assert(toslot==UNALLOCATED_TIME && toroom==UNALLOCATED_SPACE);
	
	if(fromslot!=UNALLOCATED_TIME){
		int d=fromslot%gt.rules.nDaysPerWeek;
		int h=fromslot/gt.rules.nDaysPerWeek;
		
		////////////////rooms
		int rm=fromroom;
		if(rm!=UNSPECIFIED_ROOM && rm!=UNALLOCATED_SPACE){
			for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
				assert(dd+h<gt.rules.nHoursPerDay);
				if(gt.rules.internalRoomsList[rm]->isVirtual==false){
					if(roomsTimetable(rm,d,h+dd)==ai)
						roomsTimetable(rm,d,h+dd)=-1;
					else
						assert(0);
				}
				else{
					assert(roomsTimetable(rm,d,h+dd)==-1);
				}
			}

			if(gt.rules.internalRoomsList[rm]->isVirtual){
				assert(rm==fromroom); //this test could also be done for real rooms, above?
				assert(fromRealRoomsList.count()==gt.rules.internalRoomsList[rm]->rrsl.count());
				int i=0;
				for(int rr : qAsConst(fromRealRoomsList)){
					assert(gt.rules.internalRoomsList[rm]->rrsl.at(i).contains(rr));
				
					for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
						assert(dd+h<gt.rules.nHoursPerDay);
						if(roomsTimetable(rr,d,h+dd)==ai)
							roomsTimetable(rr,d,h+dd)=-1;
						else
							assert(0);
					}
					
					i++;
				}
			}
		}
		/////////////////////
		
		if(fromslot!=toslot){
			//timetable of students
			for(int q=0; q<act->iSubgroupsList.count(); q++){
				int sb=act->iSubgroupsList.at(q);
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
				
					assert(subgroupsTimetable(sb,d,h+dd)==ai);
					subgroupsTimetable(sb,d,h+dd)=-1;
				}
			}

			for(int sb : qAsConst(mustComputeTimetableSubgroups[ai])){
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
				
					assert(newSubgroupsTimetable(sb,d,h+dd)==ai);
					newSubgroupsTimetable(sb,d,h+dd)=-1;
				}
			}

			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				updateSubgroupsNHoursGaps(ai, d);

				//update students' list of activities for each day
				/////////////////
				for(int st : qAsConst(subgroupsWithMaxDaysPerWeekForActivities[ai])){
					int tt=subgroupActivitiesOfTheDay(st,d).removeAll(ai);
					assert(tt==1);
				}
				/////////////////
			}
			else{
				updateSubgroupsNHoursGaps(ai, d);
				if(haveStudentsMaxGapsPerRealDay)
					updateSubgroupsNHoursGapsRealDay(ai, d/2);

				//update students' list of activities for each day
				/////////////////

				/*
				Consider also N1N2N3, as for teachers
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
				QSet<int> st_smhd=QSet<int>(subgroupsWithMaxDaysPerWeekForActivities[ai].constBegin(), subgroupsWithMaxDaysPerWeekForActivities[ai].constEnd());
				QSet<int> st_smtd=QSet<int>(subgroupsWithMaxThreeConsecutiveDaysForActivities[ai].constBegin(), subgroupsWithMaxThreeConsecutiveDaysForActivities[ai].constEnd());
				QSet<int> st_smd=QSet<int>(subgroupsWithMaxRealDaysPerWeekForActivities[ai].constBegin(), subgroupsWithMaxRealDaysPerWeekForActivities[ai].constEnd());
				QSet<int> st_sma=QSet<int>(subgroupsWithMaxAfternoonsPerWeekForActivities[ai].constBegin(), subgroupsWithMaxAfternoonsPerWeekForActivities[ai].constEnd());
				QSet<int> st_smm=QSet<int>(subgroupsWithMaxMorningsPerWeekForActivities[ai].constBegin(), subgroupsWithMaxMorningsPerWeekForActivities[ai].constEnd());
#else
				QSet<int> st_smhd=subgroupsWithMaxDaysPerWeekForActivities[ai].toSet();
				QSet<int> st_smtd=subgroupsWithMaxThreeConsecutiveDaysForActivities[ai].toSet();
				QSet<int> st_smd=subgroupsWithMaxRealDaysPerWeekForActivities[ai].toSet();
				QSet<int> st_sma=subgroupsWithMaxAfternoonsPerWeekForActivities[ai].toSet();
				QSet<int> st_smm=subgroupsWithMaxMorningsPerWeekForActivities[ai].toSet();
#endif
				QSet<int> st_smda=st_smhd+st_smtd+st_smd+st_sma;
				QSet<int> st_smdm=st_smhd+st_smtd+st_smd+st_smm;
				*/

				//speed improvement on 2022-02-16
				if(d%2==1){
					for(int sbg : qAsConst(/*st_smda*/ subgroupsForActivitiesOfTheDayAfternoons[ai])){
						int tt=subgroupActivitiesOfTheDay(sbg,d).removeAll(ai);
						assert(tt==1);
					}
				}
				else{
					for(int sbg : qAsConst(/*st_smdm*/ subgroupsForActivitiesOfTheDayMornings[ai])){
						int tt=subgroupActivitiesOfTheDay(sbg,d).removeAll(ai);
						assert(tt==1);
					}
				}

				/*for(int st : qAsConst(subgroupsWithMaxRealDaysPerWeekForActivities[ai])){
					int tt=subgroupActivitiesOfTheDay(st,d).removeAll(ai);
					assert(tt==1);
				}*/
				/////////////////
			}

			//teachers' timetable
			for(int q=0; q<act->iTeachersList.count(); q++){
				int tch=act->iTeachersList.at(q);
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
					
					assert(teachersTimetable(tch,d,h+dd)==ai);
					teachersTimetable(tch,d,h+dd)=-1;
				}
			}
	
			for(int tch : qAsConst(mustComputeTimetableTeachers[ai])){
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
				
					assert(newTeachersTimetable(tch,d,h+dd)==ai);
					newTeachersTimetable(tch,d,h+dd)=-1;
				}
			}

			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				updateTeachersNHoursGaps(ai, d);

				//update teachers' list of activities for each day
				/////////////////
				for(int tch : qAsConst(teachersWithMaxDaysPerWeekForActivities[ai])){
					int tt=teacherActivitiesOfTheDay(tch,d).removeAll(ai);
					assert(tt==1);
				}
				/////////////////
			}
			else{
				updateTeachersNHoursGaps(ai, d);
				if(haveTeachersMaxGapsPerRealDay)
					updateTeachersNHoursGapsRealDay(ai, d/2);

				//update teachers' list of activities for each day
				/////////////////
				/*
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
				QSet<int> smhd=QSet<int>(teachersWithMaxDaysPerWeekForActivities[ai].constBegin(), teachersWithMaxDaysPerWeekForActivities[ai].constEnd());
				QSet<int> smtd=QSet<int>(teachersWithMaxThreeConsecutiveDaysForActivities[ai].constBegin(), teachersWithMaxThreeConsecutiveDaysForActivities[ai].constEnd());
				QSet<int> smd=QSet<int>(teachersWithMaxRealDaysPerWeekForActivities[ai].constBegin(), teachersWithMaxRealDaysPerWeekForActivities[ai].constEnd());
				QSet<int> smn1n2n3=QSet<int>(teachersWithN1N2N3ForActivities[ai].constBegin(), teachersWithN1N2N3ForActivities[ai].constEnd());
				QSet<int> sma=QSet<int>(teachersWithMaxAfternoonsPerWeekForActivities[ai].constBegin(), teachersWithMaxAfternoonsPerWeekForActivities[ai].constEnd());
				QSet<int> smm=QSet<int>(teachersWithMaxMorningsPerWeekForActivities[ai].constBegin(), teachersWithMaxMorningsPerWeekForActivities[ai].constEnd());
#else
				QSet<int> smhd=teachersWithMaxDaysPerWeekForActivities[ai].toSet();
				QSet<int> smtd=teachersWithMaxThreeConsecutiveDaysForActivities[ai].toSet();
				QSet<int> smd=teachersWithMaxRealDaysPerWeekForActivities[ai].toSet();
				QSet<int> smn1n2n3=teachersWithN1N2N3ForActivities[ai].toSet();
				QSet<int> sma=teachersWithMaxAfternoonsPerWeekForActivities[ai].toSet();
				QSet<int> smm=teachersWithMaxMorningsPerWeekForActivities[ai].toSet();
#endif
				QSet<int> smda=smhd+smtd+smd+sma+smn1n2n3;
				QSet<int> smdm=smhd+smtd+smd+smm+smn1n2n3;
				*/

				//speed improvement on 2022-02-16
				if(d%2==1){
					for(int tch : qAsConst(/*smda*/ teachersForActivitiesOfTheDayAfternoons[ai])){
						int tt=teacherActivitiesOfTheDay(tch,d).removeAll(ai);
						assert(tt==1);
					}
				}
				else{
					for(int tch : qAsConst(/*smdm*/ teachersForActivitiesOfTheDayMornings[ai])){
						int tt=teacherActivitiesOfTheDay(tch,d).removeAll(ai);
						assert(tt==1);
					}
				}

				/*for(int tch : qAsConst(teachersWithMaxRealDaysPerWeekForActivities[ai])){
					int tt=teacherActivitiesOfTheDay(tch,d).removeAll(ai);
					assert(tt==1);
				}*/
				/////////////////
			}
			
			//2011-09-30
			if(activityHasOccupyMaxConstraints[ai] || activityHasMaxSimultaneousConstraints[ai]){
				for(int t=fromslot; t<fromslot+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
					assert(activitiesAtTime[t].contains(ai));
					activitiesAtTime[t].remove(ai);
				}
			}
		}
	}
	
	if(toroom>=0 && toroom<gt.rules.nInternalRooms)
		if(gt.rules.internalRoomsList[toroom]->isVirtual==true)
			assert(gt.rules.internalRoomsList[toroom]->rrsl.count()==toRealRoomsList.count());
	
	c.times[ai]=toslot;
	c.rooms[ai]=toroom;
	c.realRoomsList[ai]=toRealRoomsList;
	c._fitness=-1;
	c.changedForMatrixCalculation=true;
	
	if(toslot!=UNALLOCATED_TIME){
		int d=toslot%gt.rules.nDaysPerWeek;
		int h=toslot/gt.rules.nDaysPerWeek;
		
		////////////////rooms
		int rm=toroom;
		if(rm!=UNSPECIFIED_ROOM && rm!=UNALLOCATED_SPACE){
			for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
				assert(dd+h<gt.rules.nHoursPerDay);
				
				assert(rm!=UNALLOCATED_SPACE);

				if(gt.rules.internalRoomsList[rm]->isVirtual==false){
					assert(roomsTimetable(rm,d,h+dd)==-1);
					roomsTimetable(rm,d,h+dd)=ai;
				}
				else{
					assert(roomsTimetable(rm,d,h+dd)==-1);
				}
			}

			if(gt.rules.internalRoomsList[rm]->isVirtual){
				assert(rm==toroom); //this test could also be done for real rooms, above?
				assert(toRealRoomsList.count()==gt.rules.internalRoomsList[rm]->rrsl.count());
				int i=0;
				for(int rr : qAsConst(toRealRoomsList)){
					assert(gt.rules.internalRoomsList[rm]->rrsl.at(i).contains(rr));
				
					for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
						assert(dd+h<gt.rules.nHoursPerDay);

						assert(rr!=UNALLOCATED_SPACE);
						assert(roomsTimetable(rr,d,h+dd)==-1);
						roomsTimetable(rr,d,h+dd)=ai;
					}
					
					i++;
				}
			}
		}
		/////////////////////
		
		if(fromslot!=toslot){
			//compute timetable of subgroups
			for(int q=0; q<act->iSubgroupsList.count(); q++){
				int sb=act->iSubgroupsList.at(q);
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
				
					assert(subgroupsTimetable(sb,d,h+dd)==-1);
					subgroupsTimetable(sb,d,h+dd)=ai;
				}
			}
	
			for(int sb : qAsConst(mustComputeTimetableSubgroups[ai])){
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
				
					assert(newSubgroupsTimetable(sb,d,h+dd)==-1);
					newSubgroupsTimetable(sb,d,h+dd)=ai;
				}
			}

			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				updateSubgroupsNHoursGaps(ai, d);

				//update students' list of activities for each day
				/////////////////
				for(int st : qAsConst(subgroupsWithMaxDaysPerWeekForActivities[ai])){
					assert(subgroupActivitiesOfTheDay(st,d).indexOf(ai)==-1);
					subgroupActivitiesOfTheDay(st,d).append(ai);
				}
				/////////////////
			}
			else{
				updateSubgroupsNHoursGaps(ai, d);
				if(haveStudentsMaxGapsPerRealDay)
					updateSubgroupsNHoursGapsRealDay(ai, d/2);

				//update students' list of activities for each day
				/////////////////

				/*
				Consider also N1N2N3, as for teachers
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
				QSet<int> st_smhd=QSet<int>(subgroupsWithMaxDaysPerWeekForActivities[ai].constBegin(), subgroupsWithMaxDaysPerWeekForActivities[ai].constEnd());
				QSet<int> st_smtd=QSet<int>(subgroupsWithMaxThreeConsecutiveDaysForActivities[ai].constBegin(), subgroupsWithMaxThreeConsecutiveDaysForActivities[ai].constEnd());
				QSet<int> st_smd=QSet<int>(subgroupsWithMaxRealDaysPerWeekForActivities[ai].constBegin(), subgroupsWithMaxRealDaysPerWeekForActivities[ai].constEnd());
				QSet<int> st_sma=QSet<int>(subgroupsWithMaxAfternoonsPerWeekForActivities[ai].constBegin(), subgroupsWithMaxAfternoonsPerWeekForActivities[ai].constEnd());
				QSet<int> st_smm=QSet<int>(subgroupsWithMaxMorningsPerWeekForActivities[ai].constBegin(), subgroupsWithMaxMorningsPerWeekForActivities[ai].constEnd());
#else
				QSet<int> st_smhd=subgroupsWithMaxDaysPerWeekForActivities[ai].toSet();
				QSet<int> st_smtd=subgroupsWithMaxThreeConsecutiveDaysForActivities[ai].toSet();
				QSet<int> st_smd=subgroupsWithMaxRealDaysPerWeekForActivities[ai].toSet();
				QSet<int> st_sma=subgroupsWithMaxAfternoonsPerWeekForActivities[ai].toSet();
				QSet<int> st_smm=subgroupsWithMaxMorningsPerWeekForActivities[ai].toSet();
#endif
				QSet<int> st_smda=st_smhd+st_smtd+st_smd+st_sma;
				QSet<int> st_smdm=st_smhd+st_smtd+st_smd+st_smm;
				*/

				//speed improvement on 2022-02-16
				if(d%2==1){
					for(int sbg : qAsConst(/*st_smda*/ subgroupsForActivitiesOfTheDayAfternoons[ai])){
						assert(subgroupActivitiesOfTheDay(sbg,d).indexOf(ai)==-1);
						subgroupActivitiesOfTheDay(sbg,d).append(ai);
					}
				}
				else{
					for(int sbg : qAsConst(/*st_smdm*/ subgroupsForActivitiesOfTheDayMornings[ai])){
						assert(subgroupActivitiesOfTheDay(sbg,d).indexOf(ai)==-1);
						subgroupActivitiesOfTheDay(sbg,d).append(ai);
					}
				}

				/*for(int st : qAsConst(subgroupsWithMaxRealDaysPerWeekForActivities[ai])){
					assert(subgroupActivitiesOfTheDay(st,d).indexOf(ai)==-1);
					subgroupActivitiesOfTheDay(st,d).append(ai);
				}*/
				/////////////////
			}

			//teachers' timetable
			for(int q=0; q<act->iTeachersList.count(); q++){
				int tch=act->iTeachersList.at(q);
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
				
					assert(teachersTimetable(tch,d,h+dd)==-1);
					teachersTimetable(tch,d,h+dd)=ai;
				}
			}
	
			for(int tch : qAsConst(mustComputeTimetableTeachers[ai])){
				for(int dd=0; dd<gt.rules.internalActivitiesList[ai].duration; dd++){
					assert(dd+h<gt.rules.nHoursPerDay);
				
					assert(newTeachersTimetable(tch,d,h+dd)==-1);
					newTeachersTimetable(tch,d,h+dd)=ai;
				}
			}

			if(gt.rules.mode!=MORNINGS_AFTERNOONS){
				//////////
				updateTeachersNHoursGaps(ai, d);

				//update teachers' list of activities for each day
				/////////////////
				for(int tch : qAsConst(teachersWithMaxDaysPerWeekForActivities[ai])){
					assert(teacherActivitiesOfTheDay(tch,d).indexOf(ai)==-1);
					teacherActivitiesOfTheDay(tch,d).append(ai);
				}
				/////////////////
			}
			else{
				//////////
				updateTeachersNHoursGaps(ai, d);
				if(haveTeachersMaxGapsPerRealDay)
					updateTeachersNHoursGapsRealDay(ai, d/2);

				//update teachers' list of activities for each day
				/////////////////
				/*
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
				QSet<int> smhd=QSet<int>(teachersWithMaxDaysPerWeekForActivities[ai].constBegin(), teachersWithMaxDaysPerWeekForActivities[ai].constEnd());
				QSet<int> smtd=QSet<int>(teachersWithMaxThreeConsecutiveDaysForActivities[ai].constBegin(), teachersWithMaxThreeConsecutiveDaysForActivities[ai].constEnd());
				QSet<int> smd=QSet<int>(teachersWithMaxRealDaysPerWeekForActivities[ai].constBegin(), teachersWithMaxRealDaysPerWeekForActivities[ai].constEnd());
				QSet<int> smn1n2n3=QSet<int>(teachersWithN1N2N3ForActivities[ai].constBegin(), teachersWithN1N2N3ForActivities[ai].constEnd());
				QSet<int> sma=QSet<int>(teachersWithMaxAfternoonsPerWeekForActivities[ai].constBegin(), teachersWithMaxAfternoonsPerWeekForActivities[ai].constEnd());
				QSet<int> smm=QSet<int>(teachersWithMaxMorningsPerWeekForActivities[ai].constBegin(), teachersWithMaxMorningsPerWeekForActivities[ai].constEnd());
#else
				QSet<int> smhd=teachersWithMaxDaysPerWeekForActivities[ai].toSet();
				QSet<int> smtd=teachersWithMaxThreeConsecutiveDaysForActivities[ai].toSet();
				QSet<int> smd=teachersWithMaxRealDaysPerWeekForActivities[ai].toSet();
				QSet<int> smn1n2n3=teachersWithN1N2N3ForActivities[ai].toSet();
				QSet<int> sma=teachersWithMaxAfternoonsPerWeekForActivities[ai].toSet();
				QSet<int> smm=teachersWithMaxMorningsPerWeekForActivities[ai].toSet();
#endif
				QSet<int> smda=smhd+smtd+smd+sma+smn1n2n3;
				QSet<int> smdm=smhd+smtd+smd+smm+smn1n2n3;
				*/

				//speed improvement on 2022-02-16
				if(d%2==1){
					for(int tch : qAsConst(/*smda*/ teachersForActivitiesOfTheDayAfternoons[ai])){
						assert(teacherActivitiesOfTheDay(tch,d).indexOf(ai)==-1);
						teacherActivitiesOfTheDay(tch,d).append(ai);
					}
				}
				else{
					for(int tch : qAsConst(/*smdm*/ teachersForActivitiesOfTheDayMornings[ai])){
						assert(teacherActivitiesOfTheDay(tch,d).indexOf(ai)==-1);
						teacherActivitiesOfTheDay(tch,d).append(ai);
					}
				}

				/*for(int tch : qAsConst(teachersWithMaxRealDaysPerWeekForActivities[ai])){
					assert(teacherActivitiesOfTheDay(tch,d).indexOf(ai)==-1);
					teacherActivitiesOfTheDay(tch,d).append(ai);
				}*/
				/////////////////
			}

			//2011-09-30
			if(activityHasOccupyMaxConstraints[ai] || activityHasMaxSimultaneousConstraints[ai]){
				for(int t=toslot; t<toslot+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
					assert(!activitiesAtTime[t].contains(ai));
					activitiesAtTime[t].insert(ai);
				}
			}
		}
	}
}

//faster: (to avoid allocating memory at each call)
/*static double nMinDaysBrokenL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
static int selectedRoomL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
static int permL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
static QList<int> conflActivitiesL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
static int nConflActivitiesL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
static int roomSlotsL[MAX_LEVEL][MAX_HOURS_PER_WEEK];
static QList<int> realRoomsListL[MAX_LEVEL][MAX_HOURS_PER_WEEK]; //the chosen real rooms, in the order of the sets, one for each set, if the room is virtual

static int currentLevel;

static QSet<int> conflActivitiesSet;*/

inline bool Generate::compareFunctionGenerate(int i, int j)
{
	if(nConflActivitiesL[currentLevel][i] < nConflActivitiesL[currentLevel][j] ||
	 (nConflActivitiesL[currentLevel][i] == nConflActivitiesL[currentLevel][j] &&
	 nMinDaysBrokenL[currentLevel][i] < nMinDaysBrokenL[currentLevel][j]))
		return true;
	
	return false;
}


#define nMinDaysBroken			(nMinDaysBrokenL[level])
#define selectedRoom			(selectedRoomL[level])
#define perm					(permL[level])
#define conflActivities			(conflActivitiesL[level])
#define nConflActivities		(nConflActivitiesL[level])
#define roomSlots				(roomSlotsL[level])

#define realRoomsListLevel		(realRoomsListL[level])

void Generate::randomSwap(int ai, int level){
	if(level==0){
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
	
	bool updateSubgroups=(mustComputeTimetableSubgroups[ai].count()>0);
	bool updateTeachers=(mustComputeTimetableTeachers[ai].count()>0);
	
	int activity_count_impossible_tries=1;

again_if_impossible_activity:

	//generate a random permutation in linear time like in CLR (Cormen, Leiserson, and Rivest - Introduction to algorithms).
	//this is used to scan the times in a random order
	for(int i=0; i<gt.rules.nHoursPerWeek; i++)
		perm[i]=i;
	for(int i=0; i<gt.rules.nHoursPerWeek; i++){
		int t=perm[i];
		int r=rng.intMRG32k3a(gt.rules.nHoursPerWeek-i);
		perm[i]=perm[i+r];
		perm[i+r]=t;
	}
	
	for(int n=0; n<gt.rules.nHoursPerWeek; n++){
		int newtime=perm[n];
		
		 //bug corrected on 2021-04-13, this instruction must be executed before the if-s/continue below, because otherwise nMinDaysBroken[newtime] might remain uninitialized
		 //and give a crash lower in the code. For instance, for a Hungary/Bethlen locked timetable, it crashes after the std::stable_sort near the end of generate.cpp
		 //( std::stable_sort(perm+0, perm+gt.rules.nHoursPerWeek, [this](int i, int j){return compareFunctionGenerate(i, j);}); )
		 //when asserting that if nConflActivities[perm[i-1]] == nConflActivities[perm[i]] then it must be nMinDaysBroken[perm[i-1]] <= nMinDaysBroken[perm[i]],
		 //which sometimes seems to result in comparing -nan <= -nan, which seems not to be true. This bug appeared only
		 //with Qt 6.0.3 (not with Qt 5) and only in the fet-cl (command-line) version.
		nMinDaysBroken[newtime]=0.0;

		if(c.times[ai]!=UNALLOCATED_TIME){
			if(c.times[ai]!=newtime){
				nConflActivities[newtime]=MAX_ACTIVITIES;
				continue;
			}
		}

		nConflActivities[newtime]=0;
		conflActivities[newtime].clear();

		int d=newtime%gt.rules.nDaysPerWeek;
		int h=newtime/gt.rules.nDaysPerWeek;
		
		//For mornings-afternoons
		int d_first, d_last;
		int d_pre, d_after;

		bool okteachersmorningsafternoonsbehavior;
		bool ok_max_two_consecutive_mornings_afternoons;

		bool okbasictime;
		bool okmindays;
		bool okminhalfdays;
		bool okmaxdays;
		bool okmaxhalfdays;
		//For terms - max terms between activities
		bool okmaxterms;
		bool oksamestartingtime;
		bool oksamestartinghour;
		bool oksamestartingday;
		bool oknotoverlapping;
		bool oktwoactivitiesconsecutive;
		bool oktwoactivitiesgrouped;
		bool okthreeactivitiesgrouped;
		bool oktwoactivitiesordered;
		bool oktwoactivitiesorderedifsameday;
		bool okactivityendsstudentsday;
		bool okactivityendsteachersday;

		bool okactivitybeginsstudentsday;
		bool okactivitybeginsteachersday;

		bool okstudentsmaxdaysperweek;
		bool okstudentsmaxthreeconsecutivedays;
		bool okstudentsmaxrealdaysperweek;
		bool okstudentsintervalmaxdaysperweek;

		bool okstudentsmaxspanperday;
		bool okstudentsminrestinghours;

		bool okstudentsminrestinghoursbetweenmorningandafternoon;
		bool okstudentsmaxspanperrealday;

		bool okstudentsmaxgapsperweekforrealdays;
		bool okstudentsmaxgapsperrealday;

		bool okstudentsearlymaxbeginningsatsecondhour;
		bool okstudentsmaxgapsperweek;
		bool okstudentsmaxgapsperday;
		bool okstudentsmaxhoursdaily;
		bool okstudentsmaxhourscontinuously;
		bool okstudentsminhoursdaily;

		bool okstudentsmaxhoursdailyrealdays;

		bool okstudentsactivitytagmaxhoursdailyrealdays;
		bool okstudentsmorningintervalmaxdaysperweek;
		bool okstudentsafternoonintervalmaxdaysperweek;

		bool okstudentsafternoonsearlymaxbeginningsatsecondhour;
		bool okstudentsmorningsearlymaxbeginningsatsecondhour;
		bool okstudentsmaxafternoonsperweek;
		bool okstudentsmaxmorningsperweek;

		bool okstudentsminmorningsafternoonsperweek;
		bool okstudentsmaxhoursperallafternoons;

		bool okteachersmaxdaysperweek;
		bool okteachersmaxthreeconsecutivedays;
		bool okteachersmaxrealdaysperweek;
		bool okteachersintervalmaxdaysperweek;

		bool okteachersafternoonsearlymaxbeginningsatsecondhour;
		bool okteachersmorningsearlymaxbeginningsatsecondhour;
		bool okteachersmorningsafternoonsearlymaxbeginningsatsecondhour;
		bool okteachermaxafternoonsperweek;
		bool okteachermaxmorningsperweek;
		bool okteachersmorningintervalmaxdaysperweek;
		bool okteachersafternoonintervalmaxdaysperweek;

		bool okteachersmaxspanperday;
		bool okteachersminrestinghours;

		bool okteachersmaxspanperrealday;
		bool okteachersminrestinghoursbetweenmorningandafternoon;

		bool okteachersmaxgapsperweek;
		bool okteachersmaxgapsperday;
		bool okteachersmaxgapspermorningandafternoon;
		bool okteachersmaxhoursdaily;
		bool okteachersmaxhourscontinuously;
		bool okteachersminhoursdaily;
		bool okteachersmindaysperweek;

		bool okteachersminrealdaysperweek;

		bool okteachersmaxgapsperweekforrealdays;
		bool okteachersmaxgapsperrealday;
		bool okteachersmax0gapsperafternoon;
		bool okteachersmaxhoursdailyrealdays;
		bool okteachersminhoursdailyrealdays;
		bool okteachersminmorningsafternoonsperweek;

		bool okteachersactivitytagmaxhoursdailyrealdays;

		bool okmingapsbetweenactivities;
		bool okmaxgapsbetweenactivities;

		bool okteachersactivitytagmaxhourscontinuously;
		bool okstudentsactivitytagmaxhourscontinuously;

		bool okteachersactivitytagmaxhoursdaily;
		bool okstudentsactivitytagmaxhoursdaily;

		bool okteachersactivitytagminhoursdaily;
		bool okstudentsactivitytagminhoursdaily;

		bool okstudentsmingapsbetweenorderedpairofactivitytags;
		bool okteachersmingapsbetweenorderedpairofactivitytags;
		bool okstudentsmingapsbetweenactivitytag;
		bool okteachersmingapsbetweenactivitytag;

		bool okteachersmaxtwoactivitytagsperdayfromn1n2n3;
		bool okstudentsmaxtwoactivitytagsperdayfromn1n2n3;
		bool okteachersmaxtwoactivitytagsperrealdayfromn1n2n3;
		bool okstudentsmaxtwoactivitytagsperrealdayfromn1n2n3;
		bool okteachersmaxhoursperallafternoons;

		//2011-09-25
		bool okactivitiesoccupymaxtimeslotsfromselection;
		
		//2020-04-30
		bool okmaxtotalactivitiesfromsetinselectedtimeslots;

		//2019-11-16
		bool okactivitiesoccupymintimeslotsfromselection;
		
		//2011-09-30
		bool okactivitiesmaxsimultaneousinselectedtimeslots;
		
		//2019-11-16
		bool okactivitiesminsimultaneousinselectedtimeslots;
		
		//for terms
		//2020-01-14
		bool okactivitiesmaxinaterm;
		bool okactivitiesoccupymaxterms;
		//2022-05-19
		bool okactivitiesmininaterm;

		if(c.times[ai]!=UNALLOCATED_TIME)
			goto skip_here_if_already_allocated_in_time;

/////////////////////////////////////////////////////////////////////////////////////////////

		//Old comment below
		//not too late
		//unneeded code, because notAllowedTimesPercentages(ai,newtime)==100 now
		//you can comment this code, but you cannot put an assert failed, because the test is done in the next section (see 13 lines below).
		/*if(h+act->duration>gt.rules.nHoursPerDay){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}*/
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed (tch&st not available, break, act(s) preferred time(s))
		if(!skipRandom(notAllowedTimesPercentages(ai,newtime))){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//care about basic time constraints
		okbasictime=true;

		///////////////////////////////////
		//added in 5.0.0-preview30
		//same teacher?
		for(int dur=0; dur<act->duration; dur++){
			assert(h+dur<gt.rules.nHoursPerDay);
			for(int tch : qAsConst(act->iTeachersList)){
				if(teachersTimetable(tch,d,h+dur)>=0){
					int ai2=teachersTimetable(tch,d,h+dur);
					assert(ai2!=ai);
				
					//assert(activitiesConflictingPercentage(ai,ai2)==100);
					assert(activitiesConflictingPercentage[ai].value(ai2, -1)==100);

					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						okbasictime=false;
						goto impossiblebasictime;
					}

					if(!conflActivities[newtime].contains(ai2)){
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(nConflActivities[newtime]==conflActivities[newtime].count());
					}
				}
			}
		}
		//same subgroup?
		if(gt.rules.mode!=BLOCK_PLANNING){
			for(int dur=0; dur<act->duration; dur++){
				assert(h+dur<gt.rules.nHoursPerDay);
				for(int sbg : qAsConst(act->iSubgroupsList)){
					if(subgroupsTimetable(sbg,d,h+dur)>=0){
						int ai2=subgroupsTimetable(sbg,d,h+dur);
						assert(ai2!=ai);

						//assert(activitiesConflictingPercentage(ai,ai2)==100);
						assert(activitiesConflictingPercentage[ai].value(ai2, -1)==100);

						if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
							okbasictime=false;
							goto impossiblebasictime;
						}

						if(!conflActivities[newtime].contains(ai2)){
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(nConflActivities[newtime]==conflActivities[newtime].count());
						}
					}
				}
			}
		}
		else{
			//same subgroup in real life time slots?
			for(int dur=0; dur<act->duration; dur++){
				assert(h+dur<gt.rules.nHoursPerDay);
				for(int sbg : qAsConst(act->iSubgroupsList)){
					for(int day_tch=0; day_tch<gt.rules.nDaysPerWeek; day_tch++){
						if(subgroupsTimetable(sbg,day_tch,h+dur)>=0){
							int ai2=subgroupsTimetable(sbg,day_tch,h+dur);
							assert(ai2!=ai);

							//assert(activitiesConflictingPercentage(ai,ai2)==100);
							assert(activitiesConflictingPercentage[ai].value(ai2, -1)==100);
							if(true){
								if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
									okbasictime=false;
									goto impossiblebasictime;
								}

								if(!conflActivities[newtime].contains(ai2)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(nConflActivities[newtime]==conflActivities[newtime].count());
								}
							}
						}
					}
				}
			}
		}
		///////////////////////////////////
		
impossiblebasictime:
		if(!okbasictime){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//care about min days between activities
		okmindays=true;
		
		for(int i=0; i<minDaysListOfActivities[ai].count(); i++){
			int ai2=minDaysListOfActivities[ai].at(i);
			int md=minDaysListOfMinDays[ai].at(i);
			int ai2time=c.times[ai2];
			if(ai2time!=UNALLOCATED_TIME){
				int d2=ai2time%gt.rules.nDaysPerWeek;
				int h2=ai2time/gt.rules.nDaysPerWeek;
				if((gt.rules.mode!=MORNINGS_AFTERNOONS && md>abs(d-d2)) || (gt.rules.mode==MORNINGS_AFTERNOONS && md>abs(d/2-d2/2))){
					bool okrand=skipRandom(minDaysListOfWeightPercentages[ai].at(i));
					//if(fixedTimeActivity[ai] && minDaysListOfWeightPercentages[ai].at(i)<100.0)
					//	okrand=true;
				
					if(minDaysListOfConsecutiveIfSameDay[ai].at(i)==true){ //must place them consecutive (in any order) if on the same day
						if(okrand &&
						 ((gt.rules.mode!=MORNINGS_AFTERNOONS && ( (d==d2 && (h+act->duration==h2 || h2+gt.rules.internalActivitiesList[ai2].duration==h)) || d!=d2 ))
						 ||
						 (gt.rules.mode==MORNINGS_AFTERNOONS && ( (d==d2 && (h+act->duration==h2 || h2+gt.rules.internalActivitiesList[ai2].duration==h)) || d/2!=d2/2 ))
						 )){
							//nMinDaysBroken[newtime]++;
							nMinDaysBroken[newtime]+=minDaysListOfWeightPercentages[ai].at(i)/100.0;
						}
						else{
							if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
								okmindays=false;
								goto impossiblemindays;
							}
							
							if(!conflActivities[newtime].contains(ai2)){
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(nConflActivities[newtime]==conflActivities[newtime].count());
							}
						}
					}
					else{ //can place them anywhere
						if(okrand){
							//From version 6.4.0: no more than 2 subactivities on the same day (real day in case of MORNINGS_AFTERNOONS mode)
							if((gt.rules.mode!=MORNINGS_AFTERNOONS && d==d2) || (gt.rules.mode==MORNINGS_AFTERNOONS && d/2==d2/2)){
								for(int ai3 : qAsConst(*minDaysListOfActivitiesFromTheSameConstraint[ai].at(i))){
									if(ai3!=ai && ai3!=ai2){
										int ai3time=c.times[ai3];
										if(ai3time!=UNALLOCATED_TIME){
											int d3=ai3time%gt.rules.nDaysPerWeek;
											if((gt.rules.mode!=MORNINGS_AFTERNOONS && d3==d) || (gt.rules.mode==MORNINGS_AFTERNOONS && d3/2==d/2)){
												assert((gt.rules.mode!=MORNINGS_AFTERNOONS && d3==d2) || (gt.rules.mode==MORNINGS_AFTERNOONS && d3/2==d2/2));
												if(fixedTimeActivity[ai3] || swappedActivities[ai3]){
													okmindays=false;
													goto impossiblemindays;
												}
												
												if(!conflActivities[newtime].contains(ai3)){
													conflActivities[newtime].append(ai3);
													nConflActivities[newtime]++;
													assert(nConflActivities[newtime]==conflActivities[newtime].count());
												}
											}
										}
									}
								}
							}
						
						 	//nMinDaysBroken[newtime]++;
						 	nMinDaysBroken[newtime]+=minDaysListOfWeightPercentages[ai].at(i)/100.0;
						}
						else{
							if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
								okmindays=false;
								goto impossiblemindays;
							}
							
							if(!conflActivities[newtime].contains(ai2)){
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
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//care about min half days between activities
		okminhalfdays=true;
		
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int i=0; i<minHalfDaysListOfActivities[ai].count(); i++){
				int ai2=minHalfDaysListOfActivities[ai].at(i);
				int md=minHalfDaysListOfMinDays[ai].at(i);
				int ai2time=c.times[ai2];
				if(ai2time!=UNALLOCATED_TIME){
					int d2=ai2time%gt.rules.nDaysPerWeek;
					int h2=ai2time/gt.rules.nDaysPerWeek;
					if(md>abs(d-d2)){
						bool okrand=skipRandom(minHalfDaysListOfWeightPercentages[ai].at(i));
						//if(fixedTimeActivity[ai] && minHalfDaysListOfWeightPercentages[ai].at(i)<100.0)
						//	okrand=true;

						if(minHalfDaysListOfConsecutiveIfSameDay[ai].at(i)==true){ //must place them consecutive (in any order) if on the same day
							if(okrand &&
							((d==d2 && (h+act->duration==h2 || h2+gt.rules.internalActivitiesList[ai2].duration==h)) || d!=d2 )){
								//nMinDaysBroken[newtime]++;
								nMinDaysBroken[newtime]+=minHalfDaysListOfWeightPercentages[ai].at(i)/100.0;
							}
							else{
								if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
									okminhalfdays=false;
									goto impossibleminhalfdays;
								}

								if(!conflActivities[newtime].contains(ai2)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(nConflActivities[newtime]==conflActivities[newtime].count());
								}
							}
						}
						else{ //can place them anywhere
							if(okrand){
								//From version 6.4.0: no more than 2 subactivities on the same half day
								if(d==d2){
									for(int ai3 : qAsConst(*minHalfDaysListOfActivitiesFromTheSameConstraint[ai].at(i))){
										if(ai3!=ai && ai3!=ai2){
											int ai3time=c.times[ai3];
											if(ai3time!=UNALLOCATED_TIME){
												int d3=ai3time%gt.rules.nDaysPerWeek;
												if(d3==d){
													assert(d3==d2);
													if(fixedTimeActivity[ai3] || swappedActivities[ai3]){
														okminhalfdays=false;
														goto impossibleminhalfdays;
													}
													
													if(!conflActivities[newtime].contains(ai3)){
														conflActivities[newtime].append(ai3);
														nConflActivities[newtime]++;
														assert(nConflActivities[newtime]==conflActivities[newtime].count());
													}
												}
											}
										}
									}
								}
						
								//nMinDaysBroken[newtime]++;
								nMinDaysBroken[newtime]+=minHalfDaysListOfWeightPercentages[ai].at(i)/100.0;
							}
							else{
								if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
									okminhalfdays=false;
									goto impossibleminhalfdays;
								}

								if(!conflActivities[newtime].contains(ai2)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(nConflActivities[newtime]==conflActivities[newtime].count());
								}
							}
						}
					}
				}
			}
		}
impossibleminhalfdays:
		if(!okminhalfdays){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//care about max days between activities
		okmaxdays=true;
		
		for(int i=0; i<maxDaysListOfActivities[ai].count(); i++){
			int ai2=maxDaysListOfActivities[ai].at(i);
			int md=maxDaysListOfMaxDays[ai].at(i);
			int ai2time=c.times[ai2];
			if(ai2time!=UNALLOCATED_TIME){
				int d2=ai2time%gt.rules.nDaysPerWeek;
				//int h2=ai2time/gt.rules.nDaysPerWeek;
				if((gt.rules.mode!=MORNINGS_AFTERNOONS && md<abs(d-d2)) || (gt.rules.mode==MORNINGS_AFTERNOONS && md<abs(d/2-d2/2))){
					bool okrand=skipRandom(maxDaysListOfWeightPercentages[ai].at(i));
					if(!okrand){
						if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
							okmaxdays=false;
							goto impossiblemaxdays;
						}
						
						if(!conflActivities[newtime].contains(ai2)){
							conflActivities[newtime].append(ai2);

							nConflActivities[newtime]++;
							assert(nConflActivities[newtime]==conflActivities[newtime].count());
						}
					}
				}
			}
		}
impossiblemaxdays:
		if(!okmaxdays){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//care about max half days between activities
		okmaxhalfdays=true;
		
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int i=0; i<maxHalfDaysListOfActivities[ai].count(); i++){
				int ai2=maxHalfDaysListOfActivities[ai].at(i);
				int md=maxHalfDaysListOfMaxDays[ai].at(i);
				int ai2time=c.times[ai2];
				if(ai2time!=UNALLOCATED_TIME){
					int d2=ai2time%gt.rules.nDaysPerWeek;
					//int h2=ai2time/gt.rules.nDaysPerWeek;
					if(md<abs(d-d2)){
						bool okrand=skipRandom(maxHalfDaysListOfWeightPercentages[ai].at(i));
						if(!okrand){
							if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
								okmaxhalfdays=false;
								goto impossiblemaxhalfdays;
							}
							
							if(!conflActivities[newtime].contains(ai2)){
								conflActivities[newtime].append(ai2);

								nConflActivities[newtime]++;
								assert(nConflActivities[newtime]==conflActivities[newtime].count());
							}
						}
					}
				}
			}
		}
impossiblemaxhalfdays:
		if(!okmaxhalfdays){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//for the Terms mode - care about max terms between activities
		okmaxterms=true;

		if(gt.rules.mode==TERMS){
			int termai = (newtime%gt.rules.nDaysPerWeek)/gt.rules.nDaysPerTerm;
			for(int i=0; i<maxTermsListOfActivities[ai].count(); i++){
				int ai2=maxTermsListOfActivities[ai].at(i);
				int mt=maxTermsListOfMaxTerms[ai].at(i);
				int ai2time=c.times[ai2];
				if(ai2time!=UNALLOCATED_TIME){
					int termai2 = (ai2time%gt.rules.nDaysPerWeek)/gt.rules.nDaysPerTerm;
					if(mt<abs(termai-termai2)){
						bool okrand=skipRandom(maxTermsListOfWeightPercentages[ai].at(i));
						if(!okrand){
							if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
								okmaxterms=false;
								goto impossiblemaxterms;
							}
							
							if(!conflActivities[newtime].contains(ai2)){
								conflActivities[newtime].append(ai2);

								nConflActivities[newtime]++;
								assert(nConflActivities[newtime]==conflActivities[newtime].count());
							}
						}
					}
				}
			}
		}
impossiblemaxterms:
		if(!okmaxterms){
			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////
		//care about min gaps between activities
		okmingapsbetweenactivities=true;
		
		for(int i=0; i<minGapsBetweenActivitiesListOfActivities[ai].count(); i++){
			int ai2=minGapsBetweenActivitiesListOfActivities[ai].at(i);
			int mg=minGapsBetweenActivitiesListOfMinGaps[ai].at(i);
			int ai2time=c.times[ai2];
			if(ai2time!=UNALLOCATED_TIME){
				int d2=ai2time%gt.rules.nDaysPerWeek;
				int h2=ai2time/gt.rules.nDaysPerWeek;
				int duration2=gt.rules.internalActivitiesList[ai2].duration;
				bool oktmp=true;
				if(d==d2){
					if(h2>=h){
						if(h+act->duration+mg > h2){
							oktmp=false;
						}
					}
					else{
						if(h2+duration2+mg > h){
							oktmp=false;
						}
					}
				}
				
				if(!oktmp){
					bool okrand=skipRandom(minGapsBetweenActivitiesListOfWeightPercentages[ai].at(i));
					
					//if(fixedTimeActivity[ai] && minGapsBetweenActivitiesListOfWeightPercentages[ai].at(i)<100.0)
					//	okrand=true;
					
					if(!okrand){
						if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
							okmingapsbetweenactivities=false;
							goto impossiblemingapsbetweenactivities;
						}
						
						if(!conflActivities[newtime].contains(ai2)){
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(nConflActivities[newtime]==conflActivities[newtime].count());
						}
					}
				}
			}
		}
impossiblemingapsbetweenactivities:
		if(!okmingapsbetweenactivities){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//care about max gaps between activities
		okmaxgapsbetweenactivities=true;

		//used in block-planning mode.
		if(gt.rules.mode==BLOCK_PLANNING){
			for(int i=0; i<maxGapsBetweenActivitiesListOfActivities[ai].count(); i++){
				int ai2=maxGapsBetweenActivitiesListOfActivities[ai].at(i);
				int mg=maxGapsBetweenActivitiesListOfMaxGaps[ai].at(i);
				int ai2time=c.times[ai2];
				if(ai2time!=UNALLOCATED_TIME){
					int d2=ai2time%gt.rules.nDaysPerWeek;
					int h2=ai2time/gt.rules.nDaysPerWeek;
					int duration2=gt.rules.internalActivitiesList[ai2].duration;
					bool oktmp=true;
					if(d==d2){
						if(h2>=h){
							if(h+act->duration+mg < h2){
								oktmp=false;
							}
						}
						else{
							if(h2+duration2+mg < h){
								oktmp=false;
							}
						}
					}

					if(!oktmp){
						bool okrand=skipRandom(maxGapsBetweenActivitiesListOfWeightPercentages[ai].at(i));

						//if(fixedTimeActivity[ai] && maxGapsBetweenActivitiesListOfWeightPercentages[ai].at(i)<100.0)
						//	okrand=true;

						if(!okrand){
							if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
								okmaxgapsbetweenactivities=false;
								goto impossiblemaxgapsbetweenactivities;
							}

							if(!conflActivities[newtime].contains(ai2)){
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(nConflActivities[newtime]==conflActivities[newtime].count());
							}
						}
					}
				}
			}
		}
impossiblemaxgapsbetweenactivities:
		if(!okmaxgapsbetweenactivities){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from same starting time
		oksamestartingtime=true;
		
		for(int i=0; i<activitiesSameStartingTimeActivities[ai].count(); i++){
			int ai2=activitiesSameStartingTimeActivities[ai].at(i);
			double perc=activitiesSameStartingTimePercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				bool sR=skipRandom(perc);
				
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;
			
				if(newtime!=c.times[ai2] && !sR){
					assert(ai2!=ai);
					
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oksamestartingtime=false;
						goto impossiblesamestartingtime;
					}
					
					if(!conflActivities[newtime].contains(ai2)){
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
					}
				}
			}
		}
impossiblesamestartingtime:
		if(!oksamestartingtime){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from same starting hour
		oksamestartinghour=true;
		
		for(int i=0; i<activitiesSameStartingHourActivities[ai].count(); i++){
			int ai2=activitiesSameStartingHourActivities[ai].at(i);
			double perc=activitiesSameStartingHourPercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				bool sR=skipRandom(perc);
				
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;
			
				if((newtime/gt.rules.nDaysPerWeek)!=(c.times[ai2]/gt.rules.nDaysPerWeek) && !sR){
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oksamestartinghour=false;
						goto impossiblesamestartinghour;
					}
				
					if(!conflActivities[newtime].contains(ai2)){
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
					}
				}
			}
		}
impossiblesamestartinghour:
		if(!oksamestartinghour){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from same starting day
		oksamestartingday=true;
		
		for(int i=0; i<activitiesSameStartingDayActivities[ai].count(); i++){
			int ai2=activitiesSameStartingDayActivities[ai].at(i);
			double perc=activitiesSameStartingDayPercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				bool sR=skipRandom(perc);
				
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;
			
				if((newtime%gt.rules.nDaysPerWeek)!=(c.times[ai2]%gt.rules.nDaysPerWeek) && !sR){
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oksamestartingday=false;
						goto impossiblesamestartingday;
					}
				
					if(!conflActivities[newtime].contains(ai2)){
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
					}
				}
			}
		}
impossiblesamestartingday:
		if(!oksamestartingday){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from not overlapping
		oknotoverlapping=true;
		
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
					
					bool sR=skipRandom(perc);
					//if(fixedTimeActivity[ai] && perc<100.0)
					//	sR=true;
					
					if(!(en<=st2 || en2<=st) && !sR){
						assert(ai2!=ai);
						
						if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
							oknotoverlapping=false;
							goto impossiblenotoverlapping;
						}
						
						if(!conflActivities[newtime].contains(ai2)){
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
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from two activities consecutive
		oktwoactivitiesconsecutive=true;
		
		for(int i=0; i<constrTwoActivitiesConsecutiveActivities[ai].count(); i++){
			//direct
			int ai2=constrTwoActivitiesConsecutiveActivities[ai].at(i);
			double perc=constrTwoActivitiesConsecutivePercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
				int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
				bool ok=true;
				
				if(d2!=d)
					ok=false;
				else if(h+act->duration > h2)
					ok=false;
				else{
					assert(d2==d);
					int kk;
					for(kk=h+act->duration; kk<gt.rules.nHoursPerDay; kk++)
						if(!breakDayHour(d,kk))
							break;
					assert(kk<=h2);
					if(kk!=h2)
						ok=false;
				}
				
				bool sR=skipRandom(perc);
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;
				
				if(!ok && !sR){
					assert(ai2!=ai);
					
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oktwoactivitiesconsecutive=false;
						goto impossibletwoactivitiesconsecutive;
					}
					
					if(!conflActivities[newtime].contains(ai2)){
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
					}
				}
			}
		}

		for(int i=0; i<inverseConstrTwoActivitiesConsecutiveActivities[ai].count(); i++){
			//inverse
			int ai2=inverseConstrTwoActivitiesConsecutiveActivities[ai].at(i);
			double perc=inverseConstrTwoActivitiesConsecutivePercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
				int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
				bool ok=true;
				
				if(d2!=d)
					ok=false;
				else if(h2+gt.rules.internalActivitiesList[ai2].duration > h)
					ok=false;
				else{
					assert(d2==d);
					int kk;
					for(kk=h2+gt.rules.internalActivitiesList[ai2].duration; kk<gt.rules.nHoursPerDay; kk++)
						if(!breakDayHour(d,kk))
							break;
					assert(kk<=h);
					if(kk!=h)
						ok=false;
				}
				
				bool sR=skipRandom(perc);
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;
			
				if(!ok && !sR){
					assert(ai2!=ai);
					
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oktwoactivitiesconsecutive=false;
						goto impossibletwoactivitiesconsecutive;
					}
					
					if(!conflActivities[newtime].contains(ai2)){
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
					}
				}
			}
		}
		
impossibletwoactivitiesconsecutive:
		if(!oktwoactivitiesconsecutive){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from two activities grouped
		oktwoactivitiesgrouped=true;
		
		for(int i=0; i<constrTwoActivitiesGroupedActivities[ai].count(); i++){
			//direct
			int ai2=constrTwoActivitiesGroupedActivities[ai].at(i);
			double perc=constrTwoActivitiesGroupedPercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
				int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
				bool ok=true;
				
				if(d2!=d){
					ok=false;
				}
				else if(d==d2 && h2+gt.rules.internalActivitiesList[ai2].duration <= h){
					int kk;
					for(kk=h2+gt.rules.internalActivitiesList[ai2].duration; kk<gt.rules.nHoursPerDay; kk++)
						if(!breakDayHour(d2,kk))
							break;
					assert(kk<=h);
					if(kk!=h)
						ok=false;
				}
				else if(d==d2 && h+act->duration <= h2){
					int kk;
					for(kk=h+act->duration; kk<gt.rules.nHoursPerDay; kk++)
						if(!breakDayHour(d,kk))
							break;
					assert(kk<=h2);
					if(kk!=h2)
						ok=false;
				}
				else
					ok=false;
					
				bool sR=skipRandom(perc);
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;
				
				if(!ok && !sR){
					assert(ai2!=ai);
					
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oktwoactivitiesgrouped=false;
						goto impossibletwoactivitiesgrouped;
					}
					
					if(!conflActivities[newtime].contains(ai2)){
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
					}
				}
			}
		}

impossibletwoactivitiesgrouped:
		if(!oktwoactivitiesgrouped){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from three activities grouped
		okthreeactivitiesgrouped=true;
		
		for(int i=0; i<constrThreeActivitiesGroupedActivities[ai].count(); i++){
			int ai2=constrThreeActivitiesGroupedActivities[ai].at(i).first;
			int ai3=constrThreeActivitiesGroupedActivities[ai].at(i).second;
			double perc=constrThreeActivitiesGroupedPercentages[ai].at(i);

			bool sR=skipRandom(perc);
			//if(fixedTimeActivity[ai] && perc<100.0)
			//	sR=true;
			
			int aip1=-1, aip2=-1; //ai placed
			int ainp1=-1, ainp2=-1; //ai not placed
			if(c.times[ai2]==UNALLOCATED_TIME || conflActivities[newtime].contains(ai2))
				ainp1=ai2;
			else
				aip1=ai2;
			if(c.times[ai3]==UNALLOCATED_TIME || conflActivities[newtime].contains(ai3)){
				if(ainp1==-1)
					ainp1=ai3;
				else
					ainp2=ai3;
			}
			else{
				if(aip1==-1)
					aip1=ai3;
				else
					aip2=ai3;
			}
			
			int cnt=0;
			if(ainp1>=0)
				cnt++;
			if(ainp2>=0)
				cnt++;
			if(aip1>=0)
				cnt++;
			if(aip2>=0)
				cnt++;
			assert(cnt==2);
			
			bool ok;
			
			if(aip1==-1){
				//ok - both not placed
				ok=true;
			}
			else if(aip2==-1){
				//only one placed, one not placed
				int dp1=c.times[aip1]%gt.rules.nDaysPerWeek;
				int hp1=c.times[aip1]/gt.rules.nDaysPerWeek;
				int durp1=gt.rules.internalActivitiesList[aip1].duration;
				
				int hoursBetweenThem=-1;
				
				if(dp1!=d)
					hoursBetweenThem=-1;
				else if(dp1==d && h >= hp1+durp1){
					hoursBetweenThem=0;
					for(int kk=hp1+durp1; kk<h; kk++)
						if(!breakDayHour(d,kk)){
							//check that the working hours are not separated by a break
							//assertion that durp1>0, so the kk-1 >= 0
							if(breakDayHour(d,kk-1) && hoursBetweenThem>0){
								hoursBetweenThem=-1;
								break;
							}

							hoursBetweenThem++;
						}
				}
				else if(dp1==d && hp1 >= h+act->duration){
					hoursBetweenThem=0;
					for(int kk=h+act->duration; kk<hp1; kk++)
						if(!breakDayHour(d,kk)){
							//check that the working hours are not separated by a break
							//assertion that act->duration>0, so the kk-1 >= 0
							if(breakDayHour(d,kk-1) && hoursBetweenThem>0){
								hoursBetweenThem=-1;
								break;
							}

							hoursBetweenThem++;
						}
				}
				else
					hoursBetweenThem=-1;
					
				assert(ainp1>=0);
				if(hoursBetweenThem==0 || hoursBetweenThem==gt.rules.internalActivitiesList[ainp1].duration)
					//OK
					ok=true;
				else
					//not OK
					ok=false;
			}
			else{
				assert(aip1>=0 && aip2>=0);
				//both placed
				int dp1=c.times[aip1]%gt.rules.nDaysPerWeek;
				int hp1=c.times[aip1]/gt.rules.nDaysPerWeek;
				//int durp1=gt.rules.internalActivitiesList[aip1].duration;
				
				int dp2=c.times[aip2]%gt.rules.nDaysPerWeek;
				int hp2=c.times[aip2]/gt.rules.nDaysPerWeek;
				//int durp2=gt.rules.internalActivitiesList[aip2].duration;
				
				if(dp1==dp2 && dp1==d){
					int ao1=-1, ao2=-1, ao3=-1; //order them, 1 then 2 then 3
					if(h>=hp1 && h>=hp2 && hp2>=hp1){
						ao1=aip1;
						ao2=aip2;
						ao3=ai;
					}
					else if(h>=hp1 && h>=hp2 && hp1>=hp2){
						ao1=aip2;
						ao2=aip1;
						ao3=ai;
					}
					else if(hp1>=h && hp1>=hp2 && h>=hp2){
						ao1=aip2;
						ao2=ai;
						ao3=aip1;
					}
					else if(hp1>=h && hp1>=hp2 && hp2>=h){
						ao1=ai;
						ao2=aip2;
						ao3=aip1;
					}
					else if(hp2>=h && hp2>=hp1 && h>=hp1){
						ao1=aip1;
						ao2=ai;
						ao3=aip2;
					}
					else if(hp2>=h && hp2>=hp1 && hp1>=h){
						ao1=ai;
						ao2=aip1;
						ao3=aip2;
					}
					else
						assert(0);

					int do1;
					int ho1;
					int duro1;

					int do2;
					int ho2;
					int duro2;

					int do3;
					int ho3;
					//int duro3;
					
					if(ao1==ai){
						do1=d;
						ho1=h;
						duro1=act->duration;
					}
					else{
						do1=c.times[ao1]%gt.rules.nDaysPerWeek;
						ho1=c.times[ao1]/gt.rules.nDaysPerWeek;
						duro1=gt.rules.internalActivitiesList[ao1].duration;
					}

					if(ao2==ai){
						do2=d;
						ho2=h;
						duro2=act->duration;
					}
					else{
						do2=c.times[ao2]%gt.rules.nDaysPerWeek;
						ho2=c.times[ao2]/gt.rules.nDaysPerWeek;
						duro2=gt.rules.internalActivitiesList[ao2].duration;
					}

					if(ao3==ai){
						do3=d;
						ho3=h;
						//duro3=act->duration;
					}
					else{
						do3=c.times[ao3]%gt.rules.nDaysPerWeek;
						ho3=c.times[ao3]/gt.rules.nDaysPerWeek;
						//duro3=gt.rules.internalActivitiesList[ao3].duration;
					}
					
					assert(do1==do2 && do1==do3);
					if(ho1+duro1<=ho2 && ho2+duro2<=ho3){
						int hoursBetweenThem=0;
						
						for(int kk=ho1+duro1; kk<ho2; kk++)
							if(!breakDayHour(d,kk))
								hoursBetweenThem++;
						for(int kk=ho2+duro2; kk<ho3; kk++)
							if(!breakDayHour(d,kk))
								hoursBetweenThem++;
						
						if(hoursBetweenThem==0)
							ok=true;
						else
							ok=false;
					}
					else{
						//not OK
						ok=false;
					}
				}
				else{
					//not OK
					ok=false;
				}
			}
			
			bool again;//=false;
			
			if(!ok && !sR){
				int aidisplaced=-1;
			
				if(aip2>=0){ //two placed activities
					again=true;
				
					QList<int> acts;
					
					if(!fixedTimeActivity[aip1] && !swappedActivities[aip1])
						acts.append(aip1);
					if(!fixedTimeActivity[aip2] && !swappedActivities[aip2])
						acts.append(aip2);

					if(acts.count()==0)
						aidisplaced=-1;
					else if(acts.count()==1)
						aidisplaced=acts.at(0);
					else{
						int t;
						if(level==0){
							int optMinWrong=INF;
				
							QList<int> tl;
		
							for(int q=0; q<acts.count(); q++){
								int tta=acts.at(q);
								if(optMinWrong>triedRemovals(tta,c.times[tta])){
								 	optMinWrong=triedRemovals(tta,c.times[tta]);
								}
							}
					
							for(int q=0; q<acts.count(); q++){
								int tta=acts.at(q);
								if(optMinWrong==triedRemovals(tta,c.times[tta]))
									tl.append(q);
							}
				
							assert(tl.count()>=1);
							int mpos=tl.at(rng.intMRG32k3a(tl.count()));
					
							assert(mpos>=0 && mpos<acts.count());
							t=mpos;
						}
						else{
							t=rng.intMRG32k3a(acts.count());
						}
						
						aidisplaced=acts.at(t);
					}
				}
				else{
					again=false;
					assert(aip1>=0);
					if(!fixedTimeActivity[aip1] && !swappedActivities[aip1])
						aidisplaced=aip1;
					else
						aidisplaced=-1;
				}
			
				assert(aidisplaced!=ai);
				
				if(aidisplaced==-1){
					okthreeactivitiesgrouped=false;
					goto impossiblethreeactivitiesgrouped;
				}
				if(fixedTimeActivity[aidisplaced] || swappedActivities[aidisplaced]){
					okthreeactivitiesgrouped=false;
					goto impossiblethreeactivitiesgrouped;
				}
				
				assert(!conflActivities[newtime].contains(aidisplaced));
				conflActivities[newtime].append(aidisplaced);
				nConflActivities[newtime]++;
				assert(conflActivities[newtime].count()==nConflActivities[newtime]);
				
				//if !again, everything is OK, because there was one placed activity and it was eliminated
				
				if(again){
					aip1=-1, aip2=-1;
					ainp1=-1, ainp2=-1;
					if(c.times[ai2]==UNALLOCATED_TIME || conflActivities[newtime].contains(ai2))
						ainp1=ai2;
					else
						aip1=ai2;
					if(c.times[ai3]==UNALLOCATED_TIME || conflActivities[newtime].contains(ai3)){
						if(ainp1==-1)
							ainp1=ai3;
						else
							ainp2=ai3;
					}
					else{
						if(aip1==-1)
							aip1=ai3;
						else
							aip2=ai3;
					}

					assert(aip1>=0 && ainp1>=0 && aip2==-1 && ainp2==-1); //only one placed
					
					//again the procedure from above, with 1 placed
					int dp1=c.times[aip1]%gt.rules.nDaysPerWeek;
					int hp1=c.times[aip1]/gt.rules.nDaysPerWeek;
					int durp1=gt.rules.internalActivitiesList[aip1].duration;
					
					int hoursBetweenThem=-1;
				
					if(dp1!=d)
						hoursBetweenThem=-1;
					else if(dp1==d && h >= hp1+durp1){
						hoursBetweenThem=0;
						for(int kk=hp1+durp1; kk<h; kk++)
							if(!breakDayHour(d,kk)){
								//check that the working hours are not separated by a break
								//assertion that durp1>0, so the kk-1 >= 0
								if(breakDayHour(d,kk-1) && hoursBetweenThem>0){
									hoursBetweenThem=-1;
									break;
								}

								hoursBetweenThem++;
							}
					}
					else if(dp1==d && hp1 >= h+act->duration){
						hoursBetweenThem=0;
						for(int kk=h+act->duration; kk<hp1; kk++)
							if(!breakDayHour(d,kk)){
								//check that the working hours are not separated by a break
								//assertion that act->duration>0, so the kk-1 >= 0
								if(breakDayHour(d,kk-1) && hoursBetweenThem>0){
									hoursBetweenThem=-1;
									break;
								}

								hoursBetweenThem++;
							}
					}
					else
						hoursBetweenThem=-1;
					
					assert(ainp1>=0);
					if(hoursBetweenThem==0 || hoursBetweenThem==gt.rules.internalActivitiesList[ainp1].duration)
						//OK
						ok=true;
					else
						//not OK
						ok=false;
						
					assert(!sR);
					if(!ok){
						aidisplaced=aip1;
						if(fixedTimeActivity[aidisplaced] || swappedActivities[aidisplaced]){
							okthreeactivitiesgrouped=false;
							goto impossiblethreeactivitiesgrouped;
						}
						
						assert(!conflActivities[newtime].contains(aidisplaced));
						conflActivities[newtime].append(aidisplaced);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						
						//now it is OK, because there were two activities placed and both were eliminated
					}
				} //end if(again)
			}
		}

impossiblethreeactivitiesgrouped:
		if(!okthreeactivitiesgrouped){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from two activities ordered
		oktwoactivitiesordered=true;
		
		for(int i=0; i<constrTwoActivitiesOrderedActivities[ai].count(); i++){
			//direct
			int ai2=constrTwoActivitiesOrderedActivities[ai].at(i);
			double perc=constrTwoActivitiesOrderedPercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
				int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
				bool ok=true;
				
				if(!(d<d2 || (d==d2 && h+act->duration-1<h2)))
					ok=false;
					
				bool sR=skipRandom(perc);
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;

				if(!ok && !sR){
					assert(ai2!=ai);
					
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oktwoactivitiesordered=false;
						goto impossibletwoactivitiesordered;
					}
					
					if(!conflActivities[newtime].contains(ai2)){
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
					}
				}
			}
		}

		for(int i=0; i<inverseConstrTwoActivitiesOrderedActivities[ai].count(); i++){
			//inverse
			int ai2=inverseConstrTwoActivitiesOrderedActivities[ai].at(i);
			double perc=inverseConstrTwoActivitiesOrderedPercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
				int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
				int dur2=gt.rules.internalActivitiesList[ai2].duration;
				bool ok=true;
				
				if(!(d2<d || (d2==d && h2+dur2-1<h)))
					ok=false;
					
				bool sR=skipRandom(perc);
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;
				
				if(!ok && !sR){
					assert(ai2!=ai);
					
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oktwoactivitiesordered=false;
						goto impossibletwoactivitiesordered;
					}
					
					if(!conflActivities[newtime].contains(ai2)){
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
					}
				}
			}
		}
		
impossibletwoactivitiesordered:
		if(!oktwoactivitiesordered){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from two activities ordered if same day
		oktwoactivitiesorderedifsameday=true;
		
		for(int i=0; i<constrTwoActivitiesOrderedIfSameDayActivities[ai].count(); i++){
			//direct
			int ai2=constrTwoActivitiesOrderedIfSameDayActivities[ai].at(i);
			double perc=constrTwoActivitiesOrderedIfSameDayPercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
				int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
				bool ok=true;
				
				if(!(d!=d2 || (d==d2 && h+act->duration-1<h2)))
					ok=false;
					
				bool sR=skipRandom(perc);
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;

				if(!ok && !sR){
					assert(ai2!=ai);
					
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oktwoactivitiesorderedifsameday=false;
						goto impossibletwoactivitiesorderedifsameday;
					}
					
					if(!conflActivities[newtime].contains(ai2)){
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
					}
				}
			}
		}

		for(int i=0; i<inverseConstrTwoActivitiesOrderedIfSameDayActivities[ai].count(); i++){
			//inverse
			int ai2=inverseConstrTwoActivitiesOrderedIfSameDayActivities[ai].at(i);
			double perc=inverseConstrTwoActivitiesOrderedIfSameDayPercentages[ai].at(i);
			if(c.times[ai2]!=UNALLOCATED_TIME){
				int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
				int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
				int dur2=gt.rules.internalActivitiesList[ai2].duration;
				bool ok=true;
				
				if(!(d2!=d || (d2==d && h2+dur2-1<h)))
					ok=false;
					
				bool sR=skipRandom(perc);
				//if(fixedTimeActivity[ai] && perc<100.0)
				//	sR=true;
				
				if(!ok && !sR){
					assert(ai2!=ai);
					
					if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
						oktwoactivitiesorderedifsameday=false;
						goto impossibletwoactivitiesorderedifsameday;
					}
					
					if(!conflActivities[newtime].contains(ai2)){
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
					}
				}
			}
		}
		
impossibletwoactivitiesorderedifsameday:
		if(!oktwoactivitiesorderedifsameday){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from activity begins students day
		okactivitybeginsstudentsday=true;
		
		if(haveActivityBeginsStudentsDay){
			//1. If the current activity needs to be at the beginning
			if(activityBeginsStudentsDayPercentages[ai]>=0){
				bool skip=skipRandom(activityBeginsStudentsDayPercentages[ai]);
				if(!skip){
					for(int sb : qAsConst(act->iSubgroupsList)){
						if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sb]==0){
							int cnt;
							for(cnt=0; cnt<gt.rules.nHoursPerDay; cnt++)
								if(!breakDayHour(d,cnt) && !subgroupNotAvailableDayHour(sb,d,cnt))
									break;
							
							if(h>cnt){
								okactivitybeginsstudentsday=false;
								goto impossibleactivitybeginsstudentsday;
							}
						}
						else if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sb]>=1){
							int cnt;
							for(cnt=0; cnt<gt.rules.nHoursPerDay; cnt++)
								if(!breakDayHour(d,cnt) && !subgroupNotAvailableDayHour(sb,d,cnt))
									break;
							cnt++;
							for(; cnt<gt.rules.nHoursPerDay; cnt++)
								if(!breakDayHour(d,cnt) && !subgroupNotAvailableDayHour(sb,d,cnt))
									break;
							
							if(h>cnt){
								okactivitybeginsstudentsday=false;
								goto impossibleactivitybeginsstudentsday;
							}
						}
						for(int hh=h-1; hh>=0; hh--){
							int ai2=subgroupsTimetable(sb,d,hh);
							if(ai2>=0){
								if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
									okactivitybeginsstudentsday=false;
									goto impossibleactivitybeginsstudentsday;
								}
								
								if(!conflActivities[newtime].contains(ai2)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}
							}
						}
					}
				}
			}

			//2. Check the activities which have to be at the beginning, on the same day with the current activity
			for(int sb : qAsConst(act->iSubgroupsList)){
				for(int hh=h+act->duration; hh<gt.rules.nHoursPerDay; hh++){
					int ai2=subgroupsTimetable(sb,d,hh);
					if(ai2>=0)
						if(activityBeginsStudentsDayPercentages[ai2]>=0){
							bool skip=skipRandom(activityBeginsStudentsDayPercentages[ai2]);
							if(!skip){
								if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
									okactivitybeginsstudentsday=false;
									goto impossibleactivitybeginsstudentsday;
								}
								
								if(!conflActivities[newtime].contains(ai2)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}
							}
						}
				}
			}
		}

impossibleactivitybeginsstudentsday:
		if(!okactivitybeginsstudentsday){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from activity ends students day
		okactivityendsstudentsday=true;
		
		if(haveActivityEndsStudentsDay){
			//1. If the current activity needs to be at the end
			if(activityEndsStudentsDayPercentages[ai]>=0){
				bool skip=skipRandom(activityEndsStudentsDayPercentages[ai]);
				if(!skip){
					for(int sb : qAsConst(act->iSubgroupsList)){
						for(int hh=h+act->duration; hh<gt.rules.nHoursPerDay; hh++){
							int ai2=subgroupsTimetable(sb,d,hh);
							if(ai2>=0){
								if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
									okactivityendsstudentsday=false;
									goto impossibleactivityendsstudentsday;
								}
								
								if(!conflActivities[newtime].contains(ai2)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}
							}
						}
					}
				}
			}

			//2. Check the activities which have to be at the end, on the same day with the current activity
			for(int sb : qAsConst(act->iSubgroupsList)){
				for(int hh=h-1; hh>=0; hh--){
					int ai2=subgroupsTimetable(sb,d,hh);
					if(ai2>=0)
						if(activityEndsStudentsDayPercentages[ai2]>=0){
							bool skip=skipRandom(activityEndsStudentsDayPercentages[ai2]);
							if(!skip){
								if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
									okactivityendsstudentsday=false;
									goto impossibleactivityendsstudentsday;
								}
								
								if(!conflActivities[newtime].contains(ai2)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}
							}
						}
				}
			}
		}

impossibleactivityendsstudentsday:
		if(!okactivityendsstudentsday){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from activity begins teachers day
		okactivitybeginsteachersday=true;
		
		if(haveActivityBeginsTeachersDay){
			//1. If the current activity needs to be at the beginning
			if(activityBeginsTeachersDayPercentages[ai]>=0){
				bool skip=skipRandom(activityBeginsTeachersDayPercentages[ai]);
				if(!skip){
					for(int tch : qAsConst(act->iTeachersList)){
						for(int hh=h-1; hh>=0; hh--){
							int ai2=teachersTimetable(tch,d,hh);
							if(ai2>=0){
								if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
									okactivitybeginsteachersday=false;
									goto impossibleactivitybeginsteachersday;
								}
								
								if(!conflActivities[newtime].contains(ai2)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}
							}
						}
					}
				}
			}

			//2. Check the activities which have to be at the beginning, on the same day with the current activity
			for(int tch : qAsConst(act->iTeachersList)){
				for(int hh=h+act->duration; hh<gt.rules.nHoursPerDay; hh++){
					int ai2=teachersTimetable(tch,d,hh);
					if(ai2>=0)
						if(activityBeginsTeachersDayPercentages[ai2]>=0){
							bool skip=skipRandom(activityBeginsTeachersDayPercentages[ai2]);
							if(!skip){
								if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
									okactivitybeginsteachersday=false;
									goto impossibleactivitybeginsteachersday;
								}
								
								if(!conflActivities[newtime].contains(ai2)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}
							}
						}
				}
			}
		}

impossibleactivitybeginsteachersday:
		if(!okactivitybeginsteachersday){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from activity ends teachers day
		okactivityendsteachersday=true;
		
		if(haveActivityEndsTeachersDay){
			//1. If current activity needs to be at the end
			if(activityEndsTeachersDayPercentages[ai]>=0){
				bool skip=skipRandom(activityEndsTeachersDayPercentages[ai]);
				if(!skip){
					for(int tch : qAsConst(act->iTeachersList)){
						for(int hh=h+act->duration; hh<gt.rules.nHoursPerDay; hh++){
							int ai2=teachersTimetable(tch,d,hh);
							if(ai2>=0){
								if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
									okactivityendsteachersday=false;
									goto impossibleactivityendsteachersday;
								}
								
								if(!conflActivities[newtime].contains(ai2)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}
							}
						}
					}
				}
			}

			//2. Check the activities which have to be at the end, on the same day with the current activity
			for(int tch : qAsConst(act->iTeachersList)){
				for(int hh=h-1; hh>=0; hh--){
					int ai2=teachersTimetable(tch,d,hh);
					if(ai2>=0)
						if(activityEndsTeachersDayPercentages[ai2]>=0){
							bool skip=skipRandom(activityEndsTeachersDayPercentages[ai2]);
							if(!skip){
								if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
									okactivityendsteachersday=false;
									goto impossibleactivityendsteachersday;
								}
								
								if(!conflActivities[newtime].contains(ai2)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}
							}
						}
				}
			}
		}

impossibleactivityendsteachersday:
		if(!okactivityendsteachersday){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
		
		int dpair;
		
		dpair=-1;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(d%2==0)
				dpair=d+1;
			else
				dpair=d-1;
			assert(dpair>=0 && dpair<gt.rules.nDaysPerWeek);
		}

		///Teachers must not be in both days 1&2, both 3&4, and so on, but with the exceptions of special teachers
		
		okteachersmorningsafternoonsbehavior=true;
		
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int tc : qAsConst(act->iTeachersList)){
				int morningsAfternoonsBehavior=gt.rules.internalTeachersList[tc]->morningsAfternoonsBehavior;

				if(morningsAfternoonsBehavior==TEACHER_ONE_DAY_EXCEPTION)
				// if(gt.rules.internalExceptionTeachersSet.contains(tc))
				{
					QList<int> pairedCurrentDay;

					for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
						if(teachersTimetable(tc,dpair,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dpair,hh)))
							pairedCurrentDay.append(teachersTimetable(tc,dpair,hh));
					
					if(pairedCurrentDay.count()==0)
						continue;

					QList<int> morningOther;
					QList<int> afternoonOther;
					
					int dother=-1;
					for(int dd=0; dd<gt.rules.nDaysPerWeek/2; dd++){
						bool morning=false, afternoon=false;
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dd*2,hh)>=0)
								morning=true;
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dd*2+1,hh)>=0)
								afternoon=true;
						if(dd!=d/2 && morning && afternoon){
							assert(dd!=d/2);
							assert(dother==-1);
							dother=dd;
						}
					}
					if(dother>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother*2,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother*2,hh)))
								morningOther.append(teachersTimetable(tc,dother*2,hh));
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother*2+1,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother*2+1,hh)))
								afternoonOther.append(teachersTimetable(tc,dother*2+1,hh));
					}

					assert(pairedCurrentDay.count()>0);
					if(morningOther.count()>0 && afternoonOther.count()>0){
						//empty one of these three
						
						///////////////////////////////////
						//0 is paired current day, 1 is morning other, 2 is afternoon other
						
						bool occupiedDay[3];
						bool canEmptyDay[3];
					
						int _minWrong[3];
						int _nWrong[3];
						int _nConflActivities[3];
						int _minIndexAct[3];
					
						QList<int> _activitiesForDay[3];
						
						for(int d2=0; d2<3; d2++){
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;
							
							_minWrong[d2]=INF;
							_nWrong[d2]=0;
							_nConflActivities[d2]=0;
							_minIndexAct[d2]=gt.rules.nInternalActivities;
							_activitiesForDay[d2].clear();
							
							QList<int> tmp;
							if(d2==0)
								tmp=pairedCurrentDay;
							else if(d2==1)
								tmp=morningOther;
							else if(d2==2)
								tmp=afternoonOther;
							
							for(int ai2 : qAsConst(tmp)){
								assert(ai2>=0);
								if(ai2>=0){
									if(!conflActivities[newtime].contains(ai2)){
										occupiedDay[d2]=true;
										if(fixedTimeActivity[ai2] || swappedActivities[ai2])
											canEmptyDay[d2]=false;
										else if(!_activitiesForDay[d2].contains(ai2)){
											_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
											_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
											_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
											_nConflActivities[d2]++;
											_activitiesForDay[d2].append(ai2);
											assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
										}
									}
								}
							}
						
							assert(occupiedDay[d2]);
							if(!occupiedDay[d2])
								canEmptyDay[d2]=false;
						}
					
						//int nOc=0;
						bool canChooseDay=false;
					
						for(int j=0; j<3; j++)
							if(occupiedDay[j]){
								//nOc++;
								if(canEmptyDay[j]){
									canChooseDay=true;
								}
							}
						
						if(!canChooseDay){
							if(level==0){
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachersmorningsafternoonsbehavior=false;
							goto impossibleteachersmorningsafternoonsbehavior;
						}
						
						int d2=-1;
						
						if(level!=0){
							//choose random day from those with minimum number of conflicting activities
							QList<int> candidateDays;
							
							int m=gt.rules.nInternalActivities;
							
							for(int kk=0; kk<3; kk++)
								if(canEmptyDay[kk])
									if(m>_nConflActivities[kk])
										m=_nConflActivities[kk];
						
							candidateDays.clear();
							for(int kk=0; kk<3; kk++)
								if(canEmptyDay[kk] && m==_nConflActivities[kk])
									candidateDays.append(kk);
							
							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}
						else{ //level==0
							QList<int> candidateDays;
		
							int _mW=INF;
							int _nW=INF;
							int _mCA=gt.rules.nInternalActivities;
							int _mIA=gt.rules.nInternalActivities;

							for(int kk=0; kk<3; kk++)
								if(canEmptyDay[kk]){
									if(_mW>_minWrong[kk] ||
									(_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
									(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
									(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
										_mW=_minWrong[kk];
										_nW=_nWrong[kk];
										_mCA=_nConflActivities[kk];
										_mIA=_minIndexAct[kk];
									}
								}
								
							assert(_mW<INF);
							
							candidateDays.clear();
							for(int kk=0; kk<3; kk++)
								if(canEmptyDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);
									
							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}
						
						assert(d2>=0);
		
						assert(_activitiesForDay[d2].count()>0);
		
						for(int ai2 : qAsConst(_activitiesForDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						}
					}
				}
				else if(morningsAfternoonsBehavior==TEACHER_TWO_DAYS_EXCEPTION)
				//else if(gt.rules.internalExceptionTeachers2Set.contains(tc))
				{
					QList<int> pairedCurrentDay;

					for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
						if(teachersTimetable(tc,dpair,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dpair,hh)))
							pairedCurrentDay.append(teachersTimetable(tc,dpair,hh));
					
					if(pairedCurrentDay.count()==0)
						continue;

					QList<int> morningOther;
					QList<int> afternoonOther;
					QList<int> morningOther2;
					QList<int> afternoonOther2;
					
					int dother=-1, dother2=-1;
					for(int dd=0; dd<gt.rules.nDaysPerWeek/2; dd++){
						bool morning=false, afternoon=false;
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dd*2,hh)>=0)
								morning=true;
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dd*2+1,hh)>=0)
								afternoon=true;
						if(dd!=d/2 && morning && afternoon){
							assert(dd!=d/2);
							if(dother==-1){
								assert(dother==-1);
								dother=dd;
							}
							else{
								dother2=dd;
							}
						}
					}
					if(dother>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother*2,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother*2,hh)))
								morningOther.append(teachersTimetable(tc,dother*2,hh));
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother*2+1,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother*2+1,hh)))
								afternoonOther.append(teachersTimetable(tc,dother*2+1,hh));
					}
					if(dother2>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother2*2,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother2*2,hh)))
								morningOther2.append(teachersTimetable(tc,dother2*2,hh));
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother2*2+1,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother2*2+1,hh)))
								afternoonOther2.append(teachersTimetable(tc,dother2*2+1,hh));
					}
					
					assert(pairedCurrentDay.count()>0);
					if(morningOther.count()>0 && afternoonOther.count()>0 && morningOther2.count()>0 && afternoonOther2.count()>0){
						//empty one of these 5
						
						///////////////////////////////////
						//0 is paired current day, 1 is morning other, 2 is afternoon other
						//3 is morning other 2, 4 is afternoon other 2
						
						bool occupiedDay[3+2];
						bool canEmptyDay[3+2];
					
						int _minWrong[3+2];
						int _nWrong[3+2];
						int _nConflActivities[3+2];
						int _minIndexAct[3+2];
					
						QList<int> _activitiesForDay[3+2];
						
						for(int d2=0; d2<3+2; d2++){
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;
							
							_minWrong[d2]=INF;
							_nWrong[d2]=0;
							_nConflActivities[d2]=0;
							_minIndexAct[d2]=gt.rules.nInternalActivities;
							_activitiesForDay[d2].clear();
							
							QList<int> tmp;
							if(d2==0)
								tmp=pairedCurrentDay;
							else if(d2==1)
								tmp=morningOther;
							else if(d2==2)
								tmp=afternoonOther;
							else if(d2==3)
								tmp=morningOther2;
							else if(d2==4)
								tmp=afternoonOther2;
							
							for(int ai2 : qAsConst(tmp)){
								assert(ai2>=0);
								if(ai2>=0){
									if(!conflActivities[newtime].contains(ai2)){
										occupiedDay[d2]=true;
										if(fixedTimeActivity[ai2] || swappedActivities[ai2])
											canEmptyDay[d2]=false;
										else if(!_activitiesForDay[d2].contains(ai2)){
											_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
											_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
											_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
											_nConflActivities[d2]++;
											_activitiesForDay[d2].append(ai2);
											assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
										}
									}
								}
							}
						
							assert(occupiedDay[d2]);
							if(!occupiedDay[d2])
								canEmptyDay[d2]=false;
						}
					
						//int nOc=0;
						bool canChooseDay=false;
					
						for(int j=0; j<3+2; j++)
							if(occupiedDay[j]){
								//nOc++;
								if(canEmptyDay[j]){
									canChooseDay=true;
								}
							}
						
						if(!canChooseDay){
							if(level==0){
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachersmorningsafternoonsbehavior=false;
							goto impossibleteachersmorningsafternoonsbehavior;
						}
						
						int d2=-1;
						
						if(level!=0){
							//choose random day from those with minimum number of conflicting activities
							QList<int> candidateDays;
							
							int m=gt.rules.nInternalActivities;
							
							for(int kk=0; kk<3+2; kk++)
								if(canEmptyDay[kk])
									if(m>_nConflActivities[kk])
										m=_nConflActivities[kk];
						
							candidateDays.clear();
							for(int kk=0; kk<3+2; kk++)
								if(canEmptyDay[kk] && m==_nConflActivities[kk])
									candidateDays.append(kk);
									
							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}
						else{ //level==0
							QList<int> candidateDays;
		
							int _mW=INF;
							int _nW=INF;
							int _mCA=gt.rules.nInternalActivities;
							int _mIA=gt.rules.nInternalActivities;

							for(int kk=0; kk<3+2; kk++)
								if(canEmptyDay[kk]){
									if(_mW>_minWrong[kk] ||
									(_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
									(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
									(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
										_mW=_minWrong[kk];
										_nW=_nWrong[kk];
										_mCA=_nConflActivities[kk];
										_mIA=_minIndexAct[kk];
									}
								}
								
							assert(_mW<INF);
							
							candidateDays.clear();
							for(int kk=0; kk<3+2; kk++)
								if(canEmptyDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);
									
							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}
						
						assert(d2>=0);
		
						assert(_activitiesForDay[d2].count()>0);
		
						for(int ai2 : qAsConst(_activitiesForDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						}
					}
				}
				//2019-08-20 - for ZINEDDINE18 - three days exception
				else if(morningsAfternoonsBehavior==TEACHER_THREE_DAYS_EXCEPTION)
				//else if(gt.rules.internalExceptionTeachers3Set.contains(tc))
				{
					QList<int> pairedCurrentDay;

					for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
						if(teachersTimetable(tc,dpair,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dpair,hh)))
							pairedCurrentDay.append(teachersTimetable(tc,dpair,hh));
							
					if(pairedCurrentDay.count()==0)
						continue;

					QList<int> morningOther;
					QList<int> afternoonOther;
					QList<int> morningOther2;
					QList<int> afternoonOther2;
					QList<int> morningOther3;
					QList<int> afternoonOther3;
					
					int dother=-1, dother2=-1, dother3=-1;
					for(int dd=0; dd<gt.rules.nDaysPerWeek/2; dd++){
						bool morning=false, afternoon=false;
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dd*2,hh)>=0)
								morning=true;
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dd*2+1,hh)>=0)
								afternoon=true;
						if(dd!=d/2 && morning && afternoon){
							assert(dd!=d/2);
							if(dother==-1){
								assert(dother==-1);
								dother=dd;
							}
							else if(dother2==-1){
								assert(dother2==-1);
								dother2=dd;
							}
							else{
								dother3=dd;
							}
						}
					}
					if(dother>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother*2,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother*2,hh)))
								morningOther.append(teachersTimetable(tc,dother*2,hh));
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother*2+1,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother*2+1,hh)))
								afternoonOther.append(teachersTimetable(tc,dother*2+1,hh));
					}
					if(dother2>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother2*2,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother2*2,hh)))
								morningOther2.append(teachersTimetable(tc,dother2*2,hh));
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother2*2+1,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother2*2+1,hh)))
								afternoonOther2.append(teachersTimetable(tc,dother2*2+1,hh));
					}
					if(dother3>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother3*2,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother3*2,hh)))
								morningOther3.append(teachersTimetable(tc,dother3*2,hh));
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother3*2+1,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother3*2+1,hh)))
								afternoonOther3.append(teachersTimetable(tc,dother3*2+1,hh));
					}
					
					assert(pairedCurrentDay.count()>0);
					if(morningOther.count()>0 && afternoonOther.count()>0
					&& morningOther2.count()>0 && afternoonOther2.count()>0
					&& morningOther3.count()>0 && afternoonOther3.count()>0){
						//empty one of these 7
						
						///////////////////////////////////
						//0 is paired current day, 1 is morning other, 2 is afternoon other
						//3 is morning other 2, 4 is afternoon other 2
						//5 is morning other 3, 6 is afternoon other 3
						
						bool occupiedDay[3+2+2];
						bool canEmptyDay[3+2+2];
					
						int _minWrong[3+2+2];
						int _nWrong[3+2+2];
						int _nConflActivities[3+2+2];
						int _minIndexAct[3+2+2];
					
						QList<int> _activitiesForDay[3+2+2];
						
						for(int d2=0; d2<3+2+2; d2++){
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;
							
							_minWrong[d2]=INF;
							_nWrong[d2]=0;
							_nConflActivities[d2]=0;
							_minIndexAct[d2]=gt.rules.nInternalActivities;
							_activitiesForDay[d2].clear();
							
							QList<int> tmp;
							if(d2==0)
								tmp=pairedCurrentDay;
							else if(d2==1)
								tmp=morningOther;
							else if(d2==2)
								tmp=afternoonOther;
							else if(d2==3)
								tmp=morningOther2;
							else if(d2==4)
								tmp=afternoonOther2;
							else if(d2==5)
								tmp=morningOther3;
							else if(d2==6)
								tmp=afternoonOther3;
							
							for(int ai2 : qAsConst(tmp)){
								assert(ai2>=0);
								if(ai2>=0){
									if(!conflActivities[newtime].contains(ai2)){
										occupiedDay[d2]=true;
										if(fixedTimeActivity[ai2] || swappedActivities[ai2])
											canEmptyDay[d2]=false;
										else if(!_activitiesForDay[d2].contains(ai2)){
											_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
											_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
											_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
											_nConflActivities[d2]++;
											_activitiesForDay[d2].append(ai2);
											assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
										}
									}
								}
							}
						
							assert(occupiedDay[d2]);
							if(!occupiedDay[d2])
								canEmptyDay[d2]=false;
						}
					
						//int nOc=0;
						bool canChooseDay=false;
					
						for(int j=0; j<3+2+2; j++)
							if(occupiedDay[j]){
								//nOc++;
								if(canEmptyDay[j]){
									canChooseDay=true;
								}
							}
								
						if(!canChooseDay){
							if(level==0){
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachersmorningsafternoonsbehavior=false;
							goto impossibleteachersmorningsafternoonsbehavior;
						}
							
						int d2=-1;
							
						if(level!=0){
							//choose random day from those with minimum number of conflicting activities
							QList<int> candidateDays;
							
							int m=gt.rules.nInternalActivities;
							
							for(int kk=0; kk<3+2+2; kk++)
								if(canEmptyDay[kk])
									if(m>_nConflActivities[kk])
										m=_nConflActivities[kk];
						
							candidateDays.clear();
							for(int kk=0; kk<3+2+2; kk++)
								if(canEmptyDay[kk] && m==_nConflActivities[kk])
									candidateDays.append(kk);
									
							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}
						else{ //level==0
							QList<int> candidateDays;
		
							int _mW=INF;
							int _nW=INF;
							int _mCA=gt.rules.nInternalActivities;
							int _mIA=gt.rules.nInternalActivities;

							for(int kk=0; kk<3+2+2; kk++)
								if(canEmptyDay[kk]){
									if(_mW>_minWrong[kk] ||
									(_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
									(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
									(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
										_mW=_minWrong[kk];
										_nW=_nWrong[kk];
										_mCA=_nConflActivities[kk];
										_mIA=_minIndexAct[kk];
									}
								}
								
							assert(_mW<INF);
							
							candidateDays.clear();
							for(int kk=0; kk<3+2+2; kk++)
								if(canEmptyDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);
									
							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}
						
						assert(d2>=0);
		
						assert(_activitiesForDay[d2].count()>0);
		
						for(int ai2 : qAsConst(_activitiesForDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						}
					}
				}
				//2020-06-06 - for boulabat and ZINEDDINE18 - four days exception
				else if(morningsAfternoonsBehavior==TEACHER_FOUR_DAYS_EXCEPTION)
				//else if(gt.rules.internalExceptionTeachers4Set.contains(tc))
				{
					QList<int> pairedCurrentDay;

					for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
						if(teachersTimetable(tc,dpair,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dpair,hh)))
							pairedCurrentDay.append(teachersTimetable(tc,dpair,hh));
							
					if(pairedCurrentDay.count()==0)
						continue;

					QList<int> morningOther;
					QList<int> afternoonOther;
					QList<int> morningOther2;
					QList<int> afternoonOther2;
					QList<int> morningOther3;
					QList<int> afternoonOther3;
					QList<int> morningOther4;
					QList<int> afternoonOther4;
					
					int dother=-1, dother2=-1, dother3=-1, dother4=-1;
					for(int dd=0; dd<gt.rules.nDaysPerWeek/2; dd++){
						bool morning=false, afternoon=false;
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dd*2,hh)>=0)
								morning=true;
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dd*2+1,hh)>=0)
								afternoon=true;
						if(dd!=d/2 && morning && afternoon){
							assert(dd!=d/2);
							if(dother==-1){
								dother=dd;
							}
							else if(dother2==-1){
								dother2=dd;
							}
							else if(dother3==-1){
								dother3=dd;
							}
							else{
								assert(dother4==-1);
								dother4=dd;
							}
						}
					}
					if(dother>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother*2,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother*2,hh)))
								morningOther.append(teachersTimetable(tc,dother*2,hh));
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother*2+1,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother*2+1,hh)))
								afternoonOther.append(teachersTimetable(tc,dother*2+1,hh));
					}
					if(dother2>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother2*2,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother2*2,hh)))
								morningOther2.append(teachersTimetable(tc,dother2*2,hh));
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother2*2+1,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother2*2+1,hh)))
								afternoonOther2.append(teachersTimetable(tc,dother2*2+1,hh));
					}
					if(dother3>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother3*2,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother3*2,hh)))
								morningOther3.append(teachersTimetable(tc,dother3*2,hh));
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother3*2+1,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother3*2+1,hh)))
								afternoonOther3.append(teachersTimetable(tc,dother3*2+1,hh));
					}
					if(dother4>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother4*2,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother4*2,hh)))
								morningOther4.append(teachersTimetable(tc,dother4*2,hh));
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother4*2+1,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother4*2+1,hh)))
								afternoonOther4.append(teachersTimetable(tc,dother4*2+1,hh));
					}
					
					assert(pairedCurrentDay.count()>0);
					if(morningOther.count()>0 && afternoonOther.count()>0
					&& morningOther2.count()>0 && afternoonOther2.count()>0
					&& morningOther3.count()>0 && afternoonOther3.count()>0
					&& morningOther4.count()>0 && afternoonOther4.count()>0){
						//empty one of these 9
						
						///////////////////////////////////
						//0 is paired current day, 1 is morning other, 2 is afternoon other
						//3 is morning other 2, 4 is afternoon other 2
						//5 is morning other 3, 6 is afternoon other 3
						//7 is morning other 4, 8 is afternoon other 4
						
						bool occupiedDay[3+2+2+2];
						bool canEmptyDay[3+2+2+2];
					
						int _minWrong[3+2+2+2];
						int _nWrong[3+2+2+2];
						int _nConflActivities[3+2+2+2];
						int _minIndexAct[3+2+2+2];
					
						QList<int> _activitiesForDay[3+2+2+2];
						
						for(int d2=0; d2<3+2+2+2; d2++){
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;
							
							_minWrong[d2]=INF;
							_nWrong[d2]=0;
							_nConflActivities[d2]=0;
							_minIndexAct[d2]=gt.rules.nInternalActivities;
							_activitiesForDay[d2].clear();
							
							QList<int> tmp;
							if(d2==0)
								tmp=pairedCurrentDay;
							else if(d2==1)
								tmp=morningOther;
							else if(d2==2)
								tmp=afternoonOther;
							else if(d2==3)
								tmp=morningOther2;
							else if(d2==4)
								tmp=afternoonOther2;
							else if(d2==5)
								tmp=morningOther3;
							else if(d2==6)
								tmp=afternoonOther3;
							else if(d2==7)
								tmp=morningOther4;
							else if(d2==8)
								tmp=afternoonOther4;
							
							for(int ai2 : qAsConst(tmp)){
								assert(ai2>=0);
								if(ai2>=0){
									if(!conflActivities[newtime].contains(ai2)){
										occupiedDay[d2]=true;
										if(fixedTimeActivity[ai2] || swappedActivities[ai2])
											canEmptyDay[d2]=false;
										else if(!_activitiesForDay[d2].contains(ai2)){
											_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
											_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
											_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
											_nConflActivities[d2]++;
											_activitiesForDay[d2].append(ai2);
											assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
										}
									}
								}
							}
						
							assert(occupiedDay[d2]);
							if(!occupiedDay[d2])
								canEmptyDay[d2]=false;
						}
					
						//int nOc=0;
						bool canChooseDay=false;
					
						for(int j=0; j<3+2+2+2; j++)
							if(occupiedDay[j]){
								//nOc++;
								if(canEmptyDay[j]){
									canChooseDay=true;
								}
							}
						
						if(!canChooseDay){
							if(level==0){
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachersmorningsafternoonsbehavior=false;
							goto impossibleteachersmorningsafternoonsbehavior;
						}
							
						int d2=-1;
							
						if(level!=0){
							//choose random day from those with minimum number of conflicting activities
							QList<int> candidateDays;
							
							int m=gt.rules.nInternalActivities;
							
							for(int kk=0; kk<3+2+2+2; kk++)
								if(canEmptyDay[kk])
									if(m>_nConflActivities[kk])
										m=_nConflActivities[kk];
						
							candidateDays.clear();
							for(int kk=0; kk<3+2+2+2; kk++)
								if(canEmptyDay[kk] && m==_nConflActivities[kk])
									candidateDays.append(kk);
									
							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}
						else{ //level==0
							QList<int> candidateDays;
		
							int _mW=INF;
							int _nW=INF;
							int _mCA=gt.rules.nInternalActivities;
							int _mIA=gt.rules.nInternalActivities;

							for(int kk=0; kk<3+2+2+2; kk++)
								if(canEmptyDay[kk]){
									if(_mW>_minWrong[kk] ||
									(_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
									(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
									(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
										_mW=_minWrong[kk];
										_nW=_nWrong[kk];
										_mCA=_nConflActivities[kk];
										_mIA=_minIndexAct[kk];
									}
								}
								
							assert(_mW<INF);
							
							candidateDays.clear();
							for(int kk=0; kk<3+2+2+2; kk++)
								if(canEmptyDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);
									
							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}
						
						assert(d2>=0);
		
						assert(_activitiesForDay[d2].count()>0);
		
						for(int ai2 : qAsConst(_activitiesForDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						}
					}
				}
				//2020-06-06 - for boulabat and ZINEDDINE18 - five days exception
				else if(morningsAfternoonsBehavior==TEACHER_FIVE_DAYS_EXCEPTION)
				//else if(gt.rules.internalExceptionTeachers5Set.contains(tc))
				{
					QList<int> pairedCurrentDay;

					for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
						if(teachersTimetable(tc,dpair,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dpair,hh)))
							pairedCurrentDay.append(teachersTimetable(tc,dpair,hh));
							
					if(pairedCurrentDay.count()==0)
						continue;

					QList<int> morningOther;
					QList<int> afternoonOther;
					QList<int> morningOther2;
					QList<int> afternoonOther2;
					QList<int> morningOther3;
					QList<int> afternoonOther3;
					QList<int> morningOther4;
					QList<int> afternoonOther4;
					QList<int> morningOther5;
					QList<int> afternoonOther5;
					
					int dother=-1, dother2=-1, dother3=-1, dother4=-1, dother5=-1;
					for(int dd=0; dd<gt.rules.nDaysPerWeek/2; dd++){
						bool morning=false, afternoon=false;
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dd*2,hh)>=0)
								morning=true;
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dd*2+1,hh)>=0)
								afternoon=true;
						if(dd!=d/2 && morning && afternoon){
							assert(dd!=d/2);
							if(dother==-1){
								dother=dd;
							}
							else if(dother2==-1){
								dother2=dd;
							}
							else if(dother3==-1){
								dother3=dd;
							}
							else if(dother4==-1){
								dother4=dd;
							}
							else{
								assert(dother5==-1);
								dother5=dd;
							}
						}
					}
					if(dother>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother*2,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother*2,hh)))
								morningOther.append(teachersTimetable(tc,dother*2,hh));
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother*2+1,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother*2+1,hh)))
								afternoonOther.append(teachersTimetable(tc,dother*2+1,hh));
					}
					if(dother2>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother2*2,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother2*2,hh)))
								morningOther2.append(teachersTimetable(tc,dother2*2,hh));
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother2*2+1,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother2*2+1,hh)))
								afternoonOther2.append(teachersTimetable(tc,dother2*2+1,hh));
					}
					if(dother3>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother3*2,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother3*2,hh)))
								morningOther3.append(teachersTimetable(tc,dother3*2,hh));
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother3*2+1,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother3*2+1,hh)))
								afternoonOther3.append(teachersTimetable(tc,dother3*2+1,hh));
					}
					if(dother4>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother4*2,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother4*2,hh)))
								morningOther4.append(teachersTimetable(tc,dother4*2,hh));
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother4*2+1,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother4*2+1,hh)))
								afternoonOther4.append(teachersTimetable(tc,dother4*2+1,hh));
					}
					if(dother5>=0){
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother5*2,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother5*2,hh)))
								morningOther5.append(teachersTimetable(tc,dother5*2,hh));
						for(int hh=0; hh<gt.rules.nHoursPerDay; hh++)
							if(teachersTimetable(tc,dother5*2+1,hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc,dother5*2+1,hh)))
								afternoonOther5.append(teachersTimetable(tc,dother5*2+1,hh));
					}
					
					assert(pairedCurrentDay.count()>0);
					if(morningOther.count()>0 && afternoonOther.count()>0
					&& morningOther2.count()>0 && afternoonOther2.count()>0
					&& morningOther3.count()>0 && afternoonOther3.count()>0
					&& morningOther4.count()>0 && afternoonOther4.count()>0
					&& morningOther5.count()>0 && afternoonOther5.count()>0){
						//empty one of these 11
						
						///////////////////////////////////
						//0 is paired current day, 1 is morning other, 2 is afternoon other
						//3 is morning other 2, 4 is afternoon other 2
						//5 is morning other 3, 6 is afternoon other 3
						//7 is morning other 4, 8 is afternoon other 4
						//9 is morning other 5, 10 is afternoon other 5
						
						bool occupiedDay[3+2+2+2+2];
						bool canEmptyDay[3+2+2+2+2];
					
						int _minWrong[3+2+2+2+2];
						int _nWrong[3+2+2+2+2];
						int _nConflActivities[3+2+2+2+2];
						int _minIndexAct[3+2+2+2+2];
					
						QList<int> _activitiesForDay[3+2+2+2+2];
						
						for(int d2=0; d2<3+2+2+2+2; d2++){
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;
							
							_minWrong[d2]=INF;
							_nWrong[d2]=0;
							_nConflActivities[d2]=0;
							_minIndexAct[d2]=gt.rules.nInternalActivities;
							_activitiesForDay[d2].clear();
							
							QList<int> tmp;
							if(d2==0)
								tmp=pairedCurrentDay;
							else if(d2==1)
								tmp=morningOther;
							else if(d2==2)
								tmp=afternoonOther;
							else if(d2==3)
								tmp=morningOther2;
							else if(d2==4)
								tmp=afternoonOther2;
							else if(d2==5)
								tmp=morningOther3;
							else if(d2==6)
								tmp=afternoonOther3;
							else if(d2==7)
								tmp=morningOther4;
							else if(d2==8)
								tmp=afternoonOther4;
							else if(d2==9)
								tmp=morningOther5;
							else if(d2==10)
								tmp=afternoonOther5;
							
							for(int ai2 : qAsConst(tmp)){
								assert(ai2>=0);
								if(ai2>=0){
									if(!conflActivities[newtime].contains(ai2)){
										occupiedDay[d2]=true;
										if(fixedTimeActivity[ai2] || swappedActivities[ai2])
											canEmptyDay[d2]=false;
										else if(!_activitiesForDay[d2].contains(ai2)){
											_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
											_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
											_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
											_nConflActivities[d2]++;
											_activitiesForDay[d2].append(ai2);
											assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
										}
									}
								}
							}
						
							assert(occupiedDay[d2]);
							if(!occupiedDay[d2])
								canEmptyDay[d2]=false;
						}
					
						//int nOc=0;
						bool canChooseDay=false;
					
						for(int j=0; j<3+2+2+2+2; j++)
							if(occupiedDay[j]){
								//nOc++;
								if(canEmptyDay[j]){
									canChooseDay=true;
								}
							}
						
						if(!canChooseDay){
							if(level==0){
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachersmorningsafternoonsbehavior=false;
							goto impossibleteachersmorningsafternoonsbehavior;
						}
							
						int d2=-1;
							
						if(level!=0){
							//choose random day from those with minimum number of conflicting activities
							QList<int> candidateDays;
							
							int m=gt.rules.nInternalActivities;
							
							for(int kk=0; kk<3+2+2+2+2; kk++)
								if(canEmptyDay[kk])
									if(m>_nConflActivities[kk])
										m=_nConflActivities[kk];
						
							candidateDays.clear();
							for(int kk=0; kk<3+2+2+2+2; kk++)
								if(canEmptyDay[kk] && m==_nConflActivities[kk])
									candidateDays.append(kk);
									
							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}
						else{ //level==0
							QList<int> candidateDays;
		
							int _mW=INF;
							int _nW=INF;
							int _mCA=gt.rules.nInternalActivities;
							int _mIA=gt.rules.nInternalActivities;

							for(int kk=0; kk<3+2+2+2+2; kk++)
								if(canEmptyDay[kk]){
									if(_mW>_minWrong[kk] ||
									(_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
									(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
									(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
										_mW=_minWrong[kk];
										_nW=_nWrong[kk];
										_mCA=_nConflActivities[kk];
										_mIA=_minIndexAct[kk];
									}
								}
								
							assert(_mW<INF);
							
							candidateDays.clear();
							for(int kk=0; kk<3+2+2+2+2; kk++)
								if(canEmptyDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);
									
							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}
						
						assert(d2>=0);
		
						assert(_activitiesForDay[d2].count()>0);
		
						for(int ai2 : qAsConst(_activitiesForDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						}
					}
				}
				else if(morningsAfternoonsBehavior==TEACHER_MORNING_OR_EXCLUSIVELY_AFTERNOON)
				{ //should not have activities in both morning and afternoon, without exceptions.
					for(int hh=0; hh<gt.rules.nHoursPerDay; hh++){
						int ai2=teachersTimetable(tc,dpair,hh);
						if(ai2>=0){
							if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
								okteachersmorningsafternoonsbehavior=false;
								goto impossibleteachersmorningsafternoonsbehavior;
							}
							
							if(!conflActivities[newtime].contains(ai2)){
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							}
						}
					}
				}
				else
				{
					assert(morningsAfternoonsBehavior==TEACHER_UNRESTRICTED_MORNINGS_AFTERNOONS);
				}
			}
		}

impossibleteachersmorningsafternoonsbehavior:
		if(!okteachersmorningsafternoonsbehavior){
			//if(updateSubgroups || updateTeachers)
			//	removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		//////////////////
		//Teachers must work max 2 consecutive mornings/afternoons

		ok_max_two_consecutive_mornings_afternoons=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			d_first=d-4;
			d_last=d+4;

			d_pre=d-2;
			d_after=d+2;

			if((d_pre>=0 && d_after<gt.rules.nDaysPerWeek) || d_first>=0 || d_last<gt.rules.nDaysPerWeek){
				for(int tc : qAsConst(act->iTeachersList)){
					if((d%2==0 && teachersMaxTwoConsecutiveMorningsPercentage[tc]>=0) || (d%2==1 && teachersMaxTwoConsecutiveAfternoonsPercentage[tc]>=0)){
						if(d_pre>=0 && d_after<gt.rules.nDaysPerWeek){
							//d is in the middle
							QList<int> d_pre_activities_list;
							QSet<int> d_pre_activities_set;

							QList<int> d_after_activities_list;
							QSet<int> d_after_activities_set;

							for(int hh=0; hh<gt.rules.nHoursPerDay; hh++){
								if(teachersTimetable(tc, d_pre, hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc, d_pre, hh))){
									if(!d_pre_activities_set.contains(teachersTimetable(tc, d_pre, hh))){
										d_pre_activities_set.insert(teachersTimetable(tc, d_pre, hh));
										d_pre_activities_list.append(teachersTimetable(tc, d_pre, hh));
									}
								}

								if(teachersTimetable(tc, d_after, hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc, d_after, hh))){
									if(!d_after_activities_set.contains(teachersTimetable(tc, d_after, hh))){
										d_after_activities_set.insert(teachersTimetable(tc, d_after, hh));
										d_after_activities_list.append(teachersTimetable(tc, d_after, hh));
									}
								}
							}

							if(d_pre_activities_list.count()>0 && d_after_activities_list.count()>0){
								bool canEmptyDay[2]; //0 is d_pre, 1 is d_after

								int _minWrong[2];
								int _nWrong[2];
								int _nConflActivities[2];
								int _minIndexAct[2];
								QList<int> _activitiesForDay[2];

								for(int d2=0; d2<2; d2++){
									canEmptyDay[d2]=true;

									_minWrong[d2]=INF;
									_nWrong[d2]=0;
									_nConflActivities[d2]=0;
									_minIndexAct[d2]=gt.rules.nInternalActivities;

									QList<int> tmp;

									if(d2==0)
										tmp=d_pre_activities_list;
									else
										tmp=d_after_activities_list;

									for(int ai2 : qAsConst(tmp)){
										assert(ai2>=0);
										assert(!conflActivities[newtime].contains(ai2));

										if(fixedTimeActivity[ai2] || swappedActivities[ai2])
											canEmptyDay[d2]=false;
										else{
											_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
											_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
											_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
											_nConflActivities[d2]++;
											_activitiesForDay[d2].append(ai2);
											assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
										}
									}
								}

								if(canEmptyDay[0])
									assert(_nConflActivities[0]>0);
								if(canEmptyDay[1])
									assert(_nConflActivities[1]>0);

								if(!canEmptyDay[0] && !canEmptyDay[1]){
									if(level==0){
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									ok_max_two_consecutive_mornings_afternoons=false;
									goto impossible_max_two_consecutive_mornings_afternoons;
								}

								int d2=-1;

								if(level!=0){
									//choose random day from those with minimum number of conflicting activities
									QList<int> candidateDays;

									int m=gt.rules.nInternalActivities;

									for(int kk=0; kk<2; kk++)
										if(canEmptyDay[kk])
											if(m>_nConflActivities[kk])
												m=_nConflActivities[kk];

									candidateDays.clear();
									for(int kk=0; kk<2; kk++)
										if(canEmptyDay[kk] && m==_nConflActivities[kk])
											candidateDays.append(kk);

									assert(candidateDays.count()>0);
									d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
								}
								else{ //level==0
									QList<int> candidateDays;

									int _mW=INF;
									int _nW=INF;
									int _mCA=gt.rules.nInternalActivities;
									int _mIA=gt.rules.nInternalActivities;

									for(int kk=0; kk<2; kk++)
										if(canEmptyDay[kk]){
											if(_mW>_minWrong[kk] ||
											(_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
											(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
											(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
												_mW=_minWrong[kk];
												_nW=_nWrong[kk];
												_mCA=_nConflActivities[kk];
												_mIA=_minIndexAct[kk];
											}
										}

									assert(_mW<INF);

									candidateDays.clear();
									for(int kk=0; kk<2; kk++)
										if(canEmptyDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
											candidateDays.append(kk);

									assert(candidateDays.count()>0);
									d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
								}

								assert(d2>=0);

								assert(_activitiesForDay[d2].count()>0);

								for(int ai2 : qAsConst(_activitiesForDay[d2])){
									assert(ai2!=ai);
									assert(!swappedActivities[ai2]);
									assert(!fixedTimeActivity[ai2]);
									assert(!conflActivities[newtime].contains(ai2));
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}

								if(d2==0){ //we emptied d_pre
									d_last=d+4;
									d_first=-10;
								}
								else{ //we emptied d_after
									d_first=d-4;
									d_last=MAX_DAYS_PER_WEEK+10;
								}
							}
						}

						if(d_first>=0){
							//now we have d_first, d_pre, then d.
							//empty d_first or d_pre?
							//assert(d_last==MAX_DAYS_PER_WEEK+10);

							QList<int> d_first_activities_list;
							QSet<int> d_first_activities_set;

							QList<int> d_pre_activities_list;
							QSet<int> d_pre_activities_set;

							for(int hh=0; hh<gt.rules.nHoursPerDay; hh++){
								if(teachersTimetable(tc, d_first, hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc, d_first, hh))){
									if(!d_first_activities_set.contains(teachersTimetable(tc, d_first, hh))){
										d_first_activities_set.insert(teachersTimetable(tc, d_first, hh));
										d_first_activities_list.append(teachersTimetable(tc, d_first, hh));
									}
								}

								if(teachersTimetable(tc, d_pre, hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc, d_pre, hh))){
									if(!d_pre_activities_set.contains(teachersTimetable(tc, d_pre, hh))){
										d_pre_activities_set.insert(teachersTimetable(tc, d_pre, hh));
										d_pre_activities_list.append(teachersTimetable(tc, d_pre, hh));
									}
								}
							}

							if(d_first_activities_list.count()>0 && d_pre_activities_list.count()>0){
								bool canEmptyDay[2]; //0 is d_first, 1 is d_pre

								int _minWrong[2];
								int _nWrong[2];
								int _nConflActivities[2];
								int _minIndexAct[2];
								QList<int> _activitiesForDay[2];

								for(int d2=0; d2<2; d2++){
									canEmptyDay[d2]=true;

									_minWrong[d2]=INF;
									_nWrong[d2]=0;
									_nConflActivities[d2]=0;
									_minIndexAct[d2]=gt.rules.nInternalActivities;

									QList<int> tmp;

									if(d2==0)
										tmp=d_first_activities_list;
									else
										tmp=d_pre_activities_list;

									for(int ai2 : qAsConst(tmp)){
										assert(ai2>=0);
										assert(!conflActivities[newtime].contains(ai2));

										if(fixedTimeActivity[ai2] || swappedActivities[ai2])
											canEmptyDay[d2]=false;
										else{
											_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
											_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
											_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
											_nConflActivities[d2]++;
											_activitiesForDay[d2].append(ai2);
											assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
										}
									}
								}

								if(canEmptyDay[0])
									assert(_nConflActivities[0]>0);
								if(canEmptyDay[1])
									assert(_nConflActivities[1]>0);

								if(!canEmptyDay[0] && !canEmptyDay[1]){
									if(level==0){
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									ok_max_two_consecutive_mornings_afternoons=false;
									goto impossible_max_two_consecutive_mornings_afternoons;
								}

								int d2=-1;

								if(level!=0){
									//choose random day from those with minimum number of conflicting activities
									QList<int> candidateDays;

									int m=gt.rules.nInternalActivities;

									for(int kk=0; kk<2; kk++)
										if(canEmptyDay[kk])
											if(m>_nConflActivities[kk])
												m=_nConflActivities[kk];

									candidateDays.clear();
									for(int kk=0; kk<2; kk++)
										if(canEmptyDay[kk] && m==_nConflActivities[kk])
											candidateDays.append(kk);

									assert(candidateDays.count()>0);
									d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
								}
								else{ //level==0
									QList<int> candidateDays;

									int _mW=INF;
									int _nW=INF;
									int _mCA=gt.rules.nInternalActivities;
									int _mIA=gt.rules.nInternalActivities;

									for(int kk=0; kk<2; kk++)
										if(canEmptyDay[kk]){
											if(_mW>_minWrong[kk] ||
											(_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
											(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
											(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
												_mW=_minWrong[kk];
												_nW=_nWrong[kk];
												_mCA=_nConflActivities[kk];
												_mIA=_minIndexAct[kk];
											}
										}

									assert(_mW<INF);

									candidateDays.clear();
									for(int kk=0; kk<2; kk++)
										if(canEmptyDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
											candidateDays.append(kk);

									assert(candidateDays.count()>0);
									d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
								}

								assert(d2>=0);

								assert(_activitiesForDay[d2].count()>0);

								for(int ai2 : qAsConst(_activitiesForDay[d2])){
									assert(ai2!=ai);
									assert(!swappedActivities[ai2]);
									assert(!fixedTimeActivity[ai2]);
									assert(!conflActivities[newtime].contains(ai2));
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}
							}
						}

						if(d_last<gt.rules.nDaysPerWeek){
							//now we have d, d_after, then d_last.
							//empty d_after or d_last?
							//assert(d_first==-10);

							QList<int> d_after_activities_list;
							QSet<int> d_after_activities_set;

							QList<int> d_last_activities_list;
							QSet<int> d_last_activities_set;

							for(int hh=0; hh<gt.rules.nHoursPerDay; hh++){
								if(teachersTimetable(tc, d_after, hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc, d_after, hh))){
									if(!d_after_activities_set.contains(teachersTimetable(tc, d_after, hh))){
										d_after_activities_set.insert(teachersTimetable(tc, d_after, hh));
										d_after_activities_list.append(teachersTimetable(tc, d_after, hh));
									}
								}

								if(teachersTimetable(tc, d_last, hh)>=0 && !conflActivities[newtime].contains(teachersTimetable(tc, d_last, hh))){
									if(!d_last_activities_set.contains(teachersTimetable(tc, d_last, hh))){
										d_last_activities_set.insert(teachersTimetable(tc, d_last, hh));
										d_last_activities_list.append(teachersTimetable(tc, d_last, hh));
									}
								}
							}

							if(d_after_activities_list.count()>0 && d_last_activities_list.count()>0){
								bool canEmptyDay[2]; //0 is d_first, 1 is d_pre

								int _minWrong[2];
								int _nWrong[2];
								int _nConflActivities[2];
								int _minIndexAct[2];
								QList<int> _activitiesForDay[2];

								for(int d2=0; d2<2; d2++){
									canEmptyDay[d2]=true;

									_minWrong[d2]=INF;
									_nWrong[d2]=0;
									_nConflActivities[d2]=0;
									_minIndexAct[d2]=gt.rules.nInternalActivities;

									QList<int> tmp;

									if(d2==0)
										tmp=d_after_activities_list;
									else
										tmp=d_last_activities_list;

									for(int ai2 : qAsConst(tmp)){
										assert(ai2>=0);
										assert(!conflActivities[newtime].contains(ai2));

										if(fixedTimeActivity[ai2] || swappedActivities[ai2])
											canEmptyDay[d2]=false;
										else{
											_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
											_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
											_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
											_nConflActivities[d2]++;
											_activitiesForDay[d2].append(ai2);
											assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
										}
									}
								}

								if(canEmptyDay[0])
									assert(_nConflActivities[0]>0);
								if(canEmptyDay[1])
									assert(_nConflActivities[1]>0);

								if(!canEmptyDay[0] && !canEmptyDay[1]){
									if(level==0){
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									ok_max_two_consecutive_mornings_afternoons=false;
									goto impossible_max_two_consecutive_mornings_afternoons;
								}

								int d2=-1;

								if(level!=0){
									//choose random day from those with minimum number of conflicting activities
									QList<int> candidateDays;

									int m=gt.rules.nInternalActivities;

									for(int kk=0; kk<2; kk++)
										if(canEmptyDay[kk])
											if(m>_nConflActivities[kk])
												m=_nConflActivities[kk];

									candidateDays.clear();
									for(int kk=0; kk<2; kk++)
										if(canEmptyDay[kk] && m==_nConflActivities[kk])
											candidateDays.append(kk);

									assert(candidateDays.count()>0);
									d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
								}
								else{ //level==0
									QList<int> candidateDays;

									int _mW=INF;
									int _nW=INF;
									int _mCA=gt.rules.nInternalActivities;
									int _mIA=gt.rules.nInternalActivities;

									for(int kk=0; kk<2; kk++)
										if(canEmptyDay[kk]){
											if(_mW>_minWrong[kk] ||
											(_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
											(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
											(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
												_mW=_minWrong[kk];
												_nW=_nWrong[kk];
												_mCA=_nConflActivities[kk];
												_mIA=_minIndexAct[kk];
											}
										}

									assert(_mW<INF);

									candidateDays.clear();
									for(int kk=0; kk<2; kk++)
										if(canEmptyDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
											candidateDays.append(kk);

									assert(candidateDays.count()>0);
									d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
								}

								assert(d2>=0);

								assert(_activitiesForDay[d2].count()>0);

								for(int ai2 : qAsConst(_activitiesForDay[d2])){
									assert(ai2!=ai);
									assert(!swappedActivities[ai2]);
									assert(!fixedTimeActivity[ai2]);
									assert(!conflActivities[newtime].contains(ai2));
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
				//shouldn't appear - too few FET days (real half days) per week
			}

impossible_max_two_consecutive_mornings_afternoons:
			if(!ok_max_two_consecutive_mornings_afternoons){
				//if(updateSubgroups || updateTeachers)
				//	removeAiFromNewTimetable(ai, act, d, h);
				//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

				nConflActivities[newtime]=MAX_ACTIVITIES;
				continue;
			}
		}

/////////////////////////////////////////////////////////////////////////////////////////////
		
		//////////////////////////////////////////////////
		if(updateSubgroups || updateTeachers){
			addAiToNewTimetable(ai, act, d, h);
			if(updateTeachers){
				updateTeachersNHoursGaps(ai, d);
				if(gt.rules.mode==MORNINGS_AFTERNOONS && haveTeachersMaxGapsPerRealDay)
					updateTeachersNHoursGapsRealDay(ai, d/2);
			}
			if(updateSubgroups){
				updateSubgroupsNHoursGaps(ai, d);
				if(gt.rules.mode==MORNINGS_AFTERNOONS && haveStudentsMaxGapsPerRealDay)
					updateSubgroupsNHoursGapsRealDay(ai, d/2);
			}
		}
		//////////////////////////////////////////////////
		
/////////////////////////////////////////////////////////////////////////////////////////////
		
		////////////STUDENTS////////////////
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking the students max days per week constraints
		////////////////////////////BEGIN max days per week for students
		okstudentsmaxdaysperweek=true;
		for(int st : qAsConst(subgroupsWithMaxDaysPerWeekForActivities[ai])){
			if(skipRandom(subgroupsMaxDaysPerWeekWeightPercentages[st]))
				continue;

			int maxDays=subgroupsMaxDaysPerWeekMaxDays[st];
			assert(maxDays>=0); //the list contains real information
			
			//preliminary test
			int _nOc=0;
			for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
				//comments taken from teachers constraint:
				//if(newTeachersDayNHours(tch,d2)>0)
				
				//IT IS VITAL TO USE teacherActivitiesOfTheDay as a QList<int> (tch,d2)!!!!!!!
				//The order of evaluation of activities is changed,
				//with activities which were moved forward and back again
				//being put at the end.
				//If you do not follow this, you'll get impossible timetables
				//for the Italian example Italy/2007/difficult/highschool-Ancona.fet or the examples from
				//South Africa: South-Africa/difficult/Collegiate_Junior_School2.fet or
				//South-Africa/difficult/Insight_Learning_Centre2.fet, I am not sure which of these 3
				
				if(subgroupActivitiesOfTheDay(st,d2).count()>0 || d2==d)
					_nOc++;
			if(_nOc<=maxDays)
				continue; //OK, preliminary
			
			if(maxDays>=0){
				assert(maxDays>0);

				if(level>0){
					///getTchTimetable(tch, conflActivities[newtime]);
					///tchGetNHoursGaps(tch);

					//bool occupiedDay[MAX_DAYS_PER_WEEK];
					//bool canEmptyDay[MAX_DAYS_PER_WEEK];
					
					///int _minWrong[MAX_DAYS_PER_WEEK];
					///int _nWrong[MAX_DAYS_PER_WEEK];
					//int _nConflActivities[MAX_DAYS_PER_WEEK];
					///int _minIndexAct[MAX_DAYS_PER_WEEK];
					
					//QList<int> _activitiesForDay[MAX_DAYS_PER_WEEK];

					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						if(d2==d)
							continue;
					
						occupiedDay[d2]=false;
						canEmptyDay[d2]=true;
						
						//_minWrong[d2]=INF;
						//_nWrong[d2]=0;
						_nConflActivities[d2]=0;
						//_minIndexAct[d2]=gt.rules.nInternalActivities;
						_activitiesForDay[d2].clear();
						
						for(int ai2 : qAsConst(subgroupActivitiesOfTheDay(st,d2))){
							if(ai2>=0){
								if(!conflActivities[newtime].contains(ai2)){
									occupiedDay[d2]=true;
									if(fixedTimeActivity[ai2] || swappedActivities[ai2])
										canEmptyDay[d2]=false;
									else if(!_activitiesForDay[d2].contains(ai2)){
										//_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
										//_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
										//_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
										_nConflActivities[d2]++;
										_activitiesForDay[d2].append(ai2);
										assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
									}
								}
							}
						}
						
						if(!occupiedDay[d2])
							canEmptyDay[d2]=false;
					}
					occupiedDay[d]=true;
					canEmptyDay[d]=false;
					
					int nOc=0;
					bool canChooseDay=false;
					
					for(int j=0; j<gt.rules.nDaysPerWeek; j++)
						if(occupiedDay[j]){
							nOc++;
							if(canEmptyDay[j]){
								canChooseDay=true;
							}
						}
					
					if(nOc>maxDays){
						assert(nOc==maxDays+1);
						
						if(!canChooseDay){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okstudentsmaxdaysperweek=false;
							goto impossiblestudentsmaxdaysperweek;
						}
						
						int d2=-1;
						
						///////////////////
						//choose a random day from those with minimum number of conflicting activities
						QList<int> candidateDays;
						
						int m=gt.rules.nInternalActivities;
						
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyDay[kk])
								if(m>_nConflActivities[kk])
									m=_nConflActivities[kk];
						
						candidateDays.clear();
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyDay[kk])
								if(m==_nConflActivities[kk])
									candidateDays.append(kk);
								
						assert(candidateDays.count()>0);
						d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						//////////////////

						assert(d2>=0);

						assert(_activitiesForDay[d2].count()>0);

						for(int ai2 : qAsConst(_activitiesForDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						}
					}
				}
				else{
					assert(level==0);
					///getTchTimetable(tch, conflActivities[newtime]);
					///tchGetNHoursGaps(tch);

					//bool occupiedDay[MAX_DAYS_PER_WEEK];
					//bool canEmptyDay[MAX_DAYS_PER_WEEK];
					
					//int _minWrong[MAX_DAYS_PER_WEEK];
					//int _nWrong[MAX_DAYS_PER_WEEK];
					//int _nConflActivities[MAX_DAYS_PER_WEEK];
					//int _minIndexAct[MAX_DAYS_PER_WEEK];
					
					//QList<int> _activitiesForDay[MAX_DAYS_PER_WEEK];

					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						if(d2==d)
							continue;
					
						occupiedDay[d2]=false;
						canEmptyDay[d2]=true;
						
						_minWrong[d2]=INF;
						_nWrong[d2]=0;
						_nConflActivities[d2]=0;
						_minIndexAct[d2]=gt.rules.nInternalActivities;
						_activitiesForDay[d2].clear();
						
						for(int ai2 : qAsConst(subgroupActivitiesOfTheDay(st,d2))){
							if(ai2>=0){
								if(!conflActivities[newtime].contains(ai2)){
									occupiedDay[d2]=true;
									if(fixedTimeActivity[ai2] || swappedActivities[ai2])
										canEmptyDay[d2]=false;
									else if(!_activitiesForDay[d2].contains(ai2)){
										_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
										_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
										_nConflActivities[d2]++;
										_activitiesForDay[d2].append(ai2);
										assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
									}
								}
							}
						}
						
						if(!occupiedDay[d2])
							canEmptyDay[d2]=false;
					}
					occupiedDay[d]=true;
					canEmptyDay[d]=false;
					
					int nOc=0;
					bool canChooseDay=false;
					
					for(int j=0; j<gt.rules.nDaysPerWeek; j++)
						if(occupiedDay[j]){
							nOc++;
							if(canEmptyDay[j]){
								canChooseDay=true;
							}
						}
					
					if(nOc>maxDays){
						assert(nOc==maxDays+1);
						
						if(!canChooseDay){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okstudentsmaxdaysperweek=false;
							goto impossiblestudentsmaxdaysperweek;
						}
						
						int d2=-1;
						
						//////////////////
						QList<int> candidateDays;

						int _mW=INF;
						int _nW=INF;
						int _mCA=gt.rules.nInternalActivities;
						int _mIA=gt.rules.nInternalActivities;

						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyDay[kk]){
								if(_mW>_minWrong[kk] ||
								(_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
								(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
								(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
									_mW=_minWrong[kk];
									_nW=_nWrong[kk];
									_mCA=_nConflActivities[kk];
									_mIA=_minIndexAct[kk];
								}
							}
							
						assert(_mW<INF);
						
						candidateDays.clear();
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyDay[kk])
								if(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);
								
						assert(candidateDays.count()>0);
						d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						///////////////////
						
						assert(d2>=0);

						assert(_activitiesForDay[d2].count()>0);

						for(int ai2 : qAsConst(_activitiesForDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						}
					}
				}
			}
		}
impossiblestudentsmaxdaysperweek:
		if(!okstudentsmaxdaysperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students max days per week

/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking the students max three consecutive days
		////////////////////////////BEGIN max three consecutive days for students
		okstudentsmaxthreeconsecutivedays=true;
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int sbg : qAsConst(subgroupsWithMaxThreeConsecutiveDaysForActivities[ai])){
				if(!skipRandom(subgroupsMaxThreeConsecutiveDaysPercentages[sbg])){
					int maxDays=3;
					bool allowExceptionAMAM=subgroupsMaxThreeConsecutiveDaysAllowAMAMException[sbg];
					
					//preliminary test
					int _nOc=1;
					int _dstart=d, _dend=d;
					for(int d2=d-1; d2>=0; d2--){
						if(subgroupActivitiesOfTheDay(sbg,d2).count()>0){
							_nOc++;
							_dstart=d2;
						}
						else{
							break;
						}
					}
					for(int d2=d+1; d2<gt.rules.nDaysPerWeek; d2++){
						if(subgroupActivitiesOfTheDay(sbg,d2).count()>0){
							_nOc++;
							_dend=d2;
						}
						else{
							break;
						}
					}

					assert(_dstart>=0);
					assert(_dend>=0);

					if(_nOc<=maxDays || (allowExceptionAMAM && _dend-_dstart==maxDays && _dstart%2==1 && _dend%2==0))
						continue; //OK, preliminary

					if(level>0){
						occupiedDay[d]=true;
						canEmptyDay[d]=false;
					
						for(int d2=d-1; d2>=0; d2--){
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;

							_nConflActivities[d2]=0;
							_activitiesForDay[d2].clear();
							
							if(subgroupActivitiesOfTheDay(sbg,d2).count()>0){
								for(int ai2 : qAsConst(subgroupActivitiesOfTheDay(sbg,d2))){
									if(ai2>=0){
										if(!conflActivities[newtime].contains(ai2)){
											occupiedDay[d2]=true;
											if(fixedTimeActivity[ai2] || swappedActivities[ai2])
												canEmptyDay[d2]=false;
											else if(!_activitiesForDay[d2].contains(ai2)){
												_nConflActivities[d2]++;
												_activitiesForDay[d2].append(ai2);
												assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
											}
										}
									}
								}
							}

							if(!occupiedDay[d2])
								break;
						}
						
						for(int d2=d+1; d2<gt.rules.nDaysPerWeek; d2++){
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;

							_nConflActivities[d2]=0;
							_activitiesForDay[d2].clear();
							
							if(subgroupActivitiesOfTheDay(sbg,d2).count()>0){
								for(int ai2 : qAsConst(subgroupActivitiesOfTheDay(sbg,d2))){
									if(ai2>=0){
										if(!conflActivities[newtime].contains(ai2)){
											occupiedDay[d2]=true;
											if(fixedTimeActivity[ai2] || swappedActivities[ai2])
												canEmptyDay[d2]=false;
											else if(!_activitiesForDay[d2].contains(ai2)){
												_nConflActivities[d2]++;
												_activitiesForDay[d2].append(ai2);
												assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
											}
										}
									}
								}
							}

							if(!occupiedDay[d2])
								break;
						}

						for(;;){
							int dstart=d, dend=d;
							int nOc=1;

							for(int d2=d-1; d2>=0; d2--){
								if(occupiedDay[d2]){
									nOc++;
									dstart=d2;
								}
								else{
									break;
								}
							}
							for(int d2=d+1; d2<gt.rules.nDaysPerWeek; d2++){
								if(occupiedDay[d2]){
									nOc++;
									dend=d2;
								}
								else{
									break;
								}
							}

							if(nOc<=maxDays || (allowExceptionAMAM && dend-dstart==maxDays && dstart%2==1 && dend%2==0))
								break;
							
							bool canChooseDay=false;
							
							for(int j=dstart; j<=dend; j++)
								if(occupiedDay[j]){
									if(canEmptyDay[j]){
										canChooseDay=true;
									}
								}
							
							if(!canChooseDay){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okstudentsmaxthreeconsecutivedays=false;
								goto impossiblestudentsmaxthreeconsecutivedays;
							}
							
							int d2=-1;
							
							////////////////
							//choose a random day from those with minimum number of conflicting activities
							QList<int> candidateDays;
							
							int m=gt.rules.nInternalActivities;
							
							for(int kk=dstart; kk<=dend; kk++)
								if(occupiedDay[kk] && canEmptyDay[kk])
									if(m>_nConflActivities[kk])
										m=_nConflActivities[kk];
							
							candidateDays.clear();
							for(int kk=dstart; kk<=dend; kk++)
								if(occupiedDay[kk] && canEmptyDay[kk])
									if(m==_nConflActivities[kk])
										candidateDays.append(kk);
							
							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
							/////////////////
							
							assert(d2>=0);

							assert(_activitiesForDay[d2].count()>0);

							for(int ai2 : qAsConst(_activitiesForDay[d2])){
								assert(ai2!=ai);
								assert(!swappedActivities[ai2]);
								assert(!fixedTimeActivity[ai2]);
								assert(!conflActivities[newtime].contains(ai2));
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							}
							
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;
						}
					}
					else{
						assert(level==0);

						occupiedDay[d]=true;
						canEmptyDay[d]=false;
					
						for(int d2=d-1; d2>=0; d2--){
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;

							_minWrong[d2]=INF;
							_nWrong[d2]=0;
							_nConflActivities[d2]=0;
							_minIndexAct[d2]=gt.rules.nInternalActivities;
							_activitiesForDay[d2].clear();
							
							if(subgroupActivitiesOfTheDay(sbg,d2).count()>0){
								for(int ai2 : qAsConst(subgroupActivitiesOfTheDay(sbg,d2))){
									if(ai2>=0){
										if(!conflActivities[newtime].contains(ai2)){
											occupiedDay[d2]=true;
											if(fixedTimeActivity[ai2] || swappedActivities[ai2])
												canEmptyDay[d2]=false;
											else if(!_activitiesForDay[d2].contains(ai2)){
												_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
												_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
												_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
												_nConflActivities[d2]++;
												_activitiesForDay[d2].append(ai2);
												assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
											}
										}
									}
								}
							}

							if(!occupiedDay[d2])
								break;
						}
						
						for(int d2=d+1; d2<gt.rules.nDaysPerWeek; d2++){
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;

							_minWrong[d2]=INF;
							_nWrong[d2]=0;
							_nConflActivities[d2]=0;
							_minIndexAct[d2]=gt.rules.nInternalActivities;
							_activitiesForDay[d2].clear();
							
							if(subgroupActivitiesOfTheDay(sbg,d2).count()>0){
								for(int ai2 : qAsConst(subgroupActivitiesOfTheDay(sbg,d2))){
									if(ai2>=0){
										if(!conflActivities[newtime].contains(ai2)){
											occupiedDay[d2]=true;
											if(fixedTimeActivity[ai2] || swappedActivities[ai2])
												canEmptyDay[d2]=false;
											else if(!_activitiesForDay[d2].contains(ai2)){
												_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
												_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
												_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
												_nConflActivities[d2]++;
												_activitiesForDay[d2].append(ai2);
												assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
											}
										}
									}
								}
							}

							if(!occupiedDay[d2])
								break;
						}

						for(;;){
							int dstart=d, dend=d;
							int nOc=1;

							for(int d2=d-1; d2>=0; d2--){
								if(occupiedDay[d2]){
									nOc++;
									dstart=d2;
								}
								else{
									break;
								}
							}
							for(int d2=d+1; d2<gt.rules.nDaysPerWeek; d2++){
								if(occupiedDay[d2]){
									nOc++;
									dend=d2;
								}
								else{
									break;
								}
							}

							if(nOc<=maxDays || (allowExceptionAMAM && dend-dstart==maxDays && dstart%2==1 && dend%2==0))
								break;
							
							bool canChooseDay=false;
							
							for(int j=dstart; j<=dend; j++)
								if(occupiedDay[j]){
									if(canEmptyDay[j]){
										canChooseDay=true;
									}
								}
							
							if(!canChooseDay){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okstudentsmaxthreeconsecutivedays=false;
								goto impossiblestudentsmaxthreeconsecutivedays;
							}
							
							int d2=-1;
							
							////////////////
							//choose a random day from those with minimum number of conflicting activities
							QList<int> candidateDays;
							
							int _mW=INF;
							int _nW=INF;
							int _mCA=gt.rules.nInternalActivities;
							int _mIA=gt.rules.nInternalActivities;
							
							for(int kk=dstart; kk<=dend; kk++)
								if(occupiedDay[kk] && canEmptyDay[kk])
									if(_mW>_minWrong[kk] ||
									 (_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
										_mW=_minWrong[kk];
										_nW=_nWrong[kk];
										_mCA=_nConflActivities[kk];
										_mIA=_minIndexAct[kk];
									}
							
							candidateDays.clear();
							for(int kk=dstart; kk<=dend; kk++)
								if(occupiedDay[kk] && canEmptyDay[kk])
									if(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
										candidateDays.append(kk);
							
							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
							/////////////////
							
							assert(d2>=0);

							assert(_activitiesForDay[d2].count()>0);

							for(int ai2 : qAsConst(_activitiesForDay[d2])){
								assert(ai2!=ai);
								assert(!swappedActivities[ai2]);
								assert(!fixedTimeActivity[ai2]);
								assert(!conflActivities[newtime].contains(ai2));
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							}
							
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;
						}
					}
				}
			}
		}
impossiblestudentsmaxthreeconsecutivedays:
		if(!okstudentsmaxthreeconsecutivedays){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students max three consecutive days

/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking the students max real days per week constraints
		////////////////////////////BEGIN max real days per week for students
		okstudentsmaxrealdaysperweek=true;
		//comments taken from teachers constraint:
		//for(int tch : qAsConst(act->iTeachersList)){
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			//comments taken from teachers constraint:
			//for(int tch : qAsConst(act->iTeachersList)){
			for(int st : qAsConst(subgroupsWithMaxRealDaysPerWeekForActivities[ai])){
				if(skipRandom(subgroupsMaxRealDaysPerWeekWeightPercentages[st]))
					continue;

				int maxDays=subgroupsMaxRealDaysPerWeekMaxDays[st];
				assert(maxDays>=0); //the list contains real information

				//preliminary test
				int _nOc=0;
				for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++)
					//comments taken from teachers constraint:
					//if(newTeachersDayNHours(tch,d2)>0)

					//IT IS VITAL TO USE teacherActivitiesOfTheDay as a QList<int> (tch,d2)!!!!!!!
					//The order of evaluation of activities is changed,
					//with activities which were moved forward and back again
					//being put at the end.
					//If you do not follow this, you'll get impossible timetables
					//for the Italian example Italy/2007/difficult/highschool-Ancona.fet or the examples from
					//South Africa: South-Africa/difficult/Collegiate_Junior_School2.fet or
					//South-Africa/difficult/Insight_Learning_Centre2.fet, I am not sure which of these 3

					if(subgroupActivitiesOfTheDay(st,2*d2).count()+subgroupActivitiesOfTheDay(st,2*d2+1).count()>0 || d2==d/2)
						_nOc++;
				if(_nOc<=maxDays)
					continue; //OK, preliminary

				if(maxDays>=0){
					assert(maxDays>0);

					//getTchTimetable(tch, conflActivities[newtime]);
					//tchGetNHoursGaps(tch);

					//bool occupiedDay[MAX_DAYS_PER_WEEK]; //should be MAX_DAYS_PER_WEEK/2, but doesn't matter
					//bool canEmptyDay[MAX_DAYS_PER_WEEK];

					//int _minWrong[MAX_DAYS_PER_WEEK];
					//int _nWrong[MAX_DAYS_PER_WEEK];
					//int _nConflActivities[MAX_DAYS_PER_WEEK];
					//int _minIndexAct[MAX_DAYS_PER_WEEK];

					//QList<int> _activitiesForDay[MAX_DAYS_PER_WEEK];

					for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
						if(d2==d/2)
							continue;

						occupiedDay[d2]=false;
						canEmptyDay[d2]=true;

						_minWrong[d2]=INF;
						_nWrong[d2]=0;
						_nConflActivities[d2]=0;
						_minIndexAct[d2]=gt.rules.nInternalActivities;
						_activitiesForDay[d2].clear();

						for(int ai2 : qAsConst(subgroupActivitiesOfTheDay(st,2*d2))){
							if(ai2>=0){
								if(!conflActivities[newtime].contains(ai2)){
									occupiedDay[d2]=true;
									if(fixedTimeActivity[ai2] || swappedActivities[ai2])
										canEmptyDay[d2]=false;
									else if(!_activitiesForDay[d2].contains(ai2)){
										_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
										_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
										_nConflActivities[d2]++;
										_activitiesForDay[d2].append(ai2);
										assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
									}
								}
							}
						}
						for(int ai2 : qAsConst(subgroupActivitiesOfTheDay(st,2*d2+1))){
							if(ai2>=0){
								if(!conflActivities[newtime].contains(ai2)){
									occupiedDay[d2]=true;
									if(fixedTimeActivity[ai2] || swappedActivities[ai2])
										canEmptyDay[d2]=false;
									else if(!_activitiesForDay[d2].contains(ai2)){
										_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
										_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
										_nConflActivities[d2]++;
										_activitiesForDay[d2].append(ai2);
										assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
									}
								}
							}
						}

						if(!occupiedDay[d2])
							canEmptyDay[d2]=false;
					}
					occupiedDay[d/2]=true;
					canEmptyDay[d/2]=false;

					int nOc=0;
					bool canChooseDay=false;

					for(int j=0; j<gt.rules.nDaysPerWeek/2; j++)
						if(occupiedDay[j]){
							nOc++;
							if(canEmptyDay[j]){
								canChooseDay=true;
							}
						}

					if(nOc>maxDays){
						assert(nOc==maxDays+1);

						if(!canChooseDay){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okstudentsmaxrealdaysperweek=false;
							goto impossiblestudentsmaxrealdaysperweek;
						}

						int d2=-1;

						if(level!=0){
							//choose random day from those with minimum number of conflicting activities
							QList<int> candidateDays;

							int m=gt.rules.nInternalActivities;

							for(int kk=0; kk<gt.rules.nDaysPerWeek/2; kk++)
								if(canEmptyDay[kk])
									if(m>_nConflActivities[kk])
										m=_nConflActivities[kk];

							candidateDays.clear();
							for(int kk=0; kk<gt.rules.nDaysPerWeek/2; kk++)
								if(canEmptyDay[kk] && m==_nConflActivities[kk])
									candidateDays.append(kk);

							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}
						else{ //level==0
							QList<int> candidateDays;

							int _mW=INF;
							int _nW=INF;
							int _mCA=gt.rules.nInternalActivities;
							int _mIA=gt.rules.nInternalActivities;

							for(int kk=0; kk<gt.rules.nDaysPerWeek/2; kk++)
								if(canEmptyDay[kk]){
									if(_mW>_minWrong[kk] ||
									 (_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
										_mW=_minWrong[kk];
										_nW=_nWrong[kk];
										_mCA=_nConflActivities[kk];
										_mIA=_minIndexAct[kk];
									}
								}

							assert(_mW<INF);

							candidateDays.clear();
							for(int kk=0; kk<gt.rules.nDaysPerWeek/2; kk++)
								if(canEmptyDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);

							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}

						assert(d2>=0);

						assert(_activitiesForDay[d2].count()>0);

						for(int ai2 : qAsConst(_activitiesForDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
						}
					}
				}
			}
		}
impossiblestudentsmaxrealdaysperweek:
		if(!okstudentsmaxrealdaysperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students max real days per week

/////////////////////////////////////////////////////////////////////////////////////////////

		okstudentsafternoonsearlymaxbeginningsatsecondhour=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			//for lakhdarbe - 2020-07-24
			if(haveStudentsAfternoonsEarly){
				for(int sbg : qAsConst(act->iSubgroupsList))
					if(!skipRandom(subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[sbg])){
						//preliminary check
						int mhd=1; //min hours per day
						if(subgroupsMinHoursDailyMinHours[sbg][1]>=0)
							mhd=subgroupsMinHoursDailyMinHours[sbg][1];
						assert(mhd>=1);
						int mhm=mhd; //min hours per morning
						if(subgroupsMinHoursDailyMinHours[sbg][0]>=0)
							mhm=subgroupsMinHoursDailyMinHours[sbg][0];
						assert(mhm>=mhd);

						int mhaft=mhd; //min hours per afternoon
						if(subgroupsMinHoursPerAfternoonMinHours[sbg]>=0)
							mhaft=subgroupsMinHoursPerAfternoonMinHours[sbg];
						assert(mhaft>=mhd);

						bool maxGapsZero=false;
						if(subgroupsMaxGapsPerDayMaxGaps[sbg]==0 ||
						 subgroupsMaxGapsPerWeekMaxGaps[sbg]==0 ||
						 subgroupsMaxGapsPerRealDayMaxGaps[sbg]==0 ||
						 subgroupsMaxGapsPerWeekForRealDaysMaxGaps[sbg]==0)
							maxGapsZero=true;

						int _nUsedMornings=0;
						int _nUsedAfternoons=0;

						int _nHours=0;
						int _nfg=0;

						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							if(d2%2==0){ //morning
								if(!maxGapsZero){
									if(newSubgroupsDayNHours(sbg,d2)>0){
										_nHours+=max(newSubgroupsDayNHours(sbg,d2), mhm);
										_nUsedMornings++;
									}
								}
								else{
									if(newSubgroupsDayNHours(sbg,d2)>0){
										_nHours+=max(newSubgroupsDayNHours(sbg,d2)+newSubgroupsDayNGaps(sbg,d2), mhm);
										_nUsedMornings++;
									}
								}
							}
							else{ //afternoon
								if(!maxGapsZero){
									if(newSubgroupsDayNHours(sbg,d2)>0){
										int _nh=newSubgroupsDayNHours(sbg,d2);
										if(newSubgroupsDayNFirstGaps(sbg,d2)==1){
											if(_nh<mhaft){
												_nh=mhaft;
											}
											else{
												_nfg++;
											}
										}
										else if(newSubgroupsDayNFirstGaps(sbg,d2)>=2){
											_nh++;
										}

										_nHours+=max(_nh, mhaft);
										_nUsedAfternoons++;
									}
								}
								else{
									if(newSubgroupsDayNHours(sbg,d2)>0){
										int _nh=newSubgroupsDayNHours(sbg,d2)+newSubgroupsDayNGaps(sbg,d2);
										if(newSubgroupsDayNFirstGaps(sbg,d2)==1){
											if(_nh<mhaft){
												_nh=mhaft;
											}
											else{
												_nfg++;
											}
										}
										else if(newSubgroupsDayNFirstGaps(sbg,d2)>=2){
											_nh+=newSubgroupsDayNFirstGaps(sbg,d2)-1;
											if(_nh<mhaft){
												_nh=mhaft;
											}
											else{
												_nfg++;
											}
										}

										_nHours+=max(_nh, mhaft);
										_nUsedAfternoons++;
									}
								}
							}
						}

						if(subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]<_nfg)
							_nHours+=_nfg-subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];

						if(subgroupsMinMorningsPerWeekMinMornings[sbg]>=0)
							if(subgroupsMinMorningsPerWeekMinMornings[sbg]>_nUsedMornings)
								_nHours+=(subgroupsMinMorningsPerWeekMinMornings[sbg]-_nUsedMornings)*mhm;

						if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>=0)
							if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>_nUsedAfternoons)
								_nHours+=(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]-_nUsedAfternoons)*mhaft;

						if(_nHours > nHoursPerSubgroup[sbg]){
							if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
								okstudentsafternoonsearlymaxbeginningsatsecondhour=false;
								goto impossiblestudentsafternoonsearlymaxbeginningsatsecondhour;
							}

							getSbgTimetable(sbg, conflActivities[newtime]);
							sbgGetNHoursGaps(sbg);

							for(;;){
								int nUsedMornings=0;
								int nUsedAfternoons=0;

								int nHours=0;
								int nfg=0;

								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(d2%2==0){ //morning
										if(!maxGapsZero){
											if(sbgDayNHours[d2]>0){
												nHours+=max(sbgDayNHours[d2], mhm);
												nUsedMornings++;
											}
										}
										else{
											if(sbgDayNHours[d2]>0){
												nHours+=max(sbgDayNHours[d2]+sbgDayNGaps[d2], mhm);
												nUsedMornings++;
											}
										}
									}
									else{ //afternoon
										if(!maxGapsZero){
											if(sbgDayNHours[d2]>0){
												int nh=sbgDayNHours[d2];
												if(sbgDayNFirstGaps[d2]==1){
													if(nh<mhaft){
														nh=mhaft;
													}
													else{
														nfg++;
													}
												}
												else if(sbgDayNFirstGaps[d2]>=2){
													nh++;
												}

												nHours+=max(nh, mhaft);
												nUsedAfternoons++;
											}
										}
										else{
											if(sbgDayNHours[d2]>0){
												int nh=sbgDayNHours[d2]+sbgDayNGaps[d2];
												if(sbgDayNFirstGaps[d2]==1){
													if(nh<mhaft){
														nh=mhaft;
													}
													else{
														nfg++;
													}
												}
												else if(sbgDayNFirstGaps[d2]>=2){
													nh+=sbgDayNFirstGaps[d2]-1;
													if(nh<mhaft){
														nh=mhaft;
													}
													else{
														nfg++;
													}
												}

												nHours+=max(nh, mhaft);
												nUsedAfternoons++;
											}
										}
									}
								}

								if(subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]<nfg)
									nHours+=nfg-subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];

								if(subgroupsMinMorningsPerWeekMinMornings[sbg]>=0)
									if(subgroupsMinMorningsPerWeekMinMornings[sbg]>nUsedMornings)
										nHours+=(subgroupsMinMorningsPerWeekMinMornings[sbg]-nUsedMornings)*mhm;

								if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>=0)
									if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>nUsedAfternoons)
										nHours+=(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]-nUsedAfternoons)*mhaft;

								int ai2=-1;

								if(nHours > nHoursPerSubgroup[sbg]){
									//remove an activity
									bool k=subgroupRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									if(!k){
										if(maxGapsZero){
											okstudentsafternoonsearlymaxbeginningsatsecondhour=false;
											goto impossiblestudentsafternoonsearlymaxbeginningsatsecondhour;
										}
										else{
											bool ka=subgroupRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
											assert(conflActivities[newtime].count()==nConflActivities[newtime]);

											if(!ka){
												okstudentsafternoonsearlymaxbeginningsatsecondhour=false;
												goto impossiblestudentsafternoonsearlymaxbeginningsatsecondhour;
											}
										}
									}
								}
								else{ //OK
									break;
								}

								assert(ai2>=0);

								removeAi2FromSbgTimetable(ai2);
								updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
							}
						}
					}
			}
		}

impossiblestudentsafternoonsearlymaxbeginningsatsecondhour:
		if(!okstudentsafternoonsearlymaxbeginningsatsecondhour){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students afternoons early

/////////////////////////////////////////////////////////////////////////////////////////////

		okstudentsmorningsearlymaxbeginningsatsecondhour=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(haveStudentsMorningsEarly){
				for(int sbg : qAsConst(act->iSubgroupsList))
					if(!skipRandom(subgroupsMorningsEarlyMaxBeginningsAtSecondHourPercentage[sbg])){
						//preliminary check
						int mhd=1; //min hours per day
						if(subgroupsMinHoursDailyMinHours[sbg][1]>=0)
							mhd=subgroupsMinHoursDailyMinHours[sbg][1];
						assert(mhd>=1);
						int mhm=mhd; //min hours per morning
						if(subgroupsMinHoursDailyMinHours[sbg][0]>=0)
							mhm=subgroupsMinHoursDailyMinHours[sbg][0];
						assert(mhm>=mhd);

						int mhaft=mhd; //min hours per afternoon
						if(subgroupsMinHoursPerAfternoonMinHours[sbg]>=0)
							mhaft=subgroupsMinHoursPerAfternoonMinHours[sbg];
						assert(mhaft>=mhd);

						bool maxGapsZero=false;
						if(subgroupsMaxGapsPerDayMaxGaps[sbg]==0 ||
						 subgroupsMaxGapsPerWeekMaxGaps[sbg]==0 ||
						 subgroupsMaxGapsPerRealDayMaxGaps[sbg]==0 ||
						 subgroupsMaxGapsPerWeekForRealDaysMaxGaps[sbg]==0)
							maxGapsZero=true;

						int _nUsedMornings=0;
						int _nUsedAfternoons=0;

						int _nHours=0;
						int _nfg=0;

						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							if(d2%2==0){ //morning
								if(!maxGapsZero){
									if(newSubgroupsDayNHours(sbg,d2)>0){
										int _nh=newSubgroupsDayNHours(sbg,d2);
										if(newSubgroupsDayNFirstGaps(sbg,d2)==1){
											if(_nh<mhm){
												_nh=mhm;
											}
											else{
												_nfg++;
											}
										}
										else if(newSubgroupsDayNFirstGaps(sbg,d2)>=2){
											_nh++;
										}

										_nHours+=max(_nh, mhm);
										_nUsedMornings++;
									}
								}
								else{
									if(newSubgroupsDayNHours(sbg,d2)>0){
										int _nh=newSubgroupsDayNHours(sbg,d2)+newSubgroupsDayNGaps(sbg,d2);
										if(newSubgroupsDayNFirstGaps(sbg,d2)==1){
											if(_nh<mhm){
												_nh=mhm;
											}
											else{
												_nfg++;
											}
										}
										else if(newSubgroupsDayNFirstGaps(sbg,d2)>=2){
											_nh+=newSubgroupsDayNFirstGaps(sbg,d2)-1;
											if(_nh<mhm){
												_nh=mhm;
											}
											else{
												_nfg++;
											}
										}

										_nHours+=max(_nh, mhm);
										_nUsedMornings++;
									}
								}
							}
							else{ //afternoon
								if(!maxGapsZero){
									if(newSubgroupsDayNHours(sbg,d2)>0){
										_nHours+=max(newSubgroupsDayNHours(sbg,d2), mhaft);
										_nUsedAfternoons++;
									}
								}
								else{
									if(newSubgroupsDayNHours(sbg,d2)>0){
										_nHours+=max(newSubgroupsDayNHours(sbg,d2)+newSubgroupsDayNGaps(sbg,d2), mhaft);
										_nUsedAfternoons++;
									}
								}
							}
						}

						if(subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]<_nfg)
							_nHours+=_nfg-subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];

						if(subgroupsMinMorningsPerWeekMinMornings[sbg]>=0)
							if(subgroupsMinMorningsPerWeekMinMornings[sbg]>_nUsedMornings)
								_nHours+=(subgroupsMinMorningsPerWeekMinMornings[sbg]-_nUsedMornings)*mhm;

						if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>=0)
							if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>_nUsedAfternoons)
								_nHours+=(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]-_nUsedAfternoons)*mhaft;

						if(_nHours > nHoursPerSubgroup[sbg]){
							if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
								okstudentsmorningsearlymaxbeginningsatsecondhour=false;
								goto impossiblestudentsmorningsearlymaxbeginningsatsecondhour;
							}

							getSbgTimetable(sbg, conflActivities[newtime]);
							sbgGetNHoursGaps(sbg);

							for(;;){
								int nUsedMornings=0;
								int nUsedAfternoons=0;

								int nHours=0;
								int nfg=0;

								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(d2%2==0){ //morning
										if(!maxGapsZero){
											if(sbgDayNHours[d2]>0){
												int nh=sbgDayNHours[d2];
												if(sbgDayNFirstGaps[d2]==1){
													if(nh<mhm){
														nh=mhm;
													}
													else{
														nfg++;
													}
												}
												else if(sbgDayNFirstGaps[d2]>=2){
													nh++;
												}

												nHours+=max(nh, mhm);
												nUsedMornings++;
											}
										}
										else{
											if(sbgDayNHours[d2]>0){
												int nh=sbgDayNHours[d2]+sbgDayNGaps[d2];
												if(sbgDayNFirstGaps[d2]==1){
													if(nh<mhm){
														nh=mhm;
													}
													else{
														nfg++;
													}
												}
												else if(sbgDayNFirstGaps[d2]>=2){
													nh+=sbgDayNFirstGaps[d2]-1;
													if(nh<mhm){
														nh=mhm;
													}
													else{
														nfg++;
													}
												}

												nHours+=max(nh, mhm);
												nUsedMornings++;
											}
										}
									}
									else{ //afternoon
										if(!maxGapsZero){
											if(sbgDayNHours[d2]>0){
												nHours+=max(sbgDayNHours[d2], mhaft);
												nUsedAfternoons++;
											}
										}
										else{
											if(sbgDayNHours[d2]>0){
												nHours+=max(sbgDayNHours[d2]+sbgDayNGaps[d2], mhaft);
												nUsedAfternoons++;
											}
										}
									}
								}

								if(subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]<nfg)
									nHours+=nfg-subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];

								if(subgroupsMinMorningsPerWeekMinMornings[sbg]>=0)
									if(subgroupsMinMorningsPerWeekMinMornings[sbg]>nUsedMornings)
										nHours+=(subgroupsMinMorningsPerWeekMinMornings[sbg]-nUsedMornings)*mhm;

								if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>=0)
									if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>nUsedAfternoons)
										nHours+=(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]-nUsedAfternoons)*mhaft;

								int ai2=-1;

								if(nHours > nHoursPerSubgroup[sbg]){
									//remove an activity
									bool k=subgroupRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									if(!k){
										if(maxGapsZero){
											okstudentsmorningsearlymaxbeginningsatsecondhour=false;
											goto impossiblestudentsmorningsearlymaxbeginningsatsecondhour;
										}
										else{
											bool ka=subgroupRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
											assert(conflActivities[newtime].count()==nConflActivities[newtime]);

											if(!ka){
												okstudentsmorningsearlymaxbeginningsatsecondhour=false;
												goto impossiblestudentsmorningsearlymaxbeginningsatsecondhour;
											}
										}
									}
								}
								else{ //OK
									break;
								}

								assert(ai2>=0);

								removeAi2FromSbgTimetable(ai2);
								updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
							}
						}
					}
			}
		}

impossiblestudentsmorningsearlymaxbeginningsatsecondhour:
		if(!okstudentsmorningsearlymaxbeginningsatsecondhour){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students mornings early

/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking the students max afternoons per week constraints
		////////////////////////////BEGIN max afternoons per week for students
		okstudentsmaxafternoonsperweek=true;
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			//for(int tch : qAsConst(act->iTeachersList)){
			for(int sbg : qAsConst(subgroupsWithMaxAfternoonsPerWeekForActivities[ai])){
				if(skipRandom(subgroupsMaxAfternoonsPerWeekWeightPercentages[sbg]))
					continue;

				int maxAfternoons=subgroupsMaxAfternoonsPerWeekMaxAfternoons[sbg];
				assert(maxAfternoons>=0); //the list contains real information

				//preliminary test
				int _nOc=0;
				for(int d2=1; d2<gt.rules.nDaysPerWeek; d2+=2)
					//if(newTeachersDayNHours(tch,d2)>0)

					//IT IS VITAL TO USE teacherActivitiesOfTheDay as a QList<int> (tch,d2)!!!!!!!
					//The order of evaluation of activities is changed,
					//with activities which were moved forward and back again
					//being put at the end.
					//If you do not follow this, you'll get impossible timetables
					//for the Italian example Italy/2007/difficult/highschool-Ancona.fet or the examples from
					//South Africa: South-Africa/difficult/Collegiate_Junior_School2.fet or
					//South-Africa/difficult/Insight_Learning_Centre2.fet, I am not sure which of these 3

					if(subgroupActivitiesOfTheDay(sbg,d2).count()>0 || d2==d)
						_nOc++;
				if(_nOc<=maxAfternoons)
					continue; //OK, preliminary

				if(maxAfternoons>=0){
					assert(maxAfternoons>0);

					//getTchTimetable(tch, conflActivities[newtime]);
					//tchGetNHoursGaps(tch);

					//bool occupiedDay[MAX_DAYS_PER_WEEK];
					//bool canEmptyDay[MAX_DAYS_PER_WEEK];

					//int _minWrong[MAX_DAYS_PER_WEEK];
					//int _nWrong[MAX_DAYS_PER_WEEK];
					//int _nConflActivities[MAX_DAYS_PER_WEEK];
					//int _minIndexAct[MAX_DAYS_PER_WEEK];

					//QList<int> _activitiesForDay[MAX_DAYS_PER_WEEK];

					for(int d2=1; d2<gt.rules.nDaysPerWeek; d2+=2){
						if(d2==d)
							continue;

						occupiedDay[d2]=false;
						canEmptyDay[d2]=true;

						_minWrong[d2]=INF;
						_nWrong[d2]=0;
						_nConflActivities[d2]=0;
						_minIndexAct[d2]=gt.rules.nInternalActivities;
						_activitiesForDay[d2].clear();

						for(int ai2 : qAsConst(subgroupActivitiesOfTheDay(sbg,d2))){
							if(ai2>=0){
								if(!conflActivities[newtime].contains(ai2)){
									occupiedDay[d2]=true;
									if(fixedTimeActivity[ai2] || swappedActivities[ai2])
										canEmptyDay[d2]=false;
									else if(!_activitiesForDay[d2].contains(ai2)){
										_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
										_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
										_nConflActivities[d2]++;
										_activitiesForDay[d2].append(ai2);
										assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
									}
								}
							}
						}

						if(!occupiedDay[d2])
							canEmptyDay[d2]=false;
					}
					occupiedDay[d]=true;
					canEmptyDay[d]=false;

					int nOc=0;
					bool canChooseDay=false;

					for(int j=1; j<gt.rules.nDaysPerWeek; j+=2)
						if(occupiedDay[j]){
							nOc++;
							if(canEmptyDay[j]){
								canChooseDay=true;
							}
						}

					if(nOc>maxAfternoons){
						assert(nOc==maxAfternoons+1);

						if(!canChooseDay){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okstudentsmaxafternoonsperweek=false;
							goto impossiblestudentsmaxafternoonsperweek;
						}

						int d2=-1;

						if(level!=0){
							//choose random day from those with minimum number of conflicting activities
							QList<int> candidateDays;

							int m=gt.rules.nInternalActivities;

							for(int kk=1; kk<gt.rules.nDaysPerWeek; kk+=2)
								if(canEmptyDay[kk])
									if(m>_nConflActivities[kk])
										m=_nConflActivities[kk];

							candidateDays.clear();
							for(int kk=1; kk<gt.rules.nDaysPerWeek; kk+=2)
								if(canEmptyDay[kk] && m==_nConflActivities[kk])
									candidateDays.append(kk);

							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}
						else{ //level==0
							QList<int> candidateDays;

							int _mW=INF;
							int _nW=INF;
							int _mCA=gt.rules.nInternalActivities;
							int _mIA=gt.rules.nInternalActivities;

							for(int kk=1; kk<gt.rules.nDaysPerWeek; kk+=2)
								if(canEmptyDay[kk]){
									if(_mW>_minWrong[kk] ||
									 (_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
										_mW=_minWrong[kk];
										_nW=_nWrong[kk];
										_mCA=_nConflActivities[kk];
										_mIA=_minIndexAct[kk];
									}
								}

							assert(_mW<INF);

							candidateDays.clear();
							for(int kk=1; kk<gt.rules.nDaysPerWeek; kk+=2)
								if(canEmptyDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);

							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}

						assert(d2>=0);

						assert(_activitiesForDay[d2].count()>0);

						for(int ai2 : qAsConst(_activitiesForDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
						}
					}
				}
			}
		}
impossiblestudentsmaxafternoonsperweek:
		if(!okstudentsmaxafternoonsperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students max afternoons per week

/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking the students max mornings per week constraints
		////////////////////////////BEGIN max mornings per week for students
		okstudentsmaxmorningsperweek=true;
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			//for(int tch : qAsConst(act->iTeachersList)){
			for(int sbg : qAsConst(subgroupsWithMaxMorningsPerWeekForActivities[ai])){
				if(skipRandom(subgroupsMaxMorningsPerWeekWeightPercentages[sbg]))
					continue;

				int maxMornings=subgroupsMaxMorningsPerWeekMaxMornings[sbg];
				assert(maxMornings>=0); //the list contains real information

				//preliminary test
				int _nOc=0;
				for(int d2=0; d2<gt.rules.nDaysPerWeek; d2+=2)
					//if(newTeachersDayNHours(tch,d2)>0)

					//IT IS VITAL TO USE teacherActivitiesOfTheDay as a QList<int> (tch,d2)!!!!!!!
					//The order of evaluation of activities is changed,
					//with activities which were moved forward and back again
					//being put at the end.
					//If you do not follow this, you'll get impossible timetables
					//for the Italian example Italy/2007/difficult/highschool-Ancona.fet or the examples from
					//South Africa: South-Africa/difficult/Collegiate_Junior_School2.fet or
					//South-Africa/difficult/Insight_Learning_Centre2.fet, I am not sure which of these 3

					if(subgroupActivitiesOfTheDay(sbg,d2).count()>0 || d2==d)
						_nOc++;
				if(_nOc<=maxMornings)
					continue; //OK, preliminary

				if(maxMornings>=0){
					assert(maxMornings>0);

					//getTchTimetable(tch, conflActivities[newtime]);
					//tchGetNHoursGaps(tch);

					//bool occupiedDay[MAX_DAYS_PER_WEEK];
					//bool canEmptyDay[MAX_DAYS_PER_WEEK];

					//int _minWrong[MAX_DAYS_PER_WEEK];
					//int _nWrong[MAX_DAYS_PER_WEEK];
					//int _nConflActivities[MAX_DAYS_PER_WEEK];
					//int _minIndexAct[MAX_DAYS_PER_WEEK];

					//QList<int> _activitiesForDay[MAX_DAYS_PER_WEEK];

					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2+=2){
						if(d2==d)
							continue;

						occupiedDay[d2]=false;
						canEmptyDay[d2]=true;

						_minWrong[d2]=INF;
						_nWrong[d2]=0;
						_nConflActivities[d2]=0;
						_minIndexAct[d2]=gt.rules.nInternalActivities;
						_activitiesForDay[d2].clear();

						for(int ai2 : qAsConst(subgroupActivitiesOfTheDay(sbg,d2))){
							if(ai2>=0){
								if(!conflActivities[newtime].contains(ai2)){
									occupiedDay[d2]=true;
									if(fixedTimeActivity[ai2] || swappedActivities[ai2])
										canEmptyDay[d2]=false;
									else if(!_activitiesForDay[d2].contains(ai2)){
										_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
										_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
										_nConflActivities[d2]++;
										_activitiesForDay[d2].append(ai2);
										assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
									}
								}
							}
						}

						if(!occupiedDay[d2])
							canEmptyDay[d2]=false;
					}
					occupiedDay[d]=true;
					canEmptyDay[d]=false;

					int nOc=0;
					bool canChooseDay=false;

					for(int j=0; j<gt.rules.nDaysPerWeek; j+=2)
						if(occupiedDay[j]){
							nOc++;
							if(canEmptyDay[j]){
								canChooseDay=true;
							}
						}

					if(nOc>maxMornings){
						assert(nOc==maxMornings+1);

						if(!canChooseDay){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okstudentsmaxmorningsperweek=false;
							goto impossiblestudentsmaxmorningsperweek;
						}

						int d2=-1;

						if(level!=0){
							//choose random day from those with minimum number of conflicting activities
							QList<int> candidateDays;

							int m=gt.rules.nInternalActivities;

							for(int kk=0; kk<gt.rules.nDaysPerWeek; kk+=2)
								if(canEmptyDay[kk])
									if(m>_nConflActivities[kk])
										m=_nConflActivities[kk];

							candidateDays.clear();
							for(int kk=0; kk<gt.rules.nDaysPerWeek; kk+=2)
								if(canEmptyDay[kk] && m==_nConflActivities[kk])
									candidateDays.append(kk);

							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}
						else{ //level==0
							QList<int> candidateDays;

							int _mW=INF;
							int _nW=INF;
							int _mCA=gt.rules.nInternalActivities;
							int _mIA=gt.rules.nInternalActivities;

							for(int kk=0; kk<gt.rules.nDaysPerWeek; kk+=2)
								if(canEmptyDay[kk]){
									if(_mW>_minWrong[kk] ||
									 (_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
										_mW=_minWrong[kk];
										_nW=_nWrong[kk];
										_mCA=_nConflActivities[kk];
										_mIA=_minIndexAct[kk];
									}
								}

							assert(_mW<INF);

							candidateDays.clear();
							for(int kk=0; kk<gt.rules.nDaysPerWeek; kk+=2)
								if(canEmptyDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);

							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}

						assert(d2>=0);

						assert(_activitiesForDay[d2].count()>0);

						for(int ai2 : qAsConst(_activitiesForDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
						}
					}
				}
			}
		}

impossiblestudentsmaxmorningsperweek:
		if(!okstudentsmaxmorningsperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		////////////////////////////END students max mornings per week

/////////////////////////////////////////////////////////////////////////////////////////////

		//BEGIN students interval max days per week
		okstudentsintervalmaxdaysperweek=true;
		for(int sbg : qAsConst(act->iSubgroupsList)){
			for(int cnt=0; cnt<subgroupsIntervalMaxDaysPerWeekPercentages[sbg].count(); cnt++){
				double perc=subgroupsIntervalMaxDaysPerWeekPercentages[sbg].at(cnt);
				int maxDays=subgroupsIntervalMaxDaysPerWeekMaxDays[sbg].at(cnt);
				int sth=subgroupsIntervalMaxDaysPerWeekIntervalStart[sbg].at(cnt);
				int endh=subgroupsIntervalMaxDaysPerWeekIntervalEnd[sbg].at(cnt);
			
				assert(perc>=0);
				assert(sth>=0 && sth<gt.rules.nHoursPerDay);
				assert(endh>sth && endh<=gt.rules.nHoursPerDay);
				assert(maxDays>=0 && maxDays<=gt.rules.nDaysPerWeek);
				
				if(skipRandom(perc))
					continue;
				
				assert(perc==100.0);
				
				bool foundothers=false;
				bool foundai=false;
				for(int hh=sth; hh<endh; hh++){
					if(newSubgroupsTimetable(sbg,d,hh)==ai){
						foundai=true;
					}
					else{
						assert(newSubgroupsTimetable(sbg,d,hh)==subgroupsTimetable(sbg,d,hh));
						if(newSubgroupsTimetable(sbg,d,hh)>=0){
							if(!conflActivities[newtime].contains(newSubgroupsTimetable(sbg,d,hh))){
								foundothers=true;
							}
						}
					}
				}
				int nrotherdays=0;
				for(int dd=0; dd<gt.rules.nDaysPerWeek; dd++){
					if(dd!=d){
						for(int hh=sth; hh<endh; hh++){
							assert(newSubgroupsTimetable(sbg,dd,hh)==subgroupsTimetable(sbg,dd,hh));
							if(newSubgroupsTimetable(sbg,dd,hh)>=0 && !conflActivities[newtime].contains(newSubgroupsTimetable(sbg,dd,hh))){
								nrotherdays++;
								break;
							}
						}
					}
				}
				assert(nrotherdays<=maxDays); //if percentage==100%, then it is impossible to break this constraint
				if((foundai && !foundothers) && nrotherdays==maxDays){ //increased above limit
					if(level>0){
						//bool occupiedIntervalDay[MAX_DAYS_PER_WEEK];
						//bool canEmptyIntervalDay[MAX_DAYS_PER_WEEK];
				
						///int _minWrong[MAX_DAYS_PER_WEEK];
						///int _nWrong[MAX_DAYS_PER_WEEK];
						//int _nConflActivities[MAX_DAYS_PER_WEEK];
						///int _minIndexAct[MAX_DAYS_PER_WEEK];
				
						//QList<int> _activitiesForIntervalDay[MAX_DAYS_PER_WEEK];

						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							if(d2==d)
								continue;
				
							occupiedIntervalDay[d2]=false;
							canEmptyIntervalDay[d2]=true;
					
							//_minWrong[d2]=INF;
							//_nWrong[d2]=0;
							_nConflActivities[d2]=0;
							//_minIndexAct[d2]=gt.rules.nInternalActivities;
							_activitiesForIntervalDay[d2].clear();
							
							for(int h2=sth; h2<endh; h2++){
								int ai2=subgroupsTimetable(sbg,d2,h2);
								if(ai2>=0){
									if(!conflActivities[newtime].contains(ai2)){
										occupiedIntervalDay[d2]=true;
										if(fixedTimeActivity[ai2] || swappedActivities[ai2])
											canEmptyIntervalDay[d2]=false;
										else if(!_activitiesForIntervalDay[d2].contains(ai2)){
											//_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
											//_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
											//_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
											_nConflActivities[d2]++;
											_activitiesForIntervalDay[d2].append(ai2);
											assert(_nConflActivities[d2]==_activitiesForIntervalDay[d2].count());
										}
									}
								}
							}
					
							if(!occupiedIntervalDay[d2])
								canEmptyIntervalDay[d2]=false;
						}
						occupiedIntervalDay[d]=true;
						canEmptyIntervalDay[d]=false;
				
						int nOc=0;
						bool canChooseDay=false;
				
						for(int j=0; j<gt.rules.nDaysPerWeek; j++)
							if(occupiedIntervalDay[j]){
								nOc++;
								if(canEmptyIntervalDay[j]){
									canChooseDay=true;
								}
							}
						
						assert(nOc==maxDays+1);
					
						if(!canChooseDay){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okstudentsintervalmaxdaysperweek=false;
							goto impossiblestudentsintervalmaxdaysperweek;
						}
					
						int d2=-1;
					
						/////////////////////
						//choose a random day from those with minimum number of conflicting activities
						QList<int> candidateDays;
					
						int m=gt.rules.nInternalActivities;
						
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyIntervalDay[kk])
								if(m>_nConflActivities[kk])
									m=_nConflActivities[kk];
					
						candidateDays.clear();
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyIntervalDay[kk])
								if(m==_nConflActivities[kk])
									candidateDays.append(kk);
						
						assert(candidateDays.count()>0);
						d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						/////////////////////
					
						assert(d2>=0);

						assert(_activitiesForIntervalDay[d2].count()>0);

						for(int ai2 : qAsConst(_activitiesForIntervalDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						}
					}
					else{
						assert(level==0);
						
						//bool occupiedIntervalDay[MAX_DAYS_PER_WEEK];
						//bool canEmptyIntervalDay[MAX_DAYS_PER_WEEK];
				
						//int _minWrong[MAX_DAYS_PER_WEEK];
						//int _nWrong[MAX_DAYS_PER_WEEK];
						//int _nConflActivities[MAX_DAYS_PER_WEEK];
						//int _minIndexAct[MAX_DAYS_PER_WEEK];
				
						//QList<int> _activitiesForIntervalDay[MAX_DAYS_PER_WEEK];

						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							if(d2==d)
								continue;
				
							occupiedIntervalDay[d2]=false;
							canEmptyIntervalDay[d2]=true;
					
							_minWrong[d2]=INF;
							_nWrong[d2]=0;
							_nConflActivities[d2]=0;
							_minIndexAct[d2]=gt.rules.nInternalActivities;
							_activitiesForIntervalDay[d2].clear();
							
							for(int h2=sth; h2<endh; h2++){
								int ai2=subgroupsTimetable(sbg,d2,h2);
								if(ai2>=0){
									if(!conflActivities[newtime].contains(ai2)){
										occupiedIntervalDay[d2]=true;
										if(fixedTimeActivity[ai2] || swappedActivities[ai2])
											canEmptyIntervalDay[d2]=false;
										else if(!_activitiesForIntervalDay[d2].contains(ai2)){
											_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
											_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
											_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
											_nConflActivities[d2]++;
											_activitiesForIntervalDay[d2].append(ai2);
											assert(_nConflActivities[d2]==_activitiesForIntervalDay[d2].count());
										}
									}
								}
							}
					
							if(!occupiedIntervalDay[d2])
								canEmptyIntervalDay[d2]=false;
						}
						occupiedIntervalDay[d]=true;
						canEmptyIntervalDay[d]=false;
				
						int nOc=0;
						bool canChooseDay=false;
				
						for(int j=0; j<gt.rules.nDaysPerWeek; j++)
							if(occupiedIntervalDay[j]){
								nOc++;
								if(canEmptyIntervalDay[j]){
									canChooseDay=true;
								}
							}
						
						assert(nOc==maxDays+1);
					
						if(!canChooseDay){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okstudentsintervalmaxdaysperweek=false;
							goto impossiblestudentsintervalmaxdaysperweek;
						}
					
						int d2=-1;
					
						/////////////////////
						QList<int> candidateDays;

						int _mW=INF;
						int _nW=INF;
						int _mCA=gt.rules.nInternalActivities;
						int _mIA=gt.rules.nInternalActivities;

						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyIntervalDay[kk]){
								if(_mW>_minWrong[kk] ||
								(_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
								(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
								(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
									_mW=_minWrong[kk];
									_nW=_nWrong[kk];
									_mCA=_nConflActivities[kk];
									_mIA=_minIndexAct[kk];
								}
							}
						
						assert(_mW<INF);
						
						candidateDays.clear();
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyIntervalDay[kk])
								if(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);
						
						assert(candidateDays.count()>0);
						d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						////////////////////
					
						assert(d2>=0);

						assert(_activitiesForIntervalDay[d2].count()>0);

						for(int ai2 : qAsConst(_activitiesForIntervalDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						}
					}
				}
			}
		}
		//respecting students interval max days per week
impossiblestudentsintervalmaxdaysperweek:
		if(!okstudentsintervalmaxdaysperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students interval max days per week

/////////////////////////////////////////////////////////////////////////////////////////////

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			//BEGIN students morning interval max days per week
			if(d%2==0){ //d is a morning
				okstudentsmorningintervalmaxdaysperweek=true;
				for(int sbg : qAsConst(act->iSubgroupsList)){
					for(int cnt=0; cnt<subgroupsMorningIntervalMaxDaysPerWeekPercentages[sbg].count(); cnt++){
						double perc=subgroupsMorningIntervalMaxDaysPerWeekPercentages[sbg].at(cnt);
						int maxDays=subgroupsMorningIntervalMaxDaysPerWeekMaxDays[sbg].at(cnt);
						int sth=subgroupsMorningIntervalMaxDaysPerWeekIntervalStart[sbg].at(cnt);
						int endh=subgroupsMorningIntervalMaxDaysPerWeekIntervalEnd[sbg].at(cnt);

						assert(perc>=0);
						assert(sth>=0 && sth<gt.rules.nHoursPerDay);
						assert(endh>sth && endh<=gt.rules.nHoursPerDay);
						assert(maxDays>=0 && maxDays<=gt.rules.nDaysPerWeek/2);

						if(skipRandom(perc))
							continue;

						assert(perc==100.0);

						bool foundothers=false;
						bool foundai=false;
						for(int hh=sth; hh<endh; hh++){
							if(newSubgroupsTimetable(sbg,d,hh)==ai){
								foundai=true;
							}
							else{
								assert(newSubgroupsTimetable(sbg,d,hh)==subgroupsTimetable(sbg,d,hh));
								if(newSubgroupsTimetable(sbg,d,hh)>=0){
									if(!conflActivities[newtime].contains(newSubgroupsTimetable(sbg,d,hh))){
										foundothers=true;
									}
								}
							}
						}
						int nrotherdays=0;
						for(int dd=0; dd<gt.rules.nDaysPerWeek; dd+=2){ //morning
							if(dd!=d){
								for(int hh=sth; hh<endh; hh++){
									assert(newSubgroupsTimetable(sbg,dd,hh)==subgroupsTimetable(sbg,dd,hh));
									if(newSubgroupsTimetable(sbg,dd,hh)>=0 && !conflActivities[newtime].contains(newSubgroupsTimetable(sbg,dd,hh))){
										nrotherdays++;
										break;
									}
								}
							}
						}
						assert(nrotherdays<=maxDays); //if percentage==100%, then it is impossible to break this constraint
						if((foundai && !foundothers) && nrotherdays==maxDays){
							//increased above limit
							//bool occupiedIntervalDay[MAX_DAYS_PER_WEEK];
							//bool canEmptyIntervalDay[MAX_DAYS_PER_WEEK];

							//int _minWrong[MAX_DAYS_PER_WEEK];
							//int _nWrong[MAX_DAYS_PER_WEEK];
							//int _nConflActivities[MAX_DAYS_PER_WEEK];
							//int _minIndexAct[MAX_DAYS_PER_WEEK];

							//QList<int> _activitiesForIntervalDay[MAX_DAYS_PER_WEEK];

							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2+=2){ //morning
								if(d2==d)
									continue;

								occupiedIntervalDay[d2]=false;
								canEmptyIntervalDay[d2]=true;

								_minWrong[d2]=INF;
								_nWrong[d2]=0;
								_nConflActivities[d2]=0;
								_minIndexAct[d2]=gt.rules.nInternalActivities;
								_activitiesForIntervalDay[d2].clear();

								for(int h2=sth; h2<endh; h2++){
									int ai2=subgroupsTimetable(sbg,d2,h2);
								//for(int ai2 : qAsConst(teacherActivitiesOfTheDay(tch,d2))){
									if(ai2>=0){
										if(!conflActivities[newtime].contains(ai2)){
											occupiedIntervalDay[d2]=true;
											if(fixedTimeActivity[ai2] || swappedActivities[ai2])
												canEmptyIntervalDay[d2]=false;
											else if(!_activitiesForIntervalDay[d2].contains(ai2)){
												_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
												_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
												_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
												_nConflActivities[d2]++;
												_activitiesForIntervalDay[d2].append(ai2);
												assert(_nConflActivities[d2]==_activitiesForIntervalDay[d2].count());
											}
										}
									}
								}

								if(!occupiedIntervalDay[d2])
									canEmptyIntervalDay[d2]=false;
							}
							occupiedIntervalDay[d]=true;
							canEmptyIntervalDay[d]=false;

							int nOc=0;
							bool canChooseDay=false;

							for(int j=0; j<gt.rules.nDaysPerWeek; j+=2) //morning
								if(occupiedIntervalDay[j]){
									nOc++;
									if(canEmptyIntervalDay[j]){
										canChooseDay=true;
									}
								}

							//if(nOc>maxDays){
							assert(nOc==maxDays+1);

							if(!canChooseDay){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okstudentsmorningintervalmaxdaysperweek=false;
								goto impossiblestudentsmorningintervalmaxdaysperweek;
							}

							int d2=-1;

							if(level!=0){
								//choose random day from those with minimum number of conflicting activities
								QList<int> candidateDays;

								int m=gt.rules.nInternalActivities;

								for(int kk=0; kk<gt.rules.nDaysPerWeek; kk+=2) //morning
									if(canEmptyIntervalDay[kk])
										if(m>_nConflActivities[kk])
											m=_nConflActivities[kk];

								candidateDays.clear();
								for(int kk=0; kk<gt.rules.nDaysPerWeek; kk+=2) //morning
									if(canEmptyIntervalDay[kk] && m==_nConflActivities[kk])
										candidateDays.append(kk);

								assert(candidateDays.count()>0);
								d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
							}
							else{ //level==0
								QList<int> candidateDays;

								int _mW=INF;
								int _nW=INF;
								int _mCA=gt.rules.nInternalActivities;
								int _mIA=gt.rules.nInternalActivities;

								for(int kk=0; kk<gt.rules.nDaysPerWeek; kk+=2) //morning
									if(canEmptyIntervalDay[kk]){
										if(_mW>_minWrong[kk] ||
										(_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
										(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
										(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
											_mW=_minWrong[kk];
											_nW=_nWrong[kk];
											_mCA=_nConflActivities[kk];
											_mIA=_minIndexAct[kk];
										}
									}

								assert(_mW<INF);

								candidateDays.clear();
								for(int kk=0; kk<gt.rules.nDaysPerWeek; kk+=2) //morning
									if(canEmptyIntervalDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
										candidateDays.append(kk);

								assert(candidateDays.count()>0);
								d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
							}

							assert(d2>=0);

							assert(_activitiesForIntervalDay[d2].count()>0);

							for(int ai2 : qAsConst(_activitiesForIntervalDay[d2])){
								assert(ai2!=ai);
								assert(!swappedActivities[ai2]);
								assert(!fixedTimeActivity[ai2]);
								assert(!conflActivities[newtime].contains(ai2));
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
							}
						}
					}
				}
				//respecting students interval max days per week
impossiblestudentsmorningintervalmaxdaysperweek:
				if(!okstudentsmorningintervalmaxdaysperweek){
					if(updateSubgroups || updateTeachers)
						removeAiFromNewTimetable(ai, act, d, h);
					//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

					nConflActivities[newtime]=MAX_ACTIVITIES;
					continue;
				}
			}
		}

		////////////////////////////END students morning interval max days per week

/////////////////////////////////////////////////////////////////////////////////////////////

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			//BEGIN students afternoon interval max days per week
			if(d%2==1){ //d is an afternoon
				okstudentsafternoonintervalmaxdaysperweek=true;
				for(int sbg : qAsConst(act->iSubgroupsList)){
					for(int cnt=0; cnt<subgroupsAfternoonIntervalMaxDaysPerWeekPercentages[sbg].count(); cnt++){
						double perc=subgroupsAfternoonIntervalMaxDaysPerWeekPercentages[sbg].at(cnt);
						int maxDays=subgroupsAfternoonIntervalMaxDaysPerWeekMaxDays[sbg].at(cnt);
						int sth=subgroupsAfternoonIntervalMaxDaysPerWeekIntervalStart[sbg].at(cnt);
						int endh=subgroupsAfternoonIntervalMaxDaysPerWeekIntervalEnd[sbg].at(cnt);

						assert(perc>=0);
						assert(sth>=0 && sth<gt.rules.nHoursPerDay);
						assert(endh>sth && endh<=gt.rules.nHoursPerDay);
						assert(maxDays>=0 && maxDays<=gt.rules.nDaysPerWeek/2);

						if(skipRandom(perc))
							continue;

						assert(perc==100.0);

						bool foundothers=false;
						bool foundai=false;
						for(int hh=sth; hh<endh; hh++){
							if(newSubgroupsTimetable(sbg,d,hh)==ai){
								foundai=true;
							}
							else{
								assert(newSubgroupsTimetable(sbg,d,hh)==subgroupsTimetable(sbg,d,hh));
								if(newSubgroupsTimetable(sbg,d,hh)>=0){
									if(!conflActivities[newtime].contains(newSubgroupsTimetable(sbg,d,hh))){
										foundothers=true;
									}
								}
							}
						}
						int nrotherdays=0;
						for(int dd=1; dd<gt.rules.nDaysPerWeek; dd+=2){ //afternoon
							if(dd!=d){
								for(int hh=sth; hh<endh; hh++){
									assert(newSubgroupsTimetable(sbg,dd,hh)==subgroupsTimetable(sbg,dd,hh));
									if(newSubgroupsTimetable(sbg,dd,hh)>=0 && !conflActivities[newtime].contains(newSubgroupsTimetable(sbg,dd,hh))){
										nrotherdays++;
										break;
									}
								}
							}
						}
						assert(nrotherdays<=maxDays); //if percentage==100%, then it is impossible to break this constraint
						if((foundai && !foundothers) && nrotherdays==maxDays){
							//increased above limit
							//bool occupiedIntervalDay[MAX_DAYS_PER_WEEK];
							//bool canEmptyIntervalDay[MAX_DAYS_PER_WEEK];

							//int _minWrong[MAX_DAYS_PER_WEEK];
							//int _nWrong[MAX_DAYS_PER_WEEK];
							//int _nConflActivities[MAX_DAYS_PER_WEEK];
							//int _minIndexAct[MAX_DAYS_PER_WEEK];

							//QList<int> _activitiesForIntervalDay[MAX_DAYS_PER_WEEK];

							for(int d2=1; d2<gt.rules.nDaysPerWeek; d2+=2){ //afternoon
								if(d2==d)
									continue;

								occupiedIntervalDay[d2]=false;
								canEmptyIntervalDay[d2]=true;

								_minWrong[d2]=INF;
								_nWrong[d2]=0;
								_nConflActivities[d2]=0;
								_minIndexAct[d2]=gt.rules.nInternalActivities;
								_activitiesForIntervalDay[d2].clear();

								for(int h2=sth; h2<endh; h2++){
									int ai2=subgroupsTimetable(sbg,d2,h2);
								//for(int ai2 : qAsConst(teacherActivitiesOfTheDay(tch,d2))){
									if(ai2>=0){
										if(!conflActivities[newtime].contains(ai2)){
											occupiedIntervalDay[d2]=true;
											if(fixedTimeActivity[ai2] || swappedActivities[ai2])
												canEmptyIntervalDay[d2]=false;
											else if(!_activitiesForIntervalDay[d2].contains(ai2)){
												_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
												_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
												_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
												_nConflActivities[d2]++;
												_activitiesForIntervalDay[d2].append(ai2);
												assert(_nConflActivities[d2]==_activitiesForIntervalDay[d2].count());
											}
										}
									}
								}

								if(!occupiedIntervalDay[d2])
									canEmptyIntervalDay[d2]=false;
							}
							occupiedIntervalDay[d]=true;
							canEmptyIntervalDay[d]=false;

							int nOc=0;
							bool canChooseDay=false;

							for(int j=1; j<gt.rules.nDaysPerWeek; j+=2) //afternoon
								if(occupiedIntervalDay[j]){
									nOc++;
									if(canEmptyIntervalDay[j]){
										canChooseDay=true;
									}
								}

							//if(nOc>maxDays){
							assert(nOc==maxDays+1);

							if(!canChooseDay){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okstudentsafternoonintervalmaxdaysperweek=false;
								goto impossiblestudentsafternoonintervalmaxdaysperweek;
							}

							int d2=-1;

							if(level!=0){
								//choose random day from those with minimum number of conflicting activities
								QList<int> candidateDays;

								int m=gt.rules.nInternalActivities;

								for(int kk=1; kk<gt.rules.nDaysPerWeek; kk+=2) //afternoon
									if(canEmptyIntervalDay[kk])
										if(m>_nConflActivities[kk])
											m=_nConflActivities[kk];

								candidateDays.clear();
								for(int kk=1; kk<gt.rules.nDaysPerWeek; kk+=2) //afternoon
									if(canEmptyIntervalDay[kk] && m==_nConflActivities[kk])
										candidateDays.append(kk);

								assert(candidateDays.count()>0);
								d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
							}
							else{ //level==0
								QList<int> candidateDays;

								int _mW=INF;
								int _nW=INF;
								int _mCA=gt.rules.nInternalActivities;
								int _mIA=gt.rules.nInternalActivities;

								for(int kk=1; kk<gt.rules.nDaysPerWeek; kk+=2) //afternoon
									if(canEmptyIntervalDay[kk]){
										if(_mW>_minWrong[kk] ||
										(_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
										(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
										(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
											_mW=_minWrong[kk];
											_nW=_nWrong[kk];
											_mCA=_nConflActivities[kk];
											_mIA=_minIndexAct[kk];
										}
									}

								assert(_mW<INF);

								candidateDays.clear();
								for(int kk=1; kk<gt.rules.nDaysPerWeek; kk+=2) //afternoon
									if(canEmptyIntervalDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
										candidateDays.append(kk);

								assert(candidateDays.count()>0);
								d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
							}

							assert(d2>=0);

							assert(_activitiesForIntervalDay[d2].count()>0);

							for(int ai2 : qAsConst(_activitiesForIntervalDay[d2])){
								assert(ai2!=ai);
								assert(!swappedActivities[ai2]);
								assert(!fixedTimeActivity[ai2]);
								assert(!conflActivities[newtime].contains(ai2));
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
							}
						}
					}
				}
				//respecting students interval max days per week
impossiblestudentsafternoonintervalmaxdaysperweek:
				if(!okstudentsafternoonintervalmaxdaysperweek){
					if(updateSubgroups || updateTeachers)
						removeAiFromNewTimetable(ai, act, d, h);
					//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

					nConflActivities[newtime]=MAX_ACTIVITIES;
					continue;
				}
			}
		}

		////////////////////////////END students afternoon interval max days per week

/////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////BEGIN students max span per day
		
		//Rodolfo Ribeiro Gomes's code (https://bitbucket.org/rodolforg/fet/src/dev/) was a source of inspiration for the following constraint

		okstudentsmaxspanperday=true;
		for(int sbg : qAsConst(act->iSubgroupsList))
			if(subgroupsMaxSpanPerDayPercentages[sbg]>=0){
				//percentage is 100%
				int maxSpanPerDay=subgroupsMaxSpanPerDayMaxSpan[sbg];
			
				//preliminary test
				int _cnt=0;
				int _start=-1;
				int _end=-1;
				if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
					if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0){
						for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
							if(!breakDayHour(d,h2) && !subgroupNotAvailableDayHour(sbg,d,h2)){
								_start=h2;
								break;
							}
						}
					}
					else{
						int h2;
						for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
							if(newSubgroupsTimetable(sbg, d, h2)>=0){
								_start=h2;
								break;
							}
							else if(!breakDayHour(d,h2) && !subgroupNotAvailableDayHour(sbg,d,h2)){
								break;
							}
						}
						if(_start==-1){
							h2++;
							for(; h2<gt.rules.nHoursPerDay; h2++){
								if(!breakDayHour(d,h2) && !subgroupNotAvailableDayHour(sbg,d,h2)){
									_start=h2;
									break;
								}
							}
						}
					}
				}
				else{
					for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
						if(newSubgroupsTimetable(sbg, d, h2)>=0){
							_start=h2;
							break;
						}
					}
				}
				for(int h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
					if(newSubgroupsTimetable(sbg, d, h2)>=0){
						_end=h2;
						break;
					}
				}
				
				if(_start>=0 && _end>=0 && _end>=_start)
					_cnt=_end-_start+1;
					
				if(_cnt<=maxSpanPerDay)
					continue;
				
				if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
					okstudentsmaxspanperday=false;
					goto impossiblestudentsmaxspanperday;
				}

				getSbgTimetable(sbg, conflActivities[newtime]);
				updateSbgNHoursGaps(sbg, d); //needed for subgroupRemoveAnActivityFromBeginOrEndCertainDay or
				//subgroupRemoveAnActivityFromEndCertainDay or subgroupRemoveAnActivityFromBeginCertainDay below

				for(;;){
					int cnt=0;
					int start=-1;
					int end=-1;

					if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
						if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0){
							for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
								if(!breakDayHour(d,h2) && !subgroupNotAvailableDayHour(sbg,d,h2)){
									start=h2;
									break;
								}
							}
						}
						else{
							int h2;
							for(h2=0; h2<gt.rules.nHoursPerDay; h2++){
								if(sbgTimetable(d, h2)>=0){
									start=h2;
									break;
								}
								else if(!breakDayHour(d,h2) && !subgroupNotAvailableDayHour(sbg,d,h2)){
									break;
								}
							}
							if(start==-1){
								h2++;
								for(; h2<gt.rules.nHoursPerDay; h2++){
									if(!breakDayHour(d,h2) && !subgroupNotAvailableDayHour(sbg,d,h2)){
										start=h2;
										break;
									}
								}
							}
						}
					}
					else{
						for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
							if(sbgTimetable(d, h2)>=0){
								start=h2;
								break;
							}
						}
					}

					for(int h2=gt.rules.nHoursPerDay-1; h2>=0; h2--){
						if(sbgTimetable(d, h2)>=0){
							end=h2;
							break;
						}
					}
					
					if(start>=0 && end>=0 && end>=start)
						cnt=end-start+1;
						
					if(cnt<=maxSpanPerDay)
						break;
				
					int ai2=-1;
					
					if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
						if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0){
							bool k=subgroupRemoveAnActivityFromEndCertainDay(d, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								if(level==0){
									//old comment below
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okstudentsmaxspanperday=false;
								goto impossiblestudentsmaxspanperday;
							}
						}
						else{
							bool k=subgroupRemoveAnActivityFromEndCertainDay(d, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								bool k2=false;

								//2022-05-21 - Old comment below - bug found with the new locking of activies into days.
								//The following code is theoretically better, but practically much worse on the file
								//examples/Romania/Pedagogic-High-School-Tg-Mures/2007-2008_sem1-d-test-students-max-span-per-day.fet
								//(it slows down with 25%-50% on average - you need to generate more timetables)
								int firstAvailableHour=-1;
								for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
									if(!breakDayHour(d,h2) && !subgroupNotAvailableDayHour(sbg,d,h2)){
										firstAvailableHour=h2;
										break;
									}
								}
								//We could assert(firstAvailableHour>=0), because the day is not empty, because the span is too large.
								if(firstAvailableHour>=0){
									assert(firstAvailableHour<gt.rules.nHoursPerDay);
									if(sbgTimetable(d, firstAvailableHour)>=0){
										k2=subgroupRemoveAnActivityFromBeginCertainDay(d, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
										assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									}
								}
								//and indeed, I will assert this:
								else{
									assert(0);
								}
								
								if(!k2){
									if(level==0){
										//old comment below
										//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okstudentsmaxspanperday=false;
									goto impossiblestudentsmaxspanperday;
								}
							}
						}
					}
					else{
						bool k=subgroupRemoveAnActivityFromBeginOrEndCertainDay(d, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!k){
							if(level==0){
								//old comment below
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okstudentsmaxspanperday=false;
							goto impossiblestudentsmaxspanperday;
						}
					}
					
					assert(ai2>=0);

					removeAi2FromSbgTimetable(ai2);
					sbgDayNHours[d]-=gt.rules.internalActivitiesList[ai2].duration; //needed for subgroupRemoveAnActivityFromBeginOrEndCertainDay or
					//subgroupRemoveAnActivityFromEndCertainDay or subgroupRemoveAnActivityFromBeginCertainDay above
					assert(sbgDayNHours[d]>=0);
				}
			}
		
impossiblestudentsmaxspanperday:
		if(!okstudentsmaxspanperday){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students max span per day

/////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////BEGIN students max span per real day

		//Rodolfo Ribeiro Gomes's code (https://bitbucket.org/rodolforg/fet/src/dev/) was a source of inspiration for the following constraint

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			okstudentsmaxspanperrealday=true;
			for(int sbg : qAsConst(act->iSubgroupsList))
				if(subgroupsMaxSpanPerRealDayPercentages[sbg]>=0){
					//percentage is 100%
					int maxSpanPerDay=subgroupsMaxSpanPerRealDayMaxSpan[sbg];

					//preliminary test
					int _cnt=0;
					int _start=-1;
					int _end=-1;
					if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
						if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0){
							for(int h2=0; h2<2*gt.rules.nHoursPerDay; h2++){
								int d3=(d/2)*2+(h2<gt.rules.nHoursPerDay?0:1);
								int h3=h2%gt.rules.nHoursPerDay;
								if(!breakDayHour(d3,h3) && !subgroupNotAvailableDayHour(sbg,d3,h3)){
									_start=h2;
									break;
								}
							}
						}
						else{
							int h2;
							for(h2=0; h2<2*gt.rules.nHoursPerDay; h2++){
								int d3=(d/2)*2+(h2<gt.rules.nHoursPerDay?0:1);
								int h3=h2%gt.rules.nHoursPerDay;
								if(newSubgroupsTimetable(sbg, d3, h3)>=0){
									_start=h2;
									break;
								}
								else if(!breakDayHour(d3,h3) && !subgroupNotAvailableDayHour(sbg,d3,h3)){
									break;
								}
							}
							if(_start==-1){
								h2++;
								for(; h2<2*gt.rules.nHoursPerDay; h2++){
									int d3=(d/2)*2+(h2<gt.rules.nHoursPerDay?0:1);
									int h3=h2%gt.rules.nHoursPerDay;
									if(!breakDayHour(d3,h3) && !subgroupNotAvailableDayHour(sbg,d3,h3)){
										_start=h2;
										break;
									}
								}
							}
						}
					}
					else{
						for(int h2=0; h2<2*gt.rules.nHoursPerDay; h2++){
							int d3=(d/2)*2+(h2<gt.rules.nHoursPerDay?0:1);
							int h3=h2%gt.rules.nHoursPerDay;
							if(newSubgroupsTimetable(sbg, d3, h3)>=0){
								_start=h2;
								break;
							}
						}
					}
					for(int h2=2*gt.rules.nHoursPerDay-1; h2>=0; h2--){
						int d3=(d/2)*2+(h2<gt.rules.nHoursPerDay?0:1);
						int h3=h2%gt.rules.nHoursPerDay;
						if(newSubgroupsTimetable(sbg, d3, h3)>=0){
							_end=h2;
							break;
						}
					}

					if(_start>=0 && _end>=0 && _end>=_start)
						_cnt=_end-_start+1;

					if(_cnt<=maxSpanPerDay)
						continue;

					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okstudentsmaxspanperrealday=false;
						goto impossiblestudentsmaxspanperrealday;
					}

					getSbgTimetable(sbg, conflActivities[newtime]);
					updateSbgNHoursGaps(sbg, d); //needed for subgroupRemoveAnActivity... below
					updateSbgNHoursGaps(sbg, dpair); //needed for subgroupRemoveAnActivity... below

					for(;;){
						int cnt=0;
						int start=-1;
						int end=-1;

						if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
							if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0){
								for(int h2=0; h2<2*gt.rules.nHoursPerDay; h2++){
									int d3=(d/2)*2+(h2<gt.rules.nHoursPerDay?0:1);
									int h3=h2%gt.rules.nHoursPerDay;
									if(!breakDayHour(d3,h3) && !subgroupNotAvailableDayHour(sbg,d3,h3)){
										start=h2;
										break;
									}
								}
							}
							else{
								int h2;
								for(h2=0; h2<2*gt.rules.nHoursPerDay; h2++){
									int d3=(d/2)*2+(h2<gt.rules.nHoursPerDay?0:1);
									int h3=h2%gt.rules.nHoursPerDay;
									if(sbgTimetable(d3, h3)>=0){
										start=h2;
										break;
									}
									else if(!breakDayHour(d3,h3) && !subgroupNotAvailableDayHour(sbg,d3,h3)){
										break;
									}
								}
								if(start==-1){
									h2++;
									for(; h2<2*gt.rules.nHoursPerDay; h2++){
										int d3=(d/2)*2+(h2<gt.rules.nHoursPerDay?0:1);
										int h3=h2%gt.rules.nHoursPerDay;
										if(!breakDayHour(d3,h3) && !subgroupNotAvailableDayHour(sbg,d3,h3)){
											start=h2;
											break;
										}
									}
								}
							}
						}
						else{
							for(int h2=0; h2<2*gt.rules.nHoursPerDay; h2++){
								int d3=(d/2)*2+(h2<gt.rules.nHoursPerDay?0:1);
								int h3=h2%gt.rules.nHoursPerDay;
								if(sbgTimetable(d3, h3)>=0){
									start=h2;
									break;
								}
							}
						}

						for(int h2=2*gt.rules.nHoursPerDay-1; h2>=0; h2--){
							int d3=(d/2)*2+(h2<gt.rules.nHoursPerDay?0:1);
							int h3=h2%gt.rules.nHoursPerDay;
							if(sbgTimetable(d3, h3)>=0){
								end=h2;
								break;
							}
						}

						if(start>=0 && end>=0 && end>=start)
							cnt=end-start+1;

						if(cnt<=maxSpanPerDay)
							break;

						int ai2=-1;

						/*
						//In generate_pre.cpp there are made optimizations to make early = best(mornings early, afternoons early) etc. See the code in compute mornings early.
						if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
							if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0){
								bool k=subgroupRemoveAnActivityFromEndCertainRealDay(d/2, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k){
									if(level==0){
										//old comment below
										//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okstudentsmaxspanperrealday=false;
									goto impossiblestudentsmaxspanperrealday;
								}
							}
							else{
								bool k=subgroupRemoveAnActivityFromEndCertainRealDay(d/2, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k){
									bool k2=false;

									//2022-05-21 - Old comment below - bug found with the new locking of activies into days.
									//This situation is similar with students max span per day.
									//The following code is theoretically better, but practically much worse on the file
									//examples/Romania/Pedagogic-High-School-Tg-Mures/2007-2008_sem1-d-test-students-max-span-per-day.fet
									//(it slows down with 25%-50% on average - you need to generate more timetables)
#if 0
									int dd=d;
									if(dd%2==1) //if it is afternoon
										dd--; //make it the paired morning
									int firstAvailableHour=-1;
									int firstAvailableDay=-1;
									for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
										if(!breakDayHour(dd,h2) && !subgroupNotAvailableDayHour(sbg,dd,h2)){
											firstAvailableHour=h2;
											firstAvailableDay=dd;
											break;
										}
									}
									if(firstAvailableHour==-1){
										for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
											if(!breakDayHour(dd+1,h2) && !subgroupNotAvailableDayHour(sbg,dd+1,h2)){
												firstAvailableHour=h2;
												firstAvailableDay=dd+1;
												break;
											}
										}
									}
									//We could assert(firstAvailableHour>=0), because the real day is not empty, because the span is too large.
									//and indeed, I will assert this:
									assert(firstAvailableHour>=0);
									assert(firstAvailableDay>=0);
									if(firstAvailableHour>=0){
										assert(firstAvailableHour<gt.rules.nHoursPerDay);
										assert(firstAvailableDay<gt.rules.nDaysPerWeek);
										if(sbgTimetable(firstAvailableDay, firstAvailableHour)>=0){
											k2=subgroupRemoveAnActivityFromBeginCertainDay(firstAvailableDay, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
											assert(conflActivities[newtime].count()==nConflActivities[newtime]);
										}
									}
#endif
									k2=subgroupRemoveAnActivityFromBeginCertainRealDay(d/2, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									if(!k2){
										if(level==0){
											//old comment below
											//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
											//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
										}
										okstudentsmaxspanperrealday=false;
										goto impossiblestudentsmaxspanperrealday;
									}
								}
							}
						}
						else{
							bool k=subgroupRemoveAnActivityFromBeginOrEndCertainRealDay(d/2, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								if(level==0){
									//old comment below
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okstudentsmaxspanperrealday=false;
								goto impossiblestudentsmaxspanperrealday;
							}
						}*/

						//The following commented procedure is very simple and works, but it is not perfect.
						//Consider that, even if we have early max beginnings for mornings or half days, we may empty the morning.
						/*bool k=subgroupRemoveAnActivityFromBeginOrEndCertainRealDay(d/2, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!k){
							if(level==0){
								//old comment below
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okstudentsmaxspanperrealday=false;
							goto impossiblestudentsmaxspanperrealday;
						}*/

						//2022-05-23 begin complicated stuff...
						if(d%2==1){ //afternoon
							//Consider that, even if we have early max beginnings for mornings or half days, we may empty the morning.
							if(sbgDayNHours[d-1]==0){
								if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0 || subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0){
									bool k=subgroupRemoveAnActivityFromEndCertainDay(d, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									if(!k){
										if(level==0){
											//old comment below
											//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
											//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
										}
										okstudentsmaxspanperrealday=false;
										goto impossiblestudentsmaxspanperrealday;
									}
								}
								else if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]>0 || subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]>0){
									bool k=subgroupRemoveAnActivityFromEndCertainDay(d, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									if(!k){
										bool k2=false;

										int firstAvailableHour=-1;
										for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
											if(!breakDayHour(d,h2) && !subgroupNotAvailableDayHour(sbg,d,h2)){
												firstAvailableHour=h2;
												break;
											}
										}
										//We could assert(firstAvailableHour>=0), because the real/half day is not empty, because the span is too large.
										//and indeed, I will assert this:
										assert(firstAvailableHour>=0);
										if(firstAvailableHour>=0){
											assert(firstAvailableHour<gt.rules.nHoursPerDay);
											if(sbgTimetable(d, firstAvailableHour)>=0){
												k2=subgroupRemoveAnActivityFromBeginCertainDay(d, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
												assert(conflActivities[newtime].count()==nConflActivities[newtime]);
											}
										}
										if(!k2){
											if(level==0){
												//old comment below
												//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
												//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
											}
											okstudentsmaxspanperrealday=false;
											goto impossiblestudentsmaxspanperrealday;
										}
									}
								}
								else{
									assert(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==-1 && subgroupsAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==-1);
									bool k=subgroupRemoveAnActivityFromBeginOrEndCertainDay(d, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									if(!k){
										if(level==0){
											//old comment below
											//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
											//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
										}
										okstudentsmaxspanperrealday=false;
										goto impossiblestudentsmaxspanperrealday;
									}
								}
							}
							else{ //if(sbgDayNHours[d-1]>0)
								bool k=subgroupRemoveAnActivityFromBeginOrEndCertainRealDay(d/2, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k){
									if(level==0){
										//old comment below
										//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okstudentsmaxspanperrealday=false;
									goto impossiblestudentsmaxspanperrealday;
								}
							}
						}
						else{ //morning
							assert(sbgDayNHours[d]>0);
							if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0 || subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0){
								bool k=subgroupRemoveAnActivityFromEndCertainRealDay(d/2, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k){
									if(level==0){
										//old comment below
										//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okstudentsmaxspanperrealday=false;
									goto impossiblestudentsmaxspanperrealday;
								}
							}
							else if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]>0 || subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]>0){
								bool k=subgroupRemoveAnActivityFromEndCertainRealDay(d/2, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k){
									bool k2=false;

									int firstAvailableHour=-1;
									for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
										if(!breakDayHour(d,h2) && !subgroupNotAvailableDayHour(sbg,d,h2)){
											firstAvailableHour=h2;
											break;
										}
									}
									//We could assert(firstAvailableHour>=0), because the real/half day is not empty, because the span is too large.
									//and indeed, I will assert this:
									assert(firstAvailableHour>=0);
									if(firstAvailableHour>=0){
										assert(firstAvailableHour<gt.rules.nHoursPerDay);
									if(sbgTimetable(d, firstAvailableHour)>=0){
											k2=subgroupRemoveAnActivityFromBeginCertainDay(d, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
											assert(conflActivities[newtime].count()==nConflActivities[newtime]);
										}
									}
									if(!k2){
										if(level==0){
											//old comment below
											//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
											//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
										}
										okstudentsmaxspanperrealday=false;
										goto impossiblestudentsmaxspanperrealday;
									}
								}
							}
							else{
								assert(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==-1 && subgroupsMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==-1);
								bool k=subgroupRemoveAnActivityFromBeginOrEndCertainRealDay(d/2, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k){
									if(level==0){
										//old comment below
										//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okstudentsmaxspanperrealday=false;
									goto impossiblestudentsmaxspanperrealday;
								}
							}
						}
						//2022-05-23 end complicated stuff...

						assert(ai2>=0);

						int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
						removeAi2FromSbgTimetable(ai2);
						sbgDayNHours[d2]-=gt.rules.internalActivitiesList[ai2].duration; //needed for subgroupRemoveAnActivityFrom... above
						assert(sbgDayNHours[d2]>=0);
					}
				}

impossiblestudentsmaxspanperrealday:
			if(!okstudentsmaxspanperrealday){
				if(updateSubgroups || updateTeachers)
					removeAiFromNewTimetable(ai, act, d, h);
				//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

				nConflActivities[newtime]=MAX_ACTIVITIES;
				continue;
			}
		}

		////////////////////////////END students max span per real day

/////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////BEGIN students min resting hours

		//Rodolfo Ribeiro Gomes's code (https://bitbucket.org/rodolforg/fet/src/dev/) was a source of inspiration for the following constraint

		okstudentsminrestinghours=true;

		for(int sbg : qAsConst(act->iSubgroupsList)){
			for(int qq=0; qq<2; qq++){
				double percentage;
				int minRestingHours;
				bool circular;
				if(qq==0){
					percentage=subgroupsMinRestingHoursCircularPercentages[sbg];
					minRestingHours=subgroupsMinRestingHoursCircularMinHours[sbg];
					circular=true;
				}
				else{
					assert(qq==1);
					percentage=subgroupsMinRestingHoursNotCircularPercentages[sbg];
					minRestingHours=subgroupsMinRestingHoursNotCircularMinHours[sbg];
					circular=false;
				}
				if(percentage>=0){
					//percentage is 100%
					assert(minRestingHours<=gt.rules.nHoursPerDay);
					
					//phase 1 - activity is at the end of the day
					int _cnt1=0;
					int _cnt1d=0;
					if(d <= gt.rules.nDaysPerWeek-2+(circular?1:0) && h+act->duration-1 >= gt.rules.nHoursPerDay-minRestingHours){
						for(int h2=gt.rules.nHoursPerDay-1; h2>h+act->duration-1; h2--){
							int ai2=newSubgroupsTimetable(sbg, d, h2);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2))
								break;
							else
								_cnt1++;
						}
						_cnt1d=_cnt1;
						for(int h2=0; h2<minRestingHours-_cnt1d; h2++){
							int ai2=newSubgroupsTimetable(sbg, (d+1<=gt.rules.nDaysPerWeek-1?d+1:0), h2);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2))
								break;
							else
								_cnt1++;
						}
					}
					else
						_cnt1=minRestingHours;
					
					//phase 2 - activity is at the beginning of the day
					int _cnt2=0;
					int _cnt2d=0;
					if(d>=1-(circular?1:0) && h<=minRestingHours-1){
						for(int h2=0; h2<h; h2++){
							int ai2=newSubgroupsTimetable(sbg, d, h2);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2))
								break;
							else
								_cnt2++;
						}
						_cnt2d=_cnt2;
						for(int h2=gt.rules.nHoursPerDay-1; h2>=gt.rules.nHoursPerDay-minRestingHours+_cnt2d; h2--){
							int ai2=newSubgroupsTimetable(sbg, (d-1>=0?d-1:gt.rules.nDaysPerWeek-1), h2);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2))
								break;
							else
								_cnt2++;
						}
					}
					else
						_cnt2=minRestingHours;
						
					if(_cnt1<minRestingHours){
						QList<int> removableActs;
						/*for(int h2=gt.rules.nHoursPerDay-minRestingHours; h2<gt.rules.nHoursPerDay; h2++){
							int ai2=newSubgroupsTimetable(sbg, d, h2);
							if(ai2>=0 && ai2!=ai && !removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2] && !conflActivities[newtime].contains(ai2))
								removableActs.append(ai2);
						}*/
						for(int h2=0; h2<minRestingHours-_cnt1d; h2++){
							int ai2=newSubgroupsTimetable(sbg, (d+1<=gt.rules.nDaysPerWeek-1?d+1:0), h2);
							if(ai2>=0 && !removableActs.contains(ai2) && !conflActivities[newtime].contains(ai2)){
								if(!fixedTimeActivity[ai2] && !swappedActivities[ai2])
									removableActs.append(ai2);
								else
									break;
							}
						}
						
						for(;;){
							if(removableActs.count()==0){
								okstudentsminrestinghours=false;
								goto impossiblestudentsminrestinghours;
							}
							
							int ai2=removableActs.at(0);
							
							int t=removableActs.removeAll(ai2);
							assert(t==1);
							
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);

							int cnt1=0;
							if(d <= gt.rules.nDaysPerWeek-2+(circular?1:0) && h+act->duration-1 >= gt.rules.nHoursPerDay-minRestingHours){
								/*for(int h2=gt.rules.nHoursPerDay-1; h2>h+act->duration-1; h2--){
									int ai2=newSubgroupsTimetable(sbg, d, h2);
									if(ai2>=0 && !conflActivities[newtime].contains(ai2))
										break;
									else
										cnt1++;
								}*/
								cnt1+=_cnt1d;
								for(int h2=0; h2<minRestingHours-_cnt1d; h2++){
									int ai2=newSubgroupsTimetable(sbg, (d+1<=gt.rules.nDaysPerWeek-1?d+1:0), h2);
									if(ai2>=0 && !conflActivities[newtime].contains(ai2))
										break;
									else
										cnt1++;
								}
							}
							else{
								assert(0);
							}
							
							assert(cnt1>_cnt1);
							_cnt1=cnt1;
							
							if(cnt1>=minRestingHours)
								break;
						}
					}
					if(_cnt2<minRestingHours){
						QList<int> removableActs;
						for(int h2=gt.rules.nHoursPerDay-1; h2>=gt.rules.nHoursPerDay-minRestingHours+_cnt2d; h2--){
							int ai2=newSubgroupsTimetable(sbg, (d-1>=0?d-1:gt.rules.nDaysPerWeek-1), h2);
							if(ai2>=0 && !removableActs.contains(ai2) && !conflActivities[newtime].contains(ai2)){
								if(!fixedTimeActivity[ai2] && !swappedActivities[ai2])
									removableActs.append(ai2);
								else
									break;
							}
						}
						/*for(int h2=0; h2<minRestingHours; h2++){
							int ai2=newSubgroupsTimetable(sbg, d, h2);
							if(ai2>=0 && ai2!=ai && !removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2] && !conflActivities[newtime].contains(ai2))
								removableActs.append(ai2);
						}*/
						
						for(;;){
							if(removableActs.count()==0){
								okstudentsminrestinghours=false;
								goto impossiblestudentsminrestinghours;
							}
							
							int ai2=removableActs.at(0);
							
							int t=removableActs.removeAll(ai2);
							assert(t==1);
							
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);

							int cnt2=0;
							if(d>=1-(circular?1:0) && h<minRestingHours){
								/*for(int h2=0; h2<h; h2++){
									int ai2=newSubgroupsTimetable(sbg, d, h2);
									if(ai2>=0 && !conflActivities[newtime].contains(ai2))
										break;
									else
										cnt2++;
								}*/
								cnt2+=_cnt2d;
								for(int h2=gt.rules.nHoursPerDay-1; h2>=gt.rules.nHoursPerDay-minRestingHours+_cnt2d; h2--){
									int ai2=newSubgroupsTimetable(sbg, (d-1>=0?d-1:gt.rules.nDaysPerWeek-1), h2);
									if(ai2>=0 && !conflActivities[newtime].contains(ai2))
										break;
									else
										cnt2++;
								}
							}
							else{
								assert(0);
							}

							assert(cnt2>_cnt2);
							_cnt2=cnt2;

							if(cnt2>=minRestingHours)
								break;
						}
					}
				}
			}
		}

impossiblestudentsminrestinghours:
		if(!okstudentsminrestinghours){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students min resting hours

/////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////BEGIN students min resting hours between morning and afternoon

		//Rodolfo Ribeiro Gomes's code (https://bitbucket.org/rodolforg/fet/src/dev/) was a source of inspiration for the following constraint

		okstudentsminrestinghoursbetweenmorningandafternoon=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int sbg : qAsConst(act->iSubgroupsList)){
				double percentage=subgroupsMinRestingHoursBetweenMorningAndAfternoonPercentages[sbg];
				int minRestingHours=subgroupsMinRestingHoursBetweenMorningAndAfternoonMinHours[sbg];

				if(percentage>=0){
					assert(percentage==100.0);
					assert(minRestingHours<=2*gt.rules.nHoursPerDay);

					if(d%2==0){ //morning
						int _cnt1=0;
						int _cnt1d=0;
						if(h+act->duration-1 >= gt.rules.nHoursPerDay-minRestingHours){
							for(int h2=gt.rules.nHoursPerDay-1; h2>h+act->duration-1; h2--){
								int ai2=newSubgroupsTimetable(sbg, d, h2);
								if(ai2>=0 && !conflActivities[newtime].contains(ai2))
									break;
								else
									_cnt1++;
							}
							_cnt1d=_cnt1;
							for(int h2=0; h2<minRestingHours-_cnt1d && h2<gt.rules.nHoursPerDay; h2++){
								int ai2=newSubgroupsTimetable(sbg, d+1, h2);
								if(ai2>=0 && !conflActivities[newtime].contains(ai2))
									break;
								else
									_cnt1++;
							}
						}
						else
							_cnt1=minRestingHours;

						if(_cnt1<minRestingHours){
							QList<int> removableActs;
							/*for(int h2=gt.rules.nHoursPerDay-minRestingHours; h2<gt.rules.nHoursPerDay; h2++){
								int ai2=newTeachersTimetable(tch, d, h2);
								if(ai2>=0 && ai2!=ai && !removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2] && !conflActivities[newtime].contains(ai2))
									removableActs.append(ai2);
							}*/
							for(int h2=0; h2<minRestingHours-_cnt1d && h2<gt.rules.nHoursPerDay; h2++){
								int ai2=newSubgroupsTimetable(sbg, d+1, h2);
								if(ai2>=0 && !removableActs.contains(ai2) && !conflActivities[newtime].contains(ai2)){
									if(!fixedTimeActivity[ai2] && !swappedActivities[ai2])
										removableActs.append(ai2);
									else
										break;
								}
							}

							for(;;){
								if(removableActs.count()==0){
									okstudentsminrestinghoursbetweenmorningandafternoon=false;
									goto impossiblestudentsminrestinghoursbetweenmorningandafternoon;
								}

								int ai2=removableActs.at(0);

								int t=removableActs.removeAll(ai2);
								assert(t==1);

								assert(!conflActivities[newtime].contains(ai2));
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								int cnt1=0;
								if(h+act->duration-1 >= gt.rules.nHoursPerDay-minRestingHours){
									/*for(int h2=gt.rules.nHoursPerDay-1; h2>h+act->duration-1; h2--){
										int ai2=newTeachersTimetable(tch, d, h2);
										if(ai2>=0 && !conflActivities[newtime].contains(ai2))
											break;
										else
											cnt1++;
									}*/
									cnt1+=_cnt1d;
									for(int h2=0; h2<minRestingHours-_cnt1d && h2<gt.rules.nHoursPerDay; h2++){
										int ai2=newSubgroupsTimetable(sbg, d+1, h2);
										if(ai2>=0 && !conflActivities[newtime].contains(ai2))
											break;
										else
											cnt1++;
									}
								}
								else{
									assert(0);
								}

								assert(cnt1>_cnt1);
								_cnt1=cnt1;

								if(cnt1>=minRestingHours)
									break;
							}
						}
					}
					else{ //afternoon
						assert(d%2==1);
						int _cnt2=0;
						int _cnt2d=0;
						if(h<=minRestingHours-1){
							for(int h2=0; h2<h; h2++){
								int ai2=newSubgroupsTimetable(sbg, d, h2);
								if(ai2>=0 && !conflActivities[newtime].contains(ai2))
									break;
								else
									_cnt2++;
							}
							_cnt2d=_cnt2;
							for(int h2=gt.rules.nHoursPerDay-1; h2>=gt.rules.nHoursPerDay-minRestingHours+_cnt2d && h2>=0; h2--){
								int ai2=newSubgroupsTimetable(sbg, d-1, h2);
								if(ai2>=0 && !conflActivities[newtime].contains(ai2))
									break;
								else
									_cnt2++;
							}
						}
						else
							_cnt2=minRestingHours;

						if(_cnt2<minRestingHours){
							QList<int> removableActs;
							for(int h2=gt.rules.nHoursPerDay-1; h2>=gt.rules.nHoursPerDay-minRestingHours+_cnt2d && h2>=0; h2--){
								int ai2=newSubgroupsTimetable(sbg, d-1, h2);
								if(ai2>=0 && !removableActs.contains(ai2) && !conflActivities[newtime].contains(ai2)){
									if(!fixedTimeActivity[ai2] && !swappedActivities[ai2])
										removableActs.append(ai2);
									else
										break;
								}
							}
							/*for(int h2=0; h2<minRestingHours; h2++){
								int ai2=newTeachersTimetable(tch, d, h2);
								if(ai2>=0 && ai2!=ai && !removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2] && !conflActivities[newtime].contains(ai2))
									removableActs.append(ai2);
							}*/

							for(;;){
								if(removableActs.count()==0){
									okstudentsminrestinghoursbetweenmorningandafternoon=false;
									goto impossiblestudentsminrestinghoursbetweenmorningandafternoon;
								}

								int ai2=removableActs.at(0);

								int t=removableActs.removeAll(ai2);
								assert(t==1);

								assert(!conflActivities[newtime].contains(ai2));
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								int cnt2=0;
								if(h<minRestingHours){
									/*for(int h2=0; h2<h; h2++){
										int ai2=newTeachersTimetable(tch, d, h2);
										if(ai2>=0 && !conflActivities[newtime].contains(ai2))
											break;
										else
											cnt2++;
									}*/
									cnt2+=_cnt2d;
									for(int h2=gt.rules.nHoursPerDay-1; h2>=gt.rules.nHoursPerDay-minRestingHours+_cnt2d && h2>=0; h2--){
										int ai2=newSubgroupsTimetable(sbg, d-1, h2);
										if(ai2>=0 && !conflActivities[newtime].contains(ai2))
											break;
										else
											cnt2++;
									}
								}
								else{
									assert(0);
								}

								assert(cnt2>_cnt2);
								_cnt2=cnt2;

								if(cnt2>=minRestingHours)
									break;
							}
						}
					}
				}
			}
		}

impossiblestudentsminrestinghoursbetweenmorningandafternoon:
		if(!okstudentsminrestinghoursbetweenmorningandafternoon){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students min resting hours between morning and afternoon

/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking students early max beginnings at second hour
		//TODO: this should take care of students max gaps per day also. Very critical changes, so be very careful if you do them. Safer -> leave them as they are now.
		//see file fet-v.v.v/doc/algorithm/improve-studentsmaxgapsperday.txt for advice and (unstable) code on how to make students max gaps per day constraint perfect
		okstudentsearlymaxbeginningsatsecondhour=true;
		
		for(int sbg : qAsConst(act->iSubgroupsList))
			if(!skipRandom(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg])){
				//preliminary check
				int _nHours=0;
				int _nFirstGapsOne=0;
				int _nFirstGapsTwo=0;
				int _nGaps=0;
				int _nIllegalGaps=0;
				for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
					_nHours+=newSubgroupsDayNHours(sbg,d2);
					
					if(newSubgroupsDayNFirstGaps(sbg, d2)==1){
						_nFirstGapsOne++;
					}
					else if(newSubgroupsDayNFirstGaps(sbg, d2)>=2){
						_nFirstGapsTwo++;
						_nIllegalGaps++;
						_nGaps+=newSubgroupsDayNFirstGaps(sbg, d2)-2;
					}
					_nGaps+=newSubgroupsDayNGaps(sbg,d2);
				}
				
				int _tt=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
				if(_tt>=_nFirstGapsOne){
					_tt-=_nFirstGapsOne;
					_nFirstGapsOne=0;
				}
				else{
					_nFirstGapsOne-=_tt;
					_tt=0;
				}
				if(_tt>=_nFirstGapsTwo){
					_tt-=_nFirstGapsTwo;
					_nFirstGapsTwo=0;
				}
				else{
					_nFirstGapsTwo-=_tt;
					_tt=0;
				}
				
				if(_nFirstGapsTwo>0){
					_nGaps+=_nFirstGapsTwo;
					_nFirstGapsTwo=0;
				}
				
				int _nHoursGaps=0;
				if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
					assert(subgroupsMaxGapsPerWeekPercentage[sbg]==100);
					if(_nGaps>subgroupsMaxGapsPerWeekMaxGaps[sbg])
						_nHoursGaps=_nGaps-subgroupsMaxGapsPerWeekMaxGaps[sbg];
				}
				
				if(_nHours + _nFirstGapsOne + _nHoursGaps + _nIllegalGaps > nHoursPerSubgroup[sbg]){
					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okstudentsearlymaxbeginningsatsecondhour=false;
						goto impossiblestudentsearlymaxbeginningsatsecondhour;
					}

					getSbgTimetable(sbg, conflActivities[newtime]);
					sbgGetNHoursGaps(sbg);

					for(;;){
						int nHours=0;
						int nFirstGapsOne=0;
						int nFirstGapsTwo=0;
						int nGaps=0;
						int nIllegalGaps=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							nHours+=sbgDayNHours[d2];

							if(sbgDayNFirstGaps[d2]==1){
								nFirstGapsOne++;
							}
							else if(sbgDayNFirstGaps[d2]>=2){
								nFirstGapsTwo++;
								nIllegalGaps++;
								nGaps+=sbgDayNFirstGaps[d2]-2;
							}
							nGaps+=sbgDayNGaps[d2];
						}
						
						int tt=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
						if(tt>=nFirstGapsOne){
							tt-=nFirstGapsOne;
							nFirstGapsOne=0;
						}
						else{
							nFirstGapsOne-=tt;
							tt=0;
						}
						if(tt>=nFirstGapsTwo){
							tt-=nFirstGapsTwo;
							nFirstGapsTwo=0;
						}
						else{
							nFirstGapsTwo-=tt;
							tt=0;
						}
						
						if(nFirstGapsTwo>0){
							nGaps+=nFirstGapsTwo;
							nFirstGapsTwo=0;
						}
						
						int nHoursGaps=0;
						if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
							assert(subgroupsMaxGapsPerWeekPercentage[sbg]==100);
							if(nGaps>subgroupsMaxGapsPerWeekMaxGaps[sbg])
								nHoursGaps=nGaps-subgroupsMaxGapsPerWeekMaxGaps[sbg];
						}
				
						int ai2=-1;
						
						if(nHours + nFirstGapsOne + nHoursGaps + nIllegalGaps > nHoursPerSubgroup[sbg]){
							//remove an activity
							bool k=subgroupRemoveAnActivityFromEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								//Added on 2020-09-17 - logic improvement / theoretical bug fix.
								bool k2=false;
								//if(subgroupsMaxGapsPerWeekPercentage[sbg]<0 ||
								// (subgroupsMaxGapsPerWeekPercentage[sbg]>=0 && subgroupsMaxGapsPerWeekMaxGaps[sbg]>0))
								if(subgroupsMaxGapsPerWeekMaxGaps[sbg]!=0){ //-1 or >0
									k2=subgroupRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}
								
								if(!k2){
									if(level==0){
										//this should not be displayed
										//cout<<"WARNING - maybe bug - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okstudentsearlymaxbeginningsatsecondhour=false;
									goto impossiblestudentsearlymaxbeginningsatsecondhour;
								}
							}
						}
						else{ //OK
							break;
						}
						
						assert(ai2>=0);
						
						removeAi2FromSbgTimetable(ai2);
						updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
					}
				}
			}
		
impossiblestudentsearlymaxbeginningsatsecondhour:
		if(!okstudentsearlymaxbeginningsatsecondhour){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students early max beginnings at second hour

/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking students max gaps per week
		//TODO: this should take care of students max gaps per day also. Very critical changes, so be very careful if you do them. Safer -> leave them as they are now.
		//see file fet-v.v.v/doc/algorithm/improve-studentsmaxgapsperday.txt for advice and (unstable) code on how to make students max gaps per day constraint perfect
		okstudentsmaxgapsperweek=true;
		
		for(int sbg : qAsConst(act->iSubgroupsList))
			if(!skipRandom(subgroupsMaxGapsPerWeekPercentage[sbg])){
				//preliminary test
				int _nHours=0;
				int _nGaps=0;
				int _nFirstGaps=0;
				for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
					_nHours+=newSubgroupsDayNHours(sbg,d2);
					_nGaps+=newSubgroupsDayNGaps(sbg,d2);
					_nFirstGaps+=newSubgroupsDayNFirstGaps(sbg,d2);
				}
				
				int _nFirstHours=0;
				
				if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
					assert(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]==100);
					if(_nFirstGaps>subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg])
						_nFirstHours=_nFirstGaps-subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
				}
				
				if(_nGaps+_nHours+_nFirstHours > subgroupsMaxGapsPerWeekMaxGaps[sbg] + nHoursPerSubgroup[sbg]){
					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okstudentsmaxgapsperweek=false;
						goto impossiblestudentsmaxgapsperweek;
					}

					getSbgTimetable(sbg, conflActivities[newtime]);
					sbgGetNHoursGaps(sbg);

					for(;;){
						int nHours=0;
						int nGaps=0;
						int nFirstGaps=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							nHours+=sbgDayNHours[d2];
							nGaps+=sbgDayNGaps[d2];
							nFirstGaps+=sbgDayNFirstGaps[d2];
						}
						
						int ai2=-1;
							
						int nFirstHours=0;

						if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
							assert(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]==100);
							if(nFirstGaps>subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg])
								nFirstHours=nFirstGaps-subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
						}
						
						if(nGaps+nHours+nFirstHours > subgroupsMaxGapsPerWeekMaxGaps[sbg] + nHoursPerSubgroup[sbg]){
							//remove an activity
							bool k=subgroupRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okstudentsmaxgapsperweek=false;
								goto impossiblestudentsmaxgapsperweek;
							}
						}
						else{ //OK
							break;
						}
						
						assert(ai2>=0);

						removeAi2FromSbgTimetable(ai2);
						updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
					}
				}
			}
		
impossiblestudentsmaxgapsperweek:
		if(!okstudentsmaxgapsperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students max gaps per week

/////////////////////////////////////////////////////////////////////////////////////////////

	//!!!NOT PERFECT constraint, in other places may be improved, like in min/max hours daily.
	//see file fet-v.v.v/doc/algorithm/improve-studentsmaxgapsperday.txt for advice and (unstable) code on how to make students max gaps per day constraint perfect

	//not causing more than subgroupsMaxGapsPerDay students gaps
	
	//TODO: improve, check
	
	okstudentsmaxgapsperday=true;
	
	if(haveStudentsMaxGapsPerDay){
		//okstudentsmaxgapsperday=true;
		for(int sbg : qAsConst(act->iSubgroupsList))
			if(!skipRandom(subgroupsMaxGapsPerDayPercentage[sbg])){
				assert(subgroupsMaxGapsPerDayPercentage[sbg]==100);

				//preliminary test
				int _total=0;
				int _remnf=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
				bool _haveMaxBegs=(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0);
				for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
					_total+=newSubgroupsDayNHours(sbg,d2);
					int _g=newSubgroupsDayNGaps(sbg,d2);
					if(_haveMaxBegs){
						int _fg=newSubgroupsDayNFirstGaps(sbg,d2);
						if(_fg==0){
							if(_g>subgroupsMaxGapsPerDayMaxGaps[sbg])
								_total+=_g-subgroupsMaxGapsPerDayMaxGaps[sbg];
						}
						else if(_fg==1){
							if(_remnf>0)
								_remnf--;
							else
								_total++;
							if(_g>subgroupsMaxGapsPerDayMaxGaps[sbg])
								_total+=_g-subgroupsMaxGapsPerDayMaxGaps[sbg];
						}
						else if(_fg>=2){
							if(_g + _fg - 1 <= subgroupsMaxGapsPerDayMaxGaps[sbg])
								_total++;
							else{
								if(_remnf>0)
									_remnf--;
								else
									_total++;
								_total++;
								assert(_g + _fg - 2 >= subgroupsMaxGapsPerDayMaxGaps[sbg]);
								_total+=(_g + _fg - 2 - subgroupsMaxGapsPerDayMaxGaps[sbg]);
							}
						}
						else
							assert(0);
					}
					else{
						if(_g > subgroupsMaxGapsPerDayMaxGaps[sbg])
							_total+=_g-subgroupsMaxGapsPerDayMaxGaps[sbg];
					}
				}
				
				if(_total<=nHoursPerSubgroup[sbg]) //OK
					continue;

				if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
					okstudentsmaxgapsperday=false;
					goto impossiblestudentsmaxgapsperday;
				}

				getSbgTimetable(sbg, conflActivities[newtime]);
				sbgGetNHoursGaps(sbg);

				for(;;){
					int total=0;
					int remnf=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
					bool haveMaxBegs=(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0);
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						total+=sbgDayNHours[d2];
						int g=sbgDayNGaps[d2];
						if(haveMaxBegs){
							int fg=sbgDayNFirstGaps[d2];
							if(fg==0){
								if(g>subgroupsMaxGapsPerDayMaxGaps[sbg])
									total+=g-subgroupsMaxGapsPerDayMaxGaps[sbg];
							}
							else if(fg==1){
								if(remnf>0)
									remnf--;
								else
									total++;
								if(g>subgroupsMaxGapsPerDayMaxGaps[sbg])
									total+=g-subgroupsMaxGapsPerDayMaxGaps[sbg];
							}
							else if(fg>=2){
								if(g + fg - 1 <= subgroupsMaxGapsPerDayMaxGaps[sbg])
									total++;
								else{
									if(remnf>0)
										remnf--;
									else
										total++;
									total++;
									assert(g + fg - 2 >= subgroupsMaxGapsPerDayMaxGaps[sbg]);
									total+=(g + fg - 2 - subgroupsMaxGapsPerDayMaxGaps[sbg]);
								}
							}
							else
								assert(0);
						}
						else{
							if(g > subgroupsMaxGapsPerDayMaxGaps[sbg])
								total+=g-subgroupsMaxGapsPerDayMaxGaps[sbg];
						}
					}
					
					if(total<=nHoursPerSubgroup[sbg]) //OK
						break;

					//remove an activity from the beginning or from the end of a day
					//following code is identical to maxgapsperweek
					//remove an activity
					int ai2=-1;
					
					//it should also be allowed to take from anywhere, but it is risky to change now
					// Addition 2020-09-18: I might be mistaking and should not be allowed to take from anywhere.
					// Also, I think it should be allowed to take from anywhere only if
					// the max allowed gaps per week/day are greater than 0 (or not existing max gaps per week,
					// but this cannot happen, because I compute automatically max_per_week = n_days*max_per_day).
					if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
						bool k=subgroupRemoveAnActivityFromEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!k){
							bool kk;
							if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0 &&
							 (subgroupsMaxGapsPerWeekMaxGaps[sbg]==0 || subgroupsMaxGapsPerDayMaxGaps[sbg]==0))
								kk=false;
							else
								kk=subgroupRemoveAnActivityFromBegin(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);

							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!kk){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okstudentsmaxgapsperday=false;
								goto impossiblestudentsmaxgapsperday;
							}
						}
					}
					else{
						bool k=subgroupRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!k){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okstudentsmaxgapsperday=false;
							goto impossiblestudentsmaxgapsperday;
						}
					}
					
					assert(ai2>=0);

					removeAi2FromSbgTimetable(ai2);
					updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
				}
			}
	}
		
impossiblestudentsmaxgapsperday:
		if(!okstudentsmaxgapsperday){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END max gaps per day

/////////////////////////////////////////////////////////////////////////////////////////////

//2020-07-29

		//2018-10-29 - Maybe old comments below (duplicate from the constraint students max gaps per day).

		//!!!NOT PERFECT constraint, in other places may be improved, like in min/max hours daily.
		//see file fet-v.v.v/doc/algorithm/improve-studentsmaxgapsperday.txt for advice and (unstable) code on how to make students max gaps per day constraint perfect

		//not causing more than subgroupsMaxGapsPerWeekForRealDays students gaps

		//TODO: improve, check

		okstudentsmaxgapsperweekforrealdays=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(haveStudentsMaxGapsPerRealDay){
				//okstudentsmaxgapsperrealday=true;
				for(int sbg : qAsConst(act->iSubgroupsList))
					if(!skipRandom(subgroupsMaxGapsPerWeekForRealDaysPercentage[sbg])){
						assert(subgroupsMaxGapsPerWeekForRealDaysPercentage[sbg]==100);

						//preliminary test
						int _total=0;
						int _remnf=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
						bool _haveMaxBegs=(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0);
						for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
							_total+=newSubgroupsRealDayNHours(sbg,d2)+newSubgroupsRealDayNGaps(sbg,d2);
							if(_haveMaxBegs){
								int _fg=newSubgroupsRealDayNFirstGaps(sbg,d2);
								if(_fg==0){
									//nothing
								}
								else if(_fg==1){
									if(_remnf>0)
										_remnf--;
									else
										_total++;
								}
								else if(_fg>=2){
									if(_remnf>0){
										_remnf--;
										_total+=_fg-1;
									}
									else{
										_total+=_fg;
									}
								}
								else
									assert(0);
							}
							else{
								//nothing
							}
						}

						if(_total<=nHoursPerSubgroup[sbg]+subgroupsMaxGapsPerWeekForRealDaysMaxGaps[sbg]) //OK
							continue;

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okstudentsmaxgapsperweekforrealdays=false;
							goto impossiblestudentsmaxgapsperweekforrealdays;
						}

						getSbgTimetable(sbg, conflActivities[newtime]);
						sbgGetNHoursGapsRealDays(sbg);
						sbgGetNHoursGaps(sbg); //bug fix on 2020-02-29, because the remove an activity functions below need to know the number of hours per half-day.

						for(;;){
							int total=0;
							int remnf=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
							bool haveMaxBegs=(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0);
							for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
								total+=sbgRealDayNHours[d2]+sbgRealDayNGaps[d2];
								if(haveMaxBegs){
									int fg=sbgRealDayNFirstGaps[d2];
									if(fg==0){
										//nothing
									}
									else if(fg==1){
										if(remnf>0)
											remnf--;
										else
											total++;
									}
									else if(fg>=2){
										if(remnf>0){
											remnf--;
											total+=fg-1;
										}
										else{
											total+=fg;
										}
									}
									else
										assert(0);
								}
								else{
									//nothing
								}
							}

							if(total<=nHoursPerSubgroup[sbg]+subgroupsMaxGapsPerWeekForRealDaysMaxGaps[sbg]) //OK
								break;

							//remove an activity from the beginning or from the end of a real day
							//following code is identical to maxgapsperweek
							//remove an activity
							int ai2=-1;

							//it should also be allowed to take from anywhere, but it is risky to change now
							if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
								bool k=subgroupRemoveAnActivityFromEndAfternoon(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k){
									bool kk;
									if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0 &&
									 (subgroupsMaxGapsPerRealDayMaxGaps[sbg]==0 || subgroupsMaxGapsPerWeekForRealDaysMaxGaps[sbg]==0) )
										kk=false;
									else
										kk=subgroupRemoveAnActivityFromBeginMorning(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);

									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									if(!kk){
										if(level==0){
											//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
											//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
										}
										okstudentsmaxgapsperweekforrealdays=false;
										goto impossiblestudentsmaxgapsperweekforrealdays;
									}
								}
							}
							else{
								bool k=subgroupRemoveAnActivityFromBeginMorningOrEndAfternoon(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k){
									if(level==0){
										//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okstudentsmaxgapsperweekforrealdays=false;
									goto impossiblestudentsmaxgapsperweekforrealdays;
								}
							}

							assert(ai2>=0);

							removeAi2FromSbgTimetable(ai2);
							updateSbgNHoursGapsRealDay(sbg, (c.times[ai2]%gt.rules.nDaysPerWeek)/2);
							updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek); //bug fix on 2020-02-29, because the remove an activity functions above need to know the number of hours per half-day.
						}
					}
			}
		}

impossiblestudentsmaxgapsperweekforrealdays:
		if(!okstudentsmaxgapsperweekforrealdays){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END students max gaps per week for real days

/////////////////////////////////////////////////////////////////////////////////////////////

		//2018-10-29 - Maybe old comments below (duplicate from the constraint students max gaps per day.

		//!!!NOT PERFECT constraint, in other places may be improved, like in min/max hours daily.
		//see file fet-v.v.v/doc/algorithm/improve-studentsmaxgapsperday.txt for advice and (unstable) code on how to make students max gaps per day constraint perfect

		//not causing more than subgroupsMaxGapsPerRealDay students gaps

		//TODO: improve, check

		okstudentsmaxgapsperrealday=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(haveStudentsMaxGapsPerRealDay){
				//okstudentsmaxgapsperrealday=true;
				for(int sbg : qAsConst(act->iSubgroupsList))
					if(!skipRandom(subgroupsMaxGapsPerRealDayPercentage[sbg])){
						assert(subgroupsMaxGapsPerRealDayPercentage[sbg]==100);

						//preliminary test
						int _total=0;
						int _remnf=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
						bool _haveMaxBegs=(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0);
						for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
							_total+=newSubgroupsRealDayNHours(sbg,d2);
							int _g=newSubgroupsRealDayNGaps(sbg,d2);
							if(_haveMaxBegs){
								int _fg=newSubgroupsRealDayNFirstGaps(sbg,d2);
								if(_fg==0){
									if(_g>subgroupsMaxGapsPerRealDayMaxGaps[sbg])
										_total+=_g-subgroupsMaxGapsPerRealDayMaxGaps[sbg];
								}
								else if(_fg==1){
									if(_remnf>0)
										_remnf--;
									else
										_total++;
									if(_g>subgroupsMaxGapsPerRealDayMaxGaps[sbg])
										_total+=_g-subgroupsMaxGapsPerRealDayMaxGaps[sbg];
								}
								else if(_fg>=2){
									if(_g + _fg - 1 <= subgroupsMaxGapsPerRealDayMaxGaps[sbg])
										_total++;
									else{
										if(_remnf>0)
											_remnf--;
										else
											_total++;
										_total++;
										assert(_g + _fg - 2 >= subgroupsMaxGapsPerRealDayMaxGaps[sbg]);
										_total+=(_g + _fg - 2 - subgroupsMaxGapsPerRealDayMaxGaps[sbg]);
									}
								}
								else
									assert(0);
							}
							else{
								if(_g > subgroupsMaxGapsPerRealDayMaxGaps[sbg])
									_total+=_g-subgroupsMaxGapsPerRealDayMaxGaps[sbg];
							}
						}

						if(_total<=nHoursPerSubgroup[sbg]) //OK
							continue;

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okstudentsmaxgapsperrealday=false;
							goto impossiblestudentsmaxgapsperrealday;
						}

						getSbgTimetable(sbg, conflActivities[newtime]);
						sbgGetNHoursGapsRealDays(sbg);
						sbgGetNHoursGaps(sbg); //bug fix on 2020-02-29, because the remove an activity functions below need to know the number of hours per half-day.

						for(;;){
							int total=0;
							int remnf=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
							bool haveMaxBegs=(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0);
							for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
								total+=sbgRealDayNHours[d2];
								int g=sbgRealDayNGaps[d2];
								if(haveMaxBegs){
									int fg=sbgRealDayNFirstGaps[d2];
									if(fg==0){
										if(g>subgroupsMaxGapsPerRealDayMaxGaps[sbg])
											total+=g-subgroupsMaxGapsPerRealDayMaxGaps[sbg];
									}
									else if(fg==1){
										if(remnf>0)
											remnf--;
										else
											total++;
										if(g>subgroupsMaxGapsPerRealDayMaxGaps[sbg])
											total+=g-subgroupsMaxGapsPerRealDayMaxGaps[sbg];
									}
									else if(fg>=2){
										if(g + fg - 1 <= subgroupsMaxGapsPerRealDayMaxGaps[sbg])
											total++;
										else{
											if(remnf>0)
												remnf--;
											else
												total++;
											total++;
											assert(g + fg - 2 >= subgroupsMaxGapsPerRealDayMaxGaps[sbg]);
											total+=(g + fg - 2 - subgroupsMaxGapsPerRealDayMaxGaps[sbg]);
										}
									}
									else
										assert(0);
								}
								else{
									if(g > subgroupsMaxGapsPerRealDayMaxGaps[sbg])
										total+=g-subgroupsMaxGapsPerRealDayMaxGaps[sbg];
								}
							}

							if(total<=nHoursPerSubgroup[sbg]) //OK
								break;

							//remove an activity from the beginning or from the end of a real day
							//following code is identical to maxgapsperweek
							//remove an activity
							int ai2=-1;

							//it should also be allowed to take from anywhere, but it is risky to change now
							if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
								bool k=subgroupRemoveAnActivityFromEndAfternoon(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k){
									bool kk;
									if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0 &&
									 (subgroupsMaxGapsPerRealDayMaxGaps[sbg]==0 || subgroupsMaxGapsPerWeekForRealDaysMaxGaps[sbg]==0) )
										kk=false;
									else
										kk=subgroupRemoveAnActivityFromBeginMorning(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);

									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									if(!kk){
										if(level==0){
											//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
											//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
										}
										okstudentsmaxgapsperrealday=false;
										goto impossiblestudentsmaxgapsperrealday;
									}
								}
							}
							else{
								bool k=subgroupRemoveAnActivityFromBeginMorningOrEndAfternoon(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k){
									if(level==0){
										//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okstudentsmaxgapsperrealday=false;
									goto impossiblestudentsmaxgapsperrealday;
								}
							}

							assert(ai2>=0);

							removeAi2FromSbgTimetable(ai2);
							updateSbgNHoursGapsRealDay(sbg, (c.times[ai2]%gt.rules.nDaysPerWeek)/2);
							updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek); //bug fix on 2020-02-29, because the remove an activity functions above need to know the number of hours per half-day.
						}
					}
			}
		}

impossiblestudentsmaxgapsperrealday:
		if(!okstudentsmaxgapsperrealday){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END max gaps per real day

/////////////////////////////////////////////////////////////////////////////////////////////

		//to be put after max gaps and early!!! because of an assert

		//allowed from students max hours daily
		//TODO: this should take care of students max gaps per day also. Very critical changes, so be very careful if you do them. Safer -> leave them as they are now.
		//see file fet-v.v.v/doc/algorithm/improve-studentsmaxgapsperday.txt for advice and (unstable) code on how to make students max gaps per day constraint perfect
		okstudentsmaxhoursdaily=true;
		
		for(int sbg : qAsConst(act->iSubgroupsList)){
			for(int count=0; count<2; count++){
				int limitHoursDaily;
				double percentage;
				if(count==0){
					limitHoursDaily=subgroupsMaxHoursDailyMaxHours1[sbg];
					percentage=subgroupsMaxHoursDailyPercentages1[sbg];
				}
				else{
					limitHoursDaily=subgroupsMaxHoursDailyMaxHours2[sbg];
					percentage=subgroupsMaxHoursDailyPercentages2[sbg];
				}
				
				if(limitHoursDaily<0)
					continue;
				
				//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
				//	continue;
				
				bool increased;
				if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
					if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
						//both
						if(oldSubgroupsDayNHours(sbg,d)+oldSubgroupsDayNGaps(sbg,d)+oldSubgroupsDayNFirstGaps(sbg,d)<
						  newSubgroupsDayNHours(sbg,d)+newSubgroupsDayNGaps(sbg,d)+newSubgroupsDayNFirstGaps(sbg,d)
						  || oldSubgroupsDayNHours(sbg,d)<newSubgroupsDayNHours(sbg,d))
							increased=true;
						else
							increased=false;
					}
					else{
						//only at beginning
						if(oldSubgroupsDayNHours(sbg,d)+oldSubgroupsDayNFirstGaps(sbg,d)<
						  newSubgroupsDayNHours(sbg,d)+newSubgroupsDayNFirstGaps(sbg,d)
						  || oldSubgroupsDayNHours(sbg,d)<newSubgroupsDayNHours(sbg,d))
							increased=true;
						else
							increased=false;
					}
				}
				else{
					if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
						//only max gaps
						if(oldSubgroupsDayNHours(sbg,d)+oldSubgroupsDayNGaps(sbg,d)<
						  newSubgroupsDayNHours(sbg,d)+newSubgroupsDayNGaps(sbg,d)
						  || oldSubgroupsDayNHours(sbg,d)<newSubgroupsDayNHours(sbg,d))
							increased=true;
						else
							increased=false;
					}
					else{
						//none
						if(oldSubgroupsDayNHours(sbg,d)<newSubgroupsDayNHours(sbg,d))
							increased=true;
						else
							increased=false;
					}
				}
			
				if(limitHoursDaily>=0 && !skipRandom(percentage) && increased){
					if(limitHoursDaily<act->duration){
						okstudentsmaxhoursdaily=false;
						goto impossiblestudentsmaxhoursdaily;
					}
					
					if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0 && subgroupsMaxGapsPerWeekMaxGaps[sbg]==0){
						if(newSubgroupsDayNHours(sbg,d)+newSubgroupsDayNGaps(sbg,d)+newSubgroupsDayNFirstGaps(sbg,d) > limitHoursDaily){
							okstudentsmaxhoursdaily=false;
							goto impossiblestudentsmaxhoursdaily;
						}
						else //OK
							continue;
					}
					
					//////////////////////////new
					bool _ok;
					if(newSubgroupsDayNHours(sbg,d)>limitHoursDaily){
						_ok=false; //trivially
					}
					else{
						//basically, see that the gaps are enough
						// Comment added on 2020-09-15: This code was written a long time ago. It cares that the gaps are enough, but it is more like a heuristic,
						// because the weight might be any real number below 100.0%. So on other days the constraints should be allowed to be broken.
						// However, it is very risky to change now. I think that the best would be to allow max hours daily only with 100.0% weight,
						// but unfortunately I think that many users have files with weight <100.0%.
						// Also, don't forget that we might have two constraints max hours daily for each subgroup.
						if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
							if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
								//both
								int rg=subgroupsMaxGapsPerWeekMaxGaps[sbg]+subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(d2!=d){
										int g=limitHoursDaily-newSubgroupsDayNHours(sbg,d2);
										//TODO: if g lower than 0 make g 0
										//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
										g=newSubgroupsDayNFirstGaps(sbg,d2)+newSubgroupsDayNGaps(sbg,d2)-g;
										if(g>0)
											rg-=g;
									}
								}
								
								if(rg<0)
									rg=0;
								
								int hg=newSubgroupsDayNGaps(sbg,d)+newSubgroupsDayNFirstGaps(sbg,d)-rg;
								if(hg<0)
									hg=0;
									
								if(hg+newSubgroupsDayNHours(sbg,d) > limitHoursDaily){
									_ok=false;
								}
								else
									_ok=true;
							}
							else{
								//only max beginnings
								int lateBeginnings=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(d2!=d){
										if(newSubgroupsDayNHours(sbg,d2)>=limitHoursDaily && newSubgroupsDayNFirstGaps(sbg,d2)==1)
											lateBeginnings++;
									}
								}
								
								int fg=0, ah=0; //first gaps, added hours
								if(newSubgroupsDayNFirstGaps(sbg,d)==0){
									fg=0;
									ah=0;
								}
								else if(newSubgroupsDayNFirstGaps(sbg,d)==1){
									fg=1;
									ah=0;
									if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0 ||
									 (subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]>0 &&
									 lateBeginnings>=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]))
										ah+=fg;
									
								}
								else if(newSubgroupsDayNFirstGaps(sbg,d)>=2){
									fg=0;
									ah=1;
								}
								
								if(ah+newSubgroupsDayNHours(sbg,d) > limitHoursDaily){
									_ok=false;
								}
								else
									_ok=true;
							}
						}
						else{
							if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
								//only max gaps
								int rg=subgroupsMaxGapsPerWeekMaxGaps[sbg];
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(d2!=d){
										int g=limitHoursDaily-newSubgroupsDayNHours(sbg,d2);
										//TODO: if g lower than 0 make g 0
										//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
										g=newSubgroupsDayNGaps(sbg,d2)-g;
										if(g>0)
											rg-=g;
									}
								}
								
								if(rg<0)
									rg=0;
								
								int hg=newSubgroupsDayNGaps(sbg,d)-rg;
								if(hg<0)
									hg=0;
									
								if(hg+newSubgroupsDayNHours(sbg,d) > limitHoursDaily){
									_ok=false;
								}
								else
									_ok=true;
							}
							else{
								//none
								_ok=true;
							}
						}
					}
					
					/////////////////////////////
					
					//preliminary test
					if(_ok){
						continue;
					}
					
					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okstudentsmaxhoursdaily=false;
						goto impossiblestudentsmaxhoursdaily;
					}
					
					getSbgTimetable(sbg, conflActivities[newtime]);
					sbgGetNHoursGaps(sbg);
					
					//OLD COMMENT BELOW: (now things seem theoretically and practically OK)
					//theoretically, it should be canTakeFromBegin = true all time and ctfAnywhere = true if max gaps per week is not 0.
					//but practically, I tried these changes and it was 30% slower for a modified german sample (with max gaps per day=1,
					//12 hours per day, removed subacts. pref. times, max hours daily 6 for students).
					bool canTakeFromBegin=(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]!=0); //-1 or >0
					bool canTakeFromEnd=true;
					bool canTakeFromAnywhere=(subgroupsMaxGapsPerWeekMaxGaps[sbg]!=0 && subgroupsMaxGapsPerDayMaxGaps[sbg]!=0); //-1 or >0
					bool canTakeFromBeginOrEndAnyDay=(subgroupsMaxGapsPerWeekMaxGaps[sbg]>=0 ||
					 subgroupsMaxGapsPerDayMaxGaps[sbg]>=0 || subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]>=0);
		
					for(;;){
						//////////////////////////new
						bool ok;
						if(sbgDayNHours[d]>limitHoursDaily){
							ok=false; //trivially
						}
						else{
							//basically, see that the gaps are enough
							// Comment added on 2020-09-15: This code was written a long time ago. It cares that the gaps are enough, but it is more like a heuristic,
							// because the weight might be any real number below 100.0%. So on other days the constraints should be allowed to be broken.
							// However, it is very risky to change now. I think that the best would be to allow max hours daily only with 100.0% weight,
							// but unfortunately I think that many users have files with weight <100.0%.
							// Also, don't forget that we might have two constraints max hours daily for each subgroup.
							if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
								if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
									//both
									int rg=subgroupsMaxGapsPerWeekMaxGaps[sbg]+subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										if(d2!=d){
											int g=limitHoursDaily-sbgDayNHours[d2];
											//TODO: if g lower than 0 make g 0
											//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
											g=sbgDayNFirstGaps[d2]+sbgDayNGaps[d2]-g;
											if(g>0)
												rg-=g;
										}
									}
									
									if(rg<0)
										rg=0;
									
									int hg=sbgDayNGaps[d]+sbgDayNFirstGaps[d]-rg;
									if(hg<0)
										hg=0;
									
									if(hg+sbgDayNHours[d] > limitHoursDaily){
										ok=false;
									}
									else
										ok=true;
								}
								else{
									//only max beginnings
									int lateBeginnings=0;
									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										if(d2!=d){
											if(sbgDayNHours[d2]>=limitHoursDaily && sbgDayNFirstGaps[d2]==1)
												lateBeginnings++;
										}
									}
									
									int fg=0, ah=0; //first gaps, added hours
									if(sbgDayNFirstGaps[d]==0){
										fg=0;
										ah=0;
									}
									else if(sbgDayNFirstGaps[d]==1){
										fg=1;
										ah=0;
										if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0 ||
										 (subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]>0 &&
										 lateBeginnings>=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]))
											ah+=fg;
										
									}
									else if(sbgDayNFirstGaps[d]>=2){
										fg=0;
										ah=1;
									}
								
									if(ah+sbgDayNHours[d] > limitHoursDaily){
										ok=false;
									}
									else
										ok=true;
								}
							}
							else{
								if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
									//only max gaps
									int rg=subgroupsMaxGapsPerWeekMaxGaps[sbg];
									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										if(d2!=d){
											int g=limitHoursDaily-sbgDayNHours[d2];
											//TODO: if g lower than 0 make g 0
											//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
											g=sbgDayNGaps[d2]-g;
											if(g>0)
												rg-=g;
										}
									}
									
									if(rg<0)
										rg=0;
									
									int hg=sbgDayNGaps[d]-rg;
									if(hg<0)
										hg=0;
									
									if(hg+sbgDayNHours[d] > limitHoursDaily){
										ok=false;
									}
									else
										ok=true;
								}
								else{
									//none
									ok=true;
								}
							}
						}
						/////////////////////////////
					
						if(ok){
							break;
						}
						
						int ai2=-1;

						bool kk=false;
						if(canTakeFromEnd)
							kk=subgroupRemoveAnActivityFromEndCertainDay(d, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!kk){
							canTakeFromEnd=false;
							bool k=false;
							if(canTakeFromBegin){
								k=subgroupRemoveAnActivityFromBeginCertainDay(d, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]>0)
									canTakeFromBegin=false;
							}
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								canTakeFromBegin=false;
								bool ka=false;
								if(canTakeFromAnywhere)
									ka=subgroupRemoveAnActivityFromAnywhereCertainDay(d, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								
								if(!ka){
									canTakeFromAnywhere=false;
									bool kaa=false;
									if(canTakeFromBeginOrEndAnyDay && sbgDayNHours[d]<=limitHoursDaily)
										//Fix on 2017-08-26, to solve Volker Dirr's bug report
										kaa=subgroupRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									
									if(!kaa){
										canTakeFromBeginOrEndAnyDay=false; //useless
										if(level==0){
											//this should not be displayed
											//cout<<"WARNING - file "<<__FILE__<<" line "<<__LINE__<<endl;
										}
										okstudentsmaxhoursdaily=false;
										goto impossiblestudentsmaxhoursdaily;
									}
								}
							}
						}
		
						assert(ai2>=0);

						removeAi2FromSbgTimetable(ai2);
						updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
					}
				}
			}
		}
		
impossiblestudentsmaxhoursdaily:
		if(!okstudentsmaxhoursdaily){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//to be put after max gaps and early!!! because of an assert

		//allowed from students max hours daily real days
		//TODO: this should take care of students max gaps per day also. Very critical changes, so be very careful if you do them. Safer -> leave them as they are now.
		//see file fet-v.v.v/doc/algorithm/improve-studentsmaxgapsperday.txt for advice and (unstable) code on how to make students max gaps per day constraint perfect
		okstudentsmaxhoursdailyrealdays=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int sbg : qAsConst(act->iSubgroupsList)){
				for(int count=0; count<2; count++){
					int limitHoursDaily;
					double percentage;
					if(count==0){
						limitHoursDaily=subgroupsMaxHoursDailyRealDaysMaxHours1[sbg];
						percentage=subgroupsMaxHoursDailyRealDaysPercentages1[sbg];
					}
					else{
						limitHoursDaily=subgroupsMaxHoursDailyRealDaysMaxHours2[sbg];
						percentage=subgroupsMaxHoursDailyRealDaysPercentages2[sbg];
					}

					if(limitHoursDaily<0)
						continue;

					//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
					//	continue;

					bool increased;
					/*if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
						if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
							//both
							if(oldSubgroupsDayNHours(sbg,d)+oldSubgroupsDayNGaps(sbg,d)+oldSubgroupsDayNFirstGaps(sbg,d)<
							  newSubgroupsDayNHours(sbg,d)+newSubgroupsDayNGaps(sbg,d)+newSubgroupsDayNFirstGaps(sbg,d)
							  || oldSubgroupsDayNHours(sbg,d)<newSubgroupsDayNHours(sbg,d))
								increased=true;
							else
								increased=false;
						}
						else{
							//only at beginning
							if(oldSubgroupsDayNHours(sbg,d)+oldSubgroupsDayNFirstGaps(sbg,d)<
							  newSubgroupsDayNHours(sbg,d)+newSubgroupsDayNFirstGaps(sbg,d)
							  || oldSubgroupsDayNHours(sbg,d)<newSubgroupsDayNHours(sbg,d))
								increased=true;
							else
								increased=false;
						}
					}
					else{
						if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
							//only max gaps
							if(oldSubgroupsDayNHours(sbg,d)+oldSubgroupsDayNGaps(sbg,d)<
							  newSubgroupsDayNHours(sbg,d)+newSubgroupsDayNGaps(sbg,d)
							  || oldSubgroupsDayNHours(sbg,d)<newSubgroupsDayNHours(sbg,d))
								increased=true;
							else
								increased=false;
						}
						else{
							//none
							if(oldSubgroupsDayNHours(sbg,d)<newSubgroupsDayNHours(sbg,d))
								increased=true;
							else
								increased=false;
						}
					}*/

					//Liviu Lalescu 2021-03-29: I think this needs to remain true, because of gaps per day/real day/real day per week.
					increased=true; //?????

					if(limitHoursDaily>=0 && !skipRandom(percentage) && increased){
						if(limitHoursDaily<act->duration){
							okstudentsmaxhoursdailyrealdays=false;
							goto impossiblestudentsmaxhoursdailyrealdays;
						}

						if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0 && subgroupsMaxGapsPerWeekMaxGaps[sbg]==0){
							if(newSubgroupsDayNHours(sbg,d)+newSubgroupsDayNGaps(sbg,d)+newSubgroupsDayNFirstGaps(sbg,d)+
							  newSubgroupsDayNHours(sbg,dpair)+newSubgroupsDayNGaps(sbg,dpair)+newSubgroupsDayNFirstGaps(sbg,dpair)> limitHoursDaily){
								okstudentsmaxhoursdailyrealdays=false;
								goto impossiblestudentsmaxhoursdailyrealdays;
							}
							else //OK
								continue;
						}

						//////////////////////////new
						bool _ok;
						if(newSubgroupsDayNHours(sbg,d)+newSubgroupsDayNHours(sbg,dpair)>limitHoursDaily){
							_ok=false; //trivially
						}
						//basically, see that the gaps are enough
						else{
							if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
								if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
									//both
									int rg=subgroupsMaxGapsPerWeekMaxGaps[sbg]+subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
									for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
										if(d2!=d/2){
											int dfet1=d2*2, dfet2=d2*2+1;
											int g=limitHoursDaily-newSubgroupsDayNHours(sbg,dfet1)-newSubgroupsDayNHours(sbg,dfet2);
											//TODO: if g lower than 0 make g 0
											//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
											g=newSubgroupsDayNFirstGaps(sbg,dfet1)+newSubgroupsDayNGaps(sbg,dfet1)+
											  newSubgroupsDayNFirstGaps(sbg,dfet2)+newSubgroupsDayNGaps(sbg,dfet2)-g;
											if(g>0)
												rg-=g;
										}
									}

									if(rg<0)
										rg=0;

									int hg=newSubgroupsDayNGaps(sbg,d)+newSubgroupsDayNFirstGaps(sbg,d)+
									  newSubgroupsDayNGaps(sbg,dpair)+newSubgroupsDayNFirstGaps(sbg,dpair)-rg;
									if(hg<0)
										hg=0;

									if(hg+newSubgroupsDayNHours(sbg,d)+newSubgroupsDayNHours(sbg,dpair) > limitHoursDaily){
										_ok=false;
									}
									else
										_ok=true;
								}
								else{
									//only max beginnings
									/*
									int lateBeginnings=0;
									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										if(d2!=d){
											if(newSubgroupsDayNHours(sbg,d2)>=limitHoursDaily && newSubgroupsDayNFirstGaps(sbg,d2)==1)
												lateBeginnings++;
										}
									}

									int fg=0, ah=0; //first gaps, added hours
									if(newSubgroupsDayNFirstGaps(sbg,d)==0){
										fg=0;
										ah=0;
									}
									else if(newSubgroupsDayNFirstGaps(sbg,d)==1){
										fg=1;
										ah=0;
										if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0 ||
										 (subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]>0 &&
										 lateBeginnings>=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]))
											ah+=fg;

									}
									else if(newSubgroupsDayNFirstGaps(sbg,d)>=2){
										fg=0;
										ah=1;
									}*/

									int ah=0;
									if(ah+newSubgroupsDayNHours(sbg,d)+newSubgroupsDayNHours(sbg,dpair) > limitHoursDaily){
										_ok=false;
									}
									else
										_ok=true;
								}
							}
							else{
								if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
									//only max gaps
									int rg=subgroupsMaxGapsPerWeekMaxGaps[sbg];
									for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
										if(d2!=d/2){
											int dfet1=d2*2, dfet2=d2*2+1;
											int g=limitHoursDaily-newSubgroupsDayNHours(sbg,dfet1)-newSubgroupsDayNHours(sbg,dfet2);
											//TODO: if g lower than 0 make g 0
											//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
											g=newSubgroupsDayNGaps(sbg,dfet1)+newSubgroupsDayNGaps(sbg,dfet2)-g;
											if(g>0)
												rg-=g;
										}
									}

									if(rg<0)
										rg=0;

									int hg=newSubgroupsDayNGaps(sbg,d)+newSubgroupsDayNGaps(sbg,dpair)-rg;
									if(hg<0)
										hg=0;

									if(hg+newSubgroupsDayNHours(sbg,d)+newSubgroupsDayNHours(sbg,dpair) > limitHoursDaily){
										_ok=false;
									}
									else
										_ok=true;
								}
								else{
									//none
									_ok=true;
								}
							}
						}

						/////////////////////////////

						//preliminary test
						if(_ok){
							continue;
						}

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okstudentsmaxhoursdailyrealdays=false;
							goto impossiblestudentsmaxhoursdailyrealdays;
						}

						getSbgTimetable(sbg, conflActivities[newtime]);
						sbgGetNHoursGaps(sbg);

						//OLD COMMENT BELOW: (now things seem theoretically and practically OK)
						//theoretically, it should be canTakeFromBegin = true all time and ctfAnywhere = true if max gaps per week is not 0.
						//but practically, I tried these changes and it was 30% slower for a modified german sample (with max gaps per day=1,
						//12 hours per day, removed subacts. pref. times, max hours daily 6 for students).
						bool canTakeFromBegin=(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]!=0); //-1 or >0
						bool canTakeFromEnd=true;
						bool canTakeFromAnywhere=(subgroupsMaxGapsPerWeekMaxGaps[sbg]!=0 && subgroupsMaxGapsPerDayMaxGaps[sbg]!=0); //-1 or >0
						bool canTakeFromBeginOrEndAnyDay=(subgroupsMaxGapsPerWeekMaxGaps[sbg]>=0 ||
						 subgroupsMaxGapsPerDayMaxGaps[sbg]>=0 || subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]>=0);

						for(;;){
							//////////////////////////new
							bool ok;

							if(sbgDayNHours[d]+sbgDayNHours[dpair]>limitHoursDaily){
								ok=false;
							}
							else{
								if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
									if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
										//both
										int rg=subgroupsMaxGapsPerWeekMaxGaps[sbg]+subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg];
										for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
											if(d2!=d/2){
												int dfet1=d2*2, dfet2=d2*2+1;
												int g=limitHoursDaily-sbgDayNHours[dfet1]-sbgDayNHours[dfet2];
												//TODO: if g lower than 0 make g 0
												//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
												g=sbgDayNFirstGaps[dfet1]+sbgDayNGaps[dfet1]+sbgDayNFirstGaps[dfet2]+sbgDayNGaps[dfet2]-g;
												if(g>0)
													rg-=g;
											}
										}

										if(rg<0)
											rg=0;

										int hg=sbgDayNGaps[d]+sbgDayNFirstGaps[d]
										  +sbgDayNGaps[dpair]+sbgDayNFirstGaps[dpair]-rg;
										if(hg<0)
											hg=0;

										if(hg+sbgDayNHours[d]+sbgDayNHours[dpair] > limitHoursDaily){
											ok=false;
										}
										else
											ok=true;
									}
									else{
										//only max beginnings
										/*
										int lateBeginnings=0;
										for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
											if(d2!=d){
												if(sbgDayNHours[d2]>=limitHoursDaily && sbgDayNFirstGaps[d2]==1)
													lateBeginnings++;
											}
										}

										int fg=0, ah=0; //first gaps, added hours
										if(sbgDayNFirstGaps[d]==0){
											fg=0;
											ah=0;
										}
										else if(sbgDayNFirstGaps[d]==1){
											fg=1;
											ah=0;
											if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0 ||
											 (subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]>0 &&
											 lateBeginnings>=subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]))
												ah+=fg;

										}
										else if(sbgDayNFirstGaps[d]>=2){
											fg=0;
											ah=1;
										}*/

										int ah=0;

										if(ah+sbgDayNHours[d]+sbgDayNHours[dpair] > limitHoursDaily){
											ok=false;
										}
										else
											ok=true;
									}
								}
								else{
									if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
										//only max gaps
										int rg=subgroupsMaxGapsPerWeekMaxGaps[sbg];
										for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
											if(d2!=d/2){
												int dfet1=2*d2, dfet2=2*d2+1;
												int g=limitHoursDaily-sbgDayNHours[dfet1]-sbgDayNHours[dfet2];
												//TODO: if g lower than 0 make g 0
												//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
												g=sbgDayNGaps[dfet1]+sbgDayNGaps[dfet2]-g;
												if(g>0)
													rg-=g;
											}
										}

										if(rg<0)
											rg=0;

										int hg=sbgDayNGaps[d]+sbgDayNGaps[dpair]-rg;
										if(hg<0)
											hg=0;

										if(hg+sbgDayNHours[d]+sbgDayNHours[dpair] > limitHoursDaily){
											ok=false;
										}
										else
											ok=true;
									}
									else{
										//none
										ok=true;
									}
								}
							}
							/////////////////////////////

							if(ok){
								break;
							}

							int ai2=-1;

							bool kk=false;
							if(canTakeFromEnd)
								kk=subgroupRemoveAnActivityFromEndCertainTwoDays(d, dpair, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!kk){
								canTakeFromEnd=false;
								bool k=false;
								if(canTakeFromBegin){
									k=subgroupRemoveAnActivityFromBeginCertainTwoDays(d, dpair, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]>0)
										canTakeFromBegin=false;
								}
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k){
									canTakeFromBegin=false;
									bool ka=false;
									if(canTakeFromAnywhere)
										ka=subgroupRemoveAnActivityFromAnywhereCertainTwoDays(d, dpair, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);

									if(!ka){
										canTakeFromAnywhere=false;
										bool kaa=false;
										if(canTakeFromBeginOrEndAnyDay && sbgDayNHours[d]+sbgDayNHours[dpair]<=limitHoursDaily)
											//Fix on 2017-08-26, to solve Volker Dirr's bug report
											kaa=subgroupRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
										assert(conflActivities[newtime].count()==nConflActivities[newtime]);

										if(!kaa){
											canTakeFromBeginOrEndAnyDay=false; //useless
											if(level==0){
												/*cout<<"subgroup=="<<qPrintable(gt.rules.internalSubgroupsList[sbg]->name)<<endl;
												cout<<"d=="<<d<<endl;
												cout<<"H="<<H<<endl;
												cout<<"Timetable:"<<endl;
												for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
													for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
														cout<<"\t"<<sbgTimetable(d2,h2)<<"\t";
													cout<<endl;
												}*/

												//this should not be displayed
												//cout<<"WARNING - file "<<__FILE__<<" line "<<__LINE__<<endl;
											}
											okstudentsmaxhoursdailyrealdays=false;
											goto impossiblestudentsmaxhoursdailyrealdays;
										}
									}
								}
							}

							assert(ai2>=0);

							removeAi2FromSbgTimetable(ai2);
							updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
		}

impossiblestudentsmaxhoursdailyrealdays:
		if(!okstudentsmaxhoursdailyrealdays){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from students max hours continuously
		
		okstudentsmaxhourscontinuously=true;
		
		for(int sbg : qAsConst(act->iSubgroupsList)){
			for(int count=0; count<2; count++){
				int limitHoursCont;
				double percentage;
				if(count==0){
					limitHoursCont=subgroupsMaxHoursContinuouslyMaxHours1[sbg];
					percentage=subgroupsMaxHoursContinuouslyPercentages1[sbg];
				}
				else{
					limitHoursCont=subgroupsMaxHoursContinuouslyMaxHours2[sbg];
					percentage=subgroupsMaxHoursContinuouslyPercentages2[sbg];
				}
				
				if(limitHoursCont<0) //no constraint
					continue;
				
				//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
				//	continue;
				
				bool increased;
				int h2;
				for(h2=h; h2<h+act->duration; h2++){
					assert(h2<gt.rules.nHoursPerDay);
					if(subgroupsTimetable(sbg,d,h2)==-1)
						break;
				}
				if(h2<h+act->duration)
					increased=true;
				else
					increased=false;
					
				QList<int> removableActs;
					
				int nc=act->duration;
				for(h2=h-1; h2>=0; h2--){
					int ai2=subgroupsTimetable(sbg,d,h2);
					assert(ai2==newSubgroupsTimetable(sbg,d,h2));
					assert(ai2!=ai);
					if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
						nc++;
						
						if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
							removableActs.append(ai2);
					}
					else
						break;
				}
				for(h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
					int ai2=subgroupsTimetable(sbg,d,h2);
					assert(ai2==newSubgroupsTimetable(sbg,d,h2));
					assert(ai2!=ai);
					if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
						nc++;
						
						if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
							removableActs.append(ai2);
					}
					else
						break;
				}
				
				if(!increased && percentage==100.0)
					assert(nc<=limitHoursCont);
					
				if(!increased || nc<=limitHoursCont) //OK
					continue;
					
				assert(limitHoursCont>=0);

				if(!skipRandom(percentage) && increased){
					if(act->duration>limitHoursCont){
						okstudentsmaxhourscontinuously=false;
						goto impossiblestudentsmaxhourscontinuously;
					}
					
					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okstudentsmaxhourscontinuously=false;
						goto impossiblestudentsmaxhourscontinuously;
					}
					
					while(true){
						if(removableActs.count()==0){
							okstudentsmaxhourscontinuously=false;
							goto impossiblestudentsmaxhourscontinuously;
						}
						
						int j=-1;
					
						if(level==0){
							int optMinWrong=INF;
			
							QList<int> tl;

							for(int q=0; q<removableActs.count(); q++){
								int ai2=removableActs.at(q);
								if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
									optMinWrong=triedRemovals(ai2,c.times[ai2]);
								}
							}
				
							for(int q=0; q<removableActs.count(); q++){
								int ai2=removableActs.at(q);
								if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
									tl.append(q);
							}
			
							assert(tl.count()>=1);
							j=tl.at(rng.intMRG32k3a(tl.count()));
			
							assert(j>=0 && j<removableActs.count());
						}
						else{
							j=rng.intMRG32k3a(removableActs.count());
						}
						
						assert(j>=0);
						
						int ai2=removableActs.at(j);
						
						int t=removableActs.removeAll(ai2);
						assert(t==1);
						
						assert(!conflActivities[newtime].contains(ai2));
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						
						////////////
						removableActs.clear();
						
						int nc=act->duration;
						int h2;
						for(h2=h-1; h2>=0; h2--){
							int ai2=subgroupsTimetable(sbg,d,h2);
							assert(ai2==newSubgroupsTimetable(sbg,d,h2));
							assert(ai2!=ai);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
								nc++;
							
								if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
									removableActs.append(ai2);
							}
							else
								break;
						}
						for(h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
							int ai2=subgroupsTimetable(sbg,d,h2);
							assert(ai2==newSubgroupsTimetable(sbg,d,h2));
							assert(ai2!=ai);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
								nc++;
						
								if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
									removableActs.append(ai2);
							}
							else
								break;
						}
						
						if(nc<=limitHoursCont) //OK
							break;
						////////////
					}
				}
			}
		}
		
impossiblestudentsmaxhourscontinuously:
		if(!okstudentsmaxhourscontinuously){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from students activity tag max hours daily
		
		//!!!NOT PERFECT, there is room for improvement
		
		okstudentsactivitytagmaxhoursdaily=true;
		
		if(haveStudentsActivityTagMaxHoursDaily){
	
			for(int sbg : qAsConst(act->iSubgroupsList)){
				for(int cnt=0; cnt<subgroupsActivityTagMaxHoursDailyMaxHours[sbg].count(); cnt++){
					int activityTag=subgroupsActivityTagMaxHoursDailyActivityTag[sbg].at(cnt);
				
					if(!gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(activityTag))
						continue;

					int limitHoursDaily=subgroupsActivityTagMaxHoursDailyMaxHours[sbg].at(cnt);
					double percentage=subgroupsActivityTagMaxHoursDailyPercentage[sbg].at(cnt);

					assert(limitHoursDaily>=0);
					assert(percentage>=0);
					assert(activityTag>=0 /*&& activityTag<gt.rules.nInternalActivityTags*/);

					//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
					//	continue;
				
					bool increased;
					
					int nold=0, nnew=0;
					///////////
					for(int h2=0; h2<h; h2++){
						if(newSubgroupsTimetable(sbg,d,h2)>=0){
							int ai2=newSubgroupsTimetable(sbg,d,h2);
							assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
							Activity* act=&gt.rules.internalActivitiesList[ai2];
							if(act->iActivityTagsSet.contains(activityTag)){
								nold++;
								nnew++;
							}
						}
					}
					for(int h2=h; h2<h+act->duration; h2++){
						if(oldSubgroupsTimetable(sbg,d,h2)>=0){
							int ai2=oldSubgroupsTimetable(sbg,d,h2);
							assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
							Activity* act=&gt.rules.internalActivitiesList[ai2];
							if(act->iActivityTagsSet.contains(activityTag))
								nold++;
						}
					}
					for(int h2=h; h2<h+act->duration; h2++){
						if(newSubgroupsTimetable(sbg,d,h2)>=0){
							int ai2=newSubgroupsTimetable(sbg,d,h2);
							assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
							Activity* act=&gt.rules.internalActivitiesList[ai2];
							if(act->iActivityTagsSet.contains(activityTag))
								nnew++;
						}
					}
					for(int h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
						if(newSubgroupsTimetable(sbg,d,h2)>=0){
							int ai2=newSubgroupsTimetable(sbg,d,h2);
							assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
							Activity* act=&gt.rules.internalActivitiesList[ai2];
							if(act->iActivityTagsSet.contains(activityTag)){
								nold++;
								nnew++;
							}
						}
					}
					/////////
					if(nold<nnew)
						increased=true;
					else
						increased=false;
						
					if(percentage==100.0)
						assert(nold<=limitHoursDaily);
					if(!increased && percentage==100.0)
						assert(nnew<=limitHoursDaily);
					
					if(!increased || nnew<=limitHoursDaily) //OK
						continue;
						
					assert(limitHoursDaily>=0);
	
					assert(increased);
					assert(nnew>limitHoursDaily);
					if(!skipRandom(percentage)){
						if(act->duration>limitHoursDaily){
							okstudentsactivitytagmaxhoursdaily=false;
							goto impossiblestudentsactivitytagmaxhoursdaily;
						}
					
						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okstudentsactivitytagmaxhoursdaily=false;
							goto impossiblestudentsactivitytagmaxhoursdaily;
						}
					
						getSbgTimetable(sbg, conflActivities[newtime]);
						sbgGetNHoursGaps(sbg);
	
						while(true){
							int ncrt=0;
							for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
								if(sbgTimetable(d,h2)>=0){
									int ai2=sbgTimetable(d,h2);
									assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
									Activity* act=&gt.rules.internalActivitiesList[ai2];
									if(act->iActivityTagsSet.contains(activityTag))
										ncrt++;
								}
							}
							
							if(ncrt<=limitHoursDaily)
								break;
						
							int ai2=-1;
							
							bool ke=subgroupRemoveAnActivityFromAnywhereCertainDayCertainActivityTag(d, activityTag, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							
							if(!ke){
								if(level==0){
									//...this is not too good, but hopefully there is no problem
								}
								okstudentsactivitytagmaxhoursdaily=false;
								goto impossiblestudentsactivitytagmaxhoursdaily;
							}
							
							assert(ai2>=0);
							
							assert(gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag));
							
							removeAi2FromSbgTimetable(ai2);
							updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
			
		}
		
impossiblestudentsactivitytagmaxhoursdaily:
		if(!okstudentsactivitytagmaxhoursdaily){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from students activity tag max hours daily per real day

		//!!!NOT PERFECT, there is room for improvement

		okstudentsactivitytagmaxhoursdailyrealdays=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(haveStudentsActivityTagMaxHoursDailyRealDays){
				for(int sbg : qAsConst(act->iSubgroupsList)){
					for(int cnt=0; cnt<subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sbg].count(); cnt++){
						int activityTag=subgroupsActivityTagMaxHoursDailyRealDaysActivityTag[sbg].at(cnt);

						if(!gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(activityTag))
							continue;

						int limitHoursDaily=subgroupsActivityTagMaxHoursDailyRealDaysMaxHours[sbg].at(cnt);
						double percentage=subgroupsActivityTagMaxHoursDailyRealDaysPercentage[sbg].at(cnt);

						assert(limitHoursDaily>=0);
						assert(percentage>=0);
						assert(activityTag>=0 /*&& activityTag<gt.rules.nInternalActivityTags*/);

						//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
						//	continue;

						bool increased;

						int nold=0, nnew=0;
						///////////
						for(int h2=0; h2<h; h2++){
							if(newSubgroupsTimetable(sbg,d,h2)>=0){
								int ai2=newSubgroupsTimetable(sbg,d,h2);
								assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
								Activity* act=&gt.rules.internalActivitiesList[ai2];
								if(act->iActivityTagsSet.contains(activityTag)){
									nold++;
									nnew++;
								}
							}
						}
						for(int h2=h; h2<h+act->duration; h2++){
							if(oldSubgroupsTimetable(sbg,d,h2)>=0){
								int ai2=oldSubgroupsTimetable(sbg,d,h2);
								assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
								Activity* act=&gt.rules.internalActivitiesList[ai2];
								if(act->iActivityTagsSet.contains(activityTag))
									nold++;
							}
						}
						for(int h2=h; h2<h+act->duration; h2++){
							if(newSubgroupsTimetable(sbg,d,h2)>=0){
								int ai2=newSubgroupsTimetable(sbg,d,h2);
								assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
								Activity* act=&gt.rules.internalActivitiesList[ai2];
								if(act->iActivityTagsSet.contains(activityTag))
									nnew++;
							}
						}
						for(int h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
							if(newSubgroupsTimetable(sbg,d,h2)>=0){
								int ai2=newSubgroupsTimetable(sbg,d,h2);
								assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
								Activity* act=&gt.rules.internalActivitiesList[ai2];
								if(act->iActivityTagsSet.contains(activityTag)){
									nold++;
									nnew++;
								}
							}
						}
						for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
							if(newSubgroupsTimetable(sbg,dpair,h2)>=0){
								int ai2=newSubgroupsTimetable(sbg,dpair,h2);
								assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
								Activity* act=&gt.rules.internalActivitiesList[ai2];
								if(act->iActivityTagsSet.contains(activityTag)){
									nold++;
									nnew++;
								}
							}
						}
						/////////
						if(nold<nnew)
							increased=true;
						else
							increased=false;

						if(percentage==100.0)
							assert(nold<=limitHoursDaily);
						if(!increased && percentage==100.0)
							assert(nnew<=limitHoursDaily);

						if(!increased || nnew<=limitHoursDaily) //OK
							continue;

						assert(limitHoursDaily>=0);

						assert(increased);
						assert(nnew>limitHoursDaily);
						if(!skipRandom(percentage)){
							if(act->duration>limitHoursDaily){
								okstudentsactivitytagmaxhoursdailyrealdays=false;
								goto impossiblestudentsactivitytagmaxhoursdailyrealdays;
							}

							if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
								okstudentsactivitytagmaxhoursdailyrealdays=false;
								goto impossiblestudentsactivitytagmaxhoursdailyrealdays;
							}

							getSbgTimetable(sbg, conflActivities[newtime]);
							sbgGetNHoursGaps(sbg);

							while(true){
								int ncrt=0;
								for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
									if(sbgTimetable(d,h2)>=0){
										int ai2=sbgTimetable(d,h2);
										assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
										Activity* act=&gt.rules.internalActivitiesList[ai2];
										if(act->iActivityTagsSet.contains(activityTag))
											ncrt++;
									}
								}
								for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
									if(sbgTimetable(dpair,h2)>=0){
										int ai2=sbgTimetable(dpair,h2);
										assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
										Activity* act=&gt.rules.internalActivitiesList[ai2];
										if(act->iActivityTagsSet.contains(activityTag))
											ncrt++;
									}
								}

								if(ncrt<=limitHoursDaily)
									break;

								int ai2=-1;

								bool ke=subgroupRemoveAnActivityFromAnywhereCertainDayDayPairCertainActivityTag(d, dpair, activityTag, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								if(!ke){
									if(level==0){
										//...this is not too good, but hopefully there is no problem
									}
									okstudentsactivitytagmaxhoursdailyrealdays=false;
									goto impossiblestudentsactivitytagmaxhoursdailyrealdays;
								}

								assert(ai2>=0);

								assert(gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag));

								removeAi2FromSbgTimetable(ai2);
								updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
							}
						}
					}
				}
			}
		}

impossiblestudentsactivitytagmaxhoursdailyrealdays:
		if(!okstudentsactivitytagmaxhoursdailyrealdays){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from students activity tag max hours continuously
		
		okstudentsactivitytagmaxhourscontinuously=true;
		
		if(haveStudentsActivityTagMaxHoursContinuously){
	
			for(int sbg : qAsConst(act->iSubgroupsList)){
				for(int cnt=0; cnt<subgroupsActivityTagMaxHoursContinuouslyMaxHours[sbg].count(); cnt++){
					int activityTag=subgroupsActivityTagMaxHoursContinuouslyActivityTag[sbg].at(cnt);
				
					//if(gt.rules.internalActivitiesList[ai].activityTagIndex!=activityTag)
					//	continue;
					if(!gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(activityTag))
						continue;

					int limitHoursCont=subgroupsActivityTagMaxHoursContinuouslyMaxHours[sbg].at(cnt);
					double percentage=subgroupsActivityTagMaxHoursContinuouslyPercentage[sbg].at(cnt);

					assert(limitHoursCont>=0);
					assert(percentage>=0);
					assert(activityTag>=0 /*&& activityTag<gt.rules.nInternalActivityTags*/);

					//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
					//	continue;
				
					bool increased;
					int h2;
					for(h2=h; h2<h+act->duration; h2++){
						assert(h2<gt.rules.nHoursPerDay);
						if(subgroupsTimetable(sbg,d,h2)==-1)
							break;
						int ai2=subgroupsTimetable(sbg,d,h2);
						assert(ai2>=0);
						if(!gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag))
							break;
					}
					if(h2<h+act->duration)
						increased=true;
					else
						increased=false;
					
					QList<int> removableActs;
					
					int nc=act->duration;
					for(h2=h-1; h2>=0; h2--){
						int ai2=subgroupsTimetable(sbg,d,h2);
						assert(ai2==newSubgroupsTimetable(sbg,d,h2));
						assert(ai2!=ai);
						if(ai2<0)
							break;
						if(ai2>=0 && !conflActivities[newtime].contains(ai2) &&
						 gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag)){
							nc++;
							
							if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
								removableActs.append(ai2);
						}
						else
							break;
					}
					for(h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
						int ai2=subgroupsTimetable(sbg,d,h2);
						assert(ai2==newSubgroupsTimetable(sbg,d,h2));
						assert(ai2!=ai);
						if(ai2<0)
							break;
						if(ai2>=0 && !conflActivities[newtime].contains(ai2) &&
						 gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag)){
							nc++;
							
							if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
								removableActs.append(ai2);
						}
						else
							break;
					}
					
					if(!increased && percentage==100.0)
						assert(nc<=limitHoursCont);
					
					if(!increased || nc<=limitHoursCont) //OK
						continue;
					
					assert(limitHoursCont>=0);
	
					if(!skipRandom(percentage) && increased){
						if(act->duration>limitHoursCont){
							okstudentsactivitytagmaxhourscontinuously=false;
							goto impossiblestudentsactivitytagmaxhourscontinuously;
						}
					
						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okstudentsactivitytagmaxhourscontinuously=false;
							goto impossiblestudentsactivitytagmaxhourscontinuously;
						}
					
						while(true){
							if(removableActs.count()==0){
								okstudentsactivitytagmaxhourscontinuously=false;
								goto impossiblestudentsactivitytagmaxhourscontinuously;
							}
							
							int j=-1;
						
							if(level==0){
								int optMinWrong=INF;
				
								QList<int> tl;
	
								for(int q=0; q<removableActs.count(); q++){
									int ai2=removableActs.at(q);
									if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
									 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
									}
								}
				
								for(int q=0; q<removableActs.count(); q++){
									int ai2=removableActs.at(q);
									if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
										tl.append(q);
								}
				
								assert(tl.count()>=1);
								j=tl.at(rng.intMRG32k3a(tl.count()));
				
								assert(j>=0 && j<removableActs.count());
							}
							else{
								j=rng.intMRG32k3a(removableActs.count());
							}
						
							assert(j>=0);
							
							int ai2=removableActs.at(j);
							
							int t=removableActs.removeAll(ai2);
							assert(t==1);
						
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							
							////////////
							removableActs.clear();
							
							int nc=act->duration;
							int h2;
							for(h2=h-1; h2>=0; h2--){
								int ai2=subgroupsTimetable(sbg,d,h2);
								assert(ai2==newSubgroupsTimetable(sbg,d,h2));
								assert(ai2!=ai);
								if(ai2<0)
									break;
								if(ai2>=0 && !conflActivities[newtime].contains(ai2) &&
								 gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag)){
									nc++;
								
									if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
										removableActs.append(ai2);
								}
								else
									break;
							}
							for(h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
								int ai2=subgroupsTimetable(sbg,d,h2);
								assert(ai2==newSubgroupsTimetable(sbg,d,h2));
								assert(ai2!=ai);
								if(ai2<0)
									break;
								if(ai2>=0 && !conflActivities[newtime].contains(ai2) &&
								 //gt.rules.internalActivitiesList[ai2].activityTagIndex==activityTag){
								 gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag)){
									nc++;
							
									if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
										removableActs.append(ai2);
								}
								else
									break;
							}
							
							if(nc<=limitHoursCont) //OK
								break;
							////////////
						}
					}
				}
			}
			
		}
		
impossiblestudentsactivitytagmaxhourscontinuously:
		if(!okstudentsactivitytagmaxhourscontinuously){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		/////////begin students min hours daily
		
		//TODO: this should take care of students max gaps per day also. Very critical changes, so be very careful if you do them. Safer -> leave them as they are now.
		//see file fet-v.v.v/doc/algorithm/improve-studentsmaxgapsperday.txt for advice and (unstable) code on how to make students max gaps per day constraint perfect
		okstudentsminhoursdaily=true;
		
		if(gt.rules.mode!=MORNINGS_AFTERNOONS){
			for(int sbg : qAsConst(act->iSubgroupsList)){
				if(subgroupsMinHoursDailyMinHours[sbg][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0){
					assert(subgroupsMinHoursDailyPercentages[sbg][MIN_HOURS_DAILY_INDEX_IN_ARRAY]==100);

					bool allowEmptyDays=subgroupsMinHoursDailyAllowEmptyDays[sbg];
					int minLimitSbg=subgroupsMinHoursDailyMinHours[sbg][MIN_HOURS_DAILY_INDEX_IN_ARRAY];

					bool skip=skipRandom(subgroupsMinHoursDailyPercentages[sbg][MIN_HOURS_DAILY_INDEX_IN_ARRAY]);
					if(!skip){
						//preliminary test
						bool _ok;
						if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
							if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
								//both limitations
								int remG=0, totalH=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									int remGDay=newSubgroupsDayNFirstGaps(sbg,d2)+newSubgroupsDayNGaps(sbg,d2);
									if(/*1*/ !allowEmptyDays || newSubgroupsDayNHours(sbg,d2)>0){
										if(newSubgroupsDayNHours(sbg,d2)<minLimitSbg){
											remGDay-=minLimitSbg-newSubgroupsDayNHours(sbg,d2);
											totalH+=minLimitSbg;
										}
										else
											totalH+=newSubgroupsDayNHours(sbg,d2);
									}
									if(remGDay>0)
										remG+=remGDay;
								}
								if((remG+totalH <= nHoursPerSubgroup[sbg]
								  +subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]+subgroupsMaxGapsPerWeekMaxGaps[sbg])
								  && (totalH <= nHoursPerSubgroup[sbg]))
									_ok=true;
								else
									_ok=false;
							}
							else{
								//only first gaps limitation
								int remG=0, totalH=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									int remGDay=0;
									if(/*1*/ !allowEmptyDays || newSubgroupsDayNHours(sbg,d2)>0){
										if(newSubgroupsDayNHours(sbg,d2)<minLimitSbg){
											remGDay=0;
											totalH+=minLimitSbg;
										}
										else{
											totalH+=newSubgroupsDayNHours(sbg,d2);
											if(newSubgroupsDayNFirstGaps(sbg,d2)==0)
												remGDay=0;
											else if(newSubgroupsDayNFirstGaps(sbg,d2)==1)
												remGDay=1;
											else if(newSubgroupsDayNFirstGaps(sbg,d2)>=2){
												remGDay=0;
												totalH++;
											}
										}
									}
									if(remGDay>0)
										remG+=remGDay;
								}
								if((remG+totalH <= nHoursPerSubgroup[sbg]+subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg])
								  && (totalH <= nHoursPerSubgroup[sbg]))
									_ok=true;
								else
									_ok=false;
							}
						}
						else{
							if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
								//only max gaps per week limitation
								int remG=0, totalH=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									int remGDay=newSubgroupsDayNGaps(sbg,d2);
									if(/*1*/ !allowEmptyDays || newSubgroupsDayNHours(sbg,d2)>0){
										if(newSubgroupsDayNHours(sbg,d2)<minLimitSbg){
											remGDay-=minLimitSbg-newSubgroupsDayNHours(sbg,d2);
											totalH+=minLimitSbg;
										}
										else
											totalH+=newSubgroupsDayNHours(sbg,d2);
									}
									if(remGDay>0)
										remG+=remGDay;
								}
								if((remG+totalH <= nHoursPerSubgroup[sbg]+subgroupsMaxGapsPerWeekMaxGaps[sbg])
								  && (totalH <= nHoursPerSubgroup[sbg]))
									_ok=true;
								else
									_ok=false;
							}
							else{
								//no limitation
								int totalH=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(/*1*/ !allowEmptyDays || newSubgroupsDayNHours(sbg,d2)>0){
										if(newSubgroupsDayNHours(sbg,d2)<minLimitSbg)
											totalH+=minLimitSbg;
										else
											totalH+=newSubgroupsDayNHours(sbg,d2);
									}
								}
								if(totalH <= nHoursPerSubgroup[sbg])
									_ok=true;
								else
									_ok=false;
							}
						}

						if(_ok)
							continue;

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okstudentsminhoursdaily=false;
							goto impossiblestudentsminhoursdaily;
						}

						getSbgTimetable(sbg, conflActivities[newtime]);
						sbgGetNHoursGaps(sbg);

						for(;;){
							bool ok;
							////////////////////////////
							if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
								if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
									//both limitations
									int remG=0, totalH=0;
									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										int remGDay=sbgDayNFirstGaps[d2]+sbgDayNGaps[d2];
										if(/*1*/ !allowEmptyDays || sbgDayNHours[d2]>0){
											if(sbgDayNHours[d2]<minLimitSbg){
												remGDay-=minLimitSbg-sbgDayNHours[d2];
												totalH+=minLimitSbg;
											}
											else
												totalH+=sbgDayNHours[d2];
										}
										if(remGDay>0)
											remG+=remGDay;
									}
									if((remG+totalH <= nHoursPerSubgroup[sbg]
									  +subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]+subgroupsMaxGapsPerWeekMaxGaps[sbg])
									  && (totalH <= nHoursPerSubgroup[sbg]))
										ok=true;
									else
										ok=false;
								}
								else{
									//only first gaps limitation
									int remG=0, totalH=0;
									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										int remGDay=0;
										if(/*1*/ !allowEmptyDays || sbgDayNHours[d2]>0){
											if(sbgDayNHours[d2]<minLimitSbg){
												remGDay=0;
												totalH+=minLimitSbg;
											}
											else{
												totalH+=sbgDayNHours[d2];
												if(sbgDayNFirstGaps[d2]==0)
													remGDay=0;
												else if(sbgDayNFirstGaps[d2]==1)
													remGDay=1;
												else if(sbgDayNFirstGaps[d2]>=2){
													remGDay=0;
													totalH++;
												}
											}
										}
										if(remGDay>0)
											remG+=remGDay;
									}
									if((remG+totalH <= nHoursPerSubgroup[sbg]+subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg])
									  && (totalH <= nHoursPerSubgroup[sbg]))
										ok=true;
									else
										ok=false;
								}
							}
							else{
								if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
									//only max gaps per week limitation
									int remG=0, totalH=0;
									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										int remGDay=sbgDayNGaps[d2];
										if(/*1*/ !allowEmptyDays || sbgDayNHours[d2]>0){
											if(sbgDayNHours[d2]<minLimitSbg){
												remGDay-=minLimitSbg-sbgDayNHours[d2];
												totalH+=minLimitSbg;
											}
											else
												totalH+=sbgDayNHours[d2];
										}
										if(remGDay>0)
											remG+=remGDay;
									}
									if((remG+totalH <= nHoursPerSubgroup[sbg]+subgroupsMaxGapsPerWeekMaxGaps[sbg])
									  && (totalH <= nHoursPerSubgroup[sbg]))
										ok=true;
									else
										ok=false;
								}
								else{
									//no limitation
									int totalH=0;
									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										if(/*1*/ !allowEmptyDays || sbgDayNHours[d2]>0){
											if(sbgDayNHours[d2]<minLimitSbg)
												totalH+=minLimitSbg;
											else
												totalH+=sbgDayNHours[d2];
										}
									}
									if(totalH <= nHoursPerSubgroup[sbg])
										ok=true;
									else
										ok=false;
								}
							}
							////////////////////////////

							if(ok)
								break; //ok

							int ai2=-1;

							bool kk=subgroupRemoveAnActivityFromEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!kk){
								bool k=subgroupRemoveAnActivityFromBegin(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k){
									bool ka=subgroupRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);

									if(!ka){
										if(level==0){
											//this should not be displayed
											//cout<<"WARNING - unlikely situation - file "<<__FILE__<<" line "<<__LINE__<<endl;
										}
										okstudentsminhoursdaily=false;
										goto impossiblestudentsminhoursdaily;
									}
								}
							}

							assert(ai2>=0);

							removeAi2FromSbgTimetable(ai2);
							updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
		}
		else{
			for(int sbg : qAsConst(act->iSubgroupsList)){
				if(subgroupsMinHoursDailyMinHours[sbg][1]>=0){
					assert(subgroupsMinHoursDailyPercentages[sbg][1]==100);

					bool allowEmptyDays=subgroupsMinHoursDailyAllowEmptyDays[sbg];
					bool allowEmptyMornings=subgroupsMinHoursPerMorningAllowEmptyMornings[sbg];

					bool allowEmptyAfternoons=subgroupsMinHoursPerAfternoonAllowEmptyAfternoons[sbg];

					int mhd=1; //min hours per day
					if(subgroupsMinHoursDailyMinHours[sbg][1]>=0)
						mhd=subgroupsMinHoursDailyMinHours[sbg][1];
					assert(mhd>=1);
					int mhm=mhd; //min hours per morning
					if(subgroupsMinHoursDailyMinHours[sbg][0]>=0)
						mhm=subgroupsMinHoursDailyMinHours[sbg][0];
					assert(mhm>=mhd);

					int mhaft=mhd; //min hours per afternoon
					if(subgroupsMinHoursPerAfternoonMinHours[sbg]>=0)
						mhaft=subgroupsMinHoursPerAfternoonMinHours[sbg];
					assert(mhaft>=mhd);

					bool skip=skipRandom(subgroupsMinHoursDailyPercentages[sbg][1]);
					if(!skip){
						//preliminary test
						bool _ok=true;
						if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
							if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
								//both limitations
								//if(allowEmptyDays){
								if(1){
									int remG=0, totalH=0;

									int nUsedMornings=0;
									int nUsedAfternoons=0;

									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										int remGDay=newSubgroupsDayNFirstGaps(sbg,d2)+newSubgroupsDayNGaps(sbg,d2);
										if(/*1*/ /*!allowEmptyDays ||*/ newSubgroupsDayNHours(sbg,d2)>0 || (!allowEmptyMornings && (d2%2)==0) || (!allowEmptyAfternoons && (d2%2)==1)){
											int tm=-1;
											if(d2%2==0)
												tm=mhm;
											else
												tm=mhaft;
											assert(tm>=1);
											if(newSubgroupsDayNHours(sbg,d2)</*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm){
												remGDay-=/*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm-newSubgroupsDayNHours(sbg,d2);
												totalH+=/*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm;
											}
											else
												totalH+=newSubgroupsDayNHours(sbg,d2);
										}
										if(remGDay>0)
											remG+=remGDay;

										if(newSubgroupsDayNHours(sbg,d2)>0){
											if(d2%2==0)
												nUsedMornings++;
											else
												nUsedAfternoons++;
										}
									}

									if(subgroupsMinMorningsPerWeekMinMornings[sbg]>=0)
										if(subgroupsMinMorningsPerWeekMinMornings[sbg]>nUsedMornings)
											totalH+=(subgroupsMinMorningsPerWeekMinMornings[sbg]-nUsedMornings)*mhm;

									if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>=0)
										if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>nUsedAfternoons)
											totalH+=(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]-nUsedAfternoons)*mhaft;

									if((remG+totalH <= nHoursPerSubgroup[sbg]
									  +subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]+subgroupsMaxGapsPerWeekMaxGaps[sbg])
									  && (totalH <= nHoursPerSubgroup[sbg]))
										;//_ok=true;
									else
										_ok=false;
								}
								if(!allowEmptyDays){
									//real days
									int remG=0, totalH=0;

									for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
										int remGDay=newSubgroupsDayNFirstGaps(sbg,2*d2)+newSubgroupsDayNGaps(sbg,2*d2)
										 + newSubgroupsDayNFirstGaps(sbg,2*d2+1)+newSubgroupsDayNGaps(sbg,2*d2+1);
										if(/*1*/ !allowEmptyDays || newSubgroupsDayNHours(sbg,2*d2)+newSubgroupsDayNHours(sbg,2*d2+1)>0){
											if(newSubgroupsDayNHours(sbg,2*d2)+newSubgroupsDayNHours(sbg,2*d2+1) < mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/){
												remGDay-=mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/-(newSubgroupsDayNHours(sbg,2*d2)+newSubgroupsDayNHours(sbg,2*d2+1));
												totalH+=mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/;
											}
											else
												totalH+=newSubgroupsDayNHours(sbg,2*d2)+newSubgroupsDayNHours(sbg,2*d2+1);
										}
										if(remGDay>0)
											remG+=remGDay;
									}
									if((remG+totalH <= nHoursPerSubgroup[sbg]
									  +subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]+subgroupsMaxGapsPerWeekMaxGaps[sbg])
									  && (totalH <= nHoursPerSubgroup[sbg]))
										;//_ok=true;
									else
										_ok=false;
								}
							}
							else{
								//only first gaps limitation
								//if(allowEmptyDays){
								if(1){
									int remG=0, totalH=0;

									int nUsedMornings=0;
									int nUsedAfternoons=0;

									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										int remGDay=0;
										if(/*1*/ /*!allowEmptyDays ||*/ newSubgroupsDayNHours(sbg,d2)>0 || (!allowEmptyMornings && (d2%2)==0) || (!allowEmptyAfternoons && (d2%2)==1)){
											int tm=-1;
											if(d2%2==0)
												tm=mhm;
											else
												tm=mhaft;
											assert(tm>=1);
											if(newSubgroupsDayNHours(sbg,d2)</*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm){
												remGDay=0;
												totalH+=/*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm;
											}
											else{
												totalH+=newSubgroupsDayNHours(sbg,d2);
												if(newSubgroupsDayNFirstGaps(sbg,d2)==0)
													remGDay=0;
												else if(newSubgroupsDayNFirstGaps(sbg,d2)==1)
													remGDay=1;
												else if(newSubgroupsDayNFirstGaps(sbg,d2)>=2){
													remGDay=0;
													totalH++;
												}
											}
										}
										if(remGDay>0)
											remG+=remGDay;

										if(newSubgroupsDayNHours(sbg,d2)>0){
											if(d2%2==0)
												nUsedMornings++;
											else
												nUsedAfternoons++;
										}
									}
									if(subgroupsMinMorningsPerWeekMinMornings[sbg]>=0)
										if(subgroupsMinMorningsPerWeekMinMornings[sbg]>nUsedMornings)
											totalH+=(subgroupsMinMorningsPerWeekMinMornings[sbg]-nUsedMornings)*mhm;

									if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>=0)
										if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>nUsedAfternoons)
											totalH+=(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]-nUsedAfternoons)*mhaft;

									if((remG+totalH <= nHoursPerSubgroup[sbg]+subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg])
									  && (totalH <= nHoursPerSubgroup[sbg]))
										;//_ok=true;
									else
										_ok=false;
								}
								if(!allowEmptyDays){
									//real days
									int remG=0, totalH=0;
									for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
										int remGDay=0;
										if(/*1*/ !allowEmptyDays || newSubgroupsDayNHours(sbg,2*d2)+newSubgroupsDayNHours(sbg,2*d2+1)>0){
											if(newSubgroupsDayNHours(sbg,2*d2)+newSubgroupsDayNHours(sbg,2*d2+1)<mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/){
												remGDay=0;
												totalH+=mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/;
											}
											else{
												totalH+=newSubgroupsDayNHours(sbg,2*d2)+newSubgroupsDayNHours(sbg,2*d2+1);
												remGDay=0;
												if(newSubgroupsDayNFirstGaps(sbg,2*d2)==0)
													remGDay+=0;
												else if(newSubgroupsDayNFirstGaps(sbg,2*d2)==1)
													remGDay+=1;
												else if(newSubgroupsDayNFirstGaps(sbg,2*d2)>=2){
													remGDay+=0;
													totalH++;
												}

												if(newSubgroupsDayNFirstGaps(sbg,2*d2+1)==0)
													remGDay+=0;
												else if(newSubgroupsDayNFirstGaps(sbg,2*d2+1)==1)
													remGDay+=1;
												else if(newSubgroupsDayNFirstGaps(sbg,2*d2+1)>=2){
													remGDay+=0;
													totalH++;
												}
											}
										}
										if(remGDay>0)
											remG+=remGDay;
									}
									if((remG+totalH <= nHoursPerSubgroup[sbg]+subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg])
									  && (totalH <= nHoursPerSubgroup[sbg]))
										;//_ok=true;
									else
										_ok=false;
								}
							}
						}
						else{
							if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
								//only max gaps per week limitation
								//if(allowEmptyDays){
								if(1){
									int remG=0, totalH=0;

									int nUsedMornings=0;
									int nUsedAfternoons=0;

									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										int remGDay=newSubgroupsDayNGaps(sbg,d2);
										if(/*1*/ /*!allowEmptyDays ||*/ newSubgroupsDayNHours(sbg,d2)>0 || (!allowEmptyMornings && (d2%2)==0) || (!allowEmptyAfternoons && (d2%2)==1)){
											int tm=-1;
											if(d2%2==0)
												tm=mhm;
											else
												tm=mhaft;
											assert(tm>=1);
											if(newSubgroupsDayNHours(sbg,d2)</*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm){
												remGDay-=/*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm-newSubgroupsDayNHours(sbg,d2);
												totalH+=/*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm;
											}
											else
												totalH+=newSubgroupsDayNHours(sbg,d2);
										}
										if(remGDay>0)
											remG+=remGDay;

										if(newSubgroupsDayNHours(sbg,d2)>0){
											if(d2%2==0)
												nUsedMornings++;
											else
												nUsedAfternoons++;
										}
									}
									if(subgroupsMinMorningsPerWeekMinMornings[sbg]>=0)
										if(subgroupsMinMorningsPerWeekMinMornings[sbg]>nUsedMornings)
											totalH+=(subgroupsMinMorningsPerWeekMinMornings[sbg]-nUsedMornings)*mhm;

									if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>=0)
										if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>nUsedAfternoons)
											totalH+=(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]-nUsedAfternoons)*mhaft;

									if((remG+totalH <= nHoursPerSubgroup[sbg]+subgroupsMaxGapsPerWeekMaxGaps[sbg])
									  && (totalH <= nHoursPerSubgroup[sbg]))
										;//_ok=true;
									else
										_ok=false;
								}
								if(!allowEmptyDays){
									//real days
									int remG=0, totalH=0;
									for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
										int remGDay=newSubgroupsDayNGaps(sbg,2*d2)+newSubgroupsDayNGaps(sbg,2*d2+1);
										if(/*1*/ !allowEmptyDays || newSubgroupsDayNHours(sbg,2*d2)+newSubgroupsDayNHours(sbg,2*d2+1)>0){
											if(newSubgroupsDayNHours(sbg,2*d2)+newSubgroupsDayNHours(sbg,2*d2+1)<mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/){
												remGDay-=mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/-newSubgroupsDayNHours(sbg,2*d2)-newSubgroupsDayNHours(sbg,2*d2+1);
												totalH+=mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/;
											}
											else
												totalH+=newSubgroupsDayNHours(sbg,2*d2)+newSubgroupsDayNHours(sbg,2*d2+1);
										}
										if(remGDay>0)
											remG+=remGDay;
									}
									if((remG+totalH <= nHoursPerSubgroup[sbg]+subgroupsMaxGapsPerWeekMaxGaps[sbg])
									  && (totalH <= nHoursPerSubgroup[sbg]))
										;//_ok=true;
									else
										_ok=false;
								}
							}
							else{
								//no limitation
								//if(allowEmptyDays){
								if(1){
									int totalH=0;

									int nUsedMornings=0;
									int nUsedAfternoons=0;

									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										if(/*1*/ /*!allowEmptyDays ||*/ newSubgroupsDayNHours(sbg,d2)>0 || (!allowEmptyMornings && (d2%2)==0) || (!allowEmptyAfternoons && (d2%2)==1)){
											int tm=-1;
											if(d2%2==0)
												tm=mhm;
											else
												tm=mhaft;
											assert(tm>=1);
											if(newSubgroupsDayNHours(sbg,d2)</*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm)
												totalH+=/*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm;
											else
												totalH+=newSubgroupsDayNHours(sbg,d2);
										}

										if(newSubgroupsDayNHours(sbg,d2)>0){
											if(d2%2==0)
												nUsedMornings++;
											else
												nUsedAfternoons++;
										}
									}
									if(subgroupsMinMorningsPerWeekMinMornings[sbg]>=0)
										if(subgroupsMinMorningsPerWeekMinMornings[sbg]>nUsedMornings)
											totalH+=(subgroupsMinMorningsPerWeekMinMornings[sbg]-nUsedMornings)*mhm;

									if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>=0)
										if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>nUsedAfternoons)
											totalH+=(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]-nUsedAfternoons)*mhaft;

									if(totalH <= nHoursPerSubgroup[sbg])
										;//_ok=true;
									else
										_ok=false;

									//cout<<"totalH=="<<totalH<<", nHoursPerSubgroup[sbg]=="<<nHoursPerSubgroup[sbg]<<", _ok=="<<_ok<<endl;
								}
								if(!allowEmptyDays){
									//real days
									int totalH=0;
									for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
										if(/*1*/ !allowEmptyDays || newSubgroupsDayNHours(sbg,2*d2)+newSubgroupsDayNHours(sbg,2*d2+1)>0){
											if(newSubgroupsDayNHours(sbg,2*d2)+newSubgroupsDayNHours(sbg,2*d2+1) < mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/)
												totalH+=mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/;
											else
												totalH+=newSubgroupsDayNHours(sbg,2*d2)+newSubgroupsDayNHours(sbg,2*d2+1);
										}
									}
									if(totalH <= nHoursPerSubgroup[sbg])
										;//_ok=true;
									else
										_ok=false;

									//cout<<"totalH=="<<totalH<<", nHoursPerSubgroup[sbg]=="<<nHoursPerSubgroup[sbg]<<", _ok=="<<_ok<<endl;
								}
							}
						}

						if(_ok)
							continue;

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okstudentsminhoursdaily=false;
							goto impossiblestudentsminhoursdaily;
						}

						getSbgTimetable(sbg, conflActivities[newtime]);
						sbgGetNHoursGaps(sbg);

						for(;;){
							bool ok=true;
							////////////////////////////
							if(subgroupsEarlyMaxBeginningsAtSecondHourPercentage[sbg]>=0){
								if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
									//both limitations
									//if(allowEmptyDays){
									if(1){
										int remG=0, totalH=0;

										int nUsedMornings=0;
										int nUsedAfternoons=0;

										for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
											int remGDay=sbgDayNFirstGaps[d2]+sbgDayNGaps[d2];
											if(/*1*/ /*!allowEmptyDays ||*/ sbgDayNHours[d2]>0 || (!allowEmptyMornings && (d2%2)==0) || (!allowEmptyAfternoons && (d2%2)==1)){
												int tm=-1;
												if(d2%2==0)
													tm=mhm;
												else
													tm=mhaft;
												assert(tm>=1);
												if(sbgDayNHours[d2]</*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm){
													remGDay-=/*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm-sbgDayNHours[d2];
													totalH+=/*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm;
												}
												else
													totalH+=sbgDayNHours[d2];
											}
											if(remGDay>0)
												remG+=remGDay;

											if(sbgDayNHours[d2]>0){
												if(d2%2==0)
													nUsedMornings++;
												else
													nUsedAfternoons++;
											}
										}

										if(subgroupsMinMorningsPerWeekMinMornings[sbg]>=0)
											if(subgroupsMinMorningsPerWeekMinMornings[sbg]>nUsedMornings)
												totalH+=(subgroupsMinMorningsPerWeekMinMornings[sbg]-nUsedMornings)*mhm;

										if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>=0)
											if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>nUsedAfternoons)
												totalH+=(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]-nUsedAfternoons)*mhaft;

										if((remG+totalH <= nHoursPerSubgroup[sbg]
										  +subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]+subgroupsMaxGapsPerWeekMaxGaps[sbg])
										  && (totalH <= nHoursPerSubgroup[sbg]))
											;//ok=true;
										else
											ok=false;
									}
									if(!allowEmptyDays){
										//real days
										int remG=0, totalH=0;
										for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
											int remGDay=sbgDayNFirstGaps[2*d2]+sbgDayNFirstGaps[2*d2+1]+sbgDayNGaps[2*d2]+sbgDayNGaps[2*d2+1];
											if(/*1*/ !allowEmptyDays || sbgDayNHours[2*d2]+sbgDayNHours[2*d2+1]>0){
												if(sbgDayNHours[2*d2]+sbgDayNHours[2*d2+1]<mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/){
													remGDay-=mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/-sbgDayNHours[2*d2]-sbgDayNHours[2*d2+1];
													totalH+=mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/;
												}
												else
													totalH+=sbgDayNHours[2*d2]+sbgDayNHours[2*d2+1];
											}
											if(remGDay>0)
												remG+=remGDay;
										}
										if((remG+totalH <= nHoursPerSubgroup[sbg]
										  +subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]+subgroupsMaxGapsPerWeekMaxGaps[sbg])
										  && (totalH <= nHoursPerSubgroup[sbg]))
											;//ok=true;
										else
											ok=false;
									}
								}
								else{
									//only first gaps limitation
									//if(allowEmptyDays){
									if(1){
										int remG=0, totalH=0;

										int nUsedMornings=0;
										int nUsedAfternoons=0;

										for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
											int remGDay=0;
											if(/*1*/ /*!allowEmptyDays ||*/ sbgDayNHours[d2]>0 || (!allowEmptyMornings && (d2%2)==0) || (!allowEmptyAfternoons && (d2%2)==1)){
												int tm=-1;
												if(d2%2==0)
													tm=mhm;
												else
													tm=mhaft;
												assert(tm>=1);
												if(sbgDayNHours[d2]</*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm){
													remGDay=0;
													totalH+=/*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm;
												}
												else{
													totalH+=sbgDayNHours[d2];
													if(sbgDayNFirstGaps[d2]==0)
														remGDay=0;
													else if(sbgDayNFirstGaps[d2]==1)
														remGDay=1;
													else if(sbgDayNFirstGaps[d2]>=2){
														remGDay=0;
														totalH++;
													}
												}
											}
											if(remGDay>0)
												remG+=remGDay;

											if(sbgDayNHours[d2]>0){
												if(d2%2==0)
													nUsedMornings++;
												else
													nUsedAfternoons++;
											}
										}

										if(subgroupsMinMorningsPerWeekMinMornings[sbg]>=0)
											if(subgroupsMinMorningsPerWeekMinMornings[sbg]>nUsedMornings)
												totalH+=(subgroupsMinMorningsPerWeekMinMornings[sbg]-nUsedMornings)*mhm;

										if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>=0)
											if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>nUsedAfternoons)
												totalH+=(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]-nUsedAfternoons)*mhaft;

										if((remG+totalH <= nHoursPerSubgroup[sbg]+subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg])
										  && (totalH <= nHoursPerSubgroup[sbg]))
											;//ok=true;
										else
											ok=false;
									}
									if(!allowEmptyDays){
										//real days
										int remG=0, totalH=0;
										for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
											int remGDay=0;
											if(/*1*/ !allowEmptyDays || sbgDayNHours[2*d2]+sbgDayNHours[2*d2+1]>0){
												if(sbgDayNHours[2*d2]+sbgDayNHours[2*d2+1]<mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/){
													remGDay=0;
													totalH+=mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/;
												}
												else{
													totalH+=sbgDayNHours[2*d2]+sbgDayNHours[2*d2+1];
													remGDay=0;
													if(sbgDayNFirstGaps[2*d2]==0)
														remGDay+=0;
													else if(sbgDayNFirstGaps[2*d2]==1)
														remGDay+=1;
													else if(sbgDayNFirstGaps[2*d2]>=2){
														remGDay+=0;
														totalH++;
													}

													if(sbgDayNFirstGaps[2*d2+1]==0)
														remGDay+=0;
													else if(sbgDayNFirstGaps[2*d2+1]==1)
														remGDay+=1;
													else if(sbgDayNFirstGaps[2*d2+1]>=2){
														remGDay+=0;
														totalH++;
													}
												}
											}
											if(remGDay>0)
												remG+=remGDay;
										}
										if((remG+totalH <= nHoursPerSubgroup[sbg]+subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg])
										  && (totalH <= nHoursPerSubgroup[sbg]))
											;//ok=true;
										else
											ok=false;
									}
								}
							}
							else{
								if(subgroupsMaxGapsPerWeekPercentage[sbg]>=0){
									//only max gaps per week limitation
									//if(allowEmptyDays){
									if(1){
										int remG=0, totalH=0;

										int nUsedMornings=0;
										int nUsedAfternoons=0;

										for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
											int remGDay=sbgDayNGaps[d2];
											if(/*1*/ /*!allowEmptyDays ||*/ sbgDayNHours[d2]>0 || (!allowEmptyMornings && (d2%2)==0) || (!allowEmptyAfternoons && (d2%2)==1)){
												int tm=-1;
												if(d2%2==0)
													tm=mhm;
												else
													tm=mhaft;
												assert(tm>=1);
												if(sbgDayNHours[d2]</*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm){
													remGDay-=/*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm-sbgDayNHours[d2];
													totalH+=/*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm;
												}
												else
													totalH+=sbgDayNHours[d2];
											}
											if(remGDay>0)
												remG+=remGDay;
	
											if(sbgDayNHours[d2]>0){
												if(d2%2==0)
													nUsedMornings++;
												else
													nUsedAfternoons++;
											}
										}

										if(subgroupsMinMorningsPerWeekMinMornings[sbg]>=0)
											if(subgroupsMinMorningsPerWeekMinMornings[sbg]>nUsedMornings)
												totalH+=(subgroupsMinMorningsPerWeekMinMornings[sbg]-nUsedMornings)*mhm;

										if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>=0)
											if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>nUsedAfternoons)
												totalH+=(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]-nUsedAfternoons)*mhaft;

										if((remG+totalH <= nHoursPerSubgroup[sbg]+subgroupsMaxGapsPerWeekMaxGaps[sbg])
										  && (totalH <= nHoursPerSubgroup[sbg]))
											;//ok=true;
										else
											ok=false;
									}
									if(!allowEmptyDays){
										//real days
										int remG=0, totalH=0;
										for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
											int remGDay=sbgDayNGaps[2*d2]+sbgDayNGaps[2*d2+1];
											if(/*1*/ !allowEmptyDays || sbgDayNHours[2*d2]+sbgDayNHours[2*d2+1]>0){
											if(sbgDayNHours[2*d2]+sbgDayNHours[2*d2+1]<mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/){
													remGDay-=mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/-sbgDayNHours[2*d2]-sbgDayNHours[2*d2+1];
													totalH+=mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/;
												}
												else
													totalH+=sbgDayNHours[2*d2]+sbgDayNHours[2*d2+1];
											}
											if(remGDay>0)
												remG+=remGDay;
										}
										if((remG+totalH <= nHoursPerSubgroup[sbg]+subgroupsMaxGapsPerWeekMaxGaps[sbg])
										  && (totalH <= nHoursPerSubgroup[sbg]))
											;//ok=true;
										else
											ok=false;
									}
								}
								else{
									//no limitation
									//if(allowEmptyDays){
									if(1){
										int totalH=0;

										int nUsedMornings=0;
										int nUsedAfternoons=0;

										for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
											if(/*1*/ /*!allowEmptyDays ||*/ sbgDayNHours[d2]>0 || (!allowEmptyMornings && (d2%2)==0) || (!allowEmptyAfternoons && (d2%2)==1)){
												int tm=-1;
												if(d2%2==0)
													tm=mhm;
												else
													tm=mhaft;
												assert(tm>=1);
												if(sbgDayNHours[d2]</*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm)
													totalH+=/*subgroupsMinHoursDailyMinHours[sbg][d2%2]*/tm;
												else
													totalH+=sbgDayNHours[d2];
											}

											if(sbgDayNHours[d2]>0){
												if(d2%2==0)
													nUsedMornings++;
												else
													nUsedAfternoons++;
											}
										}
										if(subgroupsMinMorningsPerWeekMinMornings[sbg]>=0)
											if(subgroupsMinMorningsPerWeekMinMornings[sbg]>nUsedMornings)
												totalH+=(subgroupsMinMorningsPerWeekMinMornings[sbg]-nUsedMornings)*mhm;

										if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>=0)
											if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>nUsedAfternoons)
												totalH+=(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]-nUsedAfternoons)*mhaft;

										if(totalH <= nHoursPerSubgroup[sbg])
											;//ok=true;
										else
											ok=false;
									}
									if(!allowEmptyDays){
										//real days
										int totalH=0;
										for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
											if(/*1*/ !allowEmptyDays || sbgDayNHours[2*d2]+sbgDayNHours[2*d2+1]>0){
												if(sbgDayNHours[2*d2]+sbgDayNHours[2*d2+1]<mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/)
													totalH+=mhd /*subgroupsMinHoursDailyMinHours[sbg][1]*/;
												else
													totalH+=sbgDayNHours[2*d2]+sbgDayNHours[2*d2+1];
											}
										}

										if(totalH <= nHoursPerSubgroup[sbg])
											;//ok=true;
										else
											ok=false;
									}
								}
							}
							////////////////////////////

							if(ok)
								break; //ok

							int ai2=-1;

							bool kk=subgroupRemoveAnActivityFromEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!kk){
								bool k=subgroupRemoveAnActivityFromBegin(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k){
									bool ka=subgroupRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);

									if(!ka){
										if(level==0){
											/*cout<<"d=="<<d<<", h=="<<h<<", ai=="<<ai<<endl;
											for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
												for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
													cout<<"\t"<<sbgTimetable(d2,h2);
												cout<<endl;
											}*/

											//this should not be displayed
											//cout<<"WARNING - unlikely situation - file "<<__FILE__<<" line "<<__LINE__<<endl;
										}
										okstudentsminhoursdaily=false;
										goto impossiblestudentsminhoursdaily;
									}
								}
							}

							assert(ai2>=0);

							removeAi2FromSbgTimetable(ai2);
							updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
		}
		
impossiblestudentsminhoursdaily:
		if(!okstudentsminhoursdaily){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/////////end students(s) min hours daily

/////////////////////////////////////////////////////////////////////////////////////////////

		okstudentsminmorningsafternoonsperweek=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int sbg : qAsConst(act->iSubgroupsList)){
				if(subgroupsMinMorningsPerWeekMinMornings[sbg]>=0 || subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>=0){
					//assert(teachersMinHoursDailyPercentages[tch]==100);
					int mhd[2];
					mhd[1]=1; //afternoon
					mhd[0]=1; //morning, at least as large as for daily
					if(subgroupsMinHoursDailyPercentages[sbg][1]==100){
						assert(mhd[1]<subgroupsMinHoursDailyMinHours[sbg][1]);
						mhd[1]=subgroupsMinHoursDailyMinHours[sbg][1];
					}
					if(subgroupsMinHoursDailyPercentages[sbg][0]==100){
						assert(mhd[0]<subgroupsMinHoursDailyMinHours[sbg][0]);
						mhd[0]=subgroupsMinHoursDailyMinHours[sbg][0];
					}

					if(subgroupsMinHoursPerAfternoonPercentages[sbg]==100){
						assert(mhd[1]<=subgroupsMinHoursPerAfternoonMinHours[sbg]);
						mhd[1]=subgroupsMinHoursPerAfternoonMinHours[sbg];
					}

					//assert(teachersMaxGapsPerWeekMaxGaps[tch]==0 || teachersMaxGapsPerDayMaxGaps[tch]==0);

					bool skip=false;
					if(!skip){
						//preliminary test
						bool _ok;

						int requested=0;
						int filledMornings=0;
						int filledAfternoons=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							int p=/*newTeachersDayNGaps(tch, d2)+*/newSubgroupsDayNHours(sbg, d2);
							if(subgroupsMaxGapsPerWeekMaxGaps[sbg]==0 || subgroupsMaxGapsPerDayMaxGaps[sbg]==0 ||
							 subgroupsMaxGapsPerRealDayMaxGaps[sbg]==0 || subgroupsMaxGapsPerWeekForRealDaysMaxGaps[sbg]==0)
								p+=newSubgroupsDayNGaps(sbg, d2);
							if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0)
								p+=newSubgroupsDayNFirstGaps(sbg, d2);
							if(p>0 && p<mhd[d2%2])
								p=mhd[d2%2];
							requested+=p;
							if(p>0){
								if(d2%2==0)
									filledMornings++;
								else
									filledAfternoons++;
							}
						}

						if(subgroupsMinMorningsPerWeekMinMornings[sbg]>0){
							if(filledMornings<subgroupsMinMorningsPerWeekMinMornings[sbg])
								requested+=mhd[0]*(-filledMornings+subgroupsMinMorningsPerWeekMinMornings[sbg]);
						}
						if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>0){
							if(filledAfternoons<subgroupsMinAfternoonsPerWeekMinAfternoons[sbg])
								requested+=mhd[1]*(-filledAfternoons+subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]);
						}

						_ok=(requested<=nHoursPerSubgroup[sbg]);

						if(_ok)
							continue;

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okstudentsminmorningsafternoonsperweek=false;
							goto impossiblestudentsminmorningsafternoonsperweek;
						}

						getSbgTimetable(sbg, conflActivities[newtime]);
						sbgGetNHoursGaps(sbg);

						for(;;){
							bool ok;
							int requested=0;
							int filledMornings=0;
							int filledAfternoons=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								int p=/*newTeachersDayNGaps(tch, d2)+*/sbgDayNHours[d2];
								if(subgroupsMaxGapsPerWeekMaxGaps[sbg]==0 || subgroupsMaxGapsPerDayMaxGaps[sbg]==0 ||
								 subgroupsMaxGapsPerRealDayMaxGaps[sbg]==0 || subgroupsMaxGapsPerWeekForRealDaysMaxGaps[sbg]==0)
									p+=sbgDayNGaps[d2];
								if(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0)
									p+=sbgDayNFirstGaps[d2];
								if(p>0 && p<mhd[d2%2])
									p=mhd[d2%2];
								requested+=p;
								if(p>0){
									if(d2%2==0)
										filledMornings++;
									else
										filledAfternoons++;
								}
							}
							if(subgroupsMinMorningsPerWeekMinMornings[sbg]>0){
								if(filledMornings<subgroupsMinMorningsPerWeekMinMornings[sbg])
									requested+=mhd[0]*(-filledMornings+subgroupsMinMorningsPerWeekMinMornings[sbg]);
							}
							if(subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]>0){
								if(filledAfternoons<subgroupsMinAfternoonsPerWeekMinAfternoons[sbg])
									requested+=mhd[1]*(-filledAfternoons+subgroupsMinAfternoonsPerWeekMinAfternoons[sbg]);
							}

							ok=(requested<=nHoursPerSubgroup[sbg]);

							if(ok)
								break;

							int ai2=-1;

							if(!(subgroupsEarlyMaxBeginningsAtSecondHourMaxBeginnings[sbg]==0)){
								bool k=subgroupRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								if(!k){
									bool k2=false;
									if(!(subgroupsMaxGapsPerWeekMaxGaps[sbg]==0 || subgroupsMaxGapsPerDayMaxGaps[sbg]==0 ||
									 subgroupsMaxGapsPerRealDayMaxGaps[sbg]==0 || subgroupsMaxGapsPerWeekForRealDaysMaxGaps[sbg]==0)){
										k2=subgroupRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
										assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									}

									if(!k2){
										if(level==0){
											//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
											//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
										}
										okstudentsminmorningsafternoonsperweek=false;
										goto impossiblestudentsminmorningsafternoonsperweek;
									}
								}
							}
							else{
								bool k=subgroupRemoveAnActivityFromEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								if(!k){
									bool k2=false;
									if(!(subgroupsMaxGapsPerWeekMaxGaps[sbg]==0 || subgroupsMaxGapsPerDayMaxGaps[sbg]==0 ||
									 subgroupsMaxGapsPerRealDayMaxGaps[sbg]==0 || subgroupsMaxGapsPerWeekForRealDaysMaxGaps[sbg]==0)){
										k2=subgroupRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
										assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									}

									if(!k2){
										if(level==0){
											//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
											//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
										}
										okstudentsminmorningsafternoonsperweek=false;
										goto impossiblestudentsminmorningsafternoonsperweek;
									}
								}
							}

							assert(ai2>=0);

							/*Activity* act2=&gt.rules.internalActivitiesList[ai2];
							int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
							int h2=c.times[ai2]/gt.rules.nDaysPerWeek;

							for(int dur2=0; dur2<act2->duration; dur2++){
								assert(tchTimetable(d2,h2+dur2)==ai2);
								tchTimetable(d2,h2+dur2)=-1;
							}*/

							removeAi2FromSbgTimetable(ai2);
							updateSbgNHoursGaps(sbg, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
		}

impossiblestudentsminmorningsafternoonsperweek:
		if(!okstudentsminmorningsafternoonsperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		/////////end students (set) min mornings or afternoons per week

/////////////////////////////////////////////////////////////////////////////////////////////

		/////////begin students (set) max two activity tags per day from N1 N2 N3

		okstudentsmaxtwoactivitytagsperdayfromn1n2n3=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int sbg : qAsConst(act->iSubgroupsList)){
				if(subgroupsMaxTwoActivityTagsPerDayFromN1N2N3Percentages[sbg]>=0){
					assert(subgroupsMaxTwoActivityTagsPerDayFromN1N2N3Percentages[sbg]==100);
					int cnt[4]; //cnt[3] is for activities which have no tag from N1, N2, or N3. Crash bug fixed on 2021-07-25.
					cnt[0]=cnt[1]=cnt[2]=cnt[3]=0;
					for(int ai2 : qAsConst(subgroupActivitiesOfTheDay[sbg][d]))
						if(!conflActivities[newtime].contains(ai2)){
							int actTag=activityTagN1N2N3[ai2];
							cnt[actTag]++;
						}

					int cntTags=0;
					for(int i=0; i<3; i++)
						if(cnt[i]>0)
							cntTags++;
					assert(cntTags<=2);
					for(int i=0; i<3; i++){
						int c0, c1, c2;
						if(i==0){
							c0=0; c1=1; c2=2;
						}
						else if(i==1){
							c0=0; c1=2; c2=1;
						}
						else if(i==2){
							c0=1; c1=2; c2=0;
						}
						else{
							assert(0);
						}

						if(cnt[c0]>0 && cnt[c1]>0 && activityTagN1N2N3[ai]==c2){
							bool canEmptyc0=true;
							bool canEmptyc1=true;
							QList<int> activitiesWithc0;
							QList<int> activitiesWithc1;
							
							for(int ai2 : qAsConst(subgroupActivitiesOfTheDay[sbg][d])){
								if(activityTagN1N2N3[ai2]==c0 && !conflActivities[newtime].contains(ai2)){
									if(!fixedTimeActivity[ai2] && !swappedActivities[ai2]){
										activitiesWithc0.append(ai2);
									}
									else{
										canEmptyc0=false;
										break;
									}
								}
							}

							for(int ai2 : qAsConst(subgroupActivitiesOfTheDay[sbg][d])){
								if(activityTagN1N2N3[ai2]==c1 && !conflActivities[newtime].contains(ai2)){
									if(!fixedTimeActivity[ai2] && !swappedActivities[ai2]){
										activitiesWithc1.append(ai2);
									}
									else{
										canEmptyc1=false;
										break;
									}
								}
							}
							
							if(!canEmptyc0 && !canEmptyc1){
								okstudentsmaxtwoactivitytagsperdayfromn1n2n3=false;
								goto impossiblestudentsmaxtwoactivitytagsperdayfromn1n2n3;
							}
							else if(canEmptyc0 && !canEmptyc1){
								for(int ai2 : qAsConst(activitiesWithc0)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
								}
							}
							else if(!canEmptyc0 && canEmptyc1){
								for(int ai2 : qAsConst(activitiesWithc1)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
								}
							}
							else{
								assert(canEmptyc0 && canEmptyc1);
								if(level>0){
									if(activitiesWithc0.count()<activitiesWithc1.count()){
										for(int ai2 : qAsConst(activitiesWithc0)){
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
										}
									}
									else if(activitiesWithc0.count()>activitiesWithc1.count()){
										for(int ai2 : qAsConst(activitiesWithc1)){
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
										}
									}
									else{
										int rnd=rng.intMRG32k3a(2);
										if(rnd==0){
											for(int ai2 : qAsConst(activitiesWithc0)){
												conflActivities[newtime].append(ai2);
												nConflActivities[newtime]++;
											}
										}
										else{
											assert(rnd==1);
											for(int ai2 : qAsConst(activitiesWithc1)){
												conflActivities[newtime].append(ai2);
												nConflActivities[newtime]++;
											}
										}
									}
								}
								else{
									assert(level==0);

									int _minWrong[3];
									int _nWrong[3];
									int _nConflActivities[3];
									int _minIndexAct[3];
									
									_minWrong[c0]=INF;
									_nWrong[c0]=0;
									_nConflActivities[c0]=activitiesWithc0.count();
									_minIndexAct[c0]=gt.rules.nInternalActivities;

									for(int ai2 : qAsConst(activitiesWithc0)){
										_minWrong[c0] = min (_minWrong[c0], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[c0]=min(_minIndexAct[c0], invPermutation[ai2]);
										_nWrong[c0]+=triedRemovals(ai2,c.times[ai2]);
									}

									_minWrong[c1]=INF;
									_nWrong[c1]=0;
									_nConflActivities[c1]=activitiesWithc1.count();
									_minIndexAct[c1]=gt.rules.nInternalActivities;
									
									for(int ai2 : qAsConst(activitiesWithc1)){
										_minWrong[c1] = min (_minWrong[c1], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[c1]=min(_minIndexAct[c1], invPermutation[ai2]);
										_nWrong[c1]+=triedRemovals(ai2,c.times[ai2]);
									}

									if(_minWrong[c0]==_minWrong[c1] && _nWrong[c0]==_nWrong[c1] && _nConflActivities[c0]==_nConflActivities[c1] && _minIndexAct[c0]==_minIndexAct[c1]){
										int rnd=rng.intMRG32k3a(2);
										if(rnd==0){
											for(int ai2 : qAsConst(activitiesWithc0)){
												conflActivities[newtime].append(ai2);
												nConflActivities[newtime]++;
											}
										}
										else{
											assert(rnd==1);
											for(int ai2 : qAsConst(activitiesWithc1)){
												conflActivities[newtime].append(ai2);
												nConflActivities[newtime]++;
											}
										}
									}
									else if(_minWrong[c0]>_minWrong[c1] ||
									  (_minWrong[c0]==_minWrong[c1] && _nWrong[c0]>_nWrong[c1]) ||
									  (_minWrong[c0]==_minWrong[c1] && _nWrong[c0]==_nWrong[c1] && _nConflActivities[c0]>_nConflActivities[c1]) ||
									  (_minWrong[c0]==_minWrong[c1] && _nWrong[c0]==_nWrong[c1] && _nConflActivities[c0]==_nConflActivities[c1] && _minIndexAct[c0]>_minIndexAct[c1])){
										//choose c1
										for(int ai2 : qAsConst(activitiesWithc1)){
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
										}
									}
									else{
										assert(_minWrong[c1]>_minWrong[c0] ||
										  (_minWrong[c1]==_minWrong[c0] && _nWrong[c1]>_nWrong[c0]) ||
										  (_minWrong[c1]==_minWrong[c0] && _nWrong[c1]==_nWrong[c0] && _nConflActivities[c1]>_nConflActivities[c0]) ||
										  (_minWrong[c1]==_minWrong[c0] && _nWrong[c1]==_nWrong[c0] && _nConflActivities[c1]==_nConflActivities[c0] && _minIndexAct[c1]>_minIndexAct[c0]));
										//choose c0
										for(int ai2 : qAsConst(activitiesWithc0)){
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
										}
									}
								}
							}
						}
					}
				}
			}
		}

impossiblestudentsmaxtwoactivitytagsperdayfromn1n2n3:
		if(!okstudentsmaxtwoactivitytagsperdayfromn1n2n3){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		/////////end students (set) max two activity tags per day from N1 N2 N3

/////////////////////////////////////////////////////////////////////////////////////////////

		/////////begin students (set) max two activity tags per real day from N1 N2 N3

		okstudentsmaxtwoactivitytagsperrealdayfromn1n2n3=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int sbg : qAsConst(act->iSubgroupsList)){
				if(subgroupsMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages[sbg]>=0){
					assert(subgroupsMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages[sbg]==100);
					int cnt[4]; //cnt[3] is for activities which have no tag from N1, N2, or N3. Crash bug fixed on 2021-07-25.
					cnt[0]=cnt[1]=cnt[2]=cnt[3]=0;
					for(int ai2 : qAsConst(subgroupActivitiesOfTheDay[sbg][d]))
						if(!conflActivities[newtime].contains(ai2)){
							int actTag=activityTagN1N2N3[ai2];
							cnt[actTag]++;
						}
					for(int ai2 : qAsConst(subgroupActivitiesOfTheDay[sbg][dpair]))
						if(!conflActivities[newtime].contains(ai2)){
							int actTag=activityTagN1N2N3[ai2];
							cnt[actTag]++;
						}

					int cntTags=0;
					for(int i=0; i<3; i++)
						if(cnt[i]>0)
							cntTags++;
					assert(cntTags<=2);
					for(int i=0; i<3; i++){
						int c0, c1, c2;
						if(i==0){
							c0=0; c1=1; c2=2;
						}
						else if(i==1){
							c0=0; c1=2; c2=1;
						}
						else if(i==2){
							c0=1; c1=2; c2=0;
						}
						else{
							assert(0);
						}

						if(cnt[c0]>0 && cnt[c1]>0 && activityTagN1N2N3[ai]==c2){
							bool canEmptyc0=true;
							bool canEmptyc1=true;
							QList<int> activitiesWithc0;
							QList<int> activitiesWithc1;
							
							for(int ai2 : qAsConst(subgroupActivitiesOfTheDay[sbg][d])){
								if(activityTagN1N2N3[ai2]==c0 && !conflActivities[newtime].contains(ai2)){
									if(!fixedTimeActivity[ai2] && !swappedActivities[ai2]){
										activitiesWithc0.append(ai2);
									}
									else{
										canEmptyc0=false;
										break;
									}
								}
							}
							if(canEmptyc0){
								for(int ai2 : qAsConst(subgroupActivitiesOfTheDay[sbg][dpair])){
									if(activityTagN1N2N3[ai2]==c0 && !conflActivities[newtime].contains(ai2)){
										if(!fixedTimeActivity[ai2] && !swappedActivities[ai2]){
											activitiesWithc0.append(ai2);
										}
										else{
											canEmptyc0=false;
											break;
										}
									}
								}
							}

							for(int ai2 : qAsConst(subgroupActivitiesOfTheDay[sbg][d])){
								if(activityTagN1N2N3[ai2]==c1 && !conflActivities[newtime].contains(ai2)){
									if(!fixedTimeActivity[ai2] && !swappedActivities[ai2]){
										activitiesWithc1.append(ai2);
									}
									else{
										canEmptyc1=false;
										break;
									}
								}
							}
							if(canEmptyc1){
								for(int ai2 : qAsConst(subgroupActivitiesOfTheDay[sbg][dpair])){
									if(activityTagN1N2N3[ai2]==c1 && !conflActivities[newtime].contains(ai2)){
										if(!fixedTimeActivity[ai2] && !swappedActivities[ai2]){
											activitiesWithc1.append(ai2);
										}
										else{
											canEmptyc1=false;
											break;
										}
									}
								}
							}
							
							if(!canEmptyc0 && !canEmptyc1){
								okstudentsmaxtwoactivitytagsperrealdayfromn1n2n3=false;
								goto impossiblestudentsmaxtwoactivitytagsperrealdayfromn1n2n3;
							}
							else if(canEmptyc0 && !canEmptyc1){
								for(int ai2 : qAsConst(activitiesWithc0)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
								}
							}
							else if(!canEmptyc0 && canEmptyc1){
								for(int ai2 : qAsConst(activitiesWithc1)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
								}
							}
							else{
								assert(canEmptyc0 && canEmptyc1);
								if(level>0){
									if(activitiesWithc0.count()<activitiesWithc1.count()){
										for(int ai2 : qAsConst(activitiesWithc0)){
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
										}
									}
									else if(activitiesWithc0.count()>activitiesWithc1.count()){
										for(int ai2 : qAsConst(activitiesWithc1)){
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
										}
									}
									else{
										int rnd=rng.intMRG32k3a(2);
										if(rnd==0){
											for(int ai2 : qAsConst(activitiesWithc0)){
												conflActivities[newtime].append(ai2);
												nConflActivities[newtime]++;
											}
										}
										else{
											assert(rnd==1);
											for(int ai2 : qAsConst(activitiesWithc1)){
												conflActivities[newtime].append(ai2);
												nConflActivities[newtime]++;
											}
										}
									}
								}
								else{
									assert(level==0);

									int _minWrong[3];
									int _nWrong[3];
									int _nConflActivities[3];
									int _minIndexAct[3];
									
									_minWrong[c0]=INF;
									_nWrong[c0]=0;
									_nConflActivities[c0]=activitiesWithc0.count();
									_minIndexAct[c0]=gt.rules.nInternalActivities;

									for(int ai2 : qAsConst(activitiesWithc0)){
										_minWrong[c0] = min (_minWrong[c0], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[c0]=min(_minIndexAct[c0], invPermutation[ai2]);
										_nWrong[c0]+=triedRemovals(ai2,c.times[ai2]);
									}

									_minWrong[c1]=INF;
									_nWrong[c1]=0;
									_nConflActivities[c1]=activitiesWithc1.count();
									_minIndexAct[c1]=gt.rules.nInternalActivities;
									
									for(int ai2 : qAsConst(activitiesWithc1)){
										_minWrong[c1] = min (_minWrong[c1], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[c1]=min(_minIndexAct[c1], invPermutation[ai2]);
										_nWrong[c1]+=triedRemovals(ai2,c.times[ai2]);
									}

									if(_minWrong[c0]==_minWrong[c1] && _nWrong[c0]==_nWrong[c1] && _nConflActivities[c0]==_nConflActivities[c1] && _minIndexAct[c0]==_minIndexAct[c1]){
										int rnd=rng.intMRG32k3a(2);
										if(rnd==0){
											for(int ai2 : qAsConst(activitiesWithc0)){
												conflActivities[newtime].append(ai2);
												nConflActivities[newtime]++;
											}
										}
										else{
											assert(rnd==1);
											for(int ai2 : qAsConst(activitiesWithc1)){
												conflActivities[newtime].append(ai2);
												nConflActivities[newtime]++;
											}
										}
									}
									else if(_minWrong[c0]>_minWrong[c1] ||
									  (_minWrong[c0]==_minWrong[c1] && _nWrong[c0]>_nWrong[c1]) ||
									  (_minWrong[c0]==_minWrong[c1] && _nWrong[c0]==_nWrong[c1] && _nConflActivities[c0]>_nConflActivities[c1]) ||
									  (_minWrong[c0]==_minWrong[c1] && _nWrong[c0]==_nWrong[c1] && _nConflActivities[c0]==_nConflActivities[c1] && _minIndexAct[c0]>_minIndexAct[c1])){
										//choose c1
										for(int ai2 : qAsConst(activitiesWithc1)){
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
										}
									}
									else{
										assert(_minWrong[c1]>_minWrong[c0] ||
										  (_minWrong[c1]==_minWrong[c0] && _nWrong[c1]>_nWrong[c0]) ||
										  (_minWrong[c1]==_minWrong[c0] && _nWrong[c1]==_nWrong[c0] && _nConflActivities[c1]>_nConflActivities[c0]) ||
										  (_minWrong[c1]==_minWrong[c0] && _nWrong[c1]==_nWrong[c0] && _nConflActivities[c1]==_nConflActivities[c0] && _minIndexAct[c1]>_minIndexAct[c0]));
										//choose c0
										for(int ai2 : qAsConst(activitiesWithc0)){
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
										}
									}
								}
							}
						}
					}
				}
			}
		}

impossiblestudentsmaxtwoactivitytagsperrealdayfromn1n2n3:
		if(!okstudentsmaxtwoactivitytagsperrealdayfromn1n2n3){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		/////////end students (set) max two activity tags per real day from N1 N2 N3

/////////////////////////////////////////////////////////////////////////////////////////////

		//2020-06-28
		//students max hours per all afternoons.

		okstudentsmaxhoursperallafternoons=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int sbg : qAsConst(act->iSubgroupsList)){
				if(subgroupsMaxHoursPerAllAfternoonsMaxHours[sbg]>=0){
					assert(subgroupsMaxHoursPerAllAfternoonsPercentages[sbg]==100.0);

					if(d%2==1){ //afternoon
						//preliminary
						int _nOcc=0;

						for(int d2=1; d2<gt.rules.nDaysPerWeek; d2+=2){ //afternoon
							for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
								int ai2=newSubgroupsTimetable(sbg,d2,h2);
								if(ai2>=0)
									_nOcc++;
							}
						}

						if(_nOcc<=subgroupsMaxHoursPerAllAfternoonsMaxHours[sbg])
							continue; //ok

						getSbgTimetable(sbg, conflActivities[newtime]);

						int nOccupied=0;

						QSet<int> candidates;

						//static int slotActivity[MAX_DAYS_PER_WEEK*MAX_HOURS_PER_DAY];

						for(int d2=1; d2<gt.rules.nDaysPerWeek; d2+=2){ //afternoon
							for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
								int t=d2+gt.rules.nDaysPerWeek*h2;

								int ai2=sbgTimetable(d2,h2);
								slotActivity[t]=ai2;
								if(ai2>=0){
									nOccupied++;
									if(ai2!=ai && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
										candidates.insert(t);
								}
							}
						}

						if(nOccupied > subgroupsMaxHoursPerAllAfternoonsMaxHours[sbg]){
							int target=nOccupied - subgroupsMaxHoursPerAllAfternoonsMaxHours[sbg];

							while(target>0){
								bool decreased=false;

								if(candidates.count()==0){
									okstudentsmaxhoursperallafternoons=false;
									goto impossiblestudentsmaxhoursperallafternoons;
								}

								//To keep the generation identical on all computers
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
								QList<int> tmpSortedList=QList<int>(candidates.constBegin(), candidates.constEnd());
#else
								QList<int> tmpSortedList=candidates.toList();
#endif
								std::stable_sort(tmpSortedList.begin(), tmpSortedList.end());

								int t=-1;
								if(level>0){
									assert(candidates.count()==tmpSortedList.count());
									int q=rng.intMRG32k3a(candidates.count());
									t=tmpSortedList.at(q);
								}
								else{
									assert(level==0);

									int optMinWrong=INF;
									QList<int> tl;

									for(int t2 : qAsConst(tmpSortedList)){
										int ai3=slotActivity[t2];
										if(optMinWrong>triedRemovals(ai3,c.times[ai3])){
											optMinWrong=triedRemovals(ai3,c.times[ai3]);
											tl.clear();
											tl.append(t2);
										}
										else if(optMinWrong==triedRemovals(ai3,c.times[ai3])){
											tl.append(t2);
										}
									}

									assert(tl.count()>0);
									int q=rng.intMRG32k3a(tl.count());
									t=tl.at(q);
								}

								assert(t>=0);
								int ai2=slotActivity[t];

								assert(ai2>=0);
								assert(ai2!=ai);
								assert(c.times[ai2]!=UNALLOCATED_TIME);
								assert(!fixedTimeActivity[ai2] && !swappedActivities[ai2]);

								for(int tt=c.times[ai2]; tt<c.times[ai2]+gt.rules.internalActivitiesList[ai2].duration*gt.rules.nDaysPerWeek; tt+=gt.rules.nDaysPerWeek){
									assert(slotActivity[tt]==ai2);
									slotActivity[tt]=-1;
									assert(candidates.contains(tt));
									candidates.remove(tt);
									target--;

									decreased=true;
								}

								assert(!conflActivities[newtime].contains(ai2));
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(nConflActivities[newtime]==conflActivities[newtime].count());

								removeAi2FromSbgTimetable(ai2); //not really needed

								assert(decreased);
							}
						}
					}
				}
			}
		}

impossiblestudentsmaxhoursperallafternoons:
		if(!okstudentsmaxhoursperallafternoons){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from students activity tag min hours daily
		
		//!!!NOT PERFECT, there is room for improvement
		
		okstudentsactivitytagminhoursdaily=true;
		
		if(haveStudentsActivityTagMinHoursDaily){
			for(int sbg : qAsConst(act->iSubgroupsList)){
				for(SubgroupActivityTagMinHoursDaily_item* item : qAsConst(satmhdListForSubgroup[sbg])){
					if(!gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(item->activityTag))
						continue;
					
					//int sbgDayNHoursWithTag[MAX_DAYS_PER_WEEK];
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
						sbgDayNHoursWithTag[d2]=0;
						
					//bool possibleToEmptyDay[MAX_DAYS_PER_WEEK];
					
					//code similar to getSbgTimetable
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						possibleToEmptyDay[d2]=true;
						for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
							int ai2=newSubgroupsTimetable(sbg,d2,h2);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
								if(gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(item->activityTag)){
									sbgTimetable(d2,h2)=ai2;
									sbgDayNHoursWithTag[d2]++;
									
									if(item->allowEmptyDays && (ai2==ai || fixedTimeActivity[ai2] || swappedActivities[ai2]))
										possibleToEmptyDay[d2]=false;
								}
								else{
									sbgTimetable(d2,h2)=-1;
								}
							}
							else{
								sbgTimetable(d2,h2)=-1;
							}
						}
					}
					
					int necessary=0;
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						if(sbgDayNHoursWithTag[d2]>0 || (sbgDayNHoursWithTag[d2]==0 && !item->allowEmptyDays)){
							necessary+=max(item->minHoursDaily, sbgDayNHoursWithTag[d2]);
						}
					}
					
					if(necessary > item->durationOfActivitiesWithActivityTagForSubgroup){
						//not OK
						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okstudentsactivitytagminhoursdaily=false;
							goto impossiblestudentsactivitytagminhoursdaily;
						}
						
						QSet<int> candidatesSet;
						
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							if(sbgDayNHoursWithTag[d2]>0){
								if((item->allowEmptyDays && possibleToEmptyDay[d2]) || sbgDayNHoursWithTag[d2] > item->minHoursDaily){
									for(int h2=0; h2<gt.rules.nHoursPerDay; ){
										int ai2=sbgTimetable(d2,h2);
										if(ai2>=0){
											if(ai2!=ai && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
												candidatesSet.insert(ai2);
											h2+=gt.rules.internalActivitiesList[ai2].duration;
										}
										else{
											h2++;
										}
									}
								}
							}
						}
						
						for(;;){
							if(candidatesSet.count()==0){
								okstudentsactivitytagminhoursdaily=false;
								goto impossiblestudentsactivitytagminhoursdaily;
							}
							else{
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
								QList<int> candidatesList(candidatesSet.constBegin(), candidatesSet.constEnd());
#else
								QList<int> candidatesList=candidatesSet.toList();
#endif
								std::stable_sort(candidatesList.begin(), candidatesList.end()); //To keep the generation identical on all computers.
							
								int ai2;
								if(level>0){
									int q=rng.intMRG32k3a(candidatesList.count());
									ai2=candidatesList.at(q);
								}
								else{
									assert(level==0);
				
									int optMinWrong=INF;
	
									QList<int> tl;
			
									for(int ai3 : qAsConst(candidatesList)){
										if(optMinWrong>triedRemovals(ai3,c.times[ai3])){
										 	optMinWrong=triedRemovals(ai3,c.times[ai3]);
										 	tl.clear();
										 	tl.append(ai3);
										}
										else if(optMinWrong==triedRemovals(ai3,c.times[ai3])){
										 	tl.append(ai3);
										}
									}
							
									assert(tl.count()>0);
									int q=rng.intMRG32k3a(tl.count());
									ai2=tl.at(q);
								}
						
								assert(ai2!=ai);
								assert(!fixedTimeActivity[ai2] && !swappedActivities[ai2]);
	
								assert(!conflActivities[newtime].contains(ai2));
								conflActivities[newtime].append(ai2);
	
								nConflActivities[newtime]++;
								assert(nConflActivities[newtime]==conflActivities[newtime].count());

								int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
								int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
								int dur2=gt.rules.internalActivitiesList[ai2].duration;
								sbgDayNHoursWithTag[d2]-=dur2;
								assert(sbgDayNHoursWithTag[d2]>=0);
								
								if(sbgDayNHoursWithTag[d2]==0 && item->allowEmptyDays){
									necessary-=max(item->minHoursDaily, dur2);
									assert(necessary>=0);
								}
								else{
									int oldNecessary=necessary;
									necessary-=max(item->minHoursDaily, sbgDayNHoursWithTag[d2]+dur2);
									assert(necessary>=0);
									necessary+=max(item->minHoursDaily, sbgDayNHoursWithTag[d2]);
									if(!item->allowEmptyDays)
										assert(oldNecessary>necessary);
								}
								
								for(int h3=h2; h3<h2+dur2; h3++){
									assert(sbgTimetable(d2,h3)==ai2);
									sbgTimetable(d2,h3)=-1;
								}
								
								if(necessary <= item->durationOfActivitiesWithActivityTagForSubgroup)
									break; //OK
									
								bool tr=candidatesSet.remove(ai2);
								assert(tr);
								
								if(sbgDayNHoursWithTag[d2]>0 && sbgDayNHoursWithTag[d2]<=item->minHoursDaily &&
								 !(item->allowEmptyDays && possibleToEmptyDay[d2])){
									for(int h3=0; h3<gt.rules.nHoursPerDay; ){
										int ai3=sbgTimetable(d2,h3);
										if(ai3>=0){
											if(ai3!=ai && ai3!=ai2 && !fixedTimeActivity[ai3] && !swappedActivities[ai3]){
												assert(candidatesSet.contains(ai3));
												candidatesSet.remove(ai3);
											}
											h3+=gt.rules.internalActivitiesList[ai3].duration;
										}
										else{
											h3++;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		
impossiblestudentsactivitytagminhoursdaily:
		if(!okstudentsactivitytagminhoursdaily){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from students min gaps between ordered pair of activity tags
		
		okstudentsmingapsbetweenorderedpairofactivitytags=true;
		
		for(StudentsMinGapsBetweenOrderedPairOfActivityTags_item* item : qAsConst(smgbopoatListForActivity[ai])){
			bool first, second;
			if(act->iActivityTagsSet.contains(item->firstActivityTag))
				first=true;
			else
				first=false;

			if(act->iActivityTagsSet.contains(item->secondActivityTag))
				second=true;
			else
				second=false;

			assert((first && !second) || (!first && second));

			if(first){
				assert(!second);
				//after the first activity tag we need to have at least minGaps until the second activity tag.
				for(int sbg : qAsConst(act->iSubgroupsList)){
					if(item->canonicalSetOfSubgroups.contains(sbg)){
						for(int startSecond=h+act->duration; startSecond<gt.rules.nHoursPerDay; startSecond++){
							if(startSecond-h-act->duration >= item->minGaps)
								break;
							int ai2=subgroupsTimetable(sbg,d,startSecond);
							if(ai2>=0){
								if(gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(item->secondActivityTag)){
									if(!conflActivities[newtime].contains(ai2)){
										if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
											okstudentsmingapsbetweenorderedpairofactivitytags=false;
											goto impossiblestudentsmingapsbetweenorderedpairofactivitytags;
										}
										else{
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
			else{
				assert(second);
				//before the second activity tag we need to have at least minGaps until the first activity tag.
				for(int sbg : qAsConst(act->iSubgroupsList)){
					if(item->canonicalSetOfSubgroups.contains(sbg)){
						for(int endFirst=h-1; endFirst>=0; endFirst--){
							if(h-1-endFirst >= item->minGaps)
								break;
							int ai2=subgroupsTimetable(sbg,d,endFirst);
							if(ai2>=0){
								if(gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(item->firstActivityTag)){
									if(!conflActivities[newtime].contains(ai2)){
										if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
											okstudentsmingapsbetweenorderedpairofactivitytags=false;
											goto impossiblestudentsmingapsbetweenorderedpairofactivitytags;
										}
										else{
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
		}
		
impossiblestudentsmingapsbetweenorderedpairofactivitytags:
		if(!okstudentsmingapsbetweenorderedpairofactivitytags){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		

/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from students min gaps between activity tag
		
		okstudentsmingapsbetweenactivitytag=true;
		
		for(StudentsMinGapsBetweenActivityTag_item* item : qAsConst(smgbatListForActivity[ai])){
			bool hasTag;
			if(act->iActivityTagsSet.contains(item->activityTag))
				hasTag=true;
			else
				hasTag=false;

			if(hasTag){
				//before and after the activity tag we need to have at least minGaps until the same activity tag.
				for(int sbg : qAsConst(act->iSubgroupsList)){
					if(item->canonicalSetOfSubgroups.contains(sbg)){
						//after the current activity
						for(int startSecond=h+act->duration; startSecond<gt.rules.nHoursPerDay; startSecond++){
							if(startSecond-h-act->duration >= item->minGaps)
								break;
							int ai2=subgroupsTimetable(sbg,d,startSecond);
							if(ai2>=0){
								if(gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(item->activityTag)){
									if(!conflActivities[newtime].contains(ai2)){
										if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
											okstudentsmingapsbetweenactivitytag=false;
											goto impossiblestudentsmingapsbetweenactivitytag;
										}
										else{
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
											assert(conflActivities[newtime].count()==nConflActivities[newtime]);
										}
									}
								}
							}
						}

						//before the current activity
						for(int startSecond=h-1; startSecond>=0; startSecond--){
							if(h-startSecond > item->minGaps)
								break;
							int ai2=subgroupsTimetable(sbg,d,startSecond);
							if(ai2>=0){
								if(gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(item->activityTag)){
									if(!conflActivities[newtime].contains(ai2)){
										if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
											okstudentsmingapsbetweenactivitytag=false;
											goto impossiblestudentsmingapsbetweenactivitytag;
										}
										else{
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
		}
		
impossiblestudentsmingapsbetweenactivitytag:
		if(!okstudentsmingapsbetweenactivitytag){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////


				////////////TEACHERS////////////////

/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking the teachers max days per week constraints
		////////////////////////////BEGIN max days per week for teachers
		okteachersmaxdaysperweek=true;
		for(int tch : qAsConst(teachersWithMaxDaysPerWeekForActivities[ai])){
			if(skipRandom(teachersMaxDaysPerWeekWeightPercentages[tch]))
				continue;

			int maxDays=teachersMaxDaysPerWeekMaxDays[tch];
			assert(maxDays>=0); //the list contains real information
			
			//preliminary test
			int _nOc=0;
			for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
				//if(newTeachersDayNHours(tch,d2)>0)
				
				//IT IS VITAL TO USE teacherActivitiesOfTheDay as a QList<int> (tch,d2)!!!!!!!
				//The order of evaluation of activities is changed,
				//with activities which were moved forward and back again
				//being put at the end.
				//If you do not follow this, you'll get impossible timetables
				//for the Italian example Italy/2007/difficult/highschool-Ancona.fet or the examples from
				//South Africa: South-Africa/difficult/Collegiate_Junior_School2.fet or
				//South-Africa/difficult/Insight_Learning_Centre2.fet, I am not sure which of these 3
				
				if(teacherActivitiesOfTheDay(tch,d2).count()>0 || d2==d)
					_nOc++;
			if(_nOc<=maxDays)
				continue; //OK, preliminary
			
			if(maxDays>=0){
				assert(maxDays>0);

				if(level>0){
					///getTchTimetable(tch, conflActivities[newtime]);
					///tchGetNHoursGaps(tch);

					//bool occupiedDay[MAX_DAYS_PER_WEEK];
					//bool canEmptyDay[MAX_DAYS_PER_WEEK];
					
					///int _minWrong[MAX_DAYS_PER_WEEK];
					///int _nWrong[MAX_DAYS_PER_WEEK];
					//int _nConflActivities[MAX_DAYS_PER_WEEK];
					///int _minIndexAct[MAX_DAYS_PER_WEEK];
					
					//QList<int> _activitiesForDay[MAX_DAYS_PER_WEEK];

					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						if(d2==d)
							continue;
					
						occupiedDay[d2]=false;
						canEmptyDay[d2]=true;
						
						//_minWrong[d2]=INF;
						//_nWrong[d2]=0;
						_nConflActivities[d2]=0;
						//_minIndexAct[d2]=gt.rules.nInternalActivities;
						_activitiesForDay[d2].clear();
						
						for(int ai2 : qAsConst(teacherActivitiesOfTheDay(tch,d2))){
							if(ai2>=0){
								if(!conflActivities[newtime].contains(ai2)){
									occupiedDay[d2]=true;
									if(fixedTimeActivity[ai2] || swappedActivities[ai2])
										canEmptyDay[d2]=false;
									else if(!_activitiesForDay[d2].contains(ai2)){
										//_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
										//_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
										//_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
										_nConflActivities[d2]++;
										_activitiesForDay[d2].append(ai2);
										assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
									}
								}
							}
						}
						
						if(!occupiedDay[d2])
							canEmptyDay[d2]=false;
					}
					occupiedDay[d]=true;
					canEmptyDay[d]=false;
					
					int nOc=0;
					bool canChooseDay=false;
					
					for(int j=0; j<gt.rules.nDaysPerWeek; j++)
						if(occupiedDay[j]){
							nOc++;
							if(canEmptyDay[j]){
								canChooseDay=true;
							}
						}
					
					if(nOc>maxDays){
						assert(nOc==maxDays+1);
						
						if(!canChooseDay){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachersmaxdaysperweek=false;
							goto impossibleteachersmaxdaysperweek;
						}
						
						int d2=-1;
						
						////////////////
						//choose a random day from those with minimum number of conflicting activities
						QList<int> candidateDays;
						
						int m=gt.rules.nInternalActivities;
						
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyDay[kk])
								if(m>_nConflActivities[kk])
									m=_nConflActivities[kk];
						
						candidateDays.clear();
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyDay[kk])
								if(m==_nConflActivities[kk])
									candidateDays.append(kk);
						
						assert(candidateDays.count()>0);
						d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						/////////////////
						
						assert(d2>=0);

						assert(_activitiesForDay[d2].count()>0);

						for(int ai2 : qAsConst(_activitiesForDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
						}
					}
				}
				else{
					assert(level==0);
					
					///getTchTimetable(tch, conflActivities[newtime]);
					///tchGetNHoursGaps(tch);

					//bool occupiedDay[MAX_DAYS_PER_WEEK];
					//bool canEmptyDay[MAX_DAYS_PER_WEEK];
					
					//int _minWrong[MAX_DAYS_PER_WEEK];
					//int _nWrong[MAX_DAYS_PER_WEEK];
					//int _nConflActivities[MAX_DAYS_PER_WEEK];
					//int _minIndexAct[MAX_DAYS_PER_WEEK];
					
					//QList<int> _activitiesForDay[MAX_DAYS_PER_WEEK];

					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						if(d2==d)
							continue;
						
						occupiedDay[d2]=false;
						canEmptyDay[d2]=true;
						
						_minWrong[d2]=INF;
						_nWrong[d2]=0;
						_nConflActivities[d2]=0;
						_minIndexAct[d2]=gt.rules.nInternalActivities;
						_activitiesForDay[d2].clear();
						
						for(int ai2 : qAsConst(teacherActivitiesOfTheDay(tch,d2))){
							if(ai2>=0){
								if(!conflActivities[newtime].contains(ai2)){
									occupiedDay[d2]=true;
									if(fixedTimeActivity[ai2] || swappedActivities[ai2])
										canEmptyDay[d2]=false;
									else if(!_activitiesForDay[d2].contains(ai2)){
										_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
										_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
										_nConflActivities[d2]++;
										_activitiesForDay[d2].append(ai2);
										assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
									}
								}
							}
						}
						
						if(!occupiedDay[d2])
							canEmptyDay[d2]=false;
					}
					occupiedDay[d]=true;
					canEmptyDay[d]=false;
					
					int nOc=0;
					bool canChooseDay=false;
					
					for(int j=0; j<gt.rules.nDaysPerWeek; j++)
						if(occupiedDay[j]){
							nOc++;
							if(canEmptyDay[j]){
								canChooseDay=true;
							}
						}
					
					if(nOc>maxDays){
						assert(nOc==maxDays+1);
						
						if(!canChooseDay){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachersmaxdaysperweek=false;
							goto impossibleteachersmaxdaysperweek;
						}
						
						int d2=-1;
						
						///////////////////
						QList<int> candidateDays;

						int _mW=INF;
						int _nW=INF;
						int _mCA=gt.rules.nInternalActivities;
						int _mIA=gt.rules.nInternalActivities;

						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyDay[kk]){
								if(_mW>_minWrong[kk] ||
								(_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
								(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
								(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
									_mW=_minWrong[kk];
									_nW=_nWrong[kk];
									_mCA=_nConflActivities[kk];
									_mIA=_minIndexAct[kk];
								}
							}
							
						assert(_mW<INF);
						
						candidateDays.clear();
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyDay[kk])
								if(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);
						
						assert(candidateDays.count()>0);
						d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						//////////////////
						
						assert(d2>=0);

						assert(_activitiesForDay[d2].count()>0);

						for(int ai2 : qAsConst(_activitiesForDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
						}
					}
				}
			}
		}
impossibleteachersmaxdaysperweek:
		if(!okteachersmaxdaysperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END teachers max days per week

/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking the teachers max three consecutive days
		////////////////////////////BEGIN max three consecutive days for teachers
		okteachersmaxthreeconsecutivedays=true;
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int tch : qAsConst(teachersWithMaxThreeConsecutiveDaysForActivities[ai])){
				if(!skipRandom(teachersMaxThreeConsecutiveDaysPercentages[tch])){
					int maxDays=3;
					bool allowExceptionAMAM=teachersMaxThreeConsecutiveDaysAllowAMAMException[tch];
					
					//preliminary test
					int _nOc=1;
					int _dstart=d, _dend=d;
					for(int d2=d-1; d2>=0; d2--){
						if(teacherActivitiesOfTheDay(tch,d2).count()>0){
							_nOc++;
							_dstart=d2;
						}
						else{
							break;
						}
					}
					for(int d2=d+1; d2<gt.rules.nDaysPerWeek; d2++){
						if(teacherActivitiesOfTheDay(tch,d2).count()>0){
							_nOc++;
							_dend=d2;
						}
						else{
							break;
						}
					}

					assert(_dstart>=0);
					assert(_dend>=0);

					if(_nOc<=maxDays || (allowExceptionAMAM && _dend-_dstart==maxDays && _dstart%2==1 && _dend%2==0))
						continue; //OK, preliminary

					if(level>0){
						occupiedDay[d]=true;
						canEmptyDay[d]=false;
					
						for(int d2=d-1; d2>=0; d2--){
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;

							_nConflActivities[d2]=0;
							_activitiesForDay[d2].clear();
							
							if(teacherActivitiesOfTheDay(tch,d2).count()>0){
								for(int ai2 : qAsConst(teacherActivitiesOfTheDay(tch,d2))){
									if(ai2>=0){
										if(!conflActivities[newtime].contains(ai2)){
											occupiedDay[d2]=true;
											if(fixedTimeActivity[ai2] || swappedActivities[ai2])
												canEmptyDay[d2]=false;
											else if(!_activitiesForDay[d2].contains(ai2)){
												_nConflActivities[d2]++;
												_activitiesForDay[d2].append(ai2);
												assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
											}
										}
									}
								}
							}

							if(!occupiedDay[d2])
								break;
						}
						
						for(int d2=d+1; d2<gt.rules.nDaysPerWeek; d2++){
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;

							_nConflActivities[d2]=0;
							_activitiesForDay[d2].clear();
							
							if(teacherActivitiesOfTheDay(tch,d2).count()>0){
								for(int ai2 : qAsConst(teacherActivitiesOfTheDay(tch,d2))){
									if(ai2>=0){
										if(!conflActivities[newtime].contains(ai2)){
											occupiedDay[d2]=true;
											if(fixedTimeActivity[ai2] || swappedActivities[ai2])
												canEmptyDay[d2]=false;
											else if(!_activitiesForDay[d2].contains(ai2)){
												_nConflActivities[d2]++;
												_activitiesForDay[d2].append(ai2);
												assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
											}
										}
									}
								}
							}

							if(!occupiedDay[d2])
								break;
						}

						for(;;){
							int dstart=d, dend=d;
							int nOc=1;

							for(int d2=d-1; d2>=0; d2--){
								if(occupiedDay[d2]){
									nOc++;
									dstart=d2;
								}
								else{
									break;
								}
							}
							for(int d2=d+1; d2<gt.rules.nDaysPerWeek; d2++){
								if(occupiedDay[d2]){
									nOc++;
									dend=d2;
								}
								else{
									break;
								}
							}

							if(nOc<=maxDays || (allowExceptionAMAM && dend-dstart==maxDays && dstart%2==1 && dend%2==0))
								break;
							
							bool canChooseDay=false;
							
							for(int j=dstart; j<=dend; j++)
								if(occupiedDay[j]){
									if(canEmptyDay[j]){
										canChooseDay=true;
									}
								}
							
							if(!canChooseDay){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okteachersmaxthreeconsecutivedays=false;
								goto impossibleteachersmaxthreeconsecutivedays;
							}
							
							int d2=-1;
							
							////////////////
							//choose a random day from those with minimum number of conflicting activities
							QList<int> candidateDays;
							
							int m=gt.rules.nInternalActivities;
							
							for(int kk=dstart; kk<=dend; kk++)
								if(occupiedDay[kk] && canEmptyDay[kk])
									if(m>_nConflActivities[kk])
										m=_nConflActivities[kk];
							
							candidateDays.clear();
							for(int kk=dstart; kk<=dend; kk++)
								if(occupiedDay[kk] && canEmptyDay[kk])
									if(m==_nConflActivities[kk])
										candidateDays.append(kk);
							
							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
							/////////////////
							
							assert(d2>=0);

							assert(_activitiesForDay[d2].count()>0);

							for(int ai2 : qAsConst(_activitiesForDay[d2])){
								assert(ai2!=ai);
								assert(!swappedActivities[ai2]);
								assert(!fixedTimeActivity[ai2]);
								assert(!conflActivities[newtime].contains(ai2));
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							}
							
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;
						}
					}
					else{
						assert(level==0);

						occupiedDay[d]=true;
						canEmptyDay[d]=false;
					
						for(int d2=d-1; d2>=0; d2--){
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;

							_minWrong[d2]=INF;
							_nWrong[d2]=0;
							_nConflActivities[d2]=0;
							_minIndexAct[d2]=gt.rules.nInternalActivities;
							_activitiesForDay[d2].clear();
							
							if(teacherActivitiesOfTheDay(tch,d2).count()>0){
								for(int ai2 : qAsConst(teacherActivitiesOfTheDay(tch,d2))){
									if(ai2>=0){
										if(!conflActivities[newtime].contains(ai2)){
											occupiedDay[d2]=true;
											if(fixedTimeActivity[ai2] || swappedActivities[ai2])
												canEmptyDay[d2]=false;
											else if(!_activitiesForDay[d2].contains(ai2)){
												_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
												_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
												_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
												_nConflActivities[d2]++;
												_activitiesForDay[d2].append(ai2);
												assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
											}
										}
									}
								}
							}

							if(!occupiedDay[d2])
								break;
						}
						
						for(int d2=d+1; d2<gt.rules.nDaysPerWeek; d2++){
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;

							_minWrong[d2]=INF;
							_nWrong[d2]=0;
							_nConflActivities[d2]=0;
							_minIndexAct[d2]=gt.rules.nInternalActivities;
							_activitiesForDay[d2].clear();
							
							if(teacherActivitiesOfTheDay(tch,d2).count()>0){
								for(int ai2 : qAsConst(teacherActivitiesOfTheDay(tch,d2))){
									if(ai2>=0){
										if(!conflActivities[newtime].contains(ai2)){
											occupiedDay[d2]=true;
											if(fixedTimeActivity[ai2] || swappedActivities[ai2])
												canEmptyDay[d2]=false;
											else if(!_activitiesForDay[d2].contains(ai2)){
												_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
												_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
												_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
												_nConflActivities[d2]++;
												_activitiesForDay[d2].append(ai2);
												assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
											}
										}
									}
								}
							}

							if(!occupiedDay[d2])
								break;
						}

						for(;;){
							int dstart=d, dend=d;
							int nOc=1;

							for(int d2=d-1; d2>=0; d2--){
								if(occupiedDay[d2]){
									nOc++;
									dstart=d2;
								}
								else{
									break;
								}
							}
							for(int d2=d+1; d2<gt.rules.nDaysPerWeek; d2++){
								if(occupiedDay[d2]){
									nOc++;
									dend=d2;
								}
								else{
									break;
								}
							}

							if(nOc<=maxDays || (allowExceptionAMAM && dend-dstart==maxDays && dstart%2==1 && dend%2==0))
								break;
							
							bool canChooseDay=false;
							
							for(int j=dstart; j<=dend; j++)
								if(occupiedDay[j]){
									if(canEmptyDay[j]){
										canChooseDay=true;
									}
								}
							
							if(!canChooseDay){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okteachersmaxthreeconsecutivedays=false;
								goto impossibleteachersmaxthreeconsecutivedays;
							}
							
							int d2=-1;
							
							////////////////
							//choose a random day from those with minimum number of conflicting activities
							QList<int> candidateDays;
							
							int _mW=INF;
							int _nW=INF;
							int _mCA=gt.rules.nInternalActivities;
							int _mIA=gt.rules.nInternalActivities;
							
							for(int kk=dstart; kk<=dend; kk++)
								if(occupiedDay[kk] && canEmptyDay[kk])
									if(_mW>_minWrong[kk] ||
									 (_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
										_mW=_minWrong[kk];
										_nW=_nWrong[kk];
										_mCA=_nConflActivities[kk];
										_mIA=_minIndexAct[kk];
									}
							
							candidateDays.clear();
							for(int kk=dstart; kk<=dend; kk++)
								if(occupiedDay[kk] && canEmptyDay[kk])
									if(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
										candidateDays.append(kk);
							
							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
							/////////////////
							
							assert(d2>=0);

							assert(_activitiesForDay[d2].count()>0);

							for(int ai2 : qAsConst(_activitiesForDay[d2])){
								assert(ai2!=ai);
								assert(!swappedActivities[ai2]);
								assert(!fixedTimeActivity[ai2]);
								assert(!conflActivities[newtime].contains(ai2));
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							}
							
							occupiedDay[d2]=false;
							canEmptyDay[d2]=true;
						}
					}
				}
			}
		}
impossibleteachersmaxthreeconsecutivedays:
		if(!okteachersmaxthreeconsecutivedays){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END teachers max three consecutive days

/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking the teachers max real days per week constraints
		////////////////////////////BEGIN max real days per week for teachers
		okteachersmaxrealdaysperweek=true;
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			//for(int tch : qAsConst(act->iTeachersList)){
			for(int tch : qAsConst(teachersWithMaxRealDaysPerWeekForActivities[ai])){
				if(skipRandom(teachersMaxRealDaysPerWeekWeightPercentages[tch]))
					continue;

				int maxDays=teachersMaxRealDaysPerWeekMaxDays[tch];
				assert(maxDays>=0); //the list contains real information

				//preliminary test
				int _nOc=0;
				for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++)
					//if(newTeachersDayNHours(tch,d2)>0)

					//IT IS VITAL TO USE teacherActivitiesOfTheDay as a QList<int> (tch,d2)!!!!!!!
					//The order of evaluation of activities is changed,
					//with activities which were moved forward and back again
					//being put at the end.
					//If you do not follow this, you'll get impossible timetables
					//for the Italian example Italy/2007/difficult/highschool-Ancona.fet or the examples from
					//South Africa: South-Africa/difficult/Collegiate_Junior_School2.fet or
					//South-Africa/difficult/Insight_Learning_Centre2.fet, I am not sure which of these 3

					if(teacherActivitiesOfTheDay(tch,2*d2).count()+teacherActivitiesOfTheDay(tch,2*d2+1).count()>0 || d2==d/2)
						_nOc++;
				if(_nOc<=maxDays)
					continue; //OK, preliminary

				if(maxDays>=0){
					assert(maxDays>0);

					//getTchTimetable(tch, conflActivities[newtime]);
					//tchGetNHoursGaps(tch);

					//bool occupiedDay[MAX_DAYS_PER_WEEK]; //should be MAX_DAYS_PER_WEEK/2, but doesn't matter
					//bool canEmptyDay[MAX_DAYS_PER_WEEK];

					//int _minWrong[MAX_DAYS_PER_WEEK];
					//int _nWrong[MAX_DAYS_PER_WEEK];
					//int _nConflActivities[MAX_DAYS_PER_WEEK];
					//int _minIndexAct[MAX_DAYS_PER_WEEK];

					//QList<int> _activitiesForDay[MAX_DAYS_PER_WEEK];

					for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
						if(d2==d/2)
							continue;

						occupiedDay[d2]=false;
						canEmptyDay[d2]=true;

						_minWrong[d2]=INF;
						_nWrong[d2]=0;
						_nConflActivities[d2]=0;
						_minIndexAct[d2]=gt.rules.nInternalActivities;
						_activitiesForDay[d2].clear();

						for(int ai2 : qAsConst(teacherActivitiesOfTheDay(tch,2*d2))){
							if(ai2>=0){
								if(!conflActivities[newtime].contains(ai2)){
									occupiedDay[d2]=true;
									if(fixedTimeActivity[ai2] || swappedActivities[ai2])
										canEmptyDay[d2]=false;
									else if(!_activitiesForDay[d2].contains(ai2)){
										_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
										_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
										_nConflActivities[d2]++;
										_activitiesForDay[d2].append(ai2);
										assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
									}
								}
							}
						}
						for(int ai2 : qAsConst(teacherActivitiesOfTheDay(tch,2*d2+1))){
							if(ai2>=0){
								if(!conflActivities[newtime].contains(ai2)){
									occupiedDay[d2]=true;
									if(fixedTimeActivity[ai2] || swappedActivities[ai2])
										canEmptyDay[d2]=false;
									else if(!_activitiesForDay[d2].contains(ai2)){
										_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
										_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
										_nConflActivities[d2]++;
										_activitiesForDay[d2].append(ai2);
										assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
									}
								}
							}
						}

						if(!occupiedDay[d2])
							canEmptyDay[d2]=false;
					}
					occupiedDay[d/2]=true;
					canEmptyDay[d/2]=false;

					int nOc=0;
					bool canChooseDay=false;

					for(int j=0; j<gt.rules.nDaysPerWeek/2; j++)
						if(occupiedDay[j]){
							nOc++;
							if(canEmptyDay[j]){
								canChooseDay=true;
							}
						}

					if(nOc>maxDays){
						assert(nOc==maxDays+1);

						if(!canChooseDay){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachersmaxrealdaysperweek=false;
							goto impossibleteachersmaxrealdaysperweek;
						}

						int d2=-1;

						if(level!=0){
							//choose random day from those with minimum number of conflicting activities
							QList<int> candidateDays;

							int m=gt.rules.nInternalActivities;

							for(int kk=0; kk<gt.rules.nDaysPerWeek/2; kk++)
								if(canEmptyDay[kk])
									if(m>_nConflActivities[kk])
										m=_nConflActivities[kk];

							candidateDays.clear();
							for(int kk=0; kk<gt.rules.nDaysPerWeek/2; kk++)
								if(canEmptyDay[kk] && m==_nConflActivities[kk])
									candidateDays.append(kk);

							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}
						else{ //level==0
							QList<int> candidateDays;

							int _mW=INF;
							int _nW=INF;
							int _mCA=gt.rules.nInternalActivities;
							int _mIA=gt.rules.nInternalActivities;

							for(int kk=0; kk<gt.rules.nDaysPerWeek/2; kk++)
								if(canEmptyDay[kk]){
									if(_mW>_minWrong[kk] ||
									 (_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
										_mW=_minWrong[kk];
										_nW=_nWrong[kk];
										_mCA=_nConflActivities[kk];
										_mIA=_minIndexAct[kk];
									}
								}

							assert(_mW<INF);

							candidateDays.clear();
							for(int kk=0; kk<gt.rules.nDaysPerWeek/2; kk++)
								if(canEmptyDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);

							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}

						assert(d2>=0);

						assert(_activitiesForDay[d2].count()>0);

						for(int ai2 : qAsConst(_activitiesForDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
						}
					}
				}
			}
		}
impossibleteachersmaxrealdaysperweek:
		if(!okteachersmaxrealdaysperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END teachers max real days per week

/////////////////////////////////////////////////////////////////////////////////////////////

		//!!!If both teachers mornings early and afternoons early are present, the code is not perfectly optimized.
		//We should integrate these with the other constraints, like for students.
		okteachersafternoonsearlymaxbeginningsatsecondhour=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			//for Said213 - 2019-08-18
			if(haveTeachersAfternoonsEarly){
				for(int tch : qAsConst(act->iTeachersList))
					if(!skipRandom(teachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[tch])){
						//preliminary check
						int mhd=1; //min hours per day
						if(teachersMinHoursDailyMinHours[tch][1]>=0)
							mhd=teachersMinHoursDailyMinHours[tch][1];
						assert(mhd>=1);
						int mhm=mhd; //min hours per morning
						if(teachersMinHoursDailyMinHours[tch][0]>=0)
							mhm=teachersMinHoursDailyMinHours[tch][0];
						assert(mhm>=mhd);

						int mhaft=mhd; //min hours per afternoon
						if(teachersMinHoursPerAfternoonMinHours[tch]>=0)
							mhaft=teachersMinHoursPerAfternoonMinHours[tch];
						assert(mhaft>=mhd);

						bool maxGapsZero=false;
						if(teachersMaxGapsPerDayMaxGaps[tch]==0 ||
						 teachersMaxGapsPerWeekMaxGaps[tch]==0 ||
						 (teachersMaxGapsPerRealDayMaxGaps[tch]==0 && teachersMaxGapsPerRealDayAllowException[tch]==false) ||
						 teachersMaxGapsPerWeekForRealDaysMaxGaps[tch]==0)
							maxGapsZero=true;

						int _nUsedMornings=0;
						int _nUsedAfternoons=0;

						int _nHours=0;
						int _nfg=0;

						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							if(d2%2==0){ //morning
								if(!maxGapsZero){
									if(newTeachersDayNHours(tch,d2)>0){
										_nHours+=max(newTeachersDayNHours(tch,d2), mhm);
										_nUsedMornings++;
									}
								}
								else{
									if(newTeachersDayNHours(tch,d2)>0){
										_nHours+=max(newTeachersDayNHours(tch,d2)+newTeachersDayNGaps(tch,d2), mhm);
										_nUsedMornings++;
									}
								}
							}
							else{ //afternoon
								if(!teacherNoGapsPerAfternoon(tch) && !maxGapsZero){
									if(newTeachersDayNHours(tch,d2)>0){
										int _nh=newTeachersDayNHours(tch,d2);
										if(newTeachersDayNFirstGaps(tch,d2)==1){
											if(_nh<mhaft){
												_nh=mhaft;
											}
											else{
												_nfg++;
											}
										}
										else if(newTeachersDayNFirstGaps(tch,d2)>=2){
											_nh++;
										}

										_nHours+=max(_nh, mhaft);
										_nUsedAfternoons++;
									}
								}
								else{
									if(newTeachersDayNHours(tch,d2)>0){
										int _nh=newTeachersDayNHours(tch,d2)+newTeachersDayNGaps(tch,d2);
										if(newTeachersDayNFirstGaps(tch,d2)==1){
											if(_nh<mhaft){
												_nh=mhaft;
											}
											else{
												_nfg++;
											}
										}
										else if(newTeachersDayNFirstGaps(tch,d2)>=2){
											_nh+=newTeachersDayNFirstGaps(tch,d2)-1;
											if(_nh<mhaft){
												_nh=mhaft;
											}
											else{
												_nfg++;
											}
										}

										_nHours+=max(_nh, mhaft);
										_nUsedAfternoons++;
									}
								}
							}
						}

						if(teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[tch]<_nfg)
							_nHours+=_nfg-teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[tch];

						if(teachersMinMorningsPerWeekMinMornings[tch]>=0)
							if(teachersMinMorningsPerWeekMinMornings[tch]>_nUsedMornings)
								_nHours+=(teachersMinMorningsPerWeekMinMornings[tch]-_nUsedMornings)*mhm;

						if(teachersMinAfternoonsPerWeekMinAfternoons[tch]>=0)
							if(teachersMinAfternoonsPerWeekMinAfternoons[tch]>_nUsedAfternoons)
								_nHours+=(teachersMinAfternoonsPerWeekMinAfternoons[tch]-_nUsedAfternoons)*mhaft;

						if(_nHours > nHoursPerTeacher[tch]){
							if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
								okteachersafternoonsearlymaxbeginningsatsecondhour=false;
								goto impossibleteachersafternoonsearlymaxbeginningsatsecondhour;
							}

							getTchTimetable(tch, conflActivities[newtime]);
							tchGetNHoursGaps(tch);

							for(;;){
								int nUsedMornings=0;
								int nUsedAfternoons=0;

								int nHours=0;
								int nfg=0;

								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(d2%2==0){ //morning
										if(!maxGapsZero){
											if(tchDayNHours[d2]>0){
												nHours+=max(tchDayNHours[d2], mhm);
												nUsedMornings++;
											}
										}
										else{
											if(tchDayNHours[d2]>0){
												nHours+=max(tchDayNHours[d2]+tchDayNGaps[d2], mhm);
												nUsedMornings++;
											}
										}
									}
									else{ //afternoon
										if(!teacherNoGapsPerAfternoon(tch) && !maxGapsZero){
											if(tchDayNHours[d2]>0){
												int nh=tchDayNHours[d2];
												if(tchDayNFirstGaps[d2]==1){
													if(nh<mhaft){
														nh=mhaft;
													}
													else{
														nfg++;
													}
												}
												else if(tchDayNFirstGaps[d2]>=2){
													nh++;
												}

												nHours+=max(nh, mhaft);
												nUsedAfternoons++;
											}
										}
										else{
											if(tchDayNHours[d2]>0){
												int nh=tchDayNHours[d2]+tchDayNGaps[d2];
												if(tchDayNFirstGaps[d2]==1){
													if(nh<mhaft){
														nh=mhaft;
													}
													else{
														nfg++;
													}
												}
												else if(tchDayNFirstGaps[d2]>=2){
													nh+=tchDayNFirstGaps[d2]-1;
													if(nh<mhaft){
														nh=mhaft;
													}
													else{
														nfg++;
													}
												}

												nHours+=max(nh, mhaft);
												nUsedAfternoons++;
											}
										}
									}
								}

								if(teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[tch]<nfg)
									nHours+=nfg-teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[tch];

								if(teachersMinMorningsPerWeekMinMornings[tch]>=0)
									if(teachersMinMorningsPerWeekMinMornings[tch]>nUsedMornings)
										nHours+=(teachersMinMorningsPerWeekMinMornings[tch]-nUsedMornings)*mhm;

								if(teachersMinAfternoonsPerWeekMinAfternoons[tch]>=0)
									if(teachersMinAfternoonsPerWeekMinAfternoons[tch]>nUsedAfternoons)
										nHours+=(teachersMinAfternoonsPerWeekMinAfternoons[tch]-nUsedAfternoons)*mhaft;

								int ai2=-1;

								if(nHours > nHoursPerTeacher[tch]){
									//remove an activity
									bool k=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									if(!k){
										if(maxGapsZero){
											okteachersafternoonsearlymaxbeginningsatsecondhour=false;
											goto impossibleteachersafternoonsearlymaxbeginningsatsecondhour;
										}
										else{
											bool ka=teacherRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
											assert(conflActivities[newtime].count()==nConflActivities[newtime]);

											if(!ka){
												okteachersafternoonsearlymaxbeginningsatsecondhour=false;
												goto impossibleteachersafternoonsearlymaxbeginningsatsecondhour;
											}
										}
									}
								}
								else{ //OK
									break;
								}

								assert(ai2>=0);

								removeAi2FromTchTimetable(ai2);
								updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
							}
						}
					}
			}
		}

impossibleteachersafternoonsearlymaxbeginningsatsecondhour:
		if(!okteachersafternoonsearlymaxbeginningsatsecondhour){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END teachers afternoons early

/////////////////////////////////////////////////////////////////////////////////////////////

		//!!!If both teachers mornings early and afternoons early are present, the code is not perfectly optimized.
		//We should integrate these with the other constraints, like for students.
		okteachersmorningsearlymaxbeginningsatsecondhour=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(haveTeachersMorningsEarly){
				for(int tch : qAsConst(act->iTeachersList))
					if(!skipRandom(teachersMorningsEarlyMaxBeginningsAtSecondHourPercentage[tch])){
						//preliminary check
						int mhd=1; //min hours per day
						if(teachersMinHoursDailyMinHours[tch][1]>=0)
							mhd=teachersMinHoursDailyMinHours[tch][1];
						assert(mhd>=1);
						int mhm=mhd; //min hours per morning
						if(teachersMinHoursDailyMinHours[tch][0]>=0)
							mhm=teachersMinHoursDailyMinHours[tch][0];
						assert(mhm>=mhd);

						int mhaft=mhd; //min hours per afternoon
						if(teachersMinHoursPerAfternoonMinHours[tch]>=0)
							mhaft=teachersMinHoursPerAfternoonMinHours[tch];
						assert(mhaft>=mhd);

						bool maxGapsZero=false;
						if(teachersMaxGapsPerDayMaxGaps[tch]==0 ||
						 teachersMaxGapsPerWeekMaxGaps[tch]==0 ||
						 (teachersMaxGapsPerRealDayMaxGaps[tch]==0 && teachersMaxGapsPerRealDayAllowException[tch]==false) ||
						 teachersMaxGapsPerWeekForRealDaysMaxGaps[tch]==0)
							maxGapsZero=true;

						int _nUsedMornings=0;
						int _nUsedAfternoons=0;

						int _nHours=0;
						int _nfg=0;

						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							if(d2%2==0){ //morning
								if(!maxGapsZero){
									if(newTeachersDayNHours(tch,d2)>0){
										int _nh=newTeachersDayNHours(tch,d2);
										if(newTeachersDayNFirstGaps(tch,d2)==1){
											if(_nh<mhm){
												_nh=mhm;
											}
											else{
												_nfg++;
											}
										}
										else if(newTeachersDayNFirstGaps(tch,d2)>=2){
											_nh++;
										}

										_nHours+=max(_nh, mhm);
										_nUsedMornings++;
									}
								}
								else{
									if(newTeachersDayNHours(tch,d2)>0){
										int _nh=newTeachersDayNHours(tch,d2)+newTeachersDayNGaps(tch,d2);
										if(newTeachersDayNFirstGaps(tch,d2)==1){
											if(_nh<mhm){
												_nh=mhm;
											}
											else{
												_nfg++;
											}
										}
										else if(newTeachersDayNFirstGaps(tch,d2)>=2){
											_nh+=newTeachersDayNFirstGaps(tch,d2)-1;
											if(_nh<mhm){
												_nh=mhm;
											}
											else{
												_nfg++;
											}
										}

										_nHours+=max(_nh, mhm);
										_nUsedMornings++;
									}
								}
							}
							else{ //afternoon
								if(!teacherNoGapsPerAfternoon(tch) && !maxGapsZero){
									if(newTeachersDayNHours(tch,d2)>0){
										_nHours+=max(newTeachersDayNHours(tch,d2), mhaft);
										_nUsedAfternoons++;
									}
								}
								else{
									if(newTeachersDayNHours(tch,d2)>0){
										_nHours+=max(newTeachersDayNHours(tch,d2)+newTeachersDayNGaps(tch,d2), mhaft);
										_nUsedAfternoons++;
									}
								}
							}
						}

						if(teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[tch]<_nfg)
							_nHours+=_nfg-teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[tch];

						if(teachersMinMorningsPerWeekMinMornings[tch]>=0)
							if(teachersMinMorningsPerWeekMinMornings[tch]>_nUsedMornings)
								_nHours+=(teachersMinMorningsPerWeekMinMornings[tch]-_nUsedMornings)*mhm;

						if(teachersMinAfternoonsPerWeekMinAfternoons[tch]>=0)
							if(teachersMinAfternoonsPerWeekMinAfternoons[tch]>_nUsedAfternoons)
								_nHours+=(teachersMinAfternoonsPerWeekMinAfternoons[tch]-_nUsedAfternoons)*mhaft;

						if(_nHours > nHoursPerTeacher[tch]){
							if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
								okteachersmorningsearlymaxbeginningsatsecondhour=false;
								goto impossibleteachersmorningsearlymaxbeginningsatsecondhour;
							}

							getTchTimetable(tch, conflActivities[newtime]);
							tchGetNHoursGaps(tch);

							for(;;){
								int nUsedMornings=0;
								int nUsedAfternoons=0;

								int nHours=0;
								int nfg=0;

								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(d2%2==0){ //morning
										if(!maxGapsZero){
											if(tchDayNHours[d2]>0){
												int nh=tchDayNHours[d2];
												if(tchDayNFirstGaps[d2]==1){
													if(nh<mhm){
														nh=mhm;
													}
													else{
														nfg++;
													}
												}
												else if(tchDayNFirstGaps[d2]>=2){
													nh++;
												}

												nHours+=max(nh, mhm);
												nUsedMornings++;
											}
										}
										else{
											if(tchDayNHours[d2]>0){
												int nh=tchDayNHours[d2]+tchDayNGaps[d2];
												if(tchDayNFirstGaps[d2]==1){
													if(nh<mhm){
														nh=mhm;
													}
													else{
														nfg++;
													}
												}
												else if(tchDayNFirstGaps[d2]>=2){
													nh+=tchDayNFirstGaps[d2]-1;
													if(nh<mhm){
														nh=mhm;
													}
													else{
														nfg++;
													}
												}

												nHours+=max(nh, mhm);
												nUsedMornings++;
											}
										}
									}
									else{ //afternoon
										if(!teacherNoGapsPerAfternoon(tch) && !maxGapsZero){
											if(tchDayNHours[d2]>0){
												nHours+=max(tchDayNHours[d2], mhaft);
												nUsedAfternoons++;
											}
										}
										else{
											if(tchDayNHours[d2]>0){
												nHours+=max(tchDayNHours[d2]+tchDayNGaps[d2], mhaft);
												nUsedAfternoons++;
											}
										}
									}
								}

								if(teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[tch]<nfg)
									nHours+=nfg-teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[tch];

								if(teachersMinMorningsPerWeekMinMornings[tch]>=0)
									if(teachersMinMorningsPerWeekMinMornings[tch]>nUsedMornings)
										nHours+=(teachersMinMorningsPerWeekMinMornings[tch]-nUsedMornings)*mhm;

								if(teachersMinAfternoonsPerWeekMinAfternoons[tch]>=0)
									if(teachersMinAfternoonsPerWeekMinAfternoons[tch]>nUsedAfternoons)
										nHours+=(teachersMinAfternoonsPerWeekMinAfternoons[tch]-nUsedAfternoons)*mhaft;

								int ai2=-1;

								if(nHours > nHoursPerTeacher[tch]){
									//remove an activity
									bool k=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									if(!k){
										if(maxGapsZero){
											okteachersmorningsearlymaxbeginningsatsecondhour=false;
											goto impossibleteachersmorningsearlymaxbeginningsatsecondhour;
										}
										else{
											bool ka=teacherRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
											assert(conflActivities[newtime].count()==nConflActivities[newtime]);

											if(!ka){
												okteachersmorningsearlymaxbeginningsatsecondhour=false;
												goto impossibleteachersmorningsearlymaxbeginningsatsecondhour;
											}
										}
									}
								}
								else{ //OK
									break;
								}

								assert(ai2>=0);

								removeAi2FromTchTimetable(ai2);
								updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
							}
						}
					}
			}
		}

impossibleteachersmorningsearlymaxbeginningsatsecondhour:
		if(!okteachersmorningsearlymaxbeginningsatsecondhour){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END teachers mornings early

/////////////////////////////////////////////////////////////////////////////////////////////

		//If both teachers mornings early and afternoons early are present, we can do a combined better check.
		okteachersmorningsafternoonsearlymaxbeginningsatsecondhour=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(haveTeachersAfternoonsEarly && haveTeachersMorningsEarly){
				for(int tch : qAsConst(act->iTeachersList))
					if(!skipRandom(teachersAfternoonsEarlyMaxBeginningsAtSecondHourPercentage[tch]) && !skipRandom(teachersMorningsEarlyMaxBeginningsAtSecondHourPercentage[tch])){
						//preliminary check
						int mhd=1; //min hours per day
						if(teachersMinHoursDailyMinHours[tch][1]>=0)
							mhd=teachersMinHoursDailyMinHours[tch][1];
						assert(mhd>=1);
						int mhm=mhd; //min hours per morning
						if(teachersMinHoursDailyMinHours[tch][0]>=0)
							mhm=teachersMinHoursDailyMinHours[tch][0];
						assert(mhm>=mhd);

						int mhaft=mhd; //min hours per afternoon
						if(teachersMinHoursPerAfternoonMinHours[tch]>=0)
							mhaft=teachersMinHoursPerAfternoonMinHours[tch];
						assert(mhaft>=mhd);

						bool maxGapsZero=false;
						if(teachersMaxGapsPerDayMaxGaps[tch]==0 ||
						 teachersMaxGapsPerWeekMaxGaps[tch]==0 ||
						 (teachersMaxGapsPerRealDayMaxGaps[tch]==0 && teachersMaxGapsPerRealDayAllowException[tch]==false) ||
						 teachersMaxGapsPerWeekForRealDaysMaxGaps[tch]==0)
							maxGapsZero=true;

						int _nUsedMornings=0;
						int _nUsedAfternoons=0;

						int _nHours=0;
						int _nfg=0;

						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							if(d2%2==0){ //morning
								if(!maxGapsZero){
									if(newTeachersDayNHours(tch,d2)>0){
										int _nh=newTeachersDayNHours(tch,d2);
										if(newTeachersDayNFirstGaps(tch,d2)==1){
											if(_nh<mhm){
												_nh=mhm;
											}
											else{
												_nfg++;
											}
										}
										else if(newTeachersDayNFirstGaps(tch,d2)>=2){
											_nh++;
										}

										_nHours+=max(_nh, mhm);
										_nUsedMornings++;
									}
								}
								else{
									if(newTeachersDayNHours(tch,d2)>0){
										int _nh=newTeachersDayNHours(tch,d2)+newTeachersDayNGaps(tch,d2);
										if(newTeachersDayNFirstGaps(tch,d2)==1){
											if(_nh<mhm){
												_nh=mhm;
											}
											else{
												_nfg++;
											}
										}
										else if(newTeachersDayNFirstGaps(tch,d2)>=2){
											_nh+=newTeachersDayNFirstGaps(tch,d2)-1;
											if(_nh<mhm){
												_nh=mhm;
											}
											else{
												_nfg++;
											}
										}

										_nHours+=max(_nh, mhm);
										_nUsedMornings++;
									}
								}
							}
							else{ //afternoon
								if(!teacherNoGapsPerAfternoon(tch) && !maxGapsZero){
									if(newTeachersDayNHours(tch,d2)>0){
										int _nh=newTeachersDayNHours(tch,d2);
										if(newTeachersDayNFirstGaps(tch,d2)==1){
											if(_nh<mhaft){
												_nh=mhaft;
											}
											else{
												_nfg++;
											}
										}
										else if(newTeachersDayNFirstGaps(tch,d2)>=2){
											_nh++;
										}

										_nHours+=max(_nh, mhaft);
										_nUsedAfternoons++;
									}
								}
								else{
									if(newTeachersDayNHours(tch,d2)>0){
										int _nh=newTeachersDayNHours(tch,d2)+newTeachersDayNGaps(tch,d2);
										if(newTeachersDayNFirstGaps(tch,d2)==1){
											if(_nh<mhaft){
												_nh=mhaft;
											}
											else{
												_nfg++;
											}
										}
										else if(newTeachersDayNFirstGaps(tch,d2)>=2){
											_nh+=newTeachersDayNFirstGaps(tch,d2)-1;
											if(_nh<mhaft){
												_nh=mhaft;
											}
											else{
												_nfg++;
											}
										}

										_nHours+=max(_nh, mhaft);
										_nUsedAfternoons++;
									}
								}
							}
						}

						if(teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[tch]+
						 teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[tch] < _nfg)
							_nHours+=_nfg-teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[tch]-teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[tch];

						if(teachersMinMorningsPerWeekMinMornings[tch]>=0)
							if(teachersMinMorningsPerWeekMinMornings[tch]>_nUsedMornings)
								_nHours+=(teachersMinMorningsPerWeekMinMornings[tch]-_nUsedMornings)*mhm;

						if(teachersMinAfternoonsPerWeekMinAfternoons[tch]>=0)
							if(teachersMinAfternoonsPerWeekMinAfternoons[tch]>_nUsedAfternoons)
								_nHours+=(teachersMinAfternoonsPerWeekMinAfternoons[tch]-_nUsedAfternoons)*mhaft;

						if(_nHours > nHoursPerTeacher[tch]){
							if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
								okteachersmorningsafternoonsearlymaxbeginningsatsecondhour=false;
								goto impossibleteachersmorningsafternoonsearlymaxbeginningsatsecondhour;
							}

							getTchTimetable(tch, conflActivities[newtime]);
							tchGetNHoursGaps(tch);

							for(;;){
								int nUsedMornings=0;
								int nUsedAfternoons=0;

								int nHours=0;
								int nfg=0;

								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(d2%2==0){ //morning
										if(!maxGapsZero){
											if(tchDayNHours[d2]>0){
												int nh=tchDayNHours[d2];
												if(tchDayNFirstGaps[d2]==1){
													if(nh<mhm){
														nh=mhm;
													}
													else{
														nfg++;
													}
												}
												else if(tchDayNFirstGaps[d2]>=2){
													nh++;
												}

												nHours+=max(nh, mhm);
												nUsedMornings++;
											}
										}
										else{
											if(tchDayNHours[d2]>0){
												int nh=tchDayNHours[d2]+tchDayNGaps[d2];
												if(tchDayNFirstGaps[d2]==1){
													if(nh<mhm){
														nh=mhm;
													}
													else{
														nfg++;
													}
												}
												else if(tchDayNFirstGaps[d2]>=2){
													nh+=tchDayNFirstGaps[d2]-1;
													if(nh<mhm){
														nh=mhm;
													}
													else{
														nfg++;
													}
												}

												nHours+=max(nh, mhm);
												nUsedMornings++;
											}
										}
									}
									else{ //afternoon
										if(!teacherNoGapsPerAfternoon(tch) && !maxGapsZero){
											if(tchDayNHours[d2]>0){
												int nh=tchDayNHours[d2];
												if(tchDayNFirstGaps[d2]==1){
													if(nh<mhaft){
														nh=mhaft;
													}
													else{
														nfg++;
													}
												}
												else if(tchDayNFirstGaps[d2]>=2){
													nh++;
												}

												nHours+=max(nh, mhaft);
												nUsedAfternoons++;
											}
										}
										else{
											if(tchDayNHours[d2]>0){
												int nh=tchDayNHours[d2]+tchDayNGaps[d2];
												if(tchDayNFirstGaps[d2]==1){
													if(nh<mhaft){
														nh=mhaft;
													}
													else{
														nfg++;
													}
												}
												else if(tchDayNFirstGaps[d2]>=2){
													nh+=tchDayNFirstGaps[d2]-1;
													if(nh<mhaft){
														nh=mhaft;
													}
													else{
														nfg++;
													}
												}

												nHours+=max(nh, mhaft);
												nUsedAfternoons++;
											}
										}
									}
								}

								if(teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[tch]+
								 teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[tch]<nfg)
									nHours+=nfg-teachersAfternoonsEarlyMaxBeginningsAtSecondHourMaxBeginnings[tch]-teachersMorningsEarlyMaxBeginningsAtSecondHourMaxBeginnings[tch];

								if(teachersMinMorningsPerWeekMinMornings[tch]>=0)
									if(teachersMinMorningsPerWeekMinMornings[tch]>nUsedMornings)
										nHours+=(teachersMinMorningsPerWeekMinMornings[tch]-nUsedMornings)*mhm;

								if(teachersMinAfternoonsPerWeekMinAfternoons[tch]>=0)
									if(teachersMinAfternoonsPerWeekMinAfternoons[tch]>nUsedAfternoons)
										nHours+=(teachersMinAfternoonsPerWeekMinAfternoons[tch]-nUsedAfternoons)*mhaft;

								int ai2=-1;

								if(nHours > nHoursPerTeacher[tch]){
									//remove an activity
									bool k=teacherRemoveAnActivityFromEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									if(!k){
										if(maxGapsZero){
											okteachersmorningsafternoonsearlymaxbeginningsatsecondhour=false;
											goto impossibleteachersmorningsafternoonsearlymaxbeginningsatsecondhour;
										}
										else{
											bool ka=teacherRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
											assert(conflActivities[newtime].count()==nConflActivities[newtime]);

											if(!ka){
												okteachersmorningsafternoonsearlymaxbeginningsatsecondhour=false;
												goto impossibleteachersmorningsafternoonsearlymaxbeginningsatsecondhour;
											}
										}
									}
								}
								else{ //OK
									break;
								}

								assert(ai2>=0);

								removeAi2FromTchTimetable(ai2);
								updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
							}
						}
					}
			}
		}

impossibleteachersmorningsafternoonsearlymaxbeginningsatsecondhour:
		if(!okteachersmorningsafternoonsearlymaxbeginningsatsecondhour){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END teachers mornings+afternoons early

/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking the teacher max afternoons per week constraints
		////////////////////////////BEGIN max afternoons per week for teachers
		okteachermaxafternoonsperweek=true;
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			//for(int tch : qAsConst(act->iTeachersList)){
			for(int tch : qAsConst(teachersWithMaxAfternoonsPerWeekForActivities[ai])){
				if(skipRandom(teachersMaxAfternoonsPerWeekWeightPercentages[tch]))
					continue;

				int maxAfternoons=teachersMaxAfternoonsPerWeekMaxAfternoons[tch];
				assert(maxAfternoons>=0); //the list contains real information

				//preliminary test
				int _nOc=0;
				for(int d2=1; d2<gt.rules.nDaysPerWeek; d2+=2)
					//if(newTeachersDayNHours(tch,d2)>0)

					//IT IS VITAL TO USE teacherActivitiesOfTheDay as a QList<int> (tch,d2)!!!!!!!
					//The order of evaluation of activities is changed,
					//with activities which were moved forward and back again
					//being put at the end.
					//If you do not follow this, you'll get impossible timetables
					//for the Italian example Italy/2007/difficult/highschool-Ancona.fet or the examples from
					//South Africa: South-Africa/difficult/Collegiate_Junior_School2.fet or
					//South-Africa/difficult/Insight_Learning_Centre2.fet, I am not sure which of these 3

					if(teacherActivitiesOfTheDay(tch,d2).count()>0 || d2==d)
						_nOc++;
				if(_nOc<=maxAfternoons)
					continue; //OK, preliminary

				if(maxAfternoons>=0){
					assert(maxAfternoons>0);

					//getTchTimetable(tch, conflActivities[newtime]);
					//tchGetNHoursGaps(tch);

					//bool occupiedDay[MAX_DAYS_PER_WEEK];
					//bool canEmptyDay[MAX_DAYS_PER_WEEK];

					//int _minWrong[MAX_DAYS_PER_WEEK];
					//int _nWrong[MAX_DAYS_PER_WEEK];
					//int _nConflActivities[MAX_DAYS_PER_WEEK];
					//int _minIndexAct[MAX_DAYS_PER_WEEK];

					//QList<int> _activitiesForDay[MAX_DAYS_PER_WEEK];

					for(int d2=1; d2<gt.rules.nDaysPerWeek; d2+=2){
						if(d2==d)
							continue;

						occupiedDay[d2]=false;
						canEmptyDay[d2]=true;

						_minWrong[d2]=INF;
						_nWrong[d2]=0;
						_nConflActivities[d2]=0;
						_minIndexAct[d2]=gt.rules.nInternalActivities;
						_activitiesForDay[d2].clear();

						for(int ai2 : qAsConst(teacherActivitiesOfTheDay(tch,d2))){
							if(ai2>=0){
								if(!conflActivities[newtime].contains(ai2)){
									occupiedDay[d2]=true;
									if(fixedTimeActivity[ai2] || swappedActivities[ai2])
										canEmptyDay[d2]=false;
									else if(!_activitiesForDay[d2].contains(ai2)){
										_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
										_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
										_nConflActivities[d2]++;
										_activitiesForDay[d2].append(ai2);
										assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
									}
								}
							}
						}

						if(!occupiedDay[d2])
							canEmptyDay[d2]=false;
					}
					occupiedDay[d]=true;
					canEmptyDay[d]=false;

					int nOc=0;
					bool canChooseDay=false;

					for(int j=1; j<gt.rules.nDaysPerWeek; j+=2)
						if(occupiedDay[j]){
							nOc++;
							if(canEmptyDay[j]){
								canChooseDay=true;
							}
						}

					if(nOc>maxAfternoons){
						assert(nOc==maxAfternoons+1);

						if(!canChooseDay){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachermaxafternoonsperweek=false;
							goto impossibleteachermaxafternoonsperweek;
						}

						int d2=-1;

						if(level!=0){
							//choose random day from those with minimum number of conflicting activities
							QList<int> candidateDays;

							int m=gt.rules.nInternalActivities;

							for(int kk=1; kk<gt.rules.nDaysPerWeek; kk+=2)
								if(canEmptyDay[kk])
									if(m>_nConflActivities[kk])
										m=_nConflActivities[kk];

							candidateDays.clear();
							for(int kk=1; kk<gt.rules.nDaysPerWeek; kk+=2)
								if(canEmptyDay[kk] && m==_nConflActivities[kk])
									candidateDays.append(kk);

							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}
						else{ //level==0
							QList<int> candidateDays;

							int _mW=INF;
							int _nW=INF;
							int _mCA=gt.rules.nInternalActivities;
							int _mIA=gt.rules.nInternalActivities;

							for(int kk=1; kk<gt.rules.nDaysPerWeek; kk+=2)
								if(canEmptyDay[kk]){
									if(_mW>_minWrong[kk] ||
									 (_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
										_mW=_minWrong[kk];
										_nW=_nWrong[kk];
										_mCA=_nConflActivities[kk];
										_mIA=_minIndexAct[kk];
									}
								}

							assert(_mW<INF);

							candidateDays.clear();
							for(int kk=1; kk<gt.rules.nDaysPerWeek; kk+=2)
								if(canEmptyDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);

							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}

						assert(d2>=0);

						assert(_activitiesForDay[d2].count()>0);

						for(int ai2 : qAsConst(_activitiesForDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
						}
					}
				}
			}
		}
impossibleteachermaxafternoonsperweek:
		if(!okteachermaxafternoonsperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END teachers max afternoons per week

/////////////////////////////////////////////////////////////////////////////////////////////

		//not breaking the teacher max mornings per week constraints
		////////////////////////////BEGIN max mornings per week for teachers
		okteachermaxmorningsperweek=true;
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			//for(int tch : qAsConst(act->iTeachersList)){
			for(int tch : qAsConst(teachersWithMaxMorningsPerWeekForActivities[ai])){
				if(skipRandom(teachersMaxMorningsPerWeekWeightPercentages[tch]))
					continue;

				int maxMornings=teachersMaxMorningsPerWeekMaxMornings[tch];
				assert(maxMornings>=0); //the list contains real information

				//preliminary test
				int _nOc=0;
				for(int d2=0; d2<gt.rules.nDaysPerWeek; d2+=2)
					//if(newTeachersDayNHours(tch,d2)>0)

					//IT IS VITAL TO USE teacherActivitiesOfTheDay as a QList<int> (tch,d2)!!!!!!!
					//The order of evaluation of activities is changed,
					//with activities which were moved forward and back again
					//being put at the end.
					//If you do not follow this, you'll get impossible timetables
					//for the Italian example Italy/2007/difficult/highschool-Ancona.fet or the examples from
					//South Africa: South-Africa/difficult/Collegiate_Junior_School2.fet or
					//South-Africa/difficult/Insight_Learning_Centre2.fet, I am not sure which of these 3

					if(teacherActivitiesOfTheDay(tch,d2).count()>0 || d2==d)
						_nOc++;
				if(_nOc<=maxMornings)
					continue; //OK, preliminary

				if(maxMornings>=0){
					assert(maxMornings>0);

					//getTchTimetable(tch, conflActivities[newtime]);
					//tchGetNHoursGaps(tch);

					//bool occupiedDay[MAX_DAYS_PER_WEEK];
					//bool canEmptyDay[MAX_DAYS_PER_WEEK];

					//int _minWrong[MAX_DAYS_PER_WEEK];
					//int _nWrong[MAX_DAYS_PER_WEEK];
					//int _nConflActivities[MAX_DAYS_PER_WEEK];
					//int _minIndexAct[MAX_DAYS_PER_WEEK];

					//QList<int> _activitiesForDay[MAX_DAYS_PER_WEEK];

					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2+=2){
						if(d2==d)
							continue;

						occupiedDay[d2]=false;
						canEmptyDay[d2]=true;

						_minWrong[d2]=INF;
						_nWrong[d2]=0;
						_nConflActivities[d2]=0;
						_minIndexAct[d2]=gt.rules.nInternalActivities;
						_activitiesForDay[d2].clear();

						for(int ai2 : qAsConst(teacherActivitiesOfTheDay(tch,d2))){
							if(ai2>=0){
								if(!conflActivities[newtime].contains(ai2)){
									occupiedDay[d2]=true;
									if(fixedTimeActivity[ai2] || swappedActivities[ai2])
										canEmptyDay[d2]=false;
									else if(!_activitiesForDay[d2].contains(ai2)){
										_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
										_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
										_nConflActivities[d2]++;
										_activitiesForDay[d2].append(ai2);
										assert(_nConflActivities[d2]==_activitiesForDay[d2].count());
									}
								}
							}
						}

						if(!occupiedDay[d2])
							canEmptyDay[d2]=false;
					}
					occupiedDay[d]=true;
					canEmptyDay[d]=false;

					int nOc=0;
					bool canChooseDay=false;

					for(int j=0; j<gt.rules.nDaysPerWeek; j+=2)
						if(occupiedDay[j]){
							nOc++;
							if(canEmptyDay[j]){
								canChooseDay=true;
							}
						}

					if(nOc>maxMornings){
						assert(nOc==maxMornings+1);

						if(!canChooseDay){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachermaxmorningsperweek=false;
							goto impossibleteachermaxmorningsperweek;
						}

						int d2=-1;

						if(level!=0){
							//choose random day from those with minimum number of conflicting activities
							QList<int> candidateDays;

							int m=gt.rules.nInternalActivities;

							for(int kk=0; kk<gt.rules.nDaysPerWeek; kk+=2)
								if(canEmptyDay[kk])
									if(m>_nConflActivities[kk])
										m=_nConflActivities[kk];

							candidateDays.clear();
							for(int kk=0; kk<gt.rules.nDaysPerWeek; kk+=2)
								if(canEmptyDay[kk] && m==_nConflActivities[kk])
									candidateDays.append(kk);

							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}
						else{ //level==0
							QList<int> candidateDays;

							int _mW=INF;
							int _nW=INF;
							int _mCA=gt.rules.nInternalActivities;
							int _mIA=gt.rules.nInternalActivities;

							for(int kk=0; kk<gt.rules.nDaysPerWeek; kk+=2)
								if(canEmptyDay[kk]){
									if(_mW>_minWrong[kk] ||
									 (_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
									 (_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
										_mW=_minWrong[kk];
										_nW=_nWrong[kk];
										_mCA=_nConflActivities[kk];
										_mIA=_minIndexAct[kk];
									}
								}

							assert(_mW<INF);

							candidateDays.clear();
							for(int kk=0; kk<gt.rules.nDaysPerWeek; kk+=2)
								if(canEmptyDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);

							assert(candidateDays.count()>0);
							d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						}

						assert(d2>=0);

						assert(_activitiesForDay[d2].count()>0);

						for(int ai2 : qAsConst(_activitiesForDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
						}
					}
				}
			}
		}
impossibleteachermaxmorningsperweek:
		if(!okteachermaxmorningsperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		////////////////////////////END teachers max mornings per week

/////////////////////////////////////////////////////////////////////////////////////////////
		//BEGIN teachers interval max days per week

		okteachersintervalmaxdaysperweek=true;
		for(int tch : qAsConst(act->iTeachersList)){
			for(int cnt=0; cnt<teachersIntervalMaxDaysPerWeekPercentages[tch].count(); cnt++){
				double perc=teachersIntervalMaxDaysPerWeekPercentages[tch].at(cnt);
				int maxDays=teachersIntervalMaxDaysPerWeekMaxDays[tch].at(cnt);
				int sth=teachersIntervalMaxDaysPerWeekIntervalStart[tch].at(cnt);
				int endh=teachersIntervalMaxDaysPerWeekIntervalEnd[tch].at(cnt);

				assert(perc>=0);
				assert(sth>=0 && sth<gt.rules.nHoursPerDay);
				assert(endh>sth && endh<=gt.rules.nHoursPerDay);
				assert(maxDays>=0 && maxDays<=gt.rules.nDaysPerWeek);
				
				if(skipRandom(perc))
					continue;
				
				assert(perc==100.0);
				
				bool foundothers=false;
				bool foundai=false;
				for(int hh=sth; hh<endh; hh++){
					if(newTeachersTimetable(tch,d,hh)==ai){
						foundai=true;
					}
					else{
						assert(newTeachersTimetable(tch,d,hh)==teachersTimetable(tch,d,hh));
						if(newTeachersTimetable(tch,d,hh)>=0){
							if(!conflActivities[newtime].contains(newTeachersTimetable(tch,d,hh))){
								foundothers=true;
							}
						}
					}
				}
				int nrotherdays=0;
				for(int dd=0; dd<gt.rules.nDaysPerWeek; dd++){
					if(dd!=d){
						for(int hh=sth; hh<endh; hh++){
							assert(newTeachersTimetable(tch,dd,hh)==teachersTimetable(tch,dd,hh));
							if(newTeachersTimetable(tch,dd,hh)>=0 && !conflActivities[newtime].contains(newTeachersTimetable(tch,dd,hh))){
								nrotherdays++;
								break;
							}
						}
					}
				}
				assert(nrotherdays<=maxDays); //if percentage==100%, then it is impossible to break this constraint
				if((foundai && !foundothers) && nrotherdays==maxDays){ //increased above limit
					if(level>0){
						//bool occupiedIntervalDay[MAX_DAYS_PER_WEEK];
						//bool canEmptyIntervalDay[MAX_DAYS_PER_WEEK];
				
						///int _minWrong[MAX_DAYS_PER_WEEK];
						///int _nWrong[MAX_DAYS_PER_WEEK];
						//int _nConflActivities[MAX_DAYS_PER_WEEK];
						///int _minIndexAct[MAX_DAYS_PER_WEEK];
				
						//QList<int> _activitiesForIntervalDay[MAX_DAYS_PER_WEEK];

						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							if(d2==d)
								continue;
				
							occupiedIntervalDay[d2]=false;
							canEmptyIntervalDay[d2]=true;
					
							//_minWrong[d2]=INF;
							//_nWrong[d2]=0;
							_nConflActivities[d2]=0;
							//_minIndexAct[d2]=gt.rules.nInternalActivities;
							_activitiesForIntervalDay[d2].clear();
							
							for(int h2=sth; h2<endh; h2++){
								int ai2=teachersTimetable(tch,d2,h2);
								if(ai2>=0){
									if(!conflActivities[newtime].contains(ai2)){
										occupiedIntervalDay[d2]=true;
										if(fixedTimeActivity[ai2] || swappedActivities[ai2])
											canEmptyIntervalDay[d2]=false;
										else if(!_activitiesForIntervalDay[d2].contains(ai2)){
											//_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
											//_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
											//_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
											_nConflActivities[d2]++;
											_activitiesForIntervalDay[d2].append(ai2);
											assert(_nConflActivities[d2]==_activitiesForIntervalDay[d2].count());
										}
									}
								}
							}
					
							if(!occupiedIntervalDay[d2])
								canEmptyIntervalDay[d2]=false;
						}
						occupiedIntervalDay[d]=true;
						canEmptyIntervalDay[d]=false;
				
						int nOc=0;
						bool canChooseDay=false;
				
						for(int j=0; j<gt.rules.nDaysPerWeek; j++)
							if(occupiedIntervalDay[j]){
								nOc++;
								if(canEmptyIntervalDay[j]){
									canChooseDay=true;
								}
							}
						
						assert(nOc==maxDays+1);
					
						if(!canChooseDay){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachersintervalmaxdaysperweek=false;
							goto impossibleteachersintervalmaxdaysperweek;
						}
					
						int d2=-1;
					
						///////////////
						//choose a random day from those with minimum number of conflicting activities
						QList<int> candidateDays;
					
						int m=gt.rules.nInternalActivities;
						
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyIntervalDay[kk])
								if(m>_nConflActivities[kk])
									m=_nConflActivities[kk];
					
						candidateDays.clear();
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyIntervalDay[kk])
								if(m==_nConflActivities[kk])
									candidateDays.append(kk);
								
						assert(candidateDays.count()>0);
						d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						///////////////
					
						assert(d2>=0);

						assert(_activitiesForIntervalDay[d2].count()>0);

						for(int ai2 : qAsConst(_activitiesForIntervalDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
						}
					}
					else{
						assert(level==0);
						
						//bool occupiedIntervalDay[MAX_DAYS_PER_WEEK];
						//bool canEmptyIntervalDay[MAX_DAYS_PER_WEEK];
				
						//int _minWrong[MAX_DAYS_PER_WEEK];
						//int _nWrong[MAX_DAYS_PER_WEEK];
						//int _nConflActivities[MAX_DAYS_PER_WEEK];
						//int _minIndexAct[MAX_DAYS_PER_WEEK];
				
						//QList<int> _activitiesForIntervalDay[MAX_DAYS_PER_WEEK];

						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							if(d2==d)
								continue;
				
							occupiedIntervalDay[d2]=false;
							canEmptyIntervalDay[d2]=true;
					
							_minWrong[d2]=INF;
							_nWrong[d2]=0;
							_nConflActivities[d2]=0;
							_minIndexAct[d2]=gt.rules.nInternalActivities;
							_activitiesForIntervalDay[d2].clear();
							
							for(int h2=sth; h2<endh; h2++){
								int ai2=teachersTimetable(tch,d2,h2);
								if(ai2>=0){
									if(!conflActivities[newtime].contains(ai2)){
										occupiedIntervalDay[d2]=true;
										if(fixedTimeActivity[ai2] || swappedActivities[ai2])
											canEmptyIntervalDay[d2]=false;
										else if(!_activitiesForIntervalDay[d2].contains(ai2)){
											_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
											_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
											_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
											_nConflActivities[d2]++;
											_activitiesForIntervalDay[d2].append(ai2);
											assert(_nConflActivities[d2]==_activitiesForIntervalDay[d2].count());
										}
									}
								}
							}
					
							if(!occupiedIntervalDay[d2])
								canEmptyIntervalDay[d2]=false;
						}
						occupiedIntervalDay[d]=true;
						canEmptyIntervalDay[d]=false;
				
						int nOc=0;
						bool canChooseDay=false;
				
						for(int j=0; j<gt.rules.nDaysPerWeek; j++)
							if(occupiedIntervalDay[j]){
								nOc++;
								if(canEmptyIntervalDay[j]){
									canChooseDay=true;
								}
							}
						
						assert(nOc==maxDays+1);
					
						if(!canChooseDay){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachersintervalmaxdaysperweek=false;
							goto impossibleteachersintervalmaxdaysperweek;
						}
					
						int d2=-1;
					
						////////////////////
						QList<int> candidateDays;

						int _mW=INF;
						int _nW=INF;
						int _mCA=gt.rules.nInternalActivities;
						int _mIA=gt.rules.nInternalActivities;

						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyIntervalDay[kk]){
								if(_mW>_minWrong[kk] ||
								(_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
								(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
								(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
									_mW=_minWrong[kk];
									_nW=_nWrong[kk];
									_mCA=_nConflActivities[kk];
									_mIA=_minIndexAct[kk];
								}
							}
						
						assert(_mW<INF);
						
						candidateDays.clear();
						for(int kk=0; kk<gt.rules.nDaysPerWeek; kk++)
							if(canEmptyIntervalDay[kk])
								if(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
									candidateDays.append(kk);
						
						assert(candidateDays.count()>0);
						d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
						///////////////////
					
						assert(d2>=0);

						assert(_activitiesForIntervalDay[d2].count()>0);

						for(int ai2 : qAsConst(_activitiesForIntervalDay[d2])){
							assert(ai2!=ai);
							assert(!swappedActivities[ai2]);
							assert(!fixedTimeActivity[ai2]);
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
						}
					}
				}
			}
		}
		//respecting teachers interval max days per week
impossibleteachersintervalmaxdaysperweek:
		if(!okteachersintervalmaxdaysperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END teachers interval max days per week

/////////////////////////////////////////////////////////////////////////////////////////////

		//BEGIN teachers morning interval max days per week
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(d%2==0){ //d is a morning
				okteachersmorningintervalmaxdaysperweek=true;
				for(int tch : qAsConst(act->iTeachersList)){
					for(int cnt=0; cnt<teachersMorningIntervalMaxDaysPerWeekPercentages[tch].count(); cnt++){
						double perc=teachersMorningIntervalMaxDaysPerWeekPercentages[tch].at(cnt);
						int maxDays=teachersMorningIntervalMaxDaysPerWeekMaxDays[tch].at(cnt);
						int sth=teachersMorningIntervalMaxDaysPerWeekIntervalStart[tch].at(cnt);
						int endh=teachersMorningIntervalMaxDaysPerWeekIntervalEnd[tch].at(cnt);

						assert(perc>=0);
						assert(sth>=0 && sth<gt.rules.nHoursPerDay);
						assert(endh>sth && endh<=gt.rules.nHoursPerDay);
						assert(maxDays>=0 && maxDays<=gt.rules.nDaysPerWeek/2);

						if(skipRandom(perc))
							continue;

						assert(perc==100.0);

						bool foundothers=false;
						bool foundai=false;
						for(int hh=sth; hh<endh; hh++){
							if(newTeachersTimetable(tch,d,hh)==ai){
								foundai=true;
							}
							else{
								assert(newTeachersTimetable(tch,d,hh)==teachersTimetable(tch,d,hh));
								if(newTeachersTimetable(tch,d,hh)>=0){
									if(!conflActivities[newtime].contains(newTeachersTimetable(tch,d,hh))){
										foundothers=true;
									}
								}
							}
						}
						int nrotherdays=0;
						for(int dd=0; dd<gt.rules.nDaysPerWeek; dd+=2){ //morning
							if(dd!=d){
								for(int hh=sth; hh<endh; hh++){
									assert(newTeachersTimetable(tch,dd,hh)==teachersTimetable(tch,dd,hh));
									if(newTeachersTimetable(tch,dd,hh)>=0 && !conflActivities[newtime].contains(newTeachersTimetable(tch,dd,hh))){
										nrotherdays++;
										break;
									}
								}
							}
						}
						assert(nrotherdays<=maxDays); //if percentage==100%, then it is impossible to break this constraint
						if((foundai && !foundothers) && nrotherdays==maxDays){
							//increased above limit
							//bool occupiedIntervalDay[MAX_DAYS_PER_WEEK];
							//bool canEmptyIntervalDay[MAX_DAYS_PER_WEEK];

							//int _minWrong[MAX_DAYS_PER_WEEK];
							//int _nWrong[MAX_DAYS_PER_WEEK];
							//int _nConflActivities[MAX_DAYS_PER_WEEK];
							//int _minIndexAct[MAX_DAYS_PER_WEEK];

							//QList<int> _activitiesForIntervalDay[MAX_DAYS_PER_WEEK];

							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2+=2){ //morning
								if(d2==d)
									continue;

								occupiedIntervalDay[d2]=false;
								canEmptyIntervalDay[d2]=true;

								_minWrong[d2]=INF;
								_nWrong[d2]=0;
								_nConflActivities[d2]=0;
								_minIndexAct[d2]=gt.rules.nInternalActivities;
								_activitiesForIntervalDay[d2].clear();

								for(int h2=sth; h2<endh; h2++){
									int ai2=teachersTimetable(tch,d2,h2);
								//for(int ai2 : qAsConst(teacherActivitiesOfTheDay(tch,d2))){
									if(ai2>=0){
										if(!conflActivities[newtime].contains(ai2)){
											occupiedIntervalDay[d2]=true;
											if(fixedTimeActivity[ai2] || swappedActivities[ai2])
												canEmptyIntervalDay[d2]=false;
											else if(!_activitiesForIntervalDay[d2].contains(ai2)){
												_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
												_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
												_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
												_nConflActivities[d2]++;
												_activitiesForIntervalDay[d2].append(ai2);
												assert(_nConflActivities[d2]==_activitiesForIntervalDay[d2].count());
											}
										}
									}
								}

								if(!occupiedIntervalDay[d2])
									canEmptyIntervalDay[d2]=false;
							}
							occupiedIntervalDay[d]=true;
							canEmptyIntervalDay[d]=false;

							int nOc=0;
							bool canChooseDay=false;

							for(int j=0; j<gt.rules.nDaysPerWeek; j+=2) //morning
								if(occupiedIntervalDay[j]){
									nOc++;
									if(canEmptyIntervalDay[j]){
										canChooseDay=true;
									}
								}

							//if(nOc>maxDays){
							assert(nOc==maxDays+1);

							if(!canChooseDay){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okteachersmorningintervalmaxdaysperweek=false;
								goto impossibleteachersmorningintervalmaxdaysperweek;
							}

							int d2=-1;

							if(level!=0){
								//choose random day from those with minimum number of conflicting activities
								QList<int> candidateDays;

								int m=gt.rules.nInternalActivities;

								for(int kk=0; kk<gt.rules.nDaysPerWeek; kk+=2) //morning
									if(canEmptyIntervalDay[kk])
										if(m>_nConflActivities[kk])
											m=_nConflActivities[kk];

								candidateDays.clear();
								for(int kk=0; kk<gt.rules.nDaysPerWeek; kk+=2) //morning
									if(canEmptyIntervalDay[kk] && m==_nConflActivities[kk])
										candidateDays.append(kk);

								assert(candidateDays.count()>0);
								d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
							}
							else{ //level==0
								QList<int> candidateDays;

								int _mW=INF;
								int _nW=INF;
								int _mCA=gt.rules.nInternalActivities;
								int _mIA=gt.rules.nInternalActivities;

								for(int kk=0; kk<gt.rules.nDaysPerWeek; kk+=2) //morning
									if(canEmptyIntervalDay[kk]){
										if(_mW>_minWrong[kk] ||
										(_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
										(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
										(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
											_mW=_minWrong[kk];
											_nW=_nWrong[kk];
											_mCA=_nConflActivities[kk];
											_mIA=_minIndexAct[kk];
										}
									}

								assert(_mW<INF);

								candidateDays.clear();
								for(int kk=0; kk<gt.rules.nDaysPerWeek; kk+=2) //morning
									if(canEmptyIntervalDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
										candidateDays.append(kk);

								assert(candidateDays.count()>0);
								d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
							}

							assert(d2>=0);

							assert(_activitiesForIntervalDay[d2].count()>0);

							for(int ai2 : qAsConst(_activitiesForIntervalDay[d2])){
								assert(ai2!=ai);
								assert(!swappedActivities[ai2]);
								assert(!fixedTimeActivity[ai2]);
								assert(!conflActivities[newtime].contains(ai2));
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
							}
						}
					}
				}
				//respecting teachers interval max days per week
impossibleteachersmorningintervalmaxdaysperweek:
				if(!okteachersmorningintervalmaxdaysperweek){
					if(updateSubgroups || updateTeachers)
						removeAiFromNewTimetable(ai, act, d, h);
					//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

					nConflActivities[newtime]=MAX_ACTIVITIES;
					continue;
				}
			}
		}

		////////////////////////////END teachers morning interval max days per week

/////////////////////////////////////////////////////////////////////////////////////////////

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			//BEGIN teachers afternoon interval max days per week
			if(d%2==1){ //d is an afternoon
				okteachersafternoonintervalmaxdaysperweek=true;
				for(int tch : qAsConst(act->iTeachersList)){
					for(int cnt=0; cnt<teachersAfternoonIntervalMaxDaysPerWeekPercentages[tch].count(); cnt++){
						double perc=teachersAfternoonIntervalMaxDaysPerWeekPercentages[tch].at(cnt);
						int maxDays=teachersAfternoonIntervalMaxDaysPerWeekMaxDays[tch].at(cnt);
						int sth=teachersAfternoonIntervalMaxDaysPerWeekIntervalStart[tch].at(cnt);
						int endh=teachersAfternoonIntervalMaxDaysPerWeekIntervalEnd[tch].at(cnt);

						assert(perc>=0);
						assert(sth>=0 && sth<gt.rules.nHoursPerDay);
						assert(endh>sth && endh<=gt.rules.nHoursPerDay);
						assert(maxDays>=0 && maxDays<=gt.rules.nDaysPerWeek/2);

						if(skipRandom(perc))
							continue;

						assert(perc==100.0);

						bool foundothers=false;
						bool foundai=false;
						for(int hh=sth; hh<endh; hh++){
							if(newTeachersTimetable(tch,d,hh)==ai){
								foundai=true;
							}
							else{
								assert(newTeachersTimetable(tch,d,hh)==teachersTimetable(tch,d,hh));
								if(newTeachersTimetable(tch,d,hh)>=0){
									if(!conflActivities[newtime].contains(newTeachersTimetable(tch,d,hh))){
										foundothers=true;
									}
								}
							}
						}
						int nrotherdays=0;
						for(int dd=1; dd<gt.rules.nDaysPerWeek; dd+=2){ //afternoon
							if(dd!=d){
								for(int hh=sth; hh<endh; hh++){
									assert(newTeachersTimetable(tch,dd,hh)==teachersTimetable(tch,dd,hh));
									if(newTeachersTimetable(tch,dd,hh)>=0 && !conflActivities[newtime].contains(newTeachersTimetable(tch,dd,hh))){
										nrotherdays++;
										break;
									}
								}
							}
						}
						assert(nrotherdays<=maxDays); //if percentage==100%, then it is impossible to break this constraint
						if((foundai && !foundothers) && nrotherdays==maxDays){
							//increased above limit
							//bool occupiedIntervalDay[MAX_DAYS_PER_WEEK];
							//bool canEmptyIntervalDay[MAX_DAYS_PER_WEEK];

							//int _minWrong[MAX_DAYS_PER_WEEK];
							//int _nWrong[MAX_DAYS_PER_WEEK];
							//int _nConflActivities[MAX_DAYS_PER_WEEK];
							//int _minIndexAct[MAX_DAYS_PER_WEEK];

							//QList<int> _activitiesForIntervalDay[MAX_DAYS_PER_WEEK];

							for(int d2=1; d2<gt.rules.nDaysPerWeek; d2+=2){ //afternoon
								if(d2==d)
									continue;

								occupiedIntervalDay[d2]=false;
								canEmptyIntervalDay[d2]=true;

								_minWrong[d2]=INF;
								_nWrong[d2]=0;
								_nConflActivities[d2]=0;
								_minIndexAct[d2]=gt.rules.nInternalActivities;
								_activitiesForIntervalDay[d2].clear();

								for(int h2=sth; h2<endh; h2++){
									int ai2=teachersTimetable(tch,d2,h2);
								//for(int ai2 : qAsConst(teacherActivitiesOfTheDay(tch,d2))){
									if(ai2>=0){
										if(!conflActivities[newtime].contains(ai2)){
											occupiedIntervalDay[d2]=true;
											if(fixedTimeActivity[ai2] || swappedActivities[ai2])
												canEmptyIntervalDay[d2]=false;
											else if(!_activitiesForIntervalDay[d2].contains(ai2)){
												_minWrong[d2] = min (_minWrong[d2], triedRemovals(ai2,c.times[ai2]));
												_minIndexAct[d2]=min(_minIndexAct[d2], invPermutation[ai2]);
												_nWrong[d2]+=triedRemovals(ai2,c.times[ai2]);
												_nConflActivities[d2]++;
												_activitiesForIntervalDay[d2].append(ai2);
												assert(_nConflActivities[d2]==_activitiesForIntervalDay[d2].count());
											}
										}
									}
								}

								if(!occupiedIntervalDay[d2])
									canEmptyIntervalDay[d2]=false;
							}
							occupiedIntervalDay[d]=true;
							canEmptyIntervalDay[d]=false;

							int nOc=0;
							bool canChooseDay=false;

							for(int j=1; j<gt.rules.nDaysPerWeek; j+=2) //afternoon
								if(occupiedIntervalDay[j]){
									nOc++;
									if(canEmptyIntervalDay[j]){
										canChooseDay=true;
									}
								}

							//if(nOc>maxDays){
							assert(nOc==maxDays+1);

							if(!canChooseDay){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okteachersafternoonintervalmaxdaysperweek=false;
								goto impossibleteachersafternoonintervalmaxdaysperweek;
							}

							int d2=-1;

							if(level!=0){
								//choose random day from those with minimum number of conflicting activities
								QList<int> candidateDays;

								int m=gt.rules.nInternalActivities;

								for(int kk=1; kk<gt.rules.nDaysPerWeek; kk+=2) //afternoon
									if(canEmptyIntervalDay[kk])
										if(m>_nConflActivities[kk])
											m=_nConflActivities[kk];

								candidateDays.clear();
								for(int kk=1; kk<gt.rules.nDaysPerWeek; kk+=2) //afternoon
									if(canEmptyIntervalDay[kk] && m==_nConflActivities[kk])
										candidateDays.append(kk);

								assert(candidateDays.count()>0);
								d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
							}
							else{ //level==0
								QList<int> candidateDays;

								int _mW=INF;
								int _nW=INF;
								int _mCA=gt.rules.nInternalActivities;
								int _mIA=gt.rules.nInternalActivities;

								for(int kk=1; kk<gt.rules.nDaysPerWeek; kk+=2) //afternoon
									if(canEmptyIntervalDay[kk]){
										if(_mW>_minWrong[kk] ||
										(_mW==_minWrong[kk] && _nW>_nWrong[kk]) ||
										(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA>_nConflActivities[kk]) ||
										(_mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA>_minIndexAct[kk])){
											_mW=_minWrong[kk];
											_nW=_nWrong[kk];
											_mCA=_nConflActivities[kk];
											_mIA=_minIndexAct[kk];
										}
									}

								assert(_mW<INF);

								candidateDays.clear();
								for(int kk=1; kk<gt.rules.nDaysPerWeek; kk+=2) //afternoon
									if(canEmptyIntervalDay[kk] && _mW==_minWrong[kk] && _nW==_nWrong[kk] && _mCA==_nConflActivities[kk] && _mIA==_minIndexAct[kk])
										candidateDays.append(kk);

								assert(candidateDays.count()>0);
								d2=candidateDays.at(rng.intMRG32k3a(candidateDays.count()));
							}

							assert(d2>=0);

							assert(_activitiesForIntervalDay[d2].count()>0);

							for(int ai2 : qAsConst(_activitiesForIntervalDay[d2])){
								assert(ai2!=ai);
								assert(!swappedActivities[ai2]);
								assert(!fixedTimeActivity[ai2]);
								assert(!conflActivities[newtime].contains(ai2));
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								//addConflActivity(conflActivities[newtime], nConflActivities[newtime], ai2, &gt.rules.internalActivitiesList[ai2]);
							}
						}
					}
				}
				//respecting teachers interval max days per week
impossibleteachersafternoonintervalmaxdaysperweek:
				if(!okteachersafternoonintervalmaxdaysperweek){
					if(updateSubgroups || updateTeachers)
						removeAiFromNewTimetable(ai, act, d, h);
					//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

					nConflActivities[newtime]=MAX_ACTIVITIES;
					continue;
				}
			}
		}

		////////////////////////////END teachers afternoon interval max days per week

/////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////BEGIN teachers max span per day

		//Rodolfo Ribeiro Gomes's code (https://bitbucket.org/rodolforg/fet/src/dev/) was a source of inspiration for the following constraint

		okteachersmaxspanperday=true;
		for(int tch : qAsConst(act->iTeachersList))
			if(teachersMaxSpanPerDayPercentages[tch]>=0){
				//percentage is 100%
				int maxSpanPerDay=teachersMaxSpanPerDayMaxSpan[tch];
				bool allowException=teachersMaxSpanPerDayAllowOneDayExceptionPlusOne[tch];
			
				//preliminary test
				int _cnt=0;
				int _start=-1;
				int _end=-1;
				for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
					if(newTeachersTimetable(tch, d, h2)>=0){
						_start=h2;
						break;
					}
				for(int h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
					if(newTeachersTimetable(tch, d, h2)>=0){
						_end=h2;
						break;
					}
					
				if(_start>=0 && _end>=0 && _end>=_start)
					_cnt=_end-_start+1;
					
				if(_cnt<=maxSpanPerDay)
					continue;
					
				int dayOldException=-2;
				if(/*_cnt==maxSpanPerDay+1 &&*/ allowException){
					if(_cnt==maxSpanPerDay+1)
						dayOldException=-1;
					else
						dayOldException=-3;
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						if(d2==d)
							continue;
							
						int _cnt3=0;
						int _start3=-1;
						int _end3=-1;
						for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
							if(newTeachersTimetable(tch, d2, h2)>=0){
								_start3=h2;
								break;
							}
						for(int h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
							if(newTeachersTimetable(tch, d2, h2)>=0){
								_end3=h2;
								break;
							}
					
						if(_start3>=0 && _end3>=0 && _end3>=_start3)
							_cnt3=_end3-_start3+1;
							
						assert(_cnt3<=maxSpanPerDay+1);
						if(_cnt3==maxSpanPerDay+1){
							assert(dayOldException==-1 || dayOldException==-3);
							dayOldException=d2;
						}
					}
				}
				if(dayOldException==-1) //OK
					continue;
				
				if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
					okteachersmaxspanperday=false;
					goto impossibleteachersmaxspanperday;
				}

				getTchTimetable(tch, conflActivities[newtime]);
				updateTchNHoursGaps(tch, d); //needed for teacherRemoveAnActivityFromBeginOrEndCertainDay below
				if(dayOldException>=0)
					updateTchNHoursGaps(tch, dayOldException); //needed for teacherRemoveAnActivityFromBeginOrEndCertainDay below

				for(;;){
					int cnt=0;
					int start=-1;
					int end=-1;
					for(int h2=0; h2<gt.rules.nHoursPerDay; h2++)
						if(tchTimetable(d, h2)>=0){
							start=h2;
							break;
						}
					for(int h2=gt.rules.nHoursPerDay-1; h2>=0; h2--)
						if(tchTimetable(d, h2)>=0){
							end=h2;
							break;
						}
						
					if(start>=0 && end>=0 && end>=start)
						cnt=end-start+1;
						
					if(cnt<=maxSpanPerDay || (cnt==maxSpanPerDay+1 && dayOldException==-1))
						break;
				
					int ai2=-1;
					
					if(cnt>=maxSpanPerDay+2 || !allowException || dayOldException==-3){
						bool k=teacherRemoveAnActivityFromBeginOrEndCertainDay(d, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!k){
							if(level==0){
								//old comment below
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachersmaxspanperday=false;
							goto impossibleteachersmaxspanperday;
						}
					
						assert(ai2>=0);

						removeAi2FromTchTimetable(ai2);
						tchDayNHours[d]-=gt.rules.internalActivitiesList[ai2].duration; //needed for teacherRemoveAnActivityFromBeginOrEndCertainDay above
						assert(tchDayNHours[d]>=0);
					}
					else{
						assert(allowException);
						assert(cnt==maxSpanPerDay+1);
						
						assert(dayOldException>=0);
						
						//int removedDayOldException=-1;
						
						if(rng.intMRG32k3a(2)==0){
							bool k=teacherRemoveAnActivityFromBeginOrEndCertainDay(d, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								bool k2=teacherRemoveAnActivityFromBeginOrEndCertainDay(dayOldException, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k2){
									if(level==0){
										//old comment below
										//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okteachersmaxspanperday=false;
									goto impossibleteachersmaxspanperday;
								}
								else{
									/*removedDayOldException=dayOldException;
									dayOldException=-1;*/
								}
							}
						
							assert(ai2>=0);
							
							break; //OK
	
							/*removeAi2FromTchTimetable(ai2);
							if(removedDayOldException==-1){
								assert(c.times[ai2]%gt.rules.nDaysPerWeek==d);
								tchDayNHours[d]-=gt.rules.internalActivitiesList[ai2].duration; //needed for teacherRemoveAnActivityFromBeginOrEndCertainDay above
								assert(tchDayNHours[d]>=0);
							}
							else{
								assert(c.times[ai2]%gt.rules.nDaysPerWeek==removedDayOldException);
								tchDayNHours[removedDayOldException]-=gt.rules.internalActivitiesList[ai2].duration; //needed for teacherRemoveAnActivityFromBeginOrEndCertainDay above
								assert(tchDayNHours[removedDayOldException]>=0);
							}*/
						}
						else{
							bool k=teacherRemoveAnActivityFromBeginOrEndCertainDay(dayOldException, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								bool k2=teacherRemoveAnActivityFromBeginOrEndCertainDay(d, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k2){
									if(level==0){
										//old comment below
										//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okteachersmaxspanperday=false;
									goto impossibleteachersmaxspanperday;
								}
							}
							else{
								/*removedDayOldException=dayOldException;
								dayOldException=-1;*/
							}
						
							assert(ai2>=0);
	
							break; //OK
	
							/*removeAi2FromTchTimetable(ai2);
							if(removedDayOldException==-1){
								assert(c.times[ai2]%gt.rules.nDaysPerWeek==d);
								tchDayNHours[d]-=gt.rules.internalActivitiesList[ai2].duration; //needed for teacherRemoveAnActivityFromBeginOrEndCertainDay above
								assert(tchDayNHours[d]>=0);
							}
							else{
								assert(c.times[ai2]%gt.rules.nDaysPerWeek==removedDayOldException);
								tchDayNHours[removedDayOldException]-=gt.rules.internalActivitiesList[ai2].duration; //needed for teacherRemoveAnActivityFromBeginOrEndCertainDay above
								assert(tchDayNHours[removedDayOldException]>=0);
							}*/
						}
					}
				}
			}
		
impossibleteachersmaxspanperday:
		if(!okteachersmaxspanperday){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END teachers max span per day

/////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////BEGIN teachers max span per real day

		//Rodolfo Ribeiro Gomes's code (https://bitbucket.org/rodolforg/fet/src/dev/) was a source of inspiration for the following constraint

		okteachersmaxspanperrealday=true;
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int tch : qAsConst(act->iTeachersList))
				if(teachersMaxSpanPerRealDayPercentages[tch]>=0){
					//percentage is 100%
					int maxSpanPerDay=teachersMaxSpanPerRealDayMaxSpan[tch];
					bool allowException=teachersMaxSpanPerRealDayAllowOneDayExceptionPlusOne[tch];

					//preliminary test
					int _cnt=0;
					int _start=-1;
					int _end=-1;
					for(int h2=0; h2<2*gt.rules.nHoursPerDay; h2++){
						int d3=(d/2)*2+(h2<gt.rules.nHoursPerDay?0:1);
						int h3=h2%gt.rules.nHoursPerDay;
						if(newTeachersTimetable(tch, d3, h3)>=0){
							_start=h2;
							break;
						}
					}
					for(int h2=2*gt.rules.nHoursPerDay-1; h2>=0; h2--){
						int d3=(d/2)*2+(h2<gt.rules.nHoursPerDay?0:1);
						int h3=h2%gt.rules.nHoursPerDay;
						if(newTeachersTimetable(tch, d3, h3)>=0){
							_end=h2;
							break;
						}
					}

					if(_start>=0 && _end>=0 && _end>=_start)
						_cnt=_end-_start+1;

					if(_cnt<=maxSpanPerDay)
						continue;

					int dayOldException=-2;
					//cout<<"_cnt=="<<_cnt<<endl;
					//cout<<"maxSpanPerDay=="<<maxSpanPerDay<<endl;
					//cout<<"allowException=="<<allowException<<endl;
					if(/*_cnt==maxSpanPerDay+1 &&*/ allowException){
						if(_cnt==maxSpanPerDay+1)
							dayOldException=-1;
						else
							dayOldException=-3;
						for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
							if(d2==d/2)
								continue;

							int _cnt3=0;
							int _start3=-1;
							int _end3=-1;
							for(int h2=0; h2<2*gt.rules.nHoursPerDay; h2++){
								int d3=d2*2+(h2<gt.rules.nHoursPerDay?0:1);
								int h3=h2%gt.rules.nHoursPerDay;
								if(newTeachersTimetable(tch, d3, h3)>=0){
									_start3=h2;
									break;
								}
							}
							for(int h2=2*gt.rules.nHoursPerDay-1; h2>=0; h2--){
								int d3=d2*2+(h2<gt.rules.nHoursPerDay?0:1);
								int h3=h2%gt.rules.nHoursPerDay;
								if(newTeachersTimetable(tch, d3, h3)>=0){
									_end3=h2;
									break;
								}
							}

							if(_start3>=0 && _end3>=0 && _end3>=_start3)
								_cnt3=_end3-_start3+1;

							assert(_cnt3<=maxSpanPerDay+1);
							if(_cnt3==maxSpanPerDay+1){
								assert(dayOldException==-1 || dayOldException==-3);
								dayOldException=d2;
							}
						}
					}
					if(dayOldException==-1) //OK
						continue;

					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okteachersmaxspanperrealday=false;
						goto impossibleteachersmaxspanperrealday;
					}

					getTchTimetable(tch, conflActivities[newtime]);
					updateTchNHoursGaps(tch, d); //needed for teacherRemoveAnActivityFromBeginOrEndCertainRealDay below
					updateTchNHoursGaps(tch, dpair); //needed for teacherRemoveAnActivityFromBeginOrEndCertainRealDay below
					if(dayOldException>=0){
						updateTchNHoursGaps(tch, dayOldException*2); //needed for teacherRemoveAnActivityFromBeginOrEndCertainRealDay below
						updateTchNHoursGaps(tch, dayOldException*2+1); //needed for teacherRemoveAnActivityFromBeginOrEndCertainRealDay below
					}

					for(;;){
						int cnt=0;
						int start=-1;
						int end=-1;

						for(int h2=0; h2<2*gt.rules.nHoursPerDay; h2++){
							int d3=(d/2)*2+(h2<gt.rules.nHoursPerDay?0:1);
							int h3=h2%gt.rules.nHoursPerDay;
							if(tchTimetable(d3, h3)>=0){
								start=h2;
								break;
							}
						}
						for(int h2=2*gt.rules.nHoursPerDay-1; h2>=0; h2--){
							int d3=(d/2)*2+(h2<gt.rules.nHoursPerDay?0:1);
							int h3=h2%gt.rules.nHoursPerDay;
							if(tchTimetable(d3, h3)>=0){
								end=h2;
								break;
							}
						}

						if(start>=0 && end>=0 && end>=start)
							cnt=end-start+1;

						if(cnt<=maxSpanPerDay || (cnt==maxSpanPerDay+1 && dayOldException==-1))
							break;

						int ai2=-1;

						if(cnt>=maxSpanPerDay+2 || !allowException || dayOldException==-3){
							bool k=teacherRemoveAnActivityFromBeginOrEndCertainRealDay(d/2, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								if(level==0){
									//old comment below
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okteachersmaxspanperrealday=false;
								goto impossibleteachersmaxspanperrealday;
							}

							assert(ai2>=0);

							/*cout<<endl;
							cout<<"1"<<endl;
							cout<<"d2=="<<c.times[ai2]%gt.rules.nDaysPerWeek<<endl;
							cout<<"tchDayNHours[d2]=="<<tchDayNHours[c.times[ai2]%gt.rules.nDaysPerWeek]<<endl;
							cout<<"ai2=="<<ai2<<", id of ai2 == "<<gt.rules.internalActivitiesList[ai2].id
							 <<", duration of ai2 == "<<gt.rules.internalActivitiesList[ai2].duration<<endl;*/

							int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
							removeAi2FromTchTimetable(ai2);
							tchDayNHours[d2]-=gt.rules.internalActivitiesList[ai2].duration; //needed for teacherRemoveAnActivityFromBeginOrEndCertainRealDay above
							assert(tchDayNHours[d2]>=0);
						}
						else{
							assert(allowException);
							assert(cnt==maxSpanPerDay+1);

							//cout<<"dayOldException=="<<dayOldException<<endl;
							//cout<<"cnt=="<<cnt<<endl;
							//cout<<"maxSpanPerDay=="<<maxSpanPerDay<<endl;
							//cout<<"allowException=="<<allowException<<endl;

							assert(dayOldException>=0);

							//int removedDayOldException=-1;

							if(rng.intMRG32k3a(2)==0){
								bool k=teacherRemoveAnActivityFromBeginOrEndCertainRealDay(d/2, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k){
									bool k2=teacherRemoveAnActivityFromBeginOrEndCertainRealDay(dayOldException, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									if(!k2){
										if(level==0){
											//old comment below
											//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
											//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
										}
										okteachersmaxspanperrealday=false;
										goto impossibleteachersmaxspanperrealday;
									}
									else{
										/*removedDayOldException=dayOldException;
										dayOldException=-1;*/
									}
								}

								//Q_UNUSED(removedDayOldException);

								assert(ai2>=0);

								break; //OK

								/*cout<<endl;
								cout<<"2"<<endl;
								cout<<"d2=="<<c.times[ai2]%gt.rules.nDaysPerWeek<<endl;
								cout<<"tchDayNHours[d2]=="<<tchDayNHours[c.times[ai2]%gt.rules.nDaysPerWeek]<<endl;
								cout<<"ai2=="<<ai2<<", id of ai2 == "<<gt.rules.internalActivitiesList[ai2].id
								 <<", duration of ai2 == "<<gt.rules.internalActivitiesList[ai2].duration<<endl;*/

								/*int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
								removeAi2FromTchTimetable(ai2);
								tchDayNHours[d2]-=gt.rules.internalActivitiesList[ai2].duration; //needed for teacherRemoveAnActivityFromBeginOrEndCertainRealDay above
								assert(tchDayNHours[d2]>=0);*/
							}
							else{
								bool k=teacherRemoveAnActivityFromBeginOrEndCertainRealDay(dayOldException, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								if(!k){
									bool k2=teacherRemoveAnActivityFromBeginOrEndCertainRealDay(d/2, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									if(!k2){
										if(level==0){
											//old comment below
											//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
											//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
										}
										okteachersmaxspanperrealday=false;
										goto impossibleteachersmaxspanperrealday;
									}
								}
								else{
									/*removedDayOldException=dayOldException;
									dayOldException=-1;*/
								}

								//Q_UNUSED(removedDayOldException);

								assert(ai2>=0);

								break; //OK

								/*cout<<endl;
								cout<<"3"<<endl;
								cout<<"d2=="<<c.times[ai2]%gt.rules.nDaysPerWeek<<endl;
								cout<<"tchDayNHours[d2]=="<<tchDayNHours[c.times[ai2]%gt.rules.nDaysPerWeek]<<endl;
								cout<<"ai2=="<<ai2<<", id of ai2 == "<<gt.rules.internalActivitiesList[ai2].id
								 <<", duration of ai2 == "<<gt.rules.internalActivitiesList[ai2].duration<<endl;*/

								/*int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
								removeAi2FromTchTimetable(ai2);
								tchDayNHours[d2]-=gt.rules.internalActivitiesList[ai2].duration; //needed for teacherRemoveAnActivityFromBeginOrEndCertainRealDay above
								assert(tchDayNHours[d2]>=0);*/
							}
						}
					}
				}
			//cout<<"exit"<<endl;
		}

impossibleteachersmaxspanperrealday:
		if(!okteachersmaxspanperrealday){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END teachers max span per real day

/////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////BEGIN teachers min resting hours

		//Rodolfo Ribeiro Gomes's code (https://bitbucket.org/rodolforg/fet/src/dev/) was a source of inspiration for the following constraint

		okteachersminrestinghours=true;

		for(int tch : qAsConst(act->iTeachersList)){
			for(int qq=0; qq<2; qq++){
				double percentage;
				int minRestingHours;
				bool circular;
				if(qq==0){
					percentage=teachersMinRestingHoursCircularPercentages[tch];
					minRestingHours=teachersMinRestingHoursCircularMinHours[tch];
					circular=true;
				}
				else{
					assert(qq==1);
					percentage=teachersMinRestingHoursNotCircularPercentages[tch];
					minRestingHours=teachersMinRestingHoursNotCircularMinHours[tch];
					circular=false;
				}
				if(percentage>=0){
					//percentage is 100%
					assert(minRestingHours<=gt.rules.nHoursPerDay);
					
					//phase 1 - activity is at the end of the day
					int _cnt1=0;
					int _cnt1d=0;
					if(d <= gt.rules.nDaysPerWeek-2+(circular?1:0) && h+act->duration-1 >= gt.rules.nHoursPerDay-minRestingHours){
						for(int h2=gt.rules.nHoursPerDay-1; h2>h+act->duration-1; h2--){
							int ai2=newTeachersTimetable(tch, d, h2);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2))
								break;
							else
								_cnt1++;
						}
						_cnt1d=_cnt1;
						for(int h2=0; h2<minRestingHours-_cnt1d; h2++){
							int ai2=newTeachersTimetable(tch, (d+1<=gt.rules.nDaysPerWeek-1?d+1:0), h2);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2))
								break;
							else
								_cnt1++;
						}
					}
					else
						_cnt1=minRestingHours;
					
					//phase 2 - activity is at the beginning of the day
					int _cnt2=0;
					int _cnt2d=0;
					if(d>=1-(circular?1:0) && h<=minRestingHours-1){
						for(int h2=0; h2<h; h2++){
							int ai2=newTeachersTimetable(tch, d, h2);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2))
								break;
							else
								_cnt2++;
						}
						_cnt2d=_cnt2;
						for(int h2=gt.rules.nHoursPerDay-1; h2>=gt.rules.nHoursPerDay-minRestingHours+_cnt2d; h2--){
							int ai2=newTeachersTimetable(tch, (d-1>=0?d-1:gt.rules.nDaysPerWeek-1), h2);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2))
								break;
							else
								_cnt2++;
						}
					}
					else
						_cnt2=minRestingHours;
						
					if(_cnt1<minRestingHours){
						QList<int> removableActs;
						/*for(int h2=gt.rules.nHoursPerDay-minRestingHours; h2<gt.rules.nHoursPerDay; h2++){
							int ai2=newTeachersTimetable(tch, d, h2);
							if(ai2>=0 && ai2!=ai && !removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2] && !conflActivities[newtime].contains(ai2))
								removableActs.append(ai2);
						}*/
						for(int h2=0; h2<minRestingHours-_cnt1d; h2++){
							int ai2=newTeachersTimetable(tch, (d+1<=gt.rules.nDaysPerWeek-1?d+1:0), h2);
							if(ai2>=0 && !removableActs.contains(ai2) && !conflActivities[newtime].contains(ai2)){
								if(!fixedTimeActivity[ai2] && !swappedActivities[ai2])
									removableActs.append(ai2);
								else
									break;
							}
						}
						
						for(;;){
							if(removableActs.count()==0){
								okteachersminrestinghours=false;
								goto impossibleteachersminrestinghours;
							}
							
							int ai2=removableActs.at(0);
							
							int t=removableActs.removeAll(ai2);
							assert(t==1);
							
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);

							int cnt1=0;
							if(d <= gt.rules.nDaysPerWeek-2+(circular?1:0) && h+act->duration-1 >= gt.rules.nHoursPerDay-minRestingHours){
								/*for(int h2=gt.rules.nHoursPerDay-1; h2>h+act->duration-1; h2--){
									int ai2=newTeachersTimetable(tch, d, h2);
									if(ai2>=0 && !conflActivities[newtime].contains(ai2))
										break;
									else
										cnt1++;
								}*/
								cnt1+=_cnt1d;
								for(int h2=0; h2<minRestingHours-_cnt1d; h2++){
									int ai2=newTeachersTimetable(tch, (d+1<=gt.rules.nDaysPerWeek-1?d+1:0), h2);
									if(ai2>=0 && !conflActivities[newtime].contains(ai2))
										break;
									else
										cnt1++;
								}
							}
							else{
								assert(0);
							}
							
							assert(cnt1>_cnt1);
							_cnt1=cnt1;
							
							if(cnt1>=minRestingHours)
								break;
						}
					}
					if(_cnt2<minRestingHours){
						QList<int> removableActs;
						for(int h2=gt.rules.nHoursPerDay-1; h2>=gt.rules.nHoursPerDay-minRestingHours+_cnt2d; h2--){
							int ai2=newTeachersTimetable(tch, (d-1>=0?d-1:gt.rules.nDaysPerWeek-1), h2);
							if(ai2>=0 && !removableActs.contains(ai2) && !conflActivities[newtime].contains(ai2)){
								if(!fixedTimeActivity[ai2] && !swappedActivities[ai2])
									removableActs.append(ai2);
								else
									break;
							}
						}
						/*for(int h2=0; h2<minRestingHours; h2++){
							int ai2=newTeachersTimetable(tch, d, h2);
							if(ai2>=0 && ai2!=ai && !removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2] && !conflActivities[newtime].contains(ai2))
								removableActs.append(ai2);
						}*/
						
						for(;;){
							if(removableActs.count()==0){
								okteachersminrestinghours=false;
								goto impossibleteachersminrestinghours;
							}
							
							int ai2=removableActs.at(0);
							
							int t=removableActs.removeAll(ai2);
							assert(t==1);
							
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);

							int cnt2=0;
							if(d>=1-(circular?1:0) && h<minRestingHours){
								/*for(int h2=0; h2<h; h2++){
									int ai2=newTeachersTimetable(tch, d, h2);
									if(ai2>=0 && !conflActivities[newtime].contains(ai2))
										break;
									else
										cnt2++;
								}*/
								cnt2+=_cnt2d;
								for(int h2=gt.rules.nHoursPerDay-1; h2>=gt.rules.nHoursPerDay-minRestingHours+_cnt2d; h2--){
									int ai2=newTeachersTimetable(tch, (d-1>=0?d-1:gt.rules.nDaysPerWeek-1), h2);
									if(ai2>=0 && !conflActivities[newtime].contains(ai2))
										break;
									else
										cnt2++;
								}
							}
							else{
								assert(0);
							}
							
							assert(cnt2>_cnt2);
							_cnt2=cnt2;
							
							if(cnt2>=minRestingHours)
								break;
						}
					}
				}
			}
		}

impossibleteachersminrestinghours:
		if(!okteachersminrestinghours){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END teachers min resting hours

/////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////BEGIN teachers min resting hours between morning and afternoon

		//Rodolfo Ribeiro Gomes's code (https://bitbucket.org/rodolforg/fet/src/dev/) was a source of inspiration for the following constraint

		okteachersminrestinghoursbetweenmorningandafternoon=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int tch : qAsConst(act->iTeachersList)){
				double percentage=teachersMinRestingHoursBetweenMorningAndAfternoonPercentages[tch];
				int minRestingHours=teachersMinRestingHoursBetweenMorningAndAfternoonMinHours[tch];

				if(percentage>=0){
					assert(percentage==100.0);
					assert(minRestingHours<=2*gt.rules.nHoursPerDay);

					if(d%2==0){ //morning
						int _cnt1=0;
						int _cnt1d=0;
						if(h+act->duration-1 >= gt.rules.nHoursPerDay-minRestingHours){
							for(int h2=gt.rules.nHoursPerDay-1; h2>h+act->duration-1; h2--){
								int ai2=newTeachersTimetable(tch, d, h2);
								if(ai2>=0 && !conflActivities[newtime].contains(ai2))
									break;
								else
									_cnt1++;
							}
							_cnt1d=_cnt1;
							for(int h2=0; h2<minRestingHours-_cnt1d && h2<gt.rules.nHoursPerDay; h2++){
								int ai2=newTeachersTimetable(tch, d+1, h2);
								if(ai2>=0 && !conflActivities[newtime].contains(ai2))
									break;
								else
									_cnt1++;
							}
						}
						else
							_cnt1=minRestingHours;

						if(_cnt1<minRestingHours){
							QList<int> removableActs;
							/*for(int h2=gt.rules.nHoursPerDay-minRestingHours; h2<gt.rules.nHoursPerDay; h2++){
								int ai2=newTeachersTimetable(tch, d, h2);
								if(ai2>=0 && ai2!=ai && !removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2] && !conflActivities[newtime].contains(ai2))
									removableActs.append(ai2);
							}*/
							for(int h2=0; h2<minRestingHours-_cnt1d && h2<gt.rules.nHoursPerDay; h2++){
								int ai2=newTeachersTimetable(tch, d+1, h2);
								if(ai2>=0 && !removableActs.contains(ai2) && !conflActivities[newtime].contains(ai2)){
									if(!fixedTimeActivity[ai2] && !swappedActivities[ai2])
										removableActs.append(ai2);
									else
										break;
								}
							}

							for(;;){
								if(removableActs.count()==0){
									okteachersminrestinghoursbetweenmorningandafternoon=false;
									goto impossibleteachersminrestinghoursbetweenmorningandafternoon;
								}

								int ai2=removableActs.at(0);

								int t=removableActs.removeAll(ai2);
								assert(t==1);

								assert(!conflActivities[newtime].contains(ai2));
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								int cnt1=0;
								if(h+act->duration-1 >= gt.rules.nHoursPerDay-minRestingHours){
									/*for(int h2=gt.rules.nHoursPerDay-1; h2>h+act->duration-1; h2--){
										int ai2=newTeachersTimetable(tch, d, h2);
										if(ai2>=0 && !conflActivities[newtime].contains(ai2))
											break;
										else
											cnt1++;
									}*/
									cnt1+=_cnt1d;
									for(int h2=0; h2<minRestingHours-_cnt1d && h2<gt.rules.nHoursPerDay; h2++){
										int ai2=newTeachersTimetable(tch, d+1, h2);
										if(ai2>=0 && !conflActivities[newtime].contains(ai2))
											break;
										else
											cnt1++;
									}
								}
								else{
									assert(0);
								}

								assert(cnt1>_cnt1);
								_cnt1=cnt1;

								if(cnt1>=minRestingHours)
									break;
							}
						}
					}
					else{ //afternoon
						assert(d%2==1);
						int _cnt2=0;
						int _cnt2d=0;
						if(h<=minRestingHours-1){
							for(int h2=0; h2<h; h2++){
								int ai2=newTeachersTimetable(tch, d, h2);
								if(ai2>=0 && !conflActivities[newtime].contains(ai2))
									break;
								else
									_cnt2++;
							}
							_cnt2d=_cnt2;
							for(int h2=gt.rules.nHoursPerDay-1; h2>=gt.rules.nHoursPerDay-minRestingHours+_cnt2d && h2>=0; h2--){
								int ai2=newTeachersTimetable(tch, d-1, h2);
								if(ai2>=0 && !conflActivities[newtime].contains(ai2))
									break;
								else
									_cnt2++;
							}
						}
						else
							_cnt2=minRestingHours;

						if(_cnt2<minRestingHours){
							QList<int> removableActs;
							for(int h2=gt.rules.nHoursPerDay-1; h2>=gt.rules.nHoursPerDay-minRestingHours+_cnt2d && h2>=0; h2--){
								int ai2=newTeachersTimetable(tch, d-1, h2);
								if(ai2>=0 && !removableActs.contains(ai2) && !conflActivities[newtime].contains(ai2)){
									if(!fixedTimeActivity[ai2] && !swappedActivities[ai2])
										removableActs.append(ai2);
									else
										break;
								}
							}
							/*for(int h2=0; h2<minRestingHours; h2++){
								int ai2=newTeachersTimetable(tch, d, h2);
								if(ai2>=0 && ai2!=ai && !removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2] && !conflActivities[newtime].contains(ai2))
									removableActs.append(ai2);
							}*/

							for(;;){
								if(removableActs.count()==0){
									okteachersminrestinghoursbetweenmorningandafternoon=false;
									goto impossibleteachersminrestinghoursbetweenmorningandafternoon;
								}

								int ai2=removableActs.at(0);

								int t=removableActs.removeAll(ai2);
								assert(t==1);

								assert(!conflActivities[newtime].contains(ai2));
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								int cnt2=0;
								if(h<minRestingHours){
									/*for(int h2=0; h2<h; h2++){
										int ai2=newTeachersTimetable(tch, d, h2);
										if(ai2>=0 && !conflActivities[newtime].contains(ai2))
											break;
										else
											cnt2++;
									}*/
									cnt2+=_cnt2d;
									for(int h2=gt.rules.nHoursPerDay-1; h2>=gt.rules.nHoursPerDay-minRestingHours+_cnt2d && h2>=0; h2--){
										int ai2=newTeachersTimetable(tch, d-1, h2);
										if(ai2>=0 && !conflActivities[newtime].contains(ai2))
											break;
										else
											cnt2++;
									}
								}
								else{
									assert(0);
								}

								assert(cnt2>_cnt2);
								_cnt2=cnt2;

								if(cnt2>=minRestingHours)
									break;
							}
						}
					}
				}
			}
		}

impossibleteachersminrestinghoursbetweenmorningandafternoon:
		if(!okteachersminrestinghoursbetweenmorningandafternoon){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END teachers min resting hours between morning and afternoon

/////////////////////////////////////////////////////////////////////////////////////////////

		//not causing more than teachersMaxGapsPerWeek teachers gaps
		okteachersmaxgapsperweek=true;
		for(int tch : qAsConst(act->iTeachersList))
			if(!skipRandom(teachersMaxGapsPerWeekPercentage[tch])){
				assert(teachersMaxGapsPerWeekPercentage[tch]==100);
				
				//preliminary test
				int _nHours=0;
				int _nGaps=0;
				for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
					_nHours+=newTeachersDayNHours(tch,d2);
					_nGaps+=newTeachersDayNGaps(tch,d2);
				}
				
				if(_nGaps+_nHours > teachersMaxGapsPerWeekMaxGaps[tch]+nHoursPerTeacher[tch]){
			
					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okteachersmaxgapsperweek=false;
						goto impossibleteachersmaxgapsperweek;
					}
					
					getTchTimetable(tch, conflActivities[newtime]);
					tchGetNHoursGaps(tch);

					for(;;){
						int nHours=0;
						int nGaps=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							nHours+=tchDayNHours[d2];
							nGaps+=tchDayNGaps[d2];
						}
						
						int ai2=-1;
						
						if(nGaps+nHours > teachersMaxGapsPerWeekMaxGaps[tch]+nHoursPerTeacher[tch]){
							//remove an activity
							bool k=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okteachersmaxgapsperweek=false;
								goto impossibleteachersmaxgapsperweek;
							}
						}
						else{ //OK
							break;
						}
						
						assert(ai2>=0);

						removeAi2FromTchTimetable(ai2);
						updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
					}
				}
			}
		
impossibleteachersmaxgapsperweek:
		if(!okteachersmaxgapsperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END max gaps per week

/////////////////////////////////////////////////////////////////////////////////////////////

		//not causing more than teachersMaxGapsPerDay teachers gaps
		okteachersmaxgapsperday=true;
		if(gt.rules.mode!=MORNINGS_AFTERNOONS){
			for(int tch : qAsConst(act->iTeachersList))
				if(!skipRandom(teachersMaxGapsPerDayPercentage[tch])){
					assert(teachersMaxGapsPerDayPercentage[tch]==100);

					//preliminary test
					int _total=0;
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						_total+=newTeachersDayNHours(tch,d2);
						if(teachersMaxGapsPerDayMaxGaps[tch]<newTeachersDayNGaps(tch,d2))
							_total+=newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch];
					}
					if(_total<=nHoursPerTeacher[tch]) //OK
						continue;

					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okteachersmaxgapsperday=false;
						goto impossibleteachersmaxgapsperday;
					}

					getTchTimetable(tch, conflActivities[newtime]);
					tchGetNHoursGaps(tch);

					for(;;){
						int total=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							total+=tchDayNHours[d2];
							if(teachersMaxGapsPerDayMaxGaps[tch]<tchDayNGaps[d2])
								total+=tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch];
						}
						if(total<=nHoursPerTeacher[tch]) //OK
							break;

						//remove an activity from the beginning or from the end of a day
						//following code is identical to maxgapsperweek
						//remove an activity
						int ai2=-1;

						//it should also be allowed to take from anywhere, but it is risky to change now
						bool k=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!k){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachersmaxgapsperday=false;
							goto impossibleteachersmaxgapsperday;
						}

						assert(ai2>=0);

						removeAi2FromTchTimetable(ai2);
						updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
					}
				}
		}
		else{
			for(int tch : qAsConst(act->iTeachersList))
				if(!skipRandom(teachersMaxGapsPerDayPercentage[tch])){
					assert(teachersMaxGapsPerDayPercentage[tch]==100);

					//preliminary test
					int _total=0;
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						_total+=newTeachersDayNHours(tch,d2);
						if(teacherNoGapsPerAfternoon(tch)){
							//2019-09-13 - max gaps per afternoon = 0
							if(d2%2==0){ //morning
								if(teachersMaxGapsPerDayMaxGaps[tch]<newTeachersDayNGaps(tch,d2))
									_total+=newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch];
							}
							else{ //afternoon
								_total+=newTeachersDayNGaps(tch,d2);
							}
						}
						else{
							if(teachersMaxGapsPerDayMaxGaps[tch]<newTeachersDayNGaps(tch,d2))
								_total+=newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch];
						}
					}
					if(_total<=nHoursPerTeacher[tch]) //OK
						continue;

					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okteachersmaxgapsperday=false;
						goto impossibleteachersmaxgapsperday;
					}

					getTchTimetable(tch, conflActivities[newtime]);
					tchGetNHoursGaps(tch);

					for(;;){
						int total=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							total+=tchDayNHours[d2];
							if(teacherNoGapsPerAfternoon(tch)){
								//2019-09-13 - max gaps per afternoon = 0
								if(d2%2==0){ //morning
									if(teachersMaxGapsPerDayMaxGaps[tch]<tchDayNGaps[d2])
										total+=tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch];
								}
								else{ //afternoon
									total+=tchDayNGaps[d2];
								}
							}
							else{
								if(teachersMaxGapsPerDayMaxGaps[tch]<tchDayNGaps[d2])
									total+=tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch];
							}
						}
						if(total<=nHoursPerTeacher[tch]) //OK
							break;

						//remove an activity from the beginning or from the end of a day
						//following code is identical to maxgapsperweek
						//remove an activity
						int ai2=-1;

						//it should also be allowed to take from anywhere, but it is risky to change now
						bool k=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!k){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachersmaxgapsperday=false;
							goto impossibleteachersmaxgapsperday;
						}

						assert(ai2>=0);

						/*Activity* act2=&gt.rules.internalActivitiesList[ai2];
						int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
						int h2=c.times[ai2]/gt.rules.nDaysPerWeek;

						for(int dur2=0; dur2<act2->duration; dur2++){
							assert(tchTimetable(d2,h2+dur2)==ai2);
							tchTimetable(d2,h2+dur2)=-1;
						}*/

						removeAi2FromTchTimetable(ai2);
						updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
					}
				}
		}
		
impossibleteachersmaxgapsperday:
		if(!okteachersmaxgapsperday){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END max gaps per day

/////////////////////////////////////////////////////////////////////////////////////////////

		//not causing more than teachersMaxGapsPerMorningAndAfternoon teachers gaps
		okteachersmaxgapspermorningandafternoon=true;
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int tch : qAsConst(act->iTeachersList))
				if(!skipRandom(teachersMaxGapsPerMorningAndAfternoonPercentage[tch])){
					assert(teachersMaxGapsPerMorningAndAfternoonPercentage[tch]==100);

					//preliminary test
					int _total=0;
					for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
						int dm=d2*2, da=d2*2+1;
						_total+=newTeachersDayNHours(tch,dm)+newTeachersDayNHours(tch,da);
						if(teacherNoGapsPerAfternoon(tch)){
							//2019-09-13 - max gaps per afternoon = 0
							if(teachersMaxGapsPerMorningAndAfternoonMaxGaps[tch]<newTeachersDayNGaps(tch,dm))
								_total+=newTeachersDayNGaps(tch,dm)-teachersMaxGapsPerMorningAndAfternoonMaxGaps[tch];
							_total+=newTeachersDayNGaps(tch,da);
						}
						else{
							if(teachersMaxGapsPerMorningAndAfternoonMaxGaps[tch]<newTeachersDayNGaps(tch,dm)+newTeachersDayNGaps(tch,da))
								_total+=newTeachersDayNGaps(tch,dm)+newTeachersDayNGaps(tch,da)-teachersMaxGapsPerMorningAndAfternoonMaxGaps[tch];
						}
					}
					if(_total<=nHoursPerTeacher[tch]) //OK
						continue;

					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okteachersmaxgapspermorningandafternoon=false;
						goto impossibleteachersmaxgapspermorningandafternoon;
					}

					getTchTimetable(tch, conflActivities[newtime]);
					tchGetNHoursGaps(tch);

					for(;;){
						int total=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
							int dm=d2*2, da=d2*2+1;
							total+=tchDayNHours[dm]+tchDayNHours[da];
							if(teacherNoGapsPerAfternoon(tch)){
								//2019-09-13 - max gaps per afternoon = 0
								if(teachersMaxGapsPerMorningAndAfternoonMaxGaps[tch]<tchDayNGaps[dm])
									total+=tchDayNGaps[dm]-teachersMaxGapsPerMorningAndAfternoonMaxGaps[tch];
								total+=tchDayNGaps[da];
							}
							else{
								if(teachersMaxGapsPerMorningAndAfternoonMaxGaps[tch]<tchDayNGaps[dm]+tchDayNGaps[da])
									total+=tchDayNGaps[dm]+tchDayNGaps[da]-teachersMaxGapsPerMorningAndAfternoonMaxGaps[tch];
							}
						}
						if(total<=nHoursPerTeacher[tch]) //OK
							break;

						//remove an activity from the beginning or from the end of a day
						//following code is identical to maxgapsperweek
						//remove an activity
						int ai2=-1;

						//it should also be allowed to take from anywhere, but it is risky to change now
						bool k=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!k){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachersmaxgapspermorningandafternoon=false;
							goto impossibleteachersmaxgapspermorningandafternoon;
						}

						assert(ai2>=0);

						/*Activity* act2=&gt.rules.internalActivitiesList[ai2];
						int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
						int h2=c.times[ai2]/gt.rules.nDaysPerWeek;

						for(int dur2=0; dur2<act2->duration; dur2++){
							assert(tchTimetable(d2,h2+dur2)==ai2);
							tchTimetable(d2,h2+dur2)=-1;
						}*/

						removeAi2FromTchTimetable(ai2);
						updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
					}
				}
		}
		
impossibleteachersmaxgapspermorningandafternoon:
		if(!okteachersmaxgapspermorningandafternoon){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END max gaps per morning and afternoon

/////////////////////////////////////////////////////////////////////////////////////////////

		//2019-09-13 - max 0 gaps per afternoon
		okteachersmax0gapsperafternoon=true;
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int tch : qAsConst(act->iTeachersList))
				if(teacherNoGapsPerAfternoon(tch)){
					//assert(teachersMaxGapsPerDayPercentage[tch]==100);

					//preliminary test
					int _total=0;
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						_total+=newTeachersDayNHours(tch,d2);
						//2019-09-13 - max gaps per afternoon = 0
						if(d2%2==0){ //morning
							//Careful: the max gaps per day may be -1.
							//And anyway I don't need this test, because I tested in the max gaps per day constraint above.
							//if(teachersMaxGapsPerDayMaxGaps[tch]<newTeachersDayNGaps(tch,d2))
							//	_total+=newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch];
						}
						else{ //afternoon
							_total+=newTeachersDayNGaps(tch,d2);
						}
					}
					if(_total<=nHoursPerTeacher[tch]) //OK
						continue;

					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okteachersmax0gapsperafternoon=false;
						goto impossibleteachersmax0gapsperafternoon;
					}

					getTchTimetable(tch, conflActivities[newtime]);
					tchGetNHoursGaps(tch);

					for(;;){
						int total=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							total+=tchDayNHours[d2];
							//2019-09-13 - max gaps per afternoon = 0
							if(d2%2==0){ //morning
								//Careful: the max gaps per day may be -1.
								//And anyway I don't need this test, because I tested in the max gaps per day constraint above.
								//if(teachersMaxGapsPerDayMaxGaps[tch]<tchDayNGaps[d2])
								//	total+=tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch];
							}
							else{ //afternoon
								total+=tchDayNGaps[d2];
							}
						}
						if(total<=nHoursPerTeacher[tch]) //OK
							break;

						//remove an activity from the beginning or from the end of a day
						//following code is identical to maxgapsperweek
						//remove an activity
						int ai2=-1;

						//it should also be allowed to take from anywhere, but it is risky to change now
						bool k=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						if(!k){
							if(level==0){
								//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
								//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
							}
							okteachersmax0gapsperafternoon=false;
							goto impossibleteachersmax0gapsperafternoon;
						}

						assert(ai2>=0);

						/*Activity* act2=&gt.rules.internalActivitiesList[ai2];
						int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
						int h2=c.times[ai2]/gt.rules.nDaysPerWeek;

						for(int dur2=0; dur2<act2->duration; dur2++){
							assert(tchTimetable(d2,h2+dur2)==ai2);
							tchTimetable(d2,h2+dur2)=-1;
						}*/

						removeAi2FromTchTimetable(ai2);
						updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
					}
				}
		}

impossibleteachersmax0gapsperafternoon:
		if(!okteachersmax0gapsperafternoon){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END max 0 gaps per afternoon

/////////////////////////////////////////////////////////////////////////////////////////////
//2020-07-29

		//not causing more than teachersMaxGapsPerWeekForRealDays teachers gaps
		okteachersmaxgapsperweekforrealdays=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(haveTeachersMaxGapsPerRealDay){
				for(int tch : qAsConst(act->iTeachersList))
					if(!skipRandom(teachersMaxGapsPerWeekForRealDaysPercentage[tch])){
						assert(teachersMaxGapsPerWeekForRealDaysPercentage[tch]==100);

						//preliminary test
						int _total=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++)
							_total+=newTeachersRealDayNHours(tch,d2)+newTeachersRealDayNGaps(tch,d2);

						if(_total<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekForRealDaysMaxGaps[tch]) //OK
							continue;

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersmaxgapsperweekforrealdays=false;
							goto impossibleteachersmaxgapsperweekforrealdays;
						}

						getTchTimetable(tch, conflActivities[newtime]);
						tchGetNHoursGapsRealDays(tch);
						tchGetNHoursGaps(tch); //bug fix on 2020-02-29, because the remove an activity function below needs to know the number of hours per half-day.

						for(;;){
							int total=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++)
								total+=tchRealDayNHours[d2]+tchRealDayNGaps[d2];

							if(total<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekForRealDaysMaxGaps[tch]) //OK
								break;

							//remove an activity from the beginning or from the end of a real day
							//following code is identical to maxgapsperweek
							//remove an activity
							int ai2=-1;

							//it should also be allowed to take from anywhere, but it is risky to change now
							bool k=teacherRemoveAnActivityFromBeginMorningOrEndAfternoon(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okteachersmaxgapsperweekforrealdays=false;
								goto impossibleteachersmaxgapsperweekforrealdays;
							}

							assert(ai2>=0);

							removeAi2FromTchTimetable(ai2);
							updateTchNHoursGapsRealDay(tch, (c.times[ai2]%gt.rules.nDaysPerWeek)/2);
							updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek); //bug fix on 2020-02-29, because the remove an activity function above needs to know the number of hours per half-day.
						}
					}
			}
		}

impossibleteachersmaxgapsperweekforrealdays:
		if(!okteachersmaxgapsperweekforrealdays){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END teachers max gaps per week for real days

/////////////////////////////////////////////////////////////////////////////////////////////

		//not causing more than teachersMaxGapsPerRealDay teachers gaps
		okteachersmaxgapsperrealday=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(haveTeachersMaxGapsPerRealDay){
				for(int tch : qAsConst(act->iTeachersList))
					if(!skipRandom(teachersMaxGapsPerRealDayPercentage[tch])){
						assert(teachersMaxGapsPerRealDayPercentage[tch]==100);

						//preliminary test
						int _total=0;
						int _texcept;
						if(teachersMaxGapsPerRealDayAllowException[tch]==true)
							_texcept=1;
						else
							_texcept=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
							_total+=newTeachersRealDayNHours(tch,d2);
							if(teachersMaxGapsPerRealDayMaxGaps[tch]<newTeachersRealDayNGaps(tch,d2)){
								_total+=newTeachersRealDayNGaps(tch,d2)-teachersMaxGapsPerRealDayMaxGaps[tch];
								if(_texcept>0){
									_texcept--;
									_total--;
								}
							}
						}

						if(_total<=nHoursPerTeacher[tch]) //OK
							continue;

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersmaxgapsperrealday=false;
							goto impossibleteachersmaxgapsperrealday;
						}

						getTchTimetable(tch, conflActivities[newtime]);
						tchGetNHoursGapsRealDays(tch);
						tchGetNHoursGaps(tch); //bug fix on 2020-02-29, because the remove an activity function below needs to know the number of hours per half-day.

						for(;;){
							int total=0;
							int texcept;
							if(teachersMaxGapsPerRealDayAllowException[tch]==true)
								texcept=1;
							else
								texcept=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
								total+=tchRealDayNHours[d2];
								if(teachersMaxGapsPerRealDayMaxGaps[tch]<tchRealDayNGaps[d2]){
									total+=tchRealDayNGaps[d2]-teachersMaxGapsPerRealDayMaxGaps[tch];
									if(texcept>0){
										texcept--;
										total--;
									}
								}
							}
							if(total<=nHoursPerTeacher[tch]) //OK
								break;

							//remove an activity from the beginning or from the end of a real day
							//following code is identical to maxgapsperweek
							//remove an activity
							int ai2=-1;

							//it should also be allowed to take from anywhere, but it is risky to change now
							bool k=teacherRemoveAnActivityFromBeginMorningOrEndAfternoon(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								if(level==0){
									//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
									//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
								}
								okteachersmaxgapsperrealday=false;
								goto impossibleteachersmaxgapsperrealday;
							}

							assert(ai2>=0);

							/*Activity* act2=&gt.rules.internalActivitiesList[ai2];
							int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
							int h2=c.times[ai2]/gt.rules.nDaysPerWeek;

							for(int dur2=0; dur2<act2->duration; dur2++){
								assert(tchTimetable(d2,h2+dur2)==ai2);
								tchTimetable(d2,h2+dur2)=-1;
							}*/

							removeAi2FromTchTimetable(ai2);
							updateTchNHoursGapsRealDay(tch, (c.times[ai2]%gt.rules.nDaysPerWeek)/2);
							updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek); //bug fix on 2020-02-29, because the remove an activity function above needs to know the number of hours per half-day.
						}
					}
			}
		}

impossibleteachersmaxgapsperrealday:
		if(!okteachersmaxgapsperrealday){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		////////////////////////////END teachers max gaps per real day

/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from teachers max hours daily per real day

		//!!!after max gaps per week and max gaps per day

		okteachersmaxhoursdailyrealdays=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int tch : qAsConst(act->iTeachersList)){
				for(int count=0; count<2; count++){
					int limitHoursDaily;
					double percentage;
					if(count==0){
						limitHoursDaily=teachersMaxHoursDailyRealDaysMaxHours1[tch];
						percentage=teachersMaxHoursDailyRealDaysPercentages1[tch];
					}
					else{
						limitHoursDaily=teachersMaxHoursDailyRealDaysMaxHours2[tch];
						percentage=teachersMaxHoursDailyRealDaysPercentages2[tch];
					}

					if(limitHoursDaily<0)
						continue;

					//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
					//	continue;

					bool increased;
					//2019-09-13 - max gaps per afternoon = 0
					//2021-04-22: the tests below involve uninitialized variables, as reported by Valgrind.
					/*if(teachersMaxGapsPerWeekPercentage[tch]>=0 || teachersMaxGapsPerDayPercentage[tch]>=0 || teacherNoGapsPerAfternoon(tch)){
						if(newTeachersDayNHours(tch,d)+newTeachersDayNHours(tch,dpair) > oldTeachersDayNHours(tch,d)+oldTeachersDayNHours(tch,dpair)
						  || newTeachersDayNHours(tch,d)+newTeachersDayNGaps(tch,d)+newTeachersDayNHours(tch,dpair)+newTeachersDayNGaps(tch,dpair)
						  > oldTeachersDayNHours(tch,d)+oldTeachersDayNGaps(tch,d)+
						  oldTeachersDayNHours(tch,dpair)+oldTeachersDayNGaps(tch,dpair))
							increased=true;
						else
							increased=false;
					}
					else{
						if(newTeachersDayNHours(tch,d)+newTeachersDayNHours(tch,dpair) > oldTeachersDayNHours(tch,d)+oldTeachersDayNHours(tch,dpair))
							increased=true;
						else
							increased=false;
					}*/
					/*
					if(newTeachersDayNHours(tch,d) > oldTeachersDayNHours(tch,d))
						increased=true;
					else
						increased=false;*/

					//Liviu Lalescu 2021-03-29: I think this needs to remain true, because of gaps per real day/real day per week.
					///???????? TODO
					increased=true; /////???????????

					if(limitHoursDaily>=0 && !skipRandom(percentage) && increased){
						if(limitHoursDaily<act->duration){
							okteachersmaxhoursdailyrealdays=false;
							goto impossibleteachersmaxhoursdailyrealdays;
						}

						//preliminary test

						//basically, see that the gaps are enough
						bool _ok;
						if(newTeachersDayNHours(tch,d)+newTeachersDayNHours(tch,dpair)>limitHoursDaily){
							_ok=false;
						}
						else{
							if(teachersMaxGapsPerWeekPercentage[tch]>=0){
								int rg=teachersMaxGapsPerWeekMaxGaps[tch];
								for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
									if(d2!=d/2){
										int dfet1=d2*2, dfet2=d2*2+1;
										int g=limitHoursDaily-newTeachersDayNHours(tch,dfet1)-newTeachersDayNHours(tch,dfet2);
										//TODO: if g lower than 0 make g 0
										//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
										g=newTeachersDayNGaps(tch,dfet1)+newTeachersDayNGaps(tch,dfet2)-g;
										if(g>0)
											rg-=g;
									}
								}

								if(rg<0)
									rg=0;

								if(teachersMaxGapsPerDayPercentage[tch]>=0){
									if(teacherNoGapsPerAfternoon(tch)){
										//if(rg>2*teachersMaxGapsPerDayMaxGaps[tch])
										//	rg=2*teachersMaxGapsPerDayMaxGaps[tch];
										//2019-09-13 - max gaps per afternoon = 0
										if(rg>teachersMaxGapsPerDayMaxGaps[tch])
											rg=teachersMaxGapsPerDayMaxGaps[tch];
									}
									else{
										if(rg>2*teachersMaxGapsPerDayMaxGaps[tch])
											rg=2*teachersMaxGapsPerDayMaxGaps[tch];
									}
								}

								int hg=newTeachersDayNGaps(tch,d)+newTeachersDayNGaps(tch,dpair)-rg;
								if(hg<0)
									hg=0;

								if(hg+newTeachersDayNHours(tch,d)+newTeachersDayNHours(tch,dpair) > limitHoursDaily){
									_ok=false;
								}
								else
									_ok=true;
							}
							else{
								int rg=newTeachersDayNGaps(tch,d)+newTeachersDayNGaps(tch,dpair);
								int hg=rg;
								if(teachersMaxGapsPerDayPercentage[tch]>=0){
									if(teacherNoGapsPerAfternoon(tch)){
										//2019-09-13 - max gaps per afternoon = 0
										//if(rg>2*teachersMaxGapsPerDayMaxGaps[tch])
										//	rg=2*teachersMaxGapsPerDayMaxGaps[tch];
										if(rg>teachersMaxGapsPerDayMaxGaps[tch])
											rg=teachersMaxGapsPerDayMaxGaps[tch];
									}
									else{
										if(rg>2*teachersMaxGapsPerDayMaxGaps[tch])
											rg=2*teachersMaxGapsPerDayMaxGaps[tch];
									}
								}
								hg-=rg;
								if(hg+newTeachersDayNHours(tch,d)+newTeachersDayNHours(tch,dpair) > limitHoursDaily)
									_ok=false;
								else
									_ok=true;
							}
						}

						if(_ok){
							continue;
						}

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersmaxhoursdailyrealdays=false;
							goto impossibleteachersmaxhoursdailyrealdays;
						}

						getTchTimetable(tch, conflActivities[newtime]);
						tchGetNHoursGaps(tch);

						//2019-09-13 - max gaps per afternoon = 0
						bool canTakeFromBeginOrEnd=true;
						bool canTakeFromAnywhere=(teachersMaxGapsPerWeekMaxGaps[tch]!=0 && teachersMaxGapsPerDayMaxGaps[tch]!=0); //-1 or >0
						bool canTakeFromBeginOrEndAnyDay=(teacherNoGapsPerAfternoon(tch) || teachersMaxGapsPerWeekMaxGaps[tch]>=0 || teachersMaxGapsPerDayMaxGaps[tch]>=0);

						for(;;){
							//basically, see that the gaps are enough
							bool ok;
							if(tchDayNHours[d]+tchDayNHours[dpair]>limitHoursDaily){
								ok=false;
							}
							else{
								if(teachersMaxGapsPerWeekPercentage[tch]>=0){
									int rg=teachersMaxGapsPerWeekMaxGaps[tch];
									for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++){
										if(d2!=d/2){
											int dfet1=d2*2, dfet2=d2*2+1;
											int g=limitHoursDaily-tchDayNHours[dfet1]-tchDayNHours[dfet2];
											//TODO: if g lower than 0 make g 0
											//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
											g=tchDayNGaps[dfet1]+tchDayNGaps[dfet2]-g;
											if(g>0)
												rg-=g;
										}
									}

									if(rg<0)
										rg=0;

									if(teachersMaxGapsPerDayPercentage[tch]>=0){
										if(teacherNoGapsPerAfternoon(tch)){
											//2019-09-13 - max gaps per afternoon = 0
											//if(rg>2*teachersMaxGapsPerDayMaxGaps[tch])
											//	rg=2*teachersMaxGapsPerDayMaxGaps[tch];
											if(rg>teachersMaxGapsPerDayMaxGaps[tch])
												rg=teachersMaxGapsPerDayMaxGaps[tch];
										}
										else{
											if(rg>2*teachersMaxGapsPerDayMaxGaps[tch])
												rg=2*teachersMaxGapsPerDayMaxGaps[tch];
										}
									}

									int hg=tchDayNGaps[d]+tchDayNGaps[dpair]-rg;
									if(hg<0)
										hg=0;

									if(hg+tchDayNHours[d]+tchDayNHours[dpair] > limitHoursDaily){
										ok=false;
									}
									else
										ok=true;
								}
								else{
									int rg=tchDayNGaps[d]+tchDayNGaps[dpair];
									int hg=rg;
									if(teachersMaxGapsPerDayPercentage[tch]>=0){
										if(teacherNoGapsPerAfternoon(tch)){
												//2019-09-13 - max gaps per afternoon = 0
												//if(rg>2*teachersMaxGapsPerDayMaxGaps[tch])
												//	rg=2*teachersMaxGapsPerDayMaxGaps[tch];
												if(rg>teachersMaxGapsPerDayMaxGaps[tch])
													rg=teachersMaxGapsPerDayMaxGaps[tch];
										}
										else{
											if(rg>2*teachersMaxGapsPerDayMaxGaps[tch])
												rg=2*teachersMaxGapsPerDayMaxGaps[tch];
										}
									}
									hg-=rg;
									if(hg+tchDayNHours[d]+tchDayNHours[dpair] > limitHoursDaily)
										ok=false;
									else
										ok=true;
								}
							}

							if(ok){
								break;
							}

							int ai2=-1;

							bool k=false;
							if(canTakeFromBeginOrEnd)
								k=teacherRemoveAnActivityFromBeginOrEndCertainTwoDays(d, dpair, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								canTakeFromBeginOrEnd=false;
								bool ka=false;
								if(canTakeFromAnywhere)
									ka=teacherRemoveAnActivityFromAnywhereCertainTwoDays(d, dpair, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								if(!ka){
									canTakeFromAnywhere=false;
									bool kaa=false;
									if(canTakeFromBeginOrEndAnyDay && tchDayNHours[d]+tchDayNHours[dpair]<=limitHoursDaily)
										//Fix on 2017-08-26, to solve Volker Dirr's bug report
										kaa=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									if(!kaa){
										canTakeFromBeginOrEndAnyDay=false;

										if(level==0){
											/*cout<<"d=="<<d<<", h=="<<h<<", teacher=="<<qPrintable(gt.rules.internalTeachersList[tch]->name);
											cout<<", ai=="<<ai<<endl;
											for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
												for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
													cout<<"\t"<<tchTimetable(d2,h2)<<"\t";
												cout<<endl;
											}

											cout<<endl;
											for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
												for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
													cout<<"\t"<<newTeachersTimetable(tch,d2,h2)<<"\t";
												cout<<endl;
											}*/

											//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
											//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
										}
										okteachersmaxhoursdailyrealdays=false;
										goto impossibleteachersmaxhoursdailyrealdays;
									}
								}
							}

							assert(ai2>=0);

							removeAi2FromTchTimetable(ai2);
							updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
		}

impossibleteachersmaxhoursdailyrealdays:
		if(!okteachersmaxhoursdailyrealdays){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from teachers max hours daily
		
		//!!!after max gaps per week and max gaps per day
		
		okteachersmaxhoursdaily=true;

		if(gt.rules.mode!=MORNINGS_AFTERNOONS){
			for(int tch : qAsConst(act->iTeachersList)){
				for(int count=0; count<2; count++){
					int limitHoursDaily;
					double percentage;
					if(count==0){
						limitHoursDaily=teachersMaxHoursDailyMaxHours1[tch];
						percentage=teachersMaxHoursDailyPercentages1[tch];
					}
					else{
						limitHoursDaily=teachersMaxHoursDailyMaxHours2[tch];
						percentage=teachersMaxHoursDailyPercentages2[tch];
					}

					if(limitHoursDaily<0)
						continue;

					//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
					//	continue;

					bool increased;
					if(teachersMaxGapsPerWeekPercentage[tch]>=0 || teachersMaxGapsPerDayPercentage[tch]>=0){
						if(newTeachersDayNHours(tch,d) > oldTeachersDayNHours(tch,d)
						  || newTeachersDayNHours(tch,d)+newTeachersDayNGaps(tch,d) > oldTeachersDayNHours(tch,d)+oldTeachersDayNGaps(tch,d))
							increased=true;
						else
							increased=false;
					}
					else{
						if(newTeachersDayNHours(tch,d) > oldTeachersDayNHours(tch,d))
							increased=true;
						else
							increased=false;
					}
					/*
					if(newTeachersDayNHours(tch,d) > oldTeachersDayNHours(tch,d))
						increased=true;
					else
						increased=false;*/

					if(limitHoursDaily>=0 && !skipRandom(percentage) && increased){
						if(limitHoursDaily<act->duration){
							okteachersmaxhoursdaily=false;
							goto impossibleteachersmaxhoursdaily;
						}

						//preliminary test
						bool _ok;
						if(newTeachersDayNHours(tch,d)>limitHoursDaily){
							_ok=false; //trivially
						}
						else{
							//basically, see that the gaps are enough
							// Comment added on 2020-09-15: This code was written a long time ago. It cares that the gaps are enough, but it is more like a heuristic,
							// because the weight might be any real number below 100.0%. So on other days the constraints should be allowed to be broken.
							// However, it is very risky to change now. I think that the best would be to allow max hours daily only with 100.0% weight,
							// but unfortunately I think that many users have files with weight <100.0%.
							// Also, don't forget that we might have two constraints max hours daily for each subgroup.
							if(teachersMaxGapsPerWeekPercentage[tch]>=0){
								int rg=teachersMaxGapsPerWeekMaxGaps[tch];
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(d2!=d){
										int g=limitHoursDaily-newTeachersDayNHours(tch,d2);
										//TODO: if g lower than 0 make g 0
										//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
										g=newTeachersDayNGaps(tch,d2)-g;
										if(g>0)
											rg-=g;
									}
								}

								if(rg<0)
									rg=0;

								if(teachersMaxGapsPerDayPercentage[tch]>=0)
									if(rg>teachersMaxGapsPerDayMaxGaps[tch])
										rg=teachersMaxGapsPerDayMaxGaps[tch];

								int hg=newTeachersDayNGaps(tch,d)-rg;
								if(hg<0)
									hg=0;

								if(hg+newTeachersDayNHours(tch,d) > limitHoursDaily){
									_ok=false;
								}
								else
									_ok=true;
							}
							else{
								int rg=newTeachersDayNGaps(tch,d);
								int hg=rg;
								if(teachersMaxGapsPerDayPercentage[tch]>=0)
									if(rg>teachersMaxGapsPerDayMaxGaps[tch])
										rg=teachersMaxGapsPerDayMaxGaps[tch];
								hg-=rg;
								if(hg+newTeachersDayNHours(tch,d) > limitHoursDaily)
									_ok=false;
								else
									_ok=true;
							}
						}

						if(_ok){
							continue;
						}

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersmaxhoursdaily=false;
							goto impossibleteachersmaxhoursdaily;
						}

						getTchTimetable(tch, conflActivities[newtime]);
						tchGetNHoursGaps(tch);

						bool canTakeFromBeginOrEnd=true;
						bool canTakeFromAnywhere=(teachersMaxGapsPerWeekMaxGaps[tch]!=0 && teachersMaxGapsPerDayMaxGaps[tch]!=0); //-1 or >0
						bool canTakeFromBeginOrEndAnyDay=(teachersMaxGapsPerWeekMaxGaps[tch]>=0 || teachersMaxGapsPerDayMaxGaps[tch]>=0);

						for(;;){
							bool ok;
							if(tchDayNHours[d]>limitHoursDaily){
								ok=false; //trivially
							}
							else{
								//basically, see that the gaps are enough
								// Comment added on 2020-09-15: This code was written a long time ago. It cares that the gaps are enough, but it is more like a heuristic,
								// because the weight might be any real number below 100.0%. So on other days the constraints should be allowed to be broken.
								// However, it is very risky to change now. I think that the best would be to allow max hours daily only with 100.0% weight,
								// but unfortunately I think that many users have files with weight <100.0%.
								// Also, don't forget that we might have two constraints max hours daily for each subgroup.
								if(teachersMaxGapsPerWeekPercentage[tch]>=0){
									int rg=teachersMaxGapsPerWeekMaxGaps[tch];
									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										if(d2!=d){
											int g=limitHoursDaily-tchDayNHours[d2];
											//TODO: if g lower than 0 make g 0
											//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
											g=tchDayNGaps[d2]-g;
											if(g>0)
												rg-=g;
										}
									}

									if(rg<0)
										rg=0;

									if(teachersMaxGapsPerDayPercentage[tch]>=0)
										if(rg>teachersMaxGapsPerDayMaxGaps[tch])
											rg=teachersMaxGapsPerDayMaxGaps[tch];

									int hg=tchDayNGaps[d]-rg;
									if(hg<0)
										hg=0;

									if(hg+tchDayNHours[d] > limitHoursDaily){
										ok=false;
									}
									else
										ok=true;
								}
								else{
									int rg=tchDayNGaps[d];
									int hg=rg;
									if(teachersMaxGapsPerDayPercentage[tch]>=0)
										if(rg>teachersMaxGapsPerDayMaxGaps[tch])
											rg=teachersMaxGapsPerDayMaxGaps[tch];
									hg-=rg;
									if(hg+tchDayNHours[d] > limitHoursDaily)
										ok=false;
									else
										ok=true;
								}
							}

							if(ok){
								break;
							}

							int ai2=-1;

							bool k=false;
							if(canTakeFromBeginOrEnd)
								k=teacherRemoveAnActivityFromBeginOrEndCertainDay(d, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								canTakeFromBeginOrEnd=false;
								bool ka=false;
								if(canTakeFromAnywhere)
									ka=teacherRemoveAnActivityFromAnywhereCertainDay(d, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								if(!ka){
									canTakeFromAnywhere=false;
									bool kaa=false;
									if(canTakeFromBeginOrEndAnyDay && tchDayNHours[d]<=limitHoursDaily)
										//Fix on 2017-08-26, to solve Volker Dirr's bug report
										kaa=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									if(!kaa){
										canTakeFromBeginOrEndAnyDay=false;

										if(level==0){
											//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
											//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
										}
										okteachersmaxhoursdaily=false;
										goto impossibleteachersmaxhoursdaily;
									}
								}
							}

							assert(ai2>=0);

							removeAi2FromTchTimetable(ai2);
							updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
		}
		else{ //max hours daily for half-days for the mornings-afternoons version
			for(int tch : qAsConst(act->iTeachersList)){
				for(int count=0; count<2; count++){
					int limitHoursDaily;
					double percentage;
					if(count==0){
						limitHoursDaily=teachersMaxHoursDailyMaxHours1[tch];
						percentage=teachersMaxHoursDailyPercentages1[tch];
					}
					else{
						limitHoursDaily=teachersMaxHoursDailyMaxHours2[tch];
						percentage=teachersMaxHoursDailyPercentages2[tch];
					}

					if(limitHoursDaily<0)
						continue;

					//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
					//	continue;

					bool increased;
					if(teachersMaxGapsPerWeekPercentage[tch]>=0 || teachersMaxGapsPerDayPercentage[tch]>=0){
						if(newTeachersDayNHours(tch,d) > oldTeachersDayNHours(tch,d)
						  || newTeachersDayNHours(tch,d)+newTeachersDayNGaps(tch,d) > oldTeachersDayNHours(tch,d)+oldTeachersDayNGaps(tch,d))
							increased=true;
						else
							increased=false;
					}
					else{
						if(newTeachersDayNHours(tch,d) > oldTeachersDayNHours(tch,d))
							increased=true;
						else
							increased=false;
					}
					/*
					if(newTeachersDayNHours(tch,d) > oldTeachersDayNHours(tch,d))
						increased=true;
					else
						increased=false;*/

					///???????? TODO
					//commented out on 2019-07-02
					//increased=true; /////???????????

					if(limitHoursDaily>=0 && !skipRandom(percentage) && increased){
						if(limitHoursDaily<act->duration){
							okteachersmaxhoursdaily=false;
							goto impossibleteachersmaxhoursdaily;
						}

						//preliminary test

						//basically, see that the gaps are enough
						bool _ok;
						if(newTeachersDayNHours(tch,d)>limitHoursDaily){
							_ok=false;
						}
						else{
							if(teachersMaxGapsPerWeekPercentage[tch]>=0){
								int rg=teachersMaxGapsPerWeekMaxGaps[tch];
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									if(d2!=d){
										int g=limitHoursDaily-newTeachersDayNHours(tch,d2);
										//TODO: if g lower than 0 make g 0
										//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
										g=newTeachersDayNGaps(tch,d2)-g;
										if(g>0)
											rg-=g;
									}
								}

								if(rg<0)
									rg=0;

								if(teachersMaxGapsPerDayPercentage[tch]>=0){
									if(teacherNoGapsPerAfternoon(tch)){
										//2019-09-13 - max gaps per afternoon = 0
										if(d%2==0){ //morning
											if(rg>teachersMaxGapsPerDayMaxGaps[tch]){
												rg=teachersMaxGapsPerDayMaxGaps[tch];
											}
										}
										else{ //afternoon
											rg=0;
										}
									}
									else{
										if(rg>teachersMaxGapsPerDayMaxGaps[tch])
											rg=teachersMaxGapsPerDayMaxGaps[tch];
									}
								}

								int hg=newTeachersDayNGaps(tch,d)-rg;
								if(hg<0)
									hg=0;

								if(hg+newTeachersDayNHours(tch,d) > limitHoursDaily){
									_ok=false;
								}
								else
									_ok=true;
							}
							else{
								int rg=newTeachersDayNGaps(tch,d);
								int hg=rg;
								if(teachersMaxGapsPerDayPercentage[tch]>=0){
									if(teacherNoGapsPerAfternoon(tch)){
										//2019-09-13 - max gaps per afternoon = 0
										if(d%2==0){ //morning
											if(rg>teachersMaxGapsPerDayMaxGaps[tch]){
												rg=teachersMaxGapsPerDayMaxGaps[tch];
											}
										}
										else{ //afternoon
											rg=0;
										}
									}
									else{
										if(rg>teachersMaxGapsPerDayMaxGaps[tch])
											rg=teachersMaxGapsPerDayMaxGaps[tch];
									}
								}
								hg-=rg;
								if(hg+newTeachersDayNHours(tch,d) > limitHoursDaily)
									_ok=false;
								else
									_ok=true;
							}
						}

						if(_ok){
							continue;
						}

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersmaxhoursdaily=false;
							goto impossibleteachersmaxhoursdaily;
						}

						getTchTimetable(tch, conflActivities[newtime]);
						tchGetNHoursGaps(tch);

						//2019-09-13 - max gaps per afternoon = 0
						bool canTakeFromBeginOrEnd=true;
						bool canTakeFromAnywhere=(teachersMaxGapsPerWeekMaxGaps[tch]!=0 && teachersMaxGapsPerDayMaxGaps[tch]!=0); //-1 or >0
						bool canTakeFromBeginOrEndAnyDay=(teacherNoGapsPerAfternoon(tch) || teachersMaxGapsPerWeekMaxGaps[tch]>=0 || teachersMaxGapsPerDayMaxGaps[tch]>=0);

						for(;;){
							//basically, see that the gaps are enough
							bool ok;
							if(tchDayNHours[d]>limitHoursDaily){
								ok=false;
							}
							else{
								if(teachersMaxGapsPerWeekPercentage[tch]>=0){
									int rg=teachersMaxGapsPerWeekMaxGaps[tch];
									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
										if(d2!=d){
											int g=limitHoursDaily-tchDayNHours[d2];
											//TODO: if g lower than 0 make g 0
											//but with this change, speed decreases for file examples/anonymous/1/2008/anonymous-1-2008.fet
											g=tchDayNGaps[d2]-g;
											if(g>0)
												rg-=g;
										}
									}

									if(rg<0)
										rg=0;

									if(teachersMaxGapsPerDayPercentage[tch]>=0){
										if(teacherNoGapsPerAfternoon(tch)){
											//2019-09-13 - max gaps per afternoon = 0
											if(d%2==0){ //morning
												if(rg>teachersMaxGapsPerDayMaxGaps[tch]){
													rg=teachersMaxGapsPerDayMaxGaps[tch];
												}
											}
											else{ //afternoon
												rg=0;
											}
										}
										else{
											if(rg>teachersMaxGapsPerDayMaxGaps[tch])
												rg=teachersMaxGapsPerDayMaxGaps[tch];
										}
									}

									int hg=tchDayNGaps[d]-rg;
									if(hg<0)
										hg=0;

									if(hg+tchDayNHours[d] > limitHoursDaily){
										ok=false;
									}
									else
										ok=true;
								}
								else{
									int rg=tchDayNGaps[d];
									int hg=rg;
									if(teachersMaxGapsPerDayPercentage[tch]>=0){
										if(teacherNoGapsPerAfternoon(tch)){
											//2019-09-13 - max gaps per afternoon = 0
											if(d%2==0){ //morning
												if(rg>teachersMaxGapsPerDayMaxGaps[tch]){
													rg=teachersMaxGapsPerDayMaxGaps[tch];
												}
											}
											else{ //afternoon
												rg=0;
											}
										}
										else{
											if(rg>teachersMaxGapsPerDayMaxGaps[tch])
												rg=teachersMaxGapsPerDayMaxGaps[tch];
										}
									}
									hg-=rg;
									if(hg+tchDayNHours[d] > limitHoursDaily)
										ok=false;
									else
										ok=true;
								}
							}

							if(ok){
								break;
							}

							int ai2=-1;

							bool k=false;
							if(canTakeFromBeginOrEnd)
								k=teacherRemoveAnActivityFromBeginOrEndCertainDay(d, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								canTakeFromBeginOrEnd=false;
								bool ka=false;
								if(canTakeFromAnywhere)
									ka=teacherRemoveAnActivityFromAnywhereCertainDay(d, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								if(!ka){
									canTakeFromAnywhere=false;
									bool kaa=false;
									if(canTakeFromBeginOrEndAnyDay && tchDayNHours[d]<=limitHoursDaily)
										//Fix on 2017-08-26, to solve Volker Dirr's bug report
										kaa=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
									if(!kaa){
										canTakeFromBeginOrEndAnyDay=false;

										if(level==0){
											/*cout<<"d=="<<d<<", h=="<<h<<", teacher=="<<qPrintable(gt.rules.internalTeachersList[tch]->name);
											cout<<", ai=="<<ai<<endl;
											for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
												for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
													cout<<"\t"<<tchTimetable(d2,h2)<<"\t";
												cout<<endl;
											}

											cout<<endl;
											for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
												for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
													cout<<"\t"<<newTeachersTimetable(tch,d2,h2)<<"\t";
												cout<<endl;
											}*/

											//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
											//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
										}
										okteachersmaxhoursdaily=false;
										goto impossibleteachersmaxhoursdaily;
									}
								}
							}

							assert(ai2>=0);

							removeAi2FromTchTimetable(ai2);
							updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
		}
		
impossibleteachersmaxhoursdaily:
		if(!okteachersmaxhoursdaily){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from teachers max hours continuously
		
		okteachersmaxhourscontinuously=true;
		
		for(int tch : qAsConst(act->iTeachersList)){
			for(int count=0; count<2; count++){
				int limitHoursCont;
				double percentage;
				if(count==0){
					limitHoursCont=teachersMaxHoursContinuouslyMaxHours1[tch];
					percentage=teachersMaxHoursContinuouslyPercentages1[tch];
				}
				else{
					limitHoursCont=teachersMaxHoursContinuouslyMaxHours2[tch];
					percentage=teachersMaxHoursContinuouslyPercentages2[tch];
				}
				
				if(limitHoursCont<0) //no constraint
					continue;
				
				//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
				//	continue;
				
				bool increased;
				int h2;
				for(h2=h; h2<h+act->duration; h2++){
					assert(h2<gt.rules.nHoursPerDay);
					if(teachersTimetable(tch,d,h2)==-1)
						break;
				}
				if(h2<h+act->duration)
					increased=true;
				else
					increased=false;
				
				QList<int> removableActs;
				
				int nc=act->duration;
				for(h2=h-1; h2>=0; h2--){
					int ai2=teachersTimetable(tch,d,h2);
					assert(ai2==newTeachersTimetable(tch,d,h2));
					assert(ai2!=ai);
					if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
						nc++;
						
						if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
							removableActs.append(ai2);
					}
					else
						break;
				}
				for(h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
					int ai2=teachersTimetable(tch,d,h2);
					assert(ai2==newTeachersTimetable(tch,d,h2));
					assert(ai2!=ai);
					if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
						nc++;
						
						if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
							removableActs.append(ai2);
					}
					else
						break;
				}
				
				if(!increased && percentage==100.0)
					assert(nc<=limitHoursCont);
				
				if(!increased || nc<=limitHoursCont) //OK
					continue;
				
				assert(limitHoursCont>=0);

				if(!skipRandom(percentage) && increased){
					if(act->duration>limitHoursCont){
						okteachersmaxhourscontinuously=false;
						goto impossibleteachersmaxhourscontinuously;
					}
					
					if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
						okteachersmaxhourscontinuously=false;
						goto impossibleteachersmaxhourscontinuously;
					}
					
					while(true){
						if(removableActs.count()==0){
							okteachersmaxhourscontinuously=false;
							goto impossibleteachersmaxhourscontinuously;
						}
						
						int j=-1;
					
						if(level==0){
							int optMinWrong=INF;
			
							QList<int> tl;

							for(int q=0; q<removableActs.count(); q++){
								int ai2=removableActs.at(q);
								if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
								 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
								}
							}
				
							for(int q=0; q<removableActs.count(); q++){
								int ai2=removableActs.at(q);
								if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
									tl.append(q);
							}
			
							assert(tl.count()>=1);
							j=tl.at(rng.intMRG32k3a(tl.count()));
			
							assert(j>=0 && j<removableActs.count());
						}
						else{
							j=rng.intMRG32k3a(removableActs.count());
						}
						
						assert(j>=0);
						
						int ai2=removableActs.at(j);
						
						int t=removableActs.removeAll(ai2);
						assert(t==1);
						
						assert(!conflActivities[newtime].contains(ai2));
						conflActivities[newtime].append(ai2);
						nConflActivities[newtime]++;
						assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						
						////////////
						removableActs.clear();
						
						int nc=act->duration;
						int h2;
						for(h2=h-1; h2>=0; h2--){
							int ai2=teachersTimetable(tch,d,h2);
							assert(ai2==newTeachersTimetable(tch,d,h2));
							assert(ai2!=ai);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
								nc++;
							
								if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
									removableActs.append(ai2);
							}
							else
								break;
						}
						for(h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
							int ai2=teachersTimetable(tch,d,h2);
							assert(ai2==newTeachersTimetable(tch,d,h2));
							assert(ai2!=ai);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
								nc++;
						
								if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
									removableActs.append(ai2);
							}
							else
								break;
						}
						
						if(nc<=limitHoursCont) //OK
							break;
						////////////
					}
				}
			}
		}
		
impossibleteachersmaxhourscontinuously:
		if(!okteachersmaxhourscontinuously){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from teachers activity tag max hours daily

		//!!!NOT PERFECT, there is room for improvement

		okteachersactivitytagmaxhoursdaily=true;
		
		if(haveTeachersActivityTagMaxHoursDaily){
	
			for(int tch : qAsConst(act->iTeachersList)){
				for(int cnt=0; cnt<teachersActivityTagMaxHoursDailyMaxHours[tch].count(); cnt++){
					int activityTag=teachersActivityTagMaxHoursDailyActivityTag[tch].at(cnt);
				
					if(!gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(activityTag))
						continue;

					int limitHoursDaily=teachersActivityTagMaxHoursDailyMaxHours[tch].at(cnt);
					double percentage=teachersActivityTagMaxHoursDailyPercentage[tch].at(cnt);

					assert(limitHoursDaily>=0);
					assert(percentage>=0);
					assert(activityTag>=0 /*&& activityTag<gt.rules.nInternalActivityTags*/);

					//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
					//	continue;
				
					bool increased;
					
					int nold=0, nnew=0;
					///////////
					for(int h2=0; h2<h; h2++){
						if(newTeachersTimetable(tch,d,h2)>=0){
							int ai2=newTeachersTimetable(tch,d,h2);
							assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
							Activity* act=&gt.rules.internalActivitiesList[ai2];
							if(act->iActivityTagsSet.contains(activityTag)){
								nold++;
								nnew++;
							}
						}
					}
					for(int h2=h; h2<h+act->duration; h2++){
						if(oldTeachersTimetable(tch,d,h2)>=0){
							int ai2=oldTeachersTimetable(tch,d,h2);
							assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
							Activity* act=&gt.rules.internalActivitiesList[ai2];
							if(act->iActivityTagsSet.contains(activityTag))
								nold++;
						}
					}
					for(int h2=h; h2<h+act->duration; h2++){
						if(newTeachersTimetable(tch,d,h2)>=0){
							int ai2=newTeachersTimetable(tch,d,h2);
							assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
							Activity* act=&gt.rules.internalActivitiesList[ai2];
							if(act->iActivityTagsSet.contains(activityTag))
								nnew++;
						}
					}
					for(int h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
						if(newTeachersTimetable(tch,d,h2)>=0){
							int ai2=newTeachersTimetable(tch,d,h2);
							assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
							Activity* act=&gt.rules.internalActivitiesList[ai2];
							if(act->iActivityTagsSet.contains(activityTag)){
								nold++;
								nnew++;
							}
						}
					}
					/////////
					if(nold<nnew)
						increased=true;
					else
						increased=false;
					
					if(percentage==100.0)
						assert(nold<=limitHoursDaily);
					if(!increased && percentage==100.0)
						assert(nnew<=limitHoursDaily);
					
					if(!increased || nnew<=limitHoursDaily) //OK
						continue;
						
					assert(limitHoursDaily>=0);
	
					assert(increased);
					assert(nnew>limitHoursDaily);
					if(!skipRandom(percentage)){
						if(act->duration>limitHoursDaily){
							okteachersactivitytagmaxhoursdaily=false;
							goto impossibleteachersactivitytagmaxhoursdaily;
						}
					
						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersactivitytagmaxhoursdaily=false;
							goto impossibleteachersactivitytagmaxhoursdaily;
						}
					
						getTchTimetable(tch, conflActivities[newtime]);
						tchGetNHoursGaps(tch);
	
						while(true){
							int ncrt=0;
							for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
								if(tchTimetable(d,h2)>=0){
									int ai2=tchTimetable(d,h2);
									assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
									Activity* act=&gt.rules.internalActivitiesList[ai2];
									if(act->iActivityTagsSet.contains(activityTag))
										ncrt++;
								}
							}
							
							if(ncrt<=limitHoursDaily)
								break;
						
							int ai2=-1;
							
							bool ke=teacherRemoveAnActivityFromAnywhereCertainDayCertainActivityTag(d, activityTag, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							
							if(!ke){
								if(level==0){
									//...this is not too good, but hopefully there is no problem
								}
								okteachersactivitytagmaxhoursdaily=false;
								goto impossibleteachersactivitytagmaxhoursdaily;
							}
							
							assert(ai2>=0);
							
							assert(gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag));
							
							removeAi2FromTchTimetable(ai2);
							updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
			
		}
		
impossibleteachersactivitytagmaxhoursdaily:
		if(!okteachersactivitytagmaxhoursdaily){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from teachers activity tag max hours daily for real days

		//!!!NOT PERFECT, there is room for improvement

		okteachersactivitytagmaxhoursdailyrealdays=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			if(haveTeachersActivityTagMaxHoursDailyRealDays){
				for(int tch : qAsConst(act->iTeachersList)){
					for(int cnt=0; cnt<teachersActivityTagMaxHoursDailyRealDaysMaxHours[tch].count(); cnt++){
						int activityTag=teachersActivityTagMaxHoursDailyRealDaysActivityTag[tch].at(cnt);

						if(!gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(activityTag))
							continue;

						int limitHoursDaily=teachersActivityTagMaxHoursDailyRealDaysMaxHours[tch].at(cnt);
						double percentage=teachersActivityTagMaxHoursDailyRealDaysPercentage[tch].at(cnt);

						assert(limitHoursDaily>=0);
						assert(percentage>=0);
						assert(activityTag>=0 /*&& activityTag<gt.rules.nInternalActivityTags*/);

						//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
						//	continue;

						bool increased;

						int nold=0, nnew=0;
						///////////
						for(int h2=0; h2<h; h2++){
							if(newTeachersTimetable(tch,d,h2)>=0){
								int ai2=newTeachersTimetable(tch,d,h2);
								assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
								Activity* act=&gt.rules.internalActivitiesList[ai2];
								if(act->iActivityTagsSet.contains(activityTag)){
									nold++;
									nnew++;
								}
							}
						}
						for(int h2=h; h2<h+act->duration; h2++){
							if(oldTeachersTimetable(tch,d,h2)>=0){
								int ai2=oldTeachersTimetable(tch,d,h2);
								assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
								Activity* act=&gt.rules.internalActivitiesList[ai2];
								if(act->iActivityTagsSet.contains(activityTag))
									nold++;
							}
						}
						for(int h2=h; h2<h+act->duration; h2++){
							if(newTeachersTimetable(tch,d,h2)>=0){
								int ai2=newTeachersTimetable(tch,d,h2);
								assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
								Activity* act=&gt.rules.internalActivitiesList[ai2];
								if(act->iActivityTagsSet.contains(activityTag))
									nnew++;
							}
						}
						for(int h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
							if(newTeachersTimetable(tch,d,h2)>=0){
								int ai2=newTeachersTimetable(tch,d,h2);
								assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
								Activity* act=&gt.rules.internalActivitiesList[ai2];
								if(act->iActivityTagsSet.contains(activityTag)){
									nold++;
									nnew++;
								}
							}
						}
						for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
							if(newTeachersTimetable(tch,dpair,h2)>=0){
								int ai2=newTeachersTimetable(tch,dpair,h2);
								assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
								Activity* act=&gt.rules.internalActivitiesList[ai2];
								if(act->iActivityTagsSet.contains(activityTag)){
									nold++;
									nnew++;
								}
							}
						}
						/////////
						if(nold<nnew)
							increased=true;
						else
							increased=false;

						if(percentage==100.0)
							assert(nold<=limitHoursDaily);
						if(!increased && percentage==100.0)
							assert(nnew<=limitHoursDaily);

						if(!increased || nnew<=limitHoursDaily) //OK
							continue;

						assert(limitHoursDaily>=0);

						assert(increased);
						assert(nnew>limitHoursDaily);
						if(!skipRandom(percentage)){
							if(act->duration>limitHoursDaily){
								okteachersactivitytagmaxhoursdailyrealdays=false;
								goto impossibleteachersactivitytagmaxhoursdailyrealdays;
							}

							if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
								okteachersactivitytagmaxhoursdailyrealdays=false;
								goto impossibleteachersactivitytagmaxhoursdailyrealdays;
							}

							getTchTimetable(tch, conflActivities[newtime]);
							tchGetNHoursGaps(tch);

							while(true){
								int ncrt=0;
								for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
									if(tchTimetable(d,h2)>=0){
										int ai2=tchTimetable(d,h2);
										assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
										Activity* act=&gt.rules.internalActivitiesList[ai2];
										if(act->iActivityTagsSet.contains(activityTag))
											ncrt++;
									}
								}
								for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
									if(tchTimetable(dpair,h2)>=0){
										int ai2=tchTimetable(dpair,h2);
										assert(ai2>=0 && ai2<gt.rules.nInternalActivities);
										Activity* act=&gt.rules.internalActivitiesList[ai2];
										if(act->iActivityTagsSet.contains(activityTag))
											ncrt++;
									}
								}

								if(ncrt<=limitHoursDaily)
									break;

								int ai2=-1;

								bool ke=teacherRemoveAnActivityFromAnywhereCertainDayDayPairCertainActivityTag(d, dpair, activityTag, level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								if(!ke){
									if(level==0){
										//...this is not too good, but hopefully there is no problem
									}
									okteachersactivitytagmaxhoursdailyrealdays=false;
									goto impossibleteachersactivitytagmaxhoursdailyrealdays;
								}

								assert(ai2>=0);

								assert(gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag));

								removeAi2FromTchTimetable(ai2);
								updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
							}
						}
					}
				}
			}
		}

impossibleteachersactivitytagmaxhoursdailyrealdays:
		if(!okteachersactivitytagmaxhoursdailyrealdays){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from teachers activity tag max hours continuously
		okteachersactivitytagmaxhourscontinuously=true;
		
		if(haveTeachersActivityTagMaxHoursContinuously){
		
			for(int tch : qAsConst(act->iTeachersList)){
				for(int cnt=0; cnt<teachersActivityTagMaxHoursContinuouslyMaxHours[tch].count(); cnt++){
					int activityTag=teachersActivityTagMaxHoursContinuouslyActivityTag[tch].at(cnt);
					
					if(!gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(activityTag))
						continue;

					int limitHoursCont=teachersActivityTagMaxHoursContinuouslyMaxHours[tch].at(cnt);
					double percentage=teachersActivityTagMaxHoursContinuouslyPercentage[tch].at(cnt);

					assert(limitHoursCont>=0);
					assert(percentage>=0);
					assert(activityTag>=0/* && activityTag<gt.rules.nInternalActivityTags*/);
					
					//if(fixedTimeActivity[ai] && percentage<100.0) //added on 21 Feb. 2009 in FET 5.9.1, to solve a bug of impossible timetables for fixed timetables
					//	continue;
				
					bool increased;
					int h2;
					for(h2=h; h2<h+act->duration; h2++){
						assert(h2<gt.rules.nHoursPerDay);
						if(teachersTimetable(tch,d,h2)==-1)
							break;
						int ai2=teachersTimetable(tch,d,h2);
						if(!gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag))
							break;
					}
					if(h2<h+act->duration)
						increased=true;
					else
						increased=false;
					
					QList<int> removableActs;
					
					int nc=act->duration;
					for(h2=h-1; h2>=0; h2--){
						int ai2=teachersTimetable(tch,d,h2);
						assert(ai2==newTeachersTimetable(tch,d,h2));
						assert(ai2!=ai);
						if(ai2<0)
							break;
						if(ai2>=0 && !conflActivities[newtime].contains(ai2) &&
						 gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag)){
							nc++;
							
							if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
								removableActs.append(ai2);
						}
						else
							break;
					}
					for(h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
						int ai2=teachersTimetable(tch,d,h2);
						assert(ai2==newTeachersTimetable(tch,d,h2));
						assert(ai2!=ai);
						if(ai2<0)
							break;
						if(ai2>=0 && !conflActivities[newtime].contains(ai2) &&
						 gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag)){
							nc++;
						
							if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
								removableActs.append(ai2);
						}
						else
							break;
					}
					
					if(!increased && percentage==100.0)
						assert(nc<=limitHoursCont);
					
					if(!increased || nc<=limitHoursCont) //OK
						continue;
					
					assert(limitHoursCont>=0);

					if(!skipRandom(percentage) && increased){
						if(act->duration>limitHoursCont){
							okteachersactivitytagmaxhourscontinuously=false;
							goto impossibleteachersactivitytagmaxhourscontinuously;
						}
					
						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersactivitytagmaxhourscontinuously=false;
							goto impossibleteachersactivitytagmaxhourscontinuously;
						}
						
						while(true){
							if(removableActs.count()==0){
								okteachersactivitytagmaxhourscontinuously=false;
								goto impossibleteachersactivitytagmaxhourscontinuously;
							}
							
							int j=-1;
					
							if(level==0){
								int optMinWrong=INF;
				
								QList<int> tl;

								for(int q=0; q<removableActs.count(); q++){
									int ai2=removableActs.at(q);
									if(optMinWrong>triedRemovals(ai2,c.times[ai2])){
									 	optMinWrong=triedRemovals(ai2,c.times[ai2]);
									}
								}
					
								for(int q=0; q<removableActs.count(); q++){
									int ai2=removableActs.at(q);
									if(optMinWrong==triedRemovals(ai2,c.times[ai2]))
										tl.append(q);
								}
				
								assert(tl.count()>=1);
								j=tl.at(rng.intMRG32k3a(tl.count()));
			
								assert(j>=0 && j<removableActs.count());
							}
							else{
								j=rng.intMRG32k3a(removableActs.count());
							}
							
							assert(j>=0);
							
							int ai2=removableActs.at(j);
							
							int t=removableActs.removeAll(ai2);
							assert(t==1);
						
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);
							nConflActivities[newtime]++;
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
						
							////////////
							removableActs.clear();
						
							int nc=act->duration;
							int h2;
							for(h2=h-1; h2>=0; h2--){
								int ai2=teachersTimetable(tch,d,h2);
								assert(ai2==newTeachersTimetable(tch,d,h2));
								assert(ai2!=ai);
								if(ai2<0)
									break;
								if(ai2>=0 && !conflActivities[newtime].contains(ai2) &&
								 gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag)){
									nc++;
							
									if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
										removableActs.append(ai2);
								}
								else
									break;
							}
							for(h2=h+act->duration; h2<gt.rules.nHoursPerDay; h2++){
								int ai2=teachersTimetable(tch,d,h2);
								assert(ai2==newTeachersTimetable(tch,d,h2));
								assert(ai2!=ai);
								if(ai2<0)
									break;
								if(ai2>=0 && !conflActivities[newtime].contains(ai2) &&
								 gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(activityTag)){
									nc++;
							
									if(!removableActs.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
										removableActs.append(ai2);
								}
								else
									break;
							}
							
							if(nc<=limitHoursCont) //OK
								break;
							////////////
						}
					}
				}
			}
			
		}
		
impossibleteachersactivitytagmaxhourscontinuously:
		if(!okteachersactivitytagmaxhourscontinuously){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
				
/////////////////////////////////////////////////////////////////////////////////////////////

		/////////begin teacher(s) min hours daily
		
		//I think it is best to put this routine after max days per week
		
		//Added on 11 September 2009: takes care of teachers min days per week

		okteachersminhoursdaily=true;
		if(gt.rules.mode!=MORNINGS_AFTERNOONS){
			for(int tch : qAsConst(act->iTeachersList)){
				if(teachersMinHoursDailyMinHours[tch][MIN_HOURS_DAILY_INDEX_IN_ARRAY]>=0){
					assert(teachersMinHoursDailyPercentages[tch][MIN_HOURS_DAILY_INDEX_IN_ARRAY]==100);

					int minLimitTch=teachersMinHoursDailyMinHours[tch][MIN_HOURS_DAILY_INDEX_IN_ARRAY];

					bool skip=skipRandom(teachersMinHoursDailyPercentages[tch][MIN_HOURS_DAILY_INDEX_IN_ARRAY]);
					if(!skip){
						//preliminary test
						bool _ok;
						if(teachersMaxGapsPerWeekPercentage[tch]==-1){
							int _reqHours=0;
							int _usedDays=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
								if(newTeachersDayNHours(tch,d2)>0){
									_usedDays++;
									if(teachersMaxGapsPerDayPercentage[tch]==-1){
										_reqHours+=max(newTeachersDayNHours(tch,d2), minLimitTch);
									}
									else{
										int nh=max(0, newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch]);
										_reqHours+=max(newTeachersDayNHours(tch,d2)+nh, minLimitTch);
									}
								}

							if(teachersMinDaysPerWeekPercentages[tch]>=0){
								assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
								assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
								int _md=teachersMinDaysPerWeekMinDays[tch];
								assert(_md>=0);
								if(_md>_usedDays)
									_reqHours+=(_md-_usedDays)*minLimitTch;
							}

							if(_reqHours <= nHoursPerTeacher[tch])
								_ok=true; //ok
							else
								_ok=false;
						}
						else{
							int remG=0;
							int totalH=0;
							int _usedDays=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								int remGDay=newTeachersDayNGaps(tch,d2);
								int h=newTeachersDayNHours(tch,d2);
								if(h>0){
									_usedDays++;
								}
								int addh;
								if(teachersMaxGapsPerDayPercentage[tch]>=0)
									addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
								else
									addh=0;
								remGDay-=addh;
								assert(remGDay>=0);
								h+=addh;
								if(h>0){
									if(h<minLimitTch){
										remGDay-=minLimitTch-h;
										totalH+=minLimitTch;
									}
									else
										totalH+=h;
								}
								if(remGDay>0)
									remG+=remGDay;
							}

							if(teachersMinDaysPerWeekPercentages[tch]>=0){
								assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
								assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
								int _md=teachersMinDaysPerWeekMinDays[tch];
								assert(_md>=0);
								if(_md>_usedDays)
									totalH+=(_md-_usedDays)*minLimitTch;
							}

							if(remG+totalH<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]
							  && totalH<=nHoursPerTeacher[tch])
								_ok=true;
							else
								_ok=false;
						}

						if(_ok)
							continue;

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersminhoursdaily=false;
							goto impossibleteachersminhoursdaily;
						}

						getTchTimetable(tch, conflActivities[newtime]);
						tchGetNHoursGaps(tch);

						for(;;){
							bool ok;
							if(teachersMaxGapsPerWeekPercentage[tch]==-1){
								int _reqHours=0;
								int _usedDays=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
									if(tchDayNHours[d2]>0){
										_usedDays++;
										if(teachersMaxGapsPerDayPercentage[tch]==-1){
											_reqHours+=max(tchDayNHours[d2], minLimitTch);
										}
										else{
											int nh=max(0, tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch]);
											_reqHours+=max(tchDayNHours[d2]+nh, minLimitTch);
										}
									}

								if(teachersMinDaysPerWeekPercentages[tch]>=0){
									assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
									assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
									int _md=teachersMinDaysPerWeekMinDays[tch];
									assert(_md>=0);
									if(_md>_usedDays)
										_reqHours+=(_md-_usedDays)*minLimitTch;
								}

								if(_reqHours <= nHoursPerTeacher[tch])
									ok=true; //ok
								else
									ok=false;
							}
							else{
								int remG=0;
								int totalH=0;
								int _usedDays=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									int remGDay=tchDayNGaps[d2];
									int h=tchDayNHours[d2];
									if(h>0)
										_usedDays++;
									int addh;
									if(teachersMaxGapsPerDayPercentage[tch]>=0)
										addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
									else
										addh=0;
									remGDay-=addh;
									assert(remGDay>=0);
									h+=addh;
									if(h>0){
										if(h<minLimitTch){
											remGDay-=minLimitTch-h;
											totalH+=minLimitTch;
										}
										else
											totalH+=h;
									}
									if(remGDay>0)
										remG+=remGDay;
								}
								if(teachersMinDaysPerWeekPercentages[tch]>=0){
									assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
									assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
									int _md=teachersMinDaysPerWeekMinDays[tch];
									assert(_md>=0);
									if(_md>_usedDays)
										totalH+=(_md-_usedDays)*minLimitTch;
								}

								if(remG+totalH<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]
								  && totalH<=nHoursPerTeacher[tch])
									ok=true;
								else
									ok=false;
							}

							if(ok)
								break;

							int ai2=-1;

							bool k=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								bool ka=teacherRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								if(!ka){
									if(level==0){
										//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okteachersminhoursdaily=false;
									goto impossibleteachersminhoursdaily;
								}
							}

							assert(ai2>=0);

							removeAi2FromTchTimetable(ai2);
							updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
		}
		else{
			for(int tch : qAsConst(act->iTeachersList)){
				if(teachersMinHoursDailyMinHours[tch][1]>=0){
					assert(teachersMinHoursDailyPercentages[tch][1]==100);

					bool skip=skipRandom(teachersMinHoursDailyPercentages[tch][1]);
					if(!skip){
						int mhd[2];
						mhd[1]=1; //afternoon
						mhd[0]=1; //morning, at least as large as for daily
						if(teachersMinHoursDailyPercentages[tch][1]==100){
							assert(mhd[1]<teachersMinHoursDailyMinHours[tch][1]);
							mhd[1]=teachersMinHoursDailyMinHours[tch][1];
						}
						if(teachersMinHoursDailyPercentages[tch][0]==100){
							assert(mhd[0]<teachersMinHoursDailyMinHours[tch][0]);
							mhd[0]=teachersMinHoursDailyMinHours[tch][0];
						}

						if(teachersMinHoursPerAfternoonPercentages[tch]==100){
							assert(mhd[1]<=teachersMinHoursPerAfternoonMinHours[tch]);
							mhd[1]=teachersMinHoursPerAfternoonMinHours[tch];
						}
						
						//preliminary test
						bool _ok;
						if(teachersMaxGapsPerWeekPercentage[tch]==-1){
							int _reqHours=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
								if(newTeachersDayNHours(tch,d2)>0){
									//_usedDays++;
									if(teacherNoGapsPerAfternoon(tch)){
										//2019-09-13 - max gaps per afternoon = 0
										if(teachersMaxGapsPerDayPercentage[tch]==-1){
											if(d2%2==0){ //morning
												_reqHours+=max(newTeachersDayNHours(tch,d2), /*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2]);
											}
											else{ //afternoon
												_reqHours+=max(newTeachersDayNHours(tch,d2)+newTeachersDayNGaps(tch,d2), /*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2]);
											}
										}
										else{
											if(d2%2==0){ //morning
												int nh=max(0, newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch]);
												_reqHours+=max(newTeachersDayNHours(tch,d2)+nh, /*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2]);
											}
											else{ //afternoon
												int nh=max(0, newTeachersDayNGaps(tch,d2)-0);
												_reqHours+=max(newTeachersDayNHours(tch,d2)+nh, /*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2]);
											}
										}
									}
									else{
										if(teachersMaxGapsPerDayPercentage[tch]==-1){
											_reqHours+=max(newTeachersDayNHours(tch,d2), /*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2]);
										}
										else{
											int nh=max(0, newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch]);
											_reqHours+=max(newTeachersDayNHours(tch,d2)+nh, /*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2]);
										}
									}
								}
							
							int _plusMDPW=0;
							if(teachersMinDaysPerWeekPercentages[tch]>=0){
								int _usedDays=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
									if(newTeachersDayNHours(tch,d2)>0)
										_usedDays++;

								assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
								assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
								int _md=teachersMinDaysPerWeekMinDays[tch];
								assert(_md>=0);
								if(_md>_usedDays)
									_plusMDPW=(_md-_usedDays)*teachersMinHoursDailyMinHours[tch][1];
							}

							int _plusMRDPW=0;
							if(teachersMinRealDaysPerWeekPercentages[tch]>=0){
								int _usedRealDays=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++)
									if(newTeachersDayNHours(tch,2*d2)>0 || newTeachersDayNHours(tch,2*d2+1)>0)
										_usedRealDays++;

								assert(_usedRealDays>=0 && _usedRealDays<=gt.rules.nDaysPerWeek/2);
								assert(teachersMinRealDaysPerWeekPercentages[tch]==100.0);
								int _md=teachersMinRealDaysPerWeekMinDays[tch];
								assert(_md>=0);
								if(_md>_usedRealDays)
									_plusMRDPW=(_md-_usedRealDays)*teachersMinHoursDailyMinHours[tch][1];
							}
							
							_reqHours+=max(_plusMDPW, _plusMRDPW);

							if(_reqHours <= nHoursPerTeacher[tch])
								_ok=true; //ok
							else
								_ok=false;
						}
						else{
							int remG=0;
							int totalH=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								int remGDay=newTeachersDayNGaps(tch,d2);
								int h=newTeachersDayNHours(tch,d2);
								/*if(h>0){
									_usedDays++;
								}*/
								int addh;
								//2019-09-13 - max gaps per afternoon = 0
								if(teacherNoGapsPerAfternoon(tch)){
									if(teachersMaxGapsPerDayPercentage[tch]>=0){
										if(d2%2==0){ //morning
											addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
										}
										else{ //afternoon
											addh=max(0, remGDay-0);
										}
									}
									else{
										if(d2%2==0){ //morning
											addh=0;
										}
										else{
											addh=max(0, remGDay-0);
										}
									}
								}
								else{
									if(teachersMaxGapsPerDayPercentage[tch]>=0)
										addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
									else
										addh=0;
								}
								remGDay-=addh;
								assert(remGDay>=0);
								h+=addh;
								if(h>0){
									if(h</*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2]){
										remGDay-=/*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2]-h;
										totalH+=/*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2];
									}
									else
										totalH+=h;
								}
								if(remGDay>0)
									remG+=remGDay;
							}
							
							int _plusMDPW=0;
							if(teachersMinDaysPerWeekPercentages[tch]>=0){
								int _usedDays=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
									if(newTeachersDayNHours(tch,d2)>0)
										_usedDays++;
								
								assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
								assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
								int _md=teachersMinDaysPerWeekMinDays[tch];
								assert(_md>=0);
								if(_md>_usedDays)
									_plusMDPW=(_md-_usedDays)*teachersMinHoursDailyMinHours[tch][1];
							}

							int _plusMRDPW=0;
							if(teachersMinRealDaysPerWeekPercentages[tch]>=0){
								int _usedRealDays=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++)
									if(newTeachersDayNHours(tch,2*d2)>0 || newTeachersDayNHours(tch,2*d2+1)>0)
										_usedRealDays++;

								assert(_usedRealDays>=0 && _usedRealDays<=gt.rules.nDaysPerWeek/2);
								assert(teachersMinRealDaysPerWeekPercentages[tch]==100.0);
								int _md=teachersMinRealDaysPerWeekMinDays[tch];
								assert(_md>=0);
								if(_md>_usedRealDays)
									_plusMRDPW=(_md-_usedRealDays)*teachersMinHoursDailyMinHours[tch][1];
							}
							
							totalH+=max(_plusMDPW, _plusMRDPW);

							if(remG+totalH<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]
							  && totalH<=nHoursPerTeacher[tch])
								_ok=true;
							else
								_ok=false;
						}

						if(_ok)
							continue;

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersminhoursdaily=false;
							goto impossibleteachersminhoursdaily;
						}

						getTchTimetable(tch, conflActivities[newtime]);
						tchGetNHoursGaps(tch);

						for(;;){
							bool ok;
							if(teachersMaxGapsPerWeekPercentage[tch]==-1){
								int _reqHours=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
									if(tchDayNHours[d2]>0){
										//_usedDays++;
										if(teacherNoGapsPerAfternoon(tch)){
											//2019-09-13 - max gaps per afternoon = 0
											if(teachersMaxGapsPerDayPercentage[tch]==-1){
												if(d2%2==0){ //morning
													_reqHours+=max(tchDayNHours[d2], /*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2]);
												}
												else{ //afternoon
													_reqHours+=max(tchDayNHours[d2]+tchDayNGaps[d2], /*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2]);
												}
											}
											else{
												if(d2%2==0){ //morning
													int nh=max(0, tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch]);
													_reqHours+=max(tchDayNHours[d2]+nh, /*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2]);
												}
												else{ //afternoon
													int nh=max(0, tchDayNGaps[d2]-0);
													_reqHours+=max(tchDayNHours[d2]+nh, /*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2]);
												}
											}
										}
										else{
											if(teachersMaxGapsPerDayPercentage[tch]==-1){
												_reqHours+=max(tchDayNHours[d2], /*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2]);
											}
											else{
												int nh=max(0, tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch]);
												_reqHours+=max(tchDayNHours[d2]+nh, /*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2]);
											}
										}
									}

								int _plusMDPW=0;
								if(teachersMinDaysPerWeekPercentages[tch]>=0){
									int _usedDays=0;
									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
										if(tchDayNHours[d2]>0)
											_usedDays++;

									assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
									assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
									int _md=teachersMinDaysPerWeekMinDays[tch];
									assert(_md>=0);
									if(_md>_usedDays)
										_plusMDPW=(_md-_usedDays)*teachersMinHoursDailyMinHours[tch][1];
								}

								int _plusMRDPW=0;
								if(teachersMinRealDaysPerWeekPercentages[tch]>=0){
									int _usedRealDays=0;
									for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++)
										if(tchDayNHours[2*d2]>0 || tchDayNHours[2*d2+1]>0)
											_usedRealDays++;

									assert(_usedRealDays>=0 && _usedRealDays<=gt.rules.nDaysPerWeek/2);
									assert(teachersMinRealDaysPerWeekPercentages[tch]==100.0);
									int _md=teachersMinRealDaysPerWeekMinDays[tch];
									assert(_md>=0);
									if(_md>_usedRealDays)
										_plusMRDPW=(_md-_usedRealDays)*teachersMinHoursDailyMinHours[tch][1];
								}
								
								_reqHours+=max(_plusMDPW, _plusMRDPW);

								if(_reqHours <= nHoursPerTeacher[tch])
									ok=true; //ok
								else
									ok=false;
							}
							else{
								int remG=0;
								int totalH=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									int remGDay=tchDayNGaps[d2];
									int h=tchDayNHours[d2];
									//if(h>0)
									//	_usedDays++;
									int addh;
									if(teacherNoGapsPerAfternoon(tch)){
										//2019-09-13 - max gaps per afternoon = 0
										if(teachersMaxGapsPerDayPercentage[tch]>=0){
											if(d2%2==0){ //morning
												addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
											}
											else{ //afternoon
												addh=max(0, remGDay-0);
											}
										}
										else{
											if(d2%2==0){ //morning
												addh=0;
											}
											else{ //afternoon
												addh=max(0, remGDay-0);
											}
										}
									}
									else{
										if(teachersMaxGapsPerDayPercentage[tch]>=0)
											addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
										else
											addh=0;
									}
									remGDay-=addh;
									assert(remGDay>=0);
									h+=addh;
									if(h>0){
										if(h</*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2]){
											remGDay-=/*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2]-h;
											totalH+=/*teachersMinHoursDailyMinHours[tch][d2%2]*/mhd[d2%2];
										}
										else
											totalH+=h;
									}
									if(remGDay>0)
										remG+=remGDay;
								}

								int _plusMDPW=0;
								if(teachersMinDaysPerWeekPercentages[tch]>=0){
									int _usedDays=0;
									for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
										if(tchDayNHours[d2]>0)
											_usedDays++;

									assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
									assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
									int _md=teachersMinDaysPerWeekMinDays[tch];
									assert(_md>=0);
									if(_md>_usedDays)
										_plusMDPW=(_md-_usedDays)*teachersMinHoursDailyMinHours[tch][1];
								}

								int _plusMRDPW=0;
								if(teachersMinRealDaysPerWeekPercentages[tch]>=0){
									int _usedRealDays=0;
									for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++)
										if(tchDayNHours[2*d2]>0 || tchDayNHours[2*d2+1]>0)
											_usedRealDays++;

									assert(_usedRealDays>=0 && _usedRealDays<=gt.rules.nDaysPerWeek/2);
									assert(teachersMinRealDaysPerWeekPercentages[tch]==100.0);
									int _md=teachersMinRealDaysPerWeekMinDays[tch];
									assert(_md>=0);
									if(_md>_usedRealDays)
										_plusMRDPW=(_md-_usedRealDays)*teachersMinHoursDailyMinHours[tch][1];
								}
								
								totalH+=max(_plusMDPW, _plusMRDPW);

								if(remG+totalH<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]
								  && totalH<=nHoursPerTeacher[tch])
									ok=true;
								else
									ok=false;
							}

							if(ok)
								break;

							int ai2=-1;

							bool k=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								bool ka=teacherRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								if(!ka){
									if(level==0){
										//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okteachersminhoursdaily=false;
									goto impossibleteachersminhoursdaily;
								}
							}

							assert(ai2>=0);

							/*Activity* act2=&gt.rules.internalActivitiesList[ai2];
							int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
							int h2=c.times[ai2]/gt.rules.nDaysPerWeek;

							for(int dur2=0; dur2<act2->duration; dur2++){
								assert(tchTimetable(d2,h2+dur2)==ai2);
								tchTimetable(d2,h2+dur2)=-1;
							}*/

							removeAi2FromTchTimetable(ai2);
							updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
		}
		
impossibleteachersminhoursdaily:
		if(!okteachersminhoursdaily){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/////////end teacher(s) min hours daily

/////////////////////////////////////////////////////////////////////////////////////////////

		/////////begin teacher(s) min hours daily per real day

		//Maybe old comments below:
		//I think it is best to put this routine after max days per week

		//Added on 11 September 2009: takes care of teachers min days per week

		okteachersminhoursdailyrealdays=true;
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int tch : qAsConst(act->iTeachersList)){
				if(teachersMinHoursDailyRealDaysMinHours[tch]>=0){
					assert(teachersMinHoursDailyRealDaysPercentages[tch]==100);

					bool skip=skipRandom(teachersMinHoursDailyRealDaysPercentages[tch]);
					if(!skip){
						//preliminary test
						bool _ok;
						if(teachersMaxGapsPerWeekPercentage[tch]==-1){
							int _reqHours=0;
							for(int rd=0; rd<gt.rules.nDaysPerWeek/2; rd++){
								int d1=rd*2;
								int d2=rd*2+1;
								int _crtReqHoursHalfDays=0;
								int _crtReqHoursWholeDay=0;

								if(teachersMinHoursDailyMinHours[tch][1]>=0){
									int mhd[2];
									mhd[1]=1; //afternoon
									mhd[0]=1; //morning, at least as large as for daily
									if(teachersMinHoursDailyPercentages[tch][1]==100){
										assert(mhd[1]<teachersMinHoursDailyMinHours[tch][1]);
										mhd[1]=teachersMinHoursDailyMinHours[tch][1];
									}
									if(teachersMinHoursDailyPercentages[tch][0]==100){
										assert(mhd[0]<teachersMinHoursDailyMinHours[tch][0]);
										mhd[0]=teachersMinHoursDailyMinHours[tch][0];
									}

									if(teachersMinHoursPerAfternoonPercentages[tch]==100){
										assert(mhd[1]<=teachersMinHoursPerAfternoonMinHours[tch]);
										mhd[1]=teachersMinHoursPerAfternoonMinHours[tch];
									}

									if(teacherNoGapsPerAfternoon(tch)){
										//2019-09-13 - max gaps per afternoon = 0
										if(teachersMaxGapsPerDayPercentage[tch]==-1){
											if(newTeachersDayNHours(tch,d1)>0)
												_crtReqHoursHalfDays+=max(newTeachersDayNHours(tch,d1), /*teachersMinHoursDailyMinHours[tch][0]*/mhd[0]);
											if(newTeachersDayNHours(tch,d2)>0){
												//_crtReqHoursHalfDays+=max(newTeachersDayNHours(tch,d2), teachersMinHoursDailyMinHours[tch][1]);
												int nh=max(0, newTeachersDayNGaps(tch,d2)-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
												_crtReqHoursHalfDays+=max(newTeachersDayNHours(tch,d2)+nh, /*teachersMinHoursDailyMinHours[tch][1]*/mhd[1]);
											}
										}
										else{
											if(newTeachersDayNHours(tch,d1)>0){
												int nh=max(0, newTeachersDayNGaps(tch,d1)-teachersMaxGapsPerDayMaxGaps[tch]);
												_crtReqHoursHalfDays+=max(newTeachersDayNHours(tch,d1)+nh, /*teachersMinHoursDailyMinHours[tch][0]*/mhd[0]);
											}
											if(newTeachersDayNHours(tch,d2)>0){
												int nh=max(0, newTeachersDayNGaps(tch,d2)-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
												_crtReqHoursHalfDays+=max(newTeachersDayNHours(tch,d2)+nh, /*teachersMinHoursDailyMinHours[tch][1]*/mhd[1]);
											}
										}
									}
									else{
										if(teachersMaxGapsPerDayPercentage[tch]==-1){
											if(newTeachersDayNHours(tch,d1)>0)
												_crtReqHoursHalfDays+=max(newTeachersDayNHours(tch,d1), /*teachersMinHoursDailyMinHours[tch][0]*/mhd[0]);
											if(newTeachersDayNHours(tch,d2)>0)
												_crtReqHoursHalfDays+=max(newTeachersDayNHours(tch,d2), /*teachersMinHoursDailyMinHours[tch][1]*/mhd[1]);
										}
										else{
											if(newTeachersDayNHours(tch,d1)>0){
												int nh=max(0, newTeachersDayNGaps(tch,d1)-teachersMaxGapsPerDayMaxGaps[tch]);
												_crtReqHoursHalfDays+=max(newTeachersDayNHours(tch,d1)+nh, /*teachersMinHoursDailyMinHours[tch][0]*/mhd[0]);
											}
											if(newTeachersDayNHours(tch,d2)>0){
												int nh=max(0, newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch]);
												_crtReqHoursHalfDays+=max(newTeachersDayNHours(tch,d2)+nh, /*teachersMinHoursDailyMinHours[tch][1]*/mhd[1]);
											}
										}
									}
								}

								if(teacherNoGapsPerAfternoon(tch)){
									//2019-09-13 - max gaps per afternoon = 0
									if(teachersMaxGapsPerDayPercentage[tch]==-1){
										if(newTeachersDayNHours(tch,d1)+newTeachersDayNHours(tch,d2)>0){
											//_crtReqHoursWholeDay+=max(newTeachersDayNHours(tch,d1)+newTeachersDayNHours(tch,d2), teachersMinHoursDailyRealDaysMinHours[tch]);
											int nh=max(0, newTeachersDayNGaps(tch,d2)-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
											_crtReqHoursWholeDay+=max(newTeachersDayNHours(tch,d1)+newTeachersDayNHours(tch,d2)+nh, teachersMinHoursDailyRealDaysMinHours[tch]);
										}
									}
									else{
										if(newTeachersDayNHours(tch,d1)+newTeachersDayNHours(tch,d2)>0){
											int nh=max(0, newTeachersDayNGaps(tch,d1)-teachersMaxGapsPerDayMaxGaps[tch])+max(0, newTeachersDayNGaps(tch,d2)-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
											_crtReqHoursWholeDay+=max(newTeachersDayNHours(tch,d1)+newTeachersDayNHours(tch,d2)+nh, teachersMinHoursDailyRealDaysMinHours[tch]);
										}
									}
								}
								else{
									if(teachersMaxGapsPerDayPercentage[tch]==-1){
										if(newTeachersDayNHours(tch,d1)+newTeachersDayNHours(tch,d2)>0){
											_crtReqHoursWholeDay+=max(newTeachersDayNHours(tch,d1)+newTeachersDayNHours(tch,d2), teachersMinHoursDailyRealDaysMinHours[tch]);
										}
									}
									else{
										if(newTeachersDayNHours(tch,d1)+newTeachersDayNHours(tch,d2)>0){
											int nh=max(0, newTeachersDayNGaps(tch,d1)-teachersMaxGapsPerDayMaxGaps[tch])+max(0, newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch]);
											_crtReqHoursWholeDay+=max(newTeachersDayNHours(tch,d1)+newTeachersDayNHours(tch,d2)+nh, teachersMinHoursDailyRealDaysMinHours[tch]);
										}
									}
								}

								if(newTeachersDayNHours(tch,d1)>0 || newTeachersDayNHours(tch,d2)>0) //this 'if' is useless
									_reqHours+=max(_crtReqHoursHalfDays, _crtReqHoursWholeDay);
							}

							if(teachersMinRealDaysPerWeekPercentages[tch]>=0){
								int _usedRealDays=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++)
									if(newTeachersDayNHours(tch,2*d2)>0 || newTeachersDayNHours(tch,2*d2+1)>0)
										_usedRealDays++;

								assert(_usedRealDays>=0 && _usedRealDays<=gt.rules.nDaysPerWeek/2);
								assert(teachersMinRealDaysPerWeekPercentages[tch]==100.0);
								int _md=teachersMinRealDaysPerWeekMinDays[tch];
								assert(_md>=0);
								if(_md>_usedRealDays){
									if(teachersMinHoursDailyMinHours[tch][1]>=0){
										_reqHours+=(_md-_usedRealDays)*max(teachersMinHoursDailyMinHours[tch][1], teachersMinHoursDailyRealDaysMinHours[tch]);
									}
									else{
										_reqHours+=(_md-_usedRealDays)*teachersMinHoursDailyRealDaysMinHours[tch];
									}
								}
							}

							if(_reqHours <= nHoursPerTeacher[tch])
								_ok=true; //ok
							else
								_ok=false;
						}
						else{
							//int remG1=0;
							//int remG2=0;
							int remG=0;
							//int totalH1=0;
							//int totalH2=0;
							int totalH=0;
							for(int rd=0; rd<gt.rules.nDaysPerWeek/2; rd++){
								int d1=rd*2;
								int d2=rd*2+1;

								int remGDay1=newTeachersDayNGaps(tch,d1);
								int remGDay2=newTeachersDayNGaps(tch,d2);
								int h1=newTeachersDayNHours(tch,d1);
								int h2=newTeachersDayNHours(tch,d2);

								int addh1, addh2;
								if(teacherNoGapsPerAfternoon(tch)){
									//2019-09-13 - max gaps per afternoon = 0
									if(teachersMaxGapsPerDayPercentage[tch]>=0){
										addh1=max(0, remGDay1-teachersMaxGapsPerDayMaxGaps[tch]);
										addh2=max(0, remGDay2-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
									}
									else{
										addh1=0;
										addh2=max(0, remGDay2-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
									}
								}
								else{
									if(teachersMaxGapsPerDayPercentage[tch]>=0){
										addh1=max(0, remGDay1-teachersMaxGapsPerDayMaxGaps[tch]);
										addh2=max(0, remGDay2-teachersMaxGapsPerDayMaxGaps[tch]);
									}
									else{
										addh1=0;
										addh2=0;
									}
								}

								remGDay1-=addh1;
								remGDay2-=addh2;
								assert(remGDay1>=0);
								assert(remGDay2>=0);
								h1+=addh1;
								h2+=addh2;
								/*if(teachersMinHoursDailyMinHours[tch][1]>=0){
									if(h1>0){
										if(h1<teachersMinHoursDailyMinHours[tch][0]){
											remGDay1-=teachersMinHoursDailyMinHours[tch][0]-h1;
											totalH1+=teachersMinHoursDailyMinHours[tch][0];
										}
										else
											totalH1+=h1;
									}
									if(remGDay1>0)
										remG1+=remGDay1;

									if(h2>0){
										if(h2<teachersMinHoursDailyMinHours[tch][1]){
											remGDay2-=teachersMinHoursDailyMinHours[tch][1]-h2;
											totalH2+=teachersMinHoursDailyMinHours[tch][1];
										}
										else
											totalH2+=h2;
									}
									if(remGDay2>0)
										remG2+=remGDay2;
								}*/
								if(1){
									int remGDay=remGDay1+remGDay2;
									int h=h1+h2;
									if(h>0){
										if(h<teachersMinHoursDailyRealDaysMinHours[tch]){
											remGDay-=teachersMinHoursDailyRealDaysMinHours[tch]-h;
											totalH+=teachersMinHoursDailyRealDaysMinHours[tch];
										}
										else
											totalH+=h;
									}
									if(remGDay>0)
										remG+=remGDay;
								}
							}
							if(teachersMinRealDaysPerWeekPercentages[tch]>=0){
								int _usedRealDays=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++)
									if(newTeachersDayNHours(tch,2*d2)>0 || newTeachersDayNHours(tch,2*d2+1)>0)
										_usedRealDays++;

								assert(_usedRealDays>=0 && _usedRealDays<=gt.rules.nDaysPerWeek/2);
								assert(teachersMinRealDaysPerWeekPercentages[tch]==100.0);
								int _md=teachersMinRealDaysPerWeekMinDays[tch];
								assert(_md>=0);
								if(_md>_usedRealDays){
									if(teachersMinHoursDailyMinHours[tch][1]>=0){
										totalH+=(_md-_usedRealDays)*max(teachersMinHoursDailyMinHours[tch][1], teachersMinHoursDailyRealDaysMinHours[tch]);
									}
									else{
										totalH+=(_md-_usedRealDays)*teachersMinHoursDailyRealDaysMinHours[tch];
									}
								}
							}

							if(remG+totalH<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]
							  /*&& remG1+remG2+totalH1+totalH2<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]*/
							  && totalH<=nHoursPerTeacher[tch]
							  /*&& totalH1+totalH2<=nHoursPerTeacher[tch]*/)
								_ok=true;
							else
								_ok=false;
						}

						if(_ok)
							continue;

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersminhoursdailyrealdays=false;
							goto impossibleteachersminhoursdailyrealdays;
						}

						getTchTimetable(tch, conflActivities[newtime]);
						tchGetNHoursGaps(tch);

						for(;;){
							bool ok;
							if(teachersMaxGapsPerWeekPercentage[tch]==-1){
								int _reqHours=0;
								for(int rd=0; rd<gt.rules.nDaysPerWeek/2; rd++){
									int d1=rd*2;
									int d2=rd*2+1;
									int _crtReqHoursHalfDays=0;
									int _crtReqHoursWholeDay=0;

									if(teachersMinHoursDailyMinHours[tch][1]>=0){
										int mhd[2];
										mhd[1]=1; //afternoon
										mhd[0]=1; //morning, at least as large as for daily
										if(teachersMinHoursDailyPercentages[tch][1]==100){
											assert(mhd[1]<teachersMinHoursDailyMinHours[tch][1]);
											mhd[1]=teachersMinHoursDailyMinHours[tch][1];
										}
										if(teachersMinHoursDailyPercentages[tch][0]==100){
											assert(mhd[0]<teachersMinHoursDailyMinHours[tch][0]);
											mhd[0]=teachersMinHoursDailyMinHours[tch][0];
										}

										if(teachersMinHoursPerAfternoonPercentages[tch]==100){
											assert(mhd[1]<=teachersMinHoursPerAfternoonMinHours[tch]);
											mhd[1]=teachersMinHoursPerAfternoonMinHours[tch];
										}

										if(teacherNoGapsPerAfternoon(tch)){
											//2019-09-13 - max gaps per afternoon = 0
											if(teachersMaxGapsPerDayPercentage[tch]==-1){
												if(tchDayNHours[d1]>0)
													_crtReqHoursHalfDays+=max(tchDayNHours[d1], /*teachersMinHoursDailyMinHours[tch][0]*/mhd[0]);
												if(tchDayNHours[d2]>0){
													int nh=max(0, tchDayNGaps[d2]-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
													_crtReqHoursHalfDays+=max(tchDayNHours[d2]+nh, /*teachersMinHoursDailyMinHours[tch][1]*/mhd[1]);
												}
											}
											else{
												if(tchDayNHours[d1]>0){
													int nh=max(0, tchDayNGaps[d1]-teachersMaxGapsPerDayMaxGaps[tch]);
													_crtReqHoursHalfDays+=max(tchDayNHours[d1]+nh, /*teachersMinHoursDailyMinHours[tch][0]*/mhd[0]);
												}
												if(tchDayNHours[d2]>0){
													int nh=max(0, tchDayNGaps[d2]-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
													_crtReqHoursHalfDays+=max(tchDayNHours[d2]+nh, /*teachersMinHoursDailyMinHours[tch][1]*/mhd[1]);
												}
											}
										}
										else{
											if(teachersMaxGapsPerDayPercentage[tch]==-1){
												if(tchDayNHours[d1]>0)
													_crtReqHoursHalfDays+=max(tchDayNHours[d1], /*teachersMinHoursDailyMinHours[tch][0]*/mhd[0]);
												if(tchDayNHours[d2]>0)
													_crtReqHoursHalfDays+=max(tchDayNHours[d2], /*teachersMinHoursDailyMinHours[tch][1]*/mhd[1]);
											}
											else{
												if(tchDayNHours[d1]>0){
													int nh=max(0, tchDayNGaps[d1]-teachersMaxGapsPerDayMaxGaps[tch]);
													_crtReqHoursHalfDays+=max(tchDayNHours[d1]+nh, /*teachersMinHoursDailyMinHours[tch][0]*/mhd[0]);
												}
												if(tchDayNHours[d2]>0){
													int nh=max(0, tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch]);
													_crtReqHoursHalfDays+=max(tchDayNHours[d2]+nh, /*teachersMinHoursDailyMinHours[tch][1]*/mhd[1]);
												}
											}
										}
									}

									if(teacherNoGapsPerAfternoon(tch)){
										//2019-09-13 - max gaps per afternoon = 0
										if(teachersMaxGapsPerDayPercentage[tch]==-1){
											if(tchDayNHours[d1]+tchDayNHours[d2]>0){
												int nh=max(0, tchDayNGaps[d2]-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
												_crtReqHoursWholeDay+=max(tchDayNHours[d1]+tchDayNHours[d2]+nh, teachersMinHoursDailyRealDaysMinHours[tch]);
											}
										}
										else{
											if(tchDayNHours[d1]+tchDayNHours[d2]>0){
												int nh=max(0, tchDayNGaps[d1]-teachersMaxGapsPerDayMaxGaps[tch])+max(0, tchDayNGaps[d2]-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
												_crtReqHoursWholeDay+=max(tchDayNHours[d1]+tchDayNHours[d2]+nh, teachersMinHoursDailyRealDaysMinHours[tch]);
											}
										}
									}
									else{
										if(teachersMaxGapsPerDayPercentage[tch]==-1){
											if(tchDayNHours[d1]+tchDayNHours[d2]>0){
												_crtReqHoursWholeDay+=max(tchDayNHours[d1]+tchDayNHours[d2], teachersMinHoursDailyRealDaysMinHours[tch]);
											}
										}
										else{
											if(tchDayNHours[d1]+tchDayNHours[d2]>0){
												int nh=max(0, tchDayNGaps[d1]-teachersMaxGapsPerDayMaxGaps[tch])+max(0, tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch]);
												_crtReqHoursWholeDay+=max(tchDayNHours[d1]+tchDayNHours[d2]+nh, teachersMinHoursDailyRealDaysMinHours[tch]);
											}
										}
									}

									if(tchDayNHours[d1]>0 || tchDayNHours[d2]>0) //this 'if' is useless
										_reqHours+=max(_crtReqHoursHalfDays, _crtReqHoursWholeDay);
								}
								if(teachersMinRealDaysPerWeekPercentages[tch]>=0){
									int _usedRealDays=0;
									for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++)
										if(tchDayNHours[2*d2]>0 || tchDayNHours[2*d2+1]>0)
											_usedRealDays++;

									assert(_usedRealDays>=0 && _usedRealDays<=gt.rules.nDaysPerWeek/2);
									assert(teachersMinRealDaysPerWeekPercentages[tch]==100.0);
									int _md=teachersMinRealDaysPerWeekMinDays[tch];
									assert(_md>=0);
									if(_md>_usedRealDays){
										if(teachersMinHoursDailyMinHours[tch][1]>=0){
											_reqHours+=(_md-_usedRealDays)*max(teachersMinHoursDailyMinHours[tch][1], teachersMinHoursDailyRealDaysMinHours[tch]);
										}
										else{
											_reqHours+=(_md-_usedRealDays)*teachersMinHoursDailyRealDaysMinHours[tch];
										}
									}
								}

								if(_reqHours <= nHoursPerTeacher[tch])
									ok=true; //ok
								else
									ok=false;
							}
							else{
								//int remG1=0;
								//int remG2=0;
								int remG=0;
								//int totalH1=0;
								//int totalH2=0;
								int totalH=0;
								for(int rd=0; rd<gt.rules.nDaysPerWeek/2; rd++){
									int d1=rd*2;
									int d2=rd*2+1;

									int remGDay1=tchDayNGaps[d1];
									int remGDay2=tchDayNGaps[d2];
									int h1=tchDayNHours[d1];
									int h2=tchDayNHours[d2];

									int addh1, addh2;
									if(teacherNoGapsPerAfternoon(tch)){
										//2019-09-13 - max gaps per afternoon = 0
										if(teachersMaxGapsPerDayPercentage[tch]>=0){
											addh1=max(0, remGDay1-teachersMaxGapsPerDayMaxGaps[tch]);
											addh2=max(0, remGDay2-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
										}
										else{
											addh1=0;
											addh2=max(0, remGDay2-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
										}
									}
									else{
										if(teachersMaxGapsPerDayPercentage[tch]>=0){
											addh1=max(0, remGDay1-teachersMaxGapsPerDayMaxGaps[tch]);
											addh2=max(0, remGDay2-teachersMaxGapsPerDayMaxGaps[tch]);
										}
										else{
											addh1=0;
											addh2=0;
										}
									}
									remGDay1-=addh1;
									remGDay2-=addh2;
									assert(remGDay1>=0);
									assert(remGDay2>=0);
									h1+=addh1;
									h2+=addh2;
									/*if(teachersMinHoursDailyMinHours[tch][1]>=0){
										if(h1>0){
											if(h1<teachersMinHoursDailyMinHours[tch][0]){
												remGDay1-=teachersMinHoursDailyMinHours[tch][0]-h1;
												totalH1+=teachersMinHoursDailyMinHours[tch][0];
											}
											else
												totalH1+=h1;
										}
										if(remGDay1>0)
											remG1+=remGDay1;

										if(h2>0){
											if(h2<teachersMinHoursDailyMinHours[tch][1]){
												remGDay2-=teachersMinHoursDailyMinHours[tch][1]-h2;
												totalH2+=teachersMinHoursDailyMinHours[tch][1];
											}
											else
												totalH2+=h2;
										}
										if(remGDay2>0)
											remG2+=remGDay2;
									}*/
									if(1){
										int remGDay=remGDay1+remGDay2;
										int h=h1+h2;
										if(h>0){
											if(h<teachersMinHoursDailyRealDaysMinHours[tch]){
												remGDay-=teachersMinHoursDailyRealDaysMinHours[tch]-h;
												totalH+=teachersMinHoursDailyRealDaysMinHours[tch];
											}
											else
												totalH+=h;
										}
										if(remGDay>0)
											remG+=remGDay;
									}
								}
								if(teachersMinRealDaysPerWeekPercentages[tch]>=0){
									int _usedRealDays=0;
									for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++)
										if(tchDayNHours[2*d2]>0 || tchDayNHours[2*d2+1]>0)
											_usedRealDays++;

									assert(_usedRealDays>=0 && _usedRealDays<=gt.rules.nDaysPerWeek/2);
									assert(teachersMinRealDaysPerWeekPercentages[tch]==100.0);
									int _md=teachersMinRealDaysPerWeekMinDays[tch];
									assert(_md>=0);
									if(_md>_usedRealDays){
										if(teachersMinHoursDailyMinHours[tch][1]>=0){
											totalH+=(_md-_usedRealDays)*max(teachersMinHoursDailyMinHours[tch][1], teachersMinHoursDailyRealDaysMinHours[tch]);
										}
										else{
											totalH+=(_md-_usedRealDays)*teachersMinHoursDailyRealDaysMinHours[tch];
										}
									}
								}

								if(remG+totalH<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]
								  /*&& remG1+remG2+totalH1+totalH2<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]*/
								  && totalH<=nHoursPerTeacher[tch]
								  /*&& totalH1+totalH2<=nHoursPerTeacher[tch]*/)
									ok=true;
								else
									ok=false;
							}

							if(ok)
								break;

							int ai2=-1;

							bool k=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								bool ka=teacherRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								if(!ka){
									if(level==0){
										//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okteachersminhoursdailyrealdays=false;
									goto impossibleteachersminhoursdailyrealdays;
								}
							}

							assert(ai2>=0);

							/*Activity* act2=&gt.rules.internalActivitiesList[ai2];
							int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
							int h2=c.times[ai2]/gt.rules.nDaysPerWeek;

							for(int dur2=0; dur2<act2->duration; dur2++){
								assert(tchTimetable(d2,h2+dur2)==ai2);
								tchTimetable(d2,h2+dur2)=-1;
							}*/

							removeAi2FromTchTimetable(ai2);
							updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
		}

impossibleteachersminhoursdailyrealdays:
		if(!okteachersminhoursdailyrealdays){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		/////////end teacher(s) min hours daily real day

/////////////////////////////////////////////////////////////////////////////////////////////

		/////////begin teacher(s) min days per week
		
		//Put this routine after min hours daily and min hours daily real days
		
		//Added on 11 September 2009

		okteachersmindaysperweek=true;
		if(gt.rules.mode!=MORNINGS_AFTERNOONS){
			for(int tch : qAsConst(act->iTeachersList)){
				if(teachersMinDaysPerWeekMinDays[tch]>=0 && teachersMinHoursDailyMinHours[tch][MIN_HOURS_DAILY_INDEX_IN_ARRAY]==-1){ //no need to recheck, if min hours daily is set, because I tested above.
					assert(teachersMinDaysPerWeekPercentages[tch]==100);

					bool skip=skipRandom(teachersMinDaysPerWeekPercentages[tch]);
					if(!skip){
						//preliminary test
						bool _ok;
						if(teachersMaxGapsPerWeekPercentage[tch]==-1){
							int _reqHours=0;
							int _usedDays=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
								if(newTeachersDayNHours(tch,d2)>0){
									_usedDays++;
									if(teachersMaxGapsPerDayPercentage[tch]==-1){
										_reqHours+=newTeachersDayNHours(tch,d2);
									}
									else{
										int nh=max(0, newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch]);
										_reqHours+=newTeachersDayNHours(tch,d2)+nh;
									}
								}

							assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
							assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
							int _md=teachersMinDaysPerWeekMinDays[tch];
							assert(_md>=0);
							if(_md>_usedDays)
								_reqHours+=(_md-_usedDays)*1; //one hour per day minimum

							if(_reqHours <= nHoursPerTeacher[tch])
								_ok=true; //ok
							else
								_ok=false;
						}
						else{
							int remG=0;
							int totalH=0;
							int _usedDays=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								int remGDay=newTeachersDayNGaps(tch,d2);
								int h=newTeachersDayNHours(tch,d2);
								if(h>0){
									_usedDays++;
								}
								int addh;
								if(teachersMaxGapsPerDayPercentage[tch]>=0)
									addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
								else
									addh=0;
								remGDay-=addh;
								assert(remGDay>=0);
								h+=addh;
								if(h>0)
									totalH+=h;
								if(remGDay>0)
									remG+=remGDay;
							}

							assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
							assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
							int _md=teachersMinDaysPerWeekMinDays[tch];
							assert(_md>=0);
							if(_md>_usedDays)
								totalH+=(_md-_usedDays)*1; //min 1 hour per day

							if(remG+totalH<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]
							&& totalH<=nHoursPerTeacher[tch])
								_ok=true;
							else
								_ok=false;
						}

						if(_ok)
							continue;

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersmindaysperweek=false;
							goto impossibleteachersmindaysperweek;
						}

						getTchTimetable(tch, conflActivities[newtime]);
						tchGetNHoursGaps(tch);

						for(;;){
							bool ok;
							if(teachersMaxGapsPerWeekPercentage[tch]==-1){
								int _reqHours=0;
								int _usedDays=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
									if(tchDayNHours[d2]>0){
										_usedDays++;
										if(teachersMaxGapsPerDayPercentage[tch]==-1){
											_reqHours+=tchDayNHours[d2];
										}
										else{
											int nh=max(0, tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch]);
											_reqHours+=tchDayNHours[d2]+nh;
										}
									}

								assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
								assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
								int _md=teachersMinDaysPerWeekMinDays[tch];
								assert(_md>=0);
								if(_md>_usedDays)
									_reqHours+=(_md-_usedDays)*1; //min 1 hour for each day

								if(_reqHours <= nHoursPerTeacher[tch])
									ok=true; //ok
								else
									ok=false;
							}
							else{
								int remG=0;
								int totalH=0;
								int _usedDays=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									int remGDay=tchDayNGaps[d2];
									int h=tchDayNHours[d2];
									if(h>0)
										_usedDays++;
									int addh;
									if(teachersMaxGapsPerDayPercentage[tch]>=0)
										addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
									else
										addh=0;
									remGDay-=addh;
									assert(remGDay>=0);
									h+=addh;
									if(h>0)
										totalH+=h;
									if(remGDay>0)
										remG+=remGDay;
								}
								assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
								assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
								int _md=teachersMinDaysPerWeekMinDays[tch];
								assert(_md>=0);
								if(_md>_usedDays)
									totalH+=(_md-_usedDays)*1; //min 1 hour each day

								if(remG+totalH<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]
								&& totalH<=nHoursPerTeacher[tch])
									ok=true;
								else
									ok=false;
							}

							if(ok)
								break;

							int ai2=-1;

							bool k=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								bool ka=teacherRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								if(!ka){
									if(level==0){
										//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okteachersmindaysperweek=false;
									goto impossibleteachersmindaysperweek;
								}
							}

							assert(ai2>=0);

							/*Activity* act2=&gt.rules.internalActivitiesList[ai2];
							int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
							int h2=c.times[ai2]/gt.rules.nDaysPerWeek;

							for(int dur2=0; dur2<act2->duration; dur2++){
								assert(tchTimetable(d2,h2+dur2)==ai2);
								tchTimetable(d2,h2+dur2)=-1;
							}*/

							removeAi2FromTchTimetable(ai2);
							updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
		}
		else{
			for(int tch : qAsConst(act->iTeachersList)){
				if(teachersMinDaysPerWeekMinDays[tch]>=0 && teachersMinHoursDailyMinHours[tch][1]==-1 /* && teachersMinHoursDailyRealDaysMinHours[tch]==-1 */){
				//no need to recheck, if min hours daily (BUT NOT: for real days) is set, because I tested above.
					assert(teachersMinDaysPerWeekPercentages[tch]==100);

					bool skip=skipRandom(teachersMinDaysPerWeekPercentages[tch]);
					if(!skip){
						//preliminary test
						bool _ok;
						if(teachersMaxGapsPerWeekPercentage[tch]==-1){
							int _reqHours=0;
							int _usedDays=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
								if(newTeachersDayNHours(tch,d2)>0){
									//_usedDays++;
									//2019-09-13 - max gaps per afternoon = 0
									if(teacherNoGapsPerAfternoon(tch)){
										if(teachersMaxGapsPerDayPercentage[tch]==-1){
											if(d2%2==0){
												_reqHours+=newTeachersDayNHours(tch,d2);
											}
											else{
												_reqHours+=newTeachersDayNHours(tch,d2)+newTeachersDayNGaps(tch,d2);
											}
										}
										else{
											if(d2%2==0){
												int nh=max(0, newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch]);
												_reqHours+=newTeachersDayNHours(tch,d2)+nh;
											}
											else{
												int nh=max(0, newTeachersDayNGaps(tch,d2)-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
												_reqHours+=newTeachersDayNHours(tch,d2)+nh;
											}
										}
									}
									else{
										if(teachersMaxGapsPerDayPercentage[tch]==-1){
											_reqHours+=newTeachersDayNHours(tch,d2);
										}
										else{
											int nh=max(0, newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch]);
											_reqHours+=newTeachersDayNHours(tch,d2)+nh;
										}
									}
								}
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
								if(newTeachersDayNHours(tch,d2)>0)
									_usedDays++;

							assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
							assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
							int _md=teachersMinDaysPerWeekMinDays[tch];
							assert(_md>=0);
							if(_md>_usedDays)
								_reqHours+=(_md-_usedDays)*1; //one hour per day minimum

							if(_reqHours <= nHoursPerTeacher[tch])
								_ok=true; //ok
							else
								_ok=false;
						}
						else{
							int remG=0;
							int totalH=0;
							int _usedDays=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								int remGDay=newTeachersDayNGaps(tch,d2);
								int h=newTeachersDayNHours(tch,d2);
								//if(h>0){
								//	_usedDays++;
								//}
								int addh;
								//2019-09-13 - max gaps per afternoon = 0
								if(teacherNoGapsPerAfternoon(tch)){
									if(teachersMaxGapsPerDayPercentage[tch]>=0){
										if(d2%2==0){
											addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
										}
										else{
											addh=max(0, remGDay-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
										}
									}
									else{
										if(d2%2==0){
											addh=0;
										}
										else{
											addh=max(0, remGDay-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
										}
									}
								}
								else{
									if(teachersMaxGapsPerDayPercentage[tch]>=0)
										addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
									else
										addh=0;
								}
								remGDay-=addh;
								assert(remGDay>=0);
								h+=addh;
								if(h>0)
									totalH+=h;
								if(remGDay>0)
									remG+=remGDay;
							}
							
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
								if(newTeachersDayNHours(tch,d2)>0)
									_usedDays++;
							assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
							
							assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
							int _md=teachersMinDaysPerWeekMinDays[tch];
							assert(_md>=0);
							if(_md>_usedDays)
								totalH+=(_md-_usedDays)*1; //min 1 hour per day

							if(remG+totalH<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]
							  && totalH<=nHoursPerTeacher[tch])
								_ok=true;
							else
								_ok=false;
						}

						if(_ok)
							continue;

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersmindaysperweek=false;
							goto impossibleteachersmindaysperweek;
						}

						getTchTimetable(tch, conflActivities[newtime]);
						tchGetNHoursGaps(tch);

						for(;;){
							bool ok;
							if(teachersMaxGapsPerWeekPercentage[tch]==-1){
								int _reqHours=0;
								int _usedDays=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
									if(tchDayNHours[d2]>0){
										//_usedDays++;
										//2019-09-13 - max gaps per afternoon = 0

										if(teacherNoGapsPerAfternoon(tch)){
											if(teachersMaxGapsPerDayPercentage[tch]==-1){
												if(d2%2==0){
													_reqHours+=tchDayNHours[d2];
												}
												else{
													int nh=max(0, tchDayNGaps[d2]-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
													_reqHours+=tchDayNHours[d2]+nh;
												}
											}
											else{
												if(d2%2==0){
													int nh=max(0, tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch]);
													_reqHours+=tchDayNHours[d2]+nh;
												}
												else{
													int nh=max(0, tchDayNGaps[d2]-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
													_reqHours+=tchDayNHours[d2]+nh;
												}
											}
										}
										else{
											if(teachersMaxGapsPerDayPercentage[tch]==-1){
												_reqHours+=tchDayNHours[d2];
											}
											else{
												int nh=max(0, tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch]);
												_reqHours+=tchDayNHours[d2]+nh;
											}
										}

									}
								
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
									if(tchDayNHours[d2]>0)
										_usedDays++;

								assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
								assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
								int _md=teachersMinDaysPerWeekMinDays[tch];
								assert(_md>=0);
								if(_md>_usedDays)
									_reqHours+=(_md-_usedDays)*1; //min 1 hour for each day

								if(_reqHours <= nHoursPerTeacher[tch])
									ok=true; //ok
								else
									ok=false;
							}
							else{
								int remG=0;
								int totalH=0;
								int _usedDays=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									int remGDay=tchDayNGaps[d2];
									int h=tchDayNHours[d2];
									//if(h>0)
									//	_usedDays++;
									int addh;
									//2019-09-13 - max gaps per afternoon = 0
									if(teacherNoGapsPerAfternoon(tch)){
										if(teachersMaxGapsPerDayPercentage[tch]>=0){
											if(d2%2==0){
												addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
											}
											else{
												addh=max(0, remGDay-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
											}
										}
										else{
											if(d2%2==0){
												addh=0;
											}
											else{
												addh=max(0, remGDay-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
											}
										}
									}
									else{
										if(teachersMaxGapsPerDayPercentage[tch]>=0)
											addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
										else
											addh=0;
									}
									remGDay-=addh;
									assert(remGDay>=0);
									h+=addh;
									if(h>0)
										totalH+=h;
									if(remGDay>0)
										remG+=remGDay;
								}
								
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
									if(tchDayNHours[d2]>0)
										_usedDays++;
								assert(_usedDays>=0 && _usedDays<=gt.rules.nDaysPerWeek);
								assert(teachersMinDaysPerWeekPercentages[tch]==100.0);
								int _md=teachersMinDaysPerWeekMinDays[tch];
								assert(_md>=0);
								if(_md>_usedDays)
									totalH+=(_md-_usedDays)*1; //min 1 hour each day

								if(remG+totalH<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]
								  && totalH<=nHoursPerTeacher[tch])
									ok=true;
								else
									ok=false;
							}

							if(ok)
								break;

							int ai2=-1;

							bool k=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								bool ka=teacherRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								if(!ka){
									if(level==0){
										//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okteachersmindaysperweek=false;
									goto impossibleteachersmindaysperweek;
								}
							}

							assert(ai2>=0);

							/*Activity* act2=&gt.rules.internalActivitiesList[ai2];
							int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
							int h2=c.times[ai2]/gt.rules.nDaysPerWeek;

							for(int dur2=0; dur2<act2->duration; dur2++){
								assert(tchTimetable(d2,h2+dur2)==ai2);
								tchTimetable(d2,h2+dur2)=-1;
							}*/

							removeAi2FromTchTimetable(ai2);
							updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
		}

impossibleteachersmindaysperweek:
		if(!okteachersmindaysperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		
		/////////end teacher(s) min days per week

/////////////////////////////////////////////////////////////////////////////////////////////

		/////////begin teacher(s) min real days per week

		//Maybe old comments below:
		//Put this routine after min hours daily and min hours daily real days

		//Added on 11 September 2009

		okteachersminrealdaysperweek=true;
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int tch : qAsConst(act->iTeachersList)){
				if(teachersMinRealDaysPerWeekMinDays[tch]>=0 && teachersMinHoursDailyMinHours[tch][1]==-1 && teachersMinHoursDailyRealDaysMinHours[tch]==-1){
				//no need to recheck, if min hours daily (or for real days) is set, because I tested above.
					assert(teachersMinRealDaysPerWeekPercentages[tch]==100);

					bool skip=skipRandom(teachersMinRealDaysPerWeekPercentages[tch]);
					if(!skip){
						//preliminary test
						bool _ok;
						if(teachersMaxGapsPerWeekPercentage[tch]==-1){
							int _reqHours=0;
							int _usedRealDays=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
								if(newTeachersDayNHours(tch,d2)>0){
									//_usedDays++;
									//2019-09-13 - max gaps per afternoon = 0
									if(teacherNoGapsPerAfternoon(tch)){
										if(teachersMaxGapsPerDayPercentage[tch]==-1){
											if(d2%2==0){
												_reqHours+=newTeachersDayNHours(tch,d2);
											}
											else{
												_reqHours+=newTeachersDayNHours(tch,d2)+newTeachersDayNGaps(tch,d2);
											}
										}
										else{
											if(d2%2==0){
												int nh=max(0, newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch]);
												_reqHours+=newTeachersDayNHours(tch,d2)+nh;
											}
											else{
												int nh=max(0, newTeachersDayNGaps(tch,d2)-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
												_reqHours+=newTeachersDayNHours(tch,d2)+nh;
											}
										}
									}
									else{
										if(teachersMaxGapsPerDayPercentage[tch]==-1){
											_reqHours+=newTeachersDayNHours(tch,d2);
										}
										else{
											int nh=max(0, newTeachersDayNGaps(tch,d2)-teachersMaxGapsPerDayMaxGaps[tch]);
											_reqHours+=newTeachersDayNHours(tch,d2)+nh;
										}
									}
								}
							for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++)
								if(newTeachersDayNHours(tch,2*d2)>0 || newTeachersDayNHours(tch,2*d2+1)>0)
									_usedRealDays++;

							assert(_usedRealDays>=0 && _usedRealDays<=gt.rules.nDaysPerWeek/2);
							assert(teachersMinRealDaysPerWeekPercentages[tch]==100.0);
							int _md=teachersMinRealDaysPerWeekMinDays[tch];
							assert(_md>=0);
							if(_md>_usedRealDays)
								_reqHours+=(_md-_usedRealDays)*1; //one hour per day minimum

							if(_reqHours <= nHoursPerTeacher[tch])
								_ok=true; //ok
							else
								_ok=false;
						}
						else{
							int remG=0;
							int totalH=0;
							int _usedRealDays=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								int remGDay=newTeachersDayNGaps(tch,d2);
								int h=newTeachersDayNHours(tch,d2);
								//if(h>0){
								//	_usedDays++;
								//}
								int addh;
								//2019-09-13 - max gaps per afternoon = 0
								if(teacherNoGapsPerAfternoon(tch)){
									if(teachersMaxGapsPerDayPercentage[tch]>=0){
										if(d2%2==0){
											addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
										}
										else{
											addh=max(0, remGDay-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
										}
									}
									else{
										if(d2%2==0){
											addh=0;
										}
										else{
											addh=max(0, remGDay-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
										}
									}
								}
								else{
									if(teachersMaxGapsPerDayPercentage[tch]>=0)
										addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
									else
										addh=0;
								}
								remGDay-=addh;
								assert(remGDay>=0);
								h+=addh;
								if(h>0)
									totalH+=h;
								if(remGDay>0)
									remG+=remGDay;
							}
							for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++)
								if(newTeachersDayNHours(tch,2*d2)>0 || newTeachersDayNHours(tch,2*d2+1)>0)
									_usedRealDays++;

							assert(_usedRealDays>=0 && _usedRealDays<=gt.rules.nDaysPerWeek/2);
							assert(teachersMinRealDaysPerWeekPercentages[tch]==100.0);
							int _md=teachersMinRealDaysPerWeekMinDays[tch];
							assert(_md>=0);
							if(_md>_usedRealDays)
								totalH+=(_md-_usedRealDays)*1; //min 1 hour per day

							if(remG+totalH<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]
							  && totalH<=nHoursPerTeacher[tch])
								_ok=true;
							else
								_ok=false;
						}

						if(_ok)
							continue;

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersminrealdaysperweek=false;
							goto impossibleteachersminrealdaysperweek;
						}

						getTchTimetable(tch, conflActivities[newtime]);
						tchGetNHoursGaps(tch);

						for(;;){
							bool ok;
							if(teachersMaxGapsPerWeekPercentage[tch]==-1){
								int _reqHours=0;
								int _usedRealDays=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
									if(tchDayNHours[d2]>0){
										//_usedDays++;
										//2019-09-13 - max gaps per afternoon = 0

										if(teacherNoGapsPerAfternoon(tch)){
											if(teachersMaxGapsPerDayPercentage[tch]==-1){
												if(d2%2==0){
													_reqHours+=tchDayNHours[d2];
												}
												else{
													int nh=max(0, tchDayNGaps[d2]-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
													_reqHours+=tchDayNHours[d2]+nh;
												}
											}
											else{
												if(d2%2==0){
													int nh=max(0, tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch]);
													_reqHours+=tchDayNHours[d2]+nh;
												}
												else{
													int nh=max(0, tchDayNGaps[d2]-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
													_reqHours+=tchDayNHours[d2]+nh;
												}
											}
										}
										else{
											if(teachersMaxGapsPerDayPercentage[tch]==-1){
												_reqHours+=tchDayNHours[d2];
											}
											else{
												int nh=max(0, tchDayNGaps[d2]-teachersMaxGapsPerDayMaxGaps[tch]);
												_reqHours+=tchDayNHours[d2]+nh;
											}
										}

									}
								for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++)
									if(tchDayNHours[2*d2]>0 || tchDayNHours[2*d2+1]>0)
										_usedRealDays++;

								assert(_usedRealDays>=0 && _usedRealDays<=gt.rules.nDaysPerWeek/2);
								assert(teachersMinRealDaysPerWeekPercentages[tch]==100.0);
								int _md=teachersMinRealDaysPerWeekMinDays[tch];
								assert(_md>=0);
								if(_md>_usedRealDays)
									_reqHours+=(_md-_usedRealDays)*1; //min 1 hour for each day

								if(_reqHours <= nHoursPerTeacher[tch])
									ok=true; //ok
								else
									ok=false;
							}
							else{
								int remG=0;
								int totalH=0;
								int _usedRealDays=0;
								for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
									int remGDay=tchDayNGaps[d2];
									int h=tchDayNHours[d2];
									//if(h>0)
									//	_usedDays++;
									int addh;
									//2019-09-13 - max gaps per afternoon = 0
									if(teacherNoGapsPerAfternoon(tch)){
										if(teachersMaxGapsPerDayPercentage[tch]>=0){
											if(d2%2==0){
												addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
											}
											else{
												addh=max(0, remGDay-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
											}
										}
										else{
											if(d2%2==0){
												addh=0;
											}
											else{
												addh=max(0, remGDay-0/*teachersMaxGapsPerDayMaxGaps[tch]*/);
											}
										}
									}
									else{
										if(teachersMaxGapsPerDayPercentage[tch]>=0)
											addh=max(0, remGDay-teachersMaxGapsPerDayMaxGaps[tch]);
										else
											addh=0;
									}
									remGDay-=addh;
									assert(remGDay>=0);
									h+=addh;
									if(h>0)
										totalH+=h;
									if(remGDay>0)
										remG+=remGDay;
								}
								for(int d2=0; d2<gt.rules.nDaysPerWeek/2; d2++)
									if(tchDayNHours[2*d2]>0 || tchDayNHours[2*d2+1]>0)
										_usedRealDays++;
								assert(_usedRealDays>=0 && _usedRealDays<=gt.rules.nDaysPerWeek/2);
								assert(teachersMinRealDaysPerWeekPercentages[tch]==100.0);
								int _md=teachersMinRealDaysPerWeekMinDays[tch];
								assert(_md>=0);
								if(_md>_usedRealDays)
									totalH+=(_md-_usedRealDays)*1; //min 1 hour each day

								if(remG+totalH<=nHoursPerTeacher[tch]+teachersMaxGapsPerWeekMaxGaps[tch]
								  && totalH<=nHoursPerTeacher[tch])
									ok=true;
								else
									ok=false;
							}

							if(ok)
								break;

							int ai2=-1;

							bool k=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								bool ka=teacherRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
								assert(conflActivities[newtime].count()==nConflActivities[newtime]);

								if(!ka){
									if(level==0){
										//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okteachersminrealdaysperweek=false;
									goto impossibleteachersminrealdaysperweek;
								}
							}

							assert(ai2>=0);

							/*Activity* act2=&gt.rules.internalActivitiesList[ai2];
							int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
							int h2=c.times[ai2]/gt.rules.nDaysPerWeek;

							for(int dur2=0; dur2<act2->duration; dur2++){
								assert(tchTimetable(d2,h2+dur2)==ai2);
								tchTimetable(d2,h2+dur2)=-1;
							}*/

							removeAi2FromTchTimetable(ai2);
							updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
		}

impossibleteachersminrealdaysperweek:
		if(!okteachersminrealdaysperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		/////////end teacher(s) min real days per week

/////////////////////////////////////////////////////////////////////////////////////////////

		okteachersminmorningsafternoonsperweek=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int tch : qAsConst(act->iTeachersList)){
				if(teachersMinMorningsPerWeekMinMornings[tch]>=0 || teachersMinAfternoonsPerWeekMinAfternoons[tch]>=0){
					//assert(teachersMinHoursDailyPercentages[tch]==100);
					int mhd[2];
					mhd[1]=1; //afternoon
					mhd[0]=1; //morning, at least as large as for daily
					if(teachersMinHoursDailyPercentages[tch][1]==100){
						assert(mhd[1]<teachersMinHoursDailyMinHours[tch][1]);
						mhd[1]=teachersMinHoursDailyMinHours[tch][1];
					}
					if(teachersMinHoursDailyPercentages[tch][0]==100){
						assert(mhd[0]<teachersMinHoursDailyMinHours[tch][0]);
						mhd[0]=teachersMinHoursDailyMinHours[tch][0];
					}

					if(teachersMinHoursPerAfternoonPercentages[tch]==100){
						assert(mhd[1]<=teachersMinHoursPerAfternoonMinHours[tch]);
						mhd[1]=teachersMinHoursPerAfternoonMinHours[tch];
					}
					
					//assert(teachersMaxGapsPerWeekMaxGaps[tch]==0 || teachersMaxGapsPerDayMaxGaps[tch]==0);

					bool skip=false;
					if(!skip){
						//preliminary test
						bool _ok;

						int requested=0;
						int filledMornings=0;
						int filledAfternoons=0;
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							int p=/*newTeachersDayNGaps(tch, d2)+*/newTeachersDayNHours(tch, d2);
							if(teachersMaxGapsPerWeekMaxGaps[tch]==0 || teachersMaxGapsPerDayMaxGaps[tch]==0
							 || (teachersMaxGapsPerRealDayMaxGaps[tch]==0 && teachersMaxGapsPerRealDayAllowException[tch]==false) ||
							 teachersMaxGapsPerWeekForRealDaysMaxGaps[tch]==0)
								p+=newTeachersDayNGaps(tch, d2);
							if(p>0 && p<mhd[d2%2])
								p=mhd[d2%2];
							requested+=p;
							if(p>0){
								if(d2%2==0)
									filledMornings++;
								else
									filledAfternoons++;
							}
						}
						if(teachersMinMorningsPerWeekMinMornings[tch]>0){
							if(filledMornings<teachersMinMorningsPerWeekMinMornings[tch])
								requested+=mhd[0]*(-filledMornings+teachersMinMorningsPerWeekMinMornings[tch]);
						}
						if(teachersMinAfternoonsPerWeekMinAfternoons[tch]>0){
							if(filledAfternoons<teachersMinAfternoonsPerWeekMinAfternoons[tch])
								requested+=mhd[1]*(-filledAfternoons+teachersMinAfternoonsPerWeekMinAfternoons[tch]);
						}

						_ok=(requested<=nHoursPerTeacher[tch]);

						if(_ok)
							continue;

						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersminmorningsafternoonsperweek=false;
							goto impossibleteachersminmorningsafternoonsperweek;
						}

						getTchTimetable(tch, conflActivities[newtime]);
						tchGetNHoursGaps(tch);

						for(;;){
							bool ok;

							int requested=0;
							int filledMornings=0;
							int filledAfternoons=0;
							for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
								int p=/*tchDayNGaps[d2]+*/tchDayNHours[d2];
								if(teachersMaxGapsPerWeekMaxGaps[tch]==0 || teachersMaxGapsPerDayMaxGaps[tch]==0
								 || (teachersMaxGapsPerRealDayMaxGaps[tch]==0 && teachersMaxGapsPerRealDayAllowException[tch]==false)
								 || teachersMaxGapsPerWeekForRealDaysMaxGaps[tch]==0)
									p+=tchDayNGaps[d2];
								if(p>0 && p<mhd[d2%2])
									p=mhd[d2%2];
								requested+=p;
								if(p>0){
									if(d2%2==0)
										filledMornings++;
									else
										filledAfternoons++;
								}
							}
							if(teachersMinMorningsPerWeekMinMornings[tch]>0){
								if(filledMornings<teachersMinMorningsPerWeekMinMornings[tch])
									requested+=mhd[0]*(-filledMornings+teachersMinMorningsPerWeekMinMornings[tch]);
							}
							if(teachersMinAfternoonsPerWeekMinAfternoons[tch]>0){
								if(filledAfternoons<teachersMinAfternoonsPerWeekMinAfternoons[tch])
									requested+=mhd[1]*(-filledAfternoons+teachersMinAfternoonsPerWeekMinAfternoons[tch]);
							}

							ok=(requested<=nHoursPerTeacher[tch]);

							if(ok)
								break;

							int ai2=-1;

							bool k=teacherRemoveAnActivityFromBeginOrEnd(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
							assert(conflActivities[newtime].count()==nConflActivities[newtime]);
							if(!k){
								bool k2=false;
								if(!(teachersMaxGapsPerWeekMaxGaps[tch]==0 || teachersMaxGapsPerDayMaxGaps[tch]==0
								 || (teachersMaxGapsPerRealDayMaxGaps[tch]==0 && teachersMaxGapsPerRealDayAllowException[tch]==false))
								 || teachersMaxGapsPerWeekForRealDaysMaxGaps[tch]==0){
									k2=teacherRemoveAnActivityFromAnywhere(level, ai, conflActivities[newtime], nConflActivities[newtime], ai2);
									assert(conflActivities[newtime].count()==nConflActivities[newtime]);
								}
								if(!k2){
									if(level==0){
										//Liviu: inactivated from version 5.12.4 (7 Feb. 2010), because it may take too long for some files
										//cout<<"WARNING - mb - file "<<__FILE__<<" line "<<__LINE__<<endl;
									}
									okteachersminmorningsafternoonsperweek=false;
									goto impossibleteachersminmorningsafternoonsperweek;
								}
							}

							assert(ai2>=0);

							/*Activity* act2=&gt.rules.internalActivitiesList[ai2];
							int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
							int h2=c.times[ai2]/gt.rules.nDaysPerWeek;

							for(int dur2=0; dur2<act2->duration; dur2++){
								assert(tchTimetable(d2,h2+dur2)==ai2);
								tchTimetable(d2,h2+dur2)=-1;
							}*/

							removeAi2FromTchTimetable(ai2);
							updateTchNHoursGaps(tch, c.times[ai2]%gt.rules.nDaysPerWeek);
						}
					}
				}
			}
		}

impossibleteachersminmorningsafternoonsperweek:
		if(!okteachersminmorningsafternoonsperweek){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		/////////end teacher(s) min mornings or afternoons per week

/////////////////////////////////////////////////////////////////////////////////////////////

		/////////begin teacher(s) max two activity tags per day from N1 N2 N3

		okteachersmaxtwoactivitytagsperdayfromn1n2n3=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int tch : qAsConst(act->iTeachersList)){
				if(teachersMaxTwoActivityTagsPerDayFromN1N2N3Percentages[tch]>=0){
					assert(teachersMaxTwoActivityTagsPerDayFromN1N2N3Percentages[tch]==100);
					int cnt[4]; //cnt[3] is for activities which have no tag from N1, N2, or N3. Crash bug fixed on 2021-07-25.
					cnt[0]=cnt[1]=cnt[2]=cnt[3]=0;
					for(int ai2 : qAsConst(teacherActivitiesOfTheDay[tch][d]))
						if(!conflActivities[newtime].contains(ai2)){
							int actTag=activityTagN1N2N3[ai2];
							cnt[actTag]++;
						}

					int cntTags=0;
					for(int i=0; i<3; i++)
						if(cnt[i]>0)
							cntTags++;
					assert(cntTags<=2);
					for(int i=0; i<3; i++){
						int c0, c1, c2;
						if(i==0){
							c0=0; c1=1; c2=2;
						}
						else if(i==1){
							c0=0; c1=2; c2=1;
						}
						else if(i==2){
							c0=1; c1=2; c2=0;
						}
						else{
							assert(0);
						}

						if(cnt[c0]>0 && cnt[c1]>0 && activityTagN1N2N3[ai]==c2){
							bool canEmptyc0=true;
							bool canEmptyc1=true;
							QList<int> activitiesWithc0;
							QList<int> activitiesWithc1;
							
							for(int ai2 : qAsConst(teacherActivitiesOfTheDay[tch][d])){
								if(activityTagN1N2N3[ai2]==c0 && !conflActivities[newtime].contains(ai2)){
									if(!fixedTimeActivity[ai2] && !swappedActivities[ai2]){
										activitiesWithc0.append(ai2);
									}
									else{
										canEmptyc0=false;
										break;
									}
								}
							}

							for(int ai2 : qAsConst(teacherActivitiesOfTheDay[tch][d])){
								if(activityTagN1N2N3[ai2]==c1 && !conflActivities[newtime].contains(ai2)){
									if(!fixedTimeActivity[ai2] && !swappedActivities[ai2]){
										activitiesWithc1.append(ai2);
									}
									else{
										canEmptyc1=false;
										break;
									}
								}
							}
							
							if(!canEmptyc0 && !canEmptyc1){
								okteachersmaxtwoactivitytagsperdayfromn1n2n3=false;
								goto impossibleteachersmaxtwoactivitytagsperdayfromn1n2n3;
							}
							else if(canEmptyc0 && !canEmptyc1){
								for(int ai2 : qAsConst(activitiesWithc0)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
								}
							}
							else if(!canEmptyc0 && canEmptyc1){
								for(int ai2 : qAsConst(activitiesWithc1)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
								}
							}
							else{
								assert(canEmptyc0 && canEmptyc1);
								if(level>0){
									if(activitiesWithc0.count()<activitiesWithc1.count()){
										for(int ai2 : qAsConst(activitiesWithc0)){
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
										}
									}
									else if(activitiesWithc0.count()>activitiesWithc1.count()){
										for(int ai2 : qAsConst(activitiesWithc1)){
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
										}
									}
									else{
										int rnd=rng.intMRG32k3a(2);
										if(rnd==0){
											for(int ai2 : qAsConst(activitiesWithc0)){
												conflActivities[newtime].append(ai2);
												nConflActivities[newtime]++;
											}
										}
										else{
											assert(rnd==1);
											for(int ai2 : qAsConst(activitiesWithc1)){
												conflActivities[newtime].append(ai2);
												nConflActivities[newtime]++;
											}
										}
									}
								}
								else{
									assert(level==0);

									int _minWrong[3];
									int _nWrong[3];
									int _nConflActivities[3];
									int _minIndexAct[3];
									
									_minWrong[c0]=INF;
									_nWrong[c0]=0;
									_nConflActivities[c0]=activitiesWithc0.count();
									_minIndexAct[c0]=gt.rules.nInternalActivities;

									for(int ai2 : qAsConst(activitiesWithc0)){
										_minWrong[c0] = min (_minWrong[c0], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[c0]=min(_minIndexAct[c0], invPermutation[ai2]);
										_nWrong[c0]+=triedRemovals(ai2,c.times[ai2]);
									}

									_minWrong[c1]=INF;
									_nWrong[c1]=0;
									_nConflActivities[c1]=activitiesWithc1.count();
									_minIndexAct[c1]=gt.rules.nInternalActivities;
									
									for(int ai2 : qAsConst(activitiesWithc1)){
										_minWrong[c1] = min (_minWrong[c1], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[c1]=min(_minIndexAct[c1], invPermutation[ai2]);
										_nWrong[c1]+=triedRemovals(ai2,c.times[ai2]);
									}

									if(_minWrong[c0]==_minWrong[c1] && _nWrong[c0]==_nWrong[c1] && _nConflActivities[c0]==_nConflActivities[c1] && _minIndexAct[c0]==_minIndexAct[c1]){
										int rnd=rng.intMRG32k3a(2);
										if(rnd==0){
											for(int ai2 : qAsConst(activitiesWithc0)){
												conflActivities[newtime].append(ai2);
												nConflActivities[newtime]++;
											}
										}
										else{
											assert(rnd==1);
											for(int ai2 : qAsConst(activitiesWithc1)){
												conflActivities[newtime].append(ai2);
												nConflActivities[newtime]++;
											}
										}
									}
									else if(_minWrong[c0]>_minWrong[c1] ||
									  (_minWrong[c0]==_minWrong[c1] && _nWrong[c0]>_nWrong[c1]) ||
									  (_minWrong[c0]==_minWrong[c1] && _nWrong[c0]==_nWrong[c1] && _nConflActivities[c0]>_nConflActivities[c1]) ||
									  (_minWrong[c0]==_minWrong[c1] && _nWrong[c0]==_nWrong[c1] && _nConflActivities[c0]==_nConflActivities[c1] && _minIndexAct[c0]>_minIndexAct[c1])){
										//choose c1
										for(int ai2 : qAsConst(activitiesWithc1)){
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
										}
									}
									else{
										assert(_minWrong[c1]>_minWrong[c0] ||
										  (_minWrong[c1]==_minWrong[c0] && _nWrong[c1]>_nWrong[c0]) ||
										  (_minWrong[c1]==_minWrong[c0] && _nWrong[c1]==_nWrong[c0] && _nConflActivities[c1]>_nConflActivities[c0]) ||
										  (_minWrong[c1]==_minWrong[c0] && _nWrong[c1]==_nWrong[c0] && _nConflActivities[c1]==_nConflActivities[c0] && _minIndexAct[c1]>_minIndexAct[c0]));
										//choose c0
										for(int ai2 : qAsConst(activitiesWithc0)){
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
										}
									}
								}
							}
						}
					}
				}
			}
		}

impossibleteachersmaxtwoactivitytagsperdayfromn1n2n3:
		if(!okteachersmaxtwoactivitytagsperdayfromn1n2n3){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		/////////end teacher(s) max two activity tags per day from N1 N2 N3

/////////////////////////////////////////////////////////////////////////////////////////////

		/////////begin teacher(s) max two activity tags per real day from N1 N2 N3

		okteachersmaxtwoactivitytagsperrealdayfromn1n2n3=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int tch : qAsConst(act->iTeachersList)){
				if(teachersMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages[tch]>=0){
					assert(teachersMaxTwoActivityTagsPerRealDayFromN1N2N3Percentages[tch]==100);
					int cnt[4]; //cnt[3] is for activities which have no tag from N1, N2, or N3. Crash bug fixed on 2021-07-25.
					cnt[0]=cnt[1]=cnt[2]=cnt[3]=0;
					for(int ai2 : qAsConst(teacherActivitiesOfTheDay[tch][d]))
						if(!conflActivities[newtime].contains(ai2)){
							int actTag=activityTagN1N2N3[ai2];
							cnt[actTag]++;
						}
					for(int ai2 : qAsConst(teacherActivitiesOfTheDay[tch][dpair]))
						if(!conflActivities[newtime].contains(ai2)){
							int actTag=activityTagN1N2N3[ai2];
							cnt[actTag]++;
						}

					int cntTags=0;
					for(int i=0; i<3; i++)
						if(cnt[i]>0)
							cntTags++;
					assert(cntTags<=2);
					for(int i=0; i<3; i++){
						int c0, c1, c2;
						if(i==0){
							c0=0; c1=1; c2=2;
						}
						else if(i==1){
							c0=0; c1=2; c2=1;
						}
						else if(i==2){
							c0=1; c1=2; c2=0;
						}
						else{
							assert(0);
						}

						if(cnt[c0]>0 && cnt[c1]>0 && activityTagN1N2N3[ai]==c2){
							bool canEmptyc0=true;
							bool canEmptyc1=true;
							QList<int> activitiesWithc0;
							QList<int> activitiesWithc1;
							
							for(int ai2 : qAsConst(teacherActivitiesOfTheDay[tch][d])){
								if(activityTagN1N2N3[ai2]==c0 && !conflActivities[newtime].contains(ai2)){
									if(!fixedTimeActivity[ai2] && !swappedActivities[ai2]){
										activitiesWithc0.append(ai2);
									}
									else{
										canEmptyc0=false;
										break;
									}
								}
							}
							if(canEmptyc0){
								for(int ai2 : qAsConst(teacherActivitiesOfTheDay[tch][dpair])){
									if(activityTagN1N2N3[ai2]==c0 && !conflActivities[newtime].contains(ai2)){
										if(!fixedTimeActivity[ai2] && !swappedActivities[ai2]){
											activitiesWithc0.append(ai2);
										}
										else{
											canEmptyc0=false;
											break;
										}
									}
								}
							}

							for(int ai2 : qAsConst(teacherActivitiesOfTheDay[tch][d])){
								if(activityTagN1N2N3[ai2]==c1 && !conflActivities[newtime].contains(ai2)){
									if(!fixedTimeActivity[ai2] && !swappedActivities[ai2]){
										activitiesWithc1.append(ai2);
									}
									else{
										canEmptyc1=false;
										break;
									}
								}
							}
							if(canEmptyc1){
								for(int ai2 : qAsConst(teacherActivitiesOfTheDay[tch][dpair])){
									if(activityTagN1N2N3[ai2]==c1 && !conflActivities[newtime].contains(ai2)){
										if(!fixedTimeActivity[ai2] && !swappedActivities[ai2]){
											activitiesWithc1.append(ai2);
										}
										else{
											canEmptyc1=false;
											break;
										}
									}
								}
							}
							
							if(!canEmptyc0 && !canEmptyc1){
								okteachersmaxtwoactivitytagsperrealdayfromn1n2n3=false;
								goto impossibleteachersmaxtwoactivitytagsperrealdayfromn1n2n3;
							}
							else if(canEmptyc0 && !canEmptyc1){
								for(int ai2 : qAsConst(activitiesWithc0)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
								}
							}
							else if(!canEmptyc0 && canEmptyc1){
								for(int ai2 : qAsConst(activitiesWithc1)){
									conflActivities[newtime].append(ai2);
									nConflActivities[newtime]++;
								}
							}
							else{
								assert(canEmptyc0 && canEmptyc1);
								if(level>0){
									if(activitiesWithc0.count()<activitiesWithc1.count()){
										for(int ai2 : qAsConst(activitiesWithc0)){
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
										}
									}
									else if(activitiesWithc0.count()>activitiesWithc1.count()){
										for(int ai2 : qAsConst(activitiesWithc1)){
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
										}
									}
									else{
										int rnd=rng.intMRG32k3a(2);
										if(rnd==0){
											for(int ai2 : qAsConst(activitiesWithc0)){
												conflActivities[newtime].append(ai2);
												nConflActivities[newtime]++;
											}
										}
										else{
											assert(rnd==1);
											for(int ai2 : qAsConst(activitiesWithc1)){
												conflActivities[newtime].append(ai2);
												nConflActivities[newtime]++;
											}
										}
									}
								}
								else{
									assert(level==0);

									int _minWrong[3];
									int _nWrong[3];
									int _nConflActivities[3];
									int _minIndexAct[3];
									
									_minWrong[c0]=INF;
									_nWrong[c0]=0;
									_nConflActivities[c0]=activitiesWithc0.count();
									_minIndexAct[c0]=gt.rules.nInternalActivities;

									for(int ai2 : qAsConst(activitiesWithc0)){
										_minWrong[c0] = min (_minWrong[c0], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[c0]=min(_minIndexAct[c0], invPermutation[ai2]);
										_nWrong[c0]+=triedRemovals(ai2,c.times[ai2]);
									}

									_minWrong[c1]=INF;
									_nWrong[c1]=0;
									_nConflActivities[c1]=activitiesWithc1.count();
									_minIndexAct[c1]=gt.rules.nInternalActivities;
									
									for(int ai2 : qAsConst(activitiesWithc1)){
										_minWrong[c1] = min (_minWrong[c1], triedRemovals(ai2,c.times[ai2]));
										_minIndexAct[c1]=min(_minIndexAct[c1], invPermutation[ai2]);
										_nWrong[c1]+=triedRemovals(ai2,c.times[ai2]);
									}

									if(_minWrong[c0]==_minWrong[c1] && _nWrong[c0]==_nWrong[c1] && _nConflActivities[c0]==_nConflActivities[c1] && _minIndexAct[c0]==_minIndexAct[c1]){
										int rnd=rng.intMRG32k3a(2);
										if(rnd==0){
											for(int ai2 : qAsConst(activitiesWithc0)){
												conflActivities[newtime].append(ai2);
												nConflActivities[newtime]++;
											}
										}
										else{
											assert(rnd==1);
											for(int ai2 : qAsConst(activitiesWithc1)){
												conflActivities[newtime].append(ai2);
												nConflActivities[newtime]++;
											}
										}
									}
									else if(_minWrong[c0]>_minWrong[c1] ||
									  (_minWrong[c0]==_minWrong[c1] && _nWrong[c0]>_nWrong[c1]) ||
									  (_minWrong[c0]==_minWrong[c1] && _nWrong[c0]==_nWrong[c1] && _nConflActivities[c0]>_nConflActivities[c1]) ||
									  (_minWrong[c0]==_minWrong[c1] && _nWrong[c0]==_nWrong[c1] && _nConflActivities[c0]==_nConflActivities[c1] && _minIndexAct[c0]>_minIndexAct[c1])){
										//choose c1
										for(int ai2 : qAsConst(activitiesWithc1)){
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
										}
									}
									else{
										assert(_minWrong[c1]>_minWrong[c0] ||
										  (_minWrong[c1]==_minWrong[c0] && _nWrong[c1]>_nWrong[c0]) ||
										  (_minWrong[c1]==_minWrong[c0] && _nWrong[c1]==_nWrong[c0] && _nConflActivities[c1]>_nConflActivities[c0]) ||
										  (_minWrong[c1]==_minWrong[c0] && _nWrong[c1]==_nWrong[c0] && _nConflActivities[c1]==_nConflActivities[c0] && _minIndexAct[c1]>_minIndexAct[c0]));
										//choose c0
										for(int ai2 : qAsConst(activitiesWithc0)){
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
										}
									}
								}
							}
						}
					}
				}
			}
		}

impossibleteachersmaxtwoactivitytagsperrealdayfromn1n2n3:
		if(!okteachersmaxtwoactivitytagsperrealdayfromn1n2n3){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

		/////////end teacher(s) max two activity tags per real day from N1 N2 N3

/////////////////////////////////////////////////////////////////////////////////////////////

		//2020-06-28
		//teachers max hours per all afternoons.

		okteachersmaxhoursperallafternoons=true;

		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			for(int tch : qAsConst(act->iTeachersList)){
				if(teachersMaxHoursPerAllAfternoonsMaxHours[tch]>=0){
					assert(teachersMaxHoursPerAllAfternoonsPercentages[tch]==100.0);

					if(d%2==1){ //afternoon
						//preliminary
						int _nOcc=0;

						for(int d2=1; d2<gt.rules.nDaysPerWeek; d2+=2){ //afternoon
							for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
								int ai2=newTeachersTimetable(tch,d2,h2);
								if(ai2>=0)
									_nOcc++;
							}
						}

						if(_nOcc<=teachersMaxHoursPerAllAfternoonsMaxHours[tch])
							continue; //ok

						getTchTimetable(tch, conflActivities[newtime]);

						int nOccupied=0;

						QSet<int> candidates;

						//static int slotActivity[MAX_DAYS_PER_WEEK*MAX_HOURS_PER_DAY];

						for(int d2=1; d2<gt.rules.nDaysPerWeek; d2+=2){ //afternoon
							for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
								int t=d2+gt.rules.nDaysPerWeek*h2;

								int ai2=tchTimetable(d2,h2);
								slotActivity[t]=ai2;
								if(ai2>=0){
									nOccupied++;
									if(ai2!=ai && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
										candidates.insert(t);
								}
							}
						}

						if(nOccupied > teachersMaxHoursPerAllAfternoonsMaxHours[tch]){
							int target=nOccupied - teachersMaxHoursPerAllAfternoonsMaxHours[tch];

							while(target>0){
								bool decreased=false;

								if(candidates.count()==0){
									okteachersmaxhoursperallafternoons=false;
									goto impossibleteachersmaxhoursperallafternoons;
								}

								//To keep the generation identical on all computers
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
								QList<int> tmpSortedList=QList<int>(candidates.constBegin(), candidates.constEnd());
#else
								QList<int> tmpSortedList=candidates.toList();
#endif
								std::stable_sort(tmpSortedList.begin(), tmpSortedList.end());

								int t=-1;
								if(level>0){
									assert(candidates.count()==tmpSortedList.count());
									int q=rng.intMRG32k3a(candidates.count());
									t=tmpSortedList.at(q);
								}
								else{
									assert(level==0);

									int optMinWrong=INF;
									QList<int> tl;

									for(int t2 : qAsConst(tmpSortedList)){
										int ai3=slotActivity[t2];
										if(optMinWrong>triedRemovals(ai3,c.times[ai3])){
											optMinWrong=triedRemovals(ai3,c.times[ai3]);
											tl.clear();
											tl.append(t2);
										}
										else if(optMinWrong==triedRemovals(ai3,c.times[ai3])){
											tl.append(t2);
										}
									}

									assert(tl.count()>0);
									int q=rng.intMRG32k3a(tl.count());
									t=tl.at(q);
								}

								assert(t>=0);
								int ai2=slotActivity[t];

								assert(ai2>=0);
								assert(ai2!=ai);
								assert(c.times[ai2]!=UNALLOCATED_TIME);
								assert(!fixedTimeActivity[ai2] && !swappedActivities[ai2]);

								for(int tt=c.times[ai2]; tt<c.times[ai2]+gt.rules.internalActivitiesList[ai2].duration*gt.rules.nDaysPerWeek; tt+=gt.rules.nDaysPerWeek){
									assert(slotActivity[tt]==ai2);
									slotActivity[tt]=-1;
									assert(candidates.contains(tt));
									candidates.remove(tt);
									target--;

									decreased=true;
								}

								assert(!conflActivities[newtime].contains(ai2));
								conflActivities[newtime].append(ai2);
								nConflActivities[newtime]++;
								assert(nConflActivities[newtime]==conflActivities[newtime].count());

								removeAi2FromTchTimetable(ai2); //not really needed

								assert(decreased);
							}
						}
					}
				}
			}
		}

impossibleteachersmaxhoursperallafternoons:
				if(!okteachersmaxhoursperallafternoons){
					if(updateSubgroups || updateTeachers)
						removeAiFromNewTimetable(ai, act, d, h);
					//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

					nConflActivities[newtime]=MAX_ACTIVITIES;
					continue;
				}

/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from teachers activity tag min hours daily
		
		//!!!NOT PERFECT, there is room for improvement
		
		okteachersactivitytagminhoursdaily=true;
		
		if(haveTeachersActivityTagMinHoursDaily){
			for(int tch : qAsConst(act->iTeachersList)){
				for(TeacherActivityTagMinHoursDaily_item* item : qAsConst(tatmhdListForTeacher[tch])){
					if(!gt.rules.internalActivitiesList[ai].iActivityTagsSet.contains(item->activityTag))
						continue;
					
					//int tchDayNHoursWithTag[MAX_DAYS_PER_WEEK];
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++)
						tchDayNHoursWithTag[d2]=0;
						
					//bool possibleToEmptyDay[MAX_DAYS_PER_WEEK];
					
					//code similar to getTchTimetable
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						possibleToEmptyDay[d2]=true;
						for(int h2=0; h2<gt.rules.nHoursPerDay; h2++){
							int ai2=newTeachersTimetable(tch,d2,h2);
							if(ai2>=0 && !conflActivities[newtime].contains(ai2)){
								if(gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(item->activityTag)){
									tchTimetable(d2,h2)=ai2;
									tchDayNHoursWithTag[d2]++;
									
									if(item->allowEmptyDays && (ai2==ai || fixedTimeActivity[ai2] || swappedActivities[ai2]))
										possibleToEmptyDay[d2]=false;
								}
								else{
									tchTimetable(d2,h2)=-1;
								}
							}
							else{
								tchTimetable(d2,h2)=-1;
							}
						}
					}
					
					int necessary=0;
					for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
						if(tchDayNHoursWithTag[d2]>0 || (tchDayNHoursWithTag[d2]==0 && !item->allowEmptyDays)){
							necessary+=max(item->minHoursDaily, tchDayNHoursWithTag[d2]);
						}
					}
					
					if(necessary > item->durationOfActivitiesWithActivityTagForTeacher){
						//not OK
						if(level>=LEVEL_STOP_CONFLICTS_CALCULATION){
							okteachersactivitytagminhoursdaily=false;
							goto impossibleteachersactivitytagminhoursdaily;
						}
						
						QSet<int> candidatesSet;
						
						for(int d2=0; d2<gt.rules.nDaysPerWeek; d2++){
							if(tchDayNHoursWithTag[d2]>0){
								if((item->allowEmptyDays && possibleToEmptyDay[d2]) || tchDayNHoursWithTag[d2] > item->minHoursDaily){
									for(int h2=0; h2<gt.rules.nHoursPerDay; ){
										int ai2=tchTimetable(d2,h2);
										if(ai2>=0){
											if(ai2!=ai && !fixedTimeActivity[ai2] && !swappedActivities[ai2])
												candidatesSet.insert(ai2);
											h2+=gt.rules.internalActivitiesList[ai2].duration;
										}
										else{
											h2++;
										}
									}
								}
							}
						}
						
						for(;;){
							if(candidatesSet.count()==0){
								okteachersactivitytagminhoursdaily=false;
								goto impossibleteachersactivitytagminhoursdaily;
							}
							else{
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
								QList<int> candidatesList(candidatesSet.constBegin(), candidatesSet.constEnd());
#else
								QList<int> candidatesList=candidatesSet.toList();
#endif
								std::stable_sort(candidatesList.begin(), candidatesList.end()); //To keep the generation identical on all computers.
							
								int ai2;
								if(level>0){
									int q=rng.intMRG32k3a(candidatesList.count());
									ai2=candidatesList.at(q);
								}
								else{
									assert(level==0);
				
									int optMinWrong=INF;
	
									QList<int> tl;
			
									for(int ai3 : qAsConst(candidatesList)){
										if(optMinWrong>triedRemovals(ai3,c.times[ai3])){
										 	optMinWrong=triedRemovals(ai3,c.times[ai3]);
										 	tl.clear();
										 	tl.append(ai3);
										}
										else if(optMinWrong==triedRemovals(ai3,c.times[ai3])){
										 	tl.append(ai3);
										}
									}
							
									assert(tl.count()>0);
									int q=rng.intMRG32k3a(tl.count());
									ai2=tl.at(q);
								}
						
								assert(ai2!=ai);
								assert(!fixedTimeActivity[ai2] && !swappedActivities[ai2]);
	
								assert(!conflActivities[newtime].contains(ai2));
								conflActivities[newtime].append(ai2);
	
								nConflActivities[newtime]++;
								assert(nConflActivities[newtime]==conflActivities[newtime].count());

								int d2=c.times[ai2]%gt.rules.nDaysPerWeek;
								int h2=c.times[ai2]/gt.rules.nDaysPerWeek;
								int dur2=gt.rules.internalActivitiesList[ai2].duration;
								tchDayNHoursWithTag[d2]-=dur2;
								assert(tchDayNHoursWithTag[d2]>=0);
								
								if(tchDayNHoursWithTag[d2]==0 && item->allowEmptyDays){
									necessary-=max(item->minHoursDaily, dur2);
									assert(necessary>=0);
								}
								else{
									int oldNecessary=necessary;
									necessary-=max(item->minHoursDaily, tchDayNHoursWithTag[d2]+dur2);
									assert(necessary>=0);
									necessary+=max(item->minHoursDaily, tchDayNHoursWithTag[d2]);
									if(!item->allowEmptyDays)
										assert(oldNecessary>necessary);
								}
								
								for(int h3=h2; h3<h2+dur2; h3++){
									assert(tchTimetable(d2,h3)==ai2);
									tchTimetable(d2,h3)=-1;
								}
								
								if(necessary <= item->durationOfActivitiesWithActivityTagForTeacher)
									break; //OK
									
								bool tr=candidatesSet.remove(ai2);
								assert(tr);
								
								if(tchDayNHoursWithTag[d2]>0 && tchDayNHoursWithTag[d2]<=item->minHoursDaily &&
								 !(item->allowEmptyDays && possibleToEmptyDay[d2])){
									for(int h3=0; h3<gt.rules.nHoursPerDay; ){
										int ai3=tchTimetable(d2,h3);
										if(ai3>=0){
											if(ai3!=ai && ai3!=ai2 && !fixedTimeActivity[ai3] && !swappedActivities[ai3]){
												assert(candidatesSet.contains(ai3));
												candidatesSet.remove(ai3);
											}
											h3+=gt.rules.internalActivitiesList[ai3].duration;
										}
										else{
											h3++;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		
impossibleteachersactivitytagminhoursdaily:
		if(!okteachersactivitytagminhoursdaily){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
				
/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from teachers min gaps between ordered pair of activity tags
		
		okteachersmingapsbetweenorderedpairofactivitytags=true;
		
		for(TeachersMinGapsBetweenOrderedPairOfActivityTags_item* item : qAsConst(tmgbopoatListForActivity[ai])){
			bool first, second;
			if(act->iActivityTagsSet.contains(item->firstActivityTag))
				first=true;
			else
				first=false;

			if(act->iActivityTagsSet.contains(item->secondActivityTag))
				second=true;
			else
				second=false;

			assert((first && !second) || (!first && second));

			if(first){
				assert(!second);
				//after the first activity tag we need to have at least minGaps until the second activity tag.

				for(int tch : qAsConst(act->iTeachersList)){
					if(item->canonicalSetOfTeachers.contains(tch)){
						for(int startSecond=h+act->duration; startSecond<gt.rules.nHoursPerDay; startSecond++){
							if(startSecond-h-act->duration >= item->minGaps)
								break;
							int ai2=teachersTimetable(tch,d,startSecond);
							if(ai2>=0){
								if(gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(item->secondActivityTag)){
									if(!conflActivities[newtime].contains(ai2)){
										if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
											okteachersmingapsbetweenorderedpairofactivitytags=false;
											goto impossibleteachersmingapsbetweenorderedpairofactivitytags;
										}
										else{
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
			else{
				assert(second);
				//before the second activity tag we need to have at least minGaps until the first activity tag.

				for(int tch : qAsConst(act->iTeachersList)){
					if(item->canonicalSetOfTeachers.contains(tch)){
						for(int endFirst=h-1; endFirst>=0; endFirst--){
							if(h-1-endFirst >= item->minGaps)
								break;
							int ai2=teachersTimetable(tch,d,endFirst);
							if(ai2>=0){
								if(gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(item->firstActivityTag)){
									if(!conflActivities[newtime].contains(ai2)){
										if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
											okteachersmingapsbetweenorderedpairofactivitytags=false;
											goto impossibleteachersmingapsbetweenorderedpairofactivitytags;
										}
										else{
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
		}
		
impossibleteachersmingapsbetweenorderedpairofactivitytags:
		if(!okteachersmingapsbetweenorderedpairofactivitytags){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		

/////////////////////////////////////////////////////////////////////////////////////////////

		//allowed from teachers min gaps between activity tag
		
		okteachersmingapsbetweenactivitytag=true;
		
		for(TeachersMinGapsBetweenActivityTag_item* item : qAsConst(tmgbatListForActivity[ai])){
			bool hasTag;
			if(act->iActivityTagsSet.contains(item->activityTag))
				hasTag=true;
			else
				hasTag=false;

			if(hasTag){
				//before and after the activity tag we need to have at least minGaps until the same activity tag.
				for(int tch : qAsConst(act->iTeachersList)){
					if(item->canonicalSetOfTeachers.contains(tch)){
						//after the current activity
						for(int startSecond=h+act->duration; startSecond<gt.rules.nHoursPerDay; startSecond++){
							if(startSecond-h-act->duration >= item->minGaps)
								break;
							int ai2=teachersTimetable(tch,d,startSecond);
							if(ai2>=0){
								if(gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(item->activityTag)){
									if(!conflActivities[newtime].contains(ai2)){
										if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
											okteachersmingapsbetweenactivitytag=false;
											goto impossibleteachersmingapsbetweenactivitytag;
										}
										else{
											conflActivities[newtime].append(ai2);
											nConflActivities[newtime]++;
											assert(conflActivities[newtime].count()==nConflActivities[newtime]);
										}
									}
								}
							}
						}

						//before the current activity
						for(int startSecond=h-1; startSecond>=0; startSecond--){
							if(h-startSecond > item->minGaps)
								break;
							int ai2=teachersTimetable(tch,d,startSecond);
							if(ai2>=0){
								if(gt.rules.internalActivitiesList[ai2].iActivityTagsSet.contains(item->activityTag)){
									if(!conflActivities[newtime].contains(ai2)){
										if(fixedTimeActivity[ai2] || swappedActivities[ai2]){
											okteachersmingapsbetweenactivitytag=false;
											goto impossibleteachersmingapsbetweenactivitytag;
										}
										else{
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
		}
		
impossibleteachersmingapsbetweenactivitytag:
		if(!okteachersmingapsbetweenactivitytag){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		

/////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////

		//2011-09-30
		//care about activities max simultaneous in selected time slots
		//I think it is best to put this after all other major constraints
		//I think it is better to put this before activities occupy max time slots from selection
		if(haveActivitiesMaxSimultaneousConstraints && activityHasMaxSimultaneousConstraints[ai]){
			okactivitiesmaxsimultaneousinselectedtimeslots=true;
			
			for(ActivitiesMaxSimultaneousInSelectedTimeSlots_item* item : qAsConst(amsistsListForActivity[ai])){
				bool inSpecifiedTimeSlots=false;
				for(int t=newtime; t<newtime+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek)
					if(item->selectedTimeSlotsSet.contains(t)){
						inSpecifiedTimeSlots=true;
						break;
					}
					
				if(!inSpecifiedTimeSlots)
					continue;
					
				bool correct=true;
			
				for(int t=newtime; t<newtime+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
					if(item->selectedTimeSlotsSet.contains(t)){
						slotSetOfActivities[t]=activitiesAtTime[t];

						if(slotSetOfActivities[t].count()+1 <= item->maxSimultaneous)
							continue;

						slotSetOfActivities[t].intersect(item->activitiesSet);

						if(slotSetOfActivities[t].count()+1 <= item->maxSimultaneous)
							continue;

#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
						slotSetOfActivities[t].subtract(QSet<int>(conflActivities[newtime].constBegin(), conflActivities[newtime].constEnd()));
#else
						slotSetOfActivities[t].subtract(conflActivities[newtime].toSet());
#endif
						
						if(slotSetOfActivities[t].count()+1 <= item->maxSimultaneous)
							continue;
						
						assert(!slotSetOfActivities[t].contains(ai));
						slotSetOfActivities[t].insert(ai);

						correct=false;
					}
				}
				
				if(!correct){
					QList<QSet<int>> candidates;
					QSet<int> allCandidates;
					for(int t=newtime; t<newtime+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
						if(item->selectedTimeSlotsSet.contains(t)){
							if(slotSetOfActivities[t].count() > item->maxSimultaneous){
								QSet<int> tmpSet;
								
								assert(slotSetOfActivities[t].count() == item->maxSimultaneous+1);
								for(int ai2 : qAsConst(slotSetOfActivities[t]))
									if(ai2!=ai && !fixedTimeActivity[ai2] && !swappedActivities[ai2]){
										tmpSet.insert(ai2);
										if(!allCandidates.contains(ai2))
											allCandidates.insert(ai2);
									}
								
								candidates.append(tmpSet);
							}
						}
					}
					
					for(const QSet<int>& tmpSet : qAsConst(candidates))
						if(tmpSet.count()==0){
							okactivitiesmaxsimultaneousinselectedtimeslots=false;
							goto impossibleactivitiesmaxsimultaneousinselectedtimeslots;
						}
						
					//possible to fix
					while(candidates.count()>0){
						int tc=candidates.count();
						
						//To keep the generation identical on all computers - 2013-01-03
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
						QList<int> tmpSortedList=QList<int>(allCandidates.constBegin(), allCandidates.constEnd());
#else
						QList<int> tmpSortedList=allCandidates.toList();
#endif
						std::stable_sort(tmpSortedList.begin(), tmpSortedList.end());
						
						int ai2;

						if(level>0){
							assert(allCandidates.count()==tmpSortedList.count());
							int q=rng.intMRG32k3a(allCandidates.count());
							
							ai2=tmpSortedList.at(q);
							//int ai2=allCandidates.toList().at(q);
						}
						else{
							assert(level==0);
	
							int optMinWrong=INF;
							QList<int> tl;
			
							for(int ai3 : qAsConst(tmpSortedList)){
								if(optMinWrong>triedRemovals(ai3,c.times[ai3])){
								 	optMinWrong=triedRemovals(ai3,c.times[ai3]);
								 	tl.clear();
								 	tl.append(ai3);
								}
								else if(optMinWrong==triedRemovals(ai3,c.times[ai3])){
								 	tl.append(ai3);
								}
							}
							
							assert(tl.count()>0);
							int q=rng.intMRG32k3a(tl.count());
							ai2=tl.at(q);
						}

						////
						assert(ai2!=ai);
						assert(c.times[ai2]!=UNALLOCATED_TIME);
						assert(!fixedTimeActivity[ai2] && !swappedActivities[ai2]);
						
						assert(!conflActivities[newtime].contains(ai2));
						conflActivities[newtime].append(ai2);
					
						nConflActivities[newtime]++;
						assert(nConflActivities[newtime]==conflActivities[newtime].count());
						////
	
						QList<QSet<int>> newCandidates;
						QSet<int> newAllCandidates;
						
						for(const QSet<int>& tmpSet : qAsConst(candidates))
							if(!tmpSet.contains(ai2)){
								newCandidates.append(tmpSet);
								newAllCandidates.unite(tmpSet);
							}
						
						allCandidates=newAllCandidates;
						candidates=newCandidates;
						
						assert(candidates.count()<tc); //need to have a progress, not an endless loop
					}
				}
			}
			
impossibleactivitiesmaxsimultaneousinselectedtimeslots:
			if(!okactivitiesmaxsimultaneousinselectedtimeslots){
				if(updateSubgroups || updateTeachers)
					removeAiFromNewTimetable(ai, act, d, h);
				//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

				nConflActivities[newtime]=MAX_ACTIVITIES;
				continue;
			}
		}
	
/////////////////////////////////////////////////////////////////////////////////////////////

		//2011-09-25
		//care about activities max number of occupied time slots from selection
		//I think it is best to put this after all other major constraints
		//I think it is best to put this after activities max simultaneous in selected time slots

		if(haveActivitiesOccupyMaxConstraints && activityHasOccupyMaxConstraints[ai]){
			okactivitiesoccupymaxtimeslotsfromselection=true;
			
			for(ActivitiesOccupyMaxTimeSlotsFromSelection_item* item : qAsConst(aomtsListForActivity[ai])){
				//preliminary, for speed
				bool contained=false;
				for(int t=newtime; t<newtime+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek)
					if(item->selectedTimeSlotsSet.contains(t)){
						contained=true;
						break;
					}
				if(!contained)
					continue;

				int _nOcc=0;
				for(int t : qAsConst(item->selectedTimeSlotsList))
					if(activitiesAtTime[t].count()>0)
						_nOcc++;
				for(int t=newtime; t<newtime+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek)
					if(item->selectedTimeSlotsSet.contains(t))
						if(activitiesAtTime[t].count()==0)
							_nOcc++;
				if(_nOcc<=item->maxOccupiedTimeSlots)
					continue;
				///////
			
				for(int t : qAsConst(item->selectedTimeSlotsList)){
					slotSetOfActivities[t].clear();
					slotCanEmpty[t]=true;
				}
				
				for(int ai2 : qAsConst(item->activitiesList)){
					if(ai2!=ai && c.times[ai2]!=UNALLOCATED_TIME && !conflActivities[newtime].contains(ai2)){
						for(int t=c.times[ai2]; t<c.times[ai2]+gt.rules.internalActivitiesList[ai2].duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
							if(item->selectedTimeSlotsSet.contains(t)){
								slotSetOfActivities[t].insert(ai2);
								
								if(fixedTimeActivity[ai2] || swappedActivities[ai2])
									slotCanEmpty[t]=false;
							}
						}
					}
					else if(ai2==ai){
						for(int t=newtime; t<newtime+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
							if(item->selectedTimeSlotsSet.contains(t)){
								slotSetOfActivities[t].insert(ai);
								slotCanEmpty[t]=false;
							}
						}
					}
				}
				
				int nOccupied=0;
				QSet<int> candidates;
				for(int t : qAsConst(item->selectedTimeSlotsList)){
					if(slotSetOfActivities[t].count()>0){
						nOccupied++;
						
						if(slotCanEmpty[t])
							candidates.insert(t);
					}
				}
				
				if(nOccupied > item->maxOccupiedTimeSlots){
					int target = nOccupied - item->maxOccupiedTimeSlots;
					while(target>0){
						bool decreased=false;
						
						if(candidates.count()==0){
							okactivitiesoccupymaxtimeslotsfromselection=false;
							goto impossibleactivitiesoccupymaxtimeslotsfromselection;
						}
						
						//To keep the generation identical on all computers - 2013-01-03
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
						QList<int> tmpSortedList=QList<int>(candidates.constBegin(), candidates.constEnd());
#else
						QList<int> tmpSortedList=candidates.toList();
#endif
						std::stable_sort(tmpSortedList.begin(), tmpSortedList.end());

						int t;
						if(level>0){
							assert(candidates.count()==tmpSortedList.count());
							int q=rng.intMRG32k3a(candidates.count());
							t=tmpSortedList.at(q);
						}
						else{
							assert(level==0);
							
							QList<int> optCandidates;
							
							int optConflActivities=MAX_ACTIVITIES;
							int optMinWrong=INF;
							int optNWrong=INF;
							int optMinIndexAct=gt.rules.nInternalActivities;

							for(int k : qAsConst(tmpSortedList)){
								const QSet<int>& acts=slotSetOfActivities[k];
								assert(acts.count()>0);
								
								int tmp_n_confl_acts=acts.count();
								int tmp_minWrong=INF;
								int tmp_nWrong=0;
								int tmp_minIndexAct=gt.rules.nInternalActivities;
								
								for(int ai2 : qAsConst(acts)){
									tmp_minWrong=min(tmp_minWrong, triedRemovals(ai2,c.times[ai2]));
									tmp_nWrong+=triedRemovals(ai2,c.times[ai2]);
									tmp_minIndexAct=min(tmp_minIndexAct, invPermutation[ai2]);
								}

								if(optMinWrong>tmp_minWrong ||
								  (optMinWrong==tmp_minWrong && optNWrong>tmp_nWrong) ||
								  (optMinWrong==tmp_minWrong && optNWrong==tmp_nWrong && optConflActivities>tmp_n_confl_acts) ||
								  (optMinWrong==tmp_minWrong && optNWrong==tmp_nWrong && optConflActivities==tmp_n_confl_acts && optMinIndexAct>tmp_minIndexAct)){
									optConflActivities=tmp_n_confl_acts;
									optMinWrong=tmp_minWrong;
									optNWrong=tmp_nWrong;
									optMinIndexAct=tmp_minIndexAct;
									
									optCandidates.clear();
									optCandidates.append(k);
								}
								else if(optMinWrong==tmp_minWrong && optNWrong==tmp_nWrong && optConflActivities==tmp_n_confl_acts && optMinIndexAct==tmp_minIndexAct){
									//be very careful, this case is possible if we have activities with duration >= 2. Don't assert(0).
									optCandidates.append(k);
								}
							}
							
							assert(optCandidates.count()>0);
							int q=rng.intMRG32k3a(optCandidates.count());
							t=optCandidates.at(q);
						}
						/*int t=tmpSortedList.at(q);*/
						//int t=candidates.toList().at(q);
						
						const QSet<int>& tmpSet=slotSetOfActivities[t];
						//To keep the generation identical on all computers - 2013-01-03
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
						QList<int> tmpListFromSet=QList<int>(tmpSet.constBegin(), tmpSet.constEnd());
#else
						QList<int> tmpListFromSet=tmpSet.toList();
#endif
						std::stable_sort(tmpListFromSet.begin(), tmpListFromSet.end());
						//Randomize list
						for(int i=0; i<tmpListFromSet.count(); i++){
							int t=tmpListFromSet.at(i);
							int r=rng.intMRG32k3a(tmpListFromSet.count()-i);
							tmpListFromSet[i]=tmpListFromSet[i+r];
							tmpListFromSet[i+r]=t;
						}
						
						for(int ai2 : qAsConst(tmpListFromSet)){
							assert(ai2!=ai);
							assert(c.times[ai2]!=UNALLOCATED_TIME);
							assert(!fixedTimeActivity[ai2] && !swappedActivities[ai2]);
							
							for(int tt=c.times[ai2]; tt<c.times[ai2]+gt.rules.internalActivitiesList[ai2].duration*gt.rules.nDaysPerWeek; tt+=gt.rules.nDaysPerWeek)
								if(item->selectedTimeSlotsSet.contains(tt)){
									assert(slotSetOfActivities[tt].contains(ai2));
									slotSetOfActivities[tt].remove(ai2);
									if(slotSetOfActivities[tt].count()==0){
										assert(candidates.contains(tt));
										candidates.remove(tt);
										target--;
										
										decreased=true;
									}
								}
							
							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);

							nConflActivities[newtime]++;
							assert(nConflActivities[newtime]==conflActivities[newtime].count());
						}
						
						assert(decreased);
					}
				}
			}
			
impossibleactivitiesoccupymaxtimeslotsfromselection:
			if(!okactivitiesoccupymaxtimeslotsfromselection){
				if(updateSubgroups || updateTeachers)
					removeAiFromNewTimetable(ai, act, d, h);
				//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

				nConflActivities[newtime]=MAX_ACTIVITIES;
				continue;
			}
		}
		
/////////////////////////////////////////////////////////////////////////////////////////////

		//2020-04-30
		//care about max total activities from set in selected time slots
		//I think it is best to put this after all other major constraints
		//I think it is best to put this after activities max simultaneous in selected time slots
		//I think it is best to put this after activities occupy max time slots from selection

		okmaxtotalactivitiesfromsetinselectedtimeslots=true;

		if(gt.rules.mode==BLOCK_PLANNING){
			for(ActivitiesMaxTotalFromSetInSelectedTimeSlots_item* item : qAsConst(amtfsistsListForActivity[ai])){
				bool aiContained=false;
				for(int t=newtime; t<newtime+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
					if(item->selectedTimeSlotsSet.contains(t)){
						aiContained=true;
						break;
					}
				}

				if(aiContained){
					int _nOcc=1; //current activity is counted

					QList<int> candidatesList;

					for(int ai2 : qAsConst(item->activitiesList)){
						if(ai2!=ai && c.times[ai2]!=UNALLOCATED_TIME && !conflActivities[newtime].contains(ai2)){
							for(int t=c.times[ai2]; t<c.times[ai2]+gt.rules.internalActivitiesList[ai2].duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
								if(item->selectedTimeSlotsSet.contains(t)){
									_nOcc++;

									if(!fixedTimeActivity[ai2] && !swappedActivities[ai2])
										candidatesList.append(ai2);

									break;
								}
							}
						}
					}

					if(_nOcc>item->maxActivities){ //not OK
						assert(_nOcc==item->maxActivities+1);

						if(candidatesList.count()==0){
							okmaxtotalactivitiesfromsetinselectedtimeslots=false;
							goto impossiblemaxtotalactivitiesfromsetinselectedtimeslots;
						}
						else{
							int ai2;
							if(level>0){
								int q=rng.intMRG32k3a(candidatesList.count());
								ai2=candidatesList.at(q);
							}
							else{
								assert(level==0);

								int optMinWrong=INF;

								QList<int> tl;

								for(int ai3 : qAsConst(candidatesList)){
									if(optMinWrong>triedRemovals(ai3,c.times[ai3])){
										optMinWrong=triedRemovals(ai3,c.times[ai3]);
										tl.clear();
										tl.append(ai3);
									}
									else if(optMinWrong==triedRemovals(ai3,c.times[ai3])){
										tl.append(ai3);
									}
								}

								assert(tl.count()>0);
								int q=rng.intMRG32k3a(tl.count());
								ai2=tl.at(q);
							}

							assert(ai2!=ai);
							assert(!fixedTimeActivity[ai2] && !swappedActivities[ai2]);

							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);

							nConflActivities[newtime]++;
							assert(nConflActivities[newtime]==conflActivities[newtime].count());
						}
					}
				}
			}
		}

impossiblemaxtotalactivitiesfromsetinselectedtimeslots:
		if(!okmaxtotalactivitiesfromsetinselectedtimeslots){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//2019-11-16
		//care about activities min number of occupied time slots from selection
		//I think it is best to put this after all other major constraints
		//I think it is best to put this after activities max simultaneous in selected time slots
		//I think it is best to put this after activities occupy max time slots from selection
		
		okactivitiesoccupymintimeslotsfromselection=true;

		if(!aomintsListForActivity[ai].isEmpty())
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			conflActivitiesSet=QSet<int>(conflActivities[newtime].constBegin(), conflActivities[newtime].constEnd());
#else
			conflActivitiesSet=conflActivities[newtime].toSet();
#endif

		for(ActivitiesOccupyMinTimeSlotsFromSelection_item* item : qAsConst(aomintsListForActivity[ai])){
			int availableDuration=0;
			int nOccupied=0;
			//static int aminoCnt[MAX_DAYS_PER_WEEK*MAX_HOURS_PER_DAY];

			for(int t : qAsConst(item->selectedTimeSlotsList))
				aminoCnt[t]=0;
			for(int ai2 : qAsConst(item->activitiesList)){
				if(ai2==ai){
					for(int t=newtime; t<newtime+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
						if(item->selectedTimeSlotsSet.contains(t)){
							if(aminoCnt[t]==0)
								nOccupied++;
							aminoCnt[t]++;
						}
					}
				}
				else if(c.times[ai2]!=UNALLOCATED_TIME && !conflActivitiesSet.contains(ai2)){
					assert(ai2!=ai);
					for(int t=c.times[ai2]; t<c.times[ai2]+gt.rules.internalActivitiesList[ai2].duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
						if(item->selectedTimeSlotsSet.contains(t)){
							if(aminoCnt[t]==0)
								nOccupied++;
							aminoCnt[t]++;
						}
					}
					
				}
				else{
					assert(ai2!=ai);
					availableDuration+=gt.rules.internalActivitiesList[ai2].duration;
				}

				if(nOccupied+availableDuration >= item->minOccupiedTimeSlots){
					break; //OK
				}
			}

			assert(nOccupied<=item->selectedTimeSlotsSet.count());

			if(nOccupied+availableDuration < item->minOccupiedTimeSlots){
				//not OK
				for(;;){
					QList<int> candidatesList;
					for(int ai2 : qAsConst(item->activitiesList)){
						if(ai2!=ai && c.times[ai2]!=UNALLOCATED_TIME && !conflActivitiesSet.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2]){
							for(int t=c.times[ai2]; t<c.times[ai2]+gt.rules.internalActivitiesList[ai2].duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
								if(item->selectedTimeSlotsSet.contains(t)){
									assert(aminoCnt[t]>=1);
									if(aminoCnt[t]>=2){
										candidatesList.append(ai2);
										break;
									}
								}
								else{
									candidatesList.append(ai2);
									break;
								}
							}
						}
					}

					if(candidatesList.count()==0){
						okactivitiesoccupymintimeslotsfromselection=false;
						goto impossibleactivitiesoccupymintimeslotsfromselection;
					}
					else{
						int ai2;
						if(level>0){
							int q=rng.intMRG32k3a(candidatesList.count());
							ai2=candidatesList.at(q);
						}
						else{
							assert(level==0);
		
							int optMinWrong=INF;

							QList<int> tl;
			
							for(int ai3 : qAsConst(candidatesList)){
								if(optMinWrong>triedRemovals(ai3,c.times[ai3])){
									optMinWrong=triedRemovals(ai3,c.times[ai3]);
									tl.clear();
									tl.append(ai3);
								}
								else if(optMinWrong==triedRemovals(ai3,c.times[ai3])){
							 		tl.append(ai3);
								}
							}
							
							assert(tl.count()>0);
							int q=rng.intMRG32k3a(tl.count());
							ai2=tl.at(q);
						}
						
						assert(ai2!=ai);
						assert(!fixedTimeActivity[ai2] && !swappedActivities[ai2]);
	
						assert(!conflActivitiesSet.contains(ai2));
						assert(!conflActivities[newtime].contains(ai2));
						conflActivitiesSet.insert(ai2);
						conflActivities[newtime].append(ai2);

						nConflActivities[newtime]++;
						assert(nConflActivities[newtime]==conflActivities[newtime].count());
						
						for(int t=c.times[ai2]; t<c.times[ai2]+gt.rules.internalActivitiesList[ai2].duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
							if(item->selectedTimeSlotsSet.contains(t)){
								assert(aminoCnt[t]>=1);
								if(aminoCnt[t]==1){
									nOccupied--;
									assert(nOccupied>=0);
								}
								
								aminoCnt[t]--;
							}
						}
						availableDuration+=gt.rules.internalActivitiesList[ai2].duration;

						if(nOccupied+availableDuration >= item->minOccupiedTimeSlots){
							break; //OK
						}
					}
				}
			}
		}
		
impossibleactivitiesoccupymintimeslotsfromselection:
		if(!okactivitiesoccupymintimeslotsfromselection){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//2019-11-16
		//care about activities min simultaneous in selected time slots
		//I think it is best to put this after all other major constraints
		//I think it is best to put this after activities max simultaneous in selected time slots
		//I think it is best to put this after activities occupy max time slots from selection
		//I think it is best to put this after activities occupy min time slots from selection
		
		okactivitiesminsimultaneousinselectedtimeslots=true;

		if(!aminsistsListForActivity[ai].isEmpty())
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			conflActivitiesSet=QSet<int>(conflActivities[newtime].constBegin(), conflActivities[newtime].constEnd());
#else
			conflActivitiesSet=conflActivities[newtime].toSet();
#endif
		
		for(ActivitiesMinSimultaneousInSelectedTimeSlots_item* item : qAsConst(aminsistsListForActivity[ai])){
			int totalRequired;
			if(!item->allowEmptySlots)
				totalRequired = item->selectedTimeSlotsList.count() * item->minSimultaneous;
			else
				totalRequired=0;
			
			int availableDuration=0;
			//static int aminsCnt[MAX_DAYS_PER_WEEK*MAX_HOURS_PER_DAY];
			
			int partialFilled=0;
			
			//static bool possibleToEmptySlot[MAX_DAYS_PER_WEEK*MAX_HOURS_PER_DAY];
			
			for(int t : qAsConst(item->selectedTimeSlotsList)){
				aminsCnt[t]=0;
				possibleToEmptySlot[t]=true;
			}
			for(int ai2 : qAsConst(item->activitiesList)){
				if(ai2==ai){
					for(int t=newtime; t<newtime+act->duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
						if(item->selectedTimeSlotsSet.contains(t)){
							if(item->allowEmptySlots && aminsCnt[t]==0)
								totalRequired+=item->minSimultaneous;
						
							if(aminsCnt[t]<item->minSimultaneous)
								partialFilled++;
							
							aminsCnt[t]++;
							
							if(item->allowEmptySlots)
								if(possibleToEmptySlot[t])
									possibleToEmptySlot[t]=false;
						}
					}
				}
				else if(c.times[ai2]!=UNALLOCATED_TIME && !conflActivitiesSet.contains(ai2)){
					assert(ai2!=ai);
					for(int t=c.times[ai2]; t<c.times[ai2]+gt.rules.internalActivitiesList[ai2].duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
						if(item->selectedTimeSlotsSet.contains(t)){
							if(item->allowEmptySlots && aminsCnt[t]==0)
								totalRequired+=item->minSimultaneous;
						
							if(aminsCnt[t]<item->minSimultaneous)
								partialFilled++;
						
							aminsCnt[t]++;
							
							if(item->allowEmptySlots && (fixedTimeActivity[ai2] || swappedActivities[ai2]))
								if(possibleToEmptySlot[t])
									possibleToEmptySlot[t]=false;
						}
					}
				}
				else{
					assert(ai2!=ai);
					availableDuration+=gt.rules.internalActivitiesList[ai2].duration;
				}
				
				if(!item->allowEmptySlots && partialFilled+availableDuration >= totalRequired)
					break; //OK
			}
			
			if(partialFilled+availableDuration < totalRequired){
				//not OK
				for(;;){
					QList<int> candidatesList;
					for(int ai2 : qAsConst(item->activitiesList)){
						if(ai2!=ai && c.times[ai2]!=UNALLOCATED_TIME && !conflActivitiesSet.contains(ai2) && !fixedTimeActivity[ai2] && !swappedActivities[ai2]){
							for(int t=c.times[ai2]; t<c.times[ai2]+gt.rules.internalActivitiesList[ai2].duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
								if(item->selectedTimeSlotsSet.contains(t)){
									if(aminsCnt[t]>item->minSimultaneous || (item->allowEmptySlots && possibleToEmptySlot[t])){
										candidatesList.append(ai2);
										break;
									}
								}
								else{
									candidatesList.append(ai2);
									break;
								}
							}
						}
					}

					if(candidatesList.count()==0){
						okactivitiesminsimultaneousinselectedtimeslots=false;
						goto impossibleactivitiesminsimultaneousinselectedtimeslots;
					}
					else{
						int ai2;
						if(level>0){
							int q=rng.intMRG32k3a(candidatesList.count());
							ai2=candidatesList.at(q);
						}
						else{
							assert(level==0);
		
							int optMinWrong=INF;
	
							QList<int> tl;
			
							for(int ai3 : qAsConst(candidatesList)){
								if(optMinWrong>triedRemovals(ai3,c.times[ai3])){
								 	optMinWrong=triedRemovals(ai3,c.times[ai3]);
								 	tl.clear();
								 	tl.append(ai3);
								}
								else if(optMinWrong==triedRemovals(ai3,c.times[ai3])){
								 	tl.append(ai3);
								}
							}
							
							assert(tl.count()>0);
							int q=rng.intMRG32k3a(tl.count());
							ai2=tl.at(q);
						}
						
						assert(ai2!=ai);
						assert(!fixedTimeActivity[ai2] && !swappedActivities[ai2]);
	
						assert(!conflActivitiesSet.contains(ai2));
						assert(!conflActivities[newtime].contains(ai2));
						conflActivitiesSet.insert(ai2);
						conflActivities[newtime].append(ai2);
	
						nConflActivities[newtime]++;
						assert(nConflActivities[newtime]==conflActivities[newtime].count());

						for(int t=c.times[ai2]; t<c.times[ai2]+gt.rules.internalActivitiesList[ai2].duration*gt.rules.nDaysPerWeek; t+=gt.rules.nDaysPerWeek){
							if(item->selectedTimeSlotsSet.contains(t)){
								assert(aminsCnt[t]>=1);
								if(aminsCnt[t]<=item->minSimultaneous){
									partialFilled--;
									assert(partialFilled>=0);
								}
								
								aminsCnt[t]--;
								
								if(item->allowEmptySlots && aminsCnt[t]==0){
									totalRequired-=item->minSimultaneous;
									assert(totalRequired>=0);
								}
							}
						}
						availableDuration+=gt.rules.internalActivitiesList[ai2].duration;

						if(partialFilled+availableDuration >= totalRequired)
							break; //OK
					}
				}
			}
		}

impossibleactivitiesminsimultaneousinselectedtimeslots:
		if(!okactivitiesminsimultaneousinselectedtimeslots){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//2020-01-14

		okactivitiesmaxinaterm=true;
		if(gt.rules.mode==TERMS){
			if(amiatListForActivity[ai].count()>0){
				int termai = (newtime%gt.rules.nDaysPerWeek)/gt.rules.nDaysPerTerm;
				for(ActivitiesMaxInATerm_item* item : qAsConst(amiatListForActivity[ai])){
					int countTerm=1; //ai
					QList<int> candidates;

					for(int ai2 : qAsConst(item->activitiesList))
						if(ai2!=ai && c.times[ai2]!=UNALLOCATED_TIME && !conflActivities[newtime].contains(ai2)){
							int termai2 = (c.times[ai2]%gt.rules.nDaysPerWeek)/gt.rules.nDaysPerTerm;
							if(termai2==termai){
								countTerm++;
								if(!fixedTimeActivity[ai2] && !swappedActivities[ai2])
									candidates.append(ai2);
							}
						}

					if(countTerm > item->maxActivitiesInATerm){
						assert(countTerm==item->maxActivitiesInATerm+1);
						if(candidates.count()==0){
							okactivitiesmaxinaterm=false;
							goto impossibleactivitiesmaxinaterm;
						}
						int ai2=-1;
						if(level>0){
							ai2=candidates.at(rng.intMRG32k3a(candidates.count()));
						}
						else{
							assert(level==0);

							int optMinWrong=INF;

							QList<int> tl;

							for(int ai3 : qAsConst(candidates)){
								if(optMinWrong>triedRemovals(ai3,c.times[ai3])){
									optMinWrong=triedRemovals(ai3,c.times[ai3]);
									tl.clear();
									tl.append(ai3);
								}
								else if(optMinWrong==triedRemovals(ai3,c.times[ai3])){
									tl.append(ai3);
								}
							}

							assert(tl.count()>0);
							int q=rng.intMRG32k3a(tl.count());
							ai2=tl.at(q);
						}

						assert(ai2>=0);

						assert(!conflActivities[newtime].contains(ai2));
						conflActivities[newtime].append(ai2);

						nConflActivities[newtime]++;
						assert(nConflActivities[newtime]==conflActivities[newtime].count());
					}
				}
			}
		}

impossibleactivitiesmaxinaterm:
		if(!okactivitiesmaxinaterm){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//2022-05-19

		okactivitiesmininaterm=true;
		if(gt.rules.mode==TERMS){
			if(aminiatListForActivity[ai].count()>0){
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
				conflActivitiesSet=QSet<int>(conflActivities[newtime].constBegin(), conflActivities[newtime].constEnd());
#else
				conflActivitiesSet=conflActivities[newtime].toSet();
#endif
				int termai = (newtime%gt.rules.nDaysPerWeek)/gt.rules.nDaysPerTerm;
				for(ActivitiesMinInATerm_item* item : qAsConst(aminiatListForActivity[ai])){
					//int nAvailableActivities=0;
					
					for(int t=0; t<gt.rules.nTerms; t++){
						canEmptyTerm[t]=true;
						termActivities[t].clear();
					}
					
					//int nOccupiedTerms=0;

					for(int ai2 : qAsConst(item->activitiesList)){
						if(ai2==ai){
							termActivities[termai].append(ai);
							//if(termActivities[termai].count()==1)
							//	nOccupiedTerms++;
							if(item->allowEmptyTerms){
								if(canEmptyTerm[termai])
									canEmptyTerm[termai]=false;
							}
						}
						else if(c.times[ai2]!=UNALLOCATED_TIME && !conflActivitiesSet.contains(ai2)){
							assert(ai2!=ai);
							int termai2 = (c.times[ai2]%gt.rules.nDaysPerWeek)/gt.rules.nDaysPerTerm;
							termActivities[termai2].append(ai2);
							//if(termActivities[termai2].count()==1)
							//	nOccupiedTerms++;
							
							if(item->allowEmptyTerms && (fixedTimeActivity[ai2] || swappedActivities[ai2]))
								if(canEmptyTerm[termai2])
									canEmptyTerm[termai2]=false;
						}
						else{
							assert(ai2!=ai);
							//nAvailableActivities++;
						}
					}
					
					int nNecessary;
					if(!item->allowEmptyTerms){
						nNecessary=0;
						for(int t=0; t<gt.rules.nTerms; t++)
							nNecessary+=max(item->minActivitiesInATerm, int(termActivities[t].count()));
					}
					else{
						nNecessary=0;
						for(int t=0; t<gt.rules.nTerms; t++)
							if(termActivities[t].count()>0)
								nNecessary+=max(item->minActivitiesInATerm, int(termActivities[t].count()));
					}
					
					if(nNecessary>item->activitiesList.count()){ //not OK
						for(;;){
							QList<int> candidates;
							if(item->allowEmptyTerms){
								for(int t=0; t<gt.rules.nTerms; t++){
									if(canEmptyTerm[t]){
										for(int ai2 : qAsConst(termActivities[t])){
											assert(ai2!=ai && !(fixedTimeActivity[ai2] || swappedActivities[ai2]));
											candidates.append(ai2);
										}
									}
									else if(termActivities[t].count()>item->minActivitiesInATerm){
										for(int ai2 : qAsConst(termActivities[t])){
											if(ai2!=ai && !(fixedTimeActivity[ai2] || swappedActivities[ai2])){
												candidates.append(ai2);
											}
										}
									}
								}
							}
							else{
								for(int t=0; t<gt.rules.nTerms; t++){
									if(termActivities[t].count()>item->minActivitiesInATerm){
										for(int ai2 : qAsConst(termActivities[t])){
											if(ai2!=ai && !(fixedTimeActivity[ai2] || swappedActivities[ai2])){
												candidates.append(ai2);
											}
										}
									}
								}
							}
							
							if(candidates.count()==0){
								okactivitiesmininaterm=false;
								goto impossibleactivitiesmininaterm;
							}
							
							int ai2=-1;
							if(level>0){
								ai2=candidates.at(rng.intMRG32k3a(candidates.count()));
							}
							else{
								assert(level==0);

								int optMinWrong=INF;

								QList<int> tl;

								for(int ai3 : qAsConst(candidates)){
									if(optMinWrong>triedRemovals(ai3,c.times[ai3])){
										optMinWrong=triedRemovals(ai3,c.times[ai3]);
										tl.clear();
										tl.append(ai3);
									}
									else if(optMinWrong==triedRemovals(ai3,c.times[ai3])){
										tl.append(ai3);
									}
								}

								assert(tl.count()>0);
								int q=rng.intMRG32k3a(tl.count());
								ai2=tl.at(q);
							}
							
							assert(ai2>=0);

							assert(!conflActivitiesSet.contains(ai2));
							assert(!conflActivities[newtime].contains(ai2));
							conflActivitiesSet.insert(ai2);
							conflActivities[newtime].append(ai2);

							nConflActivities[newtime]++;
							assert(nConflActivities[newtime]==conflActivities[newtime].count());
							
							int tt=candidates.removeAll(ai2);
							assert(tt==1);
							int termai2 = (c.times[ai2]%gt.rules.nDaysPerWeek)/gt.rules.nDaysPerTerm;
							tt=termActivities[termai2].removeAll(ai2);
							assert(tt==1);

							int nNecessary;
							if(!item->allowEmptyTerms){
								nNecessary=0;
								for(int t=0; t<gt.rules.nTerms; t++)
									nNecessary+=max(item->minActivitiesInATerm, int(termActivities[t].count()));
							}
							else{
								nNecessary=0;
								for(int t=0; t<gt.rules.nTerms; t++)
									if(termActivities[t].count()>0)
										nNecessary+=max(item->minActivitiesInATerm, int(termActivities[t].count()));
							}
					
							if(nNecessary<=item->activitiesList.count()) //OK
								break;
						}
					}
				}
			}
		}

impossibleactivitiesmininaterm:
		if(!okactivitiesmininaterm){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

		//2020-01-14

		okactivitiesoccupymaxterms=true;
		if(gt.rules.mode==TERMS){
			if(aomtListForActivity[ai].count()>0){
				int termai = (newtime%gt.rules.nDaysPerWeek)/gt.rules.nDaysPerTerm;
				for(ActivitiesOccupyMaxTerms_item* item : qAsConst(aomtListForActivity[ai])){
					//bool canEmptyTerm[5];
					for(int i=0; i<gt.rules.nTerms; i++)
						canEmptyTerm[i]=true;

					//QList<int> termActivities[5];
					for(int i=0; i<gt.rules.nTerms; i++)
						termActivities[i].clear();
					
					termActivities[termai].append(ai);
					canEmptyTerm[termai]=false;

					for(int ai2 : qAsConst(item->activitiesList))
						if(ai2!=ai && c.times[ai2]!=UNALLOCATED_TIME && !conflActivities[newtime].contains(ai2)){
							int termai2 = (c.times[ai2]%gt.rules.nDaysPerWeek)/gt.rules.nDaysPerTerm;
							termActivities[termai2].append(ai2);
							if(fixedTimeActivity[ai2] || swappedActivities[ai2])
								canEmptyTerm[termai2]=false;
						}

					int nocc=0;
					for(int i=0; i<gt.rules.nTerms; i++)
						if(termActivities[i].count()>0)
							nocc++;

					if(nocc > item->maxOccupiedTerms){
						assert(nocc == item->maxOccupiedTerms+1);

						QList<int> candidateTerms;
						for(int i=0; i<gt.rules.nTerms; i++)
							if(canEmptyTerm[i] && termActivities[i].count()>0)
								candidateTerms.append(i);

						if(candidateTerms.count()==0){
							okactivitiesoccupymaxterms=false;
							goto impossibleactivitiesoccupymaxterms;
						}

						int t=-1;
						if(level>0){
							t=candidateTerms.at(rng.intMRG32k3a(candidateTerms.count()));
						}
						else{
							assert(level==0);

							QList<int> optCandidates;

							int optConflActivities=MAX_ACTIVITIES;
							int optMinWrong=INF;
							int optNWrong=INF;
							int optMinIndexAct=gt.rules.nInternalActivities;

							for(int k : qAsConst(candidateTerms)){
								assert(canEmptyTerm[k]);
								QList<int> acts=termActivities[k];
								assert(acts.count()>0);

								int tmp_n_confl_acts=acts.count();
								int tmp_minWrong=INF;
								int tmp_nWrong=0;
								int tmp_minIndexAct=gt.rules.nInternalActivities;

								for(int ai2 : qAsConst(acts)){
									tmp_minWrong=min(tmp_minWrong, triedRemovals(ai2,c.times[ai2]));
									tmp_nWrong+=triedRemovals(ai2,c.times[ai2]);
									tmp_minIndexAct=min(tmp_minIndexAct, invPermutation[ai2]);
								}

								if(optMinWrong>tmp_minWrong ||
								  (optMinWrong==tmp_minWrong && optNWrong>tmp_nWrong) ||
								  (optMinWrong==tmp_minWrong && optNWrong==tmp_nWrong && optConflActivities>tmp_n_confl_acts) ||
								  (optMinWrong==tmp_minWrong && optNWrong==tmp_nWrong && optConflActivities==tmp_n_confl_acts && optMinIndexAct>tmp_minIndexAct)){
									optConflActivities=tmp_n_confl_acts;
									optMinWrong=tmp_minWrong;
									optNWrong=tmp_nWrong;
									optMinIndexAct=tmp_minIndexAct;

									optCandidates.clear();
									optCandidates.append(k);
								}
								else if(optMinWrong==tmp_minWrong && optNWrong==tmp_nWrong && optConflActivities==tmp_n_confl_acts && optMinIndexAct==tmp_minIndexAct){
									//be very careful, this case is possible if we have activities with duration >= 2. Don't assert(0).
									optCandidates.append(k);
								}
							}

							assert(optCandidates.count()>0);
							int q=rng.intMRG32k3a(optCandidates.count());
							t=optCandidates.at(q);
						}

						assert(canEmptyTerm[t]);
						assert(termActivities[t].count()>0);

						for(int ai2 : qAsConst(termActivities[t])){
							assert(ai2!=ai);
							assert(c.times[ai2]!=UNALLOCATED_TIME);
							assert(!fixedTimeActivity[ai2] && !swappedActivities[ai2]);

							assert(!conflActivities[newtime].contains(ai2));
							conflActivities[newtime].append(ai2);

							nConflActivities[newtime]++;
							assert(nConflActivities[newtime]==conflActivities[newtime].count());
						}
					}
				}
			}
		}

impossibleactivitiesoccupymaxterms:
		if(!okactivitiesoccupymaxterms){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}

/////////////////////////////////////////////////////////////////////////////////////////////

skip_here_if_already_allocated_in_time:

		//////////////////rooms
		realRoomsListLevel[newtime].clear();
		bool okroomnotavailable=getRoom(level, act, ai, d, h, roomSlots[newtime], selectedRoom[newtime], conflActivities[newtime], nConflActivities[newtime], realRoomsListLevel[newtime]);
		
//impossibleroomnotavailable:
		if(!okroomnotavailable){
			if(c.times[ai]==UNALLOCATED_TIME){
				if(updateSubgroups || updateTeachers)
					removeAiFromNewTimetable(ai, act, d, h);
			}
			//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

			nConflActivities[newtime]=MAX_ACTIVITIES;
			continue;
		}
		///////////////////////

		if(c.times[ai]==UNALLOCATED_TIME){
			if(updateSubgroups || updateTeachers)
				removeAiFromNewTimetable(ai, act, d, h);
		}
		//removeConflActivities(conflActivities[newtime], nConflActivities[newtime], act, newtime);

		///////////////////////////////
		//5.0.0-preview28
		//no conflicting activities for this timeslot - place the activity and return
		if(nConflActivities[newtime]==0 && nMinDaysBroken[newtime]==0.0){
			assert(c.times[ai]==UNALLOCATED_TIME || (fixedTimeActivity[ai] && !fixedSpaceActivity[ai]));
			
			if(c.times[ai]!=UNALLOCATED_TIME && fixedTimeActivity[ai] && !fixedSpaceActivity[ai])
				assert(c.times[ai]==newtime);
			
			assert(conflActivities[newtime].count()==0);
		
			restoreActIndex[nRestore]=ai;
			restoreTime[nRestore]=c.times[ai];
			restoreRoom[nRestore]=c.rooms[ai];
			restoreRealRoomsList[nRestore]=c.realRoomsList[ai];
			nRestore++;
			
			//5.0.0-preview25
			assert(swappedActivities[ai]);
			
			moveActivity(ai, c.times[ai], newtime, c.rooms[ai], selectedRoom[newtime], c.realRoomsList[ai], realRoomsListLevel[newtime]);
			
			foundGoodSwap=true;
			return;
		}
		///////////////////////////////
		
		assert(nConflActivities[newtime]==conflActivities[newtime].count());
	}
	
	//O(n*log(n)) stable sorting
	currentLevel=level;
	std::stable_sort(perm+0, perm+gt.rules.nHoursPerWeek, [this](int i, int j){return compareFunctionGenerate(i, j);});
	
	for(int i=1; i<gt.rules.nHoursPerWeek; i++){
		assert( (nConflActivities[perm[i-1]]<nConflActivities[perm[i]])
		 || ( (nConflActivities[perm[i-1]]==nConflActivities[perm[i]]) &&
		 (nMinDaysBroken[perm[i-1]]<=nMinDaysBroken[perm[i]]) ) );
	}

	if(level==0 && (nConflActivities[perm[0]]==MAX_ACTIVITIES)){
		//to check if the generation was stopped
		/*if(this->isThreaded){
			myMutex.unlock();
			myMutex.lock();
		}*/
		if(!abortOptimization && activity_count_impossible_tries<MAX_RETRIES_FOR_AN_ACTIVITY_AT_LEVEL_0){
			if(activity_count_impossible_tries%10000==0){
				time_t crt_time;
				time(&crt_time);
				searchTime=int(difftime(crt_time, starting_time));

				emit(activityPlaced(nThread, currentlyNPlacedActivities));
				if(isThreaded){
					myMutex.unlock();
					semaphorePlacedActivity.acquire();
					myMutex.lock();
				}
			}

			activity_count_impossible_tries++;
			goto again_if_impossible_activity;
		}
		else{
			if(VERBOSE){
				cout<<__FILE__<<" line "<<__LINE__<<" - WARNING - after retrying for "<<activity_count_impossible_tries
				<<" times - no possible time slot for activity with id=="<<gt.rules.internalActivitiesList[ai].id<<endl;
			}
		}
	}
	
	if(level==0){
		for(int i=0; i<gt.rules.nHoursPerWeek; i++){
			l0nWrong[i]=INF;
			l0minWrong[i]=INF;
			l0minIndexAct[i]=gt.rules.nInternalActivities;
		}
		
		QList<int> tim;
		for(int i=0; i<gt.rules.nHoursPerWeek; i++)
			if(nConflActivities[perm[i]]>0 && nConflActivities[perm[i]]<MAX_ACTIVITIES
			 && roomSlots[perm[i]]!=UNALLOCATED_SPACE)
				tim.append(perm[i]);
		if(tim.count()==0 && nConflActivities[perm[0]]==MAX_ACTIVITIES){
			//cout<<__FILE__<<" line "<<__LINE__<<" - WARNING - no possible time slot for activity with id=="<<
			// gt.rules.internalActivitiesList[ai].id<<endl;
			
			impossibleActivity=true;
		}
		if(tim.count()>0){
			for(int i : qAsConst(tim)){
				int cnt=0;
				int m=gt.rules.nInternalActivities;
				for(int aii : qAsConst(conflActivities[i])){
					if(triedRemovals(aii,c.times[aii])>0)
						cnt+=triedRemovals(aii,c.times[aii]);
						
					if(l0minWrong[i]>triedRemovals(aii,c.times[aii]))
						l0minWrong[i]=triedRemovals(aii,c.times[aii]);
						
					int j=invPermutation[aii];
					if(m>j)
						m=j;
				}
				l0nWrong[i]=cnt;
				l0minIndexAct[i]=m;
			}
			
			int optMinIndex=gt.rules.nInternalActivities;
			int optNWrong=INF;
			int optMinWrong=INF;
			int optNConflActs=gt.rules.nInternalActivities;
			int j=-1;
			
			for(int i : qAsConst(tim)){
				//choose a random time out of those with the minimum number of wrongly replaced activities
				if(optMinWrong>l0minWrong[i]
				 || (optMinWrong==l0minWrong[i] && optNWrong>l0nWrong[i])
				 || (optMinWrong==l0minWrong[i] && optNWrong==l0nWrong[i] && optNConflActs>nConflActivities[i])
				 || (optMinWrong==l0minWrong[i] && optNWrong==l0nWrong[i] && optNConflActs==nConflActivities[i] && optMinIndex>l0minIndexAct[i])){
					optNWrong=l0nWrong[i];
					optMinWrong=l0minWrong[i];
					optNConflActs=nConflActivities[i];
					optMinIndex=l0minIndexAct[i];
					j=i;
				}
			}
			
			assert(j>=0);
			QList<int> tim2;
			for(int i : qAsConst(tim))
				if(optNWrong==l0nWrong[i] && l0minWrong[i]==optMinWrong && optNConflActs==nConflActivities[i] && optMinIndex==l0minIndexAct[i])
					tim2.append(i);
			assert(tim2.count()>0);
			int rnd=rng.intMRG32k3a(tim2.count());
			j=tim2.at(rnd);

			assert(j>=0);
			timeSlot=j;
			assert(roomSlots[j]!=UNALLOCATED_SPACE);
			roomSlot=roomSlots[j];
			realRoomsSlot=realRoomsListLevel[j];
			
			conflActivitiesTimeSlot=conflActivities[timeSlot];
		}
	}

	for(int i=0; i<gt.rules.nHoursPerWeek; i++){
		int newtime=perm[i]; //the considered time
		if(nConflActivities[newtime]>=MAX_ACTIVITIES)
			break;
		
		assert(c.times[ai]==UNALLOCATED_TIME || (fixedTimeActivity[ai]&&!fixedSpaceActivity[ai]));
		
		//no conflicting activities for this timeslot - place the activity and return
		if(nConflActivities[newtime]==0){
			assert(c.times[ai]==UNALLOCATED_TIME || (fixedTimeActivity[ai]&&!fixedSpaceActivity[ai]));
			
			if(c.times[ai]!=UNALLOCATED_TIME && fixedTimeActivity[ai] && !fixedSpaceActivity[ai])
				assert(c.times[ai]==newtime);
			
			assert(conflActivities[newtime].count()==0);
		
			restoreActIndex[nRestore]=ai;
			restoreTime[nRestore]=c.times[ai];
			restoreRoom[nRestore]=c.rooms[ai];
			restoreRealRoomsList[nRestore]=c.realRoomsList[ai];
			nRestore++;
			
			//5.0.0-preview25
			assert(swappedActivities[ai]);
			
			moveActivity(ai, c.times[ai], newtime, c.rooms[ai], selectedRoom[newtime], c.realRoomsList[ai], realRoomsListLevel[newtime]);
			
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
			for(int a : qAsConst(conflActivities[newtime])){
				if(swappedActivities[a]){
					assert(0);
					ok=false;
					break;
				}
				assert(!(fixedTimeActivity[a] && fixedSpaceActivity[a]));
			}
			
			if(!ok){
				assert(0);
				continue;
			}
			
			//////////////place it at a new time
			
			int oldNRestore=nRestore;
			
			////////////////
			assert(conflActivities[newtime].count()>0);
			
			for(int a : qAsConst(conflActivities[newtime])){
				restoreActIndex[nRestore]=a;
				restoreTime[nRestore]=c.times[a];
				restoreRoom[nRestore]=c.rooms[a];
				restoreRealRoomsList[nRestore]=c.realRoomsList[a];
				nRestore++;
				
				assert(c.times[a]!=UNALLOCATED_TIME);
				assert(c.rooms[a]!=UNALLOCATED_SPACE);
				int nt=UNALLOCATED_TIME;
				if(fixedTimeActivity[a] && !fixedSpaceActivity[a])
					nt=c.times[a];
				moveActivity(a, c.times[a], nt, c.rooms[a], UNALLOCATED_SPACE, c.realRoomsList[a], QList<int>());
			}
			////////////////
			
			restoreActIndex[nRestore]=ai;
			restoreTime[nRestore]=c.times[ai];
			restoreRoom[nRestore]=c.rooms[ai];
			restoreRealRoomsList[nRestore]=c.realRoomsList[ai];
			nRestore++;
			
			moveActivity(ai, c.times[ai], newtime, c.rooms[ai], selectedRoom[newtime], c.realRoomsList[ai], realRoomsListLevel[newtime]);
			//////////////////
			
			for(int a : qAsConst(conflActivities[newtime]))
				swappedActivities[a]=true;

			foundGoodSwap=false;
			
			ok=false;

			assert(conflActivities[newtime].count()>0);
			ok=true;
			
			for(int a : qAsConst(conflActivities[newtime])){
				randomSwap(a, level+1);
				if(!foundGoodSwap){
					ok=false;
					break;
				}
				assert(c.times[a]!=UNALLOCATED_TIME);
				assert(foundGoodSwap);
				foundGoodSwap=false;
			}
			
			if(ok){
				for(int a : qAsConst(conflActivities[newtime]))
					assert(c.times[a]!=UNALLOCATED_TIME);
				assert(c.times[ai]!=UNALLOCATED_TIME);
			
				foundGoodSwap=true;
				return;
			}

			//////////////restore times from the restore list
			for(int j=nRestore-1; j>=oldNRestore; j--){
				int aii=restoreActIndex[j];
				
				if(aii!=ai)
					swappedActivities[aii]=false;
				else
					assert(swappedActivities[aii]);
				
				moveActivity(aii, c.times[aii], restoreTime[j], c.rooms[aii], restoreRoom[j], c.realRoomsList[aii], restoreRealRoomsList[j]);
			}
			nRestore=oldNRestore;

			//////////////////////////////
			for(int a : qAsConst(conflActivities[newtime])){
				assert(c.times[a]!=UNALLOCATED_TIME);
				assert(c.rooms[a]!=UNALLOCATED_SPACE);
				//nothing to assert for realRoomsList
				assert(!swappedActivities[a]);
				assert(!(fixedTimeActivity[a] && fixedSpaceActivity[a]));
			}
			//////////////////////////////
			
			assert(!foundGoodSwap);
			
			if(level>=5) //7 might also be used? This is a value found practically.
				return;
		}
	}
}
