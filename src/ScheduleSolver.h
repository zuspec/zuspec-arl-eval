/*
 * ScheduleSolver.h
 *
 *  Created on: May 14, 2022
 *      Author: mballance
 */

#pragma once
#include <vector>
#include "zsp/arl/dm/IContext.h"
#include "ExecGraphNode.h"
#include "IScheduleSolveStage.h"
#include "ModelFieldComponent.h"
#include "IScheduleSolvePropagator.h"
#include "ScheduleSolveStage.h"

namespace zsp {
namespace arl {
namespace eval {

class ScheduleSolver {
public:
	ScheduleSolver(dm::IContext *ctxt);

	virtual ~ScheduleSolver();

	/**
	 * Returns an execution graph containing model fields.
	 * The graph owns all action instances
	 */
	std::pair<ExecGraphNode *, std::string> solve(
			dm::ModelFieldComponent				*root_comp,
			dm::IDataTypeAction					*root_action);


private:
	dm::IContext								*m_ctxt;
	std::vector<IScheduleSolvePropagatorUP>	m_stage_stack;
	bool									m_complete;

};

}
} /* namespace arl */
}

