import axios from 'axios';
import { tokenKey } from './authService'

const apiEndpoint = `/api/projects/`;

const headers = {
  'Content-Type': 'application/json',
};

const loadProjectList = ({token}) => {
    console.log(token)
    return axios.get(
        apiEndpoint,
        {
            headers: {
                ...headers,
                "Authorization": `${tokenKey} ${token}`
            }
        }
    );
};

export {
    loadProjectList
}