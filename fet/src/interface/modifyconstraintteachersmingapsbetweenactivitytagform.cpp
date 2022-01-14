/***************************************************************************
                          modifyconstraintteachersmingapsbetweenactivitytagform.cpp  -  description
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

#include "modifyconstraintteachersmingapsbetweenactivitytagform.h"
#include "timeconstraint.h"

ModifyConstraintTeachersMinGapsBetweenActivityTagForm::ModifyConstraintTeachersMinGapsBetweenActivityTagForm
 (QWidget* parent, ConstraintTeachersMinGapsBetweenActivityTag* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp4=activityTagComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	updateActivityTagComboBox();

	minGapsSpinBox->setMinimum(1);
	minGapsSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minGapsSpinBox->setValue(ctr->minGaps);
}

ModifyConstraintTeachersMinGapsBetweenActivityTagForm::~ModifyConstraintTeachersMinGapsBetweenActivityTagForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeachersMinGapsBetweenActivityTagForm::updateActivityTagComboBox()
{
	activityTagComboBox->clear();
	int j=-1;
	for(int i=0; i<gt.rules.activityTagsList.count(); i++){
		ActivityTag* at=gt.rules.activityTagsList.at(i);
		activityTagComboBox->addItem(at->name);
		if(at->name==this->_ctr->activityTag)
			j=i;
	}
	assert(j>=0);
	activityTagComboBox->setCurrentIndex(j);
}

void ModifyConstraintTeachersMinGapsBetweenActivityTagForm::ok()
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

	QString activityTagName=activityTagComboBox->currentText();
	int ati=gt.rules.searchActivityTag(activityTagName);
	if(ati<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid activity tag"));
		return;
	}

	this->_ctr->weightPercentage=weight;
	this->_ctr->activityTag=activityTagName;
	this->_ctr->minGaps=minGapsSpinBox->value();

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeachersMinGapsBetweenActivityTagForm::cancel()
{
	this->close();
}
