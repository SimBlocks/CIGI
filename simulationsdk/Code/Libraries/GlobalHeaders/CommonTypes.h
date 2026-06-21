//Copyright SimBlocks LLC 2016-2026
/**
 * @file CommonTypes.h
 * @brief Declares common strong types and shared value structs for the `sbio` namespace.
 *
 * This header provides strong typedefs, ranged types, and structures used across multiple SDK libraries, 
* including identifiers, date and time wrappers, color values, texture coordinates, and projection metadata.
 */
#pragma once
#ifndef SIMBLOCKS_COMMON_TYPES_H
#define SIMBLOCKS_COMMON_TYPES_H

#include "GlobalHeaders/StrongTypes.h"
#include "MathLib/MathTypes.h"

#pragma warning(disable : 4100)//disable warning C4100: unreferenced formal parameter

namespace sbio
{
  /**
   * @brief Stores an opacity channel value backed by `uint8_t`.
   *
   * Invariants:
   * - The wrapped scalar is stored in the generated `StrongType<uint8_t>` base.
   * - `UnknownAlpha` is the default unknown value.
   * - In the generated API, `UnknownAlpha` is outside the valid range accepted by `CheckValid()`.
   *
    * @param Alpha Alpha value.
    * @param uint8_t Uint8 t value.
   */
  RANGED_STRONG_INT(Alpha, uint8_t, 0, 255);
  /**
   * @brief Stores a database identifier backed by `uint8_t`.
   *
   * Invariants:
   * - The wrapped scalar is stored in the generated `StrongType<uint8_t>` base.
   * - `UnknownDatabaseID` uses `0` as the default unknown value.
   *
    * @param DatabaseID Database id value.
    * @param uint8_t Uint8 t value.
   */
  STRONG_TYPE(DatabaseID, uint8_t);
  /**
   * @brief Stores a font identifier backed by `uint8_t`.
   *
   * Invariants:
   * - The wrapped scalar is stored in the generated `StrongType<uint8_t>` base.
   * - `UnknownFontID` uses `0` as the default unknown value.
   *
    * @param FontID Font id value.
    * @param uint8_t Uint8 t value.
   */
  STRONG_TYPE(FontID, uint8_t);
  /**
   * @brief Stores a monotonic frame index backed by `uint32_t`.
   *
   * Invariants:
   * - The wrapped scalar is stored in the generated `StrongType<uint32_t>` base.
   * - `UnknownFrameNumber` uses `0` as the default unknown value.
   *
    * @param FrameNumber Frame number value.
    * @param uint32_t Uint32 t value.
   */
  STRONG_TYPE(FrameNumber, uint32_t);
  /**
   * @brief Stores a system-level identifier backed by `uint32_t`.
   *
   * Invariants:
   * - The wrapped scalar is stored in the generated `StrongType<uint32_t>` base.
   * - `UnknownSystemID` uses `0` as the default unknown value.
   *
    * @param SystemID System id value.
    * @param uint32_t Uint32 t value.
   */
  STRONG_TYPE(SystemID, uint32_t);
  /**
   * @brief Stores a texture identifier backed by `uint16_t`.
   *
   * Invariants:
   * - The wrapped scalar is stored in the generated `StrongType<uint16_t>` base.
   * - `UnknownTextureID` uses `0` as the default unknown value.
   *
    * @param TextureID Texture id value.
    * @param uint16_t Uint16 t value.
   */
  STRONG_TYPE(TextureID, uint16_t);
  /**
   * @brief Stores a material identifier backed by `int32_t`.
   *
   * Invariants:
   * - The wrapped scalar is stored in the generated `StrongType<int32_t>` base.
   * - `UnknownMaterialID` uses the sentinel `-1` as the default unknown value.
   *
    * @param MaterialID Material id value.
    * @param int32_t Int32 t value.
   */
  STRONG_TYPE_WITH_CUSTOM_UNKNOWN_VALUE(MaterialID, int32_t, -1);
  /**
   * @brief Stores an image generator identifier backed by `uint32_t`.
   *
   * Invariants:
   * - The wrapped scalar is stored in the generated `StrongType<uint32_t>` base.
   * - `UnknownImageGeneratorID` uses `0` as the default unknown value.
   *
    * @param ImageGeneratorID Image generator id value.
    * @param uint32_t Uint32 t value.
   */
  STRONG_TYPE(ImageGeneratorID, uint32_t);

