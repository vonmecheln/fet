/***************************************************************************
                          timetableviewstudentsdayshorizontalform.cpp  -  description
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

#include <Qt>

#include <QtGlobal>

#include "longtextmessagebox.h"

#include "fetmainform.h"
#include "timetableviewstudentsdayshorizontalform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"
#include "solution.h"

#include "matrix.h"

#include "lockunlock.h"

#include <QList>
#include <QSet>

//std::stable_sort
#include <algorithm>

#include <QMessageBox>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include <QAbstractItemView>

#include <QCoreApplication>
#include <QApplication>
#include <QGuiApplication>

#include <QString>
#include <QStringList>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

//begin by Marco Vassura
#include <QBrush>
#include <QColor>
//end by Marco Vassura

extern const QString COMPANY;
extern const QString PROGRAM;

extern bool students_schedule_ready;
extern bool teachers_schedule_ready;

extern bool generation_running;
extern bool generation_running_multi;

extern Solution best_solution;

extern Matrix3D<bool> subgroupNotAvailableDayHour;
extern Matrix2D<bool> breakDayHour;

extern QSet<int> idsOfLockedTime;		//care about locked activities in view forms
extern QSet<int> idsOfLockedSpace;		//care about locked activities in view forms
extern QSet<int> idsOfPermanentlyLockedTime;	//care about locked activities in view forms
extern QSet<int> idsOfPermanentlyLockedSpace;	//care about locked activities in view forms

extern CommunicationSpinBox communicationSpinBox;	//small hint to sync the forms

static bool REAL_VIEW=true;

void TimetableViewStudentsDaysHorizontalDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	int hour=index.row()%nRows;
	int hourPlusSpan=hour;
	if(tableView!=nullptr)
		hourPlusSpan+=tableView->rowSpan(index.row(), index.column())-1;

	if(QGuiApplication::isLeftToRight()){
		if(hour==0){
			painter->drawLine(option.rect.topLeft(), option.rect.topRight());
			painter->drawLine(option.rect.topLeft().x(), option.rect.topLeft().y()+1, option.rect.topRight().x(), option.rect.topRight().y()+1);
		}
		if(hourPlusSpan==nRows-1){
			painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
			painter->drawLine(option.rect.bottomLeft().x(), option.rect.bottomLeft().y()-1, option.rect.bottomRight().x(), option.rect.bottomRight().y()-1);
		}

		if(index.column()==0){
			painter->drawLine(option.rect.topLeft(), option.rect.bottomLeft());
			painter->drawLine(option.rect.topLeft().x()+1, option.rect.topLeft().y(), option.rect.bottomLeft().x()+1, option.rect.bottomLeft().y());
		}
		if(index.column()==nColumns-1){
			painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
			painter->drawLine(option.rect.topRight().x()-1, option.rect.topRight().y(), option.rect.bottomRight().x()-1, option.rect.bottomRight().y());
		}
	}
	else if(QGuiApplication::isRightToLeft()){
		if(hour==0){
			painter->drawLine(option.rect.topRight(), option.rect.topLeft());
			painter->drawLine(option.rect.topRight().x(), option.rect.topRight().y()+1, option.rect.topLeft().x(), option.rect.topLeft().y()+1);
		}
		if(hourPlusSpan==nRows-1){
			painter->drawLine(option.rect.bottomRight(), option.rect.bottomLeft());
			painter->drawLine(option.rect.bottomRight().x(), option.rect.bottomRight().y()-1, option.rect.bottomLeft().x(), option.rect.bottomLeft().y()-1);
		}

		if(index.column()==0){
			painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
			painter->drawLine(option.rect.topRight().x()-1, option.rect.topRight().y(), option.rect.bottomRight().x()-1, option.rect.bottomRight().y());
		}
		if(index.column()==nColumns-1){
			painter->drawLine(option.rect.topLeft(), option.rect.bottomLeft());
			painter->drawLine(option.rect.topLeft().x()+1, option.rect.topLeft().y(), option.rect.bottomLeft().x()+1, option.rect.bottomLeft().y());
		}
	}
	//I think we should not do an 'else {assert(0);}' here, because the layout might be unspecified, according to Qt documentation.
}

TimetableViewStudentsDaysHorizontalForm::TimetableViewStudentsDaysHorizontalForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);
	
	detailsTextEdit->setReadOnly(true);

	//columnResizeModeInitialized=false;

	verticalStudentsTableDetailsSplitter->setStretchFactor(0, 4);
	verticalStudentsTableDetailsSplitter->setStretchFactor(1, 1);
	horizontalSplitter->setStretchFactor(0, 3);
	horizontalSplitter->setStretchFactor(1, 10);
	
	tableViewSetHighlightHeader(studentsTimetableTable);
	
	studentsTimetableTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	
	yearsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	groupsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	subgroupsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	groupsListWidget->clear();
	subgroupsListWidget->clear();
	
	//These connect-s should be lower in the code
	//connect(yearsListWidget, SIG NAL(currentTextChanged(const QString&)), this, SL OT(yearChanged(const QString&)));
	//connect(groupsListWidget, SIG NAL(currentTextChanged(const QString&)), this, SL OT(groupChanged(const QString&)));
	//connect(subgroupsListWidget, SIG NAL(currentTextChanged(const QString&)), this, SL OT(subgroupChanged(const QString&)));
	connect(closePushButton, &QPushButton::clicked, this, &TimetableViewStudentsDaysHorizontalForm::close);
	connect(studentsTimetableTable, &QTableWidget::currentItemChanged, this, &TimetableViewStudentsDaysHorizontalForm::currentItemChanged);
	connect(lockTimePushButton, &QPushButton::clicked, this, &TimetableViewStudentsDaysHorizontalForm::lockTime);
	connect(lockSpacePushButton, &QPushButton::clicked, this, &TimetableViewStudentsDaysHorizontalForm::lockSpace);
	connect(lockTimeSpacePushButton, &QPushButton::clicked, this, &TimetableViewStudentsDaysHorizontalForm::lockTimeSpace);
	connect(helpPushButton, &QPushButton::clicked, this, &TimetableViewStudentsDaysHorizontalForm::help);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	//restore vertical students list splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/vertical-students-list-splitter-state")))
		verticalStudentsListSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/vertical-students-list-splitter-state")).toByteArray());

	//restore vertical students table details splitter state
	//QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/vertical-students-table-details-splitter-state")))
		verticalStudentsTableDetailsSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/vertical-students-table-details-splitter-state")).toByteArray());

	//restore horizontal splitter state
	//QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/horizontal-splitter-state")))
		horizontalSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/horizontal-splitter-state")).toByteArray());

	if(settings.contains(this->metaObject()->className()+QString("/show-teachers")))
		teachersCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-teachers")).toBool());
	else
		teachersCheckBox->setChecked(true);

	if(settings.contains(this->metaObject()->className()+QString("/show-students")))
		studentsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-students")).toBool());
	else
		studentsCheckBox->setChecked(true);

	if(settings.contains(this->metaObject()->className()+QString("/show-subjects")))
		subjectsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-subjects")).toBool());
	else
		subjectsCheckBox->setChecked(true);

	if(settings.contains(this->metaObject()->className()+QString("/show-activity-tags")))
		activityTagsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-activity-tags")).toBool());
	else
		activityTagsCheckBox->setChecked(true);

	if(settings.contains(this->metaObject()->className()+QString("/show-rooms")))
		roomsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-rooms")).toBool());
	else
		roomsCheckBox->setChecked(true);

	if(settings.contains(this->metaObject()->className()+QString("/use-colors")))
		colorsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/use-colors")).toBool());
	else
		colorsCheckBox->setChecked(false);

	if(settings.contains(this->metaObject()->className()+QString("/use-span")))
		spanCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/use-span")).toBool());
	else
		spanCheckBox->setChecked(true);

	connect(teachersCheckBox, &QCheckBox::toggled, this, &TimetableViewStudentsDaysHorizontalForm::updateStudentsTimetableTable);
	connect(studentsCheckBox, &QCheckBox::toggled, this, &TimetableViewStudentsDaysHorizontalForm::updateStudentsTimetableTable);
	connect(subjectsCheckBox, &QCheckBox::toggled, this, &TimetableViewStudentsDaysHorizontalForm::updateStudentsTimetableTable);
	connect(activityTagsCheckBox, &QCheckBox::toggled, this, &TimetableViewStudentsDaysHorizontalForm::updateStudentsTimetableTable);
	connect(roomsCheckBox, &QCheckBox::toggled, this, &TimetableViewStudentsDaysHorizontalForm::updateStudentsTimetableTable);

	connect(colorsCheckBox, &QCheckBox::toggled, this, &TimetableViewStudentsDaysHorizontalForm::updateStudentsTimetableTable);

	connect(spanCheckBox, &QCheckBox::toggled, this, &TimetableViewStudentsDaysHorizontalForm::updateStudentsTimetableTable);

//////////just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;

	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;

	//added by Volker Dirr
	//this line is not really needed - just to be safer
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	
	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
//////////

	//Commented on 2018-07-20
	//LockUnlock::increaseCommunicationSpinBox();

	bool realView=(gt.rules.mode==MORNINGS_AFTERNOONS && REAL_VIEW==true);

	if(!realView){
		studentsTimetableTable->setRowCount(gt.rules.nHoursPerDay);
		studentsTimetableTable->setColumnCount(gt.rules.nDaysPerWeek);
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.daysOfTheWeek[j]);
			studentsTimetableTable->setHorizontalHeaderItem(j, item);
		}
		for(int i=0; i<gt.rules.nHoursPerDay; i++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.hoursOfTheDay[i]);
			studentsTimetableTable->setVerticalHeaderItem(i, item);
		}
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				QTableWidgetItem* item=new QTableWidgetItem();
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

				studentsTimetableTable->setItem(j, k, item);

				//if(j==0 && k==0)
				//	teachersTimetableTable->setCurrentItem(item);
			}
		}
	}
	else{
		studentsTimetableTable->setRowCount(gt.rules.nRealHoursPerDay);
		studentsTimetableTable->setColumnCount(gt.rules.nRealDaysPerWeek);

		for(int j=0; j<gt.rules.nRealDaysPerWeek; j++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.realDaysOfTheWeek[j]);
			studentsTimetableTable->setHorizontalHeaderItem(j, item);
		}
		for(int i=0; i<gt.rules.nRealHoursPerDay; i++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.realHoursOfTheDay[i]);
			studentsTimetableTable->setVerticalHeaderItem(i, item);
		}
		for(int j=0; j<gt.rules.nRealHoursPerDay; j++){
			for(int k=0; k<gt.rules.nRealDaysPerWeek; k++){
				QTableWidgetItem* item=new QTableWidgetItem();
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

				studentsTimetableTable->setItem(j, k, item);

				//if(j==0 && k==0)
				//	teachersTimetableTable->setCurrentItem(item);
			}
		}
	}
	
	oldItemDelegate=studentsTimetableTable->itemDelegate();
	newItemDelegate=new TimetableViewStudentsDaysHorizontalDelegate(nullptr, studentsTimetableTable, gt.rules.nHoursPerDay, studentsTimetableTable->columnCount());
	studentsTimetableTable->setItemDelegate(newItemDelegate);

	//resize columns
	//if(!columnResizeModeInitialized){
	studentsTimetableTable->horizontalHeader()->setMinimumSectionSize(studentsTimetableTable->horizontalHeader()->defaultSectionSize());
	//	columnResizeModeInitialized=true;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	studentsTimetableTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
	studentsTimetableTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
	//}
	///////////////
	
	//2019-12-11: so that the years' and groups' list widgets are not void, even if selecting to view by groups or subgroups.
	yearsListWidget->clear();
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		yearsListWidget->addItem(sty->name);
	}
	if(yearsListWidget->count()>0){
		yearsListWidget->setCurrentRow(0);
		
		groupsListWidget->clear();
		for(StudentsGroup* stg : std::as_const(gt.rules.augmentedYearsList[0]->groupsList))
			groupsListWidget->addItem(stg->name);
		if(groupsListWidget->count()>0){
			groupsListWidget->setCurrentRow(0);
			
			subgroupsListWidget->clear();
			for(StudentsSubgroup* sts : std::as_const(gt.rules.augmentedYearsList[0]->groupsList.at(0)->subgroupsList))
				subgroupsListWidget->addItem(sts->name);
			if(subgroupsListWidget->count()>0)
				subgroupsListWidget->setCurrentRow(0);
		}
	}
	connect(yearsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::yearChanged);
	connect(groupsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::groupChanged);
	connect(subgroupsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::subgroupChanged);
	
	shownComboBox->addItem(tr("Years"));
	shownComboBox->addItem(tr("Groups"));
	shownComboBox->addItem(tr("Subgroups"));
	shownComboBox->addItem(tr("Subgroups (sorted)"));

	shownComboBox->setCurrentIndex(-1);

	if(settings.contains(this->metaObject()->className()+QString("/shown-categories")))
		shownComboBox->setCurrentIndex(settings.value(this->metaObject()->className()+QString("/shown-categories")).toInt());
	else
		shownComboBox->setCurrentIndex(0);

	connect(shownComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &TimetableViewStudentsDaysHorizontalForm::shownComboBoxChanged);

	//added by Volker Dirr
	connect(&communicationSpinBox, &CommunicationSpinBox::valueChanged, this, &TimetableViewStudentsDaysHorizontalForm::updateStudentsTimetableTable);

	shownComboBoxChanged();
}

void TimetableViewStudentsDaysHorizontalForm::newTimetableGenerated()
{
	/*setupUi(this);
	
	closePushButton->setDefault(true);
	
	detailsTextEdit->setReadOnly(true);

	//columnResizeModeInitialized=false;

	verticalStudentsTableDetailsSplitter->setStretchFactor(0, 4);
	verticalStudentsTableDetailsSplitter->setStretchFactor(1, 1);
	horizontalSplitter->setStretchFactor(0, 3);
	horizontalSplitter->setStretchFactor(1, 10);
	
	studentsTimetableTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	
	yearsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	groupsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	subgroupsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);*/

	/* commented out on 2024-08-31
	disconnect(groupsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::groupChanged);
	disconnect(subgroupsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::subgroupChanged);
	groupsListWidget->clear();
	subgroupsListWidget->clear();
	connect(groupsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::groupChanged);
	connect(subgroupsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::subgroupChanged);
	*/
	
	//This connect should be lower in the code
	//connect(yearsListWidget, SIG NAL(currentTextChanged(const QString&)), this, SL OT(yearChanged(const QString&)));
	/*connect(groupsListWidget, SIG NAL(currentTextChanged(const QString&)), this, SL OT(groupChanged(const QString&)));
	connect(subgroupsListWidget, SIG NAL(currentTextChanged(const QString&)), this, SL OT(subgroupChanged(const QString&)));
	connect(closePushButton, SIG NAL(clicked()), this, SL OT(close()));
	connect(studentsTimetableTable, SIG NAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SL OT(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)));
	connect(lockTimePushButton, SIG NAL(clicked()), this, SL OT(lockTime()));
	connect(lockSpacePushButton, SIG NAL(clicked()), this, SL OT(lockSpace()));
	connect(lockTimeSpacePushButton, SIG NAL(clicked()), this, SL OT(lockTimeSpace()));
	connect(helpPushButton, SIG NAL(clicked()), this, SL OT(help()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	//restore vertical students list splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/vertical-students-list-splitter-state")))
		verticalStudentsListSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/vertical-students-list-splitter-state")).toByteArray());

	//restore vertical students table details splitter state
	//QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/vertical-students-table-details-splitter-state")))
		verticalStudentsTableDetailsSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/vertical-students-table-details-splitter-state")).toByteArray());

	//restore horizontal splitter state
	//QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/horizontal-splitter-state")))
		horizontalSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/horizontal-splitter-state")).toByteArray());*/

