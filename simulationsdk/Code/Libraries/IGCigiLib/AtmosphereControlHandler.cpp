//Copyright SimBlocks LLC 2016-2026
#include "AtmosphereControlHandler.h"
#include "CigiLib/CigiTypes.h"
#include "EngineLib/EngineTypes.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "IGCigiLib/IGCigiLib.h"

using namespace sbio::engine;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::atmosphere;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

void CCigiAtmosphereControlHandler::Handle(bool bAtmosphereEnabled, const SAtmosphere& atmosphere)
{
  if (m_bAtmosphereEnabled != bAtmosphereEnabled)
  {
    SSetAtmosphereEnabledMessage data;
    data.Enabled = bAtmosphereEnabled;
    g_CigiLibGlobals.pEventMessenger->SendSetAtmosphereEnabledMessage(data);

    m_bAtmosphereEnabled = bAtmosphereEnabled;
  }

  if (!m_bAtmosphereEnabled)
  {
    return;
  }

  SSetAtmosphereMessage setAtmosphereMessage;
  setAtmosphereMessage.AirTemperature = atmosphere.fAirTemperature.Value();
  setAtmosphereMessage.BarometricPressure = atmosphere.fBarometricPressure;
  setAtmosphereMessage.HorizontalWindSpeed = atmosphere.fHorizontalWindSpeed;
  setAtmosphereMessage.Humidity = atmosphere.fHumidity;
  setAtmosphereMessage.VerticalWindSpeed = atmosphere.fVerticalWindSpeed;
  setAtmosphereMessage.VisibilityRange = atmosphere.fVisibilityRange;
  setAtmosphereMessage.WindDirection = atmosphere.fWindDirection;

  g_CigiLibGlobals.pEventMessenger->SendSetAtmosphereMessage(setAtmosphereMessage);
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
