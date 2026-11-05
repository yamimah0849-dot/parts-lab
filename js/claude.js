// Parts Lab — copy-paste prompts for the user's regular Claude chat, and validation of
// ideas pasted back. No API is used: everything goes through the clipboard (100% free).
import { catalog } from './data.js';
import { BASICS, VIBES, requirementsOf } from './ideas.js';

const partLine = p => `- ${p.id}: ${p.name} (${p.voltage || 'voltage n/a'})${p.custom ? ' [my custom part]' : ''}`;

export function ideasPrompt(trayParts, basicsOn) {
  const boards = catalog.parts.filter(p => p.category === 'boards').map(p => p.id);
  const allIds = catalog.parts.map(p => p.id).join(', ');
  return `I'm using an app called "Parts Lab" that suggests electronics projects. Please invent 3–5 project ideas for me.

MY PARTS (use these exact ids):
${trayParts.map(partLine).join('\n') || '- (none picked)'}
${basicsOn ? `Basics I always have: ${BASICS.join(', ')} (resistor kit = 100Ω, 220Ω, 330Ω, 1kΩ, 2kΩ, 4.7kΩ, 10kΩ).` : 'I do NOT assume any basics (breadboard, wires, resistors, USB cable) unless listed above.'}

RULES
- I'm a beginner. Ideas should be beginner to intermediate and use mostly MY parts.
- Electrically safe: current-limiting resistor for every LED; never power motors, pumps or vibration motors from a board pin (use a transistor/MOSFET or motor driver plus a flyback diode, and say when external power is needed); flag any 3.3V vs 5V mismatch and solve it (voltage divider or level shifter).
- Wiring and code must match the EXACT part variants listed (e.g. LCD with vs without I2C).
- "boards" may only contain: ${boards.join(', ')}.
- "requires", "optional" and the guide's parts list may only use ids from this list: ${allIds}
- Code must be complete, commented Arduino code (C++) for the first board in "boards". Use only common Arduino Library Manager libraries and list them in "libs".
- If you are not sure something is correct, say so in the guide's "safety" notes instead of guessing.

ANSWER FORMAT: reply with ONLY a JSON array inside one \`\`\`json code block, no other text. Each idea:
{
  "id": "short-kebab-case-id",
  "title": "Project name",
  "pitch": "One-line pitch",
  "emoji": "🔦",
  "difficulty": 1,            // 1 = beginner, 2 = easy-intermediate, 3 = intermediate
  "time": 30,                 // build time in minutes
  "vibes": ["fun"],           // any of: ${VIBES.map(v => v.id).join(', ')}
  "boards": ["${boards[0]}"],
  "requires": ["part-id", ["alternative-a", "alternative-b"]],   // NOT the board, NOT basics
  "optional": [],
  "basics": ["breadboard", "jumper-wires", "resistor-kit", "usb-cable"],
  "libs": [],
  "guide": {
    "about": "What you'll build and learn (1–2 sentences)",
    "parts": [["part-id", 1, "note, e.g. which resistor value"]],
    "wiring": [["Part pin", "Board pin or other part pin", "optional note"]],
    "steps": ["Step 1 ...", "Step 2 ..."],
    "test": "How to check it works",
    "safety": ["Short safety notes where they matter"],
    "next": ["Ideas to extend it"]
  },
  "code": "the complete sketch as one JSON string"
}`;
}

export function stuckPrompt(idea, { stepIndex = 0, boardName = '', code = '' } = {}) {
  const g = idea.guide || {};
  const steps = g.steps || [];
  const parts = (g.parts || []).map(([id, qty, note]) => `- ${qty || 1}× ${catalog.byId.get(id)?.name || id}${note ? ` (${note})` : ''}`).join('\n');
  const wiring = (g.wiring || []).map(([a, b, n]) => `- ${a} → ${b}${n ? `  (${n})` : ''}`).join('\n');
  return `I'm building the project "${idea.title}" from the Parts Lab app and I'm stuck. I'm a beginner, so please explain simply.

Board: ${boardName || (idea.boards || []).map(b => catalog.byId.get(b)?.name || b).join(' or ')}
I'm on step ${stepIndex + 1} of ${steps.length}: "${steps[stepIndex] || 'n/a'}"

PARTS
${parts || '- (see project)'}

WIRING (from the guide)
${wiring || '- (none listed)'}

CODE I'M USING
\`\`\`cpp
${code || idea.code || '(not loaded)'}
\`\`\`

WHAT HAPPENS / WHAT I SEE:
[Describe your problem here: e.g. "nothing lights up", error message text, what the Serial Monitor shows]

Please help me find the problem step by step: check my wiring, power and the code, and tell me what to measure or try first.`;
}

