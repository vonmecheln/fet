/***************************************************************************
                          addconstraintstudentssetmaxactivitytagsperdayfromsetform.cpp  -  description
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

#include <QMessageBox>

#include "longtextmessagebox.h"

#include "addconstraintstudentssetmaxactivitytagsperdayfromsetform.h"
#include "timeconstraint.h"

AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::close);

	connect(allActivityTagsListWidget, &QListWidget::itemDoubleClicked, this, &AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::addActivityTag);
	connect(selectedActivityTagsListWidget, &QListWidget::itemDoubleClicked, this, &AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::removeActivityTag);

	connect(clearActivityTagsPushButton, &QPushButton::clicked, this, &AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::clearActivityTags);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);

	populateStudentsComboBox(studentsComboBox);

	allActivityTagsListWidget->clear();
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* at=gt.rules.activityTagsList[i];
		allActivityTagsListWidget->addItem(at->name);
	}
	
	selectedActivityTagsListWidget->clear();
}

AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::~AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::addActivityTag()
{
	if(allActivityTagsListWidget->currentRow()<0 || allActivityTagsListWidget->currentRow()>=allActivityTagsListWidget->count())
		return;
	
	for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
		if(selectedActivityTagsListWidget->item(i)->text()==allActivityTagsListWidget->currentItem()->text())
			return;
	
	selectedActivityTagsListWidget->addItem(allActivityTagsListWidget->currentItem()->text());
	selectedActivityTagsListWidget->setCurrentRow(selectedActivityTagsListWidget->count()-1);
}

void AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::removeActivityTag()
{
	if(selectedActivityTagsListWidget->count()<=0 || selectedActivityTagsListWidget->currentRow()<0 ||
	 selectedActivityTagsListWidget->currentRow()>=selectedActivityTagsListWidget->count())
		return;
	
	int i=selectedActivityTagsListWidget->currentRow();
	selectedActivityTagsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedActivityTagsListWidget->takeItem(i);
	delete item;
	if(i<selectedActivityTagsListWidget->count())
		selectedActivityTagsListWidget->setCurrentRow(i);
	else
		selectedActivityTagsListWidget->setCurrentRow(selectedActivityTagsListWidget->count()-1);
}

void AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::clearActivityTags()
{
	selectedActivityTagsListWidget->clear();
}

void AddConstraintStudentsSetMaxActivityTagsPerDayFromSetForm::addCurrentConstraint()
{
	TimeConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET warning"),
			tr("Invalid weight (percentage)"));
		return;
	}
	if(weight!=100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage) - must be 100%"));
		return;
	}

	if(selectedActivityTagsListWidget->count()<2){
		QMessageBox::warning(this, tr("FET information"),
			tr("Please select at least two activity tags"));
		return;
	}

	QString students_name=studentsComboBox->currentText();
	StudentsSet* s=gt.rules.searchStudentsSet(students_name);
	if(s==nullptr){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid students set"));
		return;
	}
	
	QStringList selectedActivityTagsList;
	for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
		selectedActivityTagsList.append(selectedActivityTagsListWidget->item(i)->text());

	ctr=new ConstraintStudentsSetMaxActivityTagsPerDayFromSet(weight, students_name, maxActivityTagsSpinBox->value(), selectedActivityTagsList);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2){
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));

		gt.rules.addUndoPoint(tr("Added the constraint:\n\n%1").arg(ctr->getDetailedDescription(gt.rules)));
	}
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
