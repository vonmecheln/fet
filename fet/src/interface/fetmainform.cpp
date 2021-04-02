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

#include "export.h"
#include "import.h"

#include "institutionnameform.h"
#include "commentsform.h"
#include "daysform.h"
#include "hoursform.h"
#include "subjectsform.h"
#include "subjectsstatisticsform.h"
#include "activitytagsform.h"
#include "teachersform.h"
#include "teachersstatisticsform.h"
#include "yearsform.h"
#include "groupsform.h"
#include "subgroupsform.h"
#include "studentsstatisticsform.h"
#include "activitiesform.h"
#include "roomsform.h"
#include "buildingsform.h"
#include "alltimeconstraintsform.h"
#include "allspaceconstraintsform.h"
#include "helpaboutform.h"
#include "helpfaqform.h"
#include "helptipsform.h"
#include "helpinstructionsform.h"

#include "fet.h"

#include "constraintactivityendsstudentsdayform.h"
#include "constraintactivitiesendstudentsdayform.h"
#include "constraint2activitiesconsecutiveform.h"
#include "constraint2activitiesorderedform.h"
#include "constraintactivitiespreferredtimeslotsform.h"
#include "constraintactivitiespreferredstartingtimesform.h"

#include "constraintsubactivitiespreferredtimeslotsform.h"
#include "constraintsubactivitiespreferredstartingtimesform.h"

#include "constraintactivitiessamestartingtimeform.h"
#include "constraintactivitiessamestartinghourform.h"
#include "constraintactivitiessamestartingdayform.h"
#include "constraintteachernotavailabletimesform.h"
#include "constraintbasiccompulsorytimeform.h"
#include "constraintbasiccompulsoryspaceform.h"
#include "constraintroomnotavailabletimesform.h"
#include "constraintactivitypreferredroomform.h"
#include "constraintstudentssetnotavailabletimesform.h"
#include "constraintbreaktimesform.h"
#include "constraintteachermaxdaysperweekform.h"

#include "constraintteacherintervalmaxdaysperweekform.h"
#include "constraintteachersintervalmaxdaysperweekform.h"
#include "constraintstudentssetintervalmaxdaysperweekform.h"
#include "constraintstudentsintervalmaxdaysperweekform.h"

#include "constraintteachermaxhoursdailyform.h"
#include "constraintteachersmaxhoursdailyform.h"
#include "constraintteachermaxhourscontinuouslyform.h"
#include "constraintteachersmaxhourscontinuouslyform.h"
#include "constraintteacherminhoursdailyform.h"
#include "constraintteachersminhoursdailyform.h"
#include "constraintactivitypreferredstartingtimeform.h"
#include "constraintstudentssetmaxgapsperweekform.h"
#include "constraintstudentsmaxgapsperweekform.h"
#include "constraintteachersmaxgapsperweekform.h"
#include "constraintteachermaxgapsperweekform.h"
#include "constraintteachersmaxgapsperdayform.h"
#include "constraintteachermaxgapsperdayform.h"
#include "constraintstudentsearlymaxbeginningsatsecondhourform.h"
#include "constraintstudentssetearlymaxbeginningsatsecondhourform.h"
#include "constraintstudentssetmaxhoursdailyform.h"
#include "constraintstudentsmaxhoursdailyform.h"
#include "constraintstudentssetmaxhourscontinuouslyform.h"
#include "constraintstudentsmaxhourscontinuouslyform.h"
#include "constraintstudentssetminhoursdailyform.h"
#include "constraintstudentsminhoursdailyform.h"
#include "constraintactivitiesnotoverlappingform.h"
#include "constraintminndaysbetweenactivitiesform.h"
#include "constraintmingapsbetweenactivitiesform.h"
#include "constraintactivitypreferredtimeslotsform.h"
#include "constraintactivitypreferredstartingtimesform.h"
#include "constraintactivitypreferredroomsform.h"

#include "constraintstudentssethomeroomform.h"
#include "constraintstudentssethomeroomsform.h"
#include "constraintteacherhomeroomform.h"
#include "constraintteacherhomeroomsform.h"

#include "constraintstudentssetmaxbuildingchangesperdayform.h"
#include "constraintstudentsmaxbuildingchangesperdayform.h"
#include "constraintstudentssetmaxbuildingchangesperweekform.h"
#include "constraintstudentsmaxbuildingchangesperweekform.h"
#include "constraintstudentssetmingapsbetweenbuildingchangesform.h"
#include "constraintstudentsmingapsbetweenbuildingchangesform.h"

#include "constraintteachermaxbuildingchangesperdayform.h"
#include "constraintteachersmaxbuildingchangesperdayform.h"
#include "constraintteachermaxbuildingchangesperweekform.h"
#include "constraintteachersmaxbuildingchangesperweekform.h"
#include "constraintteachermingapsbetweenbuildingchangesform.h"
#include "constraintteachersmingapsbetweenbuildingchangesform.h"

#include "constraintsubjectpreferredroomform.h"
#include "constraintsubjectpreferredroomsform.h"
#include "constraintsubjectactivitytagpreferredroomform.h"
#include "constraintsubjectactivitytagpreferredroomsform.h"

#include "settingstimetablehtmllevelform.h"

#include "spreadconfirmationform.h"

#include <qmessagebox.h>
//#include <q3filedialog.h>
#include <QFileDialog>
#include <qstring.h>
#include <qdir.h>
#include <q3popupmenu.h>
//Added by qt3to4:
#include <QTranslator>

#include <QCloseEvent>

#include <QStatusBar>

#include "httpget.h"

#include "spreadminndaysconstraints5daysform.h"

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

Rules rules2;

static int ORIGINAL_WIDTH, ORIGINAL_HEIGHT;

//English has to be counted also
const int NUMBER_OF_LANGUAGES=17;

const int LANGUAGE_EN_GB_POSITION=0;
const int LANGUAGE_AR_POSITION=1;
const int LANGUAGE_CA_POSITION=2;
const int LANGUAGE_DE_POSITION=3;
const int LANGUAGE_EL_POSITION=4;
const int LANGUAGE_ES_POSITION=5;
const int LANGUAGE_FR_POSITION=6;
const int LANGUAGE_HU_POSITION=7;
const int LANGUAGE_ID_POSITION=8;
const int LANGUAGE_IT_POSITION=9;
const int LANGUAGE_LT_POSITION=10;
const int LANGUAGE_MK_POSITION=11;
const int LANGUAGE_MS_POSITION=12;
const int LANGUAGE_NL_POSITION=13;
const int LANGUAGE_PL_POSITION=14;
const int LANGUAGE_RO_POSITION=15;
const int LANGUAGE_TR_POSITION=16;


