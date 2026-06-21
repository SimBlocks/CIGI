//Copyright SimBlocks LLC 2016-2026

/**
 * @file Buffer.h
 * @brief Provides buffer management classes for raw and typed data.
 *
 * This header defines the CBuffer class for managing raw character buffers and the TBuffer template
 * for type-safe buffer management of arbitrary element types.
 *
 * - CBuffer: Manages a buffer of bytes, including allocation, deallocation, and ownership semantics.
 * - TBuffer<T>: Template for type-safe buffer management for any element type.
 */
#pragma once
#ifndef SIMBLOCKS_UTILITIES_BUFFER_H
#define SIMBLOCKS_UTILITIES_BUFFER_H

#include <limits>

namespace sbio
{
  namespace utils
  {
    /**
     * @brief Base class for managing a contiguous byte buffer.
     *
     * `CBuffer` stores a buffer pointer, the buffer size in bytes, and an ownership flag.
     * When `m_own_Buffer` is `true`, the destructor and `Clear()` release the buffer with `delete[]`.
     * When `m_own_Buffer` is `false`, the caller retains ownership of the pointed-to memory.
     *
     * @invariant `m_nSize` is the buffer size in bytes represented by `m_buffer`.
     * @invariant `m_buffer == nullptr` implies an empty buffer as reported by `IsEmpty()`.
     * @invariant If `m_own_Buffer` is `true`, `m_buffer` must either be `nullptr` or point to memory allocated with `new[]`.
     *
     * @note This class does not perform bounds checking on caller access through `GetBuffer()`.
     */
    class CBuffer
    {
    public:
      /**
       * @brief Allocates a new byte buffer.
       * @param nSize Number of bytes to allocate.
       *
       * @post `GetSize()` returns `nSize`.
       * @post If `nSize` is greater than zero, the instance owns the allocated memory.
       *
       * @failurecases Allocation may throw if `new[]` fails.
       */
      CBuffer(int nSize);

      /**
       * @brief Wraps an existing byte buffer.
       * @param nSize Number of bytes represented by `buffer`.
       * @param buffer Pointer to the existing memory block, or `nullptr`.
       * @param bOwnsBuffer `true` if this instance assumes responsibility for deleting `buffer` with `delete[]`; otherwise `false`.
       *
       * @ownership Ownership of `buffer` is transferred to this instance only when `bOwnsBuffer` is `true`.
       * @invariant The caller must ensure `buffer` remains valid for the lifetime of this object when `bOwnsBuffer` is `false`.
       */
      CBuffer(int nSize, char* buffer, bool bOwnsBuffer);

      /**
       * @brief Destroys the buffer wrapper.
       *
       * Releases the buffer only when this instance owns it.
       *
       * @sideeffects May free the memory referenced by `m_buffer`.
       */
      ~CBuffer();

      /**
       * @brief Constructs a `CBuffer` instance.
       */
      CBuffer(const CBuffer&) = delete;
      CBuffer& operator=(const CBuffer&) = delete;

      /**
       * @brief Clears the current buffer state.
       *
       * Releases the current buffer when owned, then sets the pointer to `nullptr` and the size to zero.
       *
       * @post `GetBuffer()` returns `nullptr`.
       * @post `GetSize()` returns `0`.
       * @sideeffects May free owned memory.
       */
      void Clear();

      /**
       * @brief Returns mutable access to the underlying byte buffer.
       * @return Pointer to the buffer, or `nullptr` if the buffer is empty.
       *
       * @ownership The returned pointer remains owned according to the current ownership state of this object.
       */
      char* GetBuffer();

      /**
       * @brief Returns read-only access to the underlying byte buffer.
       * @return Pointer to the buffer, or `nullptr` if the buffer is empty.
       *
       * @ownership The returned pointer remains owned according to the current ownership state of this object.
       */
      const char* GetBuffer() const;

      /**
       * @brief Gets the represented buffer size.
       * @return Buffer size in bytes.
       */
      int GetSize() const;

      /**
       * @brief Indicates whether the buffer currently contains usable storage.
       * @return `true` if the pointer is `nullptr` or the stored size is zero; otherwise `false`.
       */
      bool IsEmpty() const;

      /**
       * @brief Transfers the current buffer pointer to the caller.
       * @return The current buffer pointer, which may be `nullptr`.
       *
       * After this call, the instance no longer references the memory and reports an empty buffer.
       *
       * @post `GetBuffer()` returns `nullptr`.
       * @post `GetSize()` returns `0`.
       * @post This instance no longer owns any previously referenced buffer.
       * @ownership The caller becomes responsible for the returned pointer, if non-null.
       */
      char* StealPointer();

      /**
       * @brief Replaces the current buffer with an external memory block.
       * @param nSize Number of bytes represented by `data`.
       * @param data Pointer to the new memory block, or `nullptr`.
       *
       * Releases the current buffer when owned, then stores `data` without taking ownership.
       *
       * @post `GetSize()` returns `nSize`.
       * @post The stored buffer is treated as non-owned.
       * @ownership The caller retains responsibility for the lifetime of `data`.
       * @sideeffects May free previously owned memory.
       */
      void Set(int nSize, void* data);

