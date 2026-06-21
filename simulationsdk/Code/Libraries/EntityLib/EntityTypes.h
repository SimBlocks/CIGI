//Copyright SimBlocks LLC 2016-2026
/**
 * @file EntityTypes.h
 * @brief Declares strong types, enums, and structures for entity identification and enumeration.
 *
 * Provides strong type definitions for entity and part IDs, enumerations for clamp and coordinate systems,
 * and structures for entity type, value descriptions, enumerations, articulated part keys, transformation rates,
 * and acceleration rates. Supports detailed enumeration and management of entities and their articulated parts
 * in simulation and modeling applications.
 *
 * @see sbio::entity::SEntityType
 * @see sbio::entity::SEntityValueDescription
 * @see sbio::entity::SEntityEnumeration
 * @see sbio::entity::SEntityArticulatedPartKey
 * @see sbio::entity::STransformationRate
 * @see sbio::entity::SAccelerationRate
 */
#pragma once
#ifndef SIMBLOCKS_ENTITY_TYPES_H
#define SIMBLOCKS_ENTITY_TYPES_H

#include "GlobalHeaders/CommonTypes.h"
#include <map>
#include <string>

namespace sbio
{
  /** @brief Strong identifier for an articulated part on an entity. */
  STRONG_TYPE(ArticulatedPartID, uint8_t);
  /** @brief Strong identifier for an entity; `UINT16_MAX` represents the unknown value. */
  STRONG_TYPE_WITH_CUSTOM_UNKNOWN_VALUE(EntityID, uint16_t, UINT16_MAX);

  /** @brief Strong short-form SISO entity type identifier; `UINT16_MAX` represents the unknown value. */
  STRONG_TYPE_WITH_CUSTOM_UNKNOWN_VALUE(ShortEntityTypeID, uint16_t, UINT16_MAX);
  /** @brief Strong SISO enumeration set identifier. */
  STRONG_TYPE(SisoEnumSetID, uint8_t);
  /** @brief Strong SISO entity kind identifier. */
  STRONG_TYPE(SisoEntityKindID, uint8_t);
  /** @brief Strong SISO entity domain identifier. */
  STRONG_TYPE(SisoEntityDomainID, uint8_t);
  /** @brief Strong SISO entity country identifier. */
  STRONG_TYPE(SisoEntityCountryID, uint16_t);
  /** @brief Strong SISO entity category identifier. */
  STRONG_TYPE(SisoEntityCategoryID, uint8_t);
  /** @brief Strong SISO entity subcategory identifier. */
  STRONG_TYPE(SisoEntitySubCategoryID, uint8_t);
  /** @brief Strong SISO entity specific identifier. */
  STRONG_TYPE(SisoEntitySpecificID, uint8_t);
  /** @brief Strong SISO entity extra identifier. */
  STRONG_TYPE(SisoEntityExtraID, uint8_t);

  /**
   * @brief Entity altitude and attitude clamping mode.
   */
  enum class EClamp
  {
    UNKNOWN = -1,///< Unknown clamp mode.
    NONE,///< No clamp mode.
    NON_CONFORMAL,///< Altitude is clamped to an offset above the surface; pitch and roll remain relative to the geodetic reference plane.
    CONFORMAL///< Altitude is clamped to an offset above the surface and attitude conforms to the terrain.
  };

  /**
   * @brief Coordinate system used for object transform rates.
   */
  enum class EObjectCoordinateSystem
  {
    UNKNOWN = -1,///< Unknown coordinate system.
    WORLD = 0,///< World coordinate system.
    PARENT = 1,///< Coordinate system relative to the parent object; only valid when a parent exists.
    LOCAL,///< Local object coordinate system.
  };

  /**
   * @brief Coordinate system selector for top-level entity positioning.
   */
  enum class ETopLevelCoordinateSystem
  {
    UNKNOWN = -1,///< Unknown coordinate system.
    GEODETIC,///< Geodetic coordinate system.
    ENTITY,///< Entity coordinate system.
  };

  /**
   * @brief Degree-of-freedom selector for articulated offsets and rotations.
   */
  enum class EDegreeOfFreedom
  {
    UNKNOWN = -1,///< Unknown degree of freedom.
    NOT_USED = 0,///< No degree of freedom selected.
    X_OFFSET,///< X-axis offset.
    Y_OFFSET,///< Y-axis offset.
    Z_OFFSET,///< Z-axis offset.
    YAW,///< Yaw rotation.
    PITCH,///< Pitch rotation.
    ROLL,///< Roll rotation.
  };

  /**
   * @brief Attach or detach request state.
   */
  enum class EAttachState
  {
    UNKNOWN = -1,///< Unknown attach state.
    DETACH = 0,///< Detach from the current parent.
    ATTACH///< Attach to a parent.
  };

