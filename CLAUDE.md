# Parts Lab: project notes for Claude

## Goal
A free PWA (works on iPhone + PC) with a big catalog of hobby electronics parts.
User presses **Create**, picks parts, and gets project ideas they can build. Picking an idea
opens a full build guide (parts, steps, wiring table, compiled code).

## About the user (read this first)
- Beginner who can't really code. Explain simply and define jargon in a few words.
- Uses a PC and an iPhone. Repo owner: `yamimah0849-dot`.
- Everything must be 100% free: no paid APIs, subscriptions or hosting.
- Prices are shown in **Saudi Riyal (SAR)**, rough online ranges (AliExpress → Amazon.sa). Always label them "approx."
- Wants a fun **anime / mad-scientist lab** style: sounds, pictures, mascot, animations.
- Persona: the user likes Claude to talk like Makise Kurisu (Steins;Gate). In-app text stays original (no copied characters/art).

## How to work with the user
1. Ask questions and show a short plan before writing code; wait for OK.
2. Build in phases. After each phase: stop, give plain test steps, wait for confirmation.
3. Keep this file updated (progress log at the bottom).
4. If unsure whether something is electrically correct, say so instead of guessing.

## Tech choices
- Plain HTML + CSS + JavaScript (ES modules), **no framework, no build step**.
- Service worker (`sw.js`) for offline + installable PWA; `manifest.webmanifest`.
- Data lives in JSON files under `data/` (separate from code).
- User data (tray, custom parts, settings; later My Projects) in `localStorage`, with JSON export/import (Phase 3).
- Sounds are synthesized live with the Web Audio API (no audio files, no copyright issues).
- Icons are hand-drawn SVG in `js/icons.js`. Mascot "Ohmi" is an original SVG robot in `js/mascot.js`.
- Fonts self-hosted (`assets/fonts/`, SIL Open Font License): Orbitron (display) + Share Tech Mono (terminal).
- Hosting: GitHub Pages (free). All paths are relative so it works under `/parts-lab/`.

## Folder map
```
index.html              app shell
manifest.webmanifest    PWA manifest
sw.js                   service worker (network-first, offline fallback)
css/style.css           all styles (design tokens at top)
js/app.js               boot, router, screens
js/data.js              loads catalog JSON + custom parts
js/store.js             localStorage state
js/icons.js             SVG icon library (part + category icons)
js/mascot.js            mascot SVG + speech bubble
js/sfx.js               Web Audio sound effects
js/checks.js            "Lab Check": 3.3V/5V and power warnings for the tray
data/categories.json    category list (id, name, jp, icon, color)
data/parts/<cat>.json   one file per category
assets/icons/           app icons (PNG for iOS home screen)
tools/validate.mjs      data checker: run `node tools/validate.mjs`
```

## Part schema (data/parts/*.json)
| field | required | meaning |
|---|---|---|
| id | yes | unique kebab-case id |
| name | yes | display name, include variant (e.g. "16x2 LCD with I2C backpack") |
| category | yes | must match a category id and the file it's in |
| icon | yes | key in `js/icons.js` |
| desc | yes | 1–2 sentence beginner explainer |
| voltage | yes | human-readable operating voltage |
| vcc | no | numeric supply range [min, max] |
| logic | boards only | 5 or 3.3 |
| fqbn | boards only | arduino-cli board id used for compile checks |
| interfaces | yes | e.g. ["I2C"] |
| tags | yes | search keywords |
| price | yes | [low, high] SAR approx |
| basic | no | true = breadboard/jumpers/resistor kit/USB cable (always available toggle) |
| variantGroup / variantNote | no | parts that wire differently share a group; note says how |
| note | no | important tip shown on the detail sheet |
| power | no | power warning shown in Lab Check whenever the part is in the tray |
| warn | no | {"3.3": msg} shown when a 3.3V board is in the tray; {"5": msg} for 5V boards |
| color | no | LED color for icon tint |

## Accuracy & safety rules (from the user, non-negotiable)
- Compile every project's code with arduino-cli for its target board(s). Never ship code that doesn't compile.
- Wiring + code must match the exact part variant.
- LEDs always get current-limiting resistors. Motors never powered from pins: transistor/driver + flyback diode; say when external power is needed. Flag 3.3V vs 5V mismatches.
- Safety notes where it matters (soldering, power, polarity, lithium, mains).

## Dev environment (cloud session)
- The cloud environment's network allowlist includes `downloads.arduino.cc` (added in session 1).
- arduino-cli: download `https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_64bit.tar.gz`, put the binary in `/usr/local/bin`.
- arduino-cli ignores HTTPS_PROXY by itself: run `arduino-cli config init` then `arduino-cli config set network.proxy "$HTTPS_PROXY"`.
- Board URLs (espressif.github.io is blocked, use raw GitHub):
  `arduino-cli config set board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json`
- Cores installed: arduino:avr 1.8.8, esp32:esp32 3.3.12, rp2040:rp2040 6.3.2.
- Local preview: `python3 -m http.server 8080` in the repo, then screenshot with Playwright (preinstalled).

## Git / deploy
- Work happens on branch `dev`. GitHub Pages serves the `dev` branch during development so the user can test each phase from a link.
- Pull request `dev → main` = a stable save point after each confirmed phase.
- Live URL: https://yamimah0849-dot.github.io/parts-lab/

## Lessons learned (keep for Phase 3 guides)
- Generic `esp32:esp32:esp32` and `esp32cam` define **no LED_BUILTIN**: use the real pin
  (DevKit V1 blue LED = GPIO 2; ESP32-CAM flash LED = GPIO 4, red LED = GPIO 33 active-LOW).
- All 10 board FQBNs compile a test sketch (checked session 1).
- HC-05 doesn't work with iPhone (classic Bluetooth). User has an iPhone, so prefer ESP32 BLE/Wi-Fi ideas for phone control.
- iPhone: Web Audio is muted when the silent switch is on.

## Progress log
- **Session 1 (2026-11-05)**: Plan approved. Environment set up (arduino-cli + 3 cores, `downloads.arduino.cc` allowed).
  **Phase 1 built**: 102-part catalog (11 categories, validated), home screen with reactor CREATE button,
  mascot Ohmi (moods + typed speech), part picker (search, category chips, basics toggle, selection tray),
  Lab Check (3.3V/5V + power warnings from part data), part detail sheets with variants,
  custom parts, settings, boot sequence, synthesized sounds, PWA manifest + service worker + app icons.
  Waiting for the user to test Phase 1 (GitHub Pages must be switched on by the user: Settings → Pages → branch `dev`).
- **Next (Phase 2)**: idea library (first 20 ideas) as JSON, matching engine ("Build it now" / "Add 1–2 parts"),
  results screen with sort + filters (difficulty, vibe, build time) + Surprise me.
