//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2005 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef INSTITUTIONNAMEFORM_H
#define INSTITUTIONNAMEFORM_H

#include "ui_institutionnameform_template.h"

class InstitutionNameForm : public QDialog, Ui::InstitutionNameForm_template
{
	Q_OBJECT
public:
	InstitutionNameForm(QWidget* parent);

	~InstitutionNameForm();

public slots:
	void ok();
	void cancel();
};

#endif
