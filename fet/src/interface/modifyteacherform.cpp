//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2020 Liviu Lalescu <https://lalescu.ro/liviu/>
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
#include "fet.h"
#include "modifyteacherform.h"
#include "teacher.h"
//#include "teachersubjectsqualificationsform.h"

#include "longtextmessagebox.h"

#include <QMessageBox>

#include <QListWidget>
#include <QAbstractItemView>

#include <QCoreApplication>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

extern const QString COMPANY;
extern const QString PROGRAM;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;
extern bool rooms_buildings_schedule_ready;

ModifyTeacherForm::ModifyTeacherForm(QWidget* parent, Teacher* _tch, QHash<QString, int>& activeHoursHash): QDialog(parent)
{
	setupUi(this);
	
	tch=_tch;
	
	activeHoursHashPointer=&activeHoursHash;
	
	teacherNameLineEdit->setText(tch->name);
	teacherNameLineEdit->selectAll();
	teacherNameLineEdit->setFocus();
	
	okPushButton->setDefault(true);

	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyTeacherForm::cancel);
	connect(okPushButton, &QPushButton::clicked, this, &ModifyTeacherForm::ok);
	connect(helpPushButton, &QPushButton::clicked, this, &ModifyTeacherForm::help);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		if(tch->morningsAfternoonsBehavior==TEACHER_MORNINGS_AFTERNOONS_BEHAVIOR_NOT_INITIALIZED){
			unrestrictedRadioButton->setChecked(false);
			exclusiveRadioButton->setChecked(false);
			oneDayExceptionRadioButton->setChecked(false);
			twoDaysExceptionRadioButton->setChecked(false);
			threeDaysExceptionRadioButton->setChecked(false);
			fourDaysExceptionRadioButton->setChecked(false);
			fiveDaysExceptionRadioButton->setChecked(false);
		}
		else if(tch->morningsAfternoonsBehavior==TEACHER_UNRESTRICTED_MORNINGS_AFTERNOONS)
			unrestrictedRadioButton->setChecked(true);
		else if(tch->morningsAfternoonsBehavior==TEACHER_MORNING_OR_EXCLUSIVELY_AFTERNOON)
			exclusiveRadioButton->setChecked(true);
		else if(tch->morningsAfternoonsBehavior==TEACHER_ONE_DAY_EXCEPTION)
			oneDayExceptionRadioButton->setChecked(true);
		else if(tch->morningsAfternoonsBehavior==TEACHER_TWO_DAYS_EXCEPTION)
			twoDaysExceptionRadioButton->setChecked(true);
		else if(tch->morningsAfternoonsBehavior==TEACHER_THREE_DAYS_EXCEPTION)
			threeDaysExceptionRadioButton->setChecked(true);
		else if(tch->morningsAfternoonsBehavior==TEACHER_FOUR_DAYS_EXCEPTION)
			fourDaysExceptionRadioButton->setChecked(true);
		else if(tch->morningsAfternoonsBehavior==TEACHER_FIVE_DAYS_EXCEPTION)
			fiveDaysExceptionRadioButton->setChecked(true);
		else
			assert(0);
	}
	else{
		unrestrictedRadioButton->setChecked(false);
		exclusiveRadioButton->setChecked(false);
		oneDayExceptionRadioButton->setChecked(false);
		twoDaysExceptionRadioButton->setChecked(false);
		threeDaysExceptionRadioButton->setChecked(false);
		fourDaysExceptionRadioButton->setChecked(false);
		fiveDaysExceptionRadioButton->setChecked(false);

		morningsAfternoonsGroupBox->setEnabled(false);
	}
}

ModifyTeacherForm::~ModifyTeacherForm()
{
	saveFETDialogGeometry(this);
}

