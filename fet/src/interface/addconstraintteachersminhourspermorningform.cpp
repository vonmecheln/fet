/***************************************************************************
                          addconstraintteachersminhourspermorningform.cpp  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Lalescu Liviu
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

#include "addconstraintteachersminhourspermorningform.h"
#include "timeconstraint.h"

AddConstraintTeachersMinHoursPerMorningForm::AddConstraintTeachersMinHoursPerMorningForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	updateMinHoursSpinBox();
}

AddConstraintTeachersMinHoursPerMorningForm::~AddConstraintTeachersMinHoursPerMorningForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeachersMinHoursPerMorningForm::updateMinHoursSpinBox(){
	minHoursSpinBox->setMinimum(2);
	minHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minHoursSpinBox->setValue(2);
}

void AddConstraintTeachersMinHoursPerMorningForm::addCurrentConstraint()
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
			tr("Invalid weight (percentage) - must be 100%"));
		return;
	}

	if(!allowEmptyMorningsCheckBox->isChecked()){
		QMessageBox::warning(this, tr("FET information"), tr("Allow empty mornings check box must be checked. If you need to not allow empty mornings for the teachers, "
			"please use the constraint teachers min mornings per week."));
		return;
	}

	int min_hours=minHoursSpinBox->value();

	ctr=new ConstraintTeachersMinHoursPerMorning(weight, min_hours, allowEmptyMorningsCheckBox->isChecked());

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

void AddConstraintTeachersMinHoursPerMorningForm::on_allowEmptyMorningsCheckBox_toggled()
{
	bool k=allowEmptyMorningsCheckBox->isChecked();

	if(!k){
		allowEmptyMorningsCheckBox->setChecked(true);
		QMessageBox::information(this, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty mornings for the teachers,"
			" please use constraint teachers min mornings per week."));
	}
}
