/***************************************************************************
                          alltimeconstraintsform.cpp  -  description
                             -------------------
    begin                : Feb 10, 2005
    copyright            : (C) 2005 by Liviu Lalescu
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

#include "longtextmessagebox.h"

#include "alltimeconstraintsform.h"

#include "addormodifytimeconstraint.h"

#include "lockunlock.h"

#include "advancedfilterform.h"

#include <Qt>

#include <QMessageBox>

#include <QInputDialog>

#include <QPlainTextEdit>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#else
#include <QRegExp>
#endif

#include <QListWidget>
#include <QListWidgetItem>
#include <QScrollBar>
#include <QAbstractItemView>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

#include <QPalette>

#include <algorithm>
//using namespace std;

extern const QString COMPANY;
extern const QString PROGRAM;

//The order is important: we must have DESCRIPTION < DETDESCRIPTION, because we use std::stable_sort to put
//the hopefully simpler/faster/easier to check filters first.
const int DESCRIPTION=0;
const int DETDESCRIPTION=1;

const int CONTAINS=0;
const int DOESNOTCONTAIN=1;
const int REGEXP=2;
const int NOTREGEXP=3;

AllTimeConstraintsForm::AllTimeConstraintsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	filterCheckBox->setChecked(false);
	sortedCheckBox->setChecked(false);
	
	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);
	
	constraintsListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &AllTimeConstraintsForm::constraintChanged);

	//selectionChanged();
	connect(constraintsListWidget, &QListWidget::itemSelectionChanged, this, &AllTimeConstraintsForm::selectionChanged);

	connect(closePushButton, &QPushButton::clicked, this, &AllTimeConstraintsForm::close);
	connect(removeConstraintsPushButton, &QPushButton::clicked, this, &AllTimeConstraintsForm::removeConstraints);
	connect(modifyConstraintPushButton, &QPushButton::clicked, this, &AllTimeConstraintsForm::modifyConstraint);
	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &AllTimeConstraintsForm::modifyConstraint);
	connect(filterCheckBox, &QCheckBox::toggled, this, &AllTimeConstraintsForm::filter);

	connect(moveTimeConstraintUpPushButton, &QPushButton::clicked, this, &AllTimeConstraintsForm::moveTimeConstraintUp);
	connect(moveTimeConstraintDownPushButton, &QPushButton::clicked, this, &AllTimeConstraintsForm::moveTimeConstraintDown);

	connect(sortedCheckBox, &QCheckBox::toggled, this, &AllTimeConstraintsForm::sortedChanged);
	connect(activatePushButton, &QPushButton::clicked, this, &AllTimeConstraintsForm::activateConstraints);
	connect(deactivatePushButton, &QPushButton::clicked, this, &AllTimeConstraintsForm::deactivateConstraints);

	//connect(activateAllPushButton, SIG NAL(clicked()), this, SL OT(activateAllConstraints()));
	//connect(deactivateAllPushButton, SIG NAL(clicked()), this, SL OT(deactivateAllConstraints()));

	//connect(sortByCommentsPushButton, SIG NAL(clicked()), this, SL OT(sortConstraintsByComments()));
	connect(commentsPushButton, &QPushButton::clicked, this, &AllTimeConstraintsForm::constraintComments);

	connect(weightsPushButton, &QPushButton::clicked, this, &AllTimeConstraintsForm::changeWeights);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	
	QString settingsName="AllTimeConstraintsAdvancedFilterForm";
	
	all=settings.value(settingsName+"/all-conditions", "true").toBool();
	
	descrDetDescr.clear();
	int n=settings.value(settingsName+"/number-of-descriptions", "1").toInt();
	for(int i=0; i<n; i++)
		descrDetDescr.append(settings.value(settingsName+"/description/"+CustomFETString::number(i+1), CustomFETString::number(DESCRIPTION)).toInt());
	
	contains.clear();
	n=settings.value(settingsName+"/number-of-contains", "1").toInt();
	for(int i=0; i<n; i++)
		contains.append(settings.value(settingsName+"/contains/"+CustomFETString::number(i+1), CustomFETString::number(CONTAINS)).toInt());
	
	text.clear();
	n=settings.value(settingsName+"/number-of-texts", "1").toInt();
	for(int i=0; i<n; i++)
		text.append(settings.value(settingsName+"/text/"+CustomFETString::number(i+1), QString("")).toString());

	caseSensitive=settings.value(settingsName+"/case-sensitive", "false").toBool();
	
	useFilter=false;
	
	assert(filterCheckBox->isChecked()==false);
	assert(sortedCheckBox->isChecked()==false);
	
	filterChanged();
}

AllTimeConstraintsForm::~AllTimeConstraintsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());

	QString settingsName="AllTimeConstraintsAdvancedFilterForm";
	
	settings.setValue(settingsName+"/all-conditions", all);
	
	settings.setValue(settingsName+"/number-of-descriptions", descrDetDescr.count());
	settings.remove(settingsName+"/description");
	for(int i=0; i<descrDetDescr.count(); i++)
		settings.setValue(settingsName+"/description/"+CustomFETString::number(i+1), descrDetDescr.at(i));
	
	settings.setValue(settingsName+"/number-of-contains", contains.count());
	settings.remove(settingsName+"/contains");
	for(int i=0; i<contains.count(); i++)
		settings.setValue(settingsName+"/contains/"+CustomFETString::number(i+1), contains.at(i));
	
	settings.setValue(settingsName+"/number-of-texts", text.count());
	settings.remove(settingsName+"/text");
	for(int i=0; i<text.count(); i++)
		settings.setValue(settingsName+"/text/"+CustomFETString::number(i+1), text.at(i));
	
	settings.setValue(settingsName+"/case-sensitive", caseSensitive);
}

bool AllTimeConstraintsForm::filterOk(TimeConstraint* ctr)
{
	if(!useFilter)
		return true;

	assert(descrDetDescr.count()==contains.count());
	assert(contains.count()==text.count());
	
	Qt::CaseSensitivity csens=Qt::CaseSensitive;
	if(!caseSensitive)
		csens=Qt::CaseInsensitive;
	
	QList<int> perm;
	for(int i=0; i<descrDetDescr.count(); i++)
		perm.append(i);
	//Below we do a stable sorting, so that first inputted filters are hopefully filtering out more entries.
	std::stable_sort(perm.begin(), perm.end(), [this](int a, int b){return descrDetDescr.at(a)<descrDetDescr.at(b);});
	for(int i=0; i<perm.count()-1; i++)
		assert(descrDetDescr.at(perm.at(i))<=descrDetDescr.at(perm.at(i+1)));
	
	int firstPosWithDescr=-1;
	int firstPosWithDetDescr=-1;
	for(int i=0; i<perm.count(); i++){
		if(descrDetDescr.at(perm.at(i))==DESCRIPTION && firstPosWithDescr==-1){
			firstPosWithDescr=i;
		}
		else if(descrDetDescr.at(perm.at(i))==DETDESCRIPTION && firstPosWithDetDescr==-1){
			firstPosWithDetDescr=i;
		}
	}
	
	QString s=QString("");
	for(int i=0; i<perm.count(); i++){
		if(descrDetDescr.at(perm.at(i))==DESCRIPTION){
			assert(firstPosWithDescr>=0);
			
			if(i==firstPosWithDescr)
				s=ctr->getDescription(gt.rules);
		}
		else{
			assert(descrDetDescr.at(perm.at(i))==DETDESCRIPTION);
			
			assert(firstPosWithDetDescr>=0);
			
			if(i==firstPosWithDetDescr)
				s=ctr->getDetailedDescription(gt.rules);
		}

		bool okPartial=true; //We initialize okPartial to silence a MinGW 11.2.0 warning of type 'this variable might be used uninitialized'.
		
		QString t=text.at(perm.at(i));
		if(contains.at(perm.at(i))==CONTAINS){
			okPartial=s.contains(t, csens);
		}
		else if(contains.at(perm.at(i))==DOESNOTCONTAIN){
			okPartial=!(s.contains(t, csens));
		}
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
		else if(contains.at(perm.at(i))==REGEXP){
			QRegularExpression regExp(t);
			if(!caseSensitive)
				regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
			okPartial=(regExp.match(s)).hasMatch();
		}
		else if(contains.at(perm.at(i))==NOTREGEXP){
			QRegularExpression regExp(t);
			if(!caseSensitive)
				regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
			okPartial=!(regExp.match(s)).hasMatch();
		}
#else
		else if(contains.at(perm.at(i))==REGEXP){
			QRegExp regExp(t);
			regExp.setCaseSensitivity(csens);
			okPartial=(regExp.indexIn(s)>=0);
		}
		else if(contains.at(perm.at(i))==NOTREGEXP){
			QRegExp regExp(t);
			regExp.setCaseSensitivity(csens);
			okPartial=(regExp.indexIn(s)<0);
		}
#endif
		else
			assert(0);
			
		if(all && !okPartial)
			return false;
		else if(!all && okPartial)
			return true;
	}
	
	return all;
}

void AllTimeConstraintsForm::moveTimeConstraintUp()
{
	if(filterCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a time constraint up, the 'Filter' check box must not be checked."));
		return;
	}
	if(sortedCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a time constraint up, the 'Sorted' check box must not be checked."));
		return;
	}
	
	if(constraintsListWidget->count()<=1)
		return;
	int i=constraintsListWidget->currentRow();
	if(i<0 || i>=constraintsListWidget->count())
		return;
	if(i==0)
		return;
		
	QString s1=constraintsListWidget->item(i)->text();
	QString s2=constraintsListWidget->item(i-1)->text();
	
	assert(gt.rules.timeConstraintsList.count()==visibleTimeConstraintsList.count());
	TimeConstraint* tc1=gt.rules.timeConstraintsList.at(i);
	assert(tc1==visibleTimeConstraintsList.at(i));
	TimeConstraint* tc2=gt.rules.timeConstraintsList.at(i-1);
	assert(tc2==visibleTimeConstraintsList.at(i-1));
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	constraintsListWidget->item(i)->setText(s2);
	constraintsListWidget->item(i-1)->setText(s1);
	
	gt.rules.timeConstraintsList[i]=tc2;
	gt.rules.timeConstraintsList[i-1]=tc1;
	
	visibleTimeConstraintsList[i]=tc2;
	visibleTimeConstraintsList[i-1]=tc1;
	
	gt.rules.addUndoPoint(tr("A constraint was moved up:\n\n%1", "%1 is the detailed description of the constraint").arg(tc1->getDetailedDescription(gt.rules)));
	
	if(true || USE_GUI_COLORS){
		if(tc2->active)
			constraintsListWidget->item(i)->setBackground(constraintsListWidget->palette().base());
		else
			constraintsListWidget->item(i)->setBackground(constraintsListWidget->palette().alternateBase());

		if(tc1->active)
			constraintsListWidget->item(i-1)->setBackground(constraintsListWidget->palette().base());
		else
			constraintsListWidget->item(i-1)->setBackground(constraintsListWidget->palette().alternateBase());
	}

	constraintsListWidget->setCurrentRow(i-1);
	constraintChanged(/*i-1*/);
}

