/***************************************************************************
                          modifytimeconstraint.h  -  description
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

#ifndef MODIFYTIMECONSTRAINTS_H
#define MODIFYTIMECONSTRAINTS_H

#include "timeconstraint.h"

#include <QWidget>
#include <QDialog>

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QListWidget>
#include <QTabWidget>

#include <QList>
#include <QSet>

#include <QString>

#include <QEventLoop>

class ModifyTimeConstraintsDialog: public QDialog
{
	QString dialogName;
	QString dialogTitle;
	QEventLoop* eventLoop;

public:
	ModifyTimeConstraintsDialog(QWidget* parent, const QString& _dialogName, const QString& _dialogTitle, QEventLoop* _eventLoop);
	~ModifyTimeConstraintsDialog();
};

class ModifyTimeConstraints: public QObject
{
	Q_OBJECT

	QEventLoop* eventLoop;

	ModifyTimeConstraintsDialog* dialog;
	QString dialogName;
	QString dialogTitle;
	
	int type;
	QList<TimeConstraint*> tcl;

	QPushButton* okPushButton;
	QPushButton* cancelPushButton;

	QLabel* weightLabel;
	QLineEdit* weightLineEdit;
	
	QLabel* firstModifyInstructionsLabel;
	QLabel* secondModifyInstructionsLabel;

	QLabel* teacherLabel;
	QComboBox* teachersComboBox;

	QLabel* labelForSpinBox;
	QSpinBox* spinBox;

	QCheckBox* checkBox;

public:
	ModifyTimeConstraints(QWidget* parent, int _type, QList<TimeConstraint*> _tcl);
	~ModifyTimeConstraints();

private:
	void okClicked();
	void cancelClicked();
	void helpClicked();
	void checkBoxToggled();

};

#endif
