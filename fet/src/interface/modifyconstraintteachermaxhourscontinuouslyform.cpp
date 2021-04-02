/***************************************************************************
                          modifyconstraintteachermaxhourscontinuouslyform.cpp  -  description
                             -------------------
    begin                : July 19, 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#include <QMessageBox>

#include <cstdio>

#include "modifyconstraintteachermaxhourscontinuouslyform.h"
#include "timeconstraint.h"

ModifyConstraintTeacherMaxHoursContinuouslyForm::ModifyConstraintTeacherMaxHoursContinuouslyForm(ConstraintTeacherMaxHoursContinuously* ctr)
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*ModifyConstraintTeacherMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintTeacherMaxHoursContinuouslyForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintTeacherMaxHoursContinuouslyForm_template*/, SLOT(cancel()));
//    connect(maxHoursSpinBox, SIGNAL(valueChanged(int)), this /*ModifyConstraintTeacherMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));
//    connect(teachersComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintTeacherMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	//setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	/*QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	this->_ctr=ctr;
	
	//compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weightPercentage));
	
	updateMaxHoursSpinBox();
	
	maxHoursSpinBox->setValue(ctr->maxHoursContinuously);

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
}

ModifyConstraintTeacherMaxHoursContinuouslyForm::~ModifyConstraintTeacherMaxHoursContinuouslyForm()
{
}

void ModifyConstraintTeacherMaxHoursContinuouslyForm::updateMaxHoursSpinBox(){
	maxHoursSpinBox->setMinValue(1);
	maxHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);	
}

void ModifyConstraintTeacherMaxHoursContinuouslyForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);

	s+=tr("Teacher=%1").arg(teachersComboBox->currentText());
	s+="\n";

	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Teacher max hours continuously");
	s+="\n";

	s+=tr("Max hours continuously=%1").arg(maxHoursSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void ModifyConstraintTeacherMaxHoursContinuouslyForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}
	/*if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - must be 100%"));
		return;
	}*/

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	int max_hours=maxHoursSpinBox->value();

	QString teacher_name=teachersComboBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(teacher_name);
	if(teacher_ID<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}

	this->_ctr->weightPercentage=weight;
	//this->_ctr->compulsory=compulsory;
	this->_ctr->maxHoursContinuously=max_hours;
	this->_ctr->teacherName=teacher_name;

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintTeacherMaxHoursContinuouslyForm::cancel()
{
	this->close();
}
