/***************************************************************************
                          modifybuildingform.h  -  description
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

#ifndef MODIFYBUILDINGFORM_H
#define MODIFYBUILDINGFORM_H

#include "ui_modifybuildingform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyBuildingForm : public QDialog, Ui::ModifyBuildingForm_template {
	Q_OBJECT
	
	QString _initialBuildingName;
public:
	ModifyBuildingForm(QWidget* parent, const QString& initialBuildingName);
	~ModifyBuildingForm();

public Q_SLOTS:
	void ok();
	void cancel();
};

#endif
