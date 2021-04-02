//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu <Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2020 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
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

public slots:
	void help();
	
	void ok();
	void cancel();
};

#endif
