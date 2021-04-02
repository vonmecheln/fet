/***************************************************************************
                          timetableviewteacherswithroomsform.h  -  description
                             -------------------
    begin                : Feb 6 2005
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

#ifndef TIMETABLEVIEWTEACHERSWITHROOMSFORM_H
#define TIMETABLEVIEWTEACHERSWITHROOMSFORM_H

#include "timetableviewteacherswithroomsform_template.h"

class TimetableViewTeachersWithRoomsForm : public TimetableViewTeachersWithRoomsForm_template  {
public: 
	TimetableViewTeachersWithRoomsForm();
	~TimetableViewTeachersWithRoomsForm();

	void teacherChanged(const QString& teacherName);

	void updateTeachersTimetableTable();
	
	void detailActivity(int row, int col);
	
	void lock(bool lockTime, bool lockSpace);
	void lockTime();
	void lockSpace();
	void lock();
};

#endif