void AllTimeConstraintsForm::moveTimeConstraintDown()
{
	if(filterCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a time constraint down, the 'Filter' check box must not be checked."));
		return;
	}
	if(sortedCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a time constraint down, the 'Sorted' check box must not be checked."));
		return;
	}
	
	if(constraintsListWidget->count()<=1)
		return;
	int i=constraintsListWidget->currentRow();
	if(i<0 || i>=constraintsListWidget->count())
		return;
	if(i==constraintsListWidget->count()-1)
		return;
		
	QString s1=constraintsListWidget->item(i)->text();
	QString s2=constraintsListWidget->item(i+1)->text();
	
	assert(gt.rules.timeConstraintsList.count()==visibleTimeConstraintsList.count());
	TimeConstraint* tc1=gt.rules.timeConstraintsList.at(i);
	assert(tc1==visibleTimeConstraintsList.at(i));
	TimeConstraint* tc2=gt.rules.timeConstraintsList.at(i+1);
	assert(tc2==visibleTimeConstraintsList.at(i+1));
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	constraintsListWidget->item(i)->setText(s2);
	constraintsListWidget->item(i+1)->setText(s1);
	
	gt.rules.timeConstraintsList[i]=tc2;
	gt.rules.timeConstraintsList[i+1]=tc1;
	
	visibleTimeConstraintsList[i]=tc2;
	visibleTimeConstraintsList[i+1]=tc1;
	
	gt.rules.addUndoPoint(tr("A constraint was moved down:\n\n%1", "%1 is the detailed description of the constraint").arg(tc1->getDetailedDescription(gt.rules)));
	
	if(true || USE_GUI_COLORS){
		if(tc2->active)
			constraintsListWidget->item(i)->setBackground(constraintsListWidget->palette().base());
		else
			constraintsListWidget->item(i)->setBackground(constraintsListWidget->palette().alternateBase());

		if(tc1->active)
			constraintsListWidget->item(i+1)->setBackground(constraintsListWidget->palette().base());
		else
			constraintsListWidget->item(i+1)->setBackground(constraintsListWidget->palette().alternateBase());
	}

	constraintsListWidget->setCurrentRow(i+1);
	constraintChanged(/*i+1*/);
}

void AllTimeConstraintsForm::sortedChanged(bool checked)
{
	Q_UNUSED(checked);

	filterChanged();
	
	constraintsListWidget->setFocus();
}

int timeConstraintsAscendingByDescription(TimeConstraint* t1, TimeConstraint* t2)
{
	//return t1->getDescription(gt.rules) < t2->getDescription(gt.rules);
	
	//by Rodolfo Ribeiro Gomes
	return t1->getDescription(gt.rules).localeAwareCompare(t2->getDescription(gt.rules))<0;
}

void AllTimeConstraintsForm::filterChanged()
{
	disconnect(constraintsListWidget, &QListWidget::itemSelectionChanged, this, &AllTimeConstraintsForm::selectionChanged);

	visibleTimeConstraintsList.clear();
	constraintsListWidget->clear();
	int n_active=0;
	for(TimeConstraint* ctr : std::as_const(gt.rules.timeConstraintsList))
		if(filterOk(ctr))
			visibleTimeConstraintsList.append(ctr);
		
	if(sortedCheckBox->isChecked())
		std::stable_sort(visibleTimeConstraintsList.begin(), visibleTimeConstraintsList.end(), timeConstraintsAscendingByDescription);

	for(TimeConstraint* ctr : std::as_const(visibleTimeConstraintsList)){
		assert(filterOk(ctr));
		constraintsListWidget->addItem(ctr->getDescription(gt.rules));
		
		if(ctr->active)
			n_active++;
		else if(true || USE_GUI_COLORS)
			constraintsListWidget->item(constraintsListWidget->count()-1)->setBackground(constraintsListWidget->palette().alternateBase());
	}

	if(constraintsListWidget->count()<=0)
		currentConstraintTextEdit->setPlainText("");
	else
		constraintsListWidget->setCurrentRow(0);
	
	constraintsTextLabel->setText(tr("No: %1 / %2",
	 "%1 represents the number of visible active time constraints, %2 represents the total number of visible time constraints")
	 .arg(n_active).arg(visibleTimeConstraintsList.count()));
	//mSLabel->setText(tr("Multiple selection", "The list can have multiple selection. Keep translation short."));
	
	selectionChanged();
	connect(constraintsListWidget, &QListWidget::itemSelectionChanged, this, &AllTimeConstraintsForm::selectionChanged);
}

