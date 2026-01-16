//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2020 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef NOTIFICATIONCOMMANDFORM_H
#define NOTIFICATIONCOMMANDFORM_H

#include "ui_notificationcommandform_template.h"

class NotificationCommandForm : public QDialog, Ui::NotificationCommandForm_template
{
	Q_OBJECT
public:
	NotificationCommandForm(QWidget* parent);
	~NotificationCommandForm();

public Q_SLOTS:
	void ok();
	void cancel();
	
	void help();
	
	void browse();
	void browseEachTimetable();
	
	void externalCommandCheckBoxToggled();
	void externalCommandEachTimetableCheckBoxToggled();
	//void detachedCheckBoxToggled();
};

#endif
