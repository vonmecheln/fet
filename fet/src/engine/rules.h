/*
File rules.h

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

#ifndef RULES_H
#define RULES_H

#include "timetable_defs.h"
#include "timeconstraint.h"
#include "spaceconstraint.h"
#include "activity.h"
#include "studentsset.h"
#include "teacher.h"
#include "subject.h"
#include "subjecttag.h"
#include "room.h"

#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>
#include <q3ptrvector.h>

/**
This class contains the processed input (all the information regarding
the faculty: teachers, students, activities, constraints, etc.)
<p>
Or: Structure that keeps a representation of the requirements for the
timetable (all the input)
*/
class Rules{
public:
	/**
	The name of the institution
	*/
	QString institutionName;
	
	/**
	The comments
	*/
	QString comments;

	/**
	The number of hours per day
	*/
	int nHoursPerDay;

	/**
	The number of days per week
	*/
	int nDaysPerWeek;

	/**
	The days of the week (names)
	*/
	QString daysOfTheWeek[MAX_DAYS_PER_WEEK];

	/**
	The hours of the day (names). This includes also the last hour (+1)
	*/
	QString hoursOfTheDay[MAX_HOURS_PER_DAY+1];

	/**
	The number of hours per week
	*/
	int nHoursPerWeek;

	/**
	The list of teachers
	*/
	TeachersList teachersList;

	/**
	The list of subjects
	*/
	SubjectsList subjectsList;

	/**
	The list of subject tags
	*/
	SubjectTagsList subjectTagsList;

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
	The list of time constraints
	*/
	TimeConstraintsList timeConstraintsList;

	/**
	The list of space constraints
	*/
	SpaceConstraintsList spaceConstraintsList;
	
	/**
	This is the array which specifies a fixed day
	for some activities.
	-1 means that this activity has no fixed day
	*/
	qint16 fixedDay[MAX_ACTIVITIES];
	
	/**
	This is the array which specifies a fixed hour
	for some activities.
	-1 means that this activity has no fixed hour
	*/
	qint16 fixedHour[MAX_ACTIVITIES];
	
	/**
	This array specifies, for each activity (1), a reference to
	another activity (2). The starting day of activity 1 is taken
	as the starting day of activity 2.
	-1 means that the activity is independent of other activities.
	*/
	int sameDay[MAX_ACTIVITIES];
	
	/**
	This array specifies, for each activity (1), a reference to
	another activity (2). The starting hour of activity 1 is taken
	as the starting hour of activity 2.
	-1 means that the activity is independent of other activities.
	*/
	int sameHour[MAX_ACTIVITIES];
	
	/**
	This is the array which specifies a fixed room
	for some activities.
	-1 means that this activity has no fixed room
	*/
	qint16 fixedRoom[MAX_ACTIVITIES];
	
	/**
	This array specifies, for each activity (1), a reference to
	another activity (2). The room of activity 1 is taken
	as the room of activity 2.
	-1 means that the activity is independent of other activities.
	*/
	int sameRoom[MAX_ACTIVITIES];

	/**
	true if the corresponding activities share any teacher
	or students set
	*/
	//bool activitiesConflicting[MAX_ACTIVITIES][MAX_ACTIVITIES];

	//void computeActivitiesConflicting();

	/**
	True if the activities have same teachers (maybe in other order), same students sets,
	and same duration. A similar activity shouldn't be swapped with another one in the backtracking
	*/
	//bool activitiesSimilar[MAX_ACTIVITIES][MAX_ACTIVITIES];
	
	//void computeActivitiesSimilar();

	/**
	True if the second activity contains at least same teachers (maybe in other order), at least the 
	same students sets, and has at least duration as activity 1.
	An activity which contains another shouldn't be swapped in the backtracking
	*/
	//bool activityContained[MAX_ACTIVITIES][MAX_ACTIVITIES];
	
	//void computeActivitiesContained();

	//The following variables contain redundant data and are used internally
	////////////////////////////////////////////////////////////////////////
	int nInternalTeachers;
	Teacher* internalTeachersList[MAX_TEACHERS];

	int nInternalSubjects;
	Subject* internalSubjectsList[MAX_SUBJECTS];

	int nInternalSubgroups;
	StudentsSubgroup* internalSubgroupsList[MAX_TOTAL_SUBGROUPS];
	
	StudentsYearsList augmentedYearsList;

	/**
	Here will be only the active activities.
	
	For speed, I used here not pointers, but static copies.
	*/
	int nInternalActivities;
	Activity internalActivitiesList[MAX_ACTIVITIES];
	
