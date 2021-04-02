/***************************************************************************
                          modifyconstraintactivitytagpreferredroomsform.h  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTACTIVITYTAGPREFERREDROOMSFORM_H
#define MODIFYCONSTRAINTACTIVITYTAGPREFERREDROOMSFORM_H

#include "ui_modifyconstraintactivitytagpreferredroomsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <q3combobox.h>
#include <q3listbox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3textedit.h>
#include <q3valuelist.h>

class ModifyConstraintActivityTagPreferredRoomsForm : public QDialog, Ui::ModifyConstraintActivityTagPreferredRoomsForm_template  {
	Q_OBJECT
public:
	ModifyConstraintActivityTagPreferredRoomsForm(ConstraintActivityTagPreferredRooms* ctr);
	~ModifyConstraintActivityTagPreferredRoomsForm();

	void updateRoomsListBox();
	
public slots:
	void addRoom();
	void removeRoom();

	void ok();
	void cancel();
	
	void clear();
private:
	ConstraintActivityTagPreferredRooms* _ctr;
};

#endif
