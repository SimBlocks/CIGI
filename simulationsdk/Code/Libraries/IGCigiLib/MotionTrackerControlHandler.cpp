//Copyright SimBlocks LLC 2016-2026
#include "MotionTrackerControlHandler.h"
#include "CigiLib/CigiTypesHostToIG.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "IGCigiLib/CigiMessageLogger.h"
#include "IGCigiLib/IGCigiLib.h"

using namespace sbio::cigi::ig;
using namespace sbio::ig::sensor;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

void CCigiMotionTrackerControlHandler::Handle(const sbio::cigi::SMotionTrackerViewControl& motionTrackerViewControl)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(motionTrackerViewControl);
  }

  if (m_Trackers.find(motionTrackerViewControl.motionTrackerID) == m_Trackers.end())
  {
    SCreateMotionTrackerViewMessage createMotionTrackerViewMessage;
    createMotionTrackerViewMessage.TrackerID = motionTrackerViewControl.motionTrackerID;
    createMotionTrackerViewMessage.ViewID = motionTrackerViewControl.viewID;

    m_Trackers.insert(motionTrackerViewControl.motionTrackerID);

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendCreateMotionTrackerViewMessage(createMotionTrackerViewMessage);
    }
  }

  SSetMotionTrackerMessage data;
  data.BoresightEnabled = motionTrackerViewControl.bBoresightEnable;
  data.RollEnabled = motionTrackerViewControl.bRollEnable;
  data.TrackerEnabled = motionTrackerViewControl.bEnable;
  data.XEnabled = motionTrackerViewControl.bXEnable;
  data.YEnabled = motionTrackerViewControl.bYawEnable;
  data.ZEnabled = motionTrackerViewControl.bZEnable;

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetMotionTrackerMessage(data);
  }
}

void CCigiMotionTrackerControlHandler::Handle(const sbio::cigi::SMotionTrackerViewGroupControl& motionTrackerViewGroupControl)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(motionTrackerViewGroupControl);
  }

  if (m_Trackers.find(motionTrackerViewGroupControl.motionTrackerID) == m_Trackers.end())
  {
    SCreateMotionTrackerViewGroupMessage createMotionTrackerViewGroupMessage;
    createMotionTrackerViewGroupMessage.TrackerID = motionTrackerViewGroupControl.motionTrackerID;
    createMotionTrackerViewGroupMessage.ViewGroupID = motionTrackerViewGroupControl.viewGroupID;

    m_Trackers.insert(motionTrackerViewGroupControl.motionTrackerID);

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendCreateMotionTrackerViewGroupMessage(createMotionTrackerViewGroupMessage);
    }
  }

  SSetMotionTrackerMessage data;
  data.BoresightEnabled = motionTrackerViewGroupControl.bBoresightEnable;
  data.RollEnabled = motionTrackerViewGroupControl.bRollEnable;
  data.TrackerEnabled = motionTrackerViewGroupControl.bEnable;
  data.XEnabled = motionTrackerViewGroupControl.bXEnable;
  data.YEnabled = motionTrackerViewGroupControl.bYawEnable;
  data.ZEnabled = motionTrackerViewGroupControl.bZEnable;

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    g_CigiLibGlobals.pEventMessenger->SendSetMotionTrackerMessage(data);
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
