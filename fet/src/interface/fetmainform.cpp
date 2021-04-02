//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <iostream>
using namespace std;

#include "timetable_defs.h"
#include "timetable.h"
#include "solution.h"

#include "fetmainform.h"

#include "timetablegenerateform.h"
#include "timetablegeneratemultipleform.h"

#include "timetableviewstudentsform.h"
#include "timetableviewteachersform.h"
#include "timetableshowconflictsform.h"
#include "timetableviewroomsform.h"

#include "institutionnameform.h"
#include "commentsform.h"
#include "daysform.h"
#include "hoursform.h"
#include "subjectsform.h"
#include "subjectsstatisticsform.h"
#include "subjecttagsform.h"
#include "teachersform.h"
#include "teachersstatisticsform.h"
#include "yearsform.h"
#include "groupsform.h"
#include "subgroupsform.h"
#include "studentsstatisticsform.h"
#include "activitiesform.h"
#include "roomsform.h"
#include "alltimeconstraintsform.h"
#include "allspaceconstraintsform.h"
#include "helpaboutform.h"
#include "helpfaqform.h"
#include "helpinstructionsform.h"

#include "fet.h"

#include "constraint2activitiesconsecutiveform.h"
#include "constraintactivitiespreferredtimesform.h"
#include "constraintactivitiessamestartingtimeform.h"
#include "constraintactivitiessamestartinghourform.h"
#include "constraintteachernotavailableform.h"
#include "constraintbasiccompulsorytimeform.h"
#include "constraintbasiccompulsoryspaceform.h"
#include "constraintroomnotavailableform.h"
#include "constraintactivitypreferredroomform.h"
#include "constraintstudentssetnotavailableform.h"
#include "constraintbreakform.h"
#include "constraintteachermaxdaysperweekform.h"
#include "constraintteachermaxhoursdailyform.h"
#include "constraintteachersmaxhoursdailyform.h"
#include "constraintactivitypreferredtimeform.h"
#include "constraintstudentssetnogapsform.h"
#include "constraintstudentsnogapsform.h"
#include "constraintteachersmaxgapsperweekform.h"
#include "constraintteachermaxgapsperweekform.h"
#include "constraintstudentsearlyform.h"
#include "constraintstudentssetearlyform.h"
#include "constraintstudentssetmaxhoursdailyform.h"
#include "constraintstudentsmaxhoursdailyform.h"
#include "constraintstudentssetminhoursdailyform.h"
#include "constraintstudentsminhoursdailyform.h"
#include "constraintactivitiesnotoverlappingform.h"
#include "constraintminndaysbetweenactivitiesform.h"
#include "constraintactivitypreferredtimesform.h"
#include "constraintactivitypreferredroomsform.h"
#include "constraintsubjectpreferredroomform.h"
#include "constraintsubjectpreferredroomsform.h"
#include "constraintsubjectsubjecttagpreferredroomform.h"
#include "constraintsubjectsubjecttagpreferredroomsform.h"

#include <qmessagebox.h>
#include <q3filedialog.h>
#include <qstring.h>
#include <qdir.h>
#include <q3popupmenu.h>
//Added by qt3to4:
#include <QTranslator>

#include <QCloseEvent>

#include "httpget.h"

bool simulation_running; //true if the user started an allocation of the timetable

bool simulation_running_multi;

bool students_schedule_ready;
bool teachers_schedule_ready;
bool rooms_schedule_ready;

Solution best_solution;

QString conflictsString; //the string that contains a log of the broken constraints

extern QApplication* pqapplication;

#include <QDesktopWidget>

#include <QSettings>

static HttpGet getter;

//static bool finishedSearchingForUpdates;

