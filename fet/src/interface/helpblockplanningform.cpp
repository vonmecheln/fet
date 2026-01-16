/***************************************************************************
                          helpblockplanningform.cpp  -  description
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "helpblockplanningform.h"

#include "timetable_defs.h"

HelpBlockPlanningForm::HelpBlockPlanningForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);
	
	plainTextEdit->setReadOnly(true);

	connect(closePushButton, &QPushButton::clicked, this, &HelpBlockPlanningForm::close);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	setText();
}

HelpBlockPlanningForm::~HelpBlockPlanningForm()
{
	saveFETDialogGeometry(this);
}

void HelpBlockPlanningForm::setText()
{
	QString s;
	
	s+=tr("Instructions for the block-planning mode.");
	s+="\n\n";
	s+=tr("Last modified on %1.").arg(tr("4 April 2025"));
	s+="\n\n";
	s+=tr("The block-planning mode was suggested, chronologically, by these users: %1, %2, and %3. You can follow a forum discussion about it here: %4",
	 "%1, %2, and %3 are three persons, %4 is an internet link").arg("Jude G").arg("ChicagoPianoTuner").arg("Darren McDonald")
	 .arg("https://lalescu.ro/liviu/fet/forum/index.php?topic=4151.0");
	s+="\n\n";
	s+=tr("The main ideas:");
	s+="\n\n";
	s+=tr("- The user %1 wanted to allocate the activities to teachers based on qualifications.", "%1 is a person").arg("Jude G");
	s+="\n\n";
	s+=tr("- The user %1 and then %2 suggested this: the FET days are real-life teachers, and the FET hours are real-life"
	 " time slots. Activities with common students cannot intersect in the real-life slots, so they cannot have overlapping FET hours."
	 " Similarly, if rooms are assigned, activities with common rooms also cannot have overlapping FET hours. Note that rooms can only"
	 " be assigned if \"teacher\" activities are used (see the note about speeding up timetable generation below).",
	 "%1 and %2 are two persons").arg("Jude G").arg("ChicagoPianoTuner");
	s+="\n\n";
	s+=tr("- The user %1 suggested these:", "%1 is a person").arg("ChicagoPianoTuner");
	s+="\n\n";
	s+=tr("    FET days are real teachers.");
	s+="\n\n";
	s+=tr("    FET students are real students.");
	s+="\n\n";
	s+=tr("    FET subjects are real subjects. Subjects preferred starting day can be used to link real teachers (FET days) to subjects.");
	s+="\n\n";
	s+=tr("    Each activity has 1 student. It is placed on a FET day based on its subject.");
	s+="\n\n";
	s+=tr("    Constraint \"set of activities occupies max time slots from selection\" is used to make sure that activities all overlap to make a"
	 " course. E.g. if 8 students want art (1 block, FET duration = 2), I can add all 8 art activities to the constraint and make max time"
	 " slots = 2 (selecting only art teacher's FET day).");
	s+="\n\n";
	s+=tr("    Constraint \"activity tags not overlapping\" (each tag = the subject) ensures that activities with different subjects aren't placed"
	 " at the same hour on the same FET day (real teacher).");
	s+="\n\n";
	s+=tr("    Constraint \"activities max simultaneous from a set in selected time slots\" can be used to make sure that no section of a subject has"
	 " too many students enrolled. My notation: a \"section\" is an instance of a subject. So the subject is \"physics\" and if there are 40 students"
	 " taking physics, that is too big for one group. So there must be two \"sections\" of that subject. They may be with the same teacher or"
	 " different teachers - it depends on what is desired. If I want the maximum number of students to be 22, I can use \"max simultaneous from"
	 " a set in selected time slots\".");
	s+="\n\n";
	s+=tr("- %1 suggested this:", "%1 is a person").arg("Liviu Lalescu");
	s+="\n\n";
	s+=tr("    Constraint \"a set of activities occupies max time slots from selection\" can be used to ensure that a teacher doesn't have too"
	 " many blocks of activities assigned.");
	s+="\n\n";
	s+=tr("    Constraint \"max total activities from a set in selected time slots\" can be used with an overflow/fake block (see below) to"
	 " restrict the allowed number of activities that cannot be placed in a \"real\" block.");
	s+="\n\n";
	s+=tr("- %1 suggested this:", "%1 is a person").arg("Darren McDonald");
	s+="\n\n";
	s+=tr("    Constraint \"Break\" under the Time tab can be used to mark blocks in which a teacher is unavailable.");
	s+="\n\n";
	s+=tr("- With ideas from %1, %2, and %3:", "%1, %2, and %3 are three persons").arg("ChicagoPianoTuner").arg("Darren McDonald").arg("Liviu Lalescu");
	s+="\n\n";
	s+=tr("    If not all the options of the students can be respected, add one or more overflow/fake block, at the end of the day, so that"
	 " impossible activities get scheduled there. And constrain the maximum number of activities there with the new constraint designed especially"
	 " for the block-planning mode, max total activities from a set in selected time slots. You could use a progressive approach, decreasing the"
	 " maximum allowed total number of activities in the overflow/fake slot(s) until no solution is possible anymore.");
	s+="\n\n";
	s+=tr("- With ideas from %1 and %2:", "%1 and %2 are two persons").arg("ChicagoPianoTuner").arg("Darren McDonald");
	s+="\n\n";
	s+=tr("    While activity tags can be used with the constraint \"activity tags not overlapping\" to prevent a teacher who can teach more"
	 " than one subject from having those subjects placed in the same block, this may also prevent impossible activities from being placed in"
	 " the fake/overflow block. There are at least two ways to address this: adding fake teachers, or adding additional fake/overflow slots.");
	s+="\n\n";
	s+=tr("    As an example of the first approach, if you have 40 distinct courses, you could add extra teacher slots (FET days) so that there"
	 " are 40 days. Then, adjust your \"A set of activities has a set of preferred starting times\" constraints (used to ensure activities are"
	 " allocated to the correct teacher(s)) so that, for example, all S1 courses have to fall on day T1 in \"real\" blocks, or in the fake/overflow"
	 " blocks of any other teacher.");
	s+="\n\n";
	s+=tr("    As an example of the second approach, one could add a number of fake blocks equal to the maximum number of unique courses taught"
	 " by any teacher. For example, if one teacher has at most 3 distinct subjects they might teach, then 3 fake/overflow blocks could be used.");
	s+="\n\n";
	s+=tr("- The user %1 described the following use case:", "%1 is a person").arg("Darren McDonald");
	s+="\n\n";
	s+=tr("I am planning to use a two-stage process: using the 'Block-planning' mode to plan blocks, then using the 'Official' mode to prepare a final"
	 " timetable. A little bit more detail about my timetabling situation may help explain.");
	s+="\n\n";
	s+=tr("My school offers the International Baccalaureate Diploma programme (as do many international schools), which is a two year programme"
	 " offered in the final two years of high school, and block timetabling is useful for this group. Students choose 6 subjects to study over"
	 " two years, and will choose 3 HL (Higher Level) courses, and 3 SL (Standard Level) courses. (HL courses require more time each week,"
	 " but this isn't important at this stage.) Enrollment in courses will not be very uniform: for example, we may have only 5 students taking"
	 " Music SL, but 14 students taking Geography SL, and 50 students taking Mathematics SL, so, there's only need for one section (one standard"
	 " FET activity) for both Music SL and Geography SL, but 3 sections (3 standard FET activities) for Mathematics SL (since our class sizes are"
	 " capped at 17). In a standard FET timetable, each of these activities would have several subactivities (without going into the details,"
	 " SL courses would have 5, HL courses 7, in a two-week timetable).");
	s+="\n\n";
	s+=tr("Now if Geography SL and Music SL have no students (or teachers) in common, they can occur at the same time, on the same day"
	 " (say, at 9am on Monday) in the standard FET timetable. However, if they have no students in common, all of their subactivities can be"
	 " timetabled together (Tuesday at 12pm, Wednesday at 11am, etc.), which then forms a block, a group of courses that run simultaneously"
	 " in all of their instances. The question, then, is which other courses can added to that block? Well, one section of Mathematics SL could"
	 " also join that block, but only if that section of Mathematics SL has no students in common with Geography SL and Music SL. Because we"
	 " need to run multiple sections of several courses (Chemistry SL, Biology SL, Mathematics SL, etc.), the block timetabling question"
	 " to be answered has two parts: which assignments of students to sections allows us to form blocks (subject/section groups) in which"
	 " each student is able to take the courses they've selected, and what are the resulting blocks?");
	s+="\n\n";
	s+=tr("To answer that question, the block-planning mode can be used, using constraints as suggested by %1. At my school we are using seven"
	 " blocks, so in block-planning I need only 7 hours per day. Geography SL is a subject, with one activity for each student enrolled in the"
	 " course. Similarly, Mathematics SL is a subject, with one activity for each student. At this stage, then, there is no need to take into"
	 " account the number of times a course meets in a week, etc., so one activity per student per course is all that's required. A successfully"
	 " generated block-planning mode timetable will allow me to identify which courses should go in Block A (block-planning mode FET Hour 1),"
	 " Block B (block-planning FET Hour 2), etc., and which students should be allocated to each section. (This actually needs to be done only for"
	 " Grade 11, since students in Grade 12 can keep the same blocks they had in the previous year.)", "%1 is a person").arg("ChicagoPianoTuner");
	s+="\n\n";
	s+=tr("With that information, I could move to the official mode to generate the real timetable: Block A becomes an activity tag which"
	 " could be used to ensure activities in the same block get the same actual time, and I know that students assigned to a section have no"
	 " conflicts within the blocks.");
	s+="\n\n";
	s+=tr("- The user %1 described the following use case:", "%1 is a person").arg("ChicagoPianoTuner");
	s+="\n\n";
	s+=tr("There are real 8 blocks (A-H).");
	s+="\n";
	s+=tr("Students in grades 9 and 10 take one course in each block (8 courses total).");
	s+="\n";
	s+=tr("Each student in grades 9-10 has 6 \"mandatory\" courses that must be scheduled in a real block with weight 100% - easy to implement,"
	 " set of activities set preferred starting times, using a tag.");
	s+="\n";
	s+=tr("The remaining courses are listed in preference order. There are 4 preferences, ranked 1-4. It is desired that 2 of these fall in the"
	 " real blocks. The other 2 may be considered 'reserve' choices and should hopefully fall in fake blocks.");
	s+="\n\n";
	s+=tr("Two approaches using block-planning to treat this case were suggested by %1 and %2:", "%1 and %2 are two persons").arg("ChicagoPianoTuner")
	 .arg("Darren McDonald");
	s+="\n\n";
	s+=tr("- If, say, you have 50 distinct courses and 40 teachers, you could add 10 fake teacher slots (FET days) so that there are 50 teacher"
	 " slots (in general, max(number of courses, number of teachers) teacher slots would be required). Then, adjust your \"A set of activities has"
	 " a set of preferred starting times\" constraints (used to make sure the right teachers get the right courses) so that, for example,"
	 " all S1 courses have to fall on day T1 in Blocks A-H or in any of the F1-F4 time slots (on any day). Then all the unsatisfiable preference"
	 " 1 choices could be distributed along that F1 \"row\" without overlapping tags.");
	s+="\n\n";
	s+=tr("- One could also add a number of \"fake\" hours equal to the number of choices (4 in this case) times the max number of unique courses"
	 " taught by any teacher (3 in this toy example), so instead of 4 fake blocks, I have 12: F1_a, F1_b, F1_c, F2_a,... , F4_c."
	 " Then \"first choice\" could happen in F1_a, F1_b, or F1_c, and \"second choice\" in F2_a, etc.");
	s+="\n\n";
	s+=tr("- The users %1 and %2 suggested that the following method may help improve the speed of timetable generation:", "%1 and %2 are two persons")
	 .arg("Volker Dirr").arg("ChicagoPianoTuner");
	s+="\n\n";
	s+=tr("In addition to having one activity per student choice, create one activity per teacher-section. E.g. if teacher T1 teaches"
	 " subject S1, S2, S2, S2, and S3 (five hours, three unique subjects), create five activities, no FET students, FET teacher = real teacher,"
	 " tag = \"teacher_activity\" or something like that, FET subject = [S1, S2, S2, S2, S3]. Use activities occupy max slots from selection to"
	 " link each of these activities to student selections, e.g. if 15 students selected S1, then activities occupy max slots would contain"
	 " those 15 activities, + 1 teacher activity, number of slots = 1, selected slots = real hours of FET day corresponding to real teacher."
	 " For S2, if 50 students requested that course, you would do 50 + 3 teacher activities, number of slots = 3, selected slots = real hours"
	 " of FET day corresponding to real teacher.");
	s+="\n\n";
	s+=tr("In my use case, I generated a timetable with a conflict score of 80 in about 40 minutes. Then I locked all activities with"
	 " tag = \"teacher_activity,\" and then generated again. Doing this brought my conflicts from 80 down to 38, and the second generation"
	 " took ~15 seconds.");
	s+="\n\n";
	s+=tr("Further instructions by %1, regarding tricks to make a two semester timetable:", "%1 is a person").arg("ChicagoPianoTuner");
	s+=" ";
	s+=tr("If your students have some different courses in semester 1 and semester 2, there are a few things you need to do. First, double"
	 " the number of hours in your FET day, creating an hour for each block in semester 1, and another for semester 2, including fakes."
	 " The structure should be A_sem1, B_sem1, ..., FAKE_1_sem1, ... A_sem2, B_sem2, FAKE_1_sem2, ... FAKE_N_sem2. Now, for subjects that"
	 " only last one semester, you can create one activity per student choice, as you did before. If you care which semester it's in, use"
	 " a set of activities has a set of preferred starting times and select the hours corresponding to the specific semester.");
	s+="\n\n";
	s+=tr("For subjects that last the entire year (both semesters), you need to create two activities for each student choice and corresponding"
	 " teacher activity. They should already be constrained to occur on specific FET days corresponding to real teachers. If there are multiple"
	 " teachers for the same subject, you need to add a constraint to each student activity must occur on the same day to ensure the students"
	 " have the same teacher for semester 1 as semester 2. If you want those activities to occur in the same block in semester 1 as semester 2"
	 " (which you probably do to avoid confusion, but it is not strictly necessary in some cases), add two constraints for each pair of"
	 " activities: \"min gaps (hours) between a set of activities,\" and \"max gaps (hours) between a set of activities\"."
	 " The number of hours, N, should be the same for both constraints. N should be equal to the number of real blocks + number of"
	 " fake blocks - 1, assuming the courses have duration 1 (each activity occupies exactly one block). If your courses occupy more than one"
	 " block, decrease the value of N by 1 for each course duration.");
	s+=" ";
	s+=tr("The constraint \"max gaps (hours) between a set of activities\" was designed/introduced in the block-planning mode.");
	s+="\n\n";
	s+=tr("If you would like to keep the same sections in two groups of activities, you could use the constraint of type 'Two sets of"
	 " activities have the same sections', introduced on %1, suggested by %2 and %3. See its dialog for Help about it.",
	 "%1 is a date, %2 and %3 are two persons").arg("2 April 2025").arg("Edward Downes").arg("Richard B. Gross");
	s+="\n";

	plainTextEdit->setPlainText(s);
}
