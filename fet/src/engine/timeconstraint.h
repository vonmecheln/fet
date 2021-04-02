/*
File timeconstraint.h
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

#ifndef TIMECONSTRAINT_H
#define TIMECONSTRAINT_H

#include "timetable_defs.h"

#include <QString>
#include <QList>
#include <QStringList>

class Rules;
class Solution;
class TimeConstraint;
class Activity;
class Teacher;
class Subject;
class SubjectTag;
class StudentsSet;

typedef QList<TimeConstraint*> TimeConstraintsList;

const int CONSTRAINT_GENERIC_TIME										=0;

const int CONSTRAINT_BASIC_COMPULSORY_TIME								=1;
const int CONSTRAINT_BREAK												=2;

const int CONSTRAINT_TEACHER_NOT_AVAILABLE								=3;
const int CONSTRAINT_TEACHERS_MAX_HOURS_DAILY							=4;
const int CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK							=5;
const int CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK							=6;
const int CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK							=7;
const int CONSTRAINT_TEACHER_MAX_HOURS_DAILY							=8;

const int CONSTRAINT_STUDENTS_EARLY										=9;
const int CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE							=10;
const int CONSTRAINT_STUDENTS_NO_GAPS									=11;
const int CONSTRAINT_STUDENTS_SET_NO_GAPS								=12;
const int CONSTRAINT_STUDENTS_SET_EARLY									=13;
const int CONSTRAINT_STUDENTS_MAX_HOURS_DAILY							=14;
const int CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY						=15;
const int CONSTRAINT_STUDENTS_MIN_HOURS_DAILY							=16;
const int CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY						=17;

const int CONSTRAINT_ACTIVITY_PREFERRED_TIME							=18;
const int CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME						=19;
const int CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING							=20;
const int CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES						=21;
const int CONSTRAINT_ACTIVITY_PREFERRED_TIMES							=22;
const int CONSTRAINT_ACTIVITIES_PREFERRED_TIMES							=23;
const int CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR						=24;
const int CONSTRAINT_2_ACTIVITIES_CONSECUTIVE							=25;

const int CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY							=26;

const int CONSTRAINT_TEACHERS_MIN_HOURS_DAILY							=27;
const int CONSTRAINT_TEACHER_MIN_HOURS_DAILY							=28;

/**
This class represents a time constraint
*/
class TimeConstraint{
public:
	/**
	The percentage weight of this constraint, 100% compulsory, 0% non-compulsory
	*/
	double weightPercentage;

	/**
	Specifies the type of this constraint (using the above constants).
	*/
	int type;

	/**
	True for mandatory constraints, false for non-mandatory constraints.
	*/
	//bool compulsory;

	/**
	Dummy constructor - needed for the static array of constraints.
	Any other use should be avoided.
	*/
	TimeConstraint();
	
	virtual ~TimeConstraint()=0;

	/**
	DEPRECATED COMMENT
	Constructor - please note that the maximum allowed weight is 100.0
	The reason: unallocated activities must have very big conflict weight,
	and any other restrictions must have much more lower weight,
	so that the timetable can evolve when starting with uninitialized activities.
	*/
	TimeConstraint(double wp);

	/**
	The function that calculates the fitness of a chromosome, according to this
	constraint. We need the rules to compute this fitness factor.
	If conflictsString!=NULL,
	it will be initialized with a text explaining where this restriction is broken.
	*/
	virtual double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL)=0;

	/**
	Returns an XML description of this constraint
	*/
	virtual QString getXmlDescription(Rules& r)=0;

	/**
	Computes the internal structure for this constraint.
	
	It returns false if the constraint is an activity related
	one and it depends on only inactive activities.
	*/
	virtual bool computeInternalStructure(Rules& r)=0;

	/**
	Returns a small description string for this constraint
	*/
	virtual QString getDescription(Rules& r)=0;

	/**
	Returns a detailed description string for this constraint
	*/
	virtual QString getDetailedDescription(Rules& r)=0;
	
	/**
	Returns true if this constraint is related to this activity
	*/
	virtual bool isRelatedToActivity(Activity* a)=0;

	/**
	Returns true if this constraint is related to this teacher
	*/
	virtual bool isRelatedToTeacher(Teacher* t)=0;

	/**
	Returns true if this constraint is related to this subject
	*/
	virtual bool isRelatedToSubject(Subject* s)=0;

	/**
	Returns true if this constraint is related to this subject tag
	*/
	virtual bool isRelatedToSubjectTag(SubjectTag* s)=0;

	/**
	Returns true if this constraint is related to this students set
	*/
	virtual bool isRelatedToStudentsSet(Rules& r, StudentsSet* s)=0;
};

