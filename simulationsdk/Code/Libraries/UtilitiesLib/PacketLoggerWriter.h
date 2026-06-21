//Copyright SimBlocks LLC 2016-2026
/**
 * @file PacketLoggerWriter.h
 * @brief Declares a class for writing packet log files from buffers.
 *
 * Defines the CPacketLoggerWriter class, which provides methods for writing frames
 * and buffers to a packet log file. Supports beginning and ending frames, and writing
 * buffer data for each frame.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_PACKET_LOGGER_WRITER_H
#define SIMBLOCKS_UTILITIES_PACKET_LOGGER_WRITER_H

#include "UtilitiesLib/Buffer.h"
#include <string>
#include <stdint.h>
#include <filesystem>

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Sequential writer for the packet-log file format.
     *
     * `CPacketLoggerWriter` emits `BEGIN_FRAME`, `FRAME_BUFFER`, and `END_FRAME` records to a binary file.
     * The writer tracks the current frame number and closes the file permanently if any write or flush fails.
     *
     * @invariant `m_nFrame` is incremented only after a frame end marker is written successfully.
     * @invariant `m_hFile == nullptr` means the writer is inactive and subsequent write operations are no-ops.
     */
    class CPacketLoggerWriter
    {
    public:
      /**
       * @brief Opens a packet log file for binary output.
       * @param filePath Path to the packet log file.
       *
       * Any existing file at `filePath` is removed before the new file is opened.
       * On success the constructor prints a status line to `std::cout`.
       *
       * @sideeffects Deletes any existing file at `filePath`, opens a new binary file, and may write status output to `std::cout`.
       * @failurecases If the file cannot be opened, the writer remains inactive and all write methods become no-ops.
       */
      CPacketLoggerWriter(const std::filesystem::path& filePath);

      /**
       * @brief Constructs a `CPacketLoggerWriter` instance.
       */
      CPacketLoggerWriter(const CPacketLoggerWriter&) = delete;
      CPacketLoggerWriter& operator=(const CPacketLoggerWriter&) = delete;
      /**
       * @brief Constructs a `CPacketLoggerWriter` instance.
       */
      CPacketLoggerWriter(CPacketLoggerWriter&&) = delete;
      CPacketLoggerWriter& operator=(CPacketLoggerWriter&&) = delete;

      /**
       * @brief Closes the packet log file if it is still open.
       */
      virtual ~CPacketLoggerWriter();

      /**
       * @brief Writes the begin-frame record for the current frame number.
       *
       * @sideeffects Writes to the log file when active.
       * @failurecases If the writer is inactive, the function returns without writing.
       * @failurecases If the write or flush fails, the file is closed and the writer becomes inactive.
       */
      void BeginFrame();

      /**
       * @brief Writes one frame-buffer record to the current frame.
       * @param buffer Pointer to the buffer bytes.
       * @param nSize Number of buffer bytes to write.
       *
       * The buffer data is preceded by an `SFrameBuffer` header containing `nSize`.
       *
       * @ownership `buffer` remains owned by the caller.
       * @sideeffects Writes to and flushes the log file when active.
       * @failurecases If the writer is inactive, the function returns without writing.
       * @failurecases If either the header or buffer data write fails, the file is closed and the writer becomes inactive.
       */
      void WriteBuffer(const char* buffer, unsigned int nSize);

      /**
       * @brief Writes the end-frame record and advances the frame counter.
       *
       * The frame counter is incremented only when the end record is written successfully.
       * On success the new frame count is printed to `std::cout`.
       *
       * @sideeffects Writes to and flushes the log file, increments `m_nFrame` on success, and may write status output to `std::cout`.
       * @failurecases If the writer is inactive, the function returns without writing.
       * @failurecases If the end-frame write fails, the file is closed and the writer becomes inactive.
       */
      void EndFrame();

    protected:
      /**
       * @brief Closes a file handle after a packet-log write failure.
       * @param hFile File handle reference to close and null out.
       *
       * @sideeffects Closes `hFile` when non-null and sets the reference to `nullptr`.
       */
      void ClosePacketLogFileOnFailure(FILE*& hFile);
      /**
       * @brief Writes and flushes packet-log data.
       * @param hFile Open file handle.
       * @param pBuffer Pointer to the data to write.
       * @param nElementSize Size of each element in bytes.
       * @param nElementCount Number of elements to write.
       * @return `true` if all elements are written and `fflush` succeeds; otherwise `false`.
       *
       * @failurecases Short writes and flush failures return `false`.
       */
      bool WritePacketLogData(FILE* hFile, const void* pBuffer, size_t nElementSize, size_t nElementCount);

    private:
      FILE* m_hFile = nullptr;///< Open packet-log file handle, or `nullptr` when the writer is inactive.
      uint64_t m_nFrame = 0;///< Frame number that will be written by the next successful `BeginFrame()`.
    };
  }
}
#endif
//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
