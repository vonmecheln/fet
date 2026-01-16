//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#ifndef FETMAINFORM_H
#define FETMAINFORM_H

#include <QtGlobal>

#include "ui_fetmainform_template.h"

#include <QMutex>
#include <QThread>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QResizeEvent>
#include <QCloseEvent>

#include <QString>
#include <QStringList>

#include <QAction>
#include <QMenu>

#include <QMap>

#include <QList>
#include <QHash>

class QNetworkAccessManager;
class QNetworkReply;

extern const QString COMPANY;
extern const QString PROGRAM;

const int MAX_RECENT_FILES=10;

class FetMainForm: public QMainWindow, public Ui::FetMainForm_template
{
	Q_OBJECT
	
private:
	//
	QMenu* menuA_teacher_1_time_constraints;
	QMenu* menuA_teacher_2_time_constraints;
	QMenu* menuA_teacher_3_time_constraints;
	QMenu* menuA_teacher_4_time_constraints;
	QMenu* menuAll_teachers_1_time_constraints;
	QMenu* menuAll_teachers_2_time_constraints;
	QMenu* menuAll_teachers_3_time_constraints;
	QMenu* menuAll_teachers_4_time_constraints;
	//
	QMenu* menuA_students_set_1_time_constraints;
	QMenu* menuA_students_set_2_time_constraints;
	QMenu* menuA_students_set_3_time_constraints;
	QMenu* menuA_students_set_4_time_constraints;
	QMenu* menuAll_students_1_time_constraints;
	QMenu* menuAll_students_2_time_constraints;
	QMenu* menuAll_students_3_time_constraints;
	QMenu* menuAll_students_4_time_constraints;
	
	QMenu* menuActivities_preferred_times_time_constraints;
	QMenu* menuActivities_begin_end_day_time_constraints;
	QMenu* menuActivities_others_1_time_constraints;
	QMenu* menuActivities_others_2_time_constraints;
	QMenu* menuActivities_others_3_time_constraints;

	QAction* dataTimeConstraintsActivitiesPreferredTimeSlotsAction;
	QAction* dataTimeConstraintsActivitiesSameStartingTimeAction;

	QAction* dataTimeConstraintsTwoSetsOfActivitiesSameSectionsAction;

	QAction* dataTimeConstraintsActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlotsAction;

	QAction* dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction;
	QAction* dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction;
	QAction* dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction;
	QAction* dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction;
	QAction* dataTimeConstraintsTeacherNotAvailableTimesAction;
	QAction* dataTimeConstraintsTeachersNotAvailableTimesAction;
	QAction* dataTimeConstraintsBasicCompulsoryTimeAction;
	QAction* dataTimeConstraintsStudentsSetNotAvailableTimesAction;
	QAction* dataTimeConstraintsStudentsNotAvailableTimesAction;
	QAction* dataTimeConstraintsBreakTimesAction;
	QAction* dataTimeConstraintsTeacherMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsTeachersMaxHoursDailyAction;

	QAction* dataTimeConstraintsTeacherMaxHoursPerTermAction;
	QAction* dataTimeConstraintsTeachersMaxHoursPerTermAction;

	QAction* dataTimeConstraintsTeachersMaxHoursDailyInIntervalAction;
	QAction* dataTimeConstraintsTeacherMaxHoursDailyInIntervalAction;
	QAction* dataTimeConstraintsStudentsMaxHoursDailyInIntervalAction;
	QAction* dataTimeConstraintsStudentsSetMaxHoursDailyInIntervalAction;

	QAction* dataTimeConstraintsTeacherPairOfMutuallyExclusiveTimeSlotsAction;
	QAction* dataTimeConstraintsTeachersPairOfMutuallyExclusiveTimeSlotsAction;
	QAction* dataTimeConstraintsStudentsSetPairOfMutuallyExclusiveTimeSlotsAction;
	QAction* dataTimeConstraintsStudentsPairOfMutuallyExclusiveTimeSlotsAction;

	QAction* dataTimeConstraintsTeacherPairOfMutuallyExclusiveSetsOfTimeSlotsAction;
	QAction* dataTimeConstraintsTeachersPairOfMutuallyExclusiveSetsOfTimeSlotsAction;
	QAction* dataTimeConstraintsStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlotsAction;
	QAction* dataTimeConstraintsStudentsPairOfMutuallyExclusiveSetsOfTimeSlotsAction;

	QAction* dataTimeConstraintsTeacherOccupiesMaxSetsOfTimeSlotsFromSelectionAction;
	QAction* dataTimeConstraintsTeachersOccupyMaxSetsOfTimeSlotsFromSelectionAction;

	QAction* dataTimeConstraintsStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelectionAction;
	QAction* dataTimeConstraintsStudentsOccupyMaxSetsOfTimeSlotsFromSelectionAction;

	QAction* dataTimeConstraintsActivitiesOccupyMaxSetsOfTimeSlotsFromSelectionAction;

	QAction* dataTimeConstraintsActivitiesPairOfMutuallyExclusiveTimeSlotsAction;
	QAction* dataTimeConstraintsActivitiesPairOfMutuallyExclusiveSetsOfTimeSlotsAction;

	QAction* dataTimeConstraintsActivitiesOverlapCompletelyOrDoNotOverlapAction;

	QAction* dataTimeConstraintsActivityPreferredStartingTimeAction;
	QAction* dataTimeConstraintsStudentsSetMaxGapsPerWeekAction;
	QAction* dataTimeConstraintsStudentsMaxGapsPerWeekAction;
	QAction* dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsActivitiesNotOverlappingAction;
	QAction* dataTimeConstraintsActivityTagsNotOverlappingAction;
	QAction* dataTimeConstraintsMinDaysBetweenActivitiesAction;
	QAction* dataTimeConstraintsMinHalfDaysBetweenActivitiesAction;
	QAction* dataSpaceConstraintsBasicCompulsorySpaceAction;
	QAction* dataSpaceConstraintsRoomNotAvailableTimesAction;
	QAction* dataSpaceConstraintsTeacherRoomNotAvailableTimesAction;
	QAction* dataSpaceConstraintsActivityPreferredRoomAction;
	QAction* dataTimeConstraintsActivitiesSameStartingHourAction;
	QAction* dataSpaceConstraintsActivityPreferredRoomsAction;
	QAction* dataSpaceConstraintsStudentsSetHomeRoomAction;
	QAction* dataSpaceConstraintsStudentsSetHomeRoomsAction;
	QAction* dataTimeConstraintsTeachersMaxGapsPerWeekAction;
	QAction* dataTimeConstraintsTeacherMaxGapsPerWeekAction;
	QAction* dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsTeacherMaxHoursDailyAction;
	QAction* dataTimeConstraintsStudentsSetMaxHoursDailyAction;
	QAction* dataTimeConstraintsStudentsMaxHoursDailyAction;
	QAction* dataTimeConstraintsStudentsMinHoursDailyAction;
	QAction* dataTimeConstraintsStudentsSetMinHoursDailyAction;
	QAction* dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction;
	QAction* dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsAction;
	QAction* dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction;
	QAction* dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsAction;
	//
	//2021-12-15
	QAction* dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagAction;
	QAction* dataTimeConstraintsStudentsMinGapsBetweenActivityTagAction;
	QAction* dataTimeConstraintsTeacherMinGapsBetweenActivityTagAction;
	QAction* dataTimeConstraintsTeachersMinGapsBetweenActivityTagAction;
	//2024-03-16
	QAction* dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction;
	QAction* dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction;
	QAction* dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction;
	QAction* dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction;
	//
	QAction* dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagPerRealDayAction;
	QAction* dataTimeConstraintsStudentsMinGapsBetweenActivityTagPerRealDayAction;
	QAction* dataTimeConstraintsTeacherMinGapsBetweenActivityTagPerRealDayAction;
	QAction* dataTimeConstraintsTeachersMinGapsBetweenActivityTagPerRealDayAction;
	//2024-05-20
	QAction* dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction;
	QAction* dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction;
	QAction* dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction;
	QAction* dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction;
	//
	QAction* dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction;
	QAction* dataTimeConstraintsStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction;
	QAction* dataTimeConstraintsTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction;
	QAction* dataTimeConstraintsTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction;
	//
	QAction* dataTimeConstraintsTwoActivitiesConsecutiveAction;
	QAction* dataTimeConstraintsActivityEndsStudentsDayAction;
	QAction* dataTimeConstraintsActivityEndsTeachersDayAction;
	QAction* dataTimeConstraintsActivityBeginsStudentsDayAction;
	QAction* dataTimeConstraintsActivityBeginsTeachersDayAction;

