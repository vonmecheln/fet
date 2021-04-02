/*
File spacepopulation.h
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

#ifndef SPACEPOPULATION_H
#define SPACEPOPULATION_H

#include "genetictimetable_defs.h"
#include "spacechromosome.h"
#include "timechromosome.h"

#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>

/**
This class represents a current population of chromosomes
*/
class SpacePopulation{
public:
	/**
	The population number, n<MAX_POPULATION_SIZE
	*/
	int n;

	/**
	This is a boolean variable that indicates whether the population
	has been initialized, randomly or by setting "UNALLOCATED_SPACE" all the
	activities in all the chromosomes. Also, for initialization,
	it is needed that a TimeChromosome is given, which is the solution 
	to the time problem (I guess it must have no unallocated activities
	and conflicts, but it might operate on those as well)
	*/
	bool initialized;

	/**
	The array of chromosomes
	We need twice the space of the maximum population (need the extra
	room for generating new individuals)
	*/
	SpaceChromosome chromosomes[MAX_POPULATION_SIZE * 2];

	/**
	The scheduled days of the activities (remember that this is the second
	phase, and we must already have the activities scheduled)
	*/	
	int days[MAX_ACTIVITIES];
	
	/**
	The scheduled hours of the activities (remember that this is the second
	phase, and we must already have the activities scheduled)
	*/
	int hours[MAX_ACTIVITIES];
	
private:
	/**
	This boolean variable is true if the population is sorted in
	increasing fitness factor order
	*/
	bool sorted;

	/**
	This boolean variable is true if the first chromosome is the best
	*/
	bool bestFirst;
	
	/**
	Sorts the chromosomes increasingly according to hard fitness 
	(if equality, then to soft fitness)
	*/
	void sort(Rules& ru, int l, int r);

public:
	/**
	Constructor.
	*/
	SpacePopulation();

	/**
	Returns the number of chromosomes
	*/
	int populationNumber();

	/**
	Initializes the population internal variables and all the chromosomes.
	It calls the method SpaceChromosome::init for every chromosome.
	It initializes the starting day and hour of every activity
	(arrays "days" and "hours") from the values of the timeChromosome.
	*/
	void init(Rules& r, int np, TimeChromosome& timeChromosome);

	/**
	Invokes method SpaceChromosome::makeRoomsUnallocated for all the chromosomes
	*/
	void makeRoomsUnallocated(Rules& r);

	/**
	Invokes method SpaceChromosome::makeRoomsRandom for all the chromosomes
	*/
	void makeRoomsRandom(Rules& r);

	/**
	Returns the individual with the best fitness (the smallest fitness factor)
	*/
	SpaceChromosome& bestChromosome(Rules& r);

	/**
	Returns the individual with the worst fitness (the biggest fitness factor)
	For statistical purposes...
	*/
	SpaceChromosome& worstChromosome(Rules& r);

	/**
	Returns the median chromosome (n/2-1'st)
	For statistical purposes...
	*/
	SpaceChromosome& medianChromosome(Rules& r);

	/**
	For statistical purposes...
	*/
	double totalHardFitness(Rules& r);

	/**
	For statistical purposes...
	*/
	double totalSoftFitness(Rules& r);

	/**
	This function evolves from the current generation to the next generation
	My (simplified) version of evolution: we generate a complete population 
	(we will have 2*n individuals),	then we have to kill half of the 
	population, the least evoluted (of course sorting is necessary prior to
	this second phase)
	*/
	void evolve1(Rules& r); 

	/**
	This function evolves from the current generation to the next generation
	The variant I read in
	Ho Sung C. Lee - Timetabling High Constrained System via Genetic Algorithms
	<p>
	The population is changed without elitism (although I keep the best chromosome 
	from the population)	
	Selection is based on 3 tournament.
	*/
	void evolve2(Rules& r);

	/**
	Reads the population saved in this stream (to allow resuming).
	*/
	void read(Rules& r, QTextStream& tis);

	/**
	Reads the population saved in this file (to allow resuming).
	Returns false if the file does not exist.
	*/
	bool read(Rules& r, const QString& filename);

	/**
	Saves the current population in this stream, to allow
	resuming later
	*/
	void write(Rules& r, QTextStream& tos);

	/**
	Saves the current population in this file, to allow
	resuming later
	*/
	void write(Rules& r, const QString& filename);
};

#endif
