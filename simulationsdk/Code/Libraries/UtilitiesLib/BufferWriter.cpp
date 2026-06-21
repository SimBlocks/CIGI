//Copyright SimBlocks LLC 2016-2026
/**
 * @file BufferWriter.cpp
 * @brief Implements sequential buffer writing functionality.
 *
 * This file contains the implementation of the CBufferWriter class, which provides
 * methods for writing data sequentially into a buffer, tracking the used size, and
 * resetting the write position.
 *
 * @see sbio::utils::CBufferWriter
 */
#include "BufferWriter.h"

using namespace sbio::utils;

// Constructs a buffer writer with the specified size.
CBufferWriter::CBufferWriter(int nSize) : CBuffer(nSize)
{
  m_pCurrentBufferPtr = m_buffer;
}

// Destructor. Releases any owned memory.
CBufferWriter::~CBufferWriter()
{
}

// Gets the number of bytes written to the buffer.
int64_t CBufferWriter::GetInUseSize() const
{
  if (m_buffer == nullptr || m_pCurrentBufferPtr == nullptr)
  {
    return 0;
  }

  return static_cast<int64_t>(m_pCurrentBufferPtr - m_buffer);
}

// Resets the write pointer to the start of the buffer.
void CBufferWriter::Reset()
{
  m_pCurrentBufferPtr = m_buffer;
}

//The source code in this file is licensed under the MIT License. See the LICENSE text file for full terms.
//Refer all inquiries to sales@simblocks.io
//Copyright SimBlocks LLC 2016-2026
