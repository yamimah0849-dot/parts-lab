// Parts Lab — idea results, project guides, My Projects, upload help, Claude helpers.
import { catalog } from './data.js';
import { store } from './store.js';
import { icon } from './icons.js';
import { sfx } from './sfx.js';
import { createMascot, mascotSVG } from './mascot.js';
import { library, matchIdeas, passesFilters, VIBES, DIFFICULTY, timeText, compiledFor, cheapest, BASICS, rebuildIdeas } from './ideas.js';
import { ideasPrompt, stuckPrompt, validateClaudeIdeas } from './claude.js';
import { $, $$, esc, ui, go, quip, partStyle, partArt, stars, priceText, toast, openSheet, closeSheet, cutIn, copyText, reduceMotion } from './ui.js';

const view = () => $('#view');
const vibeOf = id => VIBES.find(v => v.id === id);
const vibePills = idea => (idea.vibes || []).map(v => vibeOf(v)).filter(Boolean).map(v => `<span class="pill vibe">${v.emoji} ${esc(v.name)}</span>`).join('');
const rarity = d => ['R', 'SR', 'SSR'][Math.max(0, Math.min(2, d - 1))];
// "Arduino Nano (ATmega328P)" -> "Nano", "Raspberry Pi Pico W" -> "Pico W"
const shortBoard = id => (catalog.byId.get(id)?.name || id).replace(/^Arduino |^Raspberry Pi /, '').replace(/\s*\(.*\)$/, '');

// ============================================================ RESULTS
export function renderIdeas() {
  const s = store.get();
  const trayParts = s.tray.map(id => catalog.byId.get(id)).filter(Boolean);
  view().innerHTML = `
    <div class="page-head"><div class="halftone"></div><div>
      <div class="kicker">Step 2 / 2 <span class="jp">アイデア発見</span></div>
      <h1>Your project ideas</h1>
      <div class="steps"><i class="on"></i><i class="on"></i></div>
    </div>
    <div class="head-actions">
      <button class="btn btn-ghost" id="edit-parts">${icon('back')}Edit parts</button>
      <button class="btn btn-pink" id="surprise">${icon('sparkle')}Surprise me</button>
    </div></div>
    <div id="ideas-mascot" class="create-mascot"></div>
    <div class="your-parts">${trayParts.map(p => `<span class="ypart" style="${partStyle(p)}">${partArt(p)}${esc(p.name)}</span>`).join('') || '<span class="muted mono">No parts picked yet.</span>'}</div>
    <div class="filters" id="filters"></div>
    <div id="idea-results"></div>`;

  ui.mascot = createMascot($('#ideas-mascot'), { size: 'sm', portrait: true });
  $('#edit-parts').addEventListener('click', () => { sfx.nav(); go('#/create'); });
  $('#surprise').addEventListener('click', () => {
    const { ready, almost } = matchIdeas(store.get().tray, store.get().settings.basics);
    const f = store.get().filters;
    let pool = ready.filter(r => passesFilters(r.idea, f));
    if (!pool.length) pool = almost.filter(r => passesFilters(r.idea, f));
    if (!pool.length) pool = [...ready, ...almost];
    if (!pool.length) { sfx.warn(); quip('Nothing to roll yet. Add a few more parts first!', 'worry'); return; }
    openGacha(pool);
  });
  $('#filters').addEventListener('click', e => {
    const b = e.target.closest('[data-f]'); if (!b) return;
    const k = b.dataset.f;
    const v = k === 'diff' ? Number(b.dataset.v) : b.dataset.v;
    store.setFilter(k, v);
    sfx.tap();
    renderFilters(); renderIdeaResults(false);
  });
  renderFilters();
  renderIdeaResults(true);
}

function renderFilters() {
  const f = store.get().filters;
  const chip = (k, v, label, on) => `<button class="chip ${on ? 'on' : ''}" data-f="${k}" data-v="${v}">${label}</button>`;
  $('#filters').innerHTML = `
    <div class="frow"><span class="flabel">Level</span><div class="chips">
      ${chip('diff', 0, 'Any', !f.diff)}${[1, 2, 3].map(d => chip('diff', d, `${'★'.repeat(d)} ${DIFFICULTY[d]}`, f.diff === d)).join('')}</div></div>
    <div class="frow"><span class="flabel">Vibe</span><div class="chips">
      ${chip('vibe', '', 'Any', !f.vibe)}${VIBES.map(v => chip('vibe', v.id, `${v.emoji} ${esc(v.name)}`, f.vibe === v.id)).join('')}</div></div>
    <div class="frow"><span class="flabel">Time</span><div class="chips">
      ${chip('time', '', 'Any', !f.time)}${chip('time', 'short', '≤ 30 min', f.time === 'short')}${chip('time', 'medium', '30–60 min', f.time === 'medium')}${chip('time', 'long', '1 h +', f.time === 'long')}</div></div>`;
}

