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

#include "roomsform_template.h"

/**
@author Liviu Lalescu
*/
class RoomsForm : public RoomsForm_template
{
public:
	RoomsList visibleRoomsList;

	RoomsForm();
	~RoomsForm();

	void addRoom();
	void removeRoom();
	void modifyRoom();
	void sortRooms();
	void roomChanged(int index);
	void roomsEquipments();
	void filterChanged();
	bool filterOk(Room* rm);
};

#endif
