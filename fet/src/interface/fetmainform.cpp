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

#include "genetictimetable_defs.h"
#include "genetictimetable.h"
#include "fetmainform.h"
#include "timetableallocatehoursform.h"
#include "timetableviewstudentsform.h"
#include "timetableviewteachersform.h"
#include "timetableviewstudentswithroomsform.h"
#include "timetableviewteacherswithroomsform.h"
#include "timetableviewstudentswithrooms2form.h"
#include "timetableviewteacherswithrooms2form.h"
#include "timetableshowconflictstimeform.h"
#include "timetableallocateroomsform.h"
#include "timetableallocatehoursroomsform.h"
#include "timetableviewroomsform.h"
#include "timetableshowconflictsspaceform.h"
#include "timetableviewrooms2form.h"
#include "timetableshowconflictstimespaceform.h"
#include "timetableexporticalteachersform.h"
#include "timetableexporticalstudentsform.h"
#include "timetableexporticalteacherswithrooms1form.h"
#include "timetableexporticalstudentswithrooms1form.h"
#include "timetableexporticalteacherswithrooms2form.h"
#include "timetableexporticalstudentswithrooms2form.h"
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
#include "equipmentsform.h"
#include "buildingsform.h"
#include "roomsform.h"
#include "alltimeconstraintsform.h"
#include "allspaceconstraintsform.h"
#include "helpaboutform.h"
#include "helpfaqform.h"
#include "helptimeconstraintsform.h"
#include "helptimeconstraintssubtagsform.h"
#include "populationnumberform.h"
#include "fet.h"
#include "constraint2activitiesconsecutiveform.h"
#include "constraint2activitiesorderedform.h"
#include "constraint2activitiesgroupedform.h"
#include "constraintactivitiespreferredtimesform.h"
#include "constraintactivitiessamestartingtimeform.h"
#include "constraintactivitiessamestartinghourform.h"
#include "constraintteachernotavailableform.h"
#include "constraintbasiccompulsorytimeform.h"
#include "constraintbasiccompulsoryspaceform.h"
#include "constraintteacherrequiresroomform.h"
#include "constraintteachersubjectrequireroomform.h"
#include "constraintroomnotavailableform.h"
#include "constraintactivitypreferredroomform.h"
#include "constraintminimizenumberofroomsforstudentsform.h"
#include "constraintminimizenumberofroomsforteachersform.h"
#include "constraintroomtypenotallowedsubjectsform.h"
#include "constraintsubjectrequiresequipmentsform.h"
#include "constraintsubjectsubjecttagrequireequipmentsform.h"
#include "constraintstudentssetnotavailableform.h"
#include "constraintbreakform.h"
#include "constraintteachermaxdaysperweekform.h"
#include "constraintteachersmaxhourscontinuouslyform.h"
#include "constraintteachersmaxhoursdailyform.h"
#include "constraintteachersminhoursdailyform.h"
#include "constraintteacherssubgroupsmaxhoursdailyform.h"
#include "constraintactivitypreferredtimeform.h"
#include "constraintstudentssetnogapsform.h"
#include "constraintstudentsnogapsform.h"
#include "constraintteachersnogapsform.h"
#include "constraintstudentsearlyform.h"
#include "constraintstudentssetintervalmaxdaysperweekform.h"
#include "constraintteacherintervalmaxdaysperweekform.h"
#include "constraintstudentssetnhoursdailyform.h"
#include "constraintstudentsnhoursdailyform.h"
#include "constraintactivityendsdayform.h"
#include "constraintactivitiesnotoverlappingform.h"
#include "constraintminndaysbetweenactivitiesform.h"
#include "constraintactivitypreferredtimesform.h"
#include "constraintteacherssubjecttagsmaxhourscontinuouslyform.h"
#include "constraintteacherssubjecttagmaxhourscontinuouslyform.h"
#include "constraintactivitiessameroomform.h"
#include "constraintactivitypreferredroomsform.h"
#include "constraintsubjectpreferredroomform.h"
#include "constraintsubjectsubjecttagpreferredroomform.h"
#include "constraintsubjectpreferredroomsform.h"
#include "constraintsubjectsubjecttagpreferredroomsform.h"
#include "constraintmaxbuildingchangesperdayforteachersform.h"
#include "constraintmaxbuildingchangesperdayforstudentsform.h"
#include "constraintmaxroomchangesperdayforteachersform.h"
#include "constraintmaxroomchangesperdayforstudentsform.h"

