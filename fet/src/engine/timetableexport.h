/*
File timetableexport.h
*/

/***************************************************************************
                          timetableexport.h  -  description
                             -------------------
    begin                : Tue Apr 22, 2003
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

#ifndef TIMETABLEEXPORT_H
#define TIMETABLEEXPORT_H

#include "timetable_defs.h"
#include "timetable.h"

#include <QString>
#include <QHash>
#include <QList>
#include <QSet>

class QWidget;

bool writeAtLeastATimetable();

extern QHash<int, int> hashActivityColorBySubject;
extern QList<int> activeHashActivityColorBySubjectAndStudents;
extern QHash<int, int> hashActivityColorBySubject;
extern QList<int> activeHashActivityColorBySubjectAndStudents;

class TimetableExport: public QObject{
	Q_OBJECT

public:
	TimetableExport();
	~TimetableExport();
	
	static QString dummyTh(int nSpacesBefore); //to differentiate the days in time horizontal/vertical views
	static QString dummyTr(int nSpacesBeforeTr, int nSpacesBeforeTd, int colspan);
	static QString dummyTd(int nSpacesBefore);
	
	static void stringToColor(const QString& s, int& r, int& g, int& b);
	
	static void getStudentsTeachersRoomsBuildingsTimetable(Solution& c);
	static void getNumberOfPlacedActivities(int& number1, int& number2);
	
	static void writeGenerationResults(QWidget* parent);
	static void writeHighestStageResults(QWidget* parent);
	static void writeGenerationResults(QWidget* parent, int n, bool highest=false); //write in a directory with number n (for multiple generation)
	//If highest==true, it writes a partial highest timetable.
	static void writeGenerationResultsCommandLine(QWidget* parent, const QString& outputDirectory);
	
	static void writeRandomSeed(QWidget* parent, const MRG32k3a& rng, bool before);
	static void writeRandomSeed(QWidget* parent, const MRG32k3a& rng, int n, bool before); //write in a directory with number n (for multiple generation)
	static void writeRandomSeedCommandLine(QWidget* parent, const MRG32k3a& rng, const QString& outputDirectory, bool before);
	static void writeRandomSeedFile(QWidget* parent, const MRG32k3a& rng, const QString& filename, bool before);
	
	static void writeTimetableDataFile(QWidget* parent, const QString& filename);
	
	static void writeReportForMultiple(QWidget* parent, const QString& description, bool begin);
	
	//the following functions return a single HTML table (needed for HTML file export and printing)
	static QString singleSubgroupsTimetableDaysHorizontalHtml(int htmlLevel, int subgroup, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames, const QList<int>& subgroupsSortedOrder=QList<int>());
	static QString singleSubgroupsTimetableDaysVerticalHtml(int htmlLevel, int subgroup, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames, const QList<int>& subgroupsSortedOrder=QList<int>());
	static QString singleSubgroupsTimetableTimeHorizontalHtml(int htmlLevel, int maxSubgroups, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames, const QList<int>& subgroupsSortedOrder=QList<int>());
	static QString singleSubgroupsTimetableTimeVerticalHtml(int htmlLevel, int maxSubgroups, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames, const QList<int>& subgroupsSortedOrder=QList<int>());
	static QString singleSubgroupsTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, int maxSubgroups, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames, const QList<int>& subgroupsSortedOrder=QList<int>());
	static QString singleSubgroupsTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, int maxSubgroups, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames, const QList<int>& subgroupsSortedOrder=QList<int>());
	static QString singleGroupsTimetableDaysHorizontalHtml(int htmlLevel, int group, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool detailed, bool repeatNames);
	static QString singleGroupsTimetableDaysVerticalHtml(int htmlLevel, int group, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool detailed, bool repeatNames);
	static QString singleGroupsTimetableTimeHorizontalHtml(int htmlLevel, int maxGroups, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool detailed, bool repeatNames);
	static QString singleGroupsTimetableTimeVerticalHtml(int htmlLevel, int maxGroups, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool detailed, bool repeatNames);
	static QString singleGroupsTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, int maxGroups, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool detailed, bool repeatNames);
	static QString singleGroupsTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, int maxGroups, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool detailed, bool repeatNames);
	static QString singleYearsTimetableDaysHorizontalHtml(int htmlLevel, int year, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool detailed, bool repeatNames);
	static QString singleYearsTimetableDaysVerticalHtml(int htmlLevel, int year, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool detailed, bool repeatNames);
	static QString singleYearsTimetableTimeHorizontalHtml(int htmlLevel, int maxYears, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool detailed, bool repeatNames);
	static QString singleYearsTimetableTimeVerticalHtml(int htmlLevel, int maxYears, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool detailed, bool repeatNames);
	static QString singleYearsTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, int maxYears, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool detailed, bool repeatNames);
	static QString singleYearsTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, int maxYears, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool detailed, bool repeatNames);
	static QString singleAllActivitiesTimetableDaysHorizontalHtml(int htmlLevel, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleAllActivitiesTimetableDaysVerticalHtml(int htmlLevel, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleAllActivitiesTimetableTimeHorizontalHtml(int htmlLevel, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleAllActivitiesTimetableTimeVerticalHtml(int htmlLevel, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleAllActivitiesTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleAllActivitiesTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleTeachersTimetableDaysHorizontalHtml(int htmlLevel, int teacher, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleTeachersTimetableDaysVerticalHtml(int htmlLevel, int teacher, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleTeachersTimetableTimeHorizontalHtml(int htmlLevel, int maxTeachers, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleTeachersTimetableTimeVerticalHtml(int htmlLevel, int maxTeachers, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleTeachersTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, int maxTeachers, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleTeachersTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, int maxTeachers, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);

	static QString singleBuildingsTimetableDaysHorizontalHtml(int htmlLevel, int building, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleBuildingsTimetableDaysVerticalHtml(int htmlLevel, int building, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleBuildingsTimetableTimeHorizontalHtml(int htmlLevel, int maxBuildings, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleBuildingsTimetableTimeVerticalHtml(int htmlLevel, int maxBuildings, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleBuildingsTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, int maxBuildings, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleBuildingsTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, int maxBuildings, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);

	static QString singleRoomsTimetableDaysHorizontalHtml(int htmlLevel, int room, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleRoomsTimetableDaysVerticalHtml(int htmlLevel, int room, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleRoomsTimetableTimeHorizontalHtml(int htmlLevel, int maxRooms, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleRoomsTimetableTimeVerticalHtml(int htmlLevel, int maxRooms, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleRoomsTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, int maxRooms, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleRoomsTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, int maxRooms, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleSubjectsTimetableDaysHorizontalHtml(int htmlLevel, int subject, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleSubjectsTimetableDaysVerticalHtml(int htmlLevel, int subject, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleSubjectsTimetableTimeHorizontalHtml(int htmlLevel, int maxSubjects, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleSubjectsTimetableTimeVerticalHtml(int htmlLevel, int maxSubjects, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleSubjectsTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, int maxSubjects, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleSubjectsTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, int maxSubjects, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleActivityTagsTimetableDaysHorizontalHtml(int htmlLevel, int activityTag, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleActivityTagsTimetableDaysVerticalHtml(int htmlLevel, int activityTag, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleActivityTagsTimetableTimeHorizontalHtml(int htmlLevel, int maxActivityTag, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleActivityTagsTimetableTimeVerticalHtml(int htmlLevel, int maxActivityTag, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleActivityTagsTimetableTimeHorizontalDailyHtml(int htmlLevel, int realDay, int maxActivityTag, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleActivityTagsTimetableTimeVerticalDailyHtml(int htmlLevel, int realDay, int maxActivityTag, QSet<int>& excludedNames, const QString& saveTime,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool repeatNames);
	static QString singleTeachersFreePeriodsTimetableDaysHorizontalHtml(int htmlLevel, const QString& saveTime, bool detailed, bool repeatNames);
	static QString singleTeachersFreePeriodsTimetableDaysVerticalHtml(int htmlLevel, const QString& saveTime, bool detailed, bool repeatNames);
	static QString singleTeachersStatisticsHtml(int htmlLevel, const QString& saveTime, bool detailed, bool repeatNames, bool printAll);
	static QString singleStudentsStatisticsHtml(int htmlLevel, const QString& saveTime, bool detailed, bool repeatNames, bool printAll);
	
	static void computeHashActivityColorBySubject();
	static void computeHashActivityColorBySubjectAndStudents();

	static QString printCompleteLegend(bool printSubjectsLegend, bool printActivityTagsLegend, bool printTeachersLegend, bool printStudentsLegend, bool printRoomsLegend,
		bool printCodesFirst);
	
private:
	//this function must be called before export HTML files, because it compute the IDs
	static void computeHashForIDsTimetable();
	
	//this function must be called before export HTML files, because it is needed for the allActivities tables
	static void computeActivitiesAtTime();
	
	//this function must be called before export HTML files, because it is needed to add activities with same starting time (simultaneous activities)
	static void computeActivitiesWithSameStartingTime();
	//this function add activities with same starting time into the allActivitiesList list and into the allActivitiesSet set
	static bool addActivitiesWithSameStartingTime(QList<int>& allActivitiesList, QSet<int>& allActivitiesSet, int hour);
	
	//the following functions write the conflicts as text and the XML files
	static void writeSubgroupsTimetableXml(QWidget* parent, const QString& xmlfilename, const QList<int>& subgroupsSortedOrder=QList<int>());
	static void writeTeachersTimetableXml(QWidget* parent, const QString& xmlfilename);
	static void writeActivitiesTimetableXml(QWidget* parent, const QString& xmlfilename);
	static void writeConflictsTxt(QWidget* parent, const QString& filename, const QString& saveTime, int placedActivities);
	
	//the following functions write the CSS and HTML timetable files
	static void writeIndexHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeStylesheetCss(QWidget* parent, const QString& cssfilename, const QString& saveTime, int placedActivities);
	static void writeSubgroupsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities, const QList<int>& subgroupsSortedOrder=QList<int>());
	static void writeSubgroupsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities, const QList<int>& subgroupsSortedOrder=QList<int>());
	static void writeSubgroupsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities, const QList<int>& subgroupsSortedOrder=QList<int>());
	static void writeSubgroupsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities, const QList<int>& subgroupsSortedOrder=QList<int>());
	static void writeSubgroupsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities, const QList<int>& subgroupsSortedOrder=QList<int>());
	static void writeSubgroupsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities, const QList<int>& subgroupsSortedOrder=QList<int>());
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

	static void writeBuildingsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeBuildingsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeBuildingsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeBuildingsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeBuildingsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeBuildingsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);

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
	static void writeActivityTagsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeActivityTagsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeActivityTagsTimetableTimeHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeActivityTagsTimetableTimeVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeActivityTagsTimetableTimeHorizontalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeActivityTagsTimetableTimeVerticalDailyHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersFreePeriodsTimetableDaysHorizontalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersFreePeriodsTimetableDaysVerticalHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeTeachersStatisticsHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	static void writeStudentsStatisticsHtml(QWidget* parent, const QString& htmlfilename, const QString& saveTime, int placedActivities);
	
	//the following functions return QStrings, because they are 'only' subfunctions to the writeXyzHtml functions
	static QString writeActivityStudents(int htmlLevel, int ai, int day, int hour, bool notAvailable, bool colspan, bool rowspan,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
		bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
		bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
		bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
		bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes,
		const QString& skipStudentsSet);
	static QString writeActivitiesStudents(int htmlLevel, const QList<int>& allActivitiesOriginal,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
		bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
		bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
		bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
		bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes);
	static QString writeActivityTeacher(int htmlLevel, int teacher, int day, int hour, bool colspan, bool rowspan,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
		bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
		bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
		bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
		bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes,
		const QString& skipTeacher);
	static QString writeActivitiesTeachers(int htmlLevel, const QList<int>& allActivitiesOriginal,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
		bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
		bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
		bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
		bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes);

	static QString writeActivityBuilding(int htmlLevel, int building, int day, int hour, bool colspan, bool rowspan,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
		bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
		bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
		bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
		bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes
		/*, const QString& skipBuilding*/); //currently unneeded, because FET supports only one building per activity
	static QString writeActivitiesBuildings(int htmlLevel, const QList<int>& allActivitiesOriginal,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
		bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
		bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
		bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
		bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes);

	static QString writeActivityRoom(int htmlLevel, int room, int day, int hour, bool colspan, bool rowspan,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
		bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
		bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
		bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
		bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes
		/*, const QString& skipRoom*/); //currently unneeded, because FET supports only one room per activity
	static QString writeActivitiesRooms(int htmlLevel, const QList<int>& allActivitiesOriginal,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
		bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
		bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
		bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
		bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes);
	static QString writeActivitiesSubjects(int htmlLevel, const QList<int>& allActivitiesOriginal,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
		bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
		bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
		bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
		bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes);
	static QString writeActivitiesActivityTags(int htmlLevel, const QList<int>& allActivitiesOriginal,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
		bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
		bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
		bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
		bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes);

	//the following functions return QStrings, because they are 'only' subfunctions to the writeActivity-iesXyz functions
	static QString writeHead(bool java, int placedActivities, bool printInstitution);
	static QString writeTOCDays(bool writeNames, bool writeLongNames);
	static QString writeStartTagTDofActivities(int htmlLevel, const Activity* act, bool detailed, bool colspan, bool rowspan, int colorBy,
		bool printSubjects, bool printActivityTags, bool printTeachers, bool printStudents, bool printRooms,
		bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes,
		bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes,
		bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes,
		bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes,
		bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes);
	static QString writeSubjectAndActivityTags(int htmlLevel, const Activity* act, const QString& startTag, const QString& startTagAttribute, bool activityTagsOnly, bool printActivityTags,
		bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes, bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes);
	static QString writeStudents(int htmlLevel, const Activity* act, const QString& startTag, const QString& startTagAttribute,
		bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes);
	static QString writeTeachers(int htmlLevel, const Activity* act, const QString& startTag, const QString& startTagAttribute,
		bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes);
	static QString writeRoom(int htmlLevel, int ai, const QString& startTag, const QString& startTagAttribute,
		bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes);
	static QString writeNotAvailableSlot(int htmlLevel, const QString& weight);
	static QString writeBreakSlot(int htmlLevel, const QString& weight);
	static QString writeEmpty(int htmlLevel);
	
	static QString getSubjectString(Subject* sbj, bool printSubjectsNames, bool printSubjectsLongNames, bool printSubjectsCodes);
	static QString getActivityTagString(ActivityTag* at, bool printActivityTagsNames, bool printActivityTagsLongNames, bool printActivityTagsCodes);
	static QString getTeacherString(Teacher* tch, bool printTeachersNames, bool printTeachersLongNames, bool printTeachersCodes);
	static QString getStudentsSetString(StudentsSet* ss, bool printStudentsNames, bool printStudentsLongNames, bool printStudentsCodes);
	static QString getBuildingString(Building* bu, bool printBuildingsNames, bool printBuildingsLongNames, bool printBuildingsCodes);
	static QString getRoomString(Room* rm, bool printRoomsNames, bool printRoomsLongNames, bool printRoomsCodes);

	static QString getDayOrRealDayString(int d, bool printDaysNames, bool printDaysLongNames);
	static QString getDayString(int d, bool printDaysNames, bool printDaysLongNames);
	static QString getRealDayString(int d, bool printDaysNames, bool printDaysLongNames);

	static QString getHourOrRealHourString(int h, bool printHoursNames, bool printHoursLongNames);
	static QString getHourString(int h, bool printHoursNames, bool printHoursLongNames);
	static QString getRealHourString(int h, bool printHoursNames, bool printHoursLongNames);

	static QString getVerticalDayString(int d, bool printDaysNames, bool printDaysLongNames);
	static QString getVerticalRealDayString(int d, bool printDaysNames, bool printDaysLongNames);
	
	static void addActivitiesLegend(const QList<int>& allActivitiesList, QSet<Subject*>& usedSubjectsSet, QSet<ActivityTag*>& usedActivityTagsSet, QSet<Teacher*>& usedTeachersSet, QSet<StudentsSet*>& usedStudentsSet, QSet<Room*>& usedRoomsSet,
		bool printSubjectsLegend, bool printActivityTagsLegend, bool printTeachersLegend, bool printStudentsLegend, bool printRoomsLegend);
	static void printLegend(QString& tmpString, QSet<Subject*>& usedSubjectsSet, QSet<ActivityTag*>& usedActivityTagsSet, QSet<Teacher*>& usedTeachersSet, QSet<StudentsSet*>& usedStudentsSet, QSet<Room*>& usedRoomsSet,
		bool printSubjectsLegend, bool printActivityTagsLegend, bool printTeachersLegend, bool printStudentsLegend, bool printRoomsLegend,
		bool printCodesFirst);
};

#endif
