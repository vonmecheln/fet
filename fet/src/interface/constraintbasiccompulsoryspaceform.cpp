/***************************************************************************
                          constraintbasiccompulsoryspaceform.cpp  -  description
                             -------------------
    begin                : Feb 13, 2005
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

#include "constraintbasiccompulsoryspaceform.h"
#include "addconstraintbasiccompulsoryspaceform.h"
#include "modifyconstraintbasiccompulsoryspaceform.h"

ConstraintBasicCompulsorySpaceForm::ConstraintBasicCompulsorySpaceForm()
{
    setupUi(this);

    connect(constraintsListBox, SIGNAL(highlighted(int)), this /*ConstraintBasicCompulsorySpaceForm_template*/, SLOT(constraintChanged(int)));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*ConstraintBasicCompulsorySpaceForm_template*/, SLOT(addConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*ConstraintBasicCompulsorySpaceForm_template*/, SLOT(close()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*ConstraintBasicCompulsorySpaceForm_template*/, SLOT(removeConstraint()));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*ConstraintBasicCompulsorySpaceForm_template*/, SLOT(modifyConstraint()));
    connect(constraintsListBox, SIGNAL(selected(QString)), this /*ConstraintBasicCompulsorySpaceForm_template*/, SLOT(modifyConstraint()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);
	
	this->filterChanged();
}

ConstraintBasicCompulsorySpaceForm::~ConstraintBasicCompulsorySpaceForm()
{
}

bool ConstraintBasicCompulsorySpaceForm::filterOk(SpaceConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE)
		return true;
	else
		return false;
}

void ConstraintBasicCompulsorySpaceForm::filterChanged()
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
	
	constraintsListBox->setCurrentItem(0);
	this->constraintChanged(constraintsListBox->currentItem());
}

void ConstraintBasicCompulsorySpaceForm::constraintChanged(int index)
{
	if(index<0){
		currentConstraintTextEdit->setText("");
		return;
	}
	assert(index<this->visibleConstraintsList.size());
	SpaceConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	currentConstraintTextEdit->setText(ctr->getDetailedDescription(gt.rules));
}

void ConstraintBasicCompulsorySpaceForm::addConstraint()
{
	AddConstraintBasicCompulsorySpaceForm form;
	form.exec();

	filterChanged();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintBasicCompulsorySpaceForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintBasicCompulsorySpaceForm form((ConstraintBasicCompulsorySpace*)ctr);
	form.exec();

	filterChanged();
	constraintsListBox->setCurrentItem(i);
}

void ConstraintBasicCompulsorySpaceForm::removeConstraint()
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

	int lres=LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), 0, 0, 1 );
		
	if(lres==0){
		// The user clicked the OK button or pressed Enter
		
		assert(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE);
		
		QString s=tr("Do you really want to remove the basic compulsory space constraint?");
		s+=" ";
		s+=tr("You cannot generate a timetable without this constraint.");
		s+="\n\n";
		s+=tr("Note: you can add again a constraint of this type from the menu Data -> Space constraints -> "
			"Miscellaneous -> Basic compulsory space constraints.");
		
		QMessageBox::StandardButton wr=QMessageBox::warning(this, tr("FET warning"), s,
			QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		
		if(wr==QMessageBox::Yes){
			gt.rules.removeSpaceConstraint(ctr);
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
