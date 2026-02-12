
#ifndef REPOSITORY_HPP_
#define REPOSITORY_HPP_

#include <string>
#include <vector>
#include "Log.hpp"

static constexpr auto UUID_LENGTH = 24;

class Repository {
    public:
        Repository(std::string repoName);
        void init();
        void add(const std::string& filename);
        void commit(const std::string& message);
        void addlog(const std::string& commitId,const std::string& commitMessage) const;
        void checkout(const std::string& commitID);
        
        private:
        std::string generateUUID() const;
        std::vector<Log> logbook;
        void init_config();
        std::string repoPath;
        std::string uuid;
        std::string repo_name;
        
};

std::string checkoutHash() const;

#endif