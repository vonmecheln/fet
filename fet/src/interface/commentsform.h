//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2005 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef COMMENTSFORM_H
#define COMMENTSFORM_H

#include "ui_commentsform_template.h"

class CommentsForm : public QDialog, Ui::CommentsForm_template
{
	Q_OBJECT
public:
	CommentsForm(QWidget* parent);

	~CommentsForm();

public Q_SLOTS:
	void ok();
	void cancel();
};

#endif
