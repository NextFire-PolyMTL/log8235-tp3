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

    float closestSqrCollectibleDistance = 18446744073709551610.f;
    ASDTCollectible* closestCollectible = nullptr;

    TArray<AActor*> foundCollectibles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundCollectibles);

    while (foundCollectibles.Num() != 0)
    {

        auto aiController = ownerComp.GetAIOwner();
        if (aiController == nullptr)
        {
            return EBTNodeResult::Failed;
        }
        auto aiLocation = aiController->GetPawn()->GetActorLocation();
        ASDTCollectible* collectibleActor = nullptr;
        for (int i=0; i<foundCollectibles.Num(); i++)
        {
            collectibleActor = Cast<ASDTCollectible>(foundCollectibles[i]);
            if (!collectibleActor)
                return EBTNodeResult::Failed;
            auto dist = FVector::Dist(aiLocation, collectibleActor->GetActorLocation());
            if (!collectibleActor->IsOnCooldown() && dist < closestSqrCollectibleDistance)
            {
                closestCollectible = collectibleActor;
                closestSqrCollectibleDistance = dist;
            }
        }
        if (!closestCollectible) {
            return EBTNodeResult::Failed;
        }
        else {
            blackboard->SetValueAsObject(BlackboardKey.SelectedKeyName, closestCollectible);
            //MoveToLocation(foundCollectibles[index]->GetActorLocation(), 0.5f, false, true, true, NULL, false);
            //OnMoveToTarget();
            return EBTNodeResult::Succeeded;
        }
        /*
        int index = FMath::RandRange(0, foundCollectibles.Num() - 1);

        ASDTCollectible* collectibleActor = Cast<ASDTCollectible>(foundCollectibles[index]);
        if (!collectibleActor)
            return EBTNodeResult::Failed;

        if (!collectibleActor->IsOnCooldown())
        {
            blackboard->SetValueAsObject(BlackboardKey.SelectedKeyName, foundCollectibles[index]);
            //MoveToLocation(foundCollectibles[index]->GetActorLocation(), 0.5f, false, true, true, NULL, false);
            //OnMoveToTarget();
            return EBTNodeResult::Succeeded;
        }
        else
        {
            foundCollectibles.RemoveAt(index);
        }
        */
    }

    return EBTNodeResult::Failed;
}
