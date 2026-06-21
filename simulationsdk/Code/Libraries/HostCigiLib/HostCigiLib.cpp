//Copyright SimBlocks LLC 2016-2026
#include "HostCigiLib.h"
#include "HostCigiLibTypes.h"
#include "ScriptRuntime.h"
#include "UtilitiesLib/EventDispatcher.h"

using namespace std;
using namespace sbio::cigi;

sbio::cigi::host::SHostCigiLibGlobals g_HostCigiLibGlobals;

namespace sbio
{
  namespace cigi
  {
    namespace host
    {
      std::string ConvertToString(EHostSessionDatabaseState eHostSessionDatabaseState)
      {
        switch (eHostSessionDatabaseState)
        {
        case EHostSessionDatabaseState::NO_DATABASE:
        {
          return "NO_DATABASE";
        }
        case EHostSessionDatabaseState::LOAD_DATABASE_REQUESTED:
        {
          return "LOAD_DATABASE_REQUESTED";
        }
        case EHostSessionDatabaseState::LOADING_ACKNOWLEDGED:
        {
          return "LOADING_ACKNOWLEDGED";
        }
        case EHostSessionDatabaseState::LOADED:
        {
          return "LOADED";
        }
        case EHostSessionDatabaseState::IG_CONTROLLED:
        {
          return "IG_CONTROLLED";
        }
        }

        return "UNKNOWN";
      }

      CIGI::V40::EntityCtrl::EntityState ConvertToEntityState(EActiveState state)
      {
        if (state == EActiveState::ACTIVE)
        {
          return CIGI::V40::EntityCtrl::EntityState::eEntityState_Active;
        }
        else if (state == EActiveState::DESTROYED)
        {
          return CIGI::V40::EntityCtrl::EntityState::eEntityState_Destroyed;
        }
        else if (state == EActiveState::INACTIVE_STANDBY)
        {
          return CIGI::V40::EntityCtrl::EntityState::eEntityState_InactiveStandby;
        }
        return CIGI::V40::EntityCtrl::EntityState::eEntityState_Destroyed;
      }

      void InitHostCigiLib(const sbio::SGlobals& globals, CHost* pHost)
      {
        g_HostCigiLibGlobals.applicationsDataPath = globals.applicationsDataPath;
        g_HostCigiLibGlobals.librariesDataPath = globals.librariesDataPath;
        g_HostCigiLibGlobals.thirdPartyPath = globals.thirdPartyPath;
        g_HostCigiLibGlobals.currentApplicationDataPath = globals.currentApplicationDataPath;
        g_HostCigiLibGlobals.executablePath = globals.executablePath;
        g_HostCigiLibGlobals.pEventDispatcher = globals.pEventDispatcher;
        g_HostCigiLibGlobals.pLogger = globals.pLogger;
        g_HostCigiLibGlobals.pHost = pHost;

        if (g_HostCigiLibGlobals.pEventDispatcher != nullptr)
        {
          g_HostCigiLibGlobals.pEventDispatcher->RegisterEvent<HostCigiEvent>(make_unique<HostCigiEventHandler>());
        }
      }

      CIGI::V40::IGCtrl::IGMode ToIGMode(EIGMode eIGMode)
      {
        switch (eIGMode)
        {
        default:
        case EIGMode::UNKNOWN:
          return CIGI::V40::IGCtrl::IGMode::eIGMode_Reset;
        case EIGMode::DEBUG:
          return CIGI::V40::IGCtrl::IGMode::eIGMode_Debug;
        case EIGMode::OPERATE:
          return CIGI::V40::IGCtrl::IGMode::eIGMode_Operate;
        case EIGMode::RESET:
          return CIGI::V40::IGCtrl::IGMode::eIGMode_Reset;
        }
      }
    }
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
