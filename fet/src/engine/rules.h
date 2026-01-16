/*
File rules.h
*/

/***************************************************************************
                          rules.h  -  description
                             -------------------
    begin                : 2003
    copyright            : (C) 2003 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef RULES_H
#define RULES_H

#include "timetable_defs.h"
#include "timeconstraint.h"
#include "spaceconstraint.h"
#include "activity.h"
#include "studentsset.h"
#include "teacher.h"
#include "subject.h"
#include "activitytag.h"
#include "room.h"
#include "building.h"

#include "matrix.h"

#include <QHash>
#include <QSet>
#include <QList>
#include <QStringList>
#include <QString>

#include <QCoreApplication>

#include <QByteArray>

#include <QPair>

#include <list>

class QXmlStreamReader;

class QWidget;

//If you change any of these const int-s, you need to update the const QString FET_DATA_FORMAT_VERSION from timetable_defs.cpp to a new value,
//because of the disk history feature.
const int OFFICIAL=0;
const int MORNINGS_AFTERNOONS=1;
const int BLOCK_PLANNING=2;
const int TERMS=3;

class FakeString
/*
Fake string, so that the output log is not too large
*/
{
public:
	FakeString();

	void operator=(const QString& other);
	void operator=(const char* str);
	void operator+=(const QString& other);
	void operator+=(const char* str);
};

class QDataStream;
class Rules;

QDataStream& operator<<(QDataStream& stream, const Rules& rules);
QDataStream& operator>>(QDataStream& stream, Rules& rules);

class QDate;
class QTime;

/**
This class contains all the information regarding
the institution: teachers, students, activities, constraints, etc.
*/
class Rules{
	Q_DECLARE_TR_FUNCTIONS(Rules)

public:
#ifndef FET_COMMAND_LINE
	void addUndoPoint(const QString& description, bool autosave=true, bool resetCounter=false);
	void restoreState(QWidget* parent, int iterationsBackward); //iterationsBackward<0 means Redo, >0 means Undo, and ==0 is not allowed
#endif

	void recomputeActivitiesSetForTimeConstraint(TimeConstraint* ctr);
	void insertTimeConstraintInHash(TimeConstraint* ctr);
	void recomputeActivitiesSetForSpaceConstraint(SpaceConstraint* ctr);
	void insertSpaceConstraintInHash(SpaceConstraint* ctr);

	int mode;

	bool modified;

	/**
	The name of the institution
	*/
	QString institutionName;
	
	/**
	The comments
	*/
	QString comments;

	/**
	The number of days per week
	*/
	int nDaysPerWeek;

	/**
	The number of hours per day
	*/
	int nHoursPerDay;

	/**
	The days of the week (names)
	*/
	//QString daysOfTheWeek[MAX_DAYS_PER_WEEK];
	QStringList daysOfTheWeek;
	QStringList daysOfTheWeek_longNames;

	/**
	The hours of the day (names).
	*/
	//QString hoursOfTheDay[MAX_HOURS_PER_DAY];
	QStringList hoursOfTheDay;
	QStringList hoursOfTheDay_longNames;

	//For the Mornings-Afternoons mode
	int nRealDaysPerWeek;
	int nRealHoursPerDay;
	QStringList realDaysOfTheWeek;
	QStringList realHoursOfTheDay;
	QStringList realDaysOfTheWeek_longNames;
	QStringList realHoursOfTheDay_longNames;

	/**
	The number of hours per week
	*/
	int nHoursPerWeek;
	
	int nTerms; //for terms (Finland) mode
	
	int nDaysPerTerm; //for terms (Finland) mode

	/**
	The list of teachers
	*/
	TeachersList teachersList;

	/**
	The list of subjects
	*/
	SubjectsList subjectsList;

	/**
	The list of activity tags
	*/
	ActivityTagsList activityTagsList;

	/**
	The list of students (groups and subgroups included).
	Remember that every identifier (year, group or subgroup) must be UNIQUE.
	*/
	StudentsYearsList yearsList;

	/**
	The list of activities
	*/
	ActivitiesList activitiesList;

	/**
	The list of rooms
	*/
	RoomsList roomsList;

	/**
	The list of buildings
	*/
	BuildingsList buildingsList;

	/**
	The list of time constraints
	*/
	TimeConstraintsList timeConstraintsList;

	/**
	The list of space constraints
	*/
	SpaceConstraintsList spaceConstraintsList;
	
	GroupActivitiesInInitialOrderList groupActivitiesInInitialOrderList;
	
