# RCU — Ryazha Clock Utility

**EN:** Sysmodule + Tesla overlay for Nintendo Switch (Atmosphere CFW). Per-app CPU/GPU/RAM clock profiles, refresh-rate control, autonomous frequency ladder with FPS-aware VRR. Fork of Horizon-OC adapted to the Ryazha ecosystem (libryazhahand overlay, nx-ovlloader, custom IPC). Author: Dimasick-git. License: GPL-2.0.

---

## Что это

RCU — утилита для управления частотами Nintendo Switch (CPU, GPU, RAM, частота экрана) под Atmosphere CFW. Состоит из двух частей:

- **sysmodule** `ryazha-clk.nsp` — фоновый сервис, применяющий частоты. Title ID `00FF0000636C6BFF`. Регистрирует IPC-сервис `hoc:clk` для overlay'я. Содержит автономный ladder-поток Ryazha-Авто, регулирующий CPU/GPU/RAM по FPS, и VRR-Авто, подстраивающий частоту экрана под игру.
- **overlay** `ryazha-clk.ovl` — Tesla-меню. Per-app профили, временные переопределения, настройки. Открывается через любой Ultrahand-совместимый лаунчер (`nx-ovlloader`).

## Состав фич

- Per-application профили (Handheld / Docked / Handheld+Charging вариантах).
- Глобальные временные overrides (на текущую сессию).
- Inline ползунок Hz (Y = сброс на 60 Гц).
- Ryazha-Авто: автономный ladder CPU/GPU/RAM с FPS-аware подстройкой, TDP-кэпом, термальным троттлингом. Конфиг в `/config/ryazha-clk/ryazha-auto.ini`.
- VRR-Авто: governor-режим, при котором sysmodule сам поднимает Smart VRR для приложения. Доступен 4-м пунктом в Governor (Do Not Override / Disabled / Enabled / VRR-Auto).
- 15 локалей (ru, en, uk, de, es, fr, it, nl, pl, pt, ja, ko, zh-cn, zh-tw, jp).
- IPC API v2 для overlay'я: `rclkIpcGetCurrentContext`, `rclkIpcSetProfiles`, `rclkIpcSetOverride`, `rclkIpcGet/SetLadderConfig`, `rclkIpcGet/SetConfigValues`.

## Установка

1. Установить актуальную Atmosphere CFW.
2. Установить `nx-ovlloader` (https://github.com/Dimasick-git/nx-ovlloader или совместимый).
3. Установить Tesla-меню (https://github.com/Dimasick-git/Ryazhahand-Overlay или Ultrahand).
4. Скачать релиз RCU и распаковать `sdout.zip` в корень SD-карты. Содержимое:
   - `atmosphere/contents/00FF0000636C6BFF/` — sysmodule + manifest;
   - `switch/.overlays/ryazha-clk.ovl` — overlay для Tesla;
   - `config/ryazha-clk/` — шаблон конфига и языковые файлы.
5. Запустить консоль. RCU стартует автоматически.

## Использование

- Открыть Tesla-меню (`L + ↓ + R-stick` по умолчанию или назначенный hotkey).
- Выбрать `ryazha-clk.ovl`.
- Главное меню:
  - **Edit App Profile** — профиль для запущенного приложения.
  - **Edit Global Profile** — глобальный профиль (применяется если для приложения нет).
  - **Temporary Overrides** — overrides на текущую сессию (исчезают при перезагрузке).
  - **Settings** — общие настройки + языки.
- **X-кнопка** в главном меню — скрытый экран Ryazha-Авто / VRR, ladder-настройки.

## Конфигурация

- `/config/ryazha-clk/config.ini` — глобальные настройки (создаётся автоматически при первом запуске).
- `/config/ryazha-clk/ryazha-auto.ini` — конфиг авто-ladder'а (CPU/GPU min/max границы, TDP-cap, VRR диапазон, predictor).
- `/config/ryazha-clk/lang/*.json` — переводы. Активный язык в config.ini → `[config] language=ru`.
- `/config/ryazha-clk/log.flag` — пустой файл-маркер для включения детального лога в `log.txt`.

## Сборка из исходников

Требуется devkitPro switch-dev окружение.

```sh
# Установить devkitPro: https://devkitpro.org/wiki/Getting_Started

git clone --recurse-submodules https://github.com/Dimasick-git/RCU.git
cd RCU

export DEVKITPRO=/opt/devkitpro
./Source/ryazha-clk/build.sh
```

Результат — `dist/atmosphere/contents/00FF0000636C6BFF/exefs.nsp` (sysmodule) и `dist/switch/.overlays/ryazha-clk.ovl` (overlay).

CI workflow: `.github/workflows/build.yml`. Релизные артефакты собираются в `dist/` и пакуются в `sdout.zip`.

## Архитектура

```
overlay (ryazha-clk.ovl)
  └─ Tesla GUI (libryazhahand submodule)
     └─ IPC client (common/src/client/ipc.c)
        ↓ hoc:clk service (8-char limit)
sysmodule (ryazha-clk.nsp)
  ├─ clockManager (Tick loop, applies profile freqs)
  ├─ ipcService (request dispatcher, RClkIpcCmd_* enum)
  ├─ autoRyazha (Ryazha-Авто tick thread, 200ms интервал)
  ├─ governor (CPU/GPU/VRR per-component on/off, включая VrrAuto)
  └─ board (Erista/Mariko/Aula board-specific clocks)
```

## Опасность

Разгон RAM может повредить данные на NAND/SD. Делайте полный бэкап NAND + emuMMC + SD перед использованием. Дефолтные значения консервативны. Превышение паспортных частот — на свой риск.

## Лицензия

GPL-2.0. См. `LICENSE`. Историческое происхождение — sys-clk (m4xw, natinusala, p-sam) → Horizon-OC → Ryazha-CLK.

Авторы: m4xw, natinusala, p-sam, Souldbminer, Lightos_, Dominatorul, Dimasick-git.
