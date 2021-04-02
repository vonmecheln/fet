/***************************************************************************
                          constraintbasiccompulsorytimeform.cpp  -  description
                             -------------------
    begin                : Feb 10, 2005
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

#include "longtextmessagebox.h"

#include "constraintbasiccompulsorytimeform.h"
#include "addconstraintbasiccompulsorytimeform.h"
#include "modifyconstraintbasiccompulsorytimeform.h"

ConstraintBasicCompulsoryTimeForm::ConstraintBasicCompulsoryTimeForm()
{
    setupUi(this);

    connect(constraintsListBox, SIGNAL(highlighted(int)), this /*ConstraintBasicCompulsoryTimeForm_template*/, SLOT(constraintChanged(int)));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*ConstraintBasicCompulsoryTimeForm_template*/, SLOT(addConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*ConstraintBasicCompulsoryTimeForm_template*/, SLOT(close()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*ConstraintBasicCompulsoryTimeForm_template*/, SLOT(removeConstraint()));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*ConstraintBasicCompulsoryTimeForm_template*/, SLOT(modifyConstraint()));
    connect(constraintsListBox, SIGNAL(selected(QString)), this /*ConstraintBasicCompulsoryTimeForm_template*/, SLOT(modifyConstraint()));


	//setWindowFlags(Qt::Window);
	//setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	/*QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	this->filterChanged();
}

ConstraintBasicCompulsoryTimeForm::~ConstraintBasicCompulsoryTimeForm()
{
}

bool ConstraintBasicCompulsoryTimeForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME)
		return true;
	else
		return false;
}

void ConstraintBasicCompulsoryTimeForm::filterChanged()
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

	constraintsListBox->setCurrentItem(0);
	this->constraintChanged(constraintsListBox->currentItem());
}

void ConstraintBasicCompulsoryTimeForm::constraintChanged(int index)
{
	if(index<0){
		currentConstraintTextEdit->setText("");
		return;
	}
	assert(index<this->visibleConstraintsList.size());
	TimeConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	currentConstraintTextEdit->setText(ctr->getDetailedDescription(gt.rules));
}

void ConstraintBasicCompulsoryTimeForm::addConstraint()
{
	AddConstraintBasicCompulsoryTimeForm form;
	form.exec();

	filterChanged();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintBasicCompulsoryTimeForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintBasicCompulsoryTimeForm form((ConstraintBasicCompulsoryTime*)ctr);
	form.exec();

	filterChanged();
	constraintsListBox->setCurrentItem(i);
}

void ConstraintBasicCompulsoryTimeForm::removeConstraint()
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

	int lres=LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), 0, 0, 1 );
		
	if(lres==0){
		// The user clicked the OK again button or pressed Enter
		
		assert(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME);
		
		QString s=tr("Do you really want to remove the basic compulsory time constraint?");
		s+=" ";
		s+=tr("You cannot generate a timetable without this constraint.");
		s+="\n\n";
		s+=tr("Note: you can add again a constraint of this type from the menu Data -> Time constraints -> "
			"Miscellaneous -> Basic compulsory time constraints.");
			
		QMessageBox::StandardButton wr=QMessageBox::warning(this, tr("FET warning"), s,
			QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
			
		if(wr==QMessageBox::Yes){
			gt.rules.removeTimeConstraint(ctr);
			filterChanged();
		}
	}
	//else if(lres==1){
		// The user clicked the Cancel or pressed Escape
	//}
	
	if((uint)(i) >= constraintsListBox->count())
		i=constraintsListBox->count()-1;
	constraintsListBox->setCurrentItem(i);
}