	//For faster operation
	//not internal, but based on activity id / teacher name / students set name and constraints list
	QHash<int, Activity*> activitiesPointerHash; //first is the id, second is the pointer to the activity in Rules::activitiesList
	QSet<ConstraintBasicCompulsoryTime*> bctSet;
	QSet<ConstraintBreakTimes*> btSet;
	QSet<ConstraintBasicCompulsorySpace*> bcsSet;
	QHash<int, QSet<ConstraintActivityPreferredStartingTime*>> apstHash;
	QHash<int, QSet<ConstraintActivityPreferredDay*>> apdHash;
	QHash<int, QSet<ConstraintActivityPreferredRoom*>> aprHash;
	QHash<int, QSet<ConstraintMinDaysBetweenActivities*>> mdbaHash;
	QHash<int, QSet<ConstraintMinHalfDaysBetweenActivities*>> mhdbaHash;
	QHash<QString, QSet<ConstraintTeacherNotAvailableTimes*>> tnatHash;
	QHash<QString, QSet<ConstraintStudentsSetNotAvailableTimes*>> ssnatHash;
	
	//not internal
	QHash<QString, StudentsSet*> permanentStudentsHash;
	
	//internal
	QHash<QString, int> teachersHash;
	QHash<QString, int> subjectsHash;
	QHash<QString, int> activityTagsHash;
	QHash<QString, StudentsSet*> studentsHash;
	QHash<QString, int> buildingsHash;
	QHash<QString, int> roomsHash;
	QHash<int, int> activitiesHash; //first is id, second is index in internal list
	//using activity index in internal activities
	/*QHash<QString, QSet<int>> activitiesForTeacherHash;
	QHash<QString, QSet<int>> activitiesForSubjectHash;
	QHash<QString, QSet<int>> activitiesForActivityTagHash;
	QHash<QString, QSet<int>> activitiesForStudentsSetHash;*/

	/*
	The following variables contain redundant data and are used internally
	*/
	////////////////////////////////////////////////////////////////////////
	int nInternalTeachers;
	Matrix1D<Teacher*> internalTeachersList;

	int nInternalSubjects;
	Matrix1D<Subject*> internalSubjectsList;

	int nInternalActivityTags;
	Matrix1D<ActivityTag*> internalActivityTagsList;

	int nInternalSubgroups;
	Matrix1D<StudentsSubgroup*> internalSubgroupsList;
	
	StudentsGroupsList internalGroupsList;
	
	StudentsYearsList augmentedYearsList;

	/**
	Here will be only the active activities.
	
	For speed, I used here not pointers, but static copies. (old comment.)
	*/
	int nInternalActivities;
	Matrix1D<Activity> internalActivitiesList;
	
	QSet<int> inactiveActivities;
	
	Matrix1D<QList<int>> activitiesForSubjectList;
	Matrix1D<QSet<int>> activitiesForSubjectSet;
	Matrix1D<QList<int>> activitiesForActivityTagList;
	Matrix1D<QSet<int>> activitiesForActivityTagSet;

	int nInternalRooms;
	Matrix1D<Room*> internalRoomsList;

	int nInternalBuildings;
	Matrix1D<Building*> internalBuildingsList;

	int nInternalTimeConstraints;
	Matrix1D<TimeConstraint*> internalTimeConstraintsList;

	int nInternalSpaceConstraints;
	Matrix1D<SpaceConstraint*> internalSpaceConstraintsList;

	/*
	///////////////////////////////////////////////////////////////////////
	*/

	/**
	True if the rules have been initialized in some way (new or loaded).
	*/
	bool initialized;

	/**
	True if the internal structure was computed.
	*/
	bool internalStructureComputed;

	/**
	Initializes the rules (empty)
	*/
	void init();

	/**
	Internal structure initializer.
	<p>
	After any modification of the activities or students or teachers
	or constraints, you need to call this subroutine
	*/
	bool computeInternalStructure(QWidget* parent);

	/**
	Terminator - basically clears the memory for the constraints.
	*/
	void clear();

	Rules();

	~Rules();
	
	void setMode(int newMode);
	
	void setTerms(int numberOfTerms, int numberOfDaysPerTerm);
	
	void setInstitutionName(const QString& newInstitutionName);
	
	void setComments(const QString& newComments);

	/**
	Adds a new teacher
	(if not already in the list).
	Returns false/true (unsuccessful/successful).
	*/
	bool addTeacher(Teacher* teacher);

	/*when reading rules, faster*/
	bool addTeacherFast(Teacher* teacher);

	/**
	Returns the index of this teacher in the teachersList,
	or -1 for nonexistent teacher.
	*/
	int searchTeacher(const QString& teacherName);

	/**
	Removes this teacher and all related activities and constraints.
	It returns false on failure. If successful, returns true.
	*/
	bool removeTeacher(const QString& teacherName);

	/**
	Modifies (renames) this teacher and takes care of all related activities and constraints.
	Returns true on success, false on failure (if not found)
	*/
	bool modifyTeacher(const QString& initialTeacherName, const QString& finalTeacherName);

