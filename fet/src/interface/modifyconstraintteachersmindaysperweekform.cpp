/***************************************************************************
                          modifyconstraintteachersmindaysperweekform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "modifyconstraintteachersmindaysperweekform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>

#include <qlineedit.h>

#include <QMessageBox>

#include <QDesktopWidget>

ModifyConstraintTeachersMinDaysPerWeekForm::ModifyConstraintTeachersMinDaysPerWeekForm(ConstraintTeachersMinDaysPerWeek* ctr)
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*ModifyConstraintTeachersMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintTeachersMaxDaysPerWeekForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintTeachersMaxDaysPerWeekForm_template*/, SLOT(cancel()));
//    connect(maxDaysSpinBox, SIGNAL(valueChanged(int)), this /*ModifyConstraintTeachersMaxDaysPerWeekForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	this->_ctr=ctr;
	
	//compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weightPercentage));
	
	updateMinDaysSpinBox();
	//updateTeachersComboBox();
	
	minDaysSpinBox->setValue(ctr->minDaysPerWeek);

	//constraintChanged();
}

ModifyConstraintTeachersMinDaysPerWeekForm::~ModifyConstraintTeachersMinDaysPerWeekForm()
{
}

/*
void ModifyConstraintTeachersMaxDaysPerWeekForm::updateTeachersComboBox(){
	teachersComboBox->clear();
	int i=0, j=-1;
	for(int k=0; k<gt.rules.teachersList.size(); k++, i++){
		Teacher* tch=gt.rules.teachersList[k];
		teachersComboBox->insertItem(tch->name);
		if(tch->name==this->_ctr->teacherName)
			j=i;
	}
	assert(j>=0);
	teachersComboBox->setCurrentItem(j);

	constraintChanged();
}*/

void ModifyConstraintTeachersMinDaysPerWeekForm::updateMinDaysSpinBox(){
	minDaysSpinBox->setMinValue(1);
	minDaysSpinBox->setMaxValue(gt.rules.nDaysPerWeek);	
}

void ModifyConstraintTeachersMinDaysPerWeekForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
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

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	int min_days=minDaysSpinBox->value();

	/*QString teacher_name=teachersComboBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(teacher_name);
	if(teacher_ID<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}*/

	this->_ctr->weightPercentage=weight;
	//this->_ctr->compulsory=compulsory;
	this->_ctr->minDaysPerWeek=min_days;
	//this->_ctr->teacherName=teacher_name;

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintTeachersMinDaysPerWeekForm::cancel()
{
	this->close();
}