const int STATUS_BAR_MILLISECONDS=2500;


FetMainForm::FetMainForm()
{
	setupUi(this);
	
	ORIGINAL_WIDTH=width();
	ORIGINAL_HEIGHT=height();

	QSettings settings("FET free software", "FET");
	QRect rect=settings.value("fetmainformgeometry", QRect(0,0,0,0)).toRect();
	
	if(!rect.isValid()){
		//setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
		/*QDesktopWidget* desktop=QApplication::desktop();
		int xx=desktop->width()/2 - frameGeometry().width()/2;
		int yy=desktop->height()/2 - frameGeometry().height()/2;
		move(xx, yy);*/
		centerWidgetOnScreen(this);
	}
	else{
		move(rect.topLeft());
		cout<<"read rect.x()=="<<rect.x()<<", rect.y()=="<<rect.y()<<endl;
		resize(rect.size());
	}

	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	
	if(FET_LANGUAGE=="en_GB")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_EN_GB_POSITION), true);
	else if(FET_LANGUAGE=="fr")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_FR_POSITION), true);
	else if(FET_LANGUAGE=="ca")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_CA_POSITION), true);
	else if(FET_LANGUAGE=="ro")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_RO_POSITION), true);
	else if(FET_LANGUAGE=="ms")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_MS_POSITION), true);
	else if(FET_LANGUAGE=="pl")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_PL_POSITION), true);
	else if(FET_LANGUAGE=="tr")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_TR_POSITION), true);
	else if(FET_LANGUAGE=="nl")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_NL_POSITION), true);
	else if(FET_LANGUAGE=="de")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_DE_POSITION), true);
	else if(FET_LANGUAGE=="hu")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_HU_POSITION), true);
	else if(FET_LANGUAGE=="mk")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_MK_POSITION), true);
	else if(FET_LANGUAGE=="es")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_ES_POSITION), true);
	else if(FET_LANGUAGE=="el")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_EL_POSITION), true);
	else if(FET_LANGUAGE=="ar")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_AR_POSITION), true);
	else if(FET_LANGUAGE=="id")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_ID_POSITION), true);
	else if(FET_LANGUAGE=="it")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_IT_POSITION), true);
	else if(FET_LANGUAGE=="lt")
		languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_LT_POSITION), true);

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
	
	settingsPrintNotAvailableSlotsAction->setCheckable(true);
	settingsPrintNotAvailableSlotsAction->setChecked(PRINT_NOT_AVAILABLE_TIME_SLOTS);
}

void FetMainForm::on_checkForUpdatesAction_toggled()
{
	checkForUpdates=checkForUpdatesAction->isChecked();
}

