/***************************************************************************
                          addconstraintteachersmaxafternoonsperweekform.cpp  -  description
                             -------------------
    begin                : June 2009
    copyright            : (C) 2009 by Liviu Lalescu
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

#include "addconstraintteachersmaxafternoonsperweekform.h"
#include "timeconstraint.h"

AddConstraintTeachersMaxAfternoonsPerWeekForm::AddConstraintTeachersMaxAfternoonsPerWeekForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintTeachersMaxAfternoonsPerWeekForm::addCurrentConstraint);
	connect(addConstraintsPushButton, &QPushButton::clicked, this, &AddConstraintTeachersMaxAfternoonsPerWeekForm::addCurrentConstraints);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintTeachersMaxAfternoonsPerWeekForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	updateMaxAfternoonsSpinBox();
}

AddConstraintTeachersMaxAfternoonsPerWeekForm::~AddConstraintTeachersMaxAfternoonsPerWeekForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeachersMaxAfternoonsPerWeekForm::updateMaxAfternoonsSpinBox(){
	maxAfternoonsSpinBox->setMinimum(1);
	maxAfternoonsSpinBox->setMaximum(gt.rules.nDaysPerWeek/2);
	maxAfternoonsSpinBox->setValue(gt.rules.nDaysPerWeek/2);
}

void AddConstraintTeachersMaxAfternoonsPerWeekForm::addCurrentConstraint()
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
	
	int max_afternoons=maxAfternoonsSpinBox->value();

	ctr=new ConstraintTeachersMaxAfternoonsPerWeek(weight, max_afternoons);

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

void AddConstraintTeachersMaxAfternoonsPerWeekForm::addCurrentConstraints()
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
			tr("Invalid weight (percentage) - it has to be 100%"));
		return;
	}

	int max_afternoons=maxAfternoonsSpinBox->value();

	QString ctrs;
	for(Teacher* tch : std::as_const(gt.rules.teachersList)){
		TimeConstraint *ctr=new ConstraintTeacherMaxAfternoonsPerWeek(weight, max_afternoons, tch->name);
		bool tmp2=gt.rules.addTimeConstraint(ctr);
		assert(tmp2);

		ctrs+=ctr->getDetailedDescription(gt.rules);
		ctrs+="\n";
	}

	QMessageBox::information(this, tr("FET information"), tr("Added %1 time constraints. Please note that these constraints"
	 " will be visible as constraints for individual teachers.").arg(gt.rules.teachersList.count()));

	gt.rules.addUndoPoint(tr("Added %1 constraints, one for each teacher:\n\n%2", "%1 is the number of constraints, %2 is their detailed description")
						  .arg(gt.rules.teachersList.count()).arg(ctrs));
}
