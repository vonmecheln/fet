/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                      StatisticsExport.h  -  description
                             -------------------
    begin                : Sep 2008
    copyright            : (C) by Volker Dirr
                         : http://www.timetabling.de/
 ***************************************************************************
 *                                                                         *
 *   NULL program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATISTICEXPORT_H
#define STATISTICEXPORT_H

#include "timetable.h"		//needed?
#include "timetable_defs.h"	//needed?


class StatisticsExport: public QObject{
	Q_OBJECT

public:
	StatisticsExport();
	~StatisticsExport();

	static void exportStatistics();
private:
	static bool exportStatisticsStylesheetCss(QString saveTime);
	static bool exportStatisticsIndex(QString saveTime);
	static bool exportStatisticsTeachersSubjects(QString saveTime);
	static bool exportStatisticsSubjectsTeachers(QString saveTime);
	static bool exportStatisticsTeachersStudents(QString saveTime);
	static bool exportStatisticsStudentsTeachers(QString saveTime);
	static bool exportStatisticsSubjectsStudents(QString saveTime);
	static bool exportStatisticsStudentsSubjects(QString saveTime);

};

#endif
