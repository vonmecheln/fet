/***************************************************************************
                          modifyconstraintteachermaxgapsperrealdayform.cpp  -  description
                             -------------------
    begin                : 2018
    copyright            : (C) 2018 by Lalescu Liviu
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

#include "modifyconstraintteachermaxgapsperrealdayform.h"
#include "timeconstraint.h"

ModifyConstraintTeacherMaxGapsPerRealDayForm::ModifyConstraintTeacherMaxGapsPerRealDayForm(QWidget* parent, ConstraintTeacherMaxGapsPerRealDay* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	maxGapsSpinBox->setMinimum(0);
	maxGapsSpinBox->setMaximum(2*gt.rules.nHoursPerDay);
	maxGapsSpinBox->setValue(ctr->maxGaps);
	
	exceptionCheckBox->setChecked(ctr->allowOneDayExceptionPlusOne);

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

ModifyConstraintTeacherMaxGapsPerRealDayForm::~ModifyConstraintTeacherMaxGapsPerRealDayForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeacherMaxGapsPerRealDayForm::ok()
{
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

	this->_ctr->weightPercentage=weight;
	
	this->_ctr->maxGaps=maxGapsSpinBox->value();

	this->_ctr->allowOneDayExceptionPlusOne=exceptionCheckBox->isChecked();

	QString teacher_name=teachersComboBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(teacher_name);
	if(teacher_ID<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}
	this->_ctr->teacherName=teacher_name;

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeacherMaxGapsPerRealDayForm::cancel()
{
	this->close();
}
