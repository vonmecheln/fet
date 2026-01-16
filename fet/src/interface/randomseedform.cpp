//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu (Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address))
// Copyright (C) 2020 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"

#include "randomseedform.h"

#include "longtextmessagebox.h"

#include "generate.h"

extern const QString COMPANY;
extern const QString PROGRAM;

//extern MRG32k3a rng;
extern Generate gen;

RandomSeedForm::RandomSeedForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);
	
	connect(helpPushButton, &QPushButton::clicked, this, &RandomSeedForm::help);
	connect(okPushButton, &QPushButton::clicked, this, &RandomSeedForm::ok);
	connect(cancelPushButton, &QPushButton::clicked, this, &RandomSeedForm::cancel);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	component1GroupBox->setTitle(tr("Component 1: each from min %1 to max %2, not all zero.",
	 "Component 1 has 3 values, each is constrained to be >= %1 and <=%2, and not all 3 should be 0. Keep the translation short.")
	 .arg(0).arg(gen.rng.m1-1));
	component2GroupBox->setTitle(tr("Component 2: each from min %1 to max %2, not all zero.",
	 "Component 2 has 3 values, each is constrained to be >= %1 and <=%2, and not all 3 should be 0. Keep the translation short.")
	 .arg(0).arg(gen.rng.m2-1));

	s10LineEdit->setText(QString::number(gen.rng.s10));
	s11LineEdit->setText(QString::number(gen.rng.s11));
	s12LineEdit->setText(QString::number(gen.rng.s12));

	s20LineEdit->setText(QString::number(gen.rng.s20));
	s21LineEdit->setText(QString::number(gen.rng.s21));
	s22LineEdit->setText(QString::number(gen.rng.s22));

	//s10LineEdit->selectAll();
	//s10LineEdit->setFocus();
}

RandomSeedForm::~RandomSeedForm()
{
	saveFETDialogGeometry(this);
}

