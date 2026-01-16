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

#include "removeredundantconfirmationform.h"

#include "timetable_defs.h"

RemoveRedundantConfirmationForm::RemoveRedundantConfirmationForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	continuePushButton->setDefault(true);
	
	connect(continuePushButton, &QPushButton::clicked, this, &RemoveRedundantConfirmationForm::accept);
	connect(cancelPushButton, &QPushButton::clicked, this, &RemoveRedundantConfirmationForm::reject);
	connect(dontShowAgainCheckBox, &QCheckBox::toggled, this, &RemoveRedundantConfirmationForm::dontShowAgainCheckBoxToggled);

	dontShowAgain=dontShowAgainCheckBox->isChecked();
	
	plainTextEdit->setReadOnly(true);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QString s;
	
	s+=tr("Please read carefully the description below:");
	s+="\n\n";
	s+=tr("This function is intended to be used after you inputted all data or after you used the advanced function "
	 "of spreading the activities evenly over the week. This function will automatically remove the redundant constraints of "
	 "type min days between activities (and min half days between activities, if you use the Mornings-Afternoons mode), "
	 "so that your data is more correct and the timetable easier for FET to find.");
	s+="\n\n";
	s+=tr("This function is only useful if you have redundant constraints with weight < 100%. The redundant constraints "
	 "with weight 100% are actually useful for the generation algorithm, and should not be removed. They are expected to help avoiding "
	 "potentially incorrect placements of activities in time slots at earlier stages of generating the timetable.");
	s+="\n\n";
	s+=tr("NOTE: If in a group of redundant constraints there is at least one with weight 100%, no constraint from this group "
	 "will be removed.");
	s+="\n\n";
	s+=tr("Removing means making their weight percentage 0%. This is done so you can still activate them again, if "
	 "needed, and also for you to be able to see when they are broken, in the soft conflicts list.");
	s+="\n\n";
	s+=tr("This function might be useful if you have active constraints of type activities same starting time and/or "
	 "activities same starting day and/or max 0 days between activities and/or max 0 half days between activities "
	 "(if you use the Mornings-Afternoons mode) with weight 100%.");
	s+="\n\n";
	s+=tr("IMPORTANT NOTE: If you activate/deactivate some constraints of type same starting time / same starting day / "
	 "max 0 (half) days between activities, or increase above 0 the number of max (half) days allowed between activities "
	 "of some constraints after you applied this function, you need to rethink what happens to the removed redundant "
	 "constraints, and maybe reactivate some of them, or apply the advanced function of spreading the activities "
	 "evenly over the week, again. Unfortunately, the program cannot correctly do this on its own.");
	s+="\n\n";
	s+=tr("Please SAVE/BACKUP your current file and keep it safe, in case anything goes wrong, and only continue if "
	 "you did that already. The current function might modify much your data.");
	
	plainTextEdit->setPlainText(s);
}

RemoveRedundantConfirmationForm::~RemoveRedundantConfirmationForm()
{
	saveFETDialogGeometry(this);
}

void RemoveRedundantConfirmationForm::dontShowAgainCheckBoxToggled()
{
	dontShowAgain=dontShowAgainCheckBox->isChecked();
}
