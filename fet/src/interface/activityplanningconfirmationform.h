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

#ifndef ACTIVITY_PLANNING_CONFIRMATION_FORM_H
#define ACTIVITY_PLANNING_CONFIRMATION_FORM_H

#include "ui_activityplanningconfirmationform_template.h"

class ActivityPlanningConfirmationForm: public QDialog, Ui::ActivityPlanningConfirmationForm_template
{
	Q_OBJECT

public:
	bool dontShowAgain;

	ActivityPlanningConfirmationForm(QWidget* parent);
	~ActivityPlanningConfirmationForm();

public Q_SLOTS:
	void dontShowAgainCheckBoxToggled();
};

#endif
