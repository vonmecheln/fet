/***************************************************************************
                          addconstraintteachersmaxgapsperdayform.cpp  -  description
                             -------------------
    begin                : Jan 21, 2008
    copyright            : (C) 2008 by Lalescu Liviu
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

#include "addconstraintteachersmaxgapsperdayform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

AddConstraintTeachersMaxGapsPerDayForm::AddConstraintTeachersMaxGapsPerDayForm()
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*AddConstraintTeachersMaxGapsPerDayForm_template*/, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintTeachersMaxGapsPerDayForm_template*/, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintTeachersMaxGapsPerDayForm_template*/, SLOT(close()));
//    connect(maxGapsSpinBox, SIGNAL(valueChanged(int)), this /*AddConstraintTeachersMaxGapsPerDayForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
		
	maxGapsSpinBox->setMinValue(0);
	maxGapsSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	maxGapsSpinBox->setValue(1);
	
	constraintChanged();
}

AddConstraintTeachersMaxGapsPerDayForm::~AddConstraintTeachersMaxGapsPerDayForm()
{
}

void AddConstraintTeachersMaxGapsPerDayForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Max gaps=%1").arg(maxGapsSpinBox->value());
	s+="\n";

	s+=tr("Teachers max gaps per day");
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void AddConstraintTeachersMaxGapsPerDayForm::addCurrentConstraint()
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
			tr("Invalid weight (percentage) - it must be 100%"));
		return;
	}

	ctr=new ConstraintTeachersMaxGapsPerDay(weight, maxGapsSpinBox->value());

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
