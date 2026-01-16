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

#include "modifyrealhourform.h"

#include "utilities.h"

ModifyRealHourForm::ModifyRealHourForm(QWidget* parent, const QString& name, const QString& longName): QDialog(parent)
{
	setupUi(this);
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	nameLineEdit->setText(name);
	longNameLineEdit->setText(longName);
	
	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
	
	connect(okPushButton, &QPushButton::clicked, this, &ModifyRealHourForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyRealHourForm::cancel);
	
	okPushButton->setDefault(true);
}

ModifyRealHourForm::~ModifyRealHourForm()
{
	saveFETDialogGeometry(this);
}

void ModifyRealHourForm::ok()
{
	name=nameLineEdit->text();
	longName=longNameLineEdit->text();
	
	this->accept();
}

void ModifyRealHourForm::cancel()
{
	this->reject();
}
