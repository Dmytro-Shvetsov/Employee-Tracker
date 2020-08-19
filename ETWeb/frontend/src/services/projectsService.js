import axios from './configuredAxiosInstance';

const apiEndpoint = '/api/projects/';

const searchAccountsEndpoint = '/api/accounts/search/';
const manageMembersEndpoint = apiEndpoint + 'manage_members/';
const projectsInvitationsEndpoint = apiEndpoint + 'invite_accept/';


const loadProjectList = (data, cancelToken, page=1) => {
    return axios.get(apiEndpoint, {params: {page: page}, cancelToken});
};

const getProject = (id, cancelToken) => {
    return axios.get(apiEndpoint + `${id}/`, {cancelToken});
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
    if (username === "") {
        return Promise.resolve({data: JSON.stringify({users:[]})});
    }
    return axios.get(searchAccountsEndpoint + `${username}/`, {cancelToken})
};

const addMembersToProject = ({id, ...data}, cancelToken) => {
    return axios.put(manageMembersEndpoint + `${id}/`, data, {cancelToken})
};

const acceptProjectInvitation = (invitationToken, cancelToken) => {
    return axios.post(projectsInvitationsEndpoint, {token:invitationToken}, {cancelToken});
};

const deleteMembersFromProject = ({id, ...data}, cancelToken) => {
    return axios.delete(manageMembersEndpoint + `${id}/`, {params:data, cancelToken})
};

export {
    loadProjectList,
    createNewProject,
    getProject,
    updateProject,
    deleteProject,
    findMembersByUsername,
    addMembersToProject,
    acceptProjectInvitation,
    deleteMembersFromProject
}