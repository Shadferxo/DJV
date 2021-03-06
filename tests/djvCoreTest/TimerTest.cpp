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

#include <djvCoreTest/TimerTest.h>

#include <djvCore/Assert.h>
#include <djvCore/Debug.h>
#include <djvCore/Math.h>
#include <djvCore/Time.h>
#include <djvCore/Timer.h>

#include <QString>

using namespace djv::Core;

namespace djv
{
    namespace CoreTest
    {
        void TimerTest::run(int &, char **)
        {
            DJV_DEBUG("TimerTest::run");
            ctors();
            operators();
        }

        void TimerTest::ctors()
        {
            DJV_DEBUG("TimerTest::ctors");
            {
                const Timer timer;
                DJV_ASSERT(Math::fuzzyCompare(0.f, timer.seconds()));
                DJV_ASSERT(Math::fuzzyCompare(0.f, timer.fps()));
            }
            {
                Timer a;
                a.start();
                Time::msleep(1);
                a.check();
                Timer b(a);
                DJV_ASSERT(Math::fuzzyCompare(a.seconds(), b.seconds()));
                DJV_ASSERT(Math::fuzzyCompare(a.fps(), b.fps()));
            }
        }

        void TimerTest::operators()
        {
            DJV_DEBUG("TimerTest::operators");
            {
                Timer a;
                a.start();
                Time::msleep(1);
                a.check();
                Timer b;
                b = a;
                DJV_ASSERT(Math::fuzzyCompare(a.seconds(), b.seconds()));
                DJV_ASSERT(Math::fuzzyCompare(a.fps(), b.fps()));
            }
        }

    } // namespace CoreTest
} // namespace djv
