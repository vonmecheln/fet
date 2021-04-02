/***************************************************************************
                          modifyconstraintmindaysbetweenactivitiesform.cpp  -  description
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

#include "modifyconstraintmindaysbetweenactivitiesform.h"
#include "spaceconstraint.h"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3table.h>

#include <QList>

#include <QDesktopWidget>

ModifyConstraintMinDaysBetweenActivitiesForm::ModifyConstraintMinDaysBetweenActivitiesForm(ConstraintMinDaysBetweenActivities* ctr)
{
    setupUi(this);

    connect(cancelPushButton, SIGNAL(clicked()), this /*ModifyConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(cancel()));
    connect(okPushButton, SIGNAL(clicked()), this /*ModifyConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(ok()));
    connect(activitiesListBox, SIGNAL(selected(QString)), this /*ModifyConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(addActivity()));
    connect(selectedActivitiesListBox, SIGNAL(selected(QString)), this /*ModifyConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(removeActivity()));
    connect(teachersComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(filterChanged()));
    connect(studentsComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(filterChanged()));
    connect(subjectsComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(filterChanged()));
    connect(activityTagsComboBox, SIGNAL(activated(QString)), this /*ModifyConstraintMinDaysBetweenActivitiesForm_template*/, SLOT(filterChanged()));

    connect(clearPushButton, SIGNAL(clicked()), this, SLOT(clear()));

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
	
	this->_ctr=ctr;
	//updateActivitiesListBox();

	selectedActivitiesList.clear();
	selectedActivitiesListBox->clear();	
	for(int i=0; i<ctr->n_activities; i++){
		int actId=ctr->activitiesId[i];
		this->selectedActivitiesList.append(actId);
		Activity* act=NULL;
		for(int k=0; k<gt.rules.activitiesList.size(); k++){
			act=gt.rules.activitiesList[k];
			if(act->id==actId)
				break;
		}
		assert(act);
		this->selectedActivitiesListBox->insertItem(act->getDescription(gt.rules));
	}
	
	minDaysSpinBox->setMinValue(1);
	minDaysSpinBox->setMaxValue(gt.rules.nDaysPerWeek-1);
	minDaysSpinBox->setValue(ctr->minDays);

	//compulsoryCheckBox->setChecked(ctr->compulsory);
	consecutiveIfSameDayCheckBox->setChecked(ctr->consecutiveIfSameDay);
	weightLineEdit->setText(QString::number(ctr->weightPercentage));

	////////////////
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

	filterChanged();
}

ModifyConstraintMinDaysBetweenActivitiesForm::~ModifyConstraintMinDaysBetweenActivitiesForm()
{
}

void ModifyConstraintMinDaysBetweenActivitiesForm::filterChanged()
{
	activitiesListBox->clear();
//	selectedActivitiesListBox->clear();

	this->activitiesList.clear();
//	this->selectedActivitiesList.clear();

	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* ac=gt.rules.activitiesList[i];
		if(filterOk(ac)){
			activitiesListBox->insertItem(ac->getDescription(gt.rules));
			this->activitiesList.append(ac->id);
		}
	}
}

