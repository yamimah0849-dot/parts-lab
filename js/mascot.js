// Parts Lab — "Ohmi", the original chibi android lab-assistant mascot (hand-drawn SVG).
// Mint hair with a lightning-bolt antenna strand, cable twin-tails ending in plugs,
// headphones, a resistor hair clip and an oversized lab coat.
import { sfx } from './sfx.js';

let uid = 0;

export function mascotSVG({ size = 'md', portrait = false, mood = 'normal' } = {}) {
  const u = `o${++uid}`; // unique gradient ids per instance
  const vb = portrait ? '24 -8 92 104' : '0 -14 140 186';
  return `
<svg class="mascot mascot-${size}${portrait ? ' portrait' : ''}" viewBox="${vb}" data-mood="${mood}" role="img" aria-label="Ohmi, the lab assistant">
  <defs>
    <linearGradient id="${u}h" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#5ff7dd"/><stop offset=".55" stop-color="#22c4ab"/><stop offset="1" stop-color="#127a6e"/></linearGradient>
    <linearGradient id="${u}hd" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#1c9a89"/><stop offset="1" stop-color="#0c4f48"/></linearGradient>
    <linearGradient id="${u}i" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#3b2a7a"/><stop offset=".45" stop-color="#7b5cff"/><stop offset="1" stop-color="#5ff7dd"/></linearGradient>
    <linearGradient id="${u}c" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#ffffff"/><stop offset="1" stop-color="#c7d3ec"/></linearGradient>
    <radialGradient id="${u}g"><stop offset="0" stop-color="#ffe14f" stop-opacity=".9"/><stop offset="1" stop-color="#ffe14f" stop-opacity="0"/></radialGradient>
  </defs>
  <g class="m-bob">
    <!-- cable twin-tails -->
    <g class="m-tail-l">
      <path d="M38 40C12 54 4 92 16 128c3 9 12 8 11-1-6-28-2-58 18-74z" fill="url(#${u}h)" stroke="#0c4f48" stroke-width="1.2"/>
      <rect x="15" y="125" width="10" height="13" rx="2" fill="#ffb547" transform="rotate(12 20 131)"/>
      <path d="M17 138l-1 6M22 139l0 6" stroke="#ffb547" stroke-width="2" stroke-linecap="round" transform="rotate(12 20 131)"/>
    </g>
    <g class="m-tail-r">
      <path d="M102 40c26 14 34 52 22 88-3 9-12 8-11-1 6-28 2-58-18-74z" fill="url(#${u}h)" stroke="#0c4f48" stroke-width="1.2"/>
      <rect x="115" y="125" width="10" height="13" rx="2" fill="#ffb547" transform="rotate(-12 120 131)"/>
      <path d="M118 138l-1 6M123 139l0 6" stroke="#ffb547" stroke-width="2" stroke-linecap="round" transform="rotate(-12 120 131)"/>
    </g>
    <!-- back hair -->
    <path d="M24 58C22 22 46 6 70 6s48 16 46 52l2 34c-8 7-18 6-22 0H44c-4 6-14 7-22 0z" fill="url(#${u}hd)"/>
    <!-- body: lab coat -->
    <g class="m-body">
      <path d="M50 100h40l16 66H34z" fill="url(#${u}c)" stroke="#9fb0d0" stroke-width="1.4"/>
      <path d="M60 100l10 22 10-22z" fill="#1b2340"/>
      <path d="M60 100l-8 30 18-8M80 100l8 30-18-8" fill="none" stroke="#9fb0d0" stroke-width="1.4" stroke-linejoin="round"/>
      <path d="M70 108l-9-5v10zM70 108l9-5v10z" fill="#ff4fa3"/><circle cx="70" cy="108" r="2.6" fill="#ff7ab8"/>
      <rect x="82" y="134" width="13" height="10" rx="2" fill="#36f1cd" opacity=".9"/>
      <path d="M84.5 137h8M84.5 140h5" stroke="#04332d" stroke-width="1.3" stroke-linecap="round"/>
      <path d="M44 140h12" stroke="#9fb0d0" stroke-width="1.4"/><path d="M47 140v-6M51 140v-4" stroke="#ff4fa3" stroke-width="2" stroke-linecap="round"/>
    </g>
    <g class="m-arm-l"><path d="M50 108c-10 10-14 26-12 40" fill="none" stroke="url(#${u}c)" stroke-width="11" stroke-linecap="round"/><circle cx="38.5" cy="150" r="4.5" fill="#ffe3d6"/></g>
    <g class="m-arm-r"><path d="M90 108c10 4 18-2 24-14" fill="none" stroke="url(#${u}c)" stroke-width="11" stroke-linecap="round"/><circle cx="116" cy="90" r="5" fill="#ffe3d6"/></g>
    <!-- neck + face -->
    <rect x="63" y="92" width="14" height="10" rx="4" fill="#f6cdbd"/>
    <path d="M36 56c0-22 15-33 34-33s34 11 34 33c0 24-14 42-34 44-20-2-34-20-34-44z" fill="#ffe9df"/>
    <path d="M40 48c8 6 52 6 60 0v8c-10 4-50 4-60 0z" fill="#f6cdbd" opacity=".6"/>
    <!-- headphone band -->
    <path d="M30 62C28 24 48 6 70 6s42 18 40 56" fill="none" stroke="#1b2340" stroke-width="5" stroke-linecap="round"/>
    <!-- blush -->
    <g class="m-blush"><ellipse cx="47" cy="80" rx="7" ry="3.6" fill="#ff7ab8" opacity=".45"/><ellipse cx="93" cy="80" rx="7" ry="3.6" fill="#ff7ab8" opacity=".45"/>
      <path d="M43 81l2-3M47 81l2-3M51 81l2-3M89 81l2-3M93 81l2-3M97 81l2-3" stroke="#ff4fa3" stroke-width="1.2" stroke-linecap="round" opacity=".7"/></g>
    <!-- EYES: normal -->
    <g class="eyes eyes-normal"><g class="m-blink">
      <ellipse cx="55" cy="68" rx="8.5" ry="11" fill="url(#${u}i)"/><ellipse cx="85" cy="68" rx="8.5" ry="11" fill="url(#${u}i)"/>
      <ellipse cx="55" cy="69" rx="4" ry="6" fill="#170f38"/><ellipse cx="85" cy="69" rx="4" ry="6" fill="#170f38"/>
      <circle cx="58.5" cy="63" r="3.4" fill="#fff"/><circle cx="88.5" cy="63" r="3.4" fill="#fff"/>
      <circle cx="52" cy="74" r="1.7" fill="#fff" opacity=".9"/><circle cx="82" cy="74" r="1.7" fill="#fff" opacity=".9"/>
      <path d="M45 61c5-6 15-7 21-1M75 60c6-6 16-5 21 1" fill="none" stroke="#1d1238" stroke-width="3.2" stroke-linecap="round"/>
      <path d="M45 61l-3-2M96 61l3-2" stroke="#1d1238" stroke-width="2.2" stroke-linecap="round"/>
    </g></g>
    <!-- EYES: happy ^ ^ -->
    <g class="eyes eyes-happy" fill="none" stroke="#1d1238" stroke-width="3.2" stroke-linecap="round"><path d="M46 70c4-8 14-8 18 0M76 70c4-8 14-8 18 0"/></g>
    <!-- EYES: wow (sparkle eyes) -->
    <g class="eyes eyes-wow">
      <ellipse cx="55" cy="68" rx="9.5" ry="12" fill="url(#${u}i)"/><ellipse cx="85" cy="68" rx="9.5" ry="12" fill="url(#${u}i)"/>
      <path d="M55 60l2 6 6 2-6 2-2 6-2-6-6-2 6-2zM85 60l2 6 6 2-6 2-2 6-2-6-6-2 6-2z" fill="#fff"/>
      <circle cx="51" cy="75" r="1.6" fill="#fff"/><circle cx="81" cy="75" r="1.6" fill="#fff"/>
      <path d="M44 59c5-6 16-7 22-1M74 58c6-6 17-5 22 1" fill="none" stroke="#1d1238" stroke-width="3.2" stroke-linecap="round"/>
    </g>
    <!-- EYES: worry (+ sweat drop) -->
    <g class="eyes eyes-worry">
      <ellipse cx="55" cy="70" rx="7" ry="9" fill="url(#${u}i)"/><ellipse cx="85" cy="70" rx="7" ry="9" fill="url(#${u}i)"/>
      <circle cx="57.5" cy="66" r="2.6" fill="#fff"/><circle cx="87.5" cy="66" r="2.6" fill="#fff"/>
      <path d="M46 64c5-4 13-4 18 0M76 64c5-4 13-4 18 0" fill="none" stroke="#1d1238" stroke-width="2.8" stroke-linecap="round"/>
      <path d="M45 52l14 4M95 52l-14 4" stroke="#0c4f48" stroke-width="2.4" stroke-linecap="round"/>
      <path class="m-sweat" d="M104 40c3 5 5 8 5 11a5 5 0 0 1-10 0c0-3 2-6 5-11z" fill="#7cf6ff" stroke="#2a7fa8" stroke-width="1"/>
    </g>
    <!-- EYES: wink -->
    <g class="eyes eyes-wink">
      <ellipse cx="55" cy="68" rx="8.5" ry="11" fill="url(#${u}i)"/><ellipse cx="55" cy="69" rx="4" ry="6" fill="#170f38"/>
      <circle cx="58.5" cy="63" r="3.4" fill="#fff"/><path d="M45 61c5-6 15-7 21-1" fill="none" stroke="#1d1238" stroke-width="3.2" stroke-linecap="round"/>
      <path d="M76 69l9-4 9 4M76 69l9 4" fill="none" stroke="#1d1238" stroke-width="3" stroke-linecap="round" stroke-linejoin="round"/>
    </g>
    <!-- mouths -->
    <path class="mouth mouth-normal" d="M66 88c2.5 2 5.5 2 8 0" fill="none" stroke="#b5475f" stroke-width="2" stroke-linecap="round"/>
    <path class="mouth mouth-happy" d="M63 86c2 8 12 8 14 0z" fill="#ff6b8b" stroke="#b5475f" stroke-width="1.2" stroke-linejoin="round"/>
    <ellipse class="mouth mouth-wow" cx="70" cy="89" rx="3.4" ry="4.2" fill="#ff6b8b" stroke="#b5475f" stroke-width="1.2"/>
    <path class="mouth mouth-worry" d="M64 90c2-2 4-2 6 0s4 2 6 0" fill="none" stroke="#b5475f" stroke-width="2" stroke-linecap="round"/>
    <!-- front hair: bangs + side locks -->
    <path d="M34 66c-3-30 13-50 36-50s39 20 36 50l-5-15-5 12-5-19-7 14-6-18-5 15-6-16-6 16-5-14-6 18-5-12-5 15z" fill="url(#${u}h)" stroke="#0c4f48" stroke-width="1.2" stroke-linejoin="round"/>
    <path d="M35 54c-5 18-4 34 4 46 1-14 2-28 6-40z" fill="url(#${u}h)" stroke="#0c4f48" stroke-width="1"/>
    <path d="M105 54c5 18 4 34-4 46-1-14-2-28-6-40z" fill="url(#${u}h)" stroke="#0c4f48" stroke-width="1"/>
    <path d="M48 30c12-8 32-8 44 0" fill="none" stroke="#fff" stroke-opacity=".55" stroke-width="3" stroke-linecap="round"/>
    <!-- eyebrows over bangs -->
    <path class="m-brows" d="M49 55c3-2 8-2 11 0M80 55c3-2 8-2 11 0" fill="none" stroke="#0c4f48" stroke-width="1.8" stroke-linecap="round" opacity=".8"/>
    <!-- resistor hair clip -->
    <g transform="rotate(-24 45 38)"><rect x="37" y="34" width="17" height="7" rx="3.5" fill="#e9d3a7" stroke="#8a6a3a" stroke-width="1"/>
      <path d="M41 34v7M45 34v7" stroke="#ff3b4e" stroke-width="1.8"/><path d="M48.5 34v7" stroke="#8a5a2b" stroke-width="1.8"/><path d="M51 34v7" stroke="#ffcf4a" stroke-width="1.6"/></g>
    <!-- headphone ear cups -->
    <circle cx="31" cy="64" r="10" fill="#1b2340" stroke="#36f1cd" stroke-width="2.5"/><circle cx="31" cy="64" r="4" fill="#36f1cd" class="m-led"/>
    <circle cx="109" cy="64" r="10" fill="#1b2340" stroke="#36f1cd" stroke-width="2.5"/><circle cx="109" cy="64" r="4" fill="#36f1cd" class="m-led"/>
    <!-- lightning-bolt antenna strand (ahoge) -->
    <g class="m-ahoge">
      <path d="M70 17l-7-14 9 3-6-13" fill="none" stroke="url(#${u}h)" stroke-width="4" stroke-linecap="round" stroke-linejoin="round"/>
      <circle cx="66" cy="-7" r="9" fill="url(#${u}g)" class="m-glow"/>
      <circle cx="66" cy="-7" r="3" fill="#ffe14f" class="m-bulb"/>
    </g>
    <!-- sparkles -->
    <g class="m-sparkles" fill="#ffe14f">
      <path d="M122 14l2 5 5 2-5 2-2 5-2-5-5-2 5-2z"/><path d="M16 22l1.6 3.6 3.6 1.6-3.6 1.6L16 32.4l-1.6-3.6-3.6-1.6 3.6-1.6z"/><path d="M126 44l1.2 2.8 2.8 1.2-2.8 1.2-1.2 2.8-1.2-2.8-2.8-1.2 2.8-1.2z" fill="#ff7ab8"/>
    </g>
  </g>
</svg>`;
}

