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

TArray<int32> PolygonHelper::TesselatePolygon(const TArray<FVector> &vertices)
{
    using Coord = float;
    using N = uint32_t;
    using Point = std::array<Coord,2>;

    std::vector<std::vector<Point>> polygon;
    std::vector<Point> shape;
    polygon.push_back(shape);

    for(auto &vertex : vertices) {
        Point p;
        p[0] = vertex.X;
        p[1] = vertex.Y;
        polygon[0].push_back(p);
    }

    // tesselate using earcut
    std::vector<N> indices = mapbox::earcut<N>(polygon);

    TArray<int32> ret;
    for(auto &i : indices) {
        ret.Add(i);
    }
//    UE_LOG(LogTemp, Warning, TEXT("PolygonHelper: Tesselated %d vertices to %d triangles with %d indices"), vertices.Num(), indices.size()/3, ret.Num());
    return ret;
}

TArray<FVector2D> PolygonHelper::FlatUVMap(const TArray<FVector> &vertices)
{
    TArray<FVector2D> UV0;
    for (auto &v : vertices) {
        float X = v.X  / 100.0f;
        float Y = v.Y  / 100.0f;
        UV0.Add(FVector2D(X, Y));
    }
    return UV0;
}

TArray<FVector2D> PolygonHelper::FlatUVMapTilted(const TArray<FVector> &vertices)
{
    TArray<FVector2D> UV0;
    if(vertices.Num() < 3) {
        return UV0;
    }

    // get normal of polygon
    FVector normal = VectorHelper::MakeFaceNormal(vertices[0], vertices[1], vertices[2]);

    // calculate deviation from x-y plane as rotator;
    FVector up(0,0,1);
    auto rotator = up.Rotation() - normal.Rotation();
    for(auto &v : vertices) {
        // Rotate vector into x-y plane and project in z-direction, scale to meter
        UV0.Add(FVector2D(rotator.RotateVector(v)) / 100.0f);
    }
    return UV0;
}

TArray<FVector> PolygonHelper::GenerateOffsetVertices(const TArray<FVector> &vertices, float offset, float heightdifference)
{
    TArray<FVector> offVertices;

    FVector up(0,0,-1);

    int n = vertices.Num();
    for(int i = 0; i < n; ++i) {
        FVector p0 = vertices[i]; // current vertex
        FVector p1 = vertices[(i + 1) % n]; // next vertex
        FVector pn = vertices[(i + n - 1) % n]; // precious vertex

        // calculate normals of incoming and outgoing edge
        FVector na = FVector::CrossProduct(p0-p1, up);
        na.Normalize(.0001);
        FVector nb = FVector::CrossProduct(pn-p0, up);
        nb.Normalize(.0001);

        // calculate normalized bisect vector
        FVector bisector = na + nb;
        bisector.Normalize(.0001);

        // calculate length along bisector to meet offset
        float biLen = offset / FGenericPlatformMath::Sqrt(1 + FVector::DotProduct(na, nb));

        // calculate offset vertex
        auto offVert = p0 + biLen * bisector;
        // add heigth difference
        offVert.Z -= (2 * heightdifference);
        offVertices.Add(offVert);
    }

    return offVertices;
}
