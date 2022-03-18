# GPPMagicGlassSrv

GPPMagicGlassSrv可通过pipe接受json操作指令，调用GPPMagicGlassRT.dll（MagpieRT.dll）

特别说明：
1. Runtime的初始化与Run需要放在同一线程中，同时Run是阻塞的。
2. 更改获取硬件显卡名称、LUID，同时判断显卡是否支持DX10、DX11，通过json字符串返回。
3. Run通过LUID获取显卡序号再创建。
4. 此项目为magpie/compute分支更改（2022-3-18 修复多屏下退出崩溃）。

# Magpie
[![All Contributors](https://img.shields.io/github/all-contributors/Blinue/Magpie)](#%E8%B4%A1%E7%8C%AE%E8%80%85-)
[![许可](https://img.shields.io/github/license/Blinue/Magpie)](./LICENSE)

