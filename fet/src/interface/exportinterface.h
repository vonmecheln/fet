/*
File exportinterface.h
*/

/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************
                          exportinterface.h  -  description
                             -------------------
    begin                : 2026
    copyright            : (C) by Volker Dirr
                         : https://www.timetabling.de/
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef EXPORTINTERFACE_H
#define EXPORTINTERFACE_H

#include "timetable.h"
#include "timetable_defs.h"

class QWidget;

#include <QDialog>
#include <QMessageBox>

class Export: public QObject{
	Q_OBJECT

public:
	Export();
	~Export();

	static void exportCSV(QWidget* parent);
private:
	static bool okToWrite(QWidget* parent, const QString& file, QMessageBox::StandardButton& msgBoxButton);

	static bool checkComponentSeparator(const QString& str, const QString& componentSeparator);
	static bool checkSetSeparator(const QString& str, const QString& setSeparator);
	static QString protectCSV(const QString& str);
	static QString protectCSVComments(const QString& str);

	static bool isActivityNotManuallyEditedPart1(int activityIndex, bool& diffTeachers, bool& diffSubject, bool& diffActivityTags, bool& diffStudents,
		QString& tl, QString& sl, QString& atl, QString& stl);
	static bool isActivityNotManuallyEditedPart2(int activityIndex, bool& diffCompNStud, bool& diffNStud, bool& diffActive);

	static bool selectSeparatorAndTextQuote(QWidget* parent, QDialog* &newParent, QString& textquote, QString& fieldSeparator, bool& head);

	static bool exportCSVActivities(QWidget* parent, QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, QMessageBox::StandardButton& msgBoxButton);
	static bool exportCSVActivitiesStatistics(QWidget* parent, QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, QMessageBox::StandardButton& msgBoxButton);
	static bool exportCSVActivityTags(QWidget* parent, QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, const QString& componentSeparator, const QString& setSeparator, QMessageBox::StandardButton& msgBoxButton);
	static bool exportCSVRoomsAndBuildings(QWidget* parent, QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, QMessageBox::StandardButton& msgBoxButton);
	static bool exportCSVSubjects(QWidget* parent, QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, const QString& componentSeparator, QMessageBox::StandardButton& msgBoxButton);
	static bool exportCSVTeachers(QWidget* parent, QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, const QString& componentSeparator, const QString& setSeparator, QMessageBox::StandardButton& msgBoxButton);
	static bool exportCSVStudents(QWidget* parent, QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, const QString& componentSeparator, const QString& setSeparator, QMessageBox::StandardButton& msgBoxButton);
	static bool exportCSVTimetable(QWidget* parent, QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, QMessageBox::StandardButton& msgBoxButton);
};

void exportExportCSV(QWidget* parent);

class LastWarningsDialogE: public QDialog{
	Q_OBJECT
	
public:				//could we do that private, too?
	LastWarningsDialogE(QWidget *parent, const QString& lastWarning);
	~LastWarningsDialogE();
};

#endif
