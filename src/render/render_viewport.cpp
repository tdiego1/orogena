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

#include <format>

#include <QSurfaceFormat>

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
    // Use the default format set in main() - don't override it here
    // This prevents format mismatches between QApplication and QOpenGLWidget

    Log::Debug("Viewport: Using application default OpenGL format");

#ifdef Q_OS_LINUX
    const char* qpa_platform = qgetenv("QT_QPA_PLATFORM");
    std::string platform_str = qpa_platform ? qpa_platform : "";
    if (platform_str.find("wayland") != std::string::npos)
    {
        Log::Debug("Platform: Linux (Wayland/EGL)");
    }
    else if (platform_str.find("xcb") != std::string::npos)
    {
        Log::Debug("Platform: Linux (X11/GLX)");
    }
    else
    {
        Log::Debug("Platform: Linux (auto-detected)");
    }
#elif defined(Q_OS_WIN)
    Log::Debug("Platform: Windows");
#elif defined(Q_OS_MACOS)
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
        std::string error = "Failed to initialize OpenGL functions";
        Log::Critical("Viewport: {}", error);
        emit OpenGLError(error);
        return;
    }

    // Get actual context format
    QSurfaceFormat actual_format = context()->format();
    int32_t major_version = actual_format.majorVersion();
    int32_t minor_version = actual_format.minorVersion();

    // Get OpenGL info - safely convert GLubyte* to std::string
    auto glStringToStd = [](const GLubyte* str) -> std::string {
        return str ? reinterpret_cast<const char*>(str) : "Unknown";
    };

    std::string vendor_str = glStringToStd(glGetString(GL_VENDOR));
    std::string renderer_str = glStringToStd(glGetString(GL_RENDERER));
    std::string version_str = glStringToStd(glGetString(GL_VERSION));
    std::string glsl_version_str = glStringToStd(glGetString(GL_SHADING_LANGUAGE_VERSION));

    Log::Info("Viewport: OpenGL initialized successfully");
    Log::Info("  Vendor: {}", vendor_str);
    Log::Info("  Renderer: {}", renderer_str);
    Log::Info("  Version: {} ({}.{})", version_str, major_version, minor_version);
    Log::Info("  GLSL Version: {}", glsl_version_str);

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
    m_FrameStartTime = std::chrono::steady_clock::now();
    m_LastFPSUpdateMs = 0;

    // Emit success signal
    emit OpenGLInitialized(vendor_str, renderer_str,
                           std::format("{}.{}", major_version, minor_version));
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

bool Viewport::CheckGLError(const std::string& context)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::string error_msg;
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
                error_msg = std::format("Unknown error 0x{:X}", error);
                break;
        }
        std::string full_msg = std::format("{}: {}", context, error_msg);
        Log::Error("Viewport: OpenGL error - {}", full_msg);
        emit OpenGLError(full_msg);
        return true;
    }
    return false;
}

void Viewport::UpdateFPS()
{
    m_FrameCount++;

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_FrameStartTime);
    int64_t current_time_ms = elapsed.count();
    int64_t delta_time = current_time_ms - m_LastFPSUpdateMs;

    // Update FPS counter every second
    if (delta_time >= 1000)
    {
        m_CurrentFPS =
            static_cast<int32_t>((m_FrameCount * 1000.0) / static_cast<float64_t>(delta_time));
        emit FPSUpdated(m_CurrentFPS);

        m_FrameCount = 0;
        m_LastFPSUpdateMs = current_time_ms;
    }
}

} // namespace Orogena::Render
