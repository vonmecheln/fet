/***************************************************************************
                          modifyconstraintstudentsearlymaxbeginningsatsecondhourform.cpp  -  description
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

#include "modifyconstraintstudentsearlymaxbeginningsatsecondhourform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

//#define yesNo(x)	((x)==0?tr("no"):tr("yes"))

ModifyConstraintStudentsEarlyMaxBeginningsAtSecondHourForm::ModifyConstraintStudentsEarlyMaxBeginningsAtSecondHourForm(ConstraintStudentsEarlyMaxBeginningsAtSecondHour* ctr)
{
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
	
	maxBeginningsSpinBox->setMinValue(0);
	maxBeginningsSpinBox->setMaxValue(gt.rules.nDaysPerWeek);
	maxBeginningsSpinBox->setValue(ctr->maxBeginningsAtSecondHour);
}

ModifyConstraintStudentsEarlyMaxBeginningsAtSecondHourForm::~ModifyConstraintStudentsEarlyMaxBeginningsAtSecondHourForm()
{
}

void ModifyConstraintStudentsEarlyMaxBeginningsAtSecondHourForm::constraintChanged()
{
	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight (percentage)=%1").arg(weight);
	s+="\n";

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;
	s+=tr("Compulsory=%1").arg(yesNo(compulsory));
	s+="\n";*/

	s+=tr("Students must begin activities early, with maximum %1 beginnings at the second available hour, per week (not available and break not counted)")
	 .arg(maxBeginningsSpinBox->value());
 	s+="\n";
			
	/*s+=tr("Students must begin activities as early as possible (permitted by not available and break)");
	s+="\n";*/

	currentConstraintTextEdit->setText(s);
}

void ModifyConstraintStudentsEarlyMaxBeginningsAtSecondHourForm::ok()
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

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	this->_ctr->weightPercentage=weight;
	//this->_ctr->compulsory=compulsory;
	
	this->_ctr->maxBeginningsAtSecondHour=maxBeginningsSpinBox->value();

	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintStudentsEarlyMaxBeginningsAtSecondHourForm::cancel()
{
	this->close();
}
