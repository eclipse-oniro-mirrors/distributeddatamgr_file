/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef SERVICES_STORAGE_SERVICE_TASK_STUB_H
#define SERVICES_STORAGE_SERVICE_TASK_STUB_H

#include "i_storage_service_task.h"
#include "nocopyable.h"

namespace OHOS {
class StorageServiceTaskStub : public IRemoteStub<IStorageServiceTask> {
public:
    StorageServiceTaskStub() = default;
    virtual ~StorageServiceTaskStub() = default;
    virtual int32_t
    OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    DISALLOW_COPY_AND_MOVE(StorageServiceTaskStub);

private:
};
} // namespace OHOS

#endif // SERVICES_STORAGE_SERVICE_TASK_STUB_H