#pragma once

#include <map>
#include <string>
#include <memory>
#include <functional>

#include "common.hpp"
#include "kvalue.hpp"
#include "value.hpp"
#include "container.hpp"
#include "client.hpp"

constexpr const char *P_RAW_ROOT_PID = "_root_pid";
constexpr const char *P_RAW_ID = "_id";
constexpr const char *P_RAW_LOOP_DEV = "_loop_dev";
constexpr const char *P_RAW_NAME = "_name";
constexpr const char *P_RAW_START_TIME = "_start_time";
constexpr const char *P_RAW_DEATH_TIME = "_death_time";
constexpr const char *P_COMMAND = "command";
constexpr uint64_t COMMAND_SET = (1 << 6);
constexpr const char *P_USER = "user";
constexpr uint64_t USER_SET = (1lu << 7);
constexpr const char *P_GROUP = "group";
constexpr uint64_t GROUP_SET = (1lu << 8);
constexpr const char *P_ENV = "env";
constexpr uint64_t ENV_SET = (1 << 9);
constexpr const char *P_PORTO_NAMESPACE = "porto_namespace";
constexpr const char *P_ROOT = "root";
constexpr uint64_t ROOT_SET = (1 << 11);
constexpr const char *P_ROOT_RDONLY = "root_readonly";
constexpr uint64_t ROOT_RDONLY_SET = (1 << 12);
constexpr const char *P_CWD = "cwd";
constexpr uint64_t CWD_SET = (1 << 13);
constexpr const char *P_STDIN_PATH = "stdin_path";
constexpr uint64_t STDIN_SET = (1 << 14);
constexpr const char *P_STDOUT_PATH = "stdout_path";
constexpr uint64_t STDOUT_SET = (1 << 15);
constexpr const char *P_STDERR_PATH = "stderr_path";
constexpr uint64_t STDERR_SET = (1 << 16);
constexpr const char *P_STDOUT_LIMIT = "stdout_limit";
constexpr const char *P_STDOUT_OFFSET = "stdout_offset";
constexpr const char *P_STDERR_OFFSET = "stderr_offset";
constexpr const char *P_MEM_GUARANTEE = "memory_guarantee";
constexpr uint64_t MEM_GUARANTEE_SET = (1lu << 20);
constexpr const char *P_MEM_LIMIT = "memory_limit";
constexpr const char *P_DIRTY_LIMIT = "dirty_limit";
constexpr const char *P_ANON_LIMIT = "anon_limit";
constexpr const char *P_RECHARGE_ON_PGFAULT = "recharge_on_pgfault";
constexpr const char *P_CPU_POLICY = "cpu_policy";
constexpr const char *P_CPU_GUARANTEE = "cpu_guarantee";
constexpr const char *P_CPU_LIMIT = "cpu_limit";
constexpr const char *P_IO_POLICY = "io_policy";
constexpr const char *P_IO_LIMIT = "io_limit";
constexpr const char *P_IO_OPS_LIMIT = "io_ops_limit";
constexpr const char *P_NET_GUARANTEE = "net_guarantee";
constexpr const char *P_NET_LIMIT = "net_limit";
constexpr const char *P_NET_PRIO = "net_priority";
constexpr const char *P_RESPAWN = "respawn";
constexpr const char *P_MAX_RESPAWNS = "max_respawns";
constexpr const char *P_ISOLATE = "isolate";
constexpr uint64_t ISOLATE_SET = (1lu << 36);
constexpr const char *P_PRIVATE = "private";
constexpr const char *P_ULIMIT = "ulimit";
constexpr const char *P_HOSTNAME = "hostname";
constexpr uint64_t HOSTNAME_SET = (1lu << 39);
constexpr const char *P_BIND_DNS = "bind_dns";
constexpr uint64_t BIND_DNS_SET = (1lu << 40);
constexpr const char *P_BIND = "bind";
constexpr uint64_t BIND_SET = (1lu << 41);
constexpr const char *P_NET = "net";
constexpr uint64_t NET_SET = (1lu << 42);
constexpr const char *P_NET_TOS = "net_tos";
constexpr const char *P_DEVICES = "devices";
constexpr const char *P_CAPABILITIES = "capabilities";
constexpr const char *P_IP = "ip";
constexpr uint64_t IP_SET = (1lu << 46);
constexpr const char *P_DEFAULT_GW = "default_gw";
constexpr const char *P_VIRT_MODE = "virt_mode";
constexpr uint64_t VIRT_MODE_SET = (1lu << 48);
constexpr const char *P_AGING_TIME = "aging_time";
constexpr const char *P_ENABLE_PORTO = "enable_porto";
constexpr const char *P_RESOLV_CONF = "resolv_conf";
constexpr const char *P_WEAK = "weak";
constexpr const char *P_MEM_TOTAL_GUARANTEE = "memory_guarantee_total";

