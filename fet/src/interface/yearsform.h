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

#ifndef YEARSFORM_H
#define YEARSFORM_H

#include "ui_yearsform_template.h"

class YearsForm : public QDialog, Ui::YearsForm_template
{
	Q_OBJECT
public:
	YearsForm(QWidget* parent);
	~YearsForm();

public Q_SLOTS:
	void addYear();
	void removeYear();
	void modifyYear();

	void moveYearUp();
	void moveYearDown();

	void sortYears();
	void yearChanged();
	
	void activateStudents();
	void deactivateStudents();
	
	void divideYear();
	
	void longName();
	void code();
	void comments();
};

#endif
