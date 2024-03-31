/***************************************************************************
                          modifyconstraintstudentsmingapsbetweenorderedpairofactivitytagsperrealdayform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSMINGAPSBETWEENORDEREDPAIROFACTIVITYTAGSPERREALDAYFORM_H
#define MODIFYCONSTRAINTSTUDENTSMINGAPSBETWEENORDEREDPAIROFACTIVITYTAGSPERREALDAYFORM_H

#include "ui_modifyconstraintstudentsmingapsbetweenorderedpairofactivitytagsperrealdayform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDayForm : public QDialog, Ui::ModifyConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDayForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay* _ctr;

	ModifyConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDayForm(QWidget* parent, ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay* ctr);
	~ModifyConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDayForm();

	void updateFirstActivityTagComboBox();
	void updateSecondActivityTagComboBox();

public slots:
	void ok();
	void cancel();
	
	void swap();
};

#endif
