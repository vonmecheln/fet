/***************************************************************************
                          constraintsubjectactivitytagpreferredroomsform.cpp  -  description
                             -------------------
    begin                : 18 Aug 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#include "constraintsubjectactivitytagpreferredroomsform.h"
#include "addconstraintsubjectactivitytagpreferredroomsform.h"
#include "modifyconstraintsubjectactivitytagpreferredroomsform.h"

#include <QDesktopWidget>

ConstraintSubjectActivityTagPreferredRoomsForm::ConstraintSubjectActivityTagPreferredRoomsForm()
{
    setupUi(this);

    connect(addConstraintPushButton, SIGNAL(clicked()), this /*ConstraintSubjectActivityTagPreferredRoomsForm_template*/, SLOT(addConstraint()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*ConstraintSubjectActivityTagPreferredRoomsForm_template*/, SLOT(removeConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*ConstraintSubjectActivityTagPreferredRoomsForm_template*/, SLOT(close()));
    connect(constraintsListBox, SIGNAL(highlighted(int)), this /*ConstraintSubjectActivityTagPreferredRoomsForm_template*/, SLOT(constraintChanged(int)));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*ConstraintSubjectActivityTagPreferredRoomsForm_template*/, SLOT(modifyConstraint()));
    connect(subjectsComboBox, SIGNAL(activated(QString)), this /*ConstraintSubjectActivityTagPreferredRoomsForm_template*/, SLOT(filterChanged()));
    connect(constraintsListBox, SIGNAL(selected(QString)), this /*ConstraintSubjectActivityTagPreferredRoomsForm_template*/, SLOT(modifyConstraint()));
    connect(activityTagsComboBox, SIGNAL(activated(QString)), this /*ConstraintSubjectActivityTagPreferredRoomsForm_template*/, SLOT(filterChanged()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	subjectsComboBox->insertItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsComboBox->insertItem(sb->name);
	}

	activityTagsComboBox->insertItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* sb=gt.rules.activityTagsList[i];
		activityTagsComboBox->insertItem(sb->name);
	}

	this->refreshConstraintsListBox();
}

ConstraintSubjectActivityTagPreferredRoomsForm::~ConstraintSubjectActivityTagPreferredRoomsForm()
{
}

void ConstraintSubjectActivityTagPreferredRoomsForm::refreshConstraintsListBox()
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

void ConstraintSubjectActivityTagPreferredRoomsForm::filterChanged()
{
	this->refreshConstraintsListBox();
}

bool ConstraintSubjectActivityTagPreferredRoomsForm::filterOk(SpaceConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS){
		ConstraintSubjectActivityTagPreferredRooms* c=(ConstraintSubjectActivityTagPreferredRooms*)ctr;
		return (c->subjectName==subjectsComboBox->currentText() || subjectsComboBox->currentText()=="") &&
		 (c->activityTagName==activityTagsComboBox->currentText() || activityTagsComboBox->currentText()=="");
	}
	else
		return false;
}

void ConstraintSubjectActivityTagPreferredRoomsForm::constraintChanged(int index)
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

void ConstraintSubjectActivityTagPreferredRoomsForm::addConstraint()
{
	AddConstraintSubjectActivityTagPreferredRoomsForm form;
	form.exec();

	this->refreshConstraintsListBox();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintSubjectActivityTagPreferredRoomsForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintSubjectActivityTagPreferredRoomsForm form((ConstraintSubjectActivityTagPreferredRooms*)ctr);
	form.exec();

	this->refreshConstraintsListBox();
	
	constraintsListBox->setCurrentItem(i);
}

void ConstraintSubjectActivityTagPreferredRoomsForm::removeConstraint()
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