	/**
	A function to sort the teachers alphabetically
	*/
	void sortTeachersAlphabetically();

	/**
	Adds a new subject
	(if not already in the list).
	Returns false/true (unsuccessful/successful).
	*/
	bool addSubject(Subject* subject);

	/*
	When reading rules, faster
	*/
	bool addSubjectFast(Subject* subject);

	/**
	Returns the index of this subject in the subjectsList,
	or -1 if not found.
	*/
	int searchSubject(const QString& subjectName);

	/**
	Removes this subject and all related activities and constraints.
	It returns false on failure.
	If successful, returns true.
	*/
	bool removeSubject(const QString& subjectName);

	/**
	Modifies (renames) this subject and takes care of all related activities and constraints.
	Returns true on success, false on failure (if not found)
	*/
	bool modifySubject(const QString& initialSubjectName, const QString& finalSubjectName);

	/**
	A function to sort the subjects alphabetically
	*/
	void sortSubjectsAlphabetically();

	/**
	Adds a new activity tag to the list of activity tags
	(if not already in the list).
	Returns false/true (unsuccessful/successful).
	*/
	bool addActivityTag(ActivityTag* activityTag);

	/*
	When reading rules, faster
	*/
	bool addActivityTagFast(ActivityTag* activityTag);

	/**
	Returns the index of this activity tag in the activityTagsList,
	or -1 if not found.
	*/
	int searchActivityTag(const QString& activityTagName);

	/**
	Removes this activity tag. In the list of activities, the activity tag will
	be removed from all activities which posess it.
	It returns false on failure.
	If successful, returns true.
	*/
	bool removeActivityTag(const QString& activityTagName);

	/**
	Modifies (renames) this activity tag and takes care of all related activities.
	Returns true on success, false on failure (if not found)
	*/
	bool modifyActivityTag(const QString& initialActivityTagName, const QString& finalActivityTagName);

	/**
	A function to sort the activity tags alphabetically
	*/
	void sortActivityTagsAlphabetically();
	
	void computePermanentStudentsHash();

	/**
	Returns a pointer to the structure containing this student set
	(year, group or subgroup) or nullptr.
	*/
	StudentsSet* searchStudentsSet(const QString& setName);
	
	//StudentsSet* searchAugmentedStudentsSet(const QString& setName);
	
	/**
	True if the students sets contain one common subgroup.
	This function is used in constraints isRelatedToStudentsSet
	*/
	bool setsShareStudents(const QString& studentsSet1, const QString& studentsSet2);

	//Internal
	bool augmentedSetsShareStudentsFaster(const QString& studentsSet1, const QString& studentsSet2);

	/**
	Adds a new year of study to the academic structure
	*/
	bool addYear(StudentsYear* year);
	
	/*
	When reading rules, faster
	*/
	bool addYearFast(StudentsYear* year);

//	bool emptyYear(const QString& yearName);
	bool removeYear(const QString& yearName);
//	bool removeYear(const QString& yearName, bool removeAlsoThisYear);
	
	bool removeYearPointerAfterSplit(StudentsYear* yearPointer);

	/**
	Returns -1 if not found or the index of this year in the years list
	*/
	int searchYear(const QString& yearName);

	int searchAugmentedYear(const QString& yearName);

	/**
	Modifies this students set (name, number of students) and takes care of all related
	activities and constraints. Returns true on success, false on failure (if not found)
	*/
	bool modifyStudentsSet(const QString& initialStudentsSetName, const QString& finalStudentsSetName, int finalNumberOfStudents);
	
	bool modifyStudentsSets(const QHash<QString, QString>& oldAndNewStudentsSetNames);
	
	/**
	A function to sort the years alphabetically
	*/
	void sortYearsAlphabetically();

	/**
	Adds a new group in a certain year of study to the academic structure
	*/
	bool addGroup(const QString& yearName, StudentsGroup* group);
	
	/*
	When reading rules, faster
	*/
	bool addGroupFast(StudentsYear* year, StudentsGroup* group);

	bool removeGroup(const QString& yearName, const QString& groupName);

	//Remove this group from all the years in which it exists
	bool purgeGroup(const QString& groupName);

	/**
	Returns -1 if not found or the index of this group in the groups list
	of this year.
	*/
	int searchGroup(const QString& yearName, const QString& groupName);

	int searchAugmentedGroup(const QString& yearName, const QString& groupName);

	/**
	A function to sort the groups of this year alphabetically
	*/
	void sortGroupsAlphabetically(const QString& yearName);

	/**
	Adds a new subgroup to a certain group in a certain year of study to
	the academic structure
	*/
	bool addSubgroup(const QString& yearName, const QString& groupName, StudentsSubgroup* subgroup);

	/*
	When reading rules, faster
	*/
	bool addSubgroupFast(StudentsYear* year, StudentsGroup* group, StudentsSubgroup* subgroup);

