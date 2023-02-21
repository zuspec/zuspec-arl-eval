/*
 * ScheduleGraphBuilder.h
 *
 *  Created on: May 7, 2022
 *      Author: mballance
 */

#pragma once
#include <vector>
#include "ScheduleGraphNode.h"
#include "ScheduleSolveDataActionRel.h"

namespace zsp {
namespace arl {
namespace eval {

class ScheduleGraphBuilder {
public:
	ScheduleGraphBuilder();

	virtual ~ScheduleGraphBuilder();

	void build(
			std::vector<ScheduleGraphNodeUP>		&nodes,
			const std::vector<vsc::dm::IModelField *>	&actions,
			const ScheduleSolveDataActionRel					&schedrel);

};

}
} /* namespace arl */
}