void FetMainForm::httpDone(bool error)
{
	if(error){
		QMessageBox::warning(this, tr("FET warning"), tr(
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
			QMessageBox::information(this, tr("FET information"),
			 tr("Another version: %1, is available on FET webpage: http://www.lalescu.ro/liviu/fet/\n\n"
			 "You have to manually download and install (open the FET webpage in an internet browser). "
			 "Please read the information on web page regarding the newer version and choose whether to keep your current version or upgrade "
			 "(the recommended option is to upgrade). You might need to hit Refresh in your web browser if links do not work"
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
	
	//if(event!=NULL)
	//	;

	switch(QMessageBox::information( this, tr("FET - exiting"),
	 tr("File might have been changed - do you want to save it?"),
	 tr("&Yes"), tr("&No"), tr("&Cancel"), 0 , 2 )){
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
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	close();
}

void FetMainForm::on_fileNewAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	int confirm=0;
	switch( QMessageBox::information( this, tr("FET application"),
	 tr("Are you sure you want to load new data (rules) ?"),
	 tr("&Yes"), tr("&No"), 0 , 1 ) ) {
	case 0: // Yes
		confirm=1;
		break;
	case 1: // No
		confirm=0;
		break;
	}

	if(confirm){
		INPUT_FILENAME_XML="";
	
		setWindowTitle(tr("FET - a free timetabling program"));

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

		statusBar()->showMessage(tr("New file generated"), STATUS_BAR_MILLISECONDS);
	}
}

void FetMainForm::on_fileOpenAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	int confirm=1;

	if(confirm){
		QString s = QFileDialog::getOpenFileName(this, tr("Choose a file"),
			WORKING_DIRECTORY, 
			tr("FET xml files (*.fet);;Old FET xml files (*.xml);;All files (*)", 
			"Comment for translators (do not translate this comment): This field is for File/Open dialog. Please keep ;; without spaces before, between and after (it is a separator), and consider that the first must be *.fet. In special right to left languages, you might need to leave this field untranslated"));
		if(s.isNull())
			return;

		int tmp2=s.findRev("/");
		QString s2=s.right(s.length()-tmp2-1);
			
		if(s2.indexOf("\"") >= 0){
			QMessageBox::warning(this, tr("FET info"), 
			 tr("Please do not use quotation marks \" in filename, the html css code does not work."
			  " File was not loaded. Please rename it, removing not allowed characters and open it after that with FET."));
			return;
		}		
		if(s2.indexOf(";") >= 0){
			QMessageBox::warning(this, tr("FET info"), 
			 tr("Please do not use semicolon ; in filename, the html css code does not work."
			  " File was not loaded. Please rename it, removing not allowed characters and open it after that with FET."));
			return;
		}
		if(s2.indexOf("#") >= 0){
			QMessageBox::warning(this, tr("FET info"), 
			 tr("Please do not use # in filename, the html css code does not work."
			  " File was not loaded. Please rename it, removing not allowed characters and open it after that with FET."));
			return;
		}
		/*if(s2.indexOf("(") >= 0 || s2.indexOf(")")>=0){
			QMessageBox::information(this, tr("FET info"), tr("Please do not use parentheses () in filename, the html css code does not work"));
			return;
		}*/
		else{
			if(gt.rules.read(s)){
				students_schedule_ready=false;
				teachers_schedule_ready=false;
				rooms_schedule_ready=false;

				INPUT_FILENAME_XML = s;

				statusBar()->showMessage(tr("File opened"), STATUS_BAR_MILLISECONDS);
			}
			else{
				QMessageBox::information(this, tr("FET info"), tr("Invalid file"), tr("&OK"));
			}
		}
		//get the directory
		int tmp=s.findRev("/");
		WORKING_DIRECTORY=s.left(tmp+1);
		
		if(INPUT_FILENAME_XML!="")
			setWindowTitle(tr("FET - %1").arg(INPUT_FILENAME_XML.right(INPUT_FILENAME_XML.length()-INPUT_FILENAME_XML.findRev("/")-1)));
	}
}

void FetMainForm::on_fileSaveAsAction_activated()
{
	QString s = QFileDialog::getSaveFileName(this, tr("Choose a filename to save under"),
		INPUT_FILENAME_XML, tr("FET xml files (*.fet);;All files (*)", "Comment for translators (do not translate this comment): This field is for File/Save as dialog. Please keep ;; without spaces before, between and after (it is a separator), and consider that the first must be *.fet. In special right to left languages, you might need to leave this field untranslated"),
		0, QFileDialog::DontConfirmOverwrite);
	if(s==QString::null)
		return;

	int tmp2=s.findRev("/");
	QString s2=s.right(s.length()-tmp2-1);
			
	if(s2.indexOf("\"") >= 0){
		QMessageBox::warning(this, tr("FET info"), tr("Please do not use quotation marks \" in filename, the html css code does not work"));
		return;
	}
	if(s2.indexOf(";") >= 0){
		QMessageBox::warning(this, tr("FET info"), tr("Please do not use semicolon ; in filename, the html css code does not work"));
		return;
	}
	if(s2.indexOf("#") >= 0){
		QMessageBox::warning(this, tr("FET info"), tr("Please do not use # in filename, the html css code does not work"));
		return;
	}
	/*if(s2.indexOf("(") >= 0 || s2.indexOf(")")>=0){
		QMessageBox::information(this, tr("FET info"), tr("Please do not use parentheses () in filename, the html css code does not work"));
		return;
	}*/
		
	if(s.right(4)!=".fet")
		s+=".fet";

	int tmp=s.findRev("/");
	WORKING_DIRECTORY=s.left(tmp+1);

	if(QFile::exists(s))
		if(QMessageBox::information( this, tr("FET"),
		 tr("File exists - are you sure you want to overwrite existing file?"),
		 tr("&Yes"), tr("&No"), 0 , 1 ) == 1)
		 	return;
			
	INPUT_FILENAME_XML = s;
	
	setWindowTitle(tr("FET - %1").arg(s.right(s.length()-tmp-1)));
	
	gt.rules.write(INPUT_FILENAME_XML);
	
	statusBar()->showMessage(tr("File saved"), STATUS_BAR_MILLISECONDS);
}

// Start of code contributed by Volker Dirr
void FetMainForm::on_fileImportCSVRoomsBuildingsAction_activated(){
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVRoomsAndBuildings();
}

void FetMainForm::on_fileImportCSVSubjectsAction_activated(){
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVSubjects();
}

void FetMainForm::on_fileImportCSVTeachersAction_activated(){
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVTeachers();
}

void FetMainForm::on_fileImportCSVActivitiesAction_activated(){
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVActivities();
}

void FetMainForm::on_fileImportCSVActivityTagsAction_activated(){
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVActivityTags();
}

void FetMainForm::on_fileImportCSVYearsGroupsSubgroupsAction_activated(){
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Import::importCSVStudents();
}

void FetMainForm::on_fileExportCSVAction_activated(){
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	Export::exportCSV();
}
// End of code contributed by Volker Dirr

void FetMainForm::on_timetableSaveTimetableAsAction_activated()
{
	if(!students_schedule_ready || !teachers_schedule_ready || !rooms_schedule_ready){
		QMessageBox::warning(this, tr("FET - Warning"), tr("You have not yet generated a timetable - please generate firstly"));
		return;	
	}

	Solution* tc=&best_solution;
	
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		//Activity* act=&gt.rules.internalActivitiesList[ai];
		int time=tc->times[ai];
		if(time==UNALLOCATED_TIME){
			QMessageBox::warning(this, tr("FET - Warning"), tr("It seems that you have an incomplete timetable."
			 " Saving of timetable does not work for incomplete timetables. Please generate a complete timetable"));
			 //.arg(act->id));
			return;	
		}
		
		int ri=tc->rooms[ai];
		if(ri==UNALLOCATED_SPACE){
			QMessageBox::warning(this, tr("FET - Warning"), tr("It seems that you have an incomplete timetable."
			 " Saving of timetable does not work for incomplete timetables. Please generate a complete timetable"));
			 //.arg(act->id));
			return;	
		}
	}

	QString t=tr("Please read this important information before proceeding:");
	
	t+="\n\n";
	
	t+=tr("This option is only useful if you need to lock current timetable into a file."
		" Locking means that there will be added constraints activity preferred starting time and"
		" activity preferred room with 100% importance for each activity to fix it at current place in current timetable."
		" You can save this timetable as an ordinary .fet file; when you'll open it, you'll see all old inputted data (activities, teachers, etc.)" 
		" and the locking constraints as the last time/space constraints."
		" You can unlock some of these activities (by removing constraints) if small changes appear in the configuration, and generate again"
		" and the remaining locking constraints will be respected.");
		
	t+="\n\n";
	
	t+=tr("This option is useful for institutions where you obtain a timetable, then some small changes appear,"
		" and you need to regenerate timetable, but respecting in a large proportion the old timetable");

	t+="\n\n";
	
	t+=tr("Current data file will not be affected by anything, locking constraints will only be added to the file you select to save"
		" (you can save current datafile and open saved timetable file after that to check it)");
		
	t+="\n\n";
	
	t+=tr("If you need more information, contact author or mailing list");

	QMessageBox::information(this, tr("FET - information about saving a timetable as"), t);
	
	QString s;

	for(;;){
		s = QFileDialog::getSaveFileName(this, tr("Choose a filename to save under" ), 
			INPUT_FILENAME_XML, tr("FET xml files (*.fet);;All files (*)", "Comment for translators (do not translate this comment): This field is for File/Save as dialog. Please keep ;; without spaces before, between and after (it is a separator), and consider that the first must be *.fet. In special right to left languages, you might need to leave this field untranslated"),
			0, QFileDialog::DontConfirmOverwrite);
		if(s==QString::null)
			return;

		int tmp2=s.findRev("/");
		QString s2=s.right(s.length()-tmp2-1);
			
		if(s2.indexOf("\"") >= 0){
			QMessageBox::warning(this, tr("FET info"), tr("Please do not use quotation marks \" in filename, the html css code does not work"));
			return;
		}
		if(s2.indexOf(";") >= 0){
			QMessageBox::warning(this, tr("FET info"), tr("Please do not use semicolon ; in filename, the html css code does not work"));
			return;
		}
		if(s2.indexOf("#") >= 0){
			QMessageBox::warning(this, tr("FET info"), tr("Please do not use # in filename, the html css code does not work"));
			return;
		}
		/*if(s2.indexOf("(") >= 0 || s2.indexOf(")")>=0){
			QMessageBox::information(this, tr("FET info"), tr("Please do not use parentheses () in filename, the html css code does not work"));
			return;
		}*/
			
		if(s.right(4)!=".fet")
			s+=".fet";

		int tmp=s.findRev("/");
		WORKING_DIRECTORY=s.left(tmp+1);

		if(QFile::exists(s)){
			t=tr("File exists");
			t+="\n\n";
			t+=tr("For safety (so you don't lose work), it is not allowed to overwrite an existing file with"
				" locking and saving a current data+timetable");
			t+="\n\n";
			t+=tr("Please choose a non-existing name");
	
			QMessageBox::warning( this, tr("FET warning"), t);
		}
		else
			break;
	}
			
	//INPUT_FILENAME_XML = s; - do NOT add this
	
	//setWindowTitle(tr("FET - %1").arg(s.right(s.length()-tmp-1)));
	
	rules2.initialized=true;
	
	rules2.institutionName=gt.rules.institutionName;
	rules2.comments=gt.rules.comments;
	
	rules2.nHoursPerDay=gt.rules.nHoursPerDay;
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		rules2.hoursOfTheDay[i]=gt.rules.hoursOfTheDay[i];

	rules2.nDaysPerWeek=gt.rules.nDaysPerWeek;
	for(int i=0; i<gt.rules.nDaysPerWeek; i++)
		rules2.daysOfTheWeek[i]=gt.rules.daysOfTheWeek[i];
		
	rules2.yearsList=gt.rules.yearsList;
	
	rules2.teachersList=gt.rules.teachersList;
	
	rules2.subjectsList=gt.rules.subjectsList;
	
	rules2.activityTagsList=gt.rules.activityTagsList;

	rules2.activitiesList=gt.rules.activitiesList;

	rules2.buildingsList=gt.rules.buildingsList;

	rules2.roomsList=gt.rules.roomsList;

	rules2.timeConstraintsList=gt.rules.timeConstraintsList;
	
	rules2.spaceConstraintsList=gt.rules.spaceConstraintsList;


	//add locking constraints
	TimeConstraintsList lockTimeConstraintsList;
	SpaceConstraintsList lockSpaceConstraintsList;



	bool report=true;
	
	int addedTime=0, duplicatesTime=0;
	int addedSpace=0, duplicatesSpace=0;

	//lock selected activities
	for(int ai=0; ai<gt.rules.nInternalActivities; ai++){
		Activity* act=&gt.rules.internalActivitiesList[ai];
		int time=tc->times[ai];
		if(time>=0 && time<gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay){
			int hour=time/gt.rules.nDaysPerWeek;
			int day=time%gt.rules.nDaysPerWeek;

			ConstraintActivityPreferredStartingTime* ctr=new ConstraintActivityPreferredStartingTime(100.0, act->id, day, hour);
			bool t=rules2.addTimeConstraint(ctr);
						
			if(t){
				addedTime++;
				lockTimeConstraintsList.append(ctr);
			}
			else
				duplicatesTime++;

			QString s;
						
			if(t)
				s=tr("Added the following constraint to saved file:")+"\n"+ctr->getDetailedDescription(gt.rules);
			else{
				s=tr("Constraint\n%1 NOT added to saved file - duplicate").arg(ctr->getDetailedDescription(gt.rules));
				delete ctr;
			}
						
			if(report){
				int k;
				if(t)
					k=QMessageBox::information(this, tr("FET information"), s,
				 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
				else
					k=QMessageBox::warning(this, tr("FET warning"), s,
				 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
																			 				 	
		 		if(k==0)
					report=false;
			}
		}
					
		int ri=tc->rooms[ai];
		if(ri!=UNALLOCATED_SPACE && ri!=UNSPECIFIED_ROOM && ri>=0 && ri<gt.rules.nInternalRooms){
			ConstraintActivityPreferredRoom* ctr=new ConstraintActivityPreferredRoom(100, act->id, (gt.rules.internalRoomsList[ri])->name);
			bool t=rules2.addSpaceConstraint(ctr);

			QString s;
						
			if(t){
				addedSpace++;
				lockSpaceConstraintsList.append(ctr);
			}
			else
				duplicatesSpace++;

			if(t)
				s=tr("Added the following constraint to saved file:")+"\n"+ctr->getDetailedDescription(gt.rules);
			else{
				s=tr("Constraint\n%1 NOT added to saved file - duplicate").arg(ctr->getDetailedDescription(gt.rules));
				delete ctr;
			}
						
			if(report){
				int k;
				if(t)
					k=QMessageBox::information(this, tr("FET information"), s,
				 	 tr("Skip information"), tr("See next"), QString(), 1, 0 );
				else
					k=QMessageBox::warning(this, tr("FET warning"), s,
					 tr("Skip information"), tr("See next"), QString(), 1, 0 );
																			 				 	
				if(k==0)
					report=false;
			}
		}
	}

	QMessageBox::information(this, tr("FET information"), tr("Added %1 locking time constraints and %2 locking space constraints to saved file,"
	 " ignored %3 activities which were already fixed in time and %4 activities which were already fixed in space").arg(addedTime).arg(addedSpace).arg(duplicatesTime).arg(duplicatesSpace));
		
	bool result=rules2.write(s);
	
	while(!lockTimeConstraintsList.isEmpty())
		delete lockTimeConstraintsList.takeFirst();
	while(!lockSpaceConstraintsList.isEmpty())
		delete lockSpaceConstraintsList.takeFirst();
	/*foreach(TimeConstraint* tc, lockTimeConstraintsList)
		delete tc;
	foreach(SpaceConstraint* sc, lockSpaceConstraintsList)
		delete sc;*/

	if(result)	
		QMessageBox::information(this, tr("FET information"),
			tr("File saved successfully. You can see it on the hard disk. Current data file remained untouched (of locking constraints),"
			" so you can save it also, or generate different timetables."));

	rules2.nHoursPerDay=0;
	rules2.nDaysPerWeek=0;

	rules2.yearsList.clear();
	
	rules2.teachersList.clear();
	
	rules2.subjectsList.clear();
	
	rules2.activityTagsList.clear();

	rules2.activitiesList.clear();

	rules2.buildingsList.clear();

	rules2.roomsList.clear();

	rules2.timeConstraintsList.clear();
	
	rules2.spaceConstraintsList.clear();
}

void FetMainForm::on_fileSaveAction_activated()
{
	if(INPUT_FILENAME_XML == "")
		on_fileSaveAsAction_activated();
	else{
		gt.rules.write(INPUT_FILENAME_XML);
		statusBar()->showMessage(tr("File saved"), STATUS_BAR_MILLISECONDS);
	}
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
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	DaysForm* form=new DaysForm();
	form->exec();
}

void FetMainForm::on_dataHoursAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	HoursForm* form=new HoursForm();
	form->exec();
}

void FetMainForm::on_dataTeachersAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

void FetMainForm::on_dataActivityTagsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ActivityTagsForm* form=new ActivityTagsForm();
	form->exec();
}

void FetMainForm::on_dataYearsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	YearsForm* form=new YearsForm();
	form->exec();
}

void FetMainForm::on_dataGroupsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	GroupsForm* form=new GroupsForm();
	form->exec();
}

void FetMainForm::on_dataSubgroupsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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

void FetMainForm::on_dataHelpOnStatisticsAction_activated()
{
	QString s;
	
	s+=tr("This help by Liviu Lalescu, modified 7 November 2007");
	
	s+="\n\n";
	
	s+=tr("Statistics for students might be the most difficult to understand."
	 " If you are using divisions of years: probably the most relevant statistics"
	 " are the ones for each subgroup (so you may check only subgroups check box)."
	 " You may see more hours for the years or groups, but these are not significant, please ignore them,"
	 " because each year or group will count also activities of all contained subgroups."
	 "\n\n"
	 "Each subgroup should have a number of hours per week close to the average of"
	 " all subgroups and close to the normal number of working hours of each students set."
	 " If a subgroup has a much lower value, maybe you used incorrectly"
	 " the years/groups/subgroups for activities."
	 "\n\n"
	 "Please read FAQ for detailed description"
	 " on how divisions work. The key is that the subgroups are independent and represent the smallest unit of students."
	 " Each subgroup receives the activities of the parent year and parent group and of itself."
	 "\n\n"
	 "Having a subgroup with too little working hours per week means that you inputted activities in a wrong manner,"
	 " and also that some constraints like no gaps, early or min hours daily for this subgroup"
	 " are interpreted in a wrong manner (if subgroup has only 2 activities, then these must"
	 " be placed in the first hours, which is too hard and wrong)."
	 );
	 
	//QMessageBox::information(this, tr("FET information about statistics"), s);

	//show the message in a dialog
	QDialog* dialog=new QDialog();
	
	dialog->setWindowTitle(tr("FET - information about statistics"));

	QVBoxLayout* vl=new QVBoxLayout(dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout(0);
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, SIGNAL(clicked()), dialog, SLOT(close()));

	dialog->setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QRect rect = QApplication::desktop()->availableGeometry(dialog);
	//QDesktopWidget* desktop=QApplication::desktop();
	int xx=rect.width()/2 - 350;
	int yy=rect.height()/2 - 250;
	dialog->setGeometry(xx, yy, 700, 500);

	dialog->exec();
}

void FetMainForm::on_dataActivitiesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ActivitiesForm* form=new ActivitiesForm();
	form->exec();
}

