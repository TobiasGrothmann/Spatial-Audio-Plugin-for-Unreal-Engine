#pragma once

#include "ISoundfieldEndpoint.h"
#include "SAEndpointSettings.generated.h"



class FSAEndpointSettingsProxy : public ISoundfieldEndpointSettingsProxy
{
};


UCLASS()
class USAEndpointSettings : public USoundfieldEndpointSettingsBase
{
    GENERATED_BODY()
public:

    USAEndpointSettings();

    virtual TUniquePtr<ISoundfieldEndpointSettingsProxy> GetProxy() const override;

};

