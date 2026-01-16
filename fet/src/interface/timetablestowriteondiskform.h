//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2016 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef TIMETABLESTOWRITEONDISKFORM_H
#define TIMETABLESTOWRITEONDISKFORM_H

#include "ui_timetablestowriteondiskform_template.h"

class TimetablesToWriteOnDiskForm:public QDialog, Ui::TimetablesToWriteOnDiskForm_template
{
	Q_OBJECT

public:
	TimetablesToWriteOnDiskForm(QWidget* parent);
	~TimetablesToWriteOnDiskForm();
	
public Q_SLOTS:
	void wasAccepted();
	void wasCanceled();
};

#endif
