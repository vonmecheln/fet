/***************************************************************************
                          constraintteachersminhoursdailyform.cpp  -  description
                             -------------------
    begin                : Sept. 21, 2007
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

#include "constraintteachersminhoursdailyform.h"
#include "addconstraintteachersminhoursdailyform.h"
#include "modifyconstraintteachersminhoursdailyform.h"

#include <QDesktopWidget>

ConstraintTeachersMinHoursDailyForm::ConstraintTeachersMinHoursDailyForm()
{
    setupUi(this);

    connect(constraintsListBox, SIGNAL(highlighted(int)), this /*ConstraintTeachersMinHoursDailyForm_template*/, SLOT(constraintChanged(int)));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*ConstraintTeachersMinHoursDailyForm_template*/, SLOT(addConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*ConstraintTeachersMinHoursDailyForm_template*/, SLOT(close()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*ConstraintTeachersMinHoursDailyForm_template*/, SLOT(removeConstraint()));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*ConstraintTeachersMinHoursDailyForm_template*/, SLOT(modifyConstraint()));
    connect(constraintsListBox, SIGNAL(selected(QString)), this /*ConstraintTeachersMinHoursDailyForm_template*/, SLOT(modifyConstraint()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	this->filterChanged();
}

ConstraintTeachersMinHoursDailyForm::~ConstraintTeachersMinHoursDailyForm()
{
}

bool ConstraintTeachersMinHoursDailyForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_TEACHERS_MIN_HOURS_DAILY)
		return true;
	else
		return false;
}

void ConstraintTeachersMinHoursDailyForm::filterChanged()
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

void ConstraintTeachersMinHoursDailyForm::constraintChanged(int index)
{
	if(index<0)
		return;
	assert(index<this->visibleConstraintsList.size());
	TimeConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	currentConstraintTextEdit->setText(ctr->getDetailedDescription(gt.rules));
}

void ConstraintTeachersMinHoursDailyForm::addConstraint()
{
	AddConstraintTeachersMinHoursDailyForm form;
	form.exec();

	filterChanged();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintTeachersMinHoursDailyForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintTeachersMinHoursDailyForm form((ConstraintTeachersMinHoursDaily*)ctr);
	form.exec();

	filterChanged();
	constraintsListBox->setCurrentItem(i);
}

void ConstraintTeachersMinHoursDailyForm::removeConstraint()
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
	//s+=tr("\nAre you sure?");

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
