# ⚗️ Parts Lab

Pick the electronics parts you have, get project ideas you can build with them, and follow a full build guide.
A free, installable web app (PWA) for iPhone and PC, with a mad-scientist anime lab vibe and an assistant android named Ohmi.

**Live app:** https://yamimah0849-dot.github.io/parts-lab/

## What's inside
- **305 parts** in 11 categories (24 boards: Arduino, ESP32, ESP8266, Raspberry Pi Pico…), each with a beginner explainer,
  operating voltage, rough price (SAR) and 3.3V/5V warnings. Add your own custom parts too.
- **Create flow**: pick parts → see "Build it now" and "Add 1–2 parts" ideas, with filters and a gacha-style *Surprise me*.
- **102 project ideas** with full guides: parts checklist, color-coded wiring table, numbered steps and complete,
  commented code. **Every sketch is compiled with arduino-cli for every board it lists**, automatically on each push.
- **My Projects**: start a project, tick off steps, celebrate when it's done.
- **Claude helpers** (free, copy-paste): *Get ideas from Claude* for unusual part combos (answers are checked before saving)
  and *Stuck? Ask Claude* with your project, current step and wiring.
- Works offline, saves to your device, with backup export/import.

## Install on iPhone
Open the link in Safari → Share → **Add to Home Screen**.

## For developers
No build step: it's plain HTML/CSS/JS. Serve the folder (`python3 -m http.server`) and open it.

```bash
node tools/validate.mjs            # check parts/ideas data + safety rules
node tools/compile-all.mjs         # compile every sketch (needs arduino-cli + cores + tools/libraries.txt)
```

Parts live in `data/parts/<category>.json`, ideas in `data/ideas/batch-NN.json`, code in `sketches/<id>/<id>.ino`.
Field lists and rules are in `CLAUDE.md`.

## Credits
- Fonts: Orbitron and Share Tech Mono (SIL Open Font License, see `assets/fonts/`)
- Ohmi, icons, pixel art, melodies and sounds: original, made for this app