void FetMainForm::on_dataRoomsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	RoomsForm* form=new RoomsForm();
	form->exec();
}

void FetMainForm::on_dataBuildingsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	BuildingsForm* form=new BuildingsForm();
	form->exec();
}

void FetMainForm::on_dataAllTimeConstraintsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	AllTimeConstraintsForm* form=new AllTimeConstraintsForm();
	form->exec();
}

void FetMainForm::on_dataAllSpaceConstraintsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	AllSpaceConstraintsForm* form=new AllSpaceConstraintsForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraints2ActivitiesConsecutiveAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	Constraint2ActivitiesConsecutiveForm* form=new Constraint2ActivitiesConsecutiveForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraints2ActivitiesOrderedAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	Constraint2ActivitiesOrderedForm* form=new Constraint2ActivitiesOrderedForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivitiesPreferredTimeSlotsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivitiesPreferredTimeSlotsForm* form=new ConstraintActivitiesPreferredTimeSlotsForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivitiesPreferredStartingTimesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivitiesPreferredStartingTimesForm* form=new ConstraintActivitiesPreferredStartingTimesForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintSubactivitiesPreferredTimeSlotsForm* form=new ConstraintSubactivitiesPreferredTimeSlotsForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsSubactivitiesPreferredStartingTimesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintSubactivitiesPreferredStartingTimesForm* form=new ConstraintSubactivitiesPreferredStartingTimesForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivityEndsStudentsDayAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivityEndsStudentsDayForm* form=new ConstraintActivityEndsStudentsDayForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivitiesEndStudentsDayAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivitiesEndStudentsDayForm* form=new ConstraintActivitiesEndStudentsDayForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivitiesSameStartingTimeAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivitiesSameStartingTimeForm* form=new ConstraintActivitiesSameStartingTimeForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivitiesSameStartingHourAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivitiesSameStartingHourForm* form=new ConstraintActivitiesSameStartingHourForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivitiesSameStartingDayAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivitiesSameStartingDayForm* form=new ConstraintActivitiesSameStartingDayForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeacherNotAvailableTimesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherNotAvailableTimesForm* form=new ConstraintTeacherNotAvailableTimesForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsBasicCompulsoryTimeAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintBasicCompulsoryTimeForm* form=new ConstraintBasicCompulsoryTimeForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsBasicCompulsorySpaceAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintBasicCompulsorySpaceForm* form=new ConstraintBasicCompulsorySpaceForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsRoomNotAvailableTimesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintRoomNotAvailableTimesForm* form=new ConstraintRoomNotAvailableTimesForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsActivityPreferredRoomAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivityPreferredRoomForm* form=new ConstraintActivityPreferredRoomForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsActivityPreferredRoomsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivityPreferredRoomsForm* form=new ConstraintActivityPreferredRoomsForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsSubjectPreferredRoomAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintSubjectPreferredRoomForm* form=new ConstraintSubjectPreferredRoomForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsSubjectPreferredRoomsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintSubjectPreferredRoomsForm* form=new ConstraintSubjectPreferredRoomsForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsSubjectActivityTagPreferredRoomAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintSubjectActivityTagPreferredRoomForm* form=new ConstraintSubjectActivityTagPreferredRoomForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintSubjectActivityTagPreferredRoomsForm* form=new ConstraintSubjectActivityTagPreferredRoomsForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsStudentsSetHomeRoomAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetHomeRoomForm* form=new ConstraintStudentsSetHomeRoomForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsStudentsSetHomeRoomsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetHomeRoomsForm* form=new ConstraintStudentsSetHomeRoomsForm();
	form->exec();
}


