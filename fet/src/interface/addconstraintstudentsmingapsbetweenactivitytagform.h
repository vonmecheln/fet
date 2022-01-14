/***************************************************************************
                          addconstraintstudentsmingapsbetweenactivitytagform.h  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Lalescu Liviu
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

#ifndef ADDCONSTRAINTSTUDENTSMINGAPSBETWEENACTIVITYTAGFORM_H
#define ADDCONSTRAINTSTUDENTSMINGAPSBETWEENACTIVITYTAGFORM_H

#include "ui_addconstraintstudentsmingapsbetweenactivitytagform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsMinGapsBetweenActivityTagForm : public QDialog, Ui::AddConstraintStudentsMinGapsBetweenActivityTagForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsMinGapsBetweenActivityTagForm(QWidget* parent);
	~AddConstraintStudentsMinGapsBetweenActivityTagForm();
	
	void updateActivityTagComboBox();

public slots:
	void addCurrentConstraint();
};

#endif
