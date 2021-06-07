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

bool PolygonHelper::AngularSortVertices(TArray<FVector>& Vertices, bool bClockwise)
{
    FVector Com = VectorHelper::CenterOfMass(Vertices);
    UE_LOG(LogTemp, Warning, TEXT("PolygonHelper: Com %s"), *Com.ToString())
    bool bIsDefinite = true;
    Vertices.Sort([Com, &bIsDefinite, bClockwise](const FVector& V0, const FVector &V1)
    {
        const FVector LVec0 = V0 - Com;
        const FVector LVec1 = V1 - Com;

        const float Ang0 = FMath::Atan2(LVec0.Y, LVec0.X);
        const float Ang1 = FMath::Atan2(LVec1.Y, LVec1.X);

        UE_LOG(LogTemp, Warning, TEXT("PolygonHelper: Ang0 %f, Ang1 %f"), Ang0, Ang1)
        
        if(Ang0 == Ang1)
            bIsDefinite = false;
        if(bClockwise)
            return Ang0 > Ang1;
        else
            return Ang0 < Ang1;
    });
    
    return bIsDefinite;
}

bool PolygonHelper::IsFlat(const TArray<FVector>& Polygon)
{
    // discard n<3
    if(Polygon.Num() < 3)
        return false; // no definite plane with two points
    // return true for n==3
    if(Polygon.Num() == 3)
        return true; // all vertices in plane defined by first three vertices
    // return true when all vertices are in line
    if(VectorHelper::IsLine(Polygon))
        return true; // not strictly a polygon, but strictly flat

    // select three vertices that ar not in line 
    FVector P = Polygon[0];
    FVector Q = Polygon[1];
    int j = 2;
    while(j < Polygon.Num() && VectorHelper::IsLine({P,Q,Polygon[j]}))
        j++;
    FVector R = Polygon[j];
    
    // build plane parameters from first three vertices (ax+b+by+cz+d=0)
    FVector PlaneNormal = FVector::CrossProduct((Q - P),(R - P));
    float a = PlaneNormal.X;
    float b = PlaneNormal.Y;
    float c = PlaneNormal.Z;
    float d = -(a * P.X + b * P.Y + c * P.Z);
    
    // check if all vertices fir plane equation
    for(auto &V : Polygon)
    {
        if(a * V.X + b + V.Y + c * V.Z + d != 0)
            return false;
    }
    return true;
}

bool PolygonHelper::IsConvex(const TArray<FVector>& Polygon)
{
    // no polygon with less than three vertices
    if(Polygon.Num() < 3)
        return false;
    
    // determine if z-component of crossproduct of consecutive edges all have the same sign
    bool allPositive = false;
    for(int i = 0; i < Polygon.Num(); ++i)
    {
        float dx1 = Polygon[(i+1) % Polygon.Num()].X - Polygon[i].X;
        float dy1 = Polygon[(i+1) % Polygon.Num()].Y - Polygon[i].Y;
        float dx2 = Polygon[(i+2) % Polygon.Num()].X - Polygon[(i+1) % Polygon.Num()].X;
        float dy2 = Polygon[(i+2) % Polygon.Num()].Y - Polygon[(i+1) % Polygon.Num()].Y;

        float zcross = dx1 * dy2 - dy1 * dx2;

        if(i==0)
            allPositive = zcross > 0;
        else if ((zcross > 0) != allPositive)
        {
            // at least one corner is not convex
            return false;
        }
    }

    return true;
}

bool PolygonHelper::IsClockwise(const TArray<FVector>& Polygon)
{
    return PolygonHelper::PolygonArea(Polygon) < 0;
}

TArray<int32> PolygonHelper::TesselatePolygon(const TArray<FVector> &Vertices, bool bClockwise)
{
    TArray<FVector> Verts = Vertices;
   
    using Coord = float;
    using N = uint32_t;
    using FPoint = std::array<Coord,2>;

    std::vector<std::vector<FPoint>> Polygon;
    const std::vector<FPoint> Shape;
    Polygon.push_back(Shape);

    for(auto &Vertex : Verts) {
        FPoint p;
        p[0] = Vertex.X;
        p[1] = Vertex.Y;
        Polygon[0].push_back(p);
    }

    // tesselate using earcut
    std::vector<N> Indices = mapbox::earcut<N>(Polygon);

    TArray<int32> Ret;
    for(int i = 0; i < Indices.size() - 2; i += 3) {
        //if(!PolygonHelper::IsClockwise({Vertices[Indices[i]], Vertices[Indices[i+1]], Vertices[Indices[i+2]]}))
        if(bClockwise)
        {
            Ret.Add(Indices[i]);
            Ret.Add(Indices[i+1]);
            Ret.Add(Indices[i+2]);
        }
        else
        {
            Ret.Add(Indices[i+2]);
            Ret.Add(Indices[i+1]);
            Ret.Add(Indices[i]);
        }
        
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
    FVector Forward = (Vertices[1] - Vertices[0]).GetSafeNormal(.0001);
    FVector Right = FVector::CrossProduct(Normal, Forward);
    
    FVector UP;
    if(PolygonHelper::IsClockwise(Vertices)) {
        UP = FVector(0,0,-1);
    } else {
        UP = FVector(0,0,1);
    }
    // calculate deviation from x-y plane as rotator;
    const auto Rotator = FMatrix(Right, Forward, Normal, FVector::ZeroVector).Rotator();
    
    for(auto &v : Vertices) {
        // Un-Rotate vector into x-y plane and project in z-direction, scale to meter
        UV0.Add(FVector2D(Rotator.UnrotateVector(v)) / 100.0f);
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
        OffVert.Z -= HeightDifference;
        OffVertices.Add(OffVert);
    }

    return OffVertices;
}
