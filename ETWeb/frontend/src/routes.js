import React from 'react';
import { Route } from 'react-router-dom';
import RegisterForm from './components/Auth/RegisterForm';
import LoginForm from './components/Auth/LoginForm';
import Logout from './components/Auth/Logout';
// import Dashboard from './components/Dashboard'

import Home from './components/Pages/Home'

const BaseRouter = (props) => {
    return (
        <React.Fragment>
            <Route exact path="/" component={Home}/>
            <Route exact path="/features" component={Home}/>
            {/*<Route exact path="/idea" component={Home}/>*/}
            {/*<Route exact path="/benefits" component={Home}/>*/}
            {/*<Route exact path="/contact" component={Home}/>*/}
            <Route exact path="/auth/register"
                   component={() => <RegisterForm onLogin={props.onLogin}/>}/>
            <Route exact path="/auth/login" component={() => <LoginForm onLogin={props.onLogin}/>}/>
            <Route exact path="/auth/logout" component={() => <Logout onLogout={props.onLogout}/>}/>
        </React.Fragment>
    );
};

export default BaseRouter;