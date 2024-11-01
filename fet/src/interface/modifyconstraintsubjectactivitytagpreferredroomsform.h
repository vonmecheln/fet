/***************************************************************************
                          modifyconstraintsubjectactivitytagpreferredroomsform.h  -  description
                             -------------------
    begin                : Aug 18, 2007
    copyright            : (C) 2007 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTSUBJECTACTIVITYTAGPREFERREDROOMSFORM_H
#define MODIFYCONSTRAINTSUBJECTACTIVITYTAGPREFERREDROOMSFORM_H

#include "ui_modifyconstraintsubjectactivitytagpreferredroomsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintSubjectActivityTagPreferredRoomsForm : public QDialog, Ui::ModifyConstraintSubjectActivityTagPreferredRoomsForm_template  {
	Q_OBJECT
public:
	ModifyConstraintSubjectActivityTagPreferredRoomsForm(QWidget* parent, ConstraintSubjectActivityTagPreferredRooms* ctr);
	~ModifyConstraintSubjectActivityTagPreferredRoomsForm();

	void updateRoomsListWidget();
	
public slots:
	void addRoom();
	void removeRoom();

	void ok();
	void cancel();
	
	void clear();
private:
	ConstraintSubjectActivityTagPreferredRooms* _ctr;
};

#endif
