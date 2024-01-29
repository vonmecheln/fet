/***************************************************************************
                          addconstraintstudentsmaxtwoactivitytagsperdayfromn1n2n3form.cpp  -  description
                             -------------------
    begin                : 2022
    copyright            : (C) 2022 by Liviu Lalescu
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

#include "addconstraintstudentsmaxtwoactivitytagsperdayfromn1n2n3form.h"
#include "timeconstraint.h"

AddConstraintStudentsMaxTwoActivityTagsPerDayFromN1N2N3Form::AddConstraintStudentsMaxTwoActivityTagsPerDayFromN1N2N3Form(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintStudentsMaxTwoActivityTagsPerDayFromN1N2N3Form::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintStudentsMaxTwoActivityTagsPerDayFromN1N2N3Form::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
}

AddConstraintStudentsMaxTwoActivityTagsPerDayFromN1N2N3Form::~AddConstraintStudentsMaxTwoActivityTagsPerDayFromN1N2N3Form()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsMaxTwoActivityTagsPerDayFromN1N2N3Form::addCurrentConstraint()
{
	TimeConstraint *ctr=nullptr;

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

	ctr=new ConstraintStudentsMaxTwoActivityTagsPerDayFromN1N2N3(weight);

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
