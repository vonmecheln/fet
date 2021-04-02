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

#include "genetictimetable_defs.h"

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

typedef QList<SpaceConstraint*> SpaceConstraintsList;

const int CONSTRAINT_GENERIC_SPACE								=1001; //time constraints are beginning from 1
const int CONSTRAINT_BASIC_COMPULSORY_SPACE						=1002; //space constraints from 1001
const int CONSTRAINT_ROOM_TYPE_NOT_ALLOWED_SUBJECTS				=1003;
const int CONSTRAINT_ROOM_NOT_AVAILABLE							=1004;
const int CONSTRAINT_SUBJECT_REQUIRES_EQUIPMENTS				=1005;
const int CONSTRAINT_SUBJECT_SUBJECT_TAG_REQUIRE_EQUIPMENTS		=1006;
const int CONSTRAINT_TEACHER_REQUIRES_ROOM						=1007;
const int CONSTRAINT_TEACHER_SUBJECT_REQUIRE_ROOM				=1008;
const int CONSTRAINT_MINIMIZE_NUMBER_OF_ROOMS_FOR_STUDENTS		=1009;
const int CONSTRAINT_MINIMIZE_NUMBER_OF_ROOMS_FOR_TEACHERS		=1010;
const int CONSTRAINT_ACTIVITY_PREFERRED_ROOM					=1011;
const int CONSTRAINT_ACTIVITY_PREFERRED_ROOMS					=1012;
const int CONSTRAINT_ACTIVITIES_SAME_ROOM						=1013;
const int CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOM			=1014;
const int CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS		=1015;
const int CONSTRAINT_SUBJECT_PREFERRED_ROOM						=1016;
const int CONSTRAINT_SUBJECT_PREFERRED_ROOMS					=1017;
const int CONSTRAINT_MAX_BUILDING_CHANGES_PER_DAY_FOR_TEACHERS	=1018;
const int CONSTRAINT_MAX_BUILDING_CHANGES_PER_DAY_FOR_STUDENTS	=1019;
const int CONSTRAINT_MAX_ROOM_CHANGES_PER_DAY_FOR_TEACHERS		=1020;
const int CONSTRAINT_MAX_ROOM_CHANGES_PER_DAY_FOR_STUDENTS		=1021;

/**
This class represents a space constraint
*/
class SpaceConstraint{
public:
	/**
	The weight of this constraint
	*/
	double weight;

	/**
	Specifies the type of this constraint (using the above constants).
	*/
	int type;

	/**
	Is this constraint compulsory (mandatory) or just a wish?
	*/
	bool compulsory;

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
	SpaceConstraint(double w, bool c);

	/**
	The function that calculates the fitness of a space chromosome, according to this
	constraint. We need the rules to compute this fitness factor.
	We need also the allocation of the activities on days and hours.
	If conflictsString!=NULL,
	it will be initialized with a text explaining where this restriction is broken.
	*/
	virtual int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL)=0;

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
	Returns true if this constraint is related to this equipment
	*/
	virtual bool isRelatedToEquipment(Equipment* e)=0;

	/**
	Returns true if this constraint is related to this building
	*/
	virtual bool isRelatedToBuilding(Building* b)=0;

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

	ConstraintBasicCompulsorySpace(double w);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

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

	ConstraintRoomNotAvailable(double w, bool c, const QString& rn, int day, int start_hour, int end_hour);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

	bool isRelatedToRoom(Room* r);
};

/**
This is a constraint. Its purpose: a subject must not be taught in
a room which does not support the room's type.
*/
class ConstraintRoomTypeNotAllowedSubjects: public SpaceConstraint{
private:

	//The number of rooms referred to by this constraint
	int _nRooms;

	//The list of rooms referred to by this constraint. This is
	//a list of indices in the rules internal rooms list.
	int _rooms[MAX_ROOMS];

	//The number of activities referred to by this constraint
	int _nActivities;

	//The list of activities referred to by this constraint.
	//This is a list of indices in the rules internal activities list.
	int _activities[MAX_ACTIVITIES];

public:

	QString roomType;

	QStringList subjects;

	ConstraintRoomTypeNotAllowedSubjects();

	ConstraintRoomTypeNotAllowedSubjects(double w, bool c, const QString& room_type);

	void addNotAllowedSubject(const QString& subjectName);

	/**
	Returns the number of removed subjects (must be 0 or 1, of course)
	*/
	int removeNotAllowedSubject(const QString& subjectName);

	/**
	Returns true if the subject is in the list of non allowed subjects,
	false otherwise.
	*/
	bool searchNotAllowedSubject(const QString& subjectName);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

	bool isRelatedToRoom(Room* r);
};

/**
This is a constraint. Its purpose: a subject must be taught in
a room which has the necessary equipments.
*/
class ConstraintSubjectRequiresEquipments: public SpaceConstraint{
private:
	
	//The number of activities referred to by this constraint
	int _nActivities;