function renderIdeaResults(talk) {
  const s = store.get();
  const { ready, almost } = matchIdeas(s.tray, s.settings.basics);
  const f = s.filters;
  const R = ready.filter(r => passesFilters(r.idea, f));
  const A = almost.filter(r => passesFilters(r.idea, f));
  const total = ready.length + almost.length;
  const filtered = (f.diff || f.vibe || f.time) && (R.length + A.length) < total;
  let i = 0;
  const section = (cls, title, jp, list, empty) => `
    <section class="isec ${cls}">
      <h2 class="isec-head">${title} <span class="jp">${jp}</span><span class="count">${list.length}</span></h2>
      ${list.length ? `<div class="igrid">${list.map(r => ideaCard(r, i++)).join('')}</div>` : `<p class="muted mono isec-empty">${empty}</p>`}
    </section>`;
  const claudeCard = `<div class="claude-card">
      <div class="cc-face">${mascotSVG({ size: 'sm', portrait: true, mood: 'wow' })}</div>
      <div><b>${total < 3 ? 'Only a few matches…' : 'Want even more ideas?'}</b>
      <p>Ask Claude for fresh ideas made for your exact parts. I'll copy a prompt for you, you paste it into your regular Claude chat, then paste the answer back here.</p>
      <button class="btn btn-primary" id="ask-claude">${icon('chat')}Get ideas from Claude</button></div></div>`;
  $('#idea-results').innerHTML = `
    ${filtered ? `<div class="filter-note mono">Filters are hiding ${total - R.length - A.length} idea(s). <button class="linkish" id="clear-filters">Clear filters</button></div>` : ''}
    ${section('ready', '⚡ Build it now', '今すぐ作れる', R, 'Nothing you can build with only these parts yet.')}
    ${section('almost', '🛒 Add 1–2 parts', 'あと少し', A, 'No near-misses either.')}
    ${claudeCard}`;
  $('#idea-results').addEventListener('click', onResultClick);
  $('#clear-filters')?.addEventListener('click', () => { ['diff', 'vibe', 'time'].forEach(k => store.setFilter(k, k === 'diff' ? 0 : '')); renderFilters(); renderIdeaResults(false); });
  $('#ask-claude').addEventListener('click', openClaudeIdeas);
  if (talk) {
    if (ready.length) quip(`I found ${ready.length} idea${ready.length > 1 ? 's' : ''} you can build right now${almost.length ? `, plus ${almost.length} that need just 1–2 more parts` : ''}! Sugoi~`, 'wow');
    else if (almost.length) quip(`Nothing buildable with only these parts, but ${almost.length} idea${almost.length > 1 ? 's are' : ' is'} sooo close. Check what's missing!`, 'happy');
    else quip('Hmm… my database has nothing for this combo yet. Let’s ask Claude for fresh ideas!', 'worry');
  }
}

function onResultClick(e) {
  const c = e.target.closest('[data-idea]');
  if (c) { sfx.nav(); go(`#/idea/${c.dataset.idea}`); }
}

