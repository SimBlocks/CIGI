//Copyright SimBlocks LLC 2016-2026
/**
 * @file PacketLoggerReader.h
 * @brief Declares a class for reading packet log files into buffers.
 *
 * Defines the CPacketLoggerReader class, which provides methods for reading frames
 * from a packet log file into typed buffers. Supports checking for available data
 * and reading frames as lists of buffers.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_PACKET_LOGGER_READER_H
#define SIMBLOCKS_UTILITIES_PACKET_LOGGER_READER_H

#include "UtilitiesLib/Buffer.h"
#include <string>
#include <list>
#include <memory>
#include <filesystem>

namespace sbio
{
  namespace utils
  {
    /**
     * @brief In-memory reader for packet log files.
     *
     * The reader loads the entire packet log file into a `CBuffer` at construction and then advances
     * an internal cursor as frames are read. Each `ReadFrame()` call returns newly allocated buffers for
     * the `FRAME_BUFFER` records encountered until the matching `END_FRAME` record or end-of-file.
     *
     * @invariant `m_pBuffer` is either `nullptr` or points into the memory owned by `m_pFileContents`.
     * @ownership Returned frame buffers are owned by the caller through `std::unique_ptr`.
     */
    class CPacketLoggerReader
    {
    public:
      /**
       * @brief Loads a packet log file into memory.
       * @param filePath Path to the packet log file.
       *
       * On success the constructor prints a status line to `std::cout` and positions the internal read cursor
       * at the start of the file contents.
       *
       * @sideeffects May write status output to `std::cout`.
       * @failurecases If the file cannot be read, the reader remains empty and `HasData()` returns `false`.
       */
      CPacketLoggerReader(const std::filesystem::path& filePath);

      /**
       * @brief Destroys the reader and releases the loaded file contents.
       */
      virtual ~CPacketLoggerReader();

      /**
       * @brief Reports whether unread file data remains.
       * @return `true` when the file buffer exists, is non-empty, and the internal cursor has not reached the end; otherwise `false`.
       */
      bool HasData() const;

      /**
       * @brief Reads the next frame worth of packet buffers.
       * @return List of newly allocated buffers containing the buffer data from `FRAME_BUFFER` records in the next frame.
       *
       * The read cursor advances through the file. Encountering malformed, truncated, or unknown frame data causes the reader
       * to advance to end-of-file and return the buffers collected so far for that call.
       *
       * @ownership Ownership of each returned buffer is transferred to the caller.
       * @sideeffects Advances the internal read cursor and may print `END_FRAME` progress information to `std::cout` as implemented.
       * @failurecases If no file contents are available, returns an empty list.
       */
      std::list<std::unique_ptr<sbio::utils::TBuffer<char>>> ReadFrame();

    private:
      std::unique_ptr<sbio::utils::CBuffer> m_pFileContents;///< Buffer owning the entire packet log file contents, or `nullptr` when the file could not be read.
      char* m_pBuffer = nullptr;///< Current read cursor into `m_pFileContents`.
    };
  }
}
#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