constexpr int VIRT_MODE_APP = 0;
constexpr const char *P_VIRT_MODE_APP = "app";
constexpr int VIRT_MODE_OS = 1;
constexpr const char *P_VIRT_MODE_OS = "os";
constexpr const char *P_CMD_VIRT_MODE_OS = "/sbin/init";

class TBindMap;
class TTaskEnv;

class TPropertyMap : public TValueMap {
    std::weak_ptr<TContainer> Container;

    TError GetSharedContainer(std::shared_ptr<TContainer> &c) const;

public:
    TPropertyMap(std::shared_ptr<TKeyValueNode> kvnode,
                 std::shared_ptr<TContainer> c) :
        TValueMap(kvnode),
        Container(c) {}

    bool ParentDefault(std::shared_ptr<TContainer> &c,
                       const std::string &property) const;

    bool HasFlags(const std::string &property, int flags) const;

    TError PrepareTaskEnv(const std::string &property, TTaskEnv &taskEnv);

    template<typename T>
    const T Get(const std::string &name) const {
        if (IsDefault(name)) {
            std::shared_ptr<TContainer> c;
            if (ParentDefault(c, name))
                if (c && c->GetParent())
                    return c->GetParent()->Prop->Get<T>(name);
        }

        return TValueMap::Get<T>(name);
    }

    template<typename T>
    TError Set(const std::string &name, const T& value) {
        return TValueMap::Set<T>(name, value);
    }

    template<typename T>
    const T GetRaw(const std::string &name) const {
        return TValueMap::Get<T>(name);
    }
};

void RegisterProperties(std::shared_ptr<TRawValueMap> m,
                        std::shared_ptr<TContainer> c);

class TContainerProperty {
public:
    std::string Name;
    uint64_t SetMask;
    std::string Desc;
    bool IsSupported;
    bool IsReadOnly;
    bool IsHidden;
    TError IsAliveAndStopped(void);
    TError IsAlive(void);
    virtual TError Set(const std::string &value) {
        if (IsReadOnly)
            return TError(EError::InvalidValue, "Read-only value: " + Name);

        return TError::Success();
    }
    virtual TError Get(std::string &value) = 0;
    TContainerProperty(std::string name, uint64_t set_mask,
                       std::string desc, bool hidden = false)
                       : Name(name), SetMask(set_mask), Desc(desc),
                       IsSupported(true), IsReadOnly(false), IsHidden(hidden) {}

    TContainerProperty(std::string name, std::string desc, bool hidden = false)
                       : Name(name), SetMask(0), Desc(desc), IsSupported(true),
                       IsReadOnly(true), IsHidden(hidden) {}

    virtual TError GetIndexed(const std::string &index, std::string &value) {
        return TError(EError::InvalidValue, "Invalid subscript for property");
    }
    virtual TError SetIndexed(const std::string &index, const std::string &value) {
        return TError(EError::InvalidValue, "Invalid subscript for property");
    }
};

class TContainerUser : public TContainerProperty {
public:
    TError Set(const std::string &username);
    TError Get(std::string &value);
    TContainerUser(std::string name, uint64_t set_mask, std::string desc)
                   : TContainerProperty(name, set_mask, desc) {}
};


class TContainerGroup : public TContainerProperty {
public:
    TError Set(const std::string &groupname);
    TError Get(std::string &value);
    TContainerGroup(std::string name, uint64_t set_mask, std::string desc)
                    : TContainerProperty(name, set_mask, desc) {}
};

class TContainerMemoryGuarantee : public TContainerProperty {
public:
    TError Set(const std::string &mem_guarantee);
    TError Get(std::string &value);
    TContainerMemoryGuarantee(std::string name, uint64_t set_mask,
                              std::string desc)
                              : TContainerProperty(name, set_mask, desc) {}
    TError Init(void) {
        IsSupported = MemorySubsystem.SupportGuarantee();

        return TError::Success();
    }
};

class TContainerMemTotalGuarantee : public TContainerProperty {
public:
    TError Get(std::string &value);
    TContainerMemTotalGuarantee(std::string name, std::string desc)
                                : TContainerProperty(name, desc) {}
    TError Init(void) {
        IsSupported = MemorySubsystem.SupportGuarantee();

        return TError::Success();
    }
};

