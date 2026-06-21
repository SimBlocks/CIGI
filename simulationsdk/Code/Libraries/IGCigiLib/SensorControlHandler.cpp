//Copyright SimBlocks LLC 2016-2026
#include "SensorControlHandler.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "IGCigiLib/IGCigiLib.h"

using namespace sbio::cigi::ig;
using namespace sbio::ig::sensor;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

void CCigiSensorControlHandler::Handle(const sbio::cigi::SCigiSensorControl& sensorControl)
{
  SUpdateSensorMessage data;
  data.bAutomaticGain = sensorControl.bAutomaticGain;
  data.bExtendedResponse = sensorControl.bExtendedResponse;
  data.bLineByLineDropoutEnabled = sensorControl.bLineByLineDropoutEnabled;
  data.bSensorOn = sensorControl.bSensorOn;
  data.ePolarity = sensorControl.ePolarity;
  data.eSensorTrack = sensorControl.eSensorTrack;
  data.eTrackMode = sensorControl.eTrackMode;
  data.ACCoupling = sensorControl.fACCoupling;
  data.Gain = sensorControl.gain;
  data.Level = sensorControl.level;
  data.Noise = sensorControl.noise;
  data.SensorID = sensorControl.sensorID;
  data.ViewID = sensorControl.viewID;

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendUpdateSensorMessage(data);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
