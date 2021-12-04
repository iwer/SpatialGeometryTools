// Copyright (c) Iwer Petersen. All rights reserved.

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FSpatialGeometryToolsEditorModule"

class FSpatialGeometryToolsEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
	}
	
	virtual void ShutdownModule() override
    {
	}	
};
IMPLEMENT_MODULE(FSpatialGeometryToolsEditorModule, SpatialGeometryToolsEditor);
#undef LOCTEXT_NAMESPACE
