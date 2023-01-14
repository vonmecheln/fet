/***************************************************************************
                          modifyconstraintactivitytagsnotoverlappingform.h  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Liviu Lalescu
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

#ifndef MODIFYCONSTRAINTACTIVITYTAGSNOTOVERLAPPINGFORM_H
#define MODIFYCONSTRAINTACTIVITYTAGSNOTOVERLAPPINGFORM_H

#include "ui_modifyconstraintactivitytagsnotoverlappingform_template.h"
#include "timetable_defs.h"
#include "timetable.h"

#include <QList>

class ModifyConstraintActivityTagsNotOverlappingForm : public QDialog, Ui::ModifyConstraintActivityTagsNotOverlappingForm_template  {
	Q_OBJECT
public:
	ModifyConstraintActivityTagsNotOverlappingForm(QWidget* parent, ConstraintActivityTagsNotOverlapping* ctr);
	~ModifyConstraintActivityTagsNotOverlappingForm();

public slots:
	void addActivityTag();
	void addAllActivityTags();
	void removeActivityTag();

	void clear();

	void ok();
	void cancel();
	
private:
	ConstraintActivityTagsNotOverlapping* _ctr;
};

#endif
