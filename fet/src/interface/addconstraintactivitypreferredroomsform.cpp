/***************************************************************************
                          addconstraintactivitypreferredroomsform.cpp  -  description
                             -------------------
    begin                : March 28, 2005
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

#include "addconstraintactivitypreferredroomsform.h"
#include "spaceconstraint.h"

AddConstraintActivityPreferredRoomsForm::AddConstraintActivityPreferredRoomsForm()
{
    setupUi(this);

//    QObject::connect(addPushButton, SIGNAL(clicked()), this /*AddConstraintActivityPreferredRoomsForm_template*/, SLOT(addRoom()));
//    QObject::connect(removePushButton, SIGNAL(clicked()), this /*AddConstraintActivityPreferredRoomsForm_template*/, SLOT(removeRoom()));
    connect(closePushButton, SIGNAL(clicked()), this /*AddConstraintActivityPreferredRoomsForm_template*/, SLOT(close()));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*AddConstraintActivityPreferredRoomsForm_template*/, SLOT(addConstraint()));
    connect(roomsListBox, SIGNAL(selected(QString)), this /*AddConstraintActivityPreferredRoomsForm_template*/, SLOT(addRoom()));
    connect(selectedRoomsListBox, SIGNAL(selected(QString)), this /*AddConstraintActivityPreferredRoomsForm_template*/, SLOT(removeRoom()));
    connect(teachersComboBox, SIGNAL(activated(QString)), this /*AddConstraintActivityPreferredRoomsForm_template*/, SLOT(filterChanged()));
    connect(studentsComboBox, SIGNAL(activated(QString)), this /*AddConstraintActivityPreferredRoomsForm_template*/, SLOT(filterChanged()));
    connect(subjectsComboBox, SIGNAL(activated(QString)), this /*AddConstraintActivityPreferredRoomsForm_template*/, SLOT(filterChanged()));
    connect(activityTagsComboBox, SIGNAL(activated(QString)), this /*AddConstraintActivityPreferredRoomsForm_template*/, SLOT(filterChanged()));

    connect(clearPushButton, SIGNAL(clicked()), this, SLOT(clear()));

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
	
	QSize tmp5=activitiesComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);

	activitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	
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
	
	updateActivitiesComboBox();


	updateRoomsListBox();
	
	/*for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* ac=gt.rules.activitiesList[i];
		activitiesComboBox->insertItem(ac->getDescription(gt.rules));
	}*/
}

AddConstraintActivityPreferredRoomsForm::~AddConstraintActivityPreferredRoomsForm()
{
}



bool AddConstraintActivityPreferredRoomsForm::filterOk(Activity* act)
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

void AddConstraintActivityPreferredRoomsForm::updateActivitiesComboBox(){
	activitiesComboBox->clear();
	activitiesList.clear();
	
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		
		if(filterOk(act)){
			activitiesComboBox->insertItem(act->getDescription(gt.rules));
			this->activitiesList.append(act->id);
		}
	}
}

void AddConstraintActivityPreferredRoomsForm::filterChanged()
{
	this->updateActivitiesComboBox();
}




void AddConstraintActivityPreferredRoomsForm::updateRoomsListBox()
{
	roomsListBox->clear();
	selectedRoomsListBox->clear();

	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm= gt.rules.roomsList[i];
		roomsListBox->insertItem(rm->name);
	}
}

void AddConstraintActivityPreferredRoomsForm::addConstraint()
{
	SpaceConstraint *ctr=NULL;

	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	if(selectedRoomsListBox->count()==0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Empty list of selected rooms"));
		return;
	}
	if(selectedRoomsListBox->count()==1){
		QMessageBox::warning(this, tr("FET information"),
			tr("Only one selected room - please use constraint activity preferred room if you want a single room"));
		return;
	}


	int id;
	int tmp2=activitiesComboBox->currentItem();
	//assert(tmp2<gt.rules.activitiesList.size());
	//assert(tmp2<activitiesList.size());
	if(tmp2<0 || tmp2>=gt.rules.activitiesList.size() || tmp2>=activitiesList.size()){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid activity"));
		return;
	}
	else
		id=activitiesList.at(tmp2);
		
	/*if(activitiesComboBox->currentItem()<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid selected activity"));
		return;	
	}
	int id=gt.rules.activitiesList[activitiesComboBox->currentItem()]->id;*/
	
	QStringList roomsList;
	for(uint i=0; i<selectedRoomsListBox->count(); i++)
		roomsList.append(selectedRoomsListBox->text(i));
	
	ctr=new ConstraintActivityPreferredRooms(weight, id, roomsList);
	bool tmp3=gt.rules.addSpaceConstraint(ctr);
	
	if(tmp3){
		QString s=tr("Constraint added:");
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

void AddConstraintActivityPreferredRoomsForm::addRoom()
{
	if(roomsListBox->currentItem()<0)
		return;
	QString rmName=roomsListBox->currentText();
	assert(rmName!="");
	uint i;
	//duplicate?
	for(i=0; i<selectedRoomsListBox->count(); i++)
		if(rmName==selectedRoomsListBox->text(i))
			break;
	if(i<selectedRoomsListBox->count())
		return;
	selectedRoomsListBox->insertItem(rmName);
}

void AddConstraintActivityPreferredRoomsForm::removeRoom()
{
	if(selectedRoomsListBox->currentItem()<0 || selectedRoomsListBox->count()<=0)
		return;		
	selectedRoomsListBox->removeItem(selectedRoomsListBox->currentItem());
}

void AddConstraintActivityPreferredRoomsForm::clear()
{
	selectedRoomsListBox->clear();
}
