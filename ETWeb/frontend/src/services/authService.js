import axios from 'axios';

const apiUrl = location.origin;
const apiEndpoint = 'api/auth';
const registerEndpoint = `${apiEndpoint}/register/`;

export const registerUser = ({username,
                              email,
                              password, confirm_password,
                              is_staff=false}) => {
    return axios.post(
        `${apiUrl}/${registerEndpoint}`,
        {
            'username': username,
            'email': email,
            'password': password,
            'confirm_password': confirm_password,
            'is_staff': is_staff
        },

    )
};