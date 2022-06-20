/***************************************************************************
                          modifyconstraintminhalfdaysbetweenactivitiesform.cpp  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
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

#include "modifyconstraintminhalfdaysbetweenactivitiesform.h"
#include "spaceconstraint.h"

#include <QList>

#include <QListWidget>
#include <QAbstractItemView>
#include <QScrollBar>

ModifyConstraintMinHalfDaysBetweenActivitiesForm::ModifyConstraintMinHalfDaysBetweenActivitiesForm(QWidget* parent, ConstraintMinHalfDaysBetweenActivities* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);
	
	allActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(allActivitiesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addActivity()));
	connect(addAllActivitiesPushButton, SIGNAL(clicked()), this, SLOT(addAllActivities()));
	connect(selectedActivitiesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeActivity()));

	connect(clearPushButton, SIGNAL(clicked()), this, SLOT(clear()));

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
	
	this->_ctr=ctr;

	selectedActivitiesList.clear();
	selectedActivitiesListWidget->clear();
	for(int i=0; i<ctr->n_activities; i++){
		int actId=ctr->activitiesId[i];
		this->selectedActivitiesList.append(actId);
		Activity *act=gt.rules.activitiesPointerHash.value(actId, nullptr);
		assert(act!=nullptr);
		this->selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
	}
	
	minDaysSpinBox->setMinimum(1);
	minDaysSpinBox->setMaximum(gt.rules.nDaysPerWeek-1);
	minDaysSpinBox->setValue(ctr->minDays);

	consecutiveIfSameDayCheckBox->setChecked(ctr->consecutiveIfSameDay);
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));

	////////////////
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

	filterChanged();

	connect(teachersComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
	connect(studentsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
	connect(subjectsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
	connect(activityTagsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
}

ModifyConstraintMinHalfDaysBetweenActivitiesForm::~ModifyConstraintMinHalfDaysBetweenActivitiesForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintMinHalfDaysBetweenActivitiesForm::filterChanged()
{
	allActivitiesListWidget->clear();
	this->activitiesList.clear();

	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* ac=gt.rules.activitiesList[i];
		if(filterOk(ac)){
			allActivitiesListWidget->addItem(ac->getDescription(gt.rules));
			this->activitiesList.append(ac->id);
		}
	}
	
	int q=allActivitiesListWidget->verticalScrollBar()->minimum();
	allActivitiesListWidget->verticalScrollBar()->setValue(q);
}

bool ModifyConstraintMinHalfDaysBetweenActivitiesForm::filterOk(Activity* act)
{
	QString tn=teachersComboBox->currentText();
	QString stn=studentsComboBox->currentText();
	QString sbn=subjectsComboBox->currentText();
	QString atn=activityTagsComboBox->currentText();
	int ok=true;

	//teacher
	if(tn!=""){
		bool ok2=false;
		for(QStringList::const_iterator it=act->teachersNames.constBegin(); it!=act->teachersNames.constEnd(); it++)
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
	if(atn!="" && !act->activityTagsNames.contains(atn))
		ok=false;
		
	//students
	if(stn!=""){
		bool ok2=false;
		for(QStringList::const_iterator it=act->studentsNames.constBegin(); it!=act->studentsNames.constEnd(); it++)
			if(*it == stn){
				ok2=true;
				break;
			}
		if(!ok2)
			ok=false;
	}
	
	return ok;
}

void ModifyConstraintMinHalfDaysBetweenActivitiesForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	if(minDaysSpinBox->value()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid number of min half days between activities - it must be greater than 0."));
		return;
	}

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

#if 0
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

		 tr("2. If you don't add 'force consecutive if same day', then add a larger activity split into a number of"
		  " activities equal with the number of days per week and the remaining components into other larger split activity."
		  " For example, suppose you need to add 7 activities with duration 1 in a 5 days week. Add 2 larger container activities,"
		  " first one split into 5 activities with duration 1 and second one split into 2 activities with duration 1"
		  " (possibly raising the weight of added constraints min days between activities for each of the 2 containers up to 100%)")+

	  	 "\n\n"+
		 tr("Do you want to add current constraint as it is now (not recommended) or cancel and edit as instructed?");
	
		int t=QMessageBox::warning(this, tr("FET warning"), s,
		 QMessageBox::Yes, QMessageBox::Cancel);
		if(t==QMessageBox::Cancel)
			return;
	}
#endif

	if(1){
		ConstraintMinHalfDaysBetweenActivities adc;

		int i;
		QList<int>::const_iterator it;
		adc.activitiesId.clear();
		for(i=0, it=this->selectedActivitiesList.constBegin(); it!=this->selectedActivitiesList.constEnd(); it++, i++){
			adc.activitiesId.append(*it);
		}
		adc.n_activities=i;
		
		adc.weightPercentage=weight;
		adc.consecutiveIfSameDay=consecutiveIfSameDayCheckBox->isChecked();
		adc.minDays=minDaysSpinBox->value();
		
		bool duplicate=false;
		
		for(TimeConstraint* tc : qAsConst(gt.rules.timeConstraintsList))
			if(tc!=this->_ctr && tc->type==CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES)
				if( ( *((ConstraintMinHalfDaysBetweenActivities*)tc) ) == adc){
					duplicate=true;
					break;
				}
				
		if(duplicate){
			QMessageBox::warning(this, tr("FET information"), tr("Cannot proceed, current constraint is equal to another one (it is duplicated)"));
			return;
		}
	}
	
	if(_ctr->activitiesId!=selectedActivitiesList){
		for(int oldId : qAsConst(_ctr->activitiesId)){
			QSet<ConstraintMinHalfDaysBetweenActivities*> cs=gt.rules.mhdbaHash.value(oldId, QSet<ConstraintMinHalfDaysBetweenActivities*>());
			assert(cs.contains(_ctr));
			cs.remove(_ctr);
			gt.rules.mhdbaHash.insert(oldId, cs);
		}
		
		for(int newId : qAsConst(selectedActivitiesList)){
			QSet<ConstraintMinHalfDaysBetweenActivities*> cs=gt.rules.mhdbaHash.value(newId, QSet<ConstraintMinHalfDaysBetweenActivities*>());
			assert(!cs.contains(_ctr));
			cs.insert(_ctr);
			gt.rules.mhdbaHash.insert(newId, cs);
		}
		
		_ctr->activitiesId=selectedActivitiesList;
		_ctr->n_activities=_ctr->activitiesId.count();
	}

	/*int i;
	QList<int>::iterator it;
	this->_ctr->activitiesId.clear();
	for(i=0, it=this->selectedActivitiesList.begin(); it!=this->selectedActivitiesList.end(); it++, i++){
		this->_ctr->activitiesId.append(*it);
	}
	this->_ctr->n_activities=i;*/
	
	this->_ctr->weightPercentage=weight;
	this->_ctr->consecutiveIfSameDay=consecutiveIfSameDayCheckBox->isChecked();
	this->_ctr->minDays=minDaysSpinBox->value();
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintMinHalfDaysBetweenActivitiesForm::cancel()
{
	this->close();
}

