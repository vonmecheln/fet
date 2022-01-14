/***************************************************************************
                          modifyconstraintteachersmingapsbetweenactivitytagform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERSMINGAPSBETWEENACTIVITYTAGFORM_H
#define MODIFYCONSTRAINTTEACHERSMINGAPSBETWEENACTIVITYTAGFORM_H

#include "ui_modifyconstraintteachersmingapsbetweenactivitytagform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersMinGapsBetweenActivityTagForm : public QDialog, Ui::ModifyConstraintTeachersMinGapsBetweenActivityTagForm_template  {
	Q_OBJECT
public:
	ConstraintTeachersMinGapsBetweenActivityTag* _ctr;

	ModifyConstraintTeachersMinGapsBetweenActivityTagForm(QWidget* parent, ConstraintTeachersMinGapsBetweenActivityTag* ctr);
	~ModifyConstraintTeachersMinGapsBetweenActivityTagForm();

	void updateActivityTagComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
