
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Commit.hpp"

using namespace std;

Commit::Commit(const string& msg):
    id(checkoutHash()),message(msg),timestamp(makeTimestamp()) {}


string Commit::getMessage() const {
    return message;
}

string Commit::getId() const {
    return id;
}

string Commit::getTimestamp() const {
    return timestamp;
}

string makeTimestamp() {
    std::time_t now = std::time(nullptr);
    string timestamp = std::ctime(&now);
    timestamp.pop_back();
    return timestamp;
}

string checkoutHash() {
    string checkout = "";
    unsigned char c = 0;
    srand(time(0));
    int max = 126;
    int min = 32;
    int count = 0;
    while(count < 8){
        c = rand() % (max - min + 1) + min;
        if(!isalnum(c)) continue;
        checkout += c;
        count++;
    }
    return checkout;
}