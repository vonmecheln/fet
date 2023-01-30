2018-07-28 (28 July 2018), Liviu Lalescu:

The code for students max span per day, students early max beginnings at second hour, students/teachers max gaps per day/week,
students/teachers min/max hours daily can and should be theoretically corrected. But it is very risky. Many examples and variants should be tested.
See the directory doc/algorithm/2018-07-28-improving-theoretically for a better generate file, but which behaves worse on at least a file,
examples/Romania/Pedagogic-High-School-Tg-Mures/2007-2008_sem1-d-test-students-max-span-per-day.fet (because of the new code in
students max span per day, if students early max beginnings >= 1 - if we try to remove an activity from begin only first hour. (You need to
generate more timetables, the speed seems to decrease with 25%-50%.)

The old stable code should be the one from the official FET-5.37.1. You can use a diff tool such as KDiff3 to compare with the new and theoretically
better code, to see the changes which could and better should be done in the future, but after very heavy testing. And after solving the
problem with the above problematic input file.


2023-01-25 (25 January 2023), Liviu Lalescu:

With this problematic example file above and Vangelis Karafillidis' suggestion of adding constraints of type activity preferred day (to lock
activities on a certain day), we discovered a bug in the constraint of type students (set) max span per day exactly for this situation
in which students early max beginnings >= 1. It was a cycling bug in some situations, when it should have not cycled. I wrote in the official FET code
a somehow similar code to the theoretically improving code listed here. The files of this type are indeed solved a bit slower, but they are not cycling
anymore.
