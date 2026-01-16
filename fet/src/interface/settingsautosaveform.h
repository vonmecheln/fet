/***************************************************************************
                          settingsautosaveform.h  -  description
                             -------------------
    begin                : 2023
    copyright            : (C) 2023 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef SETTINGSAUTOSAVEFORM_H
#define SETTINGSAUTOSAVEFORM_H

#include "ui_settingsautosaveform_template.h"

class SettingsAutosaveForm : public QDialog, Ui::SettingsAutosaveForm_template{
	Q_OBJECT
	
public:
	SettingsAutosaveForm(QWidget* parent);
	~SettingsAutosaveForm();

public Q_SLOTS:
	void enableAutosaveCheckBox_toggled();

	void ok();
	void cancel();
	
	void chooseDirectory();
};

#endif
