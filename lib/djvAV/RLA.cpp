//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvAV/RLA.h>

#include <djvCore/Debug.h>
#include <djvCore/Memory.h>

namespace djv
{
    namespace AV
    {
        const QString RLA::staticName = "RLA";

        void RLA::readRle(
            Core::FileIO & io,
            quint8 *       out,
            int            size,
            int            channels,
            int            bytes)
        {
            //DJV_DEBUG("RLA::readRle");
            //DJV_DEBUG_PRINT("size = " << size);
            //DJV_DEBUG_PRINT("channels = " << channels);
            //DJV_DEBUG_PRINT("bytes = " << bytes);
            qint16 _size = 0;
            io.get16(&_size);
            //DJV_DEBUG_PRINT("io size = " << _size);
            const quint8 * start = io.mmapP();
            const quint8 * p = start;
            io.seek(_size);
            for (int b = 0; b < bytes; ++b)
            {
                quint8 * outP = out + (Core::Memory::LSB == Core::Memory::endian() ? (bytes - 1 - b) : b);
                const int outInc = channels * bytes;
                for (int i = 0; i < size;)
                {
                    int count = *((qint8 *)p);
                    ++p;
                    //DJV_DEBUG_PRINT("count = " << count);
                    if (count >= 0)
                    {
                        ++count;
                        for (int j = 0; j < count; ++j, outP += outInc)
                        {
                            *outP = *p;
                        }
                        ++p;
                    }
                    else
                    {
                        count = -count;
                        for (int j = 0; j < count; ++j, ++p, outP += outInc)
                        {
                            *outP = *p;
                        }
                    }
                    i += count;
                }
            }
            //DJV_DEBUG_PRINT("out = " << p - start);
        }

        void RLA::floatLoad(
            Core::FileIO & io,
            quint8 *       out,
            int            size,
            int            channels)
        {
            //DJV_DEBUG("RLA::floatLoad");
            //DJV_DEBUG_PRINT("size = " << size);
            //DJV_DEBUG_PRINT("channels = " << channels);
            qint16 _size = 0;
            io.get16(&_size);
            //DJV_DEBUG_PRINT("io size = " << _size);
            const quint8 * start = io.mmapP();
            const quint8 * p = start;
            io.seek(_size);
            const int outInc = channels * 4;
            if (Core::Memory::LSB == Core::Memory::endian())
            {
                for (int i = 0; i < size; ++i, p += 4, out += outInc)
                {
                    out[0] = p[3];
                    out[1] = p[2];
                    out[2] = p[1];
                    out[3] = p[0];
                }
            }
            else
            {
                for (int i = 0; i < size; ++i, p += 4, out += outInc)
                {
                    out[0] = p[0];
                    out[1] = p[1];
                    out[2] = p[2];
                    out[3] = p[3];
                }
            }
            //DJV_DEBUG_PRINT("out = " << p - start);
        }

        void RLA::skip(Core::FileIO & io)
        {
            qint16 size = 0;
            io.get16(&size);
            io.seek(size);
        }

    } // namespace AV
} // namespace djv
