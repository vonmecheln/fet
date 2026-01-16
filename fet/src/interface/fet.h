/*
File fet.h
*/

/***************************************************************************
                          fet.h  -  description
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

#ifndef FET_H
#define FET_H

#include "timetable_defs.h"
#include "timetable.h"

#include "matrix.h"

#include <QTextStream>

#ifndef FET_COMMAND_LINE
#include <QApplication>
#endif

class QWidget;

class FetTranslate: public QObject{
	Q_OBJECT
};

/**
The one and only instantiation of the main class.
*/
extern Timetable gt;

/**
The name of the file from where the rules are read.
*/
extern QString INPUT_FILENAME_XML;

/**
The timetable for the teachers
*/
extern Matrix3D<int> teachers_timetable_weekly;

/**
The timetable for the students
*/
extern Matrix3D<int> students_timetable_weekly;

/**
The timetable for the rooms
*/
extern Matrix3D<int> rooms_timetable_weekly;
extern Matrix3D<QList<int>> virtual_rooms_timetable_weekly;

/**
The timetable for the buildings
*/
extern Matrix3D<QList<int>> buildings_timetable_weekly;

#ifndef FET_COMMAND_LINE
class FetSettings: public QObject{
	Q_OBJECT

public:
	void readGenerationParameters(QApplication& qapplication);

public Q_SLOTS:
	void writeGenerationParameters();
};
//void writeDefaultGenerationParameters();
#endif

#ifndef FET_COMMAND_LINE
void setLanguage(QApplication& qapplication, QWidget* parent);
#else
void setLanguage(QCoreApplication& qapplication, QWidget* parent);
#endif

void initLanguagesSet();

/**
The main function.
*/
int main(int argc, char **argv);

#endif
