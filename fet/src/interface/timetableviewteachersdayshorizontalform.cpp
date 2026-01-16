/***************************************************************************
                          timetableviewteachersdayshorizontalform.cpp  -  description
                             -------------------
    begin                : Wed May 14 2003
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
#include "timetableviewteachersdayshorizontalform.h"
#include "timetable_defs.h"
#include "timetable.h"
#include "solution.h"

#include "fet.h"

#include "matrix.h"

#include "lockunlock.h"

#include <QMessageBox>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include <QAbstractItemView>

#include <QListWidget>

#include <QList>

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

extern Solution best_solution;

extern bool generation_running;
extern bool generation_running_multi;

extern Matrix3D<bool> teacherNotAvailableDayHour;
extern Matrix2D<bool> breakDayHour;

extern QSet<int> idsOfLockedTime;		//care about locked activities in view forms
extern QSet<int> idsOfLockedSpace;		//care about locked activities in view forms
extern QSet<int> idsOfPermanentlyLockedTime;	//care about locked activities in view forms
extern QSet<int> idsOfPermanentlyLockedSpace;	//care about locked activities in view forms

extern CommunicationSpinBox communicationSpinBox;	//small hint to sync the forms

static bool REAL_VIEW=true;

void TimetableViewTeachersDaysHorizontalDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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

TimetableViewTeachersDaysHorizontalForm::TimetableViewTeachersDaysHorizontalForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);
	
	detailsTextEdit->setReadOnly(true);

	//columnResizeModeInitialized=false;

	//verticalSplitter->setStretchFactor(0, 1);	//unneeded, because both have the same value
	//verticalSplitter->setStretchFactor(1, 1);	//unneeded, because both have the same value
	horizontalSplitter->setStretchFactor(0, 3);
	horizontalSplitter->setStretchFactor(1, 10);

	tableViewSetHighlightHeader(teachersTimetableTable);

	teachersTimetableTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	
	teachersListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(closePushButton, &QPushButton::clicked, this, &TimetableViewTeachersDaysHorizontalForm::close);
	connect(teachersListWidget, &QListWidget::currentTextChanged, this, &TimetableViewTeachersDaysHorizontalForm::teacherChanged);
	connect(teachersTimetableTable, &QTableWidget::currentItemChanged, this, &TimetableViewTeachersDaysHorizontalForm::currentItemChanged);
	connect(lockTimePushButton, &QPushButton::clicked, this, &TimetableViewTeachersDaysHorizontalForm::lockTime);
	connect(lockSpacePushButton, &QPushButton::clicked, this, &TimetableViewTeachersDaysHorizontalForm::lockSpace);
	connect(lockTimeSpacePushButton, &QPushButton::clicked, this, &TimetableViewTeachersDaysHorizontalForm::lockTimeSpace);

	connect(helpPushButton, &QPushButton::clicked, this, &TimetableViewTeachersDaysHorizontalForm::help);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	//restore vertical splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/vertical-splitter-state")))
		verticalSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/vertical-splitter-state")).toByteArray());

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

	connect(teachersCheckBox, &QCheckBox::toggled, this, &TimetableViewTeachersDaysHorizontalForm::updateTeachersTimetableTable);
	connect(studentsCheckBox, &QCheckBox::toggled, this, &TimetableViewTeachersDaysHorizontalForm::updateTeachersTimetableTable);
	connect(subjectsCheckBox, &QCheckBox::toggled, this, &TimetableViewTeachersDaysHorizontalForm::updateTeachersTimetableTable);
	connect(activityTagsCheckBox, &QCheckBox::toggled, this, &TimetableViewTeachersDaysHorizontalForm::updateTeachersTimetableTable);
	connect(roomsCheckBox, &QCheckBox::toggled, this, &TimetableViewTeachersDaysHorizontalForm::updateTeachersTimetableTable);

	connect(colorsCheckBox, &QCheckBox::toggled, this, &TimetableViewTeachersDaysHorizontalForm::updateTeachersTimetableTable);

	connect(spanCheckBox, &QCheckBox::toggled, this, &TimetableViewTeachersDaysHorizontalForm::updateTeachersTimetableTable);

///////////just for testing
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
///////////

	//Commented on 2018-07-20
	//LockUnlock::increaseCommunicationSpinBox();
	
	bool realView=(gt.rules.mode==MORNINGS_AFTERNOONS && REAL_VIEW==true);

	if(!realView){
		teachersTimetableTable->setRowCount(gt.rules.nHoursPerDay);
		teachersTimetableTable->setColumnCount(gt.rules.nDaysPerWeek);

		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.daysOfTheWeek[j]);
			teachersTimetableTable->setHorizontalHeaderItem(j, item);
		}
		for(int i=0; i<gt.rules.nHoursPerDay; i++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.hoursOfTheDay[i]);
			teachersTimetableTable->setVerticalHeaderItem(i, item);
		}

		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				QTableWidgetItem* item=new QTableWidgetItem();
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

				teachersTimetableTable->setItem(j, k, item);
				
				//if(j==0 && k==0)
					//teachersTimetableTable->setCurrentItem(item);
			}
		}
	}
	else{
		teachersTimetableTable->setRowCount(gt.rules.nRealHoursPerDay);
		teachersTimetableTable->setColumnCount(gt.rules.nRealDaysPerWeek);

		for(int j=0; j<gt.rules.nRealDaysPerWeek; j++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.realDaysOfTheWeek[j]);
			teachersTimetableTable->setHorizontalHeaderItem(j, item);
		}
		for(int i=0; i<gt.rules.nRealHoursPerDay; i++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.realHoursOfTheDay[i]);
			teachersTimetableTable->setVerticalHeaderItem(i, item);
		}

		for(int j=0; j<gt.rules.nRealHoursPerDay; j++){
			for(int k=0; k<gt.rules.nRealDaysPerWeek; k++){
				QTableWidgetItem* item=new QTableWidgetItem();
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

				teachersTimetableTable->setItem(j, k, item);
				
				//if(j==0 && k==0)
					//teachersTimetableTable->setCurrentItem(item);
			}
		}
	}
	
	oldItemDelegate=teachersTimetableTable->itemDelegate();
	newItemDelegate=new TimetableViewTeachersDaysHorizontalDelegate(nullptr, teachersTimetableTable, gt.rules.nHoursPerDay, teachersTimetableTable->columnCount());
	teachersTimetableTable->setItemDelegate(newItemDelegate);

	//resize columns
	//if(!columnResizeModeInitialized){
	teachersTimetableTable->horizontalHeader()->setMinimumSectionSize(teachersTimetableTable->horizontalHeader()->defaultSectionSize());
	//	columnResizeModeInitialized=true;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	teachersTimetableTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
	teachersTimetableTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
	//}
	////////////////
	
	teachersListWidget->clear();

	if(gt.rules.nInternalTeachers!=gt.rules.teachersList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some teachers. Please regenerate the timetable and then view it"));
	}
	else{
		for(int i=0; i<gt.rules.nInternalTeachers; i++)
			teachersListWidget->addItem(gt.rules.internalTeachersList[i]->name);
	}

	if(teachersListWidget->count()>0)
		teachersListWidget->setCurrentRow(0);

	//added by Volker Dirr
	connect(&communicationSpinBox, &CommunicationSpinBox::valueChanged, this, &TimetableViewTeachersDaysHorizontalForm::updateTeachersTimetableTable);
}

void TimetableViewTeachersDaysHorizontalForm::newTimetableGenerated()
{
	/*setupUi(this);
	
	closePushButton->setDefault(true);
	
	detailsTextEdit->setReadOnly(true);

	//columnResizeModeInitialized=false;

	//verticalSplitter->setStretchFactor(0, 1);	//unneeded, because both have the same value
	//verticalSplitter->setStretchFactor(1, 1);	//unneeded, because both have the same value
	horizontalSplitter->setStretchFactor(0, 3);
	horizontalSplitter->setStretchFactor(1, 10);

	teachersTimetableTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	
	teachersListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(closePushButton, SIG NAL(clicked()), this, SL OT(close()));
	connect(teachersListWidget, SIG NAL(currentTextChanged(const QString&)), this, SL OT(teacherChanged(const QString&)));
	connect(teachersTimetableTable, SIG NAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SL OT(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)));
	connect(lockTimePushButton, SIG NAL(clicked()), this, SL OT(lockTime()));
	connect(lockSpacePushButton, SIG NAL(clicked()), this, SL OT(lockSpace()));
	connect(lockTimeSpacePushButton, SIG NAL(clicked()), this, SL OT(lockTimeSpace()));

	connect(helpPushButton, SIG NAL(clicked()), this, SL OT(help()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);

	//restore vertical splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/vertical-splitter-state")))
		verticalSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/vertical-splitter-state")).toByteArray());

	//restore horizontal splitter state
	//QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/horizontal-splitter-state")))
		horizontalSplitter->restoreState(settings.value(this->metaObject()->className()+QString("/horizontal-splitter-state")).toByteArray());
*/

