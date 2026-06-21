//Copyright SimBlocks LLC 2016-2026
#include "ComponentControlHandler.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "IGCigiLib/CigiEntity.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "EntityLib/EntityManager.h"
#include "IGCigiLib/IGCigiLib.h"
#include "UtilitiesLib/Logger.h"
#include "libCIGI/Packets/4_0/ComponentCtrl.h"
#include "ViewLib/ViewGroup.h"
#include <cstring>

using namespace std;
using namespace sbio;
using namespace sbio::symbol;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::entity;
using namespace sbio::ig::sensor;
using namespace sbio::ig::atmosphere;
using namespace sbio::ig::celestial;
using namespace sbio::ig::system;
using namespace sbio::ig::ocean;
using namespace sbio::ig::terrain;
using namespace sbio::ig::symbol;
using namespace sbio::ig::view;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiComponentControlHandler::CCigiComponentControlHandler()
{
  m_ComponentHandlerFunctions[CigiComponentClassID(CIGI::V40::ComponentCtrl::eComponentClass_Entity)] = &CCigiComponentControlHandler::HandleEntity;
  m_ComponentHandlerFunctions[CigiComponentClassID(CIGI::V40::ComponentCtrl::eComponentClass_View)] = &CCigiComponentControlHandler::HandleView;
  m_ComponentHandlerFunctions[CigiComponentClassID(CIGI::V40::ComponentCtrl::eComponentClass_ViewGroup)] = &CCigiComponentControlHandler::HandleViewGroup;
  m_ComponentHandlerFunctions[CigiComponentClassID(CIGI::V40::ComponentCtrl::eComponentClass_Sensor)] = &CCigiComponentControlHandler::HandleSensor;
  m_ComponentHandlerFunctions[CigiComponentClassID(CIGI::V40::ComponentCtrl::eComponentClass_RegionalSeaSurface)] = &CCigiComponentControlHandler::HandleRegionalSeaSurface;
  m_ComponentHandlerFunctions[CigiComponentClassID(CIGI::V40::ComponentCtrl::eComponentClass_RegionalTerrainSurface)] = &CCigiComponentControlHandler::HandleRegionalTerrainSurface;
  m_ComponentHandlerFunctions[CigiComponentClassID(CIGI::V40::ComponentCtrl::eComponentClass_RegionalLayeredWeather)] = &CCigiComponentControlHandler::HandleRegionalLayeredWeather;
  m_ComponentHandlerFunctions[CigiComponentClassID(CIGI::V40::ComponentCtrl::eComponentClass_GlobalSeaSurface)] = &CCigiComponentControlHandler::HandleGlobalSeaSurface;
  m_ComponentHandlerFunctions[CigiComponentClassID(CIGI::V40::ComponentCtrl::eComponentClass_GlobalTerrainSurface)] = &CCigiComponentControlHandler::HandleGlobalTerrainSurface;
  m_ComponentHandlerFunctions[CigiComponentClassID(CIGI::V40::ComponentCtrl::eComponentClass_GlobalLayeredWeather)] = &CCigiComponentControlHandler::HandleGlobalLayeredWeather;
  m_ComponentHandlerFunctions[CigiComponentClassID(CIGI::V40::ComponentCtrl::eComponentClass_Atmosphere)] = &CCigiComponentControlHandler::HandleAtmosphere;
  m_ComponentHandlerFunctions[CigiComponentClassID(CIGI::V40::ComponentCtrl::eComponentClass_CelestialSphere)] = &CCigiComponentControlHandler::HandleCelestialSphere;
  m_ComponentHandlerFunctions[CigiComponentClassID(CIGI::V40::ComponentCtrl::eComponentClass_Event)] = &CCigiComponentControlHandler::HandleEvent;
  m_ComponentHandlerFunctions[CigiComponentClassID(CIGI::V40::ComponentCtrl::eComponentClass_System)] = &CCigiComponentControlHandler::HandleSystem;
  m_ComponentHandlerFunctions[CigiComponentClassID(CIGI::V40::ComponentCtrl::eComponentClass_SymbolSurface)] = &CCigiComponentControlHandler::HandleSymbolSurface;
  m_ComponentHandlerFunctions[CigiComponentClassID(CIGI::V40::ComponentCtrl::eComponentClass_Symbol)] = &CCigiComponentControlHandler::HandleSymbol;
}

CCigiComponentControlHandler::~CCigiComponentControlHandler()
{
}

