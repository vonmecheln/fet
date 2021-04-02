/***************************************************************************
                          modifyconstraintstudentsmaxgapsperweekform.cpp  -  description
                             -------------------
    begin                : Feb 11, 2005
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

#include <QMessageBox>

#include <cstdio>

#include "modifyconstraintstudentsmaxgapsperweekform.h"
#include "timeconstraint.h"

ModifyConstraintStudentsMaxGapsPerWeekForm::ModifyConstraintStudentsMaxGapsPerWeekForm(ConstraintStudentsMaxGapsPerWeek* ctr)
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*ModifyConstraintStudentsMaxGapsPerWeekForm_template*/, SLOT(constraintChanged()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsMaxGapsPerWeekForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintStudentsMaxGapsPerWeekForm_template*/, SLOT(cancel()));
//    connect(maxGapsSpinBox, SIGNAL(valueChanged(int)), this /*ModifyConstraintStudentsMaxGapsPerWeekForm_template*/, SLOT(constraintChanged()));


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
	
	maxGapsSpinBox->setMinValue(0);
	maxGapsSpinBox->setMaxValue(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek);
	maxGapsSpinBox->setValue(ctr->maxGaps);
}

ModifyConstraintStudentsMaxGapsPerWeekForm::~ModifyConstraintStudentsMaxGapsPerWeekForm()
{
}

void ModifyConstraintStudentsMaxGapsPerWeekForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1\%").arg(weight);
	s+="\n";
	
	s+=tr("Max gaps=%1").arg(maxGapsSpinBox->value());
	s+="\n";

	s+=tr("Students max gaps per week");
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void ModifyConstraintStudentsMaxGapsPerWeekForm::ok()
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
			tr("Invalid weight (percentage) - it must be 100%"));
		return;
	}

	this->_ctr->weightPercentage=weight;
	
	this->_ctr->maxGaps=maxGapsSpinBox->value();

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintStudentsMaxGapsPerWeekForm::cancel()
{
	this->close();
}
