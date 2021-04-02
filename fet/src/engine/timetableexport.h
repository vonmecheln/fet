/*
File timetableexport.h
*/

/***************************************************************************
                          timetableexport.h  -  description
                             -------------------
    begin                : Tue Apr 22, 2003
    copyright            : (C) 2003 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TIMETABLEEXPORT_H
#define TIMETABLEEXPORT_H

#include "timetable_defs.h"
#include "timetable.h"

#include <QString>

class QWidget;

class TimetableExport: public QObject{
	Q_OBJECT

public:
	TimetableExport();
	~TimetableExport();

	static void getStudentsTimetable(Solution& c);
	static void getTeachersTimetable(Solution& c);
	static void getRoomsTimetable(Solution& c);

	static void writeSimulationResults(QWidget* parent);
	static void writeHighestStageResults(QWidget* parent);
	static void writeSimulationResults(QWidget* parent, int n); //write in a directory with number n (for multiple generation)
	static void writeSimulationResultsCommandLine(QWidget* parent, const QString& outputDirectory);
	
	static void writeRandomSeed(QWidget* parent);
	static void writeRandomSeed(QWidget* parent, int n); //write in a directory with number n (for multiple generation)
	static void writeRandomSeedCommandLine(QWidget* parent, const QString& outputDirectory);
	static void writeRandomSeedFile(QWidget* parent, const QString& filename);
	
	static void writeTimetableDataFile(QWidget* parent, const QString& filename);

private:
	//this function must be called before export html files, because it compute the IDs
	static void computeHashForIDsTimetable();

	//this function must be called before export html files, because it is needed for the allActivities tables
	static void computeActivitiesAtTime();
	
	//this function must be called before export html files, because it is needed to add activities with same starting time (simultanious activities)
	static void computeActivitiesWithSameStartingTime();
	//this function add activities with same starting time into the allActivities list
	static bool addActivitiesWithSameStartingTime(QList<int>& allActivities, int hour);

	//the following functions write the conflicts text and the xml files
	static void writeSubgroupsTimetableXml(QWidget* parent, const QString& xmlfilename);
	static void writeTeachersTimetableXml(QWidget* parent, const QString& xmlfilename);
	static void writeActivitiesTimetableXml(QWidget* parent, const QString& xmlfilename);
	static void writeConflictsTxt(QWidget* parent, const QString& filename, const QString& saveTime, int placedActivities);

	//the following functions write the css and html timetable files
	static void writeIndexHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeStylesheetCss(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubgroupsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubgroupsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubgroupsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubgroupsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubgroupsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubgroupsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeGroupsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeGroupsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeGroupsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeGroupsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeGroupsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeGroupsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeYearsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeYearsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeYearsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeYearsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeYearsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeYearsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeAllActivitiesTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeAllActivitiesTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeAllActivitiesTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeAllActivitiesTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeAllActivitiesTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeAllActivitiesTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeRoomsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeRoomsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeRoomsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeRoomsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeRoomsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeRoomsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubjectsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubjectsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubjectsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubjectsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubjectsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeSubjectsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersFreePeriodsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersFreePeriodsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);

	//the following functions return QStrings, because they are 'only' subfunctions to the writeXxxHtml functions
	static QString writeActivityStudents(int ai, int day, int hour, bool notAvailable, bool colspan, bool rowspan);
	static QString writeActivitiesStudents(const QList<int>& allActivities);
	static QString writeActivityTeacher(int teacher, int day, int hour, bool colspan, bool rowspan);
	static QString writeActivitiesTeachers(const QList<int>& allActivities);
	static QString writeActivityRoom(int room, int day, int hour, bool colspan, bool rowspan);
	static QString writeActivitiesRooms(const QList<int>& allActivities);
	static QString writeActivitiesSubjects(const QList<int>& allActivities);

	//the following functions return QStrings, because they are 'only' subfunctions to the writeActivity-iesXxx functions
	static QString writeHead(bool java, int placedActivities, bool printInstitution);
	static QString writeTOCDays(bool detailed);
	static QString writeStartTagTDofActivities(const Activity* act, bool detailed, bool colspan, bool rowspan);
	static QString writeSubjectAndActivityTags(const Activity* act, const QString& startTag, const QString& startTagAttribute, bool activityTagsOnly);
	static QString writeStudents(const Activity* act, const QString& startTag, const QString& startTagAttribute);
	static QString writeTeachers(const Activity* act, const QString& startTag, const QString& startTagAttribute);
	static QString writeRoom(int ai, const QString& startTag, const QString& startTagAttribute);
	static QString writeNotAvailableSlot(const QString& weight);
	static QString writeBreakSlot(const QString& weight);
	static QString writeEmpty();
};

#endif
