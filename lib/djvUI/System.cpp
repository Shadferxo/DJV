//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
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

#include <djvUI/System.h>

#include <djvUI/GeneralSettings.h>
#include <djvUI/FontSettings.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/StyleSettings.h>
#include <djvUI/Style.h>

#include <djvAV/Render2DSystem.h>
#include <djvAV/System.h>

#include <djvCore/Context.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct System::Private
        {
            std::vector<std::shared_ptr<ISystem> > uiSystems;
            std::shared_ptr<GeneralSettings> generalSettings;
            std::shared_ptr<FontSettings> fontSettings;
            std::shared_ptr<StyleSettings> styleSettings;
            std::shared_ptr<Style> style;
        };

        void System::_init(Context * context)
        {
            ISystem::_init("djv::UI::System", context);
            
            _p->uiSystems.push_back(AV::System::create(context));
            _p->uiSystems.push_back(SettingsSystem::create(context));
            
            _p->generalSettings = GeneralSettings::create(context);
            _p->fontSettings = FontSettings::create(context);
            _p->styleSettings = StyleSettings::create(context);
            
            _p->uiSystems.push_back(_p->style = Style::create(context));
        }

        System::System() :
            _p(new Private)
        {}

        System::~System()
        {
            DJV_PRIVATE_PTR();
            while (p.uiSystems.size())
            {
                p.uiSystems.pop_back();
            }
        }

        std::shared_ptr<System> System::create(Context * context)
        {
            auto out = std::shared_ptr<System>(new System);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<GeneralSettings> System::getGeneralSettings() const
        {
            return _p->generalSettings;
        }

        const std::shared_ptr<FontSettings> System::getFontSettings() const
        {
            return _p->fontSettings;
        }

        const std::shared_ptr<StyleSettings> System::getStyleSettings() const
        {
            return _p->styleSettings;
        }

        const std::shared_ptr<Style> System::getStyle() const
        {
            return _p->style;
        }

    } // namespace UI
} // namespace djv

