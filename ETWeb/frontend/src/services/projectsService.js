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

export {
    loadProjectList
}