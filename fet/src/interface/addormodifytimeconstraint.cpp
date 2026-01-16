/***************************************************************************
                          addormodifytimeconstraint.cpp  -  description
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "addormodifytimeconstraint.h"

#include "timetable.h"

#include "longtextmessagebox.h"

#include "utilities.h"

#include "lockunlock.h"

#include "timetableexport.h"

#include <QMessageBox>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QHeaderView>

#include <QStringList>

#include <QSet>
#include <QMap>

#include <QPair>

#include <QBrush>
#include <QPalette>

#include <QGuiApplication>
#include <QPainter>

#include <QSettings>

//std::stable_sort
#include <algorithm>

extern Timetable gt;

extern const QString COMPANY;
extern const QString PROGRAM;

void AddOrModifyTimeConstraintTimesTableDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	int hour=index.row()%nRows;

	if(QGuiApplication::isLeftToRight()){
		if(hour==0){
			painter->drawLine(option.rect.topLeft(), option.rect.topRight());
			painter->drawLine(option.rect.topLeft().x(), option.rect.topLeft().y()+1, option.rect.topRight().x(), option.rect.topRight().y()+1);
		}
		if(hour==nRows-1){
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
		if(hour==nRows-1){
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

AddOrModifyTimeConstraintDialog::AddOrModifyTimeConstraintDialog(QWidget* parent, const QString& _dialogName, const QString& _dialogTitle, QEventLoop* _eventLoop,
																 CornerEnabledTableWidgetOfSpinBoxes* _occupyMaxTimesTable,
																 QAbstractItemDelegate* _occupyMaxOldItemDelegate,
																 AddOrModifyTimeConstraintTimesTableDelegate* _occupyMaxNewItemDelegate,

																 CornerEnabledTableWidget* _timesTable,
																 QAbstractItemDelegate* _oldItemDelegate,
																 AddOrModifyTimeConstraintTimesTableDelegate* _newItemDelegate,

																 CornerEnabledTableWidget* _timesTable1,
																 QAbstractItemDelegate* _oldItemDelegate1,
																 AddOrModifyTimeConstraintTimesTableDelegate* _newItemDelegate1,
																 CornerEnabledTableWidget* _timesTable2,
																 QAbstractItemDelegate* _oldItemDelegate2,
																 AddOrModifyTimeConstraintTimesTableDelegate* _newItemDelegate2,

																 QCheckBox* _colorsCheckBox,

																 QCheckBox* _colorsCheckBox1,
																 QCheckBox* _colorsCheckBox2,

																 QCheckBox* _showRelatedCheckBox,
																 QCheckBox* _firstFilter_showRelatedCheckBox,
																 QCheckBox* _secondFilter_showRelatedCheckBox,
																 QCheckBox* _thirdFilter_showRelatedCheckBox): QDialog(parent)
{
	dialogName=_dialogName;
	dialogTitle=_dialogTitle;
	eventLoop=_eventLoop;

	occupyMaxTimesTable=_occupyMaxTimesTable;
	occupyMaxOldItemDelegate=_occupyMaxOldItemDelegate;
	occupyMaxNewItemDelegate=_occupyMaxNewItemDelegate;

	timesTable=_timesTable;
	oldItemDelegate=_oldItemDelegate;
	newItemDelegate=_newItemDelegate;

	timesTable1=_timesTable1;
	oldItemDelegate1=_oldItemDelegate1;
	newItemDelegate1=_newItemDelegate1;
	timesTable2=_timesTable2;
	oldItemDelegate2=_oldItemDelegate2;
	newItemDelegate2=_newItemDelegate2;

	colorsCheckBox=_colorsCheckBox;

	colorsCheckBox1=_colorsCheckBox1;
	colorsCheckBox2=_colorsCheckBox2;

	showRelatedCheckBox=_showRelatedCheckBox;

	firstFilter_showRelatedCheckBox=_firstFilter_showRelatedCheckBox;
	secondFilter_showRelatedCheckBox=_secondFilter_showRelatedCheckBox;
	thirdFilter_showRelatedCheckBox=_thirdFilter_showRelatedCheckBox;

	setWindowTitle(dialogTitle);

	setAttribute(Qt::WA_DeleteOnClose);

	//resize(600, 400);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this, dialogName);
}

AddOrModifyTimeConstraintDialog::~AddOrModifyTimeConstraintDialog()
{
	if(occupyMaxTimesTable!=nullptr){
		//assert(occupyMaxOldItemDelegate!=nullptr); don't assert this!!! It might be nullptr.
		assert(occupyMaxNewItemDelegate!=nullptr);

		occupyMaxTimesTable->setItemDelegate(occupyMaxOldItemDelegate);
		delete occupyMaxNewItemDelegate;
	}

	if(timesTable!=nullptr){
		//assert(oldItemDelegate!=nullptr); don't assert this!!! It might be nullptr.
		assert(newItemDelegate!=nullptr);

		timesTable->setItemDelegate(oldItemDelegate);
		delete newItemDelegate;
	}

	if(timesTable1!=nullptr){
		//assert(oldItemDelegate1!=nullptr); don't assert this!!! It might be nullptr.
		assert(newItemDelegate1!=nullptr);

		timesTable1->setItemDelegate(oldItemDelegate1);
		delete newItemDelegate1;
	}
	if(timesTable2!=nullptr){
		//assert(oldItemDelegate2!=nullptr); don't assert this!!! It might be nullptr.
		assert(newItemDelegate2!=nullptr);

		timesTable2->setItemDelegate(oldItemDelegate2);
		delete newItemDelegate2;
	}

	saveFETDialogGeometry(this, dialogName);

	if(colorsCheckBox!=nullptr || colorsCheckBox1!=nullptr ||colorsCheckBox2!=nullptr || showRelatedCheckBox!=nullptr || firstFilter_showRelatedCheckBox!=nullptr
	 || secondFilter_showRelatedCheckBox!=nullptr || thirdFilter_showRelatedCheckBox!=nullptr){
		QSettings settings(COMPANY, PROGRAM);

		if(colorsCheckBox!=nullptr)
			settings.setValue(dialogName+QString("/use-colors"), colorsCheckBox->isChecked());

		if(colorsCheckBox1!=nullptr)
			settings.setValue(dialogName+QString("/use-colors-1"), colorsCheckBox1->isChecked());
		if(colorsCheckBox2!=nullptr)
			settings.setValue(dialogName+QString("/use-colors-2"), colorsCheckBox2->isChecked());

		if(showRelatedCheckBox!=nullptr)
			settings.setValue(dialogName+QString("/show-related"), showRelatedCheckBox->isChecked());

		if(firstFilter_showRelatedCheckBox!=nullptr)
			settings.setValue(dialogName+QString("/first-filter-show-related"), firstFilter_showRelatedCheckBox->isChecked());
		if(secondFilter_showRelatedCheckBox!=nullptr)
			settings.setValue(dialogName+QString("/second-filter-show-related"), secondFilter_showRelatedCheckBox->isChecked());
		if(thirdFilter_showRelatedCheckBox!=nullptr)
			settings.setValue(dialogName+QString("/third-filter-show-related"), thirdFilter_showRelatedCheckBox->isChecked());
	}

	eventLoop->quit();
}

AddOrModifyTimeConstraint::AddOrModifyTimeConstraint(QWidget* parent, int _type, TimeConstraint* _oldtc,
	 const QString& _preselectedTeacherName, const QString& _preselectedStudentsSetName, const QString& _preselectedActivityTagName,
	 const QString& _preselectedFirstActivityTagName, const QString& _preselectedSecondActivityTagName)
{
	type=_type;
	oldtc=_oldtc;

	occupyMaxSetsOfTimeSlotsFromSelectionTableWidget=nullptr;
	occupyMaxOldItemDelegate=nullptr;
	occupyMaxNewItemDelegate=nullptr;

	firstTimeSlotGroupBox=nullptr;
	secondTimeSlotGroupBox=nullptr;
	firstDayComboBox=nullptr;
	firstHourComboBox=nullptr;
	secondDayComboBox=nullptr;
	secondHourComboBox=nullptr;

	showRelatedCheckBox=nullptr;

	firstFilter_showRelatedCheckBox=nullptr;
	secondFilter_showRelatedCheckBox=nullptr;
	thirdFilter_showRelatedCheckBox=nullptr;

	filterIsOnSingleRow=false;

	first_filterGroupBox=nullptr;
	second_filterGroupBox=nullptr;
	third_filterGroupBox=nullptr;

	first_filterTeachersComboBox=nullptr;
	second_filterTeachersComboBox=nullptr;
	third_filterTeachersComboBox=nullptr;

	first_filterStudentsComboBox=nullptr;
	second_filterStudentsComboBox=nullptr;
	third_filterStudentsComboBox=nullptr;

	first_filterSubjectsComboBox=nullptr;
	second_filterSubjectsComboBox=nullptr;
	third_filterSubjectsComboBox=nullptr;

	first_filterActivityTagsComboBox=nullptr;
	second_filterActivityTagsComboBox=nullptr;
	third_filterActivityTagsComboBox=nullptr;

	addEmpty=false;

	allowEmptySlotsCheckBox=nullptr;

	activityTagsLabel=nullptr;
	selectedActivityTagsLabel=nullptr;
	activityTagsListWidget=nullptr;
	selectedActivityTagsListWidget=nullptr;
	addAllActivityTagsPushButton=nullptr;
	clearActivityTagsPushButton=nullptr;
	selectedActivityTagsSet.clear();

	first_activityTagLabel=nullptr;
	first_activityTagsComboBox=nullptr;

	second_activityTagLabel=nullptr;
	second_activityTagsComboBox=nullptr;

	tabWidget=nullptr;

	tabWidgetAOMSOTSFS=nullptr;

	tabWidgetTwoSetsOfActivities=nullptr;
	//
	activitiesLabel_TwoSetsOfActivities_1=nullptr;
	selectedActivitiesLabel_TwoSetsOfActivities_1=nullptr;
	activitiesListWidget_TwoSetsOfActivities_1=nullptr;
	selectedActivitiesListWidget_TwoSetsOfActivities_1=nullptr;
	addAllActivitiesPushButton_TwoSetsOfActivities_1=nullptr;
	clearActivitiesPushButton_TwoSetsOfActivities_1=nullptr;
	//
	selectedActivitiesList_TwoSetsOfActivities_1.clear();
	//
	activitiesLabel_TwoSetsOfActivities_2=nullptr;
	selectedActivitiesLabel_TwoSetsOfActivities_2=nullptr;
	activitiesListWidget_TwoSetsOfActivities_2=nullptr;
	selectedActivitiesListWidget_TwoSetsOfActivities_2=nullptr;
	addAllActivitiesPushButton_TwoSetsOfActivities_2=nullptr;
	clearActivitiesPushButton_TwoSetsOfActivities_2=nullptr;
	//
	selectedActivitiesList_TwoSetsOfActivities_2.clear();
	//
	swapTwoSetsOfActivitiesPushButton=nullptr;

	ctrActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots=false;
	ctrActivitiesPairOfMutuallyExclusiveTimeSlots=false;

	tabWidgetPairOfMutuallyExclusiveSets=nullptr;

	intervalStartHourLabel=nullptr;
	intervalStartHourComboBox=nullptr;
	intervalEndHourLabel=nullptr;
	intervalEndHourComboBox=nullptr;

	splitIndexLabel=nullptr;
	splitIndexSpinBox=nullptr;

	first_activityLabel=nullptr;
	first_activitiesComboBox=nullptr;
	first_activitiesList.clear();
	first_initialActivityId=-1;

	second_activityLabel=nullptr;
	second_activitiesComboBox=nullptr;
	second_activitiesList.clear();
	second_initialActivityId=-1;

	third_activityLabel=nullptr;
	third_activitiesComboBox=nullptr;
	third_activitiesList.clear();
	third_initialActivityId=-1;

	swapActivitiesPushButton=nullptr;
	swapActivityTagsPushButton=nullptr;

	firstAddInstructionsLabel=nullptr;
	secondAddInstructionsLabel=nullptr;

	firstModifyInstructionsLabel=nullptr;
	secondModifyInstructionsLabel=nullptr;

	colorsCheckBox=nullptr;
	toggleAllPushButton=nullptr;

	colorsCheckBox1=nullptr;
	toggleAllPushButton1=nullptr;
	colorsCheckBox2=nullptr;
	toggleAllPushButton2=nullptr;
	
	timesTable=nullptr;
	oldItemDelegate=nullptr;
	newItemDelegate=nullptr;

	timesTable1=nullptr;
	oldItemDelegate1=nullptr;
	newItemDelegate1=nullptr;
	timesTable2=nullptr;
	oldItemDelegate2=nullptr;
	newItemDelegate2=nullptr;

	filterGroupBox=nullptr;

	teacherLabel=nullptr;
	teachersComboBox=nullptr;
	
	studentsLabel=nullptr;
	studentsComboBox=nullptr;

	subjectLabel=nullptr;
	subjectsComboBox=nullptr;

	activityTagLabel=nullptr;
	activityTagsComboBox=nullptr;

	activityLabel=nullptr;
	activitiesComboBox=nullptr;
	activitiesList.clear();
	initialActivityId=-1;

	helpPushButton=nullptr;

	labelForSpinBox=nullptr;
	spinBox=nullptr;

	secondLabelForSpinBox=nullptr;
	secondSpinBox=nullptr;

	checkBox=nullptr;

	periodGroupBox=nullptr;
	dayLabel=nullptr;
	daysComboBox=nullptr;
	hourLabel=nullptr;
	hoursComboBox=nullptr;
	permanentlyLockedCheckBox=nullptr;
	permanentlyLockedLabel=nullptr;

	activitiesLabel=nullptr;
	selectedActivitiesLabel=nullptr;
	activitiesListWidget=nullptr;
	selectedActivitiesListWidget=nullptr;
	addAllActivitiesPushButton=nullptr;
	clearActivitiesPushButton=nullptr;
	selectedActivitiesList.clear();
	//activitiesList.clear(); - done above

	durationCheckBox=nullptr;
	durationSpinBox=nullptr;

	if(oldtc!=nullptr)
		assert(oldtc->type==type);
	
	switch(type){
		//1
		case CONSTRAINT_BASIC_COMPULSORY_TIME:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add basic compulsory time", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintBasicCompulsoryTime");
				}
				else{
					dialogTitle=tr("Modify basic compulsory time", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintBasicCompulsoryTime");
				}
				
				break;
			}
		//2
		case CONSTRAINT_BREAK_TIMES:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add break times", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintBreakTimes");

					firstAddInstructionsLabel=new QLabel(tr("The important thing is that a break does not induce gaps for teachers or students"));
					secondAddInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
				}
				else{
					dialogTitle=tr("Modify break times", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintBreakTimes");

					firstModifyInstructionsLabel=new QLabel(tr("The important thing is that a break does not induce gaps for teachers or students"));
					secondModifyInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
				}
				
				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));
				
				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), false);
				
				break;
			}
		//3
		case CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher not available times", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherNotAvailableTimes");

					firstAddInstructionsLabel=new QLabel(tr("Note: this constraint does not induce gaps for teachers. If a teacher has activities before and after a not available period, there will be no gaps counted"));
					secondAddInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
				}
				else{
					dialogTitle=tr("Modify teacher not available times", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherNotAvailableTimes");

					firstModifyInstructionsLabel=new QLabel(tr("Note: this constraint does not induce gaps for teachers. If a teacher has activities before and after a not available period, there will be no gaps counted"));
					secondModifyInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
				}

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), false);

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//4
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max hours daily", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxHoursDaily");
				}
				else{
					dialogTitle=tr("Modify teachers max hours daily", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxHoursDaily");
				}

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				break;
			}
		//5
		case CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify teacher max days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxDaysPerWeek");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek);
				spinBox->setValue(gt.rules.nDaysPerWeek);

				break;
			}
		//6
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max gaps per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxGapsPerWeek");

					firstAddInstructionsLabel=new QLabel(tr("It is recommended to use only one constraint of this type. You must use weight "
														 "percentage 100%, because otherwise you will get poor timetables. Try to fix firstly "
														 "a rather large value for max gaps per week and lower it as you find new timetables. "
														 "The default given here, 3, might be too low for your school and you might need to use "
														 "higher values.\nNote: teacher not available and break are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify teachers max gaps per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxGapsPerWeek");

					firstModifyInstructionsLabel=new QLabel(tr("It is recommended to use only one constraint of this type. You must use weight "
															   "percentage 100%, because otherwise you will get poor timetables. Try to fix firstly "
															   "a rather large value for max gaps per week and lower it as you find new "
															   "timetables.\nNote: teacher not available and break are not counted as gaps."));
				}

				labelForSpinBox=new QLabel(tr("Max gaps per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(3);

				break;
			}
		//7
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max gaps per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxGapsPerWeek");

					firstAddInstructionsLabel=new QLabel(tr("You must use weight percentage 100%, because otherwise you will get poor timetables. "
														 "Try to fix firstly a rather large value for max gaps per week and lower it as you find "
														 "new timetables. The default 3 given here might be too low for your school, in which case "
														 "you need to use higher values.\nNote: teacher not available and break are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify teacher max gaps per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxGapsPerWeek");

					firstModifyInstructionsLabel=new QLabel(tr("You must use weight percentage 100%, because otherwise you will get poor timetables. "
															   "Try to fix firstly a rather large value for max gaps per week and lower it as you find "
															   "new timetables.\nNote: teacher not available and break are not counted as gaps."));
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max gaps per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(3);

				break;
			}
		//8
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max hours daily", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxHoursDaily");
				}
				else{
					dialogTitle=tr("Modify teacher max hours daily", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxHoursDaily");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				break;
			}
		//9
		case CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max hours continuously", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxHoursContinuously");
				}
				else{
					dialogTitle=tr("Modify teachers max hours continuously", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxHoursContinuously");
				}

				labelForSpinBox=new QLabel(tr("Max hours continuously"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				break;
			}
		//10
		case CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max hours continuously", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxHoursContinuously");
				}
				else{
					dialogTitle=tr("Modify teacher max hours continuously", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxHoursContinuously");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max hours continuously"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				break;
			}
		//11
		case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers min hours daily", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinHoursDaily");

					firstAddInstructionsLabel=new QLabel(tr("It is recommended to start with 2 min hours. Maybe you could try higher values "
															"after that, but they might be too large and you might get an impossible timetable."));
				}
				else{
					dialogTitle=tr("Modify teachers min hours daily", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinHoursDaily");

					firstModifyInstructionsLabel=new QLabel(tr("It is recommended to start with 2 min hours. Maybe you could try higher values "
															   "after that, but they might be too large and you might get an impossible timetable."));
				}

				labelForSpinBox=new QLabel(tr("Min hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(2);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				checkBox=new QCheckBox(tr("Allow empty days"));
				checkBox->setChecked(true);

				break;
			}
		//12
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher min hours daily", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinHoursDaily");

					firstAddInstructionsLabel=new QLabel(tr("It is recommended to start with 2 min hours and strengthen them as you find new timetables."));
				}
				else{
					dialogTitle=tr("Modify teacher min hours daily", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinHoursDaily");

					firstModifyInstructionsLabel=new QLabel(tr("It is recommended to start with 2 min hours and strengthen them as you find new timetables."));
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(2);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				checkBox=new QCheckBox(tr("Allow empty days"));
				checkBox->setChecked(true);

				break;
			}
		//13
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max gaps per day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxGapsPerDay");

					firstAddInstructionsLabel=new QLabel(tr("It is recommended to use only one constraint of this type. You must use weight "
															"percentage 100%, because otherwise you will get poor timetables. Try to fix firstly "
															"a rather large value for max gaps per day and lower it as you find new timetables. "
															"The default given here, 1, might be too low for your school and you might need to use "
															"higher values.\nNote: teacher not available and break are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify teachers max gaps per day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxGapsPerDay");

					firstModifyInstructionsLabel=new QLabel(tr("It is recommended to use only one constraint of this type. You must use weight "
															   "percentage 100%, because otherwise you will get poor timetables. Try to fix firstly "
															   "a rather large value for max gaps per day and lower it as you find new timetables.\nNote: "
															   "teacher not available and break are not counted as gaps."));
				}

				labelForSpinBox=new QLabel(tr("Max gaps per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//14
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max gaps per day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxGapsPerDay");

					firstAddInstructionsLabel=new QLabel(tr("You must use weight percentage 100%, because otherwise you will get poor timetables. "
															"Try to fix firstly a rather large value for max gaps per day and lower it as you find "
															"new timetables. The default 1 given here might be too low for your school, in which "
															"case you need to use higher values.\nNote: teacher not available and break are not "
															"counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify teacher max gaps per day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxGapsPerDay");

					firstModifyInstructionsLabel=new QLabel(tr("You must use weight percentage 100%, because otherwise you will get poor timetables. "
															   "Try to fix firstly a rather large value for max gaps per day and lower it as you find "
															   "new timetables.\nNote: teacher not available and break are not counted as gaps."));
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max gaps per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//15
		case CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students early max beginnings at second hour", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsEarlyMaxBeginningsAtSecondHour");

					firstAddInstructionsLabel=new QLabel(tr("This represents the maximum allowed beginnings at the second available hour (in a week). "
															"You might want to allow your students to begin in some days later, at the second available "
															"hour, so you might put \"max beginnings at second hour\" greater than 0 (the timetable will "
															"be easier for FET). Default 0 is given for students who need to begin all classes at the "
															"first available hour. You might want to choose a greater value and lower it as you find new "
															"timetables"));
					secondAddInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%, because otherwise you will get poor timetables. "
															 "Currently, the algorithm only accepts 100% weight percentage for this constraint (or no "
															 "constraint).\nNote: students set not available and break are not counted as gaps (early "
															 "gaps)."));
				}
				else{
					dialogTitle=tr("Modify students early max beginnings at second hour", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsEarlyMaxBeginningsAtSecondHour");

					firstModifyInstructionsLabel=new QLabel(tr("This represents the maximum allowed beginnings at the second available hour (in a week). "
															   "You might want to allow your students to begin in some days later, at the second available "
															   "hour, so you might put \"max beginnings at second hour\" greater than 0 (the timetable will "
															   "be easier for FET). You might want to choose a greater value and lower it as you find new "
															   "timetables"));
					secondModifyInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%, because otherwise you will get poor timetables. "
																"Currently, the algorithm only accepts 100% weight percentage for this constraint (or no "
																"constraint).\nNote: students set not available and break are not counted as gaps (early "
																"gaps)."));
				}

				labelForSpinBox=new QLabel(tr("Max beginnings at second hour (per week)"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek);
				spinBox->setValue(0);

				break;
			}
		//16
		case CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set early max beginnings at second hour", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetEarlyMaxBeginningsAtSecondHour");

					firstAddInstructionsLabel=new QLabel(tr("This represents the maximum allowed beginnings at the second available hour (in a week). You might "
															"want to allow your students to begin in some days later, at the second available hour, so you might "
															"put \"max beginnings at second hour\" greater than 0 (the timetable will be easier for FET). Default 0 "
															"is given for students who need to begin all classes at the first available hour. You might want to "
															"choose a greater value and lower it as you find new timetables"));
					secondAddInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%, because otherwise you will get poor timetables. Currently, the "
															 "algorithm only accepts 100% weight percentage for this constraint (or no constraint).\nNote: students "
															 "set not available and break are not counted as gaps (early gaps)."));
				}
				else{
					dialogTitle=tr("Modify students set early max beginnings at second hour", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetEarlyMaxBeginningsAtSecondHour");

					firstModifyInstructionsLabel=new QLabel(tr("This represents the maximum allowed beginnings at the second available hour (in a week). You might "
															   "want to allow your students to begin in some days later, at the second available hour, so you might "
															   "put \"max beginnings at second hour\" greater than 0 (the timetable will be easier for FET). You might "
															   "want to choose a greater value and lower it as you find new timetables"));
					secondModifyInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%, because otherwise you will get poor timetables. Currently, "
																"the algorithm only accepts 100% weight percentage for this constraint (or no constraint).\nNote: "
																"students set not available and break are not counted as gaps (early gaps)."));
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max beginnings at second hour (per week)"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek);
				spinBox->setValue(0);

				break;
			}
		//17
		case CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set not available times", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetNotAvailableTimes");

					firstAddInstructionsLabel=new QLabel(tr("Note: this constraint does not induce gaps (or early not respected) for students. If a students set has activities before and after a not available period, there will be no gaps counted"));
					secondAddInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
				}
				else{
					dialogTitle=tr("Modify students set not available times", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetNotAvailableTimes");

					firstModifyInstructionsLabel=new QLabel(tr("This constraint does not induce gaps (or early not respected) for students. If a students set has activities before and after this not available period, there will be no gaps counted"));
					secondModifyInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
				}

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), false);

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//18
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max gaps per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxGapsPerWeek");

					firstAddInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%, because otherwise you will get poor "
															"timetables. Currently, the algorithm only accepts 100% weight percentage for "
															"this constraint (or no constraint).\nNote: students set not available and break "
															"are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify students max gaps per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxGapsPerWeek");

					firstModifyInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%, because otherwise you will get poor "
															   "timetables. Currently, the algorithm only accepts 100% weight percentage for "
															   "this constraint (or no constraint).\nNote: students set not available and "
															   "break are not counted as gaps."));
				}

				labelForSpinBox=new QLabel(tr("Max gaps per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek);
				spinBox->setValue(0);

				break;
			}
		//19
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max gaps per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxGapsPerWeek");

					firstAddInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%, because otherwise you will get poor timetables. "
															"Currently, the algorithm only accepts 100% weight percentage for this constraint (or no "
															"constraint).\nNote: students set not available and break are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify students set max gaps per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxGapsPerWeek");

					firstModifyInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%, because otherwise you will get poor timetables. "
															   "Currently, the algorithm only accepts 100% weight percentage for this constraint (or no "
															   "constraint).\nNote: students set not available and break are not counted as gaps."));
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max gaps per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay*gt.rules.nDaysPerWeek);
				spinBox->setValue(0);

				break;
			}
		//20
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max hours daily", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxHoursDaily");
				}
				else{
					dialogTitle=tr("Modify students max hours daily", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxHoursDaily");
				}

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				break;
			}
		//21
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max hours daily", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxHoursDaily");
				}
				else{
					dialogTitle=tr("Modify students set max hours daily", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxHoursDaily");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				break;
			}
		//22
		case CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max hours continuously", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxHoursContinuously");
				}
				else{
					dialogTitle=tr("Modify students max hours continuously", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxHoursContinuously");
				}

				labelForSpinBox=new QLabel(tr("Max hours continuously"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				break;
			}
		//23
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max hours continuously", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxHoursContinuously");
				}
				else{
					dialogTitle=tr("Modify students set max hours continuously", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxHoursContinuously");
				}

				labelForSpinBox=new QLabel(tr("Max hours continuously"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				break;
			}
		//24
		case CONSTRAINT_STUDENTS_MIN_HOURS_DAILY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students min hours daily", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMinHoursDaily");
				}
				else{
					dialogTitle=tr("Modify students min hours daily", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMinHoursDaily");
				}

				labelForSpinBox=new QLabel(tr("Min hours daily"));
				spinBox=new QSpinBox;

				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				checkBox=new QCheckBox(tr("Allow empty days"));
				if(gt.rules.mode==MORNINGS_AFTERNOONS){
					spinBox->setValue(2);
					checkBox->setChecked(true);
				}
				else{
					spinBox->setValue(1);
					checkBox->setChecked(false);
				}

				break;
			}
		//25
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set min hours daily", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMinHoursDaily");
				}
				else{
					dialogTitle=tr("Modify students set min hours daily", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMinHoursDaily");
				}

				labelForSpinBox=new QLabel(tr("Min hours daily"));
				spinBox=new QSpinBox;

				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				checkBox=new QCheckBox(tr("Allow empty days"));
				if(gt.rules.mode==MORNINGS_AFTERNOONS){
					spinBox->setValue(2);
					checkBox->setChecked(true);
				}
				else{
					spinBox->setValue(1);
					checkBox->setChecked(false);
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				break;
			}
		//26
		case CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activity ends students day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivityEndsStudentsDay");
				}
				else{
					dialogTitle=tr("Modify activity ends students day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivityEndsStudentsDay");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activityLabel=new QLabel(tr("Activity"));
				activitiesComboBox=new QComboBox;

				break;
			}
		//27
		case CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activity preferred starting time", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivityPreferredStartingTime");
				}
				else{
					dialogTitle=tr("Modify activity preferred starting time", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivityPreferredStartingTime");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activityLabel=new QLabel(tr("Activity"));
				activitiesComboBox=new QComboBox;

				permanentlyLockedCheckBox=new QCheckBox(tr("Permanently locked"));
				permanentlyLockedLabel=new QLabel(tr("Select this if you want this constraint not to be "
													 "unlockable from the 'Timetable' menu (recommended). "
													 "You can always remove this constraint from the constraints "
													 "dialogs or uncheck \"permanently locked\" checkbox by "
													 "editing this constraint"));
				periodGroupBox=new QGroupBox(tr("Period"));
				dayLabel=new QLabel(tr("Day"));
				daysComboBox=new QComboBox;
				hourLabel=new QLabel(tr("Hour"));
				hoursComboBox=new QComboBox;

				break;
			}
		//28
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities same starting time", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesSameStartingTime");
				}
				else{
					dialogTitle=tr("Modify activities same starting time", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesSameStartingTime");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				if(oldtc==nullptr){
					checkBox=new QCheckBox(tr("Add multiple constraints"));
					checkBox->setChecked(false);
				}

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//29
		case CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities not overlapping", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesNotOverlapping");

					firstAddInstructionsLabel=new QLabel(tr("This constraint means that the activities should not be placed in common "
															"time slots. You don't need to add this constraint for activities sharing "
															"a teacher or students set."));
				}
				else{
					dialogTitle=tr("Modify activities not overlapping", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesNotOverlapping");

					firstModifyInstructionsLabel=new QLabel(tr("This constraint means that the activities should not be placed in common "
															"time slots. You don't need to add this constraint for activities sharing "
															"a teacher or students set."));
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				break;
			}
		//30
		case CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add min days between activities", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintMinDaysBetweenActivities");
				}
				else{
					dialogTitle=tr("Modify min days between activities", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintMinDaysBetweenActivities");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				labelForSpinBox=new QLabel(tr("Min days"));
				spinBox=new QSpinBox;
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					spinBox->setMinimum(1);
					spinBox->setMaximum(gt.rules.nDaysPerWeek-1);
					spinBox->setValue(1);
				}
				else{
					spinBox->setMinimum(1);
					spinBox->setMaximum(gt.rules.nDaysPerWeek/2-1);
					spinBox->setValue(1);
				}

				checkBox=new QCheckBox(tr("If two activities are on the same day, make them consecutive"));
				checkBox->setChecked(true);

				break;
			}
		//31
		case CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activity preferred time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivityPreferredTimeSlots");

					firstAddInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
				}
				else{
					dialogTitle=tr("Modify activity preferred time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivityPreferredTimeSlots");

					firstModifyInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activityLabel=new QLabel(tr("Activity"));
				activitiesComboBox=new QComboBox;

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), false);

				break;
			}
		//32
		case CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities preferred time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesPreferredTimeSlots");

					firstAddInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
				}
				else{
					dialogTitle=tr("Modify activities preferred time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesPreferredTimeSlots");

					firstModifyInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
				}

				addEmpty=true;

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				durationCheckBox=new QCheckBox(tr("Duration"));
				durationSpinBox=new QSpinBox;
				durationCheckBox->setChecked(false);
				durationSpinBox->setEnabled(false);
				durationSpinBox->setMinimum(1);
				durationSpinBox->setMaximum(gt.rules.nHoursPerDay);
				durationSpinBox->setValue(1);

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), false);

				break;
			}
		//33
		case CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activity preferred starting times", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivityPreferredStartingTimes");

					firstAddInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
				}
				else{
					dialogTitle=tr("Modify activity preferred starting times", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivityPreferredStartingTimes");

					firstModifyInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activityLabel=new QLabel(tr("Activity"));
				activitiesComboBox=new QComboBox;

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), false);

				break;
			}
		//34
		case CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities preferred starting times", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesPreferredStartingTimes");

					firstAddInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
				}
				else{
					dialogTitle=tr("Modify activities preferred starting times", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesPreferredStartingTimes");

					firstModifyInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
				}

				addEmpty=true;

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				durationCheckBox=new QCheckBox(tr("Duration"));
				durationSpinBox=new QSpinBox;
				durationCheckBox->setChecked(false);
				durationSpinBox->setEnabled(false);
				durationSpinBox->setMinimum(1);
				durationSpinBox->setMaximum(gt.rules.nHoursPerDay);
				durationSpinBox->setValue(1);

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), false);

				break;
			}
		//35
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities same starting hour", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesSameStartingHour");
				}
				else{
					dialogTitle=tr("Modify activities same starting hour", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesSameStartingHour");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				break;
			}
		//36
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities same starting day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesSameStartingDay");
				}
				else{
					dialogTitle=tr("Modify activities same starting day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesSameStartingDay");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				break;
			}
		//37
		case CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add two activities consecutive", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTwoActivitiesConsecutive");
				}
				else{
					dialogTitle=tr("Modify two activities consecutive", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTwoActivitiesConsecutive");
				}

				/*addEmpty=true; -> does not affect us
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;*/

				/*activityLabel=new QLabel(tr("Activity"));
				activitiesComboBox=new QComboBox;*/

				first_filterGroupBox=new QGroupBox(tr("First filter"));
				first_filterTeachersComboBox=new QComboBox;
				first_filterStudentsComboBox=new QComboBox;
				first_filterSubjectsComboBox=new QComboBox;
				first_filterActivityTagsComboBox=new QComboBox;

				second_filterGroupBox=new QGroupBox(tr("Second filter"));
				second_filterTeachersComboBox=new QComboBox;
				second_filterStudentsComboBox=new QComboBox;
				second_filterSubjectsComboBox=new QComboBox;
				second_filterActivityTagsComboBox=new QComboBox;

				first_activityLabel=new QLabel(tr("First activity"));
				first_activitiesComboBox=new QComboBox;

				second_activityLabel=new QLabel(tr("Second activity"));
				second_activitiesComboBox=new QComboBox;

				swapActivitiesPushButton=new QPushButton(tr("Swap"));

				break;
			}
		//38
		case CONSTRAINT_TWO_ACTIVITIES_ORDERED:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add two activities ordered", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTwoActivitiesOrdered");
				}
				else{
					dialogTitle=tr("Modify two activities ordered", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTwoActivitiesOrdered");
				}

				/*addEmpty=true; -> does not affect us
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;*/

				/*activityLabel=new QLabel(tr("Activity"));
				activitiesComboBox=new QComboBox;*/

				first_filterGroupBox=new QGroupBox(tr("First filter"));
				first_filterTeachersComboBox=new QComboBox;
				first_filterStudentsComboBox=new QComboBox;
				first_filterSubjectsComboBox=new QComboBox;
				first_filterActivityTagsComboBox=new QComboBox;

				second_filterGroupBox=new QGroupBox(tr("Second filter"));
				second_filterTeachersComboBox=new QComboBox;
				second_filterStudentsComboBox=new QComboBox;
				second_filterSubjectsComboBox=new QComboBox;
				second_filterActivityTagsComboBox=new QComboBox;

				first_activityLabel=new QLabel(tr("First activity"));
				first_activitiesComboBox=new QComboBox;

				second_activityLabel=new QLabel(tr("Second activity"));
				second_activitiesComboBox=new QComboBox;

				swapActivitiesPushButton=new QPushButton(tr("Swap"));

				break;
			}
		//39
		case CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add min gaps between activities", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintMinGapsBetweenActivities");
				}
				else{
					dialogTitle=tr("Modify min gaps between activities", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintMinGapsBetweenActivities");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//40
		case CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add subactivities preferred time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintSubactivitiesPreferredTimeSlots");

					firstAddInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
					secondAddInstructionsLabel=new QLabel(tr("Choose the component number of the subactivities to be constrained."));
				}
				else{
					dialogTitle=tr("Modify subactivities preferred time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintSubactivitiesPreferredTimeSlots");

					firstModifyInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
					secondModifyInstructionsLabel=new QLabel(tr("Choose the component number of the subactivities to be constrained."));
				}

				addEmpty=true;

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				durationCheckBox=new QCheckBox(tr("Duration"));
				durationSpinBox=new QSpinBox;
				durationCheckBox->setChecked(false);
				durationSpinBox->setEnabled(false);
				durationSpinBox->setMinimum(1);
				durationSpinBox->setMaximum(gt.rules.nHoursPerDay);
				durationSpinBox->setValue(1);

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), false);

				splitIndexLabel=new QLabel(tr("Component number (split index)"));
				splitIndexSpinBox=new QSpinBox;

				break;
			}
		//41
		case CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add subactivities preferred starting times", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintSubactivitiesPreferredStartingTimes");

					firstAddInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
					secondAddInstructionsLabel=new QLabel(tr("Choose the component number of the subactivities to be constrained."));
				}
				else{
					dialogTitle=tr("Modify subactivities preferred starting times", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintSubactivitiesPreferredStartingTimes");

					firstModifyInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
					secondModifyInstructionsLabel=new QLabel(tr("Choose the component number of the subactivities to be constrained."));
				}

				addEmpty=true;

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				durationCheckBox=new QCheckBox(tr("Duration"));
				durationSpinBox=new QSpinBox;
				durationCheckBox->setChecked(false);
				durationSpinBox->setEnabled(false);
				durationSpinBox->setMinimum(1);
				durationSpinBox->setMaximum(gt.rules.nHoursPerDay);
				durationSpinBox->setValue(1);

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), false);

				splitIndexLabel=new QLabel(tr("Component number (split index)"));
				splitIndexSpinBox=new QSpinBox;

				break;
			}
		//42
		case CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher interval max days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherIntervalMaxDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify teacher interval max days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherIntervalMaxDaysPerWeek");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek);
				spinBox->setValue(gt.rules.nDaysPerWeek);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//43
		case CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers interval max days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersIntervalMaxDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify teachers interval max days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersIntervalMaxDaysPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek);
				spinBox->setValue(gt.rules.nDaysPerWeek);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//44
		case CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set interval max days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetIntervalMaxDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify students set interval max days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetIntervalMaxDaysPerWeek");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek);
				spinBox->setValue(gt.rules.nDaysPerWeek);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//45
		case CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students interval max days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsIntervalMaxDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify students interval max days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsIntervalMaxDaysPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek);
				spinBox->setValue(gt.rules.nDaysPerWeek);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//46
		case CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities end students day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesEndStudentsDay");

					firstAddInstructionsLabel=new QLabel(tr("Instructions: you can choose a set of activities with a certain teacher (or "
															"blank for all teachers), students set, subject and activity tag."));
				}
				else{
					dialogTitle=tr("Modify activities end students day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesEndStudentsDay");

					firstModifyInstructionsLabel=new QLabel(tr("Instructions: you can choose a set of activities with a certain teacher (or "
															   "blank for all teachers), students set, subject and activity tag."));
				}

				addEmpty=true;

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//47
		case CONSTRAINT_TWO_ACTIVITIES_GROUPED:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add two activities grouped", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTwoActivitiesGrouped");
				}
				else{
					dialogTitle=tr("Modify two activities grouped", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTwoActivitiesGrouped");
				}

				/*addEmpty=true; -> does not affect us
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;*/

				/*activityLabel=new QLabel(tr("Activity"));
				activitiesComboBox=new QComboBox;*/

				first_filterGroupBox=new QGroupBox(tr("First filter"));
				first_filterTeachersComboBox=new QComboBox;
				first_filterStudentsComboBox=new QComboBox;
				first_filterSubjectsComboBox=new QComboBox;
				first_filterActivityTagsComboBox=new QComboBox;

				second_filterGroupBox=new QGroupBox(tr("Second filter"));
				second_filterTeachersComboBox=new QComboBox;
				second_filterStudentsComboBox=new QComboBox;
				second_filterSubjectsComboBox=new QComboBox;
				second_filterActivityTagsComboBox=new QComboBox;

				first_activityLabel=new QLabel(tr("First activity"));
				first_activitiesComboBox=new QComboBox;

				second_activityLabel=new QLabel(tr("Second activity"));
				second_activitiesComboBox=new QComboBox;

				break;
			}
		//48
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers activity tag max hours continuously", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersActivityTagMaxHoursContinuously");
				}
				else{
					dialogTitle=tr("Modify teachers activity tag max hours continuously", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersActivityTagMaxHoursContinuously");
				}

				labelForSpinBox=new QLabel(tr("Max hours continuously"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//49
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher activity tag max hours continuously", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherActivityTagMaxHoursContinuously");
				}
				else{
					dialogTitle=tr("Modify teacher activity tag max hours continuously", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherActivityTagMaxHoursContinuously");
				}

				labelForSpinBox=new QLabel(tr("Max hours continuously"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;
				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//50
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students activity tag max hours continuously", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsActivityTagMaxHoursContinuously");
				}
				else{
					dialogTitle=tr("Modify students activity tag max hours continuously", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsActivityTagMaxHoursContinuously");
				}

				labelForSpinBox=new QLabel(tr("Max hours continuously"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//51
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set activity tag max hours continuously", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetActivityTagMaxHoursContinuously");
				}
				else{
					dialogTitle=tr("Modify students set activity tag max hours continuously", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetActivityTagMaxHoursContinuously");
				}

				labelForSpinBox=new QLabel(tr("Max hours continuously"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;
				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//52
		case CONSTRAINT_TEACHERS_MAX_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify teachers max days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxDaysPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek);
				spinBox->setValue(gt.rules.nDaysPerWeek);

				break;
			}
		//53
		case CONSTRAINT_THREE_ACTIVITIES_GROUPED:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add three activities grouped", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintThreeActivitiesGrouped");
				}
				else{
					dialogTitle=tr("Modify three activities grouped", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintThreeActivitiesGrouped");
				}

				filterIsOnSingleRow=true;

				/*addEmpty=true; -> does not affect us
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;*/

				/*activityLabel=new QLabel(tr("Activity"));
				activitiesComboBox=new QComboBox;*/

				first_filterGroupBox=new QGroupBox(tr("First filter"));
				first_filterTeachersComboBox=new QComboBox;
				first_filterStudentsComboBox=new QComboBox;
				first_filterSubjectsComboBox=new QComboBox;
				first_filterActivityTagsComboBox=new QComboBox;

				second_filterGroupBox=new QGroupBox(tr("Second filter"));
				second_filterTeachersComboBox=new QComboBox;
				second_filterStudentsComboBox=new QComboBox;
				second_filterSubjectsComboBox=new QComboBox;
				second_filterActivityTagsComboBox=new QComboBox;

				third_filterGroupBox=new QGroupBox(tr("Third filter"));
				third_filterTeachersComboBox=new QComboBox;
				third_filterStudentsComboBox=new QComboBox;
				third_filterSubjectsComboBox=new QComboBox;
				third_filterActivityTagsComboBox=new QComboBox;

				first_activityLabel=new QLabel(tr("First activity"));
				first_activitiesComboBox=new QComboBox;

				second_activityLabel=new QLabel(tr("Second activity"));
				second_activitiesComboBox=new QComboBox;

				third_activityLabel=new QLabel(tr("Third activity"));
				third_activitiesComboBox=new QComboBox;

				break;
			}
		//54
		case CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add max days between activities", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintMaxDaysBetweenActivities");
				}
				else{
					dialogTitle=tr("Modify max days between activities", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintMaxDaysBetweenActivities");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				labelForSpinBox=new QLabel(tr("Max days"));
				spinBox=new QSpinBox;
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					spinBox->setMinimum(0);
					spinBox->setMaximum(gt.rules.nDaysPerWeek-1);
					spinBox->setValue(gt.rules.nDaysPerWeek-1);
				}
				else{
					spinBox->setMinimum(0);
					spinBox->setMaximum(gt.rules.nDaysPerWeek/2-1);
					spinBox->setValue(gt.rules.nDaysPerWeek/2-1);
				}

				break;
			}
		//55
		case CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers min days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify teachers min days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinDaysPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Min days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek);
				spinBox->setValue(1);

				break;
			}
		//56
		case CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher min days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify teacher min days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinDaysPerWeek");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek);
				spinBox->setValue(1);

				break;
			}
		//57
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers activity tag max hours daily", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersActivityTagMaxHoursDaily");
				}
				else{
					dialogTitle=tr("Modify teachers activity tag max hours daily", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersActivityTagMaxHoursDaily");
				}

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//58
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher activity tag max hours daily", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherActivityTagMaxHoursDaily");
				}
				else{
					dialogTitle=tr("Modify teacher activity tag max hours daily", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherActivityTagMaxHoursDaily");
				}

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;
				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//59
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students activity tag max hours daily", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsActivityTagMaxHoursDaily");
				}
				else{
					dialogTitle=tr("Modify students activity tag max hours daily", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsActivityTagMaxHoursDaily");
				}

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//60
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set activity tag max hours daily", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetActivityTagMaxHoursDaily");
				}
				else{
					dialogTitle=tr("Modify students set activity tag max hours daily", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetActivityTagMaxHoursDaily");
				}

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;
				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//61
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max gaps per day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxGapsPerDay");

					firstAddInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%\nNote: students set not available "
															"and break are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify students max gaps per day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxGapsPerDay");

					firstModifyInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%\nNote: students set not available "
															   "and break are not counted as gaps."));
				}

				labelForSpinBox=new QLabel(tr("Max gaps per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//62
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max gaps per day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxGapsPerDay");

					firstAddInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%\nNote: students set not available "
															"and break are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify students set max gaps per day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxGapsPerDay");

					firstModifyInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%\nNote: students set not available "
															   "and break are not counted as gaps."));
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max gaps per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//63
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TIME_SLOTS_FROM_SELECTION:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities occupy max time slots from selection", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesOccupyMaxTimeSlotsFromSelection");

					firstAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}
				else{
					dialogTitle=tr("Modify activities occupy max time slots from selection", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesOccupyMaxTimeSlotsFromSelection");

					firstModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), true);

				labelForSpinBox=new QLabel(tr("Max occupied"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				tabWidget=new QTabWidget;

				break;
			}
		//64
		case CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities max simultaneous in selected time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesMaxSimultaneousInSelectedTimeSlots");

					firstAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}
				else{
					dialogTitle=tr("Modify activities max simultaneous in selected time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesMaxSimultaneousInSelectedTimeSlots");

					firstModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), true);

				labelForSpinBox=new QLabel(tr("Max simultaneous"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(MAX_ACTIVITIES);
				spinBox->setValue(0);

				tabWidget=new QTabWidget;

				break;
			}
		//65
		case CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify students set max days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxDaysPerWeek");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek);
				spinBox->setValue(gt.rules.nDaysPerWeek);

				break;
			}
		//66
		case CONSTRAINT_STUDENTS_MAX_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify students max days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxDaysPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek);
				spinBox->setValue(gt.rules.nDaysPerWeek);

				break;
			}
		//67
		case CONSTRAINT_TEACHER_MAX_SPAN_PER_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max span per day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxSpanPerDay");
				}
				else{
					dialogTitle=tr("Modify teacher max span per day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxSpanPerDay");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max span per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				checkBox=new QCheckBox(tr("Allow one day exception of plus one"));
				checkBox->setChecked(false);

				break;
			}
		//68
		case CONSTRAINT_TEACHERS_MAX_SPAN_PER_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max span per day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxSpanPerDay");
				}
				else{
					dialogTitle=tr("Modify teachers max span per day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxSpanPerDay");
				}

				labelForSpinBox=new QLabel(tr("Max span per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				checkBox=new QCheckBox(tr("Allow one day exception of plus one"));
				checkBox->setChecked(false);

				break;
			}
		//69
		case CONSTRAINT_TEACHER_MIN_RESTING_HOURS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher min resting hours", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinRestingHours");
				}
				else{
					dialogTitle=tr("Modify teacher min resting hours", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinRestingHours");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min resting hours"));
				spinBox=new QSpinBox;

				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				checkBox=new QCheckBox(tr("Circular"));
				checkBox->setChecked(true);

				break;
			}
		//70
		case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers min resting hours", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinRestingHours");
				}
				else{
					dialogTitle=tr("Modify teachers min resting hours", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinRestingHours");
				}

				labelForSpinBox=new QLabel(tr("Min resting hours"));
				spinBox=new QSpinBox;

				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				checkBox=new QCheckBox(tr("Circular"));
				checkBox->setChecked(true);

				break;
			}
		//71
		case CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max span per day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxSpanPerDay");
				}
				else{
					dialogTitle=tr("Modify students set max span per day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxSpanPerDay");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max span per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				break;
			}
		//72
		case CONSTRAINT_STUDENTS_MAX_SPAN_PER_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max span per day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxSpanPerDay");
				}
				else{
					dialogTitle=tr("Modify students max span per day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxSpanPerDay");
				}

				labelForSpinBox=new QLabel(tr("Max span per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				break;
			}
		//73
		case CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set min resting hours", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMinRestingHours");
				}
				else{
					dialogTitle=tr("Modify students set min resting hours", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMinRestingHours");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min resting hours"));
				spinBox=new QSpinBox;

				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				checkBox=new QCheckBox(tr("Circular"));
				checkBox->setChecked(true);

				break;
			}
		//74
		case CONSTRAINT_STUDENTS_MIN_RESTING_HOURS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students min resting hours", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMinRestingHours");
				}
				else{
					dialogTitle=tr("Modify students min resting hours", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMinRestingHours");
				}

				labelForSpinBox=new QLabel(tr("Min resting hours"));
				spinBox=new QSpinBox;

				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				checkBox=new QCheckBox(tr("Circular"));
				checkBox->setChecked(true);

				break;
			}
		//75
		case CONSTRAINT_TWO_ACTIVITIES_ORDERED_IF_SAME_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add two activities ordered if same day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTwoActivitiesOrderedIfSameDay");
				}
				else{
					dialogTitle=tr("Modify two activities ordered if same day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTwoActivitiesOrderedIfSameDay");
				}

				/*addEmpty=true; -> does not affect us
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;*/

				/*activityLabel=new QLabel(tr("Activity"));
				activitiesComboBox=new QComboBox;*/

				first_filterGroupBox=new QGroupBox(tr("First filter"));
				first_filterTeachersComboBox=new QComboBox;
				first_filterStudentsComboBox=new QComboBox;
				first_filterSubjectsComboBox=new QComboBox;
				first_filterActivityTagsComboBox=new QComboBox;

				second_filterGroupBox=new QGroupBox(tr("Second filter"));
				second_filterTeachersComboBox=new QComboBox;
				second_filterStudentsComboBox=new QComboBox;
				second_filterSubjectsComboBox=new QComboBox;
				second_filterActivityTagsComboBox=new QComboBox;

				first_activityLabel=new QLabel(tr("First activity"));
				first_activitiesComboBox=new QComboBox;

				second_activityLabel=new QLabel(tr("Second activity"));
				second_activitiesComboBox=new QComboBox;

				swapActivitiesPushButton=new QPushButton(tr("Swap"));

				break;
			}
		//76
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set min gaps between ordered pair of activity tags", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags");
				}
				else{
					dialogTitle=tr("Modify students set min gaps between ordered pair of activity tags", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				first_activityTagLabel=new QLabel(tr("First activity tag"));
				first_activityTagsComboBox=new QComboBox;
				second_activityTagLabel=new QLabel(tr("Second activity tag"));
				second_activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				swapActivityTagsPushButton=new QPushButton(tr("Swap"));

				break;
			}
		//77
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students min gaps between ordered pair of activity tags", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags");
				}
				else{
					dialogTitle=tr("Modify students min gaps between ordered pair of activity tags", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags");
				}

				first_activityTagLabel=new QLabel(tr("First activity tag"));
				first_activityTagsComboBox=new QComboBox;
				second_activityTagLabel=new QLabel(tr("Second activity tag"));
				second_activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				swapActivityTagsPushButton=new QPushButton(tr("Swap"));

				break;
			}
		//78
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher min gaps between ordered pair of activity tags", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags");
				}
				else{
					dialogTitle=tr("Modify teacher min gaps between ordered pair of activity tags", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				first_activityTagLabel=new QLabel(tr("First activity tag"));
				first_activityTagsComboBox=new QComboBox;
				second_activityTagLabel=new QLabel(tr("Second activity tag"));
				second_activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				swapActivityTagsPushButton=new QPushButton(tr("Swap"));

				break;
			}
		//79
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers min gaps between ordered pair of activity tags", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags");
				}
				else{
					dialogTitle=tr("Modify teachers min gaps between ordered pair of activity tags", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags");
				}

				first_activityTagLabel=new QLabel(tr("First activity tag"));
				first_activityTagsComboBox=new QComboBox;
				second_activityTagLabel=new QLabel(tr("Second activity tag"));
				second_activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				swapActivityTagsPushButton=new QPushButton(tr("Swap"));

				break;
			}
		//80
		case CONSTRAINT_ACTIVITY_TAGS_NOT_OVERLAPPING:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activity tags not overlapping", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivityTagsNotOverlapping");
				}
				else{
					dialogTitle=tr("Modify activity tags not overlapping", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivityTagsNotOverlapping");
				}

				activityTagsLabel=new QLabel(tr("Activity tags"));
				selectedActivityTagsLabel=new QLabel(tr("Selected", "It refers to activity tags"));
				activityTagsListWidget=new QListWidget;
				selectedActivityTagsListWidget=new QListWidget;
				addAllActivityTagsPushButton=new QPushButton(tr("All", "Add all the activity tags to the list of selected activity tags"));
				clearActivityTagsPushButton=new QPushButton(tr("Clear", "Clear the list of selected activity tags"));

				break;
			}
		//81
		case CONSTRAINT_ACTIVITIES_OCCUPY_MIN_TIME_SLOTS_FROM_SELECTION:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities occupy min time slots from selection", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesOccupyMinTimeSlotsFromSelection");

					firstAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}
				else{
					dialogTitle=tr("Modify activities occupy min time slots from selection", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesOccupyMinTimeSlotsFromSelection");

					firstModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), true);

				labelForSpinBox=new QLabel(tr("Min occupied"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				tabWidget=new QTabWidget;

				break;
			}
		//82
		case CONSTRAINT_ACTIVITIES_MIN_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities min simultaneous in selected time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesMinSimultaneousInSelectedTimeSlots");

					firstAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}
				else{
					dialogTitle=tr("Modify activities min simultaneous in selected time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesMinSimultaneousInSelectedTimeSlots");

					firstModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), true);

				labelForSpinBox=new QLabel(tr("Min simultaneous"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(MAX_ACTIVITIES);
				spinBox->setValue(0);

				tabWidget=new QTabWidget;

				allowEmptySlotsCheckBox=new QCheckBox(tr("Allow empty slots"));
				allowEmptySlotsCheckBox->setChecked(false);

				break;
			}
		//83
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers activity tag min hours daily", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersActivityTagMinHoursDaily");
				}
				else{
					dialogTitle=tr("Modify teachers activity tag min hours daily", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersActivityTagMinHoursDaily");
				}

				labelForSpinBox=new QLabel(tr("Min hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				secondLabelForSpinBox=new QLabel(tr("Min days with tag", "The minimum number of days in the teachers' timetable with activities having this activity tag"));
				secondSpinBox=new QSpinBox;
				secondSpinBox->setMinimum(0);
				secondSpinBox->setMaximum(gt.rules.nDaysPerWeek);
				secondSpinBox->setValue(0);

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				/*checkBox=new QCheckBox(tr("Allow empty days"));
				checkBox->setChecked(false);*/

				break;
			}
		//84
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher activity tag min hours daily", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherActivityTagMinHoursDaily");
				}
				else{
					dialogTitle=tr("Modify teacher activity tag min hours daily", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherActivityTagMinHoursDaily");
				}

				labelForSpinBox=new QLabel(tr("Min hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				secondLabelForSpinBox=new QLabel(tr("Min days with tag", "The minimum number of days in the teacher's timetable with activities having this activity tag"));
				secondSpinBox=new QSpinBox;
				secondSpinBox->setMinimum(0);
				secondSpinBox->setMaximum(gt.rules.nDaysPerWeek);
				secondSpinBox->setValue(0);

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;
				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				/*checkBox=new QCheckBox(tr("Allow empty days"));
				checkBox->setChecked(false);*/

				break;
			}
		//85
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students activity tag min hours daily", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsActivityTagMinHoursDaily");
				}
				else{
					dialogTitle=tr("Modify students activity tag min hours daily", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsActivityTagMinHoursDaily");
				}

				labelForSpinBox=new QLabel(tr("Min hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				secondLabelForSpinBox=new QLabel(tr("Min days with tag", "The minimum number of days in the students' timetable with activities having this activity tag"));
				secondSpinBox=new QSpinBox;
				secondSpinBox->setMinimum(0);
				secondSpinBox->setMaximum(gt.rules.nDaysPerWeek);
				secondSpinBox->setValue(0);

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				/*checkBox=new QCheckBox(tr("Allow empty days"));
				checkBox->setChecked(false);*/

				break;
			}
		//86
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set activity tag min hours daily", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetActivityTagMinHoursDaily");
				}
				else{
					dialogTitle=tr("Modify students set activity tag min hours daily", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetActivityTagMinHoursDaily");
				}

				labelForSpinBox=new QLabel(tr("Min hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				secondLabelForSpinBox=new QLabel(tr("Min days with tag", "The minimum number of days in the students set's timetable with activities having this activity tag"));
				secondSpinBox=new QSpinBox;
				secondSpinBox->setMinimum(0);
				secondSpinBox->setMaximum(gt.rules.nDaysPerWeek);
				secondSpinBox->setValue(0);

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;
				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				/*checkBox=new QCheckBox(tr("Allow empty days"));
				checkBox->setChecked(false);*/

				break;
			}
		//87
		case CONSTRAINT_ACTIVITY_ENDS_TEACHERS_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activity ends teachers day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivityEndsTeachersDay");
				}
				else{
					dialogTitle=tr("Modify activity ends teachers day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivityEndsTeachersDay");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activityLabel=new QLabel(tr("Activity"));
				activitiesComboBox=new QComboBox;

				break;
			}
		//88
		case CONSTRAINT_ACTIVITIES_END_TEACHERS_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities end teachers day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesEndTeachersDay");

					firstAddInstructionsLabel=new QLabel(tr("Instructions: you can choose a set of activities with a certain teacher (or "
															"blank for all teachers), students set, subject and activity tag."));
				}
				else{
					dialogTitle=tr("Modify activities end teachers day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesEndTeachersDay");

					firstModifyInstructionsLabel=new QLabel(tr("Instructions: you can choose a set of activities with a certain teacher (or "
															   "blank for all teachers), students set, subject and activity tag."));
				}

				addEmpty=true;

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//89
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_REAL_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max hours daily real days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxHoursDailyRealDays");
				}
				else{
					dialogTitle=tr("Modify teachers max hours daily real days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxHoursDailyRealDays");
				}

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				break;
			}
		//90
		case CONSTRAINT_TEACHER_MAX_REAL_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max real days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxRealDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify teacher max real days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxRealDaysPerWeek");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//91
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max hours daily real days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxHoursDailyRealDays");
				}
				else{
					dialogTitle=tr("Modify teacher max hours daily real days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxHoursDailyRealDays");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				break;
			}
		//92
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_REAL_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max hours daily real days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxHoursDailyRealDays");
				}
				else{
					dialogTitle=tr("Modify students max hours daily real days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxHoursDailyRealDays");
				}

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				break;
			}
		//93
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_REAL_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max hours daily real days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxHoursDailyRealDays");
				}
				else{
					dialogTitle=tr("Modify students set max hours daily real days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxHoursDailyRealDays");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				break;
			}
		//94
		case CONSTRAINT_TEACHERS_MAX_REAL_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max real days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxRealDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify teachers max real days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxRealDaysPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//95
		case CONSTRAINT_TEACHERS_MIN_REAL_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers min real days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinRealDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify teachers min real days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinRealDaysPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Min days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(1);

				break;
			}
		//96
		case CONSTRAINT_TEACHER_MIN_REAL_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher min real days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinRealDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify teacher min real days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinRealDaysPerWeek");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(1);

				break;
			}
		//97
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers activity tag max hours daily real days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersActivityTagMaxHoursDailyRealDays");
				}
				else{
					dialogTitle=tr("Modify teachers activity tag max hours daily real days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersActivityTagMaxHoursDailyRealDays");
				}

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//98
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher activity tag max hours daily real days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherActivityTagMaxHoursDailyRealDays");
				}
				else{
					dialogTitle=tr("Modify teacher activity tag max hours daily real days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherActivityTagMaxHoursDailyRealDays");
				}

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;
				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//99
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students activity tag max hours daily real days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsActivityTagMaxHoursDailyRealDays");
				}
				else{
					dialogTitle=tr("Modify students activity tag max hours daily real days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsActivityTagMaxHoursDailyRealDays");
				}

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//100
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set activity tag max hours daily real days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetActivityTagMaxHoursDailyRealDays");
				}
				else{
					dialogTitle=tr("Modify students set activity tag max hours daily real days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetActivityTagMaxHoursDailyRealDays");
				}

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;
				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//101
		case CONSTRAINT_TEACHER_MAX_AFTERNOONS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max afternoons per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxAfternoonsPerWeek");
				}
				else{
					dialogTitle=tr("Modify teacher max afternoons per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxAfternoonsPerWeek");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max afternoons per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//102
		case CONSTRAINT_TEACHERS_MAX_AFTERNOONS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max afternoons per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxAfternoonsPerWeek");
				}
				else{
					dialogTitle=tr("Modify teachers max afternoons per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxAfternoonsPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max afternoons per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//103
		case CONSTRAINT_TEACHER_MAX_MORNINGS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max mornings per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxMorningsPerWeek");
				}
				else{
					dialogTitle=tr("Modify teacher max mornings per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxMorningsPerWeek");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max mornings per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//104
		case CONSTRAINT_TEACHERS_MAX_MORNINGS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max mornings per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxMorningsPerWeek");
				}
				else{
					dialogTitle=tr("Modify teachers max mornings per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxMorningsPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max mornings per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//105
		case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max activity tags per day from set", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxActivityTagsPerDayFromSet");
				}
				else{
					dialogTitle=tr("Modify teacher max activity tags per day from set", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxActivityTagsPerDayFromSet");
				}

				labelForSpinBox=new QLabel(tr("Max activity tags per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				activityTagsLabel=new QLabel(tr("Activity tags"));
				selectedActivityTagsLabel=new QLabel(tr("Selected", "It refers to activity tags"));
				activityTagsListWidget=new QListWidget;
				selectedActivityTagsListWidget=new QListWidget;
				addAllActivityTagsPushButton=new QPushButton(tr("All", "Add all the activity tags to the list of selected activity tags"));
				clearActivityTagsPushButton=new QPushButton(tr("Clear", "Clear the list of selected activity tags"));

				break;
			}
		//106
		case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max activity tags per day from set", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxActivityTagsPerDayFromSet");
				}
				else{
					dialogTitle=tr("Modify teachers max activity tags per day from set", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxActivityTagsPerDayFromSet");
				}

				labelForSpinBox=new QLabel(tr("Max activity tags per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				activityTagsLabel=new QLabel(tr("Activity tags"));
				selectedActivityTagsLabel=new QLabel(tr("Selected", "It refers to activity tags"));
				activityTagsListWidget=new QListWidget;
				selectedActivityTagsListWidget=new QListWidget;
				addAllActivityTagsPushButton=new QPushButton(tr("All", "Add all the activity tags to the list of selected activity tags"));
				clearActivityTagsPushButton=new QPushButton(tr("Clear", "Clear the list of selected activity tags"));

				break;
			}
		//107
		case CONSTRAINT_TEACHERS_MIN_MORNINGS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers min mornings per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinMorningsPerWeek");
				}
				else{
					dialogTitle=tr("Modify teachers min mornings per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinMorningsPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Min mornings per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(1);

				break;
			}
		//108
		case CONSTRAINT_TEACHER_MIN_MORNINGS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher min mornings per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinMorningsPerWeek");
				}
				else{
					dialogTitle=tr("Modify teacher min mornings per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinMorningsPerWeek");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min mornings per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(1);

				break;
			}
		//109
		case CONSTRAINT_TEACHERS_MIN_AFTERNOONS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers min afternoons per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinAfternoonsPerWeek");
				}
				else{
					dialogTitle=tr("Modify teachers min afternoons per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinAfternoonsPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Min afternoons per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(1);

				break;
			}
		//110
		case CONSTRAINT_TEACHER_MIN_AFTERNOONS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher min afternoons per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinAfternoonsPerWeek");
				}
				else{
					dialogTitle=tr("Modify teacher min afternoons per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinAfternoonsPerWeek");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min afternoons per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(1);

				break;
			}
		//111
		case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_MORNINGS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max two consecutive mornings", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxTwoConsectuiveMornings");
				}
				else{
					dialogTitle=tr("Modify teacher max two consecutive mornings", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxTwoConsecutiveMornings");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				break;
			}
		//112
		case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_MORNINGS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max two consecutive mornings", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxTwoConsecutiveMornings");
				}
				else{
					dialogTitle=tr("Modify teachers max two consecutive mornings", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxTwoConsecutiveMornings");
				}

				break;
			}
		//113
		case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_AFTERNOONS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max two consecutive afternoons", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxTwoConsecutiveAfternoons");
				}
				else{
					dialogTitle=tr("Modify teacher max two consecutive afternoons", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxTwoConsecutiveAfternoons");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				break;
			}
		//114
		case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_AFTERNOONS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max two consecutive afternoons", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxTwoConsectuiveAfternoons");
				}
				else{
					dialogTitle=tr("Modify teachers max two consecutive afternoons", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxTwoConsecutiveAfternoons");
				}

				break;
			}
		//115
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_REAL_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max gaps per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxGapsPerRealDay");

					firstAddInstructionsLabel=new QLabel(tr("It is recommended to use only one constraint of this type. You must use weight "
															"percentage 100%, because otherwise you will get poor timetables. Try to fix firstly "
															"a rather large value for max gaps per real day and lower it as you find new timetables.\n"
															"Note: teacher not available and break are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify teachers max gaps per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxGapsPerRealDay");

					firstModifyInstructionsLabel=new QLabel(tr("It is recommended to use only one constraint of this type. You must use weight "
															   "percentage 100%, because otherwise you will get poor timetables. Try to fix firstly "
															   "a rather large value for max gaps per day and lower it as you find new timetables.\n"
															   "Note: teacher not available and break are not counted as gaps."));
				}

				labelForSpinBox=new QLabel(tr("Max gaps per real day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				checkBox=new QCheckBox(tr("Allow one day exception of plus one"));
				checkBox->setChecked(false);

				break;
			}
		//116
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max gaps per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxGapsPerRealDay");

					firstAddInstructionsLabel=new QLabel(tr("You must use weight percentage 100%, because otherwise you will get poor timetables. Try "
															"to fix firstly a rather large value for max gaps per real day and lower it as you find new "
															"timetables.\nNote: teacher not available and break are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify teacher max gaps per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxGapsPerRealDay");

					firstModifyInstructionsLabel=new QLabel(tr("You must use weight percentage 100%, because otherwise you will get poor timetables. Try "
															   "to fix firstly a rather large value for max gaps per real day and lower it as you find new timetables.\n"
															   "Note: teacher not available and break are not counted as gaps."));
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max gaps per real day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				checkBox=new QCheckBox(tr("Allow one day exception of plus one"));
				checkBox->setChecked(false);

				break;
			}
		//117
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_REAL_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max gaps per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxGapsPerRealDay");

					firstAddInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%\nNote: students set not available and break "
															"are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify students max gaps per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxGapsPerRealDay");

					firstModifyInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%\nNote: students set not available and break "
															   "are not counted as gaps."));
				}

				labelForSpinBox=new QLabel(tr("Max gaps per real day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				break;
			}
		//118
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_REAL_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max gaps per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxGapsPerRealDay");

					firstAddInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%\nNote: students "
															"set not available and break are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify students set max gaps per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxGapsPerRealDay");

					firstModifyInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%\nNote: students "
															   "set not available and break are not counted as gaps."));
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max gaps per real day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				break;
			}
		//119
		case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY_REAL_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers min hours daily for real days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinHoursDailyRealDays");

					firstAddInstructionsLabel=new QLabel(tr("It is recommended to start with 2 min hours. Maybe you could try higher values after "
															"that, but they might be too large and you might get an impossible timetable."));
				}
				else{
					dialogTitle=tr("Modify teachers min hours daily for real days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinHoursDailyRealDays");

					firstModifyInstructionsLabel=new QLabel(tr("It is recommended to start with 2 min hours. Maybe you could try higher values after "
															   "that, but they might be too large and you might get an impossible timetable."));
				}

				labelForSpinBox=new QLabel(tr("Min hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(2);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				checkBox=new QCheckBox(tr("Allow empty days"));
				checkBox->setChecked(true);

				break;
			}
		//120
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher min hours daily for real days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinHoursDailyRealDays");

					firstAddInstructionsLabel=new QLabel(tr("It is recommended to start with 2 min hours and strengthen them as you find new timetables."));
				}
				else{
					dialogTitle=tr("Modify teacher min hours daily for real days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinHoursDailyRealDays");

					firstModifyInstructionsLabel=new QLabel(tr("It is recommended to start with 2 min hours and strengthen them as you find new timetables."));
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(2);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				checkBox=new QCheckBox(tr("Allow empty days"));
				checkBox->setChecked(true);

				break;
			}
		//121
		case CONSTRAINT_TEACHERS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers afternoons early max beginnings at second hour", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHour");
				}
				else{
					dialogTitle=tr("Modify teachers afternoons early max beginnings at second hour", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHour");
				}

				labelForSpinBox=new QLabel(tr("Max beginnings at second hour"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//122
		case CONSTRAINT_TEACHER_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher afternoons early max beginnings at second hour", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour");
				}
				else{
					dialogTitle=tr("Modify teacher afternoons early max beginnings at second hour", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max beginnings at second hour"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//123
		case CONSTRAINT_TEACHERS_MIN_HOURS_PER_MORNING:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers min hours per morning", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinHoursPerMorning");
				}
				else{
					dialogTitle=tr("Modify teachers min hours per morning", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinHoursPerMorning");
				}

				labelForSpinBox=new QLabel(tr("Min hours per morning"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(2);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				checkBox=new QCheckBox(tr("Allow empty mornings"));
				checkBox->setChecked(true);

				break;
			}
		//124
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher min hours per morning", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinHoursPerMorning");
				}
				else{
					dialogTitle=tr("Modify teacher min hours per morning", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinHoursPerMorning");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min hours per morning"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(2);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				checkBox=new QCheckBox(tr("Allow empty mornings"));
				checkBox->setChecked(true);

				break;
			}
		//125
		case CONSTRAINT_TEACHER_MAX_SPAN_PER_REAL_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max span per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxSpanPerRealDay");
				}
				else{
					dialogTitle=tr("Modify teacher max span per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxSpanPerRealDay");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max span per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				checkBox=new QCheckBox(tr("Allow one day exception of plus one"));
				checkBox->setChecked(false);

				break;
			}
		//126
		case CONSTRAINT_TEACHERS_MAX_SPAN_PER_REAL_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max span per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxSpanPerRealDay");
				}
				else{
					dialogTitle=tr("Modify teachers max span per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxSpanPerRealDay");
				}

				labelForSpinBox=new QLabel(tr("Max span per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				checkBox=new QCheckBox(tr("Allow one day exception of plus one"));
				checkBox->setChecked(false);

				break;
			}
		//127
		case CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_REAL_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max span per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxSpanPerRealDay");
				}
				else{
					dialogTitle=tr("Modify students set max span per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxSpanPerRealDay");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max span per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				break;
			}
		//128
		case CONSTRAINT_STUDENTS_MAX_SPAN_PER_REAL_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max span per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxSpanPerRealDay");
				}
				else{
					dialogTitle=tr("Modify students max span per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxSpanPerRealDay");
				}

				labelForSpinBox=new QLabel(tr("Max span per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2*gt.rules.nHoursPerDay);

				break;
			}
		//129
		case CONSTRAINT_TEACHER_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher morning interval max days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMorningIntervalMaxDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify teacher morning interval max days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMorningIntervalMaxDaysPerWeek");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//130
		case CONSTRAINT_TEACHERS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers morning interval max days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMorningIntervalMaxDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify teachers morning interval max days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMorningIntervalMaxDaysPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//131
		case CONSTRAINT_TEACHER_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher afternoon interval max days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherAfternoonIntervalMaxDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify teacher afternoon interval max days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherAfternoonIntervalMaxDaysPerWeek");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//132
		case CONSTRAINT_TEACHERS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers afternoon interval max days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersAfternoonIntervalMaxDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify teachers afternoon interval max days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersAfternoonIntervalMaxDaysPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//133
		case CONSTRAINT_STUDENTS_MIN_HOURS_PER_MORNING:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students min hours per morning", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMinHoursPerMorning");
				}
				else{
					dialogTitle=tr("Modify students min hours per morning", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMinHoursPerMorning");
				}

				labelForSpinBox=new QLabel(tr("Min hours"));
				spinBox=new QSpinBox;

				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);
				checkBox=new QCheckBox(tr("Allow empty mornings"));
				checkBox->setChecked(true);

				break;
			}
		//134
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_MORNING:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set min hours per morning", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMinHoursPerMorning");
				}
				else{
					dialogTitle=tr("Modify students set min hours per morning", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMinHoursPerMorning");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min hours"));
				spinBox=new QSpinBox;

				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);
				checkBox=new QCheckBox(tr("Allow empty mornings"));
				checkBox->setChecked(true);

				break;
			}
		//135
		case CONSTRAINT_TEACHER_MAX_ZERO_GAPS_PER_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max zero gaps per afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxZeroGapsPerAfternoon");

					firstAddInstructionsLabel=new QLabel(tr("Note: teacher not available and break are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify teacher max zero gaps per afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxZeroGapsPerAfternoon");

					firstModifyInstructionsLabel=new QLabel(tr("Note: teacher not available and break are not counted as gaps."));
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				break;
			}
		//136
		case CONSTRAINT_TEACHERS_MAX_ZERO_GAPS_PER_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max zero gaps per afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxZeroGapsPerAfternoon");

					firstAddInstructionsLabel=new QLabel(tr("Note: teacher not available and break are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify teachers max zero gaps per afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxZeroGapsPerAfternoon");

					firstModifyInstructionsLabel=new QLabel(tr("Note: teacher not available and break are not counted as gaps."));
				}

				break;
			}
		//137
		case CONSTRAINT_STUDENTS_SET_MAX_AFTERNOONS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max afternoons per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxAfternoonsPerWeek");
				}
				else{
					dialogTitle=tr("Modify students set max afternoons per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxAfternoonsPerWeek");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max afternoons per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//138
		case CONSTRAINT_STUDENTS_MAX_AFTERNOONS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max afternoons per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxAfternoonsPerWeek");
				}
				else{
					dialogTitle=tr("Modify students max afternoons per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxAfternoonsPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max afternoons per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//139
		case CONSTRAINT_STUDENTS_SET_MAX_MORNINGS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max mornings per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxMorningsPerWeek");
				}
				else{
					dialogTitle=tr("Modify students set max mornings per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxMorningsPerWeek");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max mornings per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//140
		case CONSTRAINT_STUDENTS_MAX_MORNINGS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max mornings per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxMorningsPerWeek");
				}
				else{
					dialogTitle=tr("Modify students max mornings per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxMorningsPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max mornings per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//141
		case CONSTRAINT_STUDENTS_MIN_MORNINGS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students min mornings per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMinMorningsPerWeek");
				}
				else{
					dialogTitle=tr("Modify students min mornings per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMinMorningsPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Min mornings per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(1);

				break;
			}
		//142
		case CONSTRAINT_STUDENTS_SET_MIN_MORNINGS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set min mornings per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMinMorningsPerWeek");
				}
				else{
					dialogTitle=tr("Modify students set min mornings per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMinMorningsPerWeek");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min mornings per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(1);

				break;
			}
		//143
		case CONSTRAINT_STUDENTS_MIN_AFTERNOONS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students min afternoons per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMinAfternoonsPerWeek");
				}
				else{
					dialogTitle=tr("Modify students min afternoons per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMinAfternoonsPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Min afternoons per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(1);

				break;
			}
		//144
		case CONSTRAINT_STUDENTS_SET_MIN_AFTERNOONS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set min afternoons per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMinAfternoonsPerWeek");
				}
				else{
					dialogTitle=tr("Modify students set min afternoons per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMinAfternoonsPerWeek");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min afternoons per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(1);

				break;
			}
		//145
		case CONSTRAINT_STUDENTS_SET_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set morning interval max days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMorningIntervalMaxDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify students set morning interval max days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMorningIntervalMaxDaysPerWeek");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//146
		case CONSTRAINT_STUDENTS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students morning interval max days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMorningIntervalMaxDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify students morning interval max days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMorningIntervalMaxDaysPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//147
		case CONSTRAINT_STUDENTS_SET_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set afternoon interval max days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify students set afternoon interval max days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//148
		case CONSTRAINT_STUDENTS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students afternoon interval max days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsAfternoonIntervalMaxDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify students afternoon interval max days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsAfternoonIntervalMaxDaysPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//149
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max hours per all afternoons", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxHoursPerAllAfternoons");
				}
				else{
					dialogTitle=tr("Modify teacher max hours per all afternoons", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxHoursPerAllAfternoons");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max hours per all afternoons"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay/2);
				spinBox->setValue(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay/2);

				break;
			}
		//150
		case CONSTRAINT_TEACHERS_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max hours per all afternoons", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxHoursPerAllAfternoons");
				}
				else{
					dialogTitle=tr("Modify teachers max hours per all afternoons", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxHoursPerAllAfternoons");
				}

				labelForSpinBox=new QLabel(tr("Max hours per all afternoons"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay/2);
				spinBox->setValue(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay/2);

				break;
			}
		//151
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max hours per all afternoons", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxHoursPerAllAfternoons");
				}
				else{
					dialogTitle=tr("Modify students set max hours per all afternoons", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxHoursPerAllAfternoons");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max hours per all afternoons"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay/2);
				spinBox->setValue(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay/2);

				break;
			}
		//152
		case CONSTRAINT_STUDENTS_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max hours per all afternoons", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxHoursPerAllAfternoons");
				}
				else{
					dialogTitle=tr("Modify students max hours per all afternoons", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxHoursPerAllAfternoons");
				}

				labelForSpinBox=new QLabel(tr("Max hours per all afternoons"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay/2);
				spinBox->setValue(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay/2);

				break;
			}
		//153
		case CONSTRAINT_TEACHER_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher min resting hours between morning and afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon");
				}
				else{
					dialogTitle=tr("Modify teacher min resting hours between morning and afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min resting hours\nbetween morning and afternoon"));
				spinBox=new QSpinBox;

				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//154
		case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers min resting hours between morning and afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinRestingHoursBetweenMorningAndAfternoon");
				}
				else{
					dialogTitle=tr("Modify teachers min resting hours between morning and afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinRestingHoursBetweenMorningAndAfternoon");
				}

				labelForSpinBox=new QLabel(tr("Min resting hours\nbetween morning and afternoon"));
				spinBox=new QSpinBox;

				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//155
		case CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set min resting hours between morning and afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon");
				}
				else{
					dialogTitle=tr("Modify students set min resting hours between morning and afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min resting hours\nbetween morning and afternoon"));
				spinBox=new QSpinBox;

				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//156
		case CONSTRAINT_STUDENTS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students min resting hours between morning and afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon");
				}
				else{
					dialogTitle=tr("Modify students min resting hours between morning and afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon");
				}

				labelForSpinBox=new QLabel(tr("Min resting hours\nbetween morning and afternoon"));
				spinBox=new QSpinBox;

				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//157
		case CONSTRAINT_STUDENTS_SET_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set afternoons early max beginnings at second hour", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour");
				}
				else{
					dialogTitle=tr("Modify students set afternoons early max beginnings at second hour", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max beginnings at second hour"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(0);

				break;
			}
		//158
		case CONSTRAINT_STUDENTS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students afternoons early max beginnings at second hour", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHour");
				}
				else{
					dialogTitle=tr("Modify students afternoons early max beginnings at second hour", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHour");
				}

				labelForSpinBox=new QLabel(tr("Max beginnings at second hour"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(0);

				break;
			}
		//159
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max gaps per week for real days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxGapsPerWeekForRealDays");

					firstAddInstructionsLabel=new QLabel(tr("It is recommended to use only one constraint of this type. You must use weight percentage "
															"100%, because otherwise you will get poor timetables. Try to fix firstly a rather large value "
															"for max gaps per real day and lower it as you find new timetables. Note: teacher not available "
															"and break are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify teachers max gaps per week for real days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxGapsPerWeekForRealDays");

					firstModifyInstructionsLabel=new QLabel(tr("It is recommended to use only one constraint of this type. You must use weight percentage "
															   "100%, because otherwise you will get poor timetables. Try to fix firstly a rather large "
															   "value for max gaps per real day and lower it as you find new timetables. Note: teacher "
															   "not available and break are not counted as gaps."));
				}

				labelForSpinBox=new QLabel(tr("Max gaps per week for real days"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);

				break;
			}
		//160
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max gaps per week for real days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxGapsPerWeekForRealDays");

					firstAddInstructionsLabel=new QLabel(tr("You must use weight percentage 100%, because otherwise you will get poor timetables. "
															"Try to fix firstly a rather large value for max gaps per real day and lower it as you "
															"find new timetables.\nNote: teacher not available and break are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify teacher max gaps per week for real days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxGapsPerWeekForRealDays");

					firstModifyInstructionsLabel=new QLabel(tr("You must use weight percentage 100%, because otherwise you will get poor timetables. "
															   "Try to fix firstly a rather large value for max gaps per real day and lower it as you "
															   "find new timetables.\nNote: teacher not available and break are not counted as gaps."));
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max gaps per week for real days"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);

				break;
			}
		//161
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max gaps per week for real days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxGapsPerWeekForRealDays");

					firstAddInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%\nNote: students set not available and break "
															"are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify students max gaps per week for real days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxGapsPerWeekForRealDays");

					firstModifyInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%\nNote: students set not available and break "
															   "are not counted as gaps."));
				}

				labelForSpinBox=new QLabel(tr("Max gaps per week for real days"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);

				break;
			}
		//162
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max gaps per week for real days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxGapsPerWeekForRealDays");

					firstAddInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%\nNote: students set not available and break "
															"are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify students set max gaps per week for real days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxGapsPerWeekForRealDays");

					firstModifyInstructionsLabel=new QLabel(tr("You have to use weight percentage 100%\nNote: students set not available and break "
															   "are not counted as gaps."));
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max gaps per week for real days"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);

				break;
			}
		//163
		case CONSTRAINT_STUDENTS_SET_MAX_REAL_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max real days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxRealDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify students set max real days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxRealDaysPerWeek");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//164
		case CONSTRAINT_STUDENTS_MAX_REAL_DAYS_PER_WEEK:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max real days per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxRealDaysPerWeek");
				}
				else{
					dialogTitle=tr("Modify students max real days per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxRealDaysPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max days per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//165
		case CONSTRAINT_MAX_TOTAL_ACTIVITIES_FROM_SET_IN_SELECTED_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add max total activities from set in selected time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots");

					firstAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}
				else{
					dialogTitle=tr("Modify max total activities from set in selected time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots");

					firstModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), true);

				labelForSpinBox=new QLabel(tr("Max total activities"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(MAX_ACTIVITIES);
				spinBox->setValue(0);

				tabWidget=new QTabWidget;

				break;
			}
		//166
		case CONSTRAINT_MAX_GAPS_BETWEEN_ACTIVITIES:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add max gaps between activities", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintMaxGapsBetweenActivities");
				}
				else{
					dialogTitle=tr("Modify max gaps between activities", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintMaxGapsBetweenActivities");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				labelForSpinBox=new QLabel(tr("Max gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				break;
			}
		//167
		case CONSTRAINT_ACTIVITIES_MAX_IN_A_TERM:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities max in a term", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesMaxInATerm");
				}
				else{
					dialogTitle=tr("Modify activities max in a term", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesMaxInATerm");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				labelForSpinBox=new QLabel(tr("Max activities in a term"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(MAX_ACTIVITIES);
				spinBox->setValue(1);

				break;
			}
		//168
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TERMS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities occupy max terms", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesOccupyMaxTerms");
				}
				else{
					dialogTitle=tr("Modify activities occupy max terms", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesOccupyMaxTerms");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				labelForSpinBox=new QLabel(tr("Max occupied terms"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nTerms);
				spinBox->setValue(gt.rules.nTerms);

				break;
			}
		//169
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max gaps per morning and afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxGapsPerMorningAndAfternoon");

					firstAddInstructionsLabel=new QLabel(tr("This constraint considers the gaps = the sum of gaps of the morning and of the afternoon of "
															"each real day."));
					secondAddInstructionsLabel=new QLabel(tr("It is recommended to use only one constraint of this type. You must use weight percentage 100%, "
															 "because otherwise you will get poor timetables. Try to fix firstly a rather large value for max "
															 "gaps per morning and afternoon and lower it as you find new timetables. The default given here, "
															 "1, might be too low for your school and you might need to use higher values.\nNote: teacher not "
															 "available and break are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify teachers max gaps per morning and afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxGapsPerMorningAndAfternoon");

					firstModifyInstructionsLabel=new QLabel(tr("This constraint considers the gaps = the sum of gaps of the morning and of the afternoon of "
															   "each real day."));
					secondModifyInstructionsLabel=new QLabel(tr("It is recommended to use only one constraint of this type. You must use weight percentage 100%, "
																"because otherwise you will get poor timetables. Try to fix firstly a rather large value for "
																"max gaps per morning and afternoon and lower it as you find new timetables.\nNote: teacher "
																"not available and break are not counted as gaps."));
				}

				labelForSpinBox=new QLabel(tr("Max gaps per morning and afternoon"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//170
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max gaps per morning and afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxGapsPerMorningAndAfternoon");

					firstAddInstructionsLabel=new QLabel(tr("This constraint considers the gaps = the sum of gaps of the morning and of the afternoon of "
															"each real day."));
					secondAddInstructionsLabel=new QLabel(tr("You must use weight percentage 100%, because otherwise you will get poor timetables. Try to fix "
															 "firstly a rather large value for max gaps per morning and afternoon and lower it as you find new "
															 "timetables. The default given here, 1, might be too low for your school and you might need to "
															 "use higher values.\nNote: teacher not available and break are not counted as gaps."));
				}
				else{
					dialogTitle=tr("Modify teacher max gaps per morning and afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxGapsPerMorningAndAfternoon");

					firstModifyInstructionsLabel=new QLabel(tr("This constraint considers the gaps = the sum of gaps of the morning and of the afternoon of "
															   "each real day."));
					secondModifyInstructionsLabel=new QLabel(tr("You must use weight percentage 100%, because otherwise you will get poor timetables. Try to "
																"fix firstly a rather large value for max gaps per morning and afternoon and lower it as you "
																"find new timetables.\nNote: teacher not available and break are not counted as gaps."));
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max gaps per morning and afternoon"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//171
		case CONSTRAINT_TEACHERS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers mornings early max beginnings at second hour", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHour");
				}
				else{
					dialogTitle=tr("Modify teachers mornings early max beginnings at second hour", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHour");
				}

				labelForSpinBox=new QLabel(tr("Max beginnings at second hour"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//172
		case CONSTRAINT_TEACHER_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher mornings early max beginnings at second hour", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour");
				}
				else{
					dialogTitle=tr("Modify teacher mornings early max beginnings at second hour", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max beginnings at second hour"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(gt.rules.nDaysPerWeek/2);

				break;
			}
		//173
		case CONSTRAINT_STUDENTS_SET_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set mornings early max beginnings at second hour", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour");
				}
				else{
					dialogTitle=tr("Modify students set mornings early max beginnings at second hour", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max beginnings at second hour"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(0);

				break;
			}
		//174
		case CONSTRAINT_STUDENTS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students mornings early max beginnings at second hour", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour");
				}
				else{
					dialogTitle=tr("Modify students mornings early max beginnings at second hour", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour");
				}

				labelForSpinBox=new QLabel(tr("Max beginnings at second hour"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek/2);
				spinBox->setValue(0);

				break;
			}
		//175
		case CONSTRAINT_TWO_SETS_OF_ACTIVITIES_ORDERED:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add two sets of activities ordered", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTwoSetsOfActivitiesOrdered");
				}
				else{
					dialogTitle=tr("Modify two sets of activities ordered", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTwoSetsOfActivitiesOrdered");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				/*activityLabel=new QLabel(tr("Activity"));
				activitiesComboBox=new QComboBox;*/

				swapTwoSetsOfActivitiesPushButton=new QPushButton(tr("Swap"));

				tabWidgetTwoSetsOfActivities=new QTabWidget;
				//
				activitiesLabel_TwoSetsOfActivities_1=new QLabel(tr("Activities"));
				selectedActivitiesLabel_TwoSetsOfActivities_1=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget_TwoSetsOfActivities_1=new QListWidget;
				selectedActivitiesListWidget_TwoSetsOfActivities_1=new QListWidget;
				addAllActivitiesPushButton_TwoSetsOfActivities_1=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton_TwoSetsOfActivities_1=new QPushButton(tr("Clear", "Clear the list of selected activities"));
				//
				activitiesLabel_TwoSetsOfActivities_2=new QLabel(tr("Activities"));
				selectedActivitiesLabel_TwoSetsOfActivities_2=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget_TwoSetsOfActivities_2=new QListWidget;
				selectedActivitiesListWidget_TwoSetsOfActivities_2=new QListWidget;
				addAllActivitiesPushButton_TwoSetsOfActivities_2=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton_TwoSetsOfActivities_2=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				break;
			}
		//176
		case CONSTRAINT_TEACHERS_MAX_THREE_CONSECUTIVE_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max three consecutive days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxThreeConsecutiveDays");
				}
				else{
					dialogTitle=tr("Modify teachers max three consecutive days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxThreeConsecutiveDays");
				}

				checkBox=new QCheckBox(tr("Allow afternoon-morning-afternoon-morning exception"));
				checkBox->setChecked(false);

				break;
			}
		//177
		case CONSTRAINT_TEACHER_MAX_THREE_CONSECUTIVE_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max three consecutive days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxThreeConsecutiveDays");
				}
				else{
					dialogTitle=tr("Modify teacher max three consecutive days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxThreeConsecutiveDays");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				checkBox=new QCheckBox(tr("Allow afternoon-morning-afternoon-morning exception"));
				checkBox->setChecked(false);

				break;
			}
		//178
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set min gaps between activity tag", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMinGapsBetweenActivityTag");
				}
				else{
					dialogTitle=tr("Modify students set min gaps between activity tag", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMinGapsBetweenActivityTag");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//179
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students min gaps between activity tag", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMinGapsBetweenActivityTag");
				}
				else{
					dialogTitle=tr("Modify students min gaps between activity tag", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMinGapsBetweenActivityTag");
				}

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//180
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher min gaps between activity tag", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinGapsBetweenActivityTag");
				}
				else{
					dialogTitle=tr("Modify teacher min gaps between activity tag", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinGapsBetweenActivityTag");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//181
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers min gaps between activity tag", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinGapsBetweenActivityTag");
				}
				else{
					dialogTitle=tr("Modify teachers min gaps between activity tag", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinGapsBetweenActivityTag");
				}

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//182
		case CONSTRAINT_STUDENTS_MAX_THREE_CONSECUTIVE_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max three consecutive days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxThreeConsecutiveDays");
				}
				else{
					dialogTitle=tr("Modify students max three consecutive days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxThreeConsecutiveDays");
				}

				checkBox=new QCheckBox(tr("Allow afternoon-morning-afternoon-morning exception"));
				checkBox->setChecked(false);

				break;
			}
		//183
		case CONSTRAINT_STUDENTS_SET_MAX_THREE_CONSECUTIVE_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max three consecutive days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxThreeConsecutiveDays");
				}
				else{
					dialogTitle=tr("Modify students set max three consecutive days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxThreeConsecutiveDays");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				checkBox=new QCheckBox(tr("Allow afternoon-morning-afternoon-morning exception"));
				checkBox->setChecked(false);

				break;
			}
		//184
		case CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add min half days between activities", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintMinHalfDaysBetweenActivities");
				}
				else{
					dialogTitle=tr("Modify min half days between activities", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintMinHalfDaysBetweenActivities");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				labelForSpinBox=new QLabel(tr("Min half days"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek-1);
				spinBox->setValue(1);

				checkBox=new QCheckBox(tr("If two activities are on the same half day, make them consecutive"));
				checkBox->setChecked(true);

				break;
			}
		//185 is activity preferred day, which is not in the interface
		case CONSTRAINT_ACTIVITY_PREFERRED_DAY:
			{
				QMessageBox::warning(dialog, tr("FET information"),
									 tr("You have encountered a FET bug. The constraint activity preferred day should not "
										"be editable in a separate dialog. The request will be ignored."));

				return;

				break;
			}
		//186
		case CONSTRAINT_ACTIVITIES_MIN_IN_A_TERM:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities min in a term", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesMinInATerm");
				}
				else{
					dialogTitle=tr("Modify activities min in a term", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesMinInATerm");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				labelForSpinBox=new QLabel(tr("Min activities in a term"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(MAX_ACTIVITIES);
				spinBox->setValue(2);

				checkBox=new QCheckBox(tr("Allow empty terms"));
				checkBox->setChecked(true);

				break;
			}
		//187
		case CONSTRAINT_MAX_TERMS_BETWEEN_ACTIVITIES:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add max terms between activities", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintMaxTermsBetweenActivities");
				}
				else{
					dialogTitle=tr("Modify max terms between activities", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintMaxTermsBetweenActivities");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				labelForSpinBox=new QLabel(tr("Max terms"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setValue(gt.rules.nTerms-1);
				spinBox->setMaximum(gt.rules.nTerms-1);

				break;
			}
		//188
		case CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max activity tags per day from set", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxActivityTagsPerDayFromSet");
				}
				else{
					dialogTitle=tr("Modify students set max activity tags per day from set", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudenstSetMaxActivityTagsPerDayFromSet");
				}

				labelForSpinBox=new QLabel(tr("Max activity tags per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				activityTagsLabel=new QLabel(tr("Activity tags"));
				selectedActivityTagsLabel=new QLabel(tr("Selected", "It refers to activity tags"));
				activityTagsListWidget=new QListWidget;
				selectedActivityTagsListWidget=new QListWidget;
				addAllActivityTagsPushButton=new QPushButton(tr("All", "Add all the activity tags to the list of selected activity tags"));
				clearActivityTagsPushButton=new QPushButton(tr("Clear", "Clear the list of selected activity tags"));

				break;
			}
		//189
		case CONSTRAINT_STUDENTS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max activity tags per day from set", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxActivityTagsPerDayFromSet");
				}
				else{
					dialogTitle=tr("Modify students max activity tags per day from set", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudenstMaxActivityTagsPerDayFromSet");
				}

				labelForSpinBox=new QLabel(tr("Max activity tags per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				activityTagsLabel=new QLabel(tr("Activity tags"));
				selectedActivityTagsLabel=new QLabel(tr("Selected", "It refers to activity tags"));
				activityTagsListWidget=new QListWidget;
				selectedActivityTagsListWidget=new QListWidget;
				addAllActivityTagsPushButton=new QPushButton(tr("All", "Add all the activity tags to the list of selected activity tags"));
				clearActivityTagsPushButton=new QPushButton(tr("Clear", "Clear the list of selected activity tags"));

				break;
			}
		//190
		case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max activity tags per real day from set", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxActivityTagsPerRealDayFromSet");
				}
				else{
					dialogTitle=tr("Modify teacher max activity tags per real day from set", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxActivityTagsPerRealDayFromSet");
				}

				labelForSpinBox=new QLabel(tr("Max activity tags per real day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				activityTagsLabel=new QLabel(tr("Activity tags"));
				selectedActivityTagsLabel=new QLabel(tr("Selected", "It refers to activity tags"));
				activityTagsListWidget=new QListWidget;
				selectedActivityTagsListWidget=new QListWidget;
				addAllActivityTagsPushButton=new QPushButton(tr("All", "Add all the activity tags to the list of selected activity tags"));
				clearActivityTagsPushButton=new QPushButton(tr("Clear", "Clear the list of selected activity tags"));

				break;
			}
		//191
		case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max activity tags per real day from set", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxActivityTagsPerRealDayFromSet");
				}
				else{
					dialogTitle=tr("Modify teachers max activity tags per real day from set", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxActivityTagsPerRealDayFromSet");
				}

				labelForSpinBox=new QLabel(tr("Max activity tags per real day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				activityTagsLabel=new QLabel(tr("Activity tags"));
				selectedActivityTagsLabel=new QLabel(tr("Selected", "It refers to activity tags"));
				activityTagsListWidget=new QListWidget;
				selectedActivityTagsListWidget=new QListWidget;
				addAllActivityTagsPushButton=new QPushButton(tr("All", "Add all the activity tags to the list of selected activity tags"));
				clearActivityTagsPushButton=new QPushButton(tr("Clear", "Clear the list of selected activity tags"));

				break;
			}
		//192
		case CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max activity tags per real day from set", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxActivityTagsPerRealDayFromSet");
				}
				else{
					dialogTitle=tr("Modify students set max activity tags per real day from set", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudenstSetMaxActivityTagsPerRealDayFromSet");
				}

				labelForSpinBox=new QLabel(tr("Max activity tags per real day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				activityTagsLabel=new QLabel(tr("Activity tags"));
				selectedActivityTagsLabel=new QLabel(tr("Selected", "It refers to activity tags"));
				activityTagsListWidget=new QListWidget;
				selectedActivityTagsListWidget=new QListWidget;
				addAllActivityTagsPushButton=new QPushButton(tr("All", "Add all the activity tags to the list of selected activity tags"));
				clearActivityTagsPushButton=new QPushButton(tr("Clear", "Clear the list of selected activity tags"));

				break;
			}
		//193
		case CONSTRAINT_STUDENTS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max activity tags per real day from set", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxActivityTagsPerRealDayFromSet");
				}
				else{
					dialogTitle=tr("Modify students max activity tags per real day from set", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudenstMaxActivityTagsPerRealDayFromSet");
				}

				labelForSpinBox=new QLabel(tr("Max activity tags per real day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				activityTagsLabel=new QLabel(tr("Activity tags"));
				selectedActivityTagsLabel=new QLabel(tr("Selected", "It refers to activity tags"));
				activityTagsListWidget=new QListWidget;
				selectedActivityTagsListWidget=new QListWidget;
				addAllActivityTagsPushButton=new QPushButton(tr("All", "Add all the activity tags to the list of selected activity tags"));
				clearActivityTagsPushButton=new QPushButton(tr("Clear", "Clear the list of selected activity tags"));

				break;
			}
		//194
		case CONSTRAINT_MAX_HALF_DAYS_BETWEEN_ACTIVITIES:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add max half days between activities", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintMaxHalfDaysBetweenActivities");
				}
				else{
					dialogTitle=tr("Modify max half days between activities", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintMaxHalfDaysBetweenActivities");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				labelForSpinBox=new QLabel(tr("Max half days"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek-1);
				spinBox->setValue(gt.rules.nDaysPerWeek-1);

				break;
			}
		//195
		case CONSTRAINT_ACTIVITY_BEGINS_STUDENTS_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activity begins students day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivityBeginsStudentsDay");
				}
				else{
					dialogTitle=tr("Modify activity begins students day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivityBeginsStudentsDay");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activityLabel=new QLabel(tr("Activity"));
				activitiesComboBox=new QComboBox;

				break;
			}
		//196
		case CONSTRAINT_ACTIVITIES_BEGIN_STUDENTS_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities begin students day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesBeginStudentsDay");

					firstAddInstructionsLabel=new QLabel(tr("Instructions: you can choose a set of activities with a certain teacher (or "
															"blank for all teachers), students set, subject and activity tag."));
				}
				else{
					dialogTitle=tr("Modify activities begin students day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesBeginStudentsDay");

					firstModifyInstructionsLabel=new QLabel(tr("Instructions: you can choose a set of activities with a certain teacher (or "
															   "blank for all teachers), students set, subject and activity tag."));
				}

				addEmpty=true;

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//197
		case CONSTRAINT_ACTIVITY_BEGINS_TEACHERS_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activity begins teachers day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivityBeginsTeachersDay");
				}
				else{
					dialogTitle=tr("Modify activity begins teachers day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivityBeginsTeachersDay");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activityLabel=new QLabel(tr("Activity"));
				activitiesComboBox=new QComboBox;

				break;
			}
		//198
		case CONSTRAINT_ACTIVITIES_BEGIN_TEACHERS_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities begin teachers day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesBeginTeachersDay");

					firstAddInstructionsLabel=new QLabel(tr("Instructions: you can choose a set of activities with a certain teacher (or "
															"blank for all teachers), students set, subject and activity tag."));
				}
				else{
					dialogTitle=tr("Modify activities begin teachers day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesBeginTeachersDay");

					firstModifyInstructionsLabel=new QLabel(tr("Instructions: you can choose a set of activities with a certain teacher (or "
															   "blank for all teachers), students set, subject and activity tag."));
				}

				addEmpty=true;

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//199
		case CONSTRAINT_TEACHERS_MIN_HOURS_PER_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers min hours per afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinHoursPerAfternoon");
				}
				else{
					dialogTitle=tr("Modify teachers min hours per afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinHoursPerAfternoon");
				}

				labelForSpinBox=new QLabel(tr("Min hours per afternoon"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(2);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				checkBox=new QCheckBox(tr("Allow empty afternoons"));
				checkBox->setChecked(true);

				break;
			}
		//200
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher min hours per afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinHoursPerAfternoon");
				}
				else{
					dialogTitle=tr("Modify teacher min hours per afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinHoursPerAfternoon");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min hours per afternoon"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(2);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				checkBox=new QCheckBox(tr("Allow empty afternoons"));
				checkBox->setChecked(true);

				break;
			}
		//201
		case CONSTRAINT_STUDENTS_MIN_HOURS_PER_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students min hours per afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMinHoursPerAfternoon");
				}
				else{
					dialogTitle=tr("Modify students min hours per afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMinHoursPerAfternoon");
				}

				labelForSpinBox=new QLabel(tr("Min hours"));
				spinBox=new QSpinBox;

				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);
				checkBox=new QCheckBox(tr("Allow empty afternoons"));
				checkBox->setChecked(true);

				break;
			}
		//202
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set min hours per afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMinHoursPerAfternoon");
				}
				else{
					dialogTitle=tr("Modify students set min hours per afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMinHoursPerAfternoon");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min hours"));
				spinBox=new QSpinBox;

				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(2);
				checkBox=new QCheckBox(tr("Allow empty afternoons"));
				checkBox->setChecked(true);

				break;
			}
		//203
		case CONSTRAINT_ACTIVITIES_MAX_HOURLY_SPAN:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities max hourly span", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesMaxHourlySpan");
				}
				else{
					dialogTitle=tr("Modify activities max hourly span", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesMaxHourlySpan");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				labelForSpinBox=new QLabel(tr("Max hourly span"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nHoursPerDay);

				break;
			}
		//204
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max hours daily in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxHoursDailyInInterval");
				}
				else{
					dialogTitle=tr("Modify teachers max hours daily in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxHoursDailyInInterval");
				}

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//205
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max hours daily in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxHoursDailyInInterval");
				}
				else{
					dialogTitle=tr("Modify teacher max hours daily in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxHoursDailyInInterval");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//206
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max hours daily in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxHoursDailyInInterval");
				}
				else{
					dialogTitle=tr("Modify students max hours daily in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxHoursDailyInInterval");
				}

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//207
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max hours daily in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxHoursDailyInInterval");
				}
				else{
					dialogTitle=tr("Modify students set max hours daily in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxHoursDailyInInterval");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max hours daily"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//208
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set min gaps between ordered pair of activity tags per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay");
				}
				else{
					dialogTitle=tr("Modify students set min gaps between ordered pair of activity tags per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				first_activityTagLabel=new QLabel(tr("First activity tag"));
				first_activityTagsComboBox=new QComboBox;
				second_activityTagLabel=new QLabel(tr("Second activity tag"));
				second_activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				swapActivityTagsPushButton=new QPushButton(tr("Swap"));

				break;
			}
		//209
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students min gaps between ordered pair of activity tags per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay");
				}
				else{
					dialogTitle=tr("Modify students min gaps between ordered pair of activity tags per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay");
				}

				first_activityTagLabel=new QLabel(tr("First activity tag"));
				first_activityTagsComboBox=new QComboBox;
				second_activityTagLabel=new QLabel(tr("Second activity tag"));
				second_activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				swapActivityTagsPushButton=new QPushButton(tr("Swap"));

				break;
			}
		//210
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher min gaps between ordered pair of activity tags per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay");
				}
				else{
					dialogTitle=tr("Modify teacher min gaps between ordered pair of activity tags per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				first_activityTagLabel=new QLabel(tr("First activity tag"));
				first_activityTagsComboBox=new QComboBox;
				second_activityTagLabel=new QLabel(tr("Second activity tag"));
				second_activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				swapActivityTagsPushButton=new QPushButton(tr("Swap"));

				break;
			}
		//211
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers min gaps between ordered pair of activity tags per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay");
				}
				else{
					dialogTitle=tr("Modify teachers min gaps between ordered pair of activity tags per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay");
				}

				first_activityTagLabel=new QLabel(tr("First activity tag"));
				first_activityTagsComboBox=new QComboBox;
				second_activityTagLabel=new QLabel(tr("Second activity tag"));
				second_activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				swapActivityTagsPushButton=new QPushButton(tr("Swap"));

				break;
			}
		//212
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set min gaps between activity tag per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMinGapsBetweenActivityTagPerRealDay");
				}
				else{
					dialogTitle=tr("Modify students set min gaps between activity tag per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMinGapsBetweenActivityTagPerRealDay");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//213
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students min gaps between activity tag per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMinGapsBetweenActivityTagPerRealDay");
				}
				else{
					dialogTitle=tr("Modify students min gaps between activity tag per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMinGapsBetweenActivityTagPerRealDay");
				}

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//214
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher min gaps between activity tag per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinGapsBetweenActivityTagPerRealDay");
				}
				else{
					dialogTitle=tr("Modify teacher min gaps between activity tag per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinGapsBetweenActivityTagPerRealDay");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//215
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers min gaps between activity tag per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinGapsBetweenActivityTagPerRealDay");
				}
				else{
					dialogTitle=tr("Modify teachers min gaps between activity tag per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinGapsBetweenActivityTagPerRealDay");
				}

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//216
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set min gaps between ordered pair of activity tags between morning and afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon");
				}
				else{
					dialogTitle=tr("Modify students set min gaps between ordered pair of activity tags between morning and afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				first_activityTagLabel=new QLabel(tr("First activity tag"));
				first_activityTagsComboBox=new QComboBox;
				second_activityTagLabel=new QLabel(tr("Second activity tag"));
				second_activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				swapActivityTagsPushButton=new QPushButton(tr("Swap"));

				break;
			}
		//217
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students min gaps between ordered pair of activity tags between morning and afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon");
				}
				else{
					dialogTitle=tr("Modify students min gaps between ordered pair of activity tags between morning and afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon");
				}

				first_activityTagLabel=new QLabel(tr("First activity tag"));
				first_activityTagsComboBox=new QComboBox;
				second_activityTagLabel=new QLabel(tr("Second activity tag"));
				second_activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				swapActivityTagsPushButton=new QPushButton(tr("Swap"));

				break;
			}
		//218
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher min gaps between ordered pair of activity tags between morning and afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon");
				}
				else{
					dialogTitle=tr("Modify teacher min gaps between ordered pair of activity tags between morning and afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				first_activityTagLabel=new QLabel(tr("First activity tag"));
				first_activityTagsComboBox=new QComboBox;
				second_activityTagLabel=new QLabel(tr("Second activity tag"));
				second_activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				swapActivityTagsPushButton=new QPushButton(tr("Swap"));

				break;
			}
		//219
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers min gaps between ordered pair of activity tags between morning and afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon");
				}
				else{
					dialogTitle=tr("Modify teachers min gaps between ordered pair of activity tags between morning and afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon");
				}

				first_activityTagLabel=new QLabel(tr("First activity tag"));
				first_activityTagsComboBox=new QComboBox;
				second_activityTagLabel=new QLabel(tr("Second activity tag"));
				second_activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				swapActivityTagsPushButton=new QPushButton(tr("Swap"));

				break;
			}
		//220
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set min gaps between activity tag between morning and afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon");
				}
				else{
					dialogTitle=tr("Modify students set min gaps between activity tag between morning and afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//221
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students min gaps between activity tag between morning and afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon");
				}
				else{
					dialogTitle=tr("Modify students min gaps between activity tag between morning and afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon");
				}

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//222
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher min gaps between activity tag between morning and afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon");
				}
				else{
					dialogTitle=tr("Modify teacher min gaps between activity tag between morning and afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//223
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers min gaps between activity tag between morning and afternoon", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon");
				}
				else{
					dialogTitle=tr("Modify teachers min gaps between activity tag between morning and afternoon", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon");
				}

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//224
		case CONSTRAINT_TEACHERS_NO_TWO_CONSECUTIVE_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers no two consecutive days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersNoTwoConsecutiveDays");
				}
				else{
					dialogTitle=tr("Modify teachers no two consecutive days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersNoTwoConsecutiveDays");
				}

				break;
			}
		//225
		case CONSTRAINT_TEACHER_NO_TWO_CONSECUTIVE_DAYS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher no two consecutive days", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherNoTwoConsecutiveDays");
				}
				else{
					dialogTitle=tr("Modify teacher no two consecutive days", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherNoTwoConsecutiveDays");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				break;
			}
		//226
		case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher pair of mutually exclusive time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherPairOfMutuallyExclusiveTimeSlots");
				}
				else{
					dialogTitle=tr("Modify teacher pair of mutually exclusive time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherPairOfMutuallyExclusiveTimeSlots");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				firstTimeSlotGroupBox=new QGroupBox(tr("First time slot"));
				firstDayComboBox=new QComboBox;
				firstHourComboBox=new QComboBox;
				secondTimeSlotGroupBox=new QGroupBox(tr("Second time slot"));
				secondDayComboBox=new QComboBox;
				secondHourComboBox=new QComboBox;

				break;
			}
		//227
		case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers pair of mutually exclusive time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersPairOfMutuallyExclusiveTimeSlots");
				}
				else{
					dialogTitle=tr("Modify teachers pair of mutually exclusive time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersPairOfMutuallyExclusiveTimeSlots");
				}

				firstTimeSlotGroupBox=new QGroupBox(tr("First time slot"));
				firstDayComboBox=new QComboBox;
				firstHourComboBox=new QComboBox;
				secondTimeSlotGroupBox=new QGroupBox(tr("Second time slot"));
				secondDayComboBox=new QComboBox;
				secondHourComboBox=new QComboBox;

				break;
			}
		//228
		case CONSTRAINT_STUDENTS_SET_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set pair of mutually exclusive time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetPairOfMutuallyExclusiveTimeSlots");
				}
				else{
					dialogTitle=tr("Modify students set pair of mutually exclusive time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetPairOfMutuallyExclusiveTimeSlots");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				firstTimeSlotGroupBox=new QGroupBox(tr("First time slot"));
				firstDayComboBox=new QComboBox;
				firstHourComboBox=new QComboBox;
				secondTimeSlotGroupBox=new QGroupBox(tr("Second time slot"));
				secondDayComboBox=new QComboBox;
				secondHourComboBox=new QComboBox;

				break;
			}
		//229
		case CONSTRAINT_STUDENTS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students pair of mutually exclusive time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsPairOfMutuallyExclusiveTimeSlots");
				}
				else{
					dialogTitle=tr("Modify students pair of mutually exclusive time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsPairOfMutuallyExclusiveTimeSlots");
				}

				firstTimeSlotGroupBox=new QGroupBox(tr("First time slot"));
				firstDayComboBox=new QComboBox;
				firstHourComboBox=new QComboBox;
				secondTimeSlotGroupBox=new QGroupBox(tr("Second time slot"));
				secondDayComboBox=new QComboBox;
				secondHourComboBox=new QComboBox;

				break;
			}
		//230
		case CONSTRAINT_TWO_SETS_OF_ACTIVITIES_SAME_SECTIONS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add two sets of activities have the same sections", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTwoSetsOfActivitiesSameSections");

					firstAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=exception, empty (darkgoldenrod)=not exception",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected as an exception, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected as an exception. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}
				else{
					dialogTitle=tr("Modify two sets of activities have the same sections", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTwoSetsOfActivitiesSameSections");

					firstModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=exception, empty (darkgoldenrod)=not exception",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected as an exception, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected as an exception. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				/*activityLabel=new QLabel(tr("Activity"));
				activitiesComboBox=new QComboBox;*/

				swapTwoSetsOfActivitiesPushButton=new QPushButton(tr("Swap"));

				tabWidgetTwoSetsOfActivities=new QTabWidget;
				//
				activitiesLabel_TwoSetsOfActivities_1=new QLabel(tr("Activities"));
				selectedActivitiesLabel_TwoSetsOfActivities_1=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget_TwoSetsOfActivities_1=new QListWidget;
				selectedActivitiesListWidget_TwoSetsOfActivities_1=new QListWidget;
				addAllActivitiesPushButton_TwoSetsOfActivities_1=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton_TwoSetsOfActivities_1=new QPushButton(tr("Clear", "Clear the list of selected activities"));
				//
				activitiesLabel_TwoSetsOfActivities_2=new QLabel(tr("Activities"));
				selectedActivitiesLabel_TwoSetsOfActivities_2=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget_TwoSetsOfActivities_2=new QListWidget;
				selectedActivitiesListWidget_TwoSetsOfActivities_2=new QListWidget;
				addAllActivitiesPushButton_TwoSetsOfActivities_2=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton_TwoSetsOfActivities_2=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), true);

				break;
			}
		//231
		case CONSTRAINT_STUDENTS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students max single gaps in selected time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxSingleGapsInSelectedTimeSlots");

					firstAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}
				else{
					dialogTitle=tr("Modify students max single gaps in selected time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxSingleGapsInSelectedTimeSlots");

					firstModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}

				labelForSpinBox=new QLabel(tr("Max single gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), true);

				break;
			}
		//232
		case CONSTRAINT_STUDENTS_SET_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set max single gaps in selected time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxSingleGapsInSelectedTimeSlots");

					firstAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}
				else{
					dialogTitle=tr("Modify students set max single gaps in selected time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxSingleGapsInSelectedTimeSlots");

					firstModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max single gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), true);

				break;
			}
		//233
		case CONSTRAINT_TEACHERS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max single gaps in selected time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxSingleGapsInSelectedTimeSlots");

					firstAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}
				else{
					dialogTitle=tr("Modify teachers max single gaps in selected time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxSingleGapsInSelectedTimeSlots");

					firstModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}

				labelForSpinBox=new QLabel(tr("Max single gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), true);

				break;
			}
		//234
		case CONSTRAINT_TEACHER_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max single gaps in selected time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxSingleGapsInSelectedTimeSlots");

					firstAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}
				else{
					dialogTitle=tr("Modify teacher max single gaps in selected time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxSingleGapsInSelectedTimeSlots");

					firstModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max single gaps"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), true);

				break;
			}
		//235
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_TERM:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher max hours per term", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxHoursPerTerm");
				}
				else{
					dialogTitle=tr("Modify teacher max hours per term", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxHoursPerTerm");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max hours per term"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerTerm*gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nDaysPerTerm*gt.rules.nHoursPerDay);

				break;
			}
		//236
		case CONSTRAINT_TEACHERS_MAX_HOURS_PER_TERM:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers max hours per term", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxHoursPerTerm");
				}
				else{
					dialogTitle=tr("Modify teachers max hours per term", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxHoursPerTerm");
				}

				labelForSpinBox=new QLabel(tr("Max hours per term"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerTerm*gt.rules.nHoursPerDay);
				spinBox->setValue(gt.rules.nDaysPerTerm*gt.rules.nHoursPerDay);

				break;
			}
		//237
		case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher pair of mutually exclusive sets of time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherPairOfMutuallyExclusiveSetsOfTimeSlots");

					firstAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}
				else{
					dialogTitle=tr("Modify teacher pair of mutually exclusive sets of time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherPairOfMutuallyExclusiveSetsOfTimeSlots");

					firstModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				colorsCheckBox1=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors-1")))
					colorsCheckBox1->setChecked(settings.value(dialogName+QString("/use-colors-1")).toBool());
				else
					colorsCheckBox1->setChecked(false);

				toggleAllPushButton1=new QPushButton(tr("Toggle all", "It refers to time slots"));
				
				timesTable1=new CornerEnabledTableWidget(colorsCheckBox1->isChecked(), true);

				colorsCheckBox2=new QCheckBox(tr("Colors"));
				//QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors-2")))
					colorsCheckBox2->setChecked(settings.value(dialogName+QString("/use-colors-2")).toBool());
				else
					colorsCheckBox2->setChecked(false);

				toggleAllPushButton2=new QPushButton(tr("Toggle all", "It refers to time slots"));
				
				timesTable2=new CornerEnabledTableWidget(colorsCheckBox2->isChecked(), true);

				tabWidgetPairOfMutuallyExclusiveSets=new QTabWidget;

				break;
			}
		//238
		case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers pair of mutually exclusive sets of time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersPairOfMutuallyExclusiveSetsOfTimeSlots");

					firstAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}
				else{
					dialogTitle=tr("Modify teachers pair of mutually exclusive sets of time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersPairOfMutuallyExclusiveSetsOfTimeSlots");

					firstModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}

				colorsCheckBox1=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors-1")))
					colorsCheckBox1->setChecked(settings.value(dialogName+QString("/use-colors-1")).toBool());
				else
					colorsCheckBox1->setChecked(false);

				toggleAllPushButton1=new QPushButton(tr("Toggle all", "It refers to time slots"));
				
				timesTable1=new CornerEnabledTableWidget(colorsCheckBox1->isChecked(), true);

				colorsCheckBox2=new QCheckBox(tr("Colors"));
				//QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors-2")))
					colorsCheckBox2->setChecked(settings.value(dialogName+QString("/use-colors-2")).toBool());
				else
					colorsCheckBox2->setChecked(false);

				toggleAllPushButton2=new QPushButton(tr("Toggle all", "It refers to time slots"));
				
				timesTable2=new CornerEnabledTableWidget(colorsCheckBox2->isChecked(), true);

				tabWidgetPairOfMutuallyExclusiveSets=new QTabWidget;

				break;
			}
		//239
		case CONSTRAINT_STUDENTS_SET_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set pair of mutually exclusive sets of time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots");

					firstAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}
				else{
					dialogTitle=tr("Modify students set pair of mutually exclusive sets of time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots");

					firstModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				colorsCheckBox1=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors-1")))
					colorsCheckBox1->setChecked(settings.value(dialogName+QString("/use-colors-1")).toBool());
				else
					colorsCheckBox1->setChecked(false);

				toggleAllPushButton1=new QPushButton(tr("Toggle all", "It refers to time slots"));
				
				timesTable1=new CornerEnabledTableWidget(colorsCheckBox1->isChecked(), true);

				colorsCheckBox2=new QCheckBox(tr("Colors"));
				//QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors-2")))
					colorsCheckBox2->setChecked(settings.value(dialogName+QString("/use-colors-2")).toBool());
				else
					colorsCheckBox2->setChecked(false);

				toggleAllPushButton2=new QPushButton(tr("Toggle all", "It refers to time slots"));
				
				timesTable2=new CornerEnabledTableWidget(colorsCheckBox2->isChecked(), true);

				tabWidgetPairOfMutuallyExclusiveSets=new QTabWidget;

				break;
			}
		//240
		case CONSTRAINT_STUDENTS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students pair of mutually exclusive sets of time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsPairOfMutuallyExclusiveSetsOfTimeSlots");

					firstAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}
				else{
					dialogTitle=tr("Modify students pair of mutually exclusive sets of time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsPairOfMutuallyExclusiveSetsOfTimeSlots");

					firstModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}

				colorsCheckBox1=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors-1")))
					colorsCheckBox1->setChecked(settings.value(dialogName+QString("/use-colors-1")).toBool());
				else
					colorsCheckBox1->setChecked(false);

				toggleAllPushButton1=new QPushButton(tr("Toggle all", "It refers to time slots"));
				
				timesTable1=new CornerEnabledTableWidget(colorsCheckBox1->isChecked(), true);

				colorsCheckBox2=new QCheckBox(tr("Colors"));
				//QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors-2")))
					colorsCheckBox2->setChecked(settings.value(dialogName+QString("/use-colors-2")).toBool());
				else
					colorsCheckBox2->setChecked(false);

				toggleAllPushButton2=new QPushButton(tr("Toggle all", "It refers to time slots"));
				
				timesTable2=new CornerEnabledTableWidget(colorsCheckBox2->isChecked(), true);

				tabWidgetPairOfMutuallyExclusiveSets=new QTabWidget;

				break;
			}
		//241
		case CONSTRAINT_ACTIVITIES_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities pair of mutually exclusive sets of time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots");

					firstAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
					//trick to use each label on its tab
					secondAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}
				else{
					dialogTitle=tr("Modify activities pair of mutually exclusive sets of time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots");

					firstModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
					//trick to use each label on its tab
					secondModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}

				colorsCheckBox1=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors-1")))
					colorsCheckBox1->setChecked(settings.value(dialogName+QString("/use-colors-1")).toBool());
				else
					colorsCheckBox1->setChecked(false);

				toggleAllPushButton1=new QPushButton(tr("Toggle all", "It refers to time slots"));
				
				timesTable1=new CornerEnabledTableWidget(colorsCheckBox1->isChecked(), true);

				colorsCheckBox2=new QCheckBox(tr("Colors"));
				//QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors-2")))
					colorsCheckBox2->setChecked(settings.value(dialogName+QString("/use-colors-2")).toBool());
				else
					colorsCheckBox2->setChecked(false);

				toggleAllPushButton2=new QPushButton(tr("Toggle all", "It refers to time slots"));
				
				timesTable2=new CornerEnabledTableWidget(colorsCheckBox2->isChecked(), true);

				tabWidgetPairOfMutuallyExclusiveSets=new QTabWidget;
				
				ctrActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots=true;
				
				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				break;
			}
		//242
		case CONSTRAINT_ACTIVITIES_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities pair of mutually exclusive time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesPairOfMutuallyExclusiveTimeSlots");
				}
				else{
					dialogTitle=tr("Modify activities pair of mutually exclusive time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesPairOfMutuallyExclusiveTimeSlots");
				}

				firstTimeSlotGroupBox=new QGroupBox(tr("First time slot"));
				firstDayComboBox=new QComboBox;
				firstHourComboBox=new QComboBox;
				secondTimeSlotGroupBox=new QGroupBox(tr("Second time slot"));
				secondDayComboBox=new QComboBox;
				secondHourComboBox=new QComboBox;

				tabWidgetPairOfMutuallyExclusiveSets=new QTabWidget;

				ctrActivitiesPairOfMutuallyExclusiveTimeSlots=true;

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				break;
			}
		//243
		case CONSTRAINT_TEACHER_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teacher occupies max sets of time slots from selection", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherOccupiesMaxSetsOfTimeSlotsFromSelection");

					firstAddInstructionsLabel=new QLabel(tr("The number chosen in a cell of the table widget represents the number of the selected set of"
					 " time slots to which this slot belongs to (the value 0, shown here as a space, represents an unselected slot)."));
				}
				else{
					dialogTitle=tr("Modify teacher occupies max sets of time slots from selection", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("ModifyConstraintTeacherOccupiesMaxSetsOfTimeSlotsFromSelection");

					firstModifyInstructionsLabel=new QLabel(tr("The number chosen in a cell of the table widget represents the number of the selected set of"
					 " time slots to which this slot belongs to (the value 0, shown here as a space, represents an unselected slot)."));
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				occupyMaxSetsOfTimeSlotsFromSelectionTableWidget=new CornerEnabledTableWidgetOfSpinBoxes;

				labelForSpinBox=new QLabel(tr("Max occupied sets of time slots"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				break;
			}
		//244
		case CONSTRAINT_TEACHERS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add teachers occupy max sets of time slots from selection", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersOccupyMaxSetsOfTimeSlotsFromSelection");

					firstAddInstructionsLabel=new QLabel(tr("The number chosen in a cell of the table widget represents the number of the selected set of"
					 " time slots to which this slot belongs to (the value 0, shown here as a space, represents an unselected slot)."));
				}
				else{
					dialogTitle=tr("Modify teachers occupy max sets of time slots from selection", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("ModifyConstraintTeachersOccupyMaxSetsOfTimeSlotsFromSelection");

					firstModifyInstructionsLabel=new QLabel(tr("The number chosen in a cell of the table widget represents the number of the selected set of"
					 " time slots to which this slot belongs to (the value 0, shown here as a space, represents an unselected slot)."));
				}

				occupyMaxSetsOfTimeSlotsFromSelectionTableWidget=new CornerEnabledTableWidgetOfSpinBoxes;

				labelForSpinBox=new QLabel(tr("Max occupied sets of time slots"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				break;
			}
		//245
		case CONSTRAINT_STUDENTS_SET_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students set occupies max sets of time slots from selection", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection");

					firstAddInstructionsLabel=new QLabel(tr("The number chosen in a cell of the table widget represents the number of the selected set of"
					 " time slots to which this slot belongs to (the value 0, shown here as a space, represents an unselected slot)."));
				}
				else{
					dialogTitle=tr("Modify students set occupies max sets of time slots from selection", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection");

					firstModifyInstructionsLabel=new QLabel(tr("The number chosen in a cell of the table widget represents the number of the selected set of"
					 " time slots to which this slot belongs to (the value 0, shown here as a space, represents an unselected slot)."));
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				occupyMaxSetsOfTimeSlotsFromSelectionTableWidget=new CornerEnabledTableWidgetOfSpinBoxes;

				labelForSpinBox=new QLabel(tr("Max occupied sets of time slots"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				break;
			}
		//246
		case CONSTRAINT_STUDENTS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add students occupy max sets of time slots from selection", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsOccupyMaxSetsOfTimeSlotsFromSelection");

					firstAddInstructionsLabel=new QLabel(tr("The number chosen in a cell of the table widget represents the number of the selected set of"
					 " time slots to which this slot belongs to (the value 0, shown here as a space, represents an unselected slot)."));
				}
				else{
					dialogTitle=tr("Modify students occupy max sets of time slots from selection", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("ModifyConstraintStudentsOccupyMaxSetsOfTimeSlotsFromSelection");

					firstModifyInstructionsLabel=new QLabel(tr("The number chosen in a cell of the table widget represents the number of the selected set of"
					 " time slots to which this slot belongs to (the value 0, shown here as a space, represents an unselected slot)."));
				}

				occupyMaxSetsOfTimeSlotsFromSelectionTableWidget=new CornerEnabledTableWidgetOfSpinBoxes;

				labelForSpinBox=new QLabel(tr("Max occupied sets of time slots"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				break;
			}
		//247
		case CONSTRAINT_ACTIVITIES_OVERLAP_COMPLETELY_OR_DO_NOT_OVERLAP:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities overlap completely or do not overlap", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesNotOverlapping");

					firstAddInstructionsLabel=new QLabel(tr("This constraint means that each pair of activities from the selected set should either have"
															" the same starting time (day+hour), or have no common time slot(s). All the selected activities"
															" should have the same duration, greater than 1."));
				}
				else{
					dialogTitle=tr("Modify activities overlap completely or do not overlap", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesNotOverlapping");

					firstModifyInstructionsLabel=new QLabel(tr("This constraint means that each pair of activities from the selected set should either have"
															" the same starting time (day+hour), or have no common time slot(s). All the selected activities"
															" should have the same duration, greater than 1."));
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				break;
			}
		//248
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities occupy max sets of time slots from selection", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesOccupyMaxSetsOfTimeSlotsFromSelection");

					firstAddInstructionsLabel=new QLabel(tr("The number chosen in a cell of the table widget represents the number of the selected set of"
					 " time slots to which this slot belongs to (the value 0, shown here as a space, represents an unselected slot)."));
				}
				else{
					dialogTitle=tr("Modify activities occupy max sets of time slots from selection", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesOccupyMaxSetsOfTimeSlotsFromSelection");

					firstModifyInstructionsLabel=new QLabel(tr("The number chosen in a cell of the table widget represents the number of the selected set of"
					 " time slots to which this slot belongs to (the value 0, shown here as a space, represents an unselected slot)."));
				}

				occupyMaxSetsOfTimeSlotsFromSelectionTableWidget=new CornerEnabledTableWidgetOfSpinBoxes;

				labelForSpinBox=new QLabel(tr("Max occupied sets of time slots"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(2);

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				//activities
				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				tabWidgetAOMSOTSFS=new QTabWidget;

				break;
			}
		//249
		case CONSTRAINT_ACTIVITY_BEGINS_OR_ENDS_STUDENTS_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activity begins or ends students day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivityBeginsOrEndsStudentsDay");
				}
				else{
					dialogTitle=tr("Modify activity begins or ends students day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivityBeginsOrEndsStudentsDay");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activityLabel=new QLabel(tr("Activity"));
				activitiesComboBox=new QComboBox;

				break;
			}
		//250
		case CONSTRAINT_ACTIVITIES_BEGIN_OR_END_STUDENTS_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities begin or end students day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesBeginOrEndStudentsDay");

					firstAddInstructionsLabel=new QLabel(tr("Instructions: you can choose a set of activities with a certain teacher (or "
															"blank for all teachers), students set, subject and activity tag."));
				}
				else{
					dialogTitle=tr("Modify activities begin or end students day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesBeginOrEndStudentsDay");

					firstModifyInstructionsLabel=new QLabel(tr("Instructions: you can choose a set of activities with a certain teacher (or "
															   "blank for all teachers), students set, subject and activity tag."));
				}

				addEmpty=true;

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//251
		case CONSTRAINT_ACTIVITY_BEGINS_OR_ENDS_TEACHERS_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activity begins or ends teachers day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivityBeginsOrEndsTeachersDay");
				}
				else{
					dialogTitle=tr("Modify activity begins or ends teachers day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivityBeginsOrEndsTeachersDay");
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activityLabel=new QLabel(tr("Activity"));
				activitiesComboBox=new QComboBox;

				break;
			}
		//252
		case CONSTRAINT_ACTIVITIES_BEGIN_OR_END_TEACHERS_DAY:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities begin or end teachers day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesBeginOrEndTeachersDay");

					firstAddInstructionsLabel=new QLabel(tr("Instructions: you can choose a set of activities with a certain teacher (or "
															"blank for all teachers), students set, subject and activity tag."));
				}
				else{
					dialogTitle=tr("Modify activities begin or end teachers day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesBeginOrEndTeachersDay");

					firstModifyInstructionsLabel=new QLabel(tr("Instructions: you can choose a set of activities with a certain teacher (or "
															   "blank for all teachers), students set, subject and activity tag."));
				}

				addEmpty=true;

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				break;
			}
		//253
		case CONSTRAINT_ACTIVITIES_MAX_TOTAL_NUMBER_OF_STUDENTS_IN_SELECTED_TIME_SLOTS:
			{
				if(oldtc==nullptr){
					dialogTitle=tr("Add activities max total number of students in selected time slots", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots");

					firstAddInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}
				else{
					dialogTitle=tr("Modify activities max total number of students in selected time slots", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots");

					firstModifyInstructionsLabel=new QLabel(tr("✓ (darkcyan)=selected, empty (darkgoldenrod)=not selected",
					 "This is an explanation in a dialog for a constraint. It says that symbol ✓ (or darkcyan) means that this slot is selected, "
					 "and an empty cell (or darkgoldenrod) means that the slot is not selected. darkcyan and darkgoldenrod are two colors, "
					 "and they can be translated; you can see them here: https://doc.qt.io/qt-6/qcolorconstants.html."));
				}

				addEmpty=true;
				filterGroupBox=new QGroupBox(tr("Filter"));

				//teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				//studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				//subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				//activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				activitiesLabel=new QLabel(tr("Activities"));
				selectedActivitiesLabel=new QLabel(tr("Selected", "It refers to activities"));
				activitiesListWidget=new QListWidget;
				selectedActivitiesListWidget=new QListWidget;
				addAllActivitiesPushButton=new QPushButton(tr("All", "Add all filtered activities to the list of selected activities"));
				clearActivitiesPushButton=new QPushButton(tr("Clear", "Clear the list of selected activities"));

				colorsCheckBox=new QCheckBox(tr("Colors"));
				QSettings settings(COMPANY, PROGRAM);
				if(settings.contains(dialogName+QString("/use-colors")))
					colorsCheckBox->setChecked(settings.value(dialogName+QString("/use-colors")).toBool());
				else
					colorsCheckBox->setChecked(false);

				toggleAllPushButton=new QPushButton(tr("Toggle all", "It refers to time slots"));

				timesTable=new CornerEnabledTableWidget(colorsCheckBox->isChecked(), true);

				labelForSpinBox=new QLabel(tr("Max total number of students"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(MAX_TOTAL_SUBGROUPS);
				spinBox->setValue(0);

				tabWidget=new QTabWidget;

				break;
			}

		default:
			assert(0);
			break;
	}

	if(splitIndexSpinBox!=nullptr){
		splitIndexSpinBox->setMinimum(1);
		splitIndexSpinBox->setMaximum(MAX_SPLIT_OF_AN_ACTIVITY);
		splitIndexSpinBox->setValue(1);
	}

	if(firstAddInstructionsLabel!=nullptr){
		firstAddInstructionsLabel->setWordWrap(true);
		//firstAddInstructionsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	}
	if(secondAddInstructionsLabel!=nullptr){
		secondAddInstructionsLabel->setWordWrap(true);
		//secondAddInstructionsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	}

	if(firstModifyInstructionsLabel!=nullptr){
		firstModifyInstructionsLabel->setWordWrap(true);
		//firstModifyInstructionsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	}
	if(secondModifyInstructionsLabel!=nullptr){
		secondModifyInstructionsLabel->setWordWrap(true);
		//secondModifyInstructionsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	}

	if(first_filterTeachersComboBox!=nullptr){
		first_filterTeachersComboBox->addItem(QString(""));
		for(Teacher* tch : std::as_const(gt.rules.teachersList))
			first_filterTeachersComboBox->addItem(tch->name);

		first_filterTeachersComboBox->setCurrentIndex(0);
	}
	if(second_filterTeachersComboBox!=nullptr){
		second_filterTeachersComboBox->addItem(QString(""));
		for(Teacher* tch : std::as_const(gt.rules.teachersList))
			second_filterTeachersComboBox->addItem(tch->name);

		second_filterTeachersComboBox->setCurrentIndex(0);
	}
	if(third_filterTeachersComboBox!=nullptr){
		third_filterTeachersComboBox->addItem(QString(""));
		for(Teacher* tch : std::as_const(gt.rules.teachersList))
			third_filterTeachersComboBox->addItem(tch->name);

		third_filterTeachersComboBox->setCurrentIndex(0);
	}

	if(first_filterStudentsComboBox!=nullptr){
		populateStudentsComboBox(first_filterStudentsComboBox, QString(""), true);

		first_filterStudentsComboBox->setCurrentIndex(0);
	}
	if(second_filterStudentsComboBox!=nullptr){
		populateStudentsComboBox(second_filterStudentsComboBox, QString(""), true);

		second_filterStudentsComboBox->setCurrentIndex(0);
	}
	if(third_filterStudentsComboBox!=nullptr){
		populateStudentsComboBox(third_filterStudentsComboBox, QString(""), true);

		third_filterStudentsComboBox->setCurrentIndex(0);
	}

	if(first_filterSubjectsComboBox!=nullptr){
		first_filterSubjectsComboBox->addItem(QString(""));
		for(Subject *sbj : std::as_const(gt.rules.subjectsList))
			first_filterSubjectsComboBox->addItem(sbj->name);

		first_filterSubjectsComboBox->setCurrentIndex(0);
	}
	if(second_filterSubjectsComboBox!=nullptr){
		second_filterSubjectsComboBox->addItem(QString(""));
		for(Subject *sbj : std::as_const(gt.rules.subjectsList))
			second_filterSubjectsComboBox->addItem(sbj->name);

		second_filterSubjectsComboBox->setCurrentIndex(0);
	}
	if(third_filterSubjectsComboBox!=nullptr){
		third_filterSubjectsComboBox->addItem(QString(""));
		for(Subject *sbj : std::as_const(gt.rules.subjectsList))
			third_filterSubjectsComboBox->addItem(sbj->name);

		third_filterSubjectsComboBox->setCurrentIndex(0);
	}

	if(first_filterActivityTagsComboBox!=nullptr){
		first_filterActivityTagsComboBox->addItem(QString(""));
		for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
			first_filterActivityTagsComboBox->addItem(at->name);

		first_filterActivityTagsComboBox->setCurrentIndex(0);
	}
	if(second_filterActivityTagsComboBox!=nullptr){
		second_filterActivityTagsComboBox->addItem(QString(""));
		for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
			second_filterActivityTagsComboBox->addItem(at->name);

		second_filterActivityTagsComboBox->setCurrentIndex(0);
	}
	if(third_filterActivityTagsComboBox!=nullptr){
		third_filterActivityTagsComboBox->addItem(QString(""));
		for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
			third_filterActivityTagsComboBox->addItem(at->name);

		third_filterActivityTagsComboBox->setCurrentIndex(0);
	}

	QVBoxLayout* teacherLayout=nullptr;
	if(teachersComboBox!=nullptr){
		teacherLayout=new QVBoxLayout;
		if(teacherLabel!=nullptr)
			teacherLayout->addWidget(teacherLabel);
		teacherLayout->addWidget(teachersComboBox);

		if(addEmpty)
			teachersComboBox->addItem(QString(""));
		for(Teacher* tch : std::as_const(gt.rules.teachersList))
			teachersComboBox->addItem(tch->name);

		teachersComboBox->setCurrentIndex(0);
	}

	QVBoxLayout* studentsLayout=nullptr;
	if(studentsComboBox!=nullptr){
		studentsLayout=new QVBoxLayout;
		if(studentsLabel!=nullptr)
			studentsLayout->addWidget(studentsLabel);

		if(filterGroupBox==nullptr){
			studentsLayout->addWidget(studentsComboBox);
		}
		else{
			showRelatedCheckBox=new QCheckBox(tr("Show related"));

			QSettings settings(COMPANY, PROGRAM);
			showRelatedCheckBox->setChecked(settings.value(dialogName+QString("/show-related"), "false").toBool());

			connect(showRelatedCheckBox, &QCheckBox::toggled, this, &AddOrModifyTimeConstraint::showRelatedCheckBoxToggled);

			QHBoxLayout* ssrl=new QHBoxLayout;

			ssrl->addWidget(studentsComboBox);
			ssrl->addWidget(showRelatedCheckBox);

			studentsLayout->addLayout(ssrl);
		}

		if(addEmpty)
			populateStudentsComboBox(studentsComboBox, QString(""), true);
		else
			populateStudentsComboBox(studentsComboBox);

		studentsComboBox->setCurrentIndex(0);
	}

	QVBoxLayout* subjectLayout=nullptr;
	if(subjectsComboBox!=nullptr){
		subjectLayout=new QVBoxLayout;
		if(subjectLabel!=nullptr)
			subjectLayout->addWidget(subjectLabel);
		subjectLayout->addWidget(subjectsComboBox);

		if(addEmpty)
			subjectsComboBox->addItem(QString(""));
		for(Subject* sbj : std::as_const(gt.rules.subjectsList))
			subjectsComboBox->addItem(sbj->name);

		subjectsComboBox->setCurrentIndex(0);
	}

	QVBoxLayout* activityTagLayout=nullptr;
	if(activityTagsComboBox!=nullptr){
		activityTagLayout=new QVBoxLayout;
		if(activityTagLabel!=nullptr)
			activityTagLayout->addWidget(activityTagLabel);
		activityTagLayout->addWidget(activityTagsComboBox);

		if(addEmpty)
			activityTagsComboBox->addItem(QString(""));
		for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
			activityTagsComboBox->addItem(at->name);

		activityTagsComboBox->setCurrentIndex(0);
	}

	QVBoxLayout* first_activityTagLayout=nullptr;
	if(first_activityTagsComboBox!=nullptr){
		first_activityTagLayout=new QVBoxLayout;
		if(first_activityTagLabel!=nullptr)
			first_activityTagLayout->addWidget(first_activityTagLabel);
		first_activityTagLayout->addWidget(first_activityTagsComboBox);

		//if(addEmpty)
			//first_activityTagsComboBox->addItem(QString(""));
		for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
			first_activityTagsComboBox->addItem(at->name);

		first_activityTagsComboBox->setCurrentIndex(0);
	}

	QVBoxLayout* second_activityTagLayout=nullptr;
	if(second_activityTagsComboBox!=nullptr){
		second_activityTagLayout=new QVBoxLayout;
		if(second_activityTagLabel!=nullptr)
			second_activityTagLayout->addWidget(second_activityTagLabel);
		second_activityTagLayout->addWidget(second_activityTagsComboBox);

		//if(addEmpty)
			//second_activityTagsComboBox->addItem(QString(""));
		for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
			second_activityTagsComboBox->addItem(at->name);

		second_activityTagsComboBox->setCurrentIndex(0);
	}

	if(activityTagsListWidget!=nullptr){
		for(ActivityTag* at : std::as_const(gt.rules.activityTagsList))
			activityTagsListWidget->addItem(at->name);

		activityTagsListWidget->setCurrentRow(0);
	}

	if(oldtc==nullptr && !addEmpty){
		if(teachersComboBox!=nullptr){
			if(!_preselectedTeacherName.isEmpty()){
				int q=teachersComboBox->findText(_preselectedTeacherName);
				if(q>=0){
					teachersComboBox->setCurrentIndex(q);
				}
			}
		}

		//Please remember that there is an option, show subgroups in students combo boxes.
		if(studentsComboBox!=nullptr){
			if(!_preselectedStudentsSetName.isEmpty()){
				int q=studentsComboBox->findText(_preselectedStudentsSetName);
				if(q>=0){
					studentsComboBox->setCurrentIndex(q);
				}
			}
		}

		if(activityTagsComboBox!=nullptr){
			if(!_preselectedActivityTagName.isEmpty()){
				int q=activityTagsComboBox->findText(_preselectedActivityTagName);
				if(q>=0){
					activityTagsComboBox->setCurrentIndex(q);
				}
			}
		}
	}
	if(oldtc==nullptr){
		if(first_activityTagsComboBox!=nullptr){
			if(!_preselectedFirstActivityTagName.isEmpty()){
				int q=first_activityTagsComboBox->findText(_preselectedFirstActivityTagName);
				if(q>=0){
					first_activityTagsComboBox->setCurrentIndex(q);
				}
			}
		}

		if(second_activityTagsComboBox!=nullptr){
			if(!_preselectedSecondActivityTagName.isEmpty()){
				int q=second_activityTagsComboBox->findText(_preselectedSecondActivityTagName);
				if(q>=0){
					second_activityTagsComboBox->setCurrentIndex(q);
				}
			}
		}
	}

	if(first_filterTeachersComboBox!=nullptr)
		connect(first_filterTeachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::first_activitiesComboBoxFilter);
	if(second_filterTeachersComboBox!=nullptr)
		connect(second_filterTeachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::second_activitiesComboBoxFilter);
	if(third_filterTeachersComboBox!=nullptr)
		connect(third_filterTeachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::third_activitiesComboBoxFilter);

	if(first_filterStudentsComboBox!=nullptr)
		connect(first_filterStudentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::firstFilter_showRelatedCheckBoxToggled);
	if(second_filterStudentsComboBox!=nullptr)
		connect(second_filterStudentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::secondFilter_showRelatedCheckBoxToggled);
	if(third_filterStudentsComboBox!=nullptr)
		connect(third_filterStudentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::thirdFilter_showRelatedCheckBoxToggled);

	if(first_filterSubjectsComboBox!=nullptr)
		connect(first_filterSubjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::first_activitiesComboBoxFilter);
	if(second_filterSubjectsComboBox!=nullptr)
		connect(second_filterSubjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::second_activitiesComboBoxFilter);
	if(third_filterSubjectsComboBox!=nullptr)
		connect(third_filterSubjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::third_activitiesComboBoxFilter);

	if(first_filterActivityTagsComboBox!=nullptr)
		connect(first_filterActivityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::first_activitiesComboBoxFilter);
	if(second_filterActivityTagsComboBox!=nullptr)
		connect(second_filterActivityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::second_activitiesComboBoxFilter);
	if(third_filterActivityTagsComboBox!=nullptr)
		connect(third_filterActivityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::third_activitiesComboBoxFilter);

	assert(activitiesComboBox==nullptr || activitiesListWidget==nullptr);
	if(teachersComboBox!=nullptr){
		if(activitiesComboBox!=nullptr)
			connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::filterActivitiesComboBox);
		if(activitiesListWidget!=nullptr)
			connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::filterActivitiesListWidget);
		if(activitiesListWidget_TwoSetsOfActivities_1!=nullptr){
			assert(activitiesListWidget_TwoSetsOfActivities_2!=nullptr);
			connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::filterActivitiesListWidgets1And2);
		}
	}
	if(studentsComboBox!=nullptr){
		if(activitiesComboBox!=nullptr)
			connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::showRelatedCheckBoxToggled);
		if(activitiesListWidget!=nullptr)
			connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::showRelatedCheckBoxToggled);
		if(activitiesListWidget_TwoSetsOfActivities_1!=nullptr){
			assert(activitiesListWidget_TwoSetsOfActivities_2!=nullptr);
			connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::showRelatedCheckBoxToggled);
		}
	}
	if(subjectsComboBox!=nullptr){
		if(activitiesComboBox!=nullptr)
			connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::filterActivitiesComboBox);
		if(activitiesListWidget!=nullptr)
			connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::filterActivitiesListWidget);
		if(activitiesListWidget_TwoSetsOfActivities_1!=nullptr){
			assert(activitiesListWidget_TwoSetsOfActivities_2!=nullptr);
			connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::filterActivitiesListWidgets1And2);
		}
	}
	if(activityTagsComboBox!=nullptr){
		if(activitiesComboBox!=nullptr)
			connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::filterActivitiesComboBox);
		if(activitiesListWidget!=nullptr)
			connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::filterActivitiesListWidget);
		if(activitiesListWidget_TwoSetsOfActivities_1!=nullptr){
			assert(activitiesListWidget_TwoSetsOfActivities_2!=nullptr);
			connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::filterActivitiesListWidgets1And2);
		}
	}

	if(periodGroupBox!=nullptr && dayLabel!=nullptr && daysComboBox!=nullptr && hourLabel!=nullptr && hoursComboBox!=nullptr){
		for(int i=0; i<gt.rules.nDaysPerWeek; i++)
			daysComboBox->addItem(gt.rules.daysOfTheWeek[i]);
		daysComboBox->setCurrentIndex(0);
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			hoursComboBox->addItem(gt.rules.hoursOfTheDay[i]);
		hoursComboBox->setCurrentIndex(0);

		QGridLayout* layout=new QGridLayout;
		layout->addWidget(dayLabel, 0, 0);
		layout->addWidget(daysComboBox, 1, 0);
		layout->addWidget(hourLabel, 0, 1);
		layout->addWidget(hoursComboBox, 1, 1);
		periodGroupBox->setLayout(layout);
	}

	if(permanentlyLockedCheckBox!=nullptr && permanentlyLockedLabel!=nullptr){
		permanentlyLockedCheckBox->setChecked(true);
		permanentlyLockedLabel->setWordWrap(true);
		//permanentlyLockedLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	}

	if(occupyMaxSetsOfTimeSlotsFromSelectionTableWidget!=nullptr){
		occupyMaxOldItemDelegate=occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->itemDelegate();
		occupyMaxNewItemDelegate=new AddOrModifyTimeConstraintTimesTableDelegate(nullptr, gt.rules.nHoursPerDay, occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->columnCount());
		occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->setItemDelegate(occupyMaxNewItemDelegate);
		
		assert(colorsCheckBox!=nullptr);
		connect(colorsCheckBox, &QCheckBox::toggled, this, &AddOrModifyTimeConstraint::colorAllSpinBoxes);
	}

	if(timesTable!=nullptr){
		connect(timesTable, &CornerEnabledTableWidget::itemClicked, this, &AddOrModifyTimeConstraint::itemClicked);
		
		assert(colorsCheckBox!=nullptr);
		connect(colorsCheckBox, &QCheckBox::toggled, this, &AddOrModifyTimeConstraint::colorsCheckBoxToggled);

		assert(toggleAllPushButton!=nullptr);
		connect(toggleAllPushButton, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::toggleAllClicked);
		
		initTimesTable(timesTable);

		oldItemDelegate=timesTable->itemDelegate();
		newItemDelegate=new AddOrModifyTimeConstraintTimesTableDelegate(nullptr, gt.rules.nHoursPerDay, timesTable->columnCount());
		timesTable->setItemDelegate(newItemDelegate);

		timesTable->resizeRowsToContents();
		//timesTable->resizeColumnsToContents();

		connect(timesTable->horizontalHeader(), &QHeaderView::sectionClicked, this, &AddOrModifyTimeConstraint::horizontalHeaderClicked);
		connect(timesTable->verticalHeader(), &QHeaderView::sectionClicked, this, &AddOrModifyTimeConstraint::verticalHeaderClicked);

		timesTable->setSelectionMode(QAbstractItemView::NoSelection);

		setStretchAvailabilityTableNicely(timesTable);

		connect(timesTable, &CornerEnabledTableWidget::cellEntered, this, &AddOrModifyTimeConstraint::cellEntered);
		timesTable->setMouseTracking(true);
	}

	if(timesTable1!=nullptr){
		connect(timesTable1, &CornerEnabledTableWidget::itemClicked, this, &AddOrModifyTimeConstraint::itemClicked1);
		
		assert(colorsCheckBox1!=nullptr);
		connect(colorsCheckBox1, &QCheckBox::toggled, this, &AddOrModifyTimeConstraint::colorsCheckBoxToggled1);

		assert(toggleAllPushButton1!=nullptr);
		connect(toggleAllPushButton1, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::toggleAllClicked1);
		
		initTimesTable(timesTable1);

		oldItemDelegate1=timesTable1->itemDelegate();
		newItemDelegate1=new AddOrModifyTimeConstraintTimesTableDelegate(nullptr, gt.rules.nHoursPerDay, timesTable1->columnCount());
		timesTable1->setItemDelegate(newItemDelegate1);

		timesTable1->resizeRowsToContents();
		//timesTable->resizeColumnsToContents();

		connect(timesTable1->horizontalHeader(), &QHeaderView::sectionClicked, this, &AddOrModifyTimeConstraint::horizontalHeaderClicked1);
		connect(timesTable1->verticalHeader(), &QHeaderView::sectionClicked, this, &AddOrModifyTimeConstraint::verticalHeaderClicked1);

		timesTable1->setSelectionMode(QAbstractItemView::NoSelection);

		setStretchAvailabilityTableNicely(timesTable1);

		connect(timesTable1, &CornerEnabledTableWidget::cellEntered, this, &AddOrModifyTimeConstraint::cellEntered1);
		timesTable1->setMouseTracking(true);
	}
	if(timesTable2!=nullptr){
		connect(timesTable2, &CornerEnabledTableWidget::itemClicked, this, &AddOrModifyTimeConstraint::itemClicked2);
		
		assert(colorsCheckBox2!=nullptr);
		connect(colorsCheckBox2, &QCheckBox::toggled, this, &AddOrModifyTimeConstraint::colorsCheckBoxToggled2);

		assert(toggleAllPushButton2!=nullptr);
		connect(toggleAllPushButton2, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::toggleAllClicked2);
		
		initTimesTable(timesTable2);

		oldItemDelegate2=timesTable2->itemDelegate();
		newItemDelegate2=new AddOrModifyTimeConstraintTimesTableDelegate(nullptr, gt.rules.nHoursPerDay, timesTable2->columnCount());
		timesTable2->setItemDelegate(newItemDelegate2);

		timesTable2->resizeRowsToContents();
		//timesTable->resizeColumnsToContents();

		connect(timesTable2->horizontalHeader(), &QHeaderView::sectionClicked, this, &AddOrModifyTimeConstraint::horizontalHeaderClicked2);
		connect(timesTable2->verticalHeader(), &QHeaderView::sectionClicked, this, &AddOrModifyTimeConstraint::verticalHeaderClicked2);

		timesTable2->setSelectionMode(QAbstractItemView::NoSelection);

		setStretchAvailabilityTableNicely(timesTable2);

		connect(timesTable2, &CornerEnabledTableWidget::cellEntered, this, &AddOrModifyTimeConstraint::cellEntered2);
		timesTable2->setMouseTracking(true);
	}

	if(activitiesListWidget_TwoSetsOfActivities_1!=nullptr){
		assert(activitiesLabel_TwoSetsOfActivities_1!=nullptr);
		assert(selectedActivitiesLabel_TwoSetsOfActivities_1!=nullptr);
		assert(selectedActivitiesListWidget_TwoSetsOfActivities_1!=nullptr);
		assert(addAllActivitiesPushButton_TwoSetsOfActivities_1!=nullptr);
		assert(clearActivitiesPushButton_TwoSetsOfActivities_1!=nullptr);

		connect(clearActivitiesPushButton_TwoSetsOfActivities_1, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::clearActivitiesClicked1);
		connect(activitiesListWidget_TwoSetsOfActivities_1, &QListWidget::itemDoubleClicked, this, &AddOrModifyTimeConstraint::addActivity1);
		connect(addAllActivitiesPushButton_TwoSetsOfActivities_1, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::addAllActivitiesClicked1);
		connect(selectedActivitiesListWidget_TwoSetsOfActivities_1, &QListWidget::itemDoubleClicked, this, &AddOrModifyTimeConstraint::removeActivity1);

		activitiesListWidget_TwoSetsOfActivities_1->setSelectionMode(QAbstractItemView::SingleSelection);
		selectedActivitiesListWidget_TwoSetsOfActivities_1->setSelectionMode(QAbstractItemView::SingleSelection);
	}
	if(activitiesListWidget_TwoSetsOfActivities_2!=nullptr){
		assert(activitiesLabel_TwoSetsOfActivities_2!=nullptr);
		assert(selectedActivitiesLabel_TwoSetsOfActivities_2!=nullptr);
		assert(selectedActivitiesListWidget_TwoSetsOfActivities_2!=nullptr);
		assert(addAllActivitiesPushButton_TwoSetsOfActivities_2!=nullptr);
		assert(clearActivitiesPushButton_TwoSetsOfActivities_2!=nullptr);

		connect(clearActivitiesPushButton_TwoSetsOfActivities_2, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::clearActivitiesClicked2);
		connect(activitiesListWidget_TwoSetsOfActivities_2, &QListWidget::itemDoubleClicked, this, &AddOrModifyTimeConstraint::addActivity2);
		connect(addAllActivitiesPushButton_TwoSetsOfActivities_2, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::addAllActivitiesClicked2);
		connect(selectedActivitiesListWidget_TwoSetsOfActivities_2, &QListWidget::itemDoubleClicked, this, &AddOrModifyTimeConstraint::removeActivity2);

		activitiesListWidget_TwoSetsOfActivities_2->setSelectionMode(QAbstractItemView::SingleSelection);
		selectedActivitiesListWidget_TwoSetsOfActivities_2->setSelectionMode(QAbstractItemView::SingleSelection);
	}

	if(activitiesListWidget!=nullptr){
		assert(activitiesLabel!=nullptr);
		assert(selectedActivitiesLabel!=nullptr);
		assert(selectedActivitiesListWidget!=nullptr);
		assert(addAllActivitiesPushButton!=nullptr);
		assert(clearActivitiesPushButton!=nullptr);

		connect(clearActivitiesPushButton, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::clearActivitiesClicked);
		connect(activitiesListWidget, &QListWidget::itemDoubleClicked, this, &AddOrModifyTimeConstraint::addActivity);
		connect(addAllActivitiesPushButton, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::addAllActivitiesClicked);
		connect(selectedActivitiesListWidget, &QListWidget::itemDoubleClicked, this, &AddOrModifyTimeConstraint::removeActivity);

		activitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
		selectedActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	}

	if(activityTagsListWidget!=nullptr){
		assert(activityTagsLabel!=nullptr);
		assert(selectedActivityTagsLabel!=nullptr);
		assert(selectedActivityTagsListWidget!=nullptr);
		assert(addAllActivityTagsPushButton!=nullptr);
		assert(clearActivityTagsPushButton!=nullptr);

		connect(clearActivityTagsPushButton, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::clearActivityTagsClicked);
		connect(activityTagsListWidget, &QListWidget::itemDoubleClicked, this, &AddOrModifyTimeConstraint::addActivityTag);
		connect(addAllActivityTagsPushButton, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::addAllActivityTagsClicked);
		connect(selectedActivityTagsListWidget, &QListWidget::itemDoubleClicked, this, &AddOrModifyTimeConstraint::removeActivityTag);

		activityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
		selectedActivityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	}

	if(first_activitiesComboBox!=nullptr && first_activityLabel!=nullptr)
		firstFilter_showRelatedCheckBox=new QCheckBox(tr("Show related"));
	if(second_activitiesComboBox!=nullptr && second_activityLabel!=nullptr)
		secondFilter_showRelatedCheckBox=new QCheckBox(tr("Show related"));
	if(third_activitiesComboBox!=nullptr && third_activityLabel!=nullptr)
		thirdFilter_showRelatedCheckBox=new QCheckBox(tr("Show related"));

	eventLoop=new QEventLoop;

	dialog=new AddOrModifyTimeConstraintDialog(parent, dialogName, dialogTitle, eventLoop,
											   occupyMaxSetsOfTimeSlotsFromSelectionTableWidget, occupyMaxOldItemDelegate, occupyMaxNewItemDelegate,
											   timesTable, oldItemDelegate, newItemDelegate,
											   timesTable1, oldItemDelegate1, newItemDelegate1,
											   timesTable2, oldItemDelegate2, newItemDelegate2,
											   colorsCheckBox,
											   colorsCheckBox1,
											   colorsCheckBox2,
											   showRelatedCheckBox,
											   firstFilter_showRelatedCheckBox,
											   secondFilter_showRelatedCheckBox,
											   thirdFilter_showRelatedCheckBox);
	//dialog->setAttribute(Qt::WA_DeleteOnClose);
	
	//dialog->setWindowTitle(dialogTitle);
	
	//centerWidgetOnScreen(dialog);
	//restoreFETDialogGeometry(dialog, dialogName);

	if(first_filterTeachersComboBox!=nullptr){
		QSize tmp=first_filterTeachersComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(second_filterTeachersComboBox!=nullptr){
		QSize tmp=second_filterTeachersComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(third_filterTeachersComboBox!=nullptr){
		QSize tmp=third_filterTeachersComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}

	if(first_filterStudentsComboBox!=nullptr){
		QSize tmp=first_filterStudentsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(second_filterStudentsComboBox!=nullptr){
		QSize tmp=second_filterStudentsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(third_filterStudentsComboBox!=nullptr){
		QSize tmp=third_filterStudentsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}

	if(first_filterSubjectsComboBox!=nullptr){
		QSize tmp=first_filterSubjectsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(second_filterSubjectsComboBox!=nullptr){
		QSize tmp=second_filterSubjectsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(third_filterSubjectsComboBox!=nullptr){
		QSize tmp=third_filterSubjectsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}

	if(first_filterActivityTagsComboBox!=nullptr){
		QSize tmp=first_filterActivityTagsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(second_filterActivityTagsComboBox!=nullptr){
		QSize tmp=second_filterActivityTagsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(third_filterActivityTagsComboBox!=nullptr){
		QSize tmp=third_filterActivityTagsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	
	if(teachersComboBox!=nullptr){
		QSize tmp=teachersComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(studentsComboBox!=nullptr){
		QSize tmp=studentsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(subjectsComboBox!=nullptr){
		QSize tmp=subjectsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(activityTagsComboBox!=nullptr){
		QSize tmp=activityTagsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(first_activityTagsComboBox!=nullptr){
		QSize tmp=first_activityTagsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(second_activityTagsComboBox!=nullptr){
		QSize tmp=second_activityTagsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(activitiesComboBox!=nullptr){
		QSize tmp=activitiesComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(daysComboBox!=nullptr){
		QSize tmp=daysComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(hoursComboBox!=nullptr){
		QSize tmp=hoursComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(first_activitiesComboBox!=nullptr){
		QSize tmp=first_activitiesComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(second_activitiesComboBox!=nullptr){
		QSize tmp=second_activitiesComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(third_activitiesComboBox!=nullptr){
		QSize tmp=third_activitiesComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(intervalStartHourComboBox!=nullptr){
		QSize tmp=intervalStartHourComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(intervalEndHourComboBox!=nullptr){
		QSize tmp=intervalEndHourComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}

	if(firstDayComboBox!=nullptr){
		QSize tmp=firstDayComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(firstHourComboBox!=nullptr){
		QSize tmp=firstHourComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(secondDayComboBox!=nullptr){
		QSize tmp=secondDayComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(secondHourComboBox!=nullptr){
		QSize tmp=secondHourComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}

	if(activitiesComboBox!=nullptr)
		activitiesComboBox->setMaximumWidth(maxRecommendedWidth(dialog));

	if(first_activitiesComboBox!=nullptr)
		first_activitiesComboBox->setMaximumWidth(maxRecommendedWidth(dialog));
	if(second_activitiesComboBox!=nullptr)
		second_activitiesComboBox->setMaximumWidth(maxRecommendedWidth(dialog));
	if(third_activitiesComboBox!=nullptr)
		third_activitiesComboBox->setMaximumWidth(maxRecommendedWidth(dialog));

	addConstraintPushButton=nullptr;
	addConstraintsPushButton=nullptr;
	closePushButton=nullptr;
	okPushButton=nullptr;
	cancelPushButton=nullptr;
	if(oldtc==nullptr){
		addConstraintPushButton=new QPushButton(tr("Add constraint"));
		addConstraintPushButton->setDefault(true);
		
		switch(type){
			case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_HOURS_DAILY:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_HOURS_DAILY:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_DAYS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_SPAN_PER_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_SPAN_PER_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_RESTING_HOURS:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MIN_HOURS_DAILY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_ACTIVITY_TAG_MIN_HOURS_DAILY:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_REAL_DAYS:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_REAL_DAYS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_REAL_DAYS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_REAL_DAYS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_REAL_DAYS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_AFTERNOONS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_AFTERNOONS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_MORNINGS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_MORNINGS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_MORNINGS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_MORNINGS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_AFTERNOONS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_AFTERNOONS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_MORNINGS:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_MORNINGS:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_AFTERNOONS:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_AFTERNOONS:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_GAPS_PER_REAL_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY_REAL_DAYS:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_HOURS_PER_MORNING:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_SPAN_PER_REAL_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_SPAN_PER_REAL_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_ZERO_GAPS_PER_AFTERNOON:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_ZERO_GAPS_PER_AFTERNOON:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_HOURS_PER_ALL_AFTERNOONS:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_HOURS_PER_ALL_AFTERNOONS:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_THREE_CONSECUTIVE_DAYS:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_THREE_CONSECUTIVE_DAYS:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_HOURS_PER_AFTERNOON:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_IN_INTERVAL:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_IN_INTERVAL:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_NO_TWO_CONSECUTIVE_DAYS:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_NO_TWO_CONSECUTIVE_DAYS:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_HOURS_PER_TERM:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_HOURS_PER_TERM:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
				addConstraintsPushButton=new QPushButton(tr("Add constraints"));
				break;
			
			default:
				//nothing
				break;
		}
		closePushButton=new QPushButton(tr("Close"));
	}
	else{
		okPushButton=new QPushButton(tr("OK"));
		okPushButton->setDefault(true);
		cancelPushButton=new QPushButton(tr("Cancel"));
	}
	
	QHBoxLayout* buttons=new QHBoxLayout;
	buttons->addStretch();
	if(swapTwoSetsOfActivitiesPushButton!=nullptr)
		buttons->addWidget(swapTwoSetsOfActivitiesPushButton);
	if(swapActivitiesPushButton!=nullptr)
		buttons->addWidget(swapActivitiesPushButton);
	if(swapActivityTagsPushButton!=nullptr)
		buttons->addWidget(swapActivityTagsPushButton);
	if(colorsCheckBox!=nullptr)
		if(tabWidget==nullptr && tabWidgetAOMSOTSFS==nullptr)
			buttons->addWidget(colorsCheckBox);
	if(toggleAllPushButton!=nullptr){
		assert(tabWidgetAOMSOTSFS==nullptr);
		if(tabWidget==nullptr)
			buttons->addWidget(toggleAllPushButton);
	}
	if(addConstraintPushButton!=nullptr)
		buttons->addWidget(addConstraintPushButton);
	if(addConstraintsPushButton!=nullptr)
		buttons->addWidget(addConstraintsPushButton);
	if(okPushButton!=nullptr)
		buttons->addWidget(okPushButton);
	if(helpPushButton!=nullptr)
		buttons->addWidget(helpPushButton);
	if(closePushButton!=nullptr)
		buttons->addWidget(closePushButton);
	if(cancelPushButton!=nullptr)
		buttons->addWidget(cancelPushButton);
	
	weightLineEdit=new QLineEdit(QString("100"));
	if(timeConstraintCanHaveAnyWeight(type)){
		switch(type){
			case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_HOURS_DAILY:
				[[fallthrough]];
			case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY:
				[[fallthrough]];
			case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_REAL_DAYS:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS:
				[[fallthrough]];
			case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_REAL_DAYS:
				[[fallthrough]];
			case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_REAL_DAYS:
				weightLabel=new QLabel(tr("Weight percentage\n(recommended: 100.0%,\nallowed: 0.0%-100.0%)"));
				break;
			case CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES:
				[[fallthrough]];
			case CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES:
				weightLabel=new QLabel(tr("Weight percentage (recommended: 95.0% - 100.0%, allowed: 0.0% - 100.0%)"));
				weightLineEdit->setText(QString("95"));
				break;
				
			default:
				weightLabel=new QLabel(tr("Weight percentage (allowed: 0.0%-100.0%)"));
		}
	}
	else{
		weightLabel=new QLabel(tr("Weight percentage (necessary: 100%)"));
		//Not read only, because the old weight may be less than 100%, and the user needs to be able to correct it to 100%.
		//weightLineEdit->setReadOnly(true);
	}
	
	QHBoxLayout* weight=new QHBoxLayout;
	weight->addWidget(weightLabel);
	weight->addWidget(weightLineEdit);
	
	QHBoxLayout* spinBoxLayout=nullptr;
	if(spinBox!=nullptr && labelForSpinBox!=nullptr){
		spinBoxLayout=new QHBoxLayout;

		if(tabWidget!=nullptr){
			assert(colorsCheckBox!=nullptr);
			spinBoxLayout->addWidget(colorsCheckBox);

			assert(toggleAllPushButton!=nullptr);
			spinBoxLayout->addWidget(toggleAllPushButton);
		}
		else if(tabWidgetAOMSOTSFS!=nullptr){
			assert(colorsCheckBox!=nullptr);
			spinBoxLayout->addWidget(colorsCheckBox);
		}

		spinBoxLayout->addWidget(labelForSpinBox);
		spinBoxLayout->addWidget(spinBox);
	}
	
	QHBoxLayout* secondSpinBoxLayout=nullptr;
	if(secondSpinBox!=nullptr && secondLabelForSpinBox!=nullptr){
		secondSpinBoxLayout=new QHBoxLayout;

		secondSpinBoxLayout->addWidget(secondLabelForSpinBox);
		secondSpinBoxLayout->addWidget(secondSpinBox);
	}
	
	QVBoxLayout* wholeDialog=new QVBoxLayout(dialog);
	//wholeDialog->addStretch();
	
	int k1=0;
	if(tabWidget!=nullptr)
		k1=1;
	
	int k2=0;
	if(tabWidgetTwoSetsOfActivities!=nullptr)
		k2=1;
	
	int k3=0;
	if(tabWidgetPairOfMutuallyExclusiveSets!=nullptr)
		k3=1;
	
	int k4=0;
	if(tabWidgetAOMSOTSFS!=nullptr)
		k4=1;
	
	assert(k1+k2+k3+k4<=1);
	
	if(ctrActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots){
		assert(tabWidgetPairOfMutuallyExclusiveSets!=nullptr);
		/*if(oldtc==nullptr){
			if(firstAddInstructionsLabel!=nullptr)
				wholeDialog->addWidget(firstAddInstructionsLabel);
			//if(secondAddInstructionsLabel!=nullptr)
			//	wholeDialog->addWidget(secondAddInstructionsLabel);
		}
		else{
			if(firstModifyInstructionsLabel!=nullptr)
				wholeDialog->addWidget(firstModifyInstructionsLabel);
			//if(secondModifyInstructionsLabel!=nullptr)
			//	wholeDialog->addWidget(secondModifyInstructionsLabel);
		}*/

		//filter
		QWidget* timeSlotsWidget0A=new QWidget;
		QVBoxLayout* layout0A=new QVBoxLayout;
		
		assert(filterGroupBox!=nullptr);

		assert(teacherLayout!=nullptr);
		assert(studentsLayout!=nullptr);
		assert(subjectLayout!=nullptr);
		assert(activityTagLayout!=nullptr);

		QVBoxLayout* layoutLeft=new QVBoxLayout;
		layoutLeft->addLayout(teacherLayout);
		layoutLeft->addLayout(studentsLayout);

		QVBoxLayout* layoutRight=new QVBoxLayout;
		layoutRight->addLayout(subjectLayout);
		layoutRight->addLayout(activityTagLayout);

		QHBoxLayout* layout=new QHBoxLayout;
		layout->addLayout(layoutLeft);
		layout->addLayout(layoutRight);

		filterGroupBox->setLayout(layout);

		layout0A->addWidget(filterGroupBox);
		
		assert(activitiesListWidget!=nullptr);
		assert(activitiesLabel!=nullptr);
		assert(selectedActivitiesLabel!=nullptr);
		assert(selectedActivitiesListWidget!=nullptr);
		assert(addAllActivitiesPushButton!=nullptr);
		assert(clearActivitiesPushButton!=nullptr);

		QVBoxLayout* layout1=new QVBoxLayout;
		layout1->addWidget(activitiesLabel);
		layout1->addWidget(activitiesListWidget);
		layout1->addWidget(addAllActivitiesPushButton);

		QVBoxLayout* layout2=new QVBoxLayout;
		layout2->addWidget(selectedActivitiesLabel);
		layout2->addWidget(selectedActivitiesListWidget);
		layout2->addWidget(clearActivitiesPushButton);

		QHBoxLayout* layout3=new QHBoxLayout;
		layout3->addLayout(layout1);
		layout3->addLayout(layout2);

		layout0A->addLayout(layout3);

		timeSlotsWidget0A->setLayout(layout0A);
		///////

		assert(colorsCheckBox1!=nullptr);
		assert(colorsCheckBox2!=nullptr);
		assert(toggleAllPushButton1!=nullptr);
		assert(toggleAllPushButton2!=nullptr);

		QHBoxLayout* buttons1=new QHBoxLayout;
		buttons1->addStretch();
		if(colorsCheckBox1!=nullptr)
			buttons1->addWidget(colorsCheckBox1);
		if(toggleAllPushButton1!=nullptr)
			buttons1->addWidget(toggleAllPushButton1);

		QHBoxLayout* buttons2=new QHBoxLayout;
		buttons2->addStretch();
		if(colorsCheckBox2!=nullptr)
			buttons2->addWidget(colorsCheckBox2);
		if(toggleAllPushButton2!=nullptr)
			buttons2->addWidget(toggleAllPushButton2);

		QVBoxLayout* timeSlotsLayout1=new QVBoxLayout;
		QVBoxLayout* timeSlotsLayout2=new QVBoxLayout;

		if(firstAddInstructionsLabel!=nullptr)
			timeSlotsLayout1->addWidget(firstAddInstructionsLabel);
		else if(firstModifyInstructionsLabel!=nullptr)
			timeSlotsLayout1->addWidget(firstModifyInstructionsLabel);
		else
			assert(0);

		if(secondAddInstructionsLabel!=nullptr)
			timeSlotsLayout2->addWidget(secondAddInstructionsLabel);
		else if(secondModifyInstructionsLabel!=nullptr)
			timeSlotsLayout2->addWidget(secondModifyInstructionsLabel);
		else
			assert(0);

		assert(timesTable1!=nullptr);
		assert(timesTable2!=nullptr);
		if(timesTable1!=nullptr)
			timeSlotsLayout1->addWidget(timesTable1);
		if(timesTable2!=nullptr)
			timeSlotsLayout2->addWidget(timesTable2);

		timeSlotsLayout1->addLayout(buttons1);
		timeSlotsLayout2->addLayout(buttons2);

		QWidget* timeSlotsWidget1=new QWidget;
		QWidget* timeSlotsWidget2=new QWidget;

		timeSlotsWidget1->setLayout(timeSlotsLayout1);
		timeSlotsWidget2->setLayout(timeSlotsLayout2);

		tabWidgetPairOfMutuallyExclusiveSets->addTab(timeSlotsWidget0A, tr("Activities"));
		tabWidgetPairOfMutuallyExclusiveSets->addTab(timeSlotsWidget1, tr("First set of time slots", "Set, as in a collection of selected time slots"));
		tabWidgetPairOfMutuallyExclusiveSets->addTab(timeSlotsWidget2, tr("Second set of time slots", "Set, as in a collection of selected time slots"));

		wholeDialog->addWidget(tabWidgetPairOfMutuallyExclusiveSets);
		wholeDialog->addLayout(weight);
		wholeDialog->addLayout(buttons);
	}
	else if(ctrActivitiesPairOfMutuallyExclusiveTimeSlots){
		assert(tabWidgetPairOfMutuallyExclusiveSets!=nullptr);
		/*if(oldtc==nullptr){
			if(firstAddInstructionsLabel!=nullptr)
				wholeDialog->addWidget(firstAddInstructionsLabel);
			//if(secondAddInstructionsLabel!=nullptr)
			//	wholeDialog->addWidget(secondAddInstructionsLabel);
		}
		else{
			if(firstModifyInstructionsLabel!=nullptr)
				wholeDialog->addWidget(firstModifyInstructionsLabel);
			//if(secondModifyInstructionsLabel!=nullptr)
			//	wholeDialog->addWidget(secondModifyInstructionsLabel);
		}*/

		//filter
		QWidget* timeSlotsWidget0A=new QWidget;
		QVBoxLayout* layout0A=new QVBoxLayout;
		
		assert(filterGroupBox!=nullptr);

		assert(teacherLayout!=nullptr);
		assert(studentsLayout!=nullptr);
		assert(subjectLayout!=nullptr);
		assert(activityTagLayout!=nullptr);

		QVBoxLayout* layoutLeft=new QVBoxLayout;
		layoutLeft->addLayout(teacherLayout);
		layoutLeft->addLayout(studentsLayout);

		QVBoxLayout* layoutRight=new QVBoxLayout;
		layoutRight->addLayout(subjectLayout);
		layoutRight->addLayout(activityTagLayout);

		QHBoxLayout* layout=new QHBoxLayout;
		layout->addLayout(layoutLeft);
		layout->addLayout(layoutRight);

		filterGroupBox->setLayout(layout);

		layout0A->addWidget(filterGroupBox);
		
		assert(activitiesListWidget!=nullptr);
		assert(activitiesLabel!=nullptr);
		assert(selectedActivitiesLabel!=nullptr);
		assert(selectedActivitiesListWidget!=nullptr);
		assert(addAllActivitiesPushButton!=nullptr);
		assert(clearActivitiesPushButton!=nullptr);

		QVBoxLayout* layout1=new QVBoxLayout;
		layout1->addWidget(activitiesLabel);
		layout1->addWidget(activitiesListWidget);
		layout1->addWidget(addAllActivitiesPushButton);

		QVBoxLayout* layout2=new QVBoxLayout;
		layout2->addWidget(selectedActivitiesLabel);
		layout2->addWidget(selectedActivitiesListWidget);
		layout2->addWidget(clearActivitiesPushButton);

		QHBoxLayout* layout3=new QHBoxLayout;
		layout3->addLayout(layout1);
		layout3->addLayout(layout2);

		layout0A->addLayout(layout3);

		timeSlotsWidget0A->setLayout(layout0A);
		
		///////
		
		if(firstTimeSlotGroupBox!=nullptr){
			assert(firstDayComboBox!=nullptr);
			assert(firstHourComboBox!=nullptr);

			for(int i=0; i<gt.rules.nDaysPerWeek; i++)
				firstDayComboBox->addItem(gt.rules.daysOfTheWeek[i]);
			firstDayComboBox->setCurrentIndex(0);

			for(int i=0; i<gt.rules.nHoursPerDay; i++)
				firstHourComboBox->addItem(gt.rules.hoursOfTheDay[i]);
			firstHourComboBox->setCurrentIndex(0);

			QHBoxLayout* layout=new QHBoxLayout;
			layout->addWidget(firstDayComboBox);
			layout->addWidget(firstHourComboBox);
			firstTimeSlotGroupBox->setLayout(layout);
		}
		else{
			assert(0);
		}

		if(secondTimeSlotGroupBox!=nullptr){
			assert(secondDayComboBox!=nullptr);
			assert(secondHourComboBox!=nullptr);

			for(int i=0; i<gt.rules.nDaysPerWeek; i++)
				secondDayComboBox->addItem(gt.rules.daysOfTheWeek[i]);
			secondDayComboBox->setCurrentIndex(0);

			for(int i=0; i<gt.rules.nHoursPerDay; i++)
				secondHourComboBox->addItem(gt.rules.hoursOfTheDay[i]);
			secondHourComboBox->setCurrentIndex(0);

			QHBoxLayout* layout=new QHBoxLayout;
			layout->addWidget(secondDayComboBox);
			layout->addWidget(secondHourComboBox);
			secondTimeSlotGroupBox->setLayout(layout);
		}
		else{
			assert(0);
		}
		
		///////
		
		QWidget* timeSlotsWidget1=new QWidget;
		QVBoxLayout* layout4=new QVBoxLayout;
		layout4->addWidget(firstTimeSlotGroupBox);
		layout4->addWidget(secondTimeSlotGroupBox);
		timeSlotsWidget1->setLayout(layout4);
		
		tabWidgetPairOfMutuallyExclusiveSets->addTab(timeSlotsWidget0A, tr("Activities"));
		tabWidgetPairOfMutuallyExclusiveSets->addTab(timeSlotsWidget1, tr("Time slots"));

		wholeDialog->addWidget(tabWidgetPairOfMutuallyExclusiveSets);
		wholeDialog->addLayout(weight);
		wholeDialog->addLayout(buttons);
	}
	else if(tabWidgetAOMSOTSFS==nullptr && tabWidget==nullptr && tabWidgetTwoSetsOfActivities==nullptr && tabWidgetPairOfMutuallyExclusiveSets==nullptr){
		if(oldtc==nullptr){
			if(firstAddInstructionsLabel!=nullptr)
				wholeDialog->addWidget(firstAddInstructionsLabel);
			if(secondAddInstructionsLabel!=nullptr)
				wholeDialog->addWidget(secondAddInstructionsLabel);
		}
		else{
			if(firstModifyInstructionsLabel!=nullptr)
				wholeDialog->addWidget(firstModifyInstructionsLabel);
			if(secondModifyInstructionsLabel!=nullptr)
				wholeDialog->addWidget(secondModifyInstructionsLabel);
		}

		if(splitIndexLabel!=nullptr && splitIndexSpinBox!=nullptr){
			QHBoxLayout* layout=new QHBoxLayout;
			layout->addWidget(splitIndexLabel);
			layout->addWidget(splitIndexSpinBox);
			wholeDialog->addLayout(layout);
		}

		if(firstTimeSlotGroupBox!=nullptr){
			assert(firstDayComboBox!=nullptr);
			assert(firstHourComboBox!=nullptr);

			for(int i=0; i<gt.rules.nDaysPerWeek; i++)
				firstDayComboBox->addItem(gt.rules.daysOfTheWeek[i]);
			firstDayComboBox->setCurrentIndex(0);

			for(int i=0; i<gt.rules.nHoursPerDay; i++)
				firstHourComboBox->addItem(gt.rules.hoursOfTheDay[i]);
			firstHourComboBox->setCurrentIndex(0);

			QHBoxLayout* layout=new QHBoxLayout;
			layout->addWidget(firstDayComboBox);
			layout->addWidget(firstHourComboBox);
			firstTimeSlotGroupBox->setLayout(layout);
		}

		if(secondTimeSlotGroupBox!=nullptr){
			assert(secondDayComboBox!=nullptr);
			assert(secondHourComboBox!=nullptr);

			for(int i=0; i<gt.rules.nDaysPerWeek; i++)
				secondDayComboBox->addItem(gt.rules.daysOfTheWeek[i]);
			secondDayComboBox->setCurrentIndex(0);

			for(int i=0; i<gt.rules.nHoursPerDay; i++)
				secondHourComboBox->addItem(gt.rules.hoursOfTheDay[i]);
			secondHourComboBox->setCurrentIndex(0);

			QHBoxLayout* layout=new QHBoxLayout;
			layout->addWidget(secondDayComboBox);
			layout->addWidget(secondHourComboBox);
			secondTimeSlotGroupBox->setLayout(layout);
		}

		if(filterGroupBox!=nullptr){
			assert(teacherLayout!=nullptr);
			assert(studentsLayout!=nullptr);
			assert(subjectLayout!=nullptr);
			assert(activityTagLayout!=nullptr);

			QVBoxLayout* layoutLeft=new QVBoxLayout;
			layoutLeft->addLayout(teacherLayout);
			layoutLeft->addLayout(studentsLayout);

			QVBoxLayout* layoutRight=new QVBoxLayout;
			layoutRight->addLayout(subjectLayout);
			layoutRight->addLayout(activityTagLayout);

			QHBoxLayout* layout=new QHBoxLayout;
			layout->addLayout(layoutLeft);
			layout->addLayout(layoutRight);

			filterGroupBox->setLayout(layout);

			wholeDialog->addWidget(filterGroupBox);
		}
		else if(teacherLayout!=nullptr && studentsLayout!=nullptr && subjectLayout!=nullptr && activityTagLayout!=nullptr){
			QVBoxLayout* layoutLeft=new QVBoxLayout;
			layoutLeft->addLayout(teacherLayout);
			layoutLeft->addLayout(studentsLayout);

			QVBoxLayout* layoutRight=new QVBoxLayout;
			layoutRight->addLayout(subjectLayout);
			layoutRight->addLayout(activityTagLayout);

			QHBoxLayout* layout=new QHBoxLayout;
			layout->addLayout(layoutLeft);
			layout->addLayout(layoutRight);

			wholeDialog->addLayout(layout);
		}
		else{
			if(teacherLayout!=nullptr)
				wholeDialog->addLayout(teacherLayout);
			if(studentsLayout!=nullptr)
				wholeDialog->addLayout(studentsLayout);
			if(activityTagLayout!=nullptr)
				wholeDialog->addLayout(activityTagLayout);

			if(first_activityTagLayout!=nullptr)
				wholeDialog->addLayout(first_activityTagLayout);
			if(second_activityTagLayout!=nullptr)
				wholeDialog->addLayout(second_activityTagLayout);

			if(firstTimeSlotGroupBox!=nullptr)
				wholeDialog->addWidget(firstTimeSlotGroupBox);
			if(secondTimeSlotGroupBox!=nullptr)
				wholeDialog->addWidget(secondTimeSlotGroupBox);
		}

		if(intervalStartHourLabel!=nullptr && intervalStartHourComboBox!=nullptr && intervalEndHourLabel!=nullptr && intervalEndHourComboBox!=nullptr){
			for(int i=0; i<gt.rules.nHoursPerDay; i++)
				intervalStartHourComboBox->addItem(gt.rules.hoursOfTheDay[i]);
			intervalStartHourComboBox->setCurrentIndex(gt.rules.nHoursPerDay-1);

			for(int i=0; i<gt.rules.nHoursPerDay; i++)
				intervalEndHourComboBox->addItem(gt.rules.hoursOfTheDay[i]);
			intervalEndHourComboBox->addItem(tr("End of day"));
			intervalEndHourComboBox->setCurrentIndex(gt.rules.nHoursPerDay);

			wholeDialog->addWidget(intervalStartHourLabel);
			wholeDialog->addWidget(intervalStartHourComboBox);
			wholeDialog->addWidget(intervalEndHourLabel);
			wholeDialog->addWidget(intervalEndHourComboBox);
		}

		if(activitiesComboBox!=nullptr && activityLabel!=nullptr){
			wholeDialog->addWidget(activityLabel);
			wholeDialog->addWidget(activitiesComboBox);
		}

		if(first_activitiesComboBox!=nullptr && first_activityLabel!=nullptr){
			assert(first_filterGroupBox!=nullptr);
			assert(first_filterTeachersComboBox!=nullptr);
			assert(first_filterStudentsComboBox!=nullptr);
			assert(first_filterSubjectsComboBox!=nullptr);
			assert(first_filterActivityTagsComboBox!=nullptr);

			if(!filterIsOnSingleRow){
				QVBoxLayout* layoutLeft=new QVBoxLayout;
				layoutLeft->addWidget(first_filterTeachersComboBox);
				
				///////
				//firstFilter_showRelatedCheckBox=new QCheckBox(tr("Show related"));

				QSettings settings(COMPANY, PROGRAM);
				firstFilter_showRelatedCheckBox->setChecked(settings.value(dialogName+QString("/first-filter-show-related"), "false").toBool());

				connect(firstFilter_showRelatedCheckBox, &QCheckBox::toggled, this, &AddOrModifyTimeConstraint::firstFilter_showRelatedCheckBoxToggled);

				QHBoxLayout* ssrl=new QHBoxLayout;

				ssrl->addWidget(first_filterStudentsComboBox);
				ssrl->addWidget(firstFilter_showRelatedCheckBox);

				layoutLeft->addLayout(ssrl);
				///////

				QVBoxLayout* layoutRight=new QVBoxLayout;
				layoutRight->addWidget(first_filterSubjectsComboBox);
				layoutRight->addWidget(first_filterActivityTagsComboBox);

				QHBoxLayout* layout=new QHBoxLayout;
				layout->addLayout(layoutLeft);
				layout->addLayout(layoutRight);

				first_filterGroupBox->setLayout(layout);
			}
			else{
				QHBoxLayout* layout=new QHBoxLayout;
				layout->addWidget(first_filterTeachersComboBox);

				///////
				//firstFilter_showRelatedCheckBox=new QCheckBox(tr("Show related"));

				QSettings settings(COMPANY, PROGRAM);
				firstFilter_showRelatedCheckBox->setChecked(settings.value(dialogName+QString("/first-filter-show-related"), "false").toBool());

				connect(firstFilter_showRelatedCheckBox, &QCheckBox::toggled, this, &AddOrModifyTimeConstraint::firstFilter_showRelatedCheckBoxToggled);

				layout->addWidget(first_filterStudentsComboBox);
				layout->addWidget(firstFilter_showRelatedCheckBox);
				///////

				layout->addWidget(first_filterSubjectsComboBox);
				layout->addWidget(first_filterActivityTagsComboBox);

				first_filterGroupBox->setLayout(layout);
			}

			wholeDialog->addWidget(first_filterGroupBox);

			wholeDialog->addWidget(first_activityLabel);
			wholeDialog->addWidget(first_activitiesComboBox);
		}
		if(second_activitiesComboBox!=nullptr && second_activityLabel!=nullptr){
			assert(second_filterGroupBox!=nullptr);
			assert(second_filterTeachersComboBox!=nullptr);
			assert(second_filterStudentsComboBox!=nullptr);
			assert(second_filterSubjectsComboBox!=nullptr);
			assert(second_filterActivityTagsComboBox!=nullptr);

			if(!filterIsOnSingleRow){
				QVBoxLayout* layoutLeft=new QVBoxLayout;
				layoutLeft->addWidget(second_filterTeachersComboBox);

				///////
				//secondFilter_showRelatedCheckBox=new QCheckBox(tr("Show related"));

				QSettings settings(COMPANY, PROGRAM);
				secondFilter_showRelatedCheckBox->setChecked(settings.value(dialogName+QString("/second-filter-show-related"), "false").toBool());

				connect(secondFilter_showRelatedCheckBox, &QCheckBox::toggled, this, &AddOrModifyTimeConstraint::secondFilter_showRelatedCheckBoxToggled);

				QHBoxLayout* ssrl=new QHBoxLayout;

				ssrl->addWidget(second_filterStudentsComboBox);
				ssrl->addWidget(secondFilter_showRelatedCheckBox);

				layoutLeft->addLayout(ssrl);
				///////

				QVBoxLayout* layoutRight=new QVBoxLayout;
				layoutRight->addWidget(second_filterSubjectsComboBox);
				layoutRight->addWidget(second_filterActivityTagsComboBox);

				QHBoxLayout* layout=new QHBoxLayout;
				layout->addLayout(layoutLeft);
				layout->addLayout(layoutRight);

				second_filterGroupBox->setLayout(layout);
			}
			else{
				QHBoxLayout* layout=new QHBoxLayout;
				layout->addWidget(second_filterTeachersComboBox);

				///////
				//secondFilter_showRelatedCheckBox=new QCheckBox(tr("Show related"));

				QSettings settings(COMPANY, PROGRAM);
				secondFilter_showRelatedCheckBox->setChecked(settings.value(dialogName+QString("/second-filter-show-related"), "false").toBool());

				connect(secondFilter_showRelatedCheckBox, &QCheckBox::toggled, this, &AddOrModifyTimeConstraint::secondFilter_showRelatedCheckBoxToggled);

				layout->addWidget(second_filterStudentsComboBox);
				layout->addWidget(secondFilter_showRelatedCheckBox);
				///////

				layout->addWidget(second_filterSubjectsComboBox);
				layout->addWidget(second_filterActivityTagsComboBox);

				second_filterGroupBox->setLayout(layout);
			}

			wholeDialog->addWidget(second_filterGroupBox);

			wholeDialog->addWidget(second_activityLabel);
			wholeDialog->addWidget(second_activitiesComboBox);
		}
		if(third_activitiesComboBox!=nullptr && third_activityLabel!=nullptr){
			assert(third_filterGroupBox!=nullptr);
			assert(third_filterTeachersComboBox!=nullptr);
			assert(third_filterStudentsComboBox!=nullptr);
			assert(third_filterSubjectsComboBox!=nullptr);
			assert(third_filterActivityTagsComboBox!=nullptr);

			if(!filterIsOnSingleRow){
				QVBoxLayout* layoutLeft=new QVBoxLayout;
				layoutLeft->addWidget(third_filterTeachersComboBox);

				///////
				//thirdFilter_showRelatedCheckBox=new QCheckBox(tr("Show related"));

				QSettings settings(COMPANY, PROGRAM);
				thirdFilter_showRelatedCheckBox->setChecked(settings.value(dialogName+QString("/third-filter-show-related"), "false").toBool());

				connect(thirdFilter_showRelatedCheckBox, &QCheckBox::toggled, this, &AddOrModifyTimeConstraint::thirdFilter_showRelatedCheckBoxToggled);

				QHBoxLayout* ssrl=new QHBoxLayout;

				ssrl->addWidget(third_filterStudentsComboBox);
				ssrl->addWidget(thirdFilter_showRelatedCheckBox);

				layoutLeft->addLayout(ssrl);
				///////

				QVBoxLayout* layoutRight=new QVBoxLayout;
				layoutRight->addWidget(third_filterSubjectsComboBox);
				layoutRight->addWidget(third_filterActivityTagsComboBox);

				QHBoxLayout* layout=new QHBoxLayout;
				layout->addLayout(layoutLeft);
				layout->addLayout(layoutRight);

				third_filterGroupBox->setLayout(layout);
			}
			else{
				QHBoxLayout* layout=new QHBoxLayout;
				layout->addWidget(third_filterTeachersComboBox);

				///////
				//thirdFilter_showRelatedCheckBox=new QCheckBox(tr("Show related"));

				QSettings settings(COMPANY, PROGRAM);
				thirdFilter_showRelatedCheckBox->setChecked(settings.value(dialogName+QString("/third-filter-show-related"), "false").toBool());

				connect(thirdFilter_showRelatedCheckBox, &QCheckBox::toggled, this, &AddOrModifyTimeConstraint::thirdFilter_showRelatedCheckBoxToggled);

				layout->addWidget(third_filterStudentsComboBox);
				layout->addWidget(thirdFilter_showRelatedCheckBox);
				///////

				layout->addWidget(third_filterSubjectsComboBox);
				layout->addWidget(third_filterActivityTagsComboBox);

				third_filterGroupBox->setLayout(layout);
			}

			wholeDialog->addWidget(third_filterGroupBox);

			wholeDialog->addWidget(third_activityLabel);
			wholeDialog->addWidget(third_activitiesComboBox);
		}

		if(activitiesListWidget!=nullptr){
			assert(activitiesLabel!=nullptr);
			assert(selectedActivitiesLabel!=nullptr);
			assert(selectedActivitiesListWidget!=nullptr);
			assert(addAllActivitiesPushButton!=nullptr);
			assert(clearActivitiesPushButton!=nullptr);

			QVBoxLayout* layout1=new QVBoxLayout;
			layout1->addWidget(activitiesLabel);
			layout1->addWidget(activitiesListWidget);
			layout1->addWidget(addAllActivitiesPushButton);

			QVBoxLayout* layout2=new QVBoxLayout;
			layout2->addWidget(selectedActivitiesLabel);
			layout2->addWidget(selectedActivitiesListWidget);
			layout2->addWidget(clearActivitiesPushButton);

			QHBoxLayout* layout3=new QHBoxLayout;
			layout3->addLayout(layout1);
			layout3->addLayout(layout2);

			wholeDialog->addLayout(layout3);
		}

		if(activityTagsListWidget!=nullptr){
			assert(activityTagsLabel!=nullptr);
			assert(selectedActivityTagsLabel!=nullptr);
			assert(selectedActivityTagsListWidget!=nullptr);
			assert(addAllActivityTagsPushButton!=nullptr);
			assert(clearActivityTagsPushButton!=nullptr);

			QVBoxLayout* layout1=new QVBoxLayout;
			layout1->addWidget(activityTagsLabel);
			layout1->addWidget(activityTagsListWidget);
			layout1->addWidget(addAllActivityTagsPushButton);

			QVBoxLayout* layout2=new QVBoxLayout;
			layout2->addWidget(selectedActivityTagsLabel);
			layout2->addWidget(selectedActivityTagsListWidget);
			layout2->addWidget(clearActivityTagsPushButton);

			QHBoxLayout* layout3=new QHBoxLayout;
			layout3->addLayout(layout1);
			layout3->addLayout(layout2);

			wholeDialog->addLayout(layout3);
		}

		if(durationCheckBox!=nullptr && durationSpinBox!=nullptr){
			QHBoxLayout* layout=new QHBoxLayout;
			layout->addWidget(durationCheckBox);
			layout->addWidget(durationSpinBox);
			wholeDialog->addLayout(layout);
		}

		if(timesTable!=nullptr)
			wholeDialog->addWidget(timesTable);
		
		if(occupyMaxSetsOfTimeSlotsFromSelectionTableWidget!=nullptr){
			assert(tabWidgetAOMSOTSFS==nullptr);
			
			occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->resizeRowsToContents();
			//occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->resizeColumnsToContents();

			occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->setSelectionMode(QAbstractItemView::NoSelection);

			setStretchAvailabilityTableNicely(occupyMaxSetsOfTimeSlotsFromSelectionTableWidget);

			wholeDialog->addWidget(occupyMaxSetsOfTimeSlotsFromSelectionTableWidget);
			
			initOccupyMaxTableWidget();
		}
		
		if(spinBoxLayout!=nullptr)
			wholeDialog->addLayout(spinBoxLayout);
		if(secondSpinBoxLayout!=nullptr)
			wholeDialog->addLayout(secondSpinBoxLayout);
		if(checkBox!=nullptr)
			wholeDialog->addWidget(checkBox);

		if(periodGroupBox!=nullptr)
			wholeDialog->addWidget(periodGroupBox);
		if(permanentlyLockedCheckBox!=nullptr)
			wholeDialog->addWidget(permanentlyLockedCheckBox);
		if(permanentlyLockedLabel!=nullptr)
			wholeDialog->addWidget(permanentlyLockedLabel);

		//wholeDialog->addStretch();
		wholeDialog->addLayout(weight);
		wholeDialog->addLayout(buttons);
	}
	else if(tabWidgetAOMSOTSFS!=nullptr){
		assert(tabWidget==nullptr && tabWidgetTwoSetsOfActivities==nullptr && tabWidgetPairOfMutuallyExclusiveSets==nullptr);

		if(occupyMaxSetsOfTimeSlotsFromSelectionTableWidget!=nullptr){
			occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->resizeRowsToContents();
			//occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->resizeColumnsToContents();

			occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->setSelectionMode(QAbstractItemView::NoSelection);

			setStretchAvailabilityTableNicely(occupyMaxSetsOfTimeSlotsFromSelectionTableWidget);

			initOccupyMaxTableWidget();
		}
		else{
			assert(0);
		}

		QVBoxLayout* activitiesLayout=new QVBoxLayout;
		QVBoxLayout* timeSlotsLayout=new QVBoxLayout;

		if(oldtc==nullptr){
			if(firstAddInstructionsLabel!=nullptr)
				timeSlotsLayout->addWidget(firstAddInstructionsLabel);
			assert(secondAddInstructionsLabel==nullptr);
			//if(secondAddInstructionsLabel!=nullptr)
				//timeSlotsLayout->addWidget(secondAddInstructionsLabel);
		}
		else{
			if(firstModifyInstructionsLabel!=nullptr)
				timeSlotsLayout->addWidget(firstModifyInstructionsLabel);
			assert(secondModifyInstructionsLabel==nullptr);
			//if(secondModifyInstructionsLabel!=nullptr)
				//wholeDialog->addWidget(secondModifyInstructionsLabel);
		}

		assert(teacherLayout!=nullptr);
		assert(studentsLayout!=nullptr);
		assert(subjectLayout!=nullptr);
		assert(activityTagLayout!=nullptr);

		QVBoxLayout* layoutLeft=new QVBoxLayout;
		layoutLeft->addLayout(teacherLayout);
		layoutLeft->addLayout(studentsLayout);

		QVBoxLayout* layoutRight=new QVBoxLayout;
		layoutRight->addLayout(subjectLayout);
		layoutRight->addLayout(activityTagLayout);

		QHBoxLayout* layout=new QHBoxLayout;
		layout->addLayout(layoutLeft);
		layout->addLayout(layoutRight);

		filterGroupBox->setLayout(layout);

		activitiesLayout->addWidget(filterGroupBox);

		if(activitiesListWidget!=nullptr){
			assert(activitiesLabel!=nullptr);
			assert(selectedActivitiesLabel!=nullptr);
			assert(selectedActivitiesListWidget!=nullptr);
			assert(addAllActivitiesPushButton!=nullptr);
			assert(clearActivitiesPushButton!=nullptr);

			QVBoxLayout* layout1=new QVBoxLayout;
			layout1->addWidget(activitiesLabel);
			layout1->addWidget(activitiesListWidget);
			layout1->addWidget(addAllActivitiesPushButton);

			QVBoxLayout* layout2=new QVBoxLayout;
			layout2->addWidget(selectedActivitiesLabel);
			layout2->addWidget(selectedActivitiesListWidget);
			layout2->addWidget(clearActivitiesPushButton);

			QHBoxLayout* layout3=new QHBoxLayout;
			layout3->addLayout(layout1);
			layout3->addLayout(layout2);

			activitiesLayout->addLayout(layout3);
		}
		else{
			assert(0);
		}

		if(occupyMaxSetsOfTimeSlotsFromSelectionTableWidget!=nullptr)
			timeSlotsLayout->addWidget(occupyMaxSetsOfTimeSlotsFromSelectionTableWidget);
		else
			assert(0);
		if(spinBoxLayout!=nullptr)
			timeSlotsLayout->addLayout(spinBoxLayout);
		else
			assert(0);

		QWidget* activitiesWidget=new QWidget;
		QWidget* timeSlotsWidget=new QWidget;

		activitiesWidget->setLayout(activitiesLayout);
		timeSlotsWidget->setLayout(timeSlotsLayout);

		tabWidgetAOMSOTSFS->addTab(activitiesWidget, tr("Activities"));
		tabWidgetAOMSOTSFS->addTab(timeSlotsWidget, tr("Time slots"));

		//wholeDialog->addStretch();
		wholeDialog->addWidget(tabWidgetAOMSOTSFS);
		wholeDialog->addLayout(weight);
		wholeDialog->addLayout(buttons);
	}
	else if(tabWidget!=nullptr){
		assert(tabWidgetTwoSetsOfActivities==nullptr && tabWidgetPairOfMutuallyExclusiveSets==nullptr);

		QVBoxLayout* activitiesLayout=new QVBoxLayout;
		QVBoxLayout* timeSlotsLayout=new QVBoxLayout;

		if(oldtc==nullptr){
			if(firstAddInstructionsLabel!=nullptr)
				timeSlotsLayout->addWidget(firstAddInstructionsLabel);
			assert(secondAddInstructionsLabel==nullptr);
			//if(secondAddInstructionsLabel!=nullptr)
				//timeSlotsLayout->addWidget(secondAddInstructionsLabel);
		}
		else{
			if(firstModifyInstructionsLabel!=nullptr)
				timeSlotsLayout->addWidget(firstModifyInstructionsLabel);
			assert(secondModifyInstructionsLabel==nullptr);
			//if(secondModifyInstructionsLabel!=nullptr)
				//wholeDialog->addWidget(secondModifyInstructionsLabel);
		}

		assert(teacherLayout!=nullptr);
		assert(studentsLayout!=nullptr);
		assert(subjectLayout!=nullptr);
		assert(activityTagLayout!=nullptr);

		QVBoxLayout* layoutLeft=new QVBoxLayout;
		layoutLeft->addLayout(teacherLayout);
		layoutLeft->addLayout(studentsLayout);

		QVBoxLayout* layoutRight=new QVBoxLayout;
		layoutRight->addLayout(subjectLayout);
		layoutRight->addLayout(activityTagLayout);

		QHBoxLayout* layout=new QHBoxLayout;
		layout->addLayout(layoutLeft);
		layout->addLayout(layoutRight);

		filterGroupBox->setLayout(layout);

		activitiesLayout->addWidget(filterGroupBox);

		if(activitiesListWidget!=nullptr){
			assert(activitiesLabel!=nullptr);
			assert(selectedActivitiesLabel!=nullptr);
			assert(selectedActivitiesListWidget!=nullptr);
			assert(addAllActivitiesPushButton!=nullptr);
			assert(clearActivitiesPushButton!=nullptr);

			QVBoxLayout* layout1=new QVBoxLayout;
			layout1->addWidget(activitiesLabel);
			layout1->addWidget(activitiesListWidget);
			layout1->addWidget(addAllActivitiesPushButton);

			QVBoxLayout* layout2=new QVBoxLayout;
			layout2->addWidget(selectedActivitiesLabel);
			layout2->addWidget(selectedActivitiesListWidget);
			layout2->addWidget(clearActivitiesPushButton);

			QHBoxLayout* layout3=new QHBoxLayout;
			layout3->addLayout(layout1);
			layout3->addLayout(layout2);

			activitiesLayout->addLayout(layout3);
		}

		if(timesTable!=nullptr)
			timeSlotsLayout->addWidget(timesTable);
		if(spinBoxLayout!=nullptr)
			timeSlotsLayout->addLayout(spinBoxLayout);
		//if(secondSpinBoxLayout!=nullptr)
		//	wholeDialog->addLayout(secondSpinBoxLayout);
		if(allowEmptySlotsCheckBox!=nullptr)
			timeSlotsLayout->addWidget(allowEmptySlotsCheckBox);

		QWidget* activitiesWidget=new QWidget;
		QWidget* timeSlotsWidget=new QWidget;

		activitiesWidget->setLayout(activitiesLayout);
		timeSlotsWidget->setLayout(timeSlotsLayout);

		tabWidget->addTab(activitiesWidget, tr("Activities"));
		tabWidget->addTab(timeSlotsWidget, tr("Time slots"));

		//wholeDialog->addStretch();
		wholeDialog->addWidget(tabWidget);
		wholeDialog->addLayout(weight);
		wholeDialog->addLayout(buttons);
	}
	else if(tabWidgetTwoSetsOfActivities!=nullptr){
		assert(tabWidgetPairOfMutuallyExclusiveSets==nullptr);

		assert(teacherLayout!=nullptr);
		assert(studentsLayout!=nullptr);
		assert(subjectLayout!=nullptr);
		assert(activityTagLayout!=nullptr);

		QVBoxLayout* layoutLeft=new QVBoxLayout;
		layoutLeft->addLayout(teacherLayout);
		layoutLeft->addLayout(studentsLayout);

		QVBoxLayout* layoutRight=new QVBoxLayout;
		layoutRight->addLayout(subjectLayout);
		layoutRight->addLayout(activityTagLayout);

		QHBoxLayout* layout=new QHBoxLayout;
		layout->addLayout(layoutLeft);
		layout->addLayout(layoutRight);

		filterGroupBox->setLayout(layout);

		wholeDialog->addWidget(filterGroupBox);

		QVBoxLayout* activitiesLayout1=new QVBoxLayout;
		QVBoxLayout* activitiesLayout2=new QVBoxLayout;

		if(activitiesListWidget_TwoSetsOfActivities_1!=nullptr){
			assert(activitiesLabel_TwoSetsOfActivities_1!=nullptr);
			assert(selectedActivitiesLabel_TwoSetsOfActivities_1!=nullptr);
			assert(selectedActivitiesListWidget_TwoSetsOfActivities_1!=nullptr);
			assert(addAllActivitiesPushButton_TwoSetsOfActivities_1!=nullptr);
			assert(clearActivitiesPushButton_TwoSetsOfActivities_1!=nullptr);

			QVBoxLayout* layout1=new QVBoxLayout;
			layout1->addWidget(activitiesLabel_TwoSetsOfActivities_1);
			layout1->addWidget(activitiesListWidget_TwoSetsOfActivities_1);
			layout1->addWidget(addAllActivitiesPushButton_TwoSetsOfActivities_1);

			QVBoxLayout* layout2=new QVBoxLayout;
			layout2->addWidget(selectedActivitiesLabel_TwoSetsOfActivities_1);
			layout2->addWidget(selectedActivitiesListWidget_TwoSetsOfActivities_1);
			layout2->addWidget(clearActivitiesPushButton_TwoSetsOfActivities_1);

			QHBoxLayout* layout3=new QHBoxLayout;
			layout3->addLayout(layout1);
			layout3->addLayout(layout2);

			activitiesLayout1->addLayout(layout3);
		}

		if(activitiesListWidget_TwoSetsOfActivities_2!=nullptr){
			assert(activitiesLabel_TwoSetsOfActivities_2!=nullptr);
			assert(selectedActivitiesLabel_TwoSetsOfActivities_2!=nullptr);
			assert(selectedActivitiesListWidget_TwoSetsOfActivities_2!=nullptr);
			assert(addAllActivitiesPushButton_TwoSetsOfActivities_2!=nullptr);
			assert(clearActivitiesPushButton_TwoSetsOfActivities_2!=nullptr);

			QVBoxLayout* layout1=new QVBoxLayout;
			layout1->addWidget(activitiesLabel_TwoSetsOfActivities_2);
			layout1->addWidget(activitiesListWidget_TwoSetsOfActivities_2);
			layout1->addWidget(addAllActivitiesPushButton_TwoSetsOfActivities_2);

			QVBoxLayout* layout2=new QVBoxLayout;
			layout2->addWidget(selectedActivitiesLabel_TwoSetsOfActivities_2);
			layout2->addWidget(selectedActivitiesListWidget_TwoSetsOfActivities_2);
			layout2->addWidget(clearActivitiesPushButton_TwoSetsOfActivities_2);

			QHBoxLayout* layout3=new QHBoxLayout;
			layout3->addLayout(layout1);
			layout3->addLayout(layout2);

			activitiesLayout2->addLayout(layout3);
		}

		QWidget* timeSlotsWidget=nullptr;
		if(timesTable!=nullptr){
			QVBoxLayout* timeSlotsLayout=new QVBoxLayout;

			if(oldtc==nullptr){
				if(firstAddInstructionsLabel!=nullptr)
					timeSlotsLayout->addWidget(firstAddInstructionsLabel);
				if(secondAddInstructionsLabel!=nullptr)
					timeSlotsLayout->addWidget(secondAddInstructionsLabel);
			}
			else{
				if(firstModifyInstructionsLabel!=nullptr)
					timeSlotsLayout->addWidget(firstModifyInstructionsLabel);
				if(secondModifyInstructionsLabel!=nullptr)
					timeSlotsLayout->addWidget(secondModifyInstructionsLabel);
			}

			timeSlotsLayout->addWidget(timesTable);
			timeSlotsWidget=new QWidget;
			timeSlotsWidget->setLayout(timeSlotsLayout);
		}

		QWidget* activitiesWidget1=new QWidget;
		QWidget* activitiesWidget2=new QWidget;

		activitiesWidget1->setLayout(activitiesLayout1);
		activitiesWidget2->setLayout(activitiesLayout2);

		tabWidgetTwoSetsOfActivities->addTab(activitiesWidget1, tr("First set of activities", "Set, as in a collection of selected activities"));
		tabWidgetTwoSetsOfActivities->addTab(activitiesWidget2, tr("Second set of activities", "Set, as in a collection of selected activities"));
		if(timesTable!=nullptr){
			tabWidgetTwoSetsOfActivities->addTab(timeSlotsWidget, tr("Exception time slots", "The selected time slots constitute an exception"));
		}

		//wholeDialog->addStretch();
		wholeDialog->addWidget(tabWidgetTwoSetsOfActivities);
		wholeDialog->addLayout(weight);
		wholeDialog->addLayout(buttons);
	}
	else if(tabWidgetPairOfMutuallyExclusiveSets!=nullptr){
		if(oldtc==nullptr){
			if(firstAddInstructionsLabel!=nullptr)
				wholeDialog->addWidget(firstAddInstructionsLabel);
			//if(secondAddInstructionsLabel!=nullptr)
			//	wholeDialog->addWidget(secondAddInstructionsLabel);
		}
		else{
			if(firstModifyInstructionsLabel!=nullptr)
				wholeDialog->addWidget(firstModifyInstructionsLabel);
			//if(secondModifyInstructionsLabel!=nullptr)
			//	wholeDialog->addWidget(secondModifyInstructionsLabel);
		}

		if(teacherLayout!=nullptr)
			wholeDialog->addLayout(teacherLayout);
		if(studentsLayout!=nullptr)
			wholeDialog->addLayout(studentsLayout);
		assert(activityTagLayout==nullptr);

		assert(colorsCheckBox1!=nullptr);
		assert(colorsCheckBox2!=nullptr);
		assert(toggleAllPushButton1!=nullptr);
		assert(toggleAllPushButton2!=nullptr);

		QHBoxLayout* buttons1=new QHBoxLayout;
		buttons1->addStretch();
		if(colorsCheckBox1!=nullptr)
			buttons1->addWidget(colorsCheckBox1);
		if(toggleAllPushButton1!=nullptr)
			buttons1->addWidget(toggleAllPushButton1);

		QHBoxLayout* buttons2=new QHBoxLayout;
		buttons2->addStretch();
		if(colorsCheckBox2!=nullptr)
			buttons2->addWidget(colorsCheckBox2);
		if(toggleAllPushButton2!=nullptr)
			buttons2->addWidget(toggleAllPushButton2);

		QVBoxLayout* timeSlotsLayout1=new QVBoxLayout;
		QVBoxLayout* timeSlotsLayout2=new QVBoxLayout;

		assert(timesTable1!=nullptr);
		assert(timesTable2!=nullptr);
		if(timesTable1!=nullptr)
			timeSlotsLayout1->addWidget(timesTable1);
		if(timesTable2!=nullptr)
			timeSlotsLayout2->addWidget(timesTable2);

		timeSlotsLayout1->addLayout(buttons1);
		timeSlotsLayout2->addLayout(buttons2);

		QWidget* timeSlotsWidget1=new QWidget;
		QWidget* timeSlotsWidget2=new QWidget;

		timeSlotsWidget1->setLayout(timeSlotsLayout1);
		timeSlotsWidget2->setLayout(timeSlotsLayout2);

		tabWidgetPairOfMutuallyExclusiveSets->addTab(timeSlotsWidget1, tr("First set of time slots", "Set, as in a collection of selected time slots"));
		tabWidgetPairOfMutuallyExclusiveSets->addTab(timeSlotsWidget2, tr("Second set of time slots", "Set, as in a collection of selected time slots"));

		wholeDialog->addWidget(tabWidgetPairOfMutuallyExclusiveSets);
		wholeDialog->addLayout(weight);
		wholeDialog->addLayout(buttons);
	}

	if(addConstraintPushButton!=nullptr)
		connect(addConstraintPushButton, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::addConstraintClicked);
	if(addConstraintsPushButton!=nullptr)
		connect(addConstraintsPushButton, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::addConstraintsClicked);
	if(okPushButton!=nullptr)
		connect(okPushButton, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::okClicked);
	if(helpPushButton!=nullptr)
		connect(helpPushButton, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::helpClicked);
	if(closePushButton!=nullptr)
		connect(closePushButton, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::closeClicked);
	if(cancelPushButton!=nullptr)
		connect(cancelPushButton, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::cancelClicked);
	if(swapTwoSetsOfActivitiesPushButton!=nullptr)
		connect(swapTwoSetsOfActivitiesPushButton, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::swapTwoSetsOfActivitiesPushButtonClicked);
	if(swapActivitiesPushButton!=nullptr)
		connect(swapActivitiesPushButton, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::swapActivitiesPushButtonClicked);
	if(swapActivityTagsPushButton!=nullptr)
		connect(swapActivityTagsPushButton, &QPushButton::clicked, this, &AddOrModifyTimeConstraint::swapActivityTagsPushButtonClicked);

	if(oldtc!=nullptr){
		weightLineEdit->setText(CustomFETString::number(oldtc->weightPercentage));
		switch(type){
			//1
			case CONSTRAINT_BASIC_COMPULSORY_TIME:
				{
					break;
				}
			//2
			case CONSTRAINT_BREAK_TIMES:
				{
					ConstraintBreakTimes* ctr=(ConstraintBreakTimes*)oldtc;

					fillTimesTable(timesTable, ctr->days, ctr->hours, true);

					break;
				}
			//3
			case CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES:
				{
					ConstraintTeacherNotAvailableTimes* ctr=(ConstraintTeacherNotAvailableTimes*)oldtc;

					fillTimesTable(timesTable, ctr->days, ctr->hours, true);

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacher));

					break;
				}
			//4
			case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY:
				{
					ConstraintTeachersMaxHoursDaily* ctr=(ConstraintTeachersMaxHoursDaily*)oldtc;

					spinBox->setValue(ctr->maxHoursDaily);

					break;
				}
			//5
			case CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK:
				{
					ConstraintTeacherMaxDaysPerWeek* ctr=(ConstraintTeacherMaxDaysPerWeek*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxDaysPerWeek);

					break;
				}
			//6
			case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK:
			{
				ConstraintTeachersMaxGapsPerWeek* ctr=(ConstraintTeachersMaxGapsPerWeek*)oldtc;

				spinBox->setValue(ctr->maxGaps);

				break;
			}
			//7
			case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK:
			{
				ConstraintTeacherMaxGapsPerWeek* ctr=(ConstraintTeacherMaxGapsPerWeek*)oldtc;

				teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
				spinBox->setValue(ctr->maxGaps);

				break;
			}
			//8
			case CONSTRAINT_TEACHER_MAX_HOURS_DAILY:
				{
					ConstraintTeacherMaxHoursDaily* ctr=(ConstraintTeacherMaxHoursDaily*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxHoursDaily);

					break;
				}
			//9
			case CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY:
				{
					ConstraintTeachersMaxHoursContinuously* ctr=(ConstraintTeachersMaxHoursContinuously*)oldtc;

					spinBox->setValue(ctr->maxHoursContinuously);

					break;
				}
			//10
			case CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY:
				{
					ConstraintTeacherMaxHoursContinuously* ctr=(ConstraintTeacherMaxHoursContinuously*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxHoursContinuously);

					break;
				}
			//11
			case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY:
				{
					ConstraintTeachersMinHoursDaily* ctr=(ConstraintTeachersMinHoursDaily*)oldtc;

					spinBox->setValue(ctr->minHoursDaily);
					checkBox->setChecked(ctr->allowEmptyDays);

					break;
				}
			//12
			case CONSTRAINT_TEACHER_MIN_HOURS_DAILY:
				{
					ConstraintTeacherMinHoursDaily* ctr=(ConstraintTeacherMinHoursDaily*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->minHoursDaily);
					checkBox->setChecked(ctr->allowEmptyDays);

					break;
				}
			//13
			case CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY:
				{
					ConstraintTeachersMaxGapsPerDay* ctr=(ConstraintTeachersMaxGapsPerDay*)oldtc;

					spinBox->setValue(ctr->maxGaps);

					break;
				}
			//14
			case CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY:
				{
					ConstraintTeacherMaxGapsPerDay* ctr=(ConstraintTeacherMaxGapsPerDay*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxGaps);

					break;
				}
			//15
			case CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
				{
					ConstraintStudentsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintStudentsEarlyMaxBeginningsAtSecondHour*)oldtc;

					spinBox->setValue(ctr->maxBeginningsAtSecondHour);

					break;
				}
			//16
			case CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
				{
					ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxBeginningsAtSecondHour);

					break;
				}
			//17
			case CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES:
				{
					ConstraintStudentsSetNotAvailableTimes* ctr=(ConstraintStudentsSetNotAvailableTimes*)oldtc;

					fillTimesTable(timesTable, ctr->days, ctr->hours, true);

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					break;
				}
			//18
			case CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK:
				{
					ConstraintStudentsMaxGapsPerWeek* ctr=(ConstraintStudentsMaxGapsPerWeek*)oldtc;

					spinBox->setValue(ctr->maxGaps);

					break;
				}
			//19
			case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK:
				{
					ConstraintStudentsSetMaxGapsPerWeek* ctr=(ConstraintStudentsSetMaxGapsPerWeek*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxGaps);

					break;
				}
			//20
			case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY:
				{
					ConstraintStudentsMaxHoursDaily* ctr=(ConstraintStudentsMaxHoursDaily*)oldtc;

					spinBox->setValue(ctr->maxHoursDaily);

					break;
				}
			//21
			case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY:
				{
					ConstraintStudentsSetMaxHoursDaily* ctr=(ConstraintStudentsSetMaxHoursDaily*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxHoursDaily);

					break;
				}
			//22
			case CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY:
				{
					ConstraintStudentsMaxHoursContinuously* ctr=(ConstraintStudentsMaxHoursContinuously*)oldtc;

					spinBox->setValue(ctr->maxHoursContinuously);

					break;
				}
			//23
			case CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY:
				{
					ConstraintStudentsSetMaxHoursContinuously* ctr=(ConstraintStudentsSetMaxHoursContinuously*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxHoursContinuously);

					break;
				}
			//24
			case CONSTRAINT_STUDENTS_MIN_HOURS_DAILY:
				{
					ConstraintStudentsMinHoursDaily* ctr=(ConstraintStudentsMinHoursDaily*)oldtc;

					spinBox->setValue(ctr->minHoursDaily);
					checkBox->setChecked(ctr->allowEmptyDays);

					break;
				}
			//25
			case CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY:
				{
					ConstraintStudentsSetMinHoursDaily* ctr=(ConstraintStudentsSetMinHoursDaily*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->minHoursDaily);
					checkBox->setChecked(ctr->allowEmptyDays);

					break;
				}
			//26
			case CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY:
				{
					ConstraintActivityEndsStudentsDay* ctr=(ConstraintActivityEndsStudentsDay*)oldtc;

					initialActivityId=ctr->activityId;

					break;
				}
			//27
			case CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME:
				{
					ConstraintActivityPreferredStartingTime* ctr=(ConstraintActivityPreferredStartingTime*)oldtc;

					initialActivityId=ctr->activityId;
					daysComboBox->setCurrentIndex(ctr->day);
					hoursComboBox->setCurrentIndex(ctr->hour);
					permanentlyLockedCheckBox->setChecked(ctr->permanentlyLocked);

					break;
				}
			//28
			case CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME:
				{
					ConstraintActivitiesSameStartingTime* ctr=(ConstraintActivitiesSameStartingTime*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int i=0; i<ctr->n_activities; i++){
						int actId=ctr->activitiesIds[i];
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					break;
				}
			//29
			case CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING:
				{
					ConstraintActivitiesNotOverlapping* ctr=(ConstraintActivitiesNotOverlapping*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int i=0; i<ctr->n_activities; i++){
						int actId=ctr->activitiesIds[i];
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					break;
				}
			//30
			case CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES:
				{
					ConstraintMinDaysBetweenActivities* ctr=(ConstraintMinDaysBetweenActivities*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int i=0; i<ctr->n_activities; i++){
						int actId=ctr->activitiesIds[i];
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					spinBox->setValue(ctr->minDays);
					checkBox->setChecked(ctr->consecutiveIfSameDay);

					break;
				}
			//31
			case CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS:
				{
					ConstraintActivityPreferredTimeSlots* ctr=(ConstraintActivityPreferredTimeSlots*)oldtc;

					initialActivityId=ctr->p_activityId;

					fillTimesTable(timesTable, ctr->p_days_L, ctr->p_hours_L, false);

					break;
				}
			//32
			case CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS:
				{
					ConstraintActivitiesPreferredTimeSlots* ctr=(ConstraintActivitiesPreferredTimeSlots*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->p_teacherName));
					int j=studentsComboBox->findText(ctr->p_studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->p_studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);
					subjectsComboBox->setCurrentIndex(subjectsComboBox->findText(ctr->p_subjectName));
					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->p_activityTagName));

					fillTimesTable(timesTable, ctr->p_days_L, ctr->p_hours_L, false);

					durationCheckBox->setChecked(ctr->duration>=1);
					durationSpinBox->setEnabled(ctr->duration>=1);
					if(ctr->duration>=1)
						durationSpinBox->setValue(ctr->duration);
					else
						durationSpinBox->setValue(1);

					break;
				}
			//33
			case CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES:
				{
					ConstraintActivityPreferredStartingTimes* ctr=(ConstraintActivityPreferredStartingTimes*)oldtc;

					initialActivityId=ctr->activityId;

					fillTimesTable(timesTable, ctr->days_L, ctr->hours_L, false);

					break;
				}
			//34
			case CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES:
				{
					ConstraintActivitiesPreferredStartingTimes* ctr=(ConstraintActivitiesPreferredStartingTimes*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);
					subjectsComboBox->setCurrentIndex(subjectsComboBox->findText(ctr->subjectName));
					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));

					fillTimesTable(timesTable, ctr->days_L, ctr->hours_L, false);

					durationCheckBox->setChecked(ctr->duration>=1);
					durationSpinBox->setEnabled(ctr->duration>=1);
					if(ctr->duration>=1)
						durationSpinBox->setValue(ctr->duration);
					else
						durationSpinBox->setValue(1);

					break;
				}
			//35
			case CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR:
				{
					ConstraintActivitiesSameStartingHour* ctr=(ConstraintActivitiesSameStartingHour*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int i=0; i<ctr->n_activities; i++){
						int actId=ctr->activitiesIds[i];
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					break;
				}
			//36
			case CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY:
				{
					ConstraintActivitiesSameStartingDay* ctr=(ConstraintActivitiesSameStartingDay*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int i=0; i<ctr->n_activities; i++){
						int actId=ctr->activitiesIds[i];
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					break;
				}
			//37
			case CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE:
				{
					ConstraintTwoActivitiesConsecutive* ctr=(ConstraintTwoActivitiesConsecutive*)oldtc;

					first_initialActivityId=ctr->firstActivityId;
					second_initialActivityId=ctr->secondActivityId;

					break;
				}
			//38
			case CONSTRAINT_TWO_ACTIVITIES_ORDERED:
				{
					ConstraintTwoActivitiesOrdered* ctr=(ConstraintTwoActivitiesOrdered*)oldtc;

					first_initialActivityId=ctr->firstActivityId;
					second_initialActivityId=ctr->secondActivityId;

					break;
				}
			//39
			case CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES:
				{
					ConstraintMinGapsBetweenActivities* ctr=(ConstraintMinGapsBetweenActivities*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int i=0; i<ctr->n_activities; i++){
						int actId=ctr->activitiesIds[i];
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					spinBox->setValue(ctr->minGaps);

					break;
				}
			//40
			case CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS:
				{
					ConstraintSubactivitiesPreferredTimeSlots* ctr=(ConstraintSubactivitiesPreferredTimeSlots*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->p_teacherName));
					int j=studentsComboBox->findText(ctr->p_studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->p_studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);
					subjectsComboBox->setCurrentIndex(subjectsComboBox->findText(ctr->p_subjectName));
					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->p_activityTagName));

					fillTimesTable(timesTable, ctr->p_days_L, ctr->p_hours_L, false);

					durationCheckBox->setChecked(ctr->duration>=1);
					durationSpinBox->setEnabled(ctr->duration>=1);
					if(ctr->duration>=1)
						durationSpinBox->setValue(ctr->duration);
					else
						durationSpinBox->setValue(1);

					splitIndexSpinBox->setValue(ctr->componentNumber);

					break;
				}
			//41
			case CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES:
				{
					ConstraintSubactivitiesPreferredStartingTimes* ctr=(ConstraintSubactivitiesPreferredStartingTimes*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);
					subjectsComboBox->setCurrentIndex(subjectsComboBox->findText(ctr->subjectName));
					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));

					fillTimesTable(timesTable, ctr->days_L, ctr->hours_L, false);

					durationCheckBox->setChecked(ctr->duration>=1);
					durationSpinBox->setEnabled(ctr->duration>=1);
					if(ctr->duration>=1)
						durationSpinBox->setValue(ctr->duration);
					else
						durationSpinBox->setValue(1);

					splitIndexSpinBox->setValue(ctr->componentNumber);

					break;
				}
			//42
			case CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK:
				{
					ConstraintTeacherIntervalMaxDaysPerWeek* ctr=(ConstraintTeacherIntervalMaxDaysPerWeek*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxDaysPerWeek);
					intervalStartHourComboBox->setCurrentIndex(ctr->startHour);
					intervalEndHourComboBox->setCurrentIndex(ctr->endHour);

					break;
				}
			//43
			case CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK:
				{
					ConstraintTeachersIntervalMaxDaysPerWeek* ctr=(ConstraintTeachersIntervalMaxDaysPerWeek*)oldtc;

					spinBox->setValue(ctr->maxDaysPerWeek);
					intervalStartHourComboBox->setCurrentIndex(ctr->startHour);
					intervalEndHourComboBox->setCurrentIndex(ctr->endHour);

					break;
				}
			//44
			case CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK:
				{
					ConstraintStudentsSetIntervalMaxDaysPerWeek* ctr=(ConstraintStudentsSetIntervalMaxDaysPerWeek*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxDaysPerWeek);
					intervalStartHourComboBox->setCurrentIndex(ctr->startHour);
					intervalEndHourComboBox->setCurrentIndex(ctr->endHour);

					break;
				}
			//45
			case CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK:
				{
					ConstraintStudentsIntervalMaxDaysPerWeek* ctr=(ConstraintStudentsIntervalMaxDaysPerWeek*)oldtc;

					spinBox->setValue(ctr->maxDaysPerWeek);
					intervalStartHourComboBox->setCurrentIndex(ctr->startHour);
					intervalEndHourComboBox->setCurrentIndex(ctr->endHour);

					break;
				}
			//46
			case CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY:
				{
					ConstraintActivitiesEndStudentsDay* ctr=(ConstraintActivitiesEndStudentsDay*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);
					subjectsComboBox->setCurrentIndex(subjectsComboBox->findText(ctr->subjectName));
					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));

					break;
				}
			//47
			case CONSTRAINT_TWO_ACTIVITIES_GROUPED:
				{
					ConstraintTwoActivitiesGrouped* ctr=(ConstraintTwoActivitiesGrouped*)oldtc;

					first_initialActivityId=ctr->firstActivityId;
					second_initialActivityId=ctr->secondActivityId;

					break;
				}
			//48
			case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
				{
					ConstraintTeachersActivityTagMaxHoursContinuously* ctr=(ConstraintTeachersActivityTagMaxHoursContinuously*)oldtc;

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));
					spinBox->setValue(ctr->maxHoursContinuously);

					break;
				}
			//49
			case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
				{
					ConstraintTeacherActivityTagMaxHoursContinuously* ctr=(ConstraintTeacherActivityTagMaxHoursContinuously*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));
					spinBox->setValue(ctr->maxHoursContinuously);

					break;
				}
			//50
			case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
				{
					ConstraintStudentsActivityTagMaxHoursContinuously* ctr=(ConstraintStudentsActivityTagMaxHoursContinuously*)oldtc;

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));
					spinBox->setValue(ctr->maxHoursContinuously);

					break;
				}
			//51
			case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
				{
					ConstraintStudentsSetActivityTagMaxHoursContinuously* ctr=(ConstraintStudentsSetActivityTagMaxHoursContinuously*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));
					spinBox->setValue(ctr->maxHoursContinuously);

					break;
				}
			//52
			case CONSTRAINT_TEACHERS_MAX_DAYS_PER_WEEK:
				{
					ConstraintTeachersMaxDaysPerWeek* ctr=(ConstraintTeachersMaxDaysPerWeek*)oldtc;

					spinBox->setValue(ctr->maxDaysPerWeek);

					break;
				}
			//53
			case CONSTRAINT_THREE_ACTIVITIES_GROUPED:
				{
					ConstraintThreeActivitiesGrouped* ctr=(ConstraintThreeActivitiesGrouped*)oldtc;

					first_initialActivityId=ctr->firstActivityId;
					second_initialActivityId=ctr->secondActivityId;
					third_initialActivityId=ctr->thirdActivityId;

					break;
				}
			//54
			case CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES:
				{
					ConstraintMaxDaysBetweenActivities* ctr=(ConstraintMaxDaysBetweenActivities*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int i=0; i<ctr->n_activities; i++){
						int actId=ctr->activitiesIds[i];
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					spinBox->setValue(ctr->maxDays);

					break;
				}
			//55
			case CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK:
				{
					ConstraintTeachersMinDaysPerWeek* ctr=(ConstraintTeachersMinDaysPerWeek*)oldtc;

					spinBox->setValue(ctr->minDaysPerWeek);

					break;
				}
			//56
			case CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK:
				{
					ConstraintTeacherMinDaysPerWeek* ctr=(ConstraintTeacherMinDaysPerWeek*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->minDaysPerWeek);

					break;
				}
			//57
			case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY:
				{
					ConstraintTeachersActivityTagMaxHoursDaily* ctr=(ConstraintTeachersActivityTagMaxHoursDaily*)oldtc;

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));
					spinBox->setValue(ctr->maxHoursDaily);

					break;
				}
			//58
			case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY:
				{
					ConstraintTeacherActivityTagMaxHoursDaily* ctr=(ConstraintTeacherActivityTagMaxHoursDaily*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));
					spinBox->setValue(ctr->maxHoursDaily);

					break;
				}
			//59
			case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY:
				{
					ConstraintStudentsActivityTagMaxHoursDaily* ctr=(ConstraintStudentsActivityTagMaxHoursDaily*)oldtc;

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));
					spinBox->setValue(ctr->maxHoursDaily);

					break;
				}
			//60
			case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY:
				{
					ConstraintStudentsSetActivityTagMaxHoursDaily* ctr=(ConstraintStudentsSetActivityTagMaxHoursDaily*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));
					spinBox->setValue(ctr->maxHoursDaily);

					break;
				}
			//61
			case CONSTRAINT_STUDENTS_MAX_GAPS_PER_DAY:
				{
					ConstraintStudentsMaxGapsPerDay* ctr=(ConstraintStudentsMaxGapsPerDay*)oldtc;

					spinBox->setValue(ctr->maxGaps);

					break;
				}
			//62
			case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY:
				{
					ConstraintStudentsSetMaxGapsPerDay* ctr=(ConstraintStudentsSetMaxGapsPerDay*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxGaps);

					break;
				}
			//63
			case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TIME_SLOTS_FROM_SELECTION:
				{
					ConstraintActivitiesOccupyMaxTimeSlotsFromSelection* ctr=(ConstraintActivitiesOccupyMaxTimeSlotsFromSelection*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int actId : std::as_const(ctr->activitiesIds)){
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					fillTimesTable(timesTable, ctr->selectedDays, ctr->selectedHours, true);

					spinBox->setValue(ctr->maxOccupiedTimeSlots);

					break;
				}
			//64
			case CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS:
				{
					ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots* ctr=(ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int actId : std::as_const(ctr->activitiesIds)){
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					fillTimesTable(timesTable, ctr->selectedDays, ctr->selectedHours, true);

					spinBox->setValue(ctr->maxSimultaneous);

					break;
				}
			//65
			case CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK:
				{
					ConstraintStudentsSetMaxDaysPerWeek* ctr=(ConstraintStudentsSetMaxDaysPerWeek*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxDaysPerWeek);

					break;
				}
			//66
			case CONSTRAINT_STUDENTS_MAX_DAYS_PER_WEEK:
				{
					ConstraintStudentsMaxDaysPerWeek* ctr=(ConstraintStudentsMaxDaysPerWeek*)oldtc;

					spinBox->setValue(ctr->maxDaysPerWeek);

					break;
				}
			//67
			case CONSTRAINT_TEACHER_MAX_SPAN_PER_DAY:
				{
					ConstraintTeacherMaxSpanPerDay* ctr=(ConstraintTeacherMaxSpanPerDay*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxSpanPerDay);
					checkBox->setChecked(ctr->allowOneDayExceptionPlusOne);

					break;
				}
			//68
			case CONSTRAINT_TEACHERS_MAX_SPAN_PER_DAY:
				{
					ConstraintTeachersMaxSpanPerDay* ctr=(ConstraintTeachersMaxSpanPerDay*)oldtc;

					spinBox->setValue(ctr->maxSpanPerDay);
					checkBox->setChecked(ctr->allowOneDayExceptionPlusOne);

					break;
				}
			//69
			case CONSTRAINT_TEACHER_MIN_RESTING_HOURS:
				{
					ConstraintTeacherMinRestingHours* ctr=(ConstraintTeacherMinRestingHours*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->minRestingHours);
					checkBox->setChecked(ctr->circular);

					break;
				}
			//70
			case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS:
				{
					ConstraintTeachersMinRestingHours* ctr=(ConstraintTeachersMinRestingHours*)oldtc;

					spinBox->setValue(ctr->minRestingHours);
					checkBox->setChecked(ctr->circular);

					break;
				}
			//71
			case CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_DAY:
				{
					ConstraintStudentsSetMaxSpanPerDay* ctr=(ConstraintStudentsSetMaxSpanPerDay*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxSpanPerDay);

					break;
				}
			//72
			case CONSTRAINT_STUDENTS_MAX_SPAN_PER_DAY:
				{
					ConstraintStudentsMaxSpanPerDay* ctr=(ConstraintStudentsMaxSpanPerDay*)oldtc;

					spinBox->setValue(ctr->maxSpanPerDay);

					break;
				}
			//73
			case CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS:
				{
					ConstraintStudentsSetMinRestingHours* ctr=(ConstraintStudentsSetMinRestingHours*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->minRestingHours);
					checkBox->setChecked(ctr->circular);

					break;
				}
			//74
			case CONSTRAINT_STUDENTS_MIN_RESTING_HOURS:
				{
					ConstraintStudentsMinRestingHours* ctr=(ConstraintStudentsMinRestingHours*)oldtc;

					spinBox->setValue(ctr->minRestingHours);
					checkBox->setChecked(ctr->circular);

					break;
				}
			//75
			case CONSTRAINT_TWO_ACTIVITIES_ORDERED_IF_SAME_DAY:
				{
					ConstraintTwoActivitiesOrderedIfSameDay* ctr=(ConstraintTwoActivitiesOrderedIfSameDay*)oldtc;

					first_initialActivityId=ctr->firstActivityId;
					second_initialActivityId=ctr->secondActivityId;

					break;
				}
			//76
			case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
				{
					ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags* ctr=(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					first_activityTagsComboBox->setCurrentIndex(first_activityTagsComboBox->findText(ctr->firstActivityTag));
					second_activityTagsComboBox->setCurrentIndex(second_activityTagsComboBox->findText(ctr->secondActivityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//77
			case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
				{
					ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags* ctr=(ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags*)oldtc;

					first_activityTagsComboBox->setCurrentIndex(first_activityTagsComboBox->findText(ctr->firstActivityTag));
					second_activityTagsComboBox->setCurrentIndex(second_activityTagsComboBox->findText(ctr->secondActivityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//78
			case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
				{
					ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags* ctr=(ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacher));

					first_activityTagsComboBox->setCurrentIndex(first_activityTagsComboBox->findText(ctr->firstActivityTag));
					second_activityTagsComboBox->setCurrentIndex(second_activityTagsComboBox->findText(ctr->secondActivityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//79
			case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
				{
					ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags* ctr=(ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags*)oldtc;

					first_activityTagsComboBox->setCurrentIndex(first_activityTagsComboBox->findText(ctr->firstActivityTag));
					second_activityTagsComboBox->setCurrentIndex(second_activityTagsComboBox->findText(ctr->secondActivityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//80
			case CONSTRAINT_ACTIVITY_TAGS_NOT_OVERLAPPING:
				{
					ConstraintActivityTagsNotOverlapping* ctr=(ConstraintActivityTagsNotOverlapping*)oldtc;

					selectedActivityTagsListWidget->clear();
					selectedActivityTagsSet.clear();

					for(const QString& at : std::as_const(ctr->activityTagsNames)){
						selectedActivityTagsSet.insert(at);
						selectedActivityTagsListWidget->addItem(at);
					}

					break;
				}
			//81
			case CONSTRAINT_ACTIVITIES_OCCUPY_MIN_TIME_SLOTS_FROM_SELECTION:
				{
					ConstraintActivitiesOccupyMinTimeSlotsFromSelection* ctr=(ConstraintActivitiesOccupyMinTimeSlotsFromSelection*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int actId : std::as_const(ctr->activitiesIds)){
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					fillTimesTable(timesTable, ctr->selectedDays, ctr->selectedHours, true);

					spinBox->setValue(ctr->minOccupiedTimeSlots);

					break;
				}
			//82
			case CONSTRAINT_ACTIVITIES_MIN_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS:
				{
					ConstraintActivitiesMinSimultaneousInSelectedTimeSlots* ctr=(ConstraintActivitiesMinSimultaneousInSelectedTimeSlots*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int actId : std::as_const(ctr->activitiesIds)){
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					fillTimesTable(timesTable, ctr->selectedDays, ctr->selectedHours, true);

					spinBox->setValue(ctr->minSimultaneous);
					allowEmptySlotsCheckBox->setChecked(ctr->allowEmptySlots);

					break;
				}
			//83
			case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MIN_HOURS_DAILY:
				{
					ConstraintTeachersActivityTagMinHoursDaily* ctr=(ConstraintTeachersActivityTagMinHoursDaily*)oldtc;

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));
					spinBox->setValue(ctr->minHoursDaily);
					secondSpinBox->setValue(ctr->minDaysWithTag);
					//checkBox->setChecked(ctr->allowEmptyDays);

					break;
				}
			//84
			case CONSTRAINT_TEACHER_ACTIVITY_TAG_MIN_HOURS_DAILY:
				{
					ConstraintTeacherActivityTagMinHoursDaily* ctr=(ConstraintTeacherActivityTagMinHoursDaily*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));
					spinBox->setValue(ctr->minHoursDaily);
					secondSpinBox->setValue(ctr->minDaysWithTag);
					//checkBox->setChecked(ctr->allowEmptyDays);

					break;
				}
			//85
			case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MIN_HOURS_DAILY:
				{
					ConstraintStudentsActivityTagMinHoursDaily* ctr=(ConstraintStudentsActivityTagMinHoursDaily*)oldtc;

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));
					spinBox->setValue(ctr->minHoursDaily);
					secondSpinBox->setValue(ctr->minDaysWithTag);
					//checkBox->setChecked(ctr->allowEmptyDays);

					break;
				}
			//86
			case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MIN_HOURS_DAILY:
				{
					ConstraintStudentsSetActivityTagMinHoursDaily* ctr=(ConstraintStudentsSetActivityTagMinHoursDaily*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));
					spinBox->setValue(ctr->minHoursDaily);
					secondSpinBox->setValue(ctr->minDaysWithTag);
					//checkBox->setChecked(ctr->allowEmptyDays);

					break;
				}
			//87
			case CONSTRAINT_ACTIVITY_ENDS_TEACHERS_DAY:
				{
					ConstraintActivityEndsTeachersDay* ctr=(ConstraintActivityEndsTeachersDay*)oldtc;

					initialActivityId=ctr->activityId;

					break;
				}
			//88
			case CONSTRAINT_ACTIVITIES_END_TEACHERS_DAY:
				{
					ConstraintActivitiesEndTeachersDay* ctr=(ConstraintActivitiesEndTeachersDay*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);
					subjectsComboBox->setCurrentIndex(subjectsComboBox->findText(ctr->subjectName));
					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));

					break;
				}
			//89
			case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_REAL_DAYS:
				{
					ConstraintTeachersMaxHoursDailyRealDays* ctr=(ConstraintTeachersMaxHoursDailyRealDays*)oldtc;

					spinBox->setValue(ctr->maxHoursDaily);

					break;
				}
			//90
			case CONSTRAINT_TEACHER_MAX_REAL_DAYS_PER_WEEK:
				{
					ConstraintTeacherMaxRealDaysPerWeek* ctr=(ConstraintTeacherMaxRealDaysPerWeek*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxDaysPerWeek);

					break;
				}
			//91
			case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS:
				{
					ConstraintTeacherMaxHoursDailyRealDays* ctr=(ConstraintTeacherMaxHoursDailyRealDays*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxHoursDaily);

					break;
				}
			//92
			case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_REAL_DAYS:
				{
					ConstraintStudentsMaxHoursDailyRealDays* ctr=(ConstraintStudentsMaxHoursDailyRealDays*)oldtc;

					spinBox->setValue(ctr->maxHoursDaily);

					break;
				}
			//93
			case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_REAL_DAYS:
				{
					ConstraintStudentsSetMaxHoursDailyRealDays* ctr=(ConstraintStudentsSetMaxHoursDailyRealDays*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxHoursDaily);

					break;
				}
			//94
			case CONSTRAINT_TEACHERS_MAX_REAL_DAYS_PER_WEEK:
				{
					ConstraintTeachersMaxRealDaysPerWeek* ctr=(ConstraintTeachersMaxRealDaysPerWeek*)oldtc;

					spinBox->setValue(ctr->maxDaysPerWeek);

					break;
				}
			//95
			case CONSTRAINT_TEACHERS_MIN_REAL_DAYS_PER_WEEK:
				{
					ConstraintTeachersMinRealDaysPerWeek* ctr=(ConstraintTeachersMinRealDaysPerWeek*)oldtc;

					spinBox->setValue(ctr->minDaysPerWeek);

					break;
				}
			//96
			case CONSTRAINT_TEACHER_MIN_REAL_DAYS_PER_WEEK:
				{
					ConstraintTeacherMinRealDaysPerWeek* ctr=(ConstraintTeacherMinRealDaysPerWeek*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->minDaysPerWeek);

					break;
				}
			//97
			case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
				{
					ConstraintTeachersActivityTagMaxHoursDailyRealDays* ctr=(ConstraintTeachersActivityTagMaxHoursDailyRealDays*)oldtc;

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));
					spinBox->setValue(ctr->maxHoursDaily);

					break;
				}
			//98
			case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
				{
					ConstraintTeacherActivityTagMaxHoursDailyRealDays* ctr=(ConstraintTeacherActivityTagMaxHoursDailyRealDays*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));
					spinBox->setValue(ctr->maxHoursDaily);

					break;
				}
			//99
			case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
				{
					ConstraintStudentsActivityTagMaxHoursDailyRealDays* ctr=(ConstraintStudentsActivityTagMaxHoursDailyRealDays*)oldtc;

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));
					spinBox->setValue(ctr->maxHoursDaily);

					break;
				}
			//100
			case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
				{
					ConstraintStudentsSetActivityTagMaxHoursDailyRealDays* ctr=(ConstraintStudentsSetActivityTagMaxHoursDailyRealDays*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));
					spinBox->setValue(ctr->maxHoursDaily);

					break;
				}
			//101
			case CONSTRAINT_TEACHER_MAX_AFTERNOONS_PER_WEEK:
				{
					ConstraintTeacherMaxAfternoonsPerWeek* ctr=(ConstraintTeacherMaxAfternoonsPerWeek*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxAfternoonsPerWeek);

					break;
				}
			//102
			case CONSTRAINT_TEACHERS_MAX_AFTERNOONS_PER_WEEK:
				{
					ConstraintTeachersMaxAfternoonsPerWeek* ctr=(ConstraintTeachersMaxAfternoonsPerWeek*)oldtc;

					spinBox->setValue(ctr->maxAfternoonsPerWeek);

					break;
				}
			//103
			case CONSTRAINT_TEACHER_MAX_MORNINGS_PER_WEEK:
				{
					ConstraintTeacherMaxMorningsPerWeek* ctr=(ConstraintTeacherMaxMorningsPerWeek*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxMorningsPerWeek);

					break;
				}
			//104
			case CONSTRAINT_TEACHERS_MAX_MORNINGS_PER_WEEK:
				{
					ConstraintTeachersMaxMorningsPerWeek* ctr=(ConstraintTeachersMaxMorningsPerWeek*)oldtc;

					spinBox->setValue(ctr->maxMorningsPerWeek);

					break;
				}
			//105
			case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
				{
					ConstraintTeacherMaxActivityTagsPerDayFromSet* ctr=(ConstraintTeacherMaxActivityTagsPerDayFromSet*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					selectedActivityTagsListWidget->clear();
					selectedActivityTagsSet.clear();

					for(const QString& at : std::as_const(ctr->tagsList)){
						selectedActivityTagsSet.insert(at);
						selectedActivityTagsListWidget->addItem(at);
					}

					spinBox->setValue(ctr->maxTags);

					break;
				}
			//106
			case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
				{
					ConstraintTeachersMaxActivityTagsPerDayFromSet* ctr=(ConstraintTeachersMaxActivityTagsPerDayFromSet*)oldtc;

					selectedActivityTagsListWidget->clear();
					selectedActivityTagsSet.clear();

					for(const QString& at : std::as_const(ctr->tagsList)){
						selectedActivityTagsSet.insert(at);
						selectedActivityTagsListWidget->addItem(at);
					}

					spinBox->setValue(ctr->maxTags);

					break;
				}
			//107
			case CONSTRAINT_TEACHERS_MIN_MORNINGS_PER_WEEK:
				{
					ConstraintTeachersMinMorningsPerWeek* ctr=(ConstraintTeachersMinMorningsPerWeek*)oldtc;

					spinBox->setValue(ctr->minMorningsPerWeek);

					break;
				}
			//108
			case CONSTRAINT_TEACHER_MIN_MORNINGS_PER_WEEK:
				{
					ConstraintTeacherMinMorningsPerWeek* ctr=(ConstraintTeacherMinMorningsPerWeek*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->minMorningsPerWeek);

					break;
				}
			//109
			case CONSTRAINT_TEACHERS_MIN_AFTERNOONS_PER_WEEK:
				{
					ConstraintTeachersMinAfternoonsPerWeek* ctr=(ConstraintTeachersMinAfternoonsPerWeek*)oldtc;

					spinBox->setValue(ctr->minAfternoonsPerWeek);

					break;
				}
			//110
			case CONSTRAINT_TEACHER_MIN_AFTERNOONS_PER_WEEK:
				{
					ConstraintTeacherMinAfternoonsPerWeek* ctr=(ConstraintTeacherMinAfternoonsPerWeek*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->minAfternoonsPerWeek);

					break;
				}
			//111
			case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_MORNINGS:
				{
					ConstraintTeacherMaxTwoConsecutiveMornings* ctr=(ConstraintTeacherMaxTwoConsecutiveMornings*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					break;
				}
			//112
			case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_MORNINGS:
				{
					//ConstraintTeachersMaxTwoConsecutiveMornings* ctr=(ConstraintTeachersMaxTwoConsecutiveMornings*)oldtc;

					//nothing to be done

					break;
				}
			//113
			case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_AFTERNOONS:
				{
					ConstraintTeacherMaxTwoConsecutiveAfternoons* ctr=(ConstraintTeacherMaxTwoConsecutiveAfternoons*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					break;
				}
			//114
			case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_AFTERNOONS:
				{
					//ConstraintTeachersMaxTwoConsecutiveAfternoons* ctr=(ConstraintTeachersMaxTwoConsecutiveAfternoons*)oldtc;

					//nothing to be done

					break;
				}
			//115
			case CONSTRAINT_TEACHERS_MAX_GAPS_PER_REAL_DAY:
				{
					ConstraintTeachersMaxGapsPerRealDay* ctr=(ConstraintTeachersMaxGapsPerRealDay*)oldtc;

					spinBox->setValue(ctr->maxGaps);

					checkBox->setChecked(ctr->allowOneDayExceptionPlusOne);

					break;
				}
			//116
			case CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY:
				{
					ConstraintTeacherMaxGapsPerRealDay* ctr=(ConstraintTeacherMaxGapsPerRealDay*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxGaps);

					checkBox->setChecked(ctr->allowOneDayExceptionPlusOne);

					break;
				}
			//117
			case CONSTRAINT_STUDENTS_MAX_GAPS_PER_REAL_DAY:
				{
					ConstraintStudentsMaxGapsPerRealDay* ctr=(ConstraintStudentsMaxGapsPerRealDay*)oldtc;

					spinBox->setValue(ctr->maxGaps);

					break;
				}
			//118
			case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_REAL_DAY:
				{
					ConstraintStudentsSetMaxGapsPerRealDay* ctr=(ConstraintStudentsSetMaxGapsPerRealDay*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxGaps);

					break;
				}
			//119
			case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY_REAL_DAYS:
				{
					ConstraintTeachersMinHoursDailyRealDays* ctr=(ConstraintTeachersMinHoursDailyRealDays*)oldtc;

					spinBox->setValue(ctr->minHoursDaily);
					checkBox->setChecked(ctr->allowEmptyDays);

					break;
				}
			//120
			case CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS:
				{
					ConstraintTeacherMinHoursDailyRealDays* ctr=(ConstraintTeacherMinHoursDailyRealDays*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->minHoursDaily);
					checkBox->setChecked(ctr->allowEmptyDays);

					break;
				}
			//121
			case CONSTRAINT_TEACHERS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
				{
					ConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHour*)oldtc;

					spinBox->setValue(ctr->maxBeginningsAtSecondHour);

					break;
				}
			//122
			case CONSTRAINT_TEACHER_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
				{
					ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->maxBeginningsAtSecondHour);

					break;
				}
			//123
			case CONSTRAINT_TEACHERS_MIN_HOURS_PER_MORNING:
				{
					ConstraintTeachersMinHoursPerMorning* ctr=(ConstraintTeachersMinHoursPerMorning*)oldtc;

					spinBox->setValue(ctr->minHoursPerMorning);
					checkBox->setChecked(ctr->allowEmptyMornings);

					break;
				}
			//124
			case CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING:
				{
					ConstraintTeacherMinHoursPerMorning* ctr=(ConstraintTeacherMinHoursPerMorning*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->minHoursPerMorning);
					checkBox->setChecked(ctr->allowEmptyMornings);

					break;
				}
			//125
			case CONSTRAINT_TEACHER_MAX_SPAN_PER_REAL_DAY:
				{
					ConstraintTeacherMaxSpanPerRealDay* ctr=(ConstraintTeacherMaxSpanPerRealDay*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxSpanPerDay);
					checkBox->setChecked(ctr->allowOneDayExceptionPlusOne);

					break;
				}
			//126
			case CONSTRAINT_TEACHERS_MAX_SPAN_PER_REAL_DAY:
				{
					ConstraintTeachersMaxSpanPerRealDay* ctr=(ConstraintTeachersMaxSpanPerRealDay*)oldtc;

					spinBox->setValue(ctr->maxSpanPerDay);
					checkBox->setChecked(ctr->allowOneDayExceptionPlusOne);

					break;
				}
			//127
			case CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_REAL_DAY:
				{
					ConstraintStudentsSetMaxSpanPerRealDay* ctr=(ConstraintStudentsSetMaxSpanPerRealDay*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxSpanPerDay);

					break;
				}
			//128
			case CONSTRAINT_STUDENTS_MAX_SPAN_PER_REAL_DAY:
				{
					ConstraintStudentsMaxSpanPerRealDay* ctr=(ConstraintStudentsMaxSpanPerRealDay*)oldtc;

					spinBox->setValue(ctr->maxSpanPerDay);

					break;
				}
			//129
			case CONSTRAINT_TEACHER_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
				{
					ConstraintTeacherMorningIntervalMaxDaysPerWeek* ctr=(ConstraintTeacherMorningIntervalMaxDaysPerWeek*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxDaysPerWeek);
					intervalStartHourComboBox->setCurrentIndex(ctr->startHour);
					intervalEndHourComboBox->setCurrentIndex(ctr->endHour);

					break;
				}
			//130
			case CONSTRAINT_TEACHERS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
				{
					ConstraintTeachersMorningIntervalMaxDaysPerWeek* ctr=(ConstraintTeachersMorningIntervalMaxDaysPerWeek*)oldtc;

					spinBox->setValue(ctr->maxDaysPerWeek);
					intervalStartHourComboBox->setCurrentIndex(ctr->startHour);
					intervalEndHourComboBox->setCurrentIndex(ctr->endHour);

					break;
				}
			//131
			case CONSTRAINT_TEACHER_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
				{
					ConstraintTeacherAfternoonIntervalMaxDaysPerWeek* ctr=(ConstraintTeacherAfternoonIntervalMaxDaysPerWeek*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxDaysPerWeek);
					intervalStartHourComboBox->setCurrentIndex(ctr->startHour);
					intervalEndHourComboBox->setCurrentIndex(ctr->endHour);

					break;
				}
			//132
			case CONSTRAINT_TEACHERS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
				{
					ConstraintTeachersAfternoonIntervalMaxDaysPerWeek* ctr=(ConstraintTeachersAfternoonIntervalMaxDaysPerWeek*)oldtc;

					spinBox->setValue(ctr->maxDaysPerWeek);
					intervalStartHourComboBox->setCurrentIndex(ctr->startHour);
					intervalEndHourComboBox->setCurrentIndex(ctr->endHour);

					break;
				}
			//133
			case CONSTRAINT_STUDENTS_MIN_HOURS_PER_MORNING:
				{
					ConstraintStudentsMinHoursPerMorning* ctr=(ConstraintStudentsMinHoursPerMorning*)oldtc;

					spinBox->setValue(ctr->minHoursPerMorning);
					checkBox->setChecked(ctr->allowEmptyMornings);

					break;
				}
			//134
			case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_MORNING:
				{
					ConstraintStudentsSetMinHoursPerMorning* ctr=(ConstraintStudentsSetMinHoursPerMorning*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->minHoursPerMorning);
					checkBox->setChecked(ctr->allowEmptyMornings);

					break;
				}
			//135
			case CONSTRAINT_TEACHER_MAX_ZERO_GAPS_PER_AFTERNOON:
				{
					ConstraintTeacherMaxZeroGapsPerAfternoon* ctr=(ConstraintTeacherMaxZeroGapsPerAfternoon*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					break;
				}
			//136
			case CONSTRAINT_TEACHERS_MAX_ZERO_GAPS_PER_AFTERNOON:
				{
					//ConstraintTeachersMaxZeroGapsPerAfternoon* ctr=(ConstraintTeachersMaxZeroGapsPerAfternoon*)oldtc;

					//nothing to be done

					break;
				}
			//137
			case CONSTRAINT_STUDENTS_SET_MAX_AFTERNOONS_PER_WEEK:
				{
					ConstraintStudentsSetMaxAfternoonsPerWeek* ctr=(ConstraintStudentsSetMaxAfternoonsPerWeek*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxAfternoonsPerWeek);

					break;
				}
			//138
			case CONSTRAINT_STUDENTS_MAX_AFTERNOONS_PER_WEEK:
				{
					ConstraintStudentsMaxAfternoonsPerWeek* ctr=(ConstraintStudentsMaxAfternoonsPerWeek*)oldtc;

					spinBox->setValue(ctr->maxAfternoonsPerWeek);

					break;
				}
			//139
			case CONSTRAINT_STUDENTS_SET_MAX_MORNINGS_PER_WEEK:
				{
					ConstraintStudentsSetMaxMorningsPerWeek* ctr=(ConstraintStudentsSetMaxMorningsPerWeek*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxMorningsPerWeek);

					break;
				}
			//140
			case CONSTRAINT_STUDENTS_MAX_MORNINGS_PER_WEEK:
				{
					ConstraintStudentsMaxMorningsPerWeek* ctr=(ConstraintStudentsMaxMorningsPerWeek*)oldtc;

					spinBox->setValue(ctr->maxMorningsPerWeek);

					break;
				}
			//141
			case CONSTRAINT_STUDENTS_MIN_MORNINGS_PER_WEEK:
				{
					ConstraintStudentsMinMorningsPerWeek* ctr=(ConstraintStudentsMinMorningsPerWeek*)oldtc;

					spinBox->setValue(ctr->minMorningsPerWeek);

					break;
				}
			//142
			case CONSTRAINT_STUDENTS_SET_MIN_MORNINGS_PER_WEEK:
				{
					ConstraintStudentsSetMinMorningsPerWeek* ctr=(ConstraintStudentsSetMinMorningsPerWeek*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->minMorningsPerWeek);

					break;
				}
			//143
			case CONSTRAINT_STUDENTS_MIN_AFTERNOONS_PER_WEEK:
				{
					ConstraintStudentsMinAfternoonsPerWeek* ctr=(ConstraintStudentsMinAfternoonsPerWeek*)oldtc;

					spinBox->setValue(ctr->minAfternoonsPerWeek);

					break;
				}
			//144
			case CONSTRAINT_STUDENTS_SET_MIN_AFTERNOONS_PER_WEEK:
				{
					ConstraintStudentsSetMinAfternoonsPerWeek* ctr=(ConstraintStudentsSetMinAfternoonsPerWeek*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->minAfternoonsPerWeek);

					break;
				}
			//145
			case CONSTRAINT_STUDENTS_SET_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
				{
					ConstraintStudentsSetMorningIntervalMaxDaysPerWeek* ctr=(ConstraintStudentsSetMorningIntervalMaxDaysPerWeek*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxDaysPerWeek);
					intervalStartHourComboBox->setCurrentIndex(ctr->startHour);
					intervalEndHourComboBox->setCurrentIndex(ctr->endHour);

					break;
				}
			//146
			case CONSTRAINT_STUDENTS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
				{
					ConstraintStudentsMorningIntervalMaxDaysPerWeek* ctr=(ConstraintStudentsMorningIntervalMaxDaysPerWeek*)oldtc;

					spinBox->setValue(ctr->maxDaysPerWeek);
					intervalStartHourComboBox->setCurrentIndex(ctr->startHour);
					intervalEndHourComboBox->setCurrentIndex(ctr->endHour);

					break;
				}
			//147
			case CONSTRAINT_STUDENTS_SET_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
				{
					ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek* ctr=(ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxDaysPerWeek);
					intervalStartHourComboBox->setCurrentIndex(ctr->startHour);
					intervalEndHourComboBox->setCurrentIndex(ctr->endHour);

					break;
				}
			//148
			case CONSTRAINT_STUDENTS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
				{
					ConstraintStudentsAfternoonIntervalMaxDaysPerWeek* ctr=(ConstraintStudentsAfternoonIntervalMaxDaysPerWeek*)oldtc;

					spinBox->setValue(ctr->maxDaysPerWeek);
					intervalStartHourComboBox->setCurrentIndex(ctr->startHour);
					intervalEndHourComboBox->setCurrentIndex(ctr->endHour);

					break;
				}
			//149
			case CONSTRAINT_TEACHER_MAX_HOURS_PER_ALL_AFTERNOONS:
				{
					ConstraintTeacherMaxHoursPerAllAfternoons* ctr=(ConstraintTeacherMaxHoursPerAllAfternoons*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxHoursPerAllAfternoons);

					break;
				}
			//150
			case CONSTRAINT_TEACHERS_MAX_HOURS_PER_ALL_AFTERNOONS:
				{
					ConstraintTeachersMaxHoursPerAllAfternoons* ctr=(ConstraintTeachersMaxHoursPerAllAfternoons*)oldtc;

					spinBox->setValue(ctr->maxHoursPerAllAfternoons);

					break;
				}
			//151
			case CONSTRAINT_STUDENTS_SET_MAX_HOURS_PER_ALL_AFTERNOONS:
				{
					ConstraintStudentsSetMaxHoursPerAllAfternoons* ctr=(ConstraintStudentsSetMaxHoursPerAllAfternoons*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxHoursPerAllAfternoons);

					break;
				}
			//152
			case CONSTRAINT_STUDENTS_MAX_HOURS_PER_ALL_AFTERNOONS:
				{
					ConstraintStudentsMaxHoursPerAllAfternoons* ctr=(ConstraintStudentsMaxHoursPerAllAfternoons*)oldtc;

					spinBox->setValue(ctr->maxHoursPerAllAfternoons);

					break;
				}
			//153
			case CONSTRAINT_TEACHER_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
				{
					ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon* ctr=(ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->minRestingHours);

					break;
				}
			//154
			case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
				{
					ConstraintTeachersMinRestingHoursBetweenMorningAndAfternoon* ctr=(ConstraintTeachersMinRestingHoursBetweenMorningAndAfternoon*)oldtc;

					spinBox->setValue(ctr->minRestingHours);

					break;
				}
			//155
			case CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
				{
					ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon* ctr=(ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->minRestingHours);

					break;
				}
			//156
			case CONSTRAINT_STUDENTS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
				{
					ConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon* ctr=(ConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon*)oldtc;

					spinBox->setValue(ctr->minRestingHours);

					break;
				}
			//157
			case CONSTRAINT_STUDENTS_SET_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
				{
					ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxBeginningsAtSecondHour);

					break;
				}
			//158
			case CONSTRAINT_STUDENTS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
				{
					ConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHour*)oldtc;

					spinBox->setValue(ctr->maxBeginningsAtSecondHour);

					break;
				}
			//159
			case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
				{
					ConstraintTeachersMaxGapsPerWeekForRealDays* ctr=(ConstraintTeachersMaxGapsPerWeekForRealDays*)oldtc;

					spinBox->setValue(ctr->maxGaps);

					break;
				}
			//160
			case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
				{
					ConstraintTeacherMaxGapsPerWeekForRealDays* ctr=(ConstraintTeacherMaxGapsPerWeekForRealDays*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxGaps);

					break;
				}
			//161
			case CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
				{
					ConstraintStudentsMaxGapsPerWeekForRealDays* ctr=(ConstraintStudentsMaxGapsPerWeekForRealDays*)oldtc;

					spinBox->setValue(ctr->maxGaps);

					break;
				}
			//162
			case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
				{
					ConstraintStudentsSetMaxGapsPerWeekForRealDays* ctr=(ConstraintStudentsSetMaxGapsPerWeekForRealDays*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxGaps);

					break;
				}
			//163
			case CONSTRAINT_STUDENTS_SET_MAX_REAL_DAYS_PER_WEEK:
				{
					ConstraintStudentsSetMaxRealDaysPerWeek* ctr=(ConstraintStudentsSetMaxRealDaysPerWeek*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxDaysPerWeek);

					break;
				}
			//164
			case CONSTRAINT_STUDENTS_MAX_REAL_DAYS_PER_WEEK:
				{
					ConstraintStudentsMaxRealDaysPerWeek* ctr=(ConstraintStudentsMaxRealDaysPerWeek*)oldtc;

					spinBox->setValue(ctr->maxDaysPerWeek);

					break;
				}
			//165
			case CONSTRAINT_MAX_TOTAL_ACTIVITIES_FROM_SET_IN_SELECTED_TIME_SLOTS:
				{
					ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots* ctr=(ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int actId : std::as_const(ctr->activitiesIds)){
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					fillTimesTable(timesTable, ctr->selectedDays, ctr->selectedHours, true);

					spinBox->setValue(ctr->maxActivities);

					break;
				}
			//166
			case CONSTRAINT_MAX_GAPS_BETWEEN_ACTIVITIES:
				{
					ConstraintMaxGapsBetweenActivities* ctr=(ConstraintMaxGapsBetweenActivities*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int i=0; i<ctr->n_activities; i++){
						int actId=ctr->activitiesIds[i];
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					spinBox->setValue(ctr->maxGaps);

					break;
				}
			//167
			case CONSTRAINT_ACTIVITIES_MAX_IN_A_TERM:
				{
					ConstraintActivitiesMaxInATerm* ctr=(ConstraintActivitiesMaxInATerm*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int actId : std::as_const(ctr->activitiesIds)){
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					spinBox->setValue(ctr->maxActivitiesInATerm);

					break;
				}
			//168
			case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TERMS:
				{
					ConstraintActivitiesOccupyMaxTerms* ctr=(ConstraintActivitiesOccupyMaxTerms*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int actId : std::as_const(ctr->activitiesIds)){
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					spinBox->setValue(ctr->maxOccupiedTerms);

					break;
				}
			//169
			case CONSTRAINT_TEACHERS_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
				{
					ConstraintTeachersMaxGapsPerMorningAndAfternoon* ctr=(ConstraintTeachersMaxGapsPerMorningAndAfternoon*)oldtc;

					spinBox->setValue(ctr->maxGaps);

					break;
				}
			//170
			case CONSTRAINT_TEACHER_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
				{
					ConstraintTeacherMaxGapsPerMorningAndAfternoon* ctr=(ConstraintTeacherMaxGapsPerMorningAndAfternoon*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxGaps);

					break;
				}
			//171
			case CONSTRAINT_TEACHERS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
				{
					ConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHour*)oldtc;

					spinBox->setValue(ctr->maxBeginningsAtSecondHour);

					break;
				}
			//172
			case CONSTRAINT_TEACHER_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
				{
					ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->maxBeginningsAtSecondHour);

					break;
				}
			//173
			case CONSTRAINT_STUDENTS_SET_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
				{
					ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxBeginningsAtSecondHour);

					break;
				}
			//174
			case CONSTRAINT_STUDENTS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
				{
					ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour*)oldtc;

					spinBox->setValue(ctr->maxBeginningsAtSecondHour);

					break;
				}
			//175
			case CONSTRAINT_TWO_SETS_OF_ACTIVITIES_ORDERED:
				{
					ConstraintTwoSetsOfActivitiesOrdered* ctr=(ConstraintTwoSetsOfActivitiesOrdered*)oldtc;

					selectedActivitiesListWidget_TwoSetsOfActivities_1->clear();
					selectedActivitiesListWidget_TwoSetsOfActivities_2->clear();
					selectedActivitiesList_TwoSetsOfActivities_1.clear();
					selectedActivitiesList_TwoSetsOfActivities_2.clear();

					for(int actId : std::as_const(ctr->firstActivitiesIdsList)){
						selectedActivitiesList_TwoSetsOfActivities_1.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget_TwoSetsOfActivities_1->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget_TwoSetsOfActivities_1->item(selectedActivitiesListWidget_TwoSetsOfActivities_1->count()-1)->setBackground(selectedActivitiesListWidget_TwoSetsOfActivities_1->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget_TwoSetsOfActivities_1->item(selectedActivitiesListWidget_TwoSetsOfActivities_1->count()-1)->setForeground(selectedActivitiesListWidget_TwoSetsOfActivities_1->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					for(int actId : std::as_const(ctr->secondActivitiesIdsList)){
						selectedActivitiesList_TwoSetsOfActivities_2.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget_TwoSetsOfActivities_2->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget_TwoSetsOfActivities_2->item(selectedActivitiesListWidget_TwoSetsOfActivities_2->count()-1)->setBackground(selectedActivitiesListWidget_TwoSetsOfActivities_2->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget_TwoSetsOfActivities_2->item(selectedActivitiesListWidget_TwoSetsOfActivities_2->count()-1)->setForeground(selectedActivitiesListWidget_TwoSetsOfActivities_2->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					break;
				}
			//176
			case CONSTRAINT_TEACHERS_MAX_THREE_CONSECUTIVE_DAYS:
				{
					ConstraintTeachersMaxThreeConsecutiveDays* ctr=(ConstraintTeachersMaxThreeConsecutiveDays*)oldtc;

					checkBox->setChecked(ctr->allowAMAMException);

					break;
				}
			//177
			case CONSTRAINT_TEACHER_MAX_THREE_CONSECUTIVE_DAYS:
				{
					ConstraintTeacherMaxThreeConsecutiveDays* ctr=(ConstraintTeacherMaxThreeConsecutiveDays*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					checkBox->setChecked(ctr->allowAMAMException);

					break;
				}
			//178
			case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
				{
					ConstraintStudentsSetMinGapsBetweenActivityTag* ctr=(ConstraintStudentsSetMinGapsBetweenActivityTag*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//179
			case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
				{
					ConstraintStudentsMinGapsBetweenActivityTag* ctr=(ConstraintStudentsMinGapsBetweenActivityTag*)oldtc;

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//180
			case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
				{
					ConstraintTeacherMinGapsBetweenActivityTag* ctr=(ConstraintTeacherMinGapsBetweenActivityTag*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacher));

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//181
			case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
				{
					ConstraintTeachersMinGapsBetweenActivityTag* ctr=(ConstraintTeachersMinGapsBetweenActivityTag*)oldtc;

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//182
			case CONSTRAINT_STUDENTS_MAX_THREE_CONSECUTIVE_DAYS:
				{
					ConstraintStudentsMaxThreeConsecutiveDays* ctr=(ConstraintStudentsMaxThreeConsecutiveDays*)oldtc;

					checkBox->setChecked(ctr->allowAMAMException);

					break;
				}
			//183
			case CONSTRAINT_STUDENTS_SET_MAX_THREE_CONSECUTIVE_DAYS:
				{
					ConstraintStudentsSetMaxThreeConsecutiveDays* ctr=(ConstraintStudentsSetMaxThreeConsecutiveDays*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					checkBox->setChecked(ctr->allowAMAMException);

					break;
				}
			//184
			case CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES:
				{
					ConstraintMinHalfDaysBetweenActivities* ctr=(ConstraintMinHalfDaysBetweenActivities*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int i=0; i<ctr->n_activities; i++){
						int actId=ctr->activitiesIds[i];
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					spinBox->setValue(ctr->minDays);
					checkBox->setChecked(ctr->consecutiveIfSameDay);

					break;
				}
			//185 is activity preferred day, which is not in the interface
			//186
			case CONSTRAINT_ACTIVITIES_MIN_IN_A_TERM:
				{
					ConstraintActivitiesMinInATerm* ctr=(ConstraintActivitiesMinInATerm*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int actId : std::as_const(ctr->activitiesIds)){
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					spinBox->setValue(ctr->minActivitiesInATerm);
					checkBox->setChecked(ctr->allowEmptyTerms);

					break;
				}
			//187
			case CONSTRAINT_MAX_TERMS_BETWEEN_ACTIVITIES:
				{
					ConstraintMaxTermsBetweenActivities* ctr=(ConstraintMaxTermsBetweenActivities*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int i=0; i<ctr->n_activities; i++){
						int actId=ctr->activitiesIds[i];
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					spinBox->setValue(ctr->maxTerms);

					break;
				}
			//188
			case CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
				{
					ConstraintStudentsSetMaxActivityTagsPerDayFromSet* ctr=(ConstraintStudentsSetMaxActivityTagsPerDayFromSet*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					selectedActivityTagsListWidget->clear();
					selectedActivityTagsSet.clear();

					for(const QString& at : std::as_const(ctr->tagsList)){
						selectedActivityTagsSet.insert(at);
						selectedActivityTagsListWidget->addItem(at);
					}

					spinBox->setValue(ctr->maxTags);

					break;
				}
			//189
			case CONSTRAINT_STUDENTS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
				{
					ConstraintStudentsMaxActivityTagsPerDayFromSet* ctr=(ConstraintStudentsMaxActivityTagsPerDayFromSet*)oldtc;

					selectedActivityTagsListWidget->clear();
					selectedActivityTagsSet.clear();

					for(const QString& at : std::as_const(ctr->tagsList)){
						selectedActivityTagsSet.insert(at);
						selectedActivityTagsListWidget->addItem(at);
					}

					spinBox->setValue(ctr->maxTags);

					break;
				}
			//190
			case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
				{
					ConstraintTeacherMaxActivityTagsPerRealDayFromSet* ctr=(ConstraintTeacherMaxActivityTagsPerRealDayFromSet*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					selectedActivityTagsListWidget->clear();
					selectedActivityTagsSet.clear();

					for(const QString& at : std::as_const(ctr->tagsList)){
						selectedActivityTagsSet.insert(at);
						selectedActivityTagsListWidget->addItem(at);
					}

					spinBox->setValue(ctr->maxTags);

					break;
				}
			//191
			case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
				{
					ConstraintTeachersMaxActivityTagsPerRealDayFromSet* ctr=(ConstraintTeachersMaxActivityTagsPerRealDayFromSet*)oldtc;

					selectedActivityTagsListWidget->clear();
					selectedActivityTagsSet.clear();

					for(const QString& at : std::as_const(ctr->tagsList)){
						selectedActivityTagsSet.insert(at);
						selectedActivityTagsListWidget->addItem(at);
					}

					spinBox->setValue(ctr->maxTags);

					break;
				}
			//192
			case CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
				{
					ConstraintStudentsSetMaxActivityTagsPerRealDayFromSet* ctr=(ConstraintStudentsSetMaxActivityTagsPerRealDayFromSet*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					selectedActivityTagsListWidget->clear();
					selectedActivityTagsSet.clear();

					for(const QString& at : std::as_const(ctr->tagsList)){
						selectedActivityTagsSet.insert(at);
						selectedActivityTagsListWidget->addItem(at);
					}

					spinBox->setValue(ctr->maxTags);

					break;
				}
			//193
			case CONSTRAINT_STUDENTS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
				{
					ConstraintStudentsMaxActivityTagsPerRealDayFromSet* ctr=(ConstraintStudentsMaxActivityTagsPerRealDayFromSet*)oldtc;

					selectedActivityTagsListWidget->clear();
					selectedActivityTagsSet.clear();

					for(const QString& at : std::as_const(ctr->tagsList)){
						selectedActivityTagsSet.insert(at);
						selectedActivityTagsListWidget->addItem(at);
					}

					spinBox->setValue(ctr->maxTags);

					break;
				}
			//194
			case CONSTRAINT_MAX_HALF_DAYS_BETWEEN_ACTIVITIES:
				{
					ConstraintMaxHalfDaysBetweenActivities* ctr=(ConstraintMaxHalfDaysBetweenActivities*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int i=0; i<ctr->n_activities; i++){
						int actId=ctr->activitiesIds[i];
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					spinBox->setValue(ctr->maxDays);

					break;
				}
			//195
			case CONSTRAINT_ACTIVITY_BEGINS_STUDENTS_DAY:
				{
					ConstraintActivityBeginsStudentsDay* ctr=(ConstraintActivityBeginsStudentsDay*)oldtc;

					initialActivityId=ctr->activityId;

					break;
				}
			//196
			case CONSTRAINT_ACTIVITIES_BEGIN_STUDENTS_DAY:
				{
					ConstraintActivitiesBeginStudentsDay* ctr=(ConstraintActivitiesBeginStudentsDay*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);
					subjectsComboBox->setCurrentIndex(subjectsComboBox->findText(ctr->subjectName));
					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));

					break;
				}
			//197
			case CONSTRAINT_ACTIVITY_BEGINS_TEACHERS_DAY:
				{
					ConstraintActivityBeginsTeachersDay* ctr=(ConstraintActivityBeginsTeachersDay*)oldtc;

					initialActivityId=ctr->activityId;

					break;
				}
			//198
			case CONSTRAINT_ACTIVITIES_BEGIN_TEACHERS_DAY:
				{
					ConstraintActivitiesBeginTeachersDay* ctr=(ConstraintActivitiesBeginTeachersDay*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);
					subjectsComboBox->setCurrentIndex(subjectsComboBox->findText(ctr->subjectName));
					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));

					break;
				}
			//199
			case CONSTRAINT_TEACHERS_MIN_HOURS_PER_AFTERNOON:
				{
					ConstraintTeachersMinHoursPerAfternoon* ctr=(ConstraintTeachersMinHoursPerAfternoon*)oldtc;

					spinBox->setValue(ctr->minHoursPerAfternoon);
					checkBox->setChecked(ctr->allowEmptyAfternoons);

					break;
				}
			//200
			case CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON:
				{
					ConstraintTeacherMinHoursPerAfternoon* ctr=(ConstraintTeacherMinHoursPerAfternoon*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->minHoursPerAfternoon);
					checkBox->setChecked(ctr->allowEmptyAfternoons);

					break;
				}
			//201
			case CONSTRAINT_STUDENTS_MIN_HOURS_PER_AFTERNOON:
				{
					ConstraintStudentsMinHoursPerAfternoon* ctr=(ConstraintStudentsMinHoursPerAfternoon*)oldtc;

					spinBox->setValue(ctr->minHoursPerAfternoon);
					checkBox->setChecked(ctr->allowEmptyAfternoons);

					break;
				}
			//202
			case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_AFTERNOON:
				{
					ConstraintStudentsSetMinHoursPerAfternoon* ctr=(ConstraintStudentsSetMinHoursPerAfternoon*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->minHoursPerAfternoon);
					checkBox->setChecked(ctr->allowEmptyAfternoons);

					break;
				}
			//203
			case CONSTRAINT_ACTIVITIES_MAX_HOURLY_SPAN:
				{
					ConstraintActivitiesMaxHourlySpan* ctr=(ConstraintActivitiesMaxHourlySpan*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int i=0; i<ctr->n_activities; i++){
						int actId=ctr->activitiesIds[i];
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					spinBox->setValue(ctr->maxHourlySpan);

					break;
				}
			//204
			case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_IN_INTERVAL:
				{
					ConstraintTeachersMaxHoursDailyInInterval* ctr=(ConstraintTeachersMaxHoursDailyInInterval*)oldtc;

					spinBox->setValue(ctr->maxHoursDaily);
					intervalStartHourComboBox->setCurrentIndex(ctr->startHour);
					intervalEndHourComboBox->setCurrentIndex(ctr->endHour);

					break;
				}
			//205
			case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_IN_INTERVAL:
				{
					ConstraintTeacherMaxHoursDailyInInterval* ctr=(ConstraintTeacherMaxHoursDailyInInterval*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->maxHoursDaily);
					intervalStartHourComboBox->setCurrentIndex(ctr->startHour);
					intervalEndHourComboBox->setCurrentIndex(ctr->endHour);

					break;
				}
			//206
			case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_IN_INTERVAL:
				{
					ConstraintStudentsMaxHoursDailyInInterval* ctr=(ConstraintStudentsMaxHoursDailyInInterval*)oldtc;

					spinBox->setValue(ctr->maxHoursDaily);
					intervalStartHourComboBox->setCurrentIndex(ctr->startHour);
					intervalEndHourComboBox->setCurrentIndex(ctr->endHour);

					break;
				}
			//207
			case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_IN_INTERVAL:
				{
					ConstraintStudentsSetMaxHoursDailyInInterval* ctr=(ConstraintStudentsSetMaxHoursDailyInInterval*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxHoursDaily);
					intervalStartHourComboBox->setCurrentIndex(ctr->startHour);
					intervalEndHourComboBox->setCurrentIndex(ctr->endHour);

					break;
				}
			//208
			case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
				{
					ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay* ctr=(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					first_activityTagsComboBox->setCurrentIndex(first_activityTagsComboBox->findText(ctr->firstActivityTag));
					second_activityTagsComboBox->setCurrentIndex(second_activityTagsComboBox->findText(ctr->secondActivityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//209
			case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
				{
					ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay* ctr=(ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay*)oldtc;

					first_activityTagsComboBox->setCurrentIndex(first_activityTagsComboBox->findText(ctr->firstActivityTag));
					second_activityTagsComboBox->setCurrentIndex(second_activityTagsComboBox->findText(ctr->secondActivityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//210
			case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
				{
					ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay* ctr=(ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacher));

					first_activityTagsComboBox->setCurrentIndex(first_activityTagsComboBox->findText(ctr->firstActivityTag));
					second_activityTagsComboBox->setCurrentIndex(second_activityTagsComboBox->findText(ctr->secondActivityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//211
			case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
				{
					ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay* ctr=(ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay*)oldtc;

					first_activityTagsComboBox->setCurrentIndex(first_activityTagsComboBox->findText(ctr->firstActivityTag));
					second_activityTagsComboBox->setCurrentIndex(second_activityTagsComboBox->findText(ctr->secondActivityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//212
			case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
				{
					ConstraintStudentsSetMinGapsBetweenActivityTagPerRealDay* ctr=(ConstraintStudentsSetMinGapsBetweenActivityTagPerRealDay*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//213
			case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
				{
					ConstraintStudentsMinGapsBetweenActivityTagPerRealDay* ctr=(ConstraintStudentsMinGapsBetweenActivityTagPerRealDay*)oldtc;

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//214
			case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
				{
					ConstraintTeacherMinGapsBetweenActivityTagPerRealDay* ctr=(ConstraintTeacherMinGapsBetweenActivityTagPerRealDay*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacher));

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//215
			case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
				{
					ConstraintTeachersMinGapsBetweenActivityTagPerRealDay* ctr=(ConstraintTeachersMinGapsBetweenActivityTagPerRealDay*)oldtc;

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//216
			case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
				{
					ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon* ctr=(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					first_activityTagsComboBox->setCurrentIndex(first_activityTagsComboBox->findText(ctr->firstActivityTag));
					second_activityTagsComboBox->setCurrentIndex(second_activityTagsComboBox->findText(ctr->secondActivityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//217
			case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
				{
					ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon* ctr=(ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon*)oldtc;

					first_activityTagsComboBox->setCurrentIndex(first_activityTagsComboBox->findText(ctr->firstActivityTag));
					second_activityTagsComboBox->setCurrentIndex(second_activityTagsComboBox->findText(ctr->secondActivityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//218
			case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
				{
					ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon* ctr=(ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacher));

					first_activityTagsComboBox->setCurrentIndex(first_activityTagsComboBox->findText(ctr->firstActivityTag));
					second_activityTagsComboBox->setCurrentIndex(second_activityTagsComboBox->findText(ctr->secondActivityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//219
			case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
				{
					ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon* ctr=(ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon*)oldtc;

					first_activityTagsComboBox->setCurrentIndex(first_activityTagsComboBox->findText(ctr->firstActivityTag));
					second_activityTagsComboBox->setCurrentIndex(second_activityTagsComboBox->findText(ctr->secondActivityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//220
			case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
				{
					ConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon* ctr=(ConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//221
			case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
				{
					ConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon* ctr=(ConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon*)oldtc;

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//222
			case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
				{
					ConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon* ctr=(ConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacher));

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//223
			case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
				{
					ConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon* ctr=(ConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon*)oldtc;

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTag));
					spinBox->setValue(ctr->minGaps);

					break;
				}
			//224
			case CONSTRAINT_TEACHERS_NO_TWO_CONSECUTIVE_DAYS:
				{
					//ConstraintTeachersNoTwoConsecutiveDays* ctr=(ConstraintTeachersNoTwoConsecutiveDays*)oldtc;

					//nothing to be done

					break;
				}
			//225
			case CONSTRAINT_TEACHER_NO_TWO_CONSECUTIVE_DAYS:
				{
					ConstraintTeacherNoTwoConsecutiveDays* ctr=(ConstraintTeacherNoTwoConsecutiveDays*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					break;
				}
			//226
			case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
				{
					ConstraintTeacherPairOfMutuallyExclusiveTimeSlots* ctr=(ConstraintTeacherPairOfMutuallyExclusiveTimeSlots*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					firstDayComboBox->setCurrentIndex(ctr->day1);
					firstHourComboBox->setCurrentIndex(ctr->hour1);

					secondDayComboBox->setCurrentIndex(ctr->day2);
					secondHourComboBox->setCurrentIndex(ctr->hour2);

					break;
				}
			//227
			case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
				{
					ConstraintTeachersPairOfMutuallyExclusiveTimeSlots* ctr=(ConstraintTeachersPairOfMutuallyExclusiveTimeSlots*)oldtc;

					firstDayComboBox->setCurrentIndex(ctr->day1);
					firstHourComboBox->setCurrentIndex(ctr->hour1);

					secondDayComboBox->setCurrentIndex(ctr->day2);
					secondHourComboBox->setCurrentIndex(ctr->hour2);

					break;
				}
			//228
			case CONSTRAINT_STUDENTS_SET_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
				{
					ConstraintStudentsSetPairOfMutuallyExclusiveTimeSlots* ctr=(ConstraintStudentsSetPairOfMutuallyExclusiveTimeSlots*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					firstDayComboBox->setCurrentIndex(ctr->day1);
					firstHourComboBox->setCurrentIndex(ctr->hour1);

					secondDayComboBox->setCurrentIndex(ctr->day2);
					secondHourComboBox->setCurrentIndex(ctr->hour2);

					break;
				}
			//229
			case CONSTRAINT_STUDENTS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
				{
					ConstraintStudentsPairOfMutuallyExclusiveTimeSlots* ctr=(ConstraintStudentsPairOfMutuallyExclusiveTimeSlots*)oldtc;

					firstDayComboBox->setCurrentIndex(ctr->day1);
					firstHourComboBox->setCurrentIndex(ctr->hour1);

					secondDayComboBox->setCurrentIndex(ctr->day2);
					secondHourComboBox->setCurrentIndex(ctr->hour2);

					break;
				}
			//230
			case CONSTRAINT_TWO_SETS_OF_ACTIVITIES_SAME_SECTIONS:
				{
					ConstraintTwoSetsOfActivitiesSameSections* ctr=(ConstraintTwoSetsOfActivitiesSameSections*)oldtc;

					selectedActivitiesListWidget_TwoSetsOfActivities_1->clear();
					selectedActivitiesListWidget_TwoSetsOfActivities_2->clear();
					selectedActivitiesList_TwoSetsOfActivities_1.clear();
					selectedActivitiesList_TwoSetsOfActivities_2.clear();

					for(int actId : std::as_const(ctr->activitiesAIds)){
						selectedActivitiesList_TwoSetsOfActivities_1.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget_TwoSetsOfActivities_1->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget_TwoSetsOfActivities_1->item(selectedActivitiesListWidget_TwoSetsOfActivities_1->count()-1)->setBackground(selectedActivitiesListWidget_TwoSetsOfActivities_1->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget_TwoSetsOfActivities_1->item(selectedActivitiesListWidget_TwoSetsOfActivities_1->count()-1)->setForeground(selectedActivitiesListWidget_TwoSetsOfActivities_1->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					for(int actId : std::as_const(ctr->activitiesBIds)){
						selectedActivitiesList_TwoSetsOfActivities_2.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget_TwoSetsOfActivities_2->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget_TwoSetsOfActivities_2->item(selectedActivitiesListWidget_TwoSetsOfActivities_2->count()-1)->setBackground(selectedActivitiesListWidget_TwoSetsOfActivities_2->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget_TwoSetsOfActivities_2->item(selectedActivitiesListWidget_TwoSetsOfActivities_2->count()-1)->setForeground(selectedActivitiesListWidget_TwoSetsOfActivities_2->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					fillTimesTable(timesTable, ctr->oDays, ctr->oHours, true);

					break;
				}
			//231
			case CONSTRAINT_STUDENTS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
				{
					ConstraintStudentsMaxSingleGapsInSelectedTimeSlots* ctr=(ConstraintStudentsMaxSingleGapsInSelectedTimeSlots*)oldtc;

					spinBox->setValue(ctr->maxSingleGaps);

					fillTimesTable(timesTable, ctr->selectedDays, ctr->selectedHours, true);

					break;
				}
			//232
			case CONSTRAINT_STUDENTS_SET_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
				{
					ConstraintStudentsSetMaxSingleGapsInSelectedTimeSlots* ctr=(ConstraintStudentsSetMaxSingleGapsInSelectedTimeSlots*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxSingleGaps);

					fillTimesTable(timesTable, ctr->selectedDays, ctr->selectedHours, true);

					break;
				}
			//233
			case CONSTRAINT_TEACHERS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
				{
					ConstraintTeachersMaxSingleGapsInSelectedTimeSlots* ctr=(ConstraintTeachersMaxSingleGapsInSelectedTimeSlots*)oldtc;

					spinBox->setValue(ctr->maxSingleGaps);

					fillTimesTable(timesTable, ctr->selectedDays, ctr->selectedHours, true);

					break;
				}
			//234
			case CONSTRAINT_TEACHER_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
				{
					ConstraintTeacherMaxSingleGapsInSelectedTimeSlots* ctr=(ConstraintTeacherMaxSingleGapsInSelectedTimeSlots*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacher));

					spinBox->setValue(ctr->maxSingleGaps);

					fillTimesTable(timesTable, ctr->selectedDays, ctr->selectedHours, true);

					break;
				}
			//235
			case CONSTRAINT_TEACHER_MAX_HOURS_PER_TERM:
				{
					ConstraintTeacherMaxHoursPerTerm* ctr=(ConstraintTeacherMaxHoursPerTerm*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					spinBox->setValue(ctr->maxHoursPerTerm);

					break;
				}
			//236
			case CONSTRAINT_TEACHERS_MAX_HOURS_PER_TERM:
				{
					ConstraintTeachersMaxHoursPerTerm* ctr=(ConstraintTeachersMaxHoursPerTerm*)oldtc;

					spinBox->setValue(ctr->maxHoursPerTerm);

					break;
				}
			//237
			case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
				{
					ConstraintTeacherPairOfMutuallyExclusiveSetsOfTimeSlots* ctr=(ConstraintTeacherPairOfMutuallyExclusiveSetsOfTimeSlots*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					fillTimesTable(timesTable1, ctr->selectedDays1, ctr->selectedHours1, true);
					fillTimesTable(timesTable2, ctr->selectedDays2, ctr->selectedHours2, true);

					break;
				}
			//238
			case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
				{
					ConstraintTeachersPairOfMutuallyExclusiveSetsOfTimeSlots* ctr=(ConstraintTeachersPairOfMutuallyExclusiveSetsOfTimeSlots*)oldtc;

					fillTimesTable(timesTable1, ctr->selectedDays1, ctr->selectedHours1, true);
					fillTimesTable(timesTable2, ctr->selectedDays2, ctr->selectedHours2, true);

					break;
				}
			//239
			case CONSTRAINT_STUDENTS_SET_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
				{
					ConstraintStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots* ctr=(ConstraintStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					fillTimesTable(timesTable1, ctr->selectedDays1, ctr->selectedHours1, true);
					fillTimesTable(timesTable2, ctr->selectedDays2, ctr->selectedHours2, true);

					break;
				}
			//240
			case CONSTRAINT_STUDENTS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
				{
					ConstraintStudentsPairOfMutuallyExclusiveSetsOfTimeSlots* ctr=(ConstraintStudentsPairOfMutuallyExclusiveSetsOfTimeSlots*)oldtc;

					fillTimesTable(timesTable1, ctr->selectedDays1, ctr->selectedHours1, true);
					fillTimesTable(timesTable2, ctr->selectedDays2, ctr->selectedHours2, true);

					break;
				}
			//241
			case CONSTRAINT_ACTIVITIES_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
				{
					ConstraintActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots* ctr=(ConstraintActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots*)oldtc;

					fillTimesTable(timesTable1, ctr->selectedDays1, ctr->selectedHours1, true);
					fillTimesTable(timesTable2, ctr->selectedDays2, ctr->selectedHours2, true);

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int i=0; i<ctr->activitiesIds.count(); i++){
						int actId=ctr->activitiesIds[i];
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					break;
				}
			//242
			case CONSTRAINT_ACTIVITIES_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
				{
					ConstraintActivitiesPairOfMutuallyExclusiveTimeSlots* ctr=(ConstraintActivitiesPairOfMutuallyExclusiveTimeSlots*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int i=0; i<ctr->activitiesIds.count(); i++){
						int actId=ctr->activitiesIds[i];
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					firstDayComboBox->setCurrentIndex(ctr->day1);
					firstHourComboBox->setCurrentIndex(ctr->hour1);

					secondDayComboBox->setCurrentIndex(ctr->day2);
					secondHourComboBox->setCurrentIndex(ctr->hour2);

					break;
				}
			//243
			case CONSTRAINT_TEACHER_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
				{
					ConstraintTeacherOccupiesMaxSetsOfTimeSlotsFromSelection* ctr=(ConstraintTeacherOccupiesMaxSetsOfTimeSlotsFromSelection*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					fillSpinBoxTimesTable(ctr->selectedDays, ctr->selectedHours);

					spinBox->setValue(ctr->maxOccupiedSets);

					break;
				}
			//244
			case CONSTRAINT_TEACHERS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
				{
					ConstraintTeachersOccupyMaxSetsOfTimeSlotsFromSelection* ctr=(ConstraintTeachersOccupyMaxSetsOfTimeSlotsFromSelection*)oldtc;

					fillSpinBoxTimesTable(ctr->selectedDays, ctr->selectedHours);

					spinBox->setValue(ctr->maxOccupiedSets);

					break;
				}
			//245
			case CONSTRAINT_STUDENTS_SET_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
				{
					ConstraintStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection* ctr=(ConstraintStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection*)oldtc;

					int j=studentsComboBox->findText(ctr->students);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->students);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					fillSpinBoxTimesTable(ctr->selectedDays, ctr->selectedHours);

					spinBox->setValue(ctr->maxOccupiedSets);

					break;
				}
			//246
			case CONSTRAINT_STUDENTS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
				{
					ConstraintStudentsOccupyMaxSetsOfTimeSlotsFromSelection* ctr=(ConstraintStudentsOccupyMaxSetsOfTimeSlotsFromSelection*)oldtc;

					fillSpinBoxTimesTable(ctr->selectedDays, ctr->selectedHours);

					spinBox->setValue(ctr->maxOccupiedSets);

					break;
				}
			//247
			case CONSTRAINT_ACTIVITIES_OVERLAP_COMPLETELY_OR_DO_NOT_OVERLAP:
				{
					ConstraintActivitiesOverlapCompletelyOrDoNotOverlap* ctr=(ConstraintActivitiesOverlapCompletelyOrDoNotOverlap*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int i=0; i<ctr->activitiesIds.count(); i++){
						int actId=ctr->activitiesIds[i];
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					break;
				}
			//248
			case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
				{
					ConstraintActivitiesOccupyMaxSetsOfTimeSlotsFromSelection* ctr=(ConstraintActivitiesOccupyMaxSetsOfTimeSlotsFromSelection*)oldtc;

					fillSpinBoxTimesTable(ctr->selectedDays, ctr->selectedHours);

					spinBox->setValue(ctr->maxOccupiedSets);

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int i=0; i<ctr->activitiesIds.count(); i++){
						int actId=ctr->activitiesIds[i];
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					break;
				}
			//249
			case CONSTRAINT_ACTIVITY_BEGINS_OR_ENDS_STUDENTS_DAY:
				{
					ConstraintActivityBeginsOrEndsStudentsDay* ctr=(ConstraintActivityBeginsOrEndsStudentsDay*)oldtc;

					initialActivityId=ctr->activityId;

					break;
				}
			//250
			case CONSTRAINT_ACTIVITIES_BEGIN_OR_END_STUDENTS_DAY:
				{
					ConstraintActivitiesBeginOrEndStudentsDay* ctr=(ConstraintActivitiesBeginOrEndStudentsDay*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);
					subjectsComboBox->setCurrentIndex(subjectsComboBox->findText(ctr->subjectName));
					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));

					break;
				}
			//251
			case CONSTRAINT_ACTIVITY_BEGINS_OR_ENDS_TEACHERS_DAY:
				{
					ConstraintActivityBeginsOrEndsTeachersDay* ctr=(ConstraintActivityBeginsOrEndsTeachersDay*)oldtc;

					initialActivityId=ctr->activityId;

					break;
				}
			//252
			case CONSTRAINT_ACTIVITIES_BEGIN_OR_END_TEACHERS_DAY:
				{
					ConstraintActivitiesBeginOrEndTeachersDay* ctr=(ConstraintActivitiesBeginOrEndTeachersDay*)oldtc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));
					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);
					subjectsComboBox->setCurrentIndex(subjectsComboBox->findText(ctr->subjectName));
					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));

					break;
				}
			//253
			case CONSTRAINT_ACTIVITIES_MAX_TOTAL_NUMBER_OF_STUDENTS_IN_SELECTED_TIME_SLOTS:
				{
					ConstraintActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots* ctr=(ConstraintActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots*)oldtc;

					selectedActivitiesListWidget->clear();
					selectedActivitiesList.clear();

					for(int actId : std::as_const(ctr->activitiesIds)){
						selectedActivitiesList.append(actId);
						Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
						assert(act!=nullptr);
						selectedActivitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
					}

					fillTimesTable(timesTable, ctr->selectedDays, ctr->selectedHours, true);

					spinBox->setValue(ctr->maxNumberOfStudents);

					break;
				}

			default:
				assert(0);
				break;
		}
	}

	if(filterGroupBox!=nullptr
			&& activitiesListWidget_TwoSetsOfActivities_1!=nullptr
			&& activitiesListWidget_TwoSetsOfActivities_2!=nullptr){

		assert(activitiesComboBox==nullptr);
		assert(activitiesListWidget==nullptr);

		filterActivitiesListWidgets1And2();
	}

	if(filterGroupBox!=nullptr && activitiesListWidget!=nullptr){
		assert(activitiesComboBox==nullptr);
		assert(activitiesListWidget_TwoSetsOfActivities_1==nullptr);
		assert(activitiesListWidget_TwoSetsOfActivities_2==nullptr);

		filterActivitiesListWidget();
	}

	if(filterGroupBox!=nullptr && activitiesComboBox!=nullptr){
		assert(activitiesListWidget==nullptr);
		assert(activitiesListWidget_TwoSetsOfActivities_1==nullptr);
		assert(activitiesListWidget_TwoSetsOfActivities_2==nullptr);

		int i=filterActivitiesComboBox();
		if(i>=0){
			assert(activitiesComboBox!=nullptr);
			assert(i<activitiesList.count());
			assert(i<activitiesComboBox->count());
			activitiesComboBox->setCurrentIndex(i);
		}
	}

	if(first_filterGroupBox!=nullptr && first_activitiesComboBox!=nullptr){
		int i=first_activitiesComboBoxFilter();
		if(i>=0){
			assert(first_activitiesComboBox!=nullptr);
			assert(i<first_activitiesList.count());
			assert(i<first_activitiesComboBox->count());
			first_activitiesComboBox->setCurrentIndex(i);
		}
	}
	if(second_filterGroupBox!=nullptr && second_activitiesComboBox!=nullptr){
		int i=second_activitiesComboBoxFilter();
		if(i>=0){
			assert(second_activitiesComboBox!=nullptr);
			assert(i<second_activitiesList.count());
			assert(i<second_activitiesComboBox->count());
			second_activitiesComboBox->setCurrentIndex(i);
		}
	}
	if(third_filterGroupBox!=nullptr && third_activitiesComboBox!=nullptr){
		int i=third_activitiesComboBoxFilter();
		if(i>=0){
			assert(third_activitiesComboBox!=nullptr);
			assert(i<third_activitiesList.count());
			assert(i<third_activitiesComboBox->count());
			third_activitiesComboBox->setCurrentIndex(i);
		}
	}

	if(checkBox!=nullptr)
		connect(checkBox, &QCheckBox::toggled, this, &AddOrModifyTimeConstraint::checkBoxToggled);

	if(durationCheckBox!=nullptr)
		connect(durationCheckBox, &QCheckBox::toggled, this, &AddOrModifyTimeConstraint::durationCheckBoxToggled);

	dialog->setModal(true);
	dialog->setWindowModality(Qt::ApplicationModal);
	dialog->show();
	
	eventLoop->exec();
}

AddOrModifyTimeConstraint::~AddOrModifyTimeConstraint()
{
	//saveFETDialogGeometry(dialog, dialogName);
	
	//dialog->hide();
	
	//delete dialog;
	
	assert(!eventLoop->isRunning());
	delete eventLoop;
}

void AddOrModifyTimeConstraint::addConstraintClicked()
{
	TimeConstraint* tc=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(dialog, tr("FET information"), tr("Invalid weight (percentage)"));
		return;
	}

	if(!timeConstraintCanHaveAnyWeight(type) && weight!=100.0){
		QMessageBox::warning(dialog, tr("FET information"), tr("Invalid weight (percentage) - it has to be 100%"));
		return;
	}

	switch(type){
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_REAL_DAYS:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_REAL_DAYS:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_REAL_DAYS:
			if(weight<100.0){
				int t=QMessageBox::warning(dialog, tr("FET warning"),
					tr("You selected a weight less than 100%. The generation algorithm is not perfectly optimized to work with such weights (even"
					 " if in practice it might work well). It is recommended to work only with 100% weights for these constraints. Are you sure you want to continue?"),
					 QMessageBox::Yes | QMessageBox::Cancel);
				if(t==QMessageBox::Cancel)
					return;
			}
			break;

		default:
			//nothing;
			break;
	}

	if(!addEmpty && teachersComboBox!=nullptr){
		assert(filterGroupBox==nullptr);
		QString teacher_name=teachersComboBox->currentText();
		int teacher_ID=gt.rules.searchTeacher(teacher_name);
		if(teacher_ID<0){
			QMessageBox::warning(dialog, tr("FET information"), tr("Invalid teacher"));
			return;
		}
	}

	if(!addEmpty && studentsComboBox!=nullptr){
		assert(filterGroupBox==nullptr);
		QString students_name=studentsComboBox->currentText();
		StudentsSet* s=gt.rules.searchStudentsSet(students_name);
		if(s==nullptr){
			QMessageBox::warning(dialog, tr("FET information"), tr("Invalid students set"));
			return;
		}
	}

	if(!addEmpty && activityTagsComboBox!=nullptr){
		assert(filterGroupBox==nullptr);
		QString activity_tag_name=activityTagsComboBox->currentText();
		int activityTag_ID=gt.rules.searchActivityTag(activity_tag_name);
		if(activityTag_ID<0){
			QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity tag"));
			return;
		}
	}

	switch(type){
		//1
		case CONSTRAINT_BASIC_COMPULSORY_TIME:
			{
				tc=new ConstraintBasicCompulsoryTime(weight);
				
				break;
			}
		//2
		case CONSTRAINT_BREAK_TIMES:
			{
				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				tc=new ConstraintBreakTimes(weight, days, hours);
				
				break;
			}
		//3
		case CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES:
			{
				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				tc=new ConstraintTeacherNotAvailableTimes(weight, teachersComboBox->currentText(), days, hours);

				break;
			}
		//4
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY:
			{
				tc=new ConstraintTeachersMaxHoursDaily(weight, spinBox->value());

				break;
			}
		//5
		case CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK:
			{
				tc=new ConstraintTeacherMaxDaysPerWeek(weight, spinBox->value(), teachersComboBox->currentText());

				break;
			}
		//6
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK:
		{
			tc=new ConstraintTeachersMaxGapsPerWeek(weight, spinBox->value());

			break;
		}
		//7
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK:
		{
			tc=new ConstraintTeacherMaxGapsPerWeek(weight, teachersComboBox->currentText(), spinBox->value());

			break;
		}
		//8
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY:
			{
				tc=new ConstraintTeacherMaxHoursDaily(weight, spinBox->value(), teachersComboBox->currentText());

				break;
			}
		//9
		case CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY:
			{
				tc=new ConstraintTeachersMaxHoursContinuously(weight, spinBox->value());

				break;
			}
		//10
		case CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY:
			{
				tc=new ConstraintTeacherMaxHoursContinuously(weight, spinBox->value(), teachersComboBox->currentText());

				break;
			}
		//11
		case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY:
			{
				if(!checkBox->isChecked()){
					if(gt.rules.mode!=MORNINGS_AFTERNOONS){
						QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for the teachers, "
							"please use the constraint teachers min days per week"));
						return;
					}
					else{
						QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for the teachers, "
							"please use the constraint teachers min days per week (but the min days per week constraint is for real days. You can also use the "
							"constraint teachers min mornings/afternoons per week.)"));
						return;
					}
				}

				tc=new ConstraintTeachersMinHoursDaily(weight, spinBox->value(), checkBox->isChecked());

				break;
			}
		//12
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY:
			{
				if(!checkBox->isChecked()){
					if(gt.rules.mode!=MORNINGS_AFTERNOONS){
						QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for a teacher, "
							"please use the constraint teacher min days per week"));
						return;
					}
					else{
						QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for a teacher, "
							"please use the constraint teacher min days per week (but the min days per week constraint is for real days. You can also use the "
							"constraints teacher min mornings/afternoons per week.)"));
						return;
					}
				}

				tc=new ConstraintTeacherMinHoursDaily(weight, spinBox->value(), teachersComboBox->currentText(), checkBox->isChecked());

				break;
			}
		//13
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY:
			{
				tc=new ConstraintTeachersMaxGapsPerDay(weight, spinBox->value());

				break;
			}
		//14
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY:
			{
				tc=new ConstraintTeacherMaxGapsPerDay(weight, teachersComboBox->currentText(), spinBox->value());

				break;
			}
		//15
		case CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				tc=new ConstraintStudentsEarlyMaxBeginningsAtSecondHour(weight, spinBox->value());

				break;
			}
		//16
		case CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				tc=new ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//17
		case CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES:
			{
				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				tc=new ConstraintStudentsSetNotAvailableTimes(weight, studentsComboBox->currentText(), days, hours);

				break;
			}
		//18
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK:
		{
			tc=new ConstraintStudentsMaxGapsPerWeek(weight, spinBox->value());

			break;
		}
		//19
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK:
		{
			tc=new ConstraintStudentsSetMaxGapsPerWeek(weight, spinBox->value(), studentsComboBox->currentText());

			break;
		}
		//20
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY:
			{
				tc=new ConstraintStudentsMaxHoursDaily(weight, spinBox->value());

				break;
			}
		//21
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY:
			{
				tc=new ConstraintStudentsSetMaxHoursDaily(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//22
		case CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY:
			{
				tc=new ConstraintStudentsMaxHoursContinuously(weight, spinBox->value());

				break;
			}
		//23
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY:
			{
				tc=new ConstraintStudentsSetMaxHoursContinuously(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//24
		case CONSTRAINT_STUDENTS_MIN_HOURS_DAILY:
			{
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					if(checkBox->isChecked() && spinBox->value()<2){
						QMessageBox::warning(dialog, tr("FET warning"), tr("If you allow empty days, the min hours must be at least 2 (to make it a non-trivial constraint)"));
						return;
					}
				}

				tc=new ConstraintStudentsMinHoursDaily(weight, spinBox->value(), checkBox->isChecked());

				break;
			}
		//25
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY:
			{
				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					if(checkBox->isChecked() && spinBox->value()<2){
						QMessageBox::warning(dialog, tr("FET warning"), tr("If you allow empty days, the min hours must be at least 2 (to make it a non-trivial constraint)"));
						return;
					}
				}

				tc=new ConstraintStudentsSetMinHoursDaily(weight, spinBox->value(), studentsComboBox->currentText(), checkBox->isChecked());

				break;
			}
		//26
		case CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY:
			{
				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				tc=new ConstraintActivityEndsStudentsDay(weight, id);

				break;
			}
		//27
		case CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME:
			{
				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				if(daysComboBox->currentIndex()<0 || daysComboBox->currentIndex()>gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid day"));
					return;
				}
				if(hoursComboBox->currentIndex()<0 || hoursComboBox->currentIndex()>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid hour"));
					return;
				}

				tc=new ConstraintActivityPreferredStartingTime(weight, id, daysComboBox->currentIndex(), hoursComboBox->currentIndex(), permanentlyLockedCheckBox->isChecked());

				break;
			}
		//28
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}

				if(!checkBox->isChecked()){
					tc=new ConstraintActivitiesSameStartingTime(weight, selectedActivitiesList.count(), selectedActivitiesList);
				}
				else{
					//block of constraints
					///////////phase 1 - how many constraints will be added?
					int nConstraints=0;
					QList<int>::const_iterator it;
					for(it=selectedActivitiesList.constBegin(); it!=selectedActivitiesList.constEnd(); it++){
						int _id=(*it);
						int tmp=0; //tmp represents the number of subactivities represented by the current (sub)activity

						for(Activity* act : std::as_const(gt.rules.activitiesList)){
							if(act->activityGroupId==0){
								if(act->id==_id){
									assert(tmp==0);
									tmp=1;
								}
							}
							else{
								if(act->id==_id){
									assert(act->activityGroupId==act->id);
									assert(tmp==0);
									tmp=1;
								}
								else if(act->activityGroupId==_id)
									tmp++;
							}
						}

						if(nConstraints==0){
							nConstraints=tmp;
						}
						else{
							if(tmp!=nConstraints){
								QString s=tr("Subactivities do not correspond. Mistake:");
								s+="\n";
								s+=tr("1. First (sub)activity has id=%1 and represents %2 subactivities")
									.arg(this->selectedActivitiesList.at(0))
									.arg(nConstraints);
								s+="\n";
								s+=tr("2. Current (sub)activity has id=%1 and represents %2 subactivities")
									.arg(_id)
									.arg(tmp);
								QMessageBox::warning(dialog, tr("FET information"), s);
								return;
							}
						}
					}

					/////////////phase 2 - compute the indices of all the (sub)activities
					Matrix1D<QList<int>> ids;
					ids.resize(nConstraints);

					for(int i=0; i<nConstraints; i++)
						ids[i].clear();
					int k;
					for(k=0, it=selectedActivitiesList.constBegin(); it!=selectedActivitiesList.constEnd(); k++, it++){
						int _id=(*it);
						int tmp=0; //tmp represents the number of subactivities represented by the current (sub)activity

						for(Activity* act : std::as_const(gt.rules.activitiesList)){
							if(act->activityGroupId==0){
								if(act->id==_id){
									assert(tmp==0);
									assert(ids[tmp].count()==k);
									ids[tmp].append(_id);
									tmp=1;
								}
							}
							else{
								if(act->id==_id){
									assert(act->activityGroupId==act->id);
									assert(tmp==0);
									assert(ids[tmp].count()==k);
									ids[tmp].append(_id);
									tmp=1;
								}
								else if(act->activityGroupId==_id){
									assert(ids[tmp].count()==k);
									ids[tmp].append(act->id);
									tmp++;
								}
							}
						}
					}

					////////////////phase 3 - add the constraints
					QString ctrs;
					for(k=0; k<nConstraints; k++){
						TimeConstraint* ctr=new ConstraintActivitiesSameStartingTime(weight, this->selectedActivitiesList.count(), ids[k]);
						bool tmp2=gt.rules.addTimeConstraint(ctr);

						if(tmp2){
							QString s;

							s+=tr("Constraint added:");
							s+="\n\n";
							s+=ctr->getDetailedDescription(gt.rules);
							LongTextMessageBox::information(dialog, tr("FET information"), s);

							ctrs+=ctr->getDetailedDescription(gt.rules);
							ctrs+="\n";
						}
						else{
							QMessageBox::warning(dialog, tr("FET information"), tr("Constraint NOT added - please report error"));
							delete ctr;
						}
					}

					gt.rules.addUndoPoint(tr("Added %1 constraints (using the option to add multiple constraints):\n\n%2",
											 "%1 is the number of constraints, %2 is their detailed description")
											 .arg(nConstraints).arg(ctrs));

					return; //very important!!!
				}

				break;
			}
		//29
		case CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}

				tc=new ConstraintActivitiesNotOverlapping(weight, selectedActivitiesList.count(), selectedActivitiesList);

				break;
			}
		//30
		case CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}

				if(spinBox->value()<=0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Invalid number of min days between activities - it must be greater than 0."));
					return;
				}

				tc=new ConstraintMinDaysBetweenActivities(weight, checkBox->isChecked(), selectedActivitiesList.count(), selectedActivitiesList, spinBox->value());

				break;
			}
		//31
		case CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS:
			{
				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				QList<int> days_L;
				QList<int> hours_L;
				getTimesTable(timesTable, days_L, hours_L, false);
				int n=days_L.count();
				assert(n==hours_L.count());

				if(n<=0){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("Warning: 0 slots selected. Are you sure?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				tc=new ConstraintActivityPreferredTimeSlots(weight, id, n, days_L, hours_L);

				break;
			}
		//32
		case CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS:
			{
				int duration=-1;
				if(durationCheckBox->isChecked()){
					assert(durationSpinBox->isEnabled());
					duration=durationSpinBox->value();
				}

				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				if(duration==-1 && teacher=="" && students=="" && subject=="" && activityTag==""){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("You specified all the activities. This might cause a minor issue: if you specify"
					  " a not allowed slot between two allowed slots, this not allowed slot will"
					  " be counted as a gap in the teachers' and students' timetable.\n\n"
					  "The best practice would be to use constraint break times.\n\n"
					  "If you need weight under 100%, then you can use this constraint, but be careful"
					  " not to obtain an impossible timetable (if your teachers/students are constrained on gaps"
					  " or early gaps and if you leave a not allowed slot between 2 allowed slots or"
					  " a not allowed slot early in the day and more allowed slots after it,"
					  " this possible gap might be counted in teachers' and students' timetable)")
					  +"\n\n"+tr("Do you want to add current constraint?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				if(duration==-1 && teacher!="" && students=="" && subject=="" && activityTag==""){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("You specified only the teacher. This might cause a minor issue: if you specify"
					  " a not allowed slot between two allowed slots, this not allowed slot will"
					  " be counted as a gap in the teacher's timetable.\n\n"
					  "The best practice would be to use constraint teacher not available times.\n\n"
					  "If you need weight under 100%, then you can use this constraint, but be careful"
					  " not to obtain an impossible timetable (if your teacher is constrained on gaps"
					  " and if you leave a not allowed slot between 2 allowed slots, this possible"
					  " gap might be counted in teacher's timetable)")
					  +"\n\n"+tr("Do you want to add current constraint?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				if(duration==-1 && teacher=="" && students!="" && subject=="" && activityTag==""){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("You specified only the students set. This might cause a minor issue: if you specify"
					  " a not allowed slot between two allowed slots (or a not allowed slot before allowed slots),"
					  " this not allowed slot will"
					  " be counted as a gap (or early gap) in the students' timetable.\n\n"
					  "The best practice would be to use constraint students set not available times.\n\n"
					  "If you need weight under 100%, then you can use this constraint, but be careful"
					  " not to obtain an impossible timetable (if your students set is constrained on gaps or early gaps"
					  " and if you leave a not allowed slot between 2 allowed slots (or a not allowed slot before allowed slots), this possible"
					  " gap might be counted in students' timetable)")
					  +"\n\n"+tr("Do you want to add current constraint?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				QList<int> days_L;
				QList<int> hours_L;
				getTimesTable(timesTable, days_L, hours_L, false);
				int n=days_L.count();
				assert(n==hours_L.count());

				if(n<=0){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("Warning: 0 slots selected. Are you sure?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				tc=new ConstraintActivitiesPreferredTimeSlots(weight, teacher, students, subject, activityTag, duration, n, days_L, hours_L);

				break;
			}
		//33
		case CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES:
			{
				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				QList<int> days_L;
				QList<int> hours_L;
				getTimesTable(timesTable, days_L, hours_L, false);
				int n=days_L.count();
				assert(n==hours_L.count());

				if(n<=0){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("Warning: 0 slots selected. Are you sure?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				tc=new ConstraintActivityPreferredStartingTimes(weight, id, n, days_L, hours_L);

				break;
			}
		//34
		case CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES:
			{
				int duration=-1;
				if(durationCheckBox->isChecked()){
					assert(durationSpinBox->isEnabled());
					duration=durationSpinBox->value();
				}

				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				if(duration==-1 && teacher=="" && students=="" && subject=="" && activityTag==""){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("You specified all the activities. This might cause a minor issue: if you specify"
					  " a not allowed slot between two allowed slots, this not allowed slot will"
					  " be counted as a gap in the teachers' and students' timetable.\n\n"
					  "The best practice would be to use constraint break times.\n\n"
					  "If you need weight under 100%, then you can use this constraint, but be careful"
					  " not to obtain an impossible timetable (if your teachers/students are constrained on gaps"
					  " or early gaps and if you leave a not allowed slot between 2 allowed slots or"
					  " a not allowed slot early in the day and more allowed slots after it,"
					  " this possible gap might be counted in teachers' and students' timetable)")
					  +"\n\n"+tr("Do you want to add current constraint?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				if(duration==-1 && teacher!="" && students=="" && subject=="" && activityTag==""){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("You specified only the teacher. This might cause a minor issue: if you specify"
					  " a not allowed slot between two allowed slots, this not allowed slot will"
					  " be counted as a gap in the teacher's timetable.\n\n"
					  "The best practice would be to use constraint teacher not available times.\n\n"
					  "If you need weight under 100%, then you can use this constraint, but be careful"
					  " not to obtain an impossible timetable (if your teacher is constrained on gaps"
					  " and if you leave a not allowed slot between 2 allowed slots, this possible"
					  " gap might be counted in teacher's timetable)")
					  +"\n\n"+tr("Do you want to add current constraint?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				if(duration==-1 && teacher=="" && students!="" && subject=="" && activityTag==""){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("You specified only the students set. This might cause a minor issue: if you specify"
					  " a not allowed slot between two allowed slots (or a not allowed slot before allowed slots),"
					  " this not allowed slot will"
					  " be counted as a gap (or early gap) in the students' timetable.\n\n"
					  "The best practice would be to use constraint students set not available times.\n\n"
					  "If you need weight under 100%, then you can use this constraint, but be careful"
					  " not to obtain an impossible timetable (if your students set is constrained on gaps or early gaps"
					  " and if you leave a not allowed slot between 2 allowed slots (or a not allowed slot before allowed slots), this possible"
					  " gap might be counted in students' timetable)")
					  +"\n\n"+tr("Do you want to add current constraint?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				QList<int> days_L;
				QList<int> hours_L;
				getTimesTable(timesTable, days_L, hours_L, false);
				int n=days_L.count();
				assert(n==hours_L.count());

				if(n<=0){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("Warning: 0 slots selected. Are you sure?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				tc=new ConstraintActivitiesPreferredStartingTimes(weight, teacher, students, subject, activityTag, duration, n, days_L, hours_L);

				break;
			}
		//35
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}

				tc=new ConstraintActivitiesSameStartingHour(weight, selectedActivitiesList.count(), selectedActivitiesList);

				break;
			}
		//36
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}

				tc=new ConstraintActivitiesSameStartingDay(weight, selectedActivitiesList.count(), selectedActivitiesList);

				break;
			}
		//37
		case CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE:
			{
				assert(first_activitiesComboBox!=nullptr);
				int i=first_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid first activity"));
					return;
				}
				assert(i<first_activitiesList.count());
				int fid=first_activitiesList.at(i);

				assert(second_activitiesComboBox!=nullptr);
				i=second_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid second activity"));
					return;
				}
				assert(i<second_activitiesList.count());
				int sid=second_activitiesList.at(i);

				if(sid==fid){
					QMessageBox::warning(dialog, tr("FET information"), tr("Same activities"));
					return;
				}

				tc=new ConstraintTwoActivitiesConsecutive(weight, fid, sid);

				break;
			}
		//38
		case CONSTRAINT_TWO_ACTIVITIES_ORDERED:
			{
				assert(first_activitiesComboBox!=nullptr);
				int i=first_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid first activity"));
					return;
				}
				assert(i<first_activitiesList.count());
				int fid=first_activitiesList.at(i);

				assert(second_activitiesComboBox!=nullptr);
				i=second_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid second activity"));
					return;
				}
				assert(i<second_activitiesList.count());
				int sid=second_activitiesList.at(i);

				if(sid==fid){
					QMessageBox::warning(dialog, tr("FET information"), tr("Same activities"));
					return;
				}

				tc=new ConstraintTwoActivitiesOrdered(weight, fid, sid);

				break;
			}
		//39
		case CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}

				tc=new ConstraintMinGapsBetweenActivities(weight, selectedActivitiesList.count(), selectedActivitiesList, spinBox->value());

				break;
			}
		//40
		case CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS:
			{
				int duration=-1;
				if(durationCheckBox->isChecked()){
					assert(durationSpinBox->isEnabled());
					duration=durationSpinBox->value();
				}

				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				QList<int> days_L;
				QList<int> hours_L;
				getTimesTable(timesTable, days_L, hours_L, false);
				int n=days_L.count();
				assert(n==hours_L.count());

				if(n<=0){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("Warning: 0 slots selected. Are you sure?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				tc=new ConstraintSubactivitiesPreferredTimeSlots(weight, splitIndexSpinBox->value(), teacher, students, subject, activityTag, duration, n, days_L, hours_L);

				break;
			}
		//41
		case CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES:
			{
				int duration=-1;
				if(durationCheckBox->isChecked()){
					assert(durationSpinBox->isEnabled());
					duration=durationSpinBox->value();
				}

				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				QList<int> days_L;
				QList<int> hours_L;
				getTimesTable(timesTable, days_L, hours_L, false);
				int n=days_L.count();
				assert(n==hours_L.count());

				if(n<=0){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("Warning: 0 slots selected. Are you sure?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				tc=new ConstraintSubactivitiesPreferredStartingTimes(weight, splitIndexSpinBox->value(), teacher, students, subject, activityTag, duration, n, days_L, hours_L);

				break;
			}
		//42
		case CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				tc=new ConstraintTeacherIntervalMaxDaysPerWeek(weight, spinBox->value(), teachersComboBox->currentText(), startHour, endHour);

				break;
			}
		//43
		case CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				tc=new ConstraintTeachersIntervalMaxDaysPerWeek(weight, spinBox->value(), startHour, endHour);

				break;
			}
		//44
		case CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				tc=new ConstraintStudentsSetIntervalMaxDaysPerWeek(weight, spinBox->value(), studentsComboBox->currentText(), startHour, endHour);

				break;
			}
		//45
		case CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				tc=new ConstraintStudentsIntervalMaxDaysPerWeek(weight, spinBox->value(), startHour, endHour);

				break;
			}
		//46
		case CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY:
			{
				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				tc=new ConstraintActivitiesEndStudentsDay(weight, teacher, students, subject, activityTag);

				break;
			}
		//47
		case CONSTRAINT_TWO_ACTIVITIES_GROUPED:
			{
				assert(first_activitiesComboBox!=nullptr);
				int i=first_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid first activity"));
					return;
				}
				assert(i<first_activitiesList.count());
				int fid=first_activitiesList.at(i);

				assert(second_activitiesComboBox!=nullptr);
				i=second_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid second activity"));
					return;
				}
				assert(i<second_activitiesList.count());
				int sid=second_activitiesList.at(i);

				if(sid==fid){
					QMessageBox::warning(dialog, tr("FET information"), tr("Same activities"));
					return;
				}

				tc=new ConstraintTwoActivitiesGrouped(weight, fid, sid);

				break;
			}
		//48
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				tc=new ConstraintTeachersActivityTagMaxHoursContinuously(weight, spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//49
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				tc=new ConstraintTeacherActivityTagMaxHoursContinuously(weight, spinBox->value(), teachersComboBox->currentText(), activityTagsComboBox->currentText());

				break;
			}
		//50
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				tc=new ConstraintStudentsActivityTagMaxHoursContinuously(weight, spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//51
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				tc=new ConstraintStudentsSetActivityTagMaxHoursContinuously(weight, spinBox->value(), studentsComboBox->currentText(), activityTagsComboBox->currentText());

				break;
			}
		//52
		case CONSTRAINT_TEACHERS_MAX_DAYS_PER_WEEK:
			{
				tc=new ConstraintTeachersMaxDaysPerWeek(weight, spinBox->value());

				break;
			}
		//53
		case CONSTRAINT_THREE_ACTIVITIES_GROUPED:
			{
				assert(first_activitiesComboBox!=nullptr);
				int i=first_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid first activity"));
					return;
				}
				assert(i<first_activitiesList.count());
				int fid=first_activitiesList.at(i);

				assert(second_activitiesComboBox!=nullptr);
				i=second_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid second activity"));
					return;
				}
				assert(i<second_activitiesList.count());
				int sid=second_activitiesList.at(i);

				assert(third_activitiesComboBox!=nullptr);
				i=third_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid third activity"));
					return;
				}
				assert(i<third_activitiesList.count());
				int tid=third_activitiesList.at(i);

				if(sid==fid || sid==tid || fid==tid){
					QMessageBox::warning(dialog, tr("FET information"), tr("Same activities"));
					return;
				}

				tc=new ConstraintThreeActivitiesGrouped(weight, fid, sid, tid);

				break;
			}
		//54
		case CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}

				tc=new ConstraintMaxDaysBetweenActivities(weight, selectedActivitiesList.count(), selectedActivitiesList, spinBox->value());

				break;
			}
		//55
		case CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK:
			{
				tc=new ConstraintTeachersMinDaysPerWeek(weight, spinBox->value());

				break;
			}
		//56
		case CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK:
			{
				tc=new ConstraintTeacherMinDaysPerWeek(weight, spinBox->value(), teachersComboBox->currentText());

				break;
			}
		//57
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				tc=new ConstraintTeachersActivityTagMaxHoursDaily(weight, spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//58
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				tc=new ConstraintTeacherActivityTagMaxHoursDaily(weight, spinBox->value(), teachersComboBox->currentText(), activityTagsComboBox->currentText());

				break;
			}
		//59
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				tc=new ConstraintStudentsActivityTagMaxHoursDaily(weight, spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//60
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				tc=new ConstraintStudentsSetActivityTagMaxHoursDaily(weight, spinBox->value(), studentsComboBox->currentText(), activityTagsComboBox->currentText());

				break;
			}
		//61
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_DAY:
			{
				tc=new ConstraintStudentsMaxGapsPerDay(weight, spinBox->value());

				break;
			}
		//62
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY:
			{
				tc=new ConstraintStudentsSetMaxGapsPerDay(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//63
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TIME_SLOTS_FROM_SELECTION:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("You specified max occupied time slots to be 0. This is "
					 "not perfect from efficiency point of view, because you can use instead constraint activity(ies) preferred time slots, "
					 "and help FET to find a timetable easier and faster, with an equivalent result. Please correct."));
					return;
				}

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				tc=new ConstraintActivitiesOccupyMaxTimeSlotsFromSelection(weight, selectedActivitiesList, days, hours, spinBox->value());

				break;
			}
		//64
		case CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("You specified max simultaneous activities to be 0. This is "
					 "not perfect from efficiency point of view, because you can use instead constraint activity(ies) preferred time slots, "
					 "and help FET to find a timetable easier and faster, with an equivalent result. Please correct."));
					return;
				}

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				tc=new ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots(weight, selectedActivitiesList, days, hours, spinBox->value());

				break;
			}
		//65
		case CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK:
			{
				tc=new ConstraintStudentsSetMaxDaysPerWeek(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//66
		case CONSTRAINT_STUDENTS_MAX_DAYS_PER_WEEK:
			{
				tc=new ConstraintStudentsMaxDaysPerWeek(weight, spinBox->value());

				break;
			}
		//67
		case CONSTRAINT_TEACHER_MAX_SPAN_PER_DAY:
			{
				tc=new ConstraintTeacherMaxSpanPerDay(weight, spinBox->value(), checkBox->isChecked(), teachersComboBox->currentText());

				break;
			}
		//68
		case CONSTRAINT_TEACHERS_MAX_SPAN_PER_DAY:
			{
				tc=new ConstraintTeachersMaxSpanPerDay(weight, spinBox->value(), checkBox->isChecked());

				break;
			}
		//69
		case CONSTRAINT_TEACHER_MIN_RESTING_HOURS:
			{
				tc=new ConstraintTeacherMinRestingHours(weight, spinBox->value(), checkBox->isChecked(), teachersComboBox->currentText());

				break;
			}
		//70
		case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS:
			{
				tc=new ConstraintTeachersMinRestingHours(weight, spinBox->value(), checkBox->isChecked());

				break;
			}
		//71
		case CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_DAY:
			{
				tc=new ConstraintStudentsSetMaxSpanPerDay(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//72
		case CONSTRAINT_STUDENTS_MAX_SPAN_PER_DAY:
			{
				tc=new ConstraintStudentsMaxSpanPerDay(weight, spinBox->value());

				break;
			}
		//73
		case CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS:
			{
				tc=new ConstraintStudentsSetMinRestingHours(weight, spinBox->value(), checkBox->isChecked(), studentsComboBox->currentText());

				break;
			}
		//74
		case CONSTRAINT_STUDENTS_MIN_RESTING_HOURS:
			{
				tc=new ConstraintStudentsMinRestingHours(weight, spinBox->value(), checkBox->isChecked());

				break;
			}
		//75
		case CONSTRAINT_TWO_ACTIVITIES_ORDERED_IF_SAME_DAY:
			{
				assert(first_activitiesComboBox!=nullptr);
				int i=first_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid first activity"));
					return;
				}
				assert(i<first_activitiesList.count());
				int fid=first_activitiesList.at(i);

				assert(second_activitiesComboBox!=nullptr);
				i=second_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid second activity"));
					return;
				}
				assert(i<second_activitiesList.count());
				int sid=second_activitiesList.at(i);

				if(sid==fid){
					QMessageBox::warning(dialog, tr("FET information"), tr("Same activities"));
					return;
				}

				tc=new ConstraintTwoActivitiesOrderedIfSameDay(weight, fid, sid);

				break;
			}
		//76
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				tc=new ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags(weight, studentsComboBox->currentText(), spinBox->value(), firstActivityTagName, secondActivityTagName);

				break;
			}
		//77
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				tc=new ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags(weight, spinBox->value(), firstActivityTagName, secondActivityTagName);

				break;
			}
		//78
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				tc=new ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags(weight, teachersComboBox->currentText(), spinBox->value(), firstActivityTagName, secondActivityTagName);

				break;
			}
		//79
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				tc=new ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags(weight, spinBox->value(), firstActivityTagName, secondActivityTagName);

				break;
			}
		//80
		case CONSTRAINT_ACTIVITY_TAGS_NOT_OVERLAPPING:
			{
				if(selectedActivityTagsListWidget->count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of not overlapping activity tags"));
					return;
				}
				if(selectedActivityTagsListWidget->count()==1){
					QMessageBox::warning(dialog, tr("FET information"), tr("Only one selected activity tag"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				tc=new ConstraintActivityTagsNotOverlapping(weight, atl);

				break;
			}
		//81
		case CONSTRAINT_ACTIVITIES_OCCUPY_MIN_TIME_SLOTS_FROM_SELECTION:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("You specified min occupied time slots to be 0. This results in "
																		 "a useless constraint. Please correct."));
					return;
				}

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				tc=new ConstraintActivitiesOccupyMinTimeSlotsFromSelection(weight, selectedActivitiesList, days, hours, spinBox->value());

				break;
			}
		//82
		case CONSTRAINT_ACTIVITIES_MIN_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("You specified min simultaneous activities to be 0. This results in "
					 "a useless constraint. Please correct."));
					return;
				}

				if(spinBox->value()==1 && allowEmptySlotsCheckBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("You specified min simultaneous activities to be 1 and you allow empty slots. "
					 "This results in a useless constraint. Please correct."));
					return;
				}

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				tc=new ConstraintActivitiesMinSimultaneousInSelectedTimeSlots(weight, selectedActivitiesList, days, hours, spinBox->value(), allowEmptySlotsCheckBox->isChecked());

				break;
			}
		//83
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				/*if(checkBox->isChecked() && spinBox->value()==1){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Allow empty days is selected and min hours daily is 1, "
																	   "so this would be a useless constraint."));
					return;
				}*/

				tc=new ConstraintTeachersActivityTagMinHoursDaily(weight, spinBox->value(), secondSpinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//84
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				/*if(checkBox->isChecked() && spinBox->value()==1){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Allow empty days is selected and min hours daily is 1, "
																	   "so this would be a useless constraint."));
					return;
				}*/

				tc=new ConstraintTeacherActivityTagMinHoursDaily(weight, spinBox->value(), secondSpinBox->value(), teachersComboBox->currentText(), activityTagsComboBox->currentText());

				break;
			}
		//85
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				/*if(checkBox->isChecked() && spinBox->value()==1){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Allow empty days is selected and min hours daily is 1, "
																	   "so this would be a useless constraint."));
					return;
				}*/

				tc=new ConstraintStudentsActivityTagMinHoursDaily(weight, spinBox->value(), secondSpinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//86
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				/*if(checkBox->isChecked() && spinBox->value()==1){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Allow empty days is selected and min hours daily is 1, "
																	   "so this would be a useless constraint."));
					return;
				}*/

				tc=new ConstraintStudentsSetActivityTagMinHoursDaily(weight, spinBox->value(), secondSpinBox->value(), studentsComboBox->currentText(), activityTagsComboBox->currentText());

				break;
			}
		//87
		case CONSTRAINT_ACTIVITY_ENDS_TEACHERS_DAY:
			{
				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				tc=new ConstraintActivityEndsTeachersDay(weight, id);

				break;
			}
		//88
		case CONSTRAINT_ACTIVITIES_END_TEACHERS_DAY:
			{
				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				tc=new ConstraintActivitiesEndTeachersDay(weight, teacher, students, subject, activityTag);

				break;
			}
		//89
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_REAL_DAYS:
			{
				tc=new ConstraintTeachersMaxHoursDailyRealDays(weight, spinBox->value());

				break;
			}
		//90
		case CONSTRAINT_TEACHER_MAX_REAL_DAYS_PER_WEEK:
			{
				tc=new ConstraintTeacherMaxRealDaysPerWeek(weight, spinBox->value(), teachersComboBox->currentText());

				break;
			}
		//91
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS:
			{
				tc=new ConstraintTeacherMaxHoursDailyRealDays(weight, spinBox->value(), teachersComboBox->currentText());

				break;
			}
		//92
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_REAL_DAYS:
			{
				tc=new ConstraintStudentsMaxHoursDailyRealDays(weight, spinBox->value());

				break;
			}
		//93
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_REAL_DAYS:
			{
				tc=new ConstraintStudentsSetMaxHoursDailyRealDays(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//94
		case CONSTRAINT_TEACHERS_MAX_REAL_DAYS_PER_WEEK:
			{
				tc=new ConstraintTeachersMaxRealDaysPerWeek(weight, spinBox->value());

				break;
			}
		//95
		case CONSTRAINT_TEACHERS_MIN_REAL_DAYS_PER_WEEK:
			{
				tc=new ConstraintTeachersMinRealDaysPerWeek(weight, spinBox->value());

				break;
			}
		//96
		case CONSTRAINT_TEACHER_MIN_REAL_DAYS_PER_WEEK:
			{
				tc=new ConstraintTeacherMinRealDaysPerWeek(weight, spinBox->value(), teachersComboBox->currentText());

				break;
			}
		//97
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				tc=new ConstraintTeachersActivityTagMaxHoursDailyRealDays(weight, spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//98
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				tc=new ConstraintTeacherActivityTagMaxHoursDailyRealDays(weight, spinBox->value(), teachersComboBox->currentText(), activityTagsComboBox->currentText());

				break;
			}
		//99
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				tc=new ConstraintStudentsActivityTagMaxHoursDailyRealDays(weight, spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//100
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				tc=new ConstraintStudentsSetActivityTagMaxHoursDailyRealDays(weight, spinBox->value(), studentsComboBox->currentText(), activityTagsComboBox->currentText());

				break;
			}
		//101
		case CONSTRAINT_TEACHER_MAX_AFTERNOONS_PER_WEEK:
			{
				tc=new ConstraintTeacherMaxAfternoonsPerWeek(weight, spinBox->value(), teachersComboBox->currentText());

				break;
			}
		//102
		case CONSTRAINT_TEACHERS_MAX_AFTERNOONS_PER_WEEK:
			{
				tc=new ConstraintTeachersMaxAfternoonsPerWeek(weight, spinBox->value());

				break;
			}
		//103
		case CONSTRAINT_TEACHER_MAX_MORNINGS_PER_WEEK:
			{
				tc=new ConstraintTeacherMaxMorningsPerWeek(weight, spinBox->value(), teachersComboBox->currentText());

				break;
			}
		//104
		case CONSTRAINT_TEACHERS_MAX_MORNINGS_PER_WEEK:
			{
				tc=new ConstraintTeachersMaxMorningsPerWeek(weight, spinBox->value());

				break;
			}
		//105
		case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				tc=new ConstraintTeacherMaxActivityTagsPerDayFromSet(weight, teachersComboBox->currentText(), spinBox->value(), atl);

				break;
			}
		//106
		case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				tc=new ConstraintTeachersMaxActivityTagsPerDayFromSet(weight, spinBox->value(), atl);

				break;
			}
		//107
		case CONSTRAINT_TEACHERS_MIN_MORNINGS_PER_WEEK:
			{
				tc=new ConstraintTeachersMinMorningsPerWeek(weight, spinBox->value());

				break;
			}
		//108
		case CONSTRAINT_TEACHER_MIN_MORNINGS_PER_WEEK:
			{
				tc=new ConstraintTeacherMinMorningsPerWeek(weight, spinBox->value(), teachersComboBox->currentText());

				break;
			}
		//109
		case CONSTRAINT_TEACHERS_MIN_AFTERNOONS_PER_WEEK:
			{
				tc=new ConstraintTeachersMinAfternoonsPerWeek(weight, spinBox->value());

				break;
			}
		//110
		case CONSTRAINT_TEACHER_MIN_AFTERNOONS_PER_WEEK:
			{
				tc=new ConstraintTeacherMinAfternoonsPerWeek(weight, spinBox->value(), teachersComboBox->currentText());

				break;
			}
		//111
		case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_MORNINGS:
			{
				tc=new ConstraintTeacherMaxTwoConsecutiveMornings(weight, teachersComboBox->currentText());

				break;
			}
		//112
		case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_MORNINGS:
			{
				tc=new ConstraintTeachersMaxTwoConsecutiveMornings(weight);

				break;
			}
		//113
		case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_AFTERNOONS:
			{
				tc=new ConstraintTeacherMaxTwoConsecutiveAfternoons(weight, teachersComboBox->currentText());

				break;
			}
		//114
		case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_AFTERNOONS:
			{
				tc=new ConstraintTeachersMaxTwoConsecutiveAfternoons(weight);

				break;
			}
		//115
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_REAL_DAY:
			{
				tc=new ConstraintTeachersMaxGapsPerRealDay(weight, spinBox->value(), checkBox->isChecked());

				break;
			}
		//116
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY:
			{
				tc=new ConstraintTeacherMaxGapsPerRealDay(weight, teachersComboBox->currentText(), spinBox->value(), checkBox->isChecked());

				break;
			}
		//117
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_REAL_DAY:
			{
				tc=new ConstraintStudentsMaxGapsPerRealDay(weight, spinBox->value());

				break;
			}
		//118
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_REAL_DAY:
			{
				tc=new ConstraintStudentsSetMaxGapsPerRealDay(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//119
		case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY_REAL_DAYS:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for the teachers, "
						"please use the constraint teachers min days per week"));
					return;
				}

				tc=new ConstraintTeachersMinHoursDailyRealDays(weight, spinBox->value(), checkBox->isChecked());

				break;
			}
		//120
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for this teacher, "
						"please use the constraint teacher min days per week"));
					return;
				}

				tc=new ConstraintTeacherMinHoursDailyRealDays(weight, spinBox->value(), teachersComboBox->currentText(), checkBox->isChecked());

				break;
			}
		//121
		case CONSTRAINT_TEACHERS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				tc=new ConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHour(weight, spinBox->value());

				break;
			}
		//122
		case CONSTRAINT_TEACHER_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				tc=new ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour(weight, spinBox->value(), teachersComboBox->currentText());

				break;
			}
		//123
		case CONSTRAINT_TEACHERS_MIN_HOURS_PER_MORNING:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty mornings check box must be checked. If you need to not allow empty mornings for the teachers, "
						"please use the constraint teachers min mornings per week."));
					return;
				}

				tc=new ConstraintTeachersMinHoursPerMorning(weight, spinBox->value(), checkBox->isChecked());

				break;
			}
		//124
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty mornings check box must be checked. If you need to not allow empty mornings for a teacher, "
						"please use the constraint teacher min mornings per week."));
					return;
				}

				tc=new ConstraintTeacherMinHoursPerMorning(weight, spinBox->value(), teachersComboBox->currentText(), checkBox->isChecked());

				break;
			}
		//125
		case CONSTRAINT_TEACHER_MAX_SPAN_PER_REAL_DAY:
			{
				tc=new ConstraintTeacherMaxSpanPerRealDay(weight, spinBox->value(), checkBox->isChecked(), teachersComboBox->currentText());

				break;
			}
		//126
		case CONSTRAINT_TEACHERS_MAX_SPAN_PER_REAL_DAY:
			{
				tc=new ConstraintTeachersMaxSpanPerRealDay(weight, spinBox->value(), checkBox->isChecked());

				break;
			}
		//127
		case CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_REAL_DAY:
			{
				tc=new ConstraintStudentsSetMaxSpanPerRealDay(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//128
		case CONSTRAINT_STUDENTS_MAX_SPAN_PER_REAL_DAY:
			{
				tc=new ConstraintStudentsMaxSpanPerRealDay(weight, spinBox->value());

				break;
			}
		//129
		case CONSTRAINT_TEACHER_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				tc=new ConstraintTeacherMorningIntervalMaxDaysPerWeek(weight, spinBox->value(), teachersComboBox->currentText(), startHour, endHour);

				break;
			}
		//130
		case CONSTRAINT_TEACHERS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				tc=new ConstraintTeachersMorningIntervalMaxDaysPerWeek(weight, spinBox->value(), startHour, endHour);

				break;
			}
		//131
		case CONSTRAINT_TEACHER_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				tc=new ConstraintTeacherAfternoonIntervalMaxDaysPerWeek(weight, spinBox->value(), teachersComboBox->currentText(), startHour, endHour);

				break;
			}
		//132
		case CONSTRAINT_TEACHERS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				tc=new ConstraintTeachersAfternoonIntervalMaxDaysPerWeek(weight, spinBox->value(), startHour, endHour);

				break;
			}
		//133
		case CONSTRAINT_STUDENTS_MIN_HOURS_PER_MORNING:
			{
				//if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					//2021-03-26 - I think I commented out this check because the user might combine this constraint with a min hours daily constraint.
					/*if(allowEmptyMorningsCheckBox->isChecked() && minHoursSpinBox->value()<2){
						QMessageBox::warning(dialog, tr("FET warning"), tr("If you allow empty mornings, the min hours must be at least 2 (to make it a non-trivial constraint)"));
						return;
					}*/
				//}

				tc=new ConstraintStudentsMinHoursPerMorning(weight, spinBox->value(), checkBox->isChecked());

				break;
			}
		//134
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_MORNING:
			{
				//if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					//2021-03-26 - I think I commented out this check because the user might combine this constraint with a min hours daily constraint.
					/*if(allowEmptyMorningsCheckBox->isChecked() && minHoursSpinBox->value()<2){
						QMessageBox::warning(dialog, tr("FET warning"), tr("If you allow empty mornings, the min hours must be at least 2 (to make it a non-trivial constraint)"));
						return;
					}*/
				//}

				tc=new ConstraintStudentsSetMinHoursPerMorning(weight, spinBox->value(), studentsComboBox->currentText(), checkBox->isChecked());

				break;
			}
		//135
		case CONSTRAINT_TEACHER_MAX_ZERO_GAPS_PER_AFTERNOON:
			{
				tc=new ConstraintTeacherMaxZeroGapsPerAfternoon(weight, teachersComboBox->currentText());

				break;
			}
		//136
		case CONSTRAINT_TEACHERS_MAX_ZERO_GAPS_PER_AFTERNOON:
			{
				tc=new ConstraintTeachersMaxZeroGapsPerAfternoon(weight);

				break;
			}
		//137
		case CONSTRAINT_STUDENTS_SET_MAX_AFTERNOONS_PER_WEEK:
			{
				tc=new ConstraintStudentsSetMaxAfternoonsPerWeek(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//138
		case CONSTRAINT_STUDENTS_MAX_AFTERNOONS_PER_WEEK:
			{
				tc=new ConstraintStudentsMaxAfternoonsPerWeek(weight, spinBox->value());

				break;
			}
		//139
		case CONSTRAINT_STUDENTS_SET_MAX_MORNINGS_PER_WEEK:
			{
				tc=new ConstraintStudentsSetMaxMorningsPerWeek(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//140
		case CONSTRAINT_STUDENTS_MAX_MORNINGS_PER_WEEK:
			{
				tc=new ConstraintStudentsMaxMorningsPerWeek(weight, spinBox->value());

				break;
			}
		//141
		case CONSTRAINT_STUDENTS_MIN_MORNINGS_PER_WEEK:
			{
				tc=new ConstraintStudentsMinMorningsPerWeek(weight, spinBox->value());

				break;
			}
		//142
		case CONSTRAINT_STUDENTS_SET_MIN_MORNINGS_PER_WEEK:
			{
				tc=new ConstraintStudentsSetMinMorningsPerWeek(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//143
		case CONSTRAINT_STUDENTS_MIN_AFTERNOONS_PER_WEEK:
			{
				tc=new ConstraintStudentsMinAfternoonsPerWeek(weight, spinBox->value());

				break;
			}
		//144
		case CONSTRAINT_STUDENTS_SET_MIN_AFTERNOONS_PER_WEEK:
			{
				tc=new ConstraintStudentsSetMinAfternoonsPerWeek(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//145
		case CONSTRAINT_STUDENTS_SET_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				tc=new ConstraintStudentsSetMorningIntervalMaxDaysPerWeek(weight, spinBox->value(), studentsComboBox->currentText(), startHour, endHour);

				break;
			}
		//146
		case CONSTRAINT_STUDENTS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				tc=new ConstraintStudentsMorningIntervalMaxDaysPerWeek(weight, spinBox->value(), startHour, endHour);

				break;
			}
		//147
		case CONSTRAINT_STUDENTS_SET_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				tc=new ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek(weight, spinBox->value(), studentsComboBox->currentText(), startHour, endHour);

				break;
			}
		//148
		case CONSTRAINT_STUDENTS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				tc=new ConstraintStudentsAfternoonIntervalMaxDaysPerWeek(weight, spinBox->value(), startHour, endHour);

				break;
			}
		//149
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				tc=new ConstraintTeacherMaxHoursPerAllAfternoons(weight, spinBox->value(), teachersComboBox->currentText());

				break;
			}
		//150
		case CONSTRAINT_TEACHERS_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				tc=new ConstraintTeachersMaxHoursPerAllAfternoons(weight, spinBox->value());

				break;
			}
		//151
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				tc=new ConstraintStudentsSetMaxHoursPerAllAfternoons(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//152
		case CONSTRAINT_STUDENTS_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				tc=new ConstraintStudentsMaxHoursPerAllAfternoons(weight, spinBox->value());

				break;
			}
		//153
		case CONSTRAINT_TEACHER_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				tc=new ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon(weight, spinBox->value(), teachersComboBox->currentText());

				break;
			}
		//154
		case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				tc=new ConstraintTeachersMinRestingHoursBetweenMorningAndAfternoon(weight, spinBox->value());

				break;
			}
		//155
		case CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				tc=new ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//156
		case CONSTRAINT_STUDENTS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				tc=new ConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon(weight, spinBox->value());

				break;
			}
		//157
		case CONSTRAINT_STUDENTS_SET_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				tc=new ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//158
		case CONSTRAINT_STUDENTS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				tc=new ConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHour(weight, spinBox->value());

				break;
			}
		//159
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				tc=new ConstraintTeachersMaxGapsPerWeekForRealDays(weight, spinBox->value());

				break;
			}
		//160
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				tc=new ConstraintTeacherMaxGapsPerWeekForRealDays(weight, teachersComboBox->currentText(), spinBox->value());

				break;
			}
		//161
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				tc=new ConstraintStudentsMaxGapsPerWeekForRealDays(weight, spinBox->value());

				break;
			}
		//162
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				tc=new ConstraintStudentsSetMaxGapsPerWeekForRealDays(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//163
		case CONSTRAINT_STUDENTS_SET_MAX_REAL_DAYS_PER_WEEK:
			{
				tc=new ConstraintStudentsSetMaxRealDaysPerWeek(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//164
		case CONSTRAINT_STUDENTS_MAX_REAL_DAYS_PER_WEEK:
			{
				tc=new ConstraintStudentsMaxRealDaysPerWeek(weight, spinBox->value());

				break;
			}
		//165
		case CONSTRAINT_MAX_TOTAL_ACTIVITIES_FROM_SET_IN_SELECTED_TIME_SLOTS:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("You specified max total activities to be 0. This is "
					 "not perfect from efficiency point of view, because you can use instead constraint activity(ies) preferred time slots, "
					 "and help FET to find a timetable easier and faster, with an equivalent result. Please correct."));
					return;
				}

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				tc=new ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots(weight, selectedActivitiesList, days, hours, spinBox->value());

				break;
			}
		//166
		case CONSTRAINT_MAX_GAPS_BETWEEN_ACTIVITIES:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}

				tc=new ConstraintMaxGapsBetweenActivities(weight, selectedActivitiesList.count(), selectedActivitiesList, spinBox->value());

				break;
			}
		//167
		case CONSTRAINT_ACTIVITIES_MAX_IN_A_TERM:
			{
				if(this->selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
					 tr("Empty list of activities"));
					return;
				}
				//we allow even only one activity
				/*if(this->selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
					 tr("Only one selected activity"));
					return;
				}*/

				tc=new ConstraintActivitiesMaxInATerm(weight, selectedActivitiesList, spinBox->value());

				break;
			}
		//168
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TERMS:
			{
				if(this->selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
					 tr("Empty list of activities"));
					return;
				}
				//we allow even only one activity
				/*if(this->selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
					 tr("Only one selected activity"));
					return;
				}*/

				tc=new ConstraintActivitiesOccupyMaxTerms(weight, selectedActivitiesList, spinBox->value());

				break;
			}
		//169
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			{
				tc=new ConstraintTeachersMaxGapsPerMorningAndAfternoon(weight, spinBox->value());

				break;
			}
		//170
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			{
				tc=new ConstraintTeacherMaxGapsPerMorningAndAfternoon(weight, teachersComboBox->currentText(), spinBox->value());

				break;
			}
		//171
		case CONSTRAINT_TEACHERS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				tc=new ConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHour(weight, spinBox->value());

				break;
			}
		//172
		case CONSTRAINT_TEACHER_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				tc=new ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour(weight, spinBox->value(), teachersComboBox->currentText());

				break;
			}
		//173
		case CONSTRAINT_STUDENTS_SET_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				tc=new ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour(weight, spinBox->value(), studentsComboBox->currentText());

				break;
			}
		//174
		case CONSTRAINT_STUDENTS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				tc=new ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour(weight, spinBox->value());

				break;
			}
		//175
		case CONSTRAINT_TWO_SETS_OF_ACTIVITIES_ORDERED:
			{
				if(selectedActivitiesList_TwoSetsOfActivities_1.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities in the first set"));
					return;
				}
				if(selectedActivitiesList_TwoSetsOfActivities_2.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities in the second set"));
					return;
				}

				tc=new ConstraintTwoSetsOfActivitiesOrdered(weight, selectedActivitiesList_TwoSetsOfActivities_1, selectedActivitiesList_TwoSetsOfActivities_2);

				break;
			}
		//176
		case CONSTRAINT_TEACHERS_MAX_THREE_CONSECUTIVE_DAYS:
			{
				tc=new ConstraintTeachersMaxThreeConsecutiveDays(weight, checkBox->isChecked());

				break;
			}
		//177
		case CONSTRAINT_TEACHER_MAX_THREE_CONSECUTIVE_DAYS:
			{
				tc=new ConstraintTeacherMaxThreeConsecutiveDays(weight, checkBox->isChecked(), teachersComboBox->currentText());

				break;
			}
		//178
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				tc=new ConstraintStudentsSetMinGapsBetweenActivityTag(weight, studentsComboBox->currentText(), spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//179
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				tc=new ConstraintStudentsMinGapsBetweenActivityTag(weight, spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//180
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				tc=new ConstraintTeacherMinGapsBetweenActivityTag(weight, teachersComboBox->currentText(), spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//181
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				tc=new ConstraintTeachersMinGapsBetweenActivityTag(weight, spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//182
		case CONSTRAINT_STUDENTS_MAX_THREE_CONSECUTIVE_DAYS:
			{
				tc=new ConstraintStudentsMaxThreeConsecutiveDays(weight, checkBox->isChecked());

				break;
			}
		//183
		case CONSTRAINT_STUDENTS_SET_MAX_THREE_CONSECUTIVE_DAYS:
			{
				tc=new ConstraintStudentsSetMaxThreeConsecutiveDays(weight, checkBox->isChecked(), studentsComboBox->currentText());

				break;
			}
		//184
		case CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES:
			{
				if(this->selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				if(this->selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}

				if(spinBox->value()<=0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Invalid number of min half days between activities - it must be greater than 0."));
					return;
				}

				tc=new ConstraintMinHalfDaysBetweenActivities(weight, checkBox->isChecked(), selectedActivitiesList.count(), selectedActivitiesList, spinBox->value());

				break;
			}
		//185 is activity preferred day, which is not in the interface
		//186
		case CONSTRAINT_ACTIVITIES_MIN_IN_A_TERM:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
					 tr("Empty list of activities"));
					return;
				}
				//we allow even only one activity
				/*if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
					 tr("Only one selected activity"));
					return;
				}*/

				if(checkBox->isChecked() && spinBox->value()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Min activities in a term is 1, and you allow empty terms, so this constraint would be useless."));
					return;
				}

				tc=new ConstraintActivitiesMinInATerm(weight, selectedActivitiesList, spinBox->value(), checkBox->isChecked());

				break;
			}
		//187
		case CONSTRAINT_MAX_TERMS_BETWEEN_ACTIVITIES:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}

				tc=new ConstraintMaxTermsBetweenActivities(weight, selectedActivitiesList.count(), selectedActivitiesList, spinBox->value());

				break;
			}
		//188
		case CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				tc=new ConstraintStudentsSetMaxActivityTagsPerDayFromSet(weight, studentsComboBox->currentText(), spinBox->value(), atl);

				break;
			}
		//189
		case CONSTRAINT_STUDENTS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				tc=new ConstraintStudentsMaxActivityTagsPerDayFromSet(weight, spinBox->value(), atl);

				break;
			}
		//190
		case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				tc=new ConstraintTeacherMaxActivityTagsPerRealDayFromSet(weight, teachersComboBox->currentText(), spinBox->value(), atl);

				break;
			}
		//191
		case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				tc=new ConstraintTeachersMaxActivityTagsPerRealDayFromSet(weight, spinBox->value(), atl);

				break;
			}
		//192
		case CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				tc=new ConstraintStudentsSetMaxActivityTagsPerRealDayFromSet(weight, studentsComboBox->currentText(), spinBox->value(), atl);

				break;
			}
		//193
		case CONSTRAINT_STUDENTS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				tc=new ConstraintStudentsMaxActivityTagsPerRealDayFromSet(weight, spinBox->value(), atl);

				break;
			}
		//194
		case CONSTRAINT_MAX_HALF_DAYS_BETWEEN_ACTIVITIES:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}

				tc=new ConstraintMaxHalfDaysBetweenActivities(weight, selectedActivitiesList.count(), selectedActivitiesList, spinBox->value());

				break;
			}
		//195
		case CONSTRAINT_ACTIVITY_BEGINS_STUDENTS_DAY:
			{
				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				tc=new ConstraintActivityBeginsStudentsDay(weight, id);

				break;
			}
		//196
		case CONSTRAINT_ACTIVITIES_BEGIN_STUDENTS_DAY:
			{
				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				tc=new ConstraintActivitiesBeginStudentsDay(weight, teacher, students, subject, activityTag);

				break;
			}
		//197
		case CONSTRAINT_ACTIVITY_BEGINS_TEACHERS_DAY:
			{
				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				tc=new ConstraintActivityBeginsTeachersDay(weight, id);

				break;
			}
		//198
		case CONSTRAINT_ACTIVITIES_BEGIN_TEACHERS_DAY:
			{
				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				tc=new ConstraintActivitiesBeginTeachersDay(weight, teacher, students, subject, activityTag);

				break;
			}
		//199
		case CONSTRAINT_TEACHERS_MIN_HOURS_PER_AFTERNOON:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty afternoons check box must be checked. If you need to not allow empty afternoons for the teachers, "
						"please use the constraint teachers min afternoons per week."));
					return;
				}

				tc=new ConstraintTeachersMinHoursPerAfternoon(weight, spinBox->value(), checkBox->isChecked());

				break;
			}
		//200
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty afternoons check box must be checked. If you need to not allow empty afternoons for a teacher, "
						"please use the constraint teacher min afternoons per week."));
					return;
				}

				tc=new ConstraintTeacherMinHoursPerAfternoon(weight, spinBox->value(), teachersComboBox->currentText(), checkBox->isChecked());

				break;
			}
		//201
		case CONSTRAINT_STUDENTS_MIN_HOURS_PER_AFTERNOON:
			{
				//if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					//2021-03-26 - I think I commented out this check because the user might combine this constraint with a min hours daily constraint.
					/*if(allowEmptyAfternoonsCheckBox->isChecked() && minHoursSpinBox->value()<2){
						QMessageBox::warning(dialog, tr("FET warning"), tr("If you allow empty afternoons, the min hours must be at least 2 (to make it a non-trivial constraint)"));
						return;
					}*/
				//}

				tc=new ConstraintStudentsMinHoursPerAfternoon(weight, spinBox->value(), checkBox->isChecked());

				break;
			}
		//202
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_AFTERNOON:
			{
				//if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					//2021-03-26 - I think I commented out this check because the user might combine this constraint with a min hours daily constraint.
					/*if(allowEmptyAfternoonsCheckBox->isChecked() && minHoursSpinBox->value()<2){
						QMessageBox::warning(dialog, tr("FET warning"), tr("If you allow empty afternoons, the min hours must be at least 2 (to make it a non-trivial constraint)"));
						return;
					}*/
				//}

				tc=new ConstraintStudentsSetMinHoursPerAfternoon(weight, spinBox->value(), studentsComboBox->currentText(), checkBox->isChecked());

				break;
			}
		//203
		case CONSTRAINT_ACTIVITIES_MAX_HOURLY_SPAN:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}

				tc=new ConstraintActivitiesMaxHourlySpan(weight, selectedActivitiesList.count(), selectedActivitiesList, spinBox->value());

				break;
			}
		//204
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("If you need the maximum hours daily in the interval to be 0, please use"
						 " the constraints of type teacher not available times or activities preferred time slots."));
					return;
				}

				tc=new ConstraintTeachersMaxHoursDailyInInterval(weight, spinBox->value(), startHour, endHour);

				break;
			}
		//205
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("If you need the maximum hours daily in the interval to be 0, please use"
						 " the constraints of type teacher not available times or activities preferred time slots."));
					return;
				}

				tc=new ConstraintTeacherMaxHoursDailyInInterval(weight, spinBox->value(), teachersComboBox->currentText(), startHour, endHour);

				break;
			}
		//206
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("If you need the maximum hours daily in the interval to be 0, please use"
						 " the constraints of type students set not available times or activities preferred time slots."));
					return;
				}

				tc=new ConstraintStudentsMaxHoursDailyInInterval(weight, spinBox->value(), startHour, endHour);

				break;
			}
		//207
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("If you need the maximum hours daily in the interval to be 0, please use"
						 " the constraints of type students set not available times or activities preferred time slots."));
					return;
				}

				tc=new ConstraintStudentsSetMaxHoursDailyInInterval(weight, spinBox->value(), studentsComboBox->currentText(), startHour, endHour);

				break;
			}
		//208
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				tc=new ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay(weight, studentsComboBox->currentText(), spinBox->value(), firstActivityTagName, secondActivityTagName);

				break;
			}
		//209
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				tc=new ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay(weight, spinBox->value(), firstActivityTagName, secondActivityTagName);

				break;
			}
		//210
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				tc=new ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay(weight, teachersComboBox->currentText(), spinBox->value(), firstActivityTagName, secondActivityTagName);

				break;
			}
		//211
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				tc=new ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay(weight, spinBox->value(), firstActivityTagName, secondActivityTagName);

				break;
			}
		//212
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				tc=new ConstraintStudentsSetMinGapsBetweenActivityTagPerRealDay(weight, studentsComboBox->currentText(), spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//213
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				tc=new ConstraintStudentsMinGapsBetweenActivityTagPerRealDay(weight, spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//214
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				tc=new ConstraintTeacherMinGapsBetweenActivityTagPerRealDay(weight, teachersComboBox->currentText(), spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//215
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				tc=new ConstraintTeachersMinGapsBetweenActivityTagPerRealDay(weight, spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//216
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				tc=new ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(weight, studentsComboBox->currentText(), spinBox->value(), firstActivityTagName, secondActivityTagName);

				break;
			}
		//217
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				tc=new ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(weight, spinBox->value(), firstActivityTagName, secondActivityTagName);

				break;
			}
		//218
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				tc=new ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(weight, teachersComboBox->currentText(), spinBox->value(), firstActivityTagName, secondActivityTagName);

				break;
			}
		//219
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				tc=new ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(weight, spinBox->value(), firstActivityTagName, secondActivityTagName);

				break;
			}
		//220
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				tc=new ConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon(weight, studentsComboBox->currentText(), spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//221
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				tc=new ConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon(weight, spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//222
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				tc=new ConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon(weight, teachersComboBox->currentText(), spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//223
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				tc=new ConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon(weight, spinBox->value(), activityTagsComboBox->currentText());

				break;
			}
		//224
		case CONSTRAINT_TEACHERS_NO_TWO_CONSECUTIVE_DAYS:
			{
				tc=new ConstraintTeachersNoTwoConsecutiveDays(weight);

				break;
			}
		//225
		case CONSTRAINT_TEACHER_NO_TWO_CONSECUTIVE_DAYS:
			{
				tc=new ConstraintTeacherNoTwoConsecutiveDays(weight, teachersComboBox->currentText());

				break;
			}
		//226
		case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				int day1=firstDayComboBox->currentIndex();
				int hour1=firstHourComboBox->currentIndex();

				int day2=secondDayComboBox->currentIndex();
				int hour2=secondHourComboBox->currentIndex();

				if(day1<0 || day1>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's day invalid"));
					return;
				}
				if(hour1<0 || hour1>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's hour invalid"));
					return;
				}

				if(day2<0 || day2>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's day invalid"));
					return;
				}
				if(hour2<0 || hour2>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's hour invalid"));
					return;
				}

				if(day1==day2 && hour1==hour2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("The two time slots are the same", "It is a user error, the two time slots should not be equal"));
					return;
				}

				tc=new ConstraintTeacherPairOfMutuallyExclusiveTimeSlots(weight, teachersComboBox->currentText(), day1, hour1, day2, hour2);

				break;
			}
		//227
		case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				int day1=firstDayComboBox->currentIndex();
				int hour1=firstHourComboBox->currentIndex();

				int day2=secondDayComboBox->currentIndex();
				int hour2=secondHourComboBox->currentIndex();

				if(day1<0 || day1>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's day invalid"));
					return;
				}
				if(hour1<0 || hour1>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's hour invalid"));
					return;
				}

				if(day2<0 || day2>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's day invalid"));
					return;
				}
				if(hour2<0 || hour2>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's hour invalid"));
					return;
				}

				if(day1==day2 && hour1==hour2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("The two time slots are the same", "It is a user error, the two time slots should not be equal"));
					return;
				}

				tc=new ConstraintTeachersPairOfMutuallyExclusiveTimeSlots(weight, day1, hour1, day2, hour2);

				break;
			}
		//228
		case CONSTRAINT_STUDENTS_SET_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				int day1=firstDayComboBox->currentIndex();
				int hour1=firstHourComboBox->currentIndex();

				int day2=secondDayComboBox->currentIndex();
				int hour2=secondHourComboBox->currentIndex();

				if(day1<0 || day1>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's day invalid"));
					return;
				}
				if(hour1<0 || hour1>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's hour invalid"));
					return;
				}

				if(day2<0 || day2>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's day invalid"));
					return;
				}
				if(hour2<0 || hour2>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's hour invalid"));
					return;
				}

				if(day1==day2 && hour1==hour2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("The two time slots are the same", "It is a user error, the two time slots should not be equal"));
					return;
				}

				tc=new ConstraintStudentsSetPairOfMutuallyExclusiveTimeSlots(weight, studentsComboBox->currentText(), day1, hour1, day2, hour2);

				break;
			}
		//229
		case CONSTRAINT_STUDENTS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				int day1=firstDayComboBox->currentIndex();
				int hour1=firstHourComboBox->currentIndex();

				int day2=secondDayComboBox->currentIndex();
				int hour2=secondHourComboBox->currentIndex();

				if(day1<0 || day1>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's day invalid"));
					return;
				}
				if(hour1<0 || hour1>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's hour invalid"));
					return;
				}

				if(day2<0 || day2>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's day invalid"));
					return;
				}
				if(hour2<0 || hour2>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's hour invalid"));
					return;
				}

				if(day1==day2 && hour1==hour2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("The two time slots are the same", "It is a user error, the two time slots should not be equal"));
					return;
				}

				tc=new ConstraintStudentsPairOfMutuallyExclusiveTimeSlots(weight, day1, hour1, day2, hour2);

				break;
			}
		//230
		case CONSTRAINT_TWO_SETS_OF_ACTIVITIES_SAME_SECTIONS:
			{
				if(selectedActivitiesList_TwoSetsOfActivities_1.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities in the first set"));
					return;
				}
				if(selectedActivitiesList_TwoSetsOfActivities_2.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities in the second set"));
					return;
				}

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				tc=new ConstraintTwoSetsOfActivitiesSameSections(weight, selectedActivitiesList_TwoSetsOfActivities_1, selectedActivitiesList_TwoSetsOfActivities_2, days, hours);

				break;
			}
		//231
		case CONSTRAINT_STUDENTS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				tc=new ConstraintStudentsMaxSingleGapsInSelectedTimeSlots(weight, spinBox->value(), days, hours);

				break;
			}
		//232
		case CONSTRAINT_STUDENTS_SET_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				tc=new ConstraintStudentsSetMaxSingleGapsInSelectedTimeSlots(weight, studentsComboBox->currentText(), spinBox->value(), days, hours);

				break;
			}
		//233
		case CONSTRAINT_TEACHERS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				tc=new ConstraintTeachersMaxSingleGapsInSelectedTimeSlots(weight, spinBox->value(), days, hours);

				break;
			}
		//234
		case CONSTRAINT_TEACHER_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				tc=new ConstraintTeacherMaxSingleGapsInSelectedTimeSlots(weight, teachersComboBox->currentText(), spinBox->value(), days, hours);

				break;
			}
		//235
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_TERM:
			{
				tc=new ConstraintTeacherMaxHoursPerTerm(weight, spinBox->value(), teachersComboBox->currentText());

				break;
			}
		//236
		case CONSTRAINT_TEACHERS_MAX_HOURS_PER_TERM:
			{
				tc=new ConstraintTeachersMaxHoursPerTerm(weight, spinBox->value());

				break;
			}
		//237
		case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				QList<int> days1;
				QList<int> hours1;
				getTimesTable(timesTable1, days1, hours1, true);
				
				assert(days1.count()==hours1.count());
				if(days1.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the first set of time slots"));
					return;
				}

				QList<int> days2;
				QList<int> hours2;
				getTimesTable(timesTable2, days2, hours2, true);
				
				assert(days2.count()==hours2.count());
				if(days2.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the second set of time slots"));
					return;
				}
				
				QSet<QPair<int, int>> set1;
				QSet<QPair<int, int>> set2;
				for(int i=0; i<days1.count(); i++)
					set1.insert(QPair<int, int>(days1.at(i), hours1.at(i)));
				for(int i=0; i<days2.count(); i++)
					set2.insert(QPair<int, int>(days2.at(i), hours2.at(i)));
				if(set1.intersects(set2)){
					QSet<QPair<int, int>> intersectionSet = set1 & set2;
					QList<QPair<int, int>> intersectionList(intersectionSet.constBegin(), intersectionSet.constEnd());
					std::stable_sort(intersectionList.begin(), intersectionList.end());
					
					QStringList cts;
					for(const QPair<int, int>& pr : std::as_const(intersectionList))
						cts.append(gt.rules.daysOfTheWeek[pr.first]+QString(" ")+gt.rules.hoursOfTheDay[pr.second]);
					
					LongTextMessageBox::information(dialog, tr("FET information"),
						 tr("The two sets of selected time slots cannot have common time slots. The common time slots are: %1.")
						 .arg(cts.join(translatedCommaSpace())));
					return;
				}

				tc=new ConstraintTeacherPairOfMutuallyExclusiveSetsOfTimeSlots(weight, teachersComboBox->currentText(), days1, hours1, days2, hours2);

				break;
			}
		//238
		case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				QList<int> days1;
				QList<int> hours1;
				getTimesTable(timesTable1, days1, hours1, true);
				
				assert(days1.count()==hours1.count());
				if(days1.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the first set of time slots"));
					return;
				}

				QList<int> days2;
				QList<int> hours2;
				getTimesTable(timesTable2, days2, hours2, true);
				
				assert(days2.count()==hours2.count());
				if(days2.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the second set of time slots"));
					return;
				}

				QSet<QPair<int, int>> set1;
				QSet<QPair<int, int>> set2;
				for(int i=0; i<days1.count(); i++)
					set1.insert(QPair<int, int>(days1.at(i), hours1.at(i)));
				for(int i=0; i<days2.count(); i++)
					set2.insert(QPair<int, int>(days2.at(i), hours2.at(i)));
				if(set1.intersects(set2)){
					QSet<QPair<int, int>> intersectionSet = set1 & set2;
					QList<QPair<int, int>> intersectionList(intersectionSet.constBegin(), intersectionSet.constEnd());
					std::stable_sort(intersectionList.begin(), intersectionList.end());
					
					QStringList cts;
					for(const QPair<int, int>& pr : std::as_const(intersectionList))
						cts.append(gt.rules.daysOfTheWeek[pr.first]+QString(" ")+gt.rules.hoursOfTheDay[pr.second]);
					
					LongTextMessageBox::information(dialog, tr("FET information"),
						 tr("The two sets of selected time slots cannot have common time slots. The common time slots are: %1.")
						 .arg(cts.join(translatedCommaSpace())));
					return;
				}

				tc=new ConstraintTeachersPairOfMutuallyExclusiveSetsOfTimeSlots(weight, days1, hours1, days2, hours2);

				break;
			}
		//239
		case CONSTRAINT_STUDENTS_SET_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				QList<int> days1;
				QList<int> hours1;
				getTimesTable(timesTable1, days1, hours1, true);
				
				assert(days1.count()==hours1.count());
				if(days1.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the first set of time slots"));
					return;
				}

				QList<int> days2;
				QList<int> hours2;
				getTimesTable(timesTable2, days2, hours2, true);
				
				assert(days2.count()==hours2.count());
				if(days2.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the second set of time slots"));
					return;
				}

				QSet<QPair<int, int>> set1;
				QSet<QPair<int, int>> set2;
				for(int i=0; i<days1.count(); i++)
					set1.insert(QPair<int, int>(days1.at(i), hours1.at(i)));
				for(int i=0; i<days2.count(); i++)
					set2.insert(QPair<int, int>(days2.at(i), hours2.at(i)));
				if(set1.intersects(set2)){
					QSet<QPair<int, int>> intersectionSet = set1 & set2;
					QList<QPair<int, int>> intersectionList(intersectionSet.constBegin(), intersectionSet.constEnd());
					std::stable_sort(intersectionList.begin(), intersectionList.end());
					
					QStringList cts;
					for(const QPair<int, int>& pr : std::as_const(intersectionList))
						cts.append(gt.rules.daysOfTheWeek[pr.first]+QString(" ")+gt.rules.hoursOfTheDay[pr.second]);
					
					LongTextMessageBox::information(dialog, tr("FET information"),
						 tr("The two sets of selected time slots cannot have common time slots. The common time slots are: %1.")
						 .arg(cts.join(translatedCommaSpace())));
					return;
				}

				tc=new ConstraintStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots(weight, studentsComboBox->currentText(), days1, hours1, days2, hours2);

				break;
			}
		//240
		case CONSTRAINT_STUDENTS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				QList<int> days1;
				QList<int> hours1;
				getTimesTable(timesTable1, days1, hours1, true);
				
				assert(days1.count()==hours1.count());
				if(days1.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the first set of time slots"));
					return;
				}

				QList<int> days2;
				QList<int> hours2;
				getTimesTable(timesTable2, days2, hours2, true);
				
				assert(days2.count()==hours2.count());
				if(days2.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the second set of time slots"));
					return;
				}

				QSet<QPair<int, int>> set1;
				QSet<QPair<int, int>> set2;
				for(int i=0; i<days1.count(); i++)
					set1.insert(QPair<int, int>(days1.at(i), hours1.at(i)));
				for(int i=0; i<days2.count(); i++)
					set2.insert(QPair<int, int>(days2.at(i), hours2.at(i)));
				if(set1.intersects(set2)){
					QSet<QPair<int, int>> intersectionSet = set1 & set2;
					QList<QPair<int, int>> intersectionList(intersectionSet.constBegin(), intersectionSet.constEnd());
					std::stable_sort(intersectionList.begin(), intersectionList.end());
					
					QStringList cts;
					for(const QPair<int, int>& pr : std::as_const(intersectionList))
						cts.append(gt.rules.daysOfTheWeek[pr.first]+QString(" ")+gt.rules.hoursOfTheDay[pr.second]);
					
					LongTextMessageBox::information(dialog, tr("FET information"),
						 tr("The two sets of selected time slots cannot have common time slots. The common time slots are: %1.")
						 .arg(cts.join(translatedCommaSpace())));
					return;
				}

				tc=new ConstraintStudentsPairOfMutuallyExclusiveSetsOfTimeSlots(weight, days1, hours1, days2, hours2);

				break;
			}
		//241
		case CONSTRAINT_ACTIVITIES_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				QList<int> days1;
				QList<int> hours1;
				getTimesTable(timesTable1, days1, hours1, true);
				
				assert(days1.count()==hours1.count());
				if(days1.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the first set of time slots"));
					return;
				}

				QList<int> days2;
				QList<int> hours2;
				getTimesTable(timesTable2, days2, hours2, true);
				
				assert(days2.count()==hours2.count());
				if(days2.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the second set of time slots"));
					return;
				}

				QSet<QPair<int, int>> set1;
				QSet<QPair<int, int>> set2;
				for(int i=0; i<days1.count(); i++)
					set1.insert(QPair<int, int>(days1.at(i), hours1.at(i)));
				for(int i=0; i<days2.count(); i++)
					set2.insert(QPair<int, int>(days2.at(i), hours2.at(i)));
				if(set1.intersects(set2)){
					QSet<QPair<int, int>> intersectionSet = set1 & set2;
					QList<QPair<int, int>> intersectionList(intersectionSet.constBegin(), intersectionSet.constEnd());
					std::stable_sort(intersectionList.begin(), intersectionList.end());
					
					QStringList cts;
					for(const QPair<int, int>& pr : std::as_const(intersectionList))
						cts.append(gt.rules.daysOfTheWeek[pr.first]+QString(" ")+gt.rules.hoursOfTheDay[pr.second]);
					
					LongTextMessageBox::information(dialog, tr("FET information"),
						 tr("The two sets of selected time slots cannot have common time slots. The common time slots are: %1.")
						 .arg(cts.join(translatedCommaSpace())));
					return;
				}

				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				/*if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}*/

				tc=new ConstraintActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots(weight, selectedActivitiesList, days1, hours1, days2, hours2);

				break;
			}
		//242
		case CONSTRAINT_ACTIVITIES_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				int day1=firstDayComboBox->currentIndex();
				int hour1=firstHourComboBox->currentIndex();

				int day2=secondDayComboBox->currentIndex();
				int hour2=secondHourComboBox->currentIndex();

				if(day1<0 || day1>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's day invalid"));
					return;
				}
				if(hour1<0 || hour1>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's hour invalid"));
					return;
				}

				if(day2<0 || day2>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's day invalid"));
					return;
				}
				if(hour2<0 || hour2>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's hour invalid"));
					return;
				}

				if(day1==day2 && hour1==hour2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("The two time slots are the same", "It is a user error, the two time slots should not be equal"));
					return;
				}

				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				/*if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}*/

				tc=new ConstraintActivitiesPairOfMutuallyExclusiveTimeSlots(weight, selectedActivitiesList, day1, hour1, day2, hour2);

				break;
			}
		//243
		case CONSTRAINT_TEACHER_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				QList<QList<int>> days;
				QList<QList<int>> hours;
				getSpinBoxTimesTable(days, hours);
				
				assert(days.count()==hours.count());
				if(days.count()<=1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two sets of time slots."));
					return;
				}
				
				for(int i=0; i<days.count(); i++){
					const QList<int>& cdl=days.at(i);
					const QList<int>& chl=hours.at(i);
					assert(cdl.count()==chl.count());
					if(cdl.count()==0){
						QMessageBox::warning(dialog, tr("FET information"),
							tr("Set number %1 of selected time slots is empty - please correct this"
							 " (the selected numbers must start from 1, and they must be continuous, without any gap between them).").arg(i+1));
						return;
					}
				}

				tc=new ConstraintTeacherOccupiesMaxSetsOfTimeSlotsFromSelection(weight, teachersComboBox->currentText(), spinBox->value(), days, hours);

				break;
			}
		//244
		case CONSTRAINT_TEACHERS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				QList<QList<int>> days;
				QList<QList<int>> hours;
				getSpinBoxTimesTable(days, hours);
				
				assert(days.count()==hours.count());
				if(days.count()<=1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two sets of time slots."));
					return;
				}
				
				for(int i=0; i<days.count(); i++){
					const QList<int>& cdl=days.at(i);
					const QList<int>& chl=hours.at(i);
					assert(cdl.count()==chl.count());
					if(cdl.count()==0){
						QMessageBox::warning(dialog, tr("FET information"),
							tr("Set number %1 of selected time slots is empty - please correct this"
							 " (the selected numbers must start from 1, and they must be continuous, without any gap between them).").arg(i+1));
						return;
					}
				}

				tc=new ConstraintTeachersOccupyMaxSetsOfTimeSlotsFromSelection(weight, spinBox->value(), days, hours);

				break;
			}
		//245
		case CONSTRAINT_STUDENTS_SET_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				QList<QList<int>> days;
				QList<QList<int>> hours;
				getSpinBoxTimesTable(days, hours);
				
				assert(days.count()==hours.count());
				if(days.count()<=1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two sets of time slots."));
					return;
				}
				
				for(int i=0; i<days.count(); i++){
					const QList<int>& cdl=days.at(i);
					const QList<int>& chl=hours.at(i);
					assert(cdl.count()==chl.count());
					if(cdl.count()==0){
						QMessageBox::warning(dialog, tr("FET information"),
							tr("Set number %1 of selected time slots is empty - please correct this"
							 " (the selected numbers must start from 1, and they must be continuous, without any gap between them).").arg(i+1));
						return;
					}
				}

				tc=new ConstraintStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection(weight, studentsComboBox->currentText(), spinBox->value(), days, hours);

				break;
			}
		//246
		case CONSTRAINT_STUDENTS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				QList<QList<int>> days;
				QList<QList<int>> hours;
				getSpinBoxTimesTable(days, hours);
				
				assert(days.count()==hours.count());
				if(days.count()<=1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two sets of time slots."));
					return;
				}
				
				for(int i=0; i<days.count(); i++){
					const QList<int>& cdl=days.at(i);
					const QList<int>& chl=hours.at(i);
					assert(cdl.count()==chl.count());
					if(cdl.count()==0){
						QMessageBox::warning(dialog, tr("FET information"),
							tr("Set number %1 of selected time slots is empty - please correct this"
							 " (the selected numbers must start from 1, and they must be continuous, without any gap between them).").arg(i+1));
						return;
					}
				}

				tc=new ConstraintStudentsOccupyMaxSetsOfTimeSlotsFromSelection(weight, spinBox->value(), days, hours);

				break;
			}
		//247
		case CONSTRAINT_ACTIVITIES_OVERLAP_COMPLETELY_OR_DO_NOT_OVERLAP:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}
				
				int dur=-1;
				for(int actId : std::as_const(selectedActivitiesList)){
					Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
					assert(act!=nullptr);
					if(dur==-1 || dur==act->duration){
						dur=act->duration;
					}
					else{
						QMessageBox::warning(dialog, tr("FET information"),
							tr("All the selected activities should have the same duration"));
						return;
					}
				}
				if(dur==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("The constraint is useless, because the durations of the activities are 1"));
					return;
				}

				tc=new ConstraintActivitiesOverlapCompletelyOrDoNotOverlap(weight, selectedActivitiesList);

				break;
			}
		//248
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}

				QList<QList<int>> days;
				QList<QList<int>> hours;
				getSpinBoxTimesTable(days, hours);
				
				assert(days.count()==hours.count());
				if(days.count()<=1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two sets of time slots."));
					return;
				}
				
				for(int i=0; i<days.count(); i++){
					const QList<int>& cdl=days.at(i);
					const QList<int>& chl=hours.at(i);
					assert(cdl.count()==chl.count());
					if(cdl.count()==0){
						QMessageBox::warning(dialog, tr("FET information"),
							tr("Set number %1 of selected time slots is empty - please correct this"
							 " (the selected numbers must start from 1, and they must be continuous, without any gap between them).").arg(i+1));
						return;
					}
				}

				tc=new ConstraintActivitiesOccupyMaxSetsOfTimeSlotsFromSelection(weight, selectedActivitiesList, spinBox->value(), days, hours);

				break;
			}
		//249
		case CONSTRAINT_ACTIVITY_BEGINS_OR_ENDS_STUDENTS_DAY:
			{
				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				tc=new ConstraintActivityBeginsOrEndsStudentsDay(weight, id);

				break;
			}
		//250
		case CONSTRAINT_ACTIVITIES_BEGIN_OR_END_STUDENTS_DAY:
			{
				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				tc=new ConstraintActivitiesBeginOrEndStudentsDay(weight, teacher, students, subject, activityTag);

				break;
			}
		//251
		case CONSTRAINT_ACTIVITY_BEGINS_OR_ENDS_TEACHERS_DAY:
			{
				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				tc=new ConstraintActivityBeginsOrEndsTeachersDay(weight, id);

				break;
			}
		//252
		case CONSTRAINT_ACTIVITIES_BEGIN_OR_END_TEACHERS_DAY:
			{
				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				tc=new ConstraintActivitiesBeginOrEndTeachersDay(weight, teacher, students, subject, activityTag);

				break;
			}
		//253
		case CONSTRAINT_ACTIVITIES_MAX_TOTAL_NUMBER_OF_STUDENTS_IN_SELECTED_TIME_SLOTS:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("You specified the max total number of students to be 0. This is "
					 "not perfect from efficiency point of view, because you can use instead constraint activity(ies) preferred time slots, "
					 "and help FET to find a timetable easier and faster, with an equivalent result. Please correct."));
					return;
				}

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				tc=new ConstraintActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots(weight, selectedActivitiesList, days, hours, spinBox->value());

				break;
			}

		default:
			assert(0);
			break;
	}
	
	bool tmp2=gt.rules.addTimeConstraint(tc);
	if(tmp2){
		LongTextMessageBox::information(dialog, tr("FET information"), tr("Constraint added:")+"\n\n"+tc->getDetailedDescription(gt.rules));

		gt.rules.addUndoPoint(tr("Added the constraint:\n\n%1").arg(tc->getDetailedDescription(gt.rules)));

		if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
			LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
			LockUnlock::increaseCommunicationSpinBox();
		}
	}
	else{
		if(tc->type==CONSTRAINT_BASIC_COMPULSORY_TIME
				|| tc->type==CONSTRAINT_BREAK_TIMES)
			QMessageBox::warning(dialog, tr("FET information"), tr("Constraint NOT added - there must be another constraint of this type. Please edit that one"));
		else if(tc->type==CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES)
			QMessageBox::warning(dialog, tr("FET information"), tr("Constraint NOT added - there must be another constraint of this "
			 "type referring to the same teacher. Please edit that one"));
		else if(tc->type==CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES)
			QMessageBox::warning(dialog, tr("FET information"), tr("Constraint NOT added - there must be another constraint of this "
			 "type referring to the same students set. Please edit that one"));
		else if(tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
			QMessageBox::warning(dialog, tr("FET information"),	tr("Constraint NOT added - duplicate",
																   "'Duplicate' means that the constraint is identical to an already existing constraint and cannot be added."));
		else if(tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES
				|| tc->type==CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES)
			QMessageBox::warning(dialog, tr("FET information"),	tr("Constraint NOT added - it must be a duplicate",
																   "'Duplicate' means that the constraint is identical to an already existing constraint and cannot be added."));
		else
			QMessageBox::warning(dialog, tr("FET information"),	tr("Constraint NOT added - please report error"));
		
		delete tc;
	}
}

void AddOrModifyTimeConstraint::addConstraintsClicked()
{
	QMessageBox::StandardButton res=QMessageBox::question(dialog, tr("FET confirmation"),
	 tr("This operation will add multiple constraints, one for each teacher. Do you want to continue?"),
	 QMessageBox::Cancel | QMessageBox::Yes);
	if(res==QMessageBox::Cancel)
		return;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(dialog, tr("FET information"), tr("Invalid weight (percentage)"));
		return;
	}

	if(!timeConstraintCanHaveAnyWeight(type) && weight!=100.0){
		QMessageBox::warning(dialog, tr("FET information"), tr("Invalid weight (percentage) - it has to be 100%"));
		return;
	}

	if(type==CONSTRAINT_TEACHERS_MAX_HOURS_DAILY
			|| type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY
			|| type==CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_REAL_DAYS
			|| type==CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS){
		if(weight<100.0){
			int t=QMessageBox::warning(dialog, tr("FET warning"),
				tr("You selected a weight less than 100%. The generation algorithm is not perfectly optimized to work with such weights (even"
				 " if in practice it might work well). It is recommended to work only with 100% weights for these constraints. Are you sure you want to continue?"),
				 QMessageBox::Yes | QMessageBox::Cancel);
			if(t==QMessageBox::Cancel)
				return;
		}
	}

	if(!addEmpty && teachersComboBox!=nullptr){
		assert(filterGroupBox==nullptr);
		QString teacher_name=teachersComboBox->currentText();
		int teacher_ID=gt.rules.searchTeacher(teacher_name);
		if(teacher_ID<0){
			QMessageBox::warning(dialog, tr("FET information"), tr("Invalid teacher"));
			return;
		}
	}

	if(!addEmpty && activityTagsComboBox!=nullptr){
		assert(filterGroupBox==nullptr);
		QString activity_tag_name=activityTagsComboBox->currentText();
		int activityTag_ID=gt.rules.searchActivityTag(activity_tag_name);
		if(activityTag_ID<0){
			QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity tag"));
			return;
		}
	}

	QString ctrs;

	switch(type){
		//4
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY:
			[[fallthrough]];
		//8
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxHoursDaily(weight, spinBox->value(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}

		//5
		case CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK:
			[[fallthrough]];
		//52
		case CONSTRAINT_TEACHERS_MAX_DAYS_PER_WEEK:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxDaysPerWeek(weight, spinBox->value(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//6
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK:
			[[fallthrough]];
		//7
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK:
		{
			for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxGapsPerWeek(weight, tch->name, spinBox->value());
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
		}
		//9
		case CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY:
			[[fallthrough]];
		//10
		case CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxHoursContinuously(weight, spinBox->value(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//12
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY:
			[[fallthrough]];
		//11
		case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY:
			{
				if(!checkBox->isChecked()){
					if(gt.rules.mode!=MORNINGS_AFTERNOONS){
						QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for the teachers, "
							"please use the constraint teachers min days per week"));
						return;
					}
					else{
						QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for the teachers, "
							"please use the constraint teachers min days per week (but the min days per week constraint is for real days. You can also use the "
							"constraint teachers min mornings/afternoons per week.)"));
						return;
					}
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMinHoursDaily(weight, spinBox->value(), tch->name, checkBox->isChecked());
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//13
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY:
			[[fallthrough]];
		//14
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
						TimeConstraint *ctr=new ConstraintTeacherMaxGapsPerDay(weight, tch->name, spinBox->value());
						bool tmp2=gt.rules.addTimeConstraint(ctr);
						assert(tmp2);

						ctrs+=ctr->getDetailedDescription(gt.rules);
						ctrs+=QString("\n");
					}

					break;
			}
		//42
		case CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK:
			[[fallthrough]];
		//43
		case CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherIntervalMaxDaysPerWeek(weight, spinBox->value(), tch->name, startHour, endHour);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//48
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			[[fallthrough]];
		//49
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherActivityTagMaxHoursContinuously(weight, spinBox->value(), tch->name, activityTagsComboBox->currentText());
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//55
		case CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK:
			[[fallthrough]];
		//56
		case CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMinDaysPerWeek(weight, spinBox->value(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//57
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY:
			[[fallthrough]];
		//58
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherActivityTagMaxHoursDaily(weight, spinBox->value(), tch->name, activityTagsComboBox->currentText());
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//67
		case CONSTRAINT_TEACHER_MAX_SPAN_PER_DAY:
			[[fallthrough]];
		//68
		case CONSTRAINT_TEACHERS_MAX_SPAN_PER_DAY:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxSpanPerDay(weight, spinBox->value(), checkBox->isChecked(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//69
		case CONSTRAINT_TEACHER_MIN_RESTING_HOURS:
			[[fallthrough]];
		//70
		case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMinRestingHours(weight, spinBox->value(), checkBox->isChecked(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//78
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			[[fallthrough]];
		//79
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags(weight, tch->name, spinBox->value(), firstActivityTagName, secondActivityTagName);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//83
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MIN_HOURS_DAILY:
			[[fallthrough]];
		//84
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				/*if(checkBox->isChecked() && spinBox->value()==1){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Allow empty days is selected and min hours daily is 1, so "
																	   "this would be a useless constraint."));
					return;
				}*/

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherActivityTagMinHoursDaily(weight, spinBox->value(), secondSpinBox->value(), tch->name, activityTagsComboBox->currentText());
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//89
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_REAL_DAYS:
			[[fallthrough]];
		//91
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxHoursDailyRealDays(weight, spinBox->value(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//90
		case CONSTRAINT_TEACHER_MAX_REAL_DAYS_PER_WEEK:
			[[fallthrough]];
		//94
		case CONSTRAINT_TEACHERS_MAX_REAL_DAYS_PER_WEEK:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxRealDaysPerWeek(weight, spinBox->value(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//95
		case CONSTRAINT_TEACHERS_MIN_REAL_DAYS_PER_WEEK:
			[[fallthrough]];
		//96
		case CONSTRAINT_TEACHER_MIN_REAL_DAYS_PER_WEEK:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMinRealDaysPerWeek(weight, spinBox->value(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//97
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			[[fallthrough]];
		//98
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherActivityTagMaxHoursDailyRealDays(weight, spinBox->value(), tch->name, activityTagsComboBox->currentText());
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//101
		case CONSTRAINT_TEACHER_MAX_AFTERNOONS_PER_WEEK:
			[[fallthrough]];
		//102
		case CONSTRAINT_TEACHERS_MAX_AFTERNOONS_PER_WEEK:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxAfternoonsPerWeek(weight, spinBox->value(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//103
		case CONSTRAINT_TEACHER_MAX_MORNINGS_PER_WEEK:
			[[fallthrough]];
		//104
		case CONSTRAINT_TEACHERS_MAX_MORNINGS_PER_WEEK:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxMorningsPerWeek(weight, spinBox->value(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//105
		case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			[[fallthrough]];
		//106
		case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxActivityTagsPerDayFromSet(weight, tch->name, spinBox->value(), atl);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//107
		case CONSTRAINT_TEACHERS_MIN_MORNINGS_PER_WEEK:
			[[fallthrough]];
		//108
		case CONSTRAINT_TEACHER_MIN_MORNINGS_PER_WEEK:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMinMorningsPerWeek(weight, spinBox->value(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//109
		case CONSTRAINT_TEACHERS_MIN_AFTERNOONS_PER_WEEK:
			[[fallthrough]];
		//110
		case CONSTRAINT_TEACHER_MIN_AFTERNOONS_PER_WEEK:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMinAfternoonsPerWeek(weight, spinBox->value(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//111
		case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_MORNINGS:
			[[fallthrough]];
		//112
		case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_MORNINGS:
		{
			for(Teacher* tch : std::as_const(gt.rules.teachersList)){
				TimeConstraint *ctr=new ConstraintTeacherMaxTwoConsecutiveMornings(weight, tch->name);
				bool tmp2=gt.rules.addTimeConstraint(ctr);
				assert(tmp2);

				ctrs+=ctr->getDetailedDescription(gt.rules);
				ctrs+=QString("\n");
			}

			break;
		}
		//113
		case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_AFTERNOONS:
			[[fallthrough]];
		//114
		case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_AFTERNOONS:
		{
			for(Teacher* tch : std::as_const(gt.rules.teachersList)){
				TimeConstraint *ctr=new ConstraintTeacherMaxTwoConsecutiveAfternoons(weight, tch->name);
				bool tmp2=gt.rules.addTimeConstraint(ctr);
				assert(tmp2);

				ctrs+=ctr->getDetailedDescription(gt.rules);
				ctrs+=QString("\n");
			}

			break;
		}
		//115
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_REAL_DAY:
			[[fallthrough]];
		//116
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
						TimeConstraint *ctr=new ConstraintTeacherMaxGapsPerRealDay(weight, tch->name, spinBox->value(), checkBox->isChecked());
						bool tmp2=gt.rules.addTimeConstraint(ctr);
						assert(tmp2);

						ctrs+=ctr->getDetailedDescription(gt.rules);
						ctrs+=QString("\n");
					}

					break;
			}
		//119
		case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY_REAL_DAYS:
			[[fallthrough]];
		//120
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for the teachers, "
						"please use the constraint teachers min days per week"));
					return;
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMinHoursDailyRealDays(weight, spinBox->value(), tch->name, checkBox->isChecked());
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//121
		case CONSTRAINT_TEACHERS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			[[fallthrough]];
		//122
		case CONSTRAINT_TEACHER_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour(weight, spinBox->value(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//124
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING:
			[[fallthrough]];
		//123
		case CONSTRAINT_TEACHERS_MIN_HOURS_PER_MORNING:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty mornings check box must be checked. If you need to not allow empty mornings for the teachers, "
						"please use the constraint teachers min mornings per week."));
					return;
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMinHoursPerMorning(weight, spinBox->value(), tch->name, checkBox->isChecked());
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//125
		case CONSTRAINT_TEACHER_MAX_SPAN_PER_REAL_DAY:
			[[fallthrough]];
		//126
		case CONSTRAINT_TEACHERS_MAX_SPAN_PER_REAL_DAY:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxSpanPerRealDay(weight, spinBox->value(), checkBox->isChecked(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//129
		case CONSTRAINT_TEACHER_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			[[fallthrough]];
		//130
		case CONSTRAINT_TEACHERS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMorningIntervalMaxDaysPerWeek(weight, spinBox->value(), tch->name, startHour, endHour);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//131
		case CONSTRAINT_TEACHER_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			[[fallthrough]];
		//132
		case CONSTRAINT_TEACHERS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherAfternoonIntervalMaxDaysPerWeek(weight, spinBox->value(), tch->name, startHour, endHour);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//135
		case CONSTRAINT_TEACHER_MAX_ZERO_GAPS_PER_AFTERNOON:
			[[fallthrough]];
		//136
		case CONSTRAINT_TEACHERS_MAX_ZERO_GAPS_PER_AFTERNOON:
		{
			for(Teacher* tch : std::as_const(gt.rules.teachersList)){
				TimeConstraint *ctr=new ConstraintTeacherMaxZeroGapsPerAfternoon(weight, tch->name);
				bool tmp2=gt.rules.addTimeConstraint(ctr);
				assert(tmp2);

				ctrs+=ctr->getDetailedDescription(gt.rules);
				ctrs+=QString("\n");
			}

			break;
		}
		//149
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_ALL_AFTERNOONS:
			[[fallthrough]];
		//150
		case CONSTRAINT_TEACHERS_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxHoursPerAllAfternoons(weight, spinBox->value(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//153
		case CONSTRAINT_TEACHER_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			[[fallthrough]];
		//154
		case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon(weight, spinBox->value(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//159
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			[[fallthrough]];
		//160
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
		{
			for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxGapsPerWeekForRealDays(weight, tch->name, spinBox->value());
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
		}
		//169
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			[[fallthrough]];
		//170
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
		{
			for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxGapsPerMorningAndAfternoon(weight, tch->name, spinBox->value());
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
		}
		//171
		case CONSTRAINT_TEACHERS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			[[fallthrough]];
		//172
		case CONSTRAINT_TEACHER_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour(weight, spinBox->value(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//176
		case CONSTRAINT_TEACHERS_MAX_THREE_CONSECUTIVE_DAYS:
			[[fallthrough]];
		//177
		case CONSTRAINT_TEACHER_MAX_THREE_CONSECUTIVE_DAYS:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxThreeConsecutiveDays(weight, checkBox->isChecked(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//180
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			[[fallthrough]];
		//181
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMinGapsBetweenActivityTag(weight, tch->name, spinBox->value(), activityTagsComboBox->currentText());
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//190
		case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			[[fallthrough]];
		//191
		case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxActivityTagsPerRealDayFromSet(weight, tch->name, spinBox->value(), atl);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//200
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON:
			[[fallthrough]];
		//199
		case CONSTRAINT_TEACHERS_MIN_HOURS_PER_AFTERNOON:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty afternoons check box must be checked. If you need to not allow empty afternoons for the teachers, "
						"please use the constraint teachers min afternoons per week."));
					return;
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMinHoursPerAfternoon(weight, spinBox->value(), tch->name, checkBox->isChecked());
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//204
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_IN_INTERVAL:
			[[fallthrough]];
		//205
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("If you need the maximum hours daily in the interval to be 0, please use"
						 " the constraints of type teacher not available times or activities preferred time slots."));
					return;
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxHoursDailyInInterval(weight, spinBox->value(), tch->name, startHour, endHour);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//210
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			[[fallthrough]];
		//211
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay(weight, tch->name, spinBox->value(), firstActivityTagName, secondActivityTagName);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//214
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			[[fallthrough]];
		//215
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMinGapsBetweenActivityTagPerRealDay(weight, tch->name, spinBox->value(), activityTagsComboBox->currentText());
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//218
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			[[fallthrough]];
		//219
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon(weight, tch->name, spinBox->value(), firstActivityTagName, secondActivityTagName);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//222
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			[[fallthrough]];
		//223
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon(weight, tch->name, spinBox->value(), activityTagsComboBox->currentText());
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//224
		case CONSTRAINT_TEACHERS_NO_TWO_CONSECUTIVE_DAYS:
			[[fallthrough]];
		//225
		case CONSTRAINT_TEACHER_NO_TWO_CONSECUTIVE_DAYS:
		{
			for(Teacher* tch : std::as_const(gt.rules.teachersList)){
				TimeConstraint *ctr=new ConstraintTeacherNoTwoConsecutiveDays(weight, tch->name);
				bool tmp2=gt.rules.addTimeConstraint(ctr);
				assert(tmp2);

				ctrs+=ctr->getDetailedDescription(gt.rules);
				ctrs+=QString("\n");
			}

			break;
		}
		//226
		case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			[[fallthrough]];
		//227
		case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				int day1=firstDayComboBox->currentIndex();
				int hour1=firstHourComboBox->currentIndex();

				int day2=secondDayComboBox->currentIndex();
				int hour2=secondHourComboBox->currentIndex();

				if(day1<0 || day1>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's day invalid"));
					return;
				}
				if(hour1<0 || hour1>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's hour invalid"));
					return;
				}

				if(day2<0 || day2>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's day invalid"));
					return;
				}
				if(hour2<0 || hour2>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's hour invalid"));
					return;
				}

				if(day1==day2 && hour1==hour2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("The two time slots are the same", "It is a user error, the two time slots should not be equal"));
					return;
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherPairOfMutuallyExclusiveTimeSlots(weight, tch->name, day1, hour1, day2, hour2);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//233
		case CONSTRAINT_TEACHERS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			[[fallthrough]];
		//234
		case CONSTRAINT_TEACHER_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxSingleGapsInSelectedTimeSlots(weight, tch->name, spinBox->value(), days, hours);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//235
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_TERM:
			[[fallthrough]];
		//236
		case CONSTRAINT_TEACHERS_MAX_HOURS_PER_TERM:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherMaxHoursPerTerm(weight, spinBox->value(), tch->name);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//237
		case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			[[fallthrough]];
		//238
		case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				QList<int> days1;
				QList<int> hours1;
				getTimesTable(timesTable1, days1, hours1, true);
				
				assert(days1.count()==hours1.count());
				if(days1.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the first set of time slots"));
					return;
				}

				QList<int> days2;
				QList<int> hours2;
				getTimesTable(timesTable2, days2, hours2, true);
				
				assert(days2.count()==hours2.count());
				if(days2.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the second set of time slots"));
					return;
				}
				
				QSet<QPair<int, int>> set1;
				QSet<QPair<int, int>> set2;
				for(int i=0; i<days1.count(); i++)
					set1.insert(QPair<int, int>(days1.at(i), hours1.at(i)));
				for(int i=0; i<days2.count(); i++)
					set2.insert(QPair<int, int>(days2.at(i), hours2.at(i)));
				if(set1.intersects(set2)){
					QSet<QPair<int, int>> intersectionSet = set1 & set2;
					QList<QPair<int, int>> intersectionList(intersectionSet.constBegin(), intersectionSet.constEnd());
					std::stable_sort(intersectionList.begin(), intersectionList.end());
					
					QStringList cts;
					for(const QPair<int, int>& pr : std::as_const(intersectionList))
						cts.append(gt.rules.daysOfTheWeek[pr.first]+QString(" ")+gt.rules.hoursOfTheDay[pr.second]);
					
					LongTextMessageBox::information(dialog, tr("FET information"),
						 tr("The two sets of selected time slots cannot have common time slots. The common time slots are: %1.")
						 .arg(cts.join(translatedCommaSpace())));
					return;
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherPairOfMutuallyExclusiveSetsOfTimeSlots(weight, tch->name, days1, hours1, days2, hours2);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//243
		case CONSTRAINT_TEACHER_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			[[fallthrough]];
		//244
		case CONSTRAINT_TEACHERS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				QList<QList<int>> days;
				QList<QList<int>> hours;
				getSpinBoxTimesTable(days, hours);
				
				assert(days.count()==hours.count());
				if(days.count()<=1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two sets of time slots."));
					return;
				}
				
				for(int i=0; i<days.count(); i++){
					const QList<int>& cdl=days.at(i);
					const QList<int>& chl=hours.at(i);
					assert(cdl.count()==chl.count());
					if(cdl.count()==0){
						QMessageBox::warning(dialog, tr("FET information"),
							tr("Set number %1 of selected time slots is empty - please correct this"
							 " (the selected numbers must start from 1, and they must be continuous, without any gap between them).").arg(i+1));
						return;
					}
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					TimeConstraint *ctr=new ConstraintTeacherOccupiesMaxSetsOfTimeSlotsFromSelection(weight, tch->name, spinBox->value(), days, hours);
					bool tmp2=gt.rules.addTimeConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}

		default:
			assert(0);
			break;
	}

	if(teachersComboBox==nullptr)
		QMessageBox::information(dialog, tr("FET information"), tr("Added %1 time constraints. Please note that these constraints"
		 " will be visible as constraints for individual teachers.").arg(gt.rules.teachersList.count()));
	else
		QMessageBox::information(dialog, tr("FET information"), tr("Added %1 time constraints.").arg(gt.rules.teachersList.count()));

	if(gt.rules.teachersList.count()>0)
		gt.rules.addUndoPoint(tr("Added %1 constraints, one for each teacher:\n\n%2", "%1 is the number of constraints, %2 is their detailed description")
		 .arg(gt.rules.teachersList.count()).arg(ctrs));
}

void AddOrModifyTimeConstraint::closeClicked()
{
	dialog->close();
}

void AddOrModifyTimeConstraint::okClicked()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(dialog, tr("FET information"), tr("Invalid weight (percentage)"));
		return;
	}

	if(!timeConstraintCanHaveAnyWeight(type) && weight!=100.0){
		QMessageBox::warning(dialog, tr("FET information"), tr("Invalid weight (percentage) - it has to be 100%"));
		return;
	}

	switch(type){
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY:
			[[fallthrough]];
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_REAL_DAYS:
			[[fallthrough]];
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_REAL_DAYS:
			[[fallthrough]];
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_REAL_DAYS:
			if(weight<100.0){
				int t=QMessageBox::warning(dialog, tr("FET warning"),
					tr("You selected a weight less than 100%. The generation algorithm is not perfectly optimized to work with such weights (even"
					 " if in practice it might work well). It is recommended to work only with 100% weights for these constraints. Are you sure you want to continue?"),
					 QMessageBox::Yes | QMessageBox::Cancel);
				if(t==QMessageBox::Cancel)
					return;
			}
			break;

		default:
			//nothing;
			break;
	}

	if(!addEmpty && teachersComboBox!=nullptr){
		assert(filterGroupBox==nullptr);
		QString teacher_name=teachersComboBox->currentText();
		int teacher_ID=gt.rules.searchTeacher(teacher_name);
		if(teacher_ID<0){
			QMessageBox::warning(dialog, tr("FET information"), tr("Invalid teacher"));
			return;
		}
	}

	if(!addEmpty && studentsComboBox!=nullptr){
		assert(filterGroupBox==nullptr);
		QString students_name=studentsComboBox->currentText();
		StudentsSet* s=gt.rules.searchStudentsSet(students_name);
		if(s==nullptr){
			QMessageBox::warning(dialog, tr("FET information"), tr("Invalid students set"));
			return;
		}
	}

	if(!addEmpty && activityTagsComboBox!=nullptr){
		assert(filterGroupBox==nullptr);
		QString activity_tag_name=activityTagsComboBox->currentText();
		int activityTag_ID=gt.rules.searchActivityTag(activity_tag_name);
		if(activityTag_ID<0){
			QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity tag"));
			return;
		}
	}

	QString oldcs=oldtc->getDetailedDescription(gt.rules);
	
	switch(type){
		//1
		case CONSTRAINT_BASIC_COMPULSORY_TIME:
			{
				break;
			}
		//2
		case CONSTRAINT_BREAK_TIMES:
			{
				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				ConstraintBreakTimes* ctr=(ConstraintBreakTimes*)oldtc;
				ctr->days=days;
				ctr->hours=hours;

				break;
			}
		//3
		case CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES:
			{
				ConstraintTeacherNotAvailableTimes* ctr=(ConstraintTeacherNotAvailableTimes*)oldtc;

				if(ctr->teacher!=teachersComboBox->currentText()){
					QSet<ConstraintTeacherNotAvailableTimes*> cs=gt.rules.tnatHash.value(teachersComboBox->currentText(), QSet<ConstraintTeacherNotAvailableTimes*>());
					if(!cs.isEmpty()){
						QMessageBox::warning(dialog, tr("FET information"), tr("A constraint of this type exists for the same teacher - cannot proceed"));
						return;
					}
				}

				if(ctr->teacher!=teachersComboBox->currentText()){
					QString oldName=ctr->teacher;
					QString newName=teachersComboBox->currentText();

					QSet<ConstraintTeacherNotAvailableTimes*> cs=gt.rules.tnatHash.value(oldName, QSet<ConstraintTeacherNotAvailableTimes*>());
					assert(cs.count()==1);
					assert(cs.contains(ctr));
					//cs.remove(_ctr);
					//gt.rules.tnatHash.insert(oldName, cs);
					gt.rules.tnatHash.remove(oldName);

					cs=gt.rules.tnatHash.value(newName, QSet<ConstraintTeacherNotAvailableTimes*>());
					//assert(!cs.contains(_ctr));
					assert(cs.isEmpty());
					cs.insert(ctr);
					gt.rules.tnatHash.insert(newName, cs);

					ctr->teacher=teachersComboBox->currentText();
				}

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);
				ctr->days=days;
				ctr->hours=hours;

				break;
			}
		//4
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY:
			{
				ConstraintTeachersMaxHoursDaily* ctr=(ConstraintTeachersMaxHoursDaily*)oldtc;

				ctr->maxHoursDaily=spinBox->value();

				break;
			}
		//5
		case CONSTRAINT_TEACHER_MAX_DAYS_PER_WEEK:
			{
				ConstraintTeacherMaxDaysPerWeek* ctr=(ConstraintTeacherMaxDaysPerWeek*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxDaysPerWeek=spinBox->value();

				break;
			}
		//6
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK:
			{
				ConstraintTeachersMaxGapsPerWeek* ctr=(ConstraintTeachersMaxGapsPerWeek*)oldtc;

				ctr->maxGaps=spinBox->value();

				break;
			}
		//7
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK:
			{
				ConstraintTeacherMaxGapsPerWeek* ctr=(ConstraintTeacherMaxGapsPerWeek*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxGaps=spinBox->value();

				break;
			}
		//8
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY:
			{
				ConstraintTeacherMaxHoursDaily* ctr=(ConstraintTeacherMaxHoursDaily*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxHoursDaily=spinBox->value();

				break;
			}
		//9
		case CONSTRAINT_TEACHERS_MAX_HOURS_CONTINUOUSLY:
			{
				ConstraintTeachersMaxHoursContinuously* ctr=(ConstraintTeachersMaxHoursContinuously*)oldtc;

				ctr->maxHoursContinuously=spinBox->value();

				break;
			}
		//10
		case CONSTRAINT_TEACHER_MAX_HOURS_CONTINUOUSLY:
			{
				ConstraintTeacherMaxHoursContinuously* ctr=(ConstraintTeacherMaxHoursContinuously*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxHoursContinuously=spinBox->value();

				break;
			}
		//11
		case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY:
			{
				if(!checkBox->isChecked()){
					if(gt.rules.mode!=MORNINGS_AFTERNOONS){
						QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for the teachers, "
							"please use the constraint teachers min days per week"));
						return;
					}
					else{
						QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for a teacher, "
							"please use the constraint teacher min days per week (but the min days per week constraint is for real days. You can also use the "
							"constraints teacher min mornings/afternoons per week.)"));
						return;
					}
				}

				ConstraintTeachersMinHoursDaily* ctr=(ConstraintTeachersMinHoursDaily*)oldtc;

				ctr->minHoursDaily=spinBox->value();
				ctr->allowEmptyDays=checkBox->isChecked();

				break;
			}
		//12
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY:
			{
				if(!checkBox->isChecked()){
					if(gt.rules.mode!=MORNINGS_AFTERNOONS){
						QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for a teacher, "
							"please use the constraint teacher min days per week"));
						return;
					}
					else{
						QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for a teacher, "
							"please use the constraint teacher min days per week (but the min days per week constraint is for real days. You can also use the "
							"constraints teacher min mornings/afternoons per week.)"));
						return;
					}
				}

				ConstraintTeacherMinHoursDaily* ctr=(ConstraintTeacherMinHoursDaily*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->minHoursDaily=spinBox->value();
				ctr->allowEmptyDays=checkBox->isChecked();

				break;
			}
		//13
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_DAY:
			{
				ConstraintTeachersMaxGapsPerDay* ctr=(ConstraintTeachersMaxGapsPerDay*)oldtc;

				ctr->maxGaps=spinBox->value();

				break;
			}
		//14
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_DAY:
			{
				ConstraintTeacherMaxGapsPerDay* ctr=(ConstraintTeacherMaxGapsPerDay*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxGaps=spinBox->value();

				break;
			}
		//15
		case CONSTRAINT_STUDENTS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ConstraintStudentsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintStudentsEarlyMaxBeginningsAtSecondHour*)oldtc;

				ctr->maxBeginningsAtSecondHour=spinBox->value();

				break;
			}
		//16
		case CONSTRAINT_STUDENTS_SET_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintStudentsSetEarlyMaxBeginningsAtSecondHour*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxBeginningsAtSecondHour=spinBox->value();

				break;
			}
		//17
		case CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES:
			{
				ConstraintStudentsSetNotAvailableTimes* ctr=(ConstraintStudentsSetNotAvailableTimes*)oldtc;

				if(ctr->students!=studentsComboBox->currentText()){
					QSet<ConstraintStudentsSetNotAvailableTimes*> cs=gt.rules.ssnatHash.value(studentsComboBox->currentText(), QSet<ConstraintStudentsSetNotAvailableTimes*>());
					if(!cs.isEmpty()){
						QMessageBox::warning(dialog, tr("FET information"), tr("A constraint of this type exists for the same students set - cannot proceed"));
						return;
					}
				}

				if(ctr->students!=studentsComboBox->currentText()){
					QString oldName=ctr->students;
					QString newName=studentsComboBox->currentText();

					QSet<ConstraintStudentsSetNotAvailableTimes*> cs=gt.rules.ssnatHash.value(oldName, QSet<ConstraintStudentsSetNotAvailableTimes*>());
					assert(cs.count()==1);
					assert(cs.contains(ctr));
					//cs.remove(_ctr);
					//gt.rules.ssnatHash.insert(oldName, cs);
					gt.rules.ssnatHash.remove(oldName);

					cs=gt.rules.ssnatHash.value(newName, QSet<ConstraintStudentsSetNotAvailableTimes*>());
					//assert(!cs.contains(_ctr));
					assert(cs.isEmpty());
					cs.insert(ctr);
					gt.rules.ssnatHash.insert(newName, cs);

					ctr->students=studentsComboBox->currentText();
				}

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);
				ctr->days=days;
				ctr->hours=hours;

				break;
			}
		//18
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK:
			{
				ConstraintStudentsMaxGapsPerWeek* ctr=(ConstraintStudentsMaxGapsPerWeek*)oldtc;

				ctr->maxGaps=spinBox->value();

				break;
			}
		//19
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK:
			{
				ConstraintStudentsSetMaxGapsPerWeek* ctr=(ConstraintStudentsSetMaxGapsPerWeek*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxGaps=spinBox->value();

				break;
			}
		//20
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY:
			{
				ConstraintStudentsMaxHoursDaily* ctr=(ConstraintStudentsMaxHoursDaily*)oldtc;

				ctr->maxHoursDaily=spinBox->value();

				break;
			}
		//21
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY:
			{
				ConstraintStudentsSetMaxHoursDaily* ctr=(ConstraintStudentsSetMaxHoursDaily*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxHoursDaily=spinBox->value();

				break;
			}
		//22
		case CONSTRAINT_STUDENTS_MAX_HOURS_CONTINUOUSLY:
			{
				ConstraintStudentsMaxHoursContinuously* ctr=(ConstraintStudentsMaxHoursContinuously*)oldtc;

				ctr->maxHoursContinuously=spinBox->value();

				break;
			}
		//23
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_CONTINUOUSLY:
			{
				ConstraintStudentsSetMaxHoursContinuously* ctr=(ConstraintStudentsSetMaxHoursContinuously*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxHoursContinuously=spinBox->value();

				break;
			}
		//24
		case CONSTRAINT_STUDENTS_MIN_HOURS_DAILY:
			{
				ConstraintStudentsMinHoursDaily* ctr=(ConstraintStudentsMinHoursDaily*)oldtc;

				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					if(checkBox->isChecked() && spinBox->value()<2){
						QMessageBox::warning(dialog, tr("FET warning"), tr("If you allow empty days, the min hours must be at least 2 (to make it a non-trivial constraint)"));
						return;
					}
				}

				ctr->minHoursDaily=spinBox->value();
				ctr->allowEmptyDays=checkBox->isChecked();

				break;
			}
		//25
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY:
			{
				ConstraintStudentsSetMinHoursDaily* ctr=(ConstraintStudentsSetMinHoursDaily*)oldtc;

				if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					if(checkBox->isChecked() && spinBox->value()<2){
						QMessageBox::warning(dialog, tr("FET warning"), tr("If you allow empty days, the min hours must be at least 2 (to make it a non-trivial constraint)"));
						return;
					}
				}

				ctr->students=studentsComboBox->currentText();
				ctr->minHoursDaily=spinBox->value();
				ctr->allowEmptyDays=checkBox->isChecked();

				break;
			}
		//26
		case CONSTRAINT_ACTIVITY_ENDS_STUDENTS_DAY:
			{
				ConstraintActivityEndsStudentsDay* ctr=(ConstraintActivityEndsStudentsDay*)oldtc;

				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				ctr->activityId=id;

				break;
			}
		//27
		case CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME:
			{
				ConstraintActivityPreferredStartingTime* ctr=(ConstraintActivityPreferredStartingTime*)oldtc;

				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				if(daysComboBox->currentIndex()<0 || daysComboBox->currentIndex()>gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid day"));
					return;
				}
				if(hoursComboBox->currentIndex()<0 || hoursComboBox->currentIndex()>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid hour"));
					return;
				}

				if(true){
					ConstraintActivityPreferredStartingTime apst;

					apst.activityId=id;
					apst.weightPercentage=weight;
					apst.day=daysComboBox->currentIndex();
					apst.hour=hoursComboBox->currentIndex();
					apst.permanentlyLocked=permanentlyLockedCheckBox->isChecked();

					bool duplicate=false;

					for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList))
						if(tc!=oldtc && tc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME)
							if( ( *((ConstraintActivityPreferredStartingTime*)tc) ) == apst){
								duplicate=true;
								break;
							}

					if(duplicate){
						QMessageBox::warning(dialog, tr("FET information"), tr("Cannot proceed, current constraint is equal to another one (it is duplicated)"));
						return;
					}
				}

				if(ctr->activityId!=id){
					int oldId=ctr->activityId;
					int newId=id;

					QSet<ConstraintActivityPreferredStartingTime*> cs=gt.rules.apstHash.value(oldId, QSet<ConstraintActivityPreferredStartingTime*>());
					assert(cs.contains(ctr));
					cs.remove(ctr);
					gt.rules.apstHash.insert(oldId, cs);

					cs=gt.rules.apstHash.value(newId, QSet<ConstraintActivityPreferredStartingTime*>());
					assert(!cs.contains(ctr));
					cs.insert(ctr);
					gt.rules.apstHash.insert(newId, cs);

					ctr->activityId=id;
				}

				ctr->day=daysComboBox->currentIndex();
				ctr->hour=hoursComboBox->currentIndex();

				ctr->permanentlyLocked=permanentlyLockedCheckBox->isChecked();

				break;
			}
		//28
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME:
			{
				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"), tr("Only one selected activity"));
					return;
				}

				ConstraintActivitiesSameStartingTime* ctr=(ConstraintActivitiesSameStartingTime*)oldtc;
				ctr->n_activities=selectedActivitiesList.count();
				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				break;
			}
		//29
		case CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING:
			{
				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"), tr("Only one selected activity"));
					return;
				}

				ConstraintActivitiesNotOverlapping* ctr=(ConstraintActivitiesNotOverlapping*)oldtc;
				ctr->n_activities=selectedActivitiesList.count();
				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				break;
			}
		//30
		case CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES:
			{
				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"), tr("Only one selected activity"));
					return;
				}

				if(spinBox->value()<=0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Invalid number of min days between activities - it must be greater than 0."));
					return;
				}

				if(true){
					ConstraintMinDaysBetweenActivities adc;
					adc.activitiesIds=selectedActivitiesList;
					adc.n_activities=selectedActivitiesList.count();
					adc.weightPercentage=weight;
					adc.consecutiveIfSameDay=checkBox->isChecked();
					adc.minDays=spinBox->value();

					bool duplicate=false;

					for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList))
						if(tc!=oldtc && tc->type==CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES)
							if( ( *((ConstraintMinDaysBetweenActivities*)tc) ) == adc){
								duplicate=true;
								break;
							}

					if(duplicate){
						QMessageBox::warning(dialog, tr("FET information"), tr("Cannot proceed, current constraint is equal to another one (it is duplicated)"));
						return;
					}
				}

				ConstraintMinDaysBetweenActivities* ctr=(ConstraintMinDaysBetweenActivities*)oldtc;

				if(ctr->activitiesIds!=selectedActivitiesList){
					for(int oldId : std::as_const(ctr->activitiesIds)){
						QSet<ConstraintMinDaysBetweenActivities*> cs=gt.rules.mdbaHash.value(oldId, QSet<ConstraintMinDaysBetweenActivities*>());
						assert(cs.contains(ctr));
						cs.remove(ctr);
						gt.rules.mdbaHash.insert(oldId, cs);
					}

					for(int newId : std::as_const(selectedActivitiesList)){
						QSet<ConstraintMinDaysBetweenActivities*> cs=gt.rules.mdbaHash.value(newId, QSet<ConstraintMinDaysBetweenActivities*>());
						assert(!cs.contains(ctr));
						cs.insert(ctr);
						gt.rules.mdbaHash.insert(newId, cs);
					}

					ctr->activitiesIds=selectedActivitiesList;
					ctr->n_activities=ctr->activitiesIds.count();
					ctr->recomputeActivitiesSet();
				}

				ctr->consecutiveIfSameDay=checkBox->isChecked();
				ctr->minDays=spinBox->value();

				break;
			}
		//31
		case CONSTRAINT_ACTIVITY_PREFERRED_TIME_SLOTS:
			{
				ConstraintActivityPreferredTimeSlots* ctr=(ConstraintActivityPreferredTimeSlots*)oldtc;

				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				QList<int> days_L;
				QList<int> hours_L;
				getTimesTable(timesTable, days_L, hours_L, false);
				int n=days_L.count();
				assert(n==hours_L.count());

				if(n<=0){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("Warning: 0 slots selected. Are you sure?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				ctr->p_activityId=id;
				ctr->p_nPreferredTimeSlots_L=n;
				ctr->p_days_L=days_L;
				ctr->p_hours_L=hours_L;

				break;
			}
		//32
		case CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS:
			{
				ConstraintActivitiesPreferredTimeSlots* ctr=(ConstraintActivitiesPreferredTimeSlots*)oldtc;

				int duration=-1;
				if(durationCheckBox->isChecked()){
					assert(durationSpinBox->isEnabled());
					duration=durationSpinBox->value();
				}

				if(studentsComboBox->currentIndex()<0){
					showWarningCannotModifyConstraintInvisibleSubgroupConstraint(dialog, ctr->p_studentsName);
					return;
				}

				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				if(duration==-1 && teacher=="" && students=="" && subject=="" && activityTag==""){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("You specified all the activities. This might cause a minor issue: if you specify"
					  " a not allowed slot between two allowed slots, this not allowed slot will"
					  " be counted as a gap in the teachers' and students' timetable.\n\n"
					  "The best practice would be to use constraint break times.\n\n"
					  "If you need weight under 100%, then you can use this constraint, but be careful"
					  " not to obtain an impossible timetable (if your teachers/students are constrained on gaps"
					  " or early gaps and if you leave a not allowed slot between 2 allowed slots or"
					  " a not allowed slot early in the day and more allowed slots after it,"
					  " this possible gap might be counted in teachers' and students' timetable)")
					  +"\n\n"+tr("Do you want to add current constraint?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				if(duration==-1 && teacher!="" && students=="" && subject=="" && activityTag==""){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("You specified only the teacher. This might cause a minor issue: if you specify"
					  " a not allowed slot between two allowed slots, this not allowed slot will"
					  " be counted as a gap in the teacher's timetable.\n\n"
					  "The best practice would be to use constraint teacher not available times.\n\n"
					  "If you need weight under 100%, then you can use this constraint, but be careful"
					  " not to obtain an impossible timetable (if your teacher is constrained on gaps"
					  " and if you leave a not allowed slot between 2 allowed slots, this possible"
					  " gap might be counted in teacher's timetable)")
					  +"\n\n"+tr("Do you want to add current constraint?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				if(duration==-1 && teacher=="" && students!="" && subject=="" && activityTag==""){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("You specified only the students set. This might cause a minor issue: if you specify"
					  " a not allowed slot between two allowed slots (or a not allowed slot before allowed slots),"
					  " this not allowed slot will"
					  " be counted as a gap (or early gap) in the students' timetable.\n\n"
					  "The best practice would be to use constraint students set not available times.\n\n"
					  "If you need weight under 100%, then you can use this constraint, but be careful"
					  " not to obtain an impossible timetable (if your students set is constrained on gaps or early gaps"
					  " and if you leave a not allowed slot between 2 allowed slots (or a not allowed slot before allowed slots), this possible"
					  " gap might be counted in students' timetable)")
					  +"\n\n"+tr("Do you want to add current constraint?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				QList<int> days_L;
				QList<int> hours_L;
				getTimesTable(timesTable, days_L, hours_L, false);
				int n=days_L.count();
				assert(n==hours_L.count());

				if(n<=0){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("Warning: 0 slots selected. Are you sure?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				ctr->p_teacherName=teacher;
				ctr->p_studentsName=students;
				ctr->p_subjectName=subject;
				ctr->p_activityTagName=activityTag;
				ctr->p_nPreferredTimeSlots_L=n;
				ctr->p_days_L=days_L;
				ctr->p_hours_L=hours_L;

				ctr->duration=duration;

				break;
			}
		//33
		case CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIMES:
			{
				ConstraintActivityPreferredStartingTimes* ctr=(ConstraintActivityPreferredStartingTimes*)oldtc;

				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				QList<int> days_L;
				QList<int> hours_L;
				getTimesTable(timesTable, days_L, hours_L, false);
				int n=days_L.count();
				assert(n==hours_L.count());

				if(n<=0){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("Warning: 0 slots selected. Are you sure?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				ctr->activityId=id;
				ctr->nPreferredStartingTimes_L=n;
				ctr->days_L=days_L;
				ctr->hours_L=hours_L;

				break;
			}
		//34
		case CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES:
			{
				ConstraintActivitiesPreferredStartingTimes* ctr=(ConstraintActivitiesPreferredStartingTimes*)oldtc;

				int duration=-1;
				if(durationCheckBox->isChecked()){
					assert(durationSpinBox->isEnabled());
					duration=durationSpinBox->value();
				}

				if(studentsComboBox->currentIndex()<0){
					showWarningCannotModifyConstraintInvisibleSubgroupConstraint(dialog, ctr->studentsName);
					return;
				}

				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				if(duration==-1 && teacher=="" && students=="" && subject=="" && activityTag==""){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("You specified all the activities. This might cause a minor issue: if you specify"
					  " a not allowed slot between two allowed slots, this not allowed slot will"
					  " be counted as a gap in the teachers' and students' timetable.\n\n"
					  "The best practice would be to use constraint break times.\n\n"
					  "If you need weight under 100%, then you can use this constraint, but be careful"
					  " not to obtain an impossible timetable (if your teachers/students are constrained on gaps"
					  " or early gaps and if you leave a not allowed slot between 2 allowed slots or"
					  " a not allowed slot early in the day and more allowed slots after it,"
					  " this possible gap might be counted in teachers' and students' timetable)")
					  +"\n\n"+tr("Do you want to add current constraint?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				if(duration==-1 && teacher!="" && students=="" && subject=="" && activityTag==""){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("You specified only the teacher. This might cause a minor issue: if you specify"
					  " a not allowed slot between two allowed slots, this not allowed slot will"
					  " be counted as a gap in the teacher's timetable.\n\n"
					  "The best practice would be to use constraint teacher not available times.\n\n"
					  "If you need weight under 100%, then you can use this constraint, but be careful"
					  " not to obtain an impossible timetable (if your teacher is constrained on gaps"
					  " and if you leave a not allowed slot between 2 allowed slots, this possible"
					  " gap might be counted in teacher's timetable)")
					  +"\n\n"+tr("Do you want to add current constraint?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				if(duration==-1 && teacher=="" && students!="" && subject=="" && activityTag==""){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("You specified only the students set. This might cause a minor issue: if you specify"
					  " a not allowed slot between two allowed slots (or a not allowed slot before allowed slots),"
					  " this not allowed slot will"
					  " be counted as a gap (or early gap) in the students' timetable.\n\n"
					  "The best practice would be to use constraint students set not available times.\n\n"
					  "If you need weight under 100%, then you can use this constraint, but be careful"
					  " not to obtain an impossible timetable (if your students set is constrained on gaps or early gaps"
					  " and if you leave a not allowed slot between 2 allowed slots (or a not allowed slot before allowed slots), this possible"
					  " gap might be counted in students' timetable)")
					  +"\n\n"+tr("Do you want to add current constraint?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				QList<int> days_L;
				QList<int> hours_L;
				getTimesTable(timesTable, days_L, hours_L, false);
				int n=days_L.count();
				assert(n==hours_L.count());

				if(n<=0){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("Warning: 0 slots selected. Are you sure?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				ctr->teacherName=teacher;
				ctr->studentsName=students;
				ctr->subjectName=subject;
				ctr->activityTagName=activityTag;
				ctr->nPreferredStartingTimes_L=n;
				ctr->days_L=days_L;
				ctr->hours_L=hours_L;

				ctr->duration=duration;

				break;
			}
		//35
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR:
			{
				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"),	tr("Only one selected activity"));
					return;
				}

				ConstraintActivitiesSameStartingHour* ctr=(ConstraintActivitiesSameStartingHour*)oldtc;
				ctr->n_activities=selectedActivitiesList.count();
				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				break;
			}
		//36
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY:
			{
				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"),	tr("Only one selected activity"));
					return;
				}

				ConstraintActivitiesSameStartingDay* ctr=(ConstraintActivitiesSameStartingDay*)oldtc;
				ctr->n_activities=selectedActivitiesList.count();
				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				break;
			}
		//37
		case CONSTRAINT_TWO_ACTIVITIES_CONSECUTIVE:
			{
				assert(first_activitiesComboBox!=nullptr);
				int i=first_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid first activity"));
					return;
				}
				assert(i<first_activitiesList.count());
				int fid=first_activitiesList.at(i);

				assert(second_activitiesComboBox!=nullptr);
				i=second_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid second activity"));
					return;
				}
				assert(i<second_activitiesList.count());
				int sid=second_activitiesList.at(i);

				if(sid==fid){
					QMessageBox::warning(dialog, tr("FET information"), tr("Same activities"));
					return;
				}

				ConstraintTwoActivitiesConsecutive* ctr=(ConstraintTwoActivitiesConsecutive*)oldtc;
				ctr->firstActivityId=fid;
				ctr->secondActivityId=sid;

				break;
			}
		//38
		case CONSTRAINT_TWO_ACTIVITIES_ORDERED:
			{
				assert(first_activitiesComboBox!=nullptr);
				int i=first_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid first activity"));
					return;
				}
				assert(i<first_activitiesList.count());
				int fid=first_activitiesList.at(i);

				assert(second_activitiesComboBox!=nullptr);
				i=second_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid second activity"));
					return;
				}
				assert(i<second_activitiesList.count());
				int sid=second_activitiesList.at(i);

				if(sid==fid){
					QMessageBox::warning(dialog, tr("FET information"), tr("Same activities"));
					return;
				}

				ConstraintTwoActivitiesOrdered* ctr=(ConstraintTwoActivitiesOrdered*)oldtc;
				ctr->firstActivityId=fid;
				ctr->secondActivityId=sid;

				break;
			}
		//39
		case CONSTRAINT_MIN_GAPS_BETWEEN_ACTIVITIES:
			{
				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"),	tr("Only one selected activity"));
					return;
				}

				ConstraintMinGapsBetweenActivities* ctr=(ConstraintMinGapsBetweenActivities*)oldtc;

				ctr->activitiesIds=selectedActivitiesList;
				ctr->n_activities=ctr->activitiesIds.count();
				ctr->recomputeActivitiesSet();

				ctr->minGaps=spinBox->value();

				break;
			}
		//40
		case CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS:
			{
				ConstraintSubactivitiesPreferredTimeSlots* ctr=(ConstraintSubactivitiesPreferredTimeSlots*)oldtc;

				int duration=-1;
				if(durationCheckBox->isChecked()){
					assert(durationSpinBox->isEnabled());
					duration=durationSpinBox->value();
				}

				if(studentsComboBox->currentIndex()<0){
					showWarningCannotModifyConstraintInvisibleSubgroupConstraint(dialog, ctr->p_studentsName);
					return;
				}

				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				QList<int> days_L;
				QList<int> hours_L;
				getTimesTable(timesTable, days_L, hours_L, false);
				int n=days_L.count();
				assert(n==hours_L.count());

				if(n<=0){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("Warning: 0 slots selected. Are you sure?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				ctr->p_teacherName=teacher;
				ctr->p_studentsName=students;
				ctr->p_subjectName=subject;
				ctr->p_activityTagName=activityTag;
				ctr->p_nPreferredTimeSlots_L=n;
				ctr->p_days_L=days_L;
				ctr->p_hours_L=hours_L;

				ctr->duration=duration;

				ctr->componentNumber=splitIndexSpinBox->value();

				break;
			}
		//41
		case CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES:
			{
				ConstraintSubactivitiesPreferredStartingTimes* ctr=(ConstraintSubactivitiesPreferredStartingTimes*)oldtc;

				int duration=-1;
				if(durationCheckBox->isChecked()){
					assert(durationSpinBox->isEnabled());
					duration=durationSpinBox->value();
				}

				if(studentsComboBox->currentIndex()<0){
					showWarningCannotModifyConstraintInvisibleSubgroupConstraint(dialog, ctr->studentsName);
					return;
				}

				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				QList<int> days_L;
				QList<int> hours_L;
				getTimesTable(timesTable, days_L, hours_L, false);
				int n=days_L.count();
				assert(n==hours_L.count());

				if(n<=0){
					int t=QMessageBox::question(dialog, tr("FET question"),
					 tr("Warning: 0 slots selected. Are you sure?"),
					 QMessageBox::Yes, QMessageBox::Cancel);

					if(t==QMessageBox::Cancel)
							return;
				}

				ctr->teacherName=teacher;
				ctr->studentsName=students;
				ctr->subjectName=subject;
				ctr->activityTagName=activityTag;
				ctr->nPreferredStartingTimes_L=n;
				ctr->days_L=days_L;
				ctr->hours_L=hours_L;

				ctr->duration=duration;

				ctr->componentNumber=splitIndexSpinBox->value();

				break;
			}
		//42
		case CONSTRAINT_TEACHER_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				ConstraintTeacherIntervalMaxDaysPerWeek* ctr=(ConstraintTeacherIntervalMaxDaysPerWeek*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxDaysPerWeek=spinBox->value();

				ctr->startHour=startHour;
				ctr->endHour=endHour;

				break;
			}
		//43
		case CONSTRAINT_TEACHERS_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				ConstraintTeachersIntervalMaxDaysPerWeek* ctr=(ConstraintTeachersIntervalMaxDaysPerWeek*)oldtc;

				ctr->maxDaysPerWeek=spinBox->value();

				ctr->startHour=startHour;
				ctr->endHour=endHour;

				break;
			}
		//44
		case CONSTRAINT_STUDENTS_SET_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				ConstraintStudentsSetIntervalMaxDaysPerWeek* ctr=(ConstraintStudentsSetIntervalMaxDaysPerWeek*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxDaysPerWeek=spinBox->value();

				ctr->startHour=startHour;
				ctr->endHour=endHour;

				break;
			}
		//45
		case CONSTRAINT_STUDENTS_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				ConstraintStudentsIntervalMaxDaysPerWeek* ctr=(ConstraintStudentsIntervalMaxDaysPerWeek*)oldtc;

				ctr->maxDaysPerWeek=spinBox->value();

				ctr->startHour=startHour;
				ctr->endHour=endHour;

				break;
			}
		//46
		case CONSTRAINT_ACTIVITIES_END_STUDENTS_DAY:
			{
				ConstraintActivitiesEndStudentsDay* ctr=(ConstraintActivitiesEndStudentsDay*)oldtc;

				if(studentsComboBox->currentIndex()<0){
					showWarningCannotModifyConstraintInvisibleSubgroupConstraint(dialog, ctr->studentsName);
					return;
				}

				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				ctr->teacherName=teacher;
				ctr->studentsName=students;
				ctr->subjectName=subject;
				ctr->activityTagName=activityTag;

				break;
			}
		//47
		case CONSTRAINT_TWO_ACTIVITIES_GROUPED:
			{
				assert(first_activitiesComboBox!=nullptr);
				int i=first_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid first activity"));
					return;
				}
				assert(i<first_activitiesList.count());
				int fid=first_activitiesList.at(i);

				assert(second_activitiesComboBox!=nullptr);
				i=second_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid second activity"));
					return;
				}
				assert(i<second_activitiesList.count());
				int sid=second_activitiesList.at(i);

				if(sid==fid){
					QMessageBox::warning(dialog, tr("FET information"), tr("Same activities"));
					return;
				}

				ConstraintTwoActivitiesGrouped* ctr=(ConstraintTwoActivitiesGrouped*)oldtc;
				ctr->firstActivityId=fid;
				ctr->secondActivityId=sid;

				break;
			}
		//48
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				ConstraintTeachersActivityTagMaxHoursContinuously* ctr=(ConstraintTeachersActivityTagMaxHoursContinuously*)oldtc;

				ctr->maxHoursContinuously=spinBox->value();
				ctr->activityTagName=activityTagsComboBox->currentText();

				break;
			}
		//49
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				ConstraintTeacherActivityTagMaxHoursContinuously* ctr=(ConstraintTeacherActivityTagMaxHoursContinuously*)oldtc;

				ctr->maxHoursContinuously=spinBox->value();
				ctr->teacherName=teachersComboBox->currentText();
				ctr->activityTagName=activityTagsComboBox->currentText();

				break;
			}
		//50
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				ConstraintStudentsActivityTagMaxHoursContinuously* ctr=(ConstraintStudentsActivityTagMaxHoursContinuously*)oldtc;

				ctr->maxHoursContinuously=spinBox->value();
				ctr->activityTagName=activityTagsComboBox->currentText();

				break;
			}
		//51
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_CONTINUOUSLY:
			{
				ConstraintStudentsSetActivityTagMaxHoursContinuously* ctr=(ConstraintStudentsSetActivityTagMaxHoursContinuously*)oldtc;

				ctr->maxHoursContinuously=spinBox->value();
				ctr->students=studentsComboBox->currentText();
				ctr->activityTagName=activityTagsComboBox->currentText();

				break;
			}
		//52
		case CONSTRAINT_TEACHERS_MAX_DAYS_PER_WEEK:
			{
				ConstraintTeachersMaxDaysPerWeek* ctr=(ConstraintTeachersMaxDaysPerWeek*)oldtc;

				ctr->maxDaysPerWeek=spinBox->value();

				break;
			}
		//53
		case CONSTRAINT_THREE_ACTIVITIES_GROUPED:
			{
				assert(first_activitiesComboBox!=nullptr);
				int i=first_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid first activity"));
					return;
				}
				assert(i<first_activitiesList.count());
				int fid=first_activitiesList.at(i);

				assert(second_activitiesComboBox!=nullptr);
				i=second_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid second activity"));
					return;
				}
				assert(i<second_activitiesList.count());
				int sid=second_activitiesList.at(i);

				assert(third_activitiesComboBox!=nullptr);
				i=third_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid third activity"));
					return;
				}
				assert(i<third_activitiesList.count());
				int tid=third_activitiesList.at(i);

				if(sid==fid || sid==tid || fid==tid){
					QMessageBox::warning(dialog, tr("FET information"), tr("Same activities"));
					return;
				}

				ConstraintThreeActivitiesGrouped* ctr=(ConstraintThreeActivitiesGrouped*)oldtc;
				ctr->firstActivityId=fid;
				ctr->secondActivityId=sid;
				ctr->thirdActivityId=tid;

				break;
			}
		//54
		case CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES:
			{
				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"), tr("Only one selected activity"));
					return;
				}

				ConstraintMaxDaysBetweenActivities* ctr=(ConstraintMaxDaysBetweenActivities*)oldtc;

				ctr->activitiesIds=selectedActivitiesList;
				ctr->n_activities=ctr->activitiesIds.count();
				ctr->recomputeActivitiesSet();

				ctr->maxDays=spinBox->value();

				break;
			}
		//55
		case CONSTRAINT_TEACHERS_MIN_DAYS_PER_WEEK:
			{
				ConstraintTeachersMinDaysPerWeek* ctr=(ConstraintTeachersMinDaysPerWeek*)oldtc;

				ctr->minDaysPerWeek=spinBox->value();

				break;
			}
		//56
		case CONSTRAINT_TEACHER_MIN_DAYS_PER_WEEK:
			{
				ConstraintTeacherMinDaysPerWeek* ctr=(ConstraintTeacherMinDaysPerWeek*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->minDaysPerWeek=spinBox->value();

				break;
			}
		//57
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				ConstraintTeachersActivityTagMaxHoursDaily* ctr=(ConstraintTeachersActivityTagMaxHoursDaily*)oldtc;

				ctr->maxHoursDaily=spinBox->value();
				ctr->activityTagName=activityTagsComboBox->currentText();

				break;
			}
		//58
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				ConstraintTeacherActivityTagMaxHoursDaily* ctr=(ConstraintTeacherActivityTagMaxHoursDaily*)oldtc;

				ctr->maxHoursDaily=spinBox->value();
				ctr->teacherName=teachersComboBox->currentText();
				ctr->activityTagName=activityTagsComboBox->currentText();

				break;
			}
		//59
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				ConstraintStudentsActivityTagMaxHoursDaily* ctr=(ConstraintStudentsActivityTagMaxHoursDaily*)oldtc;

				ctr->maxHoursDaily=spinBox->value();
				ctr->activityTagName=activityTagsComboBox->currentText();

				break;
			}
		//60
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY:
			{
				ConstraintStudentsSetActivityTagMaxHoursDaily* ctr=(ConstraintStudentsSetActivityTagMaxHoursDaily*)oldtc;

				ctr->maxHoursDaily=spinBox->value();
				ctr->students=studentsComboBox->currentText();
				ctr->activityTagName=activityTagsComboBox->currentText();

				break;
			}
		//61
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_DAY:
			{
				ConstraintStudentsMaxGapsPerDay* ctr=(ConstraintStudentsMaxGapsPerDay*)oldtc;

				ctr->maxGaps=spinBox->value();

				break;
			}
		//62
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_DAY:
			{
				ConstraintStudentsSetMaxGapsPerDay* ctr=(ConstraintStudentsSetMaxGapsPerDay*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxGaps=spinBox->value();

				break;
			}
		//63
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TIME_SLOTS_FROM_SELECTION:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of activities"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("You specified max occupied time slots to be 0. This is "
					 "not perfect from efficiency point of view, because you can use instead constraint activity(ies) preferred time slots, "
					 "and help FET to find a timetable easier and faster, with an equivalent result. Please correct."));
					return;
				}

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				ConstraintActivitiesOccupyMaxTimeSlotsFromSelection* ctr=(ConstraintActivitiesOccupyMaxTimeSlotsFromSelection*)oldtc;
				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				ctr->selectedDays=days;
				ctr->selectedHours=hours;

				ctr->maxOccupiedTimeSlots=spinBox->value();

				break;
			}
		//64
		case CONSTRAINT_ACTIVITIES_MAX_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of activities"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("You specified max simultaneous activities to be 0. This is "
					 "not perfect from efficiency point of view, because you can use instead constraint activity(ies) preferred time slots, "
					 "and help FET to find a timetable easier and faster, with an equivalent result. Please correct."));
					return;
				}

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots* ctr=(ConstraintActivitiesMaxSimultaneousInSelectedTimeSlots*)oldtc;
				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				ctr->selectedDays=days;
				ctr->selectedHours=hours;

				ctr->maxSimultaneous=spinBox->value();

				break;
			}
		//65
		case CONSTRAINT_STUDENTS_SET_MAX_DAYS_PER_WEEK:
			{
				ConstraintStudentsSetMaxDaysPerWeek* ctr=(ConstraintStudentsSetMaxDaysPerWeek*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxDaysPerWeek=spinBox->value();

				break;
			}
		//66
		case CONSTRAINT_STUDENTS_MAX_DAYS_PER_WEEK:
			{
				ConstraintStudentsMaxDaysPerWeek* ctr=(ConstraintStudentsMaxDaysPerWeek*)oldtc;

				ctr->maxDaysPerWeek=spinBox->value();

				break;
			}
		//67
		case CONSTRAINT_TEACHER_MAX_SPAN_PER_DAY:
			{
				ConstraintTeacherMaxSpanPerDay* ctr=(ConstraintTeacherMaxSpanPerDay*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxSpanPerDay=spinBox->value();
				ctr->allowOneDayExceptionPlusOne=checkBox->isChecked();

				break;
			}
		//68
		case CONSTRAINT_TEACHERS_MAX_SPAN_PER_DAY:
			{
				ConstraintTeachersMaxSpanPerDay* ctr=(ConstraintTeachersMaxSpanPerDay*)oldtc;

				ctr->maxSpanPerDay=spinBox->value();
				ctr->allowOneDayExceptionPlusOne=checkBox->isChecked();

				break;
			}
		//69
		case CONSTRAINT_TEACHER_MIN_RESTING_HOURS:
			{
				ConstraintTeacherMinRestingHours* ctr=(ConstraintTeacherMinRestingHours*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->minRestingHours=spinBox->value();
				ctr->circular=checkBox->isChecked();

				break;
			}
		//70
		case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS:
			{
				ConstraintTeachersMinRestingHours* ctr=(ConstraintTeachersMinRestingHours*)oldtc;

				ctr->minRestingHours=spinBox->value();
				ctr->circular=checkBox->isChecked();

				break;
			}
		//71
		case CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_DAY:
			{
				ConstraintStudentsSetMaxSpanPerDay* ctr=(ConstraintStudentsSetMaxSpanPerDay*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxSpanPerDay=spinBox->value();

				break;
			}
		//72
		case CONSTRAINT_STUDENTS_MAX_SPAN_PER_DAY:
			{
				ConstraintStudentsMaxSpanPerDay* ctr=(ConstraintStudentsMaxSpanPerDay*)oldtc;

				ctr->maxSpanPerDay=spinBox->value();

				break;
			}
		//73
		case CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS:
			{
				ConstraintStudentsSetMinRestingHours* ctr=(ConstraintStudentsSetMinRestingHours*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->minRestingHours=spinBox->value();
				ctr->circular=checkBox->isChecked();

				break;
			}
		//74
		case CONSTRAINT_STUDENTS_MIN_RESTING_HOURS:
			{
				ConstraintStudentsMinRestingHours* ctr=(ConstraintStudentsMinRestingHours*)oldtc;

				ctr->minRestingHours=spinBox->value();
				ctr->circular=checkBox->isChecked();

				break;
			}
		//75
		case CONSTRAINT_TWO_ACTIVITIES_ORDERED_IF_SAME_DAY:
			{
				assert(first_activitiesComboBox!=nullptr);
				int i=first_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid first activity"));
					return;
				}
				assert(i<first_activitiesList.count());
				int fid=first_activitiesList.at(i);

				assert(second_activitiesComboBox!=nullptr);
				i=second_activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid second activity"));
					return;
				}
				assert(i<second_activitiesList.count());
				int sid=second_activitiesList.at(i);

				if(sid==fid){
					QMessageBox::warning(dialog, tr("FET information"), tr("Same activities"));
					return;
				}

				ConstraintTwoActivitiesOrderedIfSameDay* ctr=(ConstraintTwoActivitiesOrderedIfSameDay*)oldtc;
				ctr->firstActivityId=fid;
				ctr->secondActivityId=sid;

				break;
			}
		//76
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags* ctr=(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTags*)oldtc;

				ctr->students=studentsComboBox->currentText();

				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				ctr->firstActivityTag=firstActivityTagName;
				ctr->secondActivityTag=secondActivityTagName;

				ctr->minGaps=spinBox->value();

				break;
			}
		//77
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags* ctr=(ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTags*)oldtc;

				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				ctr->firstActivityTag=firstActivityTagName;
				ctr->secondActivityTag=secondActivityTagName;

				ctr->minGaps=spinBox->value();

				break;
			}
		//78
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags* ctr=(ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTags*)oldtc;

				ctr->teacher=teachersComboBox->currentText();

				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				ctr->firstActivityTag=firstActivityTagName;
				ctr->secondActivityTag=secondActivityTagName;

				ctr->minGaps=spinBox->value();

				break;
			}
		//79
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS:
			{
				ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags* ctr=(ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTags*)oldtc;

				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				ctr->firstActivityTag=firstActivityTagName;
				ctr->secondActivityTag=secondActivityTagName;

				ctr->minGaps=spinBox->value();

				break;
			}
		//80
		case CONSTRAINT_ACTIVITY_TAGS_NOT_OVERLAPPING:
			{
				if(selectedActivityTagsListWidget->count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of not overlapping activity tags"));
					return;
				}
				if(selectedActivityTagsListWidget->count()==1){
					QMessageBox::warning(dialog, tr("FET information"), tr("Only one selected activity tag"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				ConstraintActivityTagsNotOverlapping* ctr=(ConstraintActivityTagsNotOverlapping*)oldtc;
				ctr->activityTagsNames=atl;

				break;
			}
		//81
		case CONSTRAINT_ACTIVITIES_OCCUPY_MIN_TIME_SLOTS_FROM_SELECTION:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of activities"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("You specified min occupied time slots to be 0. This results in "
																		 "a useless constraint. Please correct."));
					return;
				}

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				ConstraintActivitiesOccupyMinTimeSlotsFromSelection* ctr=(ConstraintActivitiesOccupyMinTimeSlotsFromSelection*)oldtc;
				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				ctr->selectedDays=days;
				ctr->selectedHours=hours;

				ctr->minOccupiedTimeSlots=spinBox->value();

				break;
			}
		//82
		case CONSTRAINT_ACTIVITIES_MIN_SIMULTANEOUS_IN_SELECTED_TIME_SLOTS:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of activities"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("You specified min simultaneous activities to be 0. This results in "
					 "a useless constraint. Please correct."));
					return;
				}

				if(spinBox->value()==1 && allowEmptySlotsCheckBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("You specified min simultaneous activities to be 1 and you allow empty slots. "
					 "This results in a useless constraint. Please correct."));
					return;
				}

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				ConstraintActivitiesMinSimultaneousInSelectedTimeSlots* ctr=(ConstraintActivitiesMinSimultaneousInSelectedTimeSlots*)oldtc;
				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				ctr->selectedDays=days;
				ctr->selectedHours=hours;

				ctr->minSimultaneous=spinBox->value();
				ctr->allowEmptySlots=allowEmptySlotsCheckBox->isChecked();

				break;
			}
		//83
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				/*if(checkBox->isChecked() && spinBox->value()==1){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Allow empty days is selected and min hours daily is 1, "
																	   "so this would be a useless constraint."));
					return;
				}*/

				ConstraintTeachersActivityTagMinHoursDaily* ctr=(ConstraintTeachersActivityTagMinHoursDaily*)oldtc;

				ctr->minHoursDaily=spinBox->value();
				ctr->minDaysWithTag=secondSpinBox->value();
				ctr->activityTagName=activityTagsComboBox->currentText();
				//ctr->allowEmptyDays=checkBox->isChecked();

				break;
			}
		//84
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				/*if(checkBox->isChecked() && spinBox->value()==1){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Allow empty days is selected and min hours daily is 1, "
																	   "so this would be a useless constraint."));
					return;
				}*/

				ConstraintTeacherActivityTagMinHoursDaily* ctr=(ConstraintTeacherActivityTagMinHoursDaily*)oldtc;

				ctr->minHoursDaily=spinBox->value();
				ctr->minDaysWithTag=secondSpinBox->value();
				ctr->teacherName=teachersComboBox->currentText();
				ctr->activityTagName=activityTagsComboBox->currentText();
				//ctr->allowEmptyDays=checkBox->isChecked();

				break;
			}
		//85
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				/*if(checkBox->isChecked() && spinBox->value()==1){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Allow empty days is selected and min hours daily is 1, "
																	   "so this would be a useless constraint."));
					return;
				}*/

				ConstraintStudentsActivityTagMinHoursDaily* ctr=(ConstraintStudentsActivityTagMinHoursDaily*)oldtc;

				ctr->minHoursDaily=spinBox->value();
				ctr->minDaysWithTag=secondSpinBox->value();
				ctr->activityTagName=activityTagsComboBox->currentText();
				//ctr->allowEmptyDays=checkBox->isChecked();

				break;
			}
		//86
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				/*if(checkBox->isChecked() && spinBox->value()==1){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Allow empty days is selected and min hours daily is 1, "
																	   "so this would be a useless constraint."));
					return;
				}*/

				ConstraintStudentsSetActivityTagMinHoursDaily* ctr=(ConstraintStudentsSetActivityTagMinHoursDaily*)oldtc;

				ctr->minHoursDaily=spinBox->value();
				ctr->minDaysWithTag=secondSpinBox->value();
				ctr->students=studentsComboBox->currentText();
				ctr->activityTagName=activityTagsComboBox->currentText();
				//ctr->allowEmptyDays=checkBox->isChecked();

				break;
			}
		//87
		case CONSTRAINT_ACTIVITY_ENDS_TEACHERS_DAY:
			{
				ConstraintActivityEndsTeachersDay* ctr=(ConstraintActivityEndsTeachersDay*)oldtc;

				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				ctr->activityId=id;

				break;
			}
		//88
		case CONSTRAINT_ACTIVITIES_END_TEACHERS_DAY:
			{
				ConstraintActivitiesEndTeachersDay* ctr=(ConstraintActivitiesEndTeachersDay*)oldtc;

				if(studentsComboBox->currentIndex()<0){
					showWarningCannotModifyConstraintInvisibleSubgroupConstraint(dialog, ctr->studentsName);
					return;
				}

				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				ctr->teacherName=teacher;
				ctr->studentsName=students;
				ctr->subjectName=subject;
				ctr->activityTagName=activityTag;

				break;
			}
		//89
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ConstraintTeachersMaxHoursDailyRealDays* ctr=(ConstraintTeachersMaxHoursDailyRealDays*)oldtc;

				ctr->maxHoursDaily=spinBox->value();

				break;
			}
		//90
		case CONSTRAINT_TEACHER_MAX_REAL_DAYS_PER_WEEK:
			{
				ConstraintTeacherMaxRealDaysPerWeek* ctr=(ConstraintTeacherMaxRealDaysPerWeek*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxDaysPerWeek=spinBox->value();

				break;
			}
		//91
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ConstraintTeacherMaxHoursDailyRealDays* ctr=(ConstraintTeacherMaxHoursDailyRealDays*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxHoursDaily=spinBox->value();

				break;
			}
		//92
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ConstraintStudentsMaxHoursDailyRealDays* ctr=(ConstraintStudentsMaxHoursDailyRealDays*)oldtc;

				ctr->maxHoursDaily=spinBox->value();

				break;
			}
		//93
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ConstraintStudentsSetMaxHoursDailyRealDays* ctr=(ConstraintStudentsSetMaxHoursDailyRealDays*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxHoursDaily=spinBox->value();

				break;
			}
		//94
		case CONSTRAINT_TEACHERS_MAX_REAL_DAYS_PER_WEEK:
			{
				ConstraintTeachersMaxRealDaysPerWeek* ctr=(ConstraintTeachersMaxRealDaysPerWeek*)oldtc;

				ctr->maxDaysPerWeek=spinBox->value();

				break;
			}
		//95
		case CONSTRAINT_TEACHERS_MIN_REAL_DAYS_PER_WEEK:
			{
				ConstraintTeachersMinRealDaysPerWeek* ctr=(ConstraintTeachersMinRealDaysPerWeek*)oldtc;

				ctr->minDaysPerWeek=spinBox->value();

				break;
			}
		//96
		case CONSTRAINT_TEACHER_MIN_REAL_DAYS_PER_WEEK:
			{
				ConstraintTeacherMinRealDaysPerWeek* ctr=(ConstraintTeacherMinRealDaysPerWeek*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->minDaysPerWeek=spinBox->value();

				break;
			}
		//97
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ConstraintTeachersActivityTagMaxHoursDailyRealDays* ctr=(ConstraintTeachersActivityTagMaxHoursDailyRealDays*)oldtc;

				ctr->maxHoursDaily=spinBox->value();
				ctr->activityTagName=activityTagsComboBox->currentText();

				break;
			}
		//98
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ConstraintTeacherActivityTagMaxHoursDailyRealDays* ctr=(ConstraintTeacherActivityTagMaxHoursDailyRealDays*)oldtc;

				ctr->maxHoursDaily=spinBox->value();
				ctr->teacherName=teachersComboBox->currentText();
				ctr->activityTagName=activityTagsComboBox->currentText();

				break;
			}
		//99
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ConstraintStudentsActivityTagMaxHoursDailyRealDays* ctr=(ConstraintStudentsActivityTagMaxHoursDailyRealDays*)oldtc;

				ctr->maxHoursDaily=spinBox->value();
				ctr->activityTagName=activityTagsComboBox->currentText();

				break;
			}
		//100
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MAX_HOURS_DAILY_REAL_DAYS:
			{
				ConstraintStudentsSetActivityTagMaxHoursDailyRealDays* ctr=(ConstraintStudentsSetActivityTagMaxHoursDailyRealDays*)oldtc;

				ctr->maxHoursDaily=spinBox->value();
				ctr->students=studentsComboBox->currentText();
				ctr->activityTagName=activityTagsComboBox->currentText();

				break;
			}
		//101
		case CONSTRAINT_TEACHER_MAX_AFTERNOONS_PER_WEEK:
			{
				ConstraintTeacherMaxAfternoonsPerWeek* ctr=(ConstraintTeacherMaxAfternoonsPerWeek*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxAfternoonsPerWeek=spinBox->value();

				break;
			}
		//102
		case CONSTRAINT_TEACHERS_MAX_AFTERNOONS_PER_WEEK:
			{
				ConstraintTeachersMaxAfternoonsPerWeek* ctr=(ConstraintTeachersMaxAfternoonsPerWeek*)oldtc;

				ctr->maxAfternoonsPerWeek=spinBox->value();

				break;
			}
		//103
		case CONSTRAINT_TEACHER_MAX_MORNINGS_PER_WEEK:
			{
				ConstraintTeacherMaxMorningsPerWeek* ctr=(ConstraintTeacherMaxMorningsPerWeek*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxMorningsPerWeek=spinBox->value();

				break;
			}
		//104
		case CONSTRAINT_TEACHERS_MAX_MORNINGS_PER_WEEK:
			{
				ConstraintTeachersMaxMorningsPerWeek* ctr=(ConstraintTeachersMaxMorningsPerWeek*)oldtc;

				ctr->maxMorningsPerWeek=spinBox->value();

				break;
			}
		//105
		case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				ConstraintTeacherMaxActivityTagsPerDayFromSet* ctr=(ConstraintTeacherMaxActivityTagsPerDayFromSet*)oldtc;

				ctr->maxTags=spinBox->value();
				ctr->teacherName=teachersComboBox->currentText();
				ctr->tagsList=atl;

				break;
			}
		//106
		case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				ConstraintTeachersMaxActivityTagsPerDayFromSet* ctr=(ConstraintTeachersMaxActivityTagsPerDayFromSet*)oldtc;

				ctr->maxTags=spinBox->value();
				ctr->tagsList=atl;

				break;
			}
		//107
		case CONSTRAINT_TEACHERS_MIN_MORNINGS_PER_WEEK:
			{
				ConstraintTeachersMinMorningsPerWeek* ctr=(ConstraintTeachersMinMorningsPerWeek*)oldtc;

				ctr->minMorningsPerWeek=spinBox->value();

				break;
			}
		//108
		case CONSTRAINT_TEACHER_MIN_MORNINGS_PER_WEEK:
			{
				ConstraintTeacherMinMorningsPerWeek* ctr=(ConstraintTeacherMinMorningsPerWeek*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->minMorningsPerWeek=spinBox->value();

				break;
			}
		//109
		case CONSTRAINT_TEACHERS_MIN_AFTERNOONS_PER_WEEK:
			{
				ConstraintTeachersMinAfternoonsPerWeek* ctr=(ConstraintTeachersMinAfternoonsPerWeek*)oldtc;

				ctr->minAfternoonsPerWeek=spinBox->value();

				break;
			}
		//110
		case CONSTRAINT_TEACHER_MIN_AFTERNOONS_PER_WEEK:
			{
				ConstraintTeacherMinAfternoonsPerWeek* ctr=(ConstraintTeacherMinAfternoonsPerWeek*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->minAfternoonsPerWeek=spinBox->value();

				break;
			}
		//111
		case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_MORNINGS:
			{
				ConstraintTeacherMaxTwoConsecutiveMornings* ctr=(ConstraintTeacherMaxTwoConsecutiveMornings*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();

				break;
			}
		//112
		case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_MORNINGS:
			{
				//ConstraintTeachersMaxTwoConsecutiveMornings* ctr=(ConstraintTeachersMaxTwoConsecutiveMornings*)oldtc;

				//nothing to be done

				break;
			}
		//113
		case CONSTRAINT_TEACHER_MAX_TWO_CONSECUTIVE_AFTERNOONS:
			{
				ConstraintTeacherMaxTwoConsecutiveAfternoons* ctr=(ConstraintTeacherMaxTwoConsecutiveAfternoons*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();

				break;
			}
		//114
		case CONSTRAINT_TEACHERS_MAX_TWO_CONSECUTIVE_AFTERNOONS:
			{
				//ConstraintTeachersMaxTwoConsecutiveAfternoons* ctr=(ConstraintTeachersMaxTwoConsecutiveAfternoons*)oldtc;

				//nothing to be done

				break;
			}
		//115
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_REAL_DAY:
			{
				ConstraintTeachersMaxGapsPerRealDay* ctr=(ConstraintTeachersMaxGapsPerRealDay*)oldtc;

				ctr->maxGaps=spinBox->value();

				ctr->allowOneDayExceptionPlusOne=checkBox->isChecked();

				break;
			}
		//116
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY:
			{
				ConstraintTeacherMaxGapsPerRealDay* ctr=(ConstraintTeacherMaxGapsPerRealDay*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxGaps=spinBox->value();

				ctr->allowOneDayExceptionPlusOne=checkBox->isChecked();

				break;
			}
		//117
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_REAL_DAY:
			{
				ConstraintStudentsMaxGapsPerRealDay* ctr=(ConstraintStudentsMaxGapsPerRealDay*)oldtc;

				ctr->maxGaps=spinBox->value();

				break;
			}
		//118
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_REAL_DAY:
			{
				ConstraintStudentsSetMaxGapsPerRealDay* ctr=(ConstraintStudentsSetMaxGapsPerRealDay*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxGaps=spinBox->value();

				break;
			}
		//119
		case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY_REAL_DAYS:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for the teachers, "
						"please use the constraint teachers min days per week"));
					return;
				}

				ConstraintTeachersMinHoursDailyRealDays* ctr=(ConstraintTeachersMinHoursDailyRealDays*)oldtc;

				ctr->minHoursDaily=spinBox->value();
				ctr->allowEmptyDays=checkBox->isChecked();

				break;
			}
		//120
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty days check box must be checked. If you need to not allow empty days for this teacher, "
						"please use the constraint teacher min days per week"));
					return;
				}

				ConstraintTeacherMinHoursDailyRealDays* ctr=(ConstraintTeacherMinHoursDailyRealDays*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();

				ctr->minHoursDaily=spinBox->value();
				ctr->allowEmptyDays=checkBox->isChecked();

				break;
			}
		//121
		case CONSTRAINT_TEACHERS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintTeachersAfternoonsEarlyMaxBeginningsAtSecondHour*)oldtc;

				ctr->maxBeginningsAtSecondHour=spinBox->value();

				break;
			}
		//122
		case CONSTRAINT_TEACHER_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintTeacherAfternoonsEarlyMaxBeginningsAtSecondHour*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();

				ctr->maxBeginningsAtSecondHour=spinBox->value();

				break;
			}
		//123
		case CONSTRAINT_TEACHERS_MIN_HOURS_PER_MORNING:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty mornings check box must be checked. If you need to not allow empty mornings for the teachers, "
						"please use the constraint teachers min mornings per week."));
					return;
				}

				ConstraintTeachersMinHoursPerMorning* ctr=(ConstraintTeachersMinHoursPerMorning*)oldtc;

				ctr->minHoursPerMorning=spinBox->value();
				ctr->allowEmptyMornings=checkBox->isChecked();

				break;
			}
		//124
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty mornings check box must be checked. If you need to not allow empty mornings for a teacher, "
						"please use the constraint teacher min mornings per week."));
					return;
				}

				ConstraintTeacherMinHoursPerMorning* ctr=(ConstraintTeacherMinHoursPerMorning*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();

				ctr->minHoursPerMorning=spinBox->value();
				ctr->allowEmptyMornings=checkBox->isChecked();

				break;
			}
		//125
		case CONSTRAINT_TEACHER_MAX_SPAN_PER_REAL_DAY:
			{
				ConstraintTeacherMaxSpanPerRealDay* ctr=(ConstraintTeacherMaxSpanPerRealDay*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxSpanPerDay=spinBox->value();
				ctr->allowOneDayExceptionPlusOne=checkBox->isChecked();

				break;
			}
		//126
		case CONSTRAINT_TEACHERS_MAX_SPAN_PER_REAL_DAY:
			{
				ConstraintTeachersMaxSpanPerRealDay* ctr=(ConstraintTeachersMaxSpanPerRealDay*)oldtc;

				ctr->maxSpanPerDay=spinBox->value();
				ctr->allowOneDayExceptionPlusOne=checkBox->isChecked();

				break;
			}
		//127
		case CONSTRAINT_STUDENTS_SET_MAX_SPAN_PER_REAL_DAY:
			{
				ConstraintStudentsSetMaxSpanPerRealDay* ctr=(ConstraintStudentsSetMaxSpanPerRealDay*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxSpanPerDay=spinBox->value();

				break;
			}
		//128
		case CONSTRAINT_STUDENTS_MAX_SPAN_PER_REAL_DAY:
			{
				ConstraintStudentsMaxSpanPerRealDay* ctr=(ConstraintStudentsMaxSpanPerRealDay*)oldtc;

				ctr->maxSpanPerDay=spinBox->value();

				break;
			}
		//129
		case CONSTRAINT_TEACHER_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				ConstraintTeacherMorningIntervalMaxDaysPerWeek* ctr=(ConstraintTeacherMorningIntervalMaxDaysPerWeek*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxDaysPerWeek=spinBox->value();

				ctr->startHour=startHour;
				ctr->endHour=endHour;

				break;
			}
		//130
		case CONSTRAINT_TEACHERS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				ConstraintTeachersMorningIntervalMaxDaysPerWeek* ctr=(ConstraintTeachersMorningIntervalMaxDaysPerWeek*)oldtc;

				ctr->maxDaysPerWeek=spinBox->value();

				ctr->startHour=startHour;
				ctr->endHour=endHour;

				break;
			}
		//131
		case CONSTRAINT_TEACHER_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				ConstraintTeacherAfternoonIntervalMaxDaysPerWeek* ctr=(ConstraintTeacherAfternoonIntervalMaxDaysPerWeek*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxDaysPerWeek=spinBox->value();

				ctr->startHour=startHour;
				ctr->endHour=endHour;

				break;
			}
		//132
		case CONSTRAINT_TEACHERS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				ConstraintTeachersAfternoonIntervalMaxDaysPerWeek* ctr=(ConstraintTeachersAfternoonIntervalMaxDaysPerWeek*)oldtc;

				ctr->maxDaysPerWeek=spinBox->value();

				ctr->startHour=startHour;
				ctr->endHour=endHour;

				break;
			}
		//133
		case CONSTRAINT_STUDENTS_MIN_HOURS_PER_MORNING:
			{
				//if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					//2021-03-26 - I think I commented out this check because the user might combine this constraint with a min hours daily constraint.
					/*if(allowEmptyMorningsCheckBox->isChecked() && minHoursSpinBox->value()<2){
						QMessageBox::warning(dialog, tr("FET warning"), tr("If you allow empty mornings, the min hours must be at least 2 (to make it a non-trivial constraint)"));
						return;
					}*/
				//}

				ConstraintStudentsMinHoursPerMorning* ctr=(ConstraintStudentsMinHoursPerMorning*)oldtc;

				ctr->minHoursPerMorning=spinBox->value();
				ctr->allowEmptyMornings=checkBox->isChecked();

				break;
			}
		//134
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_MORNING:
			{
				//if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					//2021-03-26 - I think I commented out this check because the user might combine this constraint with a min hours daily constraint.
					/*if(allowEmptyMorningsCheckBox->isChecked() && minHoursSpinBox->value()<2){
						QMessageBox::warning(dialog, tr("FET warning"), tr("If you allow empty mornings, the min hours must be at least 2 (to make it a non-trivial constraint)"));
						return;
					}*/
				//}

				ConstraintStudentsSetMinHoursPerMorning* ctr=(ConstraintStudentsSetMinHoursPerMorning*)oldtc;

				ctr->students=studentsComboBox->currentText();

				ctr->minHoursPerMorning=spinBox->value();
				ctr->allowEmptyMornings=checkBox->isChecked();

				break;
			}
		//135
		case CONSTRAINT_TEACHER_MAX_ZERO_GAPS_PER_AFTERNOON:
			{
				ConstraintTeacherMaxZeroGapsPerAfternoon* ctr=(ConstraintTeacherMaxZeroGapsPerAfternoon*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();

				break;
			}
		//136
		case CONSTRAINT_TEACHERS_MAX_ZERO_GAPS_PER_AFTERNOON:
			{
				//ConstraintTeachersMaxZeroGapsPerAfternoon* ctr=(ConstraintTeachersMaxZeroGapsPerAfternoon*)oldtc;

				//nothing to be done

				break;
			}
		//137
		case CONSTRAINT_STUDENTS_SET_MAX_AFTERNOONS_PER_WEEK:
			{
				ConstraintStudentsSetMaxAfternoonsPerWeek* ctr=(ConstraintStudentsSetMaxAfternoonsPerWeek*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxAfternoonsPerWeek=spinBox->value();

				break;
			}
		//138
		case CONSTRAINT_STUDENTS_MAX_AFTERNOONS_PER_WEEK:
			{
				ConstraintStudentsMaxAfternoonsPerWeek* ctr=(ConstraintStudentsMaxAfternoonsPerWeek*)oldtc;

				ctr->maxAfternoonsPerWeek=spinBox->value();

				break;
			}
		//139
		case CONSTRAINT_STUDENTS_SET_MAX_MORNINGS_PER_WEEK:
			{
				ConstraintStudentsSetMaxMorningsPerWeek* ctr=(ConstraintStudentsSetMaxMorningsPerWeek*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxMorningsPerWeek=spinBox->value();

				break;
			}
		//140
		case CONSTRAINT_STUDENTS_MAX_MORNINGS_PER_WEEK:
			{
				ConstraintStudentsMaxMorningsPerWeek* ctr=(ConstraintStudentsMaxMorningsPerWeek*)oldtc;

				ctr->maxMorningsPerWeek=spinBox->value();

				break;
			}
		//141
		case CONSTRAINT_STUDENTS_MIN_MORNINGS_PER_WEEK:
			{
				ConstraintStudentsMinMorningsPerWeek* ctr=(ConstraintStudentsMinMorningsPerWeek*)oldtc;

				ctr->minMorningsPerWeek=spinBox->value();

				break;
			}
		//142
		case CONSTRAINT_STUDENTS_SET_MIN_MORNINGS_PER_WEEK:
			{
				ConstraintStudentsSetMinMorningsPerWeek* ctr=(ConstraintStudentsSetMinMorningsPerWeek*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->minMorningsPerWeek=spinBox->value();

				break;
			}
		//143
		case CONSTRAINT_STUDENTS_MIN_AFTERNOONS_PER_WEEK:
			{
				ConstraintStudentsMinAfternoonsPerWeek* ctr=(ConstraintStudentsMinAfternoonsPerWeek*)oldtc;

				ctr->minAfternoonsPerWeek=spinBox->value();

				break;
			}
		//144
		case CONSTRAINT_STUDENTS_SET_MIN_AFTERNOONS_PER_WEEK:
			{
				ConstraintStudentsSetMinAfternoonsPerWeek* ctr=(ConstraintStudentsSetMinAfternoonsPerWeek*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->minAfternoonsPerWeek=spinBox->value();

				break;
			}
		//145
		case CONSTRAINT_STUDENTS_SET_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				ConstraintStudentsSetMorningIntervalMaxDaysPerWeek* ctr=(ConstraintStudentsSetMorningIntervalMaxDaysPerWeek*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxDaysPerWeek=spinBox->value();

				ctr->startHour=startHour;
				ctr->endHour=endHour;

				break;
			}
		//146
		case CONSTRAINT_STUDENTS_MORNING_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				ConstraintStudentsMorningIntervalMaxDaysPerWeek* ctr=(ConstraintStudentsMorningIntervalMaxDaysPerWeek*)oldtc;

				ctr->maxDaysPerWeek=spinBox->value();

				ctr->startHour=startHour;
				ctr->endHour=endHour;

				break;
			}
		//147
		case CONSTRAINT_STUDENTS_SET_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek* ctr=(ConstraintStudentsSetAfternoonIntervalMaxDaysPerWeek*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxDaysPerWeek=spinBox->value();

				ctr->startHour=startHour;
				ctr->endHour=endHour;

				break;
			}
		//148
		case CONSTRAINT_STUDENTS_AFTERNOON_INTERVAL_MAX_DAYS_PER_WEEK:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				ConstraintStudentsAfternoonIntervalMaxDaysPerWeek* ctr=(ConstraintStudentsAfternoonIntervalMaxDaysPerWeek*)oldtc;

				ctr->maxDaysPerWeek=spinBox->value();

				ctr->startHour=startHour;
				ctr->endHour=endHour;

				break;
			}
		//149
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				ConstraintTeacherMaxHoursPerAllAfternoons* ctr=(ConstraintTeacherMaxHoursPerAllAfternoons*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxHoursPerAllAfternoons=spinBox->value();

				break;
			}
		//150
		case CONSTRAINT_TEACHERS_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				ConstraintTeachersMaxHoursPerAllAfternoons* ctr=(ConstraintTeachersMaxHoursPerAllAfternoons*)oldtc;

				ctr->maxHoursPerAllAfternoons=spinBox->value();

				break;
			}
		//151
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				ConstraintStudentsSetMaxHoursPerAllAfternoons* ctr=(ConstraintStudentsSetMaxHoursPerAllAfternoons*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxHoursPerAllAfternoons=spinBox->value();

				break;
			}
		//152
		case CONSTRAINT_STUDENTS_MAX_HOURS_PER_ALL_AFTERNOONS:
			{
				ConstraintStudentsMaxHoursPerAllAfternoons* ctr=(ConstraintStudentsMaxHoursPerAllAfternoons*)oldtc;

				ctr->maxHoursPerAllAfternoons=spinBox->value();

				break;
			}
		//153
		case CONSTRAINT_TEACHER_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon* ctr=(ConstraintTeacherMinRestingHoursBetweenMorningAndAfternoon*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->minRestingHours=spinBox->value();

				break;
			}
		//154
		case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ConstraintTeachersMinRestingHoursBetweenMorningAndAfternoon* ctr=(ConstraintTeachersMinRestingHoursBetweenMorningAndAfternoon*)oldtc;

				ctr->minRestingHours=spinBox->value();

				break;
			}
		//155
		case CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon* ctr=(ConstraintStudentsSetMinRestingHoursBetweenMorningAndAfternoon*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->minRestingHours=spinBox->value();

				break;
			}
		//156
		case CONSTRAINT_STUDENTS_MIN_RESTING_HOURS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon* ctr=(ConstraintStudentsMinRestingHoursBetweenMorningAndAfternoon*)oldtc;

				ctr->minRestingHours=spinBox->value();

				break;
			}
		//157
		case CONSTRAINT_STUDENTS_SET_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHour*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxBeginningsAtSecondHour=spinBox->value();

				break;
			}
		//158
		case CONSTRAINT_STUDENTS_AFTERNOONS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintStudentsAfternoonsEarlyMaxBeginningsAtSecondHour*)oldtc;

				ctr->maxBeginningsAtSecondHour=spinBox->value();

				break;
			}
		//159
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				ConstraintTeachersMaxGapsPerWeekForRealDays* ctr=(ConstraintTeachersMaxGapsPerWeekForRealDays*)oldtc;

				ctr->maxGaps=spinBox->value();

				break;
			}
		//160
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				ConstraintTeacherMaxGapsPerWeekForRealDays* ctr=(ConstraintTeacherMaxGapsPerWeekForRealDays*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxGaps=spinBox->value();

				break;
			}
		//161
		case CONSTRAINT_STUDENTS_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				ConstraintStudentsMaxGapsPerWeekForRealDays* ctr=(ConstraintStudentsMaxGapsPerWeekForRealDays*)oldtc;

				ctr->maxGaps=spinBox->value();

				break;
			}
		//162
		case CONSTRAINT_STUDENTS_SET_MAX_GAPS_PER_WEEK_FOR_REAL_DAYS:
			{
				ConstraintStudentsSetMaxGapsPerWeekForRealDays* ctr=(ConstraintStudentsSetMaxGapsPerWeekForRealDays*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxGaps=spinBox->value();

				break;
			}
		//163
		case CONSTRAINT_STUDENTS_SET_MAX_REAL_DAYS_PER_WEEK:
			{
				ConstraintStudentsSetMaxRealDaysPerWeek* ctr=(ConstraintStudentsSetMaxRealDaysPerWeek*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxDaysPerWeek=spinBox->value();

				break;
			}
		//164
		case CONSTRAINT_STUDENTS_MAX_REAL_DAYS_PER_WEEK:
			{
				ConstraintStudentsMaxRealDaysPerWeek* ctr=(ConstraintStudentsMaxRealDaysPerWeek*)oldtc;

				ctr->maxDaysPerWeek=spinBox->value();

				break;
			}
		//165
		case CONSTRAINT_MAX_TOTAL_ACTIVITIES_FROM_SET_IN_SELECTED_TIME_SLOTS:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of activities"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("You specified max total activities to be 0. This is "
					 "not perfect from efficiency point of view, because you can use instead constraint activity(ies) preferred time slots, "
					 "and help FET to find a timetable easier and faster, with an equivalent result. Please correct."));
					return;
				}

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots* ctr=(ConstraintMaxTotalActivitiesFromSetInSelectedTimeSlots*)oldtc;
				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				ctr->selectedDays=days;
				ctr->selectedHours=hours;

				ctr->maxActivities=spinBox->value();

				break;
			}
		//166
		case CONSTRAINT_MAX_GAPS_BETWEEN_ACTIVITIES:
			{
				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"),	tr("Only one selected activity"));
					return;
				}

				ConstraintMaxGapsBetweenActivities* ctr=(ConstraintMaxGapsBetweenActivities*)oldtc;

				ctr->activitiesIds=selectedActivitiesList;
				ctr->n_activities=ctr->activitiesIds.count();
				ctr->recomputeActivitiesSet();

				ctr->maxGaps=spinBox->value();

				break;
			}
		//167
		case CONSTRAINT_ACTIVITIES_MAX_IN_A_TERM:
			{
				if(this->selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
					 tr("Empty list of activities"));
					return;
				}
				//we allow even only one activity
				/*if(this->selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
					 tr("Only one selected activity"));
					return;
				}*/

				ConstraintActivitiesMaxInATerm* ctr=(ConstraintActivitiesMaxInATerm*)oldtc;

				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				ctr->maxActivitiesInATerm=spinBox->value();

				break;
			}
		//168
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_TERMS:
			{
				if(this->selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
					 tr("Empty list of activities"));
					return;
				}
				//we allow even only one activity
				/*if(this->selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
					 tr("Only one selected activity"));
					return;
				}*/

				ConstraintActivitiesOccupyMaxTerms* ctr=(ConstraintActivitiesOccupyMaxTerms*)oldtc;

				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				ctr->maxOccupiedTerms=spinBox->value();

				break;
			}
		//169
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			{
				ConstraintTeachersMaxGapsPerMorningAndAfternoon* ctr=(ConstraintTeachersMaxGapsPerMorningAndAfternoon*)oldtc;

				ctr->maxGaps=spinBox->value();

				break;
			}
		//170
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_MORNING_AND_AFTERNOON:
			{
				ConstraintTeacherMaxGapsPerMorningAndAfternoon* ctr=(ConstraintTeacherMaxGapsPerMorningAndAfternoon*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxGaps=spinBox->value();

				break;
			}
		//171
		case CONSTRAINT_TEACHERS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintTeachersMorningsEarlyMaxBeginningsAtSecondHour*)oldtc;

				ctr->maxBeginningsAtSecondHour=spinBox->value();

				break;
			}
		//172
		case CONSTRAINT_TEACHER_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintTeacherMorningsEarlyMaxBeginningsAtSecondHour*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();

				ctr->maxBeginningsAtSecondHour=spinBox->value();

				break;
			}
		//173
		case CONSTRAINT_STUDENTS_SET_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintStudentsSetMorningsEarlyMaxBeginningsAtSecondHour*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxBeginningsAtSecondHour=spinBox->value();

				break;
			}
		//174
		case CONSTRAINT_STUDENTS_MORNINGS_EARLY_MAX_BEGINNINGS_AT_SECOND_HOUR:
			{
				ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour* ctr=(ConstraintStudentsMorningsEarlyMaxBeginningsAtSecondHour*)oldtc;

				ctr->maxBeginningsAtSecondHour=spinBox->value();

				break;
			}
		//175
		case CONSTRAINT_TWO_SETS_OF_ACTIVITIES_ORDERED:
			{
				ConstraintTwoSetsOfActivitiesOrdered* ctr=(ConstraintTwoSetsOfActivitiesOrdered*)oldtc;

				if(selectedActivitiesList_TwoSetsOfActivities_1.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities in the first set"));
					return;
				}
				if(selectedActivitiesList_TwoSetsOfActivities_2.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities in the second set"));
					return;
				}

				ctr->firstActivitiesIdsList=selectedActivitiesList_TwoSetsOfActivities_1;
				ctr->secondActivitiesIdsList=selectedActivitiesList_TwoSetsOfActivities_2;

				ctr->recomputeActivitiesSets();

				break;
			}
		//176
		case CONSTRAINT_TEACHERS_MAX_THREE_CONSECUTIVE_DAYS:
			{
				ConstraintTeachersMaxThreeConsecutiveDays* ctr=(ConstraintTeachersMaxThreeConsecutiveDays*)oldtc;

				ctr->allowAMAMException=checkBox->isChecked();

				break;
			}
		//177
		case CONSTRAINT_TEACHER_MAX_THREE_CONSECUTIVE_DAYS:
			{
				ConstraintTeacherMaxThreeConsecutiveDays* ctr=(ConstraintTeacherMaxThreeConsecutiveDays*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();

				ctr->allowAMAMException=checkBox->isChecked();

				break;
			}
		//178
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				ConstraintStudentsSetMinGapsBetweenActivityTag* ctr=(ConstraintStudentsSetMinGapsBetweenActivityTag*)oldtc;

				ctr->students=studentsComboBox->currentText();

				ctr->activityTag=activityTagsComboBox->currentText();

				ctr->minGaps=spinBox->value();

				break;
			}
		//179
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				ConstraintStudentsMinGapsBetweenActivityTag* ctr=(ConstraintStudentsMinGapsBetweenActivityTag*)oldtc;

				ctr->activityTag=activityTagsComboBox->currentText();

				ctr->minGaps=spinBox->value();

				break;
			}
		//180
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				ConstraintTeacherMinGapsBetweenActivityTag* ctr=(ConstraintTeacherMinGapsBetweenActivityTag*)oldtc;

				ctr->teacher=teachersComboBox->currentText();

				ctr->activityTag=activityTagsComboBox->currentText();

				ctr->minGaps=spinBox->value();

				break;
			}
		//181
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG:
			{
				ConstraintTeachersMinGapsBetweenActivityTag* ctr=(ConstraintTeachersMinGapsBetweenActivityTag*)oldtc;

				ctr->activityTag=activityTagsComboBox->currentText();

				ctr->minGaps=spinBox->value();

				break;
			}
		//182
		case CONSTRAINT_STUDENTS_MAX_THREE_CONSECUTIVE_DAYS:
			{
				ConstraintStudentsMaxThreeConsecutiveDays* ctr=(ConstraintStudentsMaxThreeConsecutiveDays*)oldtc;

				ctr->allowAMAMException=checkBox->isChecked();

				break;
			}
		//183
		case CONSTRAINT_STUDENTS_SET_MAX_THREE_CONSECUTIVE_DAYS:
			{
				ConstraintStudentsSetMaxThreeConsecutiveDays* ctr=(ConstraintStudentsSetMaxThreeConsecutiveDays*)oldtc;

				ctr->students=studentsComboBox->currentText();

				ctr->allowAMAMException=checkBox->isChecked();

				break;
			}
		//184
		case CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES:
			{
				if(this->selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities"));
					return;
				}
				if(this->selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Only one selected activity"));
					return;
				}

				if(spinBox->value()<=0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Invalid number of min half days between activities - it must be greater than 0."));
					return;
				}

				if(true){
					ConstraintMinHalfDaysBetweenActivities adc;
					adc.activitiesIds=selectedActivitiesList;
					adc.n_activities=selectedActivitiesList.count();
					adc.weightPercentage=weight;
					adc.consecutiveIfSameDay=checkBox->isChecked();
					adc.minDays=spinBox->value();

					bool duplicate=false;

					for(TimeConstraint* tc : std::as_const(gt.rules.timeConstraintsList))
						if(tc!=oldtc && tc->type==CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES)
							if( ( *((ConstraintMinHalfDaysBetweenActivities*)tc) ) == adc){
								duplicate=true;
								break;
							}

					if(duplicate){
						QMessageBox::warning(dialog, tr("FET information"), tr("Cannot proceed, current constraint is equal to another one (it is duplicated)"));
						return;
					}
				}

				ConstraintMinHalfDaysBetweenActivities* ctr=(ConstraintMinHalfDaysBetweenActivities*)oldtc;

				if(ctr->activitiesIds!=selectedActivitiesList){
					for(int oldId : std::as_const(ctr->activitiesIds)){
						QSet<ConstraintMinHalfDaysBetweenActivities*> cs=gt.rules.mhdbaHash.value(oldId, QSet<ConstraintMinHalfDaysBetweenActivities*>());
						assert(cs.contains(ctr));
						cs.remove(ctr);
						gt.rules.mhdbaHash.insert(oldId, cs);
					}

					for(int newId : std::as_const(selectedActivitiesList)){
						QSet<ConstraintMinHalfDaysBetweenActivities*> cs=gt.rules.mhdbaHash.value(newId, QSet<ConstraintMinHalfDaysBetweenActivities*>());
						assert(!cs.contains(ctr));
						cs.insert(ctr);
						gt.rules.mhdbaHash.insert(newId, cs);
					}

					ctr->activitiesIds=selectedActivitiesList;
					ctr->n_activities=ctr->activitiesIds.count();
					ctr->recomputeActivitiesSet();
				}

				ctr->consecutiveIfSameDay=checkBox->isChecked();
				ctr->minDays=spinBox->value();

				break;
			}
		//185 is activity preferred day, which is not in the interface
		//186
		case CONSTRAINT_ACTIVITIES_MIN_IN_A_TERM:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
					 tr("Empty list of activities"));
					return;
				}
				//we allow even only one activity
				/*if(selectedActivitiesList.count()==1){
					QMessageBox::warning(dialog, tr("FET information"),
					 tr("Only one selected activity"));
					return;
				}*/

				if(checkBox->isChecked() && spinBox->value()==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Min activities in a term is 1, and you allow empty terms, so this constraint would be useless."));
					return;
				}

				ConstraintActivitiesMinInATerm* ctr=(ConstraintActivitiesMinInATerm*)oldtc;

				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				ctr->minActivitiesInATerm=spinBox->value();
				ctr->allowEmptyTerms=checkBox->isChecked();

				break;
			}
		//187
		case CONSTRAINT_MAX_TERMS_BETWEEN_ACTIVITIES:
			{
				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"),	tr("Only one selected activity"));
					return;
				}

				ConstraintMaxTermsBetweenActivities* ctr=(ConstraintMaxTermsBetweenActivities*)oldtc;

				ctr->activitiesIds=selectedActivitiesList;
				ctr->n_activities=ctr->activitiesIds.count();
				ctr->recomputeActivitiesSet();

				ctr->maxTerms=spinBox->value();

				break;
			}
		//188
		case CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				ConstraintStudentsSetMaxActivityTagsPerDayFromSet* ctr=(ConstraintStudentsSetMaxActivityTagsPerDayFromSet*)oldtc;

				ctr->maxTags=spinBox->value();
				ctr->students=studentsComboBox->currentText();
				ctr->tagsList=atl;

				break;
			}
		//189
		case CONSTRAINT_STUDENTS_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				ConstraintStudentsMaxActivityTagsPerDayFromSet* ctr=(ConstraintStudentsMaxActivityTagsPerDayFromSet*)oldtc;

				ctr->maxTags=spinBox->value();
				ctr->tagsList=atl;

				break;
			}
		//190
		case CONSTRAINT_TEACHER_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				ConstraintTeacherMaxActivityTagsPerRealDayFromSet* ctr=(ConstraintTeacherMaxActivityTagsPerRealDayFromSet*)oldtc;

				ctr->maxTags=spinBox->value();
				ctr->teacherName=teachersComboBox->currentText();
				ctr->tagsList=atl;

				break;
			}
		//191
		case CONSTRAINT_TEACHERS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				ConstraintTeachersMaxActivityTagsPerRealDayFromSet* ctr=(ConstraintTeachersMaxActivityTagsPerRealDayFromSet*)oldtc;

				ctr->maxTags=spinBox->value();
				ctr->tagsList=atl;

				break;
			}
		//192
		case CONSTRAINT_STUDENTS_SET_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				ConstraintStudentsSetMaxActivityTagsPerRealDayFromSet* ctr=(ConstraintStudentsSetMaxActivityTagsPerRealDayFromSet*)oldtc;

				ctr->maxTags=spinBox->value();
				ctr->students=studentsComboBox->currentText();
				ctr->tagsList=atl;

				break;
			}
		//193
		case CONSTRAINT_STUDENTS_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two activity tags"));
					return;
				}

				QStringList atl;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					atl.append(selectedActivityTagsListWidget->item(i)->text());

				ConstraintStudentsMaxActivityTagsPerRealDayFromSet* ctr=(ConstraintStudentsMaxActivityTagsPerRealDayFromSet*)oldtc;

				ctr->maxTags=spinBox->value();
				ctr->tagsList=atl;

				break;
			}
		//194
		case CONSTRAINT_MAX_HALF_DAYS_BETWEEN_ACTIVITIES:
			{
				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"), tr("Only one selected activity"));
					return;
				}

				ConstraintMaxHalfDaysBetweenActivities* ctr=(ConstraintMaxHalfDaysBetweenActivities*)oldtc;

				ctr->activitiesIds=selectedActivitiesList;
				ctr->n_activities=ctr->activitiesIds.count();
				ctr->recomputeActivitiesSet();

				ctr->maxDays=spinBox->value();

				break;
			}
		//195
		case CONSTRAINT_ACTIVITY_BEGINS_STUDENTS_DAY:
			{
				ConstraintActivityBeginsStudentsDay* ctr=(ConstraintActivityBeginsStudentsDay*)oldtc;

				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				ctr->activityId=id;

				break;
			}
		//196
		case CONSTRAINT_ACTIVITIES_BEGIN_STUDENTS_DAY:
			{
				ConstraintActivitiesBeginStudentsDay* ctr=(ConstraintActivitiesBeginStudentsDay*)oldtc;

				if(studentsComboBox->currentIndex()<0){
					showWarningCannotModifyConstraintInvisibleSubgroupConstraint(dialog, ctr->studentsName);
					return;
				}

				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				ctr->teacherName=teacher;
				ctr->studentsName=students;
				ctr->subjectName=subject;
				ctr->activityTagName=activityTag;

				break;
			}
		//197
		case CONSTRAINT_ACTIVITY_BEGINS_TEACHERS_DAY:
			{
				ConstraintActivityBeginsTeachersDay* ctr=(ConstraintActivityBeginsTeachersDay*)oldtc;

				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				ctr->activityId=id;

				break;
			}
		//198
		case CONSTRAINT_ACTIVITIES_BEGIN_TEACHERS_DAY:
			{
				ConstraintActivitiesBeginTeachersDay* ctr=(ConstraintActivitiesBeginTeachersDay*)oldtc;

				if(studentsComboBox->currentIndex()<0){
					showWarningCannotModifyConstraintInvisibleSubgroupConstraint(dialog, ctr->studentsName);
					return;
				}

				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				ctr->teacherName=teacher;
				ctr->studentsName=students;
				ctr->subjectName=subject;
				ctr->activityTagName=activityTag;

				break;
			}
		//199
		case CONSTRAINT_TEACHERS_MIN_HOURS_PER_AFTERNOON:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty afternoons check box must be checked. If you need to not allow empty afternoons for the teachers, "
						"please use the constraint teachers min afternoons per week."));
					return;
				}

				ConstraintTeachersMinHoursPerAfternoon* ctr=(ConstraintTeachersMinHoursPerAfternoon*)oldtc;

				ctr->minHoursPerAfternoon=spinBox->value();
				ctr->allowEmptyAfternoons=checkBox->isChecked();

				break;
			}
		//200
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON:
			{
				if(!checkBox->isChecked()){
					QMessageBox::warning(dialog, tr("FET information"), tr("Allow empty afternoons check box must be checked. If you need to not allow empty afternoons for a teacher, "
						"please use the constraint teacher min afternoons per week."));
					return;
				}

				ConstraintTeacherMinHoursPerAfternoon* ctr=(ConstraintTeacherMinHoursPerAfternoon*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();

				ctr->minHoursPerAfternoon=spinBox->value();
				ctr->allowEmptyAfternoons=checkBox->isChecked();

				break;
			}
		//201
		case CONSTRAINT_STUDENTS_MIN_HOURS_PER_AFTERNOON:
			{
				//if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					//2021-03-26 - I think I commented out this check because the user might combine this constraint with a min hours daily constraint.
					/*if(allowEmptyAfternoonsCheckBox->isChecked() && minHoursSpinBox->value()<2){
						QMessageBox::warning(dialog, tr("FET warning"), tr("If you allow empty afternoons, the min hours must be at least 2 (to make it a non-trivial constraint)"));
						return;
					}*/
				//}

				ConstraintStudentsMinHoursPerAfternoon* ctr=(ConstraintStudentsMinHoursPerAfternoon*)oldtc;

				ctr->minHoursPerAfternoon=spinBox->value();
				ctr->allowEmptyAfternoons=checkBox->isChecked();

				break;
			}
		//202
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_AFTERNOON:
			{
				//if(gt.rules.mode!=MORNINGS_AFTERNOONS){
					//2021-03-26 - I think I commented out this check because the user might combine this constraint with a min hours daily constraint.
					/*if(allowEmptyAfternoonsCheckBox->isChecked() && minHoursSpinBox->value()<2){
						QMessageBox::warning(dialog, tr("FET warning"), tr("If you allow empty afternoons, the min hours must be at least 2 (to make it a non-trivial constraint)"));
						return;
					}*/
				//}

				ConstraintStudentsSetMinHoursPerAfternoon* ctr=(ConstraintStudentsSetMinHoursPerAfternoon*)oldtc;

				ctr->students=studentsComboBox->currentText();

				ctr->minHoursPerAfternoon=spinBox->value();
				ctr->allowEmptyAfternoons=checkBox->isChecked();

				break;
			}
		//203
		case CONSTRAINT_ACTIVITIES_MAX_HOURLY_SPAN:
			{
				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"),	tr("Only one selected activity"));
					return;
				}

				ConstraintActivitiesMaxHourlySpan* ctr=(ConstraintActivitiesMaxHourlySpan*)oldtc;

				ctr->activitiesIds=selectedActivitiesList;
				ctr->n_activities=ctr->activitiesIds.count();
				ctr->recomputeActivitiesSet();

				ctr->maxHourlySpan=spinBox->value();

				break;
			}
		//204
		case CONSTRAINT_TEACHERS_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("If you need the maximum hours daily in the interval to be 0, please use"
						 " the constraints of type teacher not available times or activities preferred time slots."));
					return;
				}

				ConstraintTeachersMaxHoursDailyInInterval* ctr=(ConstraintTeachersMaxHoursDailyInInterval*)oldtc;

				ctr->maxHoursDaily=spinBox->value();

				ctr->startHour=startHour;
				ctr->endHour=endHour;

				break;
			}
		//205
		case CONSTRAINT_TEACHER_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("If you need the maximum hours daily in the interval to be 0, please use"
						 " the constraints of type teacher not available times or activities preferred time slots."));
					return;
				}

				ConstraintTeacherMaxHoursDailyInInterval* ctr=(ConstraintTeacherMaxHoursDailyInInterval*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();

				ctr->maxHoursDaily=spinBox->value();

				ctr->startHour=startHour;
				ctr->endHour=endHour;

				break;
			}
		//206
		case CONSTRAINT_STUDENTS_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("If you need the maximum hours daily in the interval to be 0, please use"
						 " the constraints of type students set not available times or activities preferred time slots."));
					return;
				}

				ConstraintStudentsMaxHoursDailyInInterval* ctr=(ConstraintStudentsMaxHoursDailyInInterval*)oldtc;

				ctr->maxHoursDaily=spinBox->value();

				ctr->startHour=startHour;
				ctr->endHour=endHour;

				break;
			}
		//207
		case CONSTRAINT_STUDENTS_SET_MAX_HOURS_DAILY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour invalid"));
					return;
				}
				if(endHour<=startHour){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Start hour cannot be greater or equal than end hour"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("If you need the maximum hours daily in the interval to be 0, please use"
						 " the constraints of type students set not available times or activities preferred time slots."));
					return;
				}

				ConstraintStudentsSetMaxHoursDailyInInterval* ctr=(ConstraintStudentsSetMaxHoursDailyInInterval*)oldtc;

				ctr->students=studentsComboBox->currentText();

				ctr->maxHoursDaily=spinBox->value();

				ctr->startHour=startHour;
				ctr->endHour=endHour;

				break;
			}
		//208
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay* ctr=(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDay*)oldtc;

				ctr->students=studentsComboBox->currentText();

				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				ctr->firstActivityTag=firstActivityTagName;
				ctr->secondActivityTag=secondActivityTagName;

				ctr->minGaps=spinBox->value();

				break;
			}
		//209
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay* ctr=(ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDay*)oldtc;

				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				ctr->firstActivityTag=firstActivityTagName;
				ctr->secondActivityTag=secondActivityTagName;

				ctr->minGaps=spinBox->value();

				break;
			}
		//210
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay* ctr=(ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDay*)oldtc;

				ctr->teacher=teachersComboBox->currentText();

				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				ctr->firstActivityTag=firstActivityTagName;
				ctr->secondActivityTag=secondActivityTagName;

				ctr->minGaps=spinBox->value();

				break;
			}
		//211
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_PER_REAL_DAY:
			{
				ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay* ctr=(ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDay*)oldtc;

				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				ctr->firstActivityTag=firstActivityTagName;
				ctr->secondActivityTag=secondActivityTagName;

				ctr->minGaps=spinBox->value();

				break;
			}
		//212
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				ConstraintStudentsSetMinGapsBetweenActivityTagPerRealDay* ctr=(ConstraintStudentsSetMinGapsBetweenActivityTagPerRealDay*)oldtc;

				ctr->students=studentsComboBox->currentText();

				ctr->activityTag=activityTagsComboBox->currentText();

				ctr->minGaps=spinBox->value();

				break;
			}
		//213
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				ConstraintStudentsMinGapsBetweenActivityTagPerRealDay* ctr=(ConstraintStudentsMinGapsBetweenActivityTagPerRealDay*)oldtc;

				ctr->activityTag=activityTagsComboBox->currentText();

				ctr->minGaps=spinBox->value();

				break;
			}
		//214
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				ConstraintTeacherMinGapsBetweenActivityTagPerRealDay* ctr=(ConstraintTeacherMinGapsBetweenActivityTagPerRealDay*)oldtc;

				ctr->teacher=teachersComboBox->currentText();

				ctr->activityTag=activityTagsComboBox->currentText();

				ctr->minGaps=spinBox->value();

				break;
			}
		//215
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_PER_REAL_DAY:
			{
				ConstraintTeachersMinGapsBetweenActivityTagPerRealDay* ctr=(ConstraintTeachersMinGapsBetweenActivityTagPerRealDay*)oldtc;

				ctr->activityTag=activityTagsComboBox->currentText();

				ctr->minGaps=spinBox->value();

				break;
			}
		//216
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon* ctr=(ConstraintStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon*)oldtc;

				ctr->students=studentsComboBox->currentText();

				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				ctr->firstActivityTag=firstActivityTagName;
				ctr->secondActivityTag=secondActivityTagName;

				ctr->minGaps=spinBox->value();

				break;
			}
		//217
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon* ctr=(ConstraintStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon*)oldtc;

				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				ctr->firstActivityTag=firstActivityTagName;
				ctr->secondActivityTag=secondActivityTagName;

				ctr->minGaps=spinBox->value();

				break;
			}
		//218
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon* ctr=(ConstraintTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon*)oldtc;

				ctr->teacher=teachersComboBox->currentText();

				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				ctr->firstActivityTag=firstActivityTagName;
				ctr->secondActivityTag=secondActivityTagName;

				ctr->minGaps=spinBox->value();

				break;
			}
		//219
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ORDERED_PAIR_OF_ACTIVITY_TAGS_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon* ctr=(ConstraintTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoon*)oldtc;

				QString firstActivityTagName=first_activityTagsComboBox->currentText();
				int facttagindex=gt.rules.searchActivityTag(firstActivityTagName);
				if(facttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid first activity tag"));
					return;
				}

				QString secondActivityTagName=second_activityTagsComboBox->currentText();
				int sacttagindex=gt.rules.searchActivityTag(secondActivityTagName);
				if(sacttagindex<0){
					QMessageBox::warning(dialog, tr("FET warning"), tr("Invalid second activity tag"));
					return;
				}

				if(firstActivityTagName==secondActivityTagName){
					QMessageBox::warning(dialog, tr("FET warning"), tr("The two activity tags cannot be the same"));
					return;
				}

				ctr->firstActivityTag=firstActivityTagName;
				ctr->secondActivityTag=secondActivityTagName;

				ctr->minGaps=spinBox->value();

				break;
			}
		//220
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon* ctr=(ConstraintStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoon*)oldtc;

				ctr->students=studentsComboBox->currentText();

				ctr->activityTag=activityTagsComboBox->currentText();

				ctr->minGaps=spinBox->value();

				break;
			}
		//221
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon* ctr=(ConstraintStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoon*)oldtc;

				ctr->activityTag=activityTagsComboBox->currentText();

				ctr->minGaps=spinBox->value();

				break;
			}
		//222
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon* ctr=(ConstraintTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoon*)oldtc;

				ctr->teacher=teachersComboBox->currentText();

				ctr->activityTag=activityTagsComboBox->currentText();

				ctr->minGaps=spinBox->value();

				break;
			}
		//223
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ACTIVITY_TAG_BETWEEN_MORNING_AND_AFTERNOON:
			{
				ConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon* ctr=(ConstraintTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoon*)oldtc;

				ctr->activityTag=activityTagsComboBox->currentText();

				ctr->minGaps=spinBox->value();

				break;
			}
		//224
		case CONSTRAINT_TEACHERS_NO_TWO_CONSECUTIVE_DAYS:
			{
				//ConstraintTeachersNoTwoConsecutiveDays* ctr=(ConstraintTeachersNoTwoConsecutiveDays*)oldtc;

				//nothing to be done

				break;
			}
		//225
		case CONSTRAINT_TEACHER_NO_TWO_CONSECUTIVE_DAYS:
			{
				ConstraintTeacherNoTwoConsecutiveDays* ctr=(ConstraintTeacherNoTwoConsecutiveDays*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();

				break;
			}
		//226
		case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				ConstraintTeacherPairOfMutuallyExclusiveTimeSlots* ctr=(ConstraintTeacherPairOfMutuallyExclusiveTimeSlots*)oldtc;

				int day1=firstDayComboBox->currentIndex();
				int hour1=firstHourComboBox->currentIndex();

				int day2=secondDayComboBox->currentIndex();
				int hour2=secondHourComboBox->currentIndex();

				if(day1<0 || day1>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's day invalid"));
					return;
				}
				if(hour1<0 || hour1>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's hour invalid"));
					return;
				}

				if(day2<0 || day2>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's day invalid"));
					return;
				}
				if(hour2<0 || hour2>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's hour invalid"));
					return;
				}

				if(day1==day2 && hour1==hour2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("The two time slots are the same", "It is a user error, the two time slots should not be equal"));
					return;
				}

				ctr->teacherName=teachersComboBox->currentText();

				ctr->day1=day1;
				ctr->hour1=hour1;

				ctr->day2=day2;
				ctr->hour2=hour2;

				break;
			}
		//227
		case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				ConstraintTeachersPairOfMutuallyExclusiveTimeSlots* ctr=(ConstraintTeachersPairOfMutuallyExclusiveTimeSlots*)oldtc;

				int day1=firstDayComboBox->currentIndex();
				int hour1=firstHourComboBox->currentIndex();

				int day2=secondDayComboBox->currentIndex();
				int hour2=secondHourComboBox->currentIndex();

				if(day1<0 || day1>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's day invalid"));
					return;
				}
				if(hour1<0 || hour1>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's hour invalid"));
					return;
				}

				if(day2<0 || day2>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's day invalid"));
					return;
				}
				if(hour2<0 || hour2>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's hour invalid"));
					return;
				}

				if(day1==day2 && hour1==hour2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("The two time slots are the same", "It is a user error, the two time slots should not be equal"));
					return;
				}

				ctr->day1=day1;
				ctr->hour1=hour1;

				ctr->day2=day2;
				ctr->hour2=hour2;

				break;
			}
		//228
		case CONSTRAINT_STUDENTS_SET_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				ConstraintStudentsSetPairOfMutuallyExclusiveTimeSlots* ctr=(ConstraintStudentsSetPairOfMutuallyExclusiveTimeSlots*)oldtc;

				int day1=firstDayComboBox->currentIndex();
				int hour1=firstHourComboBox->currentIndex();

				int day2=secondDayComboBox->currentIndex();
				int hour2=secondHourComboBox->currentIndex();

				if(day1<0 || day1>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's day invalid"));
					return;
				}
				if(hour1<0 || hour1>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's hour invalid"));
					return;
				}

				if(day2<0 || day2>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's day invalid"));
					return;
				}
				if(hour2<0 || hour2>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's hour invalid"));
					return;
				}

				if(day1==day2 && hour1==hour2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("The two time slots are the same", "It is a user error, the two time slots should not be equal"));
					return;
				}

				ctr->students=studentsComboBox->currentText();

				ctr->day1=day1;
				ctr->hour1=hour1;

				ctr->day2=day2;
				ctr->hour2=hour2;

				break;
			}
		//229
		case CONSTRAINT_STUDENTS_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				ConstraintStudentsPairOfMutuallyExclusiveTimeSlots* ctr=(ConstraintStudentsPairOfMutuallyExclusiveTimeSlots*)oldtc;

				int day1=firstDayComboBox->currentIndex();
				int hour1=firstHourComboBox->currentIndex();

				int day2=secondDayComboBox->currentIndex();
				int hour2=secondHourComboBox->currentIndex();

				if(day1<0 || day1>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's day invalid"));
					return;
				}
				if(hour1<0 || hour1>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's hour invalid"));
					return;
				}

				if(day2<0 || day2>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's day invalid"));
					return;
				}
				if(hour2<0 || hour2>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's hour invalid"));
					return;
				}

				if(day1==day2 && hour1==hour2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("The two time slots are the same", "It is a user error, the two time slots should not be equal"));
					return;
				}

				ctr->day1=day1;
				ctr->hour1=hour1;

				ctr->day2=day2;
				ctr->hour2=hour2;

				break;
			}
		//230
		case CONSTRAINT_TWO_SETS_OF_ACTIVITIES_SAME_SECTIONS:
			{
				ConstraintTwoSetsOfActivitiesSameSections* ctr=(ConstraintTwoSetsOfActivitiesSameSections*)oldtc;

				if(selectedActivitiesList_TwoSetsOfActivities_1.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities in the first set"));
					return;
				}
				if(selectedActivitiesList_TwoSetsOfActivities_2.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Empty list of selected activities in the second set"));
					return;
				}

				ctr->activitiesAIds=selectedActivitiesList_TwoSetsOfActivities_1;
				ctr->activitiesBIds=selectedActivitiesList_TwoSetsOfActivities_2;
				
				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				ctr->oDays=days;
				ctr->oHours=hours;

				ctr->recomputeActivitiesSets();

				break;
			}
		//231
		case CONSTRAINT_STUDENTS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				ConstraintStudentsMaxSingleGapsInSelectedTimeSlots* ctr=(ConstraintStudentsMaxSingleGapsInSelectedTimeSlots*)oldtc;

				ctr->maxSingleGaps=spinBox->value();

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				ctr->selectedDays=days;
				ctr->selectedHours=hours;

				break;
			}
		//232
		case CONSTRAINT_STUDENTS_SET_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				ConstraintStudentsSetMaxSingleGapsInSelectedTimeSlots* ctr=(ConstraintStudentsSetMaxSingleGapsInSelectedTimeSlots*)oldtc;

				ctr->students=studentsComboBox->currentText();
				ctr->maxSingleGaps=spinBox->value();

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				ctr->selectedDays=days;
				ctr->selectedHours=hours;

				break;
			}
		//233
		case CONSTRAINT_TEACHERS_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				ConstraintTeachersMaxSingleGapsInSelectedTimeSlots* ctr=(ConstraintTeachersMaxSingleGapsInSelectedTimeSlots*)oldtc;

				ctr->maxSingleGaps=spinBox->value();

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				ctr->selectedDays=days;
				ctr->selectedHours=hours;

				break;
			}
		//234
		case CONSTRAINT_TEACHER_MAX_SINGLE_GAPS_IN_SELECTED_TIME_SLOTS:
			{
				ConstraintTeacherMaxSingleGapsInSelectedTimeSlots* ctr=(ConstraintTeacherMaxSingleGapsInSelectedTimeSlots*)oldtc;

				ctr->teacher=teachersComboBox->currentText();

				ctr->maxSingleGaps=spinBox->value();

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				ctr->selectedDays=days;
				ctr->selectedHours=hours;

				break;
			}
		//235
		case CONSTRAINT_TEACHER_MAX_HOURS_PER_TERM:
			{
				ConstraintTeacherMaxHoursPerTerm* ctr=(ConstraintTeacherMaxHoursPerTerm*)oldtc;

				ctr->teacherName=teachersComboBox->currentText();
				ctr->maxHoursPerTerm=spinBox->value();

				break;
			}
		//236
		case CONSTRAINT_TEACHERS_MAX_HOURS_PER_TERM:
			{
				ConstraintTeachersMaxHoursPerTerm* ctr=(ConstraintTeachersMaxHoursPerTerm*)oldtc;

				ctr->maxHoursPerTerm=spinBox->value();

				break;
			}
		//237
		case CONSTRAINT_TEACHER_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				ConstraintTeacherPairOfMutuallyExclusiveSetsOfTimeSlots* ctr=(ConstraintTeacherPairOfMutuallyExclusiveSetsOfTimeSlots*)oldtc;

				QList<int> days1;
				QList<int> hours1;
				getTimesTable(timesTable1, days1, hours1, true);
				
				assert(days1.count()==hours1.count());
				if(days1.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the first set of time slots"));
					return;
				}

				QList<int> days2;
				QList<int> hours2;
				getTimesTable(timesTable2, days2, hours2, true);
				
				assert(days2.count()==hours2.count());
				if(days2.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the second set of time slots"));
					return;
				}
				
				QSet<QPair<int, int>> set1;
				QSet<QPair<int, int>> set2;
				for(int i=0; i<days1.count(); i++)
					set1.insert(QPair<int, int>(days1.at(i), hours1.at(i)));
				for(int i=0; i<days2.count(); i++)
					set2.insert(QPair<int, int>(days2.at(i), hours2.at(i)));
				if(set1.intersects(set2)){
					QSet<QPair<int, int>> intersectionSet = set1 & set2;
					QList<QPair<int, int>> intersectionList(intersectionSet.constBegin(), intersectionSet.constEnd());
					std::stable_sort(intersectionList.begin(), intersectionList.end());
					
					QStringList cts;
					for(const QPair<int, int>& pr : std::as_const(intersectionList))
						cts.append(gt.rules.daysOfTheWeek[pr.first]+QString(" ")+gt.rules.hoursOfTheDay[pr.second]);
					
					LongTextMessageBox::information(dialog, tr("FET information"),
						 tr("The two sets of selected time slots cannot have common time slots. The common time slots are: %1.")
						 .arg(cts.join(translatedCommaSpace())));
					return;
				}

				ctr->teacherName=teachersComboBox->currentText();

				ctr->selectedDays1=days1;
				ctr->selectedHours1=hours1;

				ctr->selectedDays2=days2;
				ctr->selectedHours2=hours2;

				break;
			}
		//238
		case CONSTRAINT_TEACHERS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				ConstraintTeachersPairOfMutuallyExclusiveSetsOfTimeSlots* ctr=(ConstraintTeachersPairOfMutuallyExclusiveSetsOfTimeSlots*)oldtc;

				QList<int> days1;
				QList<int> hours1;
				getTimesTable(timesTable1, days1, hours1, true);
				
				assert(days1.count()==hours1.count());
				if(days1.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the first set of time slots"));
					return;
				}

				QList<int> days2;
				QList<int> hours2;
				getTimesTable(timesTable2, days2, hours2, true);
				
				assert(days2.count()==hours2.count());
				if(days2.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the second set of time slots"));
					return;
				}
				
				QSet<QPair<int, int>> set1;
				QSet<QPair<int, int>> set2;
				for(int i=0; i<days1.count(); i++)
					set1.insert(QPair<int, int>(days1.at(i), hours1.at(i)));
				for(int i=0; i<days2.count(); i++)
					set2.insert(QPair<int, int>(days2.at(i), hours2.at(i)));
				if(set1.intersects(set2)){
					QSet<QPair<int, int>> intersectionSet = set1 & set2;
					QList<QPair<int, int>> intersectionList(intersectionSet.constBegin(), intersectionSet.constEnd());
					std::stable_sort(intersectionList.begin(), intersectionList.end());
					
					QStringList cts;
					for(const QPair<int, int>& pr : std::as_const(intersectionList))
						cts.append(gt.rules.daysOfTheWeek[pr.first]+QString(" ")+gt.rules.hoursOfTheDay[pr.second]);
					
					LongTextMessageBox::information(dialog, tr("FET information"),
						 tr("The two sets of selected time slots cannot have common time slots. The common time slots are: %1.")
						 .arg(cts.join(translatedCommaSpace())));
					return;
				}

				ctr->selectedDays1=days1;
				ctr->selectedHours1=hours1;

				ctr->selectedDays2=days2;
				ctr->selectedHours2=hours2;

				break;
			}
		//239
		case CONSTRAINT_STUDENTS_SET_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				ConstraintStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots* ctr=(ConstraintStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlots*)oldtc;

				QList<int> days1;
				QList<int> hours1;
				getTimesTable(timesTable1, days1, hours1, true);
				
				assert(days1.count()==hours1.count());
				if(days1.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the first set of time slots"));
					return;
				}

				QList<int> days2;
				QList<int> hours2;
				getTimesTable(timesTable2, days2, hours2, true);
				
				assert(days2.count()==hours2.count());
				if(days2.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the second set of time slots"));
					return;
				}
				
				QSet<QPair<int, int>> set1;
				QSet<QPair<int, int>> set2;
				for(int i=0; i<days1.count(); i++)
					set1.insert(QPair<int, int>(days1.at(i), hours1.at(i)));
				for(int i=0; i<days2.count(); i++)
					set2.insert(QPair<int, int>(days2.at(i), hours2.at(i)));
				if(set1.intersects(set2)){
					QSet<QPair<int, int>> intersectionSet = set1 & set2;
					QList<QPair<int, int>> intersectionList(intersectionSet.constBegin(), intersectionSet.constEnd());
					std::stable_sort(intersectionList.begin(), intersectionList.end());
					
					QStringList cts;
					for(const QPair<int, int>& pr : std::as_const(intersectionList))
						cts.append(gt.rules.daysOfTheWeek[pr.first]+QString(" ")+gt.rules.hoursOfTheDay[pr.second]);
					
					LongTextMessageBox::information(dialog, tr("FET information"),
						 tr("The two sets of selected time slots cannot have common time slots. The common time slots are: %1.")
						 .arg(cts.join(translatedCommaSpace())));
					return;
				}

				ctr->selectedDays1=days1;
				ctr->selectedHours1=hours1;

				ctr->selectedDays2=days2;
				ctr->selectedHours2=hours2;

				break;
			}
		//240
		case CONSTRAINT_STUDENTS_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				ConstraintStudentsPairOfMutuallyExclusiveSetsOfTimeSlots* ctr=(ConstraintStudentsPairOfMutuallyExclusiveSetsOfTimeSlots*)oldtc;

				QList<int> days1;
				QList<int> hours1;
				getTimesTable(timesTable1, days1, hours1, true);
				
				assert(days1.count()==hours1.count());
				if(days1.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the first set of time slots"));
					return;
				}

				QList<int> days2;
				QList<int> hours2;
				getTimesTable(timesTable2, days2, hours2, true);
				
				assert(days2.count()==hours2.count());
				if(days2.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the second set of time slots"));
					return;
				}
				
				QSet<QPair<int, int>> set1;
				QSet<QPair<int, int>> set2;
				for(int i=0; i<days1.count(); i++)
					set1.insert(QPair<int, int>(days1.at(i), hours1.at(i)));
				for(int i=0; i<days2.count(); i++)
					set2.insert(QPair<int, int>(days2.at(i), hours2.at(i)));
				if(set1.intersects(set2)){
					QSet<QPair<int, int>> intersectionSet = set1 & set2;
					QList<QPair<int, int>> intersectionList(intersectionSet.constBegin(), intersectionSet.constEnd());
					std::stable_sort(intersectionList.begin(), intersectionList.end());
					
					QStringList cts;
					for(const QPair<int, int>& pr : std::as_const(intersectionList))
						cts.append(gt.rules.daysOfTheWeek[pr.first]+QString(" ")+gt.rules.hoursOfTheDay[pr.second]);
					
					LongTextMessageBox::information(dialog, tr("FET information"),
						 tr("The two sets of selected time slots cannot have common time slots. The common time slots are: %1.")
						 .arg(cts.join(translatedCommaSpace())));
					return;
				}

				ctr->selectedDays1=days1;
				ctr->selectedHours1=hours1;

				ctr->selectedDays2=days2;
				ctr->selectedHours2=hours2;

				break;
			}
		//241
		case CONSTRAINT_ACTIVITIES_PAIR_OF_MUTUALLY_EXCLUSIVE_SETS_OF_TIME_SLOTS:
			{
				ConstraintActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots* ctr=(ConstraintActivitiesPairOfMutuallyExclusiveSetsOfTimeSlots*)oldtc;

				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				/*if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"), tr("Only one selected activity"));
					return;
				}*/

				QList<int> days1;
				QList<int> hours1;
				getTimesTable(timesTable1, days1, hours1, true);
				
				assert(days1.count()==hours1.count());
				if(days1.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the first set of time slots"));
					return;
				}

				QList<int> days2;
				QList<int> hours2;
				getTimesTable(timesTable2, days2, hours2, true);
				
				assert(days2.count()==hours2.count());
				if(days2.count()==0){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least one time slot in the second set of time slots"));
					return;
				}
				
				QSet<QPair<int, int>> set1;
				QSet<QPair<int, int>> set2;
				for(int i=0; i<days1.count(); i++)
					set1.insert(QPair<int, int>(days1.at(i), hours1.at(i)));
				for(int i=0; i<days2.count(); i++)
					set2.insert(QPair<int, int>(days2.at(i), hours2.at(i)));
				if(set1.intersects(set2)){
					QSet<QPair<int, int>> intersectionSet = set1 & set2;
					QList<QPair<int, int>> intersectionList(intersectionSet.constBegin(), intersectionSet.constEnd());
					std::stable_sort(intersectionList.begin(), intersectionList.end());
					
					QStringList cts;
					for(const QPair<int, int>& pr : std::as_const(intersectionList))
						cts.append(gt.rules.daysOfTheWeek[pr.first]+QString(" ")+gt.rules.hoursOfTheDay[pr.second]);
					
					LongTextMessageBox::information(dialog, tr("FET information"),
						 tr("The two sets of selected time slots cannot have common time slots. The common time slots are: %1.")
						 .arg(cts.join(translatedCommaSpace())));
					return;
				}

				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				ctr->selectedDays1=days1;
				ctr->selectedHours1=hours1;

				ctr->selectedDays2=days2;
				ctr->selectedHours2=hours2;

				break;
			}
		//242
		case CONSTRAINT_ACTIVITIES_PAIR_OF_MUTUALLY_EXCLUSIVE_TIME_SLOTS:
			{
				ConstraintActivitiesPairOfMutuallyExclusiveTimeSlots* ctr=(ConstraintActivitiesPairOfMutuallyExclusiveTimeSlots*)oldtc;

				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				/*if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"), tr("Only one selected activity"));
					return;
				}*/

				int day1=firstDayComboBox->currentIndex();
				int hour1=firstHourComboBox->currentIndex();

				int day2=secondDayComboBox->currentIndex();
				int hour2=secondHourComboBox->currentIndex();

				if(day1<0 || day1>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's day invalid"));
					return;
				}
				if(hour1<0 || hour1>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("First time slot's hour invalid"));
					return;
				}

				if(day2<0 || day2>=gt.rules.nDaysPerWeek){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's day invalid"));
					return;
				}
				if(hour2<0 || hour2>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Second time slot's hour invalid"));
					return;
				}

				if(day1==day2 && hour1==hour2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("The two time slots are the same", "It is a user error, the two time slots should not be equal"));
					return;
				}

				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				ctr->day1=day1;
				ctr->hour1=hour1;

				ctr->day2=day2;
				ctr->hour2=hour2;

				break;
			}
		//243
		case CONSTRAINT_TEACHER_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				ConstraintTeacherOccupiesMaxSetsOfTimeSlotsFromSelection* ctr=(ConstraintTeacherOccupiesMaxSetsOfTimeSlotsFromSelection*)oldtc;

				QList<QList<int>> days;
				QList<QList<int>> hours;
				getSpinBoxTimesTable(days, hours);
				
				assert(days.count()==hours.count());
				if(days.count()<=1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two sets of time slots."));
					return;
				}
				
				for(int i=0; i<days.count(); i++){
					const QList<int>& cdl=days.at(i);
					const QList<int>& chl=hours.at(i);
					assert(cdl.count()==chl.count());
					if(cdl.count()==0){
						QMessageBox::warning(dialog, tr("FET information"),
							tr("Set number %1 of selected time slots is empty - please correct this"
							 " (the selected numbers must start from 1, and they must be continuous, without any gap between them).").arg(i+1));
						return;
					}
				}
				
				ctr->teacherName=teachersComboBox->currentText();

				ctr->selectedDays=days;
				ctr->selectedHours=hours;

				ctr->maxOccupiedSets=spinBox->value();

				break;
			}
		//244
		case CONSTRAINT_TEACHERS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				ConstraintTeachersOccupyMaxSetsOfTimeSlotsFromSelection* ctr=(ConstraintTeachersOccupyMaxSetsOfTimeSlotsFromSelection*)oldtc;

				QList<QList<int>> days;
				QList<QList<int>> hours;
				getSpinBoxTimesTable(days, hours);
				
				assert(days.count()==hours.count());
				if(days.count()<=1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two sets of time slots."));
					return;
				}
				
				for(int i=0; i<days.count(); i++){
					const QList<int>& cdl=days.at(i);
					const QList<int>& chl=hours.at(i);
					assert(cdl.count()==chl.count());
					if(cdl.count()==0){
						QMessageBox::warning(dialog, tr("FET information"),
							tr("Set number %1 of selected time slots is empty - please correct this"
							 " (the selected numbers must start from 1, and they must be continuous, without any gap between them).").arg(i+1));
						return;
					}
				}
				
				ctr->selectedDays=days;
				ctr->selectedHours=hours;

				ctr->maxOccupiedSets=spinBox->value();

				break;
			}
		//245
		case CONSTRAINT_STUDENTS_SET_OCCUPIES_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				ConstraintStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection* ctr=(ConstraintStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelection*)oldtc;

				QList<QList<int>> days;
				QList<QList<int>> hours;
				getSpinBoxTimesTable(days, hours);
				
				assert(days.count()==hours.count());
				if(days.count()<=1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two sets of time slots."));
					return;
				}
				
				for(int i=0; i<days.count(); i++){
					const QList<int>& cdl=days.at(i);
					const QList<int>& chl=hours.at(i);
					assert(cdl.count()==chl.count());
					if(cdl.count()==0){
						QMessageBox::warning(dialog, tr("FET information"),
							tr("Set number %1 of selected time slots is empty - please correct this"
							 " (the selected numbers must start from 1, and they must be continuous, without any gap between them).").arg(i+1));
						return;
					}
				}
				
				ctr->students=studentsComboBox->currentText();

				ctr->selectedDays=days;
				ctr->selectedHours=hours;

				ctr->maxOccupiedSets=spinBox->value();

				break;
			}
		//246
		case CONSTRAINT_STUDENTS_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				ConstraintStudentsOccupyMaxSetsOfTimeSlotsFromSelection* ctr=(ConstraintStudentsOccupyMaxSetsOfTimeSlotsFromSelection*)oldtc;

				QList<QList<int>> days;
				QList<QList<int>> hours;
				getSpinBoxTimesTable(days, hours);
				
				assert(days.count()==hours.count());
				if(days.count()<=1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two sets of time slots."));
					return;
				}
				
				for(int i=0; i<days.count(); i++){
					const QList<int>& cdl=days.at(i);
					const QList<int>& chl=hours.at(i);
					assert(cdl.count()==chl.count());
					if(cdl.count()==0){
						QMessageBox::warning(dialog, tr("FET information"),
							tr("Set number %1 of selected time slots is empty - please correct this"
							 " (the selected numbers must start from 1, and they must be continuous, without any gap between them).").arg(i+1));
						return;
					}
				}
				
				ctr->selectedDays=days;
				ctr->selectedHours=hours;

				ctr->maxOccupiedSets=spinBox->value();

				break;
			}
		//247
		case CONSTRAINT_ACTIVITIES_OVERLAP_COMPLETELY_OR_DO_NOT_OVERLAP:
			{
				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"), tr("Only one selected activity"));
					return;
				}

				int dur=-1;
				for(int actId : std::as_const(selectedActivitiesList)){
					Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
					assert(act!=nullptr);
					if(dur==-1 || dur==act->duration){
						dur=act->duration;
					}
					else{
						QMessageBox::warning(dialog, tr("FET information"),
							tr("All the selected activities should have the same duration"));
						return;
					}
				}
				if(dur==1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("The constraint is useless, because the durations of the activities are 1"));
					return;
				}

				ConstraintActivitiesOverlapCompletelyOrDoNotOverlap* ctr=(ConstraintActivitiesOverlapCompletelyOrDoNotOverlap*)oldtc;
				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				break;
			}
		//248
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_SETS_OF_TIME_SLOTS_FROM_SELECTION:
			{
				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"), tr("Only one selected activity"));
					return;
				}

				QList<QList<int>> days;
				QList<QList<int>> hours;
				getSpinBoxTimesTable(days, hours);
				
				assert(days.count()==hours.count());
				if(days.count()<=1){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("Please select at least two sets of time slots."));
					return;
				}
				
				for(int i=0; i<days.count(); i++){
					const QList<int>& cdl=days.at(i);
					const QList<int>& chl=hours.at(i);
					assert(cdl.count()==chl.count());
					if(cdl.count()==0){
						QMessageBox::warning(dialog, tr("FET information"),
							tr("Set number %1 of selected time slots is empty - please correct this"
							 " (the selected numbers must start from 1, and they must be continuous, without any gap between them).").arg(i+1));
						return;
					}
				}
				
				ConstraintActivitiesOccupyMaxSetsOfTimeSlotsFromSelection* ctr=(ConstraintActivitiesOccupyMaxSetsOfTimeSlotsFromSelection*)oldtc;

				ctr->selectedDays=days;
				ctr->selectedHours=hours;

				ctr->maxOccupiedSets=spinBox->value();

				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				break;
			}
		//249
		case CONSTRAINT_ACTIVITY_BEGINS_OR_ENDS_STUDENTS_DAY:
			{
				ConstraintActivityBeginsOrEndsStudentsDay* ctr=(ConstraintActivityBeginsOrEndsStudentsDay*)oldtc;

				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				ctr->activityId=id;

				break;
			}
		//250
		case CONSTRAINT_ACTIVITIES_BEGIN_OR_END_STUDENTS_DAY:
			{
				ConstraintActivitiesBeginOrEndStudentsDay* ctr=(ConstraintActivitiesBeginOrEndStudentsDay*)oldtc;

				if(studentsComboBox->currentIndex()<0){
					showWarningCannotModifyConstraintInvisibleSubgroupConstraint(dialog, ctr->studentsName);
					return;
				}

				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				ctr->teacherName=teacher;
				ctr->studentsName=students;
				ctr->subjectName=subject;
				ctr->activityTagName=activityTag;

				break;
			}
		//251
		case CONSTRAINT_ACTIVITY_BEGINS_OR_ENDS_TEACHERS_DAY:
			{
				ConstraintActivityBeginsOrEndsTeachersDay* ctr=(ConstraintActivityBeginsOrEndsTeachersDay*)oldtc;

				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				ctr->activityId=id;

				break;
			}
		//252
		case CONSTRAINT_ACTIVITIES_BEGIN_OR_END_TEACHERS_DAY:
			{
				ConstraintActivitiesBeginOrEndTeachersDay* ctr=(ConstraintActivitiesBeginOrEndTeachersDay*)oldtc;

				if(studentsComboBox->currentIndex()<0){
					showWarningCannotModifyConstraintInvisibleSubgroupConstraint(dialog, ctr->studentsName);
					return;
				}

				QString teacher=teachersComboBox->currentText();
				if(teacher!="")
					assert(gt.rules.searchTeacher(teacher)>=0);

				QString students=studentsComboBox->currentText();
				if(students!="")
					assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString subject=subjectsComboBox->currentText();
				if(subject!="")
					assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				if(activityTag!="")
					assert(gt.rules.searchActivityTag(activityTag)>=0);

				ctr->teacherName=teacher;
				ctr->studentsName=students;
				ctr->subjectName=subject;
				ctr->activityTagName=activityTag;

				break;
			}
		//253
		case CONSTRAINT_ACTIVITIES_MAX_TOTAL_NUMBER_OF_STUDENTS_IN_SELECTED_TIME_SLOTS:
			{
				if(selectedActivitiesList.count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of activities"));
					return;
				}

				if(spinBox->value()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("You specified the max total number of students to be 0. This is "
					 "not perfect from efficiency point of view, because you can use instead constraint activity(ies) preferred time slots, "
					 "and help FET to find a timetable easier and faster, with an equivalent result. Please correct."));
					return;
				}

				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				ConstraintActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots* ctr=(ConstraintActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlots*)oldtc;
				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				ctr->selectedDays=days;
				ctr->selectedHours=hours;

				ctr->maxNumberOfStudents=spinBox->value();

				break;
			}

		default:
			assert(0);
			break;
	}
	
	oldtc->weightPercentage=weight;

	QString newcs=oldtc->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);

	if(oldtc->type==CONSTRAINT_ACTIVITY_PREFERRED_STARTING_TIME){
		LockUnlock::computeLockedUnlockedActivitiesOnlyTime();
		LockUnlock::increaseCommunicationSpinBox();
	}

	dialog->close();
}

void AddOrModifyTimeConstraint::cancelClicked()
{
	dialog->close();
}

/*void AddOrModifyTimeConstraint::colorItem(QTableWidgetItem* item)
{
	if(USE_GUI_COLORS){
		if(item->text()==NO)
			item->setBackground(QBrush(QColorConstants::DarkGreen));
		else
			item->setBackground(QBrush(QColorConstants::DarkRed));
		item->setForeground(QBrush(QColorConstants::LightGray));
	}
}*/

void AddOrModifyTimeConstraint::horizontalHeaderClicked(int col)
{
	horizontalHeaderClickedTimesTable(timesTable, col);
	/*highlightOnHorizontalHeaderClicked(timesTable, col);

	if(col>=0 && col<gt.rules.nDaysPerWeek){
		QString s=timesTable->item(0, col)->text();
		if(s==YES)
			s=NO;
		else{
			assert(s==NO);
			s=YES;
		}

		for(int row=0; row<gt.rules.nHoursPerDay; row++){
			timesTable->item(row, col)->setText(s);
			colorItem(timesTable->item(row,col));
		}
	}*/
}

void AddOrModifyTimeConstraint::verticalHeaderClicked(int row)
{
	verticalHeaderClickedTimesTable(timesTable, row);
	/*highlightOnVerticalHeaderClicked(timesTable, row);

	if(row>=0 && row<gt.rules.nHoursPerDay){
		QString s=timesTable->item(row, 0)->text();
		if(s==YES)
			s=NO;
		else{
			assert(s==NO);
			s=YES;
		}

		for(int col=0; col<gt.rules.nDaysPerWeek; col++){
			timesTable->item(row, col)->setText(s);
			colorItem(timesTable->item(row,col));
		}
	}*/
}

void AddOrModifyTimeConstraint::cellEntered(int row, int col)
{
	cellEnteredTimesTable(timesTable, row, col);
	/*highlightOnCellEntered(timesTable, row, col);*/
}

void AddOrModifyTimeConstraint::colorsCheckBoxToggled()
{
	timesTable->useColors=colorsCheckBox->isChecked();
	
	colorsCheckBoxToggledTimesTable(timesTable);
}

void AddOrModifyTimeConstraint::toggleAllClicked()
{
	toggleAllClickedTimesTable(timesTable);
	/*QString newText;
	if(timesTable->item(0, 0)->text()==NO)
		newText=YES;
	else
		newText=NO;
	for(int i=0; i<gt.rules.nHoursPerDay; i++)
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			timesTable->item(i, j)->setText(newText);
			colorItem(timesTable->item(i,j));
		}*/
}

void AddOrModifyTimeConstraint::itemClicked(QTableWidgetItem* item)
{
	itemClickedTimesTable(timesTable, item);
	/*QString s=item->text();
	if(s==YES)
		s=NO;
	else{
		assert(s==NO);
		s=YES;
	}
	item->setText(s);
	colorItem(item);*/
}

void AddOrModifyTimeConstraint::horizontalHeaderClicked1(int col)
{
	horizontalHeaderClickedTimesTable(timesTable1, col);
}

void AddOrModifyTimeConstraint::verticalHeaderClicked1(int row)
{
	verticalHeaderClickedTimesTable(timesTable1, row);
}

void AddOrModifyTimeConstraint::cellEntered1(int row, int col)
{
	cellEnteredTimesTable(timesTable1, row, col);
}

void AddOrModifyTimeConstraint::colorsCheckBoxToggled1()
{
	timesTable1->useColors=colorsCheckBox1->isChecked();
	
	colorsCheckBoxToggledTimesTable(timesTable1);
}

void AddOrModifyTimeConstraint::toggleAllClicked1()
{
	toggleAllClickedTimesTable(timesTable1);
}

void AddOrModifyTimeConstraint::itemClicked1(QTableWidgetItem* item)
{
	itemClickedTimesTable(timesTable1, item);
}

void AddOrModifyTimeConstraint::horizontalHeaderClicked2(int col)
{
	horizontalHeaderClickedTimesTable(timesTable2, col);
}

void AddOrModifyTimeConstraint::verticalHeaderClicked2(int row)
{
	verticalHeaderClickedTimesTable(timesTable2, row);
}

void AddOrModifyTimeConstraint::cellEntered2(int row, int col)
{
	cellEnteredTimesTable(timesTable2, row, col);
}

void AddOrModifyTimeConstraint::colorsCheckBoxToggled2()
{
	timesTable2->useColors=colorsCheckBox2->isChecked();
	
	colorsCheckBoxToggledTimesTable(timesTable2);
}

void AddOrModifyTimeConstraint::toggleAllClicked2()
{
	toggleAllClickedTimesTable(timesTable2);
}

void AddOrModifyTimeConstraint::itemClicked2(QTableWidgetItem* item)
{
	itemClickedTimesTable(timesTable2, item);
}

void AddOrModifyTimeConstraint::helpClicked()
{
	switch(type){
		//3
		case CONSTRAINT_TEACHER_NOT_AVAILABLE_TIMES:
			{
				QString s;

				s=tr("This constraint does not induce gaps for teachers. If a teacher has activities"
				 " before and after a not available period, gaps will not be counted.");

				s+=QString("\n\n");

				s+=tr("If you really need to use weight under 100%, you can use activities preferred times with"
				 " only the teacher specified, but this might generate problems, as possible gaps will be"
				 " counted and you may obtain an impossible timetable.");

				QMessageBox::information(dialog, tr("FET help"), s);

				break;
			}
		//17
		case CONSTRAINT_STUDENTS_SET_NOT_AVAILABLE_TIMES:
			{
				QString s;

				s=tr("This constraint does not induce gaps (or early gaps) for students. If a student set has activities"
				 " before and after a not available period, gaps will not be counted.");

				s+=QString("\n\n");

				s+=tr("If you really need to use weight under 100%, you can use activities preferred times with"
				 " only the students set specified, but this might generate problems, as possible gaps will be"
				 " counted and you may obtain an impossible timetable.");

				QMessageBox::information(dialog, tr("FET help"), s);

				break;
			}
		//28
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME:
			{
				QString s;

				s=tr("Add multiple constraints: this is a check box. Select this if you want to input only the representatives of subactivities and FET to add multiple constraints,"
				" for all subactivities from the same larger split activity, in turn, respectively."
				" There will be added more constraints activities same starting time, one for each corresponding tuple. The number of"
				" subactivities must match for the representatives and be careful to the order, to be what you need");

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}

		default:
			assert(0);
			break;
	}
}

void AddOrModifyTimeConstraint::checkBoxToggled()
{
	switch(type){
		//11
		case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY:
			{
				bool k=checkBox->isChecked();

				if(!k){
					checkBox->setChecked(true);
					if(gt.rules.mode!=MORNINGS_AFTERNOONS)
						QMessageBox::information(dialog, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty days for the teachers,"
							" please use constraint teachers min days per week"));
					else
						QMessageBox::information(dialog, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty days for this teacher,"
							" please use constraint teacher min days per week (but the min days per week constraint is for real days. You can also use the "
							"constraints teacher min mornings/afternoons per week.)"));
				}

				break;
			}
		//12
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY:
			{
				bool k=checkBox->isChecked();

				if(!k){
					checkBox->setChecked(true);
					if(gt.rules.mode!=MORNINGS_AFTERNOONS)
						QMessageBox::information(dialog, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty days for this teacher,"
							" please use constraint teacher min days per week"));
					else
						QMessageBox::information(dialog, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty days for this teacher,"
							" please use constraint teacher min days per week (but the min days per week constraint is for real days. You can also use the "
							"constraints teacher min mornings/afternoons per week.)"));
				}

				break;
			}
		//24
		case CONSTRAINT_STUDENTS_MIN_HOURS_DAILY:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//25
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_DAILY:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//28
		case CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME:
			{
				selectedActivitiesListWidget->clear();
				selectedActivitiesList.clear();

				filterActivitiesListWidget();

				break;
			}
		//30
		case CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//67
		case CONSTRAINT_TEACHER_MAX_SPAN_PER_DAY:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//68
		case CONSTRAINT_TEACHERS_MAX_SPAN_PER_DAY:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//69
		case CONSTRAINT_TEACHER_MIN_RESTING_HOURS:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//70
		case CONSTRAINT_TEACHERS_MIN_RESTING_HOURS:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//73
		case CONSTRAINT_STUDENTS_SET_MIN_RESTING_HOURS:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//74
		case CONSTRAINT_STUDENTS_MIN_RESTING_HOURS:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//83
		case CONSTRAINT_TEACHERS_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//84
		case CONSTRAINT_TEACHER_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//85
		case CONSTRAINT_STUDENTS_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//86
		case CONSTRAINT_STUDENTS_SET_ACTIVITY_TAG_MIN_HOURS_DAILY:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//115
		case CONSTRAINT_TEACHERS_MAX_GAPS_PER_REAL_DAY:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//116
		case CONSTRAINT_TEACHER_MAX_GAPS_PER_REAL_DAY:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//119
		case CONSTRAINT_TEACHERS_MIN_HOURS_DAILY_REAL_DAYS:
			{
				bool k=checkBox->isChecked();

				if(!k){
					checkBox->setChecked(true);
					QMessageBox::information(dialog, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty days for the teachers,"
						" please use constraint teachers min days per week"));
				}

				break;
			}
		//120
		case CONSTRAINT_TEACHER_MIN_HOURS_DAILY_REAL_DAYS:
			{
				bool k=checkBox->isChecked();

				if(!k){
					checkBox->setChecked(true);
					QMessageBox::information(dialog, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty days for this teacher,"
						" please use constraint teacher min days per week"));
				}

				break;
			}
		//123
		case CONSTRAINT_TEACHERS_MIN_HOURS_PER_MORNING:
			{
				bool k=checkBox->isChecked();

				if(!k){
					checkBox->setChecked(true);
					QMessageBox::information(dialog, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty mornings for the teachers,"
						" please use constraint teachers min mornings per week."));
				}

				break;
			}
		//124
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_MORNING:
			{
				bool k=checkBox->isChecked();

				if(!k){
					checkBox->setChecked(true);
					QMessageBox::information(dialog, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty mornings for this teacher,"
						" please use constraint teacher min mornings per week."));
				}

				break;
			}
		//125
		case CONSTRAINT_TEACHER_MAX_SPAN_PER_REAL_DAY:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//126
		case CONSTRAINT_TEACHERS_MAX_SPAN_PER_REAL_DAY:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//133
		case CONSTRAINT_STUDENTS_MIN_HOURS_PER_MORNING:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//134
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_MORNING:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//176
		case CONSTRAINT_TEACHERS_MAX_THREE_CONSECUTIVE_DAYS:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//177
		case CONSTRAINT_TEACHER_MAX_THREE_CONSECUTIVE_DAYS:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//182
		case CONSTRAINT_STUDENTS_MAX_THREE_CONSECUTIVE_DAYS:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//183
		case CONSTRAINT_STUDENTS_SET_MAX_THREE_CONSECUTIVE_DAYS:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//184
		case CONSTRAINT_MIN_HALF_DAYS_BETWEEN_ACTIVITIES:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//186
		case CONSTRAINT_ACTIVITIES_MIN_IN_A_TERM:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//199
		case CONSTRAINT_TEACHERS_MIN_HOURS_PER_AFTERNOON:
			{
				bool k=checkBox->isChecked();

				if(!k){
					checkBox->setChecked(true);
					QMessageBox::information(dialog, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty afternoons for the teachers,"
						" please use constraint teachers min afternoons per week."));
				}

				break;
			}
		//200
		case CONSTRAINT_TEACHER_MIN_HOURS_PER_AFTERNOON:
			{
				bool k=checkBox->isChecked();

				if(!k){
					checkBox->setChecked(true);
					QMessageBox::information(dialog, tr("FET information"), tr("This check box must remain checked. If you really need to not allow empty afternoons for this teacher,"
						" please use constraint teacher min afternoons per week."));
				}

				break;
			}
		//201
		case CONSTRAINT_STUDENTS_MIN_HOURS_PER_AFTERNOON:
			{
				//nothing, we just avoid the assert below

				break;
			}
		//202
		case CONSTRAINT_STUDENTS_SET_MIN_HOURS_PER_AFTERNOON:
			{
				//nothing, we just avoid the assert below

				break;
			}

		default:
			assert(0);
			break;
	}
}

bool AddOrModifyTimeConstraint::filterOk(Activity* act)
{
	assert(act!=nullptr);

	if(teachersComboBox->currentText()!=QString("") && !act->teachersNames.contains(teachersComboBox->currentText()))
		return false;
	if(subjectsComboBox->currentText()!=QString("") && subjectsComboBox->currentText()!=act->subjectName)
		return false;
	if(activityTagsComboBox->currentText()!=QString("") && !act->activityTagsNames.contains(activityTagsComboBox->currentText()))
		return false;
	//if(studentsComboBox->currentText()!=QString("") && !act->studentsNames.contains(studentsComboBox->currentText()))
	//	return false;
	if(studentsComboBox->currentText()!=QString(""))
		if(!showedStudents.intersects(QSet<QString>(act->studentsNames.constBegin(), act->studentsNames.constEnd())))
			return false;
	
	return true;
}

bool AddOrModifyTimeConstraint::first_filterOk(Activity* act)
{
	assert(act!=nullptr);

	if(first_filterTeachersComboBox->currentText()!=QString("") && !act->teachersNames.contains(first_filterTeachersComboBox->currentText()))
		return false;
	if(first_filterSubjectsComboBox->currentText()!=QString("") && first_filterSubjectsComboBox->currentText()!=act->subjectName)
		return false;
	if(first_filterActivityTagsComboBox->currentText()!=QString("") && !act->activityTagsNames.contains(first_filterActivityTagsComboBox->currentText()))
		return false;
	//if(first_filterStudentsComboBox->currentText()!=QString("") && !act->studentsNames.contains(first_filterStudentsComboBox->currentText()))
	//	return false;
	if(first_filterStudentsComboBox->currentText()!=QString("")){
		if(!firstFilter_showedStudents.intersects(QSet<QString>(act->studentsNames.constBegin(), act->studentsNames.constEnd())))
			return false;
	}

	return true;
}

bool AddOrModifyTimeConstraint::second_filterOk(Activity* act)
{
	assert(act!=nullptr);

	if(second_filterTeachersComboBox->currentText()!=QString("") && !act->teachersNames.contains(second_filterTeachersComboBox->currentText()))
		return false;
	if(second_filterSubjectsComboBox->currentText()!=QString("") && second_filterSubjectsComboBox->currentText()!=act->subjectName)
		return false;
	if(second_filterActivityTagsComboBox->currentText()!=QString("") && !act->activityTagsNames.contains(second_filterActivityTagsComboBox->currentText()))
		return false;
	//if(second_filterStudentsComboBox->currentText()!=QString("") && !act->studentsNames.contains(second_filterStudentsComboBox->currentText()))
	//	return false;
	if(second_filterStudentsComboBox->currentText()!=QString(""))
		if(!secondFilter_showedStudents.intersects(QSet<QString>(act->studentsNames.constBegin(), act->studentsNames.constEnd())))
			return false;

	return true;
}

bool AddOrModifyTimeConstraint::third_filterOk(Activity* act)
{
	assert(act!=nullptr);

	if(third_filterTeachersComboBox->currentText()!=QString("") && !act->teachersNames.contains(third_filterTeachersComboBox->currentText()))
		return false;
	if(third_filterSubjectsComboBox->currentText()!=QString("") && third_filterSubjectsComboBox->currentText()!=act->subjectName)
		return false;
	if(third_filterActivityTagsComboBox->currentText()!=QString("") && !act->activityTagsNames.contains(third_filterActivityTagsComboBox->currentText()))
		return false;
	//if(third_filterStudentsComboBox->currentText()!=QString("") && !act->studentsNames.contains(third_filterStudentsComboBox->currentText()))
	//	return false;
	if(third_filterStudentsComboBox->currentText()!=QString(""))
		if(!thirdFilter_showedStudents.intersects(QSet<QString>(act->studentsNames.constBegin(), act->studentsNames.constEnd())))
			return false;

	return true;
}

int AddOrModifyTimeConstraint::filterActivitiesComboBox()
{
	assert(activitiesComboBox!=nullptr);
	assert(activitiesListWidget==nullptr);

	activitiesComboBox->clear();
	activitiesList.clear();

	int i=-1;
	for(Activity* act : std::as_const(gt.rules.activitiesList)){
		if(filterOk(act)){
			activitiesComboBox->addItem(act->getDescription(gt.rules));
			if(!act->active){
				//activitiesComboBox->item(activitiesComboBox->count()-1)->setBackground(activitiesComboBox->palette().brush(QPalette::Disabled, QPalette::Window));
				//activitiesComboBox->item(activitiesComboBox->count()-1)->setForeground(activitiesComboBox->palette().brush(QPalette::Disabled, QPalette::WindowText));
				activitiesComboBox->setItemData(activitiesComboBox->count()-1, activitiesComboBox->palette().brush(QPalette::Disabled, QPalette::Window), Qt::BackgroundRole);
				activitiesComboBox->setItemData(activitiesComboBox->count()-1, activitiesComboBox->palette().brush(QPalette::Disabled, QPalette::WindowText), Qt::ForegroundRole);
			}
			activitiesList.append(act->id);
			if(initialActivityId>=0 && initialActivityId==act->id)
				i=activitiesList.count()-1;
		}
	}

	activitiesComboBox->setCurrentIndex(0);

	return i;
}

void AddOrModifyTimeConstraint::filterActivitiesListWidgets1And2()
{
	assert(activitiesListWidget==nullptr);
	assert(activitiesComboBox==nullptr);

	assert(activitiesListWidget_TwoSetsOfActivities_1!=nullptr);
	assert(activitiesListWidget_TwoSetsOfActivities_2!=nullptr);

	activitiesListWidget_TwoSetsOfActivities_1->clear();
	activitiesListWidget_TwoSetsOfActivities_2->clear();
	activitiesList.clear();

	for(Activity* act : std::as_const(gt.rules.activitiesList)){
		if(filterOk(act)){
			activitiesListWidget_TwoSetsOfActivities_1->addItem(act->getDescription(gt.rules));
			if(!act->active){
				activitiesListWidget_TwoSetsOfActivities_1->item(activitiesListWidget_TwoSetsOfActivities_1->count()-1)->setBackground(activitiesListWidget_TwoSetsOfActivities_1->palette().brush(QPalette::Disabled, QPalette::Window));
				activitiesListWidget_TwoSetsOfActivities_1->item(activitiesListWidget_TwoSetsOfActivities_1->count()-1)->setForeground(activitiesListWidget_TwoSetsOfActivities_1->palette().brush(QPalette::Disabled, QPalette::WindowText));
			}

			activitiesListWidget_TwoSetsOfActivities_2->addItem(act->getDescription(gt.rules));
			if(!act->active){
				activitiesListWidget_TwoSetsOfActivities_2->item(activitiesListWidget_TwoSetsOfActivities_2->count()-1)->setBackground(activitiesListWidget_TwoSetsOfActivities_2->palette().brush(QPalette::Disabled, QPalette::Window));
				activitiesListWidget_TwoSetsOfActivities_2->item(activitiesListWidget_TwoSetsOfActivities_2->count()-1)->setForeground(activitiesListWidget_TwoSetsOfActivities_2->palette().brush(QPalette::Disabled, QPalette::WindowText));
			}

			activitiesList.append(act->id);
		}
	}

	int q=activitiesListWidget_TwoSetsOfActivities_1->verticalScrollBar()->minimum();
	activitiesListWidget_TwoSetsOfActivities_1->verticalScrollBar()->setValue(q);

	q=activitiesListWidget_TwoSetsOfActivities_2->verticalScrollBar()->minimum();
	activitiesListWidget_TwoSetsOfActivities_2->verticalScrollBar()->setValue(q);
}

void AddOrModifyTimeConstraint::filterActivitiesListWidget()
{
	assert(activitiesListWidget!=nullptr);
	assert(activitiesComboBox==nullptr);

	activitiesListWidget->clear();
	activitiesList.clear();

	if(checkBox!=nullptr){
		if(checkBox->isChecked() && type==CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME){
			assert(oldtc==nullptr);
			//show only non-split activities and split activities which are the representatives
			for(Activity* act : std::as_const(gt.rules.activitiesList)){
				if(filterOk(act)){
					if(act->activityGroupId==0 || act->id==act->activityGroupId){
						activitiesListWidget->addItem(act->getDescription(gt.rules));
						if(!act->active){
							activitiesListWidget->item(activitiesListWidget->count()-1)->setBackground(activitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
							activitiesListWidget->item(activitiesListWidget->count()-1)->setForeground(activitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
						}
						activitiesList.append(act->id);
					}
				}
			}
		}
		else{
			for(Activity* act : std::as_const(gt.rules.activitiesList)){
				if(filterOk(act)){
					activitiesListWidget->addItem(act->getDescription(gt.rules));
					if(!act->active){
						activitiesListWidget->item(activitiesListWidget->count()-1)->setBackground(activitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
						activitiesListWidget->item(activitiesListWidget->count()-1)->setForeground(activitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
					}
					activitiesList.append(act->id);
				}
			}
		}
	}
	else{
		for(Activity* act : std::as_const(gt.rules.activitiesList)){
			if(filterOk(act)){
				activitiesListWidget->addItem(act->getDescription(gt.rules));
				if(!act->active){
					activitiesListWidget->item(activitiesListWidget->count()-1)->setBackground(activitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
					activitiesListWidget->item(activitiesListWidget->count()-1)->setForeground(activitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
				}
				activitiesList.append(act->id);
			}
		}
	}

	int q=activitiesListWidget->verticalScrollBar()->minimum();
	activitiesListWidget->verticalScrollBar()->setValue(q);
}

int AddOrModifyTimeConstraint::first_activitiesComboBoxFilter()
{
	assert(first_activitiesComboBox!=nullptr);

	assert(activitiesListWidget==nullptr);

	first_activitiesComboBox->clear();
	first_activitiesList.clear();

	int i=-1;
	for(Activity* act : std::as_const(gt.rules.activitiesList)){
		if(first_filterOk(act)){
			first_activitiesComboBox->addItem(act->getDescription(gt.rules));
			if(!act->active){
				//first_activitiesComboBox->item(first_activitiesComboBox->count()-1)->setBackground(first_activitiesComboBox->palette().brush(QPalette::Disabled, QPalette::Window));
				//first_activitiesComboBox->item(first_activitiesComboBox->count()-1)->setForeground(first_activitiesComboBox->palette().brush(QPalette::Disabled, QPalette::WindowText));
				first_activitiesComboBox->setItemData(first_activitiesComboBox->count()-1, first_activitiesComboBox->palette().brush(QPalette::Disabled, QPalette::Window), Qt::BackgroundRole);
				first_activitiesComboBox->setItemData(first_activitiesComboBox->count()-1, first_activitiesComboBox->palette().brush(QPalette::Disabled, QPalette::WindowText), Qt::ForegroundRole);
			}
			first_activitiesList.append(act->id);
			if(first_initialActivityId>=0 && first_initialActivityId==act->id)
				i=first_activitiesList.count()-1;
		}
	}

	first_activitiesComboBox->setCurrentIndex(0);

	return i;
}

int AddOrModifyTimeConstraint::second_activitiesComboBoxFilter()
{
	assert(second_activitiesComboBox!=nullptr);

	assert(activitiesListWidget==nullptr);

	second_activitiesComboBox->clear();
	second_activitiesList.clear();

	int i=-1;
	for(Activity* act : std::as_const(gt.rules.activitiesList)){
		if(second_filterOk(act)){
			second_activitiesComboBox->addItem(act->getDescription(gt.rules));
			if(!act->active){
				//second_activitiesComboBox->item(second_activitiesComboBox->count()-1)->setBackground(second_activitiesComboBox->palette().brush(QPalette::Disabled, QPalette::Window));
				//second_activitiesComboBox->item(second_activitiesComboBox->count()-1)->setForeground(second_activitiesComboBox->palette().brush(QPalette::Disabled, QPalette::WindowText));
				second_activitiesComboBox->setItemData(second_activitiesComboBox->count()-1, second_activitiesComboBox->palette().brush(QPalette::Disabled, QPalette::Window), Qt::BackgroundRole);
				second_activitiesComboBox->setItemData(second_activitiesComboBox->count()-1, second_activitiesComboBox->palette().brush(QPalette::Disabled, QPalette::WindowText), Qt::ForegroundRole);
			}
			second_activitiesList.append(act->id);
			if(second_initialActivityId>=0 && second_initialActivityId==act->id)
				i=second_activitiesList.count()-1;
		}
	}

	second_activitiesComboBox->setCurrentIndex(0);

	return i;
}

int AddOrModifyTimeConstraint::third_activitiesComboBoxFilter()
{
	assert(third_activitiesComboBox!=nullptr);

	assert(activitiesListWidget==nullptr);

	third_activitiesComboBox->clear();
	third_activitiesList.clear();

	int i=-1;
	for(Activity* act : std::as_const(gt.rules.activitiesList)){
		if(third_filterOk(act)){
			third_activitiesComboBox->addItem(act->getDescription(gt.rules));
			if(!act->active){
				//third_activitiesComboBox->item(third_activitiesComboBox->count()-1)->setBackground(third_activitiesComboBox->palette().brush(QPalette::Disabled, QPalette::Window));
				//third_activitiesComboBox->item(third_activitiesComboBox->count()-1)->setForeground(third_activitiesComboBox->palette().brush(QPalette::Disabled, QPalette::WindowText));
				third_activitiesComboBox->setItemData(third_activitiesComboBox->count()-1, third_activitiesComboBox->palette().brush(QPalette::Disabled, QPalette::Window), Qt::BackgroundRole);
				third_activitiesComboBox->setItemData(third_activitiesComboBox->count()-1, third_activitiesComboBox->palette().brush(QPalette::Disabled, QPalette::WindowText), Qt::ForegroundRole);
			}
			third_activitiesList.append(act->id);
			if(third_initialActivityId>=0 && third_initialActivityId==act->id)
				i=third_activitiesList.count()-1;
		}
	}

	third_activitiesComboBox->setCurrentIndex(0);

	return i;
}

void AddOrModifyTimeConstraint::addAllActivitiesClicked()
{
	QSet<int> ts(selectedActivitiesList.constBegin(), selectedActivitiesList.constEnd());

	for(int tmp=0; tmp<activitiesListWidget->count(); tmp++){
		int _id=activitiesList.at(tmp);

		QString actName=activitiesListWidget->item(tmp)->text();
		assert(actName!="");

		if(ts.contains(_id))
			continue;

		selectedActivitiesListWidget->addItem(actName);
		Activity* act=gt.rules.activitiesPointerHash.value(_id, nullptr);
		if(act!=nullptr){
			if(!act->active){
				selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
				selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
			}
		}
		selectedActivitiesList.append(_id);
		ts.insert(_id);
	}

	selectedActivitiesListWidget->setCurrentRow(selectedActivitiesListWidget->count()-1);
}

void AddOrModifyTimeConstraint::clearActivitiesClicked()
{
	selectedActivitiesListWidget->clear();
	selectedActivitiesList.clear();
}

void AddOrModifyTimeConstraint::addActivity()
{
	if(activitiesListWidget->currentRow()<0)
		return;
	int tmp=activitiesListWidget->currentRow();
	assert(tmp>=0);
	assert(tmp<activitiesList.count());
	int _id=activitiesList.at(tmp);

	QString actName=activitiesListWidget->currentItem()->text();
	assert(actName!="");

	//duplicate?
	if(selectedActivitiesList.contains(_id))
		return;

	selectedActivitiesListWidget->addItem(actName);
	Activity* act=gt.rules.activitiesPointerHash.value(_id, nullptr);
	if(act!=nullptr){
		if(!act->active){
			selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setBackground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::Window));
			selectedActivitiesListWidget->item(selectedActivitiesListWidget->count()-1)->setForeground(selectedActivitiesListWidget->palette().brush(QPalette::Disabled, QPalette::WindowText));
		}
	}
	selectedActivitiesListWidget->setCurrentRow(selectedActivitiesListWidget->count()-1);

	selectedActivitiesList.append(_id);
}

void AddOrModifyTimeConstraint::removeActivity()
{
	if(selectedActivitiesListWidget->currentRow()<0 || selectedActivitiesListWidget->count()<=0)
		return;
	int tmp=selectedActivitiesListWidget->currentRow();

	selectedActivitiesList.removeAt(tmp);

	selectedActivitiesListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedActivitiesListWidget->takeItem(tmp);
	delete item;
	if(tmp<selectedActivitiesListWidget->count())
		selectedActivitiesListWidget->setCurrentRow(tmp);
	else
		selectedActivitiesListWidget->setCurrentRow(selectedActivitiesListWidget->count()-1);
}

void AddOrModifyTimeConstraint::durationCheckBoxToggled()
{
	assert(type==CONSTRAINT_ACTIVITIES_PREFERRED_TIME_SLOTS
		   || type==CONSTRAINT_ACTIVITIES_PREFERRED_STARTING_TIMES
		   || type==CONSTRAINT_SUBACTIVITIES_PREFERRED_TIME_SLOTS
		   || type==CONSTRAINT_SUBACTIVITIES_PREFERRED_STARTING_TIMES);

	durationSpinBox->setEnabled(durationCheckBox->isChecked());
}

void AddOrModifyTimeConstraint::swapActivitiesPushButtonClicked()
{
	assert(first_filterTeachersComboBox!=nullptr);
	disconnect(first_filterTeachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::first_activitiesComboBoxFilter);

	assert(second_filterTeachersComboBox!=nullptr);
	disconnect(second_filterTeachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::second_activitiesComboBoxFilter);

	assert(first_filterStudentsComboBox!=nullptr);
	disconnect(first_filterStudentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::firstFilter_showRelatedCheckBoxToggled);

	assert(second_filterStudentsComboBox!=nullptr);
	disconnect(second_filterStudentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::secondFilter_showRelatedCheckBoxToggled);

	assert(first_filterSubjectsComboBox!=nullptr);
	disconnect(first_filterSubjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::first_activitiesComboBoxFilter);

	assert(second_filterSubjectsComboBox!=nullptr);
	disconnect(second_filterSubjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::second_activitiesComboBoxFilter);

	assert(first_filterActivityTagsComboBox!=nullptr);
	disconnect(first_filterActivityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::first_activitiesComboBoxFilter);

	assert(second_filterActivityTagsComboBox!=nullptr);
	disconnect(second_filterActivityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::second_activitiesComboBoxFilter);

	int v1=first_activitiesComboBox->currentIndex();
	int v2=second_activitiesComboBox->currentIndex();

	int t1=first_filterTeachersComboBox->currentIndex();
	int t2=second_filterTeachersComboBox->currentIndex();
	first_filterTeachersComboBox->setCurrentIndex(t2);
	second_filterTeachersComboBox->setCurrentIndex(t1);

	t1=first_filterStudentsComboBox->currentIndex();
	t2=second_filterStudentsComboBox->currentIndex();
	first_filterStudentsComboBox->setCurrentIndex(t2);
	second_filterStudentsComboBox->setCurrentIndex(t1);

	t1=first_filterSubjectsComboBox->currentIndex();
	t2=second_filterSubjectsComboBox->currentIndex();
	first_filterSubjectsComboBox->setCurrentIndex(t2);
	second_filterSubjectsComboBox->setCurrentIndex(t1);

	t1=first_filterActivityTagsComboBox->currentIndex();
	t2=second_filterActivityTagsComboBox->currentIndex();
	first_filterActivityTagsComboBox->setCurrentIndex(t2);
	second_filterActivityTagsComboBox->setCurrentIndex(t1);

	bool b1=firstFilter_showRelatedCheckBox->isChecked();
	bool b2=secondFilter_showRelatedCheckBox->isChecked();
	firstFilter_showRelatedCheckBox->setChecked(b2);
	secondFilter_showRelatedCheckBox->setChecked(b1);

	/*first_activitiesComboBoxFilter();
	second_activitiesComboBoxFilter();*/
	firstFilter_showRelatedCheckBoxToggled();
	secondFilter_showRelatedCheckBoxToggled();

	first_activitiesComboBox->setCurrentIndex(v2);
	second_activitiesComboBox->setCurrentIndex(v1);

	connect(first_filterTeachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::first_activitiesComboBoxFilter);
	connect(second_filterTeachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::second_activitiesComboBoxFilter);

	connect(first_filterStudentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::firstFilter_showRelatedCheckBoxToggled);
	connect(second_filterStudentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::secondFilter_showRelatedCheckBoxToggled);

	connect(first_filterSubjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::first_activitiesComboBoxFilter);
	connect(second_filterSubjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::second_activitiesComboBoxFilter);

	connect(first_filterActivityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::first_activitiesComboBoxFilter);
	connect(second_filterActivityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifyTimeConstraint::second_activitiesComboBoxFilter);
}

void AddOrModifyTimeConstraint::swapActivityTagsPushButtonClicked()
{
	int t1=first_activityTagsComboBox->currentIndex();
	int t2=second_activityTagsComboBox->currentIndex();
	first_activityTagsComboBox->setCurrentIndex(t2);
	second_activityTagsComboBox->setCurrentIndex(t1);
}

void AddOrModifyTimeConstraint::addAllActivityTagsClicked()
{
	for(int tmp=0; tmp<activityTagsListWidget->count(); tmp++){
		QString at=activityTagsListWidget->item(tmp)->text();

		if(selectedActivityTagsSet.contains(at))
			continue;

		selectedActivityTagsListWidget->addItem(at);
		selectedActivityTagsSet.insert(at);
	}

	selectedActivityTagsListWidget->setCurrentRow(selectedActivityTagsListWidget->count()-1);
}

void AddOrModifyTimeConstraint::clearActivityTagsClicked()
{
	selectedActivityTagsListWidget->clear();
	selectedActivityTagsSet.clear();
}

void AddOrModifyTimeConstraint::addActivityTag()
{
	if(activityTagsListWidget->currentRow()<0)
		return;
	QString at=activityTagsListWidget->currentItem()->text();

	//duplicate?
	if(selectedActivityTagsSet.contains(at))
		return;

	selectedActivityTagsListWidget->addItem(at);
	selectedActivityTagsListWidget->setCurrentRow(selectedActivityTagsListWidget->count()-1);

	selectedActivityTagsSet.insert(at);
}

void AddOrModifyTimeConstraint::removeActivityTag()
{
	if(selectedActivityTagsListWidget->currentRow()<0 || selectedActivityTagsListWidget->count()<=0)
		return;
	QString at=selectedActivityTagsListWidget->currentItem()->text();

	assert(selectedActivityTagsSet.contains(at));
	selectedActivityTagsSet.remove(at);

	int tmp=selectedActivityTagsListWidget->currentRow();
	selectedActivityTagsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=selectedActivityTagsListWidget->takeItem(tmp);
	delete item;
	if(tmp<selectedActivityTagsListWidget->count())
		selectedActivityTagsListWidget->setCurrentRow(tmp);
	else
		selectedActivityTagsListWidget->setCurrentRow(selectedActivityTagsListWidget->count()-1);
}

//

void AddOrModifyTimeConstraint::addAllActivitiesClicked1()
{
	QSet<int> ts(selectedActivitiesList_TwoSetsOfActivities_1.constBegin(), selectedActivitiesList_TwoSetsOfActivities_1.constEnd());

	for(int tmp=0; tmp<activitiesListWidget_TwoSetsOfActivities_1->count(); tmp++){
		int _id=activitiesList.at(tmp);

		QString actName=activitiesListWidget_TwoSetsOfActivities_1->item(tmp)->text();
		assert(actName!="");

		if(ts.contains(_id))
			continue;

		selectedActivitiesListWidget_TwoSetsOfActivities_1->addItem(actName);
		Activity* act=gt.rules.activitiesPointerHash.value(_id, nullptr);
		if(act!=nullptr){
			if(!act->active){
				selectedActivitiesListWidget_TwoSetsOfActivities_1->item(selectedActivitiesListWidget_TwoSetsOfActivities_1->count()-1)->setBackground(selectedActivitiesListWidget_TwoSetsOfActivities_1->palette().brush(QPalette::Disabled, QPalette::Window));
				selectedActivitiesListWidget_TwoSetsOfActivities_1->item(selectedActivitiesListWidget_TwoSetsOfActivities_1->count()-1)->setForeground(selectedActivitiesListWidget_TwoSetsOfActivities_1->palette().brush(QPalette::Disabled, QPalette::WindowText));
			}
		}
		selectedActivitiesList_TwoSetsOfActivities_1.append(_id);
		ts.insert(_id);
	}

	selectedActivitiesListWidget_TwoSetsOfActivities_1->setCurrentRow(selectedActivitiesListWidget_TwoSetsOfActivities_1->count()-1);
}

void AddOrModifyTimeConstraint::clearActivitiesClicked1()
{
	selectedActivitiesListWidget_TwoSetsOfActivities_1->clear();
	selectedActivitiesList_TwoSetsOfActivities_1.clear();
}

void AddOrModifyTimeConstraint::addActivity1()
{
	if(activitiesListWidget_TwoSetsOfActivities_1->currentRow()<0)
		return;
	int tmp=activitiesListWidget_TwoSetsOfActivities_1->currentRow();
	assert(tmp>=0);
	assert(tmp<activitiesList.count());
	int _id=activitiesList.at(tmp);

	QString actName=activitiesListWidget_TwoSetsOfActivities_1->currentItem()->text();
	assert(actName!="");

	//duplicate?
	if(selectedActivitiesList_TwoSetsOfActivities_1.contains(_id))
		return;

	selectedActivitiesListWidget_TwoSetsOfActivities_1->addItem(actName);
	Activity* act=gt.rules.activitiesPointerHash.value(_id, nullptr);
	if(act!=nullptr){
		if(!act->active){
			selectedActivitiesListWidget_TwoSetsOfActivities_1->item(selectedActivitiesListWidget_TwoSetsOfActivities_1->count()-1)->setBackground(selectedActivitiesListWidget_TwoSetsOfActivities_1->palette().brush(QPalette::Disabled, QPalette::Window));
			selectedActivitiesListWidget_TwoSetsOfActivities_1->item(selectedActivitiesListWidget_TwoSetsOfActivities_1->count()-1)->setForeground(selectedActivitiesListWidget_TwoSetsOfActivities_1->palette().brush(QPalette::Disabled, QPalette::WindowText));
		}
	}
	selectedActivitiesListWidget_TwoSetsOfActivities_1->setCurrentRow(selectedActivitiesListWidget_TwoSetsOfActivities_1->count()-1);

	selectedActivitiesList_TwoSetsOfActivities_1.append(_id);
}

void AddOrModifyTimeConstraint::removeActivity1()
{
	if(selectedActivitiesListWidget_TwoSetsOfActivities_1->currentRow()<0 || selectedActivitiesListWidget_TwoSetsOfActivities_1->count()<=0)
		return;
	int tmp=selectedActivitiesListWidget_TwoSetsOfActivities_1->currentRow();

	selectedActivitiesList_TwoSetsOfActivities_1.removeAt(tmp);

	selectedActivitiesListWidget_TwoSetsOfActivities_1->setCurrentRow(-1);
	QListWidgetItem* item=selectedActivitiesListWidget_TwoSetsOfActivities_1->takeItem(tmp);
	delete item;
	if(tmp<selectedActivitiesListWidget_TwoSetsOfActivities_1->count())
		selectedActivitiesListWidget_TwoSetsOfActivities_1->setCurrentRow(tmp);
	else
		selectedActivitiesListWidget_TwoSetsOfActivities_1->setCurrentRow(selectedActivitiesListWidget_TwoSetsOfActivities_1->count()-1);
}

//

void AddOrModifyTimeConstraint::addAllActivitiesClicked2()
{
	QSet<int> ts(selectedActivitiesList_TwoSetsOfActivities_2.constBegin(), selectedActivitiesList_TwoSetsOfActivities_2.constEnd());

	for(int tmp=0; tmp<activitiesListWidget_TwoSetsOfActivities_2->count(); tmp++){
		int _id=activitiesList.at(tmp);

		QString actName=activitiesListWidget_TwoSetsOfActivities_2->item(tmp)->text();
		assert(actName!="");

		if(ts.contains(_id))
			continue;

		selectedActivitiesListWidget_TwoSetsOfActivities_2->addItem(actName);
		Activity* act=gt.rules.activitiesPointerHash.value(_id, nullptr);
		if(act!=nullptr){
			if(!act->active){
				selectedActivitiesListWidget_TwoSetsOfActivities_2->item(selectedActivitiesListWidget_TwoSetsOfActivities_2->count()-1)->setBackground(selectedActivitiesListWidget_TwoSetsOfActivities_2->palette().brush(QPalette::Disabled, QPalette::Window));
				selectedActivitiesListWidget_TwoSetsOfActivities_2->item(selectedActivitiesListWidget_TwoSetsOfActivities_2->count()-1)->setForeground(selectedActivitiesListWidget_TwoSetsOfActivities_2->palette().brush(QPalette::Disabled, QPalette::WindowText));
			}
		}
		selectedActivitiesList_TwoSetsOfActivities_2.append(_id);
		ts.insert(_id);
	}

	selectedActivitiesListWidget_TwoSetsOfActivities_2->setCurrentRow(selectedActivitiesListWidget_TwoSetsOfActivities_2->count()-1);
}

void AddOrModifyTimeConstraint::clearActivitiesClicked2()
{
	selectedActivitiesListWidget_TwoSetsOfActivities_2->clear();
	selectedActivitiesList_TwoSetsOfActivities_2.clear();
}

void AddOrModifyTimeConstraint::addActivity2()
{
	if(activitiesListWidget_TwoSetsOfActivities_2->currentRow()<0)
		return;
	int tmp=activitiesListWidget_TwoSetsOfActivities_2->currentRow();
	assert(tmp>=0);
	assert(tmp<activitiesList.count());
	int _id=activitiesList.at(tmp);

	QString actName=activitiesListWidget_TwoSetsOfActivities_2->currentItem()->text();
	assert(actName!="");

	//duplicate?
	if(selectedActivitiesList_TwoSetsOfActivities_2.contains(_id))
		return;

	selectedActivitiesListWidget_TwoSetsOfActivities_2->addItem(actName);
	Activity* act=gt.rules.activitiesPointerHash.value(_id, nullptr);
	if(act!=nullptr){
		if(!act->active){
			selectedActivitiesListWidget_TwoSetsOfActivities_2->item(selectedActivitiesListWidget_TwoSetsOfActivities_2->count()-1)->setBackground(selectedActivitiesListWidget_TwoSetsOfActivities_2->palette().brush(QPalette::Disabled, QPalette::Window));
			selectedActivitiesListWidget_TwoSetsOfActivities_2->item(selectedActivitiesListWidget_TwoSetsOfActivities_2->count()-1)->setForeground(selectedActivitiesListWidget_TwoSetsOfActivities_2->palette().brush(QPalette::Disabled, QPalette::WindowText));
		}
	}
	selectedActivitiesListWidget_TwoSetsOfActivities_2->setCurrentRow(selectedActivitiesListWidget_TwoSetsOfActivities_2->count()-1);

	selectedActivitiesList_TwoSetsOfActivities_2.append(_id);
}

void AddOrModifyTimeConstraint::removeActivity2()
{
	if(selectedActivitiesListWidget_TwoSetsOfActivities_2->currentRow()<0 || selectedActivitiesListWidget_TwoSetsOfActivities_2->count()<=0)
		return;
	int tmp=selectedActivitiesListWidget_TwoSetsOfActivities_2->currentRow();

	selectedActivitiesList_TwoSetsOfActivities_2.removeAt(tmp);

	selectedActivitiesListWidget_TwoSetsOfActivities_2->setCurrentRow(-1);
	QListWidgetItem* item=selectedActivitiesListWidget_TwoSetsOfActivities_2->takeItem(tmp);
	delete item;
	if(tmp<selectedActivitiesListWidget_TwoSetsOfActivities_2->count())
		selectedActivitiesListWidget_TwoSetsOfActivities_2->setCurrentRow(tmp);
	else
		selectedActivitiesListWidget_TwoSetsOfActivities_2->setCurrentRow(selectedActivitiesListWidget_TwoSetsOfActivities_2->count()-1);
}

void AddOrModifyTimeConstraint::swapTwoSetsOfActivitiesPushButtonClicked()
{
	selectedActivitiesList_TwoSetsOfActivities_1.swap(selectedActivitiesList_TwoSetsOfActivities_2);

	selectedActivitiesListWidget_TwoSetsOfActivities_1->clear();
	selectedActivitiesListWidget_TwoSetsOfActivities_2->clear();

	for(int actId : std::as_const(selectedActivitiesList_TwoSetsOfActivities_1)){
		Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
		assert(act!=nullptr);
		selectedActivitiesListWidget_TwoSetsOfActivities_1->addItem(act->getDescription(gt.rules));
		if(!act->active){
			selectedActivitiesListWidget_TwoSetsOfActivities_1->item(selectedActivitiesListWidget_TwoSetsOfActivities_1->count()-1)->setBackground(selectedActivitiesListWidget_TwoSetsOfActivities_1->palette().brush(QPalette::Disabled, QPalette::Window));
			selectedActivitiesListWidget_TwoSetsOfActivities_1->item(selectedActivitiesListWidget_TwoSetsOfActivities_1->count()-1)->setForeground(selectedActivitiesListWidget_TwoSetsOfActivities_1->palette().brush(QPalette::Disabled, QPalette::WindowText));
		}
	}

	for(int actId : std::as_const(selectedActivitiesList_TwoSetsOfActivities_2)){
		Activity* act=gt.rules.activitiesPointerHash.value(actId, nullptr);
		assert(act!=nullptr);
		selectedActivitiesListWidget_TwoSetsOfActivities_2->addItem(act->getDescription(gt.rules));
		if(!act->active){
			selectedActivitiesListWidget_TwoSetsOfActivities_2->item(selectedActivitiesListWidget_TwoSetsOfActivities_2->count()-1)->setBackground(selectedActivitiesListWidget_TwoSetsOfActivities_2->palette().brush(QPalette::Disabled, QPalette::Window));
			selectedActivitiesListWidget_TwoSetsOfActivities_2->item(selectedActivitiesListWidget_TwoSetsOfActivities_2->count()-1)->setForeground(selectedActivitiesListWidget_TwoSetsOfActivities_2->palette().brush(QPalette::Disabled, QPalette::WindowText));
		}
	}
}

void AddOrModifyTimeConstraint::showRelatedCheckBoxToggled()
{
	assert(studentsComboBox!=nullptr);
	
	bool showRelated=showRelatedCheckBox->isChecked();
	
	showedStudents.clear();
	
	if(!showRelated){
		showedStudents.insert(studentsComboBox->currentText());
	}
	else{
		if(studentsComboBox->currentText()=="")
			showedStudents.insert("");
		else{
			//down
			StudentsSet* studentsSet=gt.rules.searchStudentsSet(studentsComboBox->currentText());
			assert(studentsSet!=nullptr);
			if(studentsSet->type==STUDENTS_YEAR){
				StudentsYear* year=(StudentsYear*)studentsSet;
				showedStudents.insert(year->name);
				for(StudentsGroup* group : std::as_const(year->groupsList)){
					showedStudents.insert(group->name);
					for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
						showedStudents.insert(subgroup->name);
				}
			}
			else if(studentsSet->type==STUDENTS_GROUP){
				StudentsGroup* group=(StudentsGroup*)studentsSet;
				showedStudents.insert(group->name);
				for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
					showedStudents.insert(subgroup->name);
			}
			else if(studentsSet->type==STUDENTS_SUBGROUP){
				StudentsSubgroup* subgroup=(StudentsSubgroup*)studentsSet;
				showedStudents.insert(subgroup->name);
			}
			else
				assert(0);
				
			//up
			QString crt=studentsComboBox->currentText();
			for(StudentsYear* year : std::as_const(gt.rules.yearsList)){
				for(StudentsGroup* group : std::as_const(year->groupsList)){
					if(group->name==crt){
						showedStudents.insert(year->name);
					}
					for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList)){
						if(subgroup->name==crt){
							showedStudents.insert(year->name);
							showedStudents.insert(group->name);
						}
					}
				}
			}
		}
	}

	if(filterGroupBox!=nullptr
			&& activitiesListWidget_TwoSetsOfActivities_1!=nullptr
			&& activitiesListWidget_TwoSetsOfActivities_2!=nullptr){

		assert(activitiesComboBox==nullptr);
		assert(activitiesListWidget==nullptr);

		filterActivitiesListWidgets1And2();
	}

	if(filterGroupBox!=nullptr && activitiesListWidget!=nullptr){
		assert(activitiesComboBox==nullptr);
		assert(activitiesListWidget_TwoSetsOfActivities_1==nullptr);
		assert(activitiesListWidget_TwoSetsOfActivities_2==nullptr);

		filterActivitiesListWidget();
	}

	if(filterGroupBox!=nullptr && activitiesComboBox!=nullptr){
		assert(activitiesListWidget==nullptr);
		assert(activitiesListWidget_TwoSetsOfActivities_1==nullptr);
		assert(activitiesListWidget_TwoSetsOfActivities_2==nullptr);

		int i=filterActivitiesComboBox();
		if(i>=0){
			assert(activitiesComboBox!=nullptr);
			assert(i<activitiesList.count());
			assert(i<activitiesComboBox->count());
			activitiesComboBox->setCurrentIndex(i);
		}
	}
}

void AddOrModifyTimeConstraint::firstFilter_showRelatedCheckBoxToggled()
{
	assert(first_filterStudentsComboBox!=nullptr);
	
	bool firstFilter_showRelated=firstFilter_showRelatedCheckBox->isChecked();
	
	firstFilter_showedStudents.clear();
	
	if(!firstFilter_showRelated){
		firstFilter_showedStudents.insert(first_filterStudentsComboBox->currentText());
	}
	else{
		if(first_filterStudentsComboBox->currentText()=="")
			firstFilter_showedStudents.insert("");
		else{
			//down
			StudentsSet* studentsSet=gt.rules.searchStudentsSet(first_filterStudentsComboBox->currentText());
			assert(studentsSet!=nullptr);
			if(studentsSet->type==STUDENTS_YEAR){
				StudentsYear* year=(StudentsYear*)studentsSet;
				firstFilter_showedStudents.insert(year->name);
				for(StudentsGroup* group : std::as_const(year->groupsList)){
					firstFilter_showedStudents.insert(group->name);
					for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
						firstFilter_showedStudents.insert(subgroup->name);
				}
			}
			else if(studentsSet->type==STUDENTS_GROUP){
				StudentsGroup* group=(StudentsGroup*)studentsSet;
				firstFilter_showedStudents.insert(group->name);
				for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
					firstFilter_showedStudents.insert(subgroup->name);
			}
			else if(studentsSet->type==STUDENTS_SUBGROUP){
				StudentsSubgroup* subgroup=(StudentsSubgroup*)studentsSet;
				firstFilter_showedStudents.insert(subgroup->name);
			}
			else
				assert(0);
				
			//up
			QString crt=first_filterStudentsComboBox->currentText();
			for(StudentsYear* year : std::as_const(gt.rules.yearsList)){
				for(StudentsGroup* group : std::as_const(year->groupsList)){
					if(group->name==crt){
						firstFilter_showedStudents.insert(year->name);
					}
					for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList)){
						if(subgroup->name==crt){
							firstFilter_showedStudents.insert(year->name);
							firstFilter_showedStudents.insert(group->name);
						}
					}
				}
			}
		}
	}

	if(first_filterGroupBox!=nullptr && first_activitiesComboBox!=nullptr){
		assert(activitiesListWidget==nullptr);
		assert(activitiesListWidget_TwoSetsOfActivities_1==nullptr);
		assert(activitiesListWidget_TwoSetsOfActivities_2==nullptr);

		int i=first_activitiesComboBoxFilter();
		if(i>=0){
			assert(first_activitiesComboBox!=nullptr);
			assert(i<first_activitiesList.count());
			assert(i<first_activitiesComboBox->count());
			first_activitiesComboBox->setCurrentIndex(i);
		}
	}
}

void AddOrModifyTimeConstraint::secondFilter_showRelatedCheckBoxToggled()
{
	assert(second_filterStudentsComboBox!=nullptr);
	
	bool secondFilter_showRelated=secondFilter_showRelatedCheckBox->isChecked();
	
	secondFilter_showedStudents.clear();
	
	if(!secondFilter_showRelated){
		secondFilter_showedStudents.insert(second_filterStudentsComboBox->currentText());
	}
	else{
		if(second_filterStudentsComboBox->currentText()=="")
			secondFilter_showedStudents.insert("");
		else{
			//down
			StudentsSet* studentsSet=gt.rules.searchStudentsSet(second_filterStudentsComboBox->currentText());
			assert(studentsSet!=nullptr);
			if(studentsSet->type==STUDENTS_YEAR){
				StudentsYear* year=(StudentsYear*)studentsSet;
				secondFilter_showedStudents.insert(year->name);
				for(StudentsGroup* group : std::as_const(year->groupsList)){
					secondFilter_showedStudents.insert(group->name);
					for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
						secondFilter_showedStudents.insert(subgroup->name);
				}
			}
			else if(studentsSet->type==STUDENTS_GROUP){
				StudentsGroup* group=(StudentsGroup*)studentsSet;
				secondFilter_showedStudents.insert(group->name);
				for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
					secondFilter_showedStudents.insert(subgroup->name);
			}
			else if(studentsSet->type==STUDENTS_SUBGROUP){
				StudentsSubgroup* subgroup=(StudentsSubgroup*)studentsSet;
				secondFilter_showedStudents.insert(subgroup->name);
			}
			else
				assert(0);
				
			//up
			QString crt=second_filterStudentsComboBox->currentText();
			for(StudentsYear* year : std::as_const(gt.rules.yearsList)){
				for(StudentsGroup* group : std::as_const(year->groupsList)){
					if(group->name==crt){
						secondFilter_showedStudents.insert(year->name);
					}
					for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList)){
						if(subgroup->name==crt){
							secondFilter_showedStudents.insert(year->name);
							secondFilter_showedStudents.insert(group->name);
						}
					}
				}
			}
		}
	}

	if(second_filterGroupBox!=nullptr && second_activitiesComboBox!=nullptr){
		assert(activitiesListWidget==nullptr);
		assert(activitiesListWidget_TwoSetsOfActivities_1==nullptr);
		assert(activitiesListWidget_TwoSetsOfActivities_2==nullptr);

		int i=second_activitiesComboBoxFilter();
		if(i>=0){
			assert(second_activitiesComboBox!=nullptr);
			assert(i<second_activitiesList.count());
			assert(i<second_activitiesComboBox->count());
			second_activitiesComboBox->setCurrentIndex(i);
		}
	}
}

void AddOrModifyTimeConstraint::thirdFilter_showRelatedCheckBoxToggled()
{
	assert(third_filterStudentsComboBox!=nullptr);
	
	bool thirdFilter_showRelated=thirdFilter_showRelatedCheckBox->isChecked();
	
	thirdFilter_showedStudents.clear();
	
	if(!thirdFilter_showRelated){
		thirdFilter_showedStudents.insert(third_filterStudentsComboBox->currentText());
	}
	else{
		if(third_filterStudentsComboBox->currentText()=="")
			thirdFilter_showedStudents.insert("");
		else{
			//down
			StudentsSet* studentsSet=gt.rules.searchStudentsSet(third_filterStudentsComboBox->currentText());
			assert(studentsSet!=nullptr);
			if(studentsSet->type==STUDENTS_YEAR){
				StudentsYear* year=(StudentsYear*)studentsSet;
				thirdFilter_showedStudents.insert(year->name);
				for(StudentsGroup* group : std::as_const(year->groupsList)){
					thirdFilter_showedStudents.insert(group->name);
					for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
						thirdFilter_showedStudents.insert(subgroup->name);
				}
			}
			else if(studentsSet->type==STUDENTS_GROUP){
				StudentsGroup* group=(StudentsGroup*)studentsSet;
				thirdFilter_showedStudents.insert(group->name);
				for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList))
					thirdFilter_showedStudents.insert(subgroup->name);
			}
			else if(studentsSet->type==STUDENTS_SUBGROUP){
				StudentsSubgroup* subgroup=(StudentsSubgroup*)studentsSet;
				thirdFilter_showedStudents.insert(subgroup->name);
			}
			else
				assert(0);
				
			//up
			QString crt=third_filterStudentsComboBox->currentText();
			for(StudentsYear* year : std::as_const(gt.rules.yearsList)){
				for(StudentsGroup* group : std::as_const(year->groupsList)){
					if(group->name==crt){
						thirdFilter_showedStudents.insert(year->name);
					}
					for(StudentsSubgroup* subgroup : std::as_const(group->subgroupsList)){
						if(subgroup->name==crt){
							thirdFilter_showedStudents.insert(year->name);
							thirdFilter_showedStudents.insert(group->name);
						}
					}
				}
			}
		}
	}

	if(third_filterGroupBox!=nullptr && third_activitiesComboBox!=nullptr){
		assert(activitiesListWidget==nullptr);
		assert(activitiesListWidget_TwoSetsOfActivities_1==nullptr);
		assert(activitiesListWidget_TwoSetsOfActivities_2==nullptr);

		int i=third_activitiesComboBoxFilter();
		if(i>=0){
			assert(third_activitiesComboBox!=nullptr);
			assert(i<third_activitiesList.count());
			assert(i<third_activitiesComboBox->count());
			third_activitiesComboBox->setCurrentIndex(i);
		}
	}
}

void AddOrModifyTimeConstraint::initOccupyMaxTableWidget()
{
	assert(occupyMaxSetsOfTimeSlotsFromSelectionTableWidget!=nullptr);
	
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->setRowCount(gt.rules.nHoursPerDay);
		occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->setColumnCount(gt.rules.nDaysPerWeek);
		
		for(int j=0; j<gt.rules.nDaysPerWeek; j++)
			occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->setHorizontalHeaderItem(j, new QTableWidgetItem(gt.rules.daysOfTheWeek[j]));
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(gt.rules.hoursOfTheDay[i]));
	}
	else{
		occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->setRowCount(gt.rules.nRealHoursPerDay);
		occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->setColumnCount(gt.rules.nRealDaysPerWeek);

		for(int j=0; j<gt.rules.nRealDaysPerWeek; j++)
			occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->setHorizontalHeaderItem(j, new QTableWidgetItem(gt.rules.realDaysOfTheWeek[j]));
		for(int i=0; i<gt.rules.nRealHoursPerDay; i++)
			occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->setVerticalHeaderItem(i, new QTableWidgetItem(gt.rules.realHoursOfTheDay[i]));
	}
	
	spinBoxesTable.resize(occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->rowCount(), occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->columnCount());
	
	for(int i=0; i<occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->rowCount(); i++){
		for(int j=0; j<occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->columnCount(); j++){
			QSpinBox* sb=new QSpinBox;
			sb->setMinimum(0);
			if(gt.rules.mode!=MORNINGS_AFTERNOONS)
				sb->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
			else
				sb->setMaximum(gt.rules.nRealDaysPerWeek*gt.rules.nRealHoursPerDay);
			sb->setValue(0);
			//sb->setSpecialValueText(tr("Not selected", "The corresponding time slot is not selected. Please keep the translation short."));
			sb->setSpecialValueText(QString(" "));
			
			occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->setCellWidget(i, j, sb);
			spinBoxesTable(i, j)=sb;
			
			connect(sb, qOverload<int>(&QSpinBox::valueChanged), this, &AddOrModifyTimeConstraint::colorSpinBox);
		}
	}
}

void AddOrModifyTimeConstraint::fillSpinBoxTimesTable(const QList<QList<int>>& days, const QList<QList<int>>& hours)
{
	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		for(int i=0; i<gt.rules.nHoursPerDay; i++)
			for(int j=0; j<gt.rules.nDaysPerWeek; j++)
				spinBoxesTable(i, j)->setValue(0);
		
		assert(days.count()==hours.count());
		for(int q=0; q<days.count(); q++){
			QList<int> cdays=days.at(q);
			QList<int> chours=hours.at(q);
			assert(cdays.count()==chours.count());
			for(int k=0; k<cdays.count(); k++){
				int d=cdays.at(k);
				int h=chours.at(k);
				assert(d>=0 && d<gt.rules.nDaysPerWeek);
				assert(h>=0 && h<gt.rules.nHoursPerDay);
				spinBoxesTable(h, d)->setValue(q+1);
			}
		}
	}
	else{
		for(int i=0; i<gt.rules.nRealHoursPerDay; i++)
			for(int j=0; j<gt.rules.nRealDaysPerWeek; j++)
				spinBoxesTable(i, j)->setValue(0);
		
		assert(days.count()==hours.count());
		for(int q=0; q<days.count(); q++){
			QList<int> cdays=days.at(q);
			QList<int> chours=hours.at(q);
			assert(cdays.count()==chours.count());
			for(int k=0; k<cdays.count(); k++){
				int d=cdays.at(k);
				int h=chours.at(k);
				assert(d>=0 && d<gt.rules.nDaysPerWeek);
				assert(h>=0 && h<gt.rules.nHoursPerDay);
				int rd=d/2;
				int rh=h+(d%2)*gt.rules.nHoursPerDay;
				if(rd>=0 && rd<gt.rules.nRealDaysPerWeek && rh>=0 && rh<gt.rules.nRealHoursPerDay) //the number of days might be odd (incorrectly - the generation cannot begin).
					spinBoxesTable(rh, rd)->setValue(q+1);
			}
		}
	}
}

void AddOrModifyTimeConstraint::getSpinBoxTimesTable(QList<QList<int>>& days, QList<QList<int>>& hours)
{
	days.clear();
	hours.clear();

	QMap<int, QPair<QList<int>, QList<int>>> selectedMap;

	if(gt.rules.mode!=MORNINGS_AFTERNOONS){
		for(int j=0; j<gt.rules.nDaysPerWeek; j++){
			for(int i=0; i<gt.rules.nHoursPerDay; i++){
				int k=spinBoxesTable(i, j)->value();
				if(k>0){
					QPair<QList<int>, QList<int>> pr=selectedMap.value(k, QPair<QList<int>, QList<int>>());
					pr.first.append(j);
					pr.second.append(i);
					selectedMap.insert(k, pr);
				}
			}
		}
	}
	else{
		for(int j=0; j<gt.rules.nRealDaysPerWeek; j++){
			for(int i=0; i<gt.rules.nRealHoursPerDay; i++){
				int k=spinBoxesTable(i, j)->value();
				if(k>0){
					QPair<QList<int>, QList<int>> pr=selectedMap.value(k, QPair<QList<int>, QList<int>>());
					pr.first.append(2*j+(i/gt.rules.nHoursPerDay));
					pr.second.append(i%gt.rules.nHoursPerDay);
					selectedMap.insert(k, pr);
				}
			}
		}
	}
	
	for(QMap<int, QPair<QList<int>, QList<int>>>::const_iterator it=selectedMap.constBegin(); it!=selectedMap.constEnd(); it++){
		int q=it.key();
		
		if(it==selectedMap.constBegin() && q>1){ //incorrect
			days.append(QList<int>());
			hours.append(QList<int>());
		}
		
		QList<int> dtl=it.value().first;
		QList<int> htl=it.value().second;
		days.append(dtl);
		hours.append(htl);
		
		QMap<int, QPair<QList<int>, QList<int>>>::const_iterator it2=std::next(it);
		if(it2!=selectedMap.constEnd()){
			for(int k=q+1; k<it2.key(); k++){
				days.append(QList<int>());
				hours.append(QList<int>());
			}
		}
	}
}

void AddOrModifyTimeConstraint::colorSpinBoxWithPointer(QSpinBox* sb)
{
	if(!colorsCheckBox->isChecked()){
		sb->setStyleSheet("");
	}
	else{
		switch(sb->value()){
			case 0:
				sb->setStyleSheet("");
				break;
			case 1:
				sb->setStyleSheet("QSpinBox { background-color: darkblue; color: white; }");
				break;
			case 2:
				sb->setStyleSheet("QSpinBox { background-color: darkorange; color: black; }");
				break;
			case 3:
				sb->setStyleSheet("QSpinBox { background-color: darkcyan; color: white; }");
				break;
			case 4:
				sb->setStyleSheet("QSpinBox { background-color: darkmagenta; color: white; }");
				break;
			case 5:
				sb->setStyleSheet("QSpinBox { background-color: darksalmon; color: black; }");
				break;
			case 6:
				sb->setStyleSheet("QSpinBox { background-color: chartreuse; color: black; }");
				break;
			case 7:
				sb->setStyleSheet("QSpinBox { background-color: darkgrey; color: black; }");
				break;
			case 8:
				sb->setStyleSheet("QSpinBox { background-color: gold; color: black; }");
				break;
			case 9:
				sb->setStyleSheet("QSpinBox { background-color: rgb(178, 94, 199); color: black; }");
				break;
			case 10:
				sb->setStyleSheet("QSpinBox { background-color: rgb(147, 95, 53); color: white; }");
				break;
			
			default:
				int r, g, b;
				TimetableExport::stringToColor(QString::number(sb->value()), r, g, b);
				double brightness = double(r)*0.299 + double(g)*0.587 + double(b)*0.114;
				if (brightness<127.5)
					sb->setStyleSheet("QSpinBox { background-color: rgb("+QString::number(r)+", "+QString::number(g)+", "+QString::number(b)+"); color: white; }");
				else
					sb->setStyleSheet("QSpinBox { background-color: rgb("+QString::number(r)+", "+QString::number(g)+", "+QString::number(b)+"); color: black; }");
				
				break;
		}
	}
}

void AddOrModifyTimeConstraint::colorSpinBox()
{
	assert(sender()!=nullptr);
	QSpinBox* sb=(QSpinBox*)sender();
	colorSpinBoxWithPointer(sb);
}

void AddOrModifyTimeConstraint::colorAllSpinBoxes()
{
	for(int i=0; i<occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->rowCount(); i++){
		for(int j=0; j<occupyMaxSetsOfTimeSlotsFromSelectionTableWidget->columnCount(); j++){
			QSpinBox* sb=spinBoxesTable(i, j);
			colorSpinBoxWithPointer(sb);
		}
	}
}
