/*
File spaceconstraint.h
*/

/***************************************************************************
                          spaceconstraint.h  -  description
                             -------------------
    begin                : 2002
    copyright            : (C) 2002 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef SPACECONSTRAINT_H
#define SPACECONSTRAINT_H

#include <QCoreApplication>

#include "timetable_defs.h"

#include <QString>
#include <QList>
#include <QStringList>
#include <QSet>

class Rules;
class SpaceConstraint;
class Activity;
class Teacher;
class Subject;
class ActivityTag;
class StudentsSet;
class Building;
class Room;
class Solution;

class FakeString;

class QWidget;

typedef QList<SpaceConstraint*> SpaceConstraintsList;

//If you change any of these const int-s, you need to update the const QString FET_DATA_FORMAT_VERSION from timetable_defs.cpp to a new value,
//because of the disk history feature.
const int CONSTRAINT_GENERIC_SPACE										=1000; //time constraints are beginning from 1

const int CONSTRAINT_BASIC_COMPULSORY_SPACE								=1001; //space constraints from 1001
const int CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES							=1002;

const int CONSTRAINT_ACTIVITY_PREFERRED_ROOM							=1003;
const int CONSTRAINT_ACTIVITY_PREFERRED_ROOMS							=1004;

const int CONSTRAINT_STUDENTS_SET_HOME_ROOM								=1005;
const int CONSTRAINT_STUDENTS_SET_HOME_ROOMS							=1006;

const int CONSTRAINT_TEACHER_HOME_ROOM									=1007;
const int CONSTRAINT_TEACHER_HOME_ROOMS									=1008;

const int CONSTRAINT_SUBJECT_PREFERRED_ROOM								=1009;
const int CONSTRAINT_SUBJECT_PREFERRED_ROOMS							=1010;
const int CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM				=1011;
const int CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS				=1012;

const int CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY				=1013;
const int CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY			=1014;
const int CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK				=1015;
const int CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK			=1016;
const int CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES			=1017;
const int CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES		=1018;

const int CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY				=1019;
const int CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY				=1020;
const int CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK				=1021;
const int CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK				=1022;
const int CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES			=1023;
const int CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES			=1024;

const int CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM						=1025;
const int CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS						=1026;

const int CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS				=1027;
const int CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE				=1028;

//2019-11-14
const int CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY					=1029;
const int CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY				=1030;
const int CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_WEEK					=1031;
const int CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK				=1032;
const int CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ROOM_CHANGES				=1033;
const int CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES			=1034;

const int CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY					=1035;
const int CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY					=1036;
const int CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK					=1037;
const int CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK					=1038;
const int CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES				=1039;
const int CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES				=1040;

const int CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES					=1041;

//For mornings-afternoons
const int CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY				=1042;
const int CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY			=1043;
const int CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY				=1044;
const int CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY				=1045;

const int CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY			=1046;
const int CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY		=1047;
const int CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY			=1048;
const int CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY			=1049;

//For all modes
const int CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL				=1050;
const int CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL			=1051;
const int CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL				=1052;
const int CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL				=1053;

//For mornings-afternoons
const int CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL			=1054;
const int CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL		=1055;
const int CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL			=1056;
const int CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL			=1057;

//For all modes
const int CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL				=1058;
const int CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL			=1059;
const int CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL				=1060;
const int CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL				=1061;

//For mornings-afternoons
const int CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL			=1062;
const int CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL		=1063;
const int CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL			=1064;
const int CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL			=1065;

const int CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET					=1066;
const int CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET				=1067;
const int CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_WEEK_FROM_SET					=1068;

bool spaceConstraintCanHaveAnyWeight(int type);

/**
This class represents a space constraint
*/
class SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(SpaceConstraint)
	
public:
	/**
	The weight (percentage) of this constraint
	*/
	double weightPercentage;
	
	bool active;
	
	QString comments;

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
	DEPRECATED COMMENT BELOW:
	Constructor - please note that the maximum allowed weight is 100.0
	The reason: unallocated activities must have very big conflict weight,
	and any other constraints must have much more lower weight,
	so that the timetable can evolve when starting with uninitialized activities
	*/
	SpaceConstraint(double wp);

	bool canHaveAnyWeight(); //false if it can have the weight only 100%.

	bool canBeUsedInOfficialMode();
	bool canBeUsedInMorningsAfternoonsMode();
	bool canBeUsedInBlockPlanningMode();
	bool canBeUsedInTermsMode();

	/**
	The function that calculates the fitness of a solution, according to this
	constraint. We need the solution to compute this fitness factor.
	We need also the allocation of the activities on days and hours.
	If conflictsString!=nullptr, it will be initialized with a text explaining
	where this constraint is broken.
	*/
	virtual double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr)=0;

	/**
	Returns an XML description of this constraint
	*/
	virtual QString getXmlDescription(Rules& r)=0;

	/**
	Computes the internal structure for this constraint
	*/
	virtual bool computeInternalStructure(QWidget* parent, Rules& r)=0;
	
	virtual bool hasInactiveActivities(Rules& r)=0;

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
	Returns true if this constraint is related to this activity tag
	*/
	virtual bool isRelatedToActivityTag(ActivityTag* s)=0;

	/**
	Returns true if this constraint is related to this students set
	*/
	virtual bool isRelatedToStudentsSet(Rules& r, StudentsSet* s)=0;

	/**
	Returns true if this constraint is related to this room
	*/
	virtual bool isRelatedToRoom(Room* r)=0;

	virtual bool hasWrongDayOrHour(Rules& r)=0;
	virtual bool canRepairWrongDayOrHour(Rules& r)=0;
	virtual bool repairWrongDayOrHour(Rules& r)=0;
};

