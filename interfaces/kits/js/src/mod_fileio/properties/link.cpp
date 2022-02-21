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
#include "link.h"

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

static tuple<bool, string, string> GetLinkArg(napi_env env, const NFuncArg &funcArg)
{
    bool succ = false;
    unique_ptr<char[]> src;
    tie(succ, src, ignore) = NVal(env, funcArg[static_cast<size_t>(NARG_POS::FIRST)]).ToUTF8String();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid src");
        return { false, "", "" };
    }

    unique_ptr<char[]> dest;
    tie(succ, dest, ignore) = NVal(env, funcArg[static_cast<size_t>(NARG_POS::SECOND)]).ToUTF8String();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid dest");
        return { false, "", "" };
    }
    return { true, src.get(), dest.get() };
}

napi_value Link::Sync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(static_cast<size_t>(NARG_CNT::TWO))) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    bool succ = false;
    string oldPath;
    string newPath;
    tie(succ, oldPath, newPath) = GetLinkArg(env, funcArg);
    if (!succ) {
        return nullptr;
    }
    if (link(oldPath.c_str(), newPath.c_str()) == -1) {
        UniError(errno).ThrowErr(env);
        return nullptr;
    }
    return NVal::CreateUndefined(env).val_;
}

napi_value Link::Async(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);

    if (!funcArg.InitArgs(static_cast<size_t>(NARG_CNT::TWO), static_cast<size_t>(NARG_CNT::THREE))) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    bool succ = false;
    string oldPath;
    string newPath;
    tie(succ, oldPath, newPath) = GetLinkArg(env, funcArg);
    if (!succ) {
        return nullptr;
    }

    auto cbExec = [oldPath = move(oldPath), newPath = move(newPath)](napi_env env) -> UniError {
        int ret = link(oldPath.c_str(), newPath.c_str());
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

    string procedureName = "FileIOLink";
    NVal thisVar(env, funcArg.GetThisVar());
    int argc = funcArg.GetArgc();
    if (argc == static_cast<int>(NARG_CNT::TWO)) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplCallback).val_;
    } else {
        NVal cb(env, funcArg[static_cast<size_t>(NARG_POS::THIRD)]);
        return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplCallback).val_;
    }
}
} // namespace ModuleFileIO
} // namespace DistributedFS
} // namespace OHOS