/*
File exportcl.h
*/

/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************
                          exportcl.h  -  description
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

#ifndef EXPORTCL_H
#define EXPORTCL_H

#include "timetable.h"
#include "timetable_defs.h"

class QWidget;

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

void exportExportCSV(Solution* bestOrHighest, Solution* current=nullptr);

#endif
