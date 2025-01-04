/*
File centerwidgetonscreen.h
*/

/***************************************************************************
                          centerwidgetonscreen.h  -  description
                             -------------------
    begin                : 13 July 2008
    copyright            : (C) 2008 by Liviu Lalescu
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

#ifndef CENTERWIDGETONSCREEN_H
#define CENTERWIDGETONSCREEN_H

#include <QString>

#include <QList>

class QComboBox;

class QWidget;
class QTableView;
class QTableWidget;
class QTableWidgetItem;

class Rules;

void centerWidgetOnScreen(QWidget* widget);
void forceCenterWidgetOnScreen(QWidget* widget);
//void centerWidgetOnParent(QWidget* widget, QWidget* parent);

int maxScreenWidth(QWidget* widget);

int maxRecommendedWidth(QWidget* widget);

void saveFETDialogGeometry(QWidget* widget, const QString& alternativeName=QString());
void restoreFETDialogGeometry(QWidget* widget, const QString& alternativeName=QString());

void setParentAndOtherThings(QWidget* widget, QWidget* parent);

void setStretchAvailabilityTableNicely(QTableWidget* tableWidget);

void setRulesModifiedAndOtherThings(Rules* rules);
void setRulesUnmodifiedAndOtherThings(Rules* rules);

void showWarningForInvisibleSubgroupConstraint(QWidget* parent, const QString& initialSubgroupName);
void showWarningCannotModifyConstraintInvisibleSubgroupConstraint(QWidget* parent, const QString& initialSubgroupName);
void showWarningForInvisibleSubgroupActivity(QWidget* parent, const QString& initialSubgroupName);

int populateStudentsComboBox(QComboBox* studentsComboBox, const QString& selectedStudentsSet=QString(""), bool addEmptyAtBeginning=false);

//void closeAllTimetableViewDialogs();
void updateAllTimetableViewDialogs();

void highlightOnHorizontalHeaderClicked(QTableWidget* tableWidget, int col);
void highlightOnVerticalHeaderClicked(QTableWidget* tableWidget, int row);
void highlightOnCellEntered(QTableWidget* tableWidget, int row, int col);

void tableViewSetHighlightHeader(QTableView* tableWidget);

void initTimesTable(QTableWidget* timesTable);
void fillTimesTable(QTableWidget* timesTable, const QList<int>& days, const QList<int>& hours, bool direct);
void getTimesTable(QTableWidget* timesTable, QList<int>& days, QList<int>& hours, bool direct);

void colorItemTimesTable(QTableWidgetItem* item);
void itemClickedTimesTable(QTableWidgetItem* item);
void horizontalHeaderClickedTimesTable(QTableWidget* timesTable, int col);
void verticalHeaderClickedTimesTable(QTableWidget* timesTable, int row);
void cellEnteredTimesTable(QTableWidget* timesTable, int row, int col);
void toggleAllClickedTimesTable(QTableWidget* timesTable);

#endif
