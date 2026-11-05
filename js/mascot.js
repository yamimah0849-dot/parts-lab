// Parts Lab — "Ohmi", the original lab-assistant robot mascot (hand-drawn SVG).
import { sfx } from './sfx.js';

export function mascotSVG(size = 'md') {
  return `
<svg class="mascot mascot-${size}" viewBox="0 0 120 140" data-mood="normal" role="img" aria-label="Ohmi, the lab assistant robot">
  <defs>
    <radialGradient id="ohmi-glow" cx="50%" cy="50%" r="50%">
      <stop offset="0" stop-color="#36f1cd" stop-opacity=".55"/><stop offset="1" stop-color="#36f1cd" stop-opacity="0"/>
    </radialGradient>
    <linearGradient id="ohmi-head" x1="0" y1="0" x2="0" y2="1">
      <stop offset="0" stop-color="#2a3554"/><stop offset="1" stop-color="#151c30"/>
    </linearGradient>
    <linearGradient id="ohmi-coat" x1="0" y1="0" x2="0" y2="1">
      <stop offset="0" stop-color="#f4f8ff"/><stop offset="1" stop-color="#c9d4ea"/>
    </linearGradient>
  </defs>
  <g class="m-bob">
    <!-- body / lab coat -->
    <g class="m-body">
      <path d="M36 92h48l12 42H24z" fill="url(#ohmi-coat)" stroke="#9fb0d0" stroke-width="1.5"/>
      <path d="M50 92l10 16 10-16z" fill="#1b2340"/>
      <path d="M50 92l-6 22 16-6M70 92l6 22-16-6" fill="none" stroke="#9fb0d0" stroke-width="1.5"/>
      <rect x="30" y="112" width="12" height="9" rx="2" fill="none" stroke="#9fb0d0" stroke-width="1.5"/>
      <path d="M33 112v-6M37 112v-4" stroke="#ff4fa3" stroke-width="2" stroke-linecap="round"/>
      <circle cx="80" cy="116" r="4" fill="#36f1cd" class="m-badge"/>
    </g>
    <g class="m-arm-l"><path d="M34 98c-10 6-14 14-12 22" fill="none" stroke="#c9d4ea" stroke-width="7" stroke-linecap="round"/></g>
    <g class="m-arm-r"><path d="M86 98c10 4 16 0 20-8" fill="none" stroke="#c9d4ea" stroke-width="7" stroke-linecap="round"/><circle cx="107" cy="88" r="5" fill="#c9d4ea"/></g>
    <!-- antenna -->
    <path d="M60 22V9" stroke="#9fb0d0" stroke-width="3" stroke-linecap="round"/>
    <circle cx="60" cy="7" r="12" fill="url(#ohmi-glow)" class="m-glow"/>
    <circle cx="60" cy="7" r="5" fill="#36f1cd" class="m-bulb"/>
    <!-- ears -->
    <rect x="7" y="44" width="13" height="26" rx="6" fill="#1b2340" stroke="#36f1cd" stroke-width="2"/>
    <rect x="100" y="44" width="13" height="26" rx="6" fill="#1b2340" stroke="#36f1cd" stroke-width="2"/>
    <path d="M13 50v14M106 50v14" stroke="#36f1cd" stroke-width="2" stroke-linecap="round" opacity=".6"/>
    <!-- head -->
    <rect x="17" y="21" width="86" height="70" rx="28" fill="url(#ohmi-head)" stroke="#36f1cd" stroke-width="2.5"/>
    <rect x="27" y="32" width="66" height="48" rx="19" fill="#050812"/>
    <path d="M33 38c6-4 14-5 22-5" stroke="#fff" stroke-opacity=".18" stroke-width="3" fill="none" stroke-linecap="round"/>
    <!-- blush -->
    <ellipse cx="38" cy="68" rx="6" ry="3.2" fill="#ff4fa3" opacity=".55"/>
    <ellipse cx="82" cy="68" rx="6" ry="3.2" fill="#ff4fa3" opacity=".55"/>
    <!-- eyes: normal -->
    <g class="eyes eyes-normal">
      <g class="m-blink">
        <ellipse cx="46" cy="55" rx="8" ry="11" fill="#36f1cd"/>
        <ellipse cx="74" cy="55" rx="8" ry="11" fill="#36f1cd"/>
        <ellipse cx="46" cy="58" rx="5" ry="7" fill="#0b7d6e"/>
        <ellipse cx="74" cy="58" rx="5" ry="7" fill="#0b7d6e"/>
        <circle cx="49" cy="50" r="3.2" fill="#fff"/><circle cx="77" cy="50" r="3.2" fill="#fff"/>
        <circle cx="43.5" cy="60" r="1.6" fill="#fff"/><circle cx="71.5" cy="60" r="1.6" fill="#fff"/>
      </g>
    </g>
    <!-- eyes: happy ^ ^ -->
    <g class="eyes eyes-happy" fill="none" stroke="#36f1cd" stroke-width="4" stroke-linecap="round">
      <path d="M38 58c3-7 13-7 16 0M66 58c3-7 13-7 16 0"/>
    </g>
    <!-- eyes: surprised -->
    <g class="eyes eyes-wow">
      <circle cx="46" cy="55" r="10" fill="none" stroke="#36f1cd" stroke-width="3"/>
      <circle cx="74" cy="55" r="10" fill="none" stroke="#36f1cd" stroke-width="3"/>
      <circle cx="46" cy="55" r="3.5" fill="#36f1cd"/><circle cx="74" cy="55" r="3.5" fill="#36f1cd"/>
    </g>
    <!-- eyes: worried (warnings) -->
    <g class="eyes eyes-worry">
      <ellipse cx="46" cy="57" rx="7" ry="8" fill="#ffb547"/><ellipse cx="74" cy="57" rx="7" ry="8" fill="#ffb547"/>
      <circle cx="48" cy="54" r="2.5" fill="#fff"/><circle cx="76" cy="54" r="2.5" fill="#fff"/>
      <path d="M37 44l13 4M83 44l-13 4" stroke="#ffb547" stroke-width="3" stroke-linecap="round"/>
    </g>
    <!-- eyes: wink -->
    <g class="eyes eyes-wink">
      <ellipse cx="46" cy="55" rx="8" ry="11" fill="#36f1cd"/><circle cx="49" cy="50" r="3.2" fill="#fff"/>
      <path d="M66 56c3 4 13 4 16 0" fill="none" stroke="#36f1cd" stroke-width="4" stroke-linecap="round"/>
    </g>
    <!-- mouths -->
    <path class="mouth mouth-normal" d="M55 71c3 3 7 3 10 0" fill="none" stroke="#36f1cd" stroke-width="2.5" stroke-linecap="round"/>
    <path class="mouth mouth-happy" d="M53 69c2 7 12 7 14 0z" fill="#ff4fa3" stroke="#36f1cd" stroke-width="2" stroke-linejoin="round"/>
    <ellipse class="mouth mouth-wow" cx="60" cy="72" rx="3.5" ry="4.5" fill="#ff4fa3"/>
    <path class="mouth mouth-worry" d="M54 73c3-3 9-3 12 0" fill="none" stroke="#ffb547" stroke-width="2.5" stroke-linecap="round"/>
    <!-- sparkles -->
    <g class="m-sparkles" fill="#ffe14f">
      <path d="M100 18l2 5 5 2-5 2-2 5-2-5-5-2 5-2z"/><path d="M14 22l1.5 3.5 3.5 1.5-3.5 1.5L14 32l-1.5-3.5L9 27l3.5-1.5z"/>
    </g>
  </g>
</svg>`;
}

