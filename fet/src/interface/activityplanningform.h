/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************
                      activityplanningform.h  -  description
                             -------------------
    begin                : 2009
    copyright            : (C) 2009 by Volker Dirr
                         : https://www.timetabling.de/
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef ACTIVITYPLANNINGFORM_H
#define ACTIVITYPLANNINGFORM_H

#include <QResizeEvent>

#include <QObject>

#include "sparsetableview.h"

#include <QList>

#include <QDialog>

class QTableWidget;
class QRadioButton;
class QCheckBox;
class QPushButton;
class QTableWidgetItem;
class QComboBox;
class QGroupBox;
class QToolButton;
class QSizePolicy;
class QSplitter;

class StartActivityPlanning{
public:
	StartActivityPlanning();
	~StartActivityPlanning();

	static void startActivityPlanning(QWidget* parent);
};

class ActivityPlanningForm: public QDialog{
	Q_OBJECT
	
public:
	ActivityPlanningForm(QWidget *parent);
	~ActivityPlanningForm();

private:
	bool buttonsVisible;

	QSplitter* leftSplitter;
	SparseTableView* activitiesTableView;
	SparseTableView* teachersTableView;
	
	int storeStudentsForSwap;
	int storeSubjectForSwap;
	void swapTeachers(int studentsActivity1, int subjectActivity1, int studentsActivity2, int subjectActivity2);
	void swapStudents(int studentsActivity1, int subjectActivity1, int studentsActivity2, int subjectActivity2);
	
	QRadioButton* RBActivity;
	QRadioButton* RBSubactivity;
	QRadioButton* RBAdd;
	QRadioButton* RBModify;
	QRadioButton* RBDelete;
	QRadioButton* RBChangeTeacher;
	QRadioButton* RBSwapTeachers;
	QRadioButton* RBSwapStudents;
	QComboBox* CBActive;
	QCheckBox* showDuplicates;
	QCheckBox* showYears;
	QCheckBox* showGroups;
	QCheckBox* showSubgroups;
	QCheckBox* showTeachers;
	QCheckBox* showActivityTags;
	QCheckBox* hideEmptyLines;
	QCheckBox* hideUsedTeachers;
	QCheckBox* swapAxes;
	QPushButton* pbPseudoActivities;
	QPushButton* pbDeleteAll;
	//QPushButton* pbHelp;
	
	QToolButton* showHideButton;
	QSizePolicy origShowHideSizePolicy;
	
	QPushButton* pbClose;
	QCheckBox* changedActivities;
	
	QTabWidget *actionsOptionsTabWidget;
	
	void computeActivitiesForDeletion(const QString& teacherName, const QString& studentsSetName, const QString& subjectName,
		const QList<int>& tmpID, const QList<int>& tmpGroupID,
		int& nTotalActsDeleted,
		QList<int>& _idsToBeRemoved, QList<int>& _agidsToBeRemoved,
		QList<bool>& _affectOtherTeachersToBeRemoved, bool& _affectOtherTeachersOverall,
		QList<bool>& _affectOtherStudentsSetsToBeRemoved, bool& _affectOtherStudentsSetsOverall,
		QList<bool>& _affectOtherSubjectsToBeRemoved, bool& _affectOtherSubjectsOverall);
	
	void computeActivitiesForModification(const QString& teacherName, const QString& studentsSetName, const QString& subjectName,
		const QList<int>& tmpID, const QList<int>& tmpGroupID,
		int& nTotalActsModified,
		QList<int>& _idsToBeModified, QList<int>& _agidsToBeModified,
		QList<bool>& _affectOtherTeachersToBeModified, bool& _affectOtherTeachersOverall,
		QList<bool>& _affectOtherStudentsSetsToBeModified, bool& _affectOtherStudentsSetsOverall,
		QList<bool>& _affectOtherSubjectsToBeModified, bool& _affectOtherSubjectsOverall);
	
private Q_SLOTS:
	//void activitiesCellSelected(int, int);
	void activitiesCellSelected(const QModelIndex& index);
	//void teachersCellSelected(QTableWidgetItem*);
	void teachersCellSelected(const QModelIndex& index);
	//mouseTracking (start 1/4)
	//void ActivitiesCellEntered(int, int);
	//void TeachersCellEntered(int, int);
	//mouseTracking (end 1/4)
	void activitiesTableHorizontalHeaderClicked(int column);
	void activitiesTableVerticalHeaderClicked(int row);
	void teachersTableHorizontalHeaderClicked(int column);
	void updateTables();
	void updateTables_Students_Subjects();
	void updateTables_Teachers();
	void updateTablesVisual();
	//void help();
	void pseudoActivities();
	void deleteAll();
	
	void showHide();
	
/*protected:
	void resizeEvent(QResizeEvent* event);*/
};

//communication spin box by Liviu Lalescu
class PlanningCommunicationSpinBox: public QObject{
	Q_OBJECT

private:
	int value;
	int maxValue;
	int minValue;
	
public:
	PlanningCommunicationSpinBox();
	~PlanningCommunicationSpinBox();
	
Q_SIGNALS:
	int valueChanged(int newValue);
	
public Q_SLOTS:
	void increaseValue();
};

class PlanningChanged{
public:
	static void increasePlanningCommunicationSpinBox();
};

#endif