// Mount a mascot + visual-novel dialogue box into a container element.
export function createMascot(el, { size = 'md', bubble = true, portrait = false } = {}) {
  el.classList.add('mascot-wrap', `mascot-wrap-${size}`);
  el.innerHTML = `${mascotSVG({ size, portrait })}${bubble ? '<div class="bubble" aria-live="polite"><span class="bubble-name">OHMI <i>オーミ</i></span><span class="bubble-text"></span><span class="bubble-next">▼</span></div>' : ''}`;
  const svg = el.querySelector('svg');
  const textEl = el.querySelector('.bubble-text');
  const bubbleEl = el.querySelector('.bubble');
  let typing = null;
  let moodTimer = null;
  let pokes = 0;

  function mood(m, ms = 0) {
    svg.dataset.mood = m;
    clearTimeout(moodTimer);
    if (ms) moodTimer = setTimeout(() => (svg.dataset.mood = 'normal'), ms);
  }

  // Visual-novel style: text types out with soft blips.
  function say(text, m = 'normal', { instant = false } = {}) {
    if (!textEl) return;
    clearInterval(typing);
    mood(m, m === 'normal' ? 0 : 2800);
    bubbleEl.classList.remove('pop', 'done'); void bubbleEl.offsetWidth; bubbleEl.classList.add('pop');
    const reduce = document.documentElement.classList.contains('reduce-motion');
    if (instant || reduce) { textEl.textContent = text; bubbleEl.classList.add('done'); return; }
    let i = 0;
    textEl.textContent = '';
    typing = setInterval(() => {
      i += 1;
      textEl.textContent = text.slice(0, i);
      if (i % 3 === 0 && text[i - 1] !== ' ') sfx.type();
      if (i >= text.length) { clearInterval(typing); bubbleEl.classList.add('done'); }
    }, 22);
  }

  svg.addEventListener('click', () => {
    pokes += 1;
    svg.classList.remove('poked'); void svg.getBoundingClientRect(); svg.classList.add('poked');
    const q = pokes >= 5 && pokes % 5 === 0
      ? ['M-mou! Stop poking me! …I-it’s not like I mind or anything.', 'worry']
      : POKES[Math.floor(Math.random() * POKES.length)];
    say(q[0], q[1]);
  });

  return { say, mood, el };
}

const POKES = [
  ['Hey! That tickles my capacitors!', 'happy'],
  ['Ohmi, lab assistant android, at your service! ☆', 'wink'],
  ['Fun fact: the "Ohm" is the unit of resistance. I resist nothing, except bad wiring.', 'happy'],
  ['Did you remember the resistor for your LED? …Just checking.', 'worry'],
  ['Every great gadget starts with a pile of random parts.', 'normal'],
  ['Hypothesis: you own at least one LED. Let’s test it!', 'wow'],
  ['My twin-tails are USB-compatible. Don’t ask.', 'wink'],
  ['Science is just magic with a datasheet!', 'wow'],
];