function ideaCard(r, i) {
  const idea = r.idea;
  const minis = r.used.map(id => catalog.byId.get(id)).filter(Boolean).slice(0, 7)
    .map(p => `<span class="mini" style="${partStyle(p)}" title="${esc(p.name)}">${partArt(p)}</span>`).join('');
  const miss = r.missing.map(alts => {
    const p = cheapest(alts);
    const more = alts.length > 1 ? ` <span class="muted">(or ${alts.length - 1} other)</span>` : '';
    return `<div class="miss">${icon('plus')}<span>${esc(p?.name || alts[0])}${more}</span><b>${p ? priceText(p) : ''}</b></div>`;
  }).join('');
  return `<button class="icard ${r.ready ? 'ready' : 'almost'}" data-idea="${esc(idea.id)}" style="--i:${Math.min(i, 20)}">
    <span class="rarity r${idea.difficulty}">${rarity(idea.difficulty)}</span>
    <div class="icard-top"><span class="iemoji">${esc(idea.emoji || '💡')}</span>
      <div class="imeta">${stars(idea.difficulty)}<span class="mono">${icon('clock')}${timeText(idea.time)}</span></div></div>
    <b class="ititle">${esc(idea.title)}</b>
    <p class="ipitch">${esc(idea.pitch)}</p>
    <div class="iuses"><span class="mono">Uses ${r.used.length} of your part${r.used.length > 1 ? 's' : ''}</span><span class="minis">${minis}</span></div>
    ${miss ? `<div class="imissing"><span class="mono">You'd need:</span>${miss}</div>` : ''}
    <div class="itags">${vibePills(idea)}${idea.fromClaude ? '<span class="pill cus">from Claude</span>' : ''}</div>
  </button>`;
}

// ============================================================ GACHA (Surprise me)
function openGacha(pool) {
  const pick = pool[Math.floor(Math.random() * pool.length)];
  const idea = pick.idea;
  const reduce = reduceMotion();
  const el = document.createElement('div');
  el.className = `gacha ${reduce ? 'instant' : ''}`;
  el.innerHTML = `<div class="gacha-stage">
      <div class="rays"></div>
      <div class="capsule r${idea.difficulty}"><div class="cap-top"></div><div class="cap-bottom"></div></div>
      <div class="gacha-card r${idea.difficulty}">
        <span class="rarity big r${idea.difficulty}">${rarity(idea.difficulty)}</span>
        <div class="gemoji">${esc(idea.emoji || '💡')}</div>
        <b>${esc(idea.title)}</b>
        <p>${esc(idea.pitch)}</p>
        <div class="imeta">${stars(idea.difficulty)}<span class="mono">${icon('clock')}${timeText(idea.time)}</span></div>
        ${pick.ready ? '<span class="pill inc">Build it now</span>' : `<span class="pill v5">Needs ${pick.missing.length} more part${pick.missing.length > 1 ? 's' : ''}</span>`}
        <div class="gacha-actions">
          <button class="btn btn-primary" data-g="open">${icon('play')}Open guide</button>
          <button class="btn btn-ghost" data-g="again">${icon('sparkle')}Roll again</button>
        </div>
      </div>
    </div>`;
  document.body.appendChild(el);
  sfx.create();
  setTimeout(() => sfx.success(), reduce ? 0 : 1250);
  el.addEventListener('click', e => {
    const a = e.target.closest('[data-g]');
    if (a?.dataset.g === 'open') { el.remove(); go(`#/idea/${idea.id}`); return; }
    if (a?.dataset.g === 'again') { el.remove(); openGacha(pool); return; }
    if (!e.target.closest('.gacha-card')) el.remove();
  });
}

// ============================================================ IDEA GUIDE / PROJECT MODE
const codeCache = new Map();
async function loadCode(idea) {
  if (idea.code) return idea.code;
  if (codeCache.has(idea.id)) return codeCache.get(idea.id);
  const r = await fetch(`sketches/${idea.id}/${idea.id}.ino`, { cache: 'no-cache' });
  if (!r.ok) throw new Error(`code file missing (${r.status})`);
  const t = await r.text();
  codeCache.set(idea.id, t);
  return t;
}

// Tiny C++ highlighter (escapes everything it outputs).
function highlight(code) {
  const re = /(\/\/[^\n]*|\/\*[\s\S]*?\*\/)|("(?:\\.|[^"\\\n])*"|'(?:\\.|[^'\\\n])+')|(^[ \t]*#[^\n]*)|\b(void|int|long|float|double|bool|boolean|char|byte|const|unsigned|signed|if|else|for|while|do|return|true|false|static|switch|case|break|continue|default|struct|enum|class|new|delete|auto|volatile|uint8_t|uint16_t|uint32_t|int8_t|int16_t|int32_t|size_t|String)\b|\b(HIGH|LOW|INPUT|OUTPUT|INPUT_PULLUP|LED_BUILTIN|A[0-7])\b|\b(\d+(?:\.\d+)?[fFuUlL]*|0x[0-9a-fA-F]+)\b/gm;
  let out = '', last = 0, m;
  while ((m = re.exec(code))) {
    out += esc(code.slice(last, m.index));
    const cls = m[1] ? 'c' : m[2] ? 's' : m[3] ? 'p' : m[4] ? 'k' : m[5] ? 'b' : 'n';
    out += `<span class="t${cls}">${esc(m[0])}</span>`;
    last = re.lastIndex;
  }
  return out + esc(code.slice(last));
}

const wireColor = to => (/\bGND\b|−|minus/i.test(to) ? 'gnd' : /(\b5V\b|3\.3V|3V3|VCC|VIN|VBUS|\+)/i.test(to) ? 'pwr' : 'sig');

