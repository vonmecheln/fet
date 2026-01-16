/***************************************************************************
                          settingsrestoredatafrommemoryform.h  -  description
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

#ifndef SETTINGSRESTOREDATAFROMMEMORYFORM_H
#define SETTINGSRESTOREDATAFROMMEMORYFORM_H

#include "ui_settingsrestoredatafrommemoryform_template.h"

class SettingsRestoreDataFromMemoryForm : public QDialog, Ui::SettingsRestoreDataFromMemoryForm_template{
	Q_OBJECT
	
public:
	SettingsRestoreDataFromMemoryForm(QWidget* parent);
	~SettingsRestoreDataFromMemoryForm();

public Q_SLOTS:
	void enableHistoryInMemoryCheckBox_toggled();

	void ok();
	void cancel();
};

#endif
