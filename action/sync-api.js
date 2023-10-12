
const endpoint = 'https://dev-sync-use.duckduckgo.com/sync/';

// Payload requires: user_id, hashed_password, protected_encryption_key, device_id, device_name, device_type
const signup = async (payload) => {
  const res = await fetch(new URL('signup', endpoint), {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(payload)
  });
  if (!res.ok) {
    const text = await res.text();
    throw new Error(`Call to ${res.url} failed with ${res.status} ${res.statusText}: ${text}`);
  }

  return await res.json();
};

module.exports = {
  signup
};
