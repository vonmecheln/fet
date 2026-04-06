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

#include "utilities.h"

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

	CornerEnabledTableWidgetOfSpinBoxes* occupyMaxTimesTable;
	QAbstractItemDelegate* occupyMaxOldItemDelegate;
	AddOrModifySpaceConstraintTimesTableDelegate* occupyMaxNewItemDelegate;

	CornerEnabledTableWidget* timesTable;
	QAbstractItemDelegate* oldItemDelegate;
	AddOrModifySpaceConstraintTimesTableDelegate* newItemDelegate;

	//For room pair of mutually exclusive sets of time slots
	CornerEnabledTableWidget* timesTable1;
	QAbstractItemDelegate* oldItemDelegate1;
	AddOrModifySpaceConstraintTimesTableDelegate* newItemDelegate1;
	CornerEnabledTableWidget* timesTable2;
	QAbstractItemDelegate* oldItemDelegate2;
	AddOrModifySpaceConstraintTimesTableDelegate* newItemDelegate2;

	QCheckBox* colorsCheckBox;

	//For room pair of mutually exclusive sets of time slots
	QCheckBox* colorsCheckBox1;
	QCheckBox* colorsCheckBox2;

	QCheckBox* showRelatedCheckBox;

public:
	AddOrModifySpaceConstraintDialog(QWidget* parent, const QString& _dialogName, const QString& _dialogTitle, QEventLoop* _eventLoop,
									 CornerEnabledTableWidgetOfSpinBoxes* _occupyMaxTimesTable,
									 QAbstractItemDelegate* _occupyMaxOldItemDelegate,
									 AddOrModifySpaceConstraintTimesTableDelegate* _occupyMaxNewItemDelegate,

									 CornerEnabledTableWidget* _timesTable,
									 QAbstractItemDelegate* _oldItemDelegate,
									 AddOrModifySpaceConstraintTimesTableDelegate* _newItemDelegate,

									 CornerEnabledTableWidget* _timesTable1,
									 QAbstractItemDelegate* _oldItemDelegate1,
									 AddOrModifySpaceConstraintTimesTableDelegate* _newItemDelegate1,
									 CornerEnabledTableWidget* _timesTable2,
									 QAbstractItemDelegate* _oldItemDelegate2,
									 AddOrModifySpaceConstraintTimesTableDelegate* _newItemDelegate2,

									 QCheckBox* _colorsCheckBox,

									 QCheckBox* _colorsCheckBox1,
									 QCheckBox* _colorsCheckBox2,
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

	CornerEnabledTableWidgetOfSpinBoxes* occupyMaxSetsOfTimeSlotsFromSelectionTableWidget;
	QAbstractItemDelegate* occupyMaxOldItemDelegate;
	AddOrModifySpaceConstraintTimesTableDelegate* occupyMaxNewItemDelegate;
	Matrix2D<QSpinBox*> spinBoxesTable;

	//for room pair of mutually exclusive time slots
	QGroupBox* firstTimeSlotGroupBox;
	QGroupBox* secondTimeSlotGroupBox;
	QComboBox* firstDayComboBox;
	QComboBox* firstHourComboBox;
	QComboBox* secondDayComboBox;
	QComboBox* secondHourComboBox;

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

	//For room pair of mutually exclusive sets of time slots
	QCheckBox* colorsCheckBox1;
	QPushButton* toggleAllPushButton1;
	QCheckBox* colorsCheckBox2;
	QPushButton* toggleAllPushButton2;

	CornerEnabledTableWidget* timesTable;
	QAbstractItemDelegate* oldItemDelegate;
	AddOrModifySpaceConstraintTimesTableDelegate* newItemDelegate;

	//For room pair of mutually exclusive sets of time slots
	CornerEnabledTableWidget* timesTable1;
	QAbstractItemDelegate* oldItemDelegate1;
	AddOrModifySpaceConstraintTimesTableDelegate* newItemDelegate1;
	CornerEnabledTableWidget* timesTable2;
	QAbstractItemDelegate* oldItemDelegate2;
	AddOrModifySpaceConstraintTimesTableDelegate* newItemDelegate2;

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

	QLabel* buildingLabel;
	QComboBox* buildingsComboBox;

	QLabel* activityLabel;
	QComboBox* activitiesComboBox;
	QList<int> activitiesList;
	int initialActivityId;
	QList<Activity*> filteredActivitiesList;

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

	//For room pair of mutually exclusive sets of time slots
	QTabWidget* tabWidgetPairOfMutuallyExclusiveSets;

	QSet<QString> showedStudents;

public:
	AddOrModifySpaceConstraint(QWidget* parent, int _type, SpaceConstraint* _oldsc=nullptr,
	 const QString& _preselectedTeacherName=QString(), const QString& _preselectedStudentsSetName=QString(), const QString& _preselectedSubjectName=QString(), const QString& _preselectedActivityTagName=QString(),
	 const QString& _preselectedRoomName=QString(), const QString& _preselectedBuildingName=QString(),
	 const QList<int>& _filteredActivitiesIdsList=QList<int>());
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

	//For teacher(s)/students (set) pair of mutually exclusive sets of time slots
	//void colorItem1(QTableWidgetItem* item);
	void itemClicked1(QTableWidgetItem* item);
	void horizontalHeaderClicked1(int col);
	void verticalHeaderClicked1(int row);
	void cellEntered1(int row, int col);
	void colorsCheckBoxToggled1();
	void toggleAllClicked1();
	//void colorItem2(QTableWidgetItem* item);
	void itemClicked2(QTableWidgetItem* item);
	void horizontalHeaderClicked2(int col);
	void verticalHeaderClicked2(int row);
	void cellEntered2(int row, int col);
	void colorsCheckBoxToggled2();
	void toggleAllClicked2();

	void clearSelectedRealRooms();
	void addRealRoom();
	void removeRealRoom();

	void clearSelectedRooms();
	void addRoom();
	void removeRoom();

	void showRelatedCheckBoxToggled();

	void initOccupyMaxTableWidget();
	void fillSpinBoxTimesTable(const QList<QList<int>>& days, const QList<QList<int>>& hours);
	void getSpinBoxTimesTable(QList<QList<int>>& days, QList<QList<int>>& hours);

	void colorSpinBoxWithPointer(QSpinBox* sb);
	void colorSpinBox();
	void colorAllSpinBoxes();
};

#endif
