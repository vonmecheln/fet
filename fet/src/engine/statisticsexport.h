/*
File statisticsexport.h
*/

/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                      statisticsexport.h  -  description
                             -------------------
    begin                : Sep 2008
    copyright            : (C) by Volker Dirr
                         : http://www.timetabling.de/
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATISTICEXPORT_H
#define STATISTICEXPORT_H

#include <QString>

#include "timetable.h"		//needed?
#include "timetable_defs.h"	//needed?

class StatisticsExport: public QObject{
	Q_OBJECT

public:
	StatisticsExport();
	~StatisticsExport();

	static void exportStatistics(QWidget* parent);
private:
	//this function must be called before export html files, because it compute the IDs
	static void computeHashForIDsStatistics();

	//the following functions write the css and html statistics files
	static bool exportStatisticsStylesheetCss(QWidget* parent, QString saveTime);
	static bool exportStatisticsIndex(QWidget* parent, QString saveTime);
	static bool exportStatisticsTeachersSubjects(QWidget* parent, QString saveTime);
	static bool exportStatisticsSubjectsTeachers(QWidget* parent, QString saveTime);
	static bool exportStatisticsTeachersStudents(QWidget* parent, QString saveTime);
	static bool exportStatisticsStudentsTeachers(QWidget* parent, QString saveTime);
	static bool exportStatisticsSubjectsStudents(QWidget* parent, QString saveTime);
	static bool exportStatisticsStudentsSubjects(QWidget* parent, QString saveTime);
};

#endif
