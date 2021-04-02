/***************************************************************************
                          modifyconstraintteachersminhoursdailyform.cpp  -  description
                             -------------------
    begin                : Sept 21, 2007
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

#include "modifyconstraintteachersminhoursdailyform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

ModifyConstraintTeachersMinHoursDailyForm::ModifyConstraintTeachersMinHoursDailyForm(ConstraintTeachersMinHoursDaily* ctr)
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*ModifyConstraintTeachersMinHoursDailyForm_template*/, SLOT(constraintChanged()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintTeachersMinHoursDailyForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintTeachersMinHoursDailyForm_template*/, SLOT(cancel()));
//    connect(minHoursSpinBox, SIGNAL(valueChanged(int)), this /*ModifyConstraintTeachersMinHoursDailyForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(QString::number(ctr->weightPercentage));
	
	updateMinHoursSpinBox();
	
	minHoursSpinBox->setValue(ctr->minHoursDaily);
}

ModifyConstraintTeachersMinHoursDailyForm::~ModifyConstraintTeachersMinHoursDailyForm()
{
}

void ModifyConstraintTeachersMinHoursDailyForm::updateMinHoursSpinBox(){
	minHoursSpinBox->setMinValue(2);
	minHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);	
}

void ModifyConstraintTeachersMinHoursDailyForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Teachers min hours daily ");
	s+="\n";

	s+=tr("Min hours daily=%1").arg(minHoursSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void ModifyConstraintTeachersMinHoursDailyForm::ok()
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
			tr("Invalid weight (percentage) - must be 100%"));
		return;
	}

	int min_hours=minHoursSpinBox->value();

	this->_ctr->weightPercentage=weight;
	this->_ctr->minHoursDaily=min_hours;

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintTeachersMinHoursDailyForm::cancel()
{
	this->close();
}
