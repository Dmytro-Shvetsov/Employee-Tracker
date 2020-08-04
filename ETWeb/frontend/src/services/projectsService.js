import axios from 'axios';
import { tokenKey } from './authService'

const apiEndpoint = `/api/projects/`;

const headers = {
  'Content-Type': 'application/json',
};

const loadProjectList = ({token}, page=1) => {
    return axios.get(
        apiEndpoint,
        {
            headers: {
                ...headers,
                "Authorization": `${tokenKey} ${token}`
            },
            params: {
                page: page
            }
        }
    );
};

const getProject = ({token}, id) => {
    console.log(id);
    return axios.get(
        `${apiEndpoint}/${id}/`,
        {
            headers: {
                ...headers,
                "Authorization": `${tokenKey} ${token}`
            },
        }
    );
};



const updateProject = ({token}, id, data) => {
    return axios.put(
        `${apiEndpoint}/${id}/`,
        data,
        {
            headers: {
                ...headers,
                "Authorization": `${tokenKey} ${token}`
            }
    });
};

const deleteProject = ({token}, id) => {
    return axios.delete(
        `${apiEndpoint}/${id}/`,
        {
            headers: {
                ...headers,
                "Authorization": `${tokenKey} ${token}`
            }
    });
};


const createNewProject = ({token}, data) => {
    return axios.post(
        apiEndpoint,
        data,
        {
            headers: {
                ...headers,
                "Authorization": `${tokenKey} ${token}`
            },
        }
    );
};

export {
    loadProjectList,
    createNewProject,
    getProject,
    updateProject,
    deleteProject
}