/***************************************************************************
                          timetableallocatehoursform.h  -  description
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

#ifndef TIMETABLEALLOCATEHOURSFORM_H
#define TIMETABLEALLOCATEHOURSFORM_H

#include "timetableallocatehoursform_template.h"
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

class TimeSolvingThread: public QThread{
	Q_OBJECT

public:
	void run();
	
signals: 
	void generationComputed(int generation);
};

class TimetableAllocateHoursForm : public TimetableAllocateHoursForm_template  {
	Q_OBJECT

public:

	TimetableAllocateHoursForm();

	~TimetableAllocateHoursForm();

	void start();

	void stop();

	void write();

	void savePosition();

	void loadPosition();

	void initializeUnallocated();

	void initializeRandomly();

	void closePressed();
	
	void generationLogging(int generation);

	void writeSimulationResults(TimeChromosome& c);

	/**
	Function writing the students' timetable xml format, to a file
	*/
	void writeStudentsTimetableXml(const QString& xmlfilename);

	/**
	Function writing the teachers' timetable in xml format
	*/
	void writeTeachersTimetableXml(const QString& xmlfilename);

	/**
	Function writing the students' timetable html format, to a file
	(variant 1).
	Days horizontal version.
	*/
	void writeStudentsTimetable1DaysHorizontalHtml(const QString& htmlfilename);

	/**
	Function writing the students' timetable html format, to a file
	(variant 1).
	Days vertical version.
	*/
	void writeStudentsTimetable1DaysVerticalHtml(const QString& htmlfilename);

	/**
	Function writing the students' timetable html format, to a file
	(variant 2)
	*/
	void writeStudentsTimetable2Html(const QString& htmlfilename);

	/**
	Function writing the teachers' timetable in html format, version 1.
	Days horizontal version
	*/
	void writeTeachersTimetable1DaysHorizontalHtml(const QString& htmlfilename);

	/**
	Function writing the teachers' timetable in html format, version 1.
	Days vertical version
	*/
	void writeTeachersTimetable1DaysVerticalHtml(const QString& htmlfilename);

	/**
	Function writing the teachers' timetable in html format, version 2
	*/
	void writeTeachersTimetable2Html(const QString& htmlfilename);

	void getStudentsTimetable(TimeChromosome& c);

	void getTeachersTimetable(TimeChromosome& c);

private slots:
	void updateGeneration(int generation);
};

#endif