	QAction* dataTimeConstraintsActivityBeginsOrEndsStudentsDayAction;
	QAction* dataTimeConstraintsActivityBeginsOrEndsTeachersDayAction;

	QAction* dataTimeConstraintsTeachersMinHoursDailyAction;
	QAction* dataTimeConstraintsTeacherMinHoursDailyAction;
	QAction* dataTimeConstraintsTeachersMaxGapsPerDayAction;
	QAction* dataTimeConstraintsTeacherMaxGapsPerDayAction;
	QAction* dataTimeConstraintsTeachersMaxGapsPerMorningAndAfternoonAction;
	QAction* dataTimeConstraintsTeacherMaxGapsPerMorningAndAfternoonAction;
	QAction* dataTimeConstraintsTeacherMaxSpanPerDayAction;
	QAction* dataTimeConstraintsTeachersMaxSpanPerDayAction;
	QAction* dataTimeConstraintsStudentsSetMaxSpanPerDayAction;
	QAction* dataTimeConstraintsStudentsMaxSpanPerDayAction;
	QAction* dataTimeConstraintsTeacherMinRestingHoursAction;
	QAction* dataTimeConstraintsTeachersMinRestingHoursAction;
	QAction* dataTimeConstraintsStudentsSetMinRestingHoursAction;
	QAction* dataTimeConstraintsStudentsMinRestingHoursAction;
	QAction* dataSpaceConstraintsSubjectPreferredRoomAction;
	QAction* dataSpaceConstraintsSubjectPreferredRoomsAction;
	QAction* dataSpaceConstraintsSubjectActivityTagPreferredRoomAction;
	QAction* dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction;
	QAction* dataSpaceConstraintsTeacherHomeRoomAction;
	QAction* dataSpaceConstraintsTeacherHomeRoomsAction;
	QAction* dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction;
	QAction* dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction;
	QAction* dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction;
	QAction* dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction;
	QAction* dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction;
	QAction* dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction;
	QAction* dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction;
	QAction* dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction;
	QAction* dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction;
	QAction* dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction;
	QAction* dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction;
	QAction* dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction;
	QAction* dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction;
	QAction* dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction;
	QAction* dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction;
	QAction* dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction;
	QAction* dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction;
	QAction* dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction;
	QAction* dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction;
	QAction* dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction;
	QAction* dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction;
	QAction* dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction;
	QAction* dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction;
	QAction* dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction;
	QAction* dataTimeConstraintsActivitiesSameStartingDayAction;
	QAction* dataTimeConstraintsTwoActivitiesOrderedAction;
	QAction* dataTimeConstraintsTwoSetsOfActivitiesOrderedAction;
	QAction* dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction;
	QAction* dataTimeConstraintsTeachersMaxHoursContinuouslyAction;
	QAction* dataTimeConstraintsTeacherMaxHoursContinuouslyAction;
	QAction* dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction;
	QAction* dataTimeConstraintsStudentsMaxHoursContinuouslyAction;
	QAction* dataTimeConstraintsActivitiesPreferredStartingTimesAction;
	QAction* dataTimeConstraintsActivityPreferredTimeSlotsAction;
	QAction* dataTimeConstraintsActivityPreferredStartingTimesAction;
	QAction* dataTimeConstraintsMinGapsBetweenActivitiesAction;
	QAction* dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction;
	QAction* dataTimeConstraintsSubactivitiesPreferredStartingTimesAction;
	QAction* dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsActivitiesEndStudentsDayAction;
	QAction* dataTimeConstraintsActivitiesEndTeachersDayAction;
	QAction* dataTimeConstraintsActivitiesBeginStudentsDayAction;
	QAction* dataTimeConstraintsActivitiesBeginTeachersDayAction;

	QAction* dataTimeConstraintsActivitiesBeginOrEndStudentsDayAction;
	QAction* dataTimeConstraintsActivitiesBeginOrEndTeachersDayAction;

	QAction* dataTimeConstraintsTwoActivitiesGroupedAction;
	QAction* dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction;
	QAction* dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction;
	QAction* dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction;
	QAction* dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction;
	QAction* dataSpaceConstraintsActivityTagPreferredRoomAction;
	QAction* dataSpaceConstraintsActivityTagPreferredRoomsAction;
	QAction* dataTimeConstraintsTeachersMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsThreeActivitiesGroupedAction;
	QAction* dataTimeConstraintsMaxDaysBetweenActivitiesAction;
	QAction* dataTimeConstraintsActivitiesMaxHourlySpanAction;
	QAction* dataTimeConstraintsMaxHalfDaysBetweenActivitiesAction;
	QAction* dataTimeConstraintsMaxTermsBetweenActivitiesAction;
	QAction* dataTimeConstraintsTeacherMinDaysPerWeekAction;
	QAction* dataTimeConstraintsTeachersMinDaysPerWeekAction;
	QAction* dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction;
	QAction* dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction;
	QAction* dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction;
	QAction* dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction;
	QAction* dataTimeConstraintsTeacherActivityTagMinHoursDailyAction;
	QAction* dataTimeConstraintsTeachersActivityTagMinHoursDailyAction;
	QAction* dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction;
	QAction* dataTimeConstraintsStudentsActivityTagMinHoursDailyAction;
	QAction* dataTimeConstraintsStudentsSetMaxGapsPerDayAction;
	QAction* dataTimeConstraintsStudentsMaxGapsPerDayAction;
	QAction* dataSpaceConstraintsActivitiesOccupyMaxDifferentRoomsAction;
	QAction* dataSpaceConstraintsActivitiesSameRoomIfConsecutiveAction;
	QAction* dataTimeConstraintsStudentsSetMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsMaxDaysPerWeekAction;

	QAction* dataTimeConstraintsStudentsSetMaxSingleGapsInSelectedTimeSlotsAction;
	QAction* dataTimeConstraintsStudentsMaxSingleGapsInSelectedTimeSlotsAction;

	QAction* dataTimeConstraintsTeacherMaxSingleGapsInSelectedTimeSlotsAction;
	QAction* dataTimeConstraintsTeachersMaxSingleGapsInSelectedTimeSlotsAction;

	//mornings-afternoons
	QAction* dataTimeConstraintsTeacherMaxRealDaysPerWeekAction;
	QAction* dataTimeConstraintsTeacherMaxMorningsPerWeekAction;
	QAction* dataTimeConstraintsTeacherMaxTwoConsecutiveMorningsAction;
	QAction* dataTimeConstraintsTeachersMaxTwoConsecutiveMorningsAction;
	QAction* dataTimeConstraintsTeacherMaxTwoConsecutiveAfternoonsAction;
	QAction* dataTimeConstraintsTeachersMaxTwoConsecutiveAfternoonsAction;
	QAction* dataTimeConstraintsTeacherMaxAfternoonsPerWeekAction;
	QAction* dataTimeConstraintsTeachersMaxHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsTeachersAfternoonsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsTeacherAfternoonsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsStudentsAfternoonsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsTeachersMorningsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsTeacherMorningsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsStudentsMorningsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsStudentsSetMorningsEarlyMaxBeginningsAtSecondHourAction;
	QAction* dataTimeConstraintsTeacherMaxHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsStudentsSetMaxHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsStudentsMaxHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsStudentsMinHoursPerMorningAction;
	QAction* dataTimeConstraintsStudentsSetMinHoursPerMorningAction;
	QAction* dataTimeConstraintsTeachersMinHoursPerMorningAction;

