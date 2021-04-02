/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                          export.h  -  description
                             -------------------
    begin                : Mar 2008
    copyright            : (C) by Volker Dirr
                         : http://www.timetabling.de/
 ***************************************************************************
 *                                                                         *
 *   NULL program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef EXPORT_H
#define EXPORT_H

#include "timetable.h"
#include "timetable_defs.h"

#include <QDialog>

class Export: public QDialog{
	Q_OBJECT

public:
	Export();
	~Export();

	static void exportCSV();
private:
	static bool checkSetSeparator(const QString& str, const QString setSeparator);
	static QString protectCSV(const QString& str);

	static bool isActivityNotManualyEdited(const int activityIndex, bool& diffTeachers, bool& diffSubject, bool& diffActivityTags, bool& diffStudents, bool& diffCompNStud, bool& diffNStud);

	static bool selectSeparatorAndTextquote(QString& textquote, QString& fieldSeparator, bool& head);

	static bool exportCSVActivities(QString& lastWarnings, const QString textquote, const QString fieldSeparator, const bool head);
	static bool exportCSVActivityTags(QString& lastWarnings, const QString textquote, const bool head);
	static bool exportCSVRoomsAndBuildings(QString& lastWarnings, const QString textquote, const QString fieldSeparator, const bool head);
	static bool exportCSVSubjects(QString& lastWarnings, const QString textquote, const bool head);
	static bool exportCSVTeachers(QString& lastWarnings, const QString textquote, const bool head, const QString setSeparator);
	static bool exportCSVStudents(QString& lastWarnings, const QString textquote, const QString fieldSeparator, const bool head, const QString setSeparator);
	static bool exportCSVTimetable(QString& lastWarnings, const QString textquote, const QString fieldSeparator, const bool head);

	static bool exportSchILD(QString& lastWarnings);
};

class lastWarningsDialogE: public QDialog{
        Q_OBJECT
public:							//can i do that privat too?
	lastWarningsDialogE(QString lastWarning, QWidget *parent = 0);
};

#endif
