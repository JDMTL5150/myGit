
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <ctime>
#include <fstream>
#include <cctype>

#include "StageFile.hpp"
#include "Repository.hpp"

using namespace std;

Repository::Repository(string repoName) :
    repoPath(".mygit"),repo_name(repoName) {}

Repository::Repository() :
    repoPath(".mygit") {}

void Repository::init() {
    if(!filesystem::exists(repoPath)){
        filesystem::create_directory(repoPath);
        filesystem::create_directory(repoPath + "/snapshots");
        filesystem::create_directory(repoPath + "/headers");
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

void Repository::setRepoName(const string& name) {
    repo_name = name;
}

void Repository::viewLogs() const {
    ifstream infile(repoPath + "/" + "commits.txt");
    string commitId,commitMsg,timestamp;
    int logNum = 0;

    if(filesystem::file_size(repoPath + "/" + "commits.txt") == 0) {
        cout << "[-] No logs to view." << endl;
        return;
    }

    while(getline(infile,commitId)) {
        getline(infile,commitMsg);
        getline(infile,timestamp);
        cout << "[+] Commit Log " << to_string(logNum + 1) << ":" << endl;
        cout << "ID: " << commitId << endl;
        cout << "MESSAGE: " << commitMsg << endl;
        cout << "TIMESTAMP: " << timestamp << endl;
        logNum++;
    }
}

void Repository::checkout(const string& commitID) {
    string headerfilePath = repoPath + "headers/" + commitID + "_h.txt";
    if(!filesystem::exists(headerfilePath)) {
        cout << "[-] Commit not found - " << commitID << endl;
        return;
    }
    cout << "[+] Checking out commit id: " << commitID << endl;
    string filename;
    StageFile sf(filename);
    ifstream infile(headerfilePath,std::ios::in);
    while(getline(infile,filename)){
        ofstream outfile(repoPath + "/" + filename,std::ios::out);
        outfile << sf.getContent();
        outfile.close();
        cout << "[+] file restored: " << filename << endl;
    }
    infile.close();
    string time = makeTimestamp();
    cout << "[+] Checkout complete. at: " << time << endl;
}

void Repository::stageStatus(const string& stagefile) const {
    if(!filesystem::exists(repoPath + "/" + stagefile)) {
        cerr << "[-] Could not find staging area." << endl;
        return;
    }
    if(filesystem::file_size(repoPath + "/" + "stage.txt") == 0) {
        cout << "[-] no staged files to view." << endl;
        return;
    }
    string filename,hash;
    int stageNum = 0;
    ifstream infile(repoPath + "/" + stagefile,std::ios::in);
    while(infile >> filename >> hash) {
        cout << "[+] Staged File " << to_string(stageNum + 1) << ":" << endl;
        cout << "FileName: " << filename << endl;
        cout << "File Hash: " << hash << endl;
        stageNum++;
    }
    infile.close();
}

void Repository::commit(const std::string& message) {
    Commit c(message);
    ifstream infile(repoPath + "/stage.txt",std::ios::in | std::ios::out);
    logbook.push_back(c);
    auto iteration = 1;
    string filename = "";
    string hash = "";
    filesystem::create_directory(repoPath + "/snapshots/" + c.getId());
    if(filesystem::file_size(repoPath + "/" + "stage.txt") == 0) {
        cout << "[-] no staged files to commit." << endl;
        return;
    }
    ofstream headerFile(repoPath + "/headers/" + c.getId() + "_h.txt"
    ,std::ios::out);
    while(infile >> filename >> hash){
        headerFile << filename + "\n";
        StageFile sf(filename);
        ofstream outfile(repoPath + "/snapshots/"
        + c.getId() + "/" + c.getId() + "_" + to_string(iteration) + ".txt"
        ,std::ios::out);
        outfile << sf.getContent();
        outfile.close();
        iteration++;
    }
    cout << "[+] " + c.getId() + " was committed successfully." << endl;
    infile.close();
    headerFile.close();
    ofstream f(repoPath + "/" + "stage.txt",std::ios::trunc);
    f.close();
    log();
}

void Repository::add(const std::string& filename) {
    ifstream file(filename,std::ios::in);
    if(!file){
        cout << "[-] file entered does not exist." << endl;
        return;
    }
    StageFile sf(filename);
    stagingArea.push_back(sf);
    cout << "[+] File added to staging area." << endl;
    file.close();
}

void Repository::loadStagingArea() const {
    ofstream fileout(repoPath + "/" + "stage.txt",std::ios::out);
    for(auto file : stagingArea) {
        file.writeToFile(fileout);
    }
    fileout.close();
}

void Repository::log() const {
    ofstream file(repoPath + "/" + "commits.txt",std::ios::app);
    for(auto log : logbook){
        file << log.getId() + "\n";
        file << log.getMessage() + "\n";
        file << log.getTimestamp() + "\n";
    }
    file.close();
    cout << "[+] commit saved successfully." << endl;
}

void Repository::init_config() {
    ofstream commitFile(repoPath + "/commits.txt",std::ios::out);
    ofstream configFile(repoPath + "/config.txt",std::ios::out);
    ofstream stageFile(repoPath + "/stage.txt",std::ios::out);
    
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