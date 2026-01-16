/*
File spaceconstraint.cpp
*/

/***************************************************************************
                          spaceconstraint.cpp  -  description
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

#include "timetable_defs.h"
#include "spaceconstraint.h"
#include "rules.h"
#include "activity.h"
#include "teacher.h"
#include "subject.h"
#include "activitytag.h"
#include "studentsset.h"
#include "room.h"
#include "solution.h"

#include "matrix.h"

#include "generate_pre.h"

#include <QString>
#include <QSet>

#include <QDataStream>

#include "messageboxes.h"

//1
QDataStream& operator<<(QDataStream& stream, const ConstraintBasicCompulsorySpace& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	return stream;
}

//2
QDataStream& operator<<(QDataStream& stream, const ConstraintRoomNotAvailableTimes& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.days<<sc.hours<<sc.room;

	return stream;
}

//3
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityPreferredRoom& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.activityId<<sc.permanentlyLocked<<sc.roomName<<sc.preferredRealRoomsNames;

	return stream;
}

//4
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityPreferredRooms& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.activityId<<sc.roomsNames;

	return stream;
}

//5
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetHomeRoom& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.roomName<<sc.studentsName;

	return stream;
}

//6
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetHomeRooms& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.roomsNames<<sc.studentsName;

	return stream;
}

//7
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherHomeRoom& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.roomName<<sc.teacherName;

	return stream;
}

//8
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherHomeRooms& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.roomsNames<<sc.teacherName;

	return stream;
}

//9
QDataStream& operator<<(QDataStream& stream, const ConstraintSubjectPreferredRoom& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.roomName<<sc.subjectName;

	return stream;
}

//10
QDataStream& operator<<(QDataStream& stream, const ConstraintSubjectPreferredRooms& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.roomsNames<<sc.subjectName;

	return stream;
}

//11
QDataStream& operator<<(QDataStream& stream, const ConstraintSubjectActivityTagPreferredRoom& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.roomName<<sc.subjectName<<sc.activityTagName;

	return stream;
}

//12
QDataStream& operator<<(QDataStream& stream, const ConstraintSubjectActivityTagPreferredRooms& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.roomsNames<<sc.subjectName<<sc.activityTagName;

	return stream;
}

//13
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxBuildingChangesPerDay& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerDay;

	return stream;
}

//14
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxBuildingChangesPerDay& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerDay<<sc.studentsName;

	return stream;
}

//15
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxBuildingChangesPerWeek& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerWeek;

	return stream;
}

//16
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxBuildingChangesPerWeek& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerWeek<<sc.studentsName;

	return stream;
}

//17
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinGapsBetweenBuildingChanges& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.minGapsBetweenBuildingChanges;

	return stream;
}

//18
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinGapsBetweenBuildingChanges& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.minGapsBetweenBuildingChanges<<sc.studentsName;

	return stream;
}

//19
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxBuildingChangesPerDay& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerDay;

	return stream;
}

//20
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxBuildingChangesPerDay& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerDay<<sc.teacherName;

	return stream;
}

//21
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxBuildingChangesPerWeek& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerWeek;

	return stream;
}

//22
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxBuildingChangesPerWeek& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerWeek<<sc.teacherName;

	return stream;
}

//23
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinGapsBetweenBuildingChanges& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.minGapsBetweenBuildingChanges;

	return stream;
}

//24
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinGapsBetweenBuildingChanges& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.minGapsBetweenBuildingChanges<<sc.teacherName;

	return stream;
}

//25
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityTagPreferredRoom& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.roomName<<sc.activityTagName;

	return stream;
}

//26
QDataStream& operator<<(QDataStream& stream, const ConstraintActivityTagPreferredRooms& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.roomsNames<<sc.activityTagName;

	return stream;
}

//27
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesOccupyMaxDifferentRooms& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.activitiesIds<<sc.maxDifferentRooms;

	return stream;
}

//28
QDataStream& operator<<(QDataStream& stream, const ConstraintActivitiesSameRoomIfConsecutive& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.activitiesIds;

	return stream;
}

//29
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxRoomChangesPerDay& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerDay;

	return stream;
}

//30
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxRoomChangesPerDay& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerDay<<sc.studentsName;

	return stream;
}

//31
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxRoomChangesPerWeek& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerWeek;

	return stream;
}

//32
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxRoomChangesPerWeek& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerWeek<<sc.studentsName;

	return stream;
}

//33
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMinGapsBetweenRoomChanges& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.minGapsBetweenRoomChanges;

	return stream;
}

//34
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMinGapsBetweenRoomChanges& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.minGapsBetweenRoomChanges<<sc.studentsName;

	return stream;
}

//35
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxRoomChangesPerDay& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerDay;

	return stream;
}

//36
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxRoomChangesPerDay& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerDay<<sc.teacherName;

	return stream;
}

//37
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxRoomChangesPerWeek& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerWeek;

	return stream;
}

//38
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxRoomChangesPerWeek& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerWeek<<sc.teacherName;

	return stream;
}

//39
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMinGapsBetweenRoomChanges& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.minGapsBetweenRoomChanges;

	return stream;
}

//40
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMinGapsBetweenRoomChanges& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.minGapsBetweenRoomChanges<<sc.teacherName;

	return stream;
}

//41
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherRoomNotAvailableTimes& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.days<<sc.hours<<sc.teacherName<<sc.room;

	return stream;
}

//42
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxRoomChangesPerRealDay& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerDay;

	return stream;
}

//43
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxRoomChangesPerRealDay& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerDay<<sc.studentsName;

	return stream;
}

//44
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxRoomChangesPerRealDay& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerDay;

	return stream;
}

//45
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxRoomChangesPerRealDay& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerDay<<sc.teacherName;

	return stream;
}

//46
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxBuildingChangesPerRealDay& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerDay;

	return stream;
}

//47
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxBuildingChangesPerRealDay& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerDay<<sc.studentsName;

	return stream;
}

//48
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxBuildingChangesPerRealDay& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerDay;

	return stream;
}

//49
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxBuildingChangesPerRealDay& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerDay<<sc.teacherName;

	return stream;
}

//50
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxBuildingChangesPerDayInInterval& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerDay<<sc.intervalStart<<sc.intervalEnd;

	return stream;
}

//51
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxBuildingChangesPerDayInInterval& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerDay<<sc.studentsName<<sc.intervalStart<<sc.intervalEnd;

	return stream;
}

//52
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxBuildingChangesPerDayInInterval& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerDay<<sc.intervalStart<<sc.intervalEnd;

	return stream;
}

//53
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxBuildingChangesPerDayInInterval& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerDay<<sc.teacherName<<sc.intervalStart<<sc.intervalEnd;

	return stream;
}

//54
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxBuildingChangesPerRealDayInInterval& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerDay<<sc.intervalStart<<sc.intervalEnd;

	return stream;
}

//55
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerDay<<sc.studentsName<<sc.intervalStart<<sc.intervalEnd;

	return stream;
}

//56
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxBuildingChangesPerRealDayInInterval& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerDay<<sc.intervalStart<<sc.intervalEnd;

	return stream;
}

//57
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxBuildingChangesPerRealDayInInterval& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxBuildingChangesPerDay<<sc.teacherName<<sc.intervalStart<<sc.intervalEnd;

	return stream;
}

//58
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxRoomChangesPerDayInInterval& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerDay<<sc.intervalStart<<sc.intervalEnd;

	return stream;
}

//59
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxRoomChangesPerDayInInterval& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerDay<<sc.studentsName<<sc.intervalStart<<sc.intervalEnd;

	return stream;
}

//60
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxRoomChangesPerDayInInterval& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerDay<<sc.intervalStart<<sc.intervalEnd;

	return stream;
}

//61
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxRoomChangesPerDayInInterval& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerDay<<sc.teacherName<<sc.intervalStart<<sc.intervalEnd;

	return stream;
}

//62
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsMaxRoomChangesPerRealDayInInterval& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerDay<<sc.intervalStart<<sc.intervalEnd;

	return stream;
}

//63
QDataStream& operator<<(QDataStream& stream, const ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerDay<<sc.studentsName<<sc.intervalStart<<sc.intervalEnd;

	return stream;
}

//64
QDataStream& operator<<(QDataStream& stream, const ConstraintTeachersMaxRoomChangesPerRealDayInInterval& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerDay<<sc.intervalStart<<sc.intervalEnd;

	return stream;
}

//65
QDataStream& operator<<(QDataStream& stream, const ConstraintTeacherMaxRoomChangesPerRealDayInInterval& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.maxRoomChangesPerDay<<sc.teacherName<<sc.intervalStart<<sc.intervalEnd;

	return stream;
}

//66
QDataStream& operator<<(QDataStream& stream, const ConstraintRoomMaxActivityTagsPerDayFromSet& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.room<<sc.maxTags<<sc.tagsList;

	return stream;
}

//67
QDataStream& operator<<(QDataStream& stream, const ConstraintRoomMaxActivityTagsPerRealDayFromSet& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.room<<sc.maxTags<<sc.tagsList;

	return stream;
}

//68
QDataStream& operator<<(QDataStream& stream, const ConstraintRoomMaxActivityTagsPerWeekFromSet& sc)
{
	//stream<<sc.type;
	stream<<sc.weightPercentage;
	stream<<sc.active;
	stream<<sc.comments;

	stream<<sc.room<<sc.maxTags<<sc.tagsList;

	return stream;
}

//1
QDataStream& operator>>(QDataStream& stream, ConstraintBasicCompulsorySpace& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	return stream;
}

//2
QDataStream& operator>>(QDataStream& stream, ConstraintRoomNotAvailableTimes& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.days>>sc.hours>>sc.room;

	return stream;
}

//3
QDataStream& operator>>(QDataStream& stream, ConstraintActivityPreferredRoom& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.activityId>>sc.permanentlyLocked>>sc.roomName>>sc.preferredRealRoomsNames;

	return stream;
}

//4
QDataStream& operator>>(QDataStream& stream, ConstraintActivityPreferredRooms& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.activityId>>sc.roomsNames;

	return stream;
}

//5
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetHomeRoom& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.roomName>>sc.studentsName;

	return stream;
}

//6
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetHomeRooms& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.roomsNames>>sc.studentsName;

	return stream;
}

//7
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherHomeRoom& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.roomName>>sc.teacherName;

	return stream;
}

//8
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherHomeRooms& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.roomsNames>>sc.teacherName;

	return stream;
}

//9
QDataStream& operator>>(QDataStream& stream, ConstraintSubjectPreferredRoom& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.roomName>>sc.subjectName;

	return stream;
}

//10
QDataStream& operator>>(QDataStream& stream, ConstraintSubjectPreferredRooms& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.roomsNames>>sc.subjectName;

	return stream;
}

//11
QDataStream& operator>>(QDataStream& stream, ConstraintSubjectActivityTagPreferredRoom& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.roomName>>sc.subjectName>>sc.activityTagName;

	return stream;
}

//12
QDataStream& operator>>(QDataStream& stream, ConstraintSubjectActivityTagPreferredRooms& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.roomsNames>>sc.subjectName>>sc.activityTagName;

	return stream;
}

//13
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxBuildingChangesPerDay& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerDay;

	return stream;
}

//14
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxBuildingChangesPerDay& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerDay>>sc.studentsName;

	return stream;
}

//15
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxBuildingChangesPerWeek& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerWeek;

	return stream;
}

//16
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxBuildingChangesPerWeek& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerWeek>>sc.studentsName;

	return stream;
}

//17
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinGapsBetweenBuildingChanges& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.minGapsBetweenBuildingChanges;

	return stream;
}

//18
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinGapsBetweenBuildingChanges& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.minGapsBetweenBuildingChanges>>sc.studentsName;

	return stream;
}

//19
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxBuildingChangesPerDay& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerDay;

	return stream;
}

//20
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxBuildingChangesPerDay& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerDay>>sc.teacherName;

	return stream;
}

//21
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxBuildingChangesPerWeek& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerWeek;

	return stream;
}

//22
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxBuildingChangesPerWeek& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerWeek>>sc.teacherName;

	return stream;
}

//23
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinGapsBetweenBuildingChanges& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.minGapsBetweenBuildingChanges;

	return stream;
}

//24
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinGapsBetweenBuildingChanges& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.minGapsBetweenBuildingChanges>>sc.teacherName;

	return stream;
}

//25
QDataStream& operator>>(QDataStream& stream, ConstraintActivityTagPreferredRoom& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.roomName>>sc.activityTagName;

	return stream;
}

//26
QDataStream& operator>>(QDataStream& stream, ConstraintActivityTagPreferredRooms& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.roomsNames>>sc.activityTagName;

	return stream;
}

//27
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesOccupyMaxDifferentRooms& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.activitiesIds>>sc.maxDifferentRooms;

	return stream;
}

//28
QDataStream& operator>>(QDataStream& stream, ConstraintActivitiesSameRoomIfConsecutive& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.activitiesIds;

	return stream;
}

//29
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxRoomChangesPerDay& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerDay;

	return stream;
}

//30
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxRoomChangesPerDay& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerDay>>sc.studentsName;

	return stream;
}

//31
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxRoomChangesPerWeek& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerWeek;

	return stream;
}

//32
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxRoomChangesPerWeek& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerWeek>>sc.studentsName;

	return stream;
}

//33
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMinGapsBetweenRoomChanges& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.minGapsBetweenRoomChanges;

	return stream;
}

//34
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMinGapsBetweenRoomChanges& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.minGapsBetweenRoomChanges>>sc.studentsName;

	return stream;
}

//35
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxRoomChangesPerDay& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerDay;

	return stream;
}

//36
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxRoomChangesPerDay& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerDay>>sc.teacherName;

	return stream;
}

//37
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxRoomChangesPerWeek& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerWeek;

	return stream;
}

//38
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxRoomChangesPerWeek& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerWeek>>sc.teacherName;

	return stream;
}

//39
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMinGapsBetweenRoomChanges& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.minGapsBetweenRoomChanges;

	return stream;
}

//40
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMinGapsBetweenRoomChanges& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.minGapsBetweenRoomChanges>>sc.teacherName;

	return stream;
}

//41
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherRoomNotAvailableTimes& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.days>>sc.hours>>sc.teacherName>>sc.room;

	return stream;
}

//42
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxRoomChangesPerRealDay& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerDay;

	return stream;
}

//43
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxRoomChangesPerRealDay& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerDay>>sc.studentsName;

	return stream;
}

//44
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxRoomChangesPerRealDay& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerDay;

	return stream;
}

//45
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxRoomChangesPerRealDay& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerDay>>sc.teacherName;

	return stream;
}

//46
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxBuildingChangesPerRealDay& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerDay;

	return stream;
}

//47
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxBuildingChangesPerRealDay& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerDay>>sc.studentsName;

	return stream;
}

//48
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxBuildingChangesPerRealDay& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerDay;

	return stream;
}

//49
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxBuildingChangesPerRealDay& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerDay>>sc.teacherName;

	return stream;
}

//50
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxBuildingChangesPerDayInInterval& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerDay>>sc.intervalStart>>sc.intervalEnd;

	return stream;
}

//51
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxBuildingChangesPerDayInInterval& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerDay>>sc.studentsName>>sc.intervalStart>>sc.intervalEnd;

	return stream;
}

//52
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxBuildingChangesPerDayInInterval& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerDay>>sc.intervalStart>>sc.intervalEnd;

	return stream;
}

//53
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxBuildingChangesPerDayInInterval& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerDay>>sc.teacherName>>sc.intervalStart>>sc.intervalEnd;

	return stream;
}

//54
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxBuildingChangesPerRealDayInInterval& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerDay>>sc.intervalStart>>sc.intervalEnd;

	return stream;
}

//55
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerDay>>sc.studentsName>>sc.intervalStart>>sc.intervalEnd;

	return stream;
}

//56
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxBuildingChangesPerRealDayInInterval& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerDay>>sc.intervalStart>>sc.intervalEnd;

	return stream;
}

//57
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxBuildingChangesPerRealDayInInterval& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxBuildingChangesPerDay>>sc.teacherName>>sc.intervalStart>>sc.intervalEnd;

	return stream;
}

//58
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxRoomChangesPerDayInInterval& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerDay>>sc.intervalStart>>sc.intervalEnd;

	return stream;
}

//59
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxRoomChangesPerDayInInterval& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerDay>>sc.studentsName>>sc.intervalStart>>sc.intervalEnd;

	return stream;
}

//60
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxRoomChangesPerDayInInterval& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerDay>>sc.intervalStart>>sc.intervalEnd;

	return stream;
}

//61
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxRoomChangesPerDayInInterval& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerDay>>sc.teacherName>>sc.intervalStart>>sc.intervalEnd;

	return stream;
}

//62
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsMaxRoomChangesPerRealDayInInterval& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerDay>>sc.intervalStart>>sc.intervalEnd;

	return stream;
}

//63
QDataStream& operator>>(QDataStream& stream, ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerDay>>sc.studentsName>>sc.intervalStart>>sc.intervalEnd;

	return stream;
}

//64
QDataStream& operator>>(QDataStream& stream, ConstraintTeachersMaxRoomChangesPerRealDayInInterval& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerDay>>sc.intervalStart>>sc.intervalEnd;

	return stream;
}

//65
QDataStream& operator>>(QDataStream& stream, ConstraintTeacherMaxRoomChangesPerRealDayInInterval& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.maxRoomChangesPerDay>>sc.teacherName>>sc.intervalStart>>sc.intervalEnd;

	return stream;
}

//66
QDataStream& operator>>(QDataStream& stream, ConstraintRoomMaxActivityTagsPerDayFromSet& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.room>>sc.maxTags>>sc.tagsList;

	return stream;
}

//67
QDataStream& operator>>(QDataStream& stream, ConstraintRoomMaxActivityTagsPerRealDayFromSet& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.room>>sc.maxTags>>sc.tagsList;

	return stream;
}

//68
QDataStream& operator>>(QDataStream& stream, ConstraintRoomMaxActivityTagsPerWeekFromSet& sc)
{
	//stream>>sc.type;
	stream>>sc.weightPercentage;
	stream>>sc.active;
	stream>>sc.comments;

	stream>>sc.room>>sc.maxTags>>sc.tagsList;

	return stream;
}

static QString trueFalse(bool x)
{
	if(!x)
		return QString("false");
	else
		return QString("true");
}

static QString yesNoTranslated(bool x)
{
	if(!x)
		return QCoreApplication::translate("SpaceConstraint", "no", "negative");
	else
		return QCoreApplication::translate("SpaceConstraint", "yes", "affirmative");
}

static Matrix3D<int> roomsMatrix;

static int rooms_conflicts=-1;

QString getActivityDescription(Rules& r, int id); //implemented in timeconstraint.cpp
QString getActivityDetailedDescription(Rules& r, int id); //implemented in timeconstraint.cpp

void populateInternalSubgroupsList(const Rules& r, const StudentsSet* ss, QList<int>& iSubgroupsList); //implemented in timeconstraint.cpp

bool spaceConstraintCanHaveAnyWeight(int type)
{
	assert(type!=CONSTRAINT_GENERIC_SPACE);

	assert(type!=CONSTRAINT_ACTIVITY_PREFERRED_ROOM);
	/*if(type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
		if(((ConstraintActivityPreferredRoom*)this)->preferredRealRoomsNames.isEmpty())
			return true;
		else
			return false;
	}*/
	
	bool t;

	switch(type){
		case CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_HOME_ROOM:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_HOME_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_HOME_ROOM:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_HOME_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES:
			t=true;
			break;
		
		default:
			t=false;
			break;
	}
	
	return t;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

SpaceConstraint::SpaceConstraint()
{
	type=CONSTRAINT_GENERIC_SPACE;
	
	active=true;
	comments=QString("");
}

SpaceConstraint::~SpaceConstraint()
{
}

SpaceConstraint::SpaceConstraint(double wp)
{
	type=CONSTRAINT_GENERIC_SPACE;

	weightPercentage=wp;
	assert(wp<=100 && wp>=0);

	active=true;
	comments=QString("");
}

bool SpaceConstraint::canHaveAnyWeight()
{
	if(type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
		if(((ConstraintActivityPreferredRoom*)this)->preferredRealRoomsNames.isEmpty())
			return true;
		else
			return false;
	}

	return spaceConstraintCanHaveAnyWeight(type);
}

bool SpaceConstraint::canBeUsedInOfficialMode()
{
	assert(type!=CONSTRAINT_GENERIC_SPACE);

	bool t;
	
	switch(type){
		case CONSTRAINT_BASIC_COMPULSORY_SPACE:
			[[fallthrough]];
		case CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_HOME_ROOM:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_HOME_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_HOME_ROOM:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_HOME_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE:
			[[fallthrough]];
		//2019-11-14
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			[[fallthrough]];
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_WEEK_FROM_SET:
			t=true;
			break;
		
		default:
			t=false;
			break;
	}

	return t;
}

bool SpaceConstraint::canBeUsedInMorningsAfternoonsMode()
{
	assert(type!=CONSTRAINT_GENERIC_SPACE);
	
	bool t;
	
	switch(type){
		case CONSTRAINT_BASIC_COMPULSORY_SPACE:
			[[fallthrough]];
		case CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_HOME_ROOM:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_HOME_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_HOME_ROOM:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_HOME_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE:
			[[fallthrough]];
		 //2019-11-14
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES:
			[[fallthrough]];
		//mornings-afternoons
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			[[fallthrough]];
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			[[fallthrough]];
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_WEEK_FROM_SET:
			t=true;
			break;
		
		default:
			t=false;
			break;
	}

	return t;
}

bool SpaceConstraint::canBeUsedInBlockPlanningMode()
{
	assert(type!=CONSTRAINT_GENERIC_SPACE);
	
	bool t;
	
	switch(type){
		case CONSTRAINT_BASIC_COMPULSORY_SPACE:
			[[fallthrough]];
		case CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_HOME_ROOM:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_HOME_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_HOME_ROOM:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_HOME_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE:
			[[fallthrough]];
		//2019-11-14
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			[[fallthrough]];
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_WEEK_FROM_SET:
			t=true;
			break;
		
		default:
			t=false;
			break;
	}

	return t;
}

bool SpaceConstraint::canBeUsedInTermsMode()
{
	assert(type!=CONSTRAINT_GENERIC_SPACE);
	
	bool t;
	
	switch(type){
		case CONSTRAINT_BASIC_COMPULSORY_SPACE:
			[[fallthrough]];
		case CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_HOME_ROOM:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_HOME_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_HOME_ROOM:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_HOME_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS:
			[[fallthrough]];
		case CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE:
			[[fallthrough]];
		//2019-11-14
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			[[fallthrough]];
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_WEEK_FROM_SET:
			t=true;
			break;
		
		default:
			t=false;
			break;
	}

	return t;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintBasicCompulsorySpace::ConstraintBasicCompulsorySpace()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_BASIC_COMPULSORY_SPACE;
	this->weightPercentage=100;
}

ConstraintBasicCompulsorySpace::ConstraintBasicCompulsorySpace(double wp)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_BASIC_COMPULSORY_SPACE;
}

bool ConstraintBasicCompulsorySpace::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintBasicCompulsorySpace::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintBasicCompulsorySpace::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s = IL2+"<ConstraintBasicCompulsorySpace>\n";
	assert(this->weightPercentage==100.0);
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintBasicCompulsorySpace>\n";
	return s;
}

QString ConstraintBasicCompulsorySpace::getDescription(Rules& r)
{
	Q_UNUSED(r);
	
	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s = tr("Basic compulsory constraints (space)");
	s+=translatedCommaSpace();
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));
	
	return begin+s+end;
}

QString ConstraintBasicCompulsorySpace::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("These are the basic compulsory constraints (referring to rooms allocation) for any timetable");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("The basic space constraints try to avoid:");s+="\n";
	s+=QString("- "); s+=tr("rooms assigned to more than one activity simultaneously"); s+="\n";
	s+=QString("- "); s+=tr("activities with more students than the capacity of the room"); s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintBasicCompulsorySpace::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{

	assert(r.internalStructureComputed);

	int roomsConflicts;

	//This constraint fitness calculation routine is called firstly,
	//so we can compute the rooms conflicts faster this way.
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = roomsConflicts = c.getRoomsMatrix(r, roomsMatrix);
		
		c.changedForMatrixCalculation=false;
	}
	else{
		assert(rooms_conflicts>=0);
		roomsConflicts=rooms_conflicts;
	}

	int i;

	qint64 unallocated; //unallocated activities
	int nre; //number of room exhaustions
	int nor; //number of overwhelmed rooms

	//part without logging....................................................................
	if(conflictsString==nullptr){
		//Unallocated activities
		unallocated=0;
		nor=0;
		for(i=0; i<r.nInternalActivities; i++)
			if(c.rooms[i]==UNALLOCATED_SPACE){
				//Firstly, we consider a big clash each unallocated activity.
				//Needs to be very a large constant, bigger than any other broken constraint.
				unallocated += /*r.internalActivitiesList[i].duration * r.internalActivitiesList[i].nSubgroups * */ 10000;
				//(an unallocated activity for a year is more important than an unallocated activity for a subgroup)
			}
			else if(c.rooms[i]!=UNSPECIFIED_ROOM){
				//The capacity of each room must be respected
				//(the number of students must be less than the capacity)
				int rm=c.rooms[i];
				if(r.internalActivitiesList[i].nTotalStudents>r.internalRoomsList[rm]->capacity){
					int tmp;
					//if(r.internalActivitiesList[i].parity==PARITY_WEEKLY)
					//	tmp=2;
					//else
						tmp=1;
	
					nor+=tmp;
				}
			}

		//Calculates the number of rooms exhaustion (when a room is occupied
		//for more than one activity at the same time)
		/*nre=0;
		for(i=0; i<r.nInternalRooms; i++)
			for(int j=0; j<r.nDaysPerWeek; j++)
				for(int k=0; k<r.nHoursPerDay; k++){
					int tmp=roomsMatrix[i][j][k]-1;
					if(tmp>0){
						if(conflictsString!=nullptr){
							QString s=tr("Space constraint basic compulsory: room with name %1 has more than one allocated activity on day %2, hour %3.")
								.arg(r.internalRoomsList[i]->name)
								.arg(r.daysOfTheWeek[j])
								.arg(r.hoursOfTheDay[k]);
							s+=" ";
							s+=tr("This increases the conflicts total by %1").arg(tmp*weightPercentage/100);
						
							dl.append(s);
							cl.append(tmp*weightPercentage/100);
						
							*conflictsString += s+"\n";
						}
						nre+=tmp;
					}
				}
		*/
		nre=roomsConflicts;
	}
	//part with logging....................................................................
	else{
		//Unallocated activities
		unallocated=0;
		nor=0;
		for(i=0; i<r.nInternalActivities; i++)
			if(c.rooms[i]==UNALLOCATED_SPACE){
				//Firstly, we consider a big clash each unallocated activity.
				//Needs to be very a large constant, bigger than any other broken constraint.
				unallocated += /*r.internalActivitiesList[i].duration * r.internalActivitiesList[i].nSubgroups * */ 10000;
				//(an unallocated activity for a year is more important than an unallocated activity for a subgroup)
				if(conflictsString!=nullptr){
					QString s=tr("Space constraint basic compulsory broken: unallocated activity with id=%1 (%2)",
						"%2 is the detailed description of the activity").arg(r.internalActivitiesList[i].id).arg(getActivityDetailedDescription(r, r.internalActivitiesList[i].id));
					s+=QString(" - ");
					s+=tr("this increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100*10000));
					
					dl.append(s);
					cl.append(weightPercentage/100 * 10000);
					
					*conflictsString+=s+"\n";

					/*(*conflictsString) += tr("Space constraint basic compulsory: unallocated activity with id=%1").arg(r.internalActivitiesList[i].id);
					(*conflictsString) += tr(" - this increases the conflicts total by %1")
						.arg(weight*10000);
					(*conflictsString) += "\n";*/
				}
			}
			else if(c.rooms[i]!=UNSPECIFIED_ROOM){
				//The capacity of each room must be respected
				//(the number of students must be less than the capacity)
				int rm=c.rooms[i];
				if(r.internalActivitiesList[i].nTotalStudents>r.internalRoomsList[rm]->capacity){
					int tmp;
					//if(r.internalActivitiesList[i].parity==PARITY_WEEKLY)
					//	tmp=2;
					//else
						tmp=1;
	
					nor+=tmp;

					if(conflictsString!=nullptr){
						QString s;
						s=tr("Space constraint basic compulsory: room %1 has allocated activity with id %2 (%3) and the capacity of the room is overloaded",
							"%2 is act id, %3 is detailed description of activity")
						.arg(r.internalRoomsList[rm]->name)
						.arg(r.internalActivitiesList[i].id)
						.arg(getActivityDetailedDescription(r, r.internalActivitiesList[i].id));
						s+=". ";
						s+=tr("This increases conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100));
						
						dl.append(s);
						cl.append(weightPercentage/100);
						
						*conflictsString += s+"\n";
					}
				}
			}

		//Calculates the number of rooms exhaustion (when a room is occupied
		//for more than one activity at the same time)
		nre=roomsConflicts;
		/*nre=0;
		for(i=0; i<r.nInternalRooms; i++){
			if(r.internalRoomsList[i]->isVirtual==false){
				for(int j=0; j<r.nDaysPerWeek; j++)
					for(int k=0; k<r.nHoursPerDay; k++){
						int tmp=roomsMatrix[i][j][k]-1;
						if(tmp>0){
							if(conflictsString!=nullptr){
								QString s=tr("Space constraint basic compulsory: room with name %1 has more than one allocated activity on day %2, hour %3.")
									.arg(r.internalRoomsList[i]->name)
									.arg(r.daysOfTheWeek[j])
									.arg(r.hoursOfTheDay[k]);
								s+=" ";
								s+=tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(tmp*weightPercentage/100));
							
								dl.append(s);
								cl.append(tmp*weightPercentage/100);
							
								*conflictsString += s+"\n";
							}
							nre+=tmp;
						}
					}
			}
		}*/
	}
		
	if(this->weightPercentage==100){
		//assert(unallocated==0);
		assert(nre==0);
		assert(nor==0);
	}

	return weightPercentage/100 * (unallocated + qint64(nre) + qint64(nor)); //fitness factor
}

bool ConstraintBasicCompulsorySpace::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintBasicCompulsorySpace::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);

	return false;
}

bool ConstraintBasicCompulsorySpace::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintBasicCompulsorySpace::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintBasicCompulsorySpace::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintRoomNotAvailableTimes::ConstraintRoomNotAvailableTimes()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES;
}

ConstraintRoomNotAvailableTimes::ConstraintRoomNotAvailableTimes(double wp, const QString& rn, const QList<int>& d, const QList<int>& h)
	: SpaceConstraint(wp)
{
	this->room=rn;
	this->days=d;
	this->hours=h;
	this->type=CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES;
}

bool ConstraintRoomNotAvailableTimes::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintRoomNotAvailableTimes::getXmlDescription(Rules& r)
{
	QString s=IL2+"<ConstraintRoomNotAvailableTimes>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Room>"+protect(this->room)+"</Room>\n";

	s+=IL3+"<Number_of_Not_Available_Times>"+QString::number(this->days.count())+"</Number_of_Not_Available_Times>\n";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		s+=IL3+"<Not_Available_Time>\n";
		if(this->days.at(i)>=0)
			s+=IL4+"<Day>"+protect(r.daysOfTheWeek[this->days.at(i)])+"</Day>\n";
		if(this->hours.at(i)>=0)
			s+=IL4+"<Hour>"+protect(r.hoursOfTheDay[this->hours.at(i)])+"</Hour>\n";
		s+=IL3+"</Not_Available_Time>\n";
	}

	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintRoomNotAvailableTimes>\n";
	return s;
}

QString ConstraintRoomNotAvailableTimes::getDescription(Rules& r)
{
	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Room not available");s+=translatedCommaSpace();
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	s+=tr("R:%1", "Room").arg(this->room);s+=translatedCommaSpace();

	s+=tr("NA at:", "Not available at");
	s+=" ";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		if(this->days.at(i)>=0){
			s+=r.daysOfTheWeek[this->days.at(i)];
			s+=" ";
		}
		if(this->hours.at(i)>=0){
			s+=r.hoursOfTheDay[this->hours.at(i)];
		}
		if(i<days.count()-1)
			s+=translatedSemicolonSpace();
	}

	return begin+s+end;
}

QString ConstraintRoomNotAvailableTimes::getDetailedDescription(Rules& r)
{
	QString s=tr("Space constraint");s+="\n";
	s+=tr("Room not available");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Room=%1").arg(this->room);s+="\n";

	s+=tr("Not available at:", "It refers to a room");
	s+="\n";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		if(this->days.at(i)>=0){
			s+=r.daysOfTheWeek[this->days.at(i)];
			s+=" ";
		}
		if(this->hours.at(i)>=0){
			s+=r.hoursOfTheDay[this->hours.at(i)];
		}
		if(i<days.count()-1)
			s+=translatedSemicolonSpace();
	}
	s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

