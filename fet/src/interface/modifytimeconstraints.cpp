/***************************************************************************
                          addormodifytimeconstraint.cpp  -  description
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
				dialogTitle=tr("Modify multiple teacher max days per week", "The title of the dialog to modify more constraints of this type");
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
				dialogTitle=tr("Modify multiple teacher max gaps per week", "The title of the dialog to modify more constraints of this type");
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
				dialogTitle=tr("Modify multiple teacher max hours daily", "The title of the dialog to modify more constraints of this type");
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
				dialogTitle=tr("Modify multiple teacher max hours continuously", "The title of the dialog to modify more constraints of this type");
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
				dialogTitle=tr("Modify multiple teacher min hours daily", "The title of the dialog to modify more constraints of this type");
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
				dialogTitle=tr("Modify multiple teacher max gaps per day", "The title of the dialog to modify more constraints of this type");
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
				dialogTitle=tr("Modify multiple teacher min days per week", "The title of the dialog to modify more constraints of this type");
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
		if(type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY){
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

	if(type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY){
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
				performedOperation+=tr("Changed the maximum number of days per week to %1 in these %2 time constraints:\n%3")
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
				performedOperation+=tr("Changed the maximum number of gaps per week to %1 in these %2 time constraints:\n%3")
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
				performedOperation+=tr("Changed the maximum number of hours daily to %1 in these %2 time constraints:\n%3")
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
				performedOperation+=tr("Changed the maximum number of hours continuously to %1 in these %2 time constraints:\n%3")
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

				performedOperation+=tr("Changed the minimum number of hours daily to %1 in these %2 time constraints:\n%3")
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
				performedOperation+=tr("Changed the maximum number of gaps per day to %1 in these %2 time constraints:\n%3")
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
				performedOperation+=tr("Changed the minimum number of days per week to %1 in these %2 time constraints:\n%3")
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

		default:
			assert(0);
			break;
	}
}