	bool removeSubgroup(const QString& yearName, const QString& groupName, const QString& subgroupName);

	//Remove this subgroup from all the groups in which it exists
	bool purgeSubgroup(const QString& subgroupName);

	/**
	Returns -1 if not found or the index of the subgroup in the list of subgroups of this group
	*/
	int searchSubgroup(const QString& yearName, const QString& groupName, const QString& subgroupName);

	int searchAugmentedSubgroup(const QString& yearName, const QString& groupName, const QString& subgroupName);

	/**
	A function to sort the subgroups of this group alphabetically
	*/
	void sortSubgroupsAlphabetically(const QString& yearName, const QString& groupName);
	
	/**
	Adds a new indivisible activity (not split) to the list of activities.
	(It can add a subactivity of a split activity)
	Returns true if successful or false if the maximum
	number of activities was reached.
	*/
	/*bool addSimpleActivity(
		QWidget* parent,
		int _id,
		int _activityGroupId,
		const QStringList& _teachersNames,
		const QString& _subjectName,
		const QStringList& _activityTagsNames,
		const QStringList& _studentsNames,
		int _duration,
		int _totalDuration,
		bool _active,
		bool _computeNTotalStudents,
		int _nTotalStudents);*/

	/*
	Faster (no need to recompute the number of students in activity constructor)
	*/
	bool addSimpleActivityFast(
		QWidget* parent,
		int _id,
		int _activityGroupId,
		const QStringList& _teachersNames,
		const QString& _subjectName,
		const QStringList& _activityTagsNames,
		const QStringList& _studentsNames,
		int _duration,
		int _totalDuration,
		bool _active,
		bool _computeNTotalStudents,
		int _nTotalStudents,
		int _computedNumberOfStudents);

	/**
	Adds a new split activity to the list of activities.
	Returns true if successful or false if the maximum
	number of activities was reached.
	If _minDayDistance>0, there will automatically added a compulsory
	ConstraintMinDaysBetweenActivities.
	*/
	/*bool addSplitActivity(
		QWidget* parent,
		int _firstActivityId,
		int _activityGroupId,
		const QStringList& _teachersNames,
		const QString& _subjectName,
		const QStringList& _activityTagsNames,
		const QStringList& _studentsNames,
		int _nSplits,
		int _totalDuration,
		int _durations[],
		bool _active[],
		int _minDayDistance,
		double _weightPercentage,
		bool _consecutiveIfSameDay,
		bool _computeNTotalStudents,
		int _nTotalStudents);*/

	bool addSplitActivityFast(
		QWidget* parent,
		int _firstActivityId,
		int _activityGroupId,
		const QStringList& _teachersNames,
		const QString& _subjectName,
		const QStringList& _activityTagsNames,
		const QStringList& _studentsNames,
		int _nSplits,
		int _totalDuration,
		const QList<int>& _durations,
		const QList<bool>& _active,
		int _minDayDistance,
		double _weightPercentage,
		bool _consecutiveIfSameDay,
		bool _computeNTotalStudents,
		int _nTotalStudents,
		int _computedNumberOfStudents,
		bool _halfDays=false);

	bool addSplitActivityFastWithComponents(
		QWidget* parent,
		int _firstActivityId,
		int _activityGroupId,
		const QList<QStringList>& _teachersNames,
		const QList<QString>& _subjectName,
		const QList<QStringList>& _activityTagsNames,
		const QList<QStringList>& _studentsNames,
		int _nSplits,
		int _totalDuration,
		const QList<int>& _durations,
		const QList<bool>& _active,
		int _minDayDistance,
		double _weightPercentage,
		bool _consecutiveIfSameDay,
		bool _computeNTotalStudents,
		int _nTotalStudents,
		const QList<int>& _computedNumberOfStudents,
		bool _halfDays=false);

	/**
	Removes only the activity with this id.
	*/
	//void removeActivity(int _id);

	/**
	If _activityGroupId==0, then this is a non-split activity
	(if >0, then this is a single subactivity from a split activity.
	Removes this activity from the list of activities.
	For split activities, it removes all the subactivities that are contained in it.
	*/
	void removeActivity(int _id, int _activityGroupId);
	
	void removeActivities(const QList<int>& _idsList, bool updateConstraints);
	
	/**
	A function to modify the information of a certain activity.
	If this is a subactivity of a split activity,
	all the subactivities will be modified.
	*/
	void modifyActivity(
		int _id,
		int _activityGroupId,
		const QStringList& _teachersNames,
		const QString& _subjectName,
		const QStringList& _activityTagsNames,
		const QStringList& _studentsNames,
	 	int _nSplits,
		int _totalDuration,
		const QList<int>& _durations,
		const QList<bool>& _active,
		bool _computeNTotalStudents,
		int _nTotalStudents);

