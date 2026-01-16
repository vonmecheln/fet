/***************************************************************************
                          helpfaqform.cpp  -  description
                             -------------------
    begin                : Feb 20, 2005
    copyright            : (C) 2005 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "helpfaqform.h"

#include "timetable_defs.h"

HelpFaqForm::HelpFaqForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);
	
	plainTextEdit->setReadOnly(true);

	connect(closePushButton, &QPushButton::clicked, this, &HelpFaqForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	setText();
}

HelpFaqForm::~HelpFaqForm()
{
	saveFETDialogGeometry(this);
}

void HelpFaqForm::setText()
{
	QString s;
	
	s+=tr("Frequently asked questions.");
	s+="\n\n";
	s+=tr("Last modified on %1.").arg(tr("20 March 2021"));
	
	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: What is the organization of FET input data?\n\n"
		"A: - Students - organized into sets (years (or forms, or classes), containing groups, containing subgroups)."
		"\n"
		"- Teachers."
		"\n"
		"- Subjects (the names of the possible courses, eg. math, physics, etc.)."
		"\n"
		"- Activity tags (you can use them or not, option is yours: the type of activity: lab, course, seminary, or any other information attached to an activity)."
		"\n"
		"- Rooms (classrooms)."
		"\n"
		"- Buildings."
		"\n"
		"- Activities: a coupling of one or more teachers, a subject and one or more students set. This is usually named a "
		"course, a lecture, a laboratory and so on. An activity can have optionally an activity tag, to help you with some constraints."
		"\n"
		"- Constraints. They can be: time constraints (referring to the allocated day and hour) or space constraints "
		"(referring to rooms allocation). They have a weight percentage, from 0.0% to 100.0%. 100% means that the "
		"constraint will always be respected and if this constraint is impossible, FET will not be able to generate a timetable.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: What are the maximum limits FET can handle?");
	s+="\n\n";
	s+=tr("A: There are indeed maximum limits for the generation algorithm, but probably nobody will ever need larger values.");
	s+="\n\n";
	s+=tr("These limits are:");
	
	s+="\n- ";
	s+=tr("Maximum number of working days per week: %1").arg(MAX_DAYS_PER_WEEK);
	s+="\n- ";
	s+=tr("Maximum number of hours per day: %1").arg(MAX_HOURS_PER_DAY);
	s+="\n- ";
	s+=tr("Virtually unlimited number of teachers");
	s+="\n- ";
	s+=tr("Maximum number of subgroups of students: %1").arg(MAX_TOTAL_SUBGROUPS);
	s+=" (";
	s+=tr("This limitation is added just to prevent the users to add too many subgroups, which might slow down the generation much more than necessary -"
	 " but it can be lifted very easily by modifying the code.");
	s+=")\n- ";
	s+=tr("Virtually unlimited number of subjects");
	s+="\n- ";
	s+=tr("Virtually unlimited number of activity tags");
	s+="\n- ";
	s+=tr("Maximum number of activities: %1").arg(MAX_ACTIVITIES);
	s+="\n- ";
	s+=tr("Maximum number of rooms: %1").arg(MAX_ROOMS);
	s+="\n- ";
	s+=tr("Maximum number of buildings: %1").arg(MAX_BUILDINGS);
	s+="\n- ";
	s+=tr("Virtually unlimited number of teachers and students sets for each activity");
	s+="\n- ";
	s+=tr("Virtually unlimited number of time constraints");
	s+="\n- ";
	s+=tr("Virtually unlimited number of space constraints");
	
	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: Why some activities appear indented while others do not?\n\n"
		"A: The activities are indented for easier visualization. The non-indented ones are the representatives of a "
		"larger split activity (their id is the group id of the larger split activity), while indented ones are "
		"the other components of this larger split activity.");
		
	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: Is it possible to use non-integer weights for constraints?\n\n"
		"A: Yes. If a constraint is allowed values under 100%, you can use any weight, even fractional numbers like 99.75%. It might help "
		"in constraints like min days, preferred rooms or max hours daily. The precision in FET is limited to %1 decimal digits after the decimal point, "
		"but probably nobody will use such a fine precision.").arg(CUSTOM_DOUBLE_PRECISION);

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: Help on ConstraintStudentsEarlyMaxBeginningsAtSecondHour.\n\n"
		"A: You can specify the maximum number of beginnings at second available hour (beginnings at third hour not possible).\n\n"
		"If you input only partial data, please use with caution. If you add a constraint with max 0 beginnings at second "
		"hour: you might for instance input only 4+4 hourly activities of math with the same teacher for 2 students "
		"sets (each group of 4 activities must be balanced - in different days). Then it is clear that you cannot place "
		"all 8 activities in a 5 days week without breaking the students early constraint, so you will get no possible timetable.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: How about optional subjects or students sets which are divided according to options?\n\n"
		"A: One possible approach: in FET you can have each activity with more teachers and students sets. "
		"If you have for instance students set S which must attend subject Language1, English (with teacher TE) "
		"or French (teacher TF), you might choose not divide S and add an activity with Language1, S and TE and TF."
		" The drawback: each activity can take place in a single room, you cannot tell FET that this activity should "
		"be in 2 or more rooms at the same time, and from here derive other problems: if some room is not available, "
		"if capacity of room is too low for the number of students in S FET cannot find timetable, and maybe others.\n\n"
		"Another possible approach: you may choose to define students into sections (see question below).");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: How to define the students into sections?\n\n"
		"A: FET can automatically divide years by categories of options."
		"\n\n"
		"Add years and the option 'Divide ...' in the years dialog is the easiest way. You can also add manually years, groups and subgroups.");
	s+="\n\n";
	s+=tr("Important: you must consider that each year contains groups which contain independent subgroups. Subgroups should have "
		"all the activities of the year and group plus additional optional activities. Please check menu statistics/students for "
		"subgroups, each subgroup should have a reasonable number of working hours per week, close to the average of hours per week for all subgroups.");
	s+="\n\n";
	s+=tr("An impossible timetable might be caused by incorrect years division. Please check statistics/students for all subgroups"
		", each subgroup should have the necessary hours, not less. If you have for instance some subgroups with less than say 20 "
		"hours per week you might get an impossible timetable, probably because of incorrect division of years and incorrectly added "
		"activities. All the subgroups are independent. If you have a subgroup with only 2 hours per week and constraint early, "
		"then these 2 activities must be placed in the first hours of the day, which is probably much too hard (wrong).");
	s+="\n\n";
	/*s+=tr("Important note: please do the correct division of a year from the beginning. Each time you divide a year, the "
		"corresponding activities and constraints referring to year's groups and subgroups will be removed, along with groups "
		"and subgroups. This is not elegant, I know, I will try to find a better solution in the future. If you already inputted "
		"a lot of constraints and activities referring to a year's groups/subgroups, you might want to manually change the division of "
		"a year by the groups/subgroups menus.");
	s+="\n\n";*/
	s+=tr("Another possibility: you can see that each activity has allowed more teachers/students sets. If you need to split year Y "
		"according to Language1 (with teachers T1 and T2), you might not split Y and add an activity with Y and teachers T1 and "
		"T2 and subject 'Language1'. This is a small trick, which might be easier to use than to divide a year.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: What is the structure of the students FET can handle?\n\n"
		"A: FET was designed to allow any school structure:\n\n"
		"- independent subgroups (non-overlapping);\n\n"
		"- overlapping groups (several subgroups) and years (several groups).");
		
	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: How can one work with overlapping structures of students?\n\n"
		"A: If you have overlapping groups, then you must define the smallest independent subgroup, which does not overlap with any other subgroup."
		" Example: you have 1 group, subject sport (which must be taught to boys and girls separately) and subject physics, which is an optional "
		"subject and only some students would like to have this course (yes, FET can manage optional subjects). Then, you must define the "
		"subgroups: boys who want physics, boys who do not want physics, girls who want physics, girls who do not want physics."
		" Now, it is very easy. Just define\n\n"
		"group girls=subgroup girls who want physics + girls who do not want physics,\n"
		"group boys=subgroup boys who want physics + boys who do not physics\n"
		"group physics=boys who want physics + girls who want physics.\n\n"
		"Then, you can add as many activities as you want to the corresponding groups:\n"
		"Activity1: teacher A, group girls, subject sport;\n"
		"Activity2: teacher B, group boys, subject sport;\n"
		"Activity3: teacher C, group physics, subject optional physics.\n\n"
		"NEW: the thing is automatic now. Just select year->divide in the years dialog. Please see above entries in the FAQ.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: Can you add more students sets or teachers to a single activity?\n\n"
		"A: Yes, you can add several students sets (subgroups, groups or years) and several teachers per activity.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: Help on ConstraintMinDaysBetweenActivities.\n\n"
		"A: It refers to a set of activities and involves a constant, N. For every pair of activities in the set, "
		"it does not allow the distance (in days) between them to be less than N. If you specify N=1, then this "
		"constraint means that no two activities can be scheduled on the same day. N=2 means that each two activities "
		"must be separated by at least one day\n\n"
		"Example: 3 activities and N=2. Then, one can place them on Monday, Wednesday and Friday (5 days week).\n\n"
		"Example2: 2 activities, N=3. Then, one can place them on Monday and Thursday, on Monday and Friday, then on Tuesday and Friday (5 days week).\n\n"
		"The weight is recommended to be between 95.0%-100.0%. The best might be 99.75% or a value a little under 100%, "
		"because FET can detect impossible constraints this way and avoid them. The weight is subjective.\n\n"
		"You can specify consecutive if on the same day. Please be careful, even if the constraint min days between activities has 0% "
		"weight, if you select consecutive if on the same day, this 'consecutive' property will be forced. You will not be able to find "
		"a timetable with the two activities on the same day, separated by break, not available or other activities, even "
		"if the constraint has weight 0%, if you select consecutive if on the same day.");
	s+="\n\n";
	s+=tr("Note: FET will not place more than two activities involved in this constraint on the same day, if the constraint is broken "
		"(you may end up with more days containing two involved activities, though, if the constraint is to be broken more times).");
	s+="\n\n";
	s+=tr("Important: please do not input unnecessary duplicates. If you input for instance 2 constraints:\n\n"
		"1. Activities 1 and 2, min days 1, consecutive if on the same day=true, weight=95%\n"
		"2. Activities 1 and 2, min days 1, consecutive if on the same day=false, weight=95%\n"
		"(these are different constraints),\n"
		"then the outcome of these 2 constraints will be a constraint:\n\n"
		"Activities 1 and 2, min days 1, consecutive if on the same day=true, weight=100%-5%*5%=99.75%, very high. This is because of FET algorithm.\n\n"
		"You may however add 2 constraints for the same activities if you want 95% with min 2 days and 100% with min 1 day. These are not duplicates.\n\n"
		"You might get an impossible timetable with duplicates, so beware.");
	s+=tr("If you need to balance 3 activities in a 5 days week, you can add, in the new version 5.5.8 and higher, "
		"directly from the add activity dialog, 2 constraints. You just have to input min days 2, and FET will ask if "
		"you want to add a second constraint with min days 1. This way, you can ensure that the activities are balanced "
		"better (at least one day apart, usually 2 days apart)");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: Can I use FET to do interactive timetabling?\n\n"
		"A: Yes, but this is not easy. All the part regarding data representation and gradually construction of the solution is working, "
		"only the interface has to be updated.\n\n"
		"Anyway, when you add a compulsory ConstraintActivityPreferredStartingTime, it means that you fixed that activity. You can use "
		"this feature for a semi-automatic or even manual timetabling, but it is not so convenient.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: Is it possible to make a timetable where the students learn in two shifts? (for instance, the lowest forms in the morning and "
		"the highest forms in the afternoon)?\n\n"
		"A: Yes, you have to add more constraint students set not available accordingly (for the lowest forms not available Mon, "
		"Tue, Wed, Th, Fr each from middle hour to last hour and for highest forms from first hour to middle hour). "
		"The constraints no gaps and early work correctly with these not available: if not available, a students set will not have "
		"gaps or early broken for the period of non-availability.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: I added an activity with min days constraint. But I cannot see the min days value in modify activity dialog\n\n"
		"A: Min days is a time constraint. You can see it in the time constraints dialog");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: Can I work with fortnightly activities like in older FET versions?\n\n"
		"A: You have to use some tricks. It would be difficult to consider fortnightly activities into the new FET algorithm "
		"(from 5.0.0 up). But I think you can apply this: for instance, I suppose that you would like the first week to have activity "
		"A1 (teacher T1, student S1) and A2 (T2, S2), and second week A3 (T1, S2) and A4 (T2, S1) (simultaneously)."
		" You could define a weekly activity A (T1, T2, S1, S2). If you need 2 rooms for A, then you can define dummy "
		"A' (no teachers, no students) and constraint activities same starting time A and A' and add rooms for A and A' .");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";


	s+=tr("Q: How to choose the weight percentage of constraint min days between activities?\n\n"
		"A: You can use for the constraint min days the weight you want. It can be 95%, 99%, 99.75% or even 100%, but please make sure "
		"your timetable is not too constrained. Please take care of the impossible constraints, they should have under 100% weight "
		"(percentage) - best would be 0%. For instance, if a teacher teaches only two days per week and has 3 math activities for "
		"a group, then it is clear that the constraint cannot be respected, so the correct way is to specify under 100% weight "
		"(percentage) for the corresponding min days constraint - best would be 0%.");
	s+="\n\n";
	s+=tr("You could try at first the 95% minimum recommended value, then heighten the weight percentage up to maybe 100%. "
		"I am not sure here, I have not enough sample files (please contribute with advice). If you would like to change "
		"the 95% for another value for all constraints of this type, the easiest way is in Data/Time constraints/Min days between "
		"activities dialog, where starting with version 5.3.6 there is a simple command for that.");
	s+="\n\n";
	s+=tr("You might want to choose different weights for different constraint min days (for instance, higher on subjects with less activities per week)");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: Are some constraints more efficient in speed of generation than other constraints, even if they give the same result?\n\n"
		"A: From the way the algorithm is conceived, the automatic generation is faster if you use students set (or teacher) not available "
		"and/or constraint activity(ies) preferred time slots or constraint activity preferred starting time to specify impossible slots, "
		"in addition to possible use of students (set) or teacher(s) max hours daily, whenever it is possible. For instance, if you "
		"know that year 5 will only have hours from 8:00 to 13:00, it is better to add students set not available in addition to students "
		"set max hours daily. So, if possible, try to follow this advice. This trick helps guide FET better towards a solution.\n\n"
		"Each constraint of type not available or preferred times which filters out impossible slots might bring an improvement in speed.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: I have a sports room which allows more than 1 activity simultaneously in it. How to make the preferred room constraints? "
		"Can FET accept more than one activity at the same time in a single room?\n\n"
		"A: Each room can host a single activity at the same time. How to implement what you need? You can add more "
		"rooms (sport1, sport2, sport3) and instead of a single preferred room add more preferred rooms.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: I got an impossible to solve timetable!\n\n"
		"A: If you get an impossible timetable, maybe the constraints students (set) early are too difficult. "
		"Maybe you can allow more beginnings at second hour. Also teachers' min hours daily might be too strong. "
		"Please also check the statistics to be correct. Remove other constraints until you get a possible timetable.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: Is it possible to work with 0 hour?\n\n"
		"A: A bit difficult. You have to choose a day for this 0 hour. Then add breaks or not available to "
		"prevent other activities in other days at hour 0, then add students set not available to prevent other "
		"students sets from having hours at this hour 0. Or variants of this.\n\n"
		"%1 used another trick: considered the last hour to be hour 0. But this is not always applicable.", "%1 is a person").arg("Zsolt Udvari");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: How does FET care about weights? What do they mean?\n\n"
		"A: The weights under 100% are subjective, because FET can skip them if "
		"necessary.\n\n"
		"%1 tried to express how FET considers under 100% weights:\n\n"
		"weight = 50% means: In average FET retries two times to place an "
		"activity without a conflict. If it isn't able to place the activity "
		"without a conflict after average 2 times it keeps the conflict and tries "
		"to place the next activity.\n"
		"weight = 75% means: In average FET retries four times to place an "
		"activity without a conflict. If it isn't able to place the activity "
		"without a conflict after average 4 times it keeps the conflict and tries "
		"to place the next activity.\n"
		"weight = 99% means: In average FET retries 100 times to place an "
		"activity without a conflict. If it isn't able to place the activity "
		"without a conflict after average 100 times it keeps the conflict and "
		"tries to place the next activity.\n"
		"weight = 99.99% means: In average FET retries 10000 times to place an "
		"activity without a conflict. If it isn't able to place the activity "
		"without a conflict after average 10000 times it keeps the conflict and "
		"tries to place the next activity.\n\n"
		"Also, activities might get unallocated, and the cycle would be opened.", "%1 is a person").arg("Volker Dirr");

	/*s+=tr("Q: How does FET care about weights. What do they mean?\n\n"
		"A: The weights under 100% are subjective, because FET can skip them if necessary.\n\n"
		"Volker Dirr tried to express how FET considers under 100% weights, but his words are not completely reflecting the fact:\n\n"
		"Here is a comment from Volker Dirr:\n\n"
		"weight = 50% means: In average FET retries two times to place an activity without a conflict. If it isn't able to place the "
		"activity without a conflict after average 2 times it keeps the conflict and tries to place the next activity.\n"
		"weight = 75% means: In average FET retries four times to place an activity without a conflict. If it isn't "
		"able to place the activity without a conflict after average 4 times it keeps the conflict and tries to place the next activity.\n"
		"weight = 99% means: In average FET retries 100 times to place an activity without a conflict. "
		"If it isn't able to place the activity without a conflict after average 100 times it keeps the conflict and tries to place the next activity.\n"
		"weight = 99.99% means: In average FET retries 10000 times to place an activity without a conflict."
		" If it isn't able to place the activity without a conflict after average 10000 times it keeps the conflict and tries to place the next activity.\n\n"
		"This is not 100% correct. Activities might get unallocated, and cycle reopened.");*/

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: Help on constraint activities preferred starting or preferred time slots (a set of activities has a set of preferred starting or time slots)\n\n"
		"A: You can specify a set of activities by selecting a teacher (if empty - all teachers), a students set "
		"(if empty - all students), a subject (if empty - all subjects) and an activity tag (if empty, all activity tags) and a set of allowed time slots.\n\n"
		"Starting means that an activity may only start at these periods.\n\n"
		"Time slots means more restrictive, that activity may only start and end and take place in these intervals ("
		"if activity has duration 2 and on Monday is allowed 8:00, 9:00 and 10:00, then activity can only start at 8:00 or 9:00).");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: Help on statistics menu\n\n"
		"A: The statistics are important to check your data before generating.\n\n"
		"When seeing the students statistics, probably the most useful are for subgroups. Each subgroup should have a "
		"reasonable amount of hours, and if you are having subgroups with too little hours, like 2 per week, probably "
		"you misunderstood FET notation. Each subgroup is independent of the others and should have a number of hours per "
		"week close to the average of all subgroups. Do not input empty subgroups with only a few activities.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: What if I enter accidentally duplicate constraints?\n\n"
		"A: It is not a good practice to allow unnecessary duplicate constraints. For min days between activities and other constraints"
		", the combined weight becomes very high. For instance, if you have 2 constraints with weight 95%, the outcome is "
		"a constraint with weight 100%-5%*5%=99.75%, very high. You might get an impossible timetable with duplicates, so "
		"beware. For constraint activity(ies) preferred (starting time(s) or time slots) the chosen weight is the "
		"largest value, so you can use more constraints of this type for same activities.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: More information about students' structure\n\n"
		"A: The students' structure is very flexible and permits any institution structure.\n\n"
		"The main idea is that subgroups are independent. Each subgroup must be the smallest teaching unit or even a single student.\n\n"
		"The groups can contain any subgroups. The groups can be overlapping.\n\n"
		"The years can contain any groups.\n\n"
		"You do not need to follow exactly your institution's hierarchy, you can model your structure to be able to use FET better.\n\n"
		"Example: you have a faculty with years (1, 2, ..., 5), sections (1_a, 1_b, ...) and subsections (1_a_x, 1_a_y, ...)."
		" Each subsection has optional English or French or German languages (say, a students set who takes English "
		"is ENG, and who does not take English is !ENG). Then the correct structure is:\n"
		"\n"
		"subgroups\n\n"
		"1_a_x_ENG_FRE_GER,\n"
		"1_a_x_!ENG_FRE_GER,\n"
		"1_a_x_ENG_!FRE_GER,\n"
		"1_a_x_!ENG_!FRE_GER,\n"
		"1_a_x_ENG_FRE_!GER,\n"
		"1_a_x_!ENG_FRE_!GER,\n"
		"1_a_x_ENG_!FRE_!GER,\n"
		"1_a_x_!ENG_!FRE_!GER,\n"
		"\n"
		"1_a_y_ENG_FRE_GER,\n"
		"...same for 1_a_y (8 subgroups)\n\n"
		"(you can consider not adding empty subgroups, for instance if every student has one single language, then you can retain only 3 subgroups out of 8)\n\n"
		"groups:\n\n"
		"1_a\n"
		"contains subgroups:\n\n"
		"    1_a_x_ENG_FRE_GER,\n"
		"    1_a_x_!ENG_FRE_GER,\n"
		"    1_a_x_ENG_!FRE_GER,\n"
		"    1_a_x_!ENG_!FRE_GER,\n"
		"    1_a_x_ENG_FRE_!GER,\n"
		"    1_a_x_!ENG_FRE_!GER,\n"
		"    1_a_x_ENG_!FRE_!GER,\n"
		"    1_a_x_!ENG_!FRE_!GER,\n"
		"    1_a_y_ENG_FRE_GER,\n"
		"...same for 1_a_y (8 subgroups)\n\n"
		"1_a_x\n\n"
		"contains subgroups:\n\n"
		"    1_a_x_ENG_FRE_GER,\n"
		"    1_a_x_!ENG_FRE_GER,\n"
		"    1_a_x_ENG_!FRE_GER,\n"
		"    1_a_x_!ENG_!FRE_GER,\n"
		"    1_a_x_ENG_FRE_!GER,\n"
		"    1_a_x_!ENG_FRE_!GER,\n"
		"    1_a_x_ENG_!FRE_!GER,\n"
		"    1_a_x_!ENG_!FRE_!GER,\n\n"
		"1_a_y\n\n"
		"contains subgroups:\n\n"
		"    1_a_y_ENG_FRE_GER,\n"
		"... (8 subgroups)\n\n"
		"1_a_x_ENG\n\n"
		"contains subgroups:\n\n"
		"    1_a_x_ENG_FRE_GER,\n"
		"    1_a_x_ENG_!FRE_GER,\n"
		"    1_a_x_ENG_FRE_!GER,\n"
		"    1_a_x_ENG_!FRE_!GER\n\n"
		"years:\n\n"
		"1\n\n"
		"contains groups 1_a, 1_b\n\n"
		"You will have the possibility to add any activity, for a year or group\n\n"
		"Currently, the interface for students is difficult to use. I am thinking of that. Maybe it is more simple for you if you try to work on the XML .fet file.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q-1-27-March-2008", "Mnemonic name for a particular question in the FAQ");
	s+="\n\n";
	s+=tr("Q: Example: I have 7 hours of math per 5 days week (7 is larger than 5). How to add correctly this split activity?\n\n"
		"Complete question: I have a large container activity split into more activities than the number of days per week. "
		"How to add it and constraint min days between activities?\n\n"
		"A: If you add directly a container activity split into more than the number of days per week and also add a constraint "
		"min days between activities, it would be a very bad practice from the way the algorithm of generation works (it slows down the "
		"generation and makes it harder to find a solution).\n\n"
		"The best way to add the activities would be:\n\n"
		"1. If you selected 'consecutive if on the same day', then couple extra activities in pairs to obtain a number of "
		"activities equal to the number of days per week. Example: 7 activities with duration 1 in a 5 days week, then "
		"transform into 5 activities with durations: 2,2,1,1,1 and add a single container activity with these 5 "
		"components (possibly raising the weight of added constraint min days between activities up to 100%)\n\n"
		"2. If you didn't select 'consecutive if on the same day', then add a larger activity split into a number of "
		"activities equal with the number of days per week and the remaining components into other larger split activity. For "
		"example, suppose you need to add 7 activities with duration 1 in a 5 days week. Add 2 larger container activities, "
		"first one split into 5 activities with duration 1 and second one split into 2 activities with duration 1 ("
		"possibly raising the weight of added constraints min days between activities for each of the 2 containers up to 100%)");
	s+="\n\n";
	s+=tr("Note: If the weight of the added constraint min days between activities is 0% or a low value, you can safely ignore this warning.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q-1-6-June-2008", "Mnemonic name for a particular question in the FAQ");
	s+="\n\n";
	s+=tr("Q: What I need is a way to make the following constraint:\n\n"
		"If Activity1 comes first, then Activity2 can be consecutive. "
		"But, if Activity2 comes first, then Activity1 must have at least 1 period in between.\n\n"
		"For example, if the algorithm places Dance in 1st hour, then Wrestling can be placed in 2nd hour, but if Wrestling "
		"is placed in 1st hour, then Dance must be at least in 3rd hour, if not farther away.\n\n"
		"A simpler, but less useful solution could be a constraint that says:\n\n"
		"Activity1 and Activity2 will not be consecutive.\n\n"
		"A: I have a very good solution for you, but it is a bit complicated to add.\n\n"
		"Suppose you have activities A1 and A2. Add dummy A3, with duration 1, no teachers and no students. Add constraint "
		"two activities consecutive, A2 and A3 (A2 followed by A3). Add constraint activities not overlapping, A1 and A3.\n\n"
		"There is only a small problem: A2 cannot be put in the last period. To correct that: increase the number of "
		"hours with 1 and constrain all the real activities (without A3) to take place in the first periods. "
		"You can do that by adding an activity tag to A1 and A2 named Early and an activity tag to A3 named Any, and constraint "
		"activities preferred time slots for activity tag Early to be in the first n-1 slots of each day.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: What is the difference between preferred starting times and preferred time slots?\n\n"
		"A: Time slots is more restrictive, means all hours of an activity must be in the allowed intervals.\n\n"
		"Example: Preferred times Monday 8,9 and 10. If activity A has duration 2, then starting means that activity A "
		"can start at 8, 9 or 10 on Monday, while time slots means that activity A can start on Monday at 8 or 9 (10 is not "
		"allowed, because the last hour of activity is not allowed there).");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: What means constraint min gaps (hours) between a set of activities?\n\n"
		"A: I had many users wanting to put a gap between activities of same teachers or between activities with same teacher and "
		"subject, if they are on the same day. This constraint does that. If you want for instance to make teacher John to have "
		"at least one gap between all his activities, select filter 'John' and add all his activities to a "
		"constraint of this type. If you want to make teacher John to have at least one gap between all "
		"his math activities, select filter 'John' and 'math' and add all these activities to a constraint of this type.\n\n"
		"Please take care that the selected activities are not forced consecutive by constraint two activities consecutive or "
		"by constraint min days between activities which have consecutive if on the same day selected.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: What type of files uses FET?\n\n"
		"A: FET uses text files, XML, HTML, txt, or CSV (comma separated values - for import/export). The used encoding is UTF-8.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: Help on constraint subactivities preferred starting or preferred time slots (a set of subactivities has a set of preferred starting or time slots)\n\n"
		"A: You select the component number of certain activities (say, if you have an activity split into 5 activities per week, you "
		"have components 1, 2, 3, 4 or 5). Only the selected number of this split activity (the corresponding component activity) will be constrained.\n\n"
		"You will also specify the set of subactivities by selecting a teacher (if empty - all teachers), a students set ("
		"if empty - all students), a subject (if empty - all subjects) and an activity tag (if empty, all activity tags) and a set of allowed time slots.\n\n"
		"Starting times means that an activity may only start at these periods.\n\n"
		"Time slots means more restrictive, that activity may only start and end and take place in these intervals (if activity has duration 2 "
		"and on Monday is allowed 8:00, 9:00 and 10:00, then activity can only start at 8:00 or 9:00).\n\n"
		"This is useful if you need for instance, if math activities are 4-5 per week, to constrain that the first "
		"component and the second component must be early. You will add 2 constraints for that, with component number "
		"1 and 2, both with subject math. Or, if you want for activities split into 4 that 2 activities are early and for "
		"activities split into 5 that 3 activities are early, add constraint math with split number 3, 4 and 5 (nice trick).\n\n"
		"Another thing: if you have 1 or 2 activities per week for a subject, say biology, and want to constrain one "
		"of the components if there are 2 per week, and none if there is only 1, you can add such a constraint for component number=2.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: Help on constraints teacher(s) or students (set) hourly interval max days per week\n\n"
		"A: This is a constraint suggested by users, to allow you to specify an hourly interval for students or teachers, and "
		"to say that in this interval they must work at most max days per week. This is useful if for instance you want teachers "
		"not to have more than 2 days per week activities in the last 2 hours of the day.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: Help on constraint activities end students day (or activity ends students day).\n\n"
		"A: If you have activities which you want to put in the last slots of a day (like say the meetings with "
		"the class master), please use the new constraint a set of activities end students day (or singular activity ends "
		"students day). This constraint can have weight only 100%, because of the way the algorithm works.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";
	
	s+=tr("Q: At our school there are a lot of teachers which work only a few hours a week. Of course it "
		"is really nasty to drive for one hour to the school. So we set the constraint that every teacher "
		"should work at least 2 hours a day. Unfortunately we have this year a teacher which only works 1h a "
		"week. As a result of this FET doesn't start to create a timetable. Any suggestions how "
		"to fix the problem without defining a constraint for every singular teacher?\n\n"
		"A: I have a nice trick: add a dummy activity, 1 hour duration, with only this teacher (no students sets, any subject), additional to the real activity.\n\n"
		"This trick just passed through my mind as I was trying to write you that you have to do it the hard way (add constraints for each teacher)");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";
	
	s+=tr("Q: What about the automatic search for updates? Should I enable it?\n\n"
		"A: It is recommended to enable automatic search for updates. I didn't set it as default because people might be annoyed if I "
		"release too fast new versions. But if you can cope with that, it is recommended to always have the latest version and enable "
		"searching for updates at startup.");
	s+="\n\n";
	s+=tr("Please note that, by enabling this option, each time you start FET it will get the file %1 from the FET homepage, so the request for "
		"this file will be visible on the server, along with your IP address and access time.")
		.arg(QString("https://lalescu.ro/liviu/fet/crtversion/crtversion.txt"));
	s+=" ";
	s+=tr("Thus, it could be deduced if and when you use FET.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";
	
	s+=tr("Q: I have a double duration activity. Is it possible that it is spread over the break period, like:\n\n"
		"Activity math, duration 2, id say 100\n\n"
		"Hour 10:00 math (first hour of act. 100)\n"
		"Hour 11:00 break\n"
		"Hour 12:00 math (second hour of act. 100)?\n\n"
		"A: No, the activity must respect the break, so it is before or after the break with all the hours of it.");
	s+=" ";
	s+=tr("Alternative solutions: either you can split that activity into two subactivities with duration 1 (without a min days constraint "
		"between them), and add a two activities consecutive constraint, or you can choose to not add the break constraint in your file.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";
	
	s+=tr("Q and A From %1: A very difficult to diagnose unresolved case\n\n"
		"Since I started working on our school's timetable I had a problem with a specific day and teacher. No matter what I tried in FET I couldn't "
		"reduce the in-school hours for this specific teacher and day down from 7 (7 hours is the full school day, so he had 6 teaching hours and 1 gap). "
		"It was too much (other teachers have 5 teaching hours max) but he didn't mind so we kept FET solution."
		"Just a month ago this teacher had some major operation and couldn't teach so much hours in one day anymore. So I started again to experiment "
		"with the timetable and what was wrong. After a LOT of test and failures the idea hit me just as I was waking up to go to school; I hadn't enough "
		"teachers for early and late hours! I checked and it was true! Let me explain. We have 6 classes in total and every class has 7 hours per day. "
		"This means that I need 6 teachers for the first hour and 6 teachers for the 7th hour, a total of 12 teachers. But I only had 11 teachers to "
		"cover first and last hour! I had 16 teachers in total for that day but their restrictions prevented 6 of them to teach first and last hours "
		"(and it wasn't just cases of teacher not available but usually restrictions on teaching on specific hours and max gaps, so it was very "
		"difficult to look through it). So this specific teacher (because of the restrictions on the other teachers and the loose restrictions on "
		"himself) had to teach 6 hours with 1 gap so that he could cover a first and a last hour!\n\n"
		"...Not that I will fall again for it but based on the difficulty to diagnose on my part it will help others that might face the same problem.",
		"%1 is a person").arg(QString("Anestis Vovos"));

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: I want to define hard subjects (math, physics and chemistry) and I want students not to have more than 1 (or another variant 2) difficult subjects in a row.\n\n"
		"A: Define activity tag 'Difficult' and add it to all MA, PH and CH activities. Then add constraint maximum 1 (or 2) "
		"hours continuously for all students and an activity tag 'Difficult'. Please take care if you may have double activities.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: (by %1) I met a situation: a teacher asks for maximum 2 working days, but these days "
		"should not be consecutive. Is there a way to implement it in .fet?\n\n"
		"The only (manual) way I could think of is to set the teacher as unavailable on Tuesdays and Thursdays, thus "
		"leaving him available on Monday, Wednesday and Friday (any two of these are nonconsecutive).\n\n"
		"Any other ideas...?\n\n"
		"A: I have another idea: choose 2 activities of this teacher which clearly cannot be on the same day, "
		"and add constraint min days between activities, 2 days, 100%.\n\n"
		"Or add a dummy activity for this teacher, split into 2 per week, min days = 2, with 100%. You just need to "
		"take care that this teacher has place for these dummy activities (enough slots in the day) and to consider "
		"these dummy activities as possible gaps, so if teacher has max gaps 2 then make max gaps for him 0.",
		"%1 is a person").arg(QString("Horațiu Hălmăjan"));

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: (by %1) The students must have max 4 gaps per week, maximum 2 per day, continuous gaps. How to solve this?\n\n"
		"A: Add for each subgroup a dummy activity (no teachers) split into 4 per week, duration 1, min days between "
		"activities 1, weight 0%, select consecutive if on the same day. FET will never put more than 2 of these dummy activities "
		"on a day. Add max gaps for students = 0 per week.", "%1 is a person").arg(QString("Horațiu Hălmăjan"));

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Advice: to treat gaps for students or teachers, if FET constraints are not enough, you can use dummy activities."
		" For instance, if a students set can have maximum 3 gaps, add an activity split into 3 per week, with no teachers.");
		
	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Question 1/16 August 2009: How to add constraint two activities grouped, two activities consecutive and three "
		"activities grouped if the activities are constrained not to be on the same day by constraints min days between activities?\n\n"
		"If A1 and A2 are constrained not to be on the same day with 95% weight or any other weight, it is a bad practice "
		"to add a constraint grouped or consecutive to them. If they are constrained with weight 100% not to be on the "
		"same day, the timetable is impossible; if the weight is under 100%, the timetable is more difficult to find than "
		"using the correct way.\n\n"
		"The correct way would probably be to consider A1 and A2 = a single activity A12', or to modify the related constraint "
		"min days between activities. Or maybe you can find other ways.");
		
	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Three hints from an anonymous Polish user, who uses FET for very large data sets:");
	s+="\n\n";
	s+=tr("The first hint for other users is to start with a minimum number of constraints and see if FET can generate a timetable "
		"with those constraints before thinking about adding the next ones.");
	s+="\n\n";
	s+=tr("The second hint is not to change too many constraints in one generation. If it is impossible to generate a timetable with those added constraints, "
		"it is difficult to say which particular constraint was responsible.");
	s+="\n\n";
	s+=tr("It may happen, for example, that even when two consecutive activities are placed on the same day (with the same group and the same teacher) "
		"sometimes they are placed in different rooms, which would force them to needlessly change rooms. I guess that FET is focused on fulfilling constraints "
		"but not on optimizing timetables. Sometimes simply changing the two activities is better both for the students and the teachers. I think that manually "
		"improving a generated timetable is faster than creating many more constraints and repeating the generation. And this is the third hint.");
	
	s+="\n\n";
	s+="--------------------";
	s+="\n\n";
	
	s+=tr("Q: Help on shortcut buttons in the main form.");
	s+="\n\n";
	s+=tr("A: More users asked for shortcut buttons for the most commonly used functions. It is possible to show such buttons, if you select the corresponding option"
		" from the Settings->Interface menu (shortcuts are shown, by default).");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";
	
	s+=tr("This is an advanced question which probably will never appear in practice, you may skip it at first reading.");
	s+="\n\n";
	s+=tr("Q: This is a problem that probably will never appear in practice, but it is possible. Say the user generates a timetable successfully, "
		"locks a few activities, then tries to generate again, but FET stops at a certain activity and reports impossible timetable.\n\n"
		"A: Indeed, this is a potential problem in FET (but probably will never show up in practice). It may happen if you have constraints with weight under 100%, which may be broken "
		"when you generate the timetable.\n\n"
		"Here is an example to explain this (it is an impractical example, but it is better as it is very simple): you have 4 students sets (Y1, Y2, Y3, Y4). "
		"5 activities: A1 (Y1,Y2,Y3,Y4), A2 (Y1), A3 (Y2), A4 (Y3), A5 (Y4). You have a single day per week and 2 hours per day. You add a constraint students "
		"max hours daily, max 1 hour, 95% weight percentage.\n"
		"1) Start to generate. After a while (maybe a few minutes), FET will be able to find a solution (with the max hours daily broken for all students sets).\n"
		"2) Then, you lock A2, A3, A4 and A5 and try to generate again. In some cases FET will report impossible activity A1.\n"
		"3) If you lock A1, A2, A3, A4 and A5, FET will be able to find a timetable very fast.\n\n"
		"1) Why can FET schedule the timetable the first time? Because in some cases FET will be able to break one-by-one all the 4 constraints max hours daily for A2, A3, A4 and A5 "
		"(no activity is locked, so it may try more times to place/replace the activities, and it will find a way to put A1, then put the rest of the activities).\n\n"
		"2) Why sometimes FET cannot schedule the timetable the second time (with locked A2, A3, A4 and A5)? "
		"Because: A2, A3, A4 and A5 are locked and are scheduled firstly. Then FET tries to put A1. But to put A1, it means "
		"to break a 95% constraint 4 times, because there are 4 students sets in A1. Weight 95% 4 times in a row is equivalent with a constraint with weight "
		"100%-(5%^4)=99.99999375%, which is a very strong constraint, which is very hardly broken. "
		"FET will retry more times for activity A1, so in some cases it will find a schedule even under these conditions.\n\n"
		"3) Why can FET find a timetable the third time (with locked A1, A2, A3, A4 and A5)? Because activities with more students sets are scheduled firstly (in general, locked activities"
		" are placed in descending order of the sum of the number of teachers and subgroups) and a locked activity is never rescheduled. "
		"So, FET puts A1 first, then A2, A3, A4 and A5. Since it retries more times separately for each activity, it is able to find a timetable easily.\n\n"
		"Practical solution to case 2)? Reduce weights of constraints which have weight below 100% or lock (to a corresponding slot) the activity which corresponds to A1 in your data file.");
	
	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: I tried to work on a fixed timetable, to move an activity to another time slot. Now I got stuck - FET says: impossible to generate.");
	s+="\n\n";
	s+=tr("A: There is this potential problem. Suppose you have max hours daily for students = 4 hours, 99%. You generated successfully (maybe you got some days with more"
		" than 4 hours, it does not matter for our discussion)."
		" Now, if you have a day with 4 hours and try to move another activity"
		" to this day, some students will have 5 hours in this day. If in this day all activities have more students sets, FET may report an impossible timetable. Why?"
		" Because if you have an activity with say 4 subgroups, 99% is assumed for each subgroup, resulting in a very strong constraint for this activity."
		" (100%-(1%^4)). Even if FET retries more times for each activity, it is not enough.\n\n"
		" A solution: lower the weight of this constraint from 99% to 90% or less.");
	
	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("-- This entry by %1 --", "%1 is a person").arg(QString("Regis Bouguin"));
	s+="\n\n";
	s+=tr("Q: A trick to deal with fortnightly activities (my institution has a lot of fortnightly activities):");
	s+="\n\n";
	s+=tr("A: I divided hours in two, the first half (8.00 to 8.30, 9.00 to 9.30, ...) represents week A, the second half"
		" (8.30 to 9.00, 9.30 to 10.00, ...) represents week B, fortnightly activities have duration 1, weekly"
		" activities have duration 2.");
	s+="\n\n";
	s+=tr("I created an activity tag 'Start hour'. It should be added to each weekly activity to make sure they start at"
		" the same time (same day + same hour) on each week. Otherwise, an activity with duration 2 can start on a second"
		" half of hour in the timetable, so this activity is placed on a certain day and at a certain hour on week A,"
		" and on the same day but at the next hour on week B.");
	s+="\n\n";
	s+=tr("Add time constraint: activities with all teachers, all students, all subjects, activity tag = 'Start hour'"
		" have a set of preferred starting times: Monday 8.00, Monday 9.00, ...");
	s+="\n\n";
	s+=tr("It works fine and I got good timetables with strong teachers time constraints.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: I need to add a split activity with total duration 4, which can be either 2+2 or 2+1+1 (two hours on a day and two hours on another day,"
		" or two hours on a day, one hour on another day and one hour on another day).");
	s+="\n\n";
	s+=tr("A: Add 3 activities (let us assume that their id-s are 1, 2 and 3), with durations respectively 2, 1 and 1. It is preferable to add them as"
		" 3 single/independent activities (see note below).\n\n"
		"Add two constraints min 1 day between activities with id-s 1 and 2 and between activities with id-s 1 and 3, 100% weight percentage.\n\n"
		"Add another constraint: min 1 day between activities with id-s 2 and 3, consecutive if on the same day = yes, 0% weight percentage.");
	s+="\n\n";
	s+=tr("Note: It is advisable to add the 3 activities as single/independent ones (not a larger split activity with 3 components). The reason is that if you want"
		" to apply spreading of activities over the week, this operation won't add/remove constraints of type min days between activities for these 3 activities."
		" Also, if you add a split activity, you need to take care not to add a default constraint min days between the 3 components.");
	s+="\n\n";
	s+=tr("There may be other solutions possible, but this one seems perfect with respect to efficiency.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: I would like to specify that a teacher should have activities in certain time slots, no matter which activities.");
	s+="\n\n";
	s+=tr("A: You can use a constraint of type activities occupy min time slots from selection or use in an inverted way a constraint of type activities"
	 " occupy max time slots from selection (the exact menu entries are 'A set of activities occupies min/max time slots from selection').");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: How to easily find the activities with unspecified room in the timetable (after the timetable was generated)?");
	s+="\n\n";
	s+=tr("A: A trick is this: open the file representing the activities timetable in XML form (this file can be found in"
	 " the results directory, with a name like: file_activities.xml) with a text editor, and search for the text <Room></Room>");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: The students and/or teachers should have in each day some activities without interruption, then some"
	 " continuous gaps, then again some activities without interruption."
	 " This situation can appear in these cases: schools in Morocco and Algeria, which have morning and afternoon shifts,"
	 " and also in some universities in which students would prefer to have at most a single cluster of gaps, no matter how long, in each day."
	 " How to treat such situations in FET?");
	s+="\n\n";
	s+=tr("A: A nice way to treat such situations would be to consider the number of FET days = 2 * the number of real days. Each real day"
	 " corresponds to two FET days, one for the morning and one for the afternoon. Then, add constraints max zero gaps, and maybe min two hours"
	 " daily with allow empty days true. You may need to devise some other tricks, in addition.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: How can I add a comment to a (sub)activity or to a constraint?");
	s+="\n\n";
	s+=tr("A: You can add a comment to a (sub)activity by clicking the 'Comments' button in the (sub)activities dialog."
	 " You can add a comment to a constraint by clicking the 'Comments' button in the all time/space constraints dialogs.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: How can I activate/deactivate a constraint?");
	s+="\n\n";
	s+=tr("A: You can activate/deactivate a constraint by clicking the 'Activate'/'Deactivate' buttons in the all time/space"
	 " constraints dialogs.");

	/*s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: How can I sort the constraints?");
	s+="\n\n";
	s+=tr("A: You can sort the constraints in the all time/space constraints dialogs. Sorting is done ascending, according to each"
	 " constraints' comments. You can add suitable comments to obtain a desired order, like: 'rank #1 ... other comments' and"
	 " 'rank #2 ... other different comments'.");*/

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: How can I specify that some activities must be in the same room (at different time slots, of course, because a single room"
	 " can hold a single activity in a certain time slot)? This is needed for instance to constrain the Physics activities of a certain"
	 " students set to take place in the same room, be it Lab-1 or Lab-2");
	s+="\n\n";
	s+=tr("A: You can use the constraint activities occupy max different rooms, with max different rooms = 1.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: How can I backup/restore the FET settings on a computer, or share them between different computers?");
	s+="\n\n";
	s+=tr("A: GNU/Linux: Usually, in your %1 directory you will find a directory \"%2\""
	 " containing the file \"%3\". If you copy this file to another computer, the FET settings will be copied. You can also make"
	 " a backup of this file and copy it on the same computer, later, to restore the previous settings."
	 " If you remove this file, all FET settings on this computer will be reset to defaults."
	 ).arg(QString("$HOME/.config")).arg(QString("fet")).arg(QString("fettimetabling.conf"));
	s+="\n\n";
	s+=tr("macOS: It seems that the configuration file might be %1").arg(QString("$HOME/Library/Preferences/com.fet.fettimetabling.plist"));
	s+="\n\n";
	s+=tr("Windows: Run regedit.exe (Registry Editor) and search for the key \"%1\". You will find a section with this name,"
	 " with the subsection \"%2\". You can export this section to a file, and import it from this file on the same or another computer."
	 " If you remove this section, all FET settings will be reset to defaults.").arg(QString("fet")).arg(QString("fettimetabling.conf"));

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";

	s+=tr("Q: I have many subgroups and I don't explicitly use them. Can I hide them in the combo boxes, to add easier the constraints?"
	 " Also, can I make the FET interface work faster?");
	s+="\n\n";
	s+=tr("A:", "Answer");
	s+=" ";
	s+=tr("If you have many subgroups and you don't explicitly use them, it is recommended to use the three global settings: hide subgroups"
		" in combo boxes, hide subgroups in activity planning, and do not write subgroups timetables on hard disk.");
	s+="\n";
	s+=tr("Note that using the global menu setting to hide subgroups in activity planning is a different thing from the check box in the activity"
		" planning dialog, and the global setting works better and faster.");
	s+="\n";
	s+=tr("If you hide subgroups in combo boxes, the affected dialogs (like the activities dialog or the add constraint students set not available times dialog)"
		" will load much faster if you have many subgroups.");
		
	s+="\n\n";
		
	s+=tr("If you are only working on a timetable, and you do not need to publish it, you may want to disable writing some categories of timetables"
		" on the hard disk, for efficiency (the generation speed is not affected, only the overhead to write the partial/complete timetables"
		" when stopping/finishing the generation). The timetables taking the longest time are the subgroups, groups AND years ones.");
	s+=" ";
	s+=tr("(Also the conflicts timetable might take long to write, if the file is large.)");
	s+=" ";
	s+=tr("After that, you can enable the writing of the timetables and regenerate.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";
	
	s+=tr("Q: How about dividing a students year automatically into many subgroups?");
	s+="\n\n";
	s+=tr("A:", "Answer");
	s+=" ";
	s+=tr("About using a large number of categories, divisions per category and subgroups: it is highly recommended to"
		" keep these to a minimum, especially the number of categories, by using any kind of tricks. Otherwise the timetable"
		" might become impossible (taking too much time to generate).");
	s+=" ";
	s+=tr("Maybe a reasonable number of categories could be 2, 3 or maximum 4. The divide year dialog allows much higher values, but"
		" these are not at all recommended.");
	s+="\n\n";
	s+=tr("Maybe an alternative to dividing a year into many categories/subgroups would be to enter individual students as FET subgroups and add into"
		" each group the corresponding subgroups. But this is hard to do from the FET interface - maybe a solution would be to use an automatic"
		" tool to convert your institution data into a file in .fet format.");
	s+=" ";
	s+=tr("Or you might use the FET feature to import students sets from comma separated values (CSV) files.");
	s+=" ";
	s+=tr("In such cases (individual students as FET subgroups), remember that a smaller number of total subgroups means faster generation time, so"
		" you might want to consider a single subgroup for two or more students who have the exact same activities and constraints.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";
	
	s+=tr("Q: Does the order of constraints have influence on their importance while generating the timetable?");
	s+="\n\n";
	s+=tr("A:", "Answer");
	s+=" ";
	s+=tr("The order of the constraints is only important to the interaction with the user. It is only the weight percentage which counts while"
		" generating the timetable. Note however that you may get different timetables for different constraints order.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";
	
	s+=tr("Q: How does one use the constraints of type students (set) / teacher(s) min gaps between ordered pair of activity tags?");
	s+="\n\n";
	s+=tr("A:", "Answer");
	s+=" ";
	s+=tr("These constraints ensure that for the affected students (set) / teacher(s), if on the same day an activity with the second activity tag comes after"
		" an activity with the first activity tag, between them there must be at least min gaps (hours). As a trick, if you want min gaps from the first"
		" activity tag to the second activity tag and from the second activity tag to the first activity tag (so, between these two activity tags,"
		" in any order they can be, to be at least min gaps), you can add two constraints, the second constraint switching the first activity tag with the second"
		" activity tag.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";
	
	s+=tr("Q: How does one use the constraint of type activity tags not overlapping?");
	s+="\n\n";
	s+=tr("A:", "Answer");
	s+=" ";
	s+=tr("This constraint was suggested by %1, who gave the following example: You can use this constraint for instance if you have"
		" young and old students with Sport activities, and you don't want young students having the Sport activities at the same time with the"
		" old students. You then need to add an activity tag, say SY, to all the young students' Sport activities, and another activity tag,"
		" say SO, to all the old students' Sport activities, and add a constraint activity tags not overlapping for the activity tags SY and SO."
		" You can even create more categories of students' age, like SO1, SO2, SO3, and SO4, and add a single constraint activity tags not"
		" overlapping, so that at a single time slot only a single activity tag out of these four will be present in the timetable.", "%1 is a person")
		.arg(QString("Henrique Belo"));
	s+="\n\n";
	s+=tr("This constraint is related to the constraint of type activities not overlapping, but is much easier to use in the described example above,"
		" because you only need to add a single constraint instead of possibly very many constraints activities not overlapping.");
	s+="\n\n";
	s+=tr("The uses of this constraint might be wider.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";
	
	s+=tr("Q: How can I say that an activity should take place in more rooms?");
	s+="\n\n";
	s+=tr("A:", "Answer");
	s+=" ";
	s+=tr("You could use virtual rooms for this. Read the Help on those dialogs (the Rooms dialog and the Make/edit virtual room dialog)."
		" In short, you need to add a virtual room with n sets of real rooms, where n is the number of rooms you want the activity to occupy.");
	s+="\n\n";
	s+=tr("There is also another possible solution, to add dummy activities and constrain them to start at the same time as the real activity.");

	s+="\n\n";
	s+="--------------------";
	s+="\n\n";
	
	s+=tr("Q: What is the use of virtual rooms?");
	s+="\n\n";
	s+=tr("A:", "Answer");
	s+=" ";
	s+=tr("Virtual rooms were suggested by the user %1 on the FET forum (%2). They can be used to make an activity occupy more rooms, or even to let FET"
		" choose between a large room for this activity or more smaller rooms (if you set the preferred rooms for this activity the"
		" real large room or a virtual room representing the more smaller rooms)."
		" Please read the Help on the virtual rooms dialogs (the Rooms dialog and the Make/edit virtual room dialog).").arg(QString("math"))
		.arg(QString("https://lalescu.ro/liviu/fet/forum/index.php?topic=4249.0"));

	plainTextEdit->setPlainText(s);
}
