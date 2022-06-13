#ifndef EXECVE_ARRAY_HPP
#define EXECVE_ARRAY_HPP

#include <vector>

class ExecveArray {
public:
    ExecveArray() {}
    template <typename T>
    explicit ExecveArray(T const& strs) {
        initExecveArray(strs.begin(), strs.end());
    }

    ExecveArray(const ExecveArray& other) { *this = other; }

    ExecveArray& operator=(const ExecveArray& other) {
        if (this != &other) {
            str_  = other.str_;
            ptrs_ = other.ptrs_;
        }
        return *this;
    }

    char* const* Get() const { return ptrs_.data(); }
    size_t       GetSize() { return str_.size(); }

private:
    template <typename InputIt>
    void initExecveArray(InputIt first, InputIt last) {
        std::vector<size_t> offsets;

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

    std::vector<char>  str_;
    std::vector<char*> ptrs_;
};

#endif
