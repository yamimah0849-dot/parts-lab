// Parts Lab data checker.
// Usage:  node tools/validate.mjs            (check data files)
//         node tools/validate.mjs --boards   (also ask arduino-cli whether every board FQBN exists)
import { readFileSync, existsSync } from 'node:fs';
import { execFileSync } from 'node:child_process';
import { ICON_KEYS } from '../js/icons.js';

const root = new URL('../', import.meta.url);
const read = p => JSON.parse(readFileSync(new URL(p, root), 'utf8'));
const errors = [];
const warnings = [];
const err = (id, msg) => errors.push(`✗ ${id}: ${msg}`);
const warn = (id, msg) => warnings.push(`! ${id}: ${msg}`);
const icons = new Set(ICON_KEYS);

const { categories } = read('data/categories.json');
const catIds = new Set(categories.map(c => c.id));
categories.forEach(c => {
  ['id', 'name', 'jp', 'icon', 'color'].forEach(k => { if (!c[k]) err(`category ${c.id}`, `missing ${k}`); });
  if (!icons.has(c.icon)) err(`category ${c.id}`, `unknown icon "${c.icon}"`);
});

const parts = [];
for (const c of categories) {
  let list;
  try { list = read(`data/parts/${c.id}.json`); } catch (e) { err(c.id, `cannot read data/parts/${c.id}.json: ${e.message}`); continue; }
  if (!Array.isArray(list)) { err(c.id, 'file must contain an array'); continue; }
  list.forEach(p => parts.push({ ...p, _file: c.id }));
}

