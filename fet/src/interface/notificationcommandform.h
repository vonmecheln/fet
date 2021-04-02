//
//
// Description: This file is part of FET
//
//
// Author: Lalescu Liviu <Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2020 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
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

public slots:
	void ok();
	void cancel();
	
	void help();
	
	void browse();
	
	void externalCommandCheckBoxToggled();
	//void detachedCheckBoxToggled();
};

#endif
