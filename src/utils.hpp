#pragma once
#ifndef SIPSERVER_UTILS_HPP
#define SIPSERVER_UTILS_HPP

#include <algorithm>
#include <sstream>
#include <iostream>

#include <easylogging++.h>

el::Level getLogLevel(std::string logLevel) {
    /*
     *     Trace
     *     Debug
     *     Fatal
     *     Error
     *     Warning
     *     Info
     *     Verbose
     */
    std::transform(logLevel.begin(), logLevel.end(),logLevel.begin(), ::toupper);

    if ("DEBUG" == logLevel) {
        return el::Level::Debug;
    }
    else if ("FATAL" == logLevel) {
        return el::Level::Fatal;
    }
    else if ("ERROR" == logLevel) {
        return el::Level::Error;
    }
    else if ("WARNING" == logLevel) {
        return el::Level::Warning;
    }
    else if ("INFO" == logLevel) {
        return el::Level::Info;
    }
    else {
        return el::Level::Unknown;
    }
}

void setLogLevel(el::Level level, el::Configurations& conf) {
    //emulation of logging hierarchy because implementation in easylogging++ is too crutched

    /* Logging hierarchy:
     *      el::Level::Debug
     *      el::Level::Info
     *      el::Level::Warning
     *      el::Level::Error
     */

    std::vector<el::Level> usedLevels = {el::Level::Debug, el::Level::Info, el::Level::Warning, el::Level::Error};

    for(auto l: usedLevels) {
        if (l != level) {
            conf.set(l, el::ConfigurationType::Enabled, "false");
        }
        else {
            break;
        }
    }
}

void configureLogger(bool isConsoleOut, std::string loggingFile, el::Level logLevel) {
    el::Configurations conf;
    conf.set(el::Level::Global, el::ConfigurationType::Format, "%level %datetime{%H:%m:%s} [%fbase]: %msg");
    conf.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, isConsoleOut? "true": "false");
    if ("" != loggingFile) {
        conf.set(el::Level::Global, el::ConfigurationType::Filename, loggingFile);
    }
    setLogLevel(logLevel, conf);
    el::Loggers::reconfigureAllLoggers(conf);

    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);

    LOG(INFO) << "Logger of SipServer is started";
    LOG(INFO) << "Log is wrote to " << loggingFile;
    LOG(INFO) << "Log level is " << (uint)logLevel;
    LOG(INFO) << "Echo to stdout is " << (isConsoleOut? "" : "not ") << "specified";
}

#endif //SIPSERVER_UTILS_HPP
