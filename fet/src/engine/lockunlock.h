/*
File lockunlock.h
*/

/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************
                          lockunlock.h  -  description
                             -------------------
    begin                : Dec 2008
    copyright            : (C) by Liviu Lalescu (https://lalescu.ro/liviu/) and Volker Dirr (https://www.timetabling.de/)
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef LOCKUNLOCK_H
#define LOCKUNLOCK_H

#include <QObject>

class CommunicationSpinBox: public QObject{
	Q_OBJECT

private:
	int value;
	int maxValue;
	int minValue;
	
public:
	CommunicationSpinBox();
	~CommunicationSpinBox();
	
Q_SIGNALS:
	int valueChanged(int newValue);
	
public Q_SLOTS:
	void increaseValue();
};

class LockUnlock{
public:
	static void computeLockedUnlockedActivitiesTimeSpace();
	static void computeLockedUnlockedActivitiesOnlyTime();
	static void computeLockedUnlockedActivitiesOnlySpace();
	
	static void increaseCommunicationSpinBox();
};

#endif
