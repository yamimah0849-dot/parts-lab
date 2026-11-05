// Parts Lab — main app: boot, router, screens.
import { loadCatalog, catalog, rebuild, variantsOf, searchParts } from './data.js';
import { store } from './store.js';
import { icon } from './icons.js';
import { sfx, setSound, unlockAudio } from './sfx.js';
import { createMascot } from './mascot.js';
import { labCheck } from './checks.js';

const APP_VERSION = 'phase-1';
const $ = (s, r = document) => r.querySelector(s);
const esc = s => String(s ?? '').replace(/[&<>"']/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[c]));
const view = $('#view');

const ui = { cat: 'all', query: '', route: '', lastAdded: null, mascot: null, lastQuip: 0 };

// ---------------------------------------------------------------- helpers
const safeColor = c => (typeof c === 'string' && /^#[0-9a-f]{3,8}$/i.test(c) ? c : null);
const catOf = p => catalog.catById.get(p.category) || catalog.categories[0];
function partStyle(p) {
  const c = safeColor(catOf(p)?.color) || '#36f1cd';
  const ac = safeColor(p.color);
  return `--c:${c};${ac ? `--ac2:${ac};` : ''}`;
}
function partArt(p) {
  if (p.emoji) return `<span class="emoji">${esc(p.emoji)}</span>`;
  return icon(p.icon);
}
function voltPill(p) {
  if (p.logic === 3.3) return '<span class="pill v33">3.3V logic</span>';
  if (p.logic === 5) return '<span class="pill v5">5V logic</span>';
  const v = String(p.voltage || '').split('·')[0].split('(')[0].trim();
  if (!v || v.length > 16) return '';
  const cls = /^3\.3V only/.test(v) || v === '3.3V' ? 'v33' : /^5V/.test(v) ? 'v5' : '';
  return `<span class="pill ${cls}">${esc(v)}</span>`;
}
function tubes(n, digits = 3) {
  return String(Math.min(n, 10 ** digits - 1)).padStart(digits, '0').split('').map(d => `<span class="tube">${d}</span>`).join('');
}
function trayParts() { return store.get().tray.map(id => catalog.byId.get(id)).filter(Boolean); }
function basicsOn() { return !!store.get().settings.basics; }
function reduceMotion() { return document.documentElement.classList.contains('reduce-motion'); }

function toast(msg, kind = '') {
  const root = $('#toast-root');
  const el = document.createElement('div');
  el.className = `toast ${kind}`;
  el.innerHTML = `${icon(kind === 'warn' ? 'warn' : 'sparkle')}<span>${esc(msg)}</span>`;
  root.appendChild(el);
  setTimeout(() => { el.classList.add('out'); setTimeout(() => el.remove(), 260); }, 2800);
}

function sweep() {
  if (reduceMotion()) return;
  const s = document.createElement('div');
  s.className = 'scan-sweep';
  document.body.appendChild(s);
  setTimeout(() => s.remove(), 600);
}

function applySettings() {
  const s = store.get().settings;
  setSound(s.sound);
  document.documentElement.classList.toggle('reduce-motion', !!s.reduceMotion);
  $('#sound-btn').innerHTML = icon(s.sound ? 'sound-on' : 'sound-off');
  $('#sound-btn').setAttribute('aria-pressed', String(!!s.sound));
}

// ---------------------------------------------------------------- sheets (modals)
let sheet = null;
function openSheet(html, onMount) {
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
function closeSheet(instant = false) {
  if (!sheet) return;
  const s = sheet; sheet = null;
  if (instant || reduceMotion()) { $('#sheet-root').innerHTML = ''; return; }
  sfx.close();
  s.overlay.classList.add('closing'); s.el.classList.add('closing');
  setTimeout(() => { if (!sheet) $('#sheet-root').innerHTML = ''; }, 220);
}
document.addEventListener('keydown', e => { if (e.key === 'Escape') closeSheet(); });

// ---------------------------------------------------------------- mascot lines
const CAT_QUIPS = {
  boards: p => [`${p.name}! Every gadget needs a brain.`, 'happy'],
  sensors: () => ['More senses for our creation. Excellent.', 'wow'],
  displays: () => ['A screen! Pixel art and secret messages await.', 'happy'],
  motion: () => ['Things are going to MOVE! Remember: motors never run straight from pins.', 'wow'],
  audio: () => ['Let’s make some noise!', 'happy'],
  lighting: () => ['Shiny! Don’t forget the resistor.', 'wink'],
  input: () => ['Controls acquired. Ready for player input.', 'happy'],
  comms: () => ['Wireless powers, activated!', 'wow'],
  power: () => ['Power is everything. Treat it with respect.', 'normal'],
  components: () => ['The little parts matter most.', 'normal'],
  tools: () => ['A proper scientist needs proper tools.', 'wink'],
};
function quip(text, mood) {
  if (!ui.mascot) return;
  ui.mascot.say(text, mood);
  ui.lastQuip = Date.now();
}

// ---------------------------------------------------------------- router
const routes = { '': renderHome, create: () => renderPicker('create'), catalog: () => renderPicker('catalog'), settings: renderSettings };
function route() {
  const r = location.hash.replace(/^#\/?/, '').split('/')[0];
  ui.route = routes[r] ? r : '';
  closeSheet(true);
  ui.mascot = null;
  (routes[ui.route])();
  view.classList.remove('view-enter'); void view.offsetWidth; view.classList.add('view-enter');
  window.scrollTo(0, 0);
  $('#back-btn').hidden = ui.route === '';
  sweep();
}
function go(hash) { if (location.hash === hash) route(); else location.hash = hash; }

// ---------------------------------------------------------------- HOME
function renderHome() {
  setTray(false);
  const s = store.get();
  const name = (s.settings.memberName || '').trim();
  view.innerHTML = `
  <section class="home">
    <div class="home-hero">
      <div class="kicker">Gadget R&amp;D Division <span class="jp">ラボへようこそ</span></div>
      <h1 class="title-glitch" data-text="PARTS LAB">PARTS LAB</h1>
      <div class="subtitle">pick parts → get ideas → build it <span class="jp">作ろう！</span></div>
    </div>
    <div class="home-stage">
      <div id="home-mascot"></div>
      <button class="reactor" id="create-btn" aria-label="Create: pick your parts">
        <span class="ring ticks"></span><span class="ring r1"></span><span class="ring r2"></span><span class="ring r3"></span>
        <span class="core"><span><b>CREATE</b><small>作る</small></span></span>
      </button>
    </div>
    <div class="stats">
      <div class="stat"><span class="nixie">${tubes(catalog.parts.length)}</span><label>Parts</label></div>
      <div class="stat"><span class="nixie">${tubes(s.tray.length, 2)}</span><label>In tray</label></div>
      <div class="stat"><span class="nixie">${tubes(s.custom.length, 2)}</span><label>Custom</label></div>
    </div>
    <div class="home-menu">
      <button class="tile" data-go="#/catalog">${icon('grid')}<div><b>PARTS CATALOG</b><span>Browse all ${catalog.parts.length} parts</span></div></button>
      <button class="tile" id="tile-custom">${icon('plus')}<div><b>ADD CUSTOM PART</b><span>Something missing? Add it</span></div></button>
      <button class="tile locked" id="tile-projects">${icon('flask')}<div><b>MY PROJECTS</b><span>Your builds & progress</span></div><span class="lock">PHASE 3</span></button>
      <button class="tile" data-go="#/settings">${icon('gear')}<div><b>LAB SETTINGS</b><span>Sound, name, data</span></div></button>
    </div>
    <div class="member-badge">LAB MEMBER <b>No.001</b>${name ? ` · ${esc(name)}` : ''}</div>
  </section>`;

  ui.mascot = createMascot($('#home-mascot'), { size: 'lg' });
  const greet = !name && !s.tray.length
    ? ['Welcome to the lab! I’m Ohmi, your assistant. Press CREATE and pick the parts you have.', 'happy']
    : s.tray.length
      ? [`Welcome back${name ? `, ${name}` : ''}! Your tray still holds ${s.tray.length} part${s.tray.length > 1 ? 's' : ''}. Shall we continue?`, 'wink']
      : [`Welcome back${name ? `, ${name}` : ''}! What are we building today?`, 'happy'];
  setTimeout(() => ui.mascot?.say(...greet), reduceMotion() ? 0 : 350);

  $('#create-btn').addEventListener('click', e => {
    const btn = e.currentTarget;
    sfx.create();
    if (reduceMotion()) return go('#/create');
    btn.classList.add('charging');
    const sl = document.createElement('div'); sl.className = 'speedlines'; document.body.appendChild(sl);
    setTimeout(() => { const f = document.createElement('div'); f.className = 'flash'; document.body.appendChild(f); setTimeout(() => f.remove(), 520); }, 320);
    setTimeout(() => sl.remove(), 650);
    setTimeout(() => go('#/create'), 480);
  });
  view.querySelectorAll('[data-go]').forEach(b => b.addEventListener('click', () => { sfx.nav(); go(b.dataset.go); }));
  $('#tile-custom').addEventListener('click', () => openCustomForm());
  $('#tile-projects').addEventListener('click', () => {
    sfx.warn();
    ui.mascot?.say('My Projects is still being assembled. It unlocks in Phase 3 with the full build guides!', 'worry');
  });
}

// ---------------------------------------------------------------- PICKER (create + catalog)
function renderPicker(mode) {
  const isCreate = mode === 'create';
  setTray(isCreate);
  view.innerHTML = `
    <div class="page-head"><div>
      <div class="kicker">${isCreate ? 'Step 1 / 2' : 'Parts database'} <span class="jp">${isCreate ? '部品をえらぶ' : '部品ずかん'}</span></div>
      <h1>${isCreate ? 'Pick your parts' : 'Parts catalog'}</h1>
      ${isCreate ? '<div class="steps"><i class="on"></i><i></i></div>' : ''}
    </div></div>
    <div id="picker-mascot" class="create-mascot"></div>
    <div class="searchbar">${icon('search')}
      <input id="q" type="search" placeholder="Search parts…" value="${esc(ui.query)}" autocomplete="off" autocapitalize="off" spellcheck="false" enterkeyhint="search" aria-label="Search parts">
      <button class="clear" id="q-clear" aria-label="Clear search" ${ui.query ? '' : 'hidden'}>${icon('close')}</button>
    </div>
    <div class="chips" id="chips" role="tablist" aria-label="Categories"></div>
    ${isCreate ? `<div class="toggle-row">
      <div class="txt"><b>Basics always available</b><span>Breadboard, jumper wires, resistor kit, USB cable</span></div>
      <button class="switch" id="basics-switch" role="switch" aria-checked="${basicsOn()}" aria-label="Basics always available"></button>
    </div>` : ''}
    <div id="results"></div>`;

  ui.mascot = createMascot($('#picker-mascot'), { size: 'sm' });
  const n = store.get().tray.length;
  ui.mascot.say(isCreate
    ? (n ? `Welcome back! ${n} part${n > 1 ? 's are' : ' is'} already in your tray. Add more or press Show ideas.` : 'Tap the parts you want to use. Start with a board: it’s the brain!')
    : 'This is the full parts database. Tap any part to learn what it does.', 'happy', { instant: false });

  renderChips(); renderResults();

  const q = $('#q');
  let t;
  q.addEventListener('input', () => {
    clearTimeout(t);
    t = setTimeout(() => { ui.query = q.value; $('#q-clear').hidden = !ui.query; renderResults(); }, 120);
  });
  $('#q-clear').addEventListener('click', () => { ui.query = ''; q.value = ''; $('#q-clear').hidden = true; sfx.tap(); renderResults(); q.focus(); });

  $('#chips').addEventListener('click', e => {
    const c = e.target.closest('.chip'); if (!c) return;
    ui.cat = c.dataset.cat; sfx.tap(); renderChips(); renderResults();
    c.scrollIntoView?.({ behavior: reduceMotion() ? 'auto' : 'smooth', inline: 'center', block: 'nearest' });
  });

  $('#basics-switch')?.addEventListener('click', e => {
    const on = !basicsOn();
    store.setSetting('basics', on);
    if (on) catalog.parts.filter(p => p.basic).forEach(p => store.removeFromTray(p.id));
    e.currentTarget.setAttribute('aria-checked', String(on));
    sfx[on ? 'select' : 'deselect']();
    quip(on ? 'Basics are now auto-included: breadboard, jumper wires, resistor kit and USB cable.' : 'Manual mode! Pick the basics yourself, just like the other parts.', on ? 'happy' : 'wow');
    renderResults(); renderTray();
  });

  const res = $('#results');
  res.addEventListener('click', e => {
    const info = e.target.closest('[data-info]');
    if (info) { e.stopPropagation(); return openPart(info.dataset.info); }
    if (e.target.closest('.add-card')) return openCustomForm();
    const card = e.target.closest('.card[data-id]');
    if (!card) return;
    if (ui.route === 'create') togglePart(card.dataset.id, card); else openPart(card.dataset.id);
  });
  res.addEventListener('keydown', e => {
    if ((e.key === 'Enter' || e.key === ' ') && e.target.matches('.card[data-id]')) { e.preventDefault(); e.target.click(); }
  });
}

function renderChips() {
  const tray = new Set(store.get().tray);
  const countIn = id => catalog.parts.filter(p => p.category === id && tray.has(p.id)).length;
  const all = `<button class="chip ${ui.cat === 'all' ? 'on' : ''}" data-cat="all" role="tab" aria-selected="${ui.cat === 'all'}">${icon('grid')}All <span class="muted mono">${catalog.parts.length}</span></button>`;
  $('#chips').innerHTML = all + catalog.categories.map(c => {
    const n = ui.route === 'create' ? countIn(c.id) : 0;
    return `<button class="chip ${ui.cat === c.id ? 'on' : ''}" style="--c:${safeColor(c.color)}" data-cat="${esc(c.id)}" role="tab" aria-selected="${ui.cat === c.id}">${icon(c.icon)}${esc(c.name)}${n ? `<span class="count">${n}</span>` : ''}</button>`;
  }).join('');
}

function cardHTML(p, i) {
  const isCreate = ui.route === 'create';
  const included = isCreate && basicsOn() && p.basic;
  const sel = isCreate && store.inTray(p.id);
  const pills = [
    voltPill(p),
    p.variantGroup ? '<span class="pill var">variant</span>' : '',
    p.custom ? '<span class="pill cus">custom</span>' : '',
    included ? '<span class="pill inc">included</span>' : '',
  ].join('');
  return `<div class="card ${sel ? 'sel' : ''} ${included ? 'included' : ''}" style="${partStyle(p)}--i:${Math.min(i, 24)}" data-id="${esc(p.id)}" role="button" tabindex="0" aria-pressed="${sel}" aria-label="${esc(p.name)}">
    <div class="art">${partArt(p)}</div>
    <div class="nm">${esc(p.name)}</div>
    <div class="meta">${pills}</div>
    <span class="tick">${icon('check')}</span>
    <button class="info" data-info="${esc(p.id)}" aria-label="Details: ${esc(p.name)}">${icon('info')}</button>
  </div>`;
}

function renderResults() {
  const list = searchParts(ui.query, ui.cat);
  const res = $('#results');
  if (!res) return;
  const addCard = `<button class="card add-card">${icon('plus')}<b>ADD CUSTOM PART</b><span>Missing something? Add it to the lab.</span></button>`;
  if (!list.length) {
    res.innerHTML = `<div class="empty"><div class="mono muted">No parts match “${esc(ui.query)}”${ui.cat !== 'all' ? ' in this category' : ''}.</div>
      <div class="grid" style="width:min(340px,100%)">${addCard}</div></div>`;
    if (Date.now() - ui.lastQuip > 1500) quip('Hmm, not in the database yet. You can add it as a custom part!', 'worry');
    return;
  }
  let i = 0;
  const groups = catalog.categories.map(c => ({ c, items: list.filter(p => p.category === c.id) })).filter(g => g.items.length);
  res.innerHTML = groups.map(({ c, items }) => `
    <div class="cat-head" style="--c:${safeColor(c.color)}">${icon(c.icon)}${esc(c.name)} <span class="jp">${esc(c.jp)}</span></div>
    <div class="grid">${items.map(p => cardHTML(p, i++)).join('')}</div>`).join('')
    + `<div class="cat-head" style="--c:var(--pink)">${icon('custom')}Your own parts</div><div class="grid">${addCard}</div>`;
}

function togglePart(id, cardEl) {
  const p = catalog.byId.get(id); if (!p) return;
  if (basicsOn() && p.basic) {
    sfx.tap();
    toast('Basics are included automatically. Turn off the "Basics" switch to pick them yourself.');
    return;
  }
  const before = labCheck(trayParts()).filter(c => c.kind !== 'info').length;
  const added = store.toggleTray(id);
  ui.lastAdded = added ? id : null;
  if (cardEl) {
    cardEl.classList.toggle('sel', added);
    cardEl.setAttribute('aria-pressed', String(added));
    if (added && !reduceMotion()) {
      const b = document.createElement('span');
      b.className = 'burst'; b.textContent = '+1';
      b.style.left = '50%'; b.style.top = '30%';
      cardEl.appendChild(b); setTimeout(() => b.remove(), 720);
    }
  }
  sfx[added ? 'select' : 'deselect']();
  renderChips(); renderTray();
  const after = labCheck(trayParts()).filter(c => c.kind !== 'info').length;
  if (added && after > before) {
    setTimeout(() => sfx.warn(), 180);
    quip(`Careful! ${p.name} comes with a lab check. Tap the yellow button in your tray.`, 'worry');
  } else if (added && Date.now() - ui.lastQuip > 1800) {
    const q = CAT_QUIPS[p.category]?.(p);
    if (q) quip(...q);
  }
}

// ---------------------------------------------------------------- TRAY
let trayVisible = false;
function setTray(on) { trayVisible = on; renderTray(); }

function renderTray() {
  const root = $('#tray-root');
  if (!trayVisible) { root.innerHTML = ''; view.classList.remove('has-tray'); return; }
  view.classList.add('has-tray');
  const parts = trayParts();
  const warns = labCheck(parts).filter(c => c.kind !== 'info').length;
  const tchip = p => `<div class="tchip ${p.id === ui.lastAdded ? 'new' : ''}" style="${partStyle(p)}">
      <button class="tchip-open" data-open="${esc(p.id)}" style="display:flex;align-items:center;gap:6px;min-height:44px">${partArt(p)}<span>${esc(p.name)}</span></button>
      <button class="x" data-remove="${esc(p.id)}" aria-label="Remove ${esc(p.name)}">${icon('close')}</button></div>`;
  root.innerHTML = `<div class="tray" role="region" aria-label="Your parts tray"><div class="tray-inner">
    <div class="tray-top">
      <span class="nixie sm">${tubes(parts.length, 2)}</span><span class="tray-label">Parts in tray</span>
      <span class="spacer"></span>
      ${parts.length ? `<button class="labcheck-btn ${warns ? '' : 'ok'}" id="labcheck">${icon(warns ? 'warn' : 'check')}${warns ? `${warns} lab check${warns > 1 ? 's' : ''}` : 'All clear'}</button>
      <button class="icon-btn" id="tray-clear" aria-label="Clear tray">${icon('trash')}</button>` : ''}
    </div>
    ${parts.length ? `<div class="tray-chips" id="tray-chips">${parts.map(tchip).join('')}</div>` : ''}
    <div class="tray-bottom">
      ${parts.length ? '' : '<div class="tray-empty">↑ Tap parts to load them into your tray</div>'}
      <button class="btn btn-primary" id="show-ideas" ${parts.length ? '' : 'disabled'}>${icon('sparkle')}Show ideas</button>
    </div>
  </div></div>`;

  const chips = $('#tray-chips');
  if (chips && ui.lastAdded) chips.scrollLeft = chips.scrollWidth;
  ui.lastAdded = null;
  chips?.addEventListener('click', e => {
    const rm = e.target.closest('[data-remove]');
    if (rm) {
      store.removeFromTray(rm.dataset.remove); sfx.deselect();
      const card = view.querySelector(`.card[data-id="${CSS.escape(rm.dataset.remove)}"]`);
      card?.classList.remove('sel'); card?.setAttribute('aria-pressed', 'false');
      renderChips(); renderTray(); return;
    }
    const op = e.target.closest('[data-open]');
    if (op) openPart(op.dataset.open);
  });
  $('#labcheck')?.addEventListener('click', openLabCheck);
  $('#tray-clear')?.addEventListener('click', () => {
    if (!confirm('Remove all parts from your tray?')) return;
    store.clearTray(); sfx.deselect();
    view.querySelectorAll('.card.sel').forEach(c => { c.classList.remove('sel'); c.setAttribute('aria-pressed', 'false'); });
    renderChips(); renderTray();
    quip('Tray emptied. A clean bench is a happy bench.', 'normal');
  });
  $('#show-ideas').addEventListener('click', () => {
    sfx.warn();
    quip('The Idea Engine is still being assembled (Phase 2). Your tray is saved, so nothing is lost!', 'worry');
    toast('Ideas arrive in Phase 2. Your tray is saved!', 'warn');
  });
}

function openLabCheck() {
  const parts = trayParts();
  const checks = labCheck(parts);
  const boards = parts.filter(p => p.category === 'boards');
  const rows = checks.map(c => {
    if (c.kind === 'info') return `<div class="callout info">${icon('info')}<div>${esc(c.msg)}</div></div>`;
    if (c.kind === 'power') return `<div class="callout power">${icon('bolt')}<div><b>Power · ${esc(c.part.name)}</b>${esc(c.msg)}</div></div>`;
    return `<div class="callout volt">${icon('warn')}<div><b>${c.level === 3.3 ? '3.3V' : '5V'} board · ${esc(c.part.name)}</b>${esc(c.msg)}</div></div>`;
  }).join('');
  const noBoard = !boards.length ? `<div class="callout info">${icon('info')}<div><b>No board picked yet</b>Pick a board to see 3.3V / 5V compatibility checks for your parts.</div></div>` : '';
  openSheet(`
    <div class="kicker">Lab check <span class="jp">安全チェック</span></div>
    <h2>Before you wire anything…</h2>
    <p class="muted">Ohmi checked your tray for voltage mismatches and power needs.</p>
    ${noBoard}
    ${rows || `<div class="callout tip">${icon('check')}<div><b>All clear!</b>No voltage or power problems found for these parts.</div></div>`}
    <div class="sheet-actions"><button class="btn btn-ghost btn-block" data-close>Got it</button></div>`,
  el => el.querySelector('[data-close]').addEventListener('click', () => closeSheet()));
}

// ---------------------------------------------------------------- PART DETAIL
function openPart(id) {
  const p = catalog.byId.get(id); if (!p) return;
  const c = catOf(p);
  const vars = variantsOf(p);
  const isCreate = ui.route === 'create';
  const included = basicsOn() && p.basic;
  const inTray = store.inTray(p.id);
  const callout = (cls, ic, title, txt) => `<div class="callout ${cls}">${icon(ic)}<div><b>${title}</b>${esc(txt)}</div></div>`;
  let action = '';
  if (isCreate) {
    action = included
      ? `<button class="btn btn-ghost btn-block" disabled>${icon('check')}Included automatically (basics)</button>`
      : `<button class="btn ${inTray ? 'btn-ghost' : 'btn-primary'} btn-block" data-act="toggle">${icon(inTray ? 'minus' : 'plus')}${inTray ? 'Remove from tray' : 'Add to tray'}</button>`;
  }
  if (p.custom) action += `<button class="btn btn-danger btn-block" data-act="delete">${icon('trash')}Delete custom part</button>`;

  openSheet(`
    <div class="detail-head" style="${partStyle(p)}">
      <div class="art">${partArt(p)}</div>
      <div>
        <div class="kicker" style="color:${safeColor(c.color)}">${esc(c.name)} <span class="jp">${esc(c.jp)}</span></div>
        <h2>${esc(p.name)}</h2>
        <div class="meta" style="display:flex;gap:4px;flex-wrap:wrap;margin-top:6px">${voltPill(p)}${p.custom ? '<span class="pill cus">custom</span>' : ''}${p.basic ? '<span class="pill inc">basic</span>' : ''}</div>
      </div>
    </div>
    <div class="volt-row">${icon('bolt')}<div><div class="muted mono" style="font-size:11px;letter-spacing:.18em">OPERATING VOLTAGE</div><b>${esc(p.voltage || 'Not specified')}</b></div></div>
    <div class="section"><h3>What it does</h3><p>${esc(p.desc || 'No description yet.')}</p></div>
    ${p.interfaces?.length ? `<div class="section"><h3>Talks using</h3><div style="display:flex;gap:6px;flex-wrap:wrap">${p.interfaces.map(x => `<span class="pill">${esc(x)}</span>`).join('')}</div></div>` : ''}
    ${p.variantNote ? callout('info', 'flip', 'This exact version', p.variantNote) : ''}
    ${p.note ? callout('tip', 'info', 'Lab tip', p.note) : ''}
    ${p.power ? callout('power', 'bolt', 'Power warning', p.power) : ''}
    ${p.warn?.['3.3'] ? callout('volt', 'warn', 'With a 3.3V board (ESP32, Pico)', p.warn['3.3']) : ''}
    ${p.warn?.['5'] ? callout('volt', 'warn', 'With a 5V board (Uno, Nano, Mega)', p.warn['5']) : ''}
    ${vars.length ? `<div class="section"><h3>Other versions · they wire differently</h3><div class="variant-list">${vars.map(v => `<button class="variant" data-open="${esc(v.id)}">${icon(v.icon)}<div><b>${esc(v.name)}</b><span>${esc(v.variantNote || '')}</span></div></button>`).join('')}</div></div>` : ''}
    ${Array.isArray(p.price) && p.price.length === 2 ? `<div class="section"><h3>Rough price</h3><p class="mono">≈ ${Number(p.price[0])}–${Number(p.price[1])} SAR <span class="muted">(approx., online)</span></p></div>` : ''}
    ${p.fqbn ? `<div class="section"><h3>Board ID for the compiler</h3><p class="mono muted" style="word-break:break-all">${esc(p.fqbn)}</p></div>` : ''}
    <div class="sheet-actions">${action}</div>`,
  el => {
    el.querySelectorAll('[data-open]').forEach(b => b.addEventListener('click', () => openPart(b.dataset.open)));
    el.querySelector('[data-act="toggle"]')?.addEventListener('click', () => {
      const card = view.querySelector(`.card[data-id="${CSS.escape(p.id)}"]`);
      closeSheet();
      togglePart(p.id, card);
    });
    el.querySelector('[data-act="delete"]')?.addEventListener('click', () => {
      if (!confirm(`Delete "${p.name}" from your custom parts?`)) return;
      store.deleteCustom(p.id); rebuild(); closeSheet(); sfx.deselect();
      toast('Custom part deleted.');
      refreshCurrent();
    });
  });
}

// ---------------------------------------------------------------- CUSTOM PART FORM
const EMOJIS = ['🔧', '⚙️', '🔌', '💡', '🔋', '📡', '🎛️', '🧲', '🌡️', '📟', '🔊', '🤖', '🧪', '💾', '🕹️', '⭐'];
function openCustomForm() {
  let emoji = EMOJIS[0];
  const defaultCat = ui.cat !== 'all' ? ui.cat : 'components';
  openSheet(`
    <div class="kicker">New part <span class="jp">新しい部品</span></div>
    <h2>Add a custom part</h2>
    <p class="muted">It's saved on this device and works like any other part.</p>
    <form class="form" id="custom-form" novalidate>
      <div class="field"><label for="cp-name">Name *</label><input id="cp-name" maxlength="60" required placeholder="e.g. HX711 Load Cell Amplifier"></div>
      <div class="field"><label for="cp-cat">Category</label><select id="cp-cat">${catalog.categories.map(c => `<option value="${esc(c.id)}" ${c.id === defaultCat ? 'selected' : ''}>${esc(c.name)}</option>`).join('')}</select></div>
      <div class="field"><label>Icon</label><div class="emoji-pick" id="cp-emoji">${EMOJIS.map((e, i) => `<button type="button" data-e="${e}" class="${i ? '' : 'on'}" aria-label="${e}">${e}</button>`).join('')}</div></div>
      <div class="field"><label for="cp-volt">Operating voltage</label><input id="cp-volt" maxlength="40" placeholder="e.g. 5V or 3.3–5V"></div>
      <div class="field"><label for="cp-desc">What does it do?</label><textarea id="cp-desc" maxlength="240" placeholder="One or two sentences."></textarea></div>
      <div class="field"><label>Rough price (SAR, optional)</label><div class="field-row"><input id="cp-p1" type="number" inputmode="decimal" min="0" placeholder="from"><input id="cp-p2" type="number" inputmode="decimal" min="0" placeholder="to"></div></div>
      <button class="btn btn-pink btn-block" type="submit">${icon('plus')}Register part</button>
    </form>`,
  el => {
    el.querySelector('#cp-emoji').addEventListener('click', e => {
      const b = e.target.closest('[data-e]'); if (!b) return;
      emoji = b.dataset.e; sfx.tap();
      el.querySelectorAll('#cp-emoji button').forEach(x => x.classList.toggle('on', x === b));
    });
    el.querySelector('#custom-form').addEventListener('submit', e => {
      e.preventDefault();
      const name = el.querySelector('#cp-name').value.trim();
      if (!name) { sfx.warn(); el.querySelector('#cp-name').focus(); toast('Give your part a name first.', 'warn'); return; }
      const p1 = parseFloat(el.querySelector('#cp-p1').value), p2 = parseFloat(el.querySelector('#cp-p2').value);
      const part = {
        id: `custom-${Date.now().toString(36)}`,
        name,
        category: el.querySelector('#cp-cat').value,
        emoji,
        voltage: el.querySelector('#cp-volt').value.trim(),
        desc: el.querySelector('#cp-desc').value.trim(),
        tags: [],
        interfaces: [],
      };
      if (!isNaN(p1)) part.price = [p1, isNaN(p2) ? p1 : Math.max(p1, p2)];
      store.addCustom(part);
      rebuild();
      if (ui.route === 'create') { store.toggleTray(part.id); ui.lastAdded = part.id; }
      closeSheet(); sfx.success();
      toast(`"${name}" registered in the lab database!`);
      refreshCurrent();
      setTimeout(() => quip(`New part registered: ${name}. The database grows!`, 'happy'), 60);
    });
    setTimeout(() => el.querySelector('#cp-name').focus(), 350);
  });
}

function refreshCurrent() {
  if (ui.route === 'create' || ui.route === 'catalog') { renderChips(); renderResults(); renderTray(); }
  else if (ui.route === '') renderHome();
}

// ---------------------------------------------------------------- SETTINGS
function renderSettings() {
  setTray(false);
  const s = store.get();
  const sw = (id, on, title, sub) => `<div class="toggle-row"><div class="txt"><b>${title}</b><span>${sub}</span></div><button class="switch" id="${id}" role="switch" aria-checked="${!!on}" aria-label="${title}"></button></div>`;
  view.innerHTML = `
    <div class="page-head"><div><div class="kicker">Configuration <span class="jp">設定</span></div><h1>Lab settings</h1></div></div>
    <div class="settings">
      <div class="panel"><h2>LAB MEMBER</h2>
        <div class="field"><label for="member-name">Your name</label><input id="member-name" maxlength="30" placeholder="Shown on your Lab Member badge" value="${esc(s.settings.memberName)}"></div>
      </div>
      <div class="panel"><h2>EXPERIENCE</h2>
        ${sw('sw-sound', s.settings.sound, 'Sound effects', 'Beeps, blips and the reactor sound')}
        ${sw('sw-motion', s.settings.reduceMotion, 'Reduce animations', 'Calmer screens, no boot sequence')}
        <button class="btn btn-ghost" id="test-sound">${icon('sound-on')}Test sound</button>
      </div>
      <div class="panel"><h2>DATA ON THIS DEVICE</h2>
        <p class="muted" style="margin-top:0">Your tray and custom parts are saved in this browser only. Backup export/import arrives in Phase 3.</p>
        <button class="btn btn-ghost" id="clear-tray">${icon('trash')}Clear tray (${s.tray.length})</button>
        <button class="btn btn-ghost" id="clear-custom">${icon('trash')}Delete custom parts (${s.custom.length})</button>
        <button class="btn btn-danger" id="reset-all">${icon('warn')}Reset everything</button>
      </div>
      <div class="panel"><h2>ABOUT</h2>
        <p class="mono muted" style="margin:0;line-height:1.8">PARTS LAB · build ${APP_VERSION}<br>${catalog.builtin.length} built-in parts · ${catalog.categories.length} categories<br>Prices: rough online estimates in SAR (approx.)<br>Fonts: Orbitron &amp; Share Tech Mono (SIL Open Font License)<br>Mascot, icons &amp; sounds: original, made for this app</p>
      </div>
    </div>`;

  let t;
  $('#member-name').addEventListener('input', e => {
    clearTimeout(t);
    const v = e.target.value.trim();
    t = setTimeout(() => {
      store.setSetting('memberName', v);
      if (/okabe|kyouma|kyoma|hououin|hooin/i.test(v)) { sfx.success(); toast('…Lab Member 001?! The Organization must not find out about this lab.'); }
    }, 400);
  });
  $('#sw-sound').addEventListener('click', e => {
    const on = !store.get().settings.sound; store.setSetting('sound', on); applySettings();
    e.currentTarget.setAttribute('aria-checked', String(on)); if (on) sfx.select();
  });
  $('#sw-motion').addEventListener('click', e => {
    const on = !store.get().settings.reduceMotion; store.setSetting('reduceMotion', on); applySettings();
    e.currentTarget.setAttribute('aria-checked', String(on)); sfx.tap();
  });
  $('#test-sound').addEventListener('click', () => { if (!store.get().settings.sound) toast('Sound effects are off.', 'warn'); sfx.success(); });
  $('#clear-tray').addEventListener('click', () => { store.clearTray(); sfx.deselect(); toast('Tray cleared.'); renderSettings(); });
  $('#clear-custom').addEventListener('click', () => {
    if (!store.get().custom.length) return toast('No custom parts to delete.');
    if (!confirm('Delete ALL your custom parts? This cannot be undone.')) return;
    store.clearCustom(); rebuild(); sfx.deselect(); toast('Custom parts deleted.'); renderSettings();
  });
  $('#reset-all').addEventListener('click', () => {
    if (!confirm('Reset EVERYTHING (tray, custom parts, settings)? This cannot be undone.')) return;
    store.resetAll(); rebuild(); applySettings(); sfx.deselect(); toast('Lab reset to factory settings.'); renderSettings();
  });
}

// ---------------------------------------------------------------- BOOT
async function boot() {
  applySettings();
  unlockAudio();
  $('#back-btn').innerHTML = icon('back');
  $('#settings-btn').innerHTML = icon('gear');
  $('.brand').insertAdjacentHTML('afterbegin', icon('flask'));
  $('#back-btn').addEventListener('click', () => { sfx.nav(); go('#/'); });
  $('#settings-btn').addEventListener('click', () => { sfx.nav(); go('#/settings'); });
  $('#sound-btn').addEventListener('click', () => {
    const on = !store.get().settings.sound; store.setSetting('sound', on); applySettings();
    if (on) sfx.select();
    toast(on ? 'Sound effects on' : 'Sound effects off');
  });

  // floating sparks in the background
  const bg = $('#bg');
  for (let i = 0; i < 14; i++) {
    const s = document.createElement('span');
    s.className = 'spark';
    s.style.left = `${Math.random() * 100}%`;
    s.style.bottom = `${-10 - Math.random() * 20}px`;
    s.style.animationDuration = `${9 + Math.random() * 12}s`;
    s.style.animationDelay = `${-Math.random() * 18}s`;
    bg.appendChild(s);
  }

  const bootEl = $('#boot');
  const term = $('#boot-term');
  const skipBoot = sessionStorage.getItem('pl-booted') || store.get().settings.reduceMotion || matchMedia('(prefers-reduced-motion: reduce)').matches;
  let skipped = false;
  bootEl.addEventListener('click', () => { skipped = true; });
  const wait = ms => new Promise(r => setTimeout(r, skipped ? 0 : ms));

  const loading = loadCatalog();
  try {
    if (!skipBoot) {
      const lines = [
        '<span class="pk">PARTS LAB OS</span> v1.0 // ラボ起動',
        '&gt; powering up the lab bench… <span class="ok">OK</span>',
        null, // filled after catalog loads
        '&gt; waking lab assistant unit OHMI… <span class="ok">OK</span>',
        '&gt; checking resistor supply… <span class="am">plenty</span>',
      ];
      for (let i = 0; i < lines.length; i++) {
        if (i === 2) { await loading; lines[2] = `&gt; mounting parts database… <span class="ok">${catalog.parts.length} parts OK</span>`; }
        term.insertAdjacentHTML('beforeend', `<div>${lines[i]}</div>`);
        if (i === 0) sfx.boot();
        await wait(170);
      }
      const name = (store.get().settings.memberName || '').trim();
      term.insertAdjacentHTML('beforeend', `<div style="margin-top:8px">WELCOME, <span class="am">LAB MEMBER 001</span>${name ? ` (${esc(name)})` : ''}</div>`);
      await wait(380);
    }
    await loading;
    sessionStorage.setItem('pl-booted', '1');
    bootEl.classList.add('done');
    setTimeout(() => bootEl.remove(), 460);
    window.addEventListener('hashchange', route);
    route();
  } catch (err) {
    console.error(err);
    term.innerHTML = `<div class="boot-error">ERROR: could not load the parts database.</div><div class="muted">${esc(err.message)}</div><div style="margin-top:10px">Check your internet connection and reload the page.</div>`;
  }
}

boot();

if ('serviceWorker' in navigator && location.protocol === 'https:') {
  navigator.serviceWorker.register('sw.js').catch(err => console.warn('Service worker failed:', err));
}
