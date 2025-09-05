export async function getBoard() {
  const res = await fetch("http://localhost:18080/board");
  if (!res.ok) throw new Error('Failed to fetch board')
  return res.json()
}
