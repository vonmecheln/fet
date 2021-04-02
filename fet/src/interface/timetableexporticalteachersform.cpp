//
//
// C++ Implementation: $MODULE$
//
// Description: 
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "genetictimetable_defs.h"
#include "genetictimetable.h"
#include "fet.h"

#include "timetableexporticalteachersform.h"

#include <qlineedit.h>
#include <qmessagebox.h>
#include <qstring.h>

#include <QDesktopWidget>

extern GeneticTimetable gt;

TimetableExportiCalTeachersForm::TimetableExportiCalTeachersForm()
 : TimetableExportiCalTeachersForm_template()
{
	//setWindowFlags(Qt::Window);
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	//some predefined values
	startHourLineEdit->setText("0800");
	lessonLengthLineEdit->setText("0100");
	breakLengthLineEdit->setText("0000");
	startDayLineEdit->setText("20060102");
}


TimetableExportiCalTeachersForm::~TimetableExportiCalTeachersForm()
{
}

void TimetableExportiCalTeachersForm::ok()
{
	//verify the data
	if(!isCorrectHour(startHourLineEdit->text())){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Invalid start hour.\nPlease modify it."));

		return;
	}

	if(!isCorrectHour(lessonLengthLineEdit->text())){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Invalid lesson length.\nPlease modify it."));

		return;
	}

	if(!isCorrectHour(breakLengthLineEdit->text())){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Invalid break length.\nPlease modify it."));

		return;
	}
	
	QString t2=startHourLineEdit->text();
	QString t3;
	QString t4;
	bool t=sumHours(lessonLengthLineEdit->text(), breakLengthLineEdit->text(), t3);
	for(int i=0; i<gt.rules.nHoursPerDay; i++){
		bool t5=sumHours(t2, t3, t4);
		t2=t4;
		if(t5){
			t=true;
			break;
		}
	}
	if(t){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Lessons for a specific day do not fit in a single day - impossible"));

		return;
	}

	if(!isCorrectDay(startDayLineEdit->text())){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Invalid start date.\nPlease modify it."));

		return;
	}

	for(int i=0; i<gt.rules.nInternalTeachers; i++){
		QString t=protect3(gt.rules.internalTeachersList[i]->name);
		QString fileName=OUTPUT_DIR+FILE_SEP+QObject::tr("teacher")+t+".ics";
		
		ofstream out(fileName);
		if(!out){
			assert(0);
			exit(1);
		}
		
		out<<"BEGIN:VCALENDAR"<<endl;
		out<<"PRODID:-//FET//NONSGML FET "<< (const char*)(FET_VERSION) <<"//EN"<<endl;
		out<<"VERSION:2.0"<<endl;
		
		out<<endl;
		
		QString s;
		
		QString date=startDayLineEdit->text();
		QString startHour;
		QString endHour;
		
		for(int k=0; k<gt.rules.nDaysPerWeek; k++){
			if(k>0)
				date=nextDay(date);
				
			startHour=startHourLineEdit->text();
			bool t=sumHours(startHour, lessonLengthLineEdit->text(), endHour);
			assert(!t);
			
			for(int j=0; j<gt.rules.nHoursPerDay; j++){
				if(j>0){
					QString t;
					bool t2=sumHours(startHour, lessonLengthLineEdit->text(), t);
					bool t3=sumHours(t, breakLengthLineEdit->text(), startHour);
					assert(!(t2||t3));

					bool t4=sumHours(startHour, lessonLengthLineEdit->text(), endHour);
					assert(!t4);
				}
				
				s = "";
				int ai=teachers_timetable_week1[i][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					assert(act!=NULL);
					s += act->subjectName+" "+act->subjectTagName+" ";
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						s += (*it) + " ";
				}
				ai=teachers_timetable_week2[i][k][j]; //activity index
				if(ai!=UNALLOCATED_ACTIVITY){
					Activity* act=&gt.rules.internalActivitiesList[ai];
					assert(act!=NULL);
					if(!act->studentsNames.isEmpty())
						s+="/ ";
					s += act->subjectName+" "+act->subjectTagName+" ";
					for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++)
						s += (*it) + " ";
				}
				//Write s
				if(s!=""){
					out<<"BEGIN:VEVENT"<<endl;
					out<<(const char*)iCalFolding("SUMMARY:"+s)<<endl;
					out<<"CLASS:PUBLIC"<<endl;
					out<<"DTSTART:"<<(const char*)date<<"T"<<(const char*)startHour<<"00Z"<<endl;
					out<<"DTEND:"<<(const char*)date<<"T"<<(const char*)endHour<<"00Z"<<endl;
					out<<"END:VEVENT"<<endl;
					out<<endl;
				}
			}
		}
			
		out<<"END:VCALENDAR"<<endl;
		out<<endl;
	}
	
	QMessageBox::information(this, QObject::tr("FET information"),
		QObject::tr("iCal (*.ics) files written (one for each teacher).\nPlease check the %1 directory")
		.arg(OUTPUT_DIR));	

	this->close();
}

void TimetableExportiCalTeachersForm::cancel()
{
	this->close();
}
