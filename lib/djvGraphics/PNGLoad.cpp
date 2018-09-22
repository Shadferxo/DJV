//------------------------------------------------------------------------------
// Copyright (c) 2004-2018 Darby Johnston
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

#include <djvGraphics/PNGLoad.h>

#include <djvGraphics/Image.h>
#include <djvGraphics/PixelDataUtil.h>

#include <djvCore/Error.h>
#include <djvCore/StringUtil.h>

namespace djv
{
    namespace Graphics
    {
        PNGLoad::PNGLoad(Core::CoreContext * context) :
            ImageLoad(context),
            _f(0),
            _png(0),
            _pngInfo(0),
            _pngInfoEnd(0)
        {}

        PNGLoad::~PNGLoad()
        {
            close();
        }

        void PNGLoad::open(const Core::FileInfo & in, ImageIOInfo & info)
        {
            //DJV_DEBUG("PNGLoad::open");
            //DJV_DEBUG_PRINT("in = " << in);
            _file = in;
            _open(_file.fileName(_file.sequence().start()), info);
            if (Core::FileInfo::SEQUENCE == _file.type())
            {
                info.sequence.frames = _file.sequence().frames;
            }
        }

        namespace
        {

            bool pngScanline(png_structp png, quint8 * out)
            {
                if (setjmp(png_jmpbuf(png)))
                {
                    return false;
                }
                png_read_row(png, out, 0);
                return true;
            }

            bool pngEnd(png_structp png, png_infop pngInfo)
            {
                if (setjmp(png_jmpbuf(png)))
                {
                    return false;
                }
                png_read_end(png, pngInfo);
                return true;
            }

        } // namespace

        void PNGLoad::read(Image & image, const ImageIOFrameInfo & frame)
        {
            //DJV_DEBUG("PNGLoad::read");
            //DJV_DEBUG_PRINT("frame = " << frame);

            image.colorProfile = ColorProfile();
            image.tags = ImageTags();

            // Open the file.
            const QString fileName =
                _file.fileName(frame.frame != -1 ? frame.frame : _file.sequence().start());
            //DJV_DEBUG_PRINT("file name = " << fileName);
            ImageIOInfo info;
            _open(fileName, info);

            // Read the file.
            PixelData * data = frame.proxy ? &_tmp : &image;
            data->set(info);
            for (int y = 0; y < info.size.y; ++y)
            {
                if (!pngScanline(_png, data->data(0, data->h() - 1 - y)))
                {
                    throw Core::Error(
                        PNG::staticName,
                        _pngError.msg);
                }
            }
            pngEnd(_png, _pngInfoEnd);

            // Proxy scale the image.
            if (frame.proxy)
            {
                info.size = PixelDataUtil::proxyScale(info.size, frame.proxy);
                info.proxy = frame.proxy;
                image.set(info);
                PixelDataUtil::proxyScale(_tmp, image, frame.proxy);
            }

            //DJV_DEBUG_PRINT("image = " << image);

            close();
        }

        void PNGLoad::close()
        {
            if (_png || _pngInfo || _pngInfoEnd)
            {
                png_destroy_read_struct(
                    _png ? &_png : 0,
                    _pngInfo ? &_pngInfo : 0,
                    _pngInfoEnd ? &_pngInfoEnd : 0);
                _png = 0;
                _pngInfo = 0;
                _pngInfoEnd = 0;
            }

            if (_f)
            {
                ::fclose(_f);
                _f = 0;
            }
        }

        namespace
        {

            bool pngOpen(
                FILE *      f,
                png_structp png,
                png_infop * pngInfo,
                png_infop * pngInfoEnd)
            {
                if (setjmp(png_jmpbuf(png)))
                {
                    return false;
                }
                *pngInfo = png_create_info_struct(png);
                if (!*pngInfo)
                {
                    return false;
                }
                *pngInfoEnd = png_create_info_struct(png);
                if (!*pngInfoEnd)
                {
                    return false;
                }
                quint8 tmp[8];
                if (::fread(tmp, 8, 1, f) != 1)
                {
                    return false;
                }
                if (png_sig_cmp(tmp, 0, 8))
                {
                    return false;
                }
                png_init_io(png, f);
                png_set_sig_bytes(png, 8);
                png_read_info(png, *pngInfo);
                if (png_get_interlace_type(png, *pngInfo) != PNG_INTERLACE_NONE)
                {
                    return false;
                }
                png_set_expand(png);
                //png_set_gray_1_2_4_to_8(png);
                png_set_palette_to_rgb(png);
                png_set_tRNS_to_alpha(png);
                return true;
            }

        } // namespace

        void PNGLoad::_open(const QString & in, ImageIOInfo & info)
        {
            //DJV_DEBUG("PNGLoad::_open");
            //DJV_DEBUG_PRINT("in = " << in);

            close();

            // Initialize libpng.
            _png = png_create_read_struct(
                PNG_LIBPNG_VER_STRING,
                &_pngError,
                djvPNGError,
                djvPNGWarning);
            if (!_png)
            {
                throw Core::Error(
                    PNG::staticName,
                    _pngError.msg);
            }

            // Open the file.
#if defined(DJV_WINDOWS)
            ::fopen_s(&_f, in.toLatin1().data(), "rb");
#else
            _f = ::fopen(in.toLatin1().data(), "rb");
#endif
            if (!_f)
            {
                throw Core::Error(
                    PNG::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_OPEN]);
            }
            if (!pngOpen(_f, _png, &_pngInfo, &_pngInfoEnd))
            {
                throw Core::Error(
                    PNG::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_OPEN]);
            }

            // Get file information.
            info.fileName = in;
            info.size = glm::ivec2(
                png_get_image_width(_png, _pngInfo),
                png_get_image_height(_png, _pngInfo));
            int channels = png_get_channels(_png, _pngInfo);
            if (png_get_color_type(_png, _pngInfo) == PNG_COLOR_TYPE_PALETTE)
            {
                channels = 3;
            }
            if (png_get_valid(_png, _pngInfo, PNG_INFO_tRNS))
            {
                ++channels;
            }
            //DJV_DEBUG_PRINT("channels = " << channels);
            int bitDepth = png_get_bit_depth(_png, _pngInfo);
            if (bitDepth < 8)
            {
                bitDepth = 8;
            }
            //DJV_DEBUG_PRINT("bit depth = " << bitDepth);
            if (!Pixel::pixel(channels, bitDepth, Pixel::INTEGER, info.pixel))
            {
                throw Core::Error(
                    PNG::staticName,
                    ImageIO::errorLabels()[ImageIO::ERROR_UNSUPPORTED]);
            }

            // Set the endian.
            if (bitDepth >= 16 && Core::Memory::LSB == Core::Memory::endian())
            {
                png_set_swap(_png);
            }
        }

    } // namespace Graphics
} // namespace djv
