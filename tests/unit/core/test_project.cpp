/**************************************************************************************************/
/**
 * @file test_project.cpp
 * @brief Unit tests for project serialization and ProjectManager
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres. All rights reserved.
 */
/**************************************************************************************************/

#include "core/core_project.h"
#include "core/core_settings_interface.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

using namespace Orogena::Core;

//=================================================================================================
// Mock Settings Implementation
//=================================================================================================

/**
 * @brief Mock implementation of ISettings for testing
 */
class MockSettings : public ISettings
{
  public:
    void SetString(const std::string& key, const std::string& value) override
    {
        m_StringValues[key] = value;
    }

    std::optional<std::string> GetString(const std::string& key) const override
    {
        auto it = m_StringValues.find(key);
        if (it != m_StringValues.end())
        {
            return it->second;
        }
        return std::nullopt;
    }

    void SetInt(const std::string& key, int32_t value) override
    {
        m_IntValues[key] = value;
    }

    std::optional<int32_t> GetInt(const std::string& key) const override
    {
        auto it = m_IntValues.find(key);
        if (it != m_IntValues.end())
        {
            return it->second;
        }
        return std::nullopt;
    }

    void SetInt64(const std::string& key, int64_t value) override
    {
        m_Int64Values[key] = value;
    }

    std::optional<int64_t> GetInt64(const std::string& key) const override
    {
        auto it = m_Int64Values.find(key);
        if (it != m_Int64Values.end())
        {
            return it->second;
        }
        return std::nullopt;
    }

    void SetFloat(const std::string& key, float64_t value) override
    {
        m_FloatValues[key] = value;
    }

    std::optional<float64_t> GetFloat(const std::string& key) const override
    {
        auto it = m_FloatValues.find(key);
        if (it != m_FloatValues.end())
        {
            return it->second;
        }
        return std::nullopt;
    }

    void SetBool(const std::string& key, bool value) override
    {
        m_BoolValues[key] = value;
    }

    std::optional<bool> GetBool(const std::string& key) const override
    {
        auto it = m_BoolValues.find(key);
        if (it != m_BoolValues.end())
        {
            return it->second;
        }
        return std::nullopt;
    }

    void SetStringList(const std::string& key, const std::vector<std::string>& value) override
    {
        m_StringListValues[key] = value;
    }

    std::optional<std::vector<std::string>> GetStringList(const std::string& key) const override
    {
        auto it = m_StringListValues.find(key);
        if (it != m_StringListValues.end())
        {
            return it->second;
        }
        return std::nullopt;
    }

    bool Contains(const std::string& key) const override
    {
        return m_StringValues.count(key) > 0 || m_IntValues.count(key) > 0 ||
               m_Int64Values.count(key) > 0 || m_FloatValues.count(key) > 0 ||
               m_BoolValues.count(key) > 0 || m_StringListValues.count(key) > 0;
    }

    void Remove(const std::string& key) override
    {
        m_StringValues.erase(key);
        m_IntValues.erase(key);
        m_Int64Values.erase(key);
        m_FloatValues.erase(key);
        m_BoolValues.erase(key);
        m_StringListValues.erase(key);
    }

    void Clear() override
    {
        m_StringValues.clear();
        m_IntValues.clear();
        m_Int64Values.clear();
        m_FloatValues.clear();
        m_BoolValues.clear();
        m_StringListValues.clear();
    }

    void Sync() override
    {
        // No-op for mock
    }

    void BeginGroup(const std::string& /*prefix*/) override
    {
        // Not implemented for mock
    }

    void EndGroup() override
    {
        // Not implemented for mock
    }

    std::string GetDefaultProjectsDirectory() const override
    {
        // Return temp directory for testing
        return (std::filesystem::temp_directory_path() / "orogena_mock_projects").string();
    }

  private:
    std::unordered_map<std::string, std::string>              m_StringValues;
    std::unordered_map<std::string, int32_t>                  m_IntValues;
    std::unordered_map<std::string, int64_t>                  m_Int64Values;
    std::unordered_map<std::string, float64_t>                m_FloatValues;
    std::unordered_map<std::string, bool>                     m_BoolValues;
    std::unordered_map<std::string, std::vector<std::string>> m_StringListValues;
};