	//2022-09-10
	QAction* dataTimeConstraintsStudentsMinHoursPerAfternoonAction;
	QAction* dataTimeConstraintsStudentsSetMinHoursPerAfternoonAction;
	QAction* dataTimeConstraintsTeachersMinHoursPerAfternoonAction;
	QAction* dataTimeConstraintsTeacherMinHoursPerAfternoonAction;

	QAction* dataTimeConstraintsTeachersMinHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsTeacherMaxHoursPerAllAfternoonsAction;
	QAction* dataTimeConstraintsTeachersMaxHoursPerAllAfternoonsAction;
	QAction* dataTimeConstraintsStudentsSetMaxHoursPerAllAfternoonsAction;
	QAction* dataTimeConstraintsStudentsMaxHoursPerAllAfternoonsAction;
	QAction* dataTimeConstraintsTeacherMinHoursPerMorningAction;
	QAction* dataTimeConstraintsTeacherMinHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsTeachersMaxZeroGapsPerAfternoonAction;
	QAction* dataTimeConstraintsTeacherMaxZeroGapsPerAfternoonAction;
	QAction* dataTimeConstraintsTeacherMaxSpanPerRealDayAction;
	QAction* dataTimeConstraintsTeachersMaxSpanPerRealDayAction;
	QAction* dataTimeConstraintsStudentsSetMaxSpanPerRealDayAction;
	QAction* dataTimeConstraintsStudentsMaxSpanPerRealDayAction;
	QAction* dataTimeConstraintsTeacherMinRestingHoursBetweenMorningAndAfternoonAction;
	QAction* dataTimeConstraintsTeachersMinRestingHoursBetweenMorningAndAfternoonAction;
	QAction* dataTimeConstraintsStudentsSetMinRestingHoursBetweenMorningAndAfternoonAction;
	QAction* dataTimeConstraintsStudentsMinRestingHoursBetweenMorningAndAfternoonAction;
	QAction* dataTimeConstraintsTeacherMorningIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsTeachersMorningIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsTeacherAfternoonIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsTeachersAfternoonIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsSetMorningIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsMorningIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsSetAfternoonIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsAfternoonIntervalMaxDaysPerWeekAction;
	QAction* dataTimeConstraintsTeachersMaxRealDaysPerWeekAction;
	QAction* dataTimeConstraintsTeachersMaxMorningsPerWeekAction;
	QAction* dataTimeConstraintsTeachersMaxAfternoonsPerWeekAction;
	QAction* dataTimeConstraintsTeacherMinRealDaysPerWeekAction;
	QAction* dataTimeConstraintsTeachersMinRealDaysPerWeekAction;
	QAction* dataTimeConstraintsTeacherMinMorningsPerWeekAction;
	QAction* dataTimeConstraintsTeachersMinMorningsPerWeekAction;
	QAction* dataTimeConstraintsTeacherMinAfternoonsPerWeekAction;
	QAction* dataTimeConstraintsTeachersMinAfternoonsPerWeekAction;
	QAction* dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsTeacherMaxActivityTagsPerDayFromSetAction;
	QAction* dataTimeConstraintsTeachersMaxActivityTagsPerDayFromSetAction;
	QAction* dataTimeConstraintsStudentsSetMaxActivityTagsPerDayFromSetAction;
	QAction* dataTimeConstraintsStudentsMaxActivityTagsPerDayFromSetAction;
	QAction* dataTimeConstraintsTeacherMaxActivityTagsPerRealDayFromSetAction;
	QAction* dataTimeConstraintsTeachersMaxActivityTagsPerRealDayFromSetAction;
	QAction* dataTimeConstraintsStudentsSetMaxActivityTagsPerRealDayFromSetAction;
	QAction* dataTimeConstraintsStudentsMaxActivityTagsPerRealDayFromSetAction;

	QAction* dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction;
	QAction* dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction;
	QAction* dataTimeConstraintsStudentsMaxGapsPerRealDayAction;
	QAction* dataTimeConstraintsStudentsSetMaxRealDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsMaxRealDaysPerWeekAction;
	QAction* dataTimeConstraintsStudentsSetMaxMorningsPerWeekAction;
	QAction* dataTimeConstraintsStudentsMaxMorningsPerWeekAction;
	QAction* dataTimeConstraintsStudentsSetMaxAfternoonsPerWeekAction;
	QAction* dataTimeConstraintsStudentsMaxAfternoonsPerWeekAction;
	QAction* dataTimeConstraintsStudentsSetMinMorningsPerWeekAction;
	QAction* dataTimeConstraintsStudentsMinMorningsPerWeekAction;
	QAction* dataTimeConstraintsStudentsSetMinAfternoonsPerWeekAction;
	QAction* dataTimeConstraintsStudentsMinAfternoonsPerWeekAction;
	QAction* dataTimeConstraintsTeacherMaxGapsPerRealDayAction;
	QAction* dataTimeConstraintsTeachersMaxGapsPerRealDayAction;
	QAction* dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction;
	QAction* dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction;
	QAction* dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction;
	QAction* dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction;
	QAction* dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayAction;
	QAction* dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayAction;
	QAction* dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayAction;
	QAction* dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayAction;

	QAction* dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayAction;
	QAction* dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayAction;
	QAction* dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayAction;
	QAction* dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayAction;

	QAction* dataTimeConstraintsTeacherNoTwoConsecutiveDaysAction;
	QAction* dataTimeConstraintsTeachersNoTwoConsecutiveDaysAction;

	QAction* dataTimeConstraintsTeacherMaxThreeConsecutiveDaysAction;
	QAction* dataTimeConstraintsTeachersMaxThreeConsecutiveDaysAction;

	QAction* dataTimeConstraintsStudentsSetMaxThreeConsecutiveDaysAction;
	QAction* dataTimeConstraintsStudentsMaxThreeConsecutiveDaysAction;

	//block-planning
	QAction* dataTimeConstraintsMaxTotalActivitiesFromSetInSelectedTimeSlotsAction;
	QAction* dataTimeConstraintsMaxGapsBetweenActivitiesAction;

	//terms
	QAction* dataTimeConstraintsActivitiesMaxInATermAction;
	QAction* dataTimeConstraintsActivitiesMinInATermAction;
	QAction* dataTimeConstraintsActivitiesOccupyMaxTermsAction;
	
	//2024-02-09
	QAction* dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayInIntervalAction;
	QAction* dataSpaceConstraintsStudentsMaxBuildingChangesPerDayInIntervalAction;
	QAction* dataSpaceConstraintsTeacherMaxBuildingChangesPerDayInIntervalAction;
	QAction* dataSpaceConstraintsTeachersMaxBuildingChangesPerDayInIntervalAction;
	QAction* dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayInIntervalAction;
	QAction* dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayInIntervalAction;
	QAction* dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayInIntervalAction;
	QAction* dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayInIntervalAction;

	//2024-02-19
	QAction* dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayInIntervalAction;
	QAction* dataSpaceConstraintsStudentsMaxRoomChangesPerDayInIntervalAction;
	QAction* dataSpaceConstraintsTeacherMaxRoomChangesPerDayInIntervalAction;
	QAction* dataSpaceConstraintsTeachersMaxRoomChangesPerDayInIntervalAction;
	QAction* dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayInIntervalAction;
	QAction* dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayInIntervalAction;
	QAction* dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayInIntervalAction;
	QAction* dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayInIntervalAction;

	QAction* dataSpaceConstraintsRoomMaxActivityTagsPerDayFromSetAction;
	QAction* dataSpaceConstraintsRoomMaxActivityTagsPerRealDayFromSetAction;
	QAction* dataSpaceConstraintsRoomMaxActivityTagsPerWeekFromSetAction;

