/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                          lockunlock.h  -  description
                             -------------------
    begin                : Dec 2008
    copyright            : (C) by Liviu Lalescu (http://lalescu.ro/liviu/) and Volker Dirr (http://www.timetabling.de/)
 ***************************************************************************
 *                                                                         *
 *   FET program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LOCKUNLOCK_H
#define LOCKUNLOCK_H

#include <QObject>

class LockUnlock: public QObject{
	Q_OBJECT

public:
	static void lockAll();
	static void unlockAll();
	
	static void lockDay();
	static void unlockDay();
	
	static void lockEndStudentsDay();
	static void unlockEndStudentsDay();
	
	static void computeLockedUnlockedActivitiesTimeSpace();
	static void computeLockedUnlockedActivitiesOnlyTime();
	static void computeLockedUnlockedActivitiesOnlySpace();
	
	static void increaseCommunicationSpinBox();
};


#endif
