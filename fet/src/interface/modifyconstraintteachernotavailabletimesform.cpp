/***************************************************************************
                          modifyconstraintteachernotavailabletimesform.cpp  -  description
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

#include <Qt>

#include <QMessageBox>

#include "modifyconstraintteachernotavailabletimesform.h"
#include "timeconstraint.h"

#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <QBrush>
#include <QColor>
#include <QPalette>

#define YES		(QString("X"))
#define NO		(QString(" "))

ModifyConstraintTeacherNotAvailableTimesForm::ModifyConstraintTeacherNotAvailableTimesForm(QWidget* parent, ConstraintTeacherNotAvailableTimes* ctr): QDialog(parent)
{
	setupUi(this);

	okPushButton->setDefault(true);

	connect(okPushButton, &QPushButton::clicked, this, &ModifyConstraintTeacherNotAvailableTimesForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &ModifyConstraintTeacherNotAvailableTimesForm::cancel);
	connect(notAllowedTimesTable, &QTableWidget::itemClicked, this, &ModifyConstraintTeacherNotAvailableTimesForm::itemClicked);
	connect(setAllAvailablePushButton, &QPushButton::clicked, this, &ModifyConstraintTeacherNotAvailableTimesForm::setAllAvailable);
	connect(setAllNotAvailablePushButton, &QPushButton::clicked, this, &ModifyConstraintTeacherNotAvailableTimesForm::setAllNotAvailable);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);
	
	this->_ctr=ctr;
	
	weightLineEdit->setText(CustomFETString::number(ctr->weightPercentage));
	
	updateTeachersComboBox();

	notAllowedTimesTable->setRowCount(gt.rules.nHoursPerDay);
	notAllowedTimesTable->setColumnCount(gt.rules.nDaysPerWeek);

	for(int j=0; j<gt.rules.nDaysPerWeek; j++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.daysOfTheWeek[j]);
		notAllowedTimesTable->setHorizontalHeaderItem(j, item);
	}
	for(int i=0; i<gt.rules.nHoursPerDay; i++){
		QTableWidgetItem* item=new QTableWidgetItem(gt.rules.hoursOfTheDay[i]);
		notAllowedTimesTable->setVerticalHeaderItem(i, item);
	}

	Matrix2D<bool> currentMatrix;
	currentMatrix.resize(gt.rules.nHoursPerDay, gt.rules.nDaysPerWeek);

	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			currentMatrix[i][j]=false;
	assert(ctr->days.count()==ctr->hours.count());
	for(int k=0; k<ctr->days.count(); k++){
		if(ctr->hours.at(k)==-1 || ctr->days.at(k)==-1)
			assert(0);
		int i=ctr->hours.at(k);
		int j=ctr->days.at(k);
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
			notAllowedTimesTable->setItem(i, j, item);

			if(!currentMatrix[i][j])
				item->setText(NO);
			else
				item->setText(YES);
				
			colorItem(item);
		}
		
	notAllowedTimesTable->resizeRowsToContents();

	connect(notAllowedTimesTable->horizontalHeader(), &QHeaderView::sectionClicked, this, &ModifyConstraintTeacherNotAvailableTimesForm::horizontalHeaderClicked);
	connect(notAllowedTimesTable->verticalHeader(), &QHeaderView::sectionClicked, this, &ModifyConstraintTeacherNotAvailableTimesForm::verticalHeaderClicked);
	
	notAllowedTimesTable->setSelectionMode(QAbstractItemView::NoSelection);
	
	setStretchAvailabilityTableNicely(notAllowedTimesTable);

	connect(notAllowedTimesTable, &QTableWidget::cellEntered, this, &ModifyConstraintTeacherNotAvailableTimesForm::cellEntered);
	notAllowedTimesTable->setMouseTracking(true);
}

ModifyConstraintTeacherNotAvailableTimesForm::~ModifyConstraintTeacherNotAvailableTimesForm()
{
	saveFETDialogGeometry(this);
}

void ModifyConstraintTeacherNotAvailableTimesForm::colorItem(QTableWidgetItem* item)
{
	if(USE_GUI_COLORS){
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
		if(item->text()==NO)
			item->setBackground(QBrush(QColorConstants::DarkGreen));
		else
			item->setBackground(QBrush(QColorConstants::DarkRed));
		item->setForeground(QBrush(QColorConstants::LightGray));
#else
		if(item->text()==NO)
			item->setBackground(QBrush(Qt::darkGreen));
		else
			item->setBackground(QBrush(Qt::darkRed));
		item->setForeground(QBrush(Qt::lightGray));
#endif
	}
}

void ModifyConstraintTeacherNotAvailableTimesForm::horizontalHeaderClicked(int col)
{
	highlightOnHorizontalHeaderClicked(notAllowedTimesTable, col);

	if(col>=0 && col<gt.rules.nDaysPerWeek){
		QString s=notAllowedTimesTable->item(0, col)->text();
		if(s==YES)
			s=NO;
		else{
			assert(s==NO);
			s=YES;
		}

		for(int row=0; row<gt.rules.nHoursPerDay; row++){
			notAllowedTimesTable->item(row, col)->setText(s);
			colorItem(notAllowedTimesTable->item(row,col));
		}
	}
}

void ModifyConstraintTeacherNotAvailableTimesForm::verticalHeaderClicked(int row)
{
	highlightOnVerticalHeaderClicked(notAllowedTimesTable, row);

	if(row>=0 && row<gt.rules.nHoursPerDay){
		QString s=notAllowedTimesTable->item(row, 0)->text();
		if(s==YES)
			s=NO;
		else{
			assert(s==NO);
			s=YES;
		}
	
		for(int col=0; col<gt.rules.nDaysPerWeek; col++){
			notAllowedTimesTable->item(row, col)->setText(s);
			colorItem(notAllowedTimesTable->item(row,col));
		}
	}
}

void ModifyConstraintTeacherNotAvailableTimesForm::cellEntered(int row, int col)
{
	highlightOnCellEntered(notAllowedTimesTable, row, col);
}

void ModifyConstraintTeacherNotAvailableTimesForm::setAllAvailable()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			notAllowedTimesTable->item(i, j)->setText(NO);
			colorItem(notAllowedTimesTable->item(i,j));
		}
}

void ModifyConstraintTeacherNotAvailableTimesForm::setAllNotAvailable()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			notAllowedTimesTable->item(i, j)->setText(YES);
			colorItem(notAllowedTimesTable->item(i,j));
		}
}

void ModifyConstraintTeacherNotAvailableTimesForm::updateTeachersComboBox(){
	teachersComboBox->clear();
	int i=0, j=-1;
	for(int k=0; k<gt.rules.teachersList.size(); k++, i++){
		Teacher* tch=gt.rules.teachersList[k];
		teachersComboBox->addItem(tch->name);
		if(tch->name==this->_ctr->teacher)
			j=i;
	}
	assert(j>=0);
	teachersComboBox->setCurrentIndex(j);
}

void ModifyConstraintTeacherNotAvailableTimesForm::itemClicked(QTableWidgetItem* item)
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

void ModifyConstraintTeacherNotAvailableTimesForm::ok()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<100.0 || weight>100.0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight (percentage). It has to be 100"));
		return;
	}

	QString teacher_name=teachersComboBox->currentText();
	int teacher_ID=gt.rules.searchTeacher(teacher_name);
	if(teacher_ID<0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}
	
	if(this->_ctr->teacher!=teacher_name){
		QSet<ConstraintTeacherNotAvailableTimes*> cs=gt.rules.tnatHash.value(teacher_name, QSet<ConstraintTeacherNotAvailableTimes*>());
		if(!cs.isEmpty()){
			QMessageBox::warning(this, tr("FET information"),
			 tr("A constraint of this type exists for the same teacher - cannot proceed"));
			return;
		}
	}

	/*for(TimeConstraint* c : std::as_const(gt.rules.timeConstraintsList))
		if(c!=this->_ctr && c->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES){
			ConstraintTeacherNotAvailableTimes* cc=(ConstraintTeacherNotAvailableTimes*)c;
			if(cc->teacher==teacher_name){
				QMessageBox::warning(this, tr("FET information"),
				 tr("A constraint of this type exists for the same teacher - cannot proceed"));
				return;
			}
		}*/

	QString oldcs=this->_ctr->getDetailedDescription(gt.rules);

	this->_ctr->weightPercentage=weight;

	if(_ctr->teacher!=teacher_name){
		QString oldName=_ctr->teacher;
		QString newName=teacher_name;

		QSet<ConstraintTeacherNotAvailableTimes*> cs=gt.rules.tnatHash.value(oldName, QSet<ConstraintTeacherNotAvailableTimes*>());
		assert(cs.count()==1);
		assert(cs.contains(_ctr));
		//cs.remove(_ctr);
		//gt.rules.tnatHash.insert(oldName, cs);
		gt.rules.tnatHash.remove(oldName);

		cs=gt.rules.tnatHash.value(newName, QSet<ConstraintTeacherNotAvailableTimes*>());
		//assert(!cs.contains(_ctr));
		assert(cs.isEmpty());
		cs.insert(_ctr);
		gt.rules.tnatHash.insert(newName, cs);
		
		this->_ctr->teacher=teacher_name;
	}

	QList<int> days;
	QList<int> hours;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			if(notAllowedTimesTable->item(i, j)->text()==YES){
				days.append(j);
				hours.append(i);
			}

	this->_ctr->days=days;
	this->_ctr->hours=hours;

	QString newcs=this->_ctr->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);
	
	this->close();
}

void ModifyConstraintTeacherNotAvailableTimesForm::cancel()
{
	this->close();
}
