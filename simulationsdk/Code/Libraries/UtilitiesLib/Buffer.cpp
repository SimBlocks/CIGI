//Copyright SimBlocks LLC 2016-2026
/**
 * @file Buffer.cpp
 * @brief Implements buffer management for raw data.
 *
 * This file contains the implementation of the CBuffer class, which provides
 * memory management for raw character buffers, including allocation, deallocation,
 * ownership transfer, and size management.
 *
 * @see sbio::utils::CBuffer
 */
#include "Buffer.h"

#include <cstring>
#include <functional>

using namespace sbio::utils;

// Constructs a buffer of the specified size.
CBuffer::CBuffer(int nSize) : m_nSize(nSize)
{
  if (m_nSize > 0)
  {
    m_buffer = new char[m_nSize];
  }
}

CBuffer::CBuffer(std::vector<std::uint8_t>&& buffer) : m_own_Buffer(false)
{
  if (buffer.size() > static_cast<size_t>((std::numeric_limits<int>::max)()))
  {
    return;
  }

  m_nSize = static_cast<int>(buffer.size());
  m_ownedBytes = std::move(buffer);
  m_bRequiresCopyOnSteal = true;
  m_buffer = m_ownedBytes.empty() ? nullptr : reinterpret_cast<char*>(m_ownedBytes.data());
}

// Constructs a buffer with a specified size and existing data.
CBuffer::CBuffer(int nSize, char* buffer, bool bOwnsBuffer) : m_nSize(nSize), m_own_Buffer(bOwnsBuffer)
{
  m_buffer = (char*)buffer;
}

// Destructor. Releases owned memory.
CBuffer::~CBuffer()
{
  if (m_own_Buffer && m_buffer != nullptr)
  {
    delete[] m_buffer;
  }
}

// Clears the buffer, releasing any owned memory.
void CBuffer::Clear()
{
  if (m_own_Buffer && m_buffer != nullptr)
  {
    delete[] m_buffer;
  }

  m_buffer = nullptr;
  m_nSize = 0;
  std::vector<std::uint8_t>().swap(m_ownedBytes);
  m_bRequiresCopyOnSteal = false;
}

// Gets a pointer to the buffer.
char* CBuffer::GetBuffer()
{
  return m_buffer;
}

const char* CBuffer::GetBuffer() const
{
  return m_buffer;
}

// Gets the size of the buffer in bytes.
int CBuffer::GetSize() const
{
  return m_nSize;
}

// Checks if the buffer is empty.
bool CBuffer::IsEmpty() const
{
  return m_buffer == nullptr || m_nSize == 0;
}

// Transfers ownership of the buffer pointer to the caller.
char* CBuffer::StealPointer()
{
  if (m_bRequiresCopyOnSteal)
  {
    char* p = (m_nSize > 0) ? new char[m_nSize] : nullptr;
    if (m_nSize > 0)
    {
      std::memcpy(p, m_buffer, m_nSize);
    }
    std::vector<std::uint8_t>().swap(m_ownedBytes);
    m_bRequiresCopyOnSteal = false;
    m_buffer = nullptr;
    m_nSize = 0;
    return p;
  }

  char* p = m_buffer;
  m_buffer = nullptr;
  m_nSize = 0;
  m_own_Buffer = false;

  return p;
}

bool CBuffer::IsPointerInOwnedStorageRange(const void* data) const
{
  if (data == nullptr)
  {
    return false;
  }

  const auto contains = [data](const char* begin, size_t size)
  {
    const std::less<const void*> less;
    return begin != nullptr && !less(data, begin) && !less(begin + size, data);
  };

  if (m_bRequiresCopyOnSteal)
  {
    return contains(reinterpret_cast<const char*>(m_ownedBytes.data()), m_ownedBytes.capacity());
  }

  return m_own_Buffer && contains(m_buffer, m_nSize > 0 ? static_cast<size_t>(m_nSize) : 0);
}

// Sets the buffer to a new memory block.
void CBuffer::Set(int nSize, void* data)
{
  if (IsPointerInOwnedStorageRange(data))
  {
    return;
  }

  if (m_buffer && m_own_Buffer)
  {
    delete[] m_buffer;
    m_buffer = nullptr;
  }

  std::vector<std::uint8_t>().swap(m_ownedBytes);
  m_bRequiresCopyOnSteal = false;
  m_nSize = nSize;
  m_buffer = (char*)data;
  m_own_Buffer = false;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
