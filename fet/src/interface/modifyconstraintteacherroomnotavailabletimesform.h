/***************************************************************************
                          modifyconstraintteacherroomnotavailabletimesform.h  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTTEACHERROOMNOTAVAILABLETIMESFORM_H
#define MODIFYCONSTRAINTTEACHERROOMNOTAVAILABLETIMESFORM_H

#include "ui_modifyconstraintteacherroomnotavailabletimesform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherRoomNotAvailableTimesForm : public QDialog, Ui::ModifyConstraintTeacherRoomNotAvailableTimesForm_template  {
	Q_OBJECT

	ConstraintTeacherRoomNotAvailableTimes* _ctr;
public:
	ModifyConstraintTeacherRoomNotAvailableTimesForm(QWidget* parent, ConstraintTeacherRoomNotAvailableTimes* ctr);
	~ModifyConstraintTeacherRoomNotAvailableTimesForm();
	
	void updateTeachersComboBox();
	void updateRoomsComboBox();

	void colorItem(QTableWidgetItem* item);

public slots:
	void ok();
	void cancel();

	void itemClicked(QTableWidgetItem* item);
	void horizontalHeaderClicked(int col);
	void verticalHeaderClicked(int row);

	void cellEntered(int row, int col);
	
	void setAllAvailable();
	void setAllNotAvailable();
};

#endif
