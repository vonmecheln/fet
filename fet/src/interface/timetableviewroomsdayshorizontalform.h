/***************************************************************************
                          timetableviewroomsdayshorizontalform.h  -  description
                             -------------------
    begin                : Wed May 14 2003
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

#ifndef TIMETABLEVIEWROOMSDAYSHORIZONTALFORM_H
#define TIMETABLEVIEWROOMSDAYSHORIZONTALFORM_H

#include <QResizeEvent>

#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>

#include "ui_timetableviewroomsdayshorizontalform_template.h"

class QColor; //by Marco Vassura

class QColor; //by Marco Vassura

class TimetableViewRoomsDaysHorizontalDelegate: public QStyledItemDelegate
{
	Q_OBJECT
	
	QTableView* tableView;
	
public:
	int nRows; //The number of rows after which a line is drawn
	int nColumns;
	
public:
	TimetableViewRoomsDaysHorizontalDelegate(QWidget* parent, QTableView* _tableView, int _nRows, int _nColumns): QStyledItemDelegate(parent){
		tableView=_tableView;
		nRows=_nRows;
		nColumns=_nColumns;
	}
	
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

class TimetableViewRoomsDaysHorizontalForm : public QDialog, public Ui::TimetableViewRoomsDaysHorizontalForm_template
{
	Q_OBJECT
	
private:
	QAbstractItemDelegate* oldItemDelegate;
	TimetableViewRoomsDaysHorizontalDelegate* newItemDelegate;

public:
	TimetableViewRoomsDaysHorizontalForm(QWidget* parent);
	void newTimetableGenerated();
	~TimetableViewRoomsDaysHorizontalForm();

	void lock(bool lockTime, bool lockSpace);
	
	void resizeRowsAfterShow();

	void detailActivity(QTableWidgetItem* item);

public Q_SLOTS:
	void updateRoomsTimetableTable();

	void roomChanged(const QString& roomName);

	void currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);
	
	void lockTime();
	void lockSpace();
	void lockTimeSpace();
	void help();
	
protected:
	void resizeEvent(QResizeEvent* event);
	QColor stringToColor(const QString& s); //by Marco Vassura
};

#endif
