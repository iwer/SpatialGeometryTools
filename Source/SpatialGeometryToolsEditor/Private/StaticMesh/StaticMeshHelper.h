// Copyright (c) Iwer Petersen. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GeometryData.h"
#include "GeometryDataHelper.h"
#include "RawMesh.h"
#include "AssetRegistry/AssetRegistryModule.h"

/**
*
*/
class SPATIALGEOMETRYTOOLSEDITOR_API StaticMeshHelper
{
public:
    StaticMeshHelper();
    ~StaticMeshHelper();

    /**
    * Creates a static mesh asset named ObjectName at Content path AssetPath with default material
    */
    static UStaticMesh * CreateStaticMeshAsset(const FGeometryData &Geometry, FString ObjectName, FString AssetPath, UMaterialInterface * Material)
    {
        //UE_LOG(LogTemp, Warning, TEXT("GeometryDataHelper::CreateStaticMeshAsset: %d vertices, %d indices, %d normals, %d colors, %d tangents, %d tex coords"),
        //Geometry.Vertices.Num(), Geometry.Indices.Num(), Geometry.Normals.Num(), Geometry.Colors.Num(), Geometry.Tangents.Num(), Geometry.TexCoords.Num())
        if(!GeometryDataHelper::IsValid(Geometry))
            return nullptr;

        // Create Package
        FString PathPackage = FPaths::Combine(FString("/Game"), AssetPath);
        FString AbsolutePathPackage = FPaths::Combine(FPaths::ProjectContentDir(), AssetPath, FString("/"));

        UPackage * Package = CreatePackage(*PathPackage);
        const FName StaticMeshName = MakeUniqueObjectName(Package, UStaticMesh::StaticClass(), FName(*ObjectName));
        UPackage * MeshPackage = CreatePackage(*FPaths::Combine(PathPackage, StaticMeshName.ToString()));

        // Create Static Mesh
        UStaticMesh * StaticMesh = NewObject<UStaticMesh>(MeshPackage, StaticMeshName, RF_Public | RF_Standalone);

        if(StaticMesh)
        {
            FRawMesh RawMesh = FRawMesh();
            
            // add vertices
            for(auto &v : Geometry.Vertices)
            {
                RawMesh.VertexPositions.Add(FVector3f(v));
            }
            // add faces, normals, ...
            for (int i = 0; i < Geometry.Indices.Num() - 2; i+=3)
            {
                RawMesh.WedgeIndices.Add(Geometry.Indices[i+0]);
                RawMesh.WedgeIndices.Add(Geometry.Indices[i+1]);
                RawMesh.WedgeIndices.Add(Geometry.Indices[i+2]);

                RawMesh.WedgeColors.Add(Geometry.Colors[Geometry.Indices[i+0]].ToFColor(false));
                RawMesh.WedgeColors.Add(Geometry.Colors[Geometry.Indices[i+1]].ToFColor(false));
                RawMesh.WedgeColors.Add(Geometry.Colors[Geometry.Indices[i+2]].ToFColor(false));

                RawMesh.WedgeTangentX.Add(FVector3f(Geometry.Tangents[Geometry.Indices[i+0]].TangentX));
                RawMesh.WedgeTangentX.Add(FVector3f(Geometry.Tangents[Geometry.Indices[i+1]].TangentX));
                RawMesh.WedgeTangentX.Add(FVector3f(Geometry.Tangents[Geometry.Indices[i+2]].TangentX));

                RawMesh.WedgeTangentY.Add(FVector3f::ZeroVector);
                RawMesh.WedgeTangentY.Add(FVector3f::ZeroVector);
                RawMesh.WedgeTangentY.Add(FVector3f::ZeroVector);

                RawMesh.WedgeTangentZ.Add(FVector3f(Geometry.Normals[Geometry.Indices[i+0]]));
                RawMesh.WedgeTangentZ.Add(FVector3f(Geometry.Normals[Geometry.Indices[i+1]]));
                RawMesh.WedgeTangentZ.Add(FVector3f(Geometry.Normals[Geometry.Indices[i+2]]));

                RawMesh.FaceMaterialIndices.Add(0);
                RawMesh.FaceSmoothingMasks.Add(0xFFFFFFFF); // Phong

                for(int UVIndex = 0; UVIndex < MAX_MESH_TEXTURE_COORDS; UVIndex++)
                {
                    RawMesh.WedgeTexCoords[UVIndex].Add(FVector2f(Geometry.TexCoords[Geometry.Indices[i+0]]));
                    RawMesh.WedgeTexCoords[UVIndex].Add(FVector2f(Geometry.TexCoords[Geometry.Indices[i+1]]));
                    RawMesh.WedgeTexCoords[UVIndex].Add(FVector2f(Geometry.TexCoords[Geometry.Indices[i+2]]));
                }
            }

            // Save RawMesh in StaticMesh
            StaticMesh->AddSourceModel();
            StaticMesh->GetSourceModel(0).RawMeshBulkData->SaveRawMesh(RawMesh);

            // Configuration
            StaticMesh->GetSourceModel(0).BuildSettings.bRecomputeNormals = false;
            StaticMesh->GetSourceModel(0).BuildSettings.bRecomputeTangents = true;
            StaticMesh->GetSourceModel(0).BuildSettings.bUseMikkTSpace = false;
            StaticMesh->GetSourceModel(0).BuildSettings.bGenerateLightmapUVs = true;
            //StaticMesh->GetSourceModel(0).BuildSettings.bBuildAdjacencyBuffer = false;
            StaticMesh->GetSourceModel(0).BuildSettings.bBuildReversedIndexBuffer = false;
            StaticMesh->GetSourceModel(0).BuildSettings.bUseFullPrecisionUVs = false;
            StaticMesh->GetSourceModel(0).BuildSettings.bUseHighPrecisionTangentBasis = false;

            // Assign Material
            StaticMesh->GetStaticMaterials().Add(Material);
            StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;
            StaticMesh->CreateBodySetup();
            StaticMesh->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
            StaticMesh->SetLightingGuid();

            StaticMesh->Build();
            FAssetRegistryModule::AssetCreated(StaticMesh);

            StaticMesh->PostEditChange();
            if(!MeshPackage->MarkPackageDirty())
            {
                UE_LOG(LogTemp, Warning, TEXT("GeometryDataHelper: Editor suppressed marking package  %s dirty!"), *Package->GetName())
            } else
            {
                //UE_LOG( LogTemp, Log, TEXT("GeometryDataHelper: Created static mesh %s"), *ObjectName);
            }
            return StaticMesh;
        }
        return nullptr;
    }
};
