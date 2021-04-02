/***************************************************************************
                          timetableviewteachersform.h  -  description
                             -------------------
    begin                : Wed May 14 2003
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

#ifndef TIMETABLEVIEWTEACHERSFORM_H
#define TIMETABLEVIEWTEACHERSFORM_H

#include "timetableviewteachersform_template.h"

class TimetableViewTeachersForm : public TimetableViewTeachersForm_template  {
public: 
	TimetableViewTeachersForm();
	~TimetableViewTeachersForm();

	void teacherChanged(const QString& teacherName);

	void updateTeachersTimetableTable();
	
	void detailActivity(int row, int col);
	
	void lock();
};

#endif
