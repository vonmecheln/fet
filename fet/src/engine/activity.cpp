/*
File activity.cpp 
*/

/*
Copyright 2002, 2003 Lalescu Liviu.

This file is part of FET.

FET is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

FET is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with FET; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "timetable_defs.h"
#include "activity.h"
#include "rules.h"

#include <qmessagebox.h>

#include <iostream>

using namespace std;

Activity::Activity()
{
}

Activity::Activity(
	Rules& r,
	int _id,
	int _activityGroupId,
	const QStringList& _teachersNames,
	const QString& _subjectName,
	const QString& _activityTagName,
	const QStringList& _studentsNames,
	int _duration,
	int _totalDuration,
	bool _active,
	bool _computeNTotalStudents,
	int _nTotalStudents)
{
	this->id=_id;
	this->activityGroupId=_activityGroupId;
	this->teachersNames = _teachersNames;
	this->subjectName = _subjectName;
	this->activityTagName = _activityTagName;
	this->studentsNames = _studentsNames;
	this->duration=_duration;
	this->totalDuration=_totalDuration;
	//this->parity=_parity;
	this->active=_active;
	this->computeNTotalStudents=_computeNTotalStudents;
	
	if(_computeNTotalStudents==true){	
		this->nTotalStudents=0;
		for(QStringList::Iterator it=this->studentsNames.begin(); it!=this->studentsNames.end(); it++){
			StudentsSet* ss=r.searchStudentsSet(*it);
			this->nTotalStudents += ss->numberOfStudents;
		}
	}
	else{
		assert(_nTotalStudents>=0);
		this->nTotalStudents=_nTotalStudents;
	}
}

bool Activity::operator==(Activity& a)
{
	if(this->teachersNames != a.teachersNames)
		return false;
	if(this->subjectName != a.subjectName)
		return false;
	if(this->activityTagName != a.activityTagName)
		return false;
	if(this->studentsNames != a.studentsNames)
		return false;
	if(this->duration != a.duration)
	    return false;
	return true;
}

bool Activity::searchTeacher(const QString& teacherName)
{
	return this->teachersNames.find(teacherName)!=this->teachersNames.end();
}

bool Activity::removeTeacher(const QString& teacherName)
{
	int t=this->teachersNames.removeAll(teacherName);
	
	return t>0;
}

void Activity::renameTeacher(const QString& initialTeacherName, const QString& finalTeacherName)
{
	int t=0;
	for(QStringList::iterator it=this->teachersNames.begin(); it!=this->teachersNames.end(); it++)
		if((*it)==initialTeacherName){
			*it=finalTeacherName;
			t++;
		}
	assert(t<=1);
}

bool Activity::searchStudents(const QString& studentsName)
{
	return this->studentsNames.find(studentsName)!=this->studentsNames.end();
}

bool Activity::removeStudents(Rules& r, const QString& studentsName, int nStudents)
{
	if(&r==NULL){	
	}
	
	int t=this->studentsNames.removeAll(studentsName);

	if(t>0 && this->computeNTotalStudents==true){
		/*StudentsSet* s=r.searchStudentsSet(studentsName);
		assert(s!=NULL);
		this->nTotalStudents-=s->numberOfStudents;*/
		this->nTotalStudents-=nStudents;
		assert(this->nTotalStudents>=0);
	}
	
	return t>0;
}

void Activity::renameStudents(Rules& r, const QString& initialStudentsName, const QString& finalStudentsName)
{
	if(&r==NULL){	
	}

	int t=0;
	for(QStringList::iterator it=this->studentsNames.begin(); it!=this->studentsNames.end(); it++)
		if((*it)==initialStudentsName){
			/*if(this->computeNTotalStudents==true){
				StudentsSet* s=r.searchStudentsSet(initialStudentsName);
				assert(s!=NULL);
				this->nTotalStudents-=s->numberOfStudents;
				
				StudentsSet* s2=r.searchStudentsSet(finalStudentsName);
				assert(s2!=NULL);
				this->nTotalStudents+=s2->numberOfStudents;
				
				assert(this->nTotalStudents>=0);
			}*/
		
			*it=finalStudentsName;
			t++;
		}
	assert(t<=1);
}

