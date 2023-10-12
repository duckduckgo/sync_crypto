
const endpoint = 'https://dev-sync-use.duckduckgo.com/sync/';

const request = async (url, options) => {
  const res = await fetch(url, options);

  const text = await res.text();
  if (!res.ok) {
    throw new Error(`${options.method} ${res.url} => ${res.status} ${res.statusText}: ${text}`);
  }

  try {
    const json = text ? JSON.parse(text) : {};
    return {
      _status: res.status,
      _headers: Object.fromEntries(res.headers),
      ...json
    };
  } catch (err) {
    console.error('Failed to parse JSON from response.', text);
    throw new Error(`${options.method} ${res.url} => ${res.status} ${res.statusText} => Not valid JSON. ${text}`, err);
  }
};

// Payload requires: user_id, hashed_password, protected_encryption_key, device_id, device_name, device_type
const signup = async (params) => {
  const body = (typeof params === 'string') ? params : JSON.stringify(params);
  return await request(new URL('signup', endpoint), {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body
  });
};

const patchData = async (jwt, data) => {
  const body = (typeof data === 'string') ? data : JSON.stringify(data);
  return await request(new URL('data', endpoint), {
    method: 'PATCH',
    headers: {
      'Content-Type': 'application/json',
      'Authorization': `Bearer ${jwt}`,
    },
    body
  });
};

const deleteAccount = async (jwt) => {
  return await request(new URL('delete-account', endpoint), {
    method: 'POST',
    headers: {
      'Authorization': `Bearer ${jwt}`,
    }
  });
}

module.exports = {
  signup,
  patchData,
  deleteAccount,
};