void RandomSeedForm::help()
{
	QString s;
	
	s+=tr("You can set the initial state (seed) of the random number generator of FET using this option.")+"\n\n";

	s+=tr("FET uses the random number generator named %1, authored by %2 (used with permission).", "%2 is a person")
	 .arg("MRG32k3a").arg("Pierre L'Ecuyer")+" "
	 +tr("(The original code was a bit modified to use 64-bit integer operations instead of the not so reliable 'double' real number format.)"
	 , "It is indicated to keep the word 'double' as in English, but you are allowed to translate it if you consider this to be more appropriate.")
	 +" "+tr("This random number generator has a large period of about %1 or about %2 and an internal state of %3 32-bit integers.",
	 "%1 and %2 are variants of writing the size, %3 is the number of integers in the internal state").arg("2^191").arg("3*10^57").arg(6)+"\n\n";
	
	s+=tr("Reference for the %1 random number generator code:", "%1 is the name of the random number generator").arg("MRG32k3a");
	s+="\n\n";
	s+=QString("P. L'Ecuyer, ``Good Parameter Sets for Combined Multiple Recursive Random Number Generators'', Shorter version in Operations Research, 47, "
	 "1 (1999), 159--164.\n<https://pubsonline.informs.org/doi/abs/10.1287/opre.47.1.159>");
	s+="\n\n";
	s+=tr("Includes ideas and code from the files: %1, %2, and/or %3.", "%1, %2, and %3 are files from the internet")
	 .arg("https://simul.iro.umontreal.ca/rng/MRG32k3a.h").arg("https://simul.iro.umontreal.ca/rng/MRG32k3a.c")
	 .arg("https://www.iro.umontreal.ca/~lecuyer/myftp/papers/combmrg2.c");
	s+="\n\n";
	s+=tr("Used with permission from the author, %1 (%2).", "%1 is the author of the used random number generator, %2 is the date of email "
	 "correspondence when we got the permission to use it in FET.").arg("Pierre L'Ecuyer").arg(tr("9 March 2020"));
	s+="\n\n";
	
	s+=tr("The random seed is the state of the random number generator.")+" "+tr("It has two components, each with 3 components in turn.")+" "+
	 tr("Component %1 consists of 3 integers named %2, %3, and %4, and each one should be initialized with an integer value between minimum %5 and maximum %6, "
	 "and not all of them should be zero.", "%1 is the number of the component, 1 or 2.").arg(1).arg("s10").arg("s11").arg("s12").arg(0).arg(gen.rng.m1-1)+" "+
	 tr("Component %1 consists of 3 integers named %2, %3, and %4, and each one should be initialized with an integer value between minimum %5 and maximum %6, "
	 "and not all of them should be zero.", "%1 is the number of the component, 1 or 2.").arg(2).arg("s20").arg("s21").arg("s22").arg(0).arg(gen.rng.m2-1)+"\n\n";

	s+=tr("The random seed before the generation of a timetable (the %1 integer components) and the random seed after the generation are both saved on the "
	 "disk in the corresponding timetables directory, so that you can replicate the same generation after that and verify that the random seed after the "
	 "generation corresponds.", "%1 is the number of integer components of the random seed.").arg(6)+"\n\n";
	s+=tr("Mode of operation: if the FET version is the same (or if the generation code did not change), if the input file is identical, and if the starting "
	 "random seed is exactly the same, the generation and the results should be identical on any generation process on any computer.")+"\n\n";
	s+=tr("If you generate multiple timetables, the generation will follow the same path, under these conditions. So, if you generate 10 timetables twice starting "
	 "with the same random seed, the results will be the same, pairwise.")+" "
	 +tr("Note: of course, you need exactly the same conditions to replicate the same generation (so, you need exactly the same data - activities, "
	 "constraints, etc., in the exact same order - well, sometimes the order might be different and you can still replicate the same generation, because the "
	 "internal representation in FET will be the same, but this will not be generally true - in other words an identical file).")+"\n\n";
	s+=tr("Note: for different versions of FET, the behavior of generation may be totally different, even for the same data and the same random seed, "
	 "due to possible changes in the generation algorithm.")+"\n\n";
	s+=tr("The initialization of the random seed when you start FET is done using the elapsed time since the beginning of 1 January 1970, expressed in seconds and "
	 "nanoseconds. %1 and %2 will be the number of seconds (they will be both initialized from a variable computed a single time, so they will be equal), "
	 "%3 and %4 will be the current number of nanoseconds (modulo %5) (they might slightly differ, depending on the computer and compiler; they might slightly "
	 "differ because they are computed one after the other, not both at exactly the same moment), and the remaining components (%6 and %7) will be %8 "
	 "(this should not be a problem).", "%1, %2, %3, %4, %6, and %7 are components of the random seed, %5 is a number, %8 is a number.")
	 .arg("s10").arg("s20").arg("s11").arg("s21").arg(1000000000).arg("s12").arg("s22").arg(0)
	 +" "+tr("It is very important to note that if you open two or more instances of FET when the number of seconds since the beginning of 1 January 1970 is the same "
	 "(that is, if you start them at the same time), you should check that the initial random seeds will not be identical value-per-value, which would lead "
	 "to the same timetable generation behavior for an identical input file (hopefully this will not happen, since the elapsed nanoseconds will probably "
	 "be different, but please check). If you intend to generate different timetables for the same file using more FET instances, you need to ensure that "
	 "the starting random seeds are different (you can change them also manually, from the menu Settings->Advanced->Seed of random number generator).")
	 +" "+tr("Of course, if you use the same FET instance, you don't need to "
	 "be worried about same starting random seeds for the same input file, since the period of the random number generator is very large.")+" "
	 +tr("After you generate (even partially), the random seed will change very much (each call of the random number generator updates the random seed "
	 "components to the next numbers in the sequences, and there are many calls to this random number generating routine from the generate function).")+"\n\n";
	s+=tr("This setting is useful for more purposes; one purpose is bug report: send your file along with the random seed at the start of the generation "
	 "and, if available, the random seed at the end of the generation (the %1 components are saved in the timetable directory at the start and at the end "
	 "of the generation).", "%1 is the number of components of the random seed.").arg(6);
	
	LongTextMessageBox::largeInformation(this, tr("FET Help"), s);
}

