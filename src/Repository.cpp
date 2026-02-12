// class Repository {
//     public:
//         Repository();
//         void init();
//         void add(const std::string& filename);
//         void commit(const std::string& message);
//         void log() const;
//         void checkout(const std::string& commitID);
    
//     private:
//         std::string repoPath = ".mygit";
// };

#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <ctime>
#include <fstream>
#include <cctype>
#include "Repository.hpp"

using namespace std;

Repository::Repository(string repoName) :
    repoPath(".mygit"),repo_name(repoName) {}

void Repository::init() {
    if(!filesystem::exists(repoPath)){
        filesystem::create_directory(repoPath);
        filesystem::create_directory(repoPath + "snapshots");
        this->uuid = generateUUID();
        cout << "[+] repository was created." << endl;
        cout << "[+] Repo Name: " + repo_name << endl;
        cout << "[+] UUID: " + uuid << endl;
        init_config();
    }
    else{
        cout << "[-] A repository already exists." << endl;
    }
}

void Repository::addlog(const std::string& commitId,const std::string& commitMessage) const {
    std::time_t now = std::time(nullptr);
    string timestamp = std::ctime(&now);
    timestamp.pop_back();
    Log::Log l = new Log(commitId,commitMessage,timestamp);
    logbook.push_back(l);
}

void Repostiory::add(const std::string& filename) {
    ifstream file(filename,std::ios::in);
    if(!file) return;
    
}

void Repository::init_config() {
    ofstream commitFile(repoPath + "/commits.txt",std::ios::out);
    ofstream configFile(repoPath + "/config.txt",std::ios::out);

    configFile << "# Repository's configuration\n";
    configFile << "NAME: " + repo_name + "\n"; 
    configFile << "UUID: " + uuid + "\n";
}

string Repository::generateUUID() const {
    int count = 0;
    srand(time(0));
    string uuid = "";
    while(count < UUID_LENGTH){
        int x = rand() % 10;
        uuid += to_string(x);
        count++;

        if(count % 8 == 0 && count < 24) uuid += "-";
    }
    return uuid;
}

string checkoutHash() const {
    string checkout = "";
    unsigned char c = 0;
    srand(time(0));
    int max = 126;
    int min = 32;
    int count = 0;
    while(count < 8){
        c = rand() % (max - min + 1) + min;
        if(!isalnum(c)) continue;
        checkout += c;
        count++;
    }
    return checkout;
}