void AllTimeConstraintsForm::constraintChanged()
{
	int index=constraintsListWidget->currentRow();

	if(index<0)
		return;
		
	assert(index<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(index);
	assert(ctr!=nullptr);
	QString s=ctr->getDetailedDescription(gt.rules);
	currentConstraintTextEdit->setPlainText(s);
}

void AllTimeConstraintsForm::modifyConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
	
		constraintsListWidget->setFocus();

		return;
	}
	
	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();
	
	assert(i<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(i);

	//185
	if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_DAY){
		QMessageBox::warning(this, tr("FET warning"), tr("The constraints of type activity preferred day cannot be edited. They can only be added/removed"
		 " from the students/teachers timetable view time horizontal dialog or they can be removed from this dialog."));
		return;
	}

	AddOrModifyTimeConstraint(this, ctr->type, ctr);

	/*switch(ctr->type){
		//1
		case CONSTRAINT_BASIC_COMPULSORY_TIME:
			{
				ModifyConstraintBasicCompulsoryTimeForm form(this, (ConstraintBasicCompulsoryTime*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//2
		case CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE:
			{
				ModifyConstraintTwoActivitiesConsecutiveForm form(this, (ConstraintTwoActivitiesConsecutive*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//3
		case CONSTRAINT_TWO_ACTIVITIES_ORDERED:
			{
				ModifyConstraintTwoActivitiesOrderedForm form(this, (ConstraintTwoActivitiesOrdered*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//4
		case CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS:
			{
				ModifyConstraintActivityPreferredTimeSlotsForm form(this, (ConstraintActivityPreferredTimeSlots*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//5
		case CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES:
			{
				ModifyConstraintActivityPreferredStartingTimesForm form(this, (ConstraintActivityPreferredStartingTimes*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//6
		case CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS:
			{
				ModifyConstraintActivitiesPreferredTimeSlotsForm form(this, (ConstraintActivitiesPreferredTimeSlots*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//7
		case CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES:
			{
				ModifyConstraintActivitiesPreferredStartingTimesForm form(this, (ConstraintActivitiesPreferredStartingTimes*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//8
		case CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS:
			{
				ModifyConstraintSubactivitiesPreferredTimeSlotsForm form(this, (ConstraintSubactivitiesPreferredTimeSlots*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//9
		case CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES:
			{
				ModifyConstraintSubactivitiesPreferredStartingTimesForm form(this, (ConstraintSubactivitiesPreferredStartingTimes*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//10
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME:
			{
				ModifyConstraintActivitiesSameStartingTimeForm form(this, (ConstraintActivitiesSameStartingTime*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//11
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR:
			{
				ModifyConstraintActivitiesSameStartingHourForm form(this, (ConstraintActivitiesSameStartingHour*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//12
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY:
			{
				ModifyConstraintActivitiesSameStartingDayForm form(this, (ConstraintActivitiesSameStartingDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//13
		case CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES:
			{
				ModifyConstraintTeacherNotAvailableTimesForm form(this, (ConstraintTeacherNotAvailableTimes*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//14
		case CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES:
			{
				ModifyConstraintStudentsSetNotAvailableTimesForm form(this, (ConstraintStudentsSetNotAvailableTimes*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//15
		case CONSTRAINT_BREAK_TIMES:
			{
				ModifyConstraintBreakTimesForm form(this, (ConstraintBreakTimes*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//16
		case CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK:
			{
				ModifyConstraintTeacherMaxDaysPerWeekForm form(this, (ConstraintTeacherMaxDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//17
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY:
			{
				ModifyConstraintTeachersMaxHoursDailyForm form(this, (ConstraintTeachersMaxHoursDaily*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//18
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY:
			{
				ModifyConstraintTeacherMaxHoursDailyForm form(this, (ConstraintTeacherMaxHoursDaily*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//19
		case CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY:
			{
				ModifyConstraintTeachersMaxHoursContinuouslyForm form(this, (ConstraintTeachersMaxHoursContinuously*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//20
		case CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY:
			{
				ModifyConstraintTeacherMaxHoursContinuouslyForm form(this, (ConstraintTeacherMaxHoursContinuously*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//21
		case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY:
			{
				ModifyConstraintTeachersMinHoursDailyForm form(this, (ConstraintTeachersMinHoursDaily*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//22
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY:
			{
				ModifyConstraintTeacherMinHoursDailyForm form(this, (ConstraintTeacherMinHoursDaily*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//23
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK:
			{
				ModifyConstraintTeachersMaxGapsPerWeekForm form(this, (ConstraintTeachersMaxGapsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//24
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK:
			{
				ModifyConstraintTeacherMaxGapsPerWeekForm form(this, (ConstraintTeacherMaxGapsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//25
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY:
			{
				ModifyConstraintTeachersMaxGapsPerDayForm form(this, (ConstraintTeachersMaxGapsPerDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//26
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY:
			{
				ModifyConstraintTeacherMaxGapsPerDayForm form(this, (ConstraintTeacherMaxGapsPerDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//27
		case CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME:
			{
				ModifyConstraintActivityPreferredStartingTimeForm form(this, (ConstraintActivityPreferredStartingTime*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//28
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK:
			{
				ModifyConstraintStudentsSetMaxGapsPerWeekForm form(this, (ConstraintStudentsSetMaxGapsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//29
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK:
			{
				ModifyConstraintStudentsMaxGapsPerWeekForm form(this, (ConstraintStudentsMaxGapsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//30
		case CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ModifyConstraintStudentsEarlyMaxBeginningsAtSecondHourForm form(this, (ConstraintStudentsEarlyMaxBeginningsAtSecondHour*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//31
		case CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm form(this, (ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//32
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY:
			{
				ModifyConstraintStudentsSetMaxHoursDailyForm form(this, (ConstraintStudentsSetMaxHoursDaily*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//33
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY:
			{
				ModifyConstraintStudentsMaxHoursDailyForm form(this, (ConstraintStudentsMaxHoursDaily*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//34
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY:
			{
				ModifyConstraintStudentsSetMaxHoursContinuouslyForm form(this, (ConstraintStudentsSetMaxHoursContinuously*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//35
		case CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY:
			{
				ModifyConstraintStudentsMaxHoursContinuouslyForm form(this, (ConstraintStudentsMaxHoursContinuously*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//36
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY:
			{
				ModifyConstraintStudentsSetMinHoursDailyForm form(this, (ConstraintStudentsSetMinHoursDaily*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//37
		case CONSTRAINT_STUDENTS_MIN_HOURS_DAILY:
			{
				ModifyConstraintStudentsMinHoursDailyForm form(this, (ConstraintStudentsMinHoursDaily*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//38
		case CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING:
			{
				ModifyConstraintActivitiesNotOverlappingForm form(this, (ConstraintActivitiesNotOverlapping*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//39
		case CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES:
			{
				ModifyConstraintMinDaysBetweenActivitiesForm form(this, (ConstraintMinDaysBetweenActivities*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//40
		case CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES:
			{
				ModifyConstraintMinGapsBetweenActivitiesForm form(this, (ConstraintMinGapsBetweenActivities*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//41
		case CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY:
			{
				ModifyConstraintActivityEndsStudentsDayForm form(this, (ConstraintActivityEndsStudentsDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//42
		case CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				ModifyConstraintTeacherIntervalMaxDaysPerWeekForm form(this, (ConstraintTeacherIntervalMaxDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//43
		case CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				ModifyConstraintTeachersIntervalMaxDaysPerWeekForm form(this, (ConstraintTeachersIntervalMaxDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//44
		case CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				ModifyConstraintStudentsSetIntervalMaxDaysPerWeekForm form(this, (ConstraintStudentsSetIntervalMaxDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//45
		case CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				ModifyConstraintStudentsIntervalMaxDaysPerWeekForm form(this, (ConstraintStudentsIntervalMaxDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//46
		case CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY:
			{
				ModifyConstraintActivitiesEndStudentsDayForm form(this, (ConstraintActivitiesEndStudentsDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//47
		case CONSTRAINT_TWO_ACTIVITIES_GROUPED:
			{
				ModifyConstraintTwoActivitiesGroupedForm form(this, (ConstraintTwoActivitiesGrouped*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//48
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				ModifyConstraintTeachersActivityTagMaxHoursContinuouslyForm form(this, (ConstraintTeachersActivityTagMaxHoursContinuously*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//49
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				ModifyConstraintTeacherActivityTagMaxHoursContinuouslyForm form(this, (ConstraintTeacherActivityTagMaxHoursContinuously*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//50
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				ModifyConstraintStudentsActivityTagMaxHoursContinuouslyForm form(this, (ConstraintStudentsActivityTagMaxHoursContinuously*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//51
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				ModifyConstraintStudentsSetActivityTagMaxHoursContinuouslyForm form(this, (ConstraintStudentsSetActivityTagMaxHoursContinuously*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//52
		case CONSTRAINT_TEACHERS_MAX_DAYS_PER_WEEK:
			{
				ModifyConstraintTeachersMaxDaysPerWeekForm form(this, (ConstraintTeachersMaxDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//53
		case CONSTRAINT_THREE_ACTIVITIES_GROUPED:
			{
				ModifyConstraintThreeActivitiesGroupedForm form(this, (ConstraintThreeActivitiesGrouped*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//54
		case CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES:
			{
				ModifyConstraintMaxDaysBetweenActivitiesForm form(this, (ConstraintMaxDaysBetweenActivities*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//55
		case CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK:
			{
				ModifyConstraintTeachersMinDaysPerWeekForm form(this, (ConstraintTeachersMinDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//56
		case CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK:
			{
				ModifyConstraintTeacherMinDaysPerWeekForm form(this, (ConstraintTeacherMinDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//57
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				ModifyConstraintTeachersActivityTagMaxHoursDailyForm form(this, (ConstraintTeachersActivityTagMaxHoursDaily*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//58
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				ModifyConstraintTeacherActivityTagMaxHoursDailyForm form(this, (ConstraintTeacherActivityTagMaxHoursDaily*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//59
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				ModifyConstraintStudentsActivityTagMaxHoursDailyForm form(this, (ConstraintStudentsActivityTagMaxHoursDaily*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//60
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				ModifyConstraintStudentsSetActivityTagMaxHoursDailyForm form(this, (ConstraintStudentsSetActivityTagMaxHoursDaily*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}

		//61
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY:
			{
				ModifyConstraintStudentsSetMaxGapsPerDayForm form(this, (ConstraintStudentsSetMaxGapsPerDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//62
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_DAY:
			{
				ModifyConstraintStudentsMaxGapsPerDayForm form(this, (ConstraintStudentsMaxGapsPerDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//63
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TIME_SLOTS_FROM_SELECTION:
			{
				ModifyConstraintActivitiesOccupyMaxTimeSlotsFromSelectionForm form(this, (ConstraintActivitiesOccupyMaxTimeSlotsFromSelection*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//64
		case CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS:
			{
				ModifyConstraintActivitiesMaxSimultaneousInSelectedTimeSlotsForm form(this, (ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//65
		case CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK:
			{
				ModifyConstraintStudentsSetMaxDaysPerWeekForm form(this, (ConstraintStudentsSetMaxDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//66
		case CONSTRAINT_STUDENTS_MAX_DAYS_PER_WEEK:
			{
				ModifyConstraintStudentsMaxDaysPerWeekForm form(this, (ConstraintStudentsMaxDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//2017-02-07
		//67
		case CONSTRAINT_TEACHER_MAX_SPAN_PER_DAY:
			{
				ModifyConstraintTeacherMaxSpanPerDayForm form(this, (ConstraintTeacherMaxSpanPerDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//68
		case CONSTRAINT_TEACHERS_MAX_SPAN_PER_DAY:
			{
				ModifyConstraintTeachersMaxSpanPerDayForm form(this, (ConstraintTeachersMaxSpanPerDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//69
		case CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_DAY:
			{
				ModifyConstraintStudentsSetMaxSpanPerDayForm form(this, (ConstraintStudentsSetMaxSpanPerDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//70
		case CONSTRAINT_STUDENTS_MAX_SPAN_PER_DAY:
			{
				ModifyConstraintStudentsMaxSpanPerDayForm form(this, (ConstraintStudentsMaxSpanPerDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//71
		case CONSTRAINT_TEACHER_MIN_RESTING_HOURS:
			{
				ModifyConstraintTeacherMinRestingHoursForm form(this, (ConstraintTeacherMinRestingHours*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//72
		case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS:
			{
				ModifyConstraintTeachersMinRestingHoursForm form(this, (ConstraintTeachersMinRestingHours*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//73
		case CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS:
			{
				ModifyConstraintStudentsSetMinRestingHoursForm form(this, (ConstraintStudentsSetMinRestingHours*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//74
		case CONSTRAINT_STUDENTS_MIN_RESTING_HOURS:
			{
				ModifyConstraintStudentsMinRestingHoursForm form(this, (ConstraintStudentsMinRestingHours*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//2018-06-13
		//75
		case CONSTRAINT_TWO_ACTIVITIES_ORDERED_IF_SAME_DAY:
			{
				ModifyConstraintTwoActivitiesOrderedIfSameDayForm form(this, (ConstraintTwoActivitiesOrderedIfSameDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//2019-06-09
		//76
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				ModifyConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsForm form(this, (ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//77
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				ModifyConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsForm form(this, (ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//78
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				ModifyConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsForm form(this, (ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//79
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsForm form(this, (ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//80
		case CONSTRAINT_ACTIVITY_TAGS_NOT_OVERLAPPING:
			{
				ModifyConstraintActivityTagsNotOverlappingForm form(this, (ConstraintActivityTagsNotOverlapping*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//81
		case CONSTRAINT_ACTIVITIES_OCCUPY_MIN_TIME_SLOTS_FROM_SELECTION:
			{
				ModifyConstraintActivitiesOccupyMinTimeSlotsFromSelectionForm form(this, (ConstraintActivitiesOccupyMinTimeSlotsFromSelection*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//82
		case CONSTRAINT_ACTIVITIES_MIN_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS:
			{
				ModifyConstraintActivitiesMinSimultaneousInSelectedTimeSlotsForm form(this, (ConstraintActivitiesMinSimultaneousInSelectedTimeSlots*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}

		//83
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				ModifyConstraintTeachersActivityTagMinHoursDailyForm form(this, (ConstraintTeachersActivityTagMinHoursDaily*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//84
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				ModifyConstraintTeacherActivityTagMinHoursDailyForm form(this, (ConstraintTeacherActivityTagMinHoursDaily*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//85
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				ModifyConstraintStudentsActivityTagMinHoursDailyForm form(this, (ConstraintStudentsActivityTagMinHoursDaily*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//86
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				ModifyConstraintStudentsSetActivityTagMinHoursDailyForm form(this, (ConstraintStudentsSetActivityTagMinHoursDaily*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//87
		case CONSTRAINT_ACTIVITY_ENDS_TEACHERS_DAY:
			{
				ModifyConstraintActivityEndsTeachersDayForm form(this, (ConstraintActivityEndsTeachersDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//88
		case CONSTRAINT_ACTIVITIES_END_TEACHERS_DAY:
			{
				ModifyConstraintActivitiesEndTeachersDayForm form(this, (ConstraintActivitiesEndTeachersDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}

		//mornings-afternoons
		//89
		case CONSTRAINT_TEACHER_MAX_AFTERNOONS_PER_WEEK:
			{
				ModifyConstraintTeacherMaxAfternoonsPerWeekForm form(this, (ConstraintTeacherMaxAfternoonsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//90
		case CONSTRAINT_TEACHERS_MAX_AFTERNOONS_PER_WEEK:
			{
				ModifyConstraintTeachersMaxAfternoonsPerWeekForm form(this, (ConstraintTeachersMaxAfternoonsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//91
		case CONSTRAINT_TEACHER_MAX_MORNINGS_PER_WEEK:
			{
				ModifyConstraintTeacherMaxMorningsPerWeekForm form(this, (ConstraintTeacherMaxMorningsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//92
		case CONSTRAINT_TEACHERS_MAX_MORNINGS_PER_WEEK:
			{
				ModifyConstraintTeachersMaxMorningsPerWeekForm form(this, (ConstraintTeachersMaxMorningsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//93
		case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm form(this, (ConstraintTeacherMaxActivityTagsPerDayFromSet*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//94
		case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				ModifyConstraintTeachersMaxActivityTagsPerDayFromSetForm form(this, (ConstraintTeachersMaxActivityTagsPerDayFromSet*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//95
		case CONSTRAINT_TEACHER_MIN_MORNINGS_PER_WEEK:
			{
				ModifyConstraintTeacherMinMorningsPerWeekForm form(this, (ConstraintTeacherMinMorningsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//96
		case CONSTRAINT_TEACHERS_MIN_MORNINGS_PER_WEEK:
			{
				ModifyConstraintTeachersMinMorningsPerWeekForm form(this, (ConstraintTeachersMinMorningsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//97
		case CONSTRAINT_TEACHER_MIN_AFTERNOONS_PER_WEEK:
			{
				ModifyConstraintTeacherMinAfternoonsPerWeekForm form(this, (ConstraintTeacherMinAfternoonsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//98
		case CONSTRAINT_TEACHERS_MIN_AFTERNOONS_PER_WEEK:
			{
				ModifyConstraintTeachersMinAfternoonsPerWeekForm form(this, (ConstraintTeachersMinAfternoonsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//99
		case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_MORNINGS:
			{
				ModifyConstraintTeacherMaxTwoConsecutiveMorningsForm form(this, (ConstraintTeacherMaxTwoConsecutiveMornings*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//100
		case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_MORNINGS:
			{
				ModifyConstraintTeachersMaxTwoConsecutiveMorningsForm form(this, (ConstraintTeachersMaxTwoConsecutiveMornings*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//101
		case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_AFTERNOONS:
			{
				ModifyConstraintTeacherMaxTwoConsecutiveAfternoonsForm form(this, (ConstraintTeacherMaxTwoConsecutiveAfternoons*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//102
		case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_AFTERNOONS:
			{
				ModifyConstraintTeachersMaxTwoConsecutiveAfternoonsForm form(this, (ConstraintTeachersMaxTwoConsecutiveAfternoons*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//103
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_REAL_DAY:
			{
				ModifyConstraintTeachersMaxGapsPerRealDayForm form(this, (ConstraintTeachersMaxGapsPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//104
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY:
			{
				ModifyConstraintTeacherMaxGapsPerRealDayForm form(this, (ConstraintTeacherMaxGapsPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//105
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_REAL_DAY:
			{
				ModifyConstraintStudentsSetMaxGapsPerRealDayForm form(this, (ConstraintStudentsSetMaxGapsPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//106
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_REAL_DAY:
			{
				ModifyConstraintStudentsMaxGapsPerRealDayForm form(this, (ConstraintStudentsMaxGapsPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}

		//107
		case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY_REAL_DAYS:
			{
				ModifyConstraintTeachersMinHoursDailyRealDaysForm form(this, (ConstraintTeachersMinHoursDailyRealDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//108
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS:
			{
				ModifyConstraintTeacherMinHoursDailyRealDaysForm form(this, (ConstraintTeacherMinHoursDailyRealDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}

		//109
		case CONSTRAINT_TEACHERS_MIN_HOURS_PER_MORNING:
			{
				ModifyConstraintTeachersMinHoursPerMorningForm form(this, (ConstraintTeachersMinHoursPerMorning*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//110
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING:
			{
				ModifyConstraintTeacherMinHoursPerMorningForm form(this, (ConstraintTeacherMinHoursPerMorning*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//2017-02-07
		//111
		case CONSTRAINT_TEACHER_MAX_SPAN_PER_REAL_DAY:
			{
				ModifyConstraintTeacherMaxSpanPerRealDayForm form(this, (ConstraintTeacherMaxSpanPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//112
		case CONSTRAINT_TEACHERS_MAX_SPAN_PER_REAL_DAY:
			{
				ModifyConstraintTeachersMaxSpanPerRealDayForm form(this, (ConstraintTeachersMaxSpanPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//113
		case CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_REAL_DAY:
			{
				ModifyConstraintStudentsSetMaxSpanPerRealDayForm form(this, (ConstraintStudentsSetMaxSpanPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//114
		case CONSTRAINT_STUDENTS_MAX_SPAN_PER_REAL_DAY:
			{
				ModifyConstraintStudentsMaxSpanPerRealDayForm form(this, (ConstraintStudentsMaxSpanPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}

		//115
		case CONSTRAINT_TEACHER_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				ModifyConstraintTeacherMorningIntervalMaxDaysPerWeekForm form(this, (ConstraintTeacherMorningIntervalMaxDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//116
		case CONSTRAINT_TEACHERS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				ModifyConstraintTeachersMorningIntervalMaxDaysPerWeekForm form(this, (ConstraintTeachersMorningIntervalMaxDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}

		//117
		case CONSTRAINT_TEACHER_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				ModifyConstraintTeacherAfternoonIntervalMaxDaysPerWeekForm form(this, (ConstraintTeacherAfternoonIntervalMaxDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//118
		case CONSTRAINT_TEACHERS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				ModifyConstraintTeachersAfternoonIntervalMaxDaysPerWeekForm form(this, (ConstraintTeachersAfternoonIntervalMaxDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//119
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_MORNING:
			{
				ModifyConstraintStudentsSetMinHoursPerMorningForm form(this, (ConstraintStudentsSetMinHoursPerMorning*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//120
		case CONSTRAINT_STUDENTS_MIN_HOURS_PER_MORNING:
			{
				ModifyConstraintStudentsMinHoursPerMorningForm form(this, (ConstraintStudentsMinHoursPerMorning*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}

		//121
		case CONSTRAINT_TEACHERS_MAX_ZERO_GAPS_PER_AFTERNOON:
			{
				ModifyConstraintTeachersMaxZeroGapsPerAfternoonForm form(this, (ConstraintTeachersMaxZeroGapsPerAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//122
		case CONSTRAINT_TEACHER_MAX_ZERO_GAPS_PER_AFTERNOON:
			{
				ModifyConstraintTeacherMaxZeroGapsPerAfternoonForm form(this, (ConstraintTeacherMaxZeroGapsPerAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//123
		case CONSTRAINT_STUDENTS_SET_MAX_AFTERNOONS_PER_WEEK:
			{
				ModifyConstraintStudentsSetMaxAfternoonsPerWeekForm form(this, (ConstraintStudentsSetMaxAfternoonsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//124
		case CONSTRAINT_STUDENTS_MAX_AFTERNOONS_PER_WEEK:
			{
				ModifyConstraintStudentsMaxAfternoonsPerWeekForm form(this, (ConstraintStudentsMaxAfternoonsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//125
		case CONSTRAINT_STUDENTS_SET_MAX_MORNINGS_PER_WEEK:
			{
				ModifyConstraintStudentsSetMaxMorningsPerWeekForm form(this, (ConstraintStudentsSetMaxMorningsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//126
		case CONSTRAINT_STUDENTS_MAX_MORNINGS_PER_WEEK:
			{
				ModifyConstraintStudentsMaxMorningsPerWeekForm form(this, (ConstraintStudentsMaxMorningsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		/////
		//127
		case CONSTRAINT_STUDENTS_SET_MIN_AFTERNOONS_PER_WEEK:
			{
				ModifyConstraintStudentsSetMinAfternoonsPerWeekForm form(this, (ConstraintStudentsSetMinAfternoonsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//128
		case CONSTRAINT_STUDENTS_MIN_AFTERNOONS_PER_WEEK:
			{
				ModifyConstraintStudentsMinAfternoonsPerWeekForm form(this, (ConstraintStudentsMinAfternoonsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//129
		case CONSTRAINT_STUDENTS_SET_MIN_MORNINGS_PER_WEEK:
			{
				ModifyConstraintStudentsSetMinMorningsPerWeekForm form(this, (ConstraintStudentsSetMinMorningsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//130
		case CONSTRAINT_STUDENTS_MIN_MORNINGS_PER_WEEK:
			{
				ModifyConstraintStudentsMinMorningsPerWeekForm form(this, (ConstraintStudentsMinMorningsPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//131
		case CONSTRAINT_STUDENTS_SET_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				ModifyConstraintStudentsSetMorningIntervalMaxDaysPerWeekForm form(this, (ConstraintStudentsSetMorningIntervalMaxDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//132
		case CONSTRAINT_STUDENTS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				ModifyConstraintStudentsMorningIntervalMaxDaysPerWeekForm form(this, (ConstraintStudentsMorningIntervalMaxDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//133
		case CONSTRAINT_STUDENTS_SET_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				ModifyConstraintStudentsSetAfternoonIntervalMaxDaysPerWeekForm form(this, (ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//134
		case CONSTRAINT_STUDENTS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				ModifyConstraintStudentsAfternoonIntervalMaxDaysPerWeekForm form(this, (ConstraintStudentsAfternoonIntervalMaxDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//2020-06-28
		//135
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				ModifyConstraintTeacherMaxHoursPerAllAfternoonsForm form(this, (ConstraintTeacherMaxHoursPerAllAfternoons*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//136
		case CONSTRAINT_TEACHERS_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				ModifyConstraintTeachersMaxHoursPerAllAfternoonsForm form(this, (ConstraintTeachersMaxHoursPerAllAfternoons*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//137
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				ModifyConstraintStudentsSetMaxHoursPerAllAfternoonsForm form(this, (ConstraintStudentsSetMaxHoursPerAllAfternoons*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//138
		case CONSTRAINT_STUDENTS_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				ModifyConstraintStudentsMaxHoursPerAllAfternoonsForm form(this, (ConstraintStudentsMaxHoursPerAllAfternoons*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}

		//139
		case CONSTRAINT_TEACHER_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ModifyConstraintTeacherMinRestingHoursBetweenMorningAndAfternoonForm form(this, (ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//140
		case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ModifyConstraintTeachersMinRestingHoursBetweenMorningAndAfternoonForm form(this, (ConstraintTeachersMinRestingHoursBetweenMorningAndAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//141
		case CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ModifyConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoonForm form(this, (ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//142
		case CONSTRAINT_STUDENTS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ModifyConstraintStudentsMinRestingHoursBetweenMorningAndAfternoonForm form(this, (ConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		/////////
		//143
		case CONSTRAINT_STUDENTS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ModifyConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHourForm form(this, (ConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHour*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//144
		case CONSTRAINT_STUDENTS_SET_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ModifyConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourForm form(this, (ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}

		//145
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				ModifyConstraintTeachersMaxGapsPerWeekForRealDaysForm form(this, (ConstraintTeachersMaxGapsPerWeekForRealDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//146
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				ModifyConstraintTeacherMaxGapsPerWeekForRealDaysForm form(this, (ConstraintTeacherMaxGapsPerWeekForRealDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//147
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				ModifyConstraintStudentsSetMaxGapsPerWeekForRealDaysForm form(this, (ConstraintStudentsSetMaxGapsPerWeekForRealDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//148
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				ModifyConstraintStudentsMaxGapsPerWeekForRealDaysForm form(this, (ConstraintStudentsMaxGapsPerWeekForRealDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
	
		//149
		case CONSTRAINT_TEACHER_MAX_REAL_DAYS_PER_WEEK:
			{
				ModifyConstraintTeacherMaxRealDaysPerWeekForm form(this, (ConstraintTeacherMaxRealDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//150
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ModifyConstraintTeachersMaxHoursDailyRealDaysForm form(this, (ConstraintTeachersMaxHoursDailyRealDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//151
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ModifyConstraintTeacherMaxHoursDailyRealDaysForm form(this, (ConstraintTeacherMaxHoursDailyRealDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//152
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ModifyConstraintStudentsSetMaxHoursDailyRealDaysForm form(this, (ConstraintStudentsSetMaxHoursDailyRealDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//153
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ModifyConstraintStudentsMaxHoursDailyRealDaysForm form(this, (ConstraintStudentsMaxHoursDailyRealDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//154
		case CONSTRAINT_TEACHERS_MAX_REAL_DAYS_PER_WEEK:
			{
				ModifyConstraintTeachersMaxRealDaysPerWeekForm form(this, (ConstraintTeachersMaxRealDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//155
		case CONSTRAINT_TEACHERS_MIN_REAL_DAYS_PER_WEEK:
			{
				ModifyConstraintTeachersMinRealDaysPerWeekForm form(this, (ConstraintTeachersMinRealDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//156
		case CONSTRAINT_TEACHER_MIN_REAL_DAYS_PER_WEEK:
			{
				ModifyConstraintTeacherMinRealDaysPerWeekForm form(this, (ConstraintTeacherMinRealDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//157
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ModifyConstraintTeachersActivityTagMaxHoursDailyRealDaysForm form(this, (ConstraintTeachersActivityTagMaxHoursDailyRealDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//158
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ModifyConstraintTeacherActivityTagMaxHoursDailyRealDaysForm form(this, (ConstraintTeacherActivityTagMaxHoursDailyRealDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//159
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ModifyConstraintStudentsActivityTagMaxHoursDailyRealDaysForm form(this, (ConstraintStudentsActivityTagMaxHoursDailyRealDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//160
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ModifyConstraintStudentsSetActivityTagMaxHoursDailyRealDaysForm form(this, (ConstraintStudentsSetActivityTagMaxHoursDailyRealDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}

		//161
		case CONSTRAINT_STUDENTS_SET_MAX_REAL_DAYS_PER_WEEK:
			{
				ModifyConstraintStudentsSetMaxRealDaysPerWeekForm form(this, (ConstraintStudentsSetMaxRealDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//162
		case CONSTRAINT_STUDENTS_MAX_REAL_DAYS_PER_WEEK:
			{
				ModifyConstraintStudentsMaxRealDaysPerWeekForm form(this, (ConstraintStudentsMaxRealDaysPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//163
		case CONSTRAINT_TEACHERS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ModifyConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHourForm form(this, (ConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHour*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//164
		case CONSTRAINT_TEACHER_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ModifyConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHourForm form(this, (ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//block-planning
		//165
		case CONSTRAINT_MAX_TOTAL_ACTIVITIES_FROM_SET_IN_SELECTED_TIME_SLOTS:
			{
				ModifyConstraintMaxTotalActivitiesFromSetInSelectedTimeSlotsForm form(this, (ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//166
		case CONSTRAINT_MAX_GAPS_BETWEEN_ACTIVITIES:
			{
				ModifyConstraintMaxGapsBetweenActivitiesForm form(this, (ConstraintMaxGapsBetweenActivities*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//terms
		//167
		case CONSTRAINT_ACTIVITIES_MAX_IN_A_TERM:
			{
				ModifyConstraintActivitiesMaxInATermForm form(this, (ConstraintActivitiesMaxInATerm*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//168
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TERMS:
			{
				ModifyConstraintActivitiesOccupyMaxTermsForm form(this, (ConstraintActivitiesOccupyMaxTerms*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//169
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			{
				ModifyConstraintTeachersMaxGapsPerMorningAndAfternoonForm form(this, (ConstraintTeachersMaxGapsPerMorningAndAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//170
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			{
				ModifyConstraintTeacherMaxGapsPerMorningAndAfternoonForm form(this, (ConstraintTeacherMaxGapsPerMorningAndAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//171
		case CONSTRAINT_STUDENTS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ModifyConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHourForm form(this, (ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//172
		case CONSTRAINT_STUDENTS_SET_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ModifyConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHourForm form(this, (ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//173
		case CONSTRAINT_TEACHERS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ModifyConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHourForm form(this, (ConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHour*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//174
		case CONSTRAINT_TEACHER_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ModifyConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHourForm form(this, (ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//175
		case CONSTRAINT_TWO_SETS_OF_ACTIVITIES_ORDERED:
			{
				ModifyConstraintTwoSetsOfActivitiesOrderedForm form(this, (ConstraintTwoSetsOfActivitiesOrdered*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//176
		case CONSTRAINT_TEACHER_MAX_THREE_CONSECUTIVE_DAYS:
			{
				ModifyConstraintTeacherMaxThreeConsecutiveDaysForm form(this, (ConstraintTeacherMaxThreeConsecutiveDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//177
		case CONSTRAINT_TEACHERS_MAX_THREE_CONSECUTIVE_DAYS:
			{
				ModifyConstraintTeachersMaxThreeConsecutiveDaysForm form(this, (ConstraintTeachersMaxThreeConsecutiveDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//2021-12-15
		//178
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				ModifyConstraintStudentsSetMinGapsBetweenActivityTagForm form(this, (ConstraintStudentsSetMinGapsBetweenActivityTag*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//179
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				ModifyConstraintStudentsMinGapsBetweenActivityTagForm form(this, (ConstraintStudentsMinGapsBetweenActivityTag*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//180
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				ModifyConstraintTeacherMinGapsBetweenActivityTagForm form(this, (ConstraintTeacherMinGapsBetweenActivityTag*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//181
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				ModifyConstraintTeachersMinGapsBetweenActivityTagForm form(this, (ConstraintTeachersMinGapsBetweenActivityTag*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//2022-02-15
		//182
		case CONSTRAINT_STUDENTS_SET_MAX_THREE_CONSECUTIVE_DAYS:
			{
				ModifyConstraintStudentsSetMaxThreeConsecutiveDaysForm form(this, (ConstraintStudentsSetMaxThreeConsecutiveDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//183
		case CONSTRAINT_STUDENTS_MAX_THREE_CONSECUTIVE_DAYS:
			{
				ModifyConstraintStudentsMaxThreeConsecutiveDaysForm form(this, (ConstraintStudentsMaxThreeConsecutiveDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//184
		case CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES:
			{
				ModifyConstraintMinHalfDaysBetweenActivitiesForm form(this, (ConstraintMinHalfDaysBetweenActivities*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//185
		case CONSTRAINT_ACTIVITY_PREFERRED_DAY:
			QMessageBox::warning(this, tr("FET warning"), tr("The constraints of type activity preferred day cannot be edited. They can only be added/removed"
			 " from the students/teachers timetable view time horizontal dialog or they can be removed from this dialog."));
			break;
		//186
		case CONSTRAINT_ACTIVITIES_MIN_IN_A_TERM:
			{
				ModifyConstraintActivitiesMinInATermForm form(this, (ConstraintActivitiesMinInATerm*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//187
		case CONSTRAINT_MAX_TERMS_BETWEEN_ACTIVITIES:
			{
				ModifyConstraintMaxTermsBetweenActivitiesForm form(this, (ConstraintMaxTermsBetweenActivities*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//188
		case CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				ModifyConstraintStudentsSetMaxActivityTagsPerDayFromSetForm form(this, (ConstraintStudentsSetMaxActivityTagsPerDayFromSet*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//189
		case CONSTRAINT_STUDENTS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				ModifyConstraintStudentsMaxActivityTagsPerDayFromSetForm form(this, (ConstraintStudentsMaxActivityTagsPerDayFromSet*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//190
		case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				ModifyConstraintTeacherMaxActivityTagsPerRealDayFromSetForm form(this, (ConstraintTeacherMaxActivityTagsPerRealDayFromSet*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//191
		case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				ModifyConstraintTeachersMaxActivityTagsPerRealDayFromSetForm form(this, (ConstraintTeachersMaxActivityTagsPerRealDayFromSet*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//192
		case CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				ModifyConstraintStudentsSetMaxActivityTagsPerRealDayFromSetForm form(this, (ConstraintStudentsSetMaxActivityTagsPerRealDayFromSet*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//193
		case CONSTRAINT_STUDENTS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				ModifyConstraintStudentsMaxActivityTagsPerRealDayFromSetForm form(this, (ConstraintStudentsMaxActivityTagsPerRealDayFromSet*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//194
		case CONSTRAINT_MAX_HALF_DAYS_BETWEEN_ACTIVITIES:
			{
				ModifyConstraintMaxHalfDaysBetweenActivitiesForm form(this, (ConstraintMaxHalfDaysBetweenActivities*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//195
		case CONSTRAINT_ACTIVITY_BEGINS_STUDENTS_DAY:
			{
				ModifyConstraintActivityBeginsStudentsDayForm form(this, (ConstraintActivityBeginsStudentsDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//196
		case CONSTRAINT_ACTIVITIES_BEGIN_STUDENTS_DAY:
			{
				ModifyConstraintActivitiesBeginStudentsDayForm form(this, (ConstraintActivitiesBeginStudentsDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//197
		case CONSTRAINT_ACTIVITY_BEGINS_TEACHERS_DAY:
			{
				ModifyConstraintActivityBeginsTeachersDayForm form(this, (ConstraintActivityBeginsTeachersDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//198
		case CONSTRAINT_ACTIVITIES_BEGIN_TEACHERS_DAY:
			{
				ModifyConstraintActivitiesBeginTeachersDayForm form(this, (ConstraintActivitiesBeginTeachersDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}

		//199
		case CONSTRAINT_TEACHERS_MIN_HOURS_PER_AFTERNOON:
			{
				ModifyConstraintTeachersMinHoursPerAfternoonForm form(this, (ConstraintTeachersMinHoursPerAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//200
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON:
			{
				ModifyConstraintTeacherMinHoursPerAfternoonForm form(this, (ConstraintTeacherMinHoursPerAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//201
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_AFTERNOON:
			{
				ModifyConstraintStudentsSetMinHoursPerAfternoonForm form(this, (ConstraintStudentsSetMinHoursPerAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//202
		case CONSTRAINT_STUDENTS_MIN_HOURS_PER_AFTERNOON:
			{
				ModifyConstraintStudentsMinHoursPerAfternoonForm form(this, (ConstraintStudentsMinHoursPerAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}

		//203
		case CONSTRAINT_ACTIVITIES_MAX_HOURLY_SPAN:
			{
				ModifyConstraintActivitiesMaxHourlySpanForm form(this, (ConstraintActivitiesMaxHourlySpan*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}

		//204
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				ModifyConstraintTeachersMaxHoursDailyInIntervalForm form(this, (ConstraintTeachersMaxHoursDailyInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//205
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				ModifyConstraintTeacherMaxHoursDailyInIntervalForm form(this, (ConstraintTeacherMaxHoursDailyInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//206
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				ModifyConstraintStudentsMaxHoursDailyInIntervalForm form(this, (ConstraintStudentsMaxHoursDailyInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//207
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				ModifyConstraintStudentsSetMaxHoursDailyInIntervalForm form(this, (ConstraintStudentsSetMaxHoursDailyInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//2024-03-16
		//208
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				ModifyConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDayForm form(this, (ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//209
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				ModifyConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDayForm form(this, (ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//210
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				ModifyConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDayForm form(this, (ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//211
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDayForm form(this, (ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//212
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				ModifyConstraintStudentsSetMinGapsBetweenActivityTagPerRealDayForm form(this, (ConstraintStudentsSetMinGapsBetweenActivityTagPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//213
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				ModifyConstraintStudentsMinGapsBetweenActivityTagPerRealDayForm form(this, (ConstraintStudentsMinGapsBetweenActivityTagPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//214
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				ModifyConstraintTeacherMinGapsBetweenActivityTagPerRealDayForm form(this, (ConstraintTeacherMinGapsBetweenActivityTagPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//215
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				ModifyConstraintTeachersMinGapsBetweenActivityTagPerRealDayForm form(this, (ConstraintTeachersMinGapsBetweenActivityTagPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//2024-05-20
		//216
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ModifyConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm form(this, (ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//217
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ModifyConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm form(this, (ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//218
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ModifyConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm form(this, (ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//219
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonForm form(this, (ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//220
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ModifyConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm form(this, (ConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//221
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ModifyConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm form(this, (ConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//222
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ModifyConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm form(this, (ConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//223
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ModifyConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoonForm form(this, (ConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//224
		case CONSTRAINT_TEACHER_NO_TWO_CONSECUTIVE_DAYS:
			{
				ModifyConstraintTeacherNoTwoConsecutiveDaysForm form(this, (ConstraintTeacherNoTwoConsecutiveDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//225
		case CONSTRAINT_TEACHERS_NO_TWO_CONSECUTIVE_DAYS:
			{
				ModifyConstraintTeachersNoTwoConsecutiveDaysForm form(this, (ConstraintTeachersNoTwoConsecutiveDays*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}

		default:
			QMessageBox::critical(this, tr("FET critical"), tr("You have found a bug in FET. Please report it. This kind of constraint"
			 " is not correctly recognized in all time constraints dialog. FET will skip this error, so that you can continue work."
			 " Probably the constraint can be modified from the specific constraint dialog."));
			//assert(0);
			//exit(1);
			break;
	}*/
	
	filterChanged();
	
	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);

	if(i>=constraintsListWidget->count())
		i=constraintsListWidget->count()-1;

	if(i>=0)
		constraintsListWidget->setCurrentRow(i);
	
	constraintsListWidget->setFocus();
}

