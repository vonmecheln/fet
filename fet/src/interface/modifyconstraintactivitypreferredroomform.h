/***************************************************************************
                          modifyconstraintactivitypreferredroomform.h  -  description
                             -------------------
    begin                : February 13, 2005
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

#ifndef MODIFYCONSTRAINTACTIVITYPREFERREDROOMFORM_H
#define MODIFYCONSTRAINTACTIVITYPREFERREDROOMFORM_H

#include "modifyconstraintactivitypreferredroomform_template.h"
#include "genetictimetable_defs.h"
#include "genetictimetable.h"
#include "fet.h"
#include "fetmainform.h"

#include <q3combobox.h>
#include <q3listbox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3textedit.h>

class ModifyConstraintActivityPreferredRoomForm : public ModifyConstraintActivityPreferredRoomForm_template  {
	ConstraintActivityPreferredRoom* _ctr;
public:
	ModifyConstraintActivityPreferredRoomForm(ConstraintActivityPreferredRoom* ctr);
	~ModifyConstraintActivityPreferredRoomForm();

	void updateActivitiesComboBox();
	void updateRoomsComboBox();

	void ok();
	void cancel();
};

#endif