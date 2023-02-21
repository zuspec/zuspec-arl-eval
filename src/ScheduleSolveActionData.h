/*
 * ScheduleSolveActionData.h
 *
 *  Created on: May 21, 2022
 *      Author: mballance
 */

#pragma once
#include <memory>
#include "zsp/arl/dm/IModelActivity.h"
#include "vsc/dm/IModelField.h"
#include "ScheduleGraphNode.h"

namespace zsp {
namespace arl {
namespace eval {

class ScheduleSolveActionData;
using ScheduleSolveActionDataUP=std::unique_ptr<ScheduleSolveActionData>;
class ScheduleSolveActionData {
public:
	ScheduleSolveActionData(
			dm::IModelActivity			*activity,
			bool	    				owned
			);

	virtual ~ScheduleSolveActionData();

	ScheduleGraphNode *node() const { return m_node.get(); }

private:
	dm::IModelActivity				*m_activity;
	bool						m_owned;
	ScheduleGraphNodeUP			m_node;

};

}
} /* namespace arl */
}

