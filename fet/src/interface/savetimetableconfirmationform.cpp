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

#include "savetimetableconfirmationform.h"

#include "timetable_defs.h"

SaveTimetableConfirmationForm::SaveTimetableConfirmationForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	continuePushButton->setDefault(true);
	
	connect(continuePushButton, &QPushButton::clicked, this, &SaveTimetableConfirmationForm::accept);
	connect(cancelPushButton, &QPushButton::clicked, this, &SaveTimetableConfirmationForm::reject);
	connect(dontShowAgainCheckBox, &QCheckBox::toggled, this, &SaveTimetableConfirmationForm::dontShowAgainCheckBoxToggled);

	dontShowAgain=dontShowAgainCheckBox->isChecked();
	
	plainTextEdit->setReadOnly(true);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QString s;
	
	s+=tr("Please read carefully the description below:");
	s+="\n\n";

	s+=tr("This option is only useful if you need to lock current timetable into a file."
	 " Locking means that there will be added constraints activity preferred starting time and"
	 " activity preferred room with 100% importance for each activity to fix it at current place in current timetable."
	 " You can save this timetable as an ordinary .fet file; when you'll open it, you'll see all old inputted data (activities, teachers, etc.)"
	 " and the locking constraints as the last time/space constraints."
	 " You can unlock some of these activities (by removing constraints) if small changes appear in the configuration, and generate again"
	 " and the remaining locking constraints will be respected.");
	s+="\n\n";
	s+=tr("The added constraints will have the 'permanently locked' tag set to false, so you can also unlock the activities from the "
	 "'Timetable' menu, without interfering with the initial constraints which are made by you 'permanently locked'");
	s+="\n\n";
	s+=tr("This option is useful for institutions where you obtain a timetable, then some small changes appear,"
	 " and you need to regenerate timetable, but respecting in a large proportion the old timetable");
	s+="\n\n";
	s+=tr("The current data file will not be affected by anything, locking constraints will only be added to the file you selected to save to"
	 " (you can save the current data file and open the saved timetable file after that to check it)");
	
	plainTextEdit->setPlainText(s);
}

SaveTimetableConfirmationForm::~SaveTimetableConfirmationForm()
{
	saveFETDialogGeometry(this);
}

void SaveTimetableConfirmationForm::dontShowAgainCheckBoxToggled()
{
	dontShowAgain=dontShowAgainCheckBox->isChecked();
}
