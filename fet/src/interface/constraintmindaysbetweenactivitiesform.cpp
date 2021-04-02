/***************************************************************************
                          constraintmindaysbetweenactivitiesform.cpp  -  description
                             -------------------
    begin                : Feb 11, 2005
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

#include "constraintmindaysbetweenactivitiesform.h"
#include "addconstraintmindaysbetweenactivitiesform.h"
#include "modifyconstraintmindaysbetweenactivitiesform.h"

#include "changemindaysselectivelyform.h"

#include <QDesktopWidget>

#include <QInputDialog>

ConstraintMinDaysBetweenActivitiesForm::ConstraintMinDaysBetweenActivitiesForm()
{
    setupUi(this);

    connect(constraintsListBox, SIGNAL(highlighted(int)), this /*ConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(constraintChanged(int)));
    connect(addConstraintPushButton, SIGNAL(clicked()), this /*ConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(addConstraint()));
    connect(closePushButton, SIGNAL(clicked()), this /*ConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(close()));
    connect(removeConstraintPushButton, SIGNAL(clicked()), this /*ConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(removeConstraint()));
    connect(modifyConstraintPushButton, SIGNAL(clicked()), this /*ConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(modifyConstraint()));
    connect(constraintsListBox, SIGNAL(selected(QString)), this /*ConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(modifyConstraint()));
    connect(teachersComboBox, SIGNAL(activated(QString)), this /*ConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(filterChanged()));
    connect(studentsComboBox, SIGNAL(activated(QString)), this /*ConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(filterChanged()));
    connect(subjectsComboBox, SIGNAL(activated(QString)), this /*ConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(filterChanged()));
    connect(activityTagsComboBox, SIGNAL(activated(QString)), this /*ConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(filterChanged()));
    connect(changeSelectivelyPushButton, SIGNAL(clicked()), this /*ConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(changeSelectively()));


	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
/////////////
	teachersComboBox->insertItem("");
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->insertItem(tch->name);
	}
	teachersComboBox->setCurrentItem(0);

	subjectsComboBox->insertItem("");
	for(int i=0; i<gt.rules.subjectsList.size(); i++){
		Subject* sb=gt.rules.subjectsList[i];
		subjectsComboBox->insertItem(sb->name);
	}
	subjectsComboBox->setCurrentItem(0);

	activityTagsComboBox->insertItem("");
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* st=gt.rules.activityTagsList[i];
		activityTagsComboBox->insertItem(st->name);
	}
	activityTagsComboBox->setCurrentItem(0);

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
	studentsComboBox->setCurrentItem(0);
///////////////

	this->filterChanged();
}

ConstraintMinDaysBetweenActivitiesForm::~ConstraintMinDaysBetweenActivitiesForm()
{
}

bool ConstraintMinDaysBetweenActivitiesForm::filterOk(TimeConstraint* ctr)
{
	if(ctr->type!=CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES)
		return false;
		
	ConstraintMinDaysBetweenActivities* c=(ConstraintMinDaysBetweenActivities*) ctr;
	
	QString tn=teachersComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString sbtn=activityTagsComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	
	if(tn=="" && sbn=="" && sbtn=="" && stn=="")
		return true;
	
	bool foundTeacher=false, foundStudents=false, foundSubject=false, foundActivityTag=false;
		
	for(int i=0; i<c->n_activities; i++){
		//bool found=true;
	
		int id=c->activitiesId[i];
		Activity* act=NULL;
		foreach(Activity* a, gt.rules.activitiesList)
			if(a->id==id)
				act=a;
		
		if(act!=NULL){
			//teacher
			if(tn!=""){
				bool ok2=false;
				for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
					if(*it == tn){
						ok2=true;
						break;
					}
				if(ok2)
					foundTeacher=true;
				//if(!ok2)
				//	found=false;
			}
			else
				foundTeacher=true;

			//subject
			if(sbn!="" && sbn!=act->subjectName)
				;
			else
				//found=false;
				foundSubject=true;
		
			//activity tag
//			if(sbtn!="" && sbtn!=act->activityTagName)
			if(sbtn!="" && !act->activityTagsNames.contains(sbtn))
				;
			else
				//found=false;
				foundActivityTag=true;
		
			//students
			if(stn!=""){
				bool ok2=false;
				for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
					if(*it == stn){
						ok2=true;
						break;
				}
				//if(!ok2)
				//	found=false;
				if(ok2)
					foundStudents=true;
			}
			else
				foundStudents=true;
		}
		
		//if(found)
		//	return true;
	}
	
	if(foundTeacher && foundStudents && foundSubject && foundActivityTag)
		return true;
	else
		return false;
}

void ConstraintMinDaysBetweenActivitiesForm::filterChanged()
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
	if(visibleConstraintsList.count()>0)
		constraintChanged(0);
	else
		constraintChanged(-1);
}

void ConstraintMinDaysBetweenActivitiesForm::constraintChanged(int index)
{
	if(index<0){
		currentConstraintTextEdit->setText(tr("Invalid constraint"));
	
		return;
	}
	assert(index<this->visibleConstraintsList.size());
	TimeConstraint* ctr=this->visibleConstraintsList.at(index);
	assert(ctr!=NULL);
	currentConstraintTextEdit->setText(ctr->getDetailedDescription(gt.rules));
}

void ConstraintMinDaysBetweenActivitiesForm::addConstraint()
{
	AddConstraintMinDaysBetweenActivitiesForm form;
	form.exec();

	filterChanged();
	
	constraintsListBox->setCurrentItem(constraintsListBox->count()-1);
}

void ConstraintMinDaysBetweenActivitiesForm::modifyConstraint()
{
	int i=constraintsListBox->currentItem();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	TimeConstraint* ctr=this->visibleConstraintsList.at(i);

	ModifyConstraintMinDaysBetweenActivitiesForm form((ConstraintMinDaysBetweenActivities*)ctr);
	form.exec();

	filterChanged();
	constraintsListBox->setCurrentItem(i);
}

void ConstraintMinDaysBetweenActivitiesForm::removeConstraint()
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

/*
void ConstraintMinDaysBetweenActivitiesForm::changeAllWeights()
{
	bool ok = FALSE;
	QString s;
	s = QInputDialog::getText( tr("Modifying all weights for min days"), tr("Warning: all min days weights will be\n"
	 " changed to selected value. Are you sure?\n If yes, please enter weight percentage for all constraints of\n"
	 " type min days between activities (any integer/fractional\nnumber between 0.0 and 100.0, recommended 95.0 at least)") ,
     QLineEdit::Normal, QString::null, &ok, this );

	if ( ok && !s.isEmpty() ){
		double weight;
		sscanf(s, "%lf", &weight);
		if(weight<0.0 || weight>100.0){
			QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - has to be >=0.0 and <=100.0"));
			return;
		}

		foreach(TimeConstraint* tc, gt.rules.timeConstraintsList)
			if(tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES)
				tc->weightPercentage=weight;

		gt.rules.internalStructureComputed=false;

		this->filterChanged();
	}
	else
		return;// user entered nothing or pressed Cancel
}*/

