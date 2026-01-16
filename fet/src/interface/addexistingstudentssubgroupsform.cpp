/***************************************************************************
                          addexistingstudentssubgroupsform.cpp  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
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

	connect(cancelPushButton, &QPushButton::clicked, this, &AddExistingStudentsSubgroupsForm::cancel);
	connect(okPushButton, &QPushButton::clicked, this, &AddExistingStudentsSubgroupsForm::ok);
	connect(helpPushButton, &QPushButton::clicked, this, &AddExistingStudentsSubgroupsForm::help);
	connect(clearPushButton, &QPushButton::clicked, this, &AddExistingStudentsSubgroupsForm::clear);
	connect(allSubgroupsListWidget, &QListWidget::itemDoubleClicked, this, &AddExistingStudentsSubgroupsForm::addSubgroup);
	connect(selectedSubgroupsListWidget, &QListWidget::itemDoubleClicked, this, &AddExistingStudentsSubgroupsForm::removeSubgroup);

	connect(sortedCheckBox, &QCheckBox::toggled, this, &AddExistingStudentsSubgroupsForm::sortedToggled);
	
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
	for(StudentsSubgroup* sts : std::as_const(group->subgroupsList))
		ts.insert(sts);
	
	allSubgroupsList.clear();
	QSet<StudentsSubgroup*> allSubgroupsSet;
	for(StudentsGroup* stg : std::as_const(this->year->groupsList))
		for(StudentsSubgroup* sts : std::as_const(stg->subgroupsList))
			if(!allSubgroupsSet.contains(sts) && !ts.contains(sts)){
				allSubgroupsList.append(sts);
				allSubgroupsSet.insert(sts);
			}
	
	if(sortedCheckBox->isChecked())
		std::stable_sort(allSubgroupsList.begin(), allSubgroupsList.end(), alphabeticCompareFunction);
	
	allSubgroupsListWidget->clear();
	for(StudentsSubgroup* sts : std::as_const(allSubgroupsList))
		allSubgroupsListWidget->addItem(sts->name);
}

void AddExistingStudentsSubgroupsForm::ok()
{
	QStringList sl;
	int cnt=0;

	for(StudentsSubgroup* sts : std::as_const(selectedSubgroupsList)){
		cnt++;
		gt.rules.addSubgroupFast(year, group, sts);
		sl.append(sts->name);
	}
	
	if(cnt>0)
		gt.rules.addUndoPoint(tr("Added the existing subgroups %1 into the group %2 from the year %3.").arg(sl.join(translatedCommaSpace())).arg(group->name).arg(year->name));
	
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
