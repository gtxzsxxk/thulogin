#include <iostream>
#include <cstring>
#include <signal.h>
#include <thread>
#include <chrono>
#include <unistd.h>
#include "Authenticator.h"
#include "PwdInput.h"
#include "Config.h"
#include "Logger.h"
#include "KeepAlive.h"

const std::string DEFAULT_HOST = "auth4.tsinghua.edu.cn";
const std::string DEFAULT_HOST_V6 = "auth6.tsinghua.edu.cn";
const std::string DEFAULT_ONLINE_TARGET = "https://www.baidu.com/";
const std::string DEFAULT_CAMPUS_TARGET = "https://www.tsinghua.edu.cn/";
const int DEFAULT_INTERVAL = 180;
const int DEFAULT_RETRY = 2;
const std::string USER_AGENT = "Mozilla/5.0 (Windows NT 10.0; Win64; x64)";

static KeepAlive* g_keepAlive = nullptr;

void signalHandler(int sig) {
    (void)sig;
    if (g_keepAlive) {
        g_keepAlive->stop();
    }
    exit(0);
}

struct CliParams {
    std::string username;
    std::string password;
    std::string configPath;
    std::string host;
    std::string acId;
    std::string caFile;
    bool daemon = false;
    bool keepOnline = false;
    bool ipv6 = false;
    bool campusOnly = false;
    bool noCheck = false;
    bool help = false;
};

static void printUsage(const char* program) {
    std::cout << "Usage: " << program << " [options] [auth|deauth]\n"
              << "\n"
              << "Subcommands:\n"
              << "  auth       Authenticate and login (default)\n"
              << "  deauth     Logout\n"
              << "\n"
              << "Options:\n"
              << "  -u, --username <user>   Username\n"
              << "  -p, --password <pass>   Password (warning: visible in process list)\n"
              << "  -c, --config <path>     Config file path (default: ~/.auth-thu)\n"
              << "  -D, --daemon            Daemon mode (auto keep-online)\n"
              << "  -k, --keep-online       Keep online after login\n"
              << "  -6, --ipv6              Use auth6.tsinghua.edu.cn\n"
              << "      --campus-only       Use campus target for keep-alive\n"
              << "      --ac-id <id>        Specify ac_id manually\n"
              << "      --ca-file <path>    CA certificate file path\n"
              << "      --no-check          Skip online check\n"
              << "  -h, --help              Show this help message\n";
}

static CliParams parseArgs(int argc, char** argv) {
    CliParams params;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-u" || arg == "--username") {
            if (i + 1 < argc) {
                params.username = argv[++i];
            }
        } else if (arg == "-p" || arg == "--password") {
            if (i + 1 < argc) {
                params.password = argv[++i];
            }
        } else if (arg == "-c" || arg == "--config") {
            if (i + 1 < argc) {
                params.configPath = argv[++i];
            }
        } else if (arg == "-D" || arg == "--daemon") {
            params.daemon = true;
        } else if (arg == "-k" || arg == "--keep-online") {
            params.keepOnline = true;
        } else if (arg == "-6" || arg == "--ipv6") {
            params.ipv6 = true;
        } else if (arg == "--campus-only") {
            params.campusOnly = true;
        } else if (arg == "--ac-id") {
            if (i + 1 < argc) {
                params.acId = argv[++i];
            }
        } else if (arg == "--ca-file") {
            if (i + 1 < argc) {
                params.caFile = argv[++i];
            }
        } else if (arg == "--no-check") {
            params.noCheck = true;
        } else if (arg == "-h" || arg == "--help") {
            params.help = true;
        } else if (arg[0] != '-') {
            // Positional subcommand
            // Only accept if it is the last positional arg (we just overwrite)
            // But we don't validate here; main() will use the last one.
            // We store it and let main decide.
            // Actually, to match spec: last positional arg is subcommand.
            // We'll just track it.
        }
    }
    return params;
}

static std::string parseSubcommand(int argc, char** argv) {
    std::string subcommand = "auth";
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-u" || arg == "--username" ||
            arg == "-p" || arg == "--password" ||
            arg == "-c" || arg == "--config" ||
            arg == "--ac-id" || arg == "--ca-file") {
            ++i; // skip the value for this option
        } else if (arg[0] != '-') {
            subcommand = arg;
        }
    }
    return subcommand;
}