void AllTimeConstraintsForm::removeConstraints()
{
	bool recompute=false;

	QList<TimeConstraint*> tl;

	bool firstBasic=true;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleTimeConstraintsList.count());
			TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
			tl.append(ctr);

			if(firstBasic && ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME){
				firstBasic=false;

				QMessageBox::StandardButton wr;

				QString s=tr("Your selection contains the basic compulsory time constraint(s). Do you really want to remove all the selected "
				 "constraints, including this (these) one(s)? You cannot generate a timetable without this (these) constraint(s).");
				s+="\n\n";
				s+=tr("Note: you can add again a constraint of this type from the menu Data -> Time constraints -> "
					"Miscellaneous -> Basic compulsory time constraints.");

				wr=QMessageBox::warning(this, tr("FET warning"), s,
					QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

				if(wr==QMessageBox::No){
					constraintsListWidget->setFocus();
					return;
				}
			}
			else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
				recompute=true;
			}
		}

	QString s;
	s=tr("Remove these selected time constraints?");
	s+="\n\n";
	for(TimeConstraint* ctr : std::as_const(tl))
		s+=ctr->getDetailedDescription(gt.rules)+"\n";
	int lres=LongTextMessageBox::confirmation(this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), QString(), 0, 1 );

	if(lres!=0){
		constraintsListWidget->setFocus();
		return;
	}

	QString su;
	if(!tl.isEmpty()){
		su=tr("Removed %1 time constraints:").arg(tl.count());
		su+=QString("\n\n");
		for(TimeConstraint* ctr : std::as_const(tl))
			su+=ctr->getDetailedDescription(gt.rules)+"\n";
	}

	//The user clicked the OK button or pressed Enter

	gt.rules.removeTimeConstraints(tl);

	if(!tl.isEmpty())
		gt.rules.addUndoPoint(su);

	if(recompute){
		LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
		LockUnlock::increaseCommunicationSpinBox();
	}

	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	int cr=constraintsListWidget->currentRow();

	filterChanged();

	if(cr>=0){
		if(cr<constraintsListWidget->count())
			constraintsListWidget->setCurrentRow(cr);
		else if(constraintsListWidget->count()>0)
			constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
	}

	constraintsListWidget->verticalScrollBar()->setValue(valv);
	constraintsListWidget->horizontalScrollBar()->setValue(valh);

	constraintsListWidget->setFocus();
}

