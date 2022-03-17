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

#include "chmod.h"
#include <cstring>
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

napi_value Chmod::Sync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    bool succ = false;
    unique_ptr<char[]> path;
    tie(succ, path, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid path");
        return nullptr;
    }

    int mode;
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid mode");
        return nullptr;
    }

    if (chmod(path.get(), mode) == -1) {
        UniError(errno).ThrowErr(env);
        return nullptr;
        }
    return NVal::CreateUndefined(env).val_;
}

napi_value Chmod::Async(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(NARG_CNT::TWO, NARG_CNT::THREE)) {
        UniError(EINVAL).ThrowErr(env, "Number of argments unmatched");
        return nullptr;
    }

    string path;
    unique_ptr<char[]> tmp;
    bool succ = false;
    tie(succ, tmp, ignore) = NVal(env, funcArg[NARG_POS::FIRST]).ToUTF8String();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid path");
        return nullptr;
    }

    path = tmp.get();
    int mode;
    size_t argc = funcArg.GetArgc();
    tie(succ, mode) = NVal(env, funcArg[NARG_POS::SECOND]).ToInt32();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid mode");
    }

    auto cbExec = [path, mode](napi_env env) -> UniError {
        if (chmod(path.c_str(), mode) == -1) {
            return UniError(errno);
        } else {
            return UniError(ERRNO_NOERR);
        }
    };
    auto cbComplete = [](napi_env env, UniError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        return { NVal::CreateUndefined(env) };
    };
    NVal thisVar(env, funcArg.GetThisVar());

    string procedureName = "FileIOChmod";
    if (argc == NARG_CNT::TWO) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    } else {
        NVal cb(env, funcArg[NARG_POS::THIRD]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
    }
    return nullptr;
}
} // namespace ModuleFileIO
} // namespace DistributedFS
} // namespace OHOS