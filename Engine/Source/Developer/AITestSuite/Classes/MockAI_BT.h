// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "MockAI.h"
#include "MockAI_BT.generated.h"

UCLASS()
class UMockAI_BT : public UMockAI
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	UBehaviorTreeComponent* BTComp;

	static TArray<int32> ExecutionLog;
	TArray<int32> ExpectedResult;

	bool IsRunning() const;
	bool RunBT(UBehaviorTree& BTAsset, EBTExecutionMode::Type RunType = EBTExecutionMode::SingleRun);
};
