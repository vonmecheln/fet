/***************************************************************************
                          modifyconstraintteachersmaxhoursdailyform.cpp  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Lalescu Liviu
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

#include "modifyconstraintteachersmaxhoursdailyform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

ModifyConstraintTeachersMaxHoursDailyForm::ModifyConstraintTeachersMaxHoursDailyForm(ConstraintTeachersMaxHoursDaily* ctr)
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*ModifyConstraintTeachersMaxHoursDailyForm_template*/, SLOT(constraintChanged()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintTeachersMaxHoursDailyForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintTeachersMaxHoursDailyForm_template*/, SLOT(cancel()));
//    connect(maxHoursSpinBox, SIGNAL(valueChanged(int)), this /*ModifyConstraintTeachersMaxHoursDailyForm_template*/, SLOT(constraintChanged()));


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
	
	updateMaxHoursSpinBox();
	
	maxHoursSpinBox->setValue(ctr->maxHoursDaily);
}

ModifyConstraintTeachersMaxHoursDailyForm::~ModifyConstraintTeachersMaxHoursDailyForm()
{
}

void ModifyConstraintTeachersMaxHoursDailyForm::updateMaxHoursSpinBox(){
	maxHoursSpinBox->setMinValue(1);
	maxHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);	
}

void ModifyConstraintTeachersMaxHoursDailyForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Teachers max hours daily ");
	s+="\n";

	s+=tr("Max hours daily=%1").arg(maxHoursSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void ModifyConstraintTeachersMaxHoursDailyForm::ok()
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

	this->_ctr->weightPercentage=weight;
	//this->_ctr->compulsory=compulsory;
	this->_ctr->maxHoursDaily=max_hours;

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintTeachersMaxHoursDailyForm::cancel()
{
	this->close();
}