void ModifyConstraintMinHalfDaysBetweenActivitiesForm::addActivity()
{
	if(allActivitiesListWidget->currentRow()<0)
		return;
	int tmp=allActivitiesListWidget->currentRow();
	int _id=this->activitiesList.at(tmp);
	
	QString actName=allActivitiesListWidget->currentItem()->text();
	assert(actName!="");
	
	//duplicate?
	if(this->selectedActivitiesList.contains(_id))
		return;
	
	selectedActivitiesListWidget->addItem(actName);
	selectedActivitiesListWidget->setCurrentRow(selectedActivitiesListWidget->count()-1);

	this->selectedActivitiesList.append(_id);
}

void ModifyConstraintMinHalfDaysBetweenActivitiesForm::addAllActivities()
{
	for(int tmp=0; tmp<allActivitiesListWidget->count(); tmp++){
		int _id=this->activitiesList.at(tmp);
	
		QString actName=allActivitiesListWidget->item(tmp)->text();
		assert(actName!="");
		
		if(this->selectedActivitiesList.contains(_id))
			continue;
		
		selectedActivitiesListWidget->addItem(actName);
		this->selectedActivitiesList.append(_id);
	}
	
	selectedActivitiesListWidget->setCurrentRow(selectedActivitiesListWidget->count()-1);
}

void ModifyConstraintMinHalfDaysBetweenActivitiesForm::removeActivity()
{
	if(selectedActivitiesListWidget->currentRow()<0 || selectedActivitiesListWidget->count()<=0)
		return;
	int tmp=selectedActivitiesListWidget->currentRow();
	
	this->selectedActivitiesList.removeAt(tmp);

	selectedActivitiesListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedActivitiesListWidget->takeItem(tmp);
	delete item;
	if(tmp<selectedActivitiesListWidget->count())
		selectedActivitiesListWidget->setCurrentRow(tmp);
	else
		selectedActivitiesListWidget->setCurrentRow(selectedActivitiesListWidget->count()-1);
}

void ModifyConstraintMinHalfDaysBetweenActivitiesForm::clear()
{
	selectedActivitiesListWidget->clear();
	selectedActivitiesList.clear();
}
