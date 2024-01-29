/***************************************************************************
                          addconstraintstudentsactivitytagminhoursdailyform.cpp  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Liviu Lalescu
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

#include "addconstraintstudentsactivitytagminhoursdailyform.h"
#include "timeconstraint.h"

AddConstraintStudentsActivityTagMinHoursDailyForm::AddConstraintStudentsActivityTagMinHoursDailyForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintStudentsActivityTagMinHoursDailyForm::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintStudentsActivityTagMinHoursDailyForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	minHoursSpinBox->setMinimum(1);
	minHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minHoursSpinBox->setValue(1);
	
	allowEmptyDaysCheckBox->setChecked(false);

	updateActivityTagsComboBox();
}

AddConstraintStudentsActivityTagMinHoursDailyForm::~AddConstraintStudentsActivityTagMinHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsActivityTagMinHoursDailyForm::updateActivityTagsComboBox()
{
	for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
		activityTagsComboBox->addItem(at->name);
}

void AddConstraintStudentsActivityTagMinHoursDailyForm::addCurrentConstraint()
{
	TimeConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	int minHours=minHoursSpinBox->value();

	QString activityTagName=activityTagsComboBox->currentText();
	int acttagindex=gt.rules.searchActivityTag(activityTagName);
	if(acttagindex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid activity tag"));
		return;
	}
	
	if(allowEmptyDaysCheckBox->isChecked() && minHoursSpinBox->value()==1){
		QMessageBox::warning(this, tr("FET warning"), tr("Allow empty days is selected and min hours daily is 1, so this would be a useless constraint."));
		return;
	}

	ctr=new ConstraintStudentsActivityTagMinHoursDaily(weight, minHours, allowEmptyDaysCheckBox->isChecked(), activityTagName);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2){
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));

		gt.rules.addUndoPoint(tr("Added the constraint:\n\n%1").arg(ctr->getDetailedDescription(gt.rules)));
	}
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
