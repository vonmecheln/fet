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

#include "addstudentsgroupform.h"
#include "addexistingstudentsgroupsform.h"
#include "modifystudentsgroupform.h"
#include "groupsform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "studentsset.h"

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

extern const QString COMPANY;
extern const QString PROGRAM;

extern bool students_schedule_ready;
extern bool rooms_buildings_schedule_ready;
extern bool teachers_schedule_ready;

GroupsForm::GroupsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	groupTextEdit->setReadOnly(true);
	
	modifyGroupPushButton->setDefault(true);

	yearsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	groupsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(yearsListWidget, &QListWidget::currentTextChanged, this, &GroupsForm::yearChanged);
	connect(addGroupPushButton, &QPushButton::clicked, this, &GroupsForm::addGroup);
	connect(addExistingGroupsPushButton, &QPushButton::clicked, this, &GroupsForm::addExistingGroups);
	connect(removeGroupPushButton, &QPushButton::clicked, this, &GroupsForm::removeGroup);
	connect(purgeGroupPushButton, &QPushButton::clicked, this, &GroupsForm::purgeGroup);
	
	connect(closePushButton, &QPushButton::clicked, this, &GroupsForm::close);
	connect(groupsListWidget, &QListWidget::currentTextChanged, this, &GroupsForm::groupChanged);
	connect(modifyGroupPushButton, &QPushButton::clicked, this, &GroupsForm::modifyGroup);

	connect(moveGroupUpPushButton, &QPushButton::clicked, this, &GroupsForm::moveGroupUp);
	connect(moveGroupDownPushButton, &QPushButton::clicked, this, &GroupsForm::moveGroupDown);

	connect(sortGroupsPushButton, &QPushButton::clicked, this, &GroupsForm::sortGroups);
	connect(activateStudentsPushButton, &QPushButton::clicked, this, &GroupsForm::activateStudents);
	connect(deactivateStudentsPushButton, &QPushButton::clicked, this, &GroupsForm::deactivateStudents);
	connect(groupsListWidget, &QListWidget::itemDoubleClicked, this, &GroupsForm::modifyGroup);

	connect(longNamePushButton, &QPushButton::clicked, this, &GroupsForm::longName);
	connect(codePushButton, &QPushButton::clicked, this, &GroupsForm::code);
	connect(commentsPushButton, &QPushButton::clicked, this, &GroupsForm::comments);

	if(SHORTCUT_PLUS){
		QShortcut* addShortcut=new QShortcut(QKeySequence(Qt::Key_Plus), this);
		connect(addShortcut, &QShortcut::activated, [=]{addGroupPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	addGroupPushButton->setToolTip(QString("+"));
	}
	if(SHORTCUT_M){
		QShortcut* modifyShortcut=new QShortcut(QKeySequence(Qt::Key_M), this);
		connect(modifyShortcut, &QShortcut::activated, [=]{modifyGroupPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	modifyGroupPushButton->setToolTip(QString("M"));
	}
	if(SHORTCUT_DELETE){
		QShortcut* removeShortcut=new QShortcut(QKeySequence::Delete, this);
		connect(removeShortcut, &QShortcut::activated, [=]{removeGroupPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	removeGroupPushButton->setToolTip(QString("⌦"));
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
		connect(upShortcut, &QShortcut::activated, [=]{moveGroupUpPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	moveGroupUpPushButton->setToolTip(QString("U"));
	}
	if(SHORTCUT_J){
		QShortcut* downShortcut=new QShortcut(QKeySequence(Qt::Key_J), this);
		connect(downShortcut, &QShortcut::activated, [=]{moveGroupDownPushButton->animateClick();});
		//if(SHOW_TOOL_TIPS)
		//	moveGroupDownPushButton->setToolTip(QString("J"));
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
	else
		groupsListWidget->clear();
}

GroupsForm::~GroupsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
}

void GroupsForm::addGroup()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchYear(yearName);
	assert(yearIndex>=0);

	AddStudentsGroupForm form(this, yearName);
	setParentAndOtherThings(&form, this);
	form.exec();

	yearChanged(yearsListWidget->currentItem()->text());
	
	int i=groupsListWidget->count()-1;
	if(i>=0)
		groupsListWidget->setCurrentRow(i);
}

void GroupsForm::addExistingGroups()
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
	
	AddExistingStudentsGroupsForm form(this, year);
	setParentAndOtherThings(&form, this);
	int t=form.exec();
	
	if(t==QDialog::Accepted){
		yearChanged(yearsListWidget->currentItem()->text());
	
		int i=groupsListWidget->count()-1;
		if(i>=0)
			groupsListWidget->setCurrentRow(i);
	}
	else{
		assert(t==QDialog::Rejected);
	}
}

void GroupsForm::removeGroup()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListWidget->currentItem()->text());
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearsListWidget->currentItem()->text(), groupName);
	assert(groupIndex>=0);

	QList<QString> yearsContainingGroup_List;
	//QSet<QString> yearsContainingGroup_Set;
	for(StudentsYear* year : std::as_const(gt.rules.yearsList))
		for(StudentsGroup* group : std::as_const(year->groupsList))
			if(group->name==groupName)
				yearsContainingGroup_List.append(year->name);
			
	assert(yearsContainingGroup_List.count()>=1);
	QString s;
	if(yearsContainingGroup_List.count()==1){
		s=tr("This group exists only in year %1. This means that"
		 " all the related activities and constraints will be removed. Do you want to continue?").arg(yearsListWidget->currentItem()->text());
	}
	else{
		s=tr("This group exists in more places, listed below. It will only be removed from the current year,"
		 " and the related activities and constraints will not be removed. Do you want to continue?");
		s+="\n";
		for(const QString& str : std::as_const(yearsContainingGroup_List))
			s+=QString("\n")+str;
	}
	
	int t=LongTextMessageBox::mediumConfirmation(this, tr("FET confirmation"), s,
		tr("Yes"), tr("No"), QString(), 0, 1);
	if(t==1)
		return;

	/*if(QMessageBox::warning( this, tr("FET"),
	 tr("Are you sure you want to delete group %1 and all related subgroups, activities and constraints?").arg(groupName),
	 tr("Yes"), tr("No"), QString(), 0, 1 ) == 1)
		return;*/
		
	bool tmp=gt.rules.removeGroup(yearsListWidget->currentItem()->text(), groupName);
	assert(tmp);
	if(tmp){
		gt.rules.addUndoPoint(tr("Removed the group %1 from the year %2.").arg(groupName).arg(yearsListWidget->currentItem()->text()));

		int q=groupsListWidget->currentRow();
		
		groupsListWidget->setCurrentRow(-1);
		QListWidgetItem* item;
		item=groupsListWidget->takeItem(q);
		delete item;
		
		if(q>=groupsListWidget->count())
			q=groupsListWidget->count()-1;
		if(q>=0)
			groupsListWidget->setCurrentRow(q);
		else
			groupTextEdit->setPlainText(QString(""));
	}

	/*if(gt.rules.searchStudentsSet(groupName)!=nullptr)
		QMessageBox::information( this, tr("FET"), tr("This group still exists into another year. "
			"The related subgroups, activities and constraints were not removed"));*/
}

void GroupsForm::purgeGroup()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListWidget->currentItem()->text());
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearsListWidget->currentItem()->text(), groupName);
	assert(groupIndex>=0);

	QList<QString> yearsContainingGroup_List;
	//QSet<QString> yearsContainingGroup_Set;
	for(StudentsYear* year : std::as_const(gt.rules.yearsList))
		for(StudentsGroup* group : std::as_const(year->groupsList))
			if(group->name==groupName)
				yearsContainingGroup_List.append(year->name);
			
	assert(yearsContainingGroup_List.count()>=1);
	QString s;
	if(yearsContainingGroup_List.count()==1)
		s=tr("This group exists only in year %1. All the related activities and constraints "
		 "will be removed. Do you want to continue?").arg(yearsListWidget->currentItem()->text());
	else{
		s=tr("This group exists in more places, listed below. It will be removed from all these places."
		 " All the related activities and constraints will be removed. Do you want to continue?");
		s+="\n";
		for(const QString& str : std::as_const(yearsContainingGroup_List))
			s+=QString("\n")+str;
	}
	
	int t=LongTextMessageBox::mediumConfirmation(this, tr("FET confirmation"), s,
		tr("Yes"), tr("No"), QString(), 0, 1);
	if(t==1)
		return;

	/*if(QMessageBox::warning( this, tr("FET"),
	 tr("Are you sure you want to delete group %1 and all related subgroups, activities and constraints?").arg(groupName),
	 tr("Yes"), tr("No"), QString(), 0, 1 ) == 1)
		return;*/

	bool tmp=gt.rules.purgeGroup(groupName);
	assert(tmp);
	if(tmp){
		gt.rules.addUndoPoint(tr("Removed the group %1 from everywhere.").arg(groupName));

		int q=groupsListWidget->currentRow();
		
		groupsListWidget->setCurrentRow(-1);
		QListWidgetItem* item;
		item=groupsListWidget->takeItem(q);
		delete item;
		
		if(q>=groupsListWidget->count())
			q=groupsListWidget->count()-1;
		if(q>=0)
			groupsListWidget->setCurrentRow(q);
		else
			groupTextEdit->setPlainText(QString(""));
	}

	/*if(gt.rules.searchStudentsSet(groupName)!=nullptr)
		QMessageBox::information( this, tr("FET"), tr("This group still exists into another year. "
			"The related subgroups, activities and constraints were not removed"));*/
}

