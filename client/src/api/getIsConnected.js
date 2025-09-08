export async function isConnected() {
  const url = new URL("http://localhost:18080/isConnected");

  const res = await fetch(url);
  if (!res.ok) throw new Error("err fetching");

  const data = await res.json(); 
  return typeof data === "number" ? data : Number(data.isConnected);
}