void Activity::computeInternalStructure(Rules& r)
{
	//the internal subgroups list must be computed before entering here.

	//teachers
	//this->nTeachers=0;
	this->iTeachersList.clear();
	for(QStringList::Iterator it=this->teachersNames.begin(); it!=this->teachersNames.end(); it++){
		int tmp;
		for(tmp=0; tmp<r.nInternalTeachers; tmp++){
			if(r.internalTeachersList[tmp]->name == (*it))
				break;
		}
		assert(tmp < r.nInternalTeachers);
		//assert(this->nTeachers<MAX_TEACHERS_PER_ACTIVITY);
		//this->teachers[this->nTeachers++]=tmp;
		this->iTeachersList.append(tmp);
	}

	//subjects
	this->subjectIndex = r.searchSubject(this->subjectName);
	assert(this->subjectIndex>=0);

	//activity tags
	this->activityTagIndex = r.searchActivityTag(this->activityTagName);

	//students	
	//this->nSubgroups=0;
	this->iSubgroupsList.clear();
	for(QStringList::Iterator it=this->studentsNames.begin(); it!=this->studentsNames.end(); it++){
		StudentsSet* ss=r.searchAugmentedStudentsSet(*it);
		assert(ss);
		if(ss->type==STUDENTS_SUBGROUP){
			int tmp;
			/*for(tmp=0; tmp<r.nInternalSubgroups; tmp++)
				if(r.internalSubgroupsList[tmp]->name == ss->name)
					break;*/
			tmp=((StudentsSubgroup*)ss)->indexInInternalSubgroupsList;
			assert(tmp>=0);
			assert(tmp<r.nInternalSubgroups);
			//assert(this->nSubgroups<MAX_SUBGROUPS_PER_ACTIVITY);
			
			bool duplicate=false;
			if(this->iSubgroupsList.contains(tmp))
			//for(int j=0; j<this->nSubgroups; j++)
			//	if(this->subgroups[j]==tmp)
					duplicate=true;
			if(duplicate){
				QString s;
				s=QObject::tr(QString("Warning: activity with id=%1\ncontains duplicated subgroups. Automatically correcting..."))
					.arg(this->id);
				//QMessageBox::warning(NULL, QObject::tr("FET information"), s, QObject::tr("&Ok"));
				cout<<qPrintable(s)<<endl;
			}
			else
				this->iSubgroupsList.append(tmp);
				//this->subgroups[this->nSubgroups++]=tmp;
		}
		else if(ss->type==STUDENTS_GROUP){
			StudentsGroup* stg=(StudentsGroup*)ss;
			for(int k=0; k<stg->subgroupsList.size(); k++){
				StudentsSubgroup* sts=stg->subgroupsList[k];
				int tmp;
				/*for(tmp=0; tmp<r.nInternalSubgroups; tmp++)
					if(r.internalSubgroupsList[tmp]->name == sts->name)
						break;*/
				tmp=sts->indexInInternalSubgroupsList;
				assert(tmp>=0);
				assert(tmp<r.nInternalSubgroups);
				//assert(this->nSubgroups<MAX_SUBGROUPS_PER_ACTIVITY);

				bool duplicate=false;
				if(this->iSubgroupsList.contains(tmp))
				//for(int j=0; j<this->nSubgroups; j++)
				//	if(this->subgroups[j]==tmp)
						duplicate=true;
				if(duplicate){
					QString s;
					s=QObject::tr(QString("Warning: activity with id=%1\ncontains duplicated subgroups. Automatically correcting..."))
						.arg(this->id);
					//QMessageBox::warning(NULL, QObject::tr("FET information"), s, QObject::tr("&Ok"));
					cout<<qPrintable(s)<<endl;
				}
				else
					//this->subgroups[this->nSubgroups++]=tmp;
					this->iSubgroupsList.append(tmp);
			}
		}
		else if(ss->type==STUDENTS_YEAR){
			StudentsYear* sty=(StudentsYear*)ss;
			for(int k=0; k<sty->groupsList.size(); k++){
				StudentsGroup* stg=sty->groupsList[k];
				for(int l=0; l<stg->subgroupsList.size(); l++){
					StudentsSubgroup* sts=stg->subgroupsList[l];
					int tmp;
					/*for(tmp=0; tmp<r.nInternalSubgroups; tmp++)
						if(r.internalSubgroupsList[tmp]->name == sts->name)
							break;*/
					tmp=sts->indexInInternalSubgroupsList;
					assert(tmp>=0);
					assert(tmp<r.nInternalSubgroups);
					//assert(this->nSubgroups<MAX_SUBGROUPS_PER_ACTIVITY);

					bool duplicate=false;
					if(this->iSubgroupsList.contains(tmp))
					//for(int j=0; j<this->nSubgroups; j++)
					//	if(this->subgroups[j]==tmp)
							duplicate=true;
					if(duplicate){
						QString s;
						s=QObject::tr(QString("Warning: activity with id=%1\ncontains duplicated subgroups. Automatically correcting..."))
							.arg(this->id);
						//QMessageBox::warning(NULL, QObject::tr("FET information"), s, QObject::tr("&Ok"));
						cout<<qPrintable(s)<<endl;
					}
					else{
						//this->subgroups[this->nSubgroups++]=tmp;
						this->iSubgroupsList.append(tmp);
					}
				}
			}
		}
		else
			assert(0);
	}
}