FetMainForm::FetMainForm()
{
	setupUi(this);

	QSettings settings("FET free software", "FET");
	QRect rect=settings.value("fetmainformgeometry", QRect(0,0,0,0)).toRect();
	
	if(!rect.isValid()){
		//setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
		QDesktopWidget* desktop=QApplication::desktop();
		int xx=desktop->width()/2 - frameGeometry().width()/2;
		int yy=desktop->height()/2 - frameGeometry().height()/2;
		move(xx, yy);
	}
	else{
		move(rect.topLeft());
		cout<<"read rect.x()=="<<rect.x()<<", rect.y()=="<<rect.y()<<endl;
		resize(rect.size());
	}

	languageMenu->setItemChecked(languageMenu->idAt(0), false);
	languageMenu->setItemChecked(languageMenu->idAt(1), false);
	languageMenu->setItemChecked(languageMenu->idAt(2), false);
	languageMenu->setItemChecked(languageMenu->idAt(3), false);
	languageMenu->setItemChecked(languageMenu->idAt(4), false);
	languageMenu->setItemChecked(languageMenu->idAt(5), false);
	languageMenu->setItemChecked(languageMenu->idAt(6), false);
	languageMenu->setItemChecked(languageMenu->idAt(7), false);
	languageMenu->setItemChecked(languageMenu->idAt(8), false);
	languageMenu->setItemChecked(languageMenu->idAt(9), false);
	languageMenu->setItemChecked(languageMenu->idAt(10), false);
	if(FET_LANGUAGE=="en_GB")
		languageMenu->setItemChecked(languageMenu->idAt(0), true);
	else if(FET_LANGUAGE=="fr")
		languageMenu->setItemChecked(languageMenu->idAt(1), true);
	else if(FET_LANGUAGE=="ca")
		languageMenu->setItemChecked(languageMenu->idAt(2), true);
	else if(FET_LANGUAGE=="ro")
		languageMenu->setItemChecked(languageMenu->idAt(3), true);
	else if(FET_LANGUAGE=="ms")
		languageMenu->setItemChecked(languageMenu->idAt(4), true);
	else if(FET_LANGUAGE=="pl")
		languageMenu->setItemChecked(languageMenu->idAt(5), true);
	else if(FET_LANGUAGE=="tr")
		languageMenu->setItemChecked(languageMenu->idAt(6), true);
	else if(FET_LANGUAGE=="nl")
		languageMenu->setItemChecked(languageMenu->idAt(7), true);
	else if(FET_LANGUAGE=="de")
		languageMenu->setItemChecked(languageMenu->idAt(8), true);
	else if(FET_LANGUAGE=="hu")
		languageMenu->setItemChecked(languageMenu->idAt(9), true);
	else if(FET_LANGUAGE=="mk")
		languageMenu->setItemChecked(languageMenu->idAt(10), true);

	//new data
	if(gt.rules.initialized)
		gt.rules.kill();
	gt.rules.init();

	bool tmp=gt.rules.addTimeConstraint(new ConstraintBasicCompulsoryTime(100));
	assert(tmp);
	tmp=gt.rules.addSpaceConstraint(new ConstraintBasicCompulsorySpace(100));
	assert(tmp);

	students_schedule_ready=false;
	teachers_schedule_ready=false;
	rooms_schedule_ready=false;
	
	languageMenu->setCheckable(true);
	
	checkForUpdatesAction->setCheckable(true);
	checkForUpdatesAction->setChecked(checkForUpdates);

	QObject::connect(&getter, SIGNAL(done(bool)), this, SLOT(httpDone(bool)));
	
	if(checkForUpdates){
		bool t=getter.getFile(QUrl("http://www.lalescu.ro/liviu/fet/crtversion/crtversion.txt"));
		assert(t);
	}
}

void FetMainForm::on_checkForUpdatesAction_toggled()
{
	checkForUpdates=checkForUpdatesAction->isChecked();
}

