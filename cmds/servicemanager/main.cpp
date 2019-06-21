/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <android-base/logging.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/Status.h>
#include <utils/StrongPointer.h>

#include "Access.h"
#include "ServiceManager.h"

using ::android::sp;
using ::android::ProcessState;
using ::android::IPCThreadState;
using ::android::ServiceManager;
using ::android::Access;

int main(int argc, char** argv) {
    if (argc > 2) {
        LOG(FATAL) << "usage: " << argv[0] << " [binder driver]";
    }

    const char* driver = argc == 2 ? argv[1] : "/dev/binder";

    android::base::InitLogging(nullptr, &android::base::KernelLogger);

    ProcessState::self()->initWithDriver(driver);
    ProcessState::self()->setThreadPoolMaxThreadCount(0);
    ProcessState::self()->setCallRestriction(
        ProcessState::CallRestriction::FATAL_IF_NOT_ONEWAY);

    sp<ServiceManager> manager = new ServiceManager(std::make_unique<Access>());
    IPCThreadState::self()->setTheContextObject(manager);
    ProcessState::self()->becomeContextManager(nullptr, nullptr);

    IPCThreadState::self()->joinThreadPool();

    // should not be reached
    return EXIT_FAILURE;
}
