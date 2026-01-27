/***************************************************************************
                          humanreadabledataform.cpp  -  description
                             -------------------
    begin                : 2026
    copyright            : (C) 2026 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "humanreadabledataform.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "rules.h"

#include "utilities.h"

#include <QString>

#include <QSettings>
#include <QMetaObject>

#include <QMessageBox>

#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>

extern Timetable gt;

extern const QString COMPANY;
extern const QString PROGRAM;

HumanReadableDataForm::HumanReadableDataForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closeButton->setDefault(true);

	QSettings settings(COMPANY, PROGRAM);
	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());

	colorsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/use-colors"), "false").toBool());
	
	fetVersionCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-fet-version"), "true").toBool());
	modeCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-mode"), "true").toBool());
	institutionNameCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-institution-name"), "true").toBool());
	commentsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-comments"), "true").toBool());
	termsModeSettingsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-terms-mode-settings"), "true").toBool());

	daysCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-days"), "true").toBool());
	realDaysCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-real-days"), "true").toBool());
	hoursCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-hours"), "true").toBool());
	realHoursCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-real-hours"), "true").toBool());

	subjectsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-subjects"), "true").toBool());
	activityTagsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-activity-tags"), "true").toBool());
	teachersCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-teachers"), "true").toBool());
	yearsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-years"), "true").toBool());
	groupsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-groups"), "true").toBool());
	subgroupsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-subgroups"), "true").toBool());
	activitiesCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-activities"), "true").toBool());
	buildingsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-buildings"), "true").toBool());
	roomsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-rooms"), "true").toBool());
	
	timeConstraintsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-time-constraints"), "true").toBool());
	spaceConstraintsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-space-constraints"), "true").toBool());
	
	timetableGenerationOptionsCheckBox->setChecked(settings.value(this->metaObject()->className()+QString("/show-timetable-generation-options"), "true").toBool());

	connect(closeButton, &QPushButton::clicked, this, &HumanReadableDataForm::close);

	connect(saveAsPushButton, &QPushButton::clicked, this, &HumanReadableDataForm::saveAs);
	
	connect(colorsCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(fetVersionCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(modeCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(institutionNameCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(commentsCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(termsModeSettingsCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(daysCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(realDaysCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(hoursCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(realHoursCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(subjectsCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(activityTagsCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(teachersCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(yearsCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(groupsCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(subgroupsCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(activitiesCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(buildingsCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(roomsCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(timeConstraintsCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(spaceConstraintsCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	connect(timetableGenerationOptionsCheckBox, &QCheckBox::toggled, this, &HumanReadableDataForm::filterChanged);
	
	filterChanged();
}

HumanReadableDataForm::~HumanReadableDataForm()
{
	saveFETDialogGeometry(this);

	QSettings settings(COMPANY, PROGRAM);
	
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
	
	settings.setValue(this->metaObject()->className()+QString("/use-colors"), colorsCheckBox->isChecked());
	
	settings.setValue(this->metaObject()->className()+QString("/show-fet-version"), fetVersionCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-mode"), modeCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-institution-name"), institutionNameCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-comments"), commentsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-terms-mode-settings"), termsModeSettingsCheckBox->isChecked());

	settings.setValue(this->metaObject()->className()+QString("/show-days"), daysCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-real-days"), realDaysCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-hours"), hoursCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-real-hours"), realHoursCheckBox->isChecked());

	settings.setValue(this->metaObject()->className()+QString("/show-subjects"), subjectsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-activity-tags"), activityTagsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-teachers"), teachersCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-years"), yearsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-groups"), groupsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-subgroups"), subgroupsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-activities"), activitiesCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-buildings"), buildingsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-rooms"), roomsCheckBox->isChecked());
	
	settings.setValue(this->metaObject()->className()+QString("/show-time-constraints"), timeConstraintsCheckBox->isChecked());
	settings.setValue(this->metaObject()->className()+QString("/show-space-constraints"), spaceConstraintsCheckBox->isChecked());
	
	settings.setValue(this->metaObject()->className()+QString("/show-timetable-generation-options"), timetableGenerationOptionsCheckBox->isChecked());
}

void HumanReadableDataForm::filterChanged()
{
	bool colors=colorsCheckBox->isChecked();
	
	bool showFetVersion=fetVersionCheckBox->isChecked();
	bool showMode=modeCheckBox->isChecked();
	bool showInstitutionName=institutionNameCheckBox->isChecked();
	bool showComments=commentsCheckBox->isChecked();
	bool showTermsModeSettings=termsModeSettingsCheckBox->isChecked();

	bool showDaysPerWeek=daysCheckBox->isChecked();
	bool showRealDaysPerWeek=realDaysCheckBox->isChecked();
	bool showHoursPerDay=hoursCheckBox->isChecked();
	bool showRealHoursPerDay=realHoursCheckBox->isChecked();

	bool showSubjects=subjectsCheckBox->isChecked();
	bool showActivityTags=activityTagsCheckBox->isChecked();
	bool showTeachers=teachersCheckBox->isChecked();
	bool showYears=yearsCheckBox->isChecked();
	bool showGroups=groupsCheckBox->isChecked();
	bool showSubgroups=subgroupsCheckBox->isChecked();
	bool showActivities=activitiesCheckBox->isChecked();
	bool showBuildings=buildingsCheckBox->isChecked();
	bool showRooms=roomsCheckBox->isChecked();
	
	bool showTimeConstraints=timeConstraintsCheckBox->isChecked();
	bool showSpaceConstraints=spaceConstraintsCheckBox->isChecked();
	
	bool showTimetableGenerationOptions=timetableGenerationOptionsCheckBox->isChecked();

	bool originalRtl=LANGUAGE_STYLE_RIGHT_TO_LEFT;

	//HTML 5 head
	QString begin="<!DOCTYPE html>\n";
	begin+="<html lang=\""+LANGUAGE_FOR_HTML+"\"";
	if(LANGUAGE_STYLE_RIGHT_TO_LEFT){
		begin+=" dir=\"rtl\"";
		LANGUAGE_STYLE_RIGHT_TO_LEFT=false;
	}
	begin+=">\n";

	begin+="  <head>\n";
	begin+="    <title>"+protect4(QFileInfo(INPUT_FILENAME_XML).completeBaseName())+"</title>\n";
	begin+="    <meta charset=\"utf-8\">\n";
	begin+="  </head>\n";
	begin+="  <body>\n";
	begin+="    <p>\n";

	//HTML 4 head
	/*QString begin="<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	begin+="  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n";

	if(!LANGUAGE_STYLE_RIGHT_TO_LEFT){
		begin+="<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""+LANGUAGE_FOR_HTML+"\" xml:lang=\""+LANGUAGE_FOR_HTML+"\">\n";
	} else {
		begin+="<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\""+LANGUAGE_FOR_HTML+"\" xml:lang=\""+LANGUAGE_FOR_HTML+"\" dir=\"rtl\">\n";
		LANGUAGE_STYLE_RIGHT_TO_LEFT=false; //dirty workaround
	}
	begin+="  <head>\n";
	begin+="    <title>"+protect4(QFileInfo(INPUT_FILENAME_XML).completeBaseName())+"</title>\n";
	begin+="    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n";
	begin+="  </head>\n";
	begin+="  <body>\n";
	begin+="    <p>\n";*/

	QString s1;
	
	if(showFetVersion){
		s1+=tr("FET version: %1").arg(FET_VERSION);
		s1+="\n\n";
	}

	///////

	if(showMode){
		QString ms;
		if(gt.rules.mode==OFFICIAL)
			ms="Official";
		else if(gt.rules.mode==MORNINGS_AFTERNOONS)
			ms="Mornings_Afternoons";
		else if(gt.rules.mode==BLOCK_PLANNING)
			ms="Block_Planning";
		else if(gt.rules.mode==TERMS)
			ms="Terms";
		else
			assert(0);
		
		s1+=tr("Mode: %1").arg(ms);
		s1+="\n\n";
	}

	///////
	
	//the institution name and the comments
	if(showInstitutionName){
		s1+=tr("Institution name: %1").arg(gt.rules.institutionName);
		s1+="\n\n";
	}
	if(showComments){
		s1+=tr("Comments: %1").arg(gt.rules.comments);
		s1+="\n\n";
	}

	///////

	//for terms (Finland)
	if(showTermsModeSettings){
		if(gt.rules.mode==TERMS){
			s1+=tr("Number of terms: %1").arg(gt.rules.nTerms);
			s1+="\n";
			s1+=tr("Number of days per term: %1").arg(gt.rules.nDaysPerTerm);
			s1+="\n\n";
		}
	}

	///////
	
	//the days and the hours
	if(showDaysPerWeek){
		s1+=tr("Number of days per week: %1").arg(gt.rules.nDaysPerWeek);
		s1+="\n\n";
		for(int i=0; i<gt.rules.nDaysPerWeek; i++){
			s1+=tr("Day name: %1").arg(gt.rules.daysOfTheWeek[i]);
			s1+="\n";
			s1+=tr("Day long name: %1").arg(gt.rules.daysOfTheWeek_longNames[i]);
			s1+="\n";
		}
		s1+="\n\n";
	}

	//the real days, for the Mornings-Afternoons mode
	if(showRealDaysPerWeek){
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			s1+=tr("Number of real days per week: %1").arg(gt.rules.nRealDaysPerWeek);
			s1+="\n\n";
			for(int i=0; i<gt.rules.nRealDaysPerWeek; i++){
				s1+=tr("Real day name: %1").arg(gt.rules.realDaysOfTheWeek[i]);
				s1+="\n";
				s1+=tr("Real day long name: %1").arg(gt.rules.realDaysOfTheWeek_longNames[i]);
				s1+="\n";
			}
			s1+="\n\n";
		}
	}
	
	if(showHoursPerDay){
		s1+=tr("Number of hours per day: %1").arg(gt.rules.nHoursPerDay);
		s1+="\n\n";
		for(int i=0; i<gt.rules.nHoursPerDay; i++){
			s1+=tr("Hour name: %1").arg(gt.rules.hoursOfTheDay[i]);
			s1+="\n";
			s1+=tr("Hour long name: %1").arg(gt.rules.hoursOfTheDay_longNames[i]);
			s1+="\n";
		}
		s1+="\n\n";
	}

	//the real hours, for the Mornings-Afternoons mode
	if(showRealHoursPerDay){
		if(gt.rules.mode==MORNINGS_AFTERNOONS){
			s1+=tr("Number of real hours per day: %1").arg(gt.rules.nRealHoursPerDay);
			s1+="\n\n";
			for(int i=0; i<gt.rules.nRealHoursPerDay; i++){
				s1+=tr("Real hour name: %1").arg(gt.rules.realHoursOfTheDay[i]);
				s1+="\n";
				s1+=tr("Real hour long name: %1").arg(gt.rules.realHoursOfTheDay_longNames[i]);
				s1+="\n";
			}
			s1+="\n\n";
		}
	}
	
	///////

	//subjects list - should be before teachers list, because each teacher has a list of associated qualified subjects
	if(showSubjects){
		s1+=tr("Number of subjects: %1").arg(gt.rules.subjectsList.size());
		s1+="\n\n";
		for(int i=0; i<gt.rules.subjectsList.size(); i++){
			Subject* sbj=gt.rules.subjectsList[i];
			s1+=sbj->getDetailedDescription();
			s1+="\n";
		}
		if(!gt.rules.subjectsList.isEmpty())
			s1+="\n\n";
	}

	///////

	//activity tags list
	if(showActivityTags){
		s1+=tr("Number of activity tags: %1").arg(gt.rules.activityTagsList.size());
		s1+="\n\n";
		for(int i=0; i<gt.rules.activityTagsList.size(); i++){
			ActivityTag* stg=gt.rules.activityTagsList[i];
			s1+=stg->getDetailedDescription();
			s1+="\n";
		}
		if(!gt.rules.activityTagsList.isEmpty())
			s1+="\n\n";
	}

	///////

	//teachers list
	if(showTeachers){
		s1+=tr("Number of teachers: %1").arg(gt.rules.teachersList.size());
		s1+="\n\n";
		for(int i=0; i<gt.rules.teachersList.size(); i++){
			Teacher* tch=gt.rules.teachersList[i];
			s1+=tch->getDetailedDescription(gt.rules);
			s1+="\n";
		}
		if(!gt.rules.teachersList.isEmpty())
			s1+="\n\n";
	}

	///////

	//students list
	if(showYears){
		s1+=tr("Number of students years: %1").arg(gt.rules.yearsList.size());
		s1+="\n\n";
	}
	if(showYears || showGroups || showSubgroups){
		for(int i=0; i<gt.rules.yearsList.size(); i++){
			StudentsYear* sty=gt.rules.yearsList[i];
			if(showYears){
				s1+=sty->getDetailedDescription();
				s1+="\n";
			}
			if(showGroups || showSubgroups){
				for(StudentsGroup* stg : std::as_const(sty->groupsList)){
					if(showGroups){
						s1+=stg->getDetailedDescription();
						s1+="\n";
					}
					if(showSubgroups){
						for(StudentsSubgroup* sts : std::as_const(stg->subgroupsList)){
							s1+=sts->getDetailedDescription();
							s1+="\n";
						}
					}
				}
			}
		}
		if(showYears)
			if(!gt.rules.yearsList.isEmpty())
				s1+="\n\n";
	}

	///////

	//activities list
	if(showActivities){
		s1+=tr("Number of activities: %1").arg(gt.rules.activitiesList.size());
		s1+="\n\n";
		for(int i=0; i<gt.rules.activitiesList.size(); i++){
			Activity* act=gt.rules.activitiesList[i];
			s1+=act->getDetailedDescription(gt.rules);
			s1+="\n";
		}
		if(!gt.rules.activitiesList.isEmpty())
			s1+="\n\n";
	}

	///////

	//buildings list
	if(showBuildings){
		s1+=tr("Number of buildings: %1").arg(gt.rules.buildingsList.size());
		s1+="\n\n";
		for(int i=0; i<gt.rules.buildingsList.size(); i++){
			Building* bu=gt.rules.buildingsList[i];
			s1+=bu->getDetailedDescription();
			s1+="\n";
		}
		if(!gt.rules.buildingsList.isEmpty())
			s1+="\n\n";
	}

	///////

	//rooms list
	if(showRooms){
		s1+=tr("Number of rooms: %1").arg(gt.rules.roomsList.size());
		s1+="\n\n";
		for(int i=0; i<gt.rules.roomsList.size(); i++){
			Room* rm=gt.rules.roomsList[i];
			s1+=rm->getDetailedDescription();
			s1+="\n";
		}
		if(!gt.rules.roomsList.isEmpty())
			s1+="\n\n";
	}

	///////

	//time constraints list
	QString s2;
	if(showTimeConstraints){
		s2+=protect4(tr("Number of time constraints: %1").arg(gt.rules.timeConstraintsList.size()));
		s2+="<br />\n<br />\n";
		for(int i=0; i<gt.rules.timeConstraintsList.size(); i++){
			TimeConstraint* ctr=gt.rules.timeConstraintsList[i];
			s2+=ctr->getDetailedDescription(gt.rules, true, colors);
			s2+="<br />\n";
		}
		if(!gt.rules.timeConstraintsList.isEmpty())
			s2+="<br />\n<br />\n";
	}

	//space constraints list
	if(showSpaceConstraints){
		s2+=protect4(tr("Number of space constraints: %1").arg(gt.rules.spaceConstraintsList.size()));
		s2+="<br />\n<br />\n";
		for(int i=0; i<gt.rules.spaceConstraintsList.size(); i++){
			SpaceConstraint* ctr=gt.rules.spaceConstraintsList[i];
			s2+=ctr->getDetailedDescription(gt.rules, true, colors);
			s2+="<br />\n";
		}
		if(!gt.rules.spaceConstraintsList.isEmpty())
			s2+="<br />\n<br />\n";
	}

	QString s3;
	if(showTimetableGenerationOptions){
		if(true || gt.rules.groupActivitiesInInitialOrderList.count()>0){
			s3+=tr("Number of timetable generation options: %1").arg(gt.rules.groupActivitiesInInitialOrderList.count());
			s3+="\n";
			for(int i=0; i<gt.rules.groupActivitiesInInitialOrderList.count(); i++){
				GroupActivitiesInInitialOrderItem* item=gt.rules.groupActivitiesInInitialOrderList[i];
				s3+=item->getDetailedDescription(gt.rules);
			}
			if(!gt.rules.groupActivitiesInInitialOrderList.isEmpty())
				s3+="\n\n";
		}
	}
	
	QString end;
	end+="    </p>\n";
	end+="  </body>\n";
	end+="</html>\n";

	LANGUAGE_STYLE_RIGHT_TO_LEFT=originalRtl;
	
	QString finalString=begin+protect4(s1)+s2+protect4(s3)+end;
	
	textEdit->setText(finalString);
	
	htmlString=finalString;
}

