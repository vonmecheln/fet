/***************************************************************************
                          constraintstudentssethomeroomsform.cpp  -  description
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

#include "longtextmessagebox.h"

#include "constraintstudentssethomeroomsform.h"
#include "addconstraintstudentssethomeroomsform.h"
#include "modifyconstraintstudentssethomeroomsform.h"

#include <QDesktopWidget>

ConstraintStudentsSetHomeRoomsForm::ConstraintStudentsSetHomeRoomsForm()
{
    setupUi(this);

    connect(addConstraintPushButton, SIGNAL(clicked()), this /*ConstraintStudentsSetHomeRoomsForm_template*/, SLOT(addConstraint()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*ConstraintStudentsSetHomeRoomsForm_template*/, SLOT(removeConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*ConstraintStudentsSetHomeRoomsForm_template*/, SLOT(close()));
    connect(constraintsListBox, SIGNAL(highlighted(int)), this /*ConstraintStudentsSetHomeRoomsForm_template*/, SLOT(constraintChanged(int)));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*ConstraintStudentsSetHomeRoomsForm_template*/, SLOT(modifyConstraint()));
    connect(constraintsListBox, SIGNAL(selected(QString)), this /*ConstraintStudentsSetHomeRoomsForm_template*/, SLOT(modifyConstraint()));
    connect(studentsComboBox, SIGNAL(activated(QString)), this /*ConstraintStudentsSetHomeRoomsForm_template*/, SLOT(filterChanged()));
    connect(roomsComboBox, SIGNAL(activated(QString)), this /*ConstraintStudentsSetHomeRoomsForm_template*/, SLOT(filterChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	
	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
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
	
	roomsComboBox->insertItem("");
	foreach(Room* rm, gt.rules.roomsList)
		roomsComboBox->insertItem(rm->name);

	this->refreshConstraintsListBox();
}

ConstraintStudentsSetHomeRoomsForm::~ConstraintStudentsSetHomeRoomsForm()
{
}

void ConstraintStudentsSetHomeRoomsForm::refreshConstraintsListBox()
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

void ConstraintStudentsSetHomeRoomsForm::filterChanged()
{
	this->refreshConstraintsListBox();
}

bool ConstraintStudentsSetHomeRoomsForm::filterOk(SpaceConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_HOME_ROOMS){
		ConstraintStudentsSetHomeRooms* c=(ConstraintStudentsSetHomeRooms*)ctr;
		return (c->studentsName==studentsComboBox->currentText() || studentsComboBox->currentText()=="")
		  && (roomsComboBox->currentText()=="" || c->roomsNames.contains(roomsComboBox->currentText()));
	}
	else
		return false;
}

void ConstraintStudentsSetHomeRoomsForm::constraintChanged(int index)
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

void ConstraintStudentsSetHomeRoomsForm::addConstraint()
{
	AddConstraintStudentsSetHomeRoomsForm form;
	form.exec();

	this->refreshConstraintsListBox();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintStudentsSetHomeRoomsForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintStudentsSetHomeRoomsForm form((ConstraintStudentsSetHomeRooms*)ctr);
	form.exec();

	this->refreshConstraintsListBox();
	
	constraintsListBox->setCurrentItem(i);
}

void ConstraintStudentsSetHomeRoomsForm::removeConstraint()
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
		this->refreshConstraintsListBox();
		break;
	case 1: // The user clicked the Cancel or pressed Escape
		break;
	}

	if((uint)(i) >= constraintsListBox->count())
		i=constraintsListBox->count()-1;
	constraintsListBox->setCurrentItem(i);
}
