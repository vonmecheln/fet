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

#ifndef ROOM_H
#define ROOM_H

#include <QCoreApplication>

#include "timetable_defs.h"

#include <QString>
#include <QList>
#include <QStringList>

class Room;
class Rules;

typedef QList<Room*> RoomsList;

class QDataStream;

QDataStream& operator<<(QDataStream& stream, const Room& rm);
QDataStream& operator>>(QDataStream& stream, Room& rm);

/**
This class represents a room

@author Liviu Lalescu
*/
class Room{ /*classroom :-)*/
	Q_DECLARE_TR_FUNCTIONS(Room)

public:
	bool isVirtual; //If it is virtual, it has a list of sets of real rooms. Suggested by math.

	QString name;
	QString longName;
	QString code;
	int capacity;
	
	/**
	If empty string, it is ignored.
	*/
	QString building;
	
	QString comments;
	
	QList<QStringList> realRoomsSetsList; //Not internal. Contains the rooms' names.
	
	/**
	Internal. If -1, it is ignored.
	*/
	int buildingIndex;

	QList<QList<int>> rrsl; //Internal. Contains the rooms' indices.

	Room();
	~Room();
	
	void computeInternalStructure(Rules& r);
	void computeInternalStructureRealRoomsSetsList(Rules& r);

	QString getXmlDescription();
	QString getDescription();
	QString getDetailedDescription();
	QString getDetailedDescriptionWithConstraints(Rules& r);
};

int roomsAscending(const Room* r1, const Room* r2);

int roomsCodesAscending(const Room* r1, const Room* r2);

#endif
