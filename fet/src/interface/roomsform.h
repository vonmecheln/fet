//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef ROOMSFORM_H
#define ROOMSFORM_H

#include "room.h"

#include "ui_roomsform_template.h"

class RoomsForm : public QDialog, Ui::RoomsForm_template
{
	Q_OBJECT
public:
	RoomsList visibleRoomsList;

	RoomsForm(QWidget* parent);
	~RoomsForm();

	bool filterOk(Room* rm);

public Q_SLOTS:
	void addRoom();
	void removeRoom();
	void modifyRoom();

	void moveRoomUp();
	void moveRoomDown();

	void sortRooms();
	void roomChanged(int index);
	void filterChanged();
	
	void longName();
	void code();
	void comments();
	
	void makeReal();
	void makeEditVirtual();
	
	void help();
};

#endif
