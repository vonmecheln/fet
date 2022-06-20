//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu <Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2021 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "timetable.h"
#include "rules.h"

#include "termsform.h"

#include <QMessageBox>

extern Timetable gt;

extern const QString COMPANY;
extern const QString PROGRAM;

TermsForm::TermsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);
	
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	numberOfTermsSpinBox->setMinimum(1);
	numberOfTermsSpinBox->setMaximum(MAX_DAYS_PER_WEEK);
	numberOfTermsSpinBox->setValue(gt.rules.nTerms);

	numberOfDaysPerTermSpinBox->setMinimum(1);
	numberOfDaysPerTermSpinBox->setMaximum(MAX_DAYS_PER_WEEK);
	numberOfDaysPerTermSpinBox->setValue(gt.rules.nDaysPerTerm);
}

TermsForm::~TermsForm()
{
	saveFETDialogGeometry(this);
}

void TermsForm::ok()
{
	int cnt_mod=0;
	for(TimeConstraint* tc : qAsConst(gt.rules.timeConstraintsList)){
		if(tc->type==CONSTRAINT_MAX_TERMS_BETWEEN_ACTIVITIES){
			ConstraintMaxTermsBetweenActivities* cmt=(ConstraintMaxTermsBetweenActivities*)tc;
			if(cmt->maxTerms>=numberOfTermsSpinBox->value())
				cnt_mod++;
		}
		else if(tc->type==CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TERMS){
			ConstraintActivitiesOccupyMaxTerms* camt=(ConstraintActivitiesOccupyMaxTerms*)tc;
			if(camt->maxOccupiedTerms>numberOfTermsSpinBox->value())
				cnt_mod++;
		}
	}
	
	QString s=QString("");
	if(cnt_mod>0){
		s+=tr("%1 constraints will be modified.", "%1 is the number of constraints").arg(cnt_mod);
		s+=" ";
	}
	s+=tr("Do you want to continue?");

	int res=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes|QMessageBox::Cancel);
	
	if(res==QMessageBox::Cancel)
		return;
	
	gt.rules.setTerms(numberOfTermsSpinBox->value(), numberOfDaysPerTermSpinBox->value());
	
	this->close();
}

void TermsForm::cancel()
{
	this->close();
}
