/***************************************************************************
                          constraintstudentssetmingapsbetweenbuildingchangesform.cpp  -  description
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

#include "longtextmessagebox.h"

#include "constraintstudentssetmingapsbetweenbuildingchangesform.h"
#include "addconstraintstudentssetmingapsbetweenbuildingchangesform.h"
#include "modifyconstraintstudentssetmingapsbetweenbuildingchangesform.h"

#include <QDesktopWidget>

ConstraintStudentsSetMinGapsBetweenBuildingChangesForm::ConstraintStudentsSetMinGapsBetweenBuildingChangesForm()
{
    setupUi(this);

    connect(constraintsListBox, SIGNAL(highlighted(int)), this /*ConstraintStudentsSetMinGapsBetweenBuildingChangesForm_template*/, SLOT(constraintChanged(int)));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*ConstraintStudentsSetMinGapsBetweenBuildingChangesForm_template*/, SLOT(addConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*ConstraintStudentsSetMinGapsBetweenBuildingChangesForm_template*/, SLOT(close()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*ConstraintStudentsSetMinGapsBetweenBuildingChangesForm_template*/, SLOT(removeConstraint()));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*ConstraintStudentsSetMinGapsBetweenBuildingChangesForm_template*/, SLOT(modifyConstraint()));
    connect(studentsComboBox, SIGNAL(activated(QString)), this /*ConstraintStudentsSetMinGapsBetweenBuildingChangesForm_template*/, SLOT(filterChanged()));
    connect(constraintsListBox, SIGNAL(selected(QString)), this /*ConstraintStudentsSetMinGapsBetweenBuildingChangesForm_template*/, SLOT(modifyConstraint()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	
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

	this->filterChanged();
}

ConstraintStudentsSetMinGapsBetweenBuildingChangesForm::~ConstraintStudentsSetMinGapsBetweenBuildingChangesForm()
{
}

bool ConstraintStudentsSetMinGapsBetweenBuildingChangesForm::filterOk(SpaceConstraint* ctr)
{
	if(ctr->type==CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES){
		ConstraintStudentsSetMinGapsBetweenBuildingChanges* c=(ConstraintStudentsSetMinGapsBetweenBuildingChanges*) ctr;
		return c->studentsName==studentsComboBox->currentText() || studentsComboBox->currentText()=="";
	}
	else
		return false;
}

void ConstraintStudentsSetMinGapsBetweenBuildingChangesForm::filterChanged()
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

void ConstraintStudentsSetMinGapsBetweenBuildingChangesForm::constraintChanged(int index)
{
	if(index<0)
		return;
	assert(index<this->visibleConstraintsList.size());
	SpaceConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	currentConstraintTextEdit->setText(ctr->getDetailedDescription(gt.rules));
}

void ConstraintStudentsSetMinGapsBetweenBuildingChangesForm::addConstraint()
{
	AddConstraintStudentsSetMinGapsBetweenBuildingChangesForm form;
	form.exec();

	filterChanged();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintStudentsSetMinGapsBetweenBuildingChangesForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	SpaceConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintStudentsSetMinGapsBetweenBuildingChangesForm form((ConstraintStudentsSetMinGapsBetweenBuildingChanges*)ctr);
	form.exec();

	filterChanged();
	constraintsListBox->setCurrentItem(i);
}

void ConstraintStudentsSetMinGapsBetweenBuildingChangesForm::removeConstraint()
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