void FetMainForm::httpDone(bool error)
{
	if(error){
		QMessageBox::warning(this, QObject::tr("FET warning"), QObject::tr(
		 "Could not search for possible updates on internet - error message is: %1. "
		 "I am searching for the file http://www.lalescu.ro/liviu/fet/crtversion/crtversion.txt . "
		 "Maybe the current structure on web page was changed. Please visit FET web page"
		 " http://www.lalescu.ro/liviu/fet/ and get latest version or,"
		 " if the web page does not work, try to search for the new FET page on the internet."
		 " You can contact the author. Also, sometimes lalescu.ro might have temporary problems, try again later"
		 "\n\nIf you want, you can turn off automatic search for updates in Settings menu"
		 ).arg(getter.http.errorString()));
	}
	else{
		QString s;
		for(int c=0; c<internetVersion.count(); c++){
			s+=internetVersion[c];
			if((c+1)%64==0)
				s+=" ";
		}
	
		if(internetVersion!=FET_VERSION){
			QMessageBox::information(this, QObject::tr("FET information"),
			 QObject::tr("Another version: %1, is available on FET webpage: http://www.lalescu.ro/liviu/fet/\n\n"
			 "You have to manually download and install (open the FET webpage in an internet browser). "
			 "Please read the information on web page regarding the newer version and choose whether to keep you current version or upgrade."
			 " You might need to hit Refresh in your web browser if links do not work"
			 "\n\nYou can choose to disable automatic search for updates in the Settings menu")
			 .arg(s));
		}
	}
}

void FetMainForm::closeEvent(QCloseEvent* event)
{
	QSettings settings("FET free software", "FET");
	QRect rect(x(), y(), width(), height());
	settings.setValue("fetmainformgeometry", rect);
	cout<<"wrote x()=="<<x()<<", y()=="<<y()<<endl;
	
	if(event!=NULL)
		;

	switch(QMessageBox::information( this, QObject::tr("FET - exiting"),
	 QObject::tr("File not saved - do you want to save it?"),
	 QObject::tr("&Yes"), QObject::tr("&No"), QObject::tr("&Cancel"), 0 , 2 )){
	 	case 0: 
			this->on_fileSaveAction_activated();
			event->accept();
			break;
	 	case 1: 
			event->accept();
			break;
		case 2: 
			event->ignore();
			break;
	}

	//INPUT_FILENAME_XML = "";
}

FetMainForm::~FetMainForm()
{
	getter.http.abort();
}

void FetMainForm::on_fileExitAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	close();
}

void FetMainForm::on_fileNewAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	INPUT_FILENAME_XML="";
	
	setWindowTitle(QObject::tr("FET - a free evolutionary timetabling program"));

	int confirm=0;
	switch( QMessageBox::information( this, QObject::tr("FET application"),
	 QObject::tr("Are you sure you want to load new data (rules) ?"),
	 QObject::tr("&Yes"), QObject::tr("&No"), 0 , 1 ) ) {
	case 0: // Yes
		confirm=1;
		break;
	case 1: // No
		confirm=0;
		break;
	}

	if(confirm){
		if(gt.rules.initialized)
			gt.rules.kill();
		gt.rules.init();

		bool tmp=gt.rules.addTimeConstraint(new ConstraintBasicCompulsoryTime(100));
		assert(tmp);
		tmp=gt.rules.addSpaceConstraint(new ConstraintBasicCompulsorySpace(100));
		assert(tmp);

		students_schedule_ready=false;
		teachers_schedule_ready=false;
		rooms_schedule_ready=false;
	}
}

void FetMainForm::on_fileOpenAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	int confirm=1;

	if(confirm){
		QString s = Q3FileDialog::getOpenFileName(WORKING_DIRECTORY, QObject::tr("FET xml files (*.fet);;Old FET xml files (*.xml);;All files (*)"),
			this, QObject::tr("open file dialog"), QObject::tr("Choose a file"));
		if(s.isNull())
			return;
			
		if(s.indexOf("(") >= 0 || s.indexOf(")")>=0){
			QMessageBox::information(this, QObject::tr("FET info"), QObject::tr("Please do not use parantheses () in filename, the html code does not work"));
			return;
		}
		else{
			if(gt.rules.read(s)){
				students_schedule_ready=false;
				teachers_schedule_ready=false;
				rooms_schedule_ready=false;

				INPUT_FILENAME_XML = s;
			}
			else{
				QMessageBox::information(this, QObject::tr("FET info"), QObject::tr("Invalid file"), QObject::tr("&OK"));
			}
		}
		//get the directory
		int tmp=s.findRev("/");
		WORKING_DIRECTORY=s.left(tmp+1);
		
		setWindowTitle(QObject::tr("FET - %1").arg(s.right(s.length()-tmp-1)));
	}
}

