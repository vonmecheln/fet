//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2020 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef RANDOMSEEDFORM_H
#define RANDOMSEEDFORM_H

#include "ui_randomseedform_template.h"

class RandomSeedForm : public QDialog, Ui::RandomSeedForm_template
{
	Q_OBJECT
public:
	RandomSeedForm(QWidget* parent);
	~RandomSeedForm();

public Q_SLOTS:
	void help();
	
	void ok();
	void cancel();
};

#endif