void RandomSeedForm::ok()
{
	QString errors;
	
	bool ok;
	bool okNumber1=true;
	bool okNumber2=true;

	qint64 s10=s10LineEdit->text().toLongLong(&ok);
	if(!ok){
		errors+=tr("%1 is not a valid integer number.", "%1 is a variable name").arg("s10");
		errors+="\n";
		okNumber1=false;
	}
	else if(s10<0 || s10>gen.rng.m1-1){
		errors+=tr("%1 must be >= %2 and <= %3.", "%1 is the name of an integer variable, %2 is the minimum allowed value,"
		 " %3 is the maximum allowed value.").arg("s10").arg(0).arg(gen.rng.m1-1);
		errors+="\n";
	}
	qint64 s11=s11LineEdit->text().toLongLong(&ok);
	if(!ok){
		errors+=tr("%1 is not a valid integer number.", "%1 is a variable name").arg("s11");
		errors+="\n";
		okNumber1=false;
	}
	else if(s11<0 || s11>gen.rng.m1-1){
		errors+=tr("%1 must be >= %2 and <= %3.", "%1 is the name of an integer variable, %2 is the minimum allowed value,"
		 " %3 is the maximum allowed value.").arg("s11").arg(0).arg(gen.rng.m1-1);
		errors+="\n";
	}
	qint64 s12=s12LineEdit->text().toLongLong(&ok);
	if(!ok){
		errors+=tr("%1 is not a valid integer number.", "%1 is a variable name").arg("s12");
		errors+="\n";
		okNumber1=false;
	}
	else if(s12<0 || s12>gen.rng.m1-1){
		errors+=tr("%1 must be >= %2 and <= %3.", "%1 is the name of an integer variable, %2 is the minimum allowed value,"
		 " %3 is the maximum allowed value.").arg("s12").arg(0).arg(gen.rng.m1-1);
		errors+="\n";
	}

	qint64 s20=s20LineEdit->text().toLongLong(&ok);
	if(!ok){
		errors+=tr("%1 is not a valid integer number.", "%1 is a variable name").arg("s20");
		errors+="\n";
		okNumber2=false;
	}
	else if(s20<0 || s20>gen.rng.m2-1){
		errors+=tr("%1 must be >= %2 and <= %3.", "%1 is the name of an integer variable, %2 is the minimum allowed value,"
		 " %3 is the maximum allowed value.").arg("s20").arg(0).arg(gen.rng.m2-1);
		errors+="\n";
	}
	qint64 s21=s21LineEdit->text().toLongLong(&ok);
	if(!ok){
		errors+=tr("%1 is not a valid integer number.", "%1 is a variable name").arg("s21");
		errors+="\n";
		okNumber2=false;
	}
	else if(s21<0 || s21>gen.rng.m2-1){
		errors+=tr("%1 must be >= %2 and <= %3.", "%1 is the name of an integer variable, %2 is the minimum allowed value,"
		 " %3 is the maximum allowed value.").arg("s21").arg(0).arg(gen.rng.m2-1);
		errors+="\n";
	}
	qint64 s22=s22LineEdit->text().toLongLong(&ok);
	if(!ok){
		errors+=tr("%1 is not a valid integer number.", "%1 is a variable name").arg("s22");
		errors+="\n";
		okNumber2=false;
	}
	else if(s22<0 || s22>gen.rng.m2-1){
		errors+=tr("%1 must be >= %2 and <= %3.", "%1 is the name of an integer variable, %2 is the minimum allowed value,"
		 " %3 is the maximum allowed value.").arg("s22").arg(0).arg(gen.rng.m2-1);
		errors+="\n";
	}

	if(okNumber1 && (s10==0 && s11==0 && s12==0)){
		errors+=tr("Not all of component 1 values can be zero (at least one of %1, %2, or %3 must be nonzero).",
		 "%1, %2, and %3 are variable names.").arg("s10").arg("s11").arg("s12");
		errors+="\n";
	}

	if(okNumber2 && (s20==0 && s21==0 && s22==0)){
		errors+=tr("Not all of component 2 values can be zero (at least one of %1, %2, or %3 must be nonzero).",
		 "%1, %2, and %3 are variable names.").arg("s20").arg("s21").arg("s22");
		errors+="\n";
	}

	if(!errors.isEmpty()){
		LongTextMessageBox::largeInformation(this, tr("FET warning"), errors);
		return;
	}
	
	gen.rng.initializeMRG32k3a(s10, s11, s12, s20, s21, s22);
	
	this->close();
}

void RandomSeedForm::cancel()
{
	this->close();
}