//////////just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;

	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;

	//added by Volker Dirr
	//this line is not really needed - just to be safer
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace();
	
	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
//////////

	int r=studentsTimetableTable->currentRow();
	int c=studentsTimetableTable->currentColumn();

	//DON'T UNCOMMENT THIS CODE -> LEADS TO CRASH IF THERE ARE MORE VIEWS OPENED.
	//LockUnlock::increaseCommunicationSpinBox();

	bool realView=(gt.rules.mode==MORNINGS_AFTERNOONS && REAL_VIEW==true);

	if(!realView){
		studentsTimetableTable->clear();
		studentsTimetableTable->setRowCount(gt.rules.nHoursPerDay);
		studentsTimetableTable->setColumnCount(gt.rules.nDaysPerWeek);

		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.daysOfTheWeek[j]);
			studentsTimetableTable->setHorizontalHeaderItem(j, item);
		}
		for(int i=0; i<gt.rules.nHoursPerDay; i++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.hoursOfTheDay[i]);
			studentsTimetableTable->setVerticalHeaderItem(i, item);
		}
		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				QTableWidgetItem* item=new QTableWidgetItem();
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

				studentsTimetableTable->setItem(j, k, item);

				//if(j==0 && k==0)
				//	teachersTimetableTable->setCurrentItem(item);
			}
		}
	}
	else{
		studentsTimetableTable->clear();
		studentsTimetableTable->setRowCount(gt.rules.nRealHoursPerDay);
		studentsTimetableTable->setColumnCount(gt.rules.nRealDaysPerWeek);

		for(int j=0; j<gt.rules.nRealDaysPerWeek; j++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.realDaysOfTheWeek[j]);
			studentsTimetableTable->setHorizontalHeaderItem(j, item);
		}
		for(int i=0; i<gt.rules.nRealHoursPerDay; i++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.realHoursOfTheDay[i]);
			studentsTimetableTable->setVerticalHeaderItem(i, item);
		}
		for(int j=0; j<gt.rules.nRealHoursPerDay; j++){
			for(int k=0; k<gt.rules.nRealDaysPerWeek; k++){
				QTableWidgetItem* item=new QTableWidgetItem();
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

				studentsTimetableTable->setItem(j, k, item);

				//if(j==0 && k==0)
				//	teachersTimetableTable->setCurrentItem(item);
			}
		}
	}
	
	newItemDelegate->nRows=gt.rules.nHoursPerDay;
	newItemDelegate->nColumns=studentsTimetableTable->columnCount();

	//resize columns
	//if(!columnResizeModeInitialized){
