/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef STORAGE_SERVICE_SSCRYPTO_CRYPTO_PATH_H
#define STORAGE_SERVICE_SSCRYPTO_CRYPTO_PATH_H
#include <cctype>
#include <cstdio>
#include <iostream>
#include <string>
#include <unistd.h>
typedef unsigned mode_t;
namespace OHOS {
struct CryptoPath {
    std::string path;
    uid_t uid;
    gid_t gid;
    mode_t mode;
    bool isEncrypt;
    int32_t Create();
    int32_t Delete();
    bool IsExist();
};
} // namespace OHOS
#endif // STORAGE_SERVICE_SSCRYPTO_CRYPTO_PATH_H