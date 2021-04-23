/***************************************************************************
                          modifyconstraintstudentssetminrestinghoursbetweenmorningandafternoonform.h  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef MODIFYCONSTRAINTSTUDENTSSETMINRESTINGHOURSBETWEENMORNINGANDAFTERNOONFORM_H
#define MODIFYCONSTRAINTSTUDENTSSETMINRESTINGHOURSBETWEENMORNINGANDAFTERNOONFORM_H

#include "ui_modifyconstraintstudentssetminrestinghoursbetweenmorningandafternoonform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoonForm : public QDialog, Ui::ModifyConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoonForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon* _ctr;

	ModifyConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoonForm(QWidget* parent, ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon* ctr);
	~ModifyConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoonForm();

	void updateStudentsComboBox(QWidget* parent);

public slots:
	void ok();
	void cancel();
};

#endif
