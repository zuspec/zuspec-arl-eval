/*
 * ExecGraphNode.h
 *
 *  Created on: May 2, 2022
 *      Author: mballance
 */

#pragma once
#include <memory>
#include "zsp/arl/dm/IModelActivity.h"
#include "vsc/dm/IModelField.h"

namespace zsp {
namespace arl {
namespace eval {

enum class ExecGraphNodeKindE {
	Sequence,
	Parallel,
	Traverse
};

class ExecGraphNode;
using ExecGraphNodeUP=std::unique_ptr<ExecGraphNode>;
class ExecGraphNode {
public:

	ExecGraphNode(ExecGraphNodeKindE kind);

	ExecGraphNode(dm::IModelActivity *activity);

	ExecGraphNode(ExecGraphNodeKindE kind, dm::IModelActivity *activity);

	virtual ~ExecGraphNode();

	const ExecGraphNodeKindE kind() const { return m_kind; }

	dm::IModelActivity *activity() const { return m_activity; }

	void addChild(ExecGraphNode *c) {
		m_children.push_back(ExecGraphNodeUP(c));
	}

	const std::vector<ExecGraphNodeUP> &children() const { return m_children; }

	std::vector<ExecGraphNodeUP> &children() { return m_children; }

	std::string toString();

protected:
	ExecGraphNodeKindE				m_kind;
	dm::IModelActivity				*m_activity;
	std::vector<ExecGraphNodeUP>	m_children;
};

}
} /* namespace arl */
}