void GroupsForm::yearChanged(const QString& yearName)
{
	int yearIndex=gt.rules.searchYear(yearName);
	if(yearIndex<0){
		groupsListWidget->clear();
		groupTextEdit->setPlainText(QString(""));
		return;
	}

	groupsListWidget->clear();

	StudentsYear* sty=gt.rules.yearsList.at(yearIndex);
	for(int i=0; i<sty->groupsList.size(); i++){
		StudentsGroup* stg=sty->groupsList[i];
		groupsListWidget->addItem(stg->name);
	}

	if(groupsListWidget->count()>0)
		groupsListWidget->setCurrentRow(0);
	else
		groupTextEdit->setPlainText(QString(""));
}

void GroupsForm::groupChanged(const QString& groupName)
{
	StudentsSet* ss=gt.rules.searchStudentsSet(groupName);
	if(ss==nullptr){
		groupTextEdit->setPlainText(QString(""));
		return;
	}
	StudentsGroup* sg=(StudentsGroup*)ss;
	groupTextEdit->setPlainText(sg->getDetailedDescriptionWithConstraints(gt.rules));
}

void GroupsForm::moveGroupUp()
{
	if(groupsListWidget->count()<=1)
		return;
	int i=groupsListWidget->currentRow();
	if(i<0 || i>=groupsListWidget->count())
		return;
	if(i==0)
		return;
		
	QString s1=groupsListWidget->item(i)->text();
	QString s2=groupsListWidget->item(i-1)->text();
	
	assert(yearsListWidget->currentRow()>=0);
	assert(yearsListWidget->currentRow()<gt.rules.yearsList.count());
	StudentsYear* sy=gt.rules.yearsList.at(yearsListWidget->currentRow());
	
	StudentsGroup* sg1=sy->groupsList.at(i);
	StudentsGroup* sg2=sy->groupsList.at(i-1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;

	groupsListWidget->item(i)->setText(s2);
	groupsListWidget->item(i-1)->setText(s1);
	
	sy->groupsList[i]=sg2;
	sy->groupsList[i-1]=sg1;
	
	gt.rules.addUndoPoint(tr("Moved the group %1 up in the year %2.").arg(s1).arg(sy->name));
	
	groupsListWidget->setCurrentRow(i-1);
	groupChanged(/*i-1*/s1);
}

void GroupsForm::moveGroupDown()
{
	if(groupsListWidget->count()<=1)
		return;
	int i=groupsListWidget->currentRow();
	if(i<0 || i>=groupsListWidget->count())
		return;
	if(i==groupsListWidget->count()-1)
		return;
		
	QString s1=groupsListWidget->item(i)->text();
	QString s2=groupsListWidget->item(i+1)->text();
	
	assert(yearsListWidget->currentRow()>=0);
	assert(yearsListWidget->currentRow()<gt.rules.yearsList.count());
	StudentsYear* sy=gt.rules.yearsList.at(yearsListWidget->currentRow());
	
	StudentsGroup* sg1=sy->groupsList.at(i);
	StudentsGroup* sg2=sy->groupsList.at(i+1);
	
	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	teachers_schedule_ready=false;
	students_schedule_ready=false;
	rooms_buildings_schedule_ready=false;

	groupsListWidget->item(i)->setText(s2);
	groupsListWidget->item(i+1)->setText(s1);
	
	sy->groupsList[i]=sg2;
	sy->groupsList[i+1]=sg1;

	gt.rules.addUndoPoint(tr("Moved the group %1 down in the year %2.").arg(s1).arg(sy->name));
	
	groupsListWidget->setCurrentRow(i+1);
	groupChanged(/*i+1*/s1);
}

void GroupsForm::sortGroups()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListWidget->currentItem()->text());
	assert(yearIndex>=0);
	
	gt.rules.sortGroupsAlphabetically(yearsListWidget->currentItem()->text());

	gt.rules.addUndoPoint(tr("Sorted the groups in the year %1.").arg(yearsListWidget->currentItem()->text()));

	yearChanged(yearsListWidget->currentItem()->text());
}

