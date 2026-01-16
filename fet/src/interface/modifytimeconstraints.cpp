/***************************************************************************
                          modifytimeconstraint.cpp  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "modifytimeconstraints.h"

#include "timetable.h"

#include "longtextmessagebox.h"

#include "utilities.h"

#include <QMessageBox>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QHeaderView>

#include <QStringList>

#include <QSet>

#include <QBrush>

#include <QGuiApplication>
#include <QPainter>

#include <QSettings>

extern Timetable gt;

extern const QString COMPANY;
extern const QString PROGRAM;

ModifyTimeConstraintsDialog::ModifyTimeConstraintsDialog(QWidget* parent, const QString& _dialogName, const QString& _dialogTitle, QEventLoop* _eventLoop): QDialog(parent)
{
	dialogName=_dialogName;
	dialogTitle=_dialogTitle;
	eventLoop=_eventLoop;

	setWindowTitle(dialogTitle);

	setAttribute(Qt::WA_DeleteOnClose);

	//resize(600, 400);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this, dialogName);
}

ModifyTimeConstraintsDialog::~ModifyTimeConstraintsDialog()
{
	saveFETDialogGeometry(this, dialogName);

	eventLoop->quit();
}

ModifyTimeConstraints::ModifyTimeConstraints(QWidget* parent, int _type, QList<TimeConstraint*> _tcl)
{
	type=_type;
	tcl=_tcl;

	firstModifyInstructionsLabel=nullptr;
	secondModifyInstructionsLabel=nullptr;

	teacherLabel=nullptr;
	teachersComboBox=nullptr;
	
	labelForSpinBox=nullptr;
	spinBox=nullptr;

	checkBox=nullptr;

	assert(!tcl.isEmpty());
	assert(tcl.first()->type==type);
	
	switch(type){
		//5
		case CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Modify multiple teacher max days per week", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMaxDaysPerWeek");

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek);
				spinBox->setValue(gt.rules.nDaysPerWeek);

				break;
			}
		//7
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK:
			{
				dialogTitle=tr("Modify multiple teacher max gaps per week", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMaxGapsPerWeek");

				firstModifyInstructionsLabel=new QLabel(tr("You must use weight percentage 100%, because otherwise you will get poor timetables. "
														   "Try to fix firstly a rather large value for max gaps per week and lower it as you find "
														   "new timetables.\nNote: teacher not available and break are not counted as gaps."));

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max gaps per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(3);

				break;
			}
		//8
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY:
			{
				dialogTitle=tr("Modify multiple teacher max hours daily", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMaxHoursDaily");

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				break;
			}
		//10
		case CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY:
			{
				dialogTitle=tr("Modify multiple teacher max hours continuously", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMaxHoursContinuously");

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max hours continuously"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				break;
			}
		//12
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY:
			{
				dialogTitle=tr("Modify multiple teacher min hours daily", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMinHoursDaily");

				firstModifyInstructionsLabel=new QLabel(tr("It is recommended to start with 2 min hours and strengthen them as you find new timetables."));

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(2);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				checkBox=new QCheckBox(tr("Allow empty days"));
				checkBox->setChecked(true);

				break;
			}
		//14
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY:
			{
				dialogTitle=tr("Modify multiple teacher max gaps per day", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMaxGapsPerDay");

				firstModifyInstructionsLabel=new QLabel(tr("You must use weight percentage 100%, because otherwise you will get poor timetables. "
														   "Try to fix firstly a rather large value for max gaps per day and lower it as you find "
														   "new timetables.\nNote: teacher not available and break are not counted as gaps."));

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max gaps per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//56
		case CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Modify multiple teacher min days per week", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMinDaysPerWeek");

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek);
				spinBox->setValue(1);

				break;
			}
		//90
		case CONSTRAINT_TEACHER_MAX_REAL_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Modify multiple teacher max real days per week", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMaxRealDaysPerWeek");

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//91
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS:
			{
				dialogTitle=tr("Modify multiple teacher max hours daily real days", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMaxHoursDailyRealDays");

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				break;
			}
		//96
		case CONSTRAINT_TEACHER_MIN_REAL_DAYS_PER_WEEK:
			{
				dialogTitle=tr("Modify multiple teacher min real days per week", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMinRealDaysPerWeek");

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(1);

				break;
			}
		//101
		case CONSTRAINT_TEACHER_MAX_AFTERNOONS_PER_WEEK:
			{
				dialogTitle=tr("Modify multiple teacher max afternoons per week", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMaxAfternoonsPerWeek");

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max afternoons per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//103
		case CONSTRAINT_TEACHER_MAX_MORNINGS_PER_WEEK:
			{
				dialogTitle=tr("Modify multiple teacher max mornings per week", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMaxMorningsPerWeek");

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max mornings per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//108
		case CONSTRAINT_TEACHER_MIN_MORNINGS_PER_WEEK:
			{
				dialogTitle=tr("Modify multiple teacher min mornings per week", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMinMorningsPerWeek");

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min mornings per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(1);

				break;
			}
		//110
		case CONSTRAINT_TEACHER_MIN_AFTERNOONS_PER_WEEK:
			{
				dialogTitle=tr("Modify multiple teacher min afternoons per week", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMinAfternoonsPerWeek");

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min afternoons per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(1);

				break;
			}
		//116
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY:
			{
				dialogTitle=tr("Modify multiple teacher max gaps per real day", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMaxGapsPerRealDay");

				firstModifyInstructionsLabel=new QLabel(tr("You must use weight percentage 100%, because otherwise you will get poor timetables. Try "
														   "to fix firstly a rather large value for max gaps per real day and lower it as you find new timetables.\n"
														   "Note: teacher not available and break are not counted as gaps."));

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max gaps per real day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				checkBox=new QCheckBox(tr("Allow one day exception of plus one"));
				checkBox->setChecked(false);

				break;
			}
		//120
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS:
			{
				dialogTitle=tr("Modify multiple teacher min hours daily for real days", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMinHoursDailyRealDays");

				firstModifyInstructionsLabel=new QLabel(tr("It is recommended to start with 2 min hours and strengthen them as you find new timetables."));

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(2);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				checkBox=new QCheckBox(tr("Allow empty days"));
				checkBox->setChecked(true);

				break;
			}
		//122
		case CONSTRAINT_TEACHER_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				dialogTitle=tr("Modify multiple teacher afternoons early max beginnings at second hour", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherAfternoonsEarlyMaxBeginningsAtSecondHour");

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max beginnings at second hour"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//124
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING:
			{
				dialogTitle=tr("Modify multiple teacher min hours per morning", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMinHoursPerMorning");

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min hours per morning"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(2);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				checkBox=new QCheckBox(tr("Allow empty mornings"));
				checkBox->setChecked(true);

				break;
			}
		//149
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				dialogTitle=tr("Modify multiple teacher max hours per all afternoons", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMaxHoursPerAllAfternoons");

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max hours per all afternoons"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay/2);
				spinBox->setValue(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay/2);

				break;
			}
		//160
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				dialogTitle=tr("Modify multiple teacher max gaps per week for real days", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMaxGapsPerWeekForRealDays");

				firstModifyInstructionsLabel=new QLabel(tr("You must use weight percentage 100%, because otherwise you will get poor timetables. "
														   "Try to fix firstly a rather large value for max gaps per real day and lower it as you "
														   "find new timetables.\nNote: teacher not available and break are not counted as gaps."));

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max gaps per week for real days"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);

				break;
			}
		//170
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			{
				dialogTitle=tr("Modify multiple teacher max gaps per morning and afternoon", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMaxGapsPerMorningAndAfternoon");

				firstModifyInstructionsLabel=new QLabel(tr("This constraint considers the gaps = the sum of gaps of the morning and of the afternoon of "
														   "each real day."));
				secondModifyInstructionsLabel=new QLabel(tr("You must use weight percentage 100%, because otherwise you will get poor timetables. Try to "
															"fix firstly a rather large value for max gaps per morning and afternoon and lower it as you "
															"find new timetables.\nNote: teacher not available and break are not counted as gaps."));

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max gaps per morning and afternoon"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//172
		case CONSTRAINT_TEACHER_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				dialogTitle=tr("Modify multiple teacher mornings early max beginnings at second hour", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMorningsEarlyMaxBeginningsAtSecondHour");

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max beginnings at second hour"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//200
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON:
			{
				dialogTitle=tr("Modify multiple teacher min hours per afternoon", "The title of the dialog to modify multiple constraints of this type at once");
				dialogName=QString("ModifyConstraintsTeacherMinHoursPerAfternoon");

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min hours per afternoon"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(2);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				checkBox=new QCheckBox(tr("Allow empty afternoons"));
				checkBox->setChecked(true);

				break;
			}

		default:
			assert(0);
			break;
	}

	if(firstModifyInstructionsLabel!=nullptr){
		firstModifyInstructionsLabel->setWordWrap(true);
		//firstModifyInstructionsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	}
	if(secondModifyInstructionsLabel!=nullptr){
		secondModifyInstructionsLabel->setWordWrap(true);
		//secondModifyInstructionsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	}

	QVBoxLayout* teacherLayout=nullptr;
	if(teachersComboBox!=nullptr){
		teacherLayout=new QVBoxLayout;
		if(teacherLabel!=nullptr)
			teacherLayout->addWidget(teacherLabel);
		teacherLayout->addWidget(teachersComboBox);

		for(Teacher* tch : std::as_const(gt.rules.teachersList))
			teachersComboBox->addItem(tch->name);

		teachersComboBox->setCurrentIndex(0);

		teachersComboBox->setEnabled(false);
	}

	eventLoop=new QEventLoop;

	dialog=new ModifyTimeConstraintsDialog(parent, dialogName, dialogTitle, eventLoop);
	//dialog->setAttribute(Qt::WA_DeleteOnClose);
	
	//dialog->setWindowTitle(dialogTitle);
	
	//centerWidgetOnScreen(dialog);
	//restoreFETDialogGeometry(dialog, dialogName);

	if(teachersComboBox!=nullptr){
		QSize tmp=teachersComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}

	okPushButton=new QPushButton(tr("OK"));
	okPushButton->setDefault(true);
	cancelPushButton=new QPushButton(tr("Cancel"));
	
	QHBoxLayout* buttons=new QHBoxLayout;
	buttons->addStretch();
	if(okPushButton!=nullptr)
		buttons->addWidget(okPushButton);
	if(cancelPushButton!=nullptr)
		buttons->addWidget(cancelPushButton);
	
	weightLineEdit=new QLineEdit(QString("100"));
	if(timeConstraintCanHaveAnyWeight(type)){
		if(type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY || type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS){
			weightLabel=new QLabel(tr("Weight percentage\n(recommended: 100.0%,\nallowed: 0.0%-100.0%)"));
		}
		else{
			weightLabel=new QLabel(tr("Weight percentage (allowed: 0.0%-100.0%)"));
		}
	}
	else{
		weightLabel=new QLabel(tr("Weight percentage (necessary: 100%)"));
		//Not read only, because the old weight may be less than 100%, and the user needs to be able to correct it to 100%.
		//weightLineEdit->setReadOnly(true);
	}
	weightLineEdit->setEnabled(false);
	
	QHBoxLayout* weight=new QHBoxLayout;
	weight->addWidget(weightLabel);
	weight->addWidget(weightLineEdit);
	
	QHBoxLayout* spinBoxLayout=nullptr;
	if(spinBox!=nullptr && labelForSpinBox!=nullptr){
		spinBoxLayout=new QHBoxLayout;

		spinBoxLayout->addWidget(labelForSpinBox);
		spinBoxLayout->addWidget(spinBox);
	}
	
	QVBoxLayout* wholeDialog=new QVBoxLayout(dialog);
	if(teacherLayout!=nullptr)
		wholeDialog->addLayout(teacherLayout);

	if(spinBoxLayout!=nullptr)
		wholeDialog->addLayout(spinBoxLayout);
	if(checkBox!=nullptr)
		wholeDialog->addWidget(checkBox);

	wholeDialog->addLayout(weight);
	wholeDialog->addLayout(buttons);

	if(okPushButton!=nullptr)
		connect(okPushButton, &QPushButton::clicked, this, &ModifyTimeConstraints::okClicked);
	if(cancelPushButton!=nullptr)
		connect(cancelPushButton, &QPushButton::clicked, this, &ModifyTimeConstraints::cancelClicked);

	assert(!tcl.isEmpty());
	TimeConstraint* oldtc=tcl.first();

	weightLineEdit->setText(CustomFETString::number(oldtc->weightPercentage));
	switch(type){
		//5
		case CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK:
			{
				ConstraintTeacherMaxDaysPerWeek* ctr=(ConstraintTeacherMaxDaysPerWeek*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->maxDaysPerWeek);

				break;
			}
		//7
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK:
			{
				ConstraintTeacherMaxGapsPerWeek* ctr=(ConstraintTeacherMaxGapsPerWeek*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->maxGaps);

				break;
			}
		//8
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY:
			{
				ConstraintTeacherMaxHoursDaily* ctr=(ConstraintTeacherMaxHoursDaily*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->maxHoursDaily);

				break;
			}
		//10
		case CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY:
			{
				ConstraintTeacherMaxHoursContinuously* ctr=(ConstraintTeacherMaxHoursContinuously*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->maxHoursContinuously);

				break;
			}
		//12
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY:
			{
				ConstraintTeacherMinHoursDaily* ctr=(ConstraintTeacherMinHoursDaily*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->minHoursDaily);
				checkBox->setChecked(ctr->allowEmptyDays);

				break;
			}
		//14
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY:
			{
				ConstraintTeacherMaxGapsPerDay* ctr=(ConstraintTeacherMaxGapsPerDay*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->maxGaps);

				break;
			}
		//56
		case CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK:
			{
				ConstraintTeacherMinDaysPerWeek* ctr=(ConstraintTeacherMinDaysPerWeek*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->minDaysPerWeek);

				break;
			}
		//90
		case CONSTRAINT_TEACHER_MAX_REAL_DAYS_PER_WEEK:
			{
				ConstraintTeacherMaxRealDaysPerWeek* ctr=(ConstraintTeacherMaxRealDaysPerWeek*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->maxDaysPerWeek);

				break;
			}
		//91
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ConstraintTeacherMaxHoursDailyRealDays* ctr=(ConstraintTeacherMaxHoursDailyRealDays*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->maxHoursDaily);

				break;
			}
		//96
		case CONSTRAINT_TEACHER_MIN_REAL_DAYS_PER_WEEK:
			{
				ConstraintTeacherMinRealDaysPerWeek* ctr=(ConstraintTeacherMinRealDaysPerWeek*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->minDaysPerWeek);

				break;
			}
		//101
		case CONSTRAINT_TEACHER_MAX_AFTERNOONS_PER_WEEK:
			{
				ConstraintTeacherMaxAfternoonsPerWeek* ctr=(ConstraintTeacherMaxAfternoonsPerWeek*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->maxAfternoonsPerWeek);

				break;
			}
		//103
		case CONSTRAINT_TEACHER_MAX_MORNINGS_PER_WEEK:
			{
				ConstraintTeacherMaxMorningsPerWeek* ctr=(ConstraintTeacherMaxMorningsPerWeek*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->maxMorningsPerWeek);

				break;
			}
		//108
		case CONSTRAINT_TEACHER_MIN_MORNINGS_PER_WEEK:
			{
				ConstraintTeacherMinMorningsPerWeek* ctr=(ConstraintTeacherMinMorningsPerWeek*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->minMorningsPerWeek);

				break;
			}
		//110
		case CONSTRAINT_TEACHER_MIN_AFTERNOONS_PER_WEEK:
			{
				ConstraintTeacherMinAfternoonsPerWeek* ctr=(ConstraintTeacherMinAfternoonsPerWeek*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->minAfternoonsPerWeek);

				break;
			}
		//116
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY:
			{
				ConstraintTeacherMaxGapsPerRealDay* ctr=(ConstraintTeacherMaxGapsPerRealDay*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->maxGaps);

				checkBox->setChecked(ctr->allowOneDayExceptionPlusOne);

				break;
			}
		//120
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS:
			{
				ConstraintTeacherMinHoursDailyRealDays* ctr=(ConstraintTeacherMinHoursDailyRealDays*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

				spinBox->setValue(ctr->minHoursDaily);
				checkBox->setChecked(ctr->allowEmptyDays);

				break;
			}
		//122
		case CONSTRAINT_TEACHER_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

				spinBox->setValue(ctr->maxBeginningsAtSecondHour);

				break;
			}
		//124
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING:
			{
				ConstraintTeacherMinHoursPerMorning* ctr=(ConstraintTeacherMinHoursPerMorning*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

				spinBox->setValue(ctr->minHoursPerMorning);
				checkBox->setChecked(ctr->allowEmptyMornings);

				break;
			}
		//149
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				ConstraintTeacherMaxHoursPerAllAfternoons* ctr=(ConstraintTeacherMaxHoursPerAllAfternoons*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->maxHoursPerAllAfternoons);

				break;
			}
		//160
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				ConstraintTeacherMaxGapsPerWeekForRealDays* ctr=(ConstraintTeacherMaxGapsPerWeekForRealDays*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->maxGaps);

				break;
			}
		//170
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			{
				ConstraintTeacherMaxGapsPerMorningAndAfternoon* ctr=(ConstraintTeacherMaxGapsPerMorningAndAfternoon*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->maxGaps);

				break;
			}
		//172
		case CONSTRAINT_TEACHER_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

				spinBox->setValue(ctr->maxBeginningsAtSecondHour);

				break;
			}
		//200
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON:
			{
				ConstraintTeacherMinHoursPerAfternoon* ctr=(ConstraintTeacherMinHoursPerAfternoon*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

				spinBox->setValue(ctr->minHoursPerAfternoon);
				checkBox->setChecked(ctr->allowEmptyAfternoons);

				break;
			}

		default:
			assert(0);
			break;
	}

	if(checkBox!=nullptr)
		connect(checkBox, &QCheckBox::toggled, this, &ModifyTimeConstraints::checkBoxToggled);

	dialog->setModal(true);
	dialog->setWindowModality(Qt::ApplicationModal);
	dialog->show();
	
	eventLoop->exec();
}

ModifyTimeConstraints::~ModifyTimeConstraints()
{
	//saveFETDialogGeometry(dialog, dialogName);
	
	//dialog->hide();
	
	//delete dialog;
	
	assert(!eventLoop->isRunning());
	delete eventLoop;
}

void ModifyTimeConstraints::okClicked()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(dialog, tr("FET information"), tr("Invalid weight (percentage)"));
		return;
	}

	if(!timeConstraintCanHaveAnyWeight(type) && weight!=100.0){
		QMessageBox::warning(dialog, tr("FET information"), tr("Invalid weight (percentage) - it has to be 100%"));
		return;
	}

	if(type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY || type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS){
		if(weight<100.0){
			int t=QMessageBox::warning(dialog, tr("FET warning"),
				tr("You selected a weight less than 100%. The generation algorithm is not perfectly optimized to work with such weights (even"
				 " if in practice it might work well). It is recommended to work only with 100% weights for these constraints. Are you sure you want to continue?"),
				 QMessageBox::Yes | QMessageBox::Cancel);
			if(t==QMessageBox::Cancel)
				return;
		}
	}

	QString oldConstraints;
	for(TimeConstraint* oldtc : std::as_const(tcl))
		oldConstraints+="\n"+oldtc->getDetailedDescription(gt.rules);

	QString performedOperation;
	
	switch(type){
		//5
		case CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK:
			{
				performedOperation+=tr("Changed the maximum number of days per week to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
						ConstraintTeacherMaxDaysPerWeek* ctr=(ConstraintTeacherMaxDaysPerWeek*)oldtc;

						//ctr->teacherName=teachersComboBox->currentText();
						ctr->maxDaysPerWeek=spinBox->value();
				}

				break;
			}
		//7
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK:
			{
				performedOperation+=tr("Changed the maximum number of gaps per week to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMaxGapsPerWeek* ctr=(ConstraintTeacherMaxGapsPerWeek*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->maxGaps=spinBox->value();
				}

				break;
			}
		//8
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY:
			{
				performedOperation+=tr("Changed the maximum number of hours daily to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMaxHoursDaily* ctr=(ConstraintTeacherMaxHoursDaily*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->maxHoursDaily=spinBox->value();
				}

				break;
			}
		//10
		case CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY:
			{
				performedOperation+=tr("Changed the maximum number of hours continuously to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMaxHoursContinuously* ctr=(ConstraintTeacherMaxHoursContinuously*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->maxHoursContinuously=spinBox->value();
				}

				break;
			}
		//12
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY:
			{
				if(!checkBox->isChecked()){
					if(gt.rules.mode!=MORNINGS_AFTERNOONS){
						QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for a teacher, "
							"please use the constraint teacher min days per week"));
						return;
					}
					else{
						QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for a teacher, "
							"please use the constraint teacher min days per week (but the min days per week constraint is for real days. You can also use the "
							"constraints teacher min mornings/afternoons per week.)"));
						return;
					}
				}

				performedOperation+=tr("Changed the minimum number of hours daily to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMinHoursDaily* ctr=(ConstraintTeacherMinHoursDaily*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->minHoursDaily=spinBox->value();
					//ctr->allowEmptyDays=checkBox->isChecked();
				}

				break;
			}
		//14
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY:
			{
				performedOperation+=tr("Changed the maximum number of gaps per day to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMaxGapsPerDay* ctr=(ConstraintTeacherMaxGapsPerDay*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->maxGaps=spinBox->value();
				}

				break;
			}
		//56
		case CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK:
			{
				performedOperation+=tr("Changed the minimum number of days per week to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMinDaysPerWeek* ctr=(ConstraintTeacherMinDaysPerWeek*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->minDaysPerWeek=spinBox->value();
				}

				break;
			}
		//90
		case CONSTRAINT_TEACHER_MAX_REAL_DAYS_PER_WEEK:
			{
				performedOperation+=tr("Changed the maximum number of real days per week to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMaxRealDaysPerWeek* ctr=(ConstraintTeacherMaxRealDaysPerWeek*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->maxDaysPerWeek=spinBox->value();
				}

				break;
			}
		//91
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS:
			{
				performedOperation+=tr("Changed the maximum number of hours daily for real days to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMaxHoursDailyRealDays* ctr=(ConstraintTeacherMaxHoursDailyRealDays*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->maxHoursDaily=spinBox->value();
				}

				break;
			}
		//96
		case CONSTRAINT_TEACHER_MIN_REAL_DAYS_PER_WEEK:
			{
				performedOperation+=tr("Changed the minimum number of real days per week to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMinRealDaysPerWeek* ctr=(ConstraintTeacherMinRealDaysPerWeek*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->minDaysPerWeek=spinBox->value();
				}

				break;
			}
		//101
		case CONSTRAINT_TEACHER_MAX_AFTERNOONS_PER_WEEK:
			{
				performedOperation+=tr("Changed the maximum number of afternoons per week to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMaxAfternoonsPerWeek* ctr=(ConstraintTeacherMaxAfternoonsPerWeek*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->maxAfternoonsPerWeek=spinBox->value();
				}

				break;
			}
		//103
		case CONSTRAINT_TEACHER_MAX_MORNINGS_PER_WEEK:
			{
				performedOperation+=tr("Changed the maximum number of mornings per week to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMaxMorningsPerWeek* ctr=(ConstraintTeacherMaxMorningsPerWeek*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->maxMorningsPerWeek=spinBox->value();
				}

				break;
			}
		//108
		case CONSTRAINT_TEACHER_MIN_MORNINGS_PER_WEEK:
			{
				performedOperation+=tr("Changed the minimum number of mornings per week to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMinMorningsPerWeek* ctr=(ConstraintTeacherMinMorningsPerWeek*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->minMorningsPerWeek=spinBox->value();
				}

				break;
			}
		//110
		case CONSTRAINT_TEACHER_MIN_AFTERNOONS_PER_WEEK:
			{
				performedOperation+=tr("Changed the minimum number of afternoons per week to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMinAfternoonsPerWeek* ctr=(ConstraintTeacherMinAfternoonsPerWeek*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->minAfternoonsPerWeek=spinBox->value();
				}

				break;
			}
		//116
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY:
			{
				performedOperation+=tr("Changed the maximum number of gaps per real day to %1 and 'allow one day exception of plus one' to '%2'"
						" in these %3 time constraints:\n%4", "%2 is yes or no, %3 is the number of modified time constraints, and %4 is their description")
						.arg(spinBox->value())
						.arg(checkBox->isChecked()?tr("yes"):tr("no"))
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMaxGapsPerRealDay* ctr=(ConstraintTeacherMaxGapsPerRealDay*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->maxGaps=spinBox->value();

					ctr->allowOneDayExceptionPlusOne=checkBox->isChecked();
				}

				break;
			}
		//120
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for this teacher, "
						"please use the constraint teacher min days per week"));
					return;
				}

				performedOperation+=tr("Changed the minimum number of hours daily for real days to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMinHoursDailyRealDays* ctr=(ConstraintTeacherMinHoursDailyRealDays*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();

					ctr->minHoursDaily=spinBox->value();
					//ctr->allowEmptyDays=checkBox->isChecked();
				}

				break;
			}
		//122
		case CONSTRAINT_TEACHER_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				performedOperation+=tr("Changed the afternoons early maximum number of beginnings at second hour to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();

					ctr->maxBeginningsAtSecondHour=spinBox->value();
				}

				break;
			}
		//124
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty mornings check box must be checked. If you need to not allow empty mornings for a teacher, "
						"please use the constraint teacher min mornings per week."));
					return;
				}

				performedOperation+=tr("Changed the minimum number of hours per morning to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMinHoursPerMorning* ctr=(ConstraintTeacherMinHoursPerMorning*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();

					ctr->minHoursPerMorning=spinBox->value();
					//ctr->allowEmptyMornings=checkBox->isChecked();
				}

				break;
			}
		//149
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				performedOperation+=tr("Changed the maximum number of hours per all afternoons to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMaxHoursPerAllAfternoons* ctr=(ConstraintTeacherMaxHoursPerAllAfternoons*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->maxHoursPerAllAfternoons=spinBox->value();
				}

				break;
			}
		//160
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				performedOperation+=tr("Changed the maximum number of gaps per week for real days to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMaxGapsPerWeekForRealDays* ctr=(ConstraintTeacherMaxGapsPerWeekForRealDays*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->maxGaps=spinBox->value();
				}

				break;
			}
		//170
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			{
				performedOperation+=tr("Changed the maximum number of gaps per morning and afternoon to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMaxGapsPerMorningAndAfternoon* ctr=(ConstraintTeacherMaxGapsPerMorningAndAfternoon*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();
					ctr->maxGaps=spinBox->value();
				}

				break;
			}
		//172
		case CONSTRAINT_TEACHER_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				performedOperation+=tr("Changed the mornings early maximum number of beginnings at second hour to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();

					ctr->maxBeginningsAtSecondHour=spinBox->value();
				}

				break;
			}
		//200
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty afternoons check box must be checked. If you need to not allow empty afternoons for a teacher, "
						"please use the constraint teacher min afternoons per week."));
					return;
				}

				performedOperation+=tr("Changed the minimum number of hours per afternoon to %1 in these %2 time constraints:\n%3",
						"%2 is the number of modified time constraints and %3 is their description")
						.arg(spinBox->value())
						.arg(tcl.count())
						.arg(oldConstraints);

				for(TimeConstraint* oldtc : std::as_const(tcl)){
					ConstraintTeacherMinHoursPerAfternoon* ctr=(ConstraintTeacherMinHoursPerAfternoon*)oldtc;

					//ctr->teacherName=teachersComboBox->currentText();

					ctr->minHoursPerAfternoon=spinBox->value();
					//ctr->allowEmptyAfternoons=checkBox->isChecked();
				}

				break;
			}

		default:
			assert(0);
			break;
	}
	
	gt.rules.addUndoPoint(performedOperation);

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);

	dialog->close();
}

void ModifyTimeConstraints::cancelClicked()
{
	dialog->close();
}

void ModifyTimeConstraints::checkBoxToggled()
{
	switch(type){
		//12
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY:
			{
				bool k=checkBox->isChecked();

				if(!k){
					checkBox->setChecked(true);
					if(gt.rules.mode!=MORNINGS_AFTERNOONS)
						QMessageBox::information(dialog, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty days for this teacher,"
							" please use constraint teacher min days per week"));
					else
						QMessageBox::information(dialog, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty days for this teacher,"
							" please use constraint teacher min days per week (but the min days per week constraint is for real days. You can also use the "
							"constraints teacher min mornings/afternoons per week.)"));
				}

				break;
			}
		//116
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//120
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS:
			{
				bool k=checkBox->isChecked();

				if(!k){
					checkBox->setChecked(true);
					QMessageBox::information(dialog, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty days for this teacher,"
						" please use constraint teacher min days per week"));
				}

				break;
			}
		//124
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING:
			{
				bool k=checkBox->isChecked();

				if(!k){
					checkBox->setChecked(true);
					QMessageBox::information(dialog, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty mornings for this teacher,"
						" please use constraint teacher min mornings per week."));
				}

				break;
			}
		//200
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON:
			{
				bool k=checkBox->isChecked();

				if(!k){
					checkBox->setChecked(true);
					QMessageBox::information(dialog, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty afternoons for this teacher,"
						" please use constraint teacher min afternoons per week."));
				}

				break;
			}

		default:
			assert(0);
			break;
	}
}
