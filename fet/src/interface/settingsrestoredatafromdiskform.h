/***************************************************************************
                          settingsrestoredatafromdiskform.h  -  description
                             -------------------
    begin                : 2023
    copyright            : (C) 2023 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef SETTINGSRESTOREDATAFROMDISKFORM_H
#define SETTINGSRESTOREDATAFROMDISKFORM_H

#include "ui_settingsrestoredatafromdiskform_template.h"

class SettingsRestoreDataFromDiskForm : public QDialog, Ui::SettingsRestoreDataFromDiskForm_template{
	Q_OBJECT
	
public:
	SettingsRestoreDataFromDiskForm(QWidget* parent);
	~SettingsRestoreDataFromDiskForm();

public Q_SLOTS:
	void enableHistoryOnDiskCheckBox_toggled();

	void ok();
	void cancel();
};

#endif
