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

#include "fchmod.h"
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <tuple>
#include <unistd.h>

#include "../../common/napi/n_async/n_async_work_callback.h"
#include "../../common/napi/n_async/n_async_work_promise.h"
#include "../../common/napi/n_func_arg.h"
namespace OHOS {
namespace DistributedFS {
namespace ModuleFileIO {
using namespace std;

napi_value Fchmod::Sync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    bool succ = false;
    int fd;
    tie(succ, fd) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid fd");
        return nullptr;
    }

    int mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid mode");
        return nullptr;
    }

    int ret = fchmod(fd, mode);
    if (ret == -1) {
        UniError(errno).ThrowErr(env);
        return nullptr;
    }

    return NVal::CreateUndefined(env).val_;
}


napi_value Fchmod::Async(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::THREE)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    size_t argc = funcArg.GetArgc();
    bool succ = false;
    int fd;
    tie(succ, fd) = NVal(env, funcArg[NARG_POS::FIRST]).ToInt32();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid fd");
        return nullptr;
    }

    int mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid owner");
    }

    auto cbExec = [fd, mode](napi_env env) -> UniError {
        int ret = fchmod(fd, mode);
        if (ret == -1) {
            return UniError(errno);
        } else {
            return UniError(ERRNO_NOERR);
        }
    };

    auto cbComplCallback = [](napi_env env, UniError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        return { NVal::CreateUndefined(env) };
    };

    string procedureName = "FileIOFchmod";
    NVal thisVar(env, funcArg.GetThisVar());
    if (argc == NARG_CNT::TWO) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplCallback).val_;
    } else {
        NVal cb(env, funcArg[NARG_POS::THIRD]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplCallback).val_;
    }
}
} // namespace ModuleFileIO
} // namespace DistributedFS
} // namespace OHOS