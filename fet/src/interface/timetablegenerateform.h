/***************************************************************************
                          timetablegenerateform.h  -  description
                             -------------------
    begin                : Tue Apr 22 2003
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

#ifndef TIMETABLEGENERATEFORM_H
#define TIMETABLEGENERATEFORM_H

#include "timetablegenerateform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <q3combobox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3table.h>
#include <qapplication.h>
#include <qstring.h>
#include <qthread.h>
#include <qmutex.h>

class TimeSolvingThread: public QThread{
	Q_OBJECT

public:
	void run();

signals:
	void generationComputed(int generation);
};

class TimetableGenerateForm : public TimetableGenerateForm_template  {
	Q_OBJECT

public:

	TimetableGenerateForm();

	~TimetableGenerateForm();

	void start();

	void stop();

	void write();

	void closePressed();

	void generationLogging(int generation);

	void writeSimulationResults(Solution& c);

	/**
	Function writing the students' timetable xml format, to a file
	*/
	void writeStudentsTimetableXml(const QString& xmlfilename);

	/**
	Function writing the teachers' timetable in xml format
	*/
	void writeTeachersTimetableXml(const QString& xmlfilename);
		
	/**
	Function writing conflicts.txt
	*/
	void writeConflictsTxt(const QString& filename, QString saveTime, int placedActivities);

	/**
	Function writing css stylesheet for each html-file
	*/
	void writeStylesheetCss(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the students' timetable html format, to a file
	Days horizontal version.
	*/
	void writeStudentsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the students' timetable html format, to a file
	Days vertical version.
	*/
	void writeStudentsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the students' timetable html format, to a file
	Time horizontal version.
	*/
	void writeStudentsTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the students' timetable html format, to a file
	Time vertical version.
	*/
	void writeStudentsTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the teachers' timetable in html format
	Days horizontal version
	*/
	void writeTeachersTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the teachers' timetable in html format
	Days vertical version
	*/
	void writeTeachersTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the teachers' timetable in html format
	Time horizontal version.
	*/
	void writeTeachersTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the teachers' timetable in html format
	Time vertical version.
	*/
	void writeTeachersTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the rooms' timetable in html format
	Days horizontal version
	*/
	void writeRoomsTimetableDaysHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the rooms' timetable in html format
	Days vertical version
	*/
	void writeRoomsTimetableDaysVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);
	
	/**
	Function writing the rooms' timetable in html format
	Time horizontal version
	*/
	void writeRoomsTimetableTimeHorizontalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	/**
	Function writing the rooms' timetable in html format
	Time vertical version
	*/
	void writeRoomsTimetableTimeVerticalHtml(const QString& htmlfilename, QString saveTime, int placedActivities);

	
	
	void getStudentsTimetable(Solution& c);

	void getTeachersTimetable(Solution& c);

	void getRoomsTimetable(Solution& c);

private slots:
	void activityPlaced(int na);

	void simulationFinished();

	void impossibleToSolve();
};

#endif
