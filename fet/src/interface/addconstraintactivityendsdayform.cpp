/***************************************************************************
                          addconstraintactivityendsdayform.cpp  -  description
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

#include "addconstraintactivityendsdayform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

#define yesNo(x)	((x)==0?QObject::tr("no"):QObject::tr("yes"))

AddConstraintActivityEndsDayForm::AddConstraintActivityEndsDayForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	updateActivitiesComboBox();
}

AddConstraintActivityEndsDayForm::~AddConstraintActivityEndsDayForm()
{
}

void AddConstraintActivityEndsDayForm::updateActivitiesComboBox(){
	activitiesComboBox->clear();
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		activitiesComboBox->insertItem(act->getDescription(gt.rules));
	}

	constraintChanged();
}

void AddConstraintActivityEndsDayForm::constraintChanged()
{
	QString s;
	s+=QObject::tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=QObject::tr("Weight (percentage)=%1\%").arg(weight);
	s+="\n";

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;
	s+=QObject::tr("Compulsory=%1").arg(yesNo(compulsory));
	s+="\n";*/

	s+=QObject::tr("Activity ends day");
	s+="\n";
	int tmp2=activitiesComboBox->currentItem();
	if(tmp2<0 || tmp2>=gt.rules.activitiesList.size()){
		s+=QObject::tr("Invalid activity");
		s+="\n";
	}
	else{
		int id=gt.rules.activitiesList.at(tmp2)->id;
		s+=QObject::tr("Activity id=%1").arg(id);
		s+="\n";
	}
}

void AddConstraintActivityEndsDayForm::addCurrentConstraint()
{
	TimeConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid weight (percentage)"));
		return;
	}

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	int id;
	int tmp2=activitiesComboBox->currentItem();
	if(tmp2<0 || tmp2>=gt.rules.activitiesList.size()){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid activity"));
		return;
	}
	else
		id=gt.rules.activitiesList.at(tmp2)->id;
	
	ctr=new ConstraintActivityEndsDay(weight, /*compulsory,*/ id);

	bool tmp3=gt.rules.addTimeConstraint(ctr);
	if(tmp3)
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Constraint added"));
	else{
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}