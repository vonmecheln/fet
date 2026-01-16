/***************************************************************************
                          settingsstyleandcolorschemeform.h  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef SETTINGSSTYLEANDCOLORSCHEMEFORM_H
#define SETTINGSSTYLEANDCOLORSCHEMEFORM_H

#include "ui_settingsstyleandcolorschemeform_template.h"

class SettingsStyleAndColorSchemeForm : public QDialog, Ui::SettingsStyleAndColorSchemeForm_template{
	Q_OBJECT
	
public:
	SettingsStyleAndColorSchemeForm(QWidget* parent);
	~SettingsStyleAndColorSchemeForm();

public Q_SLOTS:
	void ok();
	void cancel();
};

#endif
