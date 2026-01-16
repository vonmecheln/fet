/*
File activity.cpp
*/

/***************************************************************************
                          activity.cpp  -  description
                             -------------------
    begin                : 2002
    copyright            : (C) 2002 by Liviu Lalescu
    email                : Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find there the email address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, version 3 of the License.  *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "activity.h"
#include "rules.h"

#include <QSet>

#include <QDataStream>

QDataStream& operator<<(QDataStream& stream, const Activity& act)
{
	stream<<act.comments;
	stream<<act.teachersNames;
	stream<<act.subjectName;
	stream<<act.activityTagsNames;
	stream<<act.studentsNames;
	stream<<act.duration;
	stream<<act.totalDuration;
	stream<<act.id;
	stream<<act.activityGroupId;
	stream<<act.nTotalStudents;
	stream<<act.computeNTotalStudents;
	stream<<act.active;

	return stream;
}

QDataStream& operator>>(QDataStream& stream, Activity& act)
{
	stream>>act.comments;
	stream>>act.teachersNames;
	stream>>act.subjectName;
	stream>>act.activityTagsNames;
	stream>>act.studentsNames;
	stream>>act.duration;
	stream>>act.totalDuration;
	stream>>act.id;
	stream>>act.activityGroupId;
	stream>>act.nTotalStudents;
	stream>>act.computeNTotalStudents;
	stream>>act.active;

	return stream;
}

QDataStream& operator<<(QDataStream& stream, const GroupActivitiesInInitialOrderItem& gaio)
{
	stream<<gaio.active;
	stream<<gaio.comments;
	stream<<gaio.ids;

	return stream;
}

QDataStream& operator>>(QDataStream& stream, GroupActivitiesInInitialOrderItem& gaio)
{
	stream>>gaio.active;
	stream>>gaio.comments;
	stream>>gaio.ids;

	return stream;
}

QString getActivityDetailedDescription(Rules& r, int id); //Implemented in timeconstraint.cpp

GroupActivitiesInInitialOrderItem::GroupActivitiesInInitialOrderItem()
{
	active=true;
	comments=QString("");
}

GroupActivitiesInInitialOrderItem::~GroupActivitiesInInitialOrderItem()
{
}

void GroupActivitiesInInitialOrderItem::removeUseless(Rules& r)
{
	QList<int> tmpList;
	
	for(int id : std::as_const(ids)){
		Activity* act=r.activitiesPointerHash.value(id, nullptr);
		if(act!=nullptr)
			tmpList.append(id);
	}
	
	ids=tmpList;
	
	r.internalStructureComputed=false;
}

void GroupActivitiesInInitialOrderItem::recomputeActivitiesSet(){
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
	idsSet=QSet<int>(ids.constBegin(), ids.constEnd());
#else
	idsSet=ids.toSet();
#endif
}

QString GroupActivitiesInInitialOrderItem::getXmlDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s;

	s+=IL2+"<GroupActivitiesInInitialOrder>\n";
	s+=IL3+"<Number_of_Activities>"+QString::number(ids.count())+"</Number_of_Activities>\n";
	for(int id : std::as_const(ids))
		s+=IL3+QString("<Activity_Id>")+CustomFETString::number(id)+QString("</Activity_Id>\n");

	s+=IL3+"<Active>";
	if(this->active==true)
		s+="true";
	else
		s+="false";
	s+="</Active>\n";

	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";

	s+=IL2+"</GroupActivitiesInInitialOrder>\n";
	
	return s;
}

QString GroupActivitiesInInitialOrderItem::getDescription(Rules& r)
{
	Q_UNUSED(r);

	QString begin=QString("");
	if(!active)
		begin="X - ";

	QString s=tr("Group activities in the initial order item");
	s+=translatedCommaSpace();
	s+=tr("NA:%1", "Number of activities").arg(ids.count());
	for(int id : std::as_const(ids))
		s+=translatedCommaSpace()+tr("Id:%1", "Id of activity").arg(id);

	QString end=QString("");
	if(!comments.isEmpty())
		end=translatedCommaSpace()+tr("C: %1", "Comments").arg(comments);

	return begin+s+end;
}

QString GroupActivitiesInInitialOrderItem::getDetailedDescription(Rules& r)
{
	QString s=tr("Timetable generation option"); s+=QString("\n");
	s+=tr("Group activities in the initial order item"); s+=QString("\n");
	s+=tr("Number of activities=%1").arg(ids.count()); s+=QString("\n");
	for(int id : std::as_const(ids)){
		s+=tr("Activity with id=%1 (%2)", "%1 is the id, %2 is the detailed description of the activity")
		 .arg(id)
		 .arg(getActivityDetailedDescription(r, id));
		s+=QString("\n");
	}

	//Not active?
	QString activeYesNo;
	if(this->active==true)
		activeYesNo=tr("yes");
	else
		activeYesNo=tr("no");
	if(!active){
		s+=tr("Active group activities in the initial order item=%1", "Represents a yes/no value, if a 'group activities in initial order' item is active or not, %1 is yes or no").arg(activeYesNo);
		s+="\n";
	}

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}
	
	return s;
}

Activity::Activity()
{
	comments=QString("");
}

/*Activity::Activity(
	Rules& r,
	int _id,
	int _activityGroupId,
	const QStringList& _teachersNames,
	const QString& _subjectName,
	const QStringList& _activityTagsNames,
	const QStringList& _studentsNames,
	int _duration,
	int _totalDuration,
	bool _active,
	bool _computeNTotalStudents,
	int _nTotalStudents)
{
	comments=QString("");

	this->id=_id;
	this->activityGroupId=_activityGroupId;
	this->teachersNames = _teachersNames;
	this->subjectName = _subjectName;
	this->activityTagsNames = _activityTagsNames;
	this->studentsNames = _studentsNames;
	this->duration=_duration;
	this->totalDuration=_totalDuration;
	this->active=_active;
	this->computeNTotalStudents=_computeNTotalStudents;
	
	if(_computeNTotalStudents==true){
		this->nTotalStudents=0;
		for(QStringList::Iterator it=this->studentsNames.begin(); it!=this->studentsNames.end(); it++){
			StudentsSet* ss=r.searchStudentsSet(*it);
			assert(ss!=nullptr);
			this->nTotalStudents += ss->numberOfStudents;
		}
	}
	else{
		assert(_nTotalStudents>=0);
		this->nTotalStudents=_nTotalStudents;
	}
}*/

