/***************************************************************************
                          addormodifyspaceconstraint.cpp  -  description
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

#include "addormodifyspaceconstraint.h"

#include "timetable.h"

#include "longtextmessagebox.h"

#include "utilities.h"

#include "lockunlock.h"

#include <QMessageBox>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QHeaderView>

#include <QStringList>

#include <QBrush>
#include <QPalette>

#include <QGuiApplication>
#include <QPainter>

#include <QSettings>

extern Timetable gt;

extern const QString COMPANY;
extern const QString PROGRAM;

void AddOrModifySpaceConstraintTimesTableDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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

AddOrModifySpaceConstraintDialog::AddOrModifySpaceConstraintDialog(QWidget* parent, const QString& _dialogName, const QString& _dialogTitle, QEventLoop* _eventLoop,
																   CornerEnabledTableWidget* _timesTable,
																   QAbstractItemDelegate* _oldItemDelegate,
																   AddOrModifySpaceConstraintTimesTableDelegate* _newItemDelegate,
																   QCheckBox* _colorsCheckBox,
																   QCheckBox* _showRelatedCheckBox): QDialog(parent)
{
	dialogName=_dialogName;
	dialogTitle=_dialogTitle;
	eventLoop=_eventLoop;

	timesTable=_timesTable;
	oldItemDelegate=_oldItemDelegate;
	newItemDelegate=_newItemDelegate;

	colorsCheckBox=_colorsCheckBox;

	showRelatedCheckBox=_showRelatedCheckBox;

	setWindowTitle(dialogTitle);

	setAttribute(Qt::WA_DeleteOnClose);

	//resize(600, 400);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this, dialogName);
}

AddOrModifySpaceConstraintDialog::~AddOrModifySpaceConstraintDialog()
{
	if(timesTable!=nullptr){
		//assert(oldItemDelegate!=nullptr); don't assert this!!! It might be nullptr.
		assert(newItemDelegate!=nullptr);

		timesTable->setItemDelegate(oldItemDelegate);
		delete newItemDelegate;
	}

	saveFETDialogGeometry(this, dialogName);

	if(colorsCheckBox!=nullptr || showRelatedCheckBox!=nullptr){
		QSettings settings(COMPANY, PROGRAM);

		if(colorsCheckBox!=nullptr)
			settings.setValue(dialogName+QString("/use-colors"), colorsCheckBox->isChecked());

		if(showRelatedCheckBox!=nullptr)
			settings.setValue(dialogName+QString("/show-related"), showRelatedCheckBox->isChecked());
	}

	eventLoop->quit();
}

AddOrModifySpaceConstraint::AddOrModifySpaceConstraint(QWidget* parent, int _type, SpaceConstraint* _oldsc,
 const QString& _preselectedTeacherName, const QString& _preselectedStudentsSetName, const QString& _preselectedSubjectName, const QString& _preselectedActivityTagName,
 const QString& _preselectedRoomName)
{
	type=_type;
	oldsc=_oldsc;

	showRelatedCheckBox=nullptr;

	addEmpty=false;

	activityTagsLabel=nullptr;
	selectedActivityTagsLabel=nullptr;
	activityTagsListWidget=nullptr;
	selectedActivityTagsListWidget=nullptr;
	//addAllActivityTagsPushButton;
	clearActivityTagsPushButton=nullptr;
	selectedActivityTagsSet.clear();
	filterActivityTagsCheckBox=nullptr;

	selectedRealRoomsLabel=nullptr;
	selectedRealRoomsListWidget=nullptr;
	allRealRoomsLabel=nullptr;
	allRealRoomsListWidget=nullptr;
	clearSelectedRealRoomsPushButton=nullptr;

	selectedRoomsLabel=nullptr;
	selectedRoomsListWidget=nullptr;
	allRoomsLabel=nullptr;
	allRoomsListWidget=nullptr;
	clearSelectedRoomsPushButton=nullptr;

	intervalIsForRealDay=false;
	intervalStartHourLabel=nullptr;
	intervalStartHourComboBox=nullptr;
	intervalEndHourLabel=nullptr;
	intervalEndHourComboBox=nullptr;

	firstAddInstructionsLabel=nullptr;
	secondAddInstructionsLabel=nullptr;

	firstModifyInstructionsLabel=nullptr;
	secondModifyInstructionsLabel=nullptr;

	colorsCheckBox=nullptr;
	toggleAllPushButton=nullptr;

	timesTable=nullptr;
	oldItemDelegate=nullptr;
	newItemDelegate=nullptr;

	filterGroupBox=nullptr;

	teacherLabel=nullptr;
	teachersComboBox=nullptr;

	studentsLabel=nullptr;
	studentsComboBox=nullptr;

	subjectLabel=nullptr;
	subjectsComboBox=nullptr;

	activityTagLabel=nullptr;
	activityTagsComboBox=nullptr;

	roomLabel=nullptr;
	roomsComboBox=nullptr;

	activityLabel=nullptr;
	activitiesComboBox=nullptr;
	activitiesList.clear();
	initialActivityId=-1;

	helpPushButton=nullptr;

	labelForSpinBox=nullptr;
	spinBox=nullptr;

	checkBox=nullptr;

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

	if(oldsc!=nullptr)
		assert(oldsc->type==type);

	switch(type){
		//1
		case CONSTRAINT_BASIC_COMPULSORY_SPACE:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add basic compulsory space", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintBasicCompulsorySpace");
				}
				else{
					dialogTitle=tr("Modify basic compulsory space", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintBasicCompulsorySpace");
				}

				break;
			}
		//2
		case CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add room not available times", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintRoomNotAvailableTimes");

					firstAddInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
				}
				else{
					dialogTitle=tr("Modify room not available times", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintRoomNotAvailableTimes");

					firstModifyInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
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

				roomLabel=new QLabel(tr("Room"));
				roomsComboBox=new QComboBox;

				break;
			}
		//3
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOM:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add activity preferred room", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivityPreferredRoom");
				}
				else{
					dialogTitle=tr("Modify activity preferred room", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivityPreferredRoom");
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

				roomLabel=new QLabel(tr("Room"));
				roomsComboBox=new QComboBox;

				permanentlyLockedCheckBox=new QCheckBox(tr("Permanently locked"));
				permanentlyLockedLabel=new QLabel(tr("Select this if you want this constraint not to be unlockable from the 'Timetable' "
													 "menu (recommended). You can always remove this constraint from the constraints "
													 "dialogs or uncheck \"permanently locked\" checkbox by editing this constraint"));

				selectedRealRoomsLabel=new QLabel(tr("Selected", "It refers to real rooms"));
				selectedRealRoomsListWidget=new QListWidget;
				allRealRoomsLabel=new QLabel(tr("Real rooms"));
				allRealRoomsListWidget=new QListWidget;
				clearSelectedRealRoomsPushButton=new QPushButton(tr("Clear", "Clear the list of selected real rooms"));

				helpPushButton=new QPushButton(tr("Help"));

				break;
			}
		//4
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOMS:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add activity preferred rooms", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivityPreferredRooms");
				}
				else{
					dialogTitle=tr("Modify activity preferred rooms", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivityPreferredRooms");
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

				allRoomsLabel=new QLabel(tr("Rooms"));
				allRoomsListWidget=new QListWidget;
				selectedRoomsLabel=new QLabel(tr("Selected", "It refers to rooms"));
				selectedRoomsListWidget=new QListWidget;
				clearSelectedRoomsPushButton=new QPushButton(tr("Clear", "Clear the list of selected rooms"));

				break;
			}
		//5
		case CONSTRAINT_STUDENTS_SET_HOME_ROOM:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students set home room", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetHomeRoom");
				}
				else{
					dialogTitle=tr("Modify students set home room", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetHomeRoom");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				roomLabel=new QLabel(tr("Room"));
				roomsComboBox=new QComboBox;

				break;
			}
		//6
		case CONSTRAINT_STUDENTS_SET_HOME_ROOMS:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students set home rooms", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetHomeRooms");
				}
				else{
					dialogTitle=tr("Modify students set home rooms", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetHomeRooms");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				allRoomsLabel=new QLabel(tr("Rooms"));
				allRoomsListWidget=new QListWidget;
				selectedRoomsLabel=new QLabel(tr("Selected", "It refers to rooms"));
				selectedRoomsListWidget=new QListWidget;
				clearSelectedRoomsPushButton=new QPushButton(tr("Clear", "Clear the list of selected rooms"));

				break;
			}
		//7
		case CONSTRAINT_TEACHER_HOME_ROOM:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teacher home room", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherHomeRoom");
				}
				else{
					dialogTitle=tr("Modify teacher home room", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherHomeRoom");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				roomLabel=new QLabel(tr("Room"));
				roomsComboBox=new QComboBox;

				break;
			}
		//8
		case CONSTRAINT_TEACHER_HOME_ROOMS:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teacher home rooms", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherHomeRooms");
				}
				else{
					dialogTitle=tr("Modify teacher home rooms", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherHomeRooms");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				allRoomsLabel=new QLabel(tr("Rooms"));
				allRoomsListWidget=new QListWidget;
				selectedRoomsLabel=new QLabel(tr("Selected", "It refers to rooms"));
				selectedRoomsListWidget=new QListWidget;
				clearSelectedRoomsPushButton=new QPushButton(tr("Clear", "Clear the list of selected rooms"));

				break;
			}
		//9
		case CONSTRAINT_SUBJECT_PREFERRED_ROOM:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add subject preferred room", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintSubjectPreferredRoom");
				}
				else{
					dialogTitle=tr("Modify subject preferred room", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintSubjectPreferredRoom");
				}

				subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				roomLabel=new QLabel(tr("Room"));
				roomsComboBox=new QComboBox;

				break;
			}
		//10
		case CONSTRAINT_SUBJECT_PREFERRED_ROOMS:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add subject preferred rooms", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintSubjectPreferredRooms");
				}
				else{
					dialogTitle=tr("Modify subject preferred rooms", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintSubjectPreferredRooms");
				}

				subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				allRoomsLabel=new QLabel(tr("Rooms"));
				allRoomsListWidget=new QListWidget;
				selectedRoomsLabel=new QLabel(tr("Selected", "It refers to rooms"));
				selectedRoomsListWidget=new QListWidget;
				clearSelectedRoomsPushButton=new QPushButton(tr("Clear", "Clear the list of selected rooms"));

				break;
			}
		//11
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add subject activity tag preferred room", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintSubjectActivityTagPreferredRoom");
				}
				else{
					dialogTitle=tr("Modify subject activity tag preferred room", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintSubjectActivityTagPreferredRoom");
				}

				subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				roomLabel=new QLabel(tr("Room"));
				roomsComboBox=new QComboBox;

				break;
			}
		//12
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add subject activity tag preferred rooms", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintSubjectActivityTagPreferredRooms");
				}
				else{
					dialogTitle=tr("Modify subject activity tag preferred rooms", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintSubjectActivityTagPreferredRooms");
				}

				subjectLabel=new QLabel(tr("Subject"));
				subjectsComboBox=new QComboBox;

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				allRoomsLabel=new QLabel(tr("Rooms"));
				allRoomsListWidget=new QListWidget;
				selectedRoomsLabel=new QLabel(tr("Selected", "It refers to rooms"));
				selectedRoomsListWidget=new QListWidget;
				clearSelectedRoomsPushButton=new QPushButton(tr("Clear", "Clear the list of selected rooms"));

				break;
			}
		//13
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students max building changes per day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxBuildingChangesPerDay");
				}
				else{
					dialogTitle=tr("Modify students max building changes per day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxBuildingChangesPerDay");
				}

				labelForSpinBox=new QLabel(tr("Max building changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//14
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students set max building changes per day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxBuildingChangesPerDay");
				}
				else{
					dialogTitle=tr("Modify students set max building changes per day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxBuildingChangesPerDay");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max building changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//15
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students max building changes per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxBuildingChangesPerWeek");
				}
				else{
					dialogTitle=tr("Modify students max building changes per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxBuildingChangesPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max building changes per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(3);

				break;
			}
		//16
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students set max building changes per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxBuildingChangesPerWeek");
				}
				else{
					dialogTitle=tr("Modify students set max building changes per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxBuildingChangesPerWeek");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max building changes per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(3);

				break;
			}
		//17
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students min gaps between building changes", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMinGapsBetweenBuildingChanges");
				}
				else{
					dialogTitle=tr("Modify students min gaps between building changes", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMinGapsBetweenBuildingChanges");
				}

				labelForSpinBox=new QLabel(tr("Min gaps between building changes"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//18
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students set min gaps between building changes", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMinGapsBetweenBuildingChanges");
				}
				else{
					dialogTitle=tr("Modify students set min gaps between building changes", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMinGapsBetweenBuildingChanges");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps between building changes"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//19
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teachers max building changes per day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxBuildingChangesPerDay");
				}
				else{
					dialogTitle=tr("Modify teachers max building changes per day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxBuildingChangesPerDay");
				}

				labelForSpinBox=new QLabel(tr("Max building changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//20
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teacher max building changes per day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxBuildingChangesPerDay");
				}
				else{
					dialogTitle=tr("Modify teacher max building changes per day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxBuildingChangesPerDay");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max building changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//21
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teachers max building changes per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxBuildingChangesPerWeek");
				}
				else{
					dialogTitle=tr("Modify teachers max building changes per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxBuildingChangesPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max building changes per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(3);

				break;
			}
		//22
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teacher max building changes per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxBuildingChangesPerWeek");
				}
				else{
					dialogTitle=tr("Modify teacher max building changes per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxBuildingChangesPerWeek");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max building changes per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(3);

				break;
			}
		//23
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teachers min gaps between building changes", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinGapsBetweenBuildingChanges");
				}
				else{
					dialogTitle=tr("Modify teachers min gaps between building changes", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinGapsBetweenBuildingChanges");
				}

				labelForSpinBox=new QLabel(tr("Min gaps between building changes"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//24
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teacher min gaps between building changes", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinGapsBetweenBuildingChanges");
				}
				else{
					dialogTitle=tr("Modify teacher min gaps between building changes", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinGapsBetweenBuildingChanges");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps between building changes"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//25
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add activity tag preferred room", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivityTagPreferredRoom");
				}
				else{
					dialogTitle=tr("Modify activity tag preferred room", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivityTagPreferredRoom");
				}

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				roomLabel=new QLabel(tr("Room"));
				roomsComboBox=new QComboBox;

				break;
			}
		//26
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add activity tag preferred rooms", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivityTagPreferredRooms");
				}
				else{
					dialogTitle=tr("Modify activity tag preferred rooms", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivityTagPreferredRooms");
				}

				activityTagLabel=new QLabel(tr("Activity tag"));
				activityTagsComboBox=new QComboBox;

				allRoomsLabel=new QLabel(tr("Rooms"));
				allRoomsListWidget=new QListWidget;
				selectedRoomsLabel=new QLabel(tr("Selected", "It refers to rooms"));
				selectedRoomsListWidget=new QListWidget;
				clearSelectedRoomsPushButton=new QPushButton(tr("Clear", "Clear the list of selected rooms"));

				break;
			}
		//27
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add activities occupy max different rooms", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesOccupyMaxDifferentRooms");
				}
				else{
					dialogTitle=tr("Modify activities occupy max different rooms", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesOccupyMaxDifferentRooms");
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

				labelForSpinBox=new QLabel(tr("Max different rooms"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(MAX_ROOMS);
				spinBox->setValue(1);

				break;
			}
		//28
		case CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add activities same room if consecutive", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintActivitiesSameRoomIfConsecutive");
				}
				else{
					dialogTitle=tr("Modify activities same room if consecutive", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintActivitiesSameRoomIfConsecutive");
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
		//29
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students max room changes per day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxRoomChangesPerDay");
				}
				else{
					dialogTitle=tr("Modify students max room changes per day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxRoomChangesPerDay");
				}

				labelForSpinBox=new QLabel(tr("Max room changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//30
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students set max room changes per day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxRoomChangesPerDay");
				}
				else{
					dialogTitle=tr("Modify students set max room changes per day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxRoomChangesPerDay");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max room changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//31
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_WEEK:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students max room changes per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxRoomChangesPerWeek");
				}
				else{
					dialogTitle=tr("Modify students max room changes per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxRoomChangesPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max room changes per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(3);

				break;
			}
		//32
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students set max room changes per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxRoomChangesPerWeek");
				}
				else{
					dialogTitle=tr("Modify students set max room changes per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxRoomChangesPerWeek");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max room changes per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(3);

				break;
			}
		//33
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students min gaps between room changes", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMinGapsBetweenRoomChanges");
				}
				else{
					dialogTitle=tr("Modify students min gaps between room changes", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMinGapsBetweenRoomChanges");
				}

				labelForSpinBox=new QLabel(tr("Min gaps between room changes"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//34
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students set min gaps between room changes", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMinGapsBetweenRoomChanges");
				}
				else{
					dialogTitle=tr("Modify students set min gaps between room changes", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMinGapsBetweenRoomChanges");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps between room changes"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//35
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teachers max room changes per day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxRoomChangesPerDay");
				}
				else{
					dialogTitle=tr("Modify teachers max room changes per day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxRoomChangesPerDay");
				}

				labelForSpinBox=new QLabel(tr("Max room changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//36
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teacher max room changes per day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxRoomChangesPerDay");
				}
				else{
					dialogTitle=tr("Modify teacher max room changes per day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxRoomChangesPerDay");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max room changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//37
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teachers max room changes per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxRoomChangesPerWeek");
				}
				else{
					dialogTitle=tr("Modify teachers max room changes per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxRoomChangesPerWeek");
				}

				labelForSpinBox=new QLabel(tr("Max room changes per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(3);

				break;
			}
		//38
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teacher max room changes per week", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxRoomChangesPerWeek");
				}
				else{
					dialogTitle=tr("Modify teacher max room changes per week", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxRoomChangesPerWeek");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max room changes per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(3);

				break;
			}
		//39
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teachers min gaps between room changes", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMinGapsBetweenRoomChanges");
				}
				else{
					dialogTitle=tr("Modify teachers min gaps between room changes", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMinGapsBetweenRoomChanges");
				}

				labelForSpinBox=new QLabel(tr("Min gaps between room changes"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//40
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teacher min gaps between room changes", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMinGapsBetweenRoomChanges");
				}
				else{
					dialogTitle=tr("Modify teacher min gaps between room changes", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMinGapsBetweenRoomChanges");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Min gaps between room changes"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//41
		case CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teacher room not available times", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherRoomNotAvailableTimes");

					firstAddInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
					 "This is an explanation in a dialog for a constraint. It says that symbol X (or red) means that this slot is not allowed, "
					 "and an empty cell (or green) means that the slot is allowed"));
				}
				else{
					dialogTitle=tr("Modify teacher room not available times", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherRoomNotAvailableTimes");

					firstModifyInstructionsLabel=new QLabel(tr("X (red)=not allowed, empty (green)=allowed",
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

				roomLabel=new QLabel(tr("Room"));
				roomsComboBox=new QComboBox;

				break;
			}
		//42
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students max room changes per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxRoomChangesPerRealDay");
				}
				else{
					dialogTitle=tr("Modify students max room changes per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxRoomChangesPerRealDay");
				}

				labelForSpinBox=new QLabel(tr("Max room changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//43
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students set max room changes per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxRoomChangesPerRealDay");
				}
				else{
					dialogTitle=tr("Modify students set max room changes per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxRoomChangesPerRealDay");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max room changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//44
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teachers max room changes per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxRoomChangesPerRealDay");
				}
				else{
					dialogTitle=tr("Modify teachers max room changes per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxRoomChangesPerRealDay");
				}

				labelForSpinBox=new QLabel(tr("Max room changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//45
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teacher max room changes per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxRoomChangesPerRealDay");
				}
				else{
					dialogTitle=tr("Modify teacher max room changes per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxRoomChangesPerRealDay");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max room changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//46
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students max building changes per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxBuildingChangesPerRealDay");
				}
				else{
					dialogTitle=tr("Modify students max building changes per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxBuildingChangesPerRealDay");
				}

				labelForSpinBox=new QLabel(tr("Max building changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//47
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students set max building changes per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxBuildingChangesPerRealDay");
				}
				else{
					dialogTitle=tr("Modify students set max building changes per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxBuildingChangesPerRealDay");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max building changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//48
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teachers max building changes per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxBuildingChangesPerRealDay");
				}
				else{
					dialogTitle=tr("Modify teachers max building changes per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxBuildingChangesPerRealDay");
				}

				labelForSpinBox=new QLabel(tr("Max building changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//49
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teacher max building changes per real day", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxBuildingChangesPerRealDay");
				}
				else{
					dialogTitle=tr("Modify teacher max building changes per real day", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxBuildingChangesPerRealDay");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max building changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//50
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students max building changes per day in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxBuildingChangesPerDayInInterval");
				}
				else{
					dialogTitle=tr("Modify students max building changes per day in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxBuildingChangesPerDayInInterval");
				}

				labelForSpinBox=new QLabel(tr("Max building changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//51
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students set max building changes per day in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxBuildingChangesPerDayInInterval");
				}
				else{
					dialogTitle=tr("Modify students set max building changes per day in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxBuildingChangesPerDayInInterval");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max building changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//52
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teachers max building changes per day in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxBuildingChangesPerDayInInterval");
				}
				else{
					dialogTitle=tr("Modify teachers max building changes per day in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxBuildingChangesPerDayInInterval");
				}

				labelForSpinBox=new QLabel(tr("Max building changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//53
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teacher max building changes per day in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxBuildingChangesPerDayInInterval");
				}
				else{
					dialogTitle=tr("Modify teacher max building changes per day in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxBuildingChangesPerDayInInterval");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max building changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//54
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students max building changes per real day in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxBuildingChangesPerRealDayInInterval");
				}
				else{
					dialogTitle=tr("Modify students max building changes per real day in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxBuildingChangesPerRealDayInInterval");
				}

				labelForSpinBox=new QLabel(tr("Max building changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				intervalIsForRealDay=true;
				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//55
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students set max building changes per real day in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval");
				}
				else{
					dialogTitle=tr("Modify students set max building changes per real day in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max building changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				intervalIsForRealDay=true;
				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//56
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teachers max building changes per real day in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxBuildingChangesPerRealDayInInterval");
				}
				else{
					dialogTitle=tr("Modify teachers max building changes per real day in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxBuildingChangesPerRealDayInInterval");
				}

				labelForSpinBox=new QLabel(tr("Max building changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				intervalIsForRealDay=true;
				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//57
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teacher max building changes per real day in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxBuildingChangesPerRealDayInInterval");
				}
				else{
					dialogTitle=tr("Modify teacher max building changes per real day in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxBuildingChangesPerRealDayInInterval");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max building changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				intervalIsForRealDay=true;
				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//58
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students max room changes per day in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxRoomChangesPerDayInInterval");
				}
				else{
					dialogTitle=tr("Modify students max room changes per day in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxRoomChangesPerDayInInterval");
				}

				labelForSpinBox=new QLabel(tr("Max room changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//59
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students set max room changes per day in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxRoomChangesPerDayInInterval");
				}
				else{
					dialogTitle=tr("Modify students set max room changes per day in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxRoomChangesPerDayInInterval");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max room changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//60
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teachers max room changes per day in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxRoomChangesPerDayInInterval");
				}
				else{
					dialogTitle=tr("Modify teachers max room changes per day in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxRoomChangesPerDayInInterval");
				}

				labelForSpinBox=new QLabel(tr("Max room changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//61
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teacher max room changes per day in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxRoomChangesPerDayInInterval");
				}
				else{
					dialogTitle=tr("Modify teacher max room changes per day in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxRoomChangesPerDayInInterval");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max room changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//62
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students max room changes per real day in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsMaxRoomChangesPerRealDayInInterval");
				}
				else{
					dialogTitle=tr("Modify students max room changes per real day in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsMaxRoomChangesPerRealDayInInterval");
				}

				labelForSpinBox=new QLabel(tr("Max room changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				intervalIsForRealDay=true;
				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//63
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add students set max room changes per real day in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintStudentsSetMaxRoomChangesPerRealDayInInterval");
				}
				else{
					dialogTitle=tr("Modify students set max room changes per real day in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintStudentsSetMaxRoomChangesPerRealDayInInterval");
				}

				studentsLabel=new QLabel(tr("Students set"));
				studentsComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max room changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				intervalIsForRealDay=true;
				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//64
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teachers max room changes per real day in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeachersMaxRoomChangesPerRealDayInInterval");
				}
				else{
					dialogTitle=tr("Modify teachers max room changes per real day in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeachersMaxRoomChangesPerRealDayInInterval");
				}

				labelForSpinBox=new QLabel(tr("Max room changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				intervalIsForRealDay=true;
				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//65
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add teacher max room changes per real day in interval", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintTeacherMaxRoomChangesPerRealDayInInterval");
				}
				else{
					dialogTitle=tr("Modify teacher max room changes per real day in interval", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintTeacherMaxRoomChangesPerRealDayInInterval");
				}

				teacherLabel=new QLabel(tr("Teacher"));
				teachersComboBox=new QComboBox;

				labelForSpinBox=new QLabel(tr("Max room changes per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(0);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(0);

				intervalIsForRealDay=true;
				intervalStartHourLabel=new QLabel(tr("Interval start hour (included)"));
				intervalStartHourComboBox=new QComboBox;
				intervalEndHourLabel=new QLabel(tr("Interval end hour (not included)"));
				intervalEndHourComboBox=new QComboBox;

				break;
			}
		//66
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add room max activity tags per day from set", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintRoomMaxActivityTagsPerDayFromSet");
				}
				else{
					dialogTitle=tr("Modify room max activity tags per day from set", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintRoomMaxActivityTagsPerDayFromSet");
				}

				roomLabel=new QLabel(tr("Room"));
				roomsComboBox=new QComboBox;

				activityTagsLabel=new QLabel(tr("Activity tags"));
				selectedActivityTagsLabel=new QLabel(tr("Selected", "It refers to activity tags"));
				activityTagsListWidget=new QListWidget;
				selectedActivityTagsListWidget=new QListWidget;
				//addAllActivityTagsPushButton=new QPushButton(tr("All", "Add all the activity tags to the list of selected activity tags"));
				clearActivityTagsPushButton=new QPushButton(tr("Clear", "Clear the list of selected activity tags"));
				filterActivityTagsCheckBox=new QCheckBox(tr("Filter"));
				filterActivityTagsCheckBox->setChecked(false);

				labelForSpinBox=new QLabel(tr("Max activity tags per day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//67
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add room max activity tags per real day from set", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintRoomMaxActivityTagsPerRealDayFromSet");
				}
				else{
					dialogTitle=tr("Modify room max activity tags per real day from set", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintRoomMaxActivityTagsPerRealDayFromSet");
				}

				roomLabel=new QLabel(tr("Room"));
				roomsComboBox=new QComboBox;

				activityTagsLabel=new QLabel(tr("Activity tags"));
				selectedActivityTagsLabel=new QLabel(tr("Selected", "It refers to activity tags"));
				activityTagsListWidget=new QListWidget;
				selectedActivityTagsListWidget=new QListWidget;
				//addAllActivityTagsPushButton=new QPushButton(tr("All", "Add all the activity tags to the list of selected activity tags"));
				clearActivityTagsPushButton=new QPushButton(tr("Clear", "Clear the list of selected activity tags"));
				filterActivityTagsCheckBox=new QCheckBox(tr("Filter"));
				filterActivityTagsCheckBox->setChecked(false);

				labelForSpinBox=new QLabel(tr("Max activity tags per real day"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(2*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}
		//68
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_WEEK_FROM_SET:
			{
				if(oldsc==nullptr){
					dialogTitle=tr("Add room max activity tags per week from set", "The title of the dialog to add a new constraint of this type");
					dialogName=QString("AddConstraintRoomMaxActivityTagsPerWeekFromSet");
				}
				else{
					dialogTitle=tr("Modify room max activity tags per week from set", "The title of the dialog to modify a constraint of this type");
					dialogName=QString("ModifyConstraintRoomMaxActivityTagsPerWeekFromSet");
				}

				roomLabel=new QLabel(tr("Room"));
				roomsComboBox=new QComboBox;

				activityTagsLabel=new QLabel(tr("Activity tags"));
				selectedActivityTagsLabel=new QLabel(tr("Selected", "It refers to activity tags"));
				activityTagsListWidget=new QListWidget;
				selectedActivityTagsListWidget=new QListWidget;
				//addAllActivityTagsPushButton=new QPushButton(tr("All", "Add all the activity tags to the list of selected activity tags"));
				clearActivityTagsPushButton=new QPushButton(tr("Clear", "Clear the list of selected activity tags"));
				filterActivityTagsCheckBox=new QCheckBox(tr("Filter"));
				filterActivityTagsCheckBox->setChecked(false);

				labelForSpinBox=new QLabel(tr("Max activity tags per week"));
				spinBox=new QSpinBox;
				spinBox->setMinimum(1);
				spinBox->setMaximum(gt.rules.nDaysPerWeek*gt.rules.nHoursPerDay);
				spinBox->setValue(1);

				break;
			}

		default:
			assert(0);
			break;
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

			connect(showRelatedCheckBox, &QCheckBox::toggled, this, &AddOrModifySpaceConstraint::showRelatedCheckBoxToggled);

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

	QVBoxLayout* roomLayout=nullptr;
	if(roomsComboBox!=nullptr){
		roomLayout=new QVBoxLayout;
		if(roomLabel!=nullptr)
			roomLayout->addWidget(roomLabel);
		roomLayout->addWidget(roomsComboBox);

		/*if(addEmpty)
			roomsComboBox->addItem(QString(""));*/
		for(Room* rm : std::as_const(gt.rules.roomsList))
			roomsComboBox->addItem(rm->name);

		roomsComboBox->setCurrentIndex(0);
	}

	if(oldsc==nullptr && !addEmpty){
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

		if(subjectsComboBox!=nullptr){
			if(!_preselectedSubjectName.isEmpty()){
				int q=subjectsComboBox->findText(_preselectedSubjectName);
				if(q>=0){
					subjectsComboBox->setCurrentIndex(q);
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

	if(oldsc==nullptr){
		if(roomsComboBox!=nullptr){
			if(!_preselectedRoomName.isEmpty()){
				int q=roomsComboBox->findText(_preselectedRoomName);
				if(q>=0){
					roomsComboBox->setCurrentIndex(q);
				}
			}
		}
	}

	assert(activitiesComboBox==nullptr || activitiesListWidget==nullptr);
	if(teachersComboBox!=nullptr){
		if(activitiesComboBox!=nullptr)
			connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifySpaceConstraint::filterActivitiesComboBox);
		if(activitiesListWidget!=nullptr)
			connect(teachersComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifySpaceConstraint::filterActivitiesListWidget);
	}
	if(studentsComboBox!=nullptr){
		if(activitiesComboBox!=nullptr)
			connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifySpaceConstraint::showRelatedCheckBoxToggled);
		if(activitiesListWidget!=nullptr)
			connect(studentsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifySpaceConstraint::showRelatedCheckBoxToggled);
	}
	if(subjectsComboBox!=nullptr){
		if(activitiesComboBox!=nullptr)
			connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifySpaceConstraint::filterActivitiesComboBox);
		if(activitiesListWidget!=nullptr)
			connect(subjectsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifySpaceConstraint::filterActivitiesListWidget);
	}
	if(activityTagsComboBox!=nullptr){
		if(activitiesComboBox!=nullptr)
			connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifySpaceConstraint::filterActivitiesComboBox);
		if(activitiesListWidget!=nullptr)
			connect(activityTagsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifySpaceConstraint::filterActivitiesListWidget);
	}
	//For rooms: not

	if(selectedRealRoomsListWidget!=nullptr){
		assert(selectedRealRoomsLabel!=nullptr);
		assert(allRealRoomsLabel!=nullptr);
		assert(allRealRoomsListWidget!=nullptr);
		assert(clearSelectedRealRoomsPushButton!=nullptr);

		connect(clearSelectedRealRoomsPushButton, &QPushButton::clicked, this, &AddOrModifySpaceConstraint::clearSelectedRealRooms);
		connect(allRealRoomsListWidget, &QListWidget::itemDoubleClicked, this, &AddOrModifySpaceConstraint::addRealRoom);
		connect(selectedRealRoomsListWidget, &QListWidget::itemDoubleClicked, this, &AddOrModifySpaceConstraint::removeRealRoom);

		allRealRoomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
		selectedRealRoomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

		allRealRoomsListWidget->clear();
		for(Room* rm : std::as_const(gt.rules.roomsList))
			if(rm->isVirtual==false)
				allRealRoomsListWidget->addItem(rm->name);
		allRealRoomsListWidget->setCurrentRow(0);
		selectedRealRoomsListWidget->clear();
	}

	if(selectedRoomsListWidget!=nullptr){
		assert(selectedRoomsLabel!=nullptr);
		assert(allRoomsLabel!=nullptr);
		assert(allRoomsListWidget!=nullptr);
		assert(clearSelectedRoomsPushButton!=nullptr);

		connect(clearSelectedRoomsPushButton, &QPushButton::clicked, this, &AddOrModifySpaceConstraint::clearSelectedRooms);
		connect(allRoomsListWidget, &QListWidget::itemDoubleClicked, this, &AddOrModifySpaceConstraint::addRoom);
		connect(selectedRoomsListWidget, &QListWidget::itemDoubleClicked, this, &AddOrModifySpaceConstraint::removeRoom);

		allRoomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
		selectedRoomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

		allRoomsListWidget->clear();
		for(Room* rm : std::as_const(gt.rules.roomsList))
			allRoomsListWidget->addItem(rm->name);
		allRoomsListWidget->setCurrentRow(0);
		selectedRoomsListWidget->clear();
	}

	if(permanentlyLockedCheckBox!=nullptr && permanentlyLockedLabel!=nullptr){
		permanentlyLockedCheckBox->setChecked(true);
		permanentlyLockedLabel->setWordWrap(true);
		//permanentlyLockedLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	}

	if(timesTable!=nullptr){
		connect(timesTable, &CornerEnabledTableWidget::itemClicked, this, &AddOrModifySpaceConstraint::itemClicked);

		assert(colorsCheckBox!=nullptr);
		connect(colorsCheckBox, &QCheckBox::toggled, this, &AddOrModifySpaceConstraint::colorsCheckBoxToggled);

		assert(toggleAllPushButton!=nullptr);
		connect(toggleAllPushButton, &QPushButton::clicked, this, &AddOrModifySpaceConstraint::toggleAllClicked);

		initTimesTable(timesTable);

		oldItemDelegate=timesTable->itemDelegate();
		newItemDelegate=new AddOrModifySpaceConstraintTimesTableDelegate(nullptr, gt.rules.nHoursPerDay, timesTable->columnCount());
		timesTable->setItemDelegate(newItemDelegate);

		timesTable->resizeRowsToContents();
		//timesTable->resizeColumnsToContents();

		connect(timesTable->horizontalHeader(), &QHeaderView::sectionClicked, this, &AddOrModifySpaceConstraint::horizontalHeaderClicked);
		connect(timesTable->verticalHeader(), &QHeaderView::sectionClicked, this, &AddOrModifySpaceConstraint::verticalHeaderClicked);

		timesTable->setSelectionMode(QAbstractItemView::NoSelection);

		setStretchAvailabilityTableNicely(timesTable);

		connect(timesTable, &CornerEnabledTableWidget::cellEntered, this, &AddOrModifySpaceConstraint::cellEntered);
		timesTable->setMouseTracking(true);
	}

	if(activitiesListWidget!=nullptr){
		assert(activitiesLabel!=nullptr);
		assert(selectedActivitiesLabel!=nullptr);
		assert(selectedActivitiesListWidget!=nullptr);
		assert(addAllActivitiesPushButton!=nullptr);
		assert(clearActivitiesPushButton!=nullptr);

		connect(clearActivitiesPushButton, &QPushButton::clicked, this, &AddOrModifySpaceConstraint::clearActivitiesClicked);
		connect(activitiesListWidget, &QListWidget::itemDoubleClicked, this, &AddOrModifySpaceConstraint::addActivity);
		connect(addAllActivitiesPushButton, &QPushButton::clicked, this, &AddOrModifySpaceConstraint::addAllActivitiesClicked);
		connect(selectedActivitiesListWidget, &QListWidget::itemDoubleClicked, this, &AddOrModifySpaceConstraint::removeActivity);

		activitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
		selectedActivitiesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	}

	if(activityTagsListWidget!=nullptr){
		assert(activityTagsLabel!=nullptr);
		assert(selectedActivityTagsLabel!=nullptr);
		assert(selectedActivityTagsListWidget!=nullptr);
		//assert(addAllActivityTagsPushButton!=nullptr);
		assert(clearActivityTagsPushButton!=nullptr);

		assert(filterActivityTagsCheckBox!=nullptr);

		connect(clearActivityTagsPushButton, &QPushButton::clicked, this, &AddOrModifySpaceConstraint::clearActivityTagsClicked);
		connect(activityTagsListWidget, &QListWidget::itemDoubleClicked, this, &AddOrModifySpaceConstraint::addActivityTag);
		//connect(addAllActivityTagsPushButton, &QPushButton::clicked, this, &AddOrModifySpaceConstraint::addAllActivityTagsClicked);
		connect(selectedActivityTagsListWidget, &QListWidget::itemDoubleClicked, this, &AddOrModifySpaceConstraint::removeActivityTag);

		activityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
		selectedActivityTagsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	}

	eventLoop=new QEventLoop;

	dialog=new AddOrModifySpaceConstraintDialog(parent, dialogName, dialogTitle, eventLoop,
												timesTable, oldItemDelegate, newItemDelegate,
												colorsCheckBox,
												showRelatedCheckBox);
	//dialog->setAttribute(Qt::WA_DeleteOnClose);

	//dialog->setWindowTitle(dialogTitle);

	//centerWidgetOnScreen(dialog);
	//restoreFETDialogGeometry(dialog, dialogName);
	
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
	if(roomsComboBox!=nullptr){
		QSize tmp=roomsComboBox->minimumSizeHint();
		Q_UNUSED(tmp);
	}
	if(activitiesComboBox!=nullptr){
		QSize tmp=activitiesComboBox->minimumSizeHint();
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

	if(activitiesComboBox!=nullptr)
		activitiesComboBox->setMaximumWidth(maxRecommendedWidth(dialog));

	addConstraintPushButton=nullptr;
	addConstraintsPushButton=nullptr;
	closePushButton=nullptr;
	okPushButton=nullptr;
	cancelPushButton=nullptr;
	if(oldsc==nullptr){
		addConstraintPushButton=new QPushButton(tr("Add constraint"));
		addConstraintPushButton->setDefault(true);
		
		switch(type){
			case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
				[[fallthrough]];
			case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
				[[fallthrough]];
			case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
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
	if(colorsCheckBox!=nullptr)
		buttons->addWidget(colorsCheckBox);
	if(toggleAllPushButton!=nullptr)
		buttons->addWidget(toggleAllPushButton);
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
	if(type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM)
		assert(selectedRealRoomsListWidget!=nullptr);
	else
		assert(selectedRealRoomsListWidget==nullptr);
	if(selectedRealRoomsListWidget!=nullptr){
		weightLabel=new QLabel(tr("Weight percentage (allowed: 0.0%-100.0%, necessary 100% if list nonempty)"));
	}
	else if(spaceConstraintCanHaveAnyWeight(type)){
		weightLabel=new QLabel(tr("Weight percentage (allowed: 0.0%-100.0%)"));
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

		spinBoxLayout->addWidget(labelForSpinBox);
		spinBoxLayout->addWidget(spinBox);
	}

	QVBoxLayout* wholeDialog=new QVBoxLayout(dialog);
	//wholeDialog->addStretch();

	if(oldsc==nullptr){
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

	if(type==CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES){
		assert(filterGroupBox==nullptr);

		assert(teacherLayout!=nullptr);
		assert(roomLayout!=nullptr);

		assert(studentsLayout==nullptr);
		assert(subjectLayout==nullptr);
		assert(activityTagLayout==nullptr);

		QHBoxLayout* layout=new QHBoxLayout;
		layout->addLayout(teacherLayout);
		layout->addLayout(roomLayout);

		wholeDialog->addLayout(layout);
	}
	else if(filterGroupBox!=nullptr){
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

		//rooms: not.
	}
	/*else if(teacherLayout!=nullptr && studentsLayout!=nullptr && subjectLayout!=nullptr && activityTagLayout!=nullptr){
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
	}*/
	else{
		if(teacherLayout!=nullptr)
			wholeDialog->addLayout(teacherLayout);
		if(studentsLayout!=nullptr)
			wholeDialog->addLayout(studentsLayout);
		if(subjectLayout!=nullptr)
			wholeDialog->addLayout(subjectLayout);
		if(activityTagLayout!=nullptr)
			wholeDialog->addLayout(activityTagLayout);
		if(roomLayout!=nullptr)
			wholeDialog->addLayout(roomLayout);
	}

	if(intervalStartHourLabel!=nullptr && intervalStartHourComboBox!=nullptr && intervalEndHourLabel!=nullptr && intervalEndHourComboBox!=nullptr){
		if(!intervalIsForRealDay){
			for(int i=0; i<gt.rules.nHoursPerDay; i++)
				intervalStartHourComboBox->addItem(gt.rules.hoursOfTheDay[i]);
			intervalStartHourComboBox->setCurrentIndex(0);

			for(int i=0; i<gt.rules.nHoursPerDay; i++)
				intervalEndHourComboBox->addItem(gt.rules.hoursOfTheDay[i]);
			intervalEndHourComboBox->addItem(tr("End of day"));
			intervalEndHourComboBox->setCurrentIndex(2);
		}
		else{ //interval is for a real day
			for(int i=0; i<gt.rules.nHoursPerDay; i++)
				intervalStartHourComboBox->addItem(tr("Morning %1", "Hour %1 of the morning").arg(gt.rules.hoursOfTheDay[i]));
			for(int i=0; i<gt.rules.nHoursPerDay; i++)
				intervalStartHourComboBox->addItem(tr("Afternoon %1", "Hour %1 of the afternoon").arg(gt.rules.hoursOfTheDay[i]));
			intervalStartHourComboBox->setCurrentIndex(0);

			intervalEndHourComboBox->clear();
			for(int i=0; i<gt.rules.nHoursPerDay; i++)
				intervalEndHourComboBox->addItem(tr("Morning %1", "Hour %1 of the morning").arg(gt.rules.hoursOfTheDay[i]));
			for(int i=0; i<gt.rules.nHoursPerDay; i++)
				intervalEndHourComboBox->addItem(tr("Afternoon %1", "Hour %1 of the afternoon").arg(gt.rules.hoursOfTheDay[i]));
			intervalEndHourComboBox->addItem(tr("End of real day"));
			intervalEndHourComboBox->setCurrentIndex(2);
		}

		wholeDialog->addWidget(intervalStartHourLabel);
		wholeDialog->addWidget(intervalStartHourComboBox);
		wholeDialog->addWidget(intervalEndHourLabel);
		wholeDialog->addWidget(intervalEndHourComboBox);
	}

	if(activitiesComboBox!=nullptr && activityLabel!=nullptr && selectedRealRoomsListWidget==nullptr){
		wholeDialog->addWidget(activityLabel);
		wholeDialog->addWidget(activitiesComboBox);
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
		//assert(addAllActivityTagsPushButton!=nullptr);
		assert(clearActivityTagsPushButton!=nullptr);

		assert(filterActivityTagsCheckBox!=nullptr);

		QVBoxLayout* layout1=new QVBoxLayout;
		layout1->addWidget(activityTagsLabel);
		layout1->addWidget(activityTagsListWidget);
		//layout1->addWidget(addAllActivityTagsPushButton);
		layout1->addWidget(filterActivityTagsCheckBox);

		QVBoxLayout* layout2=new QVBoxLayout;
		layout2->addWidget(selectedActivityTagsLabel);
		layout2->addWidget(selectedActivityTagsListWidget);
		layout2->addWidget(clearActivityTagsPushButton);

		QHBoxLayout* layout3=new QHBoxLayout;
		layout3->addLayout(layout1);
		layout3->addLayout(layout2);

		wholeDialog->addLayout(layout3);
	}

	if(timesTable!=nullptr)
		wholeDialog->addWidget(timesTable);
	if(spinBoxLayout!=nullptr)
		wholeDialog->addLayout(spinBoxLayout);
	if(checkBox!=nullptr)
		wholeDialog->addWidget(checkBox);

	if(selectedRealRoomsListWidget!=nullptr){
		assert(selectedRealRoomsLabel!=nullptr);
		assert(allRealRoomsLabel!=nullptr);
		assert(allRealRoomsListWidget!=nullptr);
		assert(clearSelectedRealRoomsPushButton!=nullptr);

		QVBoxLayout* layout1=new QVBoxLayout;
		layout1->addLayout(roomLayout);

		QVBoxLayout* layout2=new QVBoxLayout;
		layout2->addWidget(selectedRealRoomsLabel);
		layout2->addWidget(selectedRealRoomsListWidget);
		layout2->addWidget(clearSelectedRealRoomsPushButton);

		QVBoxLayout* layout3=new QVBoxLayout;
		layout3->addWidget(allRealRoomsLabel);
		layout3->addWidget(allRealRoomsListWidget);

		QHBoxLayout* hLayout=new QHBoxLayout;
		hLayout->addLayout(layout1);
		hLayout->addLayout(layout2);
		hLayout->addLayout(layout3);

		wholeDialog->addWidget(activityLabel);
		wholeDialog->addWidget(activitiesComboBox);
		wholeDialog->addLayout(hLayout);
	}

	if(selectedRoomsListWidget!=nullptr){
		assert(selectedRoomsLabel!=nullptr);
		assert(allRoomsLabel!=nullptr);
		assert(allRoomsListWidget!=nullptr);
		assert(clearSelectedRoomsPushButton!=nullptr);

		QVBoxLayout* layout1=new QVBoxLayout;
		layout1->addWidget(allRoomsLabel);
		layout1->addWidget(allRoomsListWidget);

		QVBoxLayout* layout2=new QVBoxLayout;
		layout2->addWidget(selectedRoomsLabel);
		layout2->addWidget(selectedRoomsListWidget);
		layout2->addWidget(clearSelectedRoomsPushButton);

		QHBoxLayout* hLayout=new QHBoxLayout;
		hLayout->addLayout(layout1);
		hLayout->addLayout(layout2);

		wholeDialog->addLayout(hLayout);
	}

	if(permanentlyLockedCheckBox!=nullptr)
		wholeDialog->addWidget(permanentlyLockedCheckBox);
	if(permanentlyLockedLabel!=nullptr)
		wholeDialog->addWidget(permanentlyLockedLabel);

	//wholeDialog->addStretch();
	wholeDialog->addLayout(weight);
	wholeDialog->addLayout(buttons);

	if(addConstraintPushButton!=nullptr)
		connect(addConstraintPushButton, &QPushButton::clicked, this, &AddOrModifySpaceConstraint::addConstraintClicked);
	if(addConstraintsPushButton!=nullptr)
		connect(addConstraintsPushButton, &QPushButton::clicked, this, &AddOrModifySpaceConstraint::addConstraintsClicked);
	if(okPushButton!=nullptr)
		connect(okPushButton, &QPushButton::clicked, this, &AddOrModifySpaceConstraint::okClicked);
	if(helpPushButton!=nullptr)
		connect(helpPushButton, &QPushButton::clicked, this, &AddOrModifySpaceConstraint::helpClicked);
	if(closePushButton!=nullptr)
		connect(closePushButton, &QPushButton::clicked, this, &AddOrModifySpaceConstraint::closeClicked);
	if(cancelPushButton!=nullptr)
		connect(cancelPushButton, &QPushButton::clicked, this, &AddOrModifySpaceConstraint::cancelClicked);

	if(oldsc!=nullptr){
		weightLineEdit->setText(CustomFETString::number(oldsc->weightPercentage));
		switch(type){
			//1
			case CONSTRAINT_BASIC_COMPULSORY_SPACE:
				{
					break;
				}
			//2
			case CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES:
				{
					ConstraintRoomNotAvailableTimes* ctr=(ConstraintRoomNotAvailableTimes*)oldsc;

					fillTimesTable(timesTable, ctr->days, ctr->hours, true);

					roomsComboBox->setCurrentIndex(roomsComboBox->findText(ctr->room));

					break;
				}
			//3
			case CONSTRAINT_ACTIVITY_PREFERRED_ROOM:
				{
					ConstraintActivityPreferredRoom* ctr=(ConstraintActivityPreferredRoom*)oldsc;

					initialActivityId=ctr->activityId;
					roomsComboBox->setCurrentIndex(roomsComboBox->findText(ctr->roomName));

					allRealRoomsListWidget->clear();
					for(Room* rm : std::as_const(gt.rules.roomsList))
						if(rm->isVirtual==false)
							allRealRoomsListWidget->addItem(rm->name);
					allRealRoomsListWidget->setCurrentRow(0);

					selectedRealRoomsListWidget->clear();
					for(const QString& rrn : std::as_const(ctr->preferredRealRoomsNames))
						selectedRealRoomsListWidget->addItem(rrn);
					selectedRealRoomsListWidget->setCurrentRow(0);

					permanentlyLockedCheckBox->setChecked(ctr->permanentlyLocked);

					break;
				}
			//4
			case CONSTRAINT_ACTIVITY_PREFERRED_ROOMS:
				{
					ConstraintActivityPreferredRooms* ctr=(ConstraintActivityPreferredRooms*)oldsc;

					initialActivityId=ctr->activityId;

					for(const QString& rn : ctr->roomsNames)
						selectedRoomsListWidget->addItem(rn);

					break;
				}
			//5
			case CONSTRAINT_STUDENTS_SET_HOME_ROOM:
				{
					ConstraintStudentsSetHomeRoom* ctr=(ConstraintStudentsSetHomeRoom*)oldsc;

					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					roomsComboBox->setCurrentIndex(roomsComboBox->findText(ctr->roomName));

					break;
				}
			//6
			case CONSTRAINT_STUDENTS_SET_HOME_ROOMS:
				{
					ConstraintStudentsSetHomeRooms* ctr=(ConstraintStudentsSetHomeRooms*)oldsc;

					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					for(const QString& rn : ctr->roomsNames)
						selectedRoomsListWidget->addItem(rn);

					break;
				}
			//7
			case CONSTRAINT_TEACHER_HOME_ROOM:
				{
					ConstraintTeacherHomeRoom* ctr=(ConstraintTeacherHomeRoom*)oldsc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					roomsComboBox->setCurrentIndex(roomsComboBox->findText(ctr->roomName));

					break;
				}
			//8
			case CONSTRAINT_TEACHER_HOME_ROOMS:
				{
					ConstraintTeacherHomeRooms* ctr=(ConstraintTeacherHomeRooms*)oldsc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					for(const QString& rn : ctr->roomsNames)
						selectedRoomsListWidget->addItem(rn);

					break;
				}
			//9
			case CONSTRAINT_SUBJECT_PREFERRED_ROOM:
				{
					ConstraintSubjectPreferredRoom* ctr=(ConstraintSubjectPreferredRoom*)oldsc;

					subjectsComboBox->setCurrentIndex(subjectsComboBox->findText(ctr->subjectName));

					roomsComboBox->setCurrentIndex(roomsComboBox->findText(ctr->roomName));

					break;
				}
			//10
			case CONSTRAINT_SUBJECT_PREFERRED_ROOMS:
				{
					ConstraintSubjectPreferredRooms* ctr=(ConstraintSubjectPreferredRooms*)oldsc;

					subjectsComboBox->setCurrentIndex(subjectsComboBox->findText(ctr->subjectName));

					for(const QString& rn : ctr->roomsNames)
						selectedRoomsListWidget->addItem(rn);

					break;
				}
			//11
			case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM:
				{
					ConstraintSubjectActivityTagPreferredRoom* ctr=(ConstraintSubjectActivityTagPreferredRoom*)oldsc;

					subjectsComboBox->setCurrentIndex(subjectsComboBox->findText(ctr->subjectName));
					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));

					roomsComboBox->setCurrentIndex(roomsComboBox->findText(ctr->roomName));

					break;
				}
			//12
			case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS:
				{
					ConstraintSubjectActivityTagPreferredRooms* ctr=(ConstraintSubjectActivityTagPreferredRooms*)oldsc;

					subjectsComboBox->setCurrentIndex(subjectsComboBox->findText(ctr->subjectName));
					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));

					for(const QString& rn : ctr->roomsNames)
						selectedRoomsListWidget->addItem(rn);

					break;
				}
			//13
			case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY:
				{
					ConstraintStudentsMaxBuildingChangesPerDay* ctr=(ConstraintStudentsMaxBuildingChangesPerDay*)oldsc;

					spinBox->setValue(ctr->maxBuildingChangesPerDay);

					break;
				}
			//14
			case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY:
				{
					ConstraintStudentsSetMaxBuildingChangesPerDay* ctr=(ConstraintStudentsSetMaxBuildingChangesPerDay*)oldsc;

					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxBuildingChangesPerDay);

					break;
				}
			//15
			case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK:
				{
					ConstraintStudentsMaxBuildingChangesPerWeek* ctr=(ConstraintStudentsMaxBuildingChangesPerWeek*)oldsc;

					spinBox->setValue(ctr->maxBuildingChangesPerWeek);

					break;
				}
			//16
			case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK:
				{
					ConstraintStudentsSetMaxBuildingChangesPerWeek* ctr=(ConstraintStudentsSetMaxBuildingChangesPerWeek*)oldsc;

					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxBuildingChangesPerWeek);

					break;
				}
			//17
			case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
				{
					ConstraintStudentsMinGapsBetweenBuildingChanges* ctr=(ConstraintStudentsMinGapsBetweenBuildingChanges*)oldsc;

					spinBox->setValue(ctr->minGapsBetweenBuildingChanges);

					break;
				}
			//18
			case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
				{
					ConstraintStudentsSetMinGapsBetweenBuildingChanges* ctr=(ConstraintStudentsSetMinGapsBetweenBuildingChanges*)oldsc;

					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->minGapsBetweenBuildingChanges);

					break;
				}
			//19
			case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY:
				{
					ConstraintTeachersMaxBuildingChangesPerDay* ctr=(ConstraintTeachersMaxBuildingChangesPerDay*)oldsc;

					spinBox->setValue(ctr->maxBuildingChangesPerDay);

					break;
				}
			//20
			case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY:
				{
					ConstraintTeacherMaxBuildingChangesPerDay* ctr=(ConstraintTeacherMaxBuildingChangesPerDay*)oldsc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->maxBuildingChangesPerDay);

					break;
				}
			//21
			case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK:
				{
					ConstraintTeachersMaxBuildingChangesPerWeek* ctr=(ConstraintTeachersMaxBuildingChangesPerWeek*)oldsc;

					spinBox->setValue(ctr->maxBuildingChangesPerWeek);

					break;
				}
			//22
			case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK:
				{
					ConstraintTeacherMaxBuildingChangesPerWeek* ctr=(ConstraintTeacherMaxBuildingChangesPerWeek*)oldsc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->maxBuildingChangesPerWeek);

					break;
				}
			//23
			case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
				{
					ConstraintTeachersMinGapsBetweenBuildingChanges* ctr=(ConstraintTeachersMinGapsBetweenBuildingChanges*)oldsc;

					spinBox->setValue(ctr->minGapsBetweenBuildingChanges);

					break;
				}
			//24
			case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
				{
					ConstraintTeacherMinGapsBetweenBuildingChanges* ctr=(ConstraintTeacherMinGapsBetweenBuildingChanges*)oldsc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->minGapsBetweenBuildingChanges);

					break;
				}
			//25
			case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM:
				{
					ConstraintActivityTagPreferredRoom* ctr=(ConstraintActivityTagPreferredRoom*)oldsc;

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));

					roomsComboBox->setCurrentIndex(roomsComboBox->findText(ctr->roomName));

					break;
				}
			//26
			case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS:
				{
					ConstraintActivityTagPreferredRooms* ctr=(ConstraintActivityTagPreferredRooms*)oldsc;

					activityTagsComboBox->setCurrentIndex(activityTagsComboBox->findText(ctr->activityTagName));

					for(const QString& rn : ctr->roomsNames)
						selectedRoomsListWidget->addItem(rn);

					break;
				}
			//27
			case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS:
				{
					ConstraintActivitiesOccupyMaxDifferentRooms* ctr=(ConstraintActivitiesOccupyMaxDifferentRooms*)oldsc;

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

					spinBox->setValue(ctr->maxDifferentRooms);

					break;
				}
			//28
			case CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE:
				{
					ConstraintActivitiesSameRoomIfConsecutive* ctr=(ConstraintActivitiesSameRoomIfConsecutive*)oldsc;

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

					break;
				}
			//29
			case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY:
				{
					ConstraintStudentsMaxRoomChangesPerDay* ctr=(ConstraintStudentsMaxRoomChangesPerDay*)oldsc;

					spinBox->setValue(ctr->maxRoomChangesPerDay);

					break;
				}
			//30
			case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY:
				{
					ConstraintStudentsSetMaxRoomChangesPerDay* ctr=(ConstraintStudentsSetMaxRoomChangesPerDay*)oldsc;

					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxRoomChangesPerDay);

					break;
				}
			//31
			case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_WEEK:
				{
					ConstraintStudentsMaxRoomChangesPerWeek* ctr=(ConstraintStudentsMaxRoomChangesPerWeek*)oldsc;

					spinBox->setValue(ctr->maxRoomChangesPerWeek);

					break;
				}
			//32
			case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK:
				{
					ConstraintStudentsSetMaxRoomChangesPerWeek* ctr=(ConstraintStudentsSetMaxRoomChangesPerWeek*)oldsc;

					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxRoomChangesPerWeek);

					break;
				}
			//33
			case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
				{
					ConstraintStudentsMinGapsBetweenRoomChanges* ctr=(ConstraintStudentsMinGapsBetweenRoomChanges*)oldsc;

					spinBox->setValue(ctr->minGapsBetweenRoomChanges);

					break;
				}
			//34
			case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES:
				{
					ConstraintStudentsSetMinGapsBetweenRoomChanges* ctr=(ConstraintStudentsSetMinGapsBetweenRoomChanges*)oldsc;

					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->minGapsBetweenRoomChanges);

					break;
				}
			//35
			case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY:
				{
					ConstraintTeachersMaxRoomChangesPerDay* ctr=(ConstraintTeachersMaxRoomChangesPerDay*)oldsc;

					spinBox->setValue(ctr->maxRoomChangesPerDay);

					break;
				}
			//36
			case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY:
				{
					ConstraintTeacherMaxRoomChangesPerDay* ctr=(ConstraintTeacherMaxRoomChangesPerDay*)oldsc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->maxRoomChangesPerDay);

					break;
				}
			//37
			case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK:
				{
					ConstraintTeachersMaxRoomChangesPerWeek* ctr=(ConstraintTeachersMaxRoomChangesPerWeek*)oldsc;

					spinBox->setValue(ctr->maxRoomChangesPerWeek);

					break;
				}
			//38
			case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK:
				{
					ConstraintTeacherMaxRoomChangesPerWeek* ctr=(ConstraintTeacherMaxRoomChangesPerWeek*)oldsc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->maxRoomChangesPerWeek);

					break;
				}
			//39
			case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
				{
					ConstraintTeachersMinGapsBetweenRoomChanges* ctr=(ConstraintTeachersMinGapsBetweenRoomChanges*)oldsc;

					spinBox->setValue(ctr->minGapsBetweenRoomChanges);

					break;
				}
			//40
			case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES:
				{
					ConstraintTeacherMinGapsBetweenRoomChanges* ctr=(ConstraintTeacherMinGapsBetweenRoomChanges*)oldsc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->minGapsBetweenRoomChanges);

					break;
				}
			//41
			case CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES:
				{
					ConstraintTeacherRoomNotAvailableTimes* ctr=(ConstraintTeacherRoomNotAvailableTimes*)oldsc;

					fillTimesTable(timesTable, ctr->days, ctr->hours, true);

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					roomsComboBox->setCurrentIndex(roomsComboBox->findText(ctr->room));

					break;
				}
			//42
			case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY:
				{
					ConstraintStudentsMaxRoomChangesPerRealDay* ctr=(ConstraintStudentsMaxRoomChangesPerRealDay*)oldsc;

					spinBox->setValue(ctr->maxRoomChangesPerDay);

					break;
				}
			//43
			case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY:
				{
					ConstraintStudentsSetMaxRoomChangesPerRealDay* ctr=(ConstraintStudentsSetMaxRoomChangesPerRealDay*)oldsc;

					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxRoomChangesPerDay);

					break;
				}
			//44
			case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY:
				{
					ConstraintTeachersMaxRoomChangesPerRealDay* ctr=(ConstraintTeachersMaxRoomChangesPerRealDay*)oldsc;

					spinBox->setValue(ctr->maxRoomChangesPerDay);

					break;
				}
			//45
			case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY:
				{
					ConstraintTeacherMaxRoomChangesPerRealDay* ctr=(ConstraintTeacherMaxRoomChangesPerRealDay*)oldsc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->maxRoomChangesPerDay);

					break;
				}
			//46
			case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
				{
					ConstraintStudentsMaxBuildingChangesPerRealDay* ctr=(ConstraintStudentsMaxBuildingChangesPerRealDay*)oldsc;

					spinBox->setValue(ctr->maxBuildingChangesPerDay);

					break;
				}
			//47
			case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY:
				{
					ConstraintStudentsSetMaxBuildingChangesPerRealDay* ctr=(ConstraintStudentsSetMaxBuildingChangesPerRealDay*)oldsc;

					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxBuildingChangesPerDay);

					break;
				}
			//48
			case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
				{
					ConstraintTeachersMaxBuildingChangesPerRealDay* ctr=(ConstraintTeachersMaxBuildingChangesPerRealDay*)oldsc;

					spinBox->setValue(ctr->maxBuildingChangesPerDay);

					break;
				}
			//49
			case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY:
				{
					ConstraintTeacherMaxBuildingChangesPerRealDay* ctr=(ConstraintTeacherMaxBuildingChangesPerRealDay*)oldsc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->maxBuildingChangesPerDay);

					break;
				}
			//50
			case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
				{
					ConstraintStudentsMaxBuildingChangesPerDayInInterval* ctr=(ConstraintStudentsMaxBuildingChangesPerDayInInterval*)oldsc;

					spinBox->setValue(ctr->maxBuildingChangesPerDay);

					intervalStartHourComboBox->setCurrentIndex(ctr->intervalStart);
					intervalEndHourComboBox->setCurrentIndex(ctr->intervalEnd);

					break;
				}
			//51
			case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
				{
					ConstraintStudentsSetMaxBuildingChangesPerDayInInterval* ctr=(ConstraintStudentsSetMaxBuildingChangesPerDayInInterval*)oldsc;

					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxBuildingChangesPerDay);

					intervalStartHourComboBox->setCurrentIndex(ctr->intervalStart);
					intervalEndHourComboBox->setCurrentIndex(ctr->intervalEnd);

					break;
				}
			//52
			case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
				{
					ConstraintTeachersMaxBuildingChangesPerDayInInterval* ctr=(ConstraintTeachersMaxBuildingChangesPerDayInInterval*)oldsc;

					spinBox->setValue(ctr->maxBuildingChangesPerDay);

					intervalStartHourComboBox->setCurrentIndex(ctr->intervalStart);
					intervalEndHourComboBox->setCurrentIndex(ctr->intervalEnd);

					break;
				}
			//53
			case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
				{
					ConstraintTeacherMaxBuildingChangesPerDayInInterval* ctr=(ConstraintTeacherMaxBuildingChangesPerDayInInterval*)oldsc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->maxBuildingChangesPerDay);

					intervalStartHourComboBox->setCurrentIndex(ctr->intervalStart);
					intervalEndHourComboBox->setCurrentIndex(ctr->intervalEnd);

					break;
				}
			//54
			case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
				{
					ConstraintStudentsMaxBuildingChangesPerRealDayInInterval* ctr=(ConstraintStudentsMaxBuildingChangesPerRealDayInInterval*)oldsc;

					spinBox->setValue(ctr->maxBuildingChangesPerDay);

					intervalStartHourComboBox->setCurrentIndex(ctr->intervalStart);
					intervalEndHourComboBox->setCurrentIndex(ctr->intervalEnd);

					break;
				}
			//55
			case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
				{
					ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval* ctr=(ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval*)oldsc;

					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxBuildingChangesPerDay);

					intervalStartHourComboBox->setCurrentIndex(ctr->intervalStart);
					intervalEndHourComboBox->setCurrentIndex(ctr->intervalEnd);

					break;
				}
			//56
			case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
				{
					ConstraintTeachersMaxBuildingChangesPerRealDayInInterval* ctr=(ConstraintTeachersMaxBuildingChangesPerRealDayInInterval*)oldsc;

					spinBox->setValue(ctr->maxBuildingChangesPerDay);

					intervalStartHourComboBox->setCurrentIndex(ctr->intervalStart);
					intervalEndHourComboBox->setCurrentIndex(ctr->intervalEnd);

					break;
				}
			//57
			case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
				{
					ConstraintTeacherMaxBuildingChangesPerRealDayInInterval* ctr=(ConstraintTeacherMaxBuildingChangesPerRealDayInInterval*)oldsc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->maxBuildingChangesPerDay);

					intervalStartHourComboBox->setCurrentIndex(ctr->intervalStart);
					intervalEndHourComboBox->setCurrentIndex(ctr->intervalEnd);

					break;
				}
			//58
			case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
				{
					ConstraintStudentsMaxRoomChangesPerDayInInterval* ctr=(ConstraintStudentsMaxRoomChangesPerDayInInterval*)oldsc;

					spinBox->setValue(ctr->maxRoomChangesPerDay);

					intervalStartHourComboBox->setCurrentIndex(ctr->intervalStart);
					intervalEndHourComboBox->setCurrentIndex(ctr->intervalEnd);

					break;
				}
			//59
			case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
				{
					ConstraintStudentsSetMaxRoomChangesPerDayInInterval* ctr=(ConstraintStudentsSetMaxRoomChangesPerDayInInterval*)oldsc;

					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxRoomChangesPerDay);

					intervalStartHourComboBox->setCurrentIndex(ctr->intervalStart);
					intervalEndHourComboBox->setCurrentIndex(ctr->intervalEnd);

					break;
				}
			//60
			case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
				{
					ConstraintTeachersMaxRoomChangesPerDayInInterval* ctr=(ConstraintTeachersMaxRoomChangesPerDayInInterval*)oldsc;

					spinBox->setValue(ctr->maxRoomChangesPerDay);

					intervalStartHourComboBox->setCurrentIndex(ctr->intervalStart);
					intervalEndHourComboBox->setCurrentIndex(ctr->intervalEnd);

					break;
				}
			//61
			case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
				{
					ConstraintTeacherMaxRoomChangesPerDayInInterval* ctr=(ConstraintTeacherMaxRoomChangesPerDayInInterval*)oldsc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->maxRoomChangesPerDay);

					intervalStartHourComboBox->setCurrentIndex(ctr->intervalStart);
					intervalEndHourComboBox->setCurrentIndex(ctr->intervalEnd);

					break;
				}
			//62
			case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
				{
					ConstraintStudentsMaxRoomChangesPerRealDayInInterval* ctr=(ConstraintStudentsMaxRoomChangesPerRealDayInInterval*)oldsc;

					spinBox->setValue(ctr->maxRoomChangesPerDay);

					intervalStartHourComboBox->setCurrentIndex(ctr->intervalStart);
					intervalEndHourComboBox->setCurrentIndex(ctr->intervalEnd);

					break;
				}
			//63
			case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
				{
					ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval* ctr=(ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval*)oldsc;

					int j=studentsComboBox->findText(ctr->studentsName);
					if(j<0)
						showWarningForInvisibleSubgroupConstraint(parent, ctr->studentsName);
					else
						assert(j>=0);
					studentsComboBox->setCurrentIndex(j);

					spinBox->setValue(ctr->maxRoomChangesPerDay);

					intervalStartHourComboBox->setCurrentIndex(ctr->intervalStart);
					intervalEndHourComboBox->setCurrentIndex(ctr->intervalEnd);

					break;
				}
			//64
			case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
				{
					ConstraintTeachersMaxRoomChangesPerRealDayInInterval* ctr=(ConstraintTeachersMaxRoomChangesPerRealDayInInterval*)oldsc;

					spinBox->setValue(ctr->maxRoomChangesPerDay);

					intervalStartHourComboBox->setCurrentIndex(ctr->intervalStart);
					intervalEndHourComboBox->setCurrentIndex(ctr->intervalEnd);

					break;
				}
			//65
			case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
				{
					ConstraintTeacherMaxRoomChangesPerRealDayInInterval* ctr=(ConstraintTeacherMaxRoomChangesPerRealDayInInterval*)oldsc;

					teachersComboBox->setCurrentIndex(teachersComboBox->findText(ctr->teacherName));

					spinBox->setValue(ctr->maxRoomChangesPerDay);

					intervalStartHourComboBox->setCurrentIndex(ctr->intervalStart);
					intervalEndHourComboBox->setCurrentIndex(ctr->intervalEnd);

					break;
				}
			//66
			case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
				{
					ConstraintRoomMaxActivityTagsPerDayFromSet* ctr=(ConstraintRoomMaxActivityTagsPerDayFromSet*)oldsc;

					roomsComboBox->setCurrentIndex(roomsComboBox->findText(ctr->room));

					selectedActivityTagsListWidget->clear();
					selectedActivityTagsSet.clear();

					for(const QString& at : std::as_const(ctr->tagsList)){
						selectedActivityTagsSet.insert(at);
						selectedActivityTagsListWidget->addItem(at);
					}

					spinBox->setValue(ctr->maxTags);

					break;
				}
			//67
			case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
				{
					ConstraintRoomMaxActivityTagsPerRealDayFromSet* ctr=(ConstraintRoomMaxActivityTagsPerRealDayFromSet*)oldsc;

					roomsComboBox->setCurrentIndex(roomsComboBox->findText(ctr->room));

					selectedActivityTagsListWidget->clear();
					selectedActivityTagsSet.clear();

					for(const QString& at : std::as_const(ctr->tagsList)){
						selectedActivityTagsSet.insert(at);
						selectedActivityTagsListWidget->addItem(at);
					}

					spinBox->setValue(ctr->maxTags);

					break;
				}
			//68
			case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_WEEK_FROM_SET:
				{
					ConstraintRoomMaxActivityTagsPerWeekFromSet* ctr=(ConstraintRoomMaxActivityTagsPerWeekFromSet*)oldsc;

					roomsComboBox->setCurrentIndex(roomsComboBox->findText(ctr->room));

					selectedActivityTagsListWidget->clear();
					selectedActivityTagsSet.clear();

					for(const QString& at : std::as_const(ctr->tagsList)){
						selectedActivityTagsSet.insert(at);
						selectedActivityTagsListWidget->addItem(at);
					}

					spinBox->setValue(ctr->maxTags);

					break;
				}

			default:
				assert(0);
				break;
		}
	}

	if(filterGroupBox!=nullptr && activitiesListWidget!=nullptr){
		assert(activitiesComboBox==nullptr);

		filterActivitiesListWidget();
	}

	if(filterGroupBox!=nullptr && activitiesComboBox!=nullptr){
		assert(activitiesListWidget==nullptr);

		int i=filterActivitiesComboBox();
		if(i>=0){
			assert(activitiesComboBox!=nullptr);
			assert(i<activitiesList.count());
			assert(i<activitiesComboBox->count());
			activitiesComboBox->setCurrentIndex(i);
		}
	}

	if(checkBox!=nullptr)
		connect(checkBox, &QCheckBox::toggled, this, &AddOrModifySpaceConstraint::checkBoxToggled);

	if(activityTagsListWidget!=nullptr){
		assert(activityTagsLabel!=nullptr);
		assert(selectedActivityTagsLabel!=nullptr);
		assert(selectedActivityTagsListWidget!=nullptr);
		//assert(addAllActivityTagsPushButton!=nullptr);
		assert(clearActivityTagsPushButton!=nullptr);

		assert(filterActivityTagsCheckBox!=nullptr);

		assert(roomsComboBox!=nullptr);

		connect(roomsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &AddOrModifySpaceConstraint::filterActivityTagsCheckBoxToggled);
		connect(filterActivityTagsCheckBox, &QCheckBox::toggled, this, &AddOrModifySpaceConstraint::filterActivityTagsCheckBoxToggled);
		filterActivityTagsCheckBoxToggled();
	}

	dialog->setModal(true);
	dialog->setWindowModality(Qt::ApplicationModal);
	dialog->show();

	eventLoop->exec();
}

