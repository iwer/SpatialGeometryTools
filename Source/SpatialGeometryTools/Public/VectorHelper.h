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

    static FVector MakeFaceNormal(const FVector &v1, const FVector &v2, const FVector &v3);
};
    