  /**
   * @brief Stores a calendar year backed by `uint16_t`.
   *
   * Invariants:
   * - The wrapped scalar is stored in the generated `StrongType<uint16_t>` base.
   * - `UnknownYear` uses `0` as the default unknown value.
   *
    * @param Year Year value.
    * @param uint16_t Uint16 t value.
   */
  STRONG_TYPE(Year, uint16_t);
  /**
   * @brief Stores a zero-based calendar month backed by `uint8_t`.
   *
   * Invariants:
   * - The wrapped scalar is stored in the generated `StrongType<uint8_t>` base.
   * - `UnknownMonth` is the default unknown value.
   * - In the generated API, `UnknownMonth` is outside the valid range accepted by `CheckValid()`.
   *
    * @param Month Month value.
    * @param uint8_t Uint8 t value.
   */
  RANGED_STRONG_INT(Month, uint8_t, 0, 11);//month is 0-based
  /**
   * @brief Stores a calendar day-of-month backed by `uint8_t`.
   *
   * Invariants:
   * - The wrapped scalar is stored in the generated `StrongType<uint8_t>` base.
   * - `UnknownDay` is the default unknown value.
   * - In the generated API, `UnknownDay` is outside the valid range accepted by `CheckValid()`.
   *
    * @param Day Day value.
    * @param uint8_t Uint8 t value.
   */
  RANGED_STRONG_INT(Day, uint8_t, 1, 31);//day is calendar day index
  /**
   * @brief Stores an hour-of-day backed by `uint8_t`.
   *
   * Invariants:
   * - The wrapped scalar is stored in the generated `StrongType<uint8_t>` base.
   * - `UnknownHour` is the default unknown value.
   * - In the generated API, `UnknownHour` is outside the valid range accepted by `CheckValid()`.
   *
    * @param Hour Hour value.
    * @param uint8_t Uint8 t value.
   */
  RANGED_STRONG_INT(Hour, uint8_t, 0, 23);
  /**
   * @brief Stores a minute-of-hour backed by `uint8_t`.
   *
   * Invariants:
   * - The wrapped scalar is stored in the generated `StrongType<uint8_t>` base.
   * - `UnknownMinute` is the default unknown value.
   * - In the generated API, `UnknownMinute` is outside the valid range accepted by `CheckValid()`.
   *
    * @param Minute Minute value.
    * @param uint8_t Uint8 t value.
   */
  RANGED_STRONG_INT(Minute, uint8_t, 0, 59);
  /**
   * @brief Stores a seconds component backed by `float`.
   *
   * Invariants:
   * - The wrapped scalar is stored in the generated `StrongType<float>` base.
   * - `UnknownSecond` is the default unknown value.
   * - In the generated API, `UnknownSecond` is outside the valid range accepted by `CheckValid()`.
   *
    * @param Second Second value.
    * @param float Float value.
   */
  RANGED_STRONG_FLOAT(Second, float, 0, 60);
  /**
   * @brief Stores a microsecond component backed by `uint32_t`.
   *
   * Invariants:
   * - The wrapped scalar is stored in the generated `StrongType<uint32_t>` base.
   * - `UnknownMicrosecond` uses `0` as the default unknown value.
   *
    * @param Microsecond Microsecond value.
    * @param uint32_t Uint32 t value.
   */
  STRONG_TYPE(Microsecond, uint32_t);
  /**
   * @brief Stores a normalized percentage backed by `float`.
   *
   * Invariants:
   * - The wrapped scalar is stored in the generated `StrongType<float>` base.
   * - `UnknownPercentage` is the default unknown value.
   * - In the generated API, `UnknownPercentage` is outside the valid range accepted by `CheckValid()`.
   *
    * @param Percentage Percentage value.
    * @param float Float value.
   */
  RANGED_STRONG_FLOAT(Percentage, float, 0, 1);

  /**
   * @brief Stores a four-channel 8-bit color value.
   *
   * Invariants:
   * - Each component is stored independently as an unsigned 8-bit value.
   * - The type does not enforce color-space semantics beyond preserving the stored channel values.
   *
   * Side effects:
   * - Member functions modify only the stored channels of the current instance.
   */
  struct SColor32
  {
    uint8_t r = 0;///< Red channel.
    uint8_t g = 0;///< Green channel.
    uint8_t b = 0;///< Blue channel.
    uint8_t a = 0;///< Alpha channel.

