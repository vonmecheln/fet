/***************************************************************************
                          activitiesform.cpp  -  description
                             -------------------
    begin                : Wed Apr 23 2003
    copyright            : (C) 2003 by Lalescu Liviu
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

#include "timetable_defs.h"
#include "fet.h"
#include "timetable.h"

#include "activitiesform.h"
#include "addactivityform.h"
#include "modifyactivityform.h"

#include <qstring.h>
#include <q3listbox.h>
#include <qmessagebox.h>
#include <q3textedit.h>

#include <QDesktopWidget>

ActivitiesForm::ActivitiesForm()
{
	//setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	centerWidgetOnScreen(this);
	/*QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/

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

	this->filterChanged();
}

ActivitiesForm::~ActivitiesForm()
{
}

bool ActivitiesForm::filterOk(Activity* act)
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
	//if(sbtn!="" && sbtn!=act->activityTagName)
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

void ActivitiesForm::filterChanged()
{
	QString s;
	activitiesListBox->clear();
	visibleActivitiesList.clear();
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		if(this->filterOk(act)){
			s=act->getDescription(gt.rules);
			visibleActivitiesList.append(act);
			activitiesListBox->insertItem(s);
		}
	}
	activityChanged(activitiesListBox->currentItem());
}

void ActivitiesForm::addActivity()
{
	int ind=activitiesListBox->currentItem();

	AddActivityForm *addActivityForm=new AddActivityForm();
	addActivityForm->exec();

	//rebuild the activities list box
	filterChanged();
	
	activitiesListBox->setCurrentItem(ind);
}

void ActivitiesForm::removeActivity()
{
	int ind=activitiesListBox->currentItem();
	if(ind<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected activity"));
		return;
	}

	Activity* act=visibleActivitiesList[ind];
	assert(act!=NULL);

	QString s;
	/*
	if(!act->isSplit())
		s=QObject::tr("Removing activity:");
	else
		s=QObject::tr("Removing sub-activity:");*/
	s=QObject::tr("Removing:");
	s+="\n";
	s+=act->getDetailedDescription(gt.rules);
	s+="\n";
	if(act->isSplit())
		s+=QObject::tr("There will also be removed the related activities from the same larger split activity");

	switch( QMessageBox::warning( this, QObject::tr("FET warning"),
	s, QObject::tr("OK"), QObject::tr("Cancel"), 0, 0, 1 ) ){
	case 0: // The user clicked the OK button or pressed Enter
		gt.rules.removeActivity(act->id, act->activityGroupId);
		filterChanged();
		break;
	case 1: // The user clicked the Cancel or pressed Escape
		break;
	}
	
	if((uint)(ind) >= activitiesListBox->count())
		ind = activitiesListBox->count()-1;
	activitiesListBox->setCurrentItem(ind);
}

void ActivitiesForm::modifyActivity()
{
	int ind=activitiesListBox->currentItem();
	if(ind<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected activity"));
		return;
	}

	Activity* act=visibleActivitiesList[ind];
	assert(act!=NULL);
	
	QStringList teachers=act->teachersNames;
	bool diffTeachers=false;
	
	QString subject=act->subjectName;
	bool diffSubject=false;
	
	QStringList activityTags=act->activityTagsNames;
	bool diffActivityTags=false;
	
	QStringList students=act->studentsNames;
	bool diffStudents=false;
	
	int nTotalStudents=act->nTotalStudents;
	bool diffNTotalStudents=false;
	
	bool computeNTotalStudents=act->computeNTotalStudents;
	bool diffComputeNTotalStudents=false;
	
	if(act->isSplit()){
		int nSplit=0;
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act2=gt.rules.activitiesList[i];
			if(act2->activityGroupId==act->activityGroupId){
				nSplit++;
				
				if(teachers!=act2->teachersNames)
					diffTeachers=true;
				if(subject!=act2->subjectName)
					diffSubject=true;
				if(activityTags!=act2->activityTagsNames)
					diffActivityTags=true;
				if(students!=act2->studentsNames)
					diffStudents=true;
				if( /* !computeNTotalStudents && !act2->computeNTotalStudents && */ nTotalStudents!=act2->nTotalStudents )
					diffNTotalStudents=true;
				if(computeNTotalStudents!=act2->computeNTotalStudents)
					diffComputeNTotalStudents=true;
			}
			if(nSplit>10){
				QMessageBox::warning(this, QObject::tr("FET information"),
					QObject::tr("Cannot modify this large activity, because it contains more than %1 activities.\n"
					"If you really need that, please talk to the author\n").arg(10));
				return;
			}
		}
		
		if(diffTeachers || diffSubject || diffActivityTags || diffStudents || diffNTotalStudents || diffComputeNTotalStudents){
			QStringList s;
			if(diffTeachers)
				s.append(QObject::tr("different teachers"));
			if(diffSubject)
				s.append(QObject::tr("different subject"));
			if(diffActivityTags)
				s.append(QObject::tr("different activity tags"));
			if(diffStudents)
				s.append(QObject::tr("different students"));
			if(diffComputeNTotalStudents)
				s.append(QObject::tr("different boolean variable 'must compute n total students'"));
			if(diffNTotalStudents)
				s.append(QObject::tr("different number of students"));
				
			QString s2;
			s2+=QObject::tr("The current split activity has subactivities which were individually modified. It is recommended to abort now"
			 " and modify individual subactivities from the corresponding menu. Otherwise you will modify the fields for all the subactivities"
			 " from this larger split activity.");
			s2+="\n\n";
			s2+=QObject::tr("The fields which are different are: %1").arg(s.join(", "));
				
			int t=QMessageBox::warning(this, QObject::tr("FET warning"), s2, QObject::tr("Abort"), QObject::tr("Continue"), QString(), 1, 0);
			
			if(t==0)
				return;
		}
	}
	
	ModifyActivityForm* modifyActivityForm=new ModifyActivityForm(act->id, act->activityGroupId);
	modifyActivityForm->exec();

	filterChanged();
	
	activitiesListBox->setCurrentItem(ind);
}

void ActivitiesForm::activityChanged(int index)
{
	if(index<0){
		currentActivityTextEdit->setText(QObject::tr("Invalid activity"));
		return;
	}

	QString s;
	Activity* act=visibleActivitiesList[index];

	assert(act!=NULL);
	s=act->getDetailedDescriptionWithConstraints(gt.rules);
	currentActivityTextEdit->setText(s);
}
