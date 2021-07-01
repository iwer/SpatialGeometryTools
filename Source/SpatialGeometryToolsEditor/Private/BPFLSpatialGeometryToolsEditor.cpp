// Fill out your copyright notice in the Description page of Project Settings.


#include "BPFLSpatialGeometryToolsEditor.h"
#include "StaticMesh/StaticMeshHelper.h"

UStaticMesh *  UBPFLSpatialGeometryToolsEditor::SaveStaticMesh(FGeometryData& Geometry, FString ObjectName, FString AssetPath,
											UMaterialInterface* Material)
{
	return StaticMeshHelper::CreateStaticMeshAsset(Geometry, ObjectName, AssetPath, Material);
}