bool ConstraintRoomNotAvailableTimes::computeInternalStructure(QWidget* parent, Rules& r){
	//this->room_ID=r.searchRoom(this->room);
	room_ID=r.roomsHash.value(room, -1);
	
	if(this->room_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint room not available times is wrong because it refers to nonexistent room."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}

	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		if(this->days.at(k) >= r.nDaysPerWeek){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET information"),
			 tr("Constraint room not available times is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
			return false;
		}
		if(this->hours.at(k) >= r.nHoursPerDay){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET information"),
			 tr("Constraint room not available times is wrong because an hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
			return false;
		}
	}
	
	assert(this->room_ID>=0);
	
	return true;
}

double ConstraintRoomNotAvailableTimes::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of hours when the room is supposed to be occupied,
	//but it is not available
	//This function considers all the hours, I mean if there are for example 5 weekly courses
	//scheduled on that hour (which is already a broken compulsory constraint - we only
	//are allowed 1 weekly activity for a certain room at a certain hour) we calculate
	//5 broken constraints for that function.
	//TODO: decide if it is better to consider only 2 or 10 as a return value in this particular case
	//(currently it is 10)
	int rm=this->room_ID;

	int nbroken;

	nbroken=0;

	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		int d=days.at(k);
		int h=hours.at(k);
		
		if(roomsMatrix[rm][d][h]>0){
			nbroken+=roomsMatrix[rm][d][h];
	
			if(conflictsString!=nullptr){
				QString s= tr("Space constraint room not available times broken for room: %1, on day %2, hour %3")
				 .arg(r.internalRoomsList[rm]->name)
				 .arg(r.daysOfTheWeek[d])
				 .arg(r.hoursOfTheDay[h]);
				s += ". ";
				s += tr("This increases the conflicts total by %1")
				 .arg(CustomFETString::numberPlusTwoDigitsPrecision(roomsMatrix[rm][d][h]*weightPercentage/100));
				
				dl.append(s);
				cl.append(roomsMatrix[rm][d][h]*weightPercentage/100);
			
				*conflictsString += s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintRoomNotAvailableTimes::isRelatedToRoom(Room* r)
{
	return this->room==r->name;
}

bool ConstraintRoomNotAvailableTimes::hasWrongDayOrHour(Rules& r)
{
	assert(days.count()==hours.count());
	
	for(int i=0; i<days.count(); i++)
		if(days.at(i)<0 || days.at(i)>=r.nDaysPerWeek
		 || hours.at(i)<0 || hours.at(i)>=r.nHoursPerDay)
			return true;

	return false;
}

bool ConstraintRoomNotAvailableTimes::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintRoomNotAvailableTimes::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(days.count()==hours.count());
	
	QList<int> newDays;
	QList<int> newHours;
	
	for(int i=0; i<days.count(); i++)
		if(days.at(i)>=0 && days.at(i)<r.nDaysPerWeek
		 && hours.at(i)>=0 && hours.at(i)<r.nHoursPerDay){
			newDays.append(days.at(i));
			newHours.append(hours.at(i));
		}
	
	days=newDays;
	hours=newHours;
	
	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherRoomNotAvailableTimes::ConstraintTeacherRoomNotAvailableTimes()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES;
}

ConstraintTeacherRoomNotAvailableTimes::ConstraintTeacherRoomNotAvailableTimes(double wp, const QString& tn, const QString& rn, const QList<int>& d, const QList<int>& h)
	: SpaceConstraint(wp)
{
	this->teacherName=tn;
	this->room=rn;
	this->days=d;
	this->hours=h;
	this->type=CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES;
}

bool ConstraintTeacherRoomNotAvailableTimes::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherRoomNotAvailableTimes::getXmlDescription(Rules& r){
	QString s=IL2+"<ConstraintTeacherRoomNotAvailableTimes>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+=IL3+"<Room>"+protect(this->room)+"</Room>\n";

	s+=IL3+"<Number_of_Not_Available_Times>"+QString::number(this->days.count())+"</Number_of_Not_Available_Times>\n";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		s+=IL3+"<Not_Available_Time>\n";
		if(this->days.at(i)>=0)
			s+=IL4+"<Day>"+protect(r.daysOfTheWeek[this->days.at(i)])+"</Day>\n";
		if(this->hours.at(i)>=0)
			s+=IL4+"<Hour>"+protect(r.hoursOfTheDay[this->hours.at(i)])+"</Hour>\n";
		s+=IL3+"</Not_Available_Time>\n";
	}

	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeacherRoomNotAvailableTimes>\n";
	return s;
}

QString ConstraintTeacherRoomNotAvailableTimes::getDescription(Rules& r){
	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher room not available");s+=translatedCommaSpace();
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	s+=tr("T:%1", "Teacher").arg(this->teacherName);s+=translatedCommaSpace();
	s+=tr("R:%1", "Room").arg(this->room);s+=translatedCommaSpace();

	s+=tr("NA at:", "Not available at");
	s+=" ";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		if(this->days.at(i)>=0){
			s+=r.daysOfTheWeek[this->days.at(i)];
			s+=" ";
		}
		if(this->hours.at(i)>=0){
			s+=r.hoursOfTheDay[this->hours.at(i)];
		}
		if(i<days.count()-1)
			s+=translatedSemicolonSpace();
	}

	return begin+s+end;
}

QString ConstraintTeacherRoomNotAvailableTimes::getDetailedDescription(Rules& r){
	QString s=tr("Space constraint");s+="\n";
	s+=tr("Teacher room not available");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";
	s+=tr("Room=%1").arg(this->room);s+="\n";

	s+=tr("Not available at:", "It refers to a room");
	s+="\n";
	assert(days.count()==hours.count());
	for(int i=0; i<days.count(); i++){
		if(this->days.at(i)>=0){
			s+=r.daysOfTheWeek[this->days.at(i)];
			s+=" ";
		}
		if(this->hours.at(i)>=0){
			s+=r.hoursOfTheDay[this->hours.at(i)];
		}
		if(i<days.count()-1)
			s+=translatedSemicolonSpace();
	}
	s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

bool ConstraintTeacherRoomNotAvailableTimes::computeInternalStructure(QWidget* parent, Rules& r){
	teacher_ID=r.teachersHash.value(teacherName, -1);
	if(this->teacher_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher room not available times is wrong because it refers to nonexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	//this->room_ID=r.searchRoom(this->room);
	room_ID=r.roomsHash.value(room, -1);
	
	if(this->room_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher room not available times is wrong because it refers to nonexistent room."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
		return false;
	}	

	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		if(this->days.at(k) >= r.nDaysPerWeek){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET information"),
			 tr("Constraint teacher room not available times is wrong because it refers to removed day. Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}		
		if(this->hours.at(k) >= r.nHoursPerDay){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET information"),
			 tr("Constraint teacher room not available times is wrong because an hour is too late (after the last acceptable slot). Please correct"
			 " and try again. Correcting means editing the constraint and updating information. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		 
			return false;
		}
	}
	
	assert(this->teacher_ID>=0);
	assert(this->room_ID>=0);
	
	return true;
}

double ConstraintTeacherRoomNotAvailableTimes::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrices roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of hours when the teacher+room is supposed to be occupied,
	//but it is not available
	//This function consideres all the hours, I mean if there are for example 5 weekly courses
	//scheduled on that hour (which is already a broken compulsory constraint - we only
	//are allowed 1 weekly activity for a certain room at a certain hour) we calculate
	//5 broken constraints for that function.
	//TODO: decide if it is better to consider only 2 or 10 as a return value in this particular case
	//(currently it is 10)

	int nbroken=0;

	Teacher* tchpointer=r.internalTeachersList[teacher_ID];
	Matrix2D<QSet<int>> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtRoomsTimetable[d2][h2].clear();

	for(int ai : std::as_const(tchpointer->activitiesForTeacher))
		if(c.times[ai]!=UNALLOCATED_TIME && c.rooms[ai]!=UNALLOCATED_SPACE && c.rooms[ai]!=UNSPECIFIED_ROOM){
			int d2=c.times[ai]%r.nDaysPerWeek;
			int h2=c.times[ai]/r.nDaysPerWeek;

			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h2+dur<r.nHoursPerDay);
				assert(!crtRoomsTimetable[d2][h2+dur].contains(c.rooms[ai]));
				assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
				crtRoomsTimetable[d2][h2+dur].insert(c.rooms[ai]);

				Room* vr=r.internalRoomsList[c.rooms[ai]];
				if(vr->isVirtual){
					for(int rr : std::as_const(c.realRoomsList[ai])){
						assert(!crtRoomsTimetable[d2][h2+dur].contains(rr));
						assert(rr>=0 && rr<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur].insert(rr);
					}
				}
			}
		}

	assert(days.count()==hours.count());
	for(int k=0; k<days.count(); k++){
		int d=days.at(k);
		int h=hours.at(k);

		if(crtRoomsTimetable[d][h].contains(room_ID)){
			nbroken++;

			if(conflictsString!=nullptr){
				QString s=tr("Space constraint teacher room not available times broken for teacher: %1, room: %2, on day %3, hour %4")
				 .arg(r.internalTeachersList[teacher_ID]->name)
				 .arg(r.internalRoomsList[room_ID]->name)
				 .arg(r.daysOfTheWeek[d])
				 .arg(r.hoursOfTheDay[h]);
				s += ". ";
				s += tr("This increases the conflicts total by %1")
				 .arg(CustomFETString::numberPlusTwoDigitsPrecision(1*weightPercentage/100));

				dl.append(s);
				cl.append(1*weightPercentage/100);

				*conflictsString += s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherRoomNotAvailableTimes::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherRoomNotAvailableTimes::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherRoomNotAvailableTimes::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherRoomNotAvailableTimes::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherRoomNotAvailableTimes::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherRoomNotAvailableTimes::isRelatedToRoom(Room* r)
{
	return this->room==r->name;
}

bool ConstraintTeacherRoomNotAvailableTimes::hasWrongDayOrHour(Rules& r)
{
	assert(days.count()==hours.count());
	
	for(int i=0; i<days.count(); i++)
		if(days.at(i)<0 || days.at(i)>=r.nDaysPerWeek
		 || hours.at(i)<0 || hours.at(i)>=r.nHoursPerDay)
			return true;

	return false;
}

bool ConstraintTeacherRoomNotAvailableTimes::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeacherRoomNotAvailableTimes::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(days.count()==hours.count());
	
	QList<int> newDays;
	QList<int> newHours;
	
	for(int i=0; i<days.count(); i++)
		if(days.at(i)>=0 && days.at(i)<r.nDaysPerWeek
		 && hours.at(i)>=0 && hours.at(i)<r.nHoursPerDay){
			newDays.append(days.at(i));
			newHours.append(hours.at(i));
		}
	
	days=newDays;
	hours=newHours;
	
	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredRoom::ConstraintActivityPreferredRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_ROOM;
}

ConstraintActivityPreferredRoom::ConstraintActivityPreferredRoom(double wp, int aid, const QString& room, const QStringList& realRooms, bool perm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_ROOM;
	this->activityId=aid;
	this->roomName=room;
	this->preferredRealRoomsNames=realRooms;
	this->permanentlyLocked=perm;
}

bool ConstraintActivityPreferredRoom::operator==(ConstraintActivityPreferredRoom& c){
	if(this->roomName!=c.roomName)
		return false;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
	if(QSet<QString>(this->preferredRealRoomsNames.constBegin(), this->preferredRealRoomsNames.constEnd()) !=
	 QSet<QString>(c.preferredRealRoomsNames.constBegin(), c.preferredRealRoomsNames.constEnd()))
#else
	if(this->preferredRealRoomsNames.toSet()!=c.preferredRealRoomsNames.toSet())
#endif
		return false;
	if(this->activityId!=c.activityId)
		return false;
	if(this->weightPercentage!=c.weightPercentage)
		return false;
	if(this->active!=c.active)
		return false;
	//no need to care about permanently locked
	return true;
}

bool ConstraintActivityPreferredRoom::computeInternalStructure(QWidget* parent, Rules& r)
{
	this->_activity=-1;
	int ac=r.activitiesHash.value(activityId, -1);
	assert(ac>=0);
	_activity=ac;
	/*for(ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].id==this->activityId){
			assert(this->_activity==-1);
			this->_activity=ac;
			break;
		}
	if(ac==r.nInternalActivities){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
			tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}*/
		
	//this->_room = r.searchRoom(this->roomName);
	_room=r.roomsHash.value(roomName, -1);
	//assert(_room>=0);

	if(this->_room<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
			tr("Following constraint is wrong (because preferred room %1 does not exist):\n%2").arg(roomName).arg(this->getDetailedDescription(r)));
		return false;
	}

	assert(this->_room>=0);
	
	preferredRealRooms.clear();
	for(const QString& rrn : std::as_const(preferredRealRoomsNames)){
		int rr=r.roomsHash.value(rrn, -1);

		if(rr<0){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
				tr("Following constraint is wrong (because preferred real room %1 does not exist):\n%2").arg(rrn).arg(this->getDetailedDescription(r)));
			return false;
		}
		
		assert(rr>=0);
		
		preferredRealRooms.insert(rr);
	}
	
	return true;
}

bool ConstraintActivityPreferredRoom::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->activityId))
		return true;

	return false;
}

QString ConstraintActivityPreferredRoom::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintActivityPreferredRoom>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Activity_Id>"+CustomFETString::number(this->activityId)+"</Activity_Id>\n";
	s+=IL3+"<Room>"+protect(this->roomName)+"</Room>\n";

	if(!preferredRealRoomsNames.isEmpty()){
		s+=IL3+"<Number_of_Real_Rooms>"+QString::number(preferredRealRoomsNames.count())+"</Number_of_Real_Rooms>\n";
		for(const QString& rrn : std::as_const(preferredRealRoomsNames))
			s+=IL3+"<Real_Room>"+protect(rrn)+"</Real_Room>\n";
	}
	
	s+=IL3+"<Permanently_Locked>";s+=trueFalse(this->permanentlyLocked);s+="</Permanently_Locked>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintActivityPreferredRoom>\n";

	return s;
}

QString ConstraintActivityPreferredRoom::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Activity preferred room"); s+=translatedCommaSpace();
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	s+=tr("Id:%1 (%2)", "%1 is activity id, %2 is detailed description of activity")
		.arg(getActivityDescription(r, this->activityId))
		.arg(getActivityDetailedDescription(r, this->activityId));
	s+=translatedCommaSpace();

	s+=tr("R:%1", "Room").arg(this->roomName);

	if(!preferredRealRoomsNames.isEmpty()){
		s+=" (";
		QStringList sl;
		for(const QString& rrn : std::as_const(preferredRealRoomsNames))
			sl.append(tr("RR:%1", "Real room").arg(rrn));
		s+=sl.join(translatedCommaSpace());
		s+=")";
	}
	
	s+=translatedCommaSpace();
	s+=tr("PL:%1", "Abbreviation for permanently locked").arg(yesNoTranslated(this->permanentlyLocked));

	return begin+s+end;
}

QString ConstraintActivityPreferredRoom::getDetailedDescription(Rules& r)
{
	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Activity preferred room"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	
	s+=tr("Activity id=%1 (%2)", "%1 is activity id, %2 is detailed description of activity")
		.arg(this->activityId)
		.arg(getActivityDetailedDescription(r, this->activityId));
	s+="\n";
	
	s+=tr("Room=%1").arg(this->roomName);s+="\n";

	if(!preferredRealRoomsNames.isEmpty())
		for(const QString& rrn : std::as_const(preferredRealRoomsNames)){
			s+=tr("Real room=%1").arg(rrn);
			s+="\n";
		}
	
	if(this->permanentlyLocked){
		s+=tr("This activity is permanently locked, which means you cannot unlock it from the 'Timetable' menu"
		" (you can unlock this activity by removing the constraint from the constraints dialog or by setting the 'permanently"
		" locked' attribute false when editing this constraint)");
	}
	else{
		s+=tr("This activity is not permanently locked, which means you can unlock it from the 'Timetable' menu");
	}
	s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivityPreferredRoom::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts

	int nbroken;
	
	bool ok=true;

	nbroken=0;

	int rm=c.rooms[this->_activity];
	if(/*rm!=UNALLOCATED_SPACE &&*/ rm!=this->_room){
		if(rm!=UNALLOCATED_SPACE){
			ok=false;

			if(conflictsString!=nullptr){
				QString s=tr("Space constraint activity preferred room broken for activity with id=%1 (%2), room=%3",
					"%1 is activity id, %2 is detailed description of activity")
					.arg(this->activityId)
					.arg(getActivityDetailedDescription(r, this->activityId))
					.arg(this->roomName);
					s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* 1));
		
				dl.append(s);
				cl.append(1*weightPercentage/100);
			
				*conflictsString += s+"\n";
			}
			nbroken++;
		}
	}
	
	if(rm!=UNALLOCATED_SPACE){
		if(!preferredRealRooms.isEmpty()){
			assert(this->weightPercentage==100.0);
		
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
			if(preferredRealRooms!=QSet<int>(c.realRoomsList[this->_activity].constBegin(), c.realRoomsList[this->_activity].constEnd())){
#else
			if(preferredRealRooms!=c.realRoomsList[this->_activity].toSet()){
#endif
				ok=false;

				if(conflictsString!=nullptr){
					QString s=tr("Space constraint activity preferred room broken for activity with id=%1 (%2), room=%3, the preferred real rooms are not satisfied",
						"%1 is activity id, %2 is detailed description of activity")
						.arg(this->activityId)
						.arg(getActivityDetailedDescription(r, this->activityId))
						.arg(this->roomName);
						s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* 1));
			
					dl.append(s);
					cl.append(1*weightPercentage/100);
			
					*conflictsString += s+"\n";
				}
				nbroken++;
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok);

	return weightPercentage/100 * nbroken;
}

bool ConstraintActivityPreferredRoom::isRelatedToActivity(Activity* a)
{
	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredRoom::isRelatedToRoom(Room* r)
{
	if(r->name==this->roomName)
		return true;
		
	if(preferredRealRoomsNames.contains(r->name))
		return true;
		
	return false;
}

bool ConstraintActivityPreferredRoom::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivityPreferredRoom::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintActivityPreferredRoom::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityPreferredRooms::ConstraintActivityPreferredRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_ROOMS;
}

ConstraintActivityPreferredRooms::ConstraintActivityPreferredRooms(double wp, int aid, const QStringList& roomsList)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_ACTIVITY_PREFERRED_ROOMS;
	this->activityId=aid;
	this->roomsNames=roomsList;
}

bool ConstraintActivityPreferredRooms::computeInternalStructure(QWidget* parent, Rules& r)
{
	_activity=r.activitiesHash.value(activityId, r.nInternalActivities);
	int ac=_activity;

	/*this->_activity=-1;
	int ac;
	for(ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].id==this->activityId){
			assert(this->_activity==-1);
			this->_activity=ac;
			break;
		}*/
		
	if(ac==r.nInternalActivities){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
			tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	
	this->_rooms.clear();
	for(const QString& rm : std::as_const(this->roomsNames)){
		//int t=r.searchRoom(rm);
		int t=r.roomsHash.value(rm, -1);

		if(t<0){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
				tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}

		assert(t>=0);
		this->_rooms.append(t);
	}
		
	return true;
}

bool ConstraintActivityPreferredRooms::hasInactiveActivities(Rules& r)
{
	if(r.inactiveActivities.contains(this->activityId))
		return true;

	return false;
}

QString ConstraintActivityPreferredRooms::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintActivityPreferredRooms>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Activity_Id>"+CustomFETString::number(this->activityId)+"</Activity_Id>\n";
	s+=IL3+"<Number_of_Preferred_Rooms>"+QString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++)
		s+=IL3+"<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintActivityPreferredRooms>\n";

	return s;
}

QString ConstraintActivityPreferredRooms::getDescription(Rules& r)
{
	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Activity preferred rooms"); s+=translatedCommaSpace();
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	s+=tr("Id:%1 (%2)", "%1 is activity id, %2 is detailed description of activity")
		.arg(getActivityDescription(r, this->activityId))
		.arg(getActivityDetailedDescription(r, this->activityId));

	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++){
		s+=translatedCommaSpace();
		s+=tr("R:%1", "Room").arg(*it);
	}

	return begin+s+end;
}

QString ConstraintActivityPreferredRooms::getDetailedDescription(Rules& r)
{
	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Activity preferred rooms"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	
	s+=tr("Activity id=%1 (%2)", "%1 is activity id, %2 is detailed description of activity")
		.arg(this->activityId)
		.arg(getActivityDetailedDescription(r, this->activityId));
	s+="\n";
	
	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++){
		s+=tr("Room=%1").arg(*it);
		s+="\n";
	}

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivityPreferredRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts

	int nbroken;
	
	bool ok=true;

	nbroken=0;

	int rm=c.rooms[this->_activity];
	if(true || rm!=UNALLOCATED_SPACE){
		int i;
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms.at(i)==rm)
				break;
		if(i==this->_rooms.count()){
			if(rm!=UNALLOCATED_SPACE){
				ok=false;
		
				if(conflictsString!=nullptr){
					QString s=tr("Space constraint activity preferred rooms broken for activity with id=%1 (%2)"
						, "%1 is activity id, %2 is detailed description of activity")
						.arg(this->activityId)
						.arg(getActivityDetailedDescription(r, this->activityId));
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100 * 1));
				
					dl.append(s);
					cl.append(weightPercentage/100 * 1);
				
					*conflictsString += s+"\n";
				}

				nbroken++;
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok);

	return weightPercentage/100 * nbroken;
}

bool ConstraintActivityPreferredRooms::isRelatedToActivity(Activity* a)
{
	if(this->activityId==a->id)
		return true;
	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityPreferredRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

bool ConstraintActivityPreferredRooms::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivityPreferredRooms::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintActivityPreferredRooms::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetHomeRoom::ConstraintStudentsSetHomeRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_HOME_ROOM;
}

ConstraintStudentsSetHomeRoom::ConstraintStudentsSetHomeRoom(double wp, const QString& st, const QString& rm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_HOME_ROOM;
	this->studentsName=st;
	this->roomName=rm;
}

bool ConstraintStudentsSetHomeRoom::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	//QStringList::iterator it;
	Activity* act;

	this->_activities.clear();
	
	for(int ac=0; ac<r.nInternalActivities; ac++){
		act=&r.internalActivitiesList[ac];

		//check if this activity has the corresponding students
		bool sameStudents=false;
		if(act->studentsNames.count()==1)
			if(act->studentsNames.at(0)==studentsName)
				sameStudents=true;
	
		if(!sameStudents)
			continue;
		
		this->_activities.append(ac);
	}

	//this->_room = r.searchRoom(this->roomName);
	_room=r.roomsHash.value(roomName, -1);
	if(this->_room<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
			tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
	assert(this->_room>=0);
	
	return true;
}

bool ConstraintStudentsSetHomeRoom::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetHomeRoom::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsSetHomeRoom>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Students>"+protect(this->studentsName)+"</Students>\n";
	s+=IL3+"<Room>"+protect(this->roomName)+"</Room>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsSetHomeRoom>\n";

	return s;
}

QString ConstraintStudentsSetHomeRoom::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set home room"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=translatedCommaSpace();

	s+=tr("R:%1", "R means Room").arg(this->roomName);

	return begin+s+end;
}

QString ConstraintStudentsSetHomeRoom::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Students set home room"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=tr("Room=%1").arg(this->roomName);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetHomeRoom::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	for(int ac : std::as_const(this->_activities)){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE) //counted as unallocated
			continue;
		
		bool ok=true;
		//if(rm!=this->_room)
		if(rm==UNSPECIFIED_ROOM) //it may be other room, from subject (activity tag) preferred room(s), which is OK
			ok=false;
		else if(rm==this->_room){
		} //OK
		else{ //other room, from subject (activity tag) pref. room(s)
			bool okk=false;
			for(const PreferredRoomsItem& it : std::as_const(activitiesPreferredRoomsList[ac]))
				if(it.preferredRooms.contains(rm))
					okk=true;
			assert(okk);
			//assert(activitiesPreferredRoomsPreferredRooms[ac].contains(rm));
		}

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
		
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint students set home room broken for activity with id %1 (%2)",
					"%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id));
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
				
				*conflictsString+=s+"\n";
			}

			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToActivity(Activity* a)
{
	if(a->studentsNames.count()==1)
		if(a->studentsNames.at(0)==studentsName)
			return true;

	return false;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);

	return s->name==this->studentsName;
}

bool ConstraintStudentsSetHomeRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

bool ConstraintStudentsSetHomeRoom::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintStudentsSetHomeRoom::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintStudentsSetHomeRoom::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetHomeRooms::ConstraintStudentsSetHomeRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_HOME_ROOMS;
}

ConstraintStudentsSetHomeRooms::ConstraintStudentsSetHomeRooms(double wp, const QString& st, const QStringList& rms)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_HOME_ROOMS;

	this->studentsName=st;

	this->roomsNames=rms;
}

bool ConstraintStudentsSetHomeRooms::computeInternalStructure(QWidget* parent, Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the constraint.
	
	this->_activities.clear();

	//QStringList::iterator it;
	Activity* act;

	for(int ac=0; ac<r.nInternalActivities; ac++){
		act=&r.internalActivitiesList[ac];

		//check if this activity has the corresponding students
		bool sameStudents=false;
		if(act->studentsNames.count()==1)
			if(act->studentsNames.at(0)==studentsName)
				sameStudents=true;
	
		if(!sameStudents)
			continue;
		
		this->_activities.append(ac);
	}

	this->_rooms.clear();

	for(const QString& rm : std::as_const(this->roomsNames)){
		//int t=r.searchRoom(rm);
		int t=r.roomsHash.value(rm, -1);
		if(t<0){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
				tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}
		else{
			assert(t>=0);
			this->_rooms.append(t);
		}
	}
	
	return true;
}

bool ConstraintStudentsSetHomeRooms::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetHomeRooms::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsSetHomeRooms>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Students>"+protect(this->studentsName)+"</Students>\n";
	s+=IL3+"<Number_of_Preferred_Rooms>"+QString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++)
		s+=IL3+"<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsSetHomeRooms>\n";

	return s;
}

QString ConstraintStudentsSetHomeRooms::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set home rooms"); s+=translatedCommaSpace();
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();

	s+=tr("St:%1", "St means students").arg(this->studentsName);

	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++){
		s+=translatedCommaSpace();
		s+=tr("R:%1", "R means Room").arg(*it);
	}

	return begin+s+end;
}

QString ConstraintStudentsSetHomeRooms::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Students set home rooms"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++){
		s+=tr("Room=%1").arg(*it);
		s+="\n";
	}

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetHomeRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	for(int ac : std::as_const(this->_activities)){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE)
			continue;
	
		bool ok=true;
		int i;
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms[i]==rm)
				break;
		if(i==this->_rooms.count()){
			if(rm==UNSPECIFIED_ROOM)
				ok=false;
			else{
				bool okk=false;
				for(const PreferredRoomsItem& it : std::as_const(activitiesPreferredRoomsList[ac]))
					if(it.preferredRooms.contains(rm))
						okk=true;
				assert(okk);
				//assert(activitiesPreferredRoomsPreferredRooms[ac].contains(rm));
			}
		}

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
			
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint students set home rooms broken for activity with id %1 (%2)"
					, "%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id));
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
			
				*conflictsString+=s+"\n";
			}
			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToActivity(Activity* a)
{
	if(a->studentsNames.count()==1)
		if(a->studentsNames.at(0)==studentsName)
			return true;

	return false;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s)
	
	return false;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);

	return s->name==this->studentsName;
}

bool ConstraintStudentsSetHomeRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

bool ConstraintStudentsSetHomeRooms::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintStudentsSetHomeRooms::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintStudentsSetHomeRooms::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherHomeRoom::ConstraintTeacherHomeRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_HOME_ROOM;
}

ConstraintTeacherHomeRoom::ConstraintTeacherHomeRoom(double wp, const QString& tc, const QString& rm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_HOME_ROOM;
	this->teacherName=tc;
	this->roomName=rm;
}

bool ConstraintTeacherHomeRoom::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	//QStringList::iterator it;
	Activity* act;

	this->_activities.clear();

	for(int ac=0; ac<r.nInternalActivities; ac++){
		act=&r.internalActivitiesList[ac];

		//check if this activity has the corresponding students
		bool sameTeacher=false;
		if(act->teachersNames.count()==1)
			if(act->teachersNames.at(0)==teacherName)
				sameTeacher=true;
	
		if(!sameTeacher)
			continue;
		
		this->_activities.append(ac);
	}

	//this->_room = r.searchRoom(this->roomName);
	_room=r.roomsHash.value(roomName, -1);
	if(this->_room<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
			tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
	assert(this->_room>=0);
	
	return true;
}

bool ConstraintTeacherHomeRoom::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherHomeRoom::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeacherHomeRoom>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+=IL3+"<Room>"+protect(this->roomName)+"</Room>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeacherHomeRoom>\n";

	return s;
}

QString ConstraintTeacherHomeRoom::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher home room"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=translatedCommaSpace();

	s+=tr("R:%1", "R means Room").arg(this->roomName);

	return begin+s+end;
}

QString ConstraintTeacherHomeRoom::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Teacher home room"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=tr("Room=%1").arg(this->roomName);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherHomeRoom::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	for(int ac : std::as_const(this->_activities)){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE) //counted as unallocated
			continue;
		
		bool ok=true;
		if(rm==UNSPECIFIED_ROOM) //it may be other room, from subject (activity tag) preferred room(s), which is OK
			ok=false;
		else if(rm==this->_room){
		} //OK
		else{ //other room, from subject (activity tag) pref. room(s)
			bool okk=false;
			for(const PreferredRoomsItem& it : std::as_const(activitiesPreferredRoomsList[ac]))
				if(it.preferredRooms.contains(rm))
					okk=true;
			assert(okk);
			//assert(activitiesPreferredRoomsPreferredRooms[ac].contains(rm));
		}

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
		
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint teacher home room broken for activity with id %1 (%2)",
					"%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id));
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
				
				*conflictsString+=s+"\n";
			}

			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherHomeRoom::isRelatedToActivity(Activity* a)
{
	if(a->teachersNames.count()==1)
		if(a->teachersNames.at(0)==teacherName)
			return true;

	return false;
}

bool ConstraintTeacherHomeRoom::isRelatedToTeacher(Teacher* t)
{
	return teacherName==t->name;
}

bool ConstraintTeacherHomeRoom::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherHomeRoom::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherHomeRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherHomeRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

bool ConstraintTeacherHomeRoom::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintTeacherHomeRoom::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintTeacherHomeRoom::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherHomeRooms::ConstraintTeacherHomeRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_HOME_ROOMS;
}

ConstraintTeacherHomeRooms::ConstraintTeacherHomeRooms(double wp, const QString& tc, const QStringList& rms)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_HOME_ROOMS;

	this->teacherName=tc;

	this->roomsNames=rms;
}

bool ConstraintTeacherHomeRooms::computeInternalStructure(QWidget* parent, Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the constraint.
	
	this->_activities.clear();

	//QStringList::iterator it;
	Activity* act;

	for(int ac=0; ac<r.nInternalActivities; ac++){
		act=&r.internalActivitiesList[ac];

		//check if this activity has the corresponding students
		bool sameTeacher=false;
		if(act->teachersNames.count()==1)
			if(act->teachersNames.at(0)==teacherName)
				sameTeacher=true;
	
		if(!sameTeacher)
			continue;
		
		this->_activities.append(ac);
	}

	this->_rooms.clear();

	for(const QString& rm : std::as_const(this->roomsNames)){
		//int t=r.searchRoom(rm);
		int t=r.roomsHash.value(rm, -1);
		if(t<0){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
				tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}
		else{
			assert(t>=0);
			this->_rooms.append(t);
		}
	}
	
	return true;
}

bool ConstraintTeacherHomeRooms::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherHomeRooms::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeacherHomeRooms>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+=IL3+"<Number_of_Preferred_Rooms>"+QString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++)
		s+=IL3+"<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
		
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeacherHomeRooms>\n";

	return s;
}

QString ConstraintTeacherHomeRooms::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher home rooms"); s+=translatedCommaSpace();
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();

	s+=tr("T:%1", "T means teacher").arg(this->teacherName);

	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++){
		s+=translatedCommaSpace();
		s+=tr("R:%1", "R means Room").arg(*it);
	}

	return begin+s+end;
}

QString ConstraintTeacherHomeRooms::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Teacher home rooms"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++){
		s+=tr("Room=%1").arg(*it);
		s+="\n";
	}

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherHomeRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	for(int ac : std::as_const(this->_activities)){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE)
			continue;
	
		bool ok=true;
		int i;
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms[i]==rm)
				break;
		if(i==this->_rooms.count()){
			if(rm==UNSPECIFIED_ROOM)
				ok=false;
			else{
				bool okk=false;
				for(const PreferredRoomsItem& it : std::as_const(activitiesPreferredRoomsList[ac]))
					if(it.preferredRooms.contains(rm))
						okk=true;
				assert(okk);
				//	assert(activitiesPreferredRoomsPreferredRooms[ac].contains(rm));
			}
		}

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
			
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint teacher home rooms broken for activity with id %1 (%2)",
					"%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id));
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
			
				*conflictsString+=s+"\n";
			}
			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherHomeRooms::isRelatedToActivity(Activity* a)
{
	if(a->teachersNames.count()==1)
		if(a->teachersNames.at(0)==teacherName)
			return true;

	return false;
}

bool ConstraintTeacherHomeRooms::isRelatedToTeacher(Teacher* t)
{
	return teacherName==t->name;
}

bool ConstraintTeacherHomeRooms::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s)
	
	return false;
}

bool ConstraintTeacherHomeRooms::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherHomeRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	return false;
}

bool ConstraintTeacherHomeRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

bool ConstraintTeacherHomeRooms::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintTeacherHomeRooms::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintTeacherHomeRooms::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectPreferredRoom::ConstraintSubjectPreferredRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_PREFERRED_ROOM;
}

ConstraintSubjectPreferredRoom::ConstraintSubjectPreferredRoom(double wp, const QString& subj, const QString& rm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_SUBJECT_PREFERRED_ROOM;
	this->subjectName=subj;
	this->roomName=rm;
}

bool ConstraintSubjectPreferredRoom::computeInternalStructure(QWidget* parent, Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_activities.clear();

	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName){
			this->_activities.append(ac);
		}
	
	//this->_room = r.searchRoom(this->roomName);
	_room=r.roomsHash.value(roomName, -1);
	if(this->_room<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
			tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	assert(this->_room>=0);
	
	return true;
}

bool ConstraintSubjectPreferredRoom::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintSubjectPreferredRoom::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintSubjectPreferredRoom>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+=IL3+"<Room>"+protect(this->roomName)+"</Room>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintSubjectPreferredRoom>\n";

	return s;
}

QString ConstraintSubjectPreferredRoom::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Subject preferred room"); s+=translatedCommaSpace();
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	s+=tr("S:%1", "Subject").arg(this->subjectName);s+=translatedCommaSpace();
	s+=tr("R:%1", "Room").arg(this->roomName);

	return begin+s+end;
}

QString ConstraintSubjectPreferredRoom::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Subject preferred room"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Subject=%1").arg(this->subjectName);s+="\n";
	s+=tr("Room=%1").arg(this->roomName);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintSubjectPreferredRoom::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	for(int ac : std::as_const(this->_activities)){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE) //counted as unallocated
			continue;
		
		bool ok=true;
		if(rm!=this->_room)
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
		
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint subject preferred room broken for activity with id %1 (%2)"
					, "%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id));
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
				
				*conflictsString+=s+"\n";
			}

			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintSubjectPreferredRoom::isRelatedToActivity(Activity* a)
{
	return a->subjectName==this->subjectName;
}

bool ConstraintSubjectPreferredRoom::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubjectPreferredRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

bool ConstraintSubjectPreferredRoom::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintSubjectPreferredRoom::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintSubjectPreferredRoom::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectPreferredRooms::ConstraintSubjectPreferredRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_PREFERRED_ROOMS;
}