void ConstraintMinDaysBetweenActivitiesForm::changeSelectively()
{
	ChangeMinDaysSelectivelyForm dialog;
	
	//int w=dialog.sizeHint().width();
	//int h=dialog.sizeHint().height();
	//dialog.setGeometry(0,0,w,h);
	
	//centerWidgetOnScreen(&dialog);
	
	bool result=dialog.exec();

	if(result==QDialog::Accepted){
		double oldWeight=dialog.oldWeight;
		double newWeight=dialog.newWeight;
		int oldConsecutive=dialog.oldConsecutive;
		int newConsecutive=dialog.newConsecutive;
		int oldDays=dialog.oldDays;
		int newDays=dialog.newDays;
		int oldNActs=dialog.oldNActs;
		if(oldWeight==-1){
		}
		else if(oldWeight>=0 && oldWeight<=100.0){
		}
		else{
			QMessageBox::critical(this, tr("FET information"),
			tr("FET has meet a critical error - aborting current operation, please report bug (old weight is not -1 and not (>=0.0 and <=100.0)"));
			return;
		}

		if(newWeight==-1){
		}
		else if(newWeight>=0 && newWeight<=100.0){
		}
		else{
			QMessageBox::critical(this, tr("FET information"),
			tr("FET has met a critical error - aborting current operation, please report bug (new weight is not -1 and not (>=0.0 and <=100.0)"));
			return;
		}
		
		enum {ANY=0, YES=1, NO=2};
		enum {NOCHANGE=0};
		
		if(oldConsecutive<0 || oldConsecutive>2){
			QMessageBox::critical(this, tr("FET information"),
			tr("FET has met a critical error - aborting current operation, please report bug (old consecutive is not any, yes or no)"));
			return;
		}
		
		if(newConsecutive<0 || newConsecutive>2){
			QMessageBox::critical(this, tr("FET information"),
			tr("FET has met a critical error - aborting current operation, please report bug (new consecutive is not no_change, yes or no)"));
			return;
		}
		
		if(oldDays==-1){
		}
		else if(oldDays>=1 && oldDays<=gt.rules.nDaysPerWeek){
		}
		else{
			QMessageBox::critical(this, tr("FET information"),
			tr("FET has met a critical error - aborting current operation, please report bug (old min days is not -1 or 1..ndaysperweek)"));
			return;
		}
		
		if(newDays==-1){
		}
		else if(newDays>=1 && newDays<=gt.rules.nDaysPerWeek){
		}
		else{
			QMessageBox::critical(this, tr("FET information"),
			tr("FET has met a critical error - aborting current operation, please report bug (new min days is not -1 or 1..ndaysperweek)"));
			return;
		}
		
		if(oldNActs==-1){
		}
		else if(oldNActs>=1){
		}
		else{
			QMessageBox::critical(this, tr("FET information"),
			tr("FET has met a critical error - aborting current operation, please report bug (old nActivities is not -1 or >=1)"));
			return;
		}
		
		int count=0;

		foreach(TimeConstraint* tc, gt.rules.timeConstraintsList)
			if(tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
				ConstraintMinDaysBetweenActivities* mc=(ConstraintMinDaysBetweenActivities*)tc;
				bool okw, okd, okc, okn;
				if(oldWeight==-1)
					okw=true;
				else if(oldWeight==mc->weightPercentage)
					okw=true;
				else
					okw=false;
					
				if(oldConsecutive==ANY)
					okc=true;
				else if(oldConsecutive==YES && mc->consecutiveIfSameDay==true)
					okc=true;
				else if(oldConsecutive==NO && mc->consecutiveIfSameDay==false)
					okc=true;
				else
					okc=false;
					
				if(oldDays==-1)
					okd=true;
				else if(oldDays==mc->minDays)
					okd=true;
				else
					okd=false;
					
				if(oldNActs==-1)
					okn=true;
				else if(mc->n_activities==oldNActs)
					okn=true;
				else
					okn=false;
					
				if(okw && okc && okd && okn){
					if(newWeight>=0)
						mc->weightPercentage=newWeight;
						
					if(newConsecutive==YES)
						mc->consecutiveIfSameDay=true;
					else if(newConsecutive==NO)
						mc->consecutiveIfSameDay=false;
						
					if(newDays>=0)
						mc->minDays=newDays;
					
					count++;
				}
			}

		QMessageBox::information(this, tr("FET information"), tr("There were inspected (and possibly modified) %1 constraints min days between activities matching your criteria")
		 .arg(count)+"\n\n"+
		 	tr("NOTE: If you are using constraints of type activities same starting time or activities same starting day, it is important"
		 	 " (after current operation) to apply the operation of removing redundant constraints.")
		 	+" "+tr("Read Help/Important tips - tip 2) for details.")
		 /*
		 +
		 "\n\n"
		 +
		 tr("Please note that this is a new feature, not thoroughly tested, so it is a good practice to check the new constraints before saving your file, and maybe making some backups")
		 */
		 );

		gt.rules.internalStructureComputed=false;

		this->filterChanged();
	}
}
