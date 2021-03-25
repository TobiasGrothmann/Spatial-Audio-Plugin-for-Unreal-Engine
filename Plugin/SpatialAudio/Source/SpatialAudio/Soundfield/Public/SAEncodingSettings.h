#pragma once

#include "ISoundfieldFormat.h"
#include "SAEncodingSettings.generated.h"



class FSAEncodingSettingsProxy : public ISoundfieldEncodingSettingsProxy
{
public:
    
    virtual uint32 GetUniqueId() const override;
    
    virtual TUniquePtr<ISoundfieldEncodingSettingsProxy> Duplicate() const override;
    
private:
    
    int32 UniqueId = FGuid::NewGuid().A;
    
};



UCLASS()
class USAEncodingSettings : public USoundfieldEncodingSettingsBase
{
    GENERATED_BODY()
public:
    
    USAEncodingSettings();
    
    virtual TUniquePtr<ISoundfieldEncodingSettingsProxy> GetProxy() const override;
    
};