QString Activity::getXmlDescription(Rules& r)
{
	if(&r==NULL){
	}

	QString s="<Activity>\n";
	for(QStringList::Iterator it=this->teachersNames.begin(); it!=this->teachersNames.end(); it++)
		s+="	<Teacher>" + protect(*it) + "</Teacher>\n";
	s+="	<Subject>"+protect(this->subjectName)+"</Subject>\n";
	s+="	<Activity_Tag>"+protect(this->activityTagName)+"</Activity_Tag>\n";

	s+="	<Duration>"+QString::number(this->duration)+"</Duration>\n";
	s+="	<Total_Duration>"+QString::number(this->totalDuration)+"</Total_Duration>\n";
	s+="	<Id>"+QString::number(this->id)+"</Id>\n";
	s+="	<Activity_Group_Id>"+QString::number(this->activityGroupId)+"</Activity_Group_Id>\n";
	/*if(this->parity==PARITY_WEEKLY)
		s+="	<Weekly></Weekly>\n";
	else{
		assert(this->parity==PARITY_FORTNIGHTLY);
		s+="	<Fortnightly></Fortnightly>\n";
	}*/
	if(this->active==true)
		s+="	<Active>yes</Active>\n";
	else
		s+="	<Active>no</Active>\n";
	for(QStringList::Iterator it=this->studentsNames.begin(); it!=this->studentsNames.end(); it++)
		s+="	<Students>" + protect(*it) + "</Students>\n";

	if(this->computeNTotalStudents==false)
		s+="	<Number_Of_Students>"+QString::number(this->nTotalStudents)+"</Number_Of_Students>\n";

	s+="</Activity>";

	return s;
}