	QLabel modeLabel;
	
//	QSpinBox communicationSpinBox;
	
	QMenu* shortcutBasicMenu;
	QMenu* shortcutDataSpaceMenu;
	QMenu* shortcutDataAdvancedMenu;
	QMenu* shortcutAdvancedTimeMenu;
	//2014-07-01
	QMenu* shortcutTimetableLockingMenu;
	QMenu* shortcutTimetableAdvancedMenu;
	
	QNetworkAccessManager* networkManager;
	
	QStringList recentFiles;
	
	QAction* recentFileActions[MAX_RECENT_FILES];
	QAction* recentSeparatorAction;
	
	void setEnabledIcon(QAction* action, bool enabled);
	
	QString strippedName(const QString& fullFileName);
	void updateRecentFileActions();

	void populateLanguagesMap(QMap<QString, QString>& languagesMap);
	
	int currentMode;
	bool dataAvailable;
	bool oldDataAvailable;
	
	bool getLastConfirmation(int newMode, int &ntm, int& nsm, int* nMinMaxDaysModified = nullptr);
	
	void createActionsForConstraints();
	void createMenusOfActionsForConstraints();
	
	void closeOtherWindows();

	bool openHistory();
	bool saveHistory();

	bool fileSave();
	bool fileSaveAs();
	
public:
	FetMainForm();
	~FetMainForm();

	void openFile(const QString& fileName);
	
	void updateMode(bool forceUpdate=false);

	void setCurrentFile(const QString& fileName);

	void retranslateMode();
	void retranslateConstraints();

public Q_SLOTS:
	void settingsOptionalKeyboardShortcutsAction_triggered();

	void settingsAutosaveAction_triggered();

	void restoreDataStateAction_triggered();
	void settingsHistoryMemoryAction_triggered();
	void settingsHistoryDiskAction_triggered();
	
	void modeOfficialAction_triggered();
	void modeMorningsAfternoonsAction_triggered();
	void modeBlockPlanningAction_triggered();
	void modeTermsAction_triggered();
	
	void dataTermsAction_triggered();

	void fileNewAction_triggered();
	void fileSaveAction_triggered();
	void fileSaveAsAction_triggered();
	void fileQuitAction_triggered();
	void fileOpenAction_triggered();
	void fileClearRecentFilesListAction_triggered();
	void openRecentFile();

	void fileImportCSVActivityTagsAction_triggered();
	void fileImportCSVActivitiesAction_triggered();
	void fileImportCSVRoomsBuildingsAction_triggered();
	void fileImportCSVSubjectsAction_triggered();
	void fileImportCSVTeachersAction_triggered();
	void fileImportCSVYearsGroupsSubgroupsAction_triggered();
	void fileExportCSVAction_triggered();
	
	void dataInstitutionNameAction_triggered();
	void dataCommentsAction_triggered();
	void dataDaysAction_triggered();
	void dataHoursAction_triggered();
	void dataTeachersAction_triggered();
	void dataTeachersStatisticsAction_triggered();
	void dataSubjectsAction_triggered();
	void dataSubjectsStatisticsAction_triggered();
	void dataActivityTagsAction_triggered();
	void dataYearsAction_triggered();
	void dataGroupsAction_triggered();
	void dataSubgroupsAction_triggered();
	void dataStudentsStatisticsAction_triggered();
	void dataActivitiesRoomsStatisticsAction_triggered();
	void dataTeachersSubjectsQualificationsStatisticsAction_triggered();
	void dataHelpOnStatisticsAction_triggered();

	void overwriteSingleGenerationFilesToggled(bool checked);

	void checkForUpdatesToggled(bool checked);
	void helpSettingsAction_triggered();
	void showSubgroupsInComboBoxesToggled(bool checked);
	void showSubgroupsInActivityPlanningToggled(bool checked);
	void settingsShowShortcutsOnMainWindowAction_toggled();
	void settingsFontIsUserSelectableAction_toggled();
	void settingsFontAction_triggered();
	void settingsShowToolTipsForConstraintsWithTablesAction_toggled();

	void settingsStyleAndColorSchemeAction_triggered();
	void setCurrentStyle();
	void setCurrentColorScheme();
	
	void timetablesToWriteOnDiskAction_triggered();
	
	void studentsComboBoxesStyleAction_triggered();
	
	void settingsShowVirtualRoomsInTimetablesAction_toggled();

	void settingsCommandAfterFinishingAction_triggered();
	///

	//////confirmations
	void settingsConfirmActivityPlanningAction_toggled();
	void settingsConfirmSpreadActivitiesAction_toggled();
	void settingsConfirmRemoveRedundantAction_toggled();
	void settingsConfirmSaveTimetableAction_toggled();
	void settingsConfirmActivateDeactivateActivitiesConstraintsAction_toggled();
	//////

	void showWarningForSubgroupsWithTheSameActivitiesToggled(bool checked);

	void showWarningForActivitiesNotLockedTimeLockedSpaceVirtualRealRoomsToggled(bool checked);
	
	void showWarningForMaxHoursDailyWithUnder100WeightToggled(bool checked);
	
	void enableGroupActivitiesInInitialOrderToggled(bool checked);
	void showWarningForGroupActivitiesInInitialOrderToggled(bool checked);
	
	void groupActivitiesInInitialOrderAction_triggered();
	
	void dataActivitiesAction_triggered();
	void dataSubactivitiesAction_triggered();
	void dataRoomsAction_triggered();
	void dataBuildingsAction_triggered();
	void dataAllTimeConstraintsAction_triggered();
	void dataAllSpaceConstraintsAction_triggered();

	void dataSpaceConstraintsRoomNotAvailableTimesAction_triggered();
	void dataSpaceConstraintsTeacherRoomNotAvailableTimesAction_triggered();

	void dataSpaceConstraintsBasicCompulsorySpaceAction_triggered();
	void dataSpaceConstraintsActivityPreferredRoomAction_triggered();
	void dataSpaceConstraintsActivityPreferredRoomsAction_triggered();
	
	void dataSpaceConstraintsStudentsSetHomeRoomAction_triggered();
	void dataSpaceConstraintsStudentsSetHomeRoomsAction_triggered();
	void dataSpaceConstraintsTeacherHomeRoomAction_triggered();
	void dataSpaceConstraintsTeacherHomeRoomsAction_triggered();

	void dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayAction_triggered();
	void dataSpaceConstraintsStudentsMaxBuildingChangesPerDayAction_triggered();
	void dataSpaceConstraintsStudentsSetMaxBuildingChangesPerWeekAction_triggered();
	void dataSpaceConstraintsStudentsMaxBuildingChangesPerWeekAction_triggered();
	void dataSpaceConstraintsStudentsSetMinGapsBetweenBuildingChangesAction_triggered();
	void dataSpaceConstraintsStudentsMinGapsBetweenBuildingChangesAction_triggered();

	void dataSpaceConstraintsTeacherMaxBuildingChangesPerDayAction_triggered();
	void dataSpaceConstraintsTeachersMaxBuildingChangesPerDayAction_triggered();
	void dataSpaceConstraintsTeacherMaxBuildingChangesPerWeekAction_triggered();
	void dataSpaceConstraintsTeachersMaxBuildingChangesPerWeekAction_triggered();
	void dataSpaceConstraintsTeacherMinGapsBetweenBuildingChangesAction_triggered();
	void dataSpaceConstraintsTeachersMinGapsBetweenBuildingChangesAction_triggered();
	
	void dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayAction_triggered();
	void dataSpaceConstraintsStudentsMaxRoomChangesPerDayAction_triggered();
	void dataSpaceConstraintsStudentsSetMaxRoomChangesPerWeekAction_triggered();
	void dataSpaceConstraintsStudentsMaxRoomChangesPerWeekAction_triggered();
	void dataSpaceConstraintsStudentsSetMinGapsBetweenRoomChangesAction_triggered();
	void dataSpaceConstraintsStudentsMinGapsBetweenRoomChangesAction_triggered();

