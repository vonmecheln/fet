/***************************************************************************
                          humanreadabledataform.h  -  description
                             -------------------
    begin                : 2026
    copyright            : (C) 2026 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef HUMANREADABLEDATAFORM_H
#define HUMANREADABLEDATAFORM_H

#include "ui_humanreadabledataform_template.h"

#include <QDialog>

class HumanReadableDataForm : public QDialog, Ui::HumanReadableDataForm_template  {
	Q_OBJECT
	
	QString htmlString;
	
public:
	HumanReadableDataForm(QWidget* parent);
	~HumanReadableDataForm();
	
public Q_SLOTS:
	void filterChanged();

	void saveAs();
};

#endif
