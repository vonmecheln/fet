/***************************************************************************
                          modifystudentsgroupform.h  -  description
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

#ifndef MODIFYSTUDENTSGROUPFORM_H
#define MODIFYSTUDENTSGROUPFORM_H

#include "modifystudentsgroupform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

//#include "fetmainform.h"

#include <qspinbox.h>
#include <qlineedit.h>
#include <q3textedit.h>


class ModifyStudentsGroupForm : public ModifyStudentsGroupForm_template {
public:
	QString _yearName;
	QString _initialGroupName;
	int _initialNumberOfStudents;

	ModifyStudentsGroupForm(const QString& yearName, const QString& initialGroupName, int initialNumberOfStudents);
	~ModifyStudentsGroupForm();

	void ok();
	void cancel();
};

#endif
