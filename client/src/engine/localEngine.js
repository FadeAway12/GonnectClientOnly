//this is essentially my cpp backend transposed into js.

const EMPTY = 0, WHITE = 1, BLACK = 2;


let size = 5;
let board = new Array(size * size).fill(EMPTY);
let prev = board.slice(); // for ko RULE

function ind(row, col) { return row * size + col; }
function hasStone(i) { return board[i] !== EMPTY; }
function getStone(i) { return board[i]; }

function neighbors(i) {
  const out = [];
  const r = Math.floor(i / size), c = i % size;
  if (r > 0) out.push(i - size);
  if (r < size - 1) out.push(i + size);
  if (c > 0) out.push(i - 1);
  if (c < size - 1) out.push(i + 1);
  return out;
}

function cloneState() {
  return { size, board: board.slice(), prev: prev.slice() };
}
function restoreState(s) {
  size = s.size;
  board = s.board.slice();
  prev = s.prev.slice();
}

function removeGroupMembers(groupStones) {
  for (const i of groupStones) board[i] = EMPTY;
}

function buildGroups() {
  const seen = new Set();
  const groups = []; 
  for (let i = 0; i < board.length; i++) {
    if (board[i] === EMPTY || seen.has(i)) continue;
    const color = board[i];
    const stack = [i];
    const stones = new Set([i]);
    seen.add(i);
    while (stack.length) {
      const cur = stack.pop();
      for (const n of neighbors(cur)) {
        if (!seen.has(n) && board[n] === color) {
          seen.add(n);
          stones.add(n);
          stack.push(n);
        }
      }
    }
    groups.push({ color, stones });
  }
  return groups;
}

function libertiesOf(stones) {
  const libs = new Set();
  for (const s of stones) {
    for (const n of neighbors(s)) if (!hasStone(n)) libs.add(n);
  }
  return libs;
}

function removeIsolated(colorToRemove) {
  const groups = buildGroups();
  for (const g of groups) {
    if (g.color !== colorToRemove) continue;
    if (libertiesOf(g.stones).size === 0) removeGroupMembers(g.stones);
  }
}

function violatesKo(row, col, color) {
  const snap = cloneState();
  _setColor(row, col, color);
  const same = board.every((v, i) => v === prev[i]);
  restoreState(snap);
  return same;
}

function isLegal(row, col, color) {
  if (row < 0 || col < 0 || row >= size || col >= size) return false;
  if (hasStone(ind(row, col))) return false;
  if (violatesKo(row, col, color)) return false;
  return true;
}

function _setColor(row, col, color) {
  prev = board.slice();
  const i = ind(row, col);
  if (hasStone(i)) return;
  board[i] = color;

  // captures (suicide IS allowed if makes sense)
  const opp = color === WHITE ? BLACK : WHITE;
  removeIsolated(opp);
  removeIsolated(color);
}

export function setColor(i, color) {
  const row = Math.floor(i / size), col = i % size;
  if (!isLegal(row, col, color)) return { status: 400, error: "Illegal move" };
  _setColor(row, col, color);
  return { status: 200 };
}

// api translated

export function getBoard() {
  const flat = board.slice();
  const blackCount = flat.filter(v => v === BLACK).length;
  const whiteCount = flat.filter(v => v === WHITE).length;
  const counter = blackCount + whiteCount;
  return Promise.resolve({
    board: Array.from({ length: size }, (_, r) =>
      Array.from({ length: size }, (_, c) => board[ind(r, c)])
    ),
    player: counter % 2 === 1 ? WHITE : BLACK, 
  });
}

export function setBoard(newArray) {
  const n = newArray.length;
  const root = Math.round(Math.sqrt(n));
  if (root * root !== n) throw new Error("Board must be square");
  size = root;
  prev = board.slice();
  board = newArray.slice();
  return Promise.resolve({ status: 200 });
}

function isConnectedColor(color) {
  const q = [];
  const vis = new Set();
  for (let c = 0; c < size; c++) {
    const i = ind(0, c);
    if (board[i] === color) { q.push(i); vis.add(i); }
  }
  while (q.length) {
    const cur = q.shift();
    if (Math.floor(cur / size) === size - 1) return true;
    for (const n of neighbors(cur)) {
      if (!vis.has(n) && board[n] === color) { vis.add(n); q.push(n); }
    }
  }

  q.length = 0;
  vis.clear();
  for (let r = 0; r < size; r++) {
    const i = ind(r, 0);
    if (board[i] === color) { q.push(i); vis.add(i); }
  }
  while (q.length) {
    const cur = q.shift();
    if (cur % size === size - 1) return true;
    for (const n of neighbors(cur)) {
      if (!vis.has(n) && board[n] === color) { vis.add(n); q.push(n); }
    }
  }
  return false;
}

export function isConnected() {
  if (isConnectedColor(BLACK)) return Promise.resolve(BLACK);
  if (isConnectedColor(WHITE)) return Promise.resolve(WHITE);
  return Promise.resolve(0);
}

