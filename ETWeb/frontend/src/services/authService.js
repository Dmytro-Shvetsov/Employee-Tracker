import axios from './configuredAxiosInstance';

const registerEndpoint = `/api/auth/register/`;
const accountConfirmEndpoint = `/api/auth/activate/`;
const loginEndpoint = `/api/auth/login/`;
const logoutEndpoint = `/api/auth/logout/`;
const accountEndpoint = `/api/auth/account/`;
const profileEndpoint = `/api/auth/profile/`;

const registerUser = ({username,
                      email,
                      password,
                      password2,
                      isStaff=false},
                      cancelToken) => {
    return axios.post(
        registerEndpoint,
        {
            'username': username,
            'email': email,
            'password': password,
            'password2': password2,
            'is_staff': isStaff
        },
        {cancelToken}
    );
};

const confirmEmail = (data, cancelToken) => {
    return axios.post(accountConfirmEndpoint, data, {cancelToken});
};

const loginUser = (data, cancelToken) => {
     return axios.post(loginEndpoint, {...data, 'include_acc_info': true}, {cancelToken});
};

const logoutUser = cancelToken => {
    return axios.post(logoutEndpoint, {cancelToken});
};

const userLoggedIn = user => {
    return user && user.token !== null && user.token !== undefined;
};

const getUserAccount = async (data, cancelToken) => {
    return await axios.post(accountEndpoint, data, {cancelToken});
};

const updateUserAccount = (data, cancelToken) => {
    return axios.put(accountEndpoint, data, {cancelToken});
};

const getUserProfile = (data, cancelToken) => {
    return axios.post(profileEndpoint, data, cancelToken);
};

const updateUserProfile = ({user, ...data}, cancelToken) => {
    const formData = new FormData();

    Object.keys(data).forEach(key => {
        formData.append(key, data[key]);
    });

    return axios.put(profileEndpoint, formData,
        {
            headers: {
                "Content-Type": "multipart/form-data; boundary=---WebKitFormBoundary7MA4YWxkTrZu0gW",
            },
            cancelToken
        }
    );
};

export {
    registerUser,
    confirmEmail,
    loginUser,
    logoutUser,
    getUserAccount,
    updateUserAccount,
    getUserProfile,
    updateUserProfile,
    userLoggedIn,
}