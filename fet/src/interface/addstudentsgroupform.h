/***************************************************************************
                          addstudentsgroupform.h  -  description
                             -------------------
    begin                : Sat Jan 24 2004
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

#ifndef ADDSTUDENTSGROUPFORM_H
#define ADDSTUDENTSGROUPFORM_H

#include "addstudentsgroupform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

//#include "fetmainform.h"

#include <qspinbox.h>
#include <qlineedit.h>
#include <q3textedit.h>


class AddStudentsGroupForm : public AddStudentsGroupForm_template {
public:
	AddStudentsGroupForm();
	~AddStudentsGroupForm();

	void addStudentsGroup();
};

#endif
