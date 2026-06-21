//Copyright SimBlocks LLC 2016-2026
#include "CigiEntityTypes.h"
#include "EntityLib/EntityTypes.h"
#include "IGCigiLib/IGCigiLib.h"
#include "EntityLib/EntityManager.h"
#include "UtilitiesLib/Logger.h"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <limits>
#include <stdexcept>

using namespace sbio;
using namespace sbio::entity;
using namespace sbio::cigi;
using namespace std;

namespace
{
  template <typename TValue>
  bool TryParseUnsignedToken(const std::string& token, TValue& value)
  {
    const std::string trimmedToken = boost::trim_copy(token);
    if (trimmedToken.empty())
    {
      return false;
    }

    try
    {
      size_t nParsedLength = 0;
      const unsigned long nValue = std::stoul(trimmedToken, &nParsedLength, 10);
      if (nParsedLength != trimmedToken.size() || nValue > std::numeric_limits<TValue>::max())
      {
        return false;
      }

      value = static_cast<TValue>(nValue);
      return true;
    }
    catch (const std::exception&)
    {
      return false;
    }
  }
}

CCigiEntityTypes::CCigiEntityTypes()
{
}

CCigiEntityTypes::~CCigiEntityTypes()
{
}

// Load a CigiEntityTypeID and SisoEntityType from a string/line in CSV file.
// It is valid to have a string with less than 8 tokens.
// The first token is the CigiEntityTypeID and the rest are SisoEntityType values in order.
// If a token is missing or invalid, it will be set to 0 and the rest of the tokens will still be parsed.
bool GetCigiAndSisoFromString(const std::string& line, ShortEntityTypeID& CigiEntityType, SEntityType& SisoEntityType)
{
  if (boost::trim_copy(line).empty())
  {
    return false;
  }

  vector<string> v;
  boost::split(v, line, boost::is_any_of(",."));

  if (v.empty())
  {
    return false;
  }

  uint16_t nCigiEntityType = 0;
  if (!TryParseUnsignedToken(v[0], nCigiEntityType))
  {
    return false;
  }

  CigiEntityType = ShortEntityTypeID(nCigiEntityType);
  SisoEntityType = SEntityType();

  if (v.size() > 1)
  {
    uint8_t nEntityKind = 0;
    TryParseUnsignedToken(v[1], nEntityKind);
    SisoEntityType.entityKindID = SisoEntityKindID(nEntityKind);
  }

  if (v.size() > 2)
  {
    uint8_t nEntityDomain = 0;
    TryParseUnsignedToken(v[2], nEntityDomain);
    SisoEntityType.entityDomainID = SisoEntityDomainID(nEntityDomain);
  }

  if (v.size() > 3)
  {
    uint16_t nEntityCountry = 0;
    TryParseUnsignedToken(v[3], nEntityCountry);
    SisoEntityType.entityCountryID = SisoEntityCountryID(nEntityCountry);
  }

  if (v.size() > 4)
  {
    uint8_t nEntityCategory = 0;
    TryParseUnsignedToken(v[4], nEntityCategory);
    SisoEntityType.entityCategoryID = SisoEntityCategoryID(nEntityCategory);
  }

  if (v.size() > 5)
  {
    uint8_t nEntitySubCategory = 0;
    TryParseUnsignedToken(v[5], nEntitySubCategory);
    SisoEntityType.entitySubCategoryID = SisoEntitySubCategoryID(nEntitySubCategory);
  }

  if (v.size() > 6)
  {
    uint8_t nEntitySpecific = 0;
    TryParseUnsignedToken(v[6], nEntitySpecific);
    SisoEntityType.entitySpecificID = SisoEntitySpecificID(nEntitySpecific);
  }

  if (v.size() > 7)
  {
    uint8_t nEntityExtra = 0;
    TryParseUnsignedToken(v[7], nEntityExtra);
    SisoEntityType.entityExtraID = SisoEntityExtraID(nEntityExtra);
  }

  return true;
}

// parse CigiEntityType SisoEntityType pairs from CSV file
void CCigiEntityTypes::LoadCigiToSisoConversionFileCsv(const std::filesystem::path& filePath)
{
  std::ifstream file;

  m_CigiEntityTypes.clear();

  file.open(filePath.string().c_str(), std::ios_base::in);

  // could not open file
  if (!file.is_open())
  {
    stringstream ss;
    ss << "Cigi to Siso conversion file could not be found!\n";
    ss << filePath;
    cout << ss.str() << endl;
    return;
  }

  // while there is still data in file
  string line;
  while (std::getline(file, line))
  {
    // try to extract information from that line and store into map
    ShortEntityTypeID shortEntityType(0);
    SEntityType sisoEntityType;

    if (!GetCigiAndSisoFromString(line, shortEntityType, sisoEntityType))
    {
      continue;
    }

    /*if (!g_CigiLibGlobals.pEntityManager->HasEntityType(sisoEntityType))
    {
      stringstream ss;
      ss << "Could not find entity type short(" << shortEntityType << ") " << sisoEntityType.ToEnumerationString() << " in the SISO-REF-010.xml document.";
      g_CigiLibGlobals.pLogger->LogWarning(ss.str());
    }*/

    m_CigiEntityTypes[shortEntityType] = sisoEntityType;
  }

  // close file and clean up
  file.close();
  cout << "Successfully loaded Cigi to Siso conversion file." << endl;
}

SEntityType CCigiEntityTypes::GetExtendedEntityType(ShortEntityTypeID entityTypeID)
{
  TCigiToSisoEntityType::const_iterator it = m_CigiEntityTypes.find(entityTypeID);
  if (it == m_CigiEntityTypes.end())
  {
    //CIGI 4.0 allows using only short entity type IDs without an entity enumeration association
    return SEntityType();
  }

  return it->second;
}

sbio::ShortEntityTypeID CCigiEntityTypes::GetShortEntityType(const sbio::entity::SEntityType& entityType)
{
  for (auto it : m_CigiEntityTypes)
  {
    if (it.second == entityType)
    {
      return it.first;
    }
  }

  return ShortEntityTypeID(0);
}

/// <summary>
/// Check to see if a Cigi Entity Type exists.
/// </summary>
bool CCigiEntityTypes::IfCigiEntityTypeExists(ShortEntityTypeID entityTypeID) const
{
  TCigiToSisoEntityType::const_iterator it = m_CigiEntityTypes.find(entityTypeID);

  if (it == m_CigiEntityTypes.end())
  {
    return false;
  }
  else
  {
    return true;
  }
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
