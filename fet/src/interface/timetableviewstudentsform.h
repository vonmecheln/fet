/***************************************************************************
                          timetableviewstudentsform.h  -  description
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

#ifndef TIMETABLEVIEWSTUDENTSFORM_H
#define TIMETABLEVIEWSTUDENTSFORM_H

#include "timetableviewstudentsform_template.h"

class TimetableViewStudentsForm : public TimetableViewStudentsForm_template  {
	Q_OBJECT
public: 
	TimetableViewStudentsForm();
	~TimetableViewStudentsForm();

	void yearChanged(const QString& yearName);
	void groupChanged(const QString& groupName);
	void subgroupChanged(const QString& subgroupName);

	void updateStudentsTimetableTable();
	
	void detailActivity(int row, int col);

	void lock(bool lockTime, bool lockSpace);	
public slots:
	void lockTime();
	void lockSpace();
	void lockTimeSpace();
};

#endif