QString Activity::getDescription(Rules& r)
{
	Q_UNUSED(r);
	
	bool _indent=false;
	if(this->isSplit() && this->id!=this->activityGroupId)
		_indent=true;
		
	QString _teachers="";
	if(teachersNames.count()==0)
		_teachers=QObject::tr("no teachers");
	else{
		bool begin=true;
		for(QStringList::Iterator it=this->teachersNames.begin(); it!=this->teachersNames.end(); it++){
			if(!begin)
				_teachers+=",";
				
			_teachers += *it;
			begin=false;
		}
	}

	QString _subject=this->subjectName;
	
	QString _activityTag=this->activityTagName;

	QString _students="";
	if(studentsNames.count()==0)
		_students=QObject::tr("no students");
	else{
		bool begin=true;
		for(QStringList::Iterator it=this->studentsNames.begin(); it!=this->studentsNames.end(); it++){
			if(!begin)
				_students+=",";
				
			_students += *it;
			begin=false;
		}
	}

	QString _id;
	_id = QString::number(id);

	QString _agid="";
	if(this->isSplit())
		_agid = QString::number(this->activityGroupId);

	QString _duration=QString::number(this->duration);
	
	QString _totalDuration="";
	if(this->isSplit())
		_totalDuration = QString::number(this->totalDuration);

	QString _active;
	if(this->active==true)
		_active=QObject::tr("yes");
	else
		_active=QObject::tr("no");

	QString _nstudents="";		
	if(this->computeNTotalStudents==false)
		_nstudents=QString::number(this->nTotalStudents);

	/////////
	QString s="";
	if(_indent)
		s+="   ";

	if(_activityTag!="" && this->isSplit() && _nstudents!=""){
		s+=QObject::tr("Act: T:%1, S:%2, AT:%3, St:%4, Id:%5, AGId:%6, D:%7, TD:%8, A:%9, NSt:%10",
		 "Comment for translators (do not translate this comment):\n"
		 "This is one of the 8 variants for description of an activity, depending on activity (if it has activity tag, etc.)"
		 "Please be careful and respect arrangement.\n"
		 "Activity: Teacher, Subject, Activity Tag, Students, Id, Activity Group Id, Duration, Total Duration, Active (yes/no), Number of Students")
		 .arg(_teachers).arg(_subject).arg(_activityTag).arg(_students)
		 .arg(_id).arg(_agid).arg(_duration).arg(_totalDuration).arg(_active).arg(_nstudents);
	}
	else if(_activityTag=="" && this->isSplit() && _nstudents!=""){
		s+=QObject::tr("Act: T:%1, S:%2, St:%3, Id:%4, AGId:%5, D:%6, TD:%7, A:%8, NSt:%9",
		 "Comment for translators (do not translate this comment):\n"
		 "This is one of the 8 variants for description of an activity, depending on activity (if it has activity tag, etc.)"
		 "Please be careful and respect arrangement.\n"
		 "Activity: Teacher, Subject, Students, Id, Activity Group Id, Duration, Total Duration, Active (yes/no), Number of Students")
		 .arg(_teachers).arg(_subject).arg(_students)
		 .arg(_id).arg(_agid).arg(_duration).arg(_totalDuration).arg(_active).arg(_nstudents);
	}
	else if(_activityTag!="" && !this->isSplit() && _nstudents!=""){
		s+=QObject::tr("Act: T:%1, S:%2, AT:%3, St:%4, Id:%5, D:%6, A:%7, NSt:%8",
		 "Comment for translators (do not translate this comment):\n"
		 "This is one of the 8 variants for description of an activity, depending on activity (if it has activity tag, etc.)"
		 "Please be careful and respect arrangement.\n"
		 "Activity: Teacher, Subject, Activity Tag, Students, Id, Duration, Active (yes/no), Number of Students")
		 .arg(_teachers).arg(_subject).arg(_activityTag).arg(_students)
		 .arg(_id).arg(_duration).arg(_active).arg(_nstudents);
	}
	else if(_activityTag=="" && !this->isSplit() && _nstudents!=""){
		s+=QObject::tr("Act: T:%1, S:%2, St:%3, Id:%4, D:%5, A:%6, NSt:%7",
		 "Comment for translators (do not translate this comment):\n"
		 "This is one of the 8 variants for description of an activity, depending on activity (if it has activity tag, etc.)"
		 "Please be careful and respect arrangement.\n"
		 "Activity: Teacher, Subject, Students, Id, Duration, Active (yes/no), Number of Students")
		 .arg(_teachers).arg(_subject).arg(_students)
		 .arg(_id).arg(_duration).arg(_active).arg(_nstudents);
	}
	else if(_activityTag!="" && this->isSplit() && _nstudents==""){
		s+=QObject::tr("Act: T:%1, S:%2, AT:%3, St:%4, Id:%5, AGId:%6, D:%7, TD:%8, A:%9",
		 "Comment for translators (do not translate this comment):\n"
		 "This is one of the 8 variants for description of an activity, depending on activity (if it has activity tag, etc.)"
		 "Please be careful and respect arrangement.\n"
		 "Activity: Teacher, Subject, Activity Tag, Students, Id, Activity Group Id, Duration, Total Duration, Active (yes/no)")
		 .arg(_teachers).arg(_subject).arg(_activityTag).arg(_students)
		 .arg(_id).arg(_agid).arg(_duration).arg(_totalDuration).arg(_active);
	}
	else if(_activityTag=="" && this->isSplit() && _nstudents==""){
		s+=QObject::tr("Act: T:%1, S:%2, St:%3, Id:%4, AGId:%5, D:%6, TD:%7, A:%8",
		 "Comment for translators (do not translate this comment):\n"
		 "This is one of the 8 variants for description of an activity, depending on activity (if it has activity tag, etc.)"
		 "Please be careful and respect arrangement.\n"
		 "Activity: Teacher, Subject, Students, Id, Activity Group Id, Duration, Total Duration, Active (yes/no)")
		 .arg(_teachers).arg(_subject).arg(_students)
		 .arg(_id).arg(_agid).arg(_duration).arg(_totalDuration).arg(_active);
	}
	else if(_activityTag!="" && !this->isSplit() && _nstudents==""){
		s+=QObject::tr("Act: T:%1, S:%2, AT:%3, St:%4, Id:%5, D:%6, A:%7",
		 "Comment for translators (do not translate this comment):\n"
		 "This is one of the 8 variants for description of an activity, depending on activity (if it has activity tag, etc.)"
		 "Please be careful and respect arrangement.\n"
		 "Activity: Teacher, Subject, Activity Tag, Students, Id, Duration, Active (yes/no)")
		 .arg(_teachers).arg(_subject).arg(_activityTag).arg(_students)
		 .arg(_id).arg(_duration).arg(_active);
	}
	else if(_activityTag=="" && !this->isSplit() && _nstudents==""){
		s+=QObject::tr("Act: T:%1, S:%2, St:%3, Id:%4, D:%5, A:%6",
		 "Comment for translators (do not translate this comment):\n"
		 "This is one of the 8 variants for description of an activity, depending on activity (if it has activity tag, etc.)"
		 "Please be careful and respect arrangement.\n"
		 "Activity: Teacher, Subject, Students, Id, Duration, Active (yes/no)")
		 .arg(_teachers).arg(_subject).arg(_students)
		 .arg(_id).arg(_duration).arg(_active);
	}	
	else
		assert(0);

	//if there is left alignment, I do a double padding with spaces in RTL languages, just to make sure
	if(_indent && LANGUAGE_STYLE_RIGHT_TO_LEFT==true)
		s+="   ";


	/*QString s="";
	if(!this->isSplit()){
		s=QObject::tr("Activity:");
		s+=" ";
	}
	else{
		s="";
		if(LANGUAGE_STYLE_RIGHT_TO_LEFT==false){
			if(this->id==this->activityGroupId)
				s="";
			else
				s="   ";
		}
		s+=QObject::tr("Activity:");
		s+=" ";
	}
	s+=QObject::tr("T:", "Initial letter from Teacher");
	if(teachersNames.count()==0){
		s+=" ";
		s+=QObject::tr("no teachers,");
		s+=", ";
	}
	else
		for(QStringList::Iterator it=this->teachersNames.begin(); it!=this->teachersNames.end(); it++)
			s += *it + ", ";
	s+=QObject::tr("S:", "Initial letter from Subject") + this->subjectName + ", ";
	if(this->subjectTagName!="")
		s+=QObject::tr("AT:", "Initial letters from Activity Tag") + this->activityTagName + ", ";
	s+=QObject::tr("St:", "Initial letter from Students");
	if(studentsNames.count()==0){
		s+=" ";
		s+=QObject::tr("no students,");
		s+=", ";
	}
	else
		for(QStringList::Iterator it=this->studentsNames.begin(); it!=this->studentsNames.end(); it++)
			s += *it + ", ";

	s += QObject::tr("Id:") + QString::number(id) + ", ";
	if(this->isSplit())
		s += QObject::tr("AGId:", "Initial letters from Activity Group Id") + QString::number(this->activityGroupId) + ", ";

	s+=QObject::tr("D:", "Initial letter from Duration") + QString::number(this->duration) + ", ";
	if(this->isSplit())
		s += QObject::tr("TD:", "Initial letters from Total Duration") + QString::number(this->totalDuration) + ", ";

	if(this->active==true)
		s+=QObject::tr("A: yes", "Initial letter from Active");
	else
		s+=QObject::tr("A: no", "Initial letter from Active");
		
	if(this->computeNTotalStudents==false){
		s+=", ";
		s+=QObject::tr("NSt:", "Initial letters from Number of Students")+QString::number(this->nTotalStudents);
	}

	if(this->isSplit() && LANGUAGE_STYLE_RIGHT_TO_LEFT==true){
		if(this->id==this->activityGroupId)
			s+="";
		else
			s+="   ";
	}*/

	return s;
}