void CCigiComponentControlHandler::Handle(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  // find the handler function based on the component class ID
  TComponentHandlerFunctions::iterator itFunction = m_ComponentHandlerFunctions.find(componentControl.key.componentClassID);

  // if component class not found
  if (itFunction == m_ComponentHandlerFunctions.end())
  {
    stringstream ss;
    ss << "Component Class " << componentControl.key.componentClassID << " not found." << std::endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  // lookup cached component state based on component key
  TComponentStates::iterator itComponentState = m_ComponentStates.find(componentControl.key);

  // if the component state is not found
  if (itComponentState == m_ComponentStates.end())
  {
    // add new state
    m_ComponentStates[componentControl.key] = componentControl.state;
  }
  else
  {
    // if the state is the same, ignore the update
    if (itComponentState->second == componentControl.state)
    {
      return;
    }

    // update the stored state
    itComponentState->second = componentControl.state;
  }

  // call the component handler function
  CCigiComponentControlHandler::TComponentHandlerFunction pFunction = itFunction->second;
  (this->*pFunction)(componentControl, pComponentDataParser);
}

sbio::ig::SComponentData ConvertToComponentData(const uint32_t data[6])
{
  sbio::ig::SComponentData componentData;
  componentData.ComponentData0 = data[0];
  componentData.ComponentData1 = data[1];
  componentData.ComponentData2 = data[2];
  componentData.ComponentData3 = data[3];
  componentData.ComponentData4 = data[4];
  componentData.ComponentData5 = data[5];

  return componentData;
}

void CCigiComponentControlHandler::HandleAtmosphere(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  AtmosphereID atmosphereID = AtmosphereID(componentControl.key.nInstanceID);

  SSetAtmosphereComponentStateMessage data;
  data.AtmosphereID = atmosphereID;
  data.ComponentID = componentControl.key.componentID;
  data.ComponentState = componentControl.state.nComponentState;
  data.ComponentData = ConvertToComponentData(componentControl.state.componentData);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetAtmosphereComponentStateMessage(data);
  }
}

void CCigiComponentControlHandler::HandleCelestialSphere(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  CelestialSphereID celestialSphereID = CelestialSphereID(componentControl.key.nInstanceID);

  SSetCelestialSphereComponentStateMessage data;
  data.CelestialSphereID = celestialSphereID;
  data.ComponentID = componentControl.key.componentID;
  data.ComponentState = componentControl.state.nComponentState;
  data.ComponentData = ConvertToComponentData(componentControl.state.componentData);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetCelestialSphereComponentStateMessage(data);
  }
}

void CCigiComponentControlHandler::HandleEntity(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  EntityID entityID = EntityID(componentControl.key.nInstanceID);
  // verify entity actually exists
  if (!g_CigiLibGlobals.pEntityManager->HasEntity(entityID))
  {
    stringstream ss;
    ss << "Entity ID " << entityID << " does not exist for component " << componentControl.key.componentID << endl;
    g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    return;
  }

  CCigiEntity* pCigiEntity = dynamic_cast<CCigiEntity*>(g_CigiLibGlobals.pEntityManager->GetEntity(entityID));
  pCigiEntity->SetEntityComponent(componentControl.key, componentControl.state);
}

void CCigiComponentControlHandler::HandleEvent(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  ComponentEventID componentEventID = ComponentEventID(componentControl.key.nInstanceID);

  SSetEventComponentStateMessage data;
  data.EventID = componentEventID;
  data.ComponentID = componentControl.key.componentID;
  data.ComponentState = componentControl.state.nComponentState;
  data.ComponentData = ConvertToComponentData(componentControl.state.componentData);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetEventComponentStateMessage(data);
  }
}

void CCigiComponentControlHandler::HandleGlobalLayeredWeather(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  GlobalLayeredWeatherID globalLayeredWeatherID = GlobalLayeredWeatherID(static_cast<uint8_t>(componentControl.key.nInstanceID));

  SSetGlobalLayeredWeatherComponentStateMessage data;
  data.LayeredWeatherID = globalLayeredWeatherID;
  data.ComponentID = componentControl.key.componentID;
  data.ComponentState = componentControl.state.nComponentState;
  data.ComponentData = ConvertToComponentData(componentControl.state.componentData);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetGlobalLayeredWeatherComponentStateMessage(data);
  }
}

void CCigiComponentControlHandler::HandleGlobalSeaSurface(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  GlobalSeaSurfaceID globalSeaSurfaceID = GlobalSeaSurfaceID(componentControl.key.nInstanceID);

  SSetGlobalMaritimeComponentStateMessage data;
  data.MaritimeID = globalSeaSurfaceID;
  data.ComponentID = componentControl.key.componentID;
  data.ComponentState = componentControl.state.nComponentState;
  data.ComponentData = ConvertToComponentData(componentControl.state.componentData);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetGlobalMaritimeComponentStateMessage(data);
  }
}

void CCigiComponentControlHandler::HandleGlobalTerrainSurface(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  GlobalTerrainSurfaceID globalTerrainSurfaceID = GlobalTerrainSurfaceID(componentControl.key.nInstanceID);

  SSetGlobalTerrainComponentStateMessage data;
  data.GlobalTerrainSurfaceID = globalTerrainSurfaceID;
  data.ComponentID = componentControl.key.componentID;
  data.ComponentState = componentControl.state.nComponentState;
  data.ComponentData = ConvertToComponentData(componentControl.state.componentData);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetGlobalTerrainComponentStateMessage(data);
  }
}

void CCigiComponentControlHandler::HandleNoComponent(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  //no component class
}

