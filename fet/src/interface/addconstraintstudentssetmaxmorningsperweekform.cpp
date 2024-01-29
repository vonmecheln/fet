/***************************************************************************
                          addconstraintstudentssetmaxmorningsperweekform.cpp  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Liviu Lalescu
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

#include "addconstraintstudentssetmaxmorningsperweekform.h"
#include "timeconstraint.h"

AddConstraintStudentsSetMaxMorningsPerWeekForm::AddConstraintStudentsSetMaxMorningsPerWeekForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetMaxMorningsPerWeekForm::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetMaxMorningsPerWeekForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);

	updateMaxMorningsSpinBox();
	updateStudentsComboBox();
}

AddConstraintStudentsSetMaxMorningsPerWeekForm::~AddConstraintStudentsSetMaxMorningsPerWeekForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsSetMaxMorningsPerWeekForm::updateStudentsComboBox()
{
	populateStudentsComboBox(studentsComboBox);
}

void AddConstraintStudentsSetMaxMorningsPerWeekForm::updateMaxMorningsSpinBox(){
	maxMorningsSpinBox->setMinimum(0);
	maxMorningsSpinBox->setMaximum(gt.rules.nDaysPerWeek/2);
	maxMorningsSpinBox->setValue(gt.rules.nDaysPerWeek/2);
}

void AddConstraintStudentsSetMaxMorningsPerWeekForm::addCurrentConstraint()
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

	int max_mornings=maxMorningsSpinBox->value();

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==nullptr){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid students set"));
		return;
	}
	
	ctr=new ConstraintStudentsSetMaxMorningsPerWeek(weight, max_mornings, students_name);

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
