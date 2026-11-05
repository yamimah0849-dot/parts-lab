// Parts Lab compile checker: compiles every idea's sketch for every board it lists.
// Usage:
//   node tools/compile-all.mjs                 compile everything, write data/compile-report.json
//   node tools/compile-all.mjs --only a,b      compile only these idea ids (merges into the report)
//   node tools/compile-all.mjs --jobs 4        parallel compiles (default 3)
//   node tools/compile-all.mjs --install       first install the libraries in tools/libraries.txt
//   node tools/compile-all.mjs --no-report     don't write the report file
import { readFileSync, writeFileSync, existsSync, mkdtempSync } from 'node:fs';
import { execFile, execFileSync } from 'node:child_process';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const root = new URL('../', import.meta.url);
const rootPath = root.pathname;
const read = p => JSON.parse(readFileSync(new URL(p, root), 'utf8'));
const args = process.argv.slice(2);
const arg = (name, def) => { const i = args.indexOf(name); return i >= 0 ? args[i + 1] : def; };
const only = arg('--only', '')?.split(',').filter(Boolean) || [];
const jobs = Math.max(1, Number(arg('--jobs', 3)));
const writeReport = !args.includes('--no-report');

if (args.includes('--install')) {
  const libs = readFileSync(new URL('tools/libraries.txt', root), 'utf8').split('\n').map(l => l.trim()).filter(l => l && !l.startsWith('#'));
  for (const lib of libs) {
    try { execFileSync('arduino-cli', ['lib', 'install', lib], { stdio: 'pipe' }); console.log(`lib ✓ ${lib}`); }
    catch (e) { console.log(`lib ✗ ${lib}: ${String(e.stderr || e.message).split('\n')[0]}`); }
  }
}

const { categories } = read('data/categories.json');
const parts = categories.flatMap(c => read(`data/parts/${c.id}.json`));
const boards = new Map(parts.filter(p => p.category === 'boards').map(p => [p.id, p]));
const { files } = read('data/ideas/index.json');
let ideas = files.flatMap(f => read(`data/ideas/${f}`));
if (only.length) ideas = ideas.filter(i => only.includes(i.id));

const jobsList = [];
for (const idea of ideas) {
  const sketch = join(rootPath, 'sketches', idea.id);
  for (const b of idea.boards) {
    const fqbn = idea.fqbn?.[b] || boards.get(b)?.fqbn;
    jobsList.push({ idea: idea.id, board: b, fqbn, sketch });
  }
}

const buildRoot = mkdtempSync(join(tmpdir(), 'partslab-build-'));
const results = {};
const failures = [];
let done = 0;

function compile(job) {
  return new Promise(resolve => {
    if (!job.fqbn) return resolve({ ...job, ok: false, log: `no FQBN for board ${job.board}` });
    if (!existsSync(join(job.sketch, `${job.idea}.ino`))) return resolve({ ...job, ok: false, log: `missing sketches/${job.idea}/${job.idea}.ino` });
    const buildPath = join(buildRoot, `${job.idea}__${job.board}`);
    execFile('arduino-cli', ['compile', '--fqbn', job.fqbn, '--build-path', buildPath, '--warnings', 'default', job.sketch],
      { maxBuffer: 20 * 1024 * 1024 }, (err, stdout, stderr) => {
        resolve({ ...job, ok: !err, log: `${stdout}\n${stderr}` });
      });
  });
}

async function worker(queue) {
  while (queue.length) {
    const job = queue.shift();
    const r = await compile(job);
    done += 1;
    (results[r.idea] ||= {})[r.board] = r.ok ? 'ok' : 'fail';
    const size = r.log.match(/Sketch uses (\d+) bytes \((\d+)%\)/);
    console.log(`[${done}/${jobsList.length}] ${r.ok ? '✓' : '✗'} ${r.idea} @ ${r.board}${size ? `  (${size[2]}% flash)` : ''}`);
    if (!r.ok) failures.push(r);
  }
}

const queue = [...jobsList];
await Promise.all(Array.from({ length: Math.min(jobs, queue.length || 1) }, () => worker(queue)));

if (writeReport) {
  const reportPath = new URL('data/compile-report.json', root);
  let report = { results: {} };
  if (only.length && existsSync(reportPath)) report = JSON.parse(readFileSync(reportPath, 'utf8'));
  report.generated = new Date().toISOString();
  try { report.arduinoCli = execFileSync('arduino-cli', ['version', '--format', 'json'], { encoding: 'utf8' }).match(/"VersionString":\s*"([^"]+)"/)?.[1] || 'unknown'; } catch { report.arduinoCli = 'unknown'; }
  try {
    const cores = JSON.parse(execFileSync('arduino-cli', ['core', 'list', '--format', 'json'], { encoding: 'utf8' }));
    const list = cores.platforms || cores;
    report.cores = Object.fromEntries(list.map(c => [c.id, c.installed_version || c.installed]));
  } catch { /* ignore */ }
  if (!only.length) report.results = {};
  Object.assign(report.results, results);
  writeFileSync(reportPath, `${JSON.stringify(report, null, 1)}\n`);
  console.log('Report written to data/compile-report.json');
}

if (failures.length) {
  console.log(`\n${failures.length} compile failure(s):`);
  for (const f of failures) {
    console.log(`\n--- ${f.idea} @ ${f.board} (${f.fqbn}) ---`);
    console.log(f.log.split('\n').filter(l => /error|Error|undefined|not declared|No such file/.test(l)).slice(0, 12).join('\n') || f.log.slice(-1500));
  }
  process.exit(1);
}
console.log(`\n✓ All ${jobsList.length} compiles passed (${ideas.length} ideas).`);
