/***************************************************************************
                          addconstraintstudentssetactivitytagminhoursdailyform.cpp  -  description
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

#include "addconstraintstudentssetactivitytagminhoursdailyform.h"
#include "timeconstraint.h"

AddConstraintStudentsSetActivityTagMinHoursDailyForm::AddConstraintStudentsSetActivityTagMinHoursDailyForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	minHoursSpinBox->setMinimum(1);
	minHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minHoursSpinBox->setValue(1);
	
	allowEmptyDaysCheckBox->setChecked(false);

	updateStudentsSetComboBox();
	
	updateActivityTagsComboBox();
}

AddConstraintStudentsSetActivityTagMinHoursDailyForm::~AddConstraintStudentsSetActivityTagMinHoursDailyForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsSetActivityTagMinHoursDailyForm::updateStudentsSetComboBox()
{
	populateStudentsComboBox(studentsComboBox);
}

void AddConstraintStudentsSetActivityTagMinHoursDailyForm::updateActivityTagsComboBox()
{
	for(ActivityTag* at : qAsConst(gt.rules.activityTagsList))
		activityTagsComboBox->addItem(at->name);
}

void AddConstraintStudentsSetActivityTagMinHoursDailyForm::addCurrentConstraint()
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

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==nullptr){
		QMessageBox::warning(this, tr("FET warning"),
			tr("Invalid students set"));
		return;
	}
	
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

	ctr=new ConstraintStudentsSetActivityTagMinHoursDaily(weight, minHours, allowEmptyDaysCheckBox->isChecked(), students_name, activityTagName);

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
