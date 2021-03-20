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

bool VectorHelper::IsLine(TArray<FVector> vertices)
{
    if(vertices.Num() < 3)
        return true;

    FVector line = vertices[1] - vertices[0];
    for(int32 i = 2; i < vertices.Num(); i++) {
        FVector iLine = vertices[i] - vertices[0];
        if(FVector::CrossProduct(iLine, line).Size() >= .0001)
            return false;
    }
    return true;
}