#include <iostream>
#include <string>

#ifndef __HEADER_H_
#define __HEADER_H_

std::string generate_header(){
    std::string name = std::string("Aeolus\n");
    std::string commit_hash = std::string("Version: ") + std::string(GIT_HASH) + "\n";
    std::string git_branch = std::string("Branch: ") + std::string(GIT_BRANCH) + "\n";
    std::string build_date = std::string("Build date: ") + std::string(COMPILE_TIME) + "\n";
    return name + git_branch + commit_hash + build_date;
}

#endif
