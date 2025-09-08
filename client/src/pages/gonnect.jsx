import { useEffect, useState } from 'react'
import blackStone from "../assets/black.png";
import whiteStone from "../assets/white.jpg";

import {getBoard} from "../api/getBoard.js"
import {setBoard} from "../api/setBoard.js"
import {setColor} from "../api/setColor.js"
import { getBestMove } from '../api/getBestMove.js';
import { isConnected } from '../api/getIsConnected.js';

const BLACK = 2;
const WHITE = 1;


function isAITurn(aiCol, blackIsNext) {
  return (aiCol === BLACK && blackIsNext) || (aiCol === WHITE && !blackIsNext);
}

function Square({ value, onClick }) {
  let src = null;
  if (value === BLACK) src = blackStone;
  if (value === WHITE) src = whiteStone;

  return (
    <button className="square" onClick={onClick}>
      {src && (
        <img
          src={src}
          alt={value}
          style={{
            width: "80%",
            height: "80%",
            objectFit: "contain",
            pointerEvents: "none",
          }}
        />
      )}
    </button>
  );
}

function Board({ size, aiColor, setGameEnded }) {
  const [squares, setSquares] = useState(Array(size * size).fill(0));
  const [blackIsNext, setBlackIsNext] = useState(true);
  const [loading, setLoading] = useState(true);

useEffect(() => {
  getBoard()
    .then(data => {
      const flatBoard = data.board.flat();
      const parsedBoard = flatBoard.map(x => {
        if (x === 0) return 0;
        if (x === 1) return 1; 
        if (x === 2) return 2;
        return null;
      });
      getBoard().then(data => console.log("Board received:", data))
      setSquares(parsedBoard);
      setLoading(false);
    })
    .catch(err => {
      console.error('Failed to fetch board:', err);
      setLoading(false);
    });
}, []);

useEffect(() => {
  if (!loading && isAITurn(aiColor, blackIsNext)) {
    handleAI();
  }
}, [loading, blackIsNext, aiColor]);

useEffect(() => {
  let alive = true;
  (async () => {
    try {
      const winner = await isConnected();
      if (!alive) return;
      if (winner === BLACK) setGameEnded(1);
      else if (winner === WHITE) setGameEnded(2);
    } catch (err) {
      console.error("Failed to check game end:", err);
    }
  })();
  return () => { alive = false; };
}, [squares, blackIsNext]);


async function handleClick(i) {
  if (squares[i] || isAITurn(aiColor, blackIsNext)) return;
  const moveColor = blackIsNext ? BLACK : WHITE;

  try {
    await setColor(i, moveColor); 
    const data = await getBoard();  
    const parsed = data.board.flat().map(Number);
    setSquares(parsed);
    setBlackIsNext(prev => !prev);
  } catch (err) {
    console.error("failed to update board", err);
  }
}


async function handleAI() {
  if (!isAITurn(aiColor, blackIsNext)) return;

  try {
    const { bestMove: i } = await getBestMove(aiColor);
    await setColor(i, aiColor);   
    const data = await getBoard();
    const parsed = data.board.flat().map(Number);
    setSquares(parsed);
    setBlackIsNext(prev => !prev); // precevnt race cond
  } catch (err) {
    console.error("failed to update board", err);
  }
}


  const rows = [];
  for (let row = 0; row < size; row++) {
    const cols = [];
    for (let col = 0; col < size; col++) {
      const idx = row * size + col;
      cols.push(
        <Square key={idx} value={squares[idx]} onClick={() => handleClick(idx)} />
      );
    }
    rows.push(
      <div className="board-row" key={row}>
        {cols}
      </div>
    );
  }

  return (
    <div className="board">
      <h3>
        Next player: {blackIsNext ? "Black" : "White"}
        {isAITurn(aiColor, blackIsNext) ? " (...thinking)" : ""}
      </h3>
      {rows}
    </div>
  );
}

export default function Game() {
  const [gameStarted, setGameStarted] = useState(false);
  const [size, setSize] = useState(3);
  const [playerCol, setPlayerCol] = useState(false); //false is whiete, true is black
  const [gameEnded, setGameEnded] = useState(0); // 0 is ongoing, 1 is black win, 2 is white win

  if (!gameStarted) {
    return (
      <div style={{ padding: 20 }}>
        <h2>Welcome to Gonnect!</h2>
        <label>
          Board size (3-19):&nbsp;
          <input
            type="number"
            value={size}
            onChange={(e) => setSize(parseInt(e.target.value) || 3)}
            min={3}
            max={19}
          />
        </label>
        <br></br>
        <label>
          Be Black?&nbsp;
          <input
            type="checkbox"
            checked = {playerCol}
            onChange={(e) => setPlayerCol(!playerCol)}
          />
        </label>
        <br />
        <button onClick={() => {
          setGameStarted(true);
          setBoard(Array(size * size).fill(0));
        }}>Start Game</button>
      </div>
    );
  }

  if (gameEnded == BLACK) {
    return <p>BLACK WON</p>;
  }

  else if (gameEnded == WHITE) {
    return <p>WHITE WON</p>;
  }

  // if player is black, ai is white (2)
  return <div>
    <Board size={size} aiColor = {playerCol ? WHITE : BLACK} setGameEnded={setGameEnded}/> 
    <br></br>
    <button
      onClick={async () => {
      await setBoard(Array(size * size).fill(0));
      setGameStarted(false);       
    }}
    >
    Go Back
    </button>

  </div>
  
}
