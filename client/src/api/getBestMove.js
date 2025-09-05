export async function getBestMove(ind) {
    const string = "http://localhost:18080/bestMove?player=" + ind;
    const res = await fetch(string);
    if (!res.ok) throw new Error('Failed to fetch board')
    return res.json()
}
