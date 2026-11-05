// Parts Lab — idea library loader + matching engine.
import { catalog } from './data.js';
import { store } from './store.js';

export const BASICS = ['breadboard', 'jumper-wires', 'resistor-kit', 'usb-cable'];
export const VIBES = [
  { id: 'fun', name: 'Fun', jp: 'たのしい', emoji: '🎉' },
  { id: 'useful', name: 'Useful', jp: 'べんり', emoji: '🛠️' },
  { id: 'gaming', name: 'Gaming / anime', jp: 'ゲーム', emoji: '🎮' },
  { id: 'gift', name: 'Gift', jp: 'プレゼント', emoji: '🎁' },
];
export const DIFFICULTY = { 1: 'Beginner', 2: 'Easy-intermediate', 3: 'Intermediate' };

export const library = { builtin: [], ideas: [], byId: new Map(), report: null };

async function fetchJSON(url) {
  const r = await fetch(url, { cache: 'no-cache' });
  if (!r.ok) throw new Error(`Could not load ${url} (${r.status})`);
  return r.json();
}

export async function loadIdeas() {
  const idx = await fetchJSON('data/ideas/index.json');
  const lists = await Promise.all(idx.files.map(f => fetchJSON(`data/ideas/${f}`)));
  library.builtin = lists.flat();
  try { library.report = await fetchJSON('data/compile-report.json'); } catch { library.report = null; }
  rebuildIdeas();
  return library;
}

// Built-in ideas + ideas the user imported from Claude.
export function rebuildIdeas() {
  const custom = store.get().customIdeas.map(i => ({ ...i, fromClaude: true }));
  library.ideas = [...library.builtin, ...custom];
  library.byId = new Map(library.ideas.map(i => [i.id, i]));
}

// Each requirement is a list of acceptable part ids (any one of them is enough).
export function requirementsOf(idea, basicsOn = true) {
  const reqs = [[...(idea.boards || [])]];
  for (const r of idea.requires || []) reqs.push(Array.isArray(r) ? [...r] : [r]);
  if (!basicsOn) for (const b of idea.basics || []) reqs.push([b]);
  return reqs.filter(r => r.length);
}

export function cheapest(alts) {
  const parts = alts.map(id => catalog.byId.get(id)).filter(Boolean);
  parts.sort((a, b) => (a.price?.[0] ?? 9999) - (b.price?.[0] ?? 9999));
  return parts[0] || null;
}

// Match ideas against the tray. Returns { ready: [...], almost: [...] } sorted by
// how many of the user's picked parts each idea uses.
export function matchIdeas(trayIds, basicsOn) {
  const picked = new Set(trayIds);
  const have = new Set(trayIds);
  if (basicsOn) BASICS.forEach(b => have.add(b));
  const out = [];
  for (const idea of library.ideas) {
    const missing = [];
    const used = new Set();
    for (const alts of requirementsOf(idea, basicsOn)) {
      const hit = alts.find(a => have.has(a));
      if (hit) { if (picked.has(hit) && !BASICS.includes(hit)) used.add(hit); } else missing.push(alts);
    }
    for (const o of idea.optional || []) if (picked.has(o) && !BASICS.includes(o)) used.add(o);
    if (!used.size || missing.length > 2) continue;
    out.push({ idea, used: [...used], missing, ready: missing.length === 0 });
  }
  out.sort((a, b) => b.used.length - a.used.length || a.missing.length - b.missing.length
    || a.idea.difficulty - b.idea.difficulty || a.idea.title.localeCompare(b.idea.title));
  return { ready: out.filter(r => r.ready), almost: out.filter(r => !r.ready) };
}

export function passesFilters(idea, f) {
  if (f.diff && idea.difficulty !== f.diff) return false;
  if (f.vibe && !(idea.vibes || []).includes(f.vibe)) return false;
  if (f.time === 'short' && idea.time > 30) return false;
  if (f.time === 'medium' && (idea.time <= 30 || idea.time > 60)) return false;
  if (f.time === 'long' && idea.time <= 60) return false;
  return true;
}

export function timeText(min) {
  if (min < 60) return `${min} min`;
  const h = Math.floor(min / 60), m = min % 60;
  return m ? `${h} h ${m} min` : `${h} h`;
}

// Which boards this idea's code was verified to compile for (from tools/compile-all.mjs).
export function compiledFor(idea) {
  const r = library.report?.results?.[idea.id];
  if (!r) return [];
  return Object.entries(r).filter(([, v]) => v === 'ok').map(([k]) => k);
}

export function boardNames(idea) {
  return (idea.boards || []).map(b => catalog.byId.get(b)?.name || b);
}
