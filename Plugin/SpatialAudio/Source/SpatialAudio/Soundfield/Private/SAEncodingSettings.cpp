#include "../Public/SAEncodingSettings.h"




uint32 FSAEncodingSettingsProxy::GetUniqueId() const
{
    return UniqueId;
}

TUniquePtr<ISoundfieldEncodingSettingsProxy> FSAEncodingSettingsProxy::Duplicate() const
{
    // no need to copy anything, nothing in settings
    return TUniquePtr<ISoundfieldEncodingSettingsProxy>(new FSAEncodingSettingsProxy());
}






USAEncodingSettings::USAEncodingSettings()
    : Super()
{
}

TUniquePtr<ISoundfieldEncodingSettingsProxy> USAEncodingSettings::GetProxy() const
{
    return TUniquePtr<ISoundfieldEncodingSettingsProxy>(new FSAEncodingSettingsProxy());
}