export function renderIdea(id) {
  const idea = library.byId.get(id);
  if (!idea) {
    view().innerHTML = `<div class="empty"><h2>Idea not found</h2><p class="muted">It may have been deleted.</p><button class="btn btn-primary" data-go="#/ideas">Back to ideas</button></div>`;
    view().querySelector('[data-go]').addEventListener('click', () => go('#/ideas'));
    return;
  }
  const s = store.get();
  const g = idea.guide || {};
  const steps = g.steps || [];
  const have = new Set(s.tray);
  if (s.settings.basics) BASICS.forEach(b => have.add(b));
  const compiled = compiledFor(idea);
  const boardsHave = idea.boards.filter(b => s.tray.includes(b));
  const mainBoard = catalog.byId.get(boardsHave[0] || idea.boards[0]);
  const compiledNames = compiled.map(shortBoard);
  const badge = idea.fromClaude
    ? `<div class="callout volt">${icon('warn')}<div><b>From Claude · not compile-checked</b>This idea was pasted from your Claude chat. Parts Lab checked its format and part names, but its code and wiring were not verified. Double-check before powering up.</div></div>`
    : compiled.length
      ? `<div class="callout tip">${icon('check')}<div><b>Code compile-checked ✓</b>Compiled without errors using arduino-cli for: ${esc(compiledNames.join(', '))}.</div></div>`
      : `<div class="callout info">${icon('info')}<div><b>Compile check pending</b>This idea's code hasn't been verified yet.</div></div>`;

  const partRows = (g.parts || []).map(([pid, qty, note], i) => {
    const p = catalog.byId.get(pid);
    const ok = have.has(pid);
    return `<li class="prow ${ok ? 'have' : ''}" data-pi="${i}">
      <button class="pcheck" data-pcheck="${i}" aria-label="Got it">${icon('check')}</button>
      <span class="pico" style="${partStyle(p)}">${partArt(p)}</span>
      <div class="ptxt"><b>${qty || 1}× ${esc(p?.name || pid)}</b>${note ? `<span>${esc(note)}</span>` : ''}${!ok && p?.price ? `<span class="muted mono">${priceText(p)}</span>` : ''}</div>
      ${ok ? '<span class="pill inc">in tray</span>' : ''}
      ${p ? `<button class="icon-btn pinfo" data-part="${esc(pid)}" aria-label="About ${esc(p.name)}">${icon('info')}</button>` : ''}
    </li>`;
  }).join('');

  const wiringRows = (g.wiring || []).map(([a, b, n]) => `<tr class="w-${wireColor(b)}"><td>${esc(a)}</td><td class="arrow">→</td><td><b>${esc(b)}</b>${n ? `<small>${esc(n)}</small>` : ''}</td></tr>`).join('');

  view().innerHTML = `
    <div class="idea-hero">
      <div class="halftone"></div>
      <div class="ihero-emoji">${esc(idea.emoji || '💡')}</div>
      <div class="ihero-txt">
        <div class="kicker"><span id="proj-kicker"></span> <span class="jp">作り方ガイド</span></div>
        <h1>${esc(idea.title)}</h1>
        <p class="ipitch">${esc(idea.pitch)}</p>
        <div class="itags">${stars(idea.difficulty)}<span class="pill">${icon('clock')}${timeText(idea.time)}</span>${vibePills(idea)}${idea.solder ? '<span class="pill v5">needs soldering</span>' : ''}</div>
      </div>
    </div>
    <div id="proj-progress"></div>
    <div class="board-line">${icon('cat-boards')}<span>Board: <b>${esc(idea.boards.map(shortBoard).join(' / '))}</b></span></div>
    ${badge}
    <nav class="toc">${[['about', 'About'], ['parts', 'Parts'], ['wiring', 'Wiring'], ['steps', 'Steps'], ['code', 'Code']].map(([k, l]) => `<button data-toc="${k}">${l}</button>`).join('')}</nav>

    <section class="gsec" id="sec-about"><h2>${icon('info')}What you'll build</h2><p>${esc(g.about || idea.pitch)}</p>
      ${(g.safety || []).map(t => `<div class="callout power">${icon('bolt')}<div><b>Safety</b>${esc(t)}</div></div>`).join('')}
    </section>
    <section class="gsec" id="sec-parts"><h2>${icon('grid')}Parts list <span class="muted mono" id="parts-count"></span></h2><ul class="plist">${partRows}</ul></section>
    <section class="gsec" id="sec-wiring"><h2>${icon('cat-components')}Wiring <span class="jp">配線</span></h2>
      <p class="muted">Unplug USB while wiring. Every row is one wire. <span class="wkey"><i class="w-pwr"></i>power <i class="w-gnd"></i>ground <i class="w-sig"></i>signal</span></p>
      ${wiringRows ? `<div class="wtable-wrap"><table class="wtable"><tbody>${wiringRows}</tbody></table></div>` : '<p class="muted">No wiring needed: the board alone does the job.</p>'}
    </section>
    <section class="gsec" id="sec-steps"><h2>${icon('check')}Build steps</h2><ol class="slist">${steps.map((t, i) => `<li class="srow" data-si="${i}"><button class="scheck" data-scheck="${i}" aria-label="Mark step ${i + 1} done">${icon('check')}</button><span class="snum">${i + 1}</span><p>${esc(t)}</p></li>`).join('')}</ol></section>
    <section class="gsec" id="sec-code"><h2>${icon('code')}Code <span class="jp">コード</span></h2>
      ${(idea.libs || []).length ? `<div class="callout info">${icon('download')}<div><b>Install these libraries first</b>Arduino IDE → Tools → Manage Libraries → search for: ${idea.libs.map(l => `<code>${esc(l)}</code>`).join(', ')}</div></div>` : ''}
      <div class="codebox"><div class="codebar"><span class="mono">${esc(idea.id)}.ino</span><button class="btn btn-primary btn-sm" id="copy-code">${icon('copy')}Copy code</button></div><pre id="code"><code class="mono">Loading code…</code></pre></div>
      <p class="muted">New to uploading? <button class="linkish" data-go-help>Read "How to upload code"</button></p>
    </section>
    ${g.test ? `<section class="gsec"><h2>${icon('play')}Test it</h2><p>${esc(g.test)}</p></section>` : ''}
    ${(g.next || []).length ? `<section class="gsec"><h2>${icon('sparkle')}Level up</h2><ul class="nlist">${g.next.map(n => `<li>${esc(n)}</li>`).join('')}</ul></section>` : ''}
    ${idea.fromClaude ? `<button class="btn btn-danger" id="del-idea">${icon('trash')}Delete this imported idea</button>` : ''}
    <div class="action-bar" id="action-bar"></div>`;

  let code = '';
  loadCode(idea).then(t => { code = t; $('#code').innerHTML = `<code class="mono">${highlight(t)}</code>`; })
    .catch(err => { $('#code').innerHTML = `<code class="mono">Couldn't load the code: ${esc(err.message)}</code>`; });

  const refresh = () => {
    const p = store.project(id);
    const done = new Set(p?.done || []);
    const got = new Set(p?.partsGot || []);
    $('#proj-kicker').textContent = p ? (p.finished ? 'Project complete' : 'Project in progress') : 'Project guide';
    $$('.srow').forEach(li => { const i = Number(li.dataset.si); li.classList.toggle('done', done.has(i)); });
    const cur = steps.findIndex((_, i) => !done.has(i));
    $$('.srow').forEach(li => li.classList.toggle('current', !!p && Number(li.dataset.si) === cur));
    $$('.prow').forEach(li => { const i = Number(li.dataset.pi); li.classList.toggle('got', got.has(i) || li.classList.contains('have')); });
    const pct = steps.length ? Math.round(done.size / steps.length * 100) : 0;
    $('#proj-progress').innerHTML = p ? `<div class="progress"><div class="pbar"><i style="width:${pct}%"></i></div><span class="mono">${done.size}/${steps.length} steps · ${pct}%</span></div>` : '';
    const gotCount = $$('.prow.got').length;
    $('#parts-count').textContent = `${gotCount}/${(g.parts || []).length} ready`;
    $('#action-bar').innerHTML = p
      ? `<button class="btn btn-ghost" id="stuck">${icon('chat')}Stuck? Ask Claude</button><button class="btn btn-primary" id="goto-step">${icon('play')}${p.finished ? 'Finished!' : `Go to step ${cur + 1}`}</button>`
      : `<button class="btn btn-ghost" id="stuck">${icon('chat')}Stuck? Ask Claude</button><button class="btn btn-primary" id="start">${icon('flask')}Start project</button>`;
    $('#start')?.addEventListener('click', start);
    $('#goto-step')?.addEventListener('click', () => {
      const el = $(`.srow[data-si="${Math.max(0, cur)}"]`) || $('#sec-steps');
      el.scrollIntoView({ behavior: reduceMotion() ? 'auto' : 'smooth', block: 'center' });
    });
    $('#stuck').addEventListener('click', () => openStuck(idea, Math.max(0, cur), mainBoard?.name, code));
  };

  const start = async () => {
    if (store.project(id)) return;
    store.startProject(id);
    sfx.create();
    await cutIn('PROJECT START!', 'プロジェクト開始!', { c1: '#36f1cd', c2: '#4fa8ff' });
    toast('Saved to My Projects. Tick off steps as you go!');
    refresh();
  };

  view().addEventListener('click', async e => {
    const t = e.target;
    const toc = t.closest('[data-toc]');
    if (toc) { $(`#sec-${toc.dataset.toc}`).scrollIntoView({ behavior: reduceMotion() ? 'auto' : 'smooth', block: 'start' }); return; }
    if (t.closest('[data-go-help]')) { go('#/help'); return; }
    const pinfo = t.closest('[data-part]');
    if (pinfo) { window.dispatchEvent(new CustomEvent('pl:open-part', { detail: pinfo.dataset.part })); return; }
    const sc = t.closest('[data-scheck]');
    if (sc) {
      if (!store.project(id)) { store.startProject(id); toast('Project started! Progress is saved in My Projects.'); }
      const i = Number(sc.dataset.scheck);
      store.toggleStep(id, i);
      const p = store.project(id);
      const nowDone = p.done.includes(i);
      sfx[nowDone ? 'select' : 'deselect']();
      refresh();
      if (nowDone && steps.length && p.done.length === steps.length && !p.finished) {
        store.setFinished(id, true);
        sfx.success();
        await cutIn('MISSION COMPLETE!', 'プロジェクト完成!', { c1: '#ffe14f', c2: '#ff4fa3', mood: 'wink' });
        toast('Project complete! Amazing work, Lab Member!');
        refresh();
      } else if (!nowDone && p.finished) { store.setFinished(id, false); refresh(); }
      return;
    }
    const pc = t.closest('[data-pcheck]');
    if (pc) {
      if (!store.project(id)) { store.startProject(id); toast('Project started! Progress is saved in My Projects.'); }
      store.togglePartGot(id, Number(pc.dataset.pcheck)); sfx.tap(); refresh(); return;
    }
    if (t.closest('#copy-code')) {
      if (!code) return toast('Code is still loading…', 'warn');
      const ok = await copyText(code);
      sfx[ok ? 'success' : 'warn']();
      toast(ok ? 'Code copied! Paste it into the Arduino IDE.' : 'Copy failed: press and hold the code to select it.', ok ? '' : 'warn');
      return;
    }
    if (t.closest('#del-idea')) {
      if (!confirm('Delete this imported idea (and its project progress)?')) return;
      store.deleteCustomIdea(id); rebuildIdeas(); toast('Idea deleted.'); go('#/ideas');
    }
  });
  refresh();
}

