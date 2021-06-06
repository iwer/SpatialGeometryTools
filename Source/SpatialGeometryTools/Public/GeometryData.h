// Copyright (c) Iwer Petersen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"

#include "GeometryData.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FGeometryData {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FVector> Vertices;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> Indices;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FVector> Normals;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FProcMeshTangent> Tangents;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FVector2D> TexCoords;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FLinearColor> Colors;

    FGeometryData(){};
};