	void dataSpaceConstraintsTeacherMaxRoomChangesPerDayAction_triggered();
	void dataSpaceConstraintsTeachersMaxRoomChangesPerDayAction_triggered();
	void dataSpaceConstraintsTeacherMaxRoomChangesPerWeekAction_triggered();
	void dataSpaceConstraintsTeachersMaxRoomChangesPerWeekAction_triggered();
	void dataSpaceConstraintsTeacherMinGapsBetweenRoomChangesAction_triggered();
	void dataSpaceConstraintsTeachersMinGapsBetweenRoomChangesAction_triggered();
	
	void dataSpaceConstraintsSubjectPreferredRoomAction_triggered();
	void dataSpaceConstraintsSubjectPreferredRoomsAction_triggered();
	void dataSpaceConstraintsSubjectActivityTagPreferredRoomAction_triggered();
	void dataSpaceConstraintsSubjectActivityTagPreferredRoomsAction_triggered();

	void dataSpaceConstraintsActivityTagPreferredRoomAction_triggered();
	void dataSpaceConstraintsActivityTagPreferredRoomsAction_triggered();

	void dataSpaceConstraintsActivitiesOccupyMaxDifferentRoomsAction_triggered();
	void dataSpaceConstraintsActivitiesSameRoomIfConsecutiveAction_triggered();

	void dataTimeConstraintsBasicCompulsoryTimeAction_triggered();
	void dataTimeConstraintsBreakTimesAction_triggered();

	void dataTimeConstraintsTwoActivitiesConsecutiveAction_triggered();
	void dataTimeConstraintsTwoActivitiesGroupedAction_triggered();
	void dataTimeConstraintsThreeActivitiesGroupedAction_triggered();
	void dataTimeConstraintsTwoActivitiesOrderedAction_triggered();
	void dataTimeConstraintsTwoSetsOfActivitiesOrderedAction_triggered();
	void dataTimeConstraintsTwoActivitiesOrderedIfSameDayAction_triggered();
	void dataTimeConstraintsActivityPreferredStartingTimeAction_triggered();
	void dataTimeConstraintsActivityPreferredTimeSlotsAction_triggered();
	void dataTimeConstraintsActivitiesPreferredTimeSlotsAction_triggered();
	void dataTimeConstraintsSubactivitiesPreferredTimeSlotsAction_triggered();
	void dataTimeConstraintsActivityPreferredStartingTimesAction_triggered();
	void dataTimeConstraintsActivitiesPreferredStartingTimesAction_triggered();
	void dataTimeConstraintsSubactivitiesPreferredStartingTimesAction_triggered();
	void dataTimeConstraintsActivitiesSameStartingTimeAction_triggered();
	void dataTimeConstraintsActivitiesSameStartingHourAction_triggered();
	void dataTimeConstraintsActivitiesSameStartingDayAction_triggered();

	void dataTimeConstraintsTwoSetsOfActivitiesSameSectionsAction_triggered();

	void dataTimeConstraintsActivitiesMaxTotalNumberOfStudentsInSelectedTimeSlotsAction_triggered();
	
	void dataTimeConstraintsActivitiesOccupyMaxTimeSlotsFromSelectionAction_triggered();
	void dataTimeConstraintsActivitiesOccupyMinTimeSlotsFromSelectionAction_triggered();
	void dataTimeConstraintsActivitiesMaxSimultaneousInSelectedTimeSlotsAction_triggered();
	void dataTimeConstraintsActivitiesMinSimultaneousInSelectedTimeSlotsAction_triggered();
	void dataTimeConstraintsActivitiesNotOverlappingAction_triggered();
	void dataTimeConstraintsActivityTagsNotOverlappingAction_triggered();
	void dataTimeConstraintsMinDaysBetweenActivitiesAction_triggered();
	void dataTimeConstraintsMinHalfDaysBetweenActivitiesAction_triggered();
	void dataTimeConstraintsMaxDaysBetweenActivitiesAction_triggered();
	void dataTimeConstraintsActivitiesMaxHourlySpanAction_triggered();
	void dataTimeConstraintsMaxHalfDaysBetweenActivitiesAction_triggered();
	void dataTimeConstraintsMaxTermsBetweenActivitiesAction_triggered();
	void dataTimeConstraintsMinGapsBetweenActivitiesAction_triggered();

	void dataTimeConstraintsActivityEndsStudentsDayAction_triggered();
	void dataTimeConstraintsActivitiesEndStudentsDayAction_triggered();

	void dataTimeConstraintsActivityEndsTeachersDayAction_triggered();
	void dataTimeConstraintsActivitiesEndTeachersDayAction_triggered();

	void dataTimeConstraintsActivityBeginsStudentsDayAction_triggered();
	void dataTimeConstraintsActivitiesBeginStudentsDayAction_triggered();

	void dataTimeConstraintsActivityBeginsTeachersDayAction_triggered();
	void dataTimeConstraintsActivitiesBeginTeachersDayAction_triggered();

	void dataTimeConstraintsActivityBeginsOrEndsStudentsDayAction_triggered();
	void dataTimeConstraintsActivitiesBeginOrEndStudentsDayAction_triggered();

	void dataTimeConstraintsActivityBeginsOrEndsTeachersDayAction_triggered();
	void dataTimeConstraintsActivitiesBeginOrEndTeachersDayAction_triggered();

	void dataTimeConstraintsTeacherNotAvailableTimesAction_triggered();
	void dataTimeConstraintsTeachersNotAvailableTimesAction_triggered();
	void dataTimeConstraintsTeacherMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMaxDaysPerWeekAction_triggered();

	void dataTimeConstraintsTeacherMaxHoursPerTermAction_triggered();
	void dataTimeConstraintsTeachersMaxHoursPerTermAction_triggered();

	void dataTimeConstraintsTeacherMinDaysPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMinDaysPerWeekAction_triggered();

	void dataTimeConstraintsTeachersMaxHoursDailyAction_triggered();
	void dataTimeConstraintsTeacherMaxHoursDailyAction_triggered();
	void dataTimeConstraintsTeachersMaxHoursContinuouslyAction_triggered();
	void dataTimeConstraintsTeacherMaxHoursContinuouslyAction_triggered();

	void dataTimeConstraintsTeachersMaxHoursDailyInIntervalAction_triggered();
	void dataTimeConstraintsTeacherMaxHoursDailyInIntervalAction_triggered();
	void dataTimeConstraintsStudentsMaxHoursDailyInIntervalAction_triggered();
	void dataTimeConstraintsStudentsSetMaxHoursDailyInIntervalAction_triggered();

	void dataTimeConstraintsTeacherPairOfMutuallyExclusiveTimeSlotsAction_triggered();
	void dataTimeConstraintsTeachersPairOfMutuallyExclusiveTimeSlotsAction_triggered();
	void dataTimeConstraintsStudentsSetPairOfMutuallyExclusiveTimeSlotsAction_triggered();
	void dataTimeConstraintsStudentsPairOfMutuallyExclusiveTimeSlotsAction_triggered();

	void dataTimeConstraintsTeacherPairOfMutuallyExclusiveSetsOfTimeSlotsAction_triggered();
	void dataTimeConstraintsTeachersPairOfMutuallyExclusiveSetsOfTimeSlotsAction_triggered();
	void dataTimeConstraintsStudentsSetPairOfMutuallyExclusiveSetsOfTimeSlotsAction_triggered();
	void dataTimeConstraintsStudentsPairOfMutuallyExclusiveSetsOfTimeSlotsAction_triggered();

	void dataTimeConstraintsTeacherOccupiesMaxSetsOfTimeSlotsFromSelectionAction_triggered();
	void dataTimeConstraintsTeachersOccupyMaxSetsOfTimeSlotsFromSelectionAction_triggered();

	void dataTimeConstraintsStudentsSetOccupiesMaxSetsOfTimeSlotsFromSelectionAction_triggered();
	void dataTimeConstraintsStudentsOccupyMaxSetsOfTimeSlotsFromSelectionAction_triggered();