Activity::Activity(
	Rules& r,
	int _id,
	int _activityGroupId,
	const QStringList& _teachersNames,
	const QString& _subjectName,
	const QStringList& _activityTagsNames,
	const QStringList& _studentsNames,
	int _duration,
	int _totalDuration,
	bool _active,
	bool _computeNTotalStudents,
	int _nTotalStudents,
	int _computedNumberOfStudents)
{
	Q_UNUSED(r);
	//Q_UNUSED(_nTotalStudents);

	comments=QString("");

	this->id=_id;
	this->activityGroupId=_activityGroupId;
	this->teachersNames = _teachersNames;
	this->subjectName = _subjectName;
	this->activityTagsNames = _activityTagsNames;
	this->studentsNames = _studentsNames;
	this->duration=_duration;
	this->totalDuration=_totalDuration;
	this->active=_active;
	this->computeNTotalStudents=_computeNTotalStudents;
	
	//assert(_computeNTotalStudents);
	if(_computeNTotalStudents)
		assert(_nTotalStudents==-1);
	else
		assert(_nTotalStudents==_computedNumberOfStudents);
	this->nTotalStudents=_computedNumberOfStudents;
}

bool Activity::operator==(const Activity& a)
{
	if(this->teachersNames != a.teachersNames)
		return false;
	if(this->subjectName != a.subjectName)
		return false;
	if(this->activityTagsNames != a.activityTagsNames)
		return false;
	if(this->studentsNames != a.studentsNames)
		return false;
	//if(this->duration != a.duration)
	  //  return false;
	if((this->activityGroupId==0 && a.activityGroupId!=0) || (this->activityGroupId!=0 && a.activityGroupId==0))
		return false;
	return true;
}

bool Activity::searchTeacher(const QString& teacherName)
{
	return this->teachersNames.indexOf(teacherName)!=-1;
}

bool Activity::removeTeacher(const QString& teacherName)
{
	int t=this->teachersNames.removeAll(teacherName);
	
	assert(t<=1);
	
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
	return this->studentsNames.indexOf(studentsName)!=-1;
}

bool Activity::removeStudents(Rules& r, const QString& studentsName, int nStudents)
{
	Q_UNUSED(r);

	int t=this->studentsNames.removeAll(studentsName);
	
	assert(t<=1);

	if(t>0 && this->computeNTotalStudents==true){
		/*StudentsSet* s=r.searchStudentsSet(studentsName);
		assert(s!=nullptr);
		this->nTotalStudents-=s->numberOfStudents;*/
		this->nTotalStudents-=t*nStudents;
		assert(this->nTotalStudents>=0);
	}
	
	return t>0;
}

