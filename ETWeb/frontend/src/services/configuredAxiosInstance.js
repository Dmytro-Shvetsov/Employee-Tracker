import axios from 'axios';

const globalConfig = {
    headers: {
        'X-CSRFToken': document.querySelector('[name=csrfmiddlewaretoken]').value,
        'Content-Type': 'application/json',
    }
};

const instance = axios.create(globalConfig);
instance.isCancel = axios.isCancel.bind(instance);
instance.CancelToken = axios.CancelToken;


// response interceptor
instance.interceptors.request.use(config => config, function (error) {
        // check if the request was canceled manually
        if (axios.isCancel(error)) {
            console.warn('Request canceled.');
        }
        return Promise.reject(error);
    });

// request interceptor
instance.interceptors.response.use(
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
                    // console.log("Unexpected error occurred. ", error);
                }
            }
        }
        return Promise.reject(error);
    });

export default instance;