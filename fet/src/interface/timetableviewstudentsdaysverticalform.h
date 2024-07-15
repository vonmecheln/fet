/***************************************************************************
                          timetableviewstudentsdaysverticalform.h  -  description
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

#ifndef TIMETABLEVIEWSTUDENTSDAYSVERTICALFORM_H
#define TIMETABLEVIEWSTUDENTSDAYSVERTICALFORM_H

#include <QResizeEvent>

#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>

#include "ui_timetableviewstudentsdaysverticalform_template.h"

class QColor; //by Marco Vassura

class TimetableViewStudentsDaysVerticalDelegate: public QStyledItemDelegate
{
	Q_OBJECT
	
public:
	int nRows; //The number of rows after which a line is drawn
	int nColumns;
	
public:
	TimetableViewStudentsDaysVerticalDelegate(QWidget* parent, int _nRows, int _nColumns): QStyledItemDelegate(parent){
		nRows=_nRows;
		nColumns=_nColumns;
	}
	
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

class TimetableViewStudentsDaysVerticalForm : public QDialog, public Ui::TimetableViewStudentsDaysVerticalForm_template
{
	Q_OBJECT

private:
	QAbstractItemDelegate* oldItemDelegate;
	TimetableViewStudentsDaysVerticalDelegate* newItemDelegate;

public:
	TimetableViewStudentsDaysVerticalForm(QWidget* parent);
	void newTimetableGenerated();
	~TimetableViewStudentsDaysVerticalForm();

	void lock(bool lockTime, bool lockSpace);
	
	void resizeRowsAfterShow();

	void detailActivity(QTableWidgetItem* item);

public slots:
	void lockTime();
	void lockSpace();
	void lockTimeSpace();
	void updateStudentsTimetableTable();

	void yearChanged(const QString& yearName);
	void groupChanged(const QString& groupName);
	void subgroupChanged(const QString& subgroupName);

	void currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);

	void help();
	
	void shownComboBoxChanged();

protected:
	void resizeEvent(QResizeEvent* event);
	QColor stringToColor(const QString& s); //by Marco Vassura
};

#endif