///////////just for testing
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
///////////

	int r=teachersTimetableTable->currentRow();
	int c=teachersTimetableTable->currentColumn();

	//DON'T UNCOMMENT THIS CODE -> LEADS TO CRASH IF THERE ARE MORE VIEWS OPENED.
	//LockUnlock::increaseCommunicationSpinBox();
	
	bool realView=(gt.rules.mode==MORNINGS_AFTERNOONS && REAL_VIEW==true);

	if(!realView){
		teachersTimetableTable->clear();
		teachersTimetableTable->setRowCount(gt.rules.nHoursPerDay);
		teachersTimetableTable->setColumnCount(gt.rules.nDaysPerWeek);

		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.daysOfTheWeek[j]);
			teachersTimetableTable->setHorizontalHeaderItem(j, item);
		}
		for(int i=0; i<gt.rules.nHoursPerDay; i++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.hoursOfTheDay[i]);
			teachersTimetableTable->setVerticalHeaderItem(i, item);
		}

		for(int j=0; j<gt.rules.nHoursPerDay; j++){
			for(int k=0; k<gt.rules.nDaysPerWeek; k++){
				QTableWidgetItem* item=new QTableWidgetItem();
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

				teachersTimetableTable->setItem(j, k, item);
				
				//if(j==0 && k==0)
					//teachersTimetableTable->setCurrentItem(item);
			}
		}
	}
	else{
		teachersTimetableTable->clear();
		teachersTimetableTable->setRowCount(gt.rules.nRealHoursPerDay);
		teachersTimetableTable->setColumnCount(gt.rules.nRealDaysPerWeek);

		for(int j=0; j<gt.rules.nRealDaysPerWeek; j++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.realDaysOfTheWeek[j]);
			teachersTimetableTable->setHorizontalHeaderItem(j, item);
		}
		for(int i=0; i<gt.rules.nRealHoursPerDay; i++){
			QTableWidgetItem* item=new QTableWidgetItem(gt.rules.realHoursOfTheDay[i]);
			teachersTimetableTable->setVerticalHeaderItem(i, item);
		}

		for(int j=0; j<gt.rules.nRealHoursPerDay; j++){
			for(int k=0; k<gt.rules.nRealDaysPerWeek; k++){
				QTableWidgetItem* item=new QTableWidgetItem();
				item->setTextAlignment(Qt::AlignCenter);
				item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

				teachersTimetableTable->setItem(j, k, item);
				
				//if(j==0 && k==0)
					//teachersTimetableTable->setCurrentItem(item);
			}
		}
	}
	
	newItemDelegate->nRows=gt.rules.nHoursPerDay;
	newItemDelegate->nColumns=teachersTimetableTable->columnCount();

