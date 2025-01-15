/***************************************************************************
                          allspaceconstraintsform.cpp  -  description
                             -------------------
    begin                : Feb 13, 2005
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

#include "allspaceconstraintsform.h"

#include "addormodifyspaceconstraint.h"

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

AllSpaceConstraintsForm::AllSpaceConstraintsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	filterCheckBox->setChecked(false);
	sortedCheckBox->setChecked(false);
	
	currentConstraintTextEdit->setReadOnly(true);
	
	modifyConstraintPushButton->setDefault(true);
	
	constraintsListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(constraintsListWidget, &QListWidget::currentRowChanged, this, &AllSpaceConstraintsForm::constraintChanged);

	//selectionChanged();
	connect(constraintsListWidget, &QListWidget::itemSelectionChanged, this, &AllSpaceConstraintsForm::selectionChanged);

	connect(closePushButton, &QPushButton::clicked, this, &AllSpaceConstraintsForm::close);
	connect(removeConstraintsPushButton, &QPushButton::clicked, this, &AllSpaceConstraintsForm::removeConstraints);
	connect(modifyConstraintPushButton, &QPushButton::clicked, this, &AllSpaceConstraintsForm::modifyConstraint);
	connect(constraintsListWidget, &QListWidget::itemDoubleClicked, this, &AllSpaceConstraintsForm::modifyConstraint);
	connect(filterCheckBox, &QCheckBox::toggled, this, &AllSpaceConstraintsForm::filter);

	connect(moveSpaceConstraintUpPushButton, &QPushButton::clicked, this, &AllSpaceConstraintsForm::moveSpaceConstraintUp);
	connect(moveSpaceConstraintDownPushButton, &QPushButton::clicked, this, &AllSpaceConstraintsForm::moveSpaceConstraintDown);

	connect(sortedCheckBox, &QCheckBox::toggled, this, &AllSpaceConstraintsForm::sortedChanged);
	connect(activatePushButton, &QPushButton::clicked, this, &AllSpaceConstraintsForm::activateConstraints);
	connect(deactivatePushButton, &QPushButton::clicked, this, &AllSpaceConstraintsForm::deactivateConstraints);

	//connect(activateAllPushButton, SIG NAL(clicked()), this, SL OT(activateAllConstraints()));
	//connect(deactivateAllPushButton, SIG NAL(clicked()), this, SL OT(deactivateAllConstraints()));

	//connect(sortByCommentsPushButton, SIG NAL(clicked()), this, SL OT(sortConstraintsByComments()));
	connect(commentsPushButton, &QPushButton::clicked, this, &AllSpaceConstraintsForm::constraintComments);

	connect(weightsPushButton, &QPushButton::clicked, this, &AllSpaceConstraintsForm::changeWeights);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());

	QString settingsName="AllSpaceConstraintsAdvancedFilterForm";

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

AllSpaceConstraintsForm::~AllSpaceConstraintsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());

	QString settingsName="AllSpaceConstraintsAdvancedFilterForm";

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

bool AllSpaceConstraintsForm::filterOk(SpaceConstraint* ctr)
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

void AllSpaceConstraintsForm::moveSpaceConstraintUp()
{
	if(filterCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a space constraint up, the 'Filter' check box must not be checked."));
		return;
	}
	if(sortedCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a space constraint up, the 'Sorted' check box must not be checked."));
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
	
	assert(gt.rules.spaceConstraintsList.count()==visibleSpaceConstraintsList.count());
	SpaceConstraint* sc1=gt.rules.spaceConstraintsList.at(i);
	assert(sc1==visibleSpaceConstraintsList.at(i));
	SpaceConstraint* sc2=gt.rules.spaceConstraintsList.at(i-1);
	assert(sc2==visibleSpaceConstraintsList.at(i-1));
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	constraintsListWidget->item(i)->setText(s2);
	constraintsListWidget->item(i-1)->setText(s1);
	
	gt.rules.spaceConstraintsList[i]=sc2;
	gt.rules.spaceConstraintsList[i-1]=sc1;
	
	visibleSpaceConstraintsList[i]=sc2;
	visibleSpaceConstraintsList[i-1]=sc1;
	
	gt.rules.addUndoPoint(tr("A constraint was moved up:\n\n%1", "%1 is the detailed description of the constraint").arg(sc1->getDetailedDescription(gt.rules)));
	
	if(true || USE_GUI_COLORS){
		if(sc2->active)
			constraintsListWidget->item(i)->setBackground(constraintsListWidget->palette().base());
		else
			constraintsListWidget->item(i)->setBackground(constraintsListWidget->palette().alternateBase());

		if(sc1->active)
			constraintsListWidget->item(i-1)->setBackground(constraintsListWidget->palette().base());
		else
			constraintsListWidget->item(i-1)->setBackground(constraintsListWidget->palette().alternateBase());
	}

	constraintsListWidget->setCurrentRow(i-1);
	constraintChanged(/*i-1*/);
}

