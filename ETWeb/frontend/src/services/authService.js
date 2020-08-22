import axios from './configuredAxiosInstance';

const apiEndpoint = '/api/accounts/';

const registerEndpoint = apiEndpoint + 'auth/register/';
const accountConfirmEndpoint = apiEndpoint + 'auth/activate/';
const loginEndpoint = apiEndpoint + 'auth/login/';
const logoutEndpoint = apiEndpoint + 'auth/logout/';
const passwordResetEndpoint = apiEndpoint + 'auth/reset_password/';

const accountEndpoint = apiEndpoint + 'account/';
const profileEndpoint = apiEndpoint + 'profile/';

const activityLogsEndpoint = apiEndpoint + 'activity_logs/';
const screenshotLogsEndpoint = activityLogsEndpoint + 'screenshots/';
const domainLogsEndpoint = activityLogsEndpoint + 'domains/';

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

const getUserProfile = ({user_id}, cancelToken) => {
    // if id is not undefined, the call is made by staff user
    const endpoint = user_id === undefined ? profileEndpoint : profileEndpoint + `${user_id}/`;
    return axios.get(endpoint, {cancelToken});
};

const updateUserProfile = (data, cancelToken) => {
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

const getUserScreenshotActivityLogs = (data, cancelToken) => {
    return axios.get(screenshotLogsEndpoint, {params:data, cancelToken});
};

const getUserDomainActivityLogs = (data, cancelToken) => {
    return axios.get(domainLogsEndpoint, {params:data, cancelToken});
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
    getUserScreenshotActivityLogs,
    getUserDomainActivityLogs,
}