void FetMainForm::on_fileSaveAsAction_activated()
{
	QString s = Q3FileDialog::getSaveFileName(
		INPUT_FILENAME_XML, QObject::tr("FET xml files (*.fet);;All files (*)"),
		this, QObject::tr("Save file dialog"), QObject::tr("Choose a filename to save under" ));
	if(s==QString::null)
		return;

	if(s.indexOf("(") >= 0 || s.indexOf(")")>=0){
		QMessageBox::information(this, QObject::tr("FET info"), QObject::tr("Please do not use parantheses () in filename, the html code does not work"));
		return;
	}
		
	if(s.right(4)!=".fet")
		s+=".fet";

	int tmp=s.findRev("/");
	WORKING_DIRECTORY=s.left(tmp+1);

	if(QFile::exists(s))
		if(QMessageBox::information( this, QObject::tr("FET"),
		 QObject::tr("File exists - are you sure you want to overwrite existing file?"),
		 QObject::tr("&Yes"), QObject::tr("&No"), 0 , 1 ) == 1)
		 	return;
			
	INPUT_FILENAME_XML = s;
	
	setWindowTitle(QObject::tr("FET - %1").arg(s.right(s.length()-tmp-1)));
	
	gt.rules.write(INPUT_FILENAME_XML);
}

void FetMainForm::on_fileSaveAction_activated()
{
	if(INPUT_FILENAME_XML == "")
		on_fileSaveAsAction_activated();
	else
		gt.rules.write(INPUT_FILENAME_XML);
}

void FetMainForm::on_dataInstitutionNameAction_activated()
{
	InstitutionNameForm* form=new InstitutionNameForm();
	form->show();
}

void FetMainForm::on_dataCommentsAction_activated()
{
	CommentsForm* form=new CommentsForm();
	form->show();
}

void FetMainForm::on_dataDaysAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	DaysForm* form=new DaysForm();
	form->exec();
}

void FetMainForm::on_dataHoursAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	HoursForm* form=new HoursForm();
	form->exec();
}

void FetMainForm::on_dataTeachersAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	TeachersForm* form=new TeachersForm();
	form->exec();
}

void FetMainForm::on_dataTeachersStatisticsAction_activated()
{
	TeachersStatisticsForm* form=new TeachersStatisticsForm();
	form->exec();
}

void FetMainForm::on_dataSubjectsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	SubjectsForm* form=new SubjectsForm();
	form->exec();
}

void FetMainForm::on_dataSubjectsStatisticsAction_activated()
{
	SubjectsStatisticsForm* form=new SubjectsStatisticsForm();
	form->exec();
}

void FetMainForm::on_dataSubjectTagsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	SubjectTagsForm* form=new SubjectTagsForm();
	form->exec();
}

void FetMainForm::on_dataYearsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	YearsForm* form=new YearsForm();
	form->exec();
}

void FetMainForm::on_dataGroupsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	GroupsForm* form=new GroupsForm();
	form->exec();
}

void FetMainForm::on_dataSubgroupsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	SubgroupsForm* form=new SubgroupsForm();
	form->exec();
}

void FetMainForm::on_dataStudentsStatisticsAction_activated()
{
	StudentsStatisticsForm* form=new StudentsStatisticsForm();
	form->exec();
}

void FetMainForm::on_dataActivitiesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ActivitiesForm* form=new ActivitiesForm();
	form->exec();
}

void FetMainForm::on_dataRoomsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	RoomsForm* form=new RoomsForm();
	form->exec();
}

void FetMainForm::on_dataAllTimeConstraintsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	AllTimeConstraintsForm* form=new AllTimeConstraintsForm();
	form->exec();
}