void AllSpaceConstraintsForm::moveSpaceConstraintDown()
{
	if(filterCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a space constraint down, the 'Filter' check box must not be checked."));
		return;
	}
	if(sortedCheckBox->isChecked()){
		QMessageBox::information(this, tr("FET information"), tr("To move a space constraint down, the 'Sorted' check box must not be checked."));
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
	
	assert(gt.rules.spaceConstraintsList.count()==visibleSpaceConstraintsList.count());
	SpaceConstraint* sc1=gt.rules.spaceConstraintsList.at(i);
	assert(sc1==visibleSpaceConstraintsList.at(i));
	SpaceConstraint* sc2=gt.rules.spaceConstraintsList.at(i+1);
	assert(sc2==visibleSpaceConstraintsList.at(i+1));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	constraintsListWidget->item(i)->setText(s2);
	constraintsListWidget->item(i+1)->setText(s1);
	
	gt.rules.spaceConstraintsList[i]=sc2;
	gt.rules.spaceConstraintsList[i+1]=sc1;
	
	visibleSpaceConstraintsList[i]=sc2;
	visibleSpaceConstraintsList[i+1]=sc1;
	
	gt.rules.addUndoPoint(tr("A constraint was moved down:\n\n%1", "%1 is the detailed description of the constraint").arg(sc1->getDetailedDescription(gt.rules)));
	
	if(true || USE_GUI_COLORS){
		if(sc2->active)
			constraintsListWidget->item(i)->setBackground(constraintsListWidget->palette().base());
		else
			constraintsListWidget->item(i)->setBackground(constraintsListWidget->palette().alternateBase());

		if(sc1->active)
			constraintsListWidget->item(i+1)->setBackground(constraintsListWidget->palette().base());
		else
			constraintsListWidget->item(i+1)->setBackground(constraintsListWidget->palette().alternateBase());
	}

	constraintsListWidget->setCurrentRow(i+1);
	constraintChanged(/*i+1*/);
}

void AllSpaceConstraintsForm::sortedChanged(bool checked)
{
	Q_UNUSED(checked);
	
	filterChanged();

	constraintsListWidget->setFocus();
}

int spaceConstraintsAscendingByDescription(SpaceConstraint* s1, SpaceConstraint* s2)
{
	//return s1->getDescription(gt.rules) < s2->getDescription(gt.rules);
	
	//by Rodolfo Ribeiro Gomes
	return s1->getDescription(gt.rules).localeAwareCompare(s2->getDescription(gt.rules))<0;
}

void AllSpaceConstraintsForm::filterChanged()
{
	disconnect(constraintsListWidget, &QListWidget::itemSelectionChanged, this, &AllSpaceConstraintsForm::selectionChanged);

	visibleSpaceConstraintsList.clear();
	constraintsListWidget->clear();
	int n_active=0;
	for(SpaceConstraint* ctr : std::as_const(gt.rules.spaceConstraintsList))
		if(filterOk(ctr))
			visibleSpaceConstraintsList.append(ctr);
			
	if(sortedCheckBox->isChecked())
		std::stable_sort(visibleSpaceConstraintsList.begin(), visibleSpaceConstraintsList.end(), spaceConstraintsAscendingByDescription);
	
	for(SpaceConstraint* ctr : std::as_const(visibleSpaceConstraintsList)){
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
	 "%1 represents the number of visible active space constraints, %2 represents the total number of visible space constraints")
	 .arg(n_active).arg(visibleSpaceConstraintsList.count()));
	//mSLabel->setText(tr("Multiple selection", "The list can have multiple selection. Keep translation short."));

	selectionChanged();
	connect(constraintsListWidget, &QListWidget::itemSelectionChanged, this, &AllSpaceConstraintsForm::selectionChanged);
}

