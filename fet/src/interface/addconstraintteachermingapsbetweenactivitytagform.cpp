/***************************************************************************
                          addconstraintteachermingapsbetweenactivitytagform.cpp  -  description
                             -------------------
    begin                : 2021
    copyright            : (C) 2021 by Liviu Lalescu
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

#include "addconstraintteachermingapsbetweenactivitytagform.h"
#include "timeconstraint.h"

AddConstraintTeacherMinGapsBetweenActivityTagForm::AddConstraintTeacherMinGapsBetweenActivityTagForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp2=teacherComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp4=activityTagComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	minGapsSpinBox->setMinimum(1);
	minGapsSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minGapsSpinBox->setValue(1);

	updateTeacherComboBox();
	
	updateActivityTagComboBox();

	if(activityTagComboBox->count()>=1)
		activityTagComboBox->setCurrentIndex(0);
}

AddConstraintTeacherMinGapsBetweenActivityTagForm::~AddConstraintTeacherMinGapsBetweenActivityTagForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeacherMinGapsBetweenActivityTagForm::updateTeacherComboBox()
{
	for(Teacher* tch : std::as_const(gt.rules.teachersList))
		teacherComboBox->addItem(tch->name);
}

void AddConstraintTeacherMinGapsBetweenActivityTagForm::updateActivityTagComboBox()
{
	for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
		activityTagComboBox->addItem(at->name);
}

void AddConstraintTeacherMinGapsBetweenActivityTagForm::addCurrentConstraint()
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
	if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - must be 100%"));
		return;
	}

	int minGaps=minGapsSpinBox->value();

	QString teacher_name=teacherComboBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(teacher_name);
	if(teacher_ID<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}
	
	QString activityTagName=activityTagComboBox->currentText();
	int acttagindex=gt.rules.searchActivityTag(activityTagName);
	if(acttagindex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid activity tag"));
		return;
	}

	ctr=new ConstraintTeacherMinGapsBetweenActivityTag(weight, teacher_name, minGaps, activityTagName);

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
