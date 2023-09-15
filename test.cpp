#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>

void executePythonScript(const std::string& scriptPath) {
    const char *script = scriptPath.c_str();
    const char *pythonexec = "python3";
    char *python = (char*)pythonexec;
    char* cmd = (char*)script;
    char* argv[] = {python, cmd, nullptr};
	std::vector<const char*> envp;
	envp.push_back("QUERY_STRING=name=hi");
	std::vector<char*> casted;
	casted.push_back(const_cast<char*>(envp[0]));

    int pipefd[2];
    pid_t pid;
    int status;

    if (pipe(pipefd) == -1) {
        std::cerr << "Error: pipe creation failed" << std::endl;
        return;
    }

    pid = fork();
    if (pid == -1) {
        std::cerr << "Error: fork failed" << std::endl;
        return;
    }

    if (pid == 0) {
        // Child process
        close(pipefd[0]);
        if (execve("/usr/bin/python3", argv, casted.data()) == -1) {
            std::cerr << "Error: execve failed" << std::endl;
            std::cerr << scriptPath << std::endl;
        }
        _exit(1); // Exit the child process
    } else {
        // Parent process
        close(pipefd[1]);
        waitpid(pid, &status, 0);
    }
}

int main() {
    std::string _cgiPath = "/Users/dnguyen/WEBSERVE/root/cgi-bin/simple.py";

    executePythonScript(_cgiPath);

    return 0;
}