ConstraintSubjectPreferredRooms::ConstraintSubjectPreferredRooms(double wp, const QString& subj, const QStringList& rms)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_SUBJECT_PREFERRED_ROOMS;
	this->subjectName=subj;
	this->roomsNames=rms;
}

bool ConstraintSubjectPreferredRooms::computeInternalStructure(QWidget* parent, Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_activities.clear();

	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName){
			this->_activities.append(ac);
		}
	
	this->_rooms.clear();
	for(const QString& rm : std::as_const(this->roomsNames)){
		//int t=r.searchRoom(rm);
		int t=r.roomsHash.value(rm, -1);
		if(t<0){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
				tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}
		assert(t>=0);
		this->_rooms.append(t);
	}

	return true;
}

bool ConstraintSubjectPreferredRooms::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintSubjectPreferredRooms::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintSubjectPreferredRooms>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+=IL3+"<Number_of_Preferred_Rooms>"+QString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++)
		s+=IL3+"<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintSubjectPreferredRooms>\n";

	return s;
}

QString ConstraintSubjectPreferredRooms::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Subject preferred rooms"); s+=translatedCommaSpace();
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	s+=tr("S:%1", "Subject").arg(this->subjectName);
	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++){
		s+=translatedCommaSpace();
		s+=tr("R:%1", "Room").arg(*it);
	}

	return begin+s+end;
}

QString ConstraintSubjectPreferredRooms::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Subject preferred rooms"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Subject=%1").arg(this->subjectName);s+="\n";
	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++){
		s+=tr("Room=%1").arg(*it);
		s+="\n";
	}

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintSubjectPreferredRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	for(int ac : std::as_const(this->_activities)){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE)
			continue;
	
		bool ok=true;
		int i;
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms.at(i)==rm)
				break;
		if(i==this->_rooms.count())
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
			
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint subject preferred rooms broken for activity with id %1 (%2)",
					"%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id));
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
			
				*conflictsString+=s+"\n";
			}
			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintSubjectPreferredRooms::isRelatedToActivity(Activity* a)
{
	return a->subjectName==this->subjectName;
}

bool ConstraintSubjectPreferredRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubjectPreferredRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

bool ConstraintSubjectPreferredRooms::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintSubjectPreferredRooms::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintSubjectPreferredRooms::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectActivityTagPreferredRoom::ConstraintSubjectActivityTagPreferredRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM;
}

ConstraintSubjectActivityTagPreferredRoom::ConstraintSubjectActivityTagPreferredRoom(double wp, const QString& subj, const QString& subjTag, const QString& rm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM;
	this->subjectName=subj;
	this->activityTagName=subjTag;
	this->roomName=rm;
}

bool ConstraintSubjectActivityTagPreferredRoom::computeInternalStructure(QWidget* parent, Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_activities.clear();
	
	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName
		 && r.internalActivitiesList[ac].activityTagsNames.contains(this->activityTagName)){
		 	this->_activities.append(ac);
		}
		
	//this->_room = r.searchRoom(this->roomName);
	_room=r.roomsHash.value(roomName, -1);
	if(this->_room<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
			tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	assert(this->_room>=0);
	
	return true;
}

bool ConstraintSubjectActivityTagPreferredRoom::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintSubjectActivityTagPreferredRoom::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintSubjectActivityTagPreferredRoom>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+=IL3+"<Activity_Tag>"+protect(this->activityTagName)+"</Activity_Tag>\n";
	s+=IL3+"<Room>"+protect(this->roomName)+"</Room>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintSubjectActivityTagPreferredRoom>\n";

	return s;
}

QString ConstraintSubjectActivityTagPreferredRoom::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Subject activity tag preferred room"); s+=translatedCommaSpace();
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	s+=tr("S:%1", "Subject").arg(this->subjectName);s+=translatedCommaSpace();
	s+=tr("AT:%1", "Activity tag").arg(this->activityTagName);s+=translatedCommaSpace();
	s+=tr("R:%1", "Room").arg(this->roomName);

	return begin+s+end;
}

QString ConstraintSubjectActivityTagPreferredRoom::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Subject activity tag preferred room"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Subject=%1").arg(this->subjectName);s+="\n";
	s+=tr("Activity tag=%1").arg(this->activityTagName);s+="\n";
	s+=tr("Room=%1").arg(this->roomName);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintSubjectActivityTagPreferredRoom::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	for(int ac : std::as_const(this->_activities)){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE) //counted as unallocated
			continue;
		
		bool ok=true;
		if(rm!=this->_room)
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
		
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint subject activity tag preferred room broken for activity with id %1 (%2) (activity tag of constraint=%3)",
					"%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id))
					.arg(this->activityTagName);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
				
				*conflictsString+=s+"\n";
			}

			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToActivity(Activity* a)
{
	return this->subjectName==a->subjectName && a->activityTagsNames.contains(this->activityTagName);
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToActivityTag(ActivityTag* s)
{
	if(this->activityTagName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubjectActivityTagPreferredRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

bool ConstraintSubjectActivityTagPreferredRoom::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintSubjectActivityTagPreferredRoom::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintSubjectActivityTagPreferredRoom::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintSubjectActivityTagPreferredRooms::ConstraintSubjectActivityTagPreferredRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS;
}

ConstraintSubjectActivityTagPreferredRooms::ConstraintSubjectActivityTagPreferredRooms(double wp, const QString& subj, const QString& subjTag, const QStringList& rms)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS;
	this->subjectName=subj;
	this->activityTagName=subjTag;
	this->roomsNames=rms;
}

bool ConstraintSubjectActivityTagPreferredRooms::computeInternalStructure(QWidget* parent, Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_activities.clear();

	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].subjectName == this->subjectName
		 && r.internalActivitiesList[ac].activityTagsNames.contains(this->activityTagName)){
			this->_activities.append(ac);
		}

	this->_rooms.clear();
	for(const QString& rm : std::as_const(roomsNames)){
		//int t=r.searchRoom(rm);
		int t=r.roomsHash.value(rm, -1);
		if(t<0){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
				tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}
		assert(t>=0);
		this->_rooms.append(t);
	}
	
	return true;
}

bool ConstraintSubjectActivityTagPreferredRooms::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintSubjectActivityTagPreferredRooms::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintSubjectActivityTagPreferredRooms>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+=IL3+"<Activity_Tag>"+protect(this->activityTagName)+"</Activity_Tag>\n";
	s+=IL3+"<Number_of_Preferred_Rooms>"+QString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++)
		s+=IL3+"<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintSubjectActivityTagPreferredRooms>\n";

	return s;
}

QString ConstraintSubjectActivityTagPreferredRooms::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Subject activity tag preferred rooms"); s+=translatedCommaSpace();
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	s+=tr("S:%1", "Subject").arg(this->subjectName);s+=translatedCommaSpace();
	s+=tr("AT:%1", "Activity tag").arg(this->activityTagName);
	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++){
		s+=translatedCommaSpace();
		s+=tr("R:%1", "Room").arg(*it);
	}

	return begin+s+end;
}

QString ConstraintSubjectActivityTagPreferredRooms::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Subject activity tag preferred rooms"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Subject=%1").arg(this->subjectName);s+="\n";
	s+=tr("Activity tag=%1").arg(this->activityTagName);s+="\n";
	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++){
		s+=tr("Room=%1").arg(*it);
		s+="\n";
	}

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintSubjectActivityTagPreferredRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	for(int ac : std::as_const(this->_activities)){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE)
			continue;
	
		bool ok=true;
		int i;
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms.at(i)==rm)
				break;
		if(i==this->_rooms.count())
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
			
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint subject activity tag preferred rooms broken for activity with id %1 (%2) (activity tag of constraint=%3)",
					"%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id))
					.arg(this->activityTagName);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
			
				*conflictsString+=s+"\n";
			}
			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToActivity(Activity* a)
{
	return this->subjectName==a->subjectName && a->activityTagsNames.contains(this->activityTagName);
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToSubject(Subject* s)
{
	if(this->subjectName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToActivityTag(ActivityTag* s)
{
	if(this->activityTagName==s->name)
		return true;
	return false;
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintSubjectActivityTagPreferredRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

bool ConstraintSubjectActivityTagPreferredRooms::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintSubjectActivityTagPreferredRooms::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintSubjectActivityTagPreferredRooms::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityTagPreferredRoom::ConstraintActivityTagPreferredRoom()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM;
}

ConstraintActivityTagPreferredRoom::ConstraintActivityTagPreferredRoom(double wp, const QString& subjTag, const QString& rm)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM;
	this->activityTagName=subjTag;
	this->roomName=rm;
}

bool ConstraintActivityTagPreferredRoom::computeInternalStructure(QWidget* parent, Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_activities.clear();

	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].activityTagsNames.contains(this->activityTagName)){
		 	this->_activities.append(ac);
		}
		
	//this->_room = r.searchRoom(this->roomName);
	_room=r.roomsHash.value(roomName, -1);
	if(this->_room<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
			tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}
	assert(this->_room>=0);
	
	return true;
}

bool ConstraintActivityTagPreferredRoom::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintActivityTagPreferredRoom::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintActivityTagPreferredRoom>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Activity_Tag>"+protect(this->activityTagName)+"</Activity_Tag>\n";
	s+=IL3+"<Room>"+protect(this->roomName)+"</Room>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintActivityTagPreferredRoom>\n";

	return s;
}

QString ConstraintActivityTagPreferredRoom::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Activity tag preferred room"); s+=translatedCommaSpace();
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	s+=tr("AT:%1", "Activity tag").arg(this->activityTagName);s+=translatedCommaSpace();
	s+=tr("R:%1", "Room").arg(this->roomName);

	return begin+s+end;
}

QString ConstraintActivityTagPreferredRoom::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Activity tag preferred room"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Activity tag=%1").arg(this->activityTagName);s+="\n";
	s+=tr("Room=%1").arg(this->roomName);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivityTagPreferredRoom::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	for(int ac : std::as_const(this->_activities)){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE) //counted as unallocated
			continue;
		
		bool ok=true;
		if(rm!=this->_room)
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
		
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint activity tag preferred room broken for activity with id %1 (%2) (activity tag of constraint=%3)",
					"%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id))
					.arg(this->activityTagName);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
				
				*conflictsString+=s+"\n";
			}

			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintActivityTagPreferredRoom::isRelatedToActivity(Activity* a)
{
	return a->activityTagsNames.contains(this->activityTagName);
}

bool ConstraintActivityTagPreferredRoom::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivityTagPreferredRoom::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintActivityTagPreferredRoom::isRelatedToActivityTag(ActivityTag* s)
{
	if(this->activityTagName==s->name)
		return true;
	return false;
}

bool ConstraintActivityTagPreferredRoom::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityTagPreferredRoom::isRelatedToRoom(Room* r)
{
	return r->name==this->roomName;
}

bool ConstraintActivityTagPreferredRoom::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivityTagPreferredRoom::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintActivityTagPreferredRoom::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintActivityTagPreferredRooms::ConstraintActivityTagPreferredRooms()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS;
}

ConstraintActivityTagPreferredRooms::ConstraintActivityTagPreferredRooms(double wp, const QString& subjTag, const QStringList& rms)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS;
	this->activityTagName=subjTag;
	this->roomsNames=rms;
}

bool ConstraintActivityTagPreferredRooms::computeInternalStructure(QWidget* parent, Rules& r)
{
	//This procedure computes the internal list of all the activities
	//which correspond to the subject of the constraint.
	
	this->_activities.clear();

	for(int ac=0; ac<r.nInternalActivities; ac++)
		if(r.internalActivitiesList[ac].activityTagsNames.contains(this->activityTagName)){
			this->_activities.append(ac);
		}

	this->_rooms.clear();
	for(const QString& rm : std::as_const(roomsNames)){
		//int t=r.searchRoom(rm);
		int t=r.roomsHash.value(rm, -1);
		if(t<0){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
				tr("Following constraint is wrong:\n%1").arg(this->getDetailedDescription(r)));

			return false;
		}
		assert(t>=0);
		this->_rooms.append(t);
	}
	
	return true;
}

bool ConstraintActivityTagPreferredRooms::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintActivityTagPreferredRooms::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintActivityTagPreferredRooms>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Activity_Tag>"+protect(this->activityTagName)+"</Activity_Tag>\n";
	s+=IL3+"<Number_of_Preferred_Rooms>"+QString::number(this->roomsNames.count())+"</Number_of_Preferred_Rooms>\n";
	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++)
		s+=IL3+"<Preferred_Room>"+protect(*it)+"</Preferred_Room>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintActivityTagPreferredRooms>\n";

	return s;
}

QString ConstraintActivityTagPreferredRooms::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Activity tag preferred rooms"); s+=translatedCommaSpace();
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	s+=tr("AT:%1", "Activity tag").arg(this->activityTagName);
	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++){
		s+=translatedCommaSpace();
		s+=tr("R:%1", "Room").arg(*it);
	}

	return begin+s+end;
}

QString ConstraintActivityTagPreferredRooms::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Activity tag preferred rooms"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Activity tag=%1").arg(this->activityTagName);s+="\n";
	for(QStringList::const_iterator it=this->roomsNames.constBegin(); it!=this->roomsNames.constEnd(); it++){
		s+=tr("Room=%1").arg(*it);
		s+="\n";
	}

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintActivityTagPreferredRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts.
	//The fastest way seems to iterate over all activities
	//involved in this constraint (share the subject and activity tag of this constraint),
	//find the scheduled room and check to see if this
	//room is accepted or not.

	int nbroken;
	
	bool ok2=true;

	nbroken=0;
	for(int ac : std::as_const(this->_activities)){
		int rm=c.rooms[ac];
		if(rm==UNALLOCATED_SPACE)
			continue;
	
		bool ok=true;
		int i;
		for(i=0; i<this->_rooms.count(); i++)
			if(this->_rooms.at(i)==rm)
				break;
		if(i==this->_rooms.count())
			ok=false;

		if(!ok){
			if(rm!=UNALLOCATED_SPACE)
				ok2=false;
			
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint activity tag preferred rooms broken for activity with id %1 (%2) (activity tag of constraint=%3)"
					, "%1 is activity id, %2 is detailed description of activity")
					.arg(r.internalActivitiesList[ac].id)
					.arg(getActivityDetailedDescription(r, r.internalActivitiesList[ac].id))
					.arg(this->activityTagName);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* 1));
				
				dl.append(s);
				cl.append(weightPercentage/100* 1);
			
				*conflictsString+=s+"\n";
			}
			nbroken++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(ok2);

	return weightPercentage/100 * nbroken;
}

bool ConstraintActivityTagPreferredRooms::isRelatedToActivity(Activity* a)
{
	return a->activityTagsNames.contains(this->activityTagName);
}

bool ConstraintActivityTagPreferredRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivityTagPreferredRooms::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintActivityTagPreferredRooms::isRelatedToActivityTag(ActivityTag* s)
{
	if(this->activityTagName==s->name)
		return true;
	return false;
}

bool ConstraintActivityTagPreferredRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivityTagPreferredRooms::isRelatedToRoom(Room* r)
{
	return this->roomsNames.contains(r->name);
}

bool ConstraintActivityTagPreferredRooms::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivityTagPreferredRooms::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintActivityTagPreferredRooms::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxBuildingChangesPerDay::ConstraintStudentsSetMaxBuildingChangesPerDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY;
}

ConstraintStudentsSetMaxBuildingChangesPerDay::ConstraintStudentsSetMaxBuildingChangesPerDay(double wp, const QString& st, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY;
	this->studentsName=st;
	this->maxBuildingChangesPerDay=mc;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
	StudentsSet* ss=r.studentsHash.value(studentsName, nullptr);
	
	if(ss==nullptr){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max building changes per day is wrong because it refers to nonexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}
	
	populateInternalSubgroupsList(r, ss, this->iSubgroupsList);
	/*this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);*/

	return true;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetMaxBuildingChangesPerDay::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsSetMaxBuildingChangesPerDay>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Students>"+protect(this->studentsName)+"</Students>\n";
	s+=IL3+"<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsSetMaxBuildingChangesPerDay>\n";

	return s;
}

QString ConstraintStudentsSetMaxBuildingChangesPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set max building changes per day"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxBuildingChangesPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students set maximum building changes per day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetMaxBuildingChangesPerDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int sbg : std::as_const(this->iSubgroupsList)){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
				
		for(int ai : std::as_const(sts->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////
	
		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_building=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d2][h2];
					}
				}
			}
			
			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;
		
				if(conflictsString!=nullptr){
					QString s=tr("Space constraint students set max building changes per day broken for students=%1 on day %2")
						.arg(this->studentsName)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(s->name, this->studentsName);
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		maxBuildingChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxBuildingChangesPerDay::ConstraintStudentsMaxBuildingChangesPerDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY;
}

ConstraintStudentsMaxBuildingChangesPerDay::ConstraintStudentsMaxBuildingChangesPerDay(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY;
	this->maxBuildingChangesPerDay=mc;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsMaxBuildingChangesPerDay::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsMaxBuildingChangesPerDay>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsMaxBuildingChangesPerDay>\n";

	return s;
}

QString ConstraintStudentsMaxBuildingChangesPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students max building changes per day"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsMaxBuildingChangesPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students maximum building changes per day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxBuildingChangesPerDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
				
		for(int ai : std::as_const(sts->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_building=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d2][h2];
					}
				}
			}
						
			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;
		
				if(conflictsString!=nullptr){
					QString s=tr("Space constraint students max building changes per day broken for students=%1 on day %2")
						.arg(r.internalSubgroupsList[sbg]->name)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		maxBuildingChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxBuildingChangesPerWeek::ConstraintStudentsSetMaxBuildingChangesPerWeek()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK;
}

ConstraintStudentsSetMaxBuildingChangesPerWeek::ConstraintStudentsSetMaxBuildingChangesPerWeek(double wp, const QString& st, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK;
	this->studentsName=st;
	this->maxBuildingChangesPerWeek=mc;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
	StudentsSet* ss=r.studentsHash.value(studentsName, nullptr);
	
	if(ss==nullptr){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max building changes per week is wrong because it refers to nonexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}
	
	populateInternalSubgroupsList(r, ss, this->iSubgroupsList);
	/*this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);*/

	return true;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetMaxBuildingChangesPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsSetMaxBuildingChangesPerWeek>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Students>"+protect(this->studentsName)+"</Students>\n";
	s+=IL3+"<Max_Building_Changes_Per_Week>"+CustomFETString::number(this->maxBuildingChangesPerWeek)+"</Max_Building_Changes_Per_Week>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsSetMaxBuildingChangesPerWeek>\n";

	return s;
}

QString ConstraintStudentsSetMaxBuildingChangesPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set max building changes per week"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerWeek);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxBuildingChangesPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students set maximum building changes per week"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=tr("Maximum building changes per week=%1").arg(this->maxBuildingChangesPerWeek);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetMaxBuildingChangesPerWeek::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	for(int sbg : std::as_const(this->iSubgroupsList)){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
		
		for(int ai : std::as_const(sts->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		int n_changes=0;
		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_building=-1;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d2][h2];
					}
				}
			}
		}
		
		if(n_changes>this->maxBuildingChangesPerWeek){
			nbroken+=-this->maxBuildingChangesPerWeek+n_changes;
		
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint students set max building changes per week broken for students=%1")
					.arg(this->studentsName);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerWeek+n_changes)));
				
				dl.append(s);
				cl.append(weightPercentage/100* (-maxBuildingChangesPerWeek+n_changes));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(s->name, this->studentsName);
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsSetMaxBuildingChangesPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBuildingChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		maxBuildingChangesPerWeek=r.nDaysPerWeek*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxBuildingChangesPerWeek::ConstraintStudentsMaxBuildingChangesPerWeek()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK;
}

ConstraintStudentsMaxBuildingChangesPerWeek::ConstraintStudentsMaxBuildingChangesPerWeek(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK;
	this->maxBuildingChangesPerWeek=mc;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	Q_UNUSED(r);

	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsMaxBuildingChangesPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsMaxBuildingChangesPerWeek>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Max_Building_Changes_Per_Week>"+CustomFETString::number(this->maxBuildingChangesPerWeek)+"</Max_Building_Changes_Per_Week>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsMaxBuildingChangesPerWeek>\n";

	return s;
}

QString ConstraintStudentsMaxBuildingChangesPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students max building changes per week"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerWeek);

	return begin+s+end;
}

QString ConstraintStudentsMaxBuildingChangesPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students maximum building changes per week"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Maximum building changes per week=%1").arg(this->maxBuildingChangesPerWeek);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxBuildingChangesPerWeek::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
		
		for(int ai : std::as_const(sts->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		int n_changes=0;
		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_building=-1;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d2][h2];
					}
				}
			}
		}
		
		if(n_changes>this->maxBuildingChangesPerWeek){
			nbroken+=-this->maxBuildingChangesPerWeek+n_changes;
		
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint students max building changes per week broken for students=%1")
					.arg(r.internalSubgroupsList[sbg]->name);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerWeek+n_changes)));
				
				dl.append(s);
				cl.append(weightPercentage/100* (-maxBuildingChangesPerWeek+n_changes));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBuildingChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		maxBuildingChangesPerWeek=r.nDaysPerWeek*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMinGapsBetweenBuildingChanges::ConstraintStudentsSetMinGapsBetweenBuildingChanges()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
}

ConstraintStudentsSetMinGapsBetweenBuildingChanges::ConstraintStudentsSetMinGapsBetweenBuildingChanges(double wp, const QString& st, int mg)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
	this->studentsName=st;
	this->minGapsBetweenBuildingChanges=mg;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::computeInternalStructure(QWidget* parent, Rules& r)
{
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
	StudentsSet* ss=r.studentsHash.value(studentsName, nullptr);
	
	if(ss==nullptr){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set min gaps between building changes is wrong because it refers to nonexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}
	
	populateInternalSubgroupsList(r, ss, this->iSubgroupsList);
	/*this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);*/

	return true;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetMinGapsBetweenBuildingChanges::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsSetMinGapsBetweenBuildingChanges>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Students>"+protect(this->studentsName)+"</Students>\n";
	s+=IL3+"<Min_Gaps_Between_Building_Changes>"+CustomFETString::number(this->minGapsBetweenBuildingChanges)+"</Min_Gaps_Between_Building_Changes>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsSetMinGapsBetweenBuildingChanges>\n";

	return s;
}

QString ConstraintStudentsSetMinGapsBetweenBuildingChanges::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set min gaps between building changes"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=translatedCommaSpace();

	s+=tr("mG:%1", "mG means min gaps").arg(this->minGapsBetweenBuildingChanges);

	return begin+s+end;
}

QString ConstraintStudentsSetMinGapsBetweenBuildingChanges::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students set minimum gaps between building changes"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=tr("Minimum gaps between building changes=%1").arg(this->minGapsBetweenBuildingChanges);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetMinGapsBetweenBuildingChanges::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	for(int sbg : std::as_const(this->iSubgroupsList)){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
				
		for(int ai : std::as_const(sts->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int h2;
			for(h2=0; h2<r.nHoursPerDay; h2++)
				if(crtBuildingsTimetable[d2][h2]!=-1)
					break;

			int crt_building=-1;					
			if(h2<r.nHoursPerDay)
				crt_building=crtBuildingsTimetable[d2][h2];
			
			int cnt_gaps=0;
			
			for(h2++; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crtBuildingsTimetable[d2][h2]==crt_building)
						cnt_gaps=0;
					else{
						if(cnt_gaps<this->minGapsBetweenBuildingChanges){
							nbroken++;
						
							if(conflictsString!=nullptr){
								QString s=tr("Space constraint students set min gaps between building changes broken for students=%1 on day %2")
									.arg(this->studentsName)
									.arg(r.daysOfTheWeek[d2]);
								s += ". ";
								s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100*1));
					
								dl.append(s);
								cl.append(weightPercentage/100*1);
						
								*conflictsString+=s+"\n";
							}
						}
						
						crt_building=crtBuildingsTimetable[d2][h2];
						cnt_gaps=0;
					}
				}
				else
					cnt_gaps++;
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(s->name, this->studentsName);
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::hasWrongDayOrHour(Rules& r)
{
	if(minGapsBetweenBuildingChanges>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChanges::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minGapsBetweenBuildingChanges>r.nHoursPerDay)
		minGapsBetweenBuildingChanges=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMinGapsBetweenBuildingChanges::ConstraintStudentsMinGapsBetweenBuildingChanges()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
}

ConstraintStudentsMinGapsBetweenBuildingChanges::ConstraintStudentsMinGapsBetweenBuildingChanges(double wp, int mg)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
	this->minGapsBetweenBuildingChanges=mg;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
		
	return true;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsMinGapsBetweenBuildingChanges::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsMinGapsBetweenBuildingChanges>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Min_Gaps_Between_Building_Changes>"+CustomFETString::number(this->minGapsBetweenBuildingChanges)+"</Min_Gaps_Between_Building_Changes>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsMinGapsBetweenBuildingChanges>\n";

	return s;
}

QString ConstraintStudentsMinGapsBetweenBuildingChanges::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students min gaps between building changes"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("mG:%1", "mG means min gaps").arg(this->minGapsBetweenBuildingChanges);

	return begin+s+end;
}

QString ConstraintStudentsMinGapsBetweenBuildingChanges::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students minimum gaps between building changes"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Minimum gaps between building changes=%1").arg(this->minGapsBetweenBuildingChanges);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMinGapsBetweenBuildingChanges::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
				
		for(int ai : std::as_const(sts->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int h2;
			for(h2=0; h2<r.nHoursPerDay; h2++)
				if(crtBuildingsTimetable[d2][h2]!=-1)
					break;

			int crt_building=-1;					
			if(h2<r.nHoursPerDay)
				crt_building=crtBuildingsTimetable[d2][h2];
			
			int cnt_gaps=0;
			
			for(h2++; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crtBuildingsTimetable[d2][h2]==crt_building)
						cnt_gaps=0;
					else{
						if(cnt_gaps<this->minGapsBetweenBuildingChanges){
							nbroken++;
						
							if(conflictsString!=nullptr){
								QString s=tr("Space constraint students min gaps between building changes broken for students=%1 on day %2")
									.arg(r.internalSubgroupsList[sbg]->name)
									.arg(r.daysOfTheWeek[d2]);
								s += ". ";
								s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100*1));
					
								dl.append(s);
								cl.append(weightPercentage/100*1);
						
								*conflictsString+=s+"\n";
							}
						}
						
						crt_building=crtBuildingsTimetable[d2][h2];
						cnt_gaps=0;
					}
				}
				else
					cnt_gaps++;
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return true;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::hasWrongDayOrHour(Rules& r)
{
	if(minGapsBetweenBuildingChanges>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsMinGapsBetweenBuildingChanges::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minGapsBetweenBuildingChanges>r.nHoursPerDay)
		minGapsBetweenBuildingChanges=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxBuildingChangesPerDay::ConstraintTeacherMaxBuildingChangesPerDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY;
}

ConstraintTeacherMaxBuildingChangesPerDay::ConstraintTeacherMaxBuildingChangesPerDay(double wp, const QString& tc, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY;
	this->teacherName=tc;
	this->maxBuildingChangesPerDay=mc;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	
	if(this->teacher_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher max building changes per day is wrong because it refers to nonexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}

	return true;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherMaxBuildingChangesPerDay::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeacherMaxBuildingChangesPerDay>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+=IL3+"<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeacherMaxBuildingChangesPerDay>\n";

	return s;
}

QString ConstraintTeacherMaxBuildingChangesPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher max building changes per day"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeacherMaxBuildingChangesPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teacher maximum building changes per day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxBuildingChangesPerDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	int tch=this->teacher_ID;

	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	//Better, less memory
	Teacher* tchpointer=r.internalTeachersList[tch];
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtBuildingsTimetable[d2][h2]=-1;
	
	for(int ai : std::as_const(tchpointer->activitiesForTeacher))
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d2=c.times[ai]%r.nDaysPerWeek;
			int h2=c.times[ai]/r.nDaysPerWeek;
			
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h2+dur<r.nHoursPerDay);
				assert(crtBuildingsTimetable[d2][h2+dur]==-1);
				if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
					assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
					crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
				}
			}
		}
	/////////////
	
	for(int d2=0; d2<r.nDaysPerWeek; d2++){
		int crt_building=-1;
		int n_changes=0;
		for(int h2=0; h2<r.nHoursPerDay; h2++){
			if(crtBuildingsTimetable[d2][h2]!=-1){
				if(crt_building!=crtBuildingsTimetable[d2][h2]){
					if(crt_building!=-1)
						n_changes++;
					crt_building=crtBuildingsTimetable[d2][h2];
				}
			}
		}
		
		if(n_changes>this->maxBuildingChangesPerDay){
			nbroken+=-this->maxBuildingChangesPerDay+n_changes;
	
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint teacher max building changes per day broken for teacher=%1 on day %2")
					.arg(this->teacherName)
					.arg(r.daysOfTheWeek[d2]);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));
				
				dl.append(s);
				cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeacherMaxBuildingChangesPerDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		maxBuildingChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxBuildingChangesPerDay::ConstraintTeachersMaxBuildingChangesPerDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY;
}

ConstraintTeachersMaxBuildingChangesPerDay::ConstraintTeachersMaxBuildingChangesPerDay(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY;
	this->maxBuildingChangesPerDay=mc;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeachersMaxBuildingChangesPerDay::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeachersMaxBuildingChangesPerDay>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeachersMaxBuildingChangesPerDay>\n";

	return s;
}

QString ConstraintTeachersMaxBuildingChangesPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teachers max building changes per day"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeachersMaxBuildingChangesPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teachers maximum building changes per day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxBuildingChangesPerDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	for(int tch=0; tch<r.nInternalTeachers; tch++){
		//Better, less memory
		Teacher* tchpointer=r.internalTeachersList[tch];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
		
		for(int ai : std::as_const(tchpointer->activitiesForTeacher))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_building=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d2][h2];
					}
				}
			}
			
			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;
		
				if(conflictsString!=nullptr){
					QString s=tr("Space constraint teachers max building changes per day broken for teacher=%1 on day %2")
						.arg(r.internalTeachersList[tch]->name)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		maxBuildingChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxBuildingChangesPerWeek::ConstraintTeacherMaxBuildingChangesPerWeek()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK;
}

ConstraintTeacherMaxBuildingChangesPerWeek::ConstraintTeacherMaxBuildingChangesPerWeek(double wp, const QString& tc, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK;
	this->teacherName=tc;
	this->maxBuildingChangesPerWeek=mc;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	
	if(this->teacher_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher max building changes per week is wrong because it refers to nonexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}

	return true;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherMaxBuildingChangesPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeacherMaxBuildingChangesPerWeek>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+=IL3+"<Max_Building_Changes_Per_Week>"+CustomFETString::number(this->maxBuildingChangesPerWeek)+"</Max_Building_Changes_Per_Week>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeacherMaxBuildingChangesPerWeek>\n";

	return s;
}

QString ConstraintTeacherMaxBuildingChangesPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher max building changes per week"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerWeek);

	return begin+s+end;
}

QString ConstraintTeacherMaxBuildingChangesPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teacher maximum building changes per week"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=tr("Maximum building changes per week=%1").arg(this->maxBuildingChangesPerWeek);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxBuildingChangesPerWeek::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	int tch=this->teacher_ID;

	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	//Better, less memory
	Teacher* tchpointer=r.internalTeachersList[tch];
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtBuildingsTimetable[d2][h2]=-1;
			
	for(int ai : std::as_const(tchpointer->activitiesForTeacher))
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d2=c.times[ai]%r.nDaysPerWeek;
			int h2=c.times[ai]/r.nDaysPerWeek;
			
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h2+dur<r.nHoursPerDay);
				assert(crtBuildingsTimetable[d2][h2+dur]==-1);
				if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
					assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
					crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
				}
			}
		}
	/////////////
	
	int n_changes=0;

	for(int d2=0; d2<r.nDaysPerWeek; d2++){
		int crt_building=-1;
		for(int h2=0; h2<r.nHoursPerDay; h2++){
			if(crtBuildingsTimetable[d2][h2]!=-1){
				if(crt_building!=crtBuildingsTimetable[d2][h2]){
					if(crt_building!=-1)
						n_changes++;
					crt_building=crtBuildingsTimetable[d2][h2];
				}
			}
		}
	}
	
	if(n_changes>this->maxBuildingChangesPerWeek){
		nbroken+=n_changes-this->maxBuildingChangesPerWeek;
	
		if(conflictsString!=nullptr){
			QString s=tr("Space constraint teacher max building changes per week broken for teacher=%1")
				.arg(this->teacherName);
			s += ". ";
			s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (n_changes-maxBuildingChangesPerWeek)));
			
			dl.append(s);
			cl.append(weightPercentage/100* (n_changes-maxBuildingChangesPerWeek));
		
			*conflictsString+=s+"\n";
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeacherMaxBuildingChangesPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBuildingChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		maxBuildingChangesPerWeek=r.nDaysPerWeek*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxBuildingChangesPerWeek::ConstraintTeachersMaxBuildingChangesPerWeek()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK;
}

ConstraintTeachersMaxBuildingChangesPerWeek::ConstraintTeachersMaxBuildingChangesPerWeek(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK;
	this->maxBuildingChangesPerWeek=mc;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeachersMaxBuildingChangesPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeachersMaxBuildingChangesPerWeek>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Max_Building_Changes_Per_Week>"+CustomFETString::number(this->maxBuildingChangesPerWeek)+"</Max_Building_Changes_Per_Week>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeachersMaxBuildingChangesPerWeek>\n";

	return s;
}

QString ConstraintTeachersMaxBuildingChangesPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teachers max building changes per week"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerWeek);

	return begin+s+end;
}

QString ConstraintTeachersMaxBuildingChangesPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teachers maximum building changes per week"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Maximum building changes per week=%1").arg(this->maxBuildingChangesPerWeek);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxBuildingChangesPerWeek::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int tch=0; tch<r.nInternalTeachers; tch++){
		//Better, less memory
		Teacher* tchpointer=r.internalTeachersList[tch];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
		
		for(int ai : std::as_const(tchpointer->activitiesForTeacher))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		int n_changes=0;

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_building=-1;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d2][h2];
					}
				}
			}
		}
		
		if(n_changes>this->maxBuildingChangesPerWeek){
			nbroken+=n_changes-this->maxBuildingChangesPerWeek;
		
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint teachers max building changes per week broken for teacher=%1")
					.arg(r.internalTeachersList[tch]->name);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (n_changes-maxBuildingChangesPerWeek)));
				
				dl.append(s);
				cl.append(weightPercentage/100* (n_changes-maxBuildingChangesPerWeek));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxBuildingChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		maxBuildingChangesPerWeek=r.nDaysPerWeek*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMinGapsBetweenBuildingChanges::ConstraintTeacherMinGapsBetweenBuildingChanges()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
}

