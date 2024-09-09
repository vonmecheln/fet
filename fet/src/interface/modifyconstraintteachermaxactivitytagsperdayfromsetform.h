/***************************************************************************
                          modifyconstraintteachermaxactivitytagsperdayfromsetform.h  -  description
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

#ifndef MODIFYCONSTRAINTTEACHERMAXACTIVITYTAGSPERDAYFROMSETFORM_H
#define MODIFYCONSTRAINTTEACHERMAXACTIVITYTAGSPERDAYFROMSETFORM_H

#include "ui_modifyconstraintteachermaxactivitytagsperdayfromsetform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm : public QDialog, Ui::ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm_template  {
	Q_OBJECT
public:
	ConstraintTeacherMaxActivityTagsPerDayFromSet* _ctr;

	ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm(QWidget* parent, ConstraintTeacherMaxActivityTagsPerDayFromSet* ctr);
	~ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm();

public slots:
	void addActivityTag();
	void removeActivityTag();
	void clearActivityTags();

	void ok();
	void cancel();
};

#endif
