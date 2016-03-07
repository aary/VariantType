#include <iostream>
using std::cout;
using std::endl;
#include <unistd.h>
#include <thread>
#include <chrono>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <cassert>
#include <vector>
#include <algorithm>

int main() {

    for (int i = 1; i < 3; ++i) {
        cout << "About to fork()" << endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        auto child_pid = fork();
        if (!child_pid) {
            cout << "In child process with process id " << getpid() << endl;
            cout << "Child's parent process is " << getppid() << endl;

            // construct a vector and fill it with each value in the range
            // [0,255]
            std::vector<char> vec;
            vec.resize(100000 * i);
            std::generate(vec.begin(), vec.end(), [] () {
                static char x {static_cast<char>(0)};
                return ++x;
            });

            // add elements together and print sum to prevent optimizer from
            // screwing this up
            int sum = 0;
            for (auto ele : vec) { sum += ele; }
            cout << "The total sum is " << sum << endl;

            return 0;

        } else {

            // this will wait for the child above to finish, type 
            // `man waitpid` in a bash terminal to learn more
            waitpid(child_pid, nullptr, 0);

            // get resource usage statistics, and assert that the system call
            // worked fine
            struct rusage usage;
            int return_val_getrusage = getrusage(RUSAGE_CHILDREN, &usage);
            assert(!return_val_getrusage);

            cout << "Memory used by child " << usage.ru_maxrss << " bytes" << endl;
        }
    }

    return 0;
}
