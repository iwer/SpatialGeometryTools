// Copyright (c) Iwer Petersen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GeometryData.h"

#include "BPFLSpatialGeometryTools.generated.h"

/**
 * 
 */
UCLASS()
class SPATIALGEOMETRYTOOLS_API UBPFLSpatialGeometryTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="SpatialGeometryTools|Face")
	static FGeometryData MakeFace(TArray<FVector> vertices);

	
	
};
