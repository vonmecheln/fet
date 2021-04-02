/***************************************************************************
                          addconstraintactivitiessamestartingtimeform.cpp  -  description
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

#include <QMessageBox>

#include <cstdio>

#include "longtextmessagebox.h"

#include "addconstraintactivitiessamestartingtimeform.h"
#include "spaceconstraint.h"

#include "matrix.h"

AddConstraintActivitiesSameStartingTimeForm::AddConstraintActivitiesSameStartingTimeForm()
{
	setupUi(this);

    connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addConstraint()));
    connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));
    connect(blockCheckBox, SIGNAL(toggled(bool)), this, SLOT(blockChanged()));
    connect(teachersComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
    connect(studentsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
    connect(subjectsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
    connect(activityTagsComboBox, SIGNAL(activated(QString)), this, SLOT(filterChanged()));
    connect(clearPushButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(activitiesListBox, SIGNAL(selected(QString)), this, SLOT(addActivity()));
    connect(simultaneousActivitiesListBox, SIGNAL(selected(QString)), this, SLOT(removeActivity()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
							
	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
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

AddConstraintActivitiesSameStartingTimeForm::~AddConstraintActivitiesSameStartingTimeForm()
{
}

bool AddConstraintActivitiesSameStartingTimeForm::filterOk(Activity* act)
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

void AddConstraintActivitiesSameStartingTimeForm::filterChanged()
{
	this->updateActivitiesListBox();
}

void AddConstraintActivitiesSameStartingTimeForm::updateActivitiesListBox()
{
	activitiesListBox->clear();
	//simultaneousActivitiesListBox->clear();

	this->activitiesList.clear();
	//this->simultaneousActivitiesList.clear();

	if(blockCheckBox->isChecked())
		//show only non-split activities and split activities which are the representatives
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* ac=gt.rules.activitiesList[i];
			if(filterOk(ac)){
				if(ac->activityGroupId==0){
					activitiesListBox->insertItem(ac->getDescription(gt.rules));
					this->activitiesList.append(ac->id);
				}
				else if(ac->id==ac->activityGroupId){
					activitiesListBox->insertItem(ac->getDescription(gt.rules));
					this->activitiesList.append(ac->id);
				}
			}
		}
	else
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* ac=gt.rules.activitiesList[i];
			if(filterOk(ac)){
				activitiesListBox->insertItem(ac->getDescription(gt.rules));
				this->activitiesList.append(ac->id);
			}
		}
}

void AddConstraintActivitiesSameStartingTimeForm::blockChanged()
{
	simultaneousActivitiesListBox->clear();
	this->simultaneousActivitiesList.clear();

	this->updateActivitiesListBox();
}

void AddConstraintActivitiesSameStartingTimeForm::addConstraint()
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

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	if(this->simultaneousActivitiesList.count()==0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Empty list of simultaneous activities"));
		return;
	}
	if(this->simultaneousActivitiesList.count()==1){
		QMessageBox::warning(this, tr("FET information"),
			tr("Only one selected activity - impossible"));
		return;
	}
	/*if(this->simultaneousActivitiesList.size()>=MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME){
		QMessageBox::warning(this, tr("FET information"),
			tr("Too many activities - please report error\n(CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME too little)"));
		return;
	}*/
	
if(blockCheckBox->isChecked()){ //block constraints
	///////////phase 1 - how many constraints will be added?
	int nConstraints=0;
	QList<int>::iterator it;
	for(it=this->simultaneousActivitiesList.begin(); it!=this->simultaneousActivitiesList.end(); it++){
		int _id=(*it);
		int tmp=0; //tmp represents the number of sub-activities represented by the current (sub)activity

		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if(act->activityGroupId==0){
				if(act->id==_id){
					assert(tmp==0);
					tmp=1;
				}
			}
			else{
				if(act->id==_id){
					assert(act->activityGroupId==act->id);
					assert(tmp==0);
					tmp=1;
				}
				else if(act->activityGroupId==_id)
					tmp++;
			}
		}

		if(nConstraints==0){
			nConstraints=tmp;
		}
		else{
			if(tmp!=nConstraints){
				QString s=tr("Sub-activities do not correspond. Mistake:");
				s+="\n";
				s+=tr("1. First (sub)activity has id=%1 and represents %2 sub-activities")
					.arg(this->simultaneousActivitiesList.at(0))
					.arg(nConstraints);
				s+="\n";
				s+=tr("2. Current (sub)activity has id=%1 and represents %2 sub-activities")
					.arg(_id)
					.arg(tmp);
				QMessageBox::warning(this, tr("FET information"), s);
				return;				
			}
		}
	}
	
	/////////////phase 2 - compute the indices of all the (sub)activities
