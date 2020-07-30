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
    console.log(remember);
    console.log(expiryTime);
    localStorage.setItem(
        location.origin,
        JSON.stringify({
            [tokenKey]: token,
            [expiryKey]: expiryTime
        }));
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

export {
    registerUser,
    loginUser,
    logoutUser,
    saveAuthToken,
    getAuthToken,
    userLoggedIn
}