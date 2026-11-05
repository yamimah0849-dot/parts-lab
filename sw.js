// Parts Lab service worker: makes the app installable and usable offline.
// Strategy: network first (always fresh when online), cached copy when offline.
const CACHE = 'partslab-v1';
const SHELL = [
  './',
  'index.html',
  'manifest.webmanifest',
  'css/style.css',
  'js/app.js', 'js/data.js', 'js/store.js', 'js/icons.js', 'js/mascot.js', 'js/sfx.js', 'js/checks.js',
  'data/categories.json',
  ...['boards', 'sensors', 'displays', 'motion', 'audio', 'lighting', 'input', 'comms', 'power', 'components', 'tools'].map(c => `data/parts/${c}.json`),
  'assets/fonts/orbitron-latin-700-normal.woff2',
  'assets/fonts/orbitron-latin-900-normal.woff2',
  'assets/fonts/share-tech-mono-latin-400-normal.woff2',
  'assets/icons/icon.svg',
  'assets/icons/icon-192.png',
  'assets/icons/apple-touch-icon.png',
];

self.addEventListener('install', e => {
  e.waitUntil(caches.open(CACHE).then(c => c.addAll(SHELL)).then(() => self.skipWaiting()));
});

self.addEventListener('activate', e => {
  e.waitUntil(
    caches.keys()
      .then(keys => Promise.all(keys.filter(k => k !== CACHE).map(k => caches.delete(k))))
      .then(() => self.clients.claim())
  );
});

self.addEventListener('fetch', e => {
  const req = e.request;
  if (req.method !== 'GET' || new URL(req.url).origin !== location.origin) return;
  e.respondWith(
    fetch(req)
      .then(res => {
        if (res.ok) { const copy = res.clone(); caches.open(CACHE).then(c => c.put(req, copy)); }
        return res;
      })
      .catch(() => caches.match(req, { ignoreSearch: true }).then(hit => hit || caches.match('index.html')))
  );
});
