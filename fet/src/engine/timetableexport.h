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

	/**
	Function writing the students' timetable xml format, to a file
	*/
	static void writeStudentsTimetableXml(const QString& xmlfilename);

	/**
	Function writing the teachers' timetable in xml format
	*/
	static void writeTeachersTimetableXml(const QString& xmlfilename);
		
	/**
	Function writing conflicts.txt
	*/
	static void writeConflictsTxt(const QString& filename, QString saveTime, int placedActivities);

	/**
	Function writing css stylesheet for each html-file
	*/
	static void writeStylesheetCss(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the students' timetable html format, to a file
	Days horizontal version.
	*/
	static void writeStudentsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the students' timetable html format, to a file
	Days vertical version.
	*/
	static void writeStudentsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the students' timetable html format, to a file
	Time horizontal version.
	*/
	static void writeStudentsTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the students' timetable html format, to a file
	Time vertical version.
	*/
	static void writeStudentsTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

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
};

#endif
