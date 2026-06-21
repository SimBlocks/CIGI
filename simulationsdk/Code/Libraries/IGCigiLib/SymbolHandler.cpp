//Copyright SimBlocks LLC 2016-2026
#include "SymbolHandler.h"
#include "CigiEntity.h"
#include "CigiLib/CigiConversions.h"
#include "CigiSymbol.h"
#include "CigiView.h"
#include "EngineLib/EngineLib.h"
#include "EngineLib/IImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorEventMessenger.h"
#include "EngineLib/ImageGeneratorMessages.h"
#include "SymbolLib/SymbolSurfaceManager.h"
#include "SymbolLib/SymbolCircle.h"
#include "SymbolLib/SymbolPolygon.h"
#include "SymbolLib/SymbolText.h"
#include "SymbolLib/SymbolTexturedCircle.h"
#include "SymbolLib/SymbolTexturedPolygon.h"
#include "ViewLib/View.h"
#include "ViewLib/ViewManager.h"
#include "EntityLib/Entity.h"
#include "EntityLib/EntityManager.h"
#include "IGCigiLib/CigiMessageLogger.h"
#include "SymbolLib/SymbolTypes.h"
#include "IGCigiLib/IGCigiLib.h"
#include "MathLib/CoordinateConversions.h"
#include "MathLib/Math.h"
#include "CigiSymbolCircle.h"
#include "CigiSymbolPolygon.h"
#include "CigiSymbolText.h"
#include "CigiSymbolTexturedCircle.h"
#include "CigiSymbolTexturedPolygon.h"
#include "UtilitiesLib/Logger.h"
#include <unordered_set>
#include <vector>

using namespace std;
using namespace sbio;
using namespace sbio::math;
using namespace sbio::symbol;
using namespace sbio::engine;
using namespace sbio::cigi;
using namespace sbio::cigi::ig;
using namespace sbio::ig::symbol;

extern sbio::cigi::ig::SIGCigiLibGlobals g_CigiLibGlobals;

CCigiSymbolHandler::CCigiSymbolHandler()
{
}

CCigiSymbolHandler::~CCigiSymbolHandler()
{
}

void CCigiSymbolHandler::DestroySymbolTree(SymbolID symbolID)
{
  // If the symbol does not exist, then there is no tree to destroy
  CCigiSymbol* pSymbol = dynamic_cast<CCigiSymbol*>(g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolID));
  if (pSymbol == nullptr)
  {
    return;
  }

  // If the symbol is attached to a parent symbol, remove the symbol from its parent before destroying the tree
  CSymbol* pParentSymbol = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(pSymbol->GetParentSymbolID());
  if (pParentSymbol != nullptr)
  {
    pParentSymbol->RemoveChild(symbolID);
  }

  // Set the parent symbol ID of the root symbol to UnknownSymbolID to detach it from any parent before destroying the tree
  pSymbol->SetParentSymbolID(UnknownSymbolID);

  // Perform a depth-first traversal of the symbol tree to collect all symbol IDs to destroy.
  // The root symbol is visited first, then each child is visited recursively before moving to the next sibling.
  // A set of visited symbols is maintained to prevent infinite loops in the case of circular references.
  std::vector<SymbolID> symbolsToVisit = {symbolID};
  std::vector<SymbolID> symbolsToDestroy;
  std::unordered_set<SymbolID, StrongTypeHash<SymbolID>> visitedSymbols;

  // Depth-first traversal to collect symbol IDs for destruction
  while (!symbolsToVisit.empty())
  {
    // Get the next symbol ID to visit
    const SymbolID currentSymbolID = symbolsToVisit.back();
    symbolsToVisit.pop_back();

    // Skip if the symbol has already been visited to prevent infinite loops
    if (!visitedSymbols.insert(currentSymbolID).second)
    {
      continue;
    }

    // Add the current symbol ID to the list of symbols to destroy
    symbolsToDestroy.push_back(currentSymbolID);

    // Get the symbol object for the current symbol ID
    CSymbol* pCurrentSymbol = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(currentSymbolID);
    if (pCurrentSymbol == nullptr)
    {
      continue;
    }

    // Add the children of the current symbol to the list of symbols to visit
    const auto children = pCurrentSymbol->GetChildren();
    for (auto childSymbolID : children)
    {
      // Set the parent symbol ID of the child symbol to UnknownSymbolID to detach it from the parent before destroying the tree
      CSymbol* pChildSymbol = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(childSymbolID);
      if (pChildSymbol != nullptr)
      {
        pChildSymbol->SetParentSymbolID(UnknownSymbolID);
      }

      // Add the child symbol ID to the list of symbols to visit
      symbolsToVisit.push_back(childSymbolID);
    }
  }

  // Destroy the symbols in reverse order to ensure that child symbols are destroyed before their parents
  for (auto it = symbolsToDestroy.rbegin(); it != symbolsToDestroy.rend(); ++it)
  {
    g_CigiLibGlobals.pSymbolSurfaceManager->RemoveSymbol(*it);

    // Send a destroy symbol message for each symbol in the tree to notify the image generator of the destruction
    SDestroySymbolMessage data;
    data.SymbolID = *it;
    g_CigiLibGlobals.pEventMessenger->SendDestroySymbolMessage(data);
  }
}

