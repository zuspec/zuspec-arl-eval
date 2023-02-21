/*
 * IScheduleSolvePropagatorQueue.h
 *
 *  Created on: May 20, 2022
 *      Author: mballance
 */

#pragma once

namespace zsp {
namespace arl {
namespace eval {

class IScheduleSolvePropagator;

class IScheduleSolveEvalQueue {
public:

	virtual ~IScheduleSolveEvalQueue() { }

	virtual int32_t eval(
			IScheduleSolveEvalQueue *q,
			int32_t					level) = 0;

	virtual void queueForEval(
			IScheduleSolvePropagator	*src,
			IScheduleSolvePropagator	*dst) = 0;

};

}
}
}
