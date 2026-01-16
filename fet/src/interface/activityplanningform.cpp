/***************************************************************************
                                FET
                          -------------------
   copyright            : (C) by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************
                      activityplanningform.cpp  -  description
                             -------------------
    begin                : Dec 2009
    copyright            : (C) by Volker Dirr
                         : https://www.timetabling.de/
 ***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

// Code contributed by Volker Dirr ( https://www.timetabling.de/ )
// Many thanks to Liviu Lalescu. He told me some speed optimizations.

#include <Qt>
#include <QtGlobal>
#include <QSizePolicy>

#include "activityplanningform.h"
#include "statisticsexport.h"

#include "timetable.h"
#include "timetable_defs.h"

#include "studentsset.h"

// BE CAREFUL: DON'T USE INTERNAL VARIABLES HERE, because maybe computeInternalStructure() is not done!

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QInputDialog>

#include <QToolButton>

#include <QTableWidget>
#include <QHeaderView>

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
#include <QPair>

#include <QRadioButton>
#include <QCheckBox>

#include <QList>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

extern const QString COMPANY;
extern const QString PROGRAM;

const QString RBActivityState="/activity-radio-button-state";
const QString RBSubactivityState="/subactivity-radio-button-state";
const QString RBAddState="/add-activity-radio-button-state";
const QString RBModifyState="/modify-activity-radio-button-state";
const QString RBDeleteState="/delete-activities-radio-button-state";
const QString RBChangeTeacherState="/change-teacher-radio-button-state";
const QString RBSwapTeachersState="/swap-teachers-radio-button-state";
const QString RBSwapStudentsState="/swap-students-radio-button-state";
////
const QString CBActiveState="/active-combo-box-state";
////
const QString showYearsState="/show-years-check-box-state";
const QString showGroupsState="/show-groups-check-box-state";
const QString showSubgroupsState="/show-subgroups-check-box-state";
const QString showTeachersState="/show-teachers-check-box-state";
const QString showActivityTagsState="/show-activity-tags-check-box-state";
const QString showDuplicatesState="/show-duplicates-check-box-state";
const QString hideEmptyLinesState="/hide-empty-lines-check-box-state";
const QString swapAxesState="/swap-axes-check-box-state";
const QString hideFullTeachersState="/hide-full-teachers-state";

//maybe put following in timetable_defs.h? (start)
const int ACTIVE_ONLY = 0;
const int INACTIVE_ONLY = 1;
const int ACTIVE_OR_INACTIVE = 2;
//maybe put following in timetable_defs.h? (end)

extern Timetable gt;

static FetStatistics statisticValues;			//maybe TODO: do it more local
static QList<bool> studentsDuplicates;			//maybe TODO: do it more local
static QList<int> yearORgroupORsubgroup;		//maybe TODO: do it more local

//TODO: need to setDefaultValue for the QHash-es ? (sum/number of hours) (also in statisticsexport?) looks like it is unneeded.
//TODO: check with toggled
//TODO: retry mouseTracking (still in source. search "mouseTracking"). check: move mouse one last visibile line. is header always highlighted, under all operating systems?!
//TODO: update if a new teacher, subject or year/group/subgroup is added - or better: just disalow that?!
//      write a "updateBasic"-function with source from constructor if you want to update/care about teacher, subject and students
//TODO: add a new row in teachers table with "comments" - of course saving data is needed to be useful
//MAYBE TODO: display "related" activities. so display all activities of a subgroup also in its related year and group. Problem: Memory and speed!
//            ! I don't like this feature, because using this activities table will be much more difficult, because it mean each cell contains (normaly) more than 1 activity!
//            ! So all shortcuts (especialy delete, add and modify) will be useless!
//            ! The table will be very full, so you will lose clear view!

static QList<int> teachersTargetNumberOfHours;
static QList<Teacher*> teachersList;
static QList<QStringList> subjectListWithQualifiedTeachers;	//maybe TODO: do it more local

PlanningCommunicationSpinBox planningCommunicationSpinBox;

StartActivityPlanning::StartActivityPlanning()
{
}

StartActivityPlanning::~StartActivityPlanning()
{
}

void StartActivityPlanning::startActivityPlanning(QWidget* parent){
	assert(gt.rules.initialized);
	teachersTargetNumberOfHours.clear();
	teachersList.clear();
	
	statisticValues.allStudentsNames.clear();
	studentsDuplicates.clear();
	yearORgroupORsubgroup.clear();
	
	QSet<QString> allStudentsNamesSet;
	
	for(StudentsYear* sty : std::as_const(gt.rules.yearsList)){
		if(allStudentsNamesSet.contains(sty->name)){
			studentsDuplicates<<true;
		} else {
			studentsDuplicates<<false;
		}
		statisticValues.allStudentsNames<<sty->name;
		allStudentsNamesSet.insert(sty->name);
		yearORgroupORsubgroup<<STUDENTS_YEAR;
		for(StudentsGroup* stg : std::as_const(sty->groupsList)){
			if(allStudentsNamesSet.contains(stg->name)){
				studentsDuplicates<<true;
			} else {
				studentsDuplicates<<false;
			}
			statisticValues.allStudentsNames<<stg->name;
			allStudentsNamesSet.insert(stg->name);
			yearORgroupORsubgroup<<STUDENTS_GROUP;
			if(SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING) for(StudentsSubgroup* sts : std::as_const(stg->subgroupsList)){
				if(allStudentsNamesSet.contains(sts->name)){
					studentsDuplicates<<true;
				} else {
					studentsDuplicates<<false;
				}
				statisticValues.allStudentsNames<<sts->name;
				allStudentsNamesSet.insert(sts->name);
				yearORgroupORsubgroup<<STUDENTS_SUBGROUP;
			}
		}
	}
	
	statisticValues.allTeachersNames.clear();		// just needed, because I don't need to care about correct iTeachersList if I do it this way.
	for(Teacher* t : std::as_const(gt.rules.teachersList)){		// So I don't need gt.rules.computeInternalStructure();
		statisticValues.allTeachersNames << t->name;
		teachersTargetNumberOfHours << t->targetNumberOfHours;
		teachersList<<t;
	}
	
	QHash<QString, int> hashSubjectID;
	for(int i=0; i<gt.rules.subjectsList.count(); i++){
		hashSubjectID.insert(gt.rules.subjectsList[i]->name, i);
	}
	statisticValues.allSubjectsNames.clear();		// just done, because I always want to do it the same way + it is faster
	subjectListWithQualifiedTeachers.clear();
	for(Subject* s : std::as_const(gt.rules.subjectsList)){		// Also don't display empty subjects is easier
		statisticValues.allSubjectsNames<<s->name;
		QStringList empty;
		subjectListWithQualifiedTeachers<<empty;
	}
	for(Teacher* t : std::as_const(gt.rules.teachersList)){
		for(const QString& subject : std::as_const(t->qualifiedSubjectsList)){
			int index=hashSubjectID.value(subject, -1);
			assert(index>-1);
			subjectListWithQualifiedTeachers[index]<<t->name;
		}
	}
	
	ActivityPlanningForm apfd(parent);
	setParentAndOtherThings(&apfd, parent);
	apfd.exec();
	
	statisticValues.allStudentsNames.clear();
	statisticValues.allTeachersNames.clear();
	statisticValues.allSubjectsNames.clear();
	
	studentsDuplicates.clear();
	yearORgroupORsubgroup.clear();

	statisticValues.teachersTotalNumberOfHours.clear();
	statisticValues.teachersTotalNumberOfHours2.clear();
	statisticValues.studentsTotalNumberOfHours.clear();
	statisticValues.studentsTotalNumberOfHours2.clear();
	statisticValues.subjectsTotalNumberOfHours.clear();
	statisticValues.subjectsTotalNumberOfHours4.clear();
}

// this is very similar to statisticsexport.cpp. so please also check there if you change something here!
ActivityPlanningForm::ActivityPlanningForm(QWidget *parent): QDialog(parent)
{
	storeStudentsForSwap=-1;
	storeSubjectForSwap=-1;
	
	this->setWindowTitle(tr("Activity Planning Dialog"));
	
	QHBoxLayout* wholeDialog=new QHBoxLayout(this);
	
	leftSplitter=new QSplitter(Qt::Vertical);
	//leftSplitter->setChildrenCollapsible(false);
	
	activitiesTableView=new SparseTableView;
	tableViewSetHighlightHeader(activitiesTableView);

	teachersTableView=new SparseTableView;
	tableViewSetHighlightHeader(teachersTableView);

	leftSplitter->addWidget(activitiesTableView);
	leftSplitter->addWidget(teachersTableView);

	leftSplitter->setStretchFactor(0, 10);
	leftSplitter->setStretchFactor(1, 4);
	leftSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* rightDialog=new QVBoxLayout();
	
	QGroupBox* actionsBox=new QGroupBox();
	QVBoxLayout* actionsBoxVertical=new QVBoxLayout();
	RBActivity=new QRadioButton(tr("Activities", "Please keep translation short"));
	RBSubactivity=new QRadioButton(tr("Subactivities", "Please keep translation short"));
	RBAdd=new QRadioButton(tr("Add activity", "Please keep translation short"));
	RBModify=new QRadioButton(tr("Modify activity", "Please keep translation short"));
	RBDelete=new QRadioButton(tr("Delete activities", "Please keep translation short"));
	RBChangeTeacher=new QRadioButton(tr("Change teacher", "Please keep translation short"));
	RBSwapTeachers=new QRadioButton(tr("Swap teachers", "Please keep translation short"));
	RBSwapStudents=new QRadioButton(tr("Swap students", "Please keep translation short"));

	actionsBoxVertical->addWidget(RBActivity);
	actionsBoxVertical->addWidget(RBSubactivity);
	actionsBoxVertical->addWidget(RBAdd);
	actionsBoxVertical->addWidget(RBModify);
	actionsBoxVertical->addWidget(RBDelete);
	actionsBoxVertical->addWidget(RBChangeTeacher);
	actionsBoxVertical->addWidget(RBSwapTeachers);
	actionsBoxVertical->addWidget(RBSwapStudents);
	RBActivity->setChecked(true);
	actionsBox->setLayout(actionsBoxVertical);
	
	QGroupBox* optionsBox=new QGroupBox();
	QVBoxLayout* optionsBoxVertical=new QVBoxLayout();
	QStringList activeStrings;
	//please do not modify order for these below, as the current combobox index is saved in settings and restored
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
	showSubgroups->setEnabled(SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING);
	showTeachers=new QCheckBox(tr("Show teachers", "Please keep translation short"));
	showTeachers->setChecked(true);
	//showActivityTags=new QCheckBox(tr("Show activity tags", "Please keep translation short"));
	showActivityTags=new QCheckBox(tr("Show tags", "Please keep translation short. It refers to activity tags, but I want to keep it shorter, because there is not enough space."));
	showActivityTags->setChecked(false);
	showDuplicates=new QCheckBox(tr("Show duplicates", "Please keep translation short"));
	showDuplicates->setChecked(false);
	hideEmptyLines=new QCheckBox(tr("Hide empty lines", "Please keep translation short"));
	hideEmptyLines->setChecked(false);
	hideUsedTeachers=new QCheckBox(tr("Hide full teachers", "It refers to teachers who have their number of hours fulfilled. Please keep translation short"));
	hideUsedTeachers->setChecked(false); //important to not hide teachers without activities, if target number of hours is deactivated
	swapAxes=new QCheckBox(tr("Swap axes", "Please keep translation short"));
	swapAxes->setChecked(false);
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
	optionsBoxVertical->addWidget(swapAxes);
	//optionsBoxVertical->addWidget(pbPseudoActivities);
	optionsBox->setLayout(optionsBoxVertical);
	
	actionsOptionsTabWidget=new QTabWidget;
	actionsOptionsTabWidget->addTab(actionsBox, tr("Actions"));
	actionsOptionsTabWidget->addTab(optionsBox, tr("Options"));
	actionsOptionsTabWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	
	rightDialog->addWidget(actionsOptionsTabWidget);
	rightDialog->addStretch();
	rightDialog->addWidget(pbPseudoActivities);
	//rightDialog->addStretch();
	rightDialog->addWidget(pbDeleteAll);
	//rightDialog->addStretch();
	//rightDialog->addWidget(pbHelp);
	rightDialog->addWidget(showHideButton);
	rightDialog->addStretch();
	rightDialog->addWidget(pbClose);
	
	wholeDialog->addWidget(leftSplitter);
	wholeDialog->addLayout(rightDialog);
	
	teachersTableView->setSelectionMode(QAbstractItemView::SingleSelection);

	activitiesTableView->setSelectionMode(QAbstractItemView::SingleSelection);
	
	//maybe disable AlternatingColors as soon as mouseTracking works correctly?!
	activitiesTableView->setAlternatingRowColors(true);

	teachersTableView->setAlternatingRowColors(true); //by Liviu
	//mouseTracking (start 1/3)
	/*
	activitiesTable->setMouseTracking(true);
	teachersTable->setMouseTracking(true);
	*/
	//mouseTracking (end 1/3)
	
	int hh=560;
	if(hh<this->minimumSizeHint().height())
		hh=this->minimumSizeHint().height();
	this->resize(840, hh);
	
	buttonsVisible=true;

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		leftSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	////////
	if(settings.contains(this->metaObject()->className()+QString("/buttons-visible"))){
		bool bv=settings.value(this->metaObject()->className()+QString("/buttons-visible")).toBool();
		if(bv==false){
			showHide();
		}
	}
	//restore other settings
	if(settings.contains(this->metaObject()->className()+RBActivityState))
		RBActivity->setChecked(settings.value(this->metaObject()->className()+RBActivityState).toBool());
	if(settings.contains(this->metaObject()->className()+RBSubactivityState))
		RBSubactivity->setChecked(settings.value(this->metaObject()->className()+RBSubactivityState).toBool());
	if(settings.contains(this->metaObject()->className()+RBAddState))
		RBAdd->setChecked(settings.value(this->metaObject()->className()+RBAddState).toBool());
	if(settings.contains(this->metaObject()->className()+RBModifyState))
		RBModify->setChecked(settings.value(this->metaObject()->className()+RBModifyState).toBool());
	if(settings.contains(this->metaObject()->className()+RBDeleteState))
		RBDelete->setChecked(settings.value(this->metaObject()->className()+RBDeleteState).toBool());
	if(settings.contains(this->metaObject()->className()+RBChangeTeacherState))
		RBChangeTeacher->setChecked(settings.value(this->metaObject()->className()+RBChangeTeacherState).toBool());
	if(settings.contains(this->metaObject()->className()+RBSwapTeachersState))
		RBSwapTeachers->setChecked(settings.value(this->metaObject()->className()+RBSwapTeachersState).toBool());
	if(settings.contains(this->metaObject()->className()+RBSwapStudentsState))
		RBSwapStudents->setChecked(settings.value(this->metaObject()->className()+RBSwapStudentsState).toBool());
	//
	if(settings.contains(this->metaObject()->className()+CBActiveState))
		CBActive->setCurrentIndex(settings.value(this->metaObject()->className()+CBActiveState).toInt());
	//
	if(settings.contains(this->metaObject()->className()+showYearsState))
		showYears->setChecked(settings.value(this->metaObject()->className()+showYearsState).toBool());
	if(settings.contains(this->metaObject()->className()+showGroupsState))
		showGroups->setChecked(settings.value(this->metaObject()->className()+showGroupsState).toBool());
	if(SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING)
		if(settings.contains(this->metaObject()->className()+showSubgroupsState))
			showSubgroups->setChecked(settings.value(this->metaObject()->className()+showSubgroupsState).toBool());
	if(settings.contains(this->metaObject()->className()+showTeachersState))
		showTeachers->setChecked(settings.value(this->metaObject()->className()+showTeachersState).toBool());
	if(settings.contains(this->metaObject()->className()+showActivityTagsState))
		showActivityTags->setChecked(settings.value(this->metaObject()->className()+showActivityTagsState).toBool());
	if(settings.contains(this->metaObject()->className()+showDuplicatesState))
		showDuplicates->setChecked(settings.value(this->metaObject()->className()+showDuplicatesState).toBool());
	if(settings.contains(this->metaObject()->className()+hideEmptyLinesState))
		hideEmptyLines->setChecked(settings.value(this->metaObject()->className()+hideEmptyLinesState).toBool());
	if(settings.contains(this->metaObject()->className()+swapAxesState))
		swapAxes->setChecked(settings.value(this->metaObject()->className()+swapAxesState).toBool());
	if(settings.contains(this->metaObject()->className()+hideFullTeachersState))
		hideUsedTeachers->setChecked(settings.value(this->metaObject()->className()+hideFullTeachersState).toBool());
	
	//connect(activitiesTableView, SIG NAL(cellClicked(int, int)), this, SL OT(activitiesCellSelected(int, int)));
	connect(activitiesTableView, &SparseTableView::activated, this, &ActivityPlanningForm::activitiesCellSelected);
	
	//connect(activitiesTable, SIG NAL(cellDoubleClicked(int, int)), this, SL OT(ActivitiesCellSelected(int, int)));

	//connect(teachersTable, SIG NAL(itemClicked(QTableWidgetItem*)), this, SL OT(teachersCellSelected(QTableWidgetItem*)));
	connect(teachersTableView, &SparseTableView::activated, this, &ActivityPlanningForm::teachersCellSelected);

	//mouseTracking (start 2/3)
	/*
	connect(activitiesTable, SIG NAL(cellEntered(int, int)), this, SL OT(ActivitiesCellEntered(int, int)));
	connect(teachersTable, SIG NAL(cellEntered(int, int)), this, SL OT(TeachersCellEntered(int, int)));
	*/
	//mouseTracking (end 2/3)
	
	connect(activitiesTableView->horizontalHeader(), &QHeaderView::sectionDoubleClicked, this, &ActivityPlanningForm::activitiesTableHorizontalHeaderClicked);
	connect(activitiesTableView->verticalHeader(), &QHeaderView::sectionDoubleClicked, this, &ActivityPlanningForm::activitiesTableVerticalHeaderClicked);

	connect(teachersTableView->horizontalHeader(), &QHeaderView::sectionDoubleClicked, this, &ActivityPlanningForm::teachersTableHorizontalHeaderClicked);
	
	connect(CBActive, qOverload<int>(&QComboBox::currentIndexChanged), this, &ActivityPlanningForm::updateTables);
	connect(showDuplicates, &QCheckBox::toggled, this, &ActivityPlanningForm::updateTablesVisual);
	connect(showYears, &QCheckBox::toggled, this, &ActivityPlanningForm::updateTablesVisual);
	connect(showGroups, &QCheckBox::toggled, this, &ActivityPlanningForm::updateTablesVisual);
	if(SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING)
		connect(showSubgroups, &QCheckBox::toggled, this, &ActivityPlanningForm::updateTablesVisual);
	connect(showTeachers, &QCheckBox::toggled, this, &ActivityPlanningForm::updateTables);
	connect(showActivityTags, &QCheckBox::toggled, this, &ActivityPlanningForm::updateTables);
	connect(hideEmptyLines, &QCheckBox::toggled, this, &ActivityPlanningForm::updateTablesVisual);
	connect(hideUsedTeachers, &QCheckBox::toggled, this, &ActivityPlanningForm::updateTablesVisual);
	connect(swapAxes, &QCheckBox::toggled, this, &ActivityPlanningForm::updateTables);
	connect(pbDeleteAll, &QPushButton::clicked, this, &ActivityPlanningForm::deleteAll);
	connect(pbPseudoActivities, &QPushButton::clicked, this, &ActivityPlanningForm::pseudoActivities);
	//connect(pbHelp, SIG NAL(clicked()), this, SL OT(help()));
	connect(showHideButton, &QToolButton::clicked, this, &ActivityPlanningForm::showHide);
	connect(pbClose, &QPushButton::clicked, this, &ActivityPlanningForm::close);
	
	connect(&planningCommunicationSpinBox, &PlanningCommunicationSpinBox::valueChanged, this, &ActivityPlanningForm::updateTables);

	updateTables();
}