AddOrModifySpaceConstraint::~AddOrModifySpaceConstraint()
{
	//saveFETDialogGeometry(dialog, dialogName);

	//dialog->hide();

	//delete dialog;

	assert(!eventLoop->isRunning());
	delete eventLoop;
}

void AddOrModifySpaceConstraint::addConstraintClicked()
{
	SpaceConstraint* sc=nullptr;

	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(dialog, tr("FET information"), tr("Invalid weight (percentage)"));
		return;
	}

	if(type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM)
		assert(selectedRealRoomsListWidget!=nullptr);
	else
		assert(selectedRealRoomsListWidget==nullptr);
	if(selectedRealRoomsListWidget!=nullptr){
		if(selectedRealRoomsListWidget->count()>0 && weight<100.0){
			QMessageBox::warning(dialog, tr("FET information"), tr("If the preferred room is virtual and the list of real rooms is not empty,"
			 " the weight percentage must be exactly 100%."));
			return;
		}
	}
	else if(!spaceConstraintCanHaveAnyWeight(type) && weight!=100.0){
		QMessageBox::warning(dialog, tr("FET information"), tr("Invalid weight (percentage) - it has to be 100%"));
		return;
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

	if(!addEmpty && subjectsComboBox!=nullptr){
		assert(filterGroupBox==nullptr);
		QString subject_name=subjectsComboBox->currentText();
		int subject_ID=gt.rules.searchSubject(subject_name);
		if(subject_ID<0){
			QMessageBox::warning(dialog, tr("FET information"), tr("Invalid subject"));
			return;
		}
	}

	if(!addEmpty && activityTagsComboBox!=nullptr){
		assert(filterGroupBox==nullptr);
		QString activityTag_name=activityTagsComboBox->currentText();
		int activityTag_ID=gt.rules.searchActivityTag(activityTag_name);
		if(activityTag_ID<0){
			QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity tag"));
			return;
		}
	}

	if(/*!addEmpty &&*/ roomsComboBox!=nullptr){
		//assert(filterGroupBox==nullptr);
		QString room_name=roomsComboBox->currentText();
		int room_ID=gt.rules.searchRoom(room_name);
		if(room_ID<0){
			QMessageBox::warning(dialog, tr("FET information"), tr("Invalid room"));
			return;
		}
	}

	switch(type){
		//1
		case CONSTRAINT_BASIC_COMPULSORY_SPACE:
			{
				sc=new ConstraintBasicCompulsorySpace(weight);

				break;
			}
		//2
		case CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES:
			{
				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				sc=new ConstraintRoomNotAvailableTimes(weight, roomsComboBox->currentText(), days, hours);

				break;
			}
		//3
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOM:
			{
				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				QString room=roomsComboBox->currentText();

				int ri=gt.rules.searchRoom(room);
				assert(ri>=0);
				Room* rm=gt.rules.roomsList.at(ri);

				if(rm->isVirtual==false){
					if(selectedRealRoomsListWidget->count()>0){
						QMessageBox::warning(dialog, tr("FET information"), tr("The preferred room of the activity is a real room, not a virtual one."
						 " This implies that the selected real rooms list should be empty."));
						return;
					}
				}
				else{
					if(selectedRealRoomsListWidget->count()>0 && weight<100.0){
						assert(0); //checked the weight above
						//QMessageBox::warning(dialog, tr("FET information"), tr("If the preferred room is virtual and the list of real rooms is not empty,"
						// " the weight percentage must be exactly 100%."));
						return;
					}

					if(selectedRealRoomsListWidget->count()>0 && rm->realRoomsSetsList.count()!=selectedRealRoomsListWidget->count()){
						QMessageBox::warning(dialog, tr("FET information"), tr("The preferred room of the activity is a virtual room."
						 " This implies that the number of selected real rooms in the list should either be zero or equal to the"
						 " number of sets of real rooms of the preferred virtual room, which is %1.").arg(rm->realRoomsSetsList.count()));
						return;
					}

					QSet<QString> rrs;
					for(const QStringList& tl : std::as_const(rm->realRoomsSetsList))
						for(const QString& s : std::as_const(tl))
							if(!rrs.contains(s))
								rrs.insert(s);

					QStringList incorrectList;
					for(int i=0; i<selectedRealRoomsListWidget->count(); i++){
						bool found=false;
						QString rrn=selectedRealRoomsListWidget->item(i)->text();
						if(rrs.contains(rrn))
							found=true;

						if(!found)
							incorrectList.append(rrn);
					}
					if(!incorrectList.isEmpty()){
						switch(LongTextMessageBox::confirmation(dialog, tr("FET information"), tr("The selected real rooms: %1 are not found in the sets of sets of real rooms of the"
						 " selected preferred virtual room. This is probably wrong. Are you sure you want to add this constraint?").arg(incorrectList.join(translatedCommaSpace())),
						 tr("Yes"), tr("No"), QString(), 0, 1)){
						case 0:
							break;
						case 1:
							return;
						}
					}
				}

				QStringList lst;
				for(int i=0; i<selectedRealRoomsListWidget->count(); i++)
					lst.append(selectedRealRoomsListWidget->item(i)->text());

				sc=new ConstraintActivityPreferredRoom(weight, id, room, lst, permanentlyLockedCheckBox->isChecked());

				break;
			}
		//4
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOMS:
			{
				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				assert(selectedRoomsListWidget!=nullptr);
				if(selectedRoomsListWidget->count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected rooms"));
					return;
				}

				QStringList roomsList;
				for(int i=0; i<selectedRoomsListWidget->count(); i++)
					roomsList.append(selectedRoomsListWidget->item(i)->text());

				sc=new ConstraintActivityPreferredRooms(weight, id, roomsList);

				break;
			}
		//5
		case CONSTRAINT_STUDENTS_SET_HOME_ROOM:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString room=roomsComboBox->currentText();
				assert(gt.rules.searchRoom(room)>=0);

				sc=new ConstraintStudentsSetHomeRoom(weight, students, room);

				break;
			}
		//6
		case CONSTRAINT_STUDENTS_SET_HOME_ROOMS:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				assert(selectedRoomsListWidget!=nullptr);
				if(selectedRoomsListWidget->count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected rooms"));
					return;
				}

				QStringList roomsList;
				for(int i=0; i<selectedRoomsListWidget->count(); i++)
					roomsList.append(selectedRoomsListWidget->item(i)->text());

				sc=new ConstraintStudentsSetHomeRooms(weight, students, roomsList);

				break;
			}
		//7
		case CONSTRAINT_TEACHER_HOME_ROOM:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				QString room=roomsComboBox->currentText();
				assert(gt.rules.searchRoom(room)>=0);

				sc=new ConstraintTeacherHomeRoom(weight, teacher, room);

				break;
			}
		//8
		case CONSTRAINT_TEACHER_HOME_ROOMS:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				assert(selectedRoomsListWidget!=nullptr);
				if(selectedRoomsListWidget->count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected rooms"));
					return;
				}

				QStringList roomsList;
				for(int i=0; i<selectedRoomsListWidget->count(); i++)
					roomsList.append(selectedRoomsListWidget->item(i)->text());

				sc=new ConstraintTeacherHomeRooms(weight, teacher, roomsList);

				break;
			}
		//9
		case CONSTRAINT_SUBJECT_PREFERRED_ROOM:
			{
				QString subject=subjectsComboBox->currentText();
				assert(gt.rules.searchSubject(subject)>=0);

				QString room=roomsComboBox->currentText();
				assert(gt.rules.searchRoom(room)>=0);

				sc=new ConstraintSubjectPreferredRoom(weight, subject, room);

				break;
			}
		//10
		case CONSTRAINT_SUBJECT_PREFERRED_ROOMS:
			{
				QString subject=subjectsComboBox->currentText();
				assert(gt.rules.searchSubject(subject)>=0);

				assert(selectedRoomsListWidget!=nullptr);
				if(selectedRoomsListWidget->count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected rooms"));
					return;
				}

				QStringList roomsList;
				for(int i=0; i<selectedRoomsListWidget->count(); i++)
					roomsList.append(selectedRoomsListWidget->item(i)->text());

				sc=new ConstraintSubjectPreferredRooms(weight, subject, roomsList);

				break;
			}
		//11
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM:
			{
				QString subject=subjectsComboBox->currentText();
				assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				assert(gt.rules.searchActivityTag(activityTag)>=0);

				QString room=roomsComboBox->currentText();
				assert(gt.rules.searchRoom(room)>=0);

				sc=new ConstraintSubjectActivityTagPreferredRoom(weight, subject, activityTag, room);

				break;
			}
		//12
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS:
			{
				QString subject=subjectsComboBox->currentText();
				assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				assert(gt.rules.searchActivityTag(activityTag)>=0);

				assert(selectedRoomsListWidget!=nullptr);
				if(selectedRoomsListWidget->count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected rooms"));
					return;
				}

				QStringList roomsList;
				for(int i=0; i<selectedRoomsListWidget->count(); i++)
					roomsList.append(selectedRoomsListWidget->item(i)->text());

				sc=new ConstraintSubjectActivityTagPreferredRooms(weight, subject, activityTag, roomsList);

				break;
			}
		//13
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY:
			{
				sc=new ConstraintStudentsMaxBuildingChangesPerDay(weight, spinBox->value());

				break;
			}
		//14
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				sc=new ConstraintStudentsSetMaxBuildingChangesPerDay(weight, students, spinBox->value());

				break;
			}
		//15
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				sc=new ConstraintStudentsMaxBuildingChangesPerWeek(weight, spinBox->value());

				break;
			}
		//16
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				sc=new ConstraintStudentsSetMaxBuildingChangesPerWeek(weight, students, spinBox->value());

				break;
			}
		//17
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				sc=new ConstraintStudentsMinGapsBetweenBuildingChanges(weight, spinBox->value());

				break;
			}
		//18
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				sc=new ConstraintStudentsSetMinGapsBetweenBuildingChanges(weight, students, spinBox->value());

				break;
			}
		//19
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY:
			{
				sc=new ConstraintTeachersMaxBuildingChangesPerDay(weight, spinBox->value());

				break;
			}
		//20
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				sc=new ConstraintTeacherMaxBuildingChangesPerDay(weight, teacher, spinBox->value());

				break;
			}
		//21
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				sc=new ConstraintTeachersMaxBuildingChangesPerWeek(weight, spinBox->value());

				break;
			}
		//22
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				sc=new ConstraintTeacherMaxBuildingChangesPerWeek(weight, teacher, spinBox->value());

				break;
			}
		//23
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				sc=new ConstraintTeachersMinGapsBetweenBuildingChanges(weight, spinBox->value());

				break;
			}
		//24
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				sc=new ConstraintTeacherMinGapsBetweenBuildingChanges(weight, teacher, spinBox->value());

				break;
			}
		//25
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM:
			{
				QString activityTag=activityTagsComboBox->currentText();
				assert(gt.rules.searchActivityTag(activityTag)>=0);

				QString room=roomsComboBox->currentText();
				assert(gt.rules.searchRoom(room)>=0);

				sc=new ConstraintActivityTagPreferredRoom(weight, activityTag, room);

				break;
			}
		//26
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS:
			{
				QString activityTag=activityTagsComboBox->currentText();
				assert(gt.rules.searchActivityTag(activityTag)>=0);

				assert(selectedRoomsListWidget!=nullptr);
				if(selectedRoomsListWidget->count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected rooms"));
					return;
				}

				QStringList roomsList;
				for(int i=0; i<selectedRoomsListWidget->count(); i++)
					roomsList.append(selectedRoomsListWidget->item(i)->text());

				sc=new ConstraintActivityTagPreferredRooms(weight, activityTag, roomsList);

				break;
			}
		//27
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS:
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

				sc=new ConstraintActivitiesOccupyMaxDifferentRooms(weight, selectedActivitiesList, spinBox->value());

				break;
			}
		//28
		case CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE:
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

				sc=new ConstraintActivitiesSameRoomIfConsecutive(weight, selectedActivitiesList);

				break;
			}
		//29
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY:
			{
				sc=new ConstraintStudentsMaxRoomChangesPerDay(weight, spinBox->value());

				break;
			}
		//30
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				sc=new ConstraintStudentsSetMaxRoomChangesPerDay(weight, students, spinBox->value());

				break;
			}
		//31
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_WEEK:
			{
				sc=new ConstraintStudentsMaxRoomChangesPerWeek(weight, spinBox->value());

				break;
			}
		//32
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				sc=new ConstraintStudentsSetMaxRoomChangesPerWeek(weight, students, spinBox->value());

				break;
			}
		//33
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				sc=new ConstraintStudentsMinGapsBetweenRoomChanges(weight, spinBox->value());

				break;
			}
		//34
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				sc=new ConstraintStudentsSetMinGapsBetweenRoomChanges(weight, students, spinBox->value());

				break;
			}
		//35
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY:
			{
				sc=new ConstraintTeachersMaxRoomChangesPerDay(weight, spinBox->value());

				break;
			}
		//36
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				sc=new ConstraintTeacherMaxRoomChangesPerDay(weight, teacher, spinBox->value());

				break;
			}
		//37
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK:
			{
				sc=new ConstraintTeachersMaxRoomChangesPerWeek(weight, spinBox->value());

				break;
			}
		//38
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				sc=new ConstraintTeacherMaxRoomChangesPerWeek(weight, teacher, spinBox->value());

				break;
			}
		//39
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				sc=new ConstraintTeachersMinGapsBetweenRoomChanges(weight, spinBox->value());

				break;
			}
		//40
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				sc=new ConstraintTeacherMinGapsBetweenRoomChanges(weight, teacher, spinBox->value());

				break;
			}
		//41
		case CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES:
			{
				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				sc=new ConstraintTeacherRoomNotAvailableTimes(weight, teachersComboBox->currentText(), roomsComboBox->currentText(), days, hours);

				break;
			}
		//42
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				sc=new ConstraintStudentsMaxRoomChangesPerRealDay(weight, spinBox->value());

				break;
			}
		//43
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				sc=new ConstraintStudentsSetMaxRoomChangesPerRealDay(weight, students, spinBox->value());

				break;
			}
		//44
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				sc=new ConstraintTeachersMaxRoomChangesPerRealDay(weight, spinBox->value());

				break;
			}
		//45
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				sc=new ConstraintTeacherMaxRoomChangesPerRealDay(weight, teacher, spinBox->value());

				break;
			}
		//46
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				sc=new ConstraintStudentsMaxBuildingChangesPerRealDay(weight, spinBox->value());

				break;
			}
		//47
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				sc=new ConstraintStudentsSetMaxBuildingChangesPerRealDay(weight, students, spinBox->value());

				break;
			}
		//48
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				sc=new ConstraintTeachersMaxBuildingChangesPerRealDay(weight, spinBox->value());

				break;
			}
		//49
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				sc=new ConstraintTeacherMaxBuildingChangesPerRealDay(weight, teacher, spinBox->value());

				break;
			}
		//50
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				sc=new ConstraintStudentsMaxBuildingChangesPerDayInInterval(weight, spinBox->value(), startHour, endHour);

				break;
			}
		//51
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				sc=new ConstraintStudentsSetMaxBuildingChangesPerDayInInterval(weight, students, spinBox->value(), startHour, endHour);

				break;
			}
		//52
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				sc=new ConstraintTeachersMaxBuildingChangesPerDayInInterval(weight, spinBox->value(), startHour, endHour);

				break;
			}
		//53
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				sc=new ConstraintTeacherMaxBuildingChangesPerDayInInterval(weight, teacher, spinBox->value(), startHour, endHour);

				break;
			}
		//54
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				sc=new ConstraintStudentsMaxBuildingChangesPerRealDayInInterval(weight, spinBox->value(), startHour, endHour);

				break;
			}
		//55
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				sc=new ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval(weight, students, spinBox->value(), startHour, endHour);

				break;
			}
		//56
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				sc=new ConstraintTeachersMaxBuildingChangesPerRealDayInInterval(weight, spinBox->value(), startHour, endHour);

				break;
			}
		//57
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				sc=new ConstraintTeacherMaxBuildingChangesPerRealDayInInterval(weight, teacher, spinBox->value(), startHour, endHour);

				break;
			}
		//58
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				sc=new ConstraintStudentsMaxRoomChangesPerDayInInterval(weight, spinBox->value(), startHour, endHour);

				break;
			}
		//59
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				sc=new ConstraintStudentsSetMaxRoomChangesPerDayInInterval(weight, students, spinBox->value(), startHour, endHour);

				break;
			}
		//60
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				sc=new ConstraintTeachersMaxRoomChangesPerDayInInterval(weight, spinBox->value(), startHour, endHour);

				break;
			}
		//61
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				sc=new ConstraintTeacherMaxRoomChangesPerDayInInterval(weight, teacher, spinBox->value(), startHour, endHour);

				break;
			}
		//62
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				sc=new ConstraintStudentsMaxRoomChangesPerRealDayInInterval(weight, spinBox->value(), startHour, endHour);

				break;
			}
		//63
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				sc=new ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval(weight, students, spinBox->value(), startHour, endHour);

				break;
			}
		//64
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				sc=new ConstraintTeachersMaxRoomChangesPerRealDayInInterval(weight, spinBox->value(), startHour, endHour);

				break;
			}
		//65
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				sc=new ConstraintTeacherMaxRoomChangesPerRealDayInInterval(weight, teacher, spinBox->value(), startHour, endHour);

				break;
			}
		//66
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
										 tr("Please select at least two activity tags"));
					return;
				}

				QStringList selectedActivityTagsList;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					selectedActivityTagsList.append(selectedActivityTagsListWidget->item(i)->text());

				sc=new ConstraintRoomMaxActivityTagsPerDayFromSet(weight, roomsComboBox->currentText(), spinBox->value(), selectedActivityTagsList);

				break;
			}
		//67
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
										 tr("Please select at least two activity tags"));
					return;
				}

				QStringList selectedActivityTagsList;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					selectedActivityTagsList.append(selectedActivityTagsListWidget->item(i)->text());

				sc=new ConstraintRoomMaxActivityTagsPerRealDayFromSet(weight, roomsComboBox->currentText(), spinBox->value(), selectedActivityTagsList);

				break;
			}
		//68
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_WEEK_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
										 tr("Please select at least two activity tags"));
					return;
				}

				QStringList selectedActivityTagsList;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					selectedActivityTagsList.append(selectedActivityTagsListWidget->item(i)->text());

				sc=new ConstraintRoomMaxActivityTagsPerWeekFromSet(weight, roomsComboBox->currentText(), spinBox->value(), selectedActivityTagsList);

				break;
			}

		default:
			assert(0);
			break;
	}

	bool tmp2=gt.rules.addSpaceConstraint(sc);
	if(tmp2){
		LongTextMessageBox::information(dialog, tr("FET information"), tr("Constraint added:")+"\n\n"+sc->getDetailedDescription(gt.rules));

		gt.rules.addUndoPoint(tr("Added the constraint:\n\n%1").arg(sc->getDetailedDescription(gt.rules)));

		if(sc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
			LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
			LockUnlock::increaseCommunicationSpinBox();
		}
	}
	else{
		if(sc->type==CONSTRAINT_BASIC_COMPULSORY_SPACE)
			QMessageBox::warning(dialog, tr("FET information"), tr("Constraint NOT added - there must be another constraint of this type. Please edit that one"));
		else if(sc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM)
			QMessageBox::warning(dialog, tr("FET information"),	tr("Constraint NOT added - duplicate",
																   "'Duplicate' means that the constraint is identical to an already existing constraint and cannot be added."));
		else
			QMessageBox::warning(dialog, tr("FET information"),	tr("Constraint NOT added - please report error"));

		delete sc;
	}
}

