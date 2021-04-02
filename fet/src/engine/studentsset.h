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
#ifndef STUDENTSSET_H
#define STUDENTSSET_H

#include "genetictimetable_defs.h"

#include <QList>

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

/**
This class represents a set of students, for instance years, groups or subgroups.

@author Liviu Lalescu
*/
class StudentsSet
{
public:
	QString name;
	int numberOfStudents;
	int type;

	StudentsSet();
	~StudentsSet();
};

class StudentsYear: public StudentsSet
{
public:
	StudentsGroupsList groupsList;

	StudentsYear();
	~StudentsYear();

	QString getXmlDescription();
	QString getDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

class StudentsGroup: public StudentsSet
{
public:
	StudentsSubgroupsList subgroupsList;

	StudentsGroup();
	~StudentsGroup();

	QString getXmlDescription();
	QString getDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

class StudentsSubgroup: public StudentsSet
{
public:
	StudentsSubgroup();
	~StudentsSubgroup();

	QString getXmlDescription();
	QString getDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

int yearsAscending(const StudentsYear* y1, const StudentsYear* y2);

int groupsAscending(const StudentsGroup* g1, const StudentsGroup* g2);

int subgroupsAscending(const StudentsSubgroup* s1, const StudentsSubgroup* s2);

#endif
