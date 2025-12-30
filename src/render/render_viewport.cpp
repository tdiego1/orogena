/**************************************************************************************************/
/**
 * @file render_viewport.cpp
 * @brief Implementation of Viewport
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres. All rights reserved.
 */
/**************************************************************************************************/

#include "render_viewport.h"

#include <QSurfaceFormat>
#include <qopenglext.h>
#include <qsurfaceformat.h>

#include "utils/utils_logger.h"
#include "utils/utils_types.h"

#include <GL/gl.h>

namespace Orogena::Render
{

//=================================================================================================
// Static Member Initialization
//=================================================================================================

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

Viewport::Viewport(QWidget* parent) : QOpenGLWidget(parent)
{
    // Request OpenGL 4.5 Core Profile
    QSurfaceFormat format;
    format.setVersion(4, 6);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4); // 4x MSAA
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setSwapInterval(1); // VSync enabled
    QSurfaceFormat::setDefaultFormat(format);

    setFormat(format);

    Log::Debug("Viewport: Requested OpenGL 4.6 Core Profile");

#ifdef Q_OS_LINUX
    Log::Debug("Platform: Linux (using X11/GLX by default)");
#elif defined(Q_OS_WIN)
    Log::Debug("Platform: Windows");
#elif define(Q_OS_MACOS)
    Log::Debug("Platform: macOS");
#endif
}

Viewport::~Viewport()
{
    // Cleanup happens in Qt's OpenGL context management
    Log::Debug("Viewport: Destroyed");
}

//=================================================================================================
// Public Functions
//=================================================================================================

void Viewport::SetClearColor(Utils::ColorRGBF color)
{
    m_ClearColor = color;

    if (context() != nullptr)
    {
        makeCurrent();
        glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0F);
        doneCurrent();
    }

    update(); // Trigger repaint
}

//=================================================================================================
// Protected Functions
//=================================================================================================

void Viewport::initializeGL()
{
    // Initialize OpenGL functions
    if (!initializeOpenGLFunctions())
    {
        QString error = "Failed to initialize OpenGL functions";
        Log::Critical("Viewport: {}", error.toStdString());
        emit OpenGLError(error);
        return;
    }

    // Get actual context format
    QSurfaceFormat actual_format = context()->format();
    int32_t major_version = actual_format.majorVersion();
    int32_t minor_version = actual_format.minorVersion();

    // Get OpenGL info
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

    QString vendor_str = reinterpret_cast<const char_t*>(vendor);
    QString renderer_str = reinterpret_cast<const char_t*>(renderer);
    QString version_str = reinterpret_cast<const char_t*>(version);
    QString glsl_version_str = reinterpret_cast<const char_t*>(glsl_version);

    Log::Info("Viewport: OpenGL initialized successfully");
    Log::Info("  Vendor: {}", vendor_str.toStdString());
    Log::Info("  Renderer: {}", renderer_str.toStdString());
    Log::Info("  Version: {} ({}.{})", version_str.toStdString(), major_version, minor_version);
    Log::Info("  GLSL Version: {}", glsl_version_str.toStdString());

    // Warn if version is below 4.5
    if (major_version < 4 || (major_version == 4 && minor_version < 5))
    {
        Log::Warn("Viewport: OpenGL {}.{} is below requested 4.5, "
                  "some features may not be available",
                  major_version, minor_version);
    }

    // Check for errors after initialization
    if (CheckGLError("initializeGL"))
    {
        return;
    }

    // Set up basic OpenGL state
    glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0F);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    CheckGLError("OpenGL state setup");

    // Start FPS timer
    m_FrameTimer.start();
    m_LastFPSUpdate = m_FrameTimer.elapsed();

    // Emit success signal
    emit OpenGLInitialized(vendor_str, renderer_str,
                           QString("%1.%2").arg(major_version).arg(minor_version));
}

void Viewport::paintGL()
{
    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Check for errors
    CheckGLError("paintGL clear");

    // Update FPS counter
    UpdateFPS();

    // Request next frame
    update();
}

void Viewport::resizeGL(int32_t widthPx, int32_t heightPx)
{
    // Update viewport
    glViewport(0, 0, widthPx, heightPx);

    Log::Debug("Viewport: Resized to {}x{}", widthPx, heightPx);

    // Check for errors
    CheckGLError("resizeGL");
}

//=================================================================================================
// Private Functions
//=================================================================================================

bool Viewport::CheckGLError(const QString& context)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        QString error_msg;
        switch (error)
        {
            case GL_INVALID_ENUM:
                error_msg = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error_msg = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error_msg = "GL_INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                error_msg = "GL_OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error_msg = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            default:
                error_msg = QString("Unkown error 0x%1").arg(error, 0, 16);
                break;
        }
        QString full_msg = QString("%1: %2").arg(context, error_msg);
        Log::Error("Viewport: OpenGL error - {}", full_msg.toStdString());
        emit OpenGLError(full_msg);
        return true;
    }
    return false;
}

void Viewport::UpdateFPS()
{
    m_FrameCount++;

    qint64 current_time = m_FrameTimer.elapsed();
    qint64 delta_time = current_time - m_LastFPSUpdate;

    // Update FPS counter every second
    if (delta_time >= 1000)
    {
        m_CurrentFPS =
            static_cast<int>((m_FrameCount * 1000.0) / static_cast<float64_t>(delta_time));
        emit FPSUpdated(m_CurrentFPS);

        m_FrameCount = 0;
        m_LastFPSUpdate = current_time;
    }
}

} // namespace Orogena::Render
