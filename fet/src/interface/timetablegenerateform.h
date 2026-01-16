/***************************************************************************
                          timetablegenerateform.h  -  description
                             -------------------
    begin                : Tue Apr 22 2003
    copyright            : (C) 2003 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef TIMETABLEGENERATEFORM_H
#define TIMETABLEGENERATEFORM_H

#include "ui_timetablegenerateform_template.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

//#include <QThread>

//#include <QProcess>
#include <QList>

/*class GenerateThread: public QThread{
	Q_OBJECT

public:
	void run();

signals:
	void generationComputed(int generation);
};*/

class TimetableGenerateForm : public QDialog, Ui::TimetableGenerateForm_template  {
	Q_OBJECT
	
//private:
//	QList<QProcess*> commandProcesses;

private:
	QString initialOutputDirectory;

public:
	TimetableGenerateForm(QWidget* parent);
	~TimetableGenerateForm();

public Q_SLOTS:
	void start();
	void stop();
	void stopHighest();
	void write();
	void closePressed();
	void help();
	void writeHighestStage();

	//void generationLogging(int generation);

private Q_SLOTS:
	void activityPlaced(int nThread, int na);

	void generationFinished();

	void impossibleToSolve();
	
	void seeImpossible();
	
	void seeInitialOrder();
	
	void pause();
	
//	void seeHighestStage();
};

#endif
