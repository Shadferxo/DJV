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

#pragma once

#include <djvCore/Error.h>
#include <djvCore/Util.h>

#include <QObject>
#include <QPointer>
#include <QString>

#include <memory>

#if defined DJV_WINDOWS
#undef ERROR
#endif // DJV_WINDOWS

namespace djv
{
    namespace Core
    {
        class CoreContext;

        //! This class provides the base functionality for plugins.
        //!
        //! \todo Document the plugin naming conventions.
        class Plugin
        {
        public:
            Plugin(const QPointer<CoreContext> &);
            virtual ~Plugin() = 0;

            //! Initialize the plugin.
            //!
            //! Throws:
            //! - Core::Error
            virtual void initPlugin() {}

            //! Release the plugin.
            virtual void releasePlugin() {}

            //! Get the plugin name.
            virtual QString pluginName() const = 0;

            //! Get the context.
            const QPointer<CoreContext>& context() const;

        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This class provides the base functionality for plugin factories.
        class PluginFactory : public QObject
        {
            Q_OBJECT

        public:
            PluginFactory(
                const QPointer<CoreContext> & context,
                const QStringList & searchPath,
                const QString & pluginEntry,
                const QString & pluginPrefix = "djv",
                const QString & pluginSuffix = "Plugin",
                QObject * parent = nullptr);
            virtual ~PluginFactory() = 0;

            //! Get the list of plugins.
            QList<Plugin *> plugins() const;

            //! Get a plugin by name.
            Plugin * plugin(const QString &) const;

            //! Get the list of plugin names.
            QStringList names() const;

            //! Add a plugin.
            virtual void addPlugin(Plugin *);

            //! This enumeration provides error codes.
            enum ERROR
            {
                ERROR_OPEN,
                ERROR_LOAD,

                ERROR_COUNT
            };

            //! Get the error code labels.
            static const QStringList & errorLabels();

        private:
            DJV_PRIVATE_COPY(PluginFactory);

            struct Private;
            std::unique_ptr<Private> _p;
        };

        //! This typedef provides a plugin entry point.
        typedef Plugin * (djvCorePluginEntry)(const QPointer<CoreContext> &);

    } // namespace Core
} // namespace djv

//! This macro provides a plugin entry point export.
#if defined(DJV_WINDOWS)
#define DJV_PLUGIN_EXPORT __declspec(dllexport)
#else
#define DJV_PLUGIN_EXPORT
#endif
