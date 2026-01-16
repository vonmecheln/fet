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
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef SETTINGSDATATOPRINTINTIMETABLESFORM_H
#define SETTINGSDATATOPRINTINTIMETABLESFORM_H

#include "ui_settingsdatatoprintintimetablesform_template.h"

class SettingsDataToPrintInTimetablesForm:public QDialog, Ui::SettingsDataToPrintInTimetablesForm_template
{
	Q_OBJECT

public:
	SettingsDataToPrintInTimetablesForm(QWidget* parent);
	~SettingsDataToPrintInTimetablesForm();
	
public Q_SLOTS:
	void wasAccepted();
	void wasCanceled();
};

#endif
