/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************
                      activityplanning.cpp  -  description
                             -------------------
    begin                : Dec 2009
    copyright            : (C) by Volker Dirr
                         : http://www.timetabling.de/
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


// Code contributed by Volker Dirr ( http://www.timetabling.de/ )
// Many thanks to Liviu Lalescu. He told me some speed optimizations.

/*#include <iostream>
using namespace std;*/

#include "activityplanningform.h"

#include "timetable.h"
#include "timetable_defs.h"

// BE CAREFUL: DON'T USE INTERNAL VARIABLES HERE, because maybe computeInternalStructure() is not done!

#include <QtGui>

#include <QToolButton>

#include <QTableWidget>
#include <QHeaderView>

#include <QKeySequence>

#include "activitiesform.h"
#include "addactivityform.h"
#include "subactivitiesform.h"
#include "modifyactivityform.h"
#include "modifysubactivityform.h"
#include "longtextmessagebox.h"

#include "sparseitemmodel.h"

#include <QString>
#include <QStringList>
#include <QHash>
#include <QMultiHash>
#include <QMap>
#include <QSet>

#include <QRadioButton>
#include <QCheckBox>

#include <QList>

//maybe put following in timetable_defs.h? (start)
const int IS_YEAR = 1;
const int IS_GROUP = 2;
const int IS_SUBGROUP = 3;

const int ACTIVE_ONLY = 0;
const int INACTIVE_ONLY = 1;
const int ACTIVE_OR_INACTIVE = 2;
//maybe put following in timetable_defs.h? (end)

/*#include <QApplication>
#include <QProgressDialog>
extern QApplication* pqapplication;*/

extern Timetable gt;

//extern bool simulation_running;	//needed?

extern QStringList allStudentsNames;				//NOT QSet <QString>!!! Because that do an incorrect order of the lists!

QList<bool> studentsDuplicates;
QList<int> yearORgroupORsubgroup;

extern QStringList allTeachersNames;				//NOT QSet <QString>!!! Because that do an incorrect order of the lists!
extern QStringList allSubjectsNames;				//NOT QSet <QString>!!! Because that do an incorrect order of the lists!

extern QHash <QString, int> teachersTotalNumberOfHours;
extern QHash <QString, int> teachersTotalNumberOfHours2;
extern QHash <QString, int> studentsTotalNumberOfHours;
extern QHash <QString, int> studentsTotalNumberOfHours2;
extern QHash <QString, int> subjectsTotalNumberOfHours;
extern QHash <QString, int> subjectsTotalNumberOfHours4;

//TODO: activate teachersTargetNumberOfHours again! it is usefull! seach teachersTargetNumberOfHours. Just 2 lines to delete!
//TODO: need to setDefaultValue for the QHashs ? (sum/number of hours) (also in statisticsexport?) look like it is unneeded.
//TODO: check with toggled
//TODO: retry mouseTracking (still in source. search "mouseTracking"). check: move mouse one last visbile line. is header always highlighted, even under linux?!
//TODO: update if a new teacher, subject or year/group/subgroup is added - or better: just disalow that?!
//      write a "updateBasic"-function with source from constructor if you want to update/care about teacher, subject and students
//TODO: add a new row in teachers table with "comments" - of course saving data is needed to be useful
//MAYBE TODO: display "related" activities. so display all activities of a subgroup also in its related year and group. Problem: Memory and speed!
//            ! i don't like this feature, because using this activitiestable will be much more difficult, because it mean each cell contains (normaly) more then 1 activity!
//            ! So all shortcuts (especialy delete, add and modify) will be useless!
//            ! The table will be very full, so you will loose clear view!

QList<int> teachersTargetNumberOfHours;		//TODO: do this global, this must/should be saved in fet file! Do it in teachers.h ?!

PlanningCommunicationSpinBox planningCommunicationSpinBox;


StartActivityPlanning::StartActivityPlanning()
{
}

StartActivityPlanning::~StartActivityPlanning()
{
}

void StartActivityPlanning::startActivityPlanning(){
	assert(gt.rules.initialized);
	teachersTargetNumberOfHours.clear();
	
	allStudentsNames.clear();
	studentsDuplicates.clear();
	yearORgroupORsubgroup.clear();
	foreach(StudentsYear* sty, gt.rules.yearsList){
		if(allStudentsNames.contains(sty->name)){
			studentsDuplicates<<true;
		} else {
			studentsDuplicates<<false;
		}
		allStudentsNames<<sty->name;
		yearORgroupORsubgroup<<IS_YEAR;
		foreach(StudentsGroup* stg, sty->groupsList){
			if(allStudentsNames.contains(stg->name)){
				studentsDuplicates<<true;
			} else {
				studentsDuplicates<<false;
			}
			allStudentsNames<<stg->name;
			yearORgroupORsubgroup<<IS_GROUP;
			foreach(StudentsSubgroup* sts, stg->subgroupsList){
				if(allStudentsNames.contains(sts->name)){
					studentsDuplicates<<true;
				} else {
					studentsDuplicates<<false;
				}
				allStudentsNames<<sts->name;
				yearORgroupORsubgroup<<IS_SUBGROUP;
			}
		}
	}
	
	allTeachersNames.clear();				// just needed, because i don't need to care about correct iTeacherList if i do it this way.
	foreach(Teacher* t, gt.rules.teachersList){		//  So i don't need gt.rules.computeInternalStructure();
		allTeachersNames<<t->name;
		teachersTargetNumberOfHours<<0;
	}

	allSubjectsNames.clear();				// just done, because i always want to do it the same way + it is faster
	foreach(Subject* s, gt.rules.subjectsList){		// Also don't display empty subjects is easier
		allSubjectsNames<<s->name;
	}
	
	ActivityPlanningForm apfd;
	apfd.exec();
	
	allStudentsNames.clear();
	allTeachersNames.clear();
	allSubjectsNames.clear();
	
	studentsDuplicates.clear();
	yearORgroupORsubgroup.clear();

	teachersTotalNumberOfHours.clear();
	teachersTotalNumberOfHours2.clear();
	studentsTotalNumberOfHours.clear();
	studentsTotalNumberOfHours2.clear();
	subjectsTotalNumberOfHours.clear();
	subjectsTotalNumberOfHours4.clear();
}


