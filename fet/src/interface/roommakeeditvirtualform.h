//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2019 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef ROOMMAKEEDITVIRTUALFORM_H
#define ROOMMAKEEDITVIRTUALFORM_H

#include <QList>
#include <QStringList>

#include "room.h"

#include "ui_roommakeeditvirtualform_template.h"

class RoomMakeEditVirtualForm : public QDialog, Ui::RoomMakeEditVirtualForm_template
{
	Q_OBJECT
public:
	RoomMakeEditVirtualForm(QWidget* parent, Room* _editedRoom);
	~RoomMakeEditVirtualForm();
	
	Room* editedRoom;
	
	QList<QStringList> sets;
	
public Q_SLOTS:
	void addSet();
	void removeSet();
	void setChanged();
	
	void removeRoom();
	void clearRooms();
	
	void addRoom();
	void addAllRooms();

	void ok();
	void cancel();

	void help();
};

#endif