	void dataTimeConstraintsActivitiesOccupyMaxSetsOfTimeSlotsFromSelectionAction_triggered();

	void dataTimeConstraintsActivitiesPairOfMutuallyExclusiveTimeSlotsAction_triggered();
	void dataTimeConstraintsActivitiesPairOfMutuallyExclusiveSetsOfTimeSlotsAction_triggered();

	void dataTimeConstraintsActivitiesOverlapCompletelyOrDoNotOverlapAction_triggered();

	void dataTimeConstraintsTeachersActivityTagMaxHoursContinuouslyAction_triggered();
	void dataTimeConstraintsTeacherActivityTagMaxHoursContinuouslyAction_triggered();

	void dataTimeConstraintsTeachersActivityTagMaxHoursDailyAction_triggered();
	void dataTimeConstraintsTeacherActivityTagMaxHoursDailyAction_triggered();

	void dataTimeConstraintsTeachersActivityTagMinHoursDailyAction_triggered();
	void dataTimeConstraintsTeacherActivityTagMinHoursDailyAction_triggered();

	void dataTimeConstraintsTeachersMinHoursDailyAction_triggered();
	void dataTimeConstraintsTeacherMinHoursDailyAction_triggered();
	void dataTimeConstraintsTeachersMaxGapsPerWeekAction_triggered();
	void dataTimeConstraintsTeacherMaxGapsPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMaxGapsPerDayAction_triggered();
	void dataTimeConstraintsTeacherMaxGapsPerDayAction_triggered();
	void dataTimeConstraintsTeachersMaxGapsPerMorningAndAfternoonAction_triggered();
	void dataTimeConstraintsTeacherMaxGapsPerMorningAndAfternoonAction_triggered();
	
	void dataTimeConstraintsTeacherIntervalMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsTeachersIntervalMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsStudentsSetIntervalMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsStudentsIntervalMaxDaysPerWeekAction_triggered();

	void dataTimeConstraintsStudentsSetMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsStudentsMaxDaysPerWeekAction_triggered();

	void dataTimeConstraintsStudentsSetNotAvailableTimesAction_triggered();
	void dataTimeConstraintsStudentsNotAvailableTimesAction_triggered();
	void dataTimeConstraintsStudentsSetMaxGapsPerWeekAction_triggered();
	void dataTimeConstraintsStudentsMaxGapsPerWeekAction_triggered();

	void dataTimeConstraintsStudentsSetMaxGapsPerDayAction_triggered();
	void dataTimeConstraintsStudentsMaxGapsPerDayAction_triggered();

	void dataTimeConstraintsStudentsSetMaxSingleGapsInSelectedTimeSlotsAction_triggered();
	void dataTimeConstraintsStudentsMaxSingleGapsInSelectedTimeSlotsAction_triggered();

	void dataTimeConstraintsTeacherMaxSingleGapsInSelectedTimeSlotsAction_triggered();
	void dataTimeConstraintsTeachersMaxSingleGapsInSelectedTimeSlotsAction_triggered();

	void dataTimeConstraintsStudentsEarlyMaxBeginningsAtSecondHourAction_triggered();
	void dataTimeConstraintsStudentsSetEarlyMaxBeginningsAtSecondHourAction_triggered();
	void dataTimeConstraintsStudentsMaxHoursDailyAction_triggered();
	void dataTimeConstraintsStudentsSetMaxHoursDailyAction_triggered();
	void dataTimeConstraintsStudentsMaxHoursContinuouslyAction_triggered();
	void dataTimeConstraintsStudentsSetMaxHoursContinuouslyAction_triggered();

	void dataTimeConstraintsStudentsActivityTagMaxHoursContinuouslyAction_triggered();
	void dataTimeConstraintsStudentsSetActivityTagMaxHoursContinuouslyAction_triggered();

	void dataTimeConstraintsStudentsActivityTagMaxHoursDailyAction_triggered();
	void dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyAction_triggered();

	void dataTimeConstraintsStudentsActivityTagMinHoursDailyAction_triggered();
	void dataTimeConstraintsStudentsSetActivityTagMinHoursDailyAction_triggered();

	void dataTimeConstraintsStudentsMinHoursDailyAction_triggered();
	void dataTimeConstraintsStudentsSetMinHoursDailyAction_triggered();

	void dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsAction_triggered();
	void dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsAction_triggered();
	void dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsAction_triggered();
	void dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsAction_triggered();

	//2021-12-15
	void dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagAction_triggered();
	void dataTimeConstraintsStudentsMinGapsBetweenActivityTagAction_triggered();
	void dataTimeConstraintsTeacherMinGapsBetweenActivityTagAction_triggered();
	void dataTimeConstraintsTeachersMinGapsBetweenActivityTagAction_triggered();

	//2024-03-16
	void dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction_triggered();
	void dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction_triggered();
	void dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction_triggered();
	void dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsPerRealDayAction_triggered();

	void dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagPerRealDayAction_triggered();
	void dataTimeConstraintsStudentsMinGapsBetweenActivityTagPerRealDayAction_triggered();
	void dataTimeConstraintsTeacherMinGapsBetweenActivityTagPerRealDayAction_triggered();
	void dataTimeConstraintsTeachersMinGapsBetweenActivityTagPerRealDayAction_triggered();

	//2024-05-20
	void dataTimeConstraintsStudentsSetMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction_triggered();
	void dataTimeConstraintsStudentsMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction_triggered();
	void dataTimeConstraintsTeacherMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction_triggered();
	void dataTimeConstraintsTeachersMinGapsBetweenOrderedPairOfActivityTagsBetweenMorningAndAfternoonAction_triggered();

	void dataTimeConstraintsStudentsSetMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction_triggered();
	void dataTimeConstraintsStudentsMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction_triggered();
	void dataTimeConstraintsTeacherMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction_triggered();
	void dataTimeConstraintsTeachersMinGapsBetweenActivityTagBetweenMorningAndAfternoonAction_triggered();

	void dataTimeConstraintsTeacherMaxSpanPerDayAction_triggered();
	void dataTimeConstraintsTeachersMaxSpanPerDayAction_triggered();
	void dataTimeConstraintsStudentsSetMaxSpanPerDayAction_triggered();
	void dataTimeConstraintsStudentsMaxSpanPerDayAction_triggered();

	void dataTimeConstraintsTeacherMinRestingHoursAction_triggered();
	void dataTimeConstraintsTeachersMinRestingHoursAction_triggered();
	void dataTimeConstraintsStudentsSetMinRestingHoursAction_triggered();
	void dataTimeConstraintsStudentsMinRestingHoursAction_triggered();

	//mornings-afternoons
	void dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayAction_triggered();
	void dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayAction_triggered();
	void dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayAction_triggered();
	void dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayAction_triggered();

	void dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayAction_triggered();
	void dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayAction_triggered();
	void dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayAction_triggered();
	void dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayAction_triggered();

	void dataTimeConstraintsTeacherMaxRealDaysPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMaxRealDaysPerWeekAction_triggered();

	void dataTimeConstraintsTeacherMaxAfternoonsPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMaxAfternoonsPerWeekAction_triggered();
	void dataTimeConstraintsTeacherMaxMorningsPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMaxMorningsPerWeekAction_triggered();

	void dataTimeConstraintsTeacherMaxTwoConsecutiveMorningsAction_triggered();
	void dataTimeConstraintsTeachersMaxTwoConsecutiveMorningsAction_triggered();
	void dataTimeConstraintsTeacherMaxTwoConsecutiveAfternoonsAction_triggered();
	void dataTimeConstraintsTeachersMaxTwoConsecutiveAfternoonsAction_triggered();

	void dataTimeConstraintsTeacherMinRealDaysPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMinRealDaysPerWeekAction_triggered();

	void dataTimeConstraintsTeacherMinMorningsPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMinMorningsPerWeekAction_triggered();
	void dataTimeConstraintsTeacherMinAfternoonsPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMinAfternoonsPerWeekAction_triggered();

