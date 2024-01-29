/***************************************************************************
                          modifyconstraintstudentssetminrestinghoursbetweenmorningandafternoonform.cpp  -  description
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

#include "modifyconstraintstudentssetminrestinghoursbetweenmorningandafternoonform.h"
#include "timeconstraint.h"

ModifyConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoonForm::ModifyConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoonForm(QWidget* parent, ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoonForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoonForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));

	minRestingHoursSpinBox->setMinimum(1);
	minRestingHoursSpinBox->setMaximum(2*gt.rules.nHoursPerDay);
	minRestingHoursSpinBox->setValue(ctr->minRestingHours);
	
	updateStudentsComboBox(parent);
}

ModifyConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoonForm::~ModifyConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoonForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoonForm::updateStudentsComboBox(QWidget* parent){
	int j=populateStudentsComboBox(studentsComboBox, this->_ctr->students);
	if(j<0)
		showWarningForInvisibleSubgroupConstraint(parent, this->_ctr->students);
	else
		assert(j>=0);
	studentsComboBox->setCurrentIndex(j);
}

void ModifyConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoonForm::ok()
{
	if(studentsComboBox->currentIndex()<0){
		showWarningCannotModifyConstraintInvisibleSubgroupConstraint(this, this->_ctr->students);
		return;
	}

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
			tr("Invalid weight (percentage) - it must be 100%"));
		return;
	}

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==nullptr){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid students set"));
		return;
	}

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;
	this->_ctr->students=students_name;

	this->_ctr->minRestingHours=minRestingHoursSpinBox->value();

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoonForm::cancel()
{
	this->close();
}
