
#ifndef STAGEFILE_HPP_
#define STAGEFILE_HPP_

#include <fstream>
#include <iostream>
#include <string>

class Commit;

class StageFile {
private:
    bool isFail;
    std::string filename;
    std::string content;
    std::string metadata;
public:
    explicit StageFile(const std::string& filename);

    std::string getContent() const;
    std::string getFileName() const;
    bool failed();
    void writeToFile(std::ofstream& file) const;
};

#endif
