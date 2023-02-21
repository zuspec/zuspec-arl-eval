/*
 * ModelEvaluator.cpp
 *
 *  Created on: Apr 29, 2022
 *      Author: mballance
 */

#include "ModelEvaluator.h"
#include "ModelEvaluatorFullElab.h"
#include "ModelEvaluatorIncrElab.h"
#include "ModelEvaluatorThread.h"
#include "TaskCollectTopLevelActivities.h"

namespace zsp {
namespace arl {
namespace eval {

ModelEvaluator::ModelEvaluator(
	dm::IContext 			*ctxt,
	dm::ModelEvaluatorKind	kind) : m_ctxt(ctxt) {
	switch (kind) {
		case dm::ModelEvaluatorKind::FullElab:
			m_impl = IModelEvaluatorUP(new ModelEvaluatorFullElab(ctxt));
			break;
		case dm::ModelEvaluatorKind::IncrElab:
			m_impl = IModelEvaluatorUP(new ModelEvaluatorIncrElab(ctxt));
			break;
	}
}

ModelEvaluator::~ModelEvaluator() {
	// TODO Auto-generated destructor stub
}

dm::IModelEvalIterator *ModelEvaluator::eval(
			const vsc::solvers::IRandState	*randstate,
			dm::IModelFieldComponent		*root_comp,
			dm::IDataTypeAction				*root_action) {
	return m_impl->eval(randstate, root_comp, root_action);
}


}
} /* namespace arl */
}