/*
	//resize columns
	//if(!columnResizeModeInitialized){
	teachersTimetableTable->horizontalHeader()->setMinimumSectionSize(teachersTimetableTable->horizontalHeader()->defaultSectionSize());
	//	columnResizeModeInitialized=true;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
	teachersTimetableTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
	teachersTimetableTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
	//}
	////////////////
	
*/

	int t=teachersListWidget->currentRow();

	disconnect(teachersListWidget, &QListWidget::currentTextChanged, this, &TimetableViewTeachersDaysHorizontalForm::teacherChanged);
	teachersListWidget->clear();

	assert(gt.rules.nInternalTeachers==gt.rules.teachersList.count());
	for(int i=0; i<gt.rules.nInternalTeachers; i++)
		teachersListWidget->addItem(gt.rules.internalTeachersList[i]->name);

	connect(teachersListWidget, &QListWidget::currentTextChanged, this, &TimetableViewTeachersDaysHorizontalForm::teacherChanged);

	if(teachersListWidget->count()>0){
		if(t>=0 && t<teachersListWidget->count())
			teachersListWidget->setCurrentRow(t);
		else
			teachersListWidget->setCurrentRow(0);
	}

	if(r>=0 && r<teachersTimetableTable->rowCount() && c>=0 && c<teachersTimetableTable->columnCount())
		teachersTimetableTable->setCurrentCell(r, c);

	//added by Volker Dirr
	//connect(&communicationSpinBox, SIG NAL(valueChanged(int)), this, SL OT(updateTeachersTimetableTable()));
}

