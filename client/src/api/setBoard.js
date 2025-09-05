export async function setBoard(array) {
  const res = await fetch('http://localhost:18080/postBoard', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },  
    body: JSON.stringify({ array }),
  })

  if (!res.ok) {
    const errorText = await res.text() // read as text to debug
    console.error('Server response:', errorText)
    throw new Error('Failed to post board')
  }

  return res.json()
}
