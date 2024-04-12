/***************************************************************************
                          addconstraintteacherroomnotavailabletimesform.cpp  -  description
                             -------------------
    begin                : 2019
    copyright            : (C) 2019 by Liviu Lalescu
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

#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <QBrush>
#include <QColor>
#include <QPalette>

#include "longtextmessagebox.h"

#include "addconstraintteacherroomnotavailabletimesform.h"

#define YES		(QString("X"))
#define NO		(QString(" "))

AddConstraintTeacherRoomNotAvailableTimesForm::AddConstraintTeacherRoomNotAvailableTimesForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);

	addConstraintPushButton->setDefault(true);

	connect(addConstraintPushButton, &QPushButton::clicked, this, &AddConstraintTeacherRoomNotAvailableTimesForm::addConstraint);
	connect(closePushButton, &QPushButton::clicked, this, &AddConstraintTeacherRoomNotAvailableTimesForm::close);
	connect(notAllowedTimesTable, &QTableWidget::itemClicked, this, &AddConstraintTeacherRoomNotAvailableTimesForm::itemClicked);
	connect(setAllAvailablePushButton, &QPushButton::clicked, this, &AddConstraintTeacherRoomNotAvailableTimesForm::setAllAvailable);
	connect(setAllNotAvailablePushButton, &QPushButton::clicked, this, &AddConstraintTeacherRoomNotAvailableTimesForm::setAllNotAvailable);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	QSize tmp5=roomsComboBox->minimumSizeHint();
	Q_UNUSED(tmp5);

	QSize tmp6=teachersComboBox->minimumSizeHint();
	Q_UNUSED(tmp6);

	updateTeachersComboBox();
	updateRoomsComboBox();

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
	
	connect(notAllowedTimesTable->horizontalHeader(), &QHeaderView::sectionClicked, this, &AddConstraintTeacherRoomNotAvailableTimesForm::horizontalHeaderClicked);
	connect(notAllowedTimesTable->verticalHeader(), &QHeaderView::sectionClicked, this, &AddConstraintTeacherRoomNotAvailableTimesForm::verticalHeaderClicked);
	
	notAllowedTimesTable->setSelectionMode(QAbstractItemView::NoSelection);

	setStretchAvailabilityTableNicely(notAllowedTimesTable);

	connect(notAllowedTimesTable, &QTableWidget::cellEntered, this, &AddConstraintTeacherRoomNotAvailableTimesForm::cellEntered);
	notAllowedTimesTable->setMouseTracking(true);
}

AddConstraintTeacherRoomNotAvailableTimesForm::~AddConstraintTeacherRoomNotAvailableTimesForm()
{
	saveFETDialogGeometry(this);
}

void AddConstraintTeacherRoomNotAvailableTimesForm::colorItem(QTableWidgetItem* item)
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

void AddConstraintTeacherRoomNotAvailableTimesForm::horizontalHeaderClicked(int col)
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
			/*QString s=notAllowedTimesTable->text(row, col);
			if(s==YES)
				s=NO;
			else{
				assert(s==NO);
				s=YES;
			}*/
			notAllowedTimesTable->item(row, col)->setText(s);
			colorItem(notAllowedTimesTable->item(row,col));
		}
	}
}

void AddConstraintTeacherRoomNotAvailableTimesForm::verticalHeaderClicked(int row)
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
			/*QString s=notAllowedTimesTable->text(row, col);
			if(s==YES)
				s=NO;
			else{
				assert(s==NO);
				s=YES;
			}*/
			notAllowedTimesTable->item(row, col)->setText(s);
			colorItem(notAllowedTimesTable->item(row,col));
		}
	}
}

void AddConstraintTeacherRoomNotAvailableTimesForm::cellEntered(int row, int col)
{
	highlightOnCellEntered(notAllowedTimesTable, row, col);
}

void AddConstraintTeacherRoomNotAvailableTimesForm::setAllAvailable()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			notAllowedTimesTable->item(i, j)->setText(NO);
			colorItem(notAllowedTimesTable->item(i,j));
		}
}

void AddConstraintTeacherRoomNotAvailableTimesForm::setAllNotAvailable()
{
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			notAllowedTimesTable->item(i, j)->setText(YES);
			colorItem(notAllowedTimesTable->item(i,j));
		}
}

void AddConstraintTeacherRoomNotAvailableTimesForm::updateTeachersComboBox()
{
	teachersComboBox->clear();
	for(int i=0; i<gt.rules.teachersList.size(); i++){
		Teacher* teacher=gt.rules.teachersList[i];
		teachersComboBox->addItem(teacher->name);
	}
}

void AddConstraintTeacherRoomNotAvailableTimesForm::updateRoomsComboBox()
{
	roomsComboBox->clear();
	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* room=gt.rules.roomsList[i];
		//roomsComboBox->addItem(room->getDescription());
		roomsComboBox->addItem(room->name);
	}
}

void AddConstraintTeacherRoomNotAvailableTimesForm::itemClicked(QTableWidgetItem* item)
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

void AddConstraintTeacherRoomNotAvailableTimesForm::addConstraint()
{
	SpaceConstraint *ctr=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid weight"));
		return;
	}

	int tch=teachersComboBox->currentIndex();
	if(tch<0 || teachersComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid teacher"));
		return;
	}
	Teacher* teacher=gt.rules.teachersList.at(tch);

	int i=roomsComboBox->currentIndex();
	if(i<0 || roomsComboBox->count()<=0){
		QMessageBox::warning(this, tr("FET information"),
			tr("Invalid room"));
		return;
	}
	Room* room=gt.rules.roomsList.at(i);

	QList<int> days;
	QList<int> hours;
	for(int j=0; j<gt.rules.nDaysPerWeek; j++)
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			if(notAllowedTimesTable->item(i, j)->text()==YES){
				days.append(j);
				hours.append(i);
			}

	ctr=new ConstraintTeacherRoomNotAvailableTimes(weight, teacher->name, room->name, days, hours);

	bool tmp2=gt.rules.addSpaceConstraint(ctr);
	if(tmp2){
		QString s=tr("Constraint added:");
		s+="\n\n";
		s+=ctr->getDetailedDescription(gt.rules);
		LongTextMessageBox::information(this, tr("FET information"), s);

		gt.rules.addUndoPoint(tr("Added the constraint:\n\n%1").arg(ctr->getDetailedDescription(gt.rules)));
	}
	else{
		/*QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - there must be another constraint of this "
			  "type referring to the same room. Please edit that one"));*/
		QMessageBox::warning(this, tr("FET information"),
			tr("Constraint NOT added - please report error"));
		delete ctr;
	}
}