#include <qmessagebox.h>
#include <q3filedialog.h>
#include <qstring.h>
#include <qdir.h>
#include <q3popupmenu.h>
//Added by qt3to4:
#include <QTranslator>

#include <QCloseEvent>

bool simulation_running; //true if the user started an allocation of the timetable

bool students_schedule_ready;
bool teachers_schedule_ready;
bool rooms_schedule_ready;
bool students_schedule_ready2;
bool teachers_schedule_ready2;
bool rooms_schedule_ready2;

SpaceChromosome best_space_chromosome;
TimeChromosome best_time_chromosome;
TimeSpaceChromosome best_time_space_chromosome;

QString timeConflictsString; //the string that contains a log of the broken time constraints
QString spaceConflictsString; //the string that contains a log of the broken space constraints
QString timeSpaceConflictsString; //the string that contains a log of the broken time&space constraints

extern QApplication* pqapplication;

#include <QDesktopWidget>

FetMainForm::FetMainForm()
{
	setupUi(this);

	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/

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

	bool tmp=gt.rules.addTimeConstraint(new ConstraintBasicCompulsoryTime(1.0));
	assert(tmp);
	tmp=gt.rules.addSpaceConstraint(new ConstraintBasicCompulsorySpace(1.0));
	assert(tmp);

	students_schedule_ready=false;
	teachers_schedule_ready=false;
	rooms_schedule_ready=false;
	students_schedule_ready2=false;
	teachers_schedule_ready2=false;
	rooms_schedule_ready2=false;
	
	languageMenu->setCheckable(true);
}

//#include <iostream>
//using namespace std;

void FetMainForm::closeEvent(QCloseEvent* event)
{
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

	INPUT_FILENAME_XML = "";
}

FetMainForm::~FetMainForm()
{
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

		bool tmp=gt.rules.addTimeConstraint(new ConstraintBasicCompulsoryTime(1.0));
		assert(tmp);
		tmp=gt.rules.addSpaceConstraint(new ConstraintBasicCompulsorySpace(1.0));
		assert(tmp);

		students_schedule_ready=false;
		teachers_schedule_ready=false;
		students_schedule_ready2=false;
		teachers_schedule_ready2=false;
		rooms_schedule_ready2=false;
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
		
		if(gt.rules.read(s)){
			students_schedule_ready=false;
			teachers_schedule_ready=false;
			students_schedule_ready2=false;
			teachers_schedule_ready2=false;
			rooms_schedule_ready2=false;

			INPUT_FILENAME_XML = s;
		}
		else{
			QMessageBox::information(this, QObject::tr("FET info"), QObject::tr("Invalid file"), QObject::tr("&OK"));
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
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	int xx, yy;
	xx=x() + frameGeometry().width()/2 - form->frameGeometry().width()/2;
	yy=y() + frameGeometry().height()/2 - form->frameGeometry().height()/2;
	form->move(xx, yy);*/
	form->exec();
}

void FetMainForm::on_dataEquipmentsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	EquipmentsForm* form=new EquipmentsForm();
	form->exec();
}

void FetMainForm::on_dataBuildingsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	BuildingsForm* form=new BuildingsForm();
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

void FetMainForm::on_dataTimeConstraints2ActivitiesOrderedAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	Constraint2ActivitiesOrderedForm* form=new Constraint2ActivitiesOrderedForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraints2ActivitiesGroupedAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	Constraint2ActivitiesGroupedForm* form=new Constraint2ActivitiesGroupedForm();
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

