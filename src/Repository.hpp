
#ifndef REPOSITORY_HPP_
#define REPOSITORY_HPP_

#include <string>
#include <vector>
#include "Commit.hpp"
#include "StageFile.hpp"

static constexpr auto UUID_LENGTH = 24;

class Repository {
    public:
        Repository();
        Repository(std::string repoName);
        void init();
        void add(const std::string& filename);
        void commit(const std::string& message);
        void log() const;
        void checkout(const std::string& commitID);
        void loadStagingArea() const;
        void stageStatus(const std::string& stagefile) const;
        void viewLogs() const;

        void setRepoName(const std::string& name);
        
        private:
        std::string generateUUID() const;
        std::vector<Commit> logbook;
        std::vector<StageFile> stagingArea;
        void init_config();
        std::string repoPath;
        std::string uuid;
        std::string repo_name;
        
};

#endif