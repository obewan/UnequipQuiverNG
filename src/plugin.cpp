#include "Plugin.h"
#include "EquipEventHandler.h"
#include <spdlog/sinks/basic_file_sink.h>

// Template source: https://github.com/SkyrimDev/HelloWorld-using-CommonLibSSE-NG
// See also: https://github.com/CharmedBaryon/CommonLibSSE-NG/wiki

void SetupLog() {
    auto logsFolder = SKSE::log::log_directory();
    if (!logsFolder) {
        SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
        return;
    }

    auto logFilePath = *logsFolder / std::format("{}.log", Plugin::NAME);
    auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
    auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
    spdlog::set_default_logger(std::move(loggerPtr));
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::info);
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SetupLog();

    SKSE::log::info(("{} v{}"), Plugin::NAME, Plugin::VERSION);
    SKSE::log::info("Game version : {}", skse->RuntimeVersion().string());

    SKSE::Init(skse);

    RegisterEquipEventHandler();

    return true;
}
