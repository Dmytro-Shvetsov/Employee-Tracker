import axios from 'axios';
import { tokenKey } from './authService'

const apiEndpoint = `/api/projects/`;

axios.interceptors.request.use(
    function (config) {
        // set authorization header before request is sent
        let { data } = config;
        if (config.headers['Authorization'] === undefined && data !== undefined) {
            config.headers['Authorization'] = `${tokenKey} ${data.user.token}`;
            delete data.user;
        }
        // console.log(config.headers)

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
                // window.location.replace("/login");
                break;
            }
            default: {
                console.log("Unexpected error occurred. ", error);
            }
        }
        return Promise.reject(error);
    });


const loadProjectList = (data, page=1) => {
    return axios.get(
        apiEndpoint,
        {
            headers:{"Authorization": `${tokenKey} ${data.user.token}`},
            params: {page: page}
        }
    );
};

const getProject = (id, data) => {
    return axios.get(`${apiEndpoint}/${id}/`, {headers:{"Authorization": `${tokenKey} ${data.user.token}`}});
};

const updateProject = (id, data) => {
    return axios.put(`${apiEndpoint}/${id}/`, data);
};

const deleteProject = (id, data) => {
    return axios.delete(`${apiEndpoint}/${id}/`, {headers:{"Authorization": `${tokenKey} ${data.user.token}`}});
};


const createNewProject = data => {
    return axios.post(apiEndpoint, data);
};

const source = {};
export {
    source,
    loadProjectList,
    createNewProject,
    getProject,
    updateProject,
    deleteProject
}