//Copyright SimBlocks LLC 2016-2026
#include "HostViewManager.h"
#include "HostCigiLibTypes.h"
#include "ViewLib/IViewCreator.h"
#include "ViewLib/View.h"
#include "ViewLib/ViewLib.h"
#include "UtilitiesLib/Logger.h"
#include "UtilitiesLib/UtilitiesLib.h"
#include "Poco/Foundation.h"
#include "Poco/JSON/Parser.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Array.h"
#include <fstream>
#include <sstream>

using namespace sbio::view;
using namespace sbio::cigi::host;

using namespace std;
using namespace Poco::JSON;

extern sbio::cigi::host::SHostCigiLibGlobals g_HostCigiLibGlobals;

namespace
{
  void LogHostViewManagerError(const std::string& message)
  {
    if (g_HostCigiLibGlobals.pLogger != nullptr)
    {
      g_HostCigiLibGlobals.pLogger->LogError(message);
    }
  }
}

void CHostViewManager::Init(const std::filesystem::path& viewsConfigFilePath, std::unique_ptr<sbio::view::IViewCreator> pViewCreator)
{
  if (pViewCreator == nullptr)
  {
    LogHostViewManagerError("Failed to initialize host views. View creator is null.");
    return;
  }

  fstream file;
  file.open(viewsConfigFilePath);

  if (!file.is_open())
  {
    LogHostViewManagerError("Failed to initialize host views from '" + viewsConfigFilePath.string() + "'. Could not open config file.");
    return;
  }

  try
  {
    string json;
    stringstream ss;

    while (getline(file, json))
    {
      ss << json << "\n";
    }
    json = ss.str();

    Parser jsonP;
    Poco::Dynamic::Var tmp;

    try
    {
      jsonP.parse(json);
    }
    catch (const Poco::Exception& e)
    {
      LogHostViewManagerError("Failed to parse views config '" + viewsConfigFilePath.string() + "': " + e.displayText());
      return;
    }

    Poco::Dynamic::Var jsonResult = jsonP.result();

    Object::Ptr jsonObject;
    try
    {
      jsonObject = jsonResult.extract<Object::Ptr>();
    }
    catch (const Poco::Exception& e)
    {
      LogHostViewManagerError("Failed to initialize host views from '" + viewsConfigFilePath.string() + "'. Root JSON value must be an object. " + e.displayText());
      return;
    }

    if (jsonObject.isNull())
    {
      LogHostViewManagerError("Failed to initialize host views from '" + viewsConfigFilePath.string() + "'. Root JSON object is null.");
      return;
    }

    if (!jsonObject->has("Views"))
    {
      LogHostViewManagerError("Failed to initialize host views from '" + viewsConfigFilePath.string() + "'. Missing 'Views' array.");
      return;
    }

    tmp = jsonObject->get("Views");
    if (tmp.isEmpty())
    {
      LogHostViewManagerError("Failed to initialize host views from '" + viewsConfigFilePath.string() + "'. 'Views' must be an array.");
      return;
    }

    Array::Ptr jsonArray;
    try
    {
      jsonArray = jsonObject->getArray("Views");
    }
    catch (const Poco::Exception& e)
    {
      LogHostViewManagerError("Failed to initialize host views from '" + viewsConfigFilePath.string() + "'. 'Views' must be an array. " + e.displayText());
      return;
    }

    if (jsonArray.isNull())
    {
      LogHostViewManagerError("Failed to initialize host views from '" + viewsConfigFilePath.string() + "'. 'Views' array is null.");
      return;
    }

    int size = static_cast<int>(jsonArray->size());

    for (int v = 0; v < size; v++)
    {
      Object::Ptr element;
      try
      {
        element = jsonArray->getObject(v);
      }
      catch (const Poco::Exception& e)
      {
        LogHostViewManagerError("Failed to initialize host views from '" + viewsConfigFilePath.string() + "'. Each entry in 'Views' must be an object. " + e.displayText());
        return;
      }

      if (element.isNull())
      {
        LogHostViewManagerError("Failed to initialize host views from '" + viewsConfigFilePath.string() + "'. Each entry in 'Views' must be an object.");
        return;
      }

      tmp = element->get("viewID");

      if (tmp.isEmpty())
      {
        continue;
      }

      uint16_t id = static_cast<uint16_t>(tmp.convert<uint16_t>());
      unique_ptr<sbio::view::CView> view = pViewCreator->CreateView(ViewID(id));
      if (view == nullptr)
      {
        LogHostViewManagerError("Failed to create host view for viewID " + to_string(id) + ".");
        return;
      }

      tmp = element->get("projectionMode");
      if (!tmp.isEmpty())
      {
        view->SetProjectionMode(ToProjectionMode(tmp.convert<std::string>()));
      }

      tmp = element->get("near");
      if (!tmp.isEmpty())
      {
        view->SetNearPlane(tmp.convert<float>());
      }

      tmp = element->get("far");
      if (!tmp.isEmpty())
      {
        view->SetFarPlane(tmp.convert<float>());
      }

      tmp = element->get("left");
      if (!tmp.isEmpty())
      {
        view->SetLeftHalfAngle(tmp.convert<float>());
      }

      tmp = element->get("right");
      if (!tmp.isEmpty())
      {
        view->SetRightHalfAngle(tmp.convert<float>());
      }

      tmp = element->get("top");
      if (!tmp.isEmpty())
      {
        view->SetTopHalfAngle(tmp.convert<float>());
      }

      tmp = element->get("bottom");
      if (!tmp.isEmpty())
      {
        view->SetBottomHalfAngle(tmp.convert<float>());
      }

      AddView(std::move(view));
    }
  }
  catch (const Poco::Exception& e)
  {
    LogHostViewManagerError("Failed to initialize host views from '" + viewsConfigFilePath.string() + "': " + e.displayText());
  }
  catch (const std::exception& e)
  {
    LogHostViewManagerError("Failed to initialize host views from '" + viewsConfigFilePath.string() + "': " + e.what());
  }
  catch (...)
  {
    LogHostViewManagerError("Failed to initialize host views from '" + viewsConfigFilePath.string() + "': unknown error.");
  }

  file.close();
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
