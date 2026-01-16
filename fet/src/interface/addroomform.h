/***************************************************************************
                          addroomform.h  -  description
                             -------------------
    begin                : Sun Jan 4 2004
    copyright            : (C) 2004 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef ADDROOMFORM_H
#define ADDROOMFORM_H

#include "ui_addroomform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddRoomForm : public QDialog, Ui::AddRoomForm_template {
	Q_OBJECT
public:
	AddRoomForm(QWidget* parent);
	~AddRoomForm();

public Q_SLOTS:
	void addRoom();
	void help();
};

#endif