void ModifyTeacherForm::ok()
{
	QString tn=teacherNameLineEdit->text();
	if(tn.isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		return;
	}
	
	QString oldName=tch->name;
	if(tn!=oldName && gt.rules.searchTeacher(tn)>=0){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name - another teacher has this name"));
		return;
	}
	
	int newMab=TEACHER_MORNINGS_AFTERNOONS_BEHAVIOR_NOT_INITIALIZED;
	
	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		if(unrestrictedRadioButton->isChecked()){
			newMab=TEACHER_UNRESTRICTED_MORNINGS_AFTERNOONS;
		}
		else if(exclusiveRadioButton->isChecked()){
			newMab=TEACHER_MORNING_OR_EXCLUSIVELY_AFTERNOON;
		}
		else if(oneDayExceptionRadioButton->isChecked()){
			newMab=TEACHER_ONE_DAY_EXCEPTION;
		}
		else if(twoDaysExceptionRadioButton->isChecked()){
			newMab=TEACHER_TWO_DAYS_EXCEPTION;
		}
		else if(threeDaysExceptionRadioButton->isChecked()){
			newMab=TEACHER_THREE_DAYS_EXCEPTION;
		}
		else if(fourDaysExceptionRadioButton->isChecked()){
			newMab=TEACHER_FOUR_DAYS_EXCEPTION;
		}
		else if(fiveDaysExceptionRadioButton->isChecked()){
			newMab=TEACHER_FIVE_DAYS_EXCEPTION;
		}
		else{
			QMessageBox::information(this, tr("FET warning"), tr("Please select an option for the mornings-afternoons behavior.")+QString(" ")+
			 tr("You can press the Help button to read the information on how to correctly choose this option."));
			return;
		}
		
		assert(newMab!=TEACHER_MORNINGS_AFTERNOONS_BEHAVIOR_NOT_INITIALIZED);
	}
	
	if(tn!=oldName){
		QString od=tch->getDetailedDescription(gt.rules);

		gt.rules.modifyTeacher(oldName, tn);

		tch->morningsAfternoonsBehavior=newMab;
		
		assert(activeHoursHashPointer->contains(oldName));
		int nh=activeHoursHashPointer->value(oldName);
		activeHoursHashPointer->remove(oldName);
		activeHoursHashPointer->insert(tn, nh);

		gt.rules.addUndoPoint(tr("Modified the teacher from:\n\n%1\ninto\n\n%2").arg(od).arg(tch->getDetailedDescription(gt.rules)));
	}
	else{
		QString od=tch->getDetailedDescription(gt.rules);

		tch->morningsAfternoonsBehavior=newMab;

		gt.rules.addUndoPoint(tr("Modified the teacher from:\n\n%1\ninto\n\n%2").arg(od).arg(tch->getDetailedDescription(gt.rules)));

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		teachers_schedule_ready=false;
		students_schedule_ready=false;
		rooms_buildings_schedule_ready=false;
	}
	
	this->close();
}

void ModifyTeacherForm::cancel()
{
	this->close();
}

void ModifyTeacherForm::help()
{
	QString s;
	
	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		s+=QCoreApplication::translate("MorningsAfternoonsTeacherBehavior", "To allow the teacher to work only in the morning or in the afternoon on each day but not both, use the option 'Exclusive'.");
		s+="\n\n";
		s+=QCoreApplication::translate("MorningsAfternoonsTeacherBehavior", "To allow the teacher to work only in the morning or in the afternoon on each day, not both, but with the exception of a single day"
		 " in which the teacher can work both in the morning and in the afternoon, use 'One day exception'.");
		s+="\n\n";
		s+=QCoreApplication::translate("MorningsAfternoonsTeacherBehavior", "Similarly, to say that a teacher should work in the morning or in the afternoon, not both, but with the exception of two days in which"
		 " the teacher can work both in the morning and in the afternoon, use 'Two days exception'.");
		s+="\n\n";
		s+=QCoreApplication::translate("MorningsAfternoonsTeacherBehavior", "Similarly, you can say that a teacher should work in the morning or in the afternoon, not both, but with the exception of three, four, or"
		 " five days exceptions in which the teacher can work both in the morning and in the afternoon: 'Three days exception', 'Four days exception',"
		 " 'Five days exception'.");
		s+="\n\n";
		s+=QCoreApplication::translate("MorningsAfternoonsTeacherBehavior", "To allow the teacher to work both in the morning and afternoon on each day, use the option 'Unrestricted'.");
	}
	else{
		s+=tr("Since you are not using the mornings-afternoons mode of FET, you need to simply input a non-existing teacher name.");
	}

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