void AllTimeConstraintsForm::filter(bool active)
{
	if(!active){
		assert(useFilter==true);
		useFilter=false;
		
		filterChanged();
	
		constraintsListWidget->setFocus();

		return;
	}
	
	assert(active);
	
	filterForm=new AdvancedFilterForm(this, tr("Advanced filter for time constraints"), false, all, descrDetDescr, contains, text, caseSensitive, "AllTimeConstraintsAdvancedFilterForm");

	int t=filterForm->exec();
	
	if(t==QDialog::Accepted){
		assert(useFilter==false);
		useFilter=true;
	
		if(filterForm->allRadio->isChecked())
			all=true;
		else if(filterForm->anyRadio->isChecked())
			all=false;
		else
			assert(0);
			
		caseSensitive=filterForm->caseSensitiveCheckBox->isChecked();
			
		descrDetDescr.clear();
		contains.clear();
		text.clear();
			
		assert(filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.count()==filterForm->contNContReNReComboBoxList.count());
		assert(filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.count()==filterForm->textLineEditList.count());
		for(int i=0; i<filterForm->rows; i++){
			QComboBox* cb1=filterForm->descrDetDescrDetDescrWithConstraintsComboBoxList.at(i);
			QComboBox* cb2=filterForm->contNContReNReComboBoxList.at(i);
			QLineEdit* tl=filterForm->textLineEditList.at(i);
			
			descrDetDescr.append(cb1->currentIndex());
			contains.append(cb2->currentIndex());
			text.append(tl->text());
		}
		
		filterChanged();

		constraintsListWidget->setFocus();
	}
	else{
		assert(useFilter==false);
		useFilter=false;
	
		disconnect(filterCheckBox, &QCheckBox::toggled, this, &AllTimeConstraintsForm::filter);
		filterCheckBox->setChecked(false);
		connect(filterCheckBox, &QCheckBox::toggled, this, &AllTimeConstraintsForm::filter);
	}
	
	delete filterForm;
}

