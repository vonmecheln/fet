/***************************************************************************
                          modifyconstraintstudentssetmaxbuildingchangesperrealdayinintervalform.cpp  -  description
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

#include "modifyconstraintstudentssetmaxbuildingchangesperrealdayinintervalform.h"

ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm(QWidget* parent, ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);

	QSize tmp5=startHourComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	QSize tmp6=endHourComboBox->minimumSizeHint();
	Q_UNUSED(tmp6);

	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));

	maxChangesSpinBox->setMinimum(0);
	maxChangesSpinBox->setMaximum(2*gt.rules.nHoursPerDay);
	maxChangesSpinBox->setValue(ctr->maxBuildingChangesPerDay);

	startHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		startHourComboBox->addItem(tr("Morning %1", "Hour %1 of the morning").arg(gt.rules.hoursOfTheDay[i]));
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		startHourComboBox->addItem(tr("Afternoon %1", "Hour %1 of the afternoon").arg(gt.rules.hoursOfTheDay[i]));
	startHourComboBox->setCurrentIndex(ctr->intervalStart);

	endHourComboBox->clear();
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		endHourComboBox->addItem(tr("Morning %1", "Hour %1 of the morning").arg(gt.rules.hoursOfTheDay[i]));
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		endHourComboBox->addItem(tr("Afternoon %1", "Hour %1 of the afternoon").arg(gt.rules.hoursOfTheDay[i]));
	endHourComboBox->addItem(tr("End of real day"));
	endHourComboBox->setCurrentIndex(ctr->intervalEnd);
	
	updateStudentsComboBox(parent);
}

ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::~ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::updateStudentsComboBox(QWidget* parent)
{
	int j=populateStudentsComboBox(studentsComboBox, this->_ctr->studentsName);
	if(j<0)
		showWarningForInvisibleSubgroupConstraint(parent, this->_ctr->studentsName);
	else
		assert(j>=0);
	studentsComboBox->setCurrentIndex(j);
}

void ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::ok()
{
	if(studentsComboBox->currentIndex()<0){
		showWarningCannotModifyConstraintInvisibleSubgroupConstraint(this, this->_ctr->studentsName);
		return;
	}

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

	int startHour=startHourComboBox->currentIndex();
	int endHour=endHourComboBox->currentIndex();
	if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
		QMessageBox::warning(this, tr("FET information"),
			tr("Start hour invalid"));
		return;
	}
	if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
		QMessageBox::warning(this, tr("FET information"),
			tr("End hour invalid"));
		return;
	}
	if(endHour-startHour<2){
		QMessageBox::warning(this, tr("FET information"),
			tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
		return;
	}

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;
	this->_ctr->studentsName=students_name;
	this->_ctr->maxBuildingChangesPerDay=maxChangesSpinBox->value();
	this->_ctr->intervalStart=startHour;
	this->_ctr->intervalEnd=endHour;

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm::cancel()
{
	this->close();
}