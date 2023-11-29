// Fill out your copyright notice in the Description page of Project Settings.

#include "GroupManager.h"

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
}

void GroupManager::UnregisterController(ABT_SDTAIController *controller)
{
    m_registeredControllers.Remove(controller);
}

TargetLKPInfo GroupManager::GetLKPFromGroup(bool &targetfound)
{
    int controllerCount = m_registeredControllers.Num();
    TargetLKPInfo outLKPInfo = TargetLKPInfo();
    targetfound = false;

    for (int i = 0; i < controllerCount; ++i)
    {
        ABT_SDTAIController *controller = m_registeredControllers[i];
        if (controller)
        {
            const TargetLKPInfo &targetLKPInfo = controller->m_currentTargetLkpInfo;
            if ((targetLKPInfo.GetLastUpdatedTimeStamp() > outLKPInfo.GetLastUpdatedTimeStamp()) ||
                (targetLKPInfo.GetLKPState() == TargetLKPInfo::ELKPState::LKPState_ValidByLOS))
            {
                targetfound = targetLKPInfo.GetLKPState() != TargetLKPInfo::ELKPState::LKPState_Invalid;
                outLKPInfo = targetLKPInfo;
            }
        }
    }

    return outLKPInfo;
}
