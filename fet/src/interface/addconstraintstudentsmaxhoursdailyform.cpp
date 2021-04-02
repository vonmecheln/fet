/***************************************************************************
                          addconstraintstudentsmaxhoursdailyform.cpp  -  description
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

#include "addconstraintstudentsmaxhoursdailyform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

AddConstraintStudentsMaxHoursDailyForm::AddConstraintStudentsMaxHoursDailyForm()
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this /*AddConstraintStudentsMaxHoursDailyForm_template*/, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintStudentsMaxHoursDailyForm_template*/, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintStudentsMaxHoursDailyForm_template*/, SLOT(close()));
//    connect(maxHoursSpinBox, SIGNAL(valueChanged(int)), this /*AddConstraintStudentsMaxHoursDailyForm_template*/, SLOT(constraintChanged()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	maxHoursSpinBox->setMinValue(1);
	maxHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	maxHoursSpinBox->setValue(gt.rules.nHoursPerDay);
}

AddConstraintStudentsMaxHoursDailyForm::~AddConstraintStudentsMaxHoursDailyForm()
{
}

void AddConstraintStudentsMaxHoursDailyForm::constraintChanged()
{
/*	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	s+=tr("Students max hours daily");
	s+="\n";

	int maxHours=maxHoursSpinBox->value();
	if(maxHours>=0){
		s+=tr("Max. hours:%1").arg(maxHours);
		s+="\n";
	}

	currentConstraintTextEdit->setText(s);*/
}

void AddConstraintStudentsMaxHoursDailyForm::addCurrentConstraint()
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
	/*if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - must be 100%"));
		return;
	}*/

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	int maxHours=maxHoursSpinBox->value();

	ctr=new ConstraintStudentsMaxHoursDaily(weight, /*compulsory,*/ maxHours);

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
