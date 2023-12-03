#pragma once
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

    TArray<ABT_SDTAIController *> GetRegisteredControllers() const { return m_registeredControllers; }

    FVector GetLKPFromGroup(bool &targetFound);

    bool ShouldInvestigate() const { return m_shouldInvestigate; }
    void SetShouldInvestigate(bool shouldInvestigate) { m_shouldInvestigate = shouldInvestigate; }

private:
    // SINGLETON
    GroupManager();
    static GroupManager *m_Instance;

    bool m_shouldInvestigate = false;

    TArray<ABT_SDTAIController *> m_registeredControllers;
};
