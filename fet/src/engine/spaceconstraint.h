/*
File spaceconstraint.h
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

#ifndef SPACECONSTRAINT_H
#define SPACECONSTRAINT_H

#include "timetable_defs.h"

#include <QString>
#include  <QList>
#include <QStringList>

class Rules;
class SpaceChromosome;
class SpaceConstraint;
class Activity;
class Teacher;
class Subject;
class SubjectTag;
class StudentsSet;
class Equipment;
class Building;
class Room;
class Solution;

typedef QList<SpaceConstraint*> SpaceConstraintsList;

const int CONSTRAINT_GENERIC_SPACE								=1000; //time constraints are beginning from 1
const int CONSTRAINT_BASIC_COMPULSORY_SPACE						=1001; //space constraints from 1001
const int CONSTRAINT_ROOM_NOT_AVAILABLE							=1002;
const int CONSTRAINT_ACTIVITY_PREFERRED_ROOM					=1003;
const int CONSTRAINT_ACTIVITY_PREFERRED_ROOMS					=1004;
const int CONSTRAINT_SUBJECT_PREFERRED_ROOM						=1005;
const int CONSTRAINT_SUBJECT_PREFERRED_ROOMS					=1006;

const int CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOM			=1007;
const int CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS		=1008;

/**
This class represents a space constraint
*/
class SpaceConstraint{
public:
	/**
	The weight (percentage) of this constraint
	*/
	double weightPercentage;

	/**
	Specifies the type of this constraint (using the above constants).
	*/
	int type;

	/**
	Dummy constructor - needed for the static array of constraints.
	Use of this function must be avoided.
	*/
	SpaceConstraint();
	
	virtual ~SpaceConstraint()=0;

	/**
	Constructor - please note that the maximum allowed weight is 100.0
	The reason: unallocated activities must have very big conflict weight,
	and any other restrictions must have much more lower weight,
	so that the timetable can evolve when starting with uninitialized activities
	*/
	SpaceConstraint(double wp);

	/**
	The function that calculates the fitness of a space chromosome, according to this
	constraint. We need the rules to compute this fitness factor.
	We need also the allocation of the activities on days and hours.
	If conflictsString!=NULL,
	it will be initialized with a text explaining where this restriction is broken.
	*/
	virtual double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL)=0;

	/**
	Returns an XML description of this constraint
	*/
	virtual QString getXmlDescription(Rules& r)=0;

	/**
	Computes the internal structure for this constraint
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

	/**
	Returns true if this constraint is related to this room
	*/
	virtual bool isRelatedToRoom(Room* r)=0;
};

/**
This class comprises all the basic compulsory constraints (constraints
which must be fulfilled for any timetable) - the space allocation part
*/
class ConstraintBasicCompulsorySpace: public SpaceConstraint{
public:
	ConstraintBasicCompulsorySpace();

	ConstraintBasicCompulsorySpace(double wp);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);
};

/**
This is a custom constraint.
It returns a fitness factor a number equal
to the product of this restriction's weight and
the number of conflicting hours for each room
(hours when it is not available, but an activity is scheduled at that time).
For the moment, this is done for a certain day and an hour interval.
(For room "roomName", on day "d", between hours "h1" and "h2").
*/
class ConstraintRoomNotAvailable: public SpaceConstraint{
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
	The room's name
	*/
	QString roomName;

	/**
	The room's id, or index in the rules
	*/
	int room_ID;

	ConstraintRoomNotAvailable();

	ConstraintRoomNotAvailable(double wp, const QString& rn, int day, int start_hour, int end_hour);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);
};

/**
This is a constraint. Its purpose: an activity must take part in
the preferred room.
*/
class ConstraintActivityPreferredRoom: public SpaceConstraint{
public:
	
	//The activity referred to by this constraint.
	//This is an index in the rules internal activities list.
	int _activity;
	
	//The index of the room
	int _room; 

	//----------------------------------------------------------

	int activityId;

	QString roomName;

	ConstraintActivityPreferredRoom();

	ConstraintActivityPreferredRoom(double wp, int aid, const QString& room);
	