TimetableViewTeachersDaysHorizontalForm::~TimetableViewTeachersDaysHorizontalForm()
{
	saveFETDialogGeometry(this);

	//save vertical splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/vertical-splitter-state"), verticalSplitter->saveState());

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

	teachersTimetableTable->setItemDelegate(oldItemDelegate);
	delete newItemDelegate;
}

void TimetableViewTeachersDaysHorizontalForm::resizeRowsAfterShow()
{
	teachersTimetableTable->resizeRowsToContents();
}

void TimetableViewTeachersDaysHorizontalForm::teacherChanged(const QString& teacherName)
{
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view teachers timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);

	if(teacherName==QString())
		return;

	int teacherId=gt.rules.searchTeacher(teacherName);
	if(teacherId<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid teacher - please close this dialog and open a new view teachers dialog"));
		return;
	}

	updateTeachersTimetableTable();
}

void TimetableViewTeachersDaysHorizontalForm::updateTeachersTimetableTable(){
	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view teachers timetable dialog - please generate a new timetable "
		 "or close the timetable view teachers dialog"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	QString s;
	QString teachername;

	if(teachersListWidget->currentRow()<0 || teachersListWidget->currentRow()>=teachersListWidget->count())
		return;

	teachername = teachersListWidget->currentItem()->text();
	
	int teacher=gt.rules.searchTeacher(teachername);
	if(teacher<0){
		QMessageBox::warning(this, tr("FET warning"), tr("You have an old timetable view teachers dialog opened - please close it"));
		return;
	}

	s = teachername;
	teacherNameTextLabel->setText(s);

	assert(gt.rules.initialized);
	
	bool realView=(gt.rules.mode==MORNINGS_AFTERNOONS && REAL_VIEW==true);
	bool normalView=!realView;

	int numberOfActivities=0;
	int durationOfActivities=0;

	teachersTimetableTable->clearSpans();
	
	for(int j=0; j<(normalView?gt.rules.nHoursPerDay:gt.rules.nRealHoursPerDay) && j<teachersTimetableTable->rowCount(); j++){
		int jj;
		if(normalView)
			jj=j;
		else
			jj=j%gt.rules.nHoursPerDay;
		
		for(int k=0; k<(normalView?gt.rules.nDaysPerWeek:gt.rules.nRealDaysPerWeek) && k<teachersTimetableTable->columnCount(); k++){
			int kk;
			if(normalView)
				kk=k;
			else
				kk=2*k+j/gt.rules.nHoursPerDay;

			QFont font(teachersTimetableTable->item(j, k)->font());
			font.setBold(false);
			font.setItalic(false);
			font.setUnderline(false);
			teachersTimetableTable->item(j, k)->setFont(font);

			//begin by Marco Vassura
			// add colors (start)
			//if(USE_GUI_COLORS) {
			teachersTimetableTable->item(j, k)->setBackground(teachersTimetableTable->palette().base());
			teachersTimetableTable->item(j, k)->setForeground(teachersTimetableTable->palette().text());
			//}
			// add colors (end)
			//end by Marco Vassura
			s = "";
			QString longString="";
			int ai=teachers_timetable_weekly[teacher][kk][jj]; //activity index
			//Activity* act=gt.rules.activitiesList.at(ai);
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				assert(act!=nullptr);
				
				if(best_solution.times[ai]/gt.rules.nDaysPerWeek==jj){
					assert(best_solution.times[ai]%gt.rules.nDaysPerWeek==kk);

					numberOfActivities++;
					durationOfActivities+=act->duration;

					if(spanCheckBox->isChecked() && act->duration>=2)
						teachersTimetableTable->setSpan(j, k, act->duration, 1);
				}

				if(teachersCheckBox->isChecked()){
					if(act->teachersNames.count()==1){
						//Don't do the assert below, because it crashes if you change the teacher's name and view the teachers' timetable,
						//without generating again (as reported by Yush Yuen).
						//assert(act->teachersNames.at(0)==teachername);
					}
					else{
						assert(act->teachersNames.count()>=2);
						//Don't do the assert below, because it crashes if you change the teacher's name and view the teachers' timetable,
						//without generating again (as reported by Yush Yuen).
						//assert(act->teachersNames.contains(teachername));
						s+=act->teachersNames.join(translatedCommaSpace());
						s+="\n";
					}
				}
				
				if(subjectsCheckBox->isChecked()){
					if(activityTagsCheckBox->isChecked() && act->activityTagsNames.count()>0){
						QString ats=act->activityTagsNames.join(translatedCommaSpace());
						s += act->subjectName+" "+ats;
					}
					else{
						s += act->subjectName;
					}
					s+="\n";
				}
				else if(activityTagsCheckBox->isChecked()){
					s+=act->activityTagsNames.join(translatedCommaSpace());
					s+="\n";
				}
				
				//students
				if(studentsCheckBox->isChecked()){
					if(act->studentsNames.count()>0){
						s+=act->studentsNames.join(translatedCommaSpace());
						s+="\n";
					}
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
					QFont font(teachersTimetableTable->item(j, k)->font());
					font.setBold(true);
					teachersTimetableTable->item(j, k)->setFont(font);
				}
				if(idsOfPermanentlyLockedSpace.contains(act->id) || idsOfLockedSpace.contains(act->id)){
					QFont font(teachersTimetableTable->item(j, k)->font());
					font.setItalic(true);
					teachersTimetableTable->item(j, k)->setFont(font);
				}
				if(!gt.rules.apdHash.value(act->id, QSet<ConstraintActivityPreferredDay*>()).isEmpty()){
					QFont font(teachersTimetableTable->item(j, k)->font());
					font.setUnderline(true);
					teachersTimetableTable->item(j, k)->setFont(font);
				}

				//begin by Marco Vassura
				// add colors (start)
				if(colorsCheckBox->isChecked() /*USE_GUI_COLORS*/ /*&& act->studentsNames.count()>0*/){
					QBrush bg(stringToColor(act->subjectName+" "+act->studentsNames.join(", ")));
					teachersTimetableTable->item(j, k)->setBackground(bg);
					double brightness = bg.color().redF()*0.299 + bg.color().greenF()*0.587 + bg.color().blueF()*0.114;
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
					if (brightness<0.5)
						teachersTimetableTable->item(j, k)->setForeground(QBrush(QColorConstants::White));
					else
						teachersTimetableTable->item(j, k)->setForeground(QBrush(QColorConstants::Black));
#else
					if (brightness<0.5)
						teachersTimetableTable->item(j, k)->setForeground(QBrush(Qt::white));
					else
						teachersTimetableTable->item(j, k)->setForeground(QBrush(Qt::black));
#endif
				}
				// add colors (end)
				//end by Marco Vassura
			}
			else{
				if(teacherNotAvailableDayHour[teacher][kk][jj] && PRINT_NOT_AVAILABLE_TIME_SLOTS)
					s+="-x-";
				else if(breakDayHour[kk][jj] && PRINT_BREAK_TIME_SLOTS)
					s+="-X-";
			}
			teachersTimetableTable->item(j, k)->setText(s);
			teachersTimetableTable->item(j, k)->setToolTip(longString);
		}
	}
	//	for(int i=0; i<gt.rules.nHoursPerDay; i++) //added in version 3_9_16, on 16 Oct. 2004
	//		teachersTimetableTable->adjustRow(i);

	numberAndDurationTextLabel->setText(tr("No: %1    Dur: %2", "No means number, %1 is the number of the placed activities,"
	 " Dur means duration, %2 is the duration of the placed activities. Note that there are four spaces between them.").arg(numberOfActivities).arg(durationOfActivities));

	teachersTimetableTable->resizeRowsToContents();
	
	detailActivity(teachersTimetableTable->currentItem());
}

