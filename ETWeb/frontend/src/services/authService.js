import axios from 'axios';

const registerEndpoint = `/api/auth/register/`;
const loginEndpoint = `/api/auth/login/`;

const tokenKey = 'token';
const expiryKey = 'expiryTime';
const defaultTokenExpiryMs = 1800000; // half an hour
const extendedTokenExpiryMs = 60480000; // one week

const headers = {
  'Content-Type': 'application/json',
};

export const registerUser = ({username,
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

export const loginUser = ({username, password}) => {
     return axios.post(
        loginEndpoint,
        {
            'username': username,
            'password': password,
        }, {
            headers: {
             ...headers,
             // 'Authorization': `token ${token}`
            }
         }
    );
};

export const logoutUser = () => {
    localStorage.removeItem(location.origin);
};

export const saveAuthToken = (token, remember=false) => {
    let expiryTime = Date.now() + (remember ? extendedTokenExpiryMs : defaultTokenExpiryMs);
    console.log(remember);
    console.log(expiryTime)
    localStorage.setItem(
        location.origin,
        JSON.stringify({
            [tokenKey]: token,
            [expiryKey]: expiryTime
        }));
};

export const getAuthToken = () => {
    const tokenInfo = JSON.parse(localStorage.getItem(location.origin));

    if (tokenInfo && Date.now() <= Number.parseInt(tokenInfo[expiryKey])) {
        return tokenInfo[tokenKey];
    }
    return null;
};

export const userLoggedIn = (user) => {
    return user.token !== null && user.token !== undefined;
};

export default {
    registerUser,
    loginUser,
    logoutUser,
    saveAuthToken,
    getAuthToken,
    userLoggedIn
}