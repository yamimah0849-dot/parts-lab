// Parts Lab — saved state (localStorage on this device).
const KEY = 'partslab.v1';

const DEFAULTS = {
  settings: { sound: true, reduceMotion: false, basics: true, memberName: '' },
  tray: [],         // selected part ids
  custom: [],       // user-added parts
  customIdeas: [],  // ideas imported from Claude
  projects: [],     // { id, started, done: [stepIndex], partsGot: [index], finished }
  filters: { diff: 0, vibe: '', time: '' },
};

let state = load();
const listeners = new Set();

function clone(o) { return JSON.parse(JSON.stringify(o)); }

export function normalize(s) {
  const d = clone(DEFAULTS);
  if (!s || typeof s !== 'object') return d;
  return {
    settings: { ...d.settings, ...(s.settings && typeof s.settings === 'object' ? s.settings : {}) },
    tray: Array.isArray(s.tray) ? s.tray.filter(x => typeof x === 'string') : [],
    custom: Array.isArray(s.custom) ? s.custom.filter(p => p && typeof p.id === 'string' && typeof p.name === 'string') : [],
    customIdeas: Array.isArray(s.customIdeas) ? s.customIdeas.filter(i => i && typeof i.id === 'string') : [],
    projects: Array.isArray(s.projects) ? s.projects.filter(p => p && typeof p.id === 'string').map(p => ({
      id: p.id, started: p.started || Date.now(),
      done: Array.isArray(p.done) ? p.done.filter(Number.isInteger) : [],
      partsGot: Array.isArray(p.partsGot) ? p.partsGot.filter(Number.isInteger) : [],
      finished: !!p.finished,
    })) : [],
    filters: { ...d.filters, ...(s.filters && typeof s.filters === 'object' ? s.filters : {}) },
  };
}

function load() {
  try { return normalize(JSON.parse(localStorage.getItem(KEY))); } catch { return clone(DEFAULTS); }
}

function save() {
  try { localStorage.setItem(KEY, JSON.stringify(state)); } catch { /* storage full or blocked */ }
  listeners.forEach(fn => fn(state));
}

export const store = {
  get: () => state,
  onChange: fn => listeners.add(fn),
  setSetting(k, v) { state.settings[k] = v; save(); },
  setFilter(k, v) { state.filters[k] = v; save(); },
  inTray: id => state.tray.includes(id),
  toggleTray(id) {
    state.tray = state.tray.includes(id) ? state.tray.filter(x => x !== id) : [...state.tray, id];
    save();
    return state.tray.includes(id);
  },
  addToTray(ids) { state.tray = [...new Set([...state.tray, ...ids])]; save(); },
  removeFromTray(id) { state.tray = state.tray.filter(x => x !== id); save(); },
  clearTray() { state.tray = []; save(); },
  pruneTray(validIds) { const n = state.tray.length; state.tray = state.tray.filter(id => validIds.has(id)); if (state.tray.length !== n) save(); },
  addCustom(part) { state.custom = [...state.custom, part]; save(); },
  deleteCustom(id) { state.custom = state.custom.filter(p => p.id !== id); state.tray = state.tray.filter(x => x !== id); save(); },
  clearCustom() { const ids = new Set(state.custom.map(p => p.id)); state.custom = []; state.tray = state.tray.filter(x => !ids.has(x)); save(); },
  addCustomIdeas(ideas) {
    const ids = new Set(ideas.map(i => i.id));
    state.customIdeas = [...state.customIdeas.filter(i => !ids.has(i.id)), ...ideas];
    save();
  },
  deleteCustomIdea(id) { state.customIdeas = state.customIdeas.filter(i => i.id !== id); state.projects = state.projects.filter(p => p.id !== id); save(); },
  // projects
  project: id => state.projects.find(p => p.id === id),
  startProject(id) {
    if (!state.projects.find(p => p.id === id)) state.projects = [{ id, started: Date.now(), done: [], partsGot: [], finished: false }, ...state.projects];
    save();
  },
  toggleStep(id, i) {
    const p = state.projects.find(x => x.id === id); if (!p) return;
    p.done = p.done.includes(i) ? p.done.filter(x => x !== i) : [...p.done, i];
    save();
  },
  togglePartGot(id, i) {
    const p = state.projects.find(x => x.id === id); if (!p) return;
    p.partsGot = p.partsGot.includes(i) ? p.partsGot.filter(x => x !== i) : [...p.partsGot, i];
    save();
  },
  setFinished(id, v) { const p = state.projects.find(x => x.id === id); if (p) { p.finished = v; save(); } },
  removeProject(id) { state.projects = state.projects.filter(p => p.id !== id); save(); },
  // backup
  exportData: () => ({ app: 'parts-lab', version: 1, exported: new Date().toISOString(), data: state }),
  importData(obj) {
    const data = obj && obj.app === 'parts-lab' ? obj.data : obj;
    if (!data || typeof data !== 'object' || !('settings' in data || 'tray' in data || 'projects' in data)) throw new Error('This file is not a Parts Lab backup.');
    state = normalize(data);
    save();
  },
  resetAll() { state = clone(DEFAULTS); save(); },
};
