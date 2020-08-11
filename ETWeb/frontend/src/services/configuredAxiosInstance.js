import axios from 'axios';

function getCookie(cname) {
     const name = cname + "=";
     const ca = document.cookie.split(';');
     for(let i = 0; i < ca.length; i++) {
         let c = ca[i];
         while (c.charAt(0) === ' ')
             c = c.substring(1);
         if (c.indexOf(name) === 0)
             return c.substring(name.length, c.length);
     }
     return '';
}

const globalConfig = {
    headers: {
        'X-CSRFToken': getCookie('csrftoken'),
        'Content-Type': 'application/json',
    }
};

const instance = axios.create(globalConfig);
instance.isCancel = axios.isCancel.bind(instance);
instance.CancelToken = axios.CancelToken.bind(instance);

export default instance;