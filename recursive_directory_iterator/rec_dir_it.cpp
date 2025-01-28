#include "rec_dir_it.hpp"
#include <dirent.h>
#include <sys/stat.h>

namespace stdlike {

directory_options operator|(directory_options lhs, directory_options rhs) {
    return static_cast<directory_options>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

bool has_option(directory_options options, directory_options flag) {
    return static_cast<int>(options) & static_cast<int>(flag);
}

directory_entry::directory_entry(const std::string &path) : path_(path) {
    if (stat(path.c_str(), &file_stat_) != 0) {
        throw std::runtime_error("Error getting file status for " + path);
    }
}

const std::string &directory_entry::path() const {
    return path_;
}

bool directory_entry::is_directory() const {
    return S_ISDIR(file_stat_.st_mode);
}

bool directory_entry::is_symlink() const {
    struct stat link_stat;
    return lstat(path_.c_str(), &link_stat) == 0 && S_ISLNK(link_stat.st_mode);
}

bool directory_entry::is_regular_file() const {
    return S_ISREG(file_stat_.st_mode);
}

bool directory_entry::is_block_file() const {
    return S_ISBLK(file_stat_.st_mode);
}

bool directory_entry::is_character_file() const {
    return S_ISCHR(file_stat_.st_mode);
}

bool directory_entry::is_socket() const {
    return S_ISSOCK(file_stat_.st_mode);
}

bool directory_entry::is_fifo() const {
    return S_ISFIFO(file_stat_.st_mode);
}

off_t directory_entry::file_size() const {
    return file_stat_.st_size;
}

nlink_t directory_entry::hard_link_count() const {
    return file_stat_.st_nlink;
}

std::time_t directory_entry::last_write_time() const {
    return file_stat_.st_mtime;
}

mode_t directory_entry::permissions() const {
    return file_stat_.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
}

recursive_directory_iterator::recursive_directory_iterator(const char *path,
                                                           directory_options options)
    : options_(options), stack_(std::make_shared<std::stack<DirectoryState>>()) {
    DIR *dir = opendir(path);
    if (!dir) {
        throw std::runtime_error("Cannot open directory: " + std::string(path));
    }
    stack_->push({path, dir, 0});
    advance();
}

recursive_directory_iterator::~recursive_directory_iterator() {
    if (stack_.unique()) {
        while (!stack_->empty()) {
            closedir(stack_->top().dir);
            stack_->pop();
        }
    }
}

void recursive_directory_iterator::advance() {
    while (!stack_->empty()) {
        DirectoryState &state = stack_->top();

        struct dirent *entry = readdir(state.dir);
        if (entry == nullptr) {
            closedir(state.dir);
            stack_->pop();
            continue;
        }

        std::string entry_name(entry->d_name);
        if (entry_name == "." || entry_name == "..") {
            continue;
        }

        std::string entry_path = state.path + "/" + entry_name;

        try {
            current_entry_ = directory_entry(entry_path);

            if (current_entry_.is_directory() &&
                (!current_entry_.is_symlink() ||
                 has_option(options_, directory_options::follow_directory_symlink))) {

                DIR *subdir = opendir(entry_path.c_str());
                if (subdir) {
                    stack_->push({entry_path, subdir, state.depth + 1});
                } else if (!has_option(options_, directory_options::skip_permission_denied)) {
                    throw std::runtime_error("Permission denied for " + entry_path);
                }
            }
            return;
        } catch (const std::exception &) {
            if (!has_option(options_, directory_options::skip_permission_denied)) {
                throw;
            }
        }
    }

    is_end_ = true;
}

recursive_directory_iterator &recursive_directory_iterator::operator++() {
    if (!is_end_) {
        advance();
        is_end_ = stack_->empty();
    }
    return *this;
}

const directory_entry &recursive_directory_iterator::operator*() const {
    if (is_end_) {
        throw std::runtime_error("Dereferencing end iterator");
    }
    return current_entry_;
}

const directory_entry *recursive_directory_iterator::operator->() const {
    if (is_end_) {
        throw std::runtime_error("Dereferencing end iterator");
    }
    return &current_entry_;
}

bool recursive_directory_iterator::stack_top_equal(
    const recursive_directory_iterator &other) const {
    if (stack_->empty() || other.stack_->empty()) {
        return false;
    }

    const DirectoryState &this_top = stack_->top();
    const DirectoryState &other_top = other.stack_->top();
    return this_top.path == other_top.path && this_top.depth == other_top.depth &&
           this_top.dir == other_top.dir;
}

bool recursive_directory_iterator::operator==(const recursive_directory_iterator &other) const {
    if (is_end_ && other.is_end_) {
        return true;
    }
    if (is_end_ != other.is_end_) {
        return false;
    }
    return stack_top_equal(other);
}

bool recursive_directory_iterator::operator!=(const recursive_directory_iterator &other) const {
    return !(*this == other);
}

int recursive_directory_iterator::depth() const {
    return stack_->empty() ? -1 : stack_->top().depth;
}

void recursive_directory_iterator::pop() {
    if (!stack_->empty()) {
        closedir(stack_->top().dir);
        stack_->pop();
    }
    if (stack_->empty()) {
        is_end_ = true;
    }
}

recursive_directory_iterator begin(const recursive_directory_iterator &iter) {
    return iter;
}

recursive_directory_iterator end(const recursive_directory_iterator &) {
    return recursive_directory_iterator{};
}
}  // namespace stdlike
