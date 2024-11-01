/***************************************************************************
                          modifyconstraintactivitiesoccupymaxdifferentroomsform.cpp  -  description
                             -------------------
    begin                : Apr 29, 2012
    copyright            : (C) 2012 by Liviu Lalescu
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

#include "modifyconstraintactivitiesoccupymaxdifferentroomsform.h"

#include <QListWidget>
#include <QAbstractItemView>
#include <QScrollBar>

ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm(QWidget* parent, ConstraintActivitiesOccupyMaxDifferentRooms* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);
	
	allActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	
	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::cancel);
	connect(allActivitiesListWidget, &QListWidget::itemDoubleClicked, this, &ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::addActivity);
	connect(addAllActivitiesPushButton, &QPushButton::clicked, this, &ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::addAllActivities);
	connect(selectedActivitiesListWidget, &QListWidget::itemDoubleClicked, this, &ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::removeActivity);
	connect(clearPushButton, &QPushButton::clicked, this, &ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::clear);
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	maxDifferentRoomsSpinBox->setMinimum(1);
	maxDifferentRoomsSpinBox->setMaximum(MAX_ROOMS);
	maxDifferentRoomsSpinBox->setValue(ctr->maxDifferentRooms);

	//activities
	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	selectedActivitiesList.clear();
	selectedActivitiesListWidget->clear();
	for(int i=0; i<ctr->activitiesIds.count(); i++){
		int actId=ctr->activitiesIds.at(i);
		this->selectedActivitiesList.append(actId);
		Activity *act=gt.rules.activitiesPointerHash.value(actId, nullptr);
		assert(act!=nullptr);
		this->selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
	}

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

	connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::filterChanged);
	connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::filterChanged);
	connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::filterChanged);
	connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::filterChanged);
}

ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::~ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}
	
	if(this->selectedActivitiesList.count()==0){
		QMessageBox::warning(this, tr("FET information"),
		 tr("Empty list of activities"));
		return;
	}
	if(this->selectedActivitiesList.count()==1){
		QMessageBox::warning(this, tr("FET information"),
		 tr("Only one selected activity"));
		return;
	}

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;

	int maxDifferentRooms=maxDifferentRoomsSpinBox->value();
	
	this->_ctr->maxDifferentRooms=maxDifferentRooms;

	this->_ctr->activitiesIds=selectedActivitiesList;
	this->_ctr->recomputeActivitiesSet();

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::cancel()
{
	this->close();
}

//activities
bool ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::filterOk(Activity* act)
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

void ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::filterChanged()
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

void ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::addActivity()
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

void ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::addAllActivities()
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

void ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::removeActivity()
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

void ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm::clear()
{
	selectedActivitiesListWidget->clear();
	selectedActivitiesList.clear();
}