/**
This class comprises all the basic compulsory constraints (constraints
which must be fulfilled for any timetable) - the space allocation part
*/
class ConstraintBasicCompulsorySpace: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintBasicCompulsorySpace)
	
public:
	ConstraintBasicCompulsorySpace();

	ConstraintBasicCompulsorySpace(double wp);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintRoomNotAvailableTimes: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintRoomNotAvailableTimes)
	
public:

	QList<int> days;
	QList<int> hours;

	/**
	The room's name
	*/
	QString room;

	/**
	The room's id, or index in the rules
	*/
	int room_ID;

	ConstraintRoomNotAvailableTimes();

	ConstraintRoomNotAvailableTimes(double wp, const QString& rn, const QList<int>& d, const QList<int>& h);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherRoomNotAvailableTimes: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherRoomNotAvailableTimes)
	
public:

	QList<int> days;
	QList<int> hours;

	/**
	The room's name
	*/
	QString room;
	
	QString teacherName;

	/**
	The room's id, or index in the rules
	*/
	int room_ID;
	
	int teacher_ID;

	ConstraintTeacherRoomNotAvailableTimes();

	ConstraintTeacherRoomNotAvailableTimes(double wp, const QString& tn, const QString& rn, const QList<int>& d, const QList<int>& h);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint. Its purpose: an activity must take place in
the preferred room.
*/
class ConstraintActivityPreferredRoom: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityPreferredRoom)
	
public:
	
	//The activity referred to by this constraint.
	//This is an index in the rules internal activities list.
	int _activity;
	
	//The index of the room
	int _room;

	//----------------------------------------------------------

	int activityId;

	QString roomName;
	
	QStringList preferredRealRoomsNames;
	
	QSet<int> preferredRealRooms;
	
	bool permanentlyLocked;

	ConstraintActivityPreferredRoom();

	ConstraintActivityPreferredRoom(double wp, int aid, const QString& room, const QStringList& realRooms, bool perm);
	
	/**
	Comparison operator - to be sure we do not introduce duplicates
	*/
	bool operator==(ConstraintActivityPreferredRoom& c);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint. Its purpose: an activity must take place in
the preferred rooms.
*/
class ConstraintActivityPreferredRooms: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityPreferredRooms)

public:
	
	//The activity referred to by this constraint.
	//This is an index in the rules internal activities list.
	int _activity;
	
	//The indexes of the rooms
	QList<int> _rooms;

	//----------------------------------------------------------

	int activityId;

	QStringList roomsNames;

	ConstraintActivityPreferredRooms();

	ConstraintActivityPreferredRooms(double wp, int aid, const QStringList& roomsList);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetHomeRoom: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetHomeRoom)

public:

	QList<int> _activities;
	
	// The index of the room
	int _room;
	
	QString studentsName;

	QString roomName;

	ConstraintStudentsSetHomeRoom();

	ConstraintStudentsSetHomeRoom(double wp, const QString& st, const QString& rm);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetHomeRooms: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetHomeRooms)

public:
	
	//The list of activities referred to by this constraint.
	//This is a list of indices in the rules internal activities list.
	QList<int> _activities;
	
	//The indexes of the rooms
	QList<int> _rooms;
	
	QString studentsName;

	QStringList roomsNames;

	ConstraintStudentsSetHomeRooms();

	ConstraintStudentsSetHomeRooms(double wp, const QString& st, const QStringList& rms);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherHomeRoom: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherHomeRoom)

public:

	QList<int> _activities;
	
	// The index of the room
	int _room;
	
	QString teacherName;

	QString roomName;

	ConstraintTeacherHomeRoom();

	ConstraintTeacherHomeRoom(double wp, const QString& tc, const QString& rm);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherHomeRooms: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherHomeRooms)

public:
	
	//The list of activities referred to by this constraint.
	//This is a list of indices in the rules internal activities list.
	QList<int> _activities;
	
	//The indexes of the rooms
	QList<int> _rooms;
	
	QString teacherName;

	QStringList roomsNames;

	ConstraintTeacherHomeRooms();

	ConstraintTeacherHomeRooms(double wp, const QString& st, const QStringList& rms);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint. Its purpose: a subject must be taught in
a certain room.
*/
class ConstraintSubjectPreferredRoom: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintSubjectPreferredRoom)

public:

	QList<int> _activities;
	
	// The index of the room
	int _room;
	
	QString subjectName;

	QString roomName;

	ConstraintSubjectPreferredRoom();

	ConstraintSubjectPreferredRoom(double wp, const QString& subj, const QString& rm);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint. Its purpose: a subject must be taught in
certain rooms.
*/
class ConstraintSubjectPreferredRooms: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintSubjectPreferredRooms)