void AddOrModifySpaceConstraint::addConstraintsClicked()
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

	assert(type!=CONSTRAINT_ACTIVITY_PREFERRED_ROOM);
	if(!spaceConstraintCanHaveAnyWeight(type) && weight!=100.0){
		QMessageBox::warning(dialog, tr("FET information"), tr("Invalid weight (percentage) - it has to be 100%"));
		return;
	}

	if(filterGroupBox==nullptr && teachersComboBox!=nullptr){
		QString teacher_name=teachersComboBox->currentText();
		int teacher_ID=gt.rules.searchTeacher(teacher_name);
		if(teacher_ID<0){
			QMessageBox::warning(dialog, tr("FET information"), tr("Invalid teacher"));
			return;
		}
	}

	QString ctrs;

	switch(type){
		//19
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY:
			[[fallthrough]];
		//20
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					SpaceConstraint* ctr=new ConstraintTeacherMaxBuildingChangesPerDay(weight, tch->name, spinBox->value());
					bool tmp2=gt.rules.addSpaceConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//21
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK:
			[[fallthrough]];
		//22
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					SpaceConstraint* ctr=new ConstraintTeacherMaxBuildingChangesPerWeek(weight, tch->name, spinBox->value());
					bool tmp2=gt.rules.addSpaceConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//23
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			[[fallthrough]];
		//24
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					SpaceConstraint* ctr=new ConstraintTeacherMinGapsBetweenBuildingChanges(weight, tch->name, spinBox->value());
					bool tmp2=gt.rules.addSpaceConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//35
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY:
			[[fallthrough]];
		//36
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					SpaceConstraint* ctr=new ConstraintTeacherMaxRoomChangesPerDay(weight, tch->name, spinBox->value());
					bool tmp2=gt.rules.addSpaceConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//37
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK:
			[[fallthrough]];
		//38
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					SpaceConstraint* ctr=new ConstraintTeacherMaxRoomChangesPerWeek(weight, tch->name, spinBox->value());
					bool tmp2=gt.rules.addSpaceConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//39
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			[[fallthrough]];
		//40
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					SpaceConstraint* ctr=new ConstraintTeacherMinGapsBetweenRoomChanges(weight, tch->name, spinBox->value());
					bool tmp2=gt.rules.addSpaceConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//44
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY:
			[[fallthrough]];
		//45
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					SpaceConstraint* ctr=new ConstraintTeacherMaxRoomChangesPerRealDay(weight, tch->name, spinBox->value());
					bool tmp2=gt.rules.addSpaceConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//48
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			[[fallthrough]];
		//49
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					SpaceConstraint* ctr=new ConstraintTeacherMaxBuildingChangesPerRealDay(weight, tch->name, spinBox->value());
					bool tmp2=gt.rules.addSpaceConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//52
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		//53
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					SpaceConstraint* ctr=new ConstraintTeacherMaxBuildingChangesPerDayInInterval(weight, tch->name, spinBox->value(), startHour, endHour);
					bool tmp2=gt.rules.addSpaceConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//56
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			[[fallthrough]];
		//57
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					SpaceConstraint* ctr=new ConstraintTeacherMaxBuildingChangesPerRealDayInInterval(weight, tch->name, spinBox->value(), startHour, endHour);
					bool tmp2=gt.rules.addSpaceConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//60
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			[[fallthrough]];
		//61
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					SpaceConstraint* ctr=new ConstraintTeacherMaxRoomChangesPerDayInInterval(weight, tch->name, spinBox->value(), startHour, endHour);
					bool tmp2=gt.rules.addSpaceConstraint(ctr);
					assert(tmp2);

					ctrs+=ctr->getDetailedDescription(gt.rules);
					ctrs+=QString("\n");
				}

				break;
			}
		//64
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			[[fallthrough]];
		//65
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				for(Teacher* tch : std::as_const(gt.rules.teachersList)){
					SpaceConstraint* ctr=new ConstraintTeacherMaxRoomChangesPerRealDayInInterval(weight, tch->name, spinBox->value(), startHour, endHour);
					bool tmp2=gt.rules.addSpaceConstraint(ctr);
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
		QMessageBox::information(dialog, tr("FET information"), tr("Added %1 space constraints. Please note that these constraints"
		 " will be visible as constraints for individual teachers.").arg(gt.rules.teachersList.count()));
	else
		QMessageBox::information(dialog, tr("FET information"), tr("Added %1 space constraints.").arg(gt.rules.teachersList.count()));

	if(gt.rules.teachersList.count()>0)
		gt.rules.addUndoPoint(tr("Added %1 constraints, one for each teacher:\n\n%2", "%1 is the number of constraints, %2 is their detailed description")
		 .arg(gt.rules.teachersList.count()).arg(ctrs));
}

