/***************************************************************************
                          addconstraintstudentsnhoursdailyform.cpp  -  description
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

#include "addconstraintstudentsnhoursdailyform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

#define yesNo(x)	((x)==0?QObject::tr("no"):QObject::tr("yes"))

AddConstraintStudentsNHoursDailyForm::AddConstraintStudentsNHoursDailyForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	minHoursSpinBox->setMinValue(-1);
	minHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	minHoursSpinBox->setValue(-1);
	maxHoursSpinBox->setMinValue(-1);
	maxHoursSpinBox->setMaxValue(gt.rules.nHoursPerDay);
	maxHoursSpinBox->setValue(-1);
}

AddConstraintStudentsNHoursDailyForm::~AddConstraintStudentsNHoursDailyForm()
{
}

void AddConstraintStudentsNHoursDailyForm::constraintChanged()
{
	QString s;
	s+=QObject::tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=QObject::tr("Weight=%1").arg(weight);
	s+="\n";

	bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;
	s+=QObject::tr("Compulsory=%1").arg(yesNo(compulsory));
	s+="\n";

	s+=QObject::tr("Students n hours daily");
	s+="\n";

	int minHours=minHoursSpinBox->value();
	if(minHours>=0){
		s+=QObject::tr("Min. hours:%1").arg(minHours);
		s+="\n";
	}
	int maxHours=maxHoursSpinBox->value();
	if(maxHours>=0){
		s+=QObject::tr("Max. hours:%1").arg(maxHours);
		s+="\n";
	}

	currentConstraintTextEdit->setText(s);
}

void AddConstraintStudentsNHoursDailyForm::addCurrentConstraint()
{
	TimeConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid weight"));
		return;
	}

	bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;

	int minHours=minHoursSpinBox->value();
	int maxHours=maxHoursSpinBox->value();

	if(minHours==-1 && maxHours==-1){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Please specify at least min or max hours"));
		return;
	}
	
	if(minHours==-1 && maxHours==-1){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Please specify at least min or max hours"));
		return;
	}

	ctr=new ConstraintStudentsNHoursDaily(weight, compulsory, maxHours, minHours);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2)
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Constraint added"));
	else{
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