ConstraintTeacherMinGapsBetweenBuildingChanges::ConstraintTeacherMinGapsBetweenBuildingChanges(double wp, const QString& tc, int mg)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
	this->teacherName=tc;
	this->minGapsBetweenBuildingChanges=mg;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	
	if(this->teacher_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher min gaps between building changes is wrong because it refers to nonexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}

	return true;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherMinGapsBetweenBuildingChanges::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeacherMinGapsBetweenBuildingChanges>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+=IL3+"<Min_Gaps_Between_Building_Changes>"+CustomFETString::number(this->minGapsBetweenBuildingChanges)+"</Min_Gaps_Between_Building_Changes>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeacherMinGapsBetweenBuildingChanges>\n";

	return s;
}

QString ConstraintTeacherMinGapsBetweenBuildingChanges::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher min gaps between building changes"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=translatedCommaSpace();

	s+=tr("mG:%1", "mG means min gaps").arg(this->minGapsBetweenBuildingChanges);

	return begin+s+end;
}

QString ConstraintTeacherMinGapsBetweenBuildingChanges::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teacher minimum gaps between building changes"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=tr("Minimum gaps between building changes=%1").arg(this->minGapsBetweenBuildingChanges);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMinGapsBetweenBuildingChanges::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	int tch=this->teacher_ID;

	//Better, less memory
	Teacher* tchpointer=r.internalTeachersList[tch];

	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtBuildingsTimetable[d2][h2]=-1;
			
	for(int ai : std::as_const(tchpointer->activitiesForTeacher))
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d2=c.times[ai]%r.nDaysPerWeek;
			int h2=c.times[ai]/r.nDaysPerWeek;
			
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h2+dur<r.nHoursPerDay);
				assert(crtBuildingsTimetable[d2][h2+dur]==-1);
				if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
					assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
					crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
				}
			}
		}
	/////////////
	
	for(int d2=0; d2<r.nDaysPerWeek; d2++){
		int h2;
		for(h2=0; h2<r.nHoursPerDay; h2++)
			if(crtBuildingsTimetable[d2][h2]!=-1)
				break;

		int crt_building=-1;					
		if(h2<r.nHoursPerDay)
			crt_building=crtBuildingsTimetable[d2][h2];
		
		int cnt_gaps=0;
		
		for(h2++; h2<r.nHoursPerDay; h2++){
			if(crtBuildingsTimetable[d2][h2]!=-1){
				if(crtBuildingsTimetable[d2][h2]==crt_building)
					cnt_gaps=0;
				else{
					if(cnt_gaps<this->minGapsBetweenBuildingChanges){
						nbroken++;
					
						if(conflictsString!=nullptr){
							QString s=tr("Space constraint teacher min gaps between building changes broken for teacher=%1 on day %2")
								.arg(this->teacherName)
								.arg(r.daysOfTheWeek[d2]);
							s += ". ";
							s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100*1));
				
							dl.append(s);
							cl.append(weightPercentage/100*1);
					
							*conflictsString+=s+"\n";
						}
					}
					
					crt_building=crtBuildingsTimetable[d2][h2];
					cnt_gaps=0;
				}
			}
			else
				cnt_gaps++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::hasWrongDayOrHour(Rules& r)
{
	if(minGapsBetweenBuildingChanges>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeacherMinGapsBetweenBuildingChanges::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minGapsBetweenBuildingChanges>r.nHoursPerDay)
		minGapsBetweenBuildingChanges=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMinGapsBetweenBuildingChanges::ConstraintTeachersMinGapsBetweenBuildingChanges()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
}

ConstraintTeachersMinGapsBetweenBuildingChanges::ConstraintTeachersMinGapsBetweenBuildingChanges(double wp, int mg)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES;
	this->minGapsBetweenBuildingChanges=mg;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeachersMinGapsBetweenBuildingChanges::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeachersMinGapsBetweenBuildingChanges>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Min_Gaps_Between_Building_Changes>"+CustomFETString::number(this->minGapsBetweenBuildingChanges)+"</Min_Gaps_Between_Building_Changes>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeachersMinGapsBetweenBuildingChanges>\n";

	return s;
}

QString ConstraintTeachersMinGapsBetweenBuildingChanges::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teachers min gaps between building changes"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("mG:%1", "mG means min gaps").arg(this->minGapsBetweenBuildingChanges);

	return begin+s+end;
}

QString ConstraintTeachersMinGapsBetweenBuildingChanges::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teachers minimum gaps between building changes"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Minimum gaps between building changes=%1").arg(this->minGapsBetweenBuildingChanges);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMinGapsBetweenBuildingChanges::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int tch=0; tch<r.nInternalTeachers; tch++){
		//Better, less memory
		Teacher* tchpointer=r.internalTeachersList[tch];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
				
		for(int ai : std::as_const(tchpointer->activitiesForTeacher))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int h2;
			for(h2=0; h2<r.nHoursPerDay; h2++)
				if(crtBuildingsTimetable[d2][h2]!=-1)
					break;

			int crt_building=-1;					
			if(h2<r.nHoursPerDay)
				crt_building=crtBuildingsTimetable[d2][h2];
				
			int cnt_gaps=0;
			
			for(h2++; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crtBuildingsTimetable[d2][h2]==crt_building)
						cnt_gaps=0;
					else{
						if(cnt_gaps<this->minGapsBetweenBuildingChanges){
							nbroken++;
					
							if(conflictsString!=nullptr){
								QString s=tr("Space constraint teachers min gaps between building changes broken for teacher=%1 on day %2")
									.arg(r.internalTeachersList[tch]->name)
									.arg(r.daysOfTheWeek[d2]);
								s += ". ";
								s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100*1));
					
								dl.append(s);
								cl.append(weightPercentage/100*1);
						
								*conflictsString+=s+"\n";
							}
						}
					
						crt_building=crtBuildingsTimetable[d2][h2];
						cnt_gaps=0;
					}
				}
				else
					cnt_gaps++;
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return true;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::hasWrongDayOrHour(Rules& r)
{
	if(minGapsBetweenBuildingChanges>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeachersMinGapsBetweenBuildingChanges::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minGapsBetweenBuildingChanges>r.nHoursPerDay)
		minGapsBetweenBuildingChanges=r.nHoursPerDay;

	return true;
}

//2019-11-14
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxRoomChangesPerDay::ConstraintStudentsSetMaxRoomChangesPerDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY;
}

ConstraintStudentsSetMaxRoomChangesPerDay::ConstraintStudentsSetMaxRoomChangesPerDay(double wp, const QString& st, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY;
	this->studentsName=st;
	this->maxRoomChangesPerDay=mc;
}

bool ConstraintStudentsSetMaxRoomChangesPerDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
	StudentsSet* ss=r.studentsHash.value(studentsName, nullptr);
	
	if(ss==nullptr){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max room changes per day is wrong because it refers to nonexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}
	
	populateInternalSubgroupsList(r, ss, this->iSubgroupsList);
	/*this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);*/

	return true;
}

bool ConstraintStudentsSetMaxRoomChangesPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetMaxRoomChangesPerDay::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsSetMaxRoomChangesPerDay>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Students>"+protect(this->studentsName)+"</Students>\n";
	s+=IL3+"<Max_Room_Changes_Per_Day>"+CustomFETString::number(this->maxRoomChangesPerDay)+"</Max_Room_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsSetMaxRoomChangesPerDay>\n";

	return s;
}

QString ConstraintStudentsSetMaxRoomChangesPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set max room changes per day"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxRoomChangesPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students set maximum room changes per day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=tr("Maximum room changes per day=%1").arg(this->maxRoomChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetMaxRoomChangesPerDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int sbg : std::as_const(this->iSubgroupsList)){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;
				
		for(int ai : std::as_const(sts->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////
	
		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_room=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtRoomsTimetable[d2][h2]!=-1){
					if(crt_room!=crtRoomsTimetable[d2][h2]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[d2][h2];
					}
				}
			}
			
			if(n_changes>this->maxRoomChangesPerDay){
				nbroken+=-this->maxRoomChangesPerDay+n_changes;
		
				if(conflictsString!=nullptr){
					QString s=tr("Space constraint students set max room changes per day broken for students=%1 on day %2")
						.arg(this->studentsName)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerDay+n_changes)));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxRoomChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxRoomChangesPerDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(s->name, this->studentsName);
}

bool ConstraintStudentsSetMaxRoomChangesPerDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerDay::hasWrongDayOrHour(Rules& r)
{
	if(maxRoomChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsSetMaxRoomChangesPerDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxRoomChangesPerDay>r.nHoursPerDay)
		maxRoomChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxRoomChangesPerDay::ConstraintStudentsMaxRoomChangesPerDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY;
}

ConstraintStudentsMaxRoomChangesPerDay::ConstraintStudentsMaxRoomChangesPerDay(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY;
	this->maxRoomChangesPerDay=mc;
}

bool ConstraintStudentsMaxRoomChangesPerDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintStudentsMaxRoomChangesPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsMaxRoomChangesPerDay::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsMaxRoomChangesPerDay>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Max_Room_Changes_Per_Day>"+CustomFETString::number(this->maxRoomChangesPerDay)+"</Max_Room_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsMaxRoomChangesPerDay>\n";

	return s;
}

QString ConstraintStudentsMaxRoomChangesPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students max room changes per day"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsMaxRoomChangesPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students maximum room changes per day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Maximum room changes per day=%1").arg(this->maxRoomChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxRoomChangesPerDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;
				
		for(int ai : std::as_const(sts->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_room=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtRoomsTimetable[d2][h2]!=-1){
					if(crt_room!=crtRoomsTimetable[d2][h2]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[d2][h2];
					}
				}
			}
			
			if(n_changes>this->maxRoomChangesPerDay){
				nbroken+=-this->maxRoomChangesPerDay+n_changes;
		
				if(conflictsString!=nullptr){
					QString s=tr("Space constraint students max room changes per day broken for students=%1 on day %2")
						.arg(r.internalSubgroupsList[sbg]->name)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerDay+n_changes)));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxRoomChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxRoomChangesPerDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxRoomChangesPerDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsMaxRoomChangesPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsMaxRoomChangesPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxRoomChangesPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return true;
}

bool ConstraintStudentsMaxRoomChangesPerDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsMaxRoomChangesPerDay::hasWrongDayOrHour(Rules& r)
{
	if(maxRoomChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsMaxRoomChangesPerDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsMaxRoomChangesPerDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxRoomChangesPerDay>r.nHoursPerDay)
		maxRoomChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxRoomChangesPerWeek::ConstraintStudentsSetMaxRoomChangesPerWeek()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK;
}

ConstraintStudentsSetMaxRoomChangesPerWeek::ConstraintStudentsSetMaxRoomChangesPerWeek(double wp, const QString& st, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK;
	this->studentsName=st;
	this->maxRoomChangesPerWeek=mc;
}

bool ConstraintStudentsSetMaxRoomChangesPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
	StudentsSet* ss=r.studentsHash.value(studentsName, nullptr);
	
	if(ss==nullptr){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max room changes per week is wrong because it refers to nonexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}
	
	populateInternalSubgroupsList(r, ss, this->iSubgroupsList);
	/*this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);*/

	return true;
}

bool ConstraintStudentsSetMaxRoomChangesPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetMaxRoomChangesPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsSetMaxRoomChangesPerWeek>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Students>"+protect(this->studentsName)+"</Students>\n";
	s+=IL3+"<Max_Room_Changes_Per_Week>"+CustomFETString::number(this->maxRoomChangesPerWeek)+"</Max_Room_Changes_Per_Week>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsSetMaxRoomChangesPerWeek>\n";

	return s;
}

QString ConstraintStudentsSetMaxRoomChangesPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set max room changes per week"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerWeek);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxRoomChangesPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students set maximum room changes per week"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=tr("Maximum room changes per week=%1").arg(this->maxRoomChangesPerWeek);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetMaxRoomChangesPerWeek::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int sbg : std::as_const(this->iSubgroupsList)){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;
		
		for(int ai : std::as_const(sts->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////

		int n_changes=0;
		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_room=-1;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtRoomsTimetable[d2][h2]!=-1){
					if(crt_room!=crtRoomsTimetable[d2][h2]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[d2][h2];
					}
				}
			}
		}
		
		if(n_changes>this->maxRoomChangesPerWeek){
			nbroken+=-this->maxRoomChangesPerWeek+n_changes;
		
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint students set max room changes per week broken for students=%1")
					.arg(this->studentsName);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerWeek+n_changes)));
				
				dl.append(s);
				cl.append(weightPercentage/100* (-maxRoomChangesPerWeek+n_changes));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxRoomChangesPerWeek::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(s->name, this->studentsName);
}

bool ConstraintStudentsSetMaxRoomChangesPerWeek::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxRoomChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsSetMaxRoomChangesPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxRoomChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		maxRoomChangesPerWeek=r.nDaysPerWeek*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxRoomChangesPerWeek::ConstraintStudentsMaxRoomChangesPerWeek()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_WEEK;
}

ConstraintStudentsMaxRoomChangesPerWeek::ConstraintStudentsMaxRoomChangesPerWeek(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_WEEK;
	this->maxRoomChangesPerWeek=mc;
}

bool ConstraintStudentsMaxRoomChangesPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);

	Q_UNUSED(r);

	return true;
}

bool ConstraintStudentsMaxRoomChangesPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsMaxRoomChangesPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsMaxRoomChangesPerWeek>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Max_Room_Changes_Per_Week>"+CustomFETString::number(this->maxRoomChangesPerWeek)+"</Max_Room_Changes_Per_Week>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsMaxRoomChangesPerWeek>\n";

	return s;
}

QString ConstraintStudentsMaxRoomChangesPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students max room changes per week"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerWeek);

	return begin+s+end;
}

QString ConstraintStudentsMaxRoomChangesPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students maximum room changes per week"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Maximum room changes per week=%1").arg(this->maxRoomChangesPerWeek);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxRoomChangesPerWeek::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;
		
		for(int ai : std::as_const(sts->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////

		int n_changes=0;
		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_room=-1;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtRoomsTimetable[d2][h2]!=-1){
					if(crt_room!=crtRoomsTimetable[d2][h2]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[d2][h2];
					}
				}
			}
		}
		
		if(n_changes>this->maxRoomChangesPerWeek){
			nbroken+=-this->maxRoomChangesPerWeek+n_changes;
		
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint students max room changes per week broken for students=%1")
					.arg(r.internalSubgroupsList[sbg]->name);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerWeek+n_changes)));
				
				dl.append(s);
				cl.append(weightPercentage/100* (-maxRoomChangesPerWeek+n_changes));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxRoomChangesPerWeek::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxRoomChangesPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsMaxRoomChangesPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsMaxRoomChangesPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxRoomChangesPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

bool ConstraintStudentsMaxRoomChangesPerWeek::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsMaxRoomChangesPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxRoomChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsMaxRoomChangesPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsMaxRoomChangesPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxRoomChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		maxRoomChangesPerWeek=r.nDaysPerWeek*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMinGapsBetweenRoomChanges::ConstraintStudentsSetMinGapsBetweenRoomChanges()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES;
}

ConstraintStudentsSetMinGapsBetweenRoomChanges::ConstraintStudentsSetMinGapsBetweenRoomChanges(double wp, const QString& st, int mg)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES;
	this->studentsName=st;
	this->minGapsBetweenRoomChanges=mg;
}

bool ConstraintStudentsSetMinGapsBetweenRoomChanges::computeInternalStructure(QWidget* parent, Rules& r)
{
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
	StudentsSet* ss=r.studentsHash.value(studentsName, nullptr);
	
	if(ss==nullptr){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set min gaps between room changes is wrong because it refers to nonexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}
	
	populateInternalSubgroupsList(r, ss, this->iSubgroupsList);
	/*this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);*/

	return true;
}

bool ConstraintStudentsSetMinGapsBetweenRoomChanges::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetMinGapsBetweenRoomChanges::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsSetMinGapsBetweenRoomChanges>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Students>"+protect(this->studentsName)+"</Students>\n";
	s+=IL3+"<Min_Gaps_Between_Room_Changes>"+CustomFETString::number(this->minGapsBetweenRoomChanges)+"</Min_Gaps_Between_Room_Changes>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsSetMinGapsBetweenRoomChanges>\n";

	return s;
}

QString ConstraintStudentsSetMinGapsBetweenRoomChanges::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set min gaps between room changes"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=translatedCommaSpace();

	s+=tr("mG:%1", "mG means min gaps").arg(this->minGapsBetweenRoomChanges);

	return begin+s+end;
}

QString ConstraintStudentsSetMinGapsBetweenRoomChanges::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students set minimum gaps between room changes"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=tr("Minimum gaps between room changes=%1").arg(this->minGapsBetweenRoomChanges);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetMinGapsBetweenRoomChanges::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int sbg : std::as_const(this->iSubgroupsList)){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;
				
		for(int ai : std::as_const(sts->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int h2;
			for(h2=0; h2<r.nHoursPerDay; h2++)
				if(crtRoomsTimetable[d2][h2]!=-1)
					break;

			int crt_room=-1;
			if(h2<r.nHoursPerDay)
				crt_room=crtRoomsTimetable[d2][h2];
			
			int cnt_gaps=0;
			
			for(h2++; h2<r.nHoursPerDay; h2++){
				if(crtRoomsTimetable[d2][h2]!=-1){
					if(crtRoomsTimetable[d2][h2]==crt_room)
						cnt_gaps=0;
					else{
						if(cnt_gaps<this->minGapsBetweenRoomChanges){
							nbroken++;
						
							if(conflictsString!=nullptr){
								QString s=tr("Space constraint students set min gaps between room changes broken for students=%1 on day %2")
									.arg(this->studentsName)
									.arg(r.daysOfTheWeek[d2]);
								s += ". ";
								s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100*1));
					
								dl.append(s);
								cl.append(weightPercentage/100*1);
						
								*conflictsString+=s+"\n";
							}
						}
						
						crt_room=crtRoomsTimetable[d2][h2];
						cnt_gaps=0;
					}
				}
				else
					cnt_gaps++;
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMinGapsBetweenRoomChanges::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMinGapsBetweenRoomChanges::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetMinGapsBetweenRoomChanges::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetMinGapsBetweenRoomChanges::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMinGapsBetweenRoomChanges::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(s->name, this->studentsName);
}

bool ConstraintStudentsSetMinGapsBetweenRoomChanges::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsSetMinGapsBetweenRoomChanges::hasWrongDayOrHour(Rules& r)
{
	if(minGapsBetweenRoomChanges>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMinGapsBetweenRoomChanges::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsSetMinGapsBetweenRoomChanges::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minGapsBetweenRoomChanges>r.nHoursPerDay)
		minGapsBetweenRoomChanges=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMinGapsBetweenRoomChanges::ConstraintStudentsMinGapsBetweenRoomChanges()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ROOM_CHANGES;
}

ConstraintStudentsMinGapsBetweenRoomChanges::ConstraintStudentsMinGapsBetweenRoomChanges(double wp, int mg)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ROOM_CHANGES;
	this->minGapsBetweenRoomChanges=mg;
}

bool ConstraintStudentsMinGapsBetweenRoomChanges::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
		
	return true;
}

bool ConstraintStudentsMinGapsBetweenRoomChanges::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsMinGapsBetweenRoomChanges::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsMinGapsBetweenRoomChanges>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Min_Gaps_Between_Room_Changes>"+CustomFETString::number(this->minGapsBetweenRoomChanges)+"</Min_Gaps_Between_Room_Changes>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsMinGapsBetweenRoomChanges>\n";

	return s;
}

QString ConstraintStudentsMinGapsBetweenRoomChanges::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students min gaps between room changes"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("mG:%1", "mG means min gaps").arg(this->minGapsBetweenRoomChanges);

	return begin+s+end;
}

QString ConstraintStudentsMinGapsBetweenRoomChanges::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students minimum gaps between room changes"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Minimum gaps between room changes=%1").arg(this->minGapsBetweenRoomChanges);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMinGapsBetweenRoomChanges::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;
				
		for(int ai : std::as_const(sts->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int h2;
			for(h2=0; h2<r.nHoursPerDay; h2++)
				if(crtRoomsTimetable[d2][h2]!=-1)
					break;

			int crt_room=-1;
			if(h2<r.nHoursPerDay)
				crt_room=crtRoomsTimetable[d2][h2];
			
			int cnt_gaps=0;
			
			for(h2++; h2<r.nHoursPerDay; h2++){
				if(crtRoomsTimetable[d2][h2]!=-1){
					if(crtRoomsTimetable[d2][h2]==crt_room)
						cnt_gaps=0;
					else{
						if(cnt_gaps<this->minGapsBetweenRoomChanges){
							nbroken++;
						
							if(conflictsString!=nullptr){
								QString s=tr("Space constraint students min gaps between room changes broken for students=%1 on day %2")
									.arg(r.internalSubgroupsList[sbg]->name)
									.arg(r.daysOfTheWeek[d2]);
								s += ". ";
								s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100*1));
					
								dl.append(s);
								cl.append(weightPercentage/100*1);
						
								*conflictsString+=s+"\n";
							}
						}
						
						crt_room=crtRoomsTimetable[d2][h2];
						cnt_gaps=0;
					}
				}
				else
					cnt_gaps++;
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMinGapsBetweenRoomChanges::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMinGapsBetweenRoomChanges::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsMinGapsBetweenRoomChanges::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsMinGapsBetweenRoomChanges::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMinGapsBetweenRoomChanges::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return true;
}

bool ConstraintStudentsMinGapsBetweenRoomChanges::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsMinGapsBetweenRoomChanges::hasWrongDayOrHour(Rules& r)
{
	if(minGapsBetweenRoomChanges>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsMinGapsBetweenRoomChanges::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsMinGapsBetweenRoomChanges::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minGapsBetweenRoomChanges>r.nHoursPerDay)
		minGapsBetweenRoomChanges=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxRoomChangesPerDay::ConstraintTeacherMaxRoomChangesPerDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY;
}

ConstraintTeacherMaxRoomChangesPerDay::ConstraintTeacherMaxRoomChangesPerDay(double wp, const QString& tc, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY;
	this->teacherName=tc;
	this->maxRoomChangesPerDay=mc;
}

bool ConstraintTeacherMaxRoomChangesPerDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	
	if(this->teacher_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher max room changes per day is wrong because it refers to nonexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}

	return true;
}

bool ConstraintTeacherMaxRoomChangesPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherMaxRoomChangesPerDay::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeacherMaxRoomChangesPerDay>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+=IL3+"<Max_Room_Changes_Per_Day>"+CustomFETString::number(this->maxRoomChangesPerDay)+"</Max_Room_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeacherMaxRoomChangesPerDay>\n";

	return s;
}

QString ConstraintTeacherMaxRoomChangesPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher max room changes per day"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeacherMaxRoomChangesPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teacher maximum room changes per day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=tr("Maximum room changes per day=%1").arg(this->maxRoomChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxRoomChangesPerDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	int tch=this->teacher_ID;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	//Better, less memory
	Teacher* tchpointer=r.internalTeachersList[tch];
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtRoomsTimetable[d2][h2]=-1;
			
	for(int ai : std::as_const(tchpointer->activitiesForTeacher))
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d2=c.times[ai]%r.nDaysPerWeek;
			int h2=c.times[ai]/r.nDaysPerWeek;
			
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h2+dur<r.nHoursPerDay);
				assert(crtRoomsTimetable[d2][h2+dur]==-1);
				if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
					assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
					crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
				}
			}
		}
	/////////////
	
	for(int d2=0; d2<r.nDaysPerWeek; d2++){			
		int crt_room=-1;
		int n_changes=0;
		for(int h2=0; h2<r.nHoursPerDay; h2++){
			if(crtRoomsTimetable[d2][h2]!=-1){
				if(crt_room!=crtRoomsTimetable[d2][h2]){
					if(crt_room!=-1)
						n_changes++;
					crt_room=crtRoomsTimetable[d2][h2];
				}
			}
		}
					
		if(n_changes>this->maxRoomChangesPerDay){
			nbroken+=-this->maxRoomChangesPerDay+n_changes;
	
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint teacher max room changes per day broken for teacher=%1 on day %2")
					.arg(this->teacherName)
					.arg(r.daysOfTheWeek[d2]);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerDay+n_changes)));
				
				dl.append(s);
				cl.append(weightPercentage/100* (-maxRoomChangesPerDay+n_changes));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxRoomChangesPerDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxRoomChangesPerDay::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMaxRoomChangesPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxRoomChangesPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxRoomChangesPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxRoomChangesPerDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeacherMaxRoomChangesPerDay::hasWrongDayOrHour(Rules& r)
{
	if(maxRoomChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherMaxRoomChangesPerDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeacherMaxRoomChangesPerDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxRoomChangesPerDay>r.nHoursPerDay)
		maxRoomChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxRoomChangesPerDay::ConstraintTeachersMaxRoomChangesPerDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY;
}

ConstraintTeachersMaxRoomChangesPerDay::ConstraintTeachersMaxRoomChangesPerDay(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY;
	this->maxRoomChangesPerDay=mc;
}

bool ConstraintTeachersMaxRoomChangesPerDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMaxRoomChangesPerDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeachersMaxRoomChangesPerDay::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeachersMaxRoomChangesPerDay>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Max_Room_Changes_Per_Day>"+CustomFETString::number(this->maxRoomChangesPerDay)+"</Max_Room_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeachersMaxRoomChangesPerDay>\n";

	return s;
}

QString ConstraintTeachersMaxRoomChangesPerDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teachers max room changes per day"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeachersMaxRoomChangesPerDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teachers maximum room changes per day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Maximum room changes per day=%1").arg(this->maxRoomChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxRoomChangesPerDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int tch=0; tch<r.nInternalTeachers; tch++){
		//Better, less memory
		Teacher* tchpointer=r.internalTeachersList[tch];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;
				
		for(int ai : std::as_const(tchpointer->activitiesForTeacher))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_room=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtRoomsTimetable[d2][h2]!=-1){
					if(crt_room!=crtRoomsTimetable[d2][h2]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[d2][h2];
					}
				}
			}
					
			if(n_changes>this->maxRoomChangesPerDay){
				nbroken+=-this->maxRoomChangesPerDay+n_changes;
		
				if(conflictsString!=nullptr){
					QString s=tr("Space constraint teachers max room changes per day broken for teacher=%1 on day %2")
						.arg(r.internalTeachersList[tch]->name)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerDay+n_changes)));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxRoomChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxRoomChangesPerDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxRoomChangesPerDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return true;
}

bool ConstraintTeachersMaxRoomChangesPerDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxRoomChangesPerDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxRoomChangesPerDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxRoomChangesPerDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeachersMaxRoomChangesPerDay::hasWrongDayOrHour(Rules& r)
{
	if(maxRoomChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersMaxRoomChangesPerDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeachersMaxRoomChangesPerDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxRoomChangesPerDay>r.nHoursPerDay)
		maxRoomChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxRoomChangesPerWeek::ConstraintTeacherMaxRoomChangesPerWeek()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK;
}

ConstraintTeacherMaxRoomChangesPerWeek::ConstraintTeacherMaxRoomChangesPerWeek(double wp, const QString& tc, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK;
	this->teacherName=tc;
	this->maxRoomChangesPerWeek=mc;
}

bool ConstraintTeacherMaxRoomChangesPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	
	if(this->teacher_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher max room changes per week is wrong because it refers to nonexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}

	return true;
}

bool ConstraintTeacherMaxRoomChangesPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherMaxRoomChangesPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeacherMaxRoomChangesPerWeek>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+=IL3+"<Max_Room_Changes_Per_Week>"+CustomFETString::number(this->maxRoomChangesPerWeek)+"</Max_Room_Changes_Per_Week>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeacherMaxRoomChangesPerWeek>\n";

	return s;
}

QString ConstraintTeacherMaxRoomChangesPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher max room changes per week"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerWeek);

	return begin+s+end;
}

QString ConstraintTeacherMaxRoomChangesPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teacher maximum room changes per week"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=tr("Maximum room changes per week=%1").arg(this->maxRoomChangesPerWeek);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxRoomChangesPerWeek::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	int tch=this->teacher_ID;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	//Better, less memory
	Teacher* tchpointer=r.internalTeachersList[tch];
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtRoomsTimetable[d2][h2]=-1;
	
	for(int ai : std::as_const(tchpointer->activitiesForTeacher))
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d2=c.times[ai]%r.nDaysPerWeek;
			int h2=c.times[ai]/r.nDaysPerWeek;
			
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h2+dur<r.nHoursPerDay);
				assert(crtRoomsTimetable[d2][h2+dur]==-1);
				if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
					assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
					crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
				}
			}
		}
	/////////////
	
	int n_changes=0;

	for(int d2=0; d2<r.nDaysPerWeek; d2++){
		int crt_room=-1;
		for(int h2=0; h2<r.nHoursPerDay; h2++){
			if(crtRoomsTimetable[d2][h2]!=-1){
				if(crt_room!=crtRoomsTimetable[d2][h2]){
					if(crt_room!=-1)
						n_changes++;
					crt_room=crtRoomsTimetable[d2][h2];
				}
			}
		}
	}
	
	if(n_changes>this->maxRoomChangesPerWeek){
		nbroken+=n_changes-this->maxRoomChangesPerWeek;
	
		if(conflictsString!=nullptr){
			QString s=tr("Space constraint teacher max room changes per week broken for teacher=%1")
				.arg(this->teacherName);
			s += ". ";
			s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (n_changes-maxRoomChangesPerWeek)));
			
			dl.append(s);
			cl.append(weightPercentage/100* (n_changes-maxRoomChangesPerWeek));
		
			*conflictsString+=s+"\n";
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxRoomChangesPerWeek::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxRoomChangesPerWeek::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMaxRoomChangesPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxRoomChangesPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxRoomChangesPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxRoomChangesPerWeek::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeacherMaxRoomChangesPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxRoomChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherMaxRoomChangesPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeacherMaxRoomChangesPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxRoomChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		maxRoomChangesPerWeek=r.nDaysPerWeek*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxRoomChangesPerWeek::ConstraintTeachersMaxRoomChangesPerWeek()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK;
}

ConstraintTeachersMaxRoomChangesPerWeek::ConstraintTeachersMaxRoomChangesPerWeek(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK;
	this->maxRoomChangesPerWeek=mc;
}

bool ConstraintTeachersMaxRoomChangesPerWeek::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMaxRoomChangesPerWeek::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeachersMaxRoomChangesPerWeek::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeachersMaxRoomChangesPerWeek>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Max_Room_Changes_Per_Week>"+CustomFETString::number(this->maxRoomChangesPerWeek)+"</Max_Room_Changes_Per_Week>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeachersMaxRoomChangesPerWeek>\n";

	return s;
}

QString ConstraintTeachersMaxRoomChangesPerWeek::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teachers max room changes per week"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerWeek);

	return begin+s+end;
}

QString ConstraintTeachersMaxRoomChangesPerWeek::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teachers maximum room changes per week"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Maximum room changes per week=%1").arg(this->maxRoomChangesPerWeek);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxRoomChangesPerWeek::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	for(int tch=0; tch<r.nInternalTeachers; tch++){
		//Better, less memory
		Teacher* tchpointer=r.internalTeachersList[tch];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;
		
		for(int ai : std::as_const(tchpointer->activitiesForTeacher))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////

		int n_changes=0;

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_room=-1;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtRoomsTimetable[d2][h2]!=-1){
					if(crt_room!=crtRoomsTimetable[d2][h2]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[d2][h2];
					}
				}
			}
		}
		
		if(n_changes>this->maxRoomChangesPerWeek){
			nbroken+=n_changes-this->maxRoomChangesPerWeek;
		
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint teachers max room changes per week broken for teacher=%1")
					.arg(r.internalTeachersList[tch]->name);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (n_changes-maxRoomChangesPerWeek)));
				
				dl.append(s);
				cl.append(weightPercentage/100* (n_changes-maxRoomChangesPerWeek));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxRoomChangesPerWeek::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxRoomChangesPerWeek::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return true;
}

bool ConstraintTeachersMaxRoomChangesPerWeek::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxRoomChangesPerWeek::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxRoomChangesPerWeek::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxRoomChangesPerWeek::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeachersMaxRoomChangesPerWeek::hasWrongDayOrHour(Rules& r)
{
	if(maxRoomChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersMaxRoomChangesPerWeek::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeachersMaxRoomChangesPerWeek::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(maxRoomChangesPerWeek>r.nDaysPerWeek*r.nHoursPerDay)
		maxRoomChangesPerWeek=r.nDaysPerWeek*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMinGapsBetweenRoomChanges::ConstraintTeacherMinGapsBetweenRoomChanges()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES;
}

ConstraintTeacherMinGapsBetweenRoomChanges::ConstraintTeacherMinGapsBetweenRoomChanges(double wp, const QString& tc, int mg)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES;
	this->teacherName=tc;
	this->minGapsBetweenRoomChanges=mg;
}

bool ConstraintTeacherMinGapsBetweenRoomChanges::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	
	if(this->teacher_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher min gaps between room changes is wrong because it refers to nonexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}

	return true;
}

bool ConstraintTeacherMinGapsBetweenRoomChanges::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherMinGapsBetweenRoomChanges::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeacherMinGapsBetweenRoomChanges>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+=IL3+"<Min_Gaps_Between_Room_Changes>"+CustomFETString::number(this->minGapsBetweenRoomChanges)+"</Min_Gaps_Between_Room_Changes>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeacherMinGapsBetweenRoomChanges>\n";

	return s;
}

QString ConstraintTeacherMinGapsBetweenRoomChanges::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher min gaps between room changes"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=translatedCommaSpace();

	s+=tr("mG:%1", "mG means min gaps").arg(this->minGapsBetweenRoomChanges);

	return begin+s+end;
}

