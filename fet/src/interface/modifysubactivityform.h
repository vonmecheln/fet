/***************************************************************************
                          modifysubactivityform.h  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef MODIFYSUBACTIVITYFORM_H
#define MODIFYSUBACTIVITYFORM_H

#include "ui_modifysubactivityform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

#include <QSet>
//#include <QHash>
#include <QList>

class ModifySubactivityForm : public QDialog, Ui::ModifySubactivityForm_template  {
	Q_OBJECT
	
private:
	QSet<QString> teachersNamesSet;
	QSet<QString> subjectsNamesSet;
	QSet<QString> activityTagsNamesSet;
	//QHash<QString, int> numberOfStudentsHash;

public:
	QList<QString> canonicalStudentsSetsNames;

	int _id;
	int _activityGroupId;
	Activity* _activity;
	QStringList _teachers;
	QString _subject;
	QStringList _activityTags;
	QStringList _students;

	ModifySubactivityForm(QWidget* parent, int id, int activityGroupId);
	~ModifySubactivityForm();

	void updateStudentsListWidget();
	void updateSubjectsComboBox();
	void updateActivityTagsListWidget();
	
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

	void addActivityTag();
	void removeActivityTag();
	void clearActivityTags();

	void showYearsChanged();
	void showGroupsChanged();
	void showSubgroupsChanged();
	
	void help();
};

#endif
