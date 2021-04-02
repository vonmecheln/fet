/***************************************************************************
                          addconstraintteachersminhoursdailyform.cpp  -  description
                             -------------------
    begin                : Sept. 21, 2007
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

#include "addconstraintteachersminhoursdailyform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

AddConstraintTeachersMinHoursDailyForm::AddConstraintTeachersMinHoursDailyForm()
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*AddConstraintTeachersMinHoursDailyForm_template*/, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintTeachersMinHoursDailyForm_template*/, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintTeachersMinHoursDailyForm_template*/, SLOT(close()));
//    connect(minHoursSpinBox, SIGNAL(valueChanged(int)), this /*AddConstraintTeachersMinHoursDailyForm_template*/, SLOT(constraintChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	updateMinHoursSpinBox();
	
	constraintChanged();
}

AddConstraintTeachersMinHoursDailyForm::~AddConstraintTeachersMinHoursDailyForm()
{
}

void AddConstraintTeachersMinHoursDailyForm::updateMinHoursSpinBox(){
	minHoursSpinBox->setMinValue(2);
	minHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	minHoursSpinBox->setValue(2);
}

void AddConstraintTeachersMinHoursDailyForm::constraintChanged()
{/*
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Teachers min hours daily");
	s+="\n";

	s+=tr("Min hours daily=%1").arg(minHoursSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void AddConstraintTeachersMinHoursDailyForm::addCurrentConstraint()
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
			tr("Invalid weight (percentage) - must be 100%"));
		return;
	}

	int min_hours=minHoursSpinBox->value();

	ctr=new ConstraintTeachersMinHoursDaily(weight, min_hours);

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
