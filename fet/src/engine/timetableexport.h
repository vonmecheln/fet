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

class TimetableExport: public QObject{
	Q_OBJECT

public:
	TimetableExport();
	~TimetableExport();


	static void getStudentsTimetable(Solution& c);

	static void getTeachersTimetable(Solution& c);

	static void getRoomsTimetable(Solution& c);


	static void writeSimulationResults();

	static void writeSimulationResults(int n); //write in a directory with number n (for multiple generation)
	
	static void writeSimulationResultsCommandLine();
	
	static void writeTimetableDataFile(const QString& filename);

	/**
	Function writing the subgroups' timetable xml format, to a file
	*/
	static void writeSubgroupsTimetableXml(const QString& xmlfilename);

	/**
	Function writing the teachers' timetable in xml format
	*/
	static void writeTeachersTimetableXml(const QString& xmlfilename);
		
	/**
	Function writing conflicts.txt
	*/
	static void writeConflictsTxt(const QString& filename, QString saveTime, int placedActivities);

	/**
	Function writing the index html-file
	*/
	static void writeIndexHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing css stylesheet for each html-file
	*/
	static void writeStylesheetCss(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the subgroups' timetable html format, to a file
	Days horizontal version.
	*/
	static void writeSubgroupsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the subgroups' timetable html format, to a file
	Days vertical version.
	*/
	static void writeSubgroupsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the subgroups' timetable html format, to a file
	Time horizontal version.
	*/
	static void writeSubgroupsTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the subgroups' timetable html format, to a file
	Time vertical version.
	*/
	static void writeSubgroupsTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the groups' timetable html format, to a file
	Days horizontal version.
	*/
	static void writeGroupsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the groups' timetable html format, to a file
	Days vertical version.
	*/
	static void writeGroupsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the groups' timetable html format, to a file
	Time horizontal version.
	*/
	static void writeGroupsTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the groups' timetable html format, to a file
	Time vertical version.
	*/
	static void writeGroupsTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the years' timetable html format, to a file
	Days horizontal version.
	*/
	static void writeYearsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the years' timetable html format, to a file
	Days vertical version.
	*/
	static void writeYearsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the years' timetable html format, to a file
	Time horizontal version.
	*/
	static void writeYearsTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the years' timetable html format, to a file
	Time vertical version.
	*/
	static void writeYearsTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing all activities' timetable html format, to a file
	Days horizontal version.
	*/
	static void writeAllActivitiesTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing all activities' timetable html format, to a file
	Days vertical version.
	*/
	static void writeAllActivitiesTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);


	/**
	Function writing all activities' timetable html format, to a file
	Time horizontal version.
	*/
	static void writeAllActivitiesTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing all activities' timetable html format, to a file
	Time vertical version.
	*/
	static void writeAllActivitiesTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);
	

	/**
	Function writing the teachers' timetable in html format
	Days horizontal version
	*/
	static void writeTeachersTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the teachers' timetable in html format
	Days vertical version
	*/
	static void writeTeachersTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the teachers' timetable in html format
	Time horizontal version.
	*/
	static void writeTeachersTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the teachers' timetable in html format
	Time vertical version.
	*/
	static void writeTeachersTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the rooms' timetable in html format
	Days horizontal version
	*/
	static void writeRoomsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the rooms' timetable in html format
	Days vertical version
	*/
	static void writeRoomsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);
	
	/**
	Function writing the rooms' timetable in html format
	Time horizontal version
	*/
	static void writeRoomsTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the rooms' timetable in html format
	Time vertical version
	*/
	static void writeRoomsTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the subjects' timetable in html format
	Days horizontal version
	*/
	static void writeSubjectsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the subjects' timetable in html format
	Days vertical version
	*/
	static void writeSubjectsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);
	
	/**
	Function writing the subjects' timetable in html format
	Time horizontal version
	*/
	static void writeSubjectsTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the subjects' timetable in html format
	Time vertical version
	*/
	static void writeSubjectsTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the teachers' free periods timetable in html format
	Days horizontal version
	*/
	static void writeTeachersFreePeriodsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the teachers' free periods timetable in html format
	Days vertical version
	*/
	static void writeTeachersFreePeriodsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);
};

#endif