//=================================================================================================
// ProjectInfo Tests
//=================================================================================================

class ProjectInfoTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        m_Info.name = "Test World";
        m_Info.description = "A test worldbuilding project";
        m_Info.author = "Test Author";
        m_Info.databaseFilename = "test.db";
        m_Info.createdTimestamp = 1704067200;  // 2024-01-01 00:00:00 UTC
        m_Info.modifiedTimestamp = 1704153600; // 2024-01-02 00:00:00 UTC
    }

    ProjectInfo m_Info;
};

TEST_F(ProjectInfoTest, IsValidReturnsTrueForValidInfo)
{
    EXPECT_TRUE(m_Info.IsValid());
}

TEST_F(ProjectInfoTest, IsValidReturnsFalseForEmptyName)
{
    m_Info.name = "";
    EXPECT_FALSE(m_Info.IsValid());
}

TEST_F(ProjectInfoTest, IsValidReturnsFalseForEmptyDatabaseFilename)
{
    m_Info.databaseFilename = "";
    EXPECT_FALSE(m_Info.IsValid());
}

TEST_F(ProjectInfoTest, TimePointConversionWorks)
{
    auto now = std::chrono::system_clock::now();
    m_Info.SetCreatedTime(now);

    auto retrieved = m_Info.GetCreatedTime();

    // Allow 1 second tolerance due to time_t precision
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - retrieved);
    EXPECT_LE(std::abs(diff.count()), 1);
}

//=================================================================================================
// JSON Serialization Tests
//=================================================================================================

class ProjectSerializationTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        m_Info.version = "1.0";
        m_Info.name = "Fantasy World";
        m_Info.description = "An epic fantasy setting";
        m_Info.author = "World Builder";
        m_Info.databaseFilename = "fantasy.db";
        m_Info.createdTimestamp = 1704067200;
        m_Info.modifiedTimestamp = 1704153600;
        m_Info.settings["mapProjection"] = "equirectangular";
        m_Info.settings["units"] = "metric";
    }

    ProjectInfo m_Info;
};

TEST_F(ProjectSerializationTest, SerializeProducesValidJson)
{
    std::string json = SerializeProjectInfo(m_Info);

    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("\"name\""), std::string::npos);
    EXPECT_NE(json.find("\"Fantasy World\""), std::string::npos);
    EXPECT_NE(json.find("\"database\""), std::string::npos);
    EXPECT_NE(json.find("\"fantasy.db\""), std::string::npos);
}

TEST_F(ProjectSerializationTest, DeserializeRecreatesProjectInfo)
{
    std::string json = SerializeProjectInfo(m_Info);
    auto        result = DeserializeProjectInfo(json);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(m_Info.version, result->version);
    EXPECT_EQ(m_Info.name, result->name);
    EXPECT_EQ(m_Info.description, result->description);
    EXPECT_EQ(m_Info.author, result->author);
    EXPECT_EQ(m_Info.databaseFilename, result->databaseFilename);
    EXPECT_EQ(m_Info.createdTimestamp, result->createdTimestamp);
    EXPECT_EQ(m_Info.modifiedTimestamp, result->modifiedTimestamp);
}

TEST_F(ProjectSerializationTest, DeserializePreservesSettings)
{
    std::string json = SerializeProjectInfo(m_Info);
    auto        result = DeserializeProjectInfo(json);

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(m_Info.settings.size(), result->settings.size());
    EXPECT_EQ(m_Info.settings.at("mapProjection"), result->settings.at("mapProjection"));
    EXPECT_EQ(m_Info.settings.at("units"), result->settings.at("units"));
}

TEST_F(ProjectSerializationTest, DeserializeReturnsNulloptForInvalidJson)
{
    auto result = DeserializeProjectInfo("not valid json");
    EXPECT_FALSE(result.has_value());
}

