/*
 *
 *    Copyright (c) 2023 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#include "AppTask.h"

#include <app/server/Server.h>
#include <app/util/attribute-storage.h>
#ifdef CONFIG_CHIP_WIFI
#include <platform/nrfconnect/wifi/WiFiManager.h>
#endif

class AppFabricTableDelegate : public chip::FabricTable::Delegate
{
public:
    ~AppFabricTableDelegate() { chip::Server::GetInstance().GetFabricTable().RemoveFabricDelegate(this); }

    /**
     * @brief Initialize module and add a delegation to the Fabric Table.
     *
     * To use the OnFabricRemoved method defined within this class and allow to react on the last fabric removal
     * this method should be called in the application code.
     */
    static void Init()
    {
#ifndef CONFIG_CHIP_LAST_FABRIC_REMOVED_NONE
        static AppFabricTableDelegate sAppFabricDelegate;
        chip::Server::GetInstance().GetFabricTable().AddFabricDelegate(&sAppFabricDelegate);
#endif // CONFIG_CHIP_LAST_FABRIC_REMOVED_NONE
    }

private:
    void OnFabricRemoved(const chip::FabricTable & fabricTable, chip::FabricIndex fabricIndex)
    {
#ifndef CONFIG_CHIP_LAST_FABRIC_REMOVED_NONE
        if (chip::Server::GetInstance().GetFabricTable().FabricCount() == 0)
        {
#ifdef CONFIG_CHIP_LAST_FABRIC_REMOVED_ERASE_AND_REBOOT
            chip::Server::GetInstance().ScheduleFactoryReset();
#elif defined(CONFIG_CHIP_LAST_FABRIC_REMOVED_ERASE_ONLY) || defined(CONFIG_CHIP_LAST_FABRIC_REMOVED_ERASE_AND_PAIRING_START)
            chip::DeviceLayer::PlatformMgr().ScheduleWork([](intptr_t) {
                /* Erase Matter data */
                chip::DeviceLayer::PersistedStorage::KeyValueStoreMgrImpl().DoFactoryReset();
                /* Erase Network credentials and disconnect */
                chip::DeviceLayer::ConnectivityMgr().ErasePersistentInfo();
#ifdef CONFIG_CHIP_WIFI
                chip::DeviceLayer::WiFiManager::Instance().Disconnect();
                chip::DeviceLayer::ConnectivityMgr().ClearWiFiStationProvision();
#endif
#ifdef CONFIG_CHIP_LAST_FABRIC_REMOVED_ERASE_AND_PAIRING_START
                /* Start the New BLE advertising */
                AppEvent event;
                event.Handler = AppTask::StartBLEAdvertisementHandler;
                AppTask::Instance().PostEvent(event);
#endif // CONFIG_CHIP_LAST_FABRIC_REMOVED_ERASE_AND_PAIRING_START
            });
#endif // CONFIG_CHIP_LAST_FABRIC_REMOVED_ERASE_AND_REBOOT
        }
#endif // CONFIG_CHIP_LAST_FABRIC_REMOVED_NONE
    }
};
