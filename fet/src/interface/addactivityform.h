/***************************************************************************
                          addactivityform.h  -  description
                             -------------------
    begin                : Wed Apr 23 2003
    copyright            : (C) 2003 by Lalescu Liviu
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

#ifndef ADDACTIVITYFORM_H
#define ADDACTIVITYFORM_H

#include "addactivityform_template.h"

#include "genetictimetable_defs.h"
#include "genetictimetable.h"
#include "fet.h"

#include "fetmainform.h"

#include <q3combobox.h>
#include <qmessagebox.h>
#include <q3groupbox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <q3textedit.h>


class AddActivityForm : public AddActivityForm_template  {
public: 
	AddActivityForm();
	~AddActivityForm();

	void updateStudentsListBox();
	void updateTeachersListBox();
	void updateSubjectsComboBox();
	void updateSubjectTagsComboBox();
	void updatePreferredDaysComboBox();
	void updatePreferredHoursComboBox();

	void addTeacher();
	void removeTeacher();
	void addStudents();
	void removeStudents();
	void subjectChanged(const QString& dummy);
	void subjectTagChanged(const QString& dummy);
	void hourChanged();
	void dayChanged();
	void parityChanged();
	void durationChanged();
	void activityChanged();
	void splitChanged();
	
	void clearTeachers();
	void clearStudents();
	
	void showYearsChanged();
	void showGroupsChanged();
	void showSubgroupsChanged();

	void addActivity();
};

#endif
