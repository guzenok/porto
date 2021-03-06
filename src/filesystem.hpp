#pragma once

#include <string>
#include <vector>

#include "util/error.hpp"
#include "util/path.hpp"
#include "util/cred.hpp"

struct TBindMount {
    TPath Source;
    TPath Target;
    bool ReadOnly = false;
    bool ControlSource = false;
    bool ControlTarget = false;
};

struct TMountNamespace {
public:
    std::string Container; /* for logging and errors */
    TCred BindCred;
    TPath Cwd;
    TPath Root; /* path in ParentNs.Mnt */
    bool RootRo;
    std::vector<TBindMount> BindMounts;
    bool BindPortoSock;
    uint64_t RunSize;
    std::string Systemd;

    TError SetupRoot();
    TError MountRun();
    TError MountBinds();
    TError ProtectProc();
    TError MountTraceFs();
    TError MountSystemd();

    TError Setup();
};

bool IsSystemPath(const TPath &path);
