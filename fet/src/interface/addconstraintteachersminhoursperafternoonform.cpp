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

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintTeachersMinHoursPerAfternoonForm::addCurrentConstraint);
	connect(addConstraintsPushButton, &QPushButton::clicked, this, &AddConstraintTeachersMinHoursPerAfternoonForm::addCurrentConstraints);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintTeachersMinHoursPerAfternoonForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	updateMinHoursSpinBox();

	connect(allowEmptyAfternoonsCheckBox, &QCheckBox::toggled, this, &AddConstraintTeachersMinHoursPerAfternoonForm::allowEmptyAfternoonsCheckBox_toggled);
	
	allowEmptyAfternoonsCheckBox_toggled();
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

void AddConstraintTeachersMinHoursPerAfternoonForm::addCurrentConstraints()
{
	QMessageBox::StandardButton res=QMessageBox::question(this, tr("FET confirmation"),
	 tr("This operation will add multiple constraints, one for each teacher. Do you want to continue?"),
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

	QString ctrs;
	for(Teacher* tch : std::as_const(gt.rules.teachersList)){
		TimeConstraint *ctr=new ConstraintTeacherMinHoursPerAfternoon(weight, min_hours, tch->name, allowEmptyAfternoonsCheckBox->isChecked());
		bool tmp2=gt.rules.addTimeConstraint(ctr);
		assert(tmp2);

		ctrs+=ctr->getDetailedDescription(gt.rules);
		ctrs+="\n";
	}

	QMessageBox::information(this, tr("FET information"), tr("Added %1 time constraints. Please note that these constraints"
	 " will be visible as constraints for individual teachers.").arg(gt.rules.teachersList.count()));

	if(gt.rules.teachersList.count()>0)
		gt.rules.addUndoPoint(tr("Added %1 constraints, one for each teacher:\n\n%2", "%1 is the number of constraints, %2 is their detailed description")
						  .arg(gt.rules.teachersList.count()).arg(ctrs));
}

void AddConstraintTeachersMinHoursPerAfternoonForm::allowEmptyAfternoonsCheckBox_toggled()
{
	bool k=allowEmptyAfternoonsCheckBox->isChecked();

	if(!k){
		allowEmptyAfternoonsCheckBox->setChecked(true);
		QMessageBox::information(this, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty afternoons for the teachers,"
			" please use constraint teachers min afternoons per week."));
	}
}