	//The list of activities referred to by this constraint.
	//This is a list of indices in the rules internal activities list.
	int _activities[MAX_ACTIVITIES_FOR_A_SUBJECT];
	
	//The number of equipments referred to by this constraint
	int _nEquipments;
	
	//The indices of the equipments referred to by this constraint
	int _equipments[MAX_EQUIPMENTS_FOR_A_CONSTRAINT];

public:

	QString subjectName;

	QStringList equipmentsNames;

	ConstraintSubjectRequiresEquipments();

	ConstraintSubjectRequiresEquipments(double w, bool c, const QString& subj);

	void addRequiredEquipment(const QString& equip);

	/**
	Returns the number of removed equipments (must be 0 or 1, of course)
	*/
	int removeRequiredEquipment(const QString& equip);

	/**
	Returns true if this equipment is in the list of required equipments,
	false otherwise.
	*/
	bool searchRequiredEquipment(const QString& equip);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

	bool isRelatedToRoom(Room* r);
};

/**
This is a constraint. Its purpose: a subject+subject tag must be taught in
a room which has the necessary equipments.
*/
class ConstraintSubjectSubjectTagRequireEquipments: public SpaceConstraint{
private:
	
	//The number of activities referred to by this constraint
	int _nActivities;

	//The list of activities referred to by this constraint.
	//This is a list of indices in the rules internal activities list.
	int _activities[MAX_ACTIVITIES_FOR_A_SUBJECT];
	
	//The number of equipments referred to by this constraint
	int _nEquipments;
	
	//The indices of the equipments referred to by this constraint
	int _equipments[MAX_EQUIPMENTS_FOR_A_CONSTRAINT];

public:

	QString subjectName;

	QString subjectTagName;

	QStringList equipmentsNames;

	ConstraintSubjectSubjectTagRequireEquipments();

	ConstraintSubjectSubjectTagRequireEquipments(double w, bool c, const QString& subj, const QString& subt);

	void addRequiredEquipment(const QString& equip);

	/**
	Returns the number of removed equipments (must be 0 or 1, of course)
	*/
	int removeRequiredEquipment(const QString& equip);

	/**
	Returns true if the equipment is in the list of required equipments,
	false otherwise.
	*/
	bool searchRequiredEquipment(const QString& equip);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

	bool isRelatedToRoom(Room* r);
};

/**
This is a constraint. Its purpose: a teacher must teach in
his preferred room.
*/
class ConstraintTeacherRequiresRoom: public SpaceConstraint{
public:
	
	//The number of activities referred to by this constraint
	int _nActivities;

	//The list of activities referred to by this constraint.
	//This is a list of indices in the rules internal activities list.
	int _activities[MAX_ACTIVITIES_FOR_A_TEACHER];
	
	//The index of the room
	int _room; 

	//----------------------------------------------------------

	QString teacherName;

	QString roomName;

	ConstraintTeacherRequiresRoom();

	ConstraintTeacherRequiresRoom(double w, bool c, const QString& teach, const QString& room);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

	bool isRelatedToRoom(Room* r);
};

/**
This is a constraint. Its purpose: a teacher must teach a certain subject in
his preferred room.
*/
class ConstraintTeacherSubjectRequireRoom: public SpaceConstraint{
public:
	
	//The number of activities referred to by this constraint
	int _nActivities;

	//The list of activities referred to by this constraint.
	//This is a list of indices in the rules internal activities list.
	int _activities[MAX_ACTIVITIES_FOR_A_TEACHER_AND_SUBJECT];
	
	//The index of the room
	int _room; 

	//----------------------------------------------------------

	QString teacherName;
	
	QString subjectName;

	QString roomName;

	ConstraintTeacherSubjectRequireRoom();

	ConstraintTeacherSubjectRequireRoom(double w, bool c, const QString& teach, const QString& subj, const QString& room);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

	bool isRelatedToRoom(Room* r);
};

/**
This is a constraint aimed to minimize the number of rooms used
for all the students.
*/
class ConstraintMinimizeNumberOfRoomsForStudents: public SpaceConstraint{
public:
	ConstraintMinimizeNumberOfRoomsForStudents();

	ConstraintMinimizeNumberOfRoomsForStudents(double w, bool c);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

	bool isRelatedToRoom(Room* r);
};

/**
This is a constraint aimed to minimize the number of rooms used
for all the teachers.
*/
class ConstraintMinimizeNumberOfRoomsForTeachers: public SpaceConstraint{
public:
	ConstraintMinimizeNumberOfRoomsForTeachers();

	ConstraintMinimizeNumberOfRoomsForTeachers(double w, bool c);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

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

	ConstraintActivityPreferredRoom(double w, bool c, int aid, const QString& room);
	
	/**
	Comparison operator - to be sure we do not introduce duplicates
	*/
	bool operator==(ConstraintActivityPreferredRoom& c);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

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

	ConstraintActivityPreferredRooms(double w, bool c, int aid, const QStringList& roomsList);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

	bool isRelatedToRoom(Room* r);
};

