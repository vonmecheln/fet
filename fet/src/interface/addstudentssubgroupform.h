/***************************************************************************
                          addstudentssubgroupform.h  -  description
                             -------------------
    begin                : Sat Jan 24 2004
    copyright            : (C) 2004 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef ADDSTUDENTSSUBGROUPFORM_H
#define ADDSTUDENTSSUBGROUPFORM_H

#include "ui_addstudentssubgroupform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddStudentsSubgroupForm : public QDialog, Ui::AddStudentsSubgroupForm_template {
	Q_OBJECT
public:
	AddStudentsSubgroupForm(QWidget* parent, const QString& yearName, const QString& groupName);
	~AddStudentsSubgroupForm();

public Q_SLOTS:
	void addStudentsSubgroup();
};

#endif
