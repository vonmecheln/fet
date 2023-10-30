/***************************************************************************
                          addconstraintteachersminhoursperafternoonform.cpp  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Liviu Lalescu
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

#include "addconstraintteachersminhoursperafternoonform.h"
#include "timeconstraint.h"

AddConstraintTeachersMinHoursPerAfternoonForm::AddConstraintTeachersMinHoursPerAfternoonForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
	connect(addConstraintsPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraints()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	updateMinHoursSpinBox();
}

AddConstraintTeachersMinHoursPerAfternoonForm::~AddConstraintTeachersMinHoursPerAfternoonForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeachersMinHoursPerAfternoonForm::updateMinHoursSpinBox(){
	minHoursSpinBox->setMinimum(2);
	minHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minHoursSpinBox->setValue(2);
}

void AddConstraintTeachersMinHoursPerAfternoonForm::addCurrentConstraint()
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

	if(!allowEmptyAfternoonsCheckBox->isChecked()){
		QMessageBox::warning(this, tr("FET information"), tr("Allow empty afternoons check box must be checked. If you need to not allow empty afternoons for the teachers, "
			"please use the constraint teachers min afternoons per week."));
		return;
	}

	int min_hours=minHoursSpinBox->value();

	ctr=new ConstraintTeachersMinHoursPerAfternoon(weight, min_hours, allowEmptyAfternoonsCheckBox->isChecked());

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

void AddConstraintTeachersMinHoursPerAfternoonForm::addCurrentConstraints()
{
	QMessageBox::StandardButton res=QMessageBox::question(this, tr("FET question"),
	 tr("Warning: This operation will add multiple constraints, one for each teacher. Are you sure?"),
	 QMessageBox::Cancel | QMessageBox::Yes);
	if(res==QMessageBox::Cancel)
		return;

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

	if(!allowEmptyAfternoonsCheckBox->isChecked()){
		QMessageBox::warning(this, tr("FET information"), tr("Allow empty afternoons check box must be checked. If you need to not allow empty afternoons for the teachers, "
			"please use the constraint teachers min afternoons per week."));
		return;
	}

	int min_hours=minHoursSpinBox->value();

	for(Teacher* tch : std::as_const(gt.rules.teachersList)){
		TimeConstraint *ctr=new ConstraintTeacherMinHoursPerAfternoon(weight, min_hours, tch->name, allowEmptyAfternoonsCheckBox->isChecked());
		bool tmp2=gt.rules.addTimeConstraint(ctr);
		assert(tmp2);
	}

	QMessageBox::information(this, tr("FET information"), tr("Added %1 time constraints. Please note that these constraints"
	 " will be visible as constraints for individual teachers.").arg(gt.rules.teachersList.count()));
}

void AddConstraintTeachersMinHoursPerAfternoonForm::on_allowEmptyAfternoonsCheckBox_toggled()
{
	bool k=allowEmptyAfternoonsCheckBox->isChecked();

	if(!k){
		allowEmptyAfternoonsCheckBox->setChecked(true);
		QMessageBox::information(this, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty afternoons for the teachers,"
			" please use constraint teachers min afternoons per week."));
	}
}