/**
This class comprises all the basic compulsory constraints (constraints
which must be fulfilled for any timetable) - the time allocation part
*/
class ConstraintBasicCompulsoryTime: public TimeConstraint{
public:
	ConstraintBasicCompulsoryTime();

	ConstraintBasicCompulsoryTime(double wp);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

/**
This is a custom constraint.
It returns a fitness factor a number equal
to the product of this restriction's weight and
the number of conflicting hours for each teacher
(hours when he is not available, but a course is scheduled at that time).
For the moment, this is done for a certain day and an hour interval.
(For teacher "teacherName", on day "d", between hours "h1" and "h2").
*/
class ConstraintTeacherNotAvailable: public TimeConstraint{
public:

	/**
	The day
	*/
	int d;

	/**
	The start hour
	*/
	int h1;

	/**
	The end hour
	*/
	int h2;

	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherNotAvailable();

	ConstraintTeacherNotAvailable(double wp, const QString& tn, int day, int start_hour, int end_hour);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

/**
This is a custom constraint. It returns a fitness factor equal to the product of this
restriction's weight and the number of conflicting hours for each students' set
(hours when it is not available, but a course is scheduled at that time).
For the moment, this is done for a certain day and an hour interval.
(on day "d", between hours "h1" and "h2").
*/
class ConstraintStudentsSetNotAvailable: public TimeConstraint{
public:
	/**
	The day
	*/
	int d;

	/**
	The start hour
	*/
	int h1;

	/**
	The end hour
	*/
	int h2;

	/**
	The name of the students
	*/
	QString students;

	/**
	The number of subgroups involved in this restriction
	*/
	int nSubgroups;

	/**
	The subgroups involved in this restriction
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;

	ConstraintStudentsSetNotAvailable();

	ConstraintStudentsSetNotAvailable(double wp, const QString& sn, int day, int start_hour, int end_hour);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

/**
DEPRECATED COMMENT
This is a constraint.
It aims at scheduling a set of activities at the same starting time.
The number of conflicts is considered the sum of differences
in the scheduling time for all pairs of activities.
The difference in the scheduling time for a pair of
activities is considered the sum between the difference in the starting
day and the difference in the starting hour.
TODO: Weekly activities are counted as two and fortnightly activities as one
(really necessary?).
IMPORTANT: Starting with version 3.2.3, the compulsory constraints of this kind
implement chromosome repairing, so no conflicts will be reported
*/
class ConstraintActivitiesSameStartingTime: public TimeConstraint{
public:
	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id)
	*/
	int activitiesId[MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME];

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (indexes in the rules) - internal structure
	*/
	int _activities[MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME];

	ConstraintActivitiesSameStartingTime();

	/**
	Constructor, using:
	the weight, the number of activities and the list of activities' id-s.
	*/
	ConstraintActivitiesSameStartingTime(double wp, int n_act, const int act[]);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	/**
	Removes useless activities from the _activities and activitiesId array
	*/
	void removeUseless(Rules& r);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

/**
This is a constraint.
It aims at scheduling a set of activities so that they do not overlap.
The number of conflicts is considered the number of overlapping
hours.
*/
class ConstraintActivitiesNotOverlapping: public TimeConstraint{
public:
	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id)
	*/
	int activitiesId[MAX_CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING];

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (index in the rules) - internal structure
	*/
	int _activities[MAX_CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING];

	ConstraintActivitiesNotOverlapping();

