/***************************************************************************
                          constraintteacherhomeroomsform.cpp  -  description
                             -------------------
    begin                : 8 April 2005
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

#include "constraintteacherhomeroomsform.h"
#include "addconstraintteacherhomeroomsform.h"
#include "modifyconstraintteacherhomeroomsform.h"

#include <QDesktopWidget>

ConstraintTeacherHomeRoomsForm::ConstraintTeacherHomeRoomsForm()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	teachersComboBox->insertItem("");
	foreach(Teacher* tch, gt.rules.teachersList)
		teachersComboBox->insertItem(tch->name);
					
	roomsComboBox->insertItem("");
	foreach(Room* rm, gt.rules.roomsList)
		roomsComboBox->insertItem(rm->name);

	this->refreshConstraintsListBox();
}

ConstraintTeacherHomeRoomsForm::~ConstraintTeacherHomeRoomsForm()
{
}

void ConstraintTeacherHomeRoomsForm::refreshConstraintsListBox()
{
	this->visibleConstraintsList.clear();
	constraintsListBox->clear();
	for(int i=0; i<gt.rules.spaceConstraintsList.size(); i++){
		SpaceConstraint* ctr=gt.rules.spaceConstraintsList[i];
		if(filterOk(ctr)){
			QString s;
			s=ctr->getDescription(gt.rules);
			visibleConstraintsList.append(ctr);
			constraintsListBox->insertItem(s);
		}
	}

	constraintsListBox->setCurrentItem(0);
	this->constraintChanged(constraintsListBox->currentItem());
}

void ConstraintTeacherHomeRoomsForm::filterChanged()
{
	this->refreshConstraintsListBox();
}

bool ConstraintTeacherHomeRoomsForm::filterOk(SpaceConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_TEACHER_HOME_ROOMS){
		ConstraintTeacherHomeRooms* c=(ConstraintTeacherHomeRooms*)ctr;
		return (c->teacherName==teachersComboBox->currentText() || teachersComboBox->currentText()=="")
		  && (roomsComboBox->currentText()=="" || c->roomsNames.contains(roomsComboBox->currentText()));
	}
	else
		return false;
}

void ConstraintTeacherHomeRoomsForm::constraintChanged(int index)
{
	if(index<0)
		return;
	QString s;
	assert(index<this->visibleConstraintsList.size());
	SpaceConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	s=ctr->getDetailedDescription(gt.rules);
	currentConstraintTextEdit->setText(s);
}

void ConstraintTeacherHomeRoomsForm::addConstraint()
{
	AddConstraintTeacherHomeRoomsForm *form=new AddConstraintTeacherHomeRoomsForm();
	form->exec();

	this->refreshConstraintsListBox();
}

void ConstraintTeacherHomeRoomsForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintTeacherHomeRoomsForm *form=new ModifyConstraintTeacherHomeRoomsForm((ConstraintTeacherHomeRooms*)ctr);
	form->exec();

	this->refreshConstraintsListBox();
	
	constraintsListBox->setCurrentItem(i);
}

void ConstraintTeacherHomeRoomsForm::removeConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);
	QString s;
	s=QObject::tr("Removing constraint:\n");
	s+=ctr->getDetailedDescription(gt.rules);
	s+=QObject::tr("\nAre you sure?");

	switch( QMessageBox::warning( this, QObject::tr("FET warning"),
		s, QObject::tr("OK"), QObject::tr("Cancel"), 0, 0, 1 ) ){
	case 0: // The user clicked the OK again button or pressed Enter
		gt.rules.removeSpaceConstraint(ctr);
		this->refreshConstraintsListBox();
		break;
	case 1: // The user clicked the Cancel or pressed Escape
		break;
	}

	if((uint)(i) >= constraintsListBox->count())
		i=constraintsListBox->count()-1;
	constraintsListBox->setCurrentItem(i);
}
