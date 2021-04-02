/***************************************************************************
                          timetableviewstudentswithrooms2form.h  -  description
                             -------------------
    begin                : 14 Feb 2005
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

#ifndef TIMETABLEVIEWSTUDENTSWITHROOMS2FORM_H
#define TIMETABLEVIEWSTUDENTSWITHROOMS2FORM_H

#include "timetableviewstudentswithrooms2form_template.h"

class TimetableViewStudentsWithRooms2Form : public TimetableViewStudentsWithRooms2Form_template  {
public: 
	TimetableViewStudentsWithRooms2Form();
	~TimetableViewStudentsWithRooms2Form();

	void yearChanged(const QString& yearName);
	void groupChanged(const QString& groupName);
	void subgroupChanged(const QString& subgroupName);

	void updateStudentsTimetableTable();
	
	void detailActivity(int row, int col);
	
	void lock();
	void lockTime();
	void lockSpace();
	void lock(bool time, bool space);
};

#endif