public:

	QList<int> _activities;
	
	QList<int> _rooms;
	
	QString subjectName;

	QStringList roomsNames;

	ConstraintSubjectPreferredRooms();

	ConstraintSubjectPreferredRooms(double wp, const QString& subj, const QStringList& rms);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint. Its purpose: a subject + an activity tag must be taught in
a certain room.
*/
class ConstraintSubjectActivityTagPreferredRoom: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintSubjectActivityTagPreferredRoom)

public:

	QList<int> _activities;
	
	// The index of the room
	int _room;
	
	QString subjectName;

	QString activityTagName;

	QString roomName;

	ConstraintSubjectActivityTagPreferredRoom();

	ConstraintSubjectActivityTagPreferredRoom(double wp, const QString& subj, const QString& actTag, const QString& rm);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

/**
This is a constraint. Its purpose: a subject + an activity tag must be taught in
certain rooms.
*/
class ConstraintSubjectActivityTagPreferredRooms: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintSubjectActivityTagPreferredRooms)

public:
	
	QList<int> _activities;
	
	QList<int> _rooms;

	QString subjectName;

	QString activityTagName;

	QStringList roomsNames;

	ConstraintSubjectActivityTagPreferredRooms();

	ConstraintSubjectActivityTagPreferredRooms(double wp, const QString& subj, const QString& actTag, const QStringList& rms);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//addded on 6 apr 2009
class ConstraintActivityTagPreferredRoom: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityTagPreferredRoom)
	
public:

	QList<int> _activities;
	
	// The index of the room
	int _room;
	
	QString activityTagName;

	QString roomName;

	ConstraintActivityTagPreferredRoom();

	ConstraintActivityTagPreferredRoom(double wp, const QString& actTag, const QString& rm);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivityTagPreferredRooms: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivityTagPreferredRooms)

public:
	
	QList<int> _activities;
	
	QList<int> _rooms;

	QString activityTagName;

	QStringList roomsNames;

	ConstraintActivityTagPreferredRooms();

	ConstraintActivityTagPreferredRooms(double wp, const QString& actTag, const QStringList& rms);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};
///////

class ConstraintStudentsSetMaxBuildingChangesPerDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxBuildingChangesPerDay)

public:
	//internal variables
	QList<int> iSubgroupsList;

	int maxBuildingChangesPerDay;

	QString studentsName;

	ConstraintStudentsSetMaxBuildingChangesPerDay();

	ConstraintStudentsSetMaxBuildingChangesPerDay(double wp, const QString& st, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxBuildingChangesPerDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxBuildingChangesPerDay)

public:

	int maxBuildingChangesPerDay;

	ConstraintStudentsMaxBuildingChangesPerDay();

	ConstraintStudentsMaxBuildingChangesPerDay(double wp, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxBuildingChangesPerDayInInterval: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxBuildingChangesPerDayInInterval)

public:
	//internal variables
	QList<int> iSubgroupsList;

	int maxBuildingChangesPerDay;
	
	int intervalStart;
	int intervalEnd;

	QString studentsName;

	ConstraintStudentsSetMaxBuildingChangesPerDayInInterval();

	ConstraintStudentsSetMaxBuildingChangesPerDayInInterval(double wp, const QString& st, int mc, int is, int ie);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxBuildingChangesPerDayInInterval: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxBuildingChangesPerDayInInterval)

public:
	int maxBuildingChangesPerDay;
	
	int intervalStart;
	int intervalEnd;

	ConstraintStudentsMaxBuildingChangesPerDayInInterval();

	ConstraintStudentsMaxBuildingChangesPerDayInInterval(double wp, int mc, int is, int ie);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval)

public:
	//internal variables
	QList<int> iSubgroupsList;

	int maxBuildingChangesPerDay;
	
	int intervalStart;
	int intervalEnd;

	QString studentsName;

	ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval();

	ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval(double wp, const QString& st, int mc, int is, int ie);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxBuildingChangesPerRealDayInInterval: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxBuildingChangesPerRealDayInInterval)

public:
	int maxBuildingChangesPerDay;
	
	int intervalStart;
	int intervalEnd;

	ConstraintStudentsMaxBuildingChangesPerRealDayInInterval();

	ConstraintStudentsMaxBuildingChangesPerRealDayInInterval(double wp, int mc, int is, int ie);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxRoomChangesPerDayInInterval: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxRoomChangesPerDayInInterval)

public:
	//internal variables
	QList<int> iSubgroupsList;

	int maxRoomChangesPerDay;
	
	int intervalStart;
	int intervalEnd;

	QString studentsName;

	ConstraintStudentsSetMaxRoomChangesPerDayInInterval();

	ConstraintStudentsSetMaxRoomChangesPerDayInInterval(double wp, const QString& st, int mc, int is, int ie);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxRoomChangesPerDayInInterval: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxRoomChangesPerDayInInterval)

public:
	int maxRoomChangesPerDay;
	
	int intervalStart;
	int intervalEnd;

	ConstraintStudentsMaxRoomChangesPerDayInInterval();

	ConstraintStudentsMaxRoomChangesPerDayInInterval(double wp, int mc, int is, int ie);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval)

public:
	//internal variables
	QList<int> iSubgroupsList;

	int maxRoomChangesPerDay;
	
	int intervalStart;
	int intervalEnd;

	QString studentsName;

	ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval();

	ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval(double wp, const QString& st, int mc, int is, int ie);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxRoomChangesPerRealDayInInterval: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxRoomChangesPerRealDayInInterval)

public:
	int maxRoomChangesPerDay;
	
	int intervalStart;
	int intervalEnd;

	ConstraintStudentsMaxRoomChangesPerRealDayInInterval();

	ConstraintStudentsMaxRoomChangesPerRealDayInInterval(double wp, int mc, int is, int ie);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxBuildingChangesPerWeek: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxBuildingChangesPerWeek)

public:
	//internal variables
	QList<int> iSubgroupsList;

	int maxBuildingChangesPerWeek;

	QString studentsName;

	ConstraintStudentsSetMaxBuildingChangesPerWeek();

	ConstraintStudentsSetMaxBuildingChangesPerWeek(double wp, const QString& st, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxBuildingChangesPerWeek: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxBuildingChangesPerWeek)

public:

	int maxBuildingChangesPerWeek;

	ConstraintStudentsMaxBuildingChangesPerWeek();

	ConstraintStudentsMaxBuildingChangesPerWeek(double wp, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMinGapsBetweenBuildingChanges: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMinGapsBetweenBuildingChanges)

public:
	//internal variables
	QList<int> iSubgroupsList;

	int minGapsBetweenBuildingChanges;

	QString studentsName;

	ConstraintStudentsSetMinGapsBetweenBuildingChanges();

	ConstraintStudentsSetMinGapsBetweenBuildingChanges(double wp, const QString& st, int mg);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMinGapsBetweenBuildingChanges: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMinGapsBetweenBuildingChanges)

public:

	int minGapsBetweenBuildingChanges;

	ConstraintStudentsMinGapsBetweenBuildingChanges();

	ConstraintStudentsMinGapsBetweenBuildingChanges(double wp, int mg);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxBuildingChangesPerDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxBuildingChangesPerDay)

public:
	//internal variables
	int teacher_ID;

	int maxBuildingChangesPerDay;

	QString teacherName;

	ConstraintTeacherMaxBuildingChangesPerDay();

	ConstraintTeacherMaxBuildingChangesPerDay(double wp, const QString& tc, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxBuildingChangesPerDayInInterval: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxBuildingChangesPerDayInInterval)

public:
	//internal variables
	int teacher_ID;

	int maxBuildingChangesPerDay;

	QString teacherName;
	
	int intervalStart;
	int intervalEnd;

	ConstraintTeacherMaxBuildingChangesPerDayInInterval();

	ConstraintTeacherMaxBuildingChangesPerDayInInterval(double wp, const QString& tc, int mc, int is, int ie);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxBuildingChangesPerDayInInterval: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxBuildingChangesPerDayInInterval)

public:
	int maxBuildingChangesPerDay;

	int intervalStart;
	int intervalEnd;

	ConstraintTeachersMaxBuildingChangesPerDayInInterval();

	ConstraintTeachersMaxBuildingChangesPerDayInInterval(double wp, int mc, int is, int ie);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxBuildingChangesPerRealDayInInterval: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxBuildingChangesPerRealDayInInterval)

public:
	//internal variables
	int teacher_ID;

	int maxBuildingChangesPerDay;

	QString teacherName;
	
	int intervalStart;
	int intervalEnd;

	ConstraintTeacherMaxBuildingChangesPerRealDayInInterval();

	ConstraintTeacherMaxBuildingChangesPerRealDayInInterval(double wp, const QString& tc, int mc, int is, int ie);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxBuildingChangesPerRealDayInInterval: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxBuildingChangesPerRealDayInInterval)

public:
	int maxBuildingChangesPerDay;

	int intervalStart;
	int intervalEnd;

	ConstraintTeachersMaxBuildingChangesPerRealDayInInterval();

	ConstraintTeachersMaxBuildingChangesPerRealDayInInterval(double wp, int mc, int is, int ie);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxRoomChangesPerDayInInterval: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxRoomChangesPerDayInInterval)

public:
	//internal variables
	int teacher_ID;

	int maxRoomChangesPerDay;

	QString teacherName;
	
	int intervalStart;
	int intervalEnd;

	ConstraintTeacherMaxRoomChangesPerDayInInterval();

	ConstraintTeacherMaxRoomChangesPerDayInInterval(double wp, const QString& tc, int mc, int is, int ie);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxRoomChangesPerDayInInterval: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxRoomChangesPerDayInInterval)

public:
	int maxRoomChangesPerDay;

	int intervalStart;
	int intervalEnd;

	ConstraintTeachersMaxRoomChangesPerDayInInterval();

	ConstraintTeachersMaxRoomChangesPerDayInInterval(double wp, int mc, int is, int ie);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxRoomChangesPerRealDayInInterval: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxRoomChangesPerRealDayInInterval)

public:
	//internal variables
	int teacher_ID;

	int maxRoomChangesPerDay;

	QString teacherName;
	
	int intervalStart;
	int intervalEnd;

	ConstraintTeacherMaxRoomChangesPerRealDayInInterval();

	ConstraintTeacherMaxRoomChangesPerRealDayInInterval(double wp, const QString& tc, int mc, int is, int ie);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxRoomChangesPerRealDayInInterval: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxRoomChangesPerRealDayInInterval)

