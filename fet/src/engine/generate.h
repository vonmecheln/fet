/*
File generate.h
*/

/***************************************************************************
                          generate.h  -  description
                             -------------------
    begin                : 2002
    copyright            : (C) 2002 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef GENERATE_H
#define GENERATE_H

#include <atomic>

#include <ctime>

#include "timetable_defs.h"
#include "solution.h"
#include "matrix.h"

#include <QObject>

#include <QTextStream>

#include <QList>
#include <QSet>

//#include <QMutex>
#include <QSemaphore>
#include <mutex>
//#include <semaphore>
//#include <condition_variable>

class Activity;

class QWidget;

/**
This class incorporates the routines for time and space allocation of activities
*/
class Generate: public QObject{
	Q_OBJECT
	
public:
#ifdef FET_COMMAND_LINE
	std::atomic<bool> writeCurrentAndHighestTimetable; //for fet-cl, we poll a file and if it exists we will write the current and highest-stage timetables.
#endif

	//QMutex myMutex;
	std::mutex myMutex;
	
	std::atomic<bool> isRunning;
	
	int nThread; //for multiple generation, otherwise it is 0.

	QSemaphore semaphorePlacedActivity;
	//std::binary_semaphore semaphorePlacedActivity;
	//std::condition_variable cvForPlacedActivity;
	//bool placedActivityProcessed;
	
	QSemaphore semaphoreFinished;

private:
	bool foundGoodSwap;
	Matrix1D<int> permutation;
	//
	//The following variables are here to accelerate the generation (so that they are not allocated more times during the generation).
	Matrix1D<int> buildings;
	Matrix1D<int> activities;
	Matrix1D<int> rooms;
	Matrix1D<int> activitiesx2;
	Matrix1D<int> roomsx2;
	Matrix1D<int> buildingsx2;
	Matrix2D<int> weekBuildings;
	Matrix2D<int> weekActivities;
	Matrix2D<int> weekRooms;
	Matrix1D<int> aminoCnt;
	Matrix1D<int> aminsCnt;
	Matrix1D<bool> possibleToEmptySlot;
	//
	Matrix1D<bool> occupiedDay;
	Matrix1D<bool> canEmptyDay;
	Matrix1D<int> _nConflActivities;
	Matrix1D<QList<int>> _activitiesForDay;
	Matrix1D<int> _minWrong;
	Matrix1D<int> _nWrong;
	Matrix1D<int> _minIndexAct;
	Matrix1D<bool> occupiedIntervalDay;
	Matrix1D<bool> canEmptyIntervalDay;
	Matrix1D<QList<int>> _activitiesForIntervalDay;
	Matrix1D<int> sbgDayNHoursWithTag;
	Matrix1D<bool> possibleToEmptyDay;
	Matrix1D<int> tchDayNHoursWithTag;
	//
	Matrix1D<bool> canEmptyTerm;
	Matrix1D<QList<int>> termActivities;
	//
	Matrix1D<bool> swappedActivities;
	Matrix1D<int> restoreActIndex;
	Matrix1D<int> restoreTime;
	Matrix1D<int> restoreRoom;
	Matrix1D<QList<int>> restoreRealRoomsList;
	Matrix1D<int> invPermutation;
	//
	//Matrix2D<double> nMinDaysBrokenL;
	Matrix2D<int> selectedRoomL;
	Matrix2D<int> permL;
	Matrix2D<QList<int>> conflActivitiesL;
	Matrix2D<int> nConflActivitiesL;
	Matrix2D<int> roomSlotsL;
	Matrix2D<QList<int>> realRoomsListL;
	//
	Matrix1D<int> slotActivity;
	//
	int currentLevel;
	//
	QSet<int> conflActivitiesSet;
	//
	int nRestore;
	int limitcallsrandomswap;
	int level_limit;
	int ncallsrandomswap;
	//
	//if level==0, choose best position with the lowest number of conflicting activities
	QList<int> conflActivitiesTimeSlot;
	int timeSlot;
	int roomSlot;
	QList<int> realRoomsSlot;
	//
	Matrix2D<int> triedRemovals;
	//
	bool impossibleActivity;
	//
	////////tabu list of tried removals (circular)
	int tabu_size;
	int crt_tabu_index;
	Matrix1D<int> tabu_activities;
	Matrix1D<int> tabu_times;
	////////////
	Matrix3D<int> teachersTimetable;
	Matrix3D<int> subgroupsTimetable;
	Matrix3D<int> roomsTimetable;
	//
	Matrix3D<int> newTeachersTimetable;
	Matrix3D<int> newSubgroupsTimetable;
	Matrix2D<int> newTeachersDayNHours;
	Matrix2D<int> newTeachersDayNGaps;
	Matrix2D<int> newSubgroupsDayNHours;
	Matrix2D<int> newSubgroupsDayNGaps;
	Matrix2D<int> newSubgroupsDayNFirstGaps;

