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

import {AsyncCallback, Callback} from "./basic";

/**
 * Provides Environment APIs.
 *
 * @since 8
 * @sysCap N/A
 * @devices phone, tablet
 */
declare namespace Statfs {
  /**
   * Get the user data path.
   *
   * @since 8
   */
  function getStorageData(callback: AsyncCallback<string>): void;
  function getStorageData(): Promise<string>;
  /**
   * Get the User storage path.
   *
   * @since 8
   */
  function getUserData(callback: AsyncCallback<string>): void;
  function getUserData(): Promise<string>;
}

export default Statfs;
