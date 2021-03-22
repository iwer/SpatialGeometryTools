// Copyright (c) Iwer Petersen. All rights reserved.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
class SPATIALGEOMETRYTOOLS_API VectorHelper
{
public:
    VectorHelper();
    ~VectorHelper();

    static FVector MakeFaceNormal(const FVector &V0, const FVector &V1, const FVector &V2);
	static FVector CenterOfMass(const TArray<FVector> &Vertices);
    static bool IsLine(TArray<FVector> Vertices);
};
    
