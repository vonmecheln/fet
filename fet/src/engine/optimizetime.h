/*
File optimizetime.h
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

#ifndef OPTIMIZETIME_H
#define OPTIMIZETIME_H

#include "genetictimetable_defs.h"
#include "timechromosome.h"

//a probabilistic function to say if we can skip a constraint based on its percentage weight
inline bool skipRandom(int weightPercentage);

/**
This class represents the solving of time
*/
class OptimizeTime: public QObject{
	Q_OBJECT

public:
	OptimizeTime();
	~OptimizeTime();

	TimeChromosome c;
	
	int nPlacedActivities;
	
	//difficult activities
	int nDifficultActivities;
	int difficultActivities[MAX_ACTIVITIES];
	
	int searchTime; //seconds
	
	bool abortOptimization;
	
	bool precompute();
	
	void optimize();
	
	//bool timesUp();
	
	////////////////////
	
	void moveActivity(int ai, int fromslot, int toslot);
	
	void randomswap(int ai, int level);
	
signals:
	void activityPlaced(int);
	
	void simulationFinished();
	
	void impossibleToSolve();
};

#endif
