#include "../Public/SAEndpointSettings.h"


USAEndpointSettings::USAEndpointSettings()
    : Super()
{
}

TUniquePtr<ISoundfieldEndpointSettingsProxy> USAEndpointSettings::GetProxy() const
{
    return MakeUnique<FSAEndpointSettingsProxy>();
}
