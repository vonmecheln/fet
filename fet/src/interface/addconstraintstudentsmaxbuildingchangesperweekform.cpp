/***************************************************************************
                          addconstraintstudentsmaxbuildingchangesperweekform.cpp  -  description
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

#include <QMessageBox>

#include <cstdio>

#include "longtextmessagebox.h"

#include "addconstraintstudentsmaxbuildingchangesperweekform.h"
#include "spaceconstraint.h"

AddConstraintStudentsMaxBuildingChangesPerWeekForm::AddConstraintStudentsMaxBuildingChangesPerWeekForm()
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*AddConstraintStudentsMaxBuildingChangesPerWeekForm_template*/, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintStudentsMaxBuildingChangesPerWeekForm_template*/, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintStudentsMaxBuildingChangesPerWeekForm_template*/, SLOT(close()));
//    connect(maxChangesSpinBox, SIGNAL(valueChanged(int)), this /*AddConstraintStudentsMaxBuildingChangesPerWeekForm_template*/, SLOT(constraintChanged()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
		
	maxChangesSpinBox->setMinValue(0);
	maxChangesSpinBox->setMaxValue(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
	maxChangesSpinBox->setValue(3);
	
	constraintChanged();
}

AddConstraintStudentsMaxBuildingChangesPerWeekForm::~AddConstraintStudentsMaxBuildingChangesPerWeekForm()
{
}

void AddConstraintStudentsMaxBuildingChangesPerWeekForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Students max building changes per week");
	s+="\n";
	
	s+=tr("Max building changes per week=%1").arg(maxChangesSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void AddConstraintStudentsMaxBuildingChangesPerWeekForm::addCurrentConstraint()
{
	SpaceConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	ctr=new ConstraintStudentsMaxBuildingChangesPerWeek(weight, maxChangesSpinBox->value());

	bool tmp2=gt.rules.addSpaceConstraint(ctr);
	if(tmp2)
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
