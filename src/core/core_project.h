/**************************************************************************************************/
/**
 * @file core_project.h
 * @brief Project data model and management for worldbuilding projects
 *
 * @details
 * Defines the ProjectInfo structure for project metadata and the ProjectManager class
 * for project lifecycle management (create, open, save, close). Projects are serialized
 * to JSON (.oro files) with associated SQLite databases for heavy data.
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
/**************************************************************************************************/

#pragma once

#include <memory>
#include <string>

#include "utils/utils_types.h"

#include <chrono>
#include <filesystem>
#include <functional>
#include <optional>
#include <unordered_map>

namespace Orogena::Database
{
class IDatabase;
}

namespace Orogena::Core
{

class ISettings;

//=================================================================================================
// Constants
//=================================================================================================

constexpr const char* c_ProjectFileExtension = ".oro";
constexpr const char* c_ProjectDatabaseExtension = ".db";
constexpr const char* c_ProjectFormatVersion = "1.0";
constexpr int32_t c_MaxRecentProjects = 10;

//=================================================================================================
// Project Info Structure
//=================================================================================================

/**
 * @brief Metadata for a worldbuilding project
 *
 * @details
 * Contains all project information that is serialized to the .oro JSON file.
 * Heavy data (heightmaps, plate configurations, etc.) lives in the associated
 * SQLite database referenced by databasePath.
 */
struct ProjectInfo
{
    std::string version{c_ProjectFormatVersion}; ///< Project format version
    std::string name;                            ///< Project display name
    std::string description;                     ///< Optional project description
    std::string author;                          ///< Optional author name

    int64_t createdTimestamp{0};  ///< Unix timestamp when project was created
    int64_t modifiedTimestamp{0}; ///< Unix timestamp when project was last modified

    std::string databaseFilename; ///< Relative path to SQLite database (e.g., "project.db")

    std::unordered_map<std::string, std::string> settings; ///< Project-specific settings

    /**
     * @brief Check if project info is valid for saving
     * @return true if name and databaseFilename are non-empty
     */
    bool IsValid() const;

    /**
     * @brief Get created time as chrono time_point
     */
    std::chrono::system_clock::time_point GetCreatedTime() const;

    /**
     * @brief Get modified time as chrono time_point
     */
    std::chrono::system_clock::time_point GetModifiedTime() const;

    /**
     * @brief Set created time from chrono time_point
     */
    void SetCreatedTime(std::chrono::system_clock::time_point time);

    /**
     * @brief Set modified time from chrono time_point
     */
    void SetModifiedTime(std::chrono::system_clock::time_point time);
};

//=================================================================================================
// Project State
//=================================================================================================

/**
 * @brief Current state of a project
 */
enum class ProjectState
{
    CLOSED,     ///< No project loaded
    LOADING,    ///< Project is being loaded
    OPEN,       ///< Project is open and unmodified
    MODIFIED,   ///< Project has unsaved changes
    SAVING,     ///< Project is being saved
    ERROR_STATE ///< Project encountered an error
};

//=================================================================================================
// Project Manager Callbacks
//=================================================================================================

/**
 * @brief Callbacks for project lifecycle events
 *
 * @details
 * Used for upward communication to GUI layer without Qt dependency.
 */
struct ProjectCallbacks
{
    std::function<void(const std::string& path)> onProjectOpened;
    std::function<void()> onProjectClosed;
    std::function<void()> onProjectModified;
    std::function<void(const std::string& path)> onProjectSaved;
    std::function<void(const std::string& error)> onError;
};

//=================================================================================================
// Project Manager Interface
//=================================================================================================

/**
 * @brief Interface for project lifecycle management
 *
 * @details
 * Abstract interface allowing different implementations (e.g., with or without
 * database integration) while maintaining a consistent API.
 */
class IProjectManager
{
  public:
    virtual ~IProjectManager() = default;

    //=============================================================================================
    // Project Lifecycle
    //=============================================================================================

    /**
     * @brief Create a new project
     *
     * @param directoryPath Directory where project files will be created
     * @param projectName Name of the project (used for display and filename)
     * @return true if project was created successfully
     *
     * @details
     * Creates:
     * - <projectName>.oro (JSON metadata file)
     * - <projectName>.db (SQLite database)
     */
    virtual bool CreateProject(const std::filesystem::path& directoryPath,
                               const std::string& projectName) = 0;

    /**
     * @brief Open an existing project
     *
     * @param projectFilePath Path to the .oro project file
     * @return true if project was opened successfully
     */
    virtual bool OpenProject(const std::filesystem::path& projectFilePath) = 0;

    /**
     * @brief Save current project to its existing location
     * @return true if save was successful
     */
    virtual bool SaveProject() = 0;

    /**
     * @brief Save current project to a new location
     *
     * @param newPath New path for the .oro file
     * @return true if save was successful
     */
    virtual bool SaveProjectAs(const std::filesystem::path& newPath) = 0;

    /**
     * @brief Close the current project
     *
     * @param force If true, discard unsaved changes
     * @return true if project was closed (false if unsaved changes and !force)
     */
    virtual bool CloseProject(bool force = false) = 0;

    //=============================================================================================
    // Project State
    //=============================================================================================

    /**
     * @brief Check if a project is currently open
     */
    virtual bool IsProjectOpen() const = 0;

    /**
     * @brief Check if current project has unsaved changes
     */
    virtual bool HasUnsavedChanges() const = 0;

