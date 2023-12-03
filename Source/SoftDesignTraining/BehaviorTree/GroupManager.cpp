// Fill out your copyright notice in the Description page of Project Settings.

#include "GroupManager.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"

GroupManager *GroupManager::m_Instance;

GroupManager::GroupManager()
{
}

GroupManager *GroupManager::GetInstance()
{
    if (!m_Instance)
    {
        m_Instance = new GroupManager();
    }

    return m_Instance;
}

void GroupManager::Destroy()
{
    delete m_Instance;
    m_Instance = nullptr;
}

void GroupManager::RegisterController(ABT_SDTAIController *controller)
{
    m_registeredControllers.Add(controller);
    m_shouldInvestigate = true;
}

void GroupManager::UnregisterController(ABT_SDTAIController *controller)
{
    m_registeredControllers.Remove(controller);
}

FVector GroupManager::GetLKPFromGroup(bool &targetfound)
{
    FVector outLKP = FVector::ZeroVector;
    float lastUpdatedTimeStamp = 0.f;
    targetfound = false;

    for (auto &controller : m_registeredControllers)
    {
        if (controller)
        {
            auto bb = controller->GetBlackboardComponent();
            auto lastSeen = bb->GetValue<UBlackboardKeyType_Float>(controller->GetLastSeenBBKeyID());
            if (lastSeen && lastSeen > lastUpdatedTimeStamp)
            {
                lastUpdatedTimeStamp = lastSeen;
                outLKP = bb->GetValue<UBlackboardKeyType_Vector>(controller->GetPlayerPositionBBKeyID());
                targetfound = true;
            }
        }
    }

    return outLKP;
}
