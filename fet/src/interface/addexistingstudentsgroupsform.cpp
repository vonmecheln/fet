/***************************************************************************
                          addexistingstudentsgroupsform.cpp  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QMessageBox>

#include "longtextmessagebox.h"

#include "addexistingstudentsgroupsform.h"

#include "matrix.h"

#include <QListWidget>
#include <QAbstractItemView>
#include <QScrollBar>

#include <algorithm>

#include <QSettings>

extern const QString COMPANY;
extern const QString PROGRAM;

static bool alphabeticCompareFunction(StudentsGroup* a, const StudentsGroup* b)
{
	return a->name.localeAwareCompare(b->name)<0;
}

AddExistingStudentsGroupsForm::AddExistingStudentsGroupsForm(QWidget* parent, StudentsYear* y): QDialog(parent)
{
	setupUi(this);
	
	this->year=y;
	
	yearGroupLabel->setText(tr("Year %1").arg(this->year->name));

	okPushButton->setDefault(true);
	
	allGroupsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedGroupsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));
	connect(clearPushButton, SIGNAL(clicked()), this, SLOT(clear()));
	connect(allGroupsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addGroup()));
	connect(selectedGroupsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeGroup()));

	connect(sortedCheckBox, SIGNAL(toggled(bool)), this, SLOT(sortedToggled()));
	
	sortedCheckBox->setChecked(false);
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/sorted-check-box")))
		sortedCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/sorted-check-box")).toBool());

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	selectedGroupsListWidget->clear();
	sortedToggled();
}

AddExistingStudentsGroupsForm::~AddExistingStudentsGroupsForm()
{
	saveFETDialogGeometry(this);

	QSettings settings(COMPANY, PROGRAM);

	settings.setValue(this->metaObject()->className()+QString("/sorted-check-box"), sortedCheckBox->isChecked());
}

void AddExistingStudentsGroupsForm::sortedToggled()
{
	QSet<StudentsGroup*> ts;
	for(StudentsGroup* stg : qAsConst(year->groupsList))
		ts.insert(stg);
	
	allGroupsList.clear();
	QSet<StudentsGroup*> allGroupsSet;
	for(StudentsYear* sty : qAsConst(gt.rules.yearsList))
		for(StudentsGroup* stg : qAsConst(sty->groupsList))
			if(!allGroupsSet.contains(stg) && !ts.contains(stg)){
				allGroupsList.append(stg);
				allGroupsSet.insert(stg);
			}
	
	if(sortedCheckBox->isChecked())
		std::stable_sort(allGroupsList.begin(), allGroupsList.end(), alphabeticCompareFunction);
	
	allGroupsListWidget->clear();
	for(StudentsGroup* stg : qAsConst(allGroupsList))
		allGroupsListWidget->addItem(stg->name);
}

void AddExistingStudentsGroupsForm::ok()
{
	for(StudentsGroup* stg : qAsConst(selectedGroupsList))
		gt.rules.addGroupFast(year, stg);
		
	this->accept();
}

void AddExistingStudentsGroupsForm::cancel()
{
	this->reject();
}

void AddExistingStudentsGroupsForm::addGroup()
{
	if(allGroupsListWidget->currentRow()<0)
		return;
	int tmp=allGroupsListWidget->currentRow();
	
	StudentsGroup* stg=allGroupsList.at(tmp);
	if(!selectedGroupsList.contains(stg)){
		selectedGroupsList.append(stg);
		selectedGroupsListWidget->addItem(stg->name);
	}
}

void AddExistingStudentsGroupsForm::removeGroup()
{
	if(selectedGroupsListWidget->currentRow()<0 || selectedGroupsListWidget->count()<=0)
		return;
	int tmp=selectedGroupsListWidget->currentRow();
	
	selectedGroupsList.removeAt(tmp);
	
	selectedGroupsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedGroupsListWidget->takeItem(tmp);
	delete item;
	if(tmp<selectedGroupsListWidget->count())
		selectedGroupsListWidget->setCurrentRow(tmp);
	else
		selectedGroupsListWidget->setCurrentRow(selectedGroupsListWidget->count()-1);
}

void AddExistingStudentsGroupsForm::clear()
{
	selectedGroupsList.clear();
	selectedGroupsListWidget->clear();
}

void AddExistingStudentsGroupsForm::help()
{
	QString s;
	
	s=tr("You can create overlapping students years with this feature, easier than by entering each students group's name manually more times.");
	s+=QString(" ");
	s+=tr("A group must be created firstly by adding it manually in a year, but then to add it in other years you can use this simple dialog.");

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
