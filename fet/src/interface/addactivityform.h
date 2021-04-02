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


class AddActivityForm : public AddActivityForm_template  {
	Q_OBJECT

public: 
	AddActivityForm();
	~AddActivityForm();

	void updateStudentsListBox();
	void updateTeachersListBox();
	void updateSubjectsComboBox();
	void updateActivityTagsListBox();
	void updatePreferredDaysComboBox();
	void updatePreferredHoursComboBox();

	void addTeacher();
	void removeTeacher();
	void addStudents();
	void removeStudents();
	void subjectChanged(const QString& dummy);

	void addActivityTag();
	void removeActivityTag();

	void hourChanged();
	void dayChanged();
	void parityChanged();
	void durationChanged();
	void activityChanged();
	void splitChanged();
	
	void clearTeachers();
	void clearStudents();
	void clearActivityTags();
	
	void showYearsChanged();
	void showGroupsChanged();
	void showSubgroupsChanged();

	void addActivity();
	void help();
};

class SecondMinDaysDialog: public QDialog
{
	Q_OBJECT
public:
	SecondMinDaysDialog(QWidget* p, int minD, double weight);
	~SecondMinDaysDialog();
	
	double weight;
	QLineEdit percText;

public slots:
	void yesPressed();
	
};


#endif
