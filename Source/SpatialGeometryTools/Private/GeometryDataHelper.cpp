// Copyright (c) Iwer Petersen. All rights reserved.


#include "GeometryDataHelper.h"
#include "PolygonHelper.h"
#include "VectorHelper.h"


GeometryDataHelper::GeometryDataHelper()
{
}

GeometryDataHelper::~GeometryDataHelper()
{
}

void GeometryDataHelper::AppendQuad(FGeometryData &data, const FVector &P0, const FVector &P1, const FVector &P0a, const FVector &P1a)
{
    int32 StartIdx = data.Vertices.Num();

    // Append vertices
    data.Vertices.Add(P0);
    data.Vertices.Add(P1);
    data.Vertices.Add(P0a);
    data.Vertices.Add(P1a);

    // triangle p0, p1, P1a
    data.Indices.Append({StartIdx, StartIdx + 1, StartIdx + 3});
    // triangle p1a, p0a, p1
    data.Indices.Append({StartIdx + 3, StartIdx + 2, StartIdx});

    FVector FaceNormal = VectorHelper::MakeFaceNormal(P0, P0a, P1);
    FVector Tangent = (P1 - P0).GetSafeNormal(.0001);
    FVector Cotangent = FVector::CrossProduct(Tangent, FaceNormal);
    
    TArray<FVector> Normals;
    TArray<FLinearColor> Colors;
    TArray<FVector> Tangents;

    Normals.Init(FaceNormal, 4);
    data.Normals.Append(Normals);

    Tangents.Init(Tangent, 4);
    data.Tangents.Append(Tangents);

    Colors.Init(FLinearColor(0, 0, 0, 1), 4);
    data.Colors.Append(Colors);

    FVector UP(0,0,-1);
    //auto Rotator = UP.Rotation() - FaceNormal.Rotation();
    auto Rotator = FMatrix(Cotangent, Tangent, FaceNormal, FVector::ZeroVector).Rotator();
    FVector uv0 = Rotator.UnrotateVector(P0) / 100;
    FVector uv1 = Rotator.UnrotateVector(P1) / 100;
    FVector uv0a = Rotator.UnrotateVector(P0a) / 100;
    FVector uv1a = Rotator.UnrotateVector(P1a) / 100;

    data.TexCoords.Add(FVector2D(uv0));
    data.TexCoords.Add(FVector2D(uv1));
    data.TexCoords.Add(FVector2D(uv0a));
    data.TexCoords.Add(FVector2D(uv1a));
}
