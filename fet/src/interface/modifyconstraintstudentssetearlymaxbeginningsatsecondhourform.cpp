/***************************************************************************
                          modifyconstraintstudentssetearlymaxbeginningsatsecondhourform.cpp  -  description
                             -------------------
    begin                : July 18, 2007
    copyright            : (C) 2007 by Liviu Lalescu
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

#include "modifyconstraintstudentssetearlymaxbeginningsatsecondhourform.h"
#include "timeconstraint.h"

ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm(QWidget* parent, ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));

	int j=populateStudentsComboBox(studentsComboBox, this->_ctr->students);
	if(j<0)
		showWarningForInvisibleSubgroupConstraint(parent, this->_ctr->students);
	else
		assert(j>=0);
	studentsComboBox->setCurrentIndex(j);
	
	maxBeginningsSpinBox->setMinimum(0);
	maxBeginningsSpinBox->setMaximum(gt.rules.nDaysPerWeek);
	maxBeginningsSpinBox->setValue(ctr->maxBeginningsAtSecondHour);
}

ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::~ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::ok()
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

	this->_ctr->students=students_name;
	this->_ctr->weightPercentage=weight;
	
	this->_ctr->maxBeginningsAtSecondHour=maxBeginningsSpinBox->value();
	
	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm::cancel()
{
	this->close();
}
