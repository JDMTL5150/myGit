
#include <filesystem>
#include <iostream>
#include <strings.h>

#include "Repository.hpp"

// check if compiler is windows - cross platform
#ifdef _MSC_VER
#define STRCASECMP _stricmp
#else
#define STRCASECMP strcasecmp
#endif

using namespace std;

void printInfo(void) {
    cout << endl;
    cout << "#### Jacob's mini git (JVC) ####\n" << endl;
    cout << "Commands:" << endl;
    cout << "init   - Initialize a repository in current directory." << endl;
    cout << "add    - Stage a file or files to the repository." << endl;
    cout << "commit - Commit the current stage area to the repo." << endl;
    cout << "status - Show currently staged files." << endl;
    cout << "delete - delete the local repository." << endl;
    cout << "clear  - Clears the staging area." << endl;
    cout << "diff   - shows the modifications between commit and parent." << endl;
    cout << "repo   - prints the data of the local repo." << endl;
    cout << "log    - Show commit log history." << endl;
    cout << endl;
    cout << "Branch Commands:" << endl;
    cout << "branch                  - print all branches." << endl;
    cout << "branch <branch_name>    - create a branch." << endl;
    cout << "branch -r <branch_name> - remove a branch." << endl;
    cout << endl;
    cout << "Flags:" << endl;
    cout << "Ex: jvc <command> -<flag> <input>" << endl;
    cout << "(add)  : i - add a file to the jvcignore file." << endl;
    cout << "(repo) : r - rename the local repository." << endl;
    cout << endl;
}

int main(int argc, char* argv[]) {
    Repository repo;
    if (argc < 2) {
        cout << "USAGE: jvc <command> - use help for info" << endl;
        return 1;
    }

    const string command = argv[1];

    if (command != "init" && !filesystem::exists(".jvc")) {
        cout << "[-] Not a jvc repository in current directory." << endl;
        return 1;
    }

    if (command == "init") {
        if (argc != 3) {
            cout << "USAGE: jvc init <repo_name>" << endl;
            return 1;
        }

        Repository repo(argv[2]);
        repo.init();
    } else if (command == "add") {
        if (argc < 3) {
            cout << "USAGE: jvc add <filename>" << endl;
            return 1;
        }

        if(argc >= 3 && strcmp(argv[2], "-i") == 0) {
            if(argc != 4) {
                cout << "USAGE: jvc add -i <filename>" << endl;
                return 1;
            }
            const string file = argv[3];
            repo.addIgnore(file);
            return 0;
        }

        for(auto i = 2; i < argc; i++){
            repo.add(argv[i]);
        }
        repo.loadStagingArea();
    }
    else if(command == "commit"){
        if (argc != 3) {
            cout << "USAGE: jvc commit <message>" << endl;
            return 1;
        }

        if (!filesystem::exists(".jvc")) {
            cout << "[-] Not a jvc repository in current directory." << endl;
            return 1;
        }
        repo.commit(argv[2]);
    }
    else if(command == "status") {
        if (!filesystem::exists(".jvc")) {
            cout << "[-] Not a jvc repository in current directory." << endl;
            return 1;
        }
        repo.stageStatus("stage.txt");
    }
    else if(command == "delete") {
        if(argc != 3 || STRCASECMP(argv[2],"y") != 0){
            cout << "USAGE: jvc delete <y> (to confirm)" << endl;
            return 1;
        }

        if(filesystem::exists(".jvc")){
            filesystem::remove_all(".jvc");
            string commitspath = ".jvc/etc/commit_ids.txt";
            ofstream f(commitspath,std::ios::trunc);
            cout << "[+] Local repository deleted." << endl;
            f.close();
        }
        else {
            cout << "[-] No repository to remove." << endl;
        }
    }
    else if(command == "repo") {
        if(argc > 2 && strcmp(argv[2],"-r") == 0) {
            if(argc != 4) {
                cout << "USAGE: jvc repo -r <new_name>" << endl;
                return 1;
            }
            repo.renameRepo(argv[3]);
        }
        else repo.printRepoData();
    }
    else if(command == "log") {
        repo.viewLogs();
    }
    else if(command == "branch") {
        if(argc == 2) {
            repo.printBranches();
        }
        else if(argc >= 4) {
            if(strcmp(argv[2],"-r") == 0) {
                const char* branchName = argv[argc - 1];
                if(branchName && *branchName) {
                    Branch b(branchName);
                    if(!b.isABranch()) {
                        cerr << "[-] Could not find branch. - " << branchName << endl;
                        return 1;
                    }
                    b.remove();
                }
            }
            else {
                cout << "[-] Command flag not found. - " << (argv[2] + 1) << endl;
                return 1;
            }
        }
        else if(argc == 3) {
            repo.makeBranch(argv[argc - 1]);
        }
        else {
            cout << "Branch Commands:" << endl;
            cout << "branch               - print all branches." << endl;
            cout << "branch <branch_name> - create a branch." << endl;
        }
    }
    else if(command == "help") {
        printInfo();
    }
    else if(command == "diff") {
        if(argc != 3) {
            cout << "USAGE: jvc diff <commitID1>" << endl;
            return 1;
        }
        repo.diff(argv[2]);
    }
    else if(command == "checkout") {
        if(argc != 3){
            cout << "USAGE: jvc checkout <commitID>" << endl;
            return 1;
        }
        repo.checkout(argv[2]);
    }
    else if(command == "clear"){
        if(argc >= 3 && strcmp(argv[2],"-f") == 0) {
            if(argc < 4) {
                cout << "USAGE: jvc clear -f <filename>" << endl;
                return 1;
            }
            string filename = argv[3];
            repo.removeStageFile(filename);
        }
        else repo.clearStagingArea();
    }
    else {
        cout << "[-] command not found. - " << command << endl;
    }

    return 0;
}
