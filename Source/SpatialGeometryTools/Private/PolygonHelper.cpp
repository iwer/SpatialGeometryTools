// Copyright (c) Iwer Petersen. All rights reserved.


#include "PolygonHelper.h"
#include <array>
#include "earcut/earcut.hpp"
#include "VectorHelper.h"

PolygonHelper::PolygonHelper()
{
}

PolygonHelper::~PolygonHelper()
{
}

float PolygonHelper::PolygonArea(const TArray<FVector>& Polygon)
{
    float A = 0;
    const int32 N = Polygon.Num();

    for(int32 i = 0; i < N; ++i) {
        const auto P0 = Polygon[i];
        const auto P1 = Polygon[(i + 1) % N];

        A += (P1.X - P0.X) * (P1.Y + P0.Y);
    }

    return A;
}

bool PolygonHelper::AngularSortVertices(TArray<FVector>& Vertices)
{
    return true;
}

bool PolygonHelper::IsClockwise(const TArray<FVector>& Polygon)
{
    return PolygonHelper::PolygonArea(Polygon) < 0;
}

TArray<int32> PolygonHelper::TesselatePolygon(const TArray<FVector> &Vertices)
{
    TArray<FVector> Verts = Vertices;
    if(PolygonHelper::IsClockwise(Vertices))
    {
        Algo::Reverse(Verts);
    }
  
    
    using Coord = float;
    using N = uint32_t;
    using Point = std::array<Coord,2>;

    std::vector<std::vector<Point>> Polygon;
    const std::vector<Point> Shape;
    Polygon.push_back(Shape);

    for(auto &Vertex : Verts) {
        Point p;
        p[0] = Vertex.X;
        p[1] = Vertex.Y;
        Polygon[0].push_back(p);
    }

    // tesselate using earcut
    std::vector<N> Indices = mapbox::earcut<N>(Polygon);

    TArray<int32> Ret;
    for(auto &i : Indices) {
        Ret.Add(i);
    }
//    UE_LOG(LogTemp, Warning, TEXT("PolygonHelper: Tesselated %d vertices to %d triangles with %d indices"), vertices.Num(), indices.size()/3, ret.Num());
    return Ret;
}

TArray<FVector2D> PolygonHelper::FlatUVMap(const TArray<FVector> &Vertices)
{
    TArray<FVector2D> UV0;
    for (auto &v : Vertices) {
        const float X = v.X  / 100.0f;
        const float Y = v.Y  / 100.0f;
        UV0.Add(FVector2D(X, Y));
    }
    return UV0;
}

TArray<FVector2D> PolygonHelper::FlatUVMapTilted(const TArray<FVector> &Vertices)
{
    TArray<FVector2D> UV0;
    if(Vertices.Num() < 3) {
        return UV0;
    }

    // get normal of polygon
    const FVector Normal = VectorHelper::MakeFaceNormal(Vertices[0], Vertices[1], Vertices[2]);

    FVector UP;
    if(PolygonHelper::IsClockwise(Vertices)) {
        UP = FVector(0,0,-1);
    } else {
        UP = FVector(0,0,1);
    }

    // calculate deviation from x-y plane as rotator;
    const auto Rotator = UP.Rotation() - Normal.Rotation();
    for(auto &v : Vertices) {
        // Rotate vector into x-y plane and project in z-direction, scale to meter
        UV0.Add(FVector2D(Rotator.RotateVector(v)) / 100.0f);
    }
    return UV0;
}

TArray<FVector> PolygonHelper::GenerateOffsetVertices(const TArray<FVector> &Vertices, float Offset, float HeightDifference)
{
    TArray<FVector> OffVertices;

    FVector UP;
    if(PolygonHelper::IsClockwise(Vertices)) {
        UP = FVector(0,0,-1);
    } else {
        UP = FVector(0,0,1);
    }


    const int n = Vertices.Num();
    for(int i = 0; i < n; ++i) {
        FVector P0 = Vertices[i]; // current vertex
        FVector P1 = Vertices[(i + 1) % n]; // next vertex
        FVector PN = Vertices[(i + n - 1) % n]; // previous vertex

        // calculate normals of incoming and outgoing edge
        FVector Na = FVector::CrossProduct(P0-P1, UP);
        Na.Normalize(.0001);
        FVector Nb = FVector::CrossProduct(PN-P0, UP);
        Nb.Normalize(.0001);

        // calculate normalized bisect vector
        FVector Bisector = Na + Nb;
        Bisector.Normalize(.0001);

        // calculate length along bisector to meet offset
        const float BiLen = Offset / FGenericPlatformMath::Sqrt(1 + FVector::DotProduct(Na, Nb));

        // calculate offset vertex
        auto OffVert = P0 + BiLen * Bisector;
        // add height difference
        OffVert.Z -= (2 * HeightDifference);
        OffVertices.Add(OffVert);
    }

    return OffVertices;
}
