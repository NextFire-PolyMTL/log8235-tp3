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

bool GroupManager::IsControllerRegistered(ABT_SDTAIController *controller)
{
    return m_registeredControllers.Contains(controller);
}

TargetLKPInfo GroupManager::GetLKPFromGroup(bool &targetfound)
{
    TargetLKPInfo outLKPInfo = TargetLKPInfo();
    targetfound = false;

    for (auto &controller : m_registeredControllers)
    {
        if (controller)
        {
            const TargetLKPInfo &targetLKPInfo = controller->m_currentTargetLkpInfo;
            if (targetLKPInfo.GetLastUpdatedTimeStamp() > outLKPInfo.GetLastUpdatedTimeStamp())
            {
                targetfound = targetLKPInfo.GetLKPState() != TargetLKPInfo::ELKPState::LKPState_Invalid;
                outLKPInfo = targetLKPInfo;
            }
        }
    }

    return outLKPInfo;
}