ActivityPlanningForm::~ActivityPlanningForm()
{
	saveFETDialogGeometry(this);

	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), leftSplitter->saveState());

	settings.setValue(this->metaObject()->className()+QString("/buttons-visible"), buttonsVisible);

	//save other settings
	settings.setValue(this->metaObject()->className()+RBActivityState, RBActivity->isChecked());
	settings.setValue(this->metaObject()->className()+RBSubactivityState, RBSubactivity->isChecked());
	settings.setValue(this->metaObject()->className()+RBAddState, RBAdd->isChecked());
	settings.setValue(this->metaObject()->className()+RBModifyState, RBModify->isChecked());
	settings.setValue(this->metaObject()->className()+RBDeleteState, RBDelete->isChecked());
	settings.setValue(this->metaObject()->className()+RBChangeTeacherState, RBChangeTeacher->isChecked());
	settings.setValue(this->metaObject()->className()+RBSwapTeachersState, RBSwapTeachers->isChecked());
	settings.setValue(this->metaObject()->className()+RBSwapStudentsState, RBSwapStudents->isChecked());
	//
	settings.setValue(this->metaObject()->className()+CBActiveState, CBActive->currentIndex());
	//
	settings.setValue(this->metaObject()->className()+showYearsState, showYears->isChecked());
	settings.setValue(this->metaObject()->className()+showGroupsState, showGroups->isChecked());
	if(SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING)
		settings.setValue(this->metaObject()->className()+showSubgroupsState, showSubgroups->isChecked());
	settings.setValue(this->metaObject()->className()+showTeachersState, showTeachers->isChecked());
	settings.setValue(this->metaObject()->className()+showActivityTagsState, showActivityTags->isChecked());
	settings.setValue(this->metaObject()->className()+showDuplicatesState, showDuplicates->isChecked());
	settings.setValue(this->metaObject()->className()+hideEmptyLinesState, hideEmptyLines->isChecked());
	settings.setValue(this->metaObject()->className()+swapAxesState, swapAxes->isChecked());
	settings.setValue(this->metaObject()->className()+hideFullTeachersState, hideUsedTeachers->isChecked());
}

