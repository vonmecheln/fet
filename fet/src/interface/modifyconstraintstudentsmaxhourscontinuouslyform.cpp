/***************************************************************************
                          modifyconstraintstudentsmaxhourscontinuouslyform.cpp  -  description
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

#include "modifyconstraintstudentsmaxhourscontinuouslyform.h"
#include "timeconstraint.h"

ModifyConstraintStudentsMaxHoursContinuouslyForm::ModifyConstraintStudentsMaxHoursContinuouslyForm(ConstraintStudentsMaxHoursContinuously* ctr)
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*ModifyConstraintStudentsMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsMaxHoursContinuouslyForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsMaxHoursContinuouslyForm_template*/, SLOT(cancel()));
//    connect(maxHoursSpinBox, SIGNAL(valueChanged(int)), this /*ModifyConstraintStudentsMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged()));

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
	
	maxHoursSpinBox->setMinValue(1);
	maxHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	maxHoursSpinBox->setValue(ctr->maxHoursContinuously);
}

ModifyConstraintStudentsMaxHoursContinuouslyForm::~ModifyConstraintStudentsMaxHoursContinuouslyForm()
{
}

void ModifyConstraintStudentsMaxHoursContinuouslyForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1\%").arg(weight);
	s+="\n";

	s+=tr("Students max hours continuously");
	s+="\n";

	s+=tr("Max hours:%1").arg(maxHoursSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void ModifyConstraintStudentsMaxHoursContinuouslyForm::ok()
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

/*	bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	this->_ctr->weightPercentage=weight;
	//this->_ctr->compulsory=compulsory;
	//this->_ctr->minHoursDaily=minHoursSpinBox->value();
	this->_ctr->maxHoursContinuously=maxHoursSpinBox->value();

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintStudentsMaxHoursContinuouslyForm::cancel()
{
	this->close();
}
