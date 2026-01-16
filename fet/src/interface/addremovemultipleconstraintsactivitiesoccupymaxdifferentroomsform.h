/***************************************************************************
                          addremovemultipleconstraintsactivitiesoccupymaxdifferentroomsform.h  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef ADDREMOVEMULTIPLECONSTRAINTSACTIVITIESOCCUPYMAXDIFFERENTROOMSFORM_H
#define ADDREMOVEMULTIPLECONSTRAINTSACTIVITIESOCCUPYMAXDIFFERENTROOMSFORM_H

#include "ui_addremovemultipleconstraintsactivitiesoccupymaxdifferentroomsform_template.h"

class AddRemoveMultipleConstraintsActivitiesOccupyMaxDifferentRoomsForm : public QDialog, Ui::AddRemoveMultipleConstraintsActivitiesOccupyMaxDifferentRoomsForm_template{
	Q_OBJECT
	
public:
	AddRemoveMultipleConstraintsActivitiesOccupyMaxDifferentRoomsForm(QWidget* parent);
	~AddRemoveMultipleConstraintsActivitiesOccupyMaxDifferentRoomsForm();

public Q_SLOTS:
	void addAllConstraints();
	void removeAllConstraints();
	void help();
};

#endif