void AddOrModifySpaceConstraint::closeClicked()
{
	dialog->close();
}

void AddOrModifySpaceConstraint::okClicked()
{
	double weight;
	QString tmp=weightLineEdit->text();
	weight_sscanf(tmp, "%lf", &weight);
	if(weight<0.0 || weight>100.0){
		QMessageBox::warning(dialog, tr("FET information"), tr("Invalid weight (percentage)"));
		return;
	}

	if(type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM)
		assert(selectedRealRoomsListWidget!=nullptr);
	else
		assert(selectedRealRoomsListWidget==nullptr);
	if(selectedRealRoomsListWidget!=nullptr){
		if(selectedRealRoomsListWidget->count()>0 && weight<100.0){
			QMessageBox::warning(dialog, tr("FET information"), tr("If the preferred room is virtual and the list of real rooms is not empty,"
			 " the weight percentage must be exactly 100%."));
			return;
		}
	}
	else if(!spaceConstraintCanHaveAnyWeight(type) && weight!=100.0){
		QMessageBox::warning(dialog, tr("FET information"), tr("Invalid weight (percentage) - it has to be 100%"));
		return;
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

	if(!addEmpty && subjectsComboBox!=nullptr){
		assert(filterGroupBox==nullptr);
		QString subject_name=subjectsComboBox->currentText();
		int subject_ID=gt.rules.searchSubject(subject_name);
		if(subject_ID<0){
			QMessageBox::warning(dialog, tr("FET information"), tr("Invalid subject"));
			return;
		}
	}

	if(!addEmpty && activityTagsComboBox!=nullptr){
		assert(filterGroupBox==nullptr);
		QString activityTag_name=activityTagsComboBox->currentText();
		int activityTag_ID=gt.rules.searchActivityTag(activityTag_name);
		if(activityTag_ID<0){
			QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity tag"));
			return;
		}
	}

	if(/*!addEmpty &&*/ roomsComboBox!=nullptr){
		//assert(filterGroupBox==nullptr);
		QString room_name=roomsComboBox->currentText();
		int room_ID=gt.rules.searchRoom(room_name);
		if(room_ID<0){
			QMessageBox::warning(dialog, tr("FET information"), tr("Invalid room"));
			return;
		}
	}

	QString oldcs=oldsc->getDetailedDescription(gt.rules);

	switch(type){
		//1
		case CONSTRAINT_BASIC_COMPULSORY_SPACE:
			{
				break;
			}
		//2
		case CONSTRAINT_ROOM_NOT_AVAILABLE_TIMES:
			{
				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				ConstraintRoomNotAvailableTimes* ctr=(ConstraintRoomNotAvailableTimes*)oldsc;

				ctr->room=roomsComboBox->currentText();

				ctr->days=days;
				ctr->hours=hours;

				break;
			}
		//3
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOM:
			{
				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				QString room=roomsComboBox->currentText();

				int ri=gt.rules.searchRoom(room);
				assert(ri>=0);
				Room* rm=gt.rules.roomsList.at(ri);

				if(rm->isVirtual==false){
					if(selectedRealRoomsListWidget->count()>0){
						QMessageBox::warning(dialog, tr("FET information"), tr("The preferred room of the activity is a real room, not a virtual one."
						 " This implies that the selected real rooms list should be empty."));
						return;
					}
				}
				else{
					if(selectedRealRoomsListWidget->count()>0 && weight<100.0){
						assert(0); //checked the weight above
						//QMessageBox::warning(dialog, tr("FET information"), tr("If the preferred room is virtual and the list of real rooms is not empty,"
						// " the weight percentage must be exactly 100%."));
						return;
					}

					if(selectedRealRoomsListWidget->count()>0 && rm->realRoomsSetsList.count()!=selectedRealRoomsListWidget->count()){
						QMessageBox::warning(dialog, tr("FET information"), tr("The preferred room of the activity is a virtual room."
						 " This implies that the number of selected real rooms in the list should either be zero or equal to the"
						 " number of sets of real rooms of the preferred virtual room, which is %1.").arg(rm->realRoomsSetsList.count()));
						return;
					}

					QSet<QString> rrs;
					for(const QStringList& tl : std::as_const(rm->realRoomsSetsList))
						for(const QString& s : std::as_const(tl))
							if(!rrs.contains(s))
								rrs.insert(s);

					QStringList incorrectList;
					for(int i=0; i<selectedRealRoomsListWidget->count(); i++){
						bool found=false;
						QString rrn=selectedRealRoomsListWidget->item(i)->text();
						if(rrs.contains(rrn))
							found=true;

						if(!found)
							incorrectList.append(rrn);
					}
					if(!incorrectList.isEmpty()){
						switch(LongTextMessageBox::confirmation(dialog, tr("FET information"), tr("The selected real rooms: %1 are not found in the sets of sets of real rooms of the"
						 " selected preferred virtual room. This is probably wrong. Are you sure you want to add this constraint?").arg(incorrectList.join(translatedCommaSpace())),
						 tr("Yes"), tr("No"), QString(), 0, 1)){
						case 0:
							break;
						case 1:
							return;
						}
					}
				}

				QStringList lst;
				for(int i=0; i<selectedRealRoomsListWidget->count(); i++)
					lst.append(selectedRealRoomsListWidget->item(i)->text());

				ConstraintActivityPreferredRoom* ctr=(ConstraintActivityPreferredRoom*)oldsc;

				if(ctr->activityId!=id){
					int oldId=ctr->activityId;
					int newId=id;

					QSet<ConstraintActivityPreferredRoom*> cs=gt.rules.aprHash.value(oldId, QSet<ConstraintActivityPreferredRoom*>());
					assert(cs.contains(ctr));
					cs.remove(ctr);
					gt.rules.aprHash.insert(oldId, cs);

					cs=gt.rules.aprHash.value(newId, QSet<ConstraintActivityPreferredRoom*>());
					assert(!cs.contains(ctr));
					cs.insert(ctr);
					gt.rules.aprHash.insert(newId, cs);

					ctr->activityId=id;
				}

				ctr->roomName=room;
				ctr->preferredRealRoomsNames=lst;
				ctr->permanentlyLocked=permanentlyLockedCheckBox->isChecked();

				break;
			}
		//4
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOMS:
			{
				assert(activitiesComboBox!=nullptr);
				int i=activitiesComboBox->currentIndex();
				if(i<0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Invalid activity"));
					return;
				}
				assert(i<activitiesList.count());
				int id=activitiesList.at(i);

				assert(selectedRoomsListWidget!=nullptr);
				if(selectedRoomsListWidget->count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected rooms"));
					return;
				}

				QStringList roomsList;
				for(int i=0; i<selectedRoomsListWidget->count(); i++)
					roomsList.append(selectedRoomsListWidget->item(i)->text());

				ConstraintActivityPreferredRooms* ctr=(ConstraintActivityPreferredRooms*)oldsc;
				ctr->activityId=id;
				ctr->roomsNames=roomsList;

				break;
			}
		//5
		case CONSTRAINT_STUDENTS_SET_HOME_ROOM:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				QString room=roomsComboBox->currentText();
				assert(gt.rules.searchRoom(room)>=0);

				ConstraintStudentsSetHomeRoom* ctr=(ConstraintStudentsSetHomeRoom*)oldsc;

				ctr->roomName=room;
				ctr->studentsName=students;

				break;
			}
		//6
		case CONSTRAINT_STUDENTS_SET_HOME_ROOMS:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				assert(selectedRoomsListWidget!=nullptr);
				if(selectedRoomsListWidget->count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected rooms"));
					return;
				}

				QStringList roomsList;
				for(int i=0; i<selectedRoomsListWidget->count(); i++)
					roomsList.append(selectedRoomsListWidget->item(i)->text());

				ConstraintStudentsSetHomeRooms* ctr=(ConstraintStudentsSetHomeRooms*)oldsc;

				ctr->roomsNames=roomsList;
				ctr->studentsName=students;

				break;
			}
		//7
		case CONSTRAINT_TEACHER_HOME_ROOM:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				QString room=roomsComboBox->currentText();
				assert(gt.rules.searchRoom(room)>=0);

				ConstraintTeacherHomeRoom* ctr=(ConstraintTeacherHomeRoom*)oldsc;

				ctr->roomName=room;
				ctr->teacherName=teacher;

				break;
			}
		//8
		case CONSTRAINT_TEACHER_HOME_ROOMS:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				assert(selectedRoomsListWidget!=nullptr);
				if(selectedRoomsListWidget->count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected rooms"));
					return;
				}

				QStringList roomsList;
				for(int i=0; i<selectedRoomsListWidget->count(); i++)
					roomsList.append(selectedRoomsListWidget->item(i)->text());

				ConstraintTeacherHomeRooms* ctr=(ConstraintTeacherHomeRooms*)oldsc;

				ctr->roomsNames=roomsList;
				ctr->teacherName=teacher;

				break;
			}
		//9
		case CONSTRAINT_SUBJECT_PREFERRED_ROOM:
			{
				QString subject=subjectsComboBox->currentText();
				assert(gt.rules.searchSubject(subject)>=0);

				QString room=roomsComboBox->currentText();
				assert(gt.rules.searchRoom(room)>=0);

				ConstraintSubjectPreferredRoom* ctr=(ConstraintSubjectPreferredRoom*)oldsc;

				ctr->roomName=room;
				ctr->subjectName=subject;

				break;
			}
		//10
		case CONSTRAINT_SUBJECT_PREFERRED_ROOMS:
			{
				QString subject=subjectsComboBox->currentText();
				assert(gt.rules.searchSubject(subject)>=0);

				assert(selectedRoomsListWidget!=nullptr);
				if(selectedRoomsListWidget->count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected rooms"));
					return;
				}

				QStringList roomsList;
				for(int i=0; i<selectedRoomsListWidget->count(); i++)
					roomsList.append(selectedRoomsListWidget->item(i)->text());

				ConstraintSubjectPreferredRooms* ctr=(ConstraintSubjectPreferredRooms*)oldsc;

				ctr->roomsNames=roomsList;
				ctr->subjectName=subject;

				break;
			}
		//11
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM:
			{
				QString subject=subjectsComboBox->currentText();
				assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				assert(gt.rules.searchActivityTag(activityTag)>=0);

				QString room=roomsComboBox->currentText();
				assert(gt.rules.searchRoom(room)>=0);

				ConstraintSubjectActivityTagPreferredRoom* ctr=(ConstraintSubjectActivityTagPreferredRoom*)oldsc;

				ctr->roomName=room;
				ctr->subjectName=subject;
				ctr->activityTagName=activityTag;

				break;
			}
		//12
		case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS:
			{
				QString subject=subjectsComboBox->currentText();
				assert(gt.rules.searchSubject(subject)>=0);

				QString activityTag=activityTagsComboBox->currentText();
				assert(gt.rules.searchActivityTag(activityTag)>=0);

				assert(selectedRoomsListWidget!=nullptr);
				if(selectedRoomsListWidget->count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected rooms"));
					return;
				}

				QStringList roomsList;
				for(int i=0; i<selectedRoomsListWidget->count(); i++)
					roomsList.append(selectedRoomsListWidget->item(i)->text());

				ConstraintSubjectActivityTagPreferredRooms* ctr=(ConstraintSubjectActivityTagPreferredRooms*)oldsc;

				ctr->roomsNames=roomsList;
				ctr->subjectName=subject;
				ctr->activityTagName=activityTag;

				break;
			}
		//13
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY:
			{
				ConstraintStudentsMaxBuildingChangesPerDay* ctr=(ConstraintStudentsMaxBuildingChangesPerDay*)oldsc;

				ctr->maxBuildingChangesPerDay=spinBox->value();

				break;
			}
		//14
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				ConstraintStudentsSetMaxBuildingChangesPerDay* ctr=(ConstraintStudentsSetMaxBuildingChangesPerDay*)oldsc;

				ctr->studentsName=students;
				ctr->maxBuildingChangesPerDay=spinBox->value();

				break;
			}
		//15
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				ConstraintStudentsMaxBuildingChangesPerWeek* ctr=(ConstraintStudentsMaxBuildingChangesPerWeek*)oldsc;

				ctr->maxBuildingChangesPerWeek=spinBox->value();

				break;
			}
		//16
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				ConstraintStudentsSetMaxBuildingChangesPerWeek* ctr=(ConstraintStudentsSetMaxBuildingChangesPerWeek*)oldsc;

				ctr->studentsName=students;
				ctr->maxBuildingChangesPerWeek=spinBox->value();

				break;
			}
		//17
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				ConstraintStudentsMinGapsBetweenBuildingChanges* ctr=(ConstraintStudentsMinGapsBetweenBuildingChanges*)oldsc;

				ctr->minGapsBetweenBuildingChanges=spinBox->value();

				break;
			}
		//18
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				ConstraintStudentsSetMinGapsBetweenBuildingChanges* ctr=(ConstraintStudentsSetMinGapsBetweenBuildingChanges*)oldsc;

				ctr->studentsName=students;
				ctr->minGapsBetweenBuildingChanges=spinBox->value();

				break;
			}
		//19
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY:
			{
				ConstraintTeachersMaxBuildingChangesPerDay* ctr=(ConstraintTeachersMaxBuildingChangesPerDay*)oldsc;

				ctr->maxBuildingChangesPerDay=spinBox->value();

				break;
			}
		//20
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				ConstraintTeacherMaxBuildingChangesPerDay* ctr=(ConstraintTeacherMaxBuildingChangesPerDay*)oldsc;

				ctr->teacherName=teacher;
				ctr->maxBuildingChangesPerDay=spinBox->value();

				break;
			}
		//21
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				ConstraintTeachersMaxBuildingChangesPerWeek* ctr=(ConstraintTeachersMaxBuildingChangesPerWeek*)oldsc;

				ctr->maxBuildingChangesPerWeek=spinBox->value();

				break;
			}
		//22
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_WEEK:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				ConstraintTeacherMaxBuildingChangesPerWeek* ctr=(ConstraintTeacherMaxBuildingChangesPerWeek*)oldsc;

				ctr->teacherName=teacher;
				ctr->maxBuildingChangesPerWeek=spinBox->value();

				break;
			}
		//23
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				ConstraintTeachersMinGapsBetweenBuildingChanges* ctr=(ConstraintTeachersMinGapsBetweenBuildingChanges*)oldsc;

				ctr->minGapsBetweenBuildingChanges=spinBox->value();

				break;
			}
		//24
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_BUILDING_CHANGES:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				ConstraintTeacherMinGapsBetweenBuildingChanges* ctr=(ConstraintTeacherMinGapsBetweenBuildingChanges*)oldsc;

				ctr->teacherName=teacher;
				ctr->minGapsBetweenBuildingChanges=spinBox->value();

				break;
			}
		//25
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM:
			{
				QString activityTag=activityTagsComboBox->currentText();
				assert(gt.rules.searchActivityTag(activityTag)>=0);

				QString room=roomsComboBox->currentText();
				assert(gt.rules.searchRoom(room)>=0);

				ConstraintActivityTagPreferredRoom* ctr=(ConstraintActivityTagPreferredRoom*)oldsc;

				ctr->roomName=room;
				ctr->activityTagName=activityTag;

				break;
			}
		//26
		case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS:
			{
				QString activityTag=activityTagsComboBox->currentText();
				assert(gt.rules.searchActivityTag(activityTag)>=0);

				assert(selectedRoomsListWidget!=nullptr);
				if(selectedRoomsListWidget->count()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected rooms"));
					return;
				}

				QStringList roomsList;
				for(int i=0; i<selectedRoomsListWidget->count(); i++)
					roomsList.append(selectedRoomsListWidget->item(i)->text());

				ConstraintActivityTagPreferredRooms* ctr=(ConstraintActivityTagPreferredRooms*)oldsc;

				ctr->roomsNames=roomsList;
				ctr->activityTagName=activityTag;

				break;
			}
		//27
		case CONSTRAINT_ACTIVITIES_OCCUPY_MAX_DIFFERENT_ROOMS:
			{
				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"), tr("Only one selected activity"));
					return;
				}

				ConstraintActivitiesOccupyMaxDifferentRooms* ctr=(ConstraintActivitiesOccupyMaxDifferentRooms*)oldsc;
				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				ctr->maxDifferentRooms=spinBox->value();

				break;
			}
		//28
		case CONSTRAINT_ACTIVITIES_SAME_ROOM_IF_CONSECUTIVE:
			{
				if(selectedActivitiesList.size()==0){
					QMessageBox::warning(dialog, tr("FET information"), tr("Empty list of selected activities"));
					return;
				}
				if(selectedActivitiesList.size()==1){
					QMessageBox::warning(dialog, tr("FET information"), tr("Only one selected activity"));
					return;
				}

				ConstraintActivitiesSameRoomIfConsecutive* ctr=(ConstraintActivitiesSameRoomIfConsecutive*)oldsc;
				ctr->activitiesIds=selectedActivitiesList;
				ctr->recomputeActivitiesSet();

				break;
			}
		//29
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY:
			{
				ConstraintStudentsMaxRoomChangesPerDay* ctr=(ConstraintStudentsMaxRoomChangesPerDay*)oldsc;

				ctr->maxRoomChangesPerDay=spinBox->value();

				break;
			}
		//30
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				ConstraintStudentsSetMaxRoomChangesPerDay* ctr=(ConstraintStudentsSetMaxRoomChangesPerDay*)oldsc;

				ctr->studentsName=students;
				ctr->maxRoomChangesPerDay=spinBox->value();

				break;
			}
		//31
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_WEEK:
			{
				ConstraintStudentsMaxRoomChangesPerWeek* ctr=(ConstraintStudentsMaxRoomChangesPerWeek*)oldsc;

				ctr->maxRoomChangesPerWeek=spinBox->value();

				break;
			}
		//32
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_WEEK:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				ConstraintStudentsSetMaxRoomChangesPerWeek* ctr=(ConstraintStudentsSetMaxRoomChangesPerWeek*)oldsc;

				ctr->studentsName=students;
				ctr->maxRoomChangesPerWeek=spinBox->value();

				break;
			}
		//33
		case CONSTRAINT_STUDENTS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				ConstraintStudentsMinGapsBetweenRoomChanges* ctr=(ConstraintStudentsMinGapsBetweenRoomChanges*)oldsc;

				ctr->minGapsBetweenRoomChanges=spinBox->value();

				break;
			}
		//34
		case CONSTRAINT_STUDENTS_SET_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				ConstraintStudentsSetMinGapsBetweenRoomChanges* ctr=(ConstraintStudentsSetMinGapsBetweenRoomChanges*)oldsc;

				ctr->studentsName=students;
				ctr->minGapsBetweenRoomChanges=spinBox->value();

				break;
			}
		//35
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY:
			{
				ConstraintTeachersMaxRoomChangesPerDay* ctr=(ConstraintTeachersMaxRoomChangesPerDay*)oldsc;

				ctr->maxRoomChangesPerDay=spinBox->value();

				break;
			}
		//36
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				ConstraintTeacherMaxRoomChangesPerDay* ctr=(ConstraintTeacherMaxRoomChangesPerDay*)oldsc;

				ctr->teacherName=teacher;
				ctr->maxRoomChangesPerDay=spinBox->value();

				break;
			}
		//37
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_WEEK:
			{
				ConstraintTeachersMaxRoomChangesPerWeek* ctr=(ConstraintTeachersMaxRoomChangesPerWeek*)oldsc;

				ctr->maxRoomChangesPerWeek=spinBox->value();

				break;
			}
		//38
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_WEEK:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				ConstraintTeacherMaxRoomChangesPerWeek* ctr=(ConstraintTeacherMaxRoomChangesPerWeek*)oldsc;

				ctr->teacherName=teacher;
				ctr->maxRoomChangesPerWeek=spinBox->value();

				break;
			}
		//39
		case CONSTRAINT_TEACHERS_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				ConstraintTeachersMinGapsBetweenRoomChanges* ctr=(ConstraintTeachersMinGapsBetweenRoomChanges*)oldsc;

				ctr->minGapsBetweenRoomChanges=spinBox->value();

				break;
			}
		//40
		case CONSTRAINT_TEACHER_MIN_GAPS_BETWEEN_ROOM_CHANGES:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				ConstraintTeacherMinGapsBetweenRoomChanges* ctr=(ConstraintTeacherMinGapsBetweenRoomChanges*)oldsc;

				ctr->teacherName=teacher;
				ctr->minGapsBetweenRoomChanges=spinBox->value();

				break;
			}
		//41
		case CONSTRAINT_TEACHER_ROOM_NOT_AVAILABLE_TIMES:
			{
				QList<int> days;
				QList<int> hours;
				getTimesTable(timesTable, days, hours, true);

				ConstraintTeacherRoomNotAvailableTimes* ctr=(ConstraintTeacherRoomNotAvailableTimes*)oldsc;

				ctr->teacherName=teachersComboBox->currentText();

				ctr->room=roomsComboBox->currentText();

				ctr->days=days;
				ctr->hours=hours;

				break;
			}
		//42
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				ConstraintStudentsMaxRoomChangesPerRealDay* ctr=(ConstraintStudentsMaxRoomChangesPerRealDay*)oldsc;

				ctr->maxRoomChangesPerDay=spinBox->value();

				break;
			}
		//43
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				ConstraintStudentsSetMaxRoomChangesPerRealDay* ctr=(ConstraintStudentsSetMaxRoomChangesPerRealDay*)oldsc;

				ctr->studentsName=students;
				ctr->maxRoomChangesPerDay=spinBox->value();

				break;
			}
		//44
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				ConstraintTeachersMaxRoomChangesPerRealDay* ctr=(ConstraintTeachersMaxRoomChangesPerRealDay*)oldsc;

				ctr->maxRoomChangesPerDay=spinBox->value();

				break;
			}
		//45
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				ConstraintTeacherMaxRoomChangesPerRealDay* ctr=(ConstraintTeacherMaxRoomChangesPerRealDay*)oldsc;

				ctr->teacherName=teacher;
				ctr->maxRoomChangesPerDay=spinBox->value();

				break;
			}
		//46
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				ConstraintStudentsMaxBuildingChangesPerRealDay* ctr=(ConstraintStudentsMaxBuildingChangesPerRealDay*)oldsc;

				ctr->maxBuildingChangesPerDay=spinBox->value();

				break;
			}
		//47
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				ConstraintStudentsSetMaxBuildingChangesPerRealDay* ctr=(ConstraintStudentsSetMaxBuildingChangesPerRealDay*)oldsc;

				ctr->studentsName=students;
				ctr->maxBuildingChangesPerDay=spinBox->value();

				break;
			}
		//48
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				ConstraintTeachersMaxBuildingChangesPerRealDay* ctr=(ConstraintTeachersMaxBuildingChangesPerRealDay*)oldsc;

				ctr->maxBuildingChangesPerDay=spinBox->value();

				break;
			}
		//49
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY:
			{
				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				ConstraintTeacherMaxBuildingChangesPerRealDay* ctr=(ConstraintTeacherMaxBuildingChangesPerRealDay*)oldsc;

				ctr->teacherName=teacher;
				ctr->maxBuildingChangesPerDay=spinBox->value();

				break;
			}
		//50
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				ConstraintStudentsMaxBuildingChangesPerDayInInterval* ctr=(ConstraintStudentsMaxBuildingChangesPerDayInInterval*)oldsc;

				ctr->maxBuildingChangesPerDay=spinBox->value();

				ctr->intervalStart=startHour;
				ctr->intervalEnd=endHour;

				break;
			}
		//51
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				ConstraintStudentsSetMaxBuildingChangesPerDayInInterval* ctr=(ConstraintStudentsSetMaxBuildingChangesPerDayInInterval*)oldsc;

				ctr->studentsName=students;
				ctr->maxBuildingChangesPerDay=spinBox->value();

				ctr->intervalStart=startHour;
				ctr->intervalEnd=endHour;

				break;
			}
		//52
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				ConstraintTeachersMaxBuildingChangesPerDayInInterval* ctr=(ConstraintTeachersMaxBuildingChangesPerDayInInterval*)oldsc;

				ctr->maxBuildingChangesPerDay=spinBox->value();

				ctr->intervalStart=startHour;
				ctr->intervalEnd=endHour;

				break;
			}
		//53
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				ConstraintTeacherMaxBuildingChangesPerDayInInterval* ctr=(ConstraintTeacherMaxBuildingChangesPerDayInInterval*)oldsc;

				ctr->teacherName=teacher;
				ctr->maxBuildingChangesPerDay=spinBox->value();

				ctr->intervalStart=startHour;
				ctr->intervalEnd=endHour;

				break;
			}
		//54
		case CONSTRAINT_STUDENTS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				ConstraintStudentsMaxBuildingChangesPerRealDayInInterval* ctr=(ConstraintStudentsMaxBuildingChangesPerRealDayInInterval*)oldsc;

				ctr->maxBuildingChangesPerDay=spinBox->value();

				ctr->intervalStart=startHour;
				ctr->intervalEnd=endHour;

				break;
			}
		//51
		case CONSTRAINT_STUDENTS_SET_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval* ctr=(ConstraintStudentsSetMaxBuildingChangesPerRealDayInInterval*)oldsc;

				ctr->studentsName=students;
				ctr->maxBuildingChangesPerDay=spinBox->value();

				ctr->intervalStart=startHour;
				ctr->intervalEnd=endHour;

				break;
			}
		//52
		case CONSTRAINT_TEACHERS_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				ConstraintTeachersMaxBuildingChangesPerRealDayInInterval* ctr=(ConstraintTeachersMaxBuildingChangesPerRealDayInInterval*)oldsc;

				ctr->maxBuildingChangesPerDay=spinBox->value();

				ctr->intervalStart=startHour;
				ctr->intervalEnd=endHour;

				break;
			}
		//53
		case CONSTRAINT_TEACHER_MAX_BUILDING_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				ConstraintTeacherMaxBuildingChangesPerRealDayInInterval* ctr=(ConstraintTeacherMaxBuildingChangesPerRealDayInInterval*)oldsc;

				ctr->teacherName=teacher;
				ctr->maxBuildingChangesPerDay=spinBox->value();

				ctr->intervalStart=startHour;
				ctr->intervalEnd=endHour;

				break;
			}
		//58
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				ConstraintStudentsMaxRoomChangesPerDayInInterval* ctr=(ConstraintStudentsMaxRoomChangesPerDayInInterval*)oldsc;

				ctr->maxRoomChangesPerDay=spinBox->value();

				ctr->intervalStart=startHour;
				ctr->intervalEnd=endHour;

				break;
			}
		//59
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				ConstraintStudentsSetMaxRoomChangesPerDayInInterval* ctr=(ConstraintStudentsSetMaxRoomChangesPerDayInInterval*)oldsc;

				ctr->studentsName=students;
				ctr->maxRoomChangesPerDay=spinBox->value();

				ctr->intervalStart=startHour;
				ctr->intervalEnd=endHour;

				break;
			}
		//60
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				ConstraintTeachersMaxRoomChangesPerDayInInterval* ctr=(ConstraintTeachersMaxRoomChangesPerDayInInterval*)oldsc;

				ctr->maxRoomChangesPerDay=spinBox->value();

				ctr->intervalStart=startHour;
				ctr->intervalEnd=endHour;

				break;
			}
		//61
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				ConstraintTeacherMaxRoomChangesPerDayInInterval* ctr=(ConstraintTeacherMaxRoomChangesPerDayInInterval*)oldsc;

				ctr->teacherName=teacher;
				ctr->maxRoomChangesPerDay=spinBox->value();

				ctr->intervalStart=startHour;
				ctr->intervalEnd=endHour;

				break;
			}
		//62
		case CONSTRAINT_STUDENTS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				ConstraintStudentsMaxRoomChangesPerRealDayInInterval* ctr=(ConstraintStudentsMaxRoomChangesPerRealDayInInterval*)oldsc;

				ctr->maxRoomChangesPerDay=spinBox->value();

				ctr->intervalStart=startHour;
				ctr->intervalEnd=endHour;

				break;
			}
		//63
		case CONSTRAINT_STUDENTS_SET_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				QString students=studentsComboBox->currentText();
				assert(gt.rules.searchStudentsSet(students)!=nullptr);

				ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval* ctr=(ConstraintStudentsSetMaxRoomChangesPerRealDayInInterval*)oldsc;

				ctr->studentsName=students;
				ctr->maxRoomChangesPerDay=spinBox->value();

				ctr->intervalStart=startHour;
				ctr->intervalEnd=endHour;

				break;
			}
		//64
		case CONSTRAINT_TEACHERS_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				ConstraintTeachersMaxRoomChangesPerRealDayInInterval* ctr=(ConstraintTeachersMaxRoomChangesPerRealDayInInterval*)oldsc;

				ctr->maxRoomChangesPerDay=spinBox->value();

				ctr->intervalStart=startHour;
				ctr->intervalEnd=endHour;

				break;
			}
		//65
		case CONSTRAINT_TEACHER_MAX_ROOM_CHANGES_PER_REAL_DAY_IN_INTERVAL:
			{
				int startHour=intervalStartHourComboBox->currentIndex();
				int endHour=intervalEndHourComboBox->currentIndex();
				if(startHour<0 || startHour>=2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("Start hour invalid"));
					return;
				}
				if(endHour<0 || endHour>2*gt.rules.nHoursPerDay){
					QMessageBox::warning(dialog, tr("FET information"), tr("End hour invalid"));
					return;
				}
				if(endHour-startHour<2){
					QMessageBox::warning(dialog, tr("FET information"),
						tr("End hour - start hour should be >= 2, so that this constraint is a nontrivial one."));
					return;
				}

				QString teacher=teachersComboBox->currentText();
				assert(gt.rules.searchTeacher(teacher)>=0);

				ConstraintTeacherMaxRoomChangesPerRealDayInInterval* ctr=(ConstraintTeacherMaxRoomChangesPerRealDayInInterval*)oldsc;

				ctr->teacherName=teacher;
				ctr->maxRoomChangesPerDay=spinBox->value();

				ctr->intervalStart=startHour;
				ctr->intervalEnd=endHour;

				break;
			}
		//66
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
										 tr("Please select at least two activity tags"));
					return;
				}

				QStringList selectedActivityTagsList;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					selectedActivityTagsList.append(selectedActivityTagsListWidget->item(i)->text());

				ConstraintRoomMaxActivityTagsPerDayFromSet* ctr=(ConstraintRoomMaxActivityTagsPerDayFromSet*)oldsc;

				ctr->room=roomsComboBox->currentText();
				ctr->maxTags=spinBox->value();
				ctr->tagsList=selectedActivityTagsList;

				break;
			}
		//67
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_REAL_DAY_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
										 tr("Please select at least two activity tags"));
					return;
				}

				QStringList selectedActivityTagsList;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					selectedActivityTagsList.append(selectedActivityTagsListWidget->item(i)->text());

				ConstraintRoomMaxActivityTagsPerRealDayFromSet* ctr=(ConstraintRoomMaxActivityTagsPerRealDayFromSet*)oldsc;

				ctr->room=roomsComboBox->currentText();
				ctr->maxTags=spinBox->value();
				ctr->tagsList=selectedActivityTagsList;

				break;
			}
		//68
		case CONSTRAINT_ROOM_MAX_ACTIVITY_TAGS_PER_WEEK_FROM_SET:
			{
				if(selectedActivityTagsListWidget->count()<2){
					QMessageBox::warning(dialog, tr("FET information"),
										 tr("Please select at least two activity tags"));
					return;
				}

				QStringList selectedActivityTagsList;
				for(int i=0; i<selectedActivityTagsListWidget->count(); i++)
					selectedActivityTagsList.append(selectedActivityTagsListWidget->item(i)->text());

				ConstraintRoomMaxActivityTagsPerWeekFromSet* ctr=(ConstraintRoomMaxActivityTagsPerWeekFromSet*)oldsc;

				ctr->room=roomsComboBox->currentText();
				ctr->maxTags=spinBox->value();
				ctr->tagsList=selectedActivityTagsList;

				break;
			}

		default:
			assert(0);
			break;
	}

	oldsc->weightPercentage=weight;

	QString newcs=oldsc->getDetailedDescription(gt.rules);
	gt.rules.addUndoPoint(tr("Modified the constraint:\n\n%1\ninto\n\n%2").arg(oldcs).arg(newcs));

	gt.rules.internalStructureComputed=false;
	setRulesModifiedAndOtherThings(&gt.rules);

	if(oldsc->type==CONSTRAINT_ACTIVITY_PREFERRED_ROOM){
		LockUnlock::computeLockedUnlockedActivitiesOnlySpace();
		LockUnlock::increaseCommunicationSpinBox();
	}

	dialog->close();
}

