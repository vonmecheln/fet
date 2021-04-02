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
#ifndef ACTIVITYTAG_H
#define ACTIVITYTAG_H

#include "timetable_defs.h"

#include <qstring.h>
#include <q3ptrlist.h>

class ActivityTag;
class Rules;

typedef QList<ActivityTag*> ActivityTagsList;

/**
This class represents an activity tag

@author Liviu Lalescu
*/
class ActivityTag{
public:
	QString name;

	ActivityTag();
	~ActivityTag();

	QString getXmlDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

int activityTagsAscending(const ActivityTag* st1, const ActivityTag* st2);

#endif
