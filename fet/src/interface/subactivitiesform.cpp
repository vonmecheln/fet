/***************************************************************************
                          subactivitiesform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#include "subactivitiesform.h"
#include "modifysubactivityform.h"

#include <qstring.h>
#include <q3listbox.h>
#include <qmessagebox.h>
#include <q3textedit.h>

#include <QDesktopWidget>

SubactivitiesForm::SubactivitiesForm()
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

SubactivitiesForm::~SubactivitiesForm()
{
}

bool SubactivitiesForm::filterOk(Activity* act)
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

void SubactivitiesForm::filterChanged()
{
	QString s;
	subactivitiesListBox->clear();
	visibleSubactivitiesList.clear();
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		if(this->filterOk(act)){
			s=act->getDescription(gt.rules);
			visibleSubactivitiesList.append(act);
			subactivitiesListBox->insertItem(s);
		}
	}
	subactivityChanged(subactivitiesListBox->currentItem());
}

void SubactivitiesForm::modifySubactivity()
{
	int ind=subactivitiesListBox->currentItem();
	if(ind<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected subactivity"));
		return;
	}

	Activity* act=visibleSubactivitiesList[ind];
	assert(act!=NULL);
	
	ModifySubactivityForm* modifySubactivityForm=new ModifySubactivityForm(act->id, act->activityGroupId);
	modifySubactivityForm->exec();

	filterChanged();
	
	subactivitiesListBox->setCurrentItem(ind);
}

void SubactivitiesForm::subactivityChanged(int index)
{
	if(index<0){
		currentSubactivityTextEdit->setText(QObject::tr("Invalid activity"));
		return;
	}

	QString s;
	Activity* act=visibleSubactivitiesList[index];

	assert(act!=NULL);
	s=act->getDetailedDescriptionWithConstraints(gt.rules);
	currentSubactivityTextEdit->setText(s);
}
