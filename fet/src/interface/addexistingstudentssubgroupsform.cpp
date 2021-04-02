/***************************************************************************
                          addexistingstudentssubgroupsform.cpp  -  description
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

#include "addexistingstudentssubgroupsform.h"

#include "matrix.h"

#include <QListWidget>
#include <QAbstractItemView>
#include <QScrollBar>

#include <algorithm>

#include <QSettings>

extern const QString COMPANY;
extern const QString PROGRAM;

static bool alphabeticCompareFunction(StudentsSubgroup* a, const StudentsSubgroup* b)
{
	return a->name.localeAwareCompare(b->name)<0;
}

AddExistingStudentsSubgroupsForm::AddExistingStudentsSubgroupsForm(QWidget* parent, StudentsYear* y, StudentsGroup* g): QDialog(parent)
{
	setupUi(this);
	
	this->year=y;
	this->group=g;
	
	yearGroupLabel->setText(tr("Year %1, group %2").arg(this->year->name).arg(this->group->name));

	okPushButton->setDefault(true);
	
	allSubgroupsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	selectedSubgroupsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(helpPushButton, SIGNAL(clicked()), this, SLOT(help()));
	connect(clearPushButton, SIGNAL(clicked()), this, SLOT(clear()));
	connect(allSubgroupsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addSubgroup()));
	connect(selectedSubgroupsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeSubgroup()));

	connect(sortedCheckBox, SIGNAL(toggled(bool)), this, SLOT(sortedToggled()));
	
	sortedCheckBox->setChecked(false);
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/sorted-check-box")))
		sortedCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/sorted-check-box")).toBool());

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	selectedSubgroupsListWidget->clear();
	sortedToggled();
}

AddExistingStudentsSubgroupsForm::~AddExistingStudentsSubgroupsForm()
{
	saveFETDialogGeometry(this);

	QSettings settings(COMPANY, PROGRAM);

	settings.setValue(this->metaObject()->className()+QString("/sorted-check-box"), sortedCheckBox->isChecked());
}

void AddExistingStudentsSubgroupsForm::sortedToggled()
{
	QSet<StudentsSubgroup*> ts;
	for(StudentsSubgroup* sts : qAsConst(group->subgroupsList))
		ts.insert(sts);
	
	allSubgroupsList.clear();
	QSet<StudentsSubgroup*> allSubgroupsSet;
	for(StudentsGroup* stg : qAsConst(this->year->groupsList))
		for(StudentsSubgroup* sts : qAsConst(stg->subgroupsList))
			if(!allSubgroupsSet.contains(sts) && !ts.contains(sts)){
				allSubgroupsList.append(sts);
				allSubgroupsSet.insert(sts);
			}
	
	if(sortedCheckBox->isChecked())
		std::stable_sort(allSubgroupsList.begin(), allSubgroupsList.end(), alphabeticCompareFunction);
	
	allSubgroupsListWidget->clear();
	for(StudentsSubgroup* sts : qAsConst(allSubgroupsList))
		allSubgroupsListWidget->addItem(sts->name);
}

void AddExistingStudentsSubgroupsForm::ok()
{
	for(StudentsSubgroup* sts : qAsConst(selectedSubgroupsList))
		gt.rules.addSubgroupFast(year, group, sts);
		
	this->accept();
}

void AddExistingStudentsSubgroupsForm::cancel()
{
	this->reject();
}

void AddExistingStudentsSubgroupsForm::addSubgroup()
{
	if(allSubgroupsListWidget->currentRow()<0)
		return;
	int tmp=allSubgroupsListWidget->currentRow();
	
	StudentsSubgroup* sts=allSubgroupsList.at(tmp);
	if(!selectedSubgroupsList.contains(sts)){
		selectedSubgroupsList.append(sts);
		selectedSubgroupsListWidget->addItem(sts->name);
	}
}

void AddExistingStudentsSubgroupsForm::removeSubgroup()
{
	if(selectedSubgroupsListWidget->currentRow()<0 || selectedSubgroupsListWidget->count()<=0)
		return;
	int tmp=selectedSubgroupsListWidget->currentRow();
	
	selectedSubgroupsList.removeAt(tmp);
	
	selectedSubgroupsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedSubgroupsListWidget->takeItem(tmp);
	delete item;
	if(tmp<selectedSubgroupsListWidget->count())
		selectedSubgroupsListWidget->setCurrentRow(tmp);
	else
		selectedSubgroupsListWidget->setCurrentRow(selectedSubgroupsListWidget->count()-1);
}

void AddExistingStudentsSubgroupsForm::clear()
{
	selectedSubgroupsList.clear();
	selectedSubgroupsListWidget->clear();
}

void AddExistingStudentsSubgroupsForm::help()
{
	QString s;
	
	s=tr("You can create overlapping students groups with this feature, easier than by entering each students subgroup's name manually more times.");
	s+=QString(" ");
	s+=tr("A subgroup must be created firstly by adding it manually in a group, but then to add it in other groups from the same year you "
	 "can use this simple dialog.");

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
