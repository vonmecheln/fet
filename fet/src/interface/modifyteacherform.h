//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2020 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef MODIFYTEACHERFORM_H
#define MODIFYTEACHERFORM_H

#include "teacher.h"

#include "ui_modifyteacherform_template.h"

#include <QHash>
#include <QString>

class ModifyTeacherForm : public QDialog, Ui::ModifyTeacherForm_template
{
	Q_OBJECT
	
	Teacher* tch;
	
	QHash<QString, int>* activeHoursHashPointer;
	
public:
	ModifyTeacherForm(QWidget* parent, Teacher* _tch, QHash<QString, int>& activeHoursHash);

	~ModifyTeacherForm();

public Q_SLOTS:
	void ok();
	void help();
	void cancel();
};

#endif