QString ConstraintTeacherMinGapsBetweenRoomChanges::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teacher minimum gaps between room changes"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=tr("Minimum gaps between room changes=%1").arg(this->minGapsBetweenRoomChanges);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMinGapsBetweenRoomChanges::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	int tch=this->teacher_ID;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	//Better, less memory
	Teacher* tchpointer=r.internalTeachersList[tch];
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtRoomsTimetable[d2][h2]=-1;
			
	for(int ai : std::as_const(tchpointer->activitiesForTeacher))
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d2=c.times[ai]%r.nDaysPerWeek;
			int h2=c.times[ai]/r.nDaysPerWeek;
			
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h2+dur<r.nHoursPerDay);
				assert(crtRoomsTimetable[d2][h2+dur]==-1);
				if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
					assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
					crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
				}
			}
		}
	/////////////
	
	for(int d2=0; d2<r.nDaysPerWeek; d2++){
		int h2;
		for(h2=0; h2<r.nHoursPerDay; h2++)
			if(crtRoomsTimetable[d2][h2]!=-1)
				break;

		int crt_room=-1;					
		if(h2<r.nHoursPerDay)
			crt_room=crtRoomsTimetable[d2][h2];
		
		int cnt_gaps=0;
		
		for(h2++; h2<r.nHoursPerDay; h2++){
			if(crtRoomsTimetable[d2][h2]!=-1){
				if(crtRoomsTimetable[d2][h2]==crt_room)
					cnt_gaps=0;
				else{
					if(cnt_gaps<this->minGapsBetweenRoomChanges){
						nbroken++;
					
						if(conflictsString!=nullptr){
							QString s=tr("Space constraint teacher min gaps between room changes broken for teacher=%1 on day %2")
								.arg(this->teacherName)
								.arg(r.daysOfTheWeek[d2]);
							s += ". ";
							s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100*1));
				
							dl.append(s);
							cl.append(weightPercentage/100*1);
					
							*conflictsString+=s+"\n";
						}
					}
					
					crt_room=crtRoomsTimetable[d2][h2];
					cnt_gaps=0;
				}
			}
			else
				cnt_gaps++;
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMinGapsBetweenRoomChanges::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMinGapsBetweenRoomChanges::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMinGapsBetweenRoomChanges::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMinGapsBetweenRoomChanges::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMinGapsBetweenRoomChanges::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMinGapsBetweenRoomChanges::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeacherMinGapsBetweenRoomChanges::hasWrongDayOrHour(Rules& r)
{
	if(minGapsBetweenRoomChanges>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherMinGapsBetweenRoomChanges::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeacherMinGapsBetweenRoomChanges::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minGapsBetweenRoomChanges>r.nHoursPerDay)
		minGapsBetweenRoomChanges=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMinGapsBetweenRoomChanges::ConstraintTeachersMinGapsBetweenRoomChanges()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES;
}

ConstraintTeachersMinGapsBetweenRoomChanges::ConstraintTeachersMinGapsBetweenRoomChanges(double wp, int mg)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES;
	this->minGapsBetweenRoomChanges=mg;
}

bool ConstraintTeachersMinGapsBetweenRoomChanges::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMinGapsBetweenRoomChanges::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeachersMinGapsBetweenRoomChanges::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeachersMinGapsBetweenRoomChanges>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Min_Gaps_Between_Room_Changes>"+CustomFETString::number(this->minGapsBetweenRoomChanges)+"</Min_Gaps_Between_Room_Changes>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeachersMinGapsBetweenRoomChanges>\n";

	return s;
}

QString ConstraintTeachersMinGapsBetweenRoomChanges::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teachers min gaps between room changes"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("mG:%1", "mG means min gaps").arg(this->minGapsBetweenRoomChanges);

	return begin+s+end;
}

QString ConstraintTeachersMinGapsBetweenRoomChanges::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teachers minimum gaps between room changes"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Minimum gaps between room changes=%1").arg(this->minGapsBetweenRoomChanges);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMinGapsBetweenRoomChanges::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int tch=0; tch<r.nInternalTeachers; tch++){
		//Better, less memory
		Teacher* tchpointer=r.internalTeachersList[tch];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;
				
		for(int ai : std::as_const(tchpointer->activitiesForTeacher))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int h2;
			for(h2=0; h2<r.nHoursPerDay; h2++)
				if(crtRoomsTimetable[d2][h2]!=-1)
					break;

			int crt_room=-1;					
			if(h2<r.nHoursPerDay)
				crt_room=crtRoomsTimetable[d2][h2];
				
			int cnt_gaps=0;
			
			for(h2++; h2<r.nHoursPerDay; h2++){
				if(crtRoomsTimetable[d2][h2]!=-1){
					if(crtRoomsTimetable[d2][h2]==crt_room)
						cnt_gaps=0;
					else{
						if(cnt_gaps<this->minGapsBetweenRoomChanges){
							nbroken++;
					
							if(conflictsString!=nullptr){
								QString s=tr("Space constraint teachers min gaps between room changes broken for teacher=%1 on day %2")
									.arg(r.internalTeachersList[tch]->name)
									.arg(r.daysOfTheWeek[d2]);
								s += ". ";
								s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100*1));
					
								dl.append(s);
								cl.append(weightPercentage/100*1);
						
								*conflictsString+=s+"\n";
							}
						}
					
						crt_room=crtRoomsTimetable[d2][h2];
						cnt_gaps=0;
					}
				}
				else
					cnt_gaps++;
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMinGapsBetweenRoomChanges::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMinGapsBetweenRoomChanges::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return true;
}

bool ConstraintTeachersMinGapsBetweenRoomChanges::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMinGapsBetweenRoomChanges::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMinGapsBetweenRoomChanges::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMinGapsBetweenRoomChanges::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeachersMinGapsBetweenRoomChanges::hasWrongDayOrHour(Rules& r)
{
	if(minGapsBetweenRoomChanges>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersMinGapsBetweenRoomChanges::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeachersMinGapsBetweenRoomChanges::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(minGapsBetweenRoomChanges>r.nHoursPerDay)
		minGapsBetweenRoomChanges=r.nHoursPerDay;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesOccupyMaxDifferentRooms::ConstraintActivitiesOccupyMaxDifferentRooms()
	: SpaceConstraint()
{
	this->type = CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS;
}

ConstraintActivitiesOccupyMaxDifferentRooms::ConstraintActivitiesOccupyMaxDifferentRooms(double wp,
	const QList<int>& a_L, int max_different_rooms)
	: SpaceConstraint(wp)
{
	this->activitiesIds=a_L;
	this->maxDifferentRooms=max_different_rooms;
	
	this->type=CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::computeInternalStructure(QWidget* parent, Rules& r)
{
	this->_activitiesIndices.clear();
	
	for(int id : std::as_const(activitiesIds)){
		int index=r.activitiesHash.value(id, -1);
		//assert(index>=0);
		if(index>=0) //take care of inactive activities
			_activitiesIndices.append(index);
	}
			
	///////////////////////
	
	if(this->_activitiesIndices.count()<2){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
			tr("Following constraint is wrong (refers to less than two activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
	else{
		assert(this->_activitiesIndices.count()>=2);
		return true;
	}
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::hasInactiveActivities(Rules& r)
{
	//returns true if all or all but one activities are inactive
	
	int cnt=0;
	for(int aid : std::as_const(this->activitiesIds))
		if(r.inactiveActivities.contains(aid))
			cnt++;
			
	if(this->activitiesIds.count()>=2 && (cnt==this->activitiesIds.count() || cnt==this->activitiesIds.count()-1) )
		return true;
	else
		return false;
}

QString ConstraintActivitiesOccupyMaxDifferentRooms::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintActivitiesOccupyMaxDifferentRooms>\n";
	
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	
	s+=IL3+"<Number_of_Activities>"+QString::number(this->activitiesIds.count())+"</Number_of_Activities>\n";
	for(int aid : std::as_const(this->activitiesIds))
		s+=IL3+"<Activity_Id>"+CustomFETString::number(aid)+"</Activity_Id>\n";
	
	s+=IL3+"<Max_Number_of_Different_Rooms>"+CustomFETString::number(this->maxDifferentRooms)+"</Max_Number_of_Different_Rooms>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintActivitiesOccupyMaxDifferentRooms>\n";
	return s;
}

QString ConstraintActivitiesOccupyMaxDifferentRooms::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);
		
	QString actids=QString("");
	for(int aid : std::as_const(this->activitiesIds))
		actids+=getActivityDescription(r, aid)+translatedCommaSpace();
	actids.chop(translatedCommaSpace().size());
		
	QString s=tr("Activities occupy max different rooms, WP:%1%, NA:%2, A: %3, MDR:%4", "Constraint description. WP means weight percentage, "
	 "NA means the number of activities, A means activities list, MDR means max different rooms")
	 .arg(CustomFETString::number(this->weightPercentage))
	 .arg(QString::number(this->activitiesIds.count()))
	 .arg(actids)
	 .arg(CustomFETString::number(this->maxDifferentRooms));
	
	return begin+s+end;
}

QString ConstraintActivitiesOccupyMaxDifferentRooms::getDetailedDescription(Rules& r)
{
	QString actids=QString("");
	for(int aid : std::as_const(this->activitiesIds))
		actids+=CustomFETString::number(aid)+translatedCommaSpace();
	actids.chop(translatedCommaSpace().size());
		
	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Activities occupy max different rooms"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage)); s+="\n";
	s+=tr("Number of activities=%1").arg(QString::number(this->activitiesIds.count())); s+="\n";
	for(int id : std::as_const(this->activitiesIds)){
		s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity")
		 .arg(id)
		 .arg(getActivityDetailedDescription(r, id));
		s+="\n";
	}
	s+=tr("Maximum number of different rooms=%1").arg(CustomFETString::number(this->maxDifferentRooms)); s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}
	
	return s;
}

double ConstraintActivitiesOccupyMaxDifferentRooms::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts

	int nbroken=0;
	
	QSet<int> usedRooms;
	
	for(int ai : std::as_const(this->_activitiesIndices)){
		if(c.rooms[ai]!=UNALLOCATED_SPACE && c.rooms[ai]!=UNSPECIFIED_ROOM)
			if(!usedRooms.contains(c.rooms[ai]))
				usedRooms.insert(c.rooms[ai]);
	}
	
	if(usedRooms.count() > this->maxDifferentRooms){
		nbroken=1;

		if(conflictsString!=nullptr){
			QString s=tr("Space constraint activities occupy max different rooms broken");
			s += QString(". ");
			s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(nbroken*weightPercentage/100));
	
			dl.append(s);
			cl.append(nbroken*weightPercentage/100);
		
			*conflictsString += s+"\n";
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return nbroken*weightPercentage/100;
}

void ConstraintActivitiesOccupyMaxDifferentRooms::removeUseless(Rules& r)
{
	QList<int> newActs;
	
	for(int aid : std::as_const(activitiesIds)){
		Activity* act=r.activitiesPointerHash.value(aid, nullptr);
		if(act!=nullptr)
		//if(validActs.contains(aid))
			newActs.append(aid);
	}
	
	activitiesIds=newActs;
	
	r.internalStructureComputed=false;
}

void ConstraintActivitiesOccupyMaxDifferentRooms::recomputeActivitiesSet()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
	activitiesIdsSet=QSet<int>(activitiesIds.constBegin(), activitiesIds.constEnd());
#else
	activitiesIdsSet=activitiesIds.toSet();
#endif
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::isRelatedToActivity(Activity* a)
{
	return activitiesIdsSet.contains(a->id);

	//return this->activitiesIds.contains(a->id);
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintActivitiesOccupyMaxDifferentRooms::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

ConstraintActivitiesSameRoomIfConsecutive::ConstraintActivitiesSameRoomIfConsecutive()
	: SpaceConstraint()
{
	this->type = CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE;
}

ConstraintActivitiesSameRoomIfConsecutive::ConstraintActivitiesSameRoomIfConsecutive(double wp,
	const QList<int>& a_L)
	: SpaceConstraint(wp)
{
	this->activitiesIds=a_L;
	
	this->type=CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE;
}

bool ConstraintActivitiesSameRoomIfConsecutive::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this cares about inactive activities, also, so do not assert this->_actIndices.count()==this->actIds.count()
	_activitiesIndices.clear();
	for(int id : std::as_const(activitiesIds)){
		int i=r.activitiesHash.value(id, -1);
		if(i>=0)
			_activitiesIndices.append(i);
	}

	///////////////////////
	
	if(this->_activitiesIndices.count()<2){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET error in data"),
			tr("Following constraint is wrong (refers to less than two activities). Please correct it:\n%1").arg(this->getDetailedDescription(r)));
		return false;
	}
	else{
		assert(this->_activitiesIndices.count()>=2);
		return true;
	}
}

bool ConstraintActivitiesSameRoomIfConsecutive::hasInactiveActivities(Rules& r)
{
	//returns true if all or all but one activities are inactive
	
	int cnt=0;
	for(int aid : std::as_const(this->activitiesIds))
		if(r.inactiveActivities.contains(aid))
			cnt++;
			
	if(this->activitiesIds.count()>=2 && (cnt==this->activitiesIds.count() || cnt==this->activitiesIds.count()-1) )
		return true;
	else
		return false;
}

QString ConstraintActivitiesSameRoomIfConsecutive::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintActivitiesSameRoomIfConsecutive>\n";
	
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	
	s+=IL3+"<Number_of_Activities>"+QString::number(this->activitiesIds.count())+"</Number_of_Activities>\n";
	for(int aid : std::as_const(this->activitiesIds))
		s+=IL3+"<Activity_Id>"+CustomFETString::number(aid)+"</Activity_Id>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintActivitiesSameRoomIfConsecutive>\n";
	return s;
}

QString ConstraintActivitiesSameRoomIfConsecutive::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";
		
	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);
		
	QString actids=QString("");
	for(int aid : std::as_const(this->activitiesIds))
		actids+=getActivityDescription(r, aid)+translatedCommaSpace();
	actids.chop(translatedCommaSpace().size());
		
	QString s=tr("Activities same room if consecutive, WP:%1%, NA:%2, A: %3", "Constraint description. WP means weight percentage, "
	 "NA means the number of activities, A means activities list")
	 .arg(CustomFETString::number(this->weightPercentage))
	 .arg(QString::number(this->activitiesIds.count()))
	 .arg(actids);
	
	return begin+s+end;
}

QString ConstraintActivitiesSameRoomIfConsecutive::getDetailedDescription(Rules& r)
{
	QString actids=QString("");
	for(int aid : std::as_const(this->activitiesIds))
		actids+=CustomFETString::number(aid)+translatedCommaSpace();
	actids.chop(translatedCommaSpace().size());
		
	QString s=tr("Space constraint"); s+="\n";
	s+=tr("Activities same room if consecutive"); s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage)); s+="\n";
	s+=tr("Number of activities=%1").arg(QString::number(this->activitiesIds.count())); s+="\n";
	for(int id : std::as_const(this->activitiesIds)){
		s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity")
		 .arg(id)
		 .arg(getActivityDetailedDescription(r, id));
		s+="\n";
	}
	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}
	
	return s;
}

double ConstraintActivitiesSameRoomIfConsecutive::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	//Calculates the number of conflicts

	int nbroken=0;
	
	for(int i=0; i<_activitiesIndices.count(); i++){
		int ai=_activitiesIndices.at(i);
		for(int j=i+1; j<_activitiesIndices.count(); j++){
			int ai2=_activitiesIndices.at(j);
			
			if(c.times[ai]!=UNALLOCATED_TIME && c.times[ai2]!=UNALLOCATED_TIME){
				int d=c.times[ai]%r.nDaysPerWeek;
				int h=c.times[ai]/r.nDaysPerWeek;
				int d2=c.times[ai2]%r.nDaysPerWeek;
				int h2=c.times[ai2]/r.nDaysPerWeek;
			
				if( (d==d2) && (h+r.internalActivitiesList[ai].duration==h2 || h2+r.internalActivitiesList[ai2].duration==h) )
					if(c.rooms[ai]!=UNALLOCATED_SPACE && c.rooms[ai]!=UNSPECIFIED_ROOM)
						if(c.rooms[ai2]!=UNALLOCATED_SPACE && c.rooms[ai2]!=UNSPECIFIED_ROOM)
							if(c.rooms[ai]!=c.rooms[ai2])
								nbroken++;
			}
		}
	}
	
	if(nbroken>0){
		if(conflictsString!=nullptr){
			QString s=tr("Space constraint activities same room if consecutive broken");
			s += QString(". ");
			s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(nbroken*weightPercentage/100));
	
			dl.append(s);
			cl.append(nbroken*weightPercentage/100);
		
			*conflictsString += s+"\n";
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return nbroken*weightPercentage/100;
}

void ConstraintActivitiesSameRoomIfConsecutive::removeUseless(Rules& r)
{
	QList<int> newActs;
	
	for(int aid : std::as_const(activitiesIds)){
		Activity* act=r.activitiesPointerHash.value(aid, nullptr);
		if(act!=nullptr)
		//if(validActs.contains(aid))
			newActs.append(aid);
	}
	
	activitiesIds=newActs;

	r.internalStructureComputed=false;
}

void ConstraintActivitiesSameRoomIfConsecutive::recomputeActivitiesSet()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
	activitiesIdsSet=QSet<int>(activitiesIds.constBegin(), activitiesIds.constEnd());
#else
	activitiesIdsSet=activitiesIds.toSet();
#endif
}

bool ConstraintActivitiesSameRoomIfConsecutive::isRelatedToActivity(Activity* a)
{
	return activitiesIdsSet.contains(a->id);

	//return this->activitiesIds.contains(a->id);
}

bool ConstraintActivitiesSameRoomIfConsecutive::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintActivitiesSameRoomIfConsecutive::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesSameRoomIfConsecutive::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintActivitiesSameRoomIfConsecutive::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintActivitiesSameRoomIfConsecutive::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintActivitiesSameRoomIfConsecutive::hasWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

bool ConstraintActivitiesSameRoomIfConsecutive::canRepairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0);

	return true;
}

bool ConstraintActivitiesSameRoomIfConsecutive::repairWrongDayOrHour(Rules& r)
{
	Q_UNUSED(r);
	assert(0); //should check hasWrongDayOrHour, firstly

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxRoomChangesPerRealDay::ConstraintStudentsMaxRoomChangesPerRealDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY;
}

ConstraintStudentsMaxRoomChangesPerRealDay::ConstraintStudentsMaxRoomChangesPerRealDay(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY;
	this->maxRoomChangesPerDay=mc;
}

bool ConstraintStudentsMaxRoomChangesPerRealDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintStudentsMaxRoomChangesPerRealDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);

	return false;
}

QString ConstraintStudentsMaxRoomChangesPerRealDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsMaxRoomChangesPerRealDay>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Max_Room_Changes_Per_Day>"+CustomFETString::number(this->maxRoomChangesPerDay)+"</Max_Room_Changes_Per_Day>\n";

	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsMaxRoomChangesPerRealDay>\n";

	return s;
}

QString ConstraintStudentsMaxRoomChangesPerRealDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students max room changes per real day"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsMaxRoomChangesPerRealDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students maximum room changes per real day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Maximum room changes per day=%1").arg(this->maxRoomChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxRoomChangesPerRealDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		//Better, less memory
		StudentsSubgroup* sbgpointer=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;

		for(int ai : std::as_const(sbgpointer->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;

				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek/2; d2++){
			int crt_room=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtRoomsTimetable[2*d2][h2]!=-1){
					if(crt_room!=crtRoomsTimetable[2*d2][h2]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[2*d2][h2];
					}
				}
			}
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtRoomsTimetable[2*d2+1][h2]!=-1){
					if(crt_room!=crtRoomsTimetable[2*d2+1][h2]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[2*d2+1][h2];
					}
				}
			}

			if(n_changes>this->maxRoomChangesPerDay){
				nbroken+=-this->maxRoomChangesPerDay+n_changes;

				if(conflictsString!=nullptr){
					QString s=tr("Space constraint students max room changes per real day broken for subgroup=%1 on real day number %2")
						.arg(r.internalSubgroupsList[sbg]->name)
						.arg(d2);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerDay+n_changes)));

					dl.append(s);
					cl.append(weightPercentage/100* (-maxRoomChangesPerDay+n_changes));

					*conflictsString+=s+"\n";
				}
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxRoomChangesPerRealDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxRoomChangesPerRealDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsMaxRoomChangesPerRealDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxRoomChangesPerRealDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxRoomChangesPerRealDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

bool ConstraintStudentsMaxRoomChangesPerRealDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);

	return false;
}

bool ConstraintStudentsMaxRoomChangesPerRealDay::hasWrongDayOrHour(Rules& r)
{
	if(maxRoomChangesPerDay>2*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintStudentsMaxRoomChangesPerRealDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsMaxRoomChangesPerRealDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	if(maxRoomChangesPerDay>2*r.nHoursPerDay)
		maxRoomChangesPerDay=2*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxRoomChangesPerRealDay::ConstraintStudentsSetMaxRoomChangesPerRealDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY;
}

ConstraintStudentsSetMaxRoomChangesPerRealDay::ConstraintStudentsSetMaxRoomChangesPerRealDay(double wp, const QString& st, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY;
	this->studentsName=st;
	this->maxRoomChangesPerDay=mc;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	this->iSubgroupsList.clear();

	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
	StudentsSet* ss=r.studentsHash.value(studentsName, nullptr);

	if(ss==nullptr){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max room changes per real day is wrong because it refers to nonexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}

	populateInternalSubgroupsList(r, ss, this->iSubgroupsList);

	/*if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);*/

	return true;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);

	return false;
}

QString ConstraintStudentsSetMaxRoomChangesPerRealDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsSetMaxRoomChangesPerRealDay>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Students>"+protect(this->studentsName)+"</Students>\n";
	s+=IL3+"<Max_Room_Changes_Per_Day>"+CustomFETString::number(this->maxRoomChangesPerDay)+"</Max_Room_Changes_Per_Day>\n";

	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsSetMaxRoomChangesPerRealDay>\n";

	return s;
}

QString ConstraintStudentsSetMaxRoomChangesPerRealDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set max room changes per real day"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();

	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxRoomChangesPerRealDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students set maximum room changes per real day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=tr("Maximum room changes per day=%1").arg(this->maxRoomChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetMaxRoomChangesPerRealDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	for(int sbg : std::as_const(this->iSubgroupsList)){
		//Better, less memory
		StudentsSubgroup* sbgpointer=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;

		for(int ai : std::as_const(sbgpointer->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;

				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek/2; d2++){
			int crt_room=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtRoomsTimetable[2*d2][h2]!=-1){
					if(crt_room!=crtRoomsTimetable[2*d2][h2]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[2*d2][h2];
					}
				}
			}
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtRoomsTimetable[2*d2+1][h2]!=-1){
					if(crt_room!=crtRoomsTimetable[2*d2+1][h2]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[2*d2+1][h2];
					}
				}
			}

			if(n_changes>this->maxRoomChangesPerDay){
				nbroken+=-this->maxRoomChangesPerDay+n_changes;

				if(conflictsString!=nullptr){
					QString s=tr("Space constraint students set max room changes per real day broken for subgroup=%1 on real day number %2")
						.arg(r.internalSubgroupsList[sbg]->name)
						.arg(d2);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerDay+n_changes)));

					dl.append(s);
					cl.append(weightPercentage/100* (-maxRoomChangesPerDay+n_changes));

					*conflictsString+=s+"\n";
				}
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(s->name, this->studentsName);

	return true;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);

	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDay::hasWrongDayOrHour(Rules& r)
{
	if(maxRoomChangesPerDay>2*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	if(maxRoomChangesPerDay>2*r.nHoursPerDay)
		maxRoomChangesPerDay=2*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxRoomChangesPerRealDay::ConstraintTeacherMaxRoomChangesPerRealDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY;
}

ConstraintTeacherMaxRoomChangesPerRealDay::ConstraintTeacherMaxRoomChangesPerRealDay(double wp, const QString& tc, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY;
	this->teacherName=tc;
	this->maxRoomChangesPerDay=mc;
}

bool ConstraintTeacherMaxRoomChangesPerRealDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);

	if(this->teacher_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher max room changes per real day is wrong because it refers to nonexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}

	return true;
}

bool ConstraintTeacherMaxRoomChangesPerRealDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);

	return false;
}

QString ConstraintTeacherMaxRoomChangesPerRealDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeacherMaxRoomChangesPerRealDay>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+=IL3+"<Max_Room_Changes_Per_Day>"+CustomFETString::number(this->maxRoomChangesPerDay)+"</Max_Room_Changes_Per_Day>\n";

	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeacherMaxRoomChangesPerRealDay>\n";

	return s;
}

QString ConstraintTeacherMaxRoomChangesPerRealDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher max room changes per real day"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();

	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeacherMaxRoomChangesPerRealDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teacher maximum room changes per real day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=tr("Maximum room changes per day=%1").arg(this->maxRoomChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxRoomChangesPerRealDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	int tch=this->teacher_ID;

	//Better, less memory
	Teacher* tchpointer=r.internalTeachersList[tch];

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtRoomsTimetable[d2][h2]=-1;

	for(int ai : std::as_const(tchpointer->activitiesForTeacher))
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d2=c.times[ai]%r.nDaysPerWeek;
			int h2=c.times[ai]/r.nDaysPerWeek;

			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h2+dur<r.nHoursPerDay);
				assert(crtRoomsTimetable[d2][h2+dur]==-1);
				if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
					assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
					crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
				}
			}
		}
	/////////////

	for(int d2=0; d2<r.nDaysPerWeek/2; d2++){
		int crt_room=-1;
		int n_changes=0;
		for(int h2=0; h2<r.nHoursPerDay; h2++){
			if(crtRoomsTimetable[2*d2][h2]!=-1){
				if(crt_room!=crtRoomsTimetable[2*d2][h2]){
					if(crt_room!=-1)
						n_changes++;
					crt_room=crtRoomsTimetable[2*d2][h2];
				}
			}
		}
		for(int h2=0; h2<r.nHoursPerDay; h2++){
			if(crtRoomsTimetable[2*d2+1][h2]!=-1){
				if(crt_room!=crtRoomsTimetable[2*d2+1][h2]){
					if(crt_room!=-1)
						n_changes++;
					crt_room=crtRoomsTimetable[2*d2+1][h2];
				}
			}
		}

		if(n_changes>this->maxRoomChangesPerDay){
			nbroken+=-this->maxRoomChangesPerDay+n_changes;

			if(conflictsString!=nullptr){
				QString s=tr("Space constraint teacher max room changes per real day broken for teacher=%1 on real day number %2")
					.arg(this->teacherName)
					.arg(d2);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerDay+n_changes)));

				dl.append(s);
				cl.append(weightPercentage/100* (-maxRoomChangesPerDay+n_changes));

				*conflictsString+=s+"\n";
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxRoomChangesPerRealDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxRoomChangesPerRealDay::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMaxRoomChangesPerRealDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxRoomChangesPerRealDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxRoomChangesPerRealDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxRoomChangesPerRealDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);

	return false;
}

bool ConstraintTeacherMaxRoomChangesPerRealDay::hasWrongDayOrHour(Rules& r)
{
	if(maxRoomChangesPerDay>2*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintTeacherMaxRoomChangesPerRealDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeacherMaxRoomChangesPerRealDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	if(maxRoomChangesPerDay>2*r.nHoursPerDay)
		maxRoomChangesPerDay=2*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxRoomChangesPerRealDay::ConstraintTeachersMaxRoomChangesPerRealDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY;
}

ConstraintTeachersMaxRoomChangesPerRealDay::ConstraintTeachersMaxRoomChangesPerRealDay(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY;
	this->maxRoomChangesPerDay=mc;
}

bool ConstraintTeachersMaxRoomChangesPerRealDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMaxRoomChangesPerRealDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);

	return false;
}

QString ConstraintTeachersMaxRoomChangesPerRealDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeachersMaxRoomChangesPerRealDay>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Max_Room_Changes_Per_Day>"+CustomFETString::number(this->maxRoomChangesPerDay)+"</Max_Room_Changes_Per_Day>\n";

	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeachersMaxRoomChangesPerRealDay>\n";

	return s;
}

QString ConstraintTeachersMaxRoomChangesPerRealDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teachers max room changes per real day"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeachersMaxRoomChangesPerRealDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teachers maximum room changes per real day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Maximum room changes per day=%1").arg(this->maxRoomChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxRoomChangesPerRealDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	for(int tch=0; tch<r.nInternalTeachers; tch++){
		//Better, less memory
		Teacher* tchpointer=r.internalTeachersList[tch];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;

		for(int ai : std::as_const(tchpointer->activitiesForTeacher))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;

				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek/2; d2++){
			int crt_room=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtRoomsTimetable[2*d2][h2]!=-1){
					if(crt_room!=crtRoomsTimetable[2*d2][h2]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[2*d2][h2];
					}
				}
			}
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtRoomsTimetable[2*d2+1][h2]!=-1){
					if(crt_room!=crtRoomsTimetable[2*d2+1][h2]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[2*d2+1][h2];
					}
				}
			}

			if(n_changes>this->maxRoomChangesPerDay){
				nbroken+=-this->maxRoomChangesPerDay+n_changes;

				if(conflictsString!=nullptr){
					QString s=tr("Space constraint teachers max room changes per real day broken for teacher=%1 on real day number %2")
						.arg(r.internalTeachersList[tch]->name)
						.arg(d2);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerDay+n_changes)));

					dl.append(s);
					cl.append(weightPercentage/100* (-maxRoomChangesPerDay+n_changes));

					*conflictsString+=s+"\n";
				}
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxRoomChangesPerRealDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxRoomChangesPerRealDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return true;
}

bool ConstraintTeachersMaxRoomChangesPerRealDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxRoomChangesPerRealDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxRoomChangesPerRealDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxRoomChangesPerRealDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);

	return false;
}

bool ConstraintTeachersMaxRoomChangesPerRealDay::hasWrongDayOrHour(Rules& r)
{
	if(maxRoomChangesPerDay>2*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintTeachersMaxRoomChangesPerRealDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeachersMaxRoomChangesPerRealDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	if(maxRoomChangesPerDay>2*r.nHoursPerDay)
		maxRoomChangesPerDay=2*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxBuildingChangesPerRealDay::ConstraintStudentsMaxBuildingChangesPerRealDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY;
}

ConstraintStudentsMaxBuildingChangesPerRealDay::ConstraintStudentsMaxBuildingChangesPerRealDay(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY;
	this->maxBuildingChangesPerDay=mc;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);

	return false;
}

QString ConstraintStudentsMaxBuildingChangesPerRealDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsMaxBuildingChangesPerRealDay>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";

	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsMaxBuildingChangesPerRealDay>\n";

	return s;
}

QString ConstraintStudentsMaxBuildingChangesPerRealDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students max building changes per real day"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsMaxBuildingChangesPerRealDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students maximum building changes per real day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxBuildingChangesPerRealDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		//Better, less memory
		StudentsSubgroup* sbgpointer=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;

		for(int ai : std::as_const(sbgpointer->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;

				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek/2; d2++){
			int crt_building=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[2*d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[2*d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[2*d2][h2];
					}
				}
			}
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[2*d2+1][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[2*d2+1][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[2*d2+1][h2];
					}
				}
			}

			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;

				if(conflictsString!=nullptr){
					QString s=tr("Space constraint students max building changes per real day broken for subgroup=%1 on real day number %2")
						.arg(r.internalSubgroupsList[sbg]->name)
						.arg(d2);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));

					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));

					*conflictsString+=s+"\n";
				}
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDay::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerDay>2*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	if(maxBuildingChangesPerDay>2*r.nHoursPerDay)
		maxBuildingChangesPerDay=2*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxBuildingChangesPerRealDay::ConstraintStudentsSetMaxBuildingChangesPerRealDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY;
}

ConstraintStudentsSetMaxBuildingChangesPerRealDay::ConstraintStudentsSetMaxBuildingChangesPerRealDay(double wp, const QString& st, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY;
	this->studentsName=st;
	this->maxBuildingChangesPerDay=mc;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	this->iSubgroupsList.clear();

	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
	StudentsSet* ss=r.studentsHash.value(studentsName, nullptr);

	if(ss==nullptr){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max building changes per real day is wrong because it refers to nonexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}

	populateInternalSubgroupsList(r, ss, this->iSubgroupsList);

	/*if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);*/

	return true;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);

	return false;
}

QString ConstraintStudentsSetMaxBuildingChangesPerRealDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsSetMaxBuildingChangesPerRealDay>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Students>"+protect(this->studentsName)+"</Students>\n";
	s+=IL3+"<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";

	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsSetMaxBuildingChangesPerRealDay>\n";

	return s;
}

QString ConstraintStudentsSetMaxBuildingChangesPerRealDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set max building changes per real day"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();

	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxBuildingChangesPerRealDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students set maximum building changes per real day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetMaxBuildingChangesPerRealDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	for(int sbg : std::as_const(this->iSubgroupsList)){
		//Better, less memory
		StudentsSubgroup* sbgpointer=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;

		for(int ai : std::as_const(sbgpointer->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;

				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek/2; d2++){
			int crt_building=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[2*d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[2*d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[2*d2][h2];
					}
				}
			}
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[2*d2+1][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[2*d2+1][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[2*d2+1][h2];
					}
				}
			}

			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;

				if(conflictsString!=nullptr){
					QString s=tr("Space constraint students set max building changes per real day broken for subgroup=%1 on real day number %2")
						.arg(r.internalSubgroupsList[sbg]->name)
						.arg(d2);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));

					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));

					*conflictsString+=s+"\n";
				}
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(s->name, this->studentsName);

	return true;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDay::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerDay>2*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	if(maxBuildingChangesPerDay>2*r.nHoursPerDay)
		maxBuildingChangesPerDay=2*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxBuildingChangesPerRealDay::ConstraintTeacherMaxBuildingChangesPerRealDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY;
}

ConstraintTeacherMaxBuildingChangesPerRealDay::ConstraintTeacherMaxBuildingChangesPerRealDay(double wp, const QString& tc, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY;
	this->teacherName=tc;
	this->maxBuildingChangesPerDay=mc;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);

	if(this->teacher_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher max building changes per real day is wrong because it refers to nonexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));

		return false;
	}

	return true;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);

	return false;
}

QString ConstraintTeacherMaxBuildingChangesPerRealDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeacherMaxBuildingChangesPerRealDay>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	s+=IL3+"<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";

	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeacherMaxBuildingChangesPerRealDay>\n";

	return s;
}

