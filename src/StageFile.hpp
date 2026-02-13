
#ifndef STAGEFILE_HPP_
#define STAGEFILE_HPP_

#include <fstream>
#include <iostream>
#include <string>

class Commit;

class StageFile {
private:
    std::string filename;
    std::string content;
    std::string metadata;
public:
    explicit StageFile(const std::string& filename);

    std::string getContent() const;
    void writeToFile(std::ofstream& file) const;
};

#endif