	void modifySubactivity(
		int _id,
		int _activityGroupId,
		const QStringList& _teachersNames,
		const QString& _subjectName,
		const QStringList& _activityTagsNames,
		const QStringList& _studentsNames,
		int _duration,
		bool _active,
		bool _computeNTotalStudents,
		int _nTotalStudents);

	/**
	Adds a new room (already allocated).
	Returns true on success, false for already existing rooms (same name).
	*/
	bool addRoom(Room* rm);

	/*
	Faster, when reading
	*/
	bool addRoomFast(Room* rm);

	/**
	Returns -1 if not found or the index in the rooms list if found.
	*/
	int searchRoom(const QString& roomName);

	/**
	Removes the room with this name.
	Returns true on success, false on failure (not found).
	*/
	bool removeRoom(const QString& roomName);
	
	/**
	Modifies this room and takes care of all related constraints.
	Returns true on success, false on failure (if not found)
	*/
	bool modifyRoom(const QString& initialRoomName, const QString& finalRoomName, const QString& building, int capacity);

	/**
	A function to sort the room alphabetically, by name
	*/
	void sortRoomsAlphabetically();

	/**
	Adds a new building (already allocated).
	Returns true on success, false for already existing buildings (same name).
	*/
	bool addBuilding(Building* rm);

	/*
	Faster, when reading
	*/
	bool addBuildingFast(Building* rm);

	/**
	Returns -1 if not found or the index in the buildings list if found.
	*/
	int searchBuilding(const QString& buildingName);

	/**
	Removes the building with this name.
	Returns true on success, false on failure (not found).
	*/
	bool removeBuilding(const QString& buildingName);
	
	/**
	Modifies this building and takes care of all related constraints.
	Returns true on success, false on failure (if not found)
	*/
	bool modifyBuilding(const QString& initialBuildingName, const QString& finalBuildingName);

	/**
	A function to sort the buildings alphabetically, by name
	*/
	void sortBuildingsAlphabetically();

	/**
	Adds a new time constraint (already allocated).
	Returns true on success, false for already existing constraints.
	*/
	bool addTimeConstraint(TimeConstraint* ctr);

	/**
	Removes this time constraint.
	Returns true on success, false on failure (not found).
	*/
	bool removeTimeConstraint(TimeConstraint* ctr);

	/**
	Adds a new space constraint (already allocated).
	Returns true on success, false for already existing constraints.
	*/
	bool addSpaceConstraint(SpaceConstraint* ctr);
	
	/**
	Removes this space constraint.
	Returns true on success, false on failure (not found).
	*/
	bool removeSpaceConstraint(SpaceConstraint* ctr);
	
	bool removeTimeConstraints(const QList<TimeConstraint*>& _tcl);
	bool removeSpaceConstraints(const QList<SpaceConstraint*>& _scl);

	int xmlReaderNumberOfUnrecognizedFields;

	QList<QString> unrecognizedXmlTags;
	QList<int> unrecognizedXmlLineNumbers;
	QList<int> unrecognizedXmlColumnNumbers;

	/**
	Reads the rules from the XML data file.
	Returns true on success, false on failure (nonexistent file or wrong format)
	*/
	bool read(QWidget* parent, const QString& fileName, bool commandLine=false, const QString& commandLineDirectory=QString());

	/**
	Write the rules to the XML data file.
	*/
	bool write(QWidget* parent, const QString& filename);
	
	int activateTeacher(const QString& teacherName);
	
	int activateStudents(const QString& studentsName);
	
	int activateSubject(const QString& subjectName);
	
	int activateActivityTag(const QString& activityTagName);

	int deactivateTeacher(const QString& teacherName);
	
	int deactivateStudents(const QString& studentsName);
	
	int deactivateSubject(const QString& subjectName);
	
	int deactivateActivityTag(const QString& activityTagName);
	
	bool makeActivityTagPrintable(const QString& activityTagName);
	bool makeActivityTagNotPrintable(const QString& activityTagName);
	
