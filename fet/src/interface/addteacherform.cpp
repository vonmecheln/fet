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
#include "addteacherform.h"
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

AddTeacherForm::AddTeacherForm(QWidget* parent, QHash<QString, int>& activeHoursHash): QDialog(parent)
{
	setupUi(this);
	
	activeHoursHashPointer=&activeHoursHash;
	
	teachersMorningsAfternoonsBehaviorChosen=false;
	
	teacherNameLineEdit->setFocus();
	
	addTeacherPushButton->setDefault(true);

	connect(closePushButton, &QPushButton::clicked, this, &AddTeacherForm::close);
	connect(addTeacherPushButton, &QPushButton::clicked, this, &AddTeacherForm::addTeacher);
	connect(helpPushButton, &QPushButton::clicked, this, &AddTeacherForm::help);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		int preselectedRadioButton=TEACHER_MORNINGS_AFTERNOONS_BEHAVIOR_NOT_INITIALIZED;
		QSettings settings(COMPANY, PROGRAM);
		if(settings.contains(this->metaObject()->className()+QString("/preselected-teachers-mornings-afternoons-behavior"))){
			preselectedRadioButton=settings.value(this->metaObject()->className()+QString("/preselected-teachers-mornings-afternoons-behavior")).toInt();
			teachersMorningsAfternoonsBehaviorChosen=true;
		}
	
		if(teachersMorningsAfternoonsBehaviorChosen){
			assert(preselectedRadioButton!=TEACHER_MORNINGS_AFTERNOONS_BEHAVIOR_NOT_INITIALIZED);
			if(preselectedRadioButton==TEACHER_UNRESTRICTED_MORNINGS_AFTERNOONS)
				unrestrictedRadioButton->setChecked(true);
			else if(preselectedRadioButton==TEACHER_MORNING_OR_EXCLUSIVELY_AFTERNOON)
				exclusiveRadioButton->setChecked(true);
			else if(preselectedRadioButton==TEACHER_ONE_DAY_EXCEPTION)
				oneDayExceptionRadioButton->setChecked(true);
			else if(preselectedRadioButton==TEACHER_TWO_DAYS_EXCEPTION)
				twoDaysExceptionRadioButton->setChecked(true);
			else if(preselectedRadioButton==TEACHER_THREE_DAYS_EXCEPTION)
				threeDaysExceptionRadioButton->setChecked(true);
			else if(preselectedRadioButton==TEACHER_FOUR_DAYS_EXCEPTION)
				fourDaysExceptionRadioButton->setChecked(true);
			else if(preselectedRadioButton==TEACHER_FIVE_DAYS_EXCEPTION)
				fiveDaysExceptionRadioButton->setChecked(true);
		}
		else{
			unrestrictedRadioButton->setChecked(false);
			exclusiveRadioButton->setChecked(false);
			oneDayExceptionRadioButton->setChecked(false);
			twoDaysExceptionRadioButton->setChecked(false);
			threeDaysExceptionRadioButton->setChecked(false);
			fourDaysExceptionRadioButton->setChecked(false);
			fiveDaysExceptionRadioButton->setChecked(false);
		}
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

AddTeacherForm::~AddTeacherForm()
{
	saveFETDialogGeometry(this);

	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		teachersMorningsAfternoonsBehaviorChosen=false;

		int preselectedRadioButton=TEACHER_MORNINGS_AFTERNOONS_BEHAVIOR_NOT_INITIALIZED;

		if(unrestrictedRadioButton->isChecked()){
			preselectedRadioButton=TEACHER_UNRESTRICTED_MORNINGS_AFTERNOONS;
			teachersMorningsAfternoonsBehaviorChosen=true;
		}
		else if(exclusiveRadioButton->isChecked()){
			preselectedRadioButton=TEACHER_MORNING_OR_EXCLUSIVELY_AFTERNOON;
			teachersMorningsAfternoonsBehaviorChosen=true;
		}
		else if(oneDayExceptionRadioButton->isChecked()){
			preselectedRadioButton=TEACHER_ONE_DAY_EXCEPTION;
			teachersMorningsAfternoonsBehaviorChosen=true;
		}
		else if(twoDaysExceptionRadioButton->isChecked()){
			preselectedRadioButton=TEACHER_TWO_DAYS_EXCEPTION;
			teachersMorningsAfternoonsBehaviorChosen=true;
		}
		else if(threeDaysExceptionRadioButton->isChecked()){
			preselectedRadioButton=TEACHER_THREE_DAYS_EXCEPTION;
			teachersMorningsAfternoonsBehaviorChosen=true;
		}
		else if(fourDaysExceptionRadioButton->isChecked()){
			preselectedRadioButton=TEACHER_FOUR_DAYS_EXCEPTION;
			teachersMorningsAfternoonsBehaviorChosen=true;
		}
		else if(fiveDaysExceptionRadioButton->isChecked()){
			preselectedRadioButton=TEACHER_FIVE_DAYS_EXCEPTION;
			teachersMorningsAfternoonsBehaviorChosen=true;
		}
	
		if(teachersMorningsAfternoonsBehaviorChosen==true){
			assert(preselectedRadioButton!=TEACHER_MORNINGS_AFTERNOONS_BEHAVIOR_NOT_INITIALIZED);
			QSettings settings(COMPANY, PROGRAM);
			settings.setValue(this->metaObject()->className()+QString("/preselected-teachers-mornings-afternoons-behavior"), preselectedRadioButton);
		}
	}
}