/*	studentsTimetableTable->horizontalHeader()->setMinimumSectionSize(studentsTimetableTable->horizontalHeader()->defaultSectionSize());
	//	columnResizeModeInitialized=true;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	studentsTimetableTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
	studentsTimetableTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
	//}
	///////////////
*/
	
	/* commented out on 2024-08-31
	disconnect(yearsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::yearChanged);
	yearsListWidget->clear();
	for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
		StudentsYear* sty=gt.rules.augmentedYearsList[i];
		yearsListWidget->addItem(sty->name);
	}
	if(yearsListWidget->count()>0)
		yearsListWidget->setCurrentRow(0);
	connect(yearsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::yearChanged);
	*/
	
/*	shownComboBox->addItem(tr("Years"));
	shownComboBox->addItem(tr("Groups"));
	shownComboBox->addItem(tr("Subgroups"));

	shownComboBox->setCurrentIndex(-1);

	if(settings.contains(this->metaObject()->className()+QString("/shown-categories")))
		shownComboBox->setCurrentIndex(settings.value(this->metaObject()->className()+QString("/shown-categories")).toInt());
	else
		shownComboBox->setCurrentIndex(0);

	connect(shownComboBox, SIG NAL(activated(QString)), this, SL OT(shownComboBoxChanged(QString)));
*/

	//added by Volker Dirr
	//connect(&communicationSpinBox, SIG NAL(valueChanged(int)), this, SL OT(updateStudentsTimetableTable()));

	int y=yearsListWidget->currentRow();
	int g=groupsListWidget->currentRow();
	int s=subgroupsListWidget->currentRow();
	
	shownComboBoxChanged();
	
	if(yearsListWidget->isVisible())
		if(y>=0 && y<yearsListWidget->count())
			yearsListWidget->setCurrentRow(y);
	if(groupsListWidget->isVisible())
		if(g>=0 && g<groupsListWidget->count())
			groupsListWidget->setCurrentRow(g);
	if(subgroupsListWidget->isVisible())
		if(s>=0 && s<subgroupsListWidget->count())
			subgroupsListWidget->setCurrentRow(s);
	
	if(r>=0 && r<studentsTimetableTable->rowCount() && c>=0 && c<studentsTimetableTable->columnCount())
		studentsTimetableTable->setCurrentCell(r, c);
}

TimetableViewStudentsDaysHorizontalForm::~TimetableViewStudentsDaysHorizontalForm()
{
	saveFETDialogGeometry(this);

	//save vertical students list splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/vertical-students-list-splitter-state"), verticalStudentsListSplitter->saveState());

	//save vertical students table details splitter state
	//QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/vertical-students-table-details-splitter-state"), verticalStudentsTableDetailsSplitter->saveState());

	//save horizontal splitter state
	//QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/horizontal-splitter-state"), horizontalSplitter->saveState());

	settings.setValue(this->metaObject()->className()+QString("/show-teachers"), teachersCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-students"), studentsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-subjects"), subjectsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-activity-tags"), activityTagsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-rooms"), roomsCheckBox->isChecked());

	settings.setValue(this->metaObject()->className()+QString("/use-colors"), colorsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/use-span"), spanCheckBox->isChecked());

	settings.setValue(this->metaObject()->className()+QString("/shown-categories"), shownComboBox->currentIndex());

	studentsTimetableTable->setItemDelegate(oldItemDelegate);
	delete newItemDelegate;
}

void TimetableViewStudentsDaysHorizontalForm::resizeRowsAfterShow()
{
	studentsTimetableTable->resizeRowsToContents();
}

