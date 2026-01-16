/***************************************************************************
                          studentsstatisticsform.h  -  description
                             -------------------
    begin                : March 25, 2006
    copyright            : (C) 2006 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef STUDENTSSTATISTICSFORM_H
#define STUDENTSSTATISTICSFORM_H

#include "ui_studentsstatisticsform_template.h"

#include <QDialog>

#include <QString>
#include <QHash>

#include "studentsset.h"

class StudentsStatisticsForm : public QDialog, Ui::StudentsStatisticsForm_template  {
	Q_OBJECT
	
private:
	QHash<QString, int> allHours;
	QHash<QString, int> allActivities;

public:
	StudentsStatisticsForm(QWidget* parent);
	~StudentsStatisticsForm();
	
	void insertStudentsSet(StudentsSet* studentsSet, int row);
	
public Q_SLOTS:
	void checkBoxesModified();

	void helpPushButton_clicked();
};

#endif
