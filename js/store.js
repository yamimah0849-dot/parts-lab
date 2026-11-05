// Parts Lab — saved state (localStorage on this device).
const KEY = 'partslab.v1';

const DEFAULTS = {
  settings: { sound: true, reduceMotion: false, basics: true, memberName: '' },
  tray: [],      // selected part ids
  custom: [],    // user-added parts
};

let state = load();
const listeners = new Set();

function load() {
  try {
    const raw = localStorage.getItem(KEY);
    if (!raw) return structuredClone(DEFAULTS);
    const s = JSON.parse(raw);
    return {
      settings: { ...DEFAULTS.settings, ...(s.settings || {}) },
      tray: Array.isArray(s.tray) ? s.tray.filter(x => typeof x === 'string') : [],
      custom: Array.isArray(s.custom) ? s.custom : [],
    };
  } catch {
    return structuredClone(DEFAULTS);
  }
}

function save() {
  try { localStorage.setItem(KEY, JSON.stringify(state)); } catch { /* storage full or blocked */ }
  listeners.forEach(fn => fn(state));
}

export const store = {
  get: () => state,
  onChange: fn => listeners.add(fn),
  setSetting(k, v) { state.settings[k] = v; save(); },
  inTray: id => state.tray.includes(id),
  toggleTray(id) {
    state.tray = state.tray.includes(id) ? state.tray.filter(x => x !== id) : [...state.tray, id];
    save();
    return state.tray.includes(id);
  },
  removeFromTray(id) { state.tray = state.tray.filter(x => x !== id); save(); },
  clearTray() { state.tray = []; save(); },
  pruneTray(validIds) { const n = state.tray.length; state.tray = state.tray.filter(id => validIds.has(id)); if (state.tray.length !== n) save(); },
  addCustom(part) { state.custom = [...state.custom, part]; save(); },
  deleteCustom(id) { state.custom = state.custom.filter(p => p.id !== id); state.tray = state.tray.filter(x => x !== id); save(); },
  clearCustom() { const ids = new Set(state.custom.map(p => p.id)); state.custom = []; state.tray = state.tray.filter(x => !ids.has(x)); save(); },
  resetAll() { state = structuredClone(DEFAULTS); save(); },
};
