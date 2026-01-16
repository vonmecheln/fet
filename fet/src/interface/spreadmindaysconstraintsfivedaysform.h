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

#ifndef SPREADMINDAYSCONSTRAINTSFIVEDAYSFORM_H
#define SPREADMINDAYSCONSTRAINTSFIVEDAYSFORM_H

#include "ui_spreadmindaysconstraintsfivedaysform_template.h"

class SpreadMinDaysConstraintsFiveDaysForm:public QDialog, Ui::SpreadMinDaysConstraintsFiveDaysForm_template
{
	Q_OBJECT

public:
	SpreadMinDaysConstraintsFiveDaysForm(QWidget* parent);
	~SpreadMinDaysConstraintsFiveDaysForm();
	
public Q_SLOTS:
	void wasAccepted();
	void wasCanceled();
	void help();
	
	void spread2CheckBox_toggled();
	void spread3CheckBox_toggled();
	void spread4OrMoreCheckBox_toggled();
};

#endif
