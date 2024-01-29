/***************************************************************************
                          addconstraintactivitiessamestartingtimeform.cpp  -  description
                             -------------------
    begin                : Wed June 23 2004
    copyright            : (C) 2004 by Liviu Lalescu
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

#include "addconstraintactivitiessamestartingtimeform.h"

#include "matrix.h"

#include <QListWidget>
#include <QAbstractItemView>
#include <QScrollBar>

AddConstraintActivitiesSameStartingTimeForm::AddConstraintActivitiesSameStartingTimeForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);
	
	allActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	
	blockCheckBox->setChecked(false);

	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintActivitiesSameStartingTimeForm::close);
	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintActivitiesSameStartingTimeForm::addConstraint);
	connect(helpPushButton, &QPushButton::clicked, this, &AddConstraintActivitiesSameStartingTimeForm::help);
	connect(blockCheckBox, &QCheckBox::toggled, this, &AddConstraintActivitiesSameStartingTimeForm::blockChanged);
	connect(clearPushButton, &QPushButton::clicked, this, &AddConstraintActivitiesSameStartingTimeForm::clear);
	connect(allActivitiesListWidget, &QListWidget::itemDoubleClicked, this, &AddConstraintActivitiesSameStartingTimeForm::addActivity);
	connect(addAllActivitiesPushButton, &QPushButton::clicked, this, &AddConstraintActivitiesSameStartingTimeForm::addAllActivities);
	connect(selectedActivitiesListWidget, &QListWidget::itemDoubleClicked, this, &AddConstraintActivitiesSameStartingTimeForm::removeActivity);

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

	this->filterChanged();

	connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddConstraintActivitiesSameStartingTimeForm::filterChanged);
	connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddConstraintActivitiesSameStartingTimeForm::filterChanged);
	connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddConstraintActivitiesSameStartingTimeForm::filterChanged);
	connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddConstraintActivitiesSameStartingTimeForm::filterChanged);
}

AddConstraintActivitiesSameStartingTimeForm::~AddConstraintActivitiesSameStartingTimeForm()
{
	saveFETDialogGeometry(this);
}

bool AddConstraintActivitiesSameStartingTimeForm::filterOk(Activity* act)
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

void AddConstraintActivitiesSameStartingTimeForm::filterChanged()
{
	allActivitiesListWidget->clear();
	this->activitiesList.clear();

	if(blockCheckBox->isChecked())
		//show only non-split activities and split activities which are the representatives
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* ac=gt.rules.activitiesList[i];
			if(filterOk(ac)){
				if(ac->activityGroupId==0){
					allActivitiesListWidget->addItem(ac->getDescription(gt.rules));
					this->activitiesList.append(ac->id);
				}
				else if(ac->id==ac->activityGroupId){
					allActivitiesListWidget->addItem(ac->getDescription(gt.rules));
					this->activitiesList.append(ac->id);
				}
			}
		}
	else
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

void AddConstraintActivitiesSameStartingTimeForm::blockChanged()
{
	selectedActivitiesListWidget->clear();
	this->selectedActivitiesList.clear();

	filterChanged();
}

void AddConstraintActivitiesSameStartingTimeForm::addConstraint()
{
	TimeConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	if(this->selectedActivitiesList.count()==0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Empty list of selected activities"));
		return;
	}
	if(this->selectedActivitiesList.count()==1){
		QMessageBox::warning(this, tr("FET information"),
			tr("Only one selected activity - impossible"));
		return;
	}
	
	if(blockCheckBox->isChecked()){ //block constraints
		///////////phase 1 - how many constraints will be added?
		int nConstraints=0;
		QList<int>::const_iterator it;
		for(it=this->selectedActivitiesList.constBegin(); it!=this->selectedActivitiesList.constEnd(); it++){
			int _id=(*it);
			int tmp=0; //tmp represents the number of subactivities represented by the current (sub)activity

			for(int i=0; i<gt.rules.activitiesList.size(); i++){
				Activity* act=gt.rules.activitiesList[i];
				if(act->activityGroupId==0){
					if(act->id==_id){
						assert(tmp==0);
						tmp=1;
					}
				}
				else{
					if(act->id==_id){
						assert(act->activityGroupId==act->id);
						assert(tmp==0);
						tmp=1;
					}
					else if(act->activityGroupId==_id)
						tmp++;
				}
			}

			if(nConstraints==0){
				nConstraints=tmp;
			}
			else{
				if(tmp!=nConstraints){
					QString s=tr("Subactivities do not correspond. Mistake:");
					s+="\n";
					s+=tr("1. First (sub)activity has id=%1 and represents %2 subactivities")
						.arg(this->selectedActivitiesList.at(0))
						.arg(nConstraints);
					s+="\n";
					s+=tr("2. Current (sub)activity has id=%1 and represents %2 subactivities")
						.arg(_id)
						.arg(tmp);
					QMessageBox::warning(this, tr("FET information"), s);
					return;
				}
			}
		}
	
		/////////////phase 2 - compute the indices of all the (sub)activities
		Matrix1D<QList<int>> ids;
		ids.resize(nConstraints);

		for(int i=0; i<nConstraints; i++)
			ids[i].clear();
		int k;
		for(k=0, it=this->selectedActivitiesList.constBegin(); it!=this->selectedActivitiesList.constEnd(); k++, it++){
			int _id=(*it);
			int tmp=0; //tmp represents the number of subactivities represented by the current (sub)activity

			for(int i=0; i<gt.rules.activitiesList.size(); i++){
				Activity* act=gt.rules.activitiesList[i];
				if(act->activityGroupId==0){
					if(act->id==_id){
						assert(tmp==0);
						assert(ids[tmp].count()==k);
						ids[tmp].append(_id);
						tmp=1;
					}
				}
				else{
					if(act->id==_id){
						assert(act->activityGroupId==act->id);
						assert(tmp==0);
						assert(ids[tmp].count()==k);
						ids[tmp].append(_id);
						tmp=1;
					}
					else if(act->activityGroupId==_id){
						assert(ids[tmp].count()==k);
						ids[tmp].append(act->id);
						tmp++;
					}
				}
			}
		}
	
		////////////////phase 3 - add the constraints
		QString ctrs;
		for(k=0; k<nConstraints; k++){
			ctr=new ConstraintActivitiesSameStartingTime(weight, this->selectedActivitiesList.count(), ids[k]);
			bool tmp2=gt.rules.addTimeConstraint(ctr);
			
			if(tmp2){
				QString s;

				s+=tr("Constraint added:");
				s+="\n\n";
				s+=ctr->getDetailedDescription(gt.rules);
				LongTextMessageBox::information(this, tr("FET information"), s);

				ctrs+=ctr->getDetailedDescription(gt.rules);
				ctrs+="\n";
			}
			else{
				QMessageBox::warning(this, tr("FET information"),
					tr("Constraint NOT added - please report error"));
				delete ctr;
			}
		}

		gt.rules.addUndoPoint(tr("Added %1 constraints (using the option to add multiple constraints):\n\n%2",
								 "%1 is the number of constraints, %2 is their detailed description")
								 .arg(nConstraints).arg(ctrs));
	}
	else{
		QList<int> ids;
		QList<int>::const_iterator it;
		ids.clear();
		for(it=this->selectedActivitiesList.constBegin(); it!=this->selectedActivitiesList.constEnd(); it++){
			ids.append(*it);
		}
		ctr=new ConstraintActivitiesSameStartingTime(weight, this->selectedActivitiesList.count(), ids);

		bool tmp2=gt.rules.addTimeConstraint(ctr);
		
		if(tmp2){
			QString s;

			s+=tr("Constraint added:");
			s+="\n\n";
			s+=ctr->getDetailedDescription(gt.rules);
			LongTextMessageBox::information(this, tr("FET information"), s);

			gt.rules.addUndoPoint(tr("Added the constraint:\n\n%1").arg(ctr->getDetailedDescription(gt.rules)));
		}
		else{
			QMessageBox::warning(this, tr("FET information"),
				tr("Constraint NOT added - please report error"));
			delete ctr;
		}
	}
}

void AddConstraintActivitiesSameStartingTimeForm::addActivity()
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

void AddConstraintActivitiesSameStartingTimeForm::addAllActivities()
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

void AddConstraintActivitiesSameStartingTimeForm::removeActivity()
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

void AddConstraintActivitiesSameStartingTimeForm::clear()
{
	selectedActivitiesListWidget->clear();
	selectedActivitiesList.clear();
}

void AddConstraintActivitiesSameStartingTimeForm::help()
{
	QString s;
	
	s=tr("Add multiple constraints: this is a check box. Select this if you want to input only the representatives of subactivities and FET to add multiple constraints,"
	" for all subactivities from the same larger split activity, in turn, respectively."
	" There will be added more constraints activities same starting time, one for each corresponding tuple. The number of"
	" subactivities must match for the representatives and be careful to the order, to be what you need");

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