void CCigiSymbolHandler::Handle(const SEntitySymbolSurfaceDefinition& entitySymbolSurfaceDefinition)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(entitySymbolSurfaceDefinition);
  }

  //if the entity does not exist, then the entity symbol surface definition is ignored
  if (!g_CigiLibGlobals.pEntityManager->HasEntity(entitySymbolSurfaceDefinition.entityID))
  {
    stringstream ss;
    ss << "Symbol Surface attached to unknown entity: " << entitySymbolSurfaceDefinition.entityID.Value() << endl;
    g_CigiLibGlobals.pLogger->LogError(ss);
    return;
  }

  if (!g_CigiLibGlobals.pSymbolSurfaceManager->HasSymbolSurface(entitySymbolSurfaceDefinition.surfaceID))
  {
    g_CigiLibGlobals.pSymbolSurfaceManager->AddSymbolSurface(entitySymbolSurfaceDefinition.surfaceID);

    SCreateSymbolSurfaceMessage data;
    data.SurfaceID = entitySymbolSurfaceDefinition.surfaceID;

    g_CigiLibGlobals.pEventMessenger->SendCreateSymbolSurfaceMessage(data);
  }

  if (entitySymbolSurfaceDefinition.eSurfaceState == ESurfaceState::DESTROYED)
  {
    g_CigiLibGlobals.pSymbolSurfaceManager->RemoveSymbolSurface(entitySymbolSurfaceDefinition.surfaceID);

    SDestroySymbolSurfaceMessage data;
    data.SurfaceID = entitySymbolSurfaceDefinition.surfaceID;
    g_CigiLibGlobals.pEventMessenger->SendDestroySymbolSurfaceMessage(data);
    return;
  }

  //For non - billboard surfaces attached to an entity, the X Offset, Y Offset, Z Offset, Yaw, Pitch, and Roll parameters specify the position
  //and attitude of the surface in relation to the entity to which it is attached.

  CigiBodyCoordinates cigiBodyCoordinates;
  cigiBodyCoordinates[0] = entitySymbolSurfaceDefinition.offsetToSurface[0];
  cigiBodyCoordinates[1] = entitySymbolSurfaceDefinition.offsetToSurface[1];
  cigiBodyCoordinates[2] = entitySymbolSurfaceDefinition.offsetToSurface[2];
  BodyCoordinates bodyCoordinates = ConvertCigiBodyCoordinatesToBodyCoordinates(cigiBodyCoordinates);

  SUpdateSymbolSurfaceMessage data;
  data.SurfaceID = entitySymbolSurfaceDefinition.surfaceID;
  data.EntityID = entitySymbolSurfaceDefinition.entityID;
  data.uvMin.U = entitySymbolSurfaceDefinition.uvMin.x();
  data.uvMin.V = entitySymbolSurfaceDefinition.uvMin.y();
  data.uvMax.U = entitySymbolSurfaceDefinition.uvMax.x();
  data.uvMax.V = entitySymbolSurfaceDefinition.uvMax.y();
  data.Offset = bodyCoordinates;
  data.Roll = entitySymbolSurfaceDefinition.roll.Value();
  data.Pitch = entitySymbolSurfaceDefinition.pitch.Value();
  data.Yaw = entitySymbolSurfaceDefinition.yaw.Value();
  data.Width = entitySymbolSurfaceDefinition.width;
  data.Height = entitySymbolSurfaceDefinition.height;

  g_CigiLibGlobals.pEventMessenger->SendUpdateSymbolSurfaceMessage(data);
}

