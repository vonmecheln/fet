//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef HOURSFORM_H
#define HOURSFORM_H

#include "ui_hoursform_template.h"

#include <QStringList>

class QWidget;

class HoursForm : public QDialog, Ui::HoursForm_template
{
	Q_OBJECT
public:
	HoursForm(QWidget* parent);

	~HoursForm();
	
	QStringList realNames;

public slots:
	void numberOfHoursChanged();
	void insertHour();
	void modifyHour();
	void removeHour();
	void ok();
	void cancel();
};

#endif
