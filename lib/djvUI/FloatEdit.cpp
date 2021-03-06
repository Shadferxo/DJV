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

#include <djvUI/FloatEdit.h>

#include <djvUI/FloatObject.h>

#include <djvCore/Debug.h>
#include <djvCore/SignalBlocker.h>

#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPointer>

namespace djv
{
    namespace UI
    {
        struct FloatEdit::Private
        {
            QPointer<FloatObject> object;
            QPointer<QDoubleSpinBox> spinBox;
        };

        FloatEdit::FloatEdit(QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->object = new FloatObject(this);

            _p->spinBox = new QDoubleSpinBox;
            _p->spinBox->setKeyboardTracking(false);

            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->spinBox);

            widgetUpdate();

            connect(
                _p->object,
                SIGNAL(valueChanged(float)),
                SLOT(valueCallback()));
            connect(
                _p->object,
                SIGNAL(minChanged(float)),
                SLOT(rangeCallback()));
            connect(
                _p->object,
                SIGNAL(maxChanged(float)),
                SLOT(rangeCallback()));
            connect(
                _p->object,
                SIGNAL(rangeChanged(float, float)),
                SLOT(rangeCallback()));
            connect(
                _p->object,
                SIGNAL(incChanged(float, float)),
                SLOT(widgetUpdate()));
            connect(
                _p->spinBox,
                SIGNAL(valueChanged(double)),
                SLOT(spinBoxCallback(double)));
        }

        FloatEdit::~FloatEdit()
        {}
        
        float FloatEdit::value() const
        {
            return _p->object->value();
        }

        float FloatEdit::min() const
        {
            return _p->object->min();
        }

        float FloatEdit::max() const
        {
            return _p->object->max();
        }

        FloatObject * FloatEdit::object() const
        {
            return _p->object;
        }

        void FloatEdit::setValue(float value)
        {
            _p->object->setValue(value);
        }

        void FloatEdit::setMin(float min)
        {
            _p->object->setMin(min);
        }

        void FloatEdit::setMax(float max)
        {
            _p->object->setMax(max);
        }

        void FloatEdit::setRange(float min, float max)
        {
            _p->object->setRange(min, max);
        }

        void FloatEdit::keyPressEvent(QKeyEvent * event)
        {
            switch (event->key())
            {
            case Qt::Key_Home:     _p->object->setToMin();       break;
            case Qt::Key_End:      _p->object->setToMax();       break;
            case Qt::Key_Left:
            case Qt::Key_Down:     _p->object->smallDecAction(); break;
            case Qt::Key_Right:
            case Qt::Key_Up:       _p->object->smallIncAction(); break;
            case Qt::Key_PageDown: _p->object->largeDecAction(); break;
            case Qt::Key_PageUp:   _p->object->largeIncAction(); break;
            default: break;
            }
        }

        void FloatEdit::valueCallback()
        {
            widgetUpdate();
            Q_EMIT valueChanged(_p->object->value());
        }

        void FloatEdit::rangeCallback()
        {
            widgetUpdate();
            Q_EMIT rangeChanged(_p->object->min(), _p->object->max());
        }

        void FloatEdit::spinBoxCallback(double value)
        {
            _p->object->setValue(static_cast<float>(value));
        }

        void FloatEdit::widgetUpdate()
        {
            Core::SignalBlocker signalBlocker(_p->spinBox);
            _p->spinBox->setRange(_p->object->min(), _p->object->max());
            _p->spinBox->setSingleStep(_p->object->smallInc());
            _p->spinBox->setValue(_p->object->value());
        }

    } // namespace UI
} // namespace djv
