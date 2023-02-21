/*
 * ModelEvalIteratorIncrElabMgr.h
 *
 *  Created on: Jun 6, 2022
 *      Author: mballance
 */

#pragma once
#include "zsp/arl/dm/IModelEvalIterator.h"
#include "ModelEvaluatorIncrElab.h"

namespace zsp {
namespace arl {
namespace eval {

class ModelEvalIteratorIncrElabMgr : public dm::IModelEvalIterator {
public:
	ModelEvalIteratorIncrElabMgr(ModelEvaluatorIncrElab *eval);

	virtual ~ModelEvalIteratorIncrElabMgr();

	virtual bool next() override;

	virtual dm::ModelEvalNodeT type() const override;

	virtual dm::IModelFieldAction *action() override;

	virtual dm::IModelEvalIterator *iterator() override;

private:
	ModelEvaluatorIncrElab				    *m_eval;
	std::vector<dm::IModelEvalIterator *>	m_eval_s;

};

}
} /* namespace arl */
}

