
const endpoint = 'https://dev-sync-use.duckduckgo.com/sync/';

const request = async (url, options) => {
  const res = await fetch(url, options);

  if (!res.ok) {
    const text = await res.text();
    throw new Error(`${options.method} ${res.url} => ${res.status} ${res.statusText}: ${text}`);
  }

  const json = await res.json();
  return {
    _status: res.status,
    _headers: Object.fromEntries(res.headers),
    ...json
  };
};

// Payload requires: user_id, hashed_password, protected_encryption_key, device_id, device_name, device_type
const signup = async (params) => {
  const body = (typeof params === 'string') ? params : JSON.stringify(params);
  return await request(new URL('signup', endpoint), {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body
  });
};

const patchData = async (jwt, data) => {
  const body = (typeof data === 'string') ? data : JSON.stringify(data);
  return await request(new URL('signup', endpoint), {
    method: 'PATCH',
    headers: {
      'Content-Type': 'application/json',
      'Authorization': `Bearer ${jwt}`,
    },
    body
  });
};

module.exports = {
  signup,
  patchData,
};
