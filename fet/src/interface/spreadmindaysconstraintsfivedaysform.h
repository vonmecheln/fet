//
//
// C++ Interface: $MODULE$
//
// Description:
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SPREAD_MIN_DAYS_CONSTRAINTS_FIVE_DAYS_FORM_H
#define SPREAD_MIN_DAYS_CONSTRAINTS_FIVE_DAYS_FORM_H

#include "ui_spreadmindaysconstraintsfivedaysform_template.h"

class SpreadMinDaysConstraintsFiveDaysForm:public QDialog, Ui::SpreadMinDaysConstraintsFiveDaysForm_template
{
	Q_OBJECT

public:
	SpreadMinDaysConstraintsFiveDaysForm();
	~SpreadMinDaysConstraintsFiveDaysForm();
	
public slots:
	void wasAccepted();
	void wasCanceled();
	void help();
	
	void on_spread2CheckBox_toggled();
	void on_spread3CheckBox_toggled();
	void on_spread4OrMoreCheckBox_toggled();
};

#endif
