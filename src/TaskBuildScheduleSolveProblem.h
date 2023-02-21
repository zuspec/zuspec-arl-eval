/*
 * TaskBuildScheduleSolveProblem.h
 *
 *  Created on: Jun 8, 2022
 *      Author: mballance
 */

#pragma once
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "ScheduleSolveActionData.h"
#include "ScheduleSolveProblem.h"

namespace zsp {
namespace arl {
namespace eval {

class TaskBuildScheduleSolveProblem : public dm::VisitorBase {
public:

	TaskBuildScheduleSolveProblem();

	virtual ~TaskBuildScheduleSolveProblem();

	ScheduleSolveProblem *build(dm::IModelActivity *activity);

	virtual void visitModelActivityParallel(dm::IModelActivityParallel *a) override;

	virtual void visitModelActivitySchedule(dm::IModelActivitySchedule *a) override;

	virtual void visitModelActivitySequence(dm::IModelActivitySequence *a) override;

	virtual void visitModelActivityTraverse(dm::IModelActivityTraverse *a) override;

private:
	enum Kind {
		Parallel,
		Schedule,
		Sequence
	};
	struct Frame {
		Kind									kind;
		std::vector<ScheduleSolveActionData *>	elems;
	};


private:
	ScheduleSolveProblemUP				m_problem;
	std::vector<Frame>					m_frame_s;

};

}
} /* namespace arl */
}
