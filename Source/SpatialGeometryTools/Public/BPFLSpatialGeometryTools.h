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
    static FGeometryData MakeFace(TArray<FVector> Vertices, bool bClockwise);

    UFUNCTION(BlueprintCallable, Category="SpatialGeometryTools|Face")
    static FGeometryData ExtrudeFaceAlongNormal(TArray<FVector> Vertices, float Distance);

    UFUNCTION(BlueprintCallable, Category="SpatialGeometryTools|Face")
    static void SortVerticesByAngle(UPARAM(ref) TArray<FVector> &Vertices, bool bClockwise);

};