	QList<int> activitiesForSubject[MAX_SUBJECTS];

	int nInternalRooms;
	Room* internalRoomsList[MAX_ROOMS];

	int nInternalTimeConstraints;
	TimeConstraint* internalTimeConstraintsList[MAX_TIME_CONSTRAINTS];

	int nInternalSpaceConstraints;
	SpaceConstraint* internalSpaceConstraintsList[MAX_SPACE_CONSTRAINTS];

	////////////////////////////////////////////////////////////////////////

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
	or constraints, there is a need to call this subroutine
	*/
	bool computeInternalStructure();

	/**
	Terminator - basically clears the memory for the constraints.
	*/
	void kill();

	Rules();

	~Rules();
	
	void setInstitutionName(const QString& newInstitutionName);
	
	void setComments(const QString& newComments);

	/**
	Adds a new teacher to the list of teachers
	(if not already in the list).
	Returns false/true (unsuccessful/successful).
	*/
	bool addTeacher(Teacher* teacher);

	/**
	Returns the index of this teacher in the teachersList,
	or -1 for inexistent teacher.
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
	Adds a new subject to the list of subjects
	(if not already in the list).
	Returns false/true (unsuccessful/successful).
	*/
	bool addSubject(Subject* subject);

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
	Adds a new subject tag to the list of subject tags
	(if not already in the list).
	Returns false/true (unsuccessful/successful).
	*/
	bool addSubjectTag(SubjectTag* subjectTag);

	/**
	Returns the index of this subject tag in the subjectTagsList,
	or -1 if not found.
	*/
	int searchSubjectTag(const QString& subjectTagName);

	/**
	Removes this subject tag. In the list of activities, the subject tag will 
	be removed from all activities which posess it.
	It returns false on failure.
	If successful, returns true.
	*/
	bool removeSubjectTag(const QString& subjectTagName);

	/**
	Modifies (renames) this subject tag and takes care of all related activities.
	Returns true on success, false on failure (if not found)
	*/
	bool modifySubjectTag(const QString& initialSubjectTagName, const QString& finalSubjectTagName);

	/**
	A function to sort the subject tags alphabetically
	*/
	void sortSubjectTagsAlphabetically();

	/**
	Returns a pointer to the structure containing this student container
	(year, group or subgroup) or NULL.
	*/
	StudentsSet* searchStudentsSet(const QString& setName);
	
	StudentsSet* searchAugmentedStudentsSet(const QString& setName);
	
	/**
	True if the students sets contain one common subgroup.
	This function is used in constraints isRelatedToStudentsSet
	*/
	bool studentsSetsRelated(const QString& studentsSet1, const QString& studentsSet2);

	/**
	Adds a new year of study to the academic structure
	*/
	bool addYear(StudentsYear* year);

	bool removeYear(const QString& yearName);

	/**
	Returns -1 if not found or the index of this year in the years list
	*/
	int searchYear(const QString& yearName);

	int searchAugmentedYear(const QString& yearName);

	/**
	Modifies this students year (name, number of students) and takes care of all related 
	activities and constraints.	Returns true on success, false on failure (if not found)
	*/
	bool modifyYear(const QString& initialYearName, const QString& finalYearName, int finalNumberOfStudents);
	
	/**
	A function to sort the years alphabetically
	*/
	void sortYearsAlphabetically();

	/**
	Adds a new group in a certain year of study to the academic structure
	*/
	bool addGroup(const QString& yearName, StudentsGroup* group);

	bool removeGroup(const QString& yearName, const QString& groupName);

	/**
	Returns -1 if not found or the index of this group in the groups list
	of this year.
	*/
	int searchGroup(const QString& yearName, const QString& groupName);

	int searchAugmentedGroup(const QString& yearName, const QString& groupName);

	/**
	Modifies this students group (name, number of students) and takes care of all related 
	activities and constraints.	Returns true on success, false on failure (if not found)
	*/
	bool modifyGroup(const QString& yearName, const QString& initialGroupName, const QString& finalGroupName, int finalNumberOfStudents);
	
	/**
	A function to sort the groups of this year alphabetically
	*/
	void sortGroupsAlphabetically(const QString& yearName);

	/**
	Adds a new subgroup to a certain group in a certain year of study to
	the academic structure
	*/
	bool addSubgroup(const QString& yearName, const QString& groupName, StudentsSubgroup* subgroup);

	bool removeSubgroup(const QString& yearName, const QString& groupName, const QString& subgroupName);