// ---------- validation of ideas pasted back from Claude
function extractJSON(text) {
  const fence = text.match(/```(?:json)?\s*([\s\S]*?)```/i);
  const body = fence ? fence[1] : text;
  const start = Math.min(...['[', '{'].map(c => { const i = body.indexOf(c); return i < 0 ? Infinity : i; }));
  if (!isFinite(start)) throw new Error('No JSON found in the pasted text.');
  const endChar = body[start] === '[' ? ']' : '}';
  const end = body.lastIndexOf(endChar);
  return JSON.parse(body.slice(start, end + 1));
}

const str = (v, max) => (typeof v === 'string' ? v.trim().slice(0, max) : '');
const strArr = (v, max, n) => (Array.isArray(v) ? v.filter(x => typeof x === 'string').map(x => x.trim().slice(0, max)).filter(Boolean).slice(0, n) : []);

export function validateClaudeIdeas(text, existingIds = new Set()) {
  let raw;
  try { raw = extractJSON(text); } catch (e) { return { ideas: [], errors: [`Couldn't read the answer as JSON: ${e.message}`], warnings: [] }; }
  const list = Array.isArray(raw) ? raw : [raw];
  const ideas = [], errors = [], warnings = [];
  const isPart = id => catalog.byId.has(id);
  const isBoard = id => catalog.byId.get(id)?.category === 'boards';
  const used = new Set(existingIds);

  list.slice(0, 12).forEach((r, n) => {
    const label = `Idea ${n + 1}${r?.title ? ` ("${String(r.title).slice(0, 40)}")` : ''}`;
    if (!r || typeof r !== 'object') { errors.push(`${label}: not an object.`); return; }
    const problems = [];
    const title = str(r.title, 80);
    if (!title) problems.push('missing title');
    const boards = strArr(r.boards, 40, 6).filter(isBoard);
    if (!boards.length) problems.push('no valid board in "boards"');
    const requires = [];
    for (const q of Array.isArray(r.requires) ? r.requires.slice(0, 12) : []) {
      const alts = (Array.isArray(q) ? q : [q]).filter(x => typeof x === 'string');
      const bad = alts.filter(a => !isPart(a));
      if (bad.length) problems.push(`unknown part id(s) in "requires": ${bad.join(', ')}`);
      else if (alts.length) requires.push(alts.length === 1 ? alts[0] : alts);
    }
    const optional = strArr(r.optional, 40, 8);
    const badOpt = optional.filter(o => !isPart(o));
    if (badOpt.length) warnings.push(`${label}: ignored unknown optional part(s): ${badOpt.join(', ')}`);
    const g = r.guide && typeof r.guide === 'object' ? r.guide : {};
    const steps = strArr(g.steps, 600, 30);
    if (!steps.length) problems.push('the guide has no steps');
    const wiring = (Array.isArray(g.wiring) ? g.wiring : []).slice(0, 60).map(w => (Array.isArray(w)
      ? [str(w[0], 80), str(w[1], 80), str(w[2], 160)]
      : [str(w?.from, 80), str(w?.to, 80), str(w?.note, 160)])).filter(w => w[0] && w[1]);
    const gparts = (Array.isArray(g.parts) ? g.parts : []).slice(0, 30).map(p => (Array.isArray(p) ? [str(p[0], 40), Number(p[1]) || 1, str(p[2], 120)] : null))
      .filter(p => p && p[0]);
    const badGP = gparts.filter(p => !isPart(p[0])).map(p => p[0]);
    if (badGP.length) warnings.push(`${label}: guide lists parts not in the catalog (${badGP.join(', ')}); shown as plain text.`);
    const code = typeof r.code === 'string' ? r.code.slice(0, 40000) : '';
    if (!code) warnings.push(`${label}: no code included.`);
    if (problems.length) { errors.push(`${label}: ${problems.join('; ')}.`); return; }

    let id = `claude-${(str(r.id, 40) || title).toLowerCase().replace(/[^a-z0-9]+/g, '-').replace(/^-|-$/g, '') || 'idea'}`;
    while (used.has(id)) id += '-2';
    used.add(id);
    const idea = {
      id, title,
      pitch: str(r.pitch, 160) || title,
      emoji: str(r.emoji, 8) || '💡',
      difficulty: Math.min(3, Math.max(1, Math.round(Number(r.difficulty) || 1))),
      time: Math.min(600, Math.max(5, Math.round(Number(r.time) || 30))),
      vibes: strArr(r.vibes, 20, 4).filter(v => VIBES.some(x => x.id === v)),
      boards, requires,
      optional: optional.filter(isPart),
      basics: strArr(r.basics, 30, 4).filter(b => BASICS.includes(b)),
      libs: strArr(r.libs, 60, 10),
      guide: {
        about: str(g.about, 400), parts: gparts, wiring, steps,
        test: str(g.test, 400), safety: strArr(g.safety, 300, 10), next: strArr(g.next, 200, 8),
      },
      code,
      imported: Date.now(),
    };
    if (!requirementsOf(idea).length) { errors.push(`${label}: no requirements.`); return; }
    ideas.push(idea);
  });
  if (!ideas.length && !errors.length) errors.push('No ideas found in the pasted text.');
  return { ideas, errors, warnings };
}
