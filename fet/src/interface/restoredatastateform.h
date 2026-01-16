/***************************************************************************
                          restoredatastateform.h  -  description
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

#ifndef RESTOREDATASTATEFORM_H
#define RESTOREDATASTATEFORM_H

#include "ui_restoredatastateform_template.h"

#include <QByteArrayList>

class RestoreDataStateForm : public QDialog, Ui::RestoreDataStateForm_template{
	Q_OBJECT
	
private:
	QByteArrayList descriptionsArchivedBA;
	
public:
	RestoreDataStateForm(QWidget* parent);
	~RestoreDataStateForm();

public Q_SLOTS:
	void restoreDataStateListWidgetSelectionChanged();

	void ok();
	void cancel();
};

#endif
