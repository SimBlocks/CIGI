//Copyright SimBlocks LLC 2016-2026
#include "EarthReferenceModelHandler.h"
#include "CigiLib/CigiTypes.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "IGCigiLib/IGCigiLib.h"

using namespace sbio::cigi::ig;
using namespace sbio::ig::earth;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

void CCigiEarthReferenceModelHandler::Handle(const sbio::cigi::SCigiEarthReferenceModel& earthReferenceModel)
{
  SSetEarthReferenceModelMessage data;
  data.EquatorialRadius = earthReferenceModel.fEquatorialRadius;
  data.Flattening = earthReferenceModel.fFlattening;

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetEarthReferenceModelMessage(data);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
