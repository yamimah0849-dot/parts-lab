# Parts Lab: project notes for Claude

## Goal
A free PWA (works on iPhone + PC) with a big catalog of hobby electronics parts.
User presses **Create**, picks parts, and gets project ideas they can build. Picking an idea
opens a full build guide (parts, steps, wiring table, compile-checked code).

## About the user (read this first)
- Beginner who can't really code. Explain simply and define jargon in a few words.
- Uses a PC and an iPhone (so: no Web Bluetooth/Web Serial; prefer Wi-Fi web pages, BLE via free apps, ntfy for push). Repo owner: `yamimah0849-dot`.
- Everything must be 100% free: no paid APIs, subscriptions or hosting.
- Prices are shown in **Saudi Riyal (SAR)**, rough online ranges (AliExpress → Amazon.sa). Always label them "approx."
- Wants a fun **anime / mad-scientist lab** style: sounds, pictures, mascot, animations.
- Persona: the user likes Claude to talk like Makise Kurisu (Steins;Gate). In-app text stays original (no copied characters/art).

## How to work with the user
1. Ask questions and show a short plan before writing code; wait for OK.
2. Originally: build in phases and stop after each. **In session 1 the user said "finish the whole thing and then show me" (they were away)**, so phases 2–4 were built in one go. For future feature requests, go back to: plan → OK → build → plain test steps.
3. Keep this file updated (progress log at the bottom).
4. If unsure whether something is electrically correct, say so instead of guessing.

## Tech choices
- Plain HTML + CSS + JavaScript (ES modules), **no framework, no build step**.
- Service worker (`sw.js`, network-first + offline cache). Bump `CACHE` when the shell file list changes.
- Data lives in JSON files under `data/` (separate from code). Sketches in `sketches/<id>/<id>.ino`.
- User data in `localStorage` (`partslab.v1`): tray, custom parts, imported Claude ideas, projects, filters, settings. Export/import backup in Settings.
- Sounds synthesized with the Web Audio API (no audio files).
- Icons hand-drawn SVG in `js/icons.js`. Mascot "Ohmi" is an original chibi android girl SVG in `js/mascot.js`.
- Fonts self-hosted (`assets/fonts/`, SIL OFL): Orbitron + Share Tech Mono.
- Hosting: GitHub Pages (free), relative paths only.

## Folder map
```
index.html              app shell
manifest.webmanifest    PWA manifest
sw.js                   service worker
css/style.css           all styles (design tokens at top)
js/app.js               boot, router, home, part picker, tray, part sheets, custom parts, settings
js/ui.js                shared helpers: escaping, sheets, toasts, cut-ins, manga pops, copyText
js/ideas.js             idea library loader + matching engine + filters
js/ideas-ui.js          results (filters, gacha), idea guide/project mode, My Projects, help, Claude sheets
js/claude.js            copy-paste prompts (Get ideas / Stuck?) + validation of pasted Claude JSON
js/data.js              catalog loader (+ custom parts)
js/store.js             localStorage state + export/import
js/icons.js / mascot.js / sfx.js / checks.js
data/categories.json, data/parts/<cat>.json     305 parts
data/ideas/index.json, data/ideas/batch-NN.json 102 ideas
data/compile-report.json                        written by tools/compile-all.mjs (shown in app as "compile-checked")
sketches/<id>/<id>.ino                          one Arduino sketch per idea
tools/validate.mjs      data + safety-rule checker (`node tools/validate.mjs --boards` also checks FQBNs)
tools/compile-all.mjs   compiles every sketch for every listed board (`--only a,b`, `--jobs N`, `--install`)
tools/libraries.txt     Arduino libraries the sketches use
.github/workflows/compile.yml  free CI: validate + compile everything on push
```

## Part schema (data/parts/*.json)
| field | required | meaning |
|---|---|---|
| id | yes | unique kebab-case id |
| name | yes | display name, include variant |
| category | yes | must match a category id and the file it's in |
| icon | yes | key in `js/icons.js` |
| desc | yes | 1–2 sentence beginner explainer |
| voltage | yes | human-readable operating voltage |
| vcc | no | numeric supply range [min, max] |
| logic / fqbn | boards only | 5 or 3.3 / arduino-cli board id |
| interfaces, tags | yes | arrays |
| price | yes | [low, high] SAR approx |
| basic | no | breadboard / jumper-wires / resistor-kit / usb-cable only |
| variantGroup / variantNote | no | parts that wire differently share a group |
| note / power / warn | no | tip; power warning (Lab Check); {"3.3": msg, "5": msg} board-voltage warnings |
| color | no | LED color for icon tint |

## Idea schema (data/ideas/batch-NN.json)
`id, title, pitch, emoji, difficulty (1-3), time (min), vibes [fun|useful|gaming|gift], boards [ids, all compiled],
fqbn {boardId: override} (optional), requires [id | [alternatives]] (no boards, no basics), optional [ids],
basics [ids], libs [Library Manager names, must be in tools/libraries.txt],
guide {about, parts [[id, qty, note]], wiring [[from, to, note]], steps [...], test, safety [...], next [...]}`.
Matching: an idea shows if it uses ≥1 picked (non-basic) part and misses ≤2 requirements; sorted by parts used.

