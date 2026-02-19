#pragma once

#include <string>
#include <fstream>
#include <vector>

class Commit {
    private:
        std::string id;
        std::string parentId;
        std::string message;
        std::string timestamp;
        int numFiles;
    
    public:
        Commit();
        Commit(const std::string& msg,const std::string& path);
        Commit(const std::string& commitId);
    
        std::string getParentId() const;
        int getNumFiles();
        int getNumFiles_g();
        std::string getId() const;
        std::string getMessage() const;
        std::string getTimestamp() const;

        void setCommitId(const std::string& commID);
        void setNumFiles(int num);
        void loadParent();
        std::string loadLatest();
    
        std::vector<std::string> loadLastCommitHashes();
        void saveMetadata(const std::string& repoPath) const;
};

std::string checkoutHash(const std::string& content);
std::string makeTimestamp();
int getNumCommits();
    