void Activity::renameStudents(Rules& r, const QString& initialStudentsName, const QString& finalStudentsName, int initialNumberOfStudents, int finalNumberOfStudents)
{
	Q_UNUSED(r);

	int t=0;
	for(QStringList::iterator it=this->studentsNames.begin(); it!=this->studentsNames.end(); it++)
		if((*it)==initialStudentsName){
			/*if(this->computeNTotalStudents==true){
				StudentsSet* s=r.searchStudentsSet(initialStudentsName);
				assert(s!=nullptr);
				this->nTotalStudents-=s->numberOfStudents;
				
				StudentsSet* s2=r.searchStudentsSet(finalStudentsName);
				assert(s2!=nullptr);
				this->nTotalStudents+=s2->numberOfStudents;
				
				assert(this->nTotalStudents>=0);
			}*/
		
			*it=finalStudentsName;
			t++;
			
			if(this->computeNTotalStudents){
				assert(initialNumberOfStudents>=0);
				assert(finalNumberOfStudents>=0);
			
				nTotalStudents-=initialNumberOfStudents;
				assert(nTotalStudents>=0);
				nTotalStudents+=finalNumberOfStudents;
			}
		}
	assert(t<=1);
}

void Activity::computeInternalStructure(Rules& r)
{
	//the internal subgroups list must be computed before entering here.

	//teachers
	//this->nTeachers=0;
	this->iTeachersList.clear();
	QSet<int> iTeachersSet;
	for(QStringList::const_iterator it=this->teachersNames.constBegin(); it!=this->teachersNames.constEnd(); it++){
		int tmp=r.teachersHash.value(*it, -1);
		/*for(tmp=0; tmp<r.nInternalTeachers; tmp++){
			if(r.internalTeachersList[tmp]->name == (*it))
				break;
		}*/
		assert(tmp>=0 && tmp < r.nInternalTeachers);
		//assert(this->nTeachers<MAX_TEACHERS_PER_ACTIVITY);
		//this->teachers[this->nTeachers++]=tmp;
		if(!iTeachersSet.contains(tmp)){
			iTeachersSet.insert(tmp);
			this->iTeachersList.append(tmp);
		}
	}

	//subjects
	this->subjectIndex = r.subjectsHash.value(subjectName, -1); //r.searchSubject(this->subjectName);
	assert(this->subjectIndex>=0);

	//activity tags
	this->iActivityTagsSet.clear();
	for(const QString& tag : std::as_const(this->activityTagsNames)){
		assert(tag!="");
		int index=r.activityTagsHash.value(tag, -1); //r.searchActivityTag(tag);
		assert(index>=0);
		if(!iActivityTagsSet.contains(index))
			this->iActivityTagsSet.insert(index);
	}
	//this->activityTagIndex = r.searchActivityTag(this->activityTagName);

	//students
	//this->nSubgroups=0;
	this->iSubgroupsList.clear();
	QSet<int> iSubgroupsSet;
	for(QStringList::const_iterator it=this->studentsNames.constBegin(); it!=this->studentsNames.constEnd(); it++){
		StudentsSet* ss=r.studentsHash.value(*it, nullptr); //r.searchAugmentedStudentsSet(*it);
		assert(ss!=nullptr);
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
			if(iSubgroupsSet.contains(tmp))
			//for(int j=0; j<this->nSubgroups; j++)
			//	if(this->subgroups[j]==tmp)
					duplicate=true;
			if(duplicate){
				/*QString s;
				s=QString("Warning: activity with id=%1 contains duplicated subgroups. Automatically correcting...")
					.arg(this->id);
				cout<<qPrintable(s)<<endl;*/
			}
			else{
				//this->subgroups[this->nSubgroups++]=tmp;
				iSubgroupsSet.insert(tmp);
				this->iSubgroupsList.append(tmp);
			}
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
				if(iSubgroupsSet.contains(tmp))
				//for(int j=0; j<this->nSubgroups; j++)
				//	if(this->subgroups[j]==tmp)
						duplicate=true;
				if(duplicate){
					/*QString s;
					s=QString("Warning: activity with id=%1 contains duplicated subgroups. Automatically correcting...")
						.arg(this->id);
					cout<<qPrintable(s)<<endl;*/
				}
				else{
					//this->subgroups[this->nSubgroups++]=tmp;
					iSubgroupsSet.insert(tmp);
					this->iSubgroupsList.append(tmp);
				}
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
					if(iSubgroupsSet.contains(tmp))
					//for(int j=0; j<this->nSubgroups; j++)
					//	if(this->subgroups[j]==tmp)
							duplicate=true;
					if(duplicate){
						/*QString s;
						s=QString("Warning: activity with id=%1 contains duplicated subgroups. Automatically correcting...")
							.arg(this->id);
						cout<<qPrintable(s)<<endl;*/
					}
					else{
						//this->subgroups[this->nSubgroups++]=tmp;
						iSubgroupsSet.insert(tmp);
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
	Q_UNUSED(r);

	QString s=IL2+"<Activity>\n";

	for(QStringList::const_iterator it=this->teachersNames.constBegin(); it!=this->teachersNames.constEnd(); it++)
		s+=IL3+"<Teacher>" + protect(*it) + "</Teacher>\n";

	s+=IL3+"<Subject>" + protect(this->subjectName) + "</Subject>\n";

	for(const QString& tag : std::as_const(this->activityTagsNames))
		s+=IL3+"<Activity_Tag>" + protect(tag) + "</Activity_Tag>\n";

	for(QStringList::const_iterator it=this->studentsNames.constBegin(); it!=this->studentsNames.constEnd(); it++)
		s+=IL3+"<Students>" + protect(*it) + "</Students>\n";

	s+=IL3+"<Duration>"+CustomFETString::number(this->duration)+"</Duration>\n";
	s+=IL3+"<Total_Duration>"+CustomFETString::number(this->totalDuration)+"</Total_Duration>\n";
	s+=IL3+"<Id>"+CustomFETString::number(this->id)+"</Id>\n";
	s+=IL3+"<Activity_Group_Id>"+CustomFETString::number(this->activityGroupId)+"</Activity_Group_Id>\n";

	if(this->computeNTotalStudents==false)
		s+=IL3+"<Number_Of_Students>"+CustomFETString::number(this->nTotalStudents)+"</Number_Of_Students>\n";

	s+=IL3+"<Active>";
	if(this->active==true)
		s+="true";
	else
		s+="false";
	s+="</Active>\n";

	s+=IL3+"<Comments>"+protect(comments)+"</Comments>\n";

	s+=IL2+"</Activity>";

	return s;
}

QString Activity::getDescription(Rules& r)
{
	const int INDENT=4;

	Q_UNUSED(r);
	
	bool _indent;
	if(this->isSplit() && this->id!=this->activityGroupId)
		_indent=true;
	else
		_indent=false;
		
	bool indentRepr;
	if(this->isSplit() && this->id==this->activityGroupId)
		indentRepr=true;
	else
		indentRepr=false;
		
	QString _teachers="";
	if(teachersNames.count()==0)
		_teachers=tr("no teachers");
	else
		_teachers=this->teachersNames.join(translatedComma());

	QString _subject=this->subjectName;
	
	QString _activityTags=this->activityTagsNames.join(translatedComma());

	QString _students="";
	if(studentsNames.count()==0)
		_students=tr("no students");
	else
		_students=this->studentsNames.join(translatedComma());

	QString _id;
	_id = CustomFETString::number(id);

	QString _agid="";
	if(this->isSplit())
		_agid = CustomFETString::number(this->activityGroupId);

	QString _duration=CustomFETString::number(this->duration);
	
	QString _totalDuration="";
	if(this->isSplit())
		_totalDuration = CustomFETString::number(this->totalDuration);

	QString _active;
	if(this->active==true)
		_active="";
	else
		_active="X";

	QString _nstudents="";
	if(this->computeNTotalStudents==false)
		_nstudents=CustomFETString::number(this->nTotalStudents);

	/////////
	QString s="";
	if(_indent)
		s+=QString(INDENT, ' ');
		
	s+=_id;
	s+=" - ";

	if(indentRepr)
		s+=QString(INDENT, ' ');
		
	if(_active!=""){
		s+=_active;
		s+=" - ";
	}
	
	s+=_duration;
	if(this->isSplit()){
		s+="/";
		s+=_totalDuration;
	}
	s+=" - ";
	
	s+=_teachers;
	s+=" - ";
	s+=_subject;
	s+=" - ";
	if(_activityTags!=""){
		s+=_activityTags;
		s+=" - ";
	}
	s+=_students;

	if(_nstudents!=""){
		s+=" - ";
		s+=_nstudents;
	}
	
	if(!comments.isEmpty()){
		s+=" - ";
		s+=comments;
	}

	return s;
}

QString Activity::getDetailedDescription(Rules& r)
{
	Q_UNUSED(r);

	QString s;

	s=tr("Activity:");
	s+="\n";

	//Id, AGId
	s += tr("Id=%1").arg(CustomFETString::number(id));
	s+="\n";
	if(this->isSplit()){
		s += tr("Activity group id=%1").arg(CustomFETString::number(this->activityGroupId));
		s+="\n";
	}

	if(!this->isSplit()){
		s+=tr("Component number=%1 (single component activity)", "The split index of this (sub)activity, which will always be equal to 1").arg(this->componentNumber());
		s+="\n";
	}
	else{
		s+=tr("Component number=%1 (in a larger split activity)", "The split index of this subactivity in the larger split activity").arg(this->componentNumber());
		s+="\n";
	}
	
	//Dur, TD
	s+=tr("Duration=%1").arg(CustomFETString::number(this->duration));
	s+="\n";
	if(this->isSplit()){
		s += tr("Total duration=%1").arg(CustomFETString::number(this->totalDuration));
		s+="\n";
	}
	
	if(teachersNames.count()==0){
		s+=tr("No teachers for this activity");
		s+="\n";
	}
	else{
		for(QStringList::const_iterator it=this->teachersNames.constBegin(); it!=this->teachersNames.constEnd(); it++){
			s+=tr("Teacher=%1").arg(*it);
			s+="\n";
		}
	}

	s+=tr("Subject=%1").arg(this->subjectName);
	s+="\n";
	for(const QString& tag : std::as_const(this->activityTagsNames)){
		assert(tag!="");
		s+=tr("Activity tag=%1").arg(tag);
		s+="\n";
	}

	if(studentsNames.count()==0){
		s+=tr("No students sets for this activity");
		s+="\n";
	}
	else{
		for(QStringList::const_iterator it=this->studentsNames.constBegin(); it!=this->studentsNames.constEnd(); it++){
			s+=tr("Students=%1").arg(*it);
			s+="\n";
		}
	}
	
	if(this->computeNTotalStudents==true){
		s+=tr("Total number of students=%1").arg(this->nTotalStudents);
		s+=" ("+tr("computed", "Computed means that the total number of students was computed for the activity, from the number of students of the constituent students sets")+")";
		s+="\n";
	}
	else{
		s+=tr("Total number of students=%1").arg(this->nTotalStudents);
		s+=" ("+tr("specified", "Specified means that the total number of students was specified for the activity")+")";
		s+="\n";
	}
	
	//Not active?
	QString activeYesNo;
	if(this->active==true)
		activeYesNo=tr("yes");
	else
		activeYesNo=tr("no");
	if(!active){
		s+=tr("Active activity=%1", "Represents a yes/no value, if an activity is active or not, %1 is yes or no").arg(activeYesNo);
		s+="\n";
	}

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

QString Activity::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription(r);

	s+="--------------------------------------------------\n";
	s+=tr("Time constraints directly related to this activity:");
	s+="\n";
	for(int i=0; i<r.timeConstraintsList.size(); i++){
		TimeConstraint* c=r.timeConstraintsList[i];
		if(c->isRelatedToActivity(r, this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}

	s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this activity:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToActivity(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	if(r.groupActivitiesInInitialOrderList.count()>0){
		s+=tr("Timetable generation options directly related to this activity:");
		s+="\n";
		for(int i=0; i<r.groupActivitiesInInitialOrderList.count(); i++){
			GroupActivitiesInInitialOrderItem* item=r.groupActivitiesInInitialOrderList[i];
			if(item->idsSet.contains(id)){
				s+="\n";
				s+=item->getDetailedDescription(r);
			}
		}
		s+="--------------------------------------------------\n";
	}

	return s;
}

bool Activity::isSplit()
{
	return this->totalDuration != this->duration;
}

bool Activity::representsComponentNumber(int index)
{
	if(this->activityGroupId==0)
		return index==1;
		//return false;
		
	//assert(this->activityGroupId>0);
	
	return index == (this->id - this->activityGroupId + 1);
}

int Activity::componentNumber()
{
	if(this->activityGroupId==0)
		return 1;
	
	return this->id - this->activityGroupId + 1;
}
