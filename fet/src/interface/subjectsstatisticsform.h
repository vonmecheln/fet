/***************************************************************************
                          subjectsstatisticsform.h  -  description
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

#ifndef SUBJECTSSTATISTICSFORM_H
#define SUBJECTSSTATISTICSFORM_H

#include "ui_subjectsstatisticsform_template.h"

#include <QDialog>

class SubjectsStatisticsForm : public QDialog, Ui::SubjectsStatisticsForm_template  {
	Q_OBJECT

public:
	SubjectsStatisticsForm(QWidget* parent);
	~SubjectsStatisticsForm();
};

#endif
