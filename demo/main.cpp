#include <fcntl.h>
#include <iostream>
#include <iterator>
#include <signal.h>
#include <string>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

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
    const char*              file_path = "/usr/bin/python3";
    std::vector<std::string> args;
    args.push_back("python3");
    args.push_back("./hello.py");
    ExecveArray arr(args);

    int                        kq = kqueue();
    std::vector<struct kevent> change_list;
    std::vector<struct kevent> active_list;
    active_list.resize(1024);

    int fds[2] = { -1, -1 };

    std::string buffer;

    signal(SIGCHLD, SIG_IGN);

    pipe(fds);
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        std::cerr << "err" << std::endl;
        exit(1);
    }

    if (pid == 0) {
        close(fds[0]);
        close(STDOUT_FILENO);
        dup2(fds[1], STDOUT_FILENO);
        close(fds[1]);
        if (execve(file_path, arr.get(), NULL) < 0) {
            perror("execve");
            std::cerr << "error" << std::endl;
            exit(1);
        }
    } else {
        close(fds[1]);
        fcntl(fds[0], F_SETFL, O_NONBLOCK);
        struct kevent ev;

        EV_SET(&ev, fds[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        change_list.push_back(ev);

        while (true) {
            int event_size =
                kevent(kq, &*change_list.begin(), (int)change_list.size(),
                       &*active_list.begin(), active_list.size(), NULL);

            change_list.clear();

            ssize_t read_size = 0;
            for (int i = 0; i < event_size; i++) {
                std::cout << "in parent" << std::endl;
                if (active_list[i].filter == EVFILT_READ) {
                    char buf[2];
                    read_size = read(fds[0], buf, 2 - 1);
                    std::cout << "readSIZE: " << read_size << std::endl;
                    if (read_size < 0) {
                        perror("read");
                        std::cerr << "err" << std::endl;
                    } else if (read_size > 0) {
                        buf[read_size] = '\0';
                        buffer.append(buf, read_size);
                    } else {
                        break;
                    }
                }
            }
            if (read_size == 0) {
                break;
            }
        }
    }
    std::cout << "buf: " << buffer << std::endl;
    return 0;
}