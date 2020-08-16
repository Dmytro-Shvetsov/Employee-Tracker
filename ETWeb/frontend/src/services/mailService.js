import axios from './configuredAxiosInstance';

const contactEndpoint = `/api/contact/`;

const contactOrganization = (data, cancelToken) => {
    return axios.post(contactEndpoint, data, {cancelToken});
};

export {
    contactOrganization
}