    /**
     * @brief Sets all channels from explicit component values.
     *
     * Side effects:
     * - Replaces the current channel values with the supplied values.
     *
     * @param r Red channel.
     * @param g Green channel.
     * @param b Blue channel.
     * @param a Alpha channel.
     */
    void Set(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    /**
     * @brief Sets all channels from a packed 32-bit color value.
     *
     * The packed input is interpreted as `0xRRGGBBAA`.
     *
     * Side effects:
     * - Replaces the current channel values with the unpacked components.
     *
     * @param color Packed RGBA value.
     */
    void Set(uint32_t color);

    /**
     * @brief Packs the channels into a 32-bit value.
     *
     * The returned value uses the layout `0xRRGGBBAA`.
     *
     * @return Packed RGBA value.
     */
    uint32_t To_uint32_t() const;

    /**
     * @brief Compares two colors for exact channel equality.
     * @param c Color to compare against.
     * @return `true` when all four channels match exactly; otherwise `false`.
     */
    bool operator==(const SColor32& c) const;

    /**
     * @brief Provides a representation-based ordering for associative containers.
     *
     * The comparison is performed on the struct's raw 32-bit in-memory representation rather than on
     * perceptual color properties.
     *
     * @param c Color to compare against.
     * @return `true` when this instance sorts before `c`; otherwise `false`.
     */
    bool operator<(const SColor32& c) const;
  };

  /**
   * @brief Stores a two-channel 8-bit color-like value.
   *
   * The semantic meaning of the second channel depends on the caller; the type itself only stores the
   * two bytes.
   *
   * Invariants:
   * - `channel1` and `channel2` are stored exactly as assigned.
   *
   * Side effects:
   * - `Set()` replaces the stored channel values.
   */
  struct SColor16
  {
    uint8_t channel1 = 0;///< First channel value.
    uint8_t channel2 = 0;///< Second channel value. Callers may use this as alpha or as another domain-specific channel.

    /**
     * @brief Sets both stored channels.
     *
     * Side effects:
     * - Replaces `channel1` and `channel2`.
     *
     * @param c1 First channel value.
     * @param c2 Second channel value.
     */
    void Set(uint8_t c1, uint8_t c2);
  };

  /**
   * @brief Stores a three-channel floating-point color value.
   *
   * Invariants:
   * - Components are stored exactly as assigned.
   * - No range clamping is performed; values are not restricted to `[0, 1]`.
   *
   * Side effects:
   * - `ScaleBy()`, `Set()`, and `SetFrom()` modify the current instance in place.
   */
  struct SColor3f
  {
    float r = 0;///< Red channel.
    float g = 0;///< Green channel.
    float b = 0;///< Blue channel.

    /**
     * @brief Multiplies each component by the corresponding component in `scale`.
     *
     * Side effects:
     * - Updates `r`, `g`, and `b` in place.
     *
     * @param scale Per-channel multiplier.
     */
    void ScaleBy(sbio::SColor3f scale);

    /**
     * @brief Sets all three color components explicitly.
     *
     * Side effects:
     * - Replaces the current component values.
     *
     * @param r Red channel.
     * @param g Green channel.
     * @param b Blue channel.
     */
    void Set(float r, float g, float b);

    /**
     * @brief Converts an 8-bit color to normalized floating-point RGB.
     *
     * The alpha component of `color` is ignored.
     *
     * Side effects:
     * - Replaces `r`, `g`, and `b` with `color` converted to the range `[0.0f, 1.0f]`.
     *
     * @param color Source 8-bit color.
     */
    void SetFrom(SColor32 color);

    /**
     * @brief Compares two colors using `sbio::math::fequals` for each component.
     * @param c Color to compare against.
     * @return `true` when all components are approximately equal; otherwise `false`.
     */
    bool operator==(const SColor3f& c) const;

    /**
     * @brief Performs lexicographical ordering by `r`, then `g`, then `b`.
     * @param c Color to compare against.
     * @return `true` when this instance sorts before `c`; otherwise `false`.
     */
    bool operator<(const SColor3f& c) const;
  };

  /**
   * @brief Stores a four-channel floating-point color value.
   *
   * Invariants:
   * - Components are stored exactly as assigned.
   * - No range clamping is performed; values are not restricted to `[0, 1]`.
   *
   * Side effects:
   * - `ScaleBy()`, `Set()`, and `SetFrom()` modify the current instance in place.
   */
  struct SColor4f
  {
    float r = 0;///< Red channel.
    float g = 0;///< Green channel.
    float b = 0;///< Blue channel.
    float a = 0;///< Alpha channel.

    /**
     * @brief Initializes all channels to `0.0f`.
     */
    SColor4f();

    /**
     * @brief Initializes RGB from `color` and alpha from `alpha`.
     * @param color Source RGB value.
     * @param alpha Alpha component to store.
     */
    SColor4f(SColor3f color, float alpha);

    /**
     * @brief Multiplies each component by the corresponding component in `scale`.
     *
     * Side effects:
     * - Updates `r`, `g`, `b`, and `a` in place.
     *
     * @param scale Per-channel multiplier.
     */
    void ScaleBy(sbio::SColor4f scale);

    /**
     * @brief Sets all four color components explicitly.
     *
     * Side effects:
     * - Replaces the current component values.
     *
     * @param r Red channel.
     * @param g Green channel.
     * @param b Blue channel.
     * @param a Alpha channel.
     */
    void Set(float r, float g, float b, float a);

