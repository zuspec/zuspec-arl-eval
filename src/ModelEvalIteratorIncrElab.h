/*
 * ModelEvalIteratorIncrElab.h
 *
 *  Created on: Apr 29, 2022
 *      Author: mballance
 */

#pragma once
#include <vector>
#include "zsp/arl/dm/IModelEvalIterator.h"
#include "zsp/arl/dm/IModelFieldAction.h"

namespace zsp {
namespace arl {
namespace eval {


class ModelEvalIteratorIncrElab : public dm::IModelEvalIterator {
public:
	struct Elem {
		dm::ModelEvalNodeT		type;
		union {
			dm::IModelFieldAction	*action;
			dm::IModelEvalIterator	*iterator;
		};
	};
public:
	ModelEvalIteratorIncrElab(const Elem &elem);

	ModelEvalIteratorIncrElab(const std::vector<Elem> &seq);

	virtual ~ModelEvalIteratorIncrElab();

	virtual bool next() override;

	virtual bool valid() override;

	virtual dm::ModelEvalNodeT type() const override;

	virtual dm::IModelFieldAction *action() override;

	virtual dm::IModelEvalIterator *iterator() override;

private:
	std::vector<Elem>						m_seq;
	int32_t									m_idx;


};

}
} /* namespace arl */
}

