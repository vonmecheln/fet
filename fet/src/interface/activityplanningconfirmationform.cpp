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
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "activityplanningconfirmationform.h"

#include "timetable_defs.h"

ActivityPlanningConfirmationForm::ActivityPlanningConfirmationForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	continuePushButton->setDefault(true);
	
	connect(continuePushButton, &QPushButton::clicked, this, &ActivityPlanningConfirmationForm::accept);
	connect(cancelPushButton, &QPushButton::clicked, this, &ActivityPlanningConfirmationForm::reject);
	connect(dontShowAgainCheckBox, &QCheckBox::toggled, this, &ActivityPlanningConfirmationForm::dontShowAgainCheckBoxToggled);
	
	dontShowAgain=dontShowAgainCheckBox->isChecked();
	
	plainTextEdit->setReadOnly(true);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QString s;
	
	s+=tr("Please read carefully the description below:");
	s+="\n\n";
	s+=tr("This is a simple activity planning dialog. You need to add all teachers, "
		"subjects and the students structure before you can work with it. It "
		"is also recommended to add the necessary activity tags before using this dialog.");
	s+="\n\n";
	s+=tr("The main work will be done in the upper table with the students-subjects "
		"matrix. Select an 'action' from the right side and activate a table cell to do that "
		"action (activate with the mouse double-click or click, depending on your platform, or by pressing Enter). "
		"The number of hours is summed in the heading, so "
		"you can always check if your planning fits your needs.");
	s+="\n\n";
	s+=tr("You can also double click the tables heading to do an action to the whole row/column.");
	s+="\n\n";
	s+=tr("You can change size of the tables by using the splitter between the tables.");
	s+="\n\n";
	s+=tr("Pseudo activities are (sometimes) difficult to see in the tables, so "
		"you can view them quickly with the corresponding push button.");
	s+="\n\n";
	s+=tr("You have also a facility to hide/show the buttons on the right. This is useful "
		"if you need more space for the tables. With the buttons shown (default), there is available a "
		"'Hide buttons' command. When you press it, all the other buttons will be hidden and you will have only "
		"one small button remaining. Press it and all the buttons will be shown again.");
	s+=" ";
	s+=tr("If the buttons are hidden, when you will double-click or click the mouse on the tables or press Enter "
		"on a table cell, the command that will be done is the one which was selected when the buttons were not hidden.");
	
	plainTextEdit->setPlainText(s);
}

ActivityPlanningConfirmationForm::~ActivityPlanningConfirmationForm()
{
	saveFETDialogGeometry(this);
}

void ActivityPlanningConfirmationForm::dontShowAgainCheckBoxToggled()
{
	dontShowAgain=dontShowAgainCheckBox->isChecked();
}