// this is very similar to statisticsexport.cpp. so please also check there if you change something here!
ActivityPlanningForm::ActivityPlanningForm(QWidget *parent): QDialog(parent){
	this->setWindowTitle(tr("Activity Planning Dialog"));
	
	QHBoxLayout* wholeDialog=new QHBoxLayout(this);
	
	QSplitter* leftSplitter=new QSplitter(Qt::Vertical);
	//leftSplitter->setMinimumSize(650, 600);
	
	activitiesTableView= new SparseTableView;

	teachersTableView= new SparseTableView;
	
	leftSplitter->addWidget(activitiesTableView);
	leftSplitter->addWidget(teachersTableView);
	
	//begin added by Liviu
	QList<int> list;
	list<<10000<<4100;
	leftSplitter->setSizes(list);
	//end added by Liviu
	
	leftSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	
	QVBoxLayout* rightDialog=new QVBoxLayout();
	
	/*QGroupBox**/ actionsBox=new QGroupBox(tr("Action", "It means the kind of action the user selects"));
	QVBoxLayout* actionsBoxVertical=new QVBoxLayout();
	RBActivity= new QRadioButton(tr("Activities", "Please keep translation short"));
	RBSubactivity= new QRadioButton(tr("Subactivities", "Please keep translation short"));
	RBAdd= new QRadioButton(tr("Add activity", "Please keep translation short"));
	RBModify= new QRadioButton(tr("Modify activity", "Please keep translation short"));
	RBDelete= new QRadioButton(tr("Delete activities", "Please keep translation short"));

	actionsBoxVertical->addWidget(RBActivity);
	actionsBoxVertical->addWidget(RBSubactivity);
	actionsBoxVertical->addWidget(RBAdd);
	actionsBoxVertical->addWidget(RBModify);
	actionsBoxVertical->addWidget(RBDelete);
	RBActivity->setChecked(true);
	actionsBox->setLayout(actionsBoxVertical);
	
	/*QGroupBox**/ optionsBox=new QGroupBox(tr("Options"));
	QVBoxLayout* optionsBoxVertical=new QVBoxLayout();
	QStringList activeStrings;
	activeStrings<<tr("Active", "It refers to active activities. Please keep translation short")
		<<tr("Inactive", "It refers to inactive activities. Please keep translation short")
		<<tr("All", "It refers to all activities. Please keep translation short");
	CBActive=new QComboBox();
	CBActive->addItems(activeStrings);
	CBActive->setCurrentIndex(ACTIVE_ONLY);
	showYears=new QCheckBox(tr("Show years", "Please keep translation short"));
	showYears->setChecked(true);
	showGroups=new QCheckBox(tr("Show groups", "Please keep translation short"));
	showGroups->setChecked(true);
	showSubgroups=new QCheckBox(tr("Show subgroups", "Please keep translation short"));
	showSubgroups->setChecked(false);
	showTeachers=new QCheckBox(tr("Show teachers", "Please keep translation short"));
	showTeachers->setChecked(true);
	//showActivityTags=new QCheckBox(tr("Show activity tags", "Please keep translation short"));
	showActivityTags=new QCheckBox(tr("Show tags", "Please keep translation short. It refers to activity tags, but I want to keep it shorter, because there is not enough space."));
	showActivityTags->setChecked(false);
	showDuplicates=new QCheckBox(tr("Show duplicates", "Please keep translation short"));
	showDuplicates->setChecked(false);
	hideEmptyLines=new QCheckBox(tr("Hide empty lines", "Please keep translation short"));
	hideEmptyLines->setChecked(false);
	hideUsedTeachers=new QCheckBox(tr("Hide full teachers", "This field is for the moment not shown in FET, but we translate it for maybe future versions. "
		"It refers to teachers who have their number of hours fulfilled. Please keep translation short"));
	hideUsedTeachers->setChecked(false); //important to not hide teachers without activities, if target number of hours is deactivated
	swapAxis=new QCheckBox(tr("Swap axes", "Please keep translation short"));
	swapAxis->setChecked(false);
	pbPseudoActivities=new QPushButton(tr("Pseudo activities", "Please keep translation short. It means activities without teachers and/or students sets"));
	//set auto default all buttons, so that the user can press Enter on a cell and have the action for the cell, not the auto default button
	pbPseudoActivities->setAutoDefault(false);
	pbDeleteAll=new QPushButton(tr("Delete all", "Refers to activities. Please keep translation short"));
	pbDeleteAll->setAutoDefault(false);
	//pbHelp=new QPushButton(tr("Help", "Please keep translation short"));
	//pbHelp->setAutoDefault(false);
	
	showHideButton=new QToolButton;
	showHideButton->setText(tr("Hide buttons", "Please keep translation short. This is an option to hide some buttons in the planning activity dialog, so that the tables are more visible."));
	showHideButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
	origShowHideSizePolicy=showHideButton->sizePolicy();
	/*cout<<"hpol=="<<showHidePushButton->sizePolicy().horizontalPolicy()<<" ";
	cout<<"vpol=="<<showHidePushButton->sizePolicy().verticalPolicy()<<endl;*/
	showHideButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	
	pbClose=new QPushButton(tr("Close", "Please keep translation short"));
	pbClose->setAutoDefault(false);
	
	optionsBoxVertical->addWidget(CBActive);
	optionsBoxVertical->addWidget(showYears);
	optionsBoxVertical->addWidget(showGroups);
	optionsBoxVertical->addWidget(showSubgroups);
	optionsBoxVertical->addWidget(showTeachers);
	optionsBoxVertical->addWidget(showActivityTags);
	optionsBoxVertical->addWidget(showDuplicates);
	optionsBoxVertical->addWidget(hideEmptyLines);
	optionsBoxVertical->addWidget(hideUsedTeachers);
	optionsBoxVertical->addWidget(swapAxis);
	//optionsBoxVertical->addWidget(pbPseudoActivities);
	optionsBox->setLayout(optionsBoxVertical);
	
	rightDialog->addWidget(actionsBox);
	rightDialog->addWidget(optionsBox);
	rightDialog->addWidget(pbPseudoActivities);
	rightDialog->addStretch();
	rightDialog->addWidget(pbDeleteAll);
	rightDialog->addStretch();
	//rightDialog->addWidget(pbHelp);
	rightDialog->addWidget(showHideButton);
	rightDialog->addStretch();
	rightDialog->addWidget(pbClose);
	
	wholeDialog->addWidget(leftSplitter);
	wholeDialog->addLayout(rightDialog);
	
	teachersTableView->setSelectionMode(QAbstractItemView::SingleSelection);

	activitiesTableView->setSelectionMode(QAbstractItemView::SingleSelection);
	
	//maybe disable AlternatingColors as soon as mouseTracking work correct?!
	activitiesTableView->setAlternatingRowColors(true);

	teachersTableView->setAlternatingRowColors(true); //by Liviu
	//mouseTracking (start 1/3)
	/*
	activitiesTable->setMouseTracking(true);
	teachersTable->setMouseTracking(true);
	*/
	//mouseTracking (end 1/3)
	
	updateTables(0);
	
	//connect(activitiesTableView, SIGNAL(cellClicked(int, int)), this, SLOT(activitiesCellSelected(int, int)));
	connect(activitiesTableView, SIGNAL(activated(const QModelIndex&)), this, SLOT(activitiesCellSelected(const QModelIndex&)));
	
	//connect(activitiesTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(ActivtiesCellSelected(int, int)));

	//connect(teachersTable, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(teachersCellSelected(QTableWidgetItem*)));
	connect(teachersTableView, SIGNAL(activated(const QModelIndex&)), this, SLOT(teachersCellSelected(const QModelIndex&)));

	//mouseTracking (start 2/3)
	/*
	connect(activitiesTable, SIGNAL(cellEntered(int, int)), this, SLOT(ActivtiesCellEntered(int, int)));
	connect(teachersTable, SIGNAL(cellEntered(int, int)), this, SLOT(TeachersCellEntered(int, int)));
	*/
	//mouseTracking (end 2/3)
	
	connect(activitiesTableView->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(activitiesTableHorizontalHeaderClicked(int)));
	connect(activitiesTableView->verticalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(activitiesTableVerticalHeaderClicked(int)));

	connect(teachersTableView->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(teachersTableHorizontalHeaderClicked(int)));
	
	connect(CBActive, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTables(int)));
	connect(showDuplicates, SIGNAL(stateChanged(int)), this, SLOT(updateTablesVisual(int)));
	connect(showYears, SIGNAL(stateChanged(int)), this, SLOT(updateTablesVisual(int)));
	connect(showGroups, SIGNAL(stateChanged(int)), this, SLOT(updateTablesVisual(int)));
	connect(showSubgroups, SIGNAL(stateChanged(int)), this, SLOT(updateTablesVisual(int)));
	connect(showTeachers, SIGNAL(stateChanged(int)), this, SLOT(updateTables(int)));
	connect(showActivityTags, SIGNAL(stateChanged(int)), this, SLOT(updateTables(int)));
	connect(hideEmptyLines, SIGNAL(stateChanged(int)), this, SLOT(updateTablesVisual(int)));
	connect(hideUsedTeachers, SIGNAL(stateChanged(int)), this, SLOT(updateTablesVisual(int)));
	connect(swapAxis, SIGNAL(stateChanged(int)), this, SLOT(updateTables(int)));
	connect(pbDeleteAll, SIGNAL(clicked()), this, SLOT(deleteAll()));
	connect(pbPseudoActivities, SIGNAL(clicked()), this, SLOT(pseudoActivities()));
	//connect(pbHelp, SIGNAL(clicked()), this, SLOT(help()));
	connect(showHideButton, SIGNAL(clicked()), this, SLOT(showHide()));
	connect(pbClose, SIGNAL(clicked()), this, SLOT(close()));
	
	connect(&planningCommunicationSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateTables(int)));

	int hh=560;
	if(hh<this->minimumSizeHint().height())
		hh=this->minimumSizeHint().height();
	this->resize(840, hh);

	centerWidgetOnScreen(this);
}

/*void ActivityPlanningForm::resizeEvent(QResizeEvent* event)
{
	QDialog::resizeEvent(event);
	
	activitiesTableView->itWasResized();
	teachersTableView->itWasResized();
}*/

void ActivityPlanningForm::showHide()
{
	/*cout<<"hpol=="<<showHidePushButton->sizePolicy().horizontalPolicy()<<" ";
	cout<<"vpol=="<<showHidePushButton->sizePolicy().verticalPolicy()<<endl;*/

	if(pbClose->isVisible()){
		/*QString s;
		s+=tr("Do you wish to hide the buttons on the right? This will give you more space for the tables.");
		s+="\n\n";
		s+=tr("You can restore them with the F11 key (and hide again with the same shortcut)");
	
		int button=QMessageBox::question(NULL, tr("Hide buttons?"), s, QMessageBox::Yes | QMessageBox::No);
		
		if(button==QMessageBox::Yes){*/

		//}

		pbPseudoActivities->hide();
		pbDeleteAll->hide();
		showHideButton->setText(tr("B", "Abbreviation for 'Buttons'. Please keep translation very short (probably only one letter)."
			" There is an option to hide the buttons in the planning activity dialog, so that the tables can have larger visible parts."
			" This button (B) will remain there after the user hides the other buttons. The user then can press this button to show again all the buttons."
			" We need this 'B' button to be very small, so keep translation abbreviated to the initial only"));
		pbClose->hide();
		
		optionsBox->hide();
		actionsBox->hide();

		showHideButton->setSizePolicy(origShowHideSizePolicy);
	}
	else{
		pbPseudoActivities->show();
		pbDeleteAll->show();
		showHideButton->setText(tr("Hide buttons", "Please keep translation short. This is an option to hide some buttons in the planning activity dialog, so that the tables are more visible."));
		pbClose->show();

		optionsBox->show();
		actionsBox->show();

		showHideButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	}
}

