/***************************************************************************
                          addconstraintstudentssetmaxgapsperdayform.h  -  description
                             -------------------
    begin                : 2018
    copyright            : (C) 2018 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTSTUDENTSSETMAXGAPSPERREALDAYFORM_H
#define ADDCONSTRAINTSTUDENTSSETMAXGAPSPERREALDAYFORM_H

#include "ui_addconstraintstudentssetmaxgapsperrealdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsSetMaxGapsPerRealDayForm : public QDialog, Ui::AddConstraintStudentsSetMaxGapsPerRealDayForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsSetMaxGapsPerRealDayForm(QWidget* parent);
	~AddConstraintStudentsSetMaxGapsPerRealDayForm();

	void updateStudentsSetComboBox();

public slots:
	void addCurrentConstraint();
};

#endif
