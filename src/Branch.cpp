#include "Branch.hpp"

using namespace std;

Branch::Branch(const std::string& branchName, const std::string& tipCommitId)
    : name(branchName), branchId(tipCommitId) { updateRef(tipCommitId); }

Branch::Branch(const std::string& branchName) :
    name(branchName) {}

std::string Branch::getName() const {
    return name;
}

std::string Branch::getTipCommitId() const {
    return branchId;
}

void Branch::setTipCommitId(const std::string& id) {
    branchId = id;
}

void Branch::setName(const std::string& branchName) {
    name = branchName;
}

vector<string> Branch::loadAllBranches() {
    vector<string> branches;
    string path = ".jvc/branches/branches.txt";
    ifstream infile(path,std::ios::in);
    string line;
    while(getline(infile, line)) branches.push_back(line);
    infile.close();
    return branches; 
}

void Branch::remove() {
    vector<string> branches = loadAllBranches();
    string path = ".jvc/branches/branches.txt";
    ofstream file(path,std::ios::trunc);
    if(!file) {
        cerr << "[-] could not find branches." << endl;
        return;
    }
    for(const auto& b : branches) {
        if(b == name) 
        { filesystem::remove(".jvc/branches/" + b); continue; }
        file << b + "\n";
    }
    cout << "[+] Branch removed successfully. - " << name << endl;
    cout << "[*] removed at: " << makeTimestamp() << endl;
}

void Branch::updateHead(const string& newHead) {
    string path = ".jvc/branches/HEAD";
    ofstream file(path,std::ios::out);
    file << newHead;
    file.close();
}

void Branch::updateRef(const std::string& tip_commit) {
    const string path = ".jvc/branches/" + name;
    ofstream file(path, std::ios::out);
    if (!file) {
        cerr << "[-] Could not create branch ref file: " << path << endl;
        return;
    }
    file << tip_commit;
    file.close();
}

bool Branch::isABranch() {
    vector<string> branches;
    ifstream file(".jvc/branches/branches.txt",std::ios::in);
    string line;
    while(getline(file,line)) branches.push_back(line);
    return vectorContains(branches,name);
}

string Branch::loadRef() const {
    string head = "N/A";
    ifstream file(".jvc/branches/" + name,std::ios::in);
    if(!file) {
        cerr << "[-] Branch ref could not be found." << endl;
        return "";
    }
    file >> head;
    file.close();
    return head;
}
