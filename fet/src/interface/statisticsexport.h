/*
File statisticsexport.h
*/

/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************
                      statisticsexport.h  -  description
                             -------------------
    begin                : Sep 2008
    copyright            : (C) by Volker Dirr
                         : https://www.timetabling.de/
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef STATISTICEXPORT_H
#define STATISTICEXPORT_H

#include <QString>
#include <QStringList>

#include <QHash>
#include <QMultiHash>

class QWidget;

#include "timetable.h"		//needed?
#include "timetable_defs.h"	//needed?

class FetStatistics{
public:
	//can be replaced by the key if I use SQL or other database
	QHash<QString, QString> hashSubjectIDsStatistics;
	QHash<QString, QString> hashActivityTagIDsStatistics;
	QHash<QString, QString> hashStudentIDsStatistics;
	QHash<QString, QString> hashTeacherIDsStatistics;
	//QHash<QString, QString> hashRoomIDsStatistics;
	//QHash<QString, QString> hashDayIDsStatistics;
	
	//needed values
	QHash<QString, int> teachersTotalNumberOfHours;
	QHash<QString, int> teachersTotalNumberOfHours2;
	QHash<QString, int> studentsTotalNumberOfHours;
	QHash<QString, int> studentsTotalNumberOfHours2;
	QHash<QString, int> subjectsTotalNumberOfHours;
	QHash<QString, int> subjectsTotalNumberOfHours4;
	QStringList allStudentsNames;				//NOT QSet<QString>!!! Because that does an incorrect order of the lists!
	QStringList allTeachersNames;				//NOT QSet<QString>!!! Because that does an incorrect order of the lists!
	QStringList allSubjectsNames;				//NOT QSet<QString>!!! Because that does an incorrect order of the lists!
	QMultiHash<QString, int> studentsActivities;
	QMultiHash<QString, int> teachersActivities;
	QMultiHash<QString, int> subjectsActivities;
};


class StatisticsExport: public QObject{
	Q_OBJECT

public:
	StatisticsExport();
	~StatisticsExport();
	
	static void exportStatistics(QWidget* parent);
	
	static QString exportStatisticsTeachersSubjectsHtml(QWidget* parent, const QString& saveTime, const FetStatistics& statisticValues, int htmlLevel, bool printActivityTags, int maxNames, QSet<int>* excludedNames);
	static QString exportStatisticsSubjectsTeachersHtml(QWidget* parent, const QString& saveTime, const FetStatistics& statisticValues, int htmlLevel, bool printActivityTags, int maxNames, QSet<int>* excludedNames);
	static QString exportStatisticsTeachersStudentsHtml(QWidget* parent, const QString& saveTime, const FetStatistics& statisticValues, int htmlLevel, bool printActivityTags, int maxNames, QSet<int>* excludedNames);
	static QString exportStatisticsStudentsTeachersHtml(QWidget* parent, const QString& saveTime, const FetStatistics& statisticValues, int htmlLevel, bool printActivityTags, int maxNames, QSet<int>* excludedNames);
	static QString exportStatisticsSubjectsStudentsHtml(QWidget* parent, const QString& saveTime, const FetStatistics& statisticValues, int htmlLevel, bool printActivityTags, int maxNames, QSet<int>* excludedNames);
	static QString exportStatisticsStudentsSubjectsHtml(QWidget* parent, const QString& saveTime, const FetStatistics& statisticValues, int htmlLevel, bool printActivityTags, int maxNames, QSet<int>* excludedNames);
	
	//this function must be called before export HTML files, because it computes the IDs
	static void computeHashForIDsStatistics(FetStatistics* statisticValues);
	static void getNamesAndHours(FetStatistics* statisticValues);
private:
	//the following functions write the CSS and HTML statistics files
	static bool exportStatisticsStylesheetCss(QWidget* parent, const QString& saveTime, const FetStatistics& statisticValues);
	static bool exportStatisticsIndex(QWidget* parent, const QString& saveTime);
	static bool exportStatisticsTeachersSubjects(QWidget* parent, const QString& saveTime, const FetStatistics& statisticValues, int htmlLevel);
	static bool exportStatisticsSubjectsTeachers(QWidget* parent, const QString& saveTime, const FetStatistics& statisticValues, int htmlLevel);
	static bool exportStatisticsTeachersStudents(QWidget* parent, const QString& saveTime, const FetStatistics& statisticValues, int htmlLevel);
	static bool exportStatisticsStudentsTeachers(QWidget* parent, const QString& saveTime, const FetStatistics& statisticValues, int htmlLevel);
	static bool exportStatisticsSubjectsStudents(QWidget* parent, const QString& saveTime, const FetStatistics& statisticValues, int htmlLevel);
	static bool exportStatisticsStudentsSubjects(QWidget* parent, const QString& saveTime, const FetStatistics& statisticValues, int htmlLevel);
};

#endif
