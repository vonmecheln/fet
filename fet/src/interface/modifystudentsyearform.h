/***************************************************************************
                          modifystudentsyearform.h  -  description
                             -------------------
    begin                : Feb 8, 2005
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

#ifndef MODIFYSTUDENTSYEARFORM_H
#define MODIFYSTUDENTSYEARFORM_H

#include "modifystudentsyearform_template.h"

#include "genetictimetable_defs.h"
#include "genetictimetable.h"
#include "fet.h"

#include "fetmainform.h"

#include <qspinbox.h>
#include <qlineedit.h>
#include <q3textedit.h>


class ModifyStudentsYearForm : public ModifyStudentsYearForm_template {
public:
	QString _initialYearName;
	int _initialNumberOfStudents;

	ModifyStudentsYearForm(const QString& initialYearName, int initialNumberOfStudents);
	~ModifyStudentsYearForm();

	void ok();
	void cancel();
};

#endif
