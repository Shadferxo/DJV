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

#include <djvUI/Core.h>

#include <djvCore/FileInfo.h>
#include <djvCore/Util.h>

#include <QWidget>

#include <memory>

namespace djv
{
    namespace UI
    {
        class UIContext;

        //! This class provides a file name edit widget.
        class FileEdit : public QWidget
        {
            Q_OBJECT

            //! This property holds the file information.    
            Q_PROPERTY(
                Core::FileInfo fileInfo
                READ           fileInfo
                WRITE          setFileInfo
                NOTIFY         fileInfoChanged)
    
        public:
            explicit FileEdit(const QPointer<UIContext> &, QWidget * parent = nullptr);
            ~FileEdit() override;

            //! Get the file information.
            const Core::FileInfo & fileInfo() const;

        public Q_SLOTS:
            //! Set the file information.
            void setFileInfo(const djv::Core::FileInfo &);

        Q_SIGNALS:
            //! This signal is emitted when the file information is changed.
            void fileInfoChanged(const djv::Core::FileInfo &);

        protected:
            bool event(QEvent *) override;

        private Q_SLOTS:
            void editCallback();
            void buttonCallback();
            void clearCallback();

        private:
            void styleUpdate();
            void widgetUpdate();

            DJV_PRIVATE_COPY(FileEdit);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv
