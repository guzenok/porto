#pragma once

#include <string>
#include <functional>

#include "util/error.hpp"
#include "string.hpp"
#include "util/cred.hpp"

class TPath {
    std::string Path;

    std::string DirNameStr() const;
    TPath AddComponent(const TPath &component) const;

public:
    TPath(const std::string &path) : Path(path) {}
    TPath(const char *path) : Path(path) {}
    TPath() : Path("") {}

    bool IsAbsolute() const { return Path[0] == '/'; }

    bool IsRoot() const { return Path == "/"; }

    bool IsEmpty() const { return Path.empty(); }

    bool IsNormal() const { return Path == NormalPath().Path; }

    const char *c_str() const noexcept { return Path.c_str(); }

    TPath operator+(const TPath &p) const {
        return TPath(Path + p.ToString());
    }

    friend bool operator==(const TPath& a, const TPath& b) {
        return a.ToString() == b.ToString();
    }

    friend bool operator!=(const TPath& a, const TPath& b) {
        return a.ToString() != b.ToString();
    }

    friend bool operator<(const TPath& a, const TPath& b) {
        return a.ToString() < b.ToString();
    }

    friend bool operator>(const TPath& a, const TPath& b) {
        return a.ToString() > b.ToString();
    }

    friend std::ostream& operator<<(std::ostream& os, const TPath& path) {
        return os << path.ToString();
    }

    friend TPath operator/(const TPath& a, const TPath &b) {
        return a.AddComponent(b);
    }

    TPath& operator/=(const TPath &b) {
        *this = AddComponent(b);
        return *this;
    }

    TPath DirName() const;
    std::string BaseName() const;
    TPath NormalPath() const;
    TPath AbsolutePath() const;
    TPath RealPath() const;
    TPath InnerPath(const TPath &path, bool absolute = true) const;

    TError StatStrict(struct stat &st) const;
    TError StatFollow(struct stat &st) const;

    bool IsRegularStrict() const;
    bool IsRegularFollow() const;

    bool IsDirectoryStrict() const;
    bool IsDirectoryFollow() const;

    unsigned int GetDev() const;
    unsigned int GetBlockDev() const;

    int64_t SinceModificationMs() const;
    std::string ToString() const;
    bool Exists() const;

    bool HasAccess(const TCred &cred, int mask) const;
    bool CanRead(const TCred &cred) const { return HasAccess(cred, 4); }
    bool CanWrite(const TCred &cred) const { return HasAccess(cred, 2); }

    TError Chdir() const;
    TError Chroot() const;
    TError PivotRoot() const;

    TError Chown(uid_t uid, gid_t gid) const;

    TError Chown(const TCred &cred) const {
        return Chown(cred.Uid, cred.Gid);
    }

    TError Chmod(const int mode) const;
    TError ReadLink(TPath &value) const;
    TError Symlink(const TPath &target) const;
    TError Mkfifo(unsigned int mode) const;
    TError Mknod(unsigned int mode, unsigned int dev) const;
    TError Mkdir(unsigned int mode) const;
    TError MkdirAll(unsigned int mode) const;
    TError CreateAll(unsigned int mode) const;
    TError Rmdir() const;
    TError Unlink() const;
    TError RemoveAll() const;
    TError Rename(const TPath &dest) const;
    TError ReadDirectory(std::vector<std::string> &result) const;
    TError ListSubdirs(std::vector<std::string> &result) const;
    TError ClearDirectory() const;
    TError StatVFS(uint64_t &space_used, uint64_t &space_avail,
                   uint64_t &inode_used, uint64_t &inode_avail) const;
    TError StatVFS(uint64_t &space_avail) const;
    TError SetXAttr(const std::string name, const std::string value) const;
    TError RotateLog(off_t max_disk_usage, off_t &loss) const;
    TError Chattr(unsigned add_flags, unsigned del_flags) const;


    static const TFlagsNames MountFlags;
    static const TFlagsNames UmountFlags;
    static std::string MountFlagsToString(unsigned long flags);
    static std::string UmountFlagsToString(unsigned long flags);

    TError Mount(TPath source, std::string type, unsigned long flags,
                 std::vector<std::string> options) const;
    TError Bind(TPath source, unsigned long flags) const;
    TError Remount(unsigned long flags) const;
    TError Umount(unsigned long flags) const;
    TError UmountAll() const;

    TError ReadAll(std::string &text, size_t max = 1048576) const;
    TError WriteAll(const std::string &text) const;

    TError ReadLines(std::vector<std::string> &lines, size_t max = 1048576) const;
};
