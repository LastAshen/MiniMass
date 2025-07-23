// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassCommonFragments.h"
#include "MassEntityElementTypes.h"
#include "MassEntityTraitBase.h"
#include "MassProcessor.h"
#include "MiniMassDefine.generated.h"

USTRUCT()
struct FMiniMassColorFragment : public FMassFragment
{
	GENERATED_BODY()
	FLinearColor Color;
};

USTRUCT()
struct FMiniMassTag : public FMassTag
{
	GENERATED_BODY()
};

USTRUCT()
struct FMiniMassStaticMeshComponentFragment : public FObjectWrapperFragment
{
	GENERATED_BODY()
	TWeakObjectPtr<UStaticMeshComponent> StaticMeshComponent;
};

UCLASS()
class UMiniMassProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
public:
	UMiniMassProcessor();

protected:
	virtual  void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
	FMassEntityQuery EntityQuery;
};

UCLASS()
class MINIMASS_API UMiniMassTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()
public:
	virtual  void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};