// Mount a mascot + speech bubble into a container element.
export function createMascot(el, { size = 'md', bubble = true } = {}) {
  el.classList.add('mascot-wrap', `mascot-wrap-${size}`);
  el.innerHTML = `${mascotSVG(size)}${bubble ? '<div class="bubble" aria-live="polite"><span class="bubble-text"></span></div>' : ''}`;
  const svg = el.querySelector('svg');
  const textEl = el.querySelector('.bubble-text');
  const bubbleEl = el.querySelector('.bubble');
  let typing = null;
  let moodTimer = null;

  function mood(m, ms = 0) {
    svg.dataset.mood = m;
    clearTimeout(moodTimer);
    if (ms) moodTimer = setTimeout(() => (svg.dataset.mood = 'normal'), ms);
  }

  // Visual-novel style: text types out with soft blips.
  function say(text, m = 'normal', { instant = false } = {}) {
    if (!textEl) return;
    clearInterval(typing);
    mood(m, m === 'normal' ? 0 : 2600);
    bubbleEl.classList.remove('pop'); void bubbleEl.offsetWidth; bubbleEl.classList.add('pop');
    const reduce = document.documentElement.classList.contains('reduce-motion');
    if (instant || reduce) { textEl.textContent = text; return; }
    let i = 0;
    textEl.textContent = '';
    typing = setInterval(() => {
      i += 1;
      textEl.textContent = text.slice(0, i);
      if (i % 3 === 0 && text[i - 1] !== ' ') sfx.type();
      if (i >= text.length) clearInterval(typing);
    }, 22);
  }

  svg.addEventListener('click', () => {
    const quips = POKES[Math.floor(Math.random() * POKES.length)];
    say(quips[0], quips[1]);
  });

  return { say, mood, el };
}

const POKES = [
  ['Hey! That tickles my capacitors.', 'happy'],
  ['Ohmi, lab assistant unit, at your service!', 'wink'],
  ['Fun fact: "Ohm" is the unit of resistance. I resist nothing, except bad wiring.', 'happy'],
  ['Did you remember the resistor for your LED? …Just checking.', 'worry'],
  ['Every great gadget starts with a pile of random parts.', 'normal'],
  ['Hypothesis: you own at least one LED. Let’s test it!', 'wow'],
];