void AddTeacherForm::addTeacher()
{
	QString tn=teacherNameLineEdit->text();
	if(tn.isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		return;
	}
	
	Teacher* tch=new Teacher();
	tch->name=tn;

	if(gt.rules.mode==MORNINGS_AFTERNOONS){
		if(unrestrictedRadioButton->isChecked()){
			tch->morningsAfternoonsBehavior=TEACHER_UNRESTRICTED_MORNINGS_AFTERNOONS;
		}
		else if(exclusiveRadioButton->isChecked()){
			tch->morningsAfternoonsBehavior=TEACHER_MORNING_OR_EXCLUSIVELY_AFTERNOON;
		}
		else if(oneDayExceptionRadioButton->isChecked()){
			tch->morningsAfternoonsBehavior=TEACHER_ONE_DAY_EXCEPTION;
		}
		else if(twoDaysExceptionRadioButton->isChecked()){
			tch->morningsAfternoonsBehavior=TEACHER_TWO_DAYS_EXCEPTION;
		}
		else if(threeDaysExceptionRadioButton->isChecked()){
			tch->morningsAfternoonsBehavior=TEACHER_THREE_DAYS_EXCEPTION;
		}
		else if(fourDaysExceptionRadioButton->isChecked()){
			tch->morningsAfternoonsBehavior=TEACHER_FOUR_DAYS_EXCEPTION;
		}
		else if(fiveDaysExceptionRadioButton->isChecked()){
			tch->morningsAfternoonsBehavior=TEACHER_FIVE_DAYS_EXCEPTION;
		}
		else{
			QMessageBox::information(this, tr("FET warning"), tr("Please select an option for the mornings-afternoons behavior.")+QString(" ")+
			 tr("You can press the Help button to read the information on how to correctly choose this option."));
			delete tch;
			return;
		}
	}
	
	if(!gt.rules.addTeacher(tch)){
		QMessageBox::information( this, tr("Teacher insertion dialog"),
		 tr("Could not insert item. Must be a duplicate"));
		delete tch;
	}
	else{
		QMessageBox::information(this, tr("FET information"), tr("Teacher added"));
		
		assert(!activeHoursHashPointer->contains(tn));
		activeHoursHashPointer->insert(tn, 0);

		gt.rules.addUndoPoint(tr("Added the teacher with the description:\n\n%1").arg(tch->getDetailedDescription(gt.rules)));
	}
	
	teacherNameLineEdit->selectAll();
	teacherNameLineEdit->setFocus();
}

void AddTeacherForm::help()
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