TEST_F(ProjectSerializationTest, DeserializeReturnsNulloptForMissingRequiredFields)
{
    // JSON missing required "name" field
    std::string json = R"({"version": "1.0", "database": "test.db"})";
    auto        result = DeserializeProjectInfo(json);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ProjectSerializationTest, DeserializeHandlesOptionalFields)
{
    // Minimal valid JSON with only required fields
    std::string json = R"({"version": "1.0", "name": "Minimal", "database": "min.db"})";
    auto        result = DeserializeProjectInfo(json);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ("Minimal", result->name);
    EXPECT_EQ("min.db", result->databaseFilename);
    EXPECT_TRUE(result->description.empty());
    EXPECT_TRUE(result->author.empty());
}

//=================================================================================================
// File I/O Tests
//=================================================================================================

class ProjectFileTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        m_TempDir = std::filesystem::temp_directory_path() / "orogena_test";
        std::filesystem::create_directories(m_TempDir);

        m_Info.name = "File Test Project";
        m_Info.databaseFilename = "filetest.db";
        m_Info.createdTimestamp = 1704067200;
    }

    void TearDown() override
    {
        std::filesystem::remove_all(m_TempDir);
    }

    std::filesystem::path m_TempDir;
    ProjectInfo           m_Info;
};

TEST_F(ProjectFileTest, SaveAndLoadProjectFile)
{
    std::filesystem::path project_path = m_TempDir / "test.oro";

    // Save
    ASSERT_TRUE(SaveProjectInfoToFile(m_Info, project_path));
    ASSERT_TRUE(std::filesystem::exists(project_path));

    // Load
    auto loaded = LoadProjectInfoFromFile(project_path);
    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(m_Info.name, loaded->name);
    EXPECT_EQ(m_Info.databaseFilename, loaded->databaseFilename);
}

TEST_F(ProjectFileTest, LoadNonexistentFileReturnsNullopt)
{
    std::filesystem::path nonexistent = m_TempDir / "nonexistent.oro";
    auto                  result = LoadProjectInfoFromFile(nonexistent);
    EXPECT_FALSE(result.has_value());
}

TEST_F(ProjectFileTest, SaveCreatesValidJsonFile)
{
    std::filesystem::path project_path = m_TempDir / "json_test.oro";

    ASSERT_TRUE(SaveProjectInfoToFile(m_Info, project_path));

    // Read file content manually
    std::ifstream file(project_path);
    std::string   content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    EXPECT_NE(content.find("\"name\""), std::string::npos);
    EXPECT_NE(content.find("\"File Test Project\""), std::string::npos);
}

//=================================================================================================
// ProjectManager Tests
//=================================================================================================

class ProjectManagerTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        m_TempDir = std::filesystem::temp_directory_path() / "orogena_pm_test";
        std::filesystem::create_directories(m_TempDir);

        m_Settings = std::make_unique<MockSettings>();
        m_Manager = std::make_unique<ProjectManager>(*m_Settings);
    }

    void TearDown() override
    {
        m_Manager.reset();
        m_Settings.reset();
        std::filesystem::remove_all(m_TempDir);
    }

    std::filesystem::path           m_TempDir;
    std::unique_ptr<MockSettings>   m_Settings;
    std::unique_ptr<ProjectManager> m_Manager;
};

TEST_F(ProjectManagerTest, InitialStateIsClosed)
{
    EXPECT_FALSE(m_Manager->IsProjectOpen());
    EXPECT_EQ(ProjectState::CLOSED, m_Manager->GetState());
    EXPECT_FALSE(m_Manager->GetProjectInfo().has_value());
    EXPECT_FALSE(m_Manager->GetProjectPath().has_value());
}

TEST_F(ProjectManagerTest, CreateProjectOpensProject)
{
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "New World"));

    EXPECT_TRUE(m_Manager->IsProjectOpen());
    EXPECT_EQ(ProjectState::OPEN, m_Manager->GetState());

    auto info = m_Manager->GetProjectInfo();
    ASSERT_TRUE(info.has_value());
    EXPECT_EQ("New World", info->name);
}