  namespace entity
  {
    /**
     * @brief Complete SISO entity type enumeration fields.
     */
    struct SEntityType
    {
      sbio::SisoEntityKindID entityKindID = UnknownSisoEntityKindID;///< SISO entity kind identifier.
      sbio::SisoEntityDomainID entityDomainID = UnknownSisoEntityDomainID;///< SISO entity domain identifier.
      sbio::SisoEntityCountryID entityCountryID = UnknownSisoEntityCountryID;///< SISO country identifier.
      sbio::SisoEntityCategoryID entityCategoryID = UnknownSisoEntityCategoryID;///< SISO category identifier.
      sbio::SisoEntitySubCategoryID entitySubCategoryID = UnknownSisoEntitySubCategoryID;///< SISO subcategory identifier.
      sbio::SisoEntitySpecificID entitySpecificID = UnknownSisoEntitySpecificID;///< SISO specific identifier.
      sbio::SisoEntityExtraID entityExtraID = UnknownSisoEntityExtraID;///< SISO extra identifier.

      /**
       * @brief Compares this entity type to another for equality.
       * @param rhs Entity type to compare against.
       * @return `true` when all enumeration fields match; otherwise `false`.
       */
      bool operator==(const SEntityType& rhs) const;
      /**
       * @brief Compares this entity type to another for inequality.
       * @param rhs Entity type to compare against.
       * @return `true` when at least one enumeration field differs; otherwise `false`.
       */
      bool operator!=(const SEntityType& rhs) const;
      /**
       * @brief Orders entity types by their enumeration fields.
       * @param rhs Entity type to compare against.
       * @return `true` when this entity type sorts before `rhs` by kind, domain, country, category,
       *         subcategory, specific, then extra identifier; otherwise `false`.
       */
      bool operator<(const SEntityType& rhs) const;
      /**
       * @brief Determines whether the entity type has a known entity kind.
       * @return `true` when `entityKindID` is not `UnknownSisoEntityKindID`; otherwise `false`.
       */
      bool IsValid() const;

      /**
       * @brief Parses enumeration fields from an entity enumeration string.
       *
       * The accepted format is five to seven unsigned integer tokens separated by periods:
       * `kind.domain.country.category.subcategory[.specific[.extra]]`.
       * If parsing fails, the existing field values are left unchanged.
       * @param sEntityEnumeration Enumeration string to parse.
       */
      void SetFromEnumerationString(const std::string& sEntityEnumeration);
      /**
       * @brief Formats the enumeration fields as a period-delimited string.
       * @return Seven-field enumeration string for this entity type.
       */
      std::string ToEnumerationString() const;
    };

    /**
     * @brief Describes one value from a SISO entity enumeration.
     */
    struct SEntityValueDescription
    {
      int nValue = 0;///< Enumeration value.
      std::string sUUID;///< UUID associated with the value.
      std::string sDescription;///< Human-readable value description.
    };

    /**
     * @brief SISO entity enumeration definition and value descriptions keyed by numeric value.
     */
    struct SEntityEnumeration
    {
      sbio::SisoEnumSetID enumSetID = UnknownSisoEnumSetID;///< SISO enumeration set identifier.
      std::string sName;///< Enumeration name.
      std::map<int, SEntityValueDescription> entityDescriptions;///< Value descriptions keyed by enumeration value.
    };

    /**
     * @brief Compound key identifying an articulated part on an entity.
     */
    struct SEntityArticulatedPartKey
    {
      sbio::EntityID entityID;///< Entity identifier.
      sbio::ArticulatedPartID articulatedPartID;///< Articulated part identifier.

      /**
       * @brief Constructs a key with unknown entity and articulated-part identifiers.
       */
      SEntityArticulatedPartKey();

      /**
       * @brief Compares this key to another for equality.
       * @param key Key to compare against.
       * @return `true` when both identifiers match; otherwise `false`.
       */
      bool operator==(const SEntityArticulatedPartKey& key) const;
      /**
       * @brief Orders articulated-part keys by entity and part identifiers.
       * @param key Key to compare against.
       * @return `true` when this key sorts before `key` by entity identifier and then articulated-part identifier;
       *         otherwise `false`.
       */
      bool operator<(const SEntityArticulatedPartKey& key) const;
    };

    /**
     * @brief Linear and angular velocity state in a selected coordinate system.
     * @tparam T Linear velocity coordinate value type.
     */
    template <typename T>
    struct STransformationRate
    {
      EObjectCoordinateSystem eCoordinateSystem = EObjectCoordinateSystem::UNKNOWN;///< Coordinate system for `linearVelocity`.
      T linearVelocity;///< Linear velocity value.
      sbio::math::TBodyEulerRotation angularVelocity;///< Angular velocity expressed as body Euler rates.
    };

    /**
     * @brief Linear and angular acceleration state in a selected coordinate system.
     * @tparam T Linear acceleration coordinate value type.
     */
    template <typename T>
    struct SAccelerationRate
    {
      EObjectCoordinateSystem eCoordinateSystem = EObjectCoordinateSystem::UNKNOWN;///< Coordinate system for `linearAcceleration`.
      T linearAcceleration;///< Linear acceleration value.
      sbio::math::TBodyEulerRotation angularAcceleration;///< Angular acceleration expressed as body Euler rates.
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
