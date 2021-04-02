/***************************************************************************
                          modifybuildingform.h  -  description
                             -------------------
    begin                : July 9, 2005
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

#ifndef MODIFYBUILDINGFORM_H
#define MODIFYBUILDINGFORM_H

#include "modifybuildingform_template.h"

#include "genetictimetable_defs.h"
#include "genetictimetable.h"
#include "fet.h"

#include "fetmainform.h"

#include <qspinbox.h>
#include <qlineedit.h>
#include <q3textedit.h>


class ModifyBuildingForm : public ModifyBuildingForm_template {
	QString _initialBuildingName;
public:
	ModifyBuildingForm(const QString& initialBuildingName);
	~ModifyBuildingForm();

	void ok();
	void cancel();
};

#endif
