// Parts Lab — loads the parts catalog from data/*.json and merges the user's custom parts.
import { store } from './store.js';

async function fetchJSON(url) {
  const r = await fetch(url, { cache: 'no-cache' });
  if (!r.ok) throw new Error(`Could not load ${url} (${r.status})`);
  return r.json();
}

export const catalog = {
  categories: [],
  catById: new Map(),
  builtin: [],
  parts: [],
  byId: new Map(),
};

export async function loadCatalog() {
  const { categories } = await fetchJSON('data/categories.json');
  const lists = await Promise.all(categories.map(c => fetchJSON(`data/parts/${c.id}.json`)));
  catalog.categories = categories;
  catalog.catById = new Map(categories.map(c => [c.id, c]));
  catalog.builtin = lists.flat();
  rebuild();
  return catalog;
}

// Merge built-in parts with custom parts from storage.
export function rebuild() {
  const custom = store.get().custom.map(p => ({
    icon: 'custom', interfaces: [], tags: [], ...p,
    category: catalog.catById.has(p.category) ? p.category : 'components',
    custom: true,
  }));
  catalog.parts = [...catalog.builtin, ...custom];
  catalog.byId = new Map(catalog.parts.map(p => [p.id, p]));
  store.pruneTray(new Set(catalog.byId.keys()));
}

export function variantsOf(part) {
  if (!part.variantGroup) return [];
  return catalog.parts.filter(p => p.variantGroup === part.variantGroup && p.id !== part.id);
}

export function searchParts(query, catId) {
  const q = query.trim().toLowerCase();
  const words = q.split(/\s+/).filter(Boolean);
  return catalog.parts.filter(p => {
    if (catId && catId !== 'all' && p.category !== catId) return false;
    if (!words.length) return true;
    const hay = [p.name, p.desc, p.voltage, ...(p.tags || []), ...(p.interfaces || []), catalog.catById.get(p.category)?.name || '']
      .join(' ').toLowerCase();
    return words.every(w => hay.includes(w));
  });
}
