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

#include <djvUI/IntDisplay.h>

#include <djvCore/SignalBlocker.h>

#include <QHBoxLayout>
#include <QPointer>
#include <QSpinBox>

namespace djv
{
    namespace UI
    {
        struct IntDisplay::Private
        {
            int value = 0;
            int min = 1;
            int max = 100;
            QPointer<QSpinBox> spinBox;
        };

        IntDisplay::IntDisplay(QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->spinBox = new QSpinBox;
            _p->spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
            _p->spinBox->setReadOnly(true);

            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->spinBox);

            widgetUpdate();
        }

        IntDisplay::~IntDisplay()
        {}
        
        int IntDisplay::value() const
        {
            return _p->value;
        }

        int IntDisplay::min() const
        {
            return _p->min;
        }

        int IntDisplay::max() const
        {
            return _p->max;
        }

        void IntDisplay::setValue(int value)
        {
            if (value == _p->value)
                return;
            _p->value = value;
            Q_EMIT valueChanged(_p->value);
            widgetUpdate();
        }

        void IntDisplay::setRange(int min, int max)
        {
            if (min == _p->min && max == _p->max)
                return;
            _p->min = min;
            _p->max = max;
            Q_EMIT rangeChanged(_p->min, _p->max);
            widgetUpdate();
        }

        void IntDisplay::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(_p->spinBox);
            _p->spinBox->setRange(_p->min, _p->max);
            _p->spinBox->setValue(_p->value);
        }

    } // namespace UI
} // namespace djv