bool ModifyConstraintMinDaysBetweenActivitiesForm::filterOk(Activity* act)
{
	QString tn=teachersComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString sbtn=activityTagsComboBox->currentText();
	int ok=true;

	//teacher
	if(tn!=""){
		bool ok2=false;
		for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++)
			if(*it == tn){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}

	//subject
	if(sbn!="" && sbn!=act->subjectName)
		ok=false;
		
	//activity tag
//	if(sbtn!="" && sbtn!=act->activityTagName)
	if(sbtn!="" && !act->activityTagsNames.contains(sbtn))
		ok=false;
		
	//students
	if(stn!=""){
		bool ok2=false;
		for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
			if(*it == stn){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}
	
	return ok;
}

void ModifyConstraintMinDaysBetweenActivitiesForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	/*bool compulsory=false;
	if(compulsoryCheckBox->isChecked())
		compulsory=true;*/

	if(this->selectedActivitiesList.size()==0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Empty list of selected activities"));
		return;
	}
	if(this->selectedActivitiesList.size()==1){
		QMessageBox::warning(this, tr("FET information"),
			tr("Only one selected activity"));
		return;
	}
	if(this->selectedActivitiesList.size()>MAX_CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES){
		QMessageBox::warning(this, tr("FET information"),
			tr("Please report error to the author\nMAX_CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES must be increased (you have too many activities)"));
		return;
	}

#if 0&0&0
	if(0 && this->selectedActivitiesList.size()>gt.rules.nDaysPerWeek){
		QString s=tr("You want to add a constraint min days between activities for more activities than the number of days per week."
		  " This is a very bad practice from the way the algorithm of generation works (it slows down the generation and makes it harder to find a solution).")+
		 "\n\n"+
		 tr("The best way to add the activities would be:")+
		 "\n\n"+

		 tr("1. If you add 'force consecutive if same day', then couple extra activities in pairs to obtain a number of activities equal to the number of days per week"
		  ". Example: 7 activities with duration 1 in a 5 days week, then transform into 5 activities with durations: 2,2,1,1,1 and add a single container activity with these 5 components"
		  " (possibly raising the weight of added constraint min days between activities up to 100%)")+

		  "\n\n"+

		 tr("2. If you don't add 'force consecutive if same day', then add a larger activity splitted into a number of"
		  " activities equal with the number of days per week and the remaining components into other larger splitted activity."
		  " For example, suppose you need to add 7 activities with duration 1 in a 5 days week. Add 2 larger container activities,"
		  " first one splitted into 5 activities with duration 1 and second one splitted into 2 activities with duration 1"
		  " (possibly raising the weight of added constraints min days between activities for each of the 2 containers up to 100%)")+

	  	 "\n\n"+
		 tr("Do you want to add current constraint as it is now (not recommended) or cancel and edit as instructed?");
	
		int t=QMessageBox::warning(this, tr("FET warning"),	s,
		 QMessageBox::Yes, QMessageBox::Cancel);
		if(t==QMessageBox::Cancel)
			return;
	}
#endif

	if(1){
		ConstraintMinDaysBetweenActivities adc;

		int i;
		QList<int>::iterator it;
		for(i=0, it=this->selectedActivitiesList.begin(); it!=this->selectedActivitiesList.end(); it++, i++)
			adc.activitiesId[i]=*it;
		adc.n_activities=i;
		
		adc.weightPercentage=weight;
		adc.consecutiveIfSameDay=consecutiveIfSameDayCheckBox->isChecked();
		adc.minDays=minDaysSpinBox->value();
		
		bool duplicate=false;
		
		foreach(TimeConstraint* tc, gt.rules.timeConstraintsList)
			if(tc!=this->_ctr && tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES)
				if( ( *((ConstraintMinDaysBetweenActivities*)tc) ) == adc){
					duplicate=true;
					break;
				}
				
		if(duplicate){
			QMessageBox::warning(this, tr("FET information"), tr("Cannot proceed, current constraint is equal to another one (it is duplicated)"));
			return;
		}
	}
	
	int i;
	QList<int>::iterator it;
	for(i=0, it=this->selectedActivitiesList.begin(); it!=this->selectedActivitiesList.end(); it++, i++)
		this->_ctr->activitiesId[i]=*it;
	this->_ctr->n_activities=i;
		
	this->_ctr->weightPercentage=weight;
	//this->_ctr->compulsory=compulsory;
	this->_ctr->consecutiveIfSameDay=consecutiveIfSameDayCheckBox->isChecked();
	this->_ctr->minDays=minDaysSpinBox->value();
	
	gt.rules.internalStructureComputed=false;
	
	this->close();
}

void ModifyConstraintMinDaysBetweenActivitiesForm::cancel()
{
	this->close();
}

void ModifyConstraintMinDaysBetweenActivitiesForm::addActivity()
{
	if(activitiesListBox->currentItem()<0)
		return;
	int tmp=activitiesListBox->currentItem();
	int _id=this->activitiesList.at(tmp);
	
	QString actName=activitiesListBox->currentText();
	assert(actName!="");
	uint i;
	//duplicate?
	for(i=0; i<selectedActivitiesListBox->count(); i++)
		if(actName==selectedActivitiesListBox->text(i))
			break;
	if(i<selectedActivitiesListBox->count())
		return;
	selectedActivitiesListBox->insertItem(actName);
	
	this->selectedActivitiesList.append(_id);
}

void ModifyConstraintMinDaysBetweenActivitiesForm::removeActivity()
{
	if(selectedActivitiesListBox->currentItem()<0 || selectedActivitiesListBox->count()<=0)
		return;		
	int tmp=selectedActivitiesListBox->currentItem();
	
	selectedActivitiesListBox->removeItem(selectedActivitiesListBox->currentItem());
	this->selectedActivitiesList.removeAt(tmp);
}

void ModifyConstraintMinDaysBetweenActivitiesForm::clear()
{
	selectedActivitiesListBox->clear();
	selectedActivitiesList.clear();
}
