#pragma once
#include "Engine_include.h"
#include "BossZombie.h"

class DLL_API SelectAttackNode : public BehaviorNode
{
public:
    SelectAttackNode() {};
    SelectAttackNode(const vector<shared_ptr<BehaviorNode>>& children) : BehaviorNode(children) {}
    SelectAttackNode(const SelectAttackNode& other) : BehaviorNode(other) {}
    SelectAttackNode& operator=(const SelectAttackNode& other) {
        if (this != &other) {
            BehaviorNode::operator=(other);
        }
        return *this;
    }

public:
    virtual void Execute() override
    {
        if (children_.size() == 0) {
            status_ = BehaviorStatus::SUCCESS;
            return;
        }
        status_ = BehaviorStatus::RUNNING;

        BehaviorStatus child_status = children_[executing_child_node_index_]->GetStatus();

        switch (child_status) {
        case BehaviorStatus::IDLE:
        case BehaviorStatus::RUNNING:
            children_[executing_child_node_index_]->Execute();
            break;
        case BehaviorStatus::FAILURE:
            if (executing_child_node_index_ == children_.size() - 1) {
                status_ = BehaviorStatus::FAILURE;
            }
            else {
                executing_child_node_index_++;
            }
            break;
        case BehaviorStatus::SUCCESS:
            status_ = BehaviorStatus::SUCCESS;
            break;
        }
    };
};