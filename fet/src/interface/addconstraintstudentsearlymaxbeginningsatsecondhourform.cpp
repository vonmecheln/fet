/***************************************************************************
                          addconstraintstudentsearlymaxbeginningssatsecondhourform.cpp  -  description
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

#include "longtextmessagebox.h"

#include "addconstraintstudentsearlymaxbeginningsatsecondhourform.h"
#include "timeconstraint.h"

AddConstraintStudentsEarlyMaxBeginningsAtSecondHourForm::AddConstraintStudentsEarlyMaxBeginningsAtSecondHourForm()
{
    setupUi(this);

//    connect(weightLineEdit, SIGNAL(textChanged(QString)), this, SLOT(constraintChanged()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
//    connect(maxBeginningsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(constraintChanged()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
		
	maxBeginningsSpinBox->setMinValue(0);
	maxBeginningsSpinBox->setMaxValue(gt.rules.nDaysPerWeek);
	maxBeginningsSpinBox->setValue(0);
	
	constraintChanged();
}

AddConstraintStudentsEarlyMaxBeginningsAtSecondHourForm::~AddConstraintStudentsEarlyMaxBeginningsAtSecondHourForm()
{
}

void AddConstraintStudentsEarlyMaxBeginningsAtSecondHourForm::constraintChanged()
{
/*	QString s;
	s+=tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=tr("Weight percentage=%1\%").arg(weight);
	s+="\n";

	s+=tr("Students must begin activities early, with maximum %1 beginnings at the second available hour, per week (not available and break not counted)")
	 .arg(maxBeginningsSpinBox->value());
	s+="\n";

	currentConstraintTextEdit->setText(s);*/
}

void AddConstraintStudentsEarlyMaxBeginningsAtSecondHourForm::addCurrentConstraint()
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

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	ctr=new ConstraintStudentsEarlyMaxBeginningsAtSecondHour(weight/*, compulsory*/, maxBeginningsSpinBox->value());

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
