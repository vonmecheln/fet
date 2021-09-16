/***************************************************************************
                          modifyconstrainttwosetsofactivitiesorderedform.cpp  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Lalescu Liviu
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

#include "modifyconstrainttwosetsofactivitiesorderedform.h"

#include <QList>

#include <QListWidget>
#include <QAbstractItemView>
#include <QScrollBar>

ModifyConstraintTwoSetsOfActivitiesOrderedForm::ModifyConstraintTwoSetsOfActivitiesOrderedForm(QWidget* parent, ConstraintTwoSetsOfActivitiesOrdered* ctr): QDialog(parent)
{
	setupUi(this);
	
	tabWidget->setCurrentIndex(0);

	okPushButton->setDefault(true);

	firstAllActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	firstSelectedActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	secondAllActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	secondSelectedActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));

	connect(firstAllActivitiesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(firstAddActivity()));
	connect(firstAddAllActivitiesPushButton, SIGNAL(clicked()), this, SLOT(firstAddAllActivities()));
	connect(firstSelectedActivitiesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(firstRemoveActivity()));

	connect(firstClearPushButton, SIGNAL(clicked()), this, SLOT(firstClear()));

	connect(secondAllActivitiesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(secondAddActivity()));
	connect(secondAddAllActivitiesPushButton, SIGNAL(clicked()), this, SLOT(secondAddAllActivities()));
	connect(secondSelectedActivitiesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(secondRemoveActivity()));

	connect(secondClearPushButton, SIGNAL(clicked()), this, SLOT(secondClear()));
	
	connect(swapPushButton, SIGNAL(clicked()), this, SLOT(swap()));

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
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	firstSelectedActivitiesListWidget->clear();
	this->firstSelectedActivitiesList.clear();

	for(int actId : qAsConst(ctr->firstActivitiesIdsList)){
		this->firstSelectedActivitiesList.append(actId);
		Activity *act=gt.rules.activitiesPointerHash.value(actId, nullptr);
		assert(act!=nullptr);
		firstSelectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
	}
	
	secondSelectedActivitiesListWidget->clear();
	this->secondSelectedActivitiesList.clear();

	for(int actId : qAsConst(ctr->secondActivitiesIdsList)){
		this->secondSelectedActivitiesList.append(actId);
		Activity* act=nullptr;
		for(int k=0; k<gt.rules.activitiesList.size(); k++){
			act=gt.rules.activitiesList[k];
			if(act->id==actId)
				break;
		}
		assert(act!=nullptr);
		secondSelectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
	}

	connect(teachersComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
	connect(studentsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
	connect(subjectsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
	connect(activityTagsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(filterChanged()));
}

ModifyConstraintTwoSetsOfActivitiesOrderedForm::~ModifyConstraintTwoSetsOfActivitiesOrderedForm()
{
	saveFETDialogGeometry(this);
}

bool ModifyConstraintTwoSetsOfActivitiesOrderedForm::filterOk(Activity* act)
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

void ModifyConstraintTwoSetsOfActivitiesOrderedForm::filterChanged()
{
	firstAllActivitiesListWidget->clear();
	this->firstActivitiesList.clear();

	secondAllActivitiesListWidget->clear();
	this->secondActivitiesList.clear();

	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* ac=gt.rules.activitiesList[i];
		if(filterOk(ac)){
			firstAllActivitiesListWidget->addItem(ac->getDescription(gt.rules));
			this->firstActivitiesList.append(ac->id);

			secondAllActivitiesListWidget->addItem(ac->getDescription(gt.rules));
			this->secondActivitiesList.append(ac->id);
		}
	}
	
	int q=firstAllActivitiesListWidget->verticalScrollBar()->minimum();
	firstAllActivitiesListWidget->verticalScrollBar()->setValue(q);

	q=secondAllActivitiesListWidget->verticalScrollBar()->minimum();
	secondAllActivitiesListWidget->verticalScrollBar()->setValue(q);
}

void ModifyConstraintTwoSetsOfActivitiesOrderedForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	if(this->firstSelectedActivitiesList.size()==0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Empty list of selected activities in the first set"));
		return;
	}

	if(this->secondSelectedActivitiesList.size()==0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Empty list of selected activities in the second set"));
		return;
	}
	
	this->_ctr->weightPercentage=weight;
	this->_ctr->firstActivitiesIdsList=this->firstSelectedActivitiesList;
	this->_ctr->secondActivitiesIdsList=this->secondSelectedActivitiesList;
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTwoSetsOfActivitiesOrderedForm::cancel()
{
	this->close();
}

void ModifyConstraintTwoSetsOfActivitiesOrderedForm::firstAddActivity()
{
	if(firstAllActivitiesListWidget->currentRow()<0)
		return;
	int tmp=firstAllActivitiesListWidget->currentRow();
	int _id=this->firstActivitiesList.at(tmp);
	
	QString actName=firstAllActivitiesListWidget->currentItem()->text();
	assert(actName!="");
	
	//duplicate?
	if(this->firstSelectedActivitiesList.contains(_id))
		return;
	
	firstSelectedActivitiesListWidget->addItem(actName);
	firstSelectedActivitiesListWidget->setCurrentRow(firstSelectedActivitiesListWidget->count()-1);

	this->firstSelectedActivitiesList.append(_id);
}

void ModifyConstraintTwoSetsOfActivitiesOrderedForm::secondAddActivity()
{
	if(secondAllActivitiesListWidget->currentRow()<0)
		return;
	int tmp=secondAllActivitiesListWidget->currentRow();
	int _id=this->secondActivitiesList.at(tmp);
	
	QString actName=secondAllActivitiesListWidget->currentItem()->text();
	assert(actName!="");
	
	//duplicate?
	if(this->secondSelectedActivitiesList.contains(_id))
		return;
	
	secondSelectedActivitiesListWidget->addItem(actName);
	secondSelectedActivitiesListWidget->setCurrentRow(secondSelectedActivitiesListWidget->count()-1);

	this->secondSelectedActivitiesList.append(_id);
}

void ModifyConstraintTwoSetsOfActivitiesOrderedForm::firstAddAllActivities()
{
	for(int tmp=0; tmp<firstAllActivitiesListWidget->count(); tmp++){
		int _id=this->firstActivitiesList.at(tmp);
	
		QString actName=firstAllActivitiesListWidget->item(tmp)->text();
		assert(actName!="");
		
		if(this->firstSelectedActivitiesList.contains(_id))
			continue;
		
		firstSelectedActivitiesListWidget->addItem(actName);
		this->firstSelectedActivitiesList.append(_id);
	}
	
	firstSelectedActivitiesListWidget->setCurrentRow(firstSelectedActivitiesListWidget->count()-1);
}

void ModifyConstraintTwoSetsOfActivitiesOrderedForm::secondAddAllActivities()
{
	for(int tmp=0; tmp<secondAllActivitiesListWidget->count(); tmp++){
		int _id=this->secondActivitiesList.at(tmp);
	
		QString actName=secondAllActivitiesListWidget->item(tmp)->text();
		assert(actName!="");
		
		if(this->secondSelectedActivitiesList.contains(_id))
			continue;
		
		secondSelectedActivitiesListWidget->addItem(actName);
		this->secondSelectedActivitiesList.append(_id);
	}
	
	secondSelectedActivitiesListWidget->setCurrentRow(secondSelectedActivitiesListWidget->count()-1);
}

void ModifyConstraintTwoSetsOfActivitiesOrderedForm::firstRemoveActivity()
{
	if(firstSelectedActivitiesListWidget->currentRow()<0 || firstSelectedActivitiesListWidget->count()<=0)
		return;
	int tmp=firstSelectedActivitiesListWidget->currentRow();
	
	firstSelectedActivitiesList.removeAt(tmp);

	firstSelectedActivitiesListWidget->setCurrentRow(-1);
	QListWidgetItem* item=firstSelectedActivitiesListWidget->takeItem(tmp);
	delete item;
	if(tmp<firstSelectedActivitiesListWidget->count())
		firstSelectedActivitiesListWidget->setCurrentRow(tmp);
	else
		firstSelectedActivitiesListWidget->setCurrentRow(firstSelectedActivitiesListWidget->count()-1);
}

void ModifyConstraintTwoSetsOfActivitiesOrderedForm::secondRemoveActivity()
{
	if(secondSelectedActivitiesListWidget->currentRow()<0 || secondSelectedActivitiesListWidget->count()<=0)
		return;
	int tmp=secondSelectedActivitiesListWidget->currentRow();
	
	secondSelectedActivitiesList.removeAt(tmp);

	secondSelectedActivitiesListWidget->setCurrentRow(-1);
	QListWidgetItem* item=secondSelectedActivitiesListWidget->takeItem(tmp);
	delete item;
	if(tmp<secondSelectedActivitiesListWidget->count())
		secondSelectedActivitiesListWidget->setCurrentRow(tmp);
	else
		secondSelectedActivitiesListWidget->setCurrentRow(secondSelectedActivitiesListWidget->count()-1);
}

void ModifyConstraintTwoSetsOfActivitiesOrderedForm::firstClear()
{
	firstSelectedActivitiesListWidget->clear();
	firstSelectedActivitiesList.clear();
}

void ModifyConstraintTwoSetsOfActivitiesOrderedForm::secondClear()
{
	secondSelectedActivitiesListWidget->clear();
	secondSelectedActivitiesList.clear();
}

void ModifyConstraintTwoSetsOfActivitiesOrderedForm::swap()
{
	this->firstSelectedActivitiesList.swap(this->secondSelectedActivitiesList);
	
	firstSelectedActivitiesListWidget->clear();
	secondSelectedActivitiesListWidget->clear();

	for(int actId : qAsConst(this->firstSelectedActivitiesList)){
		Activity *act=gt.rules.activitiesPointerHash.value(actId, nullptr);
		assert(act!=nullptr);
		firstSelectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
	}

	for(int actId : qAsConst(this->secondSelectedActivitiesList)){
		Activity *act=gt.rules.activitiesPointerHash.value(actId, nullptr);
		assert(act!=nullptr);
		secondSelectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
	}
}
