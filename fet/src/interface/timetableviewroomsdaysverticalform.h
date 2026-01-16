/***************************************************************************
                          timetableviewroomsdaysverticalform.h  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef TIMETABLEVIEWROOMSDAYSVERTICALFORM_H
#define TIMETABLEVIEWROOMSDAYSVERTICALFORM_H

#include <QResizeEvent>

#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>

#include "ui_timetableviewroomsdaysverticalform_template.h"

class QColor; //by Marco Vassura

class QColor; //by Marco Vassura

class TimetableViewRoomsDaysVerticalDelegate: public QStyledItemDelegate
{
	Q_OBJECT
	
	QTableView* tableView;
	
public:
	int nRows; //The number of rows after which a line is drawn
	int nColumns;
	
public:
	TimetableViewRoomsDaysVerticalDelegate(QWidget* parent, QTableView* _tableView, int _nRows, int _nColumns): QStyledItemDelegate(parent){
		tableView=_tableView;
		nRows=_nRows;
		nColumns=_nColumns;
	}
	
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

class TimetableViewRoomsDaysVerticalForm : public QDialog, public Ui::TimetableViewRoomsDaysVerticalForm_template
{
	Q_OBJECT
	
private:
	QAbstractItemDelegate* oldItemDelegate;
	TimetableViewRoomsDaysVerticalDelegate* newItemDelegate;

public:
	TimetableViewRoomsDaysVerticalForm(QWidget* parent);
	void newTimetableGenerated();
	~TimetableViewRoomsDaysVerticalForm();

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
