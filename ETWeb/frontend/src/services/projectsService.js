import axios from './configuredAxiosInstance';

const apiEndpoint = `/api/projects/`;

axios.interceptors.request.use(config => config, function (error) {
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

        // check if the request was canceled manually
        if (axios.isCancel(error)) {
            console.warn('Request canceled.');
        } else {
            switch (error.response.status) {
                case 401: {
                    window.location.replace("/login");
                    break;
                }
                default: {
                    console.log("Unexpected error occurred. ", error);
                }
            }
        }
        return Promise.reject(error);
    });


const loadProjectList = (data, cancelToken, page=1) => {
    return axios.get(apiEndpoint, {params: {page: page}, cancelToken});
};

const getProject = (id, data, cancelToken) => {
    return axios.get(`${apiEndpoint}/${id}/`, {cancelToken});
};

const updateProject = (id, data, cancelToken) => {
    return axios.put(`${apiEndpoint}/${id}/`, data, {cancelToken});
};

const deleteProject = (id, data, cancelToken) => {
    return axios.delete(`${apiEndpoint}/${id}/`, {cancelToken});
};

const createNewProject = (data, cancelToken) => {
    return axios.post(apiEndpoint, data, {cancelToken});
};

export {
    loadProjectList,
    createNewProject,
    getProject,
    updateProject,
    deleteProject
}