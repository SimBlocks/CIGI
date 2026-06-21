//Copyright SimBlocks LLC 2016-2026
#include "EntityTypes.h"
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

using namespace std;
using namespace sbio;
using namespace sbio::entity;

namespace
{
  // Helper function to parse an unsigned integer token from a string and validate it against the maximum value of the target type.
  template <typename TValue>
  bool TryParseUnsignedToken(const std::string& token, TValue& value)
  {
    if (token.empty())
    {
      return false;
    }

    try
    {
      size_t nParsedLength = 0;
      const unsigned long nValue = std::stoul(token, &nParsedLength, 10);
      if (nParsedLength != token.size() || nValue > std::numeric_limits<TValue>::max())
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

bool SEntityType::operator==(const SEntityType& rhs) const
{
  return (entityKindID == rhs.entityKindID) && (entityDomainID == rhs.entityDomainID) && (entityCountryID == rhs.entityCountryID) && (entityCategoryID == rhs.entityCategoryID) && (entitySubCategoryID == rhs.entitySubCategoryID) && (entitySpecificID == rhs.entitySpecificID) &&
         (entityExtraID == rhs.entityExtraID);
}

bool SEntityType::operator!=(const SEntityType& rhs) const
{
  return !(*this == rhs);
}

bool SEntityType::operator<(const SEntityType& rhs) const
{
  // Compare fields in order of significance, returning as soon as a difference is found.
  if (entityKindID == rhs.entityKindID)
  {
    if (entityDomainID == rhs.entityDomainID)
    {
      if (entityCountryID == rhs.entityCountryID)
      {
        if (entityCategoryID == rhs.entityCategoryID)
        {
          if (entitySubCategoryID == rhs.entitySubCategoryID)
          {
            if (entitySpecificID == rhs.entitySpecificID)
            {
              if (entityExtraID == rhs.entityExtraID)
              {
                return false;
              }
              else
              {
                return entityExtraID < rhs.entityExtraID;
              }
            }
            else
            {
              return entitySpecificID < rhs.entitySpecificID;
            }
          }
          else
          {
            return entitySubCategoryID < rhs.entitySubCategoryID;
          }
        }
        else
        {
          return entityCategoryID < rhs.entityCategoryID;
        }
      }
      else
      {
        return entityCountryID < rhs.entityCountryID;
      }
    }
    else
    {
      return entityDomainID < rhs.entityDomainID;
    }
  }
  else
  {
    return entityKindID < rhs.entityKindID;
  }
}

bool SEntityType::IsValid() const
{
  return entityKindID != UnknownSisoEntityKindID;
}

void SEntityType::SetFromEnumerationString(const std::string& sEntityEnumeration)
{
  istringstream stream(sEntityEnumeration);
  vector<string> tokens;
  string s;
  while (getline(stream, s, '.'))
  {
    tokens.push_back(s);
  }

  // Validate that the enumeration string is not empty, does not end with a period, and contains between 5 and 7 tokens (inclusive).
  if (sEntityEnumeration.empty() || (!sEntityEnumeration.empty() && sEntityEnumeration.back() == '.') || tokens.size() < 5 || tokens.size() > 7)
  {
    return;
  }

  uint8_t nEntityKind = 0;
  uint8_t nEntityDomain = 0;
  uint16_t nEntityCountry = 0;
  uint8_t nEntityCategory = 0;
  uint8_t nEntitySubCategory = 0;
  uint8_t nEntitySpecific = 0;
  uint8_t nEntityExtra = 0;

  // Parse the first 5 tokens, which are required, and validate that they are unsigned integers within the range of their respective types.
  // Then parse the optional specific and extra tokens if they exist.
  if (!TryParseUnsignedToken(tokens[0], nEntityKind) || !TryParseUnsignedToken(tokens[1], nEntityDomain) || !TryParseUnsignedToken(tokens[2], nEntityCountry) || !TryParseUnsignedToken(tokens[3], nEntityCategory) || !TryParseUnsignedToken(tokens[4], nEntitySubCategory))
  {
    return;
  }

  // If specific and extra tokens are present, parse and validate them as well.
  if (tokens.size() > 5 && !TryParseUnsignedToken(tokens[5], nEntitySpecific))
  {
    return;
  }

  // If specific and extra tokens are present, parse and validate them as well.
  if (tokens.size() > 6 && !TryParseUnsignedToken(tokens[6], nEntityExtra))
  {
    return;
  }

  // Assign parsed values to the respective fields, using unknown sentinels for optional fields if they are not present.
  entityKindID = SisoEntityKindID(nEntityKind);
  entityDomainID = SisoEntityDomainID(nEntityDomain);
  entityCountryID = SisoEntityCountryID(nEntityCountry);
  entityCategoryID = SisoEntityCategoryID(nEntityCategory);
  entitySubCategoryID = SisoEntitySubCategoryID(nEntitySubCategory);
  entitySpecificID = UnknownSisoEntitySpecificID;
  entityExtraID = UnknownSisoEntityExtraID;

  // Only assign specific and extra IDs if the corresponding tokens were present, otherwise leave them as unknown sentinels.
  if (tokens.size() > 5)
  {
    entitySpecificID = SisoEntitySpecificID(nEntitySpecific);
  }

  // Only assign specific and extra IDs if the corresponding tokens were present, otherwise leave them as unknown sentinels.
  if (tokens.size() > 6)
  {
    entityExtraID = SisoEntityExtraID(nEntityExtra);
  }
}

std::string SEntityType::ToEnumerationString() const
{
  stringstream ss;
  ss << entityKindID << "." << entityDomainID << "." << entityCountryID << "." << entityCategoryID << "." << entitySubCategoryID << "." << entitySpecificID << "." << entityExtraID;

  return ss.str();
}

SEntityArticulatedPartKey::SEntityArticulatedPartKey() : entityID(sbio::UnknownEntityID), articulatedPartID(sbio::UnknownArticulatedPartID)
{
}

bool SEntityArticulatedPartKey::operator==(const SEntityArticulatedPartKey& key) const
{
  return (entityID == key.entityID) && (articulatedPartID == key.articulatedPartID);
}

bool SEntityArticulatedPartKey::operator<(const SEntityArticulatedPartKey& key) const
{
  if (entityID == key.entityID)
  {
    return articulatedPartID < key.articulatedPartID;
  }

  return entityID < key.entityID;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
