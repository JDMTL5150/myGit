#ifndef BRANCH_HPP_
#define BRANCH_HPP_

#include <string>
#include <fstream>

class Branch {
public:
    Branch(const std::string& branchName, const std::string& tipCommitId);

    std::string getName() const;
    std::string getTipCommitId() const;

    void setTipCommitId(const std::string& id);
    void setName(const std::string& branchName);

    void updateHead(const std::string& tip_commit);
    std::string loadRef() const;
private:
    std::string name;
    std::string branchId;  // commit ID this branch points to (tip)
};

#endif