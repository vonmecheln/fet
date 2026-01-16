//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2021 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef GETMODEFORNEWFETFILEFORM_H
#define GETMODEFORNEWFETFILEFORM_H

#include "ui_getmodefornewfileform_template.h"

class GetModeForNewFileForm : public QDialog, Ui::GetModeForNewFileForm_template
{
	Q_OBJECT
public:
	int mode;

	GetModeForNewFileForm(QWidget* parent);
	~GetModeForNewFileForm();

public Q_SLOTS:
	void help();
	void ok();
	void cancel();
};

#endif
