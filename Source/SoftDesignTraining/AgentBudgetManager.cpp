// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentBudgetManager.h"
#include "Kismet/GameplayStatics.h"

AAgentBudgetManager::AAgentBudgetManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AAgentBudgetManager::RegisterAgent(ABT_SDTAIController* agent)
{
	RegisteredAgents.Add(agent);
}

void AAgentBudgetManager::UnregisterAgent(ABT_SDTAIController* agent)
{
	RegisteredAgents.Remove(agent);
	CurrentIdx = FMath::Max(0, FMath::Min(CurrentIdx, RegisteredAgents.Num() - 1));
}

void AAgentBudgetManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RegisteredAgents.Num() == 0)
	{
		return;
	}

	auto world = GetWorld();

	auto currentTime = FPlatformTime::Seconds();
	double newTime = 0.0;
	int stopIdx = CurrentIdx;
	do
	{
		RegisteredAgents[CurrentIdx]->UpdateAgent();
		newTime = FPlatformTime::Seconds();
		CurrentIdx = (CurrentIdx + 1) % RegisteredAgents.Num();
	} while (newTime - currentTime < TimeBudget && stopIdx != CurrentIdx);
	GEngine->AddOnScreenDebugMessage(200, 0.0f, FColor::Black, FString::Printf(TEXT("Nb agents updated: %i"), stopIdx == CurrentIdx ? RegisteredAgents.Num() : (CurrentIdx < stopIdx ? RegisteredAgents.Num() - stopIdx + CurrentIdx : CurrentIdx - stopIdx)));
}
