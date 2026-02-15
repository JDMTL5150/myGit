
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
        Commit(const std::string& msg,const std::string& path);
        Commit(const std::string& commitId);
    
        std::string getParentId() const;
        int getNumFiles() const;
        std::string getId() const;
        std::string getMessage() const;
        std::string getTimestamp() const;

        void setNumFiles(int num);
        void loadParent();
        std::string loadLatest();
    
        void saveMetadata(const std::string& repoPath) const;
};

std::string checkoutHash();
std::string makeTimestamp();
    