QString Activity::getDetailedDescription(Rules &r)
{
	QString s;
	if(!this->isSplit()){
		s=QObject::tr("Activity:");
		s+="\n";
	}
	else{
		s="";
		/*if(this->id==this->activityGroupId)
			s="";
		else
			s="   ";*/
		s+=QObject::tr("Activity:");
		s+="\n";
		//s+=QObject::tr("Component of a split activity\n");
		//s=QObject::tr("Sub-activity:\n");
	}
	if(teachersNames.count()==0){
		s+=QObject::tr("No teachers for this activity");
		s+="\n";
	}
	else
		for(QStringList::Iterator it=this->teachersNames.begin(); it!=this->teachersNames.end(); it++){
			s+=QObject::tr("Teacher=%1").arg(*it);
			s+="\n";
		}
	s+=QObject::tr("Subject=%1").arg(this->subjectName);
	s+="\n";
	if(this->activityTagName!=""){
		s+=QObject::tr("Activity tag=%1").arg(this->activityTagName);
		s+="\n";
	}
	if(studentsNames.count()==0){
		s+=QObject::tr("No students sets for this activity");
		s+="\n";
	}
	else
		for(QStringList::Iterator it=this->studentsNames.begin(); it!=this->studentsNames.end(); it++){
			s += QObject::tr("Students=%1").arg(*it);
			s+="\n";
		}

	s += QObject::tr("Id=%1").arg(QString::number(id));
	s+="\n";
	if(this->isSplit()){
		s += QObject::tr("Activity group id=%1").arg(QString::number(this->activityGroupId));
		s+="\n";
	}

	s+=QObject::tr("Duration=%1").arg(QString::number(this->duration));
	s+="\n";
	if(this->isSplit()){
		s += QObject::tr("Total duration=%1").arg(QString::number(this->totalDuration));
		s+="\n";
	}

	if(this->active==true){
		s+=QObject::tr("Active: yes");
		s+="\n";
	}
	else{
		s+=QObject::tr("Active: no");
		s+="\n";
	}
		
	if(this->computeNTotalStudents==true){
		int nStud=0;
		for(QStringList::Iterator it=this->studentsNames.begin(); it!=this->studentsNames.end(); it++){
			StudentsSet* ss=r.searchStudentsSet(*it);
			nStud += ss->numberOfStudents;
		}
		s+=QObject::tr("Total number of students=%1").arg(nStud);
		s+="\n";
	}
	else{
		s+=QObject::tr("Total number of students=%1").arg(this->nTotalStudents);
		s+="\n";
	}

	return s;
}

QString Activity::getDetailedDescriptionWithConstraints(Rules &r)
{
	QString s=this->getDetailedDescription(r);

	s+="--------------------------------------------------\n";
	s+=QObject::tr("Time constraints directly related to this activity:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToActivity(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=QObject::tr("Space constraints directly related to this activity:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToActivity(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}

bool Activity::isSplit()
{
	return this->totalDuration != this->duration;
}
