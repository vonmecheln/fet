//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2024 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef MODIFYREALDAYFORM_H
#define MODIFYREALDAYFORM_H

#include "ui_modifyrealdayform_template.h"

#include <QString>

class ModifyRealDayForm : public QDialog, Ui::ModifyRealDayForm_template
{
	Q_OBJECT
public:
	ModifyRealDayForm(QWidget* parent, const QString& name, const QString& longName);

	~ModifyRealDayForm();
	
	QString name;
	QString longName;
	
public Q_SLOTS:
	void ok();
	void cancel();
};

#endif
