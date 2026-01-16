//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef DAYSFORM_H
#define DAYSFORM_H

#include "ui_daysform_template.h"

#include <QStringList>

class QWidget;

class DaysForm : public QDialog, Ui::DaysForm_template
{
	Q_OBJECT
public:
	DaysForm(QWidget* parent);

	~DaysForm();
	
	QStringList realNamesForDays;
	QStringList realLongNamesForDays;
	QStringList realNamesForRealDays;
	QStringList realLongNamesForRealDays;

public Q_SLOTS:
	void numberOfDaysChanged();
	void insertDay();
	void modifyDay();
	void removeDay();

	void numberOfRealDaysChanged();
	void modifyRealDay();

	void ok();
	void cancel();
};

#endif