TEST_F(ProjectManagerTest, CreateProjectCreatesFiles)
{
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "File Check"));

    // Project is created in a subdirectory: TempDir/File_Check/File_Check.oro
    std::filesystem::path expected_dir = m_TempDir / "File_Check";
    std::filesystem::path expected_project = expected_dir / "File_Check.oro";
    EXPECT_TRUE(std::filesystem::exists(expected_dir));
    EXPECT_TRUE(std::filesystem::is_directory(expected_dir));
    EXPECT_TRUE(std::filesystem::exists(expected_project));
}

TEST_F(ProjectManagerTest, CreateProjectWithSpacesInName)
{
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "My Epic World"));

    // Project is created in a subdirectory with spaces replaced by underscores
    std::filesystem::path expected_dir = m_TempDir / "My_Epic_World";
    std::filesystem::path expected = expected_dir / "My_Epic_World.oro";
    EXPECT_TRUE(std::filesystem::exists(expected_dir));
    EXPECT_TRUE(std::filesystem::exists(expected));

    auto info = m_Manager->GetProjectInfo();
    ASSERT_TRUE(info.has_value());
    EXPECT_EQ("My Epic World", info->name); // Display name preserved
}

TEST_F(ProjectManagerTest, CreateProjectFailsWithEmptyName)
{
    EXPECT_FALSE(m_Manager->CreateProject(m_TempDir, ""));
    EXPECT_FALSE(m_Manager->IsProjectOpen());
}

TEST_F(ProjectManagerTest, CreateProjectFailsWhenProjectOpen)
{
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "First"));
    EXPECT_FALSE(m_Manager->CreateProject(m_TempDir, "Second"));
}

TEST_F(ProjectManagerTest, OpenProjectLoadsExistingProject)
{
    // First create a project
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "Existing"));
    auto original_path = m_Manager->GetProjectPath();
    ASSERT_TRUE(m_Manager->CloseProject());

    // Now open it
    ASSERT_TRUE(m_Manager->OpenProject(*original_path));
    EXPECT_TRUE(m_Manager->IsProjectOpen());

    auto info = m_Manager->GetProjectInfo();
    ASSERT_TRUE(info.has_value());
    EXPECT_EQ("Existing", info->name);
}

TEST_F(ProjectManagerTest, OpenProjectFailsForNonexistentFile)
{
    std::filesystem::path nonexistent = m_TempDir / "nonexistent.oro";
    EXPECT_FALSE(m_Manager->OpenProject(nonexistent));
    EXPECT_FALSE(m_Manager->IsProjectOpen());
}

TEST_F(ProjectManagerTest, CloseProjectClosesProject)
{
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "ToClose"));
    ASSERT_TRUE(m_Manager->CloseProject());

    EXPECT_FALSE(m_Manager->IsProjectOpen());
    EXPECT_EQ(ProjectState::CLOSED, m_Manager->GetState());
}

TEST_F(ProjectManagerTest, CloseProjectReturnsTrueWhenAlreadyClosed)
{
    EXPECT_TRUE(m_Manager->CloseProject());
}

TEST_F(ProjectManagerTest, SaveProjectUpdatesModifiedTime)
{
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "SaveTest"));

    auto    initial_info = m_Manager->GetProjectInfo();
    int64_t initial_modified = initial_info->modifiedTimestamp;

    // Wait a bit and save
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_TRUE(m_Manager->SaveProject());

    auto updated_info = m_Manager->GetProjectInfo();
    EXPECT_GE(updated_info->modifiedTimestamp, initial_modified);
}

TEST_F(ProjectManagerTest, MarkModifiedChangesState)
{
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "ModifyTest"));
    EXPECT_EQ(ProjectState::OPEN, m_Manager->GetState());

    m_Manager->MarkModified();
    EXPECT_EQ(ProjectState::MODIFIED, m_Manager->GetState());
    EXPECT_TRUE(m_Manager->HasUnsavedChanges());
}

TEST_F(ProjectManagerTest, SaveProjectClearsModifiedState)
{
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "SaveClearTest"));
    m_Manager->MarkModified();
    EXPECT_TRUE(m_Manager->HasUnsavedChanges());

    ASSERT_TRUE(m_Manager->SaveProject());
    EXPECT_FALSE(m_Manager->HasUnsavedChanges());
    EXPECT_EQ(ProjectState::OPEN, m_Manager->GetState());
}

