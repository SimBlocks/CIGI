//Copyright SimBlocks LLC 2016-2026
#include "CelestialSphereHandler.h"
#include "CigiLib/CigiTypes.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "IGCigiLib/IGCigiLib.h"

using namespace sbio;
using namespace sbio::engine;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::celestial;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

void CCigiCelestialSphereHandler::Handle(const SCelestialSphere& celestialSphere)
{
  SUpdateCelestialSphereMessage data;
  data.MoonEnabled = celestialSphere.bMoonEnabled;
  data.StarFieldEnabled = celestialSphere.bStarsEnabled;
  data.StarfieldIntensity = celestialSphere.fStarIntensity;
  data.SunEnabled = celestialSphere.bSunEnabled;
  data.EphemerisEnabled = celestialSphere.bEphemerisEnabled;

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendUpdateCelestialSphereMessage(data);
  }
}

void CCigiCelestialSphereHandler::SetDateTime(const SDate& date, const STime& time)
{
  SUpdateDateTimeMessage data;
  data.Date = date;
  data.Time = time;

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendUpdateDateTimeMessage(data);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
