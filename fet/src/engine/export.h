/*
File export.h
*/

/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************
                          export.h  -  description
                             -------------------
    begin                : Mar 2008
    copyright            : (C) by Volker Dirr
                         : https://www.timetabling.de/
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef EXPORT_H
#define EXPORT_H

#include "timetable.h"
#include "timetable_defs.h"

class QWidget;

#ifndef FET_COMMAND_LINE
#include <QDialog>
#include <QMessageBox>
#endif

#ifndef FET_COMMAND_LINE
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
#else
class Export: public QObject{
	Q_OBJECT

public:
	Export();
	~Export();

	static void exportCSV(Solution* bestOrHighest, Solution* current=nullptr);
private:
	static bool okToWrite(const QString& file);

	static bool checkComponentSeparator(const QString& str, const QString& componentSeparator);
	static bool checkSetSeparator(const QString& str, const QString& setSeparator);
	static QString protectCSV(const QString& str);
	static QString protectCSVComments(const QString& str);

	static bool isActivityNotManuallyEditedPart1(int activityIndex, bool& diffTeachers, bool& diffSubject, bool& diffActivityTags, bool& diffStudents,
		QString& tl, QString& sl, QString& atl, QString& stl);
	static bool isActivityNotManuallyEditedPart2(int activityIndex, bool& diffCompNStud, bool& diffNStud, bool& diffActive);

	static bool exportCSVActivities(QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head);
	static bool exportCSVActivitiesStatistics(QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head);
	static bool exportCSVActivityTags(QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, const QString& componentSeparator, const QString& setSeparator);
	static bool exportCSVRoomsAndBuildings(QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head);
	static bool exportCSVSubjects(QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, const QString& componentSeparator);
	static bool exportCSVTeachers(QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, const QString& componentSeparator, const QString& setSeparator);
	static bool exportCSVStudents(QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head, const QString& componentSeparator, const QString& setSeparator);
	static bool exportCSVTimetable(QString& lastWarnings, const QString& textquote, const QString& fieldSeparator, const bool head);
};
#endif

#ifndef FET_COMMAND_LINE
class LastWarningsDialogE: public QDialog{
	Q_OBJECT
	
public:				//can I do that private too?
	LastWarningsDialogE(QWidget *parent, const QString& lastWarning);
	~LastWarningsDialogE();
};
#endif

#endif
