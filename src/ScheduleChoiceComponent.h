/*
 * ScheduleChoiceComponent.h
 *
 *  Created on: May 14, 2022
 *      Author: mballance
 */

#pragma once
#include "SchedulePropResultE.h"

namespace zsp {
namespace arl {
namespace eval {

class ScheduleChoiceComponent {
public:
	ScheduleChoiceComponent();

	virtual ~ScheduleChoiceComponent();

	dm::SchedulePropResultE propagate();

private:
	ScheduleChoiceComponent			*m_parent;

};

}
} /* namespace arl */
}

