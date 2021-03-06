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

#include <djvAV/Cineon.h>

#include <djvCore/Assert.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        const QString Cineon::staticName = "Cineon";

        const QStringList & Cineon::colorProfileLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::Cineon", "Auto") <<
                qApp->translate("djv::AV::Cineon", "None") <<
                qApp->translate("djv::AV::Cineon", "Film Print");
            DJV_ASSERT(data.count() == COLOR_PROFILE_COUNT);
            return data;
        }

        PixelData Cineon::linearToFilmPrintLut(const LinearToFilmPrint & value)
        {
            //DJV_DEBUG("Cineon::linearToFilmPrintLut");
            //DJV_DEBUG_PRINT("black = " << value.black);
            //DJV_DEBUG_PRINT("white = " << value.white);
            //DJV_DEBUG_PRINT("gamma = " << value.gamma);
            PixelData out(PixelDataInfo(1024, 1, Pixel::L_F32));
            const int size = out.w();
            //DJV_DEBUG_PRINT("size = " << size);
            Pixel::F32_T * data = reinterpret_cast<Pixel::F32_T *>(out.data());
            const float gain =
                1.f / (
                    1.f - Core::Math::pow(
                        Core::Math::pow(10.f, (value.black - value.white) * .002f / .6f),
                        value.gamma / 1.7f));
            const float offset = gain - 1.f;
            //DJV_DEBUG_PRINT("gain = " << gain * 255);
            //DJV_DEBUG_PRINT("offset = " << offset * 255);
            for (int i = 0; i < size; ++i)
            {
                data[i] = i / Pixel::F32_T(size - 1);
            }
            for (int i = 0; i < size; ++i)
            {
                data[i] = Pixel::F32_T(
                    value.white / 1023.f +
                    Core::Math::log10(
                        Core::Math::pow((data[i] + offset) / gain, 1.7f / value.gamma)) /
                        (2.048f / .6f));
                //DJV_DEBUG_PRINT("lut[" << i << "] = " <<
                //    data[i] << " " << static_cast<int>(data[i] * 1024));
            }
            return out;
        }

        PixelData Cineon::filmPrintToLinearLut(const FilmPrintToLinear & value)
        {
            //DJV_DEBUG("Cineon::filmPrintToLinearLut");
            //DJV_DEBUG_PRINT("black = " << value.black);
            //DJV_DEBUG_PRINT("white = " << value.white);
            //DJV_DEBUG_PRINT("gamma = " << value.gamma);
            //DJV_DEBUG_PRINT("soft clip = " << value.softClip);
            PixelData out(PixelDataInfo(1024, 1, Pixel::L_F32));
            const int size = out.w();
            //DJV_DEBUG_PRINT("size = " << size);
            Pixel::F32_T * data = reinterpret_cast<Pixel::F32_T *>(out.data());
            const float gain =
                1.f / (
                    1.f - Core::Math::pow(
                        Core::Math::pow(10.f, (value.black - value.white) * .002f / .6f),
                        value.gamma / 1.7f));
            const float offset = gain - 1.f;
            //DJV_DEBUG_PRINT("gain = " << gain * 255);
            //DJV_DEBUG_PRINT("offset = " << offset * 255);
            const int breakPoint = value.white - value.softClip;
            const float kneeOffset =
                Core::Math::pow(
                    Core::Math::pow(10.f, (breakPoint - value.white) * .002f / .6f),
                    value.gamma / 1.7f
                ) *
                gain - offset;
            const float kneeGain =
                (
                (255 - (kneeOffset * 255)) /
                    Core::Math::pow(5.f * value.softClip, value.softClip / 100.f)
                    ) / 255.f;
            //DJV_DEBUG_PRINT("break point = " << breakPoint);
            //DJV_DEBUG_PRINT("knee offset = " << kneeOffset * 255);
            //DJV_DEBUG_PRINT("knee gain = " << kneeGain * 255);
            for (int i = 0; i < size; ++i)
            {
                data[i] = i / Pixel::F32_T(size - 1);
            }
            for (int i = 0; i < size; ++i)
            {
                const int tmp = static_cast<int>(data[i] * 1023.f);
                if (tmp < value.black)
                {
                    data[i] = 0.f;
                }
                else if (tmp > breakPoint)
                {
                    data[i] = Pixel::F32_T((Core::Math::pow(
                        static_cast<float>(tmp - breakPoint),
                        value.softClip / 100.f) *
                        kneeGain * 255 + kneeOffset * 255) / 255.f);
                }
                else
                {
                    data[i] = Pixel::F32_T(Core::Math::pow(
                        Core::Math::pow(10.f, (tmp - value.white) * .002f / .6f),
                        value.gamma / 1.7f) * gain - offset);
                }
                //DJV_DEBUG_PRINT("lut[" << i << "] = " <<  data[i] << " " << static_cast<int>(data[i] * 255.f));
            }
            return out;
        }

        const QStringList & Cineon::tagLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::Cineon", "Source Offset") <<
                qApp->translate("djv::AV::Cineon", "Source File") <<
                qApp->translate("djv::AV::Cineon", "Source Time") <<
                qApp->translate("djv::AV::Cineon", "Source Input Device") <<
                qApp->translate("djv::AV::Cineon", "Source Input Model") <<
                qApp->translate("djv::AV::Cineon", "Source Input Serial") <<
                qApp->translate("djv::AV::Cineon", "Source Input Pitch") <<
                qApp->translate("djv::AV::Cineon", "Source Gamma") <<
                qApp->translate("djv::AV::Cineon", "Film Format") <<
                qApp->translate("djv::AV::Cineon", "Film Frame") <<
                qApp->translate("djv::AV::Cineon", "Film Frame Rate") <<
                qApp->translate("djv::AV::Cineon", "Film Frame ID") <<
                qApp->translate("djv::AV::Cineon", "Film Slate");
            DJV_ASSERT(data.count() == TAG_COUNT);
            return data;
        }

        const QStringList & Cineon::optionsLabels()
        {
            static const QStringList data = QStringList() <<
                qApp->translate("djv::AV::Cineon", "Input Color Profile") <<
                qApp->translate("djv::AV::Cineon", "Input Film Print") <<
                qApp->translate("djv::AV::Cineon", "Output Color Profile") <<
                qApp->translate("djv::AV::Cineon", "Output Film Print");
            DJV_ASSERT(data.count() == OPTIONS_COUNT);
            return data;
        }

    } // namespace AV

    bool operator == (
        const AV::Cineon::LinearToFilmPrint & a,
        const AV::Cineon::LinearToFilmPrint & b)
    {
        return
            a.black == b.black &&
            a.white == b.white &&
            a.gamma == b.gamma;
    }

    bool operator == (
        const AV::Cineon::FilmPrintToLinear & a,
        const AV::Cineon::FilmPrintToLinear & b)
    {
        return
            a.black == b.black &&
            a.white == b.white &&
            a.gamma == b.gamma &&
            a.softClip == b.softClip;
    }

    bool operator != (
        const AV::Cineon::LinearToFilmPrint & a,
        const AV::Cineon::LinearToFilmPrint & b)
    {
        return !(a == b);
    }

    bool operator != (
        const AV::Cineon::FilmPrintToLinear & a,
        const AV::Cineon::FilmPrintToLinear & b)
    {
        return !(a == b);
    }

    QStringList & operator >> (QStringList & in, AV::Cineon::LinearToFilmPrint & out)
    {
        return in >>
            out.black >>
            out.white >>
            out.gamma;
    }

    QStringList & operator << (QStringList & out, const AV::Cineon::LinearToFilmPrint & in)
    {
        return out <<
            in.black <<
            in.white <<
            in.gamma;
    }

    QStringList & operator >> (QStringList & in, AV::Cineon::FilmPrintToLinear & out)
    {
        return in >>
            out.black >>
            out.white >>
            out.gamma >>
            out.softClip;
    }

    QStringList & operator << (QStringList & out, const AV::Cineon::FilmPrintToLinear & in)
    {
        return out <<
            in.black <<
            in.white <<
            in.gamma <<
            in.softClip;
    }

    _DJV_STRING_OPERATOR_LABEL(AV::Cineon::COLOR_PROFILE, AV::Cineon::colorProfileLabels())

} // namespace djv