void AddOrModifySpaceConstraint::cancelClicked()
{
	dialog->close();
}

/*void AddOrModifySpaceConstraint::colorItem(QTableWidgetItem* item)
{
	if(USE_GUI_COLORS){
		if(item->text()==NO)
			item->setBackground(QBrush(QColorConstants::DarkGreen));
		else
			item->setBackground(QBrush(QColorConstants::DarkRed));
		item->setForeground(QBrush(QColorConstants::LightGray));
	}
}*/

void AddOrModifySpaceConstraint::horizontalHeaderClicked(int col)
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

void AddOrModifySpaceConstraint::verticalHeaderClicked(int row)
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

void AddOrModifySpaceConstraint::cellEntered(int row, int col)
{
	cellEnteredTimesTable(timesTable, row, col);
	/*highlightOnCellEntered(timesTable, row, col);*/
}

void AddOrModifySpaceConstraint::colorsCheckBoxToggled()
{
	timesTable->useColors=colorsCheckBox->isChecked();
	
	colorsCheckBoxToggledTimesTable(timesTable);
}

void AddOrModifySpaceConstraint::toggleAllClicked()
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

void AddOrModifySpaceConstraint::itemClicked(QTableWidgetItem* item)
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

void AddOrModifySpaceConstraint::helpClicked()
{
	switch(type){
		//3
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOM:
			{
				QString s;

				s+=tr("A room can be real (the simplest and the most used scenario) or virtual. You can read more about this in the rooms dialog, by clicking the Help button there.");
				s+="\n\n";
				s+=tr("If the preferred room selected in the combo box is real, the list of selected real rooms must remain empty.");
				s+="\n\n";
				s+=tr("If the preferred room selected in the combo box is virtual, you can select also the list of real rooms to be allocated to the "
				 "selected activity (if the preferred room selected in the combo box is virtual and the list of selected real rooms is not empty, the "
				 "weight of the constraint must be 100.0%).");

				LongTextMessageBox::largeInformation(dialog, tr("FET help"), s);

				break;
			}

		default:
			assert(0);
			break;
	}
}