TEST_F(ProjectManagerTest, CloseWithUnsavedChangesReturnsFalse)
{
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "UnsavedTest"));
    m_Manager->MarkModified();

    EXPECT_FALSE(m_Manager->CloseProject(false)); // Don't force
    EXPECT_TRUE(m_Manager->IsProjectOpen());      // Still open
}

TEST_F(ProjectManagerTest, CloseWithForceDiscardsChanges)
{
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "ForceCloseTest"));
    m_Manager->MarkModified();

    EXPECT_TRUE(m_Manager->CloseProject(true)); // Force close
    EXPECT_FALSE(m_Manager->IsProjectOpen());
}

//=================================================================================================
// Recent Projects Tests
//=================================================================================================

TEST_F(ProjectManagerTest, RecentProjectsInitiallyEmpty)
{
    auto recent = m_Manager->GetRecentProjects();
    EXPECT_TRUE(recent.empty());
}

TEST_F(ProjectManagerTest, CreateProjectAddsToRecentProjects)
{
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "Recent1"));

    auto recent = m_Manager->GetRecentProjects();
    ASSERT_EQ(1, recent.size());

    std::string expected_name = "Recent1.oro";
    EXPECT_NE(recent[0].string().find(expected_name), std::string::npos);
}

TEST_F(ProjectManagerTest, OpenProjectAddsToRecentProjects)
{
    // Create and close
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "RecentOpen"));
    auto project_path = *m_Manager->GetProjectPath();
    ASSERT_TRUE(m_Manager->CloseProject());

    // Clear recent to test open specifically
    m_Manager->ClearRecentProjects();
    EXPECT_TRUE(m_Manager->GetRecentProjects().empty());

    // Open
    ASSERT_TRUE(m_Manager->OpenProject(project_path));

    auto recent = m_Manager->GetRecentProjects();
    ASSERT_EQ(1, recent.size());
}

TEST_F(ProjectManagerTest, ClearRecentProjectsWorks)
{
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "ToClear"));
    ASSERT_FALSE(m_Manager->GetRecentProjects().empty());

    m_Manager->ClearRecentProjects();
    EXPECT_TRUE(m_Manager->GetRecentProjects().empty());
}

TEST_F(ProjectManagerTest, RecentProjectsMostRecentFirst)
{
    // Create multiple projects
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "First"));
    ASSERT_TRUE(m_Manager->CloseProject(true));

    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "Second"));
    ASSERT_TRUE(m_Manager->CloseProject(true));

    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "Third"));

    auto recent = m_Manager->GetRecentProjects();
    ASSERT_EQ(3, recent.size());

    // Most recent should be first
    EXPECT_NE(recent[0].string().find("Third"), std::string::npos);
    EXPECT_NE(recent[1].string().find("Second"), std::string::npos);
    EXPECT_NE(recent[2].string().find("First"), std::string::npos);
}

//=================================================================================================
// Callback Tests
//=================================================================================================

TEST_F(ProjectManagerTest, CallbacksInvokedOnCreate)
{
    bool        opened_called = false;
    std::string opened_path;

    ProjectCallbacks callbacks;
    callbacks.onProjectOpened = [&](const std::string& path)
    {
        opened_called = true;
        opened_path = path;
    };

    m_Manager->SetCallbacks(callbacks);
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "CallbackTest"));

    EXPECT_TRUE(opened_called);
    EXPECT_FALSE(opened_path.empty());
}

TEST_F(ProjectManagerTest, CallbacksInvokedOnClose)
{
    bool closed_called = false;

    ProjectCallbacks callbacks;
    callbacks.onProjectClosed = [&]() { closed_called = true; };

    m_Manager->SetCallbacks(callbacks);
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "CloseCallback"));
    ASSERT_TRUE(m_Manager->CloseProject());

    EXPECT_TRUE(closed_called);
}

TEST_F(ProjectManagerTest, CallbacksInvokedOnModified)
{
    bool modified_called = false;

    ProjectCallbacks callbacks;
    callbacks.onProjectModified = [&]() { modified_called = true; };

    m_Manager->SetCallbacks(callbacks);
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "ModCallback"));

    m_Manager->MarkModified();
    EXPECT_TRUE(modified_called);
}

