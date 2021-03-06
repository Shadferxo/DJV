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

#include <djvAV/OpenGL.h>

#include <djvCore/Error.h>
#include <djvCore/Matrix.h>

namespace djv
{
    namespace AV
    {
        //! This class encapsulates an OpenGL shader.
        class OpenGLShader
        {
        public:
            OpenGLShader();
            ~OpenGLShader();

            //! Initialize the shader.
            //!
            //! Throws:
            //! - Core::Error
            void init(
                const QString & vertexSource,
                const QString & fragmentSource);

            //! Bind the shader.
            void bind();

            //! Get the shader vertex source.
            const QString & vertexSource() const;

            //! Get the shader fragment source.
            const QString & fragmentSource() const;

            //! Get the shader program ID.
            GLuint program() const;

            //! Set a uniform value.
            void setUniform(const QString&, int);

            //! Set a uniform value.
            void setUniform(const QString&, float);

            //! Set a uniform value.
            void setUniform(const QString&, const glm::mat4x4&);

        private:
            void del();

            struct Private;
            std::unique_ptr<Private> _p;
        };

    } // namespace AV
} // namespace djv
