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

#include <djvUI/FileBrowserThumbnailSystem.h>

#include <djvUI/FileBrowserModel.h>
#include <djvUI/UIContext.h>

#include <djvAV/Image.h>
#include <djvAV/OpenGLImage.h>
#include <djvAV/PixelDataUtil.h>

#include <djvCore/DebugLog.h>
#include <djvCore/FileInfo.h>

#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QPixmap>
#include <QThread>

#include <atomic>
#include <mutex>
#include <thread>

namespace djv
{
    namespace UI
    {
        namespace
        {
            const size_t timeout = 10;

            struct InfoRequest
            {
                InfoRequest()
                {}

                InfoRequest(InfoRequest&& other) :
                    fileInfo(other.fileInfo),
                    promise(std::move(other.promise))
                {}

                InfoRequest& operator = (InfoRequest&& other)
                {
                    if (this != &other)
                    {
                        fileInfo = other.fileInfo;
                        promise = std::move(other.promise);
                    }
                    return *this;
                }

                Core::FileInfo fileInfo;
                std::promise<AV::IOInfo> promise;
            };

            struct PixmapRequest
            {
                PixmapRequest()
                {}

                PixmapRequest(PixmapRequest&& other) :
                    fileInfo(other.fileInfo),
                    thumbnailMode(other.thumbnailMode),
                    resolution(other.resolution),
                    proxy(other.proxy),
                    promise(std::move(other.promise))
                {}

                PixmapRequest& operator = (PixmapRequest&& other)
                {
                    if (this != &other)
                    {
                        fileInfo = other.fileInfo;
                        thumbnailMode = other.thumbnailMode;
                        resolution = other.resolution;
                        proxy = other.proxy;
                        promise = std::move(other.promise);
                    }
                    return *this;
                }

                Core::FileInfo fileInfo;
                FileBrowserModel::THUMBNAIL_MODE thumbnailMode = static_cast<FileBrowserModel::THUMBNAIL_MODE>(0);
                glm::ivec2 resolution;
                AV::PixelDataInfo::PROXY proxy = static_cast<AV::PixelDataInfo::PROXY>(0);
                std::promise<QPixmap> promise;
            };

        } // namespace

        struct FileBrowserThumbnailSystem::Private
        {
            Core::DebugLog * debugLog = nullptr;
            AV::IOFactory * ioFactory = nullptr;
            std::vector<InfoRequest> infoQueue;
            std::vector<PixmapRequest> pixmapQueue;
            std::condition_variable requestCV;
            std::mutex requestMutex;
            std::vector<InfoRequest> infoRequests;
            std::vector<PixmapRequest> pixmapRequests;
            QScopedPointer<QOffscreenSurface> offscreenSurface;
            QScopedPointer<QOpenGLContext> openGLContext;
            QScopedPointer<QOpenGLDebugLogger> openGLDebugLogger;
            std::unique_ptr<AV::OpenGLImage> openGLImage;
            std::atomic<bool> running;
        };

        FileBrowserThumbnailSystem::FileBrowserThumbnailSystem(const QPointer<UIContext> & context, QObject * parent) :
            QThread(parent),
            _p(new Private)
        {
            _p->debugLog = context->debugLog();
            _p->ioFactory = context->ioFactory();

            _p->offscreenSurface.reset(new QOffscreenSurface);
            QSurfaceFormat surfaceFormat = QSurfaceFormat::defaultFormat();
            surfaceFormat.setSwapBehavior(QSurfaceFormat::SingleBuffer);
            surfaceFormat.setSamples(1);
            _p->offscreenSurface->setFormat(surfaceFormat);
            _p->offscreenSurface->create();

            _p->openGLContext.reset(new QOpenGLContext);
            _p->openGLContext->setFormat(surfaceFormat);
            _p->openGLContext->create();
            _p->openGLContext->moveToThread(this);

            _p->openGLDebugLogger.reset(new QOpenGLDebugLogger);
            connect(
                _p->openGLDebugLogger.data(),
                &QOpenGLDebugLogger::messageLogged,
                this,
                &FileBrowserThumbnailSystem::debugLogMessage);

            _p->running = true;
        }

        FileBrowserThumbnailSystem::~FileBrowserThumbnailSystem()
        {}

        std::future<AV::IOInfo> FileBrowserThumbnailSystem::getInfo(const Core::FileInfo& fileInfo)
        {
            InfoRequest request;
            request.fileInfo = fileInfo;
            auto future = request.promise.get_future();
            std::unique_lock<std::mutex> lock(_p->requestMutex);
            _p->infoQueue.push_back(std::move(request));
            _p->requestCV.notify_one();
            return future;
        }

