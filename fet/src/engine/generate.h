/*
File generate.h
*/

/***************************************************************************
                          generate.h  -  description
                             -------------------
    begin                : 2002
    copyright            : (C) 2002 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef GENERATE_H
#define GENERATE_H

#include "timetable_defs.h"
#include "solution.h"

#include <QObject>

#include <QTextStream>

#include <QList>

class Activity;

class QWidget;

//2019-09-15 - for virtual rooms, choosing randomly the real rooms.
/*bool breadthFirstSearch();
bool depthFirstSearch(int u);
int hopcroftKarp();*/

bool compareConflictsIncreasing(const int& a, const int& b);
bool compareConflictsIncreasingAtLevel0(const int& a, const int& b);
bool depthFirstSearch(int u);
int maximumBipartiteMatching();

//a probabilistic function to say if we can skip a constraint based on its percentage weight
bool skipRandom(double weightPercentage);

//for sorting slots in ascending order of potential conflicts
bool compareFunctionGenerate(int i, int j);

/**
This class incorporates the routines for time and space allocation of activities
*/
class Generate: public QObject{
	Q_OBJECT

public:
	Generate();
	~Generate();
	
	inline void addAiToNewTimetable(int ai, const Activity* act, int d, int h);
	inline void removeAiFromNewTimetable(int ai, const Activity* act, int d, int h);
	
	inline void getTchTimetable(int tch, const QList<int>& conflActivities);
	inline void getSbgTimetable(int sbg, const QList<int>& conflActivities);
	
	inline void removeAi2FromTchTimetable(int ai2);
	inline void removeAi2FromSbgTimetable(int ai2);

	inline void updateTeachersNHoursGaps(int ai, int d);
	inline void updateSubgroupsNHoursGaps(int ai, int d);
	
	inline void updateTchNHoursGaps(int tch, int d);
	inline void updateSbgNHoursGaps(int sbg, int d);
	
	inline void tchGetNHoursGaps(int tch);
	inline void teacherGetNHoursGaps(int tch);
	inline bool teacherRemoveAnActivityFromBeginOrEnd(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool teacherRemoveAnActivityFromAnywhere(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool teacherRemoveAnActivityFromBeginOrEndCertainDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool teacherRemoveAnActivityFromAnywhereCertainDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);

	inline bool teacherRemoveAnActivityFromAnywhereCertainDayCertainActivityTag(int d2, int actTag, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);

	inline void sbgGetNHoursGaps(int sbg);
	inline void subgroupGetNHoursGaps(int sbg);
	inline bool subgroupRemoveAnActivityFromBegin(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromEnd(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromBeginOrEnd(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromAnywhere(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromBeginCertainDay(int d2, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromEndCertainDay(int d2, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromAnywhereCertainDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);

	//2017-02-07
	//used only in students max span per day
	inline bool subgroupRemoveAnActivityFromBeginOrEndCertainDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	
	inline bool subgroupRemoveAnActivityFromAnywhereCertainDayCertainActivityTag(int d2, int actTag, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	
	inline bool checkActivitiesOccupyMaxDifferentRooms(const QList<int>& globalConflActivities, int rm, int level, int ai, QList<int>& tmp_list);
	inline bool checkActivitiesSameRoomIfConsecutive(const QList<int>& globalConflActivities, int rm, int ai, int d, int h, QList<int>& tmp_list);

	//only one out of sbg and tch is >=0, the other one is -1
	inline bool checkBuildingChanges(int sbg, int tch, const QList<int>& globalConflActivities, int rm, int level, const Activity* act, int ai, int d, int h, QList<int>& tmp_list);
	inline bool checkRoomChanges(int sbg, int tch, const QList<int>& globalConflActivities, int rm, int level, const Activity* act, int ai, int d, int h, QList<int>& tmp_list);

	inline bool chooseRoom(const QList<int>& listOfRooms, const QList<int>& globalConflActivities, int level, const Activity* act, int ai, int d, int h, int& roomSlot, int& selectedSlot, QList<int>& localConflActivities, QList<int>& realRoomsList);
	inline bool getHomeRoom(const QList<int>& globalConflActivities, int level, const Activity* act, int ai, int d, int h, int& roomSlot, int& selectedSlot, QList<int>& localConflActivities, QList<int>& realRoomsList);
	inline bool getPreferredRoom(const QList<int>& globalConflActivities, int level, const Activity* act, int ai, int d, int h, int& roomSlot, int& selectedSlot, QList<int>& localConflActivities, bool& canBeUnspecifiedPreferredRoom, QList<int>& realRoomsList);
	inline bool getRoom(int level, const Activity* act, int ai, int d, int h, int& roomSlot, int& selectedSlot, QList<int>& conflActivities, int& nConflActivities, QList<int>& realRoomsList);

	Solution c;
	
	int nPlacedActivities;
	
	//difficult activities
	int nDifficultActivities;
	int difficultActivities[MAX_ACTIVITIES];
	
	int searchTime; //seconds
	
	int timeToHighestStage; //seconds
	
	bool abortOptimization;
	
	bool precompute(QWidget* parent, QTextStream* maxPlacedActivityStream=nullptr);
	
	void generate(int maxSeconds, bool& impossible, bool& timeExceeded, bool threaded, QTextStream* maxPlacedActivityStream=nullptr);
	
	void moveActivity(int ai, int fromslot, int toslot, int fromroom, int toroom, const QList<int>& fromRealRoomsList, const QList<int>& toRealRoomsList);
	
	void randomSwap(int ai, int level);
	
signals:
	void activityPlaced(int);
	
	void simulationFinished();
	
	void impossibleToSolve();
	
private:
	bool isThreaded;
};

#endif