	Matrix2D<int> newTeachersDayNFirstGaps;
	Matrix2D<int> newTeachersRealDayNHours;
	Matrix2D<int> newTeachersRealDayNGaps;
	Matrix2D<int> newSubgroupsRealDayNHours;
	Matrix2D<int> newSubgroupsRealDayNGaps;
	Matrix2D<int> newSubgroupsRealDayNFirstGaps;

	//
	Matrix3D<int> oldTeachersTimetable;
	Matrix3D<int> oldSubgroupsTimetable;
	Matrix2D<int> oldTeachersDayNHours;
	Matrix2D<int> oldTeachersDayNGaps;
	Matrix2D<int> oldSubgroupsDayNHours;
	Matrix2D<int> oldSubgroupsDayNGaps;
	Matrix2D<int> oldSubgroupsDayNFirstGaps;

	Matrix2D<int> oldTeachersDayNFirstGaps;
	Matrix2D<int> oldTeachersRealDayNHours;
	Matrix2D<int> oldTeachersRealDayNGaps;
	Matrix2D<int> oldSubgroupsRealDayNHours;
	Matrix2D<int> oldSubgroupsRealDayNGaps;
	Matrix2D<int> oldSubgroupsRealDayNFirstGaps;

	//
	Matrix2D<int> tchTimetable;
	Matrix1D<int> tchDayNHours;
	Matrix1D<int> tchDayNGaps;

	Matrix2D<int> sbgTimetable;
	Matrix1D<int> sbgDayNHours;
	Matrix1D<int> sbgDayNGaps;
	Matrix1D<int> sbgDayNFirstGaps;

	Matrix1D<int> tchDayNFirstGaps;
	Matrix1D<int> tchRealDayNHours;
	Matrix1D<int> tchRealDayNGaps;
	Matrix1D<int> sbgRealDayNHours;
	Matrix1D<int> sbgRealDayNGaps;
	Matrix1D<int> sbgRealDayNFirstGaps;

	//
	Matrix2D<QList<int>> teacherActivitiesOfTheDay;
	Matrix2D<QList<int>> subgroupActivitiesOfTheDay;

	//used at level 0
	Matrix1D<int> l0nWrong;
	Matrix1D<int> l0minWrong;
	Matrix1D<int> l0minIndexAct;

	//2011-09-25
	Matrix1D<QSet<int>> slotSetOfActivities;
	Matrix1D<bool> slotCanEmpty;

	Matrix1D<QSet<int>> activitiesAtTime;
	////////////

	int nRealRooms;
	int nSets;
	int NIL_NODE;

	Matrix1D<QList<int>> adj;
	//static Matrix1D<int> dist;
	Matrix1D<bool> visited;
	Matrix1D<int> semiRandomPermutation; //the semi-random permutation of U (the points on the left, the real rooms)
	Matrix1D<int> pairNode;
	Matrix1D<int> nConflictingActivitiesBipartiteMatching;
	Matrix1D<QSet<int>> conflictingActivitiesBipartiteMatching; //only used at level 0

	Matrix1D<int> cntTagsMatrix; //for constraints students (set)/teacher(s) max activity tags per day/real day from set.
	Matrix1D<int> _tags_minWrong;
	Matrix1D<int> _tags_nWrong;
	Matrix1D<int> _tags_nConflActivities;
	Matrix1D<int> _tags_minIndexAct;

	Solution* tmpGlobalSolutionCompareLevel0;

	bool compareConflictsIncreasing(int a, int b);
	bool compareConflictsIncreasingAtLevel0(int a, int b);