void TimetableViewStudentsDaysHorizontalForm::shownComboBoxChanged()
{
	if(shownComboBox->currentIndex()==0){
		//years, groups, and subgroups shown
		yearsListWidget->show();
		groupsListWidget->show();
		subgroupsListWidget->show();
		
		disconnect(yearsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::yearChanged);
		yearsListWidget->clear();
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			yearsListWidget->addItem(sty->name);
		}
		if(yearsListWidget->count()>0)
			yearsListWidget->setCurrentRow(0);
		connect(yearsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::yearChanged);
		if(yearsListWidget->count()>0)
			yearChanged(yearsListWidget->item(0)->text());
	}
	else if(shownComboBox->currentIndex()==1){
		//only groups and subgroups shown
		yearsListWidget->hide();
		groupsListWidget->show();
		subgroupsListWidget->show();

		disconnect(groupsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::groupChanged);
		groupsListWidget->clear();
		QSet<QString> groupsSet;
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			for(StudentsGroup* stg : std::as_const(sty->groupsList))
				if(!groupsSet.contains(stg->name)){
					groupsListWidget->addItem(stg->name);
					groupsSet.insert(stg->name);
				}
		}
		if(groupsListWidget->count()>0)
			groupsListWidget->setCurrentRow(0);
		connect(groupsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::groupChanged);
		if(groupsListWidget->count()>0)
			groupChanged(groupsListWidget->item(0)->text());
	}
	else if(shownComboBox->currentIndex()==2){
		//only subgroups shown
		yearsListWidget->hide();
		groupsListWidget->hide();
		subgroupsListWidget->show();

		disconnect(subgroupsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::subgroupChanged);
		subgroupsListWidget->clear();
		QSet<QString> subgroupsSet;
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			for(StudentsGroup* stg : std::as_const(sty->groupsList))
				for(StudentsSubgroup* sts : std::as_const(stg->subgroupsList))
					if(!subgroupsSet.contains(sts->name)){
						subgroupsListWidget->addItem(sts->name);
						subgroupsSet.insert(sts->name);
					}
		}
		if(subgroupsListWidget->count()>0)
			subgroupsListWidget->setCurrentRow(0);
		connect(subgroupsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::subgroupChanged);
		if(subgroupsListWidget->count()>0)
			subgroupChanged(subgroupsListWidget->item(0)->text());
	}
	else if(shownComboBox->currentIndex()==3){
		//only subgroups shown, sorted alphabetically.
		yearsListWidget->hide();
		groupsListWidget->hide();
		subgroupsListWidget->show();
		
		disconnect(subgroupsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::subgroupChanged);
		subgroupsListWidget->clear();
		
		QSet<QString> subgroupsSet;
		QList<StudentsSubgroup*> lst;
		for(int i=0; i<gt.rules.augmentedYearsList.size(); i++){
			StudentsYear* sty=gt.rules.augmentedYearsList[i];
			for(StudentsGroup* stg : std::as_const(sty->groupsList))
				for(StudentsSubgroup* sts : std::as_const(stg->subgroupsList))
					if(!subgroupsSet.contains(sts->name)){
						lst.append(sts);
						subgroupsSet.insert(sts->name);
					}
		}
		std::stable_sort(lst.begin(), lst.end(), subgroupsAscending);
		for(StudentsSubgroup* s : std::as_const(lst))
			subgroupsListWidget->addItem(s->name);
		
		if(subgroupsListWidget->count()>0)
			subgroupsListWidget->setCurrentRow(0);
		connect(subgroupsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::subgroupChanged);
		if(subgroupsListWidget->count()>0)
			subgroupChanged(subgroupsListWidget->item(0)->text());
	}
	else{
		assert(0);
	}
}

void TimetableViewStudentsDaysHorizontalForm::yearChanged(const QString& yearName)
{
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);

	if(yearName==QString())
		return;
	int yearIndex=gt.rules.searchAugmentedYear(yearName);
	if(yearIndex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid year - please close this dialog and open a new students view timetable dialog"));
		return;
	}

	disconnect(groupsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::groupChanged);

	groupsListWidget->clear();
	StudentsYear* sty=gt.rules.augmentedYearsList.at(yearIndex);
	for(int i=0; i<sty->groupsList.size(); i++){
		StudentsGroup* stg=sty->groupsList[i];
		groupsListWidget->addItem(stg->name);
	}

	connect(groupsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::groupChanged);

	if(groupsListWidget->count()>0)
		groupsListWidget->setCurrentRow(0);
}

void TimetableViewStudentsDaysHorizontalForm::groupChanged(const QString& groupName)
{
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);

	if(groupName==QString())
		return;
	
	//StudentsSet* ss=gt.rules.searchAugmentedStudentsSet(groupName);
	StudentsSet* ss=gt.rules.studentsHash.value(groupName, nullptr);
	if(ss==nullptr){
		QMessageBox::warning(this, tr("FET warning"), tr("Nonexistent group - please reload this dialog"));
		return;
	}
	if(ss->type!=STUDENTS_GROUP){
		QMessageBox::warning(this, tr("FET warning"), tr("Incorrect group settings - please reload this dialog"));
		return;
	}
	
	StudentsGroup* stg=(StudentsGroup*)ss;
	
	/*QString yearName=yearsListWidget->currentItem()->text();
	int yearIndex=gt.rules.searchAugmentedYear(yearName);
	if(yearIndex<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid year - please close this dialog and open a new students view timetable dialog"));
		return;
	}

	StudentsYear* sty=gt.rules.augmentedYearsList.at(yearIndex);
	int groupIndex=gt.rules.searchAugmentedGroup(yearName, groupName);
	if(groupIndex<0){
		QMessageBox::warning(this, tr("FET warning"),
		 tr("Invalid group in the selected year, or the groups of the current year are not updated")+
		 "\n\n"+
		 tr("Solution: please try to select a different year and after that select the current year again, "
		 "to refresh the groups list, or close this dialog and open again the students view timetable dialog"));
		return;
	}*/
	
	disconnect(subgroupsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::subgroupChanged);

	subgroupsListWidget->clear();
	
	//StudentsGroup* stg=sty->groupsList.at(groupIndex);
	for(int i=0; i<stg->subgroupsList.size(); i++){
		StudentsSubgroup* sts=stg->subgroupsList[i];
		subgroupsListWidget->addItem(sts->name);
	}

	connect(subgroupsListWidget, &QListWidget::currentTextChanged, this, &TimetableViewStudentsDaysHorizontalForm::subgroupChanged);

	if(subgroupsListWidget->count()>0)
		subgroupsListWidget->setCurrentRow(0);
}

void TimetableViewStudentsDaysHorizontalForm::subgroupChanged(const QString& subgroupName)
{
	Q_UNUSED(subgroupName);
	
	updateStudentsTimetableTable();
}

