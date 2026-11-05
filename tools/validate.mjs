// Parts Lab data checker.
// Usage:  node tools/validate.mjs            (check data files)
//         node tools/validate.mjs --boards   (also ask arduino-cli whether every board FQBN exists)
import { readFileSync } from 'node:fs';
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
warnings.forEach(w => console.log(w));
if (errors.length) { errors.forEach(e => console.log(e)); console.log(`\n${errors.length} error(s).`); process.exit(1); }
console.log('✓ All data checks passed.');
