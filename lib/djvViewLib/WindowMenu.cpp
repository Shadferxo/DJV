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

#include <djvViewLib/WindowMenu.h>

#include <djvViewLib/WindowActions.h>

#include <QApplication>
#include <QPointer>

namespace djv
{
    namespace ViewLib
    {
        WindowMenu::WindowMenu(
            const QPointer<AbstractActions> & actions,
            QWidget * parent) :
            AbstractMenu(actions, parent)
        {
            addAction(actions->action(WindowActions::NEW));
            addAction(actions->action(WindowActions::DUPLICATE));
            addAction(actions->action(WindowActions::CLOSE));
            addAction(actions->action(WindowActions::FIT));
            addAction(actions->action(WindowActions::FULL_SCREEN));
            addSeparator();
            addAction(actions->action(WindowActions::UI_VISIBLE));
            Q_FOREACH(QAction * action, actions->group(WindowActions::UI_COMPONENT_VISIBLE_GROUP)->actions())
                addAction(action);
            addAction(actions->action(WindowActions::DETACH_CONTROLS));

            setTitle(qApp->translate("djv::ViewLib::WindowMenu", "&Window"));
        }

        WindowMenu::~WindowMenu()
        {}

    } // namespace ViewLib
} // namespace djv