void TimetableViewStudentsDaysHorizontalForm::updateStudentsTimetableTable(){
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable "
		 "or close the timetable view students dialog"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);
	
	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	QString s;
	//QString yearname;
	//QString groupname;
	QString subgroupname;

	/*if(yearsListWidget->currentRow()<0 || yearsListWidget->currentRow()>=yearsListWidget->count())
		return;
	if(groupsListWidget->currentRow()<0 || groupsListWidget->currentRow()>=groupsListWidget->count())
		return;
	if(subgroupsListWidget->currentRow()<0 || subgroupsListWidget->currentRow()>=subgroupsListWidget->count())
		return;*/

	//yearname = yearsListWidget->currentItem()->text();
	//groupname = groupsListWidget->currentItem()->text();
	subgroupname = subgroupsListWidget->currentItem()->text();

	//StudentsSet* ss=gt.rules.searchAugmentedStudentsSet(subgroupname);
	StudentsSet* ss=gt.rules.studentsHash.value(subgroupname, nullptr);
	if(ss==nullptr){
		QMessageBox::information(this, tr("FET warning"), tr("Nonexistent subgroup - please reload this dialog"));
		return;
	}
	if(ss->type!=STUDENTS_SUBGROUP){
		QMessageBox::warning(this, tr("FET warning"), tr("Incorrect subgroup settings - please reload this dialog"));
		return;
	}
	StudentsSubgroup* sts=(StudentsSubgroup*)ss;

	s="";
	s += subgroupname;

	classNameTextLabel->setText(s);

	assert(gt.rules.initialized);

	assert(sts!=nullptr);
	int i;
	/*for(i=0; i<gt.rules.nInternalSubgroups; i++)
		if(gt.rules.internalSubgroupsList[i]==sts)
			break;
	assert(i==sts->indexInInternalSubgroupsList);*/
	i=sts->indexInInternalSubgroupsList;
	assert(i>=0);
	assert(i<gt.rules.nInternalSubgroups);
	
	bool realView=(gt.rules.mode==MORNINGS_AFTERNOONS && REAL_VIEW==true);
	bool normalView=!realView;

	int numberOfActivities=0;
	int durationOfActivities=0;

	studentsTimetableTable->clearSpans();
	
	for(int j=0; j<(normalView?gt.rules.nHoursPerDay:gt.rules.nRealHoursPerDay) && j<studentsTimetableTable->rowCount(); j++){
		int jj;
		if(normalView)
			jj=j;
		else
			jj=j%gt.rules.nHoursPerDay;
		
		for(int k=0; k<(normalView?gt.rules.nDaysPerWeek:gt.rules.nRealDaysPerWeek) && k<studentsTimetableTable->columnCount(); k++){
			int kk;
			if(normalView)
				kk=k;
			else
				kk=2*k+j/gt.rules.nHoursPerDay;

			QFont font(studentsTimetableTable->item(j, k)->font());
			font.setBold(false);
			font.setItalic(false);
			font.setUnderline(false);
			studentsTimetableTable->item(j, k)->setFont(font);

			//begin by Marco Vassura
			// add colors (start)
			//if(USE_GUI_COLORS) {
			studentsTimetableTable->item(j, k)->setBackground(studentsTimetableTable->palette().base());
			studentsTimetableTable->item(j, k)->setForeground(studentsTimetableTable->palette().text());
			//}
			// add colors (end)
			//end by Marco Vassura
			s="";
			QString longString="";
			int ai=students_timetable_weekly[i][kk][jj]; //activity index
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				assert(act!=nullptr);

				if(best_solution.times[ai]/gt.rules.nDaysPerWeek==jj){
					assert(best_solution.times[ai]%gt.rules.nDaysPerWeek==kk);

					numberOfActivities++;
					durationOfActivities+=act->duration;

					if(spanCheckBox->isChecked() && act->duration>=2)
						studentsTimetableTable->setSpan(j, k, act->duration, 1);
				}

				if(studentsCheckBox->isChecked()){
					assert(act->studentsNames.count()>=1);
					if((act->studentsNames.count()==1 && act->studentsNames.at(0)!=subgroupname) || act->studentsNames.count()>=2){
						s+=act->studentsNames.join(translatedCommaSpace());
						s+="\n";
					}
				}
				
				if(subjectsCheckBox->isChecked()){
					if(activityTagsCheckBox->isChecked() && act->activityTagsNames.count()>0){
						QString ats=act->activityTagsNames.join(translatedCommaSpace());
						s+=act->subjectName +" "+ ats;
					}
					else{
						s+=act->subjectName;
					}
					s+="\n";
				}
				else if(activityTagsCheckBox->isChecked()){
					s+=act->activityTagsNames.join(translatedCommaSpace());
					s+="\n";
				}
				
				if(teachersCheckBox->isChecked()){
					if(act->teachersNames.count()>0){
						s+=act->teachersNames.join(translatedCommaSpace());
					}
					s+="\n";
				}
				
				if(roomsCheckBox->isChecked()){
					int r=best_solution.rooms[ai];
					if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
						//s+=" ";
						//s+=tr("R:%1", "Room").arg(gt.rules.internalRoomsList[r]->name);
						s+=gt.rules.internalRoomsList[r]->name;
						
						if(gt.rules.internalRoomsList[r]->isVirtual==true){
							QStringList tsl;
							for(int i : std::as_const(best_solution.realRoomsList[ai]))
								tsl.append(gt.rules.internalRoomsList[i]->name);
							s+=QString(" (")+tsl.join(translatedCommaSpace())+QString(")");
						}
						s+="\n";
					}
				}
				
				if(!s.isEmpty())
					s.chop(1);
				
				//added by Volker Dirr (start)
				QString descr="";
				QString tt="";
				if(idsOfPermanentlyLockedTime.contains(act->id)){
					descr+=QCoreApplication::translate("TimetableViewForm", "PLT", "Abbreviation for permanently locked time. There are 5 strings: permanently locked time, permanently locked space, "
						"locked time, locked space, preferred day. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
						" These abbreviations may appear also in other places, please use the same abbreviations.");
					tt=", ";
				}
				else if(idsOfLockedTime.contains(act->id)){
					descr+=QCoreApplication::translate("TimetableViewForm", "LT", "Abbreviation for locked time. There are 5 strings: permanently locked time, permanently locked space, "
					"locked time, locked space, preferred day. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
						" These abbreviations may appear also in other places, please use the same abbreviations.");
					tt=", ";
				}
				if(idsOfPermanentlyLockedSpace.contains(act->id)){
					descr+=tt+QCoreApplication::translate("TimetableViewForm", "PLS", "Abbreviation for permanently locked space. There are 5 strings: permanently locked time, permanently locked space, "
						"locked time, locked space, preferred day. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
						" These abbreviations may appear also in other places, please use the same abbreviations.");
					tt=", ";
				}
				else if(idsOfLockedSpace.contains(act->id)){
					descr+=tt+QCoreApplication::translate("TimetableViewForm", "LS", "Abbreviation for locked space. There are 5 strings: permanently locked time, permanently locked space, "
						"locked time, locked space, preferred day. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
						" These abbreviations may appear also in other places, please use the same abbreviations.");
					tt=", ";
				}
				if(!gt.rules.apdHash.value(act->id, QSet<ConstraintActivityPreferredDay*>()).isEmpty()){
					descr+=tt+QCoreApplication::translate("TimetableViewForm", "PD", "Abbreviation for preferred day. There are 5 strings: permanently locked time, permanently locked space, "
						"locked time, locked space, preferred day. Make sure their abbreviations contain different letters and are visually different, so user can easily differentiate between them."
						" These abbreviations may appear also in other places, please use the same abbreviations.");
				}
				if(descr!=""){
					descr.prepend("\n(");
					//descr.prepend(" ");
					descr.append(")");
				}

				longString=s+descr;
				//added by Volker Dirr (end)

				if(idsOfPermanentlyLockedTime.contains(act->id) || idsOfLockedTime.contains(act->id)){
					QFont font(studentsTimetableTable->item(j, k)->font());
					font.setBold(true);
					studentsTimetableTable->item(j, k)->setFont(font);
				}
				if(idsOfPermanentlyLockedSpace.contains(act->id) || idsOfLockedSpace.contains(act->id)){
					QFont font(studentsTimetableTable->item(j, k)->font());
					font.setItalic(true);
					studentsTimetableTable->item(j, k)->setFont(font);
				}
				if(!gt.rules.apdHash.value(act->id, QSet<ConstraintActivityPreferredDay*>()).isEmpty()){
					QFont font(studentsTimetableTable->item(j, k)->font());
					font.setUnderline(true);
					studentsTimetableTable->item(j, k)->setFont(font);
				}

				//begin by Marco Vassura
				// add colors (start)
				if(colorsCheckBox->isChecked() /*USE_GUI_COLORS*/){
					QBrush bg(stringToColor(act->subjectName));
					studentsTimetableTable->item(j, k)->setBackground(bg);
					double brightness = bg.color().redF()*0.299 + bg.color().greenF()*0.587 + bg.color().blueF()*0.114;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
					if (brightness<0.5)
						studentsTimetableTable->item(j, k)->setForeground(QBrush(QColorConstants::White));
					else
						studentsTimetableTable->item(j, k)->setForeground(QBrush(QColorConstants::Black));
#else
					if (brightness<0.5)
						studentsTimetableTable->item(j, k)->setForeground(QBrush(Qt::white));
					else
						studentsTimetableTable->item(j, k)->setForeground(QBrush(Qt::black));
#endif
				}
				// add colors (end)
				//end by Marco Vassura
			}
			else{
				if(subgroupNotAvailableDayHour[i][kk][jj] && PRINT_NOT_AVAILABLE_TIME_SLOTS)
					s+="-x-";
				else if(breakDayHour[kk][jj] && PRINT_BREAK_TIME_SLOTS)
					s+="-X-";
			}
			studentsTimetableTable->item(j, k)->setText(s);
			studentsTimetableTable->item(j, k)->setToolTip(longString);
		}
	}

	numberAndDurationTextLabel->setText(tr("No: %1    Dur: %2", "No means number, %1 is the number of the placed activities,"
	 " Dur means duration, %2 is the duration of the placed activities. Note that there are four spaces between them.").arg(numberOfActivities).arg(durationOfActivities));

	studentsTimetableTable->resizeRowsToContents();
	
	detailActivity(studentsTimetableTable->currentItem());
}

//begin by Marco Vassura
//slightly modified by Liviu Lalescu on 2021-03-01
QColor TimetableViewStudentsDaysHorizontalForm::stringToColor(const QString& s)
{
	// CRC-24 based on RFC 2440 Section 6.1
	unsigned long int crc = 0xB704CEUL;
	QByteArray ba=s.toUtf8();
	for(char c : std::as_const(ba)){
		unsigned char uc=(unsigned char)(c);
		crc ^= (uc & 0xFF) << 16;
		for (int i = 0; i < 8; i++) {
			crc <<= 1;
			if (crc & 0x1000000UL)
				crc ^= 0x1864CFBUL;
		}
	}
	return QColor::fromRgb(int((crc>>16) & 0xFF), int((crc>>8) & 0xFF), int(crc & 0xFF));
}
//end by Marco Vassura

