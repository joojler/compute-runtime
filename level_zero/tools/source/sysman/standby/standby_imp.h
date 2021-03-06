/*
 * Copyright (C) 2019-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <level_zero/zet_api.h>

#include "os_standby.h"
#include "standby.h"

namespace L0 {

class StandbyImp : public Standby {
  public:
    ze_result_t standbyGetProperties(zet_standby_properties_t *pProperties) override;
    ze_result_t standbyGetMode(zet_standby_promo_mode_t *pMode) override;
    ze_result_t standbySetMode(const zet_standby_promo_mode_t mode) override;

    StandbyImp() = default;
    StandbyImp(OsSysman *pOsSysman);
    ~StandbyImp() override;
    OsStandby *pOsStandby = nullptr;

    void init();

    // Don't allow copies of the StandbyImp object
    StandbyImp(const StandbyImp &obj) = delete;
    StandbyImp &operator=(const StandbyImp &obj) = delete;

  private:
    zet_standby_properties_t standbyProperties = {};
};

} // namespace L0
