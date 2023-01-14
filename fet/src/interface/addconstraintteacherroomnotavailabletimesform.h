/***************************************************************************
                          addconstraintteacherroomnotavailabletimesform.h  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef ADDCONSTRAINTTEACHERROOMNOTAVAILABLETIMESFORM_H
#define ADDCONSTRAINTTEACHERROOMNOTAVAILABLETIMESFORM_H

#include "ui_addconstraintteacherroomnotavailabletimesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintTeacherRoomNotAvailableTimesForm : public QDialog, Ui::AddConstraintTeacherRoomNotAvailableTimesForm_template {
	Q_OBJECT
public:
	AddConstraintTeacherRoomNotAvailableTimesForm(QWidget* parent);
	~AddConstraintTeacherRoomNotAvailableTimesForm();
	
	void updateTeachersComboBox();
	void updateRoomsComboBox();

	void colorItem(QTableWidgetItem* item);

public slots:
	void addConstraint();

	void itemClicked(QTableWidgetItem* item);
	void horizontalHeaderClicked(int col);
	void verticalHeaderClicked(int row);

	void setAllAvailable();
	void setAllNotAvailable();
};

#endif
