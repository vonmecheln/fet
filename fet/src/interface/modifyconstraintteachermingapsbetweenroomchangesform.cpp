/***************************************************************************
                          modifyconstraintteachermingapsbetweenroomchangesform.cpp  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Liviu Lalescu
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

#include "modifyconstraintteachermingapsbetweenroomchangesform.h"

ModifyConstraintTeacherMinGapsBetweenRoomChangesForm::ModifyConstraintTeacherMinGapsBetweenRoomChangesForm(QWidget* parent, ConstraintTeacherMinGapsBetweenRoomChanges* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintTeacherMinGapsBetweenRoomChangesForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintTeacherMinGapsBetweenRoomChangesForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	updateTeachersComboBox();

	minGapsSpinBox->setMinimum(1);
	minGapsSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minGapsSpinBox->setValue(ctr->minGapsBetweenRoomChanges);
}

ModifyConstraintTeacherMinGapsBetweenRoomChangesForm::~ModifyConstraintTeacherMinGapsBetweenRoomChangesForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeacherMinGapsBetweenRoomChangesForm::updateTeachersComboBox()
{
	teachersComboBox->clear();
	int i=0, j=-1;
	for(int k=0; k<gt.rules.teachersList.size(); k++, i++){
		Teacher* tch=gt.rules.teachersList[k];
		teachersComboBox->addItem(tch->name);
		if(tch->name==this->_ctr->teacherName)
			j=i;
	}
	assert(j>=0);
	teachersComboBox->setCurrentIndex(j);
}

void ModifyConstraintTeacherMinGapsBetweenRoomChangesForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	QString teacher_name=teachersComboBox->currentText();
	int t=gt.rules.searchTeacher(teacher_name);
	if(t==-1){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;
	this->_ctr->teacherName=teacher_name;
	this->_ctr->minGapsBetweenRoomChanges=minGapsSpinBox->value();

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeacherMinGapsBetweenRoomChangesForm::cancel()
{
	this->close();
}