QString ConstraintTeacherMaxBuildingChangesPerRealDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher max building changes per real day"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();

	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeacherMaxBuildingChangesPerRealDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teacher maximum building changes per real day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxBuildingChangesPerRealDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	int tch=this->teacher_ID;

	//Better, less memory
	Teacher* tchpointer=r.internalTeachersList[tch];

	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtBuildingsTimetable[d2][h2]=-1;

	for(int ai : std::as_const(tchpointer->activitiesForTeacher))
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d2=c.times[ai]%r.nDaysPerWeek;
			int h2=c.times[ai]/r.nDaysPerWeek;

			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h2+dur<r.nHoursPerDay);
				assert(crtBuildingsTimetable[d2][h2+dur]==-1);
				if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
					assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
					crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
				}
			}
		}
	/////////////

	for(int d2=0; d2<r.nDaysPerWeek/2; d2++){
		int crt_building=-1;
		int n_changes=0;
		for(int h2=0; h2<r.nHoursPerDay; h2++){
			if(crtBuildingsTimetable[2*d2][h2]!=-1){
				if(crt_building!=crtBuildingsTimetable[2*d2][h2]){
					if(crt_building!=-1)
						n_changes++;
					crt_building=crtBuildingsTimetable[2*d2][h2];
				}
			}
		}
		for(int h2=0; h2<r.nHoursPerDay; h2++){
			if(crtBuildingsTimetable[2*d2+1][h2]!=-1){
				if(crt_building!=crtBuildingsTimetable[2*d2+1][h2]){
					if(crt_building!=-1)
						n_changes++;
					crt_building=crtBuildingsTimetable[2*d2+1][h2];
				}
			}
		}

		if(n_changes>this->maxBuildingChangesPerDay){
			nbroken+=-this->maxBuildingChangesPerDay+n_changes;

			if(conflictsString!=nullptr){
				QString s=tr("Space constraint teacher max building changes per real day broken for teacher=%1 on real day number %2")
					.arg(this->teacherName)
					.arg(d2);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));

				dl.append(s);
				cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));

				*conflictsString+=s+"\n";
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDay::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDay::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerDay>2*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	if(maxBuildingChangesPerDay>2*r.nHoursPerDay)
		maxBuildingChangesPerDay=2*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxBuildingChangesPerRealDay::ConstraintTeachersMaxBuildingChangesPerRealDay()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY;
}

ConstraintTeachersMaxBuildingChangesPerRealDay::ConstraintTeachersMaxBuildingChangesPerRealDay(double wp, int mc)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY;
	this->maxBuildingChangesPerDay=mc;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDay::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDay::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);

	return false;
}

QString ConstraintTeachersMaxBuildingChangesPerRealDay::getXmlDescription(Rules& r){
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeachersMaxBuildingChangesPerRealDay>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";

	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeachersMaxBuildingChangesPerRealDay>\n";

	return s;
}

QString ConstraintTeachersMaxBuildingChangesPerRealDay::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teachers max building changes per real day"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeachersMaxBuildingChangesPerRealDay::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teachers maximum building changes per real day"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxBuildingChangesPerRealDay::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	for(int tch=0; tch<r.nInternalTeachers; tch++){
		//Better, less memory
		Teacher* tchpointer=r.internalTeachersList[tch];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;

		for(int ai : std::as_const(tchpointer->activitiesForTeacher))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;

				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////

		for(int d2=0; d2<r.nDaysPerWeek/2; d2++){
			int crt_building=-1;
			int n_changes=0;
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[2*d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[2*d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[2*d2][h2];
					}
				}
			}
			for(int h2=0; h2<r.nHoursPerDay; h2++){
				if(crtBuildingsTimetable[2*d2+1][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[2*d2+1][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[2*d2+1][h2];
					}
				}
			}

			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;

				if(conflictsString!=nullptr){
					QString s=tr("Space constraint teachers max building changes per real day broken for teacher=%1 on real day number %2")
						.arg(r.internalTeachersList[tch]->name)
						.arg(d2);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));

					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));

					*conflictsString+=s+"\n";
				}
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDay::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDay::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDay::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDay::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDay::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDay::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDay::hasWrongDayOrHour(Rules& r)
{
	if(maxBuildingChangesPerDay>2*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDay::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDay::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	if(maxBuildingChangesPerDay>2*r.nHoursPerDay)
		maxBuildingChangesPerDay=2*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::ConstraintStudentsSetMaxBuildingChangesPerDayInInterval()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL;
}

ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::ConstraintStudentsSetMaxBuildingChangesPerDayInInterval(double wp, const QString& st, int mc, int is, int ie)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL;
	this->studentsName=st;
	this->maxBuildingChangesPerDay=mc;
	
	this->intervalStart=is;
	this->intervalEnd=ie;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::computeInternalStructure(QWidget* parent, Rules& r)
{
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
	StudentsSet* ss=r.studentsHash.value(studentsName, nullptr);
	
	if(ss==nullptr){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max building changes per day in hourly interval is wrong because it refers to nonexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}
	
	populateInternalSubgroupsList(r, ss, this->iSubgroupsList);
	/*this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);*/

	return true;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsSetMaxBuildingChangesPerDayInInterval>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Students>"+protect(this->studentsName)+"</Students>\n";
	
	s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart])+"</Interval_Start_Hour>\n";
	if(this->intervalEnd < r.nHoursPerDay){
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd])+"</Interval_End_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_End_Hour></Interval_End_Hour>\n";
		s+=IL3+"<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}

	s+=IL3+"<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsSetMaxBuildingChangesPerDayInInterval>\n";

	return s;
}

QString ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set max building changes per day in hourly interval"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=translatedCommaSpace();

	s+=tr("ISH:%1", "Abbreviation for interval start hour. Please use different abbreviations for interval start hour and interval end hour.").arg(r.hoursOfTheDay[this->intervalStart]);s+=translatedCommaSpace();
	if(this->intervalEnd<r.nHoursPerDay)
		s+=tr("IEH:%1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(r.hoursOfTheDay[this->intervalEnd]);
	else
		s+=tr("IEH:%1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(tr("End of the day"));
	s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students set maximum building changes per day in hourly interval"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=tr("Interval start hour=%1").arg(r.hoursOfTheDay[this->intervalStart]);s+="\n";
	if(this->intervalEnd<r.nHoursPerDay)
		s+=tr("Interval end hour=%1").arg(r.hoursOfTheDay[this->intervalEnd]);
	else
		s+=tr("Interval end hour=%1").arg(tr("End of the day"));
	s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int sbg : std::as_const(this->iSubgroupsList)){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
		
		for(int ai : std::as_const(sts->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////
	
		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_building=-1;
			int n_changes=0;
			for(int h2=this->intervalStart; h2<this->intervalEnd; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d2][h2];
					}
				}
			}
			
			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;
		
				if(conflictsString!=nullptr){
					QString s=tr("Space constraint students set max building changes per day in hourly interval broken for students=%1 on day %2")
						.arg(this->studentsName)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(s->name, this->studentsName);
}

bool ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::hasWrongDayOrHour(Rules& r)
{
	if(intervalStart>=r.nHoursPerDay)
		return true;
	if(intervalEnd>r.nHoursPerDay)
		return true;
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(intervalStart<r.nHoursPerDay && intervalEnd<=r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerDayInInterval::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(intervalStart<r.nHoursPerDay && intervalEnd<=r.nHoursPerDay);
	
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		maxBuildingChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxBuildingChangesPerDayInInterval::ConstraintStudentsMaxBuildingChangesPerDayInInterval()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL;
}

ConstraintStudentsMaxBuildingChangesPerDayInInterval::ConstraintStudentsMaxBuildingChangesPerDayInInterval(double wp, int mc, int is, int ie)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL;
	this->maxBuildingChangesPerDay=mc;
	
	this->intervalStart=is;
	this->intervalEnd=ie;
}

bool ConstraintStudentsMaxBuildingChangesPerDayInInterval::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerDayInInterval::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsMaxBuildingChangesPerDayInInterval::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsMaxBuildingChangesPerDayInInterval>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	
	s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart])+"</Interval_Start_Hour>\n";
	if(this->intervalEnd < r.nHoursPerDay){
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd])+"</Interval_End_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_End_Hour></Interval_End_Hour>\n";
		s+=IL3+"<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}

	s+=IL3+"<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsMaxBuildingChangesPerDayInInterval>\n";

	return s;
}

QString ConstraintStudentsMaxBuildingChangesPerDayInInterval::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students max building changes per day in hourly interval"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("ISH:%1", "Abbreviation for interval start hour. Please use different abbreviations for interval start hour and interval end hour.").arg(r.hoursOfTheDay[this->intervalStart]);s+=translatedCommaSpace();
	if(this->intervalEnd<r.nHoursPerDay)
		s+=tr("IEH:%1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(r.hoursOfTheDay[this->intervalEnd]);
	else
		s+=tr("IEH:%1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(tr("End of the day"));
	s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsMaxBuildingChangesPerDayInInterval::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students maximum building changes per day in hourly interval"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Interval start hour=%1").arg(r.hoursOfTheDay[this->intervalStart]);s+="\n";
	if(this->intervalEnd<r.nHoursPerDay)
		s+=tr("Interval end hour=%1").arg(r.hoursOfTheDay[this->intervalEnd]);
	else
		s+=tr("Interval end hour=%1").arg(tr("End of the day"));
	s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxBuildingChangesPerDayInInterval::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
		
		for(int ai : std::as_const(sts->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////
	
		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_building=-1;
			int n_changes=0;
			for(int h2=this->intervalStart; h2<this->intervalEnd; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d2][h2];
					}
				}
			}
			
			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;
		
				if(conflictsString!=nullptr){
					QString s=tr("Space constraint students max building changes per day in hourly interval broken for subgroup=%1 on day %2")
						.arg(r.internalSubgroupsList[sbg]->name)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxBuildingChangesPerDayInInterval::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDayInInterval::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDayInInterval::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDayInInterval::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDayInInterval::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerDayInInterval::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDayInInterval::hasWrongDayOrHour(Rules& r)
{
	if(intervalStart>=r.nHoursPerDay)
		return true;
	if(intervalEnd>r.nHoursPerDay)
		return true;
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDayInInterval::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(intervalStart<r.nHoursPerDay && intervalEnd<=r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerDayInInterval::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(intervalStart<r.nHoursPerDay && intervalEnd<=r.nHoursPerDay);
	
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		maxBuildingChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxBuildingChangesPerDayInInterval::ConstraintTeacherMaxBuildingChangesPerDayInInterval()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL;
}

ConstraintTeacherMaxBuildingChangesPerDayInInterval::ConstraintTeacherMaxBuildingChangesPerDayInInterval(double wp, const QString& tc, int mc, int is, int ie)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL;
	this->teacherName=tc;
	this->maxBuildingChangesPerDay=mc;
	
	this->intervalStart=is;
	this->intervalEnd=ie;
}

bool ConstraintTeacherMaxBuildingChangesPerDayInInterval::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	
	if(this->teacher_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher max building changes per day in hourly interval is wrong because it refers to nonexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}

	return true;
}

bool ConstraintTeacherMaxBuildingChangesPerDayInInterval::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherMaxBuildingChangesPerDayInInterval::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeacherMaxBuildingChangesPerDayInInterval>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	
	s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart])+"</Interval_Start_Hour>\n";
	if(this->intervalEnd < r.nHoursPerDay){
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd])+"</Interval_End_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_End_Hour></Interval_End_Hour>\n";
		s+=IL3+"<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}

	s+=IL3+"<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeacherMaxBuildingChangesPerDayInInterval>\n";

	return s;
}

QString ConstraintTeacherMaxBuildingChangesPerDayInInterval::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher max building changes per day in hourly interval"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();

	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=translatedCommaSpace();

	s+=tr("ISH:%1", "Abbreviation for interval start hour. Please use different abbreviations for interval start hour and interval end hour.").arg(r.hoursOfTheDay[this->intervalStart]);s+=translatedCommaSpace();
	if(this->intervalEnd<r.nHoursPerDay)
		s+=tr("IEH:%1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(r.hoursOfTheDay[this->intervalEnd]);
	else
		s+=tr("IEH:%1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(tr("End of the day"));
	s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeacherMaxBuildingChangesPerDayInInterval::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teacher maximum building changes per day in hourly interval"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=tr("Interval start hour=%1").arg(r.hoursOfTheDay[this->intervalStart]);s+="\n";
	if(this->intervalEnd<r.nHoursPerDay)
		s+=tr("Interval end hour=%1").arg(r.hoursOfTheDay[this->intervalEnd]);
	else
		s+=tr("Interval end hour=%1").arg(tr("End of the day"));
	s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxBuildingChangesPerDayInInterval::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	int tch=this->teacher_ID;

	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	//Better, less memory
	Teacher* tchpointer=r.internalTeachersList[tch];
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtBuildingsTimetable[d2][h2]=-1;
	
	for(int ai : std::as_const(tchpointer->activitiesForTeacher))
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d2=c.times[ai]%r.nDaysPerWeek;
			int h2=c.times[ai]/r.nDaysPerWeek;
			
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h2+dur<r.nHoursPerDay);
				assert(crtBuildingsTimetable[d2][h2+dur]==-1);
				if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
					assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
					crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
				}
			}
		}
	/////////////
	
	for(int d2=0; d2<r.nDaysPerWeek; d2++){
		int crt_building=-1;
		int n_changes=0;
		for(int h2=this->intervalStart; h2<this->intervalEnd; h2++){
			if(crtBuildingsTimetable[d2][h2]!=-1){
				if(crt_building!=crtBuildingsTimetable[d2][h2]){
					if(crt_building!=-1)
						n_changes++;
					crt_building=crtBuildingsTimetable[d2][h2];
				}
			}
		}
		
		if(n_changes>this->maxBuildingChangesPerDay){
			nbroken+=-this->maxBuildingChangesPerDay+n_changes;
	
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint teacher max building changes per day in hourly interval broken for teacher=%1 on day %2")
					.arg(this->teacherName)
					.arg(r.daysOfTheWeek[d2]);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));
				
				dl.append(s);
				cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxBuildingChangesPerDayInInterval::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDayInInterval::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMaxBuildingChangesPerDayInInterval::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDayInInterval::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDayInInterval::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDayInInterval::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDayInInterval::hasWrongDayOrHour(Rules& r)
{
	if(intervalStart>=r.nHoursPerDay)
		return true;
	if(intervalEnd>r.nHoursPerDay)
		return true;
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDayInInterval::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(intervalStart<r.nHoursPerDay && intervalEnd<=r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerDayInInterval::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(intervalStart<r.nHoursPerDay && intervalEnd<=r.nHoursPerDay);
	
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		maxBuildingChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxBuildingChangesPerDayInInterval::ConstraintTeachersMaxBuildingChangesPerDayInInterval()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL;
}

ConstraintTeachersMaxBuildingChangesPerDayInInterval::ConstraintTeachersMaxBuildingChangesPerDayInInterval(double wp, int mc, int is, int ie)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL;
	this->maxBuildingChangesPerDay=mc;
	
	this->intervalStart=is;
	this->intervalEnd=ie;
}

bool ConstraintTeachersMaxBuildingChangesPerDayInInterval::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerDayInInterval::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeachersMaxBuildingChangesPerDayInInterval::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeachersMaxBuildingChangesPerDayInInterval>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	
	s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart])+"</Interval_Start_Hour>\n";
	if(this->intervalEnd < r.nHoursPerDay){
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd])+"</Interval_End_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_End_Hour></Interval_End_Hour>\n";
		s+=IL3+"<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}

	s+=IL3+"<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeachersMaxBuildingChangesPerDayInInterval>\n";

	return s;
}

QString ConstraintTeachersMaxBuildingChangesPerDayInInterval::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teachers max building changes per day in hourly interval"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();

	s+=tr("ISH:%1", "Abbreviation for interval start hour. Please use different abbreviations for interval start hour and interval end hour.").arg(r.hoursOfTheDay[this->intervalStart]);s+=translatedCommaSpace();
	if(this->intervalEnd<r.nHoursPerDay)
		s+=tr("IEH:%1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(r.hoursOfTheDay[this->intervalEnd]);
	else
		s+=tr("IEH:%1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(tr("End of the day"));
	s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeachersMaxBuildingChangesPerDayInInterval::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teachers maximum building changes per day in hourly interval"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Interval start hour=%1").arg(r.hoursOfTheDay[this->intervalStart]);s+="\n";
	if(this->intervalEnd<r.nHoursPerDay)
		s+=tr("Interval end hour=%1").arg(r.hoursOfTheDay[this->intervalEnd]);
	else
		s+=tr("Interval end hour=%1").arg(tr("End of the day"));
	s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxBuildingChangesPerDayInInterval::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int tch=0; tch<r.nInternalTeachers; tch++){
		//Better, less memory
		Teacher* tchpointer=r.internalTeachersList[tch];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;
		
		for(int ai : std::as_const(tchpointer->activitiesForTeacher))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////
		
		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_building=-1;
			int n_changes=0;
			for(int h2=this->intervalStart; h2<this->intervalEnd; h2++){
				if(crtBuildingsTimetable[d2][h2]!=-1){
					if(crt_building!=crtBuildingsTimetable[d2][h2]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d2][h2];
					}
				}
			}
			
			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;
			
				if(conflictsString!=nullptr){
					QString s=tr("Space constraint teachers max building changes per day in hourly interval broken for teacher=%1 on day %2")
						.arg(r.internalTeachersList[tch]->name)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxBuildingChangesPerDayInInterval::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDayInInterval::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerDayInInterval::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDayInInterval::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDayInInterval::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDayInInterval::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDayInInterval::hasWrongDayOrHour(Rules& r)
{
	if(intervalStart>=r.nHoursPerDay)
		return true;
	if(intervalEnd>r.nHoursPerDay)
		return true;
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDayInInterval::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(intervalStart<r.nHoursPerDay && intervalEnd<=r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerDayInInterval::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(intervalStart<r.nHoursPerDay && intervalEnd<=r.nHoursPerDay);
	
	if(maxBuildingChangesPerDay>r.nHoursPerDay)
		maxBuildingChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL;
}

ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval(double wp, const QString& st, int mc, int is, int ie)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL;
	this->studentsName=st;
	this->maxBuildingChangesPerDay=mc;
	
	this->intervalStart=is;
	this->intervalEnd=ie;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::computeInternalStructure(QWidget* parent, Rules& r)
{
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
	StudentsSet* ss=r.studentsHash.value(studentsName, nullptr);
	
	if(ss==nullptr){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max building changes per real day in hourly interval is wrong because it refers to nonexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}
	
	populateInternalSubgroupsList(r, ss, this->iSubgroupsList);
	/*this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);*/

	return true;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Students>"+protect(this->studentsName)+"</Students>\n";
	
	if(this->intervalStart<r.nHoursPerDay){
		s+=IL3+"<Interval_Start_Day>Morning</Interval_Start_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart])+"</Interval_Start_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_Start_Day>Afternoon</Interval_Start_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay])+"</Interval_Start_Hour>\n";
	}

	if(this->intervalEnd<r.nHoursPerDay){
		s+=IL3+"<Interval_End_Day>Morning</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd])+"</Interval_End_Hour>\n";
	}
	else if(this->intervalEnd<2*r.nHoursPerDay){
		s+=IL3+"<Interval_End_Day>Afternoon</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay])+"</Interval_End_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_End_Day>Afternoon</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour></Interval_End_Hour>\n";
		s+=IL3+"<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}

	s+=IL3+"<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval>\n";

	return s;
}

QString ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set max building changes per real day in hourly interval"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=translatedCommaSpace();

	QString sh;
	if(this->intervalStart<r.nHoursPerDay)
		sh=tr("M %1", "M means morning. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the morning.").arg(r.hoursOfTheDay[this->intervalStart]);
	else
		sh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay]);
	s+=tr("ISH: %1", "Abbreviation for interval start hour. Please use different abbreviations for interval start hour and interval end hour.").arg(sh);
	s+=translatedCommaSpace();

	QString eh;
	if(this->intervalEnd<r.nHoursPerDay)
		eh=tr("M %1", "M means morning. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the morning.").arg(r.hoursOfTheDay[this->intervalEnd]);
	else if(this->intervalEnd<2*r.nHoursPerDay)
		eh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay]);
	else
		eh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(tr("End of the day"));
	s+=tr("IEH: %1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(eh);
	s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students set maximum building changes per real day in hourly interval"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	QString sh;
	if(this->intervalStart<r.nHoursPerDay)
		sh=tr("Morning %1", "%1 is an hour of the morning").arg(r.hoursOfTheDay[this->intervalStart]);
	else
		sh=tr("Afternoon %1", "%1 is an hour of the afternoon").arg(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay]);
	s+=tr("Interval start hour=%1").arg(sh);s+="\n";
	
	QString eh;
	if(this->intervalEnd<r.nHoursPerDay)
		eh=tr("Morning %1", "%1 is an hour of the morning").arg(r.hoursOfTheDay[this->intervalEnd]);
	else if(this->intervalEnd<2*r.nHoursPerDay)
		eh=tr("Afternoon %1", "%1 is an hour of the afternoon").arg(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay]);
	else
		eh=tr("End of the real day");
	s+=tr("Interval end hour=%1").arg(eh);s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	for(int sbg : std::as_const(this->iSubgroupsList)){
		//Better, less memory
		StudentsSubgroup* sbgpointer=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;

		for(int ai : std::as_const(sbgpointer->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;

				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////
		for(int d2=0; d2<r.nDaysPerWeek/2; d2++){
			int crt_building=-1;
			int n_changes=0;
			for(int h2=intervalStart; h2<intervalEnd; h2++){
				int d3=2*d2+h2/r.nHoursPerDay;
				int h3=h2%r.nHoursPerDay;

				if(crtBuildingsTimetable[d3][h3]!=-1){
					if(crt_building!=crtBuildingsTimetable[d3][h3]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d3][h3];
					}
				}
			}

			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;

				if(conflictsString!=nullptr){
					QString s=tr("Space constraint students set max building changes per real day in hourly interval broken for subgroup=%1 on real day number %2")
						.arg(r.internalSubgroupsList[sbg]->name)
						.arg(d2);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));

					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));

					*conflictsString+=s+"\n";
				}
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(s->name, this->studentsName);
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::hasWrongDayOrHour(Rules& r)
{
	if(intervalStart>=2*r.nHoursPerDay)
		return true;
	if(intervalEnd>2*r.nHoursPerDay)
		return true;
	if(maxBuildingChangesPerDay>2*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(intervalStart<2*r.nHoursPerDay && intervalEnd<=2*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(intervalStart<2*r.nHoursPerDay && intervalEnd<=2*r.nHoursPerDay);
	
	if(maxBuildingChangesPerDay>2*r.nHoursPerDay)
		maxBuildingChangesPerDay=2*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::ConstraintStudentsMaxBuildingChangesPerRealDayInInterval()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL;
}

ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::ConstraintStudentsMaxBuildingChangesPerRealDayInInterval(double wp, int mc, int is, int ie)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL;
	this->maxBuildingChangesPerDay=mc;
	
	this->intervalStart=is;
	this->intervalEnd=ie;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsMaxBuildingChangesPerRealDayInInterval>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	
	if(this->intervalStart<r.nHoursPerDay){
		s+=IL3+"<Interval_Start_Day>Morning</Interval_Start_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart])+"</Interval_Start_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_Start_Day>Afternoon</Interval_Start_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay])+"</Interval_Start_Hour>\n";
	}

	if(this->intervalEnd<r.nHoursPerDay){
		s+=IL3+"<Interval_End_Day>Morning</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd])+"</Interval_End_Hour>\n";
	}
	else if(this->intervalEnd<2*r.nHoursPerDay){
		s+=IL3+"<Interval_End_Day>Afternoon</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay])+"</Interval_End_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_End_Day>Afternoon</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour></Interval_End_Hour>\n";
		s+=IL3+"<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}

	s+=IL3+"<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsMaxBuildingChangesPerRealDayInInterval>\n";

	return s;
}

QString ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students max building changes per real day in hourly interval"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	QString sh;
	if(this->intervalStart<r.nHoursPerDay)
		sh=tr("M %1", "M means morning. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the morning.").arg(r.hoursOfTheDay[this->intervalStart]);
	else
		sh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay]);
	s+=tr("ISH: %1", "Abbreviation for interval start hour. Please use different abbreviations for interval start hour and interval end hour.").arg(sh);
	s+=translatedCommaSpace();

	QString eh;
	if(this->intervalEnd<r.nHoursPerDay)
		eh=tr("M %1", "M means morning. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the morning.").arg(r.hoursOfTheDay[this->intervalEnd]);
	else if(this->intervalEnd<2*r.nHoursPerDay)
		eh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay]);
	else
		eh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(tr("End of the day"));
	s+=tr("IEH: %1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(eh);
	s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students maximum building changes per real day in hourly interval"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	QString sh;
	if(this->intervalStart<r.nHoursPerDay)
		sh=tr("Morning %1", "%1 is an hour of the morning").arg(r.hoursOfTheDay[this->intervalStart]);
	else
		sh=tr("Afternoon %1", "%1 is an hour of the afternoon").arg(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay]);
	s+=tr("Interval start hour=%1").arg(sh);s+="\n";
	
	QString eh;
	if(this->intervalEnd<r.nHoursPerDay)
		eh=tr("Morning %1", "%1 is an hour of the morning").arg(r.hoursOfTheDay[this->intervalEnd]);
	else if(this->intervalEnd<2*r.nHoursPerDay)
		eh=tr("Afternoon %1", "%1 is an hour of the afternoon").arg(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay]);
	else
		eh=tr("End of the real day");
	s+=tr("Interval end hour=%1").arg(eh);s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		//Better, less memory
		StudentsSubgroup* sbgpointer=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;

		for(int ai : std::as_const(sbgpointer->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;

				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////
		for(int d2=0; d2<r.nDaysPerWeek/2; d2++){
			int crt_building=-1;
			int n_changes=0;
			for(int h2=intervalStart; h2<intervalEnd; h2++){
				int d3=2*d2+h2/r.nHoursPerDay;
				int h3=h2%r.nHoursPerDay;

				if(crtBuildingsTimetable[d3][h3]!=-1){
					if(crt_building!=crtBuildingsTimetable[d3][h3]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d3][h3];
					}
				}
			}

			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;

				if(conflictsString!=nullptr){
					QString s=tr("Space constraint students max building changes per real day in hourly interval broken for subgroup=%1 on real day number %2")
						.arg(r.internalSubgroupsList[sbg]->name)
						.arg(d2);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));

					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));

					*conflictsString+=s+"\n";
				}
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return true;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::hasWrongDayOrHour(Rules& r)
{
	if(intervalStart>=2*r.nHoursPerDay)
		return true;
	if(intervalEnd>2*r.nHoursPerDay)
		return true;
	if(maxBuildingChangesPerDay>2*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(intervalStart<2*r.nHoursPerDay && intervalEnd<=2*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintStudentsMaxBuildingChangesPerRealDayInInterval::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(intervalStart<2*r.nHoursPerDay && intervalEnd<=2*r.nHoursPerDay);
	
	if(maxBuildingChangesPerDay>2*r.nHoursPerDay)
		maxBuildingChangesPerDay=2*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::ConstraintTeacherMaxBuildingChangesPerRealDayInInterval()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL;
}

ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::ConstraintTeacherMaxBuildingChangesPerRealDayInInterval(double wp, const QString& tc, int mc, int is, int ie)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL;
	this->teacherName=tc;
	this->maxBuildingChangesPerDay=mc;
	
	this->intervalStart=is;
	this->intervalEnd=ie;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	
	if(this->teacher_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher max building changes per real day in hourly interval is wrong because it refers to nonexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}

	return true;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeacherMaxBuildingChangesPerRealDayInInterval>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	
	if(this->intervalStart<r.nHoursPerDay){
		s+=IL3+"<Interval_Start_Day>Morning</Interval_Start_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart])+"</Interval_Start_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_Start_Day>Afternoon</Interval_Start_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay])+"</Interval_Start_Hour>\n";
	}

	if(this->intervalEnd<r.nHoursPerDay){
		s+=IL3+"<Interval_End_Day>Morning</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd])+"</Interval_End_Hour>\n";
	}
	else if(this->intervalEnd<2*r.nHoursPerDay){
		s+=IL3+"<Interval_End_Day>Afternoon</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay])+"</Interval_End_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_End_Day>Afternoon</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour></Interval_End_Hour>\n";
		s+=IL3+"<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}

	s+=IL3+"<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeacherMaxBuildingChangesPerRealDayInInterval>\n";

	return s;
}

QString ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher max building changes per real day in hourly interval"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=translatedCommaSpace();

	QString sh;
	if(this->intervalStart<r.nHoursPerDay)
		sh=tr("M %1", "M means morning. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the morning.").arg(r.hoursOfTheDay[this->intervalStart]);
	else
		sh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay]);
	s+=tr("ISH: %1", "Abbreviation for interval start hour. Please use different abbreviations for interval start hour and interval end hour.").arg(sh);
	s+=translatedCommaSpace();

	QString eh;
	if(this->intervalEnd<r.nHoursPerDay)
		eh=tr("M %1", "M means morning. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the morning.").arg(r.hoursOfTheDay[this->intervalEnd]);
	else if(this->intervalEnd<2*r.nHoursPerDay)
		eh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay]);
	else
		eh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(tr("End of the day"));
	s+=tr("IEH: %1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(eh);
	s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teacher maximum building changes per real day in hourly interval"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	QString sh;
	if(this->intervalStart<r.nHoursPerDay)
		sh=tr("Morning %1", "%1 is an hour of the morning").arg(r.hoursOfTheDay[this->intervalStart]);
	else
		sh=tr("Afternoon %1", "%1 is an hour of the afternoon").arg(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay]);
	s+=tr("Interval start hour=%1").arg(sh);s+="\n";
	
	QString eh;
	if(this->intervalEnd<r.nHoursPerDay)
		eh=tr("Morning %1", "%1 is an hour of the morning").arg(r.hoursOfTheDay[this->intervalEnd]);
	else if(this->intervalEnd<2*r.nHoursPerDay)
		eh=tr("Afternoon %1", "%1 is an hour of the afternoon").arg(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay]);
	else
		eh=tr("End of the real day");
	s+=tr("Interval end hour=%1").arg(eh);s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	int tch=this->teacher_ID;

	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	//Better, less memory
	Teacher* tchpointer=r.internalTeachersList[tch];
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtBuildingsTimetable[d2][h2]=-1;

	for(int ai : std::as_const(tchpointer->activitiesForTeacher))
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d2=c.times[ai]%r.nDaysPerWeek;
			int h2=c.times[ai]/r.nDaysPerWeek;

			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h2+dur<r.nHoursPerDay);
				assert(crtBuildingsTimetable[d2][h2+dur]==-1);
				if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
					assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
					crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
				}
			}
		}
	/////////////
	for(int d2=0; d2<r.nDaysPerWeek/2; d2++){
		int crt_building=-1;
		int n_changes=0;
		for(int h2=intervalStart; h2<intervalEnd; h2++){
			int d3=2*d2+h2/r.nHoursPerDay;
			int h3=h2%r.nHoursPerDay;

			if(crtBuildingsTimetable[d3][h3]!=-1){
				if(crt_building!=crtBuildingsTimetable[d3][h3]){
					if(crt_building!=-1)
						n_changes++;
					crt_building=crtBuildingsTimetable[d3][h3];
				}
			}
		}

		if(n_changes>this->maxBuildingChangesPerDay){
			nbroken+=-this->maxBuildingChangesPerDay+n_changes;

			if(conflictsString!=nullptr){
				QString s=tr("Space constraint teacher max building changes per real day in hourly interval broken for teacher=%1 on real day number %2")
					.arg(this->teacherName)
					.arg(d2);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));

				dl.append(s);
				cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));

				*conflictsString+=s+"\n";
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::hasWrongDayOrHour(Rules& r)
{
	if(intervalStart>=2*r.nHoursPerDay)
		return true;
	if(intervalEnd>2*r.nHoursPerDay)
		return true;
	if(maxBuildingChangesPerDay>2*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(intervalStart<2*r.nHoursPerDay && intervalEnd<=2*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintTeacherMaxBuildingChangesPerRealDayInInterval::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(intervalStart<2*r.nHoursPerDay && intervalEnd<=2*r.nHoursPerDay);
	
	if(maxBuildingChangesPerDay>2*r.nHoursPerDay)
		maxBuildingChangesPerDay=2*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::ConstraintTeachersMaxBuildingChangesPerRealDayInInterval()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL;
}

ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::ConstraintTeachersMaxBuildingChangesPerRealDayInInterval(double wp, int mc, int is, int ie)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL;
	this->maxBuildingChangesPerDay=mc;
	
	this->intervalStart=is;
	this->intervalEnd=ie;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeachersMaxBuildingChangesPerRealDayInInterval>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	
	if(this->intervalStart<r.nHoursPerDay){
		s+=IL3+"<Interval_Start_Day>Morning</Interval_Start_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart])+"</Interval_Start_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_Start_Day>Afternoon</Interval_Start_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay])+"</Interval_Start_Hour>\n";
	}

	if(this->intervalEnd<r.nHoursPerDay){
		s+=IL3+"<Interval_End_Day>Morning</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd])+"</Interval_End_Hour>\n";
	}
	else if(this->intervalEnd<2*r.nHoursPerDay){
		s+=IL3+"<Interval_End_Day>Afternoon</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay])+"</Interval_End_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_End_Day>Afternoon</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour></Interval_End_Hour>\n";
		s+=IL3+"<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}

	s+=IL3+"<Max_Building_Changes_Per_Day>"+CustomFETString::number(this->maxBuildingChangesPerDay)+"</Max_Building_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeachersMaxBuildingChangesPerRealDayInInterval>\n";

	return s;
}

