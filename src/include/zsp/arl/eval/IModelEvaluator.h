/*
 * IModelEvaluator.h
 *
 *  Created on: Apr 24, 2022
 *      Author: mballance
 */

#pragma once
#include "zsp/arl/dm/IModelEvalIterator.h"
#include "zsp/arl/dm/IModelFieldComponent.h"
#include "vsc/dm/IModelField.h"
#include "vsc/solvers/IRandState.h"

namespace zsp {
namespace arl {
namespace eval {


class IDataTypeAction;

enum class ModelEvaluatorKind {
	FullElab,
	IncrElab
};

class IModelEvaluator;
using IModelEvaluatorUP=vsc::dm::UP<IModelEvaluator>;
class IModelEvaluator {
public:

	virtual ~IModelEvaluator() { }

	virtual dm::IModelEvalIterator *eval(
			const vsc::solvers::IRandState	    *randstate,
			dm::IModelFieldComponent	        *root_comp,
			dm::IDataTypeAction		            *root_action) = 0;

};

}
}
}