void TimetableViewStudentsDaysHorizontalForm::resizeEvent(QResizeEvent* event)
{
	QDialog::resizeEvent(event);

	studentsTimetableTable->resizeRowsToContents();
}

void TimetableViewStudentsDaysHorizontalForm::currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
{
	Q_UNUSED(previous);
	
	detailActivity(current);
}

void TimetableViewStudentsDaysHorizontalForm::detailActivity(QTableWidgetItem* item)
{
	if(item==nullptr){
		detailsTextEdit->setPlainText(QString(""));
		return;
	}
	
	bool realView=(gt.rules.mode==MORNINGS_AFTERNOONS && REAL_VIEW==true);
	bool normalView=!realView;

	if(item->row()>=(normalView?gt.rules.nHoursPerDay:gt.rules.nRealHoursPerDay) || item->column()>=(normalView?gt.rules.nDaysPerWeek:gt.rules.nRealDaysPerWeek)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable "
		 "or close the timetable view students dialog"));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	QString s;
	//QString yearname; //not really necessary
	//QString groupname; //not really necessary
	QString subgroupname;
	
	//Note: the years and the groups list widgets might contain useless information if the user selected to see only groups or subgroups.
	//However, in this case they are not void and the currentRow() will be 0 (first year) for the year list widget and, if the variant is
	//to show only the subgroups, the same for the groups list widget (initialized in the dialog's constructor).
	
	//if(yearsListWidget->currentRow()<0 || yearsListWidget->currentRow()>=yearsListWidget->count())
	//	return;
	//if(groupsListWidget->currentRow()<0 || groupsListWidget->currentRow()>=groupsListWidget->count())
	//	return;
	if(subgroupsListWidget->currentRow()<0 || subgroupsListWidget->currentRow()>=subgroupsListWidget->count())
		return;

	//yearname = yearsListWidget->currentItem()->text();
	//groupname = groupsListWidget->currentItem()->text();
	subgroupname = subgroupsListWidget->currentItem()->text();

	//StudentsSet* ss=gt.rules.searchAugmentedStudentsSet(subgroupname);
	StudentsSet* ss=gt.rules.studentsHash.value(subgroupname, nullptr);
	if(ss==nullptr){
		QMessageBox::information(this, tr("FET warning"), tr("Nonexistent subgroup - please reload this dialog"));
		return;
	}
	if(ss->type!=STUDENTS_SUBGROUP){
		QMessageBox::warning(this, tr("FET warning"), tr("Incorrect subgroup settings - please reload this dialog"));
		return;
	}
	StudentsSubgroup* sts=(StudentsSubgroup*)ss;

	/*StudentsSubgroup* sts=(StudentsSubgroup*)gt.rules.searchAugmentedStudentsSet(subgroupname);
	if(!sts){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid students set - please close this dialog and open a new view students timetable dialog"));
		return;
	}*/
	
	assert(sts!=nullptr);
	int i;
	/*for(i=0; i<gt.rules.nInternalSubgroups; i++)
		if(gt.rules.internalSubgroupsList[i]==sts)
			break;
	assert(i==sts->indexInInternalSubgroupsList);*/
/*	if(!(i<gt.rules.nInternalSubgroups)){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid students set - please close this dialog and open a new view students dialog"));
		return;
	}*/
	i=sts->indexInInternalSubgroupsList;
	assert(i>=0);
	assert(i<gt.rules.nInternalSubgroups);
	int j=item->row();
	int k=item->column();
	s="";
	if(j>=0 && k>=0){
		int jj;
		if(normalView)
			jj=j;
		else
			jj=j%gt.rules.nHoursPerDay;
		assert(jj>=0);
		
		int kk;
		if(normalView)
			kk=k;
		else
			kk=2*k+j/gt.rules.nHoursPerDay;
		assert(kk>=0);
		
		int ai=students_timetable_weekly[i][kk][jj]; //activity index
		//Activity* act=gt.rules.activitiesList.at(ai);
		if(ai!=UNALLOCATED_ACTIVITY){
			Activity* act=&gt.rules.internalActivitiesList[ai];
			assert(act!=nullptr);
			s+=act->getDetailedDescriptionWithConstraints(gt.rules);
			//s+=act->getDetailedDescription(gt.rules);

			int r=best_solution.rooms[ai];
			if(r!=UNALLOCATED_SPACE && r!=UNSPECIFIED_ROOM){
				s+="\n";
				s+=tr("Room: %1").arg(gt.rules.internalRoomsList[r]->name);

				if(gt.rules.internalRoomsList[r]->isVirtual==true){
					QStringList tsl;
					for(int i : std::as_const(best_solution.realRoomsList[ai]))
						tsl.append(gt.rules.internalRoomsList[i]->name);
					s+=QString(" (")+tsl.join(translatedCommaSpace())+QString(")");
				}

				if(gt.rules.internalRoomsList[r]->building!=""){
					s+="\n";
					s+=tr("Building=%1").arg(gt.rules.internalRoomsList[r]->building);
				}
				s+="\n";
				s+=tr("Capacity=%1").arg(gt.rules.internalRoomsList[r]->capacity);
			}
			//added by Volker Dirr (start)
			QString descr="";
			QString t="";
			if(idsOfPermanentlyLockedTime.contains(act->id)){
				descr+=QCoreApplication::translate("TimetableViewForm", "permanently locked time", "refers to activity");
				t=", ";
			}
			else if(idsOfLockedTime.contains(act->id)){
				descr+=QCoreApplication::translate("TimetableViewForm", "locked time", "refers to activity");
				t=", ";
			}
			if(idsOfPermanentlyLockedSpace.contains(act->id)){
				descr+=t+QCoreApplication::translate("TimetableViewForm", "permanently locked space", "refers to activity");
				t=", ";
			}
			else if(idsOfLockedSpace.contains(act->id)){
				descr+=t+QCoreApplication::translate("TimetableViewForm", "locked space", "refers to activity");
				t=", ";
			}
			if(!gt.rules.apdHash.value(act->id, QSet<ConstraintActivityPreferredDay*>()).isEmpty()){
				descr+=t+QCoreApplication::translate("TimetableViewForm", "preferred day", "refers to activity");
			}
			if(descr!=""){
				descr.prepend("\n(");
				descr.append(")");
			}
			s+=descr;
			//added by Volker Dirr (end)
		}
		else{
			if(subgroupNotAvailableDayHour[i][kk][jj]){
				s+=tr("Students subgroup is not available 100% in this slot");
				s+="\n";
			}
			if(breakDayHour[kk][jj]){
				s+=tr("Break with weight 100% in this slot");
				s+="\n";
			}
		}
	}
	detailsTextEdit->setPlainText(s);
}

void TimetableViewStudentsDaysHorizontalForm::lockTime()
{
	lock(true, false);
}

void TimetableViewStudentsDaysHorizontalForm::lockSpace()
{
	lock(false, true);
}

void TimetableViewStudentsDaysHorizontalForm::lockTimeSpace()
{
	lock(true, true);
}

