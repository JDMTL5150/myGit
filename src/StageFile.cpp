// class StageFile {
//     private:
//         const Commit::Commit& commitData;
//         const std::string content;
//         const std::string metadata;
//     public:
//         StageFile(const std::string& filename);
//         void setCommit(Commit::Commit& c);
//         void setMetadata(const std::string md);
//     };

#include "StageFile.hpp"

#include "Commit.hpp"

#include <functional>
#include <string>

using namespace std;

StageFile::StageFile(const std::string& file)
    : filename(file) {
    ifstream in(file, ios::in);
    if (!in) {
        cerr << "[-] Could not open file for staging: " << file << endl;
        return;
    }

    string line;
    while (getline(in, line)) {
        content += line;
        content.push_back('\n');
    }
    in.close();

    hash<string> hasher;
    metadata = to_string(hasher(content));
}

std::string StageFile::getContent() const {
    return content;
}

void StageFile::writeToFile(ofstream& file) const {
    if (!file) {
        cerr << "[-] No file found for writing stage entry." << endl;
        return;
    }
    file << filename << " " << metadata << '\n';
}