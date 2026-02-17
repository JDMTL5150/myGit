
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Commit.hpp"

using namespace std;

Commit::Commit() {}

Commit::Commit(const string& commitId) :
    id(commitId) {}

Commit::Commit(const string& msg,const string& repoName):
    message(msg),timestamp(makeTimestamp()),numFiles(0) {
        string path = repoName + "/etc/commit_ids.txt";
        string comID;
        vector<string> commits;
        bool isUnique = true;
        ifstream commFile(path,std::ios::in);
        // reading commits into a vector
        while(getline(commFile,comID)){
            commits.push_back(comID);
        }
        // checking for unique commit id
        while(true){
            id = checkoutHash();
            for(auto comId : commits){
                if(comID == id) {
                    isUnique = false;
                }
            }
            if(!isUnique) continue;
            else break;
        }
        if(commits.size() > 0) parentId = commits.back();
    }

string Commit::getParentId() const {
    return parentId;
}

string Commit::getMessage() const {
    return message;
}

int Commit::getNumFiles() const {
    return numFiles;
}

void Commit::setNumFiles(int num) {
    numFiles = num;
}

void Commit::loadParent() {
    ifstream file(".jvc/snapshots/" + id + "/parent", std::ios::in);
    if(file) file >> this->parentId;
    file.close();
}

string Commit::loadLatest() {
    vector<string> comms;
    ifstream file(".jvc/etc/commit_ids.txt",std::ios::in);
    string line;
    while(getline(file,line)) comms.push_back(line);
    if(comms.empty()) return "";
    return comms.back();
}

vector<string> Commit::loadLastCommitHashes() {
    std::hash<string> hasher;
    vector<string> hashes;
    string latest = loadLatest();
    if(latest.empty()) {
        cerr << "[-] Error loading latest commit." << endl;
        return null;
    }
    int iteration = 1;
    string f,content;
    while(true) {
        ifstream file(".jvc/snapshots/" + latest + "/"
        + latest + "_" + to_string(iteration) + ".txt"
        ,std::ios::in);
        if(!file) break;
        while(getline(file,f)) content += f;
        hashes.push_back(hasher(content));
        file.close();
    }
    return hashes;  
}

// get number of files in current objects commit id
int Commit::getNumFiles() {
    ifstream file(".jvc/commits.txt",std::ios::in);
    string id,message,timestamp;
    string numFiles;
    while(getline(file,id)) {
        getline(file,message);
        getline(file,timestamp);
        getline(file,numFiles);
        if(id == this->id) {
            return numFiles
        }
    }
}

string Commit::getId() const {
    return id;
}

string Commit::getTimestamp() const {
    return timestamp;
}

string makeTimestamp() {
    std::time_t now = std::time(nullptr);
    string timestamp = std::ctime(&now);
    timestamp.pop_back();
    return timestamp;
}

string checkoutHash() {
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