void TimetableViewStudentsDaysHorizontalForm::lock(bool lockTime, bool lockSpace)
{
	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	//find subgroup index
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view students timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	//QString yearname; //not really necessary
	//QString groupname; //not really necessary
	QString subgroupname;
	
	//Note: the years and the groups list widgets might contain useless information if the user selected to see only groups or subgroups.
	//However, in this case they are not void and the currentRow() will be 0 (first year) for the year list widget and, if the variant is
	//to show only the subgroups, the same for the groups list widget (initialized in the dialog's constructor).
	
	/*if(yearsListWidget->currentRow()<0 || yearsListWidget->currentRow()>=yearsListWidget->count()){
		QMessageBox::information(this, tr("FET information"), tr("Please select a year"));
		return;
	}
	if(groupsListWidget->currentRow()<0 || groupsListWidget->currentRow()>=groupsListWidget->count()){
		QMessageBox::information(this, tr("FET information"), tr("Please select a group"));
		return;
	}*/
	if(subgroupsListWidget->currentRow()<0 || subgroupsListWidget->currentRow()>=subgroupsListWidget->count()){
		QMessageBox::information(this, tr("FET information"), tr("Please select a subgroup"));
		return;
	}

	//yearname = yearsListWidget->currentItem()->text();
	//groupname = groupsListWidget->currentItem()->text();
	subgroupname = subgroupsListWidget->currentItem()->text();

	Solution* tc=&best_solution;

	//StudentsSet* ss=gt.rules.searchAugmentedStudentsSet(subgroupname);
	StudentsSet* ss=gt.rules.studentsHash.value(subgroupname, nullptr);
	if(ss==nullptr){
		QMessageBox::information(this, tr("FET warning"), tr("Nonexistent subgroup - please reload this dialog"));
		return;
	}
	if(ss->type!=STUDENTS_SUBGROUP){
		QMessageBox::warning(this, tr("FET warning"), tr("Incorrect subgroup settings - please reload this dialog"));
		return;
	}
	StudentsSubgroup* sts=(StudentsSubgroup*)ss;

	/*StudentsSubgroup* sts=(StudentsSubgroup*)gt.rules.searchAugmentedStudentsSet(subgroupname);
	if(!sts){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid students set - please close this dialog and open a new view students timetable dialog"));
		return;
	}*/
	
	assert(sts!=nullptr);
	int i;
	/*for(i=0; i<gt.rules.nInternalSubgroups; i++)
		if(gt.rules.internalSubgroupsList[i]==sts)
			break;
	assert(i==sts->indexInInternalSubgroupsList);*/
	i=sts->indexInInternalSubgroupsList;
	assert(i>=0);
	assert(i<gt.rules.nInternalSubgroups);

	bool report=false; //the messages are annoying
	
	int addedT=0, unlockedT=0;
	int addedS=0, unlockedS=0;

	bool realView=(gt.rules.mode==MORNINGS_AFTERNOONS && REAL_VIEW==true);
	bool normalView=!realView;

	//lock selected activities
	QSet<int> careAboutIndex;		//added by Volker Dirr. Needed, because of activities with duration > 1
	careAboutIndex.clear();
	for(int j=0; j<(normalView?gt.rules.nHoursPerDay:gt.rules.nRealHoursPerDay) && j<studentsTimetableTable->rowCount(); j++){
		int jj;
		if(normalView)
			jj=j;
		else
			jj=j%gt.rules.nHoursPerDay;
		
		for(int k=0; k<(normalView?gt.rules.nDaysPerWeek:gt.rules.nRealDaysPerWeek) && k<studentsTimetableTable->columnCount(); k++){
			int kk;
			if(normalView)
				kk=k;
			else
				kk=2*k+j/gt.rules.nHoursPerDay;
			
			if(studentsTimetableTable->item(j, k)->isSelected()){
				int ai=students_timetable_weekly[i][kk][jj];
				if(ai!=UNALLOCATED_ACTIVITY && !careAboutIndex.contains(ai)){	//modified, because of activities with duration > 1
					careAboutIndex.insert(ai);					//Needed, because of activities with duration > 1
					int a_tim=tc->times[ai];
					int hour=a_tim/gt.rules.nDaysPerWeek;
					int day=a_tim%gt.rules.nDaysPerWeek;
					//Activity* act=gt.rules.activitiesList.at(ai);
					Activity* act=&gt.rules.internalActivitiesList[ai];
					
					if(lockTime){
						ConstraintActivityPreferredStartingTime* ctr=new ConstraintActivityPreferredStartingTime(100.0, act->id, day, hour, false);
						bool t=gt.rules.addTimeConstraint(ctr);
						QString s;
						if(t){ //modified by Volker Dirr, so you can also unlock (start)
							addedT++;
							idsOfLockedTime.insert(act->id);
							s+=tr("Added the following constraint:")+"\n"+ctr->getDetailedDescription(gt.rules);
						}
						else{
							delete ctr;
						
							QList<TimeConstraint*> tmptc;
							tmptc.clear();
							int count=0;

							for(ConstraintActivityPreferredStartingTime* c : gt.rules.apstHash.value(act->id, QSet<ConstraintActivityPreferredStartingTime*>())){
								assert(c->activityId==act->id);
								if(c->activityId==act->id && c->weightPercentage==100.0 && c->active && c->day>=0 && c->hour>=0){
									count++;
									if(c->permanentlyLocked){
										if(idsOfLockedTime.contains(c->activityId) || !idsOfPermanentlyLockedTime.contains(c->activityId)){
											QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
											  +"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred starting time with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
											  +"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
											  +"\n\n"+tr("Please report possible bug")
											);
										}
										else{
											s+=tr("Constraint %1 will not be removed, because it is permanently locked. If you want to unlock it you must go to the constraints menu.").arg("\n"+c->getDetailedDescription(gt.rules)+"\n");
										}
									}
									else{
										if(!idsOfLockedTime.contains(c->activityId) || idsOfPermanentlyLockedTime.contains(c->activityId)){
											QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
											  +"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred starting time with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
											  +"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
											  +"\n\n"+tr("Please report possible bug")
											);
										}
										else{
											tmptc.append((TimeConstraint*)c);
										}
									}
								}
							}
							if(count!=1)
								QMessageBox::warning(this, tr("FET warning"), tr("You may have a problem, because FET expected to delete 1 constraint, but will delete %1 constraints").arg(tmptc.size()));

							for(TimeConstraint* deltc : std::as_const(tmptc)){
								s+=tr("The following constraint will be deleted:")+"\n"+deltc->getDetailedDescription(gt.rules)+"\n";
								//gt.rules.removeTimeConstraint(deltc);
								//delete deltc; - this is done by rules.removeTimeConstraint(...)
								idsOfLockedTime.remove(act->id);
								unlockedT++;
							}
							gt.rules.removeTimeConstraints(tmptc);
							tmptc.clear();
						}  //modified by Volker Dirr, so you can also unlock (end)
						
						if(report){
							/*int k;
							k=QMessageBox::information(this, tr("FET information"), s,
							 tr("Skip information"), tr("See next"), QString(), 1, 0 );

		 					if(k==0)
								report=false;*/
							QMessageBox::StandardButton k;
							k=QMessageBox::information(this, tr("FET information"), s, QMessageBox::YesToAll | QMessageBox::Ok);
							
							if(k==QMessageBox::YesToAll)
								report=false;
						}
					}

					int ri=tc->rooms[ai];
					if(ri!=UNALLOCATED_SPACE && ri!=UNSPECIFIED_ROOM && lockSpace){
						QStringList tl;
						if(gt.rules.internalRoomsList[ri]->isVirtual==false)
							assert(tc->realRoomsList[ai].isEmpty());
						else
							for(int rr : std::as_const(tc->realRoomsList[ai]))
								tl.append(gt.rules.internalRoomsList[rr]->name);
						
						ConstraintActivityPreferredRoom* ctr=new ConstraintActivityPreferredRoom(100, act->id, (gt.rules.internalRoomsList[ri])->name, tl, false);
						bool t=gt.rules.addSpaceConstraint(ctr);

						QString s;
						
						if(t){ //modified by Volker Dirr, so you can also unlock (start)
							addedS++;
							idsOfLockedSpace.insert(act->id);
							s+=tr("Added the following constraint:")+"\n"+ctr->getDetailedDescription(gt.rules);
						}
						else{
							delete ctr;
						
							QList<SpaceConstraint*> tmpsc;
							tmpsc.clear();
							int count=0;

							for(ConstraintActivityPreferredRoom* c : gt.rules.aprHash.value(act->id, QSet<ConstraintActivityPreferredRoom*>())){
								assert(c->activityId==act->id);
								if(c->activityId==act->id && c->weightPercentage==100.0 && c->active
								 && (gt.rules.internalRoomsList[ri]->isVirtual==false
								 || (gt.rules.internalRoomsList[ri]->isVirtual==true && !c->preferredRealRoomsNames.isEmpty()))){
									count++;
									if(c->permanentlyLocked){
										if(idsOfLockedSpace.contains(c->activityId) || !idsOfPermanentlyLockedSpace.contains(c->activityId)){
											QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
											  +"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred room with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
											  +"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
											  +"\n\n"+tr("Please report possible bug")
											);
										}
										else{
											s+=tr("Constraint %1 will not be removed, because it is permanently locked. If you want to unlock it you must go to the constraints menu.").arg("\n"+c->getDetailedDescription(gt.rules)+"\n");
										}
									}
									else{
										if(!idsOfLockedSpace.contains(c->activityId) || idsOfPermanentlyLockedSpace.contains(c->activityId)){
											QMessageBox::warning(this, tr("FET warning"), tr("Small problem detected")
											  +"\n\n"+tr("A possible problem might be that you have 2 or more constraints of type activity preferred room with weight 100% related to activity id %1, please leave only one of them").arg(act->id)
											  +"\n\n"+tr("A possible problem might be synchronization - so maybe try to close the timetable view dialog and open it again")
											  +"\n\n"+tr("Please report possible bug")
											);
										}
										else{
											tmpsc.append((SpaceConstraint*)c);
										}
									}
								}
							}
							if(count!=1)
								QMessageBox::warning(this, tr("FET warning"), tr("You may have a problem, because FET expected to delete 1 constraint, but will delete %1 constraints").arg(tmpsc.size()));

							for(SpaceConstraint* delsc : std::as_const(tmpsc)){
								s+=tr("The following constraint will be deleted:")+"\n"+delsc->getDetailedDescription(gt.rules)+"\n";
								//gt.rules.removeSpaceConstraint(delsc);
								idsOfLockedSpace.remove(act->id);
								unlockedS++;
								//delete delsc; done by rules.removeSpaceConstraint(...)
							}
							gt.rules.removeSpaceConstraints(tmpsc);
							tmpsc.clear();
						}  //modified by Volker Dirr, so you can also unlock (end)
						
						if(report){
							/*int k;
							k=QMessageBox::information(this, tr("FET information"), s,
							 tr("Skip information"), tr("See next"), QString(), 1, 0 );
							
		 					if(k==0)
								report=false;*/
							QMessageBox::StandardButton k;
							k=QMessageBox::information(this, tr("FET information"), s, QMessageBox::YesToAll | QMessageBox::Ok);
							
							if(k==QMessageBox::YesToAll)
								report=false;
						}
					}
				}
			}
		}
	}
	
	QStringList added;
	QStringList removed;
	if(FET_LANGUAGE=="en_US"){
		if(addedT>0){
			if(addedT==1)
				added << QString("Added 1 locking time constraint.");
			else
				added << QString("Added %1 locking time constraints.").arg(addedT);
		}
		if(addedS>0){
			if(addedS==1)
				added << QString("Added 1 locking space constraint.");
			else
				added << QString("Added %1 locking space constraints.").arg(addedS);
		}
		if(unlockedT>0){
			if(unlockedT==1)
				removed << QString("Removed 1 locking time constraint.");
			else
				removed << QString("Removed %1 locking time constraints.").arg(unlockedT);
		}
		if(unlockedS>0){
			if(unlockedS==1)
				removed << QString("Removed 1 locking space constraint.");
			else
				removed << QString("Removed %1 locking space constraints.").arg(unlockedS);
		}
	}
	else{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
		if(addedT>0){
			added << QCoreApplication::translate("TimetableViewForm", "Added %n locking time constraint(s).",
			 "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
			 " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
			 " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).",
			 addedT);
		}
		if(addedS>0){
			added << QCoreApplication::translate("TimetableViewForm", "Added %n locking space constraint(s).",
			 "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
			 " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
			 " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).",
			 addedS);
		}
		if(unlockedT>0){
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %n locking time constraint(s).",
			 "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
			 " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
			 " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).",
			 unlockedT);
		}
		if(unlockedS>0){
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %n locking space constraint(s).",
			 "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
			 " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
			 " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).",
			 unlockedS);
		}
