# TinyBLE-Host

个人对自身学习内容的验证产物，目前已简单实现基于ble的小场景，后续可能会做更多开发。

![License](https://img.shields.io/badge/license-MIT-green) ![Platform](https://img.shields.io/badge/platform-STM32%20%7C%20Windows-blue)

## ✨ 项目特点

* **跨平台开发**：
    * **Windows**: 使用 CMake + MinGW。
    * **Embedded**: 完美运行于 STM32F103 (Bare-metal)。
* **功能支持**：HCI (H4), L2CAP, ATT/GATT (Server), SMP (Just Works), HID over GATT.

## 📂 目录结构

* `examples/`: 工程示例 (STM32 Keil 工程 & Windows CMake 工程)
* `source/`: 协议栈核心源码 (HCI, L2CAP, ATT, GATT, SMP)
* `utils/`: FIFO、日志工具
* `platform/`: 适配层 (STM32F103)
* `Third_party/`: 加密库 (TinyCrypt)

## 🚀 快速开始

### Windows 仿真 (调试逻辑)
```bash
cd examples/windows
cmake -B build
cmake --build build
./build/TinyBLE_Sim
```

📝 TODO / Roadmap

    [ ] RTOS 适配: 增加 FreeRTOS 移植接口层，支持多任务环境下的消息队列通信。
    [ ] 安全性增强: 完善 SMP 模块，支持 Passkey Entry（密码输入）配对模式，以及保存密钥。
    [ ] BLE UART: 添加私有透传服务 (Throughput Demo)。
    [ ] 完善内存分配策略，支持动态申请内存。
    [ ] 完善边界检查，增加鲁棒性。