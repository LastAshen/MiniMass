// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniMassDefine.h"

#include "MassEntityTemplateRegistry.h"
#include "MassEntityView.h"
#include "MassExecutionContext.h"

namespace FMiniMassTraitsHelper 
{
	template<typename T>
	T* AsComponent(UObject& Owner)
	{
		T* Component = nullptr;
		if (AActor* AsActor = Cast<AActor>(&Owner))
		{
			Component = AsActor->FindComponentByClass<T>();
		}
		else
		{
			Component = Cast<T>(&Owner);
		}

		UE_CVLOG_UELOG(Component == nullptr, &Owner, LogMass, Error, TEXT("Trying to extract %s from %s failed")
			, *T::StaticClass()->GetName(), *Owner.GetName());

		return Component;
	}
}

/////////////////////////////////////////Processor//////////

UMiniMassProcessor::UMiniMassProcessor() : EntityQuery(*this)
{
	ExecutionFlags = (int32)EProcessorExecutionFlags::AllNetModes;
	bRequiresGameThreadExecution = true; //这里必须在主线程运行，因为需要修改StaticMeshComponent的Transform
}

void UMiniMassProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.AddRequirement<FMiniMassColorFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMiniMassStaticMeshComponentFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddTagRequirement<FMiniMassTag>(EMassFragmentPresence::All);
}

void UMiniMassProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
	{
		auto ColorList = Context.GetMutableFragmentView<FMiniMassColorFragment>();
		auto MeshCompList = Context.GetMutableFragmentView<FMiniMassStaticMeshComponentFragment>();
		for(int32 i = 0; i < Context.GetNumEntities(); i++)
		{
			if(auto MeshComp = MeshCompList[i].StaticMeshComponent.Get())
			{
				auto ValueR= FMath::Sin(GetWorld()->GetTimeSeconds() + FMath::Sin((MeshComp -> GetComponentLocation() . Y / 1000 * PI)));
				auto ValueG = (static_cast<int>(MeshComp -> GetComponentLocation() . X) % 3000) * 0.001f / 3;
				auto ValueB = FMath::Sin(GetWorld()->GetTimeSeconds() + FMath::Sin((MeshComp -> GetComponentLocation() . Z / 1000 * PI)));
				FLinearColor NewColor(ValueR, ValueG, ValueB, 1.0f);
				ColorList[i].Color = NewColor;
				
				if(auto Material = MeshComp->GetMaterial(0))
				{
					auto DynamicMaterialInstance = Cast<UMaterialInstanceDynamic>(Material);
					if(!DynamicMaterialInstance)
					{
						DynamicMaterialInstance = UMaterialInstanceDynamic::Create(Material, nullptr);
						MeshComp->SetMaterial(0, DynamicMaterialInstance);
					}
					DynamicMaterialInstance->SetVectorParameterValue("Color", NewColor);
				}

				auto ZMove = FVector(0.0f, 0.0f,  ValueR * 10.f);
				MeshComp->AddLocalOffset(ZMove);
			}
		}
	});
}

/////////////////////////////////////////Trait//////////

void UMiniMassTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FMiniMassColorFragment>();
	BuildContext.AddFragment<FMiniMassStaticMeshComponentFragment>();
	BuildContext.AddTag<FMiniMassTag>();
	BuildContext.GetMutableObjectFragmentInitializers().Add([](UObject& Owner, FMassEntityView& EntityView, const EMassTranslationDirection CurrentDirection)
	{
		if(auto* Component = FMiniMassTraitsHelper::AsComponent<UStaticMeshComponent>(Owner))
		{
			auto& StaticMeshFragment = EntityView.GetFragmentData<FMiniMassStaticMeshComponentFragment>();
			StaticMeshFragment.StaticMeshComponent = Component;
		}
		else
		{
			UE_LOG(LogMass, Error, TEXT("Trying to extract UStaticMeshComponent from %s failed"), *Owner.GetName());
		}
	});
}





















