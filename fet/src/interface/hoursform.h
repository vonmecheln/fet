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

#ifndef HOURSFORM_H
#define HOURSFORM_H

#include "ui_hoursform_template.h"

#include <QStringList>

class HoursForm : public QDialog, Ui::HoursForm_template
{
	Q_OBJECT
public:
	HoursForm(QWidget* parent);

	~HoursForm();
	
	QStringList realNamesForHours;
	QStringList realLongNamesForHours;
	QStringList realNamesForRealHours;
	QStringList realLongNamesForRealHours;

public Q_SLOTS:
	void numberOfHoursChanged();
	void insertHour();
	void modifyHour();
	void removeHour();
	
	void numberOfRealHoursChanged();
	void modifyRealHour();
	
	void ok();
	void cancel();
};

#endif
