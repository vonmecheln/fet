/***************************************************************************
                          modifyroomform.h  -  description
                             -------------------
    begin                : Feb 12, 2005
    copyright            : (C) 2005 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MODIFYROOMFORM_H
#define MODIFYROOMFORM_H

#include "modifyroomform_template.h"

#include "genetictimetable_defs.h"
#include "genetictimetable.h"
#include "fet.h"

#include "fetmainform.h"

#include <qspinbox.h>
#include <qlineedit.h>
#include <q3textedit.h>


class ModifyRoomForm : public ModifyRoomForm_template {
	QString _initialRoomName;
	QString _initialRoomType;
	QString _initialRoomBuilding;
	int _initialRoomCapacity;
public: 
	ModifyRoomForm(const QString& initialRoomName, const QString& initialRoomType, const QString& initialRoomBuilding, int initialRoomCapacity);
	~ModifyRoomForm();

	void ok();
	void cancel();
};

#endif
