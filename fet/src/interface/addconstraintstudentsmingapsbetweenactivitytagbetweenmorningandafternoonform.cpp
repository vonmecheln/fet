/***************************************************************************
                          addconstraintstudentsmingapsbetweenactivitytagbetweenmorningandafternoonform.cpp  -  description
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

#include "addconstraintstudentsmingapsbetweenactivitytagbetweenmorningandafternoonform.h"
#include "timeconstraint.h"

AddConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm::AddConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp4=activityTagComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);
	
	minGapsSpinBox->setMinimum(1);
	minGapsSpinBox->setMaximum(2*gt.rules.nHoursPerDay);
	minGapsSpinBox->setValue(1);

	updateActivityTagComboBox();
	
	if(activityTagComboBox->count()>=1)
		activityTagComboBox->setCurrentIndex(0);
}

AddConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm::~AddConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm::updateActivityTagComboBox()
{
	for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
		activityTagComboBox->addItem(at->name);
}

void AddConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm::addCurrentConstraint()
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

	QString activityTagName=activityTagComboBox->currentText();
	int acttagindex=gt.rules.searchActivityTag(activityTagName);
	if(acttagindex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid activity tag"));
		return;
	}

	ctr=new ConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon(weight, minGaps, activityTagName);

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
