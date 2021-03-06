/***************************************************************************
                          addconstraintteachersmaxafternoonsperweekform.cpp  -  description
                             -------------------
    begin                : June 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#include "addconstraintteachersmaxafternoonsperweekform.h"
#include "timeconstraint.h"

AddConstraintTeachersMaxAfternoonsPerWeekForm::AddConstraintTeachersMaxAfternoonsPerWeekForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

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
	if(tmp2)
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
