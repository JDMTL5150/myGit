
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
        ifstream commFile(path,std::ios::in);
        // reading commits into a vector
        while(getline(commFile,comID)){
            commits.push_back(comID);
        }
        if(commits.size() > 0) parentId = commits.back();
    }

string Commit::getParentId() const {
    return parentId;
}

string Commit::getMessage() const {
    return message;
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
        return hashes;
    }
    int iteration = 1;
    string f,content;
    while(true) {
        ifstream file(".jvc/snapshots/" + latest + "/"
        + latest + "_" + to_string(iteration) + ".txt"
        ,std::ios::in);
        if(!file) break;
        while(getline(file,f)) content += f;
        hashes.push_back(to_string(hasher(content)));
        file.close();
    }
    return hashes;  
}

int Commit::getNumFiles() {
    return numFiles;
}

// get number of files in current objects commit id
int Commit::getNumFiles_g() {
    ifstream file(".jvc/commits.txt",std::ios::in);
    string id,message,timestamp;
    string numFiles;
    while(getline(file,id)) {
        getline(file,message);
        getline(file,timestamp);
        getline(file,numFiles);
        if(id == this->id) {
            return stoi(numFiles);
        }
    }
    return -1;
}

void Commit::setCommitId(const string& commId) {
    this->id = commId;
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

int getNumCommits() {
    auto i = 0;
    string path = ".jvc/etc/commit_ids.txt";
    ifstream infile(path,std::ios::in);
    if(!infile) return -1;
    string commit;
    while(getline(infile,commit)) i++;
    infile.close();
    return i;
}

string checkoutHash(const string& content) {
    std::hash<string> hasher;
    string checkout = content;

    checkout = to_string(hasher(checkout));

    return checkout;
}
