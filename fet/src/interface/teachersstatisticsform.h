/***************************************************************************
                          teachersstatisticsform.h  -  description
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

#ifndef TEACHERSSTATISTICSFORM_H
#define TEACHERSSTATISTICSFORM_H

#include "ui_teachersstatisticsform_template.h"

#include <QDialog>

class TeachersStatisticsForm : public QDialog, Ui::TeachersStatisticsForm_template  {
	Q_OBJECT

public:
	QList<QString> names;
	QList<int> subactivities;
	QList<int> durations;
	QList<int> targets;

	QList<int> hideFullTeacher;

	TeachersStatisticsForm(QWidget* parent);
	~TeachersStatisticsForm();
	
public Q_SLOTS:
	void highlightIncompleteTeachersCheckBoxModified();
	void hideFullTeachersCheckBoxModified();
};

#endif