void FetMainForm::on_dataSpaceConstraintsTeacherRequiresRoomAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherRequiresRoomForm* form=new ConstraintTeacherRequiresRoomForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsTeacherSubjectRequireRoomAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherSubjectRequireRoomForm* form=new ConstraintTeacherSubjectRequireRoomForm();
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

void FetMainForm::on_dataSpaceConstraintsMinimizeNumberOfRoomsForStudentsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintMinimizeNumberOfRoomsForStudentsForm* form=new ConstraintMinimizeNumberOfRoomsForStudentsForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsMinimizeNumberOfRoomsForTeachersAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintMinimizeNumberOfRoomsForTeachersForm* form=new ConstraintMinimizeNumberOfRoomsForTeachersForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsRoomTypeNotAllowedSubjectsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintRoomTypeNotAllowedSubjectsForm* form=new ConstraintRoomTypeNotAllowedSubjectsForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsSubjectRequiresEquipmentsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintSubjectRequiresEquipmentsForm* form=new ConstraintSubjectRequiresEquipmentsForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsSubjectSubjectTagRequireEquipmentsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintSubjectSubjectTagRequireEquipmentsForm* form=new ConstraintSubjectSubjectTagRequireEquipmentsForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsActivitiesSameRoomAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivitiesSameRoomForm* form=new ConstraintActivitiesSameRoomForm();
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

void FetMainForm::on_dataSpaceConstraintsMaxBuildingChangesPerDayForTeachersAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintMaxBuildingChangesPerDayForTeachersForm* form=new ConstraintMaxBuildingChangesPerDayForTeachersForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsMaxBuildingChangesPerDayForStudentsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintMaxBuildingChangesPerDayForStudentsForm* form=new ConstraintMaxBuildingChangesPerDayForStudentsForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsMaxRoomChangesPerDayForTeachersAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintMaxRoomChangesPerDayForTeachersForm* form=new ConstraintMaxRoomChangesPerDayForTeachersForm();
	form->exec();
}

void FetMainForm::on_dataSpaceConstraintsMaxRoomChangesPerDayForStudentsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintMaxRoomChangesPerDayForStudentsForm* form=new ConstraintMaxRoomChangesPerDayForStudentsForm();
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

void FetMainForm::on_dataTimeConstraintsTeachersMaxHoursContinuouslyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMaxHoursContinuouslyForm* form=new ConstraintTeachersMaxHoursContinuouslyForm();
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

void FetMainForm::on_dataTimeConstraintsTeachersMinHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersMinHoursDailyForm* form=new ConstraintTeachersMinHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeachersSubgroupsMaxHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersSubgroupsMaxHoursDailyForm* form=new ConstraintTeachersSubgroupsMaxHoursDailyForm();
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

void FetMainForm::on_dataTimeConstraintsTeachersNoGapsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersNoGapsForm* form=new ConstraintTeachersNoGapsForm();
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

void FetMainForm::on_dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetIntervalMaxDaysPerWeekForm* form=new ConstraintStudentsSetIntervalMaxDaysPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeacherIntervalMaxDaysPerWeekForm* form=new ConstraintTeacherIntervalMaxDaysPerWeekForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsSetNHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsSetNHoursDailyForm* form=new ConstraintStudentsSetNHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsStudentsNHoursDailyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintStudentsNHoursDailyForm* form=new ConstraintStudentsNHoursDailyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsActivityEndsDayAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintActivityEndsDayForm* form=new ConstraintActivityEndsDayForm();
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

void FetMainForm::on_dataTimeConstraintsTeachersSubjectTagsMaxHoursContinuouslyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersSubjectTagsMaxHoursContinuouslyForm* form=new ConstraintTeachersSubjectTagsMaxHoursContinuouslyForm();
	form->exec();
}

