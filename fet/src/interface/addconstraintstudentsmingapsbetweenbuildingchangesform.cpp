/***************************************************************************
                          addconstraintstudentsmingapsbetweenbuildingchangesform.cpp  -  description
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

#include "addconstraintstudentsmingapsbetweenbuildingchangesform.h"
#include "spaceconstraint.h"

AddConstraintStudentsMinGapsBetweenBuildingChangesForm::AddConstraintStudentsMinGapsBetweenBuildingChangesForm()
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*AddConstraintStudentsMinGapsBetweenBuildingChangesForm_template*/, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintStudentsMinGapsBetweenBuildingChangesForm_template*/, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintStudentsMinGapsBetweenBuildingChangesForm_template*/, SLOT(close()));
//    connect(minGapsSpinBox, SIGNAL(valueChanged(int)), this /*AddConstraintStudentsMinGapsBetweenBuildingChangesForm_template*/, SLOT(constraintChanged()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
		
	minGapsSpinBox->setMinValue(1);
	minGapsSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	minGapsSpinBox->setValue(1);
	
	constraintChanged();
}

AddConstraintStudentsMinGapsBetweenBuildingChangesForm::~AddConstraintStudentsMinGapsBetweenBuildingChangesForm()
{
}

void AddConstraintStudentsMinGapsBetweenBuildingChangesForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Students min gaps between building changes");
	s+="\n";
	
	s+=tr("Max building changes per day=%1").arg(minGapsSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void AddConstraintStudentsMinGapsBetweenBuildingChangesForm::addCurrentConstraint()
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

	ctr=new ConstraintStudentsMinGapsBetweenBuildingChanges(weight, minGapsSpinBox->value());

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