void GroupsForm::modifyGroup()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	
	QString yearName=yearsListWidget->currentItem()->text();
	
	int yearIndex=gt.rules.searchYear(yearsListWidget->currentItem()->text());
	assert(yearIndex>=0);
	
	int q=groupsListWidget->currentRow();
	int valv=groupsListWidget->verticalScrollBar()->value();
	int valh=groupsListWidget->horizontalScrollBar()->value();

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListWidget->currentItem()->text();
	int groupIndex=gt.rules.searchGroup(yearsListWidget->currentItem()->text(), groupName);
	assert(groupIndex>=0);

	StudentsSet* studentsSet=gt.rules.searchStudentsSet(groupName);
	assert(studentsSet!=nullptr);
	int numberOfStudents=studentsSet->numberOfStudents;
	
	ModifyStudentsGroupForm form(this, yearName, groupName, numberOfStudents);
	setParentAndOtherThings(&form, this);
	form.exec();

	yearChanged(yearsListWidget->currentItem()->text());
	
	groupsListWidget->verticalScrollBar()->setValue(valv);
	groupsListWidget->horizontalScrollBar()->setValue(valh);
	
	if(q>=groupsListWidget->count())
		q=groupsListWidget->count()-1;
	if(q>=0)
		groupsListWidget->setCurrentRow(q);
	else
		groupTextEdit->setPlainText(QString(""));
}