	/**
	Comparison operator - to be sure we do not introduce duplicates
	*/
	bool operator==(ConstraintActivityPreferredRoom& c);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	//int fitness(Solution& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);
};

/**
This is a constraint. Its purpose: an activity must take part in
the preferred rooms.
*/
class ConstraintActivityPreferredRooms: public SpaceConstraint{
public:
	
	//The activity referred to by this constraint.
	//This is an index in the rules internal activities list.
	int _activity;
	
	//The number of preferred rooms
	int _n_preferred_rooms;
	
	//The indexes of the rooms
	int _rooms[MAX_CONSTRAINT_ACTIVITY_PREFERRED_ROOMS];

	//----------------------------------------------------------

	int activityId;

	QStringList roomsNames;

	ConstraintActivityPreferredRooms();

	ConstraintActivityPreferredRooms(double wp, int aid, const QStringList& roomsList);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	//int fitness(Solution& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);
};

/**
This is a constraint. Its purpose: a subject must be taught in
a certain room.
*/
class ConstraintSubjectPreferredRoom: public SpaceConstraint{
public:
	
	//The number of activities referred to by this constraint
	int _nActivities;

	//The list of activities referred to by this constraint.
	//This is a list of indices in the rules internal activities list.
	int _activities[MAX_ACTIVITIES_FOR_A_SUBJECT];
	
	// The index of the room
	int _room;
	
public:

	QString subjectName;

	QString roomName;

	ConstraintSubjectPreferredRoom();

	ConstraintSubjectPreferredRoom(double wp, const QString& subj, const QString& rm);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	//int fitness(Solution& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);
};

/**
This is a constraint. Its purpose: a subject must be taught in
certain rooms.
*/
class ConstraintSubjectPreferredRooms: public SpaceConstraint{
public:
	
	//The number of activities referred to by this constraint
	int _nActivities;

	//The list of activities referred to by this constraint.
	//This is a list of indices in the rules internal activities list.
	int _activities[MAX_ACTIVITIES_FOR_A_SUBJECT];
	
	//The number of preferred rooms
	int _n_preferred_rooms;
	
	//The indexes of the rooms
	int _rooms[MAX_CONSTRAINT_SUBJECT_PREFERRED_ROOMS];
	
public:

	QString subjectName;

	QStringList roomsNames;

	ConstraintSubjectPreferredRooms();

	ConstraintSubjectPreferredRooms(double wp, const QString& subj, const QStringList& rms);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	//int fitness(Solution& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);
};

/**
This is a constraint. Its purpose: a subject+subject tag must be taught in
a certain room.
*/
class ConstraintSubjectSubjectTagPreferredRoom: public SpaceConstraint{
public:
	
	//The number of activities referred to by this constraint
	int _nActivities;

	//The list of activities referred to by this constraint.
	//This is a list of indices in the rules internal activities list.
	int _activities[MAX_ACTIVITIES_FOR_A_SUBJECT_SUBJECT_TAG];
	
	// The index of the room
	int _room;
	
public:

	QString subjectName;

	QString subjectTagName;

	QString roomName;

	ConstraintSubjectSubjectTagPreferredRoom();

	ConstraintSubjectSubjectTagPreferredRoom(double wp, const QString& subj, const QString& subjTag, const QString& rm);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	//int fitness(Solution& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);
};

/**
This is a constraint. Its purpose: a subject+subject tag must be taught in
certain rooms.
*/
class ConstraintSubjectSubjectTagPreferredRooms: public SpaceConstraint{
public:
	
	//The number of activities referred to by this constraint
	int _nActivities;

	//The list of activities referred to by this constraint.
	//This is a list of indices in the rules internal activities list.
	int _activities[MAX_ACTIVITIES_FOR_A_SUBJECT_SUBJECT_TAG];
	
	//The number of preferred rooms
	int _n_preferred_rooms;
	
	//The indexes of the rooms
	int _rooms[MAX_CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS];
	
public:

	QString subjectName;

	QString subjectTagName;

	QStringList roomsNames;

	ConstraintSubjectSubjectTagPreferredRooms();

	ConstraintSubjectSubjectTagPreferredRooms(double wp, const QString& subj, const QString& subjTag, const QStringList& rms);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);
};


#endif
