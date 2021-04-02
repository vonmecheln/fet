/***************************************************************************
                          modifyconstraint2activitiesgroupedform.cpp  -  description
                             -------------------
    begin                : 15 Feb 2005
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

#include "modifyconstraint2activitiesgroupedform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

#define yesNo(x)	((x)==0?QObject::tr("no"):QObject::tr("yes"))

ModifyConstraint2ActivitiesGroupedForm::ModifyConstraint2ActivitiesGroupedForm(Constraint2ActivitiesGrouped* ctr)
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	this->_ctr=ctr;
	
	activitiesIdsLineEdit->setText(QString::number(ctr->firstActivityId)+","+QString::number(ctr->secondActivityId));
	compulsoryCheckBox->setChecked(ctr->compulsory);
	weightLineEdit->setText(QString::number(ctr->weight));
}

ModifyConstraint2ActivitiesGroupedForm::~ModifyConstraint2ActivitiesGroupedForm()
{
}

void ModifyConstraint2ActivitiesGroupedForm::constraintChanged()
{
	QString s;
	s+=QObject::tr("Current constraint");
	s+=":\n";

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

	s+=QObject::tr("The activities with id's: %1 must be scheduled grouped (order is not important)").arg(activitiesIdsLineEdit->text());
	s+="\n";

	currentConstraintTextEdit->setText(s);
}

void ModifyConstraint2ActivitiesGroupedForm::cancel()
{
	this->close();
}

void ModifyConstraint2ActivitiesGroupedForm::ok()
{
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

	int act_id[2];
	int n_act=sscanf(activitiesIdsLineEdit->text(), "%d,%d", act_id, act_id+1);

	if(n_act!=2){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Please input 2 activities (in any order) separated by commas"));
		return;
	}

	this->_ctr->weight=weight;
	this->_ctr->compulsory=compulsory;
	this->_ctr->firstActivityId=act_id[0];
	this->_ctr->secondActivityId=act_id[1];
	
	gt.rules.internalStructureComputed=false;
	
	this->close();
}