//#ifdef WIN32
//	int ids[10][MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME];
//#else
	Matrix1D<QList<int> > ids;
	ids.resize(nConstraints);
	//int ids[nConstraints][this->simultaneousActivitiesList.count()];
//#endif
	for(int i=0; i<nConstraints; i++)
		ids[i].clear();
	int k;
	for(k=0, it=this->simultaneousActivitiesList.begin(); it!=this->simultaneousActivitiesList.end(); k++, it++){
		int _id=(*it);
		int tmp=0; //tmp represents the number of sub-activities represented by the current (sub)activity

		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			if(act->activityGroupId==0){
				if(act->id==_id){
					assert(tmp==0);
					//ids[tmp][k]=_id;
					assert(ids[tmp].count()==k);
					ids[tmp].append(_id);
					tmp=1;
				}
			}
			else{
				if(act->id==_id){
					assert(act->activityGroupId==act->id);
					assert(tmp==0);
					//ids[tmp][k]=_id;
					assert(ids[tmp].count()==k);
					ids[tmp].append(_id);
					tmp=1;
				}
				else if(act->activityGroupId==_id){
					//ids[tmp][k]=act->id;
					assert(ids[tmp].count()==k);
					ids[tmp].append(act->id);
					tmp++;
				}
			}
		}
	}
	
	////////////////phase 3 - add the constraints
	for(k=0; k<nConstraints; k++){
		ctr=new ConstraintActivitiesSameStartingTime(weight, /*compulsory,*/ this->simultaneousActivitiesList.count(), ids[k]);
		bool tmp2=gt.rules.addTimeConstraint(ctr);
		
		if(tmp2){
			QString s;

/*			s+=tr("Constraint added")+". "+tr("See details below")+"\n\n";

			s+=tr("IMPORTANT: after adding such constraints, it is necessary (otherwise generation might be impossible) to remove redundant constraints"
				" min days between activities. If you are sure that you don't have redundant constraints, you can skip this step, but it doesn't hurt to do it as a precaution."
				" Also, you don't have to do that after each added constraint, but only once after adding more constraints of this type."
				" Please read Help/Important tips - tip number 2 for details");
			s+="\n\n";*/
			s+=tr("Constraint added:");
			s+="\n\n";
			s+=ctr->getDetailedDescription(gt.rules);
			LongTextMessageBox::information(this, tr("FET information"), s);
		}
		else{
			QMessageBox::warning(this, tr("FET information"),
				tr("Constraint NOT added - please report error"));
			delete ctr;
		}
	}
}
else{
	QList<int> ids;
	//int ids[MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME];
	QList<int>::iterator it;
	int i;
	ids.clear();
	for(i=0, it=this->simultaneousActivitiesList.begin(); it!=this->simultaneousActivitiesList.end(); i++,it++){
		//assert(i<MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME);
		//ids[i]=*it;
		ids.append(*it);
	}
	ctr=new ConstraintActivitiesSameStartingTime(weight, /*compulsory,*/ this->simultaneousActivitiesList.count(), ids);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
		
	if(tmp2){
		QString s;

/*		s+=tr("Constraint added")+". "+tr("See details below")+"\n\n";
		
		s+=tr("IMPORTANT: after adding such constraints, it is necessary (otherwise generation might be impossible) to remove redundant constraints"
			" min days between activities. If you are sure that you don't have redundant constraints, you can skip this step, but it doesn't hurt to do it as a precaution."
			" Also, you don't have to do that after each added constraint, but only once after adding more constraints of this type."
			" Please read Help/Important tips - tip number 2 for details");
		s+="\n\n";*/
		s+=tr("Constraint added:");
		s+="\n\n";
		s+=ctr->getDetailedDescription(gt.rules);
		LongTextMessageBox::information(this, tr("FET information"), s);
	}
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
}

void AddConstraintActivitiesSameStartingTimeForm::addActivity()
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

void AddConstraintActivitiesSameStartingTimeForm::removeActivity()
{
	if(simultaneousActivitiesListBox->currentItem()<0 || simultaneousActivitiesListBox->count()<=0)
		return;		
	int tmp=simultaneousActivitiesListBox->currentItem();
	
	simultaneousActivitiesListBox->removeItem(tmp);
	this->simultaneousActivitiesList.removeAt(tmp);
}

void AddConstraintActivitiesSameStartingTimeForm::clear()
{
	simultaneousActivitiesListBox->clear();
	simultaneousActivitiesList.clear();
}

void AddConstraintActivitiesSameStartingTimeForm::help()
{
	QString s;
	
	s=tr("Add multiple constraints: this is a check box. Select this if you want to input only the representatives of sub-activities and FET to add multiple constraints,"
	" for all sub-activities from the same components, in turn, respectively."
	" There will be added more constraints activities same starting time, one for each corresponding tuple. The number of"
	" sub-activities must match for the representants and be careful to the order, to be what you need");

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