void FetMainForm::on_dataAllSpaceConstraintsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	AllSpaceConstraintsForm* form=new AllSpaceConstraintsForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraints2ActivitiesConsecutiveAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	Constraint2ActivitiesConsecutiveForm* form=new Constraint2ActivitiesConsecutiveForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivitiesPreferredTimesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivitiesPreferredTimesForm* form=new ConstraintActivitiesPreferredTimesForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivitiesSameStartingTimeAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivitiesSameStartingTimeForm* form=new ConstraintActivitiesSameStartingTimeForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivitiesSameStartingHourAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivitiesSameStartingHourForm* form=new ConstraintActivitiesSameStartingHourForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeacherNotAvailableAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherNotAvailableForm* form=new ConstraintTeacherNotAvailableForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsBasicCompulsoryTimeAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintBasicCompulsoryTimeForm* form=new ConstraintBasicCompulsoryTimeForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsBasicCompulsorySpaceAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintBasicCompulsorySpaceForm* form=new ConstraintBasicCompulsorySpaceForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsRoomNotAvailableAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintRoomNotAvailableForm* form=new ConstraintRoomNotAvailableForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsActivityPreferredRoomAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivityPreferredRoomForm* form=new ConstraintActivityPreferredRoomForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsActivityPreferredRoomsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivityPreferredRoomsForm* form=new ConstraintActivityPreferredRoomsForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsSubjectPreferredRoomAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintSubjectPreferredRoomForm* form=new ConstraintSubjectPreferredRoomForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsSubjectPreferredRoomsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintSubjectPreferredRoomsForm* form=new ConstraintSubjectPreferredRoomsForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsSubjectSubjectTagPreferredRoomAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintSubjectSubjectTagPreferredRoomForm* form=new ConstraintSubjectSubjectTagPreferredRoomForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsSubjectSubjectTagPreferredRoomsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintSubjectSubjectTagPreferredRoomsForm* form=new ConstraintSubjectSubjectTagPreferredRoomsForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsSetNotAvailableAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetNotAvailableForm* form=new ConstraintStudentsSetNotAvailableForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsBreakAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintBreakForm* form=new ConstraintBreakForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeacherMaxDaysPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherMaxDaysPerWeekForm* form=new ConstraintTeacherMaxDaysPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeachersMaxHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMaxHoursDailyForm* form=new ConstraintTeachersMaxHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeacherMaxHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherMaxHoursDailyForm* form=new ConstraintTeacherMaxHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivityPreferredTimeAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivityPreferredTimeForm* form=new ConstraintActivityPreferredTimeForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsSetNoGapsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetNoGapsForm* form=new ConstraintStudentsSetNoGapsForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsNoGapsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsNoGapsForm* form=new ConstraintStudentsNoGapsForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeachersMaxGapsPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMaxGapsPerWeekForm* form=new ConstraintTeachersMaxGapsPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeacherMaxGapsPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherMaxGapsPerWeekForm* form=new ConstraintTeacherMaxGapsPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsEarlyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsEarlyForm* form=new ConstraintStudentsEarlyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsSetEarlyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetEarlyForm* form=new ConstraintStudentsSetEarlyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsSetMaxHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetMaxHoursDailyForm* form=new ConstraintStudentsSetMaxHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsMaxHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsMaxHoursDailyForm* form=new ConstraintStudentsMaxHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsSetMinHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetMinHoursDailyForm* form=new ConstraintStudentsSetMinHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsMinHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsMinHoursDailyForm* form=new ConstraintStudentsMinHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivitiesNotOverlappingAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivitiesNotOverlappingForm* form=new ConstraintActivitiesNotOverlappingForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsMinNDaysBetweenActivitiesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintMinNDaysBetweenActivitiesForm* form=new ConstraintMinNDaysBetweenActivitiesForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivityPreferredTimesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivityPreferredTimesForm* form=new ConstraintActivityPreferredTimesForm();
	form->exec();
}

void FetMainForm::on_helpAboutAction_activated()
{
	HelpAboutForm* form=new HelpAboutForm();
	form->show();
}

void FetMainForm::on_helpFAQAction_activated()
{
	HelpFaqForm* form=new HelpFaqForm();
	form->show();
}

void FetMainForm::on_helpInstructionsAction_activated()
{
	HelpInstructionsForm* form=new HelpInstructionsForm();
	form->show();
}

void FetMainForm::on_timetableGenerateAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	int count=0;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		if(act->active){
			//cout<<"here: i=="<<i<<endl;
			count++;
		}
	}
	if(count<2){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please input at least two active activities before generating"));
		return;
	}
	TimetableGenerateForm *form=new TimetableGenerateForm();
	form->exec();
}