public:
	int maxRoomChangesPerDay;

	int intervalStart;
	int intervalEnd;

	ConstraintTeachersMaxRoomChangesPerRealDayInInterval();

	ConstraintTeachersMaxRoomChangesPerRealDayInInterval(double wp, int mc, int is, int ie);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxBuildingChangesPerDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxBuildingChangesPerDay)

public:

	int maxBuildingChangesPerDay;

	ConstraintTeachersMaxBuildingChangesPerDay();

	ConstraintTeachersMaxBuildingChangesPerDay(double wp, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxBuildingChangesPerWeek: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxBuildingChangesPerWeek)

public:
	//internal variables
	int teacher_ID;

	int maxBuildingChangesPerWeek;

	QString teacherName;

	ConstraintTeacherMaxBuildingChangesPerWeek();

	ConstraintTeacherMaxBuildingChangesPerWeek(double wp, const QString& tc, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxBuildingChangesPerWeek: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxBuildingChangesPerWeek)

public:

	int maxBuildingChangesPerWeek;

	ConstraintTeachersMaxBuildingChangesPerWeek();

	ConstraintTeachersMaxBuildingChangesPerWeek(double wp, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinGapsBetweenBuildingChanges: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinGapsBetweenBuildingChanges)

public:
	//internal variables
	int teacher_ID;

	int minGapsBetweenBuildingChanges;

	QString teacherName;

	ConstraintTeacherMinGapsBetweenBuildingChanges();

	ConstraintTeacherMinGapsBetweenBuildingChanges(double wp, const QString& tc, int mg);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinGapsBetweenBuildingChanges: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinGapsBetweenBuildingChanges)

public:

	int minGapsBetweenBuildingChanges;

	ConstraintTeachersMinGapsBetweenBuildingChanges();

	ConstraintTeachersMinGapsBetweenBuildingChanges(double wp, int mg);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//2019-11-14
class ConstraintStudentsSetMaxRoomChangesPerDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxRoomChangesPerDay)

public:
	//internal variables
	QList<int> iSubgroupsList;

	int maxRoomChangesPerDay;

	QString studentsName;

	ConstraintStudentsSetMaxRoomChangesPerDay();

	ConstraintStudentsSetMaxRoomChangesPerDay(double wp, const QString& st, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxRoomChangesPerDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxRoomChangesPerDay)

public:

	int maxRoomChangesPerDay;

	ConstraintStudentsMaxRoomChangesPerDay();

	ConstraintStudentsMaxRoomChangesPerDay(double wp, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxRoomChangesPerWeek: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxRoomChangesPerWeek)

public:
	//internal variables
	QList<int> iSubgroupsList;

	int maxRoomChangesPerWeek;

	QString studentsName;

	ConstraintStudentsSetMaxRoomChangesPerWeek();

	ConstraintStudentsSetMaxRoomChangesPerWeek(double wp, const QString& st, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxRoomChangesPerWeek: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxRoomChangesPerWeek)

public:

	int maxRoomChangesPerWeek;

	ConstraintStudentsMaxRoomChangesPerWeek();

	ConstraintStudentsMaxRoomChangesPerWeek(double wp, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMinGapsBetweenRoomChanges: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMinGapsBetweenRoomChanges)

public:
	//internal variables
	QList<int> iSubgroupsList;

	int minGapsBetweenRoomChanges;

	QString studentsName;

	ConstraintStudentsSetMinGapsBetweenRoomChanges();

	ConstraintStudentsSetMinGapsBetweenRoomChanges(double wp, const QString& st, int mg);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMinGapsBetweenRoomChanges: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMinGapsBetweenRoomChanges)

public:

	int minGapsBetweenRoomChanges;

	ConstraintStudentsMinGapsBetweenRoomChanges();

	ConstraintStudentsMinGapsBetweenRoomChanges(double wp, int mg);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxRoomChangesPerDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxRoomChangesPerDay)

public:
	//internal variables
	int teacher_ID;

	int maxRoomChangesPerDay;

	QString teacherName;

	ConstraintTeacherMaxRoomChangesPerDay();

	ConstraintTeacherMaxRoomChangesPerDay(double wp, const QString& tc, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxRoomChangesPerDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxRoomChangesPerDay)

public:

	int maxRoomChangesPerDay;

	ConstraintTeachersMaxRoomChangesPerDay();

	ConstraintTeachersMaxRoomChangesPerDay(double wp, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxRoomChangesPerWeek: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxRoomChangesPerWeek)

public:
	//internal variables
	int teacher_ID;

	int maxRoomChangesPerWeek;

	QString teacherName;

	ConstraintTeacherMaxRoomChangesPerWeek();

	ConstraintTeacherMaxRoomChangesPerWeek(double wp, const QString& tc, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxRoomChangesPerWeek: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxRoomChangesPerWeek)

public:

	int maxRoomChangesPerWeek;

	ConstraintTeachersMaxRoomChangesPerWeek();

	ConstraintTeachersMaxRoomChangesPerWeek(double wp, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMinGapsBetweenRoomChanges: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMinGapsBetweenRoomChanges)

public:
	//internal variables
	int teacher_ID;

	int minGapsBetweenRoomChanges;

	QString teacherName;

	ConstraintTeacherMinGapsBetweenRoomChanges();

	ConstraintTeacherMinGapsBetweenRoomChanges(double wp, const QString& tc, int mg);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMinGapsBetweenRoomChanges: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMinGapsBetweenRoomChanges)

public:

	int minGapsBetweenRoomChanges;

	ConstraintTeachersMinGapsBetweenRoomChanges();

	ConstraintTeachersMinGapsBetweenRoomChanges(double wp, int mg);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesOccupyMaxDifferentRooms: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesOccupyMaxDifferentRooms)

public:
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;
	
	int maxDifferentRooms;

	//internal variables
	QList<int> _activitiesIndices;

	ConstraintActivitiesOccupyMaxDifferentRooms();

	ConstraintActivitiesOccupyMaxDifferentRooms(double wp, const QList<int>& a_L, int max_rooms);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);
	
	void recomputeActivitiesSet();

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintActivitiesSameRoomIfConsecutive: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintActivitiesSameRoomIfConsecutive)

public:
	QList<int> activitiesIds;

	QSet<int> activitiesIdsSet;
	
	//internal variables
	QList<int> _activitiesIndices;

	ConstraintActivitiesSameRoomIfConsecutive();

	ConstraintActivitiesSameRoomIfConsecutive(double wp, const QList<int>& a_L);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	void removeUseless(Rules& r);

	void recomputeActivitiesSet();

	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxRoomChangesPerRealDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxRoomChangesPerRealDay)

public:

	int maxRoomChangesPerDay;

	ConstraintStudentsMaxRoomChangesPerRealDay();

	ConstraintStudentsMaxRoomChangesPerRealDay(double wp, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxRoomChangesPerRealDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxRoomChangesPerRealDay)

public:
	//internal variables
	QList<int> iSubgroupsList;

	int maxRoomChangesPerDay;

	QString studentsName;

	ConstraintStudentsSetMaxRoomChangesPerRealDay();

	ConstraintStudentsSetMaxRoomChangesPerRealDay(double wp, const QString& st, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxRoomChangesPerRealDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxRoomChangesPerRealDay)

public:
	//internal variables
	int teacher_ID;

	int maxRoomChangesPerDay;

	QString teacherName;

	ConstraintTeacherMaxRoomChangesPerRealDay();

	ConstraintTeacherMaxRoomChangesPerRealDay(double wp, const QString& tc, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxRoomChangesPerRealDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxRoomChangesPerRealDay)

public:

	int maxRoomChangesPerDay;

	ConstraintTeachersMaxRoomChangesPerRealDay();

	ConstraintTeachersMaxRoomChangesPerRealDay(double wp, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsMaxBuildingChangesPerRealDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsMaxBuildingChangesPerRealDay)

public:

	int maxBuildingChangesPerDay;

	ConstraintStudentsMaxBuildingChangesPerRealDay();

	ConstraintStudentsMaxBuildingChangesPerRealDay(double wp, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintStudentsSetMaxBuildingChangesPerRealDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintStudentsSetMaxBuildingChangesPerRealDay)

public:
	//internal variables
	QList<int> iSubgroupsList;

	int maxBuildingChangesPerDay;

	QString studentsName;

	ConstraintStudentsSetMaxBuildingChangesPerRealDay();

	ConstraintStudentsSetMaxBuildingChangesPerRealDay(double wp, const QString& st, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeacherMaxBuildingChangesPerRealDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeacherMaxBuildingChangesPerRealDay)

public:
	//internal variables
	int teacher_ID;

	int maxBuildingChangesPerDay;

	QString teacherName;

	ConstraintTeacherMaxBuildingChangesPerRealDay();

	ConstraintTeacherMaxBuildingChangesPerRealDay(double wp, const QString& tc, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	QString getXmlDescription(Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintTeachersMaxBuildingChangesPerRealDay: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintTeachersMaxBuildingChangesPerRealDay)

public:

	int maxBuildingChangesPerDay;

	ConstraintTeachersMaxBuildingChangesPerRealDay();

	ConstraintTeachersMaxBuildingChangesPerRealDay(double wp, int mc);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);

	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);

	bool isRelatedToActivity(Activity* a);

	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);

	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintRoomMaxActivityTagsPerDayFromSet: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintRoomMaxActivityTagsPerDayFromSet)
	
public:

	/**
	The room's name
	*/
	QString room;

	/**
	The room's id, or index in the rules
	*/
	int room_ID;
	
	int maxTags;
	
	QList<QString> tagsList;
	
	QSet<int> internalTagsSet;

	ConstraintRoomMaxActivityTagsPerDayFromSet();

	ConstraintRoomMaxActivityTagsPerDayFromSet(double wp, const QString& rn, int mtg, const QList<QString>& tgl);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintRoomMaxActivityTagsPerRealDayFromSet: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintRoomMaxActivityTagsPerRealDayFromSet)
	
public:

	/**
	The room's name
	*/
	QString room;

	/**
	The room's id, or index in the rules
	*/
	int room_ID;
	
	int maxTags;
	
	QList<QString> tagsList;
	
	QSet<int> internalTagsSet;

	ConstraintRoomMaxActivityTagsPerRealDayFromSet();

	ConstraintRoomMaxActivityTagsPerRealDayFromSet(double wp, const QString& rn, int mtg, const QList<QString>& tgl);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

