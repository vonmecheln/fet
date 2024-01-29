/***************************************************************************
                          addconstraintminhalfdaysbetweenactivitiesform.cpp  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Liviu Lalescu
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

#include "addconstraintminhalfdaysbetweenactivitiesform.h"

#include <QListWidget>
#include <QAbstractItemView>
#include <QScrollBar>

AddConstraintMinHalfDaysBetweenActivitiesForm::AddConstraintMinHalfDaysBetweenActivitiesForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	consecutiveIfSameDayCheckBox->setChecked(true);

	addConstraintPushButton->setDefault(true);
	
	allActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintMinHalfDaysBetweenActivitiesForm::close);
	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintMinHalfDaysBetweenActivitiesForm::addConstraint);
	connect(allActivitiesListWidget, &QListWidget::itemDoubleClicked, this, &AddConstraintMinHalfDaysBetweenActivitiesForm::addActivity);
	connect(addAllActivitiesPushButton, &QPushButton::clicked, this, &AddConstraintMinHalfDaysBetweenActivitiesForm::addAllActivities);
	connect(selectedActivitiesListWidget, &QListWidget::itemDoubleClicked, this, &AddConstraintMinHalfDaysBetweenActivitiesForm::removeActivity);
	connect(clearPushButton, &QPushButton::clicked, this, &AddConstraintMinHalfDaysBetweenActivitiesForm::clear);

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

	minDaysSpinBox->setMinimum(1);
	minDaysSpinBox->setMaximum(gt.rules.nDaysPerWeek-1);
	minDaysSpinBox->setValue(1);

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

	selectedActivitiesListWidget->clear();
	this->selectedActivitiesList.clear();

	filterChanged();

	connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddConstraintMinHalfDaysBetweenActivitiesForm::filterChanged);
	connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddConstraintMinHalfDaysBetweenActivitiesForm::filterChanged);
	connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddConstraintMinHalfDaysBetweenActivitiesForm::filterChanged);
	connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddConstraintMinHalfDaysBetweenActivitiesForm::filterChanged);
}

AddConstraintMinHalfDaysBetweenActivitiesForm::~AddConstraintMinHalfDaysBetweenActivitiesForm()
{
	saveFETDialogGeometry(this);
}

bool AddConstraintMinHalfDaysBetweenActivitiesForm::filterOk(Activity* act)
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

void AddConstraintMinHalfDaysBetweenActivitiesForm::filterChanged()
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

void AddConstraintMinHalfDaysBetweenActivitiesForm::addConstraint()
{
	TimeConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}
	
	if(minDaysSpinBox->value()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid number of min half days between activities - it must be greater than 0."));
		return;
	}

	if(this->selectedActivitiesList.count()==0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Empty list of selected activities"));
		return;
	}
	if(this->selectedActivitiesList.count()==1){
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

		 tr("1. If you selected 'consecutive if on the same day', then couple extra activities in pairs to obtain a number of activities equal to the number of days per week"
		  ". Example: 7 activities with duration 1 in a 5 days week, then transform into 5 activities with durations: 2,2,1,1,1 and add a single container activity with these 5 components"
		  " (possibly raising the weight of added constraint min days between activities up to 100%)")+

		  "\n\n"+

		 tr("2. If you didn't select 'consecutive if on the same day', then add a larger activity split into a number of"
		  " activities equal with the number of days per week and the remaining components into other larger split activity."
		  " For example, suppose you need to add 7 activities with duration 1 in a 5 days week. Add 2 larger container activities,"
		  " first one split into 5 activities with duration 1 and second one split into 2 activities with duration 1"
		  " (possibly raising the weight of added constraints min days between activities for each of the 2 containers up to 100%)")+

		  "\n\n"+

		 tr("Note: If the weight of the added constraint min days between activities is 0% or a low value, you can safely ignore this warning.")+
		  "\n\n"+

		 tr("Do you want to add current constraint as it is now (not recommended) or cancel and edit as instructed?");
	
		int t=QMessageBox::warning(this, tr("FET warning"), s,
		 QMessageBox::Yes, QMessageBox::Cancel);
		if(t==QMessageBox::Cancel)
			return;
	}
#endif
	
	QList<int> ids;
	QList<int>::const_iterator it;
	ids.clear();
	for(it=this->selectedActivitiesList.constBegin(); it!=this->selectedActivitiesList.constEnd(); it++){
		ids.append(*it);
	}
	
	ctr=new ConstraintMinHalfDaysBetweenActivities(weight, consecutiveIfSameDayCheckBox->isChecked(), this->selectedActivitiesList.count(), ids, minDaysSpinBox->value());

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	
	if(tmp2){
		QString s=tr("Constraint added:");
		s+="\n\n";
		s+=ctr->getDetailedDescription(gt.rules);
		LongTextMessageBox::information(this, tr("FET information"), s);

		gt.rules.addUndoPoint(tr("Added the constraint:\n\n%1").arg(ctr->getDetailedDescription(gt.rules)));
	}
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - it must be a duplicate"));
		delete ctr;
	}
}

void AddConstraintMinHalfDaysBetweenActivitiesForm::addActivity()
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

void AddConstraintMinHalfDaysBetweenActivitiesForm::addAllActivities()
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

void AddConstraintMinHalfDaysBetweenActivitiesForm::removeActivity()
{
	if(selectedActivitiesListWidget->currentRow()<0 || selectedActivitiesListWidget->count()<=0)
		return;
	int tmp=selectedActivitiesListWidget->currentRow();
	
	selectedActivitiesList.removeAt(tmp);
	
	selectedActivitiesListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedActivitiesListWidget->takeItem(tmp);
	delete item;
	if(tmp<selectedActivitiesListWidget->count())
		selectedActivitiesListWidget->setCurrentRow(tmp);
	else
		selectedActivitiesListWidget->setCurrentRow(selectedActivitiesListWidget->count()-1);
}

void AddConstraintMinHalfDaysBetweenActivitiesForm::clear()
{
	selectedActivitiesListWidget->clear();
	selectedActivitiesList.clear();
}
