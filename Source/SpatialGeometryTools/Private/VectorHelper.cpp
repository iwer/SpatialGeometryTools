// Copyright (c) Iwer Petersen. All rights reserved.
#include "VectorHelper.h"

#include "PolygonHelper.h"

FVectorHelper::FVectorHelper()
{}

FVectorHelper::~FVectorHelper()
{}

FVector FVectorHelper::MakeFaceNormal(const FVector &V0, const FVector &V1, const FVector &V2)
{
    if(V0.Equals(V1,.001) || V0.Equals(V2,.001) || V1.Equals(V2,.001)) {
        return FVector();
    }

    FVector A, B;
    if(PolygonHelper::IsClockwise({V0,V1,V2}))
    {
        A = V1 - V0;
        B = V2 - V0;
    } else
    {
        B = V1 - V0;
        A = V2 - V0;
    }
    
     
    
    return FVector::CrossProduct(A,B).GetSafeNormal(.0001);
}

FVector FVectorHelper::CenterOfMass(const TArray<FVector>& Vertices)
{
    FVector Ret = FVector::ZeroVector;
    for(auto &v : Vertices)
    {
        Ret += v;
    }
    Ret /= Vertices.Num();
    return Ret;
}

bool FVectorHelper::IsLine(TArray<FVector> Vertices)
{
    if(Vertices.Num() < 3)
        return true;

    const FVector Line = Vertices[1] - Vertices[0];
    for(int32 i = 2; i < Vertices.Num(); i++) {
        FVector NthLine = Vertices[i] - Vertices[0];
        if(FVector::CrossProduct(NthLine, Line).Size() >= .0001)
            return false;
    }
    return true;
}
