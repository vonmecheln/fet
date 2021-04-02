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
#ifndef HOURSFORM_H
#define HOURSFORM_H

#include "ui_hoursform_template.h"

class HoursForm : public QDialog, Ui::HoursForm_template
{
	Q_OBJECT
public:
	HoursForm();

	~HoursForm();

public slots:
	void hoursChanged();
	void ok();
	void cancel();
};

#endif