	/**
	Returns -1 if not found or the index of the subgroup in the list of subgroups of this group
	*/
	int searchSubgroup(const QString& yearName, const QString& groupName, const QString& subgroupName);

	int searchAugmentedSubgroup(const QString& yearName, const QString& groupName, const QString& subgroupName);

	/**
	Modifies this students subgroup (name, number of students) and takes care of all related 
	activities and constraints.	Returns true on success, false on failure (if not found)
	*/
	bool modifySubgroup(const QString& yearName, const QString& groupName, const QString& initialSubgroupName, const QString& finalSubgroupName, int finalNumberOfStudents);
	
	/**
	A function to sort the subgroups of this group alphabetically
	*/
	void sortSubgroupsAlphabetically(const QString& yearName, const QString& groupName);
	
	/**
	Adds a new indivisible activity (not split) to the list of activities.
	(It can add a subactivity of a split activity)
	Returns true if successful or false if the maximum
	number of activities was reached.
	There is automatically added a ConstraintActivityPreferredTime, if necessary
	*/
	bool addSimpleActivity(
		int _id,
		int _activityGroupId,
		const QStringList& _teachersNames,
		const QString& _subjectName,
		const QString& _subjectTagName,
		const QStringList& _studentsNames,
		int _duration, /*duration, in hours*/
		int _totalDuration,
		//int _parity, /*parity: PARITY_WEEKLY or PARITY_FORTNIGHTLY*/
		bool _active,
		//int _preferredDay,
		//int _preferredHour,
		bool _computeNTotalStudents,
		int _nTotalStudents);

	/**
	Adds a new split activity to the list of activities.
	Returns true if successful or false if the maximum
	number of activities was reached.
	If _minDayDistance>0, there will automatically added a compulsory
	ConstraintMinNDaysBetweenActivities.
	Also, there are automatically added several ConstraintActivityPreferredTime, if necessary
	*/
	bool addSplitActivity(
		int _firstActivityId,
		int _activityGroupId,
		const QStringList& _teachersNames,
		const QString& _subjectName,
		const QString& _subjectTagName,
		const QStringList& _studentsNames,
		int _nSplits,
		int _totalDuration,
		int _durations[],
		//int _parities[],
		bool _active[],
		int _minDayDistance,
		double _weightPercentage,
		bool _consecutiveIfSameDay,
		//int _preferredDays[],
		//int _preferredHours[],
		bool _computeNTotalStudents,
		int _nTotalStudents);

	/**
	Removes only the activity with this id.
	*/
	void removeActivity(int _id);

	/**
	If _activityGroupId==0, then this is a non-split activity
	(if >0, then this is a single sub-activity from a split activity.
	Removes this activity from the list of activities.
	For split activities, it removes all the sub-activities that are contained in it.
	*/
	void removeActivity(int _id, int _activityGroupId);
	
	/**
	A function to modify the information of a certain activity.
	If this is a sub-activity of a split activity,
	all the sub-activities will be modified.
	*/
	void modifyActivity(
		int _id, 
		int _activityGroupId, 
		const QStringList& _teachersNames,
		const QString& _subjectName, 
		const QString& _subjectTagName, 
		const QStringList& _studentsNames,
		//int _nTotalStudents,
	 	int _nSplits,
		int _totalDuration,
		int _durations[],
		//int _parities[],
		bool _active[],
		bool _computeNTotalStudents,
		int nTotalStudents);

	/**
	Adds a new room (already allocated).
	Returns true on success, false for already existing rooms (same name).
	*/
	bool addRoom(Room* rm);

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
	bool modifyRoom(const QString& initialRoomName, const QString& finalRoomName, int capacity);

	/**
	A function to sort the room alphabetically, by name
	*/
	void sortRoomsAlphabetically();

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

	/**
	Reads the rules from the xml input file "filename".
	Returns true on success, false on failure (inexistent file or wrong format)
	*/
	bool read(const QString& filename);

	/**
	Write the rules to the xml input file "inputfile".
	*/
	void write(const QString& filename);
	
	int activateTeacher(const QString& teacherName);
	
	int activateStudents(const QString& studentsName);
	
	int activateSubject(const QString& subjectName);
	
	int activateSubjectTag(const QString& subjectTagName);

	int deactivateTeacher(const QString& teacherName);
	
	int deactivateStudents(const QString& studentsName);
	
	int deactivateSubject(const QString& subjectName);
	
	int deactivateSubjectTag(const QString& subjectTagName);
};

#endif
