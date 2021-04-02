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
#ifndef SPREAD_CONFIRMATION_FORM_H
#define SPREAD_CONFIRMATION_FORM_H

#include "ui_spreadconfirmationform_template.h"

class SpreadConfirmationForm:public QDialog, Ui::SpreadConfirmationForm_template
{
	Q_OBJECT

public:
	SpreadConfirmationForm();
	~SpreadConfirmationForm();
};

#endif
