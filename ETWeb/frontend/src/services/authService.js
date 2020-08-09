import axios from './axios';

// const { CancelToken } = axios;
const registerEndpoint = `/api/auth/register/`;

const loginEndpoint = `/api/auth/login/`;
const accountEndpoint = `/api/auth/account/`;
const profileEndpoint = `/api/auth/profile/`;
const tokenKey = 'token';

const expiryKey = 'expiryTime';
const defaultTokenExpiryMs = 1800000; // half an hour
const extendedTokenExpiryMs = 60480000; // one week

// const source = CancelToken.source();

axios.interceptors.request.use(
    function (config) {
        // set authorization header before request is sent

        // set authorization header before request is sent
        let { url, data } = config;
        if (config.headers['Authorization'] === undefined && data !== undefined && data.user !== undefined) {
            config.headers['Authorization'] = `${tokenKey} ${data.user.token}`;
            delete data.user;
        }
        // console.log(config);
        return config;
    }, function (error) {
        // check if the request was canceled manually
        if (axios.isCancel(error)) {
            console.warn('Request canceled.');
        }
        return Promise.reject(error);
    });

const resInterceptor = axios.interceptors.response.use(
    response => response,
    function (error) {
        // handle response errors
        switch (error.response.status) {
            case 401: {
                window.location.replace("/login");
                break;
            }
            default: {
                console.log("Unexpected error occurred. ", error);
            }
        }
        return Promise.reject(error);
    });


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

const loginUser = data => {
     return axios.post(
        loginEndpoint,
        {
            ...data,
            'include_acc_info': true
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

const getUserAccount = async data => {
    // perform request without request interceptor
    try {
        axios.interceptors.response.eject(resInterceptor);
        return await axios.post(accountEndpoint, data);
    } catch (error) {
        throw error;
    } finally {
        axios.interceptors.response.use(resInterceptor);
    }
};

const updateUserAccount = data => {
    return axios.put(accountEndpoint, data);
};

const getUserProfile = data => {
    return axios.post(profileEndpoint, data);
};

const updateUserProfile = ({user, ...data}) => {
    const formData = new FormData();
    Object.keys(data).forEach(key => {
        formData.append(key, data[key]);
    });

    return axios.put(
        profileEndpoint,
        formData,
        {
            headers: {
                "Content-Type": "multipart/form-data; boundary=---WebKitFormBoundary7MA4YWxkTrZu0gW",
                "Authorization": `${tokenKey} ${user.token}`,
            },
        }
    );
};

const source ={};
export {
    source,
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