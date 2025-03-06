https://lalescu.ro/liviu/fet/forum/index.php?topic=4590.0

The words of Benahmed Adelkrim:

Attached below is an example of a FET file produced in a maximum of 8 minutes. It is a very good file for an Algerian school.

A one or two day exception was used for morning / afternoon behavior, depending on the number of weekly work hours per teacher.

The seeds of the random number generator at the beginning of production are:
  s10=4080711908, s11=1604716469, s12=2552074707,
    s20=3064904818, s21=2758619647, s22=2161489294.

This file has two students sets who do not have a fixed room(Mobile group). I've never used spatial restrictions. But it was compensated by the following time constraints:
1 - Max simultaneous activities from a set in selected time slots
2 - A students set's not available times: I used this constraint for a few of students sets every afternoon except Monday afternoon. because all the students set must be present for 2 hours in this afternoon, for the lesson of the pedagogical support(Mathematics, arabic, french) .

All students sets begins early with max beginnings at second hour= 0;

    All teachers get max gaps per week = 1;
    All teachers get the max working hours per day = 6;
    All students get the max gaps per week = 0;
    The min hours in the mornings = 3 for all students;

weight of min days = 99.75% for all activities divided into 4 sub-activities;
For all other activities weight = 100%.

I suggest adding this example in the examples folder for FET, in the next version.

note: with other random seeds, the time can be a matter of hours. sometimes he manages to put 572 activities / 573 in just 6 minutes or less(see the example of random seeds below), and remains the long hours to complete only the last activity!(9h 9m 55s in this example. see the screenshot attached below)

ex:
  s10=3438157813, s11=7629847, s12=3017819358,
    s20=3507393849, s21=428281730, s22=3548082366.