void FetMainForm::on_dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetMaxBuildingChangesPerDayForm* form=new ConstraintStudentsSetMaxBuildingChangesPerDayForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsMaxBuildingChangesPerDayForm* form=new ConstraintStudentsMaxBuildingChangesPerDayForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetMaxBuildingChangesPerWeekForm* form=new ConstraintStudentsSetMaxBuildingChangesPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsMaxBuildingChangesPerWeekForm* form=new ConstraintStudentsMaxBuildingChangesPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetMinGapsBetweenBuildingChangesForm* form=new ConstraintStudentsSetMinGapsBetweenBuildingChangesForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsMinGapsBetweenBuildingChangesForm* form=new ConstraintStudentsMinGapsBetweenBuildingChangesForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherMaxBuildingChangesPerDayForm* form=new ConstraintTeacherMaxBuildingChangesPerDayForm();
	form->exec();
}
void FetMainForm::on_dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMaxBuildingChangesPerDayForm* form=new ConstraintTeachersMaxBuildingChangesPerDayForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherMaxBuildingChangesPerWeekForm* form=new ConstraintTeacherMaxBuildingChangesPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMaxBuildingChangesPerWeekForm* form=new ConstraintTeachersMaxBuildingChangesPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherMinGapsBetweenBuildingChangesForm* form=new ConstraintTeacherMinGapsBetweenBuildingChangesForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMinGapsBetweenBuildingChangesForm* form=new ConstraintTeachersMinGapsBetweenBuildingChangesForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsTeacherHomeRoomAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherHomeRoomForm* form=new ConstraintTeacherHomeRoomForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsTeacherHomeRoomsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherHomeRoomsForm* form=new ConstraintTeacherHomeRoomsForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsSetNotAvailableTimesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetNotAvailableTimesForm* form=new ConstraintStudentsSetNotAvailableTimesForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsBreakTimesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintBreakTimesForm* form=new ConstraintBreakTimesForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeacherMaxDaysPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherMaxDaysPerWeekForm* form=new ConstraintTeacherMaxDaysPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherIntervalMaxDaysPerWeekForm* form=new ConstraintTeacherIntervalMaxDaysPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersIntervalMaxDaysPerWeekForm* form=new ConstraintTeachersIntervalMaxDaysPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetIntervalMaxDaysPerWeekForm* form=new ConstraintStudentsSetIntervalMaxDaysPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsIntervalMaxDaysPerWeekForm* form=new ConstraintStudentsIntervalMaxDaysPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeachersMaxHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMaxHoursDailyForm* form=new ConstraintTeachersMaxHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeacherMaxHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherMaxHoursDailyForm* form=new ConstraintTeacherMaxHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeachersMaxHoursContinuouslyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMaxHoursContinuouslyForm* form=new ConstraintTeachersMaxHoursContinuouslyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeacherMaxHoursContinuouslyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherMaxHoursContinuouslyForm* form=new ConstraintTeacherMaxHoursContinuouslyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeachersMinHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMinHoursDailyForm* form=new ConstraintTeachersMinHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeacherMinHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherMinHoursDailyForm* form=new ConstraintTeacherMinHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivityPreferredStartingTimeAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivityPreferredStartingTimeForm* form=new ConstraintActivityPreferredStartingTimeForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsSetMaxGapsPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetMaxGapsPerWeekForm* form=new ConstraintStudentsSetMaxGapsPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsMaxGapsPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsMaxGapsPerWeekForm* form=new ConstraintStudentsMaxGapsPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeachersMaxGapsPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMaxGapsPerWeekForm* form=new ConstraintTeachersMaxGapsPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeacherMaxGapsPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherMaxGapsPerWeekForm* form=new ConstraintTeacherMaxGapsPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeachersMaxGapsPerDayAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMaxGapsPerDayForm* form=new ConstraintTeachersMaxGapsPerDayForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeacherMaxGapsPerDayAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherMaxGapsPerDayForm* form=new ConstraintTeacherMaxGapsPerDayForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsEarlyMaxBeginningsAtSecondHourForm* form=new ConstraintStudentsEarlyMaxBeginningsAtSecondHourForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm* form=new ConstraintStudentsSetEarlyMaxBeginningsAtSecondHourForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsSetMaxHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetMaxHoursDailyForm* form=new ConstraintStudentsSetMaxHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsMaxHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsMaxHoursDailyForm* form=new ConstraintStudentsMaxHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetMaxHoursContinuouslyForm* form=new ConstraintStudentsSetMaxHoursContinuouslyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsMaxHoursContinuouslyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsMaxHoursContinuouslyForm* form=new ConstraintStudentsMaxHoursContinuouslyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsSetMinHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetMinHoursDailyForm* form=new ConstraintStudentsSetMinHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsMinHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsMinHoursDailyForm* form=new ConstraintStudentsMinHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivitiesNotOverlappingAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivitiesNotOverlappingForm* form=new ConstraintActivitiesNotOverlappingForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsMinNDaysBetweenActivitiesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintMinNDaysBetweenActivitiesForm* form=new ConstraintMinNDaysBetweenActivitiesForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsMinGapsBetweenActivitiesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintMinGapsBetweenActivitiesForm* form=new ConstraintMinGapsBetweenActivitiesForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivityPreferredTimeSlotsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivityPreferredTimeSlotsForm* form=new ConstraintActivityPreferredTimeSlotsForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivityPreferredStartingTimesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivityPreferredStartingTimesForm* form=new ConstraintActivityPreferredStartingTimesForm();
	form->exec();
}

