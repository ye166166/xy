#pragma once

// ============================================================
// ui.h - 深色科技风界面
//   聊天 / 设备列表 / 本机状态 / 航点 / 设置 五个界面 + 求救遮罩。
// 仅依赖 mesh/gnss/radio/store 的数据接口，负责全部绘制。
// ============================================================

#include <cstdint>
#include <cstddef>

enum class Screen : uint8_t {
  CHAT = 0,
  DEVICES,
  STATUS,
  WAYPOINTS,
  SETTINGS,
};

void uiInit();
void uiRender();

Screen uiScreen();
void   uiGoto(Screen s);
void   uiForceRedraw();

// ---- 文本输入（聊天 / 设置编辑共用输入行） ----
void uiInputAppend(char c);
void uiInputBackspace();
void uiInputClear();
const char* uiInputText();

// ---- 聊天滚动 ----
void uiChatScrollToBottom();
void uiChatScrollBy(int delta);

// ---- 列表选择 ----
void uiSelMove(int delta);
int  uiSel();

// ---- 设置面板编辑状态 ----
bool uiSettingEditing();
void uiSettingStartEdit();
void uiSettingCommitEdit();
void uiSettingCancelEdit();

// ---- SOS 告警遮罩 ----
void uiSosOverlayShow(uint32_t source, bool local);
void uiSosOverlayHide();
bool uiSosOverlayVisible();

// ---- 底部提示条（短暂显示） ----
void uiToast(const char* text);

// 双缓冲画布是否可用（用于诊断）
bool uiCanvasActive();
