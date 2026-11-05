// Parts Lab — synthesized sound effects (Web Audio API, no audio files).
let ctx = null;
let master = null;
let enabled = true;
let unlocked = false; // browsers block audio until the first tap

function ensure() {
  if (!unlocked) return null;
  if (!ctx) {
    const AC = window.AudioContext || window.webkitAudioContext;
    if (!AC) return null;
    ctx = new AC();
    master = ctx.createGain();
    master.gain.value = 0.22;
    master.connect(ctx.destination);
  }
  if (ctx.state === 'suspended') ctx.resume();
  return ctx;
}

// iOS only allows audio after a user gesture: unlock on the first touch/click.
// Safari counts touchend/click (not always pointerdown) as a gesture, so listen to all of them
// until the audio engine is actually running.
export function unlockAudio() {
  const events = ['pointerdown', 'touchend', 'click', 'keydown'];
  const unlock = () => {
    unlocked = true;
    const c = ensure();
    if (c && c.state === 'running') events.forEach(e => window.removeEventListener(e, unlock, true));
  };
  events.forEach(e => window.addEventListener(e, unlock, true));
}

export function setSound(on) { enabled = !!on; }
export function soundOn() { return enabled; }

function tone({ f = 440, f2 = null, type = 'square', dur = 0.08, vol = 0.5, delay = 0, attack = 0.005, filter = null }) {
  const c = ensure();
  if (!c || !enabled) return;
  const t = c.currentTime + delay;
  const o = c.createOscillator();
  const g = c.createGain();
  o.type = type;
  o.frequency.setValueAtTime(f, t);
  if (f2) o.frequency.exponentialRampToValueAtTime(f2, t + dur);
  g.gain.setValueAtTime(0.0001, t);
  g.gain.exponentialRampToValueAtTime(vol, t + attack);
  g.gain.exponentialRampToValueAtTime(0.0001, t + dur);
  let node = o;
  if (filter) {
    const bq = c.createBiquadFilter();
    bq.type = 'lowpass';
    bq.frequency.setValueAtTime(filter[0], t);
    bq.frequency.exponentialRampToValueAtTime(filter[1], t + dur);
    o.connect(bq); node = bq;
  }
  node.connect(g); g.connect(master);
  o.start(t); o.stop(t + dur + 0.02);
}

function noise({ dur = 0.15, vol = 0.2, delay = 0, hp = 2000 }) {
  const c = ensure();
  if (!c || !enabled) return;
  const t = c.currentTime + delay;
  const buf = c.createBuffer(1, Math.floor(c.sampleRate * dur), c.sampleRate);
  const d = buf.getChannelData(0);
  for (let i = 0; i < d.length; i++) d[i] = (Math.random() * 2 - 1) * (1 - i / d.length);
  const src = c.createBufferSource();
  src.buffer = buf;
  const f = c.createBiquadFilter(); f.type = 'highpass'; f.frequency.value = hp;
  const g = c.createGain(); g.gain.value = vol;
  src.connect(f); f.connect(g); g.connect(master);
  src.start(t);
}

export const sfx = {
  tap()      { tone({ f: 1250, type: 'square', dur: 0.035, vol: 0.18 }); },
  type()     { tone({ f: 1800 + Math.random() * 400, type: 'square', dur: 0.018, vol: 0.06 }); },
  select()   { tone({ f: 660, type: 'triangle', dur: 0.07, vol: 0.5 }); tone({ f: 990, type: 'triangle', dur: 0.1, vol: 0.5, delay: 0.06 }); tone({ f: 1980, type: 'sine', dur: 0.12, vol: 0.12, delay: 0.06 }); },
  deselect() { tone({ f: 740, type: 'triangle', dur: 0.06, vol: 0.4 }); tone({ f: 440, type: 'triangle', dur: 0.1, vol: 0.4, delay: 0.05 }); },
  open()     { tone({ f: 320, f2: 960, type: 'sine', dur: 0.16, vol: 0.35 }); },
  close()    { tone({ f: 800, f2: 300, type: 'sine', dur: 0.14, vol: 0.3 }); },
  nav()      { tone({ f: 520, f2: 1040, type: 'sawtooth', dur: 0.09, vol: 0.12, filter: [800, 4000] }); noise({ dur: 0.06, vol: 0.05, hp: 5000 }); },
  warn()     { tone({ f: 196, type: 'square', dur: 0.09, vol: 0.25 }); tone({ f: 196, type: 'square', dur: 0.12, vol: 0.25, delay: 0.13 }); },
  success()  { [523, 659, 784, 1047].forEach((f, i) => tone({ f, type: 'triangle', dur: 0.14, vol: 0.4, delay: i * 0.07 })); },
  create() {
    tone({ f: 110, f2: 880, type: 'sawtooth', dur: 0.55, vol: 0.28, filter: [300, 6000], attack: 0.05 });
    noise({ dur: 0.5, vol: 0.06, hp: 3000 });
    [784, 988, 1175, 1568].forEach((f, i) => tone({ f, type: 'square', dur: 0.12, vol: 0.18, delay: 0.5 + i * 0.05 }));
  },
  boot() {
    [220, 330, 440, 660].forEach((f, i) => tone({ f, type: 'square', dur: 0.05, vol: 0.12, delay: i * 0.08 }));
    tone({ f: 880, f2: 1760, type: 'sine', dur: 0.25, vol: 0.25, delay: 0.36 });
  },
};
