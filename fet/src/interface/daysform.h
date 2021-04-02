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
#ifndef DAYSFORM_H
#define DAYSFORM_H

#include "ui_daysform_template.h"

class DaysForm : public QDialog, Ui::DaysForm_template
{
	Q_OBJECT
public:
	DaysForm();

	~DaysForm();

public slots:
	void daysChanged();
	void ok();
	void cancel();
};

#endif
