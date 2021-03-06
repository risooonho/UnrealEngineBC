// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "EnginePrivate.h"
// @todo AIModule circular dependency
#include "Navigation/NavigationComponent.h"
#include "AI/Navigation/NavFilters/NavigationQueryFilter.h"

//----------------------------------------------------------------------//
// FNavigationQueryFilter
//----------------------------------------------------------------------//
FNavigationQueryFilter::FNavigationQueryFilter(const FNavigationQueryFilter& Source)
{
	Assign(Source);
}

FNavigationQueryFilter::FNavigationQueryFilter(const FNavigationQueryFilter* Source)
: MaxSearchNodes(DefaultMaxSearchNodes)
{
	if (Source != NULL)
	{
		Assign(*Source);
	}
}

FNavigationQueryFilter::FNavigationQueryFilter(const TSharedPtr<FNavigationQueryFilter> Source)
: MaxSearchNodes(DefaultMaxSearchNodes)
{
	if (Source.IsValid())
	{
		SetFilterImplementation(Source->GetImplementation());
	}
}

FNavigationQueryFilter& FNavigationQueryFilter::operator=(const FNavigationQueryFilter& Source)
{
	Assign(Source);
	return *this;
}

void FNavigationQueryFilter::Assign(const FNavigationQueryFilter& Source)
{
	if (Source.GetImplementation() != NULL)
	{
		QueryFilterImpl = Source.QueryFilterImpl;
	}
	MaxSearchNodes = Source.GetMaxSearchNodes();
}

TSharedPtr<FNavigationQueryFilter> FNavigationQueryFilter::GetCopy() const
{
	TSharedPtr<FNavigationQueryFilter> Copy = MakeShareable(new FNavigationQueryFilter());
	Copy->QueryFilterImpl = MakeShareable(QueryFilterImpl->CreateCopy());
	Copy->MaxSearchNodes = MaxSearchNodes;

	return Copy;
}

void FNavigationQueryFilter::SetAreaCost(uint8 AreaType, float Cost)
{
	check(QueryFilterImpl.IsValid());
	QueryFilterImpl->SetAreaCost(AreaType, Cost);
}

void FNavigationQueryFilter::SetFixedAreaEnteringCost(uint8 AreaType, float Cost)
{
	check(QueryFilterImpl.IsValid());
	QueryFilterImpl->SetFixedAreaEnteringCost(AreaType, Cost);
}

void FNavigationQueryFilter::SetExcludedArea(uint8 AreaType)
{
	QueryFilterImpl->SetExcludedArea(AreaType);
}

void FNavigationQueryFilter::SetAllAreaCosts(const TArray<float>& CostArray)
{
	SetAllAreaCosts(CostArray.GetData(), CostArray.Num());
}

void FNavigationQueryFilter::SetAllAreaCosts(const float* CostArray, const int32 Count)
{
	QueryFilterImpl->SetAllAreaCosts(CostArray, Count);
}

void FNavigationQueryFilter::GetAllAreaCosts(float* CostArray, float* FixedCostArray, const int32 Count) const
{
	QueryFilterImpl->GetAllAreaCosts(CostArray, FixedCostArray, Count);
}

void FNavigationQueryFilter::SetIncludeFlags(uint16 Flags)
{
	QueryFilterImpl->SetIncludeFlags(Flags);
}

uint16 FNavigationQueryFilter::GetIncludeFlags() const
{
	return QueryFilterImpl->GetIncludeFlags();
}

void FNavigationQueryFilter::SetExcludeFlags(uint16 Flags)
{
	QueryFilterImpl->SetExcludeFlags(Flags);
}

uint16 FNavigationQueryFilter::GetExcludeFlags() const
{
	return QueryFilterImpl->GetExcludeFlags();
}

//----------------------------------------------------------------------//
// UNavigationQueryFilter
//----------------------------------------------------------------------//
UNavigationQueryFilter::UNavigationQueryFilter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	IncludeFlags.Packed = 0xffff;
	ExcludeFlags.Packed = 0;
}

