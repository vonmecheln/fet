/***************************************************************************
                          modifystudentsgroupform.h  -  description
                             -------------------
    begin                : Feb 8, 2005
    copyright            : (C) 2005 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef MODIFYSTUDENTSGROUPFORM_H
#define MODIFYSTUDENTSGROUPFORM_H

#include "ui_modifystudentsgroupform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyStudentsGroupForm : public QDialog, Ui::ModifyStudentsGroupForm_template {
	Q_OBJECT
public:
	//QString _yearName;
	QString _initialGroupName;
	int _initialNumberOfStudents;

	ModifyStudentsGroupForm(QWidget* parent, const QString& yearName, const QString& initialGroupName, int initialNumberOfStudents);
	~ModifyStudentsGroupForm();

public Q_SLOTS:
	void ok();
	void cancel();
};

#endif
