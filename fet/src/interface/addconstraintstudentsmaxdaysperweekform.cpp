/***************************************************************************
                          addconstraintstudentsmaxdaysperweekform.cpp  -  description
                             -------------------
    begin                : 2013
    copyright            : (C) 2013 by Liviu Lalescu
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

#include "addconstraintstudentsmaxdaysperweekform.h"
#include "timeconstraint.h"

AddConstraintStudentsMaxDaysPerWeekForm::AddConstraintStudentsMaxDaysPerWeekForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintStudentsMaxDaysPerWeekForm::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintStudentsMaxDaysPerWeekForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	updateMaxDaysSpinBox();
}

AddConstraintStudentsMaxDaysPerWeekForm::~AddConstraintStudentsMaxDaysPerWeekForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsMaxDaysPerWeekForm::updateMaxDaysSpinBox(){
	maxDaysSpinBox->setMinimum(1);
	maxDaysSpinBox->setMaximum(gt.rules.nDaysPerWeek);
	maxDaysSpinBox->setValue(gt.rules.nDaysPerWeek);
}

void AddConstraintStudentsMaxDaysPerWeekForm::addCurrentConstraint()
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
	if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - it has to be 100%"));
		return;
	}

	int max_days=maxDaysSpinBox->value();
	
	ctr=new ConstraintStudentsMaxDaysPerWeek(weight, max_days);

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
