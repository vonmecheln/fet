//
//
// C++ Interface: $MODULE$
//
// Description: 
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SUBJECT_H
#define SUBJECT_H

#include "genetictimetable_defs.h"

#include <QList>
#include <QString>

class Subject;
class Rules;

typedef QList<Subject*> SubjectsList;

/**
This class represents a subject

@author Liviu Lalescu
*/
class Subject{
public:
	QString name;

	Subject();
	~Subject();

	QString getXmlDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

int subjectsAscending(const Subject* s1, const Subject* s2);

#endif