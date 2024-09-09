/***************************************************************************
                          modifyconstraintstudentsmaxactivitytagsperdayfromsetform.h  -  description
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

#ifndef MODIFYCONSTRAINTSTUDENTSMAXACTIVITYTAGSPERDAYFROMSETFORM_H
#define MODIFYCONSTRAINTSTUDENTSMAXACTIVITYTAGSPERDAYFROMSETFORM_H

#include "ui_modifyconstraintstudentsmaxactivitytagsperdayfromsetform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintStudentsMaxActivityTagsPerDayFromSetForm : public QDialog, Ui::ModifyConstraintStudentsMaxActivityTagsPerDayFromSetForm_template  {
	Q_OBJECT
public:
	ConstraintStudentsMaxActivityTagsPerDayFromSet* _ctr;

	ModifyConstraintStudentsMaxActivityTagsPerDayFromSetForm(QWidget* parent, ConstraintStudentsMaxActivityTagsPerDayFromSet* ctr);
	~ModifyConstraintStudentsMaxActivityTagsPerDayFromSetForm();

public slots:
	void addActivityTag();
	void removeActivityTag();
	void clearActivityTags();

	void ok();
	void cancel();
};

#endif
