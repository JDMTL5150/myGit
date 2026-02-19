#pragma once

#ifndef MERGE_HPP_
#define MERGE_HPP_

class Merge {
public:
    Merge(Repository& repo);

    // Returns the merge base commit
    Commit findMergeBase(const Branch& ours, const Branch& theirs);

    // Performs a 3-way merge and returns the merge commit hash
    string performMerge(const Branch& ours, const Branch& theirs, const string& message);

private:
    Repository& repo;

    // Helpers
    std::vector<std::string> walkParents(const string& commitHash);
    std::string mergeFileContents(const string& base, const string& ours, const string& theirs);
};

#endif