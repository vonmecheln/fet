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
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef LISTSPACECONSTRAINTS_H
#define LISTSPACECONSTRAINTS_H

#include "spaceconstraint.h"

#include <QWidget>
#include <QDialog>

#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>

#include <QEventLoop>

#include <QSplitter>

class ListSpaceConstraintsDialog: public QDialog
{
	QString dialogName;
	QString dialogTitle;
	QEventLoop* eventLoop;

	QSplitter* splitter;

public:
	ListSpaceConstraintsDialog(QWidget* parent, const QString& _dialogName, const QString& _dialogTitle, QEventLoop* _eventLoop, QSplitter* _splitter);
	~ListSpaceConstraintsDialog();
};

class ListSpaceConstraints: public QObject
{
	Q_OBJECT

	QEventLoop* eventLoop;

	QDialog* dialog;
	QString dialogName;
	QString dialogTitle;

	int type;

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
	QPushButton* activatePushButton;
	QPushButton* deactivatePushButton;
	QPushButton* weightsPushButton;
	QPushButton* commentsPushButton;

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
	void activateConstraints();
	void deactivateConstraints();
	void constraintComments();
	void changeWeights();
	void selectionChanged();
};

#endif