void CCigiSymbolHandler::Handle(const SEntityBillboardSymbolSurfaceDefinition& entityBillboardSurfaceDefinition)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(entityBillboardSurfaceDefinition);
  }

  //if the entity does not exist, then the entity billboard surface definition is ignored
  if (!g_CigiLibGlobals.pEntityManager->HasEntity(entityBillboardSurfaceDefinition.entityID))
  {
    stringstream ss;
    ss << "Symbol Billboard Surface attached to unknown entity: " << entityBillboardSurfaceDefinition.entityID.Value() << endl;
    g_CigiLibGlobals.pLogger->LogError(ss);
    return;
  }

  if (!g_CigiLibGlobals.pSymbolSurfaceManager->HasSymbolSurface(entityBillboardSurfaceDefinition.surfaceID))
  {
    g_CigiLibGlobals.pSymbolSurfaceManager->AddSymbolSurface(entityBillboardSurfaceDefinition.surfaceID);

    SCreateSymbolSurfaceMessage data;
    data.SurfaceID = entityBillboardSurfaceDefinition.surfaceID;
    g_CigiLibGlobals.pEventMessenger->SendCreateSymbolSurfaceMessage(data);
  }

  if (entityBillboardSurfaceDefinition.eSurfaceState == ESurfaceState::DESTROYED)
  {
    g_CigiLibGlobals.pSymbolSurfaceManager->RemoveSymbolSurface(entityBillboardSurfaceDefinition.surfaceID);

    SDestroySymbolSurfaceMessage data;
    data.SurfaceID = entityBillboardSurfaceDefinition.surfaceID;
    g_CigiLibGlobals.pEventMessenger->SendDestroySymbolSurfaceMessage(data);
    return;
  }

  //For a billboard surface attached to an entity, offset specifies the distance along the surface�s axes from the center of the surface to the
  //entity�s reference point(see Section 3.4.4.2).
  //billboard will be facing camera and be independent of entity's rotation

  // The given offset given is surface->entity, but we want the offset from entity->surface, so the vector is reversed to fix this
  CigiBodyCoordinates cigiBodyCoordinates;
  cigiBodyCoordinates[0] = -1 * entityBillboardSurfaceDefinition.offsetToEntity[0];
  cigiBodyCoordinates[1] = -1 * entityBillboardSurfaceDefinition.offsetToEntity[1];
  cigiBodyCoordinates[2] = -1 * entityBillboardSurfaceDefinition.offsetToEntity[2];
  BodyCoordinates bodyCoordinates = ConvertCigiBodyCoordinatesToBodyCoordinates(cigiBodyCoordinates);

  SUpdateEntityBillboardSymbolSurfaceMessage data;
  data.SurfaceID = entityBillboardSurfaceDefinition.surfaceID;
  data.EntityID = entityBillboardSurfaceDefinition.entityID;
  data.uvMin.U = entityBillboardSurfaceDefinition.uvMin.x();
  data.uvMin.V = entityBillboardSurfaceDefinition.uvMin.y();
  data.uvMax.U = entityBillboardSurfaceDefinition.uvMax.x();
  data.uvMax.V = entityBillboardSurfaceDefinition.uvMax.y();
  data.Offset = bodyCoordinates;
  data.Width = entityBillboardSurfaceDefinition.width;
  data.Height = entityBillboardSurfaceDefinition.height;

  g_CigiLibGlobals.pEventMessenger->SendUpdateEntityBillboardSymbolSurfaceMessage(data);
}

void CCigiSymbolHandler::Handle(const SViewSymbolSurfaceDefinition& viewSymbolSurfaceDefinition)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(viewSymbolSurfaceDefinition);
  }

  //if the view does not exist, then the view symbol surface definition is ignored
  if (!g_CigiLibGlobals.pViewManager->HasView(viewSymbolSurfaceDefinition.viewID))
  {
    stringstream ss;
    ss << "View Symbol Surface attached to unknown view: " << viewSymbolSurfaceDefinition.viewID.Value() << endl;
    g_CigiLibGlobals.pLogger->LogError(ss);
    return;
  }

  if (!g_CigiLibGlobals.pSymbolSurfaceManager->HasSymbolSurface(viewSymbolSurfaceDefinition.surfaceID))
  {
    g_CigiLibGlobals.pSymbolSurfaceManager->AddSymbolSurface(viewSymbolSurfaceDefinition.surfaceID);

    SCreateSymbolSurfaceMessage data;
    data.SurfaceID = viewSymbolSurfaceDefinition.surfaceID;
    g_CigiLibGlobals.pEventMessenger->SendCreateSymbolSurfaceMessage(data);
  }

  if (viewSymbolSurfaceDefinition.eSurfaceState == ESurfaceState::DESTROYED)
  {
    g_CigiLibGlobals.pSymbolSurfaceManager->RemoveSymbolSurface(viewSymbolSurfaceDefinition.surfaceID);

    SDestroySymbolSurfaceMessage data;
    data.SurfaceID = viewSymbolSurfaceDefinition.surfaceID;
    g_CigiLibGlobals.pEventMessenger->SendDestroySymbolSurfaceMessage(data);
    return;
  }

  //A symbol surface that is attached to a view is overlaid onto the view�s viewport.
  //The size and position of the symbol surface are defined in terms of the width and height of the viewport.
  //The coordinates (0, 0) and (1.0, 1.0) are mapped to the lower-left and upper-right corners of the viewport, respectively.
  SUpdateViewSymbolSurfaceMessage data;
  data.SurfaceID = viewSymbolSurfaceDefinition.surfaceID;
  data.ViewID = viewSymbolSurfaceDefinition.viewID;
  data.uvMin.U = viewSymbolSurfaceDefinition.uvMin.x();
  data.uvMin.V = viewSymbolSurfaceDefinition.uvMin.y();
  data.uvMax.U = viewSymbolSurfaceDefinition.uvMax.x();
  data.uvMax.V = viewSymbolSurfaceDefinition.uvMax.y();
  data.Left = viewSymbolSurfaceDefinition.fLeft;
  data.Right = viewSymbolSurfaceDefinition.fRight;
  data.Top = viewSymbolSurfaceDefinition.fTop;
  data.Bottom = viewSymbolSurfaceDefinition.fBottom;

  g_CigiLibGlobals.pEventMessenger->SendUpdateViewSymbolSurfaceMessage(data);
}

