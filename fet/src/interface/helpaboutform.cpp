/***************************************************************************
                          helpaboutform.cpp  -  description
                             -------------------
    begin                : Tue Apr 22 2003
    copyright            : (C) 2003 by Lalescu Liviu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <QCoreApplication>

#include <QString>

#include "helpaboutform.h"

#include "timetable_defs.h"

HelpAboutForm::HelpAboutForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closePushButton->setDefault(true);
	
	aboutTextBrowser->setReadOnly(true);
	authorsTextBrowser->setReadOnly(true);
	contributorsTextBrowser->setReadOnly(true);
	translatorsTextBrowser->setReadOnly(true);
	referencesTextBrowser->setReadOnly(true);
	thanksToTextBrowser->setReadOnly(true);
	
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	tabWidget->setCurrentIndex(0);
	
	aboutTextBrowser->setOpenExternalLinks(true);
	referencesTextBrowser->setOpenExternalLinks(true);
	
	//Trick to have the translations handy for future releases
	QString monthJan=QCoreApplication::translate("MonthsNames", "January");
	Q_UNUSED(monthJan);
	QString monthFeb=QCoreApplication::translate("MonthsNames", "February");
	Q_UNUSED(monthFeb);
	QString monthMar=QCoreApplication::translate("MonthsNames", "March");
	Q_UNUSED(monthMar);
	QString monthApr=QCoreApplication::translate("MonthsNames", "April");
	Q_UNUSED(monthApr);
	QString monthMay=QCoreApplication::translate("MonthsNames", "May");
	Q_UNUSED(monthMay);
	QString monthJun=QCoreApplication::translate("MonthsNames", "June");
	Q_UNUSED(monthJun);
	QString monthJul=QCoreApplication::translate("MonthsNames", "July");
	Q_UNUSED(monthJul);
	QString monthAug=QCoreApplication::translate("MonthsNames", "August");
	Q_UNUSED(monthAug);
	QString monthSep=QCoreApplication::translate("MonthsNames", "September");
	Q_UNUSED(monthSep);
	QString monthOct=QCoreApplication::translate("MonthsNames", "October");
	Q_UNUSED(monthOct);
	QString monthNov=QCoreApplication::translate("MonthsNames", "November");
	Q_UNUSED(monthNov);
	QString monthDec=QCoreApplication::translate("MonthsNames", "December");
	Q_UNUSED(monthDec);
	
	QString about=QString("");
	about+=tr("FET is free software for automatically scheduling the timetable of a school, high-school or university.");
	about+="<br /><br />";
	about+=tr("Copyright (C) %1 %2.", "%1 is the copyright period (a range between two years, more years separated by commas, "
	 "or a combination of these), %2 are the copyright holders").arg("2002-2021").arg("Liviu Lalescu, Volker Dirr");
	about+="<br /><br />";
	about+=tr("Version: %1 (%2 %3).", "%1 is the current FET version, %2 is the current release month, %3 is the current release year").arg(FET_VERSION)
	 .arg(QCoreApplication::translate("MonthsNames", "March")).arg("2021");
	about+="<br /><br />";
	about+=tr("Licensed under the GNU Affero General Public License version 3 or later.");
	about+="<br /><br />";
	about+=tr("FET homepage: %1", "%1 is the FET homepage").arg("<a href=\"https://lalescu.ro/liviu/fet/\">https://lalescu.ro/liviu/fet/</a>");
	about+="<br />";
	aboutTextBrowser->setHtml(about);
	
	QString authors=QString("");
	authors+=tr("%1 (%2)", "%1 is the name of an author, %2 is a method to contact him or her (email or webpage)")
	 .arg("Liviu Lalescu")
	 .arg("https://lalescu.ro/liviu/");
	authors+="<br /><br />";
	authors+=tr("%1 (%2)", "%1 is the name of an author, %2 is a method to contact him or her (email or webpage)")
	 .arg("Volker Dirr")
	 .arg("https://www.timetabling.de/");
	authors+="<br />";
	authors+=QString("&nbsp;&nbsp;&nbsp;&nbsp;- ")+tr("XHTML timetable export.");
	authors+="<br />";
	authors+=QString("&nbsp;&nbsp;&nbsp;&nbsp;- ")+tr("CSV import and export.");
	authors+="<br />";
	authors+=QString("&nbsp;&nbsp;&nbsp;&nbsp;- ")+tr("Advanced statistics print/export.");
	authors+="<br />";
	authors+=QString("&nbsp;&nbsp;&nbsp;&nbsp;- ")+tr("Speed improvements in the timetable generation.");
	authors+="<br />";
	authors+=QString("&nbsp;&nbsp;&nbsp;&nbsp;- ")+tr("Locking the activities.");
	authors+="<br />";
	authors+=QString("&nbsp;&nbsp;&nbsp;&nbsp;- ")+tr("Activity planning dialog.");
	authors+="<br />";
	authors+=QString("&nbsp;&nbsp;&nbsp;&nbsp;- ")+tr("Print timetable dialog.");
	authors+="<br />";
	authorsTextBrowser->setHtml(authors);
	
	QString contributors=QString("");
	contributors+=tr("Code contributors:");
	contributors+="<br /><br />";
	contributors+=tr("%1 (%2)", "%1 is the name of a contributor, %2 is a method to contact him or her (email or webpage)")
	 .arg("Rodolfo Ribeiro Gomes")
	 .arg("rodolforg AT gmail.com");
	contributors+="<br />";
	contributors+=QString("&nbsp;&nbsp;&nbsp;&nbsp;- ")+tr("An initial draft code for the constraints of type teacher(s) max span per day and teacher(s) min resting hours.");
	contributors+="<br />";
	contributors+=QString("&nbsp;&nbsp;&nbsp;&nbsp;- ")+tr("The alphabetic sorting of the items which respects the user's locale "
	 "(the special characters are correctly ordered).");
	contributors+="<br /><br />";
	contributors+=QString("Marco Vassura");
	contributors+="<br />";
	contributors+=QString("&nbsp;&nbsp;&nbsp;&nbsp;- ")+
	 tr("The colors of the timetables, in the timetable view dialogs and in the XHTML results (using CRC-24 based on RFC 2440 Section 6.1).",
	 "CRC means Cyclic Redundancy Check, RFC means Request for Comments. Please keep the fields CRC-24, RFC 2440, and 6.1 unmodified.");
	contributors+="<br />";
	contributorsTextBrowser->setHtml(contributors);
	
	QString translators=QString("");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("ar").arg(tr("Arabic translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("former translator: %1 (to contact %2 visit FET forum - %3, "
	 "section about Arabic translation, or contact forum user %4)", "%1 is the translator's complete name, %2 is his or her shorter or complete name, %3 is the FET forum address, "
	 "%4 is the username of the translator").arg("Silver").arg("Silver").arg("https://lalescu.ro/liviu/fet/forum/").arg("Silver");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("current translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Benahmed Abdelkrim").arg("pmg9.81 AT gmail.com");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("ca").arg(tr("Catalan translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("former translator: %1 (to contact %2 visit FET forum - %3, "
	 "section about Catalan translation, or contact forum user %4)", "%1 is the translator's complete name, %2 is his or her shorter or complete name, %3 is the FET forum address, "
	 "%4 is the username of the translator").arg(QString::fromUtf8("Sílvia Lag")).arg(QString::fromUtf8("Sílvia")).arg("https://lalescu.ro/liviu/fet/forum/").arg("silvia");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("current translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Innocent De Marchi").arg("tangram.peces AT gmail.com");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("cs").arg(tr("Czech translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Pavel Fric").arg("pavelfric AT seznam.cz");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("da").arg(tr("Danish translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("David Lamhauge").arg("davidlamhauge AT gmail.com");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("de").arg(tr("German translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("former translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Volker Dirr").arg("https://www.timetabling.de/");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("current translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Robert Hairgrove").arg("code AT roberthairgrove.com");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("el").arg(tr("Greek translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("former translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Dimitrios Ropokis").arg("wamy80s AT gmail.com");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("former translator: %1 (to contact %2 visit FET forum - %3, "
	 "section about Greek translation, or contact forum user %4)", "%1 is the translator's complete name, %2 is his or her shorter or complete name, %3 is the FET forum address, "
	 "%4 is the username of the translator").arg("Dimitris Kanatas").arg("Dimitris Kanatas").arg("https://lalescu.ro/liviu/fet/forum/").arg("Dimitris Kanatas");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("current translator: %1 (to contact %2 visit FET forum - %3, "
	 "section about Greek translation, or contact forum user %4)" ,"%1 is the translator's complete name, %2 is his or her shorter or complete name, %3 is the FET forum address, "
	 "%4 is the username of the translator").arg("Vangelis Karafillidis").arg("Vangelis Karafillidis").arg("https://lalescu.ro/liviu/fet/forum/").arg("Vangelis Karafillidis");
	translators+=QString(" - ")+tr("rewrote the translation from zero");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("es").arg(tr("Spanish translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("former translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address")
	 .arg(QString::fromUtf8("José César Fernández López")).arg("cesar.fernandez.lopez AT gmail.com");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("current translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address")
	 .arg(QString::fromUtf8("Emiliano Llano Díaz")).arg("compuvtt AT hotmail.com");
	translators+=QString(" - ")+tr("rewrote the translation from zero");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("eu").arg(tr("Basque translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Asier Urio Larrea").arg("asieriko AT gmail.com");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("fa").arg(tr("Persian translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("former translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Hamed SadeghiNeshat").arg("ha_sadeghi AT ce.sharif.edu");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("current translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("sally sat").arg("soory63 AT gmail.com");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("fr").arg(tr("French translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("former translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Jerome Durand").arg("fetfr AT free.fr");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("former translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Patrick Fox").arg("patrick.fox AT laposte.net");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("former translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address")
	 .arg(QString::fromUtf8("Régis Bouguin")).arg("regis.bouguin AT laposte.net");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("current translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Pascal Cohen").arg("pacohen AT laposte.net");
	translators+=QString(" - ")+tr("rewrote the translation from zero");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("gl").arg(tr("Galician translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (to contact %2 visit FET forum - %3, "
	 "section about Galician translation, or contact forum user %4)", "%1 is the translator's complete name, %2 is his or her shorter or complete name, %3 is the FET forum address, "
	 "%4 is the username of the translator").arg("Juan Marcos Filgueira Gomis").arg("marcos.filgueira").arg("https://lalescu.ro/liviu/fet/forum/").arg("marcos.filgueira");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("he").arg(tr("Hebrew translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Yotam Medini").arg("yotam.medini AT gmail.com");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("hu").arg(tr("Hungarian translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Ferenczi Viktor").arg("cx AT cx.hu");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("id").arg(tr("Indonesian translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Nirwan Yus").arg("ny.unpar AT gmail.com");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("it").arg(tr("Italian translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Marco Barsotti").arg("mbarsan AT gmail.com");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("ja").arg(tr("Japanese translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (to contact %2 visit FET forum - %3, "
	 "section about Japanese translation, or contact forum user %4)", "%1 is the translator's complete name, %2 is his or her shorter or complete name, %3 is the FET forum address, "
	 "%4 is the username of the translator").arg("Taro Tada").arg("Taro Tada").arg("https://lalescu.ro/liviu/fet/forum/").arg("Taro Tada");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("lt").arg(tr("Lithuanian translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Darius Staigys").arg("darius AT e-servisas.lt");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("mk").arg(tr("Macedonian translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Zoran Zdravkovski").arg("zoran AT pmf.ukim.edu.mk");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("ms").arg(tr("Malay translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Abdul Hadi Kamel").arg("hadikamel AT perlis.uitm.edu.my");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("nl").arg(tr("Dutch translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Niels Fikse").arg("k.fikse AT student.utwente.nl");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("pl").arg(tr("Polish translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Radoslaw Pasiok").arg("zapala AT konto.pl");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("pt_BR").arg(tr("Brazilian Portuguese translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("former translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Werner Bruns").arg("werner.bruns AT gmail.com");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("former translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address")
	 .arg(QString::fromUtf8("Frank Mártin")).arg("drfarofa AT gmail.com");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("former translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Cloves das Neves").arg("clovesneves AT gmail.com");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("current translator: %1 (%2). (Alternatively, to contact %3 visit FET forum - %4, "
	 "section about Brazilian Portuguese translation, or contact forum user %5)", "%1 is the name of the translator, %2 is his or her email or web address, "
	 "%3 is the shorter or complete name of the translator, %4 is the address of the forum, %5 is forum user name of the translator")
	 .arg("Alexandre R. Soares").arg("alexrsoares AT zoho.com").arg("Alexandre R. Soares").arg("https://lalescu.ro/liviu/fet/forum/").arg("khemis");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("ro").arg(tr("Romanian translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Liviu Lalescu").arg("https://lalescu.ro/liviu/");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("ru").arg(tr("Russian translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Ilya V. Paramonov").arg("ivparamonov AT gmail.com");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("si").arg(tr("Sinhala translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Achini Duisna").arg("duisna1012 AT gmail.com");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("sk").arg(tr("Slovak translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (to contact %2 visit FET forum - %3, "
	 "section about Slovak translation, or contact forum user %4)", "%1 is the translator's complete name, %2 is his or her shorter or complete name, %3 is the FET forum address, "
	 "%4 is the username of the translator").arg("Ondrej Gregor").arg("Ondrej").arg("https://lalescu.ro/liviu/fet/forum/").arg("Ondrej");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("sq").arg(tr("Albanian translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Artur Lugu").arg("ciaoartur AT yahoo.it");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("sr").arg(tr("Serbian translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Ivan Starchevicy").arg("ivanstar61 AT gmail.com");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Radan Putnik").arg("srastral AT gmail.com");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("tr").arg(tr("Turkish translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("former translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Mehmet Gezmisoglu").arg("m_gezmisoglu AT hotmail.com");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("former translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Mahir Nacar").arg("mahirnacar AT email.com");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("current translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Yakup Kadri Demirci").arg("yakup AT engineer.com");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("uk").arg(tr("Ukrainian translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("Andriy Melnyk").arg("melnyk.andriy AT gmail.com");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("uz").arg(tr("Uzbek translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2, or visit FET forum - %3, "
	 "section about Uzbek translation, or contact forum user %4)", "%1 is the current translator, %2 is his or her email or web address, %3 is the FET forum address, "
	 "%4 is the username of the translator").arg("Orzubek Eraliyev").arg("o.eraliyev AT gmail.com").arg("https://lalescu.ro/liviu/fet/forum/").arg("sarkor");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("vi").arg(tr("Vietnamese translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("former translator: %1 (to contact %2 visit FET forum - %3, "
	 "section about Vietnamese translation, or contact forum user %4)", "%1 is the translator's complete name, %2 is his or her shorter or complete name, %3 is the FET forum address, "
	 "%4 is the username of the translator").arg("Nguyen Truong Thang").arg("Thang").arg("https://lalescu.ro/liviu/fet/forum/").arg("NTThang");
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("current translator: %1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address")
	 .arg(QString::fromUtf8("Nguyễn Hữu Duyệt")).arg("nguyenhuuduyet AT gmail.com");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("zh_CN").arg(tr("Chinese Simplified translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("orange").arg("112260085 AT qq.com");
	translators+=QString("<br /><br /><br />");

	translators+=tr("%1 - %2", "%1 is the international abbreviation of the language, %2 is the name of the language, translated").arg("zh_TW").arg(tr("Chinese Traditional translation"));
	translators+=QString("<br /><br />&nbsp;&nbsp;&nbsp;&nbsp;");
	translators+=tr("%1 (%2)", "%1 is the name of the translator, %2 is his or her email or web address").arg("James").arg("james AT cc.shu.edu.tw");
	translators+=QString("<br />");
	
	translatorsTextBrowser->setHtml(translators);
	
	QString references=QString("");
	references+=tr("You may find references for the algorithms and techniques used in this program on the FET documentation web page, %1")
	 .arg("<a href=\"https://lalescu.ro/liviu/fet/doc/\">https://lalescu.ro/liviu/fet/doc/</a>");
	references+="<br />";
	referencesTextBrowser->setHtml(references);
	
	QString thanksTo=QString("");
	thanksTo+=tr("The following people, listed chronologically, participated in this project with suggestions, example files, "
	 "reports, and/or other kinds of contributions:");
	thanksTo+=QString("<br /><br />");
	thanksTo+=QString::fromUtf8("Costin Bădică");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Carsten Niehaus");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Imre Nagy");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Sajith V. K.");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Michael Towers");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Antti Leppik");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Ian Fantom");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Simon Ghetti");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Gibbon Tamba");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Jerome Durand");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Marek Jaszuk");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Ramanathan Srinivasan");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Vimal Joseph");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Cristian Gherman");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Nicholas Robinson");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Radu Spineanu");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Morten Piil");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Sebastian Canagaratna");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Abdul Hadi Kamel");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Miguel Gea Milvaques");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Frans de Bruijn");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Radoslaw Pasiok");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Daan Huntjens");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Yush Yuen");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Scott Sweeting");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Dragoș Petrașcu");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Daniel S.");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Gianluca Salvo");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Sebastian O'Halloran");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Mehmet Gezmisoglu");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Tom Hosty");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Niels Fikse");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Simon Bohlin");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Volker Dirr");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Les Richardson");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Gabi Danon");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Manolo Par");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Viktor Ferenczi");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Patrick Fox");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Andrés Chandía");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Zoran Zdravkovski");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Constantin Romulus");
	thanksTo+=QString("<br />");
	thanksTo+=QString("L. W. Johnstone");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Zsolt Udvari");
	thanksTo+=QString("<br />");
	thanksTo+=QString("mantas");
	thanksTo+=QString("<br />");
	thanksTo+=QString("moryus");
	thanksTo+=QString("<br />");
	thanksTo+=QString("bb");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Maciej Deorowicz");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("José César Fernández López");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Daniel Chiriac");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Dimitrios Ropokis");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Danail");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Peter Ambroz");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Nirwan Yus");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Marco Barsotti");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Silver");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Horațiu Hălmăjan");
	thanksTo+=QString("<br />");
	thanksTo+=QString("kdsayang");
	thanksTo+=QString("<br />");
	thanksTo+=QString("didit");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Bobby Wise");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Willy Henckert");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Wilfred");
	thanksTo+=QString("<br />");
	thanksTo+=QString("W. D. John");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Darius Staigys");
	thanksTo+=QString("<br />");
	thanksTo+=QString("George Miliotis [Ionio]");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Sílvia");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Chafik Graiguer");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Niels Stargardt");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Cristian Balint");
	thanksTo+=QString("<br />");
	thanksTo+=QString("sherman");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Azu Boba");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Thomas Schwartz");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Cătălin Maican");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Ilya V. Paramonov");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Hamed SadeghiNeshat");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Joan de Gracia");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Massimo Mancini");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Regina V. Kryvakovska");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("ßingen");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Angela");
	thanksTo+=QString("<br />");
	thanksTo+=QString("T. Renganathan");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Marco");
	thanksTo+=QString("<br />");
	thanksTo+=QString("sally sat");
	thanksTo+=QString("<br />");
	thanksTo+=QString("sstt2");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Nikos Koutsoukos");
	thanksTo+=QString("<br />");
	thanksTo+=QString("pinco");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Ben Bauer");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Rodica Lalescu");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Gigica Nedelcu");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Paolo Cataldi");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Gerrit Jan Veltink");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Soyeb Aswat");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Andriy Melnyk");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Frans");
	thanksTo+=QString("<br />");
	thanksTo+=QString("m");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Christoph Schilling");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Frank Mártin");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Werner Bruns");
	thanksTo+=QString("<br />");
	thanksTo+=QString("aliponte");
	thanksTo+=QString("<br />");
	thanksTo+=QString("David Lamhauge");
	thanksTo+=QString("<br />");
	thanksTo+=QString("murad");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Achini Duisna");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Ondrej Gregor");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Karel Rodríguez Varona");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Remus Turea");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Joachim");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Chichi Lalescu");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Iftekhar Ahmad");
	thanksTo+=QString("<br />");
	thanksTo+=QString("DT");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Yotam Medini");
	thanksTo+=QString("<br />");
	thanksTo+=QString("mohd");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Dimitris Kanatas");
	thanksTo+=QString("<br />");
	thanksTo+=QString("waleed");
	thanksTo+=QString("<br />");
	thanksTo+=QString("aang");
	thanksTo+=QString("<br />");
	thanksTo+=QString("M K Lohumi");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Régis Bouguin");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Ivan Starchevicy");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Radan Putnik");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Asti Widayanti");
	thanksTo+=QString("<br />");
	thanksTo+=QString("uni_instructor");
	thanksTo+=QString("<br />");
	thanksTo+=QString("liquid");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Juan Marcos Filgueira Gomis");
	thanksTo+=QString("<br />");
	thanksTo+=QString("llantones");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Christian Kemmer");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Davide G. M. Salvetti");
	thanksTo+=QString("<br />");
	thanksTo+=QString("lalloso");
	thanksTo+=QString("<br />");
	thanksTo+=QString("drew");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Fabio Piedimonte");
	thanksTo+=QString("<br />");
	thanksTo+=QString("K");
	thanksTo+=QString("<br />");
	thanksTo+=QString("skinkone");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Jonathan Block");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Nguyen Truong Thang");
	thanksTo+=QString("<br />");
	thanksTo+=QString("kdrosos");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Ian Holden");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Sarwan Bangar");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Petros Nouvakis");
	thanksTo+=QString("<br />");
	thanksTo+=QString("mma");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Orzubek Eraliyev");
	thanksTo+=QString("<br />");
	thanksTo+=QString("k1aas");
	thanksTo+=QString("<br />");
	thanksTo+=QString("nomad");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Robert Sutcliffe");
	thanksTo+=QString("<br />");
	thanksTo+=QString("rjmillett");
	thanksTo+=QString("<br />");
	thanksTo+=QString("yasin dehghan");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Daniel");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Pietro");
	thanksTo+=QString("<br />");
	thanksTo+=QString("arivasm");
	thanksTo+=QString("<br />");
	thanksTo+=QString("AZ");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Etlau");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Nemo");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Anton Anthofer");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Danny Zitzman");
	thanksTo+=QString("<br />");
	thanksTo+=QString("geaplanet");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Leandro Bueno");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Laid Messaoudi");
	thanksTo+=QString("<br />");
	thanksTo+=QString("karim");
	thanksTo+=QString("<br />");
	thanksTo+=QString("hicham_idrissi");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Davide Cottignoli");
	thanksTo+=QString("<br />");
	thanksTo+=QString("agemagician");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Vlăduț Frățiman");
	thanksTo+=QString("<br />");
	thanksTo+=QString("vlad2005");
	thanksTo+=QString("<br />");
	thanksTo+=QString("mouiata");
	thanksTo+=QString("<br />");
	thanksTo+=QString("rapsy");
	thanksTo+=QString("<br />");
	thanksTo+=QString("clouds");
	thanksTo+=QString("<br />");
	thanksTo+=QString("MarioMic");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Cloves das Neves");
	thanksTo+=QString("<br />");
	thanksTo+=QString("pedrobordon");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Tony Chan");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Artur Lugu");
	thanksTo+=QString("<br />");
	thanksTo+=QString("plaldw");
	thanksTo+=QString("<br />");
	thanksTo+=QString("jimmyjim");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Curtis Wilson");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Mohamed Bahaj");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Thomas Klausner");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Jörg Sonnenberger");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Boubker");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Alexey Loginov");
	thanksTo+=QString("<br />");
	thanksTo+=QString("_indianajones");
	thanksTo+=QString("<br />");
	thanksTo+=QString("russell");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Nguyen Huu Tuyen");
	thanksTo+=QString("<br />");
	thanksTo+=QString("fromturkey");
	thanksTo+=QString("<br />");
	thanksTo+=QString("orange");
	thanksTo+=QString("<br />");
	thanksTo+=QString("nguyenhuuduyet");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Vanyo Georgiev");
	thanksTo+=QString("<br />");
	thanksTo+=QString("bharatstank");
	thanksTo+=QString("<br />");
	thanksTo+=QString("alxgudea");
	thanksTo+=QString("<br />");
	thanksTo+=QString("andrealva");
	thanksTo+=QString("<br />");
	thanksTo+=QString("dotosouza");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Bob Hairgrove");
	thanksTo+=QString("<br />");
	thanksTo+=QString("James");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Khalilullah Yosufi");
	thanksTo+=QString("<br />");
	thanksTo+=QString("mercurialuser");
	thanksTo+=QString("<br />");
	thanksTo+=QString("azaer");
	thanksTo+=QString("<br />");
	thanksTo+=QString("chintu");
	thanksTo+=QString("<br />");
	thanksTo+=QString("khalafi");
	thanksTo+=QString("<br />");
	thanksTo+=QString("jillali elghazoui");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Mohamed NAJARI");
	thanksTo+=QString("<br />");
	thanksTo+=QString("youssouf");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Pascal Cohen");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Asier Urio Larrea");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Pavel Fric");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Michel");
	thanksTo+=QString("<br />");
	thanksTo+=QString("MilesM");
	thanksTo+=QString("<br />");
	thanksTo+=QString("adso");
	thanksTo+=QString("<br />");
	thanksTo+=QString("locutusofborg");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Maouhoub");
	thanksTo+=QString("<br />");
	thanksTo+=QString("flauta");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Marco Vassura");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Luigi Valbonesi");
	thanksTo+=QString("<br />");
	thanksTo+=QString("fernandolordao");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Wizard");
	thanksTo+=QString("<br />");
	thanksTo+=QString("ant7");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Lizio");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Omar Ben Ali");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Nguyen Trong Hieu");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Arsenio Stabile");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Vangelis Karafillidis");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Handaya");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Sudharshan K M");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Nataraj Urs H D");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Alexandre R. Soares");
	thanksTo+=QString("<br />");
	thanksTo+=QString("hudrea");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Udo Schütz");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Jijo Jose");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Fernando Poblete");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Benahmed Abdelkrim");
	thanksTo+=QString("<br />");
	thanksTo+=QString("math user");
	thanksTo+=QString("<br />");
	thanksTo+=QString("ChicagoPianoTuner");
	thanksTo+=QString("<br />");
	thanksTo+=QString("MING-KIAN JONATHAN CEDRIC LEE KIM GNOK");
	thanksTo+=QString("<br />");
	thanksTo+=QString("daltinkurt");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Léo-Paul Roch");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Matthias Söllner");
	thanksTo+=QString("<br />");
	thanksTo+=QString("auriolar");
	thanksTo+=QString("<br />");
	thanksTo+=QString("dmcdonald");
	thanksTo+=QString("<br />");
	thanksTo+=QString("wahyuamin");
	thanksTo+=QString("<br />");
	thanksTo+=QString("abautu");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Jan Losinski");
	thanksTo+=QString("<br />");
	thanksTo+=QString("mrtvillaret");
	thanksTo+=QString("<br />");
	thanksTo+=QString("alienglow");
	thanksTo+=QString("<br />");
	thanksTo+=QString("noddy11");
	thanksTo+=QString("<br />");
	thanksTo+=QString("JBoss");
	thanksTo+=QString("<br />");
	thanksTo+=QString("thanhnambkhn");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Malamojka");
	thanksTo+=QString("<br />");
	thanksTo+=QString("canhathuongnhau");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Rodolfo Ribeiro Gomes");
	thanksTo+=QString("<br />");
	thanksTo+=QString("dasa");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Julio González Gil");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Abou");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Matsumoto");
	thanksTo+=QString("<br />");
	thanksTo+=QString("bart.leyen");
	thanksTo+=QString("<br />");
	thanksTo+=QString("math");
	thanksTo+=QString("<br />");
	thanksTo+=QString("s.lanore");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Robinson A. Lemos");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Maurino C. Maria");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Valdo");
	thanksTo+=QString("<br />");
	thanksTo+=QString("sigit_yuwono");
	thanksTo+=QString("<br />");
	thanksTo+=QString("S Chandrasekar");
	thanksTo+=QString("<br />");
	thanksTo+=QString("utismetis");
	thanksTo+=QString("<br />");
	thanksTo+=QString("chernous");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Roberto Bergonzini");
	thanksTo+=QString("<br />");
	thanksTo+=QString("sln_rj");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Emiliano Llano Díaz");
	thanksTo+=QString("<br />");
	thanksTo+=QString("mohammed");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Taro Tada");
	thanksTo+=QString("<br />");
	thanksTo+=QString("V Paul C Charlesraj");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Innocent De Marchi");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Yakup Kadri Demirci");
	thanksTo+=QString("<br />");
	thanksTo+=QString("bachiri401");
	thanksTo+=QString("<br />");
	thanksTo+=QString("francescotorres");
	thanksTo+=QString("<br />");
	thanksTo+=QString("aisse");
	thanksTo+=QString("<br />");
	thanksTo+=QString("svenvanhal");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Coralie");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Diego Froner");
	thanksTo+=QString("<br />");
	thanksTo+=QString("pg788");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Dietmar Deuster");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Ahmed Ben Hassan");
	thanksTo+=QString("<br />");
	thanksTo+=QString("amin");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Anthony Siaudeau");
	thanksTo+=QString("<br />");
	thanksTo+=QString("satellite2");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Jude G");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Madas Pál");
	thanksTo+=QString("<br />");
	thanksTo+=QString("fourat");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Nagendra");
	thanksTo+=QString("<br />");
	thanksTo+=QString("CarolStott");
	thanksTo+=QString("<br />");
	thanksTo+=QString("taxaza");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Christoph Voelker");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Xingxing");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Ireri Venture");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Henrique Belo");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Luca");
	thanksTo+=QString("<br />");
	thanksTo+=QString("totera");
	thanksTo+=QString("<br />");
	thanksTo+=QString("astigol");
	thanksTo+=QString("<br />");
	thanksTo+=QString("dimzev");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Zafar Allah Askar");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Lysso");
	thanksTo+=QString("<br />");
	thanksTo+=QString::fromUtf8("Marco Diego Aurélio Mesquita");
	thanksTo+=QString("<br />");
	thanksTo+=QString("forstera");
	thanksTo+=QString("<br />");
	thanksTo+=QString("otman hashem");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Ahmed El Hawary");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Abdellah Sabri");
	thanksTo+=QString("<br />");
	thanksTo+=QString("khatou1973");
	thanksTo+=QString("<br />");
	thanksTo+=QString("nachrach111");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Eric de Quartel");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Pierre L'Ecuyer");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Francesco Rizzo");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Peter Johnson");
	thanksTo+=QString("<br />");
	thanksTo+=QString("bilal24");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Mohamed Ait Ichou");
	thanksTo+=QString("<br />");
	thanksTo+=QString("yassine bigman");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Eric Carolus");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Bouhadef");
	thanksTo+=QString("<br />");
	thanksTo+=QString("saidmechri");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Med BENNIOU");
	thanksTo+=QString("<br />");
	thanksTo+=QString("mathmake");
	thanksTo+=QString("<br />");
	thanksTo+=QString("mikkojoo");
	thanksTo+=QString("<br />");
	thanksTo+=QString("samantha.goddard");
	thanksTo+=QString("<br />");
	thanksTo+=QString("lakhdar bezzit");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Hiba Hadi");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Marcus");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Celia22");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Fiorellino");
	thanksTo+=QString("<br />");
	thanksTo+=QString("saladv");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Fran");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Nando");
	thanksTo+=QString("<br />");
	thanksTo+=QString("Adele");
	thanksTo+=QString("<br />");

	thanksToTextBrowser->setHtml(thanksTo);
}

HelpAboutForm::~HelpAboutForm()
{
	saveFETDialogGeometry(this);
}
