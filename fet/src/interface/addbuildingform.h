/***************************************************************************
                          addbuildingform.h  -  description
                             -------------------
    begin                : Feb 11, 2008
    copyright            : (C) 2008 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef ADDBUILDINGFORM_H
#define ADDBUILDINGFORM_H

#include "ui_addbuildingform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddBuildingForm : public QDialog, Ui::AddBuildingForm_template {
	Q_OBJECT
public:
	AddBuildingForm(QWidget* parent);
	~AddBuildingForm();

public Q_SLOTS:
	void addBuilding();
};

#endif
