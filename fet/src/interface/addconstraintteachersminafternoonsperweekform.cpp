/***************************************************************************
                          addconstraintteachersminafternoonsperweekform.cpp  -  description
                             -------------------
    begin                : 2009
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

#include "addconstraintteachersminafternoonsperweekform.h"
#include "timeconstraint.h"

AddConstraintTeachersMinAfternoonsPerWeekForm::AddConstraintTeachersMinAfternoonsPerWeekForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
	connect(addConstraintsPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraints()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	updateMinAfternoonsSpinBox();
}

AddConstraintTeachersMinAfternoonsPerWeekForm::~AddConstraintTeachersMinAfternoonsPerWeekForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeachersMinAfternoonsPerWeekForm::updateMinAfternoonsSpinBox(){
	minAfternoonsSpinBox->setMinimum(1);
	minAfternoonsSpinBox->setMaximum(gt.rules.nDaysPerWeek/2);
	minAfternoonsSpinBox->setValue(1);
}

void AddConstraintTeachersMinAfternoonsPerWeekForm::addCurrentConstraint()
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

	int min_afternoons=minAfternoonsSpinBox->value();

	ctr=new ConstraintTeachersMinAfternoonsPerWeek(weight, min_afternoons);

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

void AddConstraintTeachersMinAfternoonsPerWeekForm::addCurrentConstraints()
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
			tr("Invalid weight (percentage) - it has to be 100%"));
		return;
	}

	int min_afternoons=minAfternoonsSpinBox->value();

	for(Teacher* tch : std::as_const(gt.rules.teachersList)){
		TimeConstraint *ctr=new ConstraintTeacherMinAfternoonsPerWeek(weight, min_afternoons, tch->name);
		bool tmp2=gt.rules.addTimeConstraint(ctr);
		assert(tmp2);
	}

	QMessageBox::information(this, tr("FET information"), tr("Added %1 time constraints. Please note that these constraints"
	 " will be visible as constraints for individual teachers.").arg(gt.rules.teachersList.count()));
}