void ActivityPlanningForm::showHide()
{
	if(buttonsVisible){
		pbPseudoActivities->hide();
		pbDeleteAll->hide();
		showHideButton->setText(tr("B", "Abbreviation for 'Buttons'. Please keep translation very short (probably only one letter)."
			" There is an option to hide the buttons in the planning activity dialog, so that the tables can have larger visible parts."
			" This button (B) will remain there after the user hides the other buttons. The user then can press this button to show again all the buttons."
			" We need this 'B' button to be very small, so keep translation abbreviated to the initial only"));
		pbClose->hide();
		
		actionsOptionsTabWidget->hide();

		showHideButton->setSizePolicy(origShowHideSizePolicy);
		
		buttonsVisible=false;
	}
	else{
		pbPseudoActivities->show();
		pbDeleteAll->show();
		showHideButton->setText(tr("Hide buttons", "Please keep translation short. This is an option to hide some buttons in the planning activity dialog, so that the tables are more visible."));
		pbClose->show();

		actionsOptionsTabWidget->show();

		showHideButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

		buttonsVisible=true;
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
	QHash<int, int> numberOfSubactivitiesForRepresentative;
	QSet<int> affectStatusForRepresentativeTeacher; //if in set, it affects
	QSet<int> affectStatusForRepresentativeStudent; //if in set, it affects
	QSet<int> affectStatusForRepresentativeSubject; //if in set, it affects
	for(Activity* act : std::as_const(gt.rules.activitiesList)){
		int id=act->id;
		
		int agid=act->activityGroupId;
		if(agid==0)
			agid=id;
		
		assert(!representantForId.contains(id));
		representantForId.insert(id, agid);
		
		int c=numberOfSubactivitiesForRepresentative.value(agid, 0);
		c++;
		numberOfSubactivitiesForRepresentative.insert(agid, c);
		
		if(teacherName!=""){
			bool af=true;
			if(act->teachersNames.count()==1)
				if(act->teachersNames.at(0)==teacherName)
					af=false;
					
			if( af && !affectStatusForRepresentativeTeacher.contains(agid) )
				affectStatusForRepresentativeTeacher.insert(agid);
		}
		if(studentsSetName!=""){
			bool af=true;
			if(act->studentsNames.count()==1)
				if(act->studentsNames.at(0)==studentsSetName)
					af=false;
					
			if( af && !affectStatusForRepresentativeStudent.contains(agid) )
				affectStatusForRepresentativeStudent.insert(agid);
		}
		if(subjectName!=""){
			bool af=true;
			if(act->subjectName==subjectName)
				af=false;
				
			if( af && !affectStatusForRepresentativeSubject.contains(agid) )
				affectStatusForRepresentativeSubject.insert(agid);
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
			
			assert(numberOfSubactivitiesForRepresentative.contains(repr));
			int n=numberOfSubactivitiesForRepresentative.value(repr);
			assert(n>0);
			nTotalActsDeleted+=n;
			
			_idsToBeRemoved.append(id);
			_agidsToBeRemoved.append(tmpGroupID.at(i));
			
			if(affectStatusForRepresentativeTeacher.contains(repr)){
				_affectOtherTeachersToBeRemoved.append(true);
				_affectOtherTeachersOverall=true;
			}
			else{
				_affectOtherTeachersToBeRemoved.append(false);
			}
			
			if(affectStatusForRepresentativeStudent.contains(repr)){
				_affectOtherStudentsSetsToBeRemoved.append(true);
				_affectOtherStudentsSetsOverall=true;
			}
			else{
				_affectOtherStudentsSetsToBeRemoved.append(false);
			}
			
			if(affectStatusForRepresentativeSubject.contains(repr)){
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

//this is nearly the same code as "computeActivitiesForDeletion".
void ActivityPlanningForm::computeActivitiesForModification(const QString& teacherName, const QString& studentsSetName, const QString& subjectName,
	const QList<int>& tmpID, const QList<int>& tmpGroupID,
	int& nTotalActsModified,
	QList<int>& _idsToBeModified, QList<int>& _agidsToBeModified,
	QList<bool>& _affectOtherTeachersToBeModified, bool& _affectOtherTeachersOverall,
	QList<bool>& _affectOtherStudentsSetsToBeModified, bool& _affectOtherStudentsSetsOverall,
	QList<bool>& _affectOtherSubjectsToBeModified, bool& _affectOtherSubjectsOverall)
{
	QHash<int, int> representantForId;
	QHash<int, int> numberOfSubactivitiesForRepresentative;
	QSet<int> affectStatusForRepresentativeTeacher; //if in set, it affects
	QSet<int> affectStatusForRepresentativeStudent; //if in set, it affects
	QSet<int> affectStatusForRepresentativeSubject; //if in set, it affects
	for(Activity* act : std::as_const(gt.rules.activitiesList)){
		int id=act->id;
		
		int agid=act->activityGroupId;
		if(agid==0)
			agid=id;
		
		assert(!representantForId.contains(id));
		representantForId.insert(id, agid);
		
		int c=numberOfSubactivitiesForRepresentative.value(agid, 0);
		c++;
		numberOfSubactivitiesForRepresentative.insert(agid, c);
		
		if(teacherName!=""){
			bool af=true;
			if(act->teachersNames.count()==1)
				if(act->teachersNames.at(0)==teacherName)
					af=false;
					
			if( af && !affectStatusForRepresentativeTeacher.contains(agid) )
				affectStatusForRepresentativeTeacher.insert(agid);
		}
		if(studentsSetName!=""){
			bool af=true;
			if(act->studentsNames.count()==1)
				if(act->studentsNames.at(0)==studentsSetName)
					af=false;
					
			if( af && !affectStatusForRepresentativeStudent.contains(agid) )
				affectStatusForRepresentativeStudent.insert(agid);
		}
		if(subjectName!=""){
			bool af=true;
			if(act->subjectName==subjectName)
				af=false;
				
			if( af && !affectStatusForRepresentativeSubject.contains(agid) )
				affectStatusForRepresentativeSubject.insert(agid);
		}
	}
	
	QSet<int> representantCounted;

	nTotalActsModified=0;
	
	_idsToBeModified.clear();
	_agidsToBeModified.clear();
	
	_affectOtherTeachersToBeModified.clear();
	_affectOtherTeachersOverall=false;
	
	_affectOtherStudentsSetsToBeModified.clear();
	_affectOtherStudentsSetsOverall=false;
	
	_affectOtherSubjectsToBeModified.clear();
	_affectOtherSubjectsOverall=false;

	assert(tmpID.count()==tmpGroupID.count());
	
	for(int i=0; i<tmpID.count(); i++){
		int id=tmpID.at(i);
		assert(representantForId.contains(id));
		int repr=representantForId.value(id);
		
		if(!representantCounted.contains(repr)){
			representantCounted.insert(repr);
			
			assert(numberOfSubactivitiesForRepresentative.contains(repr));
			int n=numberOfSubactivitiesForRepresentative.value(repr);
			assert(n>0);
			nTotalActsModified+=n;
		}
		
		_idsToBeModified.append(id);
		_agidsToBeModified.append(tmpGroupID.at(i));
		
		if(affectStatusForRepresentativeTeacher.contains(repr)){
			_affectOtherTeachersToBeModified.append(true);
			_affectOtherTeachersOverall=true;
		}
		else{
			_affectOtherTeachersToBeModified.append(false);
		}
		
		if(affectStatusForRepresentativeStudent.contains(repr)){
			_affectOtherStudentsSetsToBeModified.append(true);
			_affectOtherStudentsSetsOverall=true;
		}
		else{
			_affectOtherStudentsSetsToBeModified.append(false);
		}
		
		if(affectStatusForRepresentativeSubject.contains(repr)){
			_affectOtherSubjectsToBeModified.append(true);
			_affectOtherSubjectsOverall=true;
		}
		else{
			_affectOtherSubjectsToBeModified.append(false);
		}
	}

	assert(_idsToBeModified.count()==_agidsToBeModified.count());
	assert(_idsToBeModified.count()==_affectOtherTeachersToBeModified.count());
	assert(_idsToBeModified.count()==_affectOtherStudentsSetsToBeModified.count());
	assert(_idsToBeModified.count()==_affectOtherSubjectsToBeModified.count());
}

void ActivityPlanningForm::swapTeachers(int studentsActivity1, int subjectActivity1, int studentsActivity2, int subjectActivity2){
	//care about set 1
	//bool affectOtherStudents=false;
	QList<int> aiList;
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
			if(act->subjectName==statisticValues.allSubjectsNames[subjectActivity1]){
				for(const QString& st : std::as_const(act->studentsNames)){
					if(st==statisticValues.allStudentsNames[studentsActivity1]){
						//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
						//if(!tmpIdentify.contains(tmpIdent)){
						if(!tmpIdentifySet.contains(act->id)){
							aiList<<ai;
							tmpID<<act->id;
							tmpGroupID<<act->activityGroupId;
							//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
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
	
	//care about set 2
	//bool affectOtherStudents2=false;
	QList<int> aiList2;
	QList<int> tmpID2;
	QList<int> tmpGroupID2;
	//QStringList tmpIdentify2;
	QSet<int> tmpIdentifySet2;
	//QList<bool> tmpAffectOtherStudents2;
	Activity* act2;
	for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
		act2=gt.rules.activitiesList[ai];
		int tmpCurrentIndex=CBActive->currentIndex();
		if( ((act2->active) && (tmpCurrentIndex==ACTIVE_ONLY))
			|| ((!act2->active) && (tmpCurrentIndex==INACTIVE_ONLY))
			|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
			if(act2->subjectName==statisticValues.allSubjectsNames[subjectActivity2]){
				for(const QString& st : std::as_const(act2->studentsNames)){
					if(st==statisticValues.allStudentsNames[studentsActivity2]){
						//QString tmpIdent=CustomFETString::number(act2->id)+" "+CustomFETString::number(act2->activityGroupId);
						//if(!tmpIdentify2.contains(tmpIdent)){
						if(!tmpIdentifySet2.contains(act2->id)){
							aiList2<<ai;
							tmpID2<<act2->id;
							tmpGroupID2<<act2->activityGroupId;
							//tmpIdentify2<<CustomFETString::number(act2->id)+" "+CustomFETString::number(act2->activityGroupId);
							tmpIdentifySet2.insert(act2->id);
							/*if(act2->studentsNames.size()>1){
								tmpAffectOtherStudents2<<true;
								affectOtherStudents2=true;
							} else {
								tmpAffectOtherStudents2<<false;
							}*/
						}
						break;
					}
				}
			}
		}
	}
	assert(tmpID2.count()==tmpIdentifySet2.size());
	assert(tmpGroupID2.count()==tmpIdentifySet2.size());
	//assert(tmpAffectOtherStudents2.count()==tmpIdentifySet2.size());
	
	//check if same teachers
	QSet<QString> teacherSet1;
	QStringList teachersList1;
	if(aiList.count()>0){
		act=gt.rules.activitiesList[aiList.at(0)];
		teachersList1=act->teachersNames;
		for(const QString& t : std::as_const(act->teachersNames)){
			teacherSet1<<t;
		}
	}
	for(int tmp : std::as_const(aiList)){
		act=gt.rules.activitiesList[tmp];
		QSet<QString> tmpSet;
		for(const QString& t : std::as_const(act->teachersNames)){
			tmpSet<<t;
		}
		if(tmpSet!=teacherSet1){
			QMessageBox::warning(this, tr("FET information"),
							 tr("Cannot change teachers, because there are different teachers in %1 %2.", "%1 is a students set name, %2 is a subject name.").arg(statisticValues.allStudentsNames[studentsActivity1]).arg(statisticValues.allSubjectsNames[subjectActivity1]));
			return;
		}
	}
	QSet<QString> teacherSet2;
	QStringList teachersList2;
	if(aiList2.count()>0){
		act2=gt.rules.activitiesList[aiList2.at(0)];
		teachersList2=act2->teachersNames;
		for(const QString& t : std::as_const(act2->teachersNames)){
			teacherSet2<<t;
		}
	}
	for(int tmp : std::as_const(aiList2)){
		act2=gt.rules.activitiesList[tmp];
		QSet<QString> tmpSet;
		for(const QString& t : std::as_const(act2->teachersNames)){
			tmpSet<<t;
		}
		if(tmpSet!=teacherSet2){
			QMessageBox::warning(this, tr("FET information"),
							 tr("Cannot change teachers, because there are different teachers in %1 %2.", "%1 is a students set name, %2 is a subject name.").arg(statisticValues.allStudentsNames[studentsActivity2]).arg(statisticValues.allSubjectsNames[subjectActivity2]));
			return;
		}
	}

	if(tmpIdentifySet.isEmpty() || tmpIdentifySet2.isEmpty()){
		int ret=QMessageBox::question(this, tr("Swap teachers"), tr("One cell is without activities, so swapping the teachers will remove the teachers from the existing activities. Should the teachers be removed?"), QMessageBox::Yes | QMessageBox::No);

		if(ret==QMessageBox::No){
			return;
		}
	}
	
	if(!tmpIdentifySet.isEmpty() || !tmpIdentifySet2.isEmpty()){
		//activities1
		int nTotalActsModified;
		QList<int> _idsToBeModified;
		QList<int> _agidsToBeModified;

		QList<bool> _affectOtherTeachersToBeModified;
		bool _affectOtherTeachersOverall;
		QList<bool> _affectOtherStudentsToBeModified;
		bool _affectOtherStudentsOverall;
		QList<bool> _affectOtherSubjectsToBeModified;
		bool _affectOtherSubjectsOverall;

		computeActivitiesForModification("", statisticValues.allStudentsNames[studentsActivity1], statisticValues.allSubjectsNames[subjectActivity1],
			tmpID, tmpGroupID,
			nTotalActsModified,
			_idsToBeModified, _agidsToBeModified,
			_affectOtherTeachersToBeModified, _affectOtherTeachersOverall,
			_affectOtherStudentsToBeModified, _affectOtherStudentsOverall,
			_affectOtherSubjectsToBeModified, _affectOtherSubjectsOverall);

		/*QString s=tr("Modify %1 activities of %2 %3?", "%1 is a number, %2 is a students set name, %3 is a subject name.").arg(tmpIdentifySet.size()).arg(statisticValues.allStudentsNames[studentsActivity1]).arg(statisticValues.allSubjectsNames[subjectActivity1]);
		assert(nTotalActsModified>=tmpIdentifySet.size());
		if(nTotalActsModified>tmpIdentifySet.size()){
			s+="\n";	//TODO check the following string
			s+=tr("Because you have individually modified the components,\nthere won't be modified an additional number "
				"of %1 activities\n(which are in the same larger split activities as the selected activities).").arg(nTotalActsModified-tmpIdentifySet.size());
		}*/
		
		//activities2
		int nTotalActsModified2;
		QList<int> _idsToBeModified2;
		QList<int> _agidsToBeModified2;

		QList<bool> _affectOtherTeachersToBeModified2;
		bool _affectOtherTeachersOverall2;
		QList<bool> _affectOtherStudentsToBeModified2;
		bool _affectOtherStudentsOverall2;
		QList<bool> _affectOtherSubjectsToBeModified2;
		bool _affectOtherSubjectsOverall2;

		computeActivitiesForModification("", statisticValues.allStudentsNames[studentsActivity2], statisticValues.allSubjectsNames[subjectActivity2],
			tmpID2, tmpGroupID2,
			nTotalActsModified2,
			_idsToBeModified2, _agidsToBeModified2,
			_affectOtherTeachersToBeModified2, _affectOtherTeachersOverall2,
			_affectOtherStudentsToBeModified2, _affectOtherStudentsOverall2,
			_affectOtherSubjectsToBeModified2, _affectOtherSubjectsOverall2);

		/*s+="\n";
		s+=tr("Modify %1 activities of %2 %3?", "%1 is a number, %2 is a students set name, %3 is a subject name.").arg(tmpIdentifySet2.size()).arg(statisticValues.allStudentsNames[studentsActivity2]).arg(statisticValues.allSubjectsNames[subjectActivity2]);
		assert(nTotalActsModified2>=tmpIdentifySet2.size());
		if(nTotalActsModified2>tmpIdentifySet2.size()){
			s+="\n";	//TODO check the following string
			s+=tr("Because you have individually modified the components,\nthere won't be modified an additional number "
				"of %1 activities\n(which are in the same larger split activities as the selected activities).").arg(nTotalActsModified2-tmpIdentifySet2.size());
		}*/
		
		//int ret=QMessageBox::question(this, tr("Modify multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				
		if(true/*ret==QMessageBox::Yes*/){
			/*int ret2=QMessageBox::No;
			if(_affectOtherStudentsOverall){
				ret2=QMessageBox::question(this, tr("Modify related?", "It refers to activities"), tr("Should the related activities also be modified?"),
					QMessageBox::Yes | QMessageBox::No);
			}*/

			for(int i=0; i<_idsToBeModified.count(); i++){
				if(true /*(_affectOtherTeachersToBeModified.at(i)==false) || (ret2==QMessageBox::Yes)*/){
					Activity* act=gt.rules.activitiesPointerHash.value(_idsToBeModified.at(i), nullptr);
					if(act==nullptr){
						assert(0==1);	//TODO: maybe just a warning
						//s+=QCoreApplication::translate("Activity", "Invalid (nonexistent) id for activity");
						break;
					}
					
					QStringList teachers_names=teachersList2;
					QString subject_name=act->subjectName;
					QStringList activity_tags_names=act->activityTagsNames;
					QStringList students_names=act->studentsNames;
					int duration=act->duration;
					bool active=act->active;
					bool computeNTotalStudents=act->computeNTotalStudents;
					int nTotalStudents=act->nTotalStudents;

					gt.rules.modifySubactivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names, students_names, duration, active, computeNTotalStudents, nTotalStudents);
					//gt.rules.modifyActivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names, students_names, duration, active, computeNTotalStudents, nTotalStudents);
				}
			}
			//modify2
			for(int i=0; i<_idsToBeModified2.count(); i++){
				if(true /*(_affectOtherTeachersToBeModified2.at(i)==false) || (ret2==QMessageBox::Yes)*/){
					Activity* act=gt.rules.activitiesPointerHash.value(_idsToBeModified2.at(i), nullptr);
					if(act==nullptr){
						assert(0==1);	//TODO: maybe just a warning
						//s+=QCoreApplication::translate("Activity", "Invalid (nonexistent) id for activity");
						break;
					}
					
					QStringList teachers_names=teachersList1;
					QString subject_name=act->subjectName;
					QStringList activity_tags_names=act->activityTagsNames;
					QStringList students_names=act->studentsNames;
					int duration=act->duration;
					bool active=act->active;
					bool computeNTotalStudents=act->computeNTotalStudents;
					int nTotalStudents=act->nTotalStudents;

					gt.rules.modifySubactivity(_idsToBeModified2.at(i), _agidsToBeModified2.at(i), teachers_names, subject_name, activity_tags_names, students_names, duration, active, computeNTotalStudents, nTotalStudents);
					//gt.rules.modifyActivity(_idsToBeModified2.at(i), _agidsToBeModified2.at(i), teachers_names, subject_name, activity_tags_names, students_names, duration, active, computeNTotalStudents, nTotalStudents);
				}
			}
			gt.rules.addUndoPoint(tr("Activity planning: Swapped the teachers of the activities with students=%1 and subject=%2 with "
			 "the teachers of the activities with students=%3 and subject=%4.")
			 .arg(statisticValues.allStudentsNames.at(studentsActivity1))
			 .arg(gt.rules.subjectsList.at(subjectActivity1)->name)
			 .arg(statisticValues.allStudentsNames.at(studentsActivity2))
			 .arg(gt.rules.subjectsList.at(subjectActivity2)->name));
		}
	}
}

void ActivityPlanningForm::swapStudents(int studentsActivity1, int subjectActivity1, int studentsActivity2, int subjectActivity2){
	//this is very similar to swap teachers. So do the same changes there. Maybe I can even merge both funtions into a single one?
	//care about set 1
	//bool affectOtherStudents=false;
	QList<int> aiList;
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
			if(act->subjectName==statisticValues.allSubjectsNames[subjectActivity1]){
				for(const QString& st : std::as_const(act->studentsNames)){
					if(st==statisticValues.allStudentsNames[studentsActivity1]){
						//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
						//if(!tmpIdentify.contains(tmpIdent)){
						if(!tmpIdentifySet.contains(act->id)){
							aiList<<ai;
							tmpID<<act->id;
							tmpGroupID<<act->activityGroupId;
							//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
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
	
	//care about set 2
	//bool affectOtherStudents2=false;
	QList<int> aiList2;
	QList<int> tmpID2;
	QList<int> tmpGroupID2;
	//QStringList tmpIdentify2;
	QSet<int> tmpIdentifySet2;
	//QList<bool> tmpAffectOtherStudents2;
	Activity* act2;
	for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
		act2=gt.rules.activitiesList[ai];
		int tmpCurrentIndex=CBActive->currentIndex();
		if( ((act2->active) && (tmpCurrentIndex==ACTIVE_ONLY))
			|| ((!act2->active) && (tmpCurrentIndex==INACTIVE_ONLY))
			|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
			if(act2->subjectName==statisticValues.allSubjectsNames[subjectActivity2]){
				for(const QString& st : std::as_const(act2->studentsNames)){
					if(st==statisticValues.allStudentsNames[studentsActivity2]){
						//QString tmpIdent=CustomFETString::number(act2->id)+" "+CustomFETString::number(act2->activityGroupId);
						//if(!tmpIdentify2.contains(tmpIdent)){
						if(!tmpIdentifySet2.contains(act2->id)){
							aiList2<<ai;
							tmpID2<<act2->id;
							tmpGroupID2<<act2->activityGroupId;
							//tmpIdentify2<<CustomFETString::number(act2->id)+" "+CustomFETString::number(act2->activityGroupId);
							tmpIdentifySet2.insert(act2->id);
							/*if(act2->studentsNames.size()>1){
								tmpAffectOtherStudents2<<true;
								affectOtherStudents2=true;
							} else {
								tmpAffectOtherStudents2<<false;
							}*/
						}
						break;
					}
				}
			}
		}
	}
	assert(tmpID2.count()==tmpIdentifySet2.size());
	assert(tmpGroupID2.count()==tmpIdentifySet2.size());
	//assert(tmpAffectOtherStudents2.count()==tmpIdentifySet2.size());
	
	//check if same students
	QSet<QString> studentsSet1;
	QStringList studentsList1;
	if(aiList.count()>0){
		act=gt.rules.activitiesList[aiList.at(0)];
		studentsList1=act->studentsNames;
		for(const QString& s : std::as_const(act->studentsNames)){
			studentsSet1<<s;
		}
	}
	for(int tmp : std::as_const(aiList)){
		act=gt.rules.activitiesList[tmp];
		QSet<QString> tmpSet;
		for(const QString& s : std::as_const(act->studentsNames)){
			tmpSet<<s;
		}
		if(tmpSet!=studentsSet1){
			QMessageBox::warning(this, tr("FET information"),
							 tr("Cannot change students, because there are different students in %1 %2.", "%1 is a students set name. %2 is a subject name.").arg(statisticValues.allStudentsNames[studentsActivity1]).arg(statisticValues.allSubjectsNames[subjectActivity1]));
			return;
		}
	}
	QSet<QString> studentsSet2;
	QStringList studentsList2;
	if(aiList2.count()>0){
		act2=gt.rules.activitiesList[aiList2.at(0)];
		studentsList2=act2->studentsNames;
		for(const QString& s : std::as_const(act2->studentsNames)){
			studentsSet2<<s;
		}
	}
	for(int tmp : std::as_const(aiList2)){
		act2=gt.rules.activitiesList[tmp];
		QSet<QString> tmpSet;
		for(const QString& s : std::as_const(act2->studentsNames)){
			tmpSet<<s;
		}
		if(tmpSet!=studentsSet2){
			QMessageBox::warning(this, tr("FET information"),
							 tr("Cannot change students, because there are different students in %1 %2.", "%1 is a students set name. %2 is a subject name.").arg(statisticValues.allStudentsNames[studentsActivity2]).arg(statisticValues.allSubjectsNames[subjectActivity2]));
			return;
		}
	}
	
	//This will change the students instead of removing them if one cell is empty (start)
	if(tmpIdentifySet.isEmpty() || tmpIdentifySet2.isEmpty()){
		if(tmpIdentifySet.isEmpty()){
			assert(studentsList1.isEmpty());
			studentsList1<<statisticValues.allStudentsNames[studentsActivity1];
		}

		if(tmpIdentifySet2.isEmpty()){
			assert(studentsList2.isEmpty());
			studentsList2<<statisticValues.allStudentsNames[studentsActivity2];
		}
	}
	//(end)
	
	if(!tmpIdentifySet.isEmpty() || !tmpIdentifySet2.isEmpty()){
		//activities1
		int nTotalActsModified;
		QList<int> _idsToBeModified;
		QList<int> _agidsToBeModified;

		QList<bool> _affectOtherTeachersToBeModified;
		bool _affectOtherTeachersOverall;
		QList<bool> _affectOtherStudentsToBeModified;
		bool _affectOtherStudentsOverall;
		QList<bool> _affectOtherSubjectsToBeModified;
		bool _affectOtherSubjectsOverall;

		computeActivitiesForModification("", statisticValues.allStudentsNames[studentsActivity1], statisticValues.allSubjectsNames[subjectActivity1],
			tmpID, tmpGroupID,
			nTotalActsModified,
			_idsToBeModified, _agidsToBeModified,
			_affectOtherTeachersToBeModified, _affectOtherTeachersOverall,
			_affectOtherStudentsToBeModified, _affectOtherStudentsOverall,
			_affectOtherSubjectsToBeModified, _affectOtherSubjectsOverall);

		/*QString s=tr("Modify %1 activities of %2 %3?", "%1 is a number, %2 is a students set name, %3 is a subject name.").arg(tmpIdentifySet.size()).arg(statisticValues.allStudentsNames[studentsActivity1]).arg(statisticValues.allSubjectsNames[subjectActivity1]);
		assert(nTotalActsModified>=tmpIdentifySet.size());
		if(nTotalActsModified>tmpIdentifySet.size()){
			s+="\n";	//TODO check the following string
			s+=tr("Because you have individually modified the components,\nthere won't be modified an additional number "
				"of %1 activities\n(which are in the same larger split activities as the selected activities).").arg(nTotalActsModified-tmpIdentifySet.size());
		}*/
		
		//activities2
		int nTotalActsModified2;
		QList<int> _idsToBeModified2;
		QList<int> _agidsToBeModified2;

		QList<bool> _affectOtherTeachersToBeModified2;
		bool _affectOtherTeachersOverall2;
		QList<bool> _affectOtherStudentsToBeModified2;
		bool _affectOtherStudentsOverall2;
		QList<bool> _affectOtherSubjectsToBeModified2;
		bool _affectOtherSubjectsOverall2;

		computeActivitiesForModification("", statisticValues.allStudentsNames[studentsActivity2], statisticValues.allSubjectsNames[subjectActivity2],
			tmpID2, tmpGroupID2,
			nTotalActsModified2,
			_idsToBeModified2, _agidsToBeModified2,
			_affectOtherTeachersToBeModified2, _affectOtherTeachersOverall2,
			_affectOtherStudentsToBeModified2, _affectOtherStudentsOverall2,
			_affectOtherSubjectsToBeModified2, _affectOtherSubjectsOverall2);

		/*s+="\n";
		s+=tr("Modify %1 activities of %2 %3?", "%1 is a number, %2 is a students set name, %3 is a subject name.").arg(tmpIdentifySet2.size()).arg(statisticValues.allStudentsNames[studentsActivity2]).arg(statisticValues.allSubjectsNames[subjectActivity2]);
		assert(nTotalActsModified2>=tmpIdentifySet2.size());
		if(nTotalActsModified2>tmpIdentifySet2.size()){
			s+="\n";	//TODO check the following string
			s+=tr("Because you have individually modified the components,\nthere won't be modified an additional number "
				"of %1 activities\n(which are in the same larger split activities as the selected activities).").arg(nTotalActsModified2-tmpIdentifySet2.size());
		}*/
		
		//int ret=QMessageBox::question(this, tr("Modify multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				
		if(true/*ret==QMessageBox::Yes*/){
			/*int ret2=QMessageBox::No;
			if(_affectOtherStudentsOverall){
				ret2=QMessageBox::question(this, tr("Modify related?", "It refers to activities"), tr("Should the related activities also be modified?"),
					QMessageBox::Yes | QMessageBox::No);
			}*/

			for(int i=0; i<_idsToBeModified.count(); i++){
				if(true/*(_affectOtherTeachersToBeModified.at(i)==false) || (ret2==QMessageBox::Yes)*/){
					Activity* act=gt.rules.activitiesPointerHash.value(_idsToBeModified.at(i), nullptr);
					if(act==nullptr){
						assert(0==1);	//TODO: maybe just a warning
						//s+=QCoreApplication::translate("Activity", "Invalid (nonexistent) id for activity");
						break;
					}
					QStringList students_names=studentsList2;
					QString subject_name=act->subjectName;		//TODO rethink: maybe it is better to swap also the subject here? I am not sure. But then the name of the function is also wrong
					QStringList activity_tags_names=act->activityTagsNames;
					QStringList teachers_names=act->teachersNames;
					int duration=act->duration;
					bool active=act->active;
					bool computeNTotalStudents=act->computeNTotalStudents;
					int nTotalStudents=act->nTotalStudents;

					gt.rules.modifySubactivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names, students_names, duration, active, computeNTotalStudents, nTotalStudents);
					//gt.rules.modifyActivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names, students_names, duration, active, computeNTotalStudents, nTotalStudents);
				}
			}
			//modify2
			for(int i=0; i<_idsToBeModified2.count(); i++){
				if(true/*(_affectOtherTeachersToBeModified2.at(i)==false) || (ret2==QMessageBox::Yes)*/){
					Activity* act=gt.rules.activitiesPointerHash.value(_idsToBeModified2.at(i), nullptr);
					if(act==nullptr){
						assert(0==1);	//TODO: maybe just a warning
						//s+=QCoreApplication::translate("Activity", "Invalid (nonexistent) id for activity");
						break;
					}
					QStringList students_names=studentsList1;
					QString subject_name=act->subjectName;		//TODO rethink: maybe it is better to swap also the subject here? I am not sure. But then the name of the function is also wrong
					QStringList activity_tags_names=act->activityTagsNames;
					QStringList teachers_names=act->teachersNames;
					int duration=act->duration;
					bool active=act->active;
					bool computeNTotalStudents=act->computeNTotalStudents;
					int nTotalStudents=act->nTotalStudents;

					gt.rules.modifySubactivity(_idsToBeModified2.at(i), _agidsToBeModified2.at(i), teachers_names, subject_name, activity_tags_names, students_names, duration, active, computeNTotalStudents, nTotalStudents);
					//gt.rules.modifyActivity(_idsToBeModified2.at(i), _agidsToBeModified2.at(i), teachers_names, subject_name, activity_tags_names, students_names, duration, active, computeNTotalStudents, nTotalStudents);
				}
			}
			gt.rules.addUndoPoint(tr("Activity planning: Swapped the students of the activities with students=%1 and subject=%2 with "
			 "the students of the activities with the students=%3 and subject=%4.")
			 .arg(statisticValues.allStudentsNames.at(studentsActivity1))
			 .arg(gt.rules.subjectsList.at(subjectActivity1)->name)
			 .arg(statisticValues.allStudentsNames.at(studentsActivity2))
			 .arg(gt.rules.subjectsList.at(subjectActivity2)->name));
		}
	}
}


void ActivityPlanningForm::activitiesTableHorizontalHeaderClicked(int column){
	if(swapAxes->checkState()==Qt::Checked){
		if(column<0 || column>=statisticValues.allStudentsNames.count()){
			return;
		}
	}
	else{
		if(column<0 || column>=statisticValues.allSubjectsNames.count()){
			return;
		}
	}
	if(RBActivity->isChecked()){
		if(swapAxes->checkState()==Qt::Checked){
			ActivitiesForm form(this, "", statisticValues.allStudentsNames[column], "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else {
			ActivitiesForm form(this, "", "", statisticValues.allSubjectsNames[column], "");
			setParentAndOtherThings(&form, this);
			form.exec();
		}
	} else if(RBSubactivity->isChecked()) {
		if(swapAxes->checkState()==Qt::Checked){
			SubactivitiesForm form(this, "", statisticValues.allStudentsNames[column], "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else {
			SubactivitiesForm form(this, "", "", statisticValues.allSubjectsNames[column], "");
			setParentAndOtherThings(&form, this);
			form.exec();
		}
	} else if(RBAdd->isChecked()) {
		if(swapAxes->checkState()==Qt::Checked){
			AddActivityForm addActivityForm(this, "", statisticValues.allStudentsNames[column], "", "");
			setParentAndOtherThings(&addActivityForm, this);
			addActivityForm.exec();
		} else {
			AddActivityForm addActivityForm(this, "", "", statisticValues.allSubjectsNames[column], "");
			setParentAndOtherThings(&addActivityForm, this);
			addActivityForm.exec();
		}
	} else if(RBModify->isChecked()) {
		//Normaly there are too many activities. So just entering the activity form
		if(swapAxes->checkState()==Qt::Checked){
			ActivitiesForm form(this, "", statisticValues.allStudentsNames[column], "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else {
			ActivitiesForm form(this, "", "", statisticValues.allSubjectsNames[column], "");
			setParentAndOtherThings(&form, this);
			form.exec();
		}
	} else if(RBDelete->isChecked()) {
		int students=-1;
		int subject=column;
		if(swapAxes->checkState()==Qt::Checked){
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
				if(subject==-1 || (subject>=0 && act->subjectName==statisticValues.allSubjectsNames[subject])){
					for(const QString& st : std::as_const(act->studentsNames)){
						if(students==-1 || (students>=0 && st==statisticValues.allStudentsNames[students] )){
							//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
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

			if(swapAxes->checkState()==Qt::Checked){
				computeActivitiesForDeletion("", statisticValues.allStudentsNames[column], "",
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				QString s=tr("Delete %1 activities of students set %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allStudentsNames[column]);
				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
				
				int ret=QMessageBox::question(this, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				
				if(ret==QMessageBox::Yes){
					int ret2=QMessageBox::No;
					if(_affectOtherStudentsOverall){
						ret2=QMessageBox::question(this, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other students sets. Should the related activities also be deleted?"),
							QMessageBox::Yes | QMessageBox::No);
					}
				
					for(int i=0; i<_idsToBeRemoved.count(); i++){
						if((_affectOtherStudentsToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
							gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
						}
					}
					gt.rules.addUndoPoint(tr("Activity planning: Deleted %1 activities of the students set %2.", "%1 is the number of activities")
					 .arg(tmpIdentifySet.size())
					 .arg(statisticValues.allStudentsNames[column]));
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
			else{
				computeActivitiesForDeletion("", "", statisticValues.allSubjectsNames[column],
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				QString s=tr("Delete %1 activities of subject %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allSubjectsNames[column]);
					//+" ("+tr("these are all the activities for this subject, possibly including activities for this subject which are not visible in the current view")+").";
				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
					
				int ret=QMessageBox::question(this, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				
				if(ret==QMessageBox::Yes){
					int ret2=QMessageBox::No;
					if(_affectOtherSubjectsOverall){
						ret2=QMessageBox::question(this, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other subjects. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
					}
					
					for(int i=0; i<_idsToBeRemoved.count(); i++){
						if((_affectOtherSubjectsToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
							gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
						}
					}
					gt.rules.addUndoPoint(tr("Activity planning: Deleted %1 activities of the subject %2.", "%1 is the number of activities")
					 .arg(tmpIdentifySet.size())
					 .arg(statisticValues.allSubjectsNames[column]));
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
		}
	} else if(RBChangeTeacher->isChecked()) {
		//following source is nearly the same code as if(RBDelete->isChecked()). So do similar changes there too
		int students=-1;
		int subject=column;
		if(swapAxes->checkState()==Qt::Checked){
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
				if(subject==-1 || (subject>=0 && act->subjectName==statisticValues.allSubjectsNames[subject])){
					for(const QString& st : std::as_const(act->studentsNames)){
						if(students==-1 || (students>=0 && st==statisticValues.allStudentsNames[students] )){
							//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
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
			int nTotalActsModified;
			QList<int> _idsToBeModified;
			QList<int> _agidsToBeModified;

			QList<bool> _affectOtherTeachersToBeModified;
			bool _affectOtherTeachersOverall;
			QList<bool> _affectOtherStudentsToBeModified;
			bool _affectOtherStudentsOverall;
			QList<bool> _affectOtherSubjectsToBeModified;
			bool _affectOtherSubjectsOverall;

			if(swapAxes->checkState()==Qt::Checked){
				computeActivitiesForModification("", statisticValues.allStudentsNames[column], "",
					tmpID, tmpGroupID,
					nTotalActsModified,
					_idsToBeModified, _agidsToBeModified,
					_affectOtherTeachersToBeModified, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeModified, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeModified, _affectOtherSubjectsOverall);

				QString s=tr("Modify %1 activities of students set %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allStudentsNames[column]);
				assert(nTotalActsModified>=tmpIdentifySet.size());
				if(nTotalActsModified>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components,\nthere won't be modified an additional number "
						"of %1 activities\n(which are in the same larger split activities as the selected activities).").arg(nTotalActsModified-tmpIdentifySet.size());
				}
				
				bool ok;
				QStringList items;
				//items=subjectListWithQualifiedTeachers.at(subject);		//subject is -1 here
				
				//if(items.count()<1){
				//	QMessageBox::information(this, tr("FET - Information"), tr("There is no teacher qualified for this subject. You should set teachers qualified subjects in FET -> Data -> Teachers."));
					items=statisticValues.allTeachersNames;
				//}
				s+="\n\n";
				s+=tr("Please choose a new teacher:");
				QString item = QInputDialog::getItem(this, tr("FET - Select new teacher dialog"), s, items, 0, false, &ok);
				if (ok) {
					int ret2=QMessageBox::Yes;
					//int ret2=QMessageBox::No;
					//if(_affectOtherSubjectsOverall){
					//	ret2=QMessageBox::question(this, tr("Modify related?", "It refers to activities"), tr("There are activities affecting other subjects. Should the related activities also be modified?"), QMessageBox::Yes | QMessageBox::No);
					//}
					
					for(int i=0; i<_idsToBeModified.count(); i++){
						if((_affectOtherSubjectsToBeModified.at(i)==false) || (ret2==QMessageBox::Yes)){
							Activity* act=gt.rules.activitiesPointerHash.value(_idsToBeModified.at(i), nullptr);
							if(act==nullptr){
								assert(0==1);	//TODO: maybe just a warning
								//s+=QCoreApplication::translate("Activity", "Invalid (nonexistent) id for activity");
								break;
							}
							
							//QStringList teachers_names=act->teachersNames;
							QStringList teachers_names;
							teachers_names<<item;
							QString subject_name=act->subjectName;
							QStringList activity_tags_names=act->activityTagsNames;
							QStringList students_names=act->studentsNames;
							int duration=act->duration;
							bool active=act->active;
							bool computeNTotalStudents=act->computeNTotalStudents;
							int nTotalStudents=act->nTotalStudents;
		
							gt.rules.modifySubactivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
							//gt.rules.modifyActivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
						}
					}
					gt.rules.addUndoPoint(tr("Activity planning: Modified %1 activities of the students set %2. The new teacher is %3.", "%1 is the number of activities")
					 .arg(tmpIdentifySet.size())
					 .arg(statisticValues.allStudentsNames[column])
					 .arg(item));
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
			else{
				computeActivitiesForModification("", "", statisticValues.allSubjectsNames[column],
					tmpID, tmpGroupID,
					nTotalActsModified,
					_idsToBeModified, _agidsToBeModified,
					_affectOtherTeachersToBeModified, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeModified, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeModified, _affectOtherSubjectsOverall);

				QString s=tr("Modify %1 activities of subject %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allSubjectsNames[column]);
					//+" ("+tr("these are all the activities for this subject, possibly including activities for this subject which are not visible in the current view")+").";
				assert(nTotalActsModified>=tmpIdentifySet.size());
				if(nTotalActsModified>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components,\nthere won't be modified an additional number "
						"of %1 activities\n(which are in the same larger split activities as the selected activities).").arg(nTotalActsModified-tmpIdentifySet.size());
				}
					
				bool ok;
				QStringList items;
				items=subjectListWithQualifiedTeachers.at(subject);
				
				if(items.count()<1){
					QMessageBox::information(this, tr("FET - Information"), tr("There is no teacher qualified for this subject. You should set teachers qualified subjects in FET -> Data -> Teachers."));
					items=statisticValues.allTeachersNames;
				}
				s+="\n\n";
				s+=tr("Please choose a new teacher:");
				QString item = QInputDialog::getItem(this, tr("FET - Select new teacher dialog"), s, items, 0, false, &ok);
				if (ok) {
					int ret2=QMessageBox::Yes;
					//int ret2=QMessageBox::No;
					//if(_affectOtherSubjectsOverall){
					//	ret2=QMessageBox::question(this, tr("Modify related?", "It refers to activities"), tr("There are activities affecting other subjects. Should the related activities also be modified?"), QMessageBox::Yes | QMessageBox::No);
					//}
					
					for(int i=0; i<_idsToBeModified.count(); i++){
						if((_affectOtherSubjectsToBeModified.at(i)==false) || (ret2==QMessageBox::Yes)){
							Activity* act=gt.rules.activitiesPointerHash.value(_idsToBeModified.at(i), nullptr);
							if(act==nullptr){
								assert(0==1);	//TODO: maybe just a warning
								//s+=QCoreApplication::translate("Activity", "Invalid (nonexistent) id for activity");
								break;
							}
							
							//QStringList teachers_names=act->teachersNames;
							QStringList teachers_names;
							teachers_names<<item;
							QString subject_name=act->subjectName;
							QStringList activity_tags_names=act->activityTagsNames;
							QStringList students_names=act->studentsNames;
							int duration=act->duration;
							bool active=act->active;
							bool computeNTotalStudents=act->computeNTotalStudents;
							int nTotalStudents=act->nTotalStudents;
		
							gt.rules.modifySubactivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
							//gt.rules.modifyActivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
						}
					}
					gt.rules.addUndoPoint(tr("Activity planning: Modified %1 activities of the subject %2. The new teacher is %3.", "%1 is the number of activities")
					 .arg(tmpIdentifySet.size())
					 .arg(statisticValues.allSubjectsNames[column])
					 .arg(item));
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
		}
	} else if(RBSwapTeachers->isChecked()) {
		if(swapAxes->checkState()==Qt::Checked){
			bool ok;
			QStringList items=statisticValues.allStudentsNames;
			QString s=tr("You have selected %1. Please select other students to swap teachers.", "%1 is a students set name.").arg(statisticValues.allStudentsNames[column]);
			QString item = QInputDialog::getItem(this, tr("FET - Swap teachers"), s, items, 0, false, &ok);
			if(ok){
				for(int i=0; i<statisticValues.allSubjectsNames.count(); i++){
					swapTeachers(column, i, statisticValues.allStudentsNames.indexOf(item), i);
				}
				PlanningChanged::increasePlanningCommunicationSpinBox();
			}
		} else {
			bool ok;
			QStringList items=statisticValues.allSubjectsNames;
			QString s=tr("You have selected %1. Please select other subject to swap teachers.", "%1 is a subject name.").arg(statisticValues.allSubjectsNames[column]);
			QString item = QInputDialog::getItem(this, tr("FET - Swap teachers"), s, items, 0, false, &ok);
			if(ok){
				for(int i=0; i<statisticValues.allStudentsNames.count(); i++){
					swapTeachers(i, column, i, statisticValues.allSubjectsNames.indexOf(item));
				}
				PlanningChanged::increasePlanningCommunicationSpinBox();
			}
		}
	} else if(RBSwapStudents->isChecked()) {
		if(swapAxes->checkState()==Qt::Checked){
			bool ok;
			QStringList items=statisticValues.allStudentsNames;
			QString s=tr("You have selected %1. Please select other students to swap students.", "%1 is a students set name.").arg(statisticValues.allStudentsNames[column]);
			QString item = QInputDialog::getItem(this, tr("FET - Swap students"), s, items, 0, false, &ok);
			if(ok){
				for(int i=0; i<statisticValues.allSubjectsNames.count(); i++){
					swapStudents(column, i, statisticValues.allStudentsNames.indexOf(item), i);
				}
				PlanningChanged::increasePlanningCommunicationSpinBox();
			}
		} else {
			//no function. following code is unneeded, since the students are always the same; so no need to swap
			/*bool ok;
			QStringList items=statisticValues.allSubjectsNames;
			QString s=tr("You have selected %1. Please select other subject to swap students.", "%1 is a subject set name.").arg(statisticValues.allSubjectsNames[column]);
			QString item = QInputDialog::getItem(this, tr("FET - Swap students"), s, items, 0, false, &ok);
			if(ok){
				for(int i=0; i<statisticValues.allStudentsNames.count(); i++){
					swapStudents(i, column, i, statisticValues.allSubjectsNames.indexOf(item));
				}
				PlanningChanged::increasePlanningCommunicationSpinBox();
			}*/
		}
	} else assert(0==1);
}

void ActivityPlanningForm::activitiesTableVerticalHeaderClicked(int row){
	if(swapAxes->checkState()==Qt::Checked){
		if(row<0 || row>=statisticValues.allSubjectsNames.count()){
			return;
		}
	}
	else{
		if(row<0 || row>=statisticValues.allStudentsNames.count()){
			return;
		}
	}
	if(RBActivity->isChecked()){
		if(swapAxes->checkState()==Qt::Checked){
			ActivitiesForm form(this, "", "", statisticValues.allSubjectsNames[row], "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else {
			ActivitiesForm form(this, "", statisticValues.allStudentsNames[row], "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		}
	} else if(RBSubactivity->isChecked()) {
		if(swapAxes->checkState()==Qt::Checked){
			SubactivitiesForm form(this, "", "", statisticValues.allSubjectsNames[row], "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else {
			SubactivitiesForm form(this, "", statisticValues.allStudentsNames[row], "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		}
	} else if(RBAdd->isChecked()) {
		if(swapAxes->checkState()==Qt::Checked){
			AddActivityForm addActivityForm(this, "", "", statisticValues.allSubjectsNames[row], "");
			setParentAndOtherThings(&addActivityForm, this);
			addActivityForm.exec();
		} else {
			AddActivityForm addActivityForm(this, "", statisticValues.allStudentsNames[row], "", "");
			setParentAndOtherThings(&addActivityForm, this);
			addActivityForm.exec();	
		}
	} else if(RBModify->isChecked()) {
		//Normaly there are too many activities. So just entering the activity form
		if(swapAxes->checkState()==Qt::Checked){
			ActivitiesForm form(this, "", "", statisticValues.allSubjectsNames[row], "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else {
			ActivitiesForm form(this, "", statisticValues.allStudentsNames[row], "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		}
	} else if(RBDelete->isChecked()) {
		int students=row;
		int subject=-1;
		if(swapAxes->checkState()==Qt::Checked){
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
				if(subject==-1 || (subject>=0 && act->subjectName==statisticValues.allSubjectsNames[subject])){
					for(const QString& st : std::as_const(act->studentsNames)){
						if(students==-1 || (students>=0 && st==statisticValues.allStudentsNames[students] )){
							//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
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

			if(swapAxes->checkState()==Qt::Checked){
				computeActivitiesForDeletion("", "", statisticValues.allSubjectsNames[row],
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				QString s=tr("Delete %1 activities of subject %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allSubjectsNames[row]);
					//+" ("+tr("these are all the activities for this subject, possibly including activities for this subject which are not visible in the current view")+").";
				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
					
				int ret=QMessageBox::question(this, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				
				if(ret==QMessageBox::Yes){
					int ret2=QMessageBox::No;
					if(_affectOtherSubjectsOverall){
						ret2=QMessageBox::question(this, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other subjects. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
					}
					
					for(int i=0; i<_idsToBeRemoved.count(); i++){
						if((_affectOtherSubjectsToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
							gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
						}
					}
					gt.rules.addUndoPoint(tr("Activity planning: Deleted %1 activities of the subject %2.", "%1 is the number of activities")
					 .arg(tmpIdentifySet.size())
					 .arg(statisticValues.allSubjectsNames[row]));
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
			else{
				computeActivitiesForDeletion("", statisticValues.allStudentsNames[row], "",
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				QString s=tr("Delete %1 activities of students set %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allStudentsNames[row]);
				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
				
				int ret=QMessageBox::question(this, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				
				if(ret==QMessageBox::Yes){
					int ret2=QMessageBox::No;
					if(_affectOtherStudentsOverall){
						ret2=QMessageBox::question(this, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other students sets. Should the related activities also be deleted?"),
							QMessageBox::Yes | QMessageBox::No);
					}
				
					for(int i=0; i<_idsToBeRemoved.count(); i++){
						if((_affectOtherStudentsToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
							gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
						}
					}
					gt.rules.addUndoPoint(tr("Activity planning: Deleted %1 activities of the students set %2.", "%1 is the number of activities")
					 .arg(tmpIdentifySet.size())
					 .arg(statisticValues.allStudentsNames[row]));
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
		}
	} else if(RBChangeTeacher->isChecked()) {
		//following source is nearly the same code as if(RBDelete->isChecked()). So do similar changes there too.
		int students=row;
		int subject=-1;
		if(swapAxes->checkState()==Qt::Checked){
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
				if(subject==-1 || (subject>=0 && act->subjectName==statisticValues.allSubjectsNames[subject])){
					for(const QString& st : std::as_const(act->studentsNames)){
						if(students==-1 || (students>=0 && st==statisticValues.allStudentsNames[students] )){
							//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
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
			int nTotalActsModified;
			QList<int> _idsToBeModified;
			QList<int> _agidsToBeModified;

			QList<bool> _affectOtherTeachersToBeModified;
			bool _affectOtherTeachersOverall;
			QList<bool> _affectOtherStudentsToBeModified;
			bool _affectOtherStudentsOverall;
			QList<bool> _affectOtherSubjectsToBeModified;
			bool _affectOtherSubjectsOverall;

			if(swapAxes->checkState()==Qt::Checked){
				computeActivitiesForModification("", "", statisticValues.allSubjectsNames[row],
					tmpID, tmpGroupID,
					nTotalActsModified,
					_idsToBeModified, _agidsToBeModified,
					_affectOtherTeachersToBeModified, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeModified, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeModified, _affectOtherSubjectsOverall);

				QString s=tr("Modify %1 activities of subject %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allSubjectsNames[row]);
					//+" ("+tr("these are all the activities for this subject, possibly including activities for this subject which are not visible in the current view")+").";
				assert(nTotalActsModified>=tmpIdentifySet.size());
				if(nTotalActsModified>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components,\nthere won't be removed an additional number "
						"of %1 activities\n(which are in the same larger split activities as the selected activities).").arg(nTotalActsModified-tmpIdentifySet.size());
				}
					
				bool ok;
				QStringList items;
				items=subjectListWithQualifiedTeachers.at(subject);
				
				if(items.count()<1){
					QMessageBox::information(this, tr("FET - Information"), tr("There is no teacher qualified for this subject. You should set teachers qualified subjects in FET -> Data -> Teachers."));
					items=statisticValues.allTeachersNames;
				}
				s+="\n\n";
				s+=tr("Please choose a new teacher:");
				QString item = QInputDialog::getItem(this, tr("FET - Select new teacher dialog"), s, items, 0, false, &ok);
				if (ok) {
					int ret2=QMessageBox::Yes;
					//int ret2=QMessageBox::No;
					//if(_affectOtherSubjectsOverall){
					//	ret2=QMessageBox::question(this, tr("Modify related?", "It refers to activities"), tr("There are activities affecting other subjects. Should the related activities also be modified?"), QMessageBox::Yes | QMessageBox::No);
					//}
					
					for(int i=0; i<_idsToBeModified.count(); i++){
						if((_affectOtherSubjectsToBeModified.at(i)==false) || (ret2==QMessageBox::Yes)){
							Activity* act=gt.rules.activitiesPointerHash.value(_idsToBeModified.at(i), nullptr);
							if(act==nullptr){
								assert(0==1);	//TODO: maybe just a warning
								//s+=QCoreApplication::translate("Activity", "Invalid (nonexistent) id for activity");
								break;
							}
							
							//QStringList teachers_names=act->teachersNames;
							QStringList teachers_names;
							teachers_names<<item;
							QString subject_name=act->subjectName;
							QStringList activity_tags_names=act->activityTagsNames;
							QStringList students_names=act->studentsNames;
							int duration=act->duration;
							bool active=act->active;
							bool computeNTotalStudents=act->computeNTotalStudents;
							int nTotalStudents=act->nTotalStudents;
		
							gt.rules.modifySubactivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
							//gt.rules.modifyActivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
						}
					}
					gt.rules.addUndoPoint(tr("Activity planning: Modified %1 activities of the subject %2. The new teacher is %3.", "%1 is the number of activities")
					 .arg(tmpIdentifySet.size())
					 .arg(statisticValues.allSubjectsNames[row]).arg(item));
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
			else{
				computeActivitiesForModification("", statisticValues.allStudentsNames[row], "",
					tmpID, tmpGroupID,
					nTotalActsModified,
					_idsToBeModified, _agidsToBeModified,
					_affectOtherTeachersToBeModified, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeModified, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeModified, _affectOtherSubjectsOverall);

				QString s=tr("Modify %1 activities of students set %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allStudentsNames[row]);
				assert(nTotalActsModified>=tmpIdentifySet.size());
				if(nTotalActsModified>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components,\nthere won't be modified an additional number "
						"of %1 activities\n(which are in the same larger split activities as the selected activities).").arg(nTotalActsModified-tmpIdentifySet.size());
				}
				
				bool ok;
				QStringList items;
				//items=subjectListWithQualifiedTeachers.at(subject);		//subject is -1 here!
				
				//if(items.count()<1){
				//	QMessageBox::information(this, tr("FET - Information"), tr("There is no teacher qualified for this subject. You should set teachers qualified subjects in FET -> Data -> Teachers."));
					items=statisticValues.allTeachersNames;
				//}
				s+="\n\n";
				s+=tr("Please choose a new teacher:");
				QString item = QInputDialog::getItem(this, tr("FET - Select new teacher dialog"), s, items, 0, false, &ok);
				if (ok) {
					int ret2=QMessageBox::No;
					if(_affectOtherStudentsOverall){
						ret2=QMessageBox::question(this, tr("Modify related?", "It refers to activities"), tr("There are activities affecting other students sets. Should the related activities also be modified?"),
							QMessageBox::Yes | QMessageBox::No);
					}
				
					for(int i=0; i<_idsToBeModified.count(); i++){
						if((_affectOtherStudentsToBeModified.at(i)==false) || (ret2==QMessageBox::Yes)){
							Activity* act=gt.rules.activitiesPointerHash.value(_idsToBeModified.at(i), nullptr);
							if(act==nullptr){
								assert(0==1);	//TODO: maybe just a warning
								//s+=QCoreApplication::translate("Activity", "Invalid (nonexistent) id for activity");
								break;
							}
							
							//QStringList teachers_names=act->teachersNames;
							QStringList teachers_names;
							teachers_names<<item;
							QString subject_name=act->subjectName;
							QStringList activity_tags_names=act->activityTagsNames;
							QStringList students_names=act->studentsNames;
							int duration=act->duration;
							bool active=act->active;
							bool computeNTotalStudents=act->computeNTotalStudents;
							int nTotalStudents=act->nTotalStudents;
		
							gt.rules.modifySubactivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
							//gt.rules.modifyActivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
						}
					}
					gt.rules.addUndoPoint(tr("Activity planning: Modified %1 activities of the students set %2. The new teacher is %3", "%1 is the number of activities")
					 .arg(tmpIdentifySet.size())
					 .arg(statisticValues.allStudentsNames[row]).arg(item));
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
		}
	} else if(RBSwapTeachers->isChecked()) {
		if(swapAxes->checkState()==Qt::Checked){
			bool ok;
			QStringList items=statisticValues.allSubjectsNames;
			QString s=tr("You have selected %1. Please select other subject to swap teachers.", "%1 is a subject name.").arg(statisticValues.allSubjectsNames[row]);
			QString item = QInputDialog::getItem(this, tr("FET - Swap teachers"), s, items, 0, false, &ok);
			if(ok){
				for(int i=0; i<statisticValues.allStudentsNames.count(); i++){
					swapTeachers(i, row, i, statisticValues.allSubjectsNames.indexOf(item));
				}
				PlanningChanged::increasePlanningCommunicationSpinBox();
			}
		} else {
			bool ok;
			QStringList items=statisticValues.allStudentsNames;
			QString s=tr("You have selected %1. Please select other students to swap teachers.", "%1 is a students set name.").arg(statisticValues.allStudentsNames[row]);
			QString item = QInputDialog::getItem(this, tr("FET - Swap teachers"), s, items, 0, false, &ok);
			if(ok){
				for(int i=0; i<statisticValues.allSubjectsNames.count(); i++){
					swapTeachers(row, i, statisticValues.allStudentsNames.indexOf(item), i);
				}
				PlanningChanged::increasePlanningCommunicationSpinBox();
			}
		}
	} else if(RBSwapStudents->isChecked()) {
		if(swapAxes->checkState()==Qt::Checked){
			//no function. following code is unneeded, since the students are always the same; so no need to swap
			/*bool ok;
			QStringList items=statisticValues.allSubjectsNames;
			QString s=tr("You have selected %1. Please select other subject to swap students.", "%1 is a subject name.").arg(statisticValues.allSubjectsNames[row]);
			QString item = QInputDialog::getItem(this, tr("FET - Swap students"), s, items, 0, false, &ok);
			if(ok){
				for(int i=0; i<statisticValues.allStudentsNames.count(); i++){
					swapStudents(i, row, i, statisticValues.allSubjectsNames.indexOf(item));
				}
				PlanningChanged::increasePlanningCommunicationSpinBox();
			}*/
		} else {
			bool ok;
			QStringList items=statisticValues.allStudentsNames;
			QString s=tr("You have selected %1. Please select other students to swap students.", "%1 is a students set name.").arg(statisticValues.allStudentsNames[row]);
			QString item = QInputDialog::getItem(this, tr("FET - Swap students"), s, items, 0, false, &ok);
			if(ok){
				for(int i=0; i<statisticValues.allSubjectsNames.count(); i++){
					swapStudents(row, i, statisticValues.allStudentsNames.indexOf(item), i);
				}
				PlanningChanged::increasePlanningCommunicationSpinBox();
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
	if(swapAxes->checkState()==Qt::Checked){
		students=column;
		subject=row;
	}
	if(RBActivity->isChecked()){
		ActivitiesForm form(this, "", statisticValues.allStudentsNames[students], statisticValues.allSubjectsNames[subject], "");
		setParentAndOtherThings(&form, this);
		form.exec();
	} else if(RBSubactivity->isChecked()) {
		SubactivitiesForm form(this, "", statisticValues.allStudentsNames[students], statisticValues.allSubjectsNames[subject], "");
		setParentAndOtherThings(&form, this);
		form.exec();
	} else if(RBAdd->isChecked()) {
		AddActivityForm addActivityForm(this, "", statisticValues.allStudentsNames[students], statisticValues.allSubjectsNames[subject], "");
		setParentAndOtherThings(&addActivityForm, this);
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
				if(act->subjectName==statisticValues.allSubjectsNames[subject]){
					for(const QString& st : std::as_const(act->studentsNames)){
						if(st==statisticValues.allStudentsNames[students]){
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
			for(int k : std::as_const(tmpAGIds)){
				if(agid==-1)
					agid=k;
				else if(agid!=k){
					sameAgid=false;
					break;
				}
			}
			
			if(!sameAgid){
				ActivitiesForm form(this, "", statisticValues.allStudentsNames[students], statisticValues.allSubjectsNames[subject], "");
				setParentAndOtherThings(&form, this);
				form.exec();
				
				return;
			}
			
			if(tmpActivities.count()==0){ //maybe representative is inactive
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
			
			int tmpAct=tmpActivities.at(0);
			//for(int tmpAct : std::as_const(tmpActivities)){
				Activity* act=gt.rules.activitiesList[tmpAct];
				if(act->isSplit()){
					//similarly to activitiesform.cpp by Liviu Lalescu (start)
					//maybe TODO: write a function 'bool activityChangedManually(...)' in activity.cpp, because we use this already 3 times:
					//            here, in activitiesform.cpp, and in the CSV export.
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
							 tr("Cannot modify this large activity, because it contains more than %1 activities.")
							 .arg(MAX_SPLIT_OF_AN_ACTIVITY));
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
							s.append(tr("different Boolean variable 'must compute n total students'"));
						if(diffNTotalStudents)
							s.append(tr("different number of students"));
							
						QString s2;
						s2+=tr("The current split activity has subactivities which were individually modified. It is recommended to abort now"
						 " and modify individual subactivities from the corresponding menu. Otherwise you will modify the fields for all the subactivities"
						 " from this larger split activity.");
						s2+="\n\n";
						s2+=tr("The fields which are different are: %1").arg(s.join(translatedCommaSpace()));
							
						int t=QMessageBox::warning(this, tr("FET warning"), s2, tr("Abort"), tr("Continue"), QString(), 1, 0);
						
						if(t==0)
							return;*/
							
						ActivitiesForm form(this, "", statisticValues.allStudentsNames[students], statisticValues.allSubjectsNames[subject], "");
						setParentAndOtherThings(&form, this);
						form.exec();
				
						return;
					}
				}
				ModifyActivityForm modifyActivityForm(this, act->id, act->activityGroupId);
				//int t;
				setParentAndOtherThings(&modifyActivityForm, this);
				modifyActivityForm.exec();
				//similar to activitiesform.cpp (end)
				return;
			//}
		}
		//else if(tmpSubactivities.count()>=1){
		else{
			ActivitiesForm form(this, "", statisticValues.allStudentsNames[students], statisticValues.allSubjectsNames[subject], "");
			setParentAndOtherThings(&form, this);
			form.exec();
		}
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
				if(act->subjectName==statisticValues.allSubjectsNames[subject]){
					for(const QString& st : std::as_const(act->studentsNames)){
						if(st==statisticValues.allStudentsNames[students]){
							//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
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
			QString undoSubject;
			QString undoStudents;
			if(swapAxes->checkState()==Qt::Checked){
				computeActivitiesForDeletion("", statisticValues.allStudentsNames[column], statisticValues.allSubjectsNames[row],
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				s=tr("Delete %1 activities from selected cell?").arg(tmpIdentifySet.size());
				s+=" (";
				s+=tr("subject=%1").arg(statisticValues.allSubjectsNames[row]);
				s+=translatedCommaSpace();
				s+=tr("students=%1").arg(statisticValues.allStudentsNames[column]);
				s+=")";
				undoSubject=statisticValues.allSubjectsNames[row];
				undoStudents=statisticValues.allStudentsNames[column];

				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
			}
			else{
				computeActivitiesForDeletion("", statisticValues.allStudentsNames[row], statisticValues.allSubjectsNames[column],
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				s=tr("Delete %1 activities from selected cell?").arg(tmpIdentifySet.size());
				s+=" (";
				s+=tr("students=%1").arg(statisticValues.allStudentsNames[row]);
				s+=translatedCommaSpace();
				s+=tr("subject=%1").arg(statisticValues.allSubjectsNames[column]);
				s+=")";
				undoSubject=statisticValues.allSubjectsNames[column];
				undoStudents=statisticValues.allStudentsNames[row];

				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
			}

			int ret=QMessageBox::question(this, tr("Delete activities?"), s, QMessageBox::Yes | QMessageBox::No);
			if(ret==QMessageBox::Yes){
				int retst=QMessageBox::No;
				if(_affectOtherStudentsOverall){
					retst=QMessageBox::question(this, tr("Delete related?", "It refers to activities"),
						tr("This cell contains activities that relate to other students. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
				}

				int retsubj=QMessageBox::No;
				if(_affectOtherSubjectsOverall){
					retsubj=QMessageBox::question(this, tr("Delete related?", "It refers to activities"),
						tr("This cell contains activities that relate to other subjects. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
				}

				for(int i=0; i<_idsToBeRemoved.count(); i++){
					if( ((_affectOtherStudentsToBeRemoved.at(i)==false) || (retst==QMessageBox::Yes)) &&
						((_affectOtherSubjectsToBeRemoved.at(i)==false) || (retsubj==QMessageBox::Yes)) ){
						gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
					}
				}
				gt.rules.addUndoPoint(tr("Activity planning: Deleted the activities with the subject %1 and the students %2.")
				 .arg(undoSubject)
				 .arg(undoStudents));
				PlanningChanged::increasePlanningCommunicationSpinBox();
			}
		}
	} else if(RBChangeTeacher->isChecked()) {
		//following source is nearly the same code as if(RBDelete->isChecked()). So do similar changes there too.
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
				if(act->subjectName==statisticValues.allSubjectsNames[subject]){
					for(const QString& st : std::as_const(act->studentsNames)){
						if(st==statisticValues.allStudentsNames[students]){
							//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
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
			int nTotalActsModified;
			QList<int> _idsToBeModified;
			QList<int> _agidsToBeModified;

			QList<bool> _affectOtherTeachersToBeModified;
			bool _affectOtherTeachersOverall;
			QList<bool> _affectOtherStudentsToBeModified;
			bool _affectOtherStudentsOverall;
			QList<bool> _affectOtherSubjectsToBeModified;
			bool _affectOtherSubjectsOverall;

			QString s;
			QString undoSubject;
			QString undoStudents;
			if(swapAxes->checkState()==Qt::Checked){
				computeActivitiesForModification("", statisticValues.allStudentsNames[column], statisticValues.allSubjectsNames[row],
					tmpID, tmpGroupID,
					nTotalActsModified,
					_idsToBeModified, _agidsToBeModified,
					_affectOtherTeachersToBeModified, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeModified, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeModified, _affectOtherSubjectsOverall);

				s=tr("Modify %1 activities from selected cell?").arg(tmpIdentifySet.size());
				s+="\n(";
				s+=tr("subject=%1").arg(statisticValues.allSubjectsNames[row]);
				s+=translatedComma()+"\n";
				s+=tr("students=%1").arg(statisticValues.allStudentsNames[column]);
				s+=")";
				undoSubject=statisticValues.allSubjectsNames[row];
				undoStudents=statisticValues.allStudentsNames[column];

				assert(nTotalActsModified>=tmpIdentifySet.size());
				if(nTotalActsModified>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components,\nthere won't be modified an additional number "
						"of %1 activities\n(which are in the same larger split activities as the selected activities).").arg(nTotalActsModified-tmpIdentifySet.size());
				}
			}
			else{
				computeActivitiesForModification("", statisticValues.allStudentsNames[row], statisticValues.allSubjectsNames[column],
					tmpID, tmpGroupID,
					nTotalActsModified,
					_idsToBeModified, _agidsToBeModified,
					_affectOtherTeachersToBeModified, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeModified, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeModified, _affectOtherSubjectsOverall);

				s=tr("Modify %1 activities from selected cell?").arg(tmpIdentifySet.size());
				s+="\n(";
				s+=tr("students=%1").arg(statisticValues.allStudentsNames[row]);
				s+=translatedComma()+"\n";
				s+=tr("subject=%1").arg(statisticValues.allSubjectsNames[column]);
				s+=")";
				undoSubject=statisticValues.allSubjectsNames[column];
				undoStudents=statisticValues.allStudentsNames[row];

				assert(nTotalActsModified>=tmpIdentifySet.size());
				if(nTotalActsModified>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components,\nthere won't be modified an additional number "
						"of %1 activities\n(which are in the same larger split activities as the selected activities).").arg(nTotalActsModified-tmpIdentifySet.size());
				}
			}
			
			bool ok;
			QStringList items;
			items=subjectListWithQualifiedTeachers.at(subject);
			
			if(items.count()<1){
				QMessageBox::information(this, tr("FET - Information"), tr("There is no teacher qualified for this subject. You should set teachers qualified subjects in FET -> Data -> Teachers."));
				items=statisticValues.allTeachersNames;
			}
			s+="\n\n";
			s+=tr("Please choose a new teacher:");
			QString item = QInputDialog::getItem(this, tr("FET - Select new teacher dialog"), s, items, 0, false, &ok);
			if (ok) {
				int retst=QMessageBox::No;
				if(_affectOtherStudentsOverall){
					retst=QMessageBox::question(this, tr("Modify related?", "It refers to activities"),
						tr("This cell contains activities that relate to other students. Should the related activities also be modified?"), QMessageBox::Yes | QMessageBox::No);
				}

				int retsubj=QMessageBox::Yes;
				//int retsubj=QMessageBox::No;
				//if(_affectOtherSubjectsOverall){
				//	retsubj=QMessageBox::question(this, tr("Modify related?", "It refers to activities"),
				//		tr("This cell contains activities that relate to other subjects. Should the related activities also be modified?"), QMessageBox::Yes | QMessageBox::No);
				//}

				for(int i=0; i<_idsToBeModified.count(); i++){
					if( ((_affectOtherStudentsToBeModified.at(i)==false) || (retst==QMessageBox::Yes)) &&
						((_affectOtherSubjectsToBeModified.at(i)==false) || (retsubj==QMessageBox::Yes)) ){

						Activity* act=gt.rules.activitiesPointerHash.value(_idsToBeModified.at(i), nullptr);
						if(act==nullptr){
							assert(0==1);	//TODO: maybe just a warning
							//s+=QCoreApplication::translate("Activity", "Invalid (nonexistent) id for activity");
							break;
						}
						
						//QStringList teachers_names=act->teachersNames;
						QStringList teachers_names;
						teachers_names<<item;
						QString subject_name=act->subjectName;
						QStringList activity_tags_names=act->activityTagsNames;
						QStringList students_names=act->studentsNames;
						int duration=act->duration;
						bool active=act->active;
						bool computeNTotalStudents=act->computeNTotalStudents;
						int nTotalStudents=act->nTotalStudents;
	
						gt.rules.modifySubactivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
						//gt.rules.modifyActivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
					}
				}
				gt.rules.addUndoPoint(tr("Activity planning: Modified the activities with the subject %1 and the students %2. The new teacher is %3.")
				 .arg(undoSubject)
				 .arg(undoStudents)
				 .arg(item));
				PlanningChanged::increasePlanningCommunicationSpinBox();
			}
		}
	} else if(RBSwapTeachers->isChecked()) {
		if(storeStudentsForSwap==-1){
			if(swapAxes->checkState()==Qt::Checked){
				storeStudentsForSwap=column;
				storeSubjectForSwap=row;
			} else {
				storeStudentsForSwap=row;
				storeSubjectForSwap=column;
			}
			int ret=QMessageBox::question(this,
					tr("Swap teachers"),
						tr("You have selected %1 %2.\nPlease select another cell to swap teachers.", "%1 is a subject name, %2 is a students set name.").arg(statisticValues.allSubjectsNames[storeSubjectForSwap]).arg(statisticValues.allStudentsNames[storeStudentsForSwap]), QMessageBox::Ok | QMessageBox::Cancel);
			if(ret!=QMessageBox::Ok){
				storeStudentsForSwap=-1;
				storeSubjectForSwap=-1;
			}
		} else {
			if(swapAxes->checkState()==Qt::Checked){
				swapTeachers(storeStudentsForSwap, storeSubjectForSwap, column, row);
			} else {
				swapTeachers(storeStudentsForSwap, storeSubjectForSwap, row, column);
			}
			storeStudentsForSwap=-1;
			storeSubjectForSwap=-1;
			PlanningChanged::increasePlanningCommunicationSpinBox();
		}
	} else if(RBSwapStudents->isChecked()) {
		if(storeStudentsForSwap==-1){
			if(swapAxes->checkState()==Qt::Checked){
				storeStudentsForSwap=column;
				storeSubjectForSwap=row;
			} else {
				storeStudentsForSwap=row;
				storeSubjectForSwap=column;
			}
			int ret=QMessageBox::question(this,
					tr("Swap students"),
						tr("You have selected %1 %2.\nPlease select another cell to swap students.", "%1 is a subject name, %2 is a students set name").arg(statisticValues.allSubjectsNames[storeSubjectForSwap]).arg(statisticValues.allStudentsNames[storeStudentsForSwap]), QMessageBox::Ok | QMessageBox::Cancel);
			if(ret!=QMessageBox::Ok){
				storeStudentsForSwap=-1;
				storeSubjectForSwap=-1;
			}
		} else {
			if(swapAxes->checkState()==Qt::Checked){
				swapStudents(storeStudentsForSwap, storeSubjectForSwap, column, row);
			} else {
				swapStudents(storeStudentsForSwap, storeSubjectForSwap, row, column);
			}
			storeStudentsForSwap=-1;
			storeSubjectForSwap=-1;
			PlanningChanged::increasePlanningCommunicationSpinBox();
		}
	} else assert(0==1);
}

//mouseTracking (start 3/3)
/*
void ActivityPlanningForm::ActivitiesCellEntered(int row, int column){
	activitiesTable->setCurrentCell(row, column);
}

void ActivityPlanningForm::TeachersCellEntered(int row, int column){
	teachersTable->setCurrentCell(row, column);
}*/
//mouseTracking (end 3/3)

void ActivityPlanningForm::teachersTableHorizontalHeaderClicked(int column){
	if(column<0 || column>=statisticValues.allTeachersNames.count()){
		return;
	}
	if(RBActivity->isChecked()){
		ActivitiesForm form(this, statisticValues.allTeachersNames[column], "", "", "");
		setParentAndOtherThings(&form, this);
		form.exec();
	} else if(RBSubactivity->isChecked()) {
		SubactivitiesForm form(this, statisticValues.allTeachersNames[column], "", "", "");
		setParentAndOtherThings(&form, this);
		form.exec();
	} else if(RBAdd->isChecked()) {
		AddActivityForm addActivityForm(this, statisticValues.allTeachersNames[column], "", "", "");
		setParentAndOtherThings(&addActivityForm, this);
		addActivityForm.exec();	
	} else if(RBModify->isChecked()) {
		//Just enter the activity dialog, because in normal case there are too many activities related to a teacher.
		ActivitiesForm form(this, statisticValues.allTeachersNames[column], "", "", "");
		setParentAndOtherThings(&form, this);
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
					for(const QString& t : std::as_const(act->teachersNames)){
						if(t==statisticValues.allTeachersNames[column]){
							//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
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

			computeActivitiesForDeletion(statisticValues.allTeachersNames[column], "", "",
				tmpID, tmpGroupID,
				nTotalActsDeleted,
				_idsToBeRemoved, _agidsToBeRemoved,
				_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
				_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
				_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

			QString s=tr("Delete %1 activities of teacher %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allTeachersNames[column]);
			assert(nTotalActsDeleted>=tmpIdentifySet.size());
			if(nTotalActsDeleted>tmpIdentifySet.size()){
				s+="\n\n";
				s+=tr("Because you have individually modified the components, there will be removed an additional number "
					"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
			}
			int ret=QMessageBox::question(this, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
			if(ret==QMessageBox::Yes){
				int ret2=QMessageBox::No;
				if(_affectOtherTeachersOverall){
					ret2=QMessageBox::question(this, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other teachers. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
				}
				
				for(int i=0; i<_idsToBeRemoved.count(); i++){
					if((_affectOtherTeachersToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
						gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
					}
				}
				gt.rules.addUndoPoint(tr("Activity planning: Deleted %1 activities of the teacher %2.", "%1 is the number of activities")
				 .arg(tmpIdentifySet.size())
				 .arg(statisticValues.allTeachersNames[column]));
				PlanningChanged::increasePlanningCommunicationSpinBox();
			}
		}
	} else if(RBChangeTeacher->isChecked()) {
		//following source is nearly the same code as if(RBDelete->isChecked()). So do similar changes there too.
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
					for(const QString& t : std::as_const(act->teachersNames)){
						if(t==statisticValues.allTeachersNames[column]){
							//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
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
			int nTotalActsModified;
			QList<int> _idsToBeModified;
			QList<int> _agidsToBeModified;

			QList<bool> _affectOtherTeachersToBeModified;
			bool _affectOtherTeachersOverall;
			QList<bool> _affectOtherStudentsToBeModified;
			bool _affectOtherStudentsOverall;
			QList<bool> _affectOtherSubjectsToBeModified;
			bool _affectOtherSubjectsOverall;

			computeActivitiesForModification(statisticValues.allTeachersNames[column], "", "",
				tmpID, tmpGroupID,
				nTotalActsModified,
				_idsToBeModified, _agidsToBeModified,
				_affectOtherTeachersToBeModified, _affectOtherTeachersOverall,
				_affectOtherStudentsToBeModified, _affectOtherStudentsOverall,
				_affectOtherSubjectsToBeModified, _affectOtherSubjectsOverall);

			QString s=tr("Modify %1 activities of teacher %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allTeachersNames[column]);
			assert(nTotalActsModified>=tmpIdentifySet.size());
			if(nTotalActsModified>tmpIdentifySet.size()){
				s+="\n\n";
				s+=tr("Because you have individually modified the components,\nthere won't be modified an additional number "
					"of %1 activities\n(which are in the same larger split activities as the selected activities).").arg(nTotalActsModified-tmpIdentifySet.size());
			}
			bool ok;
			QStringList items;
			//items=subjectListWithQualifiedTeachers.at(subject);
			
			//if(items.count()<1){
			//	QMessageBox::information(this, tr("FET - Information"), tr("There is no teacher qualified for this subject. You should set teachers qualified subjects in FET -> Data -> Teachers."));
				items=statisticValues.allTeachersNames;
			//}
			s+="\n\n";
			s+=tr("Please choose a new teacher:");
			QString item = QInputDialog::getItem(this, tr("FET - Select new teacher dialog"), s, items, 0, false, &ok);
			if (ok) {
				int ret2=QMessageBox::No;
				if(_affectOtherTeachersOverall){
					ret2=QMessageBox::question(this, tr("Modify related?", "It refers to activities"), tr("There are activities affecting other teachers. Should the related activities also be modified?"), QMessageBox::Yes | QMessageBox::No);
				}
				
				for(int i=0; i<_idsToBeModified.count(); i++){
					if((_affectOtherTeachersToBeModified.at(i)==false) || (ret2==QMessageBox::Yes)){
						Activity* act=gt.rules.activitiesPointerHash.value(_idsToBeModified.at(i), nullptr);
						if(act==nullptr){
							assert(0==1);	//TODO: maybe just a warning
							//s+=QCoreApplication::translate("Activity", "Invalid (nonexistent) id for activity");
							break;
						}
						
						//QStringList teachers_names=act->teachersNames;
						QStringList teachers_names;
						teachers_names<<item;
						QString subject_name=act->subjectName;
						QStringList activity_tags_names=act->activityTagsNames;
						QStringList students_names=act->studentsNames;
						int duration=act->duration;
						bool active=act->active;
						bool computeNTotalStudents=act->computeNTotalStudents;
						int nTotalStudents=act->nTotalStudents;
	
						gt.rules.modifySubactivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
						//gt.rules.modifyActivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
					}
				}
				gt.rules.addUndoPoint(tr("Activity planning: Modified %1 activities of the teacher %2. The new teacher is %3.", "%1 is the number of activities")
				 .arg(tmpIdentifySet.size())
				 .arg(statisticValues.allTeachersNames[column])
				 .arg(item));
				PlanningChanged::increasePlanningCommunicationSpinBox();
			}
		}
	} else if(RBSwapTeachers->isChecked()) {
		//TODO: warnings are not nice; recheck them
		//following source is nearly the same code as if(RBDelete->isChecked()). So do similar changes there too.
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
					for(const QString& t : std::as_const(act->teachersNames)){
						if(t==statisticValues.allTeachersNames[column]){
							//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
							//if(!tmpIdentify.contains(tmpIdent)){
							if(!tmpIdentifySet.contains(act->id)){
								tmpID<<act->id;
								tmpGroupID<<act->activityGroupId;
								//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
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
			int nTotalActsModified;
			QList<int> _idsToBeModified;
			QList<int> _agidsToBeModified;

			QList<bool> _affectOtherTeachersToBeModified;
			bool _affectOtherTeachersOverall;
			QList<bool> _affectOtherStudentsToBeModified;
			bool _affectOtherStudentsOverall;
			QList<bool> _affectOtherSubjectsToBeModified;
			bool _affectOtherSubjectsOverall;

			computeActivitiesForModification(statisticValues.allTeachersNames[column], "", "",
				tmpID, tmpGroupID,
				nTotalActsModified,
				_idsToBeModified, _agidsToBeModified,
				_affectOtherTeachersToBeModified, _affectOtherTeachersOverall,
				_affectOtherStudentsToBeModified, _affectOtherStudentsOverall,
				_affectOtherSubjectsToBeModified, _affectOtherSubjectsOverall);

			QString s=tr("Modify %1 activities of teacher %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allTeachersNames[column]);
			assert(nTotalActsModified>=tmpIdentifySet.size());
			if(nTotalActsModified>tmpIdentifySet.size()){
				s+="\n\n";
				s+=tr("Because you have individually modified the components,\nthere won't be modified an additional number "
					"of %1 activities\n(which are in the same larger split activities as the selected activities).").arg(nTotalActsModified-tmpIdentifySet.size());
			}
			bool ok;
			QStringList items;
			//items=subjectListWithQualifiedTeachers.at(subject);
		
			//if(items.count()<1){
			//	QMessageBox::information(this, tr("FET - Information"), tr("There is no teacher qualified for this subject. You should set teachers qualified subjects in FET -> Data -> Teachers."));
				items=statisticValues.allTeachersNames;
			//}
			s+="\n\n";
			s+=tr("Please choose teacher for swap:");
			QString teacher2 = QInputDialog::getItem(this, tr("FET - Select teacher for swap dialog"), s, items, 0, false, &ok);
			if (ok) {
				//care about activities of teacher2 (start)
				QList<int> tmpID2;
				QList<int> tmpGroupID2;
				//QStringList tmpIdentify;
				QSet<int> tmpIdentifySet2;
				//QList<bool> tmpAffectOtherTeachers;
				Activity* act2;
				for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
					act2=gt.rules.activitiesList[ai];
					int tmpCurrentIndex=CBActive->currentIndex();
					if( ((act2->active) && (tmpCurrentIndex==ACTIVE_ONLY))
						|| ((!act2->active) && (tmpCurrentIndex==INACTIVE_ONLY))
						|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
							for(const QString& t : std::as_const(act2->teachersNames)){
								if(t==teacher2){
									//QString tmpIdent=CustomFETString::number(act2->id)+" "+CustomFETString::number(act2->activityGroupId);
									//if(!tmpIdentify.contains(tmpIdent)){
									if(!tmpIdentifySet2.contains(act2->id)){
										tmpID2<<act2->id;
										tmpGroupID2<<act2->activityGroupId;
										//tmpIdentify<<CustomFETString::number(act2->id)+" "+CustomFETString::number(act2->activityGroupId);
										tmpIdentifySet2.insert(act2->id);
										/*if(act2->teachersNames.size()>1){
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
				assert(tmpID2.count()==tmpIdentifySet2.size());
				assert(tmpGroupID2.count()==tmpIdentifySet2.size());
				//assert(tmpAffectOtherTeachers2.count()==tmpIdentifySet2.size());
		
				if(!tmpIdentifySet2.isEmpty()){
					int nTotalActsModified2;
					QList<int> _idsToBeModified2;
					QList<int> _agidsToBeModified2;

					QList<bool> _affectOtherTeachersToBeModified2;
					bool _affectOtherTeachersOverall2;
					QList<bool> _affectOtherStudentsToBeModified2;
					bool _affectOtherStudentsOverall2;
					QList<bool> _affectOtherSubjectsToBeModified2;
					bool _affectOtherSubjectsOverall2;
		
					computeActivitiesForModification(teacher2, "", "",
						tmpID2, tmpGroupID2,
						nTotalActsModified2,
						_idsToBeModified2, _agidsToBeModified2,
						_affectOtherTeachersToBeModified2, _affectOtherTeachersOverall2,
						_affectOtherStudentsToBeModified2, _affectOtherStudentsOverall2,
						_affectOtherSubjectsToBeModified2, _affectOtherSubjectsOverall2);

					QString s=tr("Modify %1 activities of teacher %2?").arg(tmpIdentifySet.size()).arg(teacher2);
					s+=tr("Modify %1 activities of teacher %2?").arg(tmpIdentifySet2.size()).arg(statisticValues.allTeachersNames[column]);
					assert(nTotalActsModified>=tmpIdentifySet.size());
					if(nTotalActsModified>tmpIdentifySet.size()){
						s+="\n\n";
						s+=tr("Because you have individually modified the components,\nthere won't be modified an additional number "
							"of %1 activities\n(which are in the same larger split activities as the selected activities).").arg(nTotalActsModified-tmpIdentifySet.size());
					}
					//care about activities of teacher2 (end)
				
					int ret2=QMessageBox::No;
					if(_affectOtherTeachersOverall){
						ret2=QMessageBox::question(this, tr("Modify related?", "It refers to activities"), tr("There are activities affecting other teachers. Should the related activities also be modified?"), QMessageBox::Yes | QMessageBox::No);
					}
					
					for(int i=0; i<_idsToBeModified.count(); i++){
						if((_affectOtherTeachersToBeModified.at(i)==false) || (ret2==QMessageBox::Yes)){
							Activity* act=gt.rules.activitiesPointerHash.value(_idsToBeModified.at(i), nullptr);
							if(act==nullptr){
								assert(0==1);	//TODO: maybe just a warning
								//s+=QCoreApplication::translate("Activity", "Invalid (nonexistent) id for activity");
								break;
							}
							
							//QStringList teachers_names=act->teachersNames;
							QStringList teachers_names;
							teachers_names<<teacher2;
							QString subject_name=act->subjectName;
							QStringList activity_tags_names=act->activityTagsNames;
							QStringList students_names=act->studentsNames;
							int duration=act->duration;
							bool active=act->active;
							bool computeNTotalStudents=act->computeNTotalStudents;
							int nTotalStudents=act->nTotalStudents;
		
							gt.rules.modifySubactivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
							//gt.rules.modifyActivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
						}
					}
					for(int i=0; i<_idsToBeModified2.count(); i++){
						if((_affectOtherTeachersToBeModified2.at(i)==false) || (ret2==QMessageBox::Yes)){
							Activity* act=gt.rules.activitiesPointerHash.value(_idsToBeModified2.at(i), nullptr);
							if(act==nullptr){
								assert(0==1);	//TODO: maybe just a warning
								//s+=QCoreApplication::translate("Activity", "Invalid (nonexistent) id for activity");
								break;
							}
							
							//QStringList teachers_names=act->teachersNames;
							QStringList teachers_names;
							teachers_names<<statisticValues.allTeachersNames[column];
							QString subject_name=act->subjectName;
							QStringList activity_tags_names=act->activityTagsNames;
							QStringList students_names=act->studentsNames;
							int duration=act->duration;
							bool active=act->active;
							bool computeNTotalStudents=act->computeNTotalStudents;
							int nTotalStudents=act->nTotalStudents;
		
							gt.rules.modifySubactivity(_idsToBeModified2.at(i), _agidsToBeModified2.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
							//gt.rules.modifyActivity(_idsToBeModified2.at(i), _agidsToBeModified2.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
						}
					}
					gt.rules.addUndoPoint(tr("Activity planning: Swapped %1 activities of the teacher %2 with the teacher %3.", "%1 is the number of activities")
					 .arg(tmpIdentifySet.size())
					 .arg(statisticValues.allTeachersNames[column])
					 .arg(teacher2));
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
		}
	} else if(RBSwapStudents->isChecked()) {
		//no function
	}else assert(0==1);
}

void ActivityPlanningForm::teachersCellSelected(const QModelIndex& index){
	/*if(item==nullptr){
		return;
	}*/
	if(!index.isValid())
		return;
	int itrow=index.row();
	int itcol=index.column();
	if(itrow==0){
		bool ok=false;
		int targetHours=QInputDialog::getInt(this, tr("FET question"), tr("Please enter the target number of hours:")+" ",
		 teachersTargetNumberOfHours.at(itcol), 0, gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek, 1, &ok);
		if(ok){
			teachersTargetNumberOfHours[itcol]=targetHours;
			
			int tb=teachersList.at(itcol)->targetNumberOfHours;
			int ta=targetHours;
			
			teachersList.at(itcol)->targetNumberOfHours=targetHours;
			/*useless, because i also need to remove the table head item and i don't know how, so i redo the whole table
			teachersTable->removeCellWidget(0, itcol);
			QTableWidgetItem* newItem2=new QTableWidgetItem(CustomFETString::number(teachersTargetNumberOfHours.at(itcol)));
			newItem2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			teachersTable->setItem(0, itcol, newItem2);*/
			
			/*doesn't work, but idea is maybe better/faster!
			QTableWidgetItem* newItem2= teachersTable->takeItem(0, itcol);
			newItem2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			teachersTable->setItem(0, itcol, newItem2);
			QString tmpString=statisticValues.allTeachersNames.at(itcol)+"\n"+CustomFETString::number(statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(itcol))-teachersTargetNumberOfHours.at(itcol));
			if(statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(itcol))!=statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(itcol)))
				tmpString+=" ("+CustomFETString::number(statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(itcol))-teachersTargetNumberOfHours.at(itcol))+")";
			QTableWidgetItem* newItem3= teachersTable->takeVerticalHeaderItem(itcol);
			newItem3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			teachersTable->setItem(0, itcol, newItem3);*/
			
			//currently using not fast, but safe calculation
			updateTables_Teachers();
			updateTablesVisual();

			gt.rules.addUndoPoint(tr("Activity planning: Changed the target number of hours for the teacher %1 from %2 to %3.")
			 .arg(teachersList.at(itcol)->name)
			 .arg(tb)
			 .arg(ta));
			
			gt.rules.internalStructureComputed=false;
			setRulesModifiedAndOtherThings(&gt.rules);
		}
	} else {
		if(RBActivity->isChecked()){
			ActivitiesForm form(this, statisticValues.allTeachersNames[itcol], "", "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else if(RBSubactivity->isChecked()) {
			SubactivitiesForm form(this, statisticValues.allTeachersNames[itcol], "", "", "");
			setParentAndOtherThings(&form, this);
			form.exec();
		} else if(RBAdd->isChecked()) {
			AddActivityForm addActivityForm(this, statisticValues.allTeachersNames[itcol], "", "", "");
			setParentAndOtherThings(&addActivityForm, this);
			addActivityForm.exec();
		} else if(RBModify->isChecked()) {
			//Just enter the activity dialog, because in normal case there are too many activities related to a teacher.
			ActivitiesForm form(this, statisticValues.allTeachersNames[itcol], "", "", "");
			setParentAndOtherThings(&form, this);
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
						for(const QString& t : std::as_const(act->teachersNames)){
							if(t==statisticValues.allTeachersNames[itcol]){
								//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
								//if(!tmpIdentify.contains(tmpIdent)){
								if(!tmpIdentifySet.contains(act->id)){
									tmpID<<act->id;
									tmpGroupID<<act->activityGroupId;
									//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
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
	
				computeActivitiesForDeletion(statisticValues.allTeachersNames[itcol], "", "",
					tmpID, tmpGroupID,
					nTotalActsDeleted,
					_idsToBeRemoved, _agidsToBeRemoved,
					_affectOtherTeachersToBeRemoved, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeRemoved, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeRemoved, _affectOtherSubjectsOverall);

				QString s=tr("Delete %1 activities of teacher %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allTeachersNames[itcol]);
				assert(nTotalActsDeleted>=tmpIdentifySet.size());
				if(nTotalActsDeleted>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components, there will be removed an additional number "
						"of %1 activities (which are in the same larger split activities as the selected activities).").arg(nTotalActsDeleted-tmpIdentifySet.size());
				}
				int ret=QMessageBox::question(this, tr("Delete multiple?", "It refers to activities"), s, QMessageBox::Yes | QMessageBox::No);
				if(ret==QMessageBox::Yes){
					int ret2=QMessageBox::No;
					if(_affectOtherTeachersOverall){
						ret2=QMessageBox::question(this, tr("Delete related?", "It refers to activities"), tr("There are activities affecting other teachers. Should the related activities also be deleted?"), QMessageBox::Yes | QMessageBox::No);
					}
					
					for(int i=0; i<_idsToBeRemoved.count(); i++){
						if((_affectOtherTeachersToBeRemoved.at(i)==false) || (ret2==QMessageBox::Yes)){
							gt.rules.removeActivity(_idsToBeRemoved.at(i), _agidsToBeRemoved.at(i));
						}
					}
					gt.rules.addUndoPoint(tr("Activity planning: Deleted %1 activities of the teacher %2.", "%1 is the number of activities")
					 .arg(tmpIdentifySet.size())
					 .arg(statisticValues.allTeachersNames[itcol]));
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
		} else if(RBChangeTeacher->isChecked()) {
			//following source is nearly the same code as if(RBDelete->isChecked()). So do similar changes there too.
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
						for(const QString& t : std::as_const(act->teachersNames)){
							if(t==statisticValues.allTeachersNames[itcol]){
								//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
								//if(!tmpIdentify.contains(tmpIdent)){
								if(!tmpIdentifySet.contains(act->id)){
									tmpID<<act->id;
									tmpGroupID<<act->activityGroupId;
									//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
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
				int nTotalActsModified;
				QList<int> _idsToBeModified;
				QList<int> _agidsToBeModified;

				QList<bool> _affectOtherTeachersToBeModified;
				bool _affectOtherTeachersOverall;
				QList<bool> _affectOtherStudentsToBeModified;
				bool _affectOtherStudentsOverall;
				QList<bool> _affectOtherSubjectsToBeModified;
				bool _affectOtherSubjectsOverall;
	
				computeActivitiesForModification(statisticValues.allTeachersNames[itcol], "", "",
					tmpID, tmpGroupID,
					nTotalActsModified,
					_idsToBeModified, _agidsToBeModified,
					_affectOtherTeachersToBeModified, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeModified, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeModified, _affectOtherSubjectsOverall);

				QString s=tr("Modify %1 activities of teacher %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allTeachersNames[itcol]);
				assert(nTotalActsModified>=tmpIdentifySet.size());
				if(nTotalActsModified>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components,\nthere won't be modified an additional number "
						"of %1 activities\n(which are in the same larger split activities as the selected activities).").arg(nTotalActsModified-tmpIdentifySet.size());
				}
				bool ok;
				QStringList items;
				//items=subjectListWithQualifiedTeachers.at(subject);
				
				//if(items.count()<1){
				//	QMessageBox::information(this, tr("FET - Information"), tr("There is no teacher qualified for this subject. You should set teachers qualified subjects in FET -> Data -> Teachers."));
					items=statisticValues.allTeachersNames;
				//}
				s+="\n\n";
				s+=tr("Please choose a new teacher:");
				QString item = QInputDialog::getItem(this, tr("FET - Select new teacher dialog"), s, items, 0, false, &ok);
				if (ok) {
					int ret2=QMessageBox::No;
					if(_affectOtherTeachersOverall){
						ret2=QMessageBox::question(this, tr("Modify related?", "It refers to activities"), tr("There are activities affecting other teachers. Should the related activities also be modified?"), QMessageBox::Yes | QMessageBox::No);
					}
					
					for(int i=0; i<_idsToBeModified.count(); i++){
						if((_affectOtherTeachersToBeModified.at(i)==false) || (ret2==QMessageBox::Yes)){
							Activity* act=gt.rules.activitiesPointerHash.value(_idsToBeModified.at(i), nullptr);
							if(act==nullptr){
								assert(0==1);	//TODO: maybe just a warning
								//s+=QCoreApplication::translate("Activity", "Invalid (nonexistent) id for activity");
								break;
							}
							
							//QStringList teachers_names=act->teachersNames;
							QStringList teachers_names;
							teachers_names<<item;
							QString subject_name=act->subjectName;
							QStringList activity_tags_names=act->activityTagsNames;
							QStringList students_names=act->studentsNames;
							int duration=act->duration;
							bool active=act->active;
							bool computeNTotalStudents=act->computeNTotalStudents;
							int nTotalStudents=act->nTotalStudents;
		
							gt.rules.modifySubactivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
							//gt.rules.modifyActivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
						}
					}
					gt.rules.addUndoPoint(tr("Activity planning: Modified %1 activities of the teacher %2. The new teacher is %3.", "%1 is the number of activities")
					 .arg(tmpIdentifySet.size())
					 .arg(statisticValues.allTeachersNames[itcol])
					 .arg(item));
					PlanningChanged::increasePlanningCommunicationSpinBox();
				}
			}
		} else if(RBSwapTeachers->isChecked()) {
			//following source is nearly the same code as if(RBDelete->isChecked()). So do similar changes there too.
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
						for(const QString& t : std::as_const(act->teachersNames)){
							if(t==statisticValues.allTeachersNames[itcol]){
								//QString tmpIdent=CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
								//if(!tmpIdentify.contains(tmpIdent)){
								if(!tmpIdentifySet.contains(act->id)){
									tmpID<<act->id;
									tmpGroupID<<act->activityGroupId;
									//tmpIdentify<<CustomFETString::number(act->id)+" "+CustomFETString::number(act->activityGroupId);
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
				int nTotalActsModified;
				QList<int> _idsToBeModified;
				QList<int> _agidsToBeModified;

				QList<bool> _affectOtherTeachersToBeModified;
				bool _affectOtherTeachersOverall;
				QList<bool> _affectOtherStudentsToBeModified;
				bool _affectOtherStudentsOverall;
				QList<bool> _affectOtherSubjectsToBeModified;
				bool _affectOtherSubjectsOverall;
	
				computeActivitiesForModification(statisticValues.allTeachersNames[itcol], "", "",
					tmpID, tmpGroupID,
					nTotalActsModified,
					_idsToBeModified, _agidsToBeModified,
					_affectOtherTeachersToBeModified, _affectOtherTeachersOverall,
					_affectOtherStudentsToBeModified, _affectOtherStudentsOverall,
					_affectOtherSubjectsToBeModified, _affectOtherSubjectsOverall);

				QString s=tr("Modify %1 activities of teacher %2?").arg(tmpIdentifySet.size()).arg(statisticValues.allTeachersNames[itcol]);
				assert(nTotalActsModified>=tmpIdentifySet.size());
				if(nTotalActsModified>tmpIdentifySet.size()){
					s+="\n\n";
					s+=tr("Because you have individually modified the components,\nthere won't be modified an additional number "
						"of %1 activities\n(which are in the same larger split activities as the selected activities).").arg(nTotalActsModified-tmpIdentifySet.size());
				}
				bool ok;
				QStringList items;
				//items=subjectListWithQualifiedTeachers.at(subject);
			
				//if(items.count()<1){
				//	QMessageBox::information(this, tr("FET - Information"), tr("There is no teacher qualified for this subject. You should set teachers qualified subjects in FET -> Data -> Teachers."));
					items=statisticValues.allTeachersNames;
				//}
				s+="\n\n";
				s+=tr("Please choose teacher for swap:");
				QString teacher2 = QInputDialog::getItem(this, tr("FET - Select teacher for swap dialog"), s, items, 0, false, &ok);
				if (ok) {
					//care about activities of teacher2 (start)
					QList<int> tmpID2;
					QList<int> tmpGroupID2;
					//QStringList tmpIdentify;
					QSet<int> tmpIdentifySet2;
					//QList<bool> tmpAffectOtherTeachers;
					Activity* act2;
					for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
						act2=gt.rules.activitiesList[ai];
						int tmpCurrentIndex=CBActive->currentIndex();
						if( ((act2->active) && (tmpCurrentIndex==ACTIVE_ONLY))
							|| ((!act2->active) && (tmpCurrentIndex==INACTIVE_ONLY))
							|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
								for(const QString& t : std::as_const(act2->teachersNames)){
									if(t==teacher2){
										//QString tmpIdent=CustomFETString::number(act2->id)+" "+CustomFETString::number(act2->activityGroupId);
										//if(!tmpIdentify.contains(tmpIdent)){
										if(!tmpIdentifySet2.contains(act2->id)){
											tmpID2<<act2->id;
											tmpGroupID2<<act2->activityGroupId;
											//tmpIdentify<<CustomFETString::number(act2->id)+" "+CustomFETString::number(act2->activityGroupId);
											tmpIdentifySet2.insert(act2->id);
											/*if(act2->teachersNames.size()>1){
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
					assert(tmpID2.count()==tmpIdentifySet2.size());
					assert(tmpGroupID2.count()==tmpIdentifySet2.size());
					//assert(tmpAffectOtherTeachers2.count()==tmpIdentifySet2.size());
			
					if(!tmpIdentifySet2.isEmpty()){
						int nTotalActsModified2;
						QList<int> _idsToBeModified2;
						QList<int> _agidsToBeModified2;

						QList<bool> _affectOtherTeachersToBeModified2;
						bool _affectOtherTeachersOverall2;
						QList<bool> _affectOtherStudentsToBeModified2;
						bool _affectOtherStudentsOverall2;
						QList<bool> _affectOtherSubjectsToBeModified2;
						bool _affectOtherSubjectsOverall2;
			
						computeActivitiesForModification(teacher2, "", "",
							tmpID2, tmpGroupID2,
							nTotalActsModified2,
							_idsToBeModified2, _agidsToBeModified2,
							_affectOtherTeachersToBeModified2, _affectOtherTeachersOverall2,
							_affectOtherStudentsToBeModified2, _affectOtherStudentsOverall2,
							_affectOtherSubjectsToBeModified2, _affectOtherSubjectsOverall2);

						QString s=tr("Modify %1 activities of teacher %2?").arg(tmpIdentifySet.size()).arg(teacher2);
						s+=tr("Modify %1 activities of teacher %2?").arg(tmpIdentifySet2.size()).arg(statisticValues.allTeachersNames[itcol]);
						assert(nTotalActsModified>=tmpIdentifySet.size());
						if(nTotalActsModified>tmpIdentifySet.size()){
							s+="\n\n";
							s+=tr("Because you have individually modified the components,\nthere won't be modified an additional number "
								"of %1 activities\n(which are in the same larger split activities as the selected activities).").arg(nTotalActsModified-tmpIdentifySet.size());
						}
						//care about activities of teacher2 (end)
					
						int ret2=QMessageBox::No;
						if(_affectOtherTeachersOverall){
							ret2=QMessageBox::question(this, tr("Modify related?", "It refers to activities"), tr("There are activities affecting other teachers. Should the related activities also be modified?"), QMessageBox::Yes | QMessageBox::No);
						}
						
						for(int i=0; i<_idsToBeModified.count(); i++){
							if((_affectOtherTeachersToBeModified.at(i)==false) || (ret2==QMessageBox::Yes)){
								Activity* act=gt.rules.activitiesPointerHash.value(_idsToBeModified.at(i), nullptr);
								if(act==nullptr){
									assert(0==1);	//TODO: maybe just a warning
									//s+=QCoreApplication::translate("Activity", "Invalid (nonexistent) id for activity");
									break;
								}
								
								//QStringList teachers_names=act->teachersNames;
								QStringList teachers_names;
								teachers_names<<teacher2;
								QString subject_name=act->subjectName;
								QStringList activity_tags_names=act->activityTagsNames;
								QStringList students_names=act->studentsNames;
								int duration=act->duration;
								bool active=act->active;
								bool computeNTotalStudents=act->computeNTotalStudents;
								int nTotalStudents=act->nTotalStudents;
			
								gt.rules.modifySubactivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
								//gt.rules.modifyActivity(_idsToBeModified.at(i), _agidsToBeModified.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
							}
						}
						for(int i=0; i<_idsToBeModified2.count(); i++){
							if((_affectOtherTeachersToBeModified2.at(i)==false) || (ret2==QMessageBox::Yes)){
								Activity* act=gt.rules.activitiesPointerHash.value(_idsToBeModified2.at(i), nullptr);
								if(act==nullptr){
									assert(0==1);	//TODO: maybe just a warning
									//s+=QCoreApplication::translate("Activity", "Invalid (nonexistent) id for activity");
									break;
								}
								
								//QStringList teachers_names=act->teachersNames;
								QStringList teachers_names;
								teachers_names<<statisticValues.allTeachersNames[itcol];
								QString subject_name=act->subjectName;
								QStringList activity_tags_names=act->activityTagsNames;
								QStringList students_names=act->studentsNames;
								int duration=act->duration;
								bool active=act->active;
								bool computeNTotalStudents=act->computeNTotalStudents;
								int nTotalStudents=act->nTotalStudents;
			
								gt.rules.modifySubactivity(_idsToBeModified2.at(i), _agidsToBeModified2.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
								//gt.rules.modifyActivity(_idsToBeModified2.at(i), _agidsToBeModified2.at(i), teachers_names, subject_name, activity_tags_names,students_names, duration, active, computeNTotalStudents, nTotalStudents);
							}
						}
						gt.rules.addUndoPoint(tr("Activity planning: Modified %1 activities of the teacher %2. The new teacher is %3.", "%1 is the number of activities")
						 .arg(tmpIdentifySet.size())
						 .arg(statisticValues.allTeachersNames[itcol])
						 .arg(teacher2));
						PlanningChanged::increasePlanningCommunicationSpinBox();
					}
				}
			}
		} else if(RBSwapStudents->isChecked()) {
			//no function
		} else assert(0==1);
	}
}

void ActivityPlanningForm::updateTables(){
	updateTables_Students_Subjects();
	updateTables_Teachers();

	updateTablesVisual();
}

void ActivityPlanningForm::updateTables_Students_Subjects(){	//similar to statisticsexport.cpp
	QMultiHash<QString, int> studentsActivities;

	statisticValues.studentsTotalNumberOfHours.clear();
	statisticValues.studentsTotalNumberOfHours2.clear();
	statisticValues.subjectsTotalNumberOfHours.clear();
	statisticValues.subjectsTotalNumberOfHours4.clear();
	
	Activity* act;
	int tmpCurrentIndex=CBActive->currentIndex();
	for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
		//progress.setValue(ai);
		act=gt.rules.activitiesList[ai];
		if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
			|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
			|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
				//subjectsActivities.insert(act->subjectName, ai);
				int tmp=statisticValues.subjectsTotalNumberOfHours.value(act->subjectName)+act->duration;
				statisticValues.subjectsTotalNumberOfHours.insert(act->subjectName, tmp);						// (1) so teamlearning-teaching is not counted twice!
				for(const QString& st : std::as_const(act->studentsNames)){
					studentsActivities.insert(st, ai);
					tmp=statisticValues.studentsTotalNumberOfHours.value(st)+act->duration;
					statisticValues.studentsTotalNumberOfHours.insert(st, tmp);							// (2)
					//subjectstTotalNumberOfHours3[act->subjectIndex]+=duration;				// (1) so teamlearning is counted twice!
				}
				for(const QString& st : std::as_const(act->studentsNames)){
					tmp=statisticValues.studentsTotalNumberOfHours2.value(st);
					tmp += act->duration * act->teachersNames.count();
					statisticValues.studentsTotalNumberOfHours2.insert(st, tmp);					// (2)
				}
				tmp=statisticValues.subjectsTotalNumberOfHours4.value(act->subjectName);
				tmp += act->duration * act->studentsNames.count() * act->teachersNames.count();
				statisticValues.subjectsTotalNumberOfHours4.insert(act->subjectName, tmp);			// (1) so teamlearning-teaching is counted twice!
			}
	}
	//progress.setValue(gt.rules.activitiesList.size());
	QStringList studentsLabels;
	QStringList subjectsLabels;
	studentsLabels.clear();
	subjectsLabels.clear();
	
	activitiesTableView->model.clearDataAndHeaders();

	if(swapAxes->checkState()==Qt::Checked){
		/*activitiesTable->setRowCount(statisticValues.allSubjectsNames.count());
		activitiesTable->setColumnCount(statisticValues.allStudentsNames.count());*/
		activitiesTableView->model.resize(statisticValues.allSubjectsNames.count(), statisticValues.allStudentsNames.count());
	} else {
		/*activitiesTable->setRowCount(statisticValues.allStudentsNames.count());
		activitiesTable->setColumnCount(statisticValues.allSubjectsNames.count());*/
		activitiesTableView->model.resize(statisticValues.allStudentsNames.count(), statisticValues.allSubjectsNames.count());
	}

	for(int students=0; students<statisticValues.allStudentsNames.count(); students++){
		//progress2.setValue(students);
		QList<int> tmpStudents;
		QMultiHash<QString, int> tmpSubjects;
		tmpStudents.clear();
		tmpSubjects.clear();
		tmpStudents=studentsActivities.values(statisticValues.allStudentsNames.at(students));
		for(int aidx : std::as_const(tmpStudents)){
			Activity* act=gt.rules.activitiesList.at(aidx);
			tmpSubjects.insert(act->subjectName, aidx);
		}
		
		QString tmpStudentsLabel="";
		tmpStudentsLabel=statisticValues.allStudentsNames.at(students);
		tmpStudentsLabel+="\n"+CustomFETString::number(statisticValues.studentsTotalNumberOfHours.value(statisticValues.allStudentsNames.at(students)));
		if(statisticValues.studentsTotalNumberOfHours.value(statisticValues.allStudentsNames.at(students))!=statisticValues.studentsTotalNumberOfHours2.value(statisticValues.allStudentsNames.at(students)))
			tmpStudentsLabel+=" ("+CustomFETString::number(statisticValues.studentsTotalNumberOfHours2.value(statisticValues.allStudentsNames.at(students)))+")";
		studentsLabels<<tmpStudentsLabel;
		for(int subject=0; subject<statisticValues.allSubjectsNames.count(); subject++){
			QList<int> tmpActivities;
			tmpActivities.clear();
			tmpActivities=tmpSubjects.values(statisticValues.allSubjectsNames.at(subject));
			QString tmpString="";
			if(!tmpActivities.isEmpty()){
				QMap<LocaleString, int> tmpIdentDuration;	//not QHash, because I need the correct order of the activities
				for(int tmpAct : std::as_const(tmpActivities)){
					Activity* act=gt.rules.activitiesList[tmpAct];
					int tmpD=act->duration;
					QString tmpIdent;
					if(showTeachers->checkState()==Qt::Checked){
						if(act->teachersNames.size()>0){
							for(QStringList::const_iterator it=act->teachersNames.constBegin(); it!=act->teachersNames.constEnd(); it++){
								tmpIdent+=*it;
								if(it!=act->teachersNames.constEnd()-1)
									tmpIdent+=translatedCommaSpace();
							}
						}
					}
					tmpIdent+=" ";
					if(showActivityTags->checkState()==Qt::Checked){
						if(act->activityTagsNames.size()>0){
							for(QStringList::const_iterator it=act->activityTagsNames.constBegin(); it!=act->activityTagsNames.constEnd(); it++){
								tmpIdent+=*it;
								if(it!=act->activityTagsNames.constEnd()-1)
									tmpIdent+=translatedCommaSpace();
							}
						}
					}
					tmpD+=tmpIdentDuration.value(tmpIdent);
					tmpIdentDuration.insert(tmpIdent, tmpD);
				}
				QMap<LocaleString, int>::const_iterator it=tmpIdentDuration.constBegin();
				while(it!=tmpIdentDuration.constEnd()){
					tmpString+=CustomFETString::number(it.value());
					tmpString+=" "+it.key();
					it++;
					if(it!=tmpIdentDuration.constEnd())
						tmpString+="\n";
				}
			}
			if(!tmpActivities.isEmpty()){
				/*QTableWidgetItem* newItem=new QTableWidgetItem(tmpString);
				newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);*/
				QPair<int, int> pair;
				if(swapAxes->checkState()==Qt::Checked){
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
	//progress2.setValue(statisticValues.allStudentsNames.count());
	for(const QString& subjects : std::as_const(statisticValues.allSubjectsNames)){
		QString tmpSubjectLabel="";
		tmpSubjectLabel=subjects+"\n"+CustomFETString::number(statisticValues.subjectsTotalNumberOfHours.value(subjects));
		if(statisticValues.subjectsTotalNumberOfHours.value(subjects)!=statisticValues.subjectsTotalNumberOfHours4.value(subjects))
			tmpSubjectLabel+=" ("+CustomFETString::number(statisticValues.subjectsTotalNumberOfHours4.value(subjects))+")";
		subjectsLabels<<tmpSubjectLabel;
	}
	if(swapAxes->checkState()==Qt::Checked){
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
	QMultiHash<QString, int> teachersActivities;
	
	statisticValues.teachersTotalNumberOfHours.clear();
	statisticValues.teachersTotalNumberOfHours2.clear();
	
	Activity* act;
	int tmpCurrentIndex=CBActive->currentIndex();
	for(int ai=0; ai<gt.rules.activitiesList.size(); ai++){
		//progress.setValue(ai);
		act=gt.rules.activitiesList[ai];
		if( ((act->active) && (tmpCurrentIndex==ACTIVE_ONLY))
			|| ((!act->active) && (tmpCurrentIndex==INACTIVE_ONLY))
			|| (tmpCurrentIndex==ACTIVE_OR_INACTIVE)){
				int tmp;
				for(const QString& t : std::as_const(act->teachersNames)){
					teachersActivities.insert(t, ai);
					tmp=statisticValues.teachersTotalNumberOfHours.value(t)+act->duration;
					statisticValues.teachersTotalNumberOfHours.insert(t, tmp);							// (3)
				}
				for(const QString& t : std::as_const(act->teachersNames)){
					tmp=statisticValues.teachersTotalNumberOfHours2.value(t);
					tmp += act->duration * act->studentsNames.count();
					statisticValues.teachersTotalNumberOfHours2.insert(t, tmp);						// (3)
				}
			}
	}
	//progress.setValue(gt.rules.activitiesList.size());

	QStringList teachersLabels;
	for(int teacher=0; teacher<statisticValues.allTeachersNames.count(); teacher++){
		QString prefixString1="";
		QString prefixString2="";
		if(teachersTargetNumberOfHours.at(teacher)!=0){
			if(statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(teacher))>teachersTargetNumberOfHours.at(teacher)){
				prefixString1="+";
			}else if(statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(teacher))==teachersTargetNumberOfHours.at(teacher)){
				prefixString1="=";
			}
			if(statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(teacher))>teachersTargetNumberOfHours.at(teacher)){
				prefixString2="+";
			}else if(statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(teacher))==teachersTargetNumberOfHours.at(teacher)){
				prefixString2="=";
			}
		}
		QString tmpString=statisticValues.allTeachersNames.at(teacher)+"\n"+prefixString1+CustomFETString::number(statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(teacher))-teachersTargetNumberOfHours.at(teacher));
		if(statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(teacher))!=statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(teacher)))
			tmpString+=" ("+prefixString2+CustomFETString::number(statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(teacher))-teachersTargetNumberOfHours.at(teacher))+")";
		teachersLabels<<tmpString;
	}
	
	teachersTableView->model.clearDataAndHeaders();

	teachersTableView->model.resize(3, statisticValues.allTeachersNames.count());
	
	QStringList vLabels;
	vLabels<<tr("Target", "Please keep translation short.")
		<<tr("Subjects", "Please keep translation short")
		<<tr("Students", "Please keep translation short");

	teachersTableView->model.verticalHeaderItems=vLabels;
	teachersTableView->model.horizontalHeaderItems=teachersLabels;
	
	for(int teacher=0; teacher<statisticValues.allTeachersNames.count(); teacher++){
		//QTableWidgetItem* newItem=new QTableWidgetItem(statisticValues.allTeachersNames[teacher]);
		//newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		/*QTableWidgetItem* newItem2=new QTableWidgetItem(CustomFETString::number(teachersTargetNumberOfHours.at(teacher)));
		newItem2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		teachersTable->setItem(0, teacher, newItem2);*/
		QString str=CustomFETString::number(teachersTargetNumberOfHours.at(teacher));
		QPair<int, int> pair;
		pair.first=0;
		pair.second=teacher;
		teachersTableView->model.items.insert(pair, str);
		
		//new (start)
		QList<int> tmpActivities;
		QMap<LocaleString, int> tmpSubjectsNumberOfHours;		//using map, because it sorts alphabetically
		QMap<LocaleString, int> tmpStudentsNumberOfHours;		//using map, because it sorts alphabetically
		tmpActivities.clear();
		tmpActivities=teachersActivities.values(statisticValues.allTeachersNames.at(teacher));
		for(int aidx : std::as_const(tmpActivities)){
			Activity* act=gt.rules.activitiesList.at(aidx);
			//students start
			int tmpD=act->duration;
			QString tmpIdent;
			if(act->studentsNames.size()>0){
				for(QStringList::const_iterator it=act->studentsNames.constBegin(); it!=act->studentsNames.constEnd(); it++){
					tmpIdent+=*it;
					if(it!=act->studentsNames.constEnd()-1)
						tmpIdent+=translatedCommaSpace();
				}
			}
			tmpIdent+=" ";
			if(showActivityTags->checkState()==Qt::Checked){
				if(act->activityTagsNames.size()>0){
					for(QStringList::const_iterator it=act->activityTagsNames.constBegin(); it!=act->activityTagsNames.constEnd(); it++){
						tmpIdent+=*it;
						if(it!=act->activityTagsNames.constEnd()-1)
							tmpIdent+=translatedCommaSpace();
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
					for(QStringList::const_iterator it=act->activityTagsNames.constBegin(); it!=act->activityTagsNames.constEnd(); it++){
						tmpIdent+=*it;
						if(it!=act->activityTagsNames.constEnd()-1)
							tmpIdent+=translatedCommaSpace();
					}
				}
			}
			tmpD+=tmpSubjectsNumberOfHours.value(tmpIdent);
			tmpSubjectsNumberOfHours.insert(tmpIdent, tmpD);
			//subject end
		}
		
		QString tmpItem;
		tmpItem.clear();
		QMap<LocaleString, int>::const_iterator it=tmpSubjectsNumberOfHours.constBegin();
		while(it!=tmpSubjectsNumberOfHours.constEnd()){
			tmpItem+=CustomFETString::number(it.value())+" "+it.key()+"\n";
			it++;
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
		QMap<LocaleString, int>::const_iterator it2=tmpStudentsNumberOfHours.constBegin();
		while(it2!=tmpStudentsNumberOfHours.constEnd()){
			tmpItem+=CustomFETString::number(it2.value())+" "+it2.key()+"\n";
			it2++;
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
	//teachersTableView->hideRow(0);
	//hideUsedTeachers->hide();
	//teachersTargetNumberOfHours; Delete this lines! (end)
	
	teachersTableView->allTableChanged();
}

void ActivityPlanningForm::updateTablesVisual(){
	assert(studentsDuplicates.count()==statisticValues.allStudentsNames.count());
	assert(studentsDuplicates.count()==yearORgroupORsubgroup.count());
	
	activitiesTableView->setUpdatesEnabled(false);
	teachersTableView->setUpdatesEnabled(false);

	//(I hope) no performance slowdown, because of implicit sharing
	/*QHash<QPair<int, int>, QString> tItems=activitiesTableView->model.items;
	QStringList tHorizontalHeaderItems=activitiesTableView->model.horizontalHeaderItems;
	QStringList tVerticalHeaderItems=activitiesTableView->model.verticalHeaderItems;
	int trc=activitiesTableView->model.rowCount();
	int tcc=activitiesTableView->model.columnCount();
	activitiesTableView->model.clear();
	activitiesTableView->model.resize(trc, tcc);*/

	if(swapAxes->checkState()==Qt::Checked){
		for(int students=0; students<statisticValues.allStudentsNames.count(); students++){
			bool show=true;
			int tmpInt=yearORgroupORsubgroup.at(students);
			if(!SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING)
				assert(tmpInt!=STUDENTS_SUBGROUP);
			switch(tmpInt){
				case STUDENTS_YEAR:     if(showYears->checkState()!=Qt::Checked) show=false; break;
				case STUDENTS_GROUP:    if(showGroups->checkState()!=Qt::Checked) show=false; break;
				case STUDENTS_SUBGROUP: if(showSubgroups->checkState()!=Qt::Checked) show=false; break;
				default: assert(0==1);
			}
			if((studentsDuplicates.at(students)) && (showDuplicates->checkState()!=Qt::Checked)){
				show=false;
			}
			if(statisticValues.studentsTotalNumberOfHours.value(statisticValues.allStudentsNames.at(students))==0 && statisticValues.studentsTotalNumberOfHours2.value(statisticValues.allStudentsNames.at(students))==0 && hideEmptyLines->checkState()==Qt::Checked){
				show=false;
			}
			if(show)
				activitiesTableView->showColumn(students);
			else
				activitiesTableView->hideColumn(students);
		}
		for(int subject=0; subject<statisticValues.allSubjectsNames.count(); subject++){
			bool show=true;
			if(statisticValues.subjectsTotalNumberOfHours.value(statisticValues.allSubjectsNames.at(subject))==0 && statisticValues.subjectsTotalNumberOfHours4.value(statisticValues.allSubjectsNames.at(subject))==0 && hideEmptyLines->checkState()==Qt::Checked){
				show=false;
			}
			if(show)
				activitiesTableView->showRow(subject);
			else
				activitiesTableView->hideRow(subject);
		}
	} else {
		for(int students=0; students<statisticValues.allStudentsNames.count(); students++){
			bool show=true;
			int tmpInt=yearORgroupORsubgroup.at(students);
			if(!SHOW_SUBGROUPS_IN_ACTIVITY_PLANNING)
				assert(tmpInt!=STUDENTS_SUBGROUP);
			switch(tmpInt){
				case STUDENTS_YEAR:     if(showYears->checkState()!=Qt::Checked) show=false; break;
				case STUDENTS_GROUP:    if(showGroups->checkState()!=Qt::Checked) show=false; break;
				case STUDENTS_SUBGROUP: if(showSubgroups->checkState()!=Qt::Checked) show=false; break;
				default: assert(0==1);
			}
			if((studentsDuplicates.at(students)) && (showDuplicates->checkState()!=Qt::Checked)){
				show=false;
			}
			if(statisticValues.studentsTotalNumberOfHours.value(statisticValues.allStudentsNames.at(students))==0 && statisticValues.studentsTotalNumberOfHours2.value(statisticValues.allStudentsNames.at(students))==0 && hideEmptyLines->checkState()==Qt::Checked){
				show=false;
			}
			
			if(show)
				activitiesTableView->showRow(students);
			else
				activitiesTableView->hideRow(students);
		}
		for(int subject=0; subject<statisticValues.allSubjectsNames.count(); subject++){
			bool show=true;
			if(statisticValues.subjectsTotalNumberOfHours.value(statisticValues.allSubjectsNames.at(subject))==0 && statisticValues.subjectsTotalNumberOfHours4.value(statisticValues.allSubjectsNames.at(subject))==0 && hideEmptyLines->checkState()==Qt::Checked){
				show=false;
			}
			
			if(show)
				activitiesTableView->showColumn(subject);
			else
				activitiesTableView->hideColumn(subject);
		}
	}

	/*activitiesTableView->model.items=tItems;
	activitiesTableView->model.horizontalHeaderItems=tHorizontalHeaderItems;
	activitiesTableView->model.verticalHeaderItems=tVerticalHeaderItems;
	activitiesTableView->allTableChanged();*/
	
	
	//(I hope) no performance slowdown, because of implicit sharing
	/*tItems=teachersTableView->model.items;
	tHorizontalHeaderItems=teachersTableView->model.horizontalHeaderItems;
	tVerticalHeaderItems=teachersTableView->model.verticalHeaderItems;
	trc=teachersTableView->model.rowCount();
	tcc=teachersTableView->model.columnCount();
	teachersTableView->model.clear();
	teachersTableView->model.resize(trc, tcc);*/
	
	for(int teacher=0; teacher<statisticValues.allTeachersNames.count(); teacher++){
		bool show=true;
		if((teachersTargetNumberOfHours.at(teacher)-statisticValues.teachersTotalNumberOfHours.value(statisticValues.allTeachersNames.at(teacher)))==0
		 /*&& (teachersTargetNumberOfHours.at(teacher)-statisticValues.teachersTotalNumberOfHours2.value(statisticValues.allTeachersNames.at(teacher)))==0*/
		 && hideUsedTeachers->checkState()==Qt::Checked){
			show=false;
		}
		if(show)
			teachersTableView->showColumn(teacher);
		else
			teachersTableView->hideColumn(teacher);
	}

	/*teachersTableView->model.items=tItems;
	teachersTableView->model.horizontalHeaderItems=tHorizontalHeaderItems;
	teachersTableView->model.verticalHeaderItems=tVerticalHeaderItems;
	teachersTableView->allTableChanged();*/
	
	/*activitiesTable->resizeColumnsToContents();
	activitiesTable->resizeRowsToContents();*/
	activitiesTableView->resizeToFit();

	/*teachersTable->resizeColumnsToContents();
	teachersTable->resizeRowsToContents();*/
	teachersTableView->resizeToFit();

	activitiesTableView->setUpdatesEnabled(true);
	teachersTableView->setUpdatesEnabled(true);
}

void ActivityPlanningForm::deleteAll(){
	int ret=QMessageBox::question(this, tr("Delete all?", "It refers to activities"), tr("Are you sure you want to remove ALL the %1 activities and related constraints?", "%1 is the total number of activities")
	 .arg(gt.rules.activitiesList.count()), QMessageBox::Yes | QMessageBox::No);
	if(ret==QMessageBox::Yes){
		ret=QMessageBox::question(this, tr("Delete all?", "It refers to activities"), tr("Are you absolutely sure you want to remove ALL activities and related constraints from your data?"), QMessageBox::Yes | QMessageBox::No);
		if(ret==QMessageBox::Yes){
			QList<int> idsToBeRemoved;
			for(Activity* act : std::as_const(gt.rules.activitiesList))
				idsToBeRemoved.append(act->id);
			gt.rules.removeActivities(idsToBeRemoved, true);
		
			/*while(!gt.rules.activitiesList.isEmpty()){
				Activity* act=gt.rules.activitiesList.at(0);
				gt.rules.removeActivity(act->id, act->activityGroupId);
			}*/
			
			if(idsToBeRemoved.count()>0)
				gt.rules.addUndoPoint(tr("Activity planning: Deleted all the activities."));

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
	LongTextMessageBox::mediumInformation(this, tr("Information about pseudo activities", "Pseudo activities means activities without teachers and/or students sets"), noTeachersAndStudents+noTeachers+noStudents);
}

//communication spin box by Liviu Lalescu

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
	
	Q_EMIT valueChanged(value);
}

void PlanningChanged::increasePlanningCommunicationSpinBox()
{
	planningCommunicationSpinBox.increaseValue();
}
