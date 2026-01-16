//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef TEACHER_H
#define TEACHER_H

#include <QCoreApplication>

#include "timetable_defs.h"

#include <QString>
#include <QList>
#include <QHash>

#include <list>

class Teacher;
class Rules;

typedef QList<Teacher*> TeachersList;

//If you change any of these const int-s, you need to update the const QString FET_DATA_FORMAT_VERSION from timetable_defs.cpp to a new value,
//because of the disk history feature.
const int TEACHER_MORNINGS_AFTERNOONS_BEHAVIOR_NOT_INITIALIZED=0;
const int TEACHER_UNRESTRICTED_MORNINGS_AFTERNOONS=1; //normally used in Algeria.
const int TEACHER_MORNING_OR_EXCLUSIVELY_AFTERNOON=2; //normally used in Morocco.
const int TEACHER_ONE_DAY_EXCEPTION=3;
const int TEACHER_TWO_DAYS_EXCEPTION=4;
const int TEACHER_THREE_DAYS_EXCEPTION=5;
const int TEACHER_FOUR_DAYS_EXCEPTION=6;
const int TEACHER_FIVE_DAYS_EXCEPTION=7;

class QDataStream;

QDataStream& operator<<(QDataStream& stream, const Teacher& tch);
QDataStream& operator>>(QDataStream& stream, Teacher& tch);

/**
@author Liviu Lalescu
*/
class Teacher
{
	Q_DECLARE_TR_FUNCTIONS(Teacher)
	
public:
	QList<int> activitiesForTeacher;

	QString name;
	QString longName;
	QString code;

	int morningsAfternoonsBehavior;
	
	QString comments;
	
	int targetNumberOfHours;
	
	std::list<QString> qualifiedSubjectsList;
	QHash<QString, std::list<QString>::iterator> qualifiedSubjectsHash; //index in the above list, useful when removing/renaming subjects

	Teacher();
	~Teacher();

	QString getXmlDescription(const Rules& r);
	QString getDescription(const Rules& r);
	QString getDetailedDescription(const Rules& r);
	QString getDetailedDescriptionWithNumberOfActiveHours(const Rules& r, const QHash<QString, int>& activeHoursHash);
	QString getDetailedDescriptionWithConstraints(Rules& r);
	QString getDetailedDescriptionWithConstraintsAndNumberOfActiveHours(Rules& r, const QHash<QString, int>& activeHoursHash);

	void recomputeQualifiedSubjectsHash();
};

int teachersAscending(const Teacher* t1, const Teacher* t2);

int teachersCodesAscending(const Teacher* t1, const Teacher* t2);

#endif
