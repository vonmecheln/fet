/***************************************************************************
                          addconstraintstudentsminhoursdailyform.cpp  -  description
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

#include "longtextmessagebox.h"

#include "addconstraintstudentsminhoursdailyform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

AddConstraintStudentsMinHoursDailyForm::AddConstraintStudentsMinHoursDailyForm()
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*AddConstraintStudentsMinHoursDailyForm_template*/, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintStudentsMinHoursDailyForm_template*/, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintStudentsMinHoursDailyForm_template*/, SLOT(close()));
//    connect(minHoursSpinBox, SIGNAL(valueChanged(int)), this /*AddConstraintStudentsMinHoursDailyForm_template*/, SLOT(constraintChanged()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	minHoursSpinBox->setMinValue(1);
	minHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	minHoursSpinBox->setValue(1);
}

AddConstraintStudentsMinHoursDailyForm::~AddConstraintStudentsMinHoursDailyForm()
{
}

void AddConstraintStudentsMinHoursDailyForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Students min hours daily");
	s+="\n";

	int minHours=minHoursSpinBox->value();
	if(minHours>=0){
		s+=tr("Min. hours:%1").arg(minHours);
		s+="\n";
	}

	currentConstraintTextEdit->setText(s);*/
}

void AddConstraintStudentsMinHoursDailyForm::addCurrentConstraint()
{
	TimeConstraint *ctr=NULL;

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
	
/*	if(minHoursSpinBox->value()<1){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid min hours - must be >= 1"));
		return;
	}*/

	int minHours=minHoursSpinBox->value();

	ctr=new ConstraintStudentsMinHoursDaily(weight, /*compulsory,*/ minHours);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2)
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