void FetMainForm::on_dataTimeConstraintsTeachersSubjectTagMaxHoursContinuouslyAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	ConstraintTeachersSubjectTagMaxHoursContinuouslyForm* form=new ConstraintTeachersSubjectTagMaxHoursContinuouslyForm();
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

void FetMainForm::on_helpTimeConstraintsAction_activated()
{
	HelpTimeConstraintsForm* form=new HelpTimeConstraintsForm();
	form->show();
}

void FetMainForm::on_helpTimeConstraintsSubtagsAction_activated()
{
	HelpTimeConstraintsSubtagsForm* form=new HelpTimeConstraintsSubtagsForm();
	form->show();
}

void FetMainForm::on_timetableAllocateHoursAction_activated()
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
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please input at least two active activities before allocating hours"));
		return;
	}
	TimetableAllocateHoursForm *form=new TimetableAllocateHoursForm();
	form->show();
}

void FetMainForm::on_timetableViewStudentsAction_activated()
{
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the hours, firstly"));
		return;
	}

	TimetableViewStudentsForm *form=new TimetableViewStudentsForm();
	form->show();
}

void FetMainForm::on_timetableViewTeachersAction_activated()
{
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the hours, firstly"));
		return;
	}

	TimetableViewTeachersForm *form=new TimetableViewTeachersForm();
	form->show();
}

void FetMainForm::on_timetableViewStudentsWithRoomsAction_activated()
{
	if(!rooms_schedule_ready){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the rooms, firstly"));
		return;
	}

	TimetableViewStudentsWithRoomsForm *form=new TimetableViewStudentsWithRoomsForm();
	form->show();
}

void FetMainForm::on_timetableViewTeachersWithRoomsAction_activated()
{
	if(!rooms_schedule_ready){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the rooms, firstly"));
		return;
	}

	TimetableViewTeachersWithRoomsForm *form=new TimetableViewTeachersWithRoomsForm();
	form->show();
}

void FetMainForm::on_timetableViewStudentsWithRooms2Action_activated()
{
	if(!rooms_schedule_ready2){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the hours and the rooms, firstly"));
		return;
	}

	TimetableViewStudentsWithRooms2Form *form=new TimetableViewStudentsWithRooms2Form();
	form->show();
}

void FetMainForm::on_timetableViewTeachersWithRooms2Action_activated()
{
	if(!rooms_schedule_ready2){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the hours and the rooms, firstly"));
		return;
	}

	TimetableViewTeachersWithRooms2Form *form=new TimetableViewTeachersWithRooms2Form();
	form->show();
}

void FetMainForm::on_timetableShowConflictsTimeAction_activated()
{
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the hours, firstly"));
		return;
	}

	TimetableShowConflictsTimeForm *form=new TimetableShowConflictsTimeForm();
	form->show();
}

void FetMainForm::on_timetableAllocateRoomsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	if(!gt.rules.internalStructureComputed){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the hours prior to allocating the rooms"));
		return;
	}
	if(! (students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the hours prior to allocating the rooms"));
		return;
	}
	if(gt.rules.roomsList.count()<=0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please input at least one room before allocating the rooms"));
		return;
	}
	TimetableAllocateRoomsForm *form=new TimetableAllocateRoomsForm();
	form->show();
}

void FetMainForm::on_timetableViewRoomsAction_activated()
{
	if(!rooms_schedule_ready){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the rooms, firstly"));
		return;
	}

	TimetableViewRoomsForm* form=new TimetableViewRoomsForm();
	form->show();
}

void FetMainForm::on_timetableViewRooms2Action_activated()
{
	if(!rooms_schedule_ready2){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the hours and the rooms, firstly"));
		return;
	}

	TimetableViewRooms2Form* form=new TimetableViewRooms2Form();
	form->show();
}

