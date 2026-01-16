/***************************************************************************
                          addormodifyspaceconstraint.h  -  description
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

#ifndef ADDORMODIFYSPACECONSTRAINT_H
#define ADDORMODIFYSPACECONSTRAINT_H

#include "spaceconstraint.h"

#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>

#include <QWidget>
#include <QDialog>

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QComboBox>

#include <QList>
#include <QSet>

#include <QString>

#include <QEventLoop>

class CornerEnabledTableWidget;

class AddOrModifySpaceConstraintTimesTableDelegate: public QStyledItemDelegate
{
	Q_OBJECT

public:
	int nRows; //The number of rows after which a line is drawn
	int nColumns;

public:
	AddOrModifySpaceConstraintTimesTableDelegate(QWidget* parent, int _nRows, int _nColumns): QStyledItemDelegate(parent){
		nRows=_nRows;
		nColumns=_nColumns;
	}

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

class AddOrModifySpaceConstraintDialog: public QDialog
{
	QString dialogName;
	QString dialogTitle;
	QEventLoop* eventLoop;

	CornerEnabledTableWidget* timesTable;
	QAbstractItemDelegate* oldItemDelegate;
	AddOrModifySpaceConstraintTimesTableDelegate* newItemDelegate;

	QCheckBox* colorsCheckBox;

	QCheckBox* showRelatedCheckBox;

public:
	AddOrModifySpaceConstraintDialog(QWidget* parent, const QString& _dialogName, const QString& _dialogTitle, QEventLoop* _eventLoop,
									 CornerEnabledTableWidget* _timesTable,
									 QAbstractItemDelegate* _oldItemDelegate,
									 AddOrModifySpaceConstraintTimesTableDelegate* _newItemDelegate,
									 QCheckBox* _colorsCheckBox,
									 QCheckBox* _showRelatedCheckBox);
	~AddOrModifySpaceConstraintDialog();
};

class AddOrModifySpaceConstraint: public QObject
{
	Q_OBJECT

	QEventLoop* eventLoop;

	AddOrModifySpaceConstraintDialog* dialog;
	QString dialogName;
	QString dialogTitle;

	int type;
	SpaceConstraint* oldsc;

	QCheckBox* showRelatedCheckBox;

	QPushButton* addConstraintPushButton;
	QPushButton* addConstraintsPushButton;
	QPushButton* closePushButton;
	QPushButton* okPushButton;
	QPushButton* cancelPushButton;
	QPushButton* helpPushButton;

	QLabel* weightLabel;
	QLineEdit* weightLineEdit;

	QLabel* firstAddInstructionsLabel;
	QLabel* secondAddInstructionsLabel;

	QLabel* firstModifyInstructionsLabel;
	QLabel* secondModifyInstructionsLabel;

	QCheckBox* colorsCheckBox;
	QPushButton* toggleAllPushButton;

	CornerEnabledTableWidget* timesTable;
	QAbstractItemDelegate* oldItemDelegate;
	AddOrModifySpaceConstraintTimesTableDelegate* newItemDelegate;

	QGroupBox* filterGroupBox;

	QLabel* teacherLabel;
	QComboBox* teachersComboBox;

	QLabel* studentsLabel;
	QComboBox* studentsComboBox;

	QLabel* subjectLabel;
	QComboBox* subjectsComboBox;

	QLabel* activityTagLabel;
	QComboBox* activityTagsComboBox;

	QLabel* roomLabel;
	QComboBox* roomsComboBox;

	QLabel* activityLabel;
	QComboBox* activitiesComboBox;
	QList<int> activitiesList;
	int initialActivityId;

	QLabel* labelForSpinBox;
	QSpinBox* spinBox;

	QCheckBox* checkBox;

	QCheckBox* permanentlyLockedCheckBox;
	QLabel* permanentlyLockedLabel;

	QLabel* activitiesLabel;
	QLabel* selectedActivitiesLabel;
	QListWidget* activitiesListWidget;
	QListWidget* selectedActivitiesListWidget;
	QPushButton* addAllActivitiesPushButton;
	QPushButton* clearActivitiesPushButton;

	QList<int> selectedActivitiesList;

	bool addEmpty;

	bool intervalIsForRealDay;
	QLabel* intervalStartHourLabel;
	QComboBox* intervalStartHourComboBox;
	QLabel* intervalEndHourLabel;
	QComboBox* intervalEndHourComboBox;

	QLabel* selectedRealRoomsLabel;
	QListWidget* selectedRealRoomsListWidget;
	QLabel* allRealRoomsLabel;
	QListWidget* allRealRoomsListWidget;
	QPushButton* clearSelectedRealRoomsPushButton;

	QLabel* selectedRoomsLabel;
	QListWidget* selectedRoomsListWidget;
	QLabel* allRoomsLabel;
	QListWidget* allRoomsListWidget;
	QPushButton* clearSelectedRoomsPushButton;

	QLabel* activityTagsLabel;
	QLabel* selectedActivityTagsLabel;
	QListWidget* activityTagsListWidget;
	QListWidget* selectedActivityTagsListWidget;
	//QPushButton* addAllActivityTagsPushButton;
	QPushButton* clearActivityTagsPushButton;

	QSet<QString> selectedActivityTagsSet;

	QCheckBox* filterActivityTagsCheckBox;

	QSet<QString> showedStudents;

public:
	AddOrModifySpaceConstraint(QWidget* parent, int _type, SpaceConstraint* _oldsc=nullptr,
	 const QString& _preselectedTeacherName=QString(), const QString& _preselectedStudentsSetName=QString(), const QString& _preselectedSubjectName=QString(), const QString& _preselectedActivityTagName=QString(),
	 const QString& _preselectedRoomName=QString());
	~AddOrModifySpaceConstraint();

private:
	void addConstraintClicked();
	void addConstraintsClicked();
	void closeClicked();
	void okClicked();
	void cancelClicked();
	void helpClicked();
	void checkBoxToggled();

	void addAllActivitiesClicked();
	void clearActivitiesClicked();
	void addActivity();
	void removeActivity();

	//void addAllActivityTagsClicked();
	void clearActivityTagsClicked();
	void addActivityTag();
	void removeActivityTag();

	void filterActivityTagsCheckBoxToggled();

	bool filterOk(Activity* act);
	int filterActivitiesComboBox(); //returns the index of the current constraint's activity in modify dialog
	void filterActivitiesListWidget();

	//void colorItem(QTableWidgetItem* item);
	void itemClicked(QTableWidgetItem* item);
	void horizontalHeaderClicked(int col);
	void verticalHeaderClicked(int row);
	void cellEntered(int row, int col);
	void colorsCheckBoxToggled();
	void toggleAllClicked();

	void clearSelectedRealRooms();
	void addRealRoom();
	void removeRealRoom();

	void clearSelectedRooms();
	void addRoom();
	void removeRoom();

	void showRelatedCheckBoxToggled();
};

#endif