QString ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teachers max building changes per real day in hourly interval"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	QString sh;
	if(this->intervalStart<r.nHoursPerDay)
		sh=tr("M %1", "M means morning. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the morning.").arg(r.hoursOfTheDay[this->intervalStart]);
	else
		sh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay]);
	s+=tr("ISH: %1", "Abbreviation for interval start hour. Please use different abbreviations for interval start hour and interval end hour.").arg(sh);
	s+=translatedCommaSpace();

	QString eh;
	if(this->intervalEnd<r.nHoursPerDay)
		eh=tr("M %1", "M means morning. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the morning.").arg(r.hoursOfTheDay[this->intervalEnd]);
	else if(this->intervalEnd<2*r.nHoursPerDay)
		eh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay]);
	else
		eh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(tr("End of the day"));
	s+=tr("IEH: %1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(eh);
	s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxBuildingChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teachers maximum building changes per real day in hourly interval"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	QString sh;
	if(this->intervalStart<r.nHoursPerDay)
		sh=tr("Morning %1", "%1 is an hour of the morning").arg(r.hoursOfTheDay[this->intervalStart]);
	else
		sh=tr("Afternoon %1", "%1 is an hour of the afternoon").arg(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay]);
	s+=tr("Interval start hour=%1").arg(sh);s+="\n";
	
	QString eh;
	if(this->intervalEnd<r.nHoursPerDay)
		eh=tr("Morning %1", "%1 is an hour of the morning").arg(r.hoursOfTheDay[this->intervalEnd]);
	else if(this->intervalEnd<2*r.nHoursPerDay)
		eh=tr("Afternoon %1", "%1 is an hour of the afternoon").arg(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay]);
	else
		eh=tr("End of the real day");
	s+=tr("Interval end hour=%1").arg(eh);s+="\n";

	s+=tr("Maximum building changes per day=%1").arg(this->maxBuildingChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtBuildingsTimetable;
	crtBuildingsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	for(int tch=0; tch<r.nInternalTeachers; tch++){
		//Better, less memory
		Teacher* tchpointer=r.internalTeachersList[tch];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtBuildingsTimetable[d2][h2]=-1;

		for(int ai : std::as_const(tchpointer->activitiesForTeacher))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;

				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtBuildingsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtBuildingsTimetable[d2][h2+dur]=r.internalRoomsList[c.rooms[ai]]->buildingIndex;
					}
				}
			}
		/////////////
		for(int d2=0; d2<r.nDaysPerWeek/2; d2++){
			int crt_building=-1;
			int n_changes=0;
			for(int h2=intervalStart; h2<intervalEnd; h2++){
				int d3=2*d2+h2/r.nHoursPerDay;
				int h3=h2%r.nHoursPerDay;

				if(crtBuildingsTimetable[d3][h3]!=-1){
					if(crt_building!=crtBuildingsTimetable[d3][h3]){
						if(crt_building!=-1)
							n_changes++;
						crt_building=crtBuildingsTimetable[d3][h3];
					}
				}
			}

			if(n_changes>this->maxBuildingChangesPerDay){
				nbroken+=-this->maxBuildingChangesPerDay+n_changes;

				if(conflictsString!=nullptr){
					QString s=tr("Space constraint teachers max building changes per real day in hourly interval broken for teacher=%1 on real day number %2")
						.arg(r.internalTeachersList[tch]->name)
						.arg(d2);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes)));

					dl.append(s);
					cl.append(weightPercentage/100* (-maxBuildingChangesPerDay+n_changes));

					*conflictsString+=s+"\n";
				}
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return true;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::hasWrongDayOrHour(Rules& r)
{
	if(intervalStart>=2*r.nHoursPerDay)
		return true;
	if(intervalEnd>2*r.nHoursPerDay)
		return true;
	if(maxBuildingChangesPerDay>2*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(intervalStart<2*r.nHoursPerDay && intervalEnd<=2*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintTeachersMaxBuildingChangesPerRealDayInInterval::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(intervalStart<2*r.nHoursPerDay && intervalEnd<=2*r.nHoursPerDay);
	
	if(maxBuildingChangesPerDay>2*r.nHoursPerDay)
		maxBuildingChangesPerDay=2*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxRoomChangesPerDayInInterval::ConstraintStudentsSetMaxRoomChangesPerDayInInterval()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL;
}

ConstraintStudentsSetMaxRoomChangesPerDayInInterval::ConstraintStudentsSetMaxRoomChangesPerDayInInterval(double wp, const QString& st, int mc, int is, int ie)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL;
	this->studentsName=st;
	this->maxRoomChangesPerDay=mc;
	
	this->intervalStart=is;
	this->intervalEnd=ie;
}

bool ConstraintStudentsSetMaxRoomChangesPerDayInInterval::computeInternalStructure(QWidget* parent, Rules& r)
{
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
	StudentsSet* ss=r.studentsHash.value(studentsName, nullptr);
	
	if(ss==nullptr){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max room changes per day in hourly interval is wrong because it refers to nonexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}
	
	populateInternalSubgroupsList(r, ss, this->iSubgroupsList);
	/*this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);*/

	return true;
}

bool ConstraintStudentsSetMaxRoomChangesPerDayInInterval::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetMaxRoomChangesPerDayInInterval::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsSetMaxRoomChangesPerDayInInterval>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Students>"+protect(this->studentsName)+"</Students>\n";
	
	s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart])+"</Interval_Start_Hour>\n";
	if(this->intervalEnd < r.nHoursPerDay){
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd])+"</Interval_End_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_End_Hour></Interval_End_Hour>\n";
		s+=IL3+"<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}

	s+=IL3+"<Max_Room_Changes_Per_Day>"+CustomFETString::number(this->maxRoomChangesPerDay)+"</Max_Room_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsSetMaxRoomChangesPerDayInInterval>\n";

	return s;
}

QString ConstraintStudentsSetMaxRoomChangesPerDayInInterval::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set max room changes per day in hourly interval"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=translatedCommaSpace();

	s+=tr("ISH:%1", "Abbreviation for interval start hour. Please use different abbreviations for interval start hour and interval end hour.").arg(r.hoursOfTheDay[this->intervalStart]);s+=translatedCommaSpace();
	if(this->intervalEnd<r.nHoursPerDay)
		s+=tr("IEH:%1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(r.hoursOfTheDay[this->intervalEnd]);
	else
		s+=tr("IEH:%1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(tr("End of the day"));
	s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxRoomChangesPerDayInInterval::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students set maximum room changes per day in hourly interval"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	s+=tr("Interval start hour=%1").arg(r.hoursOfTheDay[this->intervalStart]);s+="\n";
	if(this->intervalEnd<r.nHoursPerDay)
		s+=tr("Interval end hour=%1").arg(r.hoursOfTheDay[this->intervalEnd]);
	else
		s+=tr("Interval end hour=%1").arg(tr("End of the day"));
	s+="\n";

	s+=tr("Maximum room changes per day=%1").arg(this->maxRoomChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetMaxRoomChangesPerDayInInterval::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int sbg : std::as_const(this->iSubgroupsList)){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;
		
		for(int ai : std::as_const(sts->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////
	
		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_room=-1;
			int n_changes=0;
			for(int h2=this->intervalStart; h2<this->intervalEnd; h2++){
				if(crtRoomsTimetable[d2][h2]!=-1){
					if(crt_room!=crtRoomsTimetable[d2][h2]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[d2][h2];
					}
				}
			}
			
			if(n_changes>this->maxRoomChangesPerDay){
				nbroken+=-this->maxRoomChangesPerDay+n_changes;
		
				if(conflictsString!=nullptr){
					QString s=tr("Space constraint students set max room changes per day in hourly interval broken for students=%1 on day %2")
						.arg(this->studentsName)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerDay+n_changes)));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxRoomChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxRoomChangesPerDayInInterval::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerDayInInterval::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerDayInInterval::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerDayInInterval::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerDayInInterval::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(s->name, this->studentsName);
}

bool ConstraintStudentsSetMaxRoomChangesPerDayInInterval::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerDayInInterval::hasWrongDayOrHour(Rules& r)
{
	if(intervalStart>=r.nHoursPerDay)
		return true;
	if(intervalEnd>r.nHoursPerDay)
		return true;
	if(maxRoomChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerDayInInterval::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(intervalStart<r.nHoursPerDay && intervalEnd<=r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerDayInInterval::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(intervalStart<r.nHoursPerDay && intervalEnd<=r.nHoursPerDay);
	
	if(maxRoomChangesPerDay>r.nHoursPerDay)
		maxRoomChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxRoomChangesPerDayInInterval::ConstraintStudentsMaxRoomChangesPerDayInInterval()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL;
}

ConstraintStudentsMaxRoomChangesPerDayInInterval::ConstraintStudentsMaxRoomChangesPerDayInInterval(double wp, int mc, int is, int ie)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL;
	this->maxRoomChangesPerDay=mc;
	
	this->intervalStart=is;
	this->intervalEnd=ie;
}

bool ConstraintStudentsMaxRoomChangesPerDayInInterval::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);
	
	return true;
}

bool ConstraintStudentsMaxRoomChangesPerDayInInterval::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsMaxRoomChangesPerDayInInterval::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsMaxRoomChangesPerDayInInterval>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	
	s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart])+"</Interval_Start_Hour>\n";
	if(this->intervalEnd < r.nHoursPerDay){
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd])+"</Interval_End_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_End_Hour></Interval_End_Hour>\n";
		s+=IL3+"<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}

	s+=IL3+"<Max_Room_Changes_Per_Day>"+CustomFETString::number(this->maxRoomChangesPerDay)+"</Max_Room_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsMaxRoomChangesPerDayInInterval>\n";

	return s;
}

QString ConstraintStudentsMaxRoomChangesPerDayInInterval::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students max room changes per day in hourly interval"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("ISH:%1", "Abbreviation for interval start hour. Please use different abbreviations for interval start hour and interval end hour.").arg(r.hoursOfTheDay[this->intervalStart]);s+=translatedCommaSpace();
	if(this->intervalEnd<r.nHoursPerDay)
		s+=tr("IEH:%1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(r.hoursOfTheDay[this->intervalEnd]);
	else
		s+=tr("IEH:%1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(tr("End of the day"));
	s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsMaxRoomChangesPerDayInInterval::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students maximum room changes per day in hourly interval"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Interval start hour=%1").arg(r.hoursOfTheDay[this->intervalStart]);s+="\n";
	if(this->intervalEnd<r.nHoursPerDay)
		s+=tr("Interval end hour=%1").arg(r.hoursOfTheDay[this->intervalEnd]);
	else
		s+=tr("Interval end hour=%1").arg(tr("End of the day"));
	s+="\n";

	s+=tr("Maximum room changes per day=%1").arg(this->maxRoomChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxRoomChangesPerDayInInterval::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		//Better, less memory
		StudentsSubgroup* sts=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;
		
		for(int ai : std::as_const(sts->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////
	
		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_room=-1;
			int n_changes=0;
			for(int h2=this->intervalStart; h2<this->intervalEnd; h2++){
				if(crtRoomsTimetable[d2][h2]!=-1){
					if(crt_room!=crtRoomsTimetable[d2][h2]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[d2][h2];
					}
				}
			}
			
			if(n_changes>this->maxRoomChangesPerDay){
				nbroken+=-this->maxRoomChangesPerDay+n_changes;
		
				if(conflictsString!=nullptr){
					QString s=tr("Space constraint students max room changes per day in hourly interval broken for subgroup=%1 on day %2")
						.arg(r.internalSubgroupsList[sbg]->name)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerDay+n_changes)));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxRoomChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxRoomChangesPerDayInInterval::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxRoomChangesPerDayInInterval::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsMaxRoomChangesPerDayInInterval::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsMaxRoomChangesPerDayInInterval::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxRoomChangesPerDayInInterval::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return true;
}

bool ConstraintStudentsMaxRoomChangesPerDayInInterval::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsMaxRoomChangesPerDayInInterval::hasWrongDayOrHour(Rules& r)
{
	if(intervalStart>=r.nHoursPerDay)
		return true;
	if(intervalEnd>r.nHoursPerDay)
		return true;
	if(maxRoomChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsMaxRoomChangesPerDayInInterval::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(intervalStart<r.nHoursPerDay && intervalEnd<=r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintStudentsMaxRoomChangesPerDayInInterval::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(intervalStart<r.nHoursPerDay && intervalEnd<=r.nHoursPerDay);
	
	if(maxRoomChangesPerDay>r.nHoursPerDay)
		maxRoomChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxRoomChangesPerDayInInterval::ConstraintTeacherMaxRoomChangesPerDayInInterval()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL;
}

ConstraintTeacherMaxRoomChangesPerDayInInterval::ConstraintTeacherMaxRoomChangesPerDayInInterval(double wp, const QString& tc, int mc, int is, int ie)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL;
	this->teacherName=tc;
	this->maxRoomChangesPerDay=mc;
	
	this->intervalStart=is;
	this->intervalEnd=ie;
}

bool ConstraintTeacherMaxRoomChangesPerDayInInterval::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	
	if(this->teacher_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher max room changes per day in hourly interval is wrong because it refers to nonexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}

	return true;
}

bool ConstraintTeacherMaxRoomChangesPerDayInInterval::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherMaxRoomChangesPerDayInInterval::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeacherMaxRoomChangesPerDayInInterval>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	
	s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart])+"</Interval_Start_Hour>\n";
	if(this->intervalEnd < r.nHoursPerDay){
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd])+"</Interval_End_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_End_Hour></Interval_End_Hour>\n";
		s+=IL3+"<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}

	s+=IL3+"<Max_Room_Changes_Per_Day>"+CustomFETString::number(this->maxRoomChangesPerDay)+"</Max_Room_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeacherMaxRoomChangesPerDayInInterval>\n";

	return s;
}

QString ConstraintTeacherMaxRoomChangesPerDayInInterval::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher max room changes per day in hourly interval"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();

	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=translatedCommaSpace();

	s+=tr("ISH:%1", "Abbreviation for interval start hour. Please use different abbreviations for interval start hour and interval end hour.").arg(r.hoursOfTheDay[this->intervalStart]);s+=translatedCommaSpace();
	if(this->intervalEnd<r.nHoursPerDay)
		s+=tr("IEH:%1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(r.hoursOfTheDay[this->intervalEnd]);
	else
		s+=tr("IEH:%1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(tr("End of the day"));
	s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeacherMaxRoomChangesPerDayInInterval::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teacher maximum room changes per day in hourly interval"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	s+=tr("Interval start hour=%1").arg(r.hoursOfTheDay[this->intervalStart]);s+="\n";
	if(this->intervalEnd<r.nHoursPerDay)
		s+=tr("Interval end hour=%1").arg(r.hoursOfTheDay[this->intervalEnd]);
	else
		s+=tr("Interval end hour=%1").arg(tr("End of the day"));
	s+="\n";

	s+=tr("Maximum room changes per day=%1").arg(this->maxRoomChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxRoomChangesPerDayInInterval::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	int tch=this->teacher_ID;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	//Better, less memory
	Teacher* tchpointer=r.internalTeachersList[tch];
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtRoomsTimetable[d2][h2]=-1;
	
	for(int ai : std::as_const(tchpointer->activitiesForTeacher))
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d2=c.times[ai]%r.nDaysPerWeek;
			int h2=c.times[ai]/r.nDaysPerWeek;
			
			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h2+dur<r.nHoursPerDay);
				assert(crtRoomsTimetable[d2][h2+dur]==-1);
				if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
					assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
					crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
				}
			}
		}
	/////////////
	
	for(int d2=0; d2<r.nDaysPerWeek; d2++){
		int crt_room=-1;
		int n_changes=0;
		for(int h2=this->intervalStart; h2<this->intervalEnd; h2++){
			if(crtRoomsTimetable[d2][h2]!=-1){
				if(crt_room!=crtRoomsTimetable[d2][h2]){
					if(crt_room!=-1)
						n_changes++;
					crt_room=crtRoomsTimetable[d2][h2];
				}
			}
		}
		
		if(n_changes>this->maxRoomChangesPerDay){
			nbroken+=-this->maxRoomChangesPerDay+n_changes;
	
			if(conflictsString!=nullptr){
				QString s=tr("Space constraint teacher max room changes per day in hourly interval broken for teacher=%1 on day %2")
					.arg(this->teacherName)
					.arg(r.daysOfTheWeek[d2]);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerDay+n_changes)));
				
				dl.append(s);
				cl.append(weightPercentage/100* (-maxRoomChangesPerDay+n_changes));
			
				*conflictsString+=s+"\n";
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxRoomChangesPerDayInInterval::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxRoomChangesPerDayInInterval::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMaxRoomChangesPerDayInInterval::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxRoomChangesPerDayInInterval::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxRoomChangesPerDayInInterval::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxRoomChangesPerDayInInterval::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeacherMaxRoomChangesPerDayInInterval::hasWrongDayOrHour(Rules& r)
{
	if(intervalStart>=r.nHoursPerDay)
		return true;
	if(intervalEnd>r.nHoursPerDay)
		return true;
	if(maxRoomChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherMaxRoomChangesPerDayInInterval::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(intervalStart<r.nHoursPerDay && intervalEnd<=r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintTeacherMaxRoomChangesPerDayInInterval::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(intervalStart<r.nHoursPerDay && intervalEnd<=r.nHoursPerDay);
	
	if(maxRoomChangesPerDay>r.nHoursPerDay)
		maxRoomChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxRoomChangesPerDayInInterval::ConstraintTeachersMaxRoomChangesPerDayInInterval()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL;
}

ConstraintTeachersMaxRoomChangesPerDayInInterval::ConstraintTeachersMaxRoomChangesPerDayInInterval(double wp, int mc, int is, int ie)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL;
	this->maxRoomChangesPerDay=mc;
	
	this->intervalStart=is;
	this->intervalEnd=ie;
}

bool ConstraintTeachersMaxRoomChangesPerDayInInterval::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMaxRoomChangesPerDayInInterval::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeachersMaxRoomChangesPerDayInInterval::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeachersMaxRoomChangesPerDayInInterval>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	
	s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart])+"</Interval_Start_Hour>\n";
	if(this->intervalEnd < r.nHoursPerDay){
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd])+"</Interval_End_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_End_Hour></Interval_End_Hour>\n";
		s+=IL3+"<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}

	s+=IL3+"<Max_Room_Changes_Per_Day>"+CustomFETString::number(this->maxRoomChangesPerDay)+"</Max_Room_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeachersMaxRoomChangesPerDayInInterval>\n";

	return s;
}

QString ConstraintTeachersMaxRoomChangesPerDayInInterval::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teachers max room changes per day in hourly interval"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();

	s+=tr("ISH:%1", "Abbreviation for interval start hour. Please use different abbreviations for interval start hour and interval end hour.").arg(r.hoursOfTheDay[this->intervalStart]);s+=translatedCommaSpace();
	if(this->intervalEnd<r.nHoursPerDay)
		s+=tr("IEH:%1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(r.hoursOfTheDay[this->intervalEnd]);
	else
		s+=tr("IEH:%1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(tr("End of the day"));
	s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeachersMaxRoomChangesPerDayInInterval::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teachers maximum room changes per day in hourly interval"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Interval start hour=%1").arg(r.hoursOfTheDay[this->intervalStart]);s+="\n";
	if(this->intervalEnd<r.nHoursPerDay)
		s+=tr("Interval end hour=%1").arg(r.hoursOfTheDay[this->intervalEnd]);
	else
		s+=tr("Interval end hour=%1").arg(tr("End of the day"));
	s+="\n";

	s+=tr("Maximum room changes per day=%1").arg(this->maxRoomChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxRoomChangesPerDayInInterval::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int tch=0; tch<r.nInternalTeachers; tch++){
		//Better, less memory
		Teacher* tchpointer=r.internalTeachersList[tch];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;
		
		for(int ai : std::as_const(tchpointer->activitiesForTeacher))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////
		
		for(int d2=0; d2<r.nDaysPerWeek; d2++){
			int crt_room=-1;
			int n_changes=0;
			for(int h2=this->intervalStart; h2<this->intervalEnd; h2++){
				if(crtRoomsTimetable[d2][h2]!=-1){
					if(crt_room!=crtRoomsTimetable[d2][h2]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[d2][h2];
					}
				}
			}
			
			if(n_changes>this->maxRoomChangesPerDay){
				nbroken+=-this->maxRoomChangesPerDay+n_changes;
			
				if(conflictsString!=nullptr){
					QString s=tr("Space constraint teachers max room changes per day in hourly interval broken for teacher=%1 on day %2")
						.arg(r.internalTeachersList[tch]->name)
						.arg(r.daysOfTheWeek[d2]);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerDay+n_changes)));
					
					dl.append(s);
					cl.append(weightPercentage/100* (-maxRoomChangesPerDay+n_changes));
				
					*conflictsString+=s+"\n";
				}
			}
		}
	}
	
	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxRoomChangesPerDayInInterval::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxRoomChangesPerDayInInterval::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return true;
}

bool ConstraintTeachersMaxRoomChangesPerDayInInterval::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxRoomChangesPerDayInInterval::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxRoomChangesPerDayInInterval::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxRoomChangesPerDayInInterval::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeachersMaxRoomChangesPerDayInInterval::hasWrongDayOrHour(Rules& r)
{
	if(intervalStart>=r.nHoursPerDay)
		return true;
	if(intervalEnd>r.nHoursPerDay)
		return true;
	if(maxRoomChangesPerDay>r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersMaxRoomChangesPerDayInInterval::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(intervalStart<r.nHoursPerDay && intervalEnd<=r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintTeachersMaxRoomChangesPerDayInInterval::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(intervalStart<r.nHoursPerDay && intervalEnd<=r.nHoursPerDay);
	
	if(maxRoomChangesPerDay>r.nHoursPerDay)
		maxRoomChangesPerDay=r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL;
}

ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval(double wp, const QString& st, int mc, int is, int ie)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL;
	this->studentsName=st;
	this->maxRoomChangesPerDay=mc;
	
	this->intervalStart=is;
	this->intervalEnd=ie;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::computeInternalStructure(QWidget* parent, Rules& r)
{
	//StudentsSet* ss=r.searchAugmentedStudentsSet(this->studentsName);
	StudentsSet* ss=r.studentsHash.value(studentsName, nullptr);
	
	if(ss==nullptr){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint students set max room changes per real day in hourly interval is wrong because it refers to nonexistent students set."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}
	
	populateInternalSubgroupsList(r, ss, this->iSubgroupsList);
	/*this->iSubgroupsList.clear();
	if(ss->type==STUDENTS_SUBGROUP){
		int tmp;
		tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
		assert(tmp>=0);
		assert(tmp<r.nInternalSubgroups);
		this->iSubgroupsList.append(tmp);
	}
	else if(ss->type==STUDENTS_GROUP){
		StudentsGroup* stg=(StudentsGroup*)ss;
		for(int i=0; i<stg->subgroupsList.size(); i++){
			StudentsSubgroup* sts=stg->subgroupsList[i];
			int tmp;
			tmp=sts->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			this->iSubgroupsList.append(tmp);
		}
	}
	else if(ss->type==STUDENTS_YEAR){
		StudentsYear* sty=(StudentsYear*)ss;
		for(int i=0; i<sty->groupsList.size(); i++){
			StudentsGroup* stg=sty->groupsList[i];
			for(int j=0; j<stg->subgroupsList.size(); j++){
				StudentsSubgroup* sts=stg->subgroupsList[j];
				int tmp;
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				this->iSubgroupsList.append(tmp);
			}
		}
	}
	else
		assert(0);*/

	return true;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Students>"+protect(this->studentsName)+"</Students>\n";
	
	if(this->intervalStart<r.nHoursPerDay){
		s+=IL3+"<Interval_Start_Day>Morning</Interval_Start_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart])+"</Interval_Start_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_Start_Day>Afternoon</Interval_Start_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay])+"</Interval_Start_Hour>\n";
	}

	if(this->intervalEnd<r.nHoursPerDay){
		s+=IL3+"<Interval_End_Day>Morning</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd])+"</Interval_End_Hour>\n";
	}
	else if(this->intervalEnd<2*r.nHoursPerDay){
		s+=IL3+"<Interval_End_Day>Afternoon</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay])+"</Interval_End_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_End_Day>Afternoon</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour></Interval_End_Hour>\n";
		s+=IL3+"<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}

	s+=IL3+"<Max_Room_Changes_Per_Day>"+CustomFETString::number(this->maxRoomChangesPerDay)+"</Max_Room_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval>\n";

	return s;
}

QString ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students set max room changes per real day in hourly interval"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("St:%1", "St means students").arg(this->studentsName);s+=translatedCommaSpace();

	QString sh;
	if(this->intervalStart<r.nHoursPerDay)
		sh=tr("M %1", "M means morning. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the morning.").arg(r.hoursOfTheDay[this->intervalStart]);
	else
		sh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay]);
	s+=tr("ISH: %1", "Abbreviation for interval start hour. Please use different abbreviations for interval start hour and interval end hour.").arg(sh);
	s+=translatedCommaSpace();

	QString eh;
	if(this->intervalEnd<r.nHoursPerDay)
		eh=tr("M %1", "M means morning. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the morning.").arg(r.hoursOfTheDay[this->intervalEnd]);
	else if(this->intervalEnd<2*r.nHoursPerDay)
		eh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay]);
	else
		eh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(tr("End of the day"));
	s+=tr("IEH: %1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(eh);
	s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students set maximum room changes per real day in hourly interval"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Students=%1").arg(this->studentsName);s+="\n";

	QString sh;
	if(this->intervalStart<r.nHoursPerDay)
		sh=tr("Morning %1", "%1 is an hour of the morning").arg(r.hoursOfTheDay[this->intervalStart]);
	else
		sh=tr("Afternoon %1", "%1 is an hour of the afternoon").arg(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay]);
	s+=tr("Interval start hour=%1").arg(sh);s+="\n";
	
	QString eh;
	if(this->intervalEnd<r.nHoursPerDay)
		eh=tr("Morning %1", "%1 is an hour of the morning").arg(r.hoursOfTheDay[this->intervalEnd]);
	else if(this->intervalEnd<2*r.nHoursPerDay)
		eh=tr("Afternoon %1", "%1 is an hour of the afternoon").arg(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay]);
	else
		eh=tr("End of the real day");
	s+=tr("Interval end hour=%1").arg(eh);s+="\n";

	s+=tr("Maximum room changes per day=%1").arg(this->maxRoomChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	for(int sbg : std::as_const(this->iSubgroupsList)){
		//Better, less memory
		StudentsSubgroup* sbgpointer=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;

		for(int ai : std::as_const(sbgpointer->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;

				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////
		for(int d2=0; d2<r.nDaysPerWeek/2; d2++){
			int crt_room=-1;
			int n_changes=0;
			for(int h2=intervalStart; h2<intervalEnd; h2++){
				int d3=2*d2+h2/r.nHoursPerDay;
				int h3=h2%r.nHoursPerDay;

				if(crtRoomsTimetable[d3][h3]!=-1){
					if(crt_room!=crtRoomsTimetable[d3][h3]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[d3][h3];
					}
				}
			}

			if(n_changes>this->maxRoomChangesPerDay){
				nbroken+=-this->maxRoomChangesPerDay+n_changes;

				if(conflictsString!=nullptr){
					QString s=tr("Space constraint students set max room changes per real day in hourly interval broken for subgroup=%1 on real day number %2")
						.arg(r.internalSubgroupsList[sbg]->name)
						.arg(d2);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerDay+n_changes)));

					dl.append(s);
					cl.append(weightPercentage/100* (-maxRoomChangesPerDay+n_changes));

					*conflictsString+=s+"\n";
				}
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	return r.setsShareStudents(s->name, this->studentsName);
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::hasWrongDayOrHour(Rules& r)
{
	if(intervalStart>=2*r.nHoursPerDay)
		return true;
	if(intervalEnd>2*r.nHoursPerDay)
		return true;
	if(maxRoomChangesPerDay>2*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(intervalStart<2*r.nHoursPerDay && intervalEnd<=2*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(intervalStart<2*r.nHoursPerDay && intervalEnd<=2*r.nHoursPerDay);
	
	if(maxRoomChangesPerDay>2*r.nHoursPerDay)
		maxRoomChangesPerDay=2*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintStudentsMaxRoomChangesPerRealDayInInterval::ConstraintStudentsMaxRoomChangesPerRealDayInInterval()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL;
}

ConstraintStudentsMaxRoomChangesPerRealDayInInterval::ConstraintStudentsMaxRoomChangesPerRealDayInInterval(double wp, int mc, int is, int ie)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL;
	this->maxRoomChangesPerDay=mc;
	
	this->intervalStart=is;
	this->intervalEnd=ie;
}

bool ConstraintStudentsMaxRoomChangesPerRealDayInInterval::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintStudentsMaxRoomChangesPerRealDayInInterval::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintStudentsMaxRoomChangesPerRealDayInInterval::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintStudentsMaxRoomChangesPerRealDayInInterval>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	
	if(this->intervalStart<r.nHoursPerDay){
		s+=IL3+"<Interval_Start_Day>Morning</Interval_Start_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart])+"</Interval_Start_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_Start_Day>Afternoon</Interval_Start_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay])+"</Interval_Start_Hour>\n";
	}

	if(this->intervalEnd<r.nHoursPerDay){
		s+=IL3+"<Interval_End_Day>Morning</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd])+"</Interval_End_Hour>\n";
	}
	else if(this->intervalEnd<2*r.nHoursPerDay){
		s+=IL3+"<Interval_End_Day>Afternoon</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay])+"</Interval_End_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_End_Day>Afternoon</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour></Interval_End_Hour>\n";
		s+=IL3+"<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}

	s+=IL3+"<Max_Room_Changes_Per_Day>"+CustomFETString::number(this->maxRoomChangesPerDay)+"</Max_Room_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintStudentsMaxRoomChangesPerRealDayInInterval>\n";

	return s;
}

QString ConstraintStudentsMaxRoomChangesPerRealDayInInterval::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Students max room changes per real day in hourly interval"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	QString sh;
	if(this->intervalStart<r.nHoursPerDay)
		sh=tr("M %1", "M means morning. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the morning.").arg(r.hoursOfTheDay[this->intervalStart]);
	else
		sh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay]);
	s+=tr("ISH: %1", "Abbreviation for interval start hour. Please use different abbreviations for interval start hour and interval end hour.").arg(sh);
	s+=translatedCommaSpace();

	QString eh;
	if(this->intervalEnd<r.nHoursPerDay)
		eh=tr("M %1", "M means morning. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the morning.").arg(r.hoursOfTheDay[this->intervalEnd]);
	else if(this->intervalEnd<2*r.nHoursPerDay)
		eh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay]);
	else
		eh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(tr("End of the day"));
	s+=tr("IEH: %1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(eh);
	s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerDay);

	return begin+s+end;
}

QString ConstraintStudentsMaxRoomChangesPerRealDayInInterval::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Students maximum room changes per real day in hourly interval"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	QString sh;
	if(this->intervalStart<r.nHoursPerDay)
		sh=tr("Morning %1", "%1 is an hour of the morning").arg(r.hoursOfTheDay[this->intervalStart]);
	else
		sh=tr("Afternoon %1", "%1 is an hour of the afternoon").arg(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay]);
	s+=tr("Interval start hour=%1").arg(sh);s+="\n";
	
	QString eh;
	if(this->intervalEnd<r.nHoursPerDay)
		eh=tr("Morning %1", "%1 is an hour of the morning").arg(r.hoursOfTheDay[this->intervalEnd]);
	else if(this->intervalEnd<2*r.nHoursPerDay)
		eh=tr("Afternoon %1", "%1 is an hour of the afternoon").arg(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay]);
	else
		eh=tr("End of the real day");
	s+=tr("Interval end hour=%1").arg(eh);s+="\n";

	s+=tr("Maximum room changes per day=%1").arg(this->maxRoomChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintStudentsMaxRoomChangesPerRealDayInInterval::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	for(int sbg=0; sbg<r.nInternalSubgroups; sbg++){
		//Better, less memory
		StudentsSubgroup* sbgpointer=r.internalSubgroupsList[sbg];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;

		for(int ai : std::as_const(sbgpointer->activitiesForSubgroup))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;

				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////
		for(int d2=0; d2<r.nDaysPerWeek/2; d2++){
			int crt_room=-1;
			int n_changes=0;
			for(int h2=intervalStart; h2<intervalEnd; h2++){
				int d3=2*d2+h2/r.nHoursPerDay;
				int h3=h2%r.nHoursPerDay;

				if(crtRoomsTimetable[d3][h3]!=-1){
					if(crt_room!=crtRoomsTimetable[d3][h3]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[d3][h3];
					}
				}
			}

			if(n_changes>this->maxRoomChangesPerDay){
				nbroken+=-this->maxRoomChangesPerDay+n_changes;

				if(conflictsString!=nullptr){
					QString s=tr("Space constraint students max room changes per real day in hourly interval broken for subgroup=%1 on real day number %2")
						.arg(r.internalSubgroupsList[sbg]->name)
						.arg(d2);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerDay+n_changes)));

					dl.append(s);
					cl.append(weightPercentage/100* (-maxRoomChangesPerDay+n_changes));

					*conflictsString+=s+"\n";
				}
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintStudentsMaxRoomChangesPerRealDayInInterval::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintStudentsMaxRoomChangesPerRealDayInInterval::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);
	
	return false;
}

bool ConstraintStudentsMaxRoomChangesPerRealDayInInterval::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintStudentsMaxRoomChangesPerRealDayInInterval::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintStudentsMaxRoomChangesPerRealDayInInterval::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return true;
}

bool ConstraintStudentsMaxRoomChangesPerRealDayInInterval::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintStudentsMaxRoomChangesPerRealDayInInterval::hasWrongDayOrHour(Rules& r)
{
	if(intervalStart>=2*r.nHoursPerDay)
		return true;
	if(intervalEnd>2*r.nHoursPerDay)
		return true;
	if(maxRoomChangesPerDay>2*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintStudentsMaxRoomChangesPerRealDayInInterval::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(intervalStart<2*r.nHoursPerDay && intervalEnd<=2*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintStudentsMaxRoomChangesPerRealDayInInterval::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(intervalStart<2*r.nHoursPerDay && intervalEnd<=2*r.nHoursPerDay);
	
	if(maxRoomChangesPerDay>2*r.nHoursPerDay)
		maxRoomChangesPerDay=2*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeacherMaxRoomChangesPerRealDayInInterval::ConstraintTeacherMaxRoomChangesPerRealDayInInterval()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL;
}

ConstraintTeacherMaxRoomChangesPerRealDayInInterval::ConstraintTeacherMaxRoomChangesPerRealDayInInterval(double wp, const QString& tc, int mc, int is, int ie)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL;
	this->teacherName=tc;
	this->maxRoomChangesPerDay=mc;
	
	this->intervalStart=is;
	this->intervalEnd=ie;
}

bool ConstraintTeacherMaxRoomChangesPerRealDayInInterval::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->teacher_ID=r.searchTeacher(this->teacherName);
	teacher_ID=r.teachersHash.value(teacherName, -1);
	
	if(this->teacher_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint teacher max room changes per real day in hourly interval is wrong because it refers to nonexistent teacher."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}

	return true;
}

bool ConstraintTeacherMaxRoomChangesPerRealDayInInterval::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeacherMaxRoomChangesPerRealDayInInterval::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeacherMaxRoomChangesPerRealDayInInterval>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Teacher>"+protect(this->teacherName)+"</Teacher>\n";
	
	if(this->intervalStart<r.nHoursPerDay){
		s+=IL3+"<Interval_Start_Day>Morning</Interval_Start_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart])+"</Interval_Start_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_Start_Day>Afternoon</Interval_Start_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay])+"</Interval_Start_Hour>\n";
	}

	if(this->intervalEnd<r.nHoursPerDay){
		s+=IL3+"<Interval_End_Day>Morning</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd])+"</Interval_End_Hour>\n";
	}
	else if(this->intervalEnd<2*r.nHoursPerDay){
		s+=IL3+"<Interval_End_Day>Afternoon</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay])+"</Interval_End_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_End_Day>Afternoon</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour></Interval_End_Hour>\n";
		s+=IL3+"<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}

	s+=IL3+"<Max_Room_Changes_Per_Day>"+CustomFETString::number(this->maxRoomChangesPerDay)+"</Max_Room_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeacherMaxRoomChangesPerRealDayInInterval>\n";

	return s;
}

