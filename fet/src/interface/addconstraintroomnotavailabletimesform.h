/***************************************************************************
                          addconstraintroomnotavailabletimesform.h  -  description
                             -------------------
    begin                : January 8, 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTROOMNOTAVAILABLETIMESFORM_H
#define ADDCONSTRAINTROOMNOTAVAILABLETIMESFORM_H

#include "addconstraintroomnotavailabletimesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <q3combobox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3textedit.h>

class AddConstraintRoomNotAvailableTimesForm : public AddConstraintRoomNotAvailableTimesForm_template  {
	Q_OBJECT
public:
	AddConstraintRoomNotAvailableTimesForm();
	~AddConstraintRoomNotAvailableTimesForm();
	
	void updateRoomsComboBox();

	void addConstraint();

public slots:	
	void tableClicked(int, int, int, const QPoint&);
	
	void setAllAvailable();
	void setAllNotAvailable();
};

#endif