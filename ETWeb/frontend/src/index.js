import ReactDOM from "react-dom";
import React from "react";
import App from './components/App';
import { BrowserRouter } from "react-router-dom";


ReactDOM.render((
        <BrowserRouter>
            <App />
        </BrowserRouter>
        ),
        document.getElementById('root')
    );