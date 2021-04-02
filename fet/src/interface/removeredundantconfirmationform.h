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
#ifndef REMOVE_REDUNDANT_CONFIRMATION_FORM_H
#define REMOVE_REDUNDANT_CONFIRMATION_FORM_H

#include "ui_removeredundantconfirmationform_template.h"

class RemoveRedundantConfirmationForm:public QDialog, Ui::RemoveRedundantConfirmationForm_template
{
	Q_OBJECT

public:
	RemoveRedundantConfirmationForm();
	~RemoveRedundantConfirmationForm();
};

#endif