        std::future<QPixmap> FileBrowserThumbnailSystem::getPixmap(
            const Core::FileInfo& fileInfo,
            FileBrowserModel::THUMBNAIL_MODE thumbnailMode,
            const glm::ivec2 & resolution,
            AV::PixelDataInfo::PROXY proxy)
        {
            PixmapRequest request;
            request.fileInfo = fileInfo;
            request.thumbnailMode = thumbnailMode;
            request.resolution = resolution;
            request.proxy = proxy;
            auto future = request.promise.get_future();
            std::unique_lock<std::mutex> lock(_p->requestMutex);
            _p->pixmapQueue.push_back(std::move(request));
            _p->requestCV.notify_one();
            return future;
        }

        void FileBrowserThumbnailSystem::stop()
        {
            {
                std::unique_lock<std::mutex> lock(_p->requestMutex);
                _p->infoQueue.clear();
                _p->pixmapQueue.clear();
            }
            _p->running = false;
        }

        void FileBrowserThumbnailSystem::run()
        {
            _p->openGLContext->makeCurrent(_p->offscreenSurface.data());
            if (_p->openGLContext->format().testOption(QSurfaceFormat::DebugContext))
            {
                _p->openGLDebugLogger->initialize();
                _p->openGLDebugLogger->startLogging();
            }
            _p->openGLImage.reset(new AV::OpenGLImage);
            while (_p->running)
            {
                {
                    std::unique_lock<std::mutex> lock(_p->requestMutex);
                    _p->requestCV.wait_for(
                        lock,
                        std::chrono::milliseconds(timeout),
                        [this] { return _p->infoQueue.size() || _p->pixmapQueue.size(); });
                    _p->infoRequests = std::move(_p->infoQueue);
                    _p->pixmapRequests = std::move(_p->pixmapQueue);
                }
                if (_p->infoRequests.size())
                {
                    _handleInfoRequests();
                }
                if (_p->pixmapRequests.size())
                {
                    _handlePixmapRequests();
                }
            }
            _p->openGLImage.reset();
            _p->openGLDebugLogger->stopLogging();
            _p->openGLDebugLogger.reset();
            _p->openGLContext.reset();
        }

        void FileBrowserThumbnailSystem::debugLogMessage(const QOpenGLDebugMessage & message)
        {
            _p->debugLog->addMessage("djv::UI::FileBrowserThumbnailSystem", message.message());
        }

        void FileBrowserThumbnailSystem::_handleInfoRequests()
        {
            for (auto& request : _p->infoRequests)
            {
                AV::IOInfo info;
                try
                {
                    auto load = std::unique_ptr<AV::Load>(_p->ioFactory->load(request.fileInfo, info));
                }
                catch (const Core::Error&)
                {
                }
                request.promise.set_value(info);
            }
            _p->infoRequests.clear();
        }

        void FileBrowserThumbnailSystem::_handlePixmapRequests()
        {
            //DJV_DEBUG("FileBrowserThumbnailSystem::_handlePixmapRequests");
            for (auto& request : _p->pixmapRequests)
            {
                QPixmap pixmap;
                try
                {
                    //DJV_DEBUG_PRINT("file = " << request.fileInfo);
                    
                    AV::IOInfo info;
                    auto load = std::unique_ptr<AV::Load>(_p->ioFactory->load(request.fileInfo, info));
                    AV::Image image;
                    load->read(image);
                    //DJV_DEBUG_PRINT("image = " << image);

                    AV::Image tmp(AV::PixelDataInfo(request.resolution, image.pixel()));
                    AV::OpenGLImageOptions options;
                    options.xform.scale = glm::vec2(tmp.size()) / (glm::vec2(image.size() * AV::PixelDataUtil::proxyScale(image.info().proxy)));
                    options.colorProfile = image.colorProfile;
                    if (FileBrowserModel::THUMBNAIL_MODE_HIGH == request.thumbnailMode)
                    {
                        options.filter = AV::OpenGLImageFilter::filterHighQuality();
                    }
                    _p->openGLImage->copy(image, tmp, options);
                    pixmap = _p->openGLImage->toQt(tmp);
                }
                catch (const Core::Error & error)
                {
                    Q_FOREACH(auto m, error.messages())
                    {
                        _p->debugLog->addMessage(m.prefix, m.string);
                    }
                }
                request.promise.set_value(pixmap);
            }
            _p->pixmapRequests.clear();
        }

    } // namespace UI
} // namespace djv
