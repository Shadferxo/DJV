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

#include <djvUI/IntSlider.h>

#include <djvUI/IntObject.h>

#include <djvCore/Debug.h>
#include <djvCore/SignalBlocker.h>

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPointer>
#include <QSlider>
#include <QStyle>

namespace djv
{
    namespace UI
    {
        namespace
        {
            //! \todo Should we implement our own slider for better control of the position
            //! and value?
            class Slider : public QSlider
            {
            public:
                Slider() :
                    QSlider(Qt::Horizontal)
                {}

            protected:
                void mousePressEvent(QMouseEvent * event) override
                {
                    const int t = style()->pixelMetric(QStyle::PM_SliderThickness);

                    setValue(QStyle::sliderValueFromPosition(
                        minimum(),
                        maximum(),
                        event->pos().x() - t / 2,
                        width() - t));
                }

                void mouseMoveEvent(QMouseEvent * event) override
                {
                    const int t = style()->pixelMetric(QStyle::PM_SliderThickness);

                    setValue(QStyle::sliderValueFromPosition(
                        minimum(),
                        maximum(),
                        event->pos().x() - t / 2,
                        width() - t));
                }
            };

        } // namespace

        struct IntSlider::Private
        {
            QPointer<IntObject> object;
            QPointer<Slider> slider;
        };

        IntSlider::IntSlider(QWidget * parent) :
            QWidget(parent),
            _p(new Private)
        {
            _p->object = new IntObject(this);
            _p->slider = new Slider;

            QHBoxLayout * layout = new QHBoxLayout(this);
            layout->setMargin(0);
            layout->addWidget(_p->slider);

            _p->object->setRange(1, 100);

            widgetUpdate();

            connect(
                _p->object,
                SIGNAL(valueChanged(int)),
                SLOT(valueCallback()));
            connect(
                _p->object,
                SIGNAL(minChanged(int)),
                SLOT(rangeCallback()));
            connect(
                _p->object,
                SIGNAL(maxChanged(int)),
                SLOT(rangeCallback()));
            connect(
                _p->object,
                SIGNAL(rangeChanged(int, int)),
                SLOT(rangeCallback()));
            connect(
                _p->slider,
                SIGNAL(valueChanged(int)),
                SLOT(sliderCallback(int)));
        }

        IntSlider::~IntSlider()
        {}
        
        int IntSlider::value() const
        {
            return _p->object->value();
        }

        int IntSlider::min() const
        {
            return _p->object->min();
        }

        int IntSlider::max() const
        {
            return _p->object->max();
        }

        IntObject * IntSlider::object() const
        {
            return _p->object;
        }

        void IntSlider::setValue(int value)
        {
            _p->object->setValue(value);
        }

        void IntSlider::setMin(int min)
        {
            _p->object->setMin(min);
        }

        void IntSlider::setMax(int max)
        {
            _p->object->setMax(max);
        }

        void IntSlider::setRange(int min, int max)
        {
            _p->object->setRange(min, max);
        }

        void IntSlider::keyPressEvent(QKeyEvent * event)
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

        void IntSlider::valueCallback()
        {
            widgetUpdate();
            Q_EMIT valueChanged(_p->object->value());
        }

        void IntSlider::rangeCallback()
        {
            widgetUpdate();
            Q_EMIT rangeChanged(_p->object->min(), _p->object->max());
        }

        void IntSlider::sliderCallback(int value)
        {
            setValue(value);
        }

        void IntSlider::widgetUpdate()
        {
            //DJV_DEBUG("IntSlider::widgetUpdate");
            //DJV_DEBUG_PRINT("value = " << _p->object->value());
            //DJV_DEBUG_PRINT("min = " << _p->object->min());
            //DJV_DEBUG_PRINT("max = " << _p->object->max());
            Core::SignalBlocker signalBlocker(_p->slider);
            _p->slider->setRange(_p->object->min(), _p->object->max());
            _p->slider->setValue(_p->object->value());
        }

    } // namespace UI
} // namespace djv
