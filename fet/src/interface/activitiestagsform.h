/***************************************************************************
                          activitiestagsform.h  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef ACTIVITIESTAGSFORM_H
#define ACTIVITIESTAGSFORM_H

#include "ui_activitiestagsform_template.h"

#include "activity.h"

class ActivitiesTagsForm : public QDialog, Ui::ActivitiesTagsForm_template {
	Q_OBJECT
private:
	QList<Activity*> selectedActivitiesList;

public:
	ActivitiesTagsForm(QWidget* parent, const QList<Activity*>& salist);
	~ActivitiesTagsForm();

public Q_SLOTS:
	void addActivityTag();
	void removeActivityTag();
};

#endif
