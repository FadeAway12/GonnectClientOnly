export async function setColor(ind, color) { // ind is index. color is 1 (white) 2 (black)
  const res = await fetch('http://localhost:18080/postColor', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },  
    body: JSON.stringify({ ind, color }),
  })

  if (!res.ok)
    {
    const errorText = await res.text() // read as text to debug
    console.error('Server response:', errorText)
    throw new Error('Failed to post board')
  }

  return res.json()
}
