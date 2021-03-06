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

#include <djvViewLib/AbstractGroup.h>

#include <djvAV/IO.h>
#include <djvAV/Pixel.h>

#include <memory>

class QAction;

namespace djv
{
    namespace ViewLib
    {
        class FileCacheRef;

        //! This class provides the file group. The file group encapsulates all
        //! of thefunctionality relating to files such as the currently opened file,
        //! options used when loading files, etc.
        class FileGroup : public AbstractGroup
        {
            Q_OBJECT

        public:
            FileGroup(
                const QPointer<FileGroup> & copy,
                const QPointer<Session> &,
                const QPointer<ViewContext> &);
            ~FileGroup() override;

            //! Get the currently opened file.
            const Core::FileInfo & fileInfo() const;

            //! Get the layer to load.
            int layer() const;

            //! Get the proxy scale.
            AV::PixelDataInfo::PROXY proxy() const;

            //! Get whther images are converted to 8-bits.
            bool hasU8Conversion() const;

            //! Get whether the cache is enabled.
            bool isCacheEnabled() const;

            //! Get whether the cache pre-load is enabled.
            bool hasPreload() const;

            //! Get whether the cache pre-load is active.
            bool isPreloadActive() const;

            //! Get the cache pre-load frame.
            qint64 preloadFrame() const;

            //! Get an image.
            std::shared_ptr<AV::Image> image(qint64 frame) const;

            //! Get image I/O information.
            const AV::IOInfo & ioInfo() const;

            QPointer<QMenu> createMenu() const override;
            QPointer<QToolBar> createToolBar() const override;

        public Q_SLOTS:
            //! Open a file.
            void open(const djv::Core::FileInfo &);

            //! Set the layer to load.
            void setLayer(int);

            //! Set the proxy scale.
            void setProxy(djv::AV::PixelDataInfo::PROXY);

            //! Set whether images are converted to 8-bits.
            void setU8Conversion(bool);

            //! Set whether the cache is enabled.
            void setCacheEnabled(bool);

            //! Set whether the cache pre-load is enabled.
            void setPreload(bool);

            //! Set whether the cache pre-load is active.
            void setPreloadActive(bool);

            //! Set the cache pre-load frame.
            void setPreloadFrame(qint64);

        Q_SIGNALS:
            //! This signal is emitted when a new file is opened.
            void fileInfoChanged(const djv::Core::FileInfo &);

            //! This signal is emitted when the I/O information is changed.
            void ioInfoChanged(const djv::AV::IOInfo &);

            //! This signal is emitted when the current image is changed.
            void imageChanged();

            //! This signal is emitted to store the current frame.
            void setFrameStore();

            //! This signal is emitted to reload the frame.
            void reloadFrame();

            //! This signal is emitted to export a sequence.
            void exportSequence(const djv::Core::FileInfo &);

            //! This signal is emitted to export a frame.
            void exportFrame(const djv::Core::FileInfo &);

        protected:
            void timerEvent(QTimerEvent *) override;

        private Q_SLOTS:
            void openCallback();
            void openCallback(const djv::Core::FileInfo &);
            void recentCallback(QAction *);
            void reloadCallback();
            void reloadFrameCallback();
            void closeCallback();
            void exportSequenceCallback();
            void exportFrameCallback();
            void layerCallback(QAction *);
            void layerPrevCallback();
            void layerNextCallback();
            void proxyCallback(QAction *);
            void cacheClearCallback();
            void messagesCallback();
            void prefsCallback();
            void debugLogCallback();

            void preloadUpdate();
            void update();

        private:
            void cacheDel();

            DJV_PRIVATE_COPY(FileGroup);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace ViewLib
} // namespace djv