	/**
	Constructor, using:
	the weight, the number of activities and the list of activities.
	*/
	ConstraintActivitiesNotOverlapping(double wp, int n_act, const int act[]);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	/**
	Removes useless activities from the _activities array
	*/
	void removeUseless(Rules &r);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

/**
This is a constraint.
It aims at scheduling a set of activities so that they
have a minimum of N days between any two of them.
*/
class ConstraintMinNDaysBetweenActivities: public TimeConstraint{
public:
	bool consecutiveIfSameDay;

	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id)
	*/
	int activitiesId[MAX_CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES];

	/**
	The number of minimum days between each 2 activities
	*/
	int minDays;

	//internal structure (redundant)

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (index in the rules) - internal structure
	*/
	int _activities[MAX_CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES];

	ConstraintMinNDaysBetweenActivities();

	/**
	Constructor, using:
	the weight, the number of activities and the list of activities.
	*/
	ConstraintMinNDaysBetweenActivities(double wp, bool adjacentIfBroken, int n_act, const int act[], int n);

	/**
	Comparison operator - to be sure that we do not introduce duplicates
	*/
	bool operator==(ConstraintMinNDaysBetweenActivities& c);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	/**
	Removes useless activities from the _activities array
	*/
	void removeUseless(Rules &r);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

/**
This is a constraint, aimed at obtaining timetables
which do not allow more than X hours in a day for any teacher
*/
class ConstraintTeachersMaxHoursDaily: public TimeConstraint{
public:
	/**
	The maximum hours daily
	*/
	int maxHoursDaily;

	ConstraintTeachersMaxHoursDaily();

	ConstraintTeachersMaxHoursDaily(double wp, int maxhours);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

class ConstraintTeacherMaxHoursDaily: public TimeConstraint{
public:
	/**
	The maximum hours daily
	*/
	int maxHoursDaily;
	
	QString teacherName;
	
	int teacher_ID;

	ConstraintTeacherMaxHoursDaily();

	ConstraintTeacherMaxHoursDaily(double wp, int maxhours, const QString& teacher);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

/**
This is a constraint.
The resulting timetable must respect the requirement
that this teacher must not have too much working
days per week.
*/
class ConstraintTeacherMaxDaysPerWeek: public TimeConstraint{
public:
	/**
	The number of maximum allowed working days per week (-1 for don't care)
	*/
	int maxDaysPerWeek;

	/**
	The teacher's name
	*/
	QString teacherName;

	/**
	The teacher's id, or index in the rules
	*/
	int teacher_ID;

	ConstraintTeacherMaxDaysPerWeek();

	ConstraintTeacherMaxDaysPerWeek(double wp, int maxnd, QString t);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

/**
This is a constraint.
It constrains the timetable to not schedule any activity
in the specified day, during the start hour and end hour.
*/
class ConstraintBreak: public TimeConstraint{
public:
	/**
	The day
	*/
	int d;

	/**
	The start hour
	*/
	int h1;

	/**
	The end hour
	*/
	int h2;

	ConstraintBreak();

	ConstraintBreak(double wp, int day, int start_hour, int end_hour);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

/**
This is a constraint. It adds, to the fitness of
the chromosome, a conflicts factor computed from the gaps
existing in the timetable (regarding the students).
The overall result is a timetable having less gaps for the students.
*/
class ConstraintStudentsNoGaps: public TimeConstraint{
public:
	ConstraintStudentsNoGaps();

	ConstraintStudentsNoGaps(double wp);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

/**
This is a constraint. It adds, to the fitness of
the chromosome, a conflicts factor computed from the gaps
existing in the timetable (regarding the specified students set).
*/
class ConstraintStudentsSetNoGaps: public TimeConstraint{
public:
	/**
	The name of the students set for this constraint
	*/
	QString students;

	//internal redundant data

	/**
	The number of subgroups
	*/
	int nSubgroups;

	/**
	The subgroups
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;

	ConstraintStudentsSetNoGaps();

	ConstraintStudentsSetNoGaps(double wp, const QString& st );

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

class ConstraintTeachersMaxGapsPerWeek: public TimeConstraint{
public:
	int maxGaps;	

	ConstraintTeachersMaxGapsPerWeek();

	ConstraintTeachersMaxGapsPerWeek(double wp, int maxGaps);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

class ConstraintTeacherMaxGapsPerWeek: public TimeConstraint{
public:
	int maxGaps;
	
	QString teacherName;
	
	int teacherIndex;

	ConstraintTeacherMaxGapsPerWeek();

	ConstraintTeacherMaxGapsPerWeek(double wp, QString tn, int maxGaps);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

/**
This is a constraint. It adds, to the fitness of
the chromosome, a fitness factor that is related to how early
the students begin their courses. The result is a timetable
having more activities scheduled at the beginning of the day.
IMPORTANT: fortnightly activities are treated as weekly ones,
for speed and because in normal situations this does not matter.
*/
class ConstraintStudentsEarly: public TimeConstraint{
public:

	ConstraintStudentsEarly();

	ConstraintStudentsEarly(double wp);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

class ConstraintStudentsSetEarly: public TimeConstraint{
public:
	/**
	The name of the students
	*/
	QString students;

	/**
	The number of subgroups involved in this restriction
	*/
	int nSubgroups;

	/**
	The subgroups involved in this restriction
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;

	ConstraintStudentsSetEarly();

	ConstraintStudentsSetEarly(double wp, const QString& students);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

class ConstraintStudentsMaxHoursDaily: public TimeConstraint{
public:
	int maxHoursDaily;

	ConstraintStudentsMaxHoursDaily();

	ConstraintStudentsMaxHoursDaily(double wp, int maxnh);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

class ConstraintStudentsSetMaxHoursDaily: public TimeConstraint{
public:
	int maxHoursDaily;

	/**
	The students set name
	*/
	QString students;

	//internal variables

	/**
	The number of subgroups
	*/
	int nSubgroups;

	/**
	The subgroups
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMaxHoursDaily();

	ConstraintStudentsSetMaxHoursDaily(double wp, int maxnh, QString s);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

class ConstraintStudentsMinHoursDaily: public TimeConstraint{
public:
	int minHoursDaily;

	ConstraintStudentsMinHoursDaily();

	ConstraintStudentsMinHoursDaily(double wp, int minnh);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

class ConstraintStudentsSetMinHoursDaily: public TimeConstraint{
public:
	int minHoursDaily;

	/**
	The students set name
	*/
	QString students;

	//internal variables

	/**
	The number of subgroups
	*/
	int nSubgroups;

	/**
	The subgroups
	*/
	//int subgroups[MAX_SUBGROUPS_PER_CONSTRAINT];
	QList<int> iSubgroupsList;

	ConstraintStudentsSetMinHoursDaily();

	ConstraintStudentsSetMinHoursDaily(double wp, int minnh, QString s);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

/**
This is a constraint.
It adds, to the fitness of the chromosome, a fitness factor that
grows as the activity is scheduled farther from the wanted time
For the moment, fitness factor increases with one unit for every hour
and one unit for every day.
*/
class ConstraintActivityPreferredTime: public TimeConstraint{
public:
	/**
	Activity id
	*/
	int activityId;

	/**
	The preferred day. If -1, then the user does not care about the day.
	*/
	int day;

	/**
	The preferred hour. If -1, then the user does not care about the hour.
	*/
	int hour;

	//internal variables
	/**
	The index of the activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int activityIndex;

	ConstraintActivityPreferredTime();

	ConstraintActivityPreferredTime(double wp, int actId, int d, int h);

	/**
	Comparison operator - to be sure that we do not introduce duplicates
	*/
	bool operator==(ConstraintActivityPreferredTime& c);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

/**
This is a constraint.
It returns conflicts if the activity is scheduled in another interval
than the preferred set of times.
*/
class ConstraintActivityPreferredTimes: public TimeConstraint{
public:
	/**
	Activity id
	*/
	int activityId;

	/**
	The number of preferred times
	*/
	int nPreferredTimes;

	/**
	The preferred days. If -1, then the user does not care about the day.
	*/
	int days[MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_TIMES];

	/**
	The preferred hour. If -1, then the user does not care about the hour.
	*/
	int hours[MAX_N_CONSTRAINT_ACTIVITY_PREFERRED_TIMES];

	//internal variables
	/**
	The index of the activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int activityIndex;

	ConstraintActivityPreferredTimes();

	ConstraintActivityPreferredTimes(double wp, int actId, int nPT, int d[], int h[]);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

/**
This is a constraint.
It returns conflicts if a set of activities is scheduled in another interval
than the preferred set of times.
The set of activities is specified by a subject, teacher, students or a combination
of these.
*/
class ConstraintActivitiesPreferredTimes: public TimeConstraint{
public:
	/**
	The teacher. If void, all teachers.
	*/
	QString teacherName;

	/**
	The students. If void, all students.
	*/
	QString studentsName;

	/**
	The subject. If void, all subjects.
	*/
	QString subjectName;

	/**
	The subject tag. If void, all subjects tags.
	*/
	QString subjectTagName;

	/**
	The number of preferred times
	*/
	int nPreferredTimes;

	/**
	The preferred days. If -1, then the user does not care about the day.
	*/
	int days[MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIMES];

	/**
	The preferred hours. If -1, then the user does not care about the hour.
	*/
	int hours[MAX_N_CONSTRAINT_ACTIVITIES_PREFERRED_TIMES];

	//internal variables
	
	/**
	The number of activities which are represented by the subject, teacher and students requirements.
	*/
	int nActivities;
	
	/**
	The indices of the activities in the rules (from 0 to rules.nActivities-1)
	These are indices in the internal list -> Rules::internalActivitiesList
	*/
	int activitiesIndices[MAX_ACTIVITIES];

	ConstraintActivitiesPreferredTimes();

	ConstraintActivitiesPreferredTimes(double wp, QString te,
		QString st, QString su, QString sut, int nPT, int d[], int h[]);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

/**
This is a constraint.
It aims at scheduling a set of activities at the same starting hour.
The number of conflicts is considered the sum of differences
in the scheduling time for all pairs of activities.
The difference in the scheduling time for a pair of
activities is considered the difference in the starting hour.
TODO: Weekly activities are counted as two and fortnightly activities as one
(really necessary?).
IMPORTANT: The compulsory constraints of this kind
implement chromosome repairing, so no conflicts will be reported
*/
class ConstraintActivitiesSameStartingHour: public TimeConstraint{
public:
	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id)
	*/
	int activitiesId[MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR];

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (index in the rules) - internal structure
	*/
	int _activities[MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR];

	ConstraintActivitiesSameStartingHour();

	/**
	Constructor, using:
	the weight, the number of activities and the list of activities' id-s.
	*/
	ConstraintActivitiesSameStartingHour(double wp, int n_act, const int act[]);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	/**
	Removes useless activities from the _activities array
	*/
	void removeUseless(Rules& r);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

class Constraint2ActivitiesConsecutive: public TimeConstraint{
public:
	/**
	First activity id
	*/
	int firstActivityId;

	/**
	Second activity id
	*/
	int secondActivityId;

	//internal variables
	/**
	The index of the first activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int firstActivityIndex;

	/**
	The index of the second activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int secondActivityIndex;

	Constraint2ActivitiesConsecutive();

	Constraint2ActivitiesConsecutive(double wp, int firstActId, int secondActId);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

class ConstraintActivityEndsStudentsDay: public TimeConstraint{
public:
	/**
	Activity id
	*/
	int activityId;

	//internal variables
	/**
	The index of the activity in the rules (from 0 to rules.nActivities-1) - it is not the id of the activity
	*/
	int activityIndex;

	ConstraintActivityEndsStudentsDay();

	ConstraintActivityEndsStudentsDay(double wp, int actId);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

class ConstraintTeachersMinHoursDaily: public TimeConstraint{
public:
	/**
	The minimum hours daily
	*/
	int minHoursDaily;

	ConstraintTeachersMinHoursDaily();

	ConstraintTeachersMinHoursDaily(double wp, int minhours);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};

class ConstraintTeacherMinHoursDaily: public TimeConstraint{
public:
	/**
	The minimum hours daily
	*/
	int minHoursDaily;
	
	QString teacherName;
	
	int teacher_ID;

	ConstraintTeacherMinHoursDaily();

	ConstraintTeacherMinHoursDaily(double wp, int minhours, const QString& teacher);

	QString getXmlDescription(Rules& r);

	bool computeInternalStructure(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>&dl, QString* conflictsString=NULL);

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);
};


#endif