#else
		if(addedT>0){
			added << QCoreApplication::translate("TimetableViewForm", "Added %n locking time constraint(s).",
			 "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
			 " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
			 " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).", QCoreApplication::UnicodeUTF8,
			 addedT);
		}
		if(addedS>0){
			added << QCoreApplication::translate("TimetableViewForm", "Added %n locking space constraint(s).",
			 "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
			 " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
			 " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).", QCoreApplication::UnicodeUTF8,
			 addedS);
		}
		if(unlockedT>0){
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %n locking time constraint(s).",
			 "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
			 " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
			 " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).", QCoreApplication::UnicodeUTF8,
			 unlockedT);
		}
		if(unlockedS>0){
			removed << QCoreApplication::translate("TimetableViewForm", "Removed %n locking space constraint(s).",
			 "See https://doc.qt.io/qt-6/i18n-plural-rules.html for advice on how to correctly translate this field."
			 " Also, see https://doc.qt.io/qt-6/i18n-source-translation.html, section 'Handle Plural Forms'."
			 " You have two examples on how to translate this field in fet_en_GB.ts and in fet_ro.ts"
			 " (open these files with Qt Linguist and see the translation of this field).", QCoreApplication::UnicodeUTF8,
			 unlockedS);
		}
#endif
	}
	QString ad=added.join("\n");
	QString re=removed.join("\n");
	QStringList all;
	if(!ad.isEmpty())
		all<<ad;
	if(!re.isEmpty())
		all<<re;
	QString s=all.join("\n\n");
	if(s.isEmpty())
		s=QCoreApplication::translate("TimetableViewForm", "No locking constraints added or removed.");
	QMessageBox::information(this, tr("FET information"), s);

	if(addedT>0 || addedS>0 || unlockedT>0 || unlockedS>0)
		gt.rules.addUndoPoint(tr("Toggled lock/unlock a selection of activities in the"
		  " timetable view students days horizontal dialog. The summary of the added/removed locking constraints is:\n\n%1").arg(s)+QString("\n"));

	///////////just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;
	
	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace(); //just to make sure, not really needed, but to test better
	
	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
///////////

	LockUnlock::increaseCommunicationSpinBox(); //this is needed
}

void TimetableViewStudentsDaysHorizontalForm::help()
{
	QString s="";
	s+=QCoreApplication::translate("TimetableViewForm", "Lock/unlock: you can select one or more activities in the table and toggle lock/unlock in time, space or both.");
	s+=" ";
	s+=QCoreApplication::translate("TimetableViewForm", "There will be added or removed locking constraints for the selected activities (they can be unlocked only if they are not permanently locked).");
	s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "Locking time constraints are constraints of type activity preferred starting time. Locking space constraints are constraints of type"
		" activity preferred room. You can see these constraints in the corresponding constraints dialogs. New locking constraints are added at the end of the list of constraints.");
	s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "A bold font cell means that the activity is locked in time, either permanently or not.");
	s+=" ";
	s+=QCoreApplication::translate("TimetableViewForm", "An italic font cell means that the activity is locked in space, either permanently or not.");
	s+=" ";
	s+=QCoreApplication::translate("TimetableViewForm", "An underlined font cell means that the activity has a preferred day constraint (with any weight).");

	s+="\n\n";
	s+=QCoreApplication::translate("TimetableViewForm", "There is a combo box in this dialog. You can choose each year, group, and subgroup if you select 'Years',"
		" each group and subgroup if you select 'Groups', or each subgroup if you select 'Subgroups'.");
	s+="\n\n";

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