void FetMainForm::on_helpAboutAction_activated()
{
	HelpAboutForm* form=new HelpAboutForm();
	form->show();
}

void FetMainForm::on_helpForumAction_activated()
{
	QString s=tr("FET has a forum where you can ask questions or talk about FET");
	s+="\n\n";
	s+=tr("The current address is: %1").arg("http://lalescu.ro/liviu/fet/forum/");
	s+="\n";
	s+=tr("Please open this address in a web browser");
	s+="\n\n";
	s+=tr("If it does not work, please search the FET web page, maybe the address was changed");

	//QMessageBox::information(this, tr("FET forum"), s);
	
	QDialog* dialog=new QDialog();
	
	dialog->setWindowTitle(tr("FET forum"));

	QVBoxLayout* vl=new QVBoxLayout(dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(s);
	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout(0);
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, SIGNAL(clicked()), dialog, SLOT(close()));

	dialog->setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QRect rect = QApplication::desktop()->availableGeometry(dialog);
	//QDesktopWidget* desktop=QApplication::desktop();
	int xx=rect.width()/2 - 200;
	int yy=rect.height()/2 - 125;
	dialog->setGeometry(xx, yy, 400, 250);

	dialog->exec();

}

void FetMainForm::on_helpFAQAction_activated()
{
	HelpFaqForm* form=new HelpFaqForm();
	form->show();
}

