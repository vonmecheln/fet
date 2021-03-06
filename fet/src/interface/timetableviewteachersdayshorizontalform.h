/***************************************************************************
                          timetableviewteachersdayshorizontalform.h  -  description
                             -------------------
    begin                : Wed May 14 2003
    copyright            : (C) 2003 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef TIMETABLEVIEWTEACHERSDAYSHORIZONTALFORM_H
#define TIMETABLEVIEWTEACHERSDAYSHORIZONTALFORM_H

#include <QResizeEvent>

class QColor; //by Marco Vassura

#include "ui_timetableviewteachersdayshorizontalform_template.h"

class TimetableViewTeachersDaysHorizontalForm : public QDialog, public Ui::TimetableViewTeachersDaysHorizontalForm_template
{
	Q_OBJECT

public:
	TimetableViewTeachersDaysHorizontalForm(QWidget* parent);
	void newTimetableGenerated();
	~TimetableViewTeachersDaysHorizontalForm();
	
	void lock(bool lockTime, bool lockSpace);
	
	void resizeRowsAfterShow();

	void detailActivity(QTableWidgetItem* item);

public slots:
	void lockTime();
	void lockSpace();
	void lockTimeSpace();
	void updateTeachersTimetableTable();

	void teacherChanged(const QString& teacherName);

	void currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);

	void help();

protected:
	void resizeEvent(QResizeEvent* event);
	QColor stringToColor(const QString& s); //by Marco Vassura
};

#endif