// EVALUATION FUNCTIONS TRANSLATED

function victoryStatus() {
  if (isConnectedColor(BLACK)) return -100000;
  if (isConnectedColor(WHITE)) return 100000;
  return 0;
}

function numLiberties(color) {
  const libs = new Set();
  for (let i = 0; i < board.length; i++) {
    if (getStone(i) !== color) continue;
    for (const n of neighbors(i)) if (!hasStone(n)) libs.add(n);
  }
  return libs.size;
}

function numAdjacent(color) {
  let cnt = 0;
  for (let i = 0; i < board.length; i++) {
    if (getStone(i) !== color) continue;
    for (const n of neighbors(i)) {
      if (!hasStone(n) || getStone(n) === color) cnt++;
    }
  }
  return cnt;
}

function largestGroup(color) {
  const groups = buildGroups();
  let sz = 0;
  for (const g of groups) {
    if (g.color === color) sz = Math.max(sz, g.stones.size);
  }
  return sz;
}

function pieceDiff() {
  let diff = 0;
  for (let i = 0; i < board.length; i++) {
    if (board[i] === WHITE) diff++;
    else if (board[i] === BLACK) diff--;
  }
  return diff;
}

function centerControl() {
  let diff = 0;
  const rStart = Math.floor(size / 4), rEnd = Math.ceil((3 * size) / 4);
  const cStart = Math.floor(size / 4), cEnd = Math.ceil((3 * size) / 4);
  for (let row = rStart; row < rEnd; row++) {
    for (let col = cStart; col < cEnd; col++) {
      const i = ind(row, col);
      let denom = Math.abs(size / 2 - row) + Math.abs(size / 2 - col);
      if (denom === 0) denom = 1e-6; 
      const w = (1.0 / denom) * 3;
      if (board[i] === WHITE) diff += w;
      else if (board[i] === BLACK) diff -= w;
    }
  }
  return diff;
}

function evaluate() {
  return (
    victoryStatus() +
    (numLiberties(WHITE) - numLiberties(BLACK)) * 3 +
    (numAdjacent(WHITE) - numAdjacent(BLACK)) +
    (largestGroup(WHITE) - largestGroup(BLACK)) * 2 +
    pieceDiff() * 2 +
    centerControl()
  );
}

function evaluateFor(color) {
  let score = evaluate();
  if (color === BLACK) score *= -1;
  return score;
}

function manhattanCenter(i) {
  const r = Math.floor(i / size), c = i % size;
  const cr = (size - 1) / 2, cc = (size - 1) / 2;
  return Math.abs(r - cr) + Math.abs(c - cc);
}
function orderedIndices() {
  return Array.from({ length: size * size }, (_, i) => i)
    .sort((a, b) => manhattanCenter(a) - manhattanCenter(b));
}

function wouldConnect(idx, color) {
  const snap = cloneState();
  _setColor(Math.floor(idx / size), idx % size, color);
  const ok = isConnectedColor(color);
  restoreState(snap);
  return ok;
}

export async function getBestMove(player) { // 1 = white, 2 = black. this method MIGHT have issues checklater. much less depth than cpp one 

  await sleep(1200);

  const opp = player === WHITE ? BLACK : WHITE;
  let bestI = -1, bestEval = -Infinity;

  for (const i of orderedIndices()) {
    if (hasStone(i)) continue;
    const r = Math.floor(i / size), c = i % size;
    if (!isLegal(r, c, player)) continue;
    if (wouldConnect(i, player)) return { bestMove: i };
  }

  for (const i of orderedIndices()) {
    if (hasStone(i)) continue;
    const row = Math.floor(i / size), col = i % size;
    if (!isLegal(row, col, player)) continue;

    const snap = cloneState();
    _setColor(row, col, player);

    let innerWorst = +Infinity;

    for (const j of orderedIndices()) {
      if (hasStone(j)) continue;
      const r2 = Math.floor(j / size), c2 = j % size;
      if (!isLegal(r2, c2, opp)) continue;

      if (wouldConnect(j, opp)) { innerWorst = -100000; break; }

      const snap2 = cloneState();
      _setColor(r2, c2, opp);
      const val = evaluateFor(player);
      if (val < innerWorst) innerWorst = val;
      restoreState(snap2);
    }

    if (!isFinite(innerWorst)) innerWorst = evaluateFor(player);

    if (
      innerWorst > bestEval ||
      (innerWorst === bestEval && manhattanCenter(i) < manhattanCenter(bestI))
    ) {
      bestEval = innerWorst;
      bestI = i;
    }

    restoreState(snap);
  }

  return { bestMove: bestI }; 
}

function sleep(ms) { // move happens fast with low depth
  return new Promise(resolve => setTimeout(resolve, ms));
}

export function reset(newSize) {
  size = newSize;
  board = new Array(size * size).fill(EMPTY);
  prev = board.slice();
}

export const CONST = { EMPTY, WHITE, BLACK };
