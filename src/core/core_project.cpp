/**************************************************************************************************/
/**
 * @file core_project.cpp
 * @brief Implementation of project management and JSON serialization
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

#include "core/core_project.h"

#include <nlohmann/json.hpp>

#include "core/core_settings_interface.h"
#include "database/database_interface.h"
#include "utils/utils_logger.h"

#include <fstream>

namespace Orogena::Core
{

//=================================================================================================
// Constants (internal)
//=================================================================================================

namespace
{
constexpr const char* c_RecentProjectsKey = "recentProjects";

// JSON keys
constexpr const char* c_JsonKeyVersion = "version";
constexpr const char* c_JsonKeyName = "name";
constexpr const char* c_JsonKeyDescription = "description";
constexpr const char* c_JsonKeyAuthor = "author";
constexpr const char* c_JsonKeyCreated = "created";
constexpr const char* c_JsonKeyModified = "modified";
constexpr const char* c_JsonKeyDatabase = "database";
constexpr const char* c_JsonKeySettings = "settings";

} // namespace

//=================================================================================================
// ProjectInfo Implementation
//=================================================================================================

bool ProjectInfo::IsValid() const
{
    return !name.empty() && !databaseFilename.empty();
}

std::chrono::system_clock::time_point ProjectInfo::GetCreatedTime() const
{
    return std::chrono::system_clock::from_time_t(static_cast<std::time_t>(createdTimestamp));
}

std::chrono::system_clock::time_point ProjectInfo::GetModifiedTime() const
{
    return std::chrono::system_clock::from_time_t(static_cast<std::time_t>(modifiedTimestamp));
}

void ProjectInfo::SetCreatedTime(std::chrono::system_clock::time_point time)
{
    createdTimestamp = static_cast<int64_t>(std::chrono::system_clock::to_time_t(time));
}

void ProjectInfo::SetModifiedTime(std::chrono::system_clock::time_point time)
{
    modifiedTimestamp = static_cast<int64_t>(std::chrono::system_clock::to_time_t(time));
}

//=================================================================================================
// JSON Serialization
//=================================================================================================

std::string SerializeProjectInfo(const ProjectInfo& info)
{
    nlohmann::json json;

    json[c_JsonKeyVersion] = info.version;
    json[c_JsonKeyName] = info.name;
    json[c_JsonKeyDescription] = info.description;
    json[c_JsonKeyAuthor] = info.author;
    json[c_JsonKeyCreated] = info.createdTimestamp;
    json[c_JsonKeyModified] = info.modifiedTimestamp;
    json[c_JsonKeyDatabase] = info.databaseFilename;
    json[c_JsonKeySettings] = info.settings;

    return json.dump(2); // Pretty print with 2-space indent
}

std::optional<ProjectInfo> DeserializeProjectInfo(const std::string& jsonStr)
{
    try
    {
        nlohmann::json json = nlohmann::json::parse(jsonStr);

        ProjectInfo info;

        // Required fields
        if (!json.contains(c_JsonKeyVersion) || !json.contains(c_JsonKeyName) ||
            !json.contains(c_JsonKeyDatabase))
        {
            Log::Error("Project JSON missing required fields");
            return std::nullopt;
        }

        info.version = json[c_JsonKeyVersion].get<std::string>();
        info.name = json[c_JsonKeyName].get<std::string>();
        info.databaseFilename = json[c_JsonKeyDatabase].get<std::string>();

        // Optional fields with defaults
        if (json.contains(c_JsonKeyDescription))
        {
            info.description = json[c_JsonKeyDescription].get<std::string>();
        }

        if (json.contains(c_JsonKeyAuthor))
        {
            info.author = json[c_JsonKeyAuthor].get<std::string>();
        }

        if (json.contains(c_JsonKeyCreated))
        {
            info.createdTimestamp = json[c_JsonKeyCreated].get<int64_t>();
        }

        if (json.contains(c_JsonKeyModified))
        {
            info.modifiedTimestamp = json[c_JsonKeyModified].get<int64_t>();
        }

        if (json.contains(c_JsonKeySettings))
        {
            info.settings =
                json[c_JsonKeySettings].get<std::unordered_map<std::string, std::string>>();
        }

        return info;
    }
    catch (const nlohmann::json::exception& e)
    {
        Log::Error("Failed to parse project JSON: {}", e.what());
        return std::nullopt;
    }
}

bool SaveProjectInfoToFile(const ProjectInfo& info, const std::filesystem::path& path)
{
    try
    {
        std::string json = SerializeProjectInfo(info);

        std::ofstream file(path);
        if (!file.is_open())
        {
            Log::Error("Failed to open project file for writing: {}", path.string());
            return false;
        }

        file << json;
        file.close();

        if (file.fail())
        {
            Log::Error("Failed to write project file: {}", path.string());
            return false;
        }

        Log::Info("Saved project to: {}", path.string());
        return true;
    }
    catch (const std::exception& e)
    {
        Log::Error("Exception saving project file: {}", e.what());
        return false;
    }
}

std::optional<ProjectInfo> LoadProjectInfoFromFile(const std::filesystem::path& path)
{
    try
    {
        if (!std::filesystem::exists(path))
        {
            Log::Error("Project file does not exist: {}", path.string());
            return std::nullopt;
        }

        std::ifstream file(path);
        if (!file.is_open())
        {
            Log::Error("Failed to open project file: {}", path.string());
            return std::nullopt;
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());

        auto info = DeserializeProjectInfo(content);
        if (info)
        {
            Log::Info("Loaded project from: {}", path.string());
        }

        return info;
    }
    catch (const std::exception& e)
    {
        Log::Error("Exception loading project file: {}", e.what());
        return std::nullopt;
    }
}

//=================================================================================================
// ProjectManager Implementation
//=================================================================================================

ProjectManager::ProjectManager(ISettings& settings, Database::IDatabase* database)
    : m_Settings(settings), m_Database(database)
{
}

bool ProjectManager::CreateProject(const std::filesystem::path& directoryPath,
                                   const std::string&           projectName)
{
    if (IsProjectOpen())
    {
        ReportError("Cannot create project: another project is already open");
        return false;
    }

    if (projectName.empty())
    {
        ReportError("Cannot create project: name is empty");
        return false;
    }

    try
    {
        // Build safe filename from project name
        std::string safe_name = projectName;
        // Replace spaces with underscores for filename
        for (char& c : safe_name)
        {
            if (c == ' ')
            {
                c = '_';
            }
        }

        // Create project subdirectory: directoryPath/ProjectName/
        std::filesystem::path project_dir = directoryPath / safe_name;
        if (!std::filesystem::exists(project_dir))
        {
            std::filesystem::create_directories(project_dir);
        }

        // Build file paths within the project directory
        std::filesystem::path project_path = project_dir / (safe_name + c_ProjectFileExtension);
        std::string           database_filename = safe_name + c_ProjectDatabaseExtension;
        std::filesystem::path database_path = project_dir / database_filename;

        // Check if files already exist
        if (std::filesystem::exists(project_path))
        {
            ReportError("Project file already exists: " + project_path.string());
            return false;
        }

        // Create project info
        ProjectInfo info;
        info.version = c_ProjectFormatVersion;
        info.name = projectName;
        info.databaseFilename = database_filename;
        info.SetCreatedTime(std::chrono::system_clock::now());
        info.SetModifiedTime(std::chrono::system_clock::now());

        // Initialize database if we have a database interface
        if (m_Database)
        {
            if (!InitializeDatabase(database_path))
            {
                ReportError("Failed to initialize project database");
                return false;
            }
        }

        // Save project file
        if (!SaveProjectInfoToFile(info, project_path))
        {
            DisconnectDatabase();
            return false;
        }

        // Update state
        m_ProjectInfo = info;
        m_ProjectPath = project_path;
        m_State = ProjectState::OPEN;

        // Add to recent projects
        AddToRecentProjects(project_path);

        // Notify
        if (m_Callbacks.onProjectOpened)
        {
            m_Callbacks.onProjectOpened(project_path.string());
        }

        Log::Info("Created new project: {}", projectName);
        return true;
    }
    catch (const std::exception& e)
    {
        ReportError(std::string("Exception creating project: ") + e.what());
        m_State = ProjectState::ERROR_STATE;
        return false;
    }
}

bool ProjectManager::OpenProject(const std::filesystem::path& projectFilePath)
{
    if (IsProjectOpen())
    {
        ReportError("Cannot open project: another project is already open");
        return false;
    }

    m_State = ProjectState::LOADING;

    try
    {
        // Load project info
        auto info = LoadProjectInfoFromFile(projectFilePath);
        if (!info)
        {
            m_State = ProjectState::CLOSED;
            return false;
        }

        // Validate
        if (!info->IsValid())
        {
            ReportError("Invalid project file: missing required fields");
            m_State = ProjectState::CLOSED;
            return false;
        }

        // Connect to database if we have a database interface
        if (m_Database)
        {
            std::filesystem::path database_path =
                projectFilePath.parent_path() / info->databaseFilename;

            if (!std::filesystem::exists(database_path))
            {
                ReportError("Project database not found: " + database_path.string());
                m_State = ProjectState::CLOSED;
                return false;
            }

            if (!ConnectDatabase(database_path))
            {
                m_State = ProjectState::CLOSED;
                return false;
            }
        }

        // Update state
        m_ProjectInfo = info;
        m_ProjectPath = projectFilePath;
        m_State = ProjectState::OPEN;

        // Add to recent projects
        AddToRecentProjects(projectFilePath);

        // Notify
        if (m_Callbacks.onProjectOpened)
        {
            m_Callbacks.onProjectOpened(projectFilePath.string());
        }

        Log::Info("Opened project: {}", info->name);
        return true;
    }
    catch (const std::exception& e)
    {
        ReportError(std::string("Exception opening project: ") + e.what());
        m_State = ProjectState::ERROR_STATE;
        return false;
    }
}

bool ProjectManager::SaveProject()
{
    if (!IsProjectOpen() || !m_ProjectPath)
    {
        ReportError("Cannot save: no project is open");
        return false;
    }

    m_State = ProjectState::SAVING;

    try
    {
        // Update modified timestamp
        m_ProjectInfo->SetModifiedTime(std::chrono::system_clock::now());

        // Save to file
        if (!SaveProjectInfoToFile(*m_ProjectInfo, *m_ProjectPath))
        {
            m_State = ProjectState::MODIFIED;
            return false;
        }

        m_State = ProjectState::OPEN;

        // Notify
        if (m_Callbacks.onProjectSaved)
        {
            m_Callbacks.onProjectSaved(m_ProjectPath->string());
        }

        return true;
    }
    catch (const std::exception& e)
    {
        ReportError(std::string("Exception saving project: ") + e.what());
        m_State = ProjectState::ERROR_STATE;
        return false;
    }
}

bool ProjectManager::SaveProjectAs(const std::filesystem::path& newPath)
{
    if (!IsProjectOpen())
    {
        ReportError("Cannot save: no project is open");
        return false;
    }

    m_State = ProjectState::SAVING;

    try
    {
        std::filesystem::path new_dir = newPath.parent_path();
        std::filesystem::path old_dir = m_ProjectPath->parent_path();

        // Derive new database filename from new project filename
        std::string           new_db_filename = newPath.stem().string() + ".db";
        std::filesystem::path old_db_path = old_dir / m_ProjectInfo->databaseFilename;
        std::filesystem::path new_db_path = new_dir / new_db_filename;

        // Copy database to new location with new name (if database file exists and paths differ)
        if (std::filesystem::exists(old_db_path) && old_db_path != new_db_path)
        {
            // Disconnect from database if we have an active connection
            if (m_Database)
            {
                DisconnectDatabase();
            }

            std::filesystem::copy_file(old_db_path, new_db_path,
                                       std::filesystem::copy_options::overwrite_existing);

            // Reconnect to the new database location if we have a database interface
            if (m_Database)
            {
                if (!ConnectDatabase(new_db_path))
                {
                    // Try to reconnect to old database
                    ConnectDatabase(old_db_path);
                    m_State = ProjectState::MODIFIED;
                    return false;
                }
            }
        }

        // Always update project name and database filename to match new path
        m_ProjectInfo->name = newPath.stem().string();
        m_ProjectInfo->databaseFilename = new_db_filename;

        // Update modified timestamp
        m_ProjectInfo->SetModifiedTime(std::chrono::system_clock::now());

        // Save to new file
        if (!SaveProjectInfoToFile(*m_ProjectInfo, newPath))
        {
            m_State = ProjectState::MODIFIED;
            return false;
        }

        // Update path
        m_ProjectPath = newPath;
        m_State = ProjectState::OPEN;

        // Add to recent projects
        AddToRecentProjects(newPath);

        // Notify
        if (m_Callbacks.onProjectSaved)
        {
            m_Callbacks.onProjectSaved(newPath.string());
        }

        return true;
    }
    catch (const std::exception& e)
    {
        ReportError(std::string("Exception saving project: ") + e.what());
        m_State = ProjectState::ERROR_STATE;
        return false;
    }
}

bool ProjectManager::CloseProject(bool force)
{
    if (!IsProjectOpen())
    {
        return true; // Already closed
    }

    if (HasUnsavedChanges() && !force)
    {
        return false; // Caller should prompt user
    }

    DisconnectDatabase();

    m_ProjectInfo = std::nullopt;
    m_ProjectPath = std::nullopt;
    m_State = ProjectState::CLOSED;

    // Notify
    if (m_Callbacks.onProjectClosed)
    {
        m_Callbacks.onProjectClosed();
    }

    Log::Info("Closed project");
    return true;
}

bool ProjectManager::IsProjectOpen() const
{
    return m_State != ProjectState::CLOSED && m_State != ProjectState::ERROR_STATE;
}

bool ProjectManager::HasUnsavedChanges() const
{
    return m_State == ProjectState::MODIFIED;
}

ProjectState ProjectManager::GetState() const
{
    return m_State;
}

std::optional<ProjectInfo> ProjectManager::GetProjectInfo() const
{
    return m_ProjectInfo;
}

std::optional<std::filesystem::path> ProjectManager::GetProjectPath() const
{
    return m_ProjectPath;
}

std::optional<std::filesystem::path> ProjectManager::GetDatabasePath() const
{
    if (!m_ProjectPath || !m_ProjectInfo)
    {
        return std::nullopt;
    }

    return m_ProjectPath->parent_path() / m_ProjectInfo->databaseFilename;
}

void ProjectManager::MarkModified()
{
    if (m_State == ProjectState::OPEN)
    {
        m_State = ProjectState::MODIFIED;

        if (m_Callbacks.onProjectModified)
        {
            m_Callbacks.onProjectModified();
        }
    }
}

std::vector<std::filesystem::path> ProjectManager::GetRecentProjects() const
{
    std::vector<std::filesystem::path> result;

    auto recent = m_Settings.GetStringList(c_RecentProjectsKey);
    if (recent)
    {
        for (const auto& path_str : *recent)
        {
            result.emplace_back(path_str);
        }
    }

    return result;
}

void ProjectManager::ClearRecentProjects()
{
    m_Settings.Remove(c_RecentProjectsKey);
    m_Settings.Sync();
}

void ProjectManager::SetCallbacks(const ProjectCallbacks& callbacks)
{
    m_Callbacks = callbacks;
}

void ProjectManager::AddToRecentProjects(const std::filesystem::path& path)
{
    std::vector<std::string> recent;

    auto existing = m_Settings.GetStringList(c_RecentProjectsKey);
    if (existing)
    {
        recent = *existing;
    }

    // Remove if already in list
    std::string path_str = path.string();
    recent.erase(std::remove(recent.begin(), recent.end(), path_str), recent.end());

    // Add to front
    recent.insert(recent.begin(), path_str);

    // Trim to max size
    if (recent.size() > static_cast<size_t>(c_MaxRecentProjects))
    {
        recent.resize(c_MaxRecentProjects);
    }

    m_Settings.SetStringList(c_RecentProjectsKey, recent);
    m_Settings.Sync();
}

bool ProjectManager::InitializeDatabase(const std::filesystem::path& databasePath)
{
    if (!m_Database)
    {
        return true; // No database interface, skip
    }

    if (!m_Database->Connect(databasePath.string()))
    {
        auto error = m_Database->GetLastError();
        if (error)
        {
            ReportError("Database connection failed: " + error->message);
        }
        return false;
    }

    Log::Debug("Connected to project database: {}", databasePath.string());
    return true;
}

bool ProjectManager::ConnectDatabase(const std::filesystem::path& databasePath)
{
    return InitializeDatabase(databasePath);
}

void ProjectManager::DisconnectDatabase()
{
    if (m_Database && m_Database->IsConnected())
    {
        m_Database->Disconnect();
    }
}

void ProjectManager::ReportError(const std::string& error)
{
    Log::Error("{}", error);

    if (m_Callbacks.onError)
    {
        m_Callbacks.onError(error);
    }
}

} // namespace Orogena::Core
