// Parts Lab — hand-drawn SVG icon library (original artwork, 48x48 grid).
// Classes inside icons: .f = soft body fill, .a = accent fill (uses --ac), .s = solid currentColor.
const pins = (x1, x2, y) => `<path d="M${x1} ${y}H${x2}" stroke-dasharray="1.6 2.4" stroke-width="2.4"/>`;
const legs = (...xs) => xs.map(([x, y1, y2]) => `<path d="M${x} ${y1}V${y2}"/>`).join('');

const I = {
  // ---------- boards ----------
  'board-uno': `<rect x="7" y="11" width="37" height="26" rx="3" class="f"/><rect x="2.5" y="14" width="9" height="8" rx="1.5"/><rect x="4" y="27" width="6" height="6" rx="3"/><rect x="21" y="21" width="15" height="6" rx="1" class="a"/>${pins(15, 41, 15)}${pins(19, 40, 33)}`,
  'board-nano': `<rect x="5" y="17" width="39" height="14" rx="2" class="f"/><rect x="2" y="20" width="6" height="8" rx="1.5"/><rect x="20" y="20.5" width="9" height="7" rx="1" class="a" transform="rotate(45 24.5 24)"/>${pins(10, 42, 14)}${pins(10, 42, 34)}`,
  'board-mega': `<rect x="3" y="12" width="42" height="24" rx="3" class="f"/><rect x="1" y="15" width="7" height="7" rx="1.5"/><rect x="18" y="19" width="10" height="10" rx="1" class="a"/><rect x="31" y="21" width="8" height="5" rx="1"/>${pins(11, 43, 16)}${pins(11, 43, 32)}<path d="M41 18v12" stroke-dasharray="1.6 2.4" stroke-width="2.4"/>`,
  'board-esp32': `<rect x="10" y="6" width="28" height="36" rx="3" class="f"/><rect x="15" y="12" width="18" height="14" rx="1.5" class="a" opacity=".55"/><path d="M15 9.5h3v-2h3v2h3v-2h3v2h3v-2h3"/><rect x="20" y="37" width="8" height="5" rx="1"/>${legs([13, 29, 34], [35, 29, 34])}`,
  'board-mini': `<rect x="14" y="10" width="20" height="30" rx="3" class="f"/><rect x="19" y="6" width="10" height="6" rx="2.5"/><rect x="19" y="20" width="10" height="10" rx="1" class="a"/><path d="M17 35h3M28 35h3"/>`,
  'board-cam': `<rect x="10" y="6" width="28" height="36" rx="3" class="f"/><circle cx="24" cy="20" r="8"/><circle cx="24" cy="20" r="3.5" class="a"/><rect x="16" y="33" width="16" height="5" rx="1"/>`,
  'board-pico': `<rect x="12" y="5" width="24" height="39" rx="2" class="f"/><rect x="20" y="2" width="8" height="6" rx="1.5"/><rect x="19" y="21" width="10" height="10" rx="1" class="a" transform="rotate(45 24 26)"/><path d="M14.5 10v31M33.5 10v31" stroke-dasharray="1.6 2.4" stroke-width="2.4"/>`,
  // ---------- sensors ----------
  'dht': `<rect x="13" y="5" width="22" height="27" rx="2.5" class="f"/><path d="M17 11h14M17 16h14M17 21h14M17 26h14" opacity=".7"/><rect x="21" y="14" width="6" height="6" rx="1" class="a" opacity=".6"/>${legs([19, 32, 43], [24, 32, 43], [29, 32, 43])}`,
  'probe': `<path d="M5 7c11 0 15 5 15 15"/><rect x="16" y="20" width="8" height="22" rx="4" class="a" opacity=".55"/><path d="M16 26h8"/>`,
  'ultrasonic': `<rect x="3" y="14" width="42" height="20" rx="2" class="f"/><circle cx="14" cy="24" r="7.5"/><circle cx="34" cy="24" r="7.5"/><circle cx="14" cy="24" r="3" class="a"/><circle cx="34" cy="24" r="3" class="a"/>${legs([20, 34, 41], [23, 34, 41], [26, 34, 41], [29, 34, 41])}`,
  'pir': `<path d="M9 30a15 15 0 0 1 30 0z" class="f"/><path d="M14 29a10 10 0 0 1 20 0M19 29a5 5 0 0 1 10 0" opacity=".7"/><rect x="7" y="30" width="34" height="7" rx="1.5"/><circle cx="24" cy="18" r="2" class="a"/>`,
  'ldr': `<circle cx="24" cy="19" r="11" class="f"/><path d="M16 17h3.5v5h3.5v-6h3.5v6h3.5v-5h2" class="sq"/>${legs([20, 30, 43], [28, 30, 43])}<path d="M37 6l-4 4M42 12l-5 2" class="ac"/>`,
  'soil': `<path d="M15 5h18v25l-9 13-9-13z" class="f"/><rect x="19" y="8" width="10" height="9" rx="1" class="a" opacity=".6"/><path d="M19 24h10" opacity=".6"/>`,
  'imu': `<rect x="14" y="14" width="20" height="20" rx="2" class="f"/><path d="M24 24V5M24 24h19M24 24L10 38" class="ac"/><path d="M21 8l3-3 3 3M40 21l3 3-3 3M10 33v5h5" class="ac"/><circle cx="24" cy="24" r="2.5" class="a"/>`,
  'baro': `<circle cx="24" cy="26" r="15" class="f"/><path d="M24 26l7-8" class="ac"/><circle cx="24" cy="26" r="2.5" class="a"/><path d="M13 30l2-1M14 19l2 1.5M24 13.5v2.5M34 19l-2 1.5M35 30l-2-1"/>`,
  'mic': `<rect x="18" y="5" width="12" height="21" rx="6" class="f"/><path d="M21 12h6M21 17h6" opacity=".6"/><path d="M12 22a12 12 0 0 0 24 0"/><path d="M24 34v8M17 42h14"/><circle cx="38" cy="10" r="2" class="a"/>`,
  'gas': `<circle cx="24" cy="29" r="13" class="f"/><path d="M15 25h18M14 29h20M15 33h18M20 18v22M24 16v26M28 18v22" opacity=".5"/><path d="M17 11c0-3 3-3 3-6M24 11c0-3 3-3 3-6M31 11c0-3 3-3 3-6" class="ac"/>`,
  'ir-eye': `<rect x="5" y="22" width="38" height="16" rx="2" class="f"/><circle cx="16" cy="18" r="6" class="a" opacity=".7"/><circle cx="32" cy="18" r="6"/><path d="M12 8c2-2 6-2 8 0M28 8c2-2 6-2 8 0" class="ac"/>`,
  'flame': `<path d="M24 5c3 8 12 12 12 23a12 12 0 0 1-24 0c0-6 3-9 5-13 2 3 3 6 3 9 3-5 5-11 4-19z" class="a" opacity=".55"/><path d="M24 5c3 8 12 12 12 23a12 12 0 0 1-24 0c0-6 3-9 5-13 2 3 3 6 3 9 3-5 5-11 4-19z"/>`,
  'water': `<path d="M24 5c6 10 13 16 13 24a13 13 0 0 1-26 0c0-8 7-14 13-24z" class="f"/><path d="M14 30h20M15 35h18" class="ac"/>`,
  // ---------- displays ----------
  'lcd': `<rect x="3" y="11" width="42" height="26" rx="2" class="f"/><rect x="7" y="16" width="34" height="16" rx="1" class="a" opacity=".35"/><path d="M10 21h6M18 21h4M24 21h8M10 27h10M22 27h6" class="ac" stroke-width="2.6"/>`,
  'oled': `<rect x="8" y="8" width="32" height="34" rx="2" class="f"/><rect x="11" y="15" width="26" height="20" rx="1" class="s" opacity=".85"/><path d="M19 21h2v2h6v-2h2v2h2v4h-2v2h-2v-2h-6v2h-2v-2h-2v-4h2z" class="a" stroke="none"/>${pins(16, 33, 11)}`,
  'oled-wide': `<rect x="3" y="15" width="42" height="18" rx="2" class="f"/><rect x="6" y="19" width="31" height="10" rx="1" class="s" opacity=".85"/><path d="M9 24h12" class="ac" stroke-width="2.6"/><path d="M40 19v10" stroke-dasharray="1.6 2.4" stroke-width="2.4"/>`,
  'seg7': `<rect x="3" y="13" width="42" height="22" rx="2" class="f"/><path d="M8 18h5v12H8zM8 24h5M16 18h5v12h-5zM16 24h5M27 18h5v12h-5zM27 24h5M35 18h5v12h-5zM35 24h5" class="ac"/><circle cx="24" cy="21" r="1" class="a"/><circle cx="24" cy="27" r="1" class="a"/>`,
  'matrix': `<rect x="7" y="7" width="34" height="34" rx="2" class="f"/>${[12, 20, 28, 36].map(y => [12, 20, 28, 36].map(x => `<circle cx="${x}" cy="${y}" r="2.6" class="${(x + y) % 16 === 0 || (x === 20 && y === 28) ? 'a' : 's'}" opacity="${(x + y) % 16 === 0 || (x === 20 && y === 28) ? 1 : .25}"/>`).join('')).join('')}`,
  'tft': `<rect x="5" y="8" width="38" height="32" rx="2" class="f"/><rect x="8" y="11" width="32" height="24" rx="1" class="s" opacity=".85"/><path d="M8 32l9-10 7 7 5-5 11 10z" class="a" stroke="none"/><circle cx="32" cy="17" r="3" class="a"/>`,
  'seg7-one': `<rect x="13" y="5" width="22" height="38" rx="2" class="f"/><path d="M19 11h10M18 13v9M30 13v9M19 24h10M18 26v9M30 26v9M19 37h10" class="ac" stroke-width="2.8"/><circle cx="33" cy="38" r="1.3" class="a"/>`,
  // ---------- motion ----------
  'servo': `<rect x="8" y="19" width="30" height="17" rx="2" class="f"/><rect x="3" y="24" width="5" height="7" rx="1"/><rect x="38" y="24" width="5" height="7" rx="1"/><circle cx="17" cy="19" r="4.5"/><path d="M17 19L38 9" stroke-width="3.5" class="ac"/><path d="M24 36v7"/>`,
  'servo-big': `<rect x="7" y="17" width="34" height="21" rx="2" class="f"/><rect x="2" y="22" width="5" height="9" rx="1"/><rect x="41" y="22" width="5" height="9" rx="1"/><circle cx="16" cy="17" r="6"/><circle cx="16" cy="17" r="2" class="a"/><path d="M8 7l16 20" stroke-width="3.5" class="ac" opacity=".85"/>`,
  'stepper': `<circle cx="24" cy="27" r="14" class="f"/><path d="M5 15h38" stroke-width="3"/><circle cx="24" cy="27" r="4" class="a"/><path d="M24 27l6-6"/><path d="M14 41v4M19 42v3M24 42v3M29 42v3M34 41v4" class="ac"/>`,
  'driver': `<rect x="6" y="9" width="36" height="30" rx="2" class="f"/><rect x="16" y="17" width="16" height="14" rx="1" class="a" opacity=".6"/>${pins(18, 31, 14)}${pins(18, 31, 35)}<circle cx="11" cy="15" r="1.6" class="a"/><circle cx="11" cy="21" r="1.6" class="a"/>`,
  'driver-big': `<rect x="5" y="16" width="38" height="26" rx="2" class="f"/><path d="M14 4v14M19 4v14M24 4v14M29 4v14M34 4v14" stroke-width="2.6" class="ac"/><rect x="10" y="30" width="7" height="6" rx="1"/><rect x="31" y="30" width="7" height="6" rx="1"/>`,
  'dc-motor': `<rect x="6" y="13" width="28" height="22" rx="4" class="f"/><path d="M34 24h10" stroke-width="3" class="ac"/><path d="M34 18v12" /><rect x="10" y="9" width="4" height="4" rx="1"/><rect x="10" y="35" width="4" height="4" rx="1"/><path d="M16 19v10M22 19v10" opacity=".5"/>`,
  'vibe': `<circle cx="24" cy="24" r="11" class="f"/><circle cx="24" cy="24" r="4" class="a"/><path d="M7 17c-2 2-2 5 0 7s2 5 0 7M41 17c2 2 2 5 0 7s-2 5 0 7" class="ac"/>`,
  'pump': `<rect x="14" y="16" width="20" height="25" rx="4" class="f"/><path d="M24 16V7h13"/><path d="M40 9c2 3 2 5 0 7M43 6c3 4 3 9 0 13" class="ac"/><path d="M18 33h12M18 28h12" opacity=".5"/>`,
  // ---------- audio ----------
  'buzzer': `<circle cx="22" cy="24" r="14" class="f"/><circle cx="22" cy="24" r="3" class="a"/><path d="M38 17c3 4 3 10 0 14M42 13c5 6 5 16 0 22" class="ac"/>`,
  'speaker': `<circle cx="24" cy="24" r="17" class="f"/><circle cx="24" cy="24" r="10"/><circle cx="24" cy="24" r="4" class="a"/>`,
  'amp': `<rect x="4" y="10" width="40" height="28" rx="2" class="f"/><path d="M12 17v14l12-7z" class="ac"/><circle cx="34" cy="24" r="5"/><path d="M34 24l3-3"/>`,
  'mp3': `<rect x="6" y="6" width="36" height="36" rx="3" class="f"/><path d="M20 32V15l12-3v17" class="ac" stroke-width="2.6"/><circle cx="17" cy="32" r="3.5" class="a"/><circle cx="29" cy="29" r="3.5" class="a"/>`,
  // ---------- lighting ----------
  'led': `<path d="M16 28V15a8 8 0 0 1 16 0v13z" class="a" opacity=".55"/><path d="M16 28V15a8 8 0 0 1 16 0v13z"/><rect x="13.5" y="28" width="21" height="3" rx="1"/><path d="M20 31v13M28 31v9"/><path d="M8 8l3 3M40 8l-3 3M24 2v3" class="ac"/>`,
  'rgb-led': `<path d="M15 27V15a9 9 0 0 1 18 0v12z" class="f"/><circle cx="20" cy="16" r="3" fill="#ff3b6b" stroke="none"/><circle cx="28" cy="16" r="3" fill="#3bff8a" stroke="none"/><circle cx="24" cy="22" r="3" fill="#3b8bff" stroke="none"/><rect x="12.5" y="27" width="23" height="3" rx="1"/><path d="M17 30v12M22 30v14M27 30v12M31 30v10"/>`,
  'strip': `<rect x="2" y="17" width="44" height="14" rx="2" class="f"/><rect x="6" y="21" width="6" height="6" rx="1" fill="#ff3b6b" stroke="none"/><rect x="17" y="21" width="6" height="6" rx="1" fill="#ffe14f" stroke="none"/><rect x="28" y="21" width="6" height="6" rx="1" fill="#3bff8a" stroke="none"/><rect x="39" y="21" width="5" height="6" rx="1" fill="#3b8bff" stroke="none"/>`,
  'stick': `<rect x="4" y="18" width="40" height="12" rx="2" class="f"/>${['#ff3b6b', '#ff8a3d', '#ffe14f', '#3bff8a', '#36f1cd', '#3b8bff', '#8b6cff', '#ff4fa3'].map((c, i) => `<circle cx="${8.5 + i * 4.4}" cy="24" r="1.8" fill="${c}" stroke="none"/>`).join('')}`,
  'ring': `<circle cx="24" cy="24" r="16" class="f"/><circle cx="24" cy="24" r="9"/>${['#ff3b6b', '#ff8a3d', '#ffe14f', '#3bff8a', '#36f1cd', '#3b8bff', '#8b6cff', '#ff4fa3'].map((c, i) => { const a = i * Math.PI / 4; return `<circle cx="${(24 + 12.5 * Math.cos(a)).toFixed(1)}" cy="${(24 + 12.5 * Math.sin(a)).toFixed(1)}" r="2.2" fill="${c}" stroke="none"/>`; }).join('')}`,
  // ---------- input ----------
  'button': `<rect x="11" y="11" width="26" height="26" rx="3" class="f"/><circle cx="24" cy="24" r="7.5" class="a" opacity=".75"/><circle cx="24" cy="24" r="7.5"/><path d="M14 11V6M34 11V6M14 37v5M34 37v5"/>`,
  'arcade': `<circle cx="24" cy="24" r="18" class="f"/><circle cx="24" cy="24" r="12" class="a" opacity=".8"/><circle cx="24" cy="24" r="12"/><path d="M18 19a8 8 0 0 1 8-3" stroke="#fff" opacity=".7"/>`,
  'pot': `<circle cx="24" cy="20" r="13" class="f"/><circle cx="24" cy="20" r="6.5" class="a" opacity=".6"/><path d="M24 20l-6-6" stroke-width="2.6"/>${legs([16, 33, 44], [24, 33, 44], [32, 33, 44])}`,
  'encoder': `<rect x="8" y="8" width="32" height="32" rx="3" class="f"/><circle cx="24" cy="24" r="10" stroke-dasharray="2 2.2"/><circle cx="24" cy="24" r="6" class="a" opacity=".7"/><path d="M24 24v-5"/>`,
  'joystick': `<rect x="7" y="7" width="34" height="34" rx="5" class="f"/><circle cx="24" cy="24" r="11"/><circle cx="27" cy="21" r="6.5" class="a"/><path d="M24 3v4M24 41v4M3 24h4M41 24h4" class="ac"/>`,
  'keypad': `<rect x="9" y="3" width="30" height="36" rx="3" class="f"/>${[9, 16, 23, 30].map(y => [13, 20, 27, 34].map(x => `<rect x="${x - 2.5}" y="${y - 2.5 + 0}" width="5" height="5" rx="1" class="${x === 34 ? 'a' : 's'}" opacity="${x === 34 ? .9 : .35}" stroke="none"/>`).join('')).join('')}<path d="M18 39v6M22 39v6M26 39v6M30 39v6"/>`,
  'touch': `<rect x="9" y="9" width="30" height="30" rx="15" class="f"/><path d="M18 30c0-6 1-12 6-12s6 6 6 12M21 30c0-4 0-8 3-8s3 4 3 8M24 30v-4" class="ac"/>`,
  'switch': `<rect x="8" y="17" width="32" height="14" rx="2" class="f"/><rect x="11" y="19.5" width="12" height="9" rx="1.5" class="a"/>${legs([16, 31, 41], [24, 31, 41], [32, 31, 41])}`,
  // ---------- comms ----------
  'bluetooth': `<rect x="10" y="4" width="28" height="40" rx="10" class="f"/><path d="M17 16l14 13-7 7V12l7 6-14 13" class="ac" stroke-width="2.6"/>`,
  'radio': `<rect x="8" y="22" width="26" height="20" rx="2" class="f"/><rect x="14" y="28" width="8" height="8" rx="1" class="a" opacity=".6"/><path d="M30 22V8"/><path d="M36 10a8 8 0 0 0-12 0M40 6a14 14 0 0 0-20 0" class="ac"/>`,
  'ir-remote': `<rect x="15" y="9" width="18" height="36" rx="4" class="f"/><circle cx="24" cy="18" r="3" class="a"/><circle cx="19.5" cy="27" r="1.5" class="s"/><circle cx="24" cy="27" r="1.5" class="s"/><circle cx="28.5" cy="27" r="1.5" class="s"/><circle cx="19.5" cy="33" r="1.5" class="s"/><circle cx="24" cy="33" r="1.5" class="s"/><circle cx="28.5" cy="33" r="1.5" class="s"/><path d="M19 5a7 7 0 0 1 10 0" class="ac"/>`,
  'rfid': `<rect x="4" y="12" width="30" height="22" rx="3" class="f"/><rect x="9" y="18" width="8" height="6" rx="1" class="a"/><path d="M38 17a8 8 0 0 1 0 12M42 13a14 14 0 0 1 0 20" class="ac"/>`,
  'sd': `<path d="M13 5h16l7 7v31H13z" class="f"/><path d="M18 5v7M22 5v7M26 5v7" class="ac"/><rect x="17" y="26" width="15" height="10" rx="1" class="a" opacity=".35"/>`,
  'rtc': `<circle cx="24" cy="24" r="17" class="f"/><path d="M24 13v11l7 4" class="ac" stroke-width="2.6"/><circle cx="24" cy="24" r="2" class="a"/><path d="M24 8v2M40 24h-2M24 40v-2M8 24h2"/>`,
  // ---------- power ----------
  'battery-9v': `<rect x="12" y="11" width="24" height="33" rx="3" class="f"/><path d="M17 11V6h5v5M27 11V6h5v5"/><rect x="12" y="11" width="24" height="10" rx="3" class="a" opacity=".5"/><path d="M20 30h8M24 26v8" class="ac"/>`,
  'battery-aa': `<rect x="7" y="9" width="15" height="33" rx="5" class="f"/><rect x="26" y="9" width="15" height="33" rx="5" class="f"/><path d="M12 6h5M31 6h5"/><rect x="7" y="9" width="15" height="9" rx="4" class="a" opacity=".5"/><rect x="26" y="33" width="15" height="9" rx="4" class="a" opacity=".5"/>`,
  'plug': `<rect x="11" y="13" width="26" height="22" rx="4" class="f"/><path d="M17 13V6M31 13V6M24 9V4" stroke-width="3"/><path d="M24 35v9" /><path d="M21 21l-3 6h6l-3 6" class="ac"/>`,
  'regulator': `<rect x="4" y="10" width="40" height="28" rx="2" class="f"/><circle cx="17" cy="24" r="7"/><path d="M13 24c1-3 2-3 3 0s2 3 3 0" class="ac"/><rect x="30" y="16" width="8" height="5" rx="1" class="a"/><path d="M30 30h8"/>`,
  'charger': `<rect x="10" y="10" width="28" height="32" rx="4" class="f"/><path d="M19 10V6h10v4"/><path d="M26 16l-6 10h8l-6 10" class="ac" stroke-width="2.6"/>`,
  'relay': `<rect x="9" y="9" width="30" height="26" rx="2" class="f"/><path d="M15 22c2-4 4-4 6 0s4 4 6 0 4-4 6 0" class="ac"/><path d="M15 35v6M24 35v6M33 35v6"/><path d="M42 6l-3 3M44 13h-4" class="ac"/>`,
  'battery-pack': `<rect x="6" y="11" width="36" height="26" rx="5" class="f"/><rect x="11" y="18" width="5" height="12" rx="1" class="a"/><rect x="18" y="18" width="5" height="12" rx="1" class="a"/><rect x="25" y="18" width="5" height="12" rx="1" class="a" opacity=".4"/><path d="M42 21v6" stroke-width="3"/>`,
  // ---------- components ----------
  'breadboard': `<rect x="3" y="8" width="42" height="32" rx="2" class="f"/><path d="M8 13h32M8 17h32M8 31h32M8 35h32" stroke-dasharray="1.6 2.2" stroke-width="2"/><path d="M5 24h38" class="ac"/>`,
  'jumper': `<path d="M6 34c8-20 16 10 24-10 3-7 8-10 12-10" class="ac" stroke-width="2.6"/><path d="M6 40c10-16 18 4 26-10 3-5 6-8 10-8" stroke-width="2.6"/><rect x="3" y="33" width="6" height="9" rx="1" class="a"/><rect x="40" y="10" width="6" height="7" rx="1"/>`,
  'resistor': `<path d="M2 24h9M37 24h9"/><rect x="11" y="17" width="26" height="14" rx="6" class="f"/><path d="M17 17v14" stroke="#ff3b4e" stroke-width="2.6"/><path d="M22 17v14" stroke="#ff3b4e" stroke-width="2.6"/><path d="M27 17v14" stroke="#8a5a2b" stroke-width="2.6"/><path d="M32 17v14" stroke="#ffcf4a" stroke-width="2.6"/>`,
  'usb': `<path d="M24 42V8M19 12l5-6 5 6M24 30l-9-6v-6M24 26l9-6v-4" class="ac"/><circle cx="15" cy="16" r="2.5" class="a"/><rect x="31" y="12" width="4" height="4" class="a"/><circle cx="24" cy="42" r="3" class="s"/>`,
  'cap-ceramic': `<circle cx="24" cy="18" r="10" class="a" opacity=".6"/><circle cx="24" cy="18" r="10"/>${legs([20, 27, 44], [28, 27, 44])}`,
  'cap-electrolytic': `<rect x="15" y="4" width="18" height="30" rx="3" class="f"/><rect x="27" y="4" width="6" height="30" rx="2" class="a" opacity=".7"/><path d="M29 15h2" stroke="#000"/>${legs([20, 34, 44], [28, 34, 40])}`,
  'diode': `<path d="M2 24h9M37 24h9"/><rect x="11" y="18" width="26" height="12" rx="2" class="f"/><rect x="30" y="18" width="5" height="12" class="a"/>`,
  'transistor': `<path d="M15 30V15a9 9 0 0 1 18 0v15z" class="f"/><path d="M15 21h18" opacity=".5"/>${legs([18, 30, 44], [24, 30, 44], [30, 30, 44])}<path d="M24 12v4" class="ac"/>`,
  'mosfet': `<rect x="13" y="3" width="22" height="12" rx="1"/><circle cx="24" cy="9" r="2.5"/><rect x="13" y="15" width="22" height="16" rx="1" class="f"/><rect x="17" y="19" width="14" height="4" rx="1" class="a" opacity=".6"/>${legs([18, 31, 44], [24, 31, 44], [30, 31, 44])}`,
  'shifter': `<rect x="9" y="8" width="30" height="32" rx="2" class="f"/><path d="M14 20h20M14 28h20" class="ac"/><path d="M17 17l-3 3 3 3M31 25l3 3-3 3" class="ac"/>${pins(13, 36, 12)}${pins(13, 36, 36)}`,
  // ---------- tools ----------
  'iron': `<path d="M8 40l9-9" stroke-width="2"/><path d="M16 32l4-4 17-17a3 3 0 0 1 4 4L24 32l-4 4z" class="f"/><path d="M14 34l-8 8" class="ac" stroke-width="3"/><path d="M33 15l4 4" class="ac"/><path d="M6 42l2-1" class="ac" stroke-width="3"/>`,
  'multimeter': `<rect x="11" y="4" width="26" height="40" rx="4" class="f"/><rect x="15" y="8" width="18" height="10" rx="1" class="a" opacity=".55"/><circle cx="24" cy="28" r="6"/><path d="M24 28l3-4"/><circle cx="18" cy="39" r="1.8" class="s"/><circle cx="30" cy="39" r="1.8" class="a"/>`,
  'stripper': `<path d="M8 40l14-16M40 40L26 24" stroke-width="3.2"/><path d="M22 24l-4-14a6 6 0 0 1 12 0l-4 14" class="f"/><circle cx="24" cy="24" r="2.5" class="a"/><path d="M19 13h10" class="ac"/>`,
  'glue': `<path d="M8 14h26l6 6v6H16l-2 16H8l2-16H8z" class="f"/><path d="M40 23h6" class="ac" stroke-width="3"/><path d="M2 20h6" stroke-width="3"/><rect x="14" y="17" width="10" height="4" rx="1" class="a" opacity=".6"/>`,
  // ---------- categories ----------
  'cat-boards': `<rect x="12" y="12" width="24" height="24" rx="3" class="f"/><rect x="18" y="18" width="12" height="12" rx="1" class="a" opacity=".7"/><path d="M17 12V6M24 12V6M31 12V6M17 36v6M24 36v6M31 36v6M12 17H6M12 24H6M12 31H6M36 17h6M36 24h6M36 31h6"/>`,
  'cat-sensors': `<circle cx="24" cy="24" r="4" class="a"/><path d="M15 15a13 13 0 0 0 0 18M33 15a13 13 0 0 1 0 18M9 9a21 21 0 0 0 0 30M39 9a21 21 0 0 1 0 30"/>`,
  'cat-displays': `<rect x="4" y="8" width="40" height="26" rx="3" class="f"/><path d="M10 28l8-9 6 6 5-4 9 7" class="ac"/><path d="M18 42h12M24 34v8"/>`,
  'cat-motion': `<circle cx="24" cy="24" r="6" class="a" opacity=".7"/><path d="M24 4l3 6 6-2 1 6 6 1-2 6 6 3-6 3 2 6-6 1-1 6-6-2-3 6-3-6-6 2-1-6-6-1 2-6-6-3 6-3-2-6 6-1 1-6 6 2z" class="f"/>`,
  'cat-audio': `<path d="M18 36V10l20-4v26" stroke-width="2.6"/><circle cx="13" cy="36" r="5" class="a"/><circle cx="33" cy="32" r="5" class="a"/>`,
  'cat-lighting': `<path d="M17 30c-4-3-6-7-6-11a13 13 0 0 1 26 0c0 4-2 8-6 11v5H17z" class="a" opacity=".5"/><path d="M17 30c-4-3-6-7-6-11a13 13 0 0 1 26 0c0 4-2 8-6 11v5H17z"/><path d="M19 40h10M21 44h6"/>`,
  'cat-input': `<path d="M14 14h20a10 10 0 0 1 10 10v2a8 8 0 0 1-14 5l-2-3h-8l-2 3A8 8 0 0 1 4 26v-2a10 10 0 0 1 10-10z" class="f"/><path d="M13 19v8M9 23h8"/><circle cx="33" cy="20" r="2" class="a"/><circle cx="37" cy="25" r="2" class="a"/>`,
  'cat-comms': `<circle cx="24" cy="28" r="3.5" class="a"/><path d="M24 31v13M16 20a11 11 0 0 1 16 0M11 15a18 18 0 0 1 26 0M6 10a25 25 0 0 1 36 0"/>`,
  'cat-power': `<path d="M27 3L10 27h12l-3 18 18-25H25z" class="a" opacity=".6"/><path d="M27 3L10 27h12l-3 18 18-25H25z"/>`,
  'cat-components': `<path d="M2 24h8l3-7 5 14 5-14 5 14 5-14 3 7h8" class="ac" stroke-width="2.6"/>`,
  'cat-tools': `<path d="M30 6a10 10 0 0 0-9 13L6 34a4 4 0 0 0 6 6l15-15a10 10 0 0 0 13-9l-6 3-5-5z" class="f"/>`,
  // ---------- UI ----------
  'search': `<circle cx="21" cy="21" r="12"/><path d="M30 30l11 11" stroke-width="3"/>`,
  'close': `<path d="M12 12l24 24M36 12L12 36" stroke-width="3"/>`,
  'plus': `<path d="M24 10v28M10 24h28" stroke-width="3"/>`,
  'minus': `<path d="M10 24h28" stroke-width="3"/>`,
  'trash': `<path d="M10 14h28M19 14V9h10v5M14 14l2 26h16l2-26M21 20v14M27 20v14"/>`,
  'gear': `<circle cx="24" cy="24" r="6"/><path d="M24 4v6M24 38v6M4 24h6M38 24h6M10 10l4 4M34 34l4 4M10 38l4-4M34 14l4-4"/><circle cx="24" cy="24" r="13"/>`,
  'sound-on': `<path d="M8 19h7l10-8v26l-10-8H8z" class="f"/><path d="M31 18a8 8 0 0 1 0 12M36 13a15 15 0 0 1 0 22"/>`,
  'sound-off': `<path d="M8 19h7l10-8v26l-10-8H8z" class="f"/><path d="M32 19l10 10M42 19L32 29"/>`,
  'back': `<path d="M30 10L16 24l14 14" stroke-width="3.2"/>`,
  'info': `<circle cx="24" cy="24" r="18"/><path d="M24 22v12" stroke-width="3"/><circle cx="24" cy="15" r="2" class="s"/>`,
  'warn': `<path d="M24 5l21 37H3z" class="f"/><path d="M24 18v11" stroke-width="3"/><circle cx="24" cy="35" r="2" class="s"/>`,
  'bolt': `<path d="M27 3L10 27h12l-3 18 18-25H25z"/>`,
  'flask': `<path d="M18 4h12M20 4v13L8 39a4 4 0 0 0 4 5h24a4 4 0 0 0 4-5L28 17V4"/><path d="M12 33h24l3 6a2 2 0 0 1-2 3H11a2 2 0 0 1-2-3z" class="a" opacity=".7"/><circle cx="20" cy="27" r="2" class="a"/><circle cx="27" cy="22" r="1.5" class="a"/>`,
  'home': `<path d="M6 22L24 7l18 15M11 19v21h26V19"/><rect x="20" y="28" width="8" height="12" class="a" opacity=".6"/>`,
  'grid': `<rect x="7" y="7" width="14" height="14" rx="2"/><rect x="27" y="7" width="14" height="14" rx="2"/><rect x="7" y="27" width="14" height="14" rx="2"/><rect x="27" y="27" width="14" height="14" rx="2" class="a" opacity=".6"/>`,
  'sparkle': `<path d="M24 4c2 11 9 18 20 20-11 2-18 9-20 20-2-11-9-18-20-20 11-2 18-9 20-20z" class="a" opacity=".7"/><path d="M24 4c2 11 9 18 20 20-11 2-18 9-20 20-2-11-9-18-20-20 11-2 18-9 20-20z"/>`,
  'check': `<path d="M9 25l10 10 20-22" stroke-width="3.4"/>`,
  'custom': `<path d="M24 4l18 10v20L24 44 6 34V14z" class="f"/><path d="M6 14l18 10 18-10M24 24v20" opacity=".5"/><path d="M20 15a4 4 0 1 1 6 3c-1.5 1-2 2-2 3" class="ac" stroke-width="2.6"/>`,
  'flip': `<path d="M8 18h28l-6-6M40 30H12l6 6"/>`,
};

export function icon(key, cls = '', style = '') {
  const body = I[key] || I.custom;
  return `<svg class="ico ${cls}" viewBox="0 0 48 48" aria-hidden="true"${style ? ` style="${style}"` : ''}>${body}</svg>`;
}

export const ICON_KEYS = Object.keys(I);