## Accuracy & safety rules (from the user, non-negotiable) — enforced by tools/validate.mjs
- Compile every sketch for every board in `boards` (compile-all + CI). Never ship code that doesn't compile.
- Wiring + code must match the exact part variant.
- LEDs get resistors (220Ω on 5V, 100Ω on 3.3V). Passive buzzers get 220Ω (magnetic ones are ~16Ω).
  Active buzzers via transistor. Motors/pumps: driver or transistor/MOSFET + 1N4007 flyback diode, separate supply.
- 3.3V/5V: any `warn` on a required part must be addressed in the guide (divider / level shifter / 3.3V supply).
- Safety notes for soldering, lithium, mains (beginner rule: never mains), heat, lasers, alarms ("not a safety device").

## Dev environment (cloud session)
- Network allowlist includes `downloads.arduino.cc`. arduino-cli: `.../arduino-cli_latest_Linux_64bit.tar.gz` → `/usr/local/bin`.
- arduino-cli ignores HTTPS_PROXY: `arduino-cli config init` then `arduino-cli config set network.proxy "$HTTPS_PROXY"`.
- Board URLs (espressif.github.io is blocked here, raw GitHub works):
  `arduino-cli config set board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json https://arduino.esp8266.com/stable/package_esp8266com_index.json`
- Cores: arduino:avr 1.8.8, arduino:megaavr 1.8.8, arduino:renesas_uno 1.6.0, esp32:esp32 3.3.12, esp8266:esp8266 3.1.2, rp2040:rp2040 6.3.2.
- Uno R4 (renesas_uno) package URL is plain http (proxy refuses it): download
  `https://downloads.arduino.cc/cores/staging/ArduinoCore-renesas_uno-1.6.0.tar.bz2` into `~/.arduino15/staging/packages/` first, then `arduino-cli core install arduino:renesas_uno`.
- Libraries: `node tools/compile-all.mjs --install --only <something>` installs everything in tools/libraries.txt.
- Preview: `python3 -m http.server 8080`, then Playwright (preinstalled) screenshots at 390x844 isMobile.

## Git / deploy
- Work on branch `dev`; GitHub Pages serves `dev` (user enabled it in Settings → Pages).
- PR https://github.com/yamimah0849-dot/parts-lab/pull/1 (`dev → main`) is the save point; merge when the user is happy.
- Live URL: https://yamimah0849-dot.github.io/parts-lab/
- `gh pr create` (GraphQL) is blocked here; use `gh api -X POST repos/<owner>/<repo>/pulls ...` (REST).

## Lessons learned
- Generic `esp32:esp32:esp32` / `esp32cam` define no LED_BUILTIN: DevKit blue LED = GPIO 2.
- LedControl is AVR-only (avr/pgmspace.h): on ESP32 drive the MAX7219 with shiftOut (see wifi-scrolling-sign).
- TM1637Display.h defines SEG_A..SEG_G macros: don't name your own constants SEG_*.
- IRremote uses Timer2 on Uno: avoid PWM on pins 3/11; IR send pin is fixed to 3.
- Nano Every PWM pins are 3, 5, 6, 9, 10 only (no 11).
- Adafruit SSD1306 allocates 1 KB at runtime on Uno: keep globals < ~700 B (use F() strings).
- HC-05/HC-06 (classic BT) don't work with iPhone; HM-10 / ESP32 BLE do (with nRF Connect / LightBlue).
- iPhone: Web Audio muted by the silent switch; audio only after a tap.
- Mobile overflow bug pattern: flex/grid children with horizontally scrolling chips need `min-width: 0`.

## Progress log
- **Session 1 (2026-11-05)**
  - Plan approved; env set up; Phase 1 (102 parts, picker, tray, Lab Check, custom parts, PWA) tested by user on the live site.
  - User asked for "more anime" and to finish everything while away. Built:
    - Anime upgrade: Ohmi redesign, visual-novel dialogue box, cut-ins, manga sound-effect pops, sakura petals, halftone.
    - Phase 2: idea engine, results with level/vibe/time filters, gacha "Surprise me".
    - Phase 3: guides (parts checklist, color-coded wiring table, steps, highlighted code + copy), Start project,
      My Projects with progress + "mission complete", Stuck? Ask Claude, export/import backup, How-to-upload help,
      compile-all tool + compile report + GitHub Action (passing).
    - Phase 4: catalog 305 parts (24 boards), 102 ideas in 8 batches (all compile-checked), boards widened to Uno R4 /
      Nano Every / Pico 2 where they compile, Get ideas from Claude (copy prompt, paste + validate + save),
      iPhone install hint + steps.
- **Next ideas** (ask the user first): more ideas for unused new parts (LoRa, CAN, e-paper, TF-Luna...), a parts
  "inventory" mode (my parts at home), share a project as a link, Arabic UI option.
