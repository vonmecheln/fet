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

#ifndef STUDENTSSET_H
#define STUDENTSSET_H

#include <QCoreApplication>

#include "timetable_defs.h"

#include <QList>
#include <QString>
#include <QStringList>

//If you change any of these const int-s, you need to update the const QString FET_DATA_FORMAT_VERSION from timetable_defs.cpp to a new value,
//because of the disk history feature.
const int STUDENTS_SET=0;
const int STUDENTS_YEAR=1;
const int STUDENTS_GROUP=2;
const int STUDENTS_SUBGROUP=3;

class StudentsYear;
class StudentsGroup;
class StudentsSubgroup;

class Rules;

typedef QList<StudentsYear*> StudentsYearsList;

typedef QList<StudentsGroup*> StudentsGroupsList;

typedef QList<StudentsSubgroup*> StudentsSubgroupsList;

class QDataStream;

/*QDataStream& operator<<(QDataStream& stream, const StudentsYear& year);
QDataStream& operator>>(QDataStream& stream, StudentsYear& year);

QDataStream& operator<<(QDataStream& stream, const StudentsGroup& group);
QDataStream& operator>>(QDataStream& stream, StudentsGroup& group);

QDataStream& operator<<(QDataStream& stream, const StudentsSubgroup& subgroup);
QDataStream& operator>>(QDataStream& stream, StudentsSubgroup& subgroup);*/

/**
This class represents a set of students, for instance years, groups or subgroups.

@author Liviu Lalescu
*/
class StudentsSet
{
	Q_DECLARE_TR_FUNCTIONS(StudentsSet)

public:
	QString name;
	QString longName;
	QString code;
	int numberOfStudents;
	int type;
	
	QString comments;
	
	StudentsSet();
	virtual ~StudentsSet();
};

class StudentsYear: public StudentsSet
{
	Q_DECLARE_TR_FUNCTIONS(StudentsYear)
	
public:
	StudentsGroupsList groupsList;

	bool firstCategoryIsPermanent;

	QList<QStringList> divisions;
	QString separator; //The separator when dividing a year by categories.

	int indexInAugmentedYearsList; //internal

	StudentsYear();
	~StudentsYear();

	QString getXmlDescription();
	QString getDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

class StudentsGroup: public StudentsSet
{
	Q_DECLARE_TR_FUNCTIONS(StudentsGroup)

public:
	StudentsSubgroupsList subgroupsList;

	int indexInInternalGroupsList; //internal
	
	StudentsGroup();
	~StudentsGroup();

	QString getXmlDescription();
	QString getDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

class StudentsSubgroup: public StudentsSet
{
	Q_DECLARE_TR_FUNCTIONS(StudentsSubgroup)

public:
	int indexInInternalSubgroupsList;
	
	QList<int> activitiesForSubgroup;

	StudentsSubgroup();
	~StudentsSubgroup();

	QString getXmlDescription();
	QString getDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

int studentsSetsAscending(const StudentsSet* st1, const StudentsSet* st2);

int studentsSetsCodesAscending(const StudentsSet* st1, const StudentsSet* st2);

int yearsAscending(const StudentsYear* y1, const StudentsYear* y2);

int groupsAscending(const StudentsGroup* g1, const StudentsGroup* g2);

int subgroupsAscending(const StudentsSubgroup* s1, const StudentsSubgroup* s2);

#endif
