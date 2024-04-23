#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Utilities/EnumerableClass.h>

#include <Foundation/Configuration/SubSystem.h>
/// NOTE: THIS IS NOT ALLOWED TO BE INCLUDED IN THE OPEN-SOURCE VERSION.
/// @brief Base class for implementing functionality for extra platforms (Consoles).
/// see nsSubSystem for more info.
/// @note Only one instance of this class should be created with the application. adding/creating more than once instance of the class at one time, could be a disaster.
class NS_FOUNDATION_DLL nsPlatformSubsystem : public nsSubSystem
{
    NS_DECLARE_ENUMERABLE_CLASS(nsPlatformSubsystem);
    NS_DISALLOW_COPY_AND_ASSIGN(nsPlatformSubsystem);

public:
    nsPlatformSubsystem() = default;

    virtual ~nsPlatformSubsystem() = default;

protected:
    /// @brief Function to initialize the internal console/platform plugin.
    /// @return nsResult of the initialization.
    virtual nsResult StartupInternalPlatform() { return NS_FAILURE; }

    /// @brief Function to shutdown the internal console/platform plugin.
    virtual void ShutdownInternalPlatform() {}

private:
    /// Stores which startup phase has been done already.
    bool m_bStartupDone[nsStartupStage::ENUM_COUNT];

    /// @brief Platform name of the said platform plugin. for example, if we are loading the Xbox Series Plugin, we would set this as: SCARLETT.
    nsStringView m_bPlatformName;
};

#include <Foundation/Configuration/StaticPlatformSubSystem.h>