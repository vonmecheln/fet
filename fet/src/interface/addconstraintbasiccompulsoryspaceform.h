/***************************************************************************
                          addconstraintbasiccompulsoryspaceform.h  -  description
                             -------------------
    begin                : Feb 13, 2005
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

#ifndef ADDCONSTRAINTBASICCOMPULSORYSPACEFORM_H
#define ADDCONSTRAINTBASICCOMPULSORYSPACEFORM_H

#include "ui_addconstraintbasiccompulsoryspaceform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
//#include "fetmainform.h"

#include <q3combobox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3textedit.h>

class AddConstraintBasicCompulsorySpaceForm : public QDialog, Ui::AddConstraintBasicCompulsorySpaceForm_template  {
	Q_OBJECT
public:
	AddConstraintBasicCompulsorySpaceForm();
	~AddConstraintBasicCompulsorySpaceForm();

public slots:
	void constraintChanged();
	void addCurrentConstraint();
};

#endif
