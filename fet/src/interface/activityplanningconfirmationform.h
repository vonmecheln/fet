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
#ifndef ACTIVITY_PLANNING_CONFIRMATION_FORM_H
#define ACTIVITY_PLANNING_CONFIRMATION_FORM_H

#include "ui_activityplanningconfirmationform_template.h"

class ActivityPlanningConfirmationForm:public QDialog, Ui::ActivityPlanningConfirmationForm_template
{
	Q_OBJECT

public:
	ActivityPlanningConfirmationForm();
	~ActivityPlanningConfirmationForm();
};

#endif
