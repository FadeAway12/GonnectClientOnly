import { useEffect, useState } from 'react'
import blackStone from "../assets/black.png";
import whiteStone from "../assets/white.jpg";

import {getBoard} from "../api/getBoard.js"
import {setBoard} from "../api/setBoard.js"
import {setColor} from "../api/setColor.js"
import { getBestMove } from '../api/getBestMove.js';

function Square({ value, onClick }) {
  let src = null;
  if (value === 1) src = blackStone;
  if (value === 2) src = whiteStone;

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

function Board({ size, aiColor }) {
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

async function handleClick(i) {
  if (squares[i]) return;

  const next = [...squares];
  next[i] = blackIsNext ? 1 : 2;

  setBlackIsNext(!blackIsNext);

  try {
    await setColor(i, next[i]); 
    const data = await getBoard(); 
    const flatBoard = data.board.flat();
    const parsedBoard = flatBoard.map(x => {
      if (x === 0) return 0;
      if (x === 1) return 1;
      if (x === 2) return 2;
      return null;
    });
    setSquares(parsedBoard); 
  } catch (err) {
    console.error("failed to update board", err);
  }
}

async function handleAI() {

  const i = await getBestMove(aiColor).bestMove;

  const next = [...squares];
  next[i] = blackIsNext ? 1 : 2;

  setBlackIsNext(!blackIsNext);

  try {
    await setColor(i, next[i]); 
    const data = await getBoard(); 
    const flatBoard = data.board.flat();
    const parsedBoard = flatBoard.map(x => {
      if (x === 0) return 0;
      if (x === 1) return 1;
      if (x === 2) return 2;
      return null;
    });
    setSquares(parsedBoard); 
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
      <h3>Next player: {blackIsNext ? "Black" : "White"}</h3>
      {rows}
    </div>
  );
}

export default function Game() {
  const [gameStarted, setGameStarted] = useState(false);
  const [size, setSize] = useState(3);
  const [playerCol, setPlayerCol] = useState(false); //false is black

  if (!gameStarted) {
    return (
      <div style={{ padding: 20 }}>
        <h2>Welcome to Gonnect!</h2>
        <label>
          Board size (2-19):&nbsp;
          <input
            type="number"
            value={size}
            onChange={(e) => setSize(parseInt(e.target.value) || 3)}
            min={2}
            max={19}
          />
        </label>
        <br></br>
        <label>
          Be Black?&nbsp;
          <input
            type="checkbox"
            checked = {!playerCol}
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

  return <div>
    <Board size={size} aiColor = {playerCol ? 1 : 2}/>
    <br></br>
    <button onClick = {() => {
      setGameStarted(false);
    }}>Go Back</button>
  </div>
  
}
