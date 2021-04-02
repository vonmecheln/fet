/***************************************************************************
                          modifyconstraintactivitytagpreferredroomform.h  -  description
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

#ifndef MODIFYCONSTRAINTACTIVITYTAGPREFERREDROOMFORM_H
#define MODIFYCONSTRAINTACTIVITYTAGPREFERREDROOMFORM_H

#include "ui_modifyconstraintactivitytagpreferredroomform_template.h"
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

class ModifyConstraintActivityTagPreferredRoomForm : public QDialog, Ui::ModifyConstraintActivityTagPreferredRoomForm_template  {
	Q_OBJECT
	
	ConstraintActivityTagPreferredRoom* _ctr;
public:
	ModifyConstraintActivityTagPreferredRoomForm(ConstraintActivityTagPreferredRoom* ctr);
	~ModifyConstraintActivityTagPreferredRoomForm();

	void updateActivityTagsComboBox();
	void updateRoomsComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
