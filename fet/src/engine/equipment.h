//
//
// C++ Interface: $MODULE$
//
// Description: 
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "genetictimetable_defs.h"

#include <QString>
#include <QList>
#include <QStringList>

class Equipment;
class Rules;

typedef QList<Equipment*> EquipmentsList;

/**
This class represents an equipment
*/
class Equipment
{
public:
	QString name;

	Equipment();
	~Equipment();

	QString getXmlDescription();
	QString getDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

int equipmentsAscending(const Equipment* e1, const Equipment* e2);

#endif
