/***************************************************************************
                          addconstraintteachernotavailabletimesform.cpp  -  description
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

#include "longtextmessagebox.h"

#include "addconstraintteachernotavailabletimesform.h"
#include "timeconstraint.h"

#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <QBrush>
#include <QColor>
#include <QPalette>

#define YES		(QString("X"))
#define NO		(QString(" "))

AddConstraintTeacherNotAvailableTimesForm::AddConstraintTeacherNotAvailableTimesForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintTeacherNotAvailableTimesForm::addCurrentConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintTeacherNotAvailableTimesForm::close);
	connect(notAllowedTimesTable, &QTableWidget::itemClicked, this, &AddConstraintTeacherNotAvailableTimesForm::itemClicked);
	connect(helpPushButton, &QPushButton::clicked, this, &AddConstraintTeacherNotAvailableTimesForm::help);
	connect(setAllAvailablePushButton, &QPushButton::clicked, this, &AddConstraintTeacherNotAvailableTimesForm::setAllAvailable);
	connect(setAllNotAvailablePushButton, &QPushButton::clicked, this, &AddConstraintTeacherNotAvailableTimesForm::setAllNotAvailable);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp1=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp1);

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

	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			QTableWidgetItem* item=new QTableWidgetItem(NO);
			item->setTextAlignment(Qt::AlignCenter);
			item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
			colorItem(item);
			if(SHOW_TOOLTIPS_FOR_CONSTRAINTS_WITH_TABLES)
				item->setToolTip(gt.rules.daysOfTheWeek[j]+QString("\n")+gt.rules.hoursOfTheDay[i]);
			notAllowedTimesTable->setItem(i, j, item);
		}
	
	notAllowedTimesTable->resizeRowsToContents();

	connect(notAllowedTimesTable->horizontalHeader(), &QHeaderView::sectionClicked, this, &AddConstraintTeacherNotAvailableTimesForm::horizontalHeaderClicked);
	connect(notAllowedTimesTable->verticalHeader(), &QHeaderView::sectionClicked, this, &AddConstraintTeacherNotAvailableTimesForm::verticalHeaderClicked);
	
	notAllowedTimesTable->setSelectionMode(QAbstractItemView::NoSelection);

	setStretchAvailabilityTableNicely(notAllowedTimesTable);

	connect(notAllowedTimesTable, &QTableWidget::cellEntered, this, &AddConstraintTeacherNotAvailableTimesForm::cellEntered);
	notAllowedTimesTable->setMouseTracking(true);
}

AddConstraintTeacherNotAvailableTimesForm::~AddConstraintTeacherNotAvailableTimesForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeacherNotAvailableTimesForm::colorItem(QTableWidgetItem* item)
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

void AddConstraintTeacherNotAvailableTimesForm::horizontalHeaderClicked(int col)
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

void AddConstraintTeacherNotAvailableTimesForm::verticalHeaderClicked(int row)
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

void AddConstraintTeacherNotAvailableTimesForm::cellEntered(int row, int col)
{
	highlightOnCellEntered(notAllowedTimesTable, row, col);
}

void AddConstraintTeacherNotAvailableTimesForm::setAllAvailable()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			notAllowedTimesTable->item(i, j)->setText(NO);
			colorItem(notAllowedTimesTable->item(i,j));
		}
}

void AddConstraintTeacherNotAvailableTimesForm::setAllNotAvailable()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			notAllowedTimesTable->item(i, j)->setText(YES);
			colorItem(notAllowedTimesTable->item(i,j));
		}
}

void AddConstraintTeacherNotAvailableTimesForm::updateTeachersComboBox(){
	teachersComboBox->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* tch=gt.rules.teachersList[i];
		teachersComboBox->addItem(tch->name);
	}
}

void AddConstraintTeacherNotAvailableTimesForm::itemClicked(QTableWidgetItem* item)
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

void AddConstraintTeacherNotAvailableTimesForm::addCurrentConstraint()
{
	TimeConstraint *ctr=nullptr;

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

	QList<int> days;
	QList<int> hours;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			if(notAllowedTimesTable->item(i, j)->text()==YES){
				days.append(j);
				hours.append(i);
			}

	ctr=new ConstraintTeacherNotAvailableTimes(weight, teacher_name, days, hours);

	bool tmp2=gt.rules.addTimeConstraint(ctr);
	if(tmp2){
		LongTextMessageBox::information(this, tr("FET information"),
			tr("Constraint added:")+"\n\n"+ctr->getDetailedDescription(gt.rules));

		gt.rules.addUndoPoint(tr("Added the constraint:\n\n%1").arg(ctr->getDetailedDescription(gt.rules)));
	}
	else{
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - there must be another constraint of this "
			  "type referring to the same teacher. Please edit that one"));
		delete ctr;
	}
}

void AddConstraintTeacherNotAvailableTimesForm::help()
{
	QString s;
	
	s=tr("This constraint does not induce gaps for teachers. If a teacher has activities"
	 " before and after a not available period, gaps will not be counted.");
	
	s+="\n\n";
	
	s+=tr("If you really need to use weight under 100%, you can use activities preferred times with"
	 " only the teacher specified, but this might generate problems, as possible gaps will be"
	 " counted and you may obtain an impossible timetable.");

	QMessageBox::information(this, tr("FET help"), s);
}
