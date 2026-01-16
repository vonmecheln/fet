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

#include "timetableshowconflictsform.h"

#include "timetable.h"

#include <QString>

extern Timetable gt;
extern QString conflictsStringTitle;
extern QString conflictsString;

TimetableShowConflictsForm::TimetableShowConflictsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);

	conflictsTextEdit->setReadOnly(true);

	connect(closePushButton, &QPushButton::clicked, this, &TimetableShowConflictsForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	setWindowTitle(conflictsStringTitle);
	conflictsTextEdit->setPlainText(conflictsString);
}

void TimetableShowConflictsForm::newTimetableGenerated()
{
	/*setupUi(this);
	
	closePushButton->setDefault(true);

	conflictsTextEdit->setReadOnly(true);

	connect(closePushButton, SIG NAL(clicked()), this, SL OT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);*/
	
	setWindowTitle(conflictsStringTitle);
	conflictsTextEdit->setPlainText(conflictsString);
}

TimetableShowConflictsForm::~TimetableShowConflictsForm()
{
	saveFETDialogGeometry(this);
}
