/***************************************************************************
                          addconstraintteachermingapsbetweenorderedpairofactivitytagsbetweenmorningandafternoonform.cpp  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
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

#include "addconstraintteachermingapsbetweenorderedpairofactivitytagsbetweenmorningandafternoonform.h"
#include "timeconstraint.h"

AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm::AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm::addCurrentConstraint);
	connect(addConstraintsPushButton, &QPushButton::clicked, this, &AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm::addCurrentConstraints);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm::close);
	
	connect(swapPushButton, &QPushButton::clicked, this, &AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm::swap);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp2=teacherComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp4=firstActivityTagComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	QSize tmp5=secondActivityTagComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	minGapsSpinBox->setMinimum(1);
	minGapsSpinBox->setMaximum(2*gt.rules.nHoursPerDay);
	minGapsSpinBox->setValue(1);

	updateTeacherComboBox();
	
	updateFirstActivityTagComboBox();
	updateSecondActivityTagComboBox();

	if(firstActivityTagComboBox->count()>=1)
		firstActivityTagComboBox->setCurrentIndex(0);

	if(secondActivityTagComboBox->count()>=2)
		secondActivityTagComboBox->setCurrentIndex(1);
	else if(secondActivityTagComboBox->count()>=1)
		secondActivityTagComboBox->setCurrentIndex(0);
}

AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm::~AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm::updateTeacherComboBox()
{
	for(Teacher* tch : std::as_const(gt.rules.teachersList))
		teacherComboBox->addItem(tch->name);
}

void AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm::updateFirstActivityTagComboBox()
{
	for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
		firstActivityTagComboBox->addItem(at->name);
}

void AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm::updateSecondActivityTagComboBox()
{
	for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
		secondActivityTagComboBox->addItem(at->name);
}

void AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm::addCurrentConstraint()
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

	ctr=new ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(weight, teacher_name, minGaps, firstActivityTagName, secondActivityTagName);

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

void AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm::addCurrentConstraints()
{
	QMessageBox::StandardButton res=QMessageBox::question(this, tr("FET confirmation"),
	 tr("This operation will add multiple constraints, one for each teacher, regardless of the one selected in the combo box. Do you want to continue?"),
	 QMessageBox::Cancel | QMessageBox::Yes);
	if(res==QMessageBox::Cancel)
		return;

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

	QString ctrs;
	for(Teacher* tch : std::as_const(gt.rules.teachersList)){
		TimeConstraint *ctr=new ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(weight, tch->name, minGaps, firstActivityTagName, secondActivityTagName);
		bool tmp2=gt.rules.addTimeConstraint(ctr);
		assert(tmp2);

		ctrs+=ctr->getDetailedDescription(gt.rules);
		ctrs+="\n";
	}

	QMessageBox::information(this, tr("FET information"), tr("Added %1 time constraints.").arg(gt.rules.teachersList.count()));

	if(gt.rules.teachersList.count()>0)
		gt.rules.addUndoPoint(tr("Added %1 constraints, one for each teacher:\n\n%2", "%1 is the number of constraints, %2 is their detailed description")
						  .arg(gt.rules.teachersList.count()).arg(ctrs));
}

void AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm::swap()
{
	int t1=firstActivityTagComboBox->currentIndex();
	int t2=secondActivityTagComboBox->currentIndex();
	firstActivityTagComboBox->setCurrentIndex(t2);
	secondActivityTagComboBox->setCurrentIndex(t1);
}
