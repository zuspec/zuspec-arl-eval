/*
 * ModelEvaluator.h
 *
 *  Created on: Apr 29, 2022
 *      Author: mballance
 */

#pragma once
#include "zsp/arl/dm/impl/VisitorBase.h"
#include "zsp/arl/dm/IContext.h"
#include "zsp/arl/dm/IModelActivity.h"
#include "zsp/arl/dm/IModelFieldComponent.h"
#include "zsp/arl/eval/IModelEvaluator.h"

namespace zsp {
namespace arl {
namespace eval {

class ModelEvaluator : 
    public IModelEvaluator, 
    public dm::VisitorBase {
public:
	ModelEvaluator(
		dm::IContext 				*ctxt,
		ModelEvaluatorKind		    kind);

	virtual ~ModelEvaluator();

	virtual dm::IModelEvalIterator *eval(
			const vsc::solvers::IRandState	*randstate,
			dm::IModelFieldComponent	    *root_comp,
			IDataTypeAction			        *root_action) override;

private:
	dm::IContext						*m_ctxt;
	IModelEvaluatorUP				    m_impl;


};

}
} /* namespace arl */
}

