/***************************************************************************
                          addconstraintstudentsmaxhoursdailyrealdaysform.cpp  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QMessageBox>

#include "longtextmessagebox.h"

#include "addconstraintstudentsmaxhoursdailyrealdaysform.h"
#include "timeconstraint.h"

AddConstraintStudentsMaxHoursDailyRealDaysForm::AddConstraintStudentsMaxHoursDailyRealDaysForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	maxHoursSpinBox->setMinimum(1);
	maxHoursSpinBox->setMaximum(2*gt.rules.nHoursPerDay);
	maxHoursSpinBox->setValue(2*gt.rules.nHoursPerDay);
}

AddConstraintStudentsMaxHoursDailyRealDaysForm::~AddConstraintStudentsMaxHoursDailyRealDaysForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsMaxHoursDailyRealDaysForm::addCurrentConstraint()
{
	TimeConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	if(weight<100.0){
		int t=QMessageBox::warning(this, tr("FET warning"),
			tr("You selected a weight less than 100%. The generation algorithm is not perfectly optimized to work with such weights (even"
			 " if in practice it might work well). It is recommended to work only with 100% weights for these constraints. Are you sure you want to continue?"),
			 QMessageBox::Yes | QMessageBox::Cancel);
		if(t==QMessageBox::Cancel)
			return;
	}

	int maxHours=maxHoursSpinBox->value();

	ctr=new ConstraintStudentsMaxHoursDailyRealDays(weight, maxHours);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2)
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