    /**
     * @brief Converts an 8-bit color to normalized floating-point RGBA.
     *
     * Side effects:
     * - Replaces all four components with `color` converted to the range `[0.0f, 1.0f]`.
     *
     * @param color Source 8-bit color.
     */
    void SetFrom(SColor32 color);

    /**
     * @brief Compares two colors for exact component equality.
     * @param c Color to compare against.
     * @return `true` when all four components match exactly; otherwise `false`.
     */
    bool operator==(const SColor4f& c) const;
  };

  /**
   * @brief Stores a two-dimensional texture coordinate in `(U, V)` form.
   *
   * Invariants:
   * - `U` and `V` are stored exactly as assigned or converted.
   * - No normalization or range validation is performed.
   *
   */
  struct STextureCoordinateUV
  {
    float U = 0;///< U coordinate.
    float V = 0;///< V coordinate.

    /**
     * @brief Initializes both coordinates to `0.0f`.
     */
    STextureCoordinateUV();

    /**
     * @brief Initializes the coordinate from integer inputs.
     * @param u U coordinate.
     * @param v V coordinate.
     */
    STextureCoordinateUV(int u, int v);

    /**
     * @brief Initializes the coordinate from double-precision inputs.
     * @param u U coordinate.
     * @param v V coordinate.
     */
    STextureCoordinateUV(double u, double v);

    /**
     * @brief Initializes the coordinate from single-precision inputs.
     * @param u U coordinate.
     * @param v V coordinate.
     */
    STextureCoordinateUV(float u, float v);

    /**
     * @brief Returns the component-wise sum of two texture coordinates.
     * @param other Coordinate to add.
     * @return New coordinate whose `U` and `V` are the sums of the corresponding components.
     */
    STextureCoordinateUV operator+(const STextureCoordinateUV& other) const;
  };

  /**
   * @brief Stores a two-dimensional texture coordinate in `(S, T)` form.
   *
   * Invariants:
   * - `S` and `T` are stored exactly as assigned.
   * - No normalization or range validation is performed.
   *
   */
  struct STextureCoordinateST
  {
    float S = 0;///< S coordinate.
    float T = 0;///< T coordinate.
  };

  /**
   * @brief Represents a calendar date.
   *
   * Invariants:
   * - Members default to their corresponding unknown sentinel values.
   * - The type does not enforce cross-field calendar validity such as month-specific day counts.
   *
   */
  struct SDate
  {
    Year year = UnknownYear;///< Calendar year, or `UnknownYear` when unspecified.
    Month month = UnknownMonth;///< Zero-based month, or `UnknownMonth` when unspecified.
    Day day = UnknownDay;///< Day-of-month, or `UnknownDay` when unspecified.
  };

  /**
   * @brief Represents a time of day.
   *
   * Invariants:
   * - Members default to their corresponding unknown sentinel values.
   * - The type does not enforce relationships between fields beyond the validation built into the
   *   individual strong types.
   *
   */
  struct STime
  {
    Hour hour = UnknownHour;///< Hour-of-day, or `UnknownHour` when unspecified.
    Minute minute = UnknownMinute;///< Minute-of-hour, or `UnknownMinute` when unspecified.
    Second seconds = UnknownSecond;///< Seconds component, or `UnknownSecond` when unspecified.
  };

  /**
   * @brief Enumerates supported projection representations.
   */
  enum class EDatabaseProjectionType : uint32_t
  {
    UNKNOWN = (std::numeric_limits<uint32_t>::max)(),///< Unknown projection type.
    FLAT_EARTH = 0,///< Flat-earth projection.
    ROUND_EARTH = 2,///< Round-earth projection.
    UTM = 4,///< Universal Transverse Mercator projection.
  };

  /**
   * @brief Describes the projection metadata associated with a database.
   *
   * Invariants:
   * - `projectionType` defaults to `EDatabaseProjectionType::ROUND_EARTH`.
   * - Geographic origin members default to their unknown sentinels.
   * - The type does not validate whether `UTMzone` is meaningful for the selected projection type.
   *
   */
  struct SDatabaseProjection
  {
    EDatabaseProjectionType projectionType = EDatabaseProjectionType::ROUND_EARTH;///< Projection model used by the database.
    sbio::math::Latitude originLatitude = sbio::math::UnknownLatitude;///< Projection origin latitude, or `UnknownLatitude` when unspecified.
    sbio::math::Longitude originLongitude = sbio::math::UnknownLongitude;///< Projection origin longitude, or `UnknownLongitude` when unspecified.
    int UTMzone = 0;///< UTM zone number. Interpretation is caller-defined when `projectionType` is not `EDatabaseProjectionType::UTM`.
  };
}

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
