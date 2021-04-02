/***************************************************************************
                          timetableallocatehoursroomsform.h  -  description
                             -------------------
    begin                : Feb 14, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#ifndef TIMETABLEALLOCATEHOURSROOMSFORM_H
#define TIMETABLEALLOCATEHOURSROOMSFORM_H

#include "timetableallocatehoursroomsform_template.h"
#include "genetictimetable_defs.h"
#include "genetictimetable.h"
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

class TimeSpaceSolvingThread: public QThread{
	Q_OBJECT

public:
	void run();
	
signals:
	void generationComputed(int generation);
};

class TimetableAllocateHoursRoomsForm : public TimetableAllocateHoursRoomsForm_template
{
	Q_OBJECT
	
public:

	TimetableAllocateHoursRoomsForm();

	~TimetableAllocateHoursRoomsForm();

	void start();

	void stop();

	void write();

	void savePosition();

	void loadPosition();

	void initializeUnallocated();

	void initializeRandomly();

	void generationLogging(int generation);

	void writeSimulationResults(TimeSpaceChromosome& c);

	/**
	Function writing the students' timetable in xml format, to a file
	*/
	void writeStudentsTimetableXml(const QString& xmlfilename);

	/**
	Function writing the teachers' timetable in xml format, to a file
	*/
	void writeTeachersTimetableXml(const QString& xmlfilename);

	/**
	Function writing the rooms' timetable in xml format, to a file
	*/
	void writeRoomsTimetableXml(const QString& xmlfilename);

	/**
	Function writing the students' timetable (with rooms) in html format, to a file
	Variant 1.
	Days horizontal version.
	*/
	void writeStudentsTimetableWithRooms1DaysHorizontalHtml(const QString& htmlfilename);

	/**
	Function writing the students' timetable (with rooms) in html format, to a file
	Variant 1.
	Days vertical version.
	*/
	void writeStudentsTimetableWithRooms1DaysVerticalHtml(const QString& htmlfilename);

	/**
	Function writing the students' timetable (with rooms) in html format, to a file
	Variant 2
	*/
	void writeStudentsTimetableWithRooms2Html(const QString& htmlfilename);

	/**
	Function writing the teachers' timetable (with rooms) in html format, to a file
	Variant 1.
	Days horizontal.
	*/
	void writeTeachersTimetableWithRooms1DaysHorizontalHtml(const QString& htmlfilename);

	/**
	Function writing the teachers' timetable (with rooms) in html format, to a file
	Variant 1.
	Days vertical.
	*/
	void writeTeachersTimetableWithRooms1DaysVerticalHtml(const QString& htmlfilename);

	/**
	Function writing the teachers' timetable (with rooms) in html format, to a file
	Variant 2.
	*/
	void writeTeachersTimetableWithRooms2Html(const QString& htmlfilename);

	/**
	Function writing the rooms' timetable in html format, to a file
	Variant 1.
	Days horizontal.
	*/
	void writeRoomsTimetable1DaysHorizontalHtml(const QString& htmlfilename);

	/**
	Function writing the rooms' timetable in html format, to a file
	Variant 1.
	Days vertical.
	*/
	void writeRoomsTimetable1DaysVerticalHtml(const QString& htmlfilename);

	/**
	Function writing the rooms' timetable in html format, to a file
	Variant 2
	*/
	void writeRoomsTimetable2Html(const QString& htmlfilename);

	void getTeachersTimetable(TimeSpaceChromosome& c);

	void getStudentsTimetable(TimeSpaceChromosome& c);

	void getRoomsTimetable(TimeSpaceChromosome& c);
	
private slots:
	void updateGeneration(int generation);
};

#endif
