/***************************************************************************
                          modifyroomform.h  -  description
                             -------------------
    begin                : Feb 12, 2005
    copyright            : (C) 2005 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef MODIFYROOMFORM_H
#define MODIFYROOMFORM_H

#include "ui_modifyroomform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyRoomForm : public QDialog, Ui::ModifyRoomForm_template {
	Q_OBJECT
	
	QString _initialRoomName;
	QString _initialRoomBuilding;
	int _initialRoomCapacity;
public:
	ModifyRoomForm(QWidget* parent, const QString& initialRoomName, const QString& initialRoomBuilding, int initialRoomCapacity);
	~ModifyRoomForm();

public Q_SLOTS:
	void ok();
	void cancel();
};

#endif
