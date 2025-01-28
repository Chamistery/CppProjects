#pragma once

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stack>
#include <filesystem>
#include <ctime>
#include <set>

namespace stdlike {

namespace fs = std::filesystem;

enum class directory_options {
    none = 0,
    follow_directory_symlink = 1 << 0,
    skip_permission_denied = 1 << 1
};

directory_options operator|(directory_options lhs, directory_options rhs);
bool has_option(directory_options options, directory_options flag);

class directory_entry {
public:
    directory_entry() = default;
    explicit directory_entry(const std::string &path);
    const std::string &path() const;
    bool is_directory() const;
    bool is_symlink() const;
    bool is_regular_file() const;
    bool is_block_file() const;
    bool is_character_file() const;
    bool is_socket() const;
    bool is_fifo() const;
    off_t file_size() const;
    nlink_t hard_link_count() const;
    std::time_t last_write_time() const;
    mode_t permissions() const;

private:
    std::string path_;
    struct stat file_stat_;
};

class recursive_directory_iterator {
public:
    explicit recursive_directory_iterator(const char *path,
                                          directory_options options = directory_options::none);
    recursive_directory_iterator() : options_{}, stack_{}, current_entry_{}, is_end_(true) {
    }
    recursive_directory_iterator(const fs::path &path,
                                 directory_options options = directory_options::none)
        : recursive_directory_iterator(path.c_str(), options) {
    }
    ~recursive_directory_iterator();

    recursive_directory_iterator &operator++();
    const directory_entry &operator*() const;
    const directory_entry *operator->() const;

    bool operator==(const recursive_directory_iterator &other) const;
    bool operator!=(const recursive_directory_iterator &other) const;

    int depth() const;
    void pop();

private:
    struct DirectoryState {
        std::string path;
        DIR *dir;
        int depth;
    };

    directory_options options_;
    std::shared_ptr<std::stack<DirectoryState>> stack_ =
        std::make_shared<std::stack<DirectoryState>>();
    directory_entry current_entry_;
    bool is_end_ = false;

    bool stack_top_equal(const recursive_directory_iterator &other) const;
    void advance();
};

recursive_directory_iterator begin(const recursive_directory_iterator &iter);
recursive_directory_iterator end(const recursive_directory_iterator &iter);
}  // namespace stdlike
