/*
 * ModelEvalIteratorIncrElabMgr.cpp
 *
 *  Created on: Jun 6, 2022
 *      Author: mballance
 */

#include "ModelEvalIteratorIncrElabMgr.h"

namespace zsp {
namespace arl {
namespace eval {

ModelEvalIteratorIncrElabMgr::ModelEvalIteratorIncrElabMgr(
		ModelEvaluatorIncrElab *eval) : m_eval(eval) {

}

ModelEvalIteratorIncrElabMgr::~ModelEvalIteratorIncrElabMgr() {
	// TODO Auto-generated destructor stub
}

bool ModelEvalIteratorIncrElabMgr::next() {
	bool ret = false;

	while (m_eval_s.size() > 0) {
		if (m_eval_s.back()->next()) {
			ret = true;
			break;
		}
		// Note: the iterator auto-frees on returning False
		m_eval_s.pop_back();
	}

	if (!ret) {
		// Time to go back to the source
		IModelEvalIterator *it = m_eval->next();

		if (it) {
			ret = it->next();
			if (ret) {
				m_eval_s.push_back(it);
			}
		}
	}

	return ret;
}

dm::ModelEvalNodeT ModelEvalIteratorIncrElabMgr::type() const {
	return m_eval_s.back()->type();
}

dm::IModelFieldAction *ModelEvalIteratorIncrElabMgr::action() {
	return dynamic_cast<dm::IModelFieldAction *>(m_eval_s.back()->action());
}

dm::IModelEvalIterator *ModelEvalIteratorIncrElabMgr::iterator() {
	return m_eval_s.back()->iterator();
}

}
} /* namespace arl */
}
