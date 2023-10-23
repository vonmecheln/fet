//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2016 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"

#include "settingsdatatoprintintimetablesform.h"

SettingsDataToPrintInTimetablesForm::SettingsDataToPrintInTimetablesForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	okPushButton->setDefault(true);
	
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(wasAccepted()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(wasCanceled()));
	
	subjectsCheckBox->setChecked(TIMETABLE_HTML_PRINT_SUBJECTS);
	activityTagsCheckBox->setChecked(TIMETABLE_HTML_PRINT_ACTIVITY_TAGS);
	teachersCheckBox->setChecked(TIMETABLE_HTML_PRINT_TEACHERS);
	studentsCheckBox->setChecked(TIMETABLE_HTML_PRINT_STUDENTS);
	roomsCheckBox->setChecked(TIMETABLE_HTML_PRINT_ROOMS);
}

SettingsDataToPrintInTimetablesForm::~SettingsDataToPrintInTimetablesForm()
{
	saveFETDialogGeometry(this);
}

void SettingsDataToPrintInTimetablesForm::wasAccepted()
{
	TIMETABLE_HTML_PRINT_SUBJECTS=subjectsCheckBox->isChecked();
	TIMETABLE_HTML_PRINT_ACTIVITY_TAGS=activityTagsCheckBox->isChecked();
	TIMETABLE_HTML_PRINT_TEACHERS=teachersCheckBox->isChecked();
	TIMETABLE_HTML_PRINT_STUDENTS=studentsCheckBox->isChecked();
	TIMETABLE_HTML_PRINT_ROOMS=roomsCheckBox->isChecked();

	this->accept();
}

void SettingsDataToPrintInTimetablesForm::wasCanceled()
{
	this->reject();
}