void FetMainForm::on_timetableShowConflictsSpaceAction_activated()
{
	if(!rooms_schedule_ready){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the rooms, firstly"));
		return;
	}

	TimetableShowConflictsSpaceForm* form=new TimetableShowConflictsSpaceForm();
	form->show();
}

void FetMainForm::on_timetableShowConflictsTimeSpaceAction_activated()
{
	if(!rooms_schedule_ready2){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the hours and the rooms, firstly"));
		return;
	}

	TimetableShowConflictsTimeSpaceForm* form=new TimetableShowConflictsTimeSpaceForm();
	form->show();
}

void FetMainForm::on_timetableAllocateHoursRoomsAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	int count=0;
	for(int i=0; i<gt.rules.activitiesList.size(); i++){
		Activity* act=gt.rules.activitiesList[i];
		if(act->active)
			count++;
	}
	if(count<2 || gt.rules.roomsList.count()<=0){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please input at least two active activities and a room before allocating hours and rooms"));
		return;
	}
	TimetableAllocateHoursRoomsForm* form=new TimetableAllocateHoursRoomsForm();
	form->show();
}

void FetMainForm::on_timetableExportXmlHtmlAction_activated()
{
	QMessageBox::information(this, QObject::tr("FET information"), 
	  QObject::tr("Results are exported to xml and html\n"
	  "automatically from the allocate menu.\n"
	  "Please enter the directory \"%1\"\n"
	  "and you will find the exported .xml and .html files.\n"
	  "You can translate the .xml files into .tex or .html\n"
	  "using the files \"fetxml2latex.py\" or \"students.xsl\", \n"
	  " \"teachers.xsl\" and \"rooms.xsl\" "
	  ).arg(OUTPUT_DIR));
}

void FetMainForm::on_timetableExportiCalTeachersAction_activated()
{
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the hours, firstly"));
		return;
	}
	
	TimetableExportiCalTeachersForm* form=new TimetableExportiCalTeachersForm();
	form->exec();
}

void FetMainForm::on_timetableExportiCalStudentsAction_activated()
{
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the hours, firstly"));
		return;
	}
	
	TimetableExportiCalStudentsForm* form=new TimetableExportiCalStudentsForm();
	form->exec();
}

void FetMainForm::on_timetableExportiCalTeachersWithRooms1Action_activated()
{
	if(!rooms_schedule_ready){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the rooms, firstly"));
		return;
	}
	
	TimetableExportiCalTeachersWithRooms1Form* form=new TimetableExportiCalTeachersWithRooms1Form();
	form->exec();
}

void FetMainForm::on_timetableExportiCalStudentsWithRooms1Action_activated()
{
	if(!rooms_schedule_ready){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the rooms, firstly"));
		return;
	}

	TimetableExportiCalStudentsWithRooms1Form* form=new TimetableExportiCalStudentsWithRooms1Form();
	form->exec();
}

void FetMainForm::on_timetableExportiCalTeachersWithRooms2Action_activated()
{
	if(!rooms_schedule_ready2){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the hours and the rooms, firstly"));
		return;
	}

	TimetableExportiCalTeachersWithRooms2Form* form=new TimetableExportiCalTeachersWithRooms2Form();
	form->exec();
}

void FetMainForm::on_timetableExportiCalStudentsWithRooms2Action_activated()
{
	if(!rooms_schedule_ready2){
		QMessageBox::information(this, QObject::tr("FET information"), QObject::tr("Please allocate the hours and the rooms, firstly"));
		return;
	}

	TimetableExportiCalStudentsWithRooms2Form* form=new TimetableExportiCalStudentsWithRooms2Form();
	form->exec();
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

void FetMainForm::on_parametersPopulationNumberAction_activated()
{
	if(simulation_running){
		QMessageBox::information(this, QObject::tr("FET information"),
			QObject::tr("Allocation in course.\nPlease stop simulation before this."));
		return;
	}

	PopulationNumberForm* populationNumberForm=new PopulationNumberForm();
	populationNumberForm->exec();
}
