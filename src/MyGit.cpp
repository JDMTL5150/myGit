
#include <filesystem>
#include <iostream>
#include <strings.h>

#include "Repository.hpp"

using namespace std;

void printInfo(void) {
    cout << "#### Jacob's mini git ####" << endl;
    cout << "init   - Initialize a repository in current directory." << endl;
    cout << "add    - Stage a file or files to the repository." << endl;
    cout << "commit - Commit the current stage area to the repo." << endl;
    cout << "status - Show currently staged files." << endl;
    cout << "delete - delete the local repository." << endl;
    cout << "log    - Show log history." << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "USAGE: myGit <command> - use help for info" << endl;
        return 1;
    }

    const string command = argv[1];

    if (command == "init") {
        if (argc != 3) {
            cout << "USAGE: myGit init <repo_name>" << endl;
            return 1;
        }

        Repository repo(argv[2]);
        repo.init();
    } else if (command == "add") {
        if (argc < 3) {
            cout << "USAGE: myGit add <filename>" << endl;
            return 1;
        }

        if (!filesystem::exists(".mygit")) {
            cout << "[-] Not a myGit repository in current directory." << endl;
            return 1;
        }

        Repository repo;
        for(auto i = 2; i < argc; i++){
            repo.add(argv[i]);
        }
        repo.loadStagingArea();
    }
    else if(command == "commit"){
        if (argc != 3) {
            cout << "USAGE: myGit commit <message>" << endl;
            return 1;
        }

        if (!filesystem::exists(".mygit")) {
            cout << "[-] Not a myGit repository in current directory." << endl;
            return 1;
        }
        Repository repo;
        repo.commit(argv[2]);
    }
    else if(command == "status") {
        if (!filesystem::exists(".mygit")) {
            cout << "[-] Not a myGit repository in current directory." << endl;
            return 1;
        }
        Repository repo;
        repo.stageStatus("stage.txt");
    }
    else if(command == "delete") {
        if(argc != 3 || strcasecmp(argv[2],"y") != 0){
            cout << "USAGE: myGit delete <y> (to confirm)" << endl;
            return 1;
        }

        if(filesystem::exists(".mygit")){
            filesystem::remove_all(".mygit");
            cout << "[+] Local repository deleted." << endl;
        }
        else {
            cout << "[-] No repository to remove." << endl;
        }
    }
    else if(command == "log") {
        Repository repo;
        repo.viewLogs();
    }
    else if(command == "help") {
        printInfo();
    }

    return 0;
}