TEST_F(ProjectManagerTest, CallbacksInvokedOnSave)
{
    bool        saved_called = false;
    std::string saved_path;

    ProjectCallbacks callbacks;
    callbacks.onProjectSaved = [&](const std::string& path)
    {
        saved_called = true;
        saved_path = path;
    };

    m_Manager->SetCallbacks(callbacks);
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "SaveCallback"));
    ASSERT_TRUE(m_Manager->SaveProject());

    EXPECT_TRUE(saved_called);
    EXPECT_FALSE(saved_path.empty());
}

TEST_F(ProjectManagerTest, ErrorCallbackInvokedOnFailure)
{
    bool        error_called = false;
    std::string error_message;

    ProjectCallbacks callbacks;
    callbacks.onError = [&](const std::string& error)
    {
        error_called = true;
        error_message = error;
    };

    m_Manager->SetCallbacks(callbacks);

    // Try to create with empty name (should fail)
    EXPECT_FALSE(m_Manager->CreateProject(m_TempDir, ""));
    EXPECT_TRUE(error_called);
    EXPECT_FALSE(error_message.empty());
}

//=================================================================================================
// SaveProjectAs Tests
//=================================================================================================

TEST_F(ProjectManagerTest, SaveAsUpdatesProjectNameAndDatabaseFilename)
{
    // Create initial project (creates TempDir/OriginalProject/OriginalProject.oro)
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "OriginalProject"));

    // Verify original state
    auto original_info = m_Manager->GetProjectInfo();
    ASSERT_TRUE(original_info.has_value());
    EXPECT_EQ("OriginalProject", original_info->name);
    EXPECT_EQ("OriginalProject.db", original_info->databaseFilename);

    // Create .db file in the project subdirectory to simulate database existence
    std::filesystem::path original_dir = m_TempDir / "OriginalProject";
    std::filesystem::path original_db = original_dir / "OriginalProject.db";
    std::ofstream{original_db} << "dummy";
    ASSERT_TRUE(std::filesystem::exists(original_db));

    // Save as new name in the same directory
    std::filesystem::path new_path = original_dir / "RenamedProject.oro";
    ASSERT_TRUE(m_Manager->SaveProjectAs(new_path));

    // Verify updated state
    auto new_info = m_Manager->GetProjectInfo();
    ASSERT_TRUE(new_info.has_value());
    EXPECT_EQ("RenamedProject", new_info->name);
    EXPECT_EQ("RenamedProject.db", new_info->databaseFilename);

    // Verify new .oro file exists
    EXPECT_TRUE(std::filesystem::exists(new_path));

    // Verify new .db file was created (copied from original)
    std::filesystem::path new_db = original_dir / "RenamedProject.db";
    EXPECT_TRUE(std::filesystem::exists(new_db));
}

TEST_F(ProjectManagerTest, SaveAsToDifferentDirectory)
{
    // Create initial project (creates TempDir/OriginalDir/OriginalDir.oro)
    ASSERT_TRUE(m_Manager->CreateProject(m_TempDir, "OriginalDir"));

    // Create .db file in the project subdirectory
    std::filesystem::path original_dir = m_TempDir / "OriginalDir";
    std::filesystem::path original_db = original_dir / "OriginalDir.db";
    std::ofstream{original_db} << "dummy";

    // Create new directory
    std::filesystem::path new_dir = m_TempDir / "subdir";
    std::filesystem::create_directories(new_dir);

    // Save as to new directory with new name
    std::filesystem::path new_path = new_dir / "NewDirProject.oro";
    ASSERT_TRUE(m_Manager->SaveProjectAs(new_path));

    // Verify files in new directory
    EXPECT_TRUE(std::filesystem::exists(new_path));
    EXPECT_TRUE(std::filesystem::exists(new_dir / "NewDirProject.db"));

    // Verify project info updated
    auto info = m_Manager->GetProjectInfo();
    EXPECT_EQ("NewDirProject", info->name);
    EXPECT_EQ("NewDirProject.db", info->databaseFilename);
}
