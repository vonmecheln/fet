/***************************************************************************
                          modifyconstraintteachermingapsbetweenactivitytagform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERMINGAPSBETWEENACTIVITYTAGFORM_H
#define MODIFYCONSTRAINTTEACHERMINGAPSBETWEENACTIVITYTAGFORM_H

#include "ui_modifyconstraintteachermingapsbetweenactivitytagform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMinGapsBetweenActivityTagForm : public QDialog, Ui::ModifyConstraintTeacherMinGapsBetweenActivityTagForm_template  {
	Q_OBJECT
public:
	ConstraintTeacherMinGapsBetweenActivityTag* _ctr;

	ModifyConstraintTeacherMinGapsBetweenActivityTagForm(QWidget* parent, ConstraintTeacherMinGapsBetweenActivityTag* ctr);
	~ModifyConstraintTeacherMinGapsBetweenActivityTagForm();

	void updateTeacherComboBox();
	void updateActivityTagComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