void FetMainForm::on_timetableGenerateMultipleAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	if(INPUT_FILENAME_XML==""){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Current file (data) has no name. Please save file under a certain name before proceeding"));
		return;
	}

	int count=0;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		if(act->active){
			//cout<<"here: i=="<<i<<endl;
			count++;
		}
	}
	if(count<2){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please input at least two active activities before generating multiple"));
		return;
	}
	TimetableGenerateMultipleForm *form=new TimetableGenerateMultipleForm();
	form->exec();
}

void FetMainForm::on_timetableViewStudentsAction_activated()
{
	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please generate, firstly"));
		return;
	}

	TimetableViewStudentsForm *form=new TimetableViewStudentsForm();
	form->show();
}

void FetMainForm::on_timetableViewTeachersAction_activated()
{
	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please generate, firstly"));
		return;
	}

	TimetableViewTeachersForm *form=new TimetableViewTeachersForm();
	form->show();
}

void FetMainForm::on_timetableShowConflictsAction_activated()
{
	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please generate, firstly"));
		return;
	}

	TimetableShowConflictsForm *form=new TimetableShowConflictsForm();
	form->show();
}

void FetMainForm::on_timetableViewRoomsAction_activated()
{
	if(!rooms_schedule_ready){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please generate, firstly"));
		return;
	}

	TimetableViewRoomsForm* form=new TimetableViewRoomsForm();
	form->show();
}

void FetMainForm::on_languageEnglishAction_activated()
{
	QMessageBox::information(this, QObject::tr("FET information"), 
	 QObject::tr("Please exit and restart FET to activate language change"));

	FET_LANGUAGE="en_GB";
	
	languageMenu->setItemChecked(languageMenu->idAt(0), true);
	languageMenu->setItemChecked(languageMenu->idAt(1), false);
	languageMenu->setItemChecked(languageMenu->idAt(2), false);
	languageMenu->setItemChecked(languageMenu->idAt(3), false);
	languageMenu->setItemChecked(languageMenu->idAt(4), false);
	languageMenu->setItemChecked(languageMenu->idAt(5), false);
	languageMenu->setItemChecked(languageMenu->idAt(6), false);
	languageMenu->setItemChecked(languageMenu->idAt(7), false);
	languageMenu->setItemChecked(languageMenu->idAt(8), false);
	languageMenu->setItemChecked(languageMenu->idAt(9), false);
	languageMenu->setItemChecked(languageMenu->idAt(10), false);
}

