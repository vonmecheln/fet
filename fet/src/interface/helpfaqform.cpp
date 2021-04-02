/***************************************************************************
                          helpfaqform.cpp  -  description
                             -------------------
    begin                : Feb 20, 2005
    copyright            : (C) 2005 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "helpfaqform.h"

#include "timetable_defs.h"

HelpFaqForm::HelpFaqForm()
{
    setupUi(this);

    connect(closePushButton, SIGNAL(clicked()), this /*HelpFaqForm_template*/, SLOT(close()));

	//setWindowFlags(Qt::Window);
	/*setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
	QDesktopWidget* desktop=QApplication::desktop();
	int xx=desktop->width()/2 - frameGeometry().width()/2;
	int yy=desktop->height()/2 - frameGeometry().height()/2;
	move(xx, yy);*/
	centerWidgetOnScreen(this);

	QString s;
	
	s+=tr("FET FAQ:");
	s+="\n\n";
	s+=tr("This documentation by Liviu Lalescu, reviewed and modified on %1 (new additions are written with date, most are at the end)", "%1 is the date of last modification")
		.arg(tr("27 February 2010", "Date of modification of FAQ"));
	s+="\n\n";
	s+="--------";
	s+="\n\n";
	
	s+=tr("Q: What is the organization of FET input data?\n\n"
		"A: - Students - organized into sets (years (or forms, or classes), containing groups, containing subgroups)."
		"\n"
		"- Teachers."
		"\n"
		"- Subjects (the names of the possible courses, eg. Maths, Physics, etc.)."
		"\n"
		"- Activity tags (you can use them or not, option is yours: the type of activity: lab, course, seminary, or any other information attached to an activity)."
		"\n"
		"- Rooms (classrooms)."
		"\n"
		"- Activities: a coupling of one or more teachers, a subject and one or more students set. This is usually named a "
		"course, a lecture, a laboratory and so on. An activity can have optionally an activity tag, to help you with some constraints."
		"\n"
		"- Constraints. They can be: time constraints (referring to the allocated day and hour) or space constraints "
		"(referring to rooms allocation). They have a weight percentage, from 0.0% to 100.0%. 100% means that the "
		"constraint will always be respected and if this constraint is impossible, FET will not be able to generate a timetable.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	//s+=tr("Entry added on 20 Nov. 2009.");
	//s+="\n\n";
	s+=tr("Q: What are the maximum limits FET can handle?");
	s+="\n\n";
	s+=tr("A: There are indeed maximum limits for the generation algorithm (all these limits can be increased on demand, as a custom version, because this requires a bit more memory).");
	s+="\n\n";
	s+=tr("These limits are:");
	
	s+="\n- ";
	s+=tr("Maximum total number of hours (periods) per day: %1").arg(MAX_HOURS_PER_DAY);
	s+="\n- ";
	s+=tr("Maximum number of working days per week: %1").arg(MAX_DAYS_PER_WEEK);
	s+="\n- ";
	s+=tr("Maximum total number of teachers: %1").arg(MAX_TEACHERS);
	s+="\n- ";
	s+=tr("Maximum total number of sets of students: %1").arg(MAX_TOTAL_SUBGROUPS);
	s+="\n- ";
	s+=tr("Maximum total number of subjects: %1").arg(MAX_SUBJECTS);
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
	s+=tr("Maximum number of time constraints: %1").arg(MAX_TIME_CONSTRAINTS);
	s+="\n- ";
	s+=tr("Maximum number of space constraints: %1").arg(MAX_SPACE_CONSTRAINTS);
	
	/*s+="\n\n";
	s+=tr("Other limits referring to constraints:");
	s+="\n- ";
	s+=tr("Maximum number of activities in a single constraint min days between activities: %1").arg(MAX_CONSTRAINT_MIN_DAYS_BETWEEN_ACTIVITIES);
	s+="\n- ";
	//s+=tr("Maximum number of activities in a single constraint max days between activities: %1").arg(MAX_CONSTRAINT_MAX_DAYS_BETWEEN_ACTIVITIES);
	//s+="\n- ";
	s+=tr("Maximum number of activities in a single constraint activities same starting time: %1").arg(MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_TIME);
	s+="\n- ";
	s+=tr("Maximum number of activities in a single constraint activities same starting day: %1").arg(MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_DAY);
	s+="\n- ";
	s+=tr("Maximum number of activities in a single constraint activities same starting hour: %1").arg(MAX_CONSTRAINT_ACTIVITIES_SAME_STARTING_HOUR);
	s+="\n- ";
	s+=tr("Maximum number of activities in a single constraint activities not overlapping: %1").arg(MAX_CONSTRAINT_ACTIVITIES_NOT_OVERLAPPING);*/

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: Why some activities appear indented while others not?\n\n"
		"A: The activities are indented for easier visualization. The non-indented ones are the representatives of a "
		"larger split activity (their id is the group id of the larger split activity), while indented ones are "
		"the other components of this larger split activity.");
		
	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: Is it possible to use non-integer weights for constraints?\n\n"
		"A: Yes. Using values like 99.75% might be good sometimes.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: Help on ConstraintStudentsEarlyMaxBeginningsAtSecondHour.\n\n"
		"A: You can specify the maximum number of beginnings at second available hour (arrivals at third hour not possible).\n\n"
		"If you input only partial data, please use with caution. If you add a constraint with max 0 beginnings at second "
		"hour: you might for instance input only 4+4 hourly activities of math with the same teacher for 2 students "
		"sets (each group of 4 activities must be ballanced - in different days). Then it is clear that you cannot place "
		"all 8 activities in a 5 days week without breaking the students early constraint, so you will get no possible timetable.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: How does FET work?\n\n"
		"A: A heuristic algorithm, based on swapping activities recursively to make space for new activities. Email the author or mailing list for details.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
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
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: How to define the students into sections?\n\n"
		"A: FET can automatically divide years by at most 3 categories of options. If you need to divide "
		"a year into 4 categories, you can use a small trick (see below)."
		"\n\n"
		"Add years and the option 'Divide ...' in the years dialog is the easiest way. You can also add manually years, groups and subgroups."
		"\n\n"
		"If each year (for instance 9) is divided by at most 3 categories, you can add year 9 and divide it in 3 categories. "
		"If a year is divided by 4 categories (for instance, year 9 is divided by: section (a, b, c, d), language (en, fr), "
		"religion and boys/girls), you might consider years: 9a, 9b, 9c, 9d, each divided into 3 categories, and divide each year in "
		"the dialog. For more than 4 categories, very unlikely case, you will need to manually adjust groups/subgroups.");
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
	s+=tr("Important note: please do the correct division of a year from the beginning. Each time you divide a year, the "
		"corresponding activities and constraints referring to year's groups and subgroups will be removed, along with groups "
		"and subgroups. This is not elegant, I know, I will try to find a better solution in the future. If you already inputted "
		"a lot of constraints and activities referring to a year's groups/subgroups, you might want to manually change the division of "
		"a year by the groups/subgroups menus.");
	s+="\n\n";
	s+=tr("Another possibility: you can see that each activity has allowed more teachers/students sets. If you need to split year Y "
		"according to Language1 (with teachers T1 and T2), you might not split Y and add an activity with Y and teachers T1 and "
		"T2 and subject 'Language1'. This is a small trick, which might be easier to use than to divide a year.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: What is the structure of the students FET can handle?\n\n"
		"A: FET was designed to allow any school structure:\n\n"
		"- independent subgroups (non-overlapping);\n\n"
		"- overlapping groups (several subgroups) and years (several groups).");
		
	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
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
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: Can you add more students sets or teachers to a single activity?\n\n"
		"A: Yes, you can add several students sets (subgroups, groups or years) and several teachers per activity.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: How can I contribute to/support FET?\n\n"
		"A: You can translate, improve interface, any work. Please email the author or mailing list for details.\n\n"
		"FET is free software and any donation would be great. Please contact the author for that.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: What advantages has FET over other applications?\n\n"
		"A: - It is free software and...\n\n"
		"- Independent subgroups, overlapping or independent groups, overlapping or independent years (flexible enough to permit any kind of "
		"students structure). FET can even be used to manage every individual student, if you really need that;\n\n"
		"- Possibility of optional activities;\n\n"
		"- Many kinds of constraints, possibility to add more.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: On what plaform does FET run?\n\n"
		"A: FET can be compiled for any platform supported by free software toolkit Qt. In particular, FET can be used on GNU/Linux, Mac OS X and Microsoft Windows");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: Help on ConstraintMinDaysBetweenActivities.\n\n"
		"A: It refers to a set of activities and involves a constant, N. For every pair of activities in the set, "
		"it does not allow the distance(in days) between them to be less than N. If you specify N=1, then this "
		"constraint means that no two activities can be scheduled in the same day. N=2 means that each two activities "
		"must be separated by at least one day\n\n"
		"Example: 3 activities and N=2. Then, one can place them on Monday, Wednesday and Friday (5 days week).\n\n"
		"Example2: 2 activities, N=3. Then, one can place them on Monday and Thursday, on Monday and Friday, then on Tuesday and Friday (5 days week).\n\n"
		"The weight is recommended to be between 95.0%-100.0%. The best might be 99.75% or a value a little under 100%, "
		"because FET can detect impossible constraints this way and avoid them. The weight is subjective.\n\n"
		"You can specify consecutive if same day. Please be careful, even if constraint min days between activities has 0% "
		"weight, if you select this consecutive if same day, this consecutive will be forced. You will not be able to find "
		"a timetable with the two activities in the same day, separated by break, not available or other activities, even "
		"if the constraint has weight 0%, if you select consecutive if same day.\n\n"
		"Currently FET can put at most 2 activities in the same day if 'consecutive if same day' is true. "
		"FET cannot put 3 or more activities in the same day if 'consecutive if same day' is true.");
	s+="\n\n";
	s+=tr("Important: please do not input unnecessary duplicates. If you input for instance 2 constraints:\n\n"
		"1. Activities 1 and 2, min days 1, consecutive if same day=true, weight=95%\n"
		"2. Activities 1 and 2, min days 1, consecutive if same day=false, weight=95%\n"
		"(these are different constraints),\n"
		"then the outcome of these 2 constraints will be a constraint:\n\n"
		"Activities 1 and 2, min days 1, consecutive if same day=true, weight=100%-5%*5%=99.75%, very high. This is because of FET algorithm.\n\n"
		"You may however add 2 constraints for the same activities if you want 100% with min 2 days and 95% with min 1 day. These are not duplicates.\n\n"
		"You might get an impossible timetable with duplicates, so beware.");
	s+=tr("If you need to balance 3 activities in a 5 days week, you can add, in the new version 5.5.8 and higher, "
		"directly from the add activity dialog, 2 constraints. You just have to input min days 2, and FET will ask if "
		"you want to add a second constraint with min days 1. This way, you can ensure that the activities are balanced "
		"better (at least one day apart, usually 2 days apart)");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: Can I use FET to do interactive timetabling?\n\n"
		"A: Yes, but this is not easy. All the part regarding data representation and gradually construction of the solution is working, "
		"only the interface has to be updated.\n\n"
		"Anyway, when you add a compulsory ConstraintActivityPreferredStartingTime, it means that you fixed that activity. You can use "
		"this feature for a semi-automatic or even manual timetabling, but it is not so convenient.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: After finding the timetable of our school, suppose that a single teacher needs to modify his timetable and "
		"the rest would like to keep their timetable unchanged. Thus, it is needed to fix all the activities of the rest of "
		"the teachers and re-allocate the hours. Can FET deal with such a situation?\n\n"
		"A: Yes, FET can deal with that. Just add many compulsory ConstraintActivityPreferredStartingTime-s, one for each activity "
		"that you would like to be fixed (the preferred time will be the one from the previous allocation).");
	s+="\n\n";
	s+=tr("Text added on 28 June 2008: The timetable can be saved as .fet file, with activities blocked, and you can unblock certain activities (of this teacher).");
	s+="\n\n";
	s+=tr("Text added on 25 September 2009: You have now simpler possibilities, using lock/unlock features");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: From Yush Yuen: Anyway, what i meant was, our teachers may have lunch at EITHER 5th or 6th period. so, i can't just use a break.\n\n"
		"A: From Volker Dirr: Just add a subject 'lunch'. Then add activities new activities. this activities must contain "
		"the teacher, subject lunch, split activity into number of working days of the teacher, set min day = 100% and add "
		"NO studentsset. Then add constraint activities preferd time for this subject 5th and 6th hour weight = 100%.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: Is it possible to make a timetable where the students learn in two shifts? (for instance, the lowest form in the morning and the highest forms in the afternoon)?\n\n"
		"A: Yes, you have to add more constraint students set not available accordingly (for the lowest forms not available Mon, "
		"Tue, Wed, Th, Fr each from middle hour to last hour and for highest forms from first hour to middle hour). "
		"The constraints no gaps and early work correctly with these not available: if not available, a students set will not have "
		"gaps or early broken for the period of non-availability.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: What are groups of type 'year1 WHOLE YEAR' and subgroups of type 'group1 WHOLE GROUP'?\n\n"
		"A: FET old versions (prior to 5.4.17) inserted automatically them. Please remove them now.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: I added an activity with min days constraint. But I cannot see the min days value in modify activity dialog\n\n"
		"A: Min days is a time constraint. You can see it in the time constraints dialog");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: Can I work with fortnightly activities like in older FET versions?\n\n"
		"Answer modified 21 July 2008:\n\n"
		"A: You have to use some tricks. It would be difficult to consider fortnightly activities into the new FET algorithm "
		"(from 5.0.0 up). But I think you can apply this: for instance, I suppose that you would like the first week to have activity "
		"A1 (teacher T1, student S1) and A2 (T2, S2), and second week A3 (T1, S2) and A4 (T2, S1) (simultaneously)."
		" You could define a weekly activity A (T1, T2, S1, S2). If you need 2 rooms for A, then you can define dummy "
		"A' (no teachers, no students) and constraint activities same starting time A and A' and add rooms for A and A' .");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";


	s+=tr("Q: How to choose the weight percentage of constraint min days between activities?\n\n"
		"A: You can use for the constraint min days the weight you want. It can be 95%, 99%, 99.75% or even 100%, but please make sure "
		"your timetable is not too constrained. Please take care of the impossible constraints, they should have under 100% weight "
		"(percentage) - best would be 0%. For instance, if a teacher teaches only two days per week and has 3 math lessons for "
		"a group, then it is clear that the constraint cannot be respected, so the correct way is to specify under 100% weight "
		"(percentage) for the corresponding min days constraint - best would be 0%.");
	s+="\n\n";
	s+=tr("You could try at first the 95% minimum recommended value, then highten the weight percentage up to maybe 100%. "
		"I am not sure here, I have not enough sample files (please contribute with advice). If you would like to change "
		"the 95% for another value for all constraints of this type, the easiest way is in Data/Time constraints/Min days between "
		"activities dialog, where starting with version 5.3.6 there is a simple command for that.");
	s+="\n\n";
	s+=tr("You might want to choose different weights for different constraint min days (for instance, higher on subjects with less activities per week)");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: Are some constraints more efficient in speed of generation than other constraints, even if they give the same result?\n\n"
		"A: From the way the algorithm is conceived, the automatic generation is faster if you use students set (or teacher) not available "
		"and/or constraint activity(ies) preferred time slots or constraint activity preferred starting time to specify impossible slots, "
		"in addition to possible use of students (set) or teacher(s) max hours daily, whenever it is possible. For instance, if you "
		"know that year 5 will only have hours from 8:00 to 13:00, it is better to add students set not available in addition to students "
		"set max hours daily. So, if possible, try to follow this advice. This trick helps guide FET better towards a solution.\n\n"
		"Each constraint of type not available or preferred times which filters out impossible slots might bring an improvement in speed.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: Is it allowed to use non-interger weights?\n\n"
		"A: If a constraint is allowed values under 100%, you can use any weight, even fractional numbers like 99.75%. It might help "
		"in constraints like min days, preferred rooms or max hours daily.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";


	s+=tr("Q: With max 5 hours per day and 2 max gaps per week, in 4 cases it resulted 3 lesson+2 gaps + 2 lesson that is not acceptable,"
		" cause other day only 2 lesson, I mean I don't want gaps on same day, and gaps only in extra cases extend the hours, "
		"how can I keep the balance in this?\n\n"
		"A: Then you have to add 2 extra activities for a teacher, each with duration 1 and students empty and dummy subject"
		", and max gaps for this teacher 0.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: I have a sports room which allows more than 1 activity simultaneously in it. How to make the preferred room constraints"
		"? Can FET accept more than one activity at the same time in a single room?\n\n"
		"A: Each room can host a single activity at the same time. How to implement what you need? You can add more "
		"rooms (sport1, sport2, sport3) and instead of a single preferred room add more preferred rooms.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: Help me! I got an impossible to solve timetable\n\n"
		"A: If you get an impossible timetable, maybe the constraints students (set) early are too difficult. "
		"Maybe you can allow more arrivals at second hour. Also teachers' min hours daily might be too strong."
		" Please also check the statistics to be correct. Remove other constraints until you get a possible timetable.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: Is it possible to work with 0 hour?\n\n"
		"A: A bit difficult. You have to choose a day for this 0 hour. Then add breaks or not available to "
		"prevent other activities in other days at hour 0, then add students set not available to prevent other "
		"students set from having hours at this hour 0. Or variants of this.\n\n"
		"Mr. Zsolt Udvari used another trick: considered the last hour to be hour 0. But this is not always applicable.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: How does FET care about weights. What do they mean?\n\n"
		"A: The weights under 100% are subjective, because FET can skip them if necessary.\n\n"
		"Volker Dirr tried to express how FET considers under 100% weights, but his words are not completely reflecting the fact:\n\n"
		"Here is a comment from Volker Dirr:\n\n"
		"weight = 50% means: In average FET retries two times to place an activity without a conflict. If it isn't able to place the "
		"activity without a conflict after avarage 2 times it keeps the conflict and tries to place the next activity.\n"
		"weight = 75% means: In average FET retries four times to place an activity without a conflict. If it isn't "
		"able to place the activity without a conflict after avarage 4 times it keeps the conflict and tries to place the next activity.\n"
		"weight = 99% means: In average FET retries 100 times to place an activity without a conflict. "
		"If it isn't able to place the activity without a conflict after avarage 100 times it keeps the conflict and tries to place the next activity.\n"
		"weight = 99.99% means: In average FET retries 10000 times to place an activity without a conflict."
		" If it isn't able to place the activity without a conflict after avarage 10000 times it keeps the conflict and tries to place the next activity.\n\n"
		"This is not 100% correct. Activities might get unallocated, and cycle reopened.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Modified on 28 June 2008:");
	s+="\n\n";
	s+=tr("Q: Help on constraint activities preferred starting or preferred time slots (a set of activities has a set of preferred starting or time slots)\n\n"
		"A: You can specify a set of activities by selecting a teacher (if empty - all teachers), a students set "
		"(if empty - all students), a subject (if empty - all subjects) and an activity tag (if empty, all activity tags) and a set of allowed days.\n\n"
		"Starting means that an activity may only start at these periods.\n\n"
		"Time slots means more restrictive, that activity may only start and end and take place in these intervals ("
		"if activity has duration 2 and on Monday is allowed 8:00, 9:00 and 10:00, then activity can only start at 8:00 or 9:00).");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: Help on statistics menu\n\n"
		"A: The statistics are important to check your data before generating.\n\n"
		"When seeing the students statistics, probably the most useful are for subgroups. Each subgroup should have a "
		"reasonable amount of hours, and if you are having subgroups with too little hours, like 2 per week, probably "
		"you misunderstood FET notation. Each subgroup is independent of the others and should have a number of hours per "
		"week close to the average of all subgroups. Do not input empty subgroups with only a few activities.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: What if I enter accidentally duplicate constraints?\n\n"
		"A: It is not a good practice to allow unnecessary duplicate constraints. For min days between activities and other constraints"
		", the combined weight becomes very high. For instance, if you have 2 constraints with weight 95%, the outcome is "
		"a constraint with weight 100%-5%*5%=99.75%, very high. You might get an impossible timetable with duplicates, so "
		"beware. For constraint activity(ies) preferred (starting time(s) or time slots) the chosen weight is the "
		"largest value, so you can use more constraints of this type for same activities.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Text added on 24 March 2008:");
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
		"    t1_a_x_!ENG_FRE_GER,\n"
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
		"Currently, the interface for students is difficult to use. I am thinking of that. Maybe it is more simple for you if you try to work on the xml .fet file.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Text added on 27 March 2008: (Q-1-27-March-2008)");
	s+="\n\n";
	s+=tr("Q: Example: I have 7 hours of Maths per 5 days week (7 is larger than 5). How to add correctly this split activity?\n\n"
		"Complete Question: I have a large container activity split into more activities than the number of days per week. "
		"How to add it and constraint min days between activities?\n\n"
		"A: If you add directly a container activity split into more than the number of days per week and also add a constraint "
		"min days between activities, it would be a very bad practice from the way the algorithm of generation works (it slows down the "
		"generation and makes it harder to find a solution).\n\n"
		"The best way to add the activities would be:\n\n"
		"1. If you add 'force consecutive if same day', then couple extra activities in pairs to obtain a number of "
		"activities equal to the number of days per week. Example: 7 activities with duration 1 in a 5 days week, then "
		"transform into 5 activities with durations: 2,2,1,1,1 and add a single container activity with these 5 "
		"components (possibly raising the weight of added constraint min days between activities up to 100%)\n\n"
		"2. If you don't add 'force consecutive if same day', then add a larger activity splitted into a number of "
		"activities equal with the number of days per week and the remaining components into other larger splitted activity. For "
		"example, suppose you need to add 7 activities with duration 1 in a 5 days week. Add 2 larger container activities, "
		"first one splitted into 5 activities with duration 1 and second one splitted into 2 activities with duration 1 ("
		"possibly raising the weight of added constraints min days between activities for each of the 2 containers up to 100%)");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Text added on 6 June 2008: (Q-1-6-June-2008)");
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
		"hours with 1 and constraint all the real activities (without A3) to take place in the first periods. "
		"You can do that by adding a subject tag to A1 and A2 named Early and a subject tag to A3 named Any, and constraint "
		"activities preferred time slots for subject tag Early to be in the first n-1 slots of each day.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Text added on 14 June 2008");
	s+="\n\n";
	s+=tr("It was added a possibility to add 2 constraints min days between activities when adding a split activity. "
		"Please read above the documentation related to constrain min days between activities (basically, if you "
		"add min days 2 (or 3), you get the possibility to add a second constraint with min days 1 (or 2)).");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Text added on 28 June 2008:");
	s+="\n\n";
	s+=tr("Q: What is the difference between preferred starting times and preferred time slots?\n\n"
		"A: Time slots is more restrictive, means all hours of an activity must be in the allowed intervals.\n\n"
		"Example: Preferred times Monday 8,9 and 10. If activity A has duration 2, then starting means that activity A "
		"can start at 8, 9 or 10 on Monday, while overall means that activity A can start on Monday at 8 or 9 (10 is not "
		"allowed, because the last hour of activity is not allowed there).");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Text added on 10 July 2008:");
	s+="\n\n";
	s+=tr("Q: What means constraint min gaps (hours) between a set of activities?\n\n"
		"A: I had many users wanting to put a gap between activities of same teachers or between activities with same teacher and "
		"subject, if they are on the same day. This constraint does that. If you want for instance to make teacher John to have "
		"at least one gap between all his activities, select filter 'John' and add all his activities to a "
		"constraint of this type. If you want to make teacher John to have at least one gap between all "
		"his Math activities, select filter 'John' and 'Math' and add all these activities to a constraint of this type.\n\n"
		"Please take care that the selected activities are not forced consecutive by constraint two activities consecutive or "
		"by constraint min days between activities which have consecutive if same day selected.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Text added on 11 July 2008:");
	s+="\n\n";
	s+=tr("Q: As most of our pupils are using special busses, the last lesson of the day must be at "
		"least the 5th (of 6 lessons). Year 10 for instance has 2 allowed beginnings of second hour, with 27 working hours in 5x6 hours per week.\n\n"
		"A: It is possible to add constraint students min 5 hours daily, but this is stronger that what you need, "
		"so the timetable is more difficult to generate (year 10 might have 4 hours on a day: 1:not here, 2: math, "
		"3: phys, 4: engl, 5: bio, 6: not here on a normal timetable).\n\n"
		"It is possible to use some trick, if you cannot find a solution with minimum 5 hours per day: add, for each "
		"lowest level set of year 10 (see * below), 3 dummy activities, which represent the max 2 beginnings at second "
		"hour and the minimum 1 finishing at the 5th period (2+1=3). The first 2 activities have activity tag ATBegin "
		"and last 1 activity ATEnd. Add 2 constraints a set of activities has a set of preferred time slots, ATBegin, "
		"first hour each day and last hour of each day allowed and rest not allowed and ATEnd, allowed only at last slot of "
		"each day. Optionally: add constraint students (set) early max begs at sec. hour = 0 for the involved students ("
		"may use here year or group). I think it should work nice, but you have to work a little bit more to input the data.\n\n"
		"* : if year 10 does not contain groups, add dummy activities for year 10. If year 10 contains only groups"
		", add for each group. If year 10 contains subgroups, add for each subgroup. The reason: if year 10 contains subgroups "
		"and you add dummy activity, it is constrained to be in the same time for all subgroups; if you add dummy activities "
		"for subgroups, they can be in different slots -> timetable might be easier to find.");
		

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Text added on 15 July 2008:");
	s+="\n\n";
	s+=tr("From Zsolt Udvari:");
	s+="\n\n";
	s+=tr("Q: * imho must have a constraint: a student (or set) must have activite "
		"at specified time /because in our school there is a worship in church "
		"at monday, 7:30, and the school begin at 8:00 - so every students must "
		"have an activite monday's first hour - and there is a class, where "
		"must be an 'empty' first hour.\n\n"
		"The problem (exactly): there's a class, 5A (11-12 years old), and "
		"they've 4 english hours, in two subgroups (5A1 and 5A2), with same(!) "
		"teacher (DIOR). They'll learn the other language (latin) in this "
		"subgroups, but they've 'only' 3 latin. And at the other hours they're "
		"together. So if we don't want gaps we must the english and latin hours "
		"be pair, and one english hour'll be alone. "
		"But these students are young so must escort them to lunch and can't be "
		"that they'll go in two subgroups to lunch. "
		"So, once the 5A1 hasn't first hour and 5A2 has english lesson and"
		"second vica versa (when we don't want gaps). But in monday...\n\n"
		"3 answers by Liviu:\n\n"
		"A1: Maybe you could use dummy activities to represent gaps at first hour. Say you "
		"allow 2 gaps at first hour, not on Monday. Then add a dummy activity for each "
		"subgroup (with subject tag ST) and activities preferred starting times "
		"subject tag ST allowed Tuesday first hour, Wed first hour, Th first hour, Fr "
		"first hour. It should work.\n\n"
		"Also allowed on the last slot of each day. But if you can have 2 last empty "
		"slots in a day, I think you cannot use this trick.\n\n"
		"A2: I understand that you want to allow 1 arrival at second hour, not on Monday.\n"
		"So, add hours+1: 7:30, 8, ...\n"
		"Add not available for each students set (or break - for all) on Tue, Wed, Thu, Fri, at hour 7:30.\n"
		"Add 1 activity Religion, all students, no teachers. Preferred starting time Mon 7:30 (or you can add more activities).\n"
		"Add constraint activity (one or more, as you added one or more Religion), "
		"Monday, 7:30, for the above activity/ies.\n"
		"Add constraint students early max 1 beginning at second hour.\n\n"
		"A3: Constraint activities preferred time slots creates gaps, so use this facility:\n\n"
		"Add hours from 7:30.\n\n"
		"Constraint activities preferred time slots, not allowed on Monday 7:30, 100%.\n\n"
		"Constraint break times (or not available for all students), not allowed Tue, Wed, Thu, Fri 7:30.\n\n"
		"Add constraint students early max 2 beginnings at second hour (max 2 "
		"beginnings, not 1, careful, because the Monday 7:30 is one and you allow "
		"another one).");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Text added on 15 July 2008:");
	s+="\n\n";
	s+=tr("Q: What type of files uses FET?\n\n"
		"A: FET uses text files, xml or html or txt or csv (comma separated values - for import/export). The used codec is UTF-8 and probably UTF-16 should work.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Text added on 4 August 2008:");
	s+="\n\n";
	s+=tr("Q: Help on constraint subactivities preferred starting or preferred time slots (a set of subactivities has a set of preferred starting or time slots)\n\n"
		"A: You select the component number of certain activities (say, if you have an activity split into 5 activities per week, you "
		"have components 1, 2, 3, 4 or 5). Only the selected number of this split activity (the corresponding component activity) will be constrained.\n\n"
		"You will also specify the set of subactivities by selecting a teacher (if empty - all teachers), a students set ("
		"if empty - all students), a subject (if empty - all subjects) and an activity tag (if empty, all activity tags) and a set of allowed days.\n\n"
		"Starting times means that an activity may only start at these periods.\n\n"
		"Time slots means more restrictive, that activity may only start and end and take place in these intervals (if activity has duration 2 "
		"and on Monday is allowed 8:00, 9:00 and 10:00, then activity can only start at 8:00 or 9:00).\n\n"
		"This is useful if you need for instance, if Maths lessons are 4-5 per week, to constrain that the first "
		"component and the second component must be early. You will add 2 constraints for that, with component number "
		"1 and 2, both with subject Maths. Or, if you want for activities split into 4 that 2 lessons are early and for "
		"activities split into 5 that 3 activities are early, add constraint Maths with split number 3, 4 and 5 (nice trick).\n\n"
		"Another thing: if you have 1 or 2 lessons per week for a subject, say biology, and want to constrain one "
		"of the components if there are 2 per week, and none if there is only 1, you can add such a constraint for component number=2.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Text added on 15 August 2008:");
	s+="\n\n";
	s+=tr("Q: Help on constraints teacher(s) or students (set) hourly interval max days per week\n\n"
		"A: This is a constraint suggested by users, to allow you to specify an hourly interval for students or teachers, and "
		"to say that in this interval they must work at most max days per week. This is useful if for instance you want teachers "
		"not to have more than 2 days per week activities in the last 2 hours of the day.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Text added on 16 August 2008:");
	s+="\n\n";
	s+=tr("Q: Help on constraint activities end students day (or activity ends students day).\n\n"
		"A: If you have activities which you want to put in the last slots of a day (like say the meetings with "
		"the class master), please use the new constraint a set of activities end students day (or singular activity ends "
		"students day). This constraint can have weight only 100%, because of the way the algorithm works.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";
	
	s+=tr("Text added on 16 August 2008:");
	s+="\n\n";
	s+=tr("Q: At our school there are a lot of teachers which work only a few hours a week. Of course it "
		"is really nasty to drive for one our to the school. So we set the constraint, that every teacher "
		"should work at least 2 hours a day. Unfortunately we have this year a teacher which only works 1h a "
		"week. As a result of this FET doesn't start to create a timetable. Any suggestions how "
		"to fix the problem without defining a constraint for every singular teacher?\n\n"
		"A: I have a nice trick: add a dummy activity, 1 hour duration, with only this teacher (no students sets, any subject), additional to the real activity.\n\n"
		"This trick just passed through my mind as I was trying to write you that you have to do it the hard way (add constraints for each teacher)");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";
	
	s+=tr("Text added on 17 August 2008:");
	s+="\n\n";
	s+=tr("Q: What about the automatic search for updates? Should I enable it?\n\n"
		"A: It is recommended to enable automatic search for updates. I didn't set it as default because people might be annoyed if I "
		"release too fast new versions. But if you can cope with that, it is recommended to always have the latest version and enable "
		"searching for updates at startup.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";
	
	s+=tr("Text added on 5 December 2008");
	s+="\n\n";
	s+=tr("Q: I have a double duration activity. Is it possible that it is spread over the break period, like:\n\n"
		"Activity Math, duration 2, id say 100\n\n"
		"Hour 10:00 Math (first hour of act. 100)\n"
		"Hour 11:00 Break\n"
		"Hour 12:00 Math (second hour of act. 100)\n\n"
		"A: No, the activity must respect the break, so it is before or after the break with all the hours of it.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";
	
	s+=tr("Q and A From Anestis Vovos: A very difficult to diagnose unresolved case\n\n"
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
		"...Not that I will fall again for it but based on the difficulty to diagnose on my part it will help others that might face the same problem.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: I want to define hard subjects (Math, Physics and Chemistry) and I want students not to have more than 1 (or another variant 2) difficult subjects in a row.\n\n"
		"A: Define activity tag 'Difficult' and add it to all MA, PH and CH lessons. Then add constraint maximum 1 (or 2) "
		"hours continuously for all students and an activity tag 'Difficult'. Please take care if you may have double lessons.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Q: (by Horatiu Halmajan) I met a situation: a teacher asks for maximum 2 working days, but these days "
		"should not be consecutive. Is there a way to implement it in .fet?\n\n"
		"The only (manual) way I could think of, is to set the teacher as unavailable on Tuesdays and Thursdays, thus "
		"leaving him available on Monday, Wednesday and Friday (any two of these are unconsecutive).\n\n"
		"Any other ideas...?\n\n"
		"A: I have another idea: choose 2 activities of this teacher which clearly cannot be on the same day, "
		"and add constraint min days between activities, 2 days, 100%.\n\n"
		"Or add a dummy activity for this teacher, split into 2 per week, min days = 2, with 100%. You just need to "
		"take care that this teacher has place for these dummy activities (enough slots in the day) and to consider "
		"these dummy activities as possible gaps, so if teacher has max gaps 2 then make max gaps for him 0.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";


	s+=tr("Q: (by Horatiu Halmajan) The students must have max 4 gaps per week, maximum 2 per day, continuous gaps. How to solve this?\n\n"
		"A: Add for each subgroup a dummy activity (no teachers) split into 4 per week, duration 1, min days between "
		"activities 1, weight 0%, select consecutive if same day. FET will never put more than 2 of these dummy activities "
		"in a day. Add max gaps for students = 0 per week.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Advice: to treat gaps for students or teachers, if FET constraints are not enough, you can use dummy activities."
		" For instance, if a students set can have maximum 3 gaps, add an activity split into 3 per week, with no teachers.");
		
	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Question 1/16 August 2009: How to add constraint two activities grouped, two activities consecutive and three "
		"activities grouped if the activities are constrained not to be in the same day by constraints min days between activities?\n\n"
		"If A1 and A2 are constrained not to be in the same day with 95% weight or any other weight, it is a bad practice "
		"to add a constraint grouped or consecutive to them. If they are constrained with weight 100% not to be in the "
		"same day, the timetable is impossible; if the weight is under 100%, the timetable is more difficult to find than "
		"using the correct way.\n\n"
		"The correct way would probably be to consider A1 and A2 = a single activity A12', or to modify the related constraint "
		"min days between activities. Or maybe you can find other ways.");
		
	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Question 1/25 September 2009: An observation for constraint teacher(s) or students (set) activity tag max hours daily:\n\n"
		"This constraint is implemented correctly and is working good, but it is not perfect, which means that in unusual, extreme cases the "
		"time needed to generate a timetable might be longer or much longer than really necessary. You should give FET a hand in these extreme "
		"situations.\n\n"
		"Notation: ATS=affected teacher or students set, means teachers or students sets affected by (included in) this constraint activity tag max hours daily "
		"(the problem does not appear for teachers or students sets which are not affected, which have no constraint activity tag max hours daily for them).\n\n"
		"For extreme cases, the timetable generation might be longer or much longer than it should be. These cases refer to "
		"situations in which the total duration of the activities with the specified activity tag of the ATS is high compared to the "
		"total number of hours of all activities of the ATS, combined with other conditions you have in the data."
		" If the ratio of duration of activities with this activity tag of the ATS over the total duration of activities of the ATS"
		" is over 0.50 - 0.75 or a normal (reasonable low) value, and you use constraints to control gaps or early for ATS, and the number"
		" of hours per day is higher than the possible hours for the ATS,"
		" the speed of generation might be slower or much slower than it should be.\n\n"
		"In these cases, you are advised to use constraints teacher(s) or students (set) max hours daily for the ATS"
		" (without activity tag), or not available constraints for them in slots which are clearly impossible.\n\n"
		"For example, if students S have 20 hours of activities with activity tag AT and another 4 hours "
		"of activities without activity tag (they have 24 hours in total, the ratio is 20/24"
		"=0.80), and you constrain them to 0 gaps per week and 0 beginnings at second hour and also the number "
		"of hours per day is large, say 12. Then adding a constraint students activity tag AT max 4 hours per day "
		"- alone - will not be good enough (the time to generate a timetable might be longer or much longer than necessary)."
		" A better solution will need you to add also a constraint students max 5 "
		"hours daily (if possible) and/or to make the late slots not available for the students (it is a way to "
		"guide FET to the solution), or find other good ways to compensate the situation.\n\n"
		"Conclusion: if, for the ATS, the ratio 'tag duration'/'total duration' is over 0.50 - 0.75 AND you use, for the ATS,"
		" constraints for gaps or early AND for the ATS the number of available slots per week is much higher than ATS's total working number of hours per week,"
		" then you may need to address this problem (add constraints ATS max hours daily or ATS not available or find some other way to guide FET).\n\n"
		"So, it is a very bad idea to make all activities (of a teacher or students set) have an activity tag and add constraint teacher(s) or students (set)"
		" activity tag max hours daily. You should use in this case constraint teacher(s) or students (set) max hours daily.\n\n"
		"Probably, in practice this problem will not appear and you need not to worry. But theoretically it exists.");
		
	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";
	
	s+=tr("3 Hints from an anonymous Polish user, who uses FET for very large data (added on 7 October 2009):");
	s+="\n\n";
	s+=tr("The first hint for other users is to start with minimum number of constraints and if FET would generate "
		"the plan than thinking about adding the next ones.");
	s+="\n\n";
	s+=tr("The second hint is not to change too many constraints in one simulation as it may lead to impossible timetable "
		"and than it is difficult to say which particular constraint was too much.");
	s+="\n\n";
	s+=tr("For instance even when two consecutive activities are placed at one day "
		"(the same group and the same teacher) sometimes there are placed at different rooms what would force them to needless "
		"changing room. I guess that FET is focused on fulfilling constrains but not on optimizing timetable. Sometimes "
		"simple changing of two activities makes plan better both for students and teachers. I think that manually improving "
		"generated plan is faster than creating many many more constraints and repeating simulations. And this is the third hint.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";
	
	s+=tr("Q: Why the constraints activity tag max hours daily and students max gaps per day are disabled in the FET menu?");
	s+="\n\n";
	s+=tr("A: These 6 constraints (4 for activity tag and 2 for students max gaps per day) are the only ones which are not perfectly optimized"
		" in FET. For some combinations of constraints, they may slow down the generation or even make the timetable impossible. That is why"
		" they are not enabled by default. You need to activate them from the advanced settings menu. It is recommended to add such constraints"
		" at the end of your work, after you added all the other constraints and verified that your timetable is possible.");
	s+="\n\n";
	s+=tr("If these constraints are disabled, they have an icon attached in the menu to signify that. If they are enabled, they have another icon attached to them"
		", to signify that they are enabled, but must be used with caution.");
	s+="\n\n";
	s+=tr("Use these constraints with caution, not to obtain impossible timetables.");
	
	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";
	
	s+=tr("Q: Why don't you implement the not perfect constraints (activity tag max hours daily and students max gaps per day) in a perfect way?");
	s+="\n\n";
	s+=tr("A: Activity tag max hours daily cannot be implemented perfectly, generation would take too much (because of complexity of checks)."
		" Students max gaps per day can be implemented perfectly, but it would change the algorithm for students a lot and I am afraid to change"
		" something that is working well. Very much testing would be needed, and I have not enough sample files. While changing the algorithm, it would be"
		" easy to make critical bugs and some files may not solve anymore.");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";
	
	s+=tr("Entry added on 19 Nov. 2009.");
	s+="\n\n";
	s+=tr("Q: Help on shortcut buttons in the main form.");
	s+="\n\n";
	s+=tr("A: More users asked for shortcut buttons for the most commonly used functions. It is possible to show such buttons, if you select the corresponding option"
		" from the Settings->Interface menu (shortcuts are shown, by default).");

	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";
	
	s+=tr("Entry added on 29 Jan. 2010.");
	s+="\n\n";
	s+=tr("This is an advanced question which probably will never appear in practice, you may skip it at first reading.");
	s+="\n\n";
	s+=tr("Q: This is a problem that probably will never appear in practice, but it is possible. Say the user generates a timetable successfully, "
		"locks a few activities, then tries to generate again, but FET stops at a certain activity and reports impossible timetable.\n\n"
		"A: Indeed, this is a potential problem in FET (but probably will never show up in practice). It may happen if you have constraints with weight under 100%, which may be broken "
		"when you generate the timetable.\n\n"
		"Here is an example to explain this (it is an impractical example, but it is better as it is very simple): you have 4 students sets (Y1, Y2, Y3, Y4). "
		"5 activities: A1 (Y1,Y2,Y3,Y4), A2 (Y1), A3 (Y2), A4 (Y3), A5 (Y4). You have a single day per week and 2 hours per day. You add a constraint students "
		"max hours daily, max 1 hour, 95% weight percetange.\n"
		"1) Start to generate. After a while (maybe a few minutes), FET will be able to find a solution (with the max hours daily broken for all students sets).\n"
		"2) Then, you lock A2, A3, A4 and A5 and try to generate again. In some cases FET will report impossible activity A1.\n"
		"3) If you lock A1, A2, A3, A4 and A5, FET will be able to find a timetable very fast.\n\n"
		"1) Why can FET schedule the timetable the first time? Because in some cases FET will be able to break one-by-one all the 4 constraints max hours daily for A2, A3, A4 and A5 "
		"(no activity is locked, so it may try more times to place/replace the activities, and it will find a way to put A1, then put the rest of the activities).\n\n"
		"2) Why sometimes FET cannot schedule the timetable the second time (with locked A2, A3, A4 and A5)? "
		"Because: A2, A3, A4 and A5 are locked and are scheduled firstly. Then FET tries to put A1. But to put A1, it means "
		"to break a 95% constraint 4 times, because there are 4 students sets in A1. Weight 95% 4 times in a row is equivalent with a constraint with weight "
		"100%-(5%^4)=99.99999375%, which is a very strong constraint, which is very hardly broken. "
		"FET will retry more times for activity A1, so in some cases it will find a schedule even in these conditions.\n\n"
		"3) Why can FET find a timetable the third time (with locked A1, A2, A3, A4 and A5)? Because activities with more students sets are scheduled firstly (in general, locked activities"
		" are placed in descending order of the sum of the number of teachers and subgroups) and a locked activity is never rescheduled. "
		"So, FET puts A1 first, then A2, A3, A4 and A5. Since it retries more times separately for each activity, it is able to find a timetable easily.\n\n"
		"Practical solution to case 2)? Reduce weights of constraints which have weight below 100% or lock (to a corresponding slot) the activity which corresponds to A1 in your data file.");
	
	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Entry added on 15 Feb. 2010.");
	s+="\n\n";
	s+=tr("Q: I need constraint students (set) max days per week, similar to existing teacher(s) max days per week constraint.");
	s+="\n\n";
	s+=tr("A: Maybe it will be implemented in the future. Until then, please use constraint students (set) interval max days per week, interval = whole day.");
	
	s+="\n\n";
	s+="-------------------------------------------------------------------------------";
	s+="\n\n";

	s+=tr("Entry added on 27 Feb. 2010.");
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
	
	textBrowser->setText(s);
}

HelpFaqForm::~HelpFaqForm()
{
}