    protected:
      bool m_own_Buffer = true;///< `true` when this object is responsible for deleting `m_buffer` with `delete[]`.
      int m_nSize = 0;///< Buffer size in bytes.
      char* m_buffer = nullptr;///< Pointer to the first byte of the represented buffer, or `nullptr` when empty.
    };

    /**
     * @brief Template class for managing a contiguous buffer of elements of type T..
     *
     * `TBuffer<T>` extends `CBuffer` by interpreting the underlying byte buffer as an array of `T`.
     * Buffer size is tracked in bytes by the base class and exposed as an element count through `GetNumElements()`.
     *
     * @tparam T Element type stored in the buffer.
     * @invariant `GetSize()` is always an integral multiple of `sizeof(T)` for buffers created through this class.
     * @note Resizing with `SetNumElements()` discards any existing contents.
     */
    template <typename T>
    class TBuffer : public CBuffer
    {
    public:
      /**
       * @brief Constructs an empty typed buffer.
       *
       * @post `GetNumElements()` returns `0`.
       */
      TBuffer() : CBuffer(0) {};

      /**
       * @brief Allocates storage for a number of elements.
       * @param nNumElements Number of elements to allocate.
       *
       * Negative counts and counts whose byte size exceeds `int` range result in an empty buffer.
       *
       * @post `GetNumElements()` returns the number of elements represented by the allocated byte size.
       * @failurecases Allocation may throw if `new[]` fails for a non-zero computed size.
       */
      TBuffer(int nNumElements) : CBuffer(GetCheckedByteSize(nNumElements))
      {
      }

      /**
       * @brief Wraps an existing typed buffer.
       * @param nNumElements Number of elements represented by `data`.
       * @param data Pointer to the first element, or `nullptr`.
       * @param bOwnsBuffer `true` if this instance assumes responsibility for deleting the buffer with `delete[]`; otherwise `false`.
       *
       * @ownership Ownership of `data` is transferred only when `bOwnsBuffer` is `true`.
       * @invariant The caller must provide storage compatible with deletion by `delete[]` when ownership is transferred.
       */
      TBuffer(int nNumElements, T* data, bool bOwnsBuffer) : CBuffer(GetCheckedByteSize(nNumElements), (char*)data, bOwnsBuffer)
      {
      }

      /**
       * @brief Constructs a `TBuffer` instance.
       */
      TBuffer(const TBuffer&) = delete;
      TBuffer& operator=(const TBuffer&) = delete;

      /**
       * @brief Returns mutable typed access to the underlying storage.
       * @return Pointer to the first element, or `nullptr` if the buffer is empty.
       *
       * @ownership The returned pointer follows the same ownership rules as the underlying `CBuffer`.
       */
      T* GetBuffer()
      {
        return reinterpret_cast<T*>(m_buffer);
      }

      /**
       * @brief Returns read-only typed access to the underlying storage.
       * @return Pointer to the first element, or `nullptr` if the buffer is empty.
       *
       * @ownership The returned pointer follows the same ownership rules as the underlying `CBuffer`.
       */
      const T* GetBuffer() const
      {
        return reinterpret_cast<const T*>(m_buffer);
      }

      /**
       * @brief Gets the number of represented elements.
       * @return Element count derived from the stored byte size.
       */
      int GetNumElements() const
      {
        return GetSize() / static_cast<int>(sizeof(T));
      }

      /**
       * @brief Reallocates the buffer for a new element count.
       * @param nNumElements New number of elements.
       *
       * Existing contents, if any, are discarded. Negative counts and counts whose byte size exceeds `int` range produce an empty buffer.
       *
       * @post The buffer is owned by this instance.
       * @post `GetNumElements()` reflects the new represented size.
       * @sideeffects May free previously owned memory and allocate new storage.
       * @failurecases Allocation may throw if `new[]` fails for a non-zero computed size.
       */
      void SetNumElements(int nNumElements)
      {
        const int nNewSize = GetCheckedByteSize(nNumElements);

        //clears out buffer
        if (m_own_Buffer)
        {
          delete[] m_buffer;
        }

        m_nSize = nNewSize;
        m_buffer = (m_nSize > 0) ? new char[m_nSize] : nullptr;
        m_own_Buffer = true;
      }

    private:
      /**
       * @brief Converts an element count to a checked byte size.
       * @param nNumElements Number of elements to represent.
       * @return Byte count for `nNumElements`, or `0` when the count is negative or overflows `int` range.
       */
      int GetCheckedByteSize(int nNumElements)
      {
        if (nNumElements < 0)
        {
          return 0;
        }

        const size_t nByteSize = static_cast<size_t>(nNumElements) * sizeof(T);
        if (nByteSize > static_cast<size_t>((std::numeric_limits<int>::max)()))
        {
          return 0;
        }

        return static_cast<int>(nByteSize);
      }
    };
  }
}

#endif

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
