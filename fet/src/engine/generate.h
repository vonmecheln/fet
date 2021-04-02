/*
File generate.h
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

#ifndef GENERATE_H
#define GENERATE_H

#include "timetable_defs.h"
#include "solution.h"

//a probabilistic function to say if we can skip a constraint based on its percentage weight
inline bool skipRandom(double weightPercentage);

/**
This class represents the solving of time
*/
class Generate: public QObject{
	Q_OBJECT

public:
	Generate();
	~Generate();

	Solution c;
	
	int nPlacedActivities;
	
	//difficult activities
	int nDifficultActivities;
	int difficultActivities[MAX_ACTIVITIES];
	
	int searchTime; //seconds
	
	bool abortOptimization;
	
	bool precompute();
	
	void generate(int maxSeconds, bool& impossible, bool& timeExceeded/*, const bool semaphorePlacedActivity*/);
	
	void moveActivity(int ai, int fromslot, int toslot, int fromroom, int toroom);
	
	void randomswap(int ai, int level);
	
signals:
	void activityPlaced(int);
	
	void simulationFinished();
	
	void impossibleToSolve();
};

#endif
