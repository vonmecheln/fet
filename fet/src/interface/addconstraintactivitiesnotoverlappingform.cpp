/***************************************************************************
                          addconstraintactivitiesnotoverlappingform.cpp  -  description
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

#include "addconstraintactivitiesnotoverlappingform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

AddConstraintActivitiesNotOverlappingForm::AddConstraintActivitiesNotOverlappingForm()
{
	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	updateActivitiesListBox();
}

AddConstraintActivitiesNotOverlappingForm::~AddConstraintActivitiesNotOverlappingForm()
{
}

void AddConstraintActivitiesNotOverlappingForm::updateActivitiesListBox()
{
	activitiesListBox->clear();
	notOverlappingActivitiesListBox->clear();

	this->activitiesList.clear();
	this->notOverlappingActivitiesList.clear();

	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* ac=gt.rules.activitiesList[i];
		activitiesListBox->insertItem(ac->getDescription(gt.rules));
		this->activitiesList.append(ac->id);
	}
}

void AddConstraintActivitiesNotOverlappingForm::addConstraint()
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

	if(this->notOverlappingActivitiesList.count()==0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Empty list of not overlapping activities"));
		return;
	}
	if(this->notOverlappingActivitiesList.count()==1){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Only one selected activity"));
		return;
	}
	if(this->notOverlappingActivitiesList.size()>MAX_CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Please report error to the author\nMAX_CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING must be increased (you have too many activities)"));
		return;
	}
	
	int ids[MAX_CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING];
	int i;
	QList<int>::iterator it;
	for(i=0, it=this->notOverlappingActivitiesList.begin(); it!=this->notOverlappingActivitiesList.end(); it++, i++)
		ids[i]=*it;
	
	ctr=new ConstraintActivitiesNotOverlapping(weight, /*compulsory,*/ this->notOverlappingActivitiesList.count(), ids);
	bool tmp2=gt.rules.addTimeConstraint(ctr);
	
	if(tmp2){
		QString s=QObject::tr("Constraint added:");
		s+="\n";
		s+=ctr->getDetailedDescription(gt.rules);
		QMessageBox::information(this, QObject::tr("FET information"), s);
	}
	else{
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}

void AddConstraintActivitiesNotOverlappingForm::addActivity()
{
	if(activitiesListBox->currentItem()<0)
		return;
	int tmp=activitiesListBox->currentItem();
	int _id=this->activitiesList.at(tmp);
	
	QString actName=activitiesListBox->currentText();
	assert(actName!="");
	uint i;
	//duplicate?
	for(i=0; i<notOverlappingActivitiesListBox->count(); i++)
		if(actName==notOverlappingActivitiesListBox->text(i))
			break;
	if(i<notOverlappingActivitiesListBox->count())
		return;
	notOverlappingActivitiesListBox->insertItem(actName);
	
	this->notOverlappingActivitiesList.append(_id);
}

void AddConstraintActivitiesNotOverlappingForm::removeActivity()
{
	if(notOverlappingActivitiesListBox->currentItem()<0 || notOverlappingActivitiesListBox->count()<=0)
		return;		
	int tmp=notOverlappingActivitiesListBox->currentItem();
	
	notOverlappingActivitiesListBox->removeItem(notOverlappingActivitiesListBox->currentItem());
	this->notOverlappingActivitiesList.removeAt(tmp);
}
