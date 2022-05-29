#include <iterator>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include <iostream>

class ExecveArray {
public:
    ExecveArray() {}

    // Constructs an ExecveArray by copying strings from the given range. T must
    // be a range over ranges of char.
    template <typename T>
    explicit ExecveArray(T const& strs)
        : ExecveArray(strs.begin(), strs.end()) {}

    // Constructs an ExecveArray by copying strings from [first, last). InputIt
    // must be an input iterator over a range over char.
    template <typename InputIt>
    ExecveArray(InputIt first, InputIt last) {
        std::vector<size_t>                           offsets;
        std::back_insert_iterator<std::vector<char> > output_it =
            std::back_inserter(str_);
        for (InputIt it = first; it != last; ++it) {
            offsets.push_back(str_.size());
            std::copy((*it).begin(), (*it).end(), output_it);
            str_.push_back('\0');
        }
        ptrs_.reserve(offsets.size() + 1);
        for (std::vector<size_t>::iterator it = offsets.begin();
             it != offsets.end(); it++) {
            ptrs_.push_back(str_.data() + *it);
        }
        ptrs_.push_back(NULL);
    }

    char* const* get() const { return ptrs_.data(); }
    size_t       get_size() { return str_.size(); }

private:
    std::vector<char>  str_;
    std::vector<char*> ptrs_;
};

int main(void) {
    const char* file_path =
        "/Users/nakahodojunya/.anyenv/envs/pyenv/shims/python";
    std::vector<std::string> args;
    args.push_back("python");
    args.push_back("./hello.py");
    ExecveArray arr(args);

    pid_t pid;
    pid = fork();
    if (pid < 0) {
        std::cerr << "err" << std::endl;
        exit(1);
    }

    if (pid == 0) {
        execvp("python", arr.get());
    } else {
        int status;

        wait(&status);
        std::cout << "Done" << std::endl;
    }
    return 0;
}