void AllSpaceConstraintsForm::constraintChanged()
{
	int index=constraintsListWidget->currentRow();

	if(index<0)
		return;

	assert(index<visibleSpaceConstraintsList.count());
	SpaceConstraint* ctr=visibleSpaceConstraintsList.at(index);
	assert(ctr!=nullptr);
	QString s=ctr->getDetailedDescription(gt.rules);
	currentConstraintTextEdit->setPlainText(s);
}

void AllSpaceConstraintsForm::modifyConstraint()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
	
		constraintsListWidget->setFocus();

		return;
	}

	int valv=constraintsListWidget->verticalScrollBar()->value();
	int valh=constraintsListWidget->horizontalScrollBar()->value();

	assert(i<visibleSpaceConstraintsList.count());
	SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);
	
	AddOrModifySpaceConstraint(this, ctr->type, ctr);

	/*switch(ctr->type){
		//1
		case CONSTRAINT_BASIC_COMPULSORY_SPACE:
			{
				ModifyConstraintBasicCompulsorySpaceForm form(this, (ConstraintBasicCompulsorySpace*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//2
		case CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES:
			{
				ModifyConstraintRoomNotAvailableTimesForm form(this, (ConstraintRoomNotAvailableTimes*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//3
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOM:
			{
				ModifyConstraintActivityPreferredRoomForm form(this, (ConstraintActivityPreferredRoom*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//4
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOMS:
			{
				ModifyConstraintActivityPreferredRoomsForm form(this, (ConstraintActivityPreferredRooms*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//5
		case CONSTRAINT_STUDENTS_SET_HOME_ROOM:
			{
				ModifyConstraintStudentsSetHomeRoomForm form(this, (ConstraintStudentsSetHomeRoom*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//6
		case CONSTRAINT_STUDENTS_SET_HOME_ROOMS:
			{
				ModifyConstraintStudentsSetHomeRoomsForm form(this, (ConstraintStudentsSetHomeRooms*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//7
		case CONSTRAINT_TEACHER_HOME_ROOM:
			{
				ModifyConstraintTeacherHomeRoomForm form(this, (ConstraintTeacherHomeRoom*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//8
		case CONSTRAINT_TEACHER_HOME_ROOMS:
			{
				ModifyConstraintTeacherHomeRoomsForm form(this, (ConstraintTeacherHomeRooms*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//9
		case CONSTRAINT_SUBJECT_PREFERRED_ROOM:
			{
				ModifyConstraintSubjectPreferredRoomForm form(this, (ConstraintSubjectPreferredRoom*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//10
		case CONSTRAINT_SUBJECT_PREFERRED_ROOMS:
			{
				ModifyConstraintSubjectPreferredRoomsForm form(this, (ConstraintSubjectPreferredRooms*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//11
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM:
			{
				ModifyConstraintSubjectActivityTagPreferredRoomForm form(this, (ConstraintSubjectActivityTagPreferredRoom*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//12
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS:
			{
				ModifyConstraintSubjectActivityTagPreferredRoomsForm form(this, (ConstraintSubjectActivityTagPreferredRooms*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		///6 apr 2009
		//13
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM:
			{
				ModifyConstraintActivityTagPreferredRoomForm form(this, (ConstraintActivityTagPreferredRoom*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//14
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS:
			{
				ModifyConstraintActivityTagPreferredRoomsForm form(this, (ConstraintActivityTagPreferredRooms*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		///
		//15
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY:
			{
				ModifyConstraintStudentsSetMaxBuildingChangesPerDayForm form(this, (ConstraintStudentsSetMaxBuildingChangesPerDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//16
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY:
			{
				ModifyConstraintStudentsMaxBuildingChangesPerDayForm form(this, (ConstraintStudentsMaxBuildingChangesPerDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//17
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				ModifyConstraintStudentsSetMaxBuildingChangesPerWeekForm form(this, (ConstraintStudentsSetMaxBuildingChangesPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//18
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				ModifyConstraintStudentsMaxBuildingChangesPerWeekForm form(this, (ConstraintStudentsMaxBuildingChangesPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//19
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				ModifyConstraintStudentsSetMinGapsBetweenBuildingChangesForm form(this, (ConstraintStudentsSetMinGapsBetweenBuildingChanges*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//20
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				ModifyConstraintStudentsMinGapsBetweenBuildingChangesForm form(this, (ConstraintStudentsMinGapsBetweenBuildingChanges*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//21
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY:
			{
				ModifyConstraintTeacherMaxBuildingChangesPerDayForm form(this, (ConstraintTeacherMaxBuildingChangesPerDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//22
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY:
			{
				ModifyConstraintTeachersMaxBuildingChangesPerDayForm form(this, (ConstraintTeachersMaxBuildingChangesPerDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//23
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				ModifyConstraintTeacherMaxBuildingChangesPerWeekForm form(this, (ConstraintTeacherMaxBuildingChangesPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//24
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				ModifyConstraintTeachersMaxBuildingChangesPerWeekForm form(this, (ConstraintTeachersMaxBuildingChangesPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//25
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				ModifyConstraintTeacherMinGapsBetweenBuildingChangesForm form(this, (ConstraintTeacherMinGapsBetweenBuildingChanges*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//26
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				ModifyConstraintTeachersMinGapsBetweenBuildingChangesForm form(this, (ConstraintTeachersMinGapsBetweenBuildingChanges*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//27
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS:
			{
				ModifyConstraintActivitiesOccupyMaxDifferentRoomsForm form(this, (ConstraintActivitiesOccupyMaxDifferentRooms*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//28
		case CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE:
			{
				ModifyConstraintActivitiesSameRoomIfConsecutiveForm form(this, (ConstraintActivitiesSameRoomIfConsecutive*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//2019-11-14
		//29
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY:
			{
				ModifyConstraintStudentsSetMaxRoomChangesPerDayForm form(this, (ConstraintStudentsSetMaxRoomChangesPerDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//30
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY:
			{
				ModifyConstraintStudentsMaxRoomChangesPerDayForm form(this, (ConstraintStudentsMaxRoomChangesPerDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//31
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK:
			{
				ModifyConstraintStudentsSetMaxRoomChangesPerWeekForm form(this, (ConstraintStudentsSetMaxRoomChangesPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//32
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_WEEK:
			{
				ModifyConstraintStudentsMaxRoomChangesPerWeekForm form(this, (ConstraintStudentsMaxRoomChangesPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//33
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				ModifyConstraintStudentsSetMinGapsBetweenRoomChangesForm form(this, (ConstraintStudentsSetMinGapsBetweenRoomChanges*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//34
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				ModifyConstraintStudentsMinGapsBetweenRoomChangesForm form(this, (ConstraintStudentsMinGapsBetweenRoomChanges*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//35
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY:
			{
				ModifyConstraintTeacherMaxRoomChangesPerDayForm form(this, (ConstraintTeacherMaxRoomChangesPerDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//36
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY:
			{
				ModifyConstraintTeachersMaxRoomChangesPerDayForm form(this, (ConstraintTeachersMaxRoomChangesPerDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//37
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK:
			{
				ModifyConstraintTeacherMaxRoomChangesPerWeekForm form(this, (ConstraintTeacherMaxRoomChangesPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//38
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK:
			{
				ModifyConstraintTeachersMaxRoomChangesPerWeekForm form(this, (ConstraintTeachersMaxRoomChangesPerWeek*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//39
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				ModifyConstraintTeacherMinGapsBetweenRoomChangesForm form(this, (ConstraintTeacherMinGapsBetweenRoomChanges*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//40
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				ModifyConstraintTeachersMinGapsBetweenRoomChangesForm form(this, (ConstraintTeachersMinGapsBetweenRoomChanges*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//41
		case CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES:
			{
				ModifyConstraintTeacherRoomNotAvailableTimesForm form(this, (ConstraintTeacherRoomNotAvailableTimes*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//for mornings-afternoons
		//42
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				ModifyConstraintStudentsSetMaxRoomChangesPerRealDayForm form(this, (ConstraintStudentsSetMaxRoomChangesPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//43
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				ModifyConstraintStudentsMaxRoomChangesPerRealDayForm form(this, (ConstraintStudentsMaxRoomChangesPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//44
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				ModifyConstraintTeacherMaxRoomChangesPerRealDayForm form(this, (ConstraintTeacherMaxRoomChangesPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//45
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				ModifyConstraintTeachersMaxRoomChangesPerRealDayForm form(this, (ConstraintTeachersMaxRoomChangesPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//46
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayForm form(this, (ConstraintStudentsSetMaxBuildingChangesPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//47
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				ModifyConstraintStudentsMaxBuildingChangesPerRealDayForm form(this, (ConstraintStudentsMaxBuildingChangesPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//48
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				ModifyConstraintTeacherMaxBuildingChangesPerRealDayForm form(this, (ConstraintTeacherMaxBuildingChangesPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//49
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				ModifyConstraintTeachersMaxBuildingChangesPerRealDayForm form(this, (ConstraintTeachersMaxBuildingChangesPerRealDay*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		////2024-02-09
		//50
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				ModifyConstraintStudentsMaxBuildingChangesPerDayInIntervalForm form(this, (ConstraintStudentsMaxBuildingChangesPerDayInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//51
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				ModifyConstraintStudentsSetMaxBuildingChangesPerDayInIntervalForm form(this, (ConstraintStudentsSetMaxBuildingChangesPerDayInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//52
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				ModifyConstraintTeachersMaxBuildingChangesPerDayInIntervalForm form(this, (ConstraintTeachersMaxBuildingChangesPerDayInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//53
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				ModifyConstraintTeacherMaxBuildingChangesPerDayInIntervalForm form(this, (ConstraintTeacherMaxBuildingChangesPerDayInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//54
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				ModifyConstraintStudentsMaxBuildingChangesPerRealDayInIntervalForm form(this, (ConstraintStudentsMaxBuildingChangesPerRealDayInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//55
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayInIntervalForm form(this, (ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//56
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				ModifyConstraintTeachersMaxBuildingChangesPerRealDayInIntervalForm form(this, (ConstraintTeachersMaxBuildingChangesPerRealDayInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//57
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				ModifyConstraintTeacherMaxBuildingChangesPerRealDayInIntervalForm form(this, (ConstraintTeacherMaxBuildingChangesPerRealDayInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		////2024-02-19
		//58
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				ModifyConstraintStudentsMaxRoomChangesPerDayInIntervalForm form(this, (ConstraintStudentsMaxRoomChangesPerDayInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//59
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				ModifyConstraintStudentsSetMaxRoomChangesPerDayInIntervalForm form(this, (ConstraintStudentsSetMaxRoomChangesPerDayInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//60
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				ModifyConstraintTeachersMaxRoomChangesPerDayInIntervalForm form(this, (ConstraintTeachersMaxRoomChangesPerDayInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//61
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				ModifyConstraintTeacherMaxRoomChangesPerDayInIntervalForm form(this, (ConstraintTeacherMaxRoomChangesPerDayInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//62
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				ModifyConstraintStudentsMaxRoomChangesPerRealDayInIntervalForm form(this, (ConstraintStudentsMaxRoomChangesPerRealDayInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//63
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				ModifyConstraintStudentsSetMaxRoomChangesPerRealDayInIntervalForm form(this, (ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//64
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				ModifyConstraintTeachersMaxRoomChangesPerRealDayInIntervalForm form(this, (ConstraintTeachersMaxRoomChangesPerRealDayInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//65
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				ModifyConstraintTeacherMaxRoomChangesPerRealDayInIntervalForm form(this, (ConstraintTeacherMaxRoomChangesPerRealDayInInterval*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//66
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				ModifyConstraintRoomMaxActivityTagsPerDayFromSetForm form(this, (ConstraintRoomMaxActivityTagsPerDayFromSet*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//67
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				ModifyConstraintRoomMaxActivityTagsPerRealDayFromSetForm form(this, (ConstraintRoomMaxActivityTagsPerRealDayFromSet*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		//68
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_WEEK_FROM_SET:
			{
				ModifyConstraintRoomMaxActivityTagsPerWeekFromSetForm form(this, (ConstraintRoomMaxActivityTagsPerWeekFromSet*)ctr);
				setParentAndOtherThings(&form, this);
				form.exec();
				break;
			}
		////
		default:
			QMessageBox::critical(this, tr("FET critical"), tr("You have found a bug in FET. Please report it. This kind of constraint"
			 " is not correctly recognized in all space constraints dialog. FET will skip this error, so that you can continue work."
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

void AllSpaceConstraintsForm::removeConstraints()
{
	bool recompute=false;

	QList<SpaceConstraint*> tl;

	bool firstBasic=true;
	
	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleSpaceConstraintsList.count());
			SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);
			tl.append(ctr);

			if(firstBasic && ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE){
				firstBasic=false;

				QMessageBox::StandardButton wr;

				QString s=tr("Your selection contains the basic compulsory space constraint(s). Do you really want to remove all the selected "
				 "constraints, including this (these) one(s)? You cannot generate a timetable without this (these) constraint(s).");
				s+="\n\n";
				s+=tr("Note: you can add again a constraint of this type from the menu Data -> Space constraints -> "
					"Miscellaneous -> Basic compulsory space constraints.");

				wr=QMessageBox::warning(this, tr("FET warning"), s,
					QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

				if(wr==QMessageBox::No){
					constraintsListWidget->setFocus();
					return;
				}
			}
			else if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
				recompute=true;
			}
		}

	QString s;
	s=tr("Remove these selected space constraints?");
	s+="\n\n";
	for(SpaceConstraint* ctr : std::as_const(tl))
		s+=ctr->getDetailedDescription(gt.rules)+"\n";
	int lres=LongTextMessageBox::confirmation(this, tr("FET confirmation"),
		s, tr("Yes"), tr("No"), QString(), 0, 1 );

	if(lres!=0){
		constraintsListWidget->setFocus();
		return;
	}

	QString su;
	if(!tl.isEmpty()){
		su=tr("Removed %1 space constraints:").arg(tl.count());
		su+=QString("\n\n");
		for(SpaceConstraint* ctr : std::as_const(tl))
			su+=ctr->getDetailedDescription(gt.rules)+"\n";
	}

	//The user clicked the OK button or pressed Enter
	gt.rules.removeSpaceConstraints(tl);

	if(!tl.isEmpty())
		gt.rules.addUndoPoint(su);

	if(recompute){
		LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
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

void AllSpaceConstraintsForm::filter(bool active)
{
	if(!active){
		assert(useFilter==true);
		useFilter=false;
		
		filterChanged();
		
		constraintsListWidget->setFocus();
	
		return;
	}
	
	assert(active);
	
	filterForm=new AdvancedFilterForm(this, tr("Advanced filter for space constraints"), false, all, descrDetDescr, contains, text, caseSensitive, "AllSpaceConstraintsAdvancedFilterForm");

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
		
		disconnect(filterCheckBox, &QCheckBox::toggled, this, &AllSpaceConstraintsForm::filter);
		filterCheckBox->setChecked(false);
		connect(filterCheckBox, &QCheckBox::toggled, this, &AllSpaceConstraintsForm::filter);
	}
	
	delete filterForm;
}

void AllSpaceConstraintsForm::activateConstraints()
{
	if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS){
		QMessageBox::StandardButton ret=QMessageBox::No;
		QString s=tr("Activate the selected space constraints?");
		ret=QMessageBox::question(this, tr("FET confirmation"), s, QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		if(ret==QMessageBox::No){
			constraintsListWidget->setFocus();
			return;
		}
	}

	QString su;

	int cnt=0;
	bool recomputeSpace=false;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleSpaceConstraintsList.count());
			SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);
			if(!ctr->active){
				su+=ctr->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				ctr->active=true;
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM)
					recomputeSpace=true;
			}
		}

	if(cnt>0){
		gt.rules.addUndoPoint(tr("Activated %1 space constraints:", "%1 is the number of activated space constraints").arg(cnt)+QString("\n\n")+su);

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
			QMessageBox::information(this, tr("FET information"), tr("Activated %1 space constraints").arg(cnt));
	}
	if(recomputeSpace){
		LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
		LockUnlock::increaseCommunicationSpinBox();
	}

	constraintsListWidget->setFocus();
}

void AllSpaceConstraintsForm::deactivateConstraints()
{
	if(CONFIRM_ACTIVATE_DEACTIVATE_ACTIVITIES_CONSTRAINTS){
		QMessageBox::StandardButton ret=QMessageBox::No;
		QString s=tr("Deactivate the selected space constraints? "
		 "(Note that the basic compulsory space constraints will not be deactivated, even if they are selected.)");
		ret=QMessageBox::question(this, tr("FET confirmation"), s, QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
		if(ret==QMessageBox::No){
			constraintsListWidget->setFocus();
			return;
		}
	}

	QString su;

	int cnt=0;
	bool recomputeSpace=false;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleSpaceConstraintsList.count());
			SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);
			if(ctr->type==CONSTRAINT_BASIC_COMPULSORY_SPACE)
				continue;
			if(ctr->active){
				su+=ctr->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				ctr->active=false;
				if(ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM)
					recomputeSpace=true;
			}
		}
	if(cnt>0){
		gt.rules.addUndoPoint(tr("Deactivated %1 space constraints:", "%1 is the number of deactivated space constraints").arg(cnt)+QString("\n\n")+su);

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
			QMessageBox::information(this, tr("FET information"), tr("Deactivated %1 space constraints").arg(cnt));
	}
	if(recomputeSpace){
		LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
		LockUnlock::increaseCommunicationSpinBox();
	}

	constraintsListWidget->setFocus();
}

void AllSpaceConstraintsForm::constraintComments()
{
	int i=constraintsListWidget->currentRow();
	if(i<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected constraint"));
		return;
	}
	
	assert(i<visibleSpaceConstraintsList.count());
	SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);

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
	
	const QString settingsName=QString("SpaceConstraintCommentsDialog");
	
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
			visibleSpaceConstraintsList.removeAt(i);
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
			for(SpaceConstraint* ctr2 : std::as_const(visibleSpaceConstraintsList))
				if(ctr2->active)
					n_active++;
		
			constraintsTextLabel->setText(tr("%1 / %2 space constraints",
			 "%1 represents the number of visible active space constraints, %2 represents the total number of visible space constraints")
			 .arg(n_active).arg(visibleSpaceConstraintsList.count()));
		}
		else{
			constraintsListWidget->currentItem()->setText(ctr->getDescription(gt.rules));
			constraintChanged();
		}
	}
}

void AllSpaceConstraintsForm::selectionChanged()
{
	int nTotal=0;
	int nActive=0;
	assert(constraintsListWidget->count()==visibleSpaceConstraintsList.count());
	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			nTotal++;
			if(visibleSpaceConstraintsList.at(i)->active)
				nActive++;
		}
	mSLabel->setText(tr("Multiple selection: %1 / %2", "It refers to the list of selected space constraints, %1 is the number of active"
	 " selected space constraints, %2 is the total number of selected space constraints").arg(nActive).arg(nTotal));
}

void AllSpaceConstraintsForm::changeWeights()
{
	int cnt_pre=0;
	int cnt_unchanged=0;
	double nw=100.0;
	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleSpaceConstraintsList.count());
			SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);
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
			nw=QInputDialog::getDouble(this, tr("Modify the weights of the selected space constraints",
			 "The title of a dialog to modify the weights of the selected constraints with a single click"),
			 tr("You will modify %1 selected space constraints.\n"
			 "Please enter the new weight percentage:",
			 "Translators: please split this field with new line characters, similarly to the original field, so that it is not too wide."
			 " You can use more lines (3 or even 4), if needed. %1 is the number of constraints which will change.")
			 .arg(cnt_pre),
			 nw, 0.0, 100.0, CUSTOM_DOUBLE_PRECISION, &ok, Qt::WindowFlags(), 1);
		else
			nw=QInputDialog::getDouble(this, tr("Modify the weights of the selected space constraints",
			 "The title of a dialog to modify the weights of the selected constraints with a single click"),
			 tr("You will modify %1 space constraints from your\n"
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
	bool recomputeSpace=false;

	for(int i=0; i<constraintsListWidget->count(); i++)
		if(constraintsListWidget->item(i)->isSelected()){
			assert(i<visibleSpaceConstraintsList.count());
			SpaceConstraint* ctr=visibleSpaceConstraintsList.at(i);
			if(ctr->canHaveAnyWeight()){
				su+=ctr->getDetailedDescription(gt.rules)+QString("\n");

				cnt++;
				ctr->weightPercentage=nw;
				if(ctr->active && ctr->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM)
					recomputeSpace=true;
			}
		}
		
	assert(cnt>0);
	assert(cnt==cnt_pre);

	if(cnt>0){
		gt.rules.addUndoPoint(tr("Changed the weights of the following %1 selected space constraints to %2%:",
		 "%1 is the number of space constraints for which the user has changed the weight, %2 is the new weight for all the selected constraints")
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
	if(recomputeSpace){
		LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
		LockUnlock::increaseCommunicationSpinBox();
	}
	
	constraintsListWidget->setFocus();
}
