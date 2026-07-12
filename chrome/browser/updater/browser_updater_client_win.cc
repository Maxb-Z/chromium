// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/updater/browser_updater_client.h"

#include <string>

#include "base/files/file_path.h"
#include "base/strings/string_util.h"
#include "base/strings/string_util_win.h"
#include "base/strings/sys_string_conversions.h"
#include "base/version.h"
#include "chrome/browser/google/google_brand.h"
#include "chrome/common/buildflags.h"
#include "chrome/install_static/install_details.h"
#include "chrome/updater/registration_data.h"
#include "components/version_info/version_info.h"

#if BUILDFLAG(ENABLE_CUSTOM_BROWSER)
#include "custom_browser/common/product_version.h"
#endif

namespace updater {

std::string BrowserUpdaterClient::GetAppId() {
  return base::SysWideToUTF8(
      base::ToLowerASCII(install_static::InstallDetails::Get().app_guid()));
}

base::FilePath BrowserUpdaterClient::GetExpectedEcp() {
  return {};
}

RegistrationRequest BrowserUpdaterClient::GetRegistrationRequest() {
  RegistrationRequest req;
  req.app_id = GetAppId();
  google_brand::GetBrand(&req.brand_code);
#if BUILDFLAG(ENABLE_CUSTOM_BROWSER)
  // Register with the product version — the value setup.exe writes to
  // Clients\pv and the version the update server targets — not the Chromium
  // engine version, which would desync the updater's persisted registration
  // from the installed product.
  req.version = custom_browser::kCustomBrowserProductVersion;
#else
  req.version = version_info::GetVersionNumber();
#endif
  req.ap =
      base::SysWideToUTF8(install_static::InstallDetails::Get().update_ap());
  return req;
}

bool BrowserUpdaterClient::AppMatches(const UpdateService::AppState& app) {
  return base::EqualsCaseInsensitiveASCII(app.app_id, GetAppId());
}

}  // namespace updater
