//
//
// C++ Interface: $MODULE$
//
// Description: 
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BUILDING_H
#define BUILDING_H

#include <QCoreApplication>

#include "timetable_defs.h"

#include <QString>
#include <QList>
#include <QStringList>

class Building;
class Rules;

typedef QList<Building*> BuildingsList;

/**
This class represents a building

@author Liviu Lalescu
*/
class Building{
	Q_DECLARE_TR_FUNCTIONS(Building)

public:
	QString name;

	Building();
	~Building();
	
	void computeInternalStructure(Rules& r);
	
	QString getXmlDescription();
	QString getDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

int buildingsAscending(const Building* b1, const Building* b2);

#endif
