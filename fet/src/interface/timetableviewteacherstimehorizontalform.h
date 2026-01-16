/***************************************************************************
                          timetableviewteacherstimehorizontalform.h  -  description
                             -------------------
    begin                : 2017
    copyright            : (C) 2017 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef TIMETABLEVIEWTEACHERSTIMEHORIZONTALFORM_H
#define TIMETABLEVIEWTEACHERSTIMEHORIZONTALFORM_H

#include <QResizeEvent>

#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>

class QColor; //by Marco Vassura

#include "ui_timetableviewteacherstimehorizontalform_template.h"

class TimetableViewTeachersTimeHorizontalDelegate: public QStyledItemDelegate
{
	Q_OBJECT
	
	QTableView* tableView;
	
public:
	int nRows;
	int nColumns; //The number of columns after which a line is drawn.
	
public:
	TimetableViewTeachersTimeHorizontalDelegate(QWidget* parent, QTableView* _tableView, int _nRows, int _nColumns): QStyledItemDelegate(parent){
		tableView=_tableView;
		nRows=_nRows;
		nColumns=_nColumns;
	}
	
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

class TimetableViewTeachersTimeHorizontalForm : public QDialog, public Ui::TimetableViewTeachersTimeHorizontalForm_template
{
	Q_OBJECT
	
private:
	int initialRecommendedHeight;

	QAbstractItemDelegate* oldItemDelegate;
	TimetableViewTeachersTimeHorizontalDelegate* newItemDelegate;

public:
	TimetableViewTeachersTimeHorizontalForm(QWidget* parent);
	void newTimetableGenerated();
	~TimetableViewTeachersTimeHorizontalForm();
	
	void lock(bool lockTime, bool lockSpace);
	
	void resizeRowsAfterShow();

	void detailActivity(QTableWidgetItem* item);

public Q_SLOTS:
	void lockTime();
	void lockSpace();
	void lockTimeSpace();

	void lockDays();
	void unlockDays();
	void unlockAllDays();

	void updateTeachersTimetableTable();

	void currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);

	void help();
	
	void widthSpinBoxValueChanged();
	void heightSpinBoxValueChanged();

protected:
//	void resizeEvent(QResizeEvent* event);
	QColor stringToColor(const QString& s); //by Marco Vassura
};

#endif