void AddOrModifySpaceConstraint::checkBoxToggled()
{
	switch(type){
		//3
		case CONSTRAINT_ACTIVITY_PREFERRED_ROOM:
			{
				//nothing, we just avoid the assert below

				break;
			}

		default:
			assert(0);
			break;
	}
}

bool AddOrModifySpaceConstraint::filterOk(Activity* act)
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

int AddOrModifySpaceConstraint::filterActivitiesComboBox()
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

void AddOrModifySpaceConstraint::filterActivitiesListWidget()
{
	assert(activitiesListWidget!=nullptr);
	assert(activitiesComboBox==nullptr);

	activitiesListWidget->clear();
	activitiesList.clear();

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

	int q=activitiesListWidget->verticalScrollBar()->minimum();
	activitiesListWidget->verticalScrollBar()->setValue(q);
}

void AddOrModifySpaceConstraint::addAllActivitiesClicked()
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

void AddOrModifySpaceConstraint::clearActivitiesClicked()
{
	selectedActivitiesListWidget->clear();
	selectedActivitiesList.clear();
}

void AddOrModifySpaceConstraint::addActivity()
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

void AddOrModifySpaceConstraint::removeActivity()
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

void AddOrModifySpaceConstraint::clearSelectedRealRooms()
{
	selectedRealRoomsListWidget->clear();
}

