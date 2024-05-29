/***************************************************************************
                          modifyconstraintstudentssetmingapsbetweenactivitytagbetweenmorningandafternoonform.cpp  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
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

#include "modifyconstraintstudentssetmingapsbetweenactivitytagbetweenmorningandafternoonform.h"
#include "timeconstraint.h"

ModifyConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm::ModifyConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm
 (QWidget* parent, ConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp4=activityTagComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	updateStudentsComboBox(parent);
	updateActivityTagComboBox();

	minGapsSpinBox->setMinimum(1);
	minGapsSpinBox->setMaximum(2*gt.rules.nHoursPerDay);
	minGapsSpinBox->setValue(ctr->minGaps);
}

ModifyConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm::~ModifyConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm::updateStudentsComboBox(QWidget* parent){
	int j=populateStudentsComboBox(studentsComboBox, this->_ctr->students);
	if(j<0)
		showWarningForInvisibleSubgroupConstraint(parent, this->_ctr->students);
	else
		assert(j>=0);
	studentsComboBox->setCurrentIndex(j);
}

void ModifyConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm::updateActivityTagComboBox()
{
	activityTagComboBox->clear();
	int j=-1;
	for(int i=0; i<gt.rules.activityTagsList.count(); i++){
		ActivityTag* at=gt.rules.activityTagsList.at(i);
		activityTagComboBox->addItem(at->name);
		if(at->name==this->_ctr->activityTag)
			j=i;
	}
	assert(j>=0);
	activityTagComboBox->setCurrentIndex(j);
}

void ModifyConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm::ok()
{
	if(studentsComboBox->currentIndex()<0){
		showWarningCannotModifyConstraintInvisibleSubgroupConstraint(this, this->_ctr->students);
		return;
	}

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET warning"),
			tr("Invalid weight (percentage)"));
		return;
	}
	if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - must be 100%"));
		return;
	}

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==nullptr){
		QMessageBox::warning(this, tr("FET warning"),
			tr("Invalid students set"));
		return;
	}
	
	QString activityTagName=activityTagComboBox->currentText();
	int ati=gt.rules.searchActivityTag(activityTagName);
	if(ati<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid activity tag"));
		return;
	}

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;
	this->_ctr->students=students_name;
	this->_ctr->activityTag=activityTagName;
	this->_ctr->minGaps=minGapsSpinBox->value();

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm::cancel()
{
	this->close();
}
