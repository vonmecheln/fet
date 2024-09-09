/***************************************************************************
                          modifyconstraintteachermaxactivitytagsperdayfromsetform.cpp  -  description
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

#include "modifyconstraintteachermaxactivitytagsperdayfromsetform.h"
#include "timeconstraint.h"

ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm::ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm(QWidget* parent, ConstraintTeacherMaxActivityTagsPerDayFromSet* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm::cancel);

	connect(allActivityTagsListWidget, &QListWidget::itemDoubleClicked, this, &ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm::addActivityTag);
	connect(selectedActivityTagsListWidget, &QListWidget::itemDoubleClicked, this, &ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm::removeActivityTag);

	connect(clearActivityTagsPushButton, &QPushButton::clicked, this, &ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm::clearActivityTags);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	teachersComboBox->clear();
	int i=0, j=-1;
	for(int k=0; k<gt.rules.teachersList.size(); k++, i++){
		Teacher* tch=gt.rules.teachersList[k];
		teachersComboBox->addItem(tch->name);
		if(tch->name==this->_ctr->teacherName)
			j=i;
	}
	assert(j>=0);
	teachersComboBox->setCurrentIndex(j);
	
	maxActivityTagsSpinBox->setValue(this->_ctr->maxTags);
	
	allActivityTagsListWidget->clear();
	for(int i=0; i<gt.rules.activityTagsList.size(); i++){
		ActivityTag* at=gt.rules.activityTagsList[i];
		allActivityTagsListWidget->addItem(at->name);
	}
	
	selectedActivityTagsListWidget->clear();
	for(const QString& at : std::as_const(this->_ctr->tagsList))
		selectedActivityTagsListWidget->addItem(at);
}

ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm::~ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm::addActivityTag()
{
	if(allActivityTagsListWidget->currentRow()<0 || allActivityTagsListWidget->currentRow()>=allActivityTagsListWidget->count())
		return;
	
	for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
		if(selectedActivityTagsListWidget->item(i)->text()==allActivityTagsListWidget->currentItem()->text())
			return;
	
	selectedActivityTagsListWidget->addItem(allActivityTagsListWidget->currentItem()->text());
	selectedActivityTagsListWidget->setCurrentRow(selectedActivityTagsListWidget->count()-1);
}

void ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm::removeActivityTag()
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

void ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm::clearActivityTags()
{
	selectedActivityTagsListWidget->clear();
}

void ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
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

	QString teacher=teachersComboBox->currentText();
	assert(gt.rules.searchTeacher(teacher)>=0);

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;
	this->_ctr->teacherName=teacher;
	
	this->_ctr->maxTags=maxActivityTagsSpinBox->value();
	
	QStringList selectedActivityTagsList;
	for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
		selectedActivityTagsList.append(selectedActivityTagsListWidget->item(i)->text());
	this->_ctr->tagsList=selectedActivityTagsList;

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeacherMaxActivityTagsPerDayFromSetForm::cancel()
{
	this->close();
}