void AddOrModifySpaceConstraint::addRealRoom()
{
	int ind=allRealRoomsListWidget->currentRow();
	if(ind<0 || ind>=allRealRoomsListWidget->count()){
		QMessageBox::information(dialog, tr("FET information"), tr("Invalid selected real room."));
		return;
	}

	bool exists=false;
	for(int i=0; i<selectedRealRoomsListWidget->count(); i++)
		if(selectedRealRoomsListWidget->item(i)->text()==allRealRoomsListWidget->item(ind)->text()){
			exists=true;
			break;
		}

	if(!exists){
		selectedRealRoomsListWidget->addItem(allRealRoomsListWidget->item(ind)->text());
		selectedRealRoomsListWidget->setCurrentRow(selectedRealRoomsListWidget->count()-1);
	}
}

void AddOrModifySpaceConstraint::removeRealRoom()
{
	int ind=selectedRealRoomsListWidget->currentRow();
	if(ind<0 || ind>=selectedRealRoomsListWidget->count()){
		QMessageBox::information(dialog, tr("FET information"), tr("Invalid selected real room."));
		return;
	}

	QListWidgetItem* item=selectedRealRoomsListWidget->takeItem(ind);
	delete item;

	if(ind>=selectedRealRoomsListWidget->count())
		ind=selectedRealRoomsListWidget->count()-1;
	selectedRealRoomsListWidget->setCurrentRow(ind);
}

