#pragma once
#include "../TargetLKPInfo.h"
#include "../BT_SDTAIController.h"
#include "CoreMinimal.h"

/**
 *
 */
class SOFTDESIGNTRAINING_API GroupManager
{
public:
    static GroupManager *GetInstance();
    static void Destroy();

    void RegisterController(ABT_SDTAIController *controller);
    void UnregisterController(ABT_SDTAIController *controller);
    bool IsControllerRegistered(ABT_SDTAIController *controller);

    TargetLKPInfo GetLKPFromGroup(bool &targetFound);

private:
    // SINGLETON
    GroupManager();
    static GroupManager *m_Instance;

    TSet<ABT_SDTAIController *> m_registeredControllers;
};
