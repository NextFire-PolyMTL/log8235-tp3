// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_FindCollectible.h"
#include "../BT_SDTAIController.h"
#include "../SDTCollectible.h"
#include "../SoftDesignTraining.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UBTService_FindCollectible::UBTService_FindCollectible()
{
    bCreateNodeInstance = true;
}

void UBTService_FindCollectible::TickNode(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory, float DeltaSeconds)
{
    auto ctrl = Cast<ABT_SDTAIController>(OwnerComp.GetAIOwner());

    float closestSqrCollectibleDistance = 18446744073709551610.f;
    ASDTCollectible *closestCollectible = nullptr;

    TArray<AActor *> foundCollectibles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundCollectibles);

    for (AActor *CollectibleActor : foundCollectibles)
    {
        ASDTCollectible *Collectible = Cast<ASDTCollectible>(CollectibleActor);
        if (Collectible && !Collectible->IsOnCooldown())
        {
            float CollectibleDistance = FVector::DistSquared(Collectible->GetActorLocation(), ctrl->GetPawn()->GetActorLocation());
            if (CollectibleDistance < closestSqrCollectibleDistance)
            {
                closestSqrCollectibleDistance = CollectibleDistance;
                closestCollectible = Collectible;
            }
        }
    }

    if (closestCollectible)
    {
        OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(ctrl->GetTargetPositionBBKeyID(), closestCollectible->GetActorLocation());
    }
}
