/***************************************************************************
                          modifyconstraintteachermingapsbetweenorderedpairofactivitytagsbetweenmorningandafternoonform.h  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTTEACHERMINGAPSBETWEENORDEREDPAIROFACTIVITYTAGSBETWEENMORNINGANDAFTERNOONFORM_H
#define MODIFYCONSTRAINTTEACHERMINGAPSBETWEENORDEREDPAIROFACTIVITYTAGSBETWEENMORNINGANDAFTERNOONFORM_H

#include "ui_modifyconstraintteachermingapsbetweenorderedpairofactivitytagsbetweenmorningandafternoonform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm : public QDialog, Ui::ModifyConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm_template  {
	Q_OBJECT
public:
	ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon* _ctr;

	ModifyConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm(QWidget* parent, ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon* ctr);
	~ModifyConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm();

	void updateTeacherComboBox();
	void updateFirstActivityTagComboBox();
	void updateSecondActivityTagComboBox();

public slots:
	void ok();
	void cancel();
	
	void swap();
};

#endif
