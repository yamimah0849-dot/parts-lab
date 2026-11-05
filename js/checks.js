// Parts Lab — "Lab Check": voltage-mismatch and power warnings for the parts in the tray.
// Messages come from each part's data ("warn" and "power" fields), so they're easy to edit.

export function labCheck(parts) {
  const boards = parts.filter(p => p.category === 'boards');
  const levels = [...new Set(boards.map(b => b.logic).filter(Boolean))];
  const out = [];

  if (levels.includes(5) && levels.includes(3.3)) {
    out.push({ kind: 'info', msg: 'You picked both 5V and 3.3V boards. Checks are shown for both.' });
  }
  for (const p of parts) {
    if (p.warn) {
      for (const lvl of levels) {
        const m = p.warn[String(lvl)];
        if (m) out.push({ kind: 'volt', part: p, level: lvl, msg: m });
      }
    }
    if (p.power) out.push({ kind: 'power', part: p, msg: p.power });
  }
  return out;
}
