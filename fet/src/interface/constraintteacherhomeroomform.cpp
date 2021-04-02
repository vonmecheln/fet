/***************************************************************************
                          constraintteacherhomeroomform.cpp  -  description
                             -------------------
    begin                : Apr 8, 2005
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

#include "longtextmessagebox.h"

#include "constraintteacherhomeroomform.h"
#include "addconstraintteacherhomeroomform.h"
#include "modifyconstraintteacherhomeroomform.h"

#include <QDesktopWidget>

ConstraintTeacherHomeRoomForm::ConstraintTeacherHomeRoomForm()
{
    setupUi(this);

    connect(constraintsListBox, SIGNAL(highlighted(int)), this /*ConstraintTeacherHomeRoomForm_template*/, SLOT(constraintChanged(int)));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*ConstraintTeacherHomeRoomForm_template*/, SLOT(addConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*ConstraintTeacherHomeRoomForm_template*/, SLOT(close()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*ConstraintTeacherHomeRoomForm_template*/, SLOT(removeConstraint()));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*ConstraintTeacherHomeRoomForm_template*/, SLOT(modifyConstraint()));
    connect(constraintsListBox, SIGNAL(selected(QString)), this /*ConstraintTeacherHomeRoomForm_template*/, SLOT(modifyConstraint()));
    connect(teachersComboBox, SIGNAL(activated(QString)), this /*ConstraintTeacherHomeRoomForm_template*/, SLOT(filterChanged()));
    connect(roomsComboBox, SIGNAL(activated(QString)), this /*ConstraintTeacherHomeRoomForm_template*/, SLOT(filterChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	teachersComboBox->insertItem("");
	foreach(Teacher* tch, gt.rules.teachersList)
		teachersComboBox->insertItem(tch->name);
	
	roomsComboBox->insertItem("");
	foreach(Room* rm, gt.rules.roomsList)
		roomsComboBox->insertItem(rm->name);

	this->filterChanged();
}

ConstraintTeacherHomeRoomForm::~ConstraintTeacherHomeRoomForm()
{
}

bool ConstraintTeacherHomeRoomForm::filterOk(SpaceConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_TEACHER_HOME_ROOM){
		ConstraintTeacherHomeRoom* c=(ConstraintTeacherHomeRoom*) ctr;
		return (c->roomName==roomsComboBox->currentText() || roomsComboBox->currentText()=="")
		 && (c->teacherName==teachersComboBox->currentText() || teachersComboBox->currentText()=="");
	}
	else
		return false;
}

void ConstraintTeacherHomeRoomForm::filterChanged()
{
	this->visibleConstraintsList.clear();
	constraintsListBox->clear();
	for(int i=0; i<gt.rules.spaceConstraintsList.size(); i++){
		SpaceConstraint* ctr=gt.rules.spaceConstraintsList[i];
		if(filterOk(ctr)){
			visibleConstraintsList.append(ctr);
			constraintsListBox->insertItem(ctr->getDescription(gt.rules));
		}
	}
}

void ConstraintTeacherHomeRoomForm::constraintChanged(int index)
{
	if(index<0)
		return;
	assert(index<this->visibleConstraintsList.size());
	SpaceConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	currentConstraintTextEdit->setText(ctr->getDetailedDescription(gt.rules));
}

void ConstraintTeacherHomeRoomForm::addConstraint()
{
	AddConstraintTeacherHomeRoomForm form;
	form.exec();

	filterChanged();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintTeacherHomeRoomForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintTeacherHomeRoomForm form((ConstraintTeacherHomeRoom*)ctr);
	form.exec();

	filterChanged();
	constraintsListBox->setCurrentItem(i);
}

void ConstraintTeacherHomeRoomForm::removeConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);
	QString s;
	s=tr("Remove constraint?");
	s+="\n\n";
	s+=ctr->getDetailedDescription(gt.rules);
	//s+=tr("\nAre you sure?");

	switch( LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), 0, 0, 1 ) ){
	case 0: // The user clicked the OK again button or pressed Enter
		gt.rules.removeSpaceConstraint(ctr);
		filterChanged();
		break;
	case 1: // The user clicked the Cancel or pressed Escape
		break;
	}
	
	if((uint)(i) >= constraintsListBox->count())
		i=constraintsListBox->count()-1;
	constraintsListBox->setCurrentItem(i);
}
