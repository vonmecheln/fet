/***************************************************************************
                          modifyequipmentform.h  -  description
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

#ifndef MODIFYEQUIPMENTFORM_H
#define MODIFYEQUIPMENTFORM_H

#include "modifyequipmentform_template.h"

#include "genetictimetable_defs.h"
#include "genetictimetable.h"
#include "fet.h"

#include "fetmainform.h"

#include <qspinbox.h>
#include <qlineedit.h>
#include <q3textedit.h>


class ModifyEquipmentForm : public ModifyEquipmentForm_template {
	QString _initialEquipmentName;
public:
	ModifyEquipmentForm(const QString& initialEquipmentName);
	~ModifyEquipmentForm();

	void ok();
	void cancel();
};

#endif