void ActivityPlanningForm::computeActivitiesForDeletion(const QString& teacherName, const QString& studentsSetName, const QString& subjectName,
	const QList<int>& tmpID, const QList<int>& tmpGroupID,
	int& nTotalActsDeleted,
	QList<int>& _idsToBeRemoved, QList<int>& _agidsToBeRemoved,
	QList<bool>& _affectOtherTeachersToBeRemoved, bool& _affectOtherTeachersOverall,
	QList<bool>& _affectOtherStudentsSetsToBeRemoved, bool& _affectOtherStudentsSetsOverall,
	QList<bool>& _affectOtherSubjectsToBeRemoved, bool& _affectOtherSubjectsOverall)
{
	QHash<int, int> representantForId;
	QHash<int, int> numberOfSubactivitiesForRepresentant;
	QSet<int> affectStatusForRepresentantTeacher; //if in set, it affects
	QSet<int> affectStatusForRepresentantStudent; //if in set, it affects
	QSet<int> affectStatusForRepresentantSubject; //if in set, it affects
	foreach(Activity* act, gt.rules.activitiesList){
		int id=act->id;
		
		int agid=act->activityGroupId;
		if(agid==0)
			agid=id;
		
		assert(!representantForId.contains(id));
		representantForId.insert(id, agid);
		
		int c=numberOfSubactivitiesForRepresentant.value(agid, 0);
		c++;
		numberOfSubactivitiesForRepresentant.insert(agid, c);
		
		if(teacherName!=""){
			bool af=true;
			if(act->teachersNames.count()==1)
				if(act->teachersNames.at(0)==teacherName)
					af=false;
					
			if( af && !affectStatusForRepresentantTeacher.contains(agid) )
				affectStatusForRepresentantTeacher.insert(agid);
		}
		if(studentsSetName!=""){
			bool af=true;
			if(act->studentsNames.count()==1)
				if(act->studentsNames.at(0)==studentsSetName)
					af=false;
					
			if( af && !affectStatusForRepresentantStudent.contains(agid) )
				affectStatusForRepresentantStudent.insert(agid);
		}
		if(subjectName!=""){
			bool af=true;
			if(act->subjectName==subjectName)
				af=false;
				
			if( af && !affectStatusForRepresentantSubject.contains(agid) )
				affectStatusForRepresentantSubject.insert(agid);
		}
	}
	
	QSet<int> representantCounted;

	nTotalActsDeleted=0;
	
	_idsToBeRemoved.clear();
	_agidsToBeRemoved.clear();
	
	_affectOtherTeachersToBeRemoved.clear();
	_affectOtherTeachersOverall=false;
	
	_affectOtherStudentsSetsToBeRemoved.clear();
	_affectOtherStudentsSetsOverall=false;
	
	_affectOtherSubjectsToBeRemoved.clear();
	_affectOtherSubjectsOverall=false;

	assert(tmpID.count()==tmpGroupID.count());
	
	for(int i=0; i<tmpID.count(); i++){
		int id=tmpID.at(i);
		assert(representantForId.contains(id));
		int repr=representantForId.value(id);
		
		if(!representantCounted.contains(repr)){
			representantCounted.insert(repr);
			
			assert(numberOfSubactivitiesForRepresentant.contains(repr));
			int n=numberOfSubactivitiesForRepresentant.value(repr);
			assert(n>0);
			nTotalActsDeleted+=n;
			
			_idsToBeRemoved.append(id);
			_agidsToBeRemoved.append(tmpGroupID.at(i));
			
			if(affectStatusForRepresentantTeacher.contains(repr)){
				_affectOtherTeachersToBeRemoved.append(true);
				_affectOtherTeachersOverall=true;
			}
			else{
				_affectOtherTeachersToBeRemoved.append(false);
			}
			
			if(affectStatusForRepresentantStudent.contains(repr)){
				_affectOtherStudentsSetsToBeRemoved.append(true);
				_affectOtherStudentsSetsOverall=true;
			}
			else{
				_affectOtherStudentsSetsToBeRemoved.append(false);
			}
			
			if(affectStatusForRepresentantSubject.contains(repr)){
				_affectOtherSubjectsToBeRemoved.append(true);
				_affectOtherSubjectsOverall=true;
			}
			else{
				_affectOtherSubjectsToBeRemoved.append(false);
			}
		}
	}

	assert(_idsToBeRemoved.count()==_agidsToBeRemoved.count());
	assert(_idsToBeRemoved.count()==_affectOtherTeachersToBeRemoved.count());
	assert(_idsToBeRemoved.count()==_affectOtherStudentsSetsToBeRemoved.count());
	assert(_idsToBeRemoved.count()==_affectOtherSubjectsToBeRemoved.count());
}