class TContainerVirtMode : public TContainerProperty {
public:
    TError Set(const std::string &virt_mode);
    TError Get(std::string &value);
    TContainerVirtMode(std::string name, uint64_t set_mask, std::string desc)
                       : TContainerProperty(name, set_mask, desc) {}
};

class TContainerCommand : public TContainerProperty {
public:
    TError Set(const std::string &command);
    TError Get(std::string &value);
    TContainerCommand(std::string name, uint64_t set_mask, std::string desc)
                      : TContainerProperty(name, set_mask, desc) {}
};

class TContainerCwd : public TContainerProperty {
public:
    TError Set(const std::string &cwd);
    TError Get(std::string &value);
    TContainerCwd(std::string name, uint64_t set_mask, std::string desc)
                  : TContainerProperty(name, set_mask, desc) {}
    void Propagate(const std::string &value);
};

class TContainerStdinPath : public TContainerProperty {
public:
    TError Set(const std::string &path);
    TError Get(std::string &value);
    TContainerStdinPath(std::string name, uint64_t set_mask, std::string desc)
                    : TContainerProperty(name, set_mask, desc) {}
};

class TContainerStdoutPath : public TContainerProperty {
public:
    TError Set(const std::string &path);
    TError Get(std::string &value);
    TContainerStdoutPath(std::string name, uint64_t set_mask, std::string desc)
                     : TContainerProperty(name, set_mask, desc) {}
};

class TContainerStderrPath : public TContainerProperty {
public:
    TError Set(const std::string &path);
    TError Get(std::string &value);
    TContainerStderrPath(std::string name, uint64_t set_mask, std::string desc)
                     : TContainerProperty(name, set_mask, desc) {}
};

class TContainerBindDns : public TContainerProperty {
public:
    TError Set(const std::string &bind_needed);
    TError Get(std::string &value);
    TContainerBindDns(std::string name, uint64_t set_mask, std::string desc)
                    : TContainerProperty(name, set_mask, desc) {}
};

class TContainerIsolate : public TContainerProperty {
public:
    TError Set(const std::string &isolate_needed);
    TError Get(std::string &value);
    TContainerIsolate(std::string name, uint64_t set_mask, std::string desc)
                      : TContainerProperty(name, set_mask, desc) {}
};

class TContainerRoot : public TContainerProperty {
public:
    TError Set(const std::string &root);
    TError Get(std::string &value);
    TContainerRoot(std::string name, uint64_t set_mask, std::string desc)
                   : TContainerProperty(name, set_mask, desc) {}
};

class TContainerNet : public TContainerProperty {
public:
    TError Set(const std::string &net_desc);
    TError Get(std::string &value);
    TContainerNet(std::string name, uint64_t set_mask, std::string desc)
                  : TContainerProperty(name, set_mask, desc) {}
};

class TContainerHostname : public TContainerProperty {
public:
    TError Set(const std::string &hostname);
    TError Get(std::string &value);
    TContainerHostname(std::string name, uint64_t set_mask, std::string desc)
                       : TContainerProperty(name, set_mask, desc) {}
};

class TContainerRootRo : public TContainerProperty {
public:
    TError Set(const std::string &ro);
    TError Get(std::string &value);
    TContainerRootRo(std::string name, uint64_t set_mask, std::string desc)
                     : TContainerProperty(name, set_mask, desc) {}
};

class TContainerEnv : public TContainerProperty {
public:
    TError Set(const std::string &env);
    TError Get(std::string &value);
    TError GetIndexed(const std::string &index, std::string &value);
    TError SetIndexed(const std::string &index, const std::string &env_val);
    TContainerEnv(std::string name, uint64_t set_mask, std::string desc)
                  : TContainerProperty(name, set_mask, desc) {}
};

class TContainerBind : public TContainerProperty {
public:
    TError Set(const std::string &bind_str);
    TError Get(std::string &value);
    TContainerBind(std::string name, uint64_t set_mask, std::string desc)
                   : TContainerProperty(name, set_mask, desc) {}
};

class TContainerIp : public TContainerProperty {
public:
    TError Set(const std::string &ipaddr);
    TError Get(std::string &value);
    TContainerIp(std::string name, uint64_t set_mask, std::string desc)
                 : TContainerProperty(name, set_mask, desc) {}
};

void InitContainerProperties(void);