void GroupsForm::activateStudents()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListWidget->currentItem()->text());
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListWidget->currentItem()->text();
	int count=gt.rules.activateStudents(groupName);
	QMessageBox::information(this, tr("FET information"), tr("Activated a number of %1 activities").arg(count));

	if(count>0)
		gt.rules.addUndoPoint(tr("Activated the group %1 (%2 activities).", "%2 is the number of activated activities").arg(groupName).arg(count));
}

void GroupsForm::deactivateStudents()
{
	if(yearsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected year"));
		return;
	}
	int yearIndex=gt.rules.searchYear(yearsListWidget->currentItem()->text());
	assert(yearIndex>=0);

	if(groupsListWidget->currentRow()<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}

	QString groupName=groupsListWidget->currentItem()->text();
	int count=gt.rules.deactivateStudents(groupName);
	QMessageBox::information(this, tr("FET information"), tr("Deactivated a number of %1 activities").arg(count));
	
	if(count>0)
		gt.rules.addUndoPoint(tr("Deactivated group %1 (%2 activities).", "%2 is the number of deactivated activities").arg(groupName).arg(count));
}

void GroupsForm::comments()
{
	int ind=groupsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	
	QString groupName=groupsListWidget->currentItem()->text();
	
	StudentsSet* studentsSet=gt.rules.searchStudentsSet(groupName);
	assert(studentsSet!=nullptr);

	QDialog getCommentsDialog(this);
	
	getCommentsDialog.setWindowTitle(tr("Students group comments"));
	
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
	
	const QString settingsName=QString("StudentsGroupCommentsDialog");
	
	getCommentsDialog.resize(500, 320);
	centerWidgetOnScreen(&getCommentsDialog);
	restoreFETDialogGeometry(&getCommentsDialog, settingsName);
	
	int t=getCommentsDialog.exec();
	saveFETDialogGeometry(&getCommentsDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString ocs=studentsSet->comments;
	
		studentsSet->comments=commentsPT->toPlainText();
	
		gt.rules.addUndoPoint(tr("Changed the comments for the group %1 from\n%2\nto\n%3.").arg(groupName).arg(ocs).arg(studentsSet->comments));

		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		groupChanged(groupName);
	}
}

