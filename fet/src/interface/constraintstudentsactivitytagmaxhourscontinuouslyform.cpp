/***************************************************************************
                          constraintstudentsactivitytagmaxhourscontinuouslyform.cpp  -  description
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

#include "longtextmessagebox.h"

#include "constraintstudentsactivitytagmaxhourscontinuouslyform.h"
#include "addconstraintstudentsactivitytagmaxhourscontinuouslyform.h"
#include "modifyconstraintstudentsactivitytagmaxhourscontinuouslyform.h"

#include <QDesktopWidget>

ConstraintStudentsActivityTagMaxHoursContinuouslyForm::ConstraintStudentsActivityTagMaxHoursContinuouslyForm()
{
    setupUi(this);

    connect(constraintsListBox, SIGNAL(highlighted(int)), this /*ConstraintStudentsActivityTagMaxHoursContinuouslyForm_template*/, SLOT(constraintChanged(int)));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*ConstraintStudentsActivityTagMaxHoursContinuouslyForm_template*/, SLOT(addConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*ConstraintStudentsActivityTagMaxHoursContinuouslyForm_template*/, SLOT(close()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*ConstraintStudentsActivityTagMaxHoursContinuouslyForm_template*/, SLOT(removeConstraint()));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*ConstraintStudentsActivityTagMaxHoursContinuouslyForm_template*/, SLOT(modifyConstraint()));
    connect(constraintsListBox, SIGNAL(selected(QString)), this /*ConstraintStudentsActivityTagMaxHoursContinuouslyForm_template*/, SLOT(modifyConstraint()));
    connect(activityTagsComboBox, SIGNAL(activated(QString)), this /*ConstraintStudentsActivityTagMaxHoursContinuouslyForm_template*/, SLOT(filterChanged()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	activityTagsComboBox->clear();
	activityTagsComboBox->insertItem("");
	foreach(ActivityTag* at, gt.rules.activityTagsList)
		activityTagsComboBox->insertItem(at->name);

	this->filterChanged();
}

ConstraintStudentsActivityTagMaxHoursContinuouslyForm::~ConstraintStudentsActivityTagMaxHoursContinuouslyForm()
{
}

bool ConstraintStudentsActivityTagMaxHoursContinuouslyForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY){
		ConstraintStudentsActivityTagMaxHoursContinuously* c=(ConstraintStudentsActivityTagMaxHoursContinuously*) ctr;
		return (c->activityTagName==activityTagsComboBox->currentText() || activityTagsComboBox->currentText()=="");
	}
	else
		return false;
}

void ConstraintStudentsActivityTagMaxHoursContinuouslyForm::filterChanged()
{
	this->visibleConstraintsList.clear();
	constraintsListBox->clear();
	for(int i=0; i<gt.rules.timeConstraintsList.size(); i++){
		TimeConstraint* ctr=gt.rules.timeConstraintsList[i];
		if(filterOk(ctr)){
			visibleConstraintsList.append(ctr);
			constraintsListBox->insertItem(ctr->getDescription(gt.rules));
		}
	}
}

void ConstraintStudentsActivityTagMaxHoursContinuouslyForm::constraintChanged(int index)
{
	if(index<0)
		return;
	assert(index<this->visibleConstraintsList.size());
	TimeConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	currentConstraintTextEdit->setText(ctr->getDetailedDescription(gt.rules));
}

void ConstraintStudentsActivityTagMaxHoursContinuouslyForm::addConstraint()
{
	AddConstraintStudentsActivityTagMaxHoursContinuouslyForm form;
	form.exec();

	filterChanged();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintStudentsActivityTagMaxHoursContinuouslyForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm form((ConstraintStudentsActivityTagMaxHoursContinuously*)ctr);
	form.exec();

	filterChanged();
	constraintsListBox->setCurrentItem(i);
}

void ConstraintStudentsActivityTagMaxHoursContinuouslyForm::removeConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);
	QString s;
	s=tr("Remove constraint?");
	s+="\n\n";
	s+=ctr->getDetailedDescription(gt.rules);
	//s+="\n";
	//s+=tr("Are you sure?");

	switch( LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), 0, 0, 1 ) ){
	case 0: // The user clicked the OK again button or pressed Enter
		gt.rules.removeTimeConstraint(ctr);
		filterChanged();
		break;
	case 1: // The user clicked the Cancel or pressed Escape
		break;
	}
	
	if((uint)(i) >= constraintsListBox->count())
		i=constraintsListBox->count()-1;
	constraintsListBox->setCurrentItem(i);
}
