//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "addstudentssubgroupform.h"
#include "addexistingstudentssubgroupsform.h"
#include "modifystudentssubgroupform.h"
#include "subgroupsform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include "longtextmessagebox.h"

#include <QMessageBox>

#include <Qt>
#include <QShortcut>
#include <QKeySequence>

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

#include <QSet>
#include <QList>
#include <QPair>

extern const QString COMPANY;
extern const QString PROGRAM;

extern bool students_schedule_ready;
extern bool rooms_buildings_schedule_ready;
extern bool teachers_schedule_ready;

SubgroupsForm::SubgroupsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	subgroupTextEdit->setReadOnly(true);

	modifySubgroupPushButton->setDefault(true);

	yearsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	groupsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	subgroupsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(yearsListWidget, &QListWidget::currentTextChanged, this, &SubgroupsForm::yearChanged);
	connect(groupsListWidget, &QListWidget::currentTextChanged, this, &SubgroupsForm::groupChanged);
	connect(addSubgroupPushButton, &QPushButton::clicked, this, &SubgroupsForm::addSubgroup);
	connect(addExistingSubgroupsPushButton, &QPushButton::clicked, this, &SubgroupsForm::addExistingSubgroups);
	connect(removeSubgroupPushButton, &QPushButton::clicked, this, &SubgroupsForm::removeSubgroup);

	connect(purgeSubgroupPushButton, &QPushButton::clicked, this, &SubgroupsForm::purgeSubgroup);
	connect(closePushButton, &QPushButton::clicked, this, &SubgroupsForm::close);
	connect(subgroupsListWidget, &QListWidget::currentTextChanged, this, &SubgroupsForm::subgroupChanged);
	connect(modifySubgroupPushButton, &QPushButton::clicked, this, &SubgroupsForm::modifySubgroup);

	connect(moveSubgroupUpPushButton, &QPushButton::clicked, this, &SubgroupsForm::moveSubgroupUp);
	connect(moveSubgroupDownPushButton, &QPushButton::clicked, this, &SubgroupsForm::moveSubgroupDown);

	connect(sortSubgroupsPushButton, &QPushButton::clicked, this, &SubgroupsForm::sortSubgroups);
	connect(activateStudentsPushButton, &QPushButton::clicked, this, &SubgroupsForm::activateStudents);
	connect(deactivateStudentsPushButton, &QPushButton::clicked, this, &SubgroupsForm::deactivateStudents);
	connect(subgroupsListWidget, &QListWidget::itemDoubleClicked, this, &SubgroupsForm::modifySubgroup);

	connect(longNamePushButton, &QPushButton::clicked, this, &SubgroupsForm::longName);
	connect(codePushButton, &QPushButton::clicked, this, &SubgroupsForm::code);
	connect(commentsPushButton, &QPushButton::clicked, this, &SubgroupsForm::comments);

	if(SHORTCUT_PLUS){
		QShortcut* addShortcut=new QShortcut(QKeySequence(Qt::Key_Plus), this);
		connect(addShortcut, &QShortcut::activated, [=]{addSubgroupPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	addSubgroupPushButton->setToolTip(QString("+"));
	}
	if(SHORTCUT_M){
		QShortcut* modifyShortcut=new QShortcut(QKeySequence(Qt::Key_M), this);
		connect(modifyShortcut, &QShortcut::activated, [=]{modifySubgroupPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	modifySubgroupPushButton->setToolTip(QString("M"));
	}
	if(SHORTCUT_DELETE){
		QShortcut* removeShortcut=new QShortcut(QKeySequence::Delete, this);
		connect(removeShortcut, &QShortcut::activated, [=]{removeSubgroupPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	removeSubgroupPushButton->setToolTip(QString("⌦"));
	}
	if(SHORTCUT_A){
		QShortcut* activateShortcut=new QShortcut(QKeySequence(Qt::Key_A), this);
		connect(activateShortcut, &QShortcut::activated, [=]{activateStudentsPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	activateStudentsPushButton->setToolTip(QString("A"));
	}
	if(SHORTCUT_D){
		QShortcut* deactivateShortcut=new QShortcut(QKeySequence(Qt::Key_D), this);
		connect(deactivateShortcut, &QShortcut::activated, [=]{deactivateStudentsPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	deactivateStudentsPushButton->setToolTip(QString("D"));
	}
	if(SHORTCUT_C){
		QShortcut* commentsShortcut=new QShortcut(QKeySequence(Qt::Key_C), this);
		connect(commentsShortcut, &QShortcut::activated, [=]{commentsPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	commentsPushButton->setToolTip(QString("C"));
	}
	if(SHORTCUT_U){
		QShortcut* upShortcut=new QShortcut(QKeySequence(Qt::Key_U), this);
		connect(upShortcut, &QShortcut::activated, [=]{moveSubgroupUpPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	moveSubgroupUpPushButton->setToolTip(QString("U"));
	}
	if(SHORTCUT_J){
		QShortcut* downShortcut=new QShortcut(QKeySequence(Qt::Key_J), this);
		connect(downShortcut, &QShortcut::activated, [=]{moveSubgroupDownPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	moveSubgroupDownPushButton->setToolTip(QString("J"));
	}

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	
	yearsListWidget->clear();
	for(int i=0; i<gt.rules.yearsList.size(); i++){
		StudentsYear* year=gt.rules.yearsList[i];
		yearsListWidget->addItem(year->name);
	}

	if(yearsListWidget->count()>0)
		yearsListWidget->setCurrentRow(0);
	else{
		groupsListWidget->clear();
		subgroupsListWidget->clear();
	}
}

SubgroupsForm::~SubgroupsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
}

void SubgroupsForm::addSubgroup()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	AddStudentsSubgroupForm form(this, yearName, groupName);
	setParentAndOtherThings(&form, this);
	form.exec();

	groupChanged(groupsListWidget->currentItem()->text());
	
	int i=subgroupsListWidget->count()-1;
	if(i>=0)
		subgroupsListWidget->setCurrentRow(i);
}

void SubgroupsForm::addExistingSubgroups()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	
	StudentsYear* year=nullptr;
	
	for(StudentsYear* sty : std::as_const(gt.rules.yearsList))
		if(sty->name==yearName){
			year=sty;
			break;
		}
		
	assert(year!=nullptr);
	
	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListWidget->currentItem()->text();
	
	StudentsGroup* group=nullptr;
	
	for(StudentsGroup* stg : std::as_const(year->groupsList))
		if(stg->name==groupName){
			group=stg;
			break;
		}
		
	assert(group!=nullptr);
	
	AddExistingStudentsSubgroupsForm form(this, year, group);
	setParentAndOtherThings(&form, this);
	int t=form.exec();
	
	if(t==QDialog::Accepted){
		groupChanged(groupsListWidget->currentItem()->text());
	
		int i=subgroupsListWidget->count()-1;
		if(i>=0)
			subgroupsListWidget->setCurrentRow(i);
	}
	else{
		assert(t==QDialog::Rejected);
	}
}

void SubgroupsForm::removeSubgroup()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	if(subgroupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	
	QString subgroupName=subgroupsListWidget->currentItem()->text();
	int subgroupIndex=gt.rules.searchSubgroup(yearName, groupName, subgroupName);
	assert(subgroupIndex>=0);
	
	QList<QPair<QString, QString>> yearsGroupsContainingSubgroup_List;
	//QSet<QPair<QString, QString>> yearsGroupsContainingSubgroup_Set;
	for(StudentsYear* year : std::as_const(gt.rules.yearsList))
		for(StudentsGroup* group : std::as_const(year->groupsList))
			for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
				if(subgroup->name==subgroupName)
					yearsGroupsContainingSubgroup_List.append(QPair<QString, QString>(year->name, group->name));
			
	assert(yearsGroupsContainingSubgroup_List.count()>=1);
	QString s;
	if(yearsGroupsContainingSubgroup_List.count()==1)
		s=tr("This subgroup exists only in year %1, group %2. This means that"
		 " all the related activities and constraints will be removed. Do you want to continue?").arg(yearName).arg(groupName);
	else{
		s=tr("This subgroup exists in more places, listed below. It will only be removed from the current year/group,"
		 " and the related activities and constraints will not be removed. Do you want to continue?");
		s+="\n";
		for(const QPair<QString, QString>& pair : std::as_const(yearsGroupsContainingSubgroup_List))
			s+=QString("\n")+pair.first+translatedCommaSpace()+pair.second;
	}
	
	int t=LongTextMessageBox::mediumConfirmation(this, tr("FET confirmation"), s,
		tr("Yes"), tr("No"), QString(), 0, 1);
	if(t==1)
		return;

	/*if(QMessageBox::warning( this, tr("FET"),
	 tr("Are you sure you want to delete subgroup %1 and all related activities and constraints?").arg(subgroupName),
	 tr("Yes"), tr("No"), QString(), 0, 1 ) == 1)
		return;*/

	bool tmp=gt.rules.removeSubgroup(yearName, groupName, subgroupName);
	assert(tmp);
	if(tmp){
		gt.rules.addUndoPoint(tr("Removed the subgroup %1 from the group %2, the year %3.").arg(subgroupName).arg(groupName).arg(yearName));

		int q=subgroupsListWidget->currentRow();
		
		subgroupsListWidget->setCurrentRow(-1);
		QListWidgetItem* item;
		item=subgroupsListWidget->takeItem(q);
		delete item;
		
		if(q>=subgroupsListWidget->count())
			q=subgroupsListWidget->count()-1;
		if(q>=0)
			subgroupsListWidget->setCurrentRow(q);
		else
			subgroupTextEdit->setPlainText(QString(""));
	}

	/*if(gt.rules.searchStudentsSet(subgroupName)!=nullptr)
		QMessageBox::information( this, tr("FET"), tr("This subgroup still exists into another group. "
		"The related activities and constraints were not removed"));*/
}

void SubgroupsForm::purgeSubgroup()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	if(subgroupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	
	QString subgroupName=subgroupsListWidget->currentItem()->text();
	int subgroupIndex=gt.rules.searchSubgroup(yearName, groupName, subgroupName);
	assert(subgroupIndex>=0);
	
	QList<QPair<QString, QString>> yearsGroupsContainingSubgroup_List;
	//QSet<QPair<QString, QString>> yearsGroupsContainingSubgroup_Set;
	for(StudentsYear* year : std::as_const(gt.rules.yearsList))
		for(StudentsGroup* group : std::as_const(year->groupsList))
			for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
				if(subgroup->name==subgroupName)
					yearsGroupsContainingSubgroup_List.append(QPair<QString, QString>(year->name, group->name));
			
	assert(yearsGroupsContainingSubgroup_List.count()>=1);
	QString s;
	if(yearsGroupsContainingSubgroup_List.count()==1)
		s=tr("This subgroup exists only in year %1, group %2. All the related activities and constraints "
		 "will be removed. Do you want to continue?").arg(yearName).arg(groupName);
	else{
		s=tr("This subgroup exists in more places, listed below. It will be removed from all these places."
		 " All the related activities and constraints will be removed. Do you want to continue?");
		s+="\n";
		for(const QPair<QString, QString>& pair : std::as_const(yearsGroupsContainingSubgroup_List))
			s+=QString("\n")+pair.first+translatedCommaSpace()+pair.second;
	}
	
	int t=LongTextMessageBox::mediumConfirmation(this, tr("FET confirmation"), s,
		tr("Yes"), tr("No"), QString(), 0, 1);
	if(t==1)
		return;

	/*if(QMessageBox::warning( this, tr("FET"),
	 tr("Are you sure you want to delete subgroup %1 and all related activities and constraints?").arg(subgroupName),
	 tr("Yes"), tr("No"), QString(), 0, 1 ) == 1)
		return;*/

	bool tmp=gt.rules.purgeSubgroup(subgroupName);
	assert(tmp);
	if(tmp){
		gt.rules.addUndoPoint(tr("Removed the subgroup %1 from everywhere.").arg(subgroupName));

		int q=subgroupsListWidget->currentRow();
		
		subgroupsListWidget->setCurrentRow(-1);
		QListWidgetItem* item;
		item=subgroupsListWidget->takeItem(q);
		delete item;
		
		if(q>=subgroupsListWidget->count())
			q=subgroupsListWidget->count()-1;
		if(q>=0)
			subgroupsListWidget->setCurrentRow(q);
		else
			subgroupTextEdit->setPlainText(QString(""));
	}

	/*if(gt.rules.searchStudentsSet(subgroupName)!=nullptr)
		QMessageBox::information( this, tr("FET"), tr("This subgroup still exists into another group. "
		"The related activities and constraints were not removed"));*/
}

void SubgroupsForm::yearChanged(const QString& yearName)
{
	int yearIndex=gt.rules.searchYear(yearName);
	if(yearIndex<0){
		groupsListWidget->clear();
		subgroupsListWidget->clear();
		subgroupTextEdit->setPlainText(QString(""));
		return;
	}
	StudentsYear* sty=gt.rules.yearsList.at(yearIndex);

	groupsListWidget->clear();
	for(int i=0; i<sty->groupsList.size(); i++){
		StudentsGroup* stg=sty->groupsList[i];
		groupsListWidget->addItem(stg->name);
	}

	if(groupsListWidget->count()>0)
		groupsListWidget->setCurrentRow(0);
	else{
		subgroupsListWidget->clear();
		subgroupTextEdit->setPlainText(QString(""));
	}
}

void SubgroupsForm::groupChanged(const QString& groupName)
{
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	if(yearIndex<0){
		return;
	}
	StudentsYear* sty=gt.rules.yearsList.at(yearIndex);
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	if(groupIndex<0){
		subgroupsListWidget->clear();
		subgroupTextEdit->setPlainText(QString(""));
		return;
	}

	StudentsGroup* stg=sty->groupsList.at(groupIndex);

	subgroupsListWidget->clear();
	for(int i=0; i<stg->subgroupsList.size(); i++){
		StudentsSubgroup* sts=stg->subgroupsList[i];
		subgroupsListWidget->addItem(sts->name);
	}

	if(subgroupsListWidget->count()>0)
		subgroupsListWidget->setCurrentRow(0);
	else
		subgroupTextEdit->setPlainText(QString(""));
}

void SubgroupsForm::subgroupChanged(const QString& subgroupName)
{
	StudentsSet* ss=gt.rules.searchStudentsSet(subgroupName);
	if(ss==nullptr){
		subgroupTextEdit->setPlainText(QString(""));
		return;
	}
	StudentsSubgroup* s=(StudentsSubgroup*)ss;
	subgroupTextEdit->setPlainText(s->getDetailedDescriptionWithConstraints(gt.rules));
}

void SubgroupsForm::moveSubgroupUp()
{
	if(subgroupsListWidget->count()<=1)
		return;
	int i=subgroupsListWidget->currentRow();
	if(i<0 || i>=subgroupsListWidget->count())
		return;
	if(i==0)
		return;
		
	QString s1=subgroupsListWidget->item(i)->text();
	QString s2=subgroupsListWidget->item(i-1)->text();
	
	assert(yearsListWidget->currentRow()>=0);
	assert(yearsListWidget->currentRow()<gt.rules.yearsList.count());
	StudentsYear* sy=gt.rules.yearsList.at(yearsListWidget->currentRow());
	
	assert(groupsListWidget->currentRow()>=0);
	assert(groupsListWidget->currentRow()<sy->groupsList.count());
	StudentsGroup* sg=sy->groupsList.at(groupsListWidget->currentRow());
	
	StudentsSubgroup* ss1=sg->subgroupsList.at(i);
	StudentsSubgroup* ss2=sg->subgroupsList.at(i-1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;

	subgroupsListWidget->item(i)->setText(s2);
	subgroupsListWidget->item(i-1)->setText(s1);
	
	sg->subgroupsList[i]=ss2;
	sg->subgroupsList[i-1]=ss1;

	gt.rules.addUndoPoint(tr("Moved the subgroup %1 up in the group %2, the year %3.").arg(ss1->name).arg(sg->name).arg(sy->name));

	subgroupsListWidget->setCurrentRow(i-1);
	subgroupChanged(/*i-1*/s1);
}

void SubgroupsForm::moveSubgroupDown()
{
	if(subgroupsListWidget->count()<=1)
		return;
	int i=subgroupsListWidget->currentRow();
	if(i<0 || i>=subgroupsListWidget->count())
		return;
	if(i==subgroupsListWidget->count()-1)
		return;
		
	QString s1=subgroupsListWidget->item(i)->text();
	QString s2=subgroupsListWidget->item(i+1)->text();
	
	assert(yearsListWidget->currentRow()>=0);
	assert(yearsListWidget->currentRow()<gt.rules.yearsList.count());
	StudentsYear* sy=gt.rules.yearsList.at(yearsListWidget->currentRow());
	
	assert(groupsListWidget->currentRow()>=0);
	assert(groupsListWidget->currentRow()<sy->groupsList.count());
	StudentsGroup* sg=sy->groupsList.at(groupsListWidget->currentRow());
	
	StudentsSubgroup* ss1=sg->subgroupsList.at(i);
	StudentsSubgroup* ss2=sg->subgroupsList.at(i+1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;

	subgroupsListWidget->item(i)->setText(s2);
	subgroupsListWidget->item(i+1)->setText(s1);
	
	sg->subgroupsList[i]=ss2;
	sg->subgroupsList[i+1]=ss1;

	gt.rules.addUndoPoint(tr("Moved the subgroup %1 down in the group %2, the year %3.").arg(ss1->name).arg(sg->name).arg(sy->name));

	subgroupsListWidget->setCurrentRow(i+1);
	subgroupChanged(/*i+1*/s1);
}

void SubgroupsForm::sortSubgroups()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);
	
	gt.rules.sortSubgroupsAlphabetically(yearName, groupName);
	
	gt.rules.addUndoPoint(tr("Sorted the subgroups in the group %1, the year %2.").arg(groupName).arg(yearName));

	groupChanged(groupName);
}

void SubgroupsForm::modifySubgroup()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	int q=subgroupsListWidget->currentRow();
	int valv=subgroupsListWidget->verticalScrollBar()->value();
	int valh=subgroupsListWidget->horizontalScrollBar()->value();

	if(subgroupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	QString subgroupName=subgroupsListWidget->currentItem()->text();
	int subgroupIndex=gt.rules.searchSubgroup(yearName, groupName, subgroupName);
	assert(subgroupIndex>=0);
	
	StudentsSet* studentsSet=gt.rules.searchStudentsSet(subgroupName);
	assert(studentsSet!=nullptr);
	int numberOfStudents=studentsSet->numberOfStudents;
	
	ModifyStudentsSubgroupForm form(this, yearName, groupName, subgroupName, numberOfStudents);
	setParentAndOtherThings(&form, this);
	form.exec();

	groupChanged(groupName);
	
	subgroupsListWidget->verticalScrollBar()->setValue(valv);
	subgroupsListWidget->horizontalScrollBar()->setValue(valh);

	if(q>=subgroupsListWidget->count())
		q=subgroupsListWidget->count()-1;
	if(q>=0)
		subgroupsListWidget->setCurrentRow(q);
	else
		subgroupTextEdit->setPlainText(QString(""));
}

void SubgroupsForm::activateStudents()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	if(subgroupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	
	QString subgroupName=subgroupsListWidget->currentItem()->text();
	int count=gt.rules.activateStudents(subgroupName);
	QMessageBox::information(this, tr("FET information"), tr("Activated a number of %1 activities").arg(count));

	if(count>0)
		gt.rules.addUndoPoint(tr("Activated the subgroup %1 (%2 activities).", "%2 is the number of activated activities").arg(subgroupName).arg(count));
}

void SubgroupsForm::deactivateStudents()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearName, groupName);
	assert(groupIndex>=0);

	if(subgroupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	
	QString subgroupName=subgroupsListWidget->currentItem()->text();
	int count=gt.rules.deactivateStudents(subgroupName);
	QMessageBox::information(this, tr("FET information"), tr("Deactivated a number of %1 activities").arg(count));

	if(count>0)
		gt.rules.addUndoPoint(tr("Deactivated the subgroup %1 (%2 activities).", "%2 is the number of deactivated activities").arg(subgroupName).arg(count));
}

void SubgroupsForm::comments()
{
	int ind=subgroupsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	
	QString subgroupName=subgroupsListWidget->currentItem()->text();
	
	StudentsSet* studentsSet=gt.rules.searchStudentsSet(subgroupName);
	assert(studentsSet!=nullptr);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Students subgroup comments"));
	
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
	
	QTextEdit* commentsPT=new QTextEdit();
	commentsPT->setPlainText(studentsSet->comments);
	commentsPT->selectAll();
	commentsPT->setFocus();
	
	vl->addWidget(commentsPT);
	vl->addLayout(hl);
	
	getCommentsDialog.setLayout(vl);
	
	const QString settingsName=QString("StudentsSubgroupCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString ocs=studentsSet->comments;

		studentsSet->comments=commentsPT->toPlainText();
	
		gt.rules.addUndoPoint(tr("Changed the comments for the subgroup %1 from\n%2\nto\n%3.").arg(subgroupName).arg(ocs).arg(studentsSet->comments));

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		subgroupChanged(subgroupName);
	}
}

void SubgroupsForm::longName()
{
	int ind=subgroupsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	
	QString subgroupName=subgroupsListWidget->currentItem()->text();
	
	StudentsSet* studentsSet=gt.rules.searchStudentsSet(subgroupName);
	assert(studentsSet!=nullptr);

	QDialog getLongNameDialog(this);
	
	getLongNameDialog.setWindowTitle(tr("Subgroup long name"));
	
	QPushButton* okPB=new QPushButton(tr("OK"));
	okPB->setDefault(true);
	QPushButton* cancelPB=new QPushButton(tr("Cancel"));
	
	connect(okPB, &QPushButton::clicked, &getLongNameDialog, &QDialog::accept);
	connect(cancelPB, &QPushButton::clicked, &getLongNameDialog, &QDialog::reject);

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(okPB);
	hl->addWidget(cancelPB);
	
	QVBoxLayout* vl=new QVBoxLayout();
	
	QLineEdit* longNameLE=new QLineEdit();
	longNameLE->setText(studentsSet->longName);
	longNameLE->selectAll();
	longNameLE->setFocus();
	
	vl->addWidget(longNameLE);
	vl->addLayout(hl);
	
	getLongNameDialog.setLayout(vl);
	
	const QString settingsName=QString("SubgroupLongNameDialog");
	
	getLongNameDialog.resize(300, 200);
	centerWidgetOnScreen(&getLongNameDialog);
	restoreFETDialogGeometry(&getLongNameDialog, settingsName);
	
	int t=getLongNameDialog.exec();
	saveFETDialogGeometry(&getLongNameDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oln=studentsSet->longName;
	
		studentsSet->longName=longNameLE->text();
	
		gt.rules.addUndoPoint(tr("Changed the long name for the subgroup %1 from\n%2\nto\n%3.").arg(subgroupName).arg(oln).arg(studentsSet->longName));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		subgroupChanged(subgroupName);
	}
}

void SubgroupsForm::code()
{
	int ind=subgroupsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected subgroup"));
		return;
	}
	
	QString subgroupName=subgroupsListWidget->currentItem()->text();
	
	StudentsSet* studentsSet=gt.rules.searchStudentsSet(subgroupName);
	assert(studentsSet!=nullptr);

	QDialog getCodeDialog(this);
	
	getCodeDialog.setWindowTitle(tr("Subgroup code"));
	
	QPushButton* okPB=new QPushButton(tr("OK"));
	okPB->setDefault(true);
	QPushButton* cancelPB=new QPushButton(tr("Cancel"));
	
	connect(okPB, &QPushButton::clicked, &getCodeDialog, &QDialog::accept);
	connect(cancelPB, &QPushButton::clicked, &getCodeDialog, &QDialog::reject);

	QHBoxLayout* hl=new QHBoxLayout();
	hl->addStretch();
	hl->addWidget(okPB);
	hl->addWidget(cancelPB);
	
	QVBoxLayout* vl=new QVBoxLayout();
	
	QLineEdit* codeLE=new QLineEdit();
	codeLE->setText(studentsSet->code);
	codeLE->selectAll();
	codeLE->setFocus();
	
	vl->addWidget(codeLE);
	vl->addLayout(hl);
	
	getCodeDialog.setLayout(vl);
	
	const QString settingsName=QString("SubgroupCodeDialog");
	
	getCodeDialog.resize(300, 200);
	centerWidgetOnScreen(&getCodeDialog);
	restoreFETDialogGeometry(&getCodeDialog, settingsName);
	
	int t=getCodeDialog.exec();
	saveFETDialogGeometry(&getCodeDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oc=studentsSet->code;
	
		studentsSet->code=codeLE->text();
	
		gt.rules.addUndoPoint(tr("Changed the code for the subgroup %1 from\n%2\nto\n%3.").arg(subgroupName).arg(oc).arg(studentsSet->code));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		subgroupChanged(subgroupName);
	}
}