void AllTimeConstraintsForm::activateConstraints()
{
	if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS){
		QMessageBox::StandardButton ret=QMessageBox::No;
		QString s=tr("Activate the selected time constraints?");
		ret=QMessageBox::question(this, tr("FET confirmation"), s, QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		if(ret==QMessageBox::No){
			constraintsListWidget->setFocus();
			return;
		}
	}

	QString su;

	int cnt=0;
	bool recomputeTime=false;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleTimeConstraintsList.count());
			TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
			if(!ctr->active){
				su+=ctr->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				ctr->active=true;
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
					recomputeTime=true;
			}
		}

	if(cnt>0){
		gt.rules.addUndoPoint(tr("Activated %1 time constraints:", "%1 is the number of activated time constraints").arg(cnt)+QString("\n\n")+su);

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);
		
		int valv=constraintsListWidget->verticalScrollBar()->value();
		int valh=constraintsListWidget->horizontalScrollBar()->value();

		int cr=constraintsListWidget->currentRow();

		filterChanged();

		if(cr>=0){
			if(cr<constraintsListWidget->count())
				constraintsListWidget->setCurrentRow(cr);
			else if(constraintsListWidget->count()>0)
				constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
		}

		constraintsListWidget->verticalScrollBar()->setValue(valv);
		constraintsListWidget->horizontalScrollBar()->setValue(valh);
		
		if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS)
			QMessageBox::information(this, tr("FET information"), tr("Activated %1 time constraints").arg(cnt));
	}
	if(recomputeTime){
		LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
		LockUnlock::increaseCommunicationSpinBox();
	}
	
	constraintsListWidget->setFocus();
}