void FetMainForm::on_helpTipsAction_activated()
{
	HelpTipsForm* form=new HelpTipsForm();
	form->show();
}

void FetMainForm::on_helpInstructionsAction_activated()
{
	HelpInstructionsForm* form=new HelpInstructionsForm();
	form->show();
}

void FetMainForm::on_helpInOtherLanguagesAction_activated()
{
	QString s=tr("You can see help translated into other languages in the directory doc/ of FET");
	s+="\n\n";	
	s+=tr("Currently (17 July 2008), there are:");	
	s+="\n\n";	
	s+=tr("1. ar - Arabic - Manual");
	s+="\n\n";	
	s+=tr("2. es - Spanish - Instructions");
	s+="\n\n";	
	s+=tr("3. it - Italian - Instructions, FAQ");
	s+="\n\n";	
	s+=tr("4. ro - Romanian - Import/Export Help");

	//show the message in a dialog
	QDialog* dialog=new QDialog();
	
	dialog->setWindowTitle(tr("FET - help in other languages"));

	QVBoxLayout* vl=new QVBoxLayout(dialog);
	QTextEdit* te=new QTextEdit();
	te->setPlainText(s);

	te->setReadOnly(true);
	QPushButton* pb=new QPushButton(tr("OK"));

	QHBoxLayout* hl=new QHBoxLayout(0);
	hl->addStretch(1);
	hl->addWidget(pb);

	vl->addWidget(te);
	vl->addLayout(hl);
	connect(pb, SIGNAL(clicked()), dialog, SLOT(close()));

	dialog->setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QRect rect = QApplication::desktop()->availableGeometry(dialog);
	//QDesktopWidget* desktop=QApplication::desktop();
	int xx=rect.width()/2 - 350;
	int yy=rect.height()/2 - 250;
	dialog->setGeometry(xx, yy, 700, 500);

	dialog->exec();
}

void FetMainForm::on_timetableGenerateAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
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
	if(count<1){
		QMessageBox::information(this, tr("FET information"), tr("Please input at least one active activity before generating"));
		return;
	}
	TimetableGenerateForm *form=new TimetableGenerateForm();
	form->exec();
}

void FetMainForm::on_timetableGenerateMultipleAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	if(INPUT_FILENAME_XML==""){
		QMessageBox::information(this, tr("FET information"),
			tr("Current file (data) has no name. Please save file under a certain name before proceeding"));
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
	if(count<1){
		QMessageBox::information(this, tr("FET information"), tr("Please input at least one active activity before generating multiple"));
		return;
	}
	TimetableGenerateMultipleForm *form=new TimetableGenerateMultipleForm();
	form->exec();
}

void FetMainForm::on_timetableViewStudentsAction_activated()
{
	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	TimetableViewStudentsForm *form=new TimetableViewStudentsForm();
	form->show();
}

void FetMainForm::on_timetableViewTeachersAction_activated()
{
	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	TimetableViewTeachersForm *form=new TimetableViewTeachersForm();
	form->show();
}

void FetMainForm::on_timetableShowConflictsAction_activated()
{
	if(!(students_schedule_ready && teachers_schedule_ready && rooms_schedule_ready)){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	TimetableShowConflictsForm *form=new TimetableShowConflictsForm();
	form->show();
}

void FetMainForm::on_timetableViewRoomsAction_activated()
{
	if(!rooms_schedule_ready){
		QMessageBox::information(this, tr("FET information"), tr("Please generate, firstly"));
		return;
	}

	TimetableViewRoomsForm* form=new TimetableViewRoomsForm();
	form->show();
}

void FetMainForm::on_languageEnglishAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));

	FET_LANGUAGE="en_GB";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_EN_GB_POSITION), true);
}

void FetMainForm::on_languageFrenchAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="fr";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_FR_POSITION), true);
}

void FetMainForm::on_languageCatalanAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));

	FET_LANGUAGE="ca";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_CA_POSITION), true);
}

void FetMainForm::on_languageRomanianAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="ro";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_RO_POSITION), true);
}

void FetMainForm::on_languageMalayAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="ms";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_MS_POSITION), true);
}

void FetMainForm::on_languagePolishAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="pl";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_PL_POSITION), true);
}

void FetMainForm::on_languageTurkishAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="tr";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_TR_POSITION), true);
}

void FetMainForm::on_languageDutchAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="nl";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_NL_POSITION), true);
}

void FetMainForm::on_languageGermanAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="de";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_DE_POSITION), true);
}

void FetMainForm::on_languageHungarianAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="hu";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_HU_POSITION), true);
}

void FetMainForm::on_languageMacedonianAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="mk";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_MK_POSITION), true);
}

