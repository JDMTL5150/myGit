
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <ctime>
#include <fstream>
#include <cctype>

#include "StageFile.hpp"
#include "Repository.hpp"
#include "Branch.hpp"

using namespace std;

Repository::Repository(string repoName) :
    repoPath(".jvc"),repo_name(repoName) {}

Repository::Repository() :
    repoPath(".jvc") {}

vector<string> fileToVec(const string& filename) {
    vector<string> vec;
    ifstream stream(filename,std::ios::in);
    string line;
    while(getline(stream,line)) vec.push_back(line);
    stream.close();
    return vec;
}

void Repository::init() {
    if(!filesystem::exists(repoPath)){
        filesystem::create_directory(repoPath);
        filesystem::create_directory(repoPath + "/snapshots");
        filesystem::create_directory(repoPath + "/branches");
        filesystem::create_directory(repoPath + "/headers");
        filesystem::create_directory(repoPath + "/etc");
        // init branch files
        ofstream comFile(repoPath + "/etc/commit_ids.txt",std::ios::out);
        ofstream branchesFile(repoPath + "/branches/branches.txt",std::ios::app);
        ofstream file(repoPath + "/branches/main",std::ios::out);
        ofstream headFile(repoPath + "/branches/HEAD",std::ios::out);
        headFile << "main\n";
        branchesFile << "main\n";
        file.close();
        comFile.close();
        headFile.close();
        branchesFile.close();

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

void Repository::printBranches() {
    string line;
    ifstream infile(repoPath + "/branches/branches.txt",std::ios::in);
    if(!infile) {
        cerr << "[-] could not find the branches file." << endl;
        return;
    }
    cout << "[*] BRANCHES [*]" << endl;
    while(getline(infile,line)) {
        cout << line << endl;
    }
    infile.close();
}

void Repository::removeStageFile(const std::string& filename) {
    vector<string> lines;
    ifstream infile(repoPath + "/stage.txt",std::ios::in);
    if(!infile) {
        cerr << "[-] Could not find staging area." << endl;
        return;
    }
    
    string line;
    while(getline(infile,line)) lines.push_back(line);
    infile.close();
    ofstream out(repoPath + "/stage.txt",std::ios::out);
    for(const auto& entry : lines) {
        string filepath = entry.substr(0,entry.find(' '));
        string file_name = filesystem::path(filepath).filename().string();
        if(file_name == filename) continue;
        out << entry + "\n";
    }
    
    out.close();
    
    cout << "[*] File: " << filename << endl;
    cout << "[*] if file exists, it will be removed from staging area." << endl;
}

void Repository::makeBranch(const std::string& branch_name) {
    vector<string> currCommits;
    if(branch_name == "HEAD"){
        cerr << "[-] Reserved keyword. - " << branch_name;
        return;
    }
    
    string inPath = repoPath + "/etc/commit_ids.txt";
    ifstream infile(inPath);

    if(!infile) {
        cerr << "[-] Could not find commit_ids." << endl;
        return;
    } // getting commit ids to find the tip

    if(filesystem::file_size(inPath) == 0) {
        cout << "[*] no commit ids yet." << endl;
        return;
    }

    string line;
    while(getline(infile,line)) currCommits.push_back(line);
    string commitTip = currCommits[currCommits.size() - 1];
    Branch newBranch(branch_name,commitTip);
    
    // add new branch to history
    ofstream hFile(repoPath + "/branches/branches.txt",std::ios::app);
    hFile << branch_name + "\n";
    hFile.close();

    cout << "[+] Branch made successfully. - " + branch_name << endl;
}

void Repository::viewLogs() const {
    ifstream infile(repoPath + "/" + "commits.txt");
    string commitId,commitMsg,timestamp,numFiles;
    int logNum = 0;

    if(filesystem::file_size(repoPath + "/" + "commits.txt") == 0) {
        cout << "[-] No logs to view." << endl;
        return;
    }

    while(getline(infile,commitId)) {
        if(commitId.empty()) return;

        if(logNum != 0 && logNum % 6 == 0) {
            string choice;
            cout << "press q to exit or any to show more: ";
            cin >> choice;
            if(choice == "q" || choice == "Q") return;
        }

        getline(infile,commitMsg);
        getline(infile,timestamp);
        getline(infile,numFiles);
        
        cout << "[+] Commit Log " << to_string(logNum + 1) << ":" << endl;
        cout << "ID: " << commitId << endl;
        cout << "MESSAGE: " << commitMsg << endl;
        cout << "TIMESTAMP: " << timestamp << endl;
        cout << "NUM_FILES:" << numFiles << endl;
        cout << endl;

        logNum++;
    }
}

void Repository::checkout(const string& commitID) {
    string branchName = commitID;
    string resolvedId = commitID;
    Branch b(resolvedId);

    if(b.isABranch()) {
        resolvedId = b.loadRef();
        cout << "[+] Checking out branch - " << branchName << endl;
    }

    string headerfilePath = repoPath + "/headers/" + resolvedId + "_h.txt";

    if(!filesystem::exists(headerfilePath)) {
        cout << "[-] Commit not found - " << resolvedId << endl;
        return;
    }

    cout << "[+] Checking out commit id: " << resolvedId << endl;
    string filename;
    int iteration = 1;
    ifstream infile(headerfilePath,std::ios::in);
    while(getline(infile, filename)) {
        if (filename.empty()) continue;
        StageFile sf(repoPath + "/snapshots/" + resolvedId
        + "/" + resolvedId + "_" + to_string(iteration) + ".txt");
        if(sf.failed()) return;
        ofstream outfile(filename, std::ios::out);
        outfile << sf.getContent();
        outfile.close();
        cout << "[+] file restored: " << filename << endl;
        iteration++;
    }
    infile.close();
    string time = makeTimestamp();
    cout << "[+] Checkout complete. at: " << time << endl;
}

void Repository::printRepoData() {
    string confPath = repoPath + "/config.txt";
    if(!filesystem::exists(confPath)) {
        cerr << "[-] could not find the repos config." << endl;
        return;
    }
    ifstream infile(confPath,std::ios::in);
    string name,uuid;
    getline(infile,name);
    getline(infile,name);
    getline(infile,uuid);
    cout << "[+] Repo Name: " << name << endl;
    cout << "[+] Repo UUID: " << uuid << endl;
    infile.close();
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
    Commit c(message,repoPath);
    string commitIdFilePath = repoPath + "/etc/commit_ids.txt";
    if(!filesystem::exists(commitIdFilePath)) {
        cerr << "[-] commits file not found." << endl;
    }
    ifstream infile(repoPath + "/stage.txt", std::ios::in | std::ios::out);
    auto iteration = 1;
    string filename = "";
    string hash = "";
    filesystem::create_directory(repoPath + "/snapshots/" + c.getId());
    if(filesystem::file_size(repoPath + "/" + "stage.txt") == 0) {
        cout << "[-] no staged files to commit." << endl;
        return;
    }

    ofstream commitFile(commitIdFilePath, std::ios::app);
    commitFile << c.getId() + "\n";
    commitFile.close();

    ifstream hIn(repoPath + "/branches/HEAD",std::ios::in);
    hIn >> repo_name;
    hIn.close();

    string headPath = repoPath + "/branches/" + repo_name;
    ofstream stream(headPath,std::ios::out);
    stream << c.getId();
    stream.close();

    ofstream headerFile(repoPath + "/headers/" + c.getId() + "_h.txt", std::ios::out);
    while(infile >> filename >> hash) {
        headerFile << filename + "\n";
        StageFile sf(filename);
        if(sf.failed()){
            cerr << "[-] failed to load the StageFile. - " << filename << endl;
            return; 
        }
        ofstream outfile(repoPath + "/snapshots/"
            + c.getId() + "/" + c.getId() + "_" + to_string(iteration) + ".txt", std::ios::out);
        outfile << sf.getContent();
        outfile.close();
        iteration++;
    }
    infile.close();
    headerFile.close();

    ofstream parentStream(repoPath + "/snapshots/" + c.getId() + "/parent",std::ios::out);
    string parent = c.loadLatest();
    parentStream << parent;
    parentStream.close();

    c.setNumFiles(iteration - 1);  
    logbook.push_back(c);           
    cout << "[+] " + c.getId() + " was committed successfully." << endl;
    ofstream f(repoPath + "/" + "stage.txt", std::ios::trunc);
    f.close();
    log();
}

void Repository::add(const std::string& filename) {
    if(filename == ".") {
        for(const auto& entry : filesystem::recursive_directory_iterator(".")) {
            string filepath = entry.path().string();
            // skipping repo directory and real git
            if(filepath.find(".jvc") != string::npos) continue;
            if(filepath.find(".git") != string::npos) continue;
            // checking for regular file
            if(entry.is_regular_file()) {
                StageFile sf(filepath);
                if(sf.failed()) {
                    cerr << "[-] failed to open file - " << filepath << endl;
                    continue;
                }
                stagingArea.push_back(sf);
            }
        }
        return;
    }

    ifstream file(filename,std::ios::in);
    if(!file){
        cout << "[-] file entered does not exist." << endl;
        return;
    }
    StageFile sf(filename);
    if(sf.failed()) {
        cout << "[-] Could not find file: " << filename << endl;
        return;
    }
    stagingArea.push_back(sf);
    file.close();
}

void Repository::loadStagingArea() const {
    string path = repoPath + "/" + "stage.txt";
    string filename,hash;
    vector<string> files;
    vector<StageFile> sf_v;
    ofstream fileout(path,std::ios::app);
    ifstream infile(path,std::ios::in);
    while(infile >> filename >> hash){
        files.push_back(filename);
    }
    infile.close();
    int numStaged = 0;
    for(auto file : stagingArea) {
        if (vectorContains(files, file.getFileName())) {
           cout << "[-] File was already staged, Skipping... - " << filename << endl;
           continue;  
        }
        cout << "[+] File added to staging area." << endl;
        file.writeToFile(fileout);
        numStaged++;
    }
    fileout.close();
    cout << "[+] Staged " << to_string(numStaged) << " files." << endl;
}

void Repository::log() const {
    ofstream file(repoPath + "/" + "commits.txt",std::ios::app);
    for(auto log : logbook){
        file << log.getId() + "\n";
        file << log.getMessage() + "\n";
        file << log.getTimestamp() + "\n";
        file << to_string(log.getNumFiles()) + "\n";
    }
    file.close();
    cout << "[+] commit saved successfully." << endl;
}

void computeDiff(vector<string> v1,vector<string> v2);

bool commitExists(const string& comID) {
    vector<string> commits;
    const string path = ".jvc/etc/commit_ids.txt";
    ifstream infile(path,std::ios::in);
    string name;
    while(getline(infile,name)) commits.push_back(name);
    infile.close();
    return vectorContains(commits,comID);
}

void Repository::diff(const std::string& commitId) {

    if(!commitExists(commitId)) {
        cout << "[-] commit does not exist. - " << commitId << endl;
        return;
    }

    Commit c(commitId);
    c.loadParent();
    string commitId2 = c.getParentId();

    if(commitId2.empty()) {
        cout << "[*] No parent commit (root commit). Nothing to diff against." << endl;
        return;
    }

    string path1 = repoPath + "/headers/" + commitId + "_h.txt";
    string path2 = repoPath + "/headers/" + commitId2 + "_h.txt";

    string name;
    vector<string> files1;
    vector<string> files2;

    ifstream infile_1(path1, std::ios::in);
    ifstream infile_2(path2, std::ios::in);
    if(!infile_1) {
        cerr << "[-] Could not open header: " << path1 << endl;
        return;
    }
    if(!infile_2) {
        cerr << "[-] Could not open header: " << path2 << endl;
        return;
    }
    while(getline(infile_1,name)) {
        StageFile sf(name);
        if(sf.failed()){
            cerr << "[-] error opening file. Skipping... - " << name << endl;
            continue;
        }
        files1.push_back(name);
    }

    while(getline(infile_2,name)) {
        StageFile sf(name);
        if(sf.failed()){
            cerr << "[-] error opening file. Skipping... - " << name << endl;
            continue;
        }
        files2.push_back(name);
    }

    for(auto f_name : files1) {
        vector<string> lines_file1;
        vector<string> lines_file2;

        if(vectorContains(files2,f_name)){
            int fIndex1 = vectorIndex(files1,f_name);
            int fIndex2 = vectorIndex(files2,f_name);

            string in_path1 = repoPath + "/snapshots/" + commitId + "/"
            + commitId + "_" + to_string(fIndex1 + 1) + ".txt";
            
            string in_path2 = repoPath + "/snapshots/" + commitId2 + "/"
            + commitId2 + "_" + to_string(fIndex2 + 1) + ".txt";

            ifstream f1(in_path1,std::ios::in);
            ifstream f2(in_path2,std::ios::in);
            if(!f1 || !f2) {
                cerr << "[-] Error opening files." << endl;
                return;
            }

            string line;
            while(getline(f1,line)) lines_file1.push_back(line); 
            while(getline(f2,line)) lines_file2.push_back(line);

            cout << "FILE: " << f_name << endl;
            computeDiff(lines_file1,lines_file2);
            f1.close();
            f2.close();
        }
    }
    cout << "[+] Diff operation complete." << endl;
}

void computeDiff(vector<string> v1,vector<string> v2) {
    // v1: previous commit v2: new commit
    auto changesMade = 0;
    auto minSize = v1.size() < v2.size() ? v1.size() : v2.size();
    for(size_t i = 0; i < minSize; i++) {
        if(v1[i] != v2[i]) {
            cout << "+ " << v2[i] << endl;
            cout << "- " << v1[i] << endl;
            changesMade++;
        }
    }
    // checking if one file is longer than the other
    if(v1.size() > v2.size()) {
        for(auto x = minSize; x < v1.size(); x++){
            cout << "- " << v1[x] << endl;
            changesMade++;
        }
    }
    // lines only in v2 (added)
    else if(v2.size() > v1.size()) {
        for(size_t y = minSize; y < v2.size(); y++){
            cout << "+ " << v2[y] << endl;
            changesMade++;
        }
    }
    cout << endl;
    cout << "[+] lines modified: " << to_string(changesMade) << endl;
}

void Repository::clearStagingArea() {
    string path = repoPath + "/stage.txt";
    if(!filesystem::exists(path)) {
        cerr << "[-] Could not locate staging area." << endl;
        return;
    }
    else if(filesystem::file_size(path) == 0) {
        cout << "[-] Staging area is already empty. " << path << endl;
        return;
    }
    ofstream file(path,std::ios::trunc);
    file.close();
    cout << "[+] Staging area cleared." << endl;
}

void Repository::init_config() {
    ofstream commitFile(repoPath + "/commits.txt",std::ios::out);
    ofstream configFile(repoPath + "/config.txt",std::ios::out);
    ofstream stageFile(repoPath + "/stage.txt",std::ios::out);
    
    configFile << "# Repository's configuration\n";
    configFile << repo_name + "\n"; 
    configFile << uuid + "\n";

    commitFile.close();
    configFile.close();
    stageFile.close();
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