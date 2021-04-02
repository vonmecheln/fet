/***************************************************************************
                          modifysubactivityform.h  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Lalescu Liviu
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

#ifndef MODIFYSUBACTIVITYFORM_H
#define MODIFYSUBACTIVITYFORM_H

#include "ui_modifysubactivityform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class ModifySubactivityForm : public QDialog, Ui::ModifySubactivityForm_template  {
	Q_OBJECT

public:
	QList<QString> canonicalStudentsSetsNames;

	int _id;
	int _activityGroupId;
	Activity* _activity;
	QStringList _teachers;
	QString _subject;
	QStringList _activityTags;
	QStringList _students;

	ModifySubactivityForm(int id, int activityGroupId);
	~ModifySubactivityForm();

	void updateStudentsListBox();
	void updateTeachersListBox();
	void updateSubjectsComboBox();
	void updateActivityTagsListBox();
	
	/*void durationChanged();
	void showYearsChanged();
	void showGroupsChanged();
	void showSubgroupsChanged();*/

public slots:
	void ok();
	void cancel();
	
	void subjectChanged(const QString& dummy);
	void subactivityChanged();
	
	void addTeacher();
	void removeTeacher();

	void addStudents();
	void removeStudents();

	void clearTeachers();
	void clearStudents();

	void addActivityTag();
	void removeActivityTag();
	void clearActivityTags();

	void showYearsChanged();
	void showGroupsChanged();
	void showSubgroupsChanged();
};

#endif
