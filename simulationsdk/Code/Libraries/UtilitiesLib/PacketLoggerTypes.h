//Copyright SimBlocks LLC 2016-2026
/**
 * @file PacketLoggerTypes.h
 * @brief Declares types for packet log file frame structure.
 *
 * Defines enums and structs for representing frame headers and frame types used in
 * packet log files, including frame IDs, frame headers, and specific frame data.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_PACKET_LOGGER_TYPES_H
#define SIMBLOCKS_UTILITIES_PACKET_LOGGER_TYPES_H

#include <cstdint>

/**
 * @brief Record identifiers used in the packet log binary stream.
 */
enum class EFrameID : uint64_t
{
  UNKNOWN = 0x0000,///< Unknown or uninitialized record identifier.
  BEGIN_FRAME = 0x0001,///< Record header for `SBeginFrame`.
  FRAME_BUFFER = 0x0002,///< Record header for `SFrameBuffer` followed by buffer bytes.
  END_FRAME = 0xFFFF,///< Record header for `SEndFrame`.
};

/**
 * @brief Common header stored at the start of every packet-log record.
 *
 * @invariant `ID` identifies the concrete record type serialized in the stream.
 */
struct SFrameHeader
{
  /**
   * @brief Constructs a header with `EFrameID::UNKNOWN`.
   */
  SFrameHeader()
  {
  }

  /**
   * @brief Constructs a header with a specific record identifier.
   * @param id Record identifier to store.
   */
  SFrameHeader(EFrameID id) : ID(id)
  {
  }

  EFrameID ID = EFrameID::UNKNOWN;///< Record identifier used by readers to determine the concrete record layout.
};

/**
 * @brief Packet-log record that marks the beginning of a frame.
 *
 * @invariant `ID` is initialized to `EFrameID::BEGIN_FRAME`.
 */
struct SBeginFrame : SFrameHeader
{
  /**
   * @brief Constructs a begin-frame record.
   */
  SBeginFrame() : SFrameHeader(EFrameID::BEGIN_FRAME)
  {
  }

  uint64_t nFrame = 0;///< Zero-based frame number written by `CPacketLoggerWriter`.
};

/**
 * @brief Packet-log record that describes the size of an immediately following buffer data block.
 *
 * @invariant `ID` is initialized to `EFrameID::FRAME_BUFFER`.
 */
struct SFrameBuffer : SFrameHeader
{
  /**
   * @brief Constructs a frame-buffer record.
   */
  SFrameBuffer() : SFrameHeader(EFrameID::FRAME_BUFFER)
  {
  }

  uint64_t nFrameBufferSize = 0;///< Size in bytes of the buffer data block that follows this record in the stream.
};

/**
 * @brief Packet-log record that marks the end of a frame.
 *
 * @invariant `ID` is initialized to `EFrameID::END_FRAME`.
 */
struct SEndFrame : SFrameHeader
{
  /**
   * @brief Constructs an end-frame record.
   */
  SEndFrame() : SFrameHeader(EFrameID::END_FRAME)
  {
  }
};

#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