void AddOrModifySpaceConstraint::clearSelectedRooms()
{
	selectedRoomsListWidget->clear();
}

void AddOrModifySpaceConstraint::addRoom()
{
	int ind=allRoomsListWidget->currentRow();
	if(ind<0 || ind>=allRoomsListWidget->count()){
		QMessageBox::information(dialog, tr("FET information"), tr("Invalid selected room."));
		return;
	}

	bool exists=false;
	for(int i=0; i<selectedRoomsListWidget->count(); i++)
		if(selectedRoomsListWidget->item(i)->text()==allRoomsListWidget->item(ind)->text()){
			exists=true;
			break;
		}

	if(!exists){
		selectedRoomsListWidget->addItem(allRoomsListWidget->item(ind)->text());
		selectedRoomsListWidget->setCurrentRow(selectedRoomsListWidget->count()-1);
	}
}

void AddOrModifySpaceConstraint::removeRoom()
{
	int ind=selectedRoomsListWidget->currentRow();
	if(ind<0 || ind>=selectedRoomsListWidget->count()){
		QMessageBox::information(dialog, tr("FET information"), tr("Invalid selected room."));
		return;
	}

	QListWidgetItem* item=selectedRoomsListWidget->takeItem(ind);
	delete item;

	if(ind>=selectedRoomsListWidget->count())
		ind=selectedRoomsListWidget->count()-1;
	selectedRoomsListWidget->setCurrentRow(ind);
}

/*void AddOrModifySpaceConstraint::addAllActivityTagsClicked()
{
	for(int tmp=0; tmp<activityTagsListWidget->count(); tmp++){
		QString at=activityTagsListWidget->item(tmp)->text();

		if(selectedActivityTagsSet.contains(at))
			continue;

		selectedActivityTagsListWidget->addItem(at);
		selectedActivityTagsSet.insert(at);
	}

	selectedActivityTagsListWidget->setCurrentRow(selectedActivityTagsListWidget->count()-1);
}*/

void AddOrModifySpaceConstraint::clearActivityTagsClicked()
{
	selectedActivityTagsListWidget->clear();
	selectedActivityTagsSet.clear();
}

void AddOrModifySpaceConstraint::addActivityTag()
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

void AddOrModifySpaceConstraint::removeActivityTag()
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

void AddOrModifySpaceConstraint::filterActivityTagsCheckBoxToggled()
{
	if(!filterActivityTagsCheckBox->isChecked() || roomsComboBox->count()==0 || roomsComboBox->currentIndex()<0 || roomsComboBox->currentIndex()>=gt.rules.roomsList.count()){
		activityTagsListWidget->clear();
		for(int i=0; i<gt.rules.activityTagsList.size(); i++){
			ActivityTag* at=gt.rules.activityTagsList[i];
			activityTagsListWidget->addItem(at->name);
		}
	}
	else{
		QSet<QString> tagsSet;

		QString room=roomsComboBox->currentText();
		Room* rm=gt.rules.roomsList.at(roomsComboBox->currentIndex());
		//Hmmm... for other than activity preferred room(s) it might be quite inefficient (slow).
		for(SpaceConstraint* ctr : std::as_const(gt.rules.spaceConstraintsList)){
			switch(ctr->type){
				//3
				case CONSTRAINT_ACTIVITY_PREFERRED_ROOM:
					{
						ConstraintActivityPreferredRoom* sc=(ConstraintActivityPreferredRoom*)ctr;
						if(sc->isRelatedToRoom(rm)){
							Activity* act=gt.rules.activitiesPointerHash.value(sc->activityId, nullptr);
							//assert(act!=nullptr);
							if(act!=nullptr)
								for(const QString& at : act->activityTagsNames)
									tagsSet.insert(at);
						}
						break;
					}
				//4
				case CONSTRAINT_ACTIVITY_PREFERRED_ROOMS:
					{
						ConstraintActivityPreferredRooms* sc=(ConstraintActivityPreferredRooms*)ctr;
						if(sc->isRelatedToRoom(rm)){
							Activity* act=gt.rules.activitiesPointerHash.value(sc->activityId, nullptr);
							//assert(act!=nullptr);
							if(act!=nullptr)
								for(const QString& at : act->activityTagsNames)
									tagsSet.insert(at);
						}
						break;
					}
				//5
				case CONSTRAINT_STUDENTS_SET_HOME_ROOM:
					{
						ConstraintStudentsSetHomeRoom* sc=(ConstraintStudentsSetHomeRoom*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				//6
				case CONSTRAINT_STUDENTS_SET_HOME_ROOMS:
					{
						ConstraintStudentsSetHomeRooms* sc=(ConstraintStudentsSetHomeRooms*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				//7
				case CONSTRAINT_TEACHER_HOME_ROOM:
					{
						ConstraintTeacherHomeRoom* sc=(ConstraintTeacherHomeRoom*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				//8
				case CONSTRAINT_TEACHER_HOME_ROOMS:
					{
						ConstraintTeacherHomeRooms* sc=(ConstraintTeacherHomeRooms*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				//9
				case CONSTRAINT_SUBJECT_PREFERRED_ROOM:
					{
						ConstraintSubjectPreferredRoom* sc=(ConstraintSubjectPreferredRoom*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				//10
				case CONSTRAINT_SUBJECT_PREFERRED_ROOMS:
					{
						ConstraintSubjectPreferredRooms* sc=(ConstraintSubjectPreferredRooms*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				//11
				case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOM:
					{
						ConstraintSubjectActivityTagPreferredRoom* sc=(ConstraintSubjectActivityTagPreferredRoom*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				//12
				case CONSTRAINT_SUBJECT_ACTIVITY_TAG_PREFERRED_ROOMS:
					{
						ConstraintSubjectActivityTagPreferredRooms* sc=(ConstraintSubjectActivityTagPreferredRooms*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				///6 apr 2009
				//13
				case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOM:
					{
						ConstraintActivityTagPreferredRoom* sc=(ConstraintActivityTagPreferredRoom*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				//14
				case CONSTRAINT_ACTIVITY_TAG_PREFERRED_ROOMS:
					{
						ConstraintActivityTagPreferredRooms* sc=(ConstraintActivityTagPreferredRooms*)ctr;
						if(sc->isRelatedToRoom(rm))
							for(Activity* act : std::as_const(gt.rules.activitiesList))
								if(sc->isRelatedToActivity(act))
									for(const QString& at : act->activityTagsNames)
										tagsSet.insert(at);
						break;
					}
				////
				default:
					//do nothing
					break;
			}
		}

		activityTagsListWidget->clear();
		for(int i=0; i<gt.rules.activityTagsList.size(); i++){
			ActivityTag* at=gt.rules.activityTagsList[i];
			if(tagsSet.contains(at->name))
				activityTagsListWidget->addItem(at->name);
		}
	}
}

void AddOrModifySpaceConstraint::showRelatedCheckBoxToggled()
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
	
	if(filterGroupBox!=nullptr && activitiesListWidget!=nullptr){
		assert(activitiesComboBox==nullptr);

		filterActivitiesListWidget();
	}

	if(filterGroupBox!=nullptr && activitiesComboBox!=nullptr){
		assert(activitiesListWidget==nullptr);

		int i=filterActivitiesComboBox();
		if(i>=0){
			assert(activitiesComboBox!=nullptr);
			assert(i<activitiesList.count());
			assert(i<activitiesComboBox->count());
			activitiesComboBox->setCurrentIndex(i);
		}
	}
}
