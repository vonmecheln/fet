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
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef TEACHERSNOTAVAILABLETIMESTIMEHORIZONTALFORM_H
#define TEACHERSNOTAVAILABLETIMESTIMEHORIZONTALFORM_H

#include <QResizeEvent>

#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>

#include <QSet>
#include <QString>

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
	QSet<QString> showedStudents;

	int initialRecommendedHeight;

	QAbstractItemDelegate* oldItemDelegate;
	TeachersNotAvailableTimesTimeHorizontalDelegate* newItemDelegate;

	Matrix3D<bool> tnaMatrix;
	Matrix1D<bool> inactiveConstraint;

public:
	TeachersNotAvailableTimesTimeHorizontalForm(QWidget* parent);
	~TeachersNotAvailableTimesTimeHorizontalForm();

	bool filterOk(const QString& tchName);

	void colorItem(QTableWidgetItem* item);

public Q_SLOTS:
	void itemClicked(QTableWidgetItem* item);

	void ok();
	void cancel();

	void widthSpinBoxValueChanged();
	void heightSpinBoxValueChanged();

	void selectedClicked();

	void studentsFilterChanged();

	void updateShownTeachers();

	void filterCheckBoxToggled();

	void colorsCheckBoxToggled();
};

#endif
