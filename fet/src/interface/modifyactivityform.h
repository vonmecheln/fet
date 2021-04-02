/***************************************************************************
                          modifyactivityform.h  -  description
                             -------------------
    begin                : Feb 9, 2005
    copyright            : (C) 2005 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MODIFYACTIVITYFORM_H
#define MODIFYACTIVITYFORM_H

#include "modifyactivityform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

//#include "fetmainform.h"

#include <q3combobox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3textedit.h>
#include <q3listbox.h>


class ModifyActivityForm : public ModifyActivityForm_template  {
public:
	int _id;
	int _activityGroupId;
	Activity* _activity;
	QStringList _teachers;
	QString _subject;
	QString _subjectTag;
	QStringList _students;

	ModifyActivityForm(int id, int activityGroupId);
	~ModifyActivityForm();

	void addTeacher();
	void removeTeacher();
	void addStudents();
	void removeStudents();
	void updateStudentsListBox();
	void updateTeachersListBox();
	void updateSubjectsComboBox();
	void updateSubjectTagsComboBox();
	
	void clearTeachers();
	void clearStudents();

	void subjectChanged(const QString& dummy);
	void subjectTagChanged(const QString& dummy);
	//void parityChanged();
	void durationChanged();
	void activityChanged();
	
	void showYearsChanged();
	void showGroupsChanged();
	void showSubgroupsChanged();

	void ok();
	void cancel();
};

#endif
