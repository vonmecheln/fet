/***************************************************************************
                          modifyconstrainttwoactivitiesorderedform.cpp  -  description
                             -------------------
    begin                : Aug 21, 2007
    copyright            : (C) 2007 by Liviu Lalescu
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

#include "modifyconstrainttwoactivitiesorderedform.h"
#include "timeconstraint.h"

ModifyConstraintTwoActivitiesOrderedForm::ModifyConstraintTwoActivitiesOrderedForm(QWidget* parent, ConstraintTwoActivitiesOrdered* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintTwoActivitiesOrderedForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintTwoActivitiesOrderedForm::cancel);

	connect(swapPushButton, &QPushButton::clicked, this, &ModifyConstraintTwoActivitiesOrderedForm::swap);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp5=firstActivitiesComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	QSize tmp6=secondActivitiesComboBox->minimumSizeHint();
	Q_UNUSED(tmp6);
	
	firstActivitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	secondActivitiesComboBox->setMaximumWidth(maxRecommendedWidth(this));
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));

	updateActivitiesComboBox();
}

ModifyConstraintTwoActivitiesOrderedForm::~ModifyConstraintTwoActivitiesOrderedForm()
{
	saveFETDialogGeometry(this);
}

bool ModifyConstraintTwoActivitiesOrderedForm::filterOk(Activity* act)
{
	Q_UNUSED(act);

	int ok=true;
	
	return ok;
}

void ModifyConstraintTwoActivitiesOrderedForm::filterChanged()
{
	this->updateActivitiesComboBox();
}

void ModifyConstraintTwoActivitiesOrderedForm::updateActivitiesComboBox(){
	firstActivitiesComboBox->clear();
	firstActivitiesList.clear();

	secondActivitiesComboBox->clear();
	secondActivitiesList.clear();
	
	int i=0, j=-1;
	for(int k=0; k<gt.rules.activitiesList.size(); k++){
		Activity* act=gt.rules.activitiesList[k];
		if(filterOk(act)){
			firstActivitiesComboBox->addItem(act->getDescription(gt.rules));
			this->firstActivitiesList.append(act->id);

			if(act->id==this->_ctr->firstActivityId)
				j=i;
				
			i++;
		}
	}
	//assert(j>=0); only the first time
	firstActivitiesComboBox->setCurrentIndex(j);

	i=0, j=-1;
	for(int k=0; k<gt.rules.activitiesList.size(); k++){
		Activity* act=gt.rules.activitiesList[k];
		if(filterOk(act)){
			secondActivitiesComboBox->addItem(act->getDescription(gt.rules));
			this->secondActivitiesList.append(act->id);

			if(act->id==this->_ctr->secondActivityId)
				j=i;
				
			i++;
		}
	}
	//assert(j>=0); only the first time
	secondActivitiesComboBox->setCurrentIndex(j);
}

void ModifyConstraintTwoActivitiesOrderedForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	int tmp2=firstActivitiesComboBox->currentIndex();
	assert(tmp2<gt.rules.activitiesList.size());
	assert(tmp2<firstActivitiesList.size());
	if(tmp2<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid first activity"));
		return;
	}
	int fid=firstActivitiesList.at(tmp2);
	
	int tmp3=secondActivitiesComboBox->currentIndex();
	assert(tmp3<gt.rules.activitiesList.size());
	assert(tmp3<secondActivitiesList.size());
	if(tmp3<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid second activity"));
		return;
	}
	int sid=secondActivitiesList.at(tmp3);

	if(sid==fid){
		QMessageBox::warning(this, tr("FET information"),
			tr("Same activities - impossible"));
		return;
	}

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;
	this->_ctr->firstActivityId=fid;
	this->_ctr->secondActivityId=sid;
	
	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);

	this->close();
}

void ModifyConstraintTwoActivitiesOrderedForm::cancel()
{
	this->close();
}

void ModifyConstraintTwoActivitiesOrderedForm::swap()
{
	int t1=firstActivitiesComboBox->currentIndex();
	int t2=secondActivitiesComboBox->currentIndex();
	firstActivitiesComboBox->setCurrentIndex(t2);
	secondActivitiesComboBox->setCurrentIndex(t1);
}
