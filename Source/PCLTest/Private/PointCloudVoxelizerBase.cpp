// Fill out your copyright notice in the Description page of Project Settings.


#include "PointCloudVoxelizerBase.h"
#include "VoxelImporters/VoxelMeshImporter.h"

#include "VoxelAssets/VoxelDataAsset.h"
#include "VoxelAssets/VoxelDataAssetData.inl"
#include "VoxelUtilities/VoxelMathUtilities.h"
#include "VoxelUtilities/VoxelExampleUtilities.h"
#include "VoxelUtilities/VoxelDistanceFieldUtilities.h"
#include "VoxelMessages.h"

//#include "SDFGen/makelevelset3.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetRenderingLibrary.h"

#include "Open3DUE5.h"

// Sets default values
APointCloudVoxelizerBase::APointCloudVoxelizerBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APointCloudVoxelizerBase::BeginPlay()
{
	Super::BeginPlay();
	FOpen3DUE5Module* Plugin = FModuleManager::GetModulePtr<FOpen3DUE5Module>("Open3DUE5");
	
	if (Plugin) Plugin->InitSensor();
}

bool APointCloudVoxelizerBase::Voxelize(
	UObject* WorldContextObject,
	TArray<FVector3f> Points,
	FTransform Transform,
	bool bSubtractive,
	FVoxelMeshImporterSettingsBase Settings,
	UVoxelDataAsset*& Asset)
{
	//VOXEL_FUNCTION_COUNTER();

	if (Points.IsEmpty())
	{
		FVoxelMessages::Error(FUNCTION_ERROR("Invalid Points Array"));
		return false;
	}

	if (bSubtractive)
	{
		Settings.DistanceDivisor *= -1;
	}

	FIntVector PositionOffset{ ForceInit };

	TArray<FIntVector> VoxelFilledArr = {};
	FIntVector RealSize = {};

	//FOpen3DUE5Module* Plugin = FModuleManager::GetModulePtr<FOpen3DUE5Module>("Open3DUE5");

	TArray<FVector3f> RealPoints = {};

	for (auto APoint : Points)
	{
		auto TransformedPoint = Transform.TransformPosition(FVector(APoint));

		RealPoints.Add(FVector3f(TransformedPoint));
	}

	FOpen3DUE5Module::VoxelizedArrFromPoints(RealPoints, (double)Settings.VoxelSize, VoxelFilledArr, RealSize);


	for (auto Voxel : VoxelFilledArr)
	{
		UE_LOG(LogClass, Warning, TEXT("Voxel at %s"), *Voxel.ToString())
	}

	UE_LOG(LogClass, Warning, TEXT("Size: %s"), *RealSize.ToString())

	const auto Data = MakeVoxelShared<FVoxelDataAssetData>();

	auto RealSettings = FVoxelMeshImporterSettings(Settings);

	FBox Box(ForceInit);
	for (auto& Point : Points)
	{
		const auto PointDouble = FVector(Point.X, Point.Y, Point.Z);
		//Vertices.Add(NewVertex);
		Box += PointDouble;
	}
	Box = Box.ExpandBy(Settings.VoxelSize);

	if (int64(RealSize.X) * int64(RealSize.Y) * int64(RealSize.Z) >= MAX_int32)
	{
		FVoxelMessages::Error(FUNCTION_ERROR("Converted assets would have more than 2B voxels! Abording"));
		return false;
	}
	if (RealSize.X * RealSize.Y * RealSize.Z == 0)
	{
		FVoxelMessages::Error(FUNCTION_ERROR("Size = 0! Abording"));
		return false;
	}

	auto OutOffset = FVoxelUtilities::RoundToInt(Box.Min / RealSettings.VoxelSize);

	(*Data).SetSize(FIntVector(RealSize.X, RealSize.Y, RealSize.Z), false);

	for (int32 X = 0; X < RealSize.X; X++)
	{
		for (int32 Y = 0; Y < RealSize.Y; Y++)
		{
			for (int32 Z = 0; Z < RealSize.Z; Z++)
			{
				(*Data).SetValue(X, Y, Z, FVoxelValue::Empty());
			}
		}
	}

	for (auto AVoxelLocation : VoxelFilledArr)
	{
		(*Data).SetValue(AVoxelLocation.X, AVoxelLocation.Y, AVoxelLocation.Z, FVoxelValue::Full());
	}

	Asset = NewObject<UVoxelDataAsset>(GetTransientPackage());
	Asset->bSubtractiveAsset = bSubtractive;
	Asset->PositionOffset = PositionOffset;
	Asset->SetData(Data);

	return true;
}

// Called every frame
void APointCloudVoxelizerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