	void updateActivitiesWhenRemovingStudents(const QSet<StudentsSet*>& studentsSets, bool updateConstraints);
	void updateGroupActivitiesInInitialOrderAfterRemoval();
	void updateConstraintsAfterRemoval();
	
private:
	TimeConstraint* readBasicCompulsoryTime(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherNotAvailable(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherNotAvailableTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMaxDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherNoTwoConsecutiveDays(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersNoTwoConsecutiveDays(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMaxThreeConsecutiveDays(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxThreeConsecutiveDays(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMaxThreeConsecutiveDays(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxThreeConsecutiveDays(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeachersMaxDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMinDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMinDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetNotAvailable(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetNotAvailableTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readMinNDaysBetweenActivities(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readMinDaysBetweenActivities(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readMinHalfDaysBetweenActivities(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readMaxDaysBetweenActivities(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readMaxHalfDaysBetweenActivities(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readMaxTermsBetweenActivities(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readActivitiesMaxHourlySpan(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readMinGapsBetweenActivities(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readMaxGapsBetweenActivities(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesNotOverlapping(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivityTagsNotOverlapping(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesSameStartingTime(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesSameStartingHour(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesSameStartingDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxHoursContinuously(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMaxHoursContinuously(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherActivityTagMaxHoursContinuously(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersActivityTagMaxHoursContinuously(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherActivityTagMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersActivityTagMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherActivityTagMinHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersActivityTagMinHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeachersMinHoursDaily(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMinHoursDaily(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxHoursContinuously(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxHoursContinuously(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetActivityTagMaxHoursContinuously(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsActivityTagMaxHoursContinuously(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetActivityTagMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsActivityTagMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetActivityTagMinHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsActivityTagMinHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsMinHoursDaily(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMinHoursDaily(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMinGapsBetweenOrderedPairOfActivityTags(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMinGapsBetweenOrderedPairOfActivityTags(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMinGapsBetweenOrderedPairOfActivityTags(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMinGapsBetweenOrderedPairOfActivityTags(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMinGapsBetweenActivityTag(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMinGapsBetweenActivityTag(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMinGapsBetweenActivityTag(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMinGapsBetweenActivityTag(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMinGapsBetweenActivityTagPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMinGapsBetweenActivityTagPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMinGapsBetweenActivityTagPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMinGapsBetweenActivityTagPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readActivityPreferredTime(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog,
		bool& reportUnspecifiedPermanentlyLockedTime, bool& reportUnspecifiedDayOrHourPreferredStartingTime);
	TimeConstraint* readActivityPreferredStartingTime(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog,
		bool& reportUnspecifiedPermanentlyLockedTime, bool& reportUnspecifiedDayOrHourPreferredStartingTime);

	TimeConstraint* readActivityPreferredDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readActivityEndsStudentsDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesEndStudentsDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	
	TimeConstraint* readActivityEndsTeachersDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesEndTeachersDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readActivityBeginsStudentsDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesBeginStudentsDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	
	TimeConstraint* readActivityBeginsTeachersDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesBeginTeachersDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readActivityBeginsOrEndsStudentsDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesBeginOrEndStudentsDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	
	TimeConstraint* readActivityBeginsOrEndsTeachersDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesBeginOrEndTeachersDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	/*old, with 2 and 3*/
	TimeConstraint* read2ActivitiesConsecutive(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* read2ActivitiesGrouped(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* read3ActivitiesGrouped(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* read2ActivitiesOrdered(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	/*end old*/
	
	TimeConstraint* readTwoActivitiesConsecutive(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTwoActivitiesGrouped(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readThreeActivitiesGrouped(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTwoActivitiesOrdered(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTwoSetsOfActivitiesOrdered(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTwoActivitiesOrderedIfSameDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	
	TimeConstraint* readActivityPreferredTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivityPreferredTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivityPreferredStartingTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	
	TimeConstraint* readBreak(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readBreakTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	
	TimeConstraint* readTeachersNoGaps(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxGapsPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMaxGapsPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxGapsPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMaxGapsPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeachersMaxGapsPerMorningAndAfternoon(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMaxGapsPerMorningAndAfternoon(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	
	TimeConstraint* readStudentsNoGaps(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetNoGaps(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxGapsPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxGapsPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsMaxGapsPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxGapsPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsEarly(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsEarlyMaxBeginningsAtSecondHour(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetEarly(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetEarlyMaxBeginningsAtSecondHour(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readActivitiesPreferredTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesPreferredTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesPreferredStartingTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readSubactivitiesPreferredTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readSubactivitiesPreferredStartingTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTwoSetsOfActivitiesSameSections(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesPairOfMutuallyExclusiveTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readActivitiesOverlapCompletelyOrDontOverlap(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesOverlapCompletelyOrDoNotOverlap(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readActivitiesOccupyMaxTimeSlotsFromSelection(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesOccupyMinTimeSlotsFromSelection(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesMaxSimultaneousInSelectedTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesMinSimultaneousInSelectedTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readMaxTotalActivitiesFromSetInSelectedTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readActivitiesMaxInATerm(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesMinInATerm(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readActivitiesOccupyMaxTerms(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMaxSpanPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxSpanPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxSpanPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxSpanPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMinRestingHours(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMinRestingHours(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMinRestingHours(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMinRestingHours(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	//For mornings-afternoons
	TimeConstraint* readTeacherMaxRealDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxRealDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMaxAfternoonsPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxAfternoonsPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMaxMorningsPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxMorningsPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMaxTwoConsecutiveMornings(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxTwoConsecutiveMornings(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMaxTwoConsecutiveAfternoons(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxTwoConsecutiveAfternoons(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMaxTwoActivityTagsPerDayFromN1N2N3(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxTwoActivityTagsPerDayFromN1N2N3(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMaxTwoActivityTagsPerDayFromN1N2N3(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxTwoActivityTagsPerDayFromN1N2N3(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMaxTwoActivityTagsPerRealDayFromN1N2N3(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxTwoActivityTagsPerRealDayFromN1N2N3(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMaxTwoActivityTagsPerRealDayFromN1N2N3(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxTwoActivityTagsPerRealDayFromN1N2N3(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMaxActivityTagsPerDayFromSet(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxActivityTagsPerDayFromSet(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMaxActivityTagsPerDayFromSet(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxActivityTagsPerDayFromSet(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMaxActivityTagsPerRealDayFromSet(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxActivityTagsPerRealDayFromSet(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMaxActivityTagsPerRealDayFromSet(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxActivityTagsPerRealDayFromSet(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMinRealDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMinRealDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMinMorningsPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMinMorningsPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMinAfternoonsPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMinAfternoonsPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMorningIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMorningIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherAfternoonIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersAfternoonIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMaxRealDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxRealDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMaxAfternoonsPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxAfternoonsPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxMorningsPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxMorningsPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMinAfternoonsPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMinAfternoonsPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMinMorningsPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMinMorningsPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMorningIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMorningIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetAfternoonIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsAfternoonIntervalMaxDaysPerWeek(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeachersMaxHoursDailyRealDays(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMaxHoursDailyRealDays(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherActivityTagMaxHoursDailyRealDays(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersActivityTagMaxHoursDailyRealDays(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMaxHoursPerAllAfternoons(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxHoursPerAllAfternoons(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMaxHoursPerAllAfternoons(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxHoursPerAllAfternoons(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeachersMinHoursPerMorning(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMinHoursPerMorning(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeachersMinHoursPerAfternoon(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMinHoursPerAfternoon(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeachersMinHoursDailyRealDays(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMinHoursDailyRealDays(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsMaxHoursDailyRealDays(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxHoursDailyRealDays(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetActivityTagMaxHoursDailyRealDays(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsActivityTagMaxHoursDailyRealDays(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsMinHoursPerMorning(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMinHoursPerMorning(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsMinHoursPerAfternoon(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMinHoursPerAfternoon(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMaxZeroGapsPerAfternoon(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxZeroGapsPerAfternoon(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeachersMaxGapsPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMaxGapsPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsMaxGapsPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxGapsPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeachersMaxGapsPerWeekForRealDays(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMaxGapsPerWeekForRealDays(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxGapsPerWeekForRealDays(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxGapsPerWeekForRealDays(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsAfternoonsEarlyMaxBeginningsAtSecondHour(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeachersAfternoonsEarlyMaxBeginningsAtSecondHour(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherAfternoonsEarlyMaxBeginningsAtSecondHour(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsMorningsEarlyMaxBeginningsAtSecondHour(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMorningsEarlyMaxBeginningsAtSecondHour(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeachersMorningsEarlyMaxBeginningsAtSecondHour(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeacherMorningsEarlyMaxBeginningsAtSecondHour(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMaxSpanPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxSpanPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxSpanPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxSpanPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMinRestingHoursBetweenMorningAndAfternoon(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMinRestingHoursBetweenMorningAndAfternoon(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMinRestingHoursBetweenMorningAndAfternoon(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMinRestingHoursBetweenMorningAndAfternoon(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readOldMATeacherMaxDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMATeachersMaxDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMATeacherMinDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMATeachersMinDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMAStudentsSetMaxDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMAStudentsMaxDaysPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMATeachersMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMATeacherMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMATeachersMaxHoursDailyHalfDays(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMATeacherMaxHoursDailyHalfDays(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMATeacherActivityTagMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMATeachersActivityTagMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMAStudentsMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMAStudentsSetMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMAStudentsSetActivityTagMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMAStudentsActivityTagMaxHoursDaily(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMATeacherMaxSpanPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMATeachersMaxSpanPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMAStudentsSetMaxSpanPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readOldMAStudentsMaxSpanPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMaxHoursDailyInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxHoursDailyInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetMaxHoursDailyInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxHoursDailyInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherPairOfMutuallyExclusiveTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersPairOfMutuallyExclusiveTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetPairOfMutuallyExclusiveTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsPairOfMutuallyExclusiveTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherPairOfMutuallyExclusiveSetsOfTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersPairOfMutuallyExclusiveSetsOfTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsPairOfMutuallyExclusiveSetsOfTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readActivitiesOccupyMaxSetsOfTimeSlotsFromSelection(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherOccupiesMaxSetsOfTimeSlotsFromSelection(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersOccupyMaxSetsOfTimeSlotsFromSelection(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsOccupyMaxSetsOfTimeSlotsFromSelection(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readStudentsSetMaxSingleGapsInSelectedTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readStudentsMaxSingleGapsInSelectedTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMaxSingleGapsInSelectedTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxSingleGapsInSelectedTimeSlots(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	TimeConstraint* readTeacherMaxHoursPerTerm(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	TimeConstraint* readTeachersMaxHoursPerTerm(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	//
	SpaceConstraint* readBasicCompulsorySpace(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readRoomNotAvailable(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readRoomNotAvailableTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	SpaceConstraint* readTeacherRoomNotAvailableTimes(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	SpaceConstraint* readActivityPreferredRoom(QWidget* parent, QXmlStreamReader& xml, FakeString& xmlReadingLog,
		bool& reportUnspecifiedPermanentlyLockedSpace);
	SpaceConstraint* readActivityPreferredRooms(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readSubjectPreferredRoom(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readSubjectPreferredRooms(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readSubjectSubjectTagPreferredRoom(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readSubjectSubjectTagPreferredRooms(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readSubjectActivityTagPreferredRoom(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readSubjectActivityTagPreferredRooms(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readActivityTagPreferredRoom(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readActivityTagPreferredRooms(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	SpaceConstraint* readStudentsSetHomeRoom(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsSetHomeRooms(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeacherHomeRoom(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeacherHomeRooms(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	SpaceConstraint* readTeacherMaxBuildingChangesPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeachersMaxBuildingChangesPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeacherMaxBuildingChangesPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeachersMaxBuildingChangesPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeacherMinGapsBetweenBuildingChanges(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeachersMinGapsBetweenBuildingChanges(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	SpaceConstraint* readStudentsSetMaxBuildingChangesPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsMaxBuildingChangesPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsSetMaxBuildingChangesPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsMaxBuildingChangesPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsSetMinGapsBetweenBuildingChanges(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsMinGapsBetweenBuildingChanges(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	SpaceConstraint* readTeacherMaxRoomChangesPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeachersMaxRoomChangesPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeacherMaxRoomChangesPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeachersMaxRoomChangesPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeacherMinGapsBetweenRoomChanges(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeachersMinGapsBetweenRoomChanges(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	SpaceConstraint* readStudentsSetMaxRoomChangesPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsMaxRoomChangesPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsSetMaxRoomChangesPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsMaxRoomChangesPerWeek(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsSetMinGapsBetweenRoomChanges(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsMinGapsBetweenRoomChanges(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	SpaceConstraint* readActivitiesOccupyMaxDifferentRooms(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readActivitiesSameRoomIfConsecutive(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	//For mornings-afternoons
	SpaceConstraint* readTeacherMaxRoomChangesPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeachersMaxRoomChangesPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsSetMaxRoomChangesPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsMaxRoomChangesPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	SpaceConstraint* readTeacherMaxBuildingChangesPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeachersMaxBuildingChangesPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsSetMaxBuildingChangesPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsMaxBuildingChangesPerRealDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	SpaceConstraint* readOldMATeacherMaxRoomChangesPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readOldMATeachersMaxRoomChangesPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readOldMAStudentsSetMaxRoomChangesPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readOldMAStudentsMaxRoomChangesPerDay(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	//For all modes
	SpaceConstraint* readTeacherMaxBuildingChangesPerDayInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeachersMaxBuildingChangesPerDayInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsSetMaxBuildingChangesPerDayInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsMaxBuildingChangesPerDayInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	//For mornings-afternoons
	SpaceConstraint* readTeacherMaxBuildingChangesPerRealDayInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeachersMaxBuildingChangesPerRealDayInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsSetMaxBuildingChangesPerRealDayInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsMaxBuildingChangesPerRealDayInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	//For all modes
	SpaceConstraint* readTeacherMaxRoomChangesPerDayInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeachersMaxRoomChangesPerDayInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsSetMaxRoomChangesPerDayInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsMaxRoomChangesPerDayInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	//For mornings-afternoons
	SpaceConstraint* readTeacherMaxRoomChangesPerRealDayInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readTeachersMaxRoomChangesPerRealDayInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsSetMaxRoomChangesPerRealDayInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readStudentsMaxRoomChangesPerRealDayInInterval(QXmlStreamReader& xml, FakeString& xmlReadingLog);

	SpaceConstraint* readRoomMaxActivityTagsPerDayFromSet(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readRoomMaxActivityTagsPerRealDayFromSet(QXmlStreamReader& xml, FakeString& xmlReadingLog);
	SpaceConstraint* readRoomMaxActivityTagsPerWeekFromSet(QXmlStreamReader& xml, FakeString& xmlReadingLog);
};

#endif