void FetMainForm::on_languageSpanishAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="es";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_ES_POSITION), true);
}

void FetMainForm::on_languageGreekAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="el";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_EL_POSITION), true);
}

void FetMainForm::on_languageArabicAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="ar";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_AR_POSITION), true);
}

void FetMainForm::on_languageIndonesianAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="id";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_ID_POSITION), true);
}

void FetMainForm::on_languageItalianAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="it";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_IT_POSITION), true);
}

void FetMainForm::on_languageLithuanianAction_activated()
{
	QMessageBox::information(this, tr("FET information"), 
	 tr("Please exit and restart FET to activate language change"));
	
	FET_LANGUAGE="lt";
	
	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_LT_POSITION), true);
}

void FetMainForm::on_settingsRestoreDefaultsAction_activated()
{
	QString s=tr("Are you sure you want to reset all settings to defaults?");
	s+="\n\n";
	
	s+=tr("That means");
	s+="\n";
	s+=tr("1. Mainform geometry will be reset to default");
	s+="\n";
	s+=tr("2. Check for updates at startup will be disabled");
	s+="\n";
	s+=tr("3. Language will be en_GB (restart needed to activate language change)");
	s+="\n";
	s+=tr("4. Working directory will be sample_inputs");
	s+="\n";
	s+=tr("5. Html level of the timetables will be 2");
	s+="\n";
	s+=tr("6. Import directory will be %1").arg(OUTPUT_DIR);
	s+="\n";
	s+=tr("7. Mark not available slots with -x- in timetables will be true");
	s+="\n";
	
	switch( QMessageBox::information( this, tr("FET application"), s,
	 tr("&Yes"), tr("&No"), 0 , 1 ) ) {
	case 0: // Yes
		break;
	case 1: // No
		return;
	}

	resize(ORIGINAL_WIDTH, ORIGINAL_HEIGHT);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);

	for(int i=0; i<NUMBER_OF_LANGUAGES; i++)
		languageMenu->setItemChecked(languageMenu->idAt(i), false);
	languageMenu->setItemChecked(languageMenu->idAt(LANGUAGE_EN_GB_POSITION), true);
	FET_LANGUAGE="en_GB";
	
	checkForUpdatesAction->setChecked(false);
	checkForUpdates=0;
	
	WORKING_DIRECTORY="sample_inputs";
	
	IMPORT_DIRECTORY=OUTPUT_DIR;
	
	TIMETABLE_HTML_LEVEL=2;
	
	settingsPrintNotAvailableSlotsAction->setChecked(true);
	PRINT_NOT_AVAILABLE_TIME_SLOTS=true;
}

void FetMainForm::on_settingsTimetableHtmlLevelAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	SettingsTimetableHtmlLevelForm* form=new SettingsTimetableHtmlLevelForm();
	form->exec();
}

void FetMainForm::on_settingsPrintNotAvailableSlotsAction_toggled()
{
	PRINT_NOT_AVAILABLE_TIME_SLOTS=settingsPrintNotAvailableSlotsAction->isChecked();
}

void FetMainForm::on_spreadActivitiesAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, tr("FET information"),
			tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}
	
	if(gt.rules.nDaysPerWeek>=7){
		QString s;
		s=tr("You have more than 6 days per week, so probably you won't need this feature. Do you still want to continue?");
		
		int cfrm=0;
		switch( QMessageBox::question( this, tr("FET question"),
		 s,
		 tr("&Continue"), tr("&Cancel"), 0 , 1 ) ) {
		case 0: // Yes - continue
			cfrm=1;
			break;
		case 1: // No - cancel
			cfrm=0;
			break;
		}

		if(!cfrm){
			return;
		}
	}
	
	if(gt.rules.nDaysPerWeek<=4){
		QString s;
		s=tr("You have less than 5 days per week, so probably you won't need this feature. Do you still want to continue?");
		
		int cfrm=0;
		switch( QMessageBox::question( this, tr("FET question"),
		 s,
		 tr("&Continue"), tr("&Cancel"), 0 , 1 ) ) {
		case 0: // Yes - continue
			cfrm=1;
			break;
		case 1: // No - cancel
			cfrm=0;
			break;
		}

		if(!cfrm){
			return;
		}
	}
	
/*	QString s;
	//s+=tr("Please note that this a new, not thoroughly tested feature - might have bugs (I hope not :-) . Please report any bugs.");
	//s+="\n\n";
	s+=tr("Please read VERY carefully the description below:");
	s+="\n\n";
	s+=tr("This function is usable for a 5 days week, but it should work well also for a 6 days week. If your number of days "
	 "per week is larger or lower, you might find this function useless. This function is useful for schools or high-schools with usual requirements");
	s+="\n\n";
	s+=tr("This function is intended to be used ONLY after you introduced all activities and obtained a timetable, "
	 "if you want now to spread the splitted activities more evenly in a week");
	s+="\n\n";
	s+=tr("This improvement - optimization of spreading activities in a week - is a process "
	 "of replacing current constraints of type min n days between activities with others, which "
	 "should provide you with a better timetable");
	s+=". ";
	s+=tr("Note: only min n days constraints referring to components from the same split activity will be replaced. If you have constraints min n days "
	 "referring to activities in different components, these will be left untouched - this is a useful feature");
	s+=".\n\n";
	s+=tr("This function is useful you want a rapid improvement of your timetable, without needing to manually modify the constraints "
	 "of type min n days between activities");
	s+="\n\n";
	s+=tr("Please SAVE/BACKUP your current file and keep it safe, in case anything goes wrong, and only continue if you did that already. "
	 "Current function is new and not many users tested it");
	s+="\n\n";
	s+=tr("Important: if you obtain a timetable too difficult for FET, you might need to revert to your former data or lower weights of constraints");

	int confirm=0;
	switch( QMessageBox::question( this, tr("FET question"),
	 s,
	 tr("&Continue"), tr("&Cancel"), 0 , 1 ) ) {
	case 0: // Yes - continue
		confirm=1;
		break;
	case 1: // No - cancel
		confirm=0;
		break;
	}*/
	
	int confirm;
	
	SpreadConfirmationForm* form=new SpreadConfirmationForm();
	confirm=form->exec();

	if(confirm==QDialog::Accepted){
		SpreadMinNDaysConstraints5DaysForm* form=new SpreadMinNDaysConstraints5DaysForm();
		form->exec();
	}
}

