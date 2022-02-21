/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "posix_fallocate.h"
#include <cstring>
#include <fcntl.h>
#include <tuple>
#include <unistd.h>

#include "../../common/napi/n_async/n_async_work_callback.h"
#include "../../common/napi/n_async/n_async_work_promise.h"
#include "../../common/napi/n_func_arg.h"

namespace OHOS {
namespace DistributedFS {
namespace ModuleFileIO {
using namespace std;

static tuple<bool, int, int, int> GetPosixFallocateArg(napi_env env, const NFuncArg &funcArg)
{
    bool succ = false;
    int fd;
    tie(succ, fd) = NVal(env, funcArg[static_cast<size_t>(NARG_POS::FIRST)]).ToInt32();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid fd");
        return { false, -1, -1, -1 };
    }
    int offset;
    tie(succ, offset) = NVal(env, funcArg[static_cast<size_t>(NARG_POS::SECOND)]).ToInt32();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid offset");
        return { false, -1, -1, -1 };
    }
    int len;
    tie(succ, len) = NVal(env, funcArg[static_cast<size_t>(NARG_POS::THIRD)]).ToInt32();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid len");
        return { false, -1, -1, -1 };
    }
    return { succ, fd, offset, len };
}

napi_value PosixFallocate::Sync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(static_cast<size_t>(NARG_CNT::THREE))) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    int fd;
    int offset;
    int len;
    tie(succ, fd, offset, len) = GetPosixFallocateArg(env, funcArg);
    if (!succ) {
        return nullptr;
    }
    int ret = posix_fallocate(fd, offset, len);
    if (ret == -1) {
        UniError(errno).ThrowErr(env);
        return nullptr;
    }
    return NVal::CreateUndefined(env).val_;
}

napi_value PosixFallocate::Async(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(static_cast<size_t>(NARG_CNT::THREE), static_cast<size_t>(NARG_CNT::FOUR))) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }
    bool succ = false;
    int fd;
    int offset;
    int len;
    tie(succ, fd, offset, len) = GetPosixFallocateArg(env, funcArg);
    if (!succ) {
        return nullptr;
    }
    auto cbExec = [fd, offset, len](napi_env env) -> UniError {
        if (posix_fallocate(fd, offset, len) == -1) {
            return UniError(errno);
        } else {
            return UniError(ERRNO_NOERR);
        }
    };
    auto cbCompl = [](napi_env env, UniError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        return { NVal::CreateUndefined(env) };
    };
    NVal thisVar(env, funcArg.GetThisVar());
    string procedureName = "fileioPosixFallocate";
    int argc = funcArg.GetArgc();
    if (argc == static_cast<int>(NARG_CNT::THREE)) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
    } else {
        NVal cb(env, funcArg[static_cast<size_t>(NARG_POS::FOURTH)]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbCompl).val_;
    }
    return NVal::CreateUndefined(env).val_;
}
} // namespace ModuleFileIO
} // namespace DistributedFS
} // namespace OHOS