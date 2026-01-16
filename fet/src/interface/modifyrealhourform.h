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

#ifndef MODIFYREALHOURFORM_H
#define MODIFYREALHOURFORM_H

#include "ui_modifyrealhourform_template.h"

#include <QString>

class ModifyRealHourForm : public QDialog, Ui::ModifyRealHourForm_template
{
	Q_OBJECT
public:
	ModifyRealHourForm(QWidget* parent, const QString& name, const QString& longName);

	~ModifyRealHourForm();
	
	QString name;
	QString longName;
	
public Q_SLOTS:
	void ok();
	void cancel();
};

#endif