void AllTimeConstraintsForm::deactivateConstraints()
{
	if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS){
		QMessageBox::StandardButton ret=QMessageBox::No;
		QString s=tr("Deactivate the selected time constraints? "
		 "(Note that the basic compulsory time constraints will not be deactivated, even if they are selected.)");
		ret=QMessageBox::question(this, tr("FET confirmation"), s, QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		if(ret==QMessageBox::No){
			constraintsListWidget->setFocus();
			return;
		}
	}

	QString su;

	int cnt=0;
	bool recomputeTime=false;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleTimeConstraintsList.count());
			TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
			if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_TIME)
				continue;
			if(ctr->active){
				su+=ctr->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				ctr->active=false;
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
					recomputeTime=true;
			}
		}
	if(cnt>0){
		gt.rules.addUndoPoint(tr("Deactivated %1 time constraints:", "%1 is the number of deactivated time constraints").arg(cnt)+QString("\n\n")+su);

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);
		
		int valv=constraintsListWidget->verticalScrollBar()->value();
		int valh=constraintsListWidget->horizontalScrollBar()->value();

		int cr=constraintsListWidget->currentRow();

		filterChanged();

		if(cr>=0){
			if(cr<constraintsListWidget->count())
				constraintsListWidget->setCurrentRow(cr);
			else if(constraintsListWidget->count()>0)
				constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
		}

		constraintsListWidget->verticalScrollBar()->setValue(valv);
		constraintsListWidget->horizontalScrollBar()->setValue(valh);
		
		if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS)
			QMessageBox::information(this, tr("FET information"), tr("Deactivated %1 time constraints").arg(cnt));
	}
	if(recomputeTime){
		LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
		LockUnlock::increaseCommunicationSpinBox();
	}
	
	constraintsListWidget->setFocus();
}

