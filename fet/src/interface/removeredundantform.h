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
#ifndef REMOVE_REDUNDANT_FORM_H
#define REMOVE_REDUNDANT_FORM_H

#include "ui_removeredundantform_template.h"

class RemoveRedundantForm:public QDialog, Ui::RemoveRedundantForm_template
{
	Q_OBJECT

public:
	RemoveRedundantForm();
	~RemoveRedundantForm();
	
public slots:
	void wasAccepted();
	void wasCanceled();
	
	void on_removeRedundantCheckBox_toggled();
};

#endif