const ids = new Set();
const groups = new Map();
for (const p of parts) {
  const id = p.id || '(no id)';
  for (const k of ['id', 'name', 'category', 'icon', 'desc', 'voltage', 'price']) if (p[k] === undefined || p[k] === '') err(id, `missing "${k}"`);
  if (!Array.isArray(p.interfaces)) err(id, '"interfaces" must be an array');
  if (!Array.isArray(p.tags) || !p.tags.length) err(id, '"tags" must be a non-empty array');
  if (p.id && !/^[a-z0-9]+(-[a-z0-9]+)*$/.test(p.id)) err(id, 'id must be kebab-case (a-z, 0-9, dashes)');
  if (ids.has(p.id)) err(id, 'duplicate id'); ids.add(p.id);
  if (!catIds.has(p.category)) err(id, `unknown category "${p.category}"`);
  if (p.category !== p._file) err(id, `category "${p.category}" but stored in ${p._file}.json`);
  if (p.icon && !icons.has(p.icon)) err(id, `unknown icon "${p.icon}"`);
  if (p.desc && p.desc.length > 260) warn(id, `desc is long (${p.desc.length} chars), keep it to 1–2 sentences`);
  if (p.price && !(Array.isArray(p.price) && p.price.length === 2 && p.price.every(n => typeof n === 'number' && n >= 0) && p.price[0] <= p.price[1])) err(id, 'price must be [low, high] numbers in SAR');
  if (p.vcc && !(Array.isArray(p.vcc) && p.vcc.length === 2 && p.vcc[0] <= p.vcc[1])) err(id, 'vcc must be [min, max]');
  if (p.warn) for (const k of Object.keys(p.warn)) if (!['3.3', '5'].includes(k)) err(id, `warn key "${k}" must be "3.3" or "5"`);
  if (p.color && !/^#[0-9a-f]{6}$/i.test(p.color)) err(id, 'color must be #rrggbb');
  if (p.category === 'boards') {
    if (![3.3, 5].includes(p.logic)) err(id, 'boards need "logic": 3.3 or 5');
    if (!p.fqbn) err(id, 'boards need "fqbn"');
  } else if (p.logic || p.fqbn) err(id, '"logic"/"fqbn" are only for boards');
  if (p.variantGroup) {
    if (!p.variantNote) err(id, 'variantGroup needs a variantNote explaining the difference');
    groups.set(p.variantGroup, (groups.get(p.variantGroup) || 0) + 1);
  }
}
for (const [g, n] of groups) if (n < 2) err(`variantGroup ${g}`, 'only one member: variants need at least 2 parts');

const basics = parts.filter(p => p.basic).map(p => p.id).sort().join(',');
if (basics !== 'breadboard,jumper-wires,resistor-kit,usb-cable') err('basics', `basic parts should be breadboard, jumper-wires, resistor-kit, usb-cable (found: ${basics})`);

// ---------------------------------------------------------------- ideas
const byId = new Map(parts.map(p => [p.id, p]));
const BASICS = ['breadboard', 'jumper-wires', 'resistor-kit', 'usb-cable'];
const VIBES = ['fun', 'useful', 'gaming', 'gift'];
const libsAllowed = new Set(readFileSync(new URL('tools/libraries.txt', root), 'utf8').split('\n').map(l => l.trim()).filter(l => l && !l.startsWith('#')));
const LED_PARTS = id => /^(led-|rgb-led-|seg7-)/.test(id);
const MOTOR_PARTS = ['tt-motor', 'dc-motor-130', 'vibration-motor', 'mini-pump'];
const DRIVERS = ['l298n', 'tb6612fng', 'npn-2n2222', 'mosfet-irlz44n', 'relay-5v', 'uln2003'];
const DISCRETE_SWITCHES = ['npn-2n2222', 'mosfet-irlz44n'];
let ideaCount = 0;
const ideaIds = new Set();
let ideaFiles = [];
try { ideaFiles = read('data/ideas/index.json').files; } catch (e) { err('ideas', `cannot read data/ideas/index.json: ${e.message}`); }
for (const f of ideaFiles) {
  let list;
  try { list = read(`data/ideas/${f}`); } catch (e) { err(f, `cannot read: ${e.message}`); continue; }
  for (const idea of list) {
    ideaCount += 1;
    const id = `idea ${idea.id || '(no id)'}`;
    for (const k of ['id', 'title', 'pitch', 'emoji', 'difficulty', 'time', 'vibes', 'boards', 'requires', 'guide']) if (idea[k] === undefined) err(id, `missing "${k}"`);
    if (ideaIds.has(idea.id)) err(id, 'duplicate idea id'); ideaIds.add(idea.id);
    if (![1, 2, 3].includes(idea.difficulty)) err(id, 'difficulty must be 1, 2 or 3');
    if (!(idea.time > 0)) err(id, 'time must be minutes > 0');
    (idea.vibes || []).forEach(v => { if (!VIBES.includes(v)) err(id, `unknown vibe "${v}"`); });
    if (!idea.vibes?.length) err(id, 'needs at least one vibe');
    (idea.boards || []).forEach(b => { if (byId.get(b)?.category !== 'boards') err(id, `"${b}" is not a board`); });
    if (!idea.boards?.length) err(id, 'needs at least one board');
    const req = (idea.requires || []).flatMap(r => (Array.isArray(r) ? r : [r]));
    req.forEach(r => { if (!byId.has(r)) err(id, `unknown required part "${r}"`); if (BASICS.includes(r)) err(id, `"${r}" is a basic: list it in "basics", not "requires"`); if (byId.get(r)?.category === 'boards') err(id, `board "${r}" belongs in "boards"`); });
    (idea.optional || []).forEach(r => { if (!byId.has(r)) err(id, `unknown optional part "${r}"`); });
    (idea.basics || []).forEach(b => { if (!BASICS.includes(b)) err(id, `"${b}" is not a basic`); });
    (idea.libs || []).forEach(l => { if (!libsAllowed.has(l)) err(id, `library "${l}" is not in tools/libraries.txt`); });
    const g = idea.guide || {};
    if (!g.about) err(id, 'guide.about missing');
    if (!Array.isArray(g.steps) || g.steps.length < 3) err(id, 'guide.steps needs at least 3 steps');
    if (!Array.isArray(g.parts) || !g.parts.length) err(id, 'guide.parts missing');
    (g.parts || []).forEach(p => { if (!Array.isArray(p) || !byId.has(p[0])) err(id, `guide part ${JSON.stringify(p)} is not a known part id`); });
    (g.wiring || []).forEach(w => { if (!Array.isArray(w) || w.length < 2 || !w[0] || !w[1]) err(id, `bad wiring row ${JSON.stringify(w)}`); });
    if (!existsSync(new URL(`sketches/${idea.id}/${idea.id}.ino`, root))) err(id, `missing sketches/${idea.id}/${idea.id}.ino`);
    // Every required/optional part must appear in the guide's parts list
    const guideIds = new Set((g.parts || []).map(p => p[0]));
    for (const r of idea.requires || []) { const alts = Array.isArray(r) ? r : [r]; if (!alts.some(a => guideIds.has(a))) err(id, `required part ${alts.join('/')} is not in guide.parts`); }

    // --- safety rules from the user ---
    const all = new Set([...req, ...(idea.optional || []), ...guideIds]);
    const text = JSON.stringify(g).toLowerCase();
    if ([...all].some(LED_PARTS) && !guideIds.has('resistor-kit')) err(id, 'uses LEDs but the guide has no resistors (resistor-kit)');
    if ([...all].some(LED_PARTS) && !/(220|330|100|150|1k)\s*(Ω|ohm)/i.test(JSON.stringify(g))) warn(id, 'uses LEDs: mention the resistor value in parts/wiring');
    if (req.some(r => MOTOR_PARTS.includes(r))) {
      if (!req.some(r => DRIVERS.includes(r))) err(id, 'drives a motor/pump without a driver, transistor or MOSFET in "requires"');
      if (req.some(r => DISCRETE_SWITCHES.includes(r)) && !req.includes('diode-1n4007')) err(id, 'transistor/MOSFET motor switching needs diode-1n4007 (flyback) in "requires"');
    }
    if (req.includes('servo-mg996r') && !/external|separate/.test(text)) err(id, 'MG996R needs an external supply mentioned in the guide');
    for (const b of idea.boards || []) {
      const lvl = String(byId.get(b)?.logic);
      for (const r of req) {
        const w = byId.get(r)?.warn?.[lvl];
        if (w && !/(divider|level shifter|level-shifter|3\.3v pin|to 3\.3v|from 3\.3v|1kω|1k ohm)/.test(text)) err(id, `${r} has a ${lvl}V-board warning but the guide doesn't address it (divider / level shifter / 3.3V supply)`);
      }
    }
  }
}

if (process.argv.includes('--boards')) {
  for (const b of parts.filter(p => p.fqbn)) {
    try {
      execFileSync('arduino-cli', ['board', 'details', '-b', b.fqbn], { stdio: 'pipe' });
      console.log(`  ✓ ${b.fqbn}`);
    } catch (e) {
      err(b.id, `arduino-cli does not know FQBN "${b.fqbn}": ${String(e.stderr || e.message).split('\n')[0]}`);
    }
  }
}

const perCat = categories.map(c => `${c.id}:${parts.filter(p => p.category === c.id).length}`).join('  ');
console.log(`Parts: ${parts.length}   ${perCat}`);
console.log(`Ideas: ${ideaCount}`);
warnings.forEach(w => console.log(w));
if (errors.length) { errors.forEach(e => console.log(e)); console.log(`\n${errors.length} error(s).`); process.exit(1); }
console.log('✓ All data checks passed.');