void CCigiSymbolHandler::Handle(const SSymbolControl& symbolControl)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(symbolControl);
  }

  //ignore if symbol does not exist
  if (!g_CigiLibGlobals.pSymbolSurfaceManager->HasSymbol(symbolControl.symbolID))
  {
    return;
  }

  CCigiSymbol* pSymbol = dynamic_cast<CCigiSymbol*>(g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolControl.symbolID));

  if (symbolControl.eSymbolState == ESymbolState::VISIBLE)
  {
    //When a symbol is created, that symbol is hidden until the Host sends a Symbol Control packet with the Symbol State field set to Visible.
    //Any immediate children of that symbol either remain hidden or become visible depending upon their individual states.
    if (!pSymbol->GetEffectiveVisibility())
    {
      g_CigiLibGlobals.pSymbolSurfaceManager->SetSymbolTreeVisible(symbolControl.symbolID, true);
    }
  }
  else if (symbolControl.eSymbolState == ESymbolState::HIDDEN)
  {
    //The symbol and all of its children can be hidden at any time by setting Symbol State to Hidden(0).
    g_CigiLibGlobals.pSymbolSurfaceManager->SetSymbolTreeVisible(symbolControl.symbolID, false);
  }
  else if (symbolControl.eSymbolState == ESymbolState::DESTROYED)
  {
    //When the Symbol is no longer needed, Symbol State can be set to Destroyed (2) to direct the IG to unload the symbol and free any associated resources.
    //Any children attached to the symbol are also destroyed.
    DestroySymbolTree(symbolControl.symbolID);
    return;
  }

  if (symbolControl.eAttachState == EAttachState::ATTACH)
  {
    CSymbol* pParentSymbol = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolControl.parentSymbolID);

    // If the symbol is already attached to the specified parent symbol, then no change in attachment will occur
    // and the packet will be processed as if the symbol were already attached to that parent.
    if (pParentSymbol && pParentSymbol->GetSymbolID() != symbolControl.symbolID)
    {
      // To prevent cycles in the symbol tree, a symbol cannot be attached to a parent symbol if that parent symbol is a descendant of the symbol in the tree.
      bool bCreatesCycle = false;
      std::vector<SymbolID> symbolsToVisit = {symbolControl.symbolID};
      std::unordered_set<SymbolID, StrongTypeHash<SymbolID>> visitedSymbols;

      // Perform a depth-first traversal of the symbol tree starting from the symbol being attached
      // to check if the specified parent symbol is a descendant of the symbol.
      while (!symbolsToVisit.empty())
      {
        // Get the next symbol ID to visit
        const SymbolID currentSymbolID = symbolsToVisit.back();
        symbolsToVisit.pop_back();

        // Skip if the symbol has already been visited to prevent infinite loops
        if (!visitedSymbols.insert(currentSymbolID).second)
        {
          continue;
        }

        // If the current symbol ID matches the parent symbol ID specified in the packet,
        // then attaching the symbol to that parent would create a cycle in the symbol tree.
        CSymbol* pCurrentSymbol = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(currentSymbolID);
        if (pCurrentSymbol == nullptr)
        {
          continue;
        }

        // Add the children of the current symbol to the list of symbols to visit
        const auto children = pCurrentSymbol->GetChildren();
        for (auto childSymbolID : children)
        {
          // If the child symbol ID matches the parent symbol ID specified in the packet,
          // then attaching the symbol to that parent would create a cycle in the symbol tree.
          if (childSymbolID == symbolControl.parentSymbolID)
          {
            bCreatesCycle = true;
            break;
          }

          // Add the child symbol ID to the list of symbols to visit
          symbolsToVisit.push_back(childSymbolID);
        }

        // Brak if cycle has been detected to avoid unnecessary traversal
        if (bCreatesCycle)
        {
          break;
        }
      }

      // If attaching the symbol to the specified parent symbol would create a cycle in the symbol tree,
      // then the attachment will not be performed and an error will be logged.
      if (bCreatesCycle)
      {
        stringstream ss;
        ss << "Ignoring symbol attachment that would create a cycle. SymbolID: " << symbolControl.symbolID.Value() << ", ParentSymbolID: " << symbolControl.parentSymbolID.Value() << endl;
        g_CigiLibGlobals.pLogger->LogError(ss);
        return;
      }

      // If the symbol is currently attached to a different parent symbol, then the symbol will first be detached from its current parent
      // before being attached to the new parent.
      CSymbol* pPreviousParentSymbol = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(pSymbol->GetParentSymbolID());
      if (pPreviousParentSymbol != nullptr && pPreviousParentSymbol->GetSymbolID() != symbolControl.parentSymbolID)
      {
        pPreviousParentSymbol->RemoveChild(symbolControl.symbolID);
      }

      pParentSymbol->AddChild(symbolControl.symbolID);
      pSymbol->SetParentSymbolID(symbolControl.parentSymbolID);
      g_CigiLibGlobals.pSymbolSurfaceManager->UpdateSymbolTreeHiddenByAncestor(symbolControl.symbolID);

      // When a symbol is attached to a parent symbol, the position, rotation, and scale of the child symbol become relative to the parent symbol.
      SSetSymbolAttachedMessage data;
      data.SymbolID = symbolControl.symbolID;
      data.ParentSymbolID = symbolControl.parentSymbolID;

      if (g_CigiLibGlobals.pEventMessenger != nullptr)
      {
        g_CigiLibGlobals.pEventMessenger->SendSetSymbolAttachedMessage(data);
      }

      if (symbolControl.bSetColor)
      {
        //If the Inherit Color parameter is set to Inherited (1), then the Red, Blue, Green, and Alpha parameters are ignored and the values of the parent are used.
        if (symbolControl.bInheritColor)
        {
          pSymbol->SetColor(pParentSymbol->GetColor());
        }
        else
        {
          pSymbol->SetColor(symbolControl.color);
        }
      }
    }
  }
  else
  {
    // If the symbol is currently attached to a parent symbol, then the symbol will be detached from that parent symbol.
    CSymbol* pParentSymbol = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(pSymbol->GetParentSymbolID());
    if (pParentSymbol != nullptr)
    {
      pParentSymbol->RemoveChild(symbolControl.symbolID);
    }

    pSymbol->SetParentSymbolID(UnknownSymbolID);
    g_CigiLibGlobals.pSymbolSurfaceManager->UpdateSymbolTreeHiddenByAncestor(symbolControl.symbolID);

    SSetSymbolUnattachedMessage data;
    data.SymbolID = symbolControl.symbolID;

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      g_CigiLibGlobals.pEventMessenger->SendSetSymbolUnattachedMessage(data);
    }
    if (symbolControl.bSetColor)
    {
      //The Inherit Color parameter is ignored for top-level (i.e., root) symbols.
      pSymbol->SetColor(symbolControl.color);
    }
  }

  pSymbol->SetSymbolSurfaceID(symbolControl.surfaceID);
  pSymbol->SetRotation(symbolControl.fRotation);

  if (symbolControl.bPositionSet)
  {
    pSymbol->SetPosition(sbio::math::Vec2f(symbolControl.fPositionU, symbolControl.fPositionV));
  }

  if (symbolControl.bScaleSet)
  {
    pSymbol->SetScale(sbio::math::Vec2f(symbolControl.fScaleU, symbolControl.fScaleV));
  }

  auto scale = pSymbol->GetScale();
  auto position = pSymbol->GetPosition();

  if (g_CigiLibGlobals.pEventMessenger != nullptr)
  {
    SUpdateSymbolMessage data;
    data.PositionUV.U = position.x();
    data.PositionUV.V = position.y();
    data.ScaleU = scale.x();
    data.ScaleV = scale.y();
    data.Layer = symbolControl.nLayerID;
    data.SymbolID = symbolControl.symbolID;
    g_CigiLibGlobals.pEventMessenger->SendUpdateSymbolMessage(data);
  }

  //If the Flash Control parameter is set to Reset, then the symbol�s flash cycle will be restarted from the beginning.
  if (symbolControl.eFlashControl == EFlashControl::RESET)
  {
    pSymbol->RestartFlash();
  }
  pSymbol->SetFlash(symbolControl.flashDutyCyclePercentage.Value(), symbolControl.fFlashPeriod);
}