QString ConstraintTeacherMaxRoomChangesPerRealDayInInterval::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teacher max room changes per real day in hourly interval"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	s+=tr("T:%1", "T means teacher").arg(this->teacherName);s+=translatedCommaSpace();

	QString sh;
	if(this->intervalStart<r.nHoursPerDay)
		sh=tr("M %1", "M means morning. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the morning.").arg(r.hoursOfTheDay[this->intervalStart]);
	else
		sh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay]);
	s+=tr("ISH: %1", "Abbreviation for interval start hour. Please use different abbreviations for interval start hour and interval end hour.").arg(sh);
	s+=translatedCommaSpace();

	QString eh;
	if(this->intervalEnd<r.nHoursPerDay)
		eh=tr("M %1", "M means morning. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the morning.").arg(r.hoursOfTheDay[this->intervalEnd]);
	else if(this->intervalEnd<2*r.nHoursPerDay)
		eh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay]);
	else
		eh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(tr("End of the day"));
	s+=tr("IEH: %1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(eh);
	s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeacherMaxRoomChangesPerRealDayInInterval::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teacher maximum room changes per real day in hourly interval"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	s+=tr("Teacher=%1").arg(this->teacherName);s+="\n";

	QString sh;
	if(this->intervalStart<r.nHoursPerDay)
		sh=tr("Morning %1", "%1 is an hour of the morning").arg(r.hoursOfTheDay[this->intervalStart]);
	else
		sh=tr("Afternoon %1", "%1 is an hour of the afternoon").arg(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay]);
	s+=tr("Interval start hour=%1").arg(sh);s+="\n";
	
	QString eh;
	if(this->intervalEnd<r.nHoursPerDay)
		eh=tr("Morning %1", "%1 is an hour of the morning").arg(r.hoursOfTheDay[this->intervalEnd]);
	else if(this->intervalEnd<2*r.nHoursPerDay)
		eh=tr("Afternoon %1", "%1 is an hour of the afternoon").arg(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay]);
	else
		eh=tr("End of the real day");
	s+=tr("Interval end hour=%1").arg(eh);s+="\n";

	s+=tr("Maximum room changes per day=%1").arg(this->maxRoomChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeacherMaxRoomChangesPerRealDayInInterval::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;
	
	int tch=this->teacher_ID;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);

	//Better, less memory
	Teacher* tchpointer=r.internalTeachersList[tch];
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtRoomsTimetable[d2][h2]=-1;

	for(int ai : std::as_const(tchpointer->activitiesForTeacher))
		if(c.times[ai]!=UNALLOCATED_TIME){
			int d2=c.times[ai]%r.nDaysPerWeek;
			int h2=c.times[ai]/r.nDaysPerWeek;

			for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
				assert(h2+dur<r.nHoursPerDay);
				assert(crtRoomsTimetable[d2][h2+dur]==-1);
				if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
					assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
					crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
				}
			}
		}
	/////////////
	for(int d2=0; d2<r.nDaysPerWeek/2; d2++){
		int crt_room=-1;
		int n_changes=0;
		for(int h2=intervalStart; h2<intervalEnd; h2++){
			int d3=2*d2+h2/r.nHoursPerDay;
			int h3=h2%r.nHoursPerDay;

			if(crtRoomsTimetable[d3][h3]!=-1){
				if(crt_room!=crtRoomsTimetable[d3][h3]){
					if(crt_room!=-1)
						n_changes++;
					crt_room=crtRoomsTimetable[d3][h3];
				}
			}
		}

		if(n_changes>this->maxRoomChangesPerDay){
			nbroken+=-this->maxRoomChangesPerDay+n_changes;

			if(conflictsString!=nullptr){
				QString s=tr("Space constraint teacher max room changes per real day in hourly interval broken for teacher=%1 on real day number %2")
					.arg(this->teacherName)
					.arg(d2);
				s += ". ";
				s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerDay+n_changes)));

				dl.append(s);
				cl.append(weightPercentage/100* (-maxRoomChangesPerDay+n_changes));

				*conflictsString+=s+"\n";
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeacherMaxRoomChangesPerRealDayInInterval::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeacherMaxRoomChangesPerRealDayInInterval::isRelatedToTeacher(Teacher* t)
{
	return this->teacherName==t->name;
}

bool ConstraintTeacherMaxRoomChangesPerRealDayInInterval::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxRoomChangesPerRealDayInInterval::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeacherMaxRoomChangesPerRealDayInInterval::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeacherMaxRoomChangesPerRealDayInInterval::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeacherMaxRoomChangesPerRealDayInInterval::hasWrongDayOrHour(Rules& r)
{
	if(intervalStart>=2*r.nHoursPerDay)
		return true;
	if(intervalEnd>2*r.nHoursPerDay)
		return true;
	if(maxRoomChangesPerDay>2*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeacherMaxRoomChangesPerRealDayInInterval::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(intervalStart<2*r.nHoursPerDay && intervalEnd<=2*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintTeacherMaxRoomChangesPerRealDayInInterval::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(intervalStart<2*r.nHoursPerDay && intervalEnd<=2*r.nHoursPerDay);
	
	if(maxRoomChangesPerDay>2*r.nHoursPerDay)
		maxRoomChangesPerDay=2*r.nHoursPerDay;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

ConstraintTeachersMaxRoomChangesPerRealDayInInterval::ConstraintTeachersMaxRoomChangesPerRealDayInInterval()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL;
}

ConstraintTeachersMaxRoomChangesPerRealDayInInterval::ConstraintTeachersMaxRoomChangesPerRealDayInInterval(double wp, int mc, int is, int ie)
	: SpaceConstraint(wp)
{
	this->type=CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL;
	this->maxRoomChangesPerDay=mc;
	
	this->intervalStart=is;
	this->intervalEnd=ie;
}

bool ConstraintTeachersMaxRoomChangesPerRealDayInInterval::computeInternalStructure(QWidget* parent, Rules& r)
{
	Q_UNUSED(parent);
	Q_UNUSED(r);

	return true;
}

bool ConstraintTeachersMaxRoomChangesPerRealDayInInterval::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	
	return false;
}

QString ConstraintTeachersMaxRoomChangesPerRealDayInInterval::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintTeachersMaxRoomChangesPerRealDayInInterval>\n";

	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(weightPercentage)+"</Weight_Percentage>\n";
	
	if(this->intervalStart<r.nHoursPerDay){
		s+=IL3+"<Interval_Start_Day>Morning</Interval_Start_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart])+"</Interval_Start_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_Start_Day>Afternoon</Interval_Start_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_Start_Hour>"+protect(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay])+"</Interval_Start_Hour>\n";
	}

	if(this->intervalEnd<r.nHoursPerDay){
		s+=IL3+"<Interval_End_Day>Morning</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd])+"</Interval_End_Hour>\n";
	}
	else if(this->intervalEnd<2*r.nHoursPerDay){
		s+=IL3+"<Interval_End_Day>Afternoon</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour>"+protect(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay])+"</Interval_End_Hour>\n";
	}
	else{
		s+=IL3+"<Interval_End_Day>Afternoon</Interval_End_Day>\n";
		s+=IL3+"<!-- Morning or Afternoon -->\n";
		s+=IL3+"<Interval_End_Hour></Interval_End_Hour>\n";
		s+=IL3+"<!-- Interval_End_Hour void means the end of the day (which has no name) -->\n";
	}

	s+=IL3+"<Max_Room_Changes_Per_Day>"+CustomFETString::number(this->maxRoomChangesPerDay)+"</Max_Room_Changes_Per_Day>\n";
	
	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintTeachersMaxRoomChangesPerRealDayInInterval>\n";

	return s;
}

QString ConstraintTeachersMaxRoomChangesPerRealDayInInterval::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Teachers max room changes per real day in hourly interval"); s+=translatedCommaSpace();

	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	
	QString sh;
	if(this->intervalStart<r.nHoursPerDay)
		sh=tr("M %1", "M means morning. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the morning.").arg(r.hoursOfTheDay[this->intervalStart]);
	else
		sh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay]);
	s+=tr("ISH: %1", "Abbreviation for interval start hour. Please use different abbreviations for interval start hour and interval end hour.").arg(sh);
	s+=translatedCommaSpace();

	QString eh;
	if(this->intervalEnd<r.nHoursPerDay)
		eh=tr("M %1", "M means morning. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the morning.").arg(r.hoursOfTheDay[this->intervalEnd]);
	else if(this->intervalEnd<2*r.nHoursPerDay)
		eh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay]);
	else
		eh=tr("A %1", "A means afternoon. Please use different abbreviations for Morning and Afternoon. %1 is an hour of the afternoon.").arg(tr("End of the day"));
	s+=tr("IEH: %1", "Abbreviation for interval end hour. Please use different abbreviations for interval start hour and interval end hour.").arg(eh);
	s+=translatedCommaSpace();

	s+=tr("MC:%1", "MC means max changes").arg(this->maxRoomChangesPerDay);

	return begin+s+end;
}

QString ConstraintTeachersMaxRoomChangesPerRealDayInInterval::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=tr("Space constraint"); s+="\n";

	s+=tr("Teachers maximum room changes per real day in hourly interval"); s+="\n";

	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";

	QString sh;
	if(this->intervalStart<r.nHoursPerDay)
		sh=tr("Morning %1", "%1 is an hour of the morning").arg(r.hoursOfTheDay[this->intervalStart]);
	else
		sh=tr("Afternoon %1", "%1 is an hour of the afternoon").arg(r.hoursOfTheDay[this->intervalStart-r.nHoursPerDay]);
	s+=tr("Interval start hour=%1").arg(sh);s+="\n";
	
	QString eh;
	if(this->intervalEnd<r.nHoursPerDay)
		eh=tr("Morning %1", "%1 is an hour of the morning").arg(r.hoursOfTheDay[this->intervalEnd]);
	else if(this->intervalEnd<2*r.nHoursPerDay)
		eh=tr("Afternoon %1", "%1 is an hour of the afternoon").arg(r.hoursOfTheDay[this->intervalEnd-r.nHoursPerDay]);
	else
		eh=tr("End of the real day");
	s+=tr("Interval end hour=%1").arg(eh);s+="\n";

	s+=tr("Maximum room changes per day=%1").arg(this->maxRoomChangesPerDay);s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintTeachersMaxRoomChangesPerRealDayInInterval::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetable;
	crtRoomsTimetable.resize(r.nDaysPerWeek, r.nHoursPerDay);
	for(int tch=0; tch<r.nInternalTeachers; tch++){
		//Better, less memory
		Teacher* tchpointer=r.internalTeachersList[tch];
		for(int d2=0; d2<r.nDaysPerWeek; d2++)
			for(int h2=0; h2<r.nHoursPerDay; h2++)
				crtRoomsTimetable[d2][h2]=-1;

		for(int ai : std::as_const(tchpointer->activitiesForTeacher))
			if(c.times[ai]!=UNALLOCATED_TIME){
				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;

				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetable[d2][h2+dur]==-1);
					if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
						assert(c.rooms[ai]>=0 && c.rooms[ai]<r.nInternalRooms);
						crtRoomsTimetable[d2][h2+dur]=c.rooms[ai];
					}
				}
			}
		/////////////
		for(int d2=0; d2<r.nDaysPerWeek/2; d2++){
			int crt_room=-1;
			int n_changes=0;
			for(int h2=intervalStart; h2<intervalEnd; h2++){
				int d3=2*d2+h2/r.nHoursPerDay;
				int h3=h2%r.nHoursPerDay;

				if(crtRoomsTimetable[d3][h3]!=-1){
					if(crt_room!=crtRoomsTimetable[d3][h3]){
						if(crt_room!=-1)
							n_changes++;
						crt_room=crtRoomsTimetable[d3][h3];
					}
				}
			}

			if(n_changes>this->maxRoomChangesPerDay){
				nbroken+=-this->maxRoomChangesPerDay+n_changes;

				if(conflictsString!=nullptr){
					QString s=tr("Space constraint teachers max room changes per real day in hourly interval broken for teacher=%1 on real day number %2")
						.arg(r.internalTeachersList[tch]->name)
						.arg(d2);
					s += ". ";
					s += tr("This increases the conflicts total by %1").arg(CustomFETString::numberPlusTwoDigitsPrecision(weightPercentage/100* (-maxRoomChangesPerDay+n_changes)));

					dl.append(s);
					cl.append(weightPercentage/100* (-maxRoomChangesPerDay+n_changes));

					*conflictsString+=s+"\n";
				}
			}
		}
	}

	if(this->weightPercentage==100)
		assert(nbroken==0);

	return weightPercentage/100 * nbroken;
}

bool ConstraintTeachersMaxRoomChangesPerRealDayInInterval::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintTeachersMaxRoomChangesPerRealDayInInterval::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return true;
}

bool ConstraintTeachersMaxRoomChangesPerRealDayInInterval::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxRoomChangesPerRealDayInInterval::isRelatedToActivityTag(ActivityTag* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintTeachersMaxRoomChangesPerRealDayInInterval::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);
	
	return false;
}

bool ConstraintTeachersMaxRoomChangesPerRealDayInInterval::isRelatedToRoom(Room* r)
{
	Q_UNUSED(r);
	
	return false;
}

bool ConstraintTeachersMaxRoomChangesPerRealDayInInterval::hasWrongDayOrHour(Rules& r)
{
	if(intervalStart>=2*r.nHoursPerDay)
		return true;
	if(intervalEnd>2*r.nHoursPerDay)
		return true;
	if(maxRoomChangesPerDay>2*r.nHoursPerDay)
		return true;
	
	return false;
}

bool ConstraintTeachersMaxRoomChangesPerRealDayInInterval::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	if(intervalStart<2*r.nHoursPerDay && intervalEnd<=2*r.nHoursPerDay)
		return true;

	return false;
}

bool ConstraintTeachersMaxRoomChangesPerRealDayInInterval::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));
	
	assert(intervalStart<2*r.nHoursPerDay && intervalEnd<=2*r.nHoursPerDay);
	
	if(maxRoomChangesPerDay>2*r.nHoursPerDay)
		maxRoomChangesPerDay=2*r.nHoursPerDay;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintRoomMaxActivityTagsPerDayFromSet::ConstraintRoomMaxActivityTagsPerDayFromSet()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET;
}

ConstraintRoomMaxActivityTagsPerDayFromSet::ConstraintRoomMaxActivityTagsPerDayFromSet(double wp, const QString& rn, int mtg, const QList<QString>& tgl)
	 : SpaceConstraint(wp)
{
	this->room = rn;
	this->type=CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET;
	
	this->maxTags=mtg;
	this->tagsList=tgl;
}

bool ConstraintRoomMaxActivityTagsPerDayFromSet::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->room_ID=r.searchRoom(this->room);
	room_ID=r.roomsHash.value(room, -1);
	
	if(this->room_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint room max activity tags per day from a set is wrong because it refers to a nonexistent room."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}
	
	internalTagsSet.clear();
	for(const QString& at : tagsList){
		int tgi=r.activityTagsHash.value(at, -1);
		
		if(tgi==-1){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
			 tr("Activity tag %1 is not existing in the following constraint. Please edit or remove the constraint. Constraint is:\n%2").arg(at).arg(this->getDetailedDescription(r)));
			
			return false;
		}
		
		assert(tgi>=0);
		internalTagsSet.insert(tgi);
	}
	
	return true;
}

bool ConstraintRoomMaxActivityTagsPerDayFromSet::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintRoomMaxActivityTagsPerDayFromSet::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintRoomMaxActivityTagsPerDayFromSet>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Room>"+protect(this->room)+"</Room>\n";

	s+=IL3+"<Maximum_Allowed_Activity_Tags>"+QString::number(maxTags)+"</Maximum_Allowed_Activity_Tags>\n";
	s+=IL3+"<Number_of_Activity_Tags>"+QString::number(tagsList.count())+"</Number_of_Activity_Tags>\n";
	for(const QString& atn : std::as_const(tagsList))
		s+=IL3+"<Activity_Tag>"+protect(atn)+"</Activity_Tag>\n";

	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintRoomMaxActivityTagsPerDayFromSet>\n";
	return s;
}

QString ConstraintRoomMaxActivityTagsPerDayFromSet::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Room max activity tags per day from a set");s+=translatedCommaSpace();
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	s+=tr("R:%1", "Room").arg(this->room);s+=translatedCommaSpace();
	s+=tr("MT:%1", "Max number of tags").arg(maxTags);s+=translatedCommaSpace();
	s+=tr("SAt:%1", "Set of activity tags").arg(tagsList.join(translatedCommaSpace()));

	return begin+s+end;
}

QString ConstraintRoomMaxActivityTagsPerDayFromSet::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Space constraint");s+="\n";
	s+=tr("A room must respect a maximum number of activity tags per day from a set");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Room=%1").arg(this->room);s+="\n";

	s+=tr("Maximum number of activity tags=%1").arg(maxTags);s+="\n";
	s+=tr("Set of activity tags=%1").arg(tagsList.join(translatedCommaSpace()));s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintRoomMaxActivityTagsPerDayFromSet::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	Q_UNUSED(cl);
	Q_UNUSED(dl);
	Q_UNUSED(conflictsString);

	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetableActivityTag;
	crtRoomsTimetableActivityTag.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtRoomsTimetableActivityTag[d2][h2]=-1;

	for(int ai=0; ai<r.nInternalActivities; ai++){
		if(c.times[ai]!=UNALLOCATED_TIME){
			//if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
			if(c.rooms[ai]==this->room_ID){
				QSet<int> ts=r.internalActivitiesList[ai].iActivityTagsSet;
				ts.intersect(this->internalTagsSet);
				assert(ts.count()<=1);
				int at=-1;
				if(!ts.isEmpty())
					at=*ts.constBegin();

				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetableActivityTag[d2][h2+dur]==-1);
					crtRoomsTimetableActivityTag[d2][h2+dur]=at;
				}
			}
		}
	}
	/////////////

	for(int d=0; d<r.nDaysPerWeek; d++){
		QSet<int> usedTags;
		for(int h=0; h<r.nHoursPerDay; h++)
			if(crtRoomsTimetableActivityTag[d][h]>=0)
				usedTags.insert(crtRoomsTimetableActivityTag[d][h]);

		if(usedTags.count() > this->maxTags)
			nbroken++;
	}

	assert(weightPercentage==100);

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintRoomMaxActivityTagsPerDayFromSet::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintRoomMaxActivityTagsPerDayFromSet::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintRoomMaxActivityTagsPerDayFromSet::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintRoomMaxActivityTagsPerDayFromSet::isRelatedToActivityTag(ActivityTag* s)
{
	return tagsList.contains(s->name);
}

bool ConstraintRoomMaxActivityTagsPerDayFromSet::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintRoomMaxActivityTagsPerDayFromSet::isRelatedToRoom(Room* r)
{
	return this->room==r->name;
}

bool ConstraintRoomMaxActivityTagsPerDayFromSet::hasWrongDayOrHour(Rules& r)
{
	return this->maxTags > r.nHoursPerDay;
}

bool ConstraintRoomMaxActivityTagsPerDayFromSet::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintRoomMaxActivityTagsPerDayFromSet::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	if(this->maxTags > r.nHoursPerDay)
		this->maxTags=r.nHoursPerDay;

	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintRoomMaxActivityTagsPerRealDayFromSet::ConstraintRoomMaxActivityTagsPerRealDayFromSet()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET;
}

ConstraintRoomMaxActivityTagsPerRealDayFromSet::ConstraintRoomMaxActivityTagsPerRealDayFromSet(double wp, const QString& rn, int mtg, const QList<QString>& tgl)
	 : SpaceConstraint(wp)
{
	this->room = rn;
	this->type=CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET;
	
	this->maxTags=mtg;
	this->tagsList=tgl;
}

bool ConstraintRoomMaxActivityTagsPerRealDayFromSet::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->room_ID=r.searchRoom(this->room);
	room_ID=r.roomsHash.value(room, -1);
	
	if(this->room_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint room max activity tags per real day from a set is wrong because it refers to a nonexistent room."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}
	
	internalTagsSet.clear();
	for(const QString& at : tagsList){
		int tgi=r.activityTagsHash.value(at, -1);
		
		if(tgi==-1){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
			 tr("Activity tag %1 is not existing in the following constraint. Please edit or remove the constraint. Constraint is:\n%2").arg(at).arg(this->getDetailedDescription(r)));
			
			return false;
		}
		
		assert(tgi>=0);
		internalTagsSet.insert(tgi);
	}
	
	return true;
}

bool ConstraintRoomMaxActivityTagsPerRealDayFromSet::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintRoomMaxActivityTagsPerRealDayFromSet::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintRoomMaxActivityTagsPerRealDayFromSet>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Room>"+protect(this->room)+"</Room>\n";

	s+=IL3+"<Maximum_Allowed_Activity_Tags>"+QString::number(maxTags)+"</Maximum_Allowed_Activity_Tags>\n";
	s+=IL3+"<Number_of_Activity_Tags>"+QString::number(tagsList.count())+"</Number_of_Activity_Tags>\n";
	for(const QString& atn : std::as_const(tagsList))
		s+=IL3+"<Activity_Tag>"+protect(atn)+"</Activity_Tag>\n";

	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintRoomMaxActivityTagsPerRealDayFromSet>\n";
	return s;
}

QString ConstraintRoomMaxActivityTagsPerRealDayFromSet::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Room max activity tags per real day from a set");s+=translatedCommaSpace();
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	s+=tr("R:%1", "Room").arg(this->room);s+=translatedCommaSpace();
	s+=tr("MT:%1", "Max number of tags").arg(maxTags);s+=translatedCommaSpace();
	s+=tr("SAt:%1", "Set of activity tags").arg(tagsList.join(translatedCommaSpace()));

	return begin+s+end;
}

QString ConstraintRoomMaxActivityTagsPerRealDayFromSet::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Space constraint");s+="\n";
	s+=tr("A room must respect a maximum number of activity tags per real day from a set");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Room=%1").arg(this->room);s+="\n";

	s+=tr("Maximum number of activity tags=%1").arg(maxTags);s+="\n";
	s+=tr("Set of activity tags=%1").arg(tagsList.join(translatedCommaSpace()));s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintRoomMaxActivityTagsPerRealDayFromSet::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	Q_UNUSED(cl);
	Q_UNUSED(dl);
	Q_UNUSED(conflictsString);

	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetableActivityTag;
	crtRoomsTimetableActivityTag.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtRoomsTimetableActivityTag[d2][h2]=-1;

	for(int ai=0; ai<r.nInternalActivities; ai++){
		if(c.times[ai]!=UNALLOCATED_TIME){
			//if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
			if(c.rooms[ai]==this->room_ID){
				QSet<int> ts=r.internalActivitiesList[ai].iActivityTagsSet;
				ts.intersect(this->internalTagsSet);
				assert(ts.count()<=1);
				int at=-1;
				if(!ts.isEmpty())
					at=*ts.constBegin();

				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetableActivityTag[d2][h2+dur]==-1);
					crtRoomsTimetableActivityTag[d2][h2+dur]=at;
				}
			}
		}
	}
	/////////////

	for(int d=0; d<r.nDaysPerWeek/2; d++){
		QSet<int> usedTags;
		for(int h=0; h<r.nHoursPerDay; h++){
			if(crtRoomsTimetableActivityTag[2*d][h]>=0)
				usedTags.insert(crtRoomsTimetableActivityTag[2*d][h]);
			if(crtRoomsTimetableActivityTag[2*d+1][h]>=0)
				usedTags.insert(crtRoomsTimetableActivityTag[2*d+1][h]);
		}

		if(usedTags.count() > this->maxTags)
			nbroken++;
	}

	assert(weightPercentage==100);

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintRoomMaxActivityTagsPerRealDayFromSet::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintRoomMaxActivityTagsPerRealDayFromSet::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintRoomMaxActivityTagsPerRealDayFromSet::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintRoomMaxActivityTagsPerRealDayFromSet::isRelatedToActivityTag(ActivityTag* s)
{
	return tagsList.contains(s->name);
}

bool ConstraintRoomMaxActivityTagsPerRealDayFromSet::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintRoomMaxActivityTagsPerRealDayFromSet::isRelatedToRoom(Room* r)
{
	return this->room==r->name;
}

bool ConstraintRoomMaxActivityTagsPerRealDayFromSet::hasWrongDayOrHour(Rules& r)
{
	return this->maxTags > 2*r.nHoursPerDay;
}

bool ConstraintRoomMaxActivityTagsPerRealDayFromSet::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintRoomMaxActivityTagsPerRealDayFromSet::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	if(this->maxTags > 2*r.nHoursPerDay)
		this->maxTags = 2*r.nHoursPerDay;

	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

ConstraintRoomMaxActivityTagsPerWeekFromSet::ConstraintRoomMaxActivityTagsPerWeekFromSet()
	: SpaceConstraint()
{
	this->type=CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_WEEK_FROM_SET;
}

ConstraintRoomMaxActivityTagsPerWeekFromSet::ConstraintRoomMaxActivityTagsPerWeekFromSet(double wp, const QString& rn, int mtg, const QList<QString>& tgl)
	 : SpaceConstraint(wp)
{
	this->room = rn;
	this->type=CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_WEEK_FROM_SET;
	
	this->maxTags=mtg;
	this->tagsList=tgl;
}

bool ConstraintRoomMaxActivityTagsPerWeekFromSet::computeInternalStructure(QWidget* parent, Rules& r)
{
	//this->room_ID=r.searchRoom(this->room);
	room_ID=r.roomsHash.value(room, -1);
	
	if(this->room_ID<0){
		SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
		 tr("Constraint room max activity tags per week from a set is wrong because it refers to a nonexistent room."
		 " Please correct it (removing it might be a solution). Please report potential bug. Constraint is:\n%1").arg(this->getDetailedDescription(r)));
		
		return false;
	}
	
	internalTagsSet.clear();
	for(const QString& at : tagsList){
		int tgi=r.activityTagsHash.value(at, -1);
		
		if(tgi==-1){
			SpaceConstraintIrreconcilableMessage::warning(parent, tr("FET warning"),
			 tr("Activity tag %1 is not existing in the following constraint. Please edit or remove the constraint. Constraint is:\n%2").arg(at).arg(this->getDetailedDescription(r)));
			
			return false;
		}
		
		assert(tgi>=0);
		internalTagsSet.insert(tgi);
	}
	
	return true;
}

bool ConstraintRoomMaxActivityTagsPerWeekFromSet::hasInactiveActivities(Rules& r)
{
	Q_UNUSED(r);
	return false;
}

QString ConstraintRoomMaxActivityTagsPerWeekFromSet::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s=IL2+"<ConstraintRoomMaxActivityTagsPerWeekFromSet>\n";
	s+=IL3+"<Weight_Percentage>"+CustomFETString::number(this->weightPercentage)+"</Weight_Percentage>\n";
	s+=IL3+"<Room>"+protect(this->room)+"</Room>\n";

	s+=IL3+"<Maximum_Allowed_Activity_Tags>"+QString::number(maxTags)+"</Maximum_Allowed_Activity_Tags>\n";
	s+=IL3+"<Number_of_Activity_Tags>"+QString::number(tagsList.count())+"</Number_of_Activity_Tags>\n";
	for(const QString& atn : std::as_const(tagsList))
		s+=IL3+"<Activity_Tag>"+protect(atn)+"</Activity_Tag>\n";

	s+=IL3+"<Active>"+trueFalse(active)+"</Active>\n";
	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";
	s+=IL2+"</ConstraintRoomMaxActivityTagsPerWeekFromSet>\n";
	return s;
}

QString ConstraintRoomMaxActivityTagsPerWeekFromSet::getDescription(Rules& r){
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	QString s=tr("Room max activity tags per week from a set");s+=translatedCommaSpace();
	s+=tr("WP:%1%", "Weight percentage").arg(CustomFETString::number(this->weightPercentage));s+=translatedCommaSpace();
	s+=tr("R:%1", "Room").arg(this->room);s+=translatedCommaSpace();
	s+=tr("MT:%1", "Max number of tags").arg(maxTags);s+=translatedCommaSpace();
	s+=tr("SAt:%1", "Set of activity tags").arg(tagsList.join(translatedCommaSpace()));

	return begin+s+end;
}

QString ConstraintRoomMaxActivityTagsPerWeekFromSet::getDetailedDescription(Rules& r){
	Q_UNUSED(r);

	QString s=tr("Space constraint");s+="\n";
	s+=tr("A room must respect a maximum number of activity tags per week from a set");s+="\n";
	s+=tr("Weight (percentage)=%1%").arg(CustomFETString::number(this->weightPercentage));s+="\n";
	s+=tr("Room=%1").arg(this->room);s+="\n";

	s+=tr("Maximum number of activity tags=%1").arg(maxTags);s+="\n";
	s+=tr("Set of activity tags=%1").arg(tagsList.join(translatedCommaSpace()));s+="\n";

	if(!active){
		s+=tr("Active space constraint=%1", "Represents a yes/no value, if a space constraint is active or not, %1 is yes or no").arg(yesNoTranslated(active));
		s+="\n";
	}
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

double ConstraintRoomMaxActivityTagsPerWeekFromSet::fitness(
	Solution& c,
	Rules& r,
	QList<double>& cl,
	QList<QString>& dl,
	FakeString* conflictsString)
{
	Q_UNUSED(cl);
	Q_UNUSED(dl);
	Q_UNUSED(conflictsString);

	//if the matrix roomsMatrix is already calculated, do not calculate it again!
	if(!c.roomsMatrixReady){
		c.roomsMatrixReady=true;
		rooms_conflicts = c.getRoomsMatrix(r, roomsMatrix);

		c.changedForMatrixCalculation=false;
	}

	int nbroken=0;

	Matrix2D<int> crtRoomsTimetableActivityTag;
	crtRoomsTimetableActivityTag.resize(r.nDaysPerWeek, r.nHoursPerDay);
	
	for(int d2=0; d2<r.nDaysPerWeek; d2++)
		for(int h2=0; h2<r.nHoursPerDay; h2++)
			crtRoomsTimetableActivityTag[d2][h2]=-1;

	for(int ai=0; ai<r.nInternalActivities; ai++){
		if(c.times[ai]!=UNALLOCATED_TIME){
			//if(c.rooms[ai]!=UNSPECIFIED_ROOM && c.rooms[ai]!=UNALLOCATED_SPACE){
			if(c.rooms[ai]==this->room_ID){
				QSet<int> ts=r.internalActivitiesList[ai].iActivityTagsSet;
				ts.intersect(this->internalTagsSet);
				assert(ts.count()<=1);
				int at=-1;
				if(!ts.isEmpty())
					at=*ts.constBegin();

				int d2=c.times[ai]%r.nDaysPerWeek;
				int h2=c.times[ai]/r.nDaysPerWeek;
				
				for(int dur=0; dur<r.internalActivitiesList[ai].duration; dur++){
					assert(h2+dur<r.nHoursPerDay);
					assert(crtRoomsTimetableActivityTag[d2][h2+dur]==-1);
					crtRoomsTimetableActivityTag[d2][h2+dur]=at;
				}
			}
		}
	}
	/////////////

	QSet<int> usedTags;
	for(int d=0; d<r.nDaysPerWeek; d++)
		for(int h=0; h<r.nHoursPerDay; h++)
			if(crtRoomsTimetableActivityTag[d][h]>=0)
				usedTags.insert(crtRoomsTimetableActivityTag[d][h]);

	if(usedTags.count() > this->maxTags)
		nbroken++;

	assert(weightPercentage==100);

	if(weightPercentage==100)
		assert(nbroken==0);
	return weightPercentage/100 * nbroken;
}

bool ConstraintRoomMaxActivityTagsPerWeekFromSet::isRelatedToActivity(Activity* a)
{
	Q_UNUSED(a);

	return false;
}

bool ConstraintRoomMaxActivityTagsPerWeekFromSet::isRelatedToTeacher(Teacher* t)
{
	Q_UNUSED(t);

	return false;
}

bool ConstraintRoomMaxActivityTagsPerWeekFromSet::isRelatedToSubject(Subject* s)
{
	Q_UNUSED(s);

	return false;
}

bool ConstraintRoomMaxActivityTagsPerWeekFromSet::isRelatedToActivityTag(ActivityTag* s)
{
	return tagsList.contains(s->name);
}

bool ConstraintRoomMaxActivityTagsPerWeekFromSet::isRelatedToStudentsSet(Rules& r, StudentsSet* s)
{
	Q_UNUSED(r);
	Q_UNUSED(s);

	return false;
}

bool ConstraintRoomMaxActivityTagsPerWeekFromSet::isRelatedToRoom(Room* r)
{
	return this->room==r->name;
}

bool ConstraintRoomMaxActivityTagsPerWeekFromSet::hasWrongDayOrHour(Rules& r)
{
	return this->maxTags > r.nDaysPerWeek*r.nHoursPerDay;
}

bool ConstraintRoomMaxActivityTagsPerWeekFromSet::canRepairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	return true;
}

bool ConstraintRoomMaxActivityTagsPerWeekFromSet::repairWrongDayOrHour(Rules& r)
{
	assert(hasWrongDayOrHour(r));

	if(this->maxTags > r.nDaysPerWeek*r.nHoursPerDay)
		this->maxTags = r.nDaysPerWeek*r.nHoursPerDay;

	r.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&r);

	return true;
}
