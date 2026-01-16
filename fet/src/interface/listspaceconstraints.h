/***************************************************************************
                          listspaceconstraints.h  -  description
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

#ifndef LISTSPACECONSTRAINTS_H
#define LISTSPACECONSTRAINTS_H

#include "spaceconstraint.h"

#include "advancedfilterform.h"

#include <QWidget>
#include <QDialog>

#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>

#include <QEventLoop>

#include <QSplitter>

#include <QString>

#include <QSet>

class ListSpaceConstraintsDialog: public QDialog
{
	QString dialogName;
	QString dialogTitle;
	QEventLoop* eventLoop;

	QSplitter* splitter;

	QCheckBox* showRelatedCheckBox;

public:
	ListSpaceConstraintsDialog(QWidget* parent, const QString& _dialogName, const QString& _dialogTitle, QEventLoop* _eventLoop, QSplitter* _splitter,
							   QCheckBox* showRelatedCheckBox);
	~ListSpaceConstraintsDialog();
};

class ListSpaceConstraints: public QObject
{
	Q_OBJECT

	AdvancedFilterForm* filterForm;

	bool all; //all or any, true means all, false means any
	QList<int> descrDetDescr;
	QList<int> contains;
	QStringList text;
	bool caseSensitive;

	bool useFilter;

	QEventLoop* eventLoop;

	QDialog* dialog;
	QString dialogName;
	QString dialogTitle;

	int type;

	QCheckBox* showRelatedCheckBox;

	QSplitter* splitter;

	QListWidget* constraintsListWidget;
	QTextEdit* constraintDescriptionTextEdit;

	QPushButton* addPushButton;
	QPushButton* modifyPushButton;
	QPushButton* removePushButton;
	QPushButton* helpPushButton;
	QPushButton* closePushButton;

	QLabel* firstInstructionsLabel;
	QLabel* secondInstructionsLabel;

	QList<SpaceConstraint*> visibleSpaceConstraintsList;

	QGroupBox* filterGroupBox;
	QComboBox* teachersComboBox;
	QComboBox* studentsComboBox;
	QComboBox* subjectsComboBox;
	QComboBox* activityTagsComboBox;
	QComboBox* roomsComboBox;

	QPushButton* addRemoveMultipleConstraintsPushButton;

	QLabel* countOfConstraintsLabel;
	QLabel* mSLabel;

	QCheckBox* filterCheckBox;
	QCheckBox* sortedCheckBox;

	QPushButton* activatePushButton;
	QPushButton* deactivatePushButton;
	QPushButton* weightsPushButton;
	QPushButton* commentsPushButton;

	QSet<QString> showedStudents;

public:
	ListSpaceConstraints(QWidget* parent, int _type);
	~ListSpaceConstraints();

private:
	void addClicked();
	void modifyClicked();
	void removeClicked();
	void helpClicked();
	void closeClicked();

	void addRemoveMultipleConstraintsActivitiesOccupyMaxDifferentRoomsClicked();

	bool filterOk(SpaceConstraint* sc);
	void filter();

	void constraintChanged();

	void advancedFilter(bool active);
	void sortedChanged(bool checked);

	void activateConstraints();
	void deactivateConstraints();
	void constraintComments();
	void changeWeights();
	void selectionChanged();

	void showRelatedCheckBoxToggled();
};

#endif
