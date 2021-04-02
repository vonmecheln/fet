/***************************************************************************
                          addconstraintactivitiessamestartingdayform.cpp  -  description
                             -------------------
    begin                : Wed June 23 2004
    copyright            : (C) 2004 by Lalescu Liviu
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

#include "addconstraintactivitiessamestartingdayform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QDesktopWidget>

AddConstraintActivitiesSameStartingDayForm::AddConstraintActivitiesSameStartingDayForm()
{
	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
							
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

	activityTagsComboBox->insertItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* st=gt.rules.activityTagsList[i];
		activityTagsComboBox->insertItem(st->name);
	}
	activityTagsComboBox->setCurrentItem(0);

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
	
	simultaneousActivitiesListBox->clear();
	this->simultaneousActivitiesList.clear();

	updateActivitiesListBox();
}

AddConstraintActivitiesSameStartingDayForm::~AddConstraintActivitiesSameStartingDayForm()
{
}

bool AddConstraintActivitiesSameStartingDayForm::filterOk(Activity* act)
{
	QString tn=teachersComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString sbtn=activityTagsComboBox->currentText();
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
		
	//activity tag
//	if(sbtn!="" && sbtn!=act->activityTagName)
	if(sbtn!="" && !act->activityTagsNames.contains(sbtn))
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

void AddConstraintActivitiesSameStartingDayForm::filterChanged()
{
	this->updateActivitiesListBox();
}

void AddConstraintActivitiesSameStartingDayForm::updateActivitiesListBox()
{
	activitiesListBox->clear();
	//simultaneousActivitiesListBox->clear();

	this->activitiesList.clear();
	//this->simultaneousActivitiesList.clear();

	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* ac=gt.rules.activitiesList[i];
		if(filterOk(ac)){
			activitiesListBox->insertItem(ac->getDescription(gt.rules));
			this->activitiesList.append(ac->id);
		}
	}
}

void AddConstraintActivitiesSameStartingDayForm::addConstraint()
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

	if(this->simultaneousActivitiesList.count()==0){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Empty list of simultaneous activities"));
		return;
	}
	if(this->simultaneousActivitiesList.count()==1){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Only one selected activity - impossible"));
		return;
	}
	if(this->simultaneousActivitiesList.size()>=MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY){
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Too many activities - please report error\n(CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY too little)"));
		return;
	}
	
	int ids[MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY];
	QList<int>::iterator it;
	int i;
	for(i=0, it=this->simultaneousActivitiesList.begin(); it!=this->simultaneousActivitiesList.end(); i++,it++){
		assert(i<MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY);
		ids[i]=*it;
	}
	ctr=new ConstraintActivitiesSameStartingDay(weight, this->simultaneousActivitiesList.count(), ids);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
		
	if(tmp2){
		QString s=QObject::tr("Constraint added:");
		s+="\n";
		s+=ctr->getDetailedDescription(gt.rules);
		s+="\n";
		s+=tr("IMPORTANT: when adding this constraint, it is necessary (otherwise generation might be impossible) to remove redundant constraints"
		" min n days between activities. Only if after adding this constraint you will have redundant min n days constraints."
		" Please read Help/Important tips - tip number 2 for details");
		QMessageBox::information(this, QObject::tr("FET information"), s);
	}
	else{
		QMessageBox::warning(this, QObject::tr("FET information"),
			QObject::tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}

void AddConstraintActivitiesSameStartingDayForm::addActivity()
{
	if(activitiesListBox->currentItem()<0)
		return;
	int tmp=activitiesListBox->currentItem();
	int _id=this->activitiesList.at(tmp);
	
	QString actName=activitiesListBox->currentText();
	assert(actName!="");
	uint i;
	//duplicate?
	for(i=0; i<simultaneousActivitiesListBox->count(); i++)
		if(actName==simultaneousActivitiesListBox->text(i))
			break;
	if(i<simultaneousActivitiesListBox->count())
		return;
	simultaneousActivitiesListBox->insertItem(actName);
	
	this->simultaneousActivitiesList.append(_id);
}

void AddConstraintActivitiesSameStartingDayForm::removeActivity()
{
	if(simultaneousActivitiesListBox->currentItem()<0 || simultaneousActivitiesListBox->count()<=0)
		return;		
	int tmp=simultaneousActivitiesListBox->currentItem();
	
	simultaneousActivitiesListBox->removeItem(tmp);
	this->simultaneousActivitiesList.removeAt(tmp);
}

void AddConstraintActivitiesSameStartingDayForm::clear()
{
	simultaneousActivitiesListBox->clear();
	simultaneousActivitiesList.clear();
}
