/***************************************************************************
                          addconstraintteachersmaxbuildingchangesperrealdayform.cpp  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Lalescu Liviu
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

#include "addconstraintteachersmaxbuildingchangesperrealdayform.h"
#include "spaceconstraint.h"

AddConstraintTeachersMaxBuildingChangesPerRealDayForm::AddConstraintTeachersMaxBuildingChangesPerRealDayForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	maxChangesSpinBox->setMinimum(0);
	maxChangesSpinBox->setMaximum(2*gt.rules.nHoursPerDay);
	maxChangesSpinBox->setValue(1);
}

AddConstraintTeachersMaxBuildingChangesPerRealDayForm::~AddConstraintTeachersMaxBuildingChangesPerRealDayForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeachersMaxBuildingChangesPerRealDayForm::addCurrentConstraint()
{
	SpaceConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	ctr=new ConstraintTeachersMaxBuildingChangesPerRealDay(weight, maxChangesSpinBox->value());

	bool tmp2=gt.rules.addSpaceConstraint(ctr);
	if(tmp2)
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
