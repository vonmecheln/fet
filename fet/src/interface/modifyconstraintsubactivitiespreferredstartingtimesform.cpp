/***************************************************************************
                          modifyconstraintsubactivitiespreferredstartingtimesform.cpp  -  description
                             -------------------
    begin                : 2008
    copyright            : (C) 2008 by Liviu Lalescu
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

#include <Qt>

#include <QMessageBox>

#include "modifyconstraintsubactivitiespreferredstartingtimesform.h"
#include "timeconstraint.h"

#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <QBrush>
#include <QColor>
#include <QPalette>

#define YES		(QString(" "))
#define NO		(QString("X"))

ModifyConstraintSubactivitiesPreferredStartingTimesForm::ModifyConstraintSubactivitiesPreferredStartingTimesForm(QWidget* parent, ConstraintSubactivitiesPreferredStartingTimes* ctr): QDialog(parent)
{
	setupUi(this);

	int duration=ctr->duration;
	durationCheckBox->setChecked(duration>=1);
	durationSpinBox->setEnabled(duration>=1);
	durationSpinBox->setMinimum(1);
	durationSpinBox->setMaximum(gt.rules.nHoursPerDay);
	if(duration>=1)
		durationSpinBox->setValue(duration);
	else
		durationSpinBox->setValue(1);

	okPushButton->setDefault(true);

	connect(preferredTimesTable, &QTableWidget::itemClicked, this, &ModifyConstraintSubactivitiesPreferredStartingTimesForm::itemClicked);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintSubactivitiesPreferredStartingTimesForm::cancel);
	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintSubactivitiesPreferredStartingTimesForm::ok);
	connect(setAllAllowedPushButton, &QPushButton::clicked, this, &ModifyConstraintSubactivitiesPreferredStartingTimesForm::setAllSlotsAllowed);
	connect(setAllNotAllowedPushButton, &QPushButton::clicked, this, &ModifyConstraintSubactivitiesPreferredStartingTimesForm::setAllSlotsNotAllowed);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	QSize tmp2=studentsComboBox->minimumSizeHint();
	Q_UNUSED(tmp2);
	QSize tmp3=subjectsComboBox->minimumSizeHint();
	Q_UNUSED(tmp3);
	QSize tmp4=activityTagsComboBox->minimumSizeHint();
	Q_UNUSED(tmp4);

	this->_ctr=ctr;

	updateTeachersComboBox();
	updateStudentsComboBox(parent);
	updateSubjectsComboBox();
	updateActivityTagsComboBox();
	
	componentNumberSpinBox->setMinimum(1);
	componentNumberSpinBox->setMaximum(MAX_SPLIT_OF_AN_ACTIVITY);
	componentNumberSpinBox->setValue(this->_ctr->componentNumber);

	preferredTimesTable->setRowCount(gt.rules.nHoursPerDay);
	preferredTimesTable->setColumnCount(gt.rules.nDaysPerWeek);

	for(int j=0; j<gt.rules.nDaysPerWeek; j++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.daysOfTheWeek[j]);
		preferredTimesTable->setHorizontalHeaderItem(j, item);
	}
	for(int i=0; i<gt.rules.nHoursPerDay; i++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.hoursOfTheDay[i]);
		preferredTimesTable->setVerticalHeaderItem(i, item);
	}

	Matrix2D<bool> currentMatrix;
	currentMatrix.resize(gt.rules.nHoursPerDay, gt.rules.nDaysPerWeek);
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			currentMatrix[i][j]=false;
	for(int k=0; k<ctr->nPreferredStartingTimes_L; k++){
		if(ctr->hours_L[k]==-1 || ctr->days_L[k]==-1)
			assert(0);
		int i=ctr->hours_L[k];
		int j=ctr->days_L[k];
		if(i>=0 && i<gt.rules.nHoursPerDay && j>=0 && j<gt.rules.nDaysPerWeek)
			currentMatrix[i][j]=true;
	}

	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			QTableWidgetItem* item= new QTableWidgetItem();
			item->setTextAlignment(Qt::AlignCenter);
			item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			if(SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES)
				item->setToolTip(gt.rules.daysOfTheWeek[j]+QString("\n")+gt.rules.hoursOfTheDay[i]);
			preferredTimesTable->setItem(i, j, item);

			if(!currentMatrix[i][j])
				item->setText(NO);
			else
				item->setText(YES);
				
			colorItem(item);
		}
	
	preferredTimesTable->resizeRowsToContents();
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));

	connect(preferredTimesTable->horizontalHeader(), &QHeaderView::sectionClicked, this, &ModifyConstraintSubactivitiesPreferredStartingTimesForm::horizontalHeaderClicked);
	connect(preferredTimesTable->verticalHeader(), &QHeaderView::sectionClicked, this, &ModifyConstraintSubactivitiesPreferredStartingTimesForm::verticalHeaderClicked);

	preferredTimesTable->setSelectionMode(QAbstractItemView::NoSelection);
	
	setStretchAvailabilityTableNicely(preferredTimesTable);

	connect(preferredTimesTable, &QTableWidget::cellEntered, this, &ModifyConstraintSubactivitiesPreferredStartingTimesForm::cellEntered);
	preferredTimesTable->setMouseTracking(true);

	connect(durationCheckBox, &QCheckBox::toggled, this, &ModifyConstraintSubactivitiesPreferredStartingTimesForm::durationCheckBox_toggled);
	
	durationCheckBox_toggled();
}

ModifyConstraintSubactivitiesPreferredStartingTimesForm::~ModifyConstraintSubactivitiesPreferredStartingTimesForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintSubactivitiesPreferredStartingTimesForm::colorItem(QTableWidgetItem* item)
{
	if(USE_GUI_COLORS){
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
		if(item->text()==YES)
			item->setBackground(QBrush(QColorConstants::DarkGreen));
		else
			item->setBackground(QBrush(QColorConstants::DarkRed));
		item->setForeground(QBrush(QColorConstants::LightGray));
#else
		if(item->text()==YES)
			item->setBackground(QBrush(Qt::darkGreen));
		else
			item->setBackground(QBrush(Qt::darkRed));
		item->setForeground(QBrush(Qt::lightGray));
#endif
	}
}

void ModifyConstraintSubactivitiesPreferredStartingTimesForm::horizontalHeaderClicked(int col)
{
	highlightOnHorizontalHeaderClicked(preferredTimesTable, col);

	if(col>=0 && col<gt.rules.nDaysPerWeek){
		QString s=preferredTimesTable->item(0, col)->text();
		if(s==YES)
			s=NO;
		else{
			assert(s==NO);
			s=YES;
		}

		for(int row=0; row<gt.rules.nHoursPerDay; row++){
			preferredTimesTable->item(row, col)->setText(s);
			colorItem(preferredTimesTable->item(row,col));
		}
	}
}

void ModifyConstraintSubactivitiesPreferredStartingTimesForm::verticalHeaderClicked(int row)
{
	highlightOnVerticalHeaderClicked(preferredTimesTable, row);

	if(row>=0 && row<gt.rules.nHoursPerDay){
		QString s=preferredTimesTable->item(row, 0)->text();
		if(s==YES)
			s=NO;
		else{
			assert(s==NO);
			s=YES;
		}
	
		for(int col=0; col<gt.rules.nDaysPerWeek; col++){
			preferredTimesTable->item(row, col)->setText(s);
			colorItem(preferredTimesTable->item(row,col));
		}
	}
}

void ModifyConstraintSubactivitiesPreferredStartingTimesForm::cellEntered(int row, int col)
{
	highlightOnCellEntered(preferredTimesTable, row, col);
}

void ModifyConstraintSubactivitiesPreferredStartingTimesForm::setAllSlotsAllowed()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			preferredTimesTable->item(i, j)->setText(YES);
			colorItem(preferredTimesTable->item(i,j));
		}
}

void ModifyConstraintSubactivitiesPreferredStartingTimesForm::setAllSlotsNotAllowed()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			preferredTimesTable->item(i, j)->setText(NO);
			colorItem(preferredTimesTable->item(i,j));
		}
}

void ModifyConstraintSubactivitiesPreferredStartingTimesForm::itemClicked(QTableWidgetItem* item)
{
	QString s=item->text();
	if(s==YES)
		s=NO;
	else{
		assert(s==NO);
		s=YES;
	}
	item->setText(s);
	colorItem(item);
}

void ModifyConstraintSubactivitiesPreferredStartingTimesForm::updateTeachersComboBox(){
	int i=0, j=-1;
	teachersComboBox->clear();
	teachersComboBox->addItem("");
	if(this->_ctr->teacherName=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.teachersList.size(); k++){
		Teacher* t=gt.rules.teachersList[k];
		teachersComboBox->addItem(t->name);
		if(t->name==this->_ctr->teacherName)
			j=i;
		i++;
	}
	assert(j>=0);
	teachersComboBox->setCurrentIndex(j);
}

void ModifyConstraintSubactivitiesPreferredStartingTimesForm::updateStudentsComboBox(QWidget* parent){
	int j=populateStudentsComboBox(studentsComboBox, this->_ctr->studentsName, true);
	if(j<0)
		showWarningForInvisibleSubgroupConstraint(parent, this->_ctr->studentsName);
	else
		assert(j>=0);
	studentsComboBox->setCurrentIndex(j);
}

void ModifyConstraintSubactivitiesPreferredStartingTimesForm::updateSubjectsComboBox(){
	int i=0, j=-1;
	subjectsComboBox->clear();
	subjectsComboBox->addItem("");
	if(this->_ctr->subjectName=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.subjectsList.size(); k++){
		Subject* s=gt.rules.subjectsList[k];
		subjectsComboBox->addItem(s->name);
		if(s->name==this->_ctr->subjectName)
			j=i;
		i++;
	}
	assert(j>=0);
	subjectsComboBox->setCurrentIndex(j);
}

void ModifyConstraintSubactivitiesPreferredStartingTimesForm::updateActivityTagsComboBox(){
	int i=0, j=-1;
	activityTagsComboBox->clear();
	activityTagsComboBox->addItem("");
	if(this->_ctr->activityTagName=="")
		j=i;
	i++;
	for(int k=0; k<gt.rules.activityTagsList.size(); k++){
		ActivityTag* s=gt.rules.activityTagsList[k];
		activityTagsComboBox->addItem(s->name);
		if(s->name==this->_ctr->activityTagName)
			j=i;
		i++;
	}
	assert(j>=0);
	activityTagsComboBox->setCurrentIndex(j);
}

void ModifyConstraintSubactivitiesPreferredStartingTimesForm::ok()
{
	int duration=-1;
	if(durationCheckBox->isChecked()){
		assert(durationSpinBox->isEnabled());
		duration=durationSpinBox->value();
	}

	if(studentsComboBox->currentIndex()<0){
		showWarningCannotModifyConstraintInvisibleSubgroupConstraint(this, this->_ctr->studentsName);
		return;
	}
	
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage)"));
		return;
	}

	QString teacher=teachersComboBox->currentText();
	if(teacher!="")
		assert(gt.rules.searchTeacher(teacher)>=0);

	QString students=studentsComboBox->currentText();
	if(students!="")
		assert(gt.rules.searchStudentsSet(students)!=nullptr);

	QString subject=subjectsComboBox->currentText();
	if(subject!="")
		assert(gt.rules.searchSubject(subject)>=0);
		
	QString activityTag=activityTagsComboBox->currentText();
	if(activityTag!="")
		assert(gt.rules.searchActivityTag(activityTag)>=0);
		
	QList<int> days_L;
	QList<int> hours_L;
	int n=0;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			if(preferredTimesTable->item(i, j)->text()==YES){
				days_L.append(j);
				hours_L.append(i);
				n++;
			}

	if(n<=0){
		int t=QMessageBox::question(this, tr("FET question"),
		 tr("Warning: 0 slots selected. Are you sure?"),
		 QMessageBox::Yes, QMessageBox::Cancel);
		
		if(t==QMessageBox::Cancel)
				return;
	}

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;

	this->_ctr->componentNumber=componentNumberSpinBox->value();
	
	this->_ctr->teacherName=teacher;
	this->_ctr->studentsName=students;
	this->_ctr->subjectName=subject;
	this->_ctr->activityTagName=activityTag;
	this->_ctr->nPreferredStartingTimes_L=n;
	this->_ctr->days_L=days_L;
	this->_ctr->hours_L=hours_L;

	this->_ctr->duration=duration;

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintSubactivitiesPreferredStartingTimesForm::cancel()
{
	this->close();
}

void ModifyConstraintSubactivitiesPreferredStartingTimesForm::durationCheckBox_toggled()
{
	durationSpinBox->setEnabled(durationCheckBox->isChecked());
}

#undef YES
#undef NO