void FetMainForm::on_languageFrenchAction_activated()
{
	QMessageBox::information(this, QObject::tr("FET information"), 
	 QObject::tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="fr";
	
	languageMenu->setItemChecked(languageMenu->idAt(0), false);
	languageMenu->setItemChecked(languageMenu->idAt(1), true);
	languageMenu->setItemChecked(languageMenu->idAt(2), false);
	languageMenu->setItemChecked(languageMenu->idAt(3), false);
	languageMenu->setItemChecked(languageMenu->idAt(4), false);
	languageMenu->setItemChecked(languageMenu->idAt(5), false);
	languageMenu->setItemChecked(languageMenu->idAt(6), false);
	languageMenu->setItemChecked(languageMenu->idAt(7), false);
	languageMenu->setItemChecked(languageMenu->idAt(8), false);
	languageMenu->setItemChecked(languageMenu->idAt(9), false);
	languageMenu->setItemChecked(languageMenu->idAt(10), false);
}

void FetMainForm::on_languageCatalanAction_activated()
{
	QMessageBox::information(this, QObject::tr("FET information"), 
	 QObject::tr("Please exit and restart FET to activate language change"));

	FET_LANGUAGE="ca";
	
	languageMenu->setItemChecked(languageMenu->idAt(0), false);
	languageMenu->setItemChecked(languageMenu->idAt(1), false);
	languageMenu->setItemChecked(languageMenu->idAt(2), true);
	languageMenu->setItemChecked(languageMenu->idAt(3), false);
	languageMenu->setItemChecked(languageMenu->idAt(4), false);
	languageMenu->setItemChecked(languageMenu->idAt(5), false);
	languageMenu->setItemChecked(languageMenu->idAt(6), false);
	languageMenu->setItemChecked(languageMenu->idAt(7), false);
	languageMenu->setItemChecked(languageMenu->idAt(8), false);
	languageMenu->setItemChecked(languageMenu->idAt(9), false);
	languageMenu->setItemChecked(languageMenu->idAt(10), false);
}

void FetMainForm::on_languageRomanianAction_activated()
{
	QMessageBox::information(this, QObject::tr("FET information"), 
	 QObject::tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="ro";
	
	languageMenu->setItemChecked(languageMenu->idAt(0), false);
	languageMenu->setItemChecked(languageMenu->idAt(1), false);
	languageMenu->setItemChecked(languageMenu->idAt(2), false);
	languageMenu->setItemChecked(languageMenu->idAt(3), true);
	languageMenu->setItemChecked(languageMenu->idAt(4), false);
	languageMenu->setItemChecked(languageMenu->idAt(5), false);
	languageMenu->setItemChecked(languageMenu->idAt(6), false);
	languageMenu->setItemChecked(languageMenu->idAt(7), false);
	languageMenu->setItemChecked(languageMenu->idAt(8), false);
	languageMenu->setItemChecked(languageMenu->idAt(9), false);
	languageMenu->setItemChecked(languageMenu->idAt(10), false);
}

void FetMainForm::on_languageMalayAction_activated()
{
	QMessageBox::information(this, QObject::tr("FET information"), 
	 QObject::tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="ms";
	
	languageMenu->setItemChecked(languageMenu->idAt(0), false);
	languageMenu->setItemChecked(languageMenu->idAt(1), false);
	languageMenu->setItemChecked(languageMenu->idAt(2), false);
	languageMenu->setItemChecked(languageMenu->idAt(3), false);
	languageMenu->setItemChecked(languageMenu->idAt(4), true);
	languageMenu->setItemChecked(languageMenu->idAt(5), false);
	languageMenu->setItemChecked(languageMenu->idAt(6), false);
	languageMenu->setItemChecked(languageMenu->idAt(7), false);
	languageMenu->setItemChecked(languageMenu->idAt(8), false);
	languageMenu->setItemChecked(languageMenu->idAt(9), false);
	languageMenu->setItemChecked(languageMenu->idAt(10), false);
}

void FetMainForm::on_languagePolishAction_activated()
{
	QMessageBox::information(this, QObject::tr("FET information"), 
	 QObject::tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="pl";
	
	languageMenu->setItemChecked(languageMenu->idAt(0), false);
	languageMenu->setItemChecked(languageMenu->idAt(1), false);
	languageMenu->setItemChecked(languageMenu->idAt(2), false);
	languageMenu->setItemChecked(languageMenu->idAt(3), false);
	languageMenu->setItemChecked(languageMenu->idAt(4), false);
	languageMenu->setItemChecked(languageMenu->idAt(5), true);
	languageMenu->setItemChecked(languageMenu->idAt(6), false);
	languageMenu->setItemChecked(languageMenu->idAt(7), false);
	languageMenu->setItemChecked(languageMenu->idAt(8), false);
	languageMenu->setItemChecked(languageMenu->idAt(9), false);
	languageMenu->setItemChecked(languageMenu->idAt(10), false);
}

void FetMainForm::on_languageTurkishAction_activated()
{
	QMessageBox::information(this, QObject::tr("FET information"), 
	 QObject::tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="tr";
	
	languageMenu->setItemChecked(languageMenu->idAt(0), false);
	languageMenu->setItemChecked(languageMenu->idAt(1), false);
	languageMenu->setItemChecked(languageMenu->idAt(2), false);
	languageMenu->setItemChecked(languageMenu->idAt(3), false);
	languageMenu->setItemChecked(languageMenu->idAt(4), false);
	languageMenu->setItemChecked(languageMenu->idAt(5), false);
	languageMenu->setItemChecked(languageMenu->idAt(6), true);
	languageMenu->setItemChecked(languageMenu->idAt(7), false);
	languageMenu->setItemChecked(languageMenu->idAt(8), false);
	languageMenu->setItemChecked(languageMenu->idAt(9), false);
	languageMenu->setItemChecked(languageMenu->idAt(10), false);
}

void FetMainForm::on_languageDutchAction_activated()
{
	QMessageBox::information(this, QObject::tr("FET information"), 
	 QObject::tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="nl";
	
	languageMenu->setItemChecked(languageMenu->idAt(0), false);
	languageMenu->setItemChecked(languageMenu->idAt(1), false);
	languageMenu->setItemChecked(languageMenu->idAt(2), false);
	languageMenu->setItemChecked(languageMenu->idAt(3), false);
	languageMenu->setItemChecked(languageMenu->idAt(4), false);
	languageMenu->setItemChecked(languageMenu->idAt(5), false);
	languageMenu->setItemChecked(languageMenu->idAt(6), false);
	languageMenu->setItemChecked(languageMenu->idAt(7), true);
	languageMenu->setItemChecked(languageMenu->idAt(8), false);
	languageMenu->setItemChecked(languageMenu->idAt(9), false);
	languageMenu->setItemChecked(languageMenu->idAt(10), false);
}

void FetMainForm::on_languageGermanAction_activated()
{
	QMessageBox::information(this, QObject::tr("FET information"), 
	 QObject::tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="de";
	
	languageMenu->setItemChecked(languageMenu->idAt(0), false);
	languageMenu->setItemChecked(languageMenu->idAt(1), false);
	languageMenu->setItemChecked(languageMenu->idAt(2), false);
	languageMenu->setItemChecked(languageMenu->idAt(3), false);
	languageMenu->setItemChecked(languageMenu->idAt(4), false);
	languageMenu->setItemChecked(languageMenu->idAt(5), false);
	languageMenu->setItemChecked(languageMenu->idAt(6), false);
	languageMenu->setItemChecked(languageMenu->idAt(7), false);
	languageMenu->setItemChecked(languageMenu->idAt(8), true);
	languageMenu->setItemChecked(languageMenu->idAt(9), false);
	languageMenu->setItemChecked(languageMenu->idAt(10), false);
}

void FetMainForm::on_languageHungarianAction_activated()
{
	QMessageBox::information(this, QObject::tr("FET information"), 
	 QObject::tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="hu";
	
	languageMenu->setItemChecked(languageMenu->idAt(0), false);
	languageMenu->setItemChecked(languageMenu->idAt(1), false);
	languageMenu->setItemChecked(languageMenu->idAt(2), false);
	languageMenu->setItemChecked(languageMenu->idAt(3), false);
	languageMenu->setItemChecked(languageMenu->idAt(4), false);
	languageMenu->setItemChecked(languageMenu->idAt(5), false);
	languageMenu->setItemChecked(languageMenu->idAt(6), false);
	languageMenu->setItemChecked(languageMenu->idAt(7), false);
	languageMenu->setItemChecked(languageMenu->idAt(8), false);
	languageMenu->setItemChecked(languageMenu->idAt(9), true);
	languageMenu->setItemChecked(languageMenu->idAt(10), false);
}

void FetMainForm::on_languageMacedonianAction_activated()
{
	QMessageBox::information(this, QObject::tr("FET information"), 
	 QObject::tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="mk";
	
	languageMenu->setItemChecked(languageMenu->idAt(0), false);
	languageMenu->setItemChecked(languageMenu->idAt(1), false);
	languageMenu->setItemChecked(languageMenu->idAt(2), false);
	languageMenu->setItemChecked(languageMenu->idAt(3), false);
	languageMenu->setItemChecked(languageMenu->idAt(4), false);
	languageMenu->setItemChecked(languageMenu->idAt(5), false);
	languageMenu->setItemChecked(languageMenu->idAt(6), false);
	languageMenu->setItemChecked(languageMenu->idAt(7), false);
	languageMenu->setItemChecked(languageMenu->idAt(8), false);
	languageMenu->setItemChecked(languageMenu->idAt(9), false);
	languageMenu->setItemChecked(languageMenu->idAt(10), true);
}

