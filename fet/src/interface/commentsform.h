//
//
// C++ Interface: $MODULE$
//
// Description: 
//
//
// Author: Lalescu Liviu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef COMMENTSFORM_H
#define COMMENTSFORM_H

#include "commentsform_template.h"

class CommentsForm : public CommentsForm_template
{
public:
	CommentsForm();

	~CommentsForm();

	void ok();
	void cancel();
};

#endif
