#include "Branch.hpp"

using namespace std;

Branch::Branch(const std::string& branchName, const std::string& tipCommitId)
    : name(branchName), branchId(tipCommitId) { updateHead(tipCommitId); }

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

void Branch::updateHead(const std::string& tip_commit) {
    ofstream file("jvc/branches/" + name,std::ios::out);
    file << tip_commit;
    file.close();
}

bool Branch::isABranch() {
    vector<string> branches;
    ifstream file("jvc/branches/branches.txt",std::ios::in);
    string line;
    while(getline(file,line)) branches.push_back(line);
    return vectorContains(branches,name);
}

string Branch::loadRef() const {
    string head = "N/A";
    ifstream file("jvc/branches/" + name,std::ios::in);
    if(!file) {
        cerr << "[-] Branch ref could not be found." << endl;
        return nullptr;
    }
    file >> head;
    file.close();
    return head;
}