void CCigiSymbolHandler::Handle(const SSymbolTextDefinition& symbolTextDefinition)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(symbolTextDefinition);
  }

  CSymbol* pSymbol = nullptr;
  SymbolID symbolID = symbolTextDefinition.symbolID;

  if (g_CigiLibGlobals.pSymbolSurfaceManager->HasSymbol(symbolTextDefinition.symbolID))
  {
    pSymbol = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolTextDefinition.symbolID);

    if (pSymbol->GetSymbolType() == ESymbolType::TEXT)
    {
      //Once a Symbol Text Definition packet describing a text symbol is sent to the IG, that symbol�s type may not be changed.
    }
    else
    {
      //If a Symbol Circle Definition, Symbol Line Definition, or Symbol Clone packet is received specifying the same Symbol ID but a different type,
      //then the existing text symbol will be destroyed along with any children and a new symbol will be created using the new definition packet.
      DestroySymbolTree(symbolTextDefinition.symbolID);

      std::unique_ptr<CCigiSymbol> pCigiSymbol = std::make_unique<CCigiSymbol>(symbolTextDefinition.symbolID, ESymbolType::TEXT);
      g_CigiLibGlobals.pSymbolSurfaceManager->AddSymbol(symbolTextDefinition.symbolID, std::move(pCigiSymbol));
      pSymbol = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolID);
    }
  }
  else
  {
    std::unique_ptr<CCigiSymbol> pCigiSymbol = std::make_unique<CCigiSymbol>(symbolTextDefinition.symbolID, ESymbolType::TEXT);
    g_CigiLibGlobals.pSymbolSurfaceManager->AddSymbol(symbolTextDefinition.symbolID, std::move(pCigiSymbol));
    pSymbol = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolID);

    //All symbols are initially hidden
    pSymbol->SetVisible(false, true);
  }

  CIGCigiSymbolText* pSymbolText = dynamic_cast<CIGCigiSymbolText*>(pSymbol->GetSymbolGeometry());
  pSymbolText->Set(symbolTextDefinition, symbolID);
  pSymbolText->SendUpdate();
}