class ConstraintRoomMaxActivityTagsPerWeekFromSet: public SpaceConstraint{
	Q_DECLARE_TR_FUNCTIONS(ConstraintRoomMaxActivityTagsPerWeekFromSet)
	
public:

	/**
	The room's name
	*/
	QString room;

	/**
	The room's id, or index in the rules
	*/
	int room_ID;
	
	int maxTags;
	
	QList<QString> tagsList;
	
	QSet<int> internalTagsSet;

	ConstraintRoomMaxActivityTagsPerWeekFromSet();

	ConstraintRoomMaxActivityTagsPerWeekFromSet(double wp, const QString& rn, int mtg, const QList<QString>& tgl);

	bool computeInternalStructure(QWidget* parent, Rules& r);

	bool hasInactiveActivities(Rules& r);
	
	QString getXmlDescription(Rules& r);

	QString getDescription(Rules& r);

	QString getDetailedDescription(Rules& r);

	double fitness(Solution& c, Rules& r, QList<double>& cl, QList<QString>& dl, FakeString* conflictsString=nullptr);
	
	bool isRelatedToActivity(Activity* a);
	
	bool isRelatedToTeacher(Teacher* t);

	bool isRelatedToSubject(Subject* s);

	bool isRelatedToActivityTag(ActivityTag* s);
	
	bool isRelatedToStudentsSet(Rules& r, StudentsSet* s);

	bool isRelatedToRoom(Room* r);

	bool hasWrongDayOrHour(Rules& r);
	bool canRepairWrongDayOrHour(Rules& r);
	bool repairWrongDayOrHour(Rules& r);
};

//1
QDataStream& operator<<(QDataStream& stream, const ConstraintBasicCompulsorySpace& sc);
//2
QDataStream& operator<<(QDataStream& stream, const ConstraintRoomNotAvailableTimes& sc);
//3
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityPreferredRoom& sc);
//4
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityPreferredRooms& sc);
//5
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetHomeRoom& sc);
//6
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetHomeRooms& sc);
//7
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherHomeRoom& sc);
//8
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherHomeRooms& sc);
//9
QDataStream& operator<<(QDataStream& stream, const ConstraintSubjectPreferredRoom& sc);
//10
QDataStream& operator<<(QDataStream& stream, const ConstraintSubjectPreferredRooms& sc);
//11
QDataStream& operator<<(QDataStream& stream, const ConstraintSubjectActivityTagPreferredRoom& sc);
//12
QDataStream& operator<<(QDataStream& stream, const ConstraintSubjectActivityTagPreferredRooms& sc);
//13
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxBuildingChangesPerDay& sc);
//14
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxBuildingChangesPerDay& sc);
//15
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxBuildingChangesPerWeek& sc);
//16
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxBuildingChangesPerWeek& sc);
//17
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinGapsBetweenBuildingChanges& sc);
//18
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinGapsBetweenBuildingChanges& sc);
//19
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxBuildingChangesPerDay& sc);
//20
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxBuildingChangesPerDay& sc);
//21
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxBuildingChangesPerWeek& sc);
//22
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxBuildingChangesPerWeek& sc);
//23
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinGapsBetweenBuildingChanges& sc);
//24
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinGapsBetweenBuildingChanges& sc);
//25
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityTagPreferredRoom& sc);
//26
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityTagPreferredRooms& sc);
//27
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesOccupyMaxDifferentRooms& sc);
//28
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesSameRoomIfConsecutive& sc);
//29
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxRoomChangesPerDay& sc);
//30
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxRoomChangesPerDay& sc);
//31
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxRoomChangesPerWeek& sc);
//32
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxRoomChangesPerWeek& sc);
//33
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinGapsBetweenRoomChanges& sc);
//34
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinGapsBetweenRoomChanges& sc);
//35
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxRoomChangesPerDay& sc);
//36
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxRoomChangesPerDay& sc);
//37
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxRoomChangesPerWeek& sc);
//38
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxRoomChangesPerWeek& sc);
//39
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinGapsBetweenRoomChanges& sc);
//40
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinGapsBetweenRoomChanges& sc);
//41
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherRoomNotAvailableTimes& sc);
//42
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxRoomChangesPerRealDay& sc);
//43
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxRoomChangesPerRealDay& sc);
//44
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxRoomChangesPerRealDay& sc);
//45
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxRoomChangesPerRealDay& sc);
//46
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxBuildingChangesPerRealDay& sc);
//47
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxBuildingChangesPerRealDay& sc);
//48
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxBuildingChangesPerRealDay& sc);
//49
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxBuildingChangesPerRealDay& sc);
//50
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxBuildingChangesPerDayInInterval& sc);
//51
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxBuildingChangesPerDayInInterval& sc);
//52
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxBuildingChangesPerDayInInterval& sc);
//53
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxBuildingChangesPerDayInInterval& sc);
//54
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxBuildingChangesPerRealDayInInterval& sc);
//55
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval& sc);
//56
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxBuildingChangesPerRealDayInInterval& sc);
//57
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxBuildingChangesPerRealDayInInterval& sc);
//58
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxRoomChangesPerDayInInterval& sc);
//59
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxRoomChangesPerDayInInterval& sc);
//60
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxRoomChangesPerDayInInterval& sc);
//61
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxRoomChangesPerDayInInterval& sc);
//62
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxRoomChangesPerRealDayInInterval& sc);
//63
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval& sc);
//64
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxRoomChangesPerRealDayInInterval& sc);
//65
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxRoomChangesPerRealDayInInterval& sc);
//66
QDataStream& operator<<(QDataStream& stream, const ConstraintRoomMaxActivityTagsPerDayFromSet& sc);
//67
QDataStream& operator<<(QDataStream& stream, const ConstraintRoomMaxActivityTagsPerRealDayFromSet& sc);
//68
QDataStream& operator<<(QDataStream& stream, const ConstraintRoomMaxActivityTagsPerWeekFromSet& sc);

