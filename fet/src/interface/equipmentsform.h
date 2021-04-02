//
//
// C++ Interface: $MODULE$
//
// Description: 
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EQUIPMENTSFORM_H
#define EQUIPMENTSFORM_H

#include "equipmentsform_template.h"

/**
@author Liviu Lalescu
*/
class EquipmentsForm : public EquipmentsForm_template
{
public:
	EquipmentsForm();

	~EquipmentsForm();

	void addEquipment();

	void removeEquipment();

	void modifyEquipment();

	void sortEquipments();

	void equipmentChanged(int index);
};

#endif