void CCigiSymbolHandler::Handle(const sbio::symbol::SSymbolTexturedCircle& symbolTexturedCricle)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(symbolTexturedCricle);
  }

  CSymbol* pSymbol = nullptr;
  SymbolID symbolID = symbolTexturedCricle.symbolID;

  if (g_CigiLibGlobals.pSymbolSurfaceManager->HasSymbol(symbolID))
  {
    pSymbol = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolID);

    if (pSymbol->GetSymbolType() != ESymbolType::TEXTURED_CIRCLE)
    {
      //Once a Symbol Textured Circle Definition packet describing a circle symbol is sent to the IG, that symbol�s type may not be changed.
      //If a Symbol Textured Polygon Definition is received specifying the same Symbol ID but a different type,
      //then the existing circle symbol shall be destroyed along with any children and a new symbol shall be created
      //using the new definition packet.
      DestroySymbolTree(symbolID);

      pSymbol = nullptr;
    }
  }

  if (pSymbol == nullptr)
  {
    std::unique_ptr<CCigiSymbol> pCigiSymbol = std::make_unique<CCigiSymbol>(symbolID, ESymbolType::TEXTURED_CIRCLE);
    g_CigiLibGlobals.pSymbolSurfaceManager->AddSymbol(symbolID, std::move(pCigiSymbol));
    pSymbol = dynamic_cast<CCigiSymbol*>(g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolID));

    //All symbols are initially hidden
    pSymbol->SetVisible(false, true);
  }

  CIGCigiSymbolTexturedCircle* pSymbolTexturedCircle = dynamic_cast<CIGCigiSymbolTexturedCircle*>(pSymbol->GetSymbolGeometry());
  pSymbolTexturedCircle->Set(symbolTexturedCricle, symbolID);
  pSymbolTexturedCircle->SendUpdate();
}

void CCigiSymbolHandler::Handle(const SSymbolCircle& symbolCircle)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(symbolCircle);
  }

  CSymbol* pSymbol = nullptr;
  SymbolID symbolID = symbolCircle.symbolID;

  if (g_CigiLibGlobals.pSymbolSurfaceManager->HasSymbol(symbolID))
  {
    pSymbol = dynamic_cast<CCigiSymbol*>(g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolID));

    if (pSymbol->GetSymbolType() != ESymbolType::CIRCLE)
    {
      //If a Symbol Circle Definition packet is received specifying the same Symbol ID but a different type,
      //then the existing text symbol shall be destroyed along with any children and a new symbol shall be created
      //using the new definition packet.
      DestroySymbolTree(symbolID);

      pSymbol = nullptr;
    }
  }

  if (pSymbol == nullptr)
  {
    std::unique_ptr<CCigiSymbol> pCigiSymbol = std::make_unique<CCigiSymbol>(symbolID, ESymbolType::CIRCLE);
    g_CigiLibGlobals.pSymbolSurfaceManager->AddSymbol(symbolID, std::move(pCigiSymbol));
    pSymbol = dynamic_cast<CCigiSymbol*>(g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolID));

    //All symbols are initially hidden
    pSymbol->SetVisible(false, true);
  }

  CIGCigiSymbolCircle* pSymbolCircle = dynamic_cast<CIGCigiSymbolCircle*>(pSymbol->GetSymbolGeometry());
  pSymbolCircle->Set(symbolCircle, symbolID);
  pSymbolCircle->SendUpdate();
}

