/***************************************************************************
                          addconstraintteachersmaxgapsperrealdayform.cpp  -  description
                             -------------------
    begin                : 2018
    copyright            : (C) 2018 by Lalescu Liviu
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

#include "addconstraintteachersmaxgapsperrealdayform.h"
#include "timeconstraint.h"

AddConstraintTeachersMaxGapsPerRealDayForm::AddConstraintTeachersMaxGapsPerRealDayForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	maxGapsSpinBox->setMinimum(0);
	maxGapsSpinBox->setMaximum(2*gt.rules.nHoursPerDay);
	maxGapsSpinBox->setValue(2*gt.rules.nHoursPerDay);
	
	exceptionCheckBox->setChecked(false);
}

AddConstraintTeachersMaxGapsPerRealDayForm::~AddConstraintTeachersMaxGapsPerRealDayForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeachersMaxGapsPerRealDayForm::addCurrentConstraint()
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
			tr("Invalid weight (percentage) - it must be 100%"));
		return;
	}

	ctr=new ConstraintTeachersMaxGapsPerRealDay(weight, maxGapsSpinBox->value(), exceptionCheckBox->isChecked());

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