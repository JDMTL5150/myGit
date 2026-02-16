
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

// --- Constructors ---
Repository::Repository(string repoName) :
    repoPath(".jvc"),repo_name(repoName) {}

Repository::Repository() :
    repoPath(".jvc") {}

// Read file into vector of lines.
vector<string> fileToVec(const string& filename) {
    vector<string> vec;
    ifstream stream(filename,std::ios::in);
    string line;
    while(getline(stream,line)) vec.push_back(line);
    stream.close();
    return vec;
}

// Create repo dirs and initial branch/HEAD.
void Repository::init() {
    if(!filesystem::exists(repoPath)){
        filesystem::create_directory(repoPath);
        filesystem::create_directory(repoPath + "/snapshots");
        filesystem::create_directory(repoPath + "/branches");
        filesystem::create_directory(repoPath + "/headers");
        filesystem::create_directory(repoPath + "/etc");
        // init branch files
        ofstream comFile(repoPath + "/etc/commit_ids.txt",std::ios::out);
        // main branch and HEAD pointer
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

string Repository::loadHead() {
    string head;
    ifstream file(repoPath + "/branches/HEAD",std::ios::in);
    if(!file) {
        cerr << "[-] Could not find head branch." << endl;
        return "";
    }
    file >> head;
    file.close();
    return head;
}

// List branch names from branches.txt.
void Repository::printBranches() {
    string line,currentHead;

    currentHead = loadHead();
    if(currentHead.empty()) return;
    
    ifstream infile(repoPath + "/branches/branches.txt",std::ios::in);
    if(!infile) {
        cerr << "[-] could not find the branches file." << endl;
        return;
    }
    cout << "[*] BRANCHES [*]" << endl;
    while(getline(infile,line)) {
        if(line == currentHead) cout << "(current) " << line << endl;
        else cout << line << endl; 
    }
    infile.close();
}

// locate .jvcignore if there is one.
string Repository::findIgnore() {
    auto cwd = filesystem::current_path();
    cwd = filesystem::absolute(cwd).string();
    while(!cwd.empty()) {
        if(filesystem::exists(cwd / ".jvcignore")) {
            return cwd.string();
        }
        cwd = cwd.parent_path();
    }
    return ""; // empty if not found
}

// load the contents of the .jvcignore to be examined
vector<string> Repository::loadIgnore(const string& parent_path) {
    vector<string> toIgnores;
    ifstream ignoreFile(parent_path + "/" + ".jvcignore");
    string line;
    while(getline(ignoreFile,line)) toIgnores.push_back(line);
    ignoreFile.close();
    return toIgnores;
}

void Repository::addIgnore(const string& filename) {
    string ignorePath;
    if((ignorePath = findIgnore()).empty()) {
        cerr << "[-] Could not find .jvcignore file." << endl;
        return;
    }
    ofstream file(ignorePath + "/.jvcignore",std::ios::app);
    file << filename + "\n";
    file.close();
    cout << "[+] file will now be ignored. - " << filename << endl;
    return;
}

// method to rename the local repository
void Repository::renameRepo(const std::string& name) {
    const string configPath = ".jvc/config.txt";
    vector<string> configContents;
    ifstream infile(configPath,std::ios::in);
    string line;
    while(getline(infile,line)) configContents.push_back(line);
    infile.close();

    configContents[1] = name;

    ofstream out(configPath,std::ios::out);
    for(const auto& line : configContents) {
        out << line + "\n";
    }
    out.close();
    cout << "[+] repo name was changed to - " << name << endl;
}

// Remove one file from stage.txt (rewrite file without that entry).
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
    // rewrite stage.txt, skipping the requested filename
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

// Create branch at current tip; write to branches.txt.
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
    // get the latest commit
    Commit c;
    string commitTip = c.loadLatest();
    Branch newBranch(branch_name,commitTip);
    // add new branch to history
    ofstream hFile(repoPath + "/branches/branches.txt",std::ios::app);
    hFile << branch_name + "\n";
    hFile.close();

    cout << "[+] Branch made successfully. - " + branch_name << endl;
}

// Print commit log from commits.txt (paginated).
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
        // paginate every 6 commits
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

// Restore working tree to commit (or branch tip); resolve branch name to commit id.
void Repository::checkout(const string& commitID) {
    string branchName = commitID;
    string resolvedId = commitID;
    Branch b(branchName);
    // if arg is a branch name, resolve to its tip commit id
    if(b.isABranch()) {
        resolvedId = b.loadRef();
        cout << "[+] Checking out branch - " << branchName << endl;
        b.updateHead(branchName);
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
    // restore each file from this commit's snapshot
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

// Print repo name and UUID from config.
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

// List staged files (name + hash) from stage.txt.
void Repository::stageStatus(const string& stagefile) const {
    if(!filesystem::exists(repoPath + "/" + stagefile)) {
        cerr << "[-] Could not find staging area." << endl;
        return;
    }
    if(filesystem::file_size(repoPath + "/stage.txt") == 0) {
        cout << "[-] no staged files to view." << endl;
        return;
    }
    string filename,hash;
    int stageNum = 0;
    ifstream infile(repoPath + "/" + stagefile,std::ios::in);
    while(infile >> filename >> hash) {
        cout << "[*] Staged File " << to_string(stageNum + 1) << ":" << endl;
        cout << "FileName: " << filename << endl;
        cout << "File Hash: " << hash << endl;
        stageNum++;
    }
    infile.close();
}

// Create commit from staged files; snapshot files, update branch ref and commit log.
void Repository::commit(const std::string& message) {
    Commit c(message,repoPath);
    string parent = c.loadLatest();
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
    // update current branch ref to this commit
    ifstream hIn(repoPath + "/branches/HEAD",std::ios::in);
    hIn >> repo_name;
    hIn.close();
    string headPath = repoPath + "/branches/" + repo_name;
    ofstream stream(headPath,std::ios::out);
    stream << c.getId();
    stream.close();
    // write file list to header; copy each staged file into snapshot dir
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
    // record parent for diff/history
    ofstream parentStream(repoPath + "/snapshots/" + c.getId() + "/parent",std::ios::out);
    parentStream << parent;
    parentStream.close();
    c.setNumFiles(iteration - 1);
    logbook.push_back(c);
    cout << "[+] " + c.getId() + " was committed successfully." << endl;
    ofstream f(repoPath + "/" + "stage.txt", std::ios::trunc);  // clear staging area
    f.close();
    log();
}

// Stage file(s); "." = recursive add (skip .jvc/.git).
void Repository::add(const std::string& filename) {
    bool hasIgnore = false;
    vector<string> toIgnores;
    string path;
    if(!(path = findIgnore()).empty()) {
        toIgnores = loadIgnore(path);
        hasIgnore = true;
    }

    if(filename == ".") {
        for(const auto& entry : filesystem::recursive_directory_iterator(".")) {
            string filepath = entry.path().string();
            // skipping repo directory, real git and .jvcignore files
            bool skipped = false;
            if(hasIgnore) {
                for(const auto& path : toIgnores) {
                    if(filepath.find(path) != std::string::npos) skipped = true;
                }
            }

            if(skipped) continue;

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

    if(hasIgnore) {
        for(const auto& it : toIgnores) {
            if(filename.find(it) != std::string::npos) {
                cout << "[*] file in .jvcignore, skipping... - " << filename << endl;
                return;
            }
        }
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

// Append stagingArea contents to stage.txt (skip already staged).
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
    // append only files not already in stage.txt
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

// Append logbook entries to commits.txt.
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

// True if comID is in .jvc/etc/commit_ids.txt.
bool commitExists(const string& comID) {
    vector<string> commits;
    const string path = ".jvc/etc/commit_ids.txt";
    ifstream infile(path,std::ios::in);
    string name;
    while(getline(infile,name)) commits.push_back(name);
    infile.close();
    return vectorContains(commits,comID);
}

// Diff commit vs its parent: list file-by-file line changes (+/-).
void Repository::diff(const std::string& commitId) {
    if(!commitExists(commitId)) {
        cout << "[-] commit does not exist. - " << commitId << endl;
        return;
    }

    Commit c(commitId);
    c.loadParent();
    string commitId2 = c.getParentId();

    if(commitId2.empty() || commitId == commitId2) {
        cout << "[*] No parent commit (root commit). Nothing to diff against." << endl;
        return;
    }
    // header files list filenames in each commit
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
    // only diff files that exist in both commits
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

            cout << "[*] FILE: " << f_name << endl;
            computeDiff(lines_file2,lines_file1);
            f1.close();
            f2.close();
        }
    }
    cout << "[+] Diff operation complete." << endl;
}

// Line diff: v1 = old file lines, v2 = new; print +/- and count.
void computeDiff(vector<string> v1,vector<string> v2) {
    auto changesMade = 0;
    auto minSize = v1.size() < v2.size() ? v1.size() : v2.size();
    // compare common lines
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

// Truncate stage.txt.
void Repository::clearStagingArea() {
    string path = repoPath + "/stage.txt";
    if(!filesystem::exists(path)) {  // no repo or missing stage file
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

// Write empty commits.txt, config.txt, stage.txt and config content.
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

// Generate UUID_LENGTH-digit id with dashes every 8 chars.
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