    /**
     * @brief Get current project state
     */
    virtual ProjectState GetState() const = 0;

    /**
     * @brief Get current project info
     * @return Project info or nullopt if no project is open
     */
    virtual std::optional<ProjectInfo> GetProjectInfo() const = 0;

    /**
     * @brief Get path to current project file
     * @return Path or nullopt if no project is open
     */
    virtual std::optional<std::filesystem::path> GetProjectPath() const = 0;

    /**
     * @brief Get path to current project's database
     * @return Path or nullopt if no project is open
     */
    virtual std::optional<std::filesystem::path> GetDatabasePath() const = 0;

    //=============================================================================================
    // Modification Tracking
    //=============================================================================================

    /**
     * @brief Mark project as modified
     *
     * @details
     * Called by other systems when they make changes that need saving.
     */
    virtual void MarkModified() = 0;

    //=============================================================================================
    // Recent Projects
    //=============================================================================================

    /**
     * @brief Get list of recently opened projects
     * @return Vector of project file paths (most recent first)
     */
    virtual std::vector<std::filesystem::path> GetRecentProjects() const = 0;

    /**
     * @brief Clear the recent projects list
     */
    virtual void ClearRecentProjects() = 0;

    //=============================================================================================
    // Callbacks
    //=============================================================================================

    /**
     * @brief Set callbacks for project events
     */
    virtual void SetCallbacks(const ProjectCallbacks& callbacks) = 0;
};

//=================================================================================================
// Project Manager Implementation
//=================================================================================================

/**
 * @brief Default implementation of project manager
 *
 * @details
 * Manages project lifecycle with JSON serialization and SQLite database integration.
 * Uses ISettings for storing recent projects list.
 */
class ProjectManager : public IProjectManager
{
  public:
    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    /**
     * @brief Construct project manager
     *
     * @param settings Settings interface for storing recent projects
     * @param database Database interface for project data (optional, can be null for testing)
     */
    explicit ProjectManager(ISettings& settings, Database::IDatabase* database = nullptr);

    ~ProjectManager() override = default;

    // Non-copyable, non-movable (contains reference member)
    ProjectManager(const ProjectManager&) = delete;
    ProjectManager& operator=(const ProjectManager&) = delete;
    ProjectManager(ProjectManager&&) = delete;
    ProjectManager& operator=(ProjectManager&&) = delete;

    //=============================================================================================
    // IProjectManager Implementation
    //=============================================================================================

    bool CreateProject(const std::filesystem::path& directoryPath,
                       const std::string& projectName) override;
    bool OpenProject(const std::filesystem::path& projectFilePath) override;
    bool SaveProject() override;
    bool SaveProjectAs(const std::filesystem::path& newPath) override;
    bool CloseProject(bool force = false) override;

    bool IsProjectOpen() const override;
    bool HasUnsavedChanges() const override;
    ProjectState GetState() const override;
    std::optional<ProjectInfo> GetProjectInfo() const override;
    std::optional<std::filesystem::path> GetProjectPath() const override;
    std::optional<std::filesystem::path> GetDatabasePath() const override;

    void MarkModified() override;

    std::vector<std::filesystem::path> GetRecentProjects() const override;
    void ClearRecentProjects() override;

    void SetCallbacks(const ProjectCallbacks& callbacks) override;

  private:
    //=============================================================================================
    // Private Methods
    //=============================================================================================

    /**
     * @brief Add path to recent projects list
     */
    void AddToRecentProjects(const std::filesystem::path& path);

    /**
     * @brief Create database file and initialize schema
     */
    bool InitializeDatabase(const std::filesystem::path& databasePath);

    /**
     * @brief Connect to existing database
     */
    bool ConnectDatabase(const std::filesystem::path& databasePath);

    /**
     * @brief Disconnect from current database
     */
    void DisconnectDatabase();

    /**
     * @brief Report error through callback
     */
    void ReportError(const std::string& error);

    //=============================================================================================
    // Private Members
    //=============================================================================================

    ISettings& m_Settings;           ///< Settings for recent projects
    Database::IDatabase* m_Database; ///< Database interface (may be null)

    ProjectState m_State{ProjectState::CLOSED};         ///< Current state
    std::optional<ProjectInfo> m_ProjectInfo;           ///< Current project metadata
    std::optional<std::filesystem::path> m_ProjectPath; ///< Path to .oro file

    ProjectCallbacks m_Callbacks; ///< Event callbacks
};

//=================================================================================================
// JSON Serialization (Free Functions)
//=================================================================================================

/**
 * @brief Serialize ProjectInfo to JSON string
 *
 * @param info Project info to serialize
 * @return JSON string representation
 */
std::string SerializeProjectInfo(const ProjectInfo& info);

/**
 * @brief Deserialize ProjectInfo from JSON string
 *
 * @param json JSON string to parse
 * @return ProjectInfo or nullopt on parse error
 */
std::optional<ProjectInfo> DeserializeProjectInfo(const std::string& json);

/**
 * @brief Save ProjectInfo to file
 *
 * @param info Project info to save
 * @param path File path
 * @return true if successful
 */
bool SaveProjectInfoToFile(const ProjectInfo& info, const std::filesystem::path& path);

/**
 * @brief Load ProjectInfo from file
 *
 * @param path File path
 * @return ProjectInfo or nullopt on error
 */
std::optional<ProjectInfo> LoadProjectInfoFromFile(const std::filesystem::path& path);

} // namespace Orogena::Core