void HumanReadableDataForm::saveAs()
{
	QString predefinedName=OUTPUT_DIR+FILE_SEP+QFileInfo(INPUT_FILENAME_XML).completeBaseName()+QString(".html");

	QString fileName = QFileDialog::getSaveFileName(this, tr("Choose a filename to save the HTML data"),
	 predefinedName,
	 tr("HTML files", "This field means HTML files in the text format")+" (*.html)"+";;"+tr("All files")+" (*)",
	 nullptr, QFileDialog::DontConfirmOverwrite);
	if(fileName.isEmpty())
		return;

	if(QFile::exists(fileName))
		if(QMessageBox::warning( this, tr("FET"),
		 tr("File %1 exists - are you sure you want to overwrite it?").arg(fileName),
		 QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
			return;

	QFile file(fileName);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if(!file.open(QIODeviceBase::WriteOnly)){
#else
	if(!file.open(QIODevice::WriteOnly)){
#endif
		QMessageBox::critical(this, tr("FET critical"),
		 tr("Cannot open file %1 for writing. Please check your disk's free space. Saving of %1 aborted.").arg(fileName));
		return;
	}
	QTextStream tos(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	tos.setEncoding(QStringConverter::Utf8);
#else
	tos.setCodec("UTF-8");
#endif
	tos.setGenerateByteOrderMark(true);
	
	tos<<htmlString;
	
	file.close();
}
