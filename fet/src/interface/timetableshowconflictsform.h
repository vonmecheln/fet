//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef TIMETABLESHOWCONFLICTSFORM_H
#define TIMETABLESHOWCONFLICTSFORM_H

#include "ui_timetableshowconflictsform_template.h"

class TimetableShowConflictsForm : public QDialog, public Ui::TimetableShowConflictsForm_template
{
	Q_OBJECT
	
public:
	TimetableShowConflictsForm(QWidget* parent);
	void newTimetableGenerated();
	~TimetableShowConflictsForm();
};

#endif
