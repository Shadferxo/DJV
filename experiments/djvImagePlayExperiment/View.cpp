//------------------------------------------------------------------------------
// Copyright (c) 2004-2015 Darby Johnston
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

#include <View.h>

#include <Context.h>
#include <Load.h>

#include <djvCore/Error.h>

#include <QOpenGLFunctions>
#include <QMutexLocker>

//------------------------------------------------------------------------------
// View
//------------------------------------------------------------------------------

View::View(Context * context) :
    _shader (new QOpenGLShaderProgram),
    _context(context)
{}

void View::setInfo(const djvPixelDataInfo & info)
{
    _info = info;
}

void View::initializeGL()
{
    //DJV_DEBUG("View::initializeGL");
    initializeOpenGLFunctions();
    try
    {
        _shader->addShaderFromSourceCode(
            QOpenGLShader::Vertex,
            "void main(void)\n"
            "{\n"
            "    gl_FrontColor  = gl_Color;\n"
            "    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
            "    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
            "}\n");
        _shader->addShaderFromSourceCode(
            QOpenGLShader::Fragment,
            "uniform sampler2D inTexture;\n"
            "\n"
            "void main(void)\n"
            "{\n"
            "    vec4 color;\n"
            "\n"
            "    color = texture2D(inTexture, gl_TexCoord[0].st);"
            "\n"
            "    gl_FragColor = color;\n"
            "}\n");
    }
    catch (const djvError &)
    {
    }
}

void View::paintGL()
{
    //DJV_DEBUG("View::paintGL");

    //QMutexLocker locker(_context->mutex());

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width(), 0, height(), -1.f, 1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, width(), height());

    _shader->bind();
    if (_context->load()->frontTexture() &&
        _context->load()->frontTexture()->isCreated())
    {
        glActiveTexture(GL_TEXTURE0);
        glUniform1f(
            glGetUniformLocation(_shader->programId(), "inTexture"),
            0);
        _context->load()->frontTexture()->bind();
    }

    djvBox2i box(_info.size);
    float u [] = { 0.f, 1.f };
    float v [] = { 1.f, 0.f };
    const glm::vec2 uv[] =
    {
        glm::vec2(u[0], v[0]),
        glm::vec2(u[0], v[1]),
        glm::vec2(u[1], v[1]),
        glm::vec2(u[1], v[0])
    };
    glBegin(GL_QUADS);
    glTexCoord2d(uv[0].x, uv[0].y);
    glVertex2i  (box.x, box.y);
    glTexCoord2d(uv[1].x, uv[1].y);
    glVertex2i  (box.x, box.y + box.h);
    glTexCoord2d(uv[2].x, uv[2].y);
    glVertex2i  (box.x + box.w, box.y + box.h);
    glTexCoord2d(uv[3].x, uv[3].y);
    glVertex2i  (box.x + box.w, box.y);
    glEnd();

    //glFlush();
}
