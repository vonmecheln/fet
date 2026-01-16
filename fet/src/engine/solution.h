/*
File solution.h
*/

/***************************************************************************
                          solution.h  -  description
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

#ifndef SOLUTION_H
#define SOLUTION_H

#include "timetable_defs.h"

#include "matrix.h"

#include <QString>
#include <QList>
#include <QTextStream>

class Rules;

class FakeString;

/**
This class represents a solution (time and space allocation for the activities).
*/
class Solution{
public:
	QList<double> conflictsWeightList;
	QList<QString> conflictsDescriptionList;
	double conflictsTotal;
	
	bool teachersMatrixReady;
	bool subgroupsMatrixReady;
	bool roomsMatrixReady;
	
	int nPlacedActivities;

	/*
	You will need to set this to true if altering the times array values.
	The conflicts calculating routine will reset this to false
	at the first teachers matrix and subgroups matrix calculation.
	*/
	bool changedForMatrixCalculation;

	/**
	This array represents every activity's start time
	(time is a unified representation of hour and day,
	stored as an integer value). We have a special value here:
	UNALLOCATED_TIME, which is a large number.
	*/
	Matrix1D<int> times;
	
	Matrix1D<int> rooms;
	
	Matrix1D<QList<int>> realRoomsList; //for virtual rooms

	/**
	Fitness; it is calculated only at the initialization or
	at the modification.
	Important assumption: the rules have to ramain the same;
	otherwise the user has to reset this value to -1
	*/
	double _fitness;

	/**
	Assignment method. We need to have access to the Rules instantiation
	to know the number of activities.
	*/
	void copy(Rules& r, Solution& c);
	
	//Used only in timetable generate multiple highest stage (incomplete) solutions.
	void copyForHighestStage(Rules& r, Solution& c);

	/**
	Initializes, marking all activities as unscheduled (time)
	*/
	//void init(Rules& r);

	/**
	Marks the starting time and rooms of all the activities
	as unallocated.
	*/
	void makeUnallocated(Rules& r);

	/**
	ATTENTION: if the rules change, the user has to reset _fitness to -1
	<p>
	If conflictsString is not nullptr, then this function will
	append at this string an explanation of the conflicts.
	*/
	double fitness(Rules& r, FakeString* conflictsString=nullptr);

	void getTeachersTimetable(Rules& r, Matrix3D<int>& a, Matrix3D<QList<int>>& b);

	void getSubgroupsTimetable(Rules& r, Matrix3D<int>& a);

	void getRoomsTimetable(Rules& r, Matrix3D<int>& a, Matrix3D<QList<int>>& va);
	
	void getBuildingsTimetable(Rules& r, Matrix3D<QList<int>>& a);

	int getSubgroupsMatrix(Rules& r, Matrix3D<int>& a);
	//return value is the number of conflicts, which must be 0

	int getTeachersMatrix(Rules& r, Matrix3D<int>& a);
	//return value is the number of conflicts, which must be 0

	int getRoomsMatrix(Rules& r, Matrix3D<int>& a);
	//return value is the number of conflicts, which must be 0
};

#endif
