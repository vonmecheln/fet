/***************************************************************************
                          addconstraintstudentssetmaxthreeconsecutivedaysform.cpp  -  description
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

#include "addconstraintstudentssetmaxthreeconsecutivedaysform.h"
#include "timeconstraint.h"

AddConstraintStudentsSetMaxThreeConsecutiveDaysForm::AddConstraintStudentsSetMaxThreeConsecutiveDaysForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	updateExceptionCheckBox();
	updateStudentsComboBox();
}

AddConstraintStudentsSetMaxThreeConsecutiveDaysForm::~AddConstraintStudentsSetMaxThreeConsecutiveDaysForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsSetMaxThreeConsecutiveDaysForm::updateStudentsComboBox(){
	populateStudentsComboBox(studentsComboBox);
}

void AddConstraintStudentsSetMaxThreeConsecutiveDaysForm::updateExceptionCheckBox(){
	allowAMAMExceptionCheckBox->setChecked(false);
}

void AddConstraintStudentsSetMaxThreeConsecutiveDaysForm::addCurrentConstraint()
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

	bool ae=allowAMAMExceptionCheckBox->isChecked();

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==nullptr){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid students set"));
		return;
	}

	ctr=new ConstraintStudentsSetMaxThreeConsecutiveDays(weight, ae, students_name);

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