function openStuck(idea, stepIndex, boardName, code) {
  const prompt = stuckPrompt(idea, { stepIndex, boardName, code });
  openSheet(`
    <div class="kicker">Ask Claude <span class="jp">たすけて!</span></div>
    <h2>Stuck? Let's get help</h2>
    <ol class="howto"><li>Tap <b>Copy prompt</b>.</li><li>Open your regular Claude chat (claude.ai or the Claude app) and paste it.</li><li>Replace the line in [brackets] with what you see, then send.</li></ol>
    <button class="btn btn-primary btn-block" id="copy-stuck">${icon('copy')}Copy prompt</button>
    <details class="prompt-view"><summary>Show the prompt</summary><textarea readonly rows="10">${esc(prompt)}</textarea></details>`,
  el => el.querySelector('#copy-stuck').addEventListener('click', async () => {
    const ok = await copyText(prompt);
    sfx[ok ? 'success' : 'warn']();
    toast(ok ? 'Prompt copied! Paste it into your Claude chat.' : 'Copy failed: open "Show the prompt" and copy it by hand.', ok ? '' : 'warn');
  }));
}

// ============================================================ CLAUDE IDEAS
export function openClaudeIdeas() {
  const s = store.get();
  const trayParts = s.tray.map(id => catalog.byId.get(id)).filter(Boolean);
  const prompt = ideasPrompt(trayParts, s.settings.basics);
  openSheet(`
    <div class="kicker">Claude idea link <span class="jp">アイデア召喚</span></div>
    <h2>Get ideas from Claude</h2>
    <ol class="howto">
      <li>Tap <b>Copy prompt</b>. It lists your ${trayParts.length} picked part${trayParts.length === 1 ? '' : 's'} and the exact format Parts Lab understands.</li>
      <li>Paste it into your regular Claude chat and send it.</li>
      <li>Copy Claude's <b>whole answer</b>, paste it below, and tap <b>Check &amp; save</b>.</li>
    </ol>
    <button class="btn btn-primary btn-block" id="copy-ideas">${icon('copy')}Copy prompt</button>
    <details class="prompt-view"><summary>Show the prompt</summary><textarea readonly rows="8">${esc(prompt)}</textarea></details>
    <div class="field" style="margin-top:14px"><label for="claude-answer">Claude's answer</label>
      <textarea id="claude-answer" rows="7" placeholder="Paste Claude's reply here (the part with the json code block)"></textarea></div>
    <button class="btn btn-pink btn-block" id="check-ideas">${icon('check')}Check &amp; save ideas</button>
    <div id="claude-result"></div>`,
  el => {
    el.querySelector('#copy-ideas').addEventListener('click', async () => {
      const ok = await copyText(prompt);
      sfx[ok ? 'success' : 'warn']();
      toast(ok ? 'Prompt copied! Paste it into your Claude chat.' : 'Copy failed: open "Show the prompt" and copy it by hand.', ok ? '' : 'warn');
    });
    el.querySelector('#check-ideas').addEventListener('click', () => {
      const text = el.querySelector('#claude-answer').value;
      const out = el.querySelector('#claude-result');
      if (!text.trim()) { sfx.warn(); out.innerHTML = `<div class="callout volt">${icon('warn')}<div>Paste Claude's answer first.</div></div>`; return; }
      const { ideas, errors, warnings } = validateClaudeIdeas(text, new Set(library.ideas.map(i => i.id)));
      const list = (cls, ic, title, arr) => arr.length ? `<div class="callout ${cls}">${icon(ic)}<div><b>${title}</b><ul class="mini-list">${arr.map(x => `<li>${esc(x)}</li>`).join('')}</ul></div></div>` : '';
      if (ideas.length) {
        store.addCustomIdeas(ideas); rebuildIdeas(); sfx.success();
        out.innerHTML = `<div class="callout tip">${icon('check')}<div><b>Saved ${ideas.length} idea${ideas.length > 1 ? 's' : ''}!</b>${ideas.map(i => `${esc(i.emoji)} ${esc(i.title)}`).join('<br>')}<br><span class="muted">They now appear in your results with a "from Claude" badge.</span></div></div>`
          + list('volt', 'warn', 'Skipped', errors) + list('info', 'info', 'Notes', warnings)
          + '<button class="btn btn-primary btn-block" id="see-ideas">See my ideas</button>';
        out.querySelector('#see-ideas').addEventListener('click', () => { closeSheet(); if (ui.route === 'ideas') renderIdeas(); else go('#/ideas'); });
      } else {
        sfx.warn();
        out.innerHTML = list('volt', 'warn', 'Nothing saved', errors) + list('info', 'info', 'Notes', warnings);
      }
    });
  });
}

