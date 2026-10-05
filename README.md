<div align="center">

# Vector-SR Framework

**A Vector fork focused on soft-restart reliability under KernelSU jailbreak mode**

[![Build](https://img.shields.io/github/actions/workflow/status/byemaxx/Vector-SR/core.yml?branch=master&event=push&logo=github&label=Build)](https://github.com/byemaxx/Vector-SR/actions/workflows/core.yml?query=event%3Apush+branch%3Amaster+is%3Acompleted)
[![Crowdin](https://img.shields.io/badge/Localization-Crowdin-blueviolet?logo=Crowdin)](https://crowdin.com/project/lsposed_jingmatrix)
[![Download](https://img.shields.io/github/v/release/byemaxx/Vector-SR?color=orange&logoColor=orange&label=Download&logo=DocuSign)](https://github.com/byemaxx/Vector-SR/releases/latest)
[![Total](https://shields.io/github/downloads/byemaxx/Vector-SR/total?logo=Bookmeter&label=Counts&logoColor=yellow&color=yellow)](https://github.com/byemaxx/Vector-SR/releases)

</div>

---

### Introduction

Vector-SR is a fork of [Vector](https://github.com/JingMatrix/Vector), a Zygisk module providing an ART hooking framework that maintains API consistency with the original Xposed. It is engineered on top of [LSPlant](https://github.com/JingMatrix/LSPlant) to deliver a stable, native-level instrumentation environment.

This fork is maintained at [byemaxx/Vector-SR](https://github.com/byemaxx/Vector-SR). Its primary focus is improving soft-restart recovery under KernelSU jailbreak mode, including system_server reinjection reliability, stale daemon handling, and dex2oat wrapper robustness after repeated soft restarts.

The framework allows modules to modify system and application behavior in-memory. Because no APK files are modified, changes are non-destructive, easily reversible via reboot, and compatible across various ROMs and Android versions.

---

### Compatibility

Vector-SR supports devices running **Android 8.1 through Android 17 Beta**, following the compatibility baseline of upstream Vector.

> [!TIP]
> This framework requires a recent installation of Magisk or KernelSU with Zygisk enabled. The fork is mainly tested and maintained around KernelSU jailbreak mode soft-restart scenarios.

---

### Installation

1. Download the latest release as a system module.
2. Install the module via your root manager (Magisk/KernelSU).
3. Ensure a Zygisk environment (e.g., [NeoZygisk](https://github.com/JingMatrix/NeoZygisk)).
4. Reboot the device.
5. Access management settings via the system notification.

---

### Downloads

| Channel | Source |
| :--- | :--- |
| **Stable Releases** | [GitHub Releases](https://github.com/byemaxx/Vector-SR/releases) |
| **Canary (CI) Builds** | [GitHub Actions](https://github.com/byemaxx/Vector-SR/actions/workflows/core.yml?query=branch%3Amaster) |

> [!NOTE]
> Debug builds are recommended for users encountering issues or performing troubleshooting.
> CI builds may be useful when testing soft-restart, reinjection, or dex2oat wrapper changes.

> [!CAUTION]
> GitHub requires users to be **logged in** to download CI artifacts.
>
> The link above is filtered to show only `master` branch builds.
> Please note that builds from Pull Requests (PRs) are often unstable and potentially unsafe (depending on the authors); we recommend staying on the `master` branch for verified builds, unless you are asked to help debugging sessions.

---

### Support and Contribution

If you encounter issues or wish to help improve this fork, please use the resources below.

*   **Project:** [byemaxx/Vector-SR](https://github.com/byemaxx/Vector-SR)
*   **Issues:** [GitHub Issues](https://github.com/byemaxx/Vector-SR/issues)
*   **Upstream:** [JingMatrix/Vector](https://github.com/JingMatrix/Vector)
*   **Localization:** Upstream localization is handled via [Crowdin](https://crowdin.com/project/lsposed_jingmatrix).

> [!IMPORTANT]
> Bug reports are most useful when they include logs from the latest debug or CI build, especially for soft-restart, system_server reinjection, stale daemon, or dex2oat wrapper problems.
>
> *Notice for Chinese speakers:*
>
> 为了提高沟通效率，建议优先使用英文提交 Issue。可以使用 [DeepL](https://www.deepl.com/zh/translator) 或其他翻译工具辅助整理日志和问题描述。

---

### Developer Resources

Vector-SR supports both legacy and modern hooking standards to ensure broad module compatibility.

*   [Legacy Xposed API](https://api.xposed.info/)
*   [Modern libxposed API](https://github.com/JingMatrix/libxposed-api)
*   [Xposed Module Repository](https://github.com/Xposed-Modules-Repo)

> [!NOTE]
> Vector-SR supports the `libxposed` API via two git submodules: the [module API](./xposed/) and the [service API](./services/).
>
> A successful GitHub Actions build of the [master](https://github.com/byemaxx/Vector-SR/tree/master) branch indicates that this fork builds successfully against the current submodule commits.
> Developers are suggested to check the exact commits used by this fork when debugging module compatibility.

---

### Credits

This project is based on [Vector](https://github.com/JingMatrix/Vector) and is made possible by the following open-source contributions:

*   [Magisk](https://github.com/topjohnwu/Magisk/): The foundation of Android customization.
*   [LSPlant](https://github.com/JingMatrix/LSPlant): The core ART hooking engine.
*   [XposedBridge](https://github.com/rovo89/XposedBridge): The standard Xposed APIs.
*   [Dobby](https://github.com/JingMatrix/Dobby): Inline hooking implementation.
*   [LSPosed](https://github.com/LSPosed/LSPosed): Upstream source.
*   [Vector](https://github.com/JingMatrix/Vector): Upstream project for this fork.
*   [xz-embedded](https://github.com/tukaani-project/xz-embedded): Library decompression utilities.

<details>
<summary>Legacy and Historical Dependencies</summary>

- ~~[Riru](https://github.com/RikkaApps/Riru)~~
- ~~[SandHook](https://github.com/ganyao114/SandHook/)~~
- ~~[YAHFA](https://github.com/rk700/YAHFA)~~
- ~~[dexmaker](https://github.com/linkedin/dexmaker)~~
- ~~[DexBuilder](https://github.com/LSPosed/DexBuilder)~~
</details>

---

### License

Vector-SR is licensed under the [GNU General Public License v3](http://www.gnu.org/copyleft/gpl.html).