	void dataTimeConstraintsTeachersMaxHoursDailyRealDaysAction_triggered();
	void dataTimeConstraintsTeacherMaxHoursDailyRealDaysAction_triggered();

	void dataTimeConstraintsTeacherMaxActivityTagsPerDayFromSetAction_triggered();
	void dataTimeConstraintsTeachersMaxActivityTagsPerDayFromSetAction_triggered();

	void dataTimeConstraintsStudentsSetMaxActivityTagsPerDayFromSetAction_triggered();
	void dataTimeConstraintsStudentsMaxActivityTagsPerDayFromSetAction_triggered();

	void dataTimeConstraintsTeacherMaxActivityTagsPerRealDayFromSetAction_triggered();
	void dataTimeConstraintsTeachersMaxActivityTagsPerRealDayFromSetAction_triggered();

	void dataTimeConstraintsStudentsSetMaxActivityTagsPerRealDayFromSetAction_triggered();
	void dataTimeConstraintsStudentsMaxActivityTagsPerRealDayFromSetAction_triggered();

	void dataTimeConstraintsTeachersActivityTagMaxHoursDailyRealDaysAction_triggered();
	void dataTimeConstraintsTeacherActivityTagMaxHoursDailyRealDaysAction_triggered();

	void dataTimeConstraintsTeacherMaxHoursPerAllAfternoonsAction_triggered();
	void dataTimeConstraintsTeachersMaxHoursPerAllAfternoonsAction_triggered();

	void dataTimeConstraintsStudentsSetMaxHoursPerAllAfternoonsAction_triggered();
	void dataTimeConstraintsStudentsMaxHoursPerAllAfternoonsAction_triggered();

	void dataTimeConstraintsTeachersMinHoursPerMorningAction_triggered();
	void dataTimeConstraintsTeacherMinHoursPerMorningAction_triggered();

	void dataTimeConstraintsTeachersMinHoursPerAfternoonAction_triggered();
	void dataTimeConstraintsTeacherMinHoursPerAfternoonAction_triggered();

	void dataTimeConstraintsTeachersMinHoursDailyRealDaysAction_triggered();
	void dataTimeConstraintsTeacherMinHoursDailyRealDaysAction_triggered();

	void dataTimeConstraintsTeachersMaxZeroGapsPerAfternoonAction_triggered();
	void dataTimeConstraintsTeacherMaxZeroGapsPerAfternoonAction_triggered();

	void dataTimeConstraintsTeachersMaxGapsPerRealDayAction_triggered();
	void dataTimeConstraintsTeacherMaxGapsPerRealDayAction_triggered();

	void dataTimeConstraintsTeacherMorningIntervalMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsTeachersMorningIntervalMaxDaysPerWeekAction_triggered();

	void dataTimeConstraintsTeacherAfternoonIntervalMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsTeachersAfternoonIntervalMaxDaysPerWeekAction_triggered();

	void dataTimeConstraintsStudentsSetMorningIntervalMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsStudentsMorningIntervalMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsStudentsSetAfternoonIntervalMaxDaysPerWeekAction_triggered();
	void dataTimeConstraintsStudentsAfternoonIntervalMaxDaysPerWeekAction_triggered();

	void dataTimeConstraintsStudentsSetMaxRealDaysPerWeekAction_triggered();
	void dataTimeConstraintsStudentsMaxRealDaysPerWeekAction_triggered();

	void dataTimeConstraintsStudentsSetMaxMorningsPerWeekAction_triggered();
	void dataTimeConstraintsStudentsMaxMorningsPerWeekAction_triggered();
	void dataTimeConstraintsStudentsSetMaxAfternoonsPerWeekAction_triggered();
	void dataTimeConstraintsStudentsMaxAfternoonsPerWeekAction_triggered();

	void dataTimeConstraintsStudentsSetMinMorningsPerWeekAction_triggered();
	void dataTimeConstraintsStudentsMinMorningsPerWeekAction_triggered();
	void dataTimeConstraintsStudentsSetMinAfternoonsPerWeekAction_triggered();
	void dataTimeConstraintsStudentsMinAfternoonsPerWeekAction_triggered();

	void dataTimeConstraintsStudentsSetMaxGapsPerRealDayAction_triggered();
	void dataTimeConstraintsStudentsMaxGapsPerRealDayAction_triggered();

	void dataTimeConstraintsTeachersMaxGapsPerWeekForRealDaysAction_triggered();
	void dataTimeConstraintsTeacherMaxGapsPerWeekForRealDaysAction_triggered();
	void dataTimeConstraintsStudentsSetMaxGapsPerWeekForRealDaysAction_triggered();
	void dataTimeConstraintsStudentsMaxGapsPerWeekForRealDaysAction_triggered();

	void dataTimeConstraintsTeachersAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered();
	void dataTimeConstraintsTeacherAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered();

	void dataTimeConstraintsStudentsAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered();
	void dataTimeConstraintsStudentsSetAfternoonsEarlyMaxBeginningsAtSecondHourAction_triggered();

	void dataTimeConstraintsTeachersMorningsEarlyMaxBeginningsAtSecondHourAction_triggered();
	void dataTimeConstraintsTeacherMorningsEarlyMaxBeginningsAtSecondHourAction_triggered();

	void dataTimeConstraintsStudentsMorningsEarlyMaxBeginningsAtSecondHourAction_triggered();
	void dataTimeConstraintsStudentsSetMorningsEarlyMaxBeginningsAtSecondHourAction_triggered();

	void dataTimeConstraintsStudentsMaxHoursDailyRealDaysAction_triggered();
	void dataTimeConstraintsStudentsSetMaxHoursDailyRealDaysAction_triggered();

	void dataTimeConstraintsStudentsActivityTagMaxHoursDailyRealDaysAction_triggered();
	void dataTimeConstraintsStudentsSetActivityTagMaxHoursDailyRealDaysAction_triggered();

	void dataTimeConstraintsStudentsMinHoursPerMorningAction_triggered();
	void dataTimeConstraintsStudentsSetMinHoursPerMorningAction_triggered();

	void dataTimeConstraintsStudentsMinHoursPerAfternoonAction_triggered();
	void dataTimeConstraintsStudentsSetMinHoursPerAfternoonAction_triggered();

	void dataTimeConstraintsTeacherMaxSpanPerRealDayAction_triggered();
	void dataTimeConstraintsTeachersMaxSpanPerRealDayAction_triggered();
	void dataTimeConstraintsStudentsSetMaxSpanPerRealDayAction_triggered();
	void dataTimeConstraintsStudentsMaxSpanPerRealDayAction_triggered();

	void dataTimeConstraintsTeacherMinRestingHoursBetweenMorningAndAfternoonAction_triggered();
	void dataTimeConstraintsTeachersMinRestingHoursBetweenMorningAndAfternoonAction_triggered();
	void dataTimeConstraintsStudentsSetMinRestingHoursBetweenMorningAndAfternoonAction_triggered();
	void dataTimeConstraintsStudentsMinRestingHoursBetweenMorningAndAfternoonAction_triggered();

	void dataTimeConstraintsTeacherNoTwoConsecutiveDaysAction_triggered();
	void dataTimeConstraintsTeachersNoTwoConsecutiveDaysAction_triggered();

	void dataTimeConstraintsTeacherMaxThreeConsecutiveDaysAction_triggered();
	void dataTimeConstraintsTeachersMaxThreeConsecutiveDaysAction_triggered();

	void dataTimeConstraintsStudentsSetMaxThreeConsecutiveDaysAction_triggered();
	void dataTimeConstraintsStudentsMaxThreeConsecutiveDaysAction_triggered();

	//block-planning
	void dataTimeConstraintsMaxTotalActivitiesFromSetInSelectedTimeSlotsAction_triggered();
	void dataTimeConstraintsMaxGapsBetweenActivitiesAction_triggered();

