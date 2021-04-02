/***************************************************************************
                          modifyconstraintteachersmaxbuildingchangesperdayform.cpp  -  description
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

#include "modifyconstraintteachersmaxbuildingchangesperdayform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

ModifyConstraintTeachersMaxBuildingChangesPerDayForm::ModifyConstraintTeachersMaxBuildingChangesPerDayForm(ConstraintTeachersMaxBuildingChangesPerDay* ctr)
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*ModifyConstraintTeachersMaxBuildingChangesPerDayForm_template*/, SLOT(constraintChanged()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintTeachersMaxBuildingChangesPerDayForm_template*/, SLOT(ok()));
    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintTeachersMaxBuildingChangesPerDayForm_template*/, SLOT(cancel()));
//    connect(maxChangesSpinBox, SIGNAL(valueChanged(int)), this /*ModifyConstraintTeachersMaxBuildingChangesPerDayForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
		
	this->_ctr=ctr;
	
	weightLineEdit->setText(QString::number(ctr->weightPercentage));
	
	maxChangesSpinBox->setMinValue(0);
	maxChangesSpinBox->setMaxValue(10);
	maxChangesSpinBox->setValue(ctr->maxBuildingChangesPerDay);	
		
	constraintChanged();
}

ModifyConstraintTeachersMaxBuildingChangesPerDayForm::~ModifyConstraintTeachersMaxBuildingChangesPerDayForm()
{
}

void ModifyConstraintTeachersMaxBuildingChangesPerDayForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr(QString("Weight (percentage)=%1\%").arg(weight));
	s+="\n";

	s+=tr("Teachers max building changes per day");
	s+="\n";
	
	s+=tr("Max building changes per day=%1").arg(maxChangesSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void ModifyConstraintTeachersMaxBuildingChangesPerDayForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	this->_ctr->weightPercentage=weight;
	this->_ctr->maxBuildingChangesPerDay=maxChangesSpinBox->value();

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintTeachersMaxBuildingChangesPerDayForm::cancel()
{
	this->close();
}
