//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu <Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <https://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
  ***************************************************************************/

#include "room.h"
#include "rules.h"
#include "messageboxes.h"

static QString trueFalse(bool x)
{
	if(!x)
		return QString("false");
	else
		return QString("true");
}

static QString yesNoTranslated(bool x)
{
	if(!x)
		return QCoreApplication::translate("Room", "no", "no - meaning negation");
	else
		return QCoreApplication::translate("Room", "yes", "yes - meaning affirmative");
}

Room::Room()
{
	isVirtual=false;
	realRoomsSetsList.clear();
	this->capacity=MAX_ROOM_CAPACITY;
	this->building=QString("");
	comments=QString("");
}

Room::~Room()
{
}

void Room::computeInternalStructure(Rules& r)
{
	if(building=="")
		buildingIndex=-1;
	else{
		buildingIndex=r.buildingsHash.value(building, -1);
		assert(buildingIndex>=0 && buildingIndex<r.nInternalBuildings);
	}
}

void Room::computeInternalStructureRealRoomsSetsList(Rules& r)
{
	//This function is called in Rules after Rules::roomsHash is computed
	
	if(this->isVirtual==false){
		this->rrsl.clear();
		return;
	}
	
	assert(this->isVirtual==true);
	
	this->rrsl.clear();
	for(const QStringList& setsList : qAsConst(this->realRoomsSetsList)){
		QList<int> sl;
		
		for(const QString& realRoom : qAsConst(setsList)){
			int rr=r.roomsHash.value(realRoom, -1);
			assert(rr>=0);
			
			sl.append(rr);
		}
		
		assert(sl.count()>0);

		this->rrsl.append(sl);
	}
	
	assert(this->rrsl.count()>0);
	
	return;
}

QString Room::getDescription()
{
	QString s=tr("N:%1", "Name of room").arg(this->name);
	s+=", ";
	
	if(this->building!=""){
		s+=tr("B:%1", "Building").arg(this->building);
		s+=", ";
	}
	/*s+=tr("T:");
	s+=this->type;
	s+=",";*/
	s+=tr("C:%1", "Capacity").arg(CustomFETString::number(this->capacity));
	
	if(this->isVirtual){
		s+=", ";
		s+=tr("V:%1", "The room is virtual: yes or no.").arg(yesNoTranslated(this->isVirtual));
		int i=0;
		for(const QStringList& sl : qAsConst(this->realRoomsSetsList)){
			s+=", ";
			s+=tr("S%1(%2)", "Set %1 is composed of rooms, which form the list %2").arg(i+1).arg(sl.join(","));
			
			i++;
		}
	}
	//s+=",";

	/*for(QStringList::Iterator it=this->equipments.begin(); it!=this->equipments.end(); it++)
		s+="E:"+(*it)+", ";*/
	
	QString end=QString("");
	if(!comments.isEmpty())
		end=QString(", ")+tr("C: %1", "Comments").arg(comments);

	return s+end;
}

QString Room::getDetailedDescription()
{
	QString s=tr("Room");
	s+="\n";
	s+=tr("Name=%1", "The name of the room").arg(this->name);
	s+="\n";

	if(this->building!=""){
		s+=tr("Building=%1").arg(this->building);
		s+="\n";
	}
	s+=tr("Capacity=%1").arg(CustomFETString::number(this->capacity));
	s+="\n";

	if(this->isVirtual){
		s+=tr("Virtual=%1", "The room is virtual: yes or no.").arg(yesNoTranslated(this->isVirtual));
		s+="\n";
		int i=0;
		for(const QStringList& sl : qAsConst(this->realRoomsSetsList)){
			s+=tr("Set number %1=(%2)", "Set %1 is composed of rooms, which form the list %2").arg(i+1).arg(sl.join(", "));
			s+="\n";
			
			i++;
		}
	}

	//Has comments?
	if(!comments.isEmpty()){
		s+=tr("Comments=%1").arg(comments);
		s+="\n";
	}

	return s;
}

QString Room::getXmlDescription()
{
	QString s="<Room>\n";
	s+="	<Name>"+protect(this->name)+"</Name>\n";
	s+="	<Building>"+protect(this->building)+"</Building>\n";
	//s+="	<Type>"+protect(this->type)+"</Type>\n";
	s+="	<Capacity>"+CustomFETString::number(this->capacity)+"</Capacity>\n";
	s+="	<Virtual>"+trueFalse(this->isVirtual)+"</Virtual>\n";
	if(this->isVirtual){
		s+="	<Number_of_Sets_of_Real_Rooms>"+QString::number(this->realRoomsSetsList.count())+"</Number_of_Sets_of_Real_Rooms>\n";
		for(const QStringList& sl : qAsConst(this->realRoomsSetsList)){
			s+="	<Set_of_Real_Rooms>\n";
			s+="		<Number_of_Real_Rooms>"+QString::number(sl.count())+"</Number_of_Real_Rooms>\n";
			for(const QString& rn : qAsConst(sl))
				s+="		<Real_Room>"+protect(rn)+"</Real_Room>\n";
			s+="	</Set_of_Real_Rooms>\n";
		}
	}
	s+="	<Comments>"+protect(comments)+"</Comments>\n";
	
	/*for(QStringList::Iterator it=this->equipments.begin(); it!=this->equipments.end(); it++)
		s+="	<Equipment>"+protect(*it)+"</Equipment>\n";*/
	
	s+="</Room>\n";

	return s;
}

QString Room::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=tr("Space constraints directly related to this room:");
	s+="\n";
	for(int i=0; i<r.spaceConstraintsList.size(); i++){
		SpaceConstraint* c=r.spaceConstraintsList[i];
		if(c->isRelatedToRoom(this)){
			s+="\n";
			s+=c->getDetailedDescription(r);
		}
	}
	s+="--------------------------------------------------\n";

	return s;
}

int roomsAscending(const Room* r1, const Room* r2)
{
	//return r1->name < r2->name;
	//by Rodolfo Ribeiro Gomes
	return r1->name.localeAwareCompare(r2->name)<0;
}