/**
This is a constraint.
It aims at scheduling a set of activities in the same room.
The number of conflicts is considered the sum of x for all pairs,
where x is 1 if 2 activities have different rooms and 0 if they
have the same room.
The compulsory constraints of this kind
implement chromosome repairing, so no conflicts will be reported
*/
class ConstraintActivitiesSameRoom: public SpaceConstraint{
public:
	/**
	The number of activities involved in this constraint
	*/
	int n_activities;

	/**
	The activities involved in this constraint (id)
	*/
	int activitiesId[MAX_CONSTRAINT_ACTIVITIES_SAME_ROOM];

	/**
	The number of activities involved in this constraint - internal structure
	*/
	int _n_activities;

	/**
	The activities involved in this constraint (indexes in the rules) - internal structure
	*/
	int _activities[MAX_CONSTRAINT_ACTIVITIES_SAME_ROOM];

	ConstraintActivitiesSameRoom();

	/**
	Constructor, using:
	the weight, the number of activities and the list of activities' id-s.
	*/
	ConstraintActivitiesSameRoom(double w, bool c, int n_act, const int act[]);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);

	/**
	Removes useless activities from the _activities and activitiesId array
	*/
	void removeUseless(Rules& r);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

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
	int _activities[MAX_ACTIVITIES_FOR_A_SUBJECT];
	
	// The index of the room
	int _room;
	
public:

	QString subjectName;

	QString subjectTagName;

	QString roomName;

	ConstraintSubjectSubjectTagPreferredRoom();

	ConstraintSubjectSubjectTagPreferredRoom(double w, bool c, const QString& subj, const QString& subt, const QString& rm);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

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
	int _activities[MAX_ACTIVITIES_FOR_A_SUBJECT];
	
	//The number of preferred rooms
	int _n_preferred_rooms;
	
	//The indexes of the rooms
	int _rooms[MAX_CONSTRAINT_SUBJECT_SUBJECT_TAG_PREFERRED_ROOMS];
	
public:

	QString subjectName;

	QString subjectTagName;

	QStringList roomsNames;

	ConstraintSubjectSubjectTagPreferredRooms();

	ConstraintSubjectSubjectTagPreferredRooms(double w, bool c, const QString& subj, const QString& subt, const QStringList& rms);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

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

	ConstraintSubjectPreferredRoom(double w, bool c, const QString& subj, const QString& rm);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

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

	ConstraintSubjectPreferredRooms(double w, bool c, const QString& subj, const QStringList& rms);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

	bool isRelatedToRoom(Room* r);
};

/**
This is constraint not allowing more than max building changes per day
for teachers. This constraint may also be used to minimize the number
of building changes in each day, if you put maxBuildingChanges=0.
*/
class ConstraintMaxBuildingChangesPerDayForTeachers: public SpaceConstraint{
public:
	int maxBuildingChanges;

	ConstraintMaxBuildingChangesPerDayForTeachers();

	ConstraintMaxBuildingChangesPerDayForTeachers(double w, bool c, int max_building_changes);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

	bool isRelatedToRoom(Room* r);
};

/**
This is constraint not allowing more than max building changes per day
for students. This constraint may also be used to minimize the number
of building changes in each day, if you put maxBuildingChanges=0.
*/
class ConstraintMaxBuildingChangesPerDayForStudents: public SpaceConstraint{
public:
	int maxBuildingChanges;

	ConstraintMaxBuildingChangesPerDayForStudents();

	ConstraintMaxBuildingChangesPerDayForStudents(double w, bool c, int max_building_changes);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

	bool isRelatedToRoom(Room* r);
};

/**
This is constraint not allowing more than max room changes per day
for teachers. This constraint may also be used to minimize the number
of room changes in each day, if you put maxRoomChanges=0.
*/
class ConstraintMaxRoomChangesPerDayForTeachers: public SpaceConstraint{
public:
	int maxRoomChanges;

	ConstraintMaxRoomChangesPerDayForTeachers();

	ConstraintMaxRoomChangesPerDayForTeachers(double w, bool c, int max_room_changes);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

	bool isRelatedToRoom(Room* r);
};

/**
This is constraint not allowing more than max room changes per day
for students. This constraint may also be used to minimize the number
of room changes in each day, if you put maxRoomChanges=0.
*/
class ConstraintMaxRoomChangesPerDayForStudents: public SpaceConstraint{
public:
	int maxRoomChanges;

	ConstraintMaxRoomChangesPerDayForStudents();

	ConstraintMaxRoomChangesPerDayForStudents(double w, bool c, int max_room_changes);

	bool computeInternalStructure(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	int fitness(SpaceChromosome& c, Rules& r, const int days[/*MAX_ACTIVITIES*/], const int hours[/*MAX_ACTIVITIES*/], QString* conflictsString=NULL);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToSubjectTag(SubjectTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToEquipment(Equipment* e);

	bool isRelatedToBuilding(Building* b);

	bool isRelatedToRoom(Room* r);
};


#endif
