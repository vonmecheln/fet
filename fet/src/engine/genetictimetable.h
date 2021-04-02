/*
File genetictimetable.h
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

#ifndef GENETICTIMETABLE_H
#define GENETICTIMETABLE_H

#include "genetictimetable_defs.h"
#include "rules.h"
#include "timepopulation.h"
#include "spacepopulation.h"
#include "timespacepopulation.h"

/**
This class represents the main interface for the library.
*/
class GeneticTimetable{
public:
	/**
	The set of rules for the genetic timetable (the main input).
	*/
	Rules rules;

	/**
	The population of chromosomes.
	(The internal state).
	The part referring to time.
	*/
	TimePopulation timePopulation;

	/**
	The population of chromosomes.
	(The internal state).
	The part referring to space (rooms).
	*/
	SpacePopulation spacePopulation;
	
	TimeSpacePopulation timeSpacePopulation;
};

#endif
