// Parts Lab — shared UI helpers (escaping, sheets, toasts, anime effects).
import { catalog } from './data.js';
import { icon } from './icons.js';
import { sfx } from './sfx.js';
import { mascotSVG } from './mascot.js';

export const $ = (s, r = document) => r.querySelector(s);
export const $$ = (s, r = document) => [...r.querySelectorAll(s)];
export const esc = s => String(s ?? '').replace(/[&<>"']/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[c]));
export const safeColor = c => (typeof c === 'string' && /^#[0-9a-f]{3,8}$/i.test(c) ? c : null);
export const reduceMotion = () => document.documentElement.classList.contains('reduce-motion');
export const go = hash => { if (location.hash === hash) window.dispatchEvent(new HashChangeEvent('hashchange')); else location.hash = hash; };

// Shared screen state (current route, mascot instance, picker filters).
export const ui = { cat: 'all', query: '', route: '', param: '', lastAdded: null, mascot: null, lastQuip: 0 };

export function quip(text, mood) {
  if (!ui.mascot) return;
  ui.mascot.say(text, mood);
  ui.lastQuip = Date.now();
}

export const catOf = p => catalog.catById.get(p?.category) || catalog.categories[0];
export function partStyle(p) {
  const c = safeColor(catOf(p)?.color) || '#36f1cd';
  const ac = safeColor(p?.color);
  return `--c:${c};${ac ? `--ac2:${ac};` : ''}`;
}
export function partArt(p) {
  if (!p) return icon('custom');
  if (p.emoji) return `<span class="emoji">${esc(p.emoji)}</span>`;
  return icon(p.icon);
}
export function voltPill(p) {
  if (p.logic === 3.3) return '<span class="pill v33">3.3V logic</span>';
  if (p.logic === 5) return '<span class="pill v5">5V logic</span>';
  const v = String(p.voltage || '').split('·')[0].split('(')[0].trim();
  if (!v || v.length > 16) return '';
  const cls = /^3\.3V only/.test(v) || v === '3.3V' ? 'v33' : /^5V/.test(v) ? 'v5' : '';
  return `<span class="pill ${cls}">${esc(v)}</span>`;
}
export function tubes(n, digits = 3) {
  return String(Math.min(n, 10 ** digits - 1)).padStart(digits, '0').split('').map(d => `<span class="tube">${d}</span>`).join('');
}
export const stars = n => `<span class="stars" aria-label="Difficulty ${n} of 3">${'★'.repeat(n)}<i>${'★'.repeat(3 - n)}</i></span>`;
export const priceText = p => (Array.isArray(p?.price) && p.price.length === 2 ? `≈ ${Number(p.price[0])}–${Number(p.price[1])} SAR` : 'price unknown');

export function toast(msg, kind = '') {
  const root = $('#toast-root');
  const el = document.createElement('div');
  el.className = `toast ${kind}`;
  el.innerHTML = `${icon(kind === 'warn' ? 'warn' : 'sparkle')}<span>${esc(msg)}</span>`;
  root.appendChild(el);
  setTimeout(() => { el.classList.add('out'); setTimeout(() => el.remove(), 260); }, 2800);
}

export function sweep() {
  if (reduceMotion()) return;
  const s = document.createElement('div');
  s.className = 'scan-sweep';
  document.body.appendChild(s);
  setTimeout(() => s.remove(), 600);
}

// ---------- sheets (bottom sheet on phone, modal on PC)
let sheet = null;
export function openSheet(html, onMount) {
  closeSheet(true);
  const root = $('#sheet-root');
  root.innerHTML = `<div class="overlay"></div><div class="sheet" role="dialog" aria-modal="true"><div class="grab"></div>
    <button class="icon-btn close" aria-label="Close">${icon('close')}</button>${html}</div>`;
  sheet = { overlay: root.querySelector('.overlay'), el: root.querySelector('.sheet') };
  sheet.overlay.addEventListener('click', () => closeSheet());
  sheet.el.querySelector('.close').addEventListener('click', () => closeSheet());
  sfx.open();
  onMount?.(sheet.el);
}
export function closeSheet(instant = false) {
  if (!sheet) return;
  const s = sheet; sheet = null;
  if (instant || reduceMotion()) { $('#sheet-root').innerHTML = ''; return; }
  sfx.close();
  s.overlay.classList.add('closing'); s.el.classList.add('closing');
  setTimeout(() => { if (!sheet) $('#sheet-root').innerHTML = ''; }, 220);
}
document.addEventListener('keydown', e => { if (e.key === 'Escape') closeSheet(); });

// ---------- anime effects
// Full-screen cut-in: diagonal band, Ohmi's face, big title. Resolves when done.
export function cutIn(title, jp, { c1 = '#ff4fa3', c2 = '#8b6cff', mood = 'happy' } = {}) {
  if (reduceMotion()) return Promise.resolve();
  const el = document.createElement('div');
  el.className = 'cutin';
  el.style.setProperty('--c1', c1); el.style.setProperty('--c2', c2);
  el.innerHTML = `<div class="lines"></div><div class="band"></div><div class="face">${mascotSVG({ size: 'lg', portrait: true, mood })}</div>
    <div class="words"><b>${esc(title)}</b><span>${esc(jp)}</span></div>`;
  document.body.appendChild(el);
  return new Promise(r => setTimeout(() => { el.remove(); r(); }, 1000));
}

const POPS = ['ピコッ!', 'キラッ!', 'カチッ!', 'ポン!', 'ゲット!', 'よし!'];
export function mangaPop(target, text) {
  if (reduceMotion() || !target) return;
  const b = document.createElement('span');
  b.className = 'sfx-pop';
  b.textContent = text || POPS[Math.floor(Math.random() * POPS.length)];
  b.style.left = '50%'; b.style.top = '22%';
  b.style.setProperty('--r', `${Math.round(Math.random() * 20 - 10)}deg`);
  target.appendChild(b);
  setTimeout(() => b.remove(), 820);
}

export async function copyText(text) {
  try {
    await navigator.clipboard.writeText(text);
    return true;
  } catch {
    // Fallback for older browsers / non-secure contexts.
    const ta = document.createElement('textarea');
    ta.value = text; ta.setAttribute('readonly', ''); ta.style.position = 'fixed'; ta.style.opacity = '0';
    document.body.appendChild(ta); ta.select(); ta.setSelectionRange(0, text.length);
    let ok = false;
    try { ok = document.execCommand('copy'); } catch { ok = false; }
    ta.remove();
    return ok;
  }
}
