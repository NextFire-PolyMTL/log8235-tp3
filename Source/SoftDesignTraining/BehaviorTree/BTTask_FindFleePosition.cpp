// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_FindFleePosition.h"
#include "../BT_SDTAIController.h"
#include "../SoftDesignTraining.h"
#include "../SDTFleeLocation.h"
#include "../SDTUtils.h"
#include "EngineUtils.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

EBTNodeResult::Type UBTTask_FindFleePosition::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory)
{
    auto result = SDTUtils::MeasureExecutionTime(&UBTTask_FindFleePosition::PerformFindFleePosition, this, OwnerComp, NodeMemory);
    OwnerComp.GetBlackboardComponent()->SetValueAsFloat("TimeSpentFindFleePoint", result.first);
    return result.second;
}


EBTNodeResult::Type UBTTask_FindFleePosition::PerformFindFleePosition(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    auto ctrl = Cast<ABT_SDTAIController>(OwnerComp.GetAIOwner());

    float bestLocationScore = 0.f;
    ASDTFleeLocation* bestFleeLocation = nullptr;

    auto playerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!playerCharacter)
    {
        return EBTNodeResult::Failed;
    }

    for (TActorIterator<ASDTFleeLocation> actorIterator(GetWorld(), ASDTFleeLocation::StaticClass()); actorIterator; ++actorIterator)
    {
        ASDTFleeLocation* fleeLocation = Cast<ASDTFleeLocation>(*actorIterator);
        if (fleeLocation)
        {
            float distToFleeLocation = FVector::Dist(fleeLocation->GetActorLocation(), playerCharacter->GetActorLocation());

            FVector selfToPlayer = playerCharacter->GetActorLocation() - ctrl->GetPawn()->GetActorLocation();
            selfToPlayer.Normalize();

            FVector selfToFleeLocation = fleeLocation->GetActorLocation() - ctrl->GetPawn()->GetActorLocation();
            selfToFleeLocation.Normalize();

            float fleeLocationToPlayerAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(selfToPlayer, selfToFleeLocation)));
            float locationScore = distToFleeLocation + fleeLocationToPlayerAngle * 100.f;

            if (locationScore > bestLocationScore)
            {
                bestLocationScore = locationScore;
                bestFleeLocation = fleeLocation;
            }

            DrawDebugString(GetWorld(), FVector(0.f, 0.f, 10.f), FString::SanitizeFloat(locationScore), fleeLocation, FColor::Red, 5.f, false);
        }
    }

    if (bestFleeLocation)
    {
        OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(ctrl->GetFleePositionBBKeyID(), bestFleeLocation->GetActorLocation());
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}
