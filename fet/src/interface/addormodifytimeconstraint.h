/***************************************************************************
                          addormodifytimeconstraint.h  -  description
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

#ifndef ADDORMODIFYTIMECONSTRAINT_H
#define ADDORMODIFYTIMECONSTRAINT_H

#include "timeconstraint.h"

#include "matrix.h"

#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>

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

class CornerEnabledTableWidget;

class AddOrModifyTimeConstraintTimesTableDelegate: public QStyledItemDelegate
{
	Q_OBJECT

public:
	int nRows; //The number of rows after which a line is drawn
	int nColumns;

public:
	AddOrModifyTimeConstraintTimesTableDelegate(QWidget* parent, int _nRows, int _nColumns): QStyledItemDelegate(parent){
		nRows=_nRows;
		nColumns=_nColumns;
	}

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

class AddOrModifyTimeConstraintDialog: public QDialog
{
	QString dialogName;
	QString dialogTitle;
	QEventLoop* eventLoop;

	CornerEnabledTableWidgetOfSpinBoxes* occupyMaxTimesTable;
	QAbstractItemDelegate* occupyMaxOldItemDelegate;
	AddOrModifyTimeConstraintTimesTableDelegate* occupyMaxNewItemDelegate;

	CornerEnabledTableWidget* timesTable;
	QAbstractItemDelegate* oldItemDelegate;
	AddOrModifyTimeConstraintTimesTableDelegate* newItemDelegate;

	//For teacher(s)/students (set) pair of mutually exclusive sets of time slots
	CornerEnabledTableWidget* timesTable1;
	QAbstractItemDelegate* oldItemDelegate1;
	AddOrModifyTimeConstraintTimesTableDelegate* newItemDelegate1;
	CornerEnabledTableWidget* timesTable2;
	QAbstractItemDelegate* oldItemDelegate2;
	AddOrModifyTimeConstraintTimesTableDelegate* newItemDelegate2;

	QCheckBox* colorsCheckBox;

	//For teacher(s)/students (set) pair of mutually exclusive sets of time slots
	QCheckBox* colorsCheckBox1;
	QCheckBox* colorsCheckBox2;

	QCheckBox* showRelatedCheckBox;

	QCheckBox* firstFilter_showRelatedCheckBox;
	QCheckBox* secondFilter_showRelatedCheckBox;
	QCheckBox* thirdFilter_showRelatedCheckBox;

public:
	AddOrModifyTimeConstraintDialog(QWidget* parent, const QString& _dialogName, const QString& _dialogTitle, QEventLoop* _eventLoop,
									CornerEnabledTableWidgetOfSpinBoxes* _occupyMaxTimesTable,
									QAbstractItemDelegate* _occupyMaxOldItemDelegate,
									AddOrModifyTimeConstraintTimesTableDelegate* _occupyMaxNewItemDelegate,

									CornerEnabledTableWidget* _timesTable,
									QAbstractItemDelegate* _oldItemDelegate,
									AddOrModifyTimeConstraintTimesTableDelegate* _newItemDelegate,

									CornerEnabledTableWidget* _timesTable1,
									QAbstractItemDelegate* _oldItemDelegate1,
									AddOrModifyTimeConstraintTimesTableDelegate* _newItemDelegate1,
									CornerEnabledTableWidget* _timesTable2,
									QAbstractItemDelegate* _oldItemDelegate2,
									AddOrModifyTimeConstraintTimesTableDelegate* _newItemDelegate2,

									QCheckBox* _colorsCheckBox,

									QCheckBox* _colorsCheckBox1,
									QCheckBox* _colorsCheckBox2,

									QCheckBox* _showRelatedCheckBox,
									QCheckBox* _firstFilter_showRelatedCheckBox,
									QCheckBox* _secondFilter_showRelatedCheckBox,
									QCheckBox* _thirdFilter_showRelatedCheckBox);
	~AddOrModifyTimeConstraintDialog();
};

class AddOrModifyTimeConstraint: public QObject
{
	Q_OBJECT

	QEventLoop* eventLoop;

	AddOrModifyTimeConstraintDialog* dialog;
	QString dialogName;
	QString dialogTitle;
	
	int type;
	TimeConstraint* oldtc;

	CornerEnabledTableWidgetOfSpinBoxes* occupyMaxSetsOfTimeSlotsFromSelectionTableWidget;
	QAbstractItemDelegate* occupyMaxOldItemDelegate;
	AddOrModifyTimeConstraintTimesTableDelegate* occupyMaxNewItemDelegate;
	Matrix2D<QSpinBox*> spinBoxesTable;

	bool ctrActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots; //true only if the constraint is of this type
	bool ctrActivitiesPairOfMutuallyExclusiveTimeSlots; //true only if the constraint is of this type

	//for teacher(s)/students (set) pair of mutually exclusive time slots
	QGroupBox* firstTimeSlotGroupBox;
	QGroupBox* secondTimeSlotGroupBox;
	QComboBox* firstDayComboBox;
	QComboBox* firstHourComboBox;
	QComboBox* secondDayComboBox;
	QComboBox* secondHourComboBox;

	QCheckBox* showRelatedCheckBox;

	QCheckBox* firstFilter_showRelatedCheckBox;
	QCheckBox* secondFilter_showRelatedCheckBox;
	QCheckBox* thirdFilter_showRelatedCheckBox;

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

	//For teacher(s)/students (set) pair of mutually exclusive sets of time slots
	QCheckBox* colorsCheckBox1;
	QPushButton* toggleAllPushButton1;
	QCheckBox* colorsCheckBox2;
	QPushButton* toggleAllPushButton2;
	
	CornerEnabledTableWidget* timesTable;
	QAbstractItemDelegate* oldItemDelegate;
	AddOrModifyTimeConstraintTimesTableDelegate* newItemDelegate;

	//For teacher(s)/students (set) pair of mutually exclusive sets of time slots
	CornerEnabledTableWidget* timesTable1;
	QAbstractItemDelegate* oldItemDelegate1;
	AddOrModifyTimeConstraintTimesTableDelegate* newItemDelegate1;
	CornerEnabledTableWidget* timesTable2;
	QAbstractItemDelegate* oldItemDelegate2;
	AddOrModifyTimeConstraintTimesTableDelegate* newItemDelegate2;

	QGroupBox* filterGroupBox;

	QLabel* teacherLabel;
	QComboBox* teachersComboBox;

	QLabel* studentsLabel;
	QComboBox* studentsComboBox;

	QLabel* subjectLabel;
	QComboBox* subjectsComboBox;

	QLabel* activityTagLabel;
	QComboBox* activityTagsComboBox;

	QLabel* first_activityTagLabel;
	QComboBox* first_activityTagsComboBox;

	QLabel* second_activityTagLabel;
	QComboBox* second_activityTagsComboBox;

	QLabel* activityLabel;
	QComboBox* activitiesComboBox;
	QList<int> activitiesList;
	int initialActivityId;

	QLabel* labelForSpinBox;
	QSpinBox* spinBox;

	QLabel* secondLabelForSpinBox;
	QSpinBox* secondSpinBox;

	QCheckBox* checkBox;

	QGroupBox* periodGroupBox;
	QLabel* dayLabel;
	QComboBox* daysComboBox;
	QLabel* hourLabel;
	QComboBox* hoursComboBox;
	QCheckBox* permanentlyLockedCheckBox;
	QLabel* permanentlyLockedLabel;

	QLabel* activitiesLabel;
	QLabel* selectedActivitiesLabel;
	QListWidget* activitiesListWidget;
	QListWidget* selectedActivitiesListWidget;
	QPushButton* addAllActivitiesPushButton;
	QPushButton* clearActivitiesPushButton;

	QList<int> selectedActivitiesList;

	QLabel* activityTagsLabel;
	QLabel* selectedActivityTagsLabel;
	QListWidget* activityTagsListWidget;
	QListWidget* selectedActivityTagsListWidget;
	QPushButton* addAllActivityTagsPushButton;
	QPushButton* clearActivityTagsPushButton;

	QSet<QString> selectedActivityTagsSet;

	QCheckBox* durationCheckBox;
	QSpinBox* durationSpinBox;

	QLabel* splitIndexLabel;
	QSpinBox* splitIndexSpinBox;

	bool addEmpty;

	QLabel* first_activityLabel;
	QComboBox* first_activitiesComboBox;
	QList<int> first_activitiesList;
	int first_initialActivityId;

	QLabel* second_activityLabel;
	QComboBox* second_activitiesComboBox;
	QList<int> second_activitiesList;
	int second_initialActivityId;

	QLabel* third_activityLabel;
	QComboBox* third_activitiesComboBox;
	QList<int> third_activitiesList;
	int third_initialActivityId;

	QPushButton* swapActivitiesPushButton;
	QPushButton* swapActivityTagsPushButton;

	QLabel* intervalStartHourLabel;
	QComboBox* intervalStartHourComboBox;
	QLabel* intervalEndHourLabel;
	QComboBox* intervalEndHourComboBox;

	QTabWidget* tabWidget;

	QTabWidget* tabWidgetAOMSOTSFS; //activities occupy max sets of time slots from selection

	QCheckBox* allowEmptySlotsCheckBox;

	QTabWidget* tabWidgetTwoSetsOfActivities;
	//
	QLabel* activitiesLabel_TwoSetsOfActivities_1;
	QLabel* selectedActivitiesLabel_TwoSetsOfActivities_1;
	QListWidget* activitiesListWidget_TwoSetsOfActivities_1;
	QListWidget* selectedActivitiesListWidget_TwoSetsOfActivities_1;
	QPushButton* addAllActivitiesPushButton_TwoSetsOfActivities_1;
	QPushButton* clearActivitiesPushButton_TwoSetsOfActivities_1;
	//
	QList<int> selectedActivitiesList_TwoSetsOfActivities_1;
	//
	QLabel* activitiesLabel_TwoSetsOfActivities_2;
	QLabel* selectedActivitiesLabel_TwoSetsOfActivities_2;
	QListWidget* activitiesListWidget_TwoSetsOfActivities_2;
	QListWidget* selectedActivitiesListWidget_TwoSetsOfActivities_2;
	QPushButton* addAllActivitiesPushButton_TwoSetsOfActivities_2;
	QPushButton* clearActivitiesPushButton_TwoSetsOfActivities_2;
	//
	QList<int> selectedActivitiesList_TwoSetsOfActivities_2;
	//
	QPushButton* swapTwoSetsOfActivitiesPushButton;

	//For teacher(s)/students (set) pair of mutually exclusive sets of time slots
	QTabWidget* tabWidgetPairOfMutuallyExclusiveSets;

	QGroupBox* first_filterGroupBox;
	QGroupBox* second_filterGroupBox;
	QGroupBox* third_filterGroupBox;

	QComboBox* first_filterTeachersComboBox;
	QComboBox* second_filterTeachersComboBox;
	QComboBox* third_filterTeachersComboBox;

	QComboBox* first_filterStudentsComboBox;
	QComboBox* second_filterStudentsComboBox;
	QComboBox* third_filterStudentsComboBox;

	QComboBox* first_filterSubjectsComboBox;
	QComboBox* second_filterSubjectsComboBox;
	QComboBox* third_filterSubjectsComboBox;

	QComboBox* first_filterActivityTagsComboBox;
	QComboBox* second_filterActivityTagsComboBox;
	QComboBox* third_filterActivityTagsComboBox;

	bool filterIsOnSingleRow;

	QSet<QString> showedStudents;

	QSet<QString> firstFilter_showedStudents;
	QSet<QString> secondFilter_showedStudents;
	QSet<QString> thirdFilter_showedStudents;

public:
	AddOrModifyTimeConstraint(QWidget* parent, int _type, TimeConstraint* _oldtc=nullptr,
	 const QString& _preselectedTeacherName=QString(), const QString& _preselectedStudentsSetName=QString(), const QString& _preselectedActivityTagName=QString(),
	 const QString& _preselectedFirstActivityTagName=QString(), const QString& _preselectedSecondActivityTagName=QString());
	~AddOrModifyTimeConstraint();

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

	void addAllActivityTagsClicked();
	void clearActivityTagsClicked();
	void addActivityTag();
	void removeActivityTag();

	bool filterOk(Activity* act);
	int filterActivitiesComboBox(); //returns the index of the current constraint's activity in modify dialog
	void filterActivitiesListWidget();
	void filterActivitiesListWidgets1And2();

	bool first_filterOk(Activity* act);
	bool second_filterOk(Activity* act);
	bool third_filterOk(Activity* act);
	int first_activitiesComboBoxFilter(); //returns the index of the current constraint's activity in modify dialog
	int second_activitiesComboBoxFilter();
	int third_activitiesComboBoxFilter();

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

	void durationCheckBoxToggled();

	void swapActivitiesPushButtonClicked();
	void swapActivityTagsPushButtonClicked();

	void swapTwoSetsOfActivitiesPushButtonClicked();
	//
	void addAllActivitiesClicked1();
	void clearActivitiesClicked1();
	void addActivity1();
	void removeActivity1();
	//
	void addAllActivitiesClicked2();
	void clearActivitiesClicked2();
	void addActivity2();
	void removeActivity2();

	void showRelatedCheckBoxToggled();

	void firstFilter_showRelatedCheckBoxToggled();
	void secondFilter_showRelatedCheckBoxToggled();
	void thirdFilter_showRelatedCheckBoxToggled();
	
	void initOccupyMaxTableWidget();
	void fillSpinBoxTimesTable(const QList<QList<int>>& days, const QList<QList<int>>& hours);
	void getSpinBoxTimesTable(QList<QList<int>>& days, QList<QList<int>>& hours);

	void colorSpinBoxWithPointer(QSpinBox* sb);
	void colorSpinBox();
	void colorAllSpinBoxes();
};

#endif