void AllTimeConstraintsForm::constraintComments()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	
	assert(i<visibleTimeConstraintsList.count());
	TimeConstraint* ctr=visibleTimeConstraintsList.at(i);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Constraint comments"));
	
	QPushButton* okPB=new QPushButton(tr("OK"));
	okPB->setDefault(true);
	QPushButton* cancelPB=new QPushButton(tr("Cancel"));
	
	connect(okPB, &QPushButton::clicked, &getCommentsDialog, &QDialog::accept);
	connect(cancelPB, &QPushButton::clicked, &getCommentsDialog, &QDialog::reject);

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(okPB);
	hl->addWidget(cancelPB);
	
	QVBoxLayout* vl=new QVBoxLayout();
	
	QPlainTextEdit* commentsPT=new QPlainTextEdit();
	commentsPT->setPlainText(ctr->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();
	
	vl->addWidget(commentsPT);
	vl->addLayout(hl);
	
	getCommentsDialog.setLayout(vl);
	
	const QString settingsName=QString("TimeConstraintCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString cb=ctr->getDetailedDescription(gt.rules);

		ctr->comments=commentsPT->toPlainText();
	
		gt.rules.addUndoPoint(tr("Changed a constraint's comments. Constraint before:\n\n%1\nComments after:\n\n%2").arg(cb).arg(ctr->comments));

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		if(!filterOk(ctr)){ //Maybe the constraint is no longer visible in the list widget, because of the filter.
			visibleTimeConstraintsList.removeAt(i);
			constraintsListWidget->setCurrentRow(-1);
			QListWidgetItem* item=constraintsListWidget->takeItem(i);
			delete item;

			if(i>=constraintsListWidget->count())
				i=constraintsListWidget->count()-1;
			if(i>=0)
				constraintsListWidget->setCurrentRow(i);
			else
				currentConstraintTextEdit->setPlainText(QString(""));

			int n_active=0;
			for(TimeConstraint* ctr2 : std::as_const(visibleTimeConstraintsList))
				if(ctr2->active)
					n_active++;
	
			constraintsTextLabel->setText(tr("%1 / %2 time constraints",
			 "%1 represents the number of visible active time constraints, %2 represents the total number of visible time constraints")
			 .arg(n_active).arg(visibleTimeConstraintsList.count()));
		}
		else{
			constraintsListWidget->currentItem()->setText(ctr->getDescription(gt.rules));
			constraintChanged();
		}
	}
}

void AllTimeConstraintsForm::selectionChanged()
{
	int nTotal=0;
	int nActive=0;
	assert(constraintsListWidget->count()==visibleTimeConstraintsList.count());
	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			nTotal++;
			if(visibleTimeConstraintsList.at(i)->active)
				nActive++;
		}
	mSLabel->setText(tr("Multiple selection: %1 / %2", "It refers to the list of selected time constraints, %1 is the number of active"
	 " selected time constraints, %2 is the total number of selected time constraints").arg(nActive).arg(nTotal));
}

void AllTimeConstraintsForm::changeWeights()
{
	int cnt_pre=0;
	int cnt_unchanged=0;
	double nw=100.0;
	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleTimeConstraintsList.count());
			TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
			if(ctr->canHaveAnyWeight())
				cnt_pre++;
			else
				cnt_unchanged++;
		}
	if(cnt_pre==0){
		QMessageBox::information(this, tr("FET information"), tr("No constraints from your selection can change their weight"
		 " (remember that some types of constraints are allowed to have only 100% weight)."));
		
		return;
	}
	else{
		bool ok;
		if(cnt_unchanged==0)
			nw=QInputDialog::getDouble(this, tr("Modify the weights of the selected time constraints",
			 "The title of a dialog to modify the weights of the selected constraints with a single click"),
			 tr("You will modify %1 selected time constraints.\n"
			 "Please enter the new weight percentage:",
			 "Translators: please split this field with new line characters, similarly to the original field, so that it is not too wide."
			 " You can use more lines (3 or even 4), if needed. %1 is the number of constraints which will change.")
			 .arg(cnt_pre),
			 nw, 0.0, 100.0, CUSTOM_DOUBLE_PRECISION, &ok, Qt::WindowFlags(), 1);
		else
			nw=QInputDialog::getDouble(this, tr("Modify the weights of the selected time constraints",
			 "The title of a dialog to modify the weights of the selected constraints with a single click"),
			 tr("You will modify %1 time constraints from your\n"
			 "selection (remember that some types of constraints\n"
			 "are only allowed to have 100% weight, so %2\n"
			 "constraints out of the %3 selected will not change).\n"
			 "Please enter the new weight percentage:",
			 "Translators: please split this field with new line characters, similarly to the original field, so that it is not too wide."
			 " You can use more lines (6 or even 7), if needed. %1 is the number of constraints which will change, %2 is the number of constraints"
			 " which will not change, and %3 is the number of all selected constraints.")
			 .arg(cnt_pre).arg(cnt_unchanged).arg(cnt_pre+cnt_unchanged),
			 nw, 0.0, 100.0, CUSTOM_DOUBLE_PRECISION, &ok, Qt::WindowFlags(), 1);
		
		if(!ok)
			return;
	}

	QString su;

	int cnt=0;
	bool recomputeTime=false;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleTimeConstraintsList.count());
			TimeConstraint* ctr=visibleTimeConstraintsList.at(i);
			if(ctr->canHaveAnyWeight()){
				su+=ctr->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				ctr->weightPercentage=nw;
				if(ctr->active && ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
					recomputeTime=true;
			}
		}
		
	assert(cnt>0);
	assert(cnt==cnt_pre);

	if(cnt>0){
		gt.rules.addUndoPoint(tr("Changed the weights of the following %1 selected time constraints to %2%:",
		 "%1 is the number of time constraints for which the user has changed the weight, %2 is the new weight for all the selected constraints")
		 .arg(cnt).arg(CustomFETString::number(nw))+QString("\n\n")+su);

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);
		
		int valv=constraintsListWidget->verticalScrollBar()->value();
		int valh=constraintsListWidget->horizontalScrollBar()->value();

		int cr=constraintsListWidget->currentRow();

		filterChanged();

		if(cr>=0){
			if(cr<constraintsListWidget->count())
				constraintsListWidget->setCurrentRow(cr);
			else if(constraintsListWidget->count()>0)
				constraintsListWidget->setCurrentRow(constraintsListWidget->count()-1);
		}

		constraintsListWidget->verticalScrollBar()->setValue(valv);
		constraintsListWidget->horizontalScrollBar()->setValue(valh);
	}
	if(recomputeTime){
		LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
		LockUnlock::increaseCommunicationSpinBox();
	}
	
	constraintsListWidget->setFocus();
}
