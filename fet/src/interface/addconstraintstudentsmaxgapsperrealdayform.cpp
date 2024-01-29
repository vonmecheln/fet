/***************************************************************************
                          addconstraintstudentsmaxgapsperrealdayform.cpp  -  description
                             -------------------
    begin                : 2018
    copyright            : (C) 2018 by Liviu Lalescu
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

#include "addconstraintstudentsmaxgapsperrealdayform.h"
#include "timeconstraint.h"

AddConstraintStudentsMaxGapsPerRealDayForm::AddConstraintStudentsMaxGapsPerRealDayForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintStudentsMaxGapsPerRealDayForm::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintStudentsMaxGapsPerRealDayForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	maxGapsSpinBox->setMinimum(0);
	maxGapsSpinBox->setMaximum(2*gt.rules.nHoursPerDay);
	maxGapsSpinBox->setValue(2*gt.rules.nHoursPerDay);
}

AddConstraintStudentsMaxGapsPerRealDayForm::~AddConstraintStudentsMaxGapsPerRealDayForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsMaxGapsPerRealDayForm::addCurrentConstraint()
{
	TimeConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight(percentage)"));
		return;
	}
	if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight(percentage) - it must be 100%"));
		return;
	}

	ctr=new ConstraintStudentsMaxGapsPerRealDay(weight, maxGapsSpinBox->value());

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
