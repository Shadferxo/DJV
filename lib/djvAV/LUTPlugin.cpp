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

#include <djvAV/LUTPlugin.h>

#include <djvAV/LUTLoad.h>
#include <djvAV/LUTSave.h>

#include <djvCore/Assert.h>
#include <djvCore/CoreContext.h>

#include <QCoreApplication>

namespace djv
{
    namespace AV
    {
        LUTPlugin::LUTPlugin(const QPointer<Core::CoreContext> & context) :
            IOPlugin(context)
        {}

        void LUTPlugin::commandLine(QStringList & in)
        {
            QStringList tmp;
            QString     arg;
            try
            {
                while (!in.isEmpty())
                {
                    in >> arg;
                    if (qApp->translate("djv::AV::LUTPlugin", "-lut_type") == arg)
                    {
                        in >> _options.type;
                    }
                    else
                    {
                        tmp << arg;
                    }
                }
            }
            catch (const QString &)
            {
                throw arg;
            }
            in = tmp;
        }

        QString LUTPlugin::commandLineHelp() const
        {
            QStringList typeLabel;
            typeLabel << _options.type;
            return qApp->translate("djv::AV::LUTPlugin",
                "\n"
                "LUT Options\n"
                "\n"
                "    -lut_type (value)\n"
                "        Set the pixel type used when loading LUTs: %1. Default = "
                "%2.\n").
                arg(LUT::typeLabels().join(", ")).
                arg(typeLabel.join(", "));
        }

        QString LUTPlugin::pluginName() const
        {
            return LUT::staticName;
        }

        QStringList LUTPlugin::extensions() const
        {
            return LUT::staticExtensions;
        }

        QStringList LUTPlugin::option(const QString & in) const
        {
            QStringList out;
            if (0 == in.compare(options()[LUT::TYPE_OPTION], Qt::CaseInsensitive))
            {
                out << _options.type;
            }
            return out;
        }

        bool LUTPlugin::setOption(const QString & in, QStringList & data)
        {
            try
            {
                if (0 == in.compare(options()[LUT::TYPE_OPTION], Qt::CaseInsensitive))
                {
                    LUT::TYPE type = static_cast<LUT::TYPE>(0);
                    data >> type;
                    if (type != _options.type)
                    {
                        _options.type = type;
                        Q_EMIT optionChanged(in);
                    }
                }
            }
            catch (const QString &)
            {
                return false;
            }
            return true;
        }

        QStringList LUTPlugin::options() const
        {
            return LUT::optionsLabels();
        }

        std::unique_ptr<Load> LUTPlugin::createLoad(const Core::FileInfo & fileInfo) const
        {
            return std::unique_ptr<Load>(new LUTLoad(fileInfo, _options, context()));
        }

        std::unique_ptr<Save> LUTPlugin::createSave(const Core::FileInfo & fileInfo, const IOInfo & ioInfo) const
        {
            return std::unique_ptr<Save>(new LUTSave(fileInfo, ioInfo, _options, context()));
        }

    } // namespace AV
} // namespace djv
