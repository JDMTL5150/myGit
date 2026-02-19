// class Merge {
//     public:
//         Merge(Repository& repo);
    
//         // Returns the merge base commit
//         Commit findMergeBase(const Branch& ours, const Branch& theirs);
    
//         // Performs a 3-way merge and returns the merge commit hash
//         string performMerge(const Branch& ours, const Branch& theirs, const string& message);
    
//     private:
//         Repository& repo;
    
//         // Helpers
//         std::vector<std::string> walkParents(const string& commitHash);
//         std::string mergeFileContents(const string& base, const string& ours, const string& theirs);
//     };

#include <cstdlib>
#include <iostream>

#include "Commit.hpp"
#include "Branch.hpp"
#include "Merge.hpp"
#include "Repository.hpp"

using namespace std;

Merge::Merge(Repository& repo) :
    repo(repo) {}

Commit Merge::findMergeBase(const Branch& ours, const Branch& theirs) {
    vector<string> ours_parents = walkParents(ours);
    vector<string> theirs_parents = walkParents(theirs);

    if(ours_parents.size() == 0 || theirs_parents.size() == 0) return Commit("-1");

    unordered_set<string> visited(ours_parents.begin(),ours_parents.end());
    
    string common_ancestor;

    for(const elem& : theirs_parents) {
        if(visited.count(elem)) {
            common_ancestor = elem;
            break;
        }
    }
    Commit c(common_ancestor);
    return c;
}

string performMerge(const Branch& ours, const Branch& theirs, const string& message) {

}

vector<string> walkParents(const string& commitHash) {

}

string mergeFileContents(const string& base, const string& ours, const string& theirs) {

}

