//
//
// C++ Implementation: $MODULE$
//
// Description: 
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "room.h"
#include "rules.h"

Room::Room()
{
	this->capacity=MAX_ROOM_CAPACITY;
	this->building="";
}

Room::~Room()
{
}

/*void Room::addEquipment(const QString& equipmentName)
{
	this->equipments.append(equipmentName);	
}

void Room::removeEquipment(const QString& equipmentName)
{
	this->equipments.remove(equipmentName);
}

bool Room::searchEquipment(const QString& equipmentName)
{
	int tmp=this->equipments.findIndex(equipmentName);
	if(tmp>=0)
		return true;
	else
		return false;
}*/

void Room::computeInternalStructure(Rules& r)
{
	if(building=="")
		buildingIndex=-1;
	else{
		buildingIndex=r.searchBuilding(building);
		assert(buildingIndex>=0 && buildingIndex<r.nInternalBuildings);
	}
}

QString Room::getDescription()
{
	QString s=QObject::tr("N:%1", "Name").arg(this->name);
	s+=",";
	
	if(this->building!=""){
		s+=QObject::tr("B:%1", "Building").arg(this->building);
		s+=",";
	}
	/*s+=QObject::tr("T:");
	s+=this->type;
	s+=",";*/
	s+=QObject::tr("C:%1", "Capacity").arg(QString::number(this->capacity));
	//s+=",";

	/*for(QStringList::Iterator it=this->equipments.begin(); it!=this->equipments.end(); it++)
		s+="E:"+(*it)+", ";*/

	return s;
}

QString Room::getDetailedDescription()
{
	QString s=QObject::tr("Name=%1").arg(this->name);
	s+="\n";

	if(this->building!=""){
		s+=QObject::tr("Building=%1").arg(this->building);
		s+="\n";
	}
	/*s+=QObject::tr("Type=");
	s+=this->type;
	s+="\n";*/
	s+=QObject::tr("Capacity=%1").arg(QString::number(this->capacity));
	s+="\n";

	/*for(QStringList::Iterator it=this->equipments.begin(); it!=this->equipments.end(); it++)
		s+="Equipment="+(*it)+"\n";*/

	return s;
}

QString Room::getXmlDescription()
{
	QString s="<Room>\n";
	s+="	<Name>"+protect(this->name)+"</Name>\n";
	s+="	<Building>"+protect(this->building)+"</Building>\n";
	//s+="	<Type>"+protect(this->type)+"</Type>\n";
	s+="	<Capacity>"+QString::number(this->capacity)+"</Capacity>\n";
	
	/*for(QStringList::Iterator it=this->equipments.begin(); it!=this->equipments.end(); it++)
		s+="	<Equipment>"+protect(*it)+"</Equipment>\n";*/
	
	s+="</Room>\n";

	return s;
}

QString Room::getDetailedDescriptionWithConstraints(Rules& r)
{
	QString s=this->getDetailedDescription();

	s+="--------------------------------------------------\n";
	s+=QObject::tr("Space constraints directly related to this room:");
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
	return r1->name < r2->name;
}