// ============================================================ MY PROJECTS
export function renderProjects() {
  const s = store.get();
  const projects = s.projects.map(p => ({ p, idea: library.byId.get(p.id) })).filter(x => x.idea);
  view().innerHTML = `
    <div class="page-head"><div class="halftone"></div><div>
      <div class="kicker">Lab notebook <span class="jp">マイプロジェクト</span></div>
      <h1>My Projects</h1></div></div>
    <div id="proj-mascot" class="create-mascot"></div>
    <div class="projlist">${projects.length ? projects.map(({ p, idea }) => {
      const total = idea.guide?.steps?.length || 0;
      const pct = total ? Math.round(p.done.length / total * 100) : 0;
      return `<div class="projcard ${p.finished ? 'finished' : ''}">
        <span class="iemoji">${esc(idea.emoji || '💡')}</span>
        <div class="projtxt"><b>${esc(idea.title)}</b>
          <span class="mono muted">${p.finished ? '🏆 Finished' : `Step ${Math.min(p.done.length + 1, total)} of ${total}`} · started ${new Date(p.started).toLocaleDateString()}</span>
          <div class="pbar"><i style="width:${pct}%"></i></div></div>
        <div class="projbtns"><button class="btn btn-primary btn-sm" data-open="${esc(idea.id)}">${p.finished ? 'View' : 'Continue'}</button>
          <button class="icon-btn" data-remove="${esc(idea.id)}" aria-label="Remove ${esc(idea.title)}">${icon('trash')}</button></div>
      </div>`;
    }).join('') : `<div class="empty"><p class="muted mono">No projects yet. Find an idea and press "Start project".</p><button class="btn btn-primary" data-go="#/create">${icon('flask')}Find a project</button></div>`}</div>`;
  ui.mascot = createMascot($('#proj-mascot'), { size: 'sm', portrait: true });
  const fin = projects.filter(x => x.p.finished).length;
  quip(projects.length
    ? `${projects.length} project${projects.length > 1 ? 's' : ''} in your notebook${fin ? `, ${fin} finished` : ''}. Keep it up, Lab Member!`
    : 'Your lab notebook is empty. Shall we find something to build?', projects.length ? 'happy' : 'normal');
  view().addEventListener('click', e => {
    const o = e.target.closest('[data-open]'); if (o) { sfx.nav(); go(`#/idea/${o.dataset.open}`); return; }
    const g = e.target.closest('[data-go]'); if (g) { sfx.nav(); go(g.dataset.go); return; }
    const r = e.target.closest('[data-remove]');
    if (r && confirm('Remove this project and its progress?')) { store.removeProject(r.dataset.remove); sfx.deselect(); renderProjects(); }
  });
}

