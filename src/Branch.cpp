#include "Branch.hpp"

Branch::Branch(const std::string& branchName, const std::string& tipCommitId)
    : name(branchName), branchId(tipCommitId) { updateHead(tipCommitId); }

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
    ofstream file(".mygit/branches/" + name,std::ios::out);
    file << tip_commit;
    file.close();
}

string Branch::loadRef() const {
    string head = "N/A";
    ifstream file(".mygit/branches/" + name,std::ios::in);
    file >> head;
    file.close();
    return head;
}