//1
QDataStream& operator>>(QDataStream& stream, ConstraintBasicCompulsorySpace& sc);
//2
QDataStream& operator>>(QDataStream& stream, ConstraintRoomNotAvailableTimes& sc);
//3
QDataStream& operator>>(QDataStream& stream, ConstraintActivityPreferredRoom& sc);
//4
QDataStream& operator>>(QDataStream& stream, ConstraintActivityPreferredRooms& sc);
//5
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetHomeRoom& sc);
//6
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetHomeRooms& sc);
//7
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherHomeRoom& sc);
//8
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherHomeRooms& sc);
//9
QDataStream& operator>>(QDataStream& stream, ConstraintSubjectPreferredRoom& sc);
//10
QDataStream& operator>>(QDataStream& stream, ConstraintSubjectPreferredRooms& sc);
//11
QDataStream& operator>>(QDataStream& stream, ConstraintSubjectActivityTagPreferredRoom& sc);
//12
QDataStream& operator>>(QDataStream& stream, ConstraintSubjectActivityTagPreferredRooms& sc);
//13
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxBuildingChangesPerDay& sc);
//14
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxBuildingChangesPerDay& sc);
//15
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxBuildingChangesPerWeek& sc);
//16
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxBuildingChangesPerWeek& sc);
//17
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinGapsBetweenBuildingChanges& sc);
//18
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinGapsBetweenBuildingChanges& sc);
//19
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxBuildingChangesPerDay& sc);
//20
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxBuildingChangesPerDay& sc);
//21
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxBuildingChangesPerWeek& sc);
//22
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxBuildingChangesPerWeek& sc);
//23
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinGapsBetweenBuildingChanges& sc);
//24
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinGapsBetweenBuildingChanges& sc);
//25
QDataStream& operator>>(QDataStream& stream, ConstraintActivityTagPreferredRoom& sc);
//26
QDataStream& operator>>(QDataStream& stream, ConstraintActivityTagPreferredRooms& sc);
//27
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesOccupyMaxDifferentRooms& sc);
//28
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesSameRoomIfConsecutive& sc);
//29
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxRoomChangesPerDay& sc);
//30
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxRoomChangesPerDay& sc);
//31
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxRoomChangesPerWeek& sc);
//32
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxRoomChangesPerWeek& sc);
//33
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinGapsBetweenRoomChanges& sc);
//34
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinGapsBetweenRoomChanges& sc);
//35
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxRoomChangesPerDay& sc);
//36
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxRoomChangesPerDay& sc);
//37
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxRoomChangesPerWeek& sc);
//38
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxRoomChangesPerWeek& sc);
//39
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinGapsBetweenRoomChanges& sc);
//40
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinGapsBetweenRoomChanges& sc);
//41
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherRoomNotAvailableTimes& sc);
//42
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxRoomChangesPerRealDay& sc);
//43
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxRoomChangesPerRealDay& sc);
//44
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxRoomChangesPerRealDay& sc);
//45
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxRoomChangesPerRealDay& sc);
//46
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxBuildingChangesPerRealDay& sc);
//47
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxBuildingChangesPerRealDay& sc);
//48
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxBuildingChangesPerRealDay& sc);
//49
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxBuildingChangesPerRealDay& sc);
//50
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxBuildingChangesPerDayInInterval& sc);
//51
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxBuildingChangesPerDayInInterval& sc);
//52
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxBuildingChangesPerDayInInterval& sc);
//53
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxBuildingChangesPerDayInInterval& sc);
//54
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxBuildingChangesPerRealDayInInterval& sc);
//55
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval& sc);
//56
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxBuildingChangesPerRealDayInInterval& sc);
//57
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxBuildingChangesPerRealDayInInterval& sc);
//58
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxRoomChangesPerDayInInterval& sc);
//59
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxRoomChangesPerDayInInterval& sc);
//60
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxRoomChangesPerDayInInterval& sc);
//61
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxRoomChangesPerDayInInterval& sc);
//62
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxRoomChangesPerRealDayInInterval& sc);
//63
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval& sc);
//64
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxRoomChangesPerRealDayInInterval& sc);
//65
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxRoomChangesPerRealDayInInterval& sc);
//66
QDataStream& operator>>(QDataStream& stream, ConstraintRoomMaxActivityTagsPerDayFromSet& sc);
//67
QDataStream& operator>>(QDataStream& stream, ConstraintRoomMaxActivityTagsPerRealDayFromSet& sc);
//68
QDataStream& operator>>(QDataStream& stream, ConstraintRoomMaxActivityTagsPerWeekFromSet& sc);

#endif
