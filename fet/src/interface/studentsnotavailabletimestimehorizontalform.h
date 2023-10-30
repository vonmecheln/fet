/***************************************************************************
                          studentsnotavailabletimestimehorizontalform.h  -  description
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

#ifndef STUDENTSNOTAVAILABLETIMESTIMEHORIZONTALFORM_H
#define STUDENTSNOTAVAILABLETIMESTIMEHORIZONTALFORM_H

#include <QResizeEvent>

#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>

#include "ui_studentsnotavailabletimestimehorizontalform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class StudentsNotAvailableTimesTimeHorizontalDelegate: public QStyledItemDelegate
{
	Q_OBJECT

public:
	int nRows;
	int nColumns; //The number of columns after which a line is drawn.

public:
	StudentsNotAvailableTimesTimeHorizontalDelegate(QWidget* parent, int _nRows, int _nColumns): QStyledItemDelegate(parent){
		nRows=_nRows;
		nColumns=_nColumns;
	}

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

class StudentsNotAvailableTimesTimeHorizontalForm : public QDialog, public Ui::StudentsNotAvailableTimesTimeHorizontalForm_template
{
	Q_OBJECT
	
private:
	int initialRecommendedHeight;

	QAbstractItemDelegate* oldItemDelegate;
	StudentsNotAvailableTimesTimeHorizontalDelegate* newItemDelegate;

	Matrix3D<bool> snaMatrix;
	Matrix1D<bool> inactiveConstraint;
	
	QList<QString> allStudentsNames;
	QList<int> allStudentsType;

public:
	StudentsNotAvailableTimesTimeHorizontalForm(QWidget* parent);
	~StudentsNotAvailableTimesTimeHorizontalForm();

	void colorItem(QTableWidgetItem* item);

public slots:
	void itemClicked(QTableWidgetItem* item);

	void ok();
	void cancel();

	void checkBoxesChanged();

	void widthSpinBoxValueChanged();
	void heightSpinBoxValueChanged();

	void selectedClicked();
};

#endif
