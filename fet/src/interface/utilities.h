/*
File utilities.h
*/

/***************************************************************************
                          utilities.h  -  description
                             -------------------
    begin                : 13 July 2008
    copyright            : (C) 2008 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef UTILITIES_H
#define UTILITIES_H

#include "matrix.h"

class Rules;

#ifndef FET_COMMAND_LINE
#include <QString>

#include <QList>

class QComboBox;

class QWidget;
class QTableView;
class QTableWidgetItem;

#include <QTableWidget>
#include <QSpinBox>

class CornerEnabledTableWidget: public QTableWidget
{
public:
	bool useColors;
	bool V;

	CornerEnabledTableWidget(bool _useColors, bool _V);

private:
	void selectAll();
};

class CornerEnabledTableWidgetOfSpinBoxes: public QTableWidget
{
public:
	CornerEnabledTableWidgetOfSpinBoxes();

private:
	void selectAll();
};

void centerWidgetOnScreen(QWidget* widget);
void forceCenterWidgetOnScreen(QWidget* widget);
//void centerWidgetOnParent(QWidget* widget, QWidget* parent);

int maxScreenWidth(QWidget* widget);

int maxRecommendedWidth(QWidget* widget);

void saveFETDialogGeometry(QWidget* widget, const QString& alternativeName=QString());
void restoreFETDialogGeometry(QWidget* widget, const QString& alternativeName=QString());

void setParentAndOtherThings(QWidget* widget, QWidget* parent);

void setStretchAvailabilityTableNicely(CornerEnabledTableWidget* tableWidget);
void setStretchAvailabilityTableNicely(CornerEnabledTableWidgetOfSpinBoxes* tableWidget);

void setRulesModifiedAndOtherThings(Rules* rules);
void setRulesUnmodifiedAndOtherThings(Rules* rules);

void showWarningForInvisibleSubgroupConstraint(QWidget* parent, const QString& initialSubgroupName);
void showWarningCannotModifyConstraintInvisibleSubgroupConstraint(QWidget* parent, const QString& initialSubgroupName);
void showWarningForInvisibleSubgroupActivity(QWidget* parent, const QString& initialSubgroupName);

int populateStudentsComboBox(QComboBox* studentsComboBox, const QString& selectedStudentsSet=QString(""), bool addEmptyAtBeginning=false);

//void closeAllTimetableViewDialogs();
void updateAllTimetableViewDialogs();

void highlightOnHorizontalHeaderClicked(CornerEnabledTableWidget* tableWidget, int col);
void highlightOnVerticalHeaderClicked(CornerEnabledTableWidget* tableWidget, int row);
void highlightOnCellEntered(CornerEnabledTableWidget* tableWidget, int row, int col);

void tableViewSetHighlightHeader(QTableView* tableWidget);

void initTimesTable(CornerEnabledTableWidget* timesTable);
void fillTimesTable(CornerEnabledTableWidget* timesTable, const QList<int>& days, const QList<int>& hours, bool direct);
void getTimesTable(CornerEnabledTableWidget* timesTable, QList<int>& days, QList<int>& hours, bool direct);

void colorItemTimesTable(CornerEnabledTableWidget* timesTable, QTableWidgetItem* item);
void itemClickedTimesTable(CornerEnabledTableWidget* timesTable, QTableWidgetItem* item);
void horizontalHeaderClickedTimesTable(CornerEnabledTableWidget* timesTable, int col);
void verticalHeaderClickedTimesTable(CornerEnabledTableWidget* timesTable, int row);
void cellEnteredTimesTable(CornerEnabledTableWidget* timesTable, int row, int col);
void colorsCheckBoxToggledTimesTable(CornerEnabledTableWidget* timesTable);
void toggleAllClickedTimesTable(CornerEnabledTableWidget* timesTable);

#else
void setRulesModifiedAndOtherThings(Rules* rules);
#endif

#endif
