/***************************************************************************
                          timetableviewrooms2form.h  -  description
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

#ifndef TIMETABLEVIEWROOMS2FORM_H
#define TIMETABLEVIEWROOMS2FORM_H

#include "timetableviewrooms2form_template.h"

class TimetableViewRooms2Form : public TimetableViewRooms2Form_template  {
public:
	TimetableViewRooms2Form();
	~TimetableViewRooms2Form();

	void roomChanged(const QString& roomName);

	void updateRoomsTimetableTable();

	void detailActivity(int row, int col);
	
	void lock();
	void lock(bool lockTime, bool lockSpace);
	void lockTime();
	void lockSpace();
};

#endif
