/***************************************************************************
                          addconstraintstudentssetmaxbuildingchangesperweekform.cpp  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Liviu Lalescu
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

#include "addconstraintstudentssetmaxbuildingchangesperweekform.h"

AddConstraintStudentsSetMaxBuildingChangesPerWeekForm::AddConstraintStudentsSetMaxBuildingChangesPerWeekForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetMaxBuildingChangesPerWeekForm::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetMaxBuildingChangesPerWeekForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);

	maxChangesSpinBox->setMinimum(0);
	maxChangesSpinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	maxChangesSpinBox->setValue(3);

	updateStudentsSetComboBox();
}

AddConstraintStudentsSetMaxBuildingChangesPerWeekForm::~AddConstraintStudentsSetMaxBuildingChangesPerWeekForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsSetMaxBuildingChangesPerWeekForm::updateStudentsSetComboBox()
{
	populateStudentsComboBox(studentsComboBox);
}

void AddConstraintStudentsSetMaxBuildingChangesPerWeekForm::addCurrentConstraint()
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

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==nullptr){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid students set"));
		return;
	}

	ctr=new ConstraintStudentsSetMaxBuildingChangesPerWeek(weight, students_name, maxChangesSpinBox->value());

	bool tmp2=gt.rules.addSpaceConstraint(ctr);
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