void CCigiSymbolHandler::Handle(const SSymbolPolygon& symbolPolygon)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(symbolPolygon);
  }

  CSymbol* pSymbol = nullptr;
  SymbolID symbolID = symbolPolygon.symbolID;

  if (g_CigiLibGlobals.pSymbolSurfaceManager->HasSymbol(symbolID))
  {
    pSymbol = dynamic_cast<CCigiSymbol*>(g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolID));

    if (pSymbol->GetSymbolType() != ESymbolType::POLYGON)
    {
      //Once a Symbol Polygon Definition packet describing a circle symbol is sent to the IG, that symbol�s type may not be changed.
      //If a Symbol Polygon Definition packet is received specifying the same Symbol ID but a different type, then the existing circle symbol shall
      //be destroyed along with any children and a new symbol shall be created using the new definition packet.
      DestroySymbolTree(symbolID);

      pSymbol = nullptr;
    }
  }

  if (pSymbol == nullptr)
  {
    std::unique_ptr<CCigiSymbol> pCigiSymbol = std::make_unique<CCigiSymbol>(symbolID, ESymbolType::POLYGON);
    g_CigiLibGlobals.pSymbolSurfaceManager->AddSymbol(symbolID, std::move(pCigiSymbol));
    pSymbol = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolID);

    //All symbols are initially hidden
    pSymbol->SetVisible(false, true);
  }

  CIGCigiSymbolPolygon* pSymbolPolygon = dynamic_cast<CIGCigiSymbolPolygon*>(pSymbol->GetSymbolGeometry());
  pSymbolPolygon->Set(symbolPolygon, symbolID);
  pSymbolPolygon->SendUpdate();
}

void CCigiSymbolHandler::Handle(const SSymbolTexturedPolygon& symbolPolygon)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(symbolPolygon);
  }

  CSymbol* pSymbol = nullptr;
  SymbolID symbolID = symbolPolygon.symbolID;

  if (g_CigiLibGlobals.pSymbolSurfaceManager->HasSymbol(symbolID))
  {
    pSymbol = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolID);

    if (pSymbol->GetSymbolType() != ESymbolType::TEXTURED_POLYGON)
    {
      //Once a Symbol Textured Polygon Definition packet describing a circle symbol is sent to the IG, that symbol�s type may not be changed.
      //If a Symbol Textured Polygon Definition packet is received specifying the same Symbol ID but a different type, then the existing circle symbol shall
      //be destroyed along with any children and a new symbol shall be created using the new definition packet.
      DestroySymbolTree(symbolID);

      pSymbol = nullptr;
    }
  }

  if (pSymbol == nullptr)
  {
    std::unique_ptr<CCigiSymbol> pCigiSymbol = std::make_unique<CCigiSymbol>(symbolID, ESymbolType::TEXTURED_POLYGON);
    g_CigiLibGlobals.pSymbolSurfaceManager->AddSymbol(symbolID, std::move(pCigiSymbol));
    pSymbol = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolID);

    //All symbols are initially hidden
    pSymbol->SetVisible(false, true);
  }

  CIGCigiSymbolTexturedPolygon* pSymbolTexturedPolygon = dynamic_cast<CIGCigiSymbolTexturedPolygon*>(pSymbol->GetSymbolGeometry());
  pSymbolTexturedPolygon->Set(symbolPolygon, symbolID);
  pSymbolTexturedPolygon->SendUpdate();
}