static void startKeepAlive(const CliParams& params) {
    std::string target = params.campusOnly ? DEFAULT_CAMPUS_TARGET : DEFAULT_ONLINE_TARGET;
    LOG_INFO("Starting keep-alive, target: " + target);

    KeepAlive ka(target, DEFAULT_INTERVAL, DEFAULT_RETRY);
    g_keepAlive = &ka;
    ka.start();

    while (ka.isRunning()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    g_keepAlive = nullptr;
}

static int doAuth(const CliParams& params) {
    std::string host = params.host.empty() ? (params.ipv6 ? DEFAULT_HOST_V6 : DEFAULT_HOST) : params.host;
    std::string baseUrl = "https://" + host;

    std::string acId = params.acId;
    if (acId.empty()) {
        Authenticator au(baseUrl, "1", USER_AGENT);
        acId = au.detect_ac_id(params.ipv6);
    }

    Authenticator au(baseUrl, acId, USER_AGENT);
    if (!params.caFile.empty()) {
        // TODO: au.setCaFile(params.caFile); // Not yet implemented in Authenticator
        (void)0;
    }

    if (!params.noCheck) {
        std::string onlineUser;
        if (au.is_online(onlineUser)) {
            LOG_INFO("Already online as: " + onlineUser);
            if (params.keepOnline || params.daemon) {
                startKeepAlive(params);
            }
            return 0;
        }
    }

    if (au.auth(params.username, params.password) == 0) {
        if (params.keepOnline || params.daemon) {
            startKeepAlive(params);
        }
        return 0;
    }
    return 1;
}

static int doDeauth(const CliParams& params) {
    std::string host = params.host.empty() ? (params.ipv6 ? DEFAULT_HOST_V6 : DEFAULT_HOST) : params.host;
    std::string baseUrl = "https://" + host;

    std::string acId = params.acId;
    if (acId.empty()) {
        Authenticator au(baseUrl, "1", USER_AGENT);
        acId = au.detect_ac_id(params.ipv6);
    }

    Authenticator au(baseUrl, acId, USER_AGENT);

    std::string username = params.username;
    if (username.empty()) {
        std::string onlineUser;
        if (au.is_online(onlineUser)) {
            username = onlineUser;
        }
    }

    if (username.empty()) {
        LOG_ERROR("Username required for logout");
        return 1;
    }

    au.setUsername(username);
    return au.logout();
}

int main(int argc, char** argv) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    CliParams params = parseArgs(argc, argv);

    if (params.help) {
        printUsage(argv[0]);
        return 0;
    }

    std::string configPath = params.configPath.empty() ? Config::defaultPath() : params.configPath;
    ConfigData config = Config::load(configPath);

    bool passwordFromCli = false;
    if (params.username.empty()) {
        params.username = config.username;
    }
    if (params.password.empty()) {
        params.password = config.password;
    } else {
        passwordFromCli = true;
    }

    std::string subcommand = parseSubcommand(argc, argv);

    if (subcommand == "deauth") {
        // deauth: password not needed; username can be auto-detected via is_online
        if (!params.daemon && params.username.empty()) {
            std::cout << "Username (leave blank to auto-detect): ";
            std::string input;
            std::cin >> input;
            if (!input.empty()) {
                params.username = input;
            }
        }
        return doDeauth(params);
    }

    // auth: require both username and password
    if (!params.daemon) {
        if (params.username.empty()) {
            std::cout << "Username: ";
            std::cin >> params.username;
        }
        if (params.password.empty()) {
            params.password = pwd_input("Password: ");
        }
    }

    if (params.username.empty() || params.password.empty()) {
        LOG_ERROR("Username and password are required");
        return 1;
    }

    if (passwordFromCli) {
        LOG_INFO("Warning: password provided via command line is visible in process list");
    }

    if (params.daemon) {
        setLogLevel(LogLevel::ERROR);
    }

    return doAuth(params);
}
