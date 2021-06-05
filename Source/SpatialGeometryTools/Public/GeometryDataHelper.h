// Copyright (c) Iwer Petersen. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GeometryData.h"

/**
 *
 */
class SPATIALGEOMETRYTOOLS_API GeometryDataHelper
{
public:
    GeometryDataHelper();
    ~GeometryDataHelper();

    static void AppendQuad(FGeometryData &data, const FVector &P0, const FVector &P1, const FVector &P0a, const FVector &P1a);
};