	//terms
	void dataTimeConstraintsActivitiesMaxInATermAction_triggered();
	void dataTimeConstraintsActivitiesMinInATermAction_triggered();
	void dataTimeConstraintsActivitiesOccupyMaxTermsAction_triggered();

	//2024-02-09
	void dataSpaceConstraintsStudentsSetMaxBuildingChangesPerDayInIntervalAction_triggered();
	void dataSpaceConstraintsStudentsMaxBuildingChangesPerDayInIntervalAction_triggered();
	void dataSpaceConstraintsTeacherMaxBuildingChangesPerDayInIntervalAction_triggered();
	void dataSpaceConstraintsTeachersMaxBuildingChangesPerDayInIntervalAction_triggered();
	void dataSpaceConstraintsStudentsSetMaxBuildingChangesPerRealDayInIntervalAction_triggered();
	void dataSpaceConstraintsStudentsMaxBuildingChangesPerRealDayInIntervalAction_triggered();
	void dataSpaceConstraintsTeacherMaxBuildingChangesPerRealDayInIntervalAction_triggered();
	void dataSpaceConstraintsTeachersMaxBuildingChangesPerRealDayInIntervalAction_triggered();

	//2024-02-19
	void dataSpaceConstraintsStudentsSetMaxRoomChangesPerDayInIntervalAction_triggered();
	void dataSpaceConstraintsStudentsMaxRoomChangesPerDayInIntervalAction_triggered();
	void dataSpaceConstraintsTeacherMaxRoomChangesPerDayInIntervalAction_triggered();
	void dataSpaceConstraintsTeachersMaxRoomChangesPerDayInIntervalAction_triggered();
	void dataSpaceConstraintsStudentsSetMaxRoomChangesPerRealDayInIntervalAction_triggered();
	void dataSpaceConstraintsStudentsMaxRoomChangesPerRealDayInIntervalAction_triggered();
	void dataSpaceConstraintsTeacherMaxRoomChangesPerRealDayInIntervalAction_triggered();
	void dataSpaceConstraintsTeachersMaxRoomChangesPerRealDayInIntervalAction_triggered();

	void dataSpaceConstraintsRoomMaxActivityTagsPerDayFromSetAction_triggered();
	void dataSpaceConstraintsRoomMaxActivityTagsPerRealDayFromSetAction_triggered();
	void dataSpaceConstraintsRoomMaxActivityTagsPerWeekFromSetAction_triggered();

	void helpMoroccoAction_triggered();
	void helpAlgeriaAction_triggered();
	void helpBlockPlanningAction_triggered();
	void helpTermsAction_triggered();

	void activityPlanningAction_triggered();
	void spreadActivitiesAction_triggered();
	void removeRedundantConstraintsAction_triggered();

	//about
	void helpAboutFETAction_triggered();
	void helpAboutQtAction_triggered();
	void helpAboutLibrariesAction_triggered();
	//offline
	void helpFAQAction_triggered();
	void helpTipsAction_triggered();
	void helpInstructionsAction_triggered();
	//online
	void helpHomepageAction_triggered();
	void helpContentsAction_triggered();
	void helpForumAction_triggered();
	void helpAddressesAction_triggered();

	void timetableGenerateAction_triggered();
	void timetableViewStudentsDaysHorizontalAction_triggered();
	void timetableViewStudentsDaysVerticalAction_triggered();
	void timetableViewStudentsTimeHorizontalAction_triggered();
	void timetableViewTeachersDaysHorizontalAction_triggered();
	void timetableViewTeachersDaysVerticalAction_triggered();
	void timetableViewTeachersTimeHorizontalAction_triggered();
	void timetableViewRoomsDaysHorizontalAction_triggered();
	void timetableViewRoomsDaysVerticalAction_triggered();
	void timetableViewRoomsTimeHorizontalAction_triggered();
	void timetableShowConflictsAction_triggered();
	void timetablePrintAction_triggered();
	void timetableGenerateMultipleAction_triggered();

	void timetableLockAllActivitiesAction_triggered();
	void timetableUnlockAllActivitiesAction_triggered();
	void timetableLockActivitiesDayAction_triggered();
	void timetableUnlockActivitiesDayAction_triggered();
	void timetableLockActivitiesEndStudentsDayAction_triggered();
	void timetableUnlockActivitiesEndStudentsDayAction_triggered();
	void timetableLockActivitiesWithASpecifiedActivityTagAction_triggered();
	void timetableUnlockActivitiesWithASpecifiedActivityTagAction_triggered();
	///
	void timetableLockActivitiesWithAdvancedFilterAction_triggered();
	void timetableUnlockActivitiesWithAdvancedFilterAction_triggered();

	void timetableSaveTimetableAsAction_triggered();

	void randomSeedAction_triggered();
	
	void languageAction_triggered();
	
	void settingsDivideTimetablesByDaysAction_toggled();
	void settingsDuplicateVerticalNamesAction_toggled();
	
	void settingsRestoreDefaultsAction_triggered();
	
	void settingsTimetableHtmlLevelAction_triggered();
	
	void settingsDataToPrintInTimetablesAction_triggered();
	
	void settingsOrderSubgroupsInTimetablesAction_toggled();
	void settingsPrintDetailedTimetablesAction_toggled();
	void settingsPrintDetailedTeachersFreePeriodsTimetablesAction_toggled();
	void settingsPrintNotAvailableSlotsAction_toggled();
	void settingsPrintBreakSlotsAction_toggled();
	
	void settingsPrintActivitiesWithSameStartingTimeAction_toggled();
	
	void selectOutputDirAction_triggered();
	
	void statisticsExportToDiskAction_triggered();
	void statisticsPrintAction_triggered();
	
	void shortcutAllTimeConstraintsPushButton_clicked();
	void shortcutBreakTimeConstraintsPushButton_clicked();
	void shortcutTeachersTimeConstraintsPushButton_clicked();
	void shortcutStudentsTimeConstraintsPushButton_clicked();
	void shortcutActivitiesTimeConstraintsPushButton_clicked();
	void shortcutAdvancedTimeConstraintsPushButton_clicked();
	
	void shortcutAllSpaceConstraintsPushButton_clicked();
	void shortcutRoomsSpaceConstraintsPushButton_clicked();
	void shortcutTeachersSpaceConstraintsPushButton_clicked();
	void shortcutStudentsSpaceConstraintsPushButton_clicked();
	void shortcutSubjectsSpaceConstraintsPushButton_clicked();
	void shortcutActivityTagsSpaceConstraintsPushButton_clicked();
	void shortcutSubjectsAndActivityTagsSpaceConstraintsPushButton_clicked();
	void shortcutActivitiesSpaceConstraintsPushButton_clicked();
	
	void shortcutGeneratePushButton_clicked();
	void shortcutGenerateMultiplePushButton_clicked();
	void shortcutViewTeachersPushButton_clicked();
	void shortcutViewStudentsPushButton_clicked();
	void shortcutViewRoomsPushButton_clicked();
	void shortcutShowSoftConflictsPushButton_clicked();
	//2014-07-01
	void shortcutsTimetableAdvancedPushButton_clicked();
	void shortcutsTimetablePrintPushButton_clicked();
	void shortcutsTimetableLockingPushButton_clicked();
	
	void shortcutBasicPushButton_clicked();
	void shortcutSubjectsPushButton_clicked();
	void shortcutActivityTagsPushButton_clicked();
	void shortcutTeachersPushButton_clicked();
	void shortcutStudentsPushButton_clicked();
	void shortcutActivitiesPushButton_clicked();
	void shortcutSubactivitiesPushButton_clicked();
	void shortcutDataAdvancedPushButton_clicked();
	void shortcutDataSpacePushButton_clicked();

	void shortcutOpenPushButton_clicked();
	void shortcutOpenRecentPushButton_clicked();
	void shortcutNewPushButton_clicked();
	void shortcutSavePushButton_clicked();
	void shortcutSaveAsPushButton_clicked();
	
	void replyFinished(QNetworkReply* networkReply);
	
protected:
	void closeEvent(QCloseEvent* event);
};

#endif
