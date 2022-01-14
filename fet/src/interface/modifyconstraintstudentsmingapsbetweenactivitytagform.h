/***************************************************************************
                          modifyconstraintstudentsmingapsbetweenactivitytagform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSMINGAPSBETWEENACTIVITYTAGFORM_H
#define MODIFYCONSTRAINTSTUDENTSMINGAPSBETWEENACTIVITYTAGFORM_H

#include "ui_modifyconstraintstudentsmingapsbetweenactivitytagform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMinGapsBetweenActivityTagForm : public QDialog, Ui::ModifyConstraintStudentsMinGapsBetweenActivityTagForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMinGapsBetweenActivityTag* _ctr;

	ModifyConstraintStudentsMinGapsBetweenActivityTagForm(QWidget* parent, ConstraintStudentsMinGapsBetweenActivityTag* ctr);
	~ModifyConstraintStudentsMinGapsBetweenActivityTagForm();

	void updateActivityTagComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
