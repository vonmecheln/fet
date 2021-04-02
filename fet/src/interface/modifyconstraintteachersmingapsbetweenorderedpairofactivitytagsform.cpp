/***************************************************************************
                          modifyconstraintteachersmingapsbetweenorderedpairofactivitytagsform.cpp  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Lalescu Liviu
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

#include "modifyconstraintteachersmingapsbetweenorderedpairofactivitytagsform.h"
#include "timeconstraint.h"

ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm::ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm
 (QWidget* parent, ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp4=firstActivityTagComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	QSize tmp5=secondActivityTagComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	updateFirstActivityTagComboBox();
	updateSecondActivityTagComboBox();

	minGapsSpinBox->setMinimum(1);
	minGapsSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minGapsSpinBox->setValue(ctr->minGaps);

	constraintChanged();
}

ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm::~ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm::updateFirstActivityTagComboBox()
{
	firstActivityTagComboBox->clear();
	int j=-1;
	for(int i=0; i<gt.rules.activityTagsList.count(); i++){
		ActivityTag* at=gt.rules.activityTagsList.at(i);
		firstActivityTagComboBox->addItem(at->name);
		if(at->name==this->_ctr->firstActivityTag)
			j=i;
	}
	assert(j>=0);
	firstActivityTagComboBox->setCurrentIndex(j);

	constraintChanged();
}

void ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm::updateSecondActivityTagComboBox()
{
	secondActivityTagComboBox->clear();
	int j=-1;
	for(int i=0; i<gt.rules.activityTagsList.count(); i++){
		ActivityTag* at=gt.rules.activityTagsList.at(i);
		secondActivityTagComboBox->addItem(at->name);
		if(at->name==this->_ctr->secondActivityTag)
			j=i;
	}
	assert(j>=0);
	secondActivityTagComboBox->setCurrentIndex(j);

	constraintChanged();
}

void ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm::constraintChanged()
{
}

void ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET warning"),
			tr("Invalid weight (percentage)"));
		return;
	}
	if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - must be 100%"));
		return;
	}

	QString firstActivityTagName=firstActivityTagComboBox->currentText();
	int fati=gt.rules.searchActivityTag(firstActivityTagName);
	if(fati<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid activity tag"));
		return;
	}

	QString secondActivityTagName=secondActivityTagComboBox->currentText();
	int sati=gt.rules.searchActivityTag(secondActivityTagName);
	if(sati<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid activity tag"));
		return;
	}
	
	if(firstActivityTagName==secondActivityTagName){
		QMessageBox::warning(this, tr("FET warning"), tr("The two activity tags cannot be the same"));
		return;
	}

	this->_ctr->weightPercentage=weight;
	this->_ctr->firstActivityTag=firstActivityTagName;
	this->_ctr->secondActivityTag=secondActivityTagName;
	this->_ctr->minGaps=minGapsSpinBox->value();

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm::cancel()
{
	this->close();
}
