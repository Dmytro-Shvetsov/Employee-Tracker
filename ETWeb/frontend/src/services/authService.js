import axios from './configuredAxiosInstance';

const registerEndpoint = '/api/accounts/auth/register/';
const accountConfirmEndpoint = '/api/accounts/auth/activate/';
const loginEndpoint = '/api/accounts/auth/login/';
const logoutEndpoint = '/api/accounts/auth/logout/';
const passwordResetEndpoint = '/api/accounts/auth/reset_password/';
const accountEndpoint = '/api/accounts/account/';
const profileEndpoint = '/api/accounts/profile/';

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
    return axios.post(logoutEndpoint, {}, {cancelToken});
};

const resetPassword = (data, cancelToken) => {
    return axios.post(passwordResetEndpoint, data, {cancelToken});
};

const validatePasswordResetToken = (data, cancelToken) => {
    return axios.post(passwordResetEndpoint + 'validate_token/', data, {cancelToken})
};

const confirmPasswordReset = (data, cancelToken) => {
    return axios.post(passwordResetEndpoint + 'confirm/', data,{cancelToken})
};

const userLoggedIn = user => {
    return user && user.token !== null && user.token !== undefined;
};

const getUserAccount = async cancelToken => {
    return await axios.get(accountEndpoint, {cancelToken});
};

const updateUserAccount = (data, cancelToken) => {
    return axios.put(accountEndpoint, data, {cancelToken});
};

const getUserProfile = (data, cancelToken) => {
    return axios.get(profileEndpoint, data, {cancelToken});
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
    resetPassword,
    validatePasswordResetToken,
    confirmPasswordReset,
    getUserAccount,
    updateUserAccount,
    getUserProfile,
    updateUserProfile,
    userLoggedIn,
}