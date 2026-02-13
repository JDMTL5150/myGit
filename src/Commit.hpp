#include <string>

class Commit {
    private:
        std::string id;
        std::string message;
        std::string timestamp;
    
    public:
        Commit(const std::string& msg);
    
        std::string getId() const;
        std::string getMessage() const;
        std::string getTimestamp() const;
    
        void saveMetadata(const std::string& repoPath) const;
};

std::string checkoutHash();
std::string makeTimestamp();
    