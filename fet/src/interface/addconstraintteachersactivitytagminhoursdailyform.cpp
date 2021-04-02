/***************************************************************************
                          addconstraintteachersactivitytagminhoursdailyform.cpp  -  description
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

#include "longtextmessagebox.h"

#include "addconstraintteachersactivitytagminhoursdailyform.h"
#include "timeconstraint.h"

AddConstraintTeachersActivityTagMinHoursDailyForm::AddConstraintTeachersActivityTagMinHoursDailyForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	updateMinHoursSpinBox();
	
	allowEmptyDaysCheckBox->setChecked(false);

	activityTagsComboBox->clear();
	for(ActivityTag* at : qAsConst(gt.rules.activityTagsList))
		activityTagsComboBox->addItem(at->name);
}

AddConstraintTeachersActivityTagMinHoursDailyForm::~AddConstraintTeachersActivityTagMinHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeachersActivityTagMinHoursDailyForm::updateMinHoursSpinBox(){
	minHoursSpinBox->setMinimum(1);
	minHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minHoursSpinBox->setValue(1);
}

void AddConstraintTeachersActivityTagMinHoursDailyForm::addCurrentConstraint()
{
	TimeConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET warning"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	QString activityTagName=activityTagsComboBox->currentText();
	int activityTagIndex=gt.rules.searchActivityTag(activityTagName);
	if(activityTagIndex<0){
		QMessageBox::warning(this, tr("FET warning"),
			tr("Invalid activity tag"));
		return;
	}

	if(allowEmptyDaysCheckBox->isChecked() && minHoursSpinBox->value()==1){
		QMessageBox::warning(this, tr("FET warning"), tr("Allow empty days is selected and min hours daily is 1, so this would be a useless constraint."));
		return;
	}
	
	int min_hours=minHoursSpinBox->value();

	ctr=new ConstraintTeachersActivityTagMinHoursDaily(weight, min_hours, allowEmptyDaysCheckBox->isChecked(), activityTagName);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2)
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