void CCigiComponentControlHandler::HandleRegionalLayeredWeather(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  RegionalLayeredWeatherID regionalLayeredWeatherID = RegionalLayeredWeatherID(static_cast<uint8_t>(componentControl.key.nInstanceID));

  SSetRegionalLayeredWeatherComponentStateMessage data;
  data.LayeredWeatherID = regionalLayeredWeatherID;
  data.ComponentID = componentControl.key.componentID;
  data.ComponentState = componentControl.state.nComponentState;
  data.ComponentData = ConvertToComponentData(componentControl.state.componentData);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetRegionalLayeredWeatherComponentStateMessage(data);
  }
}

void CCigiComponentControlHandler::HandleRegionalSeaSurface(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  RegionID regionalID = RegionID(componentControl.key.nInstanceID);

  SSetRegionMaritimeComponentStateMessage data;
  data.RegionID = regionalID;
  data.ComponentID = componentControl.key.componentID;
  data.ComponentState = componentControl.state.nComponentState;
  data.ComponentData = ConvertToComponentData(componentControl.state.componentData);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetRegionMaritimeComponentStateMessage(data);
  }
}

void CCigiComponentControlHandler::HandleRegionalTerrainSurface(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  RegionalTerrainSurfaceID regionalTerrainSurfaceID = RegionalTerrainSurfaceID(componentControl.key.nInstanceID);

  SSetRegionalTerrainSurfaceComponentStateMessage data;
  data.RegionalTerrainSurfaceID = regionalTerrainSurfaceID;
  data.ComponentID = componentControl.key.componentID;
  data.ComponentState = componentControl.state.nComponentState;
  data.ComponentData = ConvertToComponentData(componentControl.state.componentData);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetRegionalTerrainSurfaceComponentStateMessage(data);
  }
}

void CCigiComponentControlHandler::HandleSensor(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  //sensor ID is a uint8_t according to CIGI standard
  SensorID sensorID = SensorID(static_cast<uint8_t>(componentControl.key.nInstanceID));

  SUpdateSensorComponentMessage data;
  data.SensorID = sensorID;
  data.ComponentID = componentControl.key.componentID;
  data.ComponentState = componentControl.state.nComponentState;
  data.ComponentData = ConvertToComponentData(componentControl.state.componentData);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendUpdateSensorComponentMessage(data);
  }
}

void CCigiComponentControlHandler::HandleSymbol(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  SymbolID symbolID = SymbolID(componentControl.key.nInstanceID);

  SSetSymbolComponentStateMessage data;
  data.SymbolID = symbolID;
  data.ComponentID = componentControl.key.componentID;
  data.ComponentState = componentControl.state.nComponentState;
  data.ComponentData = ConvertToComponentData(componentControl.state.componentData);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetSymbolComponentStateMessage(data);
  }
}

void CCigiComponentControlHandler::HandleSymbolSurface(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  SymbolSurfaceID symbolSurfaceID = SymbolSurfaceID(componentControl.key.nInstanceID);

  SSetSymbolSurfaceComponentStateMessage data;
  data.SymbolSurfaceID = symbolSurfaceID;
  data.ComponentID = componentControl.key.componentID;
  data.ComponentState = componentControl.state.nComponentState;
  data.ComponentData = ConvertToComponentData(componentControl.state.componentData);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetSymbolSurfaceComponentStateMessage(data);
  }
}

void CCigiComponentControlHandler::HandleSystem(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  SystemID systemID = SystemID(componentControl.key.nInstanceID);

  SSetSystemComponentStateMessage data;
  data.SystemID = systemID;
  data.ComponentID = componentControl.key.componentID;
  data.ComponentState = componentControl.state.nComponentState;
  data.ComponentData = ConvertToComponentData(componentControl.state.componentData);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetSystemComponentStateMessage(data);
  }
}

void CCigiComponentControlHandler::HandleView(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  ViewID viewID = ViewID(componentControl.key.nInstanceID);

  SSetViewComponentStateMessage data;
  data.ViewID = viewID;
  data.ComponentID = componentControl.key.componentID;
  data.ComponentState = componentControl.state.nComponentState;
  data.ComponentData = ConvertToComponentData(componentControl.state.componentData);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetViewComponentStateMessage(data);
  }
}

void CCigiComponentControlHandler::HandleViewGroup(const SCigiComponentControl& componentControl, CBaseComponentDataParser* pComponentDataParser)
{
  ViewGroupID viewGroupID = ViewGroupID(static_cast<uint8_t>(componentControl.key.nInstanceID));

  SSetViewGroupComponentStateMessage data;
  data.ViewGroupID = viewGroupID;
  data.ComponentID = componentControl.key.componentID;
  data.ComponentState = componentControl.state.nComponentState;
  data.ComponentData = ConvertToComponentData(componentControl.state.componentData);

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetViewGroupComponentStateMessage(data);
  }
}

void CCigiComponentControlHandler::Reset()
{
  m_ComponentStates.clear();
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
