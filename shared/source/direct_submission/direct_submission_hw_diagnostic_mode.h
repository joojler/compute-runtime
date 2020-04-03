/*
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "shared/source/utilities/io_functions.h"

#include <chrono>
#include <sstream>
#include <vector>

namespace NEO {
#if defined(_RELEASE_INTERNAL) || (_DEBUG)
constexpr bool directSubmissionDiagnosticAvailable = true;
#else
constexpr bool directSubmissionDiagnosticAvailable = false;
#endif

struct DirectSubmissionSingleDelta {
    int64_t dispatchSubmitTimeDiff = 0;
    int64_t submitWaitTimeDiff = 0;
    int64_t totalTimeDiff = 0;
};

using DirectSubmissionExecution = std::vector<DirectSubmissionSingleDelta>;

class DirectSubmissionDiagnosticsCollector {
  public:
    DirectSubmissionDiagnosticsCollector(uint32_t executions, bool storeExecutions);

    ~DirectSubmissionDiagnosticsCollector() {
        storeData();
        IoFunctions::fclosePtr(logFile);
    }
    void diagnosticModeAllocation() {
        diagnosticModeAllocationTime = std::chrono::high_resolution_clock::now();
    }
    void diagnosticModeDiagnostic() {
        diagnosticModeDiagnosticTime = std::chrono::high_resolution_clock::now();
    }
    void diagnosticModeOneDispatch() {
        diagnosticModeOneDispatchTime = std::chrono::high_resolution_clock::now();
    }
    void diagnosticModeOneSubmit() {
        diagnosticModeOneSubmitTime = std::chrono::high_resolution_clock::now();
    }
    void diagnosticModeOneWait(uint32_t execution,
                               volatile void *waitLocation,
                               uint32_t waitValue) {
        volatile uint32_t *waitAddress = static_cast<volatile uint32_t *>(waitLocation);
        while (waitValue >= *waitAddress)
            ;
        diagnosticModeOneWaitTime = std::chrono::high_resolution_clock::now();

        auto timeStamp = executionList.at(execution);
        auto delta = diagnosticModeOneWaitTime - diagnosticModeOneDispatchTime;
        timeStamp.totalTimeDiff =
            std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count();

        delta = diagnosticModeOneSubmitTime - diagnosticModeOneDispatchTime;
        timeStamp.dispatchSubmitTimeDiff =
            std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count();

        delta = diagnosticModeOneWaitTime - diagnosticModeOneSubmitTime;
        timeStamp.dispatchSubmitTimeDiff =
            std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count();
    }

    uint32_t getExecutionsCount() const {
        return executionsCount;
    }

    void storeData();

  protected:
    std::chrono::high_resolution_clock::time_point diagnosticModeOneDispatchTime;
    std::chrono::high_resolution_clock::time_point diagnosticModeOneSubmitTime;
    std::chrono::high_resolution_clock::time_point diagnosticModeOneWaitTime;
    std::chrono::high_resolution_clock::time_point diagnosticModeAllocationTime;
    std::chrono::high_resolution_clock::time_point diagnosticModeDiagnosticTime;
    DirectSubmissionExecution executionList;

    FILE *logFile = nullptr;

    uint32_t executionsCount = 0;
    bool storeExecutions = true;
};

namespace DirectSubmissionDiagnostics {
inline void diagnosticModeOneDispatch(DirectSubmissionDiagnosticsCollector *collect) {
    if (directSubmissionDiagnosticAvailable) {
        if (collect) {
            collect->diagnosticModeOneDispatch();
        }
    }
}
inline void diagnosticModeOneSubmit(DirectSubmissionDiagnosticsCollector *collect) {
    if (directSubmissionDiagnosticAvailable) {
        if (collect) {
            collect->diagnosticModeOneSubmit();
        }
    }
}
} // namespace DirectSubmissionDiagnostics
} // namespace NEO