/***************************************************************************
                          addconstraintstudentssetmingapsbetweenactivitytagform.h  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Liviu Lalescu
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

#ifndef ADDCONSTRAINTSTUDENTSSETMINGAPSBETWEENACTIVITYTAGFORM_H
#define ADDCONSTRAINTSTUDENTSSETMINGAPSBETWEENACTIVITYTAGFORM_H

#include "ui_addconstraintstudentssetmingapsbetweenactivitytagform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsSetMinGapsBetweenActivityTagForm : public QDialog, Ui::AddConstraintStudentsSetMinGapsBetweenActivityTagForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsSetMinGapsBetweenActivityTagForm(QWidget* parent);
	~AddConstraintStudentsSetMinGapsBetweenActivityTagForm();
	
	void updateStudentsSetComboBox();
	void updateActivityTagComboBox();

public slots:
	void addCurrentConstraint();
};

#endif
