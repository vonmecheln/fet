/***************************************************************************
                          addconstraintstudentsmaxhoursdailyform.cpp  -  description
                             -------------------
    begin                : July 19, 2007
    copyright            : (C) 2007 by Liviu Lalescu
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

#include <QMessageBox>

#include "longtextmessagebox.h"

#include "addconstraintstudentsmaxhoursdailyform.h"
#include "timeconstraint.h"

AddConstraintStudentsMaxHoursDailyForm::AddConstraintStudentsMaxHoursDailyForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintStudentsMaxHoursDailyForm::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintStudentsMaxHoursDailyForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	maxHoursSpinBox->setMinimum(1);
	maxHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
	maxHoursSpinBox->setValue(gt.rules.nHoursPerDay);
}

AddConstraintStudentsMaxHoursDailyForm::~AddConstraintStudentsMaxHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsMaxHoursDailyForm::addCurrentConstraint()
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

	ctr=new ConstraintStudentsMaxHoursDaily(weight, maxHours);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2){
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));

		gt.rules.addUndoPoint(tr("Added the constraint:\n\n%1").arg(ctr->getDetailedDescription(gt.rules)));
	}
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
