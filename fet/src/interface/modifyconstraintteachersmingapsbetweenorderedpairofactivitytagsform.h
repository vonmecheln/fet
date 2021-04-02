/***************************************************************************
                          modifyconstraintteachersmingapsbetweenorderedpairofactivitytagsform.h  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Lalescu Liviu
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

#ifndef MODIFYCONSTRAINTTEACHERSMINGAPSBETWEENORDEREDPAIROFACTIVITYTAGSFORM_H
#define MODIFYCONSTRAINTTEACHERSMINGAPSBETWEENORDEREDPAIROFACTIVITYTAGSFORM_H

#include "ui_modifyconstraintteachersmingapsbetweenorderedpairofactivitytagsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm : public QDialog, Ui::ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm_template  {
	Q_OBJECT
public:
	ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags* _ctr;

	ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm(QWidget* parent, ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags* ctr);
	~ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm();

	void updateFirstActivityTagComboBox();
	void updateSecondActivityTagComboBox();

public slots:
	void ok();
	void cancel();
};

#endif
