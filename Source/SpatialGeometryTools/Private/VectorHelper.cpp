// Copyright (c) Iwer Petersen. All rights reserved.
#include "VectorHelper.h"

VectorHelper::VectorHelper()
{}

VectorHelper::~VectorHelper()
{}

FVector VectorHelper::MakeFaceNormal(const FVector &v0, const FVector &v1, const FVector &v2)
{
    if(v0.Equals(v1,.001) || v0.Equals(v2,.001) || v1.Equals(v2,.001)) {
        return FVector();
    }
    FVector a = v0 - v1;
    FVector b = v0 - v2;
    return FVector::CrossProduct(a,b).GetSafeNormal(.0001);
}
