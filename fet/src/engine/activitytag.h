//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2005 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef ACTIVITYTAG_H
#define ACTIVITYTAG_H

#include <QCoreApplication>

#include "timetable_defs.h"

#include <QString>
#include <QList>

class ActivityTag;
class Rules;

typedef QList<ActivityTag*> ActivityTagsList;

class QDataStream;

QDataStream& operator<<(QDataStream& stream, const ActivityTag& at);
QDataStream& operator>>(QDataStream& stream, ActivityTag& at);

/**
This class represents an activity tag

@author Liviu Lalescu
*/
class ActivityTag{
	Q_DECLARE_TR_FUNCTIONS(ActivityTag)

public:
	QString name; //short name, unique
	
	QString longName;
	QString code;
	
	bool printable;

	QString comments;
	
	ActivityTag();
	~ActivityTag();

	QString getXmlDescription();
	QString getDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

int activityTagsAscending(const ActivityTag* st1, const ActivityTag* st2);

int activityTagsCodesAscending(const ActivityTag* st1, const ActivityTag* st2);

#endif
