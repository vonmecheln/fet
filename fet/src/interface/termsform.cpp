//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2021 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
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
	
	connect(okPushButton, &QPushButton::clicked, this, &TermsForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &TermsForm::cancel);

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
	for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList)){
		switch(tc->type){
			case CONSTRAINT_MAX_TERMS_BETWEEN_ACTIVITIES:
				{
					ConstraintMaxTermsBetweenActivities* cmt=(ConstraintMaxTermsBetweenActivities*)tc;
					if(cmt->maxTerms>=numberOfTermsSpinBox->value())
						cnt_mod++;
					break;
				}
			case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TERMS:
				{
					ConstraintActivitiesOccupyMaxTerms* camt=(ConstraintActivitiesOccupyMaxTerms*)tc;
					if(camt->maxOccupiedTerms>numberOfTermsSpinBox->value())
						cnt_mod++;
					break;
				}
			case CONSTRAINT_TEACHER_MAX_HOURS_PER_TERM:
				{
					ConstraintTeacherMaxHoursPerTerm* ctmht=(ConstraintTeacherMaxHoursPerTerm*)tc;
					if(ctmht->maxHoursPerTerm > numberOfDaysPerTermSpinBox->value() * gt.rules.nHoursPerDay)
						cnt_mod++;
					break;
				}
			case CONSTRAINT_TEACHERS_MAX_HOURS_PER_TERM:
				{
					ConstraintTeachersMaxHoursPerTerm* ctsmht=(ConstraintTeachersMaxHoursPerTerm*)tc;
					if(ctsmht->maxHoursPerTerm > numberOfDaysPerTermSpinBox->value() * gt.rules.nHoursPerDay)
						cnt_mod++;
					break;
				}
			
			default:
				//do nothing
				break;
		}
	}
	
	if(cnt_mod>0){
		QString s=QString("");
		s+=tr("%1 constraints will be modified.", "%1 is the number of constraints").arg(cnt_mod);
		s+=QString(" ");
		s+=tr("Do you want to continue?");

		int res=QMessageBox::warning(this, tr("FET warning"), s, QMessageBox::Yes|QMessageBox::Cancel);
	
		if(res==QMessageBox::Cancel)
			return;
	}
	
	int oldnt=gt.rules.nTerms;
	int oldndt=gt.rules.nDaysPerTerm;

	gt.rules.setTerms(numberOfTermsSpinBox->value(), numberOfDaysPerTermSpinBox->value());

	gt.rules.addUndoPoint(tr("The number of terms was changed from %1 to %2.").arg(oldnt).arg(gt.rules.nTerms)
	  +QString(" ")+tr("The number of days per term was changed from %1 to %2.").arg(oldndt).arg(gt.rules.nDaysPerTerm));
	
	this->close();
}

void TermsForm::cancel()
{
	this->close();
}
