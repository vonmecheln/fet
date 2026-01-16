/***************************************************************************
                          timetablegeneratemultipleform.h  -  description
                             -------------------
    begin                : Aug 20 2007
    copyright            : (C) 2007 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef TIMETABLEGENERATEMULTIPLEFORM_H
#define TIMETABLEGENERATEMULTIPLEFORM_H

#include "ui_timetablegeneratemultipleform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "solution.h"

//#include <QThread>

#include <QList>

#include <thread>

class TimetablingThread: public QObject
{
	Q_OBJECT

public:
	//std::thread _internalGeneratingThread;
	int _nThread;
	int nOverallTimetable;

	void startGenerating();

Q_SIGNALS:
	void timetableGenerated(int nThread, int timetable, const QString& description, bool ok);
};

/*class Worker: public QObject
{
	Q_OBJECT

signals:
	void resultReady(int nThread, const QString& description, bool ok);

public slots:
	void doWork(int _nThread);
};

class Controller: public QObject
{
	Q_OBJECT

public:
	QThread workerThread;
	
	int _nThread;
	int nOverallTimetable;

	Controller();
	~Controller();
	
	void startOperate(int nThread);
	
public slots:
	void handleResults(int nThread, const QString& description, bool ok);

signals:
	void operate(int nThread);

	void timetableGenerated(int nThread, int timetable, const QString& description, bool ok);
};*/

class TimetableGenerateMultipleForm : public QDialog, Ui::TimetableGenerateMultipleForm_template  {
	Q_OBJECT

	QList<QLabel*> labels;
	QList<QLabel*> pausedLabels;
	
	QList<QPushButton*> restartPushButtons;
	QList<QPushButton*> pausePushButtons;
	
	//QList<Controller*> controllersList;
	
	time_t all_processes_start_time;
	
	int nGeneratedTimetables;
	int nSuccessfullyGeneratedTimetables;
	int highestPlacedActivities; //if no timetable was finished, we save the first partial best.
	QList<Solution*> highestStageSolutions;
	QList<int> nTimetableForHighestStageSolutions;
	QList<int> nThreadForHighest;
	QList<bool> generationTimedOutForHighest;
	QList<int> timeForHighestStageSolutions;

public:
	TimetableGenerateMultipleForm(QWidget* parent);
	~TimetableGenerateMultipleForm();
	
	void writeTimetableDataFile();

public Q_SLOTS:
	void help();

	void start();
	
	void restartCurrentThread();

	void stop();
	
	void closePressed();
	
private Q_SLOTS:
	void nThreadsChanged(int nt);

	void timetableStarted(int nThread, int timetable);

	void timetableGenerated(int nThread, int timetable, const QString& description, bool ok);
	
	void generationFinished();
	
	void activityPlaced(int nThread, int na);
	
	void seeInitialOrder();
	
	void pauseCurrentThread();
};

#endif