	//2019-09-15 - for virtual rooms, choosing randomly the real rooms.
	/*bool breadthFirstSearch();
	bool depthFirstSearch(int u);
	int hopcroftKarp();*/

	bool depthFirstSearch(int u);
	int maximumBipartiteMatching();

	//a probabilistic function to say if we can skip a constraint based on its percentage weight
	bool skipRandom(double weightPercentage);

	//for sorting slots in ascending order of potential conflicts
	bool compareFunctionGenerate(int i, int j);

	bool isExceptionTime(int ai, int t, const QSet<int>& ets);

#ifdef FET_COMMAND_LINE
	void checkWriteCurrentAndHighestTimetable();
#endif

public:
	MRG32k3a rng;
	
	//bool isRunning;
	
	int maxActivitiesPlaced;
	Solution highestStageSolution;

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
	
	inline void updateTeachersNHoursGapsRealDay(int ai, int real_d);
	inline void updateSubgroupsNHoursGapsRealDay(int ai, int real_d);

	inline void updateTchNHoursGaps(int tch, int d);
	inline void updateSbgNHoursGaps(int sbg, int d);
	
	inline void updateTchNHoursGapsRealDay(int tch, int real_d);
	inline void updateSbgNHoursGapsRealDay(int sbg, int real_d);

