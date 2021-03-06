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

#include <djvCore/Util.h>

#include <QDialog>

#include <memory>

namespace djv
{
    namespace UI
    {
        //! This class provides a progress bar dialog.
        class ProgressDialog : public QDialog
        {
            Q_OBJECT

        public:
            explicit ProgressDialog(
                const QString & label = QString(),
                QWidget *       parent = nullptr);
            ~ProgressDialog() override;

            //! Get the label.
            const QString & label() const;

            //! Set the label.
            void setLabel(const QString &);

        public Q_SLOTS:
            //! Start the progress.
            void start(int);

        Q_SIGNALS:
            //! This signal is emitted when the progress is changed.
            void progressSignal(int);

            //! This signal is emitted when the progress is finished or cancelled.
            void finishedSignal();

        protected:
            void hideEvent(QHideEvent *) override;
            void timerEvent(QTimerEvent *) override;

        private Q_SLOTS:
            void rejectedCallback();

        private:
            void stopTimer();

            DJV_PRIVATE_COPY(ProgressDialog);

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace UI
} // namespace djv

