/***************************************************************************
                          addconstraintstudentssetmaxactivitytagsperdayfromsetform.h  -  description
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

#ifndef ADDCONSTRAINTSTUDENTSSETMAXACTIVITYTAGSPERDAYFROMSETFORM_H
#define ADDCONSTRAINTSTUDENTSSETMAXACTIVITYTAGSPERDAYFROMSETFORM_H

#include "ui_addconstraintstudentssetmaxactivitytagsperdayfromsetform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm : public QDialog, Ui::AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm_template  {
	Q_OBJECT
public:
	AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm(QWidget* parent);
	~AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm();

public slots:
	void addActivityTag();
	void removeActivityTag();
	void clearActivityTags();

	void addCurrentConstraint();
};

#endif
