/***************************************************************************
                          addexistingstudentssubgroupsform.h  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Lalescu Liviu
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

#ifndef ADDEXISTINGSTUDENTSSUBGROUPSFORM_H
#define ADDEXISTINGSTUDENTSSUBGROUPSFORM_H

#include "ui_addexistingstudentssubgroupsform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QList>
#include <QSet>

class AddExistingStudentsSubgroupsForm : public QDialog, Ui::AddExistingStudentsSubgroupsForm_template  {
	Q_OBJECT
	
public:
	AddExistingStudentsSubgroupsForm(QWidget* parent, StudentsYear* y, StudentsGroup* g);
	~AddExistingStudentsSubgroupsForm();

public slots:
	void addSubgroup();
	void removeSubgroup();
	void clear();

	void ok();
	void cancel();
	
	void help();
	
	void sortedToggled();
	
private:
	StudentsYear* year;
	StudentsGroup* group;
	
	QList<StudentsSubgroup*> allSubgroupsList;
	QList<StudentsSubgroup*> selectedSubgroupsList;
};

#endif
