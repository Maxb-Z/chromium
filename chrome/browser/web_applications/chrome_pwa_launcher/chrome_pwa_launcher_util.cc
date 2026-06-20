// Copyright 2020 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/web_applications/chrome_pwa_launcher/chrome_pwa_launcher_util.h"

#include "base/files/file_util.h"
#include "base/path_service.h"
#include "chrome/common/buildflags.h"
#include "chrome/common/chrome_constants.h"
#include "custom_browser/common/product_version.h"

namespace {

constexpr base::FilePath::StringViewType kChromePwaLauncherExecutable =
    L"chrome_pwa_launcher.exe";

}  // namespace

namespace web_app {

base::FilePath GetChromePwaLauncherPath() {
  base::FilePath chrome_dir;
  if (!base::PathService::Get(base::DIR_EXE, &chrome_dir))
    return base::FilePath();
#if BUILDFLAG(ENABLE_CUSTOM_BROWSER)
  base::FilePath launcher_path =
      chrome_dir.AppendASCII(custom_browser::kCustomBrowserProductVersion)
          .Append(kChromePwaLauncherExecutable);
  if (base::PathExists(launcher_path))
    return launcher_path;
  launcher_path = chrome_dir.AppendASCII(chrome::kChromeVersion)
                      .Append(kChromePwaLauncherExecutable);
#else
  base::FilePath launcher_path = chrome_dir.AppendASCII(chrome::kChromeVersion)
                                     .Append(kChromePwaLauncherExecutable);
#endif
  if (base::PathExists(launcher_path))
    return launcher_path;
  // In dev builds, the launcher will be in the executable directory.
  return chrome_dir.Append(kChromePwaLauncherExecutable);
}

}  // namespace web_app