TSharedPtr<const FNavigationQueryFilter> UNavigationQueryFilter::GetQueryFilter(const ANavigationData* NavData) const
{
	TSharedPtr<const FNavigationQueryFilter> SharedFilter = NavData->GetQueryFilter(GetClass());
	if (!SharedFilter.IsValid())
	{
		FNavigationQueryFilter* NavFilter = new FNavigationQueryFilter();
		NavFilter->SetFilterImplementation(NavData->GetDefaultQueryFilterImpl());

		InitializeFilter(NavData, NavFilter);

		SharedFilter = MakeShareable(NavFilter);
		((ANavigationData*)NavData)->StoreQueryFilter(GetClass(), SharedFilter);
	}

	return SharedFilter;
}

void UNavigationQueryFilter::InitializeFilter(const ANavigationData* NavData, FNavigationQueryFilter* Filter) const
{
	// apply overrides
	for (int32 i = 0; i < Areas.Num(); i++)
	{
		const FNavigationFilterArea& AreaData = Areas[i];
		
		const int32 AreaId = NavData->GetAreaID(AreaData.AreaClass);
		if (AreaId == INDEX_NONE)
		{
			UE_LOG(LogNavigation, Warning, TEXT("Can't find area '%s' in '%s' for filter: %s"),
				*GetNameSafe(AreaData.AreaClass), *GetNameSafe(NavData), *GetName());

			continue;
		}

		if (AreaData.bIsExcluded)
		{
			Filter->SetExcludedArea(AreaId);
		}
		else
		{
			if (AreaData.bOverrideTravelCost)
			{
				Filter->SetAreaCost(AreaId, FMath::Max(1.0f, AreaData.TravelCostOverride));
			}

			if (AreaData.bOverrideEnteringCost)
			{
				Filter->SetFixedAreaEnteringCost(AreaId, FMath::Max(0.0f, AreaData.EnteringCostOverride));
			}
		}
	}

	// apply flags
	Filter->SetIncludeFlags(IncludeFlags.Packed);
	Filter->SetExcludeFlags(ExcludeFlags.Packed);
}

TSharedPtr<const struct FNavigationQueryFilter> UNavigationQueryFilter::GetQueryFilter(const ANavigationData* NavData, UClass* FilterClass)
{
	UNavigationQueryFilter* DefFilterOb = FilterClass ? FilterClass->GetDefaultObject<UNavigationQueryFilter>() : NULL;
	return NavData && DefFilterOb ? DefFilterOb->GetQueryFilter(NavData) : NULL;
}

void UNavigationQueryFilter::AddTravelCostOverride(TSubclassOf<class UNavArea> AreaClass, float TravelCost)
{
	int32 Idx = FindAreaOverride(AreaClass);
	if (Idx == INDEX_NONE)
	{
		FNavigationFilterArea AreaData;
		AreaData.AreaClass = AreaClass;

		Idx = Areas.Add(AreaData);
	}

	Areas[Idx].bOverrideTravelCost = true;
	Areas[Idx].TravelCostOverride = TravelCost;
}

void UNavigationQueryFilter::AddEnteringCostOverride(TSubclassOf<class UNavArea> AreaClass, float EnteringCost)
{
	int32 Idx = FindAreaOverride(AreaClass);
	if (Idx == INDEX_NONE)
	{
		FNavigationFilterArea AreaData;
		AreaData.AreaClass = AreaClass;

		Idx = Areas.Add(AreaData);
	}

	Areas[Idx].bOverrideEnteringCost = true;
	Areas[Idx].EnteringCostOverride = EnteringCost;
}

void UNavigationQueryFilter::AddExcludedArea(TSubclassOf<class UNavArea> AreaClass)
{
	int32 Idx = FindAreaOverride(AreaClass);
	if (Idx == INDEX_NONE)
	{
		FNavigationFilterArea AreaData;
		AreaData.AreaClass = AreaClass;

		Idx = Areas.Add(AreaData);
	}

	Areas[Idx].bIsExcluded = true;
}

int32 UNavigationQueryFilter::FindAreaOverride(TSubclassOf<class UNavArea> AreaClass) const
{
	for (int32 i = 0; i < Areas.Num(); i++)
	{
		if (Areas[i].AreaClass == AreaClass)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

#if WITH_EDITOR
void UNavigationQueryFilter::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// remove cached filter settings from existing NavigationSystems
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		UNavigationSystem* NavSys = UNavigationSystem::GetCurrent(Context.World());
		if (NavSys)
		{
			NavSys->ResetCachedFilter(GetClass());
		}
	}
}
#endif
