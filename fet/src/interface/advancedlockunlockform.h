/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************
                          advancedlockunlockform.h  -  description
                             -------------------
    begin                : Dec 2008
    copyright            : (C) by Liviu Lalescu (https://lalescu.ro/liviu/) and Volker Dirr (https://www.timetabling.de/)
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef ADVANCEDLOCKUNLOCKFORM_H
#define ADVANCEDLOCKUNLOCKFORM_H

#include <QList>

#include <QObject>

#include <QCheckBox>
#include <QListWidget>

#include "advancedfilterform.h"

class Activity;

class TimeConstraint;
class SpaceConstraint;

bool compareTimeConstraintsActivityPreferredStartingTimeActivitiesIds(TimeConstraint* a, TimeConstraint* b);
bool compareSpaceConstraintsActivityPreferredRoomActivitiesIds(SpaceConstraint* a, SpaceConstraint* b);

class LockAdvancedDialog : public QDialog{
	Q_OBJECT
	
	QCheckBox* filterCheckBox;
	
	QLabel* numberLabel;
	QLabel* durationLabel;

	bool all; //all or any, true means all, false means any
	QList<int> descrDetDescrDetDescrWithConstraints;
	QList<int> contains;
	QStringList text;
	bool caseSensitive;
	
	bool filterOk(Activity* act);

	void filterChanged();

public:
	AdvancedFilterForm* filterForm;

	QList<int> filteredActivitiesIndicesList; //indices in the internal activities list.
	QCheckBox* timeCheckBox;
	QCheckBox* spaceCheckBox;
	
	QListWidget* selectedActivitiesLW;
	
	LockAdvancedDialog(QWidget* parent);
	~LockAdvancedDialog();
	
public Q_SLOTS:
	void filter(bool selected);
	void help();
};

class UnlockAdvancedDialog : public QDialog{
	Q_OBJECT
	
	QCheckBox* filterCheckBox;
	
	QLabel* numberLabel;
	QLabel* durationLabel;
	
	bool all; //all or any, true means all, false means any
	QList<int> descrDetDescrDetDescrWithConstraints;
	QList<int> contains;
	QStringList text;
	bool caseSensitive;
	
	bool filterOk(Activity* act);

	void filterChanged();

public:
	AdvancedFilterForm* filterForm;

	QList<int> filteredActivitiesIdsList; //ids in the activities list.
	QCheckBox* timeCheckBox;
	QCheckBox* spaceCheckBox;
	
	QListWidget* selectedActivitiesLW;
	
	UnlockAdvancedDialog(QWidget* parent);
	~UnlockAdvancedDialog();

public Q_SLOTS:
	void filter(bool selected);
	void help();
};

class AdvancedLockUnlockForm : public QObject{
	Q_OBJECT

public:
	static void lockAll(QWidget* parent);
//	static void unlockAll(QWidget* parent);

	static void unlockAllWithoutTimetable(QWidget* parent);
	
	static void lockDay(QWidget* parent);
//	static void unlockDay(QWidget* parent);

	static void unlockDayWithoutTimetable(QWidget* parent);
	
	static void lockEndStudentsDay(QWidget* parent);
	static void unlockEndStudentsDay(QWidget* parent);
	
	static void lockActivityTag(QWidget* parent);
//	static void unlockActivityTag(QWidget* parent);

	static void unlockActivityTagWithoutTimetable(QWidget* parent);

	static void lockAdvancedFilter(QWidget* parent);
	static void unlockAdvancedFilterWithoutTimetable(QWidget* parent);
};

#endif
