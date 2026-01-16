/***************************************************************************
                          modifyactivityform.h  -  description
                             -------------------
    begin                : Feb 9, 2005
    copyright            : (C) 2005 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef MODIFYACTIVITYFORM_H
#define MODIFYACTIVITYFORM_H

#include "ui_modifyactivityform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QSet>
#include <QList>

class ModifyActivityForm : public QDialog, Ui::ModifyActivityForm_template  {
	Q_OBJECT

private:
	QList<QSpinBox*> durList;
	QList<QCheckBox*> activList;

	QSpinBox* dur(int i);
	QCheckBox* activ(int i);

	QSet<QString> teachersNamesSet;
	QSet<QString> subjectsNamesSet;
	QSet<QString> activityTagsNamesSet;

public:
	QList<QString> canonicalStudentsSetsNames;

	int _id;
	int _activityGroupId;
	Activity* _activity;
	QStringList _teachers;
	QString _subject;
	QStringList _activityTags;
	QStringList _students;

	ModifyActivityForm(QWidget* parent, int id, int activityGroupId);
	~ModifyActivityForm();

	void updateStudentsListWidget();
	void updateSubjectsComboBox();
	void updateActivityTagsListWidget();

	void populateSubactivitiesTabWidget(int n);

public Q_SLOTS:
	void allTeachersRadioButtonToggled(bool checked);
	void qualifiedTeachersRadioButtonToggled(bool checked);
	void updateAllTeachersListWidget();

	void ok();
	void cancel();

	void addTeacher();
	void removeTeacher();

	void addStudents();
	void removeStudents();

	void clearTeachers();
	void clearStudents();
	void clearActivityTags();

	void addActivityTag();
	void removeActivityTag();

	void showYearsChanged();
	void showGroupsChanged();
	void showSubgroupsChanged();
	
	void help();
};

#endif
