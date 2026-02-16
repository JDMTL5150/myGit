#ifndef BRANCH_HPP_
#define BRANCH_HPP_

#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>

#include "Repository.hpp"

class Branch {
public:
    Branch(const std::string& branchName, const std::string& tipCommitId);
    Branch(const std::string& branchName);

    std::string getName() const;
    std::string getTipCommitId() const;

    void setTipCommitId(const std::string& id);
    void setName(const std::string& branchName);

    std::vector<std::string> loadAllBranches();
    void remove();

    void updateRef(const std::string& tip_commit);
    void updateHead(const std::string& newHead);
    bool isABranch();
    std::string loadRef() const;
private:
    std::string name;
    std::string branchId;  // commit ID this branch points to (tip)
};

#endif
