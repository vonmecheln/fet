/***************************************************************************
                          constraintactivityendsteachersdayform.cpp  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QMessageBox>

#include "longtextmessagebox.h"

#include "constraintactivityendsteachersdayform.h"
#include "addconstraintactivityendsteachersdayform.h"
#include "modifyconstraintactivityendsteachersdayform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintActivityEndsTeachersDayForm::ConstraintActivityEndsTeachersDayForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);
	
	constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ConstraintActivityEndsTeachersDayForm::constraintChanged);
	connect(addConstraintPushButton, &QPushButton::clicked, this, &ConstraintActivityEndsTeachersDayForm::addConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &ConstraintActivityEndsTeachersDayForm::close);
	connect(removeConstraintPushButton, &QPushButton::clicked, this, &ConstraintActivityEndsTeachersDayForm::removeConstraint);
	connect(modifyConstraintPushButton, &QPushButton::clicked, this, &ConstraintActivityEndsTeachersDayForm::modifyConstraint);
	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &ConstraintActivityEndsTeachersDayForm::modifyConstraint);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	/////////////
	teachersComboBox->addItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->addItem(tch->name);
	}
	teachersComboBox->setCurrentIndex(0);

	subjectsComboBox->addItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsComboBox->addItem(sb->name);
	}
	subjectsComboBox->setCurrentIndex(0);

	activityTagsComboBox->addItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* st=gt.rules.activityTagsList[i];
		activityTagsComboBox->addItem(st->name);
	}
	activityTagsComboBox->setCurrentIndex(0);

	populateStudentsComboBox(studentsComboBox, QString(""), true);
	studentsComboBox->setCurrentIndex(0);
	///////////////

	this->filterChanged();

	connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintActivityEndsTeachersDayForm::filterChanged);
	connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintActivityEndsTeachersDayForm::filterChanged);
	connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintActivityEndsTeachersDayForm::filterChanged);
	connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintActivityEndsTeachersDayForm::filterChanged);
}

ConstraintActivityEndsTeachersDayForm::~ConstraintActivityEndsTeachersDayForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivityEndsTeachersDayForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type!=CONSTRAINT_ACTIVITY_ENDS_TEACHERS_DAY)
		return false;
		
	ConstraintActivityEndsTeachersDay* c=(ConstraintActivityEndsTeachersDay*) ctr;
	
	QString tn=teachersComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString sbtn=activityTagsComboBox->currentText();
	QString stn=studentsComboBox->currentText();
		
	bool found=true;
	
	int id=c->activityId;
	Activity* act=nullptr;
	for(Activity* a : std::as_const(gt.rules.activitiesList))
		if(a->id==id)
			act=a;

	found=true;
		
	if(act!=nullptr){
		//teacher
		if(tn!=""){
			bool ok2=false;
			for(QStringList::const_iterator it=act->teachersNames.constBegin(); it!=act->teachersNames.constEnd(); it++)
				if(*it == tn){
					ok2=true;
					break;
				}
			if(!ok2)
				found=false;
		}

		//subject
		if(sbn!="" && sbn!=act->subjectName)
			found=false;
	
		//activity tag
		if(sbtn!="" && !act->activityTagsNames.contains(sbtn))
			found=false;
	
		//students
		if(stn!=""){
			bool ok2=false;
			for(QStringList::const_iterator it=act->studentsNames.constBegin(); it!=act->studentsNames.constEnd(); it++)
				if(*it == stn){
					ok2=true;
					break;
			}
			if(!ok2)
				found=false;
		}
	}
	
	if(found)
		return true;

	return false;
}

void ConstraintActivityEndsTeachersDayForm::filterChanged()
{
	this->visibleConstraintsList.clear();
	constraintsListWidget->clear();
	for(int i=0; i<gt.rules.timeConstraintsList.size(); i++){
		TimeConstraint* ctr=gt.rules.timeConstraintsList[i];
		if(filterOk(ctr)){
			visibleConstraintsList.append(ctr);
			constraintsListWidget->addItem(ctr->getDescription(gt.rules));
		}
	}

	if(constraintsListWidget->count()>0)
		constraintsListWidget->setCurrentRow(0);
	else
		constraintChanged(-1);
}

void ConstraintActivityEndsTeachersDayForm::constraintChanged(int index)
{
	if(index<0){
		currentConstraintTextEdit->setPlainText("");
		return;
	}
	assert(index<this->visibleConstraintsList.size());
	TimeConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=nullptr);
	currentConstraintTextEdit->setPlainText(ctr->getDetailedDescription(gt.rules));
}

void ConstraintActivityEndsTeachersDayForm::addConstraint()
{
	AddConstraintActivityEndsTeachersDayForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintActivityEndsTeachersDayForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintActivityEndsTeachersDayForm form(this, (ConstraintActivityEndsTeachersDay*)ctr);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();

	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);

	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;
		
	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	else
		this->constraintChanged(-1);
}

void ConstraintActivityEndsTeachersDayForm::removeConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);
	QString s;
	s=tr("Remove constraint?");
	s+="\n\n";
	s+=ctr->getDetailedDescription(gt.rules);
	
	QListWidgetItem* item;

	QString oc;

	switch( LongTextMessageBox::confirmation( this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), QString(), 0, 1 ) ){
	case 0: // The user clicked the OK button or pressed Enter
		oc=ctr->getDetailedDescription(gt.rules);

		gt.rules.removeTimeConstraint(ctr);

		gt.rules.addUndoPoint(tr("Removed the constraint:\n\n%1").arg(oc));
		
		visibleConstraintsList.removeAt(i);
		constraintsListWidget->setCurrentRow(-1);
		item=constraintsListWidget->takeItem(i);
		delete item;
		
		break;
	case 1: // The user clicked the Cancel button or pressed Escape
		break;
	}
	
	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;
	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	else
		this->constraintChanged(-1);
}
