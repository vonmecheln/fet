/***************************************************************************
                          constraintactivitiesmaxhourlyspanform.cpp  -  description
                             -------------------
    begin                : 2023
    copyright            : (C) 2023 by Liviu Lalescu
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

#include "constraintactivitiesmaxhourlyspanform.h"
#include "addconstraintactivitiesmaxhourlyspanform.h"
#include "modifyconstraintactivitiesmaxhourlyspanform.h"

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

ConstraintActivitiesMaxHourlySpanForm::ConstraintActivitiesMaxHourlySpanForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);

	constraintsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &ConstraintActivitiesMaxHourlySpanForm::constraintChanged);
	connect(addConstraintPushButton, &QPushButton::clicked, this, &ConstraintActivitiesMaxHourlySpanForm::addConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &ConstraintActivitiesMaxHourlySpanForm::close);
	connect(removeConstraintPushButton, &QPushButton::clicked, this, &ConstraintActivitiesMaxHourlySpanForm::removeConstraint);
	connect(modifyConstraintPushButton, &QPushButton::clicked, this, &ConstraintActivitiesMaxHourlySpanForm::modifyConstraint);
	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &ConstraintActivitiesMaxHourlySpanForm::modifyConstraint);

	connect(helpPushButton, &QPushButton::clicked, this, &ConstraintActivitiesMaxHourlySpanForm::help);

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

	connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintActivitiesMaxHourlySpanForm::filterChanged);
	connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintActivitiesMaxHourlySpanForm::filterChanged);
	connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintActivitiesMaxHourlySpanForm::filterChanged);
	connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ConstraintActivitiesMaxHourlySpanForm::filterChanged);
}

ConstraintActivitiesMaxHourlySpanForm::~ConstraintActivitiesMaxHourlySpanForm()
{
	saveFETDialogGeometry(this);
}

bool ConstraintActivitiesMaxHourlySpanForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type!=CONSTRAINT_ACTIVITIES_MAX_HOURLY_SPAN)
		return false;
		
	ConstraintActivitiesMaxHourlySpan* c=(ConstraintActivitiesMaxHourlySpan*) ctr;
	
	QString tn=teachersComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString atn=activityTagsComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	
	if(tn=="" && sbn=="" && atn=="" && stn=="")
		return true;
	
	bool foundTeacher=false, foundStudents=false, foundSubject=false, foundActivityTag=false;
		
	for(int i=0; i<c->n_activities; i++){
		int id=c->activitiesIds[i];
		/*Activity* act=nullptr;
		for(Activity* a : std::as_const(gt.rules.activitiesList))
			if(a->id==id)
				act=a;*/
		Activity* act=gt.rules.activitiesPointerHash.value(id, nullptr);
		
		if(act!=nullptr){
			//teacher
			if(tn!=""){
				bool ok2=false;
				for(QStringList::const_iterator it=act->teachersNames.constBegin(); it!=act->teachersNames.constEnd(); it++)
					if(*it == tn){
						ok2=true;
						break;
					}
				if(ok2)
					foundTeacher=true;
			}
			else
				foundTeacher=true;

			//subject
			if(sbn!="" && sbn!=act->subjectName)
				;
			else
				foundSubject=true;
		
			//activity tag
			if(atn!="" && !act->activityTagsNames.contains(atn))
				;
			else
				foundActivityTag=true;
		
			//students
			if(stn!=""){
				bool ok2=false;
				for(QStringList::const_iterator it=act->studentsNames.constBegin(); it!=act->studentsNames.constEnd(); it++)
					if(*it == stn){
						ok2=true;
						break;
				}
				if(ok2)
					foundStudents=true;
			}
			else
				foundStudents=true;
		}
	}
	
	if(foundTeacher && foundStudents && foundSubject && foundActivityTag)
		return true;
	else
		return false;
}

void ConstraintActivitiesMaxHourlySpanForm::filterChanged()
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

void ConstraintActivitiesMaxHourlySpanForm::constraintChanged(int index)
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

void ConstraintActivitiesMaxHourlySpanForm::addConstraint()
{
	AddConstraintActivitiesMaxHourlySpanForm form(this);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
}

void ConstraintActivitiesMaxHourlySpanForm::modifyConstraint()
{
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintActivitiesMaxHourlySpanForm form(this, (ConstraintActivitiesMaxHourlySpan*)ctr);
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

void ConstraintActivitiesMaxHourlySpanForm::removeConstraint()
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

void ConstraintActivitiesMaxHourlySpanForm::help()
{
	QString s;
	
	s+=tr("Help about the constraint activities max hourly span:");
	s+="\n\n";
	s+=tr("This constraint was suggested by %1.", "%1 is a user").arg("pnlshd");
	s+=" ";
	s+=tr("The activities can be on any days, but their hourly span must be limited by the specified maximum value.");
	s+="\n\n";
	s+=tr("More details and an example: the hourly span of the a set of activities is the number of hours between the start of the earliest activity"
	 " and the end of the latest activity, neglecting their days. So, if for instance the constraint addresses A1, A2, A3, and A4, each of them having duration 1,"
	 " and say: A1 is on day 1 hour 2, A2 is on day 1 hour 3, A3 is on day 3 hour 5, and A4 is on day 5 hour 2, then the earliest activities"
	 " (neglecting their days) are A1 and A4 (starting at hour 2), and the latest activity (neglecting its day) is A3 (ending after hour 5)."
	 " The hourly span of A1, A2, A3, and A4 is thus (5 + 1) - 2 = 4.");

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
