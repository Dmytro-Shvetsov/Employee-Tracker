import axios from 'axios';

const registerEndpoint = `/api/auth/register/`;
const loginEndpoint = `/api/auth/login/`;
const accountEndpoint = `/api/auth/account/`;
const profileEndpoint = `/api/auth/profile/`;

const tokenKey = 'token';
const expiryKey = 'expiryTime';
const defaultTokenExpiryMs = 1800000; // half an hour
const extendedTokenExpiryMs = 60480000; // one week

const headers = {
  'Content-Type': 'application/json',
};

const registerUser = ({username,
                              email,
                              password, password2,
                              isStaff=false}) => {
    return axios.post(
        registerEndpoint,
        {
            'username': username,
            'email': email,
            'password': password,
            'password2': password2,
            'is_staff': isStaff
        },
    );
};

const loginUser = ({username, password}) => {
     return axios.post(
        loginEndpoint,
        {
            'username': username,
            'password': password,
            'include_acc_info': true
        }, {
            headers: {
             ...headers,
            }
         }
    );
};

const logoutUser = () => {
    localStorage.removeItem(location.origin);
};

const saveAuthToken = (token, remember=false) => {
    let expiryTime = Date.now() + (remember ? extendedTokenExpiryMs : defaultTokenExpiryMs);

    localStorage.setItem(
        location.origin,
        JSON.stringify({
            [tokenKey]: token,
            [expiryKey]: expiryTime
        })
    );
};

const getAuthToken = () => {
    const tokenInfo = JSON.parse(localStorage.getItem(location.origin));

    if (tokenInfo && Date.now() <= Number.parseInt(tokenInfo[expiryKey])) {
        return tokenInfo[tokenKey];
    }
    return null;
};

const userLoggedIn = user => {
    return user && user.token !== null && user.token !== undefined;
};

const getUserAccount = (authToken) => {
    return axios.post(
        accountEndpoint,
        {},
        {
            headers: {
                ...headers,
                "Authorization": `${tokenKey} ${authToken}`
            }
        }
    );
};

const updateUserAccount = ({token}, data) => {
    return axios.put(
        accountEndpoint,
        data,
        {
            headers: {
                ...headers,
                "Authorization": `${tokenKey} ${token}`
            }
        }
    );
};

const getUserProfile = ({token}) => {
    return axios.post(
        profileEndpoint,
        {},
        {
            headers: {
                ...headers,
                "Authorization": `${tokenKey} ${token}`
            }
        }
    );
};

const updateUserProfile = ({token}, data) => {
    const formData = new FormData();
    Object.keys(data).forEach(key => {
        formData.append(key, data[key]);
    });

    return axios.put(
        profileEndpoint,
        formData,
        {
            headers: {
                ...headers,
                "Content-Type": "multipart/form-data; boundary=---WebKitFormBoundary7MA4YWxkTrZu0gW",
                "Authorization": `${tokenKey} ${token}`
            }
        }
    );
};

export {
    tokenKey,
    registerUser,
    loginUser,
    logoutUser,
    saveAuthToken,
    getAuthToken,
    getUserAccount,
    updateUserAccount,
    getUserProfile,
    updateUserProfile,
    userLoggedIn,
}