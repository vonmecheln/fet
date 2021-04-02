//
//
// C++ Interface: $MODULE$
//
// Description: 
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ROOMSFORM_H
#define ROOMSFORM_H

#include "room.h"

#include "ui_roomsform_template.h"

/**
@author Liviu Lalescu
*/
class RoomsForm : public QDialog, Ui::RoomsForm_template
{
	Q_OBJECT
public:
	RoomsList visibleRoomsList;

	RoomsForm();
	~RoomsForm();

	bool filterOk(Room* rm);

public slots:
	void addRoom();
	void removeRoom();
	void modifyRoom();
	void sortRooms();
	void roomChanged(int index);
	void filterChanged();
};

#endif
