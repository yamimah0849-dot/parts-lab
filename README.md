# ⚗️ Parts Lab

Pick the electronics parts you have, get project ideas you can build with them, and follow a full build guide.
A free, installable web app (PWA) for iPhone and PC with a mad-scientist lab vibe and a robot assistant named Ohmi.

**Live app:** https://yamimah0849-dot.github.io/parts-lab/

## Status
- ✅ Phase 1: parts catalog (102 parts, 11 categories), Create part picker, tray, Lab Check, custom parts
- ⏳ Phase 2: idea library, matching and results
- ⏳ Phase 3: build guides, My Projects, compile-checked code
- ⏳ Phase 4: 300+ parts, 100+ ideas, Claude helper buttons, polish

## Adding parts
Parts live in `data/parts/<category>.json`. Copy an existing entry, change the fields, then run:

```bash
node tools/validate.mjs
```

The field list is in `CLAUDE.md`.

## Credits
- Fonts: Orbitron and Share Tech Mono (SIL Open Font License, see `assets/fonts/`)
- Mascot, icons and sounds: original, made for this app
