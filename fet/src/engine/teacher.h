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
#ifndef TEACHER_H
#define TEACHER_H

#include <QCoreApplication>

#include "timetable_defs.h"

#include <qstring.h>
#include <q3ptrlist.h>

class Teacher;
class Rules;

#include <QList>

typedef QList<Teacher*> TeachersList;

/**
@author Liviu Lalescu
*/
class Teacher
{
	Q_DECLARE_TR_FUNCTIONS(Teacher)
	
public:
	QList<qint16> activitiesForTeacher;

	QString name;

	Teacher();
	~Teacher();

	QString getXmlDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

int teachersAscending(const Teacher* t1, const Teacher* t2);

#endif
