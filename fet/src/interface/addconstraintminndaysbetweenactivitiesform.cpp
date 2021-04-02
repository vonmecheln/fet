/***************************************************************************
                          addconstraintminndaysbetweenactivitiesform.cpp  -  description
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

#include "addconstraintminndaysbetweenactivitiesform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

AddConstraintMinNDaysBetweenActivitiesForm::AddConstraintMinNDaysBetweenActivitiesForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);
	
	updateActivitiesListBox();
	
	minDaysSpinBox->setMinValue(1);
	minDaysSpinBox->setValue(1);
	minDaysSpinBox->setMaxValue(gt.rules.nDaysPerWeek-1);

	teachersComboBox->insertItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->insertItem(tch->name);
	}
	teachersComboBox->setCurrentItem(0);

	subjectsComboBox->insertItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsComboBox->insertItem(sb->name);
	}
	subjectsComboBox->setCurrentItem(0);

	subjectTagsComboBox->insertItem("");
	for(int i=0; i<gt.rules.subjectTagsList.size(); i++){
		SubjectTag* st=gt.rules.subjectTagsList[i];
		subjectTagsComboBox->insertItem(st->name);
	}
	subjectTagsComboBox->setCurrentItem(0);

	studentsComboBox->insertItem("");
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* sty=gt.rules.yearsList[i];
		studentsComboBox->insertItem(sty->name);
		for(int j=0; j<sty->groupsList.size(); j++){
			StudentsGroup* stg=sty->groupsList[j];
			studentsComboBox->insertItem(stg->name);
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				studentsComboBox->insertItem(sts->name);
			}
		}
	}
	studentsComboBox->setCurrentItem(0);

	selectedActivitiesListBox->clear();
	this->selectedActivitiesList.clear();

	filterChanged();
}

AddConstraintMinNDaysBetweenActivitiesForm::~AddConstraintMinNDaysBetweenActivitiesForm()
{
}

bool AddConstraintMinNDaysBetweenActivitiesForm::filterOk(Activity* act)
{
	QString tn=teachersComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString sbtn=subjectTagsComboBox->currentText();
	int ok=true;

	//teacher
	if(tn!=""){
		bool ok2=false;
		for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
			if(*it == tn){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}

	//subject
	if(sbn!="" && sbn!=act->subjectName)
		ok=false;
		
	//subject tag
	if(sbtn!="" && sbtn!=act->subjectTagName)
		ok=false;
		
	//students
	if(stn!=""){
		bool ok2=false;
		for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
			if(*it == stn){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}
	
	return ok;
}

void AddConstraintMinNDaysBetweenActivitiesForm::filterChanged()
{
	this->updateActivitiesListBox();
}

void AddConstraintMinNDaysBetweenActivitiesForm::updateActivitiesListBox()
{
	activitiesListBox->clear();
	//selectedActivitiesListBox->clear();

	this->activitiesList.clear();
	//this->selectedActivitiesList.clear();

	/*for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* ac=gt.rules.activitiesList[i];
		activitiesListBox->insertItem(ac->getDescription(gt.rules));
		this->activitiesList.append(ac->id);
	}*/
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* ac=gt.rules.activitiesList[i];
		if(filterOk(ac)){
			activitiesListBox->insertItem(ac->getDescription(gt.rules));
			this->activitiesList.append(ac->id);
		}
	}
}

void AddConstraintMinNDaysBetweenActivitiesForm::addConstraint()
{
	TimeConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 | weight>100){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Invalid weight (percentage)"));
		return;
	}

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	if(this->selectedActivitiesList.count()==0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Empty list of selected activities"));
		return;
	}
	if(this->selectedActivitiesList.count()==1){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Only one selected activity"));
		return;
	}
	if(this->selectedActivitiesList.size()>MAX_CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Please report error to the author\nMAX_CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES must be increased (you have too many activities)"));
		return;
	}
	
	int ids[MAX_CONSTRAINT_MIN_N_DAYS_BETWEEN_ACTIVITIES];
	int i;
	QList<int>::iterator it;
	for(i=0, it=this->selectedActivitiesList.begin(); it!=this->selectedActivitiesList.end(); it++, i++)
		ids[i]=*it;
	
	ctr=new ConstraintMinNDaysBetweenActivities(weight, consecutiveIfSameDayCheckBox->isChecked(),/*compulsory,*/ this->selectedActivitiesList.count(), ids, minDaysSpinBox->value());
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

void AddConstraintMinNDaysBetweenActivitiesForm::addActivity()
{
	if(activitiesListBox->currentItem()<0)
		return;
	int tmp=activitiesListBox->currentItem();
	int _id=this->activitiesList.at(tmp);
	
	QString actName=activitiesListBox->currentText();
	assert(actName!="");
	uint i;
	//duplicate?
	for(i=0; i<selectedActivitiesListBox->count(); i++)
		if(actName==selectedActivitiesListBox->text(i))
			break;
	if(i<selectedActivitiesListBox->count())
		return;
	selectedActivitiesListBox->insertItem(actName);
	
	this->selectedActivitiesList.append(_id);
}

void AddConstraintMinNDaysBetweenActivitiesForm::removeActivity()
{
	if(selectedActivitiesListBox->currentItem()<0 || selectedActivitiesListBox->count()<=0)
		return;		
	int tmp=selectedActivitiesListBox->currentItem();
	
	selectedActivitiesListBox->removeItem(selectedActivitiesListBox->currentItem());
	this->selectedActivitiesList.removeAt(tmp);
}
