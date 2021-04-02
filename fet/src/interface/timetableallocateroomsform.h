/***************************************************************************
                          timetableallocateroomsform.h  -  description
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

#ifndef TIMETABLEALLOCATEROOMSFORM_H
#define TIMETABLEALLOCATEROOMSFORM_H

#include "timetableallocateroomsform_template.h"
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

class TimetableAllocateRoomsForm;

class SpaceSolvingThread: public QThread{
	Q_OBJECT

public:
	void run();
	
signals:
	void generationComputed(int generation);
};

class TimetableAllocateRoomsForm : public TimetableAllocateRoomsForm_template
{
	Q_OBJECT	

public:

	TimetableAllocateRoomsForm();

	~TimetableAllocateRoomsForm();

	void start();

	void stop();

	void write();

	void savePosition();

	void loadPosition();

	void initializeUnallocated();

	void initializeRandomly();

	void generationLogging(int generation);

	void simulationRunning();

	void writeSimulationResults(SpaceChromosome& c);

	/**
	Function writing the rooms' timetable in xml format, to a file
	*/
	void writeRoomsTimetableXml(const QString& xmlfilename);

	/**
	Function writing the students' timetable (with rooms) in html format, to a file.
	Days horizontal.
	*/
	void writeStudentsTimetableWithRoomsDaysHorizontalHtml(const QString& htmlfilename);

	/**
	Function writing the students' timetable (with rooms) in html format, to a file.
	Days vertical.
	*/
	void writeStudentsTimetableWithRoomsDaysVerticalHtml(const QString& htmlfilename);

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
	Function writing the rooms' timetable in html format, to a file.
	Days horizontal.
	*/
	void writeRoomsTimetableDaysHorizontalHtml(const QString& htmlfilename);

	/**
	Function writing the rooms' timetable in html format, to a file.
	Days vertical.
	*/
	void writeRoomsTimetableDaysVerticalHtml(const QString& htmlfilename);

	void getRoomsTimetable(SpaceChromosome& c);
	
private slots:
	void updateGeneration(int generation);
};

#endif