void TimetableViewTeachersDaysHorizontalForm::resizeEvent(QResizeEvent* event)
{
	QDialog::resizeEvent(event);

	teachersTimetableTable->resizeRowsToContents();
}

//begin by Marco Vassura
//slightly modified by Liviu Lalescu on 2021-03-01
QColor TimetableViewTeachersDaysHorizontalForm::stringToColor(const QString& s)
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

void TimetableViewTeachersDaysHorizontalForm::currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
{
	Q_UNUSED(previous);
	
	detailActivity(current);
}

void TimetableViewTeachersDaysHorizontalForm::detailActivity(QTableWidgetItem* item){
	if(item==nullptr){
		detailsTextEdit->setPlainText(QString(""));
		return;
	}

	bool realView=(gt.rules.mode==MORNINGS_AFTERNOONS && REAL_VIEW==true);
	bool normalView=!realView;

	if(item->row()>=(normalView?gt.rules.nHoursPerDay:gt.rules.nRealHoursPerDay) || item->column()>=(normalView?gt.rules.nDaysPerWeek:gt.rules.nRealDaysPerWeek)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view teachers timetable dialog - please generate a new timetable "
		 "or close the timetable view teachers dialog"));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view teachers timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	QString s;
	QString teachername;

	if(teachersListWidget->currentRow()<0 || teachersListWidget->currentRow()>=teachersListWidget->count())
		return;

	teachername = teachersListWidget->currentItem()->text();

	s = teachername;

	teacherNameTextLabel->setText(s);

	int teacher=gt.rules.searchTeacher(teachername);
	if(teacher<0){
		QMessageBox::warning(this, tr("FET warning"), tr("The teacher is invalid - please close this dialog and open a new view teachers timetable"));
		return;
	}
	else{
		int j=item->row();
		int k=item->column();
		s = "";
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

			int ai=teachers_timetable_weekly[teacher][kk][jj]; //activity index
			//Activity* act=gt.rules.activitiesList.at(ai);
			if(ai!=UNALLOCATED_ACTIVITY){
				Activity* act=&gt.rules.internalActivitiesList[ai];
				assert(act!=nullptr);
				s += act->getDetailedDescriptionWithConstraints(gt.rules);
				//s += act->getDetailedDescription(gt.rules);

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
				if(teacherNotAvailableDayHour[teacher][kk][jj]){
					s+=tr("Teacher is not available 100% in this slot");
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
}

void TimetableViewTeachersDaysHorizontalForm::lockTime()
{
	this->lock(true, false);
}
	
void TimetableViewTeachersDaysHorizontalForm::lockSpace()
{
	this->lock(false, true);
}

void TimetableViewTeachersDaysHorizontalForm::lockTimeSpace()
{
	this->lock(true, true);
}
			
void TimetableViewTeachersDaysHorizontalForm::lock(bool lockTime, bool lockSpace)
{
	if(generation_running || generation_running_multi){
		QMessageBox::information(this, tr("FET information"),
			tr("Generation in progress. Please stop the generation before this."));
		return;
	}

	if(!(students_schedule_ready && teachers_schedule_ready)){
		QMessageBox::warning(this, tr("FET warning"), tr("Timetable not available in view teachers timetable dialog - please generate a new timetable"));
		return;
	}
	assert(students_schedule_ready && teachers_schedule_ready);

	if(gt.rules.nInternalRooms!=gt.rules.roomsList.count()){
		QMessageBox::warning(this, tr("FET warning"), tr("Cannot display the timetable, because you added or removed some rooms. Please regenerate the timetable and then view it"));
		return;
	}

	//find teacher index
	QString teachername;

	if(teachersListWidget->currentRow()<0 || teachersListWidget->currentRow()>=teachersListWidget->count()){
		QMessageBox::information(this, tr("FET information"), tr("Please select a teacher"));
		return;
	}

	teachername = teachersListWidget->currentItem()->text();
	int i=gt.rules.searchTeacher(teachername);
	
	if(i<0){
		QMessageBox::warning(this, tr("FET warning"), tr("Invalid teacher - please close this dialog and open a new view teachers dialog"));
		return;
	}

	Solution* tc=&best_solution;
	
	bool report=false; //the messages are annoying
	
	int addedT=0, unlockedT=0;
	int addedS=0, unlockedS=0;

	bool realView=(gt.rules.mode==MORNINGS_AFTERNOONS && REAL_VIEW==true);
	bool normalView=!realView;

	//lock selected activities
	QSet<int> careAboutIndex;		//added by Volker Dirr. Needed, because of activities with duration > 1
	careAboutIndex.clear();
	for(int j=0; j<(normalView?gt.rules.nHoursPerDay:gt.rules.nRealHoursPerDay) && j<teachersTimetableTable->rowCount(); j++){
		int jj;
		if(normalView)
			jj=j;
		else
			jj=j%gt.rules.nHoursPerDay;

		for(int k=0; k<(normalView?gt.rules.nDaysPerWeek:gt.rules.nRealDaysPerWeek) && k<teachersTimetableTable->columnCount(); k++){
			int kk;
			if(normalView)
				kk=k;
			else
				kk=2*k+j/gt.rules.nHoursPerDay;
			
			if(teachersTimetableTable->item(j, k)->isSelected()){
				int ai=teachers_timetable_weekly[i][kk][jj];
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
								idsOfLockedTime.remove(act->id);
								unlockedT++;
								//delete deltc; - done by rules.removeTimeConstraint(...)
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
		  " timetable view teachers days horizontal dialog. The summary of the added/removed locking constraints is:\n\n%1").arg(s)+QString("\n"));

////////// just for testing
	QSet<int> backupLockedTime;
	QSet<int> backupPermanentlyLockedTime;
	QSet<int> backupLockedSpace;
	QSet<int> backupPermanentlyLockedSpace;
	
	backupLockedTime=idsOfLockedTime;
	backupPermanentlyLockedTime=idsOfPermanentlyLockedTime;
	backupLockedSpace=idsOfLockedSpace;
	backupPermanentlyLockedSpace=idsOfPermanentlyLockedSpace;
	
	LockUnlock::computeLockedUnlockedActivitiesTimeSpace(); //not needed, just for testing
	
	assert(backupLockedTime==idsOfLockedTime);
	assert(backupPermanentlyLockedTime==idsOfPermanentlyLockedTime);
	assert(backupLockedSpace==idsOfLockedSpace);
	assert(backupPermanentlyLockedSpace==idsOfPermanentlyLockedSpace);
///////////

	LockUnlock::increaseCommunicationSpinBox();
}

void TimetableViewTeachersDaysHorizontalForm::help()
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

	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
