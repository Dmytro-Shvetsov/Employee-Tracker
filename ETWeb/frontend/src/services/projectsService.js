import axios from './configuredAxiosInstance';

const apiEndpoint = `/api/projects/`;
const searchAccountsEndpoint = `/api/accounts/search/`;


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

const findMembersByUsername = (username, cancelToken) => {
    return axios.get(searchAccountsEndpoint + `${username}/`, {cancelToken})
};

const addMembersToProject = (data, cancelToken) => {
    return axios.post(apiEndpoint + 'add_members/', data, {cancelToken})
};

export {
    loadProjectList,
    createNewProject,
    getProject,
    updateProject,
    deleteProject,
    findMembersByUsername,
    addMembersToProject,
}