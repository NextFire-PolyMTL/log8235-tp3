// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_FindCollectiblePosition.h"
#include "../BT_SDTAIController.h"
#include "../SDTCollectible.h"
#include "../SoftDesignTraining.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

EBTNodeResult::Type UBTTask_FindCollectiblePosition::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory)
{
    auto ctrl = Cast<ABT_SDTAIController>(OwnerComp.GetAIOwner());

    float closestSqrCollectibleDistance = 18446744073709551610.f;
    ASDTCollectible *closestCollectible = nullptr;

    TArray<AActor *> foundCollectibles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundCollectibles);

    while (foundCollectibles.Num() != 0)
    {
        int index = FMath::RandRange(0, foundCollectibles.Num() - 1);

        ASDTCollectible *collectibleActor = Cast<ASDTCollectible>(foundCollectibles[index]);
        if (!collectibleActor)
            return EBTNodeResult::Failed;

        if (!collectibleActor->IsOnCooldown())
        {
            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(ctrl->GetCollectiblePositionBBKeyID(), foundCollectibles[index]->GetActorLocation());
            return EBTNodeResult::Succeeded;
        }
        else
        {
            foundCollectibles.RemoveAt(index);
        }
    }
    return EBTNodeResult::Failed;
}
