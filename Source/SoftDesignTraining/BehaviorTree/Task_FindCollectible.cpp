// Fill out your copyright notice in the Description page of Project Settings.


#include "Task_FindCollectible.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../SDTCollectible.h"
#include "../SDTAIController.h"
#include "../SDTUtils.h"


EBTNodeResult::Type UTask_FindCollectible::ExecuteTask(UBehaviorTreeComponent& ownerComp, uint8* nodeMemory)
{
    UBlackboardComponent* blackboard = ownerComp.GetBlackboardComponent();
    if (blackboard == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    if (BlackboardKey.SelectedKeyType != UBlackboardKeyType_Object::StaticClass())
    {
        return EBTNodeResult::Failed;
    }

    double beginSearchCollectible = FPlatformTime::Seconds();
    auto result = FindCollectible(blackboard, nodeMemory);
    double endSearchCollectible = FPlatformTime::Seconds();
    blackboard->SetValueAsFloat("TimeSpentFindCollectible", (endSearchCollectible - beginSearchCollectible) * 1000.0);
    return result;
}


EBTNodeResult::Type UTask_FindCollectible::FindCollectible(UBlackboardComponent* blackboard, uint8* nodeMemory)
{
    float closestSqrCollectibleDistance = 18446744073709551610.f;
    ASDTCollectible* closestCollectible = nullptr;

    TArray<AActor*> foundCollectibles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundCollectibles);

    while (foundCollectibles.Num() != 0)
    {
        int index = FMath::RandRange(0, foundCollectibles.Num() - 1);

        ASDTCollectible* collectibleActor = Cast<ASDTCollectible>(foundCollectibles[index]);
        if (!collectibleActor)
            return EBTNodeResult::Failed;

        if (!collectibleActor->IsOnCooldown())
        {
            blackboard->SetValueAsObject(BlackboardKey.SelectedKeyName, foundCollectibles[index]);
            return EBTNodeResult::Succeeded;
        }
        else
        {
            foundCollectibles.RemoveAt(index);
        }

    }

    return EBTNodeResult::Failed;
}