void CCigiSymbolHandler::Handle(const sbio::symbol::SSymbolClone& symbolClone)
{
  if (g_CigiLibGlobals.pCigiMessageLogger != nullptr)
  {
    g_CigiLibGlobals.pCigiMessageLogger->LogMessageFromHostToIG(symbolClone);
  }

  CSymbol* pSymbol = nullptr;
  SymbolID symbolID = symbolClone.symbolID;

  if (symbolClone.eSymbolSourceType == ESymbolSourceType::SYMBOL)
  {
    if (!g_CigiLibGlobals.pSymbolSurfaceManager->HasSymbol(symbolClone.sourceID))
    {
      //source symbol is missing
      return;
    }

    CSymbol* pSymbolSource = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolClone.sourceID);

    if (g_CigiLibGlobals.pSymbolSurfaceManager->HasSymbol(symbolID))
    {
      pSymbol = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolID);

      if (pSymbol->GetSymbolType() != pSymbolSource->GetSymbolType())
      {
        //If a symbol with the specified identifier already exists, and if that symbol is of a different type,
        //then the IG shall destroy the existing symbol and any children and shall create a new symbol.
        DestroySymbolTree(symbolID);

        pSymbol = nullptr;
      }
    }

    // If a symbol with the specified identifier does not already exist,
    // then the IG shall create a new symbol using the definition of the source symbol specified in the packet.
    if (pSymbol == nullptr)
    {
      // The new symbol created by a Symbol Clone packet shall be an exact copy of the source symbol specified in the packet,
      // including all properties and geometry of the source symbol.
      std::unique_ptr<CSymbol> pCigiSymbol = pSymbolSource->Clone(symbolID);

      g_CigiLibGlobals.pSymbolSurfaceManager->AddSymbol(symbolID, std::move(pCigiSymbol));
      pSymbol = g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolID);
    }
    else
    {
      // If a symbol with the specified identifier already exists,
      // and if that symbol is of the same type as the source symbol specified in the packet,
      if (pSymbol->GetSymbolGeometry() != nullptr && pSymbolSource->GetSymbolGeometry() != nullptr)
      {
        // then the IG shall copy all properties and geometry from the source symbol to the existing symbol,
        // replacing the existing symbol�s current properties and geometry with those of the source symbol.
        pSymbol->GetSymbolGeometry()->CopyFrom(pSymbolSource->GetSymbolGeometry());
      }

      // If the Flash Control parameter of the packet is set to Reset, then the flash cycle of the symbol shall be restarted from the beginning.
      CCigiSymbol* pCigiSymbol = dynamic_cast<CCigiSymbol*>(pSymbol);
      CCigiSymbol* pCigiSymbolSource = dynamic_cast<CCigiSymbol*>(pSymbolSource);
      if (pCigiSymbol != nullptr && pCigiSymbolSource != nullptr)
      {
        pCigiSymbol->SetFlash(pCigiSymbolSource->GetFlashDutyCycle(), pCigiSymbolSource->GetFlashPeriod());
      }
    }

    // Copy all properties from the source symbol to the symbol specified in the packet, including color, symbol surface ID, rotation, position, and scale.
    pSymbol->SetColor(pSymbolSource->GetColor());
    pSymbol->SetSymbolSurfaceID(pSymbolSource->GetSymbolSurfaceID());
    pSymbol->SetRotation(pSymbolSource->GetRotation());
    pSymbol->SetPosition(pSymbolSource->GetPosition());
    pSymbol->SetScale(pSymbolSource->GetScale());
    pSymbol->SetVisible(pSymbolSource->IsVisible(), true);

    if (g_CigiLibGlobals.pEventMessenger != nullptr)
    {
      // Send symbol surface assignment message to ensure the symbol is assigned to the same surface as the source symbol
      SSetSymbolSurfaceMessage surfaceData;
      surfaceData.SymbolID = pSymbol->GetSymbolID();
      surfaceData.SurfaceID = pSymbol->GetSymbolSurfaceID();
      g_CigiLibGlobals.pEventMessenger->SendSetSymbolSurfaceMessage(surfaceData);

      // Send update symbol message to ensure the symbol is updated with the same position, scale, and rotation as the source symbol
      SUpdateSymbolMessage data;
      const auto position = pSymbolSource->GetPosition();
      const auto scale = pSymbolSource->GetScale();
      data.PositionUV.U = position.x();
      data.PositionUV.V = position.y();
      data.ScaleU = scale.x();
      data.ScaleV = scale.y();
      data.SymbolID = symbolID;
      g_CigiLibGlobals.pEventMessenger->SendUpdateSymbolMessage(data);
    }
  }
  else if (symbolClone.eSymbolSourceType == ESymbolSourceType::SYMBOL_TEMPLATE)
  {
    std::unique_ptr<CSymbol> pCigiSymbol = std::make_unique<CCigiSymbol>(symbolID, ESymbolType::TEMPLATE);
    g_CigiLibGlobals.pSymbolSurfaceManager->AddSymbol(symbolID, std::move(pCigiSymbol));
    pSymbol = dynamic_cast<CCigiSymbol*>(g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbol(symbolID));

    SCreateSymbolFromTemplateMessage data;
    data.SymbolID = symbolID;
    data.TemplateID = symbolClone.sourceID.Value();
    g_CigiLibGlobals.pEventMessenger->SendCreateSymbolFromTemplateMessage(data);
  }
}

void sbio::cigi::ig::CCigiSymbolHandler::Reset()
{
  for (const auto& symbolKeyValuePair : g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbols())
  {
    SDestroySymbolMessage data;
    data.SymbolID = symbolKeyValuePair.first;
    g_CigiLibGlobals.pEventMessenger->SendDestroySymbolMessage(data);
  }
  g_CigiLibGlobals.pSymbolSurfaceManager->ClearSymbols();

  for (auto surfaceID : g_CigiLibGlobals.pSymbolSurfaceManager->GetSymbolSurfaces())
  {
    SDestroySymbolSurfaceMessage data;
    data.SurfaceID = surfaceID;
    g_CigiLibGlobals.pEventMessenger->SendDestroySymbolSurfaceMessage(data);
  }
  g_CigiLibGlobals.pSymbolSurfaceManager->ClearSymbolSurfaces();
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
