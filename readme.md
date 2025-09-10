# 🎴 Blackjack Twist  

A modern take on the classic **Blackjack** game, built with **C++ and Qt6**.  
Play safe with a starting balance… or risk your folders in higher difficulties!  

Inspired by [CodeBullet’s Snake Game](https://youtu.be/2YdPIB7wVFk).  

---

## ✨ Features
- 🃏 Classic Blackjack rules: **Hit, Stand, Double, Split, Surrender**  
- 🗂 **Difficulty modes**:  
  - **Easy** – Start with $10,000 (safe mode)  
  - **Normal** – Wager against a chosen folder on your system  
  - **Hard** – Risk your Windows folder (⚠️ extreme mode)  
- 💾 **Save/Load game state** anytime  
- 🎨 Styled UI with card graphics and smooth layouts  
- 🔀 Play with 1–8 decks  

---

## 📦 Installation
### Windows (Prebuilt)
1. Download the latest release from the [Releases page](../../releases).  
2. Extract **`Blackjack_Twist_Windows.zip`**.  
3. Run `blackjack_twist.exe`.  

No setup required — all Qt runtime files are bundled.  

---

## 🛠 Building from Source
You’ll need:
- Qt 6.x (Widgets, Core, Gui, Svg modules)  
- CMake (3.16+)  
- A C++17-compatible compiler (MSVC / MinGW / Clang)  
cmake ..
cmake --build .
