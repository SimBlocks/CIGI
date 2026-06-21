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

using namespace sbio::utils;

// Constructs a buffer of the specified size.
CBuffer::CBuffer(int nSize) : m_nSize(nSize)
{
  if (m_nSize > 0)
  {
    m_buffer = new char[m_nSize];
  }
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
  char* p = m_buffer;
  m_buffer = nullptr;
  m_nSize = 0;
  m_own_Buffer = false;

  return p;
}

// Sets the buffer to a new memory block.
void CBuffer::Set(int nSize, void* data)
{
  if (m_buffer && m_own_Buffer)
  {
    delete[] m_buffer;
    m_buffer = nullptr;
  }

  m_nSize = nSize;
  m_buffer = (char*)data;
  m_own_Buffer = false;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
