/***************************************************************************
                          addconstraintteachermingapsbetweenorderedpairofactivitytagsform.cpp  -  description
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

#include "addconstraintteachermingapsbetweenorderedpairofactivitytagsform.h"
#include "timeconstraint.h"

AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsForm::AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, SIGNAL(clicked()), this, SLOT(addCurrentConstraint()));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp2=teacherComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp4=firstActivityTagComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	QSize tmp5=secondActivityTagComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	minGapsSpinBox->setMinimum(1);
	minGapsSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minGapsSpinBox->setValue(1);

	updateTeacherComboBox();
	
	updateFirstActivityTagComboBox();
	updateSecondActivityTagComboBox();
}

AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsForm::~AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsForm::updateTeacherComboBox()
{
	for(Teacher* tch : qAsConst(gt.rules.teachersList))
		teacherComboBox->addItem(tch->name);

	constraintChanged();
}

void AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsForm::updateFirstActivityTagComboBox()
{
	for(ActivityTag* at : qAsConst(gt.rules.activityTagsList))
		firstActivityTagComboBox->addItem(at->name);

	constraintChanged();
}

void AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsForm::updateSecondActivityTagComboBox()
{
	for(ActivityTag* at : qAsConst(gt.rules.activityTagsList))
		secondActivityTagComboBox->addItem(at->name);

	constraintChanged();
}

void AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsForm::constraintChanged()
{
}

void AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsForm::addCurrentConstraint()
{
	TimeConstraint *ctr=NULL;

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
	
	QString firstActivityTagName=firstActivityTagComboBox->currentText();
	int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
	if(facttagindex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid first activity tag"));
		return;
	}

	QString secondActivityTagName=secondActivityTagComboBox->currentText();
	int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
	if(sacttagindex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid second activity tag"));
		return;
	}
	
	if(firstActivityTagName==secondActivityTagName){
		QMessageBox::warning(this, tr("FET warning"), tr("The two activity tags cannot be the same"));
		return;
	}

	ctr=new ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags(weight, teacher_name, minGaps, firstActivityTagName, secondActivityTagName);

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
