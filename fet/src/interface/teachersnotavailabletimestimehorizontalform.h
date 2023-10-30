/***************************************************************************
                          teachersnotavailabletimestimehorizontalform.h  -  description
                             -------------------
    begin                : 2023
    copyright            : (C) 2023 by Liviu Lalescu
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

#ifndef TEACHERSNOTAVAILABLETIMESTIMEHORIZONTALFORM_H
#define TEACHERSNOTAVAILABLETIMESTIMEHORIZONTALFORM_H

#include <QResizeEvent>

#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>

#include "ui_teachersnotavailabletimestimehorizontalform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class TeachersNotAvailableTimesTimeHorizontalDelegate: public QStyledItemDelegate
{
	Q_OBJECT

public:
	int nRows;
	int nColumns; //The number of columns after which a line is drawn.

public:
	TeachersNotAvailableTimesTimeHorizontalDelegate(QWidget* parent, int _nRows, int _nColumns): QStyledItemDelegate(parent){
		nRows=_nRows;
		nColumns=_nColumns;
	}

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

class TeachersNotAvailableTimesTimeHorizontalForm : public QDialog, public Ui::TeachersNotAvailableTimesTimeHorizontalForm_template
{
	Q_OBJECT
	
private:
	int initialRecommendedHeight;

	QAbstractItemDelegate* oldItemDelegate;
	TeachersNotAvailableTimesTimeHorizontalDelegate* newItemDelegate;

	Matrix3D<bool> tnaMatrix;
	Matrix1D<bool> inactiveConstraint;

public:
	TeachersNotAvailableTimesTimeHorizontalForm(QWidget* parent);
	~TeachersNotAvailableTimesTimeHorizontalForm();

	void colorItem(QTableWidgetItem* item);

public slots:
	void itemClicked(QTableWidgetItem* item);

	void ok();
	void cancel();

	void widthSpinBoxValueChanged();
	void heightSpinBoxValueChanged();

	void selectedClicked();
};

#endif