void GroupsForm::longName()
{
	int ind=groupsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	
	QString groupName=groupsListWidget->currentItem()->text();
	
	StudentsSet* studentsSet=gt.rules.searchStudentsSet(groupName);
	assert(studentsSet!=nullptr);

	QDialog getLongNameDialog(this);
	
	getLongNameDialog.setWindowTitle(tr("Group long name"));
	
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
	
	const QString settingsName=QString("GroupLongNameDialog");
	
	getLongNameDialog.resize(300, 200);
	centerWidgetOnScreen(&getLongNameDialog);
	restoreFETDialogGeometry(&getLongNameDialog, settingsName);
	
	int t=getLongNameDialog.exec();
	saveFETDialogGeometry(&getLongNameDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oln=studentsSet->longName;
	
		studentsSet->longName=longNameLE->text();
	
		gt.rules.addUndoPoint(tr("Changed the long name for the group %1 from\n%2\nto\n%3.").arg(groupName).arg(oln).arg(studentsSet->longName));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		groupChanged(groupName);
	}
}

void GroupsForm::code()
{
	int ind=groupsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected group"));
		return;
	}
	
	QString groupName=groupsListWidget->currentItem()->text();
	
	StudentsSet* studentsSet=gt.rules.searchStudentsSet(groupName);
	assert(studentsSet!=nullptr);

	QDialog getCodeDialog(this);
	
	getCodeDialog.setWindowTitle(tr("Group code"));
	
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
	
	const QString settingsName=QString("GroupCodeDialog");
	
	getCodeDialog.resize(300, 200);
	centerWidgetOnScreen(&getCodeDialog);
	restoreFETDialogGeometry(&getCodeDialog, settingsName);
	
	int t=getCodeDialog.exec();
	saveFETDialogGeometry(&getCodeDialog, settingsName);
	
	if(t==QDialog::Accepted){
		QString oc=studentsSet->code;
	
		studentsSet->code=codeLE->text();
	
		gt.rules.addUndoPoint(tr("Changed the code for the group %1 from\n%2\nto\n%3.").arg(groupName).arg(oc).arg(studentsSet->code));
	
		gt.rules.internalStructureComputed=false;
		setRulesModifiedAndOtherThings(&gt.rules);

		groupChanged(groupName);
	}
}
