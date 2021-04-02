/***************************************************************************
                          modifyconstraint2activitiesconsecutiveform.cpp  -  description
                             -------------------
    begin                : Aug 21, 2007
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

#include "modifyconstraint2activitiesconsecutiveform.h"
#include "timeconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <QDesktopWidget>

ModifyConstraint2ActivitiesConsecutiveForm::ModifyConstraint2ActivitiesConsecutiveForm(Constraint2ActivitiesConsecutive* ctr)
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	this->_ctr=ctr;
	
	weightLineEdit->setText(QString::number(ctr->weightPercentage));

	updateActivitiesComboBox();

	constraintChanged();
}

ModifyConstraint2ActivitiesConsecutiveForm::~ModifyConstraint2ActivitiesConsecutiveForm()
{
}

bool ModifyConstraint2ActivitiesConsecutiveForm::filterOk(Activity* act)
{
	Q_UNUSED(act);

	int ok=true;
	
	return ok;
}

void ModifyConstraint2ActivitiesConsecutiveForm::filterChanged()
{
	this->updateActivitiesComboBox();
}

void ModifyConstraint2ActivitiesConsecutiveForm::updateActivitiesComboBox(){
	firstActivitiesComboBox->clear();
	firstActivitiesList.clear();

	secondActivitiesComboBox->clear();
	secondActivitiesList.clear();
	
	int i=0, j=-1;
	for(int k=0; k<gt.rules.activitiesList.size(); k++){
		Activity* act=gt.rules.activitiesList[k];
		if(filterOk(act)){
			firstActivitiesComboBox->insertItem(act->getDescription(gt.rules));
			this->firstActivitiesList.append(act->id);

			if(act->id==this->_ctr->firstActivityId)
				j=i;
				
			i++;
		}
	}
	//assert(j>=0); only first time
	firstActivitiesComboBox->setCurrentItem(j);

	i=0, j=-1;
	for(int k=0; k<gt.rules.activitiesList.size(); k++){
		Activity* act=gt.rules.activitiesList[k];
		if(filterOk(act)){
			secondActivitiesComboBox->insertItem(act->getDescription(gt.rules));
			this->secondActivitiesList.append(act->id);

			if(act->id==this->_ctr->secondActivityId)
				j=i;
				
			i++;
		}
	}
	//assert(j>=0); only first time
	secondActivitiesComboBox->setCurrentItem(j);

	constraintChanged();
}

void ModifyConstraint2ActivitiesConsecutiveForm::constraintChanged()
{
	QString s;
	s+=QObject::tr("Current constraint:");
	s+="\n";

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	s+=QObject::tr("Weight (percentage)=%1\%").arg(weight);
	s+="\n";

	s+=QObject::tr("2 activities consecutive");
	s+="\n";
	
	int fid;
	int tmp2=firstActivitiesComboBox->currentItem();
	assert(tmp2<gt.rules.activitiesList.size());
	assert(tmp2<firstActivitiesList.size());
	if(tmp2<0){
		s+=QObject::tr("Invalid activity");
		s+="\n";
	}
	else{
		fid=firstActivitiesList.at(tmp2);
		s+=QObject::tr("First activity id=%1").arg(fid);
		s+="\n";
	}

	int sid;
	int tmp3=secondActivitiesComboBox->currentItem();
	assert(tmp3<gt.rules.activitiesList.size());
	assert(tmp3<secondActivitiesList.size());
	if(tmp3<0){
		s+=QObject::tr("Invalid second activity");
		s+="\n";
	}
	else{
		sid=secondActivitiesList.at(tmp3);
		s+=QObject::tr("Second activity id=%1").arg(sid);
		s+="\n";
	}

	currentConstraintTextEdit->setText(s);
}

void ModifyConstraint2ActivitiesConsecutiveForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid weight (percentage)"));
		return;
	}

	int tmp2=firstActivitiesComboBox->currentItem();
	assert(tmp2<gt.rules.activitiesList.size());
	assert(tmp2<firstActivitiesList.size());
	if(tmp2<0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid first activity"));
		return;
	}
	int fid=firstActivitiesList.at(tmp2);
	
	int tmp3=secondActivitiesComboBox->currentItem();
	assert(tmp3<gt.rules.activitiesList.size());
	assert(tmp3<secondActivitiesList.size());
	if(tmp3<0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid second activity"));
		return;
	}
	int sid=secondActivitiesList.at(tmp3);

	if(sid==fid){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Same activities - impossible"));
		return;
	}
	
	this->_ctr->weightPercentage=weight;
	this->_ctr->firstActivityId=fid;
	this->_ctr->secondActivityId=sid;
	
	gt.rules.internalStructureComputed=false;

	this->close();
}

void ModifyConstraint2ActivitiesConsecutiveForm::cancel()
{
	this->close();
}