void ActivityPlanningForm::activitiesTableHorizontalHeaderClicked(int column){
	if(RBActivity->isChecked()){
		if(swapAxis->checkState()==Qt::Checked){
			ActivitiesForm form("", allStudentsNames[column], "", "");
			form.exec();
		} else {
			ActivitiesForm form("", "", allSubjectsNames[column], "");
			form.exec();
		}
	} else if(RBSubactivity->isChecked()) {
		if(swapAxis->checkState()==Qt::Checked){
			SubactivitiesForm form("", allStudentsNames[column], "", "");
			form.exec();
		} else {
			SubactivitiesForm form("", "", allSubjectsNames[column], "");
			form.exec();
		}
	} else if(RBAdd->isChecked()) {
		if(swapAxis->checkState()==Qt::Checked){
			AddActivityForm addActivityForm("", allStudentsNames[column], "", "");
			addActivityForm.exec();
		} else {
			AddActivityForm addActivityForm("", "", allSubjectsNames[column], "");
			addActivityForm.exec();	
		}
	} else if(RBModify->isChecked()) {
		//Normaly there are to many activities. So just entering the activity form
		if(swapAxis->checkState()==Qt::Checked){
			ActivitiesForm form("", allStudentsNames[column], "", "");
			form.exec();
		} else {
			ActivitiesForm form("", "", allSubjectsNames[column], "");
			form.exec();
		}
	} else if(RBDelete->isChecked()) {
		int students=-1;
		int subject=column;
		if(swapAxis->checkState()==Qt::Checked){
			students=column;
			subject=-1;
		}
		//bool affectOtherStudents=false;
		QList<int> tmpID;
		QList<int> tmpGroupID;
		//QStringList tmpIdentify;
		QSet<int> tmpIdentifySet;
		//QList<bool> tmpAffectOtherStudents;
		Activity* act;
		for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
			act=gt.rules.activitiesList[ai];
			int tmpCurrentIndex=CBActive->currentIndex();
			if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
				|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
				|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE) ){
				if(subject==-1 || (subject>=0 && act->subjectName==allSubjectsNames[subject])){
					foreach(QString st, act->studentsNames){
						if(students==-1 || (students>=0 && st==allStudentsNames[students] )){
							//QString tmpIdent=QString::number(act->id)+" "+QString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<QString::number(act->id)+" "+QString::number(act->activityGroupId);
								tmpIdentifySet.insert(act->id);
								/*if(act->studentsNames.size()>1){
									tmpAffectOtherStudents<<true;
									affectOtherStudents=true;
								} else {
									tmpAffectOtherStudents<<false;
								}*/
							}
							break;
						}
					}
				}
			}
		}
		
		assert(tmpID.count()==tmpIdentifySet.size());
		assert(tmpGroupID.count()==tmpIdentifySet.size());
		//assert(tmpAffectOtherStudents.count()==tmpIdentifySet.size());
		
		if(!tmpIdentifySet.isEmpty()){
			int nTotalActsDeleted;
			QList<int> _idsToBeRemoved;
			QList<int> _agidsToBeRemoved;

			QList<bool> _affectOtherTeachersToBeRemoved;
			bool _affectOtherTeachersOverall;
			QList<bool> _affectOtherStudentsToBeRemoved;
			bool _affectOtherStudentsOverall;
			QList<bool> _affectOtherSubjectsToBeRemoved;
			bool _affectOtherSubjectsOverall;

			if(swapAxis->checkState()==Qt::Checked){
				computeActivitiesForDeletion("", allStudentsNames[column], "",
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				QString s=tr("Delete %1 activities of students set %2?").arg(tmpIdentifySet.size()).arg(allStudentsNames[column]);
				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
				
				int ret=QMessageBox::question(NULL, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				
				if(ret==QMessageBox::Yes){
					int ret2=QMessageBox::No;
					if(_affectOtherStudentsOverall){
						ret2=QMessageBox::question(NULL, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other students sets. Should the related activities also be deleted?"),
							QMessageBox::Yes | QMessageBox::No);
					}
				
					for(int i=0; i<_idsToBeRemoved.count(); i++){
						if((_affectOtherStudentsToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
							gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
						}
					}
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
			else{
				computeActivitiesForDeletion("", "", allSubjectsNames[column],
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				QString s=tr("Delete %1 activities of subject %2?").arg(tmpIdentifySet.size()).arg(allSubjectsNames[column]);
					//+" ("+tr("these are all the activities for this subject, possibly including activities for this subject which are not visible in the current view")+").";
				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
					
				int ret=QMessageBox::question(NULL, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				
				if(ret==QMessageBox::Yes){
					int ret2=QMessageBox::No;
					if(_affectOtherSubjectsOverall){
						ret2=QMessageBox::question(NULL, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other subjects. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
					}
					
					for(int i=0; i<_idsToBeRemoved.count(); i++){
						if((_affectOtherSubjectsToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
							gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
						}
					}
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
		}
	} else assert(0==1);
}

void ActivityPlanningForm::activitiesTableVerticalHeaderClicked(int row){
	if(RBActivity->isChecked()){
		if(swapAxis->checkState()==Qt::Checked){
			ActivitiesForm form("", "", allSubjectsNames[row], "");
			form.exec();
		} else {
			ActivitiesForm form("", allStudentsNames[row], "", "");
			form.exec();
		}
	} else if(RBSubactivity->isChecked()) {
		if(swapAxis->checkState()==Qt::Checked){
			SubactivitiesForm form("", "", allSubjectsNames[row], "");
			form.exec();
		} else {
			SubactivitiesForm form("", allStudentsNames[row], "", "");
			form.exec();
		}
	} else if(RBAdd->isChecked()) {
		if(swapAxis->checkState()==Qt::Checked){
			AddActivityForm addActivityForm("", "", allSubjectsNames[row], "");
			addActivityForm.exec();
		} else {
			AddActivityForm addActivityForm("", allStudentsNames[row], "", "");
			addActivityForm.exec();	
		}
	} else if(RBModify->isChecked()) {
		//Normaly there are to many activities. So just entering the activity form
		if(swapAxis->checkState()==Qt::Checked){
			ActivitiesForm form("", "", allSubjectsNames[row], "");
			form.exec();
		} else {
			ActivitiesForm form("", allStudentsNames[row], "", "");
			form.exec();
		}
	} else if(RBDelete->isChecked()) {
		int students=row;
		int subject=-1;
		if(swapAxis->checkState()==Qt::Checked){
			students=-1;
			subject=row;
		}
		//bool affectOtherStudents=false;
		QList<int> tmpID;
		QList<int> tmpGroupID;
		//QStringList tmpIdentify;
		QSet<int> tmpIdentifySet;
		//QList<bool> tmpAffectOtherStudents;
		Activity* act;
		for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
			act=gt.rules.activitiesList[ai];
			int tmpCurrentIndex=CBActive->currentIndex();
			if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
				|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
				|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
				if(subject==-1 || (subject>=0 && act->subjectName==allSubjectsNames[subject])){
					foreach(QString st, act->studentsNames){
						if(students==-1 || (students>=0 && st==allStudentsNames[students] )){
							//QString tmpIdent=QString::number(act->id)+" "+QString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<QString::number(act->id)+" "+QString::number(act->activityGroupId);
								tmpIdentifySet.insert(act->id);
								/*if(act->studentsNames.size()>1){
									tmpAffectOtherStudents<<true;
									affectOtherStudents=true;
								} else {
									tmpAffectOtherStudents<<false;
								}*/
							}
							break;
						}
					}
				}
			}
		}

		assert(tmpID.count()==tmpIdentifySet.size());
		assert(tmpGroupID.count()==tmpIdentifySet.size());
		//assert(tmpAffectOtherStudents.count()==tmpIdentifySet.size());

		if(!tmpIdentifySet.isEmpty()){
			int nTotalActsDeleted;
			QList<int> _idsToBeRemoved;
			QList<int> _agidsToBeRemoved;

			QList<bool> _affectOtherTeachersToBeRemoved;
			bool _affectOtherTeachersOverall;
			QList<bool> _affectOtherStudentsToBeRemoved;
			bool _affectOtherStudentsOverall;
			QList<bool> _affectOtherSubjectsToBeRemoved;
			bool _affectOtherSubjectsOverall;

			if(swapAxis->checkState()==Qt::Checked){
				computeActivitiesForDeletion("", "", allSubjectsNames[row],
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				QString s=tr("Delete %1 activities of subject %2?").arg(tmpIdentifySet.size()).arg(allSubjectsNames[row]);
					//+" ("+tr("these are all the activities for this subject, possibly including activities for this subject which are not visible in the current view")+").";
				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
					
				int ret=QMessageBox::question(NULL, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				
				if(ret==QMessageBox::Yes){
					int ret2=QMessageBox::No;
					if(_affectOtherSubjectsOverall){
						ret2=QMessageBox::question(NULL, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other subjects. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
					}
					
					for(int i=0; i<_idsToBeRemoved.count(); i++){
						if((_affectOtherSubjectsToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
							gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
						}
					}
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
			else{
				computeActivitiesForDeletion("", allStudentsNames[row], "",
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				QString s=tr("Delete %1 activities of students set %2?").arg(tmpIdentifySet.size()).arg(allStudentsNames[row]);
				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
				
				int ret=QMessageBox::question(NULL, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				
				if(ret==QMessageBox::Yes){
					int ret2=QMessageBox::No;
					if(_affectOtherStudentsOverall){
						ret2=QMessageBox::question(NULL, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other students sets. Should the related activities also be deleted?"),
							QMessageBox::Yes | QMessageBox::No);
					}
				
					for(int i=0; i<_idsToBeRemoved.count(); i++){
						if((_affectOtherStudentsToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
							gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
						}
					}
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
		}
	} else assert(0==1);
}

void ActivityPlanningForm::activitiesCellSelected(const QModelIndex& index){
	if(!index.isValid())
		return;

	int row=index.row();
	int column=index.column();

	int students=row;
	int subject=column;
	if(swapAxis->checkState()==Qt::Checked){
		students=column;
		subject=row;
	}
	if(RBActivity->isChecked()){
		ActivitiesForm form("", allStudentsNames[students], allSubjectsNames[subject], "");
		form.exec();
	} else if(RBSubactivity->isChecked()) {
		SubactivitiesForm form("", allStudentsNames[students], allSubjectsNames[subject], "");
		form.exec();
	} else if(RBAdd->isChecked()) {
		AddActivityForm addActivityForm("", allStudentsNames[students], allSubjectsNames[subject], "");
		addActivityForm.exec();
	} else if(RBModify->isChecked()) {
		QList<int> tmpActivities;
		QList<int> tmpSubactivities;
		QList<int> tmpAGIds;
		Activity* act;
		for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
			act=gt.rules.activitiesList[ai];
			int tmpCurrentIndex=CBActive->currentIndex();
			if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
				|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
				|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
				if(act->subjectName==allSubjectsNames[subject]){
					foreach(QString st, act->studentsNames){
						if(st==allStudentsNames[students]){
							tmpSubactivities<<ai;
							if(act->activityGroupId==0 || act->activityGroupId==act->id)
								tmpActivities<<ai;
							
							tmpAGIds<<act->activityGroupId;
							
							break;
						}
					}
				}
			}
		}
		if(tmpActivities.size()<=1 && tmpSubactivities.count()>=1){
			int agid=-1;
			bool sameAgid=true;
			foreach(int k, tmpAGIds){
				if(agid==-1)
					agid=k;
				else if(agid!=k){
					sameAgid=false;
					break;
				}
			}
			
			if(!sameAgid){
				//SubactivitiesForm form("", allStudentsNames[students], allSubjectsNames[subject], "");
				ActivitiesForm form("", allStudentsNames[students], allSubjectsNames[subject], "");
				form.exec();
				
				return;
			}
			
			if(tmpActivities.count()==0){ //maybe representant is inactive
				assert(tmpAGIds.count()>=1);
			
				for(int i=0; i<gt.rules.activitiesList.count(); i++){
					Activity* act=gt.rules.activitiesList.at(i);
					if(act->id==act->activityGroupId && act->activityGroupId==tmpAGIds.at(0)){
						tmpActivities.append(i);
						//tmpSubactivities.prepend(i);
						//tmpAGIds.prepend(act->activityGroupId);
						break;
					}
				}
			}
			
			assert(tmpActivities.count()==1);
			
			foreach(int tmpAct, tmpActivities){
				Activity* act=gt.rules.activitiesList[tmpAct];
				if(act->isSplit()){
					//similar to activitiesform.cpp by Liviu Lalescu(start)
					//maybe TODO: write a function int activityCheckedManualy in activity.cpp, because we use this already 3 times (me even 5 times)
					//            here, in activitiesform.cpp, in csv export and also in willi2 export and winplan export.
					QStringList _teachers=act->teachersNames;
					bool _diffTeachers=false;
				
					QString _subject=act->subjectName;
					bool _diffSubject=false;
					
					QStringList _activityTags=act->activityTagsNames;
					bool _diffActivityTags=false;
				
					QStringList _students=act->studentsNames;
					bool _diffStudents=false;
					
					int _nTotalStudents=act->nTotalStudents;
					bool _diffNTotalStudents=false;
				
					bool _computeNTotalStudents=act->computeNTotalStudents;
					bool _diffComputeNTotalStudents=false;

					int nSplit=0;
					for(int i=0; i<gt.rules.activitiesList.size(); i++){
						Activity* act2=gt.rules.activitiesList[i];
						if(act2->activityGroupId==act->activityGroupId){
							nSplit++;
							
							if(_teachers!=act2->teachersNames)
								_diffTeachers=true;
							if(_subject!=act2->subjectName)
								_diffSubject=true;
							if(_activityTags!=act2->activityTagsNames)
								_diffActivityTags=true;
							if(_students!=act2->studentsNames)
								_diffStudents=true;
							if( /* !computeNTotalStudents && !act2->computeNTotalStudents && */ _nTotalStudents!=act2->nTotalStudents )
								_diffNTotalStudents=true;
							if(_computeNTotalStudents!=act2->computeNTotalStudents)
								_diffComputeNTotalStudents=true;
						}
						if(nSplit>MAX_SPLIT_OF_AN_ACTIVITY){
							QMessageBox::warning(this, tr("FET information"),
								tr("Cannot modify this large activity, because it contains more than %1 activities. "
								"If you really need that, please talk to the author").arg(MAX_SPLIT_OF_AN_ACTIVITY));
							return;
						}
					}
					if(_diffTeachers || _diffSubject || _diffActivityTags || _diffStudents || _diffNTotalStudents || _diffComputeNTotalStudents){
						/*QStringList s;
						if(diffTeachers)
							s.append(tr("different teachers"));
						if(diffSubject)
							s.append(tr("different subject"));
						if(diffActivityTags)
							s.append(tr("different activity tags"));
						if(diffStudents)
							s.append(tr("different students"));
						if(diffComputeNTotalStudents)
							s.append(tr("different boolean variable 'must compute n total students'"));
						if(diffNTotalStudents)
							s.append(tr("different number of students"));
							
						QString s2;
						s2+=tr("The current split activity has subactivities which were individually modified. It is recommended to abort now"
						 " and modify individual subactivities from the corresponding menu. Otherwise you will modify the fields for all the subactivities"
						 " from this larger split activity.");
						s2+="\n\n";
						s2+=tr("The fields which are different are: %1").arg(s.join(", "));
							
						int t=QMessageBox::warning(this, tr("FET warning"), s2, tr("Abort"), tr("Continue"), QString(), 1, 0);
						
						if(t==0)
							return;*/
						
						//SubactivitiesForm form("", allStudentsNames[students], allSubjectsNames[subject], "");
						ActivitiesForm form("", allStudentsNames[students], allSubjectsNames[subject], "");
						form.exec();
				
						return;
					}
				}
				ModifyActivityForm modifyActivityForm(act->id, act->activityGroupId);
				int t;
				t=modifyActivityForm.exec();
				//similar to activitiesfrom.cpp (end)
				return;
			}
		}
		//else if(tmpSubactivities.count()>=1){
		else{
			ActivitiesForm form("", allStudentsNames[students], allSubjectsNames[subject], "");
			form.exec();
		}
		/*} else if(tmpActivities.size()>1){
			ActivitiesForm form("", allStudentsNames[students], allSubjectsNames[subject], "");
			form.exec();
		} else if(tmpSubactivities.size()==1){
			Activity* act=gt.rules.activitiesList[tmpSubactivities.at(0)];
			ModifySubactivityForm modifySubactivityForm(act->id, act->activityGroupId);
			int t;
			t=modifySubactivityForm.exec();
		} else if(tmpSubactivities.size()>1){
			SubactivitiesForm form("", allStudentsNames[students], allSubjectsNames[subject], "");
			form.exec();
		}*/
	} else if(RBDelete->isChecked()) {
		//bool affectOtherStudents=false;
		QList<int> tmpID;
		QList<int> tmpGroupID;
		//QStringList tmpIdentify;
		QSet<int> tmpIdentifySet;
		//QList<bool> tmpAffectOtherStudents;
		Activity* act;
		for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
			act=gt.rules.activitiesList[ai];
			int tmpCurrentIndex=CBActive->currentIndex();
			if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
				|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
				|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
				if(act->subjectName==allSubjectsNames[subject]){
					foreach(QString st, act->studentsNames){
						if(st==allStudentsNames[students]){
							//QString tmpIdent=QString::number(act->id)+" "+QString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<QString::number(act->id)+" "+QString::number(act->activityGroupId);
								tmpIdentifySet.insert(act->id);
								/*if(act->studentsNames.size()>1){
									tmpAffectOtherStudents<<true;
									affectOtherStudents=true;
								} else {
									tmpAffectOtherStudents<<false;
								}*/
							}
							break;
						}
					}
				}
			}
		}

		assert(tmpID.count()==tmpIdentifySet.size());
		assert(tmpGroupID.count()==tmpIdentifySet.size());
		//assert(tmpAffectOtherStudents.count()==tmpIdentifySet.size());
		
		if(!tmpIdentifySet.isEmpty()){
			int nTotalActsDeleted;
			QList<int> _idsToBeRemoved;
			QList<int> _agidsToBeRemoved;

			QList<bool> _affectOtherTeachersToBeRemoved;
			bool _affectOtherTeachersOverall;
			QList<bool> _affectOtherStudentsToBeRemoved;
			bool _affectOtherStudentsOverall;
			QList<bool> _affectOtherSubjectsToBeRemoved;
			bool _affectOtherSubjectsOverall;

			QString s;
			if(swapAxis->checkState()==Qt::Checked){
				computeActivitiesForDeletion("", allStudentsNames[column], allSubjectsNames[row],
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				s=tr("Delete %1 activities from selected cell?").arg(tmpIdentifySet.size());
				s+=" (";
				s+=tr("subject=%1").arg(allSubjectsNames[row]);
				s+=", ";
				s+=tr("students=%1").arg(allStudentsNames[column]);
				s+=")";

				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
			}
			else{
				computeActivitiesForDeletion("", allStudentsNames[row], allSubjectsNames[column],
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);


				s=tr("Delete %1 activities from selected cell?").arg(tmpIdentifySet.size());
				s+=" (";
				s+=tr("students=%1").arg(allStudentsNames[row]);
				s+=", ";
				s+=tr("subject=%1").arg(allSubjectsNames[column]);
				s+=")";

				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
			}

			int ret=QMessageBox::question(NULL, tr("Delete activities?"), s, QMessageBox::Yes | QMessageBox::No);
			if(ret==QMessageBox::Yes){
				int retst=QMessageBox::No;
				if(_affectOtherStudentsOverall){
					retst=QMessageBox::question(NULL, tr("Delete related?", "It refers to activities"),
						tr("This cell contains activities that relate to other students. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
				}

				int retsubj=QMessageBox::No;
				if(_affectOtherSubjectsOverall){
					retsubj=QMessageBox::question(NULL, tr("Delete related?", "It refers to activities"),
						tr("This cell contains activities that relate to other subjects. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
				}

				for(int i=0; i<_idsToBeRemoved.count(); i++){
					if( ((_affectOtherStudentsToBeRemoved.at(i)==false) || (retst==QMessageBox::Yes)) && 
						((_affectOtherSubjectsToBeRemoved.at(i)==false) || (retsubj==QMessageBox::Yes)) ){
						gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
					}
				}
				PlanningChanged::increasePlanningCommunicationSpinBox();
			}
		}
	} else assert(0==1);
}

//mouseTracking (start 3/3)
/*
void ActivityPlanningForm::ActivtiesCellEntered(int row, int column){
	activitiesTable->setCurrentCell(row, column);
}

void ActivityPlanningForm::TeachersCellEntered(int row, int column){
	teachersTable->setCurrentCell(row, column);
}*/
//mouseTracking (end 3/3)

void ActivityPlanningForm::teachersTableHorizontalHeaderClicked(int column){
	if(RBActivity->isChecked()){
		ActivitiesForm form(allTeachersNames[column], "", "", "");
		form.exec();
	} else if(RBSubactivity->isChecked()) {
		SubactivitiesForm form(allTeachersNames[column], "", "", "");
		form.exec();
	} else if(RBAdd->isChecked()) {
		AddActivityForm addActivityForm(allTeachersNames[column], "", "", "");
		addActivityForm.exec();	
	} else if(RBModify->isChecked()) {
		//Just enter the activity dialog, because in normal case there are to many activities related to a teacher.
		ActivitiesForm form(allTeachersNames[column], "", "", "");
		form.exec();
	} else if(RBDelete->isChecked()) {
		//bool affectOtherTeachers=false;
		QList<int> tmpID;
		QList<int> tmpGroupID;
		//QStringList tmpIdentify;
		QSet<int> tmpIdentifySet;
		//QList<bool> tmpAffectOtherTeachers;
		Activity* act;
		for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
			act=gt.rules.activitiesList[ai];
			int tmpCurrentIndex=CBActive->currentIndex();
			if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
				|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
				|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
					foreach(QString t, act->teachersNames){
						if(t==allTeachersNames[column]){
							//QString tmpIdent=QString::number(act->id)+" "+QString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<QString::number(act->id)+" "+QString::number(act->activityGroupId);
								tmpIdentifySet.insert(act->id);
								/*if(act->teachersNames.size()>1){
									tmpAffectOtherTeachers<<true;
									affectOtherTeachers=true;
								} else {
									tmpAffectOtherTeachers<<false;
								}*/
							}
							break;
						}
					}
			}
		}

		assert(tmpID.count()==tmpIdentifySet.size());
		assert(tmpGroupID.count()==tmpIdentifySet.size());
		//assert(tmpAffectOtherTeachers.count()==tmpIdentifySet.size());

		if(!tmpIdentifySet.isEmpty()){
			int nTotalActsDeleted;
			QList<int> _idsToBeRemoved;
			QList<int> _agidsToBeRemoved;

			QList<bool> _affectOtherTeachersToBeRemoved;
			bool _affectOtherTeachersOverall;
			QList<bool> _affectOtherStudentsToBeRemoved;
			bool _affectOtherStudentsOverall;
			QList<bool> _affectOtherSubjectsToBeRemoved;
			bool _affectOtherSubjectsOverall;

			computeActivitiesForDeletion(allTeachersNames[column], "", "",
				tmpID, tmpGroupID,
				nTotalActsDeleted,
				_idsToBeRemoved, _agidsToBeRemoved,
				_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
				_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
				_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

			QString s=tr("Delete %1 activities of teacher %2?").arg(tmpIdentifySet.size()).arg(allTeachersNames[column]);
			assert(nTotalActsDeleted>=tmpIdentifySet.size());
			if(nTotalActsDeleted>tmpIdentifySet.size()){
				s+="\n\n";
				s+=tr("Because you have individually modified the components, there will be removed an additional number "
					"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
			}
			int ret=QMessageBox::question(NULL, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
			if(ret==QMessageBox::Yes){
				int ret2=QMessageBox::No;
				if(_affectOtherTeachersOverall){
					ret2=QMessageBox::question(NULL, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other teachers. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
				}
				
				for(int i=0; i<_idsToBeRemoved.count(); i++){
					if((_affectOtherTeachersToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
						gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
					}
				}
				PlanningChanged::increasePlanningCommunicationSpinBox();
			}
		}
	} else assert(0==1);
}

void ActivityPlanningForm::teachersCellSelected(const QModelIndex& index){
	/*if(item==NULL){
		return;
	}*/
	if(!index.isValid())
		return;
	int itrow=index.row();
	int itcol=index.column();
	if(itrow==0){
		bool ok=false;
		int targetHours=QInputDialog::getInt(this, tr("FET question"), tr("Please enter the target number of hours:",
			"This field is for the moment inactive in FET, but we translate it for possible future use")+" ", teachersTargetNumberOfHours.at(itcol), 0, gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek, 1, &ok);
		if(ok){
			teachersTargetNumberOfHours[itcol]=targetHours;
			/*useless, because i also need to remove the table head item and i don't know how, so i redo the whole table
			teachersTable->removeCellWidget(0, itcol);
			QTableWidgetItem* newItem2=new QTableWidgetItem(QString::number(teachersTargetNumberOfHours.at(itcol)));
			newItem2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			teachersTable->setItem(0, itcol, newItem2);*/
			
			/*doesn't work, but idea is maybe better/faster!
			QTableWidgetItem* newItem2= teachersTable->takeItem(0, itcol);
			newItem2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			teachersTable->setItem(0, itcol, newItem2);
			QString tmpString=allTeachersNames.at(itcol)+"\n"+QString::number(teachersTotalNumberOfHours.value(allTeachersNames.at(itcol))-teachersTargetNumberOfHours.at(itcol));
			if(teachersTotalNumberOfHours.value(allTeachersNames.at(itcol))!=teachersTotalNumberOfHours2.value(allTeachersNames.at(itcol)))
				tmpString+=" ("+QString::number(teachersTotalNumberOfHours2.value(allTeachersNames.at(itcol))-teachersTargetNumberOfHours.at(itcol))+")";
			QTableWidgetItem* newItem3= teachersTable->takeVerticalHeaderItem(itcol);
			newItem3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			teachersTable->setItem(0, itcol, newItem3);*/
			
			//currently using not fast, but safe calculation
			updateTables_Teachers();
		}
	} else {
		if(RBActivity->isChecked()){
			ActivitiesForm form(allTeachersNames[itcol], "", "", "");
			form.exec();
		} else if(RBSubactivity->isChecked()) {
			SubactivitiesForm form(allTeachersNames[itcol], "", "", "");
			form.exec();
		} else if(RBAdd->isChecked()) {
			AddActivityForm addActivityForm(allTeachersNames[itcol], "", "", "");
			addActivityForm.exec();	
		} else if(RBModify->isChecked()) {
			//Just enter the activity dialog, because in normal case there are to many activities related to a teacher.
			ActivitiesForm form(allTeachersNames[itcol], "", "", "");
			form.exec();
		} else if(RBDelete->isChecked()) {
			//bool affectOtherTeachers=false;
			QList<int> tmpID;
			QList<int> tmpGroupID;
			//QStringList tmpIdentify;
			QSet<int> tmpIdentifySet;
			//QList<bool> tmpAffectOtherTeachers;
			Activity* act;
			for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
				act=gt.rules.activitiesList[ai];
				int tmpCurrentIndex=CBActive->currentIndex();
				if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
					|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
					|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
						foreach(QString t, act->teachersNames){
							if(t==allTeachersNames[itcol]){
								//QString tmpIdent=QString::number(act->id)+" "+QString::number(act->activityGroupId);
								//if(!tmpIdentify.contains(tmpIdent)){
								if(!tmpIdentifySet.contains(act->id)){
									tmpID<<act->id;
									tmpGroupID<<act->activityGroupId;
									//tmpIdentify<<QString::number(act->id)+" "+QString::number(act->activityGroupId);
									tmpIdentifySet.insert(act->id);
									/*if(act->teachersNames.size()>1){
										tmpAffectOtherTeachers<<true;
										affectOtherTeachers=true;
									} else {
										tmpAffectOtherTeachers<<false;
									}*/
								}
								break;
							}
						}
				}
			}
			assert(tmpID.count()==tmpIdentifySet.size());
			assert(tmpGroupID.count()==tmpIdentifySet.size());
			//assert(tmpAffectOtherTeachers.count()==tmpIdentifySet.size());
	
			if(!tmpIdentifySet.isEmpty()){
				int nTotalActsDeleted;
				QList<int> _idsToBeRemoved;
				QList<int> _agidsToBeRemoved;

				QList<bool> _affectOtherTeachersToBeRemoved;
				bool _affectOtherTeachersOverall;
				QList<bool> _affectOtherStudentsToBeRemoved;
				bool _affectOtherStudentsOverall;
				QList<bool> _affectOtherSubjectsToBeRemoved;
				bool _affectOtherSubjectsOverall;
	
				computeActivitiesForDeletion(allTeachersNames[itcol], "", "",
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				QString s=tr("Delete %1 activities of teacher %2?").arg(tmpIdentifySet.size()).arg(allTeachersNames[itcol]);
				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
				int ret=QMessageBox::question(NULL, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				if(ret==QMessageBox::Yes){
					int ret2=QMessageBox::No;
					if(_affectOtherTeachersOverall){
						ret2=QMessageBox::question(NULL, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other teachers. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
					}
					
					for(int i=0; i<_idsToBeRemoved.count(); i++){
						if((_affectOtherTeachersToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
							gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
						}
					}
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
		} else assert(0==1);
	}
}

void ActivityPlanningForm::updateTables(int unneeded){
	Q_UNUSED(unneeded);

	updateTables_Students_Subjects();
	updateTables_Teachers();

	updateTablesVisual(0);
}

void ActivityPlanningForm::updateTables_Students_Subjects(){	//similar to statisticsexport.cpp
	QMultiHash <QString, int> studentsActivities;

	studentsTotalNumberOfHours.clear();
	studentsTotalNumberOfHours2.clear();
	subjectsTotalNumberOfHours.clear();
	subjectsTotalNumberOfHours4.clear();
	
	/*QProgressDialog progress(NULL);
	progress.setLabelText(tr("Calculating number of hours"));
	progress.setModal(true);
	progress.setRange(0, gt.rules.activitiesList.size());*/
	Activity* act;
	int tmpCurrentIndex=CBActive->currentIndex();
	for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
		//progress.setValue(ai);
		act=gt.rules.activitiesList[ai];
		if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
			|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
			|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
				//subjectsActivities.insert(act->subjectName, ai);
				int tmp=subjectsTotalNumberOfHours.value(act->subjectName)+act->duration;
				subjectsTotalNumberOfHours.insert(act->subjectName, tmp);						// (1) so teamlearning-teaching is not counted twice!
				foreach(QString st, act->studentsNames){
					studentsActivities.insert(st, ai);
					tmp=studentsTotalNumberOfHours.value(st)+act->duration;
					studentsTotalNumberOfHours.insert(st, tmp);							// (2)
					//subjectstTotalNumberOfHours3[act->subjectIndex]+=duration;				// (1) so teamlearning is counted twice!
				}
				foreach(QString st, act->studentsNames){
					tmp=studentsTotalNumberOfHours2.value(st);
					tmp += act->duration * act->teachersNames.count();
					studentsTotalNumberOfHours2.insert(st, tmp);					// (2)
				}
				tmp=subjectsTotalNumberOfHours4.value(act->subjectName);
				tmp += act->duration * act->studentsNames.count() * act->teachersNames.count();
				subjectsTotalNumberOfHours4.insert(act->subjectName, tmp);			// (1) so teamlearning-teaching is counted twice!
			}
	}
	//progress.setValue(gt.rules.activitiesList.size());
	QStringList studentsLabels;
	QStringList subjectsLabels;
	studentsLabels.clear();
	subjectsLabels.clear();
	
	activitiesTableView->model.clearDataAndHeaders();

	if(swapAxis->checkState()==Qt::Checked){
		/*activitiesTable->setRowCount(allSubjectsNames.count());
		activitiesTable->setColumnCount(allStudentsNames.count());*/
		activitiesTableView->model.resize(allSubjectsNames.count(), allStudentsNames.count());
	} else {
		/*activitiesTable->setRowCount(allStudentsNames.count());
		activitiesTable->setColumnCount(allSubjectsNames.count());*/
		activitiesTableView->model.resize(allStudentsNames.count(), allSubjectsNames.count());
	}

	/*QProgressDialog progress2(NULL);
	progress2.setLabelText(tr("Preparing students-subject matrix"));
	progress2.setModal(true);
	progress2.setRange(0, allStudentsNames.count());*/
	for(int students=0; students<allStudentsNames.count(); students++){
		//progress2.setValue(students);
		QList<int> tmpStudents;
		QMultiHash<QString, int> tmpSubjects;
		tmpStudents.clear();
		tmpSubjects.clear();
		tmpStudents=studentsActivities.values(allStudentsNames.at(students));
		foreach(int aidx, tmpStudents){
			Activity* act=gt.rules.activitiesList.at(aidx);
			tmpSubjects.insert(act->subjectName, aidx);
		}
		
		QString tmpStudentsLabel="";
		tmpStudentsLabel=allStudentsNames.at(students);
		tmpStudentsLabel+="\n"+QString::number(studentsTotalNumberOfHours.value(allStudentsNames.at(students)));
		if(studentsTotalNumberOfHours.value(allStudentsNames.at(students))!=studentsTotalNumberOfHours2.value(allStudentsNames.at(students)))
			tmpStudentsLabel+=" ("+QString::number(studentsTotalNumberOfHours2.value(allStudentsNames.at(students)))+")";
		studentsLabels<<tmpStudentsLabel;
		for(int subject=0; subject<allSubjectsNames.count(); subject++){
			QList<int> tmpActivities;
			tmpActivities.clear();
			tmpActivities=tmpSubjects.values(allSubjectsNames.at(subject));
			QString tmpString="";
			if(!tmpActivities.isEmpty()){
				QMap<QString, int> tmpIdentDuration;	//not QHash, because i need the correct order of the activities
				foreach(int tmpAct, tmpActivities){
					Activity* act=gt.rules.activitiesList[tmpAct];
					int tmpD=act->duration;
					QString tmpIdent;
					if(showTeachers->checkState()==Qt::Checked){
						if(act->teachersNames.size()>0){
							for(QStringList::Iterator it=act->teachersNames.begin(); it!=act->teachersNames.end(); it++){
								tmpIdent+=*it;
								if(it!=act->teachersNames.end()-1)
									tmpIdent+=", ";
							}
						}
					}
					tmpIdent+=" ";
					if(showActivityTags->checkState()==Qt::Checked){
						if(act->activityTagsNames.size()>0){
							for(QStringList::Iterator it=act->activityTagsNames.begin(); it!=act->activityTagsNames.end(); it++){
								tmpIdent+=*it;
								if(it!=act->activityTagsNames.end()-1)
									tmpIdent+=", ";
							}
						}
					}
					tmpD+=tmpIdentDuration.value(tmpIdent);
					tmpIdentDuration.insert(tmpIdent, tmpD);
				}
				QMapIterator<QString, int> it(tmpIdentDuration);
				while(it.hasNext()){
					it.next();
					tmpString+=QString::number(it.value());
					tmpString+=" "+it.key();
					if(it.hasNext())
						tmpString+="\n";
				}
			}
			if(!tmpActivities.isEmpty()){
				/*QTableWidgetItem* newItem=new QTableWidgetItem(tmpString);
				newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);*/
				QPair<int, int> pair;
				if(swapAxis->checkState()==Qt::Checked){
					pair.first=subject;
					pair.second=students;
					//activitiesTable->setItem(subject, students, newItem);
				} else {
					pair.first=students;
					pair.second=subject;
					//activitiesTable->setItem(students, subject, newItem);
				}
				activitiesTableView->model.items.insert(pair, tmpString);
			}
		}
	}
	//progress2.setValue(allStudentsNames.count());
	foreach(QString subjects, allSubjectsNames){
		QString tmpSubjectLabel="";
		tmpSubjectLabel=subjects+"\n"+QString::number(subjectsTotalNumberOfHours.value(subjects));
		if(subjectsTotalNumberOfHours.value(subjects)!=subjectsTotalNumberOfHours4.value(subjects))
			tmpSubjectLabel+=" ("+QString::number(subjectsTotalNumberOfHours4.value(subjects))+")";
		subjectsLabels<<tmpSubjectLabel;
	}
	if(swapAxis->checkState()==Qt::Checked){
		/*activitiesTableView->setHorizontalHeaderLabels(studentsLabels);
		activitiesTableView->setVerticalHeaderLabels(subjectsLabels);*/
		activitiesTableView->model.horizontalHeaderItems=studentsLabels;
		activitiesTableView->model.verticalHeaderItems=subjectsLabels;
	} else {
		/*activitiesTableView->setHorizontalHeaderLabels(subjectsLabels);
		activitiesTableView->setVerticalHeaderLabels(studentsLabels);*/
		activitiesTableView->model.horizontalHeaderItems=subjectsLabels;
		activitiesTableView->model.verticalHeaderItems=studentsLabels;
	}
	
	activitiesTableView->allTableChanged();
}


void ActivityPlanningForm::updateTables_Teachers(){	//similar to statisticsexport.cpp
	QMultiHash <QString, int> teachersActivities;
	
	teachersTotalNumberOfHours.clear();
	teachersTotalNumberOfHours2.clear();
	
	/*QProgressDialog progress(NULL);
	progress.setLabelText(tr("Calculating number of hours"));
	progress.setModal(true);
	progress.setRange(0, gt.rules.activitiesList.size());*/
	Activity* act;
	int tmpCurrentIndex=CBActive->currentIndex();
	for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
		//progress.setValue(ai);
		act=gt.rules.activitiesList[ai];
		if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
			|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
			|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
				int tmp;
				foreach(QString t, act->teachersNames){
					teachersActivities.insert(t, ai);
					tmp=teachersTotalNumberOfHours.value(t)+act->duration;
					teachersTotalNumberOfHours.insert(t, tmp);							// (3)
				}
				foreach(QString t, act->teachersNames){
					tmp=teachersTotalNumberOfHours2.value(t);
					tmp += act->duration * act->studentsNames.count();
					teachersTotalNumberOfHours2.insert(t, tmp);						// (3)
				}
			}
	}
	//progress.setValue(gt.rules.activitiesList.size());

	QStringList teachersLabels;
	for(int teacher=0; teacher<allTeachersNames.count(); teacher++){
		QString prefixString1="";
		QString prefixString2="";
		if(teachersTargetNumberOfHours.at(teacher)!=0){
			if(teachersTotalNumberOfHours.value(allTeachersNames.at(teacher))>teachersTargetNumberOfHours.at(teacher)){
				prefixString1="+";
			} else if(teachersTotalNumberOfHours.value(allTeachersNames.at(teacher))==teachersTargetNumberOfHours.at(teacher)){
				prefixString1="=";
			}
			if(teachersTotalNumberOfHours2.value(allTeachersNames.at(teacher))>teachersTargetNumberOfHours.at(teacher)){
				prefixString2="+";
			}else if(teachersTotalNumberOfHours2.value(allTeachersNames.at(teacher))>teachersTargetNumberOfHours.at(teacher)){
				prefixString2="=";
			}
		}
		QString tmpString=allTeachersNames.at(teacher)+"\n"+prefixString1+QString::number(teachersTotalNumberOfHours.value(allTeachersNames.at(teacher))-teachersTargetNumberOfHours.at(teacher));
		if(teachersTotalNumberOfHours.value(allTeachersNames.at(teacher))!=teachersTotalNumberOfHours2.value(allTeachersNames.at(teacher)))
			tmpString+=" ("+prefixString2+QString::number(teachersTotalNumberOfHours2.value(allTeachersNames.at(teacher))-teachersTargetNumberOfHours.at(teacher))+")";
		teachersLabels<<tmpString;
	}
	
	teachersTableView->model.clearDataAndHeaders();

	teachersTableView->model.resize(3, allTeachersNames.count());
	
	QStringList vLabels;
	vLabels<<tr("Target", "Please keep translation short. This field is for the moment inactive in FET, we keep it for possible future use")
		<<tr("Subjects", "Please keep translation short")
		<<tr("Students", "Please keep translation short");

	teachersTableView->model.verticalHeaderItems=vLabels;
	teachersTableView->model.horizontalHeaderItems=teachersLabels;
	
	for(int teacher=0; teacher<allTeachersNames.count(); teacher++){
		//QTableWidgetItem* newItem=new QTableWidgetItem(allTeachersNames[teacher]);
		//newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		/*QTableWidgetItem* newItem2=new QTableWidgetItem(QString::number(teachersTargetNumberOfHours.at(teacher)));
		newItem2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		teachersTable->setItem(0, teacher, newItem2);*/
		QString str=QString::number(teachersTargetNumberOfHours.at(teacher));
		QPair<int, int> pair;
		pair.first=0;
		pair.second=teacher;
		teachersTableView->model.items.insert(pair, str);
		
		//new (start)
		QList<int> tmpActivities;
		QMap<QString, int> tmpSubjectsNumberOfHours;		//using map, because it sort alphabeticly
		QMap<QString, int> tmpStudentsNumberOfHours;		//using map, because it sort alphabeticly
		tmpActivities.clear();
		tmpActivities=teachersActivities.values(allTeachersNames.at(teacher));
		foreach(int aidx, tmpActivities){
			Activity* act=gt.rules.activitiesList.at(aidx);
			//students start
			int tmpD=act->duration;
			QString tmpIdent;
			if(act->studentsNames.size()>0){
				for(QStringList::Iterator it=act->studentsNames.begin(); it!=act->studentsNames.end(); it++){
					tmpIdent+=*it;
					if(it!=act->studentsNames.end()-1)
						tmpIdent+=", ";
				}
			}
			tmpIdent+=" ";
			if(showActivityTags->checkState()==Qt::Checked){
				if(act->activityTagsNames.size()>0){
					for(QStringList::Iterator it=act->activityTagsNames.begin(); it!=act->activityTagsNames.end(); it++){
						tmpIdent+=*it;
						if(it!=act->activityTagsNames.end()-1)
							tmpIdent+=", ";
					}
				}
			}
			tmpD+=tmpStudentsNumberOfHours.value(tmpIdent);
			tmpStudentsNumberOfHours.insert(tmpIdent, tmpD);
			//students end
			//subject start
			tmpD=act->duration;
			tmpIdent.clear();
			tmpIdent=act->subjectName;
			tmpIdent+=" ";
			if(showActivityTags->checkState()==Qt::Checked){
				if(act->activityTagsNames.size()>0){
					for(QStringList::Iterator it=act->activityTagsNames.begin(); it!=act->activityTagsNames.end(); it++){
						tmpIdent+=*it;
						if(it!=act->activityTagsNames.end()-1)
							tmpIdent+=", ";
					}
				}
			}
			tmpD+=tmpSubjectsNumberOfHours.value(tmpIdent);
			tmpSubjectsNumberOfHours.insert(tmpIdent, tmpD);
			//subject end
		}
		
		QString tmpItem;
		tmpItem.clear();
		QMapIterator<QString, int> it(tmpSubjectsNumberOfHours);
		while(it.hasNext()){
			it.next();
			tmpItem+=QString::number(it.value())+" "+it.key()+"\n";
		}
		//
		/*QTableWidgetItem* newItem3=new QTableWidgetItem(tmpItem);
		newItem3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		teachersTable->setItem(1, teacher, newItem3);*/
		//
		pair.first=1;
		pair.second=teacher;
		teachersTableView->model.items.insert(pair, tmpItem);
		
		tmpItem.clear();
		QMapIterator<QString, int> it2(tmpStudentsNumberOfHours);
		while(it2.hasNext()){
			it2.next();
			tmpItem+=QString::number(it2.value())+" "+it2.key()+"\n";
		}
		//
		/*QTableWidgetItem* newItem4=new QTableWidgetItem(tmpItem);
		newItem4->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		teachersTable->setItem(2, teacher, newItem4);*/
		//
		pair.first=2;
		pair.second=teacher;
		teachersTableView->model.items.insert(pair, tmpItem);
		//new(end)
	}
	
	//teachersTargetNumberOfHours; Delete this lines! (start)
	teachersTableView->hideRow(0);
	hideUsedTeachers->hide();
	//teachersTargetNumberOfHours; Delete this lines! (end)
	
	teachersTableView->allTableChanged();
}

void ActivityPlanningForm::updateTablesVisual(int unneeded){
	Q_UNUSED(unneeded);

	assert(studentsDuplicates.count()==allStudentsNames.count());
	assert(studentsDuplicates.count()==yearORgroupORsubgroup.count());
	
	if(swapAxis->checkState()==Qt::Checked){
		for(int students=0; students<allStudentsNames.count(); students++){
			bool show=true;
			int tmpInt=yearORgroupORsubgroup.at(students);
			switch(tmpInt){
				case IS_YEAR:     if(showYears->checkState()!=Qt::Checked) show=false; break;
				case IS_GROUP:    if(showGroups->checkState()!=Qt::Checked) show=false; break;
				case IS_SUBGROUP: if(showSubgroups->checkState()!=Qt::Checked) show=false; break;
				default: assert(0==1);
			}
			if((studentsDuplicates.at(students)) && (showDuplicates->checkState()!=Qt::Checked)){
				show=false;
			}
			if(studentsTotalNumberOfHours.value(allStudentsNames.at(students))==0 && studentsTotalNumberOfHours2.value(allStudentsNames.at(students))==0 && hideEmptyLines->checkState()==Qt::Checked){
				show=false;
			}
			if(show)
				activitiesTableView->showColumn(students);
			else
				activitiesTableView->hideColumn(students);
		}
		for(int subject=0; subject<allSubjectsNames.count(); subject++){
			bool show=true;
			if(subjectsTotalNumberOfHours.value(allSubjectsNames.at(subject))==0 && subjectsTotalNumberOfHours4.value(allSubjectsNames.at(subject))==0 && hideEmptyLines->checkState()==Qt::Checked){
				show=false;
			}
			if(show)
				activitiesTableView->showRow(subject);
			else
				activitiesTableView->hideRow(subject);
		}
	} else {
		for(int students=0; students<allStudentsNames.count(); students++){
			bool show=true;
			int tmpInt=yearORgroupORsubgroup.at(students);
			switch(tmpInt){
				case IS_YEAR:     if(showYears->checkState()!=Qt::Checked) show=false; break;
				case IS_GROUP:    if(showGroups->checkState()!=Qt::Checked) show=false; break;
				case IS_SUBGROUP: if(showSubgroups->checkState()!=Qt::Checked) show=false; break;
				default: assert(0==1);
			}
			if((studentsDuplicates.at(students)) && (showDuplicates->checkState()!=Qt::Checked)){
				show=false;
			}
			if(studentsTotalNumberOfHours.value(allStudentsNames.at(students))==0 && studentsTotalNumberOfHours2.value(allStudentsNames.at(students))==0 && hideEmptyLines->checkState()==Qt::Checked){
				show=false;
			}
			
			if(show)
				activitiesTableView->showRow(students);
			else
				activitiesTableView->hideRow(students);
		}
		for(int subject=0; subject<allSubjectsNames.count(); subject++){
			bool show=true;
			if(subjectsTotalNumberOfHours.value(allSubjectsNames.at(subject))==0 && subjectsTotalNumberOfHours4.value(allSubjectsNames.at(subject))==0 && hideEmptyLines->checkState()==Qt::Checked){
				show=false;
			}
			
			if(show)
				activitiesTableView->showColumn(subject);
			else
				activitiesTableView->hideColumn(subject);
		}
	}
	
	for(int teacher=0; teacher<allTeachersNames.count(); teacher++){
		bool show=true;
		if((teachersTargetNumberOfHours.at(teacher)-teachersTotalNumberOfHours.value(allTeachersNames.at(teacher)))==0 && (teachersTargetNumberOfHours.at(teacher)-teachersTotalNumberOfHours2.value(allTeachersNames.at(teacher)))==0 && hideUsedTeachers->checkState()==Qt::Checked){
			show=false;
		}
		if(show)
			teachersTableView->showColumn(teacher);
		else
			teachersTableView->hideColumn(teacher);
	}
	
	/*activitiesTable->resizeColumnsToContents();
	activitiesTable->resizeRowsToContents();*/
	activitiesTableView->resizeToFit();
	
	/*teachersTable->resizeColumnsToContents();
	teachersTable->resizeRowsToContents();*/
	teachersTableView->resizeToFit();
}

void ActivityPlanningForm::deleteAll(){
	int ret=QMessageBox::question(NULL, tr("Delete all?", "It refers to activities"), tr("Are you sure you want to remove ALL the %1 activities and related constraints?", "%1 is the number of total activities")
		.arg(gt.rules.activitiesList.count()), QMessageBox::Yes | QMessageBox::No);
	if(ret==QMessageBox::Yes){
		ret=QMessageBox::question(NULL, tr("Delete all?", "It refers to activities"), tr("Are you absolutely sure you want to remove ALL activities and related constraints from your data?"), QMessageBox::Yes | QMessageBox::No);
		if(ret==QMessageBox::Yes){
			while(!gt.rules.activitiesList.isEmpty()){
				Activity* act=gt.rules.activitiesList.at(0);
				gt.rules.removeActivity(act->id, act->activityGroupId);
			}
			PlanningChanged::increasePlanningCommunicationSpinBox();
		}
	}
}

void ActivityPlanningForm::pseudoActivities(){
	QString noTeachersAndStudents;
	QString noTeachers;
	QString noStudents;
	Activity* act;
	int tmpCurrentIndex=CBActive->currentIndex();
	for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
		act=gt.rules.activitiesList[ai];
		if(	((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
			|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
			|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
				if(act->teachersNames.isEmpty() && act->studentsNames.isEmpty()){
					noTeachersAndStudents+=act->getDescription(gt.rules)+"\n";
				} else if(act->teachersNames.isEmpty()){
					noTeachers+=act->getDescription(gt.rules)+"\n";
				} else if(act->studentsNames.isEmpty()){
					noStudents+=act->getDescription(gt.rules)+"\n";
				}
			}
	}
	if(noTeachersAndStudents.isEmpty()){
		noTeachersAndStudents=tr("There are no activities without teachers and without students.")+"\n";
	} else {
		noTeachersAndStudents=tr("Activities without teachers and without students:")+"\n"+noTeachersAndStudents;
	}
	if(noTeachers.isEmpty()){
		noTeachers="\n"+tr("There are no activities without teachers.")+"\n";
	} else {
		noTeachers="\n"+tr("Activities without teachers:")+"\n"+noTeachers;
	}
	if(noStudents.isEmpty()){
		noStudents="\n"+tr("There are no activities without students.")+"\n";
	} else {
		noStudents="\n"+tr("Activities without students:")+"\n"+noStudents;
	}
	LongTextMessageBox::mediumInformation(NULL, tr("Information about pseudo activities", "Pseudo activities means activities without teachers and/or students sets"), noTeachersAndStudents+noTeachers+noStudents);
}

/*
void ActivityPlanningForm::help(){
	//string is just copy and past from ActivityPlanningConfirmationForm
	QString s;
	
	s+=tr("Please read carefully the description below:");
	s+="\n\n";
	s+=tr("This function is new and not thoroughly tested. Please report any problems.");
	s+="\n\n";
	s+=tr("This is a simple activity planning form. You need to add all teachers, "
		"subjects and the students structure before you can work with this form. It "
		"is also recommended to add the necessary activity tags before using this dialog.");
	s+="\n\n";
	s+=tr("The main work will be done in the upper table with the students-subjects "
		"matrix. Select an 'action' from the right side and and just click into a "
		"cell to do that action. The number of hours is summed in the heading, so "
		"you can always check if your planning fits your needs.");
	s+="\n\n";
	s+=tr("You can also click the tables heading to do an action to the whole row/column.");
	s+="\n\n";
	s+=tr("You can change size of the tables by using the splitter between the tables.");
	s+="\n\n";
	s+=tr("Pseudo activities are (sometimes) difficult to see in the tables, so "
		"you can view them quickly with the corresponding push button.");
		
	LongTextMessageBox::mediumInformation(NULL, tr("Help about activity planning"), s);
}*/

//now communication box by Liviu Lalescu

PlanningCommunicationSpinBox::PlanningCommunicationSpinBox()
{
	minValue=0;
	maxValue=9;
	value=0;
}

PlanningCommunicationSpinBox::~PlanningCommunicationSpinBox()
{
}

void PlanningCommunicationSpinBox::increaseValue()
{
	assert(maxValue>minValue);
	assert(value>=minValue && value<=maxValue);
	value++;
	if(value>maxValue)
		value=minValue;
		
	//cout<<"comm. spin box: increased value, crt value=="<<value<<endl;
	
	emit(valueChanged(value));
}


void PlanningChanged::increasePlanningCommunicationSpinBox()
{
	planningCommunicationSpinBox.increaseValue();
}
