//Copyright SimBlocks LLC 2016-2026
/**
 * @file BufferWriter.h
 * @brief Declares a buffer writer for sequentially writing data to a buffer.
 *
 * Defines the CBufferWriter class, which extends CBuffer to provide methods for
 * writing raw and typed data sequentially into a buffer. Includes support for
 * tracking the used portion of the buffer and resetting the write position.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_BUFFER_WRITER_H
#define SIMBLOCKS_UTILITIES_BUFFER_WRITER_H

#include "Buffer.h"
#include <cstdint>
#include <cstring>

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Sequential writer over a fixed-size byte buffer.
     *
     * `CBufferWriter` uses the storage owned by `CBuffer` and maintains a current write pointer.
     * Writes append data at the current position and advance the pointer when sufficient capacity exists.
     *
     * @invariant `m_pCurrentBufferPtr` is either `nullptr` or points into the buffer represented by the base `CBuffer`.
     * @invariant The capacity is fixed after construction unless the underlying base state is modified externally.
     * @note Failed writes do not partially copy data; they return without modifying the buffer state.
     */
    class CBufferWriter : public CBuffer
    {
    public:
      /**
       * @brief Allocates the backing buffer and initializes the write cursor.
       * @param nSize Number of bytes available for writing.
       *
       * @post The current write position is the start of the buffer.
       * @failurecases Allocation may throw if `new[]` fails.
       */
      CBufferWriter(int nSize);

      /**
       * @brief Destroys the writer.
       *
       * Buffer ownership and destruction semantics are inherited from `CBuffer`.
       */
      ~CBufferWriter() override;

      /**
       * @brief Clears the backing buffer and resets the write cursor.
       *
       * Releases owned storage, sets the buffer size to zero, and moves the cursor
       * to the empty-buffer state.
       *
       * @sideeffects May release owned storage. Subsequent writes require new storage.
       */
      void Clear() override;

      /**
       * @brief Transfers the backing buffer and resets the write cursor.
       * @return The previous buffer pointer, or `nullptr` when no buffer is available.
       *
       * Ownership of a non-null returned pointer is transferred to the caller.
       *
       * @sideeffects Leaves this writer empty and unable to write until storage is assigned.
       */
      char* StealPointer() override;

      /**
       * @brief Replaces the backing buffer and resets the cursor.
       * @param nSize Number of bytes represented by `data`.
       * @param data Pointer to the replacement storage, or `nullptr`.
       *
       * The supplied storage is not owned by this writer. Pointers into or one past
       * storage owned by this writer are rejected without changing its state.
       *
       * @sideeffects May release previously owned storage and resets the write cursor.
       */
      void Set(int nSize, void* data) override;

      /**
       * @brief Returns the number of bytes written so far.
       * @return Distance in bytes between the start of the buffer and the current write position, or `0` when the writer has no valid backing buffer.
       */
      int64_t GetInUseSize() const;

      /**
       * @brief Resets the write position to the start of the buffer.
       *
       * Existing bytes in the buffer are left unchanged.
       *
       * @sideeffects Subsequent writes overwrite from the beginning of the buffer.
       */
      void Reset();

      /**
       * @brief Appends raw bytes at the current write position.
       * @param data Source bytes to append.
       * @param size Number of bytes to append.
       * @return `true` when all bytes were appended; otherwise `false`.
       */
      bool Write(const void* data, size_t size)
      {
        if (m_buffer == nullptr || m_pCurrentBufferPtr == nullptr || data == nullptr || m_nSize <= 0)
        {
          return false;
        }

        const auto offset = m_pCurrentBufferPtr - m_buffer;
        if (offset < 0 || static_cast<size_t>(offset) > static_cast<size_t>(m_nSize))
        {
          return false;
        }

        if (size > static_cast<size_t>(m_nSize) - static_cast<size_t>(offset))
        {
          return false;
        }

        std::memcpy(m_pCurrentBufferPtr, data, size);
        m_pCurrentBufferPtr += size;
        return true;
      }

      /**
       * @brief Writes a trivially copyable value at the current position.
       * @tparam T Value type to copy into the buffer.
       * @param data Value to write.
       *
       * The value is copied with `std::memcpy` and the write cursor advances by `sizeof(T)`.
       * If the buffer is not initialized or remaining capacity is insufficient, the function performs no write.
       *
       * @sideeffects Advances the current write position on success.
       * @failurecases Insufficient capacity or an invalid buffer causes a silent no-op.
       */
      template <typename T>
      void Write(const T& data)
      {
        Write(&data, sizeof(T));
      }

    protected:
      char* m_pCurrentBufferPtr = nullptr;///< Pointer to the next byte that will be written, or `nullptr` when no backing buffer is available.
    };
  }
}
#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
