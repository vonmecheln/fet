/***************************************************************************
                          addexistingstudentsgroupsform.h  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef ADDEXISTINGSTUDENTSGROUPSFORM_H
#define ADDEXISTINGSTUDENTSGROUPSFORM_H

#include "ui_addexistingstudentsgroupsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>
#include <QSet>

class AddExistingStudentsGroupsForm : public QDialog, Ui::AddExistingStudentsGroupsForm_template  {
	Q_OBJECT
	
public:
	AddExistingStudentsGroupsForm(QWidget* parent, StudentsYear* y);
	~AddExistingStudentsGroupsForm();

public Q_SLOTS:
	void addGroup();
	void removeGroup();
	void clear();

	void ok();
	void cancel();
	
	void help();
	
	void sortedToggled();
	
private:
	StudentsYear* year;
	
	QList<StudentsGroup*> allGroupsList;
	QList<StudentsGroup*> selectedGroupsList;
};

#endif