	inline void tchGetNHoursGaps(int tch);
	inline void tchGetNHoursGapsRealDays(int tch);
	inline void teacherGetNHoursGaps(int tch);
	inline bool teacherRemoveAnActivityFromBeginOrEnd(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool teacherRemoveAnActivityFromEnd(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool teacherRemoveAnActivityFromAnywhere(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool teacherRemoveAnActivityFromBeginOrEndCertainDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool teacherRemoveAnActivityFromAnywhereCertainDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);

	inline bool teacherRemoveAnActivityFromAnywhereCertainDayCertainActivityTag(int d2, int actTag, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool teacherRemoveAnActivityFromAnywhereCertainDayDayPairCertainActivityTag(int d2, int dpair2, int actTag, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);

	inline bool teacherRemoveAnActivityFromBeginOrEndCertainTwoDays(int d2, int d4, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool teacherRemoveAnActivityFromAnywhereCertainTwoDays(int d2, int d4, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);

	//constraints teachers max gaps per real day
	inline bool teacherRemoveAnActivityFromBeginMorningOrEndAfternoon(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool teacherRemoveAnActivityFromBeginOrEndCertainRealDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);

	inline void sbgGetNHoursGaps(int sbg);
	inline void sbgGetNHoursGapsRealDays(int sbg);
	inline void subgroupGetNHoursGaps(int sbg);
	inline bool subgroupRemoveAnActivityFromBegin(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromEnd(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromBeginOrEnd(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromAnywhere(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromBeginCertainDay(int d2, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromEndCertainDay(int d2, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromAnywhereCertainDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);

	//2017-02-07
	//used only in students max span per (real) day
	inline bool subgroupRemoveAnActivityFromBeginOrEndCertainDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	
	inline bool subgroupRemoveAnActivityFromAnywhereCertainDayCertainActivityTag(int d2, int actTag, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);

	//constraints students max gaps per real day
	inline bool subgroupRemoveAnActivityFromBeginMorning(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromEndAfternoon(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromBeginMorningOrEndAfternoon(int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);

	inline bool subgroupRemoveAnActivityFromBeginOrEndCertainRealDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromEndCertainRealDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	//inline bool subgroupRemoveAnActivityFromBeginCertainRealDay(int d2, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);

	inline bool subgroupRemoveAnActivityFromAnywhereCertainDayDayPairCertainActivityTag(int d2, int dpair2, int actTag, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);

	inline bool subgroupRemoveAnActivityFromBeginCertainTwoDays(int d2, int d4, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromEndCertainTwoDays(int d2, int d4, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);
	inline bool subgroupRemoveAnActivityFromAnywhereCertainTwoDays(int d2, int d4, int level, int ai, QList<int>& conflActivities, int& nConflActivities, int& removedActivity);

	inline bool checkActivitiesOccupyMaxDifferentRooms(const QList<int>& globalConflActivities, int rm, int level, int ai, QList<int>& tmp_list);
	inline bool checkActivitiesSameRoomIfConsecutive(const QList<int>& globalConflActivities, int rm, int ai, int d, int h, QList<int>& tmp_list);

	inline bool checkRoomMaxActivityTagsPerDayFromSet(const QList<int>& globalConflActivities, int rm, int level, int ai, int d, QList<int>& tmp_list);
	inline bool checkRoomMaxActivityTagsPerRealDayFromSet(const QList<int>& globalConflActivities, int rm, int level, int ai, int d, QList<int>& tmp_list);
	inline bool checkRoomMaxActivityTagsPerWeekFromSet(const QList<int>& globalConflActivities, int rm, int level, int ai, QList<int>& tmp_list);

	//only one out of sbg and tch is >=0, the other one is -1
	inline bool checkBuildingChanges(int sbg, int tch, const QList<int>& globalConflActivities, int rm, int level, const Activity* act, int ai, int d, int h, QList<int>& tmp_list);
	inline bool checkRoomChanges(int sbg, int tch, const QList<int>& globalConflActivities, int rm, int level, const Activity* act, int ai, int d, int h, QList<int>& tmp_list);

	inline bool checkBuildingChangesPerRealDay(int sbg, int tch, const QList<int>& globalConflActivities, int rm, int level, const Activity* act, int ai, int d, int h, QList<int>& tmp_list);
	inline bool checkRoomChangesPerRealDay(int sbg, int tch, const QList<int>& globalConflActivities, int rm, int level, const Activity* act, int ai, int d, int h, QList<int>& tmp_list);

	inline bool chooseRoom(const QList<int>& listOfRooms, const QList<int>& globalConflActivities, int level, const Activity* act, int ai, int d, int h, int& roomSlot, int& selectedSlot, QList<int>& localConflActivities, QList<int>& realRoomsList);
	inline bool getHomeRoom(const QList<int>& globalConflActivities, int level, const Activity* act, int ai, int d, int h, int& roomSlot, int& selectedSlot, QList<int>& localConflActivities, QList<int>& realRoomsList);
	inline bool getPreferredRoom(const QList<int>& globalConflActivities, int level, const Activity* act, int ai, int d, int h, int& roomSlot, int& selectedSlot, QList<int>& localConflActivities, bool& canBeUnspecifiedPreferredRoom, QList<int>& realRoomsList);
	inline bool getRoom(int level, const Activity* act, int ai, int d, int h, int& roomSlot, int& selectedSlot, QList<int>& conflActivities, int& nConflActivities, QList<int>& realRoomsList);

	inline bool getOptimumActivitiesToDisplace(int level, const QList<QList<int>>& activitiesList, const QList<bool>& canEmpty, QList<int>& chosenActivitiesList);

	Solution c;
	
	int nPlacedActivities;
	
	//difficult activities
	int nDifficultActivities;
	Matrix1D<int> difficultActivities;
	
	int searchTime; //seconds
	
	int timeToHighestStage; //seconds
	
	std::atomic<bool> abortOptimization;

	std::atomic<bool> restart;
	
	std::atomic<bool> paused;
	std::atomic<int> pausedTime; //seconds
	
	bool precompute(QWidget* parent, QTextStream* maxPlacedActivityStream=nullptr);
	
	void generateWithSemaphore(int maxSeconds, bool& restarted, bool& impossible, bool& timeExceeded, bool threaded, QTextStream* maxPlacedActivityStream=nullptr);
	void generate(int maxSeconds, bool& restarted, bool& impossible, bool& timeExceeded, bool threaded, QTextStream* maxPlacedActivityStream=nullptr);
	
	void moveActivity(int ai, int fromslot, int toslot, int fromroom, int toroom, const QList<int>& fromRealRoomsList, const QList<int>& toRealRoomsList);
	
	void randomSwap(int ai, int level);
	
Q_SIGNALS:
	void activityPlaced(int, int);
	
	void generationFinished();
	
	void impossibleToSolve();
	
private:
	bool isThreaded;
	
	int currentlyNPlacedActivities;
	time_t starting_time;
	
	int activityRetryLevel0TimeLimit; //seconds
	bool activityRetryLevel0TimeExceeded;
};

#endif
