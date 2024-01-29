/***************************************************************************
                          addconstraintstudentssetmingapsbetweenorderedpairofactivitytagsform.cpp  -  description
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

#include "addconstraintstudentssetmingapsbetweenorderedpairofactivitytagsform.h"
#include "timeconstraint.h"

AddConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsForm::AddConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsForm::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsForm::close);

	connect(swapPushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsForm::swap);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp4=firstActivityTagComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	QSize tmp5=secondActivityTagComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);
	
	minGapsSpinBox->setMinimum(1);
	minGapsSpinBox->setMaximum(gt.rules.nHoursPerDay);
	minGapsSpinBox->setValue(1);

	updateStudentsSetComboBox();
	
	updateFirstActivityTagComboBox();
	updateSecondActivityTagComboBox();

	if(firstActivityTagComboBox->count()>=1)
		firstActivityTagComboBox->setCurrentIndex(0);

	if(secondActivityTagComboBox->count()>=2)
		secondActivityTagComboBox->setCurrentIndex(1);
	else if(secondActivityTagComboBox->count()>=1)
		secondActivityTagComboBox->setCurrentIndex(0);
}

AddConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsForm::~AddConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsForm::updateStudentsSetComboBox()
{
	populateStudentsComboBox(studentsComboBox);
}

void AddConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsForm::updateFirstActivityTagComboBox()
{
	for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
		firstActivityTagComboBox->addItem(at->name);
}

void AddConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsForm::updateSecondActivityTagComboBox()
{
	for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
		secondActivityTagComboBox->addItem(at->name);
}

void AddConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsForm::addCurrentConstraint()
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

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==nullptr){
		QMessageBox::warning(this, tr("FET warning"),
			tr("Invalid students set"));
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

	ctr=new ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags(weight, students_name, minGaps, firstActivityTagName, secondActivityTagName);

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

void AddConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsForm::swap()
{
	int t1=firstActivityTagComboBox->currentIndex();
	int t2=secondActivityTagComboBox->currentIndex();
	firstActivityTagComboBox->setCurrentIndex(t2);
	secondActivityTagComboBox->setCurrentIndex(t1);
}
