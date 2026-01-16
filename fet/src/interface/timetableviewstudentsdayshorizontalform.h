/***************************************************************************
                          timetableviewstudentsdayshorizontalform.h  -  description
                             -------------------
    begin                : Tue Apr 22 2003
    copyright            : (C) 2003 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef TIMETABLEVIEWSTUDENTSDAYSHORIZONTALFORM_H
#define TIMETABLEVIEWSTUDENTSDAYSHORIZONTALFORM_H

#include <QResizeEvent>

#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>

#include "ui_timetableviewstudentsdayshorizontalform_template.h"

class QColor; //by Marco Vassura

class TimetableViewStudentsDaysHorizontalDelegate: public QStyledItemDelegate
{
	Q_OBJECT
	
	QTableView* tableView;

public:
	int nRows; //The number of rows after which a line is drawn
	int nColumns;
	
public:
	TimetableViewStudentsDaysHorizontalDelegate(QWidget* parent, QTableView* _tableView, int _nRows, int _nColumns): QStyledItemDelegate(parent){
		tableView=_tableView;
		nRows=_nRows;
		nColumns=_nColumns;
	}
	
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

class TimetableViewStudentsDaysHorizontalForm : public QDialog, public Ui::TimetableViewStudentsDaysHorizontalForm_template
{
	Q_OBJECT

private:
	QAbstractItemDelegate* oldItemDelegate;
	TimetableViewStudentsDaysHorizontalDelegate* newItemDelegate;

public:
	TimetableViewStudentsDaysHorizontalForm(QWidget* parent);
	void newTimetableGenerated();
	~TimetableViewStudentsDaysHorizontalForm();

	void lock(bool lockTime, bool lockSpace);
	
	void resizeRowsAfterShow();

	void detailActivity(QTableWidgetItem* item);

public Q_SLOTS:
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