// ============================================================ HELP: how to upload
export function renderHelp() {
  const boardNotes = catalog.parts.filter(p => p.category === 'boards' && p.note).map(p => `<li><b>${esc(p.name)}:</b> ${esc(p.note)}</li>`).join('');
  view().innerHTML = `
    <div class="page-head"><div class="halftone"></div><div>
      <div class="kicker">Lab manual <span class="jp">マニュアル</span></div>
      <h1>How to upload code</h1></div></div>
    <div class="help">
      <section class="gsec"><h2>1 · Install the Arduino IDE (free)</h2>
        <p>On your PC, download <b>Arduino IDE 2</b> from <a href="https://www.arduino.cc/en/software" target="_blank" rel="noopener">arduino.cc/en/software</a> and install it. (Code can't be uploaded from an iPhone. Use the iPhone to read guides and the PC to upload.)</p></section>
      <section class="gsec"><h2>2 · Add your board type</h2>
        <ul class="nlist">
          <li><b>Arduino Uno / Nano / Mega / Leonardo:</b> already built in.</li>
          <li><b>ESP32 boards:</b> File → Preferences → "Additional boards manager URLs" → add<br><code>https://espressif.github.io/arduino-esp32/package_esp32_index.json</code><br>then Tools → Board → Boards Manager → install <b>esp32 by Espressif</b>.</li>
          <li><b>Raspberry Pi Pico:</b> add the URL<br><code>https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json</code><br>then install <b>Raspberry Pi Pico/RP2040</b> in Boards Manager.</li>
        </ul></section>
      <section class="gsec"><h2>3 · Connect and choose</h2>
        <ol class="nlist"><li>Plug the board into your PC with a <b>data</b> USB cable.</li><li>Tools → Board → pick your exact board.</li><li>Tools → Port → pick the port that appeared when you plugged it in.</li></ol></section>
      <section class="gsec"><h2>4 · Libraries</h2>
        <p>If a guide lists libraries: Tools → Manage Libraries → search the name → Install (accept "install all" dependencies).</p></section>
      <section class="gsec"><h2>5 · Upload</h2>
        <ol class="nlist"><li>Copy the code from the guide and paste it into a new sketch (replace everything).</li><li>Press the <b>→ Upload</b> button.</li><li>Open the <b>Serial Monitor</b> (magnifier icon, top right). Set the speed shown in the code (usually 115200 or 9600).</li></ol></section>
      <section class="gsec"><h2>Board tips</h2><ul class="nlist">${boardNotes}</ul></section>
      <section class="gsec"><h2>Common problems</h2>
        <ul class="nlist">
          <li><b>No port shows up:</b> try another USB cable (charge-only cables are common), install the CH340 driver for clone boards.</li>
          <li><b>"No such file or directory … .h":</b> a library is missing. Install it from Manage Libraries.</li>
          <li><b>ESP32 "Failed to connect":</b> hold the BOOT button while the upload starts.</li>
          <li><b>Pico not detected:</b> unplug, hold BOOTSEL, plug in, release. Pick the "UF2 Board" port.</li>
          <li><b>Garbage in Serial Monitor:</b> the speed doesn't match <code>Serial.begin(...)</code>.</li>
          <li><b>Still stuck?</b> Open the project and tap <b>Stuck? Ask Claude</b>.</li>
        </ul></section>
    </div>`;
}
