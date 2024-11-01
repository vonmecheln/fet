/***************************************************************************
                          addconstraintstudentssetactivitytagmaxhourscontinuouslyform.cpp  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Liviu Lalescu
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

#include "addconstraintstudentssetactivitytagmaxhourscontinuouslyform.h"
#include "timeconstraint.h"

AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	maxHoursSpinBox->setMinimum(1);
	maxHoursSpinBox->setMaximum(gt.rules.nHoursPerDay);
	maxHoursSpinBox->setValue(gt.rules.nHoursPerDay);

	updateStudentsSetComboBox();
	
	updateActivityTagsComboBox();
}

AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::~AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::updateStudentsSetComboBox()
{
	populateStudentsComboBox(studentsComboBox);
}

void AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::updateActivityTagsComboBox()
{
	for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
		activityTagsComboBox->addItem(at->name);
}

void AddConstraintStudentsSetActivityTagMaxHoursContinuouslyForm::addCurrentConstraint()
{
	TimeConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight"));
		return;
	}

	int maxHours=maxHoursSpinBox->value();

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

	ctr=new ConstraintStudentsSetActivityTagMaxHoursContinuously(weight, maxHours, students_name